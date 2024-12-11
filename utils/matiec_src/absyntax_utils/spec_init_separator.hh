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
 * Separation of type specification and default value constructs
 * (for e.g. simple_spec_init_c), into a type specificiation part,
 * and a default value part.
 */

#include "../absyntax/absyntax.hh"
#include "../absyntax/visitor.hh"

class spec_init_sperator_c: public null_visitor_c {

  private:
    /* this is a singleton class... */
    static spec_init_sperator_c *class_instance;
    static spec_init_sperator_c *get_class_instance(void);

  private:
    typedef enum {search_spec, search_init} search_what_t;
    static search_what_t search_what;

  public:
    /* the only two public functions... */
    static symbol_c *get_spec(symbol_c *spec_init);

    static symbol_c *get_init(symbol_c *spec_init);


  private:


  /*******************************************/
  /* B 1.1 - Letters, digits and identifiers */
  /*******************************************/
  // SYM_TOKEN(identifier_c)
  void *visit(                 identifier_c *symbol);
  void *visit(derived_datatype_identifier_c *symbol);
  void *visit(         poutype_identifier_c *symbol);


  /********************************/
  /* B 1.3.3 - Derived data types */
  /********************************/

  /* simple_specification ASSIGN constant */
  void *visit(simple_spec_init_c *symbol);
  
  /* subrange_specification ASSIGN signed_integer */
  void *visit(subrange_spec_init_c *symbol);
  
  /*  integer_type_name '(' subrange')' */
  void *visit(subrange_specification_c *symbol);
  
  /* array_specification [ASSIGN array_initialization} */
  /* array_initialization may be NULL ! */
  void *visit(array_spec_init_c *symbol);
  
  /* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
  void *visit(array_specification_c *symbol);

  /* enumerated_specification ASSIGN enumerated_value */
  void *visit(enumerated_spec_init_c *symbol);
  
  /* structure_type_name ASSIGN structure_initialization */
  /* structure_initialization may be NULL ! */
  //SYM_REF2(initialized_structure_c, structure_type_name, structure_initialization)
  void *visit(initialized_structure_c *symbol);
  
  /*  function_block_type_name ASSIGN structure_initialization */
  /* structure_initialization -> may be NULL ! */
  //SYM_REF2(fb_spec_init_c, function_block_type_name, structure_initialization)
  void *visit(fb_spec_init_c *symbol);

  /* REF_TO (non_generic_type_name | function_block_type_name) */
  // SYM_REF1(ref_spec_c, type_name)
  void *visit(ref_spec_c *symbol);

  /* ref_spec [ ASSIGN ref_initialization ] */ 
  /* NOTE: ref_initialization may be NULL!! */
  // SYM_REF2(ref_spec_init_c, ref_spec, ref_initialization)
  void *visit(ref_spec_init_c *symbol);
  
  /******************************************/
  /* B 1.4.3 - Declaration & Initialisation */
  /******************************************/
  
  /* STRING '[' integer ']' 
   * STRING ASSIGN single_byte_character_string
   * STRING '[' integer ']' ASSIGN single_byte_character_string
   */
  void *visit(single_byte_string_spec_c *symbol);

  /* WSTRING '[' integer ']' 
   * WSTRING ASSIGN double_byte_character_string
   * WSTRING '[' integer ']' ASSIGN double_byte_character_string
   */
  void *visit(double_byte_string_spec_c *symbol);
    
};   /* class spec_init_sperator_c */
