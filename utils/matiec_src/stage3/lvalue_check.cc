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



/* Expressions on the left hand side of assignment statements have aditional restrictions on their datatype.
 * For example, they cannot be literals, CONSTANT type variables, function invocations, etc...
 * This class wil do those checks.
 * 
 * Note that assignment may also be done when passing variables to OUTPUT or IN_OUT function parameters,so we check those too.
 */



#include "lvalue_check.hh"

#define FIRST_(symbol1, symbol2) (((symbol1)->first_order < (symbol2)->first_order)   ? (symbol1) : (symbol2))
#define  LAST_(symbol1, symbol2) (((symbol1)->last_order  > (symbol2)->last_order)    ? (symbol1) : (symbol2))

#define STAGE3_ERROR(error_level, symbol1, symbol2, ...) {                                                                  \
  if (current_display_error_level >= error_level) {                                                                         \
    fprintf(stderr, "%s:%d-%d..%d-%d: error: ",                                                                             \
            FIRST_(symbol1,symbol2)->first_file, FIRST_(symbol1,symbol2)->first_line, FIRST_(symbol1,symbol2)->first_column,\
                                                 LAST_(symbol1,symbol2) ->last_line,  LAST_(symbol1,symbol2) ->last_column);\
    fprintf(stderr, __VA_ARGS__);                                                                                           \
    fprintf(stderr, "\n");                                                                                                  \
    error_count++;                                                                                                     \
  }                                                                                                                         \
}


#define STAGE3_WARNING(symbol1, symbol2, ...) {                                                                             \
    fprintf(stderr, "%s:%d-%d..%d-%d: warning: ",                                                                           \
            FIRST_(symbol1,symbol2)->first_file, FIRST_(symbol1,symbol2)->first_line, FIRST_(symbol1,symbol2)->first_column,\
                                                 LAST_(symbol1,symbol2) ->last_line,  LAST_(symbol1,symbol2) ->last_column);\
    fprintf(stderr, __VA_ARGS__);                                                                                           \
    fprintf(stderr, "\n");                                                                                                  \
    warning_found = true;                                                                                                   \
}


lvalue_check_c::lvalue_check_c(symbol_c *ignore) {
	error_count = 0;
	current_display_error_level = 0;
	current_il_operand = NULL;
	search_varfb_instance_type = NULL;
	search_var_instance_decl = NULL;
}

lvalue_check_c::~lvalue_check_c(void) {
}

int lvalue_check_c::get_error_count() {
	return error_count;
}


#include <strings.h>
/* No writing to iterator variables (used in FOR loops) inside the loop itself */
void lvalue_check_c::check_assignment_to_controlvar(symbol_c *lvalue) {
	for (unsigned int i = 0; i < control_variables.size(); i++) {
		token_c *lvalue_name = get_var_name_c::get_name(lvalue);
		if (compare_identifiers(lvalue_name, control_variables[i]) == 0) {
			STAGE3_ERROR(0, lvalue, lvalue, "Assignment to FOR control variable is not allowed.");
			break;
		}
	}
}


/* fb_instance.var := ...  is not valid if var is output variable */
/* NOTE, if a fb_instance1.fb_instance2.fb_instance3.var is used, we must iteratively check that none of the 
 *       FB records are declared as OUTPUT variables!!  
 *       This is the reason why we have the while() loop in this function!
 * 
 *       Note, however, that the first record (fb_instance1 in the above example) may be an output variable!
 */
void lvalue_check_c::check_assignment_to_output(symbol_c *lvalue) {
	decompose_var_instance_name_c decompose_lvalue(lvalue);

	/* Get the first element/record of the potentially structured variable symbol */
	/* Note that if symbol is pointing to an expression (or simply a literal value), it will return a NULL.
	 * Once we have implemented the check_assignment_to_expression() method, and abort calling the other checks (including this one)
	 * when an expression is found, we may replace this check with an assertion...
	 * if (NULL == struct_elem) ERROR;
	 */
	symbol_c *struct_elem = decompose_lvalue.get_next();
	if (NULL == struct_elem) return;
	
	symbol_c *type_decl   = search_var_instance_decl->get_decl(struct_elem);
	// symbol_c *type_id  = spec_init_sperator_c::get_spec(type_decl); /* this is not required! search_base_type_c can handle spec_init symbols! */
	symbol_c *basetype_id = search_base_type_c::get_basetype_id(/*type_id*/ type_decl);
	/* If we can not determine the data type of the element, then the code must have a data type semantic error.
	 * This will have been caught by the data type semantic verifier, so we do not bother with this anymore!
	 */
	if (NULL == basetype_id) return;

	/* Determine if the record/structure element is of a FB type. */
	/* NOTE: If the structure element is not a FB type, then we can quit this check.
	 *       Remember that the standard does not allow a STRUCT data type to have elements that are FB instances!
	 *       Similarly, arrays of FB instances is also not allowed.
	 *       So, as soon as we find one record/structure element that is not a FB, no other record/structure element
	 *       will be of FB type, which means we can quit this check!
	 */
	function_block_type_symtable_t::iterator iter = function_block_type_symtable.find(basetype_id);
	if (function_block_type_symtable.end() == iter) return;
	function_block_declaration_c *fb_decl = iter->second;

	while (NULL != (struct_elem = decompose_lvalue.get_next())) {
		search_var_instance_decl_c   fb_search_var_instance_decl(fb_decl);
		if (search_var_instance_decl_c::output_vt == fb_search_var_instance_decl.get_vartype(struct_elem)) {
			STAGE3_ERROR(0, struct_elem, struct_elem, "Assignment to FB output variable is not allowed.");
			return; /* no need to carry on checking once the first error is found! */
		}

		/* prepare for any possible further record/structure elements */
		type_decl   = fb_search_var_instance_decl.get_decl(struct_elem);
		basetype_id = search_base_type_c::get_basetype_id(type_decl);
		if (NULL == basetype_id) return; /* same comment as above... */

		iter = function_block_type_symtable.find(basetype_id);
		if (function_block_type_symtable.end() == iter) return; /* same comment as above... */
		fb_decl = iter->second;  
	}
}


/*  No writing to CONSTANTs */
void lvalue_check_c::check_assignment_to_constant(symbol_c *lvalue) {
	unsigned int option = search_var_instance_decl->get_option(lvalue);
	if (option == search_var_instance_decl_c::constant_opt) {
		STAGE3_ERROR(0, lvalue, lvalue, "Assignment to CONSTANT variables is not allowed.");
	}
}


/*  No assigning values to expressions. */
void lvalue_check_c::check_assignment_to_expression(symbol_c *lvalue) {
	/* This may occur in function invocations, when passing values (possibly an expression) to one
	 * of the function's OUTPUT or IN_OUT parameters.
	 */
	if ( 
	     /*********************/
	     /* B 1.2 - Constants */
	     /*********************/
	     /*********************************/
	     /* B 1.2.XX - Reference Literals */
	     /*********************************/
	     (typeid( *lvalue ) == typeid( ref_value_null_literal_c       )) || /* defined in IEC 61131-3 v3 - Basically the 'NULL' keyword! */
	     /******************************/
	     /* B 1.2.1 - Numeric Literals */
	     /******************************/
	     (typeid( *lvalue ) == typeid( real_c                         )) ||
	     (typeid( *lvalue ) == typeid( integer_c                      )) ||
	     (typeid( *lvalue ) == typeid( binary_integer_c               )) ||
	     (typeid( *lvalue ) == typeid( octal_integer_c                )) ||
	     (typeid( *lvalue ) == typeid( hex_integer_c                  )) ||
	     (typeid( *lvalue ) == typeid( neg_real_c                     )) ||
	     (typeid( *lvalue ) == typeid( neg_integer_c                  )) ||
	     (typeid( *lvalue ) == typeid( integer_literal_c              )) ||
	     (typeid( *lvalue ) == typeid( real_literal_c                 )) ||
	     (typeid( *lvalue ) == typeid( bit_string_literal_c           )) ||
	     (typeid( *lvalue ) == typeid( boolean_literal_c              )) ||
	     (typeid( *lvalue ) == typeid( boolean_true_c                 )) || /* should not really be needed */
	     (typeid( *lvalue ) == typeid( boolean_false_c                )) || /* should not really be needed */
	     /*******************************/
	     /* B.1.2.2   Character Strings */
	     /*******************************/
	     (typeid( *lvalue ) == typeid( double_byte_character_string_c )) ||
	     (typeid( *lvalue ) == typeid( single_byte_character_string_c )) ||
	     /***************************/
	     /* B 1.2.3 - Time Literals */
	     /***************************/
	     /************************/
	     /* B 1.2.3.1 - Duration */
	     /************************/
	     (typeid( *lvalue ) == typeid( duration_c                     )) ||
	     /************************************/
	     /* B 1.2.3.2 - Time of day and Date */
	     /************************************/
	     (typeid( *lvalue ) == typeid( time_of_day_c                  )) ||
	     (typeid( *lvalue ) == typeid( daytime_c                      )) || /* should not really be needed */
	     (typeid( *lvalue ) == typeid( date_c                         )) || /* should not really be needed */
	     (typeid( *lvalue ) == typeid( date_literal_c                 )) ||
	     (typeid( *lvalue ) == typeid( date_and_time_c                )) ||
	     /***************************************/
	     /* B.3 - Language ST (Structured Text) */
	     /***************************************/
	     /***********************/
	     /* B 3.1 - Expressions */
	     /***********************/
	     (typeid( *lvalue ) == typeid( ref_expression_c               )) || /* an extension to the IEC 61131-3 standard - based on the IEC 61131-3 v3 standard. Returns address of the variable! */
	     (typeid( *lvalue ) == typeid( or_expression_c                )) ||
	     (typeid( *lvalue ) == typeid( xor_expression_c               )) ||
	     (typeid( *lvalue ) == typeid( and_expression_c               )) ||
	     (typeid( *lvalue ) == typeid( equ_expression_c               )) ||
	     (typeid( *lvalue ) == typeid( notequ_expression_c            )) ||
	     (typeid( *lvalue ) == typeid( lt_expression_c                )) ||
	     (typeid( *lvalue ) == typeid( gt_expression_c                )) ||
	     (typeid( *lvalue ) == typeid( le_expression_c                )) ||
	     (typeid( *lvalue ) == typeid( ge_expression_c                )) ||
	     (typeid( *lvalue ) == typeid( add_expression_c               )) ||
	     (typeid( *lvalue ) == typeid( sub_expression_c               )) ||
	     (typeid( *lvalue ) == typeid( mul_expression_c               )) ||
	     (typeid( *lvalue ) == typeid( div_expression_c               )) ||
	     (typeid( *lvalue ) == typeid( mod_expression_c               )) ||
	     (typeid( *lvalue ) == typeid( power_expression_c             )) ||
	     (typeid( *lvalue ) == typeid( neg_expression_c               )) ||
	     (typeid( *lvalue ) == typeid( not_expression_c               )) ||
	     (typeid( *lvalue ) == typeid( function_invocation_c          )))
		STAGE3_ERROR(0, lvalue, lvalue, "Assignment to an expression or a literal value is not allowed.");
}                                                                  



/*  No assigning values to IL lists. */
void lvalue_check_c::check_assignment_to_il_list(symbol_c *lvalue) {
	/* This may occur in formal invocations in IL, where an embedded IL list may be used instead of a symbolic_variable
	 * when passing an IN_OUT parameter! Note that it will never occur to OUT parameters, as the syntax does not allow it,
	 * although this does not affect out algorithm here!
	 */
	if ( 
	     /****************************************/
	     /* B.2 - Language IL (Instruction List) */
	     /****************************************/
	     /***********************************/
	     /* B 2.1 Instructions and Operands */
	     /***********************************/
	     (typeid( *lvalue ) == typeid( simple_instr_list_c)))
		STAGE3_ERROR(0, lvalue, lvalue, "Assigning an IL list to an IN_OUT parameter is not allowed.");
}                                                                  




void lvalue_check_c::verify_is_lvalue(symbol_c *lvalue) {
	if (NULL == lvalue) return; // missing operand in source code being compiled. Error will be caught and reported by datatype checking!
	int init_error_count = error_count;  /* stop the checks once an error has been found... */
	if (error_count == init_error_count)  check_assignment_to_expression(lvalue);
	if (error_count == init_error_count)  check_assignment_to_il_list   (lvalue);
	if (error_count == init_error_count)  check_assignment_to_controlvar(lvalue);
	if (error_count == init_error_count)  check_assignment_to_output    (lvalue);
	if (error_count == init_error_count)  check_assignment_to_constant  (lvalue);
}




/* check whether all values passed to OUT or IN_OUT parameters are legal lvalues. */
/*
 * All parameters being passed to the called function MUST be in the parameter list to which f_call points to!
 * This means that, for non formal function calls in IL, de current (default value) must be artificially added to the
 * beginning of the parameter list BEFORE calling handle_function_call().
 */
#include <string.h> /* required for strcmp() */
void lvalue_check_c::check_nonformal_call(symbol_c *f_call, symbol_c *f_decl) {
	/* if data type semantic verification was unable to determine which function is being called,
	 * then it does not make sense to go ahead and check for lvalues to unknown parameters.
	 * We simply bug out!
	 */
	if (NULL == f_decl) return;
	
	symbol_c *call_param_value;
	identifier_c *param_name;
	function_param_iterator_c       fp_iterator(f_decl);
	function_call_param_iterator_c fcp_iterator(f_call);

	/* Iterating through the non-formal parameters of the function call */
	while((call_param_value = fcp_iterator.next_nf()) != NULL) {
		/* Iterate to the next parameter of the function being called.
		 * Get the name of that parameter, and ignore if EN or ENO.
		 */
		do {
			param_name = fp_iterator.next();
			/* If there is no other parameter declared, then we are passing too many parameters... */
			/* This error should have been caught in data type verification, so we simply abandon our check! */
			if(param_name == NULL) return;
		} while ((strcmp(param_name->value, "EN") == 0) || (strcmp(param_name->value, "ENO") == 0));

		/* Determine the direction (IN, OUT, IN_OUT) of the parameter... */
		function_param_iterator_c::param_direction_t param_direction = fp_iterator.param_direction();
		
		/* We only process the parameter value if the paramater itself is valid... */
		if (param_name != NULL) {
			/* If the parameter is either OUT or IN_OUT, we check if 'call_param_value' is a valid lvalue */
			if ((function_param_iterator_c::direction_out == param_direction) || (function_param_iterator_c::direction_inout == param_direction)) 
				verify_is_lvalue(call_param_value);
			/* parameter values to IN  parameters may be expressions with function invocations that must also be checked! */
			/* parameter values to OUT or IN_OUT parameters may contain arrays, whose subscripts contain expressions that must be checked! */
			call_param_value->accept(*this);  
		}
	}
}





  
/* check whether all values passed to OUT or IN_OUT parameters are legal lvalues. */
void lvalue_check_c::check_formal_call(symbol_c *f_call, symbol_c *f_decl) {
	/* if data type semantic verification was unable to determine which function is being called,
	 * then it does not make sense to go ahead and check for lvalues to unknown parameters.
	 * We simply bug out!
	 */
	if (NULL == f_decl) return;
	
	symbol_c *call_param_name;
	function_param_iterator_c       fp_iterator(f_decl);
	function_call_param_iterator_c fcp_iterator(f_call);

	/* Iterating through the formal parameters of the function call */
	while((call_param_name = fcp_iterator.next_f()) != NULL) {

		/* Obtaining the value being passed in the function call */
		symbol_c *call_param_value = fcp_iterator.get_current_value();
		if (NULL == call_param_value) ERROR;

		/* Find the corresponding parameter in function declaration, and it's direction (IN, OUT, IN_OUT) */
		identifier_c *param_name = fp_iterator.search(call_param_name);
		function_param_iterator_c::param_direction_t param_direction = fp_iterator.param_direction();
		
		/* We only process the parameter value if the paramater itself is valid... */
		if (param_name != NULL) {
			/* If the parameter is either OUT or IN_OUT, we check if 'call_param_value' is a valid lvalue */
			if ((function_param_iterator_c::direction_out == param_direction) || (function_param_iterator_c::direction_inout == param_direction)) 
				verify_is_lvalue(call_param_value);
			/* parameter values to IN  parameters may be expressions with function invocations that must also be checked! */
			/* parameter values to OUT or IN_OUT parameters may contain arrays, whose subscripts contain expressions that must be checked! */
			call_param_value->accept(*this);  
 		}
	}
}







/**************************************/
/* B 1.5 - Program organisation units */
/**************************************/
/***********************/
/* B 1.5.1 - Functions */
/***********************/
void *lvalue_check_c::visit(function_declaration_c *symbol) {
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
	search_var_instance_decl = new search_var_instance_decl_c(symbol);
	symbol->function_body->accept(*this);
	delete search_varfb_instance_type;
	delete search_var_instance_decl;
	search_varfb_instance_type = NULL;
	search_var_instance_decl = NULL;
	return NULL;
}

/*****************************/
/* B 1.5.2 - Function blocks */
/*****************************/
void *lvalue_check_c::visit(function_block_declaration_c *symbol) {
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
	search_var_instance_decl = new search_var_instance_decl_c(symbol);
	symbol->fblock_body->accept(*this);
	delete search_varfb_instance_type;
	delete search_var_instance_decl;
	search_varfb_instance_type = NULL;
	search_var_instance_decl = NULL;
	return NULL;
}

/**********************/
/* B 1.5.3 - Programs */
/**********************/
void *lvalue_check_c::visit(program_declaration_c *symbol) {
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
	search_var_instance_decl = new search_var_instance_decl_c(symbol);
	symbol->function_block_body->accept(*this);
	delete search_varfb_instance_type;
	delete search_var_instance_decl;
	search_varfb_instance_type = NULL;
	search_var_instance_decl = NULL;
	return NULL;
}

/****************************************/
/* B.2 - Language IL (Instruction List) */
/****************************************/
/***********************************/
/* B 2.1 Instructions and Operands */
/***********************************/
void *lvalue_check_c::visit(il_instruction_c *symbol) {
	/* il_instruction will be NULL when parsing a label with no instruction
	 * e.g.:   label1:          <---- il_instruction = NULL!
	 *                 LD 33
	 *                 ...
	 */
	if (NULL != symbol->il_instruction)
		symbol->il_instruction->accept(*this);
	return NULL;
}

void *lvalue_check_c::visit(il_simple_operation_c *symbol) {
	current_il_operand = symbol->il_operand;
	symbol->il_simple_operator->accept(*this);
	current_il_operand = NULL;
	return NULL;
}




/* | function_name [il_operand_list] */
/* NOTE: The parameters 'called_function_declaration' and 'extensible_param_count' are used to pass data between the stage 3 and stage 4. */
// SYM_REF2(il_function_call_c, function_name, il_operand_list, symbol_c *called_function_declaration; int extensible_param_count;)
void *lvalue_check_c::visit(il_function_call_c *symbol) {
	/* The first parameter of a non formal function call in IL will be the 'current value' (i.e. the prev_il_instruction)
	 * In order to be able to handle this without coding special cases, we will simply prepend that symbol
	 * to the il_operand_list, and remove it after calling handle_function_call().
	 *
	 * However, if no further paramters are given, then il_operand_list will be NULL, and we will
	 * need to create a new object to hold the pointer to prev_il_instruction.
	 * This change will also be undone at the end of this method.
	 */
	/* TODO: Copying the location data will result in confusing error message. 
	 *       We need to make this better, by inserting code to handle this special situation explicitly!
	 */
	/* NOTE: When calling a function, using the 'current value' as the first parameter of the function invocation
	 *       implies that we can only call functions whose first parameter is IN. It would not do to pass
	 *       the 'current value' to an OUT or IN_OUT parameter.
	 *       In order to make sure that this will be caught by the check_nonformal_call() function,
	 *       we add a symbol that cannot be an lvalue; in this case, a real_c (REAL literal).
	 */
	real_c param_value(NULL);
	*((symbol_c *)(&param_value)) = *((symbol_c *)symbol); /* copy the symbol location (file, line, offset) data */
	if (NULL == symbol->il_operand_list)  symbol->il_operand_list = new il_operand_list_c;
	if (NULL == symbol->il_operand_list)  ERROR;
	((list_c *)symbol->il_operand_list)->insert_element(&param_value, 0);

	check_nonformal_call(symbol, symbol->called_function_declaration);

	/* Undo the changes to the abstract syntax tree we made above... */
	((list_c *)symbol->il_operand_list)->remove_element(0);
	if (((list_c *)symbol->il_operand_list)->n == 0) {
		/* if the list becomes empty, then that means that it did not exist before we made these changes, so we delete it! */
		delete 	symbol->il_operand_list;
		symbol->il_operand_list = NULL;
	}

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
void *lvalue_check_c::visit(il_fb_call_c *symbol) {
	if (NULL != symbol->il_operand_list)  check_nonformal_call(symbol, symbol->called_fb_declaration);
	if (NULL != symbol->  il_param_list)     check_formal_call(symbol, symbol->called_fb_declaration);
	return NULL;
}


/* | function_name '(' eol_list [il_param_list] ')' */
/* NOTE: The parameter 'called_function_declaration' is used to pass data between the stage 3 and stage 4. */
// SYM_REF2(il_formal_funct_call_c, function_name, il_param_list, symbol_c *called_function_declaration; int extensible_param_count;)
void *lvalue_check_c::visit(il_formal_funct_call_c *symbol) {
	check_formal_call(symbol, symbol->called_function_declaration);
	return NULL;
}






/*******************/
/* B 2.2 Operators */
/*******************/
void *lvalue_check_c::visit(ST_operator_c *symbol) {
	verify_is_lvalue(current_il_operand);
	return NULL;
}

void *lvalue_check_c::visit(STN_operator_c *symbol) {
	verify_is_lvalue(current_il_operand);
	return NULL;
}

void *lvalue_check_c::visit(S_operator_c *symbol) {
	verify_is_lvalue(current_il_operand);
	return NULL;
}

void *lvalue_check_c::visit(R_operator_c *symbol) {
	verify_is_lvalue(current_il_operand);
	return NULL;
}


/***************************************/
/* B.3 - Language ST (Structured Text) */
/***************************************/
/***********************/
/* B 3.1 - Expressions */
/***********************/
// SYM_REF3(function_invocation_c, function_name, formal_param_list, nonformal_param_list, symbol_c *called_function_declaration; int extensible_param_count; std::vector <symbol_c *> candidate_functions;)
void *lvalue_check_c::visit(function_invocation_c *symbol) {
	if (NULL != symbol->formal_param_list   )  check_formal_call   (symbol, symbol->called_function_declaration);
	if (NULL != symbol->nonformal_param_list)  check_nonformal_call(symbol, symbol->called_function_declaration);
	return NULL;
}

/*********************************/
/* B 3.2.1 Assignment Statements */
/*********************************/
void *lvalue_check_c::visit(assignment_statement_c *symbol) {
	verify_is_lvalue(symbol->l_exp);
	/* We call visit r_exp to check function_call */
	symbol->r_exp->accept(*this);
	return NULL;
}

/*****************************************/
/* B 3.2.2 Subprogram Control Statements */
/*****************************************/
void *lvalue_check_c::visit(fb_invocation_c *symbol) {
	if (NULL != symbol->formal_param_list   )  check_formal_call   (symbol, symbol->called_fb_declaration);
	if (NULL != symbol->nonformal_param_list)  check_nonformal_call(symbol, symbol->called_fb_declaration);
	return NULL;
}

/********************************/
/* B 3.2.4 Iteration Statements */
/********************************/
void *lvalue_check_c::visit(for_statement_c *symbol) {
        verify_is_lvalue(symbol->control_variable);
	control_variables.push_back(get_var_name_c::get_name(symbol->control_variable));
	symbol->statement_list->accept(*this);
	control_variables.pop_back();
	return NULL;
}







