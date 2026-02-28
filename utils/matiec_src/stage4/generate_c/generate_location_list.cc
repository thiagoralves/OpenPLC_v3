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

class generate_location_list_c: public iterator_visitor_c {

  protected:
    stage4out_c &s4o;
    generate_c_vardecl_c generate_c_vardecl;

  public:
    generate_location_list_c(stage4out_c *s4o_ptr):
        s4o(*s4o_ptr), 
        generate_c_vardecl(s4o_ptr,
                           generate_c_vardecl_c::location_list_vf,
                           generate_c_vardecl_c::global_vt) {
    }

    ~generate_location_list_c(void) {
    }


/********************************************/
/* B.1.4.3   Declaration and initilization  */
/********************************************/

/*| VAR_GLOBAL [CONSTANT|RETAIN] [PUBLISH|SUBSCRIBE 'group_name'] global_var_decl_list END_VAR */
// SYM_REF3(global_var_declarations_c, option, global_var_decl_list, iceoryx_option)
    void *visit(global_var_declarations_c *symbol) {
        /* DEL: set IceOryx2 context on generate_c_vardecl before iterating children */
        if (symbol->iceoryx_option != NULL) {
            publish_option_c   *pub = dynamic_cast<publish_option_c *>(symbol->iceoryx_option);
            subscribe_option_c *sub = dynamic_cast<subscribe_option_c *>(symbol->iceoryx_option);
            if (pub != NULL) {
                token_c *tok = dynamic_cast<token_c *>(pub->group_name);
                generate_c_vardecl.set_iceoryx_context("PUBLISH", tok ? tok->value : NULL);
            } else if (sub != NULL) {
                token_c *tok = dynamic_cast<token_c *>(sub->group_name);
                generate_c_vardecl.set_iceoryx_context("SUBSCRIBE", tok ? tok->value : NULL);
            }
        } else {
            generate_c_vardecl.set_iceoryx_context(NULL, NULL);
        }
        /* iterate into children (global_var_decl_list) using default iterator */
        symbol->global_var_decl_list->accept(*this);
        /* reset context */
        generate_c_vardecl.set_iceoryx_context(NULL, NULL);
        return NULL;
    }

/*  [variable_name] location ':' located_var_spec_init */
/* variable_name -> may be NULL ! */
//SYM_REF4(located_var_decl_c, variable_name, location, located_var_spec_init, unused)
    void *visit(located_var_decl_c *symbol) {

        generate_c_vardecl.print(symbol);

        return NULL;
    }

/*| global_var_spec ':' [located_var_spec_init|function_block_type_name] */
/* type_specification ->may be NULL ! */
//SYM_REF2(global_var_decl_c, global_var_spec, type_specification)
    void *visit(global_var_decl_c *symbol) {

        generate_c_vardecl.print(symbol);

        return NULL;
    }

}; /* generate_location_list_c */
