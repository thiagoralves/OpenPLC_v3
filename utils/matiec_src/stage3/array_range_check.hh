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

// #include <vector>
#include "../absyntax_utils/absyntax_utils.hh"
// #include "datatype_functions.hh"



class array_range_check_c: public iterator_visitor_c {

  private:
    search_varfb_instance_type_c *search_varfb_instance_type;
    int error_count;
    int current_display_error_level;

    void check_dimension_count(array_variable_c *symbol);
    void check_bounds(array_variable_c *symbol);

  public:
    array_range_check_c(symbol_c *ignore);
    virtual ~array_range_check_c(void);
    int get_error_count();

    /*************************/
    /* B.1 - Common elements */
    /*************************/
    /**********************/
    /* B.1.3 - Data types */
    /**********************/
    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
    /* NOTE: we may later want to move the following 2 methods to a visitor that will focus on analysing the data type declarations! */
    void *visit(subrange_c *symbol);
    void *visit(array_initial_elements_c *symbol);
  
    /*********************/
    /* B 1.4 - Variables */
    /*********************/
    /*************************************/
    /* B 1.4.2 - Multi-element variables */
    /*************************************/
    void *visit(array_variable_c *symbol);

    /**************************************/
    /* B 1.5 - Program organisation units */
    /**************************************/
    /***********************/
    /* B 1.5.1 - Functions */
    /***********************/
    void *visit(function_declaration_c *symbol);

    /*****************************/
    /* B 1.5.2 - Function blocks */
    /*****************************/
    void *visit(function_block_declaration_c *symbol);

    /**********************/
    /* B 1.5.3 - Programs */
    /**********************/
    void *visit(program_declaration_c *symbol);

}; /* array_range_check_c */







