/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2009-2012  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2012       Manuele Conti  (conti.ma@alice.it)
 *
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

#include <vector>
#include "../absyntax_utils/absyntax_utils.hh"
#include "datatype_functions.hh"


/* Expressions on the left hand side of assignment statements have aditional restrictions on their datatype.
 * For example, they cannot be literals, CONSTANT type variables, function invocations, etc...
 * This class wil do those checks.
 * 
 * Note that assignment may also be done when passing variables to OUTPUT or IN_OUT function parameters,so we check those too.
 */



class lvalue_check_c: public iterator_visitor_c {

  private:
    search_varfb_instance_type_c *search_varfb_instance_type;
    search_var_instance_decl_c *search_var_instance_decl;
    int error_count;
    int current_display_error_level;
    std::vector <token_c *> control_variables;
    symbol_c *current_il_operand;

    void verify_is_lvalue              (symbol_c *lvalue);
    void check_assignment_to_controlvar(symbol_c *lvalue);
    void check_assignment_to_output    (symbol_c *lvalue);
    void check_assignment_to_constant  (symbol_c *lvalue);
    void check_assignment_to_expression(symbol_c *lvalue);
    void check_assignment_to_il_list   (symbol_c *lvalue);
    
    void check_formal_call   (symbol_c *f_call, symbol_c *f_decl);
    void check_nonformal_call(symbol_c *f_call, symbol_c *f_decl);


  public:
    lvalue_check_c(symbol_c *ignore);
    virtual ~lvalue_check_c(void);
    int get_error_count();

    /**************************************/
    /* B 1.5 - Program organisation units */
    /**************************************/
    /***********************/
    /* B 1.5.1 - Functions */
    /***********************/
    void *visit(function_declaration_c *symbol);

    /*****************************/
    /* B 1.5.2 - Function blocks */
    /*****************************/
    void *visit(function_block_declaration_c *symbol);

    /**********************/
    /* B 1.5.3 - Programs */
    /**********************/
    void *visit(program_declaration_c *symbol);

    /****************************************/
    /* B.2 - Language IL (Instruction List) */
    /****************************************/
    /***********************************/
    /* B 2.1 Instructions and Operands */
    /***********************************/
    void *visit(il_instruction_c *symbol);
    void *visit(il_simple_operation_c *symbol);
    void *visit(il_function_call_c *symbol);
    void *visit(il_fb_call_c *symbol);
    void *visit(il_formal_funct_call_c *symbol);

    /*******************/
    /* B 2.2 Operators */
    /*******************/
    void *visit(ST_operator_c *symbol);
    void *visit(STN_operator_c *symbol);
    void *visit(S_operator_c *symbol);
    void *visit(R_operator_c *symbol);

    /***************************************/
    /* B.3 - Language ST (Structured Text) */
    /***************************************/
    /***********************/
    /* B 3.1 - Expressions */
    /***********************/
    void *visit(function_invocation_c *symbol);

    /*********************************/
    /* B 3.2.1 Assignment Statements */
    /*********************************/
    void *visit(assignment_statement_c *symbol);

    /*****************************************/
    /* B 3.2.2 Subprogram Control Statements */
    /*****************************************/
    void *visit(fb_invocation_c *symbol);

    /********************************/
    /* B 3.2.4 Iteration Statements */
    /********************************/
    void *visit(for_statement_c *symbol);

}; /* lvalue_check_c */







