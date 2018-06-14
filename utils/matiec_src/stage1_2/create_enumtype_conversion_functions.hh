/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2009-2012  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2012       Manuele Conti  (conti.ma@alice.it)
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
 * create_enumtype_conversion_functions_c generates ST conversion functions source code for
 * enumerate user defined data types.
 *
 */

#ifndef _CREATE_ENUMTYPE_CONVERSION_FUNCTIONS_HH
#define _CREATE_ENUMTYPE_CONVERSION_FUNCTIONS_HH

#include <string>
#include <list>

#include "../absyntax_utils/absyntax_utils.hh"


class create_enumtype_conversion_functions_c: public iterator_visitor_c {
  private:
    static create_enumtype_conversion_functions_c *singleton;
    
  public:
    explicit create_enumtype_conversion_functions_c(symbol_c *ignore);
    virtual ~create_enumtype_conversion_functions_c(void);
    static std::string &get_declaration(symbol_c *symbol);

    void *visit(                 identifier_c *symbol);
    void *visit(         poutype_identifier_c *symbol);
    void *visit(derived_datatype_identifier_c *symbol);
    
    /**********************/
    /* B 1.3 - Data types */
    /**********************/
    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
    void *visit(enumerated_type_declaration_c *symbol);
    void *visit(enumerated_value_list_c *symbol);

  private:
    std::string text;
    std::string currentToken;
    std::list <std::string> currentTokenList;
    std::string getIntegerName(bool isSigned, size_t size);
    void printStringToEnum  (std::string &enumerateName, std::list <std::string> &enumerateValues);
    void printEnumToString  (std::string &enumerateName, std::list <std::string> &enumerateValues);
    void printIntegerToEnum (std::string &enumerateName, std::list <std::string> &enumerateValues, bool isSigned, size_t size);
    void printEnumToInteger (std::string &enumerateName, std::list <std::string> &enumerateValues, bool isSigned, size_t size);
};

#endif /* _CREATE_ENUMTYPE_CONVERSION_FUNCTIONS_HH */
