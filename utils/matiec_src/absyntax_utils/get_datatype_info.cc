/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2003-2012  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2007-2011  Laurent Bessard and Edouard Tisserant
 *  Copyright (C) 2012       Manuele Conti  (conti.ma@alice.it)
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

#include "../main.hh" // required for ERROR() and ERROR_MSG() macros, as well as the runtime_options global variable



#include <typeinfo>  // required for typeid


/**********************************************************/
/**********************************************************/
/**********************************************************/
/*****                                                *****/
/*****                                                *****/
/*****           Some helper classes                  *****/
/*****                                                *****/
/*****                                                *****/
/**********************************************************/
/**********************************************************/
/**********************************************************/


/****************************************************************************************************/
/****************************************************************************************************/
/* Return the identifier (name) of a datatype, typically declared in a TYPE .. END_TYPE declaration */
/****************************************************************************************************/
/****************************************************************************************************/
class get_datatype_id_c: null_visitor_c {
  private:
    static get_datatype_id_c *singleton;
    
  public:
    static symbol_c *get_id(symbol_c *symbol) {
      if (NULL == singleton) singleton = new  get_datatype_id_c();
      if (NULL == singleton) ERROR;
      return (symbol_c *)symbol->accept(*singleton);
    }
    
  protected:
    /***********************************/
    /* B 1.3.1 - Elementary Data Types */
    /***********************************/
    void *visit(time_type_name_c        *symbol) {return (void *)symbol;};
    void *visit(bool_type_name_c        *symbol) {return (void *)symbol;};
    void *visit(sint_type_name_c        *symbol) {return (void *)symbol;};
    void *visit(int_type_name_c         *symbol) {return (void *)symbol;};
    void *visit(dint_type_name_c        *symbol) {return (void *)symbol;};
    void *visit(lint_type_name_c        *symbol) {return (void *)symbol;};
    void *visit(usint_type_name_c       *symbol) {return (void *)symbol;};
    void *visit(uint_type_name_c        *symbol) {return (void *)symbol;};
    void *visit(udint_type_name_c       *symbol) {return (void *)symbol;};
    void *visit(ulint_type_name_c       *symbol) {return (void *)symbol;};
    void *visit(real_type_name_c        *symbol) {return (void *)symbol;};
    void *visit(lreal_type_name_c       *symbol) {return (void *)symbol;};
    void *visit(date_type_name_c        *symbol) {return (void *)symbol;};
    void *visit(tod_type_name_c         *symbol) {return (void *)symbol;};
    void *visit(dt_type_name_c          *symbol) {return (void *)symbol;};
    void *visit(byte_type_name_c        *symbol) {return (void *)symbol;};
    void *visit(word_type_name_c        *symbol) {return (void *)symbol;};
    void *visit(lword_type_name_c       *symbol) {return (void *)symbol;};
    void *visit(dword_type_name_c       *symbol) {return (void *)symbol;};
    void *visit(string_type_name_c      *symbol) {return (void *)symbol;};
    void *visit(wstring_type_name_c     *symbol) {return (void *)symbol;};

    void *visit(safetime_type_name_c    *symbol) {return (void *)symbol;};
    void *visit(safebool_type_name_c    *symbol) {return (void *)symbol;};
    void *visit(safesint_type_name_c    *symbol) {return (void *)symbol;};
    void *visit(safeint_type_name_c     *symbol) {return (void *)symbol;};
    void *visit(safedint_type_name_c    *symbol) {return (void *)symbol;};
    void *visit(safelint_type_name_c    *symbol) {return (void *)symbol;};
    void *visit(safeusint_type_name_c   *symbol) {return (void *)symbol;};
    void *visit(safeuint_type_name_c    *symbol) {return (void *)symbol;};
    void *visit(safeudint_type_name_c   *symbol) {return (void *)symbol;};
    void *visit(safeulint_type_name_c   *symbol) {return (void *)symbol;};
    void *visit(safereal_type_name_c    *symbol) {return (void *)symbol;};
    void *visit(safelreal_type_name_c   *symbol) {return (void *)symbol;};
    void *visit(safedate_type_name_c    *symbol) {return (void *)symbol;};
    void *visit(safetod_type_name_c     *symbol) {return (void *)symbol;};
    void *visit(safedt_type_name_c      *symbol) {return (void *)symbol;};
    void *visit(safebyte_type_name_c    *symbol) {return (void *)symbol;};
    void *visit(safeword_type_name_c    *symbol) {return (void *)symbol;};
    void *visit(safelword_type_name_c   *symbol) {return (void *)symbol;};
    void *visit(safedword_type_name_c   *symbol) {return (void *)symbol;};
    void *visit(safestring_type_name_c  *symbol) {return (void *)symbol;};
    void *visit(safewstring_type_name_c *symbol) {return (void *)symbol;};

    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
    /*  simple_type_name ':' simple_spec_init */
    void *visit(simple_type_declaration_c     *symbol)  {return symbol->simple_type_name;}
    /*  subrange_type_name ':' subrange_spec_init */
    void *visit(subrange_type_declaration_c   *symbol)  {return symbol->subrange_type_name;}
    /*  enumerated_type_name ':' enumerated_spec_init */
    void *visit(enumerated_type_declaration_c *symbol)  {return symbol->enumerated_type_name;}
    /*  identifier ':' array_spec_init */
    void *visit(array_type_declaration_c      *symbol)  {return symbol->identifier;}
    /*  structure_type_name ':' structure_specification */
    void *visit(structure_type_declaration_c  *symbol)  {return symbol->structure_type_name;}
    /*  string_type_name ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init */
    void *visit(string_type_declaration_c     *symbol)  {return symbol->string_type_name;}
    /* ref_type_decl: identifier ':' ref_spec_init */
    void *visit(ref_type_decl_c               *symbol)  {return symbol->ref_type_name;}
    /* NOTE: DO NOT place any code here that references symbol->anotations_map["generate_c_annotaton__implicit_type_id"] !!
     *       All anotations in the symbol->anotations_map[] are considered a stage4 construct. In the above example,
     *       That anotation is specific to the generate_c stage4 code, and must therefore NOT be referenced
     *       in the absyntax_utils code, as this last code should be independent of the stage4 version!
     */ 

    /*****************************/
    /* B 1.5.2 - Function Blocks */
    /*****************************/
    /*  FUNCTION_BLOCK derived_function_block_name io_OR_other_var_declarations function_block_body END_FUNCTION_BLOCK */
    void *visit(function_block_declaration_c  *symbol)  {return symbol->fblock_name;}
    /**********************/
    /* B 1.5.3 - Programs */
    /**********************/
    /*  PROGRAM program_type_name program_var_declarations_list function_block_body END_PROGRAM */
    void *visit(program_declaration_c  *symbol)  {return symbol->program_type_name;}
    
}; // get_datatype_id_c 

get_datatype_id_c *get_datatype_id_c::singleton = NULL;






/**************************************************/
/**************************************************/
/* transform elementary data type class to string */
/**************************************************/
/**************************************************/

/* A small helper class, to transform elementary data type to string.
 * this allows us to generate more relevant error messages...
 */

class get_datatype_id_str_c: public null_visitor_c {
  protected:
     get_datatype_id_str_c(void)  {};
    ~get_datatype_id_str_c(void) {};

  private:
    /* singleton class! */
    static get_datatype_id_str_c *singleton;

  public:
    static const char *get_id_str(symbol_c *symbol) {
      if (NULL == singleton)    singleton = new get_datatype_id_str_c;
      if (NULL == singleton)    ERROR;
      const char *res           = (const char *)symbol->accept(*singleton);
      if (NULL == res)          ERROR;
      return res;
    }


    /*************************/
    /* B.1 - Common elements */
    /*************************/
    /*******************************************/
    /* B 1.1 - Letters, digits and identifiers */
    /*******************************************/
    void *visit(                 identifier_c *symbol) {return (void *)symbol->value;};
    // Should not be necessary, as datatype declarations currently use an identifier_c for their name! 
    // Only references to the datatype (when declaring variable, for ex., will use poutype_identifier_c
    void *visit(derived_datatype_identifier_c *symbol) {return (void *)symbol->value;};   
    // Should not be necessary, as FB declarations currently use an identifier_c for their name! 
    // Only references to the FB (when declaring variable, for ex., will use poutype_identifier_c
    void *visit(         poutype_identifier_c *symbol) {return (void *)symbol->value;};  

    /***********************************/
    /* B 1.3.1 - Elementary Data Types */
    /***********************************/
    void *visit(time_type_name_c        *symbol) {return (void *)"TIME";        };
    void *visit(bool_type_name_c        *symbol) {return (void *)"BOOL";        };
    void *visit(sint_type_name_c        *symbol) {return (void *)"SINT";        };
    void *visit(int_type_name_c         *symbol) {return (void *)"INT";         };
    void *visit(dint_type_name_c        *symbol) {return (void *)"DINT";        };
    void *visit(lint_type_name_c        *symbol) {return (void *)"LINT";        };
    void *visit(usint_type_name_c       *symbol) {return (void *)"USINT";       };
    void *visit(uint_type_name_c        *symbol) {return (void *)"UINT";        };
    void *visit(udint_type_name_c       *symbol) {return (void *)"UDINT";       };
    void *visit(ulint_type_name_c       *symbol) {return (void *)"ULINT";       };
    void *visit(real_type_name_c        *symbol) {return (void *)"REAL";        };
    void *visit(lreal_type_name_c       *symbol) {return (void *)"LREAL";       };
    void *visit(date_type_name_c        *symbol) {return (void *)"DATE";        };
    void *visit(tod_type_name_c         *symbol) {return (void *)"TOD";         };
    void *visit(dt_type_name_c          *symbol) {return (void *)"DT";          };
    void *visit(byte_type_name_c        *symbol) {return (void *)"BYTE";        };
    void *visit(word_type_name_c        *symbol) {return (void *)"WORD";        };
    void *visit(lword_type_name_c       *symbol) {return (void *)"LWORD";       };
    void *visit(dword_type_name_c       *symbol) {return (void *)"DWORD";       };
    void *visit(string_type_name_c      *symbol) {return (void *)"STRING";      };
    void *visit(wstring_type_name_c     *symbol) {return (void *)"WSTRING";     };

    void *visit(safetime_type_name_c    *symbol) {return (void *)"SAFETIME";    };
    void *visit(safebool_type_name_c    *symbol) {return (void *)"SAFEBOOL";    };
    void *visit(safesint_type_name_c    *symbol) {return (void *)"SAFESINT";    };
    void *visit(safeint_type_name_c     *symbol) {return (void *)"SAFEINT";     };
    void *visit(safedint_type_name_c    *symbol) {return (void *)"SAFEDINT";    };
    void *visit(safelint_type_name_c    *symbol) {return (void *)"SAFELINT";    };
    void *visit(safeusint_type_name_c   *symbol) {return (void *)"SAFEUSINT";   };
    void *visit(safeuint_type_name_c    *symbol) {return (void *)"SAFEUINT";    };
    void *visit(safeudint_type_name_c   *symbol) {return (void *)"SAFEUDINT";   };
    void *visit(safeulint_type_name_c   *symbol) {return (void *)"SAFEULINT";   };
    void *visit(safereal_type_name_c    *symbol) {return (void *)"SAFEREAL";    };
    void *visit(safelreal_type_name_c   *symbol) {return (void *)"SAFELREAL";   };
    void *visit(safedate_type_name_c    *symbol) {return (void *)"SAFEDATE";    };
    void *visit(safetod_type_name_c     *symbol) {return (void *)"SAFETOD";     };
    void *visit(safedt_type_name_c      *symbol) {return (void *)"SAFEDT";      };
    void *visit(safebyte_type_name_c    *symbol) {return (void *)"SAFEBYTE";    };
    void *visit(safeword_type_name_c    *symbol) {return (void *)"SAFEWORD";    };
    void *visit(safelword_type_name_c   *symbol) {return (void *)"SAFELWORD";   };
    void *visit(safedword_type_name_c   *symbol) {return (void *)"SAFEDWORD";   };
    void *visit(safestring_type_name_c  *symbol) {return (void *)"SAFESTRING";  };
    void *visit(safewstring_type_name_c *symbol) {return (void *)"SAFEWSTRING"; };

    /********************************/
    /* B.1.3.2 - Generic data types */
    /********************************/
    void *visit(generic_type_any_c *symbol) {return (void *)"ANY"; };

    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
    /*  simple_type_name ':' simple_spec_init */
    void *visit(simple_type_declaration_c     *symbol)  {return symbol->simple_type_name->accept(*this);}
    /*  subrange_type_name ':' subrange_spec_init */
    void *visit(subrange_type_declaration_c   *symbol)  {return symbol->subrange_type_name->accept(*this);}
    /*  enumerated_type_name ':' enumerated_spec_init */
    void *visit(enumerated_type_declaration_c *symbol)  {return symbol->enumerated_type_name->accept(*this);}
    /*  identifier ':' array_spec_init */
    void *visit(array_type_declaration_c      *symbol)  {return symbol->identifier->accept(*this);}
    /*  structure_type_name ':' structure_specification */
    void *visit(structure_type_declaration_c  *symbol)  {return symbol->structure_type_name->accept(*this);}
    /*  string_type_name ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init */
    void *visit(string_type_declaration_c     *symbol)  {return symbol->string_type_name->accept(*this);}
    /* ref_type_decl: identifier ':' ref_spec_init */
    void *visit(ref_type_decl_c               *symbol)  {return symbol->ref_type_name->accept(*this);}
    /* NOTE: DO NOT place any code here that references symbol->anotations_map["generate_c_annotaton__implicit_type_id"] !!
     *       All anotations in the symbol->anotations_map[] are considered a stage4 construct. In the above example,
     *       That anotation is specific to the generate_c stage4 code, and must therefore NOT be referenced
     *       in the absyntax_utils code, as this last code should be independent of the stage4 version!
     */ 
    
    /***********************/
    /* B 1.5.1 - Functions */
    /***********************/
    /* Functions are not really datatypes, but we include it here as it helps in printing out error messages!   */
    /* Currently this is needed only by remove_forward_depencies_c::print_circ_error()                          */
    /*  FUNCTION derived_function_name ':' elementary_type_name io_OR_function_var_declarations_list function_body END_FUNCTION */
    void *visit(      function_declaration_c  *symbol)  {return symbol->derived_function_name->accept(*this);}
    /*****************************/
    /* B 1.5.2 - Function Blocks */
    /*****************************/
    /*  FUNCTION_BLOCK derived_function_block_name io_OR_other_var_declarations function_block_body END_FUNCTION_BLOCK */
    void *visit(function_block_declaration_c  *symbol)  {return symbol->fblock_name->accept(*this);}    
    /**********************/
    /* B 1.5.3 - Programs */
    /**********************/
    /*  PROGRAM program_type_name program_var_declarations_list function_block_body END_PROGRAM */
    void *visit(       program_declaration_c  *symbol)  {return symbol->program_type_name->accept(*this);} 
};

get_datatype_id_str_c *get_datatype_id_str_c::singleton = NULL;



/*********************************************************/
/*********************************************************/
/* get the datatype of a field inside a struct data type */
/*********************************************************/
/*********************************************************/

class get_struct_info_c : null_visitor_c {
  private:
    symbol_c *current_field;
    /* singleton class! */
    static get_struct_info_c *singleton;

  public:
    get_struct_info_c(void) {current_field = NULL;}

    static symbol_c *get_field_type_id(symbol_c *struct_type, symbol_c *field_name) {
      if (NULL == singleton)    singleton = new get_struct_info_c;
      if (NULL == singleton)    ERROR;
      singleton->current_field = field_name;
      return (symbol_c *)struct_type->accept(*singleton);
    }


  private:
    /*************************/
    /* B.1 - Common elements */
    /*************************/
    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
    /*  structure_type_name ':' structure_specification */
    /* NOTE: this is only used inside a TYPE ... END_TYPE declaration.  It is never used directly when declaring a new variable! */
    /* NOTE: structure_specification will point to either initialized_structure_c  OR  structure_element_declaration_list_c */
    void *visit(structure_type_declaration_c *symbol) {return symbol->structure_specification->accept(*this);}

    /* structure_type_name ASSIGN structure_initialization */
    /* structure_initialization may be NULL ! */
    // SYM_REF2(initialized_structure_c, structure_type_name, structure_initialization)
    /* NOTE: only the initialized structure is never used when declaring a new variable instance */
    void *visit(initialized_structure_c *symbol) {return symbol->structure_type_name->accept(*this);}

    /* helper symbol for structure_declaration */
    /* structure_declaration:  STRUCT structure_element_declaration_list END_STRUCT */
    /* structure_element_declaration_list structure_element_declaration ';' */
    void *visit(structure_element_declaration_list_c *symbol) {
      /* now search the structure declaration */
      for(int i = 0; i < symbol->n; i++) {
        void *tmp = symbol->elements[i]->accept(*this);
        if (NULL != tmp) return tmp;
      }
      return NULL; // not found!!
    }  

    /*  structure_element_name ':' spec_init */
    void *visit(structure_element_declaration_c *symbol) {
      if (compare_identifiers(symbol->structure_element_name, current_field) == 0)
        return symbol->spec_init; /* found the type of the element we were looking for! */
      return NULL;   /* not the element we are looking for! */
    }
      

    /* helper symbol for structure_initialization */
    /* structure_initialization: '(' structure_element_initialization_list ')' */
    /* structure_element_initialization_list ',' structure_element_initialization */
    void *visit(structure_element_initialization_list_c *symbol) {ERROR; return NULL;} /* should never get called... */
    /*  structure_element_name ASSIGN value */
    void *visit(structure_element_initialization_c *symbol) {ERROR; return NULL;} /* should never get called... */


    /**************************************/
    /* B.1.5 - Program organization units */
    /**************************************/
    /*****************************/
    /* B 1.5.2 - Function Blocks */
    /*****************************/
    /*  FUNCTION_BLOCK derived_function_block_name io_OR_other_var_declarations function_block_body END_FUNCTION_BLOCK */
    // SYM_REF4(function_block_declaration_c, fblock_name, var_declarations, fblock_body, unused)
    void *visit(function_block_declaration_c *symbol) {
      /* now search the function block declaration for the variable... */
      search_var_instance_decl_c search_decl(symbol);
      return search_decl.get_decl(current_field);
    }
      
    /*********************************************/
    /* B.1.6  Sequential function chart elements */
    /*********************************************/
    /* INITIAL_STEP step_name ':' action_association_list END_STEP */
    // SYM_REF2(initial_step_c, step_name, action_association_list)
    void *visit(initial_step_c *symbol) {
      identifier_c T("T");  identifier_c X("X");
      /* Hard code the datatypes of the implicit variables Stepname.X and Stepname.T */
      if (compare_identifiers(&T, current_field) == 0)  return &get_datatype_info_c::time_type_name;
      if (compare_identifiers(&X, current_field) == 0)  return &get_datatype_info_c::bool_type_name;
      return NULL;
    }
      
    /* STEP step_name ':' action_association_list END_STEP */
    // SYM_REF2(step_c, step_name, action_association_list)
    /* The code here should be identicial to the code in the visit(initial_step_c *) visitor! So we simply call the other visitor! */
    void *visit(step_c *symbol) {initial_step_c initial_step(NULL, NULL); return initial_step.accept(*this);}
      
}; // get_struct_info_c

get_struct_info_c *get_struct_info_c::singleton = NULL;





/**********************************************************/
/**********************************************************/
/**********************************************************/
/*****                                                *****/
/*****                                                *****/
/*****        GET_DATATYPE_INFO_C                     *****/
/*****                                                *****/
/*****                                                *****/
/**********************************************************/
/**********************************************************/
/**********************************************************/

const char *get_datatype_info_c::get_id_str(symbol_c *datatype) {
  return get_datatype_id_str_c::get_id_str(datatype);
}


symbol_c *get_datatype_info_c::get_id(symbol_c *datatype) {
  return get_datatype_id_c::get_id(datatype);
}


symbol_c *get_datatype_info_c::get_struct_field_type_id(symbol_c *struct_datatype, symbol_c *struct_fieldname) {
  return get_struct_info_c::get_field_type_id(struct_datatype, struct_fieldname);
}



symbol_c *get_datatype_info_c::get_array_storedtype_id(symbol_c *type_symbol) {
  // returns the datatype of the variables stored in the array
  array_specification_c *symbol = NULL;
  if (NULL == symbol)  symbol = dynamic_cast<array_specification_c *>(type_symbol);
  if (NULL == symbol)  symbol = dynamic_cast<array_specification_c *>(search_base_type_c::get_basetype_decl(type_symbol));
  if (NULL != symbol)  
    return symbol->non_generic_type_name;
  return NULL; // this is not an array!
}
  
  
  

/* Returns true if both datatypes are equivalent (not necessarily equal!).
 * WARNING: May return true even though the datatypes are not the same/identicial!!!
 *          This occurs when at least one of the datatypes is of a generic
 *          datatype (or a REF_TO a generic datatype). 
 *          (Generic dataypes: ANY, ANY_INT, ANY_NUM, ...)
 * NOTE: Currently only the ANY generic datatype is implemented!
 * NOTE: Currently stage1_2 only allows the use of the ANY keyword when in conjuntion with
 *       the REF_TO keyword (i.e. REF_TO ANY), so when handling non REF_TO datatypes,
 *       this function will currently only return true if the dataypes are identicial.
 */

/* NOTE: matiec supports a strict and a relaxed data type model. Which datatype model to use is chosen
 *       as a command line option.
 * 
 * 
 *       The Strict Datatype Model
 *       =========================
 *       The strict datatype model used by matiec considers any implicitly defined datatype
 *       (e.g. an array datatype defined in the variable declaration itself, instead of inside a TYPE ... END_TYPE
 *       construct) to be different (i.e. not the same datatype, and therefore not compatible) to any other
 *       datatype, including with datatypes declared identically to the implicit datatype.
 *       e.g.
 *         TYPE my_array_t: ARRAY [1..3] OF INT; END_TYPE;
 *         FUNCTION_BLOCK FOO
 *          VAR my_array:  ARRAY [1..3] OF INT; END_VAR
 *         ...
 *         END_FUNCTION_BLOCK
 * 
 *         In the abive code, my_array is NOT considered to te compatible with my_Array_t !!!
 * 
 *       In essence, the currently supported datatype model considers all datatypes to be different to each other,
 *       even though the stored data is the same (Let us call this rule (0))!
 *       There are 2 exceptions to the above rule:
 *        (1) Datatypes that are directly derived from other datatypes.
 *              (this rule is specified in the standard, so we follow it!)
 *        (2) REF_TO datatypes that reference the same datatype
 *              (I dont think the standard says anything about this!)
 *              (This rule should actually be part of the relaxed datatype model, but for now we
 *               will leave it in the strict datatype model) 
 *
 *         TYPE 
 *          my_array_1_t: ARRAY [1..3] OF INT; 
 *          my_array_2_t: ARRAY [1..3] OF INT; 
 *          my_array_3_t: my_array_1_t; 
 *          A_ref_t: REF_TO my_array_1_t;
 *          B_ref_t: REF_TO my_array_1_t;
 *          C_ref_t: A_ref_t;
 *         END_TYPE;
 *                 
 *         In the above code, my_array_1_t is a distinct datatype to my_array_2_t
 *           (this is different to C and C++, where they would be considered the same datatype!)
 *           (following rule (0))
 *         In the above code, my_array_3_t is the same datatype as my_array_1_t
 *           (following rule (1))
 *         In the above code, A_ref_t is the same datatype as B_ref_t
 *           (following rule (2))
 *         In the above code, A_ref_t is the same datatype as C_ref_t
 *           (following rule (1))
 *
 *       Note that rule (0) means that a function/FB with a parameter whose datatype is implicitly defined
 *       can never be passed a value!
 *         FUNCTION_BLOCK FOO
 *          VAR_INPUT my_array:  ARRAY [1..3] OF INT; END_VAR
 *         ...
 *         END_FUNCTION_BLOCK
 *
 *       Any call to FB foo can never pass a value to parameter my_array, as its datatype is distinct
 *       to all other datatypes, and therefore passing any other variable to my_array will result in an
 *       'incompatible datatypes' error!
 *       The above seems natural o me (Mario) in a programming language that is very strongly typed.
 * 
 *       However, if we did not have exception (2), the following would also be invalid:
 *         TYPE my_array_t: ARRAY [1..3] OF INT; END_TYPE;
 *         FUNCTION_BLOCK FOO_t
 *          VAR_INPUT my_array: REF_TO my_array_t; END_VAR
 *         ...
 *         END_FUNCTION_BLOCK
 * 
 *         FUNCTION_BLOCK BAR
 *          VAR
 *            my_array: my_array_t; 
 *            foo: FOO_t;
 *          END_VAR
 *          foo(REF(my_array));  <----- invalid, without rule 2!!
 *         ...
 *         END_FUNCTION_BLOCK
 * 
 *       Rule/exception (2) goes against the datatype model used for all other datatypes.
 *       This rule was adopted as without it, the datatype of the value returned by the REF() 
 *       operator would be considered distinct to all other datatypes, and therefore the
 *       REF() operator would be essentially useless.
 * 
 * 
 *       The Relaxed Datatype Model
 *       ==========================
 *       In the relaxed datatype model, the same rules as the strict datatype model are followed, with the
 *       exception of implicitly defined array datatypes, which are now considered equal if they define
 *       identical datatypes.
 *       This means that in the following example
 *         TYPE 
 *          array_t: ARRAY [1..3] OF INT; 
 *         END_TYPE;
 *         VAR
 *          array_var1: array_t; 
 *          array_var2: ARRAY [1..3] OF INT; 
 *          array_var3: ARRAY [1..3] OF INT; 
 *         END_VAR
 * 
 *       all three variables (array_var1, array_var2, and array_var3) are considered as being of the
 *       same datatype.
 *      
 *       Note that the strict datatype model currently actually uses a relaxed datatype model for 
 *       REF_TO datatypes, so in both the relaxed and strict datatype models matiec currently uses a 
 *       relaxed datatype equivalince for REF_TO datatypes.
 */
bool get_datatype_info_c::is_type_equal(symbol_c *first_type, symbol_c *second_type) {
  if (!is_type_valid( first_type))                                   {return false;}
  if (!is_type_valid(second_type))                                   {return false;}

  /* GENERIC DATATYPES */
  /* For the moment, we only support the ANY generic datatype! */
  if ((is_ANY_generic_type( first_type)) ||
      (is_ANY_generic_type(second_type)))                            {return true;}
      
  /* ANY_ELEMENTARY */
  if ((is_ANY_ELEMENTARY_compatible(first_type)) &&
      (typeid(*first_type) == typeid(*second_type)))                 {return true;}
  if (   is_ANY_ELEMENTARY_compatible(first_type) 
      || is_ANY_ELEMENTARY_compatible(second_type))                  {return false;}  
  
  /* ANY_DERIVED  */
  // from now on, we are sure both datatypes are derived...
  if (is_ref_to(first_type) && is_ref_to(second_type)) {
    return is_type_equal(search_base_type_c::get_basetype_decl(get_ref_to(first_type )),
                         search_base_type_c::get_basetype_decl(get_ref_to(second_type)));
  }

    // check for same datatype
  if (first_type == second_type)                                     {return true;}
  
    // remaining type equivalence rules are not applied in the strict datatype model
  if (false == runtime_options.relaxed_datatype_model)               {return false;}
  
    // check for array equivalence usig the relaxed datatype model
  if (is_arraytype_equal_relaxed(first_type, second_type))           {return true;}

  return false;
}



/* A local helper function that transforms strings conatining signed_integers into a normalized
 * form, so they can be compared for equality.
 *   examples:
 *     82  ->  82 
 *     8_2 ->  82 
 *    +82  ->  82
 *    082  ->  82
 *   +082  ->  82
 *    -82  -> -82
 *    -8_2 -> -82
 *   -082  -> -82
 * 
 * NOTE: since matiec supports arrays with a variable size (a non compliant IEC 61131-3 extension)
 *        (e.g.: ARRAY [1..max] of INT, where max must be a constant variable)
 *       the symbol passed to this function may also be a symbolic_variable
 *       (or more correctly, a symbolic_constant_c).
 *       In this case we simply return the string itself.
 */
#include <string.h>  /* required for strlen() */
static std::string normalize_subrange_limit(symbol_c *symbol) {
  // See if it is an integer...  
  integer_c *integer = dynamic_cast<integer_c *>(symbol);
  if (NULL != integer) {
    // handle it as an integer!
    std::string str = "";
    bool leading_zero = true;
    unsigned int offset = 0;

    // handle any possible leading '-' or '+'
    if        (integer->value[0] == '-') {
        //    '-' -> retained
        str += integer->value[0];
        offset++;
    } else if (integer->value[0] == '+')
        //    '+' -> skip, so '+8' and '8' will both result in '8'
        offset++;
      
    for (unsigned int i = offset; i < strlen(integer->value); i++) {
      if (leading_zero && (integer->value[i] != '0'))
        leading_zero = false;
      if (!leading_zero && integer->value[i] != '_')
        str += integer->value[i];
    }
    return str;
  }
  
  // See if it is an sybolic_variable_c or symbolic_constant_c...  
  /* NOTE: Remember that this is only needed if the subrange limit has not yet been
   *       constant_folded --> when the const_value is valid, the normalize_subrange_limit()
   *       never gets called!!
   * 
   *       Situations where it has not been constant folded can occur if:
   *         - the get_datatype_info_c::is_type_equal() is called before the constant folding algorithm does its thing
   *         - the constant folding algorithm is called before get_datatype_info_c::is_type_equal(), but
   *           the symbol does not get constant folded
   *           (e.g: the POU containing a VAR_EXTERN is not instantiated, and that external variable is used to define a 
   *            limit of an array (ARRAY of [1..ext_var] OF INT)
   *        However, currently get_datatype_info_c::is_type_equal() is not called to handle the above case, 
   *        and constant_folding is being called before all algorithms that call get_datatype_info_c::is_type_equal(),
   *        which means that the following code is really not needed. But it is best to have it here just in case...
   */
  token_c             *token    = NULL;
  symbolic_constant_c *symconst = dynamic_cast<symbolic_constant_c *>(symbol);
  symbolic_variable_c *symvar   = dynamic_cast<symbolic_variable_c *>(symbol);
  if (NULL != symconst) token   = dynamic_cast<            token_c *>(symconst->var_name);
  if (NULL != symvar  ) token   = dynamic_cast<            token_c *>(symvar  ->var_name);
  if (NULL != token)
    // handle it as a symbolic_variable/constant_c
    return token->value;    
  
  ERROR;
  return NULL; // humour the compiler...
}


/* A helper method to get_datatype_info_c::is_type_equal()
 *  Assuming the relaxed datatype model, determine whether the two array datatypes are equal/equivalent
 */
bool get_datatype_info_c::is_arraytype_equal_relaxed(symbol_c *first_type, symbol_c *second_type) {
  symbol_c *basetype_1 = search_base_type_c::get_basetype_decl( first_type);
  symbol_c *basetype_2 = search_base_type_c::get_basetype_decl(second_type);
  array_specification_c *array_1 = dynamic_cast<array_specification_c *>(basetype_1);
  array_specification_c *array_2 = dynamic_cast<array_specification_c *>(basetype_2);

  // are they both array datatypes? 
  if ((NULL == array_1) || (NULL == array_2))
    return false;
  
  // number of subranges
  array_subrange_list_c *subrange_list_1 = dynamic_cast<array_subrange_list_c *>(array_1->array_subrange_list);
  array_subrange_list_c *subrange_list_2 = dynamic_cast<array_subrange_list_c *>(array_2->array_subrange_list);
  if ((NULL == subrange_list_1) || (NULL == subrange_list_2)) ERROR;
  if (subrange_list_1->n != subrange_list_2->n)
    return false;
  
  // comparison of each subrange start and end elements
  for (int i = 0; i < subrange_list_1->n; i++) {
    subrange_c *subrange_1 = dynamic_cast<subrange_c *>(subrange_list_1->elements[i]);
    subrange_c *subrange_2 = dynamic_cast<subrange_c *>(subrange_list_2->elements[i]);
    if ((NULL == subrange_1) || (NULL == subrange_2)) ERROR;
    
    /* check whether the subranges have the same values, using the result of the constant folding agorithm.
     * This method has the drawback that it inserts a dependency on having to run the constant folding algorithm before
     *  the get_datatype_info_c::is_type_equal() method is called.
     *  This is why we implement an alternative method in case the subrange limits have not yet been reduced to a cvalue!
     */
    if (    (subrange_1->lower_limit->const_value._int64.is_valid() || subrange_1->lower_limit->const_value._uint64.is_valid())
         && (subrange_2->lower_limit->const_value._int64.is_valid() || subrange_2->lower_limit->const_value._uint64.is_valid())
         && (subrange_1->upper_limit->const_value._int64.is_valid() || subrange_1->upper_limit->const_value._uint64.is_valid())
         && (subrange_2->upper_limit->const_value._int64.is_valid() || subrange_2->upper_limit->const_value._uint64.is_valid())
       ) {
      if (! (subrange_1->lower_limit->const_value == subrange_2->lower_limit->const_value)) return false;
      if (! (subrange_1->upper_limit->const_value == subrange_2->upper_limit->const_value)) return false;
    } else {
      // NOTE: nocasecmp_c() class is defined in absyntax.hh. nocasecmp_c() instantiates an object, and nocasecmp_c()() uses the () operator on that object. 
      if (! nocasecmp_c()(normalize_subrange_limit(subrange_1->lower_limit), normalize_subrange_limit(subrange_2->lower_limit))) return false;
      if (! nocasecmp_c()(normalize_subrange_limit(subrange_1->upper_limit), normalize_subrange_limit(subrange_2->upper_limit))) return false;
    }
  }

  return is_type_equal(search_base_type_c::get_basetype_decl(array_1->non_generic_type_name),
                       search_base_type_c::get_basetype_decl(array_2->non_generic_type_name));
}





bool get_datatype_info_c::is_type_valid(symbol_c *type) {
  if (NULL == type)                                                  {return false;}
  if (typeid(*type) == typeid(invalid_type_name_c))                  {return false;}
  return true;
}







/* returns the datatype the REF_TO datatype references/points to... */ 
symbol_c *get_datatype_info_c::get_ref_to(symbol_c *type_symbol) {
  ref_type_decl_c *type1 = dynamic_cast<ref_type_decl_c *>(type_symbol);
  if (NULL != type1) type_symbol = type1->ref_spec_init;

  ref_spec_init_c *type2 = dynamic_cast<ref_spec_init_c *>(type_symbol);
  if (NULL != type2) type_symbol = type2->ref_spec;

  ref_spec_c      *type3 = dynamic_cast<ref_spec_c      *>(type_symbol);
  if (NULL != type3) return type3->type_name;
  
  return NULL; /* this is not a ref datatype!! */
}






bool get_datatype_info_c::is_ref_to(symbol_c *type_symbol) {
  symbol_c *type_decl = search_base_type_c::get_basetype_decl(type_symbol);
  if (NULL == type_decl)                                                       {return false;}
  
  if (typeid(*type_decl) == typeid(ref_type_decl_c))                           {return true;}   /* identifier ':' ref_spec_init */
  if (typeid(*type_decl) == typeid(ref_spec_init_c))                           {return true;}   /* ref_spec [ ASSIGN ref_initialization ]; */
  if (typeid(*type_decl) == typeid(ref_spec_c))                                {return true;}   /* REF_TO (non_generic_type_name | function_block_type_name) */
  return false;
}




bool get_datatype_info_c::is_sfc_initstep(symbol_c *type_symbol) {
  symbol_c *type_decl = search_base_type_c::get_basetype_decl(type_symbol); 
  if (NULL == type_decl)                                             {return false;}
  if (typeid(*type_decl) == typeid(initial_step_c))                  {return true;}   /* INITIAL_STEP step_name ':' action_association_list END_STEP */  /* A pseudo data type! */
  return false;
}



bool get_datatype_info_c::is_sfc_step(symbol_c *type_symbol) {
  symbol_c *type_decl = search_base_type_c::get_basetype_decl(type_symbol); 
  if (NULL == type_decl)                                             {return false;}
  if (typeid(*type_decl) == typeid(initial_step_c))                  {return true;}   /* INITIAL_STEP step_name ':' action_association_list END_STEP */  /* A pseudo data type! */
  if (typeid(*type_decl) == typeid(        step_c))                  {return true;}   /*         STEP step_name ':' action_association_list END_STEP */  /* A pseudo data type! */
  return false;
}




bool get_datatype_info_c::is_function_block(symbol_c *type_symbol) {
  symbol_c *type_decl = search_base_type_c::get_basetype_decl(type_symbol); 
  if (NULL == type_decl)                                             {return false;}
  if (typeid(*type_decl) == typeid(function_block_declaration_c))    {return true;}   /*  FUNCTION_BLOCK derived_function_block_name io_OR_other_var_declarations function_block_body END_FUNCTION_BLOCK */
  return false;
}





bool get_datatype_info_c::is_subrange(symbol_c *type_symbol) {
  symbol_c *type_decl = search_base_type_c::get_equivtype_decl(type_symbol); /* NOTE: do NOT call search_base_type_c !! */
  if (NULL == type_decl)                                             {return false;}
  
  if (typeid(*type_decl) == typeid(subrange_type_declaration_c))     {return true;}   /*  subrange_type_name ':' subrange_spec_init */
  if (typeid(*type_decl) == typeid(subrange_spec_init_c))            {return true;}   /* subrange_specification ASSIGN signed_integer */
  if (typeid(*type_decl) == typeid(subrange_specification_c))        {return true;}   /*  integer_type_name '(' subrange')' */
    
  if (typeid(*type_decl) == typeid(subrange_c))                      {ERROR;}         /*  signed_integer DOTDOT signed_integer */
  return false;
}





bool get_datatype_info_c::is_enumerated(symbol_c *type_symbol) {
  symbol_c *type_decl = search_base_type_c::get_basetype_decl(type_symbol);
  if (NULL == type_decl)                                             {return false;}
  
  if (typeid(*type_decl) == typeid(enumerated_type_declaration_c))   {return true;}   /*  enumerated_type_name ':' enumerated_spec_init */
  if (typeid(*type_decl) == typeid(enumerated_spec_init_c))          {return true;}   /* enumerated_specification ASSIGN enumerated_value */
  if (typeid(*type_decl) == typeid(enumerated_value_list_c))         {return true;}   /* enumerated_value_list ',' enumerated_value */        /* once we change the way we handle enums, this will probably become an ERROR! */
  
  if (typeid(*type_decl) == typeid(enumerated_value_c))              {ERROR;}         /* enumerated_type_name '#' identifier */
  return false;
}





bool get_datatype_info_c::is_array(symbol_c *type_symbol) {
  symbol_c *type_decl = search_base_type_c::get_basetype_decl(type_symbol);
  if (NULL == type_decl)                                             {return false;}
  
  if (typeid(*type_decl) == typeid(array_type_declaration_c))        {return true;}   /*  identifier ':' array_spec_init */
  if (typeid(*type_decl) == typeid(array_spec_init_c))               {return true;}   /* array_specification [ASSIGN array_initialization} */
  if (typeid(*type_decl) == typeid(array_specification_c))           {return true;}   /* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
  
  if (typeid(*type_decl) == typeid(array_subrange_list_c))           {ERROR;}         /* array_subrange_list ',' subrange */
  if (typeid(*type_decl) == typeid(array_initial_elements_list_c))   {ERROR;}         /* array_initialization:  '[' array_initial_elements_list ']' */  /* array_initial_elements_list ',' array_initial_elements */
  if (typeid(*type_decl) == typeid(array_initial_elements_c))        {ERROR;}         /* integer '(' [array_initial_element] ')' */
  return false;
}





bool get_datatype_info_c::is_structure(symbol_c *type_symbol) {
  symbol_c *type_decl = search_base_type_c::get_basetype_decl(type_symbol);
  if (NULL == type_decl)                                                       {return false;}
  
  if (typeid(*type_decl) == typeid(structure_type_declaration_c))              {return true;}   /*  structure_type_name ':' structure_specification */
  if (typeid(*type_decl) == typeid(initialized_structure_c))                   {return true;}   /* structure_type_name ASSIGN structure_initialization */
  if (typeid(*type_decl) == typeid(structure_element_declaration_list_c))      {return true;}   /* structure_declaration:  STRUCT structure_element_declaration_list END_STRUCT */ /* structure_element_declaration_list structure_element_declaration ';' */
  
  if (typeid(*type_decl) == typeid(structure_element_declaration_c))           {ERROR;}         /*  structure_element_name ':' *_spec_init */
  if (typeid(*type_decl) == typeid(structure_element_initialization_list_c))   {ERROR;}         /* structure_initialization: '(' structure_element_initialization_list ')' */  /* structure_element_initialization_list ',' structure_element_initialization */
  if (typeid(*type_decl) == typeid(structure_element_initialization_c))        {ERROR;}         /*  structure_element_name ASSIGN value */
  return false;
}




bool get_datatype_info_c::is_ANY_generic_type(symbol_c *type_symbol) {
  symbol_c *type_decl = search_base_type_c::get_basetype_decl(type_symbol);
  if (NULL == type_decl)                                             {return false;}  
  if (typeid(*type_decl) == typeid(generic_type_any_c))              {return true;}   /*  The ANY keyword! */
  return false;
}


bool get_datatype_info_c::is_ANY_ELEMENTARY(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_MAGNITUDE(type_symbol))                           {return true;}
  if (is_ANY_BIT      (type_symbol))                           {return true;}
  if (is_ANY_STRING   (type_symbol))                           {return true;}
  if (is_ANY_DATE     (type_symbol))                           {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_SAFEELEMENTARY(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_SAFEMAGNITUDE(type_symbol))                       {return true;}
  if (is_ANY_SAFEBIT      (type_symbol))                       {return true;}
  if (is_ANY_SAFESTRING   (type_symbol))                       {return true;}
  if (is_ANY_SAFEDATE     (type_symbol))                       {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_ELEMENTARY_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_ELEMENTARY    (type_symbol))                      {return true;}
  if (is_ANY_SAFEELEMENTARY(type_symbol))                      {return true;}
  return false;
}







bool get_datatype_info_c::is_ANY_MAGNITUDE(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_TIME(type_symbol))                                    {return true;}
  if (is_ANY_NUM(type_symbol))                                 {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_SAFEMAGNITUDE(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_SAFETIME(type_symbol))                                {return true;}
  if (is_ANY_SAFENUM(type_symbol))                             {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_MAGNITUDE_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_MAGNITUDE    (type_symbol))                       {return true;}
  if (is_ANY_SAFEMAGNITUDE(type_symbol))                       {return true;}
  return false;
}







bool get_datatype_info_c::is_ANY_signed_MAGNITUDE(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(time_type_name_c))        {return true;}
  if (is_ANY_signed_NUM(type_symbol))                          {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_signed_SAFEMAGNITUDE(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(safetime_type_name_c))    {return true;}
  return is_ANY_signed_SAFENUM(type_symbol);
}


bool get_datatype_info_c::is_ANY_signed_MAGNITUDE_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_signed_MAGNITUDE    (type_symbol))                {return true;}
  if (is_ANY_signed_SAFEMAGNITUDE(type_symbol))                {return true;}
  return false;
}







bool get_datatype_info_c::is_ANY_NUM(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_REAL(type_symbol))                                {return true;}
  if (is_ANY_INT (type_symbol))                                {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_SAFENUM(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_SAFEREAL(type_symbol))                            {return true;}
  if (is_ANY_SAFEINT (type_symbol))                            {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_NUM_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_NUM    (type_symbol))                             {return true;}
  if (is_ANY_SAFENUM(type_symbol))                             {return true;}
  return false;
}







bool get_datatype_info_c::is_ANY_signed_NUM(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_REAL      (type_symbol))                          {return true;}
  if (is_ANY_signed_INT(type_symbol))                          {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_signed_SAFENUM(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_SAFEREAL      (type_symbol))                      {return true;}
  if (is_ANY_signed_SAFEINT(type_symbol))                      {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_signed_NUM_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_signed_NUM    (type_symbol))                      {return true;}
  if (is_ANY_signed_SAFENUM(type_symbol))                      {return true;}
  return false;
}







bool get_datatype_info_c::is_ANY_INT(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_signed_INT  (type_symbol))                        {return true;}
  if (is_ANY_unsigned_INT(type_symbol))                        {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_SAFEINT(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_signed_SAFEINT  (type_symbol))                    {return true;}
  if (is_ANY_unsigned_SAFEINT(type_symbol))                    {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_INT_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_INT    (type_symbol))                             {return true;}
  if (is_ANY_SAFEINT(type_symbol))                             {return true;}
  return false;
}






bool get_datatype_info_c::is_ANY_signed_INT(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(sint_type_name_c))        {return true;}
  if (typeid(*type_symbol) == typeid(int_type_name_c))         {return true;}
  if (typeid(*type_symbol) == typeid(dint_type_name_c))        {return true;}
  if (typeid(*type_symbol) == typeid(lint_type_name_c))        {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_signed_SAFEINT(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(safesint_type_name_c))    {return true;}
  if (typeid(*type_symbol) == typeid(safeint_type_name_c))     {return true;}
  if (typeid(*type_symbol) == typeid(safedint_type_name_c))    {return true;}
  if (typeid(*type_symbol) == typeid(safelint_type_name_c))    {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_signed_INT_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_signed_INT    (type_symbol))                      {return true;}
  if (is_ANY_signed_SAFEINT(type_symbol))                      {return true;}
  return false;
}









bool get_datatype_info_c::is_ANY_unsigned_INT(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(usint_type_name_c))       {return true;}
  if (typeid(*type_symbol) == typeid(uint_type_name_c))        {return true;}
  if (typeid(*type_symbol) == typeid(udint_type_name_c))       {return true;}
  if (typeid(*type_symbol) == typeid(ulint_type_name_c))       {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_unsigned_SAFEINT(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(safeusint_type_name_c))   {return true;}
  if (typeid(*type_symbol) == typeid(safeuint_type_name_c))    {return true;}
  if (typeid(*type_symbol) == typeid(safeudint_type_name_c))   {return true;}
  if (typeid(*type_symbol) == typeid(safeulint_type_name_c))   {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_unsigned_INT_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_unsigned_INT    (type_symbol))                    {return true;}
  if (is_ANY_unsigned_SAFEINT(type_symbol))                    {return true;}
  return false;
}








bool get_datatype_info_c::is_ANY_REAL(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(real_type_name_c))        {return true;}
  if (typeid(*type_symbol) == typeid(lreal_type_name_c))       {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_SAFEREAL(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(safereal_type_name_c))    {return true;}
  if (typeid(*type_symbol) == typeid(safelreal_type_name_c))   {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_REAL_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_REAL    (type_symbol))                            {return true;}
  if (is_ANY_SAFEREAL(type_symbol))                            {return true;}
  return false;
}








bool get_datatype_info_c::is_ANY_nBIT(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(byte_type_name_c))        {return true;}
  if (typeid(*type_symbol) == typeid(word_type_name_c))        {return true;}
  if (typeid(*type_symbol) == typeid(dword_type_name_c))       {return true;}
  if (typeid(*type_symbol) == typeid(lword_type_name_c))       {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_SAFEnBIT(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(safebyte_type_name_c))    {return true;}
  if (typeid(*type_symbol) == typeid(safeword_type_name_c))    {return true;}
  if (typeid(*type_symbol) == typeid(safedword_type_name_c))   {return true;}
  if (typeid(*type_symbol) == typeid(safelword_type_name_c))   {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_nBIT_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_nBIT    (type_symbol))                            {return true;}
  if (is_ANY_SAFEnBIT(type_symbol))                            {return true;}
  return false;
}










bool get_datatype_info_c::is_BOOL(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(bool_type_name_c))        {return true;}
  return false;
}


bool get_datatype_info_c::is_SAFEBOOL(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(safebool_type_name_c))    {return true;}
  return false;
}


bool get_datatype_info_c::is_BOOL_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_BOOL    (type_symbol))                                {return true;}
  if (is_SAFEBOOL(type_symbol))                                {return true;}
  return false;
}









bool get_datatype_info_c::is_ANY_BIT(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_BOOL    (type_symbol))                                {return true;}
  if (is_ANY_nBIT(type_symbol))                                {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_SAFEBIT(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_SAFEBOOL    (type_symbol))                            {return true;}
  if (is_ANY_SAFEnBIT(type_symbol))                            {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_BIT_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_BIT    (type_symbol))                             {return true;}
  if (is_ANY_SAFEBIT(type_symbol))                             {return true;}
  return false;
}








bool get_datatype_info_c::is_TIME(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(time_type_name_c))        {return true;}
  return false;
}


bool get_datatype_info_c::is_SAFETIME(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(safetime_type_name_c))    {return true;}
  return false;
}


bool get_datatype_info_c::is_TIME_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_TIME    (type_symbol))                                {return true;}
  if (is_SAFETIME(type_symbol))                                {return true;}
  return false;
}








bool get_datatype_info_c::is_ANY_DATE(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(date_type_name_c))        {return true;}
  if (typeid(*type_symbol) == typeid(tod_type_name_c))         {return true;}
  if (typeid(*type_symbol) == typeid(dt_type_name_c))          {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_SAFEDATE(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(safedate_type_name_c))    {return true;}
  if (typeid(*type_symbol) == typeid(safetod_type_name_c))     {return true;}
  if (typeid(*type_symbol) == typeid(safedt_type_name_c))      {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_DATE_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_DATE    (type_symbol))                            {return true;}
  if (is_ANY_SAFEDATE(type_symbol))                            {return true;}
  return false;
}











bool get_datatype_info_c::is_ANY_STRING(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(string_type_name_c))      {return true;}
  if (typeid(*type_symbol) == typeid(wstring_type_name_c))     {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_SAFESTRING(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (typeid(*type_symbol) == typeid(safestring_type_name_c))  {return true;}
  if (typeid(*type_symbol) == typeid(safewstring_type_name_c)) {return true;}
  return false;
}


bool get_datatype_info_c::is_ANY_STRING_compatible(symbol_c *type_symbol) {
  if (type_symbol == NULL)                                     {return false;}
  if (is_ANY_STRING    (type_symbol))                          {return true;}
  if (is_ANY_SAFESTRING(type_symbol))                          {return true;}
  return false;
}







/* Can't we do away with this?? */
bool get_datatype_info_c::is_ANY_REAL_literal(symbol_c *type_symbol) {
  if (type_symbol == NULL)                              {return true;} /* Please make sure things will work correctly before changing this to false!! */
  if (typeid(*type_symbol) == typeid(real_c))           {return true;}
  if (typeid(*type_symbol) == typeid(neg_real_c))       {return true;}
  return false;
}

/* Can't we do away with this?? */
bool get_datatype_info_c::is_ANY_INT_literal(symbol_c *type_symbol) {
  if (type_symbol == NULL)                              {return true;} /* Please make sure things will work correctly before changing this to false!! */
  if (typeid(*type_symbol) == typeid(integer_c))        {return true;}
  if (typeid(*type_symbol) == typeid(neg_integer_c))    {return true;}
  if (typeid(*type_symbol) == typeid(binary_integer_c)) {return true;}
  if (typeid(*type_symbol) == typeid(octal_integer_c))  {return true;}
  if (typeid(*type_symbol) == typeid(hex_integer_c))    {return true;}
  return false;
}










invalid_type_name_c      get_datatype_info_c::invalid_type_name;

/**********************/
/* B.1.3 - Data types */
/**********************/
/***********************************/
/* B 1.3.1 - Elementary Data Types */
/***********************************/
lreal_type_name_c        get_datatype_info_c::lreal_type_name;
real_type_name_c         get_datatype_info_c::real_type_name;

lint_type_name_c         get_datatype_info_c::lint_type_name;
dint_type_name_c         get_datatype_info_c::dint_type_name;
int_type_name_c          get_datatype_info_c::int_type_name;
sint_type_name_c         get_datatype_info_c::sint_type_name;

ulint_type_name_c        get_datatype_info_c::ulint_type_name;
udint_type_name_c        get_datatype_info_c::udint_type_name;
uint_type_name_c         get_datatype_info_c::uint_type_name;
usint_type_name_c        get_datatype_info_c::usint_type_name;

lword_type_name_c        get_datatype_info_c::lword_type_name;
dword_type_name_c        get_datatype_info_c::dword_type_name;
word_type_name_c         get_datatype_info_c::word_type_name;
byte_type_name_c         get_datatype_info_c::byte_type_name;
bool_type_name_c         get_datatype_info_c::bool_type_name;

wstring_type_name_c      get_datatype_info_c::wstring_type_name;
string_type_name_c       get_datatype_info_c::string_type_name;

dt_type_name_c           get_datatype_info_c::dt_type_name;
date_type_name_c         get_datatype_info_c::date_type_name;
tod_type_name_c          get_datatype_info_c::tod_type_name;
time_type_name_c         get_datatype_info_c::time_type_name;


/******************************************************/
/* Extensions to the base standard as defined in      */
/* "Safety Software Technical Specification,          */
/*  Part 1: Concepts and Function Blocks,             */
/*  Version 1.0 – Official Release"                   */
/* by PLCopen - Technical Committee 5 - 2006-01-31    */
/******************************************************/  
safelreal_type_name_c    get_datatype_info_c::safelreal_type_name;
safereal_type_name_c     get_datatype_info_c::safereal_type_name;

safelint_type_name_c     get_datatype_info_c::safelint_type_name;
safedint_type_name_c     get_datatype_info_c::safedint_type_name;
safeint_type_name_c      get_datatype_info_c::safeint_type_name;
safesint_type_name_c     get_datatype_info_c::safesint_type_name;

safeulint_type_name_c    get_datatype_info_c::safeulint_type_name;
safeudint_type_name_c    get_datatype_info_c::safeudint_type_name;
safeuint_type_name_c     get_datatype_info_c::safeuint_type_name;
safeusint_type_name_c    get_datatype_info_c::safeusint_type_name;

safelword_type_name_c    get_datatype_info_c::safelword_type_name;
safedword_type_name_c    get_datatype_info_c::safedword_type_name;
safeword_type_name_c     get_datatype_info_c::safeword_type_name;
safebyte_type_name_c     get_datatype_info_c::safebyte_type_name;
safebool_type_name_c     get_datatype_info_c::safebool_type_name;

safewstring_type_name_c  get_datatype_info_c::safewstring_type_name;
safestring_type_name_c   get_datatype_info_c::safestring_type_name;

safedt_type_name_c       get_datatype_info_c::safedt_type_name;
safedate_type_name_c     get_datatype_info_c::safedate_type_name;
safetod_type_name_c      get_datatype_info_c::safetod_type_name;
safetime_type_name_c     get_datatype_info_c::safetime_type_name;





