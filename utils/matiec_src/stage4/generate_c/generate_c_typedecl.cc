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
#include <stdlib.h>


/* Ths class contains two main classes:
 *    - generate_c_typedecl_c
 *    - generate_c_implicit_typedecl_c
 * 
 * and an auxiliary class
 *    - generate_datatypes_aliasid_c
 * 
 * 
 * Both the generate_c_typedecl_c and the generate_c_implicit_typedecl_c may set a stage4
 * annotation (in the stage4 annotation map of each symbol_c) named 
 *   "generate_c_annotaton__implicit_type_id"
 * If this annotation is set, the generate_c_base_c will print out this value instead of 
 * the datatype's name!
 * 
 * 
 * 
 * generate_c_typedecl_c
 * ---------------------
 *   Given a datatype object (i.e. an object in the AST that is also used to define a datatype,
 *    typically one that may be returned by search_basetype_c), this class will generate the 
 *    C code to declare an equivakent datatype in C.
 *   Note that array datatypes are handled in a special way; instead of using the name given
 *    to it in the IEC 61131-3 source code, and new alias is created for the datatype name in C.
 *    Eplanations why we do this may be found further on...
 * 
 * 
 * generate_c_implicit_typedecl_c
 * ------------------------------
 *   Given a POU or a derived datatype declaration, it will search for any implicitly defined 
 *    datatypes in that POU/datatype declaration. Implicit datatypes are datatypes that are not 
 *    explicitly declared and given a name. Example:
 *       VAR a: ARRAY [9..11] of INT; END_VAR
 *    Here, the array is implictly delcared.
 *    For eac implicitly defined datatype, an alias for that datatype is created (by calling
 *    generate_datatypes_aliasid_c), and a C declaration is generated in C source code (by
 *    calling generate_c_typedecl_c).
 * 
 * 
 * generate_datatypes_aliasid_c
 * ----------------------------
 *   Given a datatype object (i.e. an object in the AST that defines a datatype), it will create
 *    an alias name for that datatype.
 *    This class is used by both the generate_c_implicit_typedecl_c, and the generate_c_typedecl_c
 *    classes!
 */






/* generate an alias/name (identifier) for array and REF_TO datatypes */
/*
 *  The generated alias is created based on the structure of the datatype itself, in order to
 *  guarantee that any two datatypes that have the same internal format will result in the same
 *  alias.
 *   examples:
 *      ARRAY [9..11] of INT        --> __ARRAY_9_11_OF_INT
 *      REF_TO INT                  --> __REF_TO_INT
 *      ARRAY [9..11] of REF_TO INT --> __ARRAY_9_11_OF___REF_TO_INT
 */
class generate_datatypes_aliasid_c: fcall_visitor_c {

  private:
    //std::map<std::string, int> inline_array_defined;
    std::string current_array_name;
    static generate_datatypes_aliasid_c *singleton_;

  public:
    generate_datatypes_aliasid_c(void) {};

    virtual ~generate_datatypes_aliasid_c(void) {
      //inline_array_defined.clear(); // Not really necessary...
    }

    /* implement the virtual member function declared in fcall_visitor_c */
    // by default generate an ERROR if a visit method is called, unless it is explicitly handled in generate_datatypes_aliasid_c
    void fcall(symbol_c *symbol) {ERROR;} 

    static identifier_c *create_id(symbol_c *symbol) {
      if (NULL == singleton_) singleton_ = new generate_datatypes_aliasid_c();
      if (NULL == singleton_) ERROR;
      singleton_->current_array_name = "";
      symbol->accept(*singleton_);
      const char *str1 = singleton_->current_array_name.c_str();
      char       *str2 = (char *)malloc(strlen(str1)+1);
      if (NULL == str2) ERROR;
      strcpy(str2, str1);
      identifier_c *id = new identifier_c(str2);
      /* Copy all the anotations in the symbol_c object 'symbol' to the newly created 'id' object
       *   This includes the location (in the IEC 61131-3 source file) annotations set in stage1_2,
       *   the symbol->datatype set in stage3, and any other anotaions that may be created in the future!
       */
      *(dynamic_cast<symbol_c *>(id)) = *(dynamic_cast<symbol_c *>(symbol));
      return id;
    }
    
    /*************************/
    /* B.1 - Common elements */
    /*************************/
    /**********************/
    /* B.1.3 - Data types */
    /**********************/
    /***********************************/
    /* B 1.3.1 - Elementary Data Types */
    /***********************************/
    /***********************************/
    /* B 1.3.2 - Generic Data Types    */
    /***********************************/
    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
    /* ref_spec:  REF_TO (non_generic_type_name | function_block_type_name) */
    void *visit(ref_spec_c *symbol) {
      current_array_name = "__REF_TO_";
      current_array_name += get_datatype_info_c::get_id_str(symbol->type_name);
      return NULL;
    }

    /******************************************/
    /* B 1.4.3 - Declaration & Initialization */
    /******************************************/
    /* array_specification [ASSIGN array_initialization] */
    /* array_initialization may be NULL ! */
    void *visit(array_spec_init_c *symbol) {
      if (NULL == symbol->datatype) ERROR;
      symbol->datatype->accept(*this); // the base datatype should be an array_specification_c !!
      return NULL;
    }

    /* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
    void *visit(array_specification_c *symbol) {
      current_array_name = "__ARRAY_OF_";
      if (    get_datatype_info_c::is_ref_to(symbol->non_generic_type_name)
          && (get_datatype_info_c::get_ref_to(symbol->non_generic_type_name) != NULL)) {
        /* handle situations where we have 2 impliclitly defined datatype, namely a REF_TO inside an ARRAY
         *    e.g. TYPE array_of_ref_to_sint : ARRAY [1..3] OF  REF_TO  SINT; END_TYPE
         * The second condition (get_datatype_info_c::get_ref_to(symbol->non_generic_type_name) != NULL)
         * in the above if() is to make sure we use the standard algorithm if the array is of a previously 
         * defined REF_TO type, in which case symbol->non_generic_type_name will reference an identifier_c!
         *    e.g. TYPE array_of_ref_to_sint : ARRAY [1..3] OF  REF_TO  SINT; END_TYPE
         */
        current_array_name += "__REF_TO_";
        current_array_name += get_datatype_info_c::get_id_str(get_datatype_info_c::get_ref_to(symbol->non_generic_type_name));
      } else {
        current_array_name += get_datatype_info_c::get_id_str(symbol->non_generic_type_name);
      }
      symbol->array_subrange_list->accept(*this);
      return NULL;
    }

    /* helper symbol for array_specification */
    /* array_subrange_list ',' subrange */
    void *visit(array_subrange_list_c *symbol) {
      for(int i = 0; i < symbol->n; i++) {symbol->elements[i]->accept(*this);}
      return NULL;
    }

    /*  signed_integer DOTDOT signed_integer */
    //SYM_REF2(subrange_c, lower_limit, upper_limit)
    void *visit(subrange_c *symbol) {
      current_array_name += "_";
      std::stringstream ss;
      ss << symbol->dimension;
      current_array_name += ss.str();
      return NULL;
    }


};


generate_datatypes_aliasid_c *generate_datatypes_aliasid_c::singleton_ = NULL;







/***************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/

/* Given an object in the AST that defines a datatype, generate the C source code that declares an
 * equivalent dataype in C.
 * WARNING: This class maintains internal state in the datatypes_already_defined map.
 *          Using multiple isntances of this class may result in different C source code
 *          compared to when a single instance of this class is used for all datatype declarations!
 * 
 * Except for arrays, the C datatype will have the same name as the name of the datatype in the
 * IEC 61131-3 source code.
 * For arrays an alias is created for each datatype. This alias has the property of being equal
 * for arrays with the same internal structure.
 * 
 * Example:
 *  TYPE
 *      array1: ARRAY [9..11] of INT;
 *      array2: ARRAY [9..11] of INT;
 *  END_TYPE
 * 
 * will result in both arrays having the same name (__ARRAY_9_11_OF_INT) in the C source code.
 * 
 * A single C datatype declaration will be generated for both arrays
 *  (the datatypes_already_defined keeps track of which datatypes have already been declared in C)
 * This method of handling arrays is needed when the relaxed datatype model is used 
 * (see get_datatype_info_c for explanation on the relaxed datatype model).
 */
/* The generate_c_typedecl_c inherits from generate_c_base_and_typeid_c because it will need the visitor's() to
 *   identifier_c, derived_datatype_identifier_c, and enumerated_value_c
 */
class generate_c_typedecl_c: public generate_c_base_and_typeid_c {

  protected:
    /* The following member variable is completely useless - the s4o variable inherited from generate_c_base_and_typeid_c
     * could be used to the same effect. We keep it here merely because this generate_c_typedecl_c will typically be called
     * with s4o referencing an include file (typically POUS.h), and using s4o_incl throughout this code will help the reader
     * of the code to keep this fact in mind.
     */
    stage4out_c &s4o_incl;

  private:
    symbol_c* current_type_name;
    std::map<std::string, int> datatypes_already_defined;
    /* Although this generate_c_typedecl_c inherits directly from generate_c_base_and_typeid_c, we still need an independent
     * instance of that base class. This is because generate_c_typedecl_c will overload some of the visitors in the base class
     * generate_c_base_and_typeid_c.
     *  When we want the to use the version of these visitors() in generate_c_typedecl_c,        we call accept(*this);
     *  When we want the to use the version of these visitors() in generate_c_base_and_typeid_c, we call accept(*generate_c_typeid);
     */
    generate_c_base_and_typeid_c *generate_c_typeid;

    
  public:
    generate_c_typedecl_c(stage4out_c *s4o_ptr): generate_c_base_and_typeid_c(s4o_ptr), s4o_incl(*s4o_ptr) /*, generate_c_print_typename(s4o_ptr) */{
      current_typedefinition = none_td;
      current_basetypedeclaration = none_bd;
      current_type_name = NULL;
      generate_c_typeid = new generate_c_base_and_typeid_c(&s4o_incl);
    }
    ~generate_c_typedecl_c(void) {
      delete generate_c_typeid;
    }

    typedef enum {
      none_td,
      enumerated_td,
      subrange_td,
      array_td,
      struct_td
    } typedefinition_t;

    typedefinition_t current_typedefinition;

    typedef enum {
      none_bd,
      subrangebasetype_bd,
      subrangetest_bd,
      arraysubrange_bd
    } basetypedeclaration_t;

    basetypedeclaration_t current_basetypedeclaration;

    void *print_list_incl(list_c *list,
         std::string pre_elem_str = "",
         std::string inter_elem_str = "",
         std::string post_elem_str = "") {

      if (list->n > 0) {
        s4o_incl.print(pre_elem_str);
        list->elements[0]->accept(*this);
      }

      for(int i = 1; i < list->n; i++) {
        s4o_incl.print(inter_elem_str);
        list->elements[i]->accept(*this);
      }

      if (list->n > 0)
        s4o_incl.print(post_elem_str);

      return NULL;
    }


/***************************/
/* B 0 - Programming Model */
/***************************/
  /* leave for derived classes... */

/*************************/
/* B.1 - Common elements */
/*************************/
/*******************************************/
/* B 1.1 - Letters, digits and identifiers */
/*******************************************/
/*********************/
/* B 1.2 - Constants */
/*********************/
  /* originally empty... */

/******************************/
/* B 1.2.1 - Numeric Literals */
/******************************/
  /* done in base class(es) */

/*******************************/
/* B.1.2.2   Character Strings */
/*******************************/
  /* done in base class(es) */

/***************************/
/* B 1.2.3 - Time Literals */
/***************************/
/************************/
/* B 1.2.3.1 - Duration */
/************************/
  /* done in base class(es) */

/************************************/
/* B 1.2.3.2 - Time of day and Date */
/************************************/
  /* done in base class(es) */

/**********************/
/* B.1.3 - Data types */
/**********************/
/***********************************/
/* B 1.3.1 - Elementary Data Types */
/***********************************/
  /* done in base class(es) */

/********************************/
/* B.1.3.2 - Generic data types */
/********************************/
  /* originally empty... */

/********************************/
/* B 1.3.3 - Derived data types */
/********************************/
/*  subrange_type_name ':' subrange_spec_init */
void *visit(subrange_type_declaration_c *symbol) {
  TRACE("subrange_type_declaration_c");  
  
  current_typedefinition = subrange_td;
  current_type_name = symbol->subrange_type_name;

  s4o_incl.print("__DECLARE_DERIVED_TYPE(");
  current_type_name->accept(*generate_c_typeid);
  s4o_incl.print(",");
  current_basetypedeclaration = subrangebasetype_bd;
  symbol->subrange_spec_init->accept(*this); // always calls subrange_spec_init_c
  current_basetypedeclaration = none_bd;
  s4o_incl.print(")\n");
  
  current_basetypedeclaration = subrangetest_bd;
  symbol->subrange_spec_init->accept(*this); // always calls subrange_spec_init_c
  current_basetypedeclaration = none_bd;
  
  current_type_name = NULL;
  current_typedefinition = none_td;

  return NULL;
}

/* subrange_specification ASSIGN signed_integer */
void *visit(subrange_spec_init_c *symbol) {
  TRACE("subrange_spec_init_c");
  symbol->subrange_specification->accept(*this); // always calls subrange_specification_c
  return NULL;
}

/*  integer_type_name '(' subrange')' */
void *visit(subrange_specification_c *symbol) {
  TRACE("subrange_specification_c");
  if (current_typedefinition == subrange_td) {
    switch (current_basetypedeclaration) {
      case subrangebasetype_bd:
        symbol->integer_type_name->accept(*generate_c_typeid);
        break;
      case subrangetest_bd:
        if (symbol->subrange != NULL) {
          s4o_incl.print("static inline ");
          current_type_name->accept(*generate_c_typeid);
          s4o_incl.print(" __CHECK_");
          current_type_name->accept(*generate_c_typeid);
          s4o_incl.print("(");
          current_type_name->accept(*generate_c_typeid);
          s4o_incl.print(" value) {\n");
          s4o_incl.indent_right();

          /* NOTE: IEC 61131-3 v2 syntax mandates that the integer type name be one of SINT, ..., LINT, USINT, ... ULIT */
          /*       For this reason, the following condition will always be false, and therefore this is a block
           *       of dead code. However, let's not delete it for now. It might come in useful for IEC 61131-3 v3.
           *       For the moment, we just comment it out!
           */
          /*
          if (get_datatype_info_c::is_subrange(symbol->integer_type_name)) {
            s4o_incl.print(s4o_incl.indent_spaces + "value = __CHECK_");
            symbol->integer_type_name->accept(*this);
            s4o_incl.print("(value);\n");
          }
          */

          symbol->subrange->accept(*this); // always calls subrange_c

          s4o_incl.indent_left();
          s4o_incl.print("}\n");
        }
        else {
          s4o_incl.print("#define __CHECK_");
          current_type_name->accept(*generate_c_typeid);
          s4o_incl.print(" __CHECK_");
          symbol->integer_type_name->accept(*generate_c_typeid);
          s4o_incl.print("\n");
        }
        break;
      default:
        break;
    }
  }
  return NULL;
}


/*  signed_integer DOTDOT signed_integer */
void *visit(subrange_c *symbol) {
  TRACE("subrange_c");
  int dimension;
  switch (current_typedefinition) {
    case array_td:
      if (current_basetypedeclaration == arraysubrange_bd) {
        s4o_incl.print("[");
        s4o_incl.print(symbol->dimension);
        s4o_incl.print("]");
      }
      else
        symbol->lower_limit->accept(*this);  // always calls neg_integer_c or integer_c
      break;
    case subrange_td:
      s4o_incl.print(s4o_incl.indent_spaces + "if (value < ");
      symbol->lower_limit->accept(*generate_c_typeid);
      s4o_incl.print(")\n");
      s4o_incl.indent_right();
      s4o_incl.print(s4o_incl.indent_spaces + "return ");
      symbol->lower_limit->accept(*generate_c_typeid);
      s4o_incl.print(";\n");
      s4o_incl.indent_left();
      s4o_incl.print(s4o_incl.indent_spaces + "else if (value > ");
      symbol->upper_limit->accept(*generate_c_typeid);
      s4o_incl.print(")\n");
      s4o_incl.indent_right();
      s4o_incl.print(s4o_incl.indent_spaces + "return ");
      symbol->upper_limit->accept(*generate_c_typeid);
      s4o_incl.print(";\n");
      s4o_incl.indent_left();
      s4o_incl.print(s4o_incl.indent_spaces + "else\n");
      s4o_incl.indent_right();
      s4o_incl.print(s4o_incl.indent_spaces + "return value;\n");
      s4o_incl.indent_left();
    default:
      break;
  }
  return NULL;
}

/*  enumerated_type_name ':' enumerated_spec_init */
void *visit(enumerated_type_declaration_c *symbol) {
  TRACE("enumerated_type_declaration_c");
  
  current_typedefinition = enumerated_td;
  current_type_name = symbol->enumerated_type_name;

  s4o_incl.print("__DECLARE_ENUMERATED_TYPE(");
  current_type_name->accept(*generate_c_typeid);
  s4o_incl.print(",\n");
  s4o_incl.indent_right();
  symbol->enumerated_spec_init->accept(*this); // always calls enumerated_spec_init_c
  s4o_incl.indent_left();
  s4o_incl.print(")\n");

  current_type_name = NULL;
  current_typedefinition = none_td;

  return NULL;
}

/* enumerated_specification ASSIGN enumerated_value */
void *visit(enumerated_spec_init_c *symbol) {
  TRACE("enumerated_spec_init_c");
  if (current_typedefinition == enumerated_td)
    symbol->enumerated_specification->accept(*this); // always calls enumerated_value_list_c or derived_datatype_identifier_c
  else
    symbol->enumerated_specification->accept(*generate_c_typeid);
  return NULL;
}

/* helper symbol for enumerated_specification->enumerated_spec_init */
/* enumerated_value_list ',' enumerated_value */
void *visit(enumerated_value_list_c *symbol) {
  TRACE("enumerated_value_list_c");
  print_list_incl(symbol, s4o_incl.indent_spaces, ",\n"+s4o_incl.indent_spaces, "\n");  // will always call enumerated_value_c
  return NULL;
}

/* enumerated_type_name '#' identifier */
/* Handled by generate_c_base_c class!!
void *visit(enumerated_value_c *symbol) {}
*/

/*  identifier ':' array_spec_init */
void *visit(array_type_declaration_c *symbol) {
  TRACE("array_type_declaration_c");
  
  // NOTE: remeber that symbol->array_spec_init may point to a derived_datatype_identifier_c, which is why we use symbol->array_spec_init->datatype instead!
  if (NULL == symbol->array_spec_init->datatype) ERROR;
  identifier_c *id = generate_datatypes_aliasid_c::create_id(symbol->array_spec_init->datatype);
  
  /* NOTE  An array_type_declaration_c will be created in stage4 for each implicitly defined array,
   *       and this generate_c_typedecl_c will be called to define that array in C.
   *       However, every implictly defined array with the exact same parameters will be mapped
   *       to the same identifier (e.g: __ARRAY_OF_INT_33 where 33 is the number of elements in the array).
   *       In order for the C compiler not to find the same datatype being defined two or more times, 
   *       we will keep track of the array datatypes that have already been declared, and henceforth
   *       only declare arrays that have not been previously defined.
   */
  if (datatypes_already_defined.find(id->value) != datatypes_already_defined.end())
    goto end; // already defined. No need to define it again!!
  datatypes_already_defined[id->value] = 1; // insert this datatype into the list of already defined arrays!
  
  current_typedefinition = array_td;
  current_type_name = id;

  s4o_incl.print("__DECLARE_ARRAY_TYPE(");
  current_type_name->accept(*generate_c_typeid);
  s4o_incl.print(",");
  symbol->array_spec_init->accept(*this); // always calls array_spec_init_c
  s4o_incl.print(")\n");

  current_type_name = NULL;
  current_typedefinition = none_td;

end:  
  symbol                 ->anotations_map["generate_c_annotaton__implicit_type_id"] = id;
  symbol->datatype       ->anotations_map["generate_c_annotaton__implicit_type_id"] = id;
  symbol->array_spec_init->anotations_map["generate_c_annotaton__implicit_type_id"] = id; // probably not needed, bu let's play safe.
  
  return NULL;
}



/* array_specification [ASSIGN array_initialization] */
/* array_initialization may be NULL ! */
void *visit(array_spec_init_c *symbol) {
  TRACE("array_spec_init_c");  
  symbol->array_specification->accept(*this); // always calls array_specification_c or derived_datatype_identifier_c
  return NULL;
}

/* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
void *visit(array_specification_c *symbol) {
  TRACE("array_specification_c");
  // The 2nd and 3rd argument of a call to the __DECLARE_ARRAY_TYPE macro!
  symbol->non_generic_type_name->accept(/*generate_c_print_typename*/*generate_c_typeid);
  s4o_incl.print(",");
  current_basetypedeclaration = arraysubrange_bd;
  symbol->array_subrange_list->accept(*this); // always calls array_subrange_list_c, which the iterator_visitor_c base class will call subrange_c
  current_basetypedeclaration = none_bd;
  return NULL;
}


/*  TYPE type_declaration_list END_TYPE */
void *visit(data_type_declaration_c *symbol) {
  TRACE("data_type_declaration_c");
  symbol->type_declaration_list->accept(*this); // will always call type_declaration_list_c
  s4o_incl.print("\n\n");
  return NULL;
}

/* helper symbol for data_type_declaration */
void *visit(type_declaration_list_c *symbol) {
  TRACE("type_declaration_list_c");
  return print_list_incl(symbol, "", "\n", "\n"); // will always call string_type_declaration_c, structure_type_declaration_c, array_type_declaration_c, simple_type_declaration_c, subrange_type_declaration_c, enumerated_type_declaration_c, ref_type_decl_c
}

/*  simple_type_name ':' simple_spec_init */
void *visit(simple_type_declaration_c *symbol) {
  TRACE("simple_type_declaration_c");

  s4o_incl.print("__DECLARE_DERIVED_TYPE(");
  symbol->simple_type_name->accept(*generate_c_typeid);
  s4o_incl.print(",");
  symbol->simple_spec_init->accept(*this); // always calls simple_spec_init_c
  s4o_incl.print(")\n");

  if (get_datatype_info_c::is_subrange(symbol->simple_type_name)) {
    s4o_incl.print("#define __CHECK_");
    current_type_name->accept(*generate_c_typeid);
    s4o_incl.print(" __CHECK_");
    symbol->simple_spec_init->accept(*this); // always calls simple_spec_init_c
    s4o_incl.print("\n");
  }

  return NULL;
}

/* simple_specification [ASSIGN constant] */
//SYM_REF2(simple_spec_init_c, simple_specification, constant)
// <constant> may be NULL
void *visit(simple_spec_init_c *symbol) {
  TRACE("simple_spec_init_c");
  symbol->simple_specification->accept(*generate_c_typeid);
  return NULL;
}

#if 0
/*  subrange_type_name ':' subrange_spec_init */
SYM_REF2(subrange_type_declaration_c, subrange_type_name, subrange_spec_init)

/* subrange_specification ASSIGN signed_integer */
SYM_REF2(subrange_spec_init_c, subrange_specification, signed_integer)

/*  integer_type_name '(' subrange')' */
SYM_REF2(subrange_specification_c, integer_type_name, subrange)

/*  signed_integer DOTDOT signed_integer */
SYM_REF2(subrange_c, lower_limit, upper_limit)

/*  enumerated_type_name ':' enumerated_spec_init */
SYM_REF2(enumerated_type_declaration_c, enumerated_type_name, enumerated_spec_init)

/* enumerated_specification ASSIGN enumerated_value */
SYM_REF2(enumerated_spec_init_c, enumerated_specification, enumerated_value)

/* helper symbol for enumerated_specification->enumerated_spec_init */
/* enumerated_value_list ',' enumerated_value */
SYM_LIST(enumerated_value_list_c)

/* enumerated_type_name '#' identifier */
SYM_REF2(enumerated_value_c, type, value)

/*  identifier ':' array_spec_init */
SYM_REF2(array_type_declaration_c, identifier, array_spec_init)

/* array_specification [ASSIGN array_initialization} */
/* array_initialization may be NULL ! */
SYM_REF2(array_spec_init_c, array_specification, array_initialization)

/* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
SYM_REF2(array_specification_c, array_subrange_list, non_generic_type_name)

/* helper symbol for array_specification */
/* array_subrange_list ',' subrange */
SYM_LIST(array_subrange_list_c)

/* array_initialization:  '[' array_initial_elements_list ']' */
/* helper symbol for array_initialization */
/* array_initial_elements_list ',' array_initial_elements */
SYM_LIST(array_initial_elements_list_c)

/* integer '(' [array_initial_element] ')' */
/* array_initial_element may be NULL ! */
SYM_REF2(array_initial_elements_c, integer, array_initial_element)
#endif

/*  structure_type_name ':' structure_specification */
//SYM_REF2(structure_type_declaration_c, structure_type_name, structure_specification)
void *visit(structure_type_declaration_c *symbol) {
  TRACE("structure_type_declaration_c");

  current_typedefinition = struct_td;

  s4o_incl.print("__DECLARE_STRUCT_TYPE(");
  symbol->structure_type_name->accept(*generate_c_typeid);
  s4o_incl.print(",");
  symbol->structure_specification->accept(*this); // always calls initialized_structure_c or structure_element_declaration_list
  s4o_incl.print(")\n");

  current_typedefinition = none_td;

  return NULL;
}

/* structure_type_name ASSIGN structure_initialization */
/* structure_initialization may be NULL ! */
//SYM_REF2(initialized_structure_c, structure_type_name, structure_initialization)
void *visit(initialized_structure_c *symbol) {
  TRACE("initialized_structure_c");
  symbol->structure_type_name->accept(*generate_c_typeid);
  return NULL;
}

/* helper symbol for structure_declaration */
/* structure_declaration:  STRUCT structure_element_declaration_list END_STRUCT */
/* structure_element_declaration_list structure_element_declaration ';' */
//SYM_LIST(structure_element_declaration_list_c)
void *visit(structure_element_declaration_list_c *symbol) {
  TRACE("structure_element_declaration_list_c");
  s4o_incl.print("\n");
  s4o_incl.indent_right();
  s4o_incl.print(s4o_incl.indent_spaces);

  print_list_incl(symbol, "", s4o_incl.indent_spaces, ""); // will always call structure_element_declaration_c

  s4o_incl.indent_left();
  s4o_incl.print(s4o_incl.indent_spaces);
  return NULL;
}

/*  structure_element_name ':' spec_init */
//SYM_REF2(structure_element_declaration_c, structure_element_name, spec_init)
void *visit(structure_element_declaration_c *symbol) {
  TRACE("structure_element_declaration_c");

  symbol->spec_init->accept(/*generate_c_print_typename*/*generate_c_typeid);
  s4o_incl.print(" ");
  symbol->structure_element_name->accept(*generate_c_typeid);
  s4o_incl.print(";\n");
  s4o_incl.print(s4o_incl.indent_spaces);

  return NULL;
}

/* helper symbol for structure_initialization */
/* structure_initialization: '(' structure_element_initialization_list ')' */
/* structure_element_initialization_list ',' structure_element_initialization */
//SYM_LIST(structure_element_initialization_list_c)
void *visit(structure_element_initialization_list_c *symbol) {
  TRACE("structure_element_initialization_list_c");

  // TODO ???
  //ERROR;
  return NULL;
}

/*  structure_element_name ASSIGN value */
//SYM_REF2(structure_element_initialization_c, structure_element_name, value)
void *visit(structure_element_initialization_c *symbol) {
  TRACE("structure_element_initialization_c");

  // TODO ???
  //ERROR;
  return NULL;
}

#if 0
/*  string_type_name ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init */
/*
 * NOTE:
 * (Summary: Contrary to what is expected, the
 *           string_type_declaration_c is not used to store
 *           simple string type declarations that do not include
 *           size limits.
 *           For e.g.:
 *             str1_type: STRING := "hello!"
 *           will be stored in a simple_type_declaration_c
 *           instead of a string_type_declaration_c.
 *           The following:
 *             str2_type: STRING [64] := "hello!"
 *           will be stored in a sring_type_declaration_c
 *
 *           Read on for why this is done...
 * End Summary)
 *
 * According to the spec, the valid construct
 * TYPE new_str_type : STRING := "hello!"; END_TYPE
 * has two possible routes to type_declaration...
 *
 * Route 1:
 * type_declaration: single_element_type_declaration
 * single_element_type_declaration: simple_type_declaration
 * simple_type_declaration: identifier ':' simple_spec_init
 * simple_spec_init: simple_specification ASSIGN constant
 * (shift:  identifier <- 'new_str_type')
 * simple_specification: elementary_type_name
 * elementary_type_name: STRING
 * (shift: elementary_type_name <- STRING)
 * (reduce: simple_specification <- elementary_type_name)
 * (shift: constant <- "hello!")
 * (reduce: simple_spec_init: simple_specification ASSIGN constant)
 * (reduce: ...)
 *
 *
 * Route 2:
 * type_declaration: string_type_declaration
 * string_type_declaration: identifier ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init
 * (shift:  identifier <- 'new_str_type')
 * elementary_string_type_name: STRING
 * (shift: elementary_string_type_name <- STRING)
 * (shift: string_type_declaration_size <-  empty )
 * string_type_declaration_init: ASSIGN character_string
 * (shift: character_string <- "hello!")
 * (reduce: string_type_declaration_init <- ASSIGN character_string)
 * (reduce: string_type_declaration <- identifier ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init )
 * (reduce: type_declaration <- string_type_declaration)
 *
 *
 * At first glance it seems that removing route 1 would make
 * the most sense. Unfortunately the construct 'simple_spec_init'
 * shows up multiple times in other rules, so changing this construct
 * would also mean changing all the rules in which it appears.
 * I (Mario) therefore chose to remove route 2 instead. This means
 * that the above declaration gets stored in a
 * simple_type_declaration_c, and not in a string_type_declaration_c
 * as would be expected!
 */
/*  string_type_name ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init */
SYM_REF4(string_type_declaration_c,	string_type_name,
					elementary_string_type_name,
					string_type_declaration_size,
					string_type_declaration_init) /* may be == NULL! */
#endif



/* ref_spec:  REF_TO (non_generic_type_name | function_block_type_name) */
// SYM_REF1(ref_spec_c, type_name)
void *visit(ref_spec_c *symbol) { 
  symbol->type_name->accept(/*generate_c_print_typename*/*generate_c_typeid);
  s4o_incl.print("*");
  return NULL;
}

/* For the moment, we do not support initialising reference data types */
/* ref_spec_init: ref_spec [ ASSIGN ref_initialization ] */ 
/* NOTE: ref_initialization may be NULL!! */
// SYM_REF2(ref_spec_init_c, ref_spec, ref_initialization)
void *visit(ref_spec_init_c *symbol) {
  return symbol->ref_spec->accept(*generate_c_typeid);
}

/* ref_type_decl: identifier ':' ref_spec_init */
// SYM_REF2(ref_type_decl_c, ref_type_name, ref_spec_init)
void *visit(ref_type_decl_c *symbol) {
  TRACE("ref_type_decl_c");

  /* NOTE  An ref_type_decl_c will be created in stage4 for each implicitly defined REF_TO datatype,
   *       and this generate_c_typedecl_c will be called to define that REF_TO datatype in C.
   *       However, every implictly defined REF_TO datatype with the exact same parameters will be mapped
   *       to the same identifier (e.g: __REF_TO_INT).
   *       In order for the C compiler not to find the same datatype being defined two or more times, 
   *       we will keep track of the datatypes that have already been declared, and henceforth
   *       only declare the datatypes that have not been previously defined.
   */
  identifier_c *tmp_id;
  tmp_id = dynamic_cast<identifier_c *>(symbol->ref_type_name);
  if (NULL == tmp_id) ERROR;
  if (datatypes_already_defined.find(tmp_id->value) != datatypes_already_defined.end())
    return NULL; // already defined. No need to define it again!!
  datatypes_already_defined[tmp_id->value] = 1; // insert this datatype into the list of already defined arrays!
  
  current_type_name = NULL;
  current_typedefinition = none_td;

  s4o_incl.print("__DECLARE_REFTO_TYPE(");
  symbol->ref_type_name->accept(*generate_c_typeid);
  s4o_incl.print(", ");
  symbol->ref_spec_init->accept(*this); // always calls ref_spec_init_c
  s4o_incl.print(")\n");

  current_type_name = NULL;
  current_typedefinition = none_td;

  return NULL;
}



/*********************/
/* B 1.4 - Variables */
/*********************/
  /* done in base class(es) */

/********************************************/
/* B.1.4.1   Directly Represented Variables */
/********************************************/
// direct_variable: direct_variable_token	{$$ = new direct_variable_c($1);};
void *visit(direct_variable_c *symbol) {
  TRACE("direct_variable_c");
  /* Do not use print_token() as it will change everything into uppercase */
  if (strlen(symbol->value) == 0) ERROR;
  return s4o_incl.printlocation(symbol->value + 1);
}


/*************************************/
/* B.1.4.2   Multi-element Variables */
/*************************************/
  /* done in base class(es) */

/******************************************/
/* B 1.4.3 - Declaration & Initialisation */
/******************************************/
  /* leave for derived classes... */

/**************************************/
/* B.1.5 - Program organization units */
/**************************************/
/***********************/
/* B 1.5.1 - Functions */
/***********************/
  /* leave for derived classes... */

/*****************************/
/* B 1.5.2 - Function Blocks */
/*****************************/
  /* leave for derived classes... */

/**********************/
/* B 1.5.3 - Programs */
/**********************/
  /* leave for derived classes... */

/*********************************************/
/* B.1.6  Sequential function chart elements */
/*********************************************/

/********************************/
/* B 1.7 Configuration elements */
/********************************/
  /* leave for derived classes... */

/****************************************/
/* B.2 - Language IL (Instruction List) */
/****************************************/
/***********************************/
/* B 2.1 Instructions and Operands */
/***********************************/
  /* leave for derived classes... */

/*******************/
/* B 2.2 Operators */
/*******************/
  /* leave for derived classes... */


/***************************************/
/* B.3 - Language ST (Structured Text) */
/***************************************/
/***********************/
/* B 3.1 - Expressions */
/***********************/
  /* leave for derived classes... */

/********************/
/* B 3.2 Statements */
/********************/
  /* leave for derived classes... */

/*********************************/
/* B 3.2.1 Assignment Statements */
/*********************************/
  /* leave for derived classes... */

/*****************************************/
/* B 3.2.2 Subprogram Control Statements */
/*****************************************/
  /* leave for derived classes... */

/********************************/
/* B 3.2.3 Selection Statements */
/********************************/
  /* leave for derived classes... */

/********************************/
/* B 3.2.4 Iteration Statements */
/********************************/
  /* leave for derived classes... */

}; /* generate_c_typedecl_c */



/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/


/* This class will generate a new datatype for each implicitly declared array datatype
 * (i.e. arrays declared in a variable declaration, or a struct datatype declaration...)
 * It will do the same for implicitly declared REF_TO datatypes.
 * 
 * Each new implicitly datatype will be atributed an alias, and a C datatype will be declared for that alias.
 * The alias itself will be stored (annotated) in the datatype object in the AST, using the annotation
 * map reserved for stage4 anotations. The alias is stored under the "generate_c_annotaton__implicit_type_id"
 * entry, and this entry will then be used whenever the name of the datatype is needed (to declare a varable,
 * for example). 
 * 
 * The class will be called once for each POU declaration, and once for each derived datatype declaration.
 * 
 * e.g.:
 *      VAR  x: ARRAY [1..3] OF INT; END_VAR   <---- ARRAY  datatype is implicitly declared inside the variable declaration
 *      VAR  y: REF_TO INT;          END_VAR   <---- REF_TO datatype is implicitly declared inside the variable declaration
 *      TYPE STRUCT
 *               a: ARRAY [1..3] OF INT;       <---- ARRAY  datatype is implicitly declared inside the struct type declaration  
 *               b: REF_TO INT;                <---- REF_TO datatype is implicitly declared inside the struct type declaration
 *               c: INT;
 *            END_STRUCT
 *      END_TYPE
 */
class generate_c_implicit_typedecl_c: public iterator_visitor_c {
  private:
    generate_c_typedecl_c *generate_c_typedecl_;
    generate_c_typedecl_c  generate_c_typedecl_local;
    symbol_c *prefix;
  public:
    generate_c_implicit_typedecl_c(stage4out_c *s4o, generate_c_typedecl_c *generate_c_typedecl=NULL) 
      : generate_c_typedecl_local(s4o) {
        generate_c_typedecl_ = generate_c_typedecl;
        if (NULL == generate_c_typedecl_) 
          generate_c_typedecl_ = &generate_c_typedecl_local;
        prefix = NULL;
    };
    virtual ~generate_c_implicit_typedecl_c(void) {
    }

    /*************************/
    /* B.1 - Common elements */
    /*************************/
    /**********************/
    /* B.1.3 - Data types */
    /**********************/
    /********************************/
    /* B 1.3.3 - Derived data types */
    /********************************/
    /*  identifier ':' array_spec_init */
    void *visit(array_type_declaration_c *symbol) {return NULL;} // This is not an implicitly defined array!

    /* ref_spec:  REF_TO (non_generic_type_name | function_block_type_name) */
    void *visit(ref_spec_c *symbol) {
      identifier_c *id = generate_datatypes_aliasid_c::create_id(symbol);
      /* Warning: The following is dangerous... 
       * We are asking the generate_c_typedecl_c visitor to visit a newly created ref_spec_init_c object
       * that has not been through stage 3, and therefore does not have stage 3 annotations filled in.
       * This will only work if generate_c_typedecl_c does ot depend on the stage 3 annotations!
       */
      ref_spec_init_c   ref_spec(symbol, NULL);
      ref_type_decl_c   ref_decl(id, &ref_spec);
      ref_decl.accept(*generate_c_typedecl_);
      symbol->anotations_map["generate_c_annotaton__implicit_type_id"] = id;
      return NULL;
    }

    /* For the moment, we do not support initialising reference data types */
    /* ref_spec_init: ref_spec [ ASSIGN ref_initialization ] */ 
    /* NOTE: ref_initialization may be NULL!! */
    // SYM_REF2(ref_spec_init_c, ref_spec, ref_initialization)
    void *visit(ref_spec_init_c *symbol) {
      symbol->ref_spec->accept(*this); //--> always calls ref_spec_c or derived_datatype_identifier_c
      int implicit_id_count = symbol->ref_spec->anotations_map.count("generate_c_annotaton__implicit_type_id");
      if (implicit_id_count  > 1) ERROR;
      if (implicit_id_count == 1)
        symbol->anotations_map["generate_c_annotaton__implicit_type_id"] = symbol->ref_spec->anotations_map["generate_c_annotaton__implicit_type_id"];
      return NULL;
    }

    /* ref_type_decl: identifier ':' ref_spec_init */
    void *visit(ref_type_decl_c *symbol) {return NULL;} // This is not an implicitly defined REF_TO!

    /******************************************/
    /* B 1.4.3 - Declaration & Initialization */
    /******************************************/
    void *visit(edge_declaration_c           *symbol) {return NULL;}
    void *visit(en_param_declaration_c       *symbol) {return NULL;}
    void *visit(eno_param_declaration_c      *symbol) {return NULL;}

    /* array_specification [ASSIGN array_initialization] */
    /* array_initialization may be NULL ! */
    void *visit(array_spec_init_c *symbol) {
      symbol->array_specification->accept(*this); //--> always calls array_specification_c or derived_datatype_identifier_c
      int implicit_id_count = symbol->array_specification->anotations_map.count("generate_c_annotaton__implicit_type_id");
      if (implicit_id_count  > 1) ERROR;
      if (implicit_id_count == 1)
        symbol->anotations_map["generate_c_annotaton__implicit_type_id"] = symbol->array_specification->anotations_map["generate_c_annotaton__implicit_type_id"];
      return NULL;
    }

    /* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
    void *visit(array_specification_c *symbol) {
      identifier_c *id = generate_datatypes_aliasid_c::create_id(symbol);
      /* Warning: The following is dangerous... 
       * We are asking the generate_c_typedecl_c visitor to visit a newly created array_type_declaration_c object
       * that has not been through stage 3, and therefore does not have stage 3 annotations filled in.
       * This will only work if generate_c_typedecl_c does ot depend on the stage 3 annotations!
       */
      array_spec_init_c        array_spec(symbol, NULL);
      array_type_declaration_c array_decl(id, &array_spec);
      array_decl.datatype = symbol->datatype;
      array_spec.datatype = symbol->datatype;
      array_decl.accept(*generate_c_typedecl_);
      symbol->anotations_map["generate_c_annotaton__implicit_type_id"] = id;
      return NULL;
    }
    
    /*  var1_list ':' initialized_structure */
    // SYM_REF2(structured_var_init_decl_c, var1_list, initialized_structure)
    void *visit(structured_var_init_decl_c   *symbol) {return NULL;}

    /* fb_name_list ':' function_block_type_name ASSIGN structure_initialization */
    /* structure_initialization -> may be NULL ! */
    void *visit(fb_name_decl_c               *symbol) {return NULL;}

    /*  var1_list ':' structure_type_name */
    //SYM_REF2(structured_var_declaration_c, var1_list, structure_type_name)
    void *visit(structured_var_declaration_c *symbol) {return NULL;}


    /***********************/
    /* B 1.5.1 - Functions */
    /***********************/      
    void *visit(function_declaration_c *symbol) {
      prefix = symbol->derived_function_name;
      symbol->var_declarations_list->accept(*this); //--> always calls var_declarations_list_c
      prefix = NULL;
      return NULL;
    }
    /*****************************/
    /* B 1.5.2 - Function Blocks */
    /*****************************/
    void *visit(function_block_declaration_c *symbol) {
      prefix = symbol->fblock_name;
      symbol->var_declarations->accept(*this); //--> always calls var_declarations_list_c
      prefix = NULL;
      return NULL;
    }
    /**********************/
    /* B 1.5.3 - Programs */
    /**********************/    
    void *visit(program_declaration_c *symbol) {
      prefix = symbol->program_type_name;
      symbol->var_declarations->accept(*this); //--> always calls var_declarations_list_c
      prefix = NULL;
      return NULL;
    }
};





