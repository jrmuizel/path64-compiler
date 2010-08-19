/*
 *  Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright (C) 2008 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/


/* =======================================================================
 * =======================================================================
 *
 *  Module: ebo_special.h
 *  $Revision: 1.24 $
 *  $Date: 05/03/07 17:13:16-08:00 $
 *  $Author: kannann@iridot.keyresearch $
 *  $Source: be/cg/SCCS/s.ebo_special.h $
 *
 *  Revision comments:
 *
 *  17-June-1998 - Initial version
 *
 *  Description:
 *  ============
 *
 *  A couple of utility routines are included here because they require machine
 *  specific code.
 *
 *    EBO_Copy_Operand
 *         An index to the copied operand is returned, if the OP can be
 *         viewed as just moving bits from one location to another, and a -1,
 *         if not.
 *
 *    EBO_Can_Merge_Into_Offset
 *         If the OP is an add, subtract, or logical or with a small
 *         immediated constant, it may be possible to move the constant
 *         into the offset field of a memory operation and use the
 *         other operand as the index.
 *
 *  Special case optimizations are performed with the other routines.
 *  This includes simple things like transforming "x+0" into "x" and
 *  more complicated patterns such as "(x+1)+1" into "x+2".
 *
 *  As a gerneral philosophy, instructions that are redundant are not
 *  removed.  New instructions are inserted after them that redefine
 *  the result TN of the optimized instruction.  It is then left to
 *  the general dead-code eliminator to remove the unneeded definition.
 *
 *  These routines will return a TRUE value if the original OP has been
 *  optimized and can be deleted, and a FALSE value if the original value
 *  must be kept in the program.
 *
 *  delete_duplicate_op
 *	This routine handles the general case when a redundant operation
 *	is recognized.
 *
 *	The first argument is a pointer to the redundant
 *	OP, and the second operand is the EBO_OP_INFO entry for the
 *	preceeding matching entry.
 *
 *  delete_subset_mem_op
 *	This routine is called to delete a load instruction that has
 *	been matched with a store instruction.  This routine handles
 *	the case where the data being loaded is a subset of the data
 *	that was stored, thus requiring shifts and masking operations.
 *
 *	The first argument is a pointer to the redundant load OP,
 *	and the second operand is the EBO_OP_INFO entry for the
 *	preceeding matching store. The thrid argument is the offset of
 *	the memory addressed by the preceeding instruction and the fourth
 *	argument is the offset used in the redundantinstruction.
 *
 *  delete_reload_across_dependency
 *	This routine is called when the following sequence is found:
 *		store into location x
 *		store into location y
 *		load from location x (which could ansl be location y)
 *	The routine will attempt to generate a compare of the addreses
 *	x and y, and will then select the proper value to use to replace
 *	the load from x.
 *
 *  combine_adjacent_loads
 *	This routine merges two load instructions that reference adjacent
 *	memory locations into a single paired-load instruction.
 *
 *  Resolve_Conditional_Branch
 *	This routine attempts to determine which direction a conditional
 *	branch will go when the condition can be determined at
 *	compile time.
 *
 *  Fold_Constant_Expression
 *	This routine attempts to evaluate the instruction when
 *	all the operands have been determined to be constants.
 *
 *  Constant_Operand0
 *	This routine looks for special case optimizations when the
 *	first operand of the instruction is a constant.
 *
 *  Constant_Operand1
 *	This routine looks for special case optimizations when the
 *	second operand of the instruction is a constant.
 *
 *  Special_Sequence 
 *	This routine looks for special case optimizations involving
 *	a sequence of operations.
 *
 * =======================================================================
 * =======================================================================
 */
#ifdef TARG_ST
// Arthur: different interface

extern BOOL EBO_Can_Merge_Into_Offset (OP *op, INT *index, INT *immed);

// If extract opcode available, can replace some redundant loads
// with extract from a register.
extern BOOL EBO_replace_subset_load_with_extract (OP *op,
						  TN *pred_result,
						  TN *succ_result,
						  INT offset_pred,
						  INT offset_succ,
						  INT size_pred,
						  INT size_succ);

// Copy value of pred_result. intervening_result is the same
// register and may overwrite sign-extension, so make sure it is
// correct.
extern BOOL EBO_copy_value (OP *op,
			    TN *pred_result, 
			    TN *intervening_result,
			    INT size);

// Compare the reload address with the intervening store address.
// Select the stored value if the address are the same,
// and resuse the predecesor value if they are not the same.
extern BOOL EBO_select_value (OP *op,
			      TN *pred_result, 
			      TN *pred_base, 
			      TN *pred_offset,
			      TN *intervening_result,
			      TN *intervening_base,
			      TN *intervening_offset,
			      INT size);

extern BOOL EBO_condition_redundant(OP *elim_op,
				    EBO_TN_INFO **opnd_tninfo,
				    EBO_OP_INFO *prev_opinfo,
				    OPS *ops);

BOOL EBO_Try_To_Simplify_Operand0 (OP *op,
				   TN *tnr,
				   TN *tn0,
				   TN *tn1,
				   OPS *ops);

BOOL EBO_Try_To_Simplify_Operand1 (OP *op,
				   TN *tnr,
				   TN *tn0,
				   TN *tn1,
				   OPS *ops);

// 'op' has one of its operands of 'const_val' -- try to replace 
// with a more effcient opcode
extern OP* EBO_simplify_operand0 (OP *op,
				  TN *tnr, 
				  INT64 const_val, 
				  TN *tn1);

extern OP* EBO_simplify_operand1 (OP *op,
				  TN *tnr, 
				  TN *tn0, 
				  INT64 const_val);

BOOL EBO_Resolve_Conditional_Branch (OP *op, TN **opnd_tn);

// Two loads accessing the adjacent memory may be replaced by a
// single load that fills both destination registers.
BOOL EBO_combine_adjacent_loads(OP *op,
				TN **opnd_tn,
				EBO_TN_INFO **opnd_tninfo,
				EBO_OP_INFO *opinfo,
				INT64 pred_offset,
				INT64 succ_offset);

//
// Given a compare 'op' with compared operands constant, fold it.
// Return TRUE, if successfull
//
extern BOOL EBO_Fold_Constant_Compare (OP *op, TN **opnd_tn, OPS *ops);

//
// Given an 'op', see if a target-specific folding can be applied.
// If yes, 'result_val' has the folded value, and return TRUE.
//
extern BOOL EBO_Fold_Special_Opcode (OP *op, TN **opnd_tn, INT64 *result_val);

//BOOL EBO_Fold_Constant_Expression (OP *op,
//				   TN **opnd_tn,
//				   EBO_TN_INFO **opnd_tninfo);

BOOL EBO_Special_Sequence (OP *op,
			   TN **opnd_tn,
			   EBO_TN_INFO **opnd_tninfo);


// [CG]
// Returns a replacement TN not caught by OP_effectivelly_copy.
// The replacement TN must be a literal TN.
extern TN *EBO_literal_replacement_tn(OP *op);

BOOL EBO_Simplify_Special_Compare (OP *op,
				   TN **opnd_tn,
				   EBO_TN_INFO **opnd_tninfo);

#else
INT EBO_Copy_Operand (OP *op);

BOOL delete_duplicate_op (OP *op,
                          EBO_TN_INFO **opnd_tninfo,
                          EBO_OP_INFO *opinfo
#ifdef TARG_X8664
                          , EBO_TN_INFO **actual_tninfo = NULL
#endif
			  );

BOOL combine_adjacent_loads(OP *op,
                            EBO_TN_INFO **opnd_tninfo,
                            EBO_OP_INFO *opinfo,
                            INT64 pred_offset,
                            INT64 succ_offset);

BOOL delete_subset_mem_op(OP *op,
                          EBO_TN_INFO **opnd_tninfo,
                          EBO_OP_INFO *opinfo,
                          INT64 pred_offset,
                          INT64 succ_offset);

BOOL delete_reload_across_dependency (OP *op,
                                 EBO_TN_INFO **opnd_tninfo,
                                 EBO_OP_INFO *opinfo,
                                 EBO_OP_INFO *intervening_opinfo);

BOOL Resolve_Conditional_Branch (OP *op, TN **opnd_tn);

BOOL Fold_Constant_Expression (OP *op,
                               TN **opnd_tn,
                               EBO_TN_INFO **opnd_tninfo);

BOOL Constant_Operand0 (OP *op,
                        TN **opnd_tn,
                        EBO_TN_INFO **opnd_tninfo);

BOOL Constant_Operand1 (OP *op,
                        TN **opnd_tn,
                        EBO_TN_INFO **opnd_tninfo);

BOOL Special_Sequence (OP *op,
                       TN **opnd_tn,
                       EBO_TN_INFO **opnd_tninfo);
#ifdef TARG_X8664
BOOL Special_Asm_Sequence (OP *op,
			   TN **opnd_tn,
			   EBO_TN_INFO **opnd_tninfo);
#endif

#ifdef TARG_MIPS
void Redundancy_Elimination ();
BOOL EBO_Merge_Memory_Addr( OP*, TN**, EBO_TN_INFO**, EBO_TN_INFO** );
#endif

#ifdef TARG_X8664

class LOOP_DESCR;

void Update_op_must_not_be_moved( OP*, EBO_TN_INFO** );
BOOL EBO_Merge_Memory_Addr( OP*, TN**, EBO_TN_INFO**, EBO_TN_INFO** );
BOOL EBO_Not_Load_Exec_Opnd( OP* );
BOOL EBO_Fold_Lea_Const_Component( OP* );
BOOL EBO_Opt_Const_Array( OP*, LOOP_DESCR*, INT );
BOOL EBO_Load_Execution( OP*, TN**, EBO_TN_INFO**, int );
BOOL EBO_Lea_Insertion( OP*, TN**, EBO_TN_INFO** );
BOOL EBO_Fold_Load_Duplicate( OP*, TN**, EBO_TN_INFO** );
void Lea_Insertion ();
void Init_Load_Exec_Map( BB*, MEM_POOL* );
BOOL Delete_Unwanted_Prefetches( OP* );
BOOL EBO_Can_Eliminate_Zero_Opnd_OP(OP *);
#endif /* TARG_X8664 */

#ifdef TARG_MIPS //bug 14905
BOOL Delete_Redundant_Prefetches( OP* );
#endif

BOOL EBO_Can_Merge_Into_Offset (OP *op);

#ifdef KEY
void EBO_Special_Start( MEM_POOL* );
void EBO_Special_Finish();
BOOL Combine_L1_L2_Prefetches( OP*, TN**, EBO_TN_INFO** );
#endif /* TARG_ST */
#endif // KEY
