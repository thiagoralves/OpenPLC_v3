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
 * Determine the default initial value of a type declaration.
 *
 * This is part of the 4th stage that generates
 * a c++ source program equivalent to the IL and ST
 * code.
 */

/* Given a type definition declration, determine its default
 * initial value. Note that types based on other types
 * may have to iterate through each type it is based on
 * to determine the initial value.
 * E.g.
 *  TYPE
 *    A_t : INT := 10;
 *    B_t : A_t := 20;
 *    C_t : B_t;
 *    D_t : C_t := 40;
 *  END_TYPE
 * Where the default initial value for C_t is 20!
 */
/* NOTE: The main program only needs one instance of
 *       this class of object. This class
 *       is therefore a singleton.
 */

class type_initial_value_c : public null_visitor_c {

  public:
    static symbol_c *get(symbol_c *type);

  private:
    /* constants for the default values of elementary data types... */
    static ref_value_null_literal_c       *null_literal;
    static real_c                         *real_0;
    static integer_c                      *integer_0, *integer_1;
    static boolean_literal_c              *bool_0;
    static date_literal_c                 *date_literal_0;
    static daytime_c                      *daytime_literal_0;
    static duration_c                     *time_0;
    static date_c                         *date_0;
    static time_of_day_c                  *tod_0;
    static date_and_time_c                *dt_0;
    static single_byte_character_string_c *string_0;
    static double_byte_character_string_c *wstring_0;

  protected:
    type_initial_value_c(void);

  private:
    static type_initial_value_c *_instance;
    static type_initial_value_c *instance(void);
    void *handle_type_spec(symbol_c *base_type_name, symbol_c *type_spec_init);
    void *handle_type_name(symbol_c *type_name);

  private:
    void *visit(                 identifier_c *symbol);
    void *visit(derived_datatype_identifier_c *symbol);
    void *visit(         poutype_identifier_c *symbol);

    /***********************************/
    /* B 1.3.1 - Elementary Data Types */
    /***********************************/
    void *visit(time_type_name_c *symbol);
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
    void *visit(date_type_name_c *symbol);
    void *visit(tod_type_name_c *symbol);
    void *visit(dt_type_name_c *symbol);
    void *visit(byte_type_name_c *symbol);
    void *visit(word_type_name_c *symbol);
    void *visit(dword_type_name_c *symbol);
    void *visit(lword_type_name_c *symbol);
    void *visit(string_type_name_c *symbol);
    void *visit(wstring_type_name_c *symbol);

    void *visit(safetime_type_name_c *symbol);
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
    void *visit(safedate_type_name_c *symbol);
    void *visit(safetod_type_name_c *symbol);
    void *visit(safedt_type_name_c *symbol);
    void *visit(safebyte_type_name_c *symbol);
    void *visit(safeword_type_name_c *symbol);
    void *visit(safedword_type_name_c *symbol);
    void *visit(safelword_type_name_c *symbol);
    void *visit(safestring_type_name_c *symbol);
    void *visit(safewstring_type_name_c *symbol);

    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
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
     * NOTE:
     * (Summary: Contrary to what is expected, the
     *           string_type_declaration_c is not used to store
     *           simple string type declarations that do not include
     *           size limits.
     *           For e.g.:
     *             str1_type: STRING := "hello!"
     *           will be stored in a simple_type_declaration_c
     *           instead of a string_type_declaration_c.
     *           The following:
     *             str2_type: STRING [64] := "hello!"
     *           will be stored in a sring_type_declaration_c
     *
     *           Read on for why this is done...
     * End Summary)
     *
     * According to the spec, the valid construct
     * TYPE new_str_type : STRING := "hello!"; END_TYPE
     * has two possible routes to type_declaration...
     *
     * Route 1:
     * type_declaration: single_element_type_declaration
     * single_element_type_declaration: simple_type_declaration
     * simple_type_declaration: identifier ':' simple_spec_init
     * simple_spec_init: simple_specification ASSIGN constant
     * (shift:  identifier <- 'new_str_type')
     * simple_specification: elementary_type_name
     * elementary_type_name: STRING
     * (shift: elementary_type_name <- STRING)
     * (reduce: simple_specification <- elementary_type_name)
     * (shift: constant <- "hello!")
     * (reduce: simple_spec_init: simple_specification ASSIGN constant)
     * (reduce: ...)
     *
     *
     * Route 2:
     * type_declaration: string_type_declaration
     * string_type_declaration: identifier ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init
     * (shift:  identifier <- 'new_str_type')
     * elementary_string_type_name: STRING
     * (shift: elementary_string_type_name <- STRING)
     * (shift: string_type_declaration_size <-  empty )
     * string_type_declaration_init: ASSIGN character_string
     * (shift: character_string <- "hello!")
     * (reduce: string_type_declaration_init <- ASSIGN character_string)
     * (reduce: string_type_declaration <- identifier ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init )
     * (reduce: type_declaration <- string_type_declaration)
     *
     *
     * At first glance it seems that removing route 1 would make
     * the most sense. Unfortunately the construct 'simple_spec_init'
     * shows up multiple times in other rules, so changing this construct
     * would also mean changing all the rules in which it appears.
     * I (Mario) therefore chose to remove route 2 instead. This means
     * that the above declaration gets stored in a
     * simple_type_declaration_c, and not in a string_type_declaration_c
     * as would be expected!
     */
    /*  string_type_name ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init */
     // SYM_REF4(string_type_declaration_c,	string_type_name,
     //					elementary_string_type_name,
     //					string_type_declaration_size,
     // 				string_type_declaration_init) /* may be == NULL! */
    void *visit(string_type_declaration_c *symbol);
    
    /* REF_TO (non_generic_type_name | function_block_type_name) */
    void *visit(ref_spec_c *symbol);
    /* ref_spec [ ASSIGN ref_initialization ]; */
    void *visit(ref_spec_init_c *symbol);
    /* identifier ':' ref_spec_init */
    void *visit(ref_type_decl_c *symbol);
    
}; // type_initial_value_c



