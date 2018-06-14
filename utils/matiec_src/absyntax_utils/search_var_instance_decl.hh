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


/* Search in a VAR* END_VAR declaration for the delcration of the specified variable instance. 
 * Will return:
 *     - the declaration itself (get_decl() )
 *     - the type of declaration in which the variable was declared (get_vartype() )
 *
 * The variable instance may NOT be a member of a structure of a memeber
 * of a structure of an element of an array of ...
 *
 * For example, considering the following 'variables':
 *    window.points[1].coordinate.x
 *    window.points[1].colour
 *    offset[99]
 *
 *   passing a reference to 'points', 'points[1]', 'points[1].colour', 'colour'
 *    ARE NOT ALLOWED!
 *
 * This class must only be passed the name of the variable that will appear
 * in the variable declaration. In the above examples, this would be
 *   'window.points[1].coordinate.x'
 *   'window.points[1].coordinate'
 *   'window.points[1]'
 *   'window'
 *   'window.points[1].colour'
 *   'offset'
 *   'offset[99]'
 *
 *
 */
 
/* Note: 
 * Determining the declaration type of a specific variable instance (including
 * function block instances) really means determining whether the variable was declared in a
 *  VAR_INPUT
 *  VAR_OUTPUT
 *  VAR_IN_OUT
 *  VAR
 *  VAR_TEMP
 *  VAR_EXTERNAL
 *  VAR_GLOBAL
 *  VAR <var_name> AT <location>   -> Located variable!
 * 
 */

/* Note:
 *  The current_type_decl that this class returns may reference the
 * name of a type, or the type declaration itself!
 * For an example of the first, consider a variable declared as ...
 * x : AAA;
 * where the AAA type is previously declared as whatever.
 * For an example of the second, consider a variable declared as ...
 * x : array of int [10];  ---->  is allowed
 *
 * If it is the first, we will return a reference to the name, if the second
 * we return a reference to the declaration!!
 */


class search_var_instance_decl_c: public search_visitor_c {

  public:
    search_var_instance_decl_c(symbol_c *search_scope);

  public:
    typedef enum {
        input_vt   ,  // VAR_INPUT
        output_vt  ,  // VAR_OUTPUT
        inoutput_vt,  // VAR_IN_OUT
        private_vt ,  // VAR
        temp_vt    ,  // VAR_TEMP
        external_vt,  // VAR_EXTERNAL
        global_vt  ,  // VAR_GLOBAL
        located_vt ,   // VAR <var_name> AT <location>
        none_vt
      } vt_t;
      
    typedef enum {
        constant_opt  ,
        retain_opt    ,
        non_retain_opt,
        none_opt
      } opt_t;
#if 0        
    /* the types of variables that need to be processed... */
    static const unsigned int none_vt     = 0x0000;
    static const unsigned int input_vt    = 0x0001;  // VAR_INPUT
    static const unsigned int output_vt   = 0x0002;  // VAR_OUTPUT
    static const unsigned int inoutput_vt = 0x0004;  // VAR_IN_OUT
    static const unsigned int private_vt  = 0x0008;  // VAR
    static const unsigned int temp_vt     = 0x0010;  // VAR_TEMP
    static const unsigned int external_vt = 0x0020;  // VAR_EXTERNAL
    static const unsigned int global_vt   = 0x0040;  // VAR_GLOBAL
    static const unsigned int located_vt  = 0x0080;  // VAR <var_name> AT <location>

    static const unsigned int none_opt        = 0x0000;
    static const unsigned int constant_opt    = 0x0001;
    static const unsigned int retain_opt      = 0x0002;
    static const unsigned int non_retain_opt  = 0x0003;
#endif    
    
    symbol_c *get_decl          (symbol_c *variable_instance_name); 
    symbol_c *get_basetype_decl (symbol_c *variable_instance_name); 
    vt_t      get_vartype       (symbol_c *variable_instance_name);
    opt_t     get_option        (symbol_c *variable_instance_name);

  private:
    symbol_c *search_scope;
    symbol_c *search_name;
    symbol_c *current_type_decl;
    /* variable used to store the type of variable currently being processed... */
    /* Will contain a single value of generate_c_vardecl_c::XXXX_vt */
    vt_t  current_vartype;
    opt_t current_option;

    
  private:
    /***************************/
    /* B 0 - Programming Model */
    /***************************/
    void *visit(library_c *symbol);

    /******************************************/
    /* B 1.4.3 - Declaration & Initialisation */
    /******************************************/

    /* edge -> The F_EDGE or R_EDGE directive */
    // SYM_REF2(edge_declaration_c, edge, var1_list)
    // TODO
    void *visit(constant_option_c *symbol);
    void *visit(retain_option_c *symbol);
    void *visit(non_retain_option_c *symbol);


    void *visit(input_declarations_c *symbol);
    /* VAR_OUTPUT [RETAIN | NON_RETAIN] var_init_decl_list END_VAR */
    /* option -> may be NULL ! */
    void *visit(output_declarations_c *symbol);
    /*  VAR_IN_OUT var_declaration_list END_VAR */
    void *visit(input_output_declarations_c *symbol);
    /* ENO : BOOL */
    void *visit(eno_param_declaration_c *symbol);
    /* EN : BOOL */
    void *visit(en_param_declaration_c *symbol);
    /* VAR [CONSTANT] var_init_decl_list END_VAR */
    /* option -> may be NULL ! */
    /* helper symbol for input_declarations */
    void *visit(var_declarations_c *symbol);
    /*  VAR RETAIN var_init_decl_list END_VAR */
    void *visit(retentive_var_declarations_c *symbol);
    /*  VAR [CONSTANT|RETAIN|NON_RETAIN] located_var_decl_list END_VAR */
    /* option -> may be NULL ! */
    //SYM_REF2(located_var_declarations_c, option, located_var_decl_list)
    void *visit(located_var_declarations_c *symbol);
    /*| VAR_EXTERNAL [CONSTANT] external_declaration_list END_VAR */
    /* option -> may be NULL ! */
    //SYM_REF2(external_var_declarations_c, option, external_declaration_list)
    void *visit(external_var_declarations_c *symbol);
    /*| VAR_GLOBAL [CONSTANT|RETAIN] global_var_decl_list END_VAR */
    /* option -> may be NULL ! */
    //SYM_REF2(global_var_declarations_c, option, global_var_decl_list)
    void *visit(global_var_declarations_c *symbol);
    /* var1_list is one of the following...
     *    simple_spec_init_c *
     *    subrange_spec_init_c *
     *    enumerated_spec_init_c *
     */
    // SYM_REF2(var1_init_decl_c, var1_list, spec_init)
    void *visit(var1_init_decl_c *symbol);
    /* var1_list ',' variable_name */
    // SYM_LIST(var1_list_c)
    void *visit(var1_list_c *symbol);
    /* name_list ':' function_block_type_name ASSIGN structure_initialization */
    /* structure_initialization -> may be NULL ! */
    void *visit(fb_name_decl_c *symbol);
    /* name_list ',' fb_name */
    void *visit(fb_name_list_c *symbol);
    /* var1_list ':' array_spec_init */
    // SYM_REF2(array_var_init_decl_c, var1_list, array_spec_init)
    void *visit(array_var_init_decl_c *symbol);
    /*  var1_list ':' initialized_structure */
    // SYM_REF2(structured_var_init_decl_c, var1_list, initialized_structure)
    void *visit(structured_var_init_decl_c *symbol);
    /*  var1_list ':' array_specification */
    // SYM_REF2(array_var_declaration_c, var1_list, array_specification)
    void *visit(array_var_declaration_c *symbol);
    /*  var1_list ':' structure_type_name */
    // SYM_REF2(structured_var_declaration_c, var1_list, structure_type_name)
    void *visit(structured_var_declaration_c *symbol);
    /*  [variable_name] location ':' located_var_spec_init */
    /* variable_name -> may be NULL ! */
    // SYM_REF4(located_var_decl_c, variable_name, location, located_var_spec_init, unused)
    // TODO!!

    /*  global_var_name ':' (simple_specification|subrange_specification|enumerated_specification|array_specification|prev_declared_structure_type_name|function_block_type_name */
    // SYM_REF2(external_declaration_c, global_var_name, specification)
    void *visit(external_declaration_c *symbol);
    /*| global_var_spec ':' [located_var_spec_init|function_block_type_name] */
    /* type_specification ->may be NULL ! */
    // SYM_REF2(global_var_decl_c, global_var_spec, type_specification)
    void *visit(global_var_decl_c *symbol);
    /*| global_var_name location */
    //SYM_REF2(global_var_spec_c, global_var_name, location)
    void *visit(global_var_spec_c *symbol);
    /*| global_var_list ',' global_var_name */
    //SYM_LIST(global_var_list_c)
    void *visit(global_var_list_c *symbol);
    /*  [variable_name] location ':' located_var_spec_init */
    /* variable_name -> may be NULL ! */
    //SYM_REF4(located_var_decl_c, variable_name, location, located_var_spec_init, unused)
    void *visit(located_var_decl_c *symbol);
    /*| global_var_spec ':' [located_var_spec_init|function_block_type_name] */
    /* type_specification ->may be NULL ! */
    // SYM_REF2(global_var_decl_c, global_var_spec, type_specification)
    // TODO!!
    /*  AT direct_variable */
    // SYM_REF2(location_c, direct_variable, unused)
    void *visit(location_c *symbol);
    /*| global_var_list ',' global_var_name */
    // SYM_LIST(global_var_list_c)
    // TODO!!
    /*  var1_list ':' single_byte_string_spec */
    // SYM_REF2(single_byte_string_var_declaration_c, var1_list, single_byte_string_spec)
    void *visit(single_byte_string_var_declaration_c *symbol);
    /*  STRING ['[' integer ']'] [ASSIGN single_byte_character_string] */
    /* integer ->may be NULL ! */
    /* single_byte_character_string ->may be NULL ! */
    // SYM_REF2(single_byte_string_spec_c, integer, single_byte_character_string)
    // TODO!!
    
    /*  var1_list ':' double_byte_string_spec */
    // SYM_REF2(double_byte_string_var_declaration_c, var1_list, double_byte_string_spec)
    void *visit(double_byte_string_var_declaration_c *symbol);
    /*  WSTRING ['[' integer ']'] [ASSIGN double_byte_character_string] */
    /* integer ->may be NULL ! */
    /* double_byte_character_string ->may be NULL ! */
    // SYM_REF2(double_byte_string_spec_c, integer, double_byte_character_string)
    // TODO!!
    
    /*  variable_name incompl_location ':' var_spec */
    // SYM_REF4(incompl_located_var_decl_c, variable_name, incompl_location, var_spec, unused)
    // TODO!!
    
    /*  AT incompl_location_token */
    // SYM_TOKEN(incompl_location_c)
    // TODO!!


    /**************************************/
    /* B.1.5 - Program organization units */
    /**************************************/
    /***********************/
    /* B 1.5.1 - Functions */
    /***********************/
    // SYM_REF4(function_declaration_c, derived_function_name, type_name, var_declarations_list, function_body)
    void *visit(function_declaration_c *symbol);

    /*****************************/
    /* B 1.5.2 - Function Blocks */
    /*****************************/
    void *visit(function_block_declaration_c *symbol);

    /**********************/
    /* B 1.5.3 - Programs */
    /**********************/
    void *visit(program_declaration_c *symbol);

    /*********************************************/
    /* B.1.6  Sequential function chart elements */
    /*********************************************/
    /* | sequential_function_chart sfc_network */
    // SYM_LIST(sequential_function_chart_c)
    /* search_var_instance_decl_c inherits from serach_visitor_c, so no need to implement the following method. */
    // void *visit(sequential_function_chart_c *symbol);
    
    /* initial_step {step | transition | action} */
    // SYM_LIST(sfc_network_c)
    /* search_var_instance_decl_c inherits from serach_visitor_c, so no need to implement the following method. */
    // void *visit(sfc_network_c *symbol);


    /* INITIAL_STEP step_name ':' action_association_list END_STEP */
    // SYM_REF2(initial_step_c, step_name, action_association_list)
    void *visit(initial_step_c *symbol);

    /* STEP step_name ':' action_association_list END_STEP */
    // SYM_REF2(step_c, step_name, action_association_list)
    void *visit(step_c *symbol);

    /********************************/
    /* B 1.7 Configuration elements */
    /********************************/
    
    /*
    CONFIGURATION configuration_name
       optional_global_var_declarations
       (resource_declaration_list | single_resource_declaration)
       optional_access_declarations
       optional_instance_specific_initializations
    END_CONFIGURATION
    */
    /*
    SYM_REF6(configuration_declaration_c, configuration_name, global_var_declarations, resource_declarations, access_declarations, instance_specific_initializations, unused)
    */                                                                                                            
    void *visit(configuration_declaration_c *symbol);
    /*
    RESOURCE resource_name ON resource_type_name
       optional_global_var_declarations
       single_resource_declaration
    END_RESOURCE
    */
    // SYM_REF4(resource_declaration_c, resource_name, resource_type_name, global_var_declarations, resource_declaration)
    void *visit(resource_declaration_c *symbol);

    /* task_configuration_list program_configuration_list */
    // SYM_REF2(single_resource_declaration_c, task_configuration_list, program_configuration_list)
    void *visit(single_resource_declaration_c *symbol);

    
    /****************************************/
    /* B.2 - Language IL (Instruction List) */
    /****************************************/
    /***********************************/
    /* B 2.1 Instructions and Operands */
    /***********************************/
    /*| instruction_list il_instruction */
    // SYM_LIST(instruction_list_c)
    void *visit(instruction_list_c *symbol);


    /***************************************/
    /* B.3 - Language ST (Structured Text) */
    /***************************************/
    /********************/
    /* B 3.2 Statements */
    /********************/
    // SYM_LIST(statement_list_c)
    void *visit(statement_list_c *symbol);

}; // search_var_instance_decl_c

