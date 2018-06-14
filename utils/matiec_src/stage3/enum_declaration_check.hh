/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2012  Mario de Sousa (msousa@fe.up.pt)
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


#include "../absyntax_utils/absyntax_utils.hh"

class populate_enumvalue_symtable_c;

class enum_declaration_check_c : public iterator_visitor_c {
  private:
    int error_count;
    int current_display_error_level;
    populate_enumvalue_symtable_c *populate_enumvalue_symtable;
    symbol_c::enumvalue_symtable_t *global_enumvalue_symtable;
    
  public:
     enum_declaration_check_c(symbol_c *ignore);
    ~enum_declaration_check_c(void);
    int get_error_count();

    
    /***************************/
    /* B 0 - Programming Model */
    /***************************/
    void *visit(library_c *symbol);

    /**********************/
    /* B.1.3 - Data types */
    /**********************/
    void *visit(data_type_declaration_c *symbol);
  
    /***********************/
    /* B 1.5.1 - Functions */
    /***********************/
    void *visit(function_declaration_c *symbol);
   
    /*****************************/
    /* B 1.5.2 - Function Blocks */
    /*****************************/
    void *visit(function_block_declaration_c *symbol);

    /******************************************/
    /* B 1.5.3 - Declaration & Initialisation */
    /******************************************/
    void *visit(program_declaration_c *symbol);

    /********************************/
    /* B 1.7 Configuration elements */
    /********************************/
    void *visit(configuration_declaration_c *symbol);
    void *visit(resource_declaration_c *symbol);
    void *visit(single_resource_declaration_c *symbol);    
};
