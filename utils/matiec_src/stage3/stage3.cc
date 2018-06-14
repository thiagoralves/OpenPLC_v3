/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2009-2012  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2007-2011  Laurent Bessard and Edouard Tisserant
 *  Copyright (C) 2012       Manuele Conti (manuele.conti@sirius-es.it)
 *  Copyright (C) 2012       Matteo Facchinetti (matteo.facchinetti@sirius-es.it)
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

#include "stage3.hh"

#include "flow_control_analysis.hh"
#include "fill_candidate_datatypes.hh"
#include "narrow_candidate_datatypes.hh"
#include "forced_narrow_candidate_datatypes.hh"
#include "print_datatypes_error.hh"
#include "lvalue_check.hh"
#include "array_range_check.hh"
#include "case_elements_check.hh"
#include "constant_folding.hh"
#include "declaration_check.hh"
#include "enum_declaration_check.hh"
#include "remove_forward_dependencies.hh"



static int enum_declaration_check(symbol_c *tree_root){
    enum_declaration_check_c enum_declaration_check(NULL);
    tree_root->accept(enum_declaration_check);
    return enum_declaration_check.get_error_count();
}


/* In order to correctly handle variable sized arrays
 * declaration_safety() must only be run after constant folding!
 *   NOTE that the dependency does not resides directly in declaration_check_c,
 *        but rather indirectly in the call to get_datatype_info_c::is_type_equal()
 *        which may in turn call get_datatype_info_c::is_arraytype_equal_relaxed()
 *
 * Example of a variable sized array:
 *   VAR_EXTERN CONSTANT max: INT; END_VAR;
 *   VAR_EXTERN xx: ARRAY [1..max] OF INT; END_VAR;
 */
static int declaration_safety(symbol_c *tree_root){
    declaration_check_c declaration_check(tree_root);
    tree_root->accept(declaration_check);
    return declaration_check.get_error_count();
}

static int flow_control_analysis(symbol_c *tree_root){
    flow_control_analysis_c flow_control_analysis(tree_root);
    tree_root->accept(flow_control_analysis);
    return 0;
}


/* Constant folding assumes that flow control analysis has been completed!
 * so be sure to call flow_control_analysis() before calling this function!
 */
static int constant_propagation(symbol_c *tree_root){
    constant_propagation_c constant_propagation(tree_root);
    tree_root->accept(constant_propagation);
    return constant_propagation.get_error_count();
}


/* Type safety analysis assumes that 
 *    - flow control analysis 
 *    - constant folding (constant check)
 * has already been completed, so be sure to call those semantic checkers
 * before calling this function
 */
static int type_safety(symbol_c *tree_root){
	fill_candidate_datatypes_c fill_candidate_datatypes(tree_root);
	tree_root->accept(fill_candidate_datatypes);
	narrow_candidate_datatypes_c narrow_candidate_datatypes(tree_root);
	tree_root->accept(narrow_candidate_datatypes);
	print_datatypes_error_c print_datatypes_error(tree_root);
	tree_root->accept(print_datatypes_error);
	forced_narrow_candidate_datatypes_c forced_narrow_candidate_datatypes(tree_root);
	tree_root->accept(forced_narrow_candidate_datatypes);
	return print_datatypes_error.get_error_count();
}


/* Left value checking assumes that data type analysis has already been completed,
 * so be sure to call type_safety() before calling this function
 */
static int lvalue_check(symbol_c *tree_root){
	lvalue_check_c lvalue_check(tree_root);
	tree_root->accept(lvalue_check);
	return lvalue_check.get_error_count();
}

/* Array range check assumes that constant folding has been completed!
 * so be sure to call constant_folding() before calling this function!
 */
static int array_range_check(symbol_c *tree_root){
	array_range_check_c array_range_check(tree_root);
	tree_root->accept(array_range_check);
	return array_range_check.get_error_count();
}


/* Case options check assumes that constant folding has been completed!
 * so be sure to call constant_folding() before calling this function!
 */
static int case_elements_check(symbol_c *tree_root){
	case_elements_check_c case_elements_check(tree_root);
	tree_root->accept(case_elements_check);
	return case_elements_check.get_error_count();
}


/* Removing forward dependencies only makes sense when stage1_2 is run with the pre-parsing option.
 * This algorithm has no dependencies on other stage 3 algorithms.
 * Typically this is run last, just to show that the remaining algorithms also do not depend on the fact that 
 * the library_c (i.e. the source code) does not contain forward dependencies.
 */
static int remove_forward_dependencies(symbol_c *tree_root, symbol_c **ordered_tree_root) {
	if (NULL != ordered_tree_root)    *ordered_tree_root = tree_root; // by default, consider tree_root already ordered
	if (!runtime_options.pre_parsing)  return 0;                      // No re-ordering necessary, just return
	  
	/* We need to re-order the elements in the library, to fix any forward references! */
	remove_forward_dependencies_c remove_forward_dependencies;
	symbol_c *new_tree_root = remove_forward_dependencies.create_new_tree(tree_root);
	if (NULL ==     new_tree_root)   ERROR;
	if (NULL != ordered_tree_root)   *ordered_tree_root = new_tree_root;
	return remove_forward_dependencies.get_error_count();
}


int stage3(symbol_c *tree_root, symbol_c **ordered_tree_root) {
	int error_count = 0;
	error_count += enum_declaration_check(tree_root);
	error_count += flow_control_analysis(tree_root);
	error_count += constant_propagation(tree_root);
	error_count += declaration_safety(tree_root);
	error_count += type_safety(tree_root);
	error_count += lvalue_check(tree_root);
	error_count += array_range_check(tree_root);
	error_count += case_elements_check(tree_root);
	error_count += remove_forward_dependencies(tree_root, ordered_tree_root);
	
	if (error_count > 0) {
		fprintf(stderr, "%d error(s) found. Bailing out!\n", error_count); 
		return -1;
	}
	return 0;
}
