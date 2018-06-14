/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2009-2011  Mario de Sousa (msousa@fe.up.pt)
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


/* Determine the size, in bits, of the data type.
 * 
 * NOTE: Currently, only elementary data types with well defined sizes (in the standard) are supported.
 *       - derived data types are not supported, and these will return 0
 *       - TIME, DATE, TIME_OF_DAY, and DATE_AND_TIME are not supported, and will return 0
 *       - STRING and WSTRING are not supported, and the standard merely defines bit per character,
 *              and not the maximum number of characters, so these will return 0
 *
 *       We also support the 'Numeric Literals' Data types.
 *       i.e., numeric literals are considerd basic data types
 *       as their data type is undefined (e.g. the datat type of '30'
 *       could be 'INT' or 'SINT' or 'LINT' or 'USINT' or ...
 *
 *       For numeric literals, we return the minimum number of bits
 *       required to store the value.
 *
 * E.g. TYPE new_int_t : INT; END_TYPE;
 *      TYPE new_int2_t : INT = 2; END_TYPE;
 *      TYPE new_subr_t : INT (4..5); END_TYPE;
 *
 *    sizeof(SINT) ->  8
 *    sizeof(INT)  -> 16
 *    sizeof(DINT) -> 32
 *    sizeof(LINT) -> 64
 *
 *    sizeof('1')       ->  1
 *    sizeof('015')     ->  4    # Leading zeros are ignored!
 *    sizeof('0')       ->  1    # This is a special case! Even the value 0 needs at least 1 bit to store!
 *    sizeof('16')      ->  5
 *    sizeof('2#00101') ->  3
 *    sizeof('8#334')   ->  9
 *    sizeof('16#2A')   ->  8
 *
 *    sizeof('7.4')     ->  32   # all real literals return 32 bits, the size of a 'REAL'
 *                               # TODO: study IEC 60559 for the range of values that may be
 *                               #       stored in a REAL (basic single width floating point format)
 *                               #       and in a LREAL (basic double width floating point format)
 *                               #       and see if some real literals need to return 64 instead!
 */

#include "../absyntax/visitor.hh"

class get_sizeof_datatype_c: public null_visitor_c {

  public:
    static int getsize(symbol_c *data_type_symbol);
    ~get_sizeof_datatype_c(void);

  private:
    /* this class is a singleton. So we need a pointer to the single instance... */
    static get_sizeof_datatype_c *singleton;

  private:
#if 0   /* We no longer need the code for handling numeric literals. But keep it around for a little while longer... */
    /*********************/
    /* B 1.2 - Constants */
    /*********************/

    /******************************/
    /* B 1.2.1 - Numeric Literals */
    /******************************/
     /* Numeric literals without any explicit type cast have unknown data type, 
      * so we continue considering them as their own basic data types until
      * they can be resolved (for example, when using '30+x' where 'x' is a LINT variable, the
      * numeric literal '30' must then be considered a LINT so the ADD function may be called
      * with all inputs of the same data type.
      * If 'x' were a SINT, then the '30' would have to be a SINT too!
      */
    void *visit(real_c *symbol);
    void *visit(neg_real_c *symbol);
    void *visit(integer_c *symbol);
    void *visit(neg_integer_c *symbol);
    void *visit(binary_integer_c *symbol);
    void *visit(octal_integer_c *symbol);
    void *visit(hex_integer_c *symbol);
#endif
  /***********************************/
  /* B 1.3.1 - Elementary Data Types */
  /***********************************/
//     void *visit(time_type_name_c *symbol);
    void *visit(bool_type_name_c *symbol);
    void *visit(sint_type_name_c *symbol);
    void *visit(int_type_name_c *symbol);
    void *visit(dint_type_name_c *symbol);
    void *visit(lint_type_name_c *symbol);
    void *visit(usint_type_name_c *symbol);
    void *visit(uint_type_name_c *symbol);
    void *visit(udint_type_name_c *symbol);
    void *visit(ulint_type_name_c *symbol);
    void *visit(real_type_name_c *symbol);
    void *visit(lreal_type_name_c *symbol);
//     void *visit(date_type_name_c *symbol);
//     void *visit(tod_type_name_c *symbol);
//     void *visit(dt_type_name_c *symbol)	;
    void *visit(byte_type_name_c *symbol);
    void *visit(word_type_name_c *symbol);
    void *visit(dword_type_name_c *symbol);
    void *visit(lword_type_name_c *symbol);
//     void *visit(string_type_name_c *symbol);
//     void *visit(wstring_type_name_c *symbol);

    /******************************************************/
    /* Extensions to the base standard as defined in      */
    /* "Safety Software Technical Specification,          */
    /*  Part 1: Concepts and Function Blocks,             */
    /*  Version 1.0 – Official Release"                   */
    /* by PLCopen - Technical Committee 5 - 2006-01-31    */
    /******************************************************/
//     void *visit(safetime_type_name_c *symbol);
    void *visit(safebool_type_name_c *symbol);
    void *visit(safesint_type_name_c *symbol);
    void *visit(safeint_type_name_c *symbol);
    void *visit(safedint_type_name_c *symbol);
    void *visit(safelint_type_name_c *symbol);
    void *visit(safeusint_type_name_c *symbol);
    void *visit(safeuint_type_name_c *symbol);
    void *visit(safeudint_type_name_c *symbol);
    void *visit(safeulint_type_name_c *symbol);
    void *visit(safereal_type_name_c *symbol);
    void *visit(safelreal_type_name_c *symbol);
//     void *visit(safedate_type_name_c *symbol);
//     void *visit(safetod_type_name_c *symbol);
//     void *visit(safedt_type_name_c *symbol)	;
    void *visit(safebyte_type_name_c *symbol);
    void *visit(safeword_type_name_c *symbol);
    void *visit(safedword_type_name_c *symbol);
    void *visit(safelword_type_name_c *symbol);
//     void *visit(safestring_type_name_c *symbol);
//     void *visit(safewstring_type_name_c *symbol);

  /********************************/
  /* B 1.3.3 - Derived data types */
  /********************************/
#if 0
  /*  simple_type_name ':' simple_spec_init */
    void *visit(simple_type_declaration_c *symbol);
  /* simple_specification ASSIGN constant */
    void *visit(simple_spec_init_c *symbol);
  /*  subrange_type_name ':' subrange_spec_init */
    void *visit(subrange_type_declaration_c *symbol);
  /* subrange_specification ASSIGN signed_integer */
    void *visit(subrange_spec_init_c *symbol);
  /*  integer_type_name '(' subrange')' */
    void *visit(subrange_specification_c *symbol);
  /*  signed_integer DOTDOT signed_integer */
    void *visit(subrange_c *symbol);

  /*  enumerated_type_name ':' enumerated_spec_init */
    void *visit(enumerated_type_declaration_c *symbol);
  /* enumerated_specification ASSIGN enumerated_value */
    void *visit(enumerated_spec_init_c *symbol);
  /* helper symbol for enumerated_specification->enumerated_spec_init */
  /* enumerated_value_list ',' enumerated_value */
    void *visit(enumerated_value_list_c *symbol);
  /* enumerated_type_name '#' identifier */
  // SYM_REF2(enumerated_value_c, type, value)
    void *visit(enumerated_value_c *symbol);
  /*  identifier ':' array_spec_init */
    void *visit(array_type_declaration_c *symbol);
  /* array_specification [ASSIGN array_initialization} */
  /* array_initialization may be NULL ! */
    void *visit(array_spec_init_c *symbol);
  /* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
    void *visit(array_specification_c *symbol);
  /* helper symbol for array_specification */
  /* array_subrange_list ',' subrange */
    void *visit(array_subrange_list_c *symbol);
  /* array_initialization:  '[' array_initial_elements_list ']' */
  /* helper symbol for array_initialization */
  /* array_initial_elements_list ',' array_initial_elements */
    void *visit(array_initial_elements_list_c *symbol);
  /* integer '(' [array_initial_element] ')' */
  /* array_initial_element may be NULL ! */
    void *visit(array_initial_elements_c *symbol);
  /*  structure_type_name ':' structure_specification */
      /* NOTE: structure_specification will point to either a
       *       initialized_structure_c
       *       OR A
       *       structure_element_declaration_list_c
       */
    void *visit(structure_type_declaration_c *symbol);
  /* structure_type_name ASSIGN structure_initialization */
  /* structure_initialization may be NULL ! */
    void *visit(initialized_structure_c *symbol);
  /* helper symbol for structure_declaration */
  /* structure_declaration:  STRUCT structure_element_declaration_list END_STRUCT */
  /* structure_element_declaration_list structure_element_declaration ';' */
    void *visit(structure_element_declaration_list_c *symbol);
  /*  structure_element_name ':' *_spec_init */
    void *visit(structure_element_declaration_c *symbol);
  /* helper symbol for structure_initialization */
  /* structure_initialization: '(' structure_element_initialization_list ')' */
  /* structure_element_initialization_list ',' structure_element_initialization */
    void *visit(structure_element_initialization_list_c *symbol);
  /*  structure_element_name ASSIGN value */
    void *visit(structure_element_initialization_c *symbol);
  /*  string_type_name ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init */
  /*
  SYM_REF4(string_type_declaration_c,	string_type_name,
  					elementary_string_type_name,
  					string_type_declaration_size,
  					string_type_declaration_init) // may be == NULL!
  */
    void *visit(string_type_declaration_c *symbol);
#endif
}; // get_sizeof_datatype_c




