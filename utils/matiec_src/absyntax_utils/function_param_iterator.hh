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
 * Function parameter iterator.
 * Iterate through the in/out parameters of a function declaration.
 * Function blocks are also suported.
 *
 * This is part of the 4th stage that generates
 * a c++ source program equivalent to the IL and ST
 * code.
 */

/* Given a function_declaration_c, iterate through each
 * function in/out/inout parameter, returning the name
 * of each parameter...function_param_iterator_c
 */


#include "../absyntax/visitor.hh"


class function_param_iterator_c : public null_visitor_c {
  public:
    /* A type to specify the type of parameter.
     * VAR_INPUT => direction_in
     * VAR_OUTPUT => direction_out
     * VAR_IN_OUT => direction_inout
     * VAR_EXTERNAL => direction_extref
     *
     * Note that VAR_EXTERNAL declares variables that are in reality references
     * to global variables. This is used only inside programs!
     * These references to external variables must be correctly initialised to refer
     * to the correct global variable. Note that the user may define which variable is to be
     * referenced in a CONFIGURATION, and that different instantiations of a program
     * may have the same external variable reference diffenrent global variables!
     * The references must therefore be correctly initialised when the program instance
     * is created. This may be done by the PROGRAM class constructor since the ST and IL
     * languages do not allow the VAR_EXTERNAL reference to change at runtime
     * for a specific instance.
     *
     * We therefore need to call a PROGRAM class constructor with the variables
     * that should be refernced by the VAR_EXTERNAL variables. The direction_extref will
     * be used to identify these parameters!
     */
    typedef enum {direction_in, direction_out, direction_inout, direction_extref} param_direction_t ;


  private:
    /* a pointer to the function_block_declaration_c
    * or function_declaration_c currently being analysed.
    */
    symbol_c *f_decl;
    int next_param, param_count;
    /* used when called to search() for a parameter by name */
    identifier_c *search_param_name;
    /* used when called to iterate() for a parameter */
    identifier_c *current_param_name;
    symbol_c *current_param_type;
    symbol_c *current_param_default_value;
    param_direction_t current_param_direction;
    bool en_eno_param_implicit;
    /* used when we reach an extensible parameter in the function declaration */
    /* NOTE: this will handle syntax that is not in the standard.
     *       It is used to handle the extensible standard functions
     *       (e.g. AND(word#3, word#55, word#44); )
     *       See absyntax.def or iec.y for more details.
     */
    bool current_param_is_extensible;
    int  current_extensible_param_index;
    int  _first_extensible_param_index;

    /* Which operation of the class was called...
     * Search a parameter, or iterate to the next parameter.
     */
    typedef enum {iterate_op, search_op} operation_t;
    operation_t current_operation;
    
    /* the last parameter/value returned by search() or next() */
    symbol_c *last_returned_parameter; 
    
  private:
    int   cmp_extparam_names(const char* s1, const char* s2);
    void* handle_param_list(list_c *list);
    void* handle_single_param(symbol_c *var_name);

    void* iterate_list(list_c *list);

  public:
    /* start off at the first parameter once again... */
    void reset(void);

    /* initialise the iterator object.
     * We must be given a reference to one of the following
     *     - function_declaration_c
     *     - function_block_declaration_c
     *     - program_declaration_c
     * that will be analysed...
     */
    function_param_iterator_c(symbol_c *pou_decl);

    /* Skip to the next parameter. After object creation,
     * the object references on parameter _before_ the first, so
     * this function must be called once to get the object to
     * reference the first parameter...
     *
     * Returns the parameter's name!
     */
    identifier_c *next(void);

    /* Search for the parameter named <param_name>...  */
    /* The seach() function does not in any way affect the internal state related 
     * to the iterate() function.
     * It will, however, affect the internal state necessary to correctly
     * return the param_type(), default_value() and is_en_eno_param_implicit()
     * of the found parameter.
     */
    identifier_c *search(symbol_c *param_name);
    identifier_c *search(const char *param_name);

    /* Returns the currently referenced parameter's default value,
     * or NULL if none is specified in the function declrataion itself.
     */
    symbol_c *default_value(void);

    /* Returns the currently referenced parameter's type name. */
    symbol_c *param_type(void);

    /* Returns if currently referenced parameter is an implicit defined EN/ENO parameter. */
    bool is_en_eno_param_implicit(void);

    /* Returns if currently referenced parameter is an extensible parameter. */
    /* extensible paramters only occur in some standard functions, e.g. AND(word#34, word#44, word#65); */
    bool is_extensible_param(void);
    /* Returns the index of the current extensible parameter. */             
    /* If the current parameter is not an extensible paramter, returns -1 */
    int extensible_param_index(void);
    /* Returns the index of the first extensible parameter, or -1 if no extensible parameter found. */             
    /* WARNING: Will only return the correct value _after_ an extensible parameter has been found! */
    int first_extensible_param_index(void);
    
    /* Returns the currently referenced parameter's data passing direction.
     * i.e. VAR_INPUT, VAR_OUTPUT or VAR_INOUT
     */
    param_direction_t param_direction(void);

    private:
    void *visit(implicit_definition_c *symbol);
    /******************************************/
    /* B 1.4.3 - Declaration & Initialisation */
    /******************************************/
    void *visit(input_declarations_c *symbol);

    void *visit(input_declaration_list_c *symbol);

    void *visit(edge_declaration_c *symbol);
    void *visit(en_param_declaration_c *symbol);
    /* var1_list ':' array_spec_init */
    //SYM_REF2(array_var_init_decl_c, var1_list, array_spec_init)
    void *visit(array_var_init_decl_c *symbol);
    
    /*  var1_list ':' initialized_structure */
    //SYM_REF2(structured_var_init_decl_c, var1_list, initialized_structure)
    void *visit(structured_var_init_decl_c *symbol);
    
    /* name_list ':' function_block_type_name ASSIGN structure_initialization */
    /* structure_initialization -> may be NULL ! */
    // SYM_REF4(fb_name_decl_c, fb_name_list, function_block_type_name, structure_initialization, unused)
    void *visit(fb_name_decl_c *symbol);

    /* name_list ',' fb_name */
    // SYM_LIST(fb_name_list_c)
    void *visit(fb_name_list_c *symbol);
    
    void *visit(output_declarations_c *symbol);
    void *visit(eno_param_declaration_c *symbol);
    void *visit(input_output_declarations_c *symbol);
    void *visit(var_declaration_list_c *symbol);


    /*  var1_list ':' array_specification */
    //SYM_REF2(array_var_declaration_c, var1_list, array_specification)
    void *visit(array_var_declaration_c *symbol);

    /*  var1_list ':' structure_type_name */
    //SYM_REF2(structured_var_declaration_c, var1_list, structure_type_name)
    void *visit(structured_var_declaration_c *symbol);

    /* VAR [CONSTANT] var_init_decl_list END_VAR */
    void *visit(var_declarations_c *symbol);

#if 0
/*  VAR RETAIN var_init_decl_list END_VAR */
SYM_REF2(retentive_var_declarations_c, var_init_decl_list, unused)

/*  VAR [CONSTANT|RETAIN|NON_RETAIN] located_var_decl_list END_VAR */
/* option -> may be NULL ! */
SYM_REF2(located_var_declarations_c, option, located_var_decl_list)

/* helper symbol for located_var_declarations */
/* located_var_decl_list located_var_decl ';' */
SYM_LIST(located_var_decl_list_c)

/*  [variable_name] location ':' located_var_spec_init */
/* variable_name -> may be NULL ! */
SYM_REF4(located_var_decl_c, variable_name, location, located_var_spec_init, unused)
#endif

    /*| VAR_EXTERNAL [CONSTANT] external_declaration_list END_VAR */
    /* option -> may be NULL ! */
    // SYM_REF2(external_var_declarations_c, option, external_declaration_list)
    void *visit(external_var_declarations_c *symbol);

    /* helper symbol for external_var_declarations */
    /*| external_declaration_list external_declaration';' */
    // SYM_LIST(external_declaration_list_c)
    void *visit(external_declaration_list_c *symbol);

    /*  global_var_name ':' (simple_specification|subrange_specification|enumerated_specification|array_specification|prev_declared_structure_type_name|function_block_type_name */
    //SYM_REF2(external_declaration_c, global_var_name, specification)
    void *visit(external_declaration_c *symbol);


#if 0
/*| VAR_GLOBAL [CONSTANT|RETAIN] global_var_decl_list END_VAR */
/* option -> may be NULL ! */
SYM_REF2(global_var_declarations_c, option, global_var_decl_list)

/* helper symbol for global_var_declarations */
/*| global_var_decl_list global_var_decl ';' */
SYM_LIST(global_var_decl_list_c)

/*| global_var_spec ':' [located_var_spec_init|function_block_type_name] */
/* type_specification ->may be NULL ! */
SYM_REF2(global_var_decl_c, global_var_spec, type_specification)

/*| global_var_name location */
SYM_REF2(global_var_spec_c, global_var_name, location)

/*  AT direct_variable */
SYM_REF2(location_c, direct_variable, unused)

/*| global_var_list ',' global_var_name */
SYM_LIST(global_var_list_c)

/*  var1_list ':' single_byte_string_spec */
SYM_REF2(single_byte_string_var_declaration_c, var1_list, single_byte_string_spec)

/*  STRING ['[' integer ']'] [ASSIGN single_byte_character_string] */
/* integer ->may be NULL ! */
/* single_byte_character_string ->may be NULL ! */
SYM_REF2(single_byte_string_spec_c, integer, single_byte_character_string)

/*  var1_list ':' double_byte_string_spec */
SYM_REF2(double_byte_string_var_declaration_c, var1_list, double_byte_string_spec)

/*  WSTRING ['[' integer ']'] [ASSIGN double_byte_character_string] */
/* integer ->may be NULL ! */
/* double_byte_character_string ->may be NULL ! */
SYM_REF2(double_byte_string_spec_c, integer, double_byte_character_string)

/*| VAR [RETAIN|NON_RETAIN] incompl_located_var_decl_list END_VAR */
/* option ->may be NULL ! */
SYM_REF2(incompl_located_var_declarations_c, option, incompl_located_var_decl_list)

/* helper symbol for incompl_located_var_declarations */
/*| incompl_located_var_decl_list incompl_located_var_decl ';' */
SYM_LIST(incompl_located_var_decl_list_c)

/*  variable_name incompl_location ':' var_spec */
SYM_REF4(incompl_located_var_decl_c, variable_name, incompl_location, var_spec, unused)

/*  AT incompl_location_token */
SYM_TOKEN(incompl_location_c)
#endif


    void *visit(var1_init_decl_c *symbol);
    void *visit(var1_list_c *symbol);
    void *visit(var_init_decl_list_c *symbol);


    /***********************/
    /* B 1.5.1 - Functions */
    /***********************/
    void *visit(function_declaration_c *symbol);
    /* intermediate helper symbol for function_declaration */
    void *visit(var_declarations_list_c *symbol);
    void *visit(function_var_decls_c *symbol);


    /*****************************/
    /* B 1.5.2 - Function Blocks */
    /*****************************/
    /*  FUNCTION_BLOCK derived_function_block_name io_OR_other_var_declarations function_block_body END_FUNCTION_BLOCK */
    void *visit(function_block_declaration_c *symbol);

    /* intermediate helper symbol for function_declaration */
    /*  { io_var_declarations | other_var_declarations }   */
    /*
     * NOTE: we re-use the var_declarations_list_c
     */

    /*  VAR_TEMP temp_var_decl_list END_VAR */
    void *visit(temp_var_decls_c *symbol);
    void *visit(temp_var_decls_list_c *symbol);

    /*  VAR NON_RETAIN var_init_decl_list END_VAR */
    void *visit(non_retentive_var_decls_c *symbol);


    /**********************/
    /* B 1.5.3 - Programs */
    /**********************/
    /*  PROGRAM program_type_name program_var_declarations_list function_block_body END_PROGRAM */
    // SYM_REF4(program_declaration_c, program_type_name, var_declarations, function_block_body, unused)
    void *visit(program_declaration_c *symbol);

    /* intermediate helper symbol for program_declaration_c */
    /*  { io_var_declarations | other_var_declarations }   */
    /*
     * NOTE: we re-use the var_declarations_list_c
     */

}; // function_param_iterator_c







