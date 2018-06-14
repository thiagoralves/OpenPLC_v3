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


/*
 *  Search for a specific label in an IL list. 
 *
 *  when instantiated, must be given a pointer to one of the following
 *     - function_declaration_c
 *     - function_block_declaration_c
 *     - program_declaration_c
 *     - instruction_list_c
 *
 * which is where all calls to search for a specific label will look for said label.
 */



#include "absyntax_utils.hh"



/* set to 1 to see debug info during execution */
static int debug = 0;

search_il_label_c::search_il_label_c(symbol_c *search_scope) {
  this->search_scope = search_scope;
  this->search_label = NULL;
}

search_il_label_c::~search_il_label_c(void) {
}


il_instruction_c *search_il_label_c::find_label(const char *label) {
  return find_label(new identifier_c(label));
}


il_instruction_c *search_il_label_c::find_label(symbol_c *label) {
  search_label = label;
  il_instruction_c *res = (il_instruction_c *)search_scope->accept(*this);
  search_label = NULL;
  return res;
}


/****************************************/
/* B.2 - Language IL (Instruction List) */
/****************************************/
/***********************************/
/* B 2.1 Instructions and Operands */
/***********************************/

/* | label ':' [il_incomplete_instruction] eol_list */
// SYM_REF2(il_instruction_c, label, il_instruction)
// void *visit(instruction_list_c *symbol);
void *search_il_label_c::visit(il_instruction_c *symbol) {
// printf("search_il_label_c::visit(il_instruction_c *symbol): searching for %s\n", ((identifier_c *)search_label)->value);
	if (NULL != symbol->label)
		if (compare_identifiers(search_label, symbol->label) == 0)
			return symbol;

	return NULL;
}




