/*

  Copyright (C) 2008 STMicroelectronics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

*/

// isa_syntax_gen.h
////////////////////////////////////////////////////////////
//
// Interface for parsing and printing assembly instructions
// This file summarizes the new assembly instructions.
//
// void ISA_Syntax_Begin( void )
//    Initialization. To be called at the very beginning.
//
// void ISA_Syntax_End( void )
//    Complete processing.
//
///////////////////////////////////////////////////////////

#include "targ_isa_registers.h"
#include "targ_isa_lits.h"
#include "targ_isa_enums.h"
#include "targ_isa_subset.h"
#include "gen_util_gen.h"


// isa_syntax_gen.h
/////////////////////////////////////
//
//  Interface for instruction's syntax descriptions.
//
//  void ISA_Syntax_Begin( void )
//      Initialize to generate instruction syntax information for the 
//      architecture.  The information will be 
//	written to the files targ_isa_syntax.[ch].  <inst_bit_size>
//	indicates how many bits in an instruction (operation) word.
//
//  void ISA_Syntax_End(void)
//      Complete processing and generate targ_isa_print.[hc] and
//      <isa_subset>_targ_isa_syntax.pr parser description files (one by
//      ISA subset.
//
//  void Set_AsmName_Func(char *(*asmname_func)(TOP topcode))
//	Specifies a function which returns the assembly language name
//	to be used for the given topcode. If no function is specified,
//	the assembly language name is the same as TOP_Name.
//
//  void Set_Mandatory_Space(const char *str )
//      Specify mandatory space sequence. See ISA_Syntax description.
//
//  void Set_Optional_Space( const char *str )
//      Specify optional space sequence. See ISA_Syntax description.
//
//  void Set_Optional_Printed_Space( const char *str )
//      Specify optional printed space sequence. See ISA_Syntax description.
//
//  void Set_Tabulation( const char *str )
//      Specify tabulation sequence. See ISA_Syntax description.
//
//  void ISA_Parse_Globals  ( const char *globals,
//                            ISA_SUBSET subset=ISA_SUBSET_UNDEFINED )
//      Specify globals (static) variables, macros, routines used in
//      parsing process. If the ISA_SUBSET is specified, emits globals
//      only for this subset. By default, emits globals for all subsets.
//
//  void ISA_Parse_Rules    ( const char *rules,
//                            ISA_SUBSET subset=ISA_SUBSET_UNDEFINED )
//      Specify additional parsing rules that can be used in place of
//      operand/result parsing.
//
//  void ISA_Parse_Registers( const char *parse_reg_classes,
//                             ISA_SUBSET subset=ISA_SUBSET_UNDEFINED )
//      Specify register parsing function to be used when adding parsing
//      rules that contains new register class/subclass identifiers. For
//      example, 'Ra' could stand for register class integer and sub class
//      undefined.
//
//  void ISA_Parse_Literals ( const char *parse_lit_classes,
//                             ISA_SUBSET subset=ISA_SUBSET_UNDEFINED )
//      Specify literal parsing function to be used when adding parsing
//      rules that contains new literal class identifiers. For example,
//      'u12' could stands for literal class of 12 bits unsigned.
//
//  void ISA_Print_Globals  ( const char *globals )
//      Specify print hook functions. For example, it is possible to 
//      implement print of register list in case of push/pop instructions
//      via a specific routine that will be called during operand printing.
//
//  TYPE ISA_SYNTAX_RULE
//      An abstract type that describes different rules to print and parse 
//      a syntax.
//
//  ISA_SYNTAX_RULE ISA_Syntax_Rule_Create( const char *name,
//                                          bool must_be_parsed = true)
//      Used to create a new ISA_SYNTAX_RULE.  <name> is the rule name.
//	The alternative syntaxes attached to the create rule are added using
//      ISA_Syntax routine.
//
//  void ISA_Syntax( const char *syntax )
//      Used to add a new alternative syntax to previous created rule.
//      <syntax> specify the syntax format that will be printed or parsed.
//      Following special sequence is allowed:
//      %s:    printf's string format insertion and its associated flag characters.
//             For example, %-5s is accepted. This string format specify where
//             the mnemonic, the results and operands take place.
//      [TAB]: tabulation sequence specified by a call to the routine
//             Set_Tabulation( const char *str )
//             This sequence is translated into '\t' for printing and into
//             nothing for parsing. To be able to parse '\t', the [TAB] sequence
//             must not be specified using '\t'. In this case '\t', will be printed
//             and parsed.
//      [MS] : mandatory space specified by a call to the routine
//             Set_Mandatory_Space(const char *str )
//             This sequence is translated into ' ' for printing and into mandatory
//             space for parsing ('\t' in case of our parser input description).
//      [OS] : optional space specified by a call to the routine
//             Set_Optional_Space( const char *str )
//             This sequence is translated into nothing for printing and into optional
//             space for parsing (' ' in case of our parser input description).
//      [PS] : printed optional space specified by a call to the routine
//             Set_Optional_Printed_Space( const char *str )
//             This sequence is translated into ' ' for printing and into optional
//             space for parsing (' ' in case of our parser input description). 
//
//      The order in which the results and operands are printed is specified using
//      a sequence of the following two routines.
//
//  
//  void Name( void )
//	The instruction's name.
//
//  void Operand( int operand_index,
//           const char *print_action=0,
//           const char *parse_operand=0,
//           const char *parse_rule=0 )
//
//	The <operand_index>'th operand.
//      <print_action> defines a print hook that must return a string that will be
//      printed in place of the <operand_indes>'th operand. It can be a function call.
//      In this case, you have to declare and implement the function using
//      ISA_Print_Globals.
//      <parse_operand> defines the variable name that contains a specific TN. This
//      used when the syntax implies implicitly a specific operand. For example on
//      STxP70, when the predicate is not syntaxically specified, the default true
//      predicate (G7) is used. This must be also used when a <parse_rule> is given,
//      because the result of the given rule must be a new TN stored in a variable
//      defined by <parse_operand>.
//      <parse_rule> defines a parsing rule that must be used to parse this operand.
//
//  void Result( int result_index,
//          const char *print_action=0,
//          const char *parse_operand=0,
//          const char *parse_rule=0 )
//
//	The <operand_index>'th result. <print_action>, <parse_operand> and
//      <parse_rule> have the same definition as for Operand routine.
//
//  void ISA_Syntax_Print_Condition( const char *print_condition )
//      Defines a boolean condition for printing the current alternative syntax.
//      The following macros are usable:
//        O_VAL(operand_index): operand <operand_index>'th value.
//        O_NAME(operand_index): operand <operand_index>'th name.
//        R_VAL(result_index): result <result_index>'th value.
//        R_NAME(result_index): result <result_index>'th name.
//      Example: ISA_Syntax_Print_Condition("O_VAL(0) == 0");
//
//  void ISA_Syntax_Parse_Action   ( const char *parse_action )
//      Defines additional code in action of a parsing rule that will permit to 
//      instanciate TNs in case of implicit operand (case of STxP70 implicit predicate).
//      Note: fully implicit operands are automatically instanciated. This is the case
//      when there is only one register in register class or subclass.
//
//  void ISA_Syntax_Group(const char *name, ISA_SYNTAX_RULE rule, ...)
//	List all the instructions that use <rule> to define there comman alternative
//      syntaxes. All the instructions are enumerated in a list terminated
//      by TOP_UNDEFINED.
//
/////////////////////////////////////


#ifndef isa_syntax_gen_INCLUDED
#define isa_syntax_gen_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

  extern void ISA_Syntax_Begin( void );
  extern void ISA_Syntax_End( void );
  
  extern void ISA_Parse_Globals  ( const char *globals, ISA_SUBSET subset=ISA_SUBSET_UNDEFINED );
  extern void ISA_Parse_Rules    ( const char *rules, ISA_SUBSET subset=ISA_SUBSET_UNDEFINED );
  extern void ISA_Parse_Registers( const char *parse_reg_classes, ISA_SUBSET subset=ISA_SUBSET_UNDEFINED );
  extern void ISA_Parse_Literals ( const char *parse_lit_classes, ISA_SUBSET subset=ISA_SUBSET_UNDEFINED );

  extern void ISA_Print_Globals  ( const char *globals );
  
  typedef struct isa_syntax_rule_type *ISA_SYNTAX_RULE;
  extern ISA_SYNTAX_RULE ISA_Syntax_Rule_Create(const char *name, bool must_be_parsed = true);
  
  extern void ISA_Syntax( const char *syntax );
  extern void ISA_Syntax_Print_Condition( const char *print_condition );
  extern void ISA_Syntax_Parse_Action   ( const char *parse_action );

  extern void Operand( int operand_index, const char *print_action=0, const char *parse_operand=0, const char *parse_rule=0 );
  extern void Result( int result_index, const char *print_action=0, const char *parse_operand=0, const char *parse_rule=0 );
  extern void Name();

  extern void ISA_Syntax_Group(const char *name, ISA_SYNTAX_RULE rule, ...);

  extern void Set_AsmName_Func(const char *(*asmname_func)(TOP topcode));
  extern void Set_Mandatory_Space(const char *str );
  extern void Set_Optional_Space( const char *str );
  extern void Set_Optional_Printed_Space( const char *str );
  extern void Set_Tabulation( const char *str );

#ifdef __cplusplus
}
#endif
#endif /* isa_syntax_gen_INCLUDED */
