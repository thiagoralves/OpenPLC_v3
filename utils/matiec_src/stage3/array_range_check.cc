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


/*
 * Array Range Checking:
 *   - Check whether array subscript values fall within the allowed range.
 *     Note that for the checking of subscript values to work correctly, we need to have constant folding working too:
 *     array_var[8 + 99] can not be checked without constant folding.
 */


#include "array_range_check.hh"
#include <limits>  // required for std::numeric_limits<XXX>


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


#define GET_CVALUE(dtype, symbol)             ((symbol)->const_value._##dtype.get())
#define VALID_CVALUE(dtype, symbol)           ((symbol)->const_value._##dtype.is_valid())

/*  The cmp_unsigned_signed function compares two numbers u and s.
 *  It returns an integer indicating the relationship between the numbers:
 *  - A zero value indicates that both numbers are equal.
 *  - A value greater than zero indicates that numbers does not match and
 *    first has a greater value.
 *  - A value less than zero indicates that numbers does not match and
 *    first has a lesser value.
 */
static inline int cmp_unsigned_signed(const uint64_t u, const int64_t s) {
  const uint64_t INT64_MAX_uvar = INT64_MAX;
  if (u <= INT64_MAX_uvar)
    return ((int64_t)u - s);
  return -1;
}

array_range_check_c::array_range_check_c(symbol_c *ignore) {
	error_count = 0;
	current_display_error_level = 0;
	search_varfb_instance_type = NULL;
}



array_range_check_c::~array_range_check_c(void) {
}



int array_range_check_c::get_error_count() {
	return error_count;
}



void array_range_check_c::check_dimension_count(array_variable_c *symbol) {
	int dimension_count;
	symbol_c *var_decl;

	var_decl = search_varfb_instance_type->get_basetype_decl(symbol->subscripted_variable);
	array_dimension_iterator_c array_dimension_iterator(var_decl);
	for (dimension_count = 0; NULL != array_dimension_iterator.next(); dimension_count++);
	if (dimension_count != ((list_c *)symbol->subscript_list)->n)
		STAGE3_ERROR(0, symbol, symbol, "Number of subscripts/indexes does not match the number of subscripts/indexes in the array's declaration (array has %d indexes)", dimension_count);
}



void array_range_check_c::check_bounds(array_variable_c *symbol) {
  list_c *l; /* the subscript_list */
  symbol_c *var_decl;

  l = (list_c *)symbol->subscript_list;
  var_decl = search_varfb_instance_type->get_basetype_decl(symbol->subscripted_variable);
  array_dimension_iterator_c array_dimension_iterator(var_decl);
  for (int i =  0; i < l->n; i++) {
    subrange_c *dimension = array_dimension_iterator.next();
    /* mismatch between number of indexes/subscripts. This error will be caught in check_dimension_count() so we ignore it. */
    if (NULL == dimension) 
      return;

    /* Check lower limit */
    if ( VALID_CVALUE( int64, l->elements[i]) && VALID_CVALUE( int64, dimension->lower_limit))
      if ( GET_CVALUE( int64, l->elements[i]) < GET_CVALUE( int64, dimension->lower_limit) )
      {STAGE3_ERROR(0, symbol, symbol, "Array access out of bounds (using constant value of %"PRId64", should be >= %"PRId64").", GET_CVALUE( int64, l->elements[i]), GET_CVALUE( int64, dimension->lower_limit)); continue;}

    if ( VALID_CVALUE( int64, l->elements[i]) && VALID_CVALUE(uint64, dimension->lower_limit))
      if ( cmp_unsigned_signed( GET_CVALUE(uint64, dimension->lower_limit), GET_CVALUE( int64, l->elements[i])) > 0 )
      {STAGE3_ERROR(0, symbol, symbol, "Array access out of bounds (using constant value of %"PRId64", should be >= %"PRIu64").", GET_CVALUE( int64, l->elements[i]), GET_CVALUE(uint64, dimension->lower_limit)); continue;}

    if ( VALID_CVALUE(uint64, l->elements[i]) && VALID_CVALUE(uint64, dimension->lower_limit))
      if ( GET_CVALUE(uint64, l->elements[i])   <  GET_CVALUE(uint64, dimension->lower_limit))
      {STAGE3_ERROR(0, symbol, symbol, "Array access out of bounds (using constant value of %"PRIu64", should be >= %"PRIu64").", GET_CVALUE(uint64, l->elements[i]), GET_CVALUE(uint64, dimension->lower_limit)); continue;}

    if ( VALID_CVALUE(uint64, l->elements[i]) && VALID_CVALUE( int64, dimension->lower_limit))
      if ( cmp_unsigned_signed(GET_CVALUE(uint64, l->elements[i]), GET_CVALUE( int64, dimension->lower_limit)) < 0 )
      {STAGE3_ERROR(0, symbol, symbol, "Array access out of bounds (using constant value of %"PRIu64", should be >= %"PRId64").", GET_CVALUE(uint64, l->elements[i]), GET_CVALUE( int64, dimension->lower_limit)); continue;}

    /* Repeat the same check, now for upper limit */
    if ( VALID_CVALUE( int64, l->elements[i]) && VALID_CVALUE( int64, dimension->upper_limit))
      if ( GET_CVALUE( int64, l->elements[i])   >  GET_CVALUE( int64, dimension->upper_limit))
      {STAGE3_ERROR(0, symbol, symbol, "Array access out of bounds (using constant value of %"PRId64", should be <= %"PRId64").", GET_CVALUE( int64, l->elements[i]), GET_CVALUE( int64, dimension->upper_limit)); continue;}

    if ( VALID_CVALUE( int64, l->elements[i]) && VALID_CVALUE(uint64, dimension->upper_limit))
      if ( cmp_unsigned_signed( GET_CVALUE(uint64, dimension->upper_limit), GET_CVALUE( int64, l->elements[i])) < 0 )
      {STAGE3_ERROR(0, symbol, symbol, "Array access out of bounds (using constant value of %"PRId64", should be <= %"PRIu64").", GET_CVALUE( int64, l->elements[i]), GET_CVALUE(uint64, dimension->upper_limit)); continue;}

    if ( VALID_CVALUE(uint64, l->elements[i]) && VALID_CVALUE(uint64, dimension->upper_limit))
      if ( GET_CVALUE(uint64, l->elements[i])   >  GET_CVALUE(uint64, dimension->upper_limit))
      {STAGE3_ERROR(0, symbol, symbol, "Array access out of bounds (using constant value of %"PRIu64", should be <= %"PRIu64").", GET_CVALUE(uint64, l->elements[i]), GET_CVALUE(uint64, dimension->upper_limit)); continue;}
      
    if ( VALID_CVALUE(uint64, l->elements[i]) && VALID_CVALUE( int64, dimension->upper_limit))
      if ( cmp_unsigned_signed(GET_CVALUE(uint64, l->elements[i]), GET_CVALUE( int64, dimension->upper_limit)) > 0 )
      {STAGE3_ERROR(0, symbol, symbol, "Array access out of bounds (using constant value of %"PRIu64", should be <= %"PRId64").", GET_CVALUE(uint64, l->elements[i]), GET_CVALUE( int64, dimension->upper_limit)); continue;}
      
  }
}









/*************************/
/* B.1 - Common elements */
/*************************/
/**********************/
/* B.1.3 - Data types */
/**********************/
/********************************/
/* B 1.3.3 - Derived data types */
/********************************/

/*  signed_integer DOTDOT signed_integer */
/* dimension will be filled in during stage 3 (array_range_check_c) with the number of elements in this subrange */
// SYM_REF2(subrange_c, lower_limit, upper_limit, unsigned long long int dimension)
void *array_range_check_c::visit(subrange_c *symbol) {
	unsigned long long int dimension = 0; // we use unsigned long long instead of uint64_t since it might just happen to be larger than uint64_t in the platform used for compiling this code!!

	/* Determine the size of the array... */
	if        (VALID_CVALUE( int64, symbol->upper_limit) && VALID_CVALUE( int64, symbol->lower_limit)) {  
		// do the sums in such a way that no overflow is possible... even during intermediate steps used by compiler!
		// remember that the result (dimension) is unsigned, while the operands are signed!!
		// dimension = GET_CVALUE( int64, symbol->upper_limit) - VALID_CVALUE( int64, symbol->lower_limit);
		if  (GET_CVALUE( int64, symbol->lower_limit)  >   GET_CVALUE( int64, symbol->upper_limit)) {
			STAGE3_ERROR(0, symbol, symbol, "Subrange has lower limit (%"PRId64") larger than upper limit (%"PRId64").", GET_CVALUE( int64, symbol->lower_limit), GET_CVALUE( int64, symbol->upper_limit));
			dimension = std::numeric_limits< unsigned long long int >::max() - 1; // -1 because it will be incremented at the end of this function!! 
		} else if (GET_CVALUE( int64, symbol->lower_limit) >= 0) {
			dimension = GET_CVALUE( int64, symbol->upper_limit) - GET_CVALUE( int64, symbol->lower_limit);
		} else {
			dimension  = -GET_CVALUE( int64, symbol->lower_limit);
			dimension +=  GET_CVALUE( int64, symbol->upper_limit);     
		}
	} else if (VALID_CVALUE(uint64, symbol->upper_limit) && VALID_CVALUE(uint64, symbol->lower_limit)) {
		if  (GET_CVALUE(uint64, symbol->lower_limit)  >   GET_CVALUE(uint64, symbol->upper_limit)) {
			STAGE3_ERROR(0, symbol, symbol, "Subrange has lower limit (%"PRIu64") larger than upper limit (%"PRIu64").", GET_CVALUE(uint64, symbol->lower_limit), GET_CVALUE(uint64, symbol->upper_limit));
			dimension = std::numeric_limits< unsigned long long int >::max() - 1; // -1 because it will be incremented at the end of this function!! 
		} else
			dimension = GET_CVALUE(uint64, symbol->upper_limit) - GET_CVALUE(uint64, symbol->lower_limit); 
	} else if (VALID_CVALUE(uint64, symbol->upper_limit) && VALID_CVALUE( int64, symbol->lower_limit)) {
		// No need to check whether lower_limit > upper_limit, as we only reach this point if lower_limit < 0 (and upper_limit must be >= 0!)
		if (GET_CVALUE( int64, symbol->lower_limit) >= 0) {
			dimension = GET_CVALUE(uint64, symbol->upper_limit) - GET_CVALUE( int64, symbol->lower_limit);
		} else {
			unsigned long long int lower_ull;
			lower_ull  = -GET_CVALUE( int64, symbol->lower_limit);
			dimension  =  GET_CVALUE(uint64, symbol->upper_limit) + lower_ull;     
			if (dimension < lower_ull)
				STAGE3_ERROR(0, symbol, symbol, "Number of elements in array subrange exceeds maximum number of elements (%llu).", std::numeric_limits< unsigned long long int >::max());
		}
	} else if (!VALID_CVALUE(uint64, symbol->upper_limit) && !VALID_CVALUE( int64, symbol->upper_limit)) {
		STAGE3_ERROR(0, symbol->upper_limit, symbol->upper_limit, "Subrange upper limit is not a constant value.");
		// set dimension to largest possible value so we do not get any further related error messages.
		dimension = std::numeric_limits< unsigned long long int >::max() - 1; // -1 because it will be incremented at the end of this function!! 
	} else if (!VALID_CVALUE(uint64, symbol->lower_limit) && !VALID_CVALUE( int64, symbol->lower_limit)) {
		STAGE3_ERROR(0, symbol->lower_limit, symbol->lower_limit, "Subrange lower limit is not a constant value.");
		// set dimension to largest possible value so we do not get any further related error messages.
		dimension = std::numeric_limits< unsigned long long int >::max() - 1; // -1 because it will be incremented at the end of this function!! 
		/* NOTE: Note that both the "subrange *** limit is not a constant value" error messages are only necessary due to an extension
		 *       that matiec supports by allowing non-literals in subrange declarations (currently only subranges in array declarations).
		 *         e.g.:  array_var: ARRAY [1..max] of INT;   <--- illegal according to IEC 61131-1 due to the 'max' non literal
		 *       Matiec will allow the above syntax, as long as the 'max' variable can be determined to be constant throughout
		 *       the program execution at runtime (and not only constant when program initiates) - for example, a VAR CONSTANT
		 *       variable.
		 *       These two checks will verify if we were able to fold the variable into a constant value, or not.
		 */
	} else {ERROR;}

	/* correct value for dimension is actually ---> dimension = upper_limit - lower_limit + 1
	 * Up to now, we have only determined dimension = upper_limit - lower_limit
	 * We must first check whether this last increment will cause an overflow!
	 */
	if (dimension == std::numeric_limits< unsigned long long int >::max())
		STAGE3_ERROR(0, symbol, symbol, "Number of elements in array subrange exceeds maximum number of elements (%llu).", std::numeric_limits< unsigned long long int >::max());
	
	/* correct value for dimension is actually ---> dimension = upper_limit - lower_limit + 1 */
	dimension++; 
	
	symbol->dimension = dimension;
	return NULL;
}





/* integer '(' [array_initial_element] ')' */
/* array_initial_element may be NULL ! */
// SYM_REF2(array_initial_elements_c, integer, array_initial_element)
void *array_range_check_c::visit(array_initial_elements_c *symbol) {
	if (VALID_CVALUE( int64, symbol->integer) && (GET_CVALUE( int64, symbol->integer) < 0)) 
		ERROR; /* the IEC 61131-3 syntax guarantees that this value will never be negative! */

	/* TODO: check that the total number of 'initial values' does not exceed the size of the array! */

	return NULL;
}









/*********************/
/* B 1.4 - Variables */
/*********************/
/*************************************/
/* B 1.4.2 - Multi-element variables */
/*************************************/
void *array_range_check_c::visit(array_variable_c *symbol) {
	check_dimension_count(symbol);
	check_bounds(symbol);
	return NULL;
}


/**************************************/
/* B 1.5 - Program organisation units */
/**************************************/
/***********************/
/* B 1.5.1 - Functions */
/***********************/
// SYM_REF4(function_declaration_c, derived_function_name, type_name, var_declarations_list, function_body)
void *array_range_check_c::visit(function_declaration_c *symbol) {
	symbol->var_declarations_list->accept(*this); // required for visiting subrange_c
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
	// search_var_instance_decl = new search_var_instance_decl_c(symbol);
	symbol->function_body->accept(*this);
	delete search_varfb_instance_type;
	// delete search_var_instance_decl;
	search_varfb_instance_type = NULL;
	// search_var_instance_decl = NULL;
	return NULL;
}

/*****************************/
/* B 1.5.2 - Function blocks */
/*****************************/
// SYM_REF3(function_block_declaration_c, fblock_name, var_declarations, fblock_body)
void *array_range_check_c::visit(function_block_declaration_c *symbol) {
	symbol->var_declarations->accept(*this); // required for visiting subrange_c
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
	// search_var_instance_decl = new search_var_instance_decl_c(symbol);
	symbol->fblock_body->accept(*this);
	delete search_varfb_instance_type;
	// delete search_var_instance_decl;
	search_varfb_instance_type = NULL;
	// search_var_instance_decl = NULL;
	return NULL;
}

/**********************/
/* B 1.5.3 - Programs */
/**********************/
// SYM_REF3(program_declaration_c, program_type_name, var_declarations, function_block_body)
void *array_range_check_c::visit(program_declaration_c *symbol) {
	symbol->var_declarations->accept(*this); // required for visiting subrange_c
	search_varfb_instance_type = new search_varfb_instance_type_c(symbol);
	// search_var_instance_decl = new search_var_instance_decl_c(symbol);
	symbol->function_block_body->accept(*this);
	delete search_varfb_instance_type;
	// delete search_var_instance_decl;
	search_varfb_instance_type = NULL;
	// search_var_instance_decl = NULL;
	return NULL;
}



