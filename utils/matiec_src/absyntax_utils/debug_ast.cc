/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *  Copyright (C) 2012  Mario de Sousa (msousa@fe.up.pt)
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
 * Some classes to help with debuging.
 *
 * These classes will print out the current state of a symbol or a portion of the Abstract Syntax Tree.
 */

/* TODO: Use a class similar to stage4out_c so that we can have nice indentation when printing an AST
 *       Create a template so that we can TRACE the execution of other visitor classes doing usefull work!
 */



#include <unistd.h>
#include <stdio.h>  /* required for NULL */
#include "absyntax_utils.hh"
#include "../absyntax/visitor.hh"




static void dump_cvalue(const_value_c const_value) {
  fprintf(stderr, "constv{");
  if      (const_value._real64.is_valid   ()) fprintf(stderr, "%f",        const_value._real64.get());
  else if (const_value._real64.is_overflow()) fprintf(stderr, "ov");
  else if (const_value._real64.is_nonconst()) fprintf(stderr, "nc");
  else                                        fprintf(stderr, "?");
  fprintf(stderr, ", i=");
  if      (const_value. _int64.is_valid   ()) fprintf(stderr, "%"PRId64"", const_value. _int64.get());
  else if (const_value. _int64.is_overflow()) fprintf(stderr, "ov");
  else if (const_value. _int64.is_nonconst()) fprintf(stderr, "nc");
  else                                        fprintf(stderr, "?");
  fprintf(stderr, ", u=");
  if      (const_value._uint64.is_valid   ()) fprintf(stderr, "%"PRIu64"", const_value._uint64.get());
  else if (const_value._uint64.is_overflow()) fprintf(stderr, "ov");
  else if (const_value._uint64.is_nonconst()) fprintf(stderr, "nc");
  else                                        fprintf(stderr, "?");
  fprintf(stderr, ", b=");
  if      (const_value.  _bool.is_valid   ()) fprintf(stderr, "%d",        const_value.  _bool.get()?1:0);
  else if (const_value.  _bool.is_overflow()) fprintf(stderr, "ov");
  else if (const_value.  _bool.is_nonconst()) fprintf(stderr, "nc");
  else                                        fprintf(stderr, "?");
  fprintf(stderr, "}");
}



/*********************************/
/* Class to print a symbol       */
/*********************************/


class print_symbol_c: public fcall_visitor_c { 
  public:
    static void print(symbol_c *symbol);
    
  protected:
    void fcall(symbol_c *symbol);  
    /* AST symbols with extra data have their own specialised methods for printing that data */
    void *visit(il_instruction_c *symbol);

  private:
    static print_symbol_c *singleton;
    
    void dump_symbol(symbol_c* symbol);
};




print_symbol_c *print_symbol_c::singleton = NULL;


void print_symbol_c::print(symbol_c* symbol) {
  if (NULL == singleton)   singleton = new print_symbol_c();
  if (NULL == singleton)   ERROR;

  symbol->accept(*singleton);
}





void print_symbol_c::fcall(symbol_c* symbol) {
  dump_symbol(symbol);
  fprintf(stderr, "\n");
}



void print_symbol_c::dump_symbol(symbol_c* symbol) {
  fprintf(stderr, "(%s->%03d:%03d..%03d:%03d) \t%s\t", symbol->first_file, symbol->first_line, symbol->first_column, symbol->last_line, symbol->last_column, symbol->absyntax_cname());

  fprintf(stderr, "  datatype=");
  if (NULL == symbol->datatype)
    fprintf(stderr, "NULL\t\t");
  else {
	  fprintf(stderr, "%s", symbol->datatype->absyntax_cname());
  }
  fprintf(stderr, "\t<-{");
  if (symbol->candidate_datatypes.size() == 0) {
    fprintf(stderr, "\t\t\t\t\t");
  } else if (symbol->candidate_datatypes.size() <= 2) {
    for (unsigned int i = 0; i < 2; i++)
      if (i < symbol->candidate_datatypes.size())
        fprintf(stderr, " %s,", symbol->candidate_datatypes[i]->absyntax_cname());
      else
        fprintf(stderr, "\t\t\t");
  } else {
    fprintf(stderr, "(%lu)\t\t\t\t\t", (unsigned long int)symbol->candidate_datatypes.size());
  }
  fprintf(stderr, "}\t ");         
  
  /* print the const values... */
  dump_cvalue(symbol->const_value);
  fprintf(stderr, "\t");
}



void *print_symbol_c::visit(il_instruction_c *symbol) {
   dump_symbol(symbol);

   /* NOTE: std::map.size() returns a size_type, whose type is dependent on compiler/platform. To be portable, we need to do an explicit type cast. */
  fprintf(stderr, "  next_il_=%lu ", (unsigned long int)symbol->next_il_instruction.size());
  fprintf(stderr, "  prev_il_=%lu ", (unsigned long int)symbol->prev_il_instruction.size());
  
  if (symbol->prev_il_instruction.size() == 0)
    fprintf(stderr, "(----,");
  else if (symbol->prev_il_instruction[0]->datatype == NULL)
    fprintf(stderr, "(NULL,");
  else if (!get_datatype_info_c::is_type_valid(symbol->prev_il_instruction[0]->datatype))
    fprintf(stderr, "(****,");
  else
    fprintf(stderr, "(    ,");
  
  if (symbol->next_il_instruction.size() == 0)
    fprintf(stderr, "----)");
  else if (symbol->next_il_instruction[0]->datatype == NULL)
    fprintf(stderr, "NULL)");
  else if (!get_datatype_info_c::is_type_valid(symbol->next_il_instruction[0]->datatype))
    fprintf(stderr, "****)");
  else 
    fprintf(stderr, "    )");
  
  fprintf(stderr, "\n");
  
  return NULL;
};





/*********************************/
/* Class to print an AST         */
/*********************************/

class print_ast_c: public fcall_iterator_visitor_c { 
  public:
    static void print(symbol_c *symbol);
    static void print(const char *str);
    
  protected:
    void prefix_fcall(symbol_c *symbol);
    void suffix_fcall(symbol_c *symbol);  
  
  private:
    static print_ast_c *singleton;    
};




print_ast_c *print_ast_c::singleton = NULL;


void print_ast_c::print(symbol_c* symbol) {
  if (NULL == singleton)   singleton = new print_ast_c();
  if (NULL == singleton)   ERROR;

  symbol->accept(*singleton);
}


void print_ast_c::print(const char *str) {
  fprintf(stderr, "%s", str);
}

  
void print_ast_c::prefix_fcall(symbol_c* symbol) {print_symbol_c::print(symbol);}
void print_ast_c::suffix_fcall(symbol_c* symbol) {}
  




/*********************************/
/* The DEBUG class               */
/*********************************/




void debug_c::print(const char *str) {
  fprintf(stderr, "%s", str);
}

void debug_c::print(const_value_c cvalue) {
  dump_cvalue(cvalue);
}

void debug_c::print(symbol_c *symbol) {
  print_symbol_c::print(symbol);
}

void debug_c::print_ast(symbol_c *symbol) {
  print_ast_c::print(symbol);
}





