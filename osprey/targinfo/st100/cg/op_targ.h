/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


/* ====================================================================
 * ====================================================================
 *
 *  CGIR operation structure (OP) and utility routines which include target
 *  dependencies.
 *
 *  THIS FILE IS ONLY TO BE INCLUDE BY ../op.h!!!!
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef op_targ_INCLUDED
#define op_targ_INCLUDED

#define OP_COPY_OPND 1
#define OP_PREDICATE_OPND 0

/*
 * These are only here because I need to make cg.so compile.
 * Later these will not be seen in the "target independent"
 * part of cg.
 */
#define TOP_is_likely(t)	 (FALSE)
#define TOP_is_side_effects(t)   (FALSE)
#define TOP_is_noop(t)           (t == TOP_noop)

/* Convenience access macros for properties of the OP */
/* TODO: define all the macros for OP properties. */
#define OP_noop(o)		(TOP_is_noop(OP_code(o)))
#define OP_load(o)		(TOP_is_load(OP_code(o)))
#define OP_store(o)		(TOP_is_store(OP_code(o)))
#define OP_memory(o)		(OP_load(o) | OP_store(o))
#define OP_call(o)		(TOP_is_call(OP_code(o)))
//#define OP_branch(o)            (TOP_is_branch(OP_code(o)))
#define OP_dummy(o)		(TOP_is_dummy(OP_code(o)))
#define OP_unalign_ld(o)	(TOP_is_unalign_ld(OP_code(o)))
#define OP_unalign_store(o)	(TOP_is_unalign_store(OP_code(o)))
#define OP_unalign_mem(o)	(OP_unalign_ld(o) | OP_unalign_store(o))
#define OP_jump(o)		(TOP_is_jump(OP_code(o)))
#define OP_ijump(o)		(TOP_is_ijump(OP_code(o)))
#define OP_side_effects(o)	(TOP_is_side_effects(OP_code(o)))
#define OP_var_opnds(o)		(TOP_is_var_opnds(OP_code(o)))

/* Is conditional branch OP ? */
#define OP_cond(o)              (TOP_is_branch(OP_code(o)))

/* Is OP a branch ? */
#define  OP_br(o)               (TOP_is_jump(OP_code(o)) || TOP_is_ijump(OP_code(o)) || TOP_is_branch(OP_code(o)))

/* Is control transfer OP ? */
#define OP_xfer(o)              (OP_br(o) || TOP_is_call(OP_code(o)))

/* Is OP a likely branch ? */
#define OP_likely(o)            (TOP_is_likely(OP_code(o)))

/* Is it a simulated OP ? */
#define OP_simulated(o)         (TOP_is_simulated(OP_code(o)))

/* Is it a prefetch OP ? */
#define OP_prefetch(o)          (FALSE)

#define OP_has_predicate(o)     (TOP_is_predicated(OP_code(o)))

/* Is it an iadd OP ?:
 *  -- in oputil.cxx to determine if OP sets offset;
 *  -- in tnutil.cxx to 
 */
#define OP_is_iadd(o)           (FALSE)
#define OP_is_ior(o)            (FALSE)

#define OP_operand_info(o)	(ISA_OPERAND_Info(OP_code(o)))
#define OP_immediate_opnd(o)	(TOP_Immediate_Operand(OP_code(o),NULL))
#define OP_has_immediate(o)	(OP_immediate_opnd(o) >= 0)
#define OP_find_opnd_use(o,u)	(TOP_Find_Operand_Use(OP_code(o),(u)))

#define OP_inst_words(o)        (1)

inline char *ISA_PREDICATE_FORMAT (OP *op) {
  if (OP_jump(op) || OP_ijump(op) || OP_cond(op)) 
    return ("%s!");
  return ("%s?");
}

// ----------------------------------------
// Copy ASM_OP_ANNOT when duplicating an OP
// ----------------------------------------
inline void
Copy_Asm_OP_Annot(OP* new_op, OP* op) 
{
  /*
  if (OP_code(op) == TOP_asm) {
    OP_MAP_Set(OP_Asm_Map, new_op, OP_MAP_Get(OP_Asm_Map, op));
  }
  */
  return;
}

/* -----------------------------------------------------------------------
 *   OP counts for memory analysis (see whirl2ops.cxx)
 * -----------------------------------------------------------------------
 */
inline BOOL CGTARG_OP_is_barrier (OP *op) {
  return ((!OP_memory(op) || OP_no_alias(op)) && !OP_call(op));
}


/* -----------------------------------------------------------------------
 * See be/cg/op.h for interface
 * -----------------------------------------------------------------------
 */
inline BOOL 
OP_same_res(OP *op)
{
  return FALSE;
}

/* -----------------------------------------------------------------------
 * check if an operation that saves all the predicate registers
 * -----------------------------------------------------------------------
 */
inline BOOL
OP_save_predicates(OP *op)
{
  return FALSE;
}

/* -----------------------------------------------------------------------
 * check if an operation that restores all the predicate registers
 * -----------------------------------------------------------------------
 */
inline BOOL
OP_restore_predicates(OP *op)
{
  return FALSE;
}

inline void OP_Change_To_Noop(OP *op)
{
  ;
}

#endif /* op_targ_INCLUDED */
