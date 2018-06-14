/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2009-2012  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2012       Manuele Conti (manuele.conti@sirius-es.it)
 *  Copyright (C) 2012       Matteo Facchinetti (matteo.facchinetti@sirius-es.it)
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


#include "narrow_candidate_datatypes.hh"
#include "datatype_functions.hh"
#include <typeinfo>
#include <list>
#include <string>
#include <string.h>
#include <strings.h>


/* set to 1 to see debug info during execution */
static int debug = 0;

narrow_candidate_datatypes_c::narrow_candidate_datatypes_c(symbol_c *ignore) {
	search_varfb_instance_type = NULL;
	fake_prev_il_instruction = NULL;
	current_il_instruction   = NULL;
	il_operand = NULL;
}

narrow_candidate_datatypes_c::~narrow_candidate_datatypes_c(void) {
}


/* Only set the symbol's desired datatype to 'datatype' if that datatype is in the candidate_datatype list */
static void set_datatype(symbol_c *datatype, symbol_c *symbol) {
  
	/* If we are trying to set to the undefined type, and the symbol's datatype has already been set to something else, 
	 * we abort the compoiler as I don't think this should ever occur. 
	 * NOTE: In order to handle JMPs to labels that come before the JMP itself, we run the narrow algorithm twice.
	 *       This means that this situation may legally occur, so we cannot abort the compiler here!
	 */
// 	if ((NULL == datatype) && (NULL != symbol->datatype)) ERROR;
 	if ((NULL == datatype) && (NULL != symbol->datatype)) return;
	if ((NULL == datatype) && (NULL == symbol->datatype)) return;
	
	if (search_in_candidate_datatype_list(datatype, symbol->candidate_datatypes) < 0)
		symbol->datatype = &(get_datatype_info_c::invalid_type_name);   
	else {
		if (NULL == symbol->datatype)   
			/* not yet set to anything, so we set it to the requested data type */
			symbol->datatype = datatype; 
		else {
			/* had already been set previously to some data type. Let's check if they are the same! */
			if (!get_datatype_info_c::is_type_equal(symbol->datatype, datatype))
				symbol->datatype = &(get_datatype_info_c::invalid_type_name);
// 			else 
				/* we leave it unchanged, as it is the same as the requested data type! */
		}
	}
}



/* Only set the symbol's desired datatype to 'datatype' if that datatype is in the candidate_datatype list */
// static void set_datatype_in_prev_il_instructions(symbol_c *datatype, std::vector <symbol_c *> prev_il_instructions) {
static void set_datatype_in_prev_il_instructions(symbol_c *datatype, il_instruction_c *symbol) {
	if (NULL == symbol) ERROR;
	for (unsigned int i = 0; i < symbol->prev_il_instruction.size(); i++)
		set_datatype(datatype, symbol->prev_il_instruction[i]);
}



bool narrow_candidate_datatypes_c::is_widening_compatible(const struct widen_entry widen_table[], symbol_c *left_type, symbol_c *right_type, symbol_c *result_type, bool *deprecated_status) {
	/* NOTE: According to our algorithm, left_type and right_type should never by NULL (if they are, we have an internal compiler error!
	 *       However, result_type may be NULL if the code has a data type semantic error!
	 */
	if ((NULL == left_type) || (NULL == right_type) || (NULL == result_type))
		return false;

	for (int k = 0; NULL != widen_table[k].left;  k++) {
		if        ((typeid(*left_type)   == typeid(*widen_table[k].left))
		        && (typeid(*right_type)  == typeid(*widen_table[k].right))
			&& (typeid(*result_type) == typeid(*widen_table[k].result))) {
			if (NULL != deprecated_status)
				*deprecated_status = (widen_table[k].status == widen_entry::deprecated);
			return true;
		}
	}
	return false;
}

/*
 * All parameters being passed to the called function MUST be in the parameter list to which f_call points to!
 * This means that, for non formal function calls in IL, de current (default value) must be artificially added to the
 * beginning of the parameter list BEFORE calling handle_function_call().
 */
void narrow_candidate_datatypes_c::narrow_nonformal_call(symbol_c *f_call, symbol_c *f_decl, int *ext_parm_count) {
	symbol_c *call_param_value,  *param_type;
	identifier_c *param_name;
	function_param_iterator_c       fp_iterator(f_decl);
	function_call_param_iterator_c fcp_iterator(f_call);
	int extensible_parameter_highest_index = -1;
	unsigned int i;

	if (NULL != ext_parm_count) *ext_parm_count = -1;

	/* Iterating through the non-formal parameters of the function call */
	while((call_param_value = fcp_iterator.next_nf()) != NULL) {
		/* Obtaining the type of the value being passed in the function call */
		/* Iterate to the next parameter of the function being called.
		 * Get the name of that parameter, and ignore if EN or ENO.
		 */
		do {
			param_name = fp_iterator.next();
			/* If there is no other parameter declared, then we are passing too many parameters... */
			/* This error should have been caught in fill_candidate_datatypes_c, but may occur here again when we handle FB invocations! 
			 * In this case, we carry on analysing the code in order to be able to provide relevant error messages
			 * for that code too!
			 */
			if(param_name == NULL) break;
		} while ((strcmp(param_name->value, "EN") == 0) || (strcmp(param_name->value, "ENO") == 0));

		/* Set the desired datatype for this parameter, and call it recursively. */
		/* Note that if the call has more parameters than those declared in the function/FB declaration,
		 * we may be setting this to NULL!
		 */
		symbol_c *desired_datatype = base_type(fp_iterator.param_type());
		if ((NULL != param_name) && (NULL == desired_datatype)) ERROR;
		if ((NULL == param_name) && (NULL != desired_datatype)) ERROR;

		/* NOTE: When we are handling a nonformal function call made from IL, the first parameter is the 'default' or 'current'
		 *       il value. However, a pointer to a copy of the prev_il_instruction is pre-pended into the operand list, so 
		 *       the call 
		 *       call_param_value->accept(*this);
		 *       may actually be calling an object of the base symbol_c .
		 */
		set_datatype(desired_datatype, call_param_value);
		call_param_value->accept(*this);

		if (NULL != param_name) 
			if (extensible_parameter_highest_index < fp_iterator.extensible_param_index())
				extensible_parameter_highest_index = fp_iterator.extensible_param_index();
	}
	/* In the case of a call to an extensible function, we store the highest index 
	 * of the extensible parameters this particular call uses, in the symbol_c object
	 * of the function call itself!
	 * In calls to non-extensible functions, this value will be set to -1.
	 * This information is later used in stage4 to correctly generate the
	 * output code.
	 */
	if ((NULL != ext_parm_count) && (extensible_parameter_highest_index >=0) /* if call to extensible function */)
		*ext_parm_count = 1 + extensible_parameter_highest_index - fp_iterator.first_extensible_param_index();
}



void narrow_candidate_datatypes_c::narrow_formal_call(symbol_c *f_call, symbol_c *f_decl, int *ext_parm_count) {
	symbol_c *call_param_value, *call_param_name, *param_type;
	symbol_c *verify_duplicate_param;
	identifier_c *param_name;
	function_param_iterator_c       fp_iterator(f_decl);
	function_call_param_iterator_c fcp_iterator(f_call);
	int extensible_parameter_highest_index = -1;
	identifier_c *extensible_parameter_name;
	unsigned int i;

	if (NULL != ext_parm_count) *ext_parm_count = -1;
	/* Iterating through the formal parameters of the function call */
	while((call_param_name = fcp_iterator.next_f()) != NULL) {

		/* Obtaining the value being passed in the function call */
		call_param_value = fcp_iterator.get_current_value();
		/* the following should never occur. If it does, then we have a bug in our code... */
		if (NULL == call_param_value) ERROR;

		/* Find the corresponding parameter in function declaration */
		param_name = fp_iterator.search(call_param_name);

		/* Set the desired datatype for this parameter, and call it recursively. */
		/* NOTE: When handling a FB call, this narrow_formal_call() may be called to analyse
		 *       an invalid FB call (call with parameters that do not exist on the FB declaration).
		 *       For this reason, the param_name may come out as NULL!
		 */
		symbol_c *desired_datatype = base_type(fp_iterator.param_type());
		if ((NULL != param_name) && (NULL == desired_datatype)) ERROR;
		if ((NULL == param_name) && (NULL != desired_datatype)) ERROR;

		/* set the desired data type for this parameter */
		set_datatype(desired_datatype, call_param_value);
		/* And recursively call that parameter/expression, so it can propagate that info */
		call_param_value->accept(*this);

		/* set the extensible_parameter_highest_index, which will be needed in stage 4 */
		/* This value says how many extensible parameters are being passed to the standard function */
		if (NULL != param_name) 
			if (extensible_parameter_highest_index < fp_iterator.extensible_param_index())
				extensible_parameter_highest_index = fp_iterator.extensible_param_index();
	}
	/* call is compatible! */

	/* In the case of a call to an extensible function, we store the highest index 
	 * of the extensible parameters this particular call uses, in the symbol_c object
	 * of the function call itself!
	 * In calls to non-extensible functions, this value will be set to -1.
	 * This information is later used in stage4 to correctly generate the
	 * output code.
	 */
	if ((NULL != ext_parm_count) && (extensible_parameter_highest_index >=0) /* if call to extensible function */)
		*ext_parm_count = 1 + extensible_parameter_highest_index - fp_iterator.first_extensible_param_index();
}


/*
typedef struct {
  symbol_c *function_name,
  symbol_c *nonformal_operand_list,
  symbol_c *   formal_operand_list,

  std::vector <symbol_c *> &candidate_functions,  
  symbol_c &*called_function_declaration,
  int      &extensible_param_count
} generic_function_call_t;
*/
void narrow_candidate_datatypes_c::narrow_function_invocation(symbol_c *fcall, generic_function_call_t fcall_data) {
	/* set the called_function_declaration. */
	fcall_data.called_function_declaration = NULL;

	/* set the called_function_declaration taking into account the datatype that we need to return */
	for(unsigned int i = 0; i < fcall->candidate_datatypes.size(); i++) {
		if (get_datatype_info_c::is_type_equal(fcall->candidate_datatypes[i], fcall->datatype)) {
			fcall_data.called_function_declaration = fcall_data.candidate_functions[i];
			break;
		}
	}

	/* NOTE: If we can't figure out the declaration of the function being called, this is not 
	 *       necessarily an internal compiler error. It could be because the symbol->datatype is NULL
	 *       (because the ST code being analysed has an error _before_ this function invocation).
	 *       However, we don't just give, up, we carry on recursivly analysing the code, so as to be
	 *       able to print out any error messages related to the parameters being passed in this function 
	 *       invocation.
	 */
	/* if (NULL == symbol->called_function_declaration) ERROR; */
	if (fcall->candidate_datatypes.size() == 1) {
		/* If only one function declaration, then we use that (even if symbol->datatypes == NULL)
		 * so we can check for errors in the expressions used to pass parameters in this
		 * function invocation.
		 */
		fcall_data.called_function_declaration = fcall_data.candidate_functions[0];
	}

	/* If an overloaded function is being invoked, and we cannot determine which version to use,
	 * then we can not meaningfully verify the expressions used inside that function invocation.
	 * We simply give up!
	 */
	if (NULL == fcall_data.called_function_declaration)
		return;

	if (NULL != fcall_data.nonformal_operand_list)  narrow_nonformal_call(fcall, fcall_data.called_function_declaration, &(fcall_data.extensible_param_count));
	if (NULL != fcall_data.   formal_operand_list)     narrow_formal_call(fcall, fcall_data.called_function_declaration, &(fcall_data.extensible_param_count));

	return;
}




/* narrow implicit FB call in IL.
 * e.g.  CLK ton_var
 *        CU counter_var
 *
 * The algorithm will be to build a fake il_fb_call_c equivalent to the implicit IL FB call, and let 
 * the visit(il_fb_call_c *) method handle it!
 */
void *narrow_candidate_datatypes_c::narrow_implicit_il_fb_call(symbol_c *il_instruction, const char *param_name, symbol_c *&called_fb_declaration) {

	/* set the datatype of the il_operand, this is, the FB being called! */
	if (NULL != il_operand) {
		set_datatype(called_fb_declaration, il_operand); /* only set it if it is in the candidate datatypes list! */  
		il_operand->accept(*this);
	}

	if (0 == fake_prev_il_instruction->prev_il_instruction.size()) {
		/* This IL implicit FB call (e.g. CLK ton_var) is not preceded by another IL instruction
		 * (or list of instructions) that will set the IL current/default value.
		 * We cannot proceed verifying type compatibility of something that does not exist.
		 */
		return NULL;
	}

	symbol_c *fb_decl = (NULL == il_operand)? NULL : il_operand->datatype;
	
	if (NULL == fb_decl) {
		/* the il_operand is a not FB instance, or it simply does not even exist, */
		/* so we simply pass on the required datatype to the prev_il_instructions */
		/* The invalid FB invocation will be caught in the print_datatypes_error_c by analysing NULL value in il_operand->datatype! */
		set_datatype_in_prev_il_instructions(il_instruction->datatype, fake_prev_il_instruction);
		return NULL;
	}
	

	/* The value being passed to the 'param_name' parameter is actually the prev_il_instruction.
	 * However, we do not place that object directly in the fake il_param_list_c that we will be
	 * creating, since the visit(il_fb_call_c *) method will recursively call every object in that list.
	 * The il_prev_intruction object will be visited once we have handled this implici IL FB call
	 * (called from the instruction_list_c for() loop that works backwards). We DO NOT want to visit it twice.
	 * (Anyway, if we let the visit(il_fb_call_c *) recursively visit the current prev_il_instruction, this pointer
	 * would be changed to the IL instruction coming before the current prev_il_instruction! => things would get all messed up!)
	 * The easiest way to work around this is to simply use a new object, and copy the relevant details to that object!
	 */
	symbol_c param_value = *fake_prev_il_instruction; /* copy the candidate_datatypes list ! */

	identifier_c variable_name(param_name);
	// SYM_REF1(il_assign_operator_c, variable_name)
	il_assign_operator_c il_assign_operator(&variable_name);  
	// SYM_REF3(il_param_assignment_c, il_assign_operator, il_operand, simple_instr_list)
	il_param_assignment_c il_param_assignment(&il_assign_operator, &param_value/*il_operand*/, NULL);
	il_param_list_c il_param_list;
	il_param_list.add_element(&il_param_assignment);
	// SYM_REF4(il_fb_call_c, il_call_operator, fb_name, il_operand_list, il_param_list, symbol_c *called_fb_declaration)
	CAL_operator_c CAL_operator;
	il_fb_call_c il_fb_call(&CAL_operator, il_operand, NULL, &il_param_list);
	        
	/* A FB call does not return any datatype, but the IL instructions that come after this
	 * FB call may require a specific datatype in the il current/default variable, 
	 * so we must pass this information up to the IL instruction before the FB call, since it will
	 * be that IL instruction that will be required to produce the desired dtataype.
	 *
	 * The above will be done by the visit(il_fb_call_c *) method, so we must make sure to
	 * correctly set up the il_fb_call.datatype variable!
	 */
	il_fb_call.called_fb_declaration = called_fb_declaration;
	il_fb_call.accept(*this);

	/* set the required datatype of the previous IL instruction! */
	/* NOTE:
	 * When handling these implicit IL calls, the parameter_value being passed to the FB parameter
	 * is actually the prev_il_instruction.
	 * 
	 * However, since the FB call does not change the value in the current/default IL variable, this value
	 * must also be used ny the IL instruction coming after this FB call.
	 *
	 * This means that we have two consumers/users for the same value. 
	 * We must therefore check whether the datatype required by the IL instructions following this FB call 
	 * is the same as that required for the first parameter. If not, then we have a semantic error,
	 * and we set it to invalid_type_name.
	 *
	 * However, we only do that if:
	 *  - The IL instruction that comes after this IL FB call actually asked this FB call for a specific 
	 *     datatype in the current/default vairable, once this IL FB call returns.
	 *     However, sometimes, (for e.g., this FB call is the last in the IL list) the subsequent FB to not aks this
	 *     FB call for any datatype. In that case, then the datatype required to pass to the first parameter of the
	 *     FB call must be left unchanged!
	 */
	if ((NULL == il_instruction->datatype) || (get_datatype_info_c::is_type_equal(param_value.datatype, il_instruction->datatype))) {
		set_datatype_in_prev_il_instructions(param_value.datatype, fake_prev_il_instruction);
	} else {
		set_datatype_in_prev_il_instructions(&get_datatype_info_c::invalid_type_name, fake_prev_il_instruction);
	}
	return NULL;
}


/* a helper function... */
symbol_c *narrow_candidate_datatypes_c::base_type(symbol_c *symbol) {
	/* NOTE: symbol == NULL is valid. It will occur when, for e.g., an undefined/undeclared symbolic_variable is used in the code. */
	if (symbol == NULL) return NULL;
	return search_base_type_c::get_basetype_decl(symbol);
}



/*************************/
/* B.1 - Common elements */
/*************************/
/*******************************************/
/* B 1.1 - Letters, digits and identifiers */
/*******************************************/
/* Before the existance of this derived_datatype_identifier_c class, all identifiers were stored in the generic identifier_c class.
 * Since not all identifiers are used to identify a datatype, the fill/narrow algorithms could not do fill/narrow on all identifiers, and relied
 * on the parent class to do the fill/narrow when apropriate (i.e. when the identifier was known to reference a datatype
 * for example, in a variable declaration -->  var1: some_user_defined_type;)
 *
 * However, at least one location where an identifier may reference a datatype has not yet been covered:
 *     array1: ARRAY [1..2] OF INT;
 *     array2: ARRAY [1..2] OF array1;  (* array1 here is stored as a derived_datatype_identifier_c)
 *
 * Instead of doing it like the old way, I have opted to do a generic fill/narrow for all derived_datatype_identifier_c
 * This means that we can now delete the code of all parents of derived_datatype_identifier_c that are still doing the fill/narrow
 * explicitly (assuming we do also implement the visitor for poutype_identifier_c). However, I will leave this code cleanup for some later oportunity.
 */
void *narrow_candidate_datatypes_c::visit(derived_datatype_identifier_c *symbol) {
	// If this symbol was used (for example) in an ARRAY [1..2] OF <derived_datatype_identifier_c> (i.e. a datatype in an array)
	// then the symbol->datatype of this derived_datatype_identifier_c has not yet been set by the previous visit() method!
	// We therefore set the datatype ourselves!
	if ((NULL == symbol->datatype) && (symbol->candidate_datatypes.size() == 1))
		symbol->datatype = symbol->candidate_datatypes[0];
	return NULL;
}


/* The datatype of a poutype_identifier_c is currently always being set by the parent object, so we leave this code commented out for now. */
/* 
void *narrow_candidate_datatypes_c::visit(         poutype_identifier_c *symbol) {
	// If this symbol was used (for example) in an ARRAY [1..2] OF <derived_datatype_identifier_c> (i.e. a datatype in an array)
	// then the symbol->datatype of this derived_datatype_identifier_c has not yet been set by the previous visit() method!
	// We therefore set the datatype ourselves!
	if ((NULL == symbol->datatype) && (symbol->candidate_datatypes.size() == 1))
		symbol->datatype = symbol->candidate_datatypes[0];
	return NULL;
}
*/
    
    

/**********************/
/* B 1.3 - Data types */
/**********************/
/***********************************/
/* B 1.3.1 - Elementary Data Types */
/***********************************/
/* NOTE: elementary datatypes are their own basetype ! */
void *narrow_candidate_datatypes_c::visit(    time_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    bool_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    sint_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    int_type_name_c     *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    dint_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    lint_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    usint_type_name_c   *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    uint_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    udint_type_name_c   *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    ulint_type_name_c   *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    real_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    lreal_type_name_c   *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    date_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    tod_type_name_c     *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    dt_type_name_c      *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    byte_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    word_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    dword_type_name_c   *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    lword_type_name_c   *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    string_type_name_c  *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(    wstring_type_name_c *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 

void *narrow_candidate_datatypes_c::visit(safetime_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safebool_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safesint_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safeint_type_name_c     *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safedint_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safelint_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safeusint_type_name_c   *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safeuint_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safeudint_type_name_c   *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safeulint_type_name_c   *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safereal_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safelreal_type_name_c   *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safedate_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safetod_type_name_c     *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safedt_type_name_c      *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safebyte_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safeword_type_name_c    *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safedword_type_name_c   *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safelword_type_name_c   *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safestring_type_name_c  *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 
void *narrow_candidate_datatypes_c::visit(safewstring_type_name_c *symbol) {symbol->datatype = search_base_type_c::get_basetype_decl(symbol); return NULL;} 


/********************************/
/* B.1.3.2 - Generic data types */
/********************************/
/* empty!! */

/********************************/
/* B 1.3.3 - Derived data types */
/********************************/
void *narrow_candidate_datatypes_c::narrow_spec_init(symbol_c *symbol, symbol_c *type_decl, symbol_c *init_value) {
	// If we are handling an anonymous datatype (i.e. a datatype implicitly declared inside a VAR ... END_VAR declaration)
	// then the symbol->datatype has not yet been set by the previous visit(type_decl) method, because it does not exist!
	// So we set the datatype ourselves!
	if ((NULL == symbol->datatype) && (symbol->candidate_datatypes.size() == 1))
		symbol->datatype = symbol->candidate_datatypes[0];
  
	set_datatype(symbol->datatype, type_decl);
	type_decl->accept(*this);

	if (NULL != init_value) {
		set_datatype(symbol->datatype, init_value);
		init_value->accept(*this);
	}
	return NULL;
}


void *narrow_candidate_datatypes_c::narrow_type_decl(symbol_c *symbol, symbol_c *type_name, symbol_c *spec_init) {
	if (symbol->candidate_datatypes.size() == 1) {
		symbol->datatype = symbol->candidate_datatypes[0];
  
		set_datatype(symbol->datatype, type_name);
		set_datatype(symbol->datatype, spec_init);
		spec_init->accept(*this);
	}
	return NULL;
}


/*  TYPE type_declaration_list END_TYPE */
// SYM_REF1(data_type_declaration_c, type_declaration_list)
/* NOTE: Not required. already handled by iterator_visitor_c base class */

/* helper symbol for data_type_declaration */
// SYM_LIST(type_declaration_list_c)
/* NOTE: Not required. already handled by iterator_visitor_c base class */

/*  simple_type_name ':' simple_spec_init */
// SYM_REF2(simple_type_declaration_c, simple_type_name, simple_spec_init)
void *narrow_candidate_datatypes_c::visit(simple_type_declaration_c *symbol) {return narrow_type_decl(symbol, symbol->simple_type_name, symbol->simple_spec_init);}

/* simple_specification ASSIGN constant */
// SYM_REF2(simple_spec_init_c, simple_specification, constant)
void *narrow_candidate_datatypes_c::visit(simple_spec_init_c *symbol) {return narrow_spec_init(symbol, symbol->simple_specification, symbol->constant);}

/*  subrange_type_name ':' subrange_spec_init */
// SYM_REF2(subrange_type_declaration_c, subrange_type_name, subrange_spec_init)
void *narrow_candidate_datatypes_c::visit(subrange_type_declaration_c *symbol) {return narrow_type_decl(symbol, symbol->subrange_type_name, symbol->subrange_spec_init);}

/* subrange_specification ASSIGN signed_integer */
// SYM_REF2(subrange_spec_init_c, subrange_specification, signed_integer)
void *narrow_candidate_datatypes_c::visit(subrange_spec_init_c *symbol) {return narrow_spec_init(symbol, symbol->subrange_specification, symbol->signed_integer);}

/*  integer_type_name '(' subrange')' */
// SYM_REF2(subrange_specification_c, integer_type_name, subrange)
void *narrow_candidate_datatypes_c::visit(subrange_specification_c *symbol) {
	set_datatype(symbol->datatype, symbol->integer_type_name);
	symbol->integer_type_name->accept(*this);
	set_datatype(symbol->datatype, symbol->integer_type_name);
	symbol->integer_type_name->accept(*this);
	return NULL;
}

/*  signed_integer DOTDOT signed_integer */
/* dimension will be filled in during stage 3 (array_range_check_c) with the number of elements in this subrange */
// SYM_REF2(subrange_c, lower_limit, upper_limit, unsigned long long int dimension;)
void *narrow_candidate_datatypes_c::visit(subrange_c *symbol) {
	set_datatype(symbol->datatype, symbol->lower_limit);
	symbol->lower_limit->accept(*this);
	set_datatype(symbol->datatype, symbol->upper_limit);
	symbol->upper_limit->accept(*this);
	return NULL;
}


/*  enumerated_type_name ':' enumerated_spec_init */
// SYM_REF2(enumerated_type_declaration_c, enumerated_type_name, enumerated_spec_init)
void *narrow_candidate_datatypes_c::visit(enumerated_type_declaration_c *symbol) {return narrow_type_decl(symbol, symbol->enumerated_type_name, symbol->enumerated_spec_init);}


/* enumerated_specification ASSIGN enumerated_value */
// SYM_REF2(enumerated_spec_init_c, enumerated_specification, enumerated_value)
void *narrow_candidate_datatypes_c::visit(enumerated_spec_init_c *symbol) {return narrow_spec_init(symbol, symbol->enumerated_specification, symbol->enumerated_value);}

/* helper symbol for enumerated_specification->enumerated_spec_init */
/* enumerated_value_list ',' enumerated_value */
// SYM_LIST(enumerated_value_list_c)
void *narrow_candidate_datatypes_c::visit(enumerated_value_list_c *symbol) {
//if (NULL == symbol->datatype) ERROR;  // Comented out-> Reserve this check for the print_datatypes_error_c ???  
  for(int i = 0; i < symbol->n; i++) set_datatype(symbol->datatype, symbol->elements[i]);
//for(int i = 0; i < symbol->n; i++) if (NULL == symbol->elements[i]->datatype) ERROR; // Comented out-> Reserve this check for the print_datatypes_error_c ???  
  return NULL;  
}


/* enumerated_type_name '#' identifier */
// SYM_REF2(enumerated_value_c, type, value)
// void *narrow_candidate_datatypes_c::visit(enumerated_value_c *symbol) {/* do nothing! */ return NULL;}


/*  identifier ':' array_spec_init */
// SYM_REF2(array_type_declaration_c, identifier, array_spec_init)
void *narrow_candidate_datatypes_c::visit(array_type_declaration_c *symbol) {return narrow_type_decl(symbol, symbol->identifier, symbol->array_spec_init);}

/* array_specification [ASSIGN array_initialization} */
/* array_initialization may be NULL ! */
// SYM_REF2(array_spec_init_c, array_specification, array_initialization)
void *narrow_candidate_datatypes_c::visit(array_spec_init_c *symbol) {return narrow_spec_init(symbol, symbol->array_specification, symbol->array_initialization);}

/* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
// SYM_REF2(array_specification_c, array_subrange_list, non_generic_type_name)
// Not needed!!

/* helper symbol for array_specification */
/* array_subrange_list ',' subrange */
// SYM_LIST(array_subrange_list_c)
// Not needed ??

/* array_initialization:  '[' array_initial_elements_list ']' */
/* helper symbol for array_initialization */
/* array_initial_elements_list ',' array_initial_elements */
// SYM_LIST(array_initial_elements_list_c)
// Not needed ???

/* integer '(' [array_initial_element] ')' */
/* array_initial_element may be NULL ! */
// SYM_REF2(array_initial_elements_c, integer, array_initial_element)
// Not needed ???

/*  structure_type_name ':' structure_specification */
// SYM_REF2(structure_type_declaration_c, structure_type_name, structure_specification)
void *narrow_candidate_datatypes_c::visit(structure_type_declaration_c *symbol) {return narrow_type_decl(symbol, symbol->structure_type_name, symbol->structure_specification);}

/* structure_type_name ASSIGN structure_initialization */
/* structure_initialization may be NULL ! */
// SYM_REF2(initialized_structure_c, structure_type_name, structure_initialization)
void *narrow_candidate_datatypes_c::visit(initialized_structure_c *symbol) {return narrow_spec_init(symbol, symbol->structure_type_name, symbol->structure_initialization);}

/* helper symbol for structure_declaration */
/* structure_declaration:  STRUCT structure_element_declaration_list END_STRUCT */
/* structure_element_declaration_list structure_element_declaration ';' */
// SYM_LIST(structure_element_declaration_list_c)
// Not needed ???

/*  structure_element_name ':' *_spec_init */
// SYM_REF2(structure_element_declaration_c, structure_element_name, spec_init)
// Not needed ???

/* helper symbol for structure_initialization */
/* structure_initialization: '(' structure_element_initialization_list ')' */
/* structure_element_initialization_list ',' structure_element_initialization */
// SYM_LIST(structure_element_initialization_list_c)
// Not needed ???

/*  structure_element_name ASSIGN value */
// SYM_REF2(structure_element_initialization_c, structure_element_name, value)
// Not needed ???

/*  string_type_name ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init */
// SYM_REF4(string_type_declaration_c, string_type_name, elementary_string_type_name, string_type_declaration_size, string_type_declaration_init/* may be == NULL! */) 


/* structure_type_name ASSIGN structure_initialization */
/* structure_initialization may be NULL ! */
// SYM_REF2(initialized_structure_c, structure_type_name, structure_initialization)
void *narrow_candidate_datatypes_c::visit(fb_spec_init_c *symbol) {return narrow_spec_init(symbol, symbol->function_block_type_name, symbol->structure_initialization);}


/* REF_TO (non_generic_type_name | function_block_type_name) */
// SYM_REF1(ref_spec_c, type_name)
void *narrow_candidate_datatypes_c::visit(ref_spec_c *symbol) {
	/* First handle the datatype being referenced (pointed to) */
	if (symbol->type_name->candidate_datatypes.size() == 1) {
		symbol->type_name->datatype = symbol->type_name->candidate_datatypes[0];
		symbol->type_name->accept(*this);
	}

	/* Now handle the reference datatype itself (i.e. the pointer) */
	// If we are handling an anonymous datatype (i.e. a datatype implicitly declared inside a VAR ... END_VAR declaration)
	// then the symbol->datatype has not yet been set by the previous visit(type_decl) method, because it does not exist!
	// So we set the datatype ourselves!
	if ((NULL == symbol->datatype) && (symbol->candidate_datatypes.size() == 1))
		symbol->datatype = symbol->candidate_datatypes[0];

	return NULL;
}

/* For the moment, we do not support initialising reference data types */
/* ref_spec [ ASSIGN ref_initialization ] */ 
/* NOTE: ref_initialization may be NULL!! */
// SYM_REF2(ref_spec_init_c, ref_spec, ref_initialization)
void *narrow_candidate_datatypes_c::visit(ref_spec_init_c *symbol) {return narrow_spec_init(symbol, symbol->ref_spec, symbol->ref_initialization);}

/* identifier ':' ref_spec_init */
// SYM_REF2(ref_type_decl_c, ref_type_name, ref_spec_init)
void *narrow_candidate_datatypes_c::visit(ref_type_decl_c *symbol) {return narrow_type_decl(symbol, symbol->ref_type_name, symbol->ref_spec_init);}



/*********************/
/* B 1.4 - Variables */
/*********************/
// SYM_REF1(symbolic_variable_c, var_name)
void *narrow_candidate_datatypes_c::visit(symbolic_variable_c *symbol) {
	symbol->var_name->datatype = symbol->datatype;
	return NULL;
}
/********************************************/
/* B 1.4.1 - Directly Represented Variables */
/********************************************/

/*************************************/
/* B 1.4.2 - Multi-element variables */
/*************************************/
/*  subscripted_variable '[' subscript_list ']' */
// SYM_REF2(array_variable_c, subscripted_variable, subscript_list)
void *narrow_candidate_datatypes_c::visit(array_variable_c *symbol) {
	/* we need to check the data types of the expressions used for the subscripts... */
	symbol->subscript_list->accept(*this);

	/* Set the datatype of the subscripted variable and visit it recursively. For the reason why we do this,                                                 */
	/* Please read the comments in the array_variable_c and structured_variable_c visitors in the fill_candidate_datatypes.cc file! */
	if (symbol->subscripted_variable->candidate_datatypes.size() == 1)
	  symbol->subscripted_variable->datatype = symbol->subscripted_variable->candidate_datatypes[0]; // set the datatype
	symbol->subscripted_variable->accept(*this); // visit recursively

	return NULL;
}


/* subscript_list ',' subscript */
// SYM_LIST(subscript_list_c)
void *narrow_candidate_datatypes_c::visit(subscript_list_c *symbol) {
	for (int i = 0; i < symbol->n; i++) {
		for (unsigned int k = 0; k < symbol->elements[i]->candidate_datatypes.size(); k++) {
			if (get_datatype_info_c::is_ANY_INT(symbol->elements[i]->candidate_datatypes[k]))
				symbol->elements[i]->datatype = symbol->elements[i]->candidate_datatypes[k];
		}
		symbol->elements[i]->accept(*this);
	}
	return NULL;  
}



/*  record_variable '.' field_selector */
/*  WARNING: input and/or output variables of function blocks
 *           may be accessed as fields of a structured variable!
 *           Code handling a structured_variable_c must take
 *           this into account!
 */
// SYM_REF2(structured_variable_c, record_variable, field_selector)
void *narrow_candidate_datatypes_c::visit(structured_variable_c *symbol) {
	/* Set the datatype of the record_variable and visit it recursively. For the reason why we do this,                                                      */
	/* Please read the comments in the array_variable_c and structured_variable_c visitors in the fill_candidate_datatypes.cc file! */
	if (symbol->record_variable->candidate_datatypes.size() == 1)
	  symbol->record_variable->datatype = symbol->record_variable->candidate_datatypes[0]; // set the datatype
	symbol->record_variable->accept(*this); // visit recursively

	return NULL;
}


/******************************************/
/* B 1.4.3 - Declaration & Initialisation */
/******************************************/

/* When handling the declaration of variables the fill/narrow algorithm will simply visit the objects
 * in the abstract syntax tree defining the desired datatype for the variables. Tis is to set the 
 * symbol->datatype to the basetype of that datatype.
 *
 * Note that we do not currently set the symbol->datatype annotation for the identifier_c objects naming the 
 * variables inside the variable declaration. However, this is liable to change in the future, so do not write
 * any code that depends on this!
 * 
 * example:
 *    VAR  var1, var2, var3  :  my_type;  END_VAR
 *   (*    ^^^^  ^^^^  ^^^^                -> will NOT have the symbol->datatype set (for now, may change in the future!) *)
 *   (*                         ^^^^^^^    -> WILL     have the symbol->datatype set *)
 * 
 * (remeber too that the identifier_c objects identifying variables inside ST/IL/SFC code *will* have their 
 *  symbol->datatype annotation filled by the fill/narrow algorithm)
 */
void *narrow_candidate_datatypes_c::narrow_var_declaration(symbol_c *type) {
  if (type->candidate_datatypes.size() == 1)
    type->datatype = type->candidate_datatypes[0];
  type->accept(*this); 
  return NULL;
}


void *narrow_candidate_datatypes_c::visit(var1_init_decl_c             *symbol) {return narrow_var_declaration(symbol->spec_init);}
void *narrow_candidate_datatypes_c::visit(array_var_init_decl_c        *symbol) {return narrow_var_declaration(symbol->array_spec_init);}
void *narrow_candidate_datatypes_c::visit(structured_var_init_decl_c   *symbol) {return narrow_var_declaration(symbol->initialized_structure);}
void *narrow_candidate_datatypes_c::visit(fb_name_decl_c               *symbol) {return narrow_var_declaration(symbol->fb_spec_init);}
void *narrow_candidate_datatypes_c::visit(array_var_declaration_c      *symbol) {return narrow_var_declaration(symbol->array_specification);}
void *narrow_candidate_datatypes_c::visit(structured_var_declaration_c *symbol) {return narrow_var_declaration(symbol->structure_type_name);}
void *narrow_candidate_datatypes_c::visit(external_declaration_c       *symbol) {return narrow_var_declaration(symbol->specification);}
void *narrow_candidate_datatypes_c::visit(global_var_decl_c            *symbol) {return narrow_var_declaration(symbol->type_specification);}
void *narrow_candidate_datatypes_c::visit(incompl_located_var_decl_c   *symbol) {return narrow_var_declaration(symbol->var_spec);}
//void *narrow_candidate_datatypes_c::visit(single_byte_string_var_declaration_c *symbol) {return handle_var_declaration(symbol->single_byte_string_spec);}
//void *narrow_candidate_datatypes_c::visit(double_byte_string_var_declaration_c *symbol) {return handle_var_declaration(symbol->double_byte_string_spec);}



void *narrow_candidate_datatypes_c::visit(var1_list_c *symbol) {
#if 0   /* We don't really need to set the datatype of each variable. We just check the declaration itself! */
  for(int i = 0; i < symbol->n; i++) {
    if (symbol->elements[i]->candidate_datatypes.size() == 1)
      symbol->elements[i]->datatype = symbol->elements[i]->candidate_datatypes[0];
  }
#endif
  return NULL;
}  


/*  AT direct_variable */
// SYM_REF1(location_c, direct_variable)
void *narrow_candidate_datatypes_c::visit(location_c *symbol) {
  set_datatype(symbol->datatype, symbol->direct_variable);
  symbol->direct_variable->accept(*this); /* currently does nothing! */
  return NULL;
}


/*  [variable_name] location ':' located_var_spec_init */
/* variable_name -> may be NULL ! */
// SYM_REF3(located_var_decl_c, variable_name, location, located_var_spec_init)
void *narrow_candidate_datatypes_c::visit(located_var_decl_c *symbol) {
  /* let the var_spec_init set its own symbol->datatype value */
  symbol->located_var_spec_init->accept(*this);
  
  if (NULL != symbol->variable_name)
    set_datatype(symbol->located_var_spec_init->datatype, symbol->variable_name);
    
  set_datatype(symbol->located_var_spec_init->datatype, symbol->location);
  symbol->location->accept(*this);
  return NULL;
}



/************************************/
/* B 1.5 Program organization units */
/************************************/
/*********************/
/* B 1.5.1 Functions */
/*********************/
void *narrow_candidate_datatypes_c::visit(function_declaration_c *symbol) {
	/* set the function_declaration_c->datatype to the datatype returned by the function! */
	symbol->type_name->datatype = search_base_type_c::get_basetype_decl(symbol->type_name);
	symbol->datatype = symbol->type_name->datatype;
	
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
	symbol->var_declarations_list->accept(*this);
	if (debug) printf("Narrowing candidate data types list in body of function %s\n", ((token_c *)(symbol->derived_function_name))->value);
	symbol->function_body->accept(*this);
	delete search_varfb_instance_type;
	search_varfb_instance_type = NULL;
	return NULL;
}

/***************************/
/* B 1.5.2 Function blocks */
/***************************/
void *narrow_candidate_datatypes_c::visit(function_block_declaration_c *symbol) {
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
	symbol->var_declarations->accept(*this);
	if (debug) printf("Narrowing candidate data types list in body of FB %s\n", ((token_c *)(symbol->fblock_name))->value);
	symbol->fblock_body->accept(*this);
	delete search_varfb_instance_type;
	search_varfb_instance_type = NULL;

	// A FB declaration can also be used as a Datatype! We now do the narrow algorithm considering it as such!
	if (symbol->candidate_datatypes.size() == 1)
		symbol->datatype = symbol->candidate_datatypes[0];
	return NULL;
}

/********************/
/* B 1.5.3 Programs */
/********************/
void *narrow_candidate_datatypes_c::visit(program_declaration_c *symbol) {
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
	symbol->var_declarations->accept(*this);
	if (debug) printf("Narrowing candidate data types list in body of program %s\n", ((token_c *)(symbol->program_type_name))->value);
	symbol->function_block_body->accept(*this);
	delete search_varfb_instance_type;
	search_varfb_instance_type = NULL;
	return NULL;
}

/********************************************/
/* B 1.6 Sequential function chart elements */
/********************************************/
void *narrow_candidate_datatypes_c::visit(transition_condition_c *symbol) {
	// We can safely ask for a BOOL type, as even if the result is a SAFEBOOL, in that case it will aslo include BOOL as a possible datatype.
	set_datatype(&get_datatype_info_c::bool_type_name /* datatype*/, symbol /* symbol */);

	if (symbol->transition_condition_il != NULL) {
		set_datatype(symbol->datatype, symbol->transition_condition_il);
		symbol->transition_condition_il->accept(*this);
	}
	if (symbol->transition_condition_st != NULL) {
		set_datatype(symbol->datatype, symbol->transition_condition_st);
		symbol->transition_condition_st->accept(*this);
	}
	return NULL;
}

/********************************/
/* B 1.7 Configuration elements */
/********************************/
void *narrow_candidate_datatypes_c::visit(configuration_declaration_c *symbol) {
	if (debug) printf("Narrowing candidate data types list in configuration %s\n", ((token_c *)(symbol->configuration_name))->value);
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
	symbol->global_var_declarations->accept(*this);
	symbol->resource_declarations            ->accept(*this); // points to a single_resource_declaration_c or a resource_declaration_list_c
//	symbol->access_declarations              ->accept(*this); // TODO
//	symbol->instance_specific_initializations->accept(*this); // TODO
	delete search_varfb_instance_type;
	search_varfb_instance_type = NULL;
	return NULL;
}


void *narrow_candidate_datatypes_c::visit(resource_declaration_c *symbol) {
	if (debug) printf("Narrowing candidate data types list in resource %s\n", ((token_c *)(symbol->resource_name))->value);
	search_varfb_instance_type_c *prev_search_varfb_instance_type = search_varfb_instance_type;
	search_varfb_instance_type  =  new search_varfb_instance_type_c(symbol);
	symbol->global_var_declarations->accept(*this);
	symbol->resource_declaration   ->accept(*this);  // points to a single_resource_declaration_c!
	delete search_varfb_instance_type;
	search_varfb_instance_type = prev_search_varfb_instance_type;
	return NULL;
}


void *narrow_candidate_datatypes_c::visit(single_resource_declaration_c *symbol) {
//	symbol->task_configuration_list    ->accept(*this);  // TODO
//	symbol->program_configuration_list ->accept(*this);  // TODO
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
void *narrow_candidate_datatypes_c::visit(instruction_list_c *symbol) {
	/* In order to execute the narrow algoritm correctly, we need to go through the instructions backwards,
	 * so we can not use the base class' visitor 
	 */
	/* In order to execute the narrow algoritm correctly
	 * in IL instruction lists containing JMPs to labels that come before the JMP instruction
	 * itself, we need to run the narrow algorithm twice on the Instruction List.
	 * e.g.:  ...
	 *          ld 23
	 *   label1:st byte_var
	 *          ld 34
	 *          JMP label1     
	 *
	 * Note that the second time we run the narrow, most of the datatypes are already filled
	 * in, so it will be able to produce tha correct datatypes for the IL instruction referenced
	 * by the label, as in the 2nd pass we already know the datatypes of the JMP instruction!
	 */
	for(int j = 0; j < 2; j++) {
		for(int i = symbol->n-1; i >= 0; i--) {
			symbol->elements[i]->accept(*this);
		}
	}
	return NULL;
}

/* | label ':' [il_incomplete_instruction] eol_list */
// SYM_REF2(il_instruction_c, label, il_instruction)
// void *visit(instruction_list_c *symbol);
void *narrow_candidate_datatypes_c::visit(il_instruction_c *symbol) {
	if (NULL == symbol->il_instruction) {
		/* this empty/null il_instruction cannot generate the desired datatype. We pass on the request to the previous il instruction. */
		set_datatype_in_prev_il_instructions(symbol->datatype, symbol);
	} else {
		il_instruction_c tmp_prev_il_instruction(NULL, NULL);
		/* the narrow algorithm will need access to the intersected candidate_datatype lists of all prev_il_instructions, as well as the 
		 * list of the prev_il_instructions.
		 * Instead of creating two 'global' (within the class) variables, we create a single il_instruction_c variable (fake_prev_il_instruction),
		 * and shove that data into this single variable.
		 */
		tmp_prev_il_instruction.prev_il_instruction = symbol->prev_il_instruction;
		intersect_prev_candidate_datatype_lists(&tmp_prev_il_instruction);
		/* Tell the il_instruction the datatype that it must generate - this was chosen by the next il_instruction (remember: we are iterating backwards!) */
		fake_prev_il_instruction = &tmp_prev_il_instruction;
		current_il_instruction   = symbol;
		symbol->il_instruction->datatype = symbol->datatype;
		symbol->il_instruction->accept(*this);
		fake_prev_il_instruction = NULL;
		current_il_instruction   = NULL;
	}
	return NULL;
}




// void *visit(instruction_list_c *symbol);
void *narrow_candidate_datatypes_c::visit(il_simple_operation_c *symbol) {
	/* Tell the il_simple_operator the datatype that it must generate - this was chosen by the next il_instruction (we iterate backwards!) */
	symbol->il_simple_operator->datatype = symbol->datatype;
	/* recursive call to see whether data types are compatible */
	il_operand = symbol->il_operand;
	symbol->il_simple_operator->accept(*this);
	il_operand = NULL;
	return NULL;
}

/* | function_name [il_operand_list] */
/* NOTE: The parameters 'called_function_declaration' and 'extensible_param_count' are used to pass data between the stage 3 and stage 4. */
// SYM_REF2(il_function_call_c, function_name, il_operand_list, symbol_c *called_function_declaration; int extensible_param_count;)
void *narrow_candidate_datatypes_c::visit(il_function_call_c *symbol) {
	/* The first parameter of a non formal function call in IL will be the 'current value' (i.e. the prev_il_instruction)
	 * In order to be able to handle this without coding special cases, we will simply prepend that symbol
	 * to the il_operand_list, and remove it after calling handle_function_call().
	 * However, since handle_function_call() will be recursively calling all parameter, and we don't want
	 * to do that for the prev_il_instruction (since it has already been visited by the fill_candidate_datatypes_c)
	 * we create a new ____ symbol_c ____ object, and copy the relevant info to/from that object before/after
	 * the call to handle_function_call().
	 *
	 * However, if no further paramters are given, then il_operand_list will be NULL, and we will
	 * need to create a new object to hold the pointer to prev_il_instruction.
	 * This change will also be undone at the end of this method.
	 */
	symbol_c param_value = *fake_prev_il_instruction; /* copy the candidate_datatypes list */
	if (NULL == symbol->il_operand_list)  symbol->il_operand_list = new il_operand_list_c;
	if (NULL == symbol->il_operand_list)  ERROR;

	((list_c *)symbol->il_operand_list)->insert_element(&param_value, 0);

	generic_function_call_t fcall_param = {
		/* fcall_param.function_name               = */ symbol->function_name,
		/* fcall_param.nonformal_operand_list      = */ symbol->il_operand_list,
		/* fcall_param.formal_operand_list         = */ NULL,
		/* enum {POU_FB, POU_function} POU_type    = */ generic_function_call_t::POU_function,
		/* fcall_param.candidate_functions         = */ symbol->candidate_functions,
		/* fcall_param.called_function_declaration = */ symbol->called_function_declaration,
		/* fcall_param.extensible_param_count      = */ symbol->extensible_param_count
	};

	narrow_function_invocation(symbol, fcall_param);
	set_datatype_in_prev_il_instructions(param_value.datatype, fake_prev_il_instruction);

	/* Undo the changes to the abstract syntax tree we made above... */
	((list_c *)symbol->il_operand_list)->remove_element(0);
	if (((list_c *)symbol->il_operand_list)->n == 0) {
		/* if the list becomes empty, then that means that it did not exist before we made these changes, so we delete it! */
		delete 	symbol->il_operand_list;
		symbol->il_operand_list = NULL;
	}

	return NULL;
}


/* | il_expr_operator '(' [il_operand] eol_list [simple_instr_list] ')' */
// SYM_REF3(il_expression_c, il_expr_operator, il_operand, simple_instr_list);
void *narrow_candidate_datatypes_c::visit(il_expression_c *symbol) {
  /* first handle the operation (il_expr_operator) that will use the result coming from the parenthesised IL list (i.e. simple_instr_list) */
  symbol->il_expr_operator->datatype = symbol->datatype;
  il_operand = symbol->simple_instr_list; /* This is not a bug! The parenthesised expression will be used as the operator! */
  symbol->il_expr_operator->accept(*this);

  /* now give the parenthesised IL list a chance to narrow the datatypes */
  /* The datatype that is must return was set by the call symbol->il_expr_operator->accept(*this) */
  il_instruction_c *save_fake_prev_il_instruction = fake_prev_il_instruction; /*this is not really necessary, but lets play it safe */
  symbol->simple_instr_list->accept(*this);
  fake_prev_il_instruction = save_fake_prev_il_instruction;
  
  /* Since stage2 will insert an artificial (and equivalent) LD <il_operand> to the simple_instr_list when an 'il_operand' exists, we know
   * that if (symbol->il_operand != NULL), then the first IL instruction in the simple_instr_list will be the equivalent and artificial
   * 'LD <il_operand>' IL instruction.
   * Just to be consistent, we will copy the datatype info back into the il_operand, even though this should not be necessary!
   */
  if ((NULL != symbol->il_operand) && ((NULL == symbol->simple_instr_list) || (0 == ((list_c *)symbol->simple_instr_list)->n))) ERROR; // stage2 is not behaving as we expect it to!
  if  (NULL != symbol->il_operand)
    symbol->il_operand->datatype = ((list_c *)symbol->simple_instr_list)->elements[0]->datatype;
  
  return NULL;
}




/*  il_jump_operator label */
void *narrow_candidate_datatypes_c::visit(il_jump_operation_c *symbol) {
  /* recursive call to fill the datatype */
  symbol->il_jump_operator->datatype = symbol->datatype;
  symbol->il_jump_operator->accept(*this);
  return NULL;
}







/*   il_call_operator prev_declared_fb_name
 * | il_call_operator prev_declared_fb_name '(' ')'
 * | il_call_operator prev_declared_fb_name '(' eol_list ')'
 * | il_call_operator prev_declared_fb_name '(' il_operand_list ')'
 * | il_call_operator prev_declared_fb_name '(' eol_list il_param_list ')'
 */
/* NOTE: The parameter 'called_fb_declaration'is used to pass data between stage 3 and stage4 */
// SYM_REF4(il_fb_call_c, il_call_operator, fb_name, il_operand_list, il_param_list, symbol_c *called_fb_declaration)
void *narrow_candidate_datatypes_c::visit(il_fb_call_c *symbol) {
	symbol_c *fb_decl = symbol->called_fb_declaration;
	
	/* Although a call to a non-declared FB is a semantic error, this is currently caught by stage 2! */
	if (NULL == fb_decl) ERROR;
	if (NULL != symbol->il_operand_list)  narrow_nonformal_call(symbol, fb_decl);
	if (NULL != symbol->  il_param_list)     narrow_formal_call(symbol, fb_decl);

	/* Let the il_call_operator (CAL, CALC, or CALCN) set the datatype of prev_il_instruction... */
	symbol->il_call_operator->datatype = symbol->datatype;
	symbol->il_call_operator->accept(*this);
	return NULL;
}


/* | function_name '(' eol_list [il_param_list] ')' */
/* NOTE: The parameter 'called_function_declaration' is used to pass data between the stage 3 and stage 4. */
// SYM_REF2(il_formal_funct_call_c, function_name, il_param_list, symbol_c *called_function_declaration; int extensible_param_count;)
void *narrow_candidate_datatypes_c::visit(il_formal_funct_call_c *symbol) {
	generic_function_call_t fcall_param = {
		/* fcall_param.function_name               = */ symbol->function_name,
		/* fcall_param.nonformal_operand_list      = */ NULL,
		/* fcall_param.formal_operand_list         = */ symbol->il_param_list,
		/* enum {POU_FB, POU_function} POU_type    = */ generic_function_call_t::POU_function,
		/* fcall_param.candidate_functions         = */ symbol->candidate_functions,
		/* fcall_param.called_function_declaration = */ symbol->called_function_declaration,
		/* fcall_param.extensible_param_count      = */ symbol->extensible_param_count
	};
  
	narrow_function_invocation(symbol, fcall_param);
	/* The desired datatype of the previous il instruction was already set by narrow_function_invocation() */
	return NULL;
}


//     void *visit(il_operand_list_c *symbol);


/* | simple_instr_list il_simple_instruction */
/* This object is referenced by il_expression_c objects */
void *narrow_candidate_datatypes_c::visit(simple_instr_list_c *symbol) {
	if (symbol->n > 0)
		symbol->elements[symbol->n - 1]->datatype = symbol->datatype;

	for(int i = symbol->n-1; i >= 0; i--) {
		symbol->elements[i]->accept(*this);
	}
	return NULL;
}


// SYM_REF1(il_simple_instruction_c, il_simple_instruction, symbol_c *prev_il_instruction;)
void *narrow_candidate_datatypes_c::visit(il_simple_instruction_c *symbol)	{
  if (symbol->prev_il_instruction.size() > 1) ERROR; /* There should be no labeled insructions inside an IL expression! */
    
  il_instruction_c tmp_prev_il_instruction(NULL, NULL);
  /* the narrow algorithm will need access to the intersected candidate_datatype lists of all prev_il_instructions, as well as the 
   * list of the prev_il_instructions.
   * Instead of creating two 'global' (within the class) variables, we create a single il_instruction_c variable (fake_prev_il_instruction),
   * and shove that data into this single variable.
   */
  if (symbol->prev_il_instruction.size() > 0)
    tmp_prev_il_instruction.candidate_datatypes = symbol->prev_il_instruction[0]->candidate_datatypes;
  tmp_prev_il_instruction.prev_il_instruction = symbol->prev_il_instruction;
  
   /* copy the candidate_datatypes list */
  fake_prev_il_instruction = &tmp_prev_il_instruction;
  symbol->il_simple_instruction->datatype = symbol->datatype;
  symbol->il_simple_instruction->accept(*this);
  fake_prev_il_instruction = NULL;
  return NULL;
}

//     void *visit(il_param_list_c *symbol);
//     void *visit(il_param_assignment_c *symbol);
//     void *visit(il_param_out_assignment_c *symbol);


/*******************/
/* B 2.2 Operators */
/*******************/
/* Sets the datatype of the il_operand, and calls it recursively!
 * 
 * NOTE 1: the il_operand __may__ be pointing to a parenthesized list of IL instructions. 
 * e.g.  LD 33
 *       AND ( 45
 *            OR 56
 *            )
 *       When we handle the first 'AND' IL_operator, the il_operand will point to an simple_instr_list_c.
 *       In this case, when we call il_operand->accept(*this);, the prev_il_instruction pointer will be overwritten!
 *
 *       So, if yoy wish to set the prev_il_instruction->datatype = symbol->datatype;
 *       do it __before__ calling set_il_operand_datatype() (which in turn calls il_operand->accept(*this)) !!
 */
void *narrow_candidate_datatypes_c::set_il_operand_datatype(symbol_c *il_operand, symbol_c *datatype) {
	if (NULL == il_operand) return NULL; /* if no IL operand => error in the source code!! */

	/* If il_operand already has a non-NULL datatype (remember, narrow algorithm runs twice over IL lists!),
	 * but narrow algorithm has not yet been able to determine what datatype it should take? This is strange,
	 * and most probably an error!
	 */
	if ((NULL != il_operand->datatype) && (NULL == datatype)) ERROR;

	/* If the il_operand's datatype has already been set previously, and
	 * the narrow algorithm has already determined the datatype the il_operand should take!
	 *   ...we just make sure that the new datatype is the same as the current il_operand's datatype
	 */
	if ((NULL != il_operand->datatype)  && (NULL != datatype)) {
		/* Both datatypes are an invalid_type_name_c. This implies they are the same!! */
		if ((!get_datatype_info_c::is_type_valid(datatype)) && ((!get_datatype_info_c::is_type_valid(il_operand->datatype)))) 
			return NULL;;
		/* OK, so both the datatypes are valid, but are they equal? */
		if ( !get_datatype_info_c::is_type_equal(il_operand->datatype, datatype)) 
			ERROR; 
		/* The datatypes are the same. We have nothing to do, so we simply return! */
		return NULL;
	}

	/* Set the il_operand's datatype. Note that the new 'datatype' may even be NULL!!! */
	il_operand->datatype = datatype;
	/* Even if we are not able to determine the il_operand's datatype ('datatype' is NULL), we still visit it recursively,
	 * to give a chance of any complex expressions embedded in the il_operand (e.g. expressions inside array subscripts!) 
	 * to be narrowed too.
	 */
	il_operand->accept(*this);
	return NULL;
}




void *narrow_candidate_datatypes_c::narrow_binary_operator(const struct widen_entry widen_table[], symbol_c *symbol, bool *deprecated_operation) {
	symbol_c *prev_instruction_type, *operand_type;
	int count = 0;

        if (NULL != deprecated_operation)
		*deprecated_operation = false;

	if (NULL == il_operand) return NULL; /* if no IL operand => error in the source code!! */

	 /* NOTE 1: the il_operand __may__ be pointing to a parenthesized list of IL instructions. 
	 * e.g.  LD 33
	 *       AND ( 45
	 *            OR 56
	 *            )
	 *       When we handle the first 'AND' IL_operator, the il_operand will point to an simple_instr_list_c.
	 *       In this case, when we call il_operand->accept(*this);, the prev_il_instruction pointer will be overwritten!
	 *
	 *       We must therefore set the prev_il_instruction->datatype = symbol->datatype;
	 *       __before__ calling il_operand->accept(*this) !!
	 *
	 * NOTE 2: We do not need to call prev_il_instruction->accept(*this), as the object to which prev_il_instruction
	 *         is pointing to will be later narrowed by the call from the for() loop of the instruction_list_c
	 *         (or simple_instr_list_c), which iterates backwards.
	 */
	if (NULL != symbol->datatype) { // next IL instructions were able to determine the datatype this instruction should produce
		for(unsigned int i = 0; i < fake_prev_il_instruction->candidate_datatypes.size(); i++) {
			for(unsigned int j = 0; j < il_operand->candidate_datatypes.size(); j++) {
				prev_instruction_type = fake_prev_il_instruction->candidate_datatypes[i];
				operand_type = il_operand->candidate_datatypes[j];
				if (is_widening_compatible(widen_table, prev_instruction_type, operand_type, symbol->datatype, deprecated_operation)) {
					/* set the desired datatype of the previous il instruction */
					set_datatype_in_prev_il_instructions(prev_instruction_type, fake_prev_il_instruction);
					/* set the datatype for the operand */
					set_il_operand_datatype(il_operand, operand_type);
					
					/* NOTE: DO NOT search any further! Return immediately!
					 * Since we support SAFE*** datatypes, multiple entries in the widen_table may be compatible.
					 * If we try to set more than one distinct datatype on the same symbol, then the datatype will be set to
					 * an invalid_datatype, which is NOT what we want!
					 */
					return NULL;
				}
			}
		}
	}
	/* We were not able to determine the required datatype, but we still give the il_operand a chance to be narrowed! */
	set_il_operand_datatype(il_operand, NULL);
	return NULL;
}




/* Narrow IL operators whose execution is conditional on the boolean value in the accumulator.
 * Basically, narrow the JMPC, JMPCN, RETC, RETCN, CALC, and CALCN operators!
 * Also does part of the S and R operator narrowing!!!
 */
void *narrow_candidate_datatypes_c::narrow_conditional_operator(symbol_c *symbol) {
	/* if the next IL instructions needs us to provide a datatype other than a BOOL or a SAFEBOOL, 
	 * then we have an internal compiler error - most likely in fill_candidate_datatypes_c 
	 */
	// I (mario) am confident the fill/narrow algorithms are working correctly, so for now we can disable the assertions!
	//if ((NULL != symbol->datatype) && (!get_datatype_info_c::is_BOOL_compatible(symbol->datatype))) ERROR;
	//if (symbol->candidate_datatypes.size() > 2) ERROR; /* may contain, at most, a BOOL and a SAFEBOOL */

	/* NOTE: If there is no IL instruction following this S, R, CALC, CALCN, JMPC, JMPCN, RETC, or RETCN instruction,
	 *       we must still provide a bool_type_name_c datatype (if possible, i.e. if it exists in the candidate datatype list).
	 *       If it is not possible, we set it to NULL
	 * 
	 * NOTE: Note that this algorithm we are implementing is slightly wrong. 
	 *        (a) It ignores that a SAFEBOOL may be needed instead of a BOOL datatype. 
	 *        (b) It also ignores that this method gets to be called twice on the same 
	 *            object (the narrow algorithm runs through the IL list twice in order to
	 *            handle forward JMPs), so the assumption that we must immediately set our
	 *            own datatype if we get called with a NULL symbol->datatype is incorrect 
	 *           (it may be that the second time it is called it will be with the correct datatype!).
	 * 
	 *       These two issues (a) and (b) together means that we should only really be setting our own
	 *       datatype if we are certain that the following IL instructions will never set it for us
	 *       - basically if the following IL instruction is a LD, or a JMP to a LD, or a JMP to a JMP to a LD,
	 *        etc..., or a conditional JMP whose both branches go to LD, etc...!!!
	 *       
	 *       At the moment, it seems to me that we would need to write a visitor class to do this for us!
	 *       I currently have other things on my mind at the moment, so I will leave this for later...
	 *       For the moment we just set it to BOOL, and ignore the support of SAFEBOOL!
	 */
	if (NULL == symbol->datatype) set_datatype(&get_datatype_info_c::bool_type_name /* datatype*/, symbol /* symbol */);
	if (NULL == symbol->datatype) ERROR; // the BOOL is not on the candidate_datatypes! Strange... Probably a bug in fill_candidate_datatype_c

	/* set the required datatype of the previous IL instruction, i.e. a bool_type_name_c! */
	set_datatype_in_prev_il_instructions(symbol->datatype, fake_prev_il_instruction);
	return NULL;
}



void *narrow_candidate_datatypes_c::narrow_S_and_R_operator(symbol_c *symbol, const char *param_name, symbol_c *called_fb_declaration) {
	if (NULL != called_fb_declaration) 
	  /* FB call semantics */  
	  return narrow_implicit_il_fb_call(symbol, param_name, called_fb_declaration); 
	
	/* Set/Reset semantics */  
	narrow_conditional_operator(symbol);
	/* set the datatype for the il_operand */
	if ((NULL != il_operand) && (il_operand->candidate_datatypes.size() > 0))
		set_il_operand_datatype(il_operand, il_operand->candidate_datatypes[0]);
	return NULL;
}



void *narrow_candidate_datatypes_c::narrow_store_operator(symbol_c *symbol) {
	if (symbol->candidate_datatypes.size() == 1) {
		symbol->datatype = symbol->candidate_datatypes[0];
		/* set the desired datatype of the previous il instruction */
		set_datatype_in_prev_il_instructions(symbol->datatype, fake_prev_il_instruction);
		/* In the case of the ST operator, we must set the datatype of the il_instruction_c object that points to this ST_operator_c ourselves,
		 * since the following il_instruction_c objects have not done it, as is normal/standard for other instructions!
		 */
		current_il_instruction->datatype = symbol->datatype;
	}
	
	/* set the datatype for the operand */
	set_il_operand_datatype(il_operand, symbol->datatype);
	return NULL;
}



void *narrow_candidate_datatypes_c::visit(  LD_operator_c *symbol)  {return set_il_operand_datatype(il_operand, symbol->datatype);}
void *narrow_candidate_datatypes_c::visit( LDN_operator_c *symbol)  {return set_il_operand_datatype(il_operand, symbol->datatype);}

void *narrow_candidate_datatypes_c::visit(  ST_operator_c *symbol)  {return narrow_store_operator(symbol);}
void *narrow_candidate_datatypes_c::visit( STN_operator_c *symbol)  {return narrow_store_operator(symbol);}


/* NOTE: the standard allows syntax in which the NOT operator is followed by an optional <il_operand>
 *              NOT [<il_operand>]
 *       However, it does not define the semantic of the NOT operation when the <il_operand> is specified.
 *       We therefore consider it an error if an il_operand is specified!
 *       This error will be detected in print_datatypes_error_c!!
 */
/* This operator does not change the data type, it simply inverts the bits in the ANT_BIT data types! */
/* So, we merely set the desired datatype of the previous il instruction */
void *narrow_candidate_datatypes_c::visit( NOT_operator_c *symbol)  {set_datatype_in_prev_il_instructions(symbol->datatype, fake_prev_il_instruction);return NULL;}

void *narrow_candidate_datatypes_c::visit(   S_operator_c *symbol)  {return narrow_S_and_R_operator   (symbol, "S",   symbol->called_fb_declaration);}
void *narrow_candidate_datatypes_c::visit(   R_operator_c *symbol)  {return narrow_S_and_R_operator   (symbol, "R",   symbol->called_fb_declaration);}

void *narrow_candidate_datatypes_c::visit(  S1_operator_c *symbol)  {return narrow_implicit_il_fb_call(symbol, "S1",  symbol->called_fb_declaration);}
void *narrow_candidate_datatypes_c::visit(  R1_operator_c *symbol)  {return narrow_implicit_il_fb_call(symbol, "R1",  symbol->called_fb_declaration);}
void *narrow_candidate_datatypes_c::visit( CLK_operator_c *symbol)  {return narrow_implicit_il_fb_call(symbol, "CLK", symbol->called_fb_declaration);}
void *narrow_candidate_datatypes_c::visit(  CU_operator_c *symbol)  {return narrow_implicit_il_fb_call(symbol, "CU",  symbol->called_fb_declaration);}
void *narrow_candidate_datatypes_c::visit(  CD_operator_c *symbol)  {return narrow_implicit_il_fb_call(symbol, "CD",  symbol->called_fb_declaration);}
void *narrow_candidate_datatypes_c::visit(  PV_operator_c *symbol)  {return narrow_implicit_il_fb_call(symbol, "PV",  symbol->called_fb_declaration);}
void *narrow_candidate_datatypes_c::visit(  IN_operator_c *symbol)  {return narrow_implicit_il_fb_call(symbol, "IN",  symbol->called_fb_declaration);}
void *narrow_candidate_datatypes_c::visit(  PT_operator_c *symbol)  {return narrow_implicit_il_fb_call(symbol, "PT",  symbol->called_fb_declaration);}

void *narrow_candidate_datatypes_c::visit( AND_operator_c *symbol)  {return narrow_binary_operator(widen_AND_table, symbol);}
void *narrow_candidate_datatypes_c::visit(  OR_operator_c *symbol)  {return narrow_binary_operator( widen_OR_table, symbol);}
void *narrow_candidate_datatypes_c::visit( XOR_operator_c *symbol)  {return narrow_binary_operator(widen_XOR_table, symbol);}
void *narrow_candidate_datatypes_c::visit(ANDN_operator_c *symbol)  {return narrow_binary_operator(widen_AND_table, symbol);}
void *narrow_candidate_datatypes_c::visit( ORN_operator_c *symbol)  {return narrow_binary_operator( widen_OR_table, symbol);}
void *narrow_candidate_datatypes_c::visit(XORN_operator_c *symbol)  {return narrow_binary_operator(widen_XOR_table, symbol);}
void *narrow_candidate_datatypes_c::visit( ADD_operator_c *symbol)  {return narrow_binary_operator(widen_ADD_table, symbol, &(symbol->deprecated_operation));}
void *narrow_candidate_datatypes_c::visit( SUB_operator_c *symbol)  {return narrow_binary_operator(widen_SUB_table, symbol, &(symbol->deprecated_operation));}
void *narrow_candidate_datatypes_c::visit( MUL_operator_c *symbol)  {return narrow_binary_operator(widen_MUL_table, symbol, &(symbol->deprecated_operation));}
void *narrow_candidate_datatypes_c::visit( DIV_operator_c *symbol)  {return narrow_binary_operator(widen_DIV_table, symbol, &(symbol->deprecated_operation));}
void *narrow_candidate_datatypes_c::visit( MOD_operator_c *symbol)  {return narrow_binary_operator(widen_MOD_table, symbol);}
void *narrow_candidate_datatypes_c::visit(  GT_operator_c *symbol)  {return narrow_binary_operator(widen_CMP_table, symbol);}
void *narrow_candidate_datatypes_c::visit(  GE_operator_c *symbol)  {return narrow_binary_operator(widen_CMP_table, symbol);}
void *narrow_candidate_datatypes_c::visit(  EQ_operator_c *symbol)  {return narrow_binary_operator(widen_CMP_table, symbol);}
void *narrow_candidate_datatypes_c::visit(  LT_operator_c *symbol)  {return narrow_binary_operator(widen_CMP_table, symbol);}
void *narrow_candidate_datatypes_c::visit(  LE_operator_c *symbol)  {return narrow_binary_operator(widen_CMP_table, symbol);}
void *narrow_candidate_datatypes_c::visit(  NE_operator_c *symbol)  {return narrow_binary_operator(widen_CMP_table, symbol);}


/* visitors to CAL_operator_c, CALC_operator_c and CALCN_operator_c are called from visit(il_fb_call_c *) {symbol->il_call_operator->accept(*this)} */
/* NOTE: The CAL, JMP and RET instructions simply set the desired datatype of the previous il instruction since they do not change the value in the current/default IL variable */
void *narrow_candidate_datatypes_c::visit(  CAL_operator_c *symbol) {set_datatype_in_prev_il_instructions(symbol->datatype, fake_prev_il_instruction); return NULL;}
void *narrow_candidate_datatypes_c::visit(  RET_operator_c *symbol) {set_datatype_in_prev_il_instructions(symbol->datatype, fake_prev_il_instruction); return NULL;}
void *narrow_candidate_datatypes_c::visit(  JMP_operator_c *symbol) {set_datatype_in_prev_il_instructions(symbol->datatype, fake_prev_il_instruction); return NULL;}
void *narrow_candidate_datatypes_c::visit( CALC_operator_c *symbol) {return narrow_conditional_operator(symbol);}
void *narrow_candidate_datatypes_c::visit(CALCN_operator_c *symbol) {return narrow_conditional_operator(symbol);}
void *narrow_candidate_datatypes_c::visit( RETC_operator_c *symbol) {return narrow_conditional_operator(symbol);}
void *narrow_candidate_datatypes_c::visit(RETCN_operator_c *symbol) {return narrow_conditional_operator(symbol);}
void *narrow_candidate_datatypes_c::visit( JMPC_operator_c *symbol) {return narrow_conditional_operator(symbol);}
void *narrow_candidate_datatypes_c::visit(JMPCN_operator_c *symbol) {return narrow_conditional_operator(symbol);}

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
/* SYM_REF1(deref_expression_c, exp)  --> an extension to the IEC 61131-3 standard - based on the IEC 61131-3 v3 standard. Returns address of the varible! */
void *narrow_candidate_datatypes_c::visit(deref_expression_c  *symbol) {
  for (unsigned int i = 0; i < symbol->exp->candidate_datatypes.size(); i++) {
    symbol_c *typ = symbol->exp->candidate_datatypes[i];
    symbol_c *ref = get_datatype_info_c::get_ref_to(typ);
    if (   (get_datatype_info_c::is_ref_to(typ)) 
        && (get_datatype_info_c::is_type_equal(search_base_type_c::get_basetype_decl(ref), symbol->datatype))
       )
      /* if it points to the required datatype for symbol, then that is the required datatype for symbol->exp */
      symbol->exp->datatype = typ;
  }
  
  symbol->exp->accept(*this);
  return NULL;
}


/* SYM_REF1(deref_operator_c, exp)  --> an extension to the IEC 61131-3 standard - based on the IEC 61131-3 v3 standard. Returns address of the varible! */
void *narrow_candidate_datatypes_c::visit(deref_operator_c  *symbol) {
  for (unsigned int i = 0; i < symbol->exp->candidate_datatypes.size(); i++) {
    symbol_c *typ = symbol->exp->candidate_datatypes[i];
    symbol_c *ref = get_datatype_info_c::get_ref_to(typ);
    if (   (get_datatype_info_c::is_ref_to(typ)) 
        && (get_datatype_info_c::is_type_equal(search_base_type_c::get_basetype_decl(ref), symbol->datatype))
       )
      /* if it points to the required datatype for symbol, then that is the required datatype for symbol->exp */
      symbol->exp->datatype = typ;
  }
  
  symbol->exp->accept(*this);
  return NULL;
}


/* SYM_REF1(ref_expression_c, exp)  --> an extension to the IEC 61131-3 standard - based on the IEC 61131-3 v3 standard. Returns address of the varible! */
void *narrow_candidate_datatypes_c::visit(  ref_expression_c  *symbol) {
  if (symbol->exp->candidate_datatypes.size() > 0) {
    symbol->exp->datatype = symbol->exp->candidate_datatypes[0]; /* just use the first possible datatype */
  }
  symbol->exp->accept(*this);
  return NULL;
}



/* allow_enums is FALSE by default!!
 * deprecated_operation is NULL by default!!
 * if (allow_enums) then consider that we are ectually processing an equ_expression or notequ_expression, where two enums of the same data type may also be legally compared 
 *  e.g.      symbol := l_expr == r_expr              
 *            symbol := l_expr != r_expr
 *  In the above situation it is a legal operation when (l_expr.datatype == r_expr.datatype) && is_enumerated(r/l_expr.datatype) && is_bool(symbol.datatype)
 */
void *narrow_candidate_datatypes_c::narrow_binary_expression(const struct widen_entry widen_table[], symbol_c *symbol, symbol_c *l_expr, symbol_c *r_expr, bool *deprecated_operation, bool allow_enums) {
	symbol_c *l_type, *r_type;
	int count = 0;

	if (NULL != deprecated_operation)
		*deprecated_operation = false;

	for(unsigned int i = 0; i < l_expr->candidate_datatypes.size(); i++) {
		for(unsigned int j = 0; j < r_expr->candidate_datatypes.size(); j++) {
			/* test widening compatibility */
			l_type = l_expr->candidate_datatypes[i];
			r_type = r_expr->candidate_datatypes[j];
			if        (is_widening_compatible(widen_table, l_type, r_type, symbol->datatype, deprecated_operation)) {
				l_expr->datatype = l_type;
				r_expr->datatype = r_type;
				count ++;
			} else if (   /* handle the special case of enumerations */
			              (get_datatype_info_c::is_BOOL_compatible(symbol->datatype) && get_datatype_info_c::is_enumerated(l_type) && (l_type == r_type))
			              /* handle the special case of comparison between REF_TO datatypes */
			           || (get_datatype_info_c::is_BOOL_compatible(symbol->datatype) && get_datatype_info_c::is_ref_to    (l_type) && get_datatype_info_c::is_type_equal(l_type, r_type))) {
				if (NULL != deprecated_operation)  *deprecated_operation = false;
				l_expr->datatype = l_type;
				r_expr->datatype = r_type;
				count ++;
			}
			  
		}
	}
// 	if (count > 1) ERROR; /* Since we also support SAFE data types, this assertion is not necessarily always tru! */
	if (get_datatype_info_c::is_type_valid(symbol->datatype) && (count <= 0)) ERROR;
	
	l_expr->accept(*this);
	r_expr->accept(*this);
	return NULL;
}


void *narrow_candidate_datatypes_c::narrow_equality_comparison(const struct widen_entry widen_table[], symbol_c *symbol, symbol_c *l_expr, symbol_c *r_expr, bool *deprecated_operation) {
	return narrow_binary_expression(widen_table, symbol, l_expr, r_expr, deprecated_operation, true);
}


void *narrow_candidate_datatypes_c::visit(    or_expression_c *symbol) {return narrow_binary_expression  ( widen_OR_table, symbol, symbol->l_exp, symbol->r_exp);}
void *narrow_candidate_datatypes_c::visit(   xor_expression_c *symbol) {return narrow_binary_expression  (widen_XOR_table, symbol, symbol->l_exp, symbol->r_exp);}
void *narrow_candidate_datatypes_c::visit(   and_expression_c *symbol) {return narrow_binary_expression  (widen_AND_table, symbol, symbol->l_exp, symbol->r_exp);}

void *narrow_candidate_datatypes_c::visit(   equ_expression_c *symbol) {return narrow_equality_comparison(widen_CMP_table, symbol, symbol->l_exp, symbol->r_exp);}
void *narrow_candidate_datatypes_c::visit(notequ_expression_c *symbol) {return narrow_equality_comparison(widen_CMP_table, symbol, symbol->l_exp, symbol->r_exp);}
void *narrow_candidate_datatypes_c::visit(    lt_expression_c *symbol) {return narrow_binary_expression  (widen_CMP_table, symbol, symbol->l_exp, symbol->r_exp);}
void *narrow_candidate_datatypes_c::visit(    gt_expression_c *symbol) {return narrow_binary_expression  (widen_CMP_table, symbol, symbol->l_exp, symbol->r_exp);}
void *narrow_candidate_datatypes_c::visit(    le_expression_c *symbol) {return narrow_binary_expression  (widen_CMP_table, symbol, symbol->l_exp, symbol->r_exp);}
void *narrow_candidate_datatypes_c::visit(    ge_expression_c *symbol) {return narrow_binary_expression  (widen_CMP_table, symbol, symbol->l_exp, symbol->r_exp);}

void *narrow_candidate_datatypes_c::visit(   add_expression_c *symbol) {return narrow_binary_expression  (widen_ADD_table, symbol, symbol->l_exp, symbol->r_exp, &symbol->deprecated_operation);}
void *narrow_candidate_datatypes_c::visit(   sub_expression_c *symbol) {return narrow_binary_expression  (widen_SUB_table, symbol, symbol->l_exp, symbol->r_exp, &symbol->deprecated_operation);}
void *narrow_candidate_datatypes_c::visit(   mul_expression_c *symbol) {return narrow_binary_expression  (widen_MUL_table, symbol, symbol->l_exp, symbol->r_exp, &symbol->deprecated_operation);}
void *narrow_candidate_datatypes_c::visit(   div_expression_c *symbol) {return narrow_binary_expression  (widen_DIV_table, symbol, symbol->l_exp, symbol->r_exp, &symbol->deprecated_operation);}
void *narrow_candidate_datatypes_c::visit(   mod_expression_c *symbol) {return narrow_binary_expression  (widen_MOD_table, symbol, symbol->l_exp, symbol->r_exp);}
void *narrow_candidate_datatypes_c::visit( power_expression_c *symbol) {return narrow_binary_expression  (widen_EXPT_table,symbol, symbol->l_exp, symbol->r_exp);}


void *narrow_candidate_datatypes_c::visit(neg_expression_c *symbol) {
	symbol->exp->datatype = symbol->datatype;
	symbol->exp->accept(*this);
	return NULL;
}


void *narrow_candidate_datatypes_c::visit(not_expression_c *symbol) {
	symbol->exp->datatype = symbol->datatype;
	symbol->exp->accept(*this);
	return NULL;
}



/* NOTE: The parameter 'called_function_declaration', 'extensible_param_count' and 'candidate_functions' are used to pass data between the stage 3 and stage 4. */
/*    formal_param_list -> may be NULL ! */
/* nonformal_param_list -> may be NULL ! */
// SYM_REF3(function_invocation_c, function_name, formal_param_list, nonformal_param_list, symbol_c *called_function_declaration; int extensible_param_count; std::vector <symbol_c *> candidate_functions;)
void *narrow_candidate_datatypes_c::visit(function_invocation_c *symbol) {
	generic_function_call_t fcall_param = {
		/* fcall_param.function_name               = */ symbol->function_name,
		/* fcall_param.nonformal_operand_list      = */ symbol->nonformal_param_list,
		/* fcall_param.formal_operand_list         = */ symbol->formal_param_list,
		/* enum {POU_FB, POU_function} POU_type    = */ generic_function_call_t::POU_function,
		/* fcall_param.candidate_functions         = */ symbol->candidate_functions,
		/* fcall_param.called_function_declaration = */ symbol->called_function_declaration,
		/* fcall_param.extensible_param_count      = */ symbol->extensible_param_count
	};
  
	narrow_function_invocation(symbol, fcall_param);
	return NULL;
}

/********************/
/* B 3.2 Statements */
/********************/


/*********************************/
/* B 3.2.1 Assignment Statements */
/*********************************/

void *narrow_candidate_datatypes_c::visit(assignment_statement_c *symbol) {
	if (symbol->candidate_datatypes.size() == 1) {
		symbol->datatype = symbol->candidate_datatypes[0];
		symbol->l_exp->datatype = symbol->datatype;
		symbol->r_exp->datatype = symbol->datatype;
	}
	/* give the chance of any expressions inside array subscripts to be narrowed correctly */
	symbol->l_exp->accept(*this);
	symbol->r_exp->accept(*this);
	return NULL;
}


/*****************************************/
/* B 3.2.2 Subprogram Control Statements */
/*****************************************/

void *narrow_candidate_datatypes_c::visit(fb_invocation_c *symbol) {
	/* Note: We do not use the symbol->called_fb_declaration value (set in fill_candidate_datatypes_c)
	 *       because we try to identify any other datatype errors in the expressions used in the 
	 *       parameters to the FB call (e.g.  fb_var(var1 * 56 + func(var * 43)) )
	 *       even it the call to the FB is invalid. 
	 *       This makes sense because it may be errors in those expressions which are
	 *       making this an invalid call, so it makes sense to point them out to the user!
	 */
	symbol_c *fb_decl = search_varfb_instance_type->get_basetype_decl(symbol->fb_name);

	/* Although a call to a non-declared FB is a semantic error, this is currently caught by stage 2! */
	if (NULL == fb_decl) ERROR;
	if (NULL != symbol->nonformal_param_list)  narrow_nonformal_call(symbol, fb_decl);
	if (NULL != symbol->   formal_param_list)     narrow_formal_call(symbol, fb_decl);

	return NULL;
}


/********************************/
/* B 3.2.3 Selection Statements */
/********************************/

void *narrow_candidate_datatypes_c::visit(if_statement_c *symbol) {
	for(unsigned int i = 0; i < symbol->expression->candidate_datatypes.size(); i++) {
		if (get_datatype_info_c::is_BOOL_compatible(symbol->expression->candidate_datatypes[i]))
			symbol->expression->datatype = symbol->expression->candidate_datatypes[i];
	}
	symbol->expression->accept(*this);
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	if (NULL != symbol->elseif_statement_list)
		symbol->elseif_statement_list->accept(*this);
	if (NULL != symbol->else_statement_list)
		symbol->else_statement_list->accept(*this);
	return NULL;
}


void *narrow_candidate_datatypes_c::visit(elseif_statement_c *symbol) {
	for (unsigned int i = 0; i < symbol->expression->candidate_datatypes.size(); i++) {
		if (get_datatype_info_c::is_BOOL_compatible(symbol->expression->candidate_datatypes[i]))
			symbol->expression->datatype = symbol->expression->candidate_datatypes[i];
	}
	symbol->expression->accept(*this);
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	return NULL;
}

/* CASE expression OF case_element_list ELSE statement_list END_CASE */
// SYM_REF3(case_statement_c, expression, case_element_list, statement_list)
void *narrow_candidate_datatypes_c::visit(case_statement_c *symbol) {
	for (unsigned int i = 0; i < symbol->expression->candidate_datatypes.size(); i++) {
		if ((get_datatype_info_c::is_ANY_INT(symbol->expression->candidate_datatypes[i]))
				 || (get_datatype_info_c::is_enumerated(symbol->expression->candidate_datatypes[i])))
			symbol->expression->datatype = symbol->expression->candidate_datatypes[i];
	}
	symbol->expression->accept(*this);
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	if (NULL != symbol->case_element_list) {
		symbol->case_element_list->datatype = symbol->expression->datatype;
		symbol->case_element_list->accept(*this);
	}
	return NULL;
}

/* helper symbol for case_statement */
// SYM_LIST(case_element_list_c)
void *narrow_candidate_datatypes_c::visit(case_element_list_c *symbol) {
	for (int i = 0; i < symbol->n; i++) {
		symbol->elements[i]->datatype = symbol->datatype;
		symbol->elements[i]->accept(*this);
	}
	return NULL;
}

/*  case_list ':' statement_list */
// SYM_REF2(case_element_c, case_list, statement_list)
void *narrow_candidate_datatypes_c::visit(case_element_c *symbol) {
	symbol->case_list->datatype = symbol->datatype;
	symbol->case_list->accept(*this);
	symbol->statement_list->accept(*this);
	return NULL;
}

// SYM_LIST(case_list_c)
void *narrow_candidate_datatypes_c::visit(case_list_c *symbol) {
	for (int i = 0; i < symbol->n; i++) {
		for (unsigned int k = 0; k < symbol->elements[i]->candidate_datatypes.size(); k++) {
			if (get_datatype_info_c::is_type_equal(symbol->datatype, symbol->elements[i]->candidate_datatypes[k]))
				symbol->elements[i]->datatype = symbol->elements[i]->candidate_datatypes[k];
		}
		/* NOTE: this may be an integer, a subrange_c, or a enumerated value! */
		symbol->elements[i]->accept(*this);
	}
	return NULL;
}


/********************************/
/* B 3.2.4 Iteration Statements */
/********************************/
void *narrow_candidate_datatypes_c::visit(for_statement_c *symbol) {
	/* Control variable */
	for(unsigned int i = 0; i < symbol->control_variable->candidate_datatypes.size(); i++) {
		if (get_datatype_info_c::is_ANY_INT(symbol->control_variable->candidate_datatypes[i])) {
			symbol->control_variable->datatype = symbol->control_variable->candidate_datatypes[i];
		}
	}
	symbol->control_variable->accept(*this);
	/* BEG expression */
	for(unsigned int i = 0; i < symbol->beg_expression->candidate_datatypes.size(); i++) {
		if (get_datatype_info_c::is_type_equal(symbol->control_variable->datatype,symbol->beg_expression->candidate_datatypes[i]) &&
				get_datatype_info_c::is_ANY_INT(symbol->beg_expression->candidate_datatypes[i])) {
			symbol->beg_expression->datatype = symbol->beg_expression->candidate_datatypes[i];
		}
	}
	symbol->beg_expression->accept(*this);
	/* END expression */
	for(unsigned int i = 0; i < symbol->end_expression->candidate_datatypes.size(); i++) {
		if (get_datatype_info_c::is_type_equal(symbol->control_variable->datatype,symbol->end_expression->candidate_datatypes[i]) &&
				get_datatype_info_c::is_ANY_INT(symbol->end_expression->candidate_datatypes[i])) {
			symbol->end_expression->datatype = symbol->end_expression->candidate_datatypes[i];
		}
	}
	symbol->end_expression->accept(*this);
	/* BY expression */
	if (NULL != symbol->by_expression) {
		for(unsigned int i = 0; i < symbol->by_expression->candidate_datatypes.size(); i++) {
			if (get_datatype_info_c::is_type_equal(symbol->control_variable->datatype,symbol->by_expression->candidate_datatypes[i]) &&
					get_datatype_info_c::is_ANY_INT(symbol->by_expression->candidate_datatypes[i])) {
				symbol->by_expression->datatype = symbol->by_expression->candidate_datatypes[i];
			}
		}
		symbol->by_expression->accept(*this);
	}
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	return NULL;
}

void *narrow_candidate_datatypes_c::visit(while_statement_c *symbol) {
	for (unsigned int i = 0; i < symbol->expression->candidate_datatypes.size(); i++) {
		if(get_datatype_info_c::is_BOOL(symbol->expression->candidate_datatypes[i]))
			symbol->expression->datatype = symbol->expression->candidate_datatypes[i];
	}
	symbol->expression->accept(*this);
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	return NULL;
}

void *narrow_candidate_datatypes_c::visit(repeat_statement_c *symbol) {
	for (unsigned int i = 0; i < symbol->expression->candidate_datatypes.size(); i++) {
		if(get_datatype_info_c::is_BOOL(symbol->expression->candidate_datatypes[i]))
			symbol->expression->datatype = symbol->expression->candidate_datatypes[i];
	}
	symbol->expression->accept(*this);
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	return NULL;
}





