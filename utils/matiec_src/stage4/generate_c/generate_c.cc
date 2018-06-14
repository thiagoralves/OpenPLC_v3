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

#include <string>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <list>
#include <map>
#include <sstream>
#include <strings.h>


#include "../../util/symtable.hh"
#include "../../util/dsymtable.hh"
#include "../../absyntax/visitor.hh"
#include "../../absyntax_utils/absyntax_utils.hh"
#include "../../main.hh" // required for ERROR() and ERROR_MSG() macros.

#include "../stage4.hh"

//#define DEBUG
#ifdef DEBUG
#define TRACE(classname) printf("\n____%s____\n",classname);
#else
#define TRACE(classname)
#endif



#define STAGE4_ERROR(symbol1, symbol2, ...) {stage4err("while generating C code", symbol1, symbol2, __VA_ARGS__); exit(EXIT_FAILURE);}


/* Macros to access the constant value of each expression (if it exists) from the annotation introduced to the symbol_c object by constant_folding_c in stage3! */
#define VALID_CVALUE(dtype, symbol)           ((symbol)->const_value._##dtype.is_valid())
#define GET_CVALUE(dtype, symbol)             ((symbol)->const_value._##dtype.get()) 



/***********************************************************************/

/* Unlike Programs and Configurations which get mapped onto C++ classes,
 * Function Blocks are mapped onto a C structure containing the variables, and
 * a C function containing the code in the FB's body. This is to allow direct allocation
 * of a FB variable (which is really an instance of the C data structure) to
 * a member of a union variable (note that classes with constructors cannot
 * be mebers of a union), which is done in IL when loading a FB onto IL's
 * default variable.
 *
 * So as not to clash the names of the C data structure and the C function,
 * the C structure is given a name identical to that of the FB name, whereas
 * the name of the function is the FB name with a constant string appended.
 * The value of that constant string which is appended is defined in the following
 * constant.
 * In order not to clash with any variable in the IL and ST source codem the
 * following constant should contain a double underscore, which is not allowed
 * in IL and ST.
 *
 * e.g.:  FUNTION_BLOCK TEST
 * is mapped onto a TEST data structure, and a TEST_body__ function.
 */

#define FB_FUNCTION_SUFFIX "_body__"

/* Idem as body, but for initializer FB function */
#define FB_INIT_SUFFIX "_init__"

/* Idem as body, but for run CONFIG and RESOURCE function */
#define FB_RUN_SUFFIX "_run__"

/* The FB body function is passed as the only parameter a pointer to the FB data
 * structure instance. The name of this parameter is given by the following constant.
 * In order not to clash with any variable in the IL and ST source codem the
 * following constant should contain a double underscore, which is not allowed
 * in IL and ST.
 *
 * e.g.: the body of FUNTION_BLOCK TEST
 * is mapped onto the C function
 *  TEST_body__(TEST *data__)
 */

#define FB_FUNCTION_PARAM "data__"


#define SFC_STEP_ACTION_PREFIX "__SFC_"


/* Variable declaration symbol for accessor macros */
#define DECLARE_VAR "__DECLARE_VAR"
#define DECLARE_GLOBAL "__DECLARE_GLOBAL"
#define DECLARE_GLOBAL_FB "__DECLARE_GLOBAL_FB"
#define DECLARE_GLOBAL_LOCATION "__DECLARE_GLOBAL_LOCATION"
#define DECLARE_GLOBAL_LOCATED "__DECLARE_GLOBAL_LOCATED"
#define DECLARE_EXTERNAL "__DECLARE_EXTERNAL"
#define DECLARE_EXTERNAL_FB "__DECLARE_EXTERNAL_FB"
#define DECLARE_LOCATED "__DECLARE_LOCATED"
#define DECLARE_GLOBAL_PROTOTYPE "__DECLARE_GLOBAL_PROTOTYPE"

/* Variable declaration symbol for accessor macros */
#define INIT_VAR "__INIT_VAR"
#define INIT_GLOBAL "__INIT_GLOBAL"
#define INIT_GLOBAL_FB "__INIT_GLOBAL_FB"
#define INIT_GLOBAL_LOCATED "__INIT_GLOBAL_LOCATED"
#define INIT_EXTERNAL "__INIT_EXTERNAL"
#define INIT_EXTERNAL_FB "__INIT_EXTERNAL_FB"
#define INIT_LOCATED "__INIT_LOCATED"
#define INIT_LOCATED_VALUE "__INIT_LOCATED_VALUE"

/* Variable getter symbol for accessor macros */
#define GET_VAR "__GET_VAR"
#define GET_EXTERNAL "__GET_EXTERNAL"
#define GET_EXTERNAL_FB "__GET_EXTERNAL_FB"
#define GET_LOCATED "__GET_LOCATED"

#define GET_VAR_REF "__GET_VAR_REF"
#define GET_EXTERNAL_REF "__GET_EXTERNAL_REF"
#define GET_EXTERNAL_FB_REF "__GET_EXTERNAL_FB_REF"
#define GET_LOCATED_REF "__GET_LOCATED_REF"

#define GET_VAR_DREF "__GET_VAR_DREF"
#define GET_EXTERNAL_DREF "__GET_EXTERNAL_DREF"
#define GET_EXTERNAL_FB_DREF "__GET_EXTERNAL_FB_DREF"
#define GET_LOCATED_DREF "__GET_LOCATED_DREF"

#define GET_VAR_BY_REF "__GET_VAR_BY_REF"
#define GET_EXTERNAL_BY_REF "__GET_EXTERNAL_BY_REF"
#define GET_EXTERNAL_FB_BY_REF "__GET_EXTERNAL_FB_BY_REF"
#define GET_LOCATED_BY_REF "__GET_LOCATED_BY_REF"

/* Variable setter symbol for accessor macros */
#define SET_VAR "__SET_VAR"
#define SET_EXTERNAL "__SET_EXTERNAL"
#define SET_EXTERNAL_FB "__SET_EXTERNAL_FB"
#define SET_LOCATED "__SET_LOCATED"

/* Variable initial value symbol for accessor macros */
#define INITIAL_VALUE "__INITIAL_VALUE"

/* Generate a name for a temporary variable.
 * Each new name generated is appended a different number,
 * starting off from 0.
 * After calling reset(), the names will start off again from 0.
 */
#define VAR_LEADER "__"
#define TEMP_VAR VAR_LEADER "TMP_"
#define SOURCE_VAR VAR_LEADER "SRC_"

/* please see the comment before the RET_operator_c visitor for details... */
#define END_LABEL VAR_LEADER "end"


/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/


static int generate_line_directives__ = 0;
static int generate_pou_filepairs__   = 0;

#ifdef __unix__
/* Parse command line options passed from main.c !! */
#include <stdlib.h> // for getsybopt()
int  stage4_parse_options(char *options) {
  enum {                    LINE_OPT = 0            ,  SEPTFILE_OPT              /*, SOME_OTHER_OPT, YET_ANOTHER_OPT */};
  char *const token[] = { /*[LINE_OPT]=*/(char *)"l",/*SEPTFILE_OPT*/(char *)"p" /*, SOME_OTHER_OPT, ...             */, NULL };
  /* unfortunately, the above commented out syntax for array initialization is valid in C, but not in C++ */
  
  char *subopts = options;
  char *value;
  int opt;

  while (*subopts != '\0') {
    switch (getsubopt(&subopts, token, &value)) {
      case     LINE_OPT: generate_line_directives__  = 1; break;
      case SEPTFILE_OPT: generate_pou_filepairs__    = 1; break;
      default          : fprintf(stderr, "Unrecognized option: -O %s\n", value); return -1; break;
     }
  }     
  return 0;
}


void stage4_print_options(void) {
  printf("          (options must be separated by commas. Example: 'l,w,x')\n"); 
  printf("      l : insert '#line' directives in generated C code.\n"); 
  printf("      p : place each POU in a separate pair of files (<pou_name>.c, <pou_name>.h).\n"); 
}
#else /* not __unix__ */
/* getsubopt isn't supported with mingw, 
 *  then stage4 options aren't available on windows*/
void stage4_print_options(void) {}
int  stage4_parse_options(char *options) {}
#endif 

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

#include "generate_c_base.cc"
#include "generate_c_typedecl.cc"
#include "generate_c_sfcdecl.cc"
#include "generate_c_vardecl.cc"
#include "generate_c_configbody.cc"
#include "generate_location_list.cc"
#include "generate_var_list.cc"

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/


#include "generate_c.hh"


/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

/* A helper class that prints out the identifiers for function calls to overloaded functions */
/* Given a function declaration of the function being called, it 
 * will simply print out the returned data type,
 * followed by the data types of all input, output, and in_out parameters.
 *   for e.g.; 
 *     SIN( REAL) : REAL      -> prints out ->  REAL__REAL
 *     LEN( STRING) : INT     -> prints out ->  INT__STRING
 *     MUL(TIME, INT) : TIME  -> prints out ->  TIME__TIME__INT
 */
class print_function_parameter_data_types_c: public generate_c_base_and_typeid_c {
  private:
    symbol_c *current_type;
    bool_type_name_c tmp_bool;

    void print_list(symbol_c *var_list, symbol_c *data_type) { 
      if (data_type != NULL) {
        /* print out the data type once for every variable! */
        list_c *list = dynamic_cast<list_c *>(var_list);
        if (list == NULL) ERROR;  
        for (int i=0; i < list->n; i++) {
          s4o.print("__");
          data_type->accept(*this);
        }  
      }
    }
    
  public:
    print_function_parameter_data_types_c(stage4out_c *s4o_ptr): 
      generate_c_base_and_typeid_c(s4o_ptr)
      {current_type = NULL;}

    /**************************************/
    /* B.1.5 - Program organization units */
    /**************************************/
    /***********************/
    /* B 1.5.1 - Functions */
    /***********************/
    /*   FUNCTION derived_function_name ':' elementary_type_name io_OR_function_var_declarations_list function_body END_FUNCTION */
    /* | FUNCTION derived_function_name ':' derived_type_name io_OR_function_var_declarations_list function_body END_FUNCTION */
    void *visit(function_declaration_c *symbol) {
      symbol->type_name->accept(*this); /* return type */
      symbol->var_declarations_list->accept(*this);
      return NULL;
    }

    /* already handled by iterator base class (note that generate_c_base_c inherits from iterator_c) */
    //void *visit(var_declarations_list_c *symbol) {// iterate through list}
    
    /* already handled by iterator base class (note that generate_c_base_c inherits from iterator_c) */
    //void *visit(input_declarations_c *symbol) {// iterate through list}
        
    /* already handled by iterator base class (note that generate_c_base_c inherits from iterator_c) */
    //void *visit(input_declaration_list_c *symbol) {// iterate through list}

    void *visit(edge_declaration_c *symbol) {
      current_type = &tmp_bool; 
      symbol->var1_list->accept(*this);
      current_type = NULL; 
      return NULL;
    }
    
    /* We do NOT print out EN and ENO parameters! */
    void *visit(en_param_declaration_c *symbol) {return NULL;}

    /* already handled by iterator base class (note that generate_c_base_c inherits from iterator_c) */
    //void *visit(output_declarations_c *symbol) {// iterate through list}    
    
    /* already handled by iterator base class (note that generate_c_base_c inherits from iterator_c) */
    //void *visit(var_init_decl_list_c *symbol) {// iterate through list}

    void *visit(simple_spec_init_c *symbol) {
      /* return the data type */
      return symbol->simple_specification; 
    }

    /* currently we do not support data types defined in the declaration itself */
    /* For now, sugest the user define a TYPE .. END_TYPE */
    /* NOTE: although this class may also sometimes point to a previously_declared_subrange_type_name
     * we don't need this for now, so it is easier to just skip it allocation
     */
    void *visit(subrange_spec_init_c *symbol) {return NULL;}

    /* currently we do not support data types defined in the declaration itself */
    /* For now, sugest the user define a TYPE .. END_TYPE */
    /* NOTE: although this class may also sometimes point to a previously_declared_enumerated_type_name
     * we don't need this for now, so it is easier to just skip it allocation
     */
    void *visit(enumerated_spec_init_c *symbol) {return NULL;}

    /* currently we do not support data types defined in the declaration itself */
    /* For now, sugest the user define a TYPE .. END_TYPE */
    /* NOTE: although this class may also sometimes point to a previously_declared_array_type_name
     * we don't need this for now, so it is easier to just skip it allocation
     */
    void *visit(array_var_init_decl_c *symbol) {return NULL;}

    /* currently we do not support data types defined in the declaration itself */
    /* For now, sugest the user define a TYPE .. END_TYPE */
    /* NOTE: although this class may also sometimes point to a previously_declared_structured_type_name
     * we don't need this for now, so it is easier to just skip it allocation
     */
    void *visit(structured_var_init_decl_c *symbol) {return NULL;}

    /* We do NOT print out EN and ENO parameters! */
    void *visit(eno_param_declaration_c *symbol) {return NULL;}

    /* already handled by iterator base class (note that generate_c_base_c inherits from iterator_c) */
    //void *visit(input_output_declarations_c *symbol) {// iterate through list}    

    /* already handled by iterator base class (note that generate_c_base_c inherits from iterator_c) */
    //void *visit(var_declaration_list_c *symbol) {iterate through list}

    void *visit(fb_name_decl_c *symbol) {
      print_list(symbol->fb_name_list, spec_init_sperator_c::get_spec(symbol->fb_spec_init)); 
      return NULL;
    }

    void *visit(var1_init_decl_c *symbol) {
      print_list(symbol->var1_list, (symbol_c *)symbol->spec_init->accept(*this));
      return NULL;
    }

    /* currently we do not support data types defined in the declaration itself */
    /* For now, sugest the user define a TYPE .. END_TYPE */
    void *visit(array_var_declaration_c *symbol) {return NULL;}

    void *visit(structured_var_declaration_c *symbol) {
      current_type = symbol->structure_type_name; 
      symbol->var1_list->accept(*this);
      current_type = NULL; 
      return NULL;
    }

    /* currently we do not support data types defined in the declaration itself */
    /* For now, sugest the user define a TYPE .. END_TYPE */
    /* Note that this class is used for fixed length strings...
     *   STRING [ 42 ]
     */
    void *visit(single_byte_string_var_declaration_c *symbol) {return NULL;}

    /* currently we do not support data types defined in the declaration itself */
    /* For now, sugest the user define a TYPE .. END_TYPE */
    /* Note that this class is used for fixed length strings...
     *   WSTRING [ 42 ]
     */
    void *visit(double_byte_string_var_declaration_c *symbol) {return NULL;}
};
    

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

/* A helper class that analyses if the datatype of a variable is 'complex'. */
/* 'complex' means that it is either a strcuture or an array!               */
class analyse_variable_c: public search_visitor_c {
  private:
    static analyse_variable_c *singleton_;

  public:
    analyse_variable_c(void) {};
    
    static bool is_complex_type(symbol_c *symbol) {
      if (NULL == symbol) ERROR;
      if (!get_datatype_info_c::is_type_valid     (symbol->datatype)) return false;
      return (   get_datatype_info_c::is_structure(symbol->datatype) 
              || get_datatype_info_c::is_array    (symbol->datatype) 
             );
    }

    
  private:
    symbol_c *last_fb, *first_non_fb_identifier;

  public:  
    /* returns the first element (from left to right) in a structured variable that is not a FB, i.e. is either a structure or an array! */
    /* eg:
     *      fb1.fb2.fb3.real       returns ??????
     *      fb1.fb2.struct1.real   returns struct1
     *      struct1.real           returns struct1
     */
    static symbol_c *find_first_nonfb(symbol_c *symbol) {
      if (NULL == singleton_)       singleton_ = new analyse_variable_c();
      if (NULL == singleton_)       ERROR;
      if (NULL == symbol)           ERROR;
      
      singleton_->last_fb                 = NULL;
      singleton_->first_non_fb_identifier = NULL;
      return (symbol_c *)symbol->accept(*singleton_);
    }
    
    /* returns true if a strcutured variable (e.g. fb1.fb2.strcut1.real) contains a structure or array */
    /* eg:
     *      fb1.fb2.fb3.real       returns FALSE
     *      fb1.fb2.struct1.real   returns TRUE
     *      struct1.real           returns TRUE
     */
    static bool contains_complex_type(symbol_c *symbol) {
      if (NULL == symbol) ERROR;
      if (!get_datatype_info_c::is_type_valid(symbol->datatype)) ERROR;
      
      symbol_c *first_non_fb = (symbol_c *)find_first_nonfb(symbol);
      return is_complex_type(first_non_fb->datatype);
    }
    
    
    /* returns the datatype of the variable returned by find_first_nonfb() */
    /* eg:
     *      fb1.fb2.fb3.real       returns ??????
     *      fb1.fb2.struct1.real   returns datatype of struct1
     *      struct1.real           returns datatype of struct1
     */
    static search_var_instance_decl_c::vt_t first_nonfb_vardecltype(symbol_c *symbol, symbol_c *scope) {
      if (NULL == symbol) ERROR;
      if (!get_datatype_info_c::is_type_valid(symbol->datatype)) ERROR;
      
      symbol_c *first_non_fb = (symbol_c *)find_first_nonfb(symbol);
      if (NULL != singleton_->last_fb) {
        scope = singleton_->last_fb->datatype;
        symbol = singleton_->first_non_fb_identifier;
      }
      
      search_var_instance_decl_c search_var_instance_decl(scope);
      
      return search_var_instance_decl.get_vartype(symbol);
    }
    
    
    /*********************/
    /* B 1.4 - Variables */
    /*********************/
    void *visit(symbolic_variable_c *symbol) {
      if (!get_datatype_info_c::is_type_valid    (symbol->datatype)) ERROR;
      if (!get_datatype_info_c::is_function_block(symbol->datatype)) {
         first_non_fb_identifier = symbol; 
         return (void *)symbol;
      }
      last_fb = symbol;
      return NULL;
    }
    
    /*************************************/
    /* B.1.4.2   Multi-element Variables */
    /*************************************/
    
    // SYM_REF2(structured_variable_c, record_variable, field_selector)
    void *visit(structured_variable_c *symbol) {
      symbol_c *res = (symbol_c *)symbol->record_variable->accept(*this);
      if (NULL != res) return res;
      
      if (!get_datatype_info_c::is_type_valid    (symbol->datatype)) ERROR;
      if (!get_datatype_info_c::is_function_block(symbol->datatype)) {
         first_non_fb_identifier = symbol->field_selector; 
         return (void *)symbol;
      }

      last_fb = symbol;      
      return NULL;      
    }
    
    /*  subscripted_variable '[' subscript_list ']' */
    //SYM_REF2(array_variable_c, subscripted_variable, subscript_list)
    void *visit(array_variable_c *symbol) {
      void *res = symbol->subscripted_variable->accept(*this);
      if (NULL != res) return res;
      return (void *)symbol;      
    }

    
};

analyse_variable_c *analyse_variable_c::singleton_ = NULL;

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/


#include "generate_c_st.cc"
#include "generate_c_il.cc"
#include "generate_c_inlinefcall.cc"

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

#define MILLISECOND 1000000
#define SECOND 1000 * MILLISECOND

unsigned long long calculate_time(symbol_c *symbol) {
  if (NULL == symbol) return 0;
  
  interval_c *interval = dynamic_cast<interval_c *>(symbol);
  duration_c *duration = dynamic_cast<duration_c *>(symbol);
  
  if ((NULL == interval) && (NULL == duration))
  	  {STAGE4_ERROR(symbol, symbol, "This type of interval value is not currently supported"); ERROR;}

  if (NULL != duration) {
    /* SYM_REF2(duration_c, neg, interval) */
    if (duration->neg != NULL)
      {STAGE4_ERROR(duration, duration, "Negative TIME literals for interval are not currently supported"); ERROR;}
    return calculate_time(duration->interval);
  }

  if (NULL != interval) {
    /* SYM_REF5(interval_c, days, hours, minutes, seconds, milliseconds) */
      unsigned long long int time_ull = 0; 
      long double            time_ld  = 0;
      /*
      const unsigned long long int MILLISECOND = 1000000;
      const unsigned long long int      SECOND = 1000 * MILLISECOND
      */
      
      if (NULL != interval->milliseconds) {
        if      (VALID_CVALUE( int64, interval->milliseconds) &&           GET_CVALUE( int64, interval->milliseconds) < 0) ERROR; // interval elements should always be positive!
        if      (VALID_CVALUE( int64, interval->milliseconds)) time_ull += GET_CVALUE( int64, interval->milliseconds) * MILLISECOND;
        else if (VALID_CVALUE(uint64, interval->milliseconds)) time_ull += GET_CVALUE(uint64, interval->milliseconds) * MILLISECOND;
        else if (VALID_CVALUE(real64, interval->milliseconds)) time_ld  += GET_CVALUE(real64, interval->milliseconds) * MILLISECOND;
        else ERROR; // if (NULL != interval->milliseconds) is true, then it must have a valid constant value!
      }
   
      if (NULL != interval->seconds     ) {
        if      (VALID_CVALUE( int64, interval->seconds     ) &&           GET_CVALUE( int64, interval->seconds     ) < 0) ERROR; // interval elements should always be positive!
        if      (VALID_CVALUE( int64, interval->seconds     )) time_ull += GET_CVALUE( int64, interval->seconds     ) * SECOND;
        else if (VALID_CVALUE(uint64, interval->seconds     )) time_ull += GET_CVALUE(uint64, interval->seconds     ) * SECOND;
        else if (VALID_CVALUE(real64, interval->seconds     )) time_ld  += GET_CVALUE(real64, interval->seconds     ) * SECOND;
        else ERROR; // if (NULL != interval->seconds) is true, then it must have a valid constant value!
      }

      if (NULL != interval->minutes     ) {
        if      (VALID_CVALUE( int64, interval->minutes     ) &&           GET_CVALUE( int64, interval->minutes     ) < 0) ERROR; // interval elements should always be positive!
        if      (VALID_CVALUE( int64, interval->minutes     )) time_ull += GET_CVALUE( int64, interval->minutes     ) * SECOND * 60;
        else if (VALID_CVALUE(uint64, interval->minutes     )) time_ull += GET_CVALUE(uint64, interval->minutes     ) * SECOND * 60;
        else if (VALID_CVALUE(real64, interval->minutes     )) time_ld  += GET_CVALUE(real64, interval->minutes     ) * SECOND * 60;
        else ERROR; // if (NULL != interval->minutes) is true, then it must have a valid constant value!
      }

      if (NULL != interval->hours       ) {
        if      (VALID_CVALUE( int64, interval->hours       ) &&           GET_CVALUE( int64, interval->hours       ) < 0) ERROR; // interval elements should always be positive!
        if      (VALID_CVALUE( int64, interval->hours       )) time_ull += GET_CVALUE( int64, interval->hours       ) * SECOND * 60 * 60;
        else if (VALID_CVALUE(uint64, interval->hours       )) time_ull += GET_CVALUE(uint64, interval->hours       ) * SECOND * 60 * 60;
        else if (VALID_CVALUE(real64, interval->hours       )) time_ld  += GET_CVALUE(real64, interval->hours       ) * SECOND * 60 * 60;
        else ERROR; // if (NULL != interval->hours) is true, then it must have a valid constant value!
      }

      if (NULL != interval->days        ) {
        if      (VALID_CVALUE( int64, interval->days        ) &&           GET_CVALUE( int64, interval->days        ) < 0) ERROR; // interval elements should always be positive!
        if      (VALID_CVALUE( int64, interval->days        )) time_ull += GET_CVALUE( int64, interval->days        ) * SECOND * 60 * 60 * 24;
        else if (VALID_CVALUE(uint64, interval->days        )) time_ull += GET_CVALUE(uint64, interval->days        ) * SECOND * 60 * 60 * 24;
        else if (VALID_CVALUE(real64, interval->days        )) time_ld  += GET_CVALUE(real64, interval->days        ) * SECOND * 60 * 60 * 24;
        else ERROR; // if (NULL != interval->days) is true, then it must have a valid constant value!
      }

      time_ull += time_ld;
      return time_ull;
  };
  ERROR; // should never reach this point!
  return 0; // humour the compiler!
}

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

class calculate_common_ticktime_c: public iterator_visitor_c {
  private:
    unsigned long long common_ticktime;
    unsigned long long least_common_ticktime;
    
  public:
    calculate_common_ticktime_c(void){
      common_ticktime = 0;
      least_common_ticktime = 0;
    }
    
    unsigned long long euclide(unsigned long long a, unsigned long long b) {
      unsigned long long c = a % b;
      if (c == 0)
        return b;
      else
        return euclide(b, c);
    }
    
    void update_ticktime(unsigned long long time) {
      if (common_ticktime == 0)
        common_ticktime = time;
      else if (time > common_ticktime)
        common_ticktime = euclide(time, common_ticktime);
      else
        common_ticktime = euclide(common_ticktime, time);
      if (least_common_ticktime == 0)
        least_common_ticktime = time;
      else
        least_common_ticktime = (least_common_ticktime * time) / common_ticktime;
    }

    unsigned long long get_common_ticktime(void) {
      return common_ticktime;
    }

    unsigned long get_greatest_tick_count(void) {
      unsigned long long least_common_tick = least_common_ticktime / common_ticktime;
      if (least_common_tick >> 32)
        ERROR;
      return (unsigned long)(~(((unsigned long)-1) % (unsigned long)least_common_tick) + 1);
    }

/*  TASK task_name task_initialization */
//SYM_REF2(task_configuration_c, task_name, task_initialization)  
    void *visit(task_initialization_c *symbol) {
      if (symbol->interval_data_source != NULL) {
    	  unsigned long long time = calculate_time(symbol->interval_data_source);
    	  if (time < 0)  ERROR;
    	  else           update_ticktime(time);
      }
      return NULL;
    }
};    

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

/* A helper class that knows how to generate code for the SFC, IL and ST languages... */
class generate_c_SFC_IL_ST_c: public null_visitor_c {
  private:
    stage4out_c *s4o_ptr;
    symbol_c *scope;
    symbol_c *fbname;
    const char *variable_prefix;

  public:
    generate_c_SFC_IL_ST_c(stage4out_c *s4o_ptr, symbol_c *name, symbol_c *scope, const char *variable_prefix = NULL);
    
    /********************/
    /* 2.1.6 - Pragmas  */
    /********************/
    void *visit(enable_code_generation_pragma_c * symbol)   {s4o_ptr->enable_output();  return NULL;}
    void *visit(disable_code_generation_pragma_c * symbol)  {s4o_ptr->disable_output(); return NULL;} 
    
    /*********************************************/
    /* B.1.6  Sequential function chart elements */
    /*********************************************/
    /*| sequential_function_chart sfc_network*/
    void *visit(sequential_function_chart_c * symbol);
    
    /****************************************/
    /* B.2 - Language IL (Instruction List) */
    /****************************************/
    
    /***********************************/
    /* B 2.1 Instructions and Operands */
    /***********************************/
    /*| instruction_list il_instruction */
    void *visit(instruction_list_c *symbol);
    
    /* Remainder implemented in generate_c_il_c... */
    
    /***************************************/
    /* B.3 - Language ST (Structured Text) */
    /***************************************/
    /***********************/
    /* B 3.1 - Expressions */
    /***********************/
    /* Implemented in generate_c_st_c */
    
    /********************/
    /* B 3.2 Statements */
    /********************/
    void *visit(statement_list_c *symbol);

/* Remainder implemented in generate_c_st_c... */
};

#include "generate_c_sfc.cc"

generate_c_SFC_IL_ST_c::generate_c_SFC_IL_ST_c(stage4out_c *s4o_ptr, symbol_c *name, symbol_c *scope, const char *variable_prefix) {
  if (NULL == scope) ERROR;
  this->s4o_ptr = s4o_ptr;
  this->scope = scope;
  this->fbname = name;
  this->variable_prefix = variable_prefix;
}

void *generate_c_SFC_IL_ST_c::visit(sequential_function_chart_c * symbol) {
  generate_c_sfc_c generate_c_sfc(s4o_ptr, fbname, scope, variable_prefix);
  generate_c_sfc.generate(symbol);
  return NULL;
}

void *generate_c_SFC_IL_ST_c::visit(instruction_list_c *symbol) {
  generate_c_il_c generate_c_il(s4o_ptr, fbname, scope, variable_prefix);
  generate_c_il.generate(symbol);
  return NULL;
}

void *generate_c_SFC_IL_ST_c::visit(statement_list_c *symbol) {
  generate_c_st_c generate_c_st(s4o_ptr, fbname, scope, variable_prefix);
  generate_c_st.generate(symbol);
  return NULL;
}









/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

class generate_c_pous_c {  
  /* NOTE: This is NOT a visistor class!!
   * 
   *       Actually, it does not even really need to be a class. It is simply a collection of similar functions!!
   */ 

  private:
    static void print_end_of_block_label(stage4out_c &s4o) {
      /* Print and __end label for return statements!
       * If label is not used by at least one goto, compiler will generate a warning.
       * To work around this we introduce the useless goto.
       */
      s4o.print("\n");
      /* to humour the compiler, we insert a goto */
      s4o.print(s4o.indent_spaces);
      s4o.print("goto ");
      s4o.print(END_LABEL);
      s4o.print(";\n");
      s4o.indent_left();

      /* write the label marking the end of the code block */
      /* please see the comment before the RET_operator_c visitor for details... */
      /* also needed for return_statement_c */
      s4o.print("\n");
      s4o.print(s4o.indent_spaces);
      s4o.print(END_LABEL);
      s4o.print(":\n");
      s4o.indent_right();
    }
  

    /*************/
    /* Functions */
    /*************/
  public:
    /* NOTE: The following function will be called twice:
     *         1st time:  s4o will reference the .h file, and print_declaration=true
     *                     Here, we generate the function prototypes...
     *         2nd time:  s4o will reference the .c file, and print_declaration=false
     *                     Here we generate the source code!
     */
    /*   FUNCTION derived_function_name ':' elementary_type_name io_OR_function_var_declarations_list function_body END_FUNCTION */
    /* | FUNCTION derived_function_name ':' derived_type_name io_OR_function_var_declarations_list function_body END_FUNCTION */
    static void handle_function(function_declaration_c *symbol, stage4out_c &s4o, bool print_declaration) {
      generate_c_vardecl_c          *vardecl = NULL;
      generate_c_base_and_typeid_c   print_base(&s4o);
      
      TRACE("function_declaration_c");
    
      /* (A) Function declaration... */
      /* (A.1) Function return type */
      s4o.print("// FUNCTION\n");
      symbol->type_name->accept(print_base); /* return type */
      s4o.print(" ");
      /* (A.2) Function name */
      symbol->derived_function_name->accept(print_base);
      s4o.print("(");
    
      /* (A.3) Function parameters */
      s4o.indent_right();
      vardecl = new generate_c_vardecl_c(&s4o,
                                         generate_c_vardecl_c::finterface_vf,
                                         generate_c_vardecl_c::input_vt    |
                                         generate_c_vardecl_c::output_vt   |
                                         generate_c_vardecl_c::inoutput_vt |
                                         generate_c_vardecl_c::en_vt       |
                                         generate_c_vardecl_c::eno_vt);
      vardecl->print(symbol->var_declarations_list);
      delete vardecl;
      
      s4o.indent_left();
      
      s4o.print(")");
      
      /* If we only want the declaration/prototype, then return!! */
      if (print_declaration) 
        {s4o.print(";\n"); return;}
      
      /* continue generating the function definition/code... */
      s4o.print("\n" + s4o.indent_spaces + "{\n");
    
      /* (B) Function local variable declaration */
      /* (B.1) Variables declared in ST source code */
      s4o.indent_right();
      
      vardecl = new generate_c_vardecl_c(&s4o,
                    generate_c_vardecl_c::localinit_vf,
                    generate_c_vardecl_c::output_vt   |
                    generate_c_vardecl_c::inoutput_vt |
                    generate_c_vardecl_c::private_vt  |
                    generate_c_vardecl_c::eno_vt);
      vardecl->print(symbol->var_declarations_list);
      delete vardecl;
    
      /* (B.2) Temporary variable for function's return value */
      /* It will have the same name as the function itself! */
      s4o.print(s4o.indent_spaces);
      symbol->type_name->accept(print_base); /* return type */
      s4o.print(" ");
      symbol->derived_function_name->accept(print_base);
      s4o.print(" = ");
      {
        /* get the default value of this variable's type */
        symbol_c *default_value = type_initial_value_c::get(symbol->type_name);
        if (default_value == NULL) ERROR;
        initialization_analyzer_c initialization_analyzer(default_value);
        switch (initialization_analyzer.get_initialization_type()) {
          case initialization_analyzer_c::struct_it:
            {
              generate_c_structure_initialization_c *structure_initialization = new generate_c_structure_initialization_c(&s4o);
              structure_initialization->init_structure_default(symbol->type_name);
              structure_initialization->init_structure_values(default_value);
              delete structure_initialization;
            }
            break;
          case initialization_analyzer_c::array_it:
            {
              generate_c_array_initialization_c *array_initialization = new generate_c_array_initialization_c(&s4o);
              array_initialization->init_array_size(symbol->type_name);
              array_initialization->init_array_values(default_value);
              delete array_initialization;
            }
            break;
          default:
            default_value->accept(print_base);
            break;
        }
      }
      s4o.print(";\n\n");
      
      
      // Only generate the code that controls the execution of the function's body if the
      // function contains a declaration of both the EN and ENO variables
      search_var_instance_decl_c search_var(symbol);
      identifier_c  en_var("EN");
      identifier_c eno_var("ENO");
      if (   (search_var.get_vartype(& en_var) == search_var_instance_decl_c::input_vt)
          && (search_var.get_vartype(&eno_var) == search_var_instance_decl_c::output_vt)) {
        s4o.print(s4o.indent_spaces + "// Control execution\n");
        s4o.print(s4o.indent_spaces + "if (!EN) {\n");
        s4o.indent_right();
        s4o.print(s4o.indent_spaces + "if (__ENO != NULL) {\n");
        s4o.indent_right();
        s4o.print(s4o.indent_spaces + "*__ENO = __BOOL_LITERAL(FALSE);\n");
        s4o.indent_left();
        s4o.print(s4o.indent_spaces + "}\n");
        s4o.print(s4o.indent_spaces + "return ");
        symbol->derived_function_name->accept(print_base);
        s4o.print(";\n");
        s4o.indent_left();
        s4o.print(s4o.indent_spaces + "}\n");
      }
    
      /* (C) Function body */
      generate_c_SFC_IL_ST_c generate_c_code(&s4o, symbol->derived_function_name, symbol);
      symbol->function_body->accept(generate_c_code);
      
      print_end_of_block_label(s4o);
      
      vardecl = new generate_c_vardecl_c(&s4o,
                    generate_c_vardecl_c::foutputassign_vf,
                    generate_c_vardecl_c::output_vt   |
                    generate_c_vardecl_c::inoutput_vt |
                    generate_c_vardecl_c::eno_vt);
      vardecl->print(symbol->var_declarations_list);
      delete vardecl;
      
      s4o.print(s4o.indent_spaces + "return ");
      symbol->derived_function_name->accept(print_base);
      s4o.print(";\n");
      s4o.indent_left();
      s4o.print(s4o.indent_spaces + "}\n\n\n");
    
      return;
    }
    
    
    /*******************/
    /* Function Blocks */
    /*******************/
  public:
    /* NOTE: The following function will be called twice:
     *         1st time:  s4o will reference the .h file, and print_declaration=true
     *                     Here, we generate the function prototypes...
     *         2nd time:  s4o will reference the .c file, and print_declaration=false
     *                     Here we generate the source code!
     */
    /*  FUNCTION_BLOCK derived_function_block_name io_OR_other_var_declarations function_block_body END_FUNCTION_BLOCK */
    //SYM_REF4(function_block_declaration_c, fblock_name, var_declarations, fblock_body, unused)
    static void handle_function_block(function_block_declaration_c *symbol, stage4out_c &s4o, bool print_declaration) {
      generate_c_vardecl_c          *vardecl;
      generate_c_sfcdecl_c          *sfcdecl;
      generate_c_base_and_typeid_c   print_base(&s4o);
      TRACE("function_block_declaration_c");
    
      /* (A) Function Block data structure declaration... */
      if (print_declaration) {
        /* (A.1) Data structure declaration */
        s4o.print("// FUNCTION_BLOCK ");
        symbol->fblock_name->accept(print_base);
        s4o.print("\n// Data part\n");
        s4o.print("typedef struct {\n");
        s4o.indent_right();

        /* (A.2) Public variables: i.e. the function parameters... */
        s4o.print(s4o.indent_spaces + "// FB Interface - IN, OUT, IN_OUT variables\n");
        vardecl = new generate_c_vardecl_c(&s4o,
                                           generate_c_vardecl_c::local_vf,
                                           generate_c_vardecl_c::input_vt    |
                                           generate_c_vardecl_c::output_vt   |
                                           generate_c_vardecl_c::inoutput_vt |
                                           generate_c_vardecl_c::en_vt       |
                                           generate_c_vardecl_c::eno_vt);
        vardecl->print(symbol->var_declarations);
        delete vardecl;
        s4o.print("\n");

        /* (A.3) Private internal variables */
        s4o.print(s4o.indent_spaces + "// FB private variables - TEMP, private and located variables\n");
        vardecl = new generate_c_vardecl_c(&s4o,
                                           generate_c_vardecl_c::local_vf,
                                           generate_c_vardecl_c::temp_vt    |
                                           generate_c_vardecl_c::private_vt |
                                           generate_c_vardecl_c::located_vt |
                                           generate_c_vardecl_c::external_vt);
        vardecl->print(symbol->var_declarations);
        delete vardecl;
        
        /* (A.4) Generate private internal variables for SFC */
        sfcdecl = new generate_c_sfcdecl_c(&s4o, symbol);
        sfcdecl->generate(symbol->fblock_body, generate_c_sfcdecl_c::sfcdecl_sd);
        delete sfcdecl;
        s4o.print("\n");
      
        /* (A.5) Function Block data structure type name. */
        s4o.indent_left();
        s4o.print("} ");
        symbol->fblock_name->accept(print_base);
        s4o.print(";\n\n");
      }
      
      if (!print_declaration) {
        /* (A.6) Function Block inline function declaration for function invocation */
        generate_c_inlinefcall_c *inlinedecl = new generate_c_inlinefcall_c(&s4o, symbol->fblock_name, symbol, FB_FUNCTION_PARAM"->");
        symbol->fblock_body->accept(*inlinedecl);
        delete inlinedecl;
      }
      
      /* (B) Constructor */
      /* (B.1) Constructor name... */
      s4o.print(s4o.indent_spaces + "void ");
      symbol->fblock_name->accept(print_base);
      s4o.print(FB_INIT_SUFFIX);
      s4o.print("(");
    
      /* first and only parameter is a pointer to the data */
      symbol->fblock_name->accept(print_base);
      s4o.print(" *");
      s4o.print(FB_FUNCTION_PARAM);
      s4o.print(", BOOL retain)");

      if (print_declaration) {
        s4o.print(";\n");
      } else {
        s4o.print(" {\n");
        s4o.indent_right();
      
        /* (B.2) Member initializations... */
        s4o.print(s4o.indent_spaces);
        vardecl = new generate_c_vardecl_c(&s4o,
                                           generate_c_vardecl_c::constructorinit_vf,
                                           generate_c_vardecl_c::input_vt    |
                                           generate_c_vardecl_c::output_vt   |
                                           generate_c_vardecl_c::inoutput_vt |
                                           generate_c_vardecl_c::private_vt  |
                                           generate_c_vardecl_c::located_vt  |
                                           generate_c_vardecl_c::external_vt |
                                           generate_c_vardecl_c::en_vt       |
                                           generate_c_vardecl_c::eno_vt);
        vardecl->print(symbol->var_declarations, NULL, FB_FUNCTION_PARAM"->");
        delete vardecl;
        s4o.print("\n");
            
        /* (B.3) Generate private internal variables for SFC */
        sfcdecl = new generate_c_sfcdecl_c(&s4o, symbol, FB_FUNCTION_PARAM"->");
        sfcdecl->generate(symbol->fblock_body, generate_c_sfcdecl_c::sfcinit_sd);
      
        s4o.indent_left();
        s4o.print(s4o.indent_spaces + "}\n\n");

        /* (C) Function with FB body */
        /* (C.1) Step definitions */
        sfcdecl->generate(symbol->fblock_body, generate_c_sfcdecl_c::stepdef_sd);
      
        /* (C.2) Action definitions */
        sfcdecl->generate(symbol->fblock_body, generate_c_sfcdecl_c::actiondef_sd);
        delete sfcdecl;
      }
      
      /* (C.3) Function declaration */
      s4o.print("// Code part\n");
      /* function interface */
      s4o.print("void ");
      symbol->fblock_name->accept(print_base);
      s4o.print(FB_FUNCTION_SUFFIX);
      s4o.print("(");
      /* first and only parameter is a pointer to the data */
      symbol->fblock_name->accept(print_base);
      s4o.print(" *");
      s4o.print(FB_FUNCTION_PARAM);
      s4o.print(")");

      if (print_declaration) {
        s4o.print(";\n");
      } else {
        s4o.print(" {\n");
        s4o.indent_right();

        // Only generate the code that controls the execution of the function's body if the
        // function contains a declaration of both the EN and ENO variables
        search_var_instance_decl_c search_var(symbol);
        identifier_c  en_var("EN");
        identifier_c eno_var("ENO");
        if (   (search_var.get_vartype(& en_var) == search_var_instance_decl_c::input_vt)
            && (search_var.get_vartype(&eno_var) == search_var_instance_decl_c::output_vt)) {

          s4o.print(s4o.indent_spaces + "// Control execution\n");
          s4o.print(s4o.indent_spaces + "if (!");
          s4o.print(GET_VAR);
          s4o.print("(");
          s4o.print(FB_FUNCTION_PARAM);
          s4o.print("->EN)) {\n");
          s4o.indent_right();
          s4o.print(s4o.indent_spaces);
          s4o.print(SET_VAR);
          s4o.print("(");
          s4o.print(FB_FUNCTION_PARAM);
          s4o.print("->,ENO,,__BOOL_LITERAL(FALSE));\n");
          s4o.print(s4o.indent_spaces + "return;\n");
          s4o.indent_left();
          s4o.print(s4o.indent_spaces + "}\n");
          s4o.print(s4o.indent_spaces + "else {\n");
          s4o.indent_right();
          s4o.print(s4o.indent_spaces);
          s4o.print(SET_VAR);
          s4o.print("(");
          s4o.print(FB_FUNCTION_PARAM);
          s4o.print("->,ENO,,__BOOL_LITERAL(TRUE));\n");
          s4o.indent_left();
          s4o.print(s4o.indent_spaces + "}\n");
        }
      
        /* (C.4) Initialize TEMP variables */
        /* function body */
        s4o.print(s4o.indent_spaces + "// Initialise TEMP variables\n");
        vardecl = new generate_c_vardecl_c(&s4o,
                                           generate_c_vardecl_c::init_vf,
                                           generate_c_vardecl_c::temp_vt);
        vardecl->print(symbol->var_declarations, NULL,  FB_FUNCTION_PARAM"->");
        delete vardecl;
        s4o.print("\n");
      
        /* (C.5) Function code */
        generate_c_SFC_IL_ST_c generate_c_code(&s4o, symbol->fblock_name, symbol, FB_FUNCTION_PARAM"->");
        symbol->fblock_body->accept(generate_c_code);
        print_end_of_block_label(s4o);
        s4o.print(s4o.indent_spaces + "return;\n");
        s4o.indent_left();
        s4o.print(s4o.indent_spaces + "} // ");
        symbol->fblock_name->accept(print_base);
        s4o.print(FB_FUNCTION_SUFFIX);
        s4o.print(s4o.indent_spaces + "() \n\n");
      
        /* (C.6) Step undefinitions */
        sfcdecl = new generate_c_sfcdecl_c(&s4o, symbol, FB_FUNCTION_PARAM"->");
        sfcdecl->generate(symbol->fblock_body, generate_c_sfcdecl_c::stepundef_sd);
      
        /* (C.7) Action undefinitions */
        sfcdecl->generate(symbol->fblock_body, generate_c_sfcdecl_c::actionundef_sd);
        delete sfcdecl;
      
        s4o.indent_left();
        s4o.print("\n\n\n\n");
      }
      return;
    }
    
    
    /************/
    /* Programs */
    /************/
  public:
    /* NOTE: The following function will be called twice:
     *         1st time:  s4o will reference the .h file, and print_declaration=true
     *                     Here, we generate the function prototypes...
     *         2nd time:  s4o will reference the .c file, and print_declaration=false
     *                     Here we generate the source code!
     */
    /*  PROGRAM program_type_name program_var_declarations_list function_block_body END_PROGRAM */
    //SYM_REF4(program_declaration_c, program_type_name, var_declarations, function_block_body, unused)
    static void handle_program(program_declaration_c *symbol, stage4out_c &s4o, bool print_declaration) {
      generate_c_vardecl_c          *vardecl;
      generate_c_sfcdecl_c          *sfcdecl;
      generate_c_base_and_typeid_c   print_base(&s4o);
      TRACE("program_declaration_c");
    
      /* (A) Program data structure declaration... */
      if (print_declaration) {      
        /* (A.1) Data structure declaration */
        s4o.print("// PROGRAM ");
        symbol->program_type_name->accept(print_base);
        s4o.print("\n// Data part\n");
        s4o.print("typedef struct {\n");
        s4o.indent_right();
      
        /* (A.2) Public variables: i.e. the program parameters... */
        s4o.print(s4o.indent_spaces + "// PROGRAM Interface - IN, OUT, IN_OUT variables\n");
        vardecl = new generate_c_vardecl_c(&s4o,
                                           generate_c_vardecl_c::local_vf,
                                           generate_c_vardecl_c::input_vt  |
                                           generate_c_vardecl_c::output_vt |
                                           generate_c_vardecl_c::inoutput_vt);
        vardecl->print(symbol->var_declarations);
        delete vardecl;
        s4o.print("\n");
  
        /* (A.3) Private internal variables */
        s4o.print(s4o.indent_spaces + "// PROGRAM private variables - TEMP, private and located variables\n");
        vardecl = new generate_c_vardecl_c(&s4o,
                      generate_c_vardecl_c::local_vf,
                      generate_c_vardecl_c::temp_vt    |
                      generate_c_vardecl_c::private_vt |
                      generate_c_vardecl_c::located_vt |
                      generate_c_vardecl_c::external_vt);
        vardecl->print(symbol->var_declarations);
        delete vardecl;
      
        /* (A.4) Generate private internal variables for SFC */
        sfcdecl = new generate_c_sfcdecl_c(&s4o, symbol);
        sfcdecl->generate(symbol->function_block_body, generate_c_sfcdecl_c::sfcdecl_sd);
        delete sfcdecl;
        s4o.print("\n");
        
        /* (A.5) Program data structure type name. */
        s4o.indent_left();
        s4o.print("} ");
        symbol->program_type_name->accept(print_base);
        s4o.print(";\n\n");
      }
      
      if (!print_declaration) {      
        /* (A.6) Function Block inline function declaration for function invocation */
        generate_c_inlinefcall_c *inlinedecl = new generate_c_inlinefcall_c(&s4o, symbol->program_type_name, symbol, FB_FUNCTION_PARAM"->");
        symbol->function_block_body->accept(*inlinedecl);
        delete inlinedecl;
      }
    
      /* (B) Constructor */
      /* (B.1) Constructor name... */
      s4o.print(s4o.indent_spaces + "void ");
      symbol->program_type_name->accept(print_base);
      s4o.print(FB_INIT_SUFFIX);
      s4o.print("(");
    
      /* first and only parameter is a pointer to the data */
      symbol->program_type_name->accept(print_base);
      s4o.print(" *");
      s4o.print(FB_FUNCTION_PARAM);
      s4o.print(", BOOL retain)");

      if (print_declaration) {
        s4o.print(";\n");
      } else {
        s4o.print(" {\n");
        s4o.indent_right();
      
        /* (B.2) Member initializations... */
        s4o.print(s4o.indent_spaces);
        vardecl = new generate_c_vardecl_c(&s4o,
                                           generate_c_vardecl_c::constructorinit_vf,
                                           generate_c_vardecl_c::input_vt    |
                                           generate_c_vardecl_c::output_vt   |
                                           generate_c_vardecl_c::inoutput_vt |
                                           generate_c_vardecl_c::private_vt  |
                                           generate_c_vardecl_c::located_vt  |
                                           generate_c_vardecl_c::external_vt);
        vardecl->print(symbol->var_declarations, NULL,  FB_FUNCTION_PARAM"->");
        delete vardecl;
        s4o.print("\n");
      
        /* (B.3) Generate private internal variables for SFC */
        sfcdecl = new generate_c_sfcdecl_c(&s4o, symbol, FB_FUNCTION_PARAM"->");
        sfcdecl->generate(symbol->function_block_body, generate_c_sfcdecl_c::sfcinit_sd);
        delete sfcdecl;
      
        s4o.indent_left();
        s4o.print(s4o.indent_spaces + "}\n\n");
      }
    
      if (!print_declaration) {    
        /* (C) Function with PROGRAM body */
        /* (C.1) Step definitions */
        sfcdecl = new generate_c_sfcdecl_c(&s4o, symbol, FB_FUNCTION_PARAM"->");
        sfcdecl->generate(symbol->function_block_body, generate_c_sfcdecl_c::stepdef_sd);
      
        /* (C.2) Action definitions */
        sfcdecl->generate(symbol->function_block_body, generate_c_sfcdecl_c::actiondef_sd);
        delete sfcdecl;
      }
      
      /* (C.3) Function declaration */
      s4o.print("// Code part\n");
      /* function interface */
      s4o.print("void ");
      symbol->program_type_name->accept(print_base);
      s4o.print(FB_FUNCTION_SUFFIX);
      s4o.print("(");
      /* first and only parameter is a pointer to the data */
      symbol->program_type_name->accept(print_base);
      s4o.print(" *");
      s4o.print(FB_FUNCTION_PARAM);
      s4o.print(")");

      if (print_declaration) {
        s4o.print(";\n");
      } else {
        s4o.print(" {\n");
        s4o.indent_right();
          
        /* (C.4) Initialize TEMP variables */
        /* function body */
        s4o.print(s4o.indent_spaces + "// Initialise TEMP variables\n");
        vardecl = new generate_c_vardecl_c(&s4o,
                                           generate_c_vardecl_c::init_vf,
                                           generate_c_vardecl_c::temp_vt);
        vardecl->print(symbol->var_declarations, NULL,  FB_FUNCTION_PARAM"->");
        delete vardecl;
        s4o.print("\n");
      
        /* (C.5) Function code */
        generate_c_SFC_IL_ST_c generate_c_code(&s4o, symbol->program_type_name, symbol, FB_FUNCTION_PARAM"->");
        symbol->function_block_body->accept(generate_c_code);
        print_end_of_block_label(s4o);
        s4o.print(s4o.indent_spaces + "return;\n");
        s4o.indent_left();
        s4o.print(s4o.indent_spaces + "} // ");
        symbol->program_type_name->accept(print_base);
        s4o.print(FB_FUNCTION_SUFFIX);
        s4o.print(s4o.indent_spaces + "() \n\n");
      
        /* (C.6) Step undefinitions */
        sfcdecl = new generate_c_sfcdecl_c(&s4o, symbol, FB_FUNCTION_PARAM"->");
        sfcdecl->generate(symbol->function_block_body, generate_c_sfcdecl_c::stepundef_sd);
        
        /* (C.7) Action undefinitions */
        sfcdecl->generate(symbol->function_block_body, generate_c_sfcdecl_c::actionundef_sd); 
        delete sfcdecl;
      
        s4o.indent_left();
        s4o.print("\n\n\n\n");
      }  
      return;
    }
}; /* generate_c_pous_c */

    
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

class generate_c_config_c: public generate_c_base_and_typeid_c {
    private:
    stage4out_c &s4o_incl;
    
    public:
    generate_c_config_c(stage4out_c *s4o_ptr, stage4out_c *s4o_incl_ptr)
      : generate_c_base_and_typeid_c(s4o_ptr), s4o_incl(*s4o_incl_ptr) {
    };

    virtual ~generate_c_config_c(void) {}

    typedef enum {
      initprotos_dt,
      initdeclare_dt,
      runprotos_dt,
      rundeclare_dt
    } declaretype_t;

    declaretype_t wanted_declaretype;

    
public:
/********************/
/* 2.1.6 - Pragmas  */
/********************/
void *visit(enable_code_generation_pragma_c * symbol)   {
    s4o.enable_output();
    s4o_incl.enable_output();
    return NULL;
}

void *visit(disable_code_generation_pragma_c * symbol)  {
    s4o.disable_output();
    s4o_incl.disable_output();    
    return NULL;
}
    
    
/********************************/
/* B 1.7 Configuration elements */
/********************************/
/*
CONFIGURATION configuration_name
   optional_global_var_declarations
   (resource_declaration_list | single_resource_declaration)
   optional_access_declarations
   optional_instance_specific_initializations
END_CONFIGURATION
*/
/*
SYM_REF6(configuration_declaration_c, configuration_name, global_var_declarations, resource_declarations, access_declarations, instance_specific_initializations, unused)
*/
void *visit(configuration_declaration_c *symbol) {
  generate_c_vardecl_c *vardecl;
  
  /* Insert the header... */
  s4o.print("/*******************************************/\n");
  s4o.print("/*     FILE GENERATED BY iec2c             */\n");
  s4o.print("/* Editing this file is not recommended... */\n");
  s4o.print("/*******************************************/\n\n");
  
  if (runtime_options.disable_implicit_en_eno) {
    // If we are not generating the EN and ENO parameters for functions and FB,
    //   then make sure we use the standard library version compiled without these parameters too!
    s4o.print("#ifndef DISABLE_EN_ENO_PARAMETERS\n");
    s4o.print("#define DISABLE_EN_ENO_PARAMETERS\n");
    s4o.print("#endif\n");
  }
  
  s4o.print("#include \"iec_std_lib.h\"\n\n");
  s4o.print("#include \"accessor.h\"\n\n"); 
  s4o.print("#include \"POUS.h\"\n\n");

  /* (A) configuration declaration... */
  /* (A.1) configuration name in comment */
  s4o.print("// CONFIGURATION ");
  symbol->configuration_name->accept(*this);
  s4o.print("\n");
  
  /* (A.2) Global variables */
  vardecl = new generate_c_vardecl_c(&s4o,
                                     generate_c_vardecl_c::local_vf,
                                     generate_c_vardecl_c::global_vt,
                                     symbol->configuration_name);
  vardecl->print(symbol);
  delete vardecl;
  s4o.print("\n");

  /* (A.3) Declare global prototypes in include file */
  vardecl = new generate_c_vardecl_c(&s4o_incl,
                                     generate_c_vardecl_c::globalprototype_vf,
                                     generate_c_vardecl_c::global_vt,
                                     symbol->configuration_name);
  vardecl->print(symbol);
  delete vardecl;
  s4o_incl.print("\n");

  /* (B) Initialisation Function */
  /* (B.1) Ressources initialisation protos... */
  wanted_declaretype = initprotos_dt;
  symbol->resource_declarations->accept(*this);
  s4o.print("\n");
  
  /* (B.2) Initialisation function name... */
  s4o.print(s4o.indent_spaces + "void config");
  s4o.print(FB_INIT_SUFFIX);
  s4o.print("(void) {\n");
  s4o.indent_right();
  s4o.print(s4o.indent_spaces);
  s4o.print("BOOL retain;\n");
  s4o.print(s4o.indent_spaces);
  s4o.print("retain = 0;\n");
  
  /* (B.3) Global variables initializations... */
  s4o.print(s4o.indent_spaces);
  vardecl = new generate_c_vardecl_c(&s4o,
                                     generate_c_vardecl_c::constructorinit_vf,
                                     generate_c_vardecl_c::global_vt,
                                     symbol->configuration_name);
  vardecl->print(symbol);
  delete vardecl;
  s4o.print("\n");
  
  /* (B.3) Resources initializations... */
  wanted_declaretype = initdeclare_dt;
  symbol->resource_declarations->accept(*this);
  
  s4o.indent_left();
  s4o.print(s4o.indent_spaces + "}\n\n");


  /* (C) Run Function*/
  /* (C.1) Resources run functions protos... */
  wanted_declaretype = runprotos_dt;
  symbol->resource_declarations->accept(*this);
  s4o.print("\n");

  /* (C.2) Run function name... */
  s4o.print(s4o.indent_spaces + "void config");
  s4o.print(FB_RUN_SUFFIX);
  s4o.print("(unsigned long tick) {\n");
  s4o.indent_right();

  /* (C.3) Resources initializations... */
  wanted_declaretype = rundeclare_dt;
  symbol->resource_declarations->accept(*this);

  /* (C.3) Close Public Function body */
  s4o.indent_left();
  s4o.print(s4o.indent_spaces + "}\n");

  return NULL;
}

void *visit(resource_declaration_c *symbol) {
  if (wanted_declaretype == initprotos_dt || wanted_declaretype == runprotos_dt) {
    s4o.print(s4o.indent_spaces + "void ");
    symbol->resource_name->accept(*this);
    if (wanted_declaretype == initprotos_dt) {
      s4o.print(FB_INIT_SUFFIX);
      s4o.print("(void);\n");
    }
    else {
      s4o.print(FB_RUN_SUFFIX);
      s4o.print("(unsigned long tick);\n");
    }
  }
  if (wanted_declaretype == initdeclare_dt || wanted_declaretype == rundeclare_dt) {
    s4o.print(s4o.indent_spaces);
    symbol->resource_name->accept(*this);
    if (wanted_declaretype == initdeclare_dt) {
      s4o.print(FB_INIT_SUFFIX);
      s4o.print("();\n");
    }
    else {
      s4o.print(FB_RUN_SUFFIX);
      s4o.print("(tick);\n");
    }
  }
  return NULL;
}

void *visit(single_resource_declaration_c *symbol) {
  if (wanted_declaretype == initprotos_dt || wanted_declaretype == runprotos_dt) {
    s4o.print(s4o.indent_spaces + "void RESOURCE");
    if (wanted_declaretype == initprotos_dt) {
      s4o.print(FB_INIT_SUFFIX);
      s4o.print("(void);\n");
    }
    else {
      s4o.print(FB_RUN_SUFFIX);
      s4o.print("(unsigned long tick);\n");
    }
  }
  if (wanted_declaretype == initdeclare_dt || wanted_declaretype == rundeclare_dt) {
    s4o.print(s4o.indent_spaces + "RESOURCE");
    if (wanted_declaretype == initdeclare_dt) {
      s4o.print(FB_INIT_SUFFIX);
      s4o.print("();\n");
    }
    else {
      s4o.print(FB_RUN_SUFFIX);
      s4o.print("(tick);\n");
    }
  }
  return NULL;
}

};

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/


class generate_c_resources_c: public generate_c_base_and_typeid_c {

  search_var_instance_decl_c *search_config_instance;
  search_var_instance_decl_c *search_resource_instance;

  private:
    /* The name of the resource curretnly being processed... */
    symbol_c *current_configuration;
    symbol_c *current_resource_name;
    symbol_c *current_task_name;
    symbol_c *current_global_vars;
    bool configuration_name;
    stage4out_c *s4o_ptr;

  public:
    generate_c_resources_c(stage4out_c *s4o_ptr, symbol_c *config_scope, symbol_c *resource_scope, unsigned long time)
      : generate_c_base_and_typeid_c(s4o_ptr) {
      current_configuration = config_scope;
      search_config_instance   = new search_var_instance_decl_c(config_scope);
      search_resource_instance = new search_var_instance_decl_c(resource_scope);
      common_ticktime = time;
      current_resource_name = NULL;
      current_task_name = NULL;
      current_global_vars = NULL;
      configuration_name = false;
      generate_c_resources_c::s4o_ptr = s4o_ptr;
    };

    virtual ~generate_c_resources_c(void) {
      delete search_config_instance;
      delete search_resource_instance;
    }

    typedef enum {
      declare_dt,
      init_dt,
      run_dt
    } declaretype_t;

    declaretype_t wanted_declaretype;

    unsigned long long common_ticktime;
    
    const char *current_program_name;

    typedef enum {
      assign_at,
      send_at
    } assigntype_t;

    assigntype_t wanted_assigntype;

    /* the qualifier of variables that need to be processed... */
    static const unsigned int none_vq        = 0x0000;
    static const unsigned int constant_vq    = 0x0001;  // CONSTANT
    static const unsigned int retain_vq      = 0x0002;  // RETAIN
    static const unsigned int non_retain_vq  = 0x0004;  // NON_RETAIN

    /* variable used to store the qualifier of program currently being processed... */
    unsigned int current_varqualifier;

    void *print_retain(void) {
      s4o.print(",");
      switch (current_varqualifier) {
        case retain_vq:
          s4o.print("1");
          break;
        case non_retain_vq:
          s4o.print("0");
          break;
        default:
          s4o.print("retain");
          break;
      }
      return NULL;
    }

    /*************************/
    /* B.1 - Common elements */
    /*************************/
    /*******************************************/
    /* B 1.1 - Letters, digits and identifiers */
    /*******************************************/
    void *visit(identifier_c *symbol) {
      if (configuration_name)  s4o.print(symbol->value);
      else                     generate_c_base_c::visit(symbol);
      return NULL;
    }

    /********************/
    /* 2.1.6 - Pragmas  */
    /********************/
    void *visit(enable_code_generation_pragma_c * symbol)   {s4o_ptr->enable_output();  return NULL;}
    void *visit(disable_code_generation_pragma_c * symbol)  {s4o_ptr->disable_output(); return NULL;} 
    

    /******************************************/
    /* B 1.4.3 - Declaration & Initialisation */
    /******************************************/

    void *visit(constant_option_c *symbol) {
      current_varqualifier = constant_vq;
      return NULL;
    }

    void *visit(retain_option_c *symbol) {
      current_varqualifier = retain_vq;
      return NULL;
    }

    void *visit(non_retain_option_c *symbol) {
      current_varqualifier = non_retain_vq;
      return NULL;
    }

    /********************************/
    /* B 1.7 Configuration elements */
    /********************************/

    void *visit(configuration_declaration_c *symbol) {
      return symbol->configuration_name->accept(*this);
    }

/*
RESOURCE resource_name ON resource_type_name
   optional_global_var_declarations
   single_resource_declaration
END_RESOURCE
*/
// SYM_REF4(resource_declaration_c, resource_name, resource_type_name, global_var_declarations, resource_declaration)
    void *visit(resource_declaration_c *symbol) {
      current_resource_name = symbol->resource_name;
      current_global_vars = symbol->global_var_declarations;
      
      symbol->resource_declaration->accept(*this);
      
      current_resource_name = NULL;
      current_global_vars = NULL;
      return NULL;
    }

/* task_configuration_list program_configuration_list */
// SYM_REF2(single_resource_declaration_c, task_configuration_list, program_configuration_list)
    void *visit(single_resource_declaration_c *symbol) {
      bool single_resource = current_resource_name == NULL;
      if (single_resource)
        current_resource_name = new identifier_c("RESOURCE");
      generate_c_vardecl_c *vardecl;
      
      /* Insert the header... */
      s4o.print("/*******************************************/\n");
      s4o.print("/*     FILE GENERATED BY iec2c             */\n");
      s4o.print("/* Editing this file is not recommended... */\n");
      s4o.print("/*******************************************/\n\n");
  
      if (runtime_options.disable_implicit_en_eno) {
        // If we are not generating the EN and ENO parameters for functions and FB,
        //   then make sure we use the standard library version compiled without these parameters too!
        s4o.print("#ifndef DISABLE_EN_ENO_PARAMETERS\n");
        s4o.print("#define DISABLE_EN_ENO_PARAMETERS\n");
        s4o.print("#endif\n");
      }
      
      s4o.print("#include \"iec_std_lib.h\"\n\n");
      
      /* (A) resource declaration... */
      /* (A.1) resource name in comment */
      s4o.print("// RESOURCE ");
      current_resource_name->accept(*this);
      s4o.print("\n\n");
      
      s4o.print("extern unsigned long long common_ticktime__;\n\n");

      s4o.print("#include \"accessor.h\"\n");
      s4o.print("#include \"POUS.h\"\n\n");
      s4o.print("#include \"");
      configuration_name = true;
      current_configuration->accept(*this);
      configuration_name = false;
      s4o.print(".h\"\n");

      /* (A.2) Global variables... */
      if (current_global_vars != NULL) {
        vardecl = new generate_c_vardecl_c(&s4o,
                                           generate_c_vardecl_c::local_vf,
                                           generate_c_vardecl_c::global_vt,
                                           current_resource_name);
        vardecl->print(current_global_vars);
        delete vardecl;
        s4o.print("\n");
      }
      
      /* (A.3) POUs inclusion */
      s4o.print("#include \"POUS.c\"\n\n");
      
      wanted_declaretype = declare_dt;
      
      /* (A.4) Resource programs declaration... */
      symbol->task_configuration_list->accept(*this);
      
      /* (A.5) Resource programs declaration... */
      symbol->program_configuration_list->accept(*this);
      
      s4o.print("\n");
      
      /* (B) resource initialisation function... */
      /* (B.1) initialisation function name... */
      s4o.print("void ");
      current_resource_name->accept(*this);
      s4o.print(FB_INIT_SUFFIX);
      s4o.print("(void) {\n");
      s4o.indent_right();
      s4o.print(s4o.indent_spaces);
      s4o.print("BOOL retain;\n");
      s4o.print(s4o.indent_spaces);
      s4o.print("retain = 0;\n");
      
      /* (B.2) Global variables initialisations... */
      if (current_global_vars != NULL) {
        s4o.print(s4o.indent_spaces);
        vardecl = new generate_c_vardecl_c(&s4o,
                                           generate_c_vardecl_c::constructorinit_vf,
                                           generate_c_vardecl_c::global_vt,
                                           current_resource_name);
        vardecl->print(current_global_vars);
        delete vardecl;
      }
      s4o.print("\n");
      
      wanted_declaretype = init_dt;
      
      /* (B.3) Tasks initialisations... */
      symbol->task_configuration_list->accept(*this);
      
      /* (B.4) Resource programs initialisations... */
      symbol->program_configuration_list->accept(*this);
      
      s4o.indent_left();
      s4o.print("}\n\n");
      
      /* (C) Resource run function... */
      /* (C.1) Run function name... */
      s4o.print("void ");
      current_resource_name->accept(*this);
      s4o.print(FB_RUN_SUFFIX);
      s4o.print("(unsigned long tick) {\n");
      s4o.indent_right();
      
      wanted_declaretype = run_dt;
      
      /* (C.2) Task management... */
      symbol->task_configuration_list->accept(*this);
      
      /* (C.3) Program run declaration... */
      symbol->program_configuration_list->accept(*this);
      
      s4o.indent_left();
      s4o.print("}\n\n");
      
      if (single_resource) {
        delete current_resource_name;
        current_resource_name = NULL;
      }
      return NULL;
    }
    
/*  PROGRAM [RETAIN | NON_RETAIN] program_name [WITH task_name] ':' program_type_name ['(' prog_conf_elements ')'] */
//SYM_REF6(program_configuration_c, retain_option, program_name, task_name, program_type_name, prog_conf_elements, unused)
    void *visit(program_configuration_c *symbol) {
      switch (wanted_declaretype) {
        case declare_dt:
          s4o.print(s4o.indent_spaces);
          symbol->program_type_name->accept(*this);
          s4o.print(" ");
          current_resource_name->accept(*this);
          s4o.print("__");
          symbol->program_name->accept(*this);
          s4o.print(";\n#define ");
          symbol->program_name->accept(*this);
          s4o.print(" ");
          current_resource_name->accept(*this);
          s4o.print("__");
          symbol->program_name->accept(*this);
          s4o.print("\n");
          break;
        case init_dt:
          if (symbol->retain_option != NULL)
            symbol->retain_option->accept(*this);
          s4o.print(s4o.indent_spaces);
          symbol->program_type_name->accept(*this);
          s4o.print(FB_INIT_SUFFIX);
          s4o.print("(&");
          symbol->program_name->accept(*this);
          print_retain();
          s4o.print(");\n");
          break;
        case run_dt: 
          { identifier_c *tmp_id = dynamic_cast<identifier_c*>(symbol->program_name);
            if (NULL == tmp_id) ERROR;
            current_program_name = tmp_id->value;
	  }
          if (symbol->task_name != NULL) {
            s4o.print(s4o.indent_spaces);
            s4o.print("if (");
            symbol->task_name->accept(*this);
            s4o.print(") {\n");
            s4o.indent_right(); 
          }
        
          wanted_assigntype = assign_at;
          if (symbol->prog_conf_elements != NULL)
            symbol->prog_conf_elements->accept(*this);
          
          s4o.print(s4o.indent_spaces);
          symbol->program_type_name->accept(*this);
          s4o.print(FB_FUNCTION_SUFFIX);
          s4o.print("(&");
          symbol->program_name->accept(*this);
          s4o.print(");\n");
          
          wanted_assigntype = send_at;
          if (symbol->prog_conf_elements != NULL)
            symbol->prog_conf_elements->accept(*this);
          
          if (symbol->task_name != NULL) {
            s4o.indent_left();
            s4o.print(s4o.indent_spaces + "}\n");
          }
          break;
        default:
          break;
      }
      return NULL;
    }
    
/*  TASK task_name task_initialization */
//SYM_REF2(task_configuration_c, task_name, task_initialization)
    void *visit(task_configuration_c *symbol) {
      current_task_name = symbol->task_name;
      switch (wanted_declaretype) {
        case declare_dt:
          s4o.print(s4o.indent_spaces + "BOOL ");
          current_task_name->accept(*this);
          s4o.print(";\n");
          symbol->task_initialization->accept(*this);
          break;
        case init_dt:
          s4o.print(s4o.indent_spaces);
          current_task_name->accept(*this);
          s4o.print(" = __BOOL_LITERAL(FALSE);\n");
          symbol->task_initialization->accept(*this);
          break;
        case run_dt:
          symbol->task_initialization->accept(*this);
          break;
        default:
          break;
      }
      current_task_name = NULL;
      return NULL;
    }
    
/*  '(' [SINGLE ASSIGN data_source ','] [INTERVAL ASSIGN data_source ','] PRIORITY ASSIGN integer ')' */
//SYM_REF4(task_initialization_c, single_data_source, interval_data_source, priority_data_source, unused)
    void *visit(task_initialization_c *symbol) {
      switch (wanted_declaretype) {
        case declare_dt:
          if (symbol->single_data_source != NULL) {
            s4o.print(s4o.indent_spaces + "R_TRIG ");
            current_task_name->accept(*this);
            s4o.print("_R_TRIG;\n");
          }
          break;
        case init_dt:
          if (symbol->single_data_source != NULL) {
            s4o.print(s4o.indent_spaces + "R_TRIG");
            s4o.print(FB_INIT_SUFFIX);
            s4o.print("(&");
            current_task_name->accept(*this);
            s4o.print("_R_TRIG, retain);\n");
          }
          break;
        case run_dt:
          if (symbol->single_data_source != NULL) {
            symbol_c *config_var_decl = NULL;
            symbol_c *res_var_decl = NULL;
            s4o.print(s4o.indent_spaces + "{");
            symbol_c *current_var_reference = ((global_var_reference_c *)(symbol->single_data_source))->global_var_name;
            res_var_decl = search_resource_instance->get_decl(current_var_reference);
            if (res_var_decl == NULL) {
              config_var_decl = search_config_instance->get_decl(current_var_reference);
              if (config_var_decl == NULL)
                ERROR;
              config_var_decl->accept(*this);
            }
            else {
              res_var_decl->accept(*this);
            }
            s4o.print("* ");
            symbol->single_data_source->accept(*this);
            s4o.print(" = __GET_GLOBAL_");
            symbol->single_data_source->accept(*this);
            s4o.print("();");
            s4o.print(SET_VAR);
            s4o.print("(");
            current_task_name->accept(*this);
            s4o.print("_R_TRIG.,CLK,, *");
            symbol->single_data_source->accept(*this);
            s4o.print(");}\n");
            s4o.print(s4o.indent_spaces + "R_TRIG");
            s4o.print(FB_FUNCTION_SUFFIX);
            s4o.print("(&");
            current_task_name->accept(*this);
            s4o.print("_R_TRIG);\n");
            s4o.print(s4o.indent_spaces);
            current_task_name->accept(*this);
            s4o.print(" = ");
            s4o.print(GET_VAR);
            s4o.print("(");
            current_task_name->accept(*this);
            s4o.print("_R_TRIG.Q)");
          }
          else {
            s4o.print(s4o.indent_spaces);
            current_task_name->accept(*this);
            s4o.print(" = ");
            if (symbol->interval_data_source != NULL) {
              unsigned long long int time = calculate_time(symbol->interval_data_source);
              if (time != 0) {
                s4o.print("!(tick % ");
                s4o.print(time / common_ticktime);
                s4o.print(")");
              }
              else
                s4o.print("1");
            }
            else 
              s4o.print("1");
          }
          s4o.print(";\n");
          break;
        default:
          break;
      }
      return NULL;
    }

/*  any_symbolic_variable ASSIGN prog_data_source */
//SYM_REF2(prog_cnxn_assign_c, symbolic_variable, prog_data_source)
    void *visit(prog_cnxn_assign_c *symbol) {
      if (wanted_assigntype == assign_at) {
        symbol_c *var_decl;
        unsigned int vartype = 0;
        symbol_c *current_var_reference = ((global_var_reference_c *)(symbol->prog_data_source))->global_var_name;
        var_decl = search_resource_instance->get_decl(current_var_reference);
        if (var_decl == NULL) {
          var_decl = search_config_instance->get_decl(current_var_reference);
          if (var_decl == NULL)
            ERROR;
          else
            vartype = search_config_instance->get_vartype(current_var_reference);
        }
        else
          vartype = search_resource_instance->get_vartype(current_var_reference);
        
        s4o.print(s4o.indent_spaces + "{extern ");
        var_decl->accept(*this);
        s4o.print(" *");
        symbol->prog_data_source->accept(*this);
        s4o.print("; ");
        s4o.printupper(current_program_name);
        s4o.print(".");
        symbol->symbolic_variable->accept(*this);
        s4o.print(" = ");
        if (vartype == search_var_instance_decl_c::global_vt)
          s4o.print("*");
        symbol->prog_data_source->accept(*this);
        s4o.print(";}\n");
      }
      return NULL;
    }

/* any_symbolic_variable SENDTO data_sink */
//SYM_REF2(prog_cnxn_sendto_c, symbolic_variable, data_sink)
    void *visit(prog_cnxn_sendto_c *symbol) {
      if (wanted_assigntype == send_at) {
        symbol_c *var_decl;
        unsigned int vartype = 0;
        symbol_c *current_var_reference = ((global_var_reference_c *)(symbol->data_sink))->global_var_name;
        var_decl = search_resource_instance->get_decl(current_var_reference);
        if (var_decl == NULL) {
          var_decl = search_config_instance->get_decl(current_var_reference);
          if (var_decl == NULL)
            ERROR;
          else
            vartype = search_config_instance->get_vartype(current_var_reference);
        }
        else
          vartype = search_resource_instance->get_vartype(current_var_reference);
        
        s4o.print(s4o.indent_spaces + "{extern ");
        var_decl->accept(*this);
        s4o.print(" *");
        symbol->data_sink->accept(*this);
        s4o.print("; ");
        if (vartype == search_var_instance_decl_c::global_vt)
          s4o.print("*");
        symbol->data_sink->accept(*this);
        s4o.print(" = ");
        s4o.printupper(current_program_name);
        s4o.print(".");
        symbol->symbolic_variable->accept(*this);
        s4o.print(";};\n");
      }
      return NULL;
    }

};

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

class generate_c_c: public iterator_visitor_c {
  protected:
    stage4out_c                      &s4o;
    stage4out_c                  pous_s4o;
    stage4out_c             pous_incl_s4o;
    stage4out_c     located_variables_s4o;
    stage4out_c             variables_s4o;
    
    generate_c_typedecl_c          generate_c_typedecl;
    generate_c_implicit_typedecl_c generate_c_implicit_typedecl;
    generate_c_pous_c              generate_c_pous;
    
    symbol_c   *current_configuration;

    const char *current_name;
    const char *current_builddir;

    bool        allow_output;
    
    unsigned long long common_ticktime;

  public:
    generate_c_c(stage4out_c *s4o_ptr, const char *builddir): 
            s4o(*s4o_ptr),
            pous_s4o(builddir, "POUS", "c"),
            pous_incl_s4o(builddir, "POUS", "h"),
            located_variables_s4o(builddir, "LOCATED_VARIABLES","h"),
            variables_s4o(builddir, "VARIABLES","csv"),
            generate_c_typedecl         (&pous_incl_s4o),
            generate_c_implicit_typedecl(&pous_incl_s4o, &generate_c_typedecl)
    {
      current_builddir = builddir;
      current_configuration = NULL;
      allow_output = true;
    }
            
    ~generate_c_c(void) {}



/********************/
/* 2.1.6 - Pragmas  */
/********************/
    void *visit(enable_code_generation_pragma_c * symbol)  {
      s4o                  .enable_output();  
      pous_s4o             .enable_output();  
      pous_incl_s4o        .enable_output();  
      located_variables_s4o.enable_output();  
      variables_s4o        .enable_output();  
      allow_output = true;      
      return NULL;
    }
    
    void *visit(disable_code_generation_pragma_c * symbol)  {
      s4o                  .disable_output();  
      pous_s4o             .disable_output();  
      pous_incl_s4o        .disable_output();  
      located_variables_s4o.disable_output();  
      variables_s4o        .disable_output();  
      allow_output = false;      
      return NULL;
    } 


/***************************/
/* B 0 - Programming Model */
/***************************/
    void *visit(library_c *symbol) {
      pous_incl_s4o.print("#ifndef __POUS_H\n#define __POUS_H\n\n");
      
      if (runtime_options.disable_implicit_en_eno) {
        // If we are not generating the EN and ENO parameters for functions and FB,
        //   then make sure we use the standard library version compiled without these parameters too!
        pous_incl_s4o.print("#ifndef DISABLE_EN_ENO_PARAMETERS\n");
        pous_incl_s4o.print("#define DISABLE_EN_ENO_PARAMETERS\n");
        pous_incl_s4o.print("#endif\n");
      }
      
      pous_incl_s4o.print("#include \"accessor.h\"\n#include \"iec_std_lib.h\"\n\n");

      for(int i = 0; i < symbol->n; i++) {
        symbol->elements[i]->accept(*this);
      }

      pous_incl_s4o.print("#endif //__POUS_H\n");
      
      generate_var_list_c generate_var_list(&variables_s4o, symbol);
      generate_var_list.generate_programs(symbol);
      generate_var_list.generate_variables(symbol);
      variables_s4o.print("\n// Ticktime\n");
      variables_s4o.print_long_long_integer(common_ticktime, false);
      variables_s4o.print("\n");

      generate_location_list_c generate_location_list(&located_variables_s4o);
      symbol->accept(generate_location_list);
      return NULL;
    }

/*************************/
/* B.1 - Common elements */
/*************************/
/*******************************************/
/* B 1.1 - Letters, digits and identifiers */
/*******************************************/
    void *visit(identifier_c *symbol) {current_name = symbol->value; return NULL;}
    /* In the derived datatype and POUs declarations, the names are stored as identfier_c, so the following visitors are not required! */
    void *visit(derived_datatype_identifier_c *symbol) {ERROR; return NULL;}
    void *visit(         poutype_identifier_c *symbol) {ERROR; return NULL;}
    

/********************************/
/* B 1.3.3 - Derived data types */
/********************************/
    /*  TYPE type_declaration_list END_TYPE */
//   void *visit(data_type_declaration_c *symbol)  // handled by iterator_visitor_c

    /* helper symbol for data_type_declaration */
    void *visit(type_declaration_list_c *symbol) {
      for(int i = 0; i < symbol->n; i++) {
        symbol->elements[i]->accept(generate_c_implicit_typedecl);
        symbol->elements[i]->accept(generate_c_typedecl);
      }
      return NULL;
    }

/**************************************/
/* B.1.5 - Program organization units */
/**************************************/
/* WARNING: The following code is buggy when generating an independent pair of files for each POU, as the
 *          specially created stage4out_c (s4o_c and s4o_h) will not comply with the enable/disable_code_generation_pragma_c
 */
#define handle_pou(fname,pname) \
      if (!allow_output) return NULL;\
      if (generate_pou_filepairs__) {\
        const char *pou_name = get_datatype_info_c::get_id_str(pname);\
        stage4out_c s4o_c(current_builddir, pou_name, "c");\
        stage4out_c s4o_h(current_builddir, pou_name, "h");\
        s4o_c.print("#include \""); s4o_c.print(pou_name); s4o_c.print(".h\"\n");\
        s4o_h.print("#ifndef __");  s4o_h.print(pou_name); s4o_h.print("_H\n");\
        s4o_h.print("#define __");  s4o_h.print(pou_name); s4o_h.print("_H\n");\
        generate_c_implicit_typedecl_c generate_c_implicit_typedecl__(&s4o_h);\
        symbol->accept(generate_c_implicit_typedecl__); /* generate implicitly delcared datatypes (arrays and ref_to) */\
        generate_c_pous_c::fname(symbol, s4o_h, true); /* generate the <pou_name>.h file */\
        generate_c_pous_c::fname(symbol, s4o_c, false);/* generate the <pou_name>.c file */\
        s4o_h.print("#endif /* __");  s4o_h.print(pou_name); s4o_h.print("_H */\n");\
        /* add #include directives to the POUS.h and POUS.c files... */\
        pous_incl_s4o.print("#include \"");\
        pous_s4o.     print("#include \"");\
        pous_incl_s4o.print(pou_name);\
        pous_s4o.     print(pou_name);\
        pous_incl_s4o.print(".h\"\n");\
        pous_s4o.     print(".c\"\n");\
      } else {\
        symbol->accept(generate_c_implicit_typedecl);\
        generate_c_pous_c::fname(symbol, pous_incl_s4o, true);\
        generate_c_pous_c::fname(symbol, pous_s4o,      false);\
      }

/***********************/
/* B 1.5.1 - Functions */
/***********************/      
    void *visit(function_declaration_c *symbol) {
      handle_pou(handle_function,symbol->derived_function_name)
      return NULL;
    }
    
/*****************************/
/* B 1.5.2 - Function Blocks */
/*****************************/
    void *visit(function_block_declaration_c *symbol) {
      handle_pou(handle_function_block,symbol->fblock_name)
      return NULL;
    }
    
/**********************/
/* B 1.5.3 - Programs */
/**********************/    
    void *visit(program_declaration_c *symbol) {
      handle_pou(handle_program,symbol->program_type_name)
      return NULL;
    }
    

/********************************/
/* B 1.7 Configuration elements */
/********************************/
    void *visit(configuration_declaration_c *symbol) {
      if (symbol->global_var_declarations != NULL)
        symbol->global_var_declarations->accept(generate_c_implicit_typedecl);
      static int configuration_count = 0;

      if (configuration_count++) {
        /* the first configuration is the one we will use!! */
        STAGE4_ERROR(symbol, symbol, "A previous CONFIGURATION has already been declared (C code generation currently only allows a single configuration).");
        ERROR;
      }

      current_configuration = symbol;

      {
        calculate_common_ticktime_c calculate_common_ticktime;
        symbol->accept(calculate_common_ticktime);
        common_ticktime = calculate_common_ticktime.get_common_ticktime();
        if (common_ticktime == 0) {
          STAGE4_ERROR(symbol, symbol, "You must define at least one periodic task (to set cycle period)!");
          ERROR;
        }

        symbol->configuration_name->accept(*this);
        
        stage4out_c config_s4o(current_builddir, current_name, "c");
        stage4out_c config_incl_s4o(current_builddir, current_name, "h");
        generate_c_config_c generate_c_config(&config_s4o, &config_incl_s4o);
        symbol->accept(generate_c_config);

        config_s4o.print("unsigned long long common_ticktime__ = ");
        config_s4o.print_long_long_integer(common_ticktime);
        config_s4o.print("; /*ns*/\n");
        config_s4o.print("unsigned long greatest_tick_count__ = ");
        config_s4o.print_long_integer(calculate_common_ticktime.get_greatest_tick_count());
        config_s4o.print("; /*tick*/\n");
      }

      symbol->resource_declarations->accept(*this);

      current_configuration = NULL;
      return NULL;
    }

    void *visit(resource_declaration_c *symbol) {
      if (symbol->global_var_declarations != NULL)
        symbol->global_var_declarations->accept(generate_c_implicit_typedecl);
      symbol->resource_name->accept(*this);
      stage4out_c resources_s4o(current_builddir, current_name, "c");
      generate_c_resources_c generate_c_resources(&resources_s4o, current_configuration, symbol, common_ticktime);
      symbol->accept(generate_c_resources);
      return NULL;
    }

    void *visit(single_resource_declaration_c *symbol) {
      stage4out_c resources_s4o(current_builddir, "RESOURCE", "c");
      generate_c_resources_c generate_c_resources(&resources_s4o, current_configuration, symbol, common_ticktime);
      symbol->accept(generate_c_resources);
      return NULL;
    }
    
};

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/




visitor_c *new_code_generator(stage4out_c *s4o, const char *builddir)  {return new generate_c_c(s4o, builddir);}
void delete_code_generator(visitor_c *code_generator) {delete code_generator;}


