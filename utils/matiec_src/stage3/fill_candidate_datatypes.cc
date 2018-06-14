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


/* TODO - things yet not checked by this data type checker...
 *
 * - check variable declarations
 * - check data type declarations
 * - check inside configurations (variable declarations)
 * - check SFC code
 * - must fix S and R IL functions (includes potientialy fixing stage4 code!) 
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

#include <../main.hh>         /* required for UINT64_MAX, INT64_MAX, INT64_MIN, ... */
#include "fill_candidate_datatypes.hh"
#include "datatype_functions.hh"
#include <typeinfo>
#include <list>
#include <string>
#include <string.h>
#include <strings.h>


#define GET_CVALUE(dtype, symbol)             ((symbol)->const_value._##dtype.get())
#define VALID_CVALUE(dtype, symbol)           ((symbol)->const_value._##dtype.is_valid())
#define IS_OVERFLOW(dtype, symbol)            ((symbol)->const_value._##dtype.is_overflow())


/* set to 1 to see debug info during execution */
static int debug = 0;



/*****************************************************/
/*                                                   */
/*  A small helper class...                          */
/*                                                   */
/*****************************************************/

/* Add to the global_enumerated_value_symtable the global enum value constants, i.e. the enum constants used in the enumerated
 * datatypes that are defined inside a TYPE ... END_TYPE declaration.
 */
/* NOTE: we do not store any NULL values in this symbol table, so we can safely use NULL and the null value. */

typedef dsymtable_c<symbol_c *> enumerated_value_symtable_t;
static enumerated_value_symtable_t global_enumerated_value_symtable;
 
 
class populate_globalenumvalue_symtable_c: public iterator_visitor_c {
  private:
    symbol_c *current_enumerated_type;

  public:
     populate_globalenumvalue_symtable_c(void) {current_enumerated_type = NULL;};
    ~populate_globalenumvalue_symtable_c(void) {}

  public:
  /*************************/
  /* B.1 - Common elements */
  /*************************/
  /**********************/
  /* B.1.3 - Data types */
  /**********************/
  /********************************/
  /* B 1.3.3 - Derived data types */
  /********************************/
  /*  enumerated_type_name ':' enumerated_spec_init */
  void *visit(enumerated_type_declaration_c *symbol) {
  //current_enumerated_type = symbol->enumerated_type_name;
    current_enumerated_type = symbol;
    symbol->enumerated_spec_init->accept(*this);
    current_enumerated_type = NULL;
    return NULL;
  }

  /* enumerated_specification ASSIGN enumerated_value */
  void *visit(enumerated_spec_init_c *symbol) {
    return symbol->enumerated_specification->accept(*this);
  }

  /* [enumerated_type_name '#'] identifier */
  void *visit(enumerated_value_c *symbol) {
    if (current_enumerated_type == NULL) ERROR;
    if (symbol->type != NULL) ERROR;

    enumerated_value_symtable_t::iterator lower = global_enumerated_value_symtable.lower_bound(symbol->value);
    enumerated_value_symtable_t::iterator upper = global_enumerated_value_symtable.upper_bound(symbol->value);
    for (; lower != upper; lower++)
      if (lower->second == current_enumerated_type) {
        /*  The same identifier is used more than once as an enumerated value/constant inside the same enumerated datat type! */
        return NULL; /* No need to insert it! It is already in the table! */
      }

    global_enumerated_value_symtable.insert(symbol->value, current_enumerated_type);
    return NULL;
  }

  /**************************************/
  /* B.1.5 - Program organization units */
  /**************************************/
  /* B 1.5.1 - Functions */
  void *visit(function_declaration_c *symbol) {return NULL;}
  /* B 1.5.2 - Function Blocks */
  void *visit(function_block_declaration_c *symbol) {return NULL;}
  /* B 1.5.3 - Programs */
  void *visit(program_declaration_c *symbol) {return NULL;}
  
}; /* populate_globalenumvalue_symtable_c */

static populate_globalenumvalue_symtable_c populate_globalenumvalue_symtable;


/*****************************************************/
/*                                                   */
/*  A small helper class...                          */
/*                                                   */
/*****************************************************/

/* Add to the local_enumerated_value_symtable the local enum value constants */
/* Notes:
 * Some enumerations are 
 *   (A) declared anonymously inside a VAR ... END_VAR declaration
 *       (e.g. VAR enum_var : (enumvalue1, enumvalue2); END_VAR)
 *  while others are 
 *   (B) declared (with a name) inside a TYPE .. END_TYPE declaration.
 *
 *  Values in (A) are added to the enumerated_value_symtable in absyntaxt_utils.cc.
 *  Values in (B) are only in scope inside the POU with the VAR END_VAR declaration.
 *
 * This class will add the enum values in (B) to the local_enumerated_value_symtable.
 *
 * If a locally defined enum value is identical to another locally defined enum_value, a 
 *  duplicate entry is created.
 *  However, if a locally defined enum value is identical to another globally defined enum_value, the
 *  corresponding entry in local_enumerated_value_symtable is also set to the local datatype.
 *  This is because anonynous locally feined enum datatypes are anonymous, and its enum values cannot therefore
 *  be disambiguated using EnumType#enum_value (since the enum type does not have a name, it is anonymous!).
 *  For this reason we implement the semantics where locally defined enum values, when in scope, will 'cover'
 *  the globally defined enum value with the same name/identifier.
 *  For example:
 *
 *  TYPE  GlobalEnumT: (xxx1, xxx2, xxx3) END_TYPE
 * 
 *   FUNCTION_BLOCK FOO
 *    VAR_INPUT
 *       GlobalEnumVar: GlobalEnumT;
 *      LocalEnumVar : (xxx1, yyy2, yyy3);
 *     END_VAR
 *     LocalEnumVar  := xxx1;   <-- We consider it OK!!!     xxx1 will reference the anonymous type used for LocalEnumVar
 *     GlobalEnumVar := xxx1;   <-- We consider it an error. xxx1 will reference the anonymous type used for LocalEnumVar
 *     GlobalEnumVar := GlobalEnumT#xxx1;
 *     END_FUNCTION_BLOCK
 */
 
static enumerated_value_symtable_t local_enumerated_value_symtable;


class populate_localenumvalue_symtable_c: public iterator_visitor_c {
  private:
    symbol_c *current_enumerated_type;

  public:
     populate_localenumvalue_symtable_c(void) {current_enumerated_type = NULL;};
    ~populate_localenumvalue_symtable_c(void) {}

  public:
  /*************************/
  /* B.1 - Common elements */
  /*************************/
  /**********************/
  /* B.1.3 - Data types */
  /**********************/
  /********************************/
  /* B 1.3.3 - Derived data types */
  /********************************/
  /*  TYPE type_declaration_list END_TYPE */
  void *visit(data_type_declaration_c *symbol) {return NULL;} // do not visit the type declarations!!
  
  /* enumerated_specification ASSIGN enumerated_value */
  void *visit(enumerated_spec_init_c *symbol) {
    current_enumerated_type = symbol;
    symbol->enumerated_specification->accept(*this);
    /* DO NOT visit the symbol->enumerated_value   !!! */
    current_enumerated_type = NULL;
    return NULL;
  }

  /* [enumerated_type_name '#'] identifier */
  void *visit(enumerated_value_c *symbol) {
    /* if the enumerated_value_c is not inside a enumerated_spec_init_c (e.g. used as the inital value of a variable), we simply return */
    if (current_enumerated_type == NULL) return NULL;  
    /* this is really an ERROR! The initial value may use the syntax NUM_TYPE#enum_value, but in that case we should have return'd in the above statement !! */
    if (symbol->type != NULL) ERROR;  

    enumerated_value_symtable_t::iterator lower = local_enumerated_value_symtable.lower_bound(symbol->value);
    enumerated_value_symtable_t::iterator upper = local_enumerated_value_symtable.upper_bound(symbol->value);
    for (; lower != upper; lower++)
      if (lower->second == current_enumerated_type) {
        /*  The same identifier is used more than once as an enumerated value/constant inside the same enumerated datat type! */
        return NULL; /* No need to insert it! It is already in the table! */
      }
    
    /* add it to the local symbol table. */
    local_enumerated_value_symtable.insert(symbol->value, current_enumerated_type);
    return NULL;
  }
}; // class populate_enumvalue_symtable_c

static populate_localenumvalue_symtable_c populate_enumvalue_symtable;




/*****************************************************/
/*                                                   */
/*  Main  FILL candidate datatypes algorithm...      */
/*                                                   */
/*****************************************************/


fill_candidate_datatypes_c::fill_candidate_datatypes_c(symbol_c *ignore) {
	il_operand = NULL;
	prev_il_instruction = NULL;
	search_var_instance_decl = NULL;
	current_enumerated_spec_type = NULL;
	current_scope = NULL;
}

fill_candidate_datatypes_c::~fill_candidate_datatypes_c(void) {
}







symbol_c *fill_candidate_datatypes_c::widening_conversion(symbol_c *left_type, symbol_c *right_type, const struct widen_entry widen_table[]) {
	int k;
	/* find a widening table entry compatible */
	for (k = 0; NULL != widen_table[k].left;  k++)
		if ((typeid(*left_type) == typeid(*widen_table[k].left)) && (typeid(*right_type) == typeid(*widen_table[k].right)))
                      return widen_table[k].result;
	return NULL;
}


/* add a data type to a candidate data type list, while guaranteeing no duplicate entries! */
/* Returns true if it really did add the datatype to the list, or false if it was already present in the list! */
bool fill_candidate_datatypes_c::add_datatype_to_candidate_list(symbol_c *symbol, symbol_c *datatype) {
  /* If it is an invalid data type, do not insert!
   * NOTE: it reduces overall code size to do this test here, instead of doing every time before calling the add_datatype_to_candidate_list() function. 
   */
  if (!get_datatype_info_c::is_type_valid(datatype)) /* checks for NULL and invalid_type_name_c */
    return false;

  if (search_in_candidate_datatype_list(datatype, symbol->candidate_datatypes) >= 0) 
    /* already in the list, Just return! */
    return false;
  
  /* not yet in the candidate data type list, so we insert it now! */
  symbol->candidate_datatypes.push_back(datatype);
  return true;
}
    
    
bool fill_candidate_datatypes_c::add_2datatypes_to_candidate_list(symbol_c *symbol, symbol_c *datatype1, symbol_c *datatype2) {
  add_datatype_to_candidate_list(symbol, datatype1);
  add_datatype_to_candidate_list(symbol, datatype2);
  return true;
}



void fill_candidate_datatypes_c::remove_incompatible_datatypes(symbol_c *symbol) {
  #ifdef __REMOVE__
    #error __REMOVE__ macro already exists. Choose another name!
  #endif
  #define __REMOVE__(datatype)\
      remove_from_candidate_datatype_list(&get_datatype_info_c::datatype,       symbol->candidate_datatypes);\
      remove_from_candidate_datatype_list(&get_datatype_info_c::safe##datatype, symbol->candidate_datatypes);
  
  {/* Remove unsigned data types */
    uint64_t value = 0;
    if (VALID_CVALUE( uint64, symbol)) value = GET_CVALUE(uint64, symbol);
    if (IS_OVERFLOW ( uint64, symbol)) value = (uint64_t)UINT32_MAX + (uint64_t)1;
    
    if (value > 1          )          {__REMOVE__(bool_type_name);}
    if (value > UINT8_MAX  )          {__REMOVE__(usint_type_name);  __REMOVE__( byte_type_name);}
    if (value > UINT16_MAX )          {__REMOVE__( uint_type_name);  __REMOVE__( word_type_name);}
    if (value > UINT32_MAX )          {__REMOVE__(udint_type_name);  __REMOVE__(dword_type_name);}
    if (IS_OVERFLOW( uint64, symbol)) {__REMOVE__(ulint_type_name);  __REMOVE__(lword_type_name);}
  }

  {/* Remove signed data types */
    int64_t value = 0;
    if (VALID_CVALUE(  int64, symbol)) value = GET_CVALUE(int64, symbol);
    if (IS_OVERFLOW (  int64, symbol)) value = (int64_t)INT32_MAX + (int64_t)1;
    
    if ((value <  INT8_MIN) || (value >  INT8_MAX)) {__REMOVE__(sint_type_name);}
    if ((value < INT16_MIN) || (value > INT16_MAX)) {__REMOVE__( int_type_name);}
    if ((value < INT32_MIN) || (value > INT32_MAX)) {__REMOVE__(dint_type_name);}
    if (IS_OVERFLOW( int64, symbol))                {__REMOVE__(lint_type_name);}
  }
    
  {/* Remove floating point data types */
    real64_t value = 0;
    if (VALID_CVALUE( real64, symbol)) value = GET_CVALUE(real64, symbol);
    if (IS_OVERFLOW ( real64, symbol)) value = (real64_t)REAL32_MAX + (real64_t)1;
    if (value >  REAL32_MAX )         {__REMOVE__( real_type_name);}
    if (value < -REAL32_MAX )         {__REMOVE__( real_type_name);}
    if (IS_OVERFLOW( real64, symbol)) {__REMOVE__(lreal_type_name);}
  }
  #undef __REMOVE__
}
    

/* returns true if compatible function/FB invocation, otherwise returns false */
/* Assumes that the candidate_datatype lists of all the parameters being passed haved already been filled in */
/*
 * All parameters being passed to the called function MUST be in the parameter list to which f_call points to!
 * This means that, for non formal function calls in IL, de current (default value) must be artificially added to the
 * beginning of the parameter list BEFORE calling handle_function_call().
 */
bool fill_candidate_datatypes_c::match_nonformal_call(symbol_c *f_call, symbol_c *f_decl) {
	symbol_c *call_param_value,  *param_datatype;
	identifier_c *param_name;
	function_param_iterator_c       fp_iterator(f_decl);
	function_call_param_iterator_c fcp_iterator(f_call);
	int extensible_parameter_highest_index = -1;
	unsigned int i;

	/* Iterating through the non-formal parameters of the function call */
	while((call_param_value = fcp_iterator.next_nf()) != NULL) {
		/* Iterate to the next parameter of the function being called.
		 * Get the name of that parameter, and ignore if EN or ENO.
		 */
		do {
			param_name = fp_iterator.next();
			/* If there is no other parameter declared, then we are passing too many parameters... */
			if(param_name == NULL) return false;
		} while ((strcmp(param_name->value, "EN") == 0) || (strcmp(param_name->value, "ENO") == 0));

		/* Get the parameter type */
		param_datatype = base_type(fp_iterator.param_type());
		
		/* check whether one of the candidate_data_types of the value being passed is the same as the param_type */
		if (search_in_candidate_datatype_list(param_datatype, call_param_value->candidate_datatypes) < 0)
			return false; /* return false if param_type not in the list! */
	}
	/* call is compatible! */
	return true;
}



/* returns true if compatible function/FB invocation, otherwise returns false */
/* Assumes that the candidate_datatype lists of all the parameters being passed haved already been filled in */
bool fill_candidate_datatypes_c::match_formal_call(symbol_c *f_call, symbol_c *f_decl, symbol_c **first_param_datatype) {
	symbol_c *call_param_value, *call_param_name, *param_datatype;
	symbol_c *verify_duplicate_param;
	identifier_c *param_name;
	function_param_iterator_c       fp_iterator(f_decl);
	function_call_param_iterator_c fcp_iterator(f_call);
	int extensible_parameter_highest_index = -1;
	identifier_c *extensible_parameter_name;
	unsigned int i;
	bool is_first_param = true;

	/* Iterating through the formal parameters of the function call */
	while((call_param_name = fcp_iterator.next_f()) != NULL) {
		/* Obtaining the value being passed in the function call */
		call_param_value = fcp_iterator.get_current_value();
		/* the following should never occur. If it does, then we have a bug in our code... */
		if (NULL == call_param_value) ERROR;

		/* Obtaining the assignment direction:  := (assign_in) or => (assign_out) */
		function_call_param_iterator_c::assign_direction_t call_param_dir = fcp_iterator.get_assign_direction();

		/* Checking if there are duplicated parameter values */
		verify_duplicate_param = fcp_iterator.search_f(call_param_name);
		if(verify_duplicate_param != call_param_value)
			return false;

		/* Obtaining the type of the value being passed in the function call */
		std::vector <symbol_c *>&call_param_types = call_param_value->candidate_datatypes;

		/* Find the corresponding parameter in function declaration */
		param_name = fp_iterator.search(call_param_name);
		if(param_name == NULL) return false;
		/* Get the parameter data type */
		param_datatype = base_type(fp_iterator.param_type());
		/* Get the parameter direction: IN, OUT, IN_OUT */
		function_param_iterator_c::param_direction_t param_dir = fp_iterator.param_direction();

		/* check whether direction (IN, OUT, IN_OUT) and assignment types (:= , =>) are compatible !!! */
		if          (function_call_param_iterator_c::assign_in  == call_param_dir) {
			if ((function_param_iterator_c::direction_in    != param_dir) &&
			    (function_param_iterator_c::direction_inout != param_dir))
				return false;
		} else if   (function_call_param_iterator_c::assign_out == call_param_dir) {
			if ((function_param_iterator_c::direction_out   != param_dir))
				return false;
		} else ERROR;
		
		/* check whether one of the candidate_data_types of the value being passed is the same as the param_type */
		if (search_in_candidate_datatype_list(param_datatype, call_param_types) < 0)
			return false; /* return false if param_type not in the list! */
		
		/* If this is the first parameter, then copy the datatype to *first_param_datatype */
		if (is_first_param)
			if (NULL != first_param_datatype)
				*first_param_datatype = param_datatype;
		is_first_param = false;
	}
	/* call is compatible! */
	return true;
}




/* Handle a generic function call!
 * Assumes that the parameter_list containing the values being passed in this function invocation
 * has already had all the candidate_datatype lists filled in!
 *
 * All parameters being passed to the called function MUST be in the parameter list to which f_call points to!
 * This means that, for non formal function calls in IL, de current (default value) must be artificially added to the
 * beginning of the parameter list BEFORE calling handle_function_call().
 */
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
/*
void narrow_candidate_datatypes_c::narrow_function_invocation(symbol_c *fcall, generic_function_call_t fcall_data) {
void *fill_candidate_datatypes_c::handle_function_call(symbol_c *f_call, symbol_c *function_name, invocation_type_t invocation_type,
                                                       std::vector <symbol_c *> *candidate_datatypes,
                                                       std::vector <symbol_c *> *candidate_functions) {
  */
void fill_candidate_datatypes_c::handle_function_call(symbol_c *fcall, generic_function_call_t fcall_data) {
	function_declaration_c *f_decl;
	list_c *parameter_list;
	list_c *parameter_candidate_datatypes;
	symbol_c *returned_parameter_type;

	if (debug) std::cout << "function()\n";

	function_symtable_t::iterator lower = function_symtable.lower_bound(fcall_data.function_name);
	function_symtable_t::iterator upper = function_symtable.upper_bound(fcall_data.function_name);
	/* If the name of the function being called is not found in the function symbol table, then this is an invalid call */
	/* Since the lexical parser already checks for this, then if this occurs then we have an internal compiler error. */
	if (lower == function_symtable.end()) ERROR;
	
	/* Look for all compatible function declarations, and add their return datatypes 
	 * to the candidate_datatype list of this function invocation. 
	 *
	 * If only one function exists, we add its return datatype to the candidate_datatype list,
	 * even if the parameters passed to it are invalid.
	 * This guarantees that the remainder of the expression in which the function call is inserted
	 * is treated as if the function call returns correctly, and therefore does not generate
	 * spurious error messages.
	 * Even if the parameters to the function call are invalid, doing this is still safe, as the 
	 * expressions inside the function call will themselves have erros and will  guarantee that 
	 * compilation is aborted in stage3 (in print_datatypes_error_c).
	 */
	if (function_symtable.count(fcall_data.function_name) == 1) {
		f_decl = function_symtable.get_value(lower);
		returned_parameter_type = base_type(f_decl->type_name);
		if (add_datatype_to_candidate_list(fcall, returned_parameter_type))
			/* we only add it to the function declaration list if this entry was not already present in the candidate datatype list! */
			fcall_data.candidate_functions.push_back(f_decl);
		
	}
	for(; lower != upper; lower++) {
		bool compatible = false;
		
		f_decl = function_symtable.get_value(lower);
		/* Check if function declaration in symbol_table is compatible with parameters */
		if (NULL != fcall_data.nonformal_operand_list) compatible=match_nonformal_call(fcall, f_decl);
		if (NULL != fcall_data.   formal_operand_list) compatible=   match_formal_call(fcall, f_decl);
		if (compatible) {
			/* Add the data type returned by the called functions. 
			 * However, only do this if this data type is not already present in the candidate_datatypes list_c
			 */
			returned_parameter_type = base_type(f_decl->type_name);		
			if (add_datatype_to_candidate_list(fcall, returned_parameter_type))
				/* we only add it to the function declaration list if this entry was not already present in the candidate datatype list! */
				fcall_data.candidate_functions.push_back(f_decl);
		}
	}
	if (debug) std::cout << "end_function() [" << fcall->candidate_datatypes.size() << "] result.\n";
	return;
}


/* handle implicit FB call in IL.
 * e.g.  CLK ton_var
 *        CU counter_var
 */
void *fill_candidate_datatypes_c::handle_implicit_il_fb_call(symbol_c *il_instruction, const char *param_name, symbol_c *&called_fb_declaration) {
	symbol_c *fb_decl = (NULL == il_operand)? NULL : search_var_instance_decl->get_basetype_decl(il_operand);
	if (! get_datatype_info_c::is_function_block(fb_decl)) fb_decl = NULL;

	/* Although a call to a non-declared FB is a semantic error, this is currently caught by stage 2! */
	/* However, when calling using the 'S' and 'R' operators, this error is not caught by stage 2, as these operators have two possible semantics */
	// if (NULL == fb_type_id) ERROR;

	/* The narrow_candidate_datatypes_c does not rely on this called_fb_declaration pointer being == NULL to conclude that
	 * we have a datatype incompatibility error, so we set it to fb_decl to allow the print_datatype_error_c to print out
	 * more informative error messages!
	 */
	called_fb_declaration = fb_decl;

	/* This implicit FB call does not change the value stored in the current/default IL variable */
	/* It does, however, require that the datatype be compatible with the input parameter of the FB being called. 
	 * If we were to follow the filling & narrowing algorithm correctly (implemented in fill_candidate_datatypes_c
	 * & narrow_candidate_datatypes_c respectively), we should be restricting the candidate_datatpes to the datatypes
	 * that are compatible to the FB call. 
	 * However, doing the above will often result in some very confusing error messages for the user, especially in the case
	 * in which the FB call is wrong, so the resulting cadidate datatypes is an empty list. In this case, the user would see
	 * many error messages related to the IL instructions that follow the FB call, even though those IL instructions may be perfectly
	 * correct.
	 * For now, we will simply let the narrow_candidate_datatypes_c verify if the datatypes are compatible (something that should be done
	 * here).
	 */
	if (NULL != prev_il_instruction)
		il_instruction->candidate_datatypes = prev_il_instruction->candidate_datatypes; 

	if (debug) std::cout << "handle_implicit_il_fb_call() [" << prev_il_instruction->candidate_datatypes.size() << "] ==> " << il_instruction->candidate_datatypes.size() << " result.\n";
	return NULL;
}





/* handle the S and R IL operators... */
/* operator_str should be set to either "S" or "R" */
void *fill_candidate_datatypes_c::handle_S_and_R_operator(symbol_c *symbol, const char *operator_str, symbol_c *&called_fb_declaration) {
	/* NOTE: this operator has two possible semantic meanings:
	 *          - Set/Reset the BOOL operand variable to true
	 *          - call the FB specified by the operand.
	 *       Which of the two semantics will have to be determined by the datatype of the operand!
	 */
	symbol_c *prev_instruction_type, *operand_type;

	if (NULL == prev_il_instruction) return NULL;
	if (NULL == il_operand)          return NULL;

	/* Try the Set/Reset semantics */
	for (unsigned int i = 0; i < prev_il_instruction->candidate_datatypes.size(); i++) {
		for(unsigned int j = 0; j < il_operand->candidate_datatypes.size(); j++) {
			prev_instruction_type = prev_il_instruction->candidate_datatypes[i];
			operand_type = il_operand->candidate_datatypes[j];
			/* IEC61131-3, Table 52, Note (e) states that the datatype of the operand must be BOOL!
			 * IEC61131-3, Table 52, line 3 states that this operator should "Set operand to 1 if current result is Boolean 1"
			 *     which implies that the prev_instruction_type MUST also be BOOL compatible.
			 */
			if (get_datatype_info_c::is_BOOL_compatible(prev_instruction_type) && get_datatype_info_c::is_BOOL_compatible(operand_type))
				add_datatype_to_candidate_list(symbol, prev_instruction_type);
		}
	}

	/* if the appropriate semantics is not a Set/Reset of a boolean variable, the we try for the FB invocation! */
	if (symbol->candidate_datatypes.size() == 0) {
		handle_implicit_il_fb_call(symbol,  operator_str, called_fb_declaration);
		/* If it is also not a valid FB call, make sure the candidate_datatypes is empty (handle_implicit_il_fb_call may leave it non-empty!!) */
		/* From here on out, all later code will consider the symbol->called_fb_declaration being NULL as an indication that this operator must use the
		 * Set/Reset semantics, so we must also guarantee that the remainder of the state of this symbol is compatible with that assumption!
		 */
		if (NULL == called_fb_declaration)
			symbol->candidate_datatypes.clear();
	}

	if (debug) std::cout << operator_str << " [" << prev_il_instruction->candidate_datatypes.size() << "," << il_operand->candidate_datatypes.size() << "] ==> "  << symbol->candidate_datatypes.size() << " result.\n";
	return NULL;
}



/* handle a binary IL operator, like ADD, SUB, etc... */
void *fill_candidate_datatypes_c::handle_binary_operator(const struct widen_entry widen_table[], symbol_c *symbol, symbol_c *l_expr, symbol_c *r_expr) {
	if (NULL == l_expr) return NULL; /* if no prev_il_instruction */
	if (NULL == r_expr) return NULL; /* if no IL operand!! */

	for(unsigned int i = 0; i < l_expr->candidate_datatypes.size(); i++)
		for(unsigned int j = 0; j < r_expr->candidate_datatypes.size(); j++)
			/* NOTE: add_datatype_to_candidate_list() will only really add the datatype if it is != NULL !!! */
			add_datatype_to_candidate_list(symbol, widening_conversion(l_expr->candidate_datatypes[i], r_expr->candidate_datatypes[j], widen_table));
	remove_incompatible_datatypes(symbol);
	if (debug) std::cout <<  "[" << l_expr->candidate_datatypes.size() << "," << r_expr->candidate_datatypes.size() << "] ==> "  << symbol->candidate_datatypes.size() << " result.\n";
	return NULL;
}



/* handle a binary ST expression, like '+', '-', etc... */
void *fill_candidate_datatypes_c::handle_binary_expression(const struct widen_entry widen_table[], symbol_c *symbol, symbol_c *l_expr, symbol_c *r_expr) {
	l_expr->accept(*this);
	r_expr->accept(*this);
	return handle_binary_operator(widen_table, symbol, l_expr, r_expr);
}



/* handle the two equality comparison operations, i.e. = (euqal) and != (not equal) */
/* This function is special, as it will also allow enumeration data types to be compared, with the result being a BOOL data type!
 * It will also allow to REF_TO datatypes to be compared.
 * These possibilities are not expressed in the 'widening' tables, so we need to hard code it here
 */
void *fill_candidate_datatypes_c::handle_equality_comparison(const struct widen_entry widen_table[], symbol_c *symbol, symbol_c *l_expr, symbol_c *r_expr) {
	handle_binary_expression(widen_table, symbol, l_expr, r_expr);
	for(unsigned int i = 0; i < l_expr->candidate_datatypes.size(); i++)
		for(unsigned int j = 0; j < r_expr->candidate_datatypes.size(); j++) {
			if (   (get_datatype_info_c::is_enumerated(l_expr->candidate_datatypes[i]) && (l_expr->candidate_datatypes[i] == r_expr->candidate_datatypes[j]))
			    || (get_datatype_info_c::is_ref_to    (l_expr->candidate_datatypes[i]) && get_datatype_info_c::is_type_equal(l_expr->candidate_datatypes[i], r_expr->candidate_datatypes[j])))   
				add_datatype_to_candidate_list(symbol, &get_datatype_info_c::bool_type_name);
		}
	return NULL;
}



/* a helper function... */
symbol_c *fill_candidate_datatypes_c::base_type(symbol_c *symbol) {
	/* NOTE: symbol == NULL is valid. It will occur when, for e.g., an undefined/undeclared symbolic_variable is used in the code. */
	if (symbol == NULL) return NULL;
	return search_base_type_c::get_basetype_decl(symbol);
}


/***************************/
/* B 0 - Programming Model */
/***************************/
/* main entry function! */
void *fill_candidate_datatypes_c::visit(library_c *symbol) {
	symbol->accept(populate_globalenumvalue_symtable);
	/* Now let the base class iterator_visitor_c iterate through all the library elements */
	return iterator_visitor_c::visit(symbol);  
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
void *fill_candidate_datatypes_c::visit(derived_datatype_identifier_c *symbol) {
  add_datatype_to_candidate_list(symbol, base_type(                type_symtable[symbol->value])); // will only add if datatype is not NULL!
  return NULL;
}

/* The datatype of a poutype_identifier_c is currently always being set by the parent object, so we leave this code commented out for now. */
/* 
void *fill_candidate_datatypes_c::visit(         poutype_identifier_c *symbol) {
  add_datatype_to_candidate_list(symbol, base_type( function_block_type_symtable[symbol->value])); // will only add if datatype is not NULL!
  add_datatype_to_candidate_list(symbol, base_type(        program_type_symtable[symbol->value])); // will only add if datatype is not NULL!
//add_datatype_to_candidate_list(symbol, base_type(            function_symtable[symbol->value])); // will only add if datatype is not NULL!
  // NOTE: Although poutype_identifier_c is also used to identify functions, these symbols are not used as a datatype, 
  //       so we do NOT add it as a candidate datatype!
  return NULL;
}
*/
    
    
/*********************/
/* B 1.2 - Constants */
/*********************/
/*********************************/
/* B 1.2.XX - Reference Literals */
/*********************************/
/* defined in IEC 61131-3 v3 - Basically the 'NULL' keyword! */
void *fill_candidate_datatypes_c::visit(ref_value_null_literal_c *symbol) {
	/* 'NULL' does not have any specific datatype. It is compatible with any reference, i.e. REF_TO <anything>
	 * The fill_candidate_datatypes / narrow_candidate_datatypes algorithm would require us to add
	 * as possible datatypes all the REF_TO <datatype>. To do this we would need to go through the list of all 
	 * user declared datatypes, as well as all the elementary datatypes. This is easily done by using the
	 * type_symtable symbol table declared in absyntax_utils.hh.
	 * 
	 *  for(int i=0; i<type_symtable.n; i++)  add_datatype_to_candidate_list(symbol, new ref_spec_c(type_symtable[i]));
	 *  add_datatype_to_candidate_list(symbol, new ref_spec_c( ... SINT ...));
	 *  add_datatype_to_candidate_list(symbol, new ref_spec_c( ...  INT ...));
	 *  add_datatype_to_candidate_list(symbol, new ref_spec_c( ... LINT ...));
	 *     ...
	 * 
	 * However, doing this for all NULL constants that may show up is probably a little too crazy, just for 
	 * the 'pleasure' of following the standard fill/narrow algorithm.
	 *
	 * I have therefore opted to handle this as a special case:
	 *     We use the ref_spec_c, pointing to a generic_type_any_c, as a pointer to ANY (basically, a void *)
	 */
	add_datatype_to_candidate_list(symbol, new ref_spec_c(new generic_type_any_c()));
	return NULL;
}


/******************************/
/* B 1.2.1 - Numeric Literals */
/******************************/
#define sizeoftype(symbol) get_sizeof_datatype_c::getsize(symbol)

void *fill_candidate_datatypes_c::handle_any_integer(symbol_c *symbol) {
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::bool_type_name,  &get_datatype_info_c::safebool_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::byte_type_name,  &get_datatype_info_c::safebyte_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::word_type_name,  &get_datatype_info_c::safeword_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::dword_type_name, &get_datatype_info_c::safedword_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::lword_type_name, &get_datatype_info_c::safelword_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::sint_type_name,  &get_datatype_info_c::safesint_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::int_type_name,   &get_datatype_info_c::safeint_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::dint_type_name,  &get_datatype_info_c::safedint_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::lint_type_name,  &get_datatype_info_c::safelint_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::usint_type_name, &get_datatype_info_c::safeusint_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::uint_type_name,  &get_datatype_info_c::safeuint_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::udint_type_name, &get_datatype_info_c::safeudint_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::ulint_type_name, &get_datatype_info_c::safeulint_type_name);
	remove_incompatible_datatypes(symbol);
	if (debug) std::cout << "ANY_INT [" << symbol->candidate_datatypes.size()<< "]" << std::endl;
	return NULL;
}



void *fill_candidate_datatypes_c::handle_any_real(symbol_c *symbol) {
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::real_type_name,  &get_datatype_info_c::safereal_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::lreal_type_name, &get_datatype_info_c::safelreal_type_name);
	remove_incompatible_datatypes(symbol);
	if (debug) std::cout << "ANY_REAL [" << symbol->candidate_datatypes.size() << "]" << std::endl;
	return NULL;
}



void *fill_candidate_datatypes_c::handle_any_literal(symbol_c *symbol, symbol_c *symbol_value, symbol_c *symbol_type) {
	symbol_value->accept(*this);
	if (search_in_candidate_datatype_list(symbol_type, symbol_value->candidate_datatypes) >= 0)
		add_datatype_to_candidate_list(symbol, symbol_type);
	remove_incompatible_datatypes(symbol);
	if (debug) std::cout << "ANY_LITERAL [" << symbol->candidate_datatypes.size() << "]\n";
	return NULL;
}



void *fill_candidate_datatypes_c::visit(    real_c *symbol) {return handle_any_real(symbol);}
void *fill_candidate_datatypes_c::visit(neg_real_c *symbol) {return handle_any_real(symbol);}



void *fill_candidate_datatypes_c::visit(neg_integer_c *symbol) {
	/* Please read the comment in neg_expression_c method, as it also applies here */
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::int_type_name, &get_datatype_info_c::safeint_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::sint_type_name, &get_datatype_info_c::safesint_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::dint_type_name, &get_datatype_info_c::safedint_type_name);
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::lint_type_name, &get_datatype_info_c::safelint_type_name);
	remove_incompatible_datatypes(symbol);
	if (debug) std::cout << "neg ANY_INT [" << symbol->candidate_datatypes.size() << "]" << std::endl;
	return NULL;
}



void *fill_candidate_datatypes_c::visit(integer_c        *symbol) {return handle_any_integer(symbol);}
void *fill_candidate_datatypes_c::visit(binary_integer_c *symbol) {return handle_any_integer(symbol);}
void *fill_candidate_datatypes_c::visit(octal_integer_c  *symbol) {return handle_any_integer(symbol);}
void *fill_candidate_datatypes_c::visit(hex_integer_c    *symbol) {return handle_any_integer(symbol);}



// SYM_REF2(integer_literal_c, type, value)
/*
 * integer_literal:
 *   integer_type_name '#' signed_integer
 * | integer_type_name '#' binary_integer
 * | integer_type_name '#' octal_integer
 * | integer_type_name '#' hex_integer
 */
void *fill_candidate_datatypes_c::visit(   integer_literal_c *symbol) {return handle_any_literal(symbol, symbol->value, symbol->type);}
void *fill_candidate_datatypes_c::visit(      real_literal_c *symbol) {return handle_any_literal(symbol, symbol->value, symbol->type);}
void *fill_candidate_datatypes_c::visit(bit_string_literal_c *symbol) {return handle_any_literal(symbol, symbol->value, symbol->type);}

void *fill_candidate_datatypes_c::visit(   boolean_literal_c *symbol) {
	if (NULL != symbol->type) return handle_any_literal(symbol, symbol->value, symbol->type);

	symbol->value->accept(*this);
	symbol->candidate_datatypes = symbol->value->candidate_datatypes;
	return NULL;
}


void *fill_candidate_datatypes_c::visit(boolean_true_c *symbol) {
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::bool_type_name, &get_datatype_info_c::safebool_type_name);
	return NULL;
}

void *fill_candidate_datatypes_c::visit(boolean_false_c *symbol) {
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::bool_type_name, &get_datatype_info_c::safebool_type_name);
	return NULL;
}

/*******************************/
/* B.1.2.2   Character Strings */
/*******************************/
void *fill_candidate_datatypes_c::visit(double_byte_character_string_c *symbol) {
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::wstring_type_name, &get_datatype_info_c::safewstring_type_name);
	return NULL;
}

void *fill_candidate_datatypes_c::visit(single_byte_character_string_c *symbol) {
	add_2datatypes_to_candidate_list(symbol, &get_datatype_info_c::string_type_name, &get_datatype_info_c::safestring_type_name);
	return NULL;
}

/***************************/
/* B 1.2.3 - Time Literals */
/***************************/
/************************/
/* B 1.2.3.1 - Duration */
/************************/
void *fill_candidate_datatypes_c::visit(duration_c *symbol) {
	add_datatype_to_candidate_list(symbol, symbol->type_name);
	if (debug) std::cout << "TIME_LITERAL [" << symbol->candidate_datatypes.size() << "]\n";
	return NULL;
}

/************************************/
/* B 1.2.3.2 - Time of day and Date */
/************************************/
void *fill_candidate_datatypes_c::visit(time_of_day_c   *symbol) {add_datatype_to_candidate_list(symbol, symbol->type_name); return NULL;}
void *fill_candidate_datatypes_c::visit(date_c          *symbol) {add_datatype_to_candidate_list(symbol, symbol->type_name); return NULL;}
void *fill_candidate_datatypes_c::visit(date_and_time_c *symbol) {add_datatype_to_candidate_list(symbol, symbol->type_name); return NULL;}

/**********************/
/* B 1.3 - Data types */
/**********************/
/********************************/
/* B 1.3.3 - Derived data types */
/********************************/

void *fill_candidate_datatypes_c::fill_type_decl(symbol_c *symbol, symbol_c *type_name, symbol_c *spec_init) {
  /* NOTE: Unlike the rest of the 'fill' algorithm that works using a bottom->up approach, when handling 
   *       data type declarations (section B.1.3.3 - Derived data types) we use a top->bottom approach. 
   *       This is intentional, and not a bug! Explanation follows...
   *       Here we are essentially determining the base type of each defined data type. In many cases (especially structs,
   *       enumerations, arrays, etc...), the datatype is its own base type. However, the derived datatype is stored in
   *       multiple symbol_c classes (e.g. an enumeration uses enumerated_type_declaration_c, enumerated_spec_init_c,
   *       enumerated_value_list_c, enumerated_value_c, ...). Several of these could be chosen to work as the canonical base datatype
   *       symbol. Which symbol is used is really up to the search_base_type_c, and not this fill_candidate_datatypes_c.
   *       Here we must right the code to handle whatever the search_base_type_c chooses to use as the canonical symbol to represent
   *       the base datatype.
   *       Since the base datatype may be (and sometimes/often/always(?) actually is) the top level symbol_c (an enumerated_type_declaration_c
   *       in the case of the enumerations), it only makes sense to ask search_base_type_c for a basetype when we pass it the 
   *       symbol in the highest level of the type declaration (the enumerated_type_declaration_c in the case of the enumerations).
   *       For this reason, we determine the basetype at the top level, and send that info down to the bottom level of the data type 
   *       declaration. In summary, a top->down algorithm!
   */ 
  add_datatype_to_candidate_list(symbol, base_type(symbol));
  type_name->candidate_datatypes = symbol->candidate_datatypes;  // use top->down algorithm!!
  spec_init->candidate_datatypes = symbol->candidate_datatypes;  // use top->down algorithm!!
  spec_init->accept(*this);
  return NULL;
}


void *fill_candidate_datatypes_c::fill_spec_init(symbol_c *symbol, symbol_c *type_spec, symbol_c *init_value) {
	/* NOTE: The note in the fill_type_decl() function is also partially valid here, 
	 *       i.e. here too we work using a top->down algorithm for the type_spec part, but a bottom->up algorithm
	 *       for the init_value part!!
	 */
	/* NOTE: When a variable is declared inside a POU as, for example
	 *         VAR
	 *            a : ARRAY[9] OF REAL;
	 *            e : ENUM (black, white, gray);
	 *            s : STRUCT x, y: REAL; END_STRUCT
	 *         END_VAR
	 *      the anonymous datatype will be defined directly by the ***_spec_init_c, and will not have an
	 *      ****_type_declaration_c. In these cases, the anonymous data type is its own basetype, and the
	 *      ***_spec_init_c class will act as the canonical symbol that represents the (anonymous) basetype.
	 *      
	 *      This method must handle the above case, as well as the case in which the ***_spec_init_c is called
	 *      from an ****_type_declaration_c.
	 */
	if (symbol->candidate_datatypes.size() == 0) // i.e., if this is an anonymous datatype!
		add_datatype_to_candidate_list(symbol, base_type(symbol)); 
	
	// use top->down algorithm!!
	type_spec->candidate_datatypes = symbol->candidate_datatypes;   
	type_spec->accept(*this);
	
	// use bottom->up algorithm!!
	if (NULL != init_value) init_value->accept(*this);  
	/* NOTE: Even if the constant and the type are of incompatible data types, we let the
	 *       ***_spec_init_c object inherit the data type of the type declaration (simple_specification)
	 *       This will let us produce more informative error messages when checking data type compatibility
	 *       with located variables (AT %QW3.4 : WORD).
	 */
	// if (NULL != init_value) intersect_candidate_datatype_list(symbol /*origin, dest.*/, init_value /*with*/);
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
void *fill_candidate_datatypes_c::visit(simple_type_declaration_c *symbol) {return fill_type_decl(symbol, symbol->simple_type_name, symbol->simple_spec_init);}
  

/* simple_specification ASSIGN constant */
// SYM_REF2(simple_spec_init_c, simple_specification, constant)
void *fill_candidate_datatypes_c::visit(simple_spec_init_c *symbol) {return fill_spec_init(symbol, symbol->simple_specification, symbol->constant);}


/*  subrange_type_name ':' subrange_spec_init */
// SYM_REF2(subrange_type_declaration_c, subrange_type_name, subrange_spec_init)
void *fill_candidate_datatypes_c::visit(subrange_type_declaration_c *symbol) {return fill_type_decl(symbol, symbol->subrange_type_name, symbol->subrange_spec_init);}

/* subrange_specification ASSIGN signed_integer */
// SYM_REF2(subrange_spec_init_c, subrange_specification, signed_integer)
void *fill_candidate_datatypes_c::visit(subrange_spec_init_c *symbol) {return fill_spec_init(symbol, symbol->subrange_specification, symbol->signed_integer);}

/*  integer_type_name '(' subrange')' */
// SYM_REF2(subrange_specification_c, integer_type_name, subrange)
// NOTE: not needed! Iterator visitor already handles this!

/*  signed_integer DOTDOT signed_integer */
/* dimension will be filled in during stage 3 (array_range_check_c) with the number of elements in this subrange */
// SYM_REF2(subrange_c, lower_limit, upper_limit, unsigned long long int dimension;)
void *fill_candidate_datatypes_c::visit(subrange_c *symbol) {
	symbol->lower_limit->accept(*this);
	symbol->upper_limit->accept(*this);
	
	for (unsigned int u = 0; u < symbol->upper_limit->candidate_datatypes.size(); u++) {
		for(unsigned int l = 0; l < symbol->lower_limit->candidate_datatypes.size(); l++) {
			if (get_datatype_info_c::is_type_equal(symbol->upper_limit->candidate_datatypes[u], symbol->lower_limit->candidate_datatypes[l]))
				add_datatype_to_candidate_list(symbol, symbol->lower_limit->candidate_datatypes[l]);
		}
	}
	return NULL;
}


/*  enumerated_type_name ':' enumerated_spec_init */
// SYM_REF2(enumerated_type_declaration_c, enumerated_type_name, enumerated_spec_init)
void *fill_candidate_datatypes_c::visit(enumerated_type_declaration_c *symbol) {return fill_type_decl(symbol, symbol->enumerated_type_name, symbol->enumerated_spec_init);}


/* enumerated_specification ASSIGN enumerated_value */
// SYM_REF2(enumerated_spec_init_c, enumerated_specification, enumerated_value)
// NOTE: enumerated_specification is either an enumerated_value_list_c or derived_datatype_identifier_c.
void *fill_candidate_datatypes_c::visit(enumerated_spec_init_c *symbol) {return fill_spec_init(symbol, symbol->enumerated_specification, symbol->enumerated_value);}


/* helper symbol for enumerated_specification->enumerated_spec_init */
/* enumerated_value_list ',' enumerated_value */
// SYM_LIST(enumerated_value_list_c)
void *fill_candidate_datatypes_c::visit(enumerated_value_list_c *symbol) {
  if (symbol->candidate_datatypes.size() != 1) ERROR;
  symbol_c *current_enumerated_spec_type = symbol->candidate_datatypes[0];
  
  /* We already know the datatype of the enumerated_value(s) in the list, so we set them directly instead of recursively calling the enumerated_value_c visit method! */
  for(int i = 0; i < symbol->n; i++)
    add_datatype_to_candidate_list(symbol->elements[i], current_enumerated_spec_type); // top->down algorithm!!

  return NULL;  
}


/* enumerated_type_name '#' identifier */
// SYM_REF2(enumerated_value_c, type, value)
/* WARNING: The enumerated_value_c is used when delcaring an enumerated datatype
 *          (e.g.   TYPE enumT: (xxx1, xxx2); END_TYPE ---> xxx1 and xxx2 will be enumerated_value_c)
 *          as well as in the source code of POU bodies
 *          (e.g.    enumVar := xxx1    ---> xxx1 will be enumerated_value_c)
 *
 *          The following method will only be used to visit enumerated_value_c that show up inside the 
 *          source code of POU bodies (or the initial values of an enumerated type). When used inside an 
 *          enumerated type declaration to list the possible enum values (whether inside
 *          a TYPE ... END_TYPE, or inside a VAR .. END_VAR), the visitor method for enumerated_value_list_c
 *          will NOT recursively call the following enumerated_value_c visitor method!
 */
void *fill_candidate_datatypes_c::visit(enumerated_value_c *symbol) {
	symbol_c *global_enumerated_type;
	symbol_c *local_enumerated_type;
	symbol_c *enumerated_type = NULL;

	if (NULL != symbol->type) {
		/* NOTE: This code must take into account the following situation:
		 *
		 *        TYPE  
		 *           base_enum_t: (x1, x2, x3);
		 *           enum_t1 : base_enum_t := x1;
		 *           enum_t2 : base_enum_t := x2;
		 *           enum_t12: enum_t1     := x2;
		 *        END_TYPE
		 *
		 *     considering the above, ALL of the following are correct!
		 *         base_enum_t#x1
		 *             enum_t1#x1
		 *             enum_t2#x1
		 *            enum_t12#x1
		 */
		/* check whether the value really belongs to that datatype!! */
		/* All local enum values are declared inside anonymous enumeration datatypes (i.e. inside a VAR ... END_VAR declaration, with
		 * the enum type having no type name), so thay cannot possibly be referenced using a datatype_t#enumvalue syntax.
		 * Because of this, we only look for the datatype identifier in the global enum value symbol table!
		 */
		enumerated_type = NULL;  // assume error...
		enumerated_value_symtable_t::iterator lower = global_enumerated_value_symtable.lower_bound(symbol->value);
		enumerated_value_symtable_t::iterator upper = global_enumerated_value_symtable.upper_bound(symbol->value);
		for (; lower != upper; lower++)
			if (get_datatype_info_c::is_type_equal(base_type(lower->second), base_type(symbol->type)))
				enumerated_type = symbol->type; 
	}
	else {
		symbol_c *global_enumerated_type = global_enumerated_value_symtable.find (symbol->value)->second;
		symbol_c * local_enumerated_type =  local_enumerated_value_symtable.find (symbol->value)->second;
		int       global_multiplicity    = global_enumerated_value_symtable.count(symbol->value);
		int        local_multiplicity    =  local_enumerated_value_symtable.count(symbol->value);

		if      (( local_multiplicity == 0) && (global_multiplicity == 0))
		  enumerated_type = NULL; // not found!
		else if (  local_multiplicity  > 1)
			enumerated_type = NULL; // Local duplicate, so it is ambiguous!
		else if (  local_multiplicity == 1)
			enumerated_type = local_enumerated_type;
		else if ( global_multiplicity  > 1)
			enumerated_type = NULL; // Global duplicate, so it is ambiguous!
		else if ( global_multiplicity == 1)
			enumerated_type = global_enumerated_type;
		else ERROR;
	}
	enumerated_type = base_type(enumerated_type);
	if (NULL != enumerated_type)
		add_datatype_to_candidate_list(symbol, enumerated_type);

	if (debug) std::cout << "ENUMERATE [" << symbol->candidate_datatypes.size() << "]\n";
	return NULL;
}


/*  identifier ':' array_spec_init */
// SYM_REF2(array_type_declaration_c, identifier, array_spec_init)
void *fill_candidate_datatypes_c::visit(array_type_declaration_c *symbol) {return fill_type_decl(symbol, symbol->identifier, symbol->array_spec_init);}

/* array_specification [ASSIGN array_initialization} */
/* array_initialization may be NULL ! */
// SYM_REF2(array_spec_init_c, array_specification, array_initialization)
void *fill_candidate_datatypes_c::visit(array_spec_init_c *symbol) {return fill_spec_init(symbol, symbol->array_specification, symbol->array_initialization);}

/* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
// SYM_REF2(array_specification_c, array_subrange_list, non_generic_type_name)
// void *fill_candidate_datatypes_c::visit(array_specification_c *symbol)
//  NOTE: This visitor does not need to be implemented as fill_candidate_datatypes_c inherits from iterator_visitor_c

/* helper symbol for array_specification */
/* array_subrange_list ',' subrange */
// SYM_LIST(array_subrange_list_c)

/* array_initialization:  '[' array_initial_elements_list ']' */
/* helper symbol for array_initialization */
/* array_initial_elements_list ',' array_initial_elements */
// SYM_LIST(array_initial_elements_list_c)

/* integer '(' [array_initial_element] ')' */
/* array_initial_element may be NULL ! */
// SYM_REF2(array_initial_elements_c, integer, array_initial_element)

/*  structure_type_name ':' structure_specification */
// SYM_REF2(structure_type_declaration_c, structure_type_name, structure_specification)
void *fill_candidate_datatypes_c::visit(structure_type_declaration_c *symbol) {return fill_type_decl(symbol, symbol->structure_type_name, symbol->structure_specification);}

/* structure_type_name ASSIGN structure_initialization */
/* structure_initialization may be NULL ! */
// SYM_REF2(initialized_structure_c, structure_type_name, structure_initialization)
void *fill_candidate_datatypes_c::visit(initialized_structure_c *symbol) {return fill_spec_init(symbol, symbol->structure_type_name, symbol->structure_initialization);}

/* helper symbol for structure_declaration */
/* structure_declaration:  STRUCT structure_element_declaration_list END_STRUCT */
/* structure_element_declaration_list structure_element_declaration ';' */
// SYM_LIST(structure_element_declaration_list_c)

/*  structure_element_name ':' *_spec_init */
// SYM_REF2(structure_element_declaration_c, structure_element_name, spec_init)

/* helper symbol for structure_initialization */
/* structure_initialization: '(' structure_element_initialization_list ')' */
/* structure_element_initialization_list ',' structure_element_initialization */
// SYM_LIST(structure_element_initialization_list_c)

/*  structure_element_name ASSIGN value */
// SYM_REF2(structure_element_initialization_c, structure_element_name, value)

/*  string_type_name ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init */
// SYM_REF4(string_type_declaration_c, string_type_name, elementary_string_type_name, string_type_declaration_size, string_type_declaration_init/* may be == NULL! */) 


/*  function_block_type_name ASSIGN structure_initialization */
/* structure_initialization -> may be NULL ! */
// SYM_REF2(fb_spec_init_c, function_block_type_name, structure_initialization)
void *fill_candidate_datatypes_c::visit(fb_spec_init_c *symbol) {return fill_spec_init(symbol, symbol->function_block_type_name, symbol->structure_initialization);}


/* REF_TO (non_generic_type_name | function_block_type_name) */
// SYM_REF1(ref_spec_c, type_name)
void *fill_candidate_datatypes_c::visit(ref_spec_c *symbol) {
  
	// when parsing datatype declarations, fill_candidate_datatypes_c follows a top->down algorithm (see the comment in fill_type_decl() for an explanation)
	add_datatype_to_candidate_list(symbol->type_name, base_type(symbol->type_name)); 
	symbol->type_name->accept(*this);  /* The referenced/pointed to datatype! */

	if (symbol->candidate_datatypes.size() == 0) // i.e., if this is an anonymous datatype!
		add_datatype_to_candidate_list(symbol, base_type(symbol)); 

	return NULL;
}

/* For the moment, we do not support initialising reference data types */
/* ref_spec [ ASSIGN ref_initialization ] */ 
/* NOTE: ref_initialization may be NULL!! */
// SYM_REF2(ref_spec_init_c, ref_spec, ref_initialization)
void *fill_candidate_datatypes_c::visit(ref_spec_init_c *symbol) {return fill_spec_init(symbol, symbol->ref_spec, symbol->ref_initialization);}

/* identifier ':' ref_spec_init */
// SYM_REF2(ref_type_decl_c, ref_type_name, ref_spec_init)
void *fill_candidate_datatypes_c::visit(ref_type_decl_c *symbol) {return fill_type_decl(symbol, symbol->ref_type_name, symbol->ref_spec_init);}





/*********************/
/* B 1.4 - Variables */
/*********************/
void *fill_candidate_datatypes_c::visit(symbolic_variable_c *symbol) {
	symbol->scope = current_scope;  // the scope in which this variable was declared!
	add_datatype_to_candidate_list(symbol, search_var_instance_decl->get_basetype_decl(symbol)); /* will only add if non NULL */
	if (debug) std::cout << "VAR [" << symbol->candidate_datatypes.size() << "]\n";
	return NULL;
}


/********************************************/
/* B 1.4.1 - Directly Represented Variables */
/********************************************/
void *fill_candidate_datatypes_c::visit(direct_variable_c *symbol) {
	/* Comment added by mario:
	 * The following code is safe, actually, as the lexical parser guarantees the correct IEC61131-3 syntax was used.
	 */
	/* However, we should probably add an assertion in case we later change the lexical parser! */
	/* if (symbol->value == NULL) ERROR;
	 * if (symbol->value[0] == '\0') ERROR;
	 * if (symbol->value[1] == '\0') ERROR;
	 */
	switch (symbol->value[2]) {
		case 'x': case 'X': /* bit   -  1 bit  */ add_datatype_to_candidate_list(symbol, &get_datatype_info_c::bool_type_name);  break;
		case 'b': case 'B': /* byte  -  8 bits */ add_datatype_to_candidate_list(symbol, &get_datatype_info_c::byte_type_name);  break;
		case 'w': case 'W': /* word  - 16 bits */ add_datatype_to_candidate_list(symbol, &get_datatype_info_c::word_type_name);  break;
		case 'd': case 'D': /* dword - 32 bits */ add_datatype_to_candidate_list(symbol, &get_datatype_info_c::dword_type_name); break;
		case 'l': case 'L': /* lword - 64 bits */ add_datatype_to_candidate_list(symbol, &get_datatype_info_c::lword_type_name); break;
        	          /* if none of the above, then the empty string was used <=> boolean */
		default:                        add_datatype_to_candidate_list(symbol, &get_datatype_info_c::bool_type_name);  break;
	}
	return NULL;
}

/*************************************/
/* B 1.4.2 - Multi-element variables */
/*************************************/
/*  subscripted_variable '[' subscript_list ']' */
// SYM_REF2(array_variable_c, subscripted_variable, subscript_list)
void *fill_candidate_datatypes_c::visit(array_variable_c *symbol) {
	/* recursively call the subscripted_variable. We need to do this since the array variable may be stored inside a structured
	 * variable (i.e. if it is an element inside a struct), in which case we want to recursively visit every element of the struct,
	 * as it may contain more arrays whose subscripts must also be visited!
	 * e.g.   structvar.a1[v1+2].b1.c1[v2+3].d1
	 *        TYPE
	 *           d_s: STRUCT d1: int; d2: int;
	 *           d_a: ARRAY [1..3] OF d_s;  
	 *           c_s: STRUCT c1: d_a; c2: d_a;
	 *           b_s: STRUCT b1: c_s; b2: c_s;
	 *           b_a: ARRAY [1..3] OF b_s;  
	 *           a_s: STRUCT a1: b_a; a2: b_a;
	 *        END_TYPE 
	 *        VAR
	 *          structvar: a_s;
	 *        END_VAR
	 */
	symbol->subscripted_variable->accept(*this);
	// the scope in which this variable was declared! It will be the same as the subscripted variable (a symbolic_variable_ !)
	symbol->scope = symbol->subscripted_variable->scope;
	if (NULL == symbol->scope) ERROR;

	
	for (unsigned int i = 0; i < symbol->subscripted_variable->candidate_datatypes.size(); i++) {
	  /* get the declaration of the data type __stored__ in the array... */
	  add_datatype_to_candidate_list(symbol, search_base_type_c::get_basetype_decl(get_datatype_info_c::get_array_storedtype_id(symbol->subscripted_variable->candidate_datatypes[i])));   /* will only add if non NULL */
	}

	/* recursively call the subscript list, so we can check the data types of the expressions used for the subscripts */
	symbol->subscript_list->accept(*this);

	if (debug) std::cout << "ARRAY_VAR [" << symbol->candidate_datatypes.size() << "]\n";	
	return NULL;
}


/* subscript_list ',' subscript */
// SYM_LIST(subscript_list_c)
/* NOTE: we inherit from iterator visitor, so we do not need to implement this method... */
// void *fill_candidate_datatypes_c::visit(subscript_list_c *symbol)


/*  record_variable '.' field_selector */
/*  WARNING: input and/or output variables of function blocks
 *           may be accessed as fields of a structured variable!
 *           Code handling a structured_variable_c must take
 *           this into account!
 */
// SYM_REF2(structured_variable_c, record_variable, field_selector)
void *fill_candidate_datatypes_c::visit(structured_variable_c *symbol) {
	/* Remember that a structured variable may be stored inside an array (e.g. arrayvar[33].elem1)
	 * The array subscripts may contain a complex expression (e.g. arrayvar[ varx + 33].elem1) whose datatype must be correctly determined!
	 * The expression, may even contain a function call to an overloaded function!
	 *      (e.g.  arrayvar[ varx + TRUNC(realvar)].elem1)
	 */
	symbol->record_variable->accept(*this);

	if (symbol->record_variable->candidate_datatypes.size() == 1) {
	  // set the scope in which this variable is declared (will be a struct datatype declaration!)
	  // We rely on the fact that if only one candidate datatype exists, then it will be the scope in which the field_variable is declared!
	  symbol->scope = symbol->record_variable->candidate_datatypes[0];  // the scope in which this variable was declared! Will be used in stage4
	  // Determine candidate datatypes...
	  add_datatype_to_candidate_list(symbol, search_base_type_c::get_basetype_decl(get_datatype_info_c::get_struct_field_type_id(symbol->scope, symbol->field_selector)));  /* will only add if non NULL */
	}

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
void *fill_candidate_datatypes_c::fill_var_declaration(symbol_c *var_list, symbol_c *type) {
  /* The type may be either a datatype object (e.g. array_spec_init_c, ...), or a derived_datatype_identifier_c
   * naming a previously declared datatype.
   * If it is a derived_datatype_identifier_c, we will search the list of all declared datatypes to determine 
   * the requested datatype. This is done automatically by the search_base_type_c::get_basetype_decl() method, 
   * so we do not need to do anything special here!
   */
  add_datatype_to_candidate_list(type, search_base_type_c::get_basetype_decl(type));  /* will only add if non NULL */
  type->accept(*this);
  // handle the extensible_input_parameter_c, etc...
  /* The extensible_input_parameter_c will be visited since this class inherits from the iterator_visitor_c.
   * It needs to be visited in order to handle the datatype of the first_index parameter of that class.
   */
  var_list->accept(*this);
  return NULL;
}


void *fill_candidate_datatypes_c::visit(var1_init_decl_c             *symbol) {return fill_var_declaration(symbol->var1_list,       symbol->spec_init);}
void *fill_candidate_datatypes_c::visit(array_var_init_decl_c        *symbol) {return fill_var_declaration(symbol->var1_list,       symbol->array_spec_init);}
void *fill_candidate_datatypes_c::visit(structured_var_init_decl_c   *symbol) {return fill_var_declaration(symbol->var1_list,       symbol->initialized_structure);}
void *fill_candidate_datatypes_c::visit(fb_name_decl_c               *symbol) {return fill_var_declaration(symbol->fb_name_list,    symbol->fb_spec_init);}
void *fill_candidate_datatypes_c::visit(array_var_declaration_c      *symbol) {return fill_var_declaration(symbol->var1_list,       symbol->array_specification);}
void *fill_candidate_datatypes_c::visit(structured_var_declaration_c *symbol) {return fill_var_declaration(symbol->var1_list,       symbol->structure_type_name);}
void *fill_candidate_datatypes_c::visit(external_declaration_c       *symbol) {return fill_var_declaration(symbol->global_var_name, symbol->specification);}
void *fill_candidate_datatypes_c::visit(global_var_decl_c            *symbol) {return fill_var_declaration(symbol->global_var_spec, symbol->type_specification);}
void *fill_candidate_datatypes_c::visit(incompl_located_var_decl_c   *symbol) {return fill_var_declaration(symbol->variable_name,   symbol->var_spec);}
//void *fill_candidate_datatypes_c::visit(single_byte_string_var_declaration_c *symbol) {return handle_var_declaration(symbol->single_byte_string_spec);}
//void *fill_candidate_datatypes_c::visit(double_byte_string_var_declaration_c *symbol) {return handle_var_declaration(symbol->double_byte_string_spec);}



// NOTE: this method is not required since fill_candidate_datatypes_c inherits from iterator_visitor_c. TODO: delete this method!
void *fill_candidate_datatypes_c::visit(var1_list_c *symbol) {
  for(int i = 0; i < symbol->n; i++) {symbol->elements[i]->accept(*this);}
  return NULL;
}  


/*  AT direct_variable */
// SYM_REF1(location_c, direct_variable)
void *fill_candidate_datatypes_c::visit(location_c *symbol) {
 /* This is a special situation. 
  *
  * The reason is that a located variable may be declared to be of any data type, as long as the size
  * matches the location (lines 1 3 and 4 of table 17). For example:
  *   var1 AT %MB42.0 : BYTE;
  *   var1 AT %MB42.1 : SINT;
  *   var1 AT %MB42.2 : USINT;
  *   var1 AT %MW64   : INT;
  *   var1 AT %MD56   : DINT;
  *   var1 AT %MD57   : REAL;
  *  are all valid!!
  *
  *  However, when used inside an expression, the direct variable (uses the same syntax as the location
  *  of a located variable) is limited to the following (ANY_BIT) data types:
  *    %MX --> BOOL
  *    %MB --> BYTE
  *    %MW --> WORD
  *    %MD --> DWORD
  *    %ML --> LWORD
  *
  *  So, in order to be able to analyse expressions with direct variables
  *   e.g:  var1 := 66 OR %MW34
  *  where the direct variable may only take the ANY_BIT data types, the fill_candidate_datatypes_c
  *  considers that only the ANY_BIT data types are allowed for a direct variable.
  *  However, it appears from the examples in the standard (lines 1 3 and 4 of table 17)
  *  a location may have any data type (presumably as long as the size in bits match).
  *  For this reason, a location_c may have more allowable data types than a direct_variable_c
  */

	symbol->direct_variable->accept(*this);
	for (unsigned int i = 0; i < symbol->direct_variable->candidate_datatypes.size(); i++) {
        	switch (get_sizeof_datatype_c::getsize(symbol->direct_variable->candidate_datatypes[i])) {
			case  1: /* bit   -  1 bit  */
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::bool_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safebool_type_name);
					break;
			case  8: /* byte  -  8 bits */
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::byte_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safebyte_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::sint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safesint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::usint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safeusint_type_name);
					break;
			case 16: /* word  - 16 bits */
	 				add_datatype_to_candidate_list(symbol, &get_datatype_info_c::word_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safeword_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::int_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safeint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::uint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safeuint_type_name);
					break;
			case 32: /* dword - 32 bits */
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::dword_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safedword_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::dint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safedint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::udint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safeudint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::real_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safereal_type_name);
					break;
			case 64: /* lword - 64 bits */
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::lword_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safelword_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::lint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safelint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::ulint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safeulint_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::lreal_type_name);
					add_datatype_to_candidate_list(symbol, &get_datatype_info_c::safelreal_type_name);
					break;
			default: /* if none of the above, then no valid datatype allowed... */
					break;
		} /* switch() */
	} /* for */

	return NULL;
}


/*  [variable_name] location ':' located_var_spec_init */
/* variable_name -> may be NULL ! */
// SYM_REF3(located_var_decl_c, variable_name, location, located_var_spec_init)
void *fill_candidate_datatypes_c::visit(located_var_decl_c *symbol) {
  symbol->located_var_spec_init->accept(*this);
  symbol->location->accept(*this);
  if (NULL != symbol->variable_name) {
    symbol->variable_name->candidate_datatypes = symbol->location->candidate_datatypes;
    intersect_candidate_datatype_list(symbol->variable_name /*origin, dest.*/, symbol->located_var_spec_init /*with*/);
  }
  return NULL;
}  



/************************************/
/* B 1.5 Program organization units */
/************************************/
/*********************/
/* B 1.5.1 Functions */
/*********************/
void *fill_candidate_datatypes_c::visit(function_declaration_c *symbol) {
	if (debug) printf("Filling candidate data types list of function %s\n", ((token_c *)(symbol->derived_function_name))->value);
	local_enumerated_value_symtable.reset();
	current_scope = symbol;	
	symbol->var_declarations_list->accept(populate_enumvalue_symtable);

	search_var_instance_decl = new search_var_instance_decl_c(symbol);
	symbol->var_declarations_list->accept(*this);
	symbol->function_body->accept(*this);
	delete search_var_instance_decl;
	search_var_instance_decl = NULL;

	current_scope = NULL;	
	local_enumerated_value_symtable.reset();
	return NULL;
}

/***************************/
/* B 1.5.2 Function blocks */
/***************************/
void *fill_candidate_datatypes_c::visit(function_block_declaration_c *symbol) {
	if (debug) printf("Filling candidate data types list of FB %s\n", ((token_c *)(symbol->fblock_name))->value);
	local_enumerated_value_symtable.reset();
	current_scope = symbol;	
	symbol->var_declarations->accept(populate_enumvalue_symtable);

	search_var_instance_decl = new search_var_instance_decl_c(symbol);
	symbol->var_declarations->accept(*this);
	symbol->fblock_body->accept(*this);
	delete search_var_instance_decl;
	search_var_instance_decl = NULL;

	current_scope = NULL;	
	local_enumerated_value_symtable.reset();
	
	/* The FB declaration itself may be used as a dataype! We now do the fill algorithm considering 
	 * function_block_declaration_c a data type declaration...
	 */
	// The next line is essentially equivalent to doing-->  symbol->candidate_datatypes.push_back(symbol);
	add_datatype_to_candidate_list(symbol, base_type(symbol));
	return NULL;
}

/**********************/
/* B 1.5.3 - Programs */
/**********************/
void *fill_candidate_datatypes_c::visit(program_declaration_c *symbol) {
	if (debug) printf("Filling candidate data types list in program %s\n", ((token_c *)(symbol->program_type_name))->value);
	local_enumerated_value_symtable.reset();
	current_scope = symbol;	
	symbol->var_declarations->accept(populate_enumvalue_symtable);
	
	search_var_instance_decl = new search_var_instance_decl_c(symbol);
	symbol->var_declarations->accept(*this);
	symbol->function_block_body->accept(*this);
	delete search_var_instance_decl;
	search_var_instance_decl = NULL;

	current_scope = NULL;	
	local_enumerated_value_symtable.reset();
	return NULL;
}

/********************************************/
/* B 1.6 Sequential function chart elements */
/********************************************/

void *fill_candidate_datatypes_c::visit(transition_condition_c *symbol) {
	symbol_c *condition_type;

	if (symbol->transition_condition_il != NULL) {
		symbol->transition_condition_il->accept(*this);
		for (unsigned int i = 0; i < symbol->transition_condition_il->candidate_datatypes.size(); i++) {
			condition_type = symbol->transition_condition_il->candidate_datatypes[i];
			if (get_datatype_info_c::is_BOOL_compatible(condition_type))
				add_datatype_to_candidate_list(symbol, condition_type);
		}
	}
	if (symbol->transition_condition_st != NULL) {
		symbol->transition_condition_st->accept(*this);
		for (unsigned int i = 0; i < symbol->transition_condition_st->candidate_datatypes.size(); i++) {
			condition_type = symbol->transition_condition_st->candidate_datatypes[i];
			if (get_datatype_info_c::is_BOOL_compatible(condition_type))
				add_datatype_to_candidate_list(symbol, condition_type);
		}
	}
	return NULL;
}

/********************************/
/* B 1.7 Configuration elements */
/********************************/
void *fill_candidate_datatypes_c::visit(configuration_declaration_c *symbol) {
	if (debug) printf("Filling candidate data types list in configuration %s\n", ((token_c *)(symbol->configuration_name))->value);
	current_scope = symbol;
//	local_enumerated_value_symtable.reset();  // TODO
//	symbol->global_var_declarations->accept(populate_enumvalue_symtable);  // TODO
	
	search_var_instance_decl = new search_var_instance_decl_c(symbol);
	symbol->global_var_declarations          ->accept(*this);
	symbol->resource_declarations            ->accept(*this); // points to a single_resource_declaration_c or a resource_declaration_list_c
//	symbol->access_declarations              ->accept(*this); // TODO
//	symbol->instance_specific_initializations->accept(*this); // TODO

	delete search_var_instance_decl;
	search_var_instance_decl = NULL;

	current_scope = NULL;	
//	local_enumerated_value_symtable.reset();  // TODO
	return NULL;
}


void *fill_candidate_datatypes_c::visit(resource_declaration_c *symbol) {
	if (debug) printf("Filling candidate data types list in resource %s\n", ((token_c *)(symbol->resource_name))->value);
//	local_enumerated_value_symtable.reset();  // TODO-> this must be replaced with local_enumerated_value_symtable.push(), which is not yet implemented for the dsyntable_c!
	symbol_c *prev_scope = current_scope;
	current_scope = symbol;
	/* TODO Enumeration constants may be defined inside a VAR_GLOBAL .. END_VAR variable declaration list. 
	 * We currently do not yet consider enumeration values defined in the var declarations inside a resource!
	 */
//	symbol->global_var_declarations->accept(populate_enumvalue_symtable);  // TODO!
	
	search_var_instance_decl_c *prev_search_var_instance_decl = search_var_instance_decl;
	search_var_instance_decl  = new  search_var_instance_decl_c(symbol);
	symbol->global_var_declarations->accept(*this);
	symbol->resource_declaration   ->accept(*this);  // points to a single_resource_declaration_c!

	delete search_var_instance_decl;
	search_var_instance_decl = prev_search_var_instance_decl;

	current_scope = prev_scope;	
//	local_enumerated_value_symtable.reset();  // TODO-> this must be replaced with local_enumerated_value_symtable.pop(), which is not yet implemented for the dsyntable_c!
	return NULL;
}


void *fill_candidate_datatypes_c::visit(single_resource_declaration_c *symbol) {
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
void *fill_candidate_datatypes_c::visit(instruction_list_c *symbol) {
	/* In order to fill the data type candidates correctly
	 * in IL instruction lists containing JMPs to labels that come before the JMP instruction
	 * itself, we need to run the fill candidate datatypes algorithm twice on the Instruction List.
	 * e.g.:  ...
	 *          ld 23
	 *   label1:st byte_var
	 *          ld 34
	 *          JMP label1     
	 *
	 * Note that the second time we run the algorithm, most of the candidate datatypes are already filled
	 * in, so it will be able to produce tha correct candidate datatypes for the IL instruction referenced
	 * by the label, as in the 2nd pass we already know the candidate datatypes of the JMP instruction!
	 */
	for(int j = 0; j < 2; j++) {
		for(int i = 0; i < symbol->n; i++) {
			symbol->elements[i]->accept(*this);
		}
	}
	return NULL;
}



/* | label ':' [il_incomplete_instruction] eol_list */
// SYM_REF2(il_instruction_c, label, il_instruction)
// void *visit(instruction_list_c *symbol);
void *fill_candidate_datatypes_c::visit(il_instruction_c *symbol) {
	if (NULL == symbol->il_instruction) {
		/* This empty/null il_instruction does not change the value of the current/default IL variable.
		 * So it inherits the candidate_datatypes from it's previous IL instructions!
		 */
		intersect_prev_candidate_datatype_lists(symbol);
	} else {
		il_instruction_c fake_prev_il_instruction = *symbol;
		intersect_prev_candidate_datatype_lists(&fake_prev_il_instruction);

		if (symbol->prev_il_instruction.size() == 0)  prev_il_instruction = NULL;
		else                                          prev_il_instruction = &fake_prev_il_instruction;
		symbol->il_instruction->accept(*this);
		prev_il_instruction = NULL;

		/* This object has (inherits) the same candidate datatypes as the il_instruction */
		symbol->candidate_datatypes = symbol->il_instruction->candidate_datatypes;
	}

	return NULL;
}



void *fill_candidate_datatypes_c::visit(il_simple_operation_c *symbol) {
	/* determine the data type of the operand */
	if (NULL != symbol->il_operand) {
		symbol->il_operand->accept(*this);
	}
	/* recursive call to fill the candidate data types list */
	il_operand = symbol->il_operand;
	symbol->il_simple_operator->accept(*this);
	il_operand = NULL;
	/* This object has (inherits) the same candidate datatypes as the il_simple_operator */
	symbol->candidate_datatypes = symbol->il_simple_operator->candidate_datatypes;
	return NULL;
}


/* | function_name [il_operand_list] */
/* NOTE: The parameters 'called_function_declaration' and 'extensible_param_count' are used to pass data between the stage 3 and stage 4. */
// SYM_REF2(il_function_call_c, function_name, il_operand_list, symbol_c *called_function_declaration; int extensible_param_count;)
void *fill_candidate_datatypes_c::visit(il_function_call_c *symbol) {
	/* The first parameter of a non formal function call in IL will be the 'current value' (i.e. the prev_il_instruction)
	 * In order to be able to handle this without coding special cases, we will simply prepend that symbol
	 * to the il_operand_list, and remove it after calling handle_function_call().
	 *
	 * However, if no further paramters are given, then il_operand_list will be NULL, and we will
	 * need to create a new object to hold the pointer to prev_il_instruction.
	 */
	if (NULL == symbol->il_operand_list)  symbol->il_operand_list = new il_operand_list_c;
	if (NULL == symbol->il_operand_list)  ERROR;

	symbol->il_operand_list->accept(*this);

	if (NULL != prev_il_instruction) {
		((list_c *)symbol->il_operand_list)->insert_element(prev_il_instruction, 0);	

		generic_function_call_t fcall_param = {
			/* fcall_param.function_name               = */ symbol->function_name,
			/* fcall_param.nonformal_operand_list      = */ symbol->il_operand_list,
			/* fcall_param.formal_operand_list         = */ NULL,
			/* enum {POU_FB, POU_function} POU_type    = */ generic_function_call_t::POU_function,
			/* fcall_param.candidate_functions         = */ symbol->candidate_functions,
			/* fcall_param.called_function_declaration = */ symbol->called_function_declaration,
			/* fcall_param.extensible_param_count      = */ symbol->extensible_param_count
		};
		handle_function_call(symbol, fcall_param);

		/* Undo the changes to the abstract syntax tree we made above... */
		((list_c *)symbol->il_operand_list)->remove_element(0);
	}

	/* Undo the changes to the abstract syntax tree we made above... */
	if (((list_c *)symbol->il_operand_list)->n == 0) {
		/* if the list becomes empty, then that means that it did not exist before we made these changes, so we delete it! */
		delete 	symbol->il_operand_list;
		symbol->il_operand_list = NULL;
	}
	
	if (debug) std::cout << "il_function_call_c [" << symbol->candidate_datatypes.size() << "] result.\n";
	return NULL;
}


/* | il_expr_operator '(' [il_operand] eol_list [simple_instr_list] ')' */
// SYM_REF3(il_expression_c, il_expr_operator, il_operand, simple_instr_list);
void *fill_candidate_datatypes_c::visit(il_expression_c *symbol) {
  symbol_c *prev_il_instruction_backup = prev_il_instruction;
  
  /* Stage2 will insert an artificial (and equivalent) LD <il_operand> to the simple_instr_list if necessary. We can therefore ignore the 'il_operand' entry! */
  // if (NULL != symbol->il_operand)
  //   symbol->il_operand->accept(*this);

  if(symbol->simple_instr_list != NULL)
    symbol->simple_instr_list->accept(*this);

  /* Since stage2 will insert an artificial (and equivalent) LD <il_operand> to the simple_instr_list when an 'il_operand' exists, we know
   * that if (symbol->il_operand != NULL), then the first IL instruction in the simple_instr_list will be the equivalent and artificial
   * 'LD <il_operand>' IL instruction.
   * Just to be cosistent, we will copy the datatype info back into the il_operand, even though this should not be necessary!
   */
  if ((NULL != symbol->il_operand) && ((NULL == symbol->simple_instr_list) || (0 == ((list_c *)symbol->simple_instr_list)->n))) ERROR; // stage2 is not behaving as we expect it to!
  if  (NULL != symbol->il_operand)
    symbol->il_operand->candidate_datatypes = ((list_c *)symbol->simple_instr_list)->elements[0]->candidate_datatypes;
  
  /* Now check the if the data type semantics of operation are correct,  */
  il_operand = symbol->simple_instr_list;
  prev_il_instruction = prev_il_instruction_backup;
  symbol->il_expr_operator->accept(*this);
  il_operand = NULL;
  
  /* This object has the same candidate datatypes as the il_expr_operator. */
  symbol->candidate_datatypes = symbol->il_expr_operator->candidate_datatypes;
  return NULL;
}


void *fill_candidate_datatypes_c::visit(il_jump_operation_c *symbol) {
  /* recursive call to fill the candidate data types list */
  il_operand = NULL;
  symbol->il_jump_operator->accept(*this);
  il_operand = NULL;
  /* This object has the same candidate datatypes as the il_jump_operator. */
  symbol->candidate_datatypes = symbol->il_jump_operator->candidate_datatypes;
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
void *fill_candidate_datatypes_c::visit(il_fb_call_c *symbol) {
	symbol_c *fb_decl = search_var_instance_decl->get_basetype_decl(symbol->fb_name);
	if (! get_datatype_info_c::is_function_block(fb_decl)) fb_decl = NULL;

	/* Although a call to a non-declared FB is a semantic error, this is currently caught by stage 2! */
	if (NULL == fb_decl) ERROR;

	if (symbol->  il_param_list != NULL) symbol->il_param_list->accept(*this);
	if (symbol->il_operand_list != NULL) symbol->il_operand_list->accept(*this);

	/* The print_datatypes_error_c does not rely on this called_fb_declaration pointer being != NULL to conclude that
	 * we have a datat type incompatibility error, so setting it to the correct fb_decl is actually safe,
	 * as the compiler will never reach the compilation stage!
	 */
	symbol->called_fb_declaration = fb_decl;

	/* Let the il_call_operator (CAL, CALC, or CALCN) determine the candidate datatypes of the il_fb_call_c... */
	/* NOTE: We ignore whether the call is 'compatible' or not when filling in the candidate datatypes list.
	 *       Even if it is not compatible, we fill in the candidate datatypes list correctly so that the following
	 *       IL instructions may be handled correctly and debuged.
	 *       Doing this is actually safe, as the parameter_list will still contain errors that will be found by
	 *       print_datatypes_error_c, so the code will never reach stage 4!
	 */
	symbol->il_call_operator->accept(*this);
	symbol->candidate_datatypes = symbol->il_call_operator->candidate_datatypes;

	if (debug) std::cout << "FB [] ==> "  << symbol->candidate_datatypes.size() << " result.\n";
	return NULL;
}


/* | function_name '(' eol_list [il_param_list] ')' */
/* NOTE: The parameter 'called_function_declaration' is used to pass data between the stage 3 and stage 4. */
// SYM_REF2(il_formal_funct_call_c, function_name, il_param_list, symbol_c *called_function_declaration; int extensible_param_count;)
void *fill_candidate_datatypes_c::visit(il_formal_funct_call_c *symbol) {
	symbol->il_param_list->accept(*this);

	generic_function_call_t fcall_param = {
		/* fcall_param.function_name               = */ symbol->function_name,
		/* fcall_param.nonformal_operand_list      = */ NULL,
		/* fcall_param.formal_operand_list         = */ symbol->il_param_list,
		/* enum {POU_FB, POU_function} POU_type    = */ generic_function_call_t::POU_function,
		/* fcall_param.candidate_functions         = */ symbol->candidate_functions,
		/* fcall_param.called_function_declaration = */ symbol->called_function_declaration,
		/* fcall_param.extensible_param_count      = */ symbol->extensible_param_count
	};
	handle_function_call(symbol, fcall_param);

	if (debug) std::cout << "il_formal_funct_call_c [" << symbol->candidate_datatypes.size() << "] result.\n";
	return NULL;
}


//     void *visit(il_operand_list_c *symbol);


/* | simple_instr_list il_simple_instruction */
/* This object is referenced by il_expression_c objects */
void *fill_candidate_datatypes_c::visit(simple_instr_list_c *symbol) {
  if (symbol->n <= 0)
    return NULL;  /* List is empty! Nothing to do. */
    
  for(int i = 0; i < symbol->n; i++)
    symbol->elements[i]->accept(*this);

  /* This object has (inherits) the same candidate datatypes as the last il_instruction */
  symbol->candidate_datatypes = symbol->elements[symbol->n-1]->candidate_datatypes;
  
  if (debug) std::cout << "simple_instr_list_c [" << symbol->candidate_datatypes.size() << "] result.\n";
  return NULL;
}




// SYM_REF1(il_simple_instruction_c, il_simple_instruction, symbol_c *prev_il_instruction;)
void *fill_candidate_datatypes_c::visit(il_simple_instruction_c *symbol) {
  if (symbol->prev_il_instruction.size() > 1) ERROR; /* There should be no labeled insructions inside an IL expression! */
  if (symbol->prev_il_instruction.size() == 0)  prev_il_instruction = NULL;
  else                                          prev_il_instruction = symbol->prev_il_instruction[0];
  symbol->il_simple_instruction->accept(*this);
  prev_il_instruction = NULL;

  /* This object has (inherits) the same candidate datatypes as the il_simple_instruction it points to */
  symbol->candidate_datatypes = symbol->il_simple_instruction->candidate_datatypes;
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
void *fill_candidate_datatypes_c::visit(LD_operator_c *symbol) {
	if (NULL == il_operand)          return NULL;
	for(unsigned int i = 0; i < il_operand->candidate_datatypes.size(); i++) {
		add_datatype_to_candidate_list(symbol, il_operand->candidate_datatypes[i]);
	}
	if (debug) std::cout << "LD [" <<  il_operand->candidate_datatypes.size() << "] ==> "  << symbol->candidate_datatypes.size() << " result.\n";
	return NULL;
}

void *fill_candidate_datatypes_c::visit(LDN_operator_c *symbol) {
	if (NULL == il_operand)          return NULL;
	for(unsigned int i = 0; i < il_operand->candidate_datatypes.size(); i++) {
		if      (get_datatype_info_c::is_ANY_BIT_compatible(il_operand->candidate_datatypes[i]))
			add_datatype_to_candidate_list(symbol, il_operand->candidate_datatypes[i]);
	}
	if (debug) std::cout << "LDN [" << il_operand->candidate_datatypes.size() << "] ==> "  << symbol->candidate_datatypes.size() << " result.\n";
	return NULL;
}

void *fill_candidate_datatypes_c::visit(ST_operator_c *symbol) {
	symbol_c *prev_instruction_type, *operand_type;

	if (NULL == prev_il_instruction) return NULL;
	if (NULL == il_operand)          return NULL;
	for (unsigned int i = 0; i < prev_il_instruction->candidate_datatypes.size(); i++) {
		for(unsigned int j = 0; j < il_operand->candidate_datatypes.size(); j++) {
			prev_instruction_type = prev_il_instruction->candidate_datatypes[i];
			operand_type = il_operand->candidate_datatypes[j];
			if (get_datatype_info_c::is_type_equal(prev_instruction_type, operand_type))
				add_datatype_to_candidate_list(symbol, prev_instruction_type);
		}
	}
	if (debug) std::cout << "ST [" << prev_il_instruction->candidate_datatypes.size() << "," << il_operand->candidate_datatypes.size() << "] ==> "  << symbol->candidate_datatypes.size() << " result.\n";
	return NULL;
}

void *fill_candidate_datatypes_c::visit(STN_operator_c *symbol) {
	symbol_c *prev_instruction_type, *operand_type;

	if (NULL == prev_il_instruction) return NULL;
	if (NULL == il_operand)          return NULL;
	for (unsigned int i = 0; i < prev_il_instruction->candidate_datatypes.size(); i++) {
		for(unsigned int j = 0; j < il_operand->candidate_datatypes.size(); j++) {
			prev_instruction_type = prev_il_instruction->candidate_datatypes[i];
			operand_type = il_operand->candidate_datatypes[j];
			if (get_datatype_info_c::is_type_equal(prev_instruction_type,operand_type) && get_datatype_info_c::is_ANY_BIT_compatible(operand_type))
				add_datatype_to_candidate_list(symbol, prev_instruction_type);
		}
	}
	if (debug) std::cout << "STN [" << prev_il_instruction->candidate_datatypes.size() << "," << il_operand->candidate_datatypes.size() << "] ==> "  << symbol->candidate_datatypes.size() << " result.\n";
	return NULL;
}

void *fill_candidate_datatypes_c::visit(NOT_operator_c *symbol) {
	/* NOTE: the standard allows syntax in which the NOT operator is followed by an optional <il_operand>
	 *              NOT [<il_operand>]
	 *       However, it does not define the semantic of the NOT operation when the <il_operand> is specified.
	 *       We therefore consider it an error if an il_operand is specified!
	 *       We do not need to generate an error message. This error will be caught somewhere else!
	 */
	if (NULL == prev_il_instruction) return NULL;
	if (NULL == il_operand)          return NULL;
	for (unsigned int i = 0; i < prev_il_instruction->candidate_datatypes.size(); i++) {
		if (get_datatype_info_c::is_ANY_BIT_compatible(prev_il_instruction->candidate_datatypes[i]))
			add_datatype_to_candidate_list(symbol, prev_il_instruction->candidate_datatypes[i]);
	}
	if (debug) std::cout <<  "NOT_operator [" << prev_il_instruction->candidate_datatypes.size() << "] ==> "  << symbol->candidate_datatypes.size() << " result.\n";
	return NULL;
}


void *fill_candidate_datatypes_c::visit(   S_operator_c *symbol) {return handle_S_and_R_operator   (symbol,   "S", symbol->called_fb_declaration);}
void *fill_candidate_datatypes_c::visit(   R_operator_c *symbol) {return handle_S_and_R_operator   (symbol,   "R", symbol->called_fb_declaration);}

void *fill_candidate_datatypes_c::visit(  S1_operator_c *symbol) {return handle_implicit_il_fb_call(symbol,  "S1", symbol->called_fb_declaration);}
void *fill_candidate_datatypes_c::visit(  R1_operator_c *symbol) {return handle_implicit_il_fb_call(symbol,  "R1", symbol->called_fb_declaration);}
void *fill_candidate_datatypes_c::visit( CLK_operator_c *symbol) {return handle_implicit_il_fb_call(symbol, "CLK", symbol->called_fb_declaration);}
void *fill_candidate_datatypes_c::visit(  CU_operator_c *symbol) {return handle_implicit_il_fb_call(symbol,  "CU", symbol->called_fb_declaration);}
void *fill_candidate_datatypes_c::visit(  CD_operator_c *symbol) {return handle_implicit_il_fb_call(symbol,  "CD", symbol->called_fb_declaration);}
void *fill_candidate_datatypes_c::visit(  PV_operator_c *symbol) {return handle_implicit_il_fb_call(symbol,  "PV", symbol->called_fb_declaration);}
void *fill_candidate_datatypes_c::visit(  IN_operator_c *symbol) {return handle_implicit_il_fb_call(symbol,  "IN", symbol->called_fb_declaration);}
void *fill_candidate_datatypes_c::visit(  PT_operator_c *symbol) {return handle_implicit_il_fb_call(symbol,  "PT", symbol->called_fb_declaration);}

void *fill_candidate_datatypes_c::visit( AND_operator_c *symbol) {return handle_binary_operator(widen_AND_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit(  OR_operator_c *symbol) {return handle_binary_operator( widen_OR_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit( XOR_operator_c *symbol) {return handle_binary_operator(widen_XOR_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit(ANDN_operator_c *symbol) {return handle_binary_operator(widen_AND_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit( ORN_operator_c *symbol) {return handle_binary_operator( widen_OR_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit(XORN_operator_c *symbol) {return handle_binary_operator(widen_XOR_table, symbol, prev_il_instruction, il_operand);}

void *fill_candidate_datatypes_c::visit( ADD_operator_c *symbol) {return handle_binary_operator(widen_ADD_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit( SUB_operator_c *symbol) {return handle_binary_operator(widen_SUB_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit( MUL_operator_c *symbol) {return handle_binary_operator(widen_MUL_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit( DIV_operator_c *symbol) {return handle_binary_operator(widen_DIV_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit( MOD_operator_c *symbol) {return handle_binary_operator(widen_MOD_table, symbol, prev_il_instruction, il_operand);}

void *fill_candidate_datatypes_c::visit(  GT_operator_c *symbol) {return handle_binary_operator(widen_CMP_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit(  GE_operator_c *symbol) {return handle_binary_operator(widen_CMP_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit(  EQ_operator_c *symbol) {return handle_binary_operator(widen_CMP_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit(  LT_operator_c *symbol) {return handle_binary_operator(widen_CMP_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit(  LE_operator_c *symbol) {return handle_binary_operator(widen_CMP_table, symbol, prev_il_instruction, il_operand);}
void *fill_candidate_datatypes_c::visit(  NE_operator_c *symbol) {return handle_binary_operator(widen_CMP_table, symbol, prev_il_instruction, il_operand);}



void *fill_candidate_datatypes_c::handle_conditional_il_flow_control_operator(symbol_c *symbol) {
	if (NULL == prev_il_instruction) return NULL;
	for (unsigned int i = 0; i < prev_il_instruction->candidate_datatypes.size(); i++) {
		if (get_datatype_info_c::is_BOOL_compatible(prev_il_instruction->candidate_datatypes[i]))
			add_datatype_to_candidate_list(symbol, prev_il_instruction->candidate_datatypes[i]);
	}
	return NULL;
}

void *fill_candidate_datatypes_c::visit(  CAL_operator_c *symbol) {if (NULL != prev_il_instruction) symbol->candidate_datatypes = prev_il_instruction->candidate_datatypes; return NULL;}
void *fill_candidate_datatypes_c::visit(  RET_operator_c *symbol) {if (NULL != prev_il_instruction) symbol->candidate_datatypes = prev_il_instruction->candidate_datatypes; return NULL;}
void *fill_candidate_datatypes_c::visit(  JMP_operator_c *symbol) {if (NULL != prev_il_instruction) symbol->candidate_datatypes = prev_il_instruction->candidate_datatypes; return NULL;}
void *fill_candidate_datatypes_c::visit( CALC_operator_c *symbol) {return handle_conditional_il_flow_control_operator(symbol);}
void *fill_candidate_datatypes_c::visit(CALCN_operator_c *symbol) {return handle_conditional_il_flow_control_operator(symbol);}
void *fill_candidate_datatypes_c::visit( RETC_operator_c *symbol) {return handle_conditional_il_flow_control_operator(symbol);}
void *fill_candidate_datatypes_c::visit(RETCN_operator_c *symbol) {return handle_conditional_il_flow_control_operator(symbol);}
void *fill_candidate_datatypes_c::visit( JMPC_operator_c *symbol) {return handle_conditional_il_flow_control_operator(symbol);}
void *fill_candidate_datatypes_c::visit(JMPCN_operator_c *symbol) {return handle_conditional_il_flow_control_operator(symbol);}




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
void *fill_candidate_datatypes_c::visit(deref_expression_c  *symbol) {
  symbol->exp->accept(*this);

  for (unsigned int i = 0; i < symbol->exp->candidate_datatypes.size(); i++) {
    /* Determine whether the datatype is a ref_spec_c, as this is the class used as the    */
    /* canonical/base datatype of REF_TO types (see search_base_type_c ...)                */ 
    ref_spec_c *ref_spec = dynamic_cast<ref_spec_c *>(symbol->exp->candidate_datatypes[i]);
    
    if (NULL != ref_spec)
      add_datatype_to_candidate_list(symbol, search_base_type_c::get_basetype_decl(ref_spec->type_name));
  }
  
  return NULL;
}


/* SYM_REF1(deref_operator_c, exp)  --> an extension to the IEC 61131-3 standard - based on the IEC 61131-3 v3 standard. Returns address of the varible! */
void *fill_candidate_datatypes_c::visit(deref_operator_c  *symbol) {
  symbol->exp->accept(*this);

  for (unsigned int i = 0; i < symbol->exp->candidate_datatypes.size(); i++) {
    /* Determine whether the datatype is a ref_spec_c, as this is the class used as the    */
    /* canonical/base datatype of REF_TO types (see search_base_type_c ...)                */ 
    ref_spec_c *ref_spec = dynamic_cast<ref_spec_c *>(symbol->exp->candidate_datatypes[i]);
    
    if (NULL != ref_spec)
      add_datatype_to_candidate_list(symbol, search_base_type_c::get_basetype_decl(ref_spec->type_name));
    
  }

  /* Since the deref_operator_c may be used inside structures, we must handle set the 'scope' annotation here too! */
  symbol->scope = symbol->exp->scope;
  
  return NULL;
}


/* SYM_REF1(ref_expression_c, exp)  --> an extension to the IEC 61131-3 standard - based on the IEC 61131-3 v3 standard. Returns address of the varible! */
void *fill_candidate_datatypes_c::visit(  ref_expression_c  *symbol) {
  /* 
   * Note that symbol->exp may be an array variable, and these may containg complex
   * expressions that include function calls in their indexes. These complex expressions must also be
   * analysed using the standard fill/narrow algorithm...
   */
  symbol->exp->accept(*this);

  /* Currently the IEC 61131-3 symtax requires that the REF() operator have as a parameter a lvalue (a variable), that will have
   * at most one candidate_datatype. This means that we do not really need the for() loop here, but we use it
   * anyway as it is the correct way of implementing the fill/narrow algorithm! 
   */
  for (unsigned int i = 0; i < symbol->exp->candidate_datatypes.size(); i++) {
    /* Create a new object of ref_spec_c, as this is the class used as the  */
    /* canonical/base datatype of REF_TO types (see search_base_type_c ...) */ 
    ref_spec_c *ref_spec = new ref_spec_c(symbol->exp->candidate_datatypes[i]);
    add_datatype_to_candidate_list(symbol, ref_spec);
  }
  return NULL;
}
    
void *fill_candidate_datatypes_c::visit(   or_expression_c  *symbol) {return handle_binary_expression  (widen_OR_table,  symbol, symbol->l_exp, symbol->r_exp);}
void *fill_candidate_datatypes_c::visit(   xor_expression_c *symbol) {return handle_binary_expression  (widen_XOR_table, symbol, symbol->l_exp, symbol->r_exp);}
void *fill_candidate_datatypes_c::visit(   and_expression_c *symbol) {return handle_binary_expression  (widen_AND_table, symbol, symbol->l_exp, symbol->r_exp);}

void *fill_candidate_datatypes_c::visit(   equ_expression_c *symbol) {return handle_equality_comparison(widen_CMP_table, symbol, symbol->l_exp, symbol->r_exp);}
void *fill_candidate_datatypes_c::visit(notequ_expression_c *symbol) {return handle_equality_comparison(widen_CMP_table, symbol, symbol->l_exp, symbol->r_exp);}
void *fill_candidate_datatypes_c::visit(    lt_expression_c *symbol) {return handle_binary_expression  (widen_CMP_table, symbol, symbol->l_exp, symbol->r_exp);}
void *fill_candidate_datatypes_c::visit(    gt_expression_c *symbol) {return handle_binary_expression  (widen_CMP_table, symbol, symbol->l_exp, symbol->r_exp);}
void *fill_candidate_datatypes_c::visit(    le_expression_c *symbol) {return handle_binary_expression  (widen_CMP_table, symbol, symbol->l_exp, symbol->r_exp);}
void *fill_candidate_datatypes_c::visit(    ge_expression_c *symbol) {return handle_binary_expression  (widen_CMP_table, symbol, symbol->l_exp, symbol->r_exp);}
 
void *fill_candidate_datatypes_c::visit(   add_expression_c *symbol) {return handle_binary_expression  (widen_ADD_table,  symbol, symbol->l_exp, symbol->r_exp);}
void *fill_candidate_datatypes_c::visit(   sub_expression_c *symbol) {return handle_binary_expression  (widen_SUB_table,  symbol, symbol->l_exp, symbol->r_exp);}
void *fill_candidate_datatypes_c::visit(   mul_expression_c *symbol) {return handle_binary_expression  (widen_MUL_table,  symbol, symbol->l_exp, symbol->r_exp);}
void *fill_candidate_datatypes_c::visit(   div_expression_c *symbol) {return handle_binary_expression  (widen_DIV_table,  symbol, symbol->l_exp, symbol->r_exp);}
void *fill_candidate_datatypes_c::visit(   mod_expression_c *symbol) {return handle_binary_expression  (widen_MOD_table,  symbol, symbol->l_exp, symbol->r_exp);}
void *fill_candidate_datatypes_c::visit( power_expression_c *symbol) {return handle_binary_expression  (widen_EXPT_table, symbol, symbol->l_exp, symbol->r_exp);}


void *fill_candidate_datatypes_c::visit(neg_expression_c *symbol) {
  /* NOTE: The standard defines the syntax for this 'negation' operation, but
   *       does not define the its semantics.
   *
   *       We could be tempted to consider that the semantics of the
   *       'negation' operation are similar/identical to the semantics of the 
   *       SUB expression/operation. This would include assuming that the
   *       possible datatypes for the 'negation' operation is also
   *       the same as those for the SUB expression/operation, namely ANY_MAGNITUDE.
   *
   *       However, this would then mean that the following ST code would be 
   *       syntactically and semantically correct:
   *       VAR uint_var : UINT END_VAR;
   *       uint_var := - (uint_var);
   *
   *       Assuming uint_var is not 0, the standard states that the above code should result in a 
   *       runtime error since the operation will result in an overflow. Since the above operation
   *       is only valid when uint_var=0, it would probably make more sense for the programmer to
   *       use if (uint_var=0) ..., so we will simply assume that the above statement simply
   *       does not make sense in any situation (whether or not uint_var is 0), and therefore
   *       we will not allow it.
   *       (Notice that doing so does not ago against the standard, as the standard does not
   *       explicitly define the semantics of the NEG operator, nor the data types it may accept
   *       as input. We are simply assuming that the NEG operator may not be applied to unsigned
   *       ANY_NUM data types!).
   *
   *       It is much easier for the compiler to detect this at compile time,
   *       and it is probably safer to the resulting code too.
   *
   *       To detect these tyes of errors at compile time, the easisest solution
   *       is to only allow ANY_NUM datatytpes that are signed.
   *        So, that is what we do here!
   *
   * NOTE: The above argument also applies to the neg_integer_c method!
   */
	symbol->exp->accept(*this);
	for (unsigned int i = 0; i < symbol->exp->candidate_datatypes.size(); i++) {
		if (get_datatype_info_c::is_ANY_signed_MAGNITUDE_compatible(symbol->exp->candidate_datatypes[i]))
			add_datatype_to_candidate_list(symbol, symbol->exp->candidate_datatypes[i]);
	}
	if (debug) std::cout << "neg [" << symbol->exp->candidate_datatypes.size() << "] ==> "  << symbol->candidate_datatypes.size() << " result.\n";
	return NULL;
}


void *fill_candidate_datatypes_c::visit(not_expression_c *symbol) {
	symbol->exp->accept(*this);
	for (unsigned int i = 0; i < symbol->exp->candidate_datatypes.size(); i++) {
		if      (get_datatype_info_c::is_ANY_BIT_compatible(symbol->exp->candidate_datatypes[i]))
			add_datatype_to_candidate_list(symbol, symbol->exp->candidate_datatypes[i]);
	}
	if (debug) std::cout << "not [" << symbol->exp->candidate_datatypes.size() << "] ==> "  << symbol->candidate_datatypes.size() << " result.\n";
	return NULL;
}


void *fill_candidate_datatypes_c::visit(function_invocation_c *symbol) {
	if      (NULL != symbol->formal_param_list)        symbol->   formal_param_list->accept(*this);
	else if (NULL != symbol->nonformal_param_list)     symbol->nonformal_param_list->accept(*this);
	// else ERROR;  NOTE-> We support the non-standard feature of POUS with no in, out and inout parameters, so this is no longer an internal error!

	generic_function_call_t fcall_param = {
			  function_name:                symbol->function_name,
			  nonformal_operand_list:       symbol->nonformal_param_list,
			  formal_operand_list:          symbol->formal_param_list,
			  POU_type:                     generic_function_call_t::POU_function,
			  candidate_functions:          symbol->candidate_functions,
			  called_function_declaration:  symbol->called_function_declaration,
			  extensible_param_count:       symbol->extensible_param_count
	};

	handle_function_call(symbol, fcall_param);

	if (debug) std::cout << "function_invocation_c [" << symbol->candidate_datatypes.size() << "] result.\n";
	return NULL;
}



/********************/
/* B 3.2 Statements */
/********************/
// SYM_LIST(statement_list_c)
/* The visitor of the base class search_visitor_c will handle calling each instruction in the list.
 * We do not need to do anything here...
 */
// void *fill_candidate_datatypes_c::visit(statement_list_c *symbol)


/*********************************/
/* B 3.2.1 Assignment Statements */
/*********************************/
void *fill_candidate_datatypes_c::visit(assignment_statement_c *symbol) {
	symbol_c *left_type, *right_type;
	symbol->l_exp->accept(*this);
	symbol->r_exp->accept(*this);
	for (unsigned int i = 0; i < symbol->l_exp->candidate_datatypes.size(); i++) {
		for(unsigned int j = 0; j < symbol->r_exp->candidate_datatypes.size(); j++) {
			left_type = symbol->l_exp->candidate_datatypes[i];
			right_type = symbol->r_exp->candidate_datatypes[j];
			if (get_datatype_info_c::is_type_equal(left_type, right_type))
				add_datatype_to_candidate_list(symbol, left_type);  // NOTE: Must use left_type, as the right_type may be the 'NULL' reference! (see comment in visit(ref_value_null_literal_c)) */
		}
	}
	if (debug) std::cout << ":= [" << symbol->l_exp->candidate_datatypes.size() << "," << symbol->r_exp->candidate_datatypes.size() << "] ==> "  << symbol->candidate_datatypes.size() << " result.\n";
	return NULL;
}

/*****************************************/
/* B 3.2.2 Subprogram Control Statements */
/*****************************************/
void *fill_candidate_datatypes_c::visit(fb_invocation_c *symbol) {
	symbol_c *fb_decl = search_var_instance_decl->get_basetype_decl(symbol->fb_name);
	if (! get_datatype_info_c::is_function_block(fb_decl )) fb_decl = NULL;
	if (NULL == fb_decl) ERROR; /* Although a call to a non-declared FB is a semantic error, this is currently caught by stage 2! */
	
	if (symbol->   formal_param_list != NULL) symbol->formal_param_list->accept(*this);
	if (symbol->nonformal_param_list != NULL) symbol->nonformal_param_list->accept(*this);

	/* The print_datatypes_error_c does not rely on this called_fb_declaration pointer being != NULL to conclude that
	 * we have a datat type incompatibility error, so setting it to the correct fb_decl is actually safe,
	 * as the compiler will never reach the compilation stage!
	 */
	symbol->called_fb_declaration = fb_decl;

	if (debug) std::cout << "FB [] ==> "  << symbol->candidate_datatypes.size() << " result.\n";
	return NULL;
}



/********************************/
/* B 3.2.3 Selection Statements */
/********************************/
void *fill_candidate_datatypes_c::visit(if_statement_c *symbol) {
	symbol->expression->accept(*this);
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	if (NULL != symbol->elseif_statement_list)
		symbol->elseif_statement_list->accept(*this);
	if (NULL != symbol->else_statement_list)
		symbol->else_statement_list->accept(*this);
	return NULL;
}


void *fill_candidate_datatypes_c::visit(elseif_statement_c *symbol) {
	symbol->expression->accept(*this);
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	return NULL;
}

/* CASE expression OF case_element_list ELSE statement_list END_CASE */
// SYM_REF3(case_statement_c, expression, case_element_list, statement_list)
void *fill_candidate_datatypes_c::visit(case_statement_c *symbol) {
	symbol->expression->accept(*this);
	if (NULL != symbol->case_element_list)
		symbol->case_element_list->accept(*this);
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	return NULL;
}


/* helper symbol for case_statement */
// SYM_LIST(case_element_list_c)
/* NOTE: visitor method for case_element_list_c is not required since we inherit from iterator_visitor_c */

/*  case_list ':' statement_list */
// SYM_REF2(case_element_c, case_list, statement_list)
/* NOTE: visitor method for case_element_c is not required since we inherit from iterator_visitor_c */

// SYM_LIST(case_list_c)
/* NOTE: visitor method for case_list_c is not required since we inherit from iterator_visitor_c */

/********************************/
/* B 3.2.4 Iteration Statements */
/********************************/

void *fill_candidate_datatypes_c::visit(for_statement_c *symbol) {
	symbol->control_variable->accept(*this);
	symbol->beg_expression->accept(*this);
	symbol->end_expression->accept(*this);
	if (NULL != symbol->by_expression)
		symbol->by_expression->accept(*this);
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	return NULL;
}


void *fill_candidate_datatypes_c::visit(while_statement_c *symbol) {
	symbol->expression->accept(*this);
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	return NULL;
}


void *fill_candidate_datatypes_c::visit(repeat_statement_c *symbol) {
	symbol->expression->accept(*this);
	if (NULL != symbol->statement_list)
		symbol->statement_list->accept(*this);
	return NULL;
}






