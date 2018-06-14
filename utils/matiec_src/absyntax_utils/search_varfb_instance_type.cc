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



#include "absyntax_utils.hh"


void search_varfb_instance_type_c::init(void) {
  this->current_type_id        = NULL;
  this->current_basetype_id    = NULL;
  this->current_basetype_decl  = NULL;
  this->current_field_selector = NULL;
}


search_varfb_instance_type_c::search_varfb_instance_type_c(symbol_c *search_scope): search_var_instance_decl(search_scope) {
  this->init();
}


/* We expect to be passed a symbolic_variable_c */
symbol_c *search_varfb_instance_type_c::get_type_id(symbol_c *variable_name) {
  this->init();
  variable_name->accept(*this);
  return current_type_id;
}


symbol_c *search_varfb_instance_type_c::get_basetype_id(symbol_c *variable_name) {
  this->init();
  variable_name->accept(*this);
  return current_basetype_id;
}


symbol_c *search_varfb_instance_type_c::get_basetype_decl(symbol_c *variable_name) {
  this->init();
  variable_name->accept(*this);
  return current_basetype_decl;
}  




/*************************/
/* B.1 - Common elements */
/*************************/
/*******************************************/
/* B 1.1 - Letters, digits and identifiers */
/*******************************************/
// SYM_TOKEN(identifier_c)
void *search_varfb_instance_type_c::visit(identifier_c *variable_name) {
  /* symbol should be a variable name!! */
  /* Note: although the method is called get_decl(), it is getting the declaration of the variable, which for us is the type_id of that variable! */
  current_type_id       = search_var_instance_decl.get_decl (variable_name);
  current_basetype_decl = search_base_type_c::get_basetype_decl(current_type_id);
  current_basetype_id   = search_base_type_c::get_basetype_id  (current_type_id);
    
  /* What if the variable has not been declared?  Then this should not be a compiler error! 
   * However, currently stage 2 of the compiler already detects when variables have not been delcared,
   * so if the variable's declaration is not found, then that means that we have an internal compiler error!
   * 
   * Actually, the above is not true anymore. See the use of the any_symbolic_variable in iec_bison.yy
   *  - when defining the delay of a delayed action association in SFC
   *  - in program connections inside configurations (will this search_varfb_instance_type_c class be called to handle this??)
   */
  // if (NULL == current_type_id) ERROR; 

  return NULL;
}





/********************************/
/* B 1.3.3 - Derived data types */
/********************************/
/*  identifier ':' array_spec_init */
/* NOTE: I don't think this will ever get called, since in the visit method for array_variable_c
 * we use the basetype_decl for recursively calling this class, and the base type should never be a 
 * array_type_declaration_c, but for now, let's leave it in...
 */
void *search_varfb_instance_type_c::visit(array_type_declaration_c *symbol) {
  ERROR;
  return NULL;
}
    
/* array_specification [ASSIGN array_initialization] */
/* array_initialization may be NULL ! */
/* NOTE: I don't think this will ever get called, since in the visit method for array_variable_c
 * we use the basetype_decl for recursively calling this class, and the base type should never be a 
 * array_spec_init_c, but for now, let's leave it in...
 */
void *search_varfb_instance_type_c::visit(array_spec_init_c *symbol) {
  /* Note that the 'array_specification' may be either an identifier of a previsously defined array type, 
   * or an array_specification_c, so we can not stop here and simply return a array_spec_init_c, 
   * especially if we are looking for the base class!
   */
  ERROR;
  return NULL;
}

/* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
/* NOTE: This method will be reached after being called from the 
 * search_varfb_instance_type_c::visit(array_variable_c *symbol)
 * method, so we must return the data type of the data stored in the array, 
 * and not the data type of the array itself!
 */
void *search_varfb_instance_type_c::visit(array_specification_c *symbol) {
  /* found the type of the element we were looking for! */
  current_type_id       = symbol->non_generic_type_name;
  current_basetype_decl = search_base_type_c::get_basetype_decl(current_type_id);
  current_basetype_id   = search_base_type_c::get_basetype_id  (current_type_id);
    
  return NULL; 
}


/*  structure_type_name ':' structure_specification */
/* NOTE: this is only used inside a TYPE ... END_TYPE declaration. 
 * It is never used directly when declaring a new variable! 
 */
/* NOTE: I don't think this will ever get called, since in the visit method for structured_variable_c
 * we use the basetype_decl for recursively calling this class, and the base type should never be a 
 * structure_type_declaration_c, but for now, let's leave it in...
 */
void *search_varfb_instance_type_c::visit(structure_type_declaration_c *symbol) {
  if (NULL == current_field_selector) return NULL; // the source code has a datatype consistency bug that will be caught later!!
  
  symbol->structure_specification->accept(*this);
  return NULL;
  /* NOTE: structure_specification will point to either a
   *       initialized_structure_c
   *       OR A
   *       structure_element_declaration_list_c
   */
}

/* structure_type_name ASSIGN structure_initialization */
/* structure_initialization may be NULL ! */
// SYM_REF2(initialized_structure_c, structure_type_name, structure_initialization)
/* NOTE: only the initialized structure is never used when declaring a new variable instance */
/* NOTE: I don't think this will ever get called, since in the visit method for structured_variable_c
 * we use the basetype_decl for recursively calling this class, and the base type should never be a 
 * initialized_structure_c, but for now, let's leave it in...
 */
void *search_varfb_instance_type_c::visit(initialized_structure_c *symbol)	{
  if (NULL == current_field_selector) return NULL; // the source code has a datatype consistency bug that will be caught later!!
  
  /* recursively find out the data type of current_field_selector... */
  symbol->structure_type_name->accept(*this);
  return NULL;
}

/* helper symbol for structure_declaration */
/* structure_declaration:  STRUCT structure_element_declaration_list END_STRUCT */
/* structure_element_declaration_list structure_element_declaration ';' */
void *search_varfb_instance_type_c::visit(structure_element_declaration_list_c *symbol)	{
  if (NULL == current_field_selector) return NULL; // the source code has a datatype consistency bug that will be caught later!!

  /* now search the structure declaration */
  for(int i = 0; i < symbol->n; i++) {
    symbol->elements[i]->accept(*this);
  }

  return NULL;
}

/*  structure_element_name ':' spec_init */
void *search_varfb_instance_type_c::visit(structure_element_declaration_c *symbol) {
  if (NULL == current_field_selector) return NULL; // the source code has a datatype consistency bug that will be caught later!!

  if (compare_identifiers(symbol->structure_element_name, current_field_selector) == 0) {
    /* found the type of the element we were looking for! */
    current_type_id       = symbol->spec_init;
    current_basetype_decl = search_base_type_c::get_basetype_decl(current_type_id);
    current_basetype_id   = search_base_type_c::get_basetype_id  (current_type_id);
  }  

  /* Did not find the type of the element we were looking for! */
  /* Will keep looking... */
  return NULL;
}

/* helper symbol for structure_initialization */
/* structure_initialization: '(' structure_element_initialization_list ')' */
/* structure_element_initialization_list ',' structure_element_initialization */
void *search_varfb_instance_type_c::visit(structure_element_initialization_list_c *symbol) {ERROR; return NULL;} /* should never get called... */
/*  structure_element_name ASSIGN value */
void *search_varfb_instance_type_c::visit(structure_element_initialization_c *symbol) {ERROR; return NULL;} /* should never get called... */


/*********************/
/* B 1.4 - Variables */
/*********************/
// SYM_REF1(symbolic_variable_c, var_name)
void *search_varfb_instance_type_c::visit(symbolic_variable_c *symbol) {
  symbol->var_name->accept(*this);
  return NULL;
}

/********************************************/
/* B.1.4.1   Directly Represented Variables */
/********************************************/
// SYM_TOKEN(direct_variable_c)
/* We do not yet handle this. Will we ever need to handle it, as the data type of the direct variable is
 * directly obtainable from the syntax of the direct variable itself?
 */

/*************************************/
/* B 1.4.2 - Multi-element variables */
/*************************************/
/*  subscripted_variable '[' subscript_list ']' */
// SYM_REF2(array_variable_c, subscripted_variable, subscript_list)
/* NOTE: when passed a array_variable_c, which represents some IEC61131-3 code similar to X[42]
 * we must return the data type of the value _stored_ in the array.
 * If you want to get the data type of the array itself (i.e. just the X variable, without the [42])
 * then this class must be called with the identifier_c 'X'.
 */
void *search_varfb_instance_type_c::visit(array_variable_c *symbol) {
  /* determine the data type of the subscripted_variable... 
   * This should be an array_specification_c
   *    ARRAY [xx..yy] OF Stored_Data_Type
   */
  symbol->subscripted_variable->accept(*this);
  symbol_c *basetype_decl = current_basetype_decl;
  this->init(); /* set all current_*** pointers to NULL ! */
  
  /* Now we determine the 'Stored_Data_Type', i.e. the data type of the variable stored in the array. */
  if (NULL != basetype_decl) {
    basetype_decl->accept(*this);
  }
  
  return NULL;
}


/*  record_variable '.' field_selector */
/*  WARNING: input and/or output variables of function blocks
 *           may be accessed as fields of a structured variable!
 *           Code handling a structured_variable_c must take this into account!
 *           (i.e. that a FB instance may be accessed as a structured variable)!
 *
 *  WARNING: Status bit (.X) and activation time (.T) of STEPS in SFC diagrams
 *           may be accessed as fields of a structured variable!
 *           Code handling a structured_variable_c must take this into account 
 *           (i.e. that an SFC STEP may be accessed as a structured variable)!
 */
// SYM_REF2(structured_variable_c, record_variable, field_selector)
void *search_varfb_instance_type_c::visit(structured_variable_c *symbol) {
  symbol->record_variable->accept(*this);
  symbol_c *basetype_decl = current_basetype_decl;
  this->init(); /* set all current_*** pointers to NULL ! */
  
  /* Now we search for the data type of the field... But only if we were able to determine the data type of the variable */
  if (NULL != basetype_decl) {
    current_field_selector = symbol->field_selector;
    basetype_decl->accept(*this);
    current_field_selector = NULL;
  }
  
  return NULL;
}



/**************************************/
/* B.1.5 - Program organization units */
/**************************************/
/*****************************/
/* B 1.5.2 - Function Blocks */
/*****************************/

/*  FUNCTION_BLOCK derived_function_block_name io_OR_other_var_declarations function_block_body END_FUNCTION_BLOCK */
// SYM_REF4(function_block_declaration_c, fblock_name, var_declarations, fblock_body, unused)
void *search_varfb_instance_type_c::visit(function_block_declaration_c *symbol) {
  if (NULL == current_field_selector) return NULL; // the source code has a datatype consistency bug that will be caught later!!

  /* now search the function block declaration for the variable... */
  /* If not found, these pointers will all be set to NULL!! */
  search_var_instance_decl_c search_decl(symbol);
  current_type_id       = search_decl.get_decl(current_field_selector);
  current_basetype_decl = search_base_type_c::get_basetype_decl(current_type_id);
  current_basetype_id   = search_base_type_c::get_basetype_id  (current_type_id);
  
  return NULL;
}



/*********************************************/
/* B.1.6  Sequential function chart elements */
/*********************************************/
/* INITIAL_STEP step_name ':' action_association_list END_STEP */
// SYM_REF2(initial_step_c, step_name, action_association_list)
/* NOTE: this method may be called from visit(structured_variable_c *symbol) method| */
void *search_varfb_instance_type_c::visit(initial_step_c *symbol) {
  if (NULL == current_field_selector) return NULL; // the source code has a datatype consistency bug that will be caught later!!

  identifier_c T("T");
  identifier_c X("X");
  
  /* Hard code the datatypes of the implicit variables Stepname.X and Stepname.T */
  if (compare_identifiers(&T, current_field_selector) == 0)   
    current_type_id = &get_datatype_info_c::time_type_name;
  if (compare_identifiers(&X, current_field_selector) == 0)   
    current_type_id = &get_datatype_info_c::bool_type_name;
  
  current_basetype_decl = search_base_type_c::get_basetype_decl(current_type_id);
  current_basetype_id   = search_base_type_c::get_basetype_id  (current_type_id);

  return NULL;
}


/* STEP step_name ':' action_association_list END_STEP */
// SYM_REF2(step_c, step_name, action_association_list)
/* NOTE: this method may be called from visit(structured_variable_c *symbol) method| */
void *search_varfb_instance_type_c::visit(step_c *symbol) {
  /* The code here should be identicial to the code in the visit(initial_step_c *) visitor! So we simply call the other visitor! */
  initial_step_c initial_step(NULL, NULL);
  return initial_step.accept(*this);
}


/***************************************/
/* B.3 - Language ST (Structured Text) */
/***************************************/
/***********************/
/* B 3.1 - Expressions */
/***********************/
/* SYM_REF1(deref_expression_c, exp)  --> an extension to the IEC 61131-3 standard - based on the IEC 61131-3 v3 standard. Returns address of the varible! */
void *search_varfb_instance_type_c::visit(deref_expression_c  *symbol) {
  symbol->exp->accept(*this);
  symbol_c *basetype_decl = current_basetype_decl;
  this->init(); /* set all current_*** pointers to NULL ! */
  
  /* Check whether the expression if a REF_TO datatype, and if so, set the new datatype to the datatype it references! */
    /* Determine whether the datatype is a ref_spec_c, as this is the class used as the    */
    /* canonical/base datatype of REF_TO types (see search_base_type_c ...)                */   
  ref_spec_c * ref_spec = dynamic_cast<ref_spec_c *>(basetype_decl);
  if (NULL != ref_spec) {
    current_basetype_decl = search_base_type_c::get_basetype_decl(ref_spec->type_name);
    current_basetype_id   = search_base_type_c::get_basetype_id  (ref_spec->type_name);
  }
  
  return NULL;
}

  
/* SYM_REF1(deref_operator_c, exp)  --> an extension to the IEC 61131-3 standard - based on the IEC 61131-3 v3 standard. Returns address of the varible! */
void *search_varfb_instance_type_c::visit(deref_operator_c  *symbol) {
  symbol->exp->accept(*this);
  symbol_c *basetype_decl = current_basetype_decl;
  this->init(); /* set all current_*** pointers to NULL ! */
  
  /* Check whether the expression if a REF_TO datatype, and if so, set the new datatype to the datatype it references! */
    /* Determine whether the datatype is a ref_spec_c, as this is the class used as the    */
    /* canonical/base datatype of REF_TO types (see search_base_type_c ...)                */   
  ref_spec_c * ref_spec = dynamic_cast<ref_spec_c *>(basetype_decl);
  if (NULL != ref_spec) {
    current_basetype_decl = search_base_type_c::get_basetype_decl(ref_spec->type_name);
    current_basetype_id   = search_base_type_c::get_basetype_id  (ref_spec->type_name);
  }
  
  return NULL;
}
