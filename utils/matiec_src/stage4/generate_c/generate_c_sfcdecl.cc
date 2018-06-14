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

typedef struct
{
  identifier_c *symbol;
} VARIABLE;

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

class generate_c_sfcdecl_c: protected generate_c_base_and_typeid_c {
  
  public:
      typedef enum {
        sfcdecl_sd,
        sfcinit_sd,
        stepcount_sd,
        stepdef_sd,
        stepundef_sd,
        actiondef_sd,
        actionundef_sd,
        actioncount_sd,
        transitioncount_sd
       } sfcdeclaration_t;
  
  private:
    int step_number;
    int action_number;
    int transition_number;
    std::list<VARIABLE> variable_list;
    
    sfcdeclaration_t wanted_sfcdeclaration;

    search_var_instance_decl_c *search_var_instance_decl;
    
  public:
    generate_c_sfcdecl_c(stage4out_c *s4o_ptr, symbol_c *scope, const char *variable_prefix = NULL)
    : generate_c_base_and_typeid_c(s4o_ptr) {
      this->set_variable_prefix(variable_prefix);
      search_var_instance_decl = new search_var_instance_decl_c(scope);
    }
    ~generate_c_sfcdecl_c(void) {
      variable_list.clear();
      delete search_var_instance_decl;
    }
    
    void generate(symbol_c *symbol, sfcdeclaration_t declaration_type) {
      wanted_sfcdeclaration = declaration_type;

      symbol->accept(*this);
    }

/*********************************************/
/* B.1.6  Sequential function chart elements */
/*********************************************/
    
    void *visit(sequential_function_chart_c *symbol) {
      step_number = 0;
      action_number = 0;
      transition_number = 0;
      switch (wanted_sfcdeclaration) {
        case sfcdecl_sd:
          for(int i = 0; i < symbol->n; i++)
            symbol->elements[i]->accept(*this);
          
          /* steps table declaration */
          s4o.print(s4o.indent_spaces + "STEP __step_list[");
          s4o.print(step_number);
          s4o.print("];\n");
          s4o.print(s4o.indent_spaces + "UINT __nb_steps;\n");
          
          /* actions table declaration */
          s4o.print(s4o.indent_spaces + "ACTION __action_list[");
          s4o.print(action_number);
          s4o.print("];\n");
          s4o.print(s4o.indent_spaces + "UINT __nb_actions;\n");
          
          /* transitions table declaration */
          s4o.print(s4o.indent_spaces + "__IEC_BOOL_t __transition_list[");
          s4o.print(transition_number);
          s4o.print("];\n");
          
          /* transitions debug table declaration */
          s4o.print(s4o.indent_spaces + "__IEC_BOOL_t __debug_transition_list[");
          s4o.print(transition_number);
          s4o.print("];\n");
          s4o.print(s4o.indent_spaces + "UINT __nb_transitions;\n");
          
          /* last_ticktime declaration */
          s4o.print(s4o.indent_spaces + "TIME __lasttick_time;\n");
          break;
        case sfcinit_sd:
          s4o.print(s4o.indent_spaces);
          s4o.print("UINT i;\n");
          
          /* steps table count */
          wanted_sfcdeclaration = stepcount_sd;
          for(int i = 0; i < symbol->n; i++)
            symbol->elements[i]->accept(*this);
          s4o.print(s4o.indent_spaces);
          print_variable_prefix();
          s4o.print("__nb_steps = ");
          s4o.print(step_number);
          s4o.print(";\n");
          step_number = 0;
          wanted_sfcdeclaration = sfcinit_sd;
          
          /* steps table initialisation */
          s4o.print(s4o.indent_spaces + "static const STEP temp_step = {{0, 0}, 0, {0, 0}};\n");
          s4o.print(s4o.indent_spaces + "for(i = 0; i < ");
          print_variable_prefix();
          s4o.print("__nb_steps; i++) {\n");
          s4o.indent_right();
          s4o.print(s4o.indent_spaces);
          print_variable_prefix();
          s4o.print("__step_list[i] = temp_step;\n");
          s4o.indent_left();
          s4o.print(s4o.indent_spaces + "}\n");
          for(int i = 0; i < symbol->n; i++)
            symbol->elements[i]->accept(*this);
          
          /* actions table count */
          wanted_sfcdeclaration = actioncount_sd;
          for(int i = 0; i < symbol->n; i++)
            symbol->elements[i]->accept(*this);
          s4o.print(s4o.indent_spaces);
          print_variable_prefix();
          s4o.print("__nb_actions = ");
          s4o.print(action_number);
          s4o.print(";\n");
          action_number = 0;
          wanted_sfcdeclaration = sfcinit_sd;
          
          /* actions table initialisation */
          s4o.print(s4o.indent_spaces + "static const ACTION temp_action = {0, {0, 0}, 0, 0, {0, 0}, {0, 0}};\n");
          s4o.print(s4o.indent_spaces + "for(i = 0; i < ");
          print_variable_prefix();
          s4o.print("__nb_actions; i++) {\n");
          s4o.indent_right();
          s4o.print(s4o.indent_spaces);
          print_variable_prefix();
          s4o.print("__action_list[i] = temp_action;\n");
          s4o.indent_left();
          s4o.print(s4o.indent_spaces + "}\n");
          
          /* transitions table count */
          wanted_sfcdeclaration = transitioncount_sd;
          for(int i = 0; i < symbol->n; i++)
            symbol->elements[i]->accept(*this);
          s4o.print(s4o.indent_spaces);
          print_variable_prefix();
          s4o.print("__nb_transitions = ");
          s4o.print(transition_number);
          s4o.print(";\n");
          transition_number = 0;
          wanted_sfcdeclaration = sfcinit_sd;

          /* last_ticktime initialisation */
          s4o.print(s4o.indent_spaces);
          print_variable_prefix();
          s4o.print("__lasttick_time = __CURRENT_TIME;\n");
          break;
        case stepdef_sd:
          s4o.print("// Steps definitions\n");
          for(int i = 0; i < symbol->n; i++)
            symbol->elements[i]->accept(*this);
          s4o.print("\n");
          break;
        case actiondef_sd:
          s4o.print("// Actions definitions\n");
          {
            // first fill up the this->variable_list variable!
            wanted_sfcdeclaration = actioncount_sd;
            for(int i = 0; i < symbol->n; i++)
               symbol->elements[i]->accept(*this);
            action_number = 0; // reset the counter!
            wanted_sfcdeclaration = actiondef_sd;
            // Now do the defines for actions!
            for(int i = 0; i < symbol->n; i++)
              symbol->elements[i]->accept(*this);
            // Now do the defines for actions that reference a variable instead of an action block!
            std::list<VARIABLE>::iterator pt;
            for(pt = variable_list.begin(); pt != variable_list.end(); pt++) {
              s4o.print("#define ");
              s4o.print(SFC_STEP_ACTION_PREFIX);
              pt->symbol->accept(*this);
              s4o.print(" ");
              s4o.print(action_number);
              s4o.print("\n");
              action_number++;
            }
          }
          s4o.print("\n");
          break;
        case stepundef_sd:
          s4o.print("// Steps undefinitions\n");
          for(int i = 0; i < symbol->n; i++)
            symbol->elements[i]->accept(*this);
          s4o.print("\n");
          break;
        case actionundef_sd:
          s4o.print("// Actions undefinitions\n");
          for(int i = 0; i < symbol->n; i++)
            symbol->elements[i]->accept(*this);
          {
            // first fill up the this->variable_list variable!
            wanted_sfcdeclaration = actioncount_sd;
            for(int i = 0; i < symbol->n; i++)
               symbol->elements[i]->accept(*this);
            wanted_sfcdeclaration = actionundef_sd;
            std::list<VARIABLE>::iterator pt;
            for(pt = variable_list.begin(); pt != variable_list.end(); pt++) {
              s4o.print("#undef ");
              s4o.print(SFC_STEP_ACTION_PREFIX);
              pt->symbol->accept(*this);
              s4o.print("\n");
            }
          }
          s4o.print("\n");
          break;
        default:
          break;
      }
      return NULL;
    }
    
    void *visit(initial_step_c *symbol) {
      switch (wanted_sfcdeclaration) {
        case actioncount_sd:
          symbol->action_association_list->accept(*this);
          break;
        case sfcdecl_sd:
          symbol->action_association_list->accept(*this);
        case stepcount_sd:
          step_number++;
          break;
        case sfcinit_sd:
          s4o.print(s4o.indent_spaces);
          s4o.print(SET_VAR);
          s4o.print("(");
          print_variable_prefix();
          s4o.print(",__step_list[");
          s4o.print(step_number);
          s4o.print("].X,,1);\n");
          step_number++;
          break;
        case stepdef_sd:
          s4o.print("#define ");
          symbol->step_name->accept(*this);
          s4o.print(" __step_list[");
          s4o.print(step_number);
          s4o.print("]\n");

          s4o.print("#define ");
          s4o.print(SFC_STEP_ACTION_PREFIX);
          symbol->step_name->accept(*this);
          s4o.print(" ");
          s4o.print(step_number);
          s4o.print("\n");
          step_number++;
          break;
        case stepundef_sd:
          s4o.print("#undef ");
          symbol->step_name->accept(*this);
          s4o.print("\n");

          s4o.print("#undef ");
          s4o.print(SFC_STEP_ACTION_PREFIX);
          symbol->step_name->accept(*this);
          s4o.print("\n");
          break;
        default:
          break;
      }
      return NULL;
    }
    
    void *visit(step_c *symbol) {
      switch (wanted_sfcdeclaration) {
        case actioncount_sd:
          symbol->action_association_list->accept(*this);
          break;
        case sfcdecl_sd:
          symbol->action_association_list->accept(*this);
        case stepcount_sd:
        case sfcinit_sd:
          step_number++;
          break;
        case stepdef_sd:
          s4o.print("#define ");
          symbol->step_name->accept(*this);
          s4o.print(" __step_list[");
          s4o.print(step_number);
          s4o.print("]\n");

          s4o.print("#define ");
          s4o.print(SFC_STEP_ACTION_PREFIX);
          symbol->step_name->accept(*this);
          s4o.print(" ");
          s4o.print(step_number);
          s4o.print("\n");
          step_number++;
          break;
        case stepundef_sd:
          s4o.print("#undef ");
          symbol->step_name->accept(*this);
          s4o.print("\n");

          s4o.print("#undef ");
          s4o.print(SFC_STEP_ACTION_PREFIX);
          symbol->step_name->accept(*this);
          s4o.print("\n");
          break;
        default:
          break;
      }
      return NULL;
    }

    void *visit(action_association_c *symbol) {
      /* we try to find the variable instance declaration, to determine if symbol is variable... */
      symbol_c *var_decl = search_var_instance_decl->get_decl(symbol->action_name);
      if (var_decl != NULL) {
        std::list<VARIABLE>::iterator pt;
        for(pt = variable_list.begin(); pt != variable_list.end(); pt++) {
          if (!compare_identifiers(pt->symbol, symbol->action_name))
            return NULL;
        }
        VARIABLE *variable;
        variable = new VARIABLE;
        variable->symbol = (identifier_c*)(symbol->action_name);
        variable_list.push_back(*variable);
        action_number++;
      }
      return NULL;
    }

    void *visit(transition_c *symbol) {
      switch (wanted_sfcdeclaration) {
        case sfcdecl_sd:
        case transitioncount_sd:
          transition_number++;
          break;
        default:
          break;
      }
      return NULL;
    }

    void *visit(action_c *symbol) {
      switch (wanted_sfcdeclaration) {
        case actiondef_sd:
          s4o.print("#define ");
          s4o.print(SFC_STEP_ACTION_PREFIX);
          symbol->action_name->accept(*this);
          s4o.print(" ");
          s4o.print(action_number);
          s4o.print("\n");
          action_number++;
          break;
        case actionundef_sd:
          s4o.print("#undef ");
          s4o.print(SFC_STEP_ACTION_PREFIX);
          symbol->action_name->accept(*this);
          s4o.print("\n");
          break;
        case actioncount_sd:
        case sfcdecl_sd:
          action_number++;
          break;
        default:
          break;
      }
      return NULL;
    }

    void *visit(instruction_list_c *symbol) {
      return NULL;
    }
    
    void *visit(statement_list_c *symbol) {
      return NULL;
    }

}; /* generate_c_sfcdecl_c */

