/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
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
 * Some classes to help with debuging.
 *
 * These classes will print out the current state of a symbol or a portion of the Abstract Syntax Tree.
 */






#include "../absyntax/absyntax.hh"


class debug_c { 
  public:
    static void print(const_value_c  cvalue);
    static void print(symbol_c      *symbol);
    static void print(const char    *str);

    /* print the AST from this point downwards */
    static void print_ast(symbol_c *root_symbol);
};


















