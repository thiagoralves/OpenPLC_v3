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


#include "function_param_iterator.hh"  /* no longer required, aready included by absyntax_utils.hh */
#include "spec_init_separator.hh"  /* no longer required, aready included by absyntax_utils.hh */
#include <stdlib.h>  /* required for strtol() */
#include <string.h>
#include <strings.h>
#include <limits> // required for std::numeric_limits< XXX >::max()
#include <errno.h> // required for errno
#include "../main.hh" // required for ERROR() and ERROR_MSG() macros.

//#define DEBUG
#ifdef DEBUG
#define TRACE(classname) printf("\n____%s____\n",classname);
#else
#define TRACE(classname)
#endif





/* NOTE: The following function is not really needed, as we could get the value that constant_folding_c determined for this
 *       integer. Remember that currently constant_folding_c runs before this class is ever used/called!
 *       However, I (Mario) do not currently feel it would be a good idea to restrict the use of this
 *       abstract syntax utility to only after the constant_folding_c has had a chance to fill in the constant value
 *       of this symbol. 
 *       For this reason only, I have opted to let this abstract syntax utility have its own private copy of the
 *       extract_integer() function.
 *       Another aspect that makes this OK is that this function will only be used to extract the integer value of the
 *       index for the first extensible paramater (examples follow shortly). Since this is an extension to IEC 61131-3 
 *       that we created to allow us to handle extensible functions with very little hard coding, it is OK if we
 *       impose extra/different limits on how an integer may be legally be formated in this case. This will also 
 *       only show up in code that describes the interface to the standard function of IEC 61131-3, which the user
 *       will not ever get to see. We write that IEC 61131-3 code ourselves!
 *
 *      Example of source code we will be parsing and analysing:
 *
 *      FUNCTION ADD : REAL VAR_INPUT IN 1 .. : REAL; END_VAR RETURN; END_FUNCTION
 *                                      ^^^
 *
 *      FUNCTION MUX : REAL VAR_INPUT K : USINT; IN 0 .. : REAL; END_VAR RETURN; END_FUNCTION
 *                                                 ^^^
 *
 *      Basically, currently this will only be either a '0' or a '1' !!
 */

/* NOTE: it must ignore underscores! */
static int extract_first_index_value(symbol_c *sym) {
  std::string str = "";
  integer_c *integer;
  long int ret;

  if ((integer = dynamic_cast<integer_c *>(sym)) == NULL) ERROR;
  for(unsigned int i = 0; i < strlen(integer->value); i++)
    if (integer->value[i] != '_')  str += integer->value[i];

  errno = 0; // since strtoXX() may legally return 0, we must set errno to 0 to detect errors correctly!
  ret = strtol(str.c_str(), NULL, 10);
  if (errno != 0) ERROR;
  if (ret < 0) ERROR; // the following code assumes that the first index will never be negative!
  if (ret > std::numeric_limits< int >::max()) ERROR; // output of this function is only an int!!

  return ret;
}










/* compare the name of two __extensible__ function parameters.
 * The usual use case is to have one of the parameters as used
 * in the function declaration, and another as used in a formal function call.
 *
 * Will return:
 *         < 0 : if two parameters are not compatible, or one is invalid
 *        >= 0 : if both parameters ..........
 */
/*
 *   ("in", "i0")      -> returns error (<0)
 *   ("in1", "in")     -> returns error (<0)
 *   ("in", "in")      -> returns error (<0)
 *   ("in", "inw")     -> returns error (<0)
 *   ("in", "in10.4")  -> returns error (<0)
 *   ("in", "in10e")   -> returns error (<0)
 *   ("in", "")        -> returns error (<0)
 *   ("", "in10e")     -> returns error (<0)
 *   ("in", "in0")     -> returns 0
 *   ("in", "in9")     -> returns 9
 *   ("in", "in42")    -> returns 42
 *   ("in", "in-42")   -> returns -42 (error!)
 */
int function_param_iterator_c::cmp_extparam_names(const char* s1, const char* s2) {
  int res;
  char *endptr;
  int len;

  if ((s1 == NULL) || (s2 == NULL) || (*s1 == '\0') || (*s2 == '\0')) return -1;

  len = strlen(s1);
  if (strncasecmp(s1, s2, len)) return -2;

  s1 = &s2[len];
  if (*s1 == '\0') return -3;

  res = strtol(s1, &endptr, 10);
  if (*endptr != '\0') return -4;

  return res;
}



void* function_param_iterator_c::handle_param_list(list_c *list) {
  switch (current_operation) {
    case iterate_op:
      if (next_param <= param_count + list->n)
        return list->elements[next_param - param_count - 1];

      /* the desired param is not on this list... */
      param_count += list->n;
      break;

    case search_op:
      for(int i = 0; i < list->n; i++) {
        symbol_c *sym = list->elements[i];
        extensible_input_parameter_c *extensible_parameter = dynamic_cast<extensible_input_parameter_c *>(sym);
        if (extensible_parameter != NULL) {
          sym = extensible_parameter->var_name;
          current_param_is_extensible = true;
          _first_extensible_param_index = extract_first_index_value(extensible_parameter->first_index);
        }
        identifier_c *variable_name = dynamic_cast<identifier_c *>(sym);
        if (variable_name == NULL) ERROR;
        
        if (!current_param_is_extensible)
          if (strcasecmp(search_param_name->value, variable_name->value) == 0)
            /* FOUND! This is the same parameter!! */
            return (void *)variable_name;
  
        if (current_param_is_extensible) {
          current_extensible_param_index = cmp_extparam_names(variable_name->value, search_param_name->value);
          if (current_extensible_param_index >= 0)
            /* FOUND! This is a compatible extensible parameter!! */
            return (void *)variable_name;
        }  
      }
      break;
  } /* switch */

  /* Not found! */
  return NULL;
}

void* function_param_iterator_c::handle_single_param(symbol_c *var_name) {
  switch (current_operation) {
    case iterate_op:
      param_count++;
      if (next_param == param_count)
        return var_name;
      break;

    case search_op:
      extensible_input_parameter_c *extensible_parameter = dynamic_cast<extensible_input_parameter_c *>(var_name);
      if (extensible_parameter != NULL) {
        var_name = extensible_parameter->var_name;
        current_param_is_extensible = true;
        _first_extensible_param_index = extract_first_index_value(extensible_parameter->first_index);
      }
      identifier_c *variable_name = dynamic_cast<identifier_c *>(var_name);
      if (variable_name == NULL) ERROR;
      
      if (!current_param_is_extensible)
        if (strcasecmp(search_param_name->value, variable_name->value) == 0)
          /* FOUND! This is the same parameter!! */
          return (void *)variable_name;

      if (current_param_is_extensible) {
        current_extensible_param_index = cmp_extparam_names(variable_name->value, search_param_name->value);
        if (current_extensible_param_index >= 0)
          /* FOUND! This is a compatible extensible parameter!! */
          return (void *)variable_name;
      }  
      break;
  } /* switch */

  /* Not found! */
  return NULL;
}

void* function_param_iterator_c::iterate_list(list_c *list) {
  void *res;
  for (int i = 0; i < list->n; i++) {
    res = list->elements[i]->accept(*this);
    if (res != NULL)
        return res;
  }
  return NULL;
}

/* start off at the first parameter once again... */
void function_param_iterator_c::reset(void) {
  next_param = param_count = 0;
  _first_extensible_param_index = -1;
  current_param_is_extensible = false;
  current_param_name = NULL;
  current_param_type = NULL;
  current_param_default_value = NULL;
  last_returned_parameter = NULL; /* the last parameter returned by search() or next() */
}


/* initialise the iterator object.
 * We must be given a reference to one of the following
 *     - function_declaration_c
 *     - function_block_declaration_c
 *     - program_declaration_c
 * that will be analysed...
 */
function_param_iterator_c::function_param_iterator_c(symbol_c *pou_decl) {
  /* do some consistency checks... */
  function_declaration_c       * f_decl = dynamic_cast<function_declaration_c       *>(pou_decl);
  function_block_declaration_c *fb_decl = dynamic_cast<function_block_declaration_c *>(pou_decl);
  program_declaration_c        * p_decl = dynamic_cast<program_declaration_c        *>(pou_decl);

  if ((NULL == f_decl) && (NULL == fb_decl) && (NULL == p_decl)) 
    ERROR;

  /* OK. Now initialise this object... */
  this->f_decl = pou_decl;
  reset();
}



/* Skip to the next parameter. After object creation,
 * the object references on parameter _before_ the first, so
 * this function must be called once to get the object to
 * reference the first parameter...
 *
 * Returns the parameter's name!
 */
identifier_c *function_param_iterator_c::next(void) {
  void *res;
  identifier_c *identifier;
 
  if (current_param_is_extensible) {
    current_extensible_param_index++;
    return current_param_name;
  }
  
  last_returned_parameter = NULL; 
  param_count = 0;
  en_eno_param_implicit = false;
  next_param++;
  current_operation = function_param_iterator_c::iterate_op;
  res = f_decl->accept(*this);
  if (res == NULL) 
    return NULL;

  symbol_c *sym = (symbol_c *)res;
  extensible_input_parameter_c *extensible_parameter = dynamic_cast<extensible_input_parameter_c *>(sym);
  if (extensible_parameter != NULL) {
    sym = extensible_parameter->var_name;
    current_param_is_extensible = true;
    _first_extensible_param_index = extract_first_index_value(extensible_parameter->first_index);
    current_extensible_param_index = _first_extensible_param_index;
  }
  identifier = dynamic_cast<identifier_c *>(sym);
  if (identifier == NULL)
    ERROR;
  current_param_name = identifier;
  last_returned_parameter = current_param_name; 
  return current_param_name;
}

/* Search for the value passed to the parameter named <param_name>...  */
identifier_c *function_param_iterator_c::search(symbol_c *param_name) {
  if (NULL == param_name) ERROR;
  search_param_name = dynamic_cast<identifier_c *>(param_name);
  if (NULL == search_param_name) ERROR;
  en_eno_param_implicit = false;
  current_param_is_extensible = false;
  current_operation = function_param_iterator_c::search_op;
  void *res = f_decl->accept(*this);
  identifier_c *res_param_name = dynamic_cast<identifier_c *>((symbol_c *)res);
  last_returned_parameter = res_param_name; 
  return res_param_name;
}

identifier_c *function_param_iterator_c::search(const char *param_name) {
  identifier_c   param_name_id(param_name);
  return search(&param_name_id);
}



/* Returns the currently referenced parameter's default value,
 * or NULL if none is specified in the function declrataion itself.
 */
symbol_c *function_param_iterator_c::default_value(void) {
  if (NULL == last_returned_parameter) 
    return NULL;
  return current_param_default_value;
}

/* Returns the currently referenced parameter's type name. */
symbol_c *function_param_iterator_c::param_type(void) {
  if (NULL == last_returned_parameter) 
    return NULL;
  return current_param_type;
}

/* Returns if currently referenced parameter is an implicit defined EN/ENO parameter. */
bool function_param_iterator_c::is_en_eno_param_implicit(void) {
  if (NULL == last_returned_parameter) 
    ERROR;
  return en_eno_param_implicit;
}

/* Returns if currently referenced parameter is an extensible parameter. */
/* extensible paramters only occur in some standard functions, e.g. AND(word#34, word#44, word#65); */
bool function_param_iterator_c::is_extensible_param(void) {
  if (NULL == last_returned_parameter) 
    ERROR;
  return current_param_is_extensible;
}

/* Returns the index of the current extensible parameter. */             
/* If the current parameter is not an extensible paramter, returns -1 */
int function_param_iterator_c::extensible_param_index(void) {
  if (NULL == last_returned_parameter) 
    ERROR;
  return (current_param_is_extensible? current_extensible_param_index : -1);
}

/* Returns the index of the first extensible parameter, or -1 if no extensible parameter found. */             
/* WARNING: Will only return the correct value _after_ an extensible parameter has been found! */
int function_param_iterator_c::first_extensible_param_index(void) {
  return _first_extensible_param_index;
}

/* Returns the currently referenced parameter's data passing direction.
 * i.e. VAR_INPUT, VAR_OUTPUT or VAR_INOUT
 */
function_param_iterator_c::param_direction_t function_param_iterator_c::param_direction(void) {
  if (NULL == last_returned_parameter) 
    ERROR;
  return current_param_direction;
}

void *function_param_iterator_c::visit(implicit_definition_c *symbol) {
	en_eno_param_implicit = true;
	return NULL;
}

/****************************************/
/* 1.4.3 - Declaration & Initialisation */
/****************************************/
void *function_param_iterator_c::visit(input_declarations_c *symbol) {
  TRACE("input_declarations_c");
  current_param_direction = direction_in;
  return symbol->input_declaration_list->accept(*this);
}

void *function_param_iterator_c::visit(input_declaration_list_c *symbol) {TRACE("input_declaration_list_c"); return iterate_list(symbol);}

void *function_param_iterator_c::visit(edge_declaration_c *symbol) {TRACE("edge_declaration_c"); return symbol->var1_list->accept(*this);}

void *function_param_iterator_c::visit(en_param_declaration_c *symbol) {
  TRACE("en_param_declaration_c");
  /* It is OK to store these values in the current_param_XXX
   * variables, because if the desired parameter is not in the
   * variable list we will be analysing, the current_param_XXXX
   * variables will get overwritten when we visit the next
   * var1_init_decl_c list!
   */
  current_param_default_value = spec_init_sperator_c::get_init(symbol->type_decl);
  current_param_type = spec_init_sperator_c::get_spec(symbol->type_decl);
  
  void *res = handle_single_param(symbol->name);
  
    /* If we have found the parameter we will be returning, we set the en_eno_param_implicit to TRUE if implicitly defined */
  if (res != NULL) symbol->method->accept(*this);
  
  return res;
}

/* var1_list ':' array_spec_init */
//SYM_REF2(array_var_init_decl_c, var1_list, array_spec_init)
void *function_param_iterator_c::visit(array_var_init_decl_c *symbol) {
  TRACE("array_var_init_decl_c");
  current_param_default_value = spec_init_sperator_c::get_init(symbol->array_spec_init);
  current_param_type = spec_init_sperator_c::get_spec(symbol->array_spec_init);

  return symbol->var1_list->accept(*this);
}

/*  var1_list ':' initialized_structure */
//SYM_REF2(structured_var_init_decl_c, var1_list, initialized_structure)
void *function_param_iterator_c::visit(structured_var_init_decl_c *symbol) {
  TRACE("structured_var_init_decl_c");

  current_param_default_value = spec_init_sperator_c::get_init(symbol->initialized_structure);
  current_param_type = spec_init_sperator_c::get_spec(symbol->initialized_structure);

  return symbol->var1_list->accept(*this);
}




/* fb_name_list ':' function_block_type_name ASSIGN structure_initialization */
/* structure_initialization -> may be NULL ! */
// SYM_REF3(fb_name_decl_c, fb_name_list, function_block_type_name, structure_initialization)
void *function_param_iterator_c::visit(fb_name_decl_c *symbol) {
  TRACE("structured_var_init_decl_c");
  current_param_default_value = spec_init_sperator_c::get_init(symbol->fb_spec_init); 
  current_param_type          = spec_init_sperator_c::get_spec(symbol->fb_spec_init);

  return symbol->fb_name_list->accept(*this);
}


/* fb_name_list ',' fb_name */
// SYM_LIST(fb_name_list_c)
void *function_param_iterator_c::visit(fb_name_list_c *symbol) {TRACE("fb_name_list_c"); return handle_param_list(symbol);}


void *function_param_iterator_c::visit(output_declarations_c *symbol) {
  TRACE("output_declarations_c");
  current_param_direction = direction_out;
  return symbol->var_init_decl_list->accept(*this);
}

void *function_param_iterator_c::visit(eno_param_declaration_c *symbol) {
  TRACE("eno_param_declaration_c");
  /* It is OK to store these values in the current_param_XXX
   * variables, because if the desired parameter is not in the
   * variable list we will be analysing, the current_param_XXXX
   * variables will get overwritten when we visit the next
   * var1_init_decl_c list!
   */
  current_param_default_value = NULL;
  current_param_type = symbol->type;

  void *res = handle_single_param(symbol->name);
  
    /* If we have found the parameter we will be returning, we set the en_eno_param_implicit to TRUE if implicitly defined */
  if (res != NULL) symbol->method->accept(*this);
  
  return res;
}

void *function_param_iterator_c::visit(input_output_declarations_c *symbol) {
  TRACE("input_output_declarations_c");
  current_param_direction = direction_inout;
  return symbol->var_declaration_list->accept(*this);
}

void *function_param_iterator_c::visit(var_declaration_list_c *symbol) {TRACE("var_declaration_list_c"); return iterate_list(symbol);}

/*  var1_list ':' array_specification */
//SYM_REF2(array_var_declaration_c, var1_list, array_specification)
void *function_param_iterator_c::visit(array_var_declaration_c *symbol) {
  TRACE("array_var_declaration_c");
  current_param_default_value = NULL;
  current_param_type = symbol->array_specification;
  return symbol->var1_list->accept(*this);
}

/*  var1_list ':' structure_type_name */
//SYM_REF2(structured_var_declaration_c, var1_list, structure_type_name)
void *function_param_iterator_c::visit(structured_var_declaration_c *symbol) {
  TRACE("structured_var_declaration_c");
  current_param_default_value = NULL;
  current_param_type = symbol->structure_type_name;
  return symbol->var1_list->accept(*this);
}

/* VAR [CONSTANT] var_init_decl_list END_VAR */
void *function_param_iterator_c::visit(var_declarations_c *symbol) {TRACE("var_declarations_c"); return NULL;}

/*| VAR_EXTERNAL [CONSTANT] external_declaration_list END_VAR */
/* option -> may be NULL ! */
// SYM_REF2(external_var_declarations_c, option, external_declaration_list)
void *function_param_iterator_c::visit(external_var_declarations_c *symbol) {
  TRACE("external_var_declarations_c");
  current_param_direction = direction_extref;
  return symbol->external_declaration_list->accept(*this);
}

/* helper symbol for external_var_declarations */
/*| external_declaration_list external_declaration';' */
// SYM_LIST(external_declaration_list_c)
void *function_param_iterator_c::visit(external_declaration_list_c *symbol) {TRACE("external_declaration_list_c"); return iterate_list(symbol);}

/*  global_var_name ':' (simple_specification|subrange_specification|enumerated_specification|array_specification|prev_declared_structure_type_name|function_block_type_name */
//SYM_REF2(external_declaration_c, global_var_name, specification)
void *function_param_iterator_c::visit(external_declaration_c *symbol) {
  TRACE("external_declaration_c");
  /* It is OK to store these values in the current_param_XXX
   * variables, because if the desired parameter is not in the
   * variable list we will be analysing, the current_param_XXXX
   * variables will get overwritten when we visit the next
   * var1_init_decl_c list!
   */
  current_param_default_value = spec_init_sperator_c::get_init(symbol->specification);
  current_param_type = spec_init_sperator_c::get_spec(symbol->specification);

  return handle_single_param(symbol->global_var_name);
}


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


void *function_param_iterator_c::visit(var1_init_decl_c *symbol) {
  TRACE("var1_init_decl_c");
  /* It is OK to store these values in the current_param_XXX
   * variables, because if the desired parameter is not in the
   * variable list we will be analysing, the current_param_XXXX
   * variables will get overwritten when we visit the next
   * var1_init_decl_c list!
   */
  current_param_default_value = spec_init_sperator_c::get_init(symbol->spec_init);
  current_param_type = spec_init_sperator_c::get_spec(symbol->spec_init);

  return symbol->var1_list->accept(*this);
}


void *function_param_iterator_c::visit(var1_list_c *symbol) {
  TRACE("var1_list_c");
  return handle_param_list(symbol);
}


void *function_param_iterator_c::visit(var_init_decl_list_c *symbol) {TRACE("var_init_decl_list_c"); return iterate_list(symbol);}


/***********************/
/* B 1.5.1 - Functions */
/***********************/
void *function_param_iterator_c::visit(function_declaration_c *symbol) {TRACE("function_declaration_c"); return symbol->var_declarations_list->accept(*this);}
/* intermediate helper symbol for function_declaration */
void *function_param_iterator_c::visit(var_declarations_list_c *symbol) {TRACE("var_declarations_list_c"); return iterate_list(symbol);}
void *function_param_iterator_c::visit(function_var_decls_c *symbol) {TRACE("function_var_decls_c"); /* ignore */ return NULL;}


/*****************************/
/* B 1.5.2 - Function Blocks */
/*****************************/
/*  FUNCTION_BLOCK derived_function_block_name io_OR_other_var_declarations function_block_body END_FUNCTION_BLOCK */
void *function_param_iterator_c::visit(function_block_declaration_c *symbol) {TRACE("function_block_declaration_c"); return symbol->var_declarations->accept(*this);}

/* intermediate helper symbol for function_declaration */
/*  { io_var_declarations | other_var_declarations }   */
/*
 * NOTE: we re-use the var_declarations_list_c
 */

/*  VAR_TEMP temp_var_decl_list END_VAR */
void *function_param_iterator_c::visit(temp_var_decls_c *symbol) {TRACE("temp_var_decls_c"); /* ignore */ return NULL;}
void *function_param_iterator_c::visit(temp_var_decls_list_c *symbol) {TRACE("temp_var_decls_list_c"); /* ignore */ return NULL;}

/*  VAR NON_RETAIN var_init_decl_list END_VAR */
void *function_param_iterator_c::visit(non_retentive_var_decls_c *symbol) {TRACE("non_retentive_var_decls_c"); /* ignore */ return NULL;}


/**********************/
/* B 1.5.3 - Programs */
/**********************/
/*  PROGRAM program_type_name program_var_declarations_list function_block_body END_PROGRAM */
// SYM_REF4(program_declaration_c, program_type_name, var_declarations, function_block_body, unused)
void *function_param_iterator_c::visit(program_declaration_c *symbol) {TRACE("program_declaration_c"); return symbol->var_declarations->accept(*this);}

/* intermediate helper symbol for program_declaration_c */
/*  { io_var_declarations | other_var_declarations }   */
/*
 * NOTE: we re-use the var_declarations_list_c
 */






