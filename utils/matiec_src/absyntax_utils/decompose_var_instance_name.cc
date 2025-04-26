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


/* Decomposes a variable instance name into its constituents,
 * example:
 *    window.points[1].coordinate.x
 *
 *  will succesfully return
 *        - window
 *        - points
 *        - coordinate
 *        - x
 * on succesive calls to decompose_var_instance_name_c::next_part()
 */



#include "decompose_var_instance_name.hh"

decompose_var_instance_name_c::decompose_var_instance_name_c(symbol_c *variable_instance_name) {
  variable_name = variable_instance_name;
  next_variable_name = NULL;
  current_recursive_variable_name = NULL;
  previously_returned_variable_name = NULL;
  current_array_subscript_list = NULL;
}

/* Get the next element in the strcutured variable */
symbol_c *decompose_var_instance_name_c::get_next() {
  /* We must always start from the top!
   * See note in the structured_variable_c visitor
   * to understand why...
   */
  current_array_subscript_list = NULL;
  symbol_c *res = (symbol_c *)variable_name->accept(*this);
  next_variable_name = current_recursive_variable_name;

  if (previously_returned_variable_name == res)
    return NULL;
  
  previously_returned_variable_name = res;
  return res;
}

/* If the current element in the structured variable is an array, return its subscript_list, 
 * otherwise return NULL
 */
list_c *decompose_var_instance_name_c::get_current_arraysubs_list(void) {return current_array_subscript_list;}

/*************************/
/* B.1 - Common elements */
/*************************/
/*******************************************/
/* B 1.1 - Letters, digits and identifiers */
/*******************************************/
/* sometimes (e.g. FB calls) the name of the variable is stored directly in an identifier_c object */
void *decompose_var_instance_name_c::visit(identifier_c *symbol) {return (void *)symbol;}

/*********************/
/* B 1.4 - Variables */
/*********************/
void *decompose_var_instance_name_c::visit(symbolic_variable_c *symbol) {return (void *)(symbol->var_name);}

/********************************************/
/* B.1.4.1   Directly Represented Variables */
/********************************************/
void *decompose_var_instance_name_c::visit(direct_variable_c *symbol) {return (void *)symbol;}

/*************************************/
/* B.1.4.2   Multi-element Variables */
/*************************************/
/*  subscripted_variable '[' subscript_list ']' */
// SYM_REF2(array_variable_c, subscripted_variable, subscript_list)
void *decompose_var_instance_name_c::visit(array_variable_c *symbol) {
  if (NULL == symbol->subscript_list) ERROR; // array may not have an empty subscript list!
  current_array_subscript_list = dynamic_cast<list_c *>(symbol->subscript_list);
  if (NULL == current_array_subscript_list) ERROR; // if it does not point to a subscript_list_c, then the abstract syntax tree has been changed, and this code needs to be fixed accordingly!
  
  /* NOTE: the subscripted_variable may itself be a structure or an array!, so we must recursevily visit! */
  /* the next line will call either:
   *   - visit(structured_variable_c *) or visit(array_variable_c *), if the array variable is itself an element of another array os structure
   *   - visit(symbolic_variable_c *) if it is a simple array variable
   */
  return symbol->subscripted_variable->accept(*this);
}

/*  record_variable '.' field_selector */
/*  WARNING: input and/or output variables of function blocks
 *           may be accessed as fields of a tructured variable!
 *           Code handling a structured_variable_c must take
 *           this into account!
 */
//SYM_REF2(structured_variable_c, record_variable, field_selector)
void *decompose_var_instance_name_c::visit(structured_variable_c *symbol) {
  /* NOTE: The following code will not work, as structured_variable_c
   *       are grouped on the left, and not on the right!
   *
   *       example: window.origin.x
   *       will result in
   *       s1 = structured_variable_c("window, "origin");
   *       s2 = structured_variable_c(s1, "x");
   *       AND NOT
   *       s1 = structured_variable_c("origin", "x");
   *       s2 = structured_variable_c("window", s1);
   *
   *       as the following code assumes!!
   *
  current_variable_name = symbol->field_selector;
  return symbol->record_variable->accept(*this);
   */

  /* The correct code, is therefore more complex... */
  if (next_variable_name == symbol) {
    return (void *)symbol->field_selector->accept(*this);
  }

  current_array_subscript_list = NULL;
  current_recursive_variable_name = symbol;
  /* the next line will call either:
   *   - visit(structured_variable_c *) or visit(array_variable_c *), if the record variable has more elements to visit
   *   - visit(symbolic_variable_c *) if it is the last element in the record variable
   */
  return symbol->record_variable->accept(*this); 
}

/********************************/
/* B 2.2 - Operators */
/********************************/
void *decompose_var_instance_name_c::visit(LD_operator_c *symbol) {return (void *)&LD_operator_name;}
void *decompose_var_instance_name_c::visit(S_operator_c *symbol) {return (void *)&S_operator_name;}
void *decompose_var_instance_name_c::visit(R_operator_c *symbol) {return (void *)&R_operator_name;}
void *decompose_var_instance_name_c::visit(S1_operator_c *symbol) {return (void *)&S1_operator_name;}
void *decompose_var_instance_name_c::visit(R1_operator_c *symbol) {return (void *)&R1_operator_name;}
void *decompose_var_instance_name_c::visit(CLK_operator_c *symbol) {return (void *)&CLK_operator_name;}
void *decompose_var_instance_name_c::visit(CU_operator_c *symbol) {return (void *)&CU_operator_name;}
void *decompose_var_instance_name_c::visit(CD_operator_c *symbol) {return (void *)&CD_operator_name;}
void *decompose_var_instance_name_c::visit(PV_operator_c *symbol) {return (void *)&PV_operator_name;}
void *decompose_var_instance_name_c::visit(IN_operator_c *symbol) {return (void *)&IN_operator_name;}
void *decompose_var_instance_name_c::visit(PT_operator_c *symbol) {return (void *)&PT_operator_name;}

identifier_c     decompose_var_instance_name_c::LD_operator_name("LD");
identifier_c     decompose_var_instance_name_c::S_operator_name("S");
identifier_c     decompose_var_instance_name_c::R_operator_name("R");
identifier_c     decompose_var_instance_name_c::S1_operator_name("S1");
identifier_c     decompose_var_instance_name_c::R1_operator_name("R1");
identifier_c     decompose_var_instance_name_c::CLK_operator_name("CLK");
identifier_c     decompose_var_instance_name_c::CU_operator_name("CU");
identifier_c     decompose_var_instance_name_c::CD_operator_name("CD");
identifier_c     decompose_var_instance_name_c::PV_operator_name("PV");
identifier_c     decompose_var_instance_name_c::IN_operator_name("IN");
identifier_c     decompose_var_instance_name_c::PT_operator_name("PT");


