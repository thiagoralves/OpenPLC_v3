/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2003-2011  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2007-2011  Laurent Bessard and Edouard Tisserant
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * This code is made available on the understanding that it will not be
 * used in safety-critical situations without a full and competent review.
 */

/*
 * An IEC 61131-3 compiler.
 *
 * Based on the
 * FINAL DRAFT - IEC 61131-3, 2nd Ed. (2001-12-10)
 *
 */


/*
 * This file contains the code that calls the stage 1 (lexical anayser) 
 * and stage 2 (syntax parser) during the first pass.
 */

#include <string.h>
#include <stdlib.h>

/* file with declaration of absyntax classes... */
#include "../absyntax/absyntax.hh"


#include "../main.hh"
#include "stage1_2.hh"
#include "iec_bison.hh"
#include "stage1_2_priv.hh"
#include "create_enumtype_conversion_functions.hh"





/******************************************************/
/* whether we are supporting safe extensions          */
/* as defined in PLCopen - Technical Committee 5      */
/* Safety Software Technical Specification,           */
/* Part 1: Concepts and Function Blocks,              */
/* Version 1.0 – Official Release                   */
/******************************************************/
bool get_opt_safe_extensions() {return runtime_options.safe_extensions;}

/************************************/
/* whether to allow nested comments */
/************************************/
bool get_opt_nested_comments() {return runtime_options.nested_comments;}

/**************************************************************************/
/* whether to allow REF(), DREF(), REF_TO, NULL and ^ operators/keywords  */
/**************************************************************************/
bool get_opt_ref_standard_extensions() {return runtime_options.ref_standard_extensions;}


/**********************************************************************************************/
/* whether bison is doing the pre-parsing, where POU bodies and var declarations are ignored! */
/**********************************************************************************************/
static bool preparse_state__ = false;

void set_preparse_state(void) {preparse_state__ = true; }
void rst_preparse_state(void) {preparse_state__ = false;}
bool get_preparse_state(void) {return preparse_state__;}     // returns true if bison is in preparse state


/****************************************************/
/* Controlling the entry to the body_state in flex. */
/****************************************************/
static int goto_body_state__ = 0;

void cmd_goto_body_state(void) {goto_body_state__ = 1;}
int  get_goto_body_state(void) {return goto_body_state__;}
void rst_goto_body_state(void) {goto_body_state__ = 0;}

/*************************************************************/
/* Controlling the entry to the sfc_qualifier_state in flex. */
/*************************************************************/
static int goto_sfc_qualifier_state__ = 0;

void cmd_goto_sfc_qualifier_state(void) {goto_sfc_qualifier_state__ = 1;}
int  get_goto_sfc_qualifier_state(void) {return goto_sfc_qualifier_state__;}
void rst_goto_sfc_qualifier_state(void) {goto_sfc_qualifier_state__ = 0;}

/*************************************************************/
/* Controlling the entry to the sfc_priority_state in flex.  */
/*************************************************************/
static int goto_sfc_priority_state__ = 0;

void cmd_goto_sfc_priority_state(void) {goto_sfc_priority_state__ = 1;}
int  get_goto_sfc_priority_state(void) {return goto_sfc_priority_state__;}
void rst_goto_sfc_priority_state(void) {goto_sfc_priority_state__ = 0;}

/*************************************************************/
/* Controlling the entry to the sfc_qualifier_state in flex. */
/*************************************************************/
static int goto_task_init_state__ = 0;

void cmd_goto_task_init_state(void) {goto_task_init_state__ = 1;}
int  get_goto_task_init_state(void) {return goto_task_init_state__;}
void rst_goto_task_init_state(void) {goto_task_init_state__ = 0;}

/****************************************************************/
/* Returning to state in flex previously pushed onto the stack. */
/****************************************************************/
static int pop_state__ = 0;

void cmd_pop_state(void) {pop_state__ = 1;}
int  get_pop_state(void) {return pop_state__;}
void rst_pop_state(void) {pop_state__ = 0;}


/*********************************/
/* The global symbol tables...   */
/*********************************/
/* NOTE: only accessed indirectly by the lexical parser (flex)
 *       through the function get_identifier_token()
 */
/* A symbol table to store all the library elements */
/* e.g.: <function_name , function_decl>
 *       <fb_name , fb_decl>
 *       <type_name , type_decl>
 *       <program_name , program_decl>
 *       <configuration_name , configuration_decl>
 */
/* static */ library_element_symtable_t library_element_symtable;

/* A symbol table to store the declared variables of
 * the function currently being parsed...
 */
/* static */ variable_name_symtable_t   variable_name_symtable;

/* A symbol table to store the declared direct variables of
 * the function currently being parsed...
 */
/* static */ direct_variable_symtable_t direct_variable_symtable;

/* Function only called from within flex!
 *
 * search for a symbol in either of the two symbol tables
 * declared above, and return the token id of the first
 * symbol found.
 * Searches first in the variables, and only if not found
 * does it continue searching in the library elements
 */
int get_identifier_token(const char *identifier_str) {
//  std::cout << "get_identifier_token(" << identifier_str << "): \n";
  variable_name_symtable_t  ::iterator iter1;
  library_element_symtable_t::iterator iter2;

  if ((iter1 = variable_name_symtable.find(identifier_str)) != variable_name_symtable.end())
    return iter1->second;
    
  if ((iter2 = library_element_symtable.find(identifier_str)) != library_element_symtable.end())
    return iter2->second;
  
  return identifier_token;
}

/* Function only called from within flex!
 *
 * search for a symbol in direct variables symbol table
 * declared above, and return the token id of the first
 * symbol found.
 */
int get_direct_variable_token(const char *direct_variable_str) {
  direct_variable_symtable_t::iterator iter;

  if ((iter = direct_variable_symtable.find(direct_variable_str)) != direct_variable_symtable.end())
    return iter->second;

  return direct_variable_token;
}

/************************/
/* Utility Functions... */
/************************/

/*
 * Join two strings together. Allocate space with malloc(3).
 */
char *strdup2(const char *a, const char *b) {
  char *res = (char *)malloc(strlen(a) + strlen(b) + 1);

  if (!res)
    return NULL;
  return strcat(strcpy(res, a), b);  /* safe, actually */
}

/*
 * Join three strings together. Allocate space with malloc(3).
 */
char *strdup3(const char *a, const char *b, const char *c) {
  char *res = (char *)malloc(strlen(a) + strlen(b) + strlen(c) + 1);

  if (!res)
    return NULL;
  return strcat(strcat(strcpy(res, a), b), c);  /* safe, actually */
}



/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

int stage2__(const char *filename, 
             symbol_c **tree_root_ref
            );


int stage1_2(const char *filename, symbol_c **tree_root_ref) {
      /* NOTE: we only call stage2 (bison - syntax analysis) directly, as stage 2 will itself call stage1 (flex - lexical analysis)
       *       automatically as needed
       */

      /* NOTE: Since we do not call stage1__ (flex) directly, we cannot directly pass any parameters to that function either.
       *       In this case, we use callback functions, i.e. stage1__ (i.e. flex) will call functions defined in this file
       *       whenever it needs info/parameters coming from stage1_2().
       *       These callback functions will get their data from local (to this file) global variables...
       *       We now set those variables...
       */
  return stage2__(filename, tree_root_ref);
}

