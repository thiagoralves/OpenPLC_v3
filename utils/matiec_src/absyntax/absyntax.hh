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
 * Declaration of the Abstract Syntax data structure components
 */

/*
 * ABSYNTAX.H
 *
 * This generates the parse tree structure used to bind the components
 * identified by Bison in the correct syntax order. At the end of the
 * Bison analysis the tree is walked in a sequential fashion generating
 * the relavent code.
 */

#ifndef _ABSYNTAX_HH
#define _ABSYNTAX_HH


#include <stdio.h> // required for NULL
#include <vector>
#include <map>
#include <string>
#include <stdint.h>  // required for uint64_t, etc...
#include "../main.hh" // required for uint8_t, real_64_t, ..., and the macros INT8_MAX, REAL32_MAX, ... */




/* Forward declaration of the visitor interface
 * declared in the visitor.hh file
 * We cannot include the visitor.hh file, as it will
 * include this same file first, as it too requires references
 * to the abstract syntax classes defined here.
 */
class visitor_c; // forward declaration


class symbol_c; // forward declaration







/* Case insensitive string compare */
  /* Case insensitive string compare copied from
   * "The C++ Programming Language" - 3rd Edition
   * by Bjarne Stroustrup, ISBN 0201889544.
   */
class nocasecmp_c {
    public:
      bool operator() (const std::string& x, const std::string& y) const {
        std::string::const_iterator ix = x.begin();
        std::string::const_iterator iy = y.begin();

        for(; (ix != x.end()) && (iy != y.end()) && (toupper(*ix) == toupper(*iy)); ++ix, ++iy);
        if (ix == x.end()) return (iy != y.end());
        if (iy == y.end()) return false;
        return (toupper(*ix) < toupper(*iy));
      };
  };



/*** constant folding ***/
/* During stage 3 (semantic analysis/checking) we will be doing constant folding.
 * That algorithm will anotate the abstract syntax tree with the result of operations
 * on literals (i.e. 44 + 55 will store the result 99).
 * Since the same source code (e.g. 1 + 0) may actually be a BOOL or an ANY_INT,
 * or an ANY_BIT, we need to handle all possibilities, and determine the result of the
 * operation assuming each type.
 * For this reason, we have one entry for each possible type, with some expressions
 * having more than one entry filled in!
 */    
class const_value_c {
  public:
    typedef enum { cs_undefined,   /* not defined/not yet evaluated --> const_value is not valid! */
                   cs_non_const,   /* we have determined that expression is not a const value --> const_value is not valid! */
                   cs_const_value, /* const value is valid */
                   cs_overflow     /* result produced overflow or underflow --> const_value is not valid! */
                 } const_status_t;
 
    template<typename value_type> class const_value__ {
      const_status_t status;  
      value_type     value;
      
      public:
      const_value__(void): status(cs_undefined), value(0) {};
      
      value_type get(void)              {return value;}
      void       set(value_type value_) {status = cs_const_value; value = value_;}
      void       set_overflow(void)     {status = cs_overflow   ;}
      void       set_nonconst(void)     {status = cs_non_const  ;}
      bool       is_valid    (void)     {return (status == cs_const_value);}
      bool       is_overflow (void)     {return (status == cs_overflow   );}
      bool       is_nonconst (void)     {return (status == cs_non_const  );}
      bool       is_undefined(void)     {return (status == cs_undefined  );}
      bool       is_zero     (void)     {return (is_valid() && (get() == 0));}

      /* comparison operator */
      bool operator==(const const_value__ cv) {
        return (    ((status!=cs_const_value) && (status==cv.status)) 
                 || ((status==cs_const_value) && (value ==cv.value )));
      }

    };

    const_value__< int64_t>  _int64; /* status is initialised to UNDEFINED */
    const_value__<uint64_t> _uint64; /* status is initialised to UNDEFINED */
    const_value__<real64_t> _real64; /* status is initialised to UNDEFINED */
    const_value__<bool    >   _bool; /* status is initialised to UNDEFINED */
    
    /* default constructor and destructor */
     const_value_c(void) {};
    ~const_value_c(void) {};
    
    /* comparison operator */
    bool operator==(const const_value_c cv)
      {return ((_int64==cv._int64) && (_uint64==cv._uint64) && (_real64==cv._real64) && (_bool==cv._bool));}                                                     
      
    /* return true if at least one of the const values (int, real, ...) is a valid const value */
    bool is_const(void) 
      {return (_int64.is_valid() || _uint64.is_valid() || _real64.is_valid() || _bool.is_valid());}   
};



/* The base class of all symbols */
class symbol_c {

  public:
    /* WARNING: only use this method for debugging purposes!! */
    virtual const char *absyntax_cname(void) {return "symbol_c";};

    /*
     * Annotations produced during stage 1_2
     */    
    /* Points to the parent symbol in the AST, i.e. the symbol in the AST that will contain the current symbol */
    symbol_c *parent; 
    
    /* Line number for the purposes of error checking.  */
    int first_line;
    int first_column;
    const char *first_file;  /* filename referenced by first line/column */
    long int first_order;    /* relative order in which it is read by lexcial analyser */
    int last_line;
    int last_column;
    const char *last_file;  /* filename referenced by last line/column */
    long int last_order;    /* relative order in which it is read by lexcial analyser */


    /*
     * Annotations produced during stage 3
     */    
    /*** Data type analysis ***/
    std::vector <symbol_c *> candidate_datatypes; /* All possible data types the expression/literal/etc. may take. Filled in stage3 by fill_candidate_datatypes_c class */
    /* Data type of the expression/literal/etc. Filled in stage3 by narrow_candidate_datatypes_c 
     * If set to NULL, it means it has not yet been evaluated.
     * If it points to an object of type invalid_type_name_c, it means it is invalid.
     * Otherwise, it points to an object of the apropriate data type (e.g. int_type_name_c, bool_type_name_c, ...)
     */
    symbol_c *datatype;
    /* The POU in which the symbolic variable (or structured variable, or array variable, or located variable, - any more?)
     * was declared. This will point to a Configuration, Resource, Program, FB, or Function.
     * This is set in stage 3 by the datatype analyser algorithm (fill/narrow) for the symbols:
     *  symbolic_variable_c, array_variable_c, structured_variable_c
     */
    symbol_c *scope;    

    /*** constant folding ***/
    /* If the symbol has a constant numerical value, this will be set to that value by constant_folding_c */
    const_value_c const_value;
    
    /*** Enumeration datatype checking ***/    
    /* Not all symbols will contain the following anotations, which is why they are not declared here in symbol_c
     * They will be declared only inside the symbols that require them (have a look at absyntax.def)
     */
    typedef std::multimap<std::string, symbol_c *, nocasecmp_c> enumvalue_symtable_t;
    
    /*
     * Annotations produced during stage 4
     */
    /* Since we support several distinct stage_4 implementations, having explicit entries for each
     * possible use would quickly get out of hand.
     * We therefore simply add a map, that each stage 4 may use for all its needs.
     */
    typedef std::map<std::string, symbol_c *> anotations_map_t;
    anotations_map_t anotations_map;
    

  public:
    /* default constructor */
    symbol_c(int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0, /* order in which it is read by lexcial analyser */
             int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0  /* order in which it is read by lexcial analyser */
            );

    /* default destructor */
    /* must be virtual so compiler does not complain... */ 
    virtual ~symbol_c(void) {return;};

    virtual void *accept(visitor_c &visitor) {return NULL;};
};




class token_c: public symbol_c {
  public:
    /* WARNING: only use this method for debugging purposes!! */
    virtual const char *absyntax_cname(void) {return "token_c";};

    /* the value of the symbol. */
    const char *value;

  public:
    token_c(const char *value, 
            int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0, /* order in which it is read by lexcial analyser */
            int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0  /* order in which it is read by lexcial analyser */
           );
};




 /* a list of symbols... */
class list_c: public symbol_c {
  public:
    /* WARNING: only use this method for debugging purposes!! */
    virtual const char *absyntax_cname(void) {return "list_c";};

    int c,n; /* c: current capacity of list (malloc'd memory);  n: current number of elements in list */
    symbol_c **elements;

  public:
    list_c(int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0, /* order in which it is read by lexcial analyser */
           int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0  /* order in which it is read by lexcial analyser */
          );

    list_c(symbol_c *elem, 
           int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0, /* order in which it is read by lexcial analyser */
           int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0  /* order in which it is read by lexcial analyser */
          );
     /* append a new element to the end of the list */
    virtual void add_element(symbol_c *elem);
     /* insert a new element before position pos. */
     /* To insert into the begining of list, call with pos=0  */
     /* To insert into the end of list, call with pos=list->n */
    virtual void insert_element(symbol_c *elem, int pos = 0);
     /* remove element at position pos. */
    virtual void remove_element(int pos = 0);
     /* remove all elements from list. Does not delete the elements in the list! */ 
    virtual void clear(void);
};









#define SYM_LIST(class_name_c, ...)											\
class class_name_c:	public list_c {											\
  public:														\
    __VA_ARGS__														\
  public:														\
    class_name_c(													\
                 int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0,			\
                 int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0);			\
    class_name_c(symbol_c *elem, 											\
                 int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0,			\
                 int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0);			\
    virtual void *accept(visitor_c &visitor);										\
    /* WARNING: only use this method for debugging purposes!! */							\
    virtual const char *absyntax_cname(void) {return #class_name_c;};							\
};


#define SYM_TOKEN(class_name_c, ...)											\
class class_name_c: 	public token_c {										\
  public:														\
    __VA_ARGS__														\
  public:														\
    class_name_c(const char *value, 											\
                 int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0,			\
                 int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0);			\
    virtual void *accept(visitor_c &visitor);										\
    /* WARNING: only use this method for debugging purposes!! */							\
    virtual const char *absyntax_cname(void) {return #class_name_c;};							\
};


#define SYM_REF0(class_name_c, ...)											\
class class_name_c: public symbol_c {											\
  public:														\
    __VA_ARGS__														\
  public:														\
    class_name_c(		 											\
                 int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0,			\
                 int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0);			\
    virtual void *accept(visitor_c &visitor);										\
    /* WARNING: only use this method for debugging purposes!! */							\
    virtual const char *absyntax_cname(void) {return #class_name_c;};							\
};


#define SYM_REF1(class_name_c, ref1, ...)										\
class class_name_c: public symbol_c {											\
  public:														\
    symbol_c *ref1;													\
    __VA_ARGS__														\
  public:														\
    class_name_c(symbol_c *ref1,											\
                 int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0,			\
                 int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0);			\
    virtual void *accept(visitor_c &visitor);										\
    /* WARNING: only use this method for debugging purposes!! */							\
    virtual const char *absyntax_cname(void) {return #class_name_c;};							\
};


#define SYM_REF2(class_name_c, ref1, ref2, ...)										\
class class_name_c: public symbol_c {											\
  public:														\
    symbol_c *ref1;													\
    symbol_c *ref2;													\
    __VA_ARGS__														\
  public:														\
    class_name_c(symbol_c *ref1,											\
		 symbol_c *ref2 = NULL,											\
                 int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0,			\
                 int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0);			\
    virtual void *accept(visitor_c &visitor);										\
    /* WARNING: only use this method for debugging purposes!! */							\
    virtual const char *absyntax_cname(void) {return #class_name_c;};							\
};


#define SYM_REF3(class_name_c, ref1, ref2, ref3, ...)									\
class class_name_c: public symbol_c {											\
  public:														\
    symbol_c *ref1;													\
    symbol_c *ref2;													\
    symbol_c *ref3;													\
    __VA_ARGS__														\
  public:														\
    class_name_c(symbol_c *ref1,											\
		 symbol_c *ref2,											\
		 symbol_c *ref3,											\
                 int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0,			\
                 int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0);			\
    virtual void *accept(visitor_c &visitor);										\
    /* WARNING: only use this method for debugging purposes!! */							\
    virtual const char *absyntax_cname(void) {return #class_name_c;};							\
};


#define SYM_REF4(class_name_c, ref1, ref2, ref3, ref4, ...)								\
class class_name_c: public symbol_c {											\
  public:														\
    symbol_c *ref1;													\
    symbol_c *ref2;													\
    symbol_c *ref3;													\
    symbol_c *ref4;													\
    __VA_ARGS__														\
  public:														\
    class_name_c(symbol_c *ref1,											\
		 symbol_c *ref2,											\
		 symbol_c *ref3,											\
		 symbol_c *ref4 = NULL,											\
                 int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0,			\
                 int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0);			\
    virtual void *accept(visitor_c &visitor);										\
    /* WARNING: only use this method for debugging purposes!! */							\
    virtual const char *absyntax_cname(void) {return #class_name_c;};							\
};


#define SYM_REF5(class_name_c, ref1, ref2, ref3, ref4, ref5, ...)							\
class class_name_c: public symbol_c {											\
  public:														\
    symbol_c *ref1;													\
    symbol_c *ref2;													\
    symbol_c *ref3;													\
    symbol_c *ref4;													\
    symbol_c *ref5;													\
    __VA_ARGS__														\
  public:														\
    class_name_c(symbol_c *ref1,											\
		 symbol_c *ref2,											\
		 symbol_c *ref3,											\
		 symbol_c *ref4,											\
		 symbol_c *ref5,											\
                 int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0,			\
                 int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0);			\
    virtual void *accept(visitor_c &visitor);										\
    /* WARNING: only use this method for debugging purposes!! */							\
    virtual const char *absyntax_cname(void) {return #class_name_c;};							\
};


#define SYM_REF6(class_name_c, ref1, ref2, ref3, ref4, ref5, ref6, ...)							\
class class_name_c: public symbol_c {											\
  public:														\
    symbol_c *ref1;													\
    symbol_c *ref2;													\
    symbol_c *ref3;													\
    symbol_c *ref4;													\
    symbol_c *ref5;													\
    symbol_c *ref6;													\
    __VA_ARGS__														\
  public:														\
    class_name_c(symbol_c *ref1,											\
		 symbol_c *ref2,											\
		 symbol_c *ref3,											\
		 symbol_c *ref4,											\
		 symbol_c *ref5,											\
		 symbol_c *ref6 = NULL,											\
                 int fl = 0, int fc = 0, const char *ffile = NULL /* filename */, long int forder=0,			\
                 int ll = 0, int lc = 0, const char *lfile = NULL /* filename */, long int lorder=0);			\
    virtual void *accept(visitor_c &visitor);										\
    /* WARNING: only use this method for debugging purposes!! */							\
    virtual const char *absyntax_cname(void) {return #class_name_c;};							\
};


#include "absyntax.def"



#undef SYM_LIST
#undef SYM_TOKEN
#undef SYM_REF0
#undef SYM_REF1
#undef SYM_REF2
#undef SYM_REF3
#undef SYM_REF4
#undef SYM_REF5
#undef SYM_REF6

#endif /*  _ABSYNTAX_HH */
