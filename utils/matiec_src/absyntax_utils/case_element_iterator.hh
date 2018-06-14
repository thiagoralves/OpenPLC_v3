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


#include "../absyntax/visitor.hh"


class case_element_iterator_c : public null_visitor_c {
  public:
    /* A type to specify the type of element.
     */
    typedef enum {
      element_single,
      element_subrange
    } case_element_t ;


  private:
    /* a pointer to the case_list_c currently being analyzed */
    symbol_c *case_list;
    /* used when called to iterate() for a parameter */
    symbol_c *current_case_element;

    /* used to indicate the type of case element on which iterate */
    case_element_t wanted_element_type;

  private:
    void* handle_case_element(symbol_c *case_element);

    void* iterate_list(list_c *list);

  public:
    /* start off at the first case element once again... */
    void reset(void);

    /* initialize the iterator object.
     * We must be given a reference to a case_list_c that will be analyzed...
     */
    case_element_iterator_c(symbol_c *list, case_element_t element_type);

    /* Skip to the next case element of type chosen. After object creation,
     * the object references on case element _before_ the first, so
     * this function must be called once to get the object to
     * reference the first element...
     *
     * Returns the case element's symbol!
     */
    symbol_c *next(void);

    private:
    
    /******************************/
    /* B 1.2.1 - Numeric Literals */
    /******************************/
    void *visit(integer_c *symbol);
    void *visit(neg_integer_c *symbol);

    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
    /*  signed_integer DOTDOT signed_integer */
    void *visit(subrange_c *symbol);

    /* enumerated_type_name '#' identifier */
    void *visit(enumerated_value_c *symbol);

    /********************************/
    /* B 3.2.3 Selection Statements */
    /********************************/
    void *visit(case_list_c *symbol);

    

}; // function_param_iterator_c







