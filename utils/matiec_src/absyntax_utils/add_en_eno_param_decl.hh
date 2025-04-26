/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2009-2011  Mario de Sousa (msousa@fe.up.pt)
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
 * Iterate through all declared functions and Function Blocks, 
 * and, for each function/FB, add a declaration of the EN and ENO 
 * parameters, if they have not already been explicitly declared.
 *
 * EN and ENO parameters declared explicitly (by the user in the source code)
 * and implicitly (by the comnpiler, i.e. by this visitor class) may be
 * distinguished later on by the 'method' flag in the en_param_declaration_c
 * and eno_param_declaration_c objects.
 */

#ifndef _ADD_EN_ENO_PARAM_DECL_HH
#define _ADD_EN_ENO_PARAM_DECL_HH

#include "../absyntax/visitor.hh"


class add_en_eno_param_decl_c : public null_visitor_c {
  public:
    static symbol_c *add_to(symbol_c *tree_root);
    ~add_en_eno_param_decl_c(void);

  private:
    /* this class is a singleton. So we need a pointer to the single instance... */
    static add_en_eno_param_decl_c *singleton;

    /* flags to remember whether the EN and/or ENO parameters have already
     * been explicitly declared by the user in the IEC 61131-3 source code we are parsing...
     */
    bool en_declared;
    bool eno_declared;

  private:
    void* iterate_list(list_c *list);
    input_declarations_c  *build_en_param (void);
    output_declarations_c *build_eno_param(void);

  private:
    /***************************/
    /* B 0 - Programming Model */
    /***************************/
    void *visit(library_c *symbol);

    /***********************/
    /* B 1.5.1 - Functions */
    /***********************/
    void *visit(function_declaration_c *symbol);
    /* intermediate helper symbol for function_declaration */
    void *visit(var_declarations_list_c *symbol);

    /******************************************/
    /* B 1.4.3 - Declaration & Initialisation */
    /******************************************/
    void *visit(input_declarations_c *symbol);
    void *visit(input_declaration_list_c *symbol);
    void *visit(en_param_declaration_c *symbol);
    void *visit(eno_param_declaration_c *symbol);
    void *visit(output_declarations_c *symbol);
    void *visit(var_init_decl_list_c *symbol);

    /*****************************/
    /* B 1.5.2 - Function Blocks */
    /*****************************/
    /*  FUNCTION_BLOCK derived_function_block_name io_OR_other_var_declarations function_block_body END_FUNCTION_BLOCK */
    void *visit(function_block_declaration_c *symbol);

}; // function_param_iterator_c

#endif /* _ADD_EN_ENO_PARAM_DECL_HH */







