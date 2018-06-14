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
 * Case element iterator.
 * Iterate through the elements of a case list.
 *
 * This is part of the 4th stage that generates
 * a c++ source program equivalent to the IL and ST
 * code.
 */

/* Given a case_list_c and a type of element, iterate through
 * each element, returning the symbol of each element if from
 * the good type...case_element_iterator_c
 */




#include "case_element_iterator.hh"
#include "../main.hh" // required for ERROR() and ERROR_MSG() macros.


//#define DEBUG
#ifdef DEBUG
#define TRACE(classname) printf("\n____%s____\n",classname);
#else
#define TRACE(classname)
#endif




void* case_element_iterator_c::handle_case_element(symbol_c *case_element) {
  if (current_case_element == case_element) {
    current_case_element = NULL;
  }
  else if (current_case_element == NULL) {
	current_case_element = case_element;
	return case_element;
  }

  /* Not found! */
  return NULL;
}

void* case_element_iterator_c::iterate_list(list_c *list) {
  void *res;
  for (int i = 0; i < list->n; i++) {
    res = list->elements[i]->accept(*this);
    if (res != NULL)
        return res;
  }
  return NULL;
}

/* start off at the first case element once again... */
void case_element_iterator_c::reset(void) {
  current_case_element = NULL;
}


/* initialize the iterator object.
 * We must be given a reference to a case_list_c that will be analyzed...
 */
case_element_iterator_c::case_element_iterator_c(symbol_c *list, case_element_t element_type) {
  /* do some consistency check... */
  case_list_c* case_list = dynamic_cast<case_list_c*>(list);

  if (NULL == case_list) ERROR;

  /* OK. Now initialize this object... */
  this->case_list = list;
  this->wanted_element_type = element_type;
  reset();
}



/* Skip to the next case element of type chosen. After object creation,
 * the object references on case element _before_ the first, so
 * this function must be called once to get the object to
 * reference the first element...
 *
 * Returns the case element's symbol!
 */
symbol_c *case_element_iterator_c::next(void) {
  void *res = case_list->accept(*this);
  if (res == NULL) 
    return NULL;

  return current_case_element;
}

/******************************/
/* B 1.2.1 - Numeric Literals */
/******************************/
void *case_element_iterator_c::visit(integer_c *symbol) {
  switch (wanted_element_type) {
	case element_single:
	  return handle_case_element(symbol);
	  break;
	default:
	  break;
  }
  return NULL;
}

void *case_element_iterator_c::visit(neg_integer_c *symbol) {
  switch (wanted_element_type) {
	case element_single:
	  return handle_case_element(symbol);
	  break;
	default:
	  break;
  }
  return NULL;
}

/********************************/
/* B 1.3.3 - Derived data types */
/********************************/
/*  signed_integer DOTDOT signed_integer */
void *case_element_iterator_c::visit(subrange_c *symbol) {
  switch (wanted_element_type) {
    case element_subrange:
      return handle_case_element(symbol);
      break;
    default:
      break;
  }
  return NULL;
}

/* enumerated_type_name '#' identifier */
void *case_element_iterator_c::visit(enumerated_value_c *symbol) {
  switch (wanted_element_type) {
    case element_single:
      return handle_case_element(symbol);
      break;
    default:
      break;
  }
  return NULL;
}

/********************************/
/* B 3.2.3 Selection Statements */
/********************************/
void *case_element_iterator_c::visit(case_list_c *symbol) {
  return iterate_list(symbol);
}

