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
 * This file contains the code that stores the output generated
 * by each specific version of the 4th stage.
 */

#ifndef _STAGE4_HH
#define _STAGE4_HH

#include "../absyntax/absyntax.hh"


void stage4err(const char *stage4_generator_id, symbol_c *symbol1, symbol_c *symbol2, const char *errmsg, ...);


class stage4out_c {
  public:
    std::string indent_level;
    std::string indent_spaces;

  public:
    stage4out_c(std::string indent_level = "  ");
    stage4out_c(const char *dir, const char *radix, const char *extension, std::string indent_level = "  ");
    ~stage4out_c(void);
    
    void flush(void);
    
    void enable_output(void);
    void disable_output(void);

    void indent_right(void);
    void indent_left(void);

    void *print(          std::string  value);
    void *print(           const char *value);
    //void *print(               int64_t value); // not required, since we have long long int, or similar
    //void *print(              uint64_t value); // not required, since we have long long int, or similar
    void *print(              real64_t value);
    void *print(                   int value);
    void *print(              long int value);
    void *print(         long long int value);
    void *print(unsigned           int value);
    void *print(unsigned      long int value);
    void *print(unsigned long long int value);
    
    void *print_long_integer(unsigned long l_integer, bool suffix=true);
    void *print_long_long_integer(unsigned long long ll_integer, bool suffix=true);


    void *printupper(const char *str);
    void *printupper(std::string str);

    void *printlocation(const char *str);
    void *printlocation(std::string str);

    void *printlocation_comasep(const char *str);

  protected:
    std::ostream *out;
    std::fstream *m_file;
    
    /* A flag to tell whether to really print to the file, or to ignore any request to print to the file */
    /* This is used to implement the no_code_generation pragmas, that lets the user tell the compiler
     * when to switch on and off the code generation, without stoping the lexical, syntatical, and
     * semantic analysers from analysing the code.
     */
    bool allow_output;

};



int stage4(symbol_c *tree_root, const char *builddir);

/* Functions to be implemented by each generate_XX version of stage 4 */
int  stage4_parse_options(char *options);
void stage4_print_options(void);

#endif /* _STAGE4_HH */
