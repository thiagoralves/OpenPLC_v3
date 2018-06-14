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



/* Returns the function block type declaration
 * of a specific function block instance.
 */



/* Returns the type name of a specific function block
 * instance. This class will search the variable
 * declarations inside the scope given to it
 * searching for the declaration of the function
 * block instance.
 *
 * The class constructor must be given the search scope
 * (function, function block or program within which
 * the function block instance was declared).
 *
 * This class will search the tree from the root given to the
 * constructor. Another option would be to build a symbol table,
 * and search that instead. Building the symbol table would be done
 * while visiting the variable declaration objects in the parse
 * tree. Unfortuantely, generate_c_c does not visit these
 * objects, delegating it to another class. This means that
 * we would need another specialised class just to build the
 * symbol table. We might just as well have a specialised class
 * that searches the tree itself for the relevant info. This
 * class is exactly that...!
 */


class search_fb_instance_decl_c: public search_visitor_c {

  private:
    symbol_c *search_scope;

    symbol_c *search_name;
    symbol_c *current_fb_type_name;

  public:
    search_fb_instance_decl_c(symbol_c *search_scope);
    symbol_c *get_type_name(symbol_c *fb_instance_name);

  public:
  /***************************/
  /* B 0 - Programming Model */
  /***************************/
    void *visit(library_c *symbol);

  /******************************************/
  /* B 1.4.3 - Declaration & Initialisation */
  /******************************************/
  
  /* name_list ':' function_block_type_name ASSIGN structure_initialization */
  /* structure_initialization -> may be NULL ! */
    void *visit(fb_name_decl_c *symbol);
  /* name_list ',' fb_name */
    void *visit(fb_name_list_c *symbol);
  /*  global_var_name ':' (simple_specification|subrange_specification|enumerated_specification|array_specification|prev_declared_structure_type_name|function_block_type_name */
    void *visit(external_declaration_c *symbol);


  /**************************************/
  /* B.1.5 - Program organization units */
  /**************************************/
  /***********************/
  /* B 1.5.1 - Functions */
  /***********************/
    void *visit(function_declaration_c *symbol);

  /*****************************/
  /* B 1.5.2 - Function Blocks */
  /*****************************/
    void *visit(function_block_declaration_c *symbol);

  /**********************/
  /* B 1.5.3 - Programs */
  /**********************/
    void *visit(program_declaration_c *symbol);
}; // search_fb_instance_decl_c




