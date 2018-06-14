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
 * A generic symbol table that allows duplicate values.
 *
 * This is used to create a symbol table of previously defined
 * functions. Duplicate are allowed since the standard permits function\
 * overloading in the standard library.
 */



#ifndef _DSYMTABLE_HH
#define _DSYMTABLE_HH

#include "../absyntax/absyntax.hh"

#include <map>
#include <string>




template<typename value_type> class dsymtable_c {
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
    typedef std::multimap<std::string, value_t, nocase_c> base_t;
    base_t _base;

  public:
  typedef typename base_t::iterator iterator;
  typedef typename base_t::const_iterator const_iterator;
  typedef typename base_t::reverse_iterator reverse_iterator;
  typedef typename base_t::const_reverse_iterator const_reverse_iterator;

  private:
    const char *symbol_to_string(const symbol_c *symbol);

  public:
    dsymtable_c(void) {};

    void reset(void); /* clear all entries... */
    
    void insert(const char *identifier_str, value_t value);
    void insert(const symbol_c *symbol, value_t value);

    /* Determine how many entries are associated to key identifier_str */ 
    /* returns: 0 if no entry is found, 1 if 1 entry is found, ..., n if n entries are found */
    int count(const char *identifier_str)    {return _base.count(identifier_str);}
    int count(const symbol_c *symbol)        {return count(symbol_to_string(symbol));}
    
    /* Search for an entry associated with identifier_str. Will return end() if not found */
    iterator find(const char *identifier_str)        {return _base.find(identifier_str);}
    iterator find(const symbol_c *symbol)            {return find(symbol_to_string(symbol));}
    
    /* Search for the first entry associated with (i.e. with key ==) identifier_str. Will return end() if not found (NOTE: end() != end_value()) */
    iterator lower_bound(const char *identifier_str) {return ((count(identifier_str) == 0)? _base.end() : _base.lower_bound(identifier_str));}
    iterator lower_bound(const symbol_c *symbol)     {return lower_bound(symbol_to_string(symbol));}
    
    /* Search for the first entry with key greater than identifier_str. Will return end() if not found */
    iterator upper_bound(const char *identifier_str) {return ((count(identifier_str) == 0)? _base.end() : _base.upper_bound(identifier_str));}
    iterator upper_bound(const symbol_c *symbol)     {return upper_bound(symbol_to_string(symbol));}

    /* get the value to which an iterator is pointing to... */
    value_t get_value(const iterator i) {return i->second;}

  /* iterators pointing to beg/end of map... */
    iterator begin() 			{return _base.begin();}
    const_iterator begin() const	{return _base.begin();}
    iterator end()			{return _base.end();}
    const_iterator end() const 		{return _base.end();}
    reverse_iterator rbegin()		{return _base.rbegin();}
    const_reverse_iterator rbegin() const {return _base.rbegin();}
    reverse_iterator rend() 		{return _base.rend();}
    const_reverse_iterator rend() const	{return _base.rend();}

    /* debuging function... */
    void print(void);
};



/* Templates must include the source into the code! */
#include "dsymtable.cc"

#endif /*  _DSYMTABLE_HH */
