/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2012  Mario de Sousa (msousa@fe.up.pt)
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


/*
 *  Do flow control analysis of the IEC 61131-3 code.
 *
 *  We currently only do this for IL code.
 *  This class will annotate the abstract syntax tree, by filling in the
 *  prev_il_instruction variable in the il_instruction_c, so it points to
 *  the previous il_instruction_c object in the instruction list instruction_list_c.
 */



#include "../absyntax_utils/absyntax_utils.hh"


class flow_control_analysis_c: public iterator_visitor_c {

  private:
    search_il_label_c *search_il_label;
    symbol_c          *prev_il_instruction;
    symbol_c          *curr_il_instruction;
    bool      prev_il_instruction_is_JMP_or_RET;

  private:
    void link_insert  (symbol_c *prev_instruction, symbol_c *next_instruction);
    void link_pushback(symbol_c *prev_instruction, symbol_c *next_instruction);

  public:
    flow_control_analysis_c(symbol_c *ignore);
    virtual ~flow_control_analysis_c(void);

    /**************************************/
    /* B 1.5 - Program organization units */
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

    /********************************/
    /* B 1.7 Configuration elements */
    /********************************/
    void *visit(configuration_declaration_c *symbol);
    
    /****************************************/
    /* B.2 - Language IL (Instruction List) */
    /****************************************/
    /***********************************/
    /* B 2.1 Instructions and Operands */
    /***********************************/
    void *visit(instruction_list_c *symbol);
    void *visit(il_instruction_c *symbol);
//     void *visit(il_simple_operation_c *symbol);
//     void *visit(il_function_call_c *symbol);
    void *visit(il_expression_c *symbol);
    void *visit(il_jump_operation_c *symbol);
//     void *visit(il_fb_call_c *symbol);
//     void *visit(il_formal_funct_call_c *symbol);
//     void *visit(il_operand_list_c *symbol);
       void *visit(simple_instr_list_c *symbol);
       void *visit(il_simple_instruction_c*symbol);
//     void *visit(il_param_list_c *symbol);
//     void *visit(il_param_assignment_c *symbol);
//     void *visit(il_param_out_assignment_c *symbol);

    /*******************/
    /* B 2.2 Operators */
    /*******************/
//     void *visit(   LD_operator_c *symbol);
//     void *visit(  LDN_operator_c *symbol);
//     void *visit(   ST_operator_c *symbol);
//     void *visit(  STN_operator_c *symbol);
//     void *visit(  NOT_operator_c *symbol);
//     void *visit(    S_operator_c *symbol);
//     void *visit(    R_operator_c *symbol);
//     void *visit(   S1_operator_c *symbol);
//     void *visit(   R1_operator_c *symbol);
//     void *visit(  CLK_operator_c *symbol);
//     void *visit(   CU_operator_c *symbol);
//     void *visit(   CD_operator_c *symbol);
//     void *visit(   PV_operator_c *symbol);
//     void *visit(   IN_operator_c *symbol);
//     void *visit(   PT_operator_c *symbol);
//     void *visit(  AND_operator_c *symbol);
//     void *visit(   OR_operator_c *symbol);
//     void *visit(  XOR_operator_c *symbol);
//     void *visit( ANDN_operator_c *symbol);
//     void *visit(  ORN_operator_c *symbol);
//     void *visit( XORN_operator_c *symbol);
//     void *visit(  ADD_operator_c *symbol);
//     void *visit(  SUB_operator_c *symbol);
//     void *visit(  MUL_operator_c *symbol);
//     void *visit(  DIV_operator_c *symbol);
//     void *visit(  MOD_operator_c *symbol);
//     void *visit(   GT_operator_c *symbol);
//     void *visit(   GE_operator_c *symbol);
//     void *visit(   EQ_operator_c *symbol);
//     void *visit(   LT_operator_c *symbol);
//     void *visit(   LE_operator_c *symbol);
//     void *visit(   NE_operator_c *symbol);
//     void *visit(  CAL_operator_c *symbol);
//     void *visit( CALC_operator_c *symbol);
//     void *visit(CALCN_operator_c *symbol);
     void *visit(  RET_operator_c *symbol);
//     void *visit( RETC_operator_c *symbol);
//     void *visit(RETCN_operator_c *symbol);
     void *visit(  JMP_operator_c *symbol);
//     void *visit( JMPC_operator_c *symbol);
//     void *visit(JMPCN_operator_c *symbol);

    /* Symbol class handled together with function call checks */
    // void *visit(il_assign_operator_c *symbol, variable_name);
    /* Symbol class handled together with function call checks */
    // void *visit(il_assign_operator_c *symbol, option, variable_name);

}; // flow_control_analysis_c








