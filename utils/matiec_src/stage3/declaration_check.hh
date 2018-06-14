/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2003-2012  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2012       Manuele Conti (conti.ma@alice.it)
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

#include <vector>

#include "../absyntax_utils/absyntax_utils.hh"


class declaration_check_c : public iterator_visitor_c {
    int error_count;
    int current_display_error_level;
    symbol_c *current_pou_decl;
    symbol_c *current_resource_decl;

public:
    declaration_check_c(symbol_c *ignore);
    virtual ~declaration_check_c(void);
    int get_error_count();

    /*****************************/
    /* B 1.5.2 - Function Blocks */
    /*****************************/
    void *visit(function_block_declaration_c *symbol);

    /******************************************/
    /* B 1.5.3 - Declaration & Initialisation */
    /******************************************/
    void *visit(program_declaration_c *symbol);

    /********************************/
    /* B 1.7 Configuration elements */
    /********************************/
    void *visit(configuration_declaration_c *symbol);
    void *visit(resource_declaration_c      *symbol);
    void *visit(program_configuration_c     *symbol);
};
