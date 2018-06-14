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
 *  forced_narrow_candidate_datatypes_c
 *
 */



#include "../absyntax_utils/absyntax_utils.hh"
#include "narrow_candidate_datatypes.hh"



class forced_narrow_candidate_datatypes_c: public narrow_candidate_datatypes_c {

  private:
    void forced_narrow_il_instruction(symbol_c *symbol, std::vector <symbol_c *> &next_il_instruction);
    
  public:
    forced_narrow_candidate_datatypes_c(symbol_c *ignore);
    virtual ~forced_narrow_candidate_datatypes_c(void);
   
    /****************************************/
    /* B.2 - Language IL (Instruction List) */
    /****************************************/
    /***********************************/
    /* B 2.1 Instructions and Operands */
    /***********************************/
    void *visit(instruction_list_c *symbol);
    void *visit(il_instruction_c *symbol);
//  void *visit(il_simple_operation_c *symbol);
//  void *visit(il_function_call_c *symbol);
//  void *visit(il_expression_c *symbol);
//  void *visit(il_jump_operation_c *symbol);
//  void *visit(il_fb_call_c *symbol);
//  void *visit(il_formal_funct_call_c *symbol);
//  void *visit(il_operand_list_c *symbol);
//  void *visit(simple_instr_list_c *symbol);
    void *visit(il_simple_instruction_c*symbol);
//  void *visit(il_param_list_c *symbol);
//  void *visit(il_param_assignment_c *symbol);
//  void *visit(il_param_out_assignment_c *symbol);
//  void *visit(il_assign_operator_c *symbol);
//  void *visit(il_assign_operator_c *symbol);
 
    /***************************************/
    /* B.3 - Language ST (Structured Text) */
    /***************************************/
    void *visit(statement_list_c *symbol);
}; // forced_narrow_candidate_datatypes_c









