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


#include "../absyntax/visitor.hh"


class array_dimension_iterator_c : public null_visitor_c {
  private:
    /* a pointer to the array_specification_c currently being analyzed */
    symbol_c *array_specification;
    /* used when called to iterate() for a parameter */
    subrange_c *current_array_dimension;

  private:
    void* iterate_list(list_c *list);

  public:
    /* start off at the first dimension once again... */
    void reset(void);

    /* initialize the iterator object.
     * We must be given a reference to a array_specification_c that will be analyzed...
     */
    array_dimension_iterator_c(symbol_c *symbol);

    /* Skip to the next subrange. After object creation,
     * the object references on subrange _before_ the first, so
     * this function must be called once to get the object to
     * reference the first subrange...
     *
     * Returns the subrange symbol!
     */
    subrange_c *next(void);
    

    private:
    
    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
    /*  signed_integer DOTDOT signed_integer */
    void *visit(subrange_c *symbol);
    
    /* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
    void *visit(array_specification_c *symbol);

    /* array_subrange_list ',' subrange */
    void *visit(array_subrange_list_c *symbol);

}; // function_param_iterator_c







