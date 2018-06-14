/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2003-2012  Mario de Sousa (msousa@fe.up.pt)
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

/* Determine the characteristics of a specific data type
 *  e.g., is it an enumeration, is it an array, is it ANY_INT, etc...
 *
 * The methods of this class may be passed either:
 *  - a data type declaration symbol_c, 
 *   OR
 *  - the name of a data type (identifier_c)
 *    In this case, we shall first serach for the basetype declaration using search_base_type_c, and then 
 *    run the normal process.
 */
#include "absyntax_utils.hh"

#include "../main.hh" // required for ERROR() and ERROR_MSG() macros.






class get_datatype_info_c { 


  private: // this is a purely static class. No need for constructors!
     get_datatype_info_c(void) {};
    ~get_datatype_info_c(void) {};

    // A helper method to get_datatype_info_c::is_type_equal()
    // Assuming the relaxed datatype model, return whether the two array datatypes are equal/equivalent
    static bool is_arraytype_equal_relaxed(symbol_c *first_type, symbol_c *second_type);
  
  public:
    static symbol_c   *get_id    (symbol_c *datatype); /* get the identifier (name) of the datatype); returns NULL if anonymous datatype! Does not work for elementary datatypes!*/
    static const char *get_id_str(symbol_c *datatype); /* get the identifier (name) of the datatype); returns NULL if anonymous datatype! */

    static symbol_c *get_struct_field_type_id      (symbol_c *struct_datatype, symbol_c *struct_fieldname); // returns datatype of a field in a structure
    static symbol_c *get_array_storedtype_id       (symbol_c *type_symbol);    // returns the datatype of the variables stored in the array
    static symbol_c *get_ref_to                    (symbol_c *type_symbol);    // Defined in IEC 61131-3 v3 (returns the type that is being referenced/pointed to)        
    
    /* Returns true if both datatypes are equivalent (not necessarily equal!).
     * Two datatype models are supported: relaxed and strict (chosen from a command line option).
     * WARNING: May return true even though the datatypes are not the same/identical!!!
     *          In both of the models, this may occur when at least one of the datatypes is of a generic
     *          datatype (Generic dataypes: ANY, ANY_INT, ANY_NUM, ...), 
     *          or when two REF_TO datatypes both reference an equivalent datatype.
     *          In only the relaxed datatype, it may also return true if two array datatypes
     *          have the same subrange limits, and contain the same data. 
     *          
     * NOTE: Currently only the ANY generic datatype is implemented!
     * NOTE: Currently stage1_2 only allows the use of the ANY keyword when in conjuntion with
     *       the REF_TO keyword (i.e. REF_TO ANY).
     */
    static bool is_type_equal(symbol_c *first_type, symbol_c *second_type);
    static bool is_type_valid(symbol_c *type);

    static bool is_ref_to                          (symbol_c *type_symbol);    // Defined in IEC 61131-3 v3
    static bool is_sfc_initstep                    (symbol_c *type_symbol);
    static bool is_sfc_step                        (symbol_c *type_symbol);
    static bool is_function_block                  (symbol_c *type_symbol);
    static bool is_subrange                        (symbol_c *type_symbol);
    static bool is_enumerated                      (symbol_c *type_symbol);
    static bool is_array                           (symbol_c *type_symbol);
    static bool is_structure                       (symbol_c *type_symbol);
  
    static bool is_ANY_REAL_literal(symbol_c *type_symbol); /* Can't we do away with this?? */
    static bool is_ANY_INT_literal (symbol_c *type_symbol); /* Can't we do away with this?? */

    static bool is_ANY_generic_type                (symbol_c *type_symbol);

    //static bool is_ANY_ELEMENTARY_generic_type   (symbol_c *type_symbol);
    static bool is_ANY_ELEMENTARY                  (symbol_c *type_symbol);
    static bool is_ANY_SAFEELEMENTARY              (symbol_c *type_symbol);
    static bool is_ANY_ELEMENTARY_compatible       (symbol_c *type_symbol);

    //static bool is_ANY_MAGNITUDE_generic_type    (symbol_c *type_symbol);
    static bool is_ANY_MAGNITUDE                   (symbol_c *type_symbol);
    static bool is_ANY_SAFEMAGNITUDE               (symbol_c *type_symbol);
    static bool is_ANY_MAGNITUDE_compatible        (symbol_c *type_symbol);

    static bool is_ANY_signed_MAGNITUDE            (symbol_c *type_symbol);
    static bool is_ANY_signed_SAFEMAGNITUDE        (symbol_c *type_symbol);
    static bool is_ANY_signed_MAGNITUDE_compatible (symbol_c *type_symbol);

    //static bool is_ANY_NUM_generic_type          (symbol_c *type_symbol);
    static bool is_ANY_NUM                         (symbol_c *type_symbol);
    static bool is_ANY_SAFENUM                     (symbol_c *type_symbol);
    static bool is_ANY_NUM_compatible              (symbol_c *type_symbol);

    static bool is_ANY_signed_NUM                  (symbol_c *type_symbol);
    static bool is_ANY_signed_SAFENUM              (symbol_c *type_symbol);
    static bool is_ANY_signed_NUM_compatible       (symbol_c *type_symbol);

    //static bool is_ANY_INT_generic_type          (symbol_c *type_symbol);
    static bool is_ANY_INT                         (symbol_c *type_symbol);
    static bool is_ANY_SAFEINT                     (symbol_c *type_symbol);
    static bool is_ANY_INT_compatible              (symbol_c *type_symbol);

    static bool is_ANY_signed_INT                  (symbol_c *type_symbol);
    static bool is_ANY_signed_SAFEINT              (symbol_c *type_symbol);
    static bool is_ANY_signed_INT_compatible       (symbol_c *type_symbol);

    static bool is_ANY_unsigned_INT                (symbol_c *type_symbol);
    static bool is_ANY_unsigned_SAFEINT            (symbol_c *type_symbol);
    static bool is_ANY_unsigned_INT_compatible     (symbol_c *type_symbol);

    //static bool is_ANY_REAL_generic_type         (symbol_c *type_symbol);
    static bool is_ANY_REAL                        (symbol_c *type_symbol);
    static bool is_ANY_SAFEREAL                    (symbol_c *type_symbol);
    static bool is_ANY_REAL_compatible             (symbol_c *type_symbol);

    static bool is_ANY_nBIT                        (symbol_c *type_symbol);
    static bool is_ANY_SAFEnBIT                    (symbol_c *type_symbol);
    static bool is_ANY_nBIT_compatible             (symbol_c *type_symbol);

    static bool is_BOOL                            (symbol_c *type_symbol);
    static bool is_SAFEBOOL                        (symbol_c *type_symbol);
    static bool is_BOOL_compatible                 (symbol_c *type_symbol);

    //static bool is_ANY_BIT_generic_type          (symbol_c *type_symbol);
    static bool is_ANY_BIT                         (symbol_c *type_symbol);
    static bool is_ANY_SAFEBIT                     (symbol_c *type_symbol);
    static bool is_ANY_BIT_compatible              (symbol_c *type_symbol);

    //static bool is_ANY_DATE_generic_type         (symbol_c *type_symbol);
    static bool is_ANY_DATE                        (symbol_c *type_symbol);
    static bool is_ANY_SAFEDATE                    (symbol_c *type_symbol);
    static bool is_ANY_DATE_compatible             (symbol_c *type_symbol);

    static bool is_TIME                            (symbol_c *type_symbol);
    static bool is_SAFETIME                        (symbol_c *type_symbol);
    static bool is_TIME_compatible                 (symbol_c *type_symbol);

    //static bool is_ANY_STRING_generic_type       (symbol_c *type_symbol);
    static bool is_ANY_STRING                      (symbol_c *type_symbol);
    static bool is_ANY_SAFESTRING                  (symbol_c *type_symbol);
    static bool is_ANY_STRING_compatible           (symbol_c *type_symbol);

    
    
    
  public:
    /* object used to identify an entry in the abstract syntax tree with an invalid data type */
    /* This is only used from stage3 onwards. Stages 1 and 2 will never create any instances of invalid_type_name_c */
    static invalid_type_name_c     invalid_type_name;

    /**********************/
    /* B.1.3 - Data types */
    /**********************/
    /***********************************/
    /* B 1.3.1 - Elementary Data Types */
    /***********************************/
    static lreal_type_name_c        lreal_type_name;
    static real_type_name_c         real_type_name;
    
    static lint_type_name_c         lint_type_name;
    static dint_type_name_c         dint_type_name;
    static int_type_name_c          int_type_name;
    static sint_type_name_c         sint_type_name;
    
    static ulint_type_name_c        ulint_type_name;
    static udint_type_name_c        udint_type_name;
    static uint_type_name_c         uint_type_name;
    static usint_type_name_c        usint_type_name;

    static lword_type_name_c        lword_type_name;
    static dword_type_name_c        dword_type_name;
    static word_type_name_c         word_type_name;
    static byte_type_name_c         byte_type_name;
    static bool_type_name_c         bool_type_name;
    
    static wstring_type_name_c      wstring_type_name;
    static string_type_name_c       string_type_name;
    
    static dt_type_name_c           dt_type_name;
    static date_type_name_c         date_type_name;
    static tod_type_name_c          tod_type_name;
    static time_type_name_c         time_type_name;


    /******************************************************/
    /* Extensions to the base standard as defined in      */
    /* "Safety Software Technical Specification,          */
    /*  Part 1: Concepts and Function Blocks,             */
    /*  Version 1.0 – Official Release"                   */
    /* by PLCopen - Technical Committee 5 - 2006-01-31    */
    /******************************************************/  
    static safelreal_type_name_c    safelreal_type_name;
    static safereal_type_name_c     safereal_type_name;
    
    static safelint_type_name_c     safelint_type_name;
    static safedint_type_name_c     safedint_type_name;
    static safeint_type_name_c      safeint_type_name;
    static safesint_type_name_c     safesint_type_name;
           
    static safeulint_type_name_c    safeulint_type_name;
    static safeudint_type_name_c    safeudint_type_name;
    static safeuint_type_name_c     safeuint_type_name;
    static safeusint_type_name_c    safeusint_type_name;
           
    static safelword_type_name_c    safelword_type_name;
    static safedword_type_name_c    safedword_type_name;
    static safeword_type_name_c     safeword_type_name;
    static safebyte_type_name_c     safebyte_type_name;
    static safebool_type_name_c     safebool_type_name;
    
    static safewstring_type_name_c  safewstring_type_name;
    static safestring_type_name_c   safestring_type_name;
    
    static safedt_type_name_c       safedt_type_name;
    static safedate_type_name_c     safedate_type_name;
    static safetod_type_name_c      safetod_type_name;
    static safetime_type_name_c     safetime_type_name;               
};

