/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
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


/* Declaration sequence is a source code part needed to declare variables.
 * There are some checks we need to do before start with other analysis:
 *
 *   - Check external option redefinition.
 *   - Check external data type redefinition.
 *   - Check initial values consistently with the data types of the variables/data types being declared.
 *   - Check whether a function block uses a CONSTANT qualifier as described in 2.5.2.1.
 *
 */


#include "enum_declaration_check.hh"



#define FIRST_(symbol1, symbol2) (((symbol1)->first_order < (symbol2)->first_order)   ? (symbol1) : (symbol2))
#define  LAST_(symbol1, symbol2) (((symbol1)->last_order  > (symbol2)->last_order)    ? (symbol1) : (symbol2))

#define STAGE3_ERROR(error_level, symbol1, symbol2, ...) {                                                                  \
  if (current_display_error_level >= error_level) {                                                                         \
    fprintf(stderr, "%s:%d-%d..%d-%d: error: ",                                                                             \
            FIRST_(symbol1,symbol2)->first_file, FIRST_(symbol1,symbol2)->first_line, FIRST_(symbol1,symbol2)->first_column,\
                                                 LAST_(symbol1,symbol2) ->last_line,  LAST_(symbol1,symbol2) ->last_column);\
    fprintf(stderr, __VA_ARGS__);                                                                                           \
    fprintf(stderr, "\n");                                                                                                  \
    error_count++;                                                                                                     \
  }                                                                                                                         \
}


#define STAGE3_WARNING(symbol1, symbol2, ...) {                                                                             \
    fprintf(stderr, "%s:%d-%d..%d-%d: warning: ",                                                                           \
            FIRST_(symbol1,symbol2)->first_file, FIRST_(symbol1,symbol2)->first_line, FIRST_(symbol1,symbol2)->first_column,\
                                                 LAST_(symbol1,symbol2) ->last_line,  LAST_(symbol1,symbol2) ->last_column);\
    fprintf(stderr, __VA_ARGS__);                                                                                           \
    fprintf(stderr, "\n");                                                                                                  \
    warning_found = true;                                                                                                   \
}








/*****************************************************/
/*                                                   */
/*  A small helper class...                          */
/*                                                   */
/*****************************************************/

/* Add to the local_enumerated_value_symtable the local enum value constants */
 

class populate_enumvalue_symtable_c: public iterator_visitor_c {
  private:
    symbol_c                        *current_enumerated_type;
    symbol_c::enumvalue_symtable_t  *enumvalue_symtable;

  private:
    int &error_count;
    int &current_display_error_level;    

  public:
     populate_enumvalue_symtable_c(int &error_count_, int &current_display_error_level_) 
       : error_count(error_count_), current_display_error_level(current_display_error_level_) {
       current_enumerated_type = NULL;  
       enumvalue_symtable = NULL;
     };
    ~populate_enumvalue_symtable_c(void) {}
    
    void populate(symbol_c::enumvalue_symtable_t *symtable, symbol_c *symbol) {
       enumvalue_symtable = symtable;
       symbol->accept(*this);
       enumvalue_symtable = NULL;
     }

  protected:
  /*************************/
  /* B.1 - Common elements */
  /*************************/
  /**********************/
  /* B.1.3 - Data types */
  /**********************/
  /********************************/
  /* B 1.3.3 - Derived data types */
  /********************************/
  /*  enumerated_type_name ':' enumerated_spec_init */
  void *visit(enumerated_type_declaration_c *symbol) {
    current_enumerated_type = symbol;
    symbol->enumerated_spec_init->accept(*this);
    current_enumerated_type = NULL;
    return NULL;
  }

  /* enumerated_specification ASSIGN enumerated_value */
  void *visit(enumerated_spec_init_c *symbol) {
    if (NULL == current_enumerated_type)
      current_enumerated_type = symbol;
    symbol->enumerated_specification->accept(*this);
    /* DO NOT visit the symbol->enumerated_value   !!! */
    current_enumerated_type = NULL;
    return NULL;
  }

  /* [enumerated_type_name '#'] identifier */
  void *visit(enumerated_value_c *symbol) {
    token_c *value = dynamic_cast <token_c *>(symbol->value);
    if (NULL == value) ERROR;
    const char *value_str = value->value;

    if (current_enumerated_type == NULL) ERROR;  
    /* this is really an ERROR! The initial value may use the syntax NUM_TYPE#enum_value, but in that case we should not have reached this visit method !! */
    if (symbol->type != NULL) ERROR;  

    symbol_c::enumvalue_symtable_t::iterator lower = enumvalue_symtable->lower_bound(value_str);
    symbol_c::enumvalue_symtable_t::iterator upper = enumvalue_symtable->upper_bound(value_str);
    for (; lower != upper; lower++)
      if (lower->second == current_enumerated_type) {
        /*  The same identifier is used more than once as an enumerated value/constant inside the same enumerated datat type! */
        STAGE3_ERROR(0, symbol, symbol, "Duplicate identifier in enumerated data type.");
        return NULL; /* No need to insert it! It is already in the table! */
      }
    
    /* add it to the local symbol table. */
    enumvalue_symtable->insert(std::pair<const char *, symbol_c *>(value_str, current_enumerated_type));
    return NULL;
  }
}; // class populate_enumvalue_symtable_c











enum_declaration_check_c::enum_declaration_check_c(symbol_c *ignore) {
  error_count = 0;
  current_display_error_level = 0;
  global_enumvalue_symtable = NULL;
  populate_enumvalue_symtable = new populate_enumvalue_symtable_c(error_count, current_display_error_level);
}

enum_declaration_check_c::~enum_declaration_check_c(void) {}

int enum_declaration_check_c::get_error_count() {return error_count;}


/***************************/
/* B 0 - Programming Model */
/***************************/
void *enum_declaration_check_c::visit(library_c *symbol) {
  global_enumvalue_symtable = &(symbol->enumvalue_symtable);
  iterator_visitor_c::visit(symbol); // fall back to base class
  return NULL;
}



/**********************/
/* B.1.3 - Data types */
/**********************/
void *enum_declaration_check_c::visit(data_type_declaration_c *symbol) {
  if (NULL == global_enumvalue_symtable) ERROR;
  populate_enumvalue_symtable->populate(global_enumvalue_symtable, symbol);
  return NULL;
}

/*********************/
/* B 1.5.1 Functions */
/*********************/
void *enum_declaration_check_c::visit(function_declaration_c *symbol) {
  populate_enumvalue_symtable->populate(&(symbol->enumvalue_symtable), symbol->var_declarations_list);
  return NULL;
}

/***************************/
/* B 1.5.2 Function blocks */
/***************************/
void *enum_declaration_check_c::visit(function_block_declaration_c *symbol) {
  populate_enumvalue_symtable->populate(&(symbol->enumvalue_symtable), symbol->var_declarations);
  return NULL;
}

/**********************/
/* B 1.5.3 - Programs */
/**********************/
void *enum_declaration_check_c::visit(program_declaration_c *symbol) {
  populate_enumvalue_symtable->populate(&(symbol->enumvalue_symtable), symbol->var_declarations);
  return NULL;
}

/********************************/
/* B 1.7 Configuration elements */
/********************************/
void *enum_declaration_check_c::visit(configuration_declaration_c *symbol) {
  if (NULL != symbol->global_var_declarations)
    populate_enumvalue_symtable->populate(&(symbol->enumvalue_symtable), symbol->global_var_declarations);
  if (NULL != symbol->resource_declarations)
    /* May reference either a list of resource_declaration_c, or a single_resource_declaration_c */
    populate_enumvalue_symtable->populate(&(symbol->enumvalue_symtable), symbol->resource_declarations);
  
  return NULL;
}


void *enum_declaration_check_c::visit(resource_declaration_c *symbol) {
  if (NULL != symbol->global_var_declarations)
    populate_enumvalue_symtable->populate(&(symbol->enumvalue_symtable), symbol->global_var_declarations);
  return NULL;
}


void *enum_declaration_check_c::visit(single_resource_declaration_c *symbol) {
  /* do NOT visit! */
  return NULL;
}
