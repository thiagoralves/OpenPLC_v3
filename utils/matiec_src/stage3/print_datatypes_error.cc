/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2009-2011  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2011-2012 Manuele Conti (manuele.conti@sirius-es.it)
 *  Copyright (C) 2011-2012 Matteo Facchinetti (matteo.facchinetti@sirius-es.it)
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

/* NOTE: The algorithm implemented here assumes that the symbol_c.candidate_datatype, and the symbol_c.datatype 
 *       annotations have already been apropriately filled in!
 *       BEFORE running this visitor, be sure to CALL the fill_candidate_datatypes_c, and the narrow_candidate_datatypes_c visitors!
 */


/*
 *  By analysing the candidate datatype lists, as well as the chosen datatype for each expression, determine
 *  if an datatype error has been found, and if so, print out an error message.
 */


#include "print_datatypes_error.hh"
#include "datatype_functions.hh"

#include <typeinfo>
#include <list>
#include <string>
#include <string.h>
#include <strings.h>






#define FIRST_(symbol1, symbol2) (((symbol1)->first_order < (symbol2)->first_order)   ? (symbol1) : (symbol2))
#define  LAST_(symbol1, symbol2) (((symbol1)->last_order  > (symbol2)->last_order)    ? (symbol1) : (symbol2))

#define STAGE3_ERROR(error_level, symbol1, symbol2, ...) {                                                                  \
  if (current_display_error_level >= error_level) {                                                                         \
    fprintf(stderr, "%s:%d-%d..%d-%d: error: ",                                                                             \
            FIRST_(symbol1,symbol2)->first_file, FIRST_(symbol1,symbol2)->first_line, FIRST_(symbol1,symbol2)->first_column,\
                                                 LAST_(symbol1,symbol2) ->last_line,  LAST_(symbol1,symbol2) ->last_column);\
    fprintf(stderr, __VA_ARGS__);                                                                                           \
    fprintf(stderr, "\n");                                                                                                  \
    il_error = true;                                                                                                        \
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


/* set to 1 to see debug info during execution */
static int debug = 0;

print_datatypes_error_c::print_datatypes_error_c(symbol_c *ignore) {
	error_count = 0;
	warning_found = false;
	current_display_error_level = error_level_default;
}

print_datatypes_error_c::~print_datatypes_error_c(void) {
}

int print_datatypes_error_c::get_error_count() {
	return error_count;
}





/* Verify if the datatypes of all symbols in the vector are valid and equal!  */
static bool are_all_datatypes_equal(std::vector <symbol_c *> &symbol_vect) {
	if (symbol_vect.size() <= 0) return false;

	bool res = get_datatype_info_c::is_type_valid(symbol_vect[0]->datatype);
	for (unsigned int i = 1; i < symbol_vect.size(); i++)
		res &= get_datatype_info_c::is_type_equal(symbol_vect[i-1]->datatype, symbol_vect[i]->datatype);	
	return res;
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
void print_datatypes_error_c::handle_function_invocation(symbol_c *fcall, generic_function_call_t fcall_data) {
	symbol_c *param_value, *param_name;
	function_call_param_iterator_c fcp_iterator(fcall);
	bool function_invocation_error = false;
	const char *POU_str = NULL;

	if (generic_function_call_t::POU_FB       == fcall_data.POU_type)  POU_str = "FB";
	if (generic_function_call_t::POU_function == fcall_data.POU_type)  POU_str = "function";
	if (NULL == POU_str) ERROR;

	if ((NULL != fcall_data.formal_operand_list) && (NULL != fcall_data.nonformal_operand_list)) 
		ERROR;

	symbol_c *f_decl = fcall_data.called_function_declaration;
	if ((NULL == f_decl) && (generic_function_call_t::POU_FB ==fcall_data.POU_type)) {
		/* Due to the way the syntax analysis is buit (i.e. stage 2), this should never occur. */
		/* I.e., a FB invocation using an undefined FB variable is not possible in the current implementation of stage 2. */
		ERROR;
	}
	if (NULL == f_decl) {
		/* we now try to find any function declaration with the same name, just so we can provide some relevant error messages */
		function_symtable_t::iterator lower = function_symtable.lower_bound(fcall_data.function_name);
		if (lower == function_symtable.end()) ERROR;
		f_decl = function_symtable.get_value(lower);
	}

	if (NULL != fcall_data.formal_operand_list) {
		fcall_data.formal_operand_list->accept(*this);
		if (NULL != f_decl) {
			function_param_iterator_c fp_iterator(f_decl);
			while ((param_name = fcp_iterator.next_f()) != NULL) {
				param_value = fcp_iterator.get_current_value();
				
				/* Check if there are duplicate parameter values */
				if(fcp_iterator.search_f(param_name) != param_value) {
					function_invocation_error = true;
					STAGE3_ERROR(0, param_name, param_name, "Duplicate parameter '%s' when invoking %s '%s'", ((token_c *)param_name)->value, POU_str, ((token_c *)fcall_data.function_name)->value);
					continue; /* jump to next parameter */
				}

				/* Find the corresponding parameter in function declaration */
				if (NULL == fp_iterator.search(param_name)) {
					function_invocation_error = true;
					STAGE3_ERROR(0, param_name, param_name, "Invalid parameter '%s' when invoking %s '%s'", ((token_c *)param_name)->value, POU_str, ((token_c *)fcall_data.function_name)->value);
					continue; /* jump to next parameter */
				} 

				/* check whether direction (IN, OUT, IN_OUT) and assignment types (:= , =>) are compatible !!! */
				/* Obtaining the assignment direction:  := (assign_in) or => (assign_out) */
				function_call_param_iterator_c::assign_direction_t call_param_dir = fcp_iterator.get_assign_direction();
				/* Get the parameter direction: IN, OUT, IN_OUT */
				function_param_iterator_c::param_direction_t param_dir = fp_iterator.param_direction();
				if          (function_call_param_iterator_c::assign_in  == call_param_dir) {
					if ((function_param_iterator_c::direction_in    != param_dir) &&
					    (function_param_iterator_c::direction_inout != param_dir)) {
						function_invocation_error = true;
						STAGE3_ERROR(0, param_name, param_name, "Invalid assignment syntax ':=' used for parameter '%s', when invoking %s '%s'", ((token_c *)param_name)->value, POU_str, ((token_c *)fcall_data.function_name)->value);
						continue; /* jump to next parameter */
					}
				} else if   (function_call_param_iterator_c::assign_out == call_param_dir) {
					if ((function_param_iterator_c::direction_out   != param_dir)) {
						function_invocation_error = true;
						STAGE3_ERROR(0, param_name, param_name, "Invalid assignment syntax '=>' used for parameter '%s', when invoking %s '%s'", ((token_c *)param_name)->value, POU_str, ((token_c *)fcall_data.function_name)->value);
						continue; /* jump to next parameter */
					}
				} else ERROR;

				if (!get_datatype_info_c::is_type_valid(param_value->datatype)) {
					function_invocation_error = true;
					STAGE3_ERROR(0, param_value, param_value, "Data type incompatibility between parameter '%s' and value being passed, when invoking %s '%s'", ((token_c *)param_name)->value, POU_str, ((token_c *)fcall_data.function_name)->value);
					continue; /* jump to next parameter */
				}
			}
		}
	}
	if (NULL != fcall_data.nonformal_operand_list) {
		if (f_decl)
			for (int i = 1; (param_value = fcp_iterator.next_nf()) != NULL; i++) {
		  		/* TODO: verify if it is lvalue when INOUT or OUTPUT parameters! */

				/* This handle_function_invocation() will be called to handle IL function calls, where the first parameter comes from the previous IL instruction.
				 * In this case, the previous IL instruction will be artifically (and temporarily) added to the begining ot the parameter list
				 * so we (in this function) can handle this situation like all the other function calls.
				 * However, 
				 *     a) if NO previous IL function exists, then we get a fake previous IL function, with no location data (i.e. not found anywhere in the source code.
				 *     b) the function call may actually have several prev IL instructions (if several JMP instructions jump directly to the il function call).
				 * In order to handle these situations gracefully, we first check whether the first parameter is really an IL istruction!
				 */
				il_instruction_c *il_instruction_symbol = dynamic_cast<il_instruction_c *>(param_value);
				if ((NULL != il_instruction_symbol) && (i == 1)) {
					/* We are in a situation where an IL function call is passed the first parameter, which is actually the previous IL instruction */
					/* However, this is really a fake previous il instruction (see visit(il_instruction_c *) )
					 * We will iterate through all the real previous IL instructions, and analyse each of them one by one */
					if (il_instruction_symbol->prev_il_instruction.size() == 0) {
						function_invocation_error = true;
						STAGE3_ERROR(0, fcall, fcall, "No available data to pass to first parameter of IL function %s. Missing a previous LD instruction?", ((token_c *)fcall_data.function_name)->value);
					}
#if 0
					/* NOTE: We currently comment out this code...
					 * This does not currently work, since the narrow operation is currently done on the intersection 
					 * of all the previous IL instructions, so we currently either accept them all, or none at all.
					 * In order to be able to produce these user freindly error messages, we will need to update the 
					 * narrow algorithm. We leave this untill somebody aks for it...
					 * So, for now, we simply comment out this code.
					 */
					for (unsigned int p = 0; p < il_instruction_symbol->prev_il_instruction.size(); p++) {
						symbol_c *value = il_instruction_symbol->prev_il_instruction[p];  
						if (!get_datatype_info_c::is_type_valid(value->datatype)) {
							function_invocation_error = true;
							STAGE3_ERROR(0, fcall, fcall, "Data type incompatibility for value passed to first parameter when invoking function '%s'", ((token_c *)fcall_data.function_name)->value);
							STAGE3_ERROR(0, value, value, "This is the IL instruction producing the incompatible data type to first parameter of function '%s'", ((token_c *)fcall_data.function_name)->value);
						}
					}
#else
					if (!get_datatype_info_c::is_type_valid(il_instruction_symbol->datatype)) {
						function_invocation_error = true;
						STAGE3_ERROR(0, fcall, fcall, "Data type incompatibility between value in IL 'accumulator' and first parameter of function '%s'", ((token_c *)fcall_data.function_name)->value);
					}
#endif
					if (function_invocation_error)
						/* when handling a IL function call, and an error is found in the first parameter, then we bug out and do not print out any more error messages. */
						return;
				} else {
					if (!get_datatype_info_c::is_type_valid(param_value->datatype)) {
						function_invocation_error = true;
						STAGE3_ERROR(0, param_value, param_value, "Data type incompatibility for value passed in position %d when invoking %s '%s'", i, POU_str, ((token_c *)fcall_data.function_name)->value);
					}
					param_value->accept(*this);
				}
			}
	}

	if (NULL == fcall_data.called_function_declaration) {
		function_invocation_error = true;
		STAGE3_ERROR(0, fcall, fcall, "Unable to resolve which overloaded %s '%s' is being invoked.", POU_str, ((token_c *)fcall_data.function_name)->value);
	}

	if (function_invocation_error) {
		/* No compatible function exists */
		STAGE3_ERROR(2, fcall, fcall, "Invalid parameters when invoking %s '%s'", POU_str, ((token_c *)fcall_data.function_name)->value);
	} 

	return;
}



void *print_datatypes_error_c::handle_implicit_il_fb_invocation(const char *param_name, symbol_c *il_operator, symbol_c *called_fb_declaration) {
	if (NULL == il_operand) {
		STAGE3_ERROR(0, il_operator, il_operator, "Missing operand for FB call operator '%s'.", param_name);
		return NULL;
	}
	// il_operand->accept(*this);  // NOT required. The il_simple_operation_c already visits it!!
	
	if (NULL == called_fb_declaration) {
		STAGE3_ERROR(0, il_operator, il_operand, "Invalid FB call: operand is not a FB instance.");
		return NULL;
	}

	if (fake_prev_il_instruction->prev_il_instruction.empty()) {
		STAGE3_ERROR(0, il_operator, il_operand, "FB invocation operator '%s' must be preceded by a 'LD' (or equivalent) operator.", param_name);	
		return NULL;
	}

	/* Find the corresponding parameter in function declaration */
	function_param_iterator_c fp_iterator(called_fb_declaration);
	if (NULL == fp_iterator.search(param_name)) {
		/* TODO: must also check whther it is an IN parameter!! */
		/* NOTE: although all standard FBs have the implicit FB calls defined as input parameters
		*        (i.e., for all standard FBs, CLK, PT, IN, CU, CD, S1, R1, etc... is always an input parameter)
		*        if a non-standard (i.e. a FB not defined in the standard library) FB is being called, then
		*        this (CLK, PT, IN, CU, ...) parameter may just have been defined as OUT or INOUT,
		*        which will not work for an implicit FB call!
		*/
		STAGE3_ERROR(0, il_operator, il_operand, "FB called by '%s' operator does not have a parameter named '%s'", param_name, param_name);	
		return NULL;
	}
	if (!are_all_datatypes_equal(fake_prev_il_instruction->prev_il_instruction)) {
		STAGE3_ERROR(0, il_operator, il_operand, "Data type incompatibility between parameter '%s' and value being passed.", param_name);
		return NULL;
	}
	

	/* NOTE: The error_level currently being used for errors in variables/constants etc... is rather high.
	 *       However, in the case of an implicit FB call, if the datatype of the operand == NULL, this may be
	 *       the __only__ indication of an error! So we test it here again, to make sure thtis error will really
	 *       be printed out!
	 */
	if (!get_datatype_info_c::is_type_valid(il_operand->datatype)) {
		/* Note: the case of (NULL == fb_declaration) was already caught above! */
// 		if (NULL != fb_declaration) {
			STAGE3_ERROR(0, il_operator, il_operator, "Invalid FB call: Datatype incompatibility between the FB's '%s' parameter and value being passed, or paramater '%s' is not a 'VAR_INPUT' parameter.", param_name, param_name);
			return NULL;
// 		}
	}

	return NULL;
}


/*********************/
/* B 1.2 - Constants */
/*********************/
/******************************/
/* B 1.2.1 - Numeric Literals */
/******************************/
void *print_datatypes_error_c::visit(real_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Numerical value exceeds range for ANY_REAL data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_REAL data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(integer_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Numerical value exceeds range for ANY_INT data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_INT data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(neg_real_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Numerical value exceeds range for ANY_REAL data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_REAL data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(neg_integer_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Numerical value exceeds range for ANY_INT data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_INT data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(binary_integer_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Numerical value exceeds range for ANY_INT data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_INT data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(octal_integer_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Numerical value exceeds range for ANY_INT data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_INT data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(hex_integer_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Numerical value exceeds range for ANY_INT data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_INT data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(integer_literal_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Numerical value exceeds range for %s data type.", get_datatype_info_c::get_id_str(symbol->type));
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_INT data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(real_literal_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Numerical value exceeds range for %s data type.", get_datatype_info_c::get_id_str(symbol->type));
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_REAL data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(bit_string_literal_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Numerical value exceeds range for %s data type.", get_datatype_info_c::get_id_str(symbol->type));
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_BIT data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(boolean_literal_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Value is not valid for %s data type.", get_datatype_info_c::get_id_str(symbol->type));
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_BOOL data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(boolean_true_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Value is not valid for ANY_BOOL data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_BOOL data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(boolean_false_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Value is not valid for ANY_BOOL data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "ANY_BOOL data type not valid in this location.");
	}
	return NULL;
}

/*******************************/
/* B.1.2.2   Character Strings */
/*******************************/
void *print_datatypes_error_c::visit(double_byte_character_string_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Numerical value exceeds range for WSTRING data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "WSTRING data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(single_byte_character_string_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Numerical value exceeds range for STRING data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "STRING data type not valid in this location.");
	}
	return NULL;
}

/***************************/
/* B 1.2.3 - Time Literals */
/***************************/
/************************/
/* B 1.2.3.1 - Duration */
/************************/
void *print_datatypes_error_c::visit(duration_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Invalid syntax for TIME data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "TIME data type not valid in this location.");
	}
	return NULL;
}

/************************************/
/* B 1.2.3.2 - Time of day and Date */
/************************************/
void *print_datatypes_error_c::visit(time_of_day_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Invalid syntax for TOD data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "TOD data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(date_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Invalid syntax for DATE data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "DATE data type not valid in this location.");
	}
	return NULL;
}

void *print_datatypes_error_c::visit(date_and_time_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) {
		STAGE3_ERROR(0, symbol, symbol, "Invalid syntax for DT data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		STAGE3_ERROR(4, symbol, symbol, "DT data type not valid in this location.");
	}
	return NULL;
}

/**********************/
/* B 1.3 - Data types */
/**********************/
/********************************/
/* B 1.3.3 - Derived data types */
/********************************/
void *print_datatypes_error_c::visit(simple_spec_init_c *symbol) {
	if (!get_datatype_info_c::is_type_valid(symbol->simple_specification->datatype)) {
		STAGE3_ERROR(0, symbol->simple_specification, symbol->simple_specification, "Invalid data type.");
	} else if (NULL != symbol->constant) {
		if (!get_datatype_info_c::is_type_valid(symbol->constant->datatype))
			STAGE3_ERROR(0, symbol->constant, symbol->constant, "Initial value has incompatible data type.");
	} else if (!get_datatype_info_c::is_type_valid(symbol->datatype)) {
		ERROR; /* If we have an error here, then we must also have an error in one of
		        * the two previous tests. If we reach this point, some strange error is ocurring!
			*/
	}
	return NULL;
}


void *print_datatypes_error_c::visit(enumerated_value_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0)
		STAGE3_ERROR(0, symbol, symbol, "Ambiguous enumerate value or Variable not declared in this scope.");
	return NULL;
}


/*********************/
/* B 1.4 - Variables */
/*********************/
void *print_datatypes_error_c::visit(symbolic_variable_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0)
		STAGE3_ERROR(0, symbol, symbol, "Variable not declared in this scope.");
	return NULL;
}

/********************************************/
/* B 1.4.1 - Directly Represented Variables */
/********************************************/
void *print_datatypes_error_c::visit(direct_variable_c *symbol) {
	if (symbol->candidate_datatypes.size() == 0) ERROR;
	if (!get_datatype_info_c::is_type_valid(symbol->datatype))
		STAGE3_ERROR(4, symbol, symbol, "Direct variable has incompatible data type with expression.");
	return NULL;
}

/*************************************/
/* B 1.4.2 - Multi-element variables */
/*************************************/
/*  subscripted_variable '[' subscript_list ']' */
// SYM_REF2(array_variable_c, subscripted_variable, subscript_list)
void *print_datatypes_error_c::visit(array_variable_c *symbol) {
	/* the subscripted variable may be a structure or another array variable, that must also be visisted! */
	/* Please read the comments in the array_variable_c and structured_variable_c visitors in the fill_candidate_datatypes.cc file! */
	symbol->subscripted_variable->accept(*this); 
	
	if (symbol->candidate_datatypes.size() == 0)
		STAGE3_ERROR(0, symbol, symbol, "Array variable not declared in this scope.");
	
	/* recursively call the subscript list to print any errors in the expressions used in the subscript...*/
	symbol->subscript_list->accept(*this);
	return NULL;
}

/* subscript_list ',' subscript */
// SYM_LIST(subscript_list_c)
void *print_datatypes_error_c::visit(subscript_list_c *symbol) {
	for (int i = 0; i < symbol->n; i++) {
		int start_error_count = error_count;
		symbol->elements[i]->accept(*this);
		/* The following error message will only get printed if the current_display_error_level is set higher than 0! */
		if ((start_error_count == error_count) && (!get_datatype_info_c::is_type_valid(symbol->elements[i]->datatype)))
			STAGE3_ERROR(0, symbol, symbol, "Invalid data type for array subscript field.");
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
void *print_datatypes_error_c::visit(structured_variable_c *symbol) {
	/* the record variable may be another structure or even an array variable, that must also be visisted! */
	/* Please read the comments in the array_variable_c and structured_variable_c visitors in the fill_candidate_datatypes.cc file! */
	symbol->record_variable->accept(*this);
	
	if (symbol->candidate_datatypes.size() == 0)
		STAGE3_ERROR(0, symbol, symbol, "Undeclared structured (or FB) variable, or non-existant field (variable) in structure (FB).");
	return NULL;
}



/******************************************/
/* B 1.4.3 - Declaration & Initialisation */
/******************************************/

/*  AT direct_variable */
// SYM_REF1(location_c, direct_variable)
void *print_datatypes_error_c::visit(location_c *symbol) {
	symbol->direct_variable->accept(*this);
	return NULL;
}


/*  [variable_name] location ':' located_var_spec_init */
/* variable_name -> may be NULL ! */
// SYM_REF3(located_var_decl_c, variable_name, location, located_var_spec_init)
void *print_datatypes_error_c::visit(located_var_decl_c *symbol) {
  symbol->located_var_spec_init->accept(*this);
  /* It does not make sense to call symbol->location->accept(*this). The check is done right here if the following if() */
  // symbol->location->accept(*this); 
  if ((get_datatype_info_c::is_type_valid(symbol->located_var_spec_init->datatype)) && (!get_datatype_info_c::is_type_valid(symbol->location->datatype)))
    STAGE3_ERROR(0, symbol, symbol, "Bit size of data type is incompatible with bit size of location.");
  return NULL;
}  


/************************************/
/* B 1.5 Program organization units */
/************************************/
/*********************/
/* B 1.5.1 Functions */
/*********************/
void *print_datatypes_error_c::visit(function_declaration_c *symbol) {
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
 	symbol->var_declarations_list->accept(*this);
	if (debug) printf("Print error data types list in body of function %s\n", ((token_c *)(symbol->derived_function_name))->value);
	il_parenthesis_level = 0;
	il_error = false;
	symbol->function_body->accept(*this);
	delete search_varfb_instance_type;
	search_varfb_instance_type = NULL;
	return NULL;
}

/***************************/
/* B 1.5.2 Function blocks */
/***************************/
void *print_datatypes_error_c::visit(function_block_declaration_c *symbol) {
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
 	symbol->var_declarations->accept(*this);
	if (debug) printf("Print error data types list in body of FB %s\n", ((token_c *)(symbol->fblock_name))->value);
	il_parenthesis_level = 0;
	il_error = false;
	symbol->fblock_body->accept(*this);
	delete search_varfb_instance_type;
	search_varfb_instance_type = NULL;
	return NULL;
}

/**********************/
/* B 1.5.3 - Programs */
/**********************/
void *print_datatypes_error_c::visit(program_declaration_c *symbol) {
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
	symbol->var_declarations->accept(*this);
	if (debug) printf("Print error data types list in body of program %s\n", ((token_c *)(symbol->program_type_name))->value);
	il_parenthesis_level = 0;
	il_error = false;
	symbol->function_block_body->accept(*this);
	delete search_varfb_instance_type;
	search_varfb_instance_type = NULL;
	return NULL;
}


/********************************************/
/* B 1.6 Sequential function chart elements */
/********************************************/
void *print_datatypes_error_c::visit(transition_condition_c *symbol) {
	if (symbol->transition_condition_il != NULL)   symbol->transition_condition_il->accept(*this);
	if (symbol->transition_condition_st != NULL)   symbol->transition_condition_st->accept(*this);

	if (!get_datatype_info_c::is_type_valid(symbol->datatype))
		STAGE3_ERROR(0, symbol, symbol, "Transition condition has invalid data type (should be BOOL).");
	return NULL;
}


/********************************/
/* B 1.7 Configuration elements */
/********************************/
void *print_datatypes_error_c::visit(configuration_declaration_c *symbol) {
	// TODO !!!
	/* for the moment we must return NULL so semantic analysis of remaining code is not interrupted! */
	return NULL;
}

/****************************************/
/* B.2 - Language IL (Instruction List) */
/****************************************/
/***********************************/
/* B 2.1 Instructions and Operands */
/***********************************/

// void *visit(instruction_list_c *symbol);

/* | label ':' [il_incomplete_instruction] eol_list */
// SYM_REF2(il_instruction_c, label, il_instruction)
void *print_datatypes_error_c::visit(il_instruction_c *symbol) {
	if (NULL != symbol->il_instruction) {
		il_instruction_c tmp_prev_il_instruction(NULL, NULL);
#if 0
		/* NOTE: The following is currently no longer needed. Since the following code is actually cool, 
		 * we don't delete it, but simply comment it out. It might just come in handy later on...
		 */
		/* When handling a il function call, this fake_prev_il_instruction may be used as a standard function call parameter, so it is important that 
		 * it contain some valid location info so error messages make sense.
		 */
		if (symbol->prev_il_instruction.size() > 0) {
			/* since we don't want to copy all that data one variable at a time, we copy it all at once */
			/* This has the advantage that, if we ever add some more data to the base symbol_c later on, we will not need to
			 * change the following line to guarantee that the data is copied correctly!
			 * However, it does have the drawback of copying more data than what we want!
			 * In order to only copy the data in the base class symbol_c, we use the tmp_symbol pointer!
			 * I (mario) have checked with a debugger, and it is working as intended!
			 */
			symbol_c *tmp_symbol1 = symbol->prev_il_instruction[0];
			symbol_c *tmp_symbol2 = &tmp_prev_il_instruction;
			*tmp_symbol2 = *tmp_symbol1;
			/* we do not want to copy the datatype variable, so we reset it to NULL */
			tmp_prev_il_instruction.datatype = NULL;
			/* We don't need to worry about the candidate_datatype list (which we don't want to copy just yet), since that will 
			 * be reset to the correct value when we call intersect_prev_candidate_datatype_lists() later on...
			 */
		}
#endif
		/* the print error algorithm will need access to the intersected candidate_datatype lists of all prev_il_instructions, as well as the 
		 * list of the prev_il_instructions.
		 * Instead of creating two 'global' (within the class) variables, we create a single il_instruction_c variable (fake_prev_il_instruction),
		 * and shove that data into this single variable.
		 */
		tmp_prev_il_instruction.prev_il_instruction = symbol->prev_il_instruction;
		intersect_prev_candidate_datatype_lists(&tmp_prev_il_instruction);
		if (are_all_datatypes_equal(symbol->prev_il_instruction))
			if (symbol->prev_il_instruction.size() > 0)
				tmp_prev_il_instruction.datatype = (symbol->prev_il_instruction[0])->datatype;
		
		/* Tell the il_instruction the datatype that it must generate - this was chosen by the next il_instruction (remember: we are iterating backwards!) */
		fake_prev_il_instruction = &tmp_prev_il_instruction;
		symbol->il_instruction->accept(*this);
		fake_prev_il_instruction = NULL;
	}

	return NULL;
}



void *print_datatypes_error_c::visit(il_simple_operation_c *symbol) {
	il_operand = symbol->il_operand;
	if (NULL != il_operand)     symbol->il_operand->accept(*this); /* recursive call to see whether data types are compatible */

	symbol->il_simple_operator->accept(*this);
	il_operand = NULL;
	return NULL;
}

/* | function_name [il_operand_list] */
/* NOTE: The parameters 'called_function_declaration' and 'extensible_param_count' are used to pass data between the stage 3 and stage 4. */
// SYM_REF2(il_function_call_c, function_name, il_operand_list, symbol_c *called_function_declaration; int extensible_param_count;)
void *print_datatypes_error_c::visit(il_function_call_c *symbol) {
	/* The first parameter of a non formal function call in IL will be the 'current value' (i.e. the prev_il_instruction)
	 * In order to be able to handle this without coding special cases, we will simply prepend that symbol
	 * to the il_operand_list, and remove it after calling handle_function_call().
	 *
	 * However, if no further paramters are given, then il_operand_list will be NULL, and we will
	 * need to create a new object to hold the pointer to prev_il_instruction.
	 * This change will also be undone later in print_datatypes_error_c.
	 */
	if (NULL == symbol->il_operand_list)  symbol->il_operand_list = new il_operand_list_c;
	if (NULL == symbol->il_operand_list)  ERROR;

	((list_c *)symbol->il_operand_list)->insert_element(fake_prev_il_instruction, 0);

	generic_function_call_t fcall_param = {
		/* fcall_param.function_name               = */ symbol->function_name,
		/* fcall_param.nonformal_operand_list      = */ symbol->il_operand_list,
		/* fcall_param.formal_operand_list         = */ NULL,
		/* enum {POU_FB, POU_function} POU_type    = */ generic_function_call_t::POU_function,
		/* fcall_param.candidate_functions         = */ symbol->candidate_functions,
		/* fcall_param.called_function_declaration = */ symbol->called_function_declaration,
		/* fcall_param.extensible_param_count      = */ symbol->extensible_param_count
	};

/* TODO: check what error message (if any) the compiler will give out if this function invocation
 * is not preceded by a LD operator (or another equivalent operator or list of operators).
 */
	handle_function_invocation(symbol, fcall_param);
	
	/* We now undo those changes to the abstract syntax tree made above! */
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
void *print_datatypes_error_c::visit(il_expression_c *symbol) {
  /* Stage2 will insert an artificial (and equivalent) LD <il_operand> to the simple_instr_list if necessary. We can therefore ignore the 'il_operand' entry! */
  
  /* first give the parenthesised IL list a chance to print errors */
  il_instruction_c *save_fake_prev_il_instruction = fake_prev_il_instruction;
  symbol->simple_instr_list->accept(*this);
  fake_prev_il_instruction = save_fake_prev_il_instruction;

  /* Now handle the operation (il_expr_operator) that will use the result coming from the parenthesised IL list (i.e. simple_instr_list) */
  il_operand = symbol->simple_instr_list; /* This is not a bug! The parenthesised expression will be used as the operator! */
  symbol->il_expr_operator->accept(*this);

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
void *print_datatypes_error_c::visit(il_fb_call_c *symbol) {
	int extensible_param_count;                      /* unused vairable! Needed for compilation only! */
	std::vector <symbol_c *> candidate_functions;    /* unused vairable! Needed for compilation only! */
	generic_function_call_t fcall_param = {
		/* fcall_param.function_name               = */ symbol->fb_name,
		/* fcall_param.nonformal_operand_list      = */ symbol->il_operand_list,
		/* fcall_param.formal_operand_list         = */ symbol->il_param_list,
		/* enum {POU_FB, POU_function} POU_type    = */ generic_function_call_t::POU_FB,
		/* fcall_param.candidate_functions         = */ candidate_functions,             /* will not be used, but must provide a reference to be able to compile */
		/* fcall_param.called_function_declaration = */ symbol->called_fb_declaration,
		/* fcall_param.extensible_param_count      = */ extensible_param_count           /* will not be used, but must provide a reference to be able to compile */
	};
  
	handle_function_invocation(symbol, fcall_param);
	/* check the semantics of the CALC, CALCN operators! */
	symbol->il_call_operator->accept(*this);
	return NULL;
}

/* | function_name '(' eol_list [il_param_list] ')' */
/* NOTE: The parameter 'called_function_declaration' is used to pass data between the stage 3 and stage 4. */
// SYM_REF2(il_formal_funct_call_c, function_name, il_param_list, symbol_c *called_function_declaration; int extensible_param_count;)
void *print_datatypes_error_c::visit(il_formal_funct_call_c *symbol) {
	generic_function_call_t fcall_param = {
		/* fcall_param.function_name               = */ symbol->function_name,
		/* fcall_param.nonformal_operand_list      = */ NULL,
		/* fcall_param.formal_operand_list         = */ symbol->il_param_list,
		/* enum {POU_FB, POU_function} POU_type    = */ generic_function_call_t::POU_function,
		/* fcall_param.candidate_functions         = */ symbol->candidate_functions,
		/* fcall_param.called_function_declaration = */ symbol->called_function_declaration,
		/* fcall_param.extensible_param_count      = */ symbol->extensible_param_count
	};
  
	handle_function_invocation(symbol, fcall_param);
	return NULL;
}


//     void *visit(il_operand_list_c *symbol);
//     void *visit(simple_instr_list_c *symbol);

// SYM_REF1(il_simple_instruction_c, il_simple_instruction, symbol_c *prev_il_instruction;)
void *print_datatypes_error_c::visit(il_simple_instruction_c *symbol)	{
  if (symbol->prev_il_instruction.size() > 1) ERROR; /* There should be no labeled insructions inside an IL expression! */
    
  il_instruction_c tmp_prev_il_instruction(NULL, NULL);
#if 0
  /* the print error algorithm will need access to the intersected candidate_datatype lists of all prev_il_instructions, as well as the 
   * list of the prev_il_instructions.
   * Instead of creating two 'global' (within the class) variables, we create a single il_instruction_c variable (fake_prev_il_instruction),
   * and shove that data into this single variable.
   */
  if (symbol->prev_il_instruction.size() > 0)
    tmp_prev_il_instruction.candidate_datatypes = symbol->prev_il_instruction[0]->candidate_datatypes;
  tmp_prev_il_instruction.prev_il_instruction = symbol->prev_il_instruction;
#endif
  
  /* the print error algorithm will need access to the intersected candidate_datatype lists of all prev_il_instructions, as well as the 
   * list of the prev_il_instructions.
   * Instead of creating two 'global' (within the class) variables, we create a single il_instruction_c variable (fake_prev_il_instruction),
   * and shove that data into this single variable.
   */
  tmp_prev_il_instruction.prev_il_instruction = symbol->prev_il_instruction;
  intersect_prev_candidate_datatype_lists(&tmp_prev_il_instruction);
  if (are_all_datatypes_equal(symbol->prev_il_instruction))
    if (symbol->prev_il_instruction.size() > 0)
      tmp_prev_il_instruction.datatype = (symbol->prev_il_instruction[0])->datatype;
  
  
   /* copy the candidate_datatypes list */
  fake_prev_il_instruction = &tmp_prev_il_instruction;
  symbol->il_simple_instruction->accept(*this);
  fake_prev_il_instruction = NULL;
  return NULL;
  
  
//   if (symbol->prev_il_instruction.size() == 0)  prev_il_instruction = NULL;
//   else                                          prev_il_instruction = symbol->prev_il_instruction[0];

//   symbol->il_simple_instruction->accept(*this);
//   prev_il_instruction = NULL;
//   return NULL;
}


//     void *visit(il_param_list_c *symbol);
//     void *visit(il_param_assignment_c *symbol);
//     void *visit(il_param_out_assignment_c *symbol);

/*******************/
/* B 2.2 Operators */
/*******************/
void *print_datatypes_error_c::print_binary_operator_errors(const char *il_operator, symbol_c *symbol, bool deprecated_operation) {
	if (NULL == il_operand) {
		STAGE3_ERROR(0, symbol, symbol, "Missing operand for %s operator.", il_operator);		// message (a)
	} else if ((symbol->candidate_datatypes.size() == 0) && (il_operand->candidate_datatypes.size() > 0)) {
		STAGE3_ERROR(0, symbol, symbol, "Data type mismatch for '%s' operator.", il_operator);		// message (b)
	} else if (NULL == symbol->datatype) {  // do NOT use !get_datatype_info_c::is_type_valid() here!
		STAGE3_WARNING(symbol, symbol, "Result of '%s' operation is never used.", il_operator);		// message (c)
	} else if (deprecated_operation)
		STAGE3_WARNING(symbol, symbol, "Deprecated operation for '%s' operator.", il_operator);		// message (d)
	return NULL;
}


void *print_datatypes_error_c::visit(  LD_operator_c *symbol) {return print_binary_operator_errors("LD"  , symbol);}  // I believe it will never emit messages (b) and (c)!!
void *print_datatypes_error_c::visit( LDN_operator_c *symbol) {return print_binary_operator_errors("LDN" , symbol);}  // I believe it will never emit message (c)
void *print_datatypes_error_c::visit(  ST_operator_c *symbol) {return print_binary_operator_errors("ST"  , symbol);}  // I believe it will never emit message (c)
void *print_datatypes_error_c::visit( STN_operator_c *symbol) {return print_binary_operator_errors("STN" , symbol);}  // I believe it will never emit message (c)

void *print_datatypes_error_c::visit(NOT_operator_c *symbol) {
	/* NOTE: the standard allows syntax in which the NOT operator is followed by an optional <il_operand>
	 *              NOT [<il_operand>]
	 *       However, it does not define the semantic of the NOT operation when the <il_operand> is specified.
	 *       We therefore consider it an error if an il_operand is specified!
	 */
	if (il_operand != NULL) {
		STAGE3_ERROR(0, symbol, symbol, "'NOT' operator may not have an operand.");
	} else if (symbol->candidate_datatypes.size() == 0)
		STAGE3_ERROR(0, symbol, symbol, "Data type mismatch for 'NOT' operator.");
	return NULL;
}

void *print_datatypes_error_c::visit(S_operator_c *symbol) {
	if (NULL != symbol->called_fb_declaration) /* FB call semantics */  return handle_implicit_il_fb_invocation("S", symbol, symbol->called_fb_declaration);
	else                                       /* Reset   semantics */  return print_binary_operator_errors    ("S", symbol);
}

void *print_datatypes_error_c::visit(R_operator_c *symbol) {
	if (NULL != symbol->called_fb_declaration) /* FB call semantics */  return handle_implicit_il_fb_invocation("R", symbol, symbol->called_fb_declaration);
	else                                       /* Reset   semantics */  return print_binary_operator_errors    ("R", symbol);
}

void *print_datatypes_error_c::visit( S1_operator_c *symbol) {return handle_implicit_il_fb_invocation( "S1", symbol, symbol->called_fb_declaration);}
void *print_datatypes_error_c::visit( R1_operator_c *symbol) {return handle_implicit_il_fb_invocation( "R1", symbol, symbol->called_fb_declaration);}
void *print_datatypes_error_c::visit(CLK_operator_c *symbol) {return handle_implicit_il_fb_invocation("CLK", symbol, symbol->called_fb_declaration);}
void *print_datatypes_error_c::visit( CU_operator_c *symbol) {return handle_implicit_il_fb_invocation( "CU", symbol, symbol->called_fb_declaration);}
void *print_datatypes_error_c::visit( CD_operator_c *symbol) {return handle_implicit_il_fb_invocation( "CD", symbol, symbol->called_fb_declaration);}
void *print_datatypes_error_c::visit( PV_operator_c *symbol) {return handle_implicit_il_fb_invocation( "PV", symbol, symbol->called_fb_declaration);}
void *print_datatypes_error_c::visit( IN_operator_c *symbol) {return handle_implicit_il_fb_invocation( "IN", symbol, symbol->called_fb_declaration);}
void *print_datatypes_error_c::visit( PT_operator_c *symbol) {return handle_implicit_il_fb_invocation( "PT", symbol, symbol->called_fb_declaration);}

void *print_datatypes_error_c::visit( AND_operator_c *symbol) {return print_binary_operator_errors("AND" , symbol);}
void *print_datatypes_error_c::visit(  OR_operator_c *symbol) {return print_binary_operator_errors( "OR" , symbol);}
void *print_datatypes_error_c::visit( XOR_operator_c *symbol) {return print_binary_operator_errors("XOR" , symbol);}
void *print_datatypes_error_c::visit(ANDN_operator_c *symbol) {return print_binary_operator_errors("ANDN", symbol);}
void *print_datatypes_error_c::visit( ORN_operator_c *symbol) {return print_binary_operator_errors( "ORN", symbol);}
void *print_datatypes_error_c::visit(XORN_operator_c *symbol) {return print_binary_operator_errors("XORN", symbol);}
void *print_datatypes_error_c::visit( ADD_operator_c *symbol) {return print_binary_operator_errors("ADD" , symbol, symbol->deprecated_operation);}
void *print_datatypes_error_c::visit( SUB_operator_c *symbol) {return print_binary_operator_errors("SUB" , symbol, symbol->deprecated_operation);}
void *print_datatypes_error_c::visit( MUL_operator_c *symbol) {return print_binary_operator_errors("MUL" , symbol, symbol->deprecated_operation);}
void *print_datatypes_error_c::visit( DIV_operator_c *symbol) {return print_binary_operator_errors("DIV" , symbol, symbol->deprecated_operation);}
void *print_datatypes_error_c::visit( MOD_operator_c *symbol) {return print_binary_operator_errors("MOD" , symbol);}

void *print_datatypes_error_c::visit(  GT_operator_c *symbol) {return print_binary_operator_errors( "GT" , symbol);}
void *print_datatypes_error_c::visit(  GE_operator_c *symbol) {return print_binary_operator_errors( "GE" , symbol);}
void *print_datatypes_error_c::visit(  EQ_operator_c *symbol) {return print_binary_operator_errors( "EQ" , symbol);}
void *print_datatypes_error_c::visit(  LT_operator_c *symbol) {return print_binary_operator_errors( "LT" , symbol);}
void *print_datatypes_error_c::visit(  LE_operator_c *symbol) {return print_binary_operator_errors( "LE" , symbol);}
void *print_datatypes_error_c::visit(  NE_operator_c *symbol) {return print_binary_operator_errors( "NE" , symbol);}

  


void *print_datatypes_error_c::handle_conditional_flow_control_IL_instruction(symbol_c *symbol, const char *oper) {
	if (!get_datatype_info_c::is_type_valid(symbol->datatype))
		STAGE3_ERROR(0, symbol, symbol, "%s operator must be preceded by an IL instruction producing a BOOL value.", oper);
	return NULL;
}

void *print_datatypes_error_c::visit(  CAL_operator_c *symbol) {return NULL;}
void *print_datatypes_error_c::visit( CALC_operator_c *symbol) {return handle_conditional_flow_control_IL_instruction(symbol, "CALC" );}
void *print_datatypes_error_c::visit(CALCN_operator_c *symbol) {return handle_conditional_flow_control_IL_instruction(symbol, "CALCN");}
void *print_datatypes_error_c::visit(  RET_operator_c *symbol) {return NULL;}
void *print_datatypes_error_c::visit( RETC_operator_c *symbol) {return handle_conditional_flow_control_IL_instruction(symbol, "RETC" );}
void *print_datatypes_error_c::visit(RETCN_operator_c *symbol) {return handle_conditional_flow_control_IL_instruction(symbol, "RETCN");}
void *print_datatypes_error_c::visit(  JMP_operator_c *symbol) {return NULL;}
void *print_datatypes_error_c::visit( JMPC_operator_c *symbol) {return handle_conditional_flow_control_IL_instruction(symbol, "JMPC" );}
void *print_datatypes_error_c::visit(JMPCN_operator_c *symbol) {return handle_conditional_flow_control_IL_instruction(symbol, "JMPCN");}



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
/* SYM_REF1(deref_operator_c, exp)  --> an extension to the IEC 61131-3 standard - based on the IEC 61131-3 v3 standard. Returns address of the varible! */
void *print_datatypes_error_c::visit(deref_operator_c  *symbol) {
	symbol->exp->accept(*this);
	/* we should really check whether the expression is merely a variable. For now, leave it for the future! */
	if ((symbol->candidate_datatypes.size() == 0) && (symbol->exp->candidate_datatypes.size() > 0))
		STAGE3_ERROR(0, symbol, symbol, "^ operator must be preceded by a value of type REF_TO.");
	return NULL;
}

/* SYM_REF1(deref_expression_c, exp)  --> an extension to the IEC 61131-3 standard - based on the IEC 61131-3 v3 standard. Returns address of the varible! */
void *print_datatypes_error_c::visit(deref_expression_c  *symbol) {
	symbol->exp->accept(*this);
	/* we should really check whether the expression is merely a variable. For now, leave it for the future! */
	if ((symbol->candidate_datatypes.size() == 0) && (symbol->exp->candidate_datatypes.size() > 0))
		STAGE3_ERROR(0, symbol, symbol, "DREF operator must be used with a value of type REF_TO.");
	return NULL;
}

/* SYM_REF1(ref_expression_c, exp)  --> an extension to the IEC 61131-3 standard - based on the IEC 61131-3 v3 standard. Returns address of the varible! */
void *print_datatypes_error_c::visit(  ref_expression_c  *symbol) {
	symbol->exp->accept(*this);
	/* we should really check whether the expression is merely a variable. For now, leave it for the future! */
	if ((symbol->candidate_datatypes.size() == 0) && (symbol->exp->candidate_datatypes.size() > 0))
		STAGE3_ERROR(0, symbol, symbol, "REF operator must be used with a variable.");
	return NULL;
}
    

void *print_datatypes_error_c::print_binary_expression_errors(const char *operation, symbol_c *symbol, symbol_c *l_expr, symbol_c *r_expr, bool deprecated_operation) {
	l_expr->accept(*this);
	r_expr->accept(*this);
	if ((symbol->candidate_datatypes.size() == 0) 		&&
		(l_expr->candidate_datatypes.size() > 0)	&&
		(r_expr->candidate_datatypes.size() > 0))
		STAGE3_ERROR(0, symbol, symbol, "Data type mismatch for '%s' expression.", operation);
        if (deprecated_operation)
                STAGE3_WARNING(symbol, symbol, "Deprecated operation for '%s' expression.", operation);
	return NULL;
}


void *print_datatypes_error_c::visit(    or_expression_c *symbol) {return print_binary_expression_errors( "OR", symbol, symbol->l_exp, symbol->r_exp);}
void *print_datatypes_error_c::visit(   xor_expression_c *symbol) {return print_binary_expression_errors("XOR", symbol, symbol->l_exp, symbol->r_exp);}
void *print_datatypes_error_c::visit(   and_expression_c *symbol) {return print_binary_expression_errors("AND", symbol, symbol->l_exp, symbol->r_exp);}
void *print_datatypes_error_c::visit(   equ_expression_c *symbol) {return print_binary_expression_errors( "=" , symbol, symbol->l_exp, symbol->r_exp);}
void *print_datatypes_error_c::visit(notequ_expression_c *symbol) {return print_binary_expression_errors( "<>", symbol, symbol->l_exp, symbol->r_exp);}
void *print_datatypes_error_c::visit(    lt_expression_c *symbol) {return print_binary_expression_errors( "<" , symbol, symbol->l_exp, symbol->r_exp);}
void *print_datatypes_error_c::visit(    gt_expression_c *symbol) {return print_binary_expression_errors( ">" , symbol, symbol->l_exp, symbol->r_exp);}
void *print_datatypes_error_c::visit(    le_expression_c *symbol) {return print_binary_expression_errors( "<=", symbol, symbol->l_exp, symbol->r_exp);}
void *print_datatypes_error_c::visit(    ge_expression_c *symbol) {return print_binary_expression_errors( ">=", symbol, symbol->l_exp, symbol->r_exp);}
void *print_datatypes_error_c::visit(   add_expression_c *symbol) {return print_binary_expression_errors( "+" , symbol, symbol->l_exp, symbol->r_exp, symbol->deprecated_operation);}
void *print_datatypes_error_c::visit(   sub_expression_c *symbol) {return print_binary_expression_errors( "-" , symbol, symbol->l_exp, symbol->r_exp, symbol->deprecated_operation);}
void *print_datatypes_error_c::visit(   mul_expression_c *symbol) {return print_binary_expression_errors( "*" , symbol, symbol->l_exp, symbol->r_exp, symbol->deprecated_operation);}
void *print_datatypes_error_c::visit(   div_expression_c *symbol) {return print_binary_expression_errors( "/" , symbol, symbol->l_exp, symbol->r_exp, symbol->deprecated_operation);}
void *print_datatypes_error_c::visit(   mod_expression_c *symbol) {return print_binary_expression_errors("MOD", symbol, symbol->l_exp, symbol->r_exp);}
void *print_datatypes_error_c::visit( power_expression_c *symbol) {return print_binary_expression_errors( "**", symbol, symbol->l_exp, symbol->r_exp);}


void *print_datatypes_error_c::visit(neg_expression_c *symbol) {
	symbol->exp->accept(*this);
	if ((symbol->candidate_datatypes.size() == 0)      &&
		(symbol->exp->candidate_datatypes.size() > 0))
		STAGE3_ERROR(0, symbol, symbol, "Invalid data type for 'NEG' expression.");
	return NULL;
}


void *print_datatypes_error_c::visit(not_expression_c *symbol) {
	symbol->exp->accept(*this);
	if ((symbol->candidate_datatypes.size() == 0)      &&
		(symbol->exp->candidate_datatypes.size() > 0))
		STAGE3_ERROR(0, symbol, symbol, "Invalid data type for 'NOT' expression.");
	return NULL;
}

/* NOTE: The parameter 'called_function_declaration', 'extensible_param_count' and 'candidate_functions' are used to pass data between the stage 3 and stage 4. */
/*    formal_param_list -> may be NULL ! */
/* nonformal_param_list -> may be NULL ! */
// SYM_REF3(function_invocation_c, function_name, formal_param_list, nonformal_param_list, symbol_c *called_function_declaration; int extensible_param_count; std::vector <symbol_c *> candidate_functions;)
void *print_datatypes_error_c::visit(function_invocation_c *symbol) {
	generic_function_call_t fcall_param = {
		/* fcall_param.function_name               = */ symbol->function_name,
		/* fcall_param.nonformal_operand_list      = */ symbol->nonformal_param_list,
		/* fcall_param.formal_operand_list         = */ symbol->formal_param_list,
		/* enum {POU_FB, POU_function} POU_type    = */ generic_function_call_t::POU_function,
		/* fcall_param.candidate_functions         = */ symbol->candidate_functions,
		/* fcall_param.called_function_declaration = */ symbol->called_function_declaration,
		/* fcall_param.extensible_param_count      = */ symbol->extensible_param_count
	};

	handle_function_invocation(symbol, fcall_param);
	return NULL;
}



/********************/
/* B 3.2 Statements */
/********************/

/*********************************/
/* B 3.2.1 Assignment Statements */
/*********************************/
void *print_datatypes_error_c::visit(assignment_statement_c *symbol) {
	symbol->l_exp->accept(*this);
	symbol->r_exp->accept(*this);
	if ((!get_datatype_info_c::is_type_valid(symbol->l_exp->datatype)) &&
	    (!get_datatype_info_c::is_type_valid(symbol->r_exp->datatype)) &&
	    (symbol->l_exp->candidate_datatypes.size() > 0)	&&
	    (symbol->r_exp->candidate_datatypes.size() > 0))
		STAGE3_ERROR(0, symbol, symbol, "Incompatible data types for ':=' operation.");
	return NULL;
}


/*****************************************/
/* B 3.2.2 Subprogram Control Statements */
/*****************************************/
/* fb_name '(' [param_assignment_list] ')' */
/*    formal_param_list -> may be NULL ! */
/* nonformal_param_list -> may be NULL ! */
/* NOTE: The parameter 'called_fb_declaration'is used to pass data between stage 3 and stage4 (although currently it is not used in stage 4 */
// SYM_REF3(fb_invocation_c, fb_name, formal_param_list, nonformal_param_list, symbol_c *called_fb_declaration;)
void *print_datatypes_error_c::visit(fb_invocation_c *symbol) {
	int extensible_param_count;                      /* unused vairable! Needed for compilation only! */
	std::vector <symbol_c *> candidate_functions;    /* unused vairable! Needed for compilation only! */
	generic_function_call_t fcall_param = {
		/* fcall_param.function_name               = */ symbol->fb_name,
		/* fcall_param.nonformal_operand_list      = */ symbol->nonformal_param_list,
		/* fcall_param.formal_operand_list         = */ symbol->formal_param_list,
		/* enum {POU_FB, POU_function} POU_type    = */ generic_function_call_t::POU_FB,
		/* fcall_param.candidate_functions         = */ candidate_functions,             /* will not be used, but must provide a reference to be able to compile */
		/* fcall_param.called_function_declaration = */ symbol->called_fb_declaration,
		/* fcall_param.extensible_param_count      = */ extensible_param_count           /* will not be used, but must provide a reference to be able to compile */
	};
	
	handle_function_invocation(symbol, fcall_param);
	return NULL;
}


/********************************/
/* B 3.2.3 Selection Statements */
/********************************/

void *print_datatypes_error_c::visit(if_statement_c *symbol) {
	symbol->expression->accept(*this);
	if ((!get_datatype_info_c::is_type_valid(symbol->expression->datatype)) &&
	    (symbol->expression->candidate_datatypes.size() > 0)) {
		STAGE3_ERROR(0, symbol->expression, symbol->expression, "Invalid data type for 'IF' condition (should be BOOL).");
	}
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	if (NULL != symbol->elseif_statement_list)
		symbol->elseif_statement_list->accept(*this);
	if (NULL != symbol->else_statement_list)
		symbol->else_statement_list->accept(*this);
	return NULL;
}

void *print_datatypes_error_c::visit(elseif_statement_c *symbol) {
	symbol->expression->accept(*this);
	if ((!get_datatype_info_c::is_type_valid(symbol->expression->datatype)) &&
	    (symbol->expression->candidate_datatypes.size() > 0)) {
		STAGE3_ERROR(0, symbol->expression, symbol->expression, "Invalid data type for 'ELSIF' condition (should be BOOL).");
	}
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	return NULL;
}


void *print_datatypes_error_c::visit(case_statement_c *symbol) {
	symbol->expression->accept(*this);
	if ((!get_datatype_info_c::is_type_valid(symbol->expression->datatype)) &&
	    (symbol->expression->candidate_datatypes.size() > 0)) {
		STAGE3_ERROR(0, symbol->expression, symbol->expression, "'CASE' quantity not an integer or enumerated.");
	}
	symbol->case_element_list->accept(*this);
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	return NULL;
}

/********************************/
/* B 3.2.4 Iteration Statements */
/********************************/

void *print_datatypes_error_c::visit(for_statement_c *symbol) {
	symbol->control_variable->accept(*this);
	symbol->beg_expression->accept(*this);
	symbol->end_expression->accept(*this);
	/* Control variable */
	if ((!get_datatype_info_c::is_type_valid(symbol->control_variable->datatype)) &&
	    (symbol->control_variable->candidate_datatypes.size() > 0)) {
		STAGE3_ERROR(0, symbol->control_variable, symbol->control_variable, "Invalid data type for 'FOR' control variable.");
	}
	/* BEG expression */
	if ((!get_datatype_info_c::is_type_valid(symbol->beg_expression->datatype)) &&
	    (symbol->beg_expression->candidate_datatypes.size() > 0)) {
		STAGE3_ERROR(0, symbol->beg_expression, symbol->beg_expression, "Invalid data type for 'FOR' begin expression.");
	}
	/* END expression */
	if ((!get_datatype_info_c::is_type_valid(symbol->end_expression->datatype)) &&
	    (symbol->end_expression->candidate_datatypes.size() > 0)) {
		STAGE3_ERROR(0, symbol->end_expression, symbol->end_expression, "Invalid data type for 'FOR' end expression.");
	}
	/* BY expression */
	if ((NULL != symbol->by_expression) &&
	    (!get_datatype_info_c::is_type_valid(symbol->by_expression->datatype)) &&
	    (symbol->end_expression->candidate_datatypes.size() > 0)) {
		STAGE3_ERROR(0, symbol->by_expression, symbol->by_expression, "Invalid data type for 'FOR' by expression.");
	}
	/* DO statement */
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);

	return NULL;
}

void *print_datatypes_error_c::visit(while_statement_c *symbol) {
	symbol->expression->accept(*this);
	if (!get_datatype_info_c::is_type_valid(symbol->expression->datatype)) {
		STAGE3_ERROR(0, symbol->expression, symbol->expression, "Invalid data type for 'WHILE' condition.");
		return NULL;
	}
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	return NULL;
}

void *print_datatypes_error_c::visit(repeat_statement_c *symbol) {
	if (!get_datatype_info_c::is_type_valid(symbol->expression->datatype)) {
		STAGE3_ERROR(0, symbol->expression, symbol->expression, "Invalid data type for 'REPEAT' condition.");
		return NULL;
	}
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	symbol->expression->accept(*this);
	return NULL;
}





