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
 * An IEC 61131-3 compiler.
 *
 * Based on the
 * FINAL DRAFT - IEC 61131-3, 2nd Ed. (2001-12-10)
 *
 */

/*
 * Seperation of type specification and default value constructs
 * (for e.g. simple_spec_init_c), into a type specificiation part,
 * and a default value part.
 */

#include "spec_init_separator.hh"
#include "../main.hh" // required for ERROR() and ERROR_MSG() macros.


//#define DEBUG
#ifdef DEBUG
#define TRACE(classname) printf("\n____%s____\n",classname);
#else
#define TRACE(classname)
#endif



spec_init_sperator_c *spec_init_sperator_c::get_class_instance(void) {
  if (NULL == class_instance)
    class_instance = new spec_init_sperator_c();

  if (NULL == class_instance)
    ERROR;

  return class_instance;
}

 /* the only two public functions... */
symbol_c *spec_init_sperator_c::get_spec(symbol_c *spec_init) {
   search_what = search_spec;
   return (symbol_c *)spec_init->accept(*get_class_instance());
}

symbol_c *spec_init_sperator_c::get_init(symbol_c *spec_init) {
   search_what = search_init;
   return (symbol_c *)spec_init->accept(*get_class_instance());
}

/*******************************************/
/* B 1.1 - Letters, digits and identifiers */
/*******************************************/
// SYM_TOKEN(identifier_c)
/* visitor for identifier_c should no longer be necessary. All references to derived datatypes are now stored in then */
/* AST using either poutype_identifier_c or derived_datatype_identifier_c                                             */
void *spec_init_sperator_c::visit(                 identifier_c *symbol) { ERROR; return NULL;} /* should never occur */

void *spec_init_sperator_c::visit(         poutype_identifier_c *symbol) {
  TRACE("spec_init_sperator_c::poutype_identifier_c");
  switch (search_what) {
    /* if we ever get called sith a simple identifier_c, then it must be a previously declared type... */
    case search_spec: return symbol;
    case search_init: return NULL;
  }
  ERROR; /* should never occur */
  return NULL;
  }
  
  
void *spec_init_sperator_c::visit(derived_datatype_identifier_c *symbol) {
  TRACE("spec_init_sperator_c::derived_datatype_identifier_c");
  switch (search_what) {
    /* if we ever get called sith a simple identifier_c, then it must be a previously declared type... */
    case search_spec: return symbol;
    case search_init: return NULL;
  }
  ERROR; /* should never occur */
  return NULL;
}


/********************************/
/* B 1.3.3 - Derived data types */
/********************************/

/* simple_specification ASSIGN constant */
void *spec_init_sperator_c::visit(simple_spec_init_c *symbol) {
  TRACE("spec_init_sperator_c::simple_spec_init_c");
  switch (search_what) {
    case search_spec: return symbol->simple_specification;
    case search_init: return symbol->constant;
  }
  ERROR; /* should never occur */
  return NULL;
}

/* subrange_specification ASSIGN signed_integer */
void *spec_init_sperator_c::visit(subrange_spec_init_c *symbol) {
  TRACE("spec_init_sperator_c::subrange_spec_init_c");
  switch (search_what) {
    case search_spec: return symbol->subrange_specification->accept(*this);
    case search_init: return symbol->signed_integer;
  }
  ERROR; /* should never occur */
  return NULL;
}

/*  integer_type_name '(' subrange')' */
void *spec_init_sperator_c::visit(subrange_specification_c *symbol) {
  TRACE("spec_init_sperator_c::subrange_specification_c");
  switch (search_what) {
    case search_spec: return symbol->integer_type_name;
    case search_init: return NULL; /* should never occur */
  }
  ERROR; /* should never occur */
  return NULL;
}

/* array_specification [ASSIGN array_initialization} */
/* array_initialization may be NULL ! */
void *spec_init_sperator_c::visit(array_spec_init_c *symbol) {
  TRACE("spec_init_sperator_c::array_spec_init_c");
  switch (search_what) {
    case search_spec: return symbol->array_specification;
    case search_init: return symbol->array_initialization;
  }
  return NULL;
}

/* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
/* NOTE: this symbol may be used directly in implictly defined variables in VAR_TEMP!! 
 *       so we must consider it here too!
 */
void *spec_init_sperator_c::visit(array_specification_c *symbol) {
  TRACE("spec_init_sperator_c::array_spec_init_c");
  switch (search_what) {
    case search_spec: return symbol;
    case search_init: return NULL;
  }
  return NULL;
}

/* enumerated_specification ASSIGN enumerated_value */
void *spec_init_sperator_c::visit(enumerated_spec_init_c *symbol) {
  TRACE("spec_init_sperator_c::enumerated_spec_init_c");
  switch (search_what) {
    case search_spec: return symbol->enumerated_specification;
    case search_init: return symbol->enumerated_value;
  }
  ERROR; /* should never occur */
  return NULL;
}

/* structure_type_name ASSIGN structure_initialization */
/* structure_initialization may be NULL ! */
//SYM_REF2(initialized_structure_c, structure_type_name, structure_initialization)
void *spec_init_sperator_c::visit(initialized_structure_c *symbol) {
  TRACE("spec_init_sperator_c::initialized_structure_c");
  switch (search_what) {
    case search_spec: return symbol->structure_type_name;
    case search_init: return symbol->structure_initialization;
  }
  ERROR; /* should never occur */
  return NULL;
}

/*  function_block_type_name ASSIGN structure_initialization */
/* structure_initialization -> may be NULL ! */
//SYM_REF2(fb_spec_init_c, function_block_type_name, structure_initialization)
void *spec_init_sperator_c::visit(fb_spec_init_c *symbol) {
  TRACE("spec_init_sperator_c::fb_spec_init_c");
  switch (search_what) {
    case search_spec: return symbol->function_block_type_name;
    case search_init: return symbol->structure_initialization;
  }
  ERROR; /* should never occur */
  return NULL;
}


/* ref_spec:  REF_TO (non_generic_type_name | function_block_type_name) */
// SYM_REF1(ref_spec_c, type_name)
void *spec_init_sperator_c::visit(ref_spec_c *symbol) {
  TRACE("spec_init_sperator_c::ref_spec_c");
  /* This visitor should never really be called, but there is no harm in including it... */
  switch (search_what) {
    case search_spec: return symbol;
    case search_init: return NULL; /* should never occur */
  }
  ERROR; /* should never occur */
  return NULL;
}


/* For the moment, we do not support initialising reference data types */
/* ref_spec_init: ref_spec [ ASSIGN ref_initialization ] */ 
/* NOTE: ref_initialization may be NULL!! */
// SYM_REF2(ref_spec_init_c, ref_spec, ref_initialization)
void *spec_init_sperator_c::visit(ref_spec_init_c *symbol) {
  TRACE("spec_init_sperator_c::ref_spec_init_c");
  switch (search_what) {
    case search_spec: return symbol->ref_spec;
    case search_init: return symbol->ref_initialization;
  }
  ERROR; /* should never occur */
  return NULL;

}



/******************************************/
/* B 1.4.3 - Declaration & Initialisation */
/******************************************/

/* STRING '[' integer ']' 
 * STRING ASSIGN single_byte_character_string
 * STRING '[' integer ']' ASSIGN single_byte_character_string
 */
void *spec_init_sperator_c::visit(single_byte_string_spec_c *symbol) {
  TRACE("spec_init_sperator_c::single_byte_string_spec_c");
  switch (search_what) {
    case search_spec: return symbol->string_spec;
    case search_init: return symbol->single_byte_character_string;
  }
  ERROR; /* should never occur */
  return NULL;
}

/* WSTRING '[' integer ']' 
 * WSTRING ASSIGN double_byte_character_string
 * WSTRING '[' integer ']' ASSIGN double_byte_character_string
 */
void *spec_init_sperator_c::visit(double_byte_string_spec_c *symbol) {
  TRACE("spec_init_sperator_c::double_byte_string_spec_c");
  switch (search_what) {
    case search_spec: return symbol->string_spec;
    case search_init: return symbol->double_byte_character_string;
  }
  ERROR; /* should never occur */
  return NULL;
}


spec_init_sperator_c *spec_init_sperator_c ::class_instance = NULL;
spec_init_sperator_c::search_what_t spec_init_sperator_c::search_what;
