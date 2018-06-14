/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2012  Mario de Sousa (msousa@fe.up.pt)
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
 *
 *  Since IL code can contain jumps (JMP), the same il_instruction may effectively have
 *  several previous il_instructions. In order to accommodate this, each il_instruction
 *  will maintain a vector (i..e an array) of pointers to all the previous il_instructions.
 *  We do however attempt to guarantee that the first element in the vector (array) will preferentially
 *  point to the il instruction that is right before / imediately preceding the current il instructions, 
 *  i.e. the first element in the array will tend to point to the previous il_instruction
 *  that is not a jump JMP IL instruction!
 *
 *  The result will essentially be a graph of il_instruction_c objects, each 
 *  pointing to the previous il_instruction_c object.
 *
 *  The reality is we will get several independent and isolated linked lists
 *  (actually, since we now process labels correctly, this is really a graph):
 *  one for each block of IL code (e.g. inside a Function, FB or Program).
 *  Additionally, when the IL code has an expression (expression_c object), we will actually
 *  have one more isolated linked list for the IL code inside that expression.
 *
 *  e.g.
 *       line_1:   LD 1
 *       line_2:   ADD (42
 *       line_3:        ADD B
 *       line_4:        ADD C 
 *       line_5:       )
 *       line_6:   ADD D
 *       line_7:   ST  E
 * 
 *     will result in two independent linked lists:
 *       main list:  line_7 -> line_6 -> line2 -> line_1
 *       expr list:  lin4_4 -> line_3 -> (operand of line_2, i.e. '42')
 * 
 * 
 *     In the main list, each:
 *        line_x: IL_operation IL_operand
 *      is encoded as
 *          il_instruction_c(label, il_incomplete_instruction)
 *      these il_instruction_c objects will point back to the previous il_instruction_c object.
 *
 *     In the expr list, each 
 *        line_x:        IL_operation IL_operand
 *      is encoded as
 *          il_simple_instruction_c(il_simple_instruction)
 *      these il_simple_instruction_c objects will point back to the previous il_simple_instruction_c object,
 *      except the for the first il_simple_instruction_c object in the list, which will point back to
 *      the first il_operand (in the above example, '42'), or NULL is it does not exist.
 *          
 *
 * label:
 *   identifier_c  
 *   
 * il_incomplete_instruction:
 *   il_simple_operation   (il_simple_operation_c, il_function_call_c)
 * | il_expression         (il_expression_c)
 * | il_jump_operation     (il_jump_operation_c)
 * | il_fb_call            (il_fb_call_c)
 * | il_formal_funct_call  (il_formal_funct_call_c)
 * | il_return_operator    (RET_operator_c, RETC_operator_c, RETCN_operator_c)
 *  
 * 
 * il_expression_c(il_expr_operator, il_operand, simple_instr_list)
 * 
 * il_operand:
 *   variable            (symbolic_variable_c, direct_variable_c, array_variable_c, structured_variable_c)  
 * | enumerated_value    (enumerated_value_c)
 * | constant            (lots of literal classes _c)
 * 
 * simple_instr_list:
 *   list of il_simple_instruction
 * 
 * il_simple_instruction:
 *   il_simple_operation       (il_simple_operation_c, il_function_call_c)
 * | il_expression             (il_expression_c)
 * | il_formal_funct_call      (il_formal_funct_call_c)
 * 
 */

#include "flow_control_analysis.hh"



/* set to 1 to see debug info during execution */
static int debug = 0;

flow_control_analysis_c::flow_control_analysis_c(symbol_c *ignore) {
  prev_il_instruction = NULL;
  curr_il_instruction = NULL;
  prev_il_instruction_is_JMP_or_RET = false;
  search_il_label = NULL;
}

flow_control_analysis_c::~flow_control_analysis_c(void) {
}


void flow_control_analysis_c::link_insert(symbol_c *prev_instruction, symbol_c *next_instruction) {
	il_instruction_c        *next_a = dynamic_cast<il_instruction_c        *>(next_instruction);
	il_instruction_c        *prev_a = dynamic_cast<il_instruction_c        *>(prev_instruction);
	il_simple_instruction_c *next_b = dynamic_cast<il_simple_instruction_c *>(next_instruction);
	il_simple_instruction_c *prev_b = dynamic_cast<il_simple_instruction_c *>(prev_instruction);
	
	if       (NULL != next_a)  next_a->prev_il_instruction.insert(next_a->prev_il_instruction.begin(), prev_instruction);
	else if  (NULL != next_b)  next_b->prev_il_instruction.insert(next_b->prev_il_instruction.begin(), prev_instruction);
	else ERROR;
	
	if       (NULL != prev_a)  prev_a->next_il_instruction.insert(prev_a->next_il_instruction.begin(), next_instruction);
	else if  (NULL != prev_b)  prev_b->next_il_instruction.insert(prev_b->next_il_instruction.begin(), next_instruction);
	else ERROR;
}


void flow_control_analysis_c::link_pushback(symbol_c *prev_instruction, symbol_c *next_instruction) {
	il_instruction_c *next = dynamic_cast<il_instruction_c *>(next_instruction);
	il_instruction_c *prev = dynamic_cast<il_instruction_c *>(prev_instruction);
	if ((NULL == next) || (NULL == prev)) ERROR;

	next->prev_il_instruction.push_back(prev);
	prev->next_il_instruction.push_back(next);
}


/************************************/
/* B 1.5 Program organization units */
/************************************/
/*********************/
/* B 1.5.1 Functions */
/*********************/
void *flow_control_analysis_c::visit(function_declaration_c *symbol) {
	search_il_label = new search_il_label_c(symbol);
	if (debug) printf("Doing flow control analysis in body of function %s\n", ((token_c *)(symbol->derived_function_name))->value);
	symbol->function_body->accept(*this);
	delete search_il_label;
	search_il_label = NULL;
	return NULL;
}

/***************************/
/* B 1.5.2 Function blocks */
/***************************/
void *flow_control_analysis_c::visit(function_block_declaration_c *symbol) {
	search_il_label = new search_il_label_c(symbol);
	if (debug) printf("Doing flow control analysis in body of FB %s\n", ((token_c *)(symbol->fblock_name))->value);
	symbol->fblock_body->accept(*this);
	delete search_il_label;
	search_il_label = NULL;
	return NULL;
}

/********************/
/* B 1.5.3 Programs */
/********************/
void *flow_control_analysis_c::visit(program_declaration_c *symbol) {
	search_il_label = new search_il_label_c(symbol);
	if (debug) printf("Doing flow control analysis in body of program %s\n", ((token_c *)(symbol->program_type_name))->value);
	symbol->function_block_body->accept(*this);
	delete search_il_label;
	search_il_label = NULL;
	return NULL;
}


/********************************/
/* B 1.7 Configuration elements */
/********************************/
void *flow_control_analysis_c::visit(configuration_declaration_c *symbol) {
	return NULL;
}


/****************************************/
/* B.2 - Language IL (Instruction List) */
/****************************************/
/***********************************/
/* B 2.1 Instructions and Operands */
/***********************************/

/*| instruction_list il_instruction */
// SYM_LIST(instruction_list_c)
void *flow_control_analysis_c::visit(instruction_list_c *symbol) {
	prev_il_instruction_is_JMP_or_RET = false;
	for(int i = 0; i < symbol->n; i++) {
		prev_il_instruction = NULL;
		if (i > 0) prev_il_instruction = symbol->elements[i-1];
		curr_il_instruction = symbol->elements[i];
		curr_il_instruction->accept(*this);
		curr_il_instruction = NULL;
	}
	return NULL;
}

/* | label ':' [il_incomplete_instruction] eol_list */
// SYM_REF2(il_instruction_c, label, il_instruction)
// void *visit(instruction_list_c *symbol);
void *flow_control_analysis_c::visit(il_instruction_c *symbol) {
	if ((NULL != prev_il_instruction) && (!prev_il_instruction_is_JMP_or_RET))
		/* We try to guarantee that the previous il instruction that is in the previous line, will occupy the first element of the vector.
		 * In order to do that, we use insert() instead of push_back()
		 */
		link_insert(prev_il_instruction, symbol);

	/* check if it is an il_expression_c, a JMP[C[N]], or a RET, and if so, handle it correctly */
	prev_il_instruction_is_JMP_or_RET = false;
	if (NULL != symbol->il_instruction)
		symbol->il_instruction->accept(*this);
	return NULL;
}



/* | il_simple_operator [il_operand] */
// SYM_REF2(il_simple_operation_c, il_simple_operator, il_operand)
// void *flow_control_analysis_c::visit(il_simple_operation_c *symbol)



/* | function_name [il_operand_list] */
/* NOTE: The parameters 'called_function_declaration' and 'extensible_param_count' are used to pass data between the stage 3 and stage 4. */
// SYM_REF2(il_function_call_c, function_name, il_operand_list, symbol_c *called_function_declaration; int extensible_param_count;)
// void *flow_control_analysis_c::visit(il_function_call_c *symbol) 


/* | il_expr_operator '(' [il_operand] eol_list [simple_instr_list] ')' */
// SYM_REF3(il_expression_c, il_expr_operator, il_operand, simple_instr_list);
void *flow_control_analysis_c::visit(il_expression_c *symbol) {
	if(NULL == symbol->simple_instr_list) 
		/* nothing to do... */
		return NULL;
  
	symbol_c *save_prev_il_instruction = prev_il_instruction;
	/* Stage2 will insert an artificial (and equivalent) LD <il_operand> to the simple_instr_list if necessary. We can therefore ignore the 'il_operand' entry! */
	// prev_il_instruction = symbol->il_operand;
	prev_il_instruction = NULL;
	symbol->simple_instr_list->accept(*this);
	prev_il_instruction = save_prev_il_instruction;
	return NULL;
}


/*  il_jump_operator label */
// SYM_REF2(il_jump_operation_c, il_jump_operator, label)
void *flow_control_analysis_c::visit(il_jump_operation_c *symbol) {
  /* search for the il_instruction_c containing the label */
  il_instruction_c *destination = search_il_label->find_label(symbol->label);

  /* give the visit(JMP_operator *) an oportunity to set the prev_il_instruction_is_JMP_or_RET flag! */
  symbol->il_jump_operator->accept(*this);
  if (NULL != destination)
    link_pushback(curr_il_instruction, destination);
  return NULL;
}


/*   il_call_operator prev_declared_fb_name
 * | il_call_operator prev_declared_fb_name '(' ')'
 * | il_call_operator prev_declared_fb_name '(' eol_list ')'
 * | il_call_operator prev_declared_fb_name '(' il_operand_list ')'
 * | il_call_operator prev_declared_fb_name '(' eol_list il_param_list ')'
 */
/* NOTE: The parameter 'called_fb_declaration'is used to pass data between stage 3 and stage4 (although currently it is not used in stage 4 */
// SYM_REF4(il_fb_call_c, il_call_operator, fb_name, il_operand_list, il_param_list, symbol_c *called_fb_declaration)
// void *flow_control_analysis_c::visit(il_fb_call_c *symbol)


/* | function_name '(' eol_list [il_param_list] ')' */
/* NOTE: The parameter 'called_function_declaration' is used to pass data between the stage 3 and stage 4. */
// SYM_REF2(il_formal_funct_call_c, function_name, il_param_list, symbol_c *called_function_declaration; int extensible_param_count;)
// void *flow_control_analysis_c::visit(il_formal_funct_call_c *symbol)



//  void *visit(il_operand_list_c *symbol);
void *flow_control_analysis_c::visit(simple_instr_list_c *symbol) {
	for(int i = 0; i < symbol->n; i++) {
		/* The prev_il_instruction for element[0] was set in visit(il_expression_c *) */
		if (i>0) prev_il_instruction = symbol->elements[i-1];
		symbol->elements[i]->accept(*this);
	}
	return NULL;
}


// SYM_REF1(il_simple_instruction_c, il_simple_instruction, symbol_c *prev_il_instruction;)
void *flow_control_analysis_c::visit(il_simple_instruction_c*symbol) {
	if (NULL != prev_il_instruction)
		/* We try to guarantee that the previous il instruction that is in the previous line, will occupy the first element of the vector.
		 * In order to do that, we use insert() instead of push_back()
		 */
		link_insert(prev_il_instruction, symbol);

	return NULL;
}


/*
    void *visit(il_param_list_c *symbol);
    void *visit(il_param_assignment_c *symbol);
    void *visit(il_param_out_assignment_c *symbol);
 */




/*******************/
/* B 2.2 Operators */
/*******************/
// void *visit(   LD_operator_c *symbol);
// void *visit(  LDN_operator_c *symbol);
// void *visit(   ST_operator_c *symbol);
// void *visit(  STN_operator_c *symbol);
// void *visit(  NOT_operator_c *symbol);
// void *visit(    S_operator_c *symbol);
// void *visit(    R_operator_c *symbol);
// void *visit(   S1_operator_c *symbol);
// void *visit(   R1_operator_c *symbol);
// void *visit(  CLK_operator_c *symbol);
// void *visit(   CU_operator_c *symbol);
// void *visit(   CD_operator_c *symbol);
// void *visit(   PV_operator_c *symbol);
// void *visit(   IN_operator_c *symbol);
// void *visit(   PT_operator_c *symbol);
// void *visit(  AND_operator_c *symbol);
// void *visit(   OR_operator_c *symbol);
// void *visit(  XOR_operator_c *symbol);
// void *visit( ANDN_operator_c *symbol);
// void *visit(  ORN_operator_c *symbol);
// void *visit( XORN_operator_c *symbol);
// void *visit(  ADD_operator_c *symbol);
// void *visit(  SUB_operator_c *symbol);
// void *visit(  MUL_operator_c *symbol);
// void *visit(  DIV_operator_c *symbol);
// void *visit(  MOD_operator_c *symbol);
// void *visit(   GT_operator_c *symbol);
// void *visit(   GE_operator_c *symbol);
// void *visit(   EQ_operator_c *symbol);
// void *visit(   LT_operator_c *symbol);
// void *visit(   LE_operator_c *symbol);
// void *visit(   NE_operator_c *symbol);
// void *visit(  CAL_operator_c *symbol);
// void *visit( CALC_operator_c *symbol);
// void *visit(CALCN_operator_c *symbol);

/* this next visit function will be called directly from visit(il_instruction_c *) */
void *flow_control_analysis_c::visit(  RET_operator_c *symbol) {
	prev_il_instruction_is_JMP_or_RET = true;
	return NULL;
}

// void *visit( RETC_operator_c *symbol);
// void *visit(RETCN_operator_c *symbol);

/* this next visit function will be called from visit(il_jump_operation_c *) */
void *flow_control_analysis_c::visit(  JMP_operator_c *symbol) {
	prev_il_instruction_is_JMP_or_RET = true;
	return NULL;
}

// void *visit( JMPC_operator_c *symbol);
// void *visit(JMPCN_operator_c *symbol);

/* Symbol class handled together with function call checks */
// void *visit(il_assign_operator_c *symbol, variable_name);
/* Symbol class handled together with function call checks */
// void *visit(il_assign_operator_c *symbol, option, variable_name);

