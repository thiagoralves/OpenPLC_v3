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

class generate_c_configbody_c: public generate_c_base_and_typeid_c {

  public:
    generate_c_configbody_c(stage4out_c *s4o_ptr)
    : generate_c_base_and_typeid_c(s4o_ptr) {
      current_resource_name = NULL;
    }

    virtual ~generate_c_configbody_c(void) {}


  private:
    /* The name of the resource curretnly being processed... */
    symbol_c *current_resource_name;


/********************************/
/* B 1.7 Configuration elements */
/********************************/


public:
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
void *visit(configuration_declaration_c *symbol) {
  TRACE("configuration_declaration_c");

  symbol->resource_declarations->accept(*this);
  return NULL;
}



/* helper symbol for configuration_declaration */
// SYM_LIST(resource_declaration_list_c)
void *visit(resource_declaration_list_c *symbol) {
  TRACE("resource_declaration_list_c");

  return print_list(symbol);
}

/*
RESOURCE resource_name ON resource_type_name
   optional_global_var_declarations
   single_resource_declaration
END_RESOURCE
*/
// SYM_REF4(resource_declaration_c, resource_name, resource_type_name, global_var_declarations, resource_declaration)
void *visit(resource_declaration_c *symbol) {
  TRACE("resource_declaration_c");

  current_resource_name = symbol->resource_name;
  symbol->resource_declaration->accept(*this);
  current_resource_name = NULL;
  return NULL;
}



/* task_configuration_list program_configuration_list */
// SYM_REF2(single_resource_declaration_c, task_configuration_list, program_configuration_list)
void *visit(single_resource_declaration_c *symbol) {
  TRACE("single_resource_declaration_c");

  symbol->program_configuration_list->accept(*this);
  return NULL;
}



/* helper symbol for single_resource_declaration */
//SYM_LIST(task_configuration_list_c)


/* helper symbol for single_resource_declaration */
//SYM_LIST(program_configuration_list_c)
void *visit(program_configuration_list_c *symbol) {
  TRACE("program_configuration_list_c");

  return print_list(symbol);
}


/* helper symbol for
 *  - access_path
 *  - instance_specific_init
 */
//SYM_LIST(any_fb_name_list_c)

/*  [resource_name '.'] global_var_name ['.' structure_element_name] */
//SYM_REF4(global_var_reference_c, resource_name, global_var_name, structure_element_name, unused)

/*  prev_declared_program_name '.' symbolic_variable */
//SYM_REF2(program_output_reference_c, program_name, symbolic_variable)

/*  TASK task_name task_initialization */
//SYM_REF2(task_configuration_c, task_name, task_initialization)

/*  '(' [SINGLE ASSIGN data_source ','] [INTERVAL ASSIGN data_source ','] PRIORITY ASSIGN integer ')' */
//SYM_REF4(task_initialization_c, single_data_source, interval_data_source, priority_data_source, unused)

/*  PROGRAM [RETAIN | NON_RETAIN] program_name [WITH task_name] ':' program_type_name ['(' prog_conf_elements ')'] */
//SYM_REF6(program_configuration_c, retain_option, program_name, task_name, program_type_name, prog_conf_elements, unused)
void *visit(program_configuration_c *symbol) {
  TRACE("program_configuration_c");

  s4o.print(s4o.indent_spaces);
  if (NULL != current_resource_name) {
    current_resource_name->accept(*this);
    s4o.print(".");
  }
  symbol->program_name->accept(*this);
  s4o.print(".f();\n");
  return NULL;
}

/* prog_conf_elements ',' prog_conf_element */
//SYM_LIST(prog_conf_elements_c)

/*  fb_name WITH task_name */
//SYM_REF2(fb_task_c, fb_name, task_name)
// TODO...

/*  any_symbolic_variable ASSIGN prog_data_source */
//SYM_REF2(prog_cnxn_assign_c, symbolic_variable, prog_data_source)

/* any_symbolic_variable SENDTO data_sink */
//SYM_REF2(prog_cnxn_sendto_c, symbolic_variable, prog_data_source)

/* VAR_CONFIG instance_specific_init_list END_VAR */
//SYM_REF2(instance_specific_initializations_c, instance_specific_init_list, unused)

/* helper symbol for instance_specific_initializations */
//SYM_LIST(instance_specific_init_list_c)

/* resource_name '.' program_name '.' {fb_name '.'}
    ((variable_name [location] ':' located_var_spec_init) | (fb_name ':' fb_initialization))
*/
//SYM_REF6(instance_specific_init_c, resource_name, program_name, any_fb_name_list, variable_name, location, initialization)

/* helper symbol for instance_specific_init */
/* function_block_type_name ':=' structure_initialization */
//SYM_REF2(fb_initialization_c, function_block_type_name, structure_initialization)



}; /* generate_c_configbody_c */


