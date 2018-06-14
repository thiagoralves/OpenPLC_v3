/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2003-2011  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2007-2011  Laurent Bessard and Edouard Tisserant
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
 * Array dimension iterator.
 * Iterate through the dimensions of array specification.
 *
 * This is part of the 4th stage that generates
 * a c++ source program equivalent to the IL and ST
 * code.
 */

/* Given a array_specification_c, iterate through
 * each subrange, returning the symbol of each subrange
 * ...array_dimension_iterator_c
 */




#include "array_dimension_iterator.hh"
#include "../main.hh" // required for ERROR() and ERROR_MSG() macros.


//#define DEBUG
#ifdef DEBUG
#define TRACE(classname) printf("\n____%s____\n",classname);
#else
#define TRACE(classname)
#endif



void* array_dimension_iterator_c::iterate_list(list_c *list) {
  void *res;
  for (int i = 0; i < list->n; i++) {
    res = list->elements[i]->accept(*this);
    if (res != NULL)
        return res;
  }
  return NULL;
}

/* start off at the first case element once again... */
void array_dimension_iterator_c::reset(void) {
  current_array_dimension = NULL;
}


/* initialize the iterator object.
 * We must be given a reference to a array_specification_c that will be analyzed...
 */
array_dimension_iterator_c::array_dimension_iterator_c(symbol_c *symbol) {
  /* do some consistency check... */
  /* NOTE: We comment out the consistency check so the compiler does not bork when it encounters buggy source code.
   *        e.g. Code that handles a non array variable as an array!
   *               VAR  v1, v2: int; END_VAR
   *               v1 := v2[33, 45];
   *       The above error will be caught by the datatype checking algorithms!
   */
  array_spec_init_c    * array_spec_init = dynamic_cast<array_spec_init_c    *>(symbol); 
  if (NULL != array_spec_init)    symbol = array_spec_init->array_specification;
  array_specification_c* array_spec      = dynamic_cast<array_specification_c*>(symbol);
  // if (NULL == array_spec) ERROR;

  /* OK. Now initialize this object... */
  this->array_specification = array_spec; // Set to array_spec and not symbol => will be NULL if not an array_specification_c* !!
  reset();
}



/* Skip to the next subrange. After object creation,
 * the object references on subrange _before_ the first, so
 * this function must be called once to get the object to
 * reference the first subrange...
 *
 * Returns the subrange symbol!
 */
subrange_c *array_dimension_iterator_c::next(void) {
  if (NULL == array_specification) return NULL; /* The source code probably has a bug which will be caught somewhere else! */
  void *res = array_specification->accept(*this);
  if (NULL == res)                 return NULL;

  return current_array_dimension;
}

/********************************/
/* B 1.3.3 - Derived data types */
/********************************/
/*  signed_integer DOTDOT signed_integer */
void *array_dimension_iterator_c::visit(subrange_c *symbol) {
  if (current_array_dimension == symbol) {
	current_array_dimension = NULL;
  }
  else if (current_array_dimension == NULL) {
	current_array_dimension = symbol;
	return symbol;
  }

  /* Not found! */
  return NULL;
}

/* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
void *array_dimension_iterator_c::visit(array_specification_c *symbol) {
  return symbol->array_subrange_list->accept(*this);
}

/* array_subrange_list ',' subrange */
void *array_dimension_iterator_c::visit(array_subrange_list_c *symbol) {
  return iterate_list(symbol);
}

