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
 * Function call parameter iterator.
 * It will iterate through the non-formal parameters of a function call
 * (i.e. function calls using the foo(<param1>, <param2>, ...) syntax).
 * and/or search through the formal parameters of a function call
 * (i.e. function calls using the foo(<name1> = <param1>, <name2> = <param2>, ...) syntax).
 *
 * Calls to function blocks and programs are also supported.
 *
 * Note that calls to next_nf() will only iterate through non-formal parameters,
 * calls to next_f() will only iterate through formal parameters,
 * and calls to search_f() will only serach through formal parameters.
 */



#include "function_call_param_iterator.hh"
#include <strings.h>
#include "../main.hh" // required for ERROR() and ERROR_MSG() macros.


//#define DEBUG
#ifdef DEBUG
#define TRACE(classname) printf("\n____%s____\n",classname);
#else
#define TRACE(classname)
#endif






void *function_call_param_iterator_c::search_list(list_c *list) {
  switch (current_operation) {
    case iterate_nf_op:
      for(int i = 0; i < list->n; i++) {
        void *res = list->elements[i]->accept(*this);
        if (NULL != res) {
          /* It went through the handle_parameter_assignment() function,
           * and is therefore a parameter assignment (<param> = <value>),
           * and not a simple expression (<value>).
           */
          /* we do nothing... */
        } else {
          param_count++;
          if (param_count == iterate_nf_next_param) {
            return list->elements[i];
          }
        }
      }
      return NULL;
      break;

    case iterate_f_op:
      for(int i = 0; i < list->n; i++) {
        void *res = list->elements[i]->accept(*this);
        if (NULL != res) {
          /* It went through the handle_parameter_assignment() function,
           * and is therefore a parameter assignment (<param> = <value>),
           * and not a simple expression (<value>).
           */
          param_count++;
          if (param_count == iterate_f_next_param) {
            return res;
          }
        } else {
          /* we do nothing... */
        }
      }
      return NULL;
      break;

    case search_f_op:
      for(int i = 0; i < list->n; i++) {
        void *res = list->elements[i]->accept(*this);
        if (res != NULL)
          return res;
      }
      return NULL;
      break;
  } /* switch */
  return NULL;
}



void *function_call_param_iterator_c::handle_parameter_assignment(symbol_c *variable_name, symbol_c *expression) {
  switch (current_operation) {
    case iterate_nf_op:
          /* UGLY HACK -> this will be detected in the search_list() function */
      return (void *)variable_name; /* anything, as long as it is not NULL!! */
      break;

    case iterate_f_op:
      current_value = expression;
      return (void *)variable_name;
      break;

    case search_f_op:
      identifier_c *variable_name2 = dynamic_cast<identifier_c *>(variable_name);

      if (variable_name2 == NULL) ERROR;

      if (strcasecmp(search_param_name->value, variable_name2->value) == 0)
        /* FOUND! This is the same parameter!! */
        return (void *)expression;
      return NULL;
      break;
  }

  ERROR;
  return NULL;
}


/* start off at the first parameter once again... */
void function_call_param_iterator_c::reset(void) {
  iterate_nf_next_param = 0;
  iterate_f_next_param  = 0;
  param_count = 0;
}

/* initialise the iterator object.
 * We must be given a reference to the function/program/function block call
 * that will be analysed...
 */
function_call_param_iterator_c::function_call_param_iterator_c(symbol_c *f_call) {
  /* It is expected that f_call will reference one of the following:
   *  program_configuration_c
   *  function_invocation_c
   *  fb_invocation_c
   *  il_function_call_c
   *  il_formal_funct_call_c
   *  ... (have I missed any?)
   */
  this->f_call = f_call;
  search_param_name = NULL;
  reset();
}

/* Skip to the next formal parameter. After object creation,
 * the object references on parameter _before_ the first, so
 * this function must be called once to get the object to
 * reference the first parameter...
 *
 * Returns the paramater name to which a value is being passed!
 * You can determine the value being passed by calling 
 * function_call_param_iterator_c::search_f()
 */
symbol_c *function_call_param_iterator_c::next_f(void) {
  current_value = NULL;
  current_assign_direction = assign_none;
  param_count = 0;
  iterate_f_next_param++;
  current_operation = function_call_param_iterator_c::iterate_f_op;
  void *res = f_call->accept(*this);
  return (symbol_c *)res;
}


/* Skip to the next non-formal parameter. After object creation,
 * the object references on parameter _before_ the first, so
 * this function must be called once to get the object to
 * reference the first parameter...
 *
 * Returns whatever is being passed to the parameter!
 */
symbol_c *function_call_param_iterator_c::next_nf(void) {
  current_value = NULL;
  current_assign_direction = assign_none;
  param_count = 0;
  iterate_nf_next_param++;
  current_operation = function_call_param_iterator_c::iterate_nf_op;
  void *res = f_call->accept(*this);
  current_value = (symbol_c *)res;
  return (symbol_c *)res;
}

/* Search for the value passed to the parameter named <param_name>...  */
symbol_c *function_call_param_iterator_c::search_f(symbol_c *param_name) {
  current_value = NULL;
  current_assign_direction = assign_none;
  if (NULL == param_name) ERROR;
  search_param_name = dynamic_cast<identifier_c *>(param_name);
  if (NULL == search_param_name) ERROR;
  current_operation = function_call_param_iterator_c::search_f_op;
  void *res = f_call->accept(*this);
  current_value = (symbol_c *)res;
  return (symbol_c *)res;
}

/* Search for the value passed to the parameter named <param_name>...  */
symbol_c *function_call_param_iterator_c::search_f(const char *param_name) {
  identifier_c tmp_indentifier(param_name);
  return search_f(&tmp_indentifier);
}


/* Returns the value being passed to the current parameter. */
symbol_c *function_call_param_iterator_c::get_current_value(void) {
  return current_value;
}

/* Returns the value being passed to the current parameter. */
function_call_param_iterator_c::assign_direction_t function_call_param_iterator_c::get_assign_direction(void) {
  return current_assign_direction;
}


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

/* helper symbol for configuration_declaration */
/*
SYM_LIST(resource_declaration_list_c)
*/

/*
RESOURCE resource_name ON resource_type_name
   optional_global_var_declarations
   single_resource_declaration
END_RESOURCE
*/
/*
SYM_REF4(resource_declaration_c, resource_name, resource_type_name, global_var_declarations, resource_declaration)
*/

/* task_configuration_list program_configuration_list */
/*
SYM_REF2(single_resource_declaration_c, task_configuration_list, program_configuration_list)
*/

/* helper symbol for single_resource_declaration */
/*
SYM_LIST(task_configuration_list_c)
*/

/* helper symbol for single_resource_declaration */
/*
SYM_LIST(program_configuration_list_c)
*/

/* helper symbol for
 *  - access_path
 *  - instance_specific_init
 */
/*
SYM_LIST(any_fb_name_list_c)
*/

/*  [resource_name '.'] global_var_name ['.' structure_element_name] */
/*
SYM_REF4(global_var_reference_c, resource_name, global_var_name, structure_element_name, unused)
*/

/*  prev_declared_program_name '.' symbolic_variable */
/*
SYM_REF2(program_output_reference_c, program_name, symbolic_variable)
*/

/*  TASK task_name task_initialization */
/*
SYM_REF2(task_configuration_c, task_name, task_initialization)
*/

/*  '(' [SINGLE ASSIGN data_source ','] [INTERVAL ASSIGN data_source ','] PRIORITY ASSIGN integer ')' */
/*
SYM_REF4(task_initialization_c, single_data_source, interval_data_source, priority_data_source, unused)
*/

/*  PROGRAM [RETAIN | NON_RETAIN] program_name [WITH task_name] ':' program_type_name ['(' prog_conf_elements ')'] */
// SYM_REF6(program_configuration_c, retain_option, program_name, task_name, program_type_name, prog_conf_elements, unused)
void *function_call_param_iterator_c::visit(program_configuration_c *symbol) {
  TRACE("program_configuration_c");
  return symbol->prog_conf_elements->accept(*this);
}

/* prog_conf_elements ',' prog_conf_element */
// SYM_LIST(prog_conf_elements_c)
void *function_call_param_iterator_c::visit(prog_conf_elements_c *symbol) {
  TRACE("prog_conf_elements_c");
  return search_list(symbol);
}

/*  fb_name WITH task_name */
/*
SYM_REF2(fb_task_c, fb_name, task_name)
*/

/*  any_symbolic_variable ASSIGN prog_data_source */
// SYM_REF2(prog_cnxn_assign_c, symbolic_variable, prog_data_source)
void *function_call_param_iterator_c::visit(prog_cnxn_assign_c *symbol) {
  TRACE("prog_cnxn_assign_c");

  /* NOTE: symbolic_variable may be something other than a symbolic_variable_c, but I (Mario)
   *       do not understand the semantics that should be implmeneted if it is not a
   *        symbolic_variable, so for the moment we simply give up!
   */
  symbolic_variable_c *symb_var = dynamic_cast<symbolic_variable_c *>(symbol->symbolic_variable);
  if (NULL == symb_var)
    ERROR;

  current_assign_direction = assign_in;
  return handle_parameter_assignment(symb_var->var_name, symbol->prog_data_source);
}

/* any_symbolic_variable SENDTO data_sink */
// SYM_REF2(prog_cnxn_sendto_c, symbolic_variable, prog_data_source)
void *function_call_param_iterator_c::visit(prog_cnxn_sendto_c *symbol) {
  TRACE("prog_cnxn_sendto_c");

  /* NOTE: symbolic_variable may be something other than a symbolic_variable_c, but I (Mario)
   *       do not understand the semantics that should be implmeneted if it is not a
   *        symbolic_variable, so for the moment we simply give up!
   */
  symbolic_variable_c *symb_var = dynamic_cast<symbolic_variable_c *>(symbol->symbolic_variable);
  if (NULL == symb_var)
    ERROR;

  current_assign_direction = assign_out;
  return handle_parameter_assignment(symb_var->var_name, symbol->data_sink);
}

/* VAR_CONFIG instance_specific_init_list END_VAR */
/*
SYM_REF2(instance_specific_initializations_c, instance_specific_init_list, unused)
*/

/* helper symbol for instance_specific_initializations */
/*
SYM_LIST(instance_specific_init_list_c)
*/

/* resource_name '.' program_name '.' {fb_name '.'}
    ((variable_name [location] ':' located_var_spec_init) | (fb_name ':' fb_initialization))
*/
/*
SYM_REF6(instance_specific_init_c, resource_name, program_name, any_fb_name_list, variable_name, location, initialization)
*/

/* helper symbol for instance_specific_init */
/* function_block_type_name ':=' structure_initialization */
/*
SYM_REF2(fb_initialization_c, function_block_type_name, structure_initialization)
*/



/****************************************/
/* B.2 - Language IL (Instruction List) */
/****************************************/
/***********************************/
/* B 2.1 Instructions and Operands */
/***********************************/

/* | function_name [il_operand_list] */
// SYM_REF2(il_function_call_c, function_name, il_operand_list)
void *function_call_param_iterator_c::visit(il_function_call_c *symbol) {
  TRACE("il_function_call_c");
  if (NULL != symbol->il_operand_list)
    return symbol->il_operand_list->accept(*this);
  return NULL;
}


/* | function_name '(' eol_list [il_param_list] ')' */
// SYM_REF2(il_formal_funct_call_c, function_name, il_param_list)
void *function_call_param_iterator_c::visit(il_formal_funct_call_c *symbol) {
  TRACE("il_formal_funct_call_c");
  if (NULL != symbol->il_param_list)
    return symbol->il_param_list->accept(*this);
  return NULL;
}


/*   il_call_operator prev_declared_fb_name
 * | il_call_operator prev_declared_fb_name '(' ')'
 * | il_call_operator prev_declared_fb_name '(' eol_list ')'
 * | il_call_operator prev_declared_fb_name '(' il_operand_list ')'
 * | il_call_operator prev_declared_fb_name '(' eol_list il_param_list ')'
 */
// SYM_REF4(il_fb_call_c, il_call_operator, fb_name, il_operand_list, il_param_list)
void *function_call_param_iterator_c::visit(il_fb_call_c *symbol) {
  TRACE("il_fb_call_c");
  /* the following should never occur. In reality the syntax parser
   * will guarantee that they never occur, but it makes it easier to
   * understand the remaining code :-)
   */
  //if ((NULL == symbol->il_operand_list) && (NULL == symbol->il_param_list)) ERROR;
  //if ((NULL != symbol->il_operand_list) && (NULL != symbol->il_param_list)) ERROR;

  if (NULL != symbol->il_operand_list)
    return symbol->il_operand_list->accept(*this);
  if (NULL != symbol->il_param_list)
    return symbol->il_param_list->accept(*this);
  return NULL;
}

/* | il_operand_list ',' il_operand */
// SYM_LIST(il_operand_list_c)
void *function_call_param_iterator_c::visit(il_operand_list_c *symbol) {
  TRACE("il_operand_list_c");
  return search_list(symbol);
}


/* | il_initial_param_list il_param_instruction */
// SYM_LIST(il_param_list_c)
void *function_call_param_iterator_c::visit(il_param_list_c *symbol) {
  TRACE("il_param_list_c");
  return search_list(symbol);
}

/*  il_assign_operator il_operand
 * | il_assign_operator '(' eol_list simple_instr_list ')'
 */
// SYM_REF4(il_param_assignment_c, il_assign_operator, il_operand, simple_instr_list, unused)
void *function_call_param_iterator_c::visit(il_param_assignment_c *symbol) {
  TRACE("il_param_assignment_c");

  symbol_c *expression = NULL;
  if (!((NULL != symbol->simple_instr_list) ^ (NULL != symbol->il_operand))) ERROR;
  if    (NULL != symbol->simple_instr_list) expression = symbol->simple_instr_list;
  if    (NULL != symbol->il_operand       ) expression = symbol->il_operand;

  current_assign_direction = assign_in;
  return handle_parameter_assignment((symbol_c *)symbol->il_assign_operator->accept(*this), expression);
}

/*  il_assign_out_operator variable */
// SYM_REF2(il_param_out_assignment_c, il_assign_out_operator, variable);
void *function_call_param_iterator_c::visit(il_param_out_assignment_c *symbol) {
  TRACE("il_param_out_assignment_c");
  current_assign_direction = assign_out;
  return handle_parameter_assignment((symbol_c *)symbol->il_assign_out_operator->accept(*this), symbol->variable);
}


/*******************/
/* B 2.2 Operators */
/*******************/
/*  any_identifier ASSIGN */
// SYM_REF1(il_assign_operator_c, variable_name)
void *function_call_param_iterator_c::visit(il_assign_operator_c *symbol) {
  TRACE("il_assign_operator_c");
  return (void *)symbol->variable_name;
}

/*| [NOT] any_identifier SENDTO */
// SYM_REF2(il_assign_out_operator_c, option, variable_name)
void *function_call_param_iterator_c::visit(il_assign_out_operator_c *symbol) {
  TRACE("il_assign_out_operator_c");

  // TODO : Handle not_param !!!
  // we do not yet support it, so it is best to simply stop than to fail silently...
  // if (NULL != symbol->option) ERROR;

  return (void *)symbol->variable_name;
}




/***************************************/
/* B.3 - Language ST (Structured Text) */
/***************************************/
/***********************/
/* B 3.1 - Expressions */
/***********************/

/*
SYM_REF2(function_invocation_c, function_name, parameter_assignment_list)
*/
void *function_call_param_iterator_c::visit(function_invocation_c *symbol) {
  TRACE("function_invocation_c");
  /* If the syntax parser is working correctly, exactly one of the 
   * following two symbols will be NULL, while the other is != NULL.
   */
  if (symbol == (function_invocation_c *)f_call) {
	if (symbol->   formal_param_list != NULL) return symbol->   formal_param_list->accept(*this);
    if (symbol->nonformal_param_list != NULL) return symbol->nonformal_param_list->accept(*this);
  }

  return NULL;
}


/********************/
/* B 3.2 Statements */
/********************/

/*********************************/
/* B 3.2.1 Assignment Statements */
/*********************************/
/*
SYM_REF2(assignment_statement_c, l_exp, r_exp)
*/

/*****************************************/
/* B 3.2.2 Subprogram Control Statements */
/*****************************************/
/*  RETURN */
// SYM_REF0(return_statement_c)


/* fb_name '(' [param_assignment_list] ')' */
/* param_assignment_list -> may be NULL ! */
// SYM_REF2(fb_invocation_c, fb_name, param_assignment_list)
void *function_call_param_iterator_c::visit(fb_invocation_c *symbol) {
  TRACE("fb_invocation_c");
  /* If the syntax parser is working correctly, only one of the 
   * following two symbols will be != NULL.
   * However, both may be NULL simultaneously!
   */
  if (symbol->   formal_param_list != NULL) return symbol->   formal_param_list->accept(*this);
  if (symbol->nonformal_param_list != NULL) return symbol->nonformal_param_list->accept(*this);

  return NULL;
}

/* helper symbol for fb_invocation */
/* param_assignment_list ',' param_assignment */
// SYM_LIST(param_assignment_list_c)
void *function_call_param_iterator_c::visit(param_assignment_list_c *symbol) {
  TRACE("param_assignment_list_c");
  return search_list(symbol);
}

/*  variable_name ASSIGN expression */
// SYM_REF2(input_variable_param_assignment_c, variable_name, expression)
void *function_call_param_iterator_c::visit(input_variable_param_assignment_c *symbol) {
  TRACE("input_variable_param_assignment_c");
  current_assign_direction = assign_in;
  return handle_parameter_assignment(symbol->variable_name, symbol->expression);
}

/* [NOT] variable_name '=>' variable */
// SYM_REF4(output_variable_param_assignment_c, not_param, variable_name, variable, unused)
void *function_call_param_iterator_c::visit(output_variable_param_assignment_c *symbol) {
  TRACE("output_variable_param_assignment_c");
  // TODO : Handle not_param !!!
  if (NULL != symbol->not_param) ERROR;  // we do not yet support it, so it is best to simply stop than to fail silently...

  current_assign_direction = assign_out;
  return handle_parameter_assignment(symbol->variable_name, symbol->variable);
}

/* helper CLASS for output_variable_param_assignment */
// SYM_REF0(not_paramassign_c)
// TODO... ???


