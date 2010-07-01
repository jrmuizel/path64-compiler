/*
  Copyright (C) 2002, STMicroelectronics, All Rights Reserved.

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
 * Module: cg_affirm.cxx
 *
 * Description:
 *
 *
 * ====================================================================
 * ====================================================================
 */

#ifdef TARG_ST

#ifdef _KEEP_RCS_ID
static const char source_file[] = __FILE__;
static const char rcs_id[] = "";
#endif /* _KEEP_RCS_ID */

#include "tracing.h"
#include "cg_affirm.h"
#include "symtab.h"
#include "data_layout.h"
#include "op_map.h"
#include "cg_ssa.h"
#include "cgexp.h"

INT32 CG_affirm_opt = 0;

// =======================================================================
// OP_Set_Affirm
// Return TRUE if the <op> has a WN_AFFIRM whirl node attached to it
// =======================================================================
static OP_MAP OP_to_WN_AFFIRM_map = NULL;

void
OP_Set_Affirm (OP *op, WN *wn_affirm) {

  if (OP_to_WN_AFFIRM_map == NULL)
    OP_to_WN_AFFIRM_map = OP_MAP_Create();

  OP_MAP_Set(OP_to_WN_AFFIRM_map, op, wn_affirm);
}

WN *
OP_Get_Affirm(const OP *op)
{
  if (OP_to_WN_AFFIRM_map == NULL)
    return NULL;
  return (WN *)OP_MAP_Get(OP_to_WN_AFFIRM_map, op);
}

void
OP_Affirm_delete_map()
{
  if (OP_to_WN_AFFIRM_map != NULL) {
    OP_MAP_Delete(OP_to_WN_AFFIRM_map);
    OP_to_WN_AFFIRM_map = NULL;
  }
}

static void
Normalized_Binary_WN(WN *wn, WN **kid0, WN **kid1) {
  Is_True(WN_kid_count(wn) == 2, ("Normalized_Binary_WN called on a non-binary Whirl node."));
  *kid0 = WN_kid0(wn);
  *kid1 = WN_kid1(wn);
  if (WN_operator_is(*kid0, OPR_INTCONST)) {
    WN *swap = *kid1;
    *kid1 = *kid0;
    *kid0 = swap;
  }
}

typedef std::vector<WN *> wn_and_exprs_t;

static void
Affirm_Extract_And_Expressions(WN *wn, wn_and_exprs_t *wn_list) {

  if (WN_operator(wn) != OPR_LAND) {
    // Push this one and return
    wn_list->push_back(wn);
    return;
  }

  Affirm_Extract_And_Expressions(WN_kid0(wn), wn_list);
  Affirm_Extract_And_Expressions(WN_kid1(wn), wn_list);
}

static WN *
Affirm_Combine_And_Expressions(wn_and_exprs_t *wn_list, int idx) {

  if ((idx+1) >= wn_list->size())
    return (*wn_list)[idx];
  else
    return WN_LAND((*wn_list)[idx], Affirm_Combine_And_Expressions(wn_list, idx+1));
}

BOOL
Get_Affirm_modulo(WN *wn_affirm, WN**wn_modulo, INT* affirm_base, INT* affirm_bias) {

  Is_True(WN_operator_is(wn_affirm, OPR_AFFIRM), ("AFFIRM property is not an AFFIRM whirl node"));

  wn_and_exprs_t wn_and_list;
  *affirm_base = 1;
  *affirm_bias = 0;

  Affirm_Extract_And_Expressions(WN_kid0(wn_affirm), &wn_and_list);

  for (int i = 0; i < wn_and_list.size(); i++) {

    WN *exp_affirm = wn_and_list[i];

    if (WN_operator(exp_affirm) == OPR_EQ) {

      // one child must be 0, the other must be var&(align-1)
      WN *wn_eq1, *wn_eq2;
      Normalized_Binary_WN(exp_affirm, &wn_eq1, &wn_eq2);

      if (WN_operator_is(wn_eq2, OPR_INTCONST) &&
	  (WN_operator_is(wn_eq1, OPR_BAND) || WN_operator_is(wn_eq1, OPR_MOD))) {

	int eq_val = WN_const_val(wn_eq2);
	WN *wn_band1, *wn_band2;
	Normalized_Binary_WN(wn_eq1, &wn_band1, &wn_band2);

	if (WN_operator_is(wn_band2, OPR_INTCONST) &&
	    WN_operator_is(wn_band1, OPR_LDID)) {
	  int band_val = WN_const_val(wn_band2);
	  if (band_val > 0 && eq_val >= 0) {
	    if (wn_modulo != NULL)
	      *wn_modulo = exp_affirm;
	    if (WN_operator_is(wn_eq1, OPR_BAND) && 
		((band_val & (band_val + 1))==0))
	      *affirm_base = band_val+1;
	    else if (WN_operator_is(wn_eq1, OPR_MOD))
	      *affirm_base = band_val;
	    else {
	      return FALSE;
	    }
	  }
	    *affirm_bias = eq_val;
	    return TRUE;
	}
      }
    }
  }
  return FALSE;
}

static WN *
Affirm_Replace_wn(WN *wn_affirm, WN *wn, WN *wn_new) {

  wn_and_exprs_t wn_and_list;
  Affirm_Extract_And_Expressions(WN_kid0(wn_affirm), &wn_and_list);

  int i;
  for (i = 0; i < wn_and_list.size(); i++) {
    if (wn_and_list[i] == wn) {
      wn_and_list[i] = wn_new;
      break;
    }
  }

  FmtAssert(i < wn_and_list.size(), ("Affirm_Replace_wn: WN not found"));

  return WN_CreateAffirm(Affirm_Combine_And_Expressions(&wn_and_list, 0));
}

static WN *
Affirm_Insert_wn(WN *wn_affirm, WN *wn_new) {
  return WN_CreateAffirm(WN_LAND(WN_kid0(wn_affirm), wn_new));
}

static BOOL
OP_Insert_Affirm_for_modulo(OP *op_affirm, INT base, INT bias) {

  // Fisrt, look if there is already an AFFIRM property on this op
  WN *wn_affirm = OP_Get_Affirm(op_affirm);
  INT affirm_base = -1;
  INT affirm_bias = -1;
  WN *wn_modulo = NULL;

  // Check if wn_affirm is a superset of the modulo property we get
  if ((wn_affirm != NULL) &&
      Get_Affirm_modulo(wn_affirm, &wn_modulo, &affirm_base, &affirm_bias)) {

    FmtAssert((affirm_bias < affirm_base),
	      ("Unsupported base/bias information on affirm: %d %d", affirm_base, affirm_bias));

    // Affirm node already includes this modulo information
    if (affirm_base >= base)
      return FALSE;
  }

  // Normalize bias such that 0 <= bias < base
  bias = bias % base;
  if (bias < 0) bias += base;
  Is_True((0 <= bias) && (bias < base), ("bias must be normalized here"));

  // Create a new affirm whirl node
  WN *wn_new_modulo;
  ST *st = Gen_Temp_Symbol(MTYPE_To_TY(MTYPE_I4), ".affirm");
  wn_new_modulo = WN_EQ(MTYPE_I4,
			WN_Binary(OPR_MOD, MTYPE_I4,
				  WN_Ldid(MTYPE_I4, 0, st, 4),
				  WN_Intconst(MTYPE_I4, base)),
			WN_Intconst(MTYPE_I4, bias));

  // If there was already a modulo information, replace it with this
  // new, more precise, one
  if (affirm_base != -1)
    wn_affirm = Affirm_Replace_wn(wn_affirm, wn_modulo, wn_new_modulo);

  // If there was already an affirm property, complete it with this
  // modulo information
  else if (wn_affirm != NULL)
    wn_affirm = Affirm_Insert_wn(wn_affirm, wn_new_modulo);

  // There was no affirm property, create a new one with this modulo
  // information.
  else
    wn_affirm = WN_CreateAffirm(wn_new_modulo);

  // Finally, attach the information to the operation
  OP_Set_Affirm(op_affirm, wn_affirm);

  return TRUE;
}

BOOL
BB_Insert_Affirm_for_modulo(BB *bb, TN *tn, INT base, INT bias) {

  OP *op_affirm = NULL;

//   Is_True((base > 0) && ((base&(base-1)) == 0), ("Unsupported value for base (%d) in Insert_Affirm_for_modulo", base));

  if (Get_Trace(TP_AFFIRM, 0x1)) {
    fPrint_TN(TFile, "TN has modularity %s%%", tn);
    fprintf(TFile, "%d==%d, will set AFFIRM property.\n", base, bias);
  }

  // First, look if the definition for tn is not already in bb, or in
  // predecessors bb while there is no incoming or outgoing edges.
  if (SSA_Active()) {
    OP *tn_def = TN_ssa_def(tn);
    OP *tn_copy = tn_def;
    // First look for an operation with AFFIRM property, following COPY operations.
    while (tn_copy != NULL) {
      // Found an operation with AFFIRM property
      if (OP_Is_Affirm(tn_copy)) {
	break;
      }
      // Found a COPY operation, continue with the def of its argument
      else if ((OP_results(tn_copy) == 1) && (OP_Copy_Operand_TN(tn_copy) != NULL)) {
	tn_copy = TN_ssa_def(OP_Copy_Operand_TN(tn_copy));
      }
      // Did not find an operation with AFFIRM property.
      else
	tn_copy = NULL;
    }

    // Then, check that tn_def is defined in the current basic block
    // or in predecessors bb while there is no incoming or outgoing
    // edges.
    if (tn_def != NULL) {
      BB *bb_def = bb;
      do {
	// Initialize op_affirm with the copy operation. It may be
	// redefined if tn_def is also in a proper predecessor.
	if (OP_bb(tn_def) == bb_def) {
	  op_affirm = tn_def;
	}
	if ((tn_copy != NULL) && (OP_bb(tn_copy) == bb_def)) {
	  op_affirm = tn_copy;
	  break;
	}
	
	bb_def = BB_Unique_Predecessor(bb_def);
      } while ((bb_def != NULL) && (BB_succs_len(bb_def) == 1));
    }
  }
  else {
    // Not in SSA form, just look for a definition of tn in bb.
    OP *op;
    FOR_ALL_BB_OPs_REV(bb, op) {
      for (INT i = 0; i < OP_results(op); i++) {
	if (OP_result(op,i) == tn) {
	  op_affirm = op;
	  break;
	}
      }
      if (op_affirm != NULL)
	break;
    }
  }

  if (op_affirm == NULL) {
    OPS New_OPs = OPS_EMPTY;
    Exp_COPY(tn, tn, &New_OPs);
    op_affirm = OPS_last(&New_OPs);
    OP *point = BB_last_op(bb);
    BOOL before = (point != NULL) && OP_xfer(point);
    // Disable SSA while inserting this non-SSA op, because otherwise
    // it tries to set an SSA def.
    if (SSA_Active()) {
      SSA_unset(op_affirm);
      SSA_Disable();
      BB_Insert_Ops(bb, point, &New_OPs, before);
      SSA_Enable();
    }
    else
      BB_Insert_Ops(bb, point, &New_OPs, before);

    if (Get_Trace(TP_AFFIRM, 0x1))
      fprintf(TFile, "Inserted COPY operation to attach AFFIRM property.\n");
  }

  OP_Insert_Affirm_for_modulo(op_affirm, base, bias);

  return TRUE;
}

BOOL
OPS_Insert_Affirm_for_modulo(OPS *ops, TN *tn, INT base, INT bias) {

  OP *op_affirm = NULL;

//   Is_True((base > 0) && ((base&(base-1)) == 0), ("Unsupported value for base (%d) in Insert_Affirm_for_modulo", base));

  if (Get_Trace(TP_AFFIRM, 0x1)) {
    fPrint_TN(TFile, "TN has modularity %s%%", tn);
    fprintf(TFile, "%d==%d, will set AFFIRM property.\n", base, bias);
  }

  // First, look if the definition for tn is not already in ops
  OP *op;
  FOR_ALL_OPS_OPs_REV(ops, op) {
    for (INT i = 0; i < OP_results(op); i++) {
      if (OP_result(op,i) == tn) {
	op_affirm = op;
	break;
      }
    }
    if (op_affirm != NULL)
      break;
  }

  if (op_affirm == NULL) {
    Exp_COPY(tn, tn, ops);
    op_affirm = OPS_last(ops);

    if (Get_Trace(TP_AFFIRM, 0x1))
      fprintf(TFile, "Inserted COPY operation to attach AFFIRM property.\n");
  }

  OP_Insert_Affirm_for_modulo(op_affirm, base, bias);

  return TRUE;
}

// =======================================================================
// Get_Affirm_modulo
// Returns TRUE if a non-trivial alignment information has been infered
// by an affirm property on tn at exit of bb
// =======================================================================
BOOL
Get_Affirm_modulo(TN *tn, BB *bb, INT* base, INT* bias) {
  // Look backward for a definition of tn in bb;
  *base = 1;
  *bias = 0;
  do {
    OP *op;
    FOR_ALL_BB_OPs_REV(bb, op) {
      if (OP_Is_Affirm(op) && (OP_results(op) == 1) && (tn == OP_result(op, 0))){
	return Get_Affirm_modulo(OP_Get_Affirm(op), NULL, base, bias);
      }
      for (INT i = 0; i < OP_results(op); i++) {
	if (OP_result(op,i) == tn) {
	  if ((OP_results(op) == 1) &&
	      (OP_Copy_Operand_TN(op) != NULL) &&
	      // FdF 20090630: Do not propagate through a dedicated
	      // register, otherwise more checks are needed to detect
	      // operations such as call, asm, ...
	      !TN_is_dedicated(OP_Copy_Operand_TN(op)))
	    tn = OP_Copy_Operand_TN(op);
	  else
	    return FALSE;
	}
      }
    }

    // FdF 20080528: Go up one bb while there is only one predecessor.
    // Edges leaving the region do not invalidate an AFFIRM property.
    bb = BB_Unique_Predecessor(bb);
  } while (bb != NULL);

  return FALSE;
}

BOOL 
Generate_Affirm(RangeAnalysis *range_analysis, BB *bb)
{

  BOOL Update_SSA = FALSE;

  if ((CG_affirm_opt&(AFFIRM_TRIP_COUNT|AFFIRM_ALIGN)) == 0)
    return FALSE;

  // FdF 20080311: Check if some information can be collected for a
  // loop trip count TN
  if (BB_loophead(bb) && (BB_preds_len(bb) == 2)) {

    BB *prolog;
    BB *pred = BBLIST_item(BB_preds(bb));
    if (BB_loop_head_bb(pred) == bb)
      prolog = BB_Other_Predecessor(bb, pred);
    else
      prolog = pred;

    // Generate AFFIRM property if trip count as some modularity property.    

    if ((CG_affirm_opt&AFFIRM_TRIP_COUNT) != 0) {

      ANNOTATION *annot = ANNOT_Get(BB_annotations(bb), ANNOT_LOOPINFO);
      LOOPINFO *info = annot ? ANNOT_loopinfo(annot) : NULL;
      TN *trip_count_tn = info ? LOOPINFO_primary_trip_count_tn(info) : NULL;
      if (trip_count_tn && TN_is_ssa_var(trip_count_tn)) {
	// Insert a copy with an assume property before the loop head.
	const LRange_p r = range_analysis->Get_Value(trip_count_tn);
       UINT64 base = r->getBase();
       UINT64 bias = r->getBias();
      if ((base != 0) && (base != 1)) {
	  if (Get_Trace(TP_AFFIRM, 0x1))
	      fprintf(TFile, "Trip count: ");
	  if (BB_Insert_Affirm_for_modulo(prolog, trip_count_tn, base, bias))
	    Update_SSA = TRUE;
	}
      }
    }

    // Generate AFFIRM property for TNs used in PHI operations in loop
    // head and defined at loop entry.
    if ((CG_affirm_opt&AFFIRM_ALIGN) != 0) {

      OP *op;
      FOR_ALL_BB_OPs_FWD(bb, op) {

	if (OP_code(op) != TOP_phi)
	  break;

	INT idx = Get_PHI_Predecessor_Idx(op, prolog);
	TN *tn = OP_opnd(op, idx);
	const LRange_p r = range_analysis->Get_Value(tn);
	UINT64 base = r->getBase();
	UINT64 bias = r->getBias();
	if ((base != 0) && (base != 1) && (base <= 0x10000)) {
	  if (Get_Trace(TP_AFFIRM, 0x1))
	      fprintf(TFile, "PHI Operand: ");
	  if (BB_Insert_Affirm_for_modulo(prolog, tn, base, bias))
	    Update_SSA = TRUE;
	}
      }
    }

  }

  return Update_SSA;
}
#endif
