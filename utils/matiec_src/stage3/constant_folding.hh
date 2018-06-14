/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2009-2012  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2012       Manuele Conti (conti.ma@alice.it)
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

/* Determine the value of an constant expression.
 * A reference to the relevant type definition is returned.
 *
 * For example:
 *       2 + 3       -> returns reference 
 *       22.2 - 5    -> returns reference
 *       etc...
 */

#include <vector>
#include "../absyntax_utils/absyntax_utils.hh"
#include "../util/symtable.hh"



/* For the moment we disable constant propagation algorithm as it is not yet complete, 
 * and due to this is currently brocken and producing incorrect results!
 */
#define DO_CONSTANT_PROPAGATION__ 0



class constant_folding_c : public iterator_visitor_c {
  protected:
    int error_count;
    bool warning_found;
    int current_display_error_level;
  private:
    /* Pointer to the previous IL instruction, which contains the current cvalue of the data stored in the IL stack, i.e. the default variable, a.k.a. accumulator */
    symbol_c *prev_il_instruction;
    /* the current IL operand being analyzed */
    symbol_c *il_operand;



  public:
    constant_folding_c(symbol_c *symbol = NULL);
    virtual ~constant_folding_c(void);
    int get_error_count();
 
  private:
    /*********************/
    /* B 1.2 - Constants */
    /*********************/
    /******************************/
    /* B 1.2.1 - Numeric Literals */
    /******************************/
    void *visit(real_c *symbol);
    void *visit(integer_c *symbol);
    void *visit(neg_real_c *symbol);
    void *visit(neg_integer_c *symbol);
    void *visit(binary_integer_c *symbol);
    void *visit(octal_integer_c *symbol);
    void *visit(hex_integer_c *symbol);
    void *visit(integer_literal_c *symbol);
    void *visit(real_literal_c *symbol);
    void *visit(bit_string_literal_c *symbol);
    void *visit(boolean_literal_c *symbol);
    void *visit(boolean_true_c *symbol);
    void *visit(boolean_false_c *symbol);

    /************************/
    /* B 1.2.3.1 - Duration */
    /********* **************/
    void *visit(fixed_point_c *symbol);

    /****************************************/
    /* B.2 - Language IL (Instruction List) */
    /****************************************/
    /***********************************/
    /* B 2.1 Instructions and Operands */
    /***********************************/
    // void *visit(instruction_list_c *symbol); /* Not needed, since we inherit from iterator_visitor_c */
    void *visit(il_instruction_c *symbol);
    void *visit(il_simple_operation_c *symbol);
    //void *visit(il_function_call_c *symbol);  /* TODO */
    void *visit(il_expression_c *symbol);
    void *visit(il_jump_operation_c *symbol);
    void *visit(il_fb_call_c *symbol);
    //void *visit(il_formal_funct_call_c *symbol);   /* TODO */
    //void *visit(il_operand_list_c *symbol);  /* Not needed, since we inherit from iterator_visitor_c */
    void *visit(simple_instr_list_c *symbol);
    void *visit(il_simple_instruction_c *symbol);


    /*******************/
    /* B 2.2 Operators */
    /*******************/
    void *visit(   LD_operator_c *symbol);
    void *visit(  LDN_operator_c *symbol);
    void *visit(   ST_operator_c *symbol);
    void *visit(  STN_operator_c *symbol);
    void *visit(  NOT_operator_c *symbol);
    void *visit(    S_operator_c *symbol);
    void *visit(    R_operator_c *symbol);
    void *visit(   S1_operator_c *symbol);
    void *visit(   R1_operator_c *symbol);
    void *visit(  CLK_operator_c *symbol);
    void *visit(   CU_operator_c *symbol);
    void *visit(   CD_operator_c *symbol);
    void *visit(   PV_operator_c *symbol);
    void *visit(   IN_operator_c *symbol);
    void *visit(   PT_operator_c *symbol);
    void *visit(  AND_operator_c *symbol);
    void *visit(   OR_operator_c *symbol);
    void *visit(  XOR_operator_c *symbol);
    void *visit( ANDN_operator_c *symbol);
    void *visit(  ORN_operator_c *symbol);
    void *visit( XORN_operator_c *symbol);
    void *visit(  ADD_operator_c *symbol);
    void *visit(  SUB_operator_c *symbol);
    void *visit(  MUL_operator_c *symbol);
    void *visit(  DIV_operator_c *symbol);
    void *visit(  MOD_operator_c *symbol);
    void *visit(   GT_operator_c *symbol);
    void *visit(   GE_operator_c *symbol);
    void *visit(   EQ_operator_c *symbol);
    void *visit(   LT_operator_c *symbol);
    void *visit(   LE_operator_c *symbol);
    void *visit(   NE_operator_c *symbol);
    void *visit(  CAL_operator_c *symbol);
    void *visit( CALC_operator_c *symbol);
    void *visit(CALCN_operator_c *symbol);
    void *visit(  RET_operator_c *symbol);
    void *visit( RETC_operator_c *symbol);
    void *visit(RETCN_operator_c *symbol);
    void *visit(  JMP_operator_c *symbol);
    void *visit( JMPC_operator_c *symbol);
    void *visit(JMPCN_operator_c *symbol);
    /* Symbol class handled together with function call checks */
    // void *visit(il_assign_operator_c *symbol, variable_name);
    /* Symbol class handled together with function call checks */
    // void *visit(il_assign_operator_c *symbol, option, variable_name);

    /***************************************/
    /* B.3 - Language ST (Structured Text) */
    /***************************************/
    /***********************/
    /* B 3.1 - Expressions */
    /***********************/
    void *visit(    or_expression_c *symbol);
    void *visit(   xor_expression_c *symbol);
    void *visit(   and_expression_c *symbol);
    void *visit(   equ_expression_c *symbol);
    void *visit(notequ_expression_c *symbol);
    void *visit(    lt_expression_c *symbol);
    void *visit(    gt_expression_c *symbol);
    void *visit(    le_expression_c *symbol);
    void *visit(    ge_expression_c *symbol);
    void *visit(   add_expression_c *symbol);
    void *visit(   sub_expression_c *symbol);
    void *visit(   mul_expression_c *symbol);
    void *visit(   div_expression_c *symbol);
    void *visit(   mod_expression_c *symbol);
    void *visit( power_expression_c *symbol);
    void *visit(   neg_expression_c *symbol);
    void *visit(   not_expression_c *symbol);
    //void *visit(function_invocation_c *symbol); /* TODO */
};












#include <deque>

class constant_propagation_c : public constant_folding_c {
  public:
    constant_propagation_c(symbol_c *symbol = NULL);
    virtual ~constant_propagation_c(void);
    typedef symtable_c<const_value_c> map_values_t;
  private:
    symbol_c *current_resource;
    symbol_c *current_configuration;
    map_values_t *values;
    map_values_t var_global_values;
    /* A stack of all the FB declarations currently being recursively constant propagated */
    std::deque<function_block_declaration_c *> fbs_currently_being_visited; // We use a deque instead of stack, so we can search in the stack using direct access to its elements!

    void *handle_var_list_decl(symbol_c *var_list, symbol_c *type_decl, bool is_global_var = false);
    void *handle_var_decl     (symbol_c *var_list, bool fixed_init_value);
    // Flag to indicate whether the variables in the variable declaration list will always have a fixed value when the POU is executed!
    // VAR CONSTANT ... END_VAR will always be true
    // VAR          ... END_VAR will always be true for functions (who initialise local variables every time they are called), but false for FBs and PROGRAMS
    bool fixed_init_value_; 
    bool function_pou_;
    bool is_constant(symbol_c *option);
    bool is_retain  (symbol_c *option);
    static map_values_t inner_left_join_values(map_values_t m1, map_values_t m2);


  private:
    /***************************/
    /* B 0 - Programming Model */
    /***************************/
    void *visit(library_c *symbol);

    /*********************/
    /* B 1.4 - Variables */
    /*********************/
    #if DO_CONSTANT_PROPAGATION__
    void *visit(symbolic_variable_c *symbol);
    #endif // DO_CONSTANT_PROPAGATION__
    void *visit(symbolic_constant_c *symbol);
                             
    /******************************************/
    /* B 1.4.3 - Declaration & Initialisation */
    /******************************************/
    void *visit(        input_declarations_c *symbol);
    void *visit(       output_declarations_c *symbol);
    void *visit( input_output_declarations_c *symbol);
    void *visit(          var_declarations_c *symbol);
    void *visit(retentive_var_declarations_c *symbol);
    void *visit( external_var_declarations_c *symbol);
    void *visit(   global_var_declarations_c *symbol);
    void *visit(  external_declaration_c     *symbol);
    void *visit(global_var_decl_c            *symbol);
    void *visit( var1_init_decl_c            *symbol);
    void *visit(   fb_name_decl_c            *symbol);

    /**************************************/
    /* B.1.5 - Program organization units */
    /**************************************/
    /***********************/
    /* B 1.5.1 - Functions */
    /***********************/
    void *visit(function_declaration_c *symbol);
    void *visit(function_var_decls_c   *symbol);

    /*****************************/
    /* B 1.5.2 - Function Blocks */
    /*****************************/
    void *visit(function_block_declaration_c *symbol);
    void *visit(            temp_var_decls_c *symbol);
    void *visit(   non_retentive_var_decls_c *symbol);

    /**********************/
    /* B 1.5.3 - Programs */
    /**********************/
    void *visit(       program_declaration_c *symbol);

    /********************************/
    /* B 1.7 Configuration elements */
    /********************************/
    void *visit( configuration_declaration_c *symbol);
    void *visit(      resource_declaration_c *symbol);
    void *visit(     program_configuration_c *symbol);
    void *visit(                   fb_task_c *symbol);


    /****************************************/
    /* B.2 - Language IL (Instruction List) */
    /****************************************/
    /***********************************/
    /* B 2.1 Instructions and Operands */
    /***********************************/
    //void *visit(il_function_call_c *symbol);  /* TODO */
    // void *visit(il_fb_call_c *symbol);       /* TODO: move from constant_folding_c */
    //void *visit(il_formal_funct_call_c *symbol);   /* TODO */
    //void *visit(il_operand_list_c *symbol);  /* Not needed, since we inherit from iterator_visitor_c */


    /*******************/
    /* B 2.2 Operators */
    /*******************/
    /* Symbol class handled together with function call checks */
    // void *visit(il_assign_operator_c *symbol, variable_name);
    /* Symbol class handled together with function call checks */
    // void *visit(il_assign_operator_c *symbol, option, variable_name);

    /***************************************/
    /* B.3 - Language ST (Structured Text) */
    /***************************************/
    /***********************/
    /* B 3.1 - Expressions */
    /***********************/
    //void *visit(function_invocation_c *symbol); /* TODO */

    #if DO_CONSTANT_PROPAGATION__
    /*********************************/
    /* B 3.2.1 Assignment Statements */
    /*********************************/
    void *visit(assignment_statement_c *symbol);

    /********************************/
    /* B 3.2.3 Selection Statements */
    /********************************/
    void *visit(if_statement_c *symbol);

    /********************************/
    /* B 3.2.4 Iteration Statements */
    /********************************/
    void *visit(for_statement_c *symbol);
    void *visit(while_statement_c *symbol);
    void *visit(repeat_statement_c *symbol);
    #endif // DO_CONSTANT_PROPAGATION__
};

