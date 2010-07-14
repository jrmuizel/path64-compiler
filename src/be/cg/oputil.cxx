/*
 * Copyright (C) 2007, 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
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


/* ====================================================================
 * ====================================================================
 *
 * Module: oputil.c
 * $Revision: 1.26 $
 * $Date: 04/12/21 14:57:07-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/be/cg/SCCS/s.oputil.cxx $
 *
 * Revision history:
 *  12-Oct-89 - Original Version
 *  01-Feb-91 - Copied for TP/Muse
 *  12-Jun-91 - Removed INS/INSCH stuff to insutil.c
 *  12-Jun-91 - Added OP insertion/deletion stuff from bbutil.c
 *
 * Description:
 *
 * Utility routines for manipulating the CGIR OP and OPS data
 * structures.  Also implements a few routines that manipulate BBs as
 * well since the BB implementation is intrinsically intertwined with
 * the OP implementation.  See "op.h" and "bb.h" for interfaces.
 *
 * TODO: Combine "op.h" and "bb.h" into "cgir.h", and "oputil.cxx" and
 *       "bbutil.cxx" into "cgir.cxx".
 *
 * ====================================================================
 * ==================================================================== */

#ifdef USE_PCH
#include "cg_pch.h"
#endif // USE_PCH
#pragma hdrstop

#include <stdarg.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include "defs.h"
#include "config.h"
#include "tracing.h"
#include "erglob.h"
#include "printsrc.h"

#include "import.h"
#include "opt_alias_interface.h"        /* for Print_alias_info */
#include "cg.h"                         /* for Alias_Manager */

#include "cgir.h"
#include "cg.h"
#include "register.h"
#include "cgtarget.h"
#include "cg_dep_graph.h"
#include "cgprep.h"
#include "cg_loop.h"
#include "cg_spill.h"

#include "wn.h"
#include "whirl2ops.h"
#include "cgexp.h"
#include "xstats.h"
#include "tag.h"
#ifdef TARG_ST
#include "cg_ssa.h"
#include "op_map.h"			/* For using OP_Asm_Map */
#include "cg_affirm.h"
#endif

/* Allocate OPs for the duration of the PU. */
#define OP_Alloc(size)  ((OP *)Pu_Alloc(size))

/* OP mutators that are NOT to be made public */
#define Set_OP_code(o,opc)	((o)->opr = (mTOP)(opc))
#define Set_OP_opnds(o,n)	((o)->opnds = (n))
#define Set_OP_results(o,n)	((o)->results = (n))
#ifdef TARG_ST
BOOL 
Set_OP_opnd_Immediate_Variant(OP *op, INT idx, TN *tn) {
	TOP top = TOP_UNDEFINED;
	DevAssert(TN_has_value(tn),("Set_OP_opnd_Immediate_Variant must be called with a Litteral TN\n"));
	top=TOP_opnd_immediate_variant(OP_code(op),idx,TN_value(tn));
	if (top!=TOP_UNDEFINED) {
		Set_OP_opnd(op, idx, tn);
		OP_Change_Opcode(op,top);
		return TRUE;
	}
	return FALSE;
}
#endif

// ----------------------------------------
// Copy ASM_OP_ANNOT when duplicating an OP
// ----------------------------------------
inline void
Copy_Asm_OP_Annot(OP* new_op, const  OP* op) 
{
  if (OP_code(op) == TOP_asm) {
    OP_MAP_Set(OP_Asm_Map, new_op, OP_MAP_Get(OP_Asm_Map, op));
  }
}
// ----------------------------------------
// Copy ASM_OP_ANNOT when duplicating an OP
// ----------------------------------------
static void
Copy_OP_Annot(OP* new_op, OP* op) 
{
  Copy_Asm_OP_Annot (new_op, op);
}

/* ====================================================================
 *
 * New_OP
 *
 * Create and clear a new OP structure.
 *
 * ====================================================================
 */

static OP *
New_OP ( INT results, INT opnds )
{
  OP *op = OP_Alloc ( OP_sizeof(results, opnds) );
  PU_OP_Cnt++;
  Set_OP_opnds(op, opnds);
  Set_OP_results(op, results);
#ifdef TARG_ST
  // (cbr) make sure effects are reset
  for (int i = 0; i < opnds; i++)
    Reset_OP_effects(op, i, -1);
#endif

#ifdef TARG_ST
  op->g_map_idx=PU_OP_Cnt;
  op->scycle = -1;
#endif

  return op;
}

#ifdef TARG_ST
/* ====================================================================
 *
 * OPS_Copy_Predicate()
 *
 * See interface description
 * ====================================================================
 */
 void
 OPS_Copy_Predicate (OPS *ops, OP *src_op) 
 {
   int pred_idx = OP_find_opnd_use(src_op, OU_predicate);

   if (OP_has_predicate(src_op) && OP_opnd(src_op, pred_idx) != True_TN)  {
     FmtAssert(pred_idx >= 0, ("invalide predicate operand"));
     TN *pred = OP_opnd (src_op, pred_idx);
     bool on_false = OP_Pred_False(src_op, pred_idx);
     OP *op;
     
     FOR_ALL_OPS_OPs(ops, op) {
       FmtAssert(OP_has_predicate(op), ("try to predicate a non predicated op."));
       CGTARG_Predicate_OP (NULL, op, pred, on_false);
     }
   }
 }
#endif

/* ====================================================================
 *
 * Dup_OP
 *
 * Create a new OP structure as a duplicate of another, with zero ID.
 *
 * ====================================================================
 */

OP *
Dup_OP (const OP *op )
{
  INT results = OP_results(op);
  INT opnds = OP_opnds(op);
  OP *new_op = New_OP ( results, opnds );

  memcpy(new_op, op, OP_sizeof(results, opnds));
  new_op->next = new_op->prev = NULL;
  new_op->bb = NULL;

  Copy_Asm_OP_Annot ( new_op, op );
#ifdef TARG_ST
  LABEL_IDX tag = Get_OP_Tag(op);
  new_op->g_map_idx=PU_OP_Cnt;
  if (tag) {
#else
  if (OP_has_tag(op)) {
#endif
	Set_OP_Tag (new_op, Gen_Tag());
  }
  
#ifdef TARG_X8664
  if ( TOP_is_vector_high_loadstore ( OP_code ( new_op ) ) )
    Set_OP_cond_def_kind(new_op, OP_ALWAYS_COND_DEF);
#endif

#ifdef KEY
  // If OP is a restore, increment the spill location's restore count.
  if (OP_load(op)) {
    ST *spill_loc = CGSPILL_OP_Spill_Location(op);
    if (spill_loc != (ST *)0)		// It's a spill OP.
      CGSPILL_Inc_Restore_Count(spill_loc);
  }
#endif

  return new_op;
}
#ifdef TARG_ST

/* ====================================================================
 *
 * Resize_OP
 *
 * Create a new OP structure as a duplicate of another, with extended number of results and opnds.
 *
 * ====================================================================
 */

OP *
  Resize_OP ( OP *op, INT results, INT opnds )
{
  OP *new_op = New_OP ( results, opnds );

  memcpy(new_op, op, OP_sizeof(0,0));
  new_op->results = results;
  new_op->opnds = opnds;
  INT min_results = MIN(OP_results(op), results);
  INT min_opnds = MIN(OP_opnds(op), opnds);
  memcpy(new_op->res_opnd+OP_result_offset(new_op), op->res_opnd+OP_result_offset(op), sizeof(TN *)*min_results);
  memcpy(new_op->res_opnd+OP_opnd_offset(new_op), op->res_opnd+OP_opnd_offset(op), sizeof(TN *)*min_opnds);
  new_op->next = new_op->prev = NULL;
  new_op->bb = NULL;

  Copy_OP_Annot ( new_op, op );
  WN *wn;
  if (wn = Get_WN_From_Memory_OP(op))
    OP_MAP_Set(OP_to_WN_map, new_op, wn);
  LABEL_IDX tag = Get_OP_Tag(op);
  new_op->g_map_idx=PU_OP_Cnt;
  if (tag) {
    Set_OP_Tag (new_op, Gen_Tag());
  }
  
  return new_op;
}

/* ====================================================================
 *
 * OP_Copy_Properties()
 *
 * See interface description
 * ====================================================================
 */
void
OP_Copy_Properties(OP *op, OP *src_op)
{
  /* Copy srcpos. */
  op->srcpos = src_op->srcpos;

  /* Copy unrolling related fields. */
  op->unroll_bb = src_op->unroll_bb;
  op->orig_idx = src_op->orig_idx;
  op->unrolling = src_op->unrolling;

  /* Copy all flags. */
  op->flags = src_op->flags;
  op->flags2 = src_op->flags2;

  if (OP_spill(src_op)) {
    FmtAssert (OP_spilled_tn(src_op) != NULL, ("Missing spilled tn"));
    Set_OP_spilled_tn(op, OP_spilled_tn(src_op));
  }

  /* Copy scheduling info. */
  op->scycle = src_op->scycle;

  /* Copy annotations. */
  Copy_OP_Annot(op, src_op);
}

#endif

/* =====================================================================
 *			      OPS stuff
 *		(see "op.h" for interface description)
 * =====================================================================
 */

/* -----------------------------------------------------------------------
 *
 * void insert_ops_before(OPS *ops, OP *point, OP *first, OP *last)
 * void insert_ops_after(OPS *ops, OP *point, OP *first, OP *last)
 * void append_ops(OPS *ops, OP *first, OP *last)
 * void prepend_ops(OPS *ops, OP *first, OP *last)
 * void insert_ops(OPS *ops, OP *point, OP *first, OP *last, BOOL before)
 *
 * Requires: <last> is a (not necessarily direct) successor of <first>.
 *
 * Insert the OPs from <first> to <last> inclusive in the place implied
 * by the function names and/or <point> and/or <before> arguments when
 * applicable.
 *
 * Basically these are the workhorses for the OPS/BB Insert routines,
 * but they avoid setting any OP attributes other than the next and
 * previous pointers.
 *
 * -----------------------------------------------------------------------
 */

inline void prepend_ops(OPS *ops, OP *first, OP *last)
{
  OP **pprev = OPS_first(ops) ? &OPS_first(ops)->prev : &ops->last;
  first->prev = NULL;
  last->next = OPS_first(ops);
  ops->first = first;
  *pprev = last;
}


inline void append_ops(OPS *ops, OP *first, OP *last)
{
  OP **pnext = OPS_last(ops) ? &OPS_last(ops)->next : &ops->first;
  last->next = NULL;
  first->prev = OPS_last(ops);
  ops->last = last;
  *pnext = first;
}

inline void insert_ops_before(OPS *ops, OP *point, OP *first, OP *last)
{
  OP **prevp = OP_prev(point) ? &OP_prev(point)->next : &ops->first;
  *prevp = first;
  last->next = point;
  first->prev = OP_prev(point);
  point->prev = last;
}

inline void insert_ops_after(OPS *ops, OP *point, OP *first, OP *last)
{
  OP **nextp = OP_next(point) ? &OP_next(point)->prev : &ops->last;
  *nextp = last;
  first->prev = point;
  last->next = OP_next(point);
  point->next = first;
}

inline void insert_ops(OPS *ops, OP *point, OP *first, OP *last, BOOL before)
{
  if (point == NULL) {
    if (before)
      prepend_ops(ops, first, last);
    else
      append_ops(ops, first, last);
  } else {
    if (before)
      insert_ops_before(ops, point, first, last);
    else
      insert_ops_after(ops, point, first, last);
  }
}


/* -----------------------------------------------------------------------
 *   
 *  Sink the OP before point
 *
 * -----------------------------------------------------------------------
 */
void BB_Sink_Op_Before(BB *bb, OP *op, OP *point)
{
  if (OP_next(op) == point) return;

  Is_True(OP_bb(op) == bb && OP_bb(point) == bb,
	  ("Sink_Op_Before: must sink inside the bb."));

  // Disconnect "op" from body
  OP *t1 = OP_prev(op);
  OP *t2 = OP_next(op);
  if (t1) t1->next = t2;
  if (t2) t2->prev = t1;

  // Reconnect "op" to "succ"
  OP *prev = OP_prev(point);
  op->prev = prev;
  op->next = point;
  prev->next = op;
  point->prev = op;

  if (op == BB_first_op(OP_bb(op)))
    OP_bb(op)->ops.first = t2;

}


/* -----------------------------------------------------------------------
 *
 *  void setup_ops(BB *bb, OP *first, OP *last, UINT32 len)
 *
 *  Setup various fields (bb/map_idx/order) on OPs between <first> and
 *  <last>, inclusive, newly inserted into <bb>.  The bb and map_idx
 *  fields are fairly straightforward.  The order field is used to
 *  indicate relative order within <bb> (unless it is NULL).  May also
 *  change the order fields of other OPs in the BB.  Should almost
 *  always execute in time linear in the length of the chain from
 *  <first> to <last>.  Worst case time is linearly dependent on the
 *  size of the BB, but should be tuned to make this case extremely
 *  rare.
 *
 * ----------------------------------------------------------------------- */

/* Assume op->order is some kind of unsigned integer type.
 */
#define ORDER_TYPE UINT16
#define mORDER_TYPE mUINT16
#define mMAP_IDX_TYPE mUINT16
#define ORDER_BITS (sizeof(mORDER_TYPE) * 8)
#define MIN_INITIAL_SPACING \
  ((mORDER_TYPE)1 << (ORDER_BITS-(sizeof(mMAP_IDX_TYPE)*8)))
#define INITIAL_SPACING ((ORDER_TYPE)(MIN_INITIAL_SPACING * 8))
#ifdef TARG_ST
// [SC] We only guarantee 16 bits for ORDER_TYPE, but note
// that the type UINT16 is * at least * 16 bits, therefore
// it is not safe to assume that ((ORDER_TYPE)-1) fits in 16 bits.
#define MAX_ORDER UINT16_MAX
#else
#define MAX_ORDER ((ORDER_TYPE)-1)
#endif

#ifdef TARG_ST
static void
verify_same_res(OP *op)
{
  if (!tn_ssa_map) {
    INT i;
    for (i = 0; i < OP_results(op); i++) {
      INT same_res = OP_same_res(op, i);
      if (same_res >= 0) {
	TN *tn1 = OP_result(op, i);
	TN *tn2 = OP_opnd(op, same_res); 

	if (!(tn1 == tn2 ||
	      ((TN_is_register(tn1) && TN_is_register(tn2) &&
		(TN_is_dedicated(tn1) || (TN_register(tn1) != REGISTER_UNDEFINED)) &&
		(TN_is_dedicated(tn2) || (TN_register(tn2) != REGISTER_UNDEFINED)) &&
		(TN_register_and_class(tn1) == TN_register_and_class(tn2)))))) {
          
	  FmtAssert(0, ("same result operand mismatch when inserting op (%s)",TOP_Name(OP_code(op))));
	}
      }
    }
  }
}
#endif


static void setup_ops(BB *bb, OP *first, OP *last, UINT32 len)
{
  OP *op;
  ORDER_TYPE incr;
  ORDER_TYPE order_before;
  ORDER_TYPE order_after;
  ORDER_TYPE order;

  /* Empty lists are easy.
   */
  if (len == 0) return;

  /* Get the 'order' number of the OPs immediately before and after
   * the OPs were inserting.
   */
  order_before = OP_prev(first) ? OP_prev(first)->order : 0;
  order_after = OP_next(last) ? OP_next(last)->order : MAX_ORDER;

  /* Compute the increment to use when assigning 'order' numbers
   * so they will fit between the OPs were inserting at. If there
   * isn't enough room, i.e. 'incr' is 0, we'll detect that later.
   */
  incr = (order_after - order_before - 1) / (len + 1);
  if (incr > INITIAL_SPACING) incr = INITIAL_SPACING;

  /* Loop over the OPs being inserted and initialize the necessary
   * fields. Our attempt at generating 'order' numbers is an
   * educated guess, but in many cases we'll guess right.
   */
  order = order_before;
  op = first;
  do {
    FmtAssert(op, ("input ops not connected properly"));
    op->bb = bb;
    op->map_idx = BB_New_Op_Map_Idx(bb);
    order += incr;
    op->order = order;
    REGISTER_CLASS_OP_Update_Mapping (op);
#ifdef TARG_ST
    SSA_setup(op);
    verify_same_res(op);
#endif
    op = OP_next(op);
  } while (op != OP_next(last));

  /* All done if we were able to squeeze in the 'order' numbers.
   */
  if (incr != 0) goto done;

  /* It was not possible to assign 'order' numbers to the new OPs --
   * we'll have to re-order some of the OPs on the list we're inserting
   * into. Include OPs from before and/or after the inserted OPs until
   * we make a big enough hole that it is possible to re-order.
   *
   * NOTE: We include all 'after' OPs before including any of the 
   * 'before' OPs. This tends to keep the beginning of the list
   * "nicely" ordered. Another approach would be to include from
   * the direction that adds the most to the delta between 
   * 'order_after' and 'order_before'.
   */
  do {
    if (OP_next(last)) {
      last = OP_next(last);
      order_after = OP_next(last) ? OP_next(last)->order : MAX_ORDER;
    } else if (OP_prev(first)) {
      first = OP_prev(first);
      order_before = OP_prev(first) ? OP_prev(first)->order : 0;
    } else {
      FmtAssert(FALSE, ("unable to reorder"));
    }
    len++;
    incr = (order_after - order_before - 1) / (len + 1);
  } while (incr == 0);
  if (incr > INITIAL_SPACING) incr = INITIAL_SPACING;

  /* Re-order the OPs.
   */
  op = first;
  order = order_before;
  do {
    order += incr;
    op->order = order;
    op = OP_next(op);
  } while (op != OP_next(last));

done:
  /* C insists on requiring at least one statement after a label.
   * There won't be any when VERIFY_OPS is not defined, so here ya go...
   */
  ;

#ifdef VERIFY_OPS
  {
    UINT16 len = 1;
    op = first;
    while (OP_prev(op)) op = OP_prev(op);
    while (OP_next(op)) {
      FmtAssert(op->order < OP_next(op)->order, ("OP order set wrong"));
      FmtAssert(op->bb == bb, ("OP bb set wrong"));
      op = OP_next(op);
      len++;
    }
    FmtAssert(len == BB_length(bb), ("BB_length set wrong"));
  }
#endif
}


void OPS_Insert_Op(OPS *ops, OP *point, OP *op, BOOL before)
{
  insert_ops(ops, point, op, op, before);
  ops->length++;
}


void OPS_Insert_Op_Before(OPS *ops, OP *point, OP *op)
{
  insert_ops_before(ops, point, op, op);
  ops->length++;
}


void OPS_Insert_Op_After(OPS *ops, OP *point, OP *op)
{
  insert_ops_after(ops, point, op, op);
  ops->length++;
}


void OPS_Append_Op(OPS *ops, OP *op)
{
  append_ops(ops, op, op);
  ops->length++;
}


void OPS_Prepend_Op(OPS *ops, OP *op)
{
  prepend_ops(ops, op, op);
  ops->length++;
}


void OPS_Insert_Ops(OPS *ops, OP *point, OPS *new_ops, BOOL before)
{
  if (OPS_first(new_ops) == NULL) return;
  insert_ops(ops, point, OPS_first(new_ops), OPS_last(new_ops), before);
  ops->length += OPS_length(new_ops);
}


void OPS_Insert_Ops_Before(OPS *ops, OP *point, OPS *new_ops)
{
  if (OPS_first(new_ops) == NULL) return;
  insert_ops_before(ops, point, OPS_first(new_ops), OPS_last(new_ops));
  ops->length += OPS_length(new_ops);
}


void OPS_Insert_Ops_After(OPS *ops, OP *point, OPS *new_ops)
{
  if (OPS_first(new_ops) == NULL) return;
  insert_ops_after(ops, point, OPS_first(new_ops), OPS_last(new_ops));
  ops->length += OPS_length(new_ops);
}


void OPS_Append_Ops(OPS *ops, OPS *new_ops)
{
  if (OPS_first(new_ops) == NULL) return;
  append_ops(ops, OPS_first(new_ops), OPS_last(new_ops));
  ops->length += OPS_length(new_ops);
}


void OPS_Prepend_Ops(OPS *ops, OPS *new_ops)
{
  if (OPS_first(new_ops) == NULL) return;
  prepend_ops(ops, OPS_first(new_ops), OPS_last(new_ops));
  ops->length += OPS_length(new_ops);
}

// Update OP order
//
void BB_Update_OP_Order(BB *bb)
{
  INT order = 0;
  INT incr = INITIAL_SPACING;
  for (OP *op = BB_first_op(bb); op; op = OP_next(op)) {
    order += incr;
    op->order = order;
  } 
}

// Verify OP order
//
void BB_Verify_OP_Order(BB *bb)
{
  INT prev_order = -1;
  for (OP *op = BB_first_op(bb); op; op = OP_next(op)) {
    FmtAssert(prev_order < op->order,
	      ("BB_Verify_OP_Order: OP_order() is not correct."));
    prev_order = op->order;
  }
}


/* =====================================================================
 *			   (Some) BB stuff
 *		(see "bb.h" for interface description)
 * =====================================================================
 */

void BB_Insert_Op(BB *bb, OP *point, OP *op, BOOL before)
{
  Is_True(bb, ("can't insert in NULL BB"));
  insert_ops(&bb->ops, point, op, op, before);
  bb->ops.length++;
  setup_ops(bb, op, op, 1);
}


void BB_Insert_Op_Before(BB *bb, OP *point, OP *op)
{
  Is_True(bb, ("can't insert in NULL BB"));
  insert_ops_before(&bb->ops, point, op, op);
  bb->ops.length++;
  setup_ops(bb, op, op, 1);
}


void BB_Insert_Op_After(BB *bb, OP *point, OP *op)
{
  Is_True(bb, ("can't insert in NULL BB"));
  insert_ops_after(&bb->ops, point, op, op);
  bb->ops.length++;
  setup_ops(bb, op, op, 1);
}


void BB_Prepend_Op(BB *bb, OP *op)
{
  Is_True(bb, ("can't insert in NULL BB"));
  prepend_ops(&bb->ops, op, op);
  bb->ops.length++;
  setup_ops(bb, op, op, 1);
}


void BB_Append_Op(BB *bb, OP *op)
{
  Is_True(bb, ("can't insert in NULL BB"));
  append_ops(&bb->ops, op, op);
  bb->ops.length++;
  setup_ops(bb, op, op, 1);
}


void BB_Insert_Ops(BB *bb, OP *point, OPS *ops, BOOL before)
{
  if (OPS_first(ops) == NULL) return;
  insert_ops(&bb->ops, point, OPS_first(ops), OPS_last(ops), before);
  bb->ops.length += OPS_length(ops);
  setup_ops(bb, OPS_first(ops), OPS_last(ops), OPS_length(ops));
}


void BB_Insert_Ops_Before(BB *bb, OP *point, OPS *ops)
{
  if (OPS_first(ops) == NULL) return;
  insert_ops_before(&bb->ops, point, OPS_first(ops), OPS_last(ops));
  bb->ops.length += OPS_length(ops);
  setup_ops(bb, OPS_first(ops), OPS_last(ops), OPS_length(ops));
}


void BB_Insert_Ops_After(BB *bb, OP *point, OPS *ops)
{
  if (OPS_first(ops) == NULL) return;
  insert_ops_after(&bb->ops, point, OPS_first(ops), OPS_last(ops));
  bb->ops.length += OPS_length(ops);
  setup_ops(bb, OPS_first(ops), OPS_last(ops), OPS_length(ops));
}


void  BB_Insert_Noops(OP *op, INT num, BOOL before)
{
  OPS new_ops = OPS_EMPTY;
  INT i;

  for (i = 0; i < num; i++) {
    Exp_Noop (&new_ops);
  }
  BB_Insert_Ops(OP_bb(op), op, &new_ops, before);
}


void BB_Prepend_Ops(BB *bb, OPS *ops)
{
  if (OPS_first(ops) == NULL) return;
  prepend_ops(&bb->ops, OPS_first(ops), OPS_last(ops));
  bb->ops.length += OPS_length(ops);
  setup_ops(bb, OPS_first(ops), OPS_last(ops), OPS_length(ops));
}


void BB_Append_Ops(BB *bb, OPS *ops)
{
  if (OPS_first(ops) == NULL) return;
  append_ops(&bb->ops, OPS_first(ops), OPS_last(ops));
  bb->ops.length += OPS_length(ops);
  setup_ops(bb, OPS_first(ops), OPS_last(ops), OPS_length(ops));
}


void BB_Move_Op(BB *to_bb, OP *point, BB *from_bb, OP *op, BOOL before)
{
  Is_True(OP_bb(op) == from_bb, ("op not in from_bb"));
  Is_True(OP_bb(point) == to_bb, ("point not in to_bb"));
  OPS_Remove_Op(&from_bb->ops, op);
  insert_ops(&to_bb->ops, point, op, op, before);
  to_bb->ops.length++;
  setup_ops(to_bb, op, op, 1);
}


void BB_Move_Op_Before(BB *to_bb, OP *point, BB *from_bb, OP *op)
{
  Is_True(OP_bb(op) == from_bb, ("op not in from_bb"));
  Is_True(OP_bb(point) == to_bb, ("point not in to_bb"));
  OPS_Remove_Op(&from_bb->ops, op);
  insert_ops_before(&to_bb->ops, point, op, op);
  to_bb->ops.length++;
  setup_ops(to_bb, op, op, 1);
}

void BB_Move_Op_After(BB *to_bb, OP *point, BB *from_bb, OP *op)
{
  Is_True(OP_bb(op) == from_bb, ("op not in from_bb"));
  Is_True(OP_bb(point) == to_bb, ("point not in to_bb"));
  OPS_Remove_Op(&from_bb->ops, op);
  insert_ops_after(&to_bb->ops, point, op, op);
  to_bb->ops.length++;
  setup_ops(to_bb, op, op, 1);
}


void BB_Move_Op_To_Start(BB *to_bb, BB *from_bb, OP *op)
{
  Is_True(OP_bb(op) == from_bb, ("op not in from_bb"));
  OPS_Remove_Op(&from_bb->ops, op);
  prepend_ops(&to_bb->ops, op, op);
  to_bb->ops.length++;
  setup_ops(to_bb, op, op, 1);
}


void BB_Move_Op_To_End(BB *to_bb, BB *from_bb, OP *op)
{
  Is_True(OP_bb(op) == from_bb, ("op not in from_bb"));
  OPS_Remove_Op(&from_bb->ops, op);
  append_ops(&to_bb->ops, op, op);
  to_bb->ops.length++;
  setup_ops(to_bb, op, op, 1);
}

#ifdef TARG_ST
void BB_Replace_Op(OP *old_op, OP *new_op) {

  Is_True(OP_bb(old_op) != NULL, ("old_op not in a BB"));
  Is_True(OP_bb(new_op) == NULL, ("new_op already in a BB"));
  OP *point = OP_prev(old_op);
  BOOL before = (point == NULL);
  BB *bb = OP_bb(old_op);

  BB_Remove_Op(bb, old_op);
  BB_Insert_Op(bb, point, new_op, before);
}
#endif

void BB_Append_All(BB *to_bb, BB *from_bb)
{
  OPS the_ops;

  if (BB_length(from_bb) == 0) return;

  the_ops = from_bb->ops;
  BB_Remove_All(from_bb);
  BB_Append_Ops(to_bb, &the_ops);
}


void BB_Prepend_All (BB *to_bb, BB *from_bb)
{
  OPS the_ops;

  if (BB_length(from_bb) == 0) return;

  the_ops = from_bb->ops;
  BB_Remove_All (from_bb);
  BB_Prepend_Ops (to_bb, &the_ops);
}


OP *BB_Remove_Branch(BB *bb)
{
  OP *last_op;
  OP *br = BB_branch_op(bb);

  if (br) {
    last_op = BB_last_op(bb);
    if (OP_noop(last_op)) BB_Remove_Op(bb, last_op);
    BB_Remove_Op(bb, br);
  }

  return br;
}


void BB_Remove_Op(BB *bb, OP *op)
{
  OPS_Remove_Op(&bb->ops, op);
  op->bb = NULL;
}


void BB_Remove_Ops(BB *bb, OPS *ops)
{
  OP *op;

  if (OPS_first(ops) == NULL) return;

  OPS_Remove_Ops(&bb->ops, ops);

#ifndef TARG_ST
  FOR_ALL_OPS_OPs(ops, op) op->bb = NULL;
#endif
}


void BB_Remove_All(BB *bb)
{
#ifdef TARG_ST
  /* FDF: Because ops is emptied by BB_Remove_Ops ! */
  OP *op;
  FOR_ALL_OPS_OPs(&bb->ops, op) op->bb = NULL;
#endif
  BB_Remove_Ops(bb, &bb->ops);
  BB_next_op_map_idx(bb) = 0;
}

/* ====================================================================
 *
 * Mk_OP / Mk_VarOP
 *
 * Make new OP records.
 *
 * ====================================================================
 */
OP *
Mk_OP(TOP opr, ...)
{
  va_list ap;
  INT i;
  INT results = TOP_fixed_results(opr);
  INT opnds = TOP_fixed_opnds(opr);
  OP *op = New_OP(results, opnds);

  FmtAssert(!TOP_is_var_opnds(opr), ("Mk_OP not allowed with variable operands"));

#ifdef TARG_ST
  FmtAssert(ISA_SUBSET_LIST_Member (ISA_SUBSET_List, opr),
	    ("Mk_OP: op not supported on target (%s)", TOP_Name(opr)));
#endif
  Set_OP_code(op, opr);

  va_start(ap, opr);

  for (i = 0; i < results; ++i) {
    TN *result = va_arg(ap, TN *);
    Set_OP_result(op, i, result);
  }
#ifndef TARG_ST
  if (TOP_is_defs_fpu_int(opr)) Set_TN_is_fpu_int(OP_result(op, 0));
#endif
  for (i = 0; i < opnds; ++i) {
    TN *opnd = va_arg(ap, TN *);
    Set_OP_opnd(op, i, opnd);
  }

  va_end(ap);

  CGTARG_Init_OP_cond_def_kind(op);

#if Is_True_On
#ifdef TARG_X8664
  // Make sure no 64-bit int operations for n32 will be generated.
  if( Is_Target_32bit() &&
      !OP_dummy( op )   &&
      !OP_simulated(op) &&
      !OP_cond_move(op) &&
      OP_code(op) != TOP_leave ){

    for( int i = 0; i < OP_results(op); i++ ){
      TN* tn = OP_result( op, i );
      if( tn != NULL && 
	  OP_result_size( op, i ) > 32 &&
	  TN_register_class(tn) == ISA_REGISTER_CLASS_integer ){
	FmtAssert( FALSE, ("i386 does not support 64-bit operation -- %s",
			   TOP_Name(opr) ) );
      }
    }

    const int base_idx = OP_find_opnd_use( op, OU_base );
    const int index_idx = OP_find_opnd_use( op, OU_index );
    const int target_idx = OP_find_opnd_use( op, OU_target );

    for( int i = 0; i < OP_opnds(op); i++ ){
      TN* tn = OP_opnd( op, i );
      if( tn != NULL     &&
	  i != base_idx  &&
	  i != index_idx &&
	  i != target_idx&&
	  OP_opnd_size( op, i ) > 32 &&
	  TN_register_class(tn) == ISA_REGISTER_CLASS_integer ){
	FmtAssert( FALSE, ("i386 does not support 64-bit operation -- %s",
			   TOP_Name(opr) ) );
      }
    }
  }
#endif // TARG_X8664
#endif // Is_True_On

#ifdef TARG_X8664
  if ( TOP_is_vector_high_loadstore ( OP_code ( op ) ) )
    Set_OP_cond_def_kind(op, OP_ALWAYS_COND_DEF);
#endif
  return op;
}

OP *
Mk_VarOP(TOP opr, INT results, INT opnds, TN **res_tn, TN **opnd_tn)
{
  if (results != TOP_fixed_results(opr)) {
    FmtAssert(TOP_is_var_opnds(opr) && results > TOP_fixed_results(opr),
	      ("%d is not enough results for %s", results, TOP_Name(opr)));
  }
  if (opnds != TOP_fixed_opnds(opr)) {
    FmtAssert(TOP_is_var_opnds(opr) && opnds > TOP_fixed_opnds(opr),
	      ("%d is not enough operands for %s", opnds, TOP_Name(opr)));
  }
#ifdef TARG_ST
  FmtAssert(ISA_SUBSET_LIST_Member (ISA_SUBSET_List, opr),
	    ("Mk_OP: op not supported on target (%s)", TOP_Name(opr)));
#endif

  INT i;
  OP *op = New_OP(results, opnds);

  Set_OP_code(op, opr);

  for (i = 0; i < results; ++i) Set_OP_result(op, i, res_tn[i]);
#ifndef TARG_ST
  if (TOP_is_defs_fpu_int(opr)) Set_TN_is_fpu_int(res_tn[0]);
#endif
  for (i = 0; i < opnds; ++i) Set_OP_opnd(op, i, opnd_tn[i]);

  CGTARG_Init_OP_cond_def_kind(op);

  return op;
}

/* ====================================================================
 *
 * Print_OP / Print_OP_No_SrcLine / Print_OPs / Print_OPS
 *
 * Print an OP (or OP list) to the trace file.  These shouldn't be
 * inlined since they're useful for debugging and don't affect user
 * compile-time performance.
 *
 * ====================================================================
 */

void Print_OP_No_SrcLine (const OP *op, FILE *f) {
  WN    *wn;
  BOOL  cg_loop_op = Is_CG_LOOP_Op (op);
#ifdef KEY
  fprintf (f, "[%4d,%2d] ", Srcpos_To_Line (OP_srcpos (op)), OP_scycle (op));
#else
  fprintf (f, "[%4d] ", Srcpos_To_Line (OP_srcpos (op)));
#endif
#ifdef TARG_ST
  LABEL_IDX tag = Get_OP_Tag(op);
  if (tag) {
#else
  if (OP_has_tag (op)) {
    LABEL_IDX tag = Get_OP_Tag (op);
#endif
    fprintf (f, "<tag %s>: ", LABEL_name (tag));
  }

  for (INT i = 0; i < OP_results (op); ++i) {
    Print_TN (OP_result (op, i), FALSE, f);
#ifdef TARG_ST
    TN *pinned;
    if (op_ssa_pinning_map && ((pinned = OP_Get_result_pinning(op, i)) != NULL)) {
      fprintf(TFile, "^");
      Print_TN(pinned, FALSE);
    }
#endif
    fprintf (f, " ");
  }
  fprintf(f, ":- ");
  fprintf(f, "%s ", TOP_Name (OP_code (op)));  
  if ( OP_variant(op) != 0 ) {
    fprintf ( TFile, "(%x) ", OP_variant(op));
  }
#ifdef TARG_ST
  if (OP_code(op) == TOP_psi) {
    for (INT i=0; i<PSI_opnds(op); i++) {
      TN *guard = PSI_guard(op, i);
      if (guard) {
        // (cbr) Support for guards on false
        if (PSI_Pred_False(op, i))
          fprintf(TFile, "!");
	Print_TN(guard, FALSE);
      }
      fprintf(TFile, "?");
      TN *tn = PSI_opnd(op,i);
      Print_TN(tn,FALSE);
      if (OP_Defs_TN(op, tn)) fprintf(TFile, "<defopnd>");
      fprintf(TFile, " ");
    }
  }
else
#endif
  for (INT i=0; i<OP_opnds (op); ++i) {
    TN *tn = OP_opnd (op, i);
#ifdef TARG_ST
    if (OP_code(op) == TOP_phi) {
      BB *pred_bb = Get_PHI_Predecessor(op, i);
      if (pred_bb)
	fprintf(TFile, "BB%d?", BB_id(pred_bb));
    }
    // (cbr) Support for guards on false
    if (OP_Pred_False(op, i))
      fprintf(TFile, "!");
#endif
    Print_TN (tn, FALSE, f);
#ifdef TARG_ST
    TN *pinned;
    if (op_ssa_pinning_map && ((pinned = OP_Get_opnd_pinning(op, i)) != NULL)) {
      fprintf(TFile, "^");
      Print_TN(pinned, FALSE);
    }
#endif
    if (cg_loop_op) {
      INT omega = TN_is_symbol (tn) ? OP_restore_omega (op) : OP_omega (op, i);
      if (omega) fprintf (f, "[%d]", omega);
    }
    if (OP_Defs_TN (op, tn)) fprintf(f, "<defopnd>");
    fprintf (f, " ");
  }

  fprintf (f, ";");

  /* print flags */
  // fprintf(f," flags 0x%08x ",OP_flags(op));
  if (OP_glue (op))               fprintf (f, " glue");
  if (OP_no_alias (op))           fprintf (f, " noalias");
  if (OP_copy (op))               fprintf (f, " copy");
  if (OP_volatile (op))           fprintf (f, " volatile");
  if (OP_side_effects (op))       fprintf (f, " side_effects");
  if (OP_hoisted (op))            fprintf (f, " hoisted");
  if (OP_cond_def (op))           fprintf (f, " cond_def");
  if (OP_end_group (op))          fprintf (f, " end_group");
  if (OP_tail_call (op))          fprintf (f, " tail_call");
  if (OP_no_move_before_gra (op)) fprintf (f, " no_move");
  if (OP_spadjust_plus (op))      fprintf (f, " spadjust_plus");
  if (OP_spadjust_minus (op))     fprintf (f, " spadjust_minus");
#ifdef TARG_ST
  if (OP_spill(op)) fprintf (f, " spill");
  if (OP_ssa_move(op)) fprintf (f, " ssa_move");
  if (OP_black_hole(op)) fprintf (f, " black_hole");
  if (OP_Get_Flag_Effects(op) & OP_FE_WRITE) fprintf (f, " flag_write");
  if (OP_Get_Flag_Effects(op) & OP_FE_READ) fprintf (f, " flag_read");
  if (OP_prologue(op)) fprintf (f, " prologue");
  if (OP_epilogue(op)) fprintf (f, " epilogue");
#endif

  if (wn = Get_WN_From_Memory_OP (op)) {
    char buf[500];
    buf[0] = '\0';
    if (Alias_Manager) Print_alias_info (buf, Alias_Manager, wn);
#ifdef KEY
    fprintf (f, " WN %s", buf);
#else
    fprintf (f, " WN=0x%p %s", wn, buf);
#endif
  }
#ifdef TARG_ST
  if (OP_Get_Affirm(op)) {
    fprintf(TFile, " affirm");
  }
#endif
  if (OP_unrolling (op)) {
    UINT16 unr = OP_unrolling (op);
    fprintf (f, " %d%s unrolling", unr,
	    unr == 1 ? "st" : unr == 2 ? "nd" : unr == 3 ? "rd" : "th");
  }
  fprintf (f, "\n");
}

void Print_OP (const OP *op, FILE *f) {
  Print_Src_Line (OP_srcpos (op), f);
  Print_OP_No_SrcLine (op, f);
}

void Print_OPs (const OP *op, FILE *f) {
  for ( ; op; op = OP_next (op)) {
    Print_OP (op, f);
  }
}

void Print_OPS (const OPS *ops, FILE *f) {
  OP *op;
  FOR_ALL_OPS_OPs_FWD (ops, op) {
    Print_OP (op, f);
  }
}

void Print_OPs_No_SrcLines (const OP *op, FILE *f) {
  for ( ; op; op = OP_next (op)) {
    Print_OP_No_SrcLine (op, f);
  }
}

void Print_OPS_No_SrcLines (const OPS *ops, FILE *f){
  OP *op;
  FOR_ALL_OPS_OPs_FWD (ops, op) {
    Print_OP_No_SrcLine (op, f);
  }
}


/* ====================================================================
 *
 * OP_Defs_Reg
 *
 * See interface description.
 *
 * ====================================================================
 */

BOOL
OP_Defs_Reg(const OP *op, ISA_REGISTER_CLASS cl, REGISTER reg)
{
  register INT num;

  for ( num = 0; num < OP_results(op); num++ ) {
    TN *res_tn = OP_result(op,num);
    if (TN_is_register(res_tn)) {
      if (TN_register_class(res_tn) == cl && TN_register(res_tn) == reg ) {
	return TRUE;
      }
    }
  }

  /* if we made it here, we must not have found it */
  return FALSE;
}

#ifdef TARG_ST
/* ====================================================================
 *
 * OP_Defs_Regs
 *
 * See interface description.
 *
 * ====================================================================
 */

INT
OP_Defs_Regs(const OP *op, ISA_REGISTER_CLASS cl, REGISTER reg, INT nregs)
{
  INT low_reg = -1, high_reg = -1;
  register INT num;

  for ( num = 0; num < OP_results(op); num++ ) {
    TN *res_tn = OP_result(op,num);
    if (TN_is_register(res_tn)) {
      if (TN_register_class(res_tn) == cl) {
        REGISTER r = TN_register(res_tn);
        if (r != REGISTER_UNDEFINED) {
          INT nr = TN_nhardregs(res_tn);
          if ((r <= reg && (r + nr) > reg) ||
              (reg <= r && (reg + nregs) > r)) {
            // We have an overlap
            INT lo = (reg > r) ? reg : r;
            INT hi = (reg + nregs - 1) < (r + nr - 1)
              ? (reg + nregs - 1)
	      : (r + nr -1);
            if (lo < low_reg) low_reg = lo;
            if (hi > high_reg) high_reg = hi;
	  }
	}
      }
    }
  }

  return (low_reg == -1) ? 0 : high_reg - low_reg;
}


/* ====================================================================
 *
 * OP_Refs_Regs
 *
 * See interface description.
 *
 * ====================================================================
 */

INT
OP_Refs_Regs(const OP *op, ISA_REGISTER_CLASS cl, REGISTER reg, INT nregs)
{
  REGISTER_SET regs = REGISTER_SET_Range (reg, reg + nregs - 1);
  REGISTER_SET referenced_regs = REGISTER_SET_EMPTY_SET;
  INT low_reg = -1, high_reg = -1;
  register INT num;

  for ( num = 0; num < OP_opnds(op); num++ ) {
    TN *opnd_tn = OP_opnd(op,num);
    if (TN_is_register(opnd_tn)) {
      if (TN_register_class(opnd_tn) == cl) {
	referenced_regs = REGISTER_SET_Union
	  (referenced_regs,
	   REGISTER_SET_Intersection (TN_registers (opnd_tn), regs));
      }
    }
  }

  return REGISTER_SET_Size (referenced_regs);
}
#endif

/* ====================================================================
 *
 * OP_Refs_Reg
 *
 * See interface description.
 *
 * ====================================================================
 */

BOOL
OP_Refs_Reg(const OP *op, ISA_REGISTER_CLASS cl, REGISTER reg)
{
  register INT num;

  for ( num = 0; num < OP_opnds(op); num++ ) {
    TN *opnd_tn = OP_opnd(op,num);
    if (TN_is_register(opnd_tn)) {
      if (TN_register_class(opnd_tn) == cl && TN_register(opnd_tn) == reg ) {
	return TRUE;
      }
    }
  }

#ifdef KEY
  if( OP_cond_def( op ) ){
    for ( num = 0; num < OP_results(op); num++ ) {
      TN* result_tn = OP_result( op, num );
      if (TN_is_register(result_tn)          &&
	  TN_register_class(result_tn) == cl &&
	  TN_register(result_tn) == reg ) {
	return TRUE;
      }      
    }
  }
#endif

  /* if we made it here, we must not have found it */
  return FALSE;
}


/* ====================================================================
 *
 * OP_Defs_TN
 *
 * See interface description.
 *
 * ====================================================================
 */

BOOL
OP_Defs_TN(const OP *op, const struct tn *res)
{
  register INT num;

  for ( num = 0; num < OP_results(op); num++ ) {
    if ( OP_result(op,num) == res ) {
      return( TRUE );
    }
  }

  /* if we made it here, we must not have found it */
  return( FALSE );
}


/* ====================================================================
 *
 * OP_Refs_TN
 *
 * See interface description.
 *
 * ====================================================================
 */

BOOL
OP_Refs_TN( const OP *op, const struct tn *opnd )
{
  register INT16 num;

  for ( num = 0; num < OP_opnds(op); num++ ) {
    if ( OP_opnd(op,num) == opnd ) {
      return( TRUE );
    }
  }

#ifdef KEY
  if( OP_cond_def( op ) ){
    for ( num = 0; num < OP_results(op); num++ ) {
      if( OP_result( op, num ) == opnd )
	return TRUE;
    }
  }
#endif

  /* if we made it here, we must not have found it */
  return( FALSE );
}


/* ====================================================================
 *
 * OP_Real_Ops - How many ops does this op really represent, i.e. will
 * be emitted.
 *
 * ====================================================================
 */

INT16
OP_Real_Ops( const OP *op )
{
  if ( op == NULL || OP_dummy(op) ) {
    return 0;
  }
  else if ( OP_simulated(op) ) {
    return Simulated_Op_Real_Ops (op);
  }
  return 1;
}

#ifdef TARG_ST
/* ====================================================================
 *
 * OP_Find_TN_Def_In_BB - see interface.
 *
 * ====================================================================
 */
OP *
OP_Find_TN_Def_In_BB(const OP *op, TN *tn) {
  if (!TN_is_register(tn)) {
    return NULL;
  }
  
  if (TN_register(tn) != REGISTER_UNDEFINED) {
    // Search register definer
    ISA_REGISTER_CLASS rc = TN_register_class(tn);
    REGISTER r = TN_register(tn);
    for (OP *tmp = OP_prev(op); tmp != NULL; tmp = OP_prev(tmp)) {
      if (OP_Defs_Reg (tmp, rc, r)) return tmp;
    }
  }
  else {
    // Search TN definer
    for (OP *tmp = OP_prev(op); tmp != NULL; tmp = OP_prev(tmp)) {
      if (OP_Defs_TN (tmp, tn)) return tmp;
    }
  }
  return NULL;
}

/* ====================================================================
 *
 * OP_opnd_is_multi - see interface.
 *
 * ====================================================================
 */

BOOL
OP_opnd_is_multi(const OP *op, INT opnd)
{
  const ISA_OPERAND_INFO *info = ISA_OPERAND_Info (OP_code(op));
  INT nb_opnd = ISA_OPERAND_INFO_Operands (info);
  return (   ( opnd    < nb_opnd && ISA_OPERAND_INFO_Use (info, opnd  ) & OU_multi)
          || ((opnd+1) < nb_opnd && ISA_OPERAND_INFO_Use (info, opnd+1) & OU_multi));
}

/* ====================================================================
 *
 * OP_result_is_multi - see interface.
 *
 * ====================================================================
 */

BOOL
OP_result_is_multi(const OP *op, INT result)
{
  const ISA_OPERAND_INFO *info = ISA_OPERAND_Info (OP_code(op));
  INT nb_res = ISA_OPERAND_INFO_Results (info);
  return (   ( result    < nb_res && ISA_OPERAND_INFO_Def (info, result  ) & OU_multi)
	  || ((result+1) < nb_res && ISA_OPERAND_INFO_Def (info, result+1) & OU_multi));
}
#endif

/* ====================================================================
 *
 * OP_Real_Inst_Words - How many instruction words does this op really 
 * represent, i.e. will be emitted.
 *
 * ====================================================================
 */

INT
OP_Real_Inst_Words( const OP *op )
{
  if ( op == NULL || OP_dummy(op) ) {
    return 0;
  }
  else if ( OP_simulated(op) ) {
    return Simulated_Op_Real_Inst_Words (op);
  }
  return OP_inst_words(op);
}

/* ====================================================================
 *
 * OP_Real_Unit_Slots - How many Unit slots does this op really 
 * represent, i.e. will be emitted.
 *
 * ====================================================================
 */
#ifdef TARG_ST
INT
OP_Real_Unit_Slots( const OP *op )
{
  if ( op == NULL || OP_dummy(op) ) {
    return 0;
  }
  else if ( OP_simulated(op) ) {
    return Simulated_Op_Real_Inst_Words (op);
  }
  return OP_unit_slots(op);
}
#endif
/* ====================================================================
 *
 * OP_Is_Float_Mem - Is OP a floating point memory operation?
 *
 * ====================================================================
 */

BOOL
OP_Is_Float_Mem( const OP *op )
{
  return (OP_load(op) && TN_is_float(OP_result(op, 0))) ||
	 (OP_store(op) && TN_is_float(OP_opnd(op, 0)));
}

/* ====================================================================
 *
 * OP_Alloca_Barrier - Is OP a alloca barrier node with alias info?
 *
 * ====================================================================
 */

BOOL
OP_Alloca_Barrier(OP *op )
{
  return (OP_code(op) == TOP_spadjust && Get_WN_From_Memory_OP(op));
}

// =======================================================================
// Is_Delay_Slot_Op
// Return TRUE if the <op> is the right type to put into a delay slot of
// <xfer_op>.
// =======================================================================
BOOL 
Is_Delay_Slot_Op (OP *xfer_op, OP *op)
{
  if (op == NULL || OP_xfer(op) || OP_Real_Ops(op) != 1) return FALSE;

#ifndef KEY
  // R10k chip bug workaround: Avoid placing integer mult/div in delay 
  // slots of unconditional branches. (see pv516598) for more details.
  if (xfer_op && OP_uncond(xfer_op) &&
      (OP_imul(op) || OP_idiv(op))) return FALSE;
#endif

#ifdef TARG_MIPS
  if (!CGTARG_Is_Delay_Slot_Op(op))
    return FALSE;
#endif

  // TODO: do we need the following restriction ?
  if (OP_has_hazard(op) || OP_has_implicit_interactions(op))
    return FALSE;
  return TRUE;
}



// Debugging routine
void dump_op (const OP *op)
{
   FILE *f;
   f = TFile;
   Set_Trace_File_internal(stdout);
   Print_OP_No_SrcLine(op);
   Set_Trace_File_internal(f);
}


/* ====================================================================
 *
 * OP_cond_def
 *
 * Return TRUE if the OP conditionally modifies some of the results.
 *
 * ====================================================================
 */

BOOL OP_cond_def(const OP *op) 
{
  return OP_cond_def_kind(op) == OP_ALWAYS_COND_DEF ||
    ((OP_cond_def_kind(op) == OP_PREDICATED_DEF) &&
#ifdef TARG_ST
  /* (cbr) predicate operand # is not necessary constant */
     !TN_is_true_pred(OP_opnd(op, OP_find_opnd_use(op, OU_predicate))));
#else
     !TN_is_true_pred(OP_opnd(op, OP_PREDICATE_OPND)));
#endif
}

/* ====================================================================
 *
 * OP_has_implicit_interactions
 *
 * Return TRUE if the OP has some implicit interaction properties with
 * other OPs in a non-obvious way.
 *
 * ====================================================================
 */

BOOL OP_has_implicit_interactions(OP *op) 
{
  if (OP_volatile(op) || OP_side_effects(op)
#ifdef TARG_ST
      || OP_Has_Flag_Effect(op)
      || OP_Is_Barrier(op)
#endif
      )
    return TRUE;

  INT i;
  for (i = 0; i < OP_opnds(op); i++) {
    TN *opnd_tn = OP_opnd(op, i);
    if (TN_is_tag(opnd_tn)) return TRUE;
  }

  return FALSE;
}

/* ====================================================================
 *
 * OP_Base_Offset_TNs
 *
 * Return the base and offset TNs for the given memory OP.
 *
 * ====================================================================
 */
void OP_Base_Offset_TNs(OP *memop, TN **base_tn, TN **offset_tn)
{
#ifdef TARG_X8664
  Is_True(OP_load(memop) || OP_load_exe(memop) || OP_store(memop), ("not a load or store"));
#else
  Is_True(OP_load(memop) || OP_store(memop), ("not a load or store"));
#endif

  INT offset_num = OP_find_opnd_use (memop, OU_offset);
  INT base_num   = OP_find_opnd_use (memop, OU_base);

  *offset_tn = NULL;

  *base_tn = base_num >= 0 ? OP_opnd(memop, base_num) : NULL;

  // <offset> TNs are not part of <memop>. Find the definining OP_iadd
  // instruction which sets the offset and matches the base_tn.

  if (offset_num < 0) {

    DEF_KIND kind;
    OP *defop = TN_Reaching_Value_At_Op(*base_tn, memop, &kind, TRUE);
    if (defop && OP_iadd(defop) && kind == VAL_KNOWN) {
      TN *defop_offset_tn = OP_opnd(defop, 1);
      TN *defop_base_tn = OP_opnd(defop, 2);
      if (defop_base_tn == *base_tn && TN_has_value(defop_base_tn)) {
	*offset_tn = defop_offset_tn;
      }
    }
  } else {
#ifdef TARG_ST
    // FdF 20060517: Support for automod addressing mode
    if (OP_find_opnd_use(memop, OU_postincr) >= 0)
      *offset_tn = Gen_Literal_TN(0, 4);
    else
#endif
    *offset_tn = OP_opnd(memop, offset_num);
  }
}
#ifdef TARG_ST
/* ====================================================================
 * Imm_Value_In_Range
 *
 * Returns whether an immediate value can be encoded at a given
 * operand in an operation.
 * ====================================================================
 */
BOOL
TOP_opnd_value_in_range (TOP top, int opnd, INT64 imm) {

  if (opnd < TOP_fixed_opnds(top)) {
    const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(top);
    const ISA_OPERAND_VALTYP *vtype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
    ISA_LIT_CLASS lc = ISA_OPERAND_VALTYP_Literal_Class(vtype);
    return ISA_LC_Value_In_Class(imm, lc);
  }
  else {
    return TRUE;
  }
} 

/* ====================================================================
 * OP_same_res()
 *
 * Handle the architecture constraint same_res.
 * Handles the result/opnd constraint on ASM statements.
 * ====================================================================
 */
extern OP_MAP OP_Asm_Map;
INT
OP_same_res(OP *op, INT i) {
  INT opnd_idx = -1;
  TOP top = OP_code(op);
  const ISA_OPERAND_INFO *oinfo;
  if (top != TOP_asm) {
    /* Check architectural description for same res. */
    oinfo = OP_operand_info(op);
    if (i < ISA_OPERAND_INFO_Results(oinfo)) {
      opnd_idx = ISA_OPERAND_INFO_Same_Res(oinfo, i); 
    }
  }
  else {
    /* check for ASM statements with same res on operand constraint. */
    ASM_OP_ANNOT* asm_info = (ASM_OP_ANNOT *)OP_MAP_Get(OP_Asm_Map, op);
    opnd_idx = ASM_OP_result_same_opnd(asm_info)[i];
  }
  return opnd_idx;
} 

#ifdef TARG_ST
BOOL
Opnds_Are_Equivalent(OP *op1, OP *op2, int idx1, int idx2)
{
  TN *tn1 = (idx1 == -1 ? True_TN : OP_opnd(op1, idx1));
  TN *tn2 = (idx2 == -1 ? True_TN : OP_opnd(op2, idx2));

  if (TNs_Are_Equivalent (tn1, tn2) &&
      OP_Pred_False (op1, idx1) == OP_Pred_False (op2, idx2))
    return TRUE;

  return FALSE;
}
#endif

/* ====================================================================
 *
 * OPs_Are_Equivalent()
 *
 * See interface description
 * ====================================================================
 */
BOOL
OPs_Are_Equivalent(OP *op1, OP *op2)
{
  if (OP_code(op1) != OP_code(op2)) return FALSE;
  for (int i = 0; i < OP_opnds(op1); i++) {
    if (TN_is_register(OP_opnd(op1, i)) &&
	TN_is_register(OP_opnd(op2, i)) &&
#ifdef TARG_ST
	!Opnds_Are_Equivalent(op1, op2, i, i))
#else
      !TNs_Are_Equivalent(OP_opnd(op1, i), OP_opnd(op2, i)))
#endif
      return FALSE;
    else if (OP_opnd(op1, i) != OP_opnd(op2, i)) return FALSE;
  }
  return TRUE;
}

/* ====================================================================
 *
 * OP_plain_load()
 *
 * See interface description
 * ====================================================================
 */
BOOL
OP_plain_load(OP *op)
{

  int offset_opnd, base_opnd;

  if (!OP_load(op)) return FALSE;
  offset_opnd = OP_find_opnd_use(op, OU_offset);
  base_opnd = OP_find_opnd_use(op, OU_base);
  if (offset_opnd < 0 || base_opnd < 0) return FALSE;
  if (OP_results(op) != 1) return FALSE;
  if (OP_opnds(op) != 2) return FALSE;
  if (OP_has_implicit_interactions(op)) return FALSE;
  if (OP_cond_def(op)) return FALSE;
  return TRUE;
}

/* ====================================================================
 *
 * OP_plain_store()
 *
 * See interface description
 * ====================================================================
 */
BOOL
OP_plain_store(OP *op)
{
  int offset_opnd, base_opnd, storeval_opnd;
  if (!OP_store(op)) return FALSE;
  offset_opnd = OP_find_opnd_use(op, OU_offset);
  base_opnd = OP_find_opnd_use(op, OU_base);
  storeval_opnd = OP_find_opnd_use(op, OU_storeval);
  if (offset_opnd < 0 || base_opnd < 0) return FALSE;
  if (OP_results(op) != 0) return FALSE;
  if (OP_opnds(op) != 3) return FALSE;
  if (OP_has_implicit_interactions(op)) return FALSE;
  if (OP_cond_def(op)) return FALSE;
  return TRUE;
}

/* ====================================================================
 *
 * OP_storeval_byte_offset
 *
 * See interface description
 * ====================================================================
 */
INT
OP_storeval_byte_offset (OP *op, INT opndno)
{
  TOP opcode = OP_code(op);
  Is_True (TOP_is_store (opcode), ("OP_storeval_byte_offset (%s)",
				   TOP_Name(opcode)));
  INT byte_offset = 0;
  INT storeval = OP_find_opnd_use (op, OU_storeval);
  Is_True (opndno >= storeval && opndno < OP_opnds (op), ("OP_storeval_byte_offset invalid opndno (%d)", opndno));
  for (INT i = storeval; i < opndno; i++) {
    byte_offset += OP_opnd_size(op, i)/8;
  }
  BOOL reversed = ((TOP_is_mem_highest_reg_first(opcode) != 0)
		   ^ (Target_Byte_Sex == BIG_ENDIAN
		      && TOP_is_mem_endian_reg_reversed(opcode)));
  if (reversed) {
    unsigned short mem_bytes = TOP_Mem_Bytes (opcode);
    byte_offset = mem_bytes - byte_offset - OP_opnd_size(op, opndno)/8;
  }
  return byte_offset;
}


/* ====================================================================
 *
 * OP_loadval_byte_offset
 *
 * See interface description
 * ====================================================================
 */
INT
OP_loadval_byte_offset (OP *op, INT resno)
{
  TOP opcode = OP_code(op);
  Is_True (TOP_is_load (opcode), ("OP_loadval_byte_offset (%s)", TOP_Name(opcode)));
  Is_True (resno < OP_results(op), ("OP_loadval_byte_offset invalid resno (%d)", resno));
  INT byte_offset = 0;
  INT postincr_result = TOP_Find_Result_With_Usage (opcode, OU_postincr);
  INT preincr_result = TOP_Find_Result_With_Usage (opcode, OU_preincr);
  for (INT i = 0; i < resno; i++) {
    if (i != postincr_result && i != preincr_result) {
      byte_offset += OP_result_size (op, i) / 8;
    }
  }
  BOOL reversed = ((TOP_is_mem_highest_reg_first(opcode) != 0)
		   ^ (Target_Byte_Sex == BIG_ENDIAN
		      && TOP_is_mem_endian_reg_reversed(opcode)));
  if (reversed) {
    unsigned short mem_bytes = TOP_Mem_Bytes (opcode);
    byte_offset = mem_bytes - byte_offset - OP_result_size (op, resno)/8;
  }
  return byte_offset;
}

// FdF: Returns 0 if op is not an auto-mod operation, of if tn is not
// the auto-modified register, or is redefined also by the operation.
// Otherwise, return -1 for a pre-automod, and 1 for a post-automod.
INT
TN_isAutoMod(OP *op, TN *tn) {

  INT isAutoMod;
  INT opndAutoMod_idx;

  // Check this op is an auto-mod operation
  if ((opndAutoMod_idx = OP_find_opnd_use(op, OU_preincr)) != -1)
    isAutoMod = -1;
  else if ((opndAutoMod_idx = OP_find_opnd_use(op, OU_postincr)) != -1)
    isAutoMod = 1;
  else
    return 0;

  // tn is not the auto-mod operand
  if (OP_opnd(op, opndAutoMod_idx) != tn)
    return 0;

  // Check that tn is only defined as the auto-mode result.
  for (INT res_idx = 0; res_idx < OP_results(op); res_idx++) {
    if ((OP_result(op, res_idx) == tn) &&
	(OP_same_res(op, res_idx) != opndAutoMod_idx))
      return 0;
  }

  return isAutoMod;
}

/* ====================================================================
 *
 * OP_load_result
 *
 * See interface description
 * ====================================================================
 */
INT
OP_load_result (OP *op)
{
  TOP opcode = OP_code(op);
  Is_True (TOP_is_load (opcode), ("OP_loadval (%s)", TOP_Name(opcode)));
  if (OP_results(op) == 1) {
    // Common case
    return 0;
  }
  const ISA_OPERAND_INFO *info = ISA_OPERAND_Info (opcode);
  if (OP_results (op) > ISA_OPERAND_INFO_Results (info)) {
    // No way to disambiguate
    return -1;
  }

  INT candidate = -1;
  INT i;
  for (i=0; i<OP_results(op); i++) {
    if ( ! ( ( ISA_OPERAND_INFO_Def (info, i) & ( OU_multi | OU_postincr | OU_preincr ) )
             || ( ISA_OPERAND_INFO_Same_Res (info, i) != -1 ) ) ) {
      if (candidate != -1) {
        // Found more than one result candidate
        // Unable to disambiguate results
        return -1;
      }
      candidate = i;
    }
  }
  return candidate;
}
#endif

#ifdef KEY
/* ====================================================================
 *
 * TN_Pair_In_OP
 *
 * See interface description.
 *
 * ====================================================================
 */

BOOL 
TN_Pair_In_OP(OP* op, struct tn *tn_res, struct tn *tn_opnd) 
{
  INT i;
  for (i = 0; i < OP_results(op); i++) {
    if (tn_res == OP_result(op,i)) {
      break; 
    }
  }
  if (i == OP_results(op)) {
    // If tn_res has an assigned register, check if it matches a result.  (This
    // changes the semantics of TN_Pair_In_OP, but that's ok since the only
    // user of TN_Pair_In_OP is LRA, which wants this check.)  Bug 9489.
    BOOL result_match = FALSE;
    if (TN_register(tn_res) != REGISTER_UNDEFINED) {
      for (int j = 0; j < OP_results(op); j++) {
        TN *res = OP_result(op, j);
	if (TN_register_and_class(res) == TN_register_and_class(tn_res)) {
	  result_match = TRUE;
	}
      }
    }
    if (!result_match)
      return FALSE;
  }
  for (i = 0; i < OP_opnds(op); i++) {
    if (tn_opnd == OP_opnd(op,i)) {
      return TRUE; 
    }
  }
  return FALSE;
}

/* ====================================================================
 *
 * TN_Resnum_In_OP
 *
 * See interface description.
 *
 * ====================================================================
 */

INT 
TN_Resnum_In_OP (OP* op, struct tn *tn, BOOL match_assigned_reg) 
{
  for (INT i = 0; i < OP_results(op); i++) {
    TN *res = OP_result(op, i);
    if (tn == res) {
      return i;
    }

    if (match_assigned_reg &&
	TN_register(res) != REGISTER_UNDEFINED &&
	TN_register_and_class(res) == TN_register_and_class(tn)) {
      return i;
    }
  }
  FmtAssert (FALSE,
             ("TN_resnum_in_OP: Could not find <tn> in results list\n"));
  return -1;
}
#endif
