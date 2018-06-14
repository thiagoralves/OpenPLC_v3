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
 *  Data type analysis of IL code may leave some IL instructions with an undefined datatype.
 *  This visitor will set the datatype for all these symbols, so that all symbols have a well 
 *  defined datatype when we reach stage4.
 *
 *  Example:
 * =========
 *
 *  VAR 
 *     N   : INT := 99 ;    
 *     tonv: TON;
 *     byte_var: BYTE;
 *     tonv : TON;
 *     a : BYTE;
 *     t : time;
 *     tod1: tod;
 *  END_VAR
 *  
 * (0) --> Data type before executing forced_narrow_candidate_datatypes_c
 * (1) --> Data type after executing 1st pass of forced_narrow_candidate_datatypes_c
 * (2) --> Data type after executing 2nd pass of forced_narrow_candidate_datatypes_c
 * 
 * --- --> NULL (undefined datatype)
 * *** --> invalid_type_name_c (invalid datatype)
 *
 * (0)   PASS1   (1)    PASS2   (2)
 *      
 * ---     (e)   ***            ***       CAL tonv (                          
 *                                               PT := T#1s                   
 *                                            )                               
 * ---     (e)   ***            ***       JMP l4                              
 *
 * ---     (e)   sint           sint  l0: LD  1                               
 * ---     (e)   sint           sint      ADD 2                               
 * ---  (c)      sint           sint      CAL tonv (                          
 *                                            PT := T#1s                      
 *                                            )                               
 *                                                                            
 * ---     (e)   sint           sint      LD  45                              
 * ---  (c)      sint           sint      ADD 45                              
 *
 *
 * ---     (e)   sint           sint      LD  3                               
 * ---     (e)   sint           sint  l1:                                     
 * ---  (c)      sint           sint  l2: ADD 4                               
 * int           int            int       LD  5                               
 * int           int            int       ST  n                               
 * int           int            int       JMP l3                              
 *                                                                            
 * ---  (d)      ---      (e)   sint      LD  5                               
 * ---  (d)      ---      (e)   sint      SUB 6                               
 * ---  (d)(e)   sint           sint      JMP l1                              
 *
 * ---     (e)   bool           bool      LD  FALSE                           
 * ---     (e)   bool           bool      NOT                                 
 * ---  (b)      bool           bool      RET                                 
 *
 * int           int            int   l3:                                     
 * int           int            int       ST  n                               
 * ---  (b)      int            int       RET                                 
 *
 * ---     (e)   ***            ***   l4:                                     
 * ---     (e)   ***            ***       CAL tonv (                          
 *                                               PT := T#1s                   
 *                                            )                               
 * ---  (a)      ***            ***       JMP l0                              
 * ---  (b)      byte           byte      LD  88                              
 *
 *
 *   
 */



#include "forced_narrow_candidate_datatypes.hh"
#include "datatype_functions.hh"


/* set to 1 to see debug info during execution */
static int debug = 0;

forced_narrow_candidate_datatypes_c::forced_narrow_candidate_datatypes_c(symbol_c *ignore) 
 :narrow_candidate_datatypes_c(ignore) {
}

forced_narrow_candidate_datatypes_c::~forced_narrow_candidate_datatypes_c(void) {
}



void forced_narrow_candidate_datatypes_c::forced_narrow_il_instruction(symbol_c *symbol, std::vector <symbol_c *> &next_il_instruction) {
  if (NULL == symbol->datatype) {
    if (symbol->candidate_datatypes.empty()) {
      symbol->datatype = &(get_datatype_info_c::invalid_type_name); // This will occur in the situations (a) in the above example
      // return NULL; // No need to return control to the visit() method of the base class... But we do so, just to be safe (called at the end of this function)!
    } else {
      if (next_il_instruction.empty()) {
        symbol->datatype = symbol->candidate_datatypes[0]; // This will occur in the situations (b) in the above example
      } else {
        symbol_c *next_datatype = NULL;

        /* find the datatype of the following IL instructions (they should all be identical by now, but we don't have an assertion checking for this. */
        for (unsigned int i=0; i < next_il_instruction.size(); i++)
          if (NULL != next_il_instruction[i]->datatype)
            next_datatype = next_il_instruction[i]->datatype;
        if (get_datatype_info_c::is_type_valid(next_datatype)) {
          //  This will occur in the situations (c) in the above example
          symbol->datatype = symbol->candidate_datatypes[0]; 
        } else {
          //  This will occur in the situations (d) in the above example
          // it is not possible to determine the exact situation in the current pass, so we can't do anything just yet. Leave it for the next time around!
        }
      }
    }
  }
}



/****************************************/
/* B.2 - Language IL (Instruction List) */
/****************************************/
/***********************************/
/* B 2.1 Instructions and Operands */
/***********************************/

/*| instruction_list il_instruction */
// SYM_LIST(instruction_list_c)
void *forced_narrow_candidate_datatypes_c::visit(instruction_list_c *symbol) {
  for(int j = 0; j < 2; j++) {
    for(int i = symbol->n-1; i >= 0; i--) {
      symbol->elements[i]->accept(*this);
    }
  }

  /* Assert that this algorithm managed to remove all NULL datatypes! */
  /* NOTE: The forced_narrow_candidate_datatypes_c assumes that the original IEC 61131-3 source code does not have any bugs!
   *       This means we cannot run this assertion here, as the compiler will bork in the presence of bug in the code being compiled! Not good!!
   */
  /*
  for(int i = symbol->n-1; i >= 0; i--) {
    if (NULL == symbol->elements[i]->datatype)
      ERROR;
  }
  */
  
  return NULL;
}


  
/* | label ':' [il_incomplete_instruction] eol_list */
// SYM_REF2(il_instruction_c, label, il_instruction)
// void *visit(instruction_list_c *symbol);
void *forced_narrow_candidate_datatypes_c::visit(il_instruction_c *symbol) {
  forced_narrow_il_instruction(symbol, symbol->next_il_instruction);
  
  /* return control to the visit() method of the base class! */
  return narrow_candidate_datatypes_c::visit(symbol);  //  This handles the situations (e) in the above example
}





/* | il_simple_operator [il_operand] */
// SYM_REF2(il_simple_operation_c, il_simple_operator, il_operand)
// void *forced_narrow_candidate_datatypes_c::visit(il_simple_operation_c *symbol)

/* | function_name [il_operand_list] */
/* NOTE: The parameters 'called_function_declaration' and 'extensible_param_count' are used to pass data between the stage 3 and stage 4. */
// SYM_REF2(il_function_call_c, function_name, il_operand_list, symbol_c *called_function_declaration; int extensible_param_count;)
// void *forced_narrow_candidate_datatypes_c::visit(il_function_call_c *symbol) 

/* | il_expr_operator '(' [il_operand] eol_list [simple_instr_list] ')' */
// SYM_REF3(il_expression_c, il_expr_operator, il_operand, simple_instr_list);
// void *forced_narrow_candidate_datatypes_c::visit(il_expression_c *symbol)

/*  il_jump_operator label */
// SYM_REF2(il_jump_operation_c, il_jump_operator, label)
// void *forced_narrow_candidate_datatypes_c::visit(il_jump_operation_c *symbol)

/*   il_call_operator prev_declared_fb_name
 * | il_call_operator prev_declared_fb_name '(' ')'
 * | il_call_operator prev_declared_fb_name '(' eol_list ')'
 * | il_call_operator prev_declared_fb_name '(' il_operand_list ')'
 * | il_call_operator prev_declared_fb_name '(' eol_list il_param_list ')'
 */
/* NOTE: The parameter 'called_fb_declaration'is used to pass data between stage 3 and stage4 (although currently it is not used in stage 4 */
// SYM_REF4(il_fb_call_c, il_call_operator, fb_name, il_operand_list, il_param_list, symbol_c *called_fb_declaration)
// void *forced_narrow_candidate_datatypes_c::visit(il_fb_call_c *symbol)

/* | function_name '(' eol_list [il_param_list] ')' */
/* NOTE: The parameter 'called_function_declaration' is used to pass data between the stage 3 and stage 4. */
// SYM_REF2(il_formal_funct_call_c, function_name, il_param_list, symbol_c *called_function_declaration; int extensible_param_count;)
// void *forced_narrow_candidate_datatypes_c::visit(il_formal_funct_call_c *symbol)

// void *visit(il_operand_list_c *symbol);
// void *forced_narrow_candidate_datatypes_c::visit(simple_instr_list_c *symbol)

// SYM_REF1(il_simple_instruction_c, il_simple_instruction, symbol_c *prev_il_instruction;)
void *forced_narrow_candidate_datatypes_c::visit(il_simple_instruction_c*symbol) {
  forced_narrow_il_instruction(symbol, symbol->next_il_instruction);
  
  /* return control to the visit() method of the base class! */
  return narrow_candidate_datatypes_c::visit(symbol);  //  This handle the situations (e) in the above example
}


/*
    void *visit(il_param_list_c *symbol);
    void *visit(il_param_assignment_c *symbol);
    void *visit(il_param_out_assignment_c *symbol);
 */



/***************************************/
/* B.3 - Language ST (Structured Text) */
/***************************************/
// SYM_LIST(statement_list_c)
/* The normal narrow_candidate_datatypes_c algorithm does not leave any symbol, in an ST code, with an undefined datatype.
 * There is therefore no need to re-run the narrow algorithm here, so we overide the narrow_candidate_datatypes_c visitor,
 * and simply bug out!
 */
void *forced_narrow_candidate_datatypes_c::visit(statement_list_c *symbol) {return NULL;}

