/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2015  Mario de Sousa (msousa@fe.up.pt)
 *
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
 * Case Options Checking:
 *   - Check whether the options in a case statement are repeated, either directly, or in a range.
 *       For example:
 *         case var of
 *           1: ...   <- OK
 *           2: ...   <- OK
 *           1: ...   <- OK (not an error), but produce a warning!
 *           0..8: ...<- OK (not an error), but produce a warning!
 */

#include "../absyntax_utils/absyntax_utils.hh"



class case_elements_check_c: public iterator_visitor_c {

  private:
    bool warning_found;
    int error_count;
    int current_display_error_level;

    std::vector<symbol_c *> case_elements_list;
    void check_subr_subr(symbol_c *s1, symbol_c *s2);
    void check_subr_symb(symbol_c *s1, symbol_c *s2);
    void check_symb_symb(symbol_c *s1, symbol_c *s2);
  

  public:
    case_elements_check_c(symbol_c *ignore);
    virtual ~case_elements_check_c(void);
    int get_error_count();

    /***************************************/
    /* B.3 - Language ST (Structured Text) */
    /***************************************/
    /********************/
    /* B 3.2 Statements */
    /********************/
    /********************************/
    /* B 3.2.3 Selection Statements */
    /********************************/
    void *visit(case_statement_c *symbol);
    void *visit(case_list_c      *symbol);
}; /* case_elements_check_c */







