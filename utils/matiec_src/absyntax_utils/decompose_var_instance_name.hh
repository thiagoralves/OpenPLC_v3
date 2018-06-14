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



#include "../absyntax/visitor.hh"



class decompose_var_instance_name_c: null_visitor_c {

  public:
    /***********************************/
    /* B 1.2 - Operators               */
    /***********************************/
    static identifier_c     LD_operator_name;
    static identifier_c     S_operator_name;
    static identifier_c     R_operator_name;
    static identifier_c     S1_operator_name;
    static identifier_c     R1_operator_name;
    static identifier_c     CLK_operator_name;
    static identifier_c     CU_operator_name;
    static identifier_c     CD_operator_name;
    static identifier_c     PV_operator_name;
    static identifier_c     IN_operator_name;
    static identifier_c     PT_operator_name;


  private:
    symbol_c *variable_name;
    symbol_c *next_variable_name;
    symbol_c *current_recursive_variable_name;
    symbol_c *previously_returned_variable_name;
    list_c   *current_array_subscript_list;

  public:
    decompose_var_instance_name_c(symbol_c *variable_instance_name);
    /* Get the next element in the strcutured variable */
    symbol_c *get_next(void);
    /* If the current element in the structured variable is an array, return its subscript_list, otherwise return NULL */
    list_c *get_current_arraysubs_list(void);

  private:
  /*************************/
  /* B.1 - Common elements */
  /*************************/
  /*******************************************/
  /* B 1.1 - Letters, digits and identifiers */
  /*******************************************/
    void *visit(identifier_c *symbol);

  /*********************/
  /* B 1.4 - Variables */
  /*********************/
    void *visit(symbolic_variable_c *symbol);

  /********************************************/
  /* B.1.4.1   Directly Represented Variables */
  /********************************************/
    void *visit(direct_variable_c *symbol);
  
  /*************************************/
  /* B.1.4.2   Multi-element Variables */
  /*************************************/
  /*  subscripted_variable '[' subscript_list ']' */
  // SYM_REF2(array_variable_c, subscripted_variable, subscript_list)
    void *visit(array_variable_c *symbol);
  
  /*  record_variable '.' field_selector */
  /*  WARNING: input and/or output variables of function blocks
   *           may be accessed as fields of a tructured variable!
   *           Code handling a structured_variable_c must take
   *           this into account!
   */
  //SYM_REF2(structured_variable_c, record_variable, field_selector)
    void *visit(structured_variable_c *symbol);

    /********************************/
    /* B 2.2 - Operators            */
    /********************************/
    void *visit(LD_operator_c *symbol);
    void *visit(S_operator_c *symbol);
    void *visit(R_operator_c *symbol);
    void *visit(S1_operator_c *symbol);
    void *visit(R1_operator_c *symbol);
    void *visit(CLK_operator_c *symbol);
    void *visit(CU_operator_c *symbol);
    void *visit(CD_operator_c *symbol);
    void *visit(PV_operator_c *symbol);
    void *visit(IN_operator_c *symbol);
    void *visit(PT_operator_c *symbol);

}; // decompose_var_instance_name_c




