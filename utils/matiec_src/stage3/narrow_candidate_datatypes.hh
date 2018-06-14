/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2009-2012  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2012       Manuele Conti (manuele.conti@sirius-es.it)
 *  Copyright (C) 2012       Matteo Facchinetti (matteo.facchinetti@sirius-es.it)
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


/* NOTE: The algorithm implemented here assumes that candidate datatype lists have already been filled!
 *       BEFORE running this visitor, be sure to CALL the fill_candidate_datatype_c visitor!
 */


/*
 *  Choose, from the list of all the possible datatypes each expression may take, the single datatype that it will in fact take.
 *  The resulting (chosen) datatype, will be stored in the symbol_c.datatype variable, leaving the candidate datatype list untouched!
 * 
 *  For rvalue expressions, this decision will be based on the datatype of the lvalue expression.
 *  For lvalue expressions, the candidate datatype list should have a single entry.
 * 
 *  For example, the very simple literal '0' in 'foo := 0', may represent a:
 *    BOOL, BYTE, WORD, DWORD, LWORD, USINT, SINT, UINT, INT, UDINT, DINT, ULINT, LINT (as well as the SAFE versions of these data tyes too!)
 * 
 *  In this class, the datatype of '0' will be set to the same datatype as the 'foo' variable.
 *  If the intersection of the candidate datatype lists of the left and right side expressions is empty, 
 *  then a datatype error has been found, and the datatype is either left at NULL, or set to a pointer of an invalid_type_name_c object!
 */

#ifndef _NARROW_CANDIDATE_DATATYPES_HH
#define _NARROW_CANDIDATE_DATATYPES_HH



#include "../absyntax_utils/absyntax_utils.hh"
#include "datatype_functions.hh"

class narrow_candidate_datatypes_c: public iterator_visitor_c {

  private:
    search_varfb_instance_type_c *search_varfb_instance_type;
    symbol_c *il_operand;
    il_instruction_c *fake_prev_il_instruction;
    il_instruction_c   *current_il_instruction;

    bool is_widening_compatible(const struct widen_entry widen_table[], symbol_c *left_type, symbol_c *right_type, symbol_c *result_type, bool *deprecated_status = NULL);

    void *narrow_spec_init           (symbol_c *symbol, symbol_c *type_decl, symbol_c *init_value);
    void *narrow_type_decl           (symbol_c *symbol, symbol_c *type_name, symbol_c *spec_init);
    void  narrow_function_invocation (symbol_c *f_call, generic_function_call_t fcall_data);
    void  narrow_nonformal_call      (symbol_c *f_call, symbol_c *f_decl, int *ext_parm_count = NULL);
    void  narrow_formal_call         (symbol_c *f_call, symbol_c *f_decl, int *ext_parm_count = NULL);
    void *narrow_implicit_il_fb_call (symbol_c *symbol, const char *param_name, symbol_c *&called_fb_declaration);
    void *narrow_S_and_R_operator    (symbol_c *symbol, const char *param_name, symbol_c * called_fb_declaration);
    void *narrow_store_operator      (symbol_c *symbol);
    void *narrow_conditional_operator(symbol_c *symbol);
    void *narrow_binary_operator     (const struct widen_entry widen_table[], symbol_c *symbol,                                     bool *deprecated_operation = NULL);
    void *narrow_binary_expression   (const struct widen_entry widen_table[], symbol_c *symbol, symbol_c *l_expr, symbol_c *r_expr, bool *deprecated_operation = NULL, bool allow_enums = false);
    void *narrow_equality_comparison (const struct widen_entry widen_table[], symbol_c *symbol, symbol_c *l_expr, symbol_c *r_expr, bool *deprecated_operation = NULL);
    void *narrow_var_declaration     (symbol_c *type);

    void *set_il_operand_datatype    (symbol_c *il_operand, symbol_c *datatype);



  public:
    narrow_candidate_datatypes_c(symbol_c *ignore);
    virtual ~narrow_candidate_datatypes_c(void);

    symbol_c *base_type(symbol_c *symbol);

    /*************************/
    /* B.1 - Common elements */
    /*************************/
    /*******************************************/
    /* B 1.1 - Letters, digits and identifiers */
    /*******************************************/
//  void *visit(                 identifier_c *symbol);
    void *visit(derived_datatype_identifier_c *symbol);
//  void *visit(         poutype_identifier_c *symbol);

    /**********************/
    /* B 1.3 - Data types */
    /**********************/
    /***********************************/
    /* B 1.3.1 - Elementary Data Types */
    /***********************************/
    void *visit(    time_type_name_c    *symbol); 
    void *visit(    bool_type_name_c    *symbol); 
    void *visit(    sint_type_name_c    *symbol); 
    void *visit(    int_type_name_c     *symbol); 
    void *visit(    dint_type_name_c    *symbol); 
    void *visit(    lint_type_name_c    *symbol); 
    void *visit(    usint_type_name_c   *symbol); 
    void *visit(    uint_type_name_c    *symbol); 
    void *visit(    udint_type_name_c   *symbol); 
    void *visit(    ulint_type_name_c   *symbol); 
    void *visit(    real_type_name_c    *symbol); 
    void *visit(    lreal_type_name_c   *symbol); 
    void *visit(    date_type_name_c    *symbol); 
    void *visit(    tod_type_name_c     *symbol); 
    void *visit(    dt_type_name_c      *symbol); 
    void *visit(    byte_type_name_c    *symbol); 
    void *visit(    word_type_name_c    *symbol); 
    void *visit(    dword_type_name_c   *symbol); 
    void *visit(    lword_type_name_c   *symbol); 
    void *visit(    string_type_name_c  *symbol); 
    void *visit(    wstring_type_name_c *symbol); 

    void *visit(safetime_type_name_c    *symbol); 
    void *visit(safebool_type_name_c    *symbol); 
    void *visit(safesint_type_name_c    *symbol); 
    void *visit(safeint_type_name_c     *symbol); 
    void *visit(safedint_type_name_c    *symbol); 
    void *visit(safelint_type_name_c    *symbol); 
    void *visit(safeusint_type_name_c   *symbol); 
    void *visit(safeuint_type_name_c    *symbol); 
    void *visit(safeudint_type_name_c   *symbol); 
    void *visit(safeulint_type_name_c   *symbol); 
    void *visit(safereal_type_name_c    *symbol); 
    void *visit(safelreal_type_name_c   *symbol); 
    void *visit(safedate_type_name_c    *symbol); 
    void *visit(safetod_type_name_c     *symbol); 
    void *visit(safedt_type_name_c      *symbol); 
    void *visit(safebyte_type_name_c    *symbol); 
    void *visit(safeword_type_name_c    *symbol); 
    void *visit(safedword_type_name_c   *symbol); 
    void *visit(safelword_type_name_c   *symbol); 
    void *visit(safestring_type_name_c  *symbol); 
    void *visit(safewstring_type_name_c *symbol); 


    /********************************/
    /* B.1.3.2 - Generic data types */
    /********************************/

    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
//  void *visit(data_type_declaration_c *symbol);   /* Not required. already handled by iterator_visitor_c base class */
//  void *visit(type_declaration_list_c *symbol);   /* Not required. already handled by iterator_visitor_c base class */
    void *visit(simple_type_declaration_c *symbol); /* Not required. already handled by iterator_visitor_c base class */
    void *visit(simple_spec_init_c *symbol);
    void *visit(subrange_type_declaration_c *symbol);
    void *visit(subrange_spec_init_c *symbol);
    void *visit(subrange_specification_c *symbol);
    void *visit(subrange_c *symbol);
    void *visit(enumerated_type_declaration_c *symbol);
    void *visit(enumerated_spec_init_c *symbol);
    void *visit(enumerated_value_list_c *symbol);
//  void *visit(enumerated_value_c *symbol);        /* Not required */
    void *visit(array_type_declaration_c *symbol);
    void *visit(array_spec_init_c *symbol);
//  void *visit(array_specification_c *symbol);
//  void *visit(array_subrange_list_c *symbol);
//  void *visit(array_initial_elements_list_c *symbol);
//  void *visit(array_initial_elements_c *symbol);
    void *visit(structure_type_declaration_c *symbol);
    void *visit(initialized_structure_c *symbol);
//  void *visit(structure_element_declaration_list_c *symbol);
//  void *visit(structure_element_declaration_c *symbol);
//  void *visit(structure_element_initialization_list_c *symbol);
//  void *visit(structure_element_initialization_c *symbol);
//  void *visit(string_type_declaration_c *symbol);
    void *visit(fb_spec_init_c *symbol);
      
    void *visit(ref_spec_c *symbol);      // Defined in IEC 61131-3 v3
    void *visit(ref_spec_init_c *symbol); // Defined in IEC 61131-3 v3
    void *visit(ref_type_decl_c *symbol); // Defined in IEC 61131-3 v3


    /*********************/
    /* B 1.4 - Variables */
    /*********************/
    void *visit(symbolic_variable_c *symbol);
    /********************************************/
    /* B 1.4.1 - Directly Represented Variables */
    /********************************************/
    /*************************************/
    /* B 1.4.2 - Multi-element variables */
    /*************************************/
    void *visit(array_variable_c *symbol);
    void *visit(subscript_list_c *symbol);
    void *visit(structured_variable_c *symbol);

    /******************************************/
    /* B 1.4.3 - Declaration & Initialisation */
    /******************************************/
    void *visit(var1_list_c                  *symbol);
    void *visit(location_c                   *symbol);
    void *visit(located_var_decl_c           *symbol);
    void *visit(var1_init_decl_c             *symbol);
    void *visit(array_var_init_decl_c        *symbol);
    void *visit(structured_var_init_decl_c   *symbol);
    void *visit(fb_name_decl_c               *symbol);
    void *visit(array_var_declaration_c      *symbol);
    void *visit(structured_var_declaration_c *symbol);
    void *visit(external_declaration_c       *symbol);
    void *visit(global_var_decl_c            *symbol);
    void *visit(incompl_located_var_decl_c   *symbol);
    //void *visit(single_byte_string_var_declaration_c *symbol);
    //void *visit(double_byte_string_var_declaration_c *symbol);

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

    /********************************************/
    /* B 1.6 Sequential function chart elements */
    /********************************************/
    void *visit(transition_condition_c *symbol);

    /********************************/
    /* B 1.7 Configuration elements */
    /********************************/
    void *visit(configuration_declaration_c   *symbol);
    void *visit(resource_declaration_c        *symbol);
    void *visit(single_resource_declaration_c *symbol);

    /****************************************/
    /* B.2 - Language IL (Instruction List) */
    /****************************************/
    /***********************************/
    /* B 2.1 Instructions and Operands */
    /***********************************/
    void *visit(instruction_list_c *symbol);
    void *visit(il_instruction_c *symbol);
    void *visit(il_simple_operation_c *symbol);
    void *visit(il_function_call_c *symbol);
    void *visit(il_expression_c *symbol);
    void *visit(il_jump_operation_c *symbol);
    void *visit(il_fb_call_c *symbol);
    void *visit(il_formal_funct_call_c *symbol);
//  void *visit(il_operand_list_c *symbol);
    void *visit(simple_instr_list_c *symbol);
    void *visit(il_simple_instruction_c*symbol);
//  void *visit(il_param_list_c *symbol);
//  void *visit(il_param_assignment_c *symbol);
//  void *visit(il_param_out_assignment_c *symbol);

    /*******************/
    /* B 2.2 Operators */
    /*******************/
    void *visit(LD_operator_c *symbol);
    void *visit(LDN_operator_c *symbol);
    void *visit(ST_operator_c *symbol);
    void *visit(STN_operator_c *symbol);
    void *visit(NOT_operator_c *symbol);
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
    void *visit(AND_operator_c *symbol);
    void *visit(OR_operator_c *symbol);
    void *visit(XOR_operator_c *symbol);
    void *visit(ANDN_operator_c *symbol);
    void *visit(ORN_operator_c *symbol);
    void *visit(XORN_operator_c *symbol);
    void *visit(ADD_operator_c *symbol);
    void *visit(SUB_operator_c *symbol);
    void *visit(MUL_operator_c *symbol);
    void *visit(DIV_operator_c *symbol);
    void *visit(MOD_operator_c *symbol);
    void *visit(GT_operator_c *symbol);
    void *visit(GE_operator_c *symbol);
    void *visit(EQ_operator_c *symbol);
    void *visit(LT_operator_c *symbol);
    void *visit(LE_operator_c *symbol);
    void *visit(NE_operator_c *symbol);
    void *visit(CAL_operator_c *symbol);
    void *visit(CALC_operator_c *symbol);
    void *visit(CALCN_operator_c *symbol);
    void *visit(RET_operator_c *symbol);
    void *visit(RETC_operator_c *symbol);
    void *visit(RETCN_operator_c *symbol);
    void *visit(JMP_operator_c *symbol);
    void *visit(JMPC_operator_c *symbol);
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
    void *visit(   deref_operator_c   *symbol);
    void *visit(   deref_expression_c *symbol);
    void *visit(     ref_expression_c *symbol);
    void *visit(      or_expression_c *symbol);
    void *visit(     xor_expression_c *symbol);
    void *visit(     and_expression_c *symbol);
    void *visit(     equ_expression_c *symbol);
    void *visit(  notequ_expression_c *symbol);
    void *visit(      lt_expression_c *symbol);
    void *visit(      gt_expression_c *symbol);
    void *visit(      le_expression_c *symbol);
    void *visit(      ge_expression_c *symbol);
    void *visit(     add_expression_c *symbol);
    void *visit(     sub_expression_c *symbol);
    void *visit(     mul_expression_c *symbol);
    void *visit(     div_expression_c *symbol);
    void *visit(     mod_expression_c *symbol);
    void *visit(   power_expression_c *symbol);
    void *visit(     neg_expression_c *symbol);
    void *visit(     not_expression_c *symbol);
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
    /* B 3.2.3 Selection Statements */
    /********************************/
    void *visit(if_statement_c *symbol);
    void *visit(elseif_statement_c *symbol);
    void *visit(case_statement_c *symbol);
    void *visit(case_element_list_c *symbol);
    void *visit(case_element_c *symbol);
    void *visit(case_list_c *symbol);

    /********************************/
    /* B 3.2.4 Iteration Statements */
    /********************************/
    void *visit(for_statement_c *symbol);
    void *visit(while_statement_c *symbol);
    void *visit(repeat_statement_c *symbol);

}; // narrow_candidate_datatypes_c




#endif // #ifndef _NARROW_CANDIDATE_DATATYPES_HH



