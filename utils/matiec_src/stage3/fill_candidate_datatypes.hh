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

/* NOTE: The algorithm implemented here assumes that flow control analysis has already been completed!
 *       BEFORE running this visitor, be sure to CALL the flow_control_analysis_c visitor!
 */


/*
 *  Fill the candidate datatype list for all symbols that may legally 'have' a data type (e.g. variables, literals, function calls, expressions, etc.)
 * 
 *  The candidate datatype list will be filled with a list of all the data types that expression may legally take.
 *  For example, the very simple literal '0' (as in foo := 0), may represent a:
 *    BOOL, BYTE, WORD, DWORD, LWORD, USINT, SINT, UINT, INT, UDINT, DINT, ULINT, LINT (as well as the SAFE versions of these data tyes too!)
 *
 * WARNING: This visitor class starts off by building a map of all enumeration constants that are defined in the source code (i.e. a library_c symbol),
 *          and this map is later used to determine the datatpe of each use of an enumeration constant. By implication, the fill_candidate_datatypes_c 
 *          visitor class will only work corretly if it is asked to visit a symbol of class library_c!!
 */


#include "../absyntax_utils/absyntax_utils.hh"
#include "datatype_functions.hh"

class fill_candidate_datatypes_c: public iterator_visitor_c {

  private:
    search_var_instance_decl_c *search_var_instance_decl;
    /* When calling a function block, we must first find it's type,
     * by searching through the declarations of the variables currently
     * in scope.
     * This class does just that...
     * A new object instance is instantiated whenever we start checking semantics
     * for a function block type declaration, or a program declaration.
     * This object instance will then later be called while the
     * function block's or the program's body is being handled.
     *
     * Note that functions cannot contain calls to function blocks,
     * so we do not create an object instance when handling
     * a function declaration.
     */
    //     search_var_instance_decl_c *search_var_instance_decl;

    /* This variable was created to pass information from
     * fill_candidate_datatypes_c::visit(enumerated_spec_init_c *symbol) function to
     * fill_candidate_datatypes_c::visit(enumerated_value_list_c *symbol) function.
     */
    symbol_c *current_enumerated_spec_type;
    
    /* pointer to the Function, FB, or Program currently being analysed */
    symbol_c *current_scope;
    /* Pointer to the previous IL instruction, which contains the current data type (actually, the list of candidate data types) of the data stored in the IL stack, i.e. the default variable, a.k.a. accumulator */
    symbol_c *prev_il_instruction;
    /* the current IL operand being analyzed */
    symbol_c *il_operand;
    symbol_c *widening_conversion(symbol_c *left_type, symbol_c *right_type, const struct widen_entry widen_table[]);

    /* Match a function declaration with a function call through their parameters.*/
    /* returns true if compatible function/FB invocation, otherwise returns false */
    bool  match_nonformal_call(symbol_c *f_call, symbol_c *f_decl);
    bool  match_formal_call   (symbol_c *f_call, symbol_c *f_decl, symbol_c **first_param_datatype = NULL);
    void  handle_function_call(symbol_c *fcall, generic_function_call_t fcall_data);
    void *handle_implicit_il_fb_call(symbol_c *il_instruction, const char *param_name,   symbol_c *&called_fb_declaration);
    void *handle_S_and_R_operator   (symbol_c *symbol,         const char *operator_str, symbol_c *&called_fb_declaration);
    void *handle_equality_comparison(const struct widen_entry widen_table[], symbol_c *symbol, symbol_c *l_expr, symbol_c *r_expr);
    void *handle_binary_expression  (const struct widen_entry widen_table[], symbol_c *symbol, symbol_c *l_expr, symbol_c *r_expr);
    void *handle_binary_operator    (const struct widen_entry widen_table[], symbol_c *symbol, symbol_c *l_expr, symbol_c *r_expr);
    void *handle_conditional_il_flow_control_operator   (symbol_c *symbol);
    void *fill_type_decl            (symbol_c *symbol,   symbol_c *type_name, symbol_c *spec_init);
    void *fill_spec_init            (symbol_c *symbol,   symbol_c *type_spec, symbol_c *init_value);
    void *fill_var_declaration      (symbol_c *var_list, symbol_c *type);

    /* a helper function... */
    symbol_c *base_type(symbol_c *symbol);    
    
    /* add a data type to a candidate data type list, while guaranteeing no duplicate entries! */
    /* Returns true if it really did add the datatype to the list, or false if it was already present in the list! */
    bool add_datatype_to_candidate_list  (symbol_c *symbol, symbol_c *datatype);
    bool add_2datatypes_to_candidate_list(symbol_c *symbol, symbol_c *datatype1, symbol_c *datatype2);
    void remove_incompatible_datatypes(symbol_c *symbol);
    
    
  public:
    fill_candidate_datatypes_c(symbol_c *ignore);
    virtual ~fill_candidate_datatypes_c(void);

    
    /***************************/
    /* B 0 - Programming Model */
    /***************************/
    void *visit(library_c *symbol);

    /*************************/
    /* B.1 - Common elements */
    /*************************/
    /*******************************************/
    /* B 1.1 - Letters, digits and identifiers */
    /*******************************************/
//  void *visit(                 identifier_c *symbol);
    void *visit(derived_datatype_identifier_c *symbol);
//  void *visit(         poutype_identifier_c *symbol);

    /*********************/
    /* B 1.2 - Constants */
    /*********************/
    /*********************************/
    /* B 1.2.XX - Reference Literals */
    /*********************************/
    /* defined in IEC 61131-3 v3 - Basically the 'NULL' keyword! */
    void *visit(ref_value_null_literal_c *symbol);
      
    /******************************/
    /* B 1.2.1 - Numeric Literals */
    /******************************/
    void *handle_any_integer(symbol_c *symbol);
    void *handle_any_real   (symbol_c *symbol);
    void *handle_any_literal(symbol_c *symbol, symbol_c *symbol_value, symbol_c *symbol_type);
    
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

    /*******************************/
    /* B.1.2.2   Character Strings */
    /*******************************/
    void *visit(double_byte_character_string_c *symbol);
    void *visit(single_byte_character_string_c *symbol);

    /***************************/
    /* B 1.2.3 - Time Literals */
    /***************************/
    /************************/
    /* B 1.2.3.1 - Duration */
    /************************/
    void *visit(duration_c *symbol);

    /************************************/
    /* B 1.2.3.2 - Time of day and Date */
    /************************************/
    void *visit(time_of_day_c *symbol);
    void *visit(date_c *symbol);
    void *visit(date_and_time_c *symbol);


    /**********************/
    /* B 1.3 - Data types */
    /**********************/
    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
//  void *visit(data_type_declaration_c *symbol);   /* Not required. already handled by iterator_visitor_c base class */
//  void *visit(type_declaration_list_c *symbol);   /* Not required. already handled by iterator_visitor_c base class */
    void *visit(simple_type_declaration_c *symbol);
    void *visit(simple_spec_init_c *symbol);
    void *visit(subrange_type_declaration_c *symbol);
    void *visit(subrange_spec_init_c *symbol);
//  void *visit(subrange_specification_c *symbol);
    void *visit(subrange_c *symbol);
    void *visit(enumerated_type_declaration_c *symbol);
    void *visit(enumerated_spec_init_c *symbol);
    void *visit(enumerated_value_list_c *symbol);
    void *visit(enumerated_value_c *symbol);
    void *visit(array_type_declaration_c *symbol);
    void *visit(array_spec_init_c *symbol);
//  void *visit(array_specification_c *symbol);     /* Not required. already handled by iterator_visitor_c base class */
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
    void *visit(direct_variable_c *symbol);

    /*************************************/
    /* B 1.4.2 - Multi-element variables */
    /*************************************/
    void *visit(array_variable_c *symbol);
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
    //     void *visit(elseif_statement_list_c *symbol);
    void *visit(elseif_statement_c *symbol);
    void *visit(case_statement_c *symbol);

    /********************************/
    /* B 3.2.4 Iteration Statements */
    /********************************/
    void *visit(for_statement_c *symbol);
    void *visit(while_statement_c *symbol);
    void *visit(repeat_statement_c *symbol);

}; // fill_candidate_datatypes_c











