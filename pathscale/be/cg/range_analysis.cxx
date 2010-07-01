/*
  Copyright (C) 2006, STMicroelectronics, All Rights Reserved

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

/* ====================================================================
 * ====================================================================
 *
 * Module: range_analysis.cxx
 *
 * Description:
 *
 * Perform range analysis over a region for all SSA variables
 * defined in that region.
 *
 * ====================================================================
 */

#include "defs.h"
#include "tracing.h"
#include "config_target.h"
#include "cgir.h"
#include "cg_flags.h"
#include "range_analysis.h"
#include "lrange.h"
#include "range.h"
#include "cg_ssa.h"
#include "cg_affirm.h"
#include "stblock.h" // for ST_alignment
#include "data_layout.h" // for Stack_Alignment


MEM_POOL TN_Equivalence::pool;
MEM_POOL RangeAnalysis::pool;

/* ====================================================================
 *
 * Tracing functions.
 *
 *    Separated out from main code to improve readability.
 *
 * ====================================================================
 */

void
RangeAnalysis::Trace_SSA_TNs () const
{
  if (trace_forward || trace_backward) {
    fprintf (TFile, "SSA Tns:" );
    TN_SET_Print (ssa_tns, TFile);
    putc ('\n', TFile);
  }
}

void
RangeAnalysis::Trace_SetValue (const TN* tn,
			       const TN_Lattice_Info &new_value,
			       const LRange_p old_value) const
{
  if (trace_forward || trace_backward) {
    fputs ("SetValue ", TFile);
    Print_TN ((TN *)tn, FALSE);
    fputs (": ", TFile);
    new_value.value->Print (TFile);
    fputs (" <- ", TFile);
    old_value->Print (TFile);
    fprintf (TFile, " (lowered %d times)\n", new_value.lowered_count);
  }
}

void
RangeAnalysis::Trace_Recompute (BB *bb) const
{
  if (trace_forward || trace_backward) {
    fprintf (TFile, "Adding BB%d to worklist\n", BB_id(bb));
  }
}

void
RangeAnalysis::Trace_Recompute (const TN *tn) const
{
  if (trace_forward || trace_backward) {
    fprintf (TFile, "Adding TN%d to worklist\n", TN_number(tn));
  }
}

void
RangeAnalysis::Trace_Recompute (const Opnd_Info *use) const
{
  if (trace_forward || trace_backward) {
    fprintf (TFile, "Adding use (BB%d, op%d, opnd%d TN%d) to worklist\n",
	     BB_id (use->Bb()), OP_map_idx(use->Op()), use->Opnd_idx(),
	     TN_number (use->Tn()));
  }
}  

void
RangeAnalysis::Trace_Visit (OP *op) const
{
  if (trace_forward || trace_backward) {
    fprintf(TFile, "  Visiting: ");
    Print_OP_No_SrcLine (op);
  }
}

void
RangeAnalysis::Trace_Visit (BB *bb) const
{
  if (trace_forward || trace_backward) {
    fprintf (TFile, "Visiting BB%d\n", BB_id(bb));
  }
}

void
RangeAnalysis::Trace_Visit (const TN *tn) const
{
  if (trace_forward || trace_backward) {
    fputs ("Visiting ", TFile);
    Print_TN ((TN *)tn, FALSE);
    putc ('\n', TFile);
  }
}

void
RangeAnalysis::Trace_Analyze_Forward_Start () const
{
  if (trace_forward) {
    fprintf(TFile, "%s Before forward analysis\n%s\n", DBar, DBar);
    Print_All_BBs ();
    fprintf(TFile, "Beginning forward analysis\n");
  }
}

void
RangeAnalysis::Trace_Analyze_Forward_End () const
{
  if (trace_forward) {
    TN *tn;

    fprintf(TFile, "After forward analysis, ranges calculated:\n");
    FOR_ALL_TN_SET_members (ssa_tns, tn) {
      Print_TN (tn, FALSE);
      fputs (": ", TFile);
      Get_Value(tn)->Print (TFile);
      putc ('\n', TFile);
    }
  }
}

void
RangeAnalysis::Trace_Analyze_Backward_Start () const
{
  if (trace_backward) {
    fprintf(TFile, "Beginning backward analysis\n");
  }
}

void
RangeAnalysis::Trace_Analyze_Backward_End () const
{
  if (trace_backward) {
    TN *tn;

    fprintf(TFile, "After backward analysis, ranges calculated:\n");
    FOR_ALL_TN_SET_members (ssa_tns, tn) {
      Print_TN (tn, FALSE);
      fputs (": ", TFile);
      Get_Value_Backward(tn)->Print (TFile);
      putc ('\n', TFile);
    }
  }
}

void
TN_Equivalence::Trace_Equiv_Class_Merge (const TN *tn1, INT reg1,
					 const TN *tn2, INT reg2) const
{
  if (trace_equivalence) {
    fprintf (TFile, "Merge equivalence: { TN%d, r%d } and { TN%d, r%d }\n",
	     TN_number (tn1), reg1, TN_number (tn2), reg2);
  }
}

/* ====================================================================
 *
 * EquivClass implementation.
 *
 * ====================================================================
 */

EquivClass *
EquivClass::Find()
{
  EquivClass *root;
  for (root = this; root->parent != NULL; root = root->parent)
    ;

  EquivClass *pref;
  EquivClass *next_parent;
  for (pref = this, next_parent = pref->parent;
       next_parent != NULL;
       pref = next_parent, next_parent = next_parent->parent) {
    pref->parent = root;
  }

  return root;
}

/* ====================================================================
 *
 * TN_Equivalence implementation.
 *
 * ====================================================================
 */

EquivClass *
TN_Equivalence::Equivalence_Class (const TN *tn, INT reg)
{
  INT nregs = TN_nhardregs (tn);
  EquivClass *e = (EquivClass *)TN_MAP_Get (tn_equivalences, tn);
  if (! e) {
    e = TYPE_MEM_POOL_ALLOC_N (EquivClass, &pool, nregs);
    for (INT r = 0; r < nregs; r++) {
      EquivClass ec (equiv_class_id++);
      e[r] = ec;
    }
    TN_MAP_Set (tn_equivalences, (TN *)tn, e);
  }
  return &e[reg];
}

void
TN_Equivalence::Merge_Equivalence_Classes (const TN *tn1, INT reg1,
					   const TN *tn2, INT reg2)
{
  EquivClass *e1 = Equivalence_Class (tn1, reg1);
  EquivClass *e2 = Equivalence_Class (tn2, reg2);

  e1->Merge (e2);

  Trace_Equiv_Class_Merge (tn1, reg1, tn2, reg2);
}

void
TN_Equivalence::Build_Equivalence_Classes ()
{
  // Build equivalence sets of SSA tns.
  // Two TNs in the same equivalence set are guaranteed to
  // contain the same value.

  equiv_class_id = 0;
  tn_equivalences = TN_MAP_Create ();

  // Create an equivalence class for each SSA TN.
  for (TN_NUM num = First_Regular_TN; num <= Last_TN; num++) {
    TN *tn = TNvec(num);
    OP *op = TN_ssa_def (tn);
    if (op) {
      for (INT i = 0; i < TN_nhardregs (tn); i++) {
	Equivalence_Class (tn, i);
      }
    }
  }

  // Merge equivalence classes for src/dst of copies.
  for (BB *bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    OP *op;
    FOR_ALL_BB_OPs_FWD (bb, op) {
      if (OP_copy (op)) {
	const TN *dst = OP_Copy_Result_TN (op);
	const TN *src = OP_Copy_Operand_TN (op);
	if (TN_is_ssa_var (dst) && TN_is_ssa_var (src)) {
	  for (INT regno = 0; regno < TN_nhardregs (dst); regno++) {
	    Merge_Equivalence_Classes (dst, regno, src, regno);
	  }
	}
      } else if (OP_extract (op)) {
	INT src_regno = 0;
	const TN *src = OP_opnd (op, 0);
	if (TN_is_ssa_var (src)) {
	  for (INT resultnum = 0; resultnum < OP_results (op); resultnum++) {
	    TN *dst = OP_result (op, resultnum);
	    if (TN_is_ssa_var (dst)) {
	      for (INT i = 0; i < TN_nhardregs (dst); i++) {
		Merge_Equivalence_Classes (dst, i, src, src_regno);
		src_regno++;
	      }
	    }
	  }
	}
      } else if (OP_compose (op)) {
	INT dst_regno = 0;
	const TN *dst = OP_result (op, 0);
	if (TN_is_ssa_var (dst)) {
	  for (INT opndnum = 0; opndnum < OP_opnds (op); opndnum++) {
	    TN *src = OP_opnd (op, opndnum);
	    if (TN_is_ssa_var (src)) {
	      for (INT i = 0; i < TN_nhardregs (src); i++) {
		Merge_Equivalence_Classes (dst, dst_regno, src, i);
		dst_regno++;
	      }
	    }
	  }
	}
      }
    }
  }
  // Create a mapping from TN to root TN of equiv. class.
  equiv_class_root = TYPE_MEM_POOL_ALLOC_N (const TN *, &pool, equiv_class_id);
  for (INT i = 0; i < equiv_class_id; i++) {
    equiv_class_root[i] = NULL;
  }
  for (TN_NUM num = First_Regular_TN; num <= Last_TN; num++) {
    TN *tn = TNvec(num);
    OP *op = TN_ssa_def (tn);
    if (op && ! OP_copy (op) && ! OP_extract (op) && ! OP_compose (op)) {
      for (INT i = 0; i < TN_nhardregs (tn); i++) {
	EquivClass *e = Equivalence_Class (tn, i)->Find ();
	equiv_class_root[e->Id ()] = tn;
      }
    }
  }
}

BOOL
TN_Equivalence::Equivalent (const TN *tn1, INT reg1,
			    const TN *tn2, INT reg2) const
{
  if (tn1 == tn2 && reg1 == reg2) return TRUE;

  EquivClass *e1 = (EquivClass *)TN_MAP_Get (tn_equivalences, tn1);
  EquivClass *e2 = (EquivClass *)TN_MAP_Get (tn_equivalences, tn2);
  
  if (! e1 || ! e2) return FALSE;

  return e1[reg1].Equivalent (e2[reg2]);
}

BOOL
TN_Equivalence::Equivalent (const TN *tn1, const TN *tn2) const
{
  if (tn1 == tn2) return TRUE;
    
  INT nregs = TN_nhardregs (tn1);
  
  if (TN_nhardregs (tn2) == nregs) {
    for (INT i = 0; i < nregs; i++) {
      if (!Equivalent (tn1, i, tn2, i))
	return FALSE;
    }
    return TRUE;
  }

  return FALSE;
}

/* ====================================================================
 *
 * Support functions.
 *
 * ====================================================================
 */

static inline BB *
SSA_BB (const TN *tn)
{
  OP *op = (OP *)TN_MAP_Get (tn_ssa_map, tn);
  FmtAssert (op != NULL, ("Missing OP for SSA tn"));

  return OP_bb(op);
}

static LRange_p
Fit_To_TN (const LRange_p a, const TN *tn, BOOL isBackward)
{
  // When held in a TN, we represent a
  // range as a signed range with the width of the
  // TN; except for TNs narrower than 8 bits,
  // which are represented unsigned.
  // This is an arbitrary choice, but it is
  // useful to have a convention for the signedness
  // of a TN range.
  INT w = TN_bitwidth (tn);
  if (w < 8 || isBackward)
    return MakeUnsigned (a, w);
  else
    return MakeSigned (a, w);
}

OP_LIST *
RangeAnalysis::Uses (const TN *tn) const
{
  return (OP_LIST *) TN_MAP_Get (tn_uses, tn);
}

void
RangeAnalysis::Add_Use (const TN *tn, OP *op)
{
  OP_LIST *uses = Uses (tn);

  uses = OP_LIST_Push (op, uses, &pool);

  TN_MAP_Set (tn_uses, (TN *)tn, uses);
}

void
RangeAnalysis::Initialize_TN_Uses ()
{
  BB *bb;
  OP *op;

  tn_uses = TN_MAP_Create ();
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    FOR_ALL_BB_OPs_FWD (bb, op) {
      for (INT i = 0; i < OP_opnds(op); i++) {
	TN *opnd = OP_opnd(op, i);
	if (TN_is_ssa_var (opnd))
	  Add_Use (opnd, op);
      }
      FmtAssert (OP_bb (op) == bb, ("Op has wrong bb (is BB %d should be BB%d)\n",
				    BB_id (OP_bb (op)), BB_id (bb)));
				    
    }
  }
}

void
RangeAnalysis::Initialize_SSA_TNs ()
{
  TN_NUM num;

  ssa_tns = TN_SET_Create (Last_TN, &pool);

  for (num = First_Regular_TN; num <= Last_TN; num++) {
    TN *tn = TNvec(num);
    if (TN_is_ssa_var (tn)) {
      ssa_tns = TN_SET_Union1D (ssa_tns, tn, &pool);
    }
  }

  Trace_SSA_TNs ();
}

static int
compare_topological_order (const void *v1, const void *v2)
{
  // We want to return -1 if val1 is before val2 in topological
  // order.
  // val1 and val2 are in post-depth-first order, so
  // reverse them to get topological order.
  INT32 val1 = *(const INT32 *)v1;
  INT32 val2 = *(const INT32 *)v2;
  return val2 - val1;
}

void
RangeAnalysis::Create_Topological_Ordering ()
{
  // Problem: BB_Topological_Map does not work on
  // irreducible graphs, so use BB_Postorder_Map, which
  // gives post-depth-first order.
  BB_MAP dfo_map = BB_Postorder_Map (NULL, NULL);
  BB *bb;
  INT tot_successors = 0;
  INT max_successors = 0;
  
  topological_order = TYPE_MEM_POOL_ALLOC_N (BB *, &pool, PU_BB_Count + 1);
  // Here we first set up topological_order to be
  // post-depth-first order (which is reverse topological)
  n_topological_order = 0;
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next (bb)) {
    INT32 bb_topo_id = BB_MAP32_Get (dfo_map, bb) - 1;
    if (bb_topo_id >= 0) {
      Reset_BB_unreachable(bb);
      topological_order[bb_topo_id] = bb;
      if (n_topological_order < bb_topo_id) {
	n_topological_order = bb_topo_id;
      }
    } else {
      Set_BB_unreachable(bb);
    }
    if (BB_handler (bb)) {
      handler_seen = TRUE;
    }
    INT n_succs = BB_succs_len (bb);
    if (max_successors < n_succs) max_successors = n_succs;
    tot_successors += BB_succs_len (bb);
  }
  if (trace_forward) {
    fprintf (TFile, "BB_Postorder_Map returned:");
    for (INT i = 0; i <= n_topological_order; i++) {
      fprintf (TFile, " BB%d", (int)BB_id(topological_order[i]));
    }
    fprintf (TFile, "\n");
  }
  // Now reverse the ordering, to get topological.
  {
    INT x, y;
    for (x = 0, y = n_topological_order;
	 x < y;
	 x++, y--) {
      BB *tmp = topological_order[x];
      topological_order[x] = topological_order[y];
      topological_order[y] = tmp;
    }
  }
  if (trace_forward) {
    fprintf (TFile, "Topological order is:");
    for (INT i = 0; i <= n_topological_order; i++) {
      fprintf (TFile, " BB%d", (int)BB_id(topological_order[i]));
    }
    fprintf (TFile, "\n");
  }
	     
  // Also for each BB, we want to list its successors in
  // topological order.
  BB **BB_succs = TYPE_MEM_POOL_ALLOC_N (BB *, &pool, tot_successors);
  INT32 succ_ix = 0;
  INT32 succs [max_successors];
  ordered_succs = BB_MAP_Create ();
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next (bb)) {
    BBLIST *edge;
    INT n_succs = 0;

    FOR_ALL_BB_SUCCS (bb, edge) {
      succs[n_succs++] = BB_MAP32_Get (dfo_map, BBLIST_item (edge));
    }
    qsort (succs, n_succs, sizeof (INT32), compare_topological_order);
    BB_MAP_Set (ordered_succs, bb, BB_succs + succ_ix);
    for (INT i = 0; i < n_succs; i++) {
      if (succs[i] == 0) {
	BB_succs[succ_ix++] = NULL;
      } else {
	// Care: we have reversed the topological_order[] array.
	INT32 bb_topo_id = n_topological_order - (succs[i] - 1);
	BB_succs[succ_ix++] = topological_order[bb_topo_id];
      }
    }
    if (trace_forward) {
      fprintf (TFile, "Topological successors of BB%d are:", (int)BB_id(bb));
      for (INT i = 0; i < n_succs; i++) {
	BB *succ = BB_ordered_succ (bb, i);
	if (succ == NULL) {
	  fprintf (TFile, " <null>");
	} else {
	  fprintf (TFile, " BB%d", BB_id(succ));
	}
      }
      fprintf (TFile, "\n");
    }
  }

  BB_MAP_Delete (dfo_map);
}

void
RangeAnalysis::Initialize (LRange_p instance, MEM_POOL mempool)
{
//   MEM_POOL_Initialize (&pool, "range analysis pool", TRUE);
  pool = mempool;
  Set_Error_Phase ("Range Analysis");
  trace_forward = Get_Trace(TP_RANGE, RANGE_ANALYSIS_FORWARD);
  trace_backward = Get_Trace(TP_RANGE,RANGE_ANALYSIS_BACKWARD);
  tn_equivalence.SetTracing (Get_Trace(TP_RANGE,RANGE_ANALYSIS_EQUIVALENCE));

//   lattice = new Lattice<Range> (Union, Intersect,
// 				Range::Empty (), Range::Universe ());

  lattice = instance;

  Initialize_SSA_TNs ();
  handler_seen = FALSE;
  tn_value_forward = NULL;
  tn_value_backward = NULL;
  ordered_succs = NULL;
  Create_Topological_Ordering ();
  executed = BB_SET_Create_Empty (PU_BB_Count, &pool);
  visited_backward = BB_SET_Create_Empty (PU_BB_Count, &pool);
  Initialize_TN_Uses ();
  tn_equivalence.Build_Equivalence_Classes ();
  analysis = None;
  forward_valid = FALSE;
  backward_valid = FALSE;
}

void
RangeAnalysis::Initialize_Backward (LRange_p instance)
{
  lattice = instance;
}

void
RangeAnalysis::Finalize ()
{
  forward_valid = FALSE;
  backward_valid = FALSE;
  if (tn_value_forward) TN_MAP_Delete (tn_value_forward);
  if (tn_value_backward) TN_MAP_Delete (tn_value_backward);
  if (ordered_succs) BB_MAP_Delete (ordered_succs);

//   delete lattice;
//   MEM_POOL_Delete (&pool);
}



/* ====================================================================
 *
 * Routines used for both forward and backward analysis.
 *
 * ====================================================================
 */

void
RangeAnalysis::SetValue (const TN *tn, LRange_p value)
{
  LRange_p old_value = lattice->makeTop ();

  TN_Lattice_Info *v = (TN_Lattice_Info *)TN_MAP_Get (tn_value, tn);
  if (! v) {
    v = TYPE_MEM_POOL_ALLOC (TN_Lattice_Info, &pool);
    TN_MAP_Set (tn_value, (TN *)tn, v);
    v->value = old_value;
    v->lowered_count = 0;
  } else {
    old_value = v->value;
  }

  v->value = value;

  Trace_SetValue (tn, *v, old_value);
}

LRange_p
RangeAnalysis::Value (const TN *tn)
{
  // [SC]: I choose to put all register TNs in the
  // tn_value map, even those which are dedicated,
  // and thus are not tracked (TN_is_ssa_var() is false).
  // This may make the map larger,
  // but means I can avoid the TN_is_ssa_var() test
  // before the lookup here.
  // The lookup is simply an index into an
  // array, so it will not be made slower as the map
  // gets larger.
  // It is an efficiency trade-off,
  // if the map lookup becomes slow, then we could
  // weed out non-SSA-var tns before the look up, because
  // they always have a constant value.
  LRange_p result;
  if (tn == SP_TN || tn == FP_TN) {
    result = lattice->makeAlign (Stack_Alignment(), 0);
  }
  else if (TN_is_register (tn)) {

    TN_Lattice_Info *v = (TN_Lattice_Info *)TN_MAP_Get (tn_value, tn);

    if (! v) {
      if (TN_is_ssa_var(tn)) {
	result = lattice->makeTop ();
      } else if (TN_is_zero_reg (tn) && (analysis == Forward)) {
	result = lattice->makeRangeValue (0);
      } else {
	result = lattice->makeBottom ();
      }
      if (analysis == Forward)
	result = lattice->makeJoin (result,
				    lattice->makeRangeWidth(Range::Signed, TN_bitwidth (tn)));
      SetValue (tn, result);
    } else {
      result = v->value;
    }
  } else if (TN_has_value (tn)) {
    result = lattice->makeRangeValue (TN_value(tn));
  } else if (TN_is_symbol (tn)) {
    result = lattice->makeAlign (ST_alignment(TN_var(tn)), TN_offset(tn));
  } else {
    result = lattice->makeBottom ();
  }

  return result;
}

INT
RangeAnalysis::LoweredCount (const TN *tn)
{
  TN_Lattice_Info *v = (TN_Lattice_Info *)TN_MAP_Get (tn_value, tn);
  return v ? v->lowered_count : 0;
}

INT
RangeAnalysis::IncrementLoweredCount (const TN *tn)
{
  TN_Lattice_Info *v = (TN_Lattice_Info *)TN_MAP_Get (tn_value, tn);
  
  if (!v)
    return 0;
  else
    return ++v->lowered_count;
}

/* ====================================================================
 *
 * Forward analysis.
 *
 * ====================================================================
 */

void
RangeAnalysis::Recompute_Forward (const TN *tn, LRange_p &new_value)
{
  const LRange_p old_value = Value (tn);
  
  if (LoweredCount (tn) > CG_range_recompute_limit) {
      new_value = Fit_To_TN (lattice->makeBottom (), tn, 0);
  }
  if (new_value->StrictlyContains (old_value)) {
    SetValue (tn, new_value);
    IncrementLoweredCount (tn);
    Trace_Recompute (tn);
    tn_work.push (tn);
  } else if (analysis != Backward)
    FmtAssert (new_value->Equal (old_value),
	       ("Value raised in recompute (TN %d)", TN_number (tn)));
}

void
RangeAnalysis::Recompute_Forward (BB *bb)
{
  if (!isExecuted(bb)) {
    SetExecuted (bb);
    Trace_Recompute (bb);
    bb_work.push (bb);
    // As a side-effect build an array of BBs in reached order.
  }
}

void
RangeAnalysis::Visit_Forward (OP *op, BOOL *succs_done)
{
  // Determine width of result from op behaviour
  // and width of operands.

  FmtAssert (isExecuted (OP_bb(op)), ("Visit_Forward of op in unexecuted BB"));

  TOP opcode = OP_code(op);

  Trace_Visit (op);

  for (INT i = 0; i < OP_results(op); i++) {
    TN *result = OP_result(op, i);
    INT result_width = TN_bitwidth (result);
    LRange_p new_value = lattice->makeBottom();

    if (!TN_is_ssa_var(result)) continue;
    
    if (TARG_Visit_Forward (op, i, new_value, succs_done))
      ;
    else if (OP_copy(op) && i == OP_Copy_Result(op)) {

      // Copy
      // Result gets value range of operand.

      TN *source = OP_opnd(op, OP_Copy_Operand(op));
      new_value = Value (source);
    } else if (OP_move(op)) {

      // Move
      // Result gets value of operand 0.
      TN *operand = OP_opnd(op, 0);
      new_value = Value (operand);
    } else if (OP_load(op)) {

      // Load
      // Result gets width and signedness of load.

      // Assume that the (single) result is the loaded value.
      UINT32 width = OP_Mem_Ref_Bytes (op) * 8;
      Range::RangeSign s = (TOP_is_unsign (opcode)
			    ? Range::Unsigned : Range::Signed);
      new_value = lattice->makeRangeWidth (s, width);
    } else if (OP_sext(op) || OP_zext(op)) {

      // Sign-/Zero-extend.

      INT32 opnd_idx = OP_find_opnd_use(op, OU_opnd1);
      TN *operand = OP_opnd(op, opnd_idx);
      new_value = Value (OP_opnd(op, opnd_idx));
      UINT32 width = TOP_opnd_use_bits(opcode, opnd_idx);
      if (OP_sext(op)) {
	new_value = SignExtend (new_value, width);
      } else {
	new_value = MakeUnsigned (new_value, TN_bitwidth (operand));
	new_value = ZeroExtend (new_value, width);
      }
    } else if (OP_ishl(op) || OP_ishr(op) || OP_ishru(op)) {

      // Left- or right-shift (arithmetic or logical).

      TN *shifted_tn = OP_Opnd1(op);
      TN *shiftcount_tn = OP_Opnd2(op);
      LRange_p shifted = Value (shifted_tn);
      LRange_p shiftcount = MakeUnsigned (Value (shiftcount_tn),
				       TN_bitwidth (shiftcount_tn));
      //TDR Fix for bug #72758: in some cases, EBO may have optimized out explicit cast as operation 
	  // take only part of the register. In such cases, the range must be computed based on used bits.
   	  if(TOP_opnd_use_signed(opcode,OP_find_opnd_use(op,OU_opnd2)))
   		  shiftcount = SignExtend (shiftcount, TOP_opnd_use_bits(opcode, OP_find_opnd_use(op,OU_opnd2)));
   	  else
   		  shiftcount = ZeroExtend (shiftcount, TOP_opnd_use_bits(opcode, OP_find_opnd_use(op,OU_opnd2)));
      if (TOP_is_unsign (opcode)) {
	shifted = MakeUnsigned (shifted, TN_bitwidth (shifted_tn));
      }
      if (OP_ishl(op)) {
	new_value = LeftShift (shifted, shiftcount);
      } else {
	new_value = SignedRightShift (shifted, shiftcount);
      }
    } else if (OP_iadd(op) || OP_isub(op) || OP_imul(op)
	       || OP_iand(op) || OP_ior(op) || OP_ixor(op)
	       || OP_imin(op) || OP_imax(op)) {

      // Integer add, sub, mul, bitand, bitor, bitxor, min, max.

      TN *opnd1 = OP_Opnd1 (op);
      TN *opnd2 = OP_Opnd2 (op);
      LRange_p opnd1_value = Value (opnd1);
      LRange_p opnd2_value = Value (opnd2);
      if (TOP_is_unsign (opcode)) {
	opnd1_value = MakeUnsigned (opnd1_value, TN_bitwidth (opnd1));
	opnd2_value = MakeUnsigned (opnd2_value, TN_bitwidth (opnd2));
      }
      if (OP_iadd(op)) {
	new_value = AddModulo(opnd1_value, opnd2_value, result_width);
      } else if (OP_isub(op)) {
	new_value = SubModulo(opnd1_value, opnd2_value, result_width);
      } else if (OP_imul(op)) {
	new_value = MulModulo(opnd1_value, opnd2_value, result_width);
      } else if (OP_iand(op)) {
	new_value = BitAnd (opnd1_value, opnd2_value);
      } else if (OP_ixor(op)) {
	new_value = BitXor(opnd1_value, opnd2_value);
      } else if (OP_ior(op)) {
	new_value = BitOr(opnd1_value, opnd2_value);
      } else if (OP_imin(op)) {
	new_value = Min(opnd1_value, opnd2_value);
      } else if (OP_imax(op)) {
	new_value = Max(opnd1_value, opnd2_value);
      } else
	FmtAssert (FALSE, ("Case missing in RangeAnalysis::Visit_Forward"));
    } else if (OP_icmp(op)) {

      // Integer compare.

      LRange_p opnd1 = Value(OP_Opnd1(op));
      LRange_p opnd2 = Value(OP_Opnd2(op));
      
#ifdef TARG_ST
      VARIANT variant = OP_cmp_variant(op);
#else
      VARIANT variant = TOP_cmp_variant(opcode);
#endif
      INT cmp_width = 8 * TN_size (OP_Opnd1(op));

      if (TOP_is_unsign(opcode)) {
	opnd1 = MakeUnsigned (opnd1, cmp_width);
	opnd2 = MakeUnsigned (opnd2, cmp_width);
      } else {
	opnd1 = MakeSigned (opnd1, cmp_width);
	opnd2 = MakeSigned (opnd2, cmp_width);
      }
      switch (variant) {
      case V_CMP_EQ:
	new_value = Eq (opnd1, opnd2);
	break;
      case V_CMP_NE:
	new_value = Ne (opnd1, opnd2);
	break;
      case V_CMP_GE:
      case V_CMP_GEU:
	new_value = Ge (opnd1, opnd2);
	break;
      case V_CMP_GT:
      case V_CMP_GTU:
	new_value = Gt (opnd1, opnd2);
	break;
      case V_CMP_LE:
      case V_CMP_LEU:
	new_value = Le (opnd1, opnd2);
	break;
      case V_CMP_LT:
      case V_CMP_LTU:
	new_value = Lt (opnd1, opnd2);
	break;
      case V_CMP_ANDL:
	new_value = Andl (opnd1, opnd2);
	break;
      case V_CMP_NANDL:
	new_value = Nandl (opnd1, opnd2);
	break;
      case V_CMP_ORL:
	new_value = Orl (opnd1, opnd2);
	break;
      case V_CMP_NORL:
	new_value = Norl (opnd1, opnd2);
	break;
      default:
	FmtAssert (FALSE, ("Unhandled compare variant in Visit_Forward %d", variant));
	break;
      }
    } else if (OP_fcmp(op)) {
      
      // F.P. compare.

      new_value = lattice->makeRangeWidth(Range::Unsigned, 1);
    } else if (OP_extract(op)) {

      // Extract parts of multi-register value.

      new_value = Value (OP_opnd(op, 0));
      UINT32 bits = TN_size (result) * 8;
      new_value = Extract(new_value, i * bits, bits);
    } else if (OP_compose(op)) {

      // Compose multi-register value.

      new_value = Value (OP_opnd (op, OP_opnds(op) - 1));
      for (INT j = OP_opnds (op) - 2; j >= 0; j--) {
	TN *opnd = OP_opnd(op, j);
	UINT32 bits = TN_bitwidth (opnd);
	new_value = LeftShift (new_value, bits);
	new_value = BitOr (new_value, MakeUnsigned (Value (opnd), bits));
      }
    } else if (OP_phi(op) || OP_psi(op)) {

      // Phi/Psi operation.

      // Merge the inputs.
      if (OP_phi(op)) {
	new_value = Value (result);
	for (INT i = 0; i < OP_opnds(op); i++) {
	  TN *opnd = OP_opnd(op, i);
	  if (isExecuted(SSA_BB(opnd)))
	    new_value = lattice->makeMeet (new_value, Value (opnd));
	}
      }
      // Only actual PSI opnds must be considered, not guards.
      // Use specific PSI opnd iterator.
      if (OP_psi(op)) {
	new_value = Value (result);
	for (INT i = 0; i < PSI_opnds(op); i++) {
	  TN *opnd = PSI_opnd(op, i);
	  if (isExecuted(SSA_BB(opnd)))
	    new_value = lattice->makeMeet (new_value, Value (opnd));
	}
      }
      
    } else if (OP_select(op)) {

      // Select.

      // Select result has meet of the operands.
      new_value = lattice->makeMeet (Value (OP_Opnd1(op)),
				 Value (OP_Opnd2(op)));
    }

    // Get affirm information, if available
    INT base, bias;
    if (Get_Affirm_modulo(result, OP_bb(op), &base, &bias)){
      LRange_p aff_value = lattice->makeAlign(base, bias);
      new_value = lattice->makeJoin(new_value, aff_value);
    }
    // Value is limited to the range of the tn that holds it.
    new_value =  Fit_To_TN (new_value, result, 0);
    Recompute_Forward (result, new_value);
  }

  // Handle conditional exit.
  if (OP_cond(op)) {
    BB *bb = OP_bb(op);
    if (! BB_is_SSA_region_exit (bb)) {
      BB *fall_thru = BB_Fall_Thru_Successor (bb);
      BB *target = BB_Other_Successor (bb, fall_thru);
      // Much more work needed for compare-and-branch instructions.
      LRange_p condition = Value (OP_Condition(op));
      VARIANT v = TOP_cond_variant (opcode);
      if (v != V_COND_NONE && condition->isNonZero ()) {
	Recompute_Forward ((v == V_COND_TRUE) ? target : fall_thru);
	if (succs_done) *succs_done = TRUE;
      } else if (v != V_COND_NONE && condition->isZero ()) {
	Recompute_Forward ((v == V_COND_TRUE) ? fall_thru : target);
	if (succs_done) *succs_done = TRUE;
      } else if (! succs_done) {
	for (INT i = 0; i < BB_succs_len (bb); i++) {
	  Recompute_Forward (BB_ordered_succ (bb, i));
	}
      }
    }
  }
}

void
RangeAnalysis::Visit_Forward (OP_LIST *uses) {
  for (; uses != NULL; uses = OP_LIST_rest (uses)) {
    OP *op = OP_LIST_first (uses);
    if (isExecuted (OP_bb (op)))
      Visit_Forward (op, NULL);
  }
}

void
RangeAnalysis::Analyze_Forward ()
{
  BB *bb;
  OP *op;

  analysis = Forward;

  forward_valid = FALSE;
  backward_valid = FALSE;

  Trace_Analyze_Forward_Start ();

  tn_value = TN_MAP_Create ();

  FmtAssert (tn_work.empty (), ("Analyze_Forward: tn worklist not empty"));
  FmtAssert (bb_work.empty (), ("Analyze_Forward: bb worklist not empty"));

  FOR_ALL_BB_SET_members (SSA_region_entries (), bb) {
    // Care: when we have unreachable code, SSA_region_entries() can classify
    // unreachable BBs as region entries.  This causes problems here because when
    // we set up BB_ordered_succ we set null for successors of unreachable BBs.
    // So here, call Recompute_Forward only if the bb is reachable from a real
    // entrypoint.
    if (BB_entry (bb) || BB_handler (bb)) {
      Recompute_Forward (bb);
    }
  }
  
  while (! bb_work.empty () || ! tn_work.empty ()) {
    // Patterson 95: "experience has shown that preferring
    // to select from the Flow worklist tends to ... reduce
    // the running time of the algorithm."
    if (! bb_work.empty ()) {
      bb = bb_work.choose ();

      Trace_Visit (bb);

      BOOL succs_done = FALSE;
      FOR_ALL_BB_OPs_FWD(bb, op) {
	Visit_Forward (op, &succs_done);
      }
      if (! succs_done && ! BB_is_SSA_region_exit (bb)) {
	for (INT i = 0; i < BB_succs_len (bb); i++) {
	  Recompute_Forward (BB_ordered_succ (bb, i));
	}
      }
    }

    else if (! tn_work.empty ()) {
      const TN *tn = tn_work.choose ();
      Trace_Visit (tn);
      Visit_Forward (Uses (tn));
    }
  }

  tn_value_forward = tn_value;
  tn_value = NULL;

  forward_valid = TRUE;

  Trace_Analyze_Forward_End ();

  analysis = None;

}

/* ====================================================================
 *
 * Backward analysis.
 *
 * ====================================================================
 */

void
RangeAnalysis::Create_Opnd_Info ()
{
  INT n_operands = 0;
  INT ix;
  BB *bb;
  OP *op;
  Opnd_Info *all_operand_info;

  // Need to know how many operands there are before we can
  // allocate the operand info...
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    FOR_ALL_BB_OPs (bb, op) {
      n_operands += OP_opnds (op);
    }
  }

  all_operand_info = (Opnd_Info *)TYPE_MEM_POOL_ALLOC_N (Opnd_Info, &pool,
							 n_operands);

  // Create map from OP to the Opnd_Info for its operands, and
  // initialize the Opnd_Info.
  opnd_info_map = OP_MAP_Create ();
  ix = 0;
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    FOR_ALL_BB_OPs (bb, op) {
      OP_MAP_Set (opnd_info_map, op, all_operand_info + ix);
      for (INT opnd_idx = 0; opnd_idx < OP_opnds (op); opnd_idx++) {
	all_operand_info[ix].Set_Op (op);
	all_operand_info[ix].Set_Opnd_idx (opnd_idx);
	all_operand_info[ix].Set_Value (lattice->makeTop ());
	all_operand_info[ix].Set_LoweredCount (0);
	ix++;
      }
    }
  }
}

void
RangeAnalysis::Recompute_Backward (OP *op, INT opnd_idx, LRange_p &new_value)
{
  Opnd_Info *use = Find_Opnd_Info (op, opnd_idx);
  TN *tn = use->Tn ();
  const LRange_p old_value = use->Value ();

  if (use->LoweredCount () > CG_range_recompute_limit) {
    new_value = Fit_To_TN (lattice->makeBottom (), tn, 1);
  }
  if (new_value->StrictlyContains (old_value)) {
    use->Set_Value (new_value);
    use->IncrementLoweredCount ();
    Trace_Recompute (use);
    tn_work.push (tn);
    SetValue (tn, lattice->makeMeet (Value (tn), new_value));
  } else
    FmtAssert (new_value->Equal (old_value),
	       ("Value raised in Recompute_Backward (TN %d)", TN_number (tn)));
}

void
RangeAnalysis::Visit_Backward (OP *op)
{
  // Recompute the range of all operands of op.

  TOP opcode = OP_code(op);

  Trace_Visit (op);

  for (INT opnd_idx = 0; opnd_idx < OP_opnds(op); opnd_idx++) {
    TN *opnd = OP_opnd(op, opnd_idx);
    LRange_p new_value = lattice->makeBottom();
    
    if (!TN_is_ssa_var(opnd)) continue;

    if (TARG_Visit_Backward (op, opnd_idx, new_value))
      ;
    else if (OP_copy(op)
	&& opnd_idx == OP_Copy_Operand(op)) {

      // Copy
      // Operand gets range of result.

      TN *result = OP_result(op, OP_Copy_Result(op));
      new_value = Value (result);
    } else if (OP_store(op)
	       && opnd_idx == OP_find_opnd_use (op, OU_storeval)) {

      // Store
      // Stored value gets size of store.

      UINT32 width = OP_Mem_Ref_Bytes (op) * 8;
      new_value = lattice->makeRangeBitWidth ( width);
    } else if ((OP_sext(op) || OP_zext(op))
		&& opnd_idx == OP_find_opnd_use (op, OU_opnd1)) {

      // Sign-/Zero-extend.
      // Upper bits of operand are ignored.

      UINT32 width = TOP_opnd_use_bits(opcode, opnd_idx);
      new_value = lattice->makeRangeBitWidth ( width);
    } else if (OP_phi(op) || OP_psi(op)) {

      // Phi/Psi operation.
      // Operand gets range of result.

      new_value = Value (OP_result(op, 0));
    } else if (OP_select(op)) {

      // Select.
      // Operand gets range of result.

      if (opnd_idx == OP_find_opnd_use (op, OU_opnd1)
	  || opnd_idx == OP_find_opnd_use (op, OU_opnd2)) {
	// Select operand gets range of result.
	new_value = Value (OP_result(op, 0));
      }
    } else if (OP_icmp(op)) {

      // Integer compare.
      // Some comparisons only require sign bit.
      
#ifdef TARG_ST
      VARIANT variant = OP_cmp_variant(op);
#else
      VARIANT variant = TOP_cmp_variant(opcode);
#endif

      switch (variant) {
      case V_CMP_GE:
      case V_CMP_LT:
	if (opnd_idx == OP_find_opnd_use (op, OU_opnd1)
	    && TN_is_zero (OP_Opnd2(op))) {
	  // These comparisons require only sign bit of opnd1.
	  new_value = lattice->makeRangeLowBitWidth (
			     TOP_opnd_use_bits(opcode, opnd_idx) - 1, 1);
	}
      }
    } else if (OP_ixor(op) || OP_ior(op)) {

      // Bitwise and, or, xor.
      // Operand gets range of result.

      new_value = Value (OP_result(op, 0));
    } else if (OP_iand(op)) {
      TN *opnd2 = opnd_idx ? OP_opnd(op, 0) : OP_opnd(op, 1);
      if (TN_has_value(opnd2))
	new_value = lattice->makeJoin(Value (OP_result(op, 0)), Value (opnd2)); 
      else 
	new_value = Value (OP_result(op, 0));
    } else if (OP_iadd(op) || OP_imul(op) || OP_isub(op) ) {

      // add, sub, mul
      // Operand gets range of result bit-width.

      new_value =  lattice->makeRangeBitWidth (
			  Value (OP_result(op, 0))->bits ());
    }
	  
    new_value = Fit_To_TN (new_value, opnd, 1);
    Recompute_Backward (op, opnd_idx, new_value);
  }
}

void
RangeAnalysis::Analyze_Backward ()
{
  BB *bb;
  OP *op;

  forward_valid = FALSE;
  backward_valid = FALSE;

  analysis = Backward;

  Trace_Analyze_Backward_Start ();

  Create_Opnd_Info ();

  tn_value = TN_MAP_Create ();

  FmtAssert (tn_work.empty(), ("Analyze_Backward: tn worklist not empty"));
  
  for (INT i = n_topological_order; i >= 0; i--) {
    bb = topological_order[i];

    if (!isExecuted (bb)) continue;
    Trace_Visit (bb);

    FOR_ALL_BB_OPs_REV (bb, op) {
      Visit_Backward (op);
    }
  }

  while (! tn_work.empty ()) {
    const TN *tn = tn_work.choose ();
    Trace_Visit (tn);
    Visit_Backward (TN_ssa_def (tn));
  }

  tn_value_backward = tn_value;
  tn_value = NULL;

  backward_valid = TRUE;

  Trace_Analyze_Backward_End ();

  analysis = None;
}

LRange_p
RangeAnalysis::Get_Value (const TN *tn) const
{
  FmtAssert (Forward_Valid (),
	     ("Get_Value called when information not valid"));

  if (TN_is_register (tn)) {
    const TN_Lattice_Info *info =
      (const TN_Lattice_Info *)TN_MAP_Get (tn_value_forward, tn);
    if (info) {
      return info->value;
    }
  } else if (TN_has_value (tn))
    return lattice->makeRangeValue (TN_value (tn));

  return Fit_To_TN (lattice->makeUniverse (), tn, 0);
}

LRange_p
RangeAnalysis::Get_Value_Backward (const TN *tn) const
{
  FmtAssert (Backward_Valid (),
	     ("Get_Value called when information not valid"));
  if (TN_is_register (tn)) {
    const TN_Lattice_Info *info = 
      (const TN_Lattice_Info *)TN_MAP_Get (tn_value_backward, tn);
    if (info)
      return info->value;
  }

  return Fit_To_TN (lattice->makeUniverse (), tn, 1);
}

