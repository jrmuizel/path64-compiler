/*
 * Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009 PathScale Inc.  All Rights Reserved.
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


/* ====================================================================
 *
 * CGIR operation structure (OP) and utility routines.
 *
 * Exported types:
 *
 *   OP
 *     A machine-level operation.  The following accessors are provided.
 *     These are lvalues:
 *       SRCPOS OP_srcpos(OP *)	source position
 *
 *     These are rvalues with standard Set functions:
 *       UINT8 OP_unrolling(OP *op)
 *	 void Set_OP_unrolling(OP *op, UINT8 unrolling)
 *         Whether or not a loop body is unrolled, each OP in the loop
 *         body has an associated "unrolling" number.  This number is
 *         zero for the original loop body OPs, and incremented by one
 *         for each unrolled copy created.
 *
 *	 TODO: document rest of these
 *
 *     These are rvalues.  They are set by the BB/OPS Insert/Remove routines
 *     (see utility routines below and in "bb.h").
 *	 TOP OP_code(OP *)	opcode
 *	 BB *OP_bb(OP *)	BB (if any) to which this OP belongs
 *	 OP *OP_next(OP *)	next OP (if any) in OPS/BB
 *	 OP *OP_prev(OP *)	previous OP (if any) in OPS/BB
 *	 UINT16 OP_map_idx(OP*)	unique/compact/stable per-BB index
 *				(won't change unless OP is moved to diff BB)
 *				(undefined if OP_bb(op) == NULL)
 *     Constructors:
 *	 OP *Dup_OP(const OP *op)
 *	    Return a copy of <op>, detached from any BB or OPS.
 *	 OP *Mk_OP(TOP opr, ...)
 *	    Create, and return, an OP with opcode <opr>. The variable
 *	    argument list contains the result TNs, followed immediately
 *	    by the operand TNs. The topcode determines the number of
 *	    result and operand parameters. This routine can only be
 *	    used with topcodes with a fixed number of operands/results.
 *	 OP *Mk_VarOP(TOP opr, INT results, INT opnds,
 *		      struct tn **res_tn, 
 *		      struct tn **opnd_tn)
 *	    Create, and return, an OP with opcode <opr>. The parameters
 *	    <results> and <operands> specify the number of results and
 *	    operands. The results and operands are contained in arrays,
 *	    pointed to by the parameters <res_tn> and <opnd_tn>.
 *	    Mk_VarOP is used for topcodes with a variable number of
 *	    operands/results, but may also be used with fixed topcodes.
 *
 *     Constants:
 *       OP_MAX_FIXED_OPNDS
 *       OP_MAX_FIXED_RESULTS
 *	   The maximum number of fixed operands and results an OP can
 *	   have. Note that OPs with variable numbers of operands, e.g. asm,
 *	   will have at least this number, but can have more.
 *
 *     Accessors:
 *	 TN *OP_result(OP *op, UINT8 i)
 *	 void Set_OP_result(OP *op, UINT8 i, TN *tn)
 *	 TN *OP_opnd(OP *op, UINT8 i)
 *	 void Set_OP_opnd(OP *op, UINT8 i, TN *tn)
 *
 *   OPS
 *     Internal doubly-linked list of OPS.  The following accessors are
 *     provided.  They are all rvalues.  See utility functions below for
 *     OPS_Insert/Remove functions to modify OPS.
 *       OP *OPS_first(OPS *)	first OP (if any) in OPS
 *	 OP *OPS_last(OPS *)	last OP (if any) in OPS
 *     Constructors:
 *	 OPS *OPS_Create(void)
 *	   Return a new empty OPS allocated in the PU pool.
 *     Initializers:
 *	 OPS *OPS_Init(OPS *ops)
 *	   Make <ops> empty and return it.
 *     Constants:
 *	 OPS OPS_EMPTY
 *	   Empty OPS used to initialize automatically allocated OPS.
 *
 *
 * Utility functions:
 *
 *   void OP_Change_Opcode(OP *op, TOP opc)
 *	Change the opcode of an OP. The new opcode must have the
 *	same number of fixed results and operands as the old
 *	opcode, and they must both have the same var_opnds property.
 *	If the client can't guarantee these conditions, then a
 *	new OP should be created.
 *
#ifdef TARG_ST
 *   void OP_Copy_Properties(OP *op, OP *src_op)
 *      Copy all properties from src_op to op.
 *      This function can be used for instance when tranforming an 
 *      operation and if OP_Change_Opcode does not apply.
 *	The properties that are NOT copied are:
 *	- opcode/arguments: OP_code(), OP_variant(), OP_results(), OP_opnds()
 *	- BB related stuffs: OP_bb(), OP_map_idx(), OP_order() : 
 *	- OPS related stuffs: OP_prev(), OP_next()
 *      Creating an op with the same opcode and results/operands as src_op
 *	and calling OP_Copy_Properties creates a strictly equivalent
 *	operation with the expection of BB related properties that
 *	are created only when the op is inserted into a BB.
 *	Note that external mappings are not updated, for instance
 *	Copy_WN_For_Memory_OP must be called explicitly for updating
 *	the OP->WN map of memory operations.
#endif
 *   void OP_Change_To_Noop(OP *op)
 *	Change the specified OP into a generic noop (TOP_noop).
 *
 *   INT64 OP_Ordering(OP *op1, OP *op2)
 *   BOOL OP_Precedes(OP *op1, OP *op2)
 *   BOOL OP_Follows(OP *op1, OP *op2)
 *   INT32 OP_Order(OP *op1, OP *op2)
 *     Requires: OP_bb(op1) == OP_bb(op2) && OP_bb(op1) != NULL
 *     OP_Ordering returns an integer indicating the relative order
 *     of <op1> and <op2> in the BB.  The value returned is:
 *	 > 0  iff  <op1> follows <op2>
 *	 = 0  iff  <op1> and <op2> are the same
 *	 < 0  iff  <op2> follows <op1>
 *     The values returned by OP_Ordering may be very sparsely distributed,
 *     and the actual value returned may differ for the same pair of OPs
 *     if any other OP is added anywhere in the BB between the calls to
 *     OP_Ordering.  OP_Follows(op1,op2) is (OP_Ordering(op1,op2) > 0).
 *     OP_Precedes(op1,op2) is (OP_Ordering(op1,op2) < 0).  Provided no OPs
 *     are added to the BB between calls to OP_Ordering, the following
 *     implications hold:
 *	(OP_Ordering(op1,op2) > OP_Ordering(op1,op3)) ==> OP_Follows(op2,op3)
 *	(OP_Ordering(op1,op2) < OP_Ordering(op1,op3)) ==> OP_Precedes(op2,op3)
 *     OP_Order is similar to OP_Ordering, except that a) the return
 *     value is 32 bits, and b) the return value is -1,0,1 (so there
 *     is no significance in the magnatude of the values).
 *
 *   void OPS_Insert_Op(OPS *ops, OP *point, OP *op, BOOL before)
 *     Requires: <point> is in <ops>.  <op> isn't in another OPS.
 *     Insert <op> in <ops>.  If <point> isn't NULL, insert before
 *     <point> if <before> or after it otherwise.  If <point> is NULL
 *     insert at start of <ops> if <before>, or end of <ops> otherwise.
 *
 *   void OPS_Insert_Ops(OPS *ops, OP *point, OPS *insert_ops, BOOL before)
 *     Requires: <point> is in <ops>.  <insert_ops> isn't in another OPS.
 *     Insert <insert_ops> in <ops>.  If <point> isn't NULL, insert before
 *     <point> if <before> or after it otherwise.  If <point> is NULL
 *     insert at start of <ops> if <before>, or end of <ops> otherwise.
 *
 *   void OPS_Insert_Op_Before(OPS *ops, OP *point, OP *op)
 *   void OPS_Insert_Op_After(OPS *ops, OP *point, OP *op)
 *   void OPS_Append_Op(OPS *ops, OP *op)
 *   void OPS_Prepend_Op(OPS *ops, OP *op)
 *   void OPS_Insert_Ops_Before(OPS *ops, OP *point, OPS *insert_ops)
 *   void OPS_Insert_Ops_After(OPS *ops, OP *point, OPS *insert_ops)
 *   void OPS_Append_Ops(OPS *ops, OPS *insert_ops)
 *   void OPS_Prepend_Ops(OPS *ops, OPS *insert_ops)
 *     More-specific names for the general Insert routines.  These do
 *     what you think they should do.
 *
 *   void OPS_Remove_Op(OPS *ops, OP *op)
 *     Requires: <op> is in <ops>.
 *     Remove <op> from <ops>.
 *
 *   void OPS_Remove_Ops(OPS *ops, OP *remove_ops)
 *     Requires: <remove_ops> is in <ops>.
 *     Remove <remove_ops> from <ops>.
 *
 *   void OPS_Remove_All(OPS *ops)
 *     Make <ops> empty.
 *
 *   UINT32 OPS_length(OPS *ops)
 *     Returns the number of OPs in <ops>.
 *
 *   void Print_OP(OP *op)
 *   void Print_OPs(OP *first)
 *   void Print_OPS(OPS *ops)
 *     Print OP(s) to the trace file.  Print_OP prints only <op>;
 *     Print_OPs prints the list of OPs starting with <first>;
 *     Print_OPS prints the OPs in <ops>.
 *
 *   void Print_OP_No_SrcLine(OP *op)
 *   void Print_OPs_No_SrcLines(OP *first)
 *   void Print_OPS_No_SrcLines(OPS *ops)
 *     Same as above, but don't print lines from source code with the OP(s).
 *
 *   BOOL OP_Is_Float_Mem(const OP *op)
 *     Return a boolean that indicates whether <op> is a floating point
 *     memory operation, i.e. a floating point load or store.
 *
#ifdef TARG_ST
 *   INT OP_same_res(const OP *op, INT i)
 *     Returns index of the operand that must be same register as result <i>
 *     if any. Otherwise return -1.
#else
 *   BOOL OP_same_res(OP *op)
 *     OPER_same_res(OP_code(op)) indicates that an OP *may* have an
 *     operand that needs to be the same register as the result. This
 *     function checks a specific instance.  (E.g., the initial unaligned
 *     load instruction in an unaligned load pair does NOT need to have
 *     its last operand be the same as the result.)
 *     OP_same_res is target-specific; it's defined in TARGET/op_targ.h
#endif
 *
 *   BOOL OP_Defs_TN(const OP *op, const struct tn *res)
 *   BOOL OP_Refs_TN(const OP *op, const struct tn *opnd)
 *     Determine if the OP defines/references the given TN.
 *
 *   BOOL OP_Defs_Reg(const OP *op, ISA_REGISTER_CLASS rclass, REGISTER reg)
 *   BOOL OP_Refs_Reg(const OP *op, ISA_REGISTER_CLASS rclass, REGISTER reg)
 *     Determine if the OP defines/references the given register.
 *
#ifdef TARG_ST
 *   INT OP_Defs_Regs(const OP *op, ISA_REGISTER_CLASS rclass, REGISTER reg,
 *                    INT nregs)
 *   INT OP_Refs_Regs(const OP *op, ISA_REGISTER_CLASS rclass, REGISTER reg,
 *                    INT nregs)
 *     Determine the number of registers in [ REG : REG + NREGS - 1 ] that
 *     the OP defines/references.
 *
#endif
 *   void OP_Base_Offset_TNs(OP *memop, TN **base_tn, TN **offset_tn)
 *     A generic utility routine which returns the <base_tn> and the
 *     <offset_tn> for the given <memory_op>.
 *
 *   const INT OP_COPY_OPND
 *	For OPs flagged with OP_copy, OP_COPY_OPND gives the operand
 *	number the source operand.
 *
 *   const INT OP_PREDICATE_OPND
 *	For OPs flagged with OP_has_predicate, OP_PREDICATE_OPND gives the
 *	operand number of the predicate operand.
 *
 *   BOOL OP_cond_def(OP *)
 *     OP_cond_def is an attribute of the OP including its operands.
 * 
 *     OP_cond_def can be solely derived from the def OP and its uses.
 *     For example, OP_cond_def is position independent.  It does not change
 *     if the OP is moved into the delay slot of a branch-likely on MIPS.
 *     The higher level routines are responsible to model nullify-able delay 
 *     slots.
 *
 *     OP_cond_def is not set for OPi defining TNj for 0 <= j < OP_opnds(OPi)
 *     if there is no OPk reachable from OPi referencing a previous defined 
 *     value of any TNj.  It is set otherwise.
 *
 *   BOOL OP_has_implicit_interactions
 *     A generic placeholder routine where all OPs with implicit interactions
 *     with other OPs in an non-obvious way are placed.
 *
 *   BOOL TN_Pair_In_OP (OP* op, TN *tn_res, TN* tn_opnd)
 *      Returns TRUE if <op> contains <tn_res> as a result
 *      and <tn_opnd> as an operand.
 *
 *   INT TN_Resnum_In_OP (OP* op, TN *tn)
 *      This routine assumes that the given <tn> is a result in <op>.
 *      It returns an INT that is the value of the result-number for
 *      this <tn> in this <op>.
 *
 *   INT TN_Opernum_In_OP (OP* op, TN *tn)
 *      This routine assumes that the given <tn> is an operand in <op>.
 *      It returns an INT that is the value of the operand-number for
 *      this <tn> in this <op>.
 *
 * TODO: Complete this interface description.
 *
 *
 * ====================================================================
 * ==================================================================== */


#ifndef op_INCLUDED
#define op_INCLUDED
/* Include the values for the variant field: */
#include "variants.h"

/* to get the definition of SRCPOS. */
#include "srcpos.h"

#include "targ_isa_operands.h"
#include "targ_isa_properties.h"
#include "targ_isa_hazards.h"
#include "targ_isa_pack.h"
#include "targ_isa_bundle.h"
/* to get Is_True */
#include "errors.h"
 
 /* to get definition of TN */
#include "tn.h"

/* to get definition of REGISTER */
#include "register.h"

/* to get TYPE_MEM_POOL_ALLOC */
#include "mempool.h"

#include "tracing.h"
 #ifdef TARG_ST
#include "targ_isa_variants.h"
#endif


/* Declare some structures from elsewhere: */
struct tn;
struct bb;

/* ---------------------------------------------------------------------
 *			       OP stuff
 * ---------------------------------------------------------------------
 */

/* Define the maximum number of fixed operands and results an OP can
 * have. Note that OPs with variable numbers of operands, e.g. asm,
 * may have more operands and/or results.
 */
#define OP_MAX_FIXED_OPNDS	ISA_OPERAND_max_operands
#define OP_MAX_FIXED_RESULTS	ISA_OPERAND_max_results

/* Define the OP structure: */
typedef struct op OP;
typedef struct op {
  SRCPOS	srcpos;		/* source position for the OP */
  OP		*next;		/* Next OP in BB list */
  OP		*prev;		/* Preceding OP in BB list */
  struct bb	*bb;		/* BB in which this OP lives */
  struct bb	*unroll_bb;	/* BB just after unrolling */
  mUINT32	flags;		/* attributes associated with OP */
  mUINT16  	order;		/* relative order in BB */
  mINT16        variant;        /* Opcode variant */
  mUINT16	map_idx;	/* index used by OP_MAPs; unique in BB */
  mUINT16	orig_idx;	/* index of orig op before unrolling */
  mINT16	scycle;		/* Start cycle */
#ifdef TARG_ST
  mUINT16	flags2;		/* [CL] attributes extension */
  mUINT8	unrolling;	/* which unrolled replication (if any) */
  UINT64	g_map_idx;	/* Global unique id for OP */
  mTOP		opr;		/* Opcode. topcode.h */
  // FdF 20090507: The limit of 255 operands is reached for PHI
  // operations in basic blocks with more than 256 predecessors.
  // FdF 20090514: The limit of 255 results may be reached on parallel
  // copies
  mUINT32	opnds;		/* Number of operands */
  mUINT32	results;	/* Number of results */
#else
  mTOP		opr;		/* Opcode. topcode.h */
  mUINT8	unrolling;	/* which unrolled replication (if any) */
  mUINT8	results;	/* Number of results */
  mUINT8	opnds;		/* Number of operands */
#endif
  #ifdef TARG_ST
  struct tn	*spilled_tn;	/* Spilled TN if OP_spill(). */
#endif
  struct tn     *res_opnd[10];	/* result/operand array (see OP_sizeof for info)
	
                                 * !!! THIS FIELD MUST BE LAST !!!
				 */
} OP;

#ifdef TARG_ST
extern void Verify_Instruction_Simple ( OP *op );
#endif
/* Return the 'sizeof' the OP struct needed for the specified number
 * of operands and results. For the sake of simplicity, the result of 
 * the calculation is not rounded up to the alignment of the struct. 
 * Note that the sizeof the res_opnd field as declared in the struct 
 * is arbitrary and is set to something other than one only for convenience 
 * in printing structs in the debugger (OP_sizeof's result is independent 
 * of the dimension).
 */
#ifdef TARG_ST
// (cbr) add effect supports to operands. Currently only used for false guards.
//  no effects on results until necessary to save space.
typedef int TN_effect;
#define OP_sizeof(nresults, nopnds) \
	((size_t)(offsetof(OP, res_opnd[0]) + ((nresults) + (nopnds)) * sizeof(TN *)) + ((nopnds) * sizeof(TN_effect)))
#else
#define OP_sizeof(nresults, nopnds) \
	((size_t)(offsetof(OP, res_opnd[0]) + ((nresults) + (nopnds)) * sizeof(TN *)))
#endif

/* Both the operand and result TNs are stored in _res_opnds.
 * OP_opnd_offset and OP_result_offset give the offset into the
 * array to the start of the operands and results. 
 * NOTE: the offset is NOT constant for all OPs!!!
 */
#define OP_opnd_offset(o)	(0)
#define OP_result_offset(o)	OP_opnds(o)
#ifdef TARG_ST
// (cbr) add effect supports to operands. Currently used for false guards.
#define OP_effects_offset(o)	(OP_opnds(o)+OP_results(o))
#endif

/* Define the access functions: */
#define OP_srcpos(o)	((o)->srcpos)
#ifdef KEY
#define OP_variant(o)	((o)->variant)
#endif
#define OP_scycle(o)	((o)->scycle)
#define OP_flags(o)	((o)->flags)
#ifdef TARG_ST // [CL] attributes extension
#define OP_flags2(o)	((o)->flags2)
#endif

/* These are rvalues */
#define OP_next(o)	((o)->next+0)
#define OP_prev(o)	((o)->prev+0)
#define OP_map_idx(o)	((o)->map_idx+0)
#define OP_orig_idx(o)	((o)->orig_idx+0)
#define OP_unroll_bb(o)	((o)->unroll_bb+0)
#define OP_unrolling(o)	((o)->unrolling+0)
#define OP_bb(o)	((o)->bb+0)
#define OP_results(o)	((o)->results+0)
#define OP_opnds(o)	((o)->opnds+0)
#define OP_code(o)	((TOP)(o)->opr)
#ifdef TARG_ST
#define OP_result(o,n) \
        (Is_True((n)>=0,("invalid result number")), \
         (struct tn *)(o)->res_opnd[(n)+OP_result_offset(o)])
#define OP_opnd(o,n) \
  (Is_True((n)>=0,("invalid operand number (%d(%s),%d)", OP_code(o), TOP_Name (OP_code(o)),n)), \
   (struct tn *)(o)->res_opnd[(n)+OP_opnd_offset(o)])

#ifdef TARG_ST
// (cbr) add effect supports to operands. Currently used for false guards.
#define OP_effects(o,n)	((TN_effect)(o)->res_opnd[(n)+OP_effects_offset(o)])
#endif

#else
#define OP_result(o,n)	((struct tn *)(o)->res_opnd[(n)+OP_result_offset(o)])
#define OP_opnd(o,n)	((struct tn *)(o)->res_opnd[(n)+OP_opnd_offset(o)])
#endif

/* Mutators: */
#define Set_OP_orig_idx(op,idx) ((op)->orig_idx = (idx))
#define Set_OP_unroll_bb(op,bb) ((op)->unroll_bb = (bb))
#define Set_OP_unrolling(op,u)	((op)->unrolling = (u))
#define Set_OP_result(o,result,tn) \
	((o)->res_opnd[(result) + OP_result_offset(o)] = (tn))
#define Set_OP_opnd(o,opnd,tn) \
	((o)->res_opnd[(opnd) + OP_opnd_offset(o)] = (tn))
// [HK] added opr setting macro to avoid casts to non reference type on lvalue
#define Set_OP_opr(op,c) ((op)->opr = (mTOP)(c))

#ifdef TARG_ST
// (cbr) add effect supports to operands. Currently used for false guards.
#define Set_OP_effects(o,opnd,v) \
	((o)->res_opnd[(opnd) + OP_effects_offset(o)] = \
	 (TN*)((TN_effect)((o)->res_opnd[(opnd) + OP_effects_offset(o)]) | (v)))
#define Reset_OP_effects(o,opnd,v) \
	((o)->res_opnd[(opnd) + OP_effects_offset(o)] = (TN*)((TN_effect)(uintptr_t) (void *)((o)->res_opnd[(opnd) + OP_effects_offset(o)]) & ~(v)))

#ifdef TARG_STxP70
#define EFFECT_PRED_FALSE 0x1
#define Set_OP_Pred_True(o, opnd) (Reset_OP_effects((o), (opnd), EFFECT_PRED_FALSE))
#define Set_OP_Pred_False(o, opnd) (Set_OP_effects((o), (opnd) ,EFFECT_PRED_FALSE))
#define OP_Pred_False(o, opnd) ((opnd) == -1 ? false \
				: OP_effects((o), (opnd)) & EFFECT_PRED_FALSE)
#define Invert_OP_Pred(o, opnd) (OP_Pred_False((o), (opnd)) ? Set_OP_Pred_True((o), (opnd)) \
                                 : Set_OP_Pred_False((o), (opnd)))
#else
#define Set_OP_Pred_True(o, opnd) false
#define Set_OP_Pred_False(o, opnd) false
#define OP_Pred_False(o, opnd) false
#define Invert_OP_Pred(o, opnd) false
#endif /* TARG_STxP70 */

#define OP_PredOnFalse(o) OP_Pred_False(o, OP_find_opnd_use(o, OU_predicate))

BOOL Set_OP_opnd_Immediate_Variant(OP *op, INT idx, TN *tn);

#endif /* TARG_ST */

/*
 * Define the OP cond def mask.
 *
 * OP_ALWAYS_UNC_DEF:
 *   Results are always unconditionally defined, independent of
 *   the value of the control predicate.  No upward exposed use!
 *   All unconditional compare falls into this category.
 *   All local TN def falls into this category.
 *
 * OP_ALWAYS_COND_DEF:
 *   Results are conditionally defined.  Whether the result is modified
 *   cannot not be solely determined by the OP's control predicate.
 *   All parallel compare falls into this category.
 *
 * PREDICATED_DEF:
 *   Results are defined if the control predicate evaluates to TRUE.
 *
 * Please notice that the OP_cond_def_type is sticky.  It does not
 * need to updated by optimizations (such as HB and EBO).  It is
 * initialized when the OP is first created.   [OP created through
 * Mk_OP will have this initialized automatically.  Other means
 * of OP creation have to worry about the initialization]
 *
 * It can be promoted (i.e, from PREDICATED_DEF to ALWAYS_UNC_DEF)
 * when any TN is discovered to be local.  Promotion is optional
 * in the sense that it is required by optimization, but not
 * by correctness.
 *
 */
enum OP_COND_DEF_KIND {
  OP_UNKNOWN_DEF = 0,
  OP_ALWAYS_UNC_DEF = 0x800,      
  OP_ALWAYS_COND_DEF = 0x1000,
  OP_PREDICATED_DEF = 0x1800,
  OP_MASK_COND_DEF = 0x1800,   
};

/* Define the flag masks. */

#define OP_MASK_GLUE	  0x00000001 /* Is OP region "glue"? */
#define OP_MASK_NO_ALIAS  0x00000002 /* Is OP Memop that can't be aliased */
#define OP_MASK_COPY	  0x00000004 /* Is OP a COPY? */
#define OP_MASK_FLAG1	  0x00000008 /* temporary flag for local use */
#define OP_MASK_VOLATILE  0x00000010 /* Is OP a volatile Memop? */
#define OP_MASK_HOISTED   0x00000020 /* Is there a WN map attatched to a hoisted OP */
#define OP_MASK_END_GROUP 0x00000040 /* Is OP end of an instruction group */
#ifdef TARG_ST
#define OP_MASK_SSA_MOVE  0x00000080 /* Is OP a move result of SSA algorithm */
#else
#define OP_MASK_M_UNIT	  0x00000080 /* Is OP assigned to M unit */
#endif
#define OP_MASK_TAIL_CALL 0x00000100 /* Is OP a tail call? */
#define OP_MASK_BUNDLED	  0x00000200 /* Is OP bundled? */
#define OP_MASK_SPECULATIVE  0x00000400 /* Is OP a speculative live-range op? */
/* OP_MAKS_COND_DEF  0x00001800  -- See enum defined above  */
#define OP_MASK_NO_CI_ALIAS  0x00002000 /* no cross-iteration alias */
#define OP_MASK_NO_MOVE_BEFORE_GRA 0x00004000 /* do not move this op before GRA */
#ifdef TARG_ST
/* 
 * Arthur: we need a flag for OP_MASK_SPILL because the way spill OPs
 *         are detected seems wrong. Tagged OPs are OP_MAPed, so
 *         it is easy to detect when there is a tag for an OP without
 *         this flag (perhaps more expensive looking at a map).
 */
#define OP_MASK_SPILL     0x00008000 /* OP is a spill OP */
#else
#define OP_MASK_TAG 	  0x00008000 /* OP has tag */
#endif

#ifdef TARG_ST // [CL] attributes extension
#define OP_MASK_PROLOGUE  0x00000001 /* OP belongs to prologue */
#define OP_MASK_EPILOGUE  0x00000002 /* OP belongs to epilogue */
#define OP_MASK_BLACK_HOLE 0x00000004 /* Store OP may store into a black hole */
// FdF 20041221
#define OP_MASK_PFT_SCHEDULED 0x00000008 /* PFT OP was scheduled in pre-pass. */
#define OP_MASK_PFT_BEFORE    0x00000010 /* PFT OP is placed before a memop
				     for prefetch in the current
				     iteration. */
#define OP_MASK_NOP2GOTO   0x00000020 /* A goto operation replacing two
				     NOPs, for scheduling/bundling. */
// BD3 20073008
#define OP_MASK_PRELOAD    0x00000040 /* A load that must be scheduled at L1 miss latency */

// FdF 20071023
#define OP_MASK_PACKED     0x00000080 /* This is is a compiler generated packed operation. */

// VCDV 20080401 codex #41195
#define OP_MASK_MULTI_EXIT_LOOP   0x00000100 /* this operation
                                              correspond to a
                                              multi-exit loop */
// FdF 20080320
#define OP_MASK_SAMERES	   0x00000200 /* This op has one or more SameRes constraint. */

/* on STxP70, add/sub intructions are not "pure. These instructions
   write the carry flag. To avoid a preformance regression, this flag
   can be used to mark add/sub instructions that do not generate a
   usefull carry (meaning a carry value that is always ignored)
 */
#define OP_MASK_CARRY_IS_IGNORED    0x00000400 /* carry result can be ignored for
                                              this op */

// FdF 20090605
#define OP_MASK_UNIQRES   0x00000800 /* This op has one or more Conflict constraint. */

#endif

#define OP_MASK_SPADJ_PLUS  0x00010000 /* Is OP de-alloca spadjust (plus)? */
#define OP_MASK_SPADJ_MINUS 0x00020000 /* Is OP alloca spadjust (minus)? */
#ifdef KEY
/* Is this OP the first OP following the PRAGMA_PREAMBLE_END ? */
#define OP_MASK_FIRST_OP_AFTER_PREAMBLE_END 0x00040000
#define OP_MASK_PREFETCH_DELETED 0x00080000 /* prefetch deleted by scheduler */
#endif
#ifdef TARG_X8664
#define OP_MASK_MEMORY_HI     0x00100000 /* Is OP load/store the high 32-bit? */
#define OP_MASK_COMPUTES_GOT  0x00200000 /* Does OP compute GOT ? */
#endif


# define OP_glue(o)		(OP_flags(o) & OP_MASK_GLUE)
# define Set_OP_glue(o)		(OP_flags(o) |= OP_MASK_GLUE)
# define Reset_OP_glue(o)	(OP_flags(o) &= ~OP_MASK_GLUE)
# define OP_no_alias(o)		(OP_flags(o) & OP_MASK_NO_ALIAS)
# define Set_OP_no_alias(o)	(OP_flags(o) |= OP_MASK_NO_ALIAS)
# define Reset_OP_no_alias(o)	(OP_flags(o) &= ~OP_MASK_NO_ALIAS)
# define OP_copy(o)		(OP_flags(o) & OP_MASK_COPY)
# define Set_OP_copy(o)		(OP_flags(o) |= OP_MASK_COPY)
# define Reset_OP_copy(o)	(OP_flags(o) &= ~OP_MASK_COPY)
# define OP_flag1(o)		(OP_flags(o) & OP_MASK_FLAG1)
# define Set_OP_flag1(o)	(OP_flags(o) |= OP_MASK_FLAG1)
# define Reset_OP_flag1(o)	(OP_flags(o) &= ~OP_MASK_FLAG1)
# define OP_volatile(o)		(OP_flags(o) & OP_MASK_VOLATILE)
# define Set_OP_volatile(o)	(OP_flags(o) |= OP_MASK_VOLATILE)
# define Reset_OP_volatile(o)	(OP_flags(o) &= ~OP_MASK_VOLATILE)
# define OP_hoisted(o)		(OP_flags(o) & OP_MASK_HOISTED)
# define Set_OP_hoisted(o)	(OP_flags(o) |= OP_MASK_HOISTED)
# define Reset_OP_hoisted(o)	(OP_flags(o) &= ~OP_MASK_HOISTED)
# define OP_end_group(o)	(OP_flags(o) & OP_MASK_END_GROUP)
# define Set_OP_end_group(o)	(OP_flags(o) |= OP_MASK_END_GROUP)
# define Reset_OP_end_group(o)	(OP_flags(o) &= ~OP_MASK_END_GROUP)
#ifdef TARG_ST
# define OP_ssa_move(o)		(OP_flags(o) & OP_MASK_SSA_MOVE)
# define Set_OP_ssa_move(o)	(OP_flags(o) |= OP_MASK_SSA_MOVE)
# define Reset_OP_ssa_move(o)	(OP_flags(o) &= ~OP_MASK_SSA_MOVE)
#else
# define OP_m_unit(o)		(OP_flags(o) & OP_MASK_M_UNIT)
# define Set_OP_m_unit(o)	(OP_flags(o) |= OP_MASK_M_UNIT)
# define Reset_OP_m_unit(o)	(OP_flags(o) &= ~OP_MASK_M_UNIT)
#endif
# define OP_tail_call(o)	(OP_flags(o) & OP_MASK_TAIL_CALL)
# define Set_OP_tail_call(o)	(OP_flags(o) |= OP_MASK_TAIL_CALL)
# define Reset_OP_tail_call(o)	(OP_flags(o) &= ~OP_MASK_TAIL_CALL)
# define OP_bundled(o)		(OP_flags(o) & OP_MASK_BUNDLED)
# define Set_OP_bundled(o)	(OP_flags(o) |= OP_MASK_BUNDLED)
# define Reset_OP_bundled(o)	(OP_flags(o) &= ~OP_MASK_BUNDLED)
# define OP_speculative(o)	(OP_flags(o) & OP_MASK_SPECULATIVE)
# define Set_OP_speculative(o)	(OP_flags(o) |= OP_MASK_SPECULATIVE)
# define Reset_OP_speculative(o)(OP_flags(o) &= ~OP_MASK_SPECULATIVE)
# define OP_cond_def_kind(o)	(OP_flags(o) & OP_MASK_COND_DEF)
# define Set_OP_cond_def_kind(o,k) (OP_flags(o) = (OP_flags(o) & ~OP_MASK_COND_DEF) | k)
# define OP_no_ci_alias(o)	(OP_flags(o) & OP_MASK_NO_CI_ALIAS)
# define Set_OP_no_ci_alias(o)	(OP_flags(o) |= OP_MASK_NO_CI_ALIAS)
# define Reset_OP_no_ci_alias(o) (OP_flags(o) &= ~OP_MASK_NO_CI_ALIAS)
# define OP_no_move_before_gra(o) (OP_flags(o) & OP_MASK_NO_MOVE_BEFORE_GRA)
# define Set_OP_no_move_before_gra(o) (OP_flags(o) |= OP_MASK_NO_MOVE_BEFORE_GRA)
#ifdef TARG_ST
# define OP_spill(o)		(OP_flags(o) & OP_MASK_SPILL)
# define Set_OP_spill(o)	(OP_flags(o) |= OP_MASK_SPILL)
# define Reset_OP_spill(o) 	(OP_flags(o) &= ~OP_MASK_SPILL)
# define OP_spilled_tn(o)		((o)->spilled_tn)
# define Set_OP_spilled_tn(o,tn)	((o)->spilled_tn = (tn))
# define Reset_OP_spilled_tn(o,tn) ((o)->spilled_tn = NULL)
#else

# define OP_has_tag(o)		(OP_flags(o) & OP_MASK_TAG)
# define Set_OP_has_tag(o)	(OP_flags(o) |= OP_MASK_TAG)
# define Reset_OP_has_tag(o) 	(OP_flags(o) &= ~OP_MASK_TAG)
#endif
# define OP_spadjust_plus(o)	(OP_flags(o) & OP_MASK_SPADJ_PLUS)
# define Set_OP_spadjust_plus(o)(OP_flags(o) |= OP_MASK_SPADJ_PLUS)
# define Reset_OP_spadjust_plus(o) (OP_flags(o) &= ~OP_MASK_SPADJ_PLUS)
# define OP_spadjust_minus(o)	(OP_flags(o) & OP_MASK_SPADJ_MINUS)
# define Set_OP_spadjust_minus(o) (OP_flags(o) |= OP_MASK_SPADJ_MINUS)
# define Reset_OP_spadjust_minus(o) (OP_flags(o) &= ~OP_MASK_SPADJ_MINUS)
#ifdef TARG_X8664
# define OP_memory_hi(o)	(OP_flags(o) & OP_MASK_MEMORY_HI)
# define Set_OP_memory_hi(o)	(OP_flags(o) |= OP_MASK_MEMORY_HI)
# define Reset_OP_memory_hi(o)	(OP_flags(o) &= ~OP_MASK_MEMORY_HI)
# define OP_computes_got(o)       (OP_flags(o) & OP_MASK_COMPUTES_GOT)
# define Set_OP_computes_got(o)   (OP_flags(o) |= OP_MASK_COMPUTES_GOT)
#endif // TARG_X8664

#ifdef KEY
# define OP_first_after_preamble_end(o) (OP_flags(o) & OP_MASK_FIRST_OP_AFTER_PREAMBLE_END)
# define Set_OP_first_after_preamble_end(o) (OP_flags(o) |= OP_MASK_FIRST_OP_AFTER_PREAMBLE_END)
# define Reset_OP_first_after_preamble_end(o) (OP_flags(o) &= ~OP_MASK_FIRST_OP_AFTER_PREAMBLE_END)

# define OP_prefetch_deleted(o) (OP_flags(o) & OP_MASK_PREFETCH_DELETED)
# define Set_OP_prefetch_deleted(o) (OP_flags(o) |= OP_MASK_PREFETCH_DELETED)
# define Reset_OP_prefetch_deleted(o) (OP_flags(o) &= ~OP_MASK_PREFETCH_DELETED)
#endif // KEY

#ifdef TARG_ST
// [CL] attributes extension to track prologue/epilogue (used for debug)
# define OP_prologue(o)		(OP_flags2(o) & OP_MASK_PROLOGUE)
# define Set_OP_prologue(o)	(OP_flags2(o) |= OP_MASK_PROLOGUE)
# define Reset_OP_prologue(o)	(OP_flags2(o) &= ~OP_MASK_PROLOGUE)
# define OP_epilogue(o)		(OP_flags2(o) & OP_MASK_EPILOGUE)
# define Set_OP_epilogue(o)	(OP_flags2(o) |= OP_MASK_EPILOGUE)
# define Reset_OP_epilogue(o)	(OP_flags2(o) &= ~OP_MASK_EPILOGUE)
# define OP_black_hole(o)	(OP_flags2(o) & OP_MASK_BLACK_HOLE)
# define Set_OP_black_hole(o)	(OP_flags2(o) |= OP_MASK_BLACK_HOLE)
# define Reset_OP_black_hole(o)	(OP_flags2(o) &= ~OP_MASK_BLACK_HOLE)
// FdF
# define OP_pft_scheduled(o)	(OP_flags2(o) & OP_MASK_PFT_SCHEDULED)
# define Set_OP_pft_scheduled(o)	(OP_flags2(o) |= OP_MASK_PFT_SCHEDULED)
# define Reset_OP_pft_scheduled(o)	(OP_flags2(o) &= ~OP_MASK_PFT_SCHEDULED)
# define OP_pft_before(o)	(OP_flags2(o) & OP_MASK_PFT_BEFORE)
# define Set_OP_pft_before(o)	(OP_flags2(o) |= OP_MASK_PFT_BEFORE)
# define Reset_OP_pft_before(o)	(OP_flags2(o) &= ~OP_MASK_PFT_BEFORE)
# define OP_nop2goto(o)		(OP_flags2(o) & OP_MASK_NOP2GOTO)
# define Set_OP_nop2goto(o)	(OP_flags2(o) |= OP_MASK_NOP2GOTO)
# define Reset_OP_nop2goto(o)	(OP_flags2(o) &= ~OP_MASK_NOP2GOTO)
# define OP_preload(o)		(OP_flags2(o) & OP_MASK_PRELOAD)
# define Set_OP_preload(o)	(OP_flags2(o) |= OP_MASK_PRELOAD)
# define Reset_OP_preload(o)	(OP_flags2(o) &= ~OP_MASK_PRELOAD)
# define OP_packed(o)		(OP_flags2(o) & OP_MASK_PACKED)
# define Set_OP_packed(o)	(OP_flags2(o) |= OP_MASK_PACKED)
# define Reset_OP_packed(o)	(OP_flags2(o) &= ~OP_MASK_PACKED)
# define OP_multi_exit_loop(o)		(OP_flags2(o) & OP_MASK_MULTI_EXIT_LOOP)
# define Set_OP_multi_exit_loop(o)	(OP_flags2(o) |= OP_MASK_MULTI_EXIT_LOOP)
# define Reset_OP_multi_exit_loop(o)	(OP_flags2(o) &= ~OP_MASK_MULTI_EXIT_LOOP)
# define OP_sameres(o)		(OP_flags2(o) & OP_MASK_SAMERES)
# define Set_OP_sameres(o)	(OP_flags2(o) |= OP_MASK_SAMERES)
# define Reset_OP_sameres(o)	(OP_flags2(o) &= ~OP_MASK_SAMERES)
# define OP_carryisignored(o)		(OP_flags2(o) & OP_MASK_CARRY_IS_IGNORED)
# define Set_OP_carryisignored(o)	(OP_flags2(o) |= OP_MASK_CARRY_IS_IGNORED)
# define Reset_OP_carryisignored(o)	(OP_flags2(o) &= ~OP_MASK_CARRY_IS_IGNORED)
# define OP_uniqres(o)		(OP_flags2(o) & OP_MASK_UNIQRES)
# define Set_OP_uniqres(o)	(OP_flags2(o) |= OP_MASK_UNIQRES)
# define Reset_OP_uniqres(o)	(OP_flags2(o) &= ~OP_MASK_UNIQRES)
#endif

extern BOOL OP_cond_def(const OP*);
extern BOOL OP_has_implicit_interactions(OP*);
/* 
 * If target supports predication, predicate operand is always 0.
 * Otherwise, -1.
 */
#ifdef TARG_ST
/* (cbr) poison. should not use */
/* [JV] Use 'OP_find_opnd_use(op,OU_predicate)' instead */
#define OP_PREDICATE_OPND (abort(), 0)
#endif

#ifdef TARG_ST
/* [SC] Sugar for finding OU_xxxx operands. */
#define OP_findopnd(op,type) ((OP_find_opnd_use((op),type) == (-1)) \
                              ? NULL \
                              : OP_opnd((op), OP_find_opnd_use((op),type)))
#define OP_Opnd1(op)      OP_findopnd(op,OU_opnd1)
#define OP_Opnd2(op)      OP_findopnd(op,OU_opnd2)
#define OP_Condition(op)  OP_findopnd(op,OU_condition)
#define OP_Storeval(op)   OP_findopnd(op,OU_storeval)
#define OP_Base(op)       OP_findopnd(op,OU_base)
#define OP_Offset(op)     OP_findopnd(op,OU_offset)
#define OP_Predicate(op)  OP_findopnd(op,OU_predicate)
#endif
#ifdef TARG_ST
#define OP_automod(o)		(TOP_is_automod(OP_code(o)))
//[CG]: Added semantic queries
#define OP_ishl(o)		(TOP_is_shl(OP_code(o)) && OP_intop(o))
#define OP_ishr(o)		(TOP_is_shr(OP_code(o)) && OP_intop(o))
#define OP_ishru(o)		(TOP_is_shru(OP_code(o)) && OP_intop(o))
#define OP_sext(o)		(TOP_is_sext(OP_code(o)))
#define OP_zext(o)		(TOP_is_zext(OP_code(o)))
#define OP_phi(o)               (OP_code(o) == TOP_phi)
#define OP_psi(o)               (OP_code(o) == TOP_psi)
#define OP_PCOPY(o)		(OP_code(o) == TOP_PCOPY)

//[SC]: Added multi-op queries
#define OP_multi(o)             (TOP_is_multi(OP_code(o)))
#define OP_extract(o)           (TOP_is_extract(OP_code(o)))
#define OP_compose(o)           (TOP_is_compose(OP_code(o)))
#define OP_widemove(o)          (TOP_is_widemove(OP_code(o)))
#endif

/* Convenience access macros for properties of the OP */
/* TODO: define all the macros for OP properties. */
#define OP_noop(o)		(TOP_is_noop(OP_code(o)))
#define OP_load(o)		(TOP_is_load(OP_code(o)))
#define OP_store(o)		(TOP_is_store(OP_code(o)))
#define OP_prefetch(o)		(TOP_is_prefetch(OP_code(o)))
#ifdef TARG_X8664
#define OP_load_exe(o)		(TOP_is_load_exe(OP_code(o)))
#define OP_load_exe_store(o)	(TOP_is_load_exe_store(OP_code(o)))
#endif
#define OP_memory(o)		(OP_load(o) | OP_store(o) | OP_prefetch(o))
#define OP_mem_fill_type(o)     (TOP_is_mem_fill_type(OP_code(o)))
#define OP_call(o)		(TOP_is_call(OP_code(o)))
#define OP_xfer(o)		(TOP_is_xfer(OP_code(o)))
#define OP_cond(o)		(TOP_is_cond(OP_code(o)))
#define OP_likely(o)		(TOP_is_likely(OP_code(o)))
#define OP_dummy(o)		(TOP_is_dummy(OP_code(o)))
#define OP_flop(o)		(TOP_is_flop(OP_code(o)))
#define OP_fadd(o)		(TOP_is_fadd(OP_code(o)))
#define OP_fdiv(o)		(TOP_is_fdiv(OP_code(o)))
#define OP_fmul(o)		(TOP_is_fmul(OP_code(o)))
#define OP_fmisc(o)		(TOP_is_fmisc(OP_code(o)))
#define OP_fsub(o)		(TOP_is_fsub(OP_code(o)))
#define OP_fcmp(o)              (TOP_is_cmp(OP_code(o)) && OP_flop(o))
#define OP_intop(o)             (TOP_is_intop(OP_code(o)))
#define OP_ptrop(o)             (TOP_is_ptrop(OP_code(o)))
#define OP_flop(o)		(TOP_is_flop(OP_code(o)))

#ifdef TARG_ST
#define OP_iadd(o)		(TOP_is_add(OP_code(o)) && OP_intop(o))
#define OP_ior(o)		(TOP_is_or(OP_code(o)) && OP_intop(o))
#define OP_ixor(o)		(TOP_is_xor(OP_code(o)) && OP_intop(o))
#define OP_iand(o)		(TOP_is_and(OP_code(o)) && OP_intop(o))
#define OP_icmp(o)		(TOP_is_cmp(OP_code(o)) && OP_intop(o))
#define OP_idiv(o)		(TOP_is_div(OP_code(o)) && OP_intop(o))
#define OP_imul(o)		(TOP_is_mul(OP_code(o)) && OP_intop(o))
#define OP_isub(o)		(TOP_is_sub(OP_code(o)) && OP_intop(o))
#define OP_imin(o)		(TOP_is_min(OP_code(o)) && OP_intop(o))
#define OP_imax(o)		(TOP_is_max(OP_code(o)) && OP_intop(o))
#else
#define OP_iadd(o)		(TOP_is_iadd(OP_code(o)))
#define OP_ior(o)		(TOP_is_ior(OP_code(o)))
#define OP_ixor(o)		(TOP_is_ixor(OP_code(o)))
#define OP_iand(o)		(TOP_is_iand(OP_code(o)))
#define OP_icmp(o)		(TOP_is_icmp(OP_code(o)))
#define OP_idiv(o)		(TOP_is_idiv(OP_code(o)))
#define OP_imul(o)		(TOP_is_imul(OP_code(o)))
#define OP_isub(o)		(TOP_is_isub(OP_code(o)))
#endif
#define OP_madd(o)		(TOP_is_madd(OP_code(o)))
#define OP_sqrt(o)		(TOP_is_sqrt(OP_code(o)))
#define OP_mmmul(o)		(TOP_is_mmmul(OP_code(o)))
#define OP_mmshf(o)		(TOP_is_mmshf(OP_code(o)))
#ifdef TARG_ST
#define OP_move(o)		(TOP_is_move(OP_code(o)))
#endif
#define OP_mmalu(o)		(TOP_is_mmalu(OP_code(o)))
#define OP_select(o)		(TOP_is_select(OP_code(o)))
#define OP_cond_move(o)		(TOP_is_cond_move(OP_code(o)))
#define OP_operand_info(o)	(ISA_OPERAND_Info(OP_code(o)))
#define OP_dismissible(o)	(TOP_is_dismissible(OP_code(o)))
#ifdef TARG_ST
/* Result must not be same as operand */ 
inline BOOL OP_uniq_res(OP *op, INT i) { 
  const ISA_OPERAND_INFO *oinfo = OP_operand_info(op);
  if (i >= ISA_OPERAND_INFO_Results(oinfo)) return FALSE;
  ISA_OPERAND_USE this_def = ISA_OPERAND_INFO_Def(oinfo, i); 
  if (this_def & OU_uniq_res)  
    return TRUE; 
 
#ifdef TARG_ST
  if(ISA_OPERAND_INFO_Conflicts(oinfo, i)) {
    return TRUE;
  }
#endif

  return FALSE; 
} 
#else
#define OP_uniq_res(o)		(TOP_is_uniq_res(OP_code(o)))
#endif
#ifdef TARG_ST
/* Check if a given result and operand must not share the same
   register. */
inline BOOL OP_conflict(OP *op, INT res_idx, INT opnd_idx) { 
  const ISA_OPERAND_INFO *oinfo = OP_operand_info(op);
  if (res_idx >= ISA_OPERAND_INFO_Results(oinfo) ||
      opnd_idx >= ISA_OPERAND_INFO_Operands(oinfo))
    return FALSE;
  ISA_OPERAND_USE this_def = ISA_OPERAND_INFO_Def(oinfo, res_idx);
  // This result must not share a register with any operand
  if (this_def & OU_uniq_res)  
    return TRUE;
 
  if(ISA_OPERAND_INFO_Conflicts(oinfo, res_idx) &&
     ISA_OPERAND_INFO_Has_Conflict(oinfo, res_idx, opnd_idx))
    return TRUE;

  return FALSE; 
}
#endif
#ifdef TARG_ST
#define OP_unalign_ld(o)	(TOP_is_load(OP_code(o)) && TOP_is_unalign(OP_code(o)))
#define OP_unalign_store(o)	(TOP_is_store(OP_code(o)) && TOP_is_unalign(OP_code(o)))
#else
#define OP_unalign_ld(o)	(TOP_is_unalign_ld(OP_code(o)))
#define OP_unalign_store(o)	(TOP_is_unalign_store(OP_code(o)))
#endif
#define OP_unsigned_ld(o)	(TOP_is_load(OP_code(o)) && TOP_is_unsign(OP_code(o)))
#define OP_unalign_mem(o)	(OP_unalign_ld(o) | OP_unalign_store(o))
#define OP_defs_fcc(o)		(TOP_is_defs_fcc(OP_code(o)))
#define OP_defs_fcr(o)		(TOP_is_defs_fcr(OP_code(o)))
#define OP_defs_fpu_int(o)	(TOP_is_defs_fpu_int(OP_code(o)))
#define OP_defs_fp(o)		(TOP_is_defs_fp(OP_code(o)))
#define OP_refs_fcr(o)		(TOP_is_refs_fcr(OP_code(o)))
#define OP_unsafe(o)            (TOP_is_unsafe(OP_code(o)))
#define OP_jump(o)		(TOP_is_jump(OP_code(o)))
#define OP_ijump(o)		(TOP_is_ijump(OP_code(o)))
#define OP_f_group(o)           (TOP_is_f_group(OP_code(o)))
#define OP_l_group(o)           (TOP_is_l_group(OP_code(o)))
#define OP_privileged(o)        (TOP_is_privileged(OP_code(o)))
#define OP_simulated(o)		(TOP_is_simulated(OP_code(o)))
#ifdef TARG_ST
#define TOP_is_predicated(t)    (TOP_is_guard_t(t) || TOP_is_guard_f(t))
#define OP_is_predicated(o)     (TOP_is_predicated(OP_code(o)))
#endif
#define OP_is_guard_t(o)        (TOP_is_guard_t(OP_code(o)))
#define OP_is_guard_f(o)         (OP_is_guard_t(o) && \
                                 OP_Pred_False(o, OP_find_opnd_use(o, OU_predicate)))

#define OP_has_predicate(o)	(TOP_is_predicated(OP_code(o)))
#define OP_access_reg_bank(o)	(TOP_is_access_reg_bank(OP_code(o)))
#define OP_side_effects(o)	(TOP_is_side_effects(OP_code(o)))
#define OP_branch_predict(o)	(TOP_is_branch_predict(OP_code(o)))
#define OP_var_opnds(o)		(TOP_is_var_opnds(OP_code(o)))
#define OP_uncond(o)            (OP_xfer(o) && !OP_cond(o))

#define OP_operand_info(o)	(ISA_OPERAND_Info(OP_code(o)))
#define OP_has_hazard(o)	(ISA_HAZARD_TOP_Has_Hazard(OP_code(o)))
#define OP_immediate_opnd(o,lc)	(TOP_Immediate_Operand(OP_code(o), lc))
#define OP_has_immediate(o)	(OP_immediate_opnd(o, NULL) >= 0)
#define OP_inst_words(o)	(ISA_PACK_Inst_Words(OP_code(o)))
#ifdef TARG_ST
#define OP_unit_slots(o)	(ISA_EXEC_Unit_Slots(OP_code(o)))
#endif
#define OP_find_opnd_use(o,u)	(TOP_Find_Operand_Use(OP_code(o),(u)))
#define OP_has_result(o)        (OP_results(o) != 0)

#ifdef TARG_X8664
#define OP_x86_style(o)	        (TOP_is_x86_style(OP_code(o)))
#define OP_reads_rflags(o)      (TOP_is_read_rflags(OP_code(o)))
#define OP_change_rflags(o)     (TOP_is_change_rflags(OP_code(o)))
#define OP_x87(o)		(TOP_is_x87(OP_code(o)))
#define OP_mmx(o)		(TOP_is_mmx(OP_code(o)))
#define OP_prefix_lock(o)	(TOP_is_prefix_lock(OP_code(o)))
#endif
#ifdef TARG_MIPS
#define OP_fp_load(o)		(TOP_is_fp_load(OP_code(o)))
#define OP_fp_store(o)		(TOP_is_fp_store(OP_code(o)))
#define OP_fp_move(o)		(TOP_is_fp_move(OP_code(o)))
#ifdef ICE9A_HW_WORKAROUND
#define OP_fp_arith(o)		(TOP_is_fp_arith(OP_code(o)))
#define OP_fp_unpredictable_latency(o) \
				(TOP_is_fp_unpredictable_latency(OP_code(o)))
#define OP_fp_stall_hazard(o)	(TOP_is_fp_stall_hazard(OP_code(o)))
#define OP_fp_round_hazard(o)	(TOP_is_fp_round_hazard(OP_code(o)))
#define OP_fp_round_trigger(o)	(TOP_is_fp_round_trigger(OP_code(o)))
#endif	// ICE9A_HW_WORKAROUND
#endif

#ifdef TARG_ST
/* Returns index of operand that must be same register as result i.
   This has been moved into oputil.cxx as now this handles the
   case of asm statements with same result/opnd constraints.
*/
extern INT OP_same_res(OP *op, INT i);
#endif

inline INT OP_result_size(OP *op, INT result)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Result(oinfo, result);
  return ISA_OPERAND_VALTYP_Size(otype);
}

inline INT OP_opnd_size(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
  return ISA_OPERAND_VALTYP_Size(otype);
}

inline BOOL OP_result_is_reg(OP *op, INT result)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Result(oinfo, result);
  return ISA_OPERAND_VALTYP_Is_Register(otype);
}

inline BOOL OP_opnd_is_reg(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
  return ISA_OPERAND_VALTYP_Is_Register(otype);
}

inline BOOL OP_opnd_is_literal(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
  return ISA_OPERAND_VALTYP_Is_Literal(otype);
}

inline BOOL OP_opnd_is_enum(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
  return ISA_OPERAND_VALTYP_Is_Enum(otype);
}

inline BOOL OP_result_is_signed(OP *op, INT result)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Result(oinfo, result);
  return ISA_OPERAND_VALTYP_Is_Signed(otype);
}

inline BOOL OP_opnd_is_signed(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
  return ISA_OPERAND_VALTYP_Is_Signed(otype);
}

inline BOOL OP_result_is_fpu_int(OP *op, INT result)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Result(oinfo, result);
  return ISA_OPERAND_VALTYP_Is_FPU_Int(otype);
}

inline BOOL OP_opnd_is_fpu_int(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
  return ISA_OPERAND_VALTYP_Is_FPU_Int(otype);
}

inline BOOL OP_opnd_is_pcrel(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
  return ISA_OPERAND_VALTYP_Is_PCRel(otype);
}

inline ISA_LIT_CLASS OP_opnd_lit_class(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
  return ISA_OPERAND_VALTYP_Literal_Class(otype);
}

inline ISA_REGISTER_CLASS OP_opnd_reg_class(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
  return ISA_OPERAND_VALTYP_Register_Class(otype);
}

inline ISA_REGISTER_CLASS OP_result_reg_class(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Result(oinfo, opnd);
  return ISA_OPERAND_VALTYP_Register_Class(otype);
}

inline ISA_REGISTER_SUBCLASS OP_opnd_reg_subclass(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
#ifdef TARG_ST
  if (opnd >= ISA_OPERAND_INFO_Operands(oinfo)) {
    FmtAssert (OP_var_opnds (op) || OP_call(op), ("Invalid operand number (%d) in OP_opnd_reg_subclass", opnd));
    return ISA_REGISTER_SUBCLASS_UNDEFINED;
  }
#endif
  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
#ifdef EFFECT_PRED_FALSE
// (cbr) add effect supports to operands. Currently used for false guards.
  if (OP_Pred_False(op, opnd))
    return ISA_REGISTER_SUBCLASS_gr_false;
#endif
  return ISA_OPERAND_VALTYP_Register_Subclass(otype);
}

inline ISA_REGISTER_SUBCLASS OP_result_reg_subclass(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
#ifdef TARG_ST
  if (opnd >= ISA_OPERAND_INFO_Results(oinfo)) {
    FmtAssert (OP_var_opnds (op) || OP_call(op), ("Invalid operand number (%d) in OP_result_reg_subclass", opnd));
    return ISA_REGISTER_SUBCLASS_UNDEFINED;
  }
#endif

  const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Result(oinfo, opnd);
  return ISA_OPERAND_VALTYP_Register_Subclass(otype);
}

inline ISA_OPERAND_USE OP_opnd_use(OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
#ifdef TARG_ST
  FmtAssert (opnd < ISA_OPERAND_INFO_Operands(oinfo),
	     ("Invalid operand (%d) in OP_opnd_use", opnd));
#endif
  
  return ISA_OPERAND_INFO_Use(oinfo, opnd);
}
extern TOP CGTARG_Noop_Top (ISA_EXEC_UNIT_PROPERTY);
extern void CGTARG_Init_OP_cond_def_kind(OP *);

/*
 * If 'op' performs a copy operation, return the index of
 * the source operand; otherwise return -1.
 */
#ifdef TARG_ST
extern INT OP_Copy_Operand(OP *op);
extern INT OP_Copy_Result(OP *op);

//inline INT OP_Predicate_Operand(OP *op) {
//  return OP_find_opnd_use(op, OU_predicate);
//}
#else
extern INT CGTARG_Copy_Operand(OP *op);
#endif

// Return a boolean indicating if 'op' performs a copy operation.
inline BOOL OP_Is_Copy(OP *op)
{
#ifdef TARG_ST
  return OP_Copy_Operand(op) >= 0;
#else
  return CGTARG_Copy_Operand(op) >= 0;
#endif
}// Return a boolean indicating if 'op' performs a copy operation
// that is a candidate for preferencing.  Does extra consistency
// checks.  This is the preferred method for testing for a copy
// before invoking OP_Copy_Operand() and friends.
inline BOOL OP_Is_Preference_Copy (OP* op) 
{
  if (OP_copy(op)) {
    if (OP_Is_Copy(op)) {
      return TRUE;
    } else {
      //
      // Ops that are to be deleted sometimes have their opcode temporarily
      // set to nop prior to deletion (principally in lra).
      //
      Is_True(OP_code(op) == TOP_noop, 
          ("Op_copy set on non-copy op %s", TOP_Name(OP_code(op))));
      return FALSE;
    }
  }
  return FALSE;
}

// Returns the right opcode for simulated TOP which matches 
// ISA_EXEC_UNIT_PROPERTY.
inline TOP OP_Simulated_Top (OP *op, ISA_EXEC_UNIT_PROPERTY unit)
{
  TOP top = OP_code(op);
 
  /* Prune the obvious cases. */
  if (!TOP_is_simulated(top)) return top;

  /* Placeholder for itemizing specific simulated ops */
  switch (top) {

  case TOP_noop:
    return CGTARG_Noop_Top(unit);

  default:
    // TOP_intrncall, TOP_spadjust, TOP_copy_br, TOP_asm,
    return top;
  }
}



// If 'op' performs a copy operation, return the TN of
// the source operand; otherwise return NULL.
#ifdef TARG_ST
inline TN *OP_Copy_Operand_TN(OP *op)
{
  INT iopnd = OP_Copy_Operand(op);
  return (iopnd < 0) ? NULL : OP_opnd(op,iopnd);
}
inline TN *OP_Copy_Result_TN(OP *op)
{
  INT ires = OP_Copy_Result(op);
  return (ires < 0) ? NULL : OP_result(op,ires);
}
#endif

#ifdef TARG_ST
#define OP_Mem_Ref_Bytes(o)  TOP_Mem_Bytes(OP_code(o))
#else
extern UINT32 CGTARG_Mem_Ref_Bytes(const OP *memop);
#endif


extern BOOL CGTARG_Is_Long_Latency (TOP *opcode);
#ifdef TARG_ST
extern BOOL OP_is_ext_op(OP *op);
extern BOOL OP_Is_Barrier(OP *op);
extern BOOL OP_Is_Counted_Loop(OP* op);
extern BOOL OP_Is_Copy_Immediate_Into_Register(OP *op);
extern BOOL OP_Has_Latency(OP *op);
extern BOOL OP_Is_Speculative_Load(OP* memop);
extern BOOL OP_Is_Check_Load(OP* memop);
extern BOOL OP_Is_Speculative(OP *op);
extern BOOL OP_Can_Be_Speculative (OP *op);
extern BOOL OP_Is_Unconditional_Compare (OP *op);
extern BOOL OP_Performance_Effects (OP *op);
extern BOOL OP_Safe_Effects (OP *op);
#endif

// Copy ASM_OP_ANNOT when duplicating an OP.
#ifdef TARG_ST
void Copy_Asm_OP_Annot(OP* new_op, const OP* op);
#else
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
#endif
// check if an operation that saves all the predicate registers
extern BOOL OP_save_predicates(OP *op);

// check if an operation that restores all the predicate registers
extern BOOL OP_restore_predicates(OP *op);

#define OP_has_result(o) (OP_results(o) != 0)
#ifdef TARG_ST // [CL]
#define OP_find_result_with_usage(o,u)	(TOP_Find_Result_With_Usage(OP_code(o),(u)))
#endif
/* Is OP a branch ? */
#define  OP_br(o) (OP_xfer(o) && !OP_call(o))

/* _fixed_results and _fixed_opnds return how many fixed
 * results/operands an instruction has (OP_result/OP_opnds includes
 * any variable operands in the count).
 */
#define TOP_fixed_results(o)	(ISA_OPERAND_INFO_Results(ISA_OPERAND_Info(o)))
#define TOP_fixed_opnds(o)	(ISA_OPERAND_INFO_Operands(ISA_OPERAND_Info(o)))
#define OP_fixed_results(o)	(TOP_fixed_results(OP_code(o)))
#define OP_fixed_opnds(o)	(TOP_fixed_opnds(OP_code(o)))
#ifdef TARG_ST
/*
 * Returns whether the operations are plain load or store.
 * A plain load or store is defined by:
 * - is OP_load() or OP_store(),
 * - no implicit interaction,
 * - no partial definition,
 * - not predicated, or statically proved not conditional.
 */
extern BOOL OP_plain_load(OP *op);
extern BOOL OP_plain_store(OP *op);
#endif

#ifdef TARG_ST
/* ====================================================================
 *
 * OP_Find_TN_Def_In_BB - Return the definer op of <tn> within a BB scope
 * by walking backward starting from <op>. BB is implicitly defined as
 * being <op> container.
 * Return NULL if not found.
 *
 * ====================================================================
 */
extern OP * OP_Find_TN_Def_In_BB(const OP *op, TN *tn);

/* ====================================================================
 *
 * OP_opnd_is_multi - return TRUE if opnd is part of a multi-register
 * operand.
 *
 * ====================================================================
 */
extern BOOL OP_opnd_is_multi(const OP *op, INT opnd);

/* ====================================================================
 *
 * OP_result_is_multi - return TRUE if result is part of a multi-register
 * result.
 *
 * ====================================================================
 */
extern BOOL OP_result_is_multi(const OP *op, INT result);

// OP is associative
extern BOOL OP_is_associative(OP *op);

// The other opnd involved in reassociation
extern INT OP_other_opnd(OP *op, INT this_opnd);

// Give the opposite form, e.g,  - => +,  + => -.
// TODO: move to targ_info
extern TOP TOP_opposite (TOP top);

// Give immediate form.
// TODO: move to targ_info
extern TOP TOP_immediate(TOP top);

// Return the nonindexed OP code corresponding to a indexed memory
// TOP or TOP_UNDEFINED if code is not an indexed memory OP.
// TODO: move to targ_info
extern TOP TOP_equiv_nonindex_memory (TOP top);

// If given operand can be associated
extern BOOL OP_opnd_can_be_reassociated (OP *op, INT opnd);

/*
 * If a load/store OP autoincrements/decrements its base TN, by
 * convention that's the following one
 * NOTE: moved here from cg_swp_target.h
 */
inline INT TOP_base_update_tn(TOP top)
{
  if (TOP_is_load(top)) {
    if (ISA_OPERAND_INFO_Results(ISA_OPERAND_Info(top)) == 2)
      return 1;
  } else if (TOP_is_store(top)) {
    if (ISA_OPERAND_INFO_Results(ISA_OPERAND_Info(top)) == 1)
      return 0;
  }
  return -1;
}

inline TN *OP_base_update_tn(OP *op)
{
  INT opnd = TOP_base_update_tn(OP_code(op));
  if (opnd >= 0)
    return OP_opnd(op,opnd);
  return NULL;
}

extern TOP TOP_opnd_register_variant(TOP top, int opnd, ISA_REGISTER_CLASS regclass);
extern TOP TOP_opnd_immediate_variant(TOP top, int opnd, INT64 imm);
extern TOP TOP_Combine_Opcodes( TOP opcode1, TOP opcode2, INT opnd_idx, BOOL *opnd_swapped );
extern TOP TOP_result_register_variant(TOP top, int rslt, ISA_REGISTER_CLASS regclass);
extern INT TOP_opnd_use_bits(TOP top, int opnd);
extern BOOL TOP_opnd_use_signed(TOP top, int opnd);

extern BOOL TOP_opnd_value_in_range (TOP top, int opnd, INT64 imm);
extern TOP TOP_AM_automod_variant(TOP top, BOOL post_mod, BOOL inc_mod, ISA_REGISTER_CLASS regclass);
extern BOOL TOP_evaluate_top( OP *op, INT64 *opnd_values, INT64 *result_val, int result_idx );

extern OP *OP_get_unconditional_variant( OP *op );
extern BOOL OP_condition_is_true( OP *op, INT64 pred_val );

extern VARIANT TOP_cond_variant(TOP top);
#ifdef TARG_ST
extern VARIANT OP_cmp_variant(const OP* op);
extern VARIANT OP_br_variant(const OP* op);
extern TOP OP_opnd_swapped_variant(OP *op, int opnd1, int opnd2);
#else
extern VARIANT TOP_cmp_variant(TOP top);
extern TOP TOP_opnd_swapped_variant(TOP top, int opnd1, int opnd2);
#endif

#endif


#define FOR_ALL_OP_RESULTs_FWD(op, res) \
  for (res = 0; res < OP_results (op); ++res)

#define FOR_ALL_OP_RESULTs_REV(op, res) \
  for (res = OP_results (op) - 1; res >= 0; --res)

#define FOR_ALL_OP_RESULTs FOR_ALL_OP_RESULTs_FWD

#define FOR_ALL_OP_OPNDs_FWD(op, opr) \
  for (opr = 0; opr < OP_opnds (op); ++opr)

#define FOR_ALL_OP_OPNDs_REV(op, opr) \
  for (opr = OP_opnds (op) - 1; opr >= 0; --opr)

#define FOR_ALL_OP_OPNDs FOR_ALL_OP_OPNDs_FWD
/* ---------------------------------------------------------------------
 *			       OPS stuff
 * ---------------------------------------------------------------------
 */

typedef struct ops {
  struct op *first;
  struct op *last;
  UINT16 length;
} OPS;

#define OPS_first(ops) ((ops)->first+0)
#define OPS_last(ops) ((ops)->last+0)
#define OPS_length(ops) ((ops)->length+0)
#define OPS_EMPTY { NULL, NULL, 0 }

inline OPS *OPS_Create(void)
{
  OPS *result = TYPE_MEM_POOL_ALLOC(OPS, &MEM_pu_pool);
  result->first = result->last = NULL;
  result->length = 0;
  return result;
}

inline void OPS_Remove_All(OPS *ops)
{
  ops->first = ops->last = NULL;
  ops->length = 0;
}
  
inline OPS *OPS_Init(OPS *ops)
{
  ops->first = ops->last = NULL;
  ops->length = 0;
  return ops;
}

inline void OPS_Remove_Op(OPS *ops, OP *op)
{
  OP **prevp = OP_next(op) ? &OP_next(op)->prev : &ops->last;
  OP **nextp = OP_prev(op) ? &OP_prev(op)->next : &ops->first;
  
  *prevp = OP_prev(op);
  *nextp = OP_next(op);

  op->prev = op->next = NULL;
  ops->length--;
}

inline void OPS_Remove_Ops(OPS *ops, OPS *remove_ops)
{
  OP *first = OPS_first(remove_ops);
  OP *last = OPS_last(remove_ops);
  OP **prevp, **nextp;

  if (first == NULL) return;

  FmtAssert(last != NULL,
	    ("remove_ops is malformed (last is NULL but first isn't)"));

  prevp = OP_next(last) ? &OP_next(last)->prev : &ops->last;
  nextp = OP_prev(first) ? &OP_prev(first)->next : &ops->first;
  
  *prevp = OP_prev(first);
  *nextp = OP_next(last);

  first->prev = last->next = NULL;
  ops->length -= OPS_length(remove_ops);
}

void OPS_Insert_Op_Before(OPS *ops, OP *point, OP *op);
void OPS_Insert_Op_After(OPS *ops, OP *point, OP *op);
void OPS_Append_Op(OPS *ops, OP *op);
void OPS_Prepend_Op(OPS *ops, OP *op);
void OPS_Insert_Ops_Before(OPS *ops, OP *point, OPS *insert_ops);
void OPS_Insert_Ops_After(OPS *ops, OP *point, OPS *insert_ops);
void OPS_Append_Ops(OPS *ops, OPS *insert_ops);
void OPS_Prepend_Ops(OPS *ops, OPS *insert_ops);
void OPS_Insert_Op(OPS *ops, OP *point, OP *op, BOOL before);
void OPS_Insert_Ops(OPS *ops, OP *point, OPS *insert_ops, BOOL before);

/* ====================================================================
 *
 * Utility routines which manipulate OPs.
 *
 * ====================================================================
 */

/* Basic OP generators: */
extern OP *Dup_OP (const OP *op);
#ifdef TARG_ST
extern OP *Resize_OP(OP *op, int results, int opnds);
extern void OP_Copy_Properties(OP *op, OP *src_op);
#endif

extern void Free_OP_List ( OP *op );	/* Free OP list's space */

/* OP generation utilities: */
extern OP *Mk_OP (
  TOP opr,		/* operator */
  ...			/* the results and operands */
);

extern OP *Mk_VarOP (
  TOP opr,		/* operator */
  INT results,		/* number of results */
  INT opnds,		/* number of operands */
  struct tn **res_tn,	/* result array */
  struct tn **opnd_tn	/* operand array */
);

/* Build OP and append to ops list: */

inline void Build_OP(TOP opc, OPS *ops)
{
  OPS_Append_Op(ops, Mk_OP(opc));
}

inline void Build_OP(TOP opc, struct tn *t1, OPS *ops)
{
  OPS_Append_Op(ops, Mk_OP(opc, t1));
}

inline void Build_OP(TOP opc, struct tn *t1, struct tn *t2, OPS *ops)
{
  OPS_Append_Op(ops, Mk_OP(opc, t1, t2));
}

inline void Build_OP(TOP opc, struct tn *t1, struct tn *t2, struct tn *t3, 
			      OPS *ops)
{
  OPS_Append_Op(ops, Mk_OP(opc, t1, t2, t3));
}

inline void Build_OP(TOP opc, struct tn *t1, struct tn *t2, struct tn *t3, 
			      struct tn *t4, OPS *ops)
{
  OPS_Append_Op(ops, Mk_OP(opc, t1, t2, t3, t4));
}

inline void Build_OP(TOP opc, struct tn *t1, struct tn *t2, struct tn *t3, 
			      struct tn *t4, struct tn *t5, OPS *ops)
{
  OPS_Append_Op(ops, Mk_OP(opc, t1, t2, t3, t4, t5));
}

inline void Build_OP(TOP opc, struct tn *t1, struct tn *t2, struct tn *t3, 
			      struct tn *t4, struct tn *t5, struct tn *t6, 
			      OPS *ops)
{
  OPS_Append_Op(ops, Mk_OP(opc, t1, t2, t3, t4, t5, t6));
}

inline void Build_OP(TOP opc, struct tn *t1, struct tn *t2, struct tn *t3, 
			      struct tn *t4, struct tn *t5, struct tn *t6, 
			      struct tn *t7, OPS *ops)
{
  OPS_Append_Op(ops, Mk_OP(opc, t1, t2, t3, t4, t5, t6, t7));
}

inline void Build_OP(TOP opc, struct tn *t1, struct tn *t2, struct tn *t3, 
			      struct tn *t4, struct tn *t5, struct tn *t6, 
			      struct tn *t7, struct tn *t8, OPS *ops)
{
  OPS_Append_Op(ops, Mk_OP(opc, t1, t2, t3, t4, t5, t6, t7, t8));
}
inline void Build_OP(TOP opc, struct tn *t1, struct tn *t2, struct tn *t3, 
			      struct tn *t4, struct tn *t5, struct tn *t6, 
			      struct tn *t7, struct tn *t8, struct tn *t9, 
			      OPS *ops)
{
  OPS_Append_Op(ops, Mk_OP(opc, t1, t2, t3, t4, t5, t6, t7, t8, t9));
}
#ifdef TARG_ST
// (cbr) Support for op effects
extern void CGTARG_Predicate_OP(struct bb *bb, OP *op, struct tn *pred_tn, bool on_false);
#else
extern void CGTARG_Predicate_OP(struct bb *bb, OP *op, struct tn *pred_tn);
#endif
/* Determine if the op defines/references the given TN result/operand. */
extern BOOL OP_Defs_TN(const OP *op, const struct tn *res);
extern BOOL OP_Refs_TN(const OP *op, const struct tn *opnd);

/* Determine if the op defines/references the given register. */
extern BOOL OP_Defs_Reg(const OP *op, ISA_REGISTER_CLASS rclass, REGISTER reg);
extern BOOL OP_Refs_Reg(const OP *op, ISA_REGISTER_CLASS rclass, REGISTER reg);
/* Determine the number of registers in [ REG : REG + NREGS - 1 ] that
 * the OP defines/references.
 */
extern INT OP_Defs_Regs(const OP *op, ISA_REGISTER_CLASS rclass, REGISTER reg,
                        INT nregs);
extern INT OP_Refs_Regs(const OP *op, ISA_REGISTER_CLASS rclass, REGISTER reg,
                        INT nregs);

inline void OP_Change_Opcode(OP *op, TOP opc)
{
  Is_True(   OP_fixed_results(op) == TOP_fixed_results(opc)
	  && OP_fixed_opnds(op) == TOP_fixed_opnds(opc),
	  ("different number of results/operands for %s and %s",
	   TOP_Name(OP_code(op)), TOP_Name(opc)));
  Is_True(OP_var_opnds(op) == TOP_is_var_opnds(opc),
	  ("var_opnds property not same for %s and %s",
	   TOP_Name(OP_code(op)), TOP_Name(opc)));

  op->opr = (mTOP)opc;
}

inline void OP_Change_To_Noop(OP *op)
{
  op->opr = (mTOP)TOP_noop;
  op->opnds = 0;
  op->results = 0;
  OP_flags(op) = 0;
  #ifdef TARG_ST // [CL] attributes extension
  OP_flags2(op) = 0;
#endif

}
#ifdef TARG_ST
// Determine which opcode to use to copy TNs based upon whether or
// not the value being copied is floating point, its size in bits,
// and whether the condition to be used is a floating CC or integer
// register.
//
// Arthur: moved over from cgtarg.h
// TODO: what is it really ??
//
extern TOP CGTARG_Which_OP_Select (UINT16 bit_size, BOOL is_float,
				    BOOL is_fcc);

/*
 * Handling of implicit architectural flags (or other resources) 
 * effects that are not described accurately with the architecture description.
 * We do not distinguish different flag effects, but we track operations that
 * have flag effects whatever the flags. This is an inaccuracy.
 * The possible flag effects as:
 * - OP_FE_NONE: no flag access
 * - OP_FE_READ: the operation read an implicit flag,
 * - OP_FE_WRITE: the operation writes a useful flag value,
 * - OP_FE_CLOBBER: the operation clobbers flag values (write useless value) .
 * For instance an operation which is OP_FA_WRITE can't be deleted, while
 * a OP_FE_CLOBBER can be deleted is it does not generate value otherwise.
 * For dependencies between operations, there is a dependence between
 * two operations that access flags if:
 * 1. both operations access the flag (~OP_FE_NONE),
 * 2. at least one is a usefull write (OP_FE_WRITE).
 * Note that an operation may have multiple flag effect at the same time,
 * for instance OP_FE_WRITE|OP_FE_READ.
 * We distinguish such operations from OP_side_effects operations, though
 * we must handle the flag effects in the code generator for avoiding deletion
 * of usefull flag settings.
 */
typedef enum {
  OP_FE_NONE = 0,
  OP_FE_READ = 1,
  OP_FE_WRITE = 2,
  OP_FE_CLOBBER = 4
} OP_Flag_Effect;
typedef int OP_Flag_Effects;

/*
 * Target dependent interface for getting the flag effects of an operation.
 */
extern OP_Flag_Effects CGTARG_OP_Get_Flag_Effects(const OP *op);

#define OP_Get_Flag_Effects(op) CGTARG_OP_Get_Flag_Effects(op)
#define OP_Has_Flag_Effect(op) (CGTARG_OP_Get_Flag_Effects(op) & OP_FE_WRITE)
#endif

inline BOOL OP_Precedes(OP *op1, OP *op2)
{
  return op1->order < op2->order;
}

inline BOOL OP_Follows(OP *op1, OP *op2)
{
  return op1->order > op2->order;
}

inline INT64 OP_Ordering(OP *op1, OP *op2)
{
  return (INT64)op1->order - (INT64)op2->order;
}

inline INT32 OP_Order(OP *op1, OP *op2)
{
  return -(op1->order < op2->order) | (op2->order < op1->order);
}


/* Count the number of real ops/instruction-words emitted by the op.
 */
extern INT16 OP_Real_Ops( const OP *op );
extern INT OP_Real_Inst_Words( const OP *op );
#ifdef TARG_ST
// Count the number of real unit slots used by the op.
extern INT OP_Real_Unit_Slots( const OP *op );
#endif


/* ======================================================================
 * Macros to traverse the OPs within a basic block. Use of these macros
 * allows us to change the underlying representation of the list OPs
 * without affecting most users.
 * ======================================================================*/

#define FOR_ALL_BB_OPs_FWD(bb,op) \
    for (op = BB_first_op(bb); op != NULL; op = OP_next(op))

#define FOR_ALL_BB_OPs_REV(bb,op) \
    for (op = BB_last_op(bb); op != NULL; op = OP_prev(op))

#define FOR_ALL_BB_OPs 	FOR_ALL_BB_OPs_FWD

#define FOR_ALL_OPS_OPs_FWD(ops,op) \
    for (op = OPS_first(ops); op && op != OP_next(OPS_last(ops)); \
	 op = OP_next(op))

#define FOR_ALL_OPS_OPs_REV(ops,op) \
    for (op = OPS_last(ops); op && op != OP_prev(OPS_first(ops)); \
	 op = OP_prev(op))

#define FOR_ALL_OPS_OPs FOR_ALL_OPS_OPs_FWD

void Print_OP (const OP *op, FILE *f = TFile);
#pragma mips_frequency_hint NEVER Print_OP
void Print_OPs (const OP *op, FILE *f = TFile);
#pragma mips_frequency_hint NEVER Print_OPs
void Print_OPS (const OPS *ops, FILE *f = TFile);
#pragma mips_frequency_hint NEVER Print_OPS
void Print_OP_No_SrcLine (const OP *op, FILE *f = TFile);
#pragma mips_frequency_hint NEVER Print_OP_No_SrcLine
void Print_OPs_No_SrcLines (const OP *op, FILE *f = TFile);
#pragma mips_frequency_hint NEVER Print_OPs_No_SrcLines
void Print_OPS_No_SrcLines (const OPS *ops, FILE *f = TFile);
#pragma mips_frequency_hint NEVER Print_OPS_No_SrcLines

BOOL OP_Is_Float_Mem(const OP *op);
BOOL OP_Alloca_Barrier(OP *op);
BOOL Is_Delay_Slot_Op (OP *xfer_op, OP *op);

extern void OP_Base_Offset_TNs(OP *memop, struct tn **base_tn, struct tn **offset_tn);

#ifdef KEY
extern BOOL TN_Pair_In_OP(OP* op, struct tn *tn_res, struct tn *tn_opnd);
#else
/***********************************************************************
 *
 *      Return a boolean to indicate if <tn> is both an operand and a
 *      result in the given <op>, i.e. is same_res.
 *
 ***********************************************************************/

inline BOOL 
TN_Pair_In_OP(OP* op, struct tn *tn_res, struct tn *tn_opnd) 
{
  INT i;
  for (i = 0; i < OP_results(op); i++) {
    if (tn_res == OP_result(op,i)) {
      break; 
    }
  }
  if (i == OP_results(op)) {
    return FALSE;
  }
  for (i = 0; i < OP_opnds(op); i++) {
    if (tn_opnd == OP_opnd(op,i)) {
      return TRUE; 
    }
  }
  return FALSE;
}
#endif	// KEY

#ifdef KEY
INT TN_Resnum_In_OP (OP* op, struct tn *tn, BOOL match_assigned_reg = FALSE); 
#else
/***********************************************************************
 *
 *      This routine assumes that the given <tn> is a result in <op>.
 *      It returns an INT that is the value of the result-number for
 *      this <tn> in this <op>.
 *
 ***********************************************************************/

inline INT 
TN_Resnum_In_OP (OP* op, struct tn *tn) 
{
  for (INT i = 0; i < OP_results(op); i++) {
    if (tn == OP_result(op,i)) {
      return i;
    }
  }
  FmtAssert (FALSE,
             ("TN_resnum_in_OP: Could not find <tn> in results list\n"));
  return -1;
}
#endif	// KEY

/***********************************************************************
 *
 *      This routine assumes that the given <tn> is an operand in <op>.
 *      It returns an INT that is the value of the operand-number for
 *      this <tn> in this <op>.
 *
 ***********************************************************************/

inline INT 
TN_Opernum_In_OP (OP* op, struct tn *tn) 
{
  for (INT i = 0; i < OP_opnds(op); i++) {
    if (tn == OP_opnd(op, i)) {
      return i;
    }
  }
  FmtAssert (FALSE,
             ("TN_Opernum_in_OP: Could not find <tn> in operands list\n"));
  return -1;
}
#ifndef TARG_ST
#include "op_targ.h"
#endif

#ifdef TARG_ST
/* =====================================================================
 * OPs_Are_Equivalent
 *
 * Returns true is 2 ops are equivalent.
 * If true, it is useless to replace one by hte other
 * for instance.
 * 2 ops are equivalent if the intruction is the same
 * and arguments are the same that is:
 * 1. opcode are the same
 * 2. all non register operands are the same
 * 3. all register operands are the same, or if allocated
 *    allocated registers are the same.
 * Note that flags attached to ops are not tested.
 * =====================================================================
 */
extern BOOL OPs_Are_Equivalent(OP *op1, OP *op2);

/* =====================================================================
   Opnds_Are_Equivalent
   Returns TRUE if TNs have the same base TN number or that they are
   assigned the same register.
 ===================================================================== */
extern BOOL Opnds_Are_Equivalent(OP *op1, OP *op2, int idx1, int idx2);

/* =====================================================================
   OPS_Copy_Predicate  
   Copy predicate from src_op to ops.
 ===================================================================== */
extern void OPS_Copy_Predicate(OPS* ops, OP *src_op);

/* ====================================================================
 *
 * OP_storeval_byte_offset
 *
 * OP is a memory store.
 * Returns the byte offset at which operand opndno is stored, ofset from
 * the start of the memory area written by op.
 * ====================================================================
 */
extern INT OP_storeval_byte_offset (OP *op, INT opndno);

/* ====================================================================
 *
 * OP_loadval_byte_offset
 *
 * OP is a memory load.
 * Returns the byte offset from which result resno is loaded, ofset from
 * the start of the memory area read by op.
 * ====================================================================
 */
extern INT OP_loadval_byte_offset (OP *op, INT resno);

// FdF: Returns 0 if op is not an auto-mod operation, or if tn is not
// the auto-modified register, or is redefined also by the operation.
// Otherwise, return -1 for a pre-automod, and 1 for a post-automod.
extern INT TN_isAutoMod(OP *op, TN *tn);

/* ====================================================================
 *
 * OP_load_result
 *
 * OP is a memory load.
 * Returns the index of the result that will contains the loaded value.
 * -1 is returned if it cannot be determined (load with several 
 * candidate results).
 * ====================================================================
 */
extern INT OP_load_result (OP *op);

/**
 * Generate a conditional move with predicate information.
 * If no real operation exists, generate a simulated one.
 * ([CG]: we might use a temporary pseudo OP for SSA repairs
 *        as it has a predicate information and thus avoid
 *        multiple repairs (see repair code in cg_ssa.cxx).
 *        After the out of SSA all movc are replaced by select
 *        operations or the corresponding target dependent
 *        conditional)
 */
void
CGTARG_OP_Make_movc( TN *guard,
                     TN *dst,
                     TN *src,
                     OPS *cmov_ops,
                     bool on_false );

/**
 * If argument <op> is a simulated operation representing a conditional move,
 * generates the real target code to perform the move and return TRUE.
 * Otherwise return FALSE.
 */
BOOL
CGTARG_OP_Lower_movc( OP *op,
                      OPS *lowered_ops );
#endif

#endif /* op_INCLUDED */

