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
 * A generic symbol table.
 *
 * This is used to create symbol tables such as a list of
 * variables currently in scope, a list of previously defined
 * functions, etc...
 */



#ifndef _SYMTABLE_HH
#define _SYMTABLE_HH

#include "../absyntax/absyntax.hh"

#include <map>
#include <string>




template<typename value_type> class symtable_c {
  /* Case insensitive string compare copied from
   * "The C++ Programming Language" - 3rd Edition
   * by Bjarne Stroustrup, ISBN 0201889544.
   */
  class nocase_c {
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

  public:
    typedef value_type value_t;

  private:
    /* Comparison between identifiers must ignore case, therefore the use of nocase_c */
    typedef std::map<std::string, value_t, nocase_c> base_t;
    base_t _base;

  public:
  typedef typename base_t::iterator iterator;
  typedef typename base_t::const_iterator const_iterator;
  typedef typename base_t::reverse_iterator reverse_iterator;
  typedef typename base_t::const_reverse_iterator const_reverse_iterator;

  private:
      /* pointer to symbol table of the next inner scope */
    symtable_c *inner_scope;

  public:
    symtable_c(void);

    void clear(void); /* clear all entries... */

    void push(void); /* create new inner scope */
    int  pop(void);  /* clear most inner scope */

    void set(const char *identifier_str, value_t value);    // Will change value associated to string if already in map. Will create new entry if string not in map.
    void set(const symbol_c *symbol, value_t value);        // Will change value associated to string if already in map. Will create new entry if string not in map.
    void insert(const char *identifier_str, value_t value); // insert a new (string,value) pair. Give an error if string already in map associated to different value!
    void insert(const symbol_c *symbol, value_t value);     // insert a new (string,value) pair. Give an error if string already in map associated to different value!

    value_t& operator[](const       char *identifier_str);
    value_t& operator[](const std::string identifier_str);
 // value_t& operator[](const   symbol_c *identifier    ); // not yet implemented
   
    /* Since symtable_c does not allow duplicates in each level, count() will return
     *  - 0 : if not found in any level
     *  - n : number of level containing that entry (max is current number of levels!)
     */
    int count(const       char *identifier_str);
    int count(const std::string identifier_str);
 // int count(const   symbol_c *identifier    ); // not yet implemented
    
    /* Search for an entry. Will return end() if not found */
    iterator               begin(void);
    iterator               end  (void);
    iterator               find (const char       *identifier_str);
    iterator               find (const std::string identifier_str);
    iterator               find (const symbol_c   *symbol        );


  /* iterators ... */
  /* NOTE: These member functions are incorrect, as the returned iterator will not iterate through the inner_scopes!! */
  /*       We simply comment it all out, as they are not currently needed!                                            */
  #if 0
    iterator               find (const char *identifier_str)
    iterator               begin()                          
    const_iterator         begin()  const                   
    iterator               end()                            
    const_iterator         end()    const                   
    reverse_iterator       rbegin()                         
    const_reverse_iterator rbegin() const                   
    reverse_iterator       rend()                           
    const_reverse_iterator rend()   const                   
  #endif
    /* debuging function... */
    void print(void);
};



/* Templates must include the source into the code! */
#include "symtable.cc"

#endif /*  _SYMTABLE_HH */
