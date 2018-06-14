/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2014  Mario de Sousa (msousa@fe.up.pt)
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
 * Re-oder the POUs in te library so that no forward references occur.
 * 
 * Since stage1_2 now suppport POUs that contain references to POUS that are only declared later,
 * (e.g. a variable of FB1_t is declared, before the FB1_T function block is itself declared!)
 * we may need to re-order all the POUs in the library so that these forward references do not occur.
 * 
 * This utility function will do just that. However, it does not destroy the original abstract syntax 
 * tree (AST). It instead creates a new re-ordered AST, by instantiating a new library_c object.
 * This new library_c object will however point to the *same* objects of the original AST, just in 
 * a new order. 
 * This means that the new and original AST share all the object instances, and only use a distinct
 * library_c object!
 */

#include "remove_forward_dependencies.hh"
#include "../main.hh" // required for ERROR() and ERROR_MSG() macros.
#include "../absyntax_utils/absyntax_utils.hh"





#define FIRST_(symbol1, symbol2) (((symbol1)->first_order < (symbol2)->first_order)   ? (symbol1) : (symbol2))
#define  LAST_(symbol1, symbol2) (((symbol1)->last_order  > (symbol2)->last_order)    ? (symbol1) : (symbol2))

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



/* NOTE: We create an independent visitor for this task instead of having this done by the remove_forward_dependencies_c
 *       because we do not want to handle the ***_pragma_c classes while doing this search.
 *      (remove_forward_dependencies_c needs to visit those classes when handling all the possible entries in
 *       library_c, and must have those visitors)
 * 
 * NOTE:
 *      This class could have been written by visiting all the AST objects that could _reference_ a
 *          - FB type
 *          - Program type
 *          - Function type
 *      and by checking whether those references are in the declared_identifiers list.
 *      However, one of those objects, the ref_spec_c, may reference an FB type, or any other datatype, so we must have a way
 *      of knowing what is being referenced in this case. I have opted to introduce a new object type in the AST, the
 *      poutype_identifier_c, that will be used anywhere in the AST that references either a PROGRAM name or a FB type name
 *      or a FUNCTION name (previously a simple identifier_c was used!).
 *      This means that we merely need to visit the new poutype_identifier_c object in this visitor.
 */
class find_forward_dependencies_c: public search_visitor_c {
  private:
    identifiers_symbtable_t *declared_identifiers; // list of identifiers already declared by the symbols in the new tree
  public:
    find_forward_dependencies_c(identifiers_symbtable_t *declared_identifiers_) {declared_identifiers = declared_identifiers_;}
  /*******************************************/
  /* B 1.1 - Letters, digits and identifiers */
  /*******************************************/
  // return NULL if the symbol is already in the declared_identifiers symbol table, otherwise return the missing symbol!
  void *visit(            poutype_identifier_c *symbol)
    {if (declared_identifiers->find(symbol) != declared_identifiers->end()) return NULL; else return symbol;}
};   /* class find_forward_dependencies_c */




/* A class to count the number of POUs (Function, FBs Programs and Configurations) in a library.
 * This will be used to make sure whether we have copied all the POUs from the original AST (abstract
 * syntax tree) to the new AST.
 * Note that we can't simply use the number of 'elements' in the AST, as it may contain unknown/unsupported pragmas.
 */
class pou_count_c: public search_visitor_c {
  private:
    static pou_count_c *singleton;
    long long int count;

  public:
    static long long int get_count(library_c *library) {
      if (NULL == singleton) singleton = new pou_count_c;
      if (NULL == singleton) ERROR;
      singleton->count = 0;
      library->accept(*singleton);
      return singleton->count;
    }
    
    /**************************************/
    /* B.1.5 - Program organization units */
    /**************************************/
    void *visit(      function_declaration_c *symbol) {count++; return NULL;}
    void *visit(function_block_declaration_c *symbol) {count++; return NULL;} 
    void *visit(       program_declaration_c *symbol) {count++; return NULL;} 
    void *visit( configuration_declaration_c *symbol) {count++; return NULL;} 
};   /* class pou_count_c */

pou_count_c *pou_count_c::singleton = NULL;
symbol_c remove_forward_dependencies_c_null_symbol;








/************************************************************/
/************************************************************/
/******   The main class: Remove Forward Depencies    *******/
/************************************************************/
/************************************************************/

// constructor & destructor
remove_forward_dependencies_c:: remove_forward_dependencies_c(void) {
  find_forward_dependencies      = new find_forward_dependencies_c(&declared_identifiers);
  current_display_error_level = error_level_default;
  error_count = 0;
}

remove_forward_dependencies_c::~remove_forward_dependencies_c(void) {
  delete find_forward_dependencies;
}


int remove_forward_dependencies_c::get_error_count(void) {
  return error_count;
}



library_c *remove_forward_dependencies_c::create_new_tree(symbol_c *tree) {
  library_c *old_tree = dynamic_cast<library_c *>(tree);
  if (NULL == old_tree) ERROR;
  new_tree = new library_c;
  *((symbol_c *)new_tree) = *((symbol_c *)tree); // copy any annotations from tree to new_tree;
  new_tree->clear(); // remove all elements from list.
  tree->accept(*this);
  return new_tree;
}



void *remove_forward_dependencies_c::handle_library_symbol(symbol_c *symbol, symbol_c *name, symbol_c *search1, symbol_c *search2, symbol_c *search3) {
  if (inserted_symbols.find(symbol) != inserted_symbols.end())                 return NULL; // already previously inserted into new_tree and declared_identifiers. Do not handle again!
  if ((search1 != NULL) && (search1->accept(*find_forward_dependencies) != NULL)) return NULL; // A forward depency has not yet been satisfied. Wait for a later iteration to try again! 
  if ((search2 != NULL) && (search2->accept(*find_forward_dependencies) != NULL)) return NULL; // A forward depency has not yet been satisfied. Wait for a later iteration to try again!
  if ((search3 != NULL) && (search3->accept(*find_forward_dependencies) != NULL)) return NULL; // A forward depency has not yet been satisfied. Wait for a later iteration to try again!
  /* no forward dependencies found => insert into new AST, and add to the 'defined identifiers' and 'inserted symbol' lists */
  if (declared_identifiers.find(name) == declared_identifiers.end())
      declared_identifiers.insert(name, NULL);  // only add if not yet in the symbol table (an overloaded version of this same POU could have been inderted previously!)
  inserted_symbols.insert(symbol);
  new_tree->add_element(current_code_generation_pragma);  
  new_tree->add_element(symbol);  
  return NULL;
}


/* Tell the user that the source code contains a circular dependency */
void remove_forward_dependencies_c::print_circ_error(library_c *symbol) {
  /* Note that we only print Functions and FBs, as Programs and Configurations cannot contain circular references due to syntax rules */
  /* Note too that circular references in derived datatypes is also not possible due to sytax!                                        */
  int initial_error_count = error_count;
  for (int i = 0; i < symbol->n; i++) 
    if (   (inserted_symbols.find(symbol->elements[i]) == inserted_symbols.end())            // if not copied to new AST
        &&(  (NULL != dynamic_cast <function_block_declaration_c *>(symbol->elements[i]))    // and (is a FB  
           ||(NULL != dynamic_cast <      function_declaration_c *>(symbol->elements[i]))))  //      or a Function)
      STAGE3_ERROR(0, symbol->elements[i], symbol->elements[i], "POU (%s) contains a self-reference and/or belongs in a circular referencing loop", get_datatype_info_c::get_id_str(symbol->elements[i]));
  if (error_count == initial_error_count) ERROR; // We were unable to determine which POUs contain the circular references!!
}


/***************************/
/* B 0 - Programming Model */
/***************************/
/* enumvalue_symtable is filled in by enum_declaration_check_c, during stage3 semantic verification, with a list of all enumerated constants declared inside this POU */
// SYM_LIST(library_c, enumvalue_symtable_t enumvalue_symtable;)
void *remove_forward_dependencies_c::visit(library_c *symbol) {
  /* this method is the expected entry point for this visitor, and implements the main algorithm of the visitor */
  
  /* first insert all the derived datatype declarations, in the same order by which they are delcared in the original AST */
  /* Since IEC 61131-3 does not allow FBs in arrays or structures, it is actually safe to place all the datatypes before all the POUs! */
  for (int i = 0; i < symbol->n; i++) 
    if (NULL != dynamic_cast <data_type_declaration_c *>(symbol->elements[i]))
      new_tree->add_element(symbol->elements[i]);  

  /* now do the POUs, in whatever order is necessary to guarantee no forward references. */    
  long long int old_tree_pou_count = pou_count_c::get_count(symbol);
    // if no code generation pragma exists before the first entry in the library, the default is to enable code generation.
  enable_code_generation_pragma_c *default_code_generation_pragma = new enable_code_generation_pragma_c; 
  int prev_n;
  cycle_count = 0;
  do {
    cycle_count++;
    prev_n = new_tree->n;
    current_code_generation_pragma = default_code_generation_pragma;
    for (int i = 0; i < symbol->n; i++)  symbol->elements[i]->accept(*this);
  } while (prev_n != new_tree->n); // repeat while new elementns are still being added to the new AST
  
  if (old_tree_pou_count != pou_count_c::get_count(new_tree)) 
    print_circ_error(symbol);

  return NULL;
}


/**************************************/
/* B.1.5 - Program organization units */
/**************************************/
/***********************/
/* B 1.5.1 - Functions */
/***********************/
// SYM_REF4(function_declaration_c, derived_function_name, type_name, var_declarations_list, function_body, enumvalue_symtable_t enumvalue_symtable;)
void *remove_forward_dependencies_c::visit(function_declaration_c *symbol) 
  {return handle_library_symbol(symbol, symbol->derived_function_name, symbol->type_name, symbol->var_declarations_list, symbol->function_body);}
/*****************************/
/* B 1.5.2 - Function Blocks */
/*****************************/
/*  FUNCTION_BLOCK derived_function_block_name io_OR_other_var_declarations function_block_body END_FUNCTION_BLOCK */
// SYM_REF3(function_block_declaration_c, fblock_name, var_declarations, fblock_body, enumvalue_symtable_t enumvalue_symtable;)
void *remove_forward_dependencies_c::visit(function_block_declaration_c *symbol) 
  {return handle_library_symbol(symbol, symbol->fblock_name, symbol->var_declarations, symbol->fblock_body);}
/**********************/
/* B 1.5.3 - Programs */
/**********************/
/*  PROGRAM program_type_name program_var_declarations_list function_block_body END_PROGRAM */
// SYM_REF3(program_declaration_c, program_type_name, var_declarations, function_block_body, enumvalue_symtable_t enumvalue_symtable;)
void *remove_forward_dependencies_c::visit(program_declaration_c *symbol) 
  {return handle_library_symbol(symbol, symbol->program_type_name, symbol->var_declarations, symbol->function_block_body);}
/********************************/
/* B 1.7 Configuration elements */
/********************************/
/* CONFIGURATION configuration_name (...) END_CONFIGURATION */
// SYM_REF5(configuration_declaration_c, configuration_name, global_var_declarations, resource_declarations, access_declarations, instance_specific_initializations, enumvalue_symtable_t enumvalue_symtable;)
void *remove_forward_dependencies_c::visit(configuration_declaration_c *symbol) 
  {return handle_library_symbol(symbol, symbol->configuration_name, symbol->global_var_declarations, symbol->resource_declarations, symbol->access_declarations);}
/********************/
/* 2.1.6 - Pragmas  */
/********************/
void *remove_forward_dependencies_c::visit(disable_code_generation_pragma_c *symbol) {current_code_generation_pragma = symbol; return NULL;}
void *remove_forward_dependencies_c::visit( enable_code_generation_pragma_c *symbol) {current_code_generation_pragma = symbol; return NULL;}
/* I have no ideia what this pragma is. Where should we place it in the re-ordered tree? 
 * Without knowing the semantics of the pragma, it is not possible to hande it correctly.
 * We therefore print out an error message, and abort!
 */
// TODO: print error message!
void *remove_forward_dependencies_c::visit(pragma_c *symbol) {
  if (1 != cycle_count) return NULL; // only handle unknown pragmas in the first cycle!
  STAGE3_WARNING(symbol, symbol, "Unrecognized pragma. Including the pragma when using the '-p' command line option for 'allow use of forward references' may result in unwanted behaviour.");
  new_tree->add_element(symbol);
  return NULL;
}




