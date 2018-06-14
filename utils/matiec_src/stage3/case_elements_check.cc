/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2015  Mario de Sousa (msousa@fe.up.pt)
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
 * Case Options Checking:
 *   - Check whether the options in a case statement are repeated, either directly, or in a range.
 *       For example:
 *         case var of
 *           1: ...   <- OK
 *           2: ...   <- OK
 *           1: ...   <- OK (not an error), but produce a warning due to repeated '1'!
 *           0..8: ...<- OK (not an error), but produce a warning cue to repeated '1' and '2'!
 */


#include "case_elements_check.hh"


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




case_elements_check_c::case_elements_check_c(symbol_c *ignore) {
  warning_found = false;
  error_count = 0;
  current_display_error_level = 0;
}



case_elements_check_c::~case_elements_check_c(void) {
}



int case_elements_check_c::get_error_count() {
  return error_count;
}




/* compare two integer constants, and determins if s1 < s2 */
static bool less_than(symbol_c *s1, symbol_c *s2) {
  if (   (VALID_CVALUE( int64, s1))
      && (VALID_CVALUE( int64, s2))
      && (  GET_CVALUE( int64, s1) < GET_CVALUE( int64, s2)))
    return true;
  
  if (   (VALID_CVALUE(uint64, s1))
      && (VALID_CVALUE(uint64, s2))
      && (  GET_CVALUE(uint64, s1) < GET_CVALUE(uint64, s2)))
    return true;
  
  if (   (VALID_CVALUE( int64, s1))
      && (VALID_CVALUE(uint64, s2))
      && (  GET_CVALUE( int64, s1) < 0))
    return true;
  
  return false;
}



void case_elements_check_c::check_subr_subr(symbol_c *s1, symbol_c *s2) {
  subrange_c *sub1 = dynamic_cast<subrange_c *>(s1);
  subrange_c *sub2 = dynamic_cast<subrange_c *>(s2);
  
  if ((NULL == sub1) || (NULL == sub2)) return;
  symbol_c *l1 = sub1->lower_limit;
  symbol_c *u1 = sub1->upper_limit;
  symbol_c *l2 = sub2->lower_limit;
  symbol_c *u2 = sub2->upper_limit;
  
  if (less_than(u1, l2))  return; // no overlap!
  if (less_than(u2, l1))  return; // no overlap!

  if (   (VALID_CVALUE( int64, l1) || (VALID_CVALUE(uint64, l1)))
      && (VALID_CVALUE( int64, l2) || (VALID_CVALUE(uint64, l2)))
      && (VALID_CVALUE( int64, u1) || (VALID_CVALUE(uint64, u1)))
      && (VALID_CVALUE( int64, u2) || (VALID_CVALUE(uint64, u2))))
    STAGE3_WARNING(s1, s2, "Elements in CASE options have overlapping ranges.");
}




void case_elements_check_c::check_subr_symb(symbol_c *s1, symbol_c *s2) {
  subrange_c *subr = NULL;
  symbol_c   *symb = NULL;
  if ((subr = dynamic_cast<subrange_c *>(s1)) != NULL) {symb = s2;}
  if ((subr = dynamic_cast<subrange_c *>(s2)) != NULL) {symb = s1;}
  
  if ((NULL == subr) || (NULL == symb)) return;
  symbol_c   *lowl = subr->lower_limit;
  symbol_c   *uppl = subr->upper_limit;
  
  if (   (VALID_CVALUE(int64, symb))
      && (VALID_CVALUE(int64, lowl))
      && (VALID_CVALUE(int64, uppl))
      && (  GET_CVALUE(int64, symb) >= GET_CVALUE(int64, lowl))
      && (  GET_CVALUE(int64, symb) <= GET_CVALUE(int64, uppl)))
    {STAGE3_WARNING(s1, s2, "Element in CASE option falls within range of another element."); return;}
    
  if (   (VALID_CVALUE(uint64, symb))
      && (VALID_CVALUE( int64, lowl))
      && (VALID_CVALUE(uint64, uppl))
      && (  GET_CVALUE( int64, lowl) < 0)
      && (  GET_CVALUE(uint64, symb) <= GET_CVALUE(uint64, uppl)))
    {STAGE3_WARNING(s1, s2, "Element in CASE option falls within range of another element."); return;}

  if (   (VALID_CVALUE(uint64, symb))
      && (VALID_CVALUE(uint64, lowl))
      && (VALID_CVALUE(uint64, uppl))
      && (  GET_CVALUE(uint64, symb) >= GET_CVALUE(uint64, lowl))
      && (  GET_CVALUE(uint64, symb) <= GET_CVALUE(uint64, uppl)))
    {STAGE3_WARNING(s1, s2, "Element in CASE option falls within range of another element."); return;}
}




#include <typeinfo>
void case_elements_check_c::check_symb_symb(symbol_c *s1, symbol_c *s2) {
  if (   (dynamic_cast<subrange_c *>(s1) != NULL)
      || (dynamic_cast<subrange_c *>(s2) != NULL)) 
    return; // only run this test if neither s1 nor s2 are subranges!
  
  if (   (s1->const_value.is_const() && s2->const_value.is_const() && (s1->const_value == s2->const_value))  // if const, then compare const values (using overloaded '==' operator!)
      || (compare_identifiers(s1, s2) == 0))  // if token_c, compare tokens! (compare_identifiers() returns 0 when equal tokens!, -1 when either is not token_c)
    STAGE3_WARNING(s1, s2, "Duplicate element found in CASE options.");
}









/***************************************/
/* B.3 - Language ST (Structured Text) */
/***************************************/
/********************/
/* B 3.2 Statements */
/********************/
/********************************/
/* B 3.2.3 Selection Statements */
/********************************/
/* CASE expression OF case_element_list ELSE statement_list END_CASE */
// SYM_REF3(case_statement_c, expression, case_element_list, statement_list)
void *case_elements_check_c::visit(case_statement_c *symbol) {
  std::vector<symbol_c *> case_elements_list_local = case_elements_list; // Required when source code contains CASE inside another CASE !

  case_elements_list.clear();
  symbol->case_element_list->accept(*this); // will fill up the case_elements_list with all the elements in the case!
  
  // OK, now check whether we have any overlappings...
  std::vector<symbol_c *>::iterator s1 = case_elements_list.begin();
  for (  ; s1 != case_elements_list.end(); s1++) {
    std::vector<symbol_c *>::iterator s2 = s1;
    s2++; // do not compare the value with itself!
    for (; s2 != case_elements_list.end(); s2++) {
      // Check for overlapping elements
      check_subr_subr(*s1, *s2);
      check_subr_symb(*s1, *s2);
      check_symb_symb(*s2, *s1);
    }
  }
  
  case_elements_list = case_elements_list_local;
  return NULL;
}

/* helper symbol for case_statement */
// SYM_LIST(case_element_list_c)
// void *case_elements_check_c::visit(case_element_list_c *symbol) // not needed! We inherit from iterator_visitor_c

/*  case_list ':' statement_list */
// SYM_REF2(case_element_c, case_list, statement_list)
// void *case_elements_check_c::visit(case_element_c *symbol) // not needed! We inherit from iterator_visitor_c


// SYM_LIST(case_list_c)
void *case_elements_check_c::visit(case_list_c *symbol) {
  for (int i = 0; i < symbol->n; i++)
    case_elements_list.push_back(symbol->elements[i]);
  return NULL;
}


