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
 * The private interface to stage1_2.cc 
 */



/* !!! WARNING !!!
 *
 *       Whoever includes this file (stage1_2_priv.hh) will need
 *       to first inlcude iec_bison.hh !!
 *
 *       Read other comments further down to understand why we don't
 *       include iec_bison.hh in this file.
 */



/* file with the declarations of symbol tables... */
#include "../util/symtable.hh"
#include "stage1_2.hh"


/*
 * This file includes the interface through which the lexical parser (stage 1 - flex)
 * and the syntax analyser (stage 2 - bison) interact between themselves.
 *
 * This is mostly through direct access to shared global variables, however some
 * of the global variables will only be accessed through some accessor functions.
 *
 * This file also includes the interface between the main stage1_2() functions and 
 * the flex lexical parser.
 *
 * This file also includes some utility functions (strdupX() ) that are both used
 * in the lexical and syntax analysers.
 */




/*************************************************************/
/*************************************************************/
/****                                                    *****/
/****  I n t e r f a c e    B e t w e e n                *****/
/****           F l e x    a n d     s t a g e 1 _ 2 ()  *****/
/****                                                    *****/
/*************************************************************/
/*************************************************************/

/******************************************************/
/* whether we are suporting safe extensions           */
/* as defined in PLCopen - Technical Committee 5      */
/* Safety Software Technical Specification,           */
/* Part 1: Concepts and Function Blocks,              */
/* Version 1.0 – Official Release                     */
/******************************************************/
bool get_opt_safe_extensions();

/************************************/
/* whether to allow nested comments */
/************************************/
bool get_opt_nested_comments();

/**************************************************************************/
/* whether to allow REF(), DREF(), REF_TO, NULL and ^ operators/keywords  */
/**************************************************************************/
bool get_opt_ref_standard_extensions();



/*************************************************************/
/*************************************************************/
/****                                                    *****/
/****  I n t e r f a c e    B e t w e e n                *****/
/****           F l e x    a n d     B i s o n           *****/
/****                                                    *****/
/*************************************************************/
/*************************************************************/

/*********************************************/
/* print the include file stack to stderr... */
/*********************************************/
/* This is a service that flex provides to bison... */
void print_include_stack(void);

/*****************************************************/
/* Ask flex to include the source code in the string */
/*****************************************************/
/* This is a service that flex provides to bison... */
/* The string should contain valid IEC 61131-3 source code. Bison will ask flex to insert source 
 * code into the input stream of IEC code being parsed. The code to be inserted is typically
 * generated automatically.
 * Currently this is used to insert conversion functions ***_TO_*** (as defined by the standard)
 * between user defined (i.e. derived) enumerated datatypes, and some basic datatypes 
 * (e.g. INT, STRING, etc...)
 */
void include_string(const char *source_code);


/**********************************/
/* Tell flex which file to parse. */
/**********************************/
/* This is a service that flex provides to bison... */
/* Tell flex which file to parse. This function will not imediately start parsing the file.
 * To parse the file, you then need to call yyparse()
 *
 * Returns NULL on error opening the file (and a valid errno), or 0 on success.
 * Caller must close the file!
 */
FILE *parse_file(const char *filename);


/**********************************************************************************************/
/* whether bison is doing the pre-parsing, where POU bodies and var declarations are ignored! */
/**********************************************************************************************/
void set_preparse_state(void);
void rst_preparse_state(void);
bool get_preparse_state();  // returns true if bison is in preparse state

/****************************************************/
/* Controlling the entry to the body_state in flex. */
/****************************************************/
void cmd_goto_body_state(void);
int  get_goto_body_state(void);
void rst_goto_body_state(void);


/*************************************************************/
/* Controlling the entry to the sfc_qualifier_state in flex. */
/*************************************************************/
void cmd_goto_sfc_qualifier_state(void);
int  get_goto_sfc_qualifier_state(void);
void rst_goto_sfc_qualifier_state(void);


/*************************************************************/
/* Controlling the entry to the sfc_priority_state in flex.  */
/*************************************************************/
void cmd_goto_sfc_priority_state(void);
int  get_goto_sfc_priority_state(void);
void rst_goto_sfc_priority_state(void);


/*********************************************************/
/* Controlling the entry to the task_init_state in flex. */
/*********************************************************/
void cmd_goto_task_init_state(void);
int  get_goto_task_init_state(void);
void rst_goto_task_init_state(void);


/****************************************************************/
/* Returning to state in flex previously pushed onto the stack. */
/****************************************************************/
void cmd_pop_state(void);
int  get_pop_state(void);
void rst_pop_state(void);



/*********************************/
/* The global symbol tables...   */
/*********************************/
/* NOTE: only accessed indirectly by the lexical parser (flex)
 *       through the function get_identifier_token()
 *
 *       Bison accesses these data structures directly.
 *
 *       In essence, they are a data passing mechanism between Bison and Flex.
 */
/* A symbol table to store all the library elements */
/* e.g.: <function_name , function_decl>
 *       <fb_name , fb_decl>
 *       <type_name , type_decl>
 *       <program_name , program_decl>
 *       <configuration_name , configuration_decl>
 */
typedef symtable_c<int>             library_element_symtable_t;
extern  library_element_symtable_t  library_element_symtable;

/* A symbol table to store the declared variables of
 * the function currently being parsed...
 */
typedef symtable_c<int>             variable_name_symtable_t;
extern  variable_name_symtable_t    variable_name_symtable;

/* A symbol table to store the declared direct variables of
 * the function currently being parsed...
 */
typedef symtable_c<int>             direct_variable_symtable_t;
extern  direct_variable_symtable_t  direct_variable_symtable;

/* Function only called from within flex!
 *
 * search for a symbol in either of the two symbol tables
 * declared above, and return the token id of the first
 * symbol found.
 * Searches first in the variables, and only if not found
 * does it continue searching in the library elements
 */
int get_identifier_token(const char *identifier_str);

/* Function only called from within flex!
 *
 * search for a symbol in direct variables symbol table
 * declared above, and return the token id of the first
 * symbol found.
 */
int get_direct_variable_token(const char *direct_variable_str);


/*************************************************************/
/*************************************************************/
/****                                                    *****/
/****  U t i l i t y   F u n c t i o n s ...             *****/
/****                                                    *****/
/****                                                    *****/
/*************************************************************/
/*************************************************************/

/* Join two strings together. Allocate space with malloc(3). */
char *strdup2(const char *a, const char *b);

/* Join three strings together. Allocate space with malloc(3). */
char *strdup3(const char *a, const char *b, const char *c);



