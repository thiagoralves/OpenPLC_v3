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
 * Definition of the Abstract Syntax data structure components
 */

#include <stdio.h>
#include <stdlib.h>	/* required for exit() */
#include <string.h>

#include "absyntax.hh"
//#include "../stage1_2/iec.hh" /* required for BOGUS_TOKEN_ID, etc... */
#include "visitor.hh"
#include "../main.hh" // required for ERROR() and ERROR_MSG() macros.




/* The base class of all symbols */
symbol_c::symbol_c(
                   int first_line, int first_column, const char *ffile, long int first_order,
                   int last_line,  int last_column,  const char *lfile, long int last_order ) {
  this->first_file   = ffile,
  this->first_line   = first_line;
  this->first_column = first_column;
  this->first_order  = first_order;
  this->last_file    = lfile,
  this->last_line    = last_line;
  this->last_column  = last_column;
  this->last_order   = last_order;
  this->parent       = NULL;
  this->datatype     = NULL;
  this->scope        = NULL;
}



token_c::token_c(const char *value, 
                 int fl, int fc, const char *ffile, long int forder,
                 int ll, int lc, const char *lfile, long int lorder)
  :symbol_c(fl, fc, ffile, forder, ll, lc, lfile, lorder) {
  this->value = value;
//  printf("New token: %s\n", value);
}




# define LIST_CAP_INIT 8
# define LIST_CAP_INCR 8

list_c::list_c(
               int fl, int fc, const char *ffile, long int forder,
               int ll, int lc, const char *lfile, long int lorder)
  :symbol_c(fl, fc, ffile, forder, ll, lc, lfile, lorder),c(LIST_CAP_INIT) {
  n = 0;
  elements = (symbol_c**)malloc(LIST_CAP_INIT*sizeof(symbol_c*));
  if (NULL == elements) ERROR_MSG("out of memory");
}


list_c::list_c(symbol_c *elem, 
               int fl, int fc, const char *ffile, long int forder,
               int ll, int lc, const char *lfile, long int lorder)
  :symbol_c(fl, fc, ffile, forder, ll, lc, lfile, lorder),c(LIST_CAP_INIT) { 
  n = 0;
  elements = (symbol_c**)malloc(LIST_CAP_INIT*sizeof(symbol_c*));
  if (NULL == elements) ERROR_MSG("out of memory");
  add_element(elem); 
}


/* append a new element to the end of the list */
void list_c::add_element(symbol_c *elem) {
  // printf("list_c::add_element()\n");
  if (c <= n)
    if (!(elements=(symbol_c**)realloc(elements,(c+=LIST_CAP_INCR)*sizeof(symbol_c *))))
      ERROR_MSG("out of memory");
  elements[n++] = elem;
 
  if (NULL == elem) return;
  /* Sometimes add_element() is called in stage3 or stage4 to temporarily add an AST symbol to the list.
   * Since this symbol already belongs in some other place in the aST, it will have the 'parent' pointer set, 
   * and so we must not overwrite it. We only set the 'parent' pointer on new symbols that have the 'parent'
   * pointer still set to NULL.
   */
  if (NULL == elem->parent) elem->parent = this;  

  /* adjust the location parameters, taking into account the new element. */
  if (NULL == first_file) {
    first_file = elem->first_file;
    first_line = elem->first_line;
    first_column = elem->first_column;
  }
  if ((first_line == elem->first_line) && (first_column > elem->first_column)) {
    first_column = elem->first_column;
  }
  if (first_line > elem->first_line) {
    first_line = elem->first_line;
    first_column = elem->first_column;
  }
  if (NULL == last_file) {
    last_file = elem->last_file;
    last_line = elem->last_line;
    last_column = elem->last_column;
  }
  if ((last_line == elem->last_line) &&
      (last_column < elem->last_column)) {
    last_column = elem->last_column;
  }
  if (last_line < elem->last_line) {
    last_line = elem->last_line;
    last_column = elem->last_column;
  }
}

/* insert a new element before position pos. */
/* To insert into the begining of list, call with pos=0  */
/* To insert into the end of list, call with pos=list->n */
void list_c::insert_element(symbol_c *elem, int pos) {
  if((pos<0) || (n<pos)) ERROR;
  
  /* add new element to end of list. Basically alocate required memory... */
  /* will also increment n by 1 ! */
  add_element(elem);
  /* if not inserting into end position, shift all elements up one position, to open up a slot in pos for new element */
  if(pos < (n-1)){ 
    for(int i=n-2 ; i>=pos ; --i) elements[i+1] = elements[i];
    elements[pos] = elem;
  }
}


/* remove element at position pos. */
void list_c::remove_element(int pos) {
  if((pos<0) || (n<=pos)) ERROR;
  
  /* Shift all elements down one position, starting at the entry to delete. */
  for (int i = pos; i < n-1; i++) elements[i] = elements[i+1];
  /* corrent the new size */
  n--;
  /* elements = (symbol_c **)realloc(elements, n * sizeof(symbol_c *)); */
  /* TODO: adjust the location parameters, taking into account the removed element. */
}


/* remove element at position pos. */
void list_c::clear(void) {
  n = 0;
  /* TODO: adjust the location parameters, taking into account the removed element. */
}


#define SYM_LIST(class_name_c, ...)								\
class_name_c::class_name_c(									\
                           int fl, int fc, const char *ffile, long int forder,			\
                           int ll, int lc, const char *lfile, long int lorder)			\
                        :list_c(fl, fc, ffile, forder, ll, lc, lfile, lorder) {}		\
class_name_c::class_name_c(symbol_c *elem, 							\
                           int fl, int fc, const char *ffile, long int forder,			\
                           int ll, int lc, const char *lfile, long int lorder)			\
			:list_c(elem, fl, fc, ffile, forder, ll, lc, lfile, lorder) {}		\
void *class_name_c::accept(visitor_c &visitor) {return visitor.visit(this);}

#define SYM_TOKEN(class_name_c, ...)								\
class_name_c::class_name_c(const char *value, 							\
                           int fl, int fc, const char *ffile, long int forder,			\
                           int ll, int lc, const char *lfile, long int lorder)			\
			:token_c(value, fl, fc, ffile, forder, ll, lc, lfile, lorder) {}	\
void *class_name_c::accept(visitor_c &visitor) {return visitor.visit(this);}

#define SYM_REF0(class_name_c, ...)								\
class_name_c::class_name_c(									\
                           int fl, int fc, const char *ffile, long int forder,			\
                           int ll, int lc, const char *lfile, long int lorder)			\
			  :symbol_c(fl, fc, ffile, forder, ll, lc, lfile, lorder) {}		\
void *class_name_c::accept(visitor_c &visitor) {return visitor.visit(this);}


#define SYM_REF1(class_name_c, ref1, ...)							\
class_name_c::class_name_c(symbol_c *ref1,							\
                           int fl, int fc, const char *ffile, long int forder,			\
                           int ll, int lc, const char *lfile, long int lorder)			\
			  :symbol_c(fl, fc, ffile, forder, ll, lc, lfile, lorder) {		\
  this->ref1 = ref1;										\
  if  (NULL != ref1)   ref1->parent = this;							\
}												\
void *class_name_c::accept(visitor_c &visitor) {return visitor.visit(this);}


#define SYM_REF2(class_name_c, ref1, ref2, ...)							\
class_name_c::class_name_c(symbol_c *ref1,							\
			   symbol_c *ref2,							\
                           int fl, int fc, const char *ffile, long int forder,			\
                           int ll, int lc, const char *lfile, long int lorder)			\
			  :symbol_c(fl, fc, ffile, forder, ll, lc, lfile, lorder) {		\
  this->ref1 = ref1;										\
  this->ref2 = ref2;										\
  if  (NULL != ref1)   ref1->parent = this;							\
  if  (NULL != ref2)   ref2->parent = this;										\
}												\
void *class_name_c::accept(visitor_c &visitor) {return visitor.visit(this);}


#define SYM_REF3(class_name_c, ref1, ref2, ref3, ...)						\
class_name_c::class_name_c(symbol_c *ref1,							\
			   symbol_c *ref2,							\
			   symbol_c *ref3,							\
                           int fl, int fc, const char *ffile, long int forder,			\
                           int ll, int lc, const char *lfile, long int lorder)			\
			  :symbol_c(fl, fc, ffile, forder, ll, lc, lfile, lorder) {		\
  this->ref1 = ref1;										\
  this->ref2 = ref2;										\
  this->ref3 = ref3;										\
  if  (NULL != ref1)   ref1->parent = this;							\
  if  (NULL != ref2)   ref2->parent = this;							\
  if  (NULL != ref3)   ref3->parent = this;							\
}												\
void *class_name_c::accept(visitor_c &visitor) {return visitor.visit(this);}


#define SYM_REF4(class_name_c, ref1, ref2, ref3, ref4, ...)					\
class_name_c::class_name_c(symbol_c *ref1,							\
			   symbol_c *ref2,							\
			   symbol_c *ref3,							\
			   symbol_c *ref4,							\
                           int fl, int fc, const char *ffile, long int forder,			\
                           int ll, int lc, const char *lfile, long int lorder)			\
			  :symbol_c(fl, fc, ffile, forder, ll, lc, lfile, lorder) {		\
  this->ref1 = ref1;										\
  this->ref2 = ref2;										\
  this->ref3 = ref3;										\
  this->ref4 = ref4;										\
  if  (NULL != ref1)   ref1->parent = this;							\
  if  (NULL != ref2)   ref2->parent = this;							\
  if  (NULL != ref3)   ref3->parent = this;							\
  if  (NULL != ref4)   ref4->parent = this;							\
}												\
void *class_name_c::accept(visitor_c &visitor) {return visitor.visit(this);}


#define SYM_REF5(class_name_c, ref1, ref2, ref3, ref4, ref5, ...)				\
class_name_c::class_name_c(symbol_c *ref1,							\
			   symbol_c *ref2,							\
			   symbol_c *ref3,							\
			   symbol_c *ref4,							\
			   symbol_c *ref5,							\
                           int fl, int fc, const char *ffile, long int forder,			\
                           int ll, int lc, const char *lfile, long int lorder)			\
			  :symbol_c(fl, fc, ffile, forder, ll, lc, lfile, lorder) {		\
  this->ref1 = ref1;										\
  this->ref2 = ref2;										\
  this->ref3 = ref3;										\
  this->ref4 = ref4;										\
  this->ref5 = ref5;										\
  if  (NULL != ref1)   ref1->parent = this;							\
  if  (NULL != ref2)   ref2->parent = this;							\
  if  (NULL != ref3)   ref3->parent = this;							\
  if  (NULL != ref4)   ref4->parent = this;							\
  if  (NULL != ref5)   ref5->parent = this;							\
}												\
void *class_name_c::accept(visitor_c &visitor) {return visitor.visit(this);}



#define SYM_REF6(class_name_c, ref1, ref2, ref3, ref4, ref5, ref6, ...)				\
class_name_c::class_name_c(symbol_c *ref1,							\
			   symbol_c *ref2,							\
			   symbol_c *ref3,							\
			   symbol_c *ref4,							\
			   symbol_c *ref5,							\
			   symbol_c *ref6,							\
                           int fl, int fc, const char *ffile, long int forder,			\
                           int ll, int lc, const char *lfile, long int lorder)			\
			  :symbol_c(fl, fc, ffile, forder, ll, lc, lfile, lorder) {		\
  this->ref1 = ref1;										\
  this->ref2 = ref2;										\
  this->ref3 = ref3;										\
  this->ref4 = ref4;										\
  this->ref5 = ref5;										\
  this->ref6 = ref6;										\
  if  (NULL != ref1)   ref1->parent = this;							\
  if  (NULL != ref2)   ref2->parent = this;							\
  if  (NULL != ref3)   ref3->parent = this;							\
  if  (NULL != ref4)   ref4->parent = this;							\
  if  (NULL != ref5)   ref5->parent = this;							\
  if  (NULL != ref6)   ref6->parent = this;							\
}												\
void *class_name_c::accept(visitor_c &visitor) {return visitor.visit(this);}



#include "absyntax.def"




#undef SYM_LIST
#undef SYM_TOKEN
#undef SYM_TOKEN
#undef SYM_REF0
#undef SYM_REF1
#undef SYM_REF2
#undef SYM_REF3
#undef SYM_REF4
#undef SYM_REF5
#undef SYM_REF6





