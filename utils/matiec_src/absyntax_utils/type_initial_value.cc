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
 * Determine the default initial value of a type declaration.
 *
 * This is part of the 4th stage that generates
 * a c++ source program equivalent to the IL and ST
 * code.
 */
/* Given a type definition declration, determine its default
 * initial value. Note that types based on other types
 * may have to iterate through each type it is based on
 * to determine the initial value.
 * E.g.
 *  TYPE
 *    A_t : INT := 10;
 *    B_t : A_t := 20;
 *    C_t : B_t;
 *    D_t : C_t := 40;
 *  END_TYPE
 * Where the default initial value for C_t is 20!
 */
/* NOTE: The main program only needs one instance of
 *       this class of object. This class
 *       is therefore a singleton.
 */


#include "absyntax_utils.hh"

//#define DEBUG
#ifdef DEBUG
#define TRACE(classname) printf("\n____%s____\n",classname);
#else
#define TRACE(classname)
#endif


type_initial_value_c *type_initial_value_c::instance(void) {
  if (_instance != NULL)
    return _instance;

  _instance = new type_initial_value_c;

  null_literal = new ref_value_null_literal_c();
  real_0       = new real_c("0");
  integer_0    = new integer_c("0");
  integer_1    = new integer_c("1");
  bool_0       = new boolean_literal_c(new bool_type_name_c(),new boolean_false_c());
  /* FIXME: Our current implementation only allows dates from 1970 onwards,
   * but the standard defines the date 0001-01-01 as the default value
   * for the DATE data type. Untill we fix our implementation, we use 1970-01-01
   * as our default value!!
   */
//date_literal_0 =  new date_literal_c(integer_1, integer_1, integer_1);
  date_literal_0    = new date_literal_c(new integer_c("1970"), integer_1, integer_1);
  daytime_literal_0 = new daytime_c(integer_0, integer_0, real_0);
  time_0       = new duration_c     (new time_type_name_c(), NULL, new interval_c(NULL, NULL, NULL, integer_0, NULL));  // T#0s
  date_0       = new date_c         (new date_type_name_c(), date_literal_0);  //  D#0001-01-01
  tod_0        = new time_of_day_c  (new  tod_type_name_c(), daytime_literal_0);  //  TOD#00:00:00
  dt_0         = new date_and_time_c(new   dt_type_name_c(), date_literal_0, daytime_literal_0);  //  DT#0001-01-01-00:00:00
  string_0     = new single_byte_character_string_c("''");
  wstring_0    = new double_byte_character_string_c("\"\"");

  return _instance;
}

type_initial_value_c::type_initial_value_c(void) {}



symbol_c *type_initial_value_c::get(symbol_c *type) {
  TRACE("type_initial_value_c::get(): called ");
  return (symbol_c *)type->accept(*type_initial_value_c::instance());
}




void *type_initial_value_c::handle_type_spec(symbol_c *base_type_name, symbol_c *type_spec_init) {
  if (type_spec_init != NULL)
     return type_spec_init;
  /* no initial value specified, so we return the initial value of the type this type is based on... */
  return base_type_name->accept(*this);
}


void *type_initial_value_c::handle_type_name(symbol_c *type_name) {
  /* look up the type declaration... */
  type_symtable_t::iterator iter = type_symtable.find(type_name);
    /* Type declaration not found!! */
    /* NOTE: Variables declared out of function block 'data types',for eg:  VAR  timer: TON; END_VAR
     * do not have a default value, so (TON) will never be found in the type symbol table. This means 
     * we cannot simply consider this an error and abort, but must rather return a NULL.
     */
  if (iter == type_symtable.end())   return NULL;

  return iter->second->accept(*this);  // iter->second is the type_decl
}

/* visitor for identifier_c should no longer be necessary. All references to derived datatypes are now stored in then          */
/* AST using either poutype_identifier_c or derived_datatype_identifier_c. In principe, the following should not be necesasry  */
void *type_initial_value_c::visit(                 identifier_c *symbol) {return handle_type_name(symbol);} /* should never occur */
void *type_initial_value_c::visit(         poutype_identifier_c *symbol) {return handle_type_name(symbol);} /* in practice it might never get called, as FB, Functions and Programs do not have initial value  */
void *type_initial_value_c::visit(derived_datatype_identifier_c *symbol) {return handle_type_name(symbol);}

/***********************************/
/* B 1.3.1 - Elementary Data Types */
/***********************************/
void *type_initial_value_c::visit(time_type_name_c *symbol)         {return (void *)time_0;}
void *type_initial_value_c::visit(bool_type_name_c *symbol)         {return (void *)bool_0;}
void *type_initial_value_c::visit(sint_type_name_c *symbol)         {return (void *)integer_0;}
void *type_initial_value_c::visit(int_type_name_c *symbol)          {return (void *)integer_0;}
void *type_initial_value_c::visit(dint_type_name_c *symbol)         {return (void *)integer_0;}
void *type_initial_value_c::visit(lint_type_name_c *symbol)         {return (void *)integer_0;}
void *type_initial_value_c::visit(usint_type_name_c *symbol)        {return (void *)integer_0;}
void *type_initial_value_c::visit(uint_type_name_c *symbol)         {return (void *)integer_0;}
void *type_initial_value_c::visit(udint_type_name_c *symbol)        {return (void *)integer_0;}
void *type_initial_value_c::visit(ulint_type_name_c *symbol)        {return (void *)integer_0;}
void *type_initial_value_c::visit(real_type_name_c *symbol)         {return (void *)real_0;}
void *type_initial_value_c::visit(lreal_type_name_c *symbol)        {return (void *)real_0;}
void *type_initial_value_c::visit(date_type_name_c *symbol)         {return (void *)date_0;}
void *type_initial_value_c::visit(tod_type_name_c *symbol)          {return (void *)tod_0;}
void *type_initial_value_c::visit(dt_type_name_c *symbol)           {return (void *)dt_0;}
void *type_initial_value_c::visit(byte_type_name_c *symbol)         {return (void *)integer_0;}
void *type_initial_value_c::visit(word_type_name_c *symbol)         {return (void *)integer_0;}
void *type_initial_value_c::visit(dword_type_name_c *symbol)        {return (void *)integer_0;}
void *type_initial_value_c::visit(lword_type_name_c *symbol)        {return (void *)integer_0;}
void *type_initial_value_c::visit(string_type_name_c *symbol)       {return (void *)string_0;}
void *type_initial_value_c::visit(wstring_type_name_c *symbol)      {return (void *)wstring_0;}

void *type_initial_value_c::visit(safetime_type_name_c *symbol)     {return (void *)time_0;}
void *type_initial_value_c::visit(safebool_type_name_c *symbol)     {return (void *)bool_0;}
void *type_initial_value_c::visit(safesint_type_name_c *symbol)     {return (void *)integer_0;}
void *type_initial_value_c::visit(safeint_type_name_c *symbol)      {return (void *)integer_0;}
void *type_initial_value_c::visit(safedint_type_name_c *symbol)     {return (void *)integer_0;}
void *type_initial_value_c::visit(safelint_type_name_c *symbol)     {return (void *)integer_0;}
void *type_initial_value_c::visit(safeusint_type_name_c *symbol)    {return (void *)integer_0;}
void *type_initial_value_c::visit(safeuint_type_name_c *symbol)     {return (void *)integer_0;}
void *type_initial_value_c::visit(safeudint_type_name_c *symbol)    {return (void *)integer_0;}
void *type_initial_value_c::visit(safeulint_type_name_c *symbol)    {return (void *)integer_0;}
void *type_initial_value_c::visit(safereal_type_name_c *symbol)     {return (void *)real_0;}
void *type_initial_value_c::visit(safelreal_type_name_c *symbol)    {return (void *)real_0;}
void *type_initial_value_c::visit(safedate_type_name_c *symbol)     {return (void *)date_0;}
void *type_initial_value_c::visit(safetod_type_name_c *symbol)      {return (void *)tod_0;}
void *type_initial_value_c::visit(safedt_type_name_c *symbol)       {return (void *)dt_0;}
void *type_initial_value_c::visit(safebyte_type_name_c *symbol)     {return (void *)integer_0;}
void *type_initial_value_c::visit(safeword_type_name_c *symbol)     {return (void *)integer_0;}
void *type_initial_value_c::visit(safedword_type_name_c *symbol)    {return (void *)integer_0;}
void *type_initial_value_c::visit(safelword_type_name_c *symbol)    {return (void *)integer_0;}
void *type_initial_value_c::visit(safestring_type_name_c *symbol)   {return (void *)string_0;}
void *type_initial_value_c::visit(safewstring_type_name_c *symbol)  {return (void *)wstring_0;}

/********************************/
/* B 1.3.3 - Derived data types */
/********************************/
/*  simple_type_name ':' simple_spec_init */
void *type_initial_value_c::visit(simple_type_declaration_c *symbol) {
  return symbol->simple_spec_init->accept(*this);
}
/* simple_specification ASSIGN constant */
void *type_initial_value_c::visit(simple_spec_init_c *symbol) {
  return handle_type_spec(symbol->simple_specification, symbol->constant);
}
/*  subrange_type_name ':' subrange_spec_init */
void *type_initial_value_c::visit(subrange_type_declaration_c *symbol) {
  return symbol->subrange_spec_init->accept(*this);
}
/* subrange_specification ASSIGN signed_integer */
void *type_initial_value_c::visit(subrange_spec_init_c *symbol) {
  return handle_type_spec(symbol->subrange_specification, symbol->signed_integer);
}
/*  integer_type_name '(' subrange')' */
void *type_initial_value_c::visit(subrange_specification_c *symbol) {
 /* if no initial value explicitly given, then use the lowest value of the subrange */
  if (symbol->subrange != NULL)
    return symbol->subrange->accept(*this);
  else
    return symbol->integer_type_name->accept(*this);
}
/*  signed_integer DOTDOT signed_integer */
void *type_initial_value_c::visit(subrange_c *symbol)	{return symbol->lower_limit;}
/*  enumerated_type_name ':' enumerated_spec_init */
void *type_initial_value_c::visit(enumerated_type_declaration_c *symbol) {
  return symbol->enumerated_spec_init->accept(*this);
}
/* enumerated_specification ASSIGN enumerated_value */
void *type_initial_value_c::visit(enumerated_spec_init_c *symbol) {
  return handle_type_spec(symbol->enumerated_specification, symbol->enumerated_value);
}
/* helper symbol for enumerated_specification->enumerated_spec_init */
/* enumerated_value_list ',' enumerated_value */
void *type_initial_value_c::visit(enumerated_value_list_c *symbol) {
  /* stage1_2 never creates an enumerated_value_list_c with no entries. If this occurs, then something must have changed! */
  if (symbol->n <= 0) ERROR;
 /* if no initial value explicitly given, then use the lowest value of the subrange */
  return (void *)symbol->elements[0];
}
/* enumerated_type_name '#' identifier */
// SYM_REF2(enumerated_value_c, type, value)
void *type_initial_value_c::visit(enumerated_value_c *symbol)	{ERROR; return NULL;}
/*  identifier ':' array_spec_init */
void *type_initial_value_c::visit(array_type_declaration_c *symbol) {
  return symbol->array_spec_init->accept(*this);
}
/* array_specification [ASSIGN array_initialization} */
/* array_initialization may be NULL ! */
void *type_initial_value_c::visit(array_spec_init_c *symbol) {
  return handle_type_spec(symbol->array_specification, symbol->array_initialization);
}
/* ARRAY '[' array_subrange_list ']' OF non_generic_type_name */
void *type_initial_value_c::visit(array_specification_c *symbol)	{
  //symbol_c *init_value = (symbol_c *)symbol->non_generic_type_name->accept(*this);

  /* Now build a array_initial_elements_list_c list, and populate it
   * with 1 element of the array_initial_elements_c class
   */
  /* The array_initial_elements_c will contain a reference to the init_value,
   * and another constant representing the number of elements in the array.
   * In essence, we are building the equivilant of the following ST/IL code:
   *    New_array_t : ARRAY [1..30, 51..60] of INT := [40(XXX)];
   * from the user given code
   *    New_array_t : ARRAY [1..30, 51..60] of INT;
   * and replacing XXX with the default initial value of INT.
   */
  /* now we need to determine the number of elements in the array... */
  /* Easier said than done, as the array may have a list of subranges, as in the
   * example given above!!
   */
  /* TODO !!!!!*/
  /* For now, just assume an array with 1 element.
   * I (Mario) want to finish off this part of the code before getting boged down
   * in something else...
   */
  // NOTE: We are leaking memory, as the integer will never get free'd!!
  //integer_c *integer = new integer_c("1");
  // NOTE: We are leaking memory, as the array_initial_elements will never get free'd!!
  //array_initial_elements_c *array_initial_elements = new array_initial_elements_c(integer, init_value);
  // NOTE: We are leaking memory, as the array_initial_elements_list will never get free'd!!
  array_initial_elements_list_c *array_initial_elements_list  = new array_initial_elements_list_c();
  //array_initial_elements_list->add_element(array_initial_elements);
  return array_initial_elements_list;
}
/* helper symbol for array_specification */
/* array_subrange_list ',' subrange */
void *type_initial_value_c::visit(array_subrange_list_c *symbol)	{ERROR; return NULL;}
/* array_initialization:  '[' array_initial_elements_list ']' */
/* helper symbol for array_initialization */
/* array_initial_elements_list ',' array_initial_elements */
void *type_initial_value_c::visit(array_initial_elements_list_c *symbol)	{ERROR; return NULL;}
/* integer '(' [array_initial_element] ')' */
/* array_initial_element may be NULL ! */
void *type_initial_value_c::visit(array_initial_elements_c *symbol)	{ERROR; return NULL;}



/* TODO: from this point forward... */

/*  structure_type_name ':' structure_specification */
void *type_initial_value_c::visit(structure_type_declaration_c *symbol) {return NULL;}
/* structure_type_name ASSIGN structure_initialization */
/* structure_initialization may be NULL ! */
void *type_initial_value_c::visit(initialized_structure_c *symbol)	{
  return handle_type_spec(symbol->structure_type_name, symbol->structure_initialization);
}
/* helper symbol for structure_declaration */
/* structure_declaration:  STRUCT structure_element_declaration_list END_STRUCT */
/* structure_element_declaration_list structure_element_declaration ';' */
void *type_initial_value_c::visit(structure_element_declaration_list_c *symbol)	{
  structure_element_initialization_list_c *structure_element_initialization_list = new structure_element_initialization_list_c();
  return structure_element_initialization_list;
}
/*  structure_element_name ':' *_spec_init */
void *type_initial_value_c::visit(structure_element_declaration_c *symbol)	{return NULL;}
/* helper symbol for structure_initialization */
/* structure_initialization: '(' structure_element_initialization_list ')' */
/* structure_element_initialization_list ',' structure_element_initialization */
void *type_initial_value_c::visit(structure_element_initialization_list_c *symbol)	{return NULL;}
/*  structure_element_name ASSIGN value */
void *type_initial_value_c::visit(structure_element_initialization_c *symbol)	{return NULL;}
/*  string_type_name ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init */
/*
 * NOTE:
 * (Summary: Contrary to what is expected, the
 *           string_type_declaration_c is not used to store
 *           simple string type declarations that do not include
 *           size limits.
 *           For e.g.:
 *             str1_type: STRING := "hello!"
 *           will be stored in a simple_type_declaration_c
 *           instead of a string_type_declaration_c.
 *           The following:
 *             str2_type: STRING [64] := "hello!"
 *           will be stored in a sring_type_declaration_c
 *
 *           Read on for why this is done...
 * End Summary)
 *
 * According to the spec, the valid construct
 * TYPE new_str_type : STRING := "hello!"; END_TYPE
 * has two possible routes to type_declaration...
 *
 * Route 1:
 * type_declaration: single_element_type_declaration
 * single_element_type_declaration: simple_type_declaration
 * simple_type_declaration: identifier ':' simple_spec_init
 * simple_spec_init: simple_specification ASSIGN constant
 * (shift:  identifier <- 'new_str_type')
 * simple_specification: elementary_type_name
 * elementary_type_name: STRING
 * (shift: elementary_type_name <- STRING)
 * (reduce: simple_specification <- elementary_type_name)
 * (shift: constant <- "hello!")
 * (reduce: simple_spec_init: simple_specification ASSIGN constant)
 * (reduce: ...)
 *
 *
 * Route 2:
 * type_declaration: string_type_declaration
 * string_type_declaration: identifier ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init
 * (shift:  identifier <- 'new_str_type')
 * elementary_string_type_name: STRING
 * (shift: elementary_string_type_name <- STRING)
 * (shift: string_type_declaration_size <-  empty )
 * string_type_declaration_init: ASSIGN character_string
 * (shift: character_string <- "hello!")
 * (reduce: string_type_declaration_init <- ASSIGN character_string)
 * (reduce: string_type_declaration <- identifier ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init )
 * (reduce: type_declaration <- string_type_declaration)
 *
 *
 * At first glance it seems that removing route 1 would make
 * the most sense. Unfortunately the construct 'simple_spec_init'
 * shows up multiple times in other rules, so changing this construct
 * would also mean changing all the rules in which it appears.
 * I (Mario) therefore chose to remove route 2 instead. This means
 * that the above declaration gets stored in a
 * simple_type_declaration_c, and not in a string_type_declaration_c
 * as would be expected!
 */
/*  string_type_name ':' elementary_string_type_name string_type_declaration_size string_type_declaration_init */
// SYM_REF4(string_type_declaration_c,	string_type_name,
// 					elementary_string_type_name,
// 					string_type_declaration_size,
// 					string_type_declaration_init) /* may be == NULL! */
void *type_initial_value_c::visit(string_type_declaration_c *symbol)	{
  return handle_type_spec(symbol->elementary_string_type_name, symbol->string_type_declaration_init);
}


/* REF_TO (non_generic_type_name | function_block_type_name) */
void *type_initial_value_c::visit(ref_spec_c *symbol) {
  return null_literal;
}


/* ref_spec [ ASSIGN ref_initialization ]; */
/* NOTE: ref_initialization may be NULL!!  */
void *type_initial_value_c::visit(ref_spec_init_c *symbol) {
  return handle_type_spec(symbol->ref_spec, symbol->ref_initialization);
}
/* identifier ':' ref_spec_init */
void *type_initial_value_c::visit(ref_type_decl_c *symbol) {
  return symbol->ref_spec_init->accept(*this);
}








type_initial_value_c            *type_initial_value_c::_instance         = NULL;
ref_value_null_literal_c        *type_initial_value_c::null_literal      = NULL;
real_c                          *type_initial_value_c::real_0            = NULL;
integer_c                       *type_initial_value_c::integer_0         = NULL;
integer_c                       *type_initial_value_c::integer_1         = NULL;
boolean_literal_c               *type_initial_value_c::bool_0            = NULL;
date_literal_c                  *type_initial_value_c::date_literal_0    = NULL;
daytime_c                       *type_initial_value_c::daytime_literal_0 = NULL;
duration_c                      *type_initial_value_c::time_0            = NULL;
date_c                          *type_initial_value_c::date_0            = NULL;
time_of_day_c                   *type_initial_value_c::tod_0             = NULL;
date_and_time_c                 *type_initial_value_c::dt_0              = NULL;
single_byte_character_string_c  *type_initial_value_c::string_0          = NULL;
double_byte_character_string_c  *type_initial_value_c::wstring_0         = NULL;
