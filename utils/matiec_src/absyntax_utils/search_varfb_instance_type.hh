/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2003-2012  Mario de Sousa (msousa@fe.up.pt)
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



/* Determine the data type of a variable.
 * The variable may be a simple variable, a function block instance, a
 * struture element within a data structured type (a struct or a fb), or
 * an array element.
 * A mixture of array element of a structure element of a structure element
 * of a .... is also suported!
 *
 *  example:
 *    window.points[1].coordinate.x
 *    window.points[1].colour
 *    etc... ARE ALLOWED!
 *
 * This class must be passed the scope within which the
 * variable was declared, and the variable name...
 *
 *
 *
 *
 *
 * This class has several members, depending on the exact data the caller
 * is looking for...
 *
 *    - item i: we can get either the name of the data type(A),
 *              or it's declaration (B)
 *             (notice however that some variables belong to a data type that does
 *              not have a name, only a declaration as in
 *              VAR a: ARRAY [1..3] of INT; END_VAR
 *             )
 *    - item ii: we can get either the direct data type (1), 
 *               or the base type (2)
 * 
 *   By direct type, I mean the data type of the variable. By base type, I 
 * mean the data type on which the direct type is based on. For example, in 
 * a subrange on INT, the direct type is the subrange itself, while the 
 * base type is INT.
 * e.g.
 *   This means that if we find that the variable is of type MY_INT,
 *   which was previously declared to be
 *   TYPE MY_INT: INT := 9;
 *   option (1) will return MY_INT
 *   option (2) will return INT
 * 
 *
 * Member functions:
 * ================
 *   get_basetype_id()    ---> returns 2A   (implemented, although currently it is not needed! )
 *   get_basetype_decl()  ---> returns 2B 
 *   get_type_id()        ---> returns 1A
 * 
 *   Since we haven't yet needed it, we don't yet implement
 *   get_type_decl()      ---> returns 1B 
 */ 

class search_varfb_instance_type_c : null_visitor_c {

  private:
    search_var_instance_decl_c search_var_instance_decl;

//  symbol_c *current_type_decl;
    symbol_c *current_type_id;
    symbol_c *current_basetype_decl;
    symbol_c *current_basetype_id;
    
    symbol_c *current_field_selector;

    /* sets all the above variables to NULL, or false */
    void init(void);

  public:
    search_varfb_instance_type_c(symbol_c *search_scope );
    symbol_c *get_basetype_decl (symbol_c *variable_name);
    symbol_c *get_basetype_id   (symbol_c *variable_name);
//  symbol_c *get_type_decl     (symbol_c *variable_name);
    symbol_c *get_type_id       (symbol_c *variable_name);



  private:
    /* a helper function... */
    void *visit_list(list_c *list);

    /* a helper function... */
    void *base_type(symbol_c *symbol);


  private:
    /*************************/
    /* B.1 - Common elements */
    /*************************/
    /*******************************************/
    /* B 1.1 - Letters, digits and identifiers */
    /*******************************************/
    void *visit(identifier_c *variable_name);
      
    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
    /*  identifier ':' array_spec_init */
    void *visit(array_type_declaration_c *symbol);
    
    /* array_specification [ASSIGN array_initialization} */
    /* array_initialization may be NULL ! */
    void *visit(array_spec_init_c *symbol);
    
    /* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
    void *visit(array_specification_c *symbol);

    /*  structure_type_name ':' structure_specification */
    void *visit(structure_type_declaration_c *symbol);

    /* structure_type_name ASSIGN structure_initialization */
    /* structure_initialization may be NULL ! */
    // SYM_REF2(initialized_structure_c, structure_type_name, structure_initialization)
    void *visit(initialized_structure_c *symbol);

    /* helper symbol for structure_declaration */
    /* structure_declaration:  STRUCT structure_element_declaration_list END_STRUCT */
    /* structure_element_declaration_list structure_element_declaration ';' */
    void *visit(structure_element_declaration_list_c *symbol);

    /*  structure_element_name ':' spec_init */
    void *visit(structure_element_declaration_c *symbol);

    /* helper symbol for structure_initialization */
    /* structure_initialization: '(' structure_element_initialization_list ')' */
    /* structure_element_initialization_list ',' structure_element_initialization */
    void *visit(structure_element_initialization_list_c *symbol); /* should never get called... */
    /*  structure_element_name ASSIGN value */
    void *visit(structure_element_initialization_c *symbol); /* should never get called... */


    /*********************/
    /* B 1.4 - Variables */
    /*********************/
    void *visit(symbolic_variable_c *symbol);

    /********************************************/
    /* B.1.4.1   Directly Represented Variables */
    /********************************************/
    /*************************************/
    /* B 1.4.2 - Multi-element variables */
    /*************************************/
    void *visit(array_variable_c *symbol);
    void *visit(structured_variable_c *symbol);
    
    /**************************************/
    /* B.1.5 - Program organization units */
    /**************************************/
    /*****************************/
    /* B 1.5.2 - Function Blocks */
    /*****************************/
    /*  FUNCTION_BLOCK derived_function_block_name io_OR_other_var_declarations function_block_body END_FUNCTION_BLOCK */
    // SYM_REF4(function_block_declaration_c, fblock_name, var_declarations, fblock_body, unused)
    void *visit(function_block_declaration_c *symbol);

    
    /*********************************************/
    /* B.1.6  Sequential function chart elements */
    /*********************************************/
    /* INITIAL_STEP step_name ':' action_association_list END_STEP */
    // SYM_REF2(initial_step_c, step_name, action_association_list)
    void *visit(initial_step_c *symbol);
    /* STEP step_name ':' action_association_list END_STEP */
    // SYM_REF2(step_c, step_name, action_association_list)
    void *visit(step_c *symbol);
    
    /***************************************/
    /* B.3 - Language ST (Structured Text) */
    /***************************************/
    /***********************/
    /* B 3.1 - Expressions */
    /***********************/
    /* SYM_REF1(deref_expression_c, exp)  --> an extension to the IEC 61131-3 standard - based on the IEC 61131-3 v3 standard. Returns address of the varible! */
    void *visit(deref_operator_c    *symbol);
    void *visit(deref_expression_c  *symbol);
    
}; // search_varfb_instance_type_c






