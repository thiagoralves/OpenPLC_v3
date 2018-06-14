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
 * VISITOR.HH
 *
 *
 * The asbract syntax tree is scaned several times
 * during the whole compilation process. For e.g., stage 3 verifies
 * semantics, and stage 4 produces output.
 *
 * Since the number of functions that need to scan through
 * the abtract syntax tree may increase in time, (e.g. new
 * output formats, new semantic checks, etc...) while the
 * abstract syntax tree class structure is pretty stable
 * (i.e. the ST and IL languages will not be changing much...),
 * we use the 'visitor' design patern.
 *
 * It is not strictly necessary to use this pattern, but it does
 * have the advantage of bringing together the functions
 * that implement the same algorithm, each on a different
 * class of object in the abstract syntax tree.
 *
 *
 * This file contains the interface that each visitor class
 * must implement in order to be able to visit the abstract
 * syntax tree (class visitor_c)
 *
 * Three implementations of this interface are also provided,
 * that may be later extended to execute a particular algorithm.
 *
 * The null (class null_visitor_c) does nothing.
 *
 * The iterator (class iterator_visitor_c) iterates through
 * every object in the syntax tree.
 *
 * The search class (class search_visitor_c) iterates through
 * every object, until one returns a value != NULL.
 */


#ifndef _VISITOR_HH
#define _VISITOR_HH




#include "absyntax.hh"



#define SYM_LIST(class_name_c, ...)                                             virtual void *visit(class_name_c *symbol) = 0;
#define SYM_TOKEN(class_name_c, ...)                                            virtual void *visit(class_name_c *symbol) = 0;
#define SYM_REF0(class_name_c, ...)                                             virtual void *visit(class_name_c *symbol) = 0;
#define SYM_REF1(class_name_c, ref1, ...)                                       virtual void *visit(class_name_c *symbol) = 0;
#define SYM_REF2(class_name_c, ref1, ref2, ...)                                 virtual void *visit(class_name_c *symbol) = 0;
#define SYM_REF3(class_name_c, ref1, ref2, ref3, ...)                           virtual void *visit(class_name_c *symbol) = 0;
#define SYM_REF4(class_name_c, ref1, ref2, ref3, ref4, ...)                     virtual void *visit(class_name_c *symbol) = 0;
#define SYM_REF5(class_name_c, ref1, ref2, ref3, ref4, ref5, ...)               virtual void *visit(class_name_c *symbol) = 0;
#define SYM_REF6(class_name_c, ref1, ref2, ref3, ref4, ref5, ref6, ...)         virtual void *visit(class_name_c *symbol) = 0;

class visitor_c {
  public:
  #include "absyntax.def"

  virtual ~visitor_c(void);
};

#undef SYM_LIST
#undef SYM_TOKEN
#undef SYM_REF0
#undef SYM_REF1
#undef SYM_REF2
#undef SYM_REF3
#undef SYM_REF4
#undef SYM_REF5
#undef SYM_REF6






#define SYM_LIST(class_name_c, ...)                                             virtual void *visit(class_name_c *symbol);
#define SYM_TOKEN(class_name_c, ...)                                            virtual void *visit(class_name_c *symbol);
#define SYM_REF0(class_name_c, ...)                                             virtual void *visit(class_name_c *symbol);
#define SYM_REF1(class_name_c, ref1, ...)                                       virtual void *visit(class_name_c *symbol);
#define SYM_REF2(class_name_c, ref1, ref2, ...)                                 virtual void *visit(class_name_c *symbol);
#define SYM_REF3(class_name_c, ref1, ref2, ref3, ...)                           virtual void *visit(class_name_c *symbol);
#define SYM_REF4(class_name_c, ref1, ref2, ref3, ref4, ...)                     virtual void *visit(class_name_c *symbol);
#define SYM_REF5(class_name_c, ref1, ref2, ref3, ref4, ref5, ...)               virtual void *visit(class_name_c *symbol);
#define SYM_REF6(class_name_c, ref1, ref2, ref3, ref4, ref5, ref6, ...)         virtual void *visit(class_name_c *symbol);





class null_visitor_c: public visitor_c {
  public:
  #include "absyntax.def"

  virtual ~null_visitor_c(void);
};





class fcall_visitor_c: public visitor_c {
  public:
  virtual void fcall(symbol_c *symbol) = 0;

  public:
  #include "absyntax.def"

//   virtual ~fcall_visitor_c(void);
};




class iterator_visitor_c: public visitor_c {
  protected:
  void *visit_list(list_c *list);

  public:
  #include "absyntax.def"

  virtual ~iterator_visitor_c(void);
};





class fcall_iterator_visitor_c: public iterator_visitor_c {
  public:
  virtual void prefix_fcall(symbol_c *symbol);
  virtual void suffix_fcall(symbol_c *symbol);

  public:
  #include "absyntax.def"

  virtual ~fcall_iterator_visitor_c(void);
};





class search_visitor_c: public visitor_c {
  protected:
  void *visit_list(list_c *list);

  public:
  #include "absyntax.def"

  virtual ~search_visitor_c(void);
};


#undef SYM_LIST
#undef SYM_TOKEN
#undef SYM_REF0
#undef SYM_REF1
#undef SYM_REF2
#undef SYM_REF3
#undef SYM_REF4
#undef SYM_REF5
#undef SYM_REF6








#endif /*  _VISITOR_HH */
