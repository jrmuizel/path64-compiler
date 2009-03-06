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
 * Module: targ_select.cxx
 *
 * Description:
 *
 * Select target-specific optimizations.
 *
 * ====================================================================
 * ====================================================================
 */

#include "cgir.h"
#include "cgexp.h"
#include "data_layout.h"
#include "stblock.h"
#include "whirl2ops.h"
#include "tn.h"
#include "targ_cg_private.h"
#include "cg_ssa.h"
#include "cg_sched_est.h"
#include "cg_select.h"

void CG_SCHED_EST_Add_Merge_Pred(CG_SCHED_EST *se) {
  se->cached_resource_cycles = 0;
  se->cached_crit_path_len = 0;
  TI_RES_COUNT_Subtract_Op_Resources(se->res_count, TOP_and_r_r_npc);
}

TOP Get_TOP_Merge_Preds()
{
  return TOP_UNDEFINED;
}

static TYPE_ID 
Get_TOP_desc_type( TOP top ) {
  TYPE_ID desc = MTYPE_UNKNOWN;

#if 0
  switch (top) {
  case TOP_sw_r_post_inc:
  case TOP_sw_i5_post_inc:
  case TOP_sw_r_inc:
  case TOP_sw_i8_inc:
  case TOP_sw_i12_inc:
  case TOP_sw_i5_post_dec:
  case TOP_sw_i5_pre_dec:
  case TOP_sw_i8_dec:
    desc = MTYPE_I4;
    break;

  case TOP_sh_r_post_inc:
  case TOP_sh_i5_post_inc:
  case TOP_sh_r_inc:
  case TOP_sh_i8_inc:
  case TOP_sh_i12_inc:
  case TOP_sh_i5_post_dec:
  case TOP_sh_i5_pre_dec:
  case TOP_sh_i8_dec:
    desc = MTYPE_I2;
    break;

  case TOP_sb_r_post_inc:
  case TOP_sb_i5_post_inc:
  case TOP_sb_r_inc:
  case TOP_sb_i8_inc:
  case TOP_sb_i12_inc:
  case TOP_sb_i5_post_dec:
  case TOP_sb_i5_pre_dec:
  case TOP_sb_i8_dec:
    desc = MTYPE_I1;
    break;

  default:
    DevAssert(FALSE, ("stw"));    
  }
#endif
  return desc;
}

/* --------------------------------------------------------------------
 *    Make sure the compare instruction returns in a general register.
 * --------------------------------------------------------------------
 */
TN *
Expand_CMP_Reg (TN *btn, OPS *ops)
{
  DevAssert(Is_Predicate_REGISTER_CLASS(TN_register_class(btn)), ("must a guard"));

  return btn;
}

// We need a store location to allow the if conversion of conditional stores.
// This location is actually a global common. but we could allocate it on the
// stack frame instead. define LOCAL_SELECT_STOREDUMP to allow this address
// to be generated from the frame.
// Create a common symbol to speculate the address while if converting store.

// #define COMMON_SELECT_STOREDUMP

#define LOCAL_SELECT_STOREDUMP

static ST *blackhole;

#ifdef  COMMON_SELECT_STOREDUMP
static ST *
Gen_Common_Symbol (TY_IDX      ty,	// type of the desired symbol
		 const char *rootname)	// root of the name to use
{
  ST *st = blackhole;

  if (!st) {
    st = New_ST(GLOBAL_SYMTAB);
    STR_IDX str_idx = Save_Str(rootname);
    ST_Init(st, str_idx, CLASS_VAR, SCLASS_COMMON, EXPORT_INTERNAL, ty);
    Allocate_Object (st);
  }
  return st;
}
#endif
#ifdef LOCAL_SELECT_STOREDUMP
static ST *latest_pu;

static ST *
Gen_Local_Symbol (TY_IDX      ty,	// type of the desired symbol
		 const char *rootname)	// root of the name to use
{
  ST *st = blackhole;

  // Only need to reinitialize for each new PU.
  if (latest_pu != Get_Current_PU_ST()) {
    latest_pu = Get_Current_PU_ST();
    st = Gen_Temp_Symbol(ty, "__store_dump_area");
    Allocate_Temp_To_Memory(st);
  }
  return st;
}
#endif

void
Expand_BlackHole (
  OP *store_op,
  TN *blackhole_tn,
  OPS *ops
  )
{
  TY_IDX ty = MTYPE_To_TY(Pointer_Mtype);

#ifdef COMMON_SELECT_STOREDUMP
  blackhole = Gen_Common_Symbol(ty, "__store_dump_area");
  TN *tmp = Gen_Symbol_TN(blackhole, 0, 0);
  Expand_Copy(blackhole_tn, True_TN, tmp, ops);
#else
#ifdef LOCAL_SELECT_STOREDUMP
  blackhole = Gen_Local_Symbol (ty, "__store_dump_area");

#else
#error SELECT_STOREDUMP
#endif
#endif

  TN *offsetTN = OP_opnd(store_op, OP_find_opnd_use(store_op, OU_offset));
  INT32 offset = 0;

  if (TN_is_constant (offsetTN) && TN_has_value (offsetTN))
    offset = -TN_value(offsetTN);

  Exp_Lda (Pointer_type, blackhole_tn, blackhole, offset, OPERATOR_UNKNOWN, ops);
}

void
Expand_CondStoreAddr (
  OP *store_op,
  TN *storeaddr_tn,
  OPS *ops
  )
{
  TN *offsetTN = OP_opnd(store_op, OP_find_opnd_use(store_op, OU_offset));

  if (TN_is_constant (offsetTN) && TN_has_value (offsetTN))
    offsetTN = Zero_TN;

  Expand_Add (storeaddr_tn, OP_opnd(store_op, OP_find_opnd_use(store_op, OU_base)), offsetTN, Pointer_Mtype, ops);
}

void
Expand_CondStoreOP (
  OP *store_op,
  TN *storeaddr_tn,
  OPS *ops
  )
{
  TN *base = storeaddr_tn;
  TN *offset = OP_opnd(store_op, OP_find_opnd_use(store_op, OU_offset));
  TN *val  = OP_opnd(store_op, OP_find_opnd_use(store_op, OU_storeval));
  TYPE_ID desc;

  if (!(TN_is_constant (offset) && TN_has_value (offset)))
    offset = Gen_Literal_TN (0, 4);

  desc = Get_TOP_desc_type(OP_code(store_op));

  Expand_Store (desc, val, base, offset, ops);
  Copy_WN_For_Memory_OP(OPS_last(ops), store_op);
  Set_OP_black_hole(OPS_last(ops));
}

/* --------------------------------------------------------------------
 *    One of the operands is the result of a select instruction.
 *    If it is the ofst, then must generate a correct store format.
 * --------------------------------------------------------------------
 */
void
Expand_Cond_Store (
  TN   *cond_tn,
  OP   *op1,
  OP   *op2,
  UINT8 idx,
  OPS   *ops
  )
{
  TN *tns[3];
  TN *true_tn, *false_tn;
  BOOL is_black_hole = FALSE;
  BOOL invert = false;


  FmtAssert(FALSE,("Not implemented"));
#if 0

  if (!op1) {
    op1 = op2;
    op2 = NULL;
    invert = true;
  }
  
  UINT8 validx    = OP_find_opnd_use(op1, OU_storeval);
  UINT8 baseidx   = OP_find_opnd_use(op1, OU_base);
  UINT8 offsetidx = OP_find_opnd_use(op1, OU_offset);

  tns[validx]    = OP_opnd(op1, validx);
  tns[baseidx]   = OP_opnd(op1, baseidx);
  tns[offsetidx] = OP_opnd(op1, offsetidx);

  if (!op2) {
    is_black_hole = TRUE;
    FmtAssert (TN_size (tns[validx]) <= 8,
	       ("Expand_Cond_Store: TN too large"));
    TY_IDX ty = ((TN_size (tns[validx]) <= 4)
		 ? MTYPE_To_TY (MTYPE_I4)
		 : MTYPE_To_TY (MTYPE_I8));
    false_tn = Gen_Register_TN (ISA_REGISTER_CLASS_gpr, Pointer_Size);

#ifdef COMMON_SELECT_STOREDUMP
    blackhole = Gen_Common_Symbol(ty, "__store_dump_area");
    TN *tmp = Gen_Symbol_TN(blackhole, 0, 0);
    Expand_Copy(false_tn,True_TN,tmp,ops);
#else
#ifdef LOCAL_SELECT_STOREDUMP
    blackhole = Gen_Local_Symbol (ty, "__store_dump_area");

#else
#error SELECT_STOREDUMP
#endif
#endif

    if (TN_is_constant (tns[offsetidx]) &&
        TN_has_value (tns[offsetidx])) {
      INT32 offset = TN_value(tns[offsetidx]);
      Exp_Lda (Pointer_type, false_tn, blackhole, -offset, OPERATOR_UNKNOWN, ops);
      true_tn = OP_opnd(op1, baseidx);
      tns[offsetidx] = Gen_Literal_TN (offset, 4);
    }
    else {
      true_tn = Build_RCLASS_TN(ISA_REGISTER_CLASS_gpr);
      Exp_Lda (Pointer_type, false_tn, blackhole, 0, OPERATOR_UNKNOWN, ops);
      Expand_Add (true_tn, tns[baseidx], tns[offsetidx], Pointer_Mtype, ops);
      tns[offsetidx] = Gen_Literal_TN (0, 4);
    }
  }
  else {
    false_tn = OP_opnd(op2, idx);
    true_tn = OP_opnd(op1, idx);
  }

  if (invert) {
    TN *tmp = false_tn;
    false_tn = true_tn;
    true_tn = tmp;
  }

  TN *temp_tn;
  if (TN_is_register(true_tn)) 
    temp_tn = Build_TN_Like(true_tn);
  else if (TN_is_register(false_tn)) 
    temp_tn = Build_TN_Like(false_tn);
  else 
    temp_tn = Build_TN_Of_Mtype(MTYPE_I4);

  tns[idx] = temp_tn;

  Expand_Select (temp_tn, cond_tn, true_tn, false_tn, TN_size(temp_tn) == 8 ? MTYPE_I8: MTYPE_I4, FALSE, ops);

  TN *val  = tns[validx];
  TN *base = tns[baseidx];
  TN *ofst = tns[offsetidx];

  if (TN_is_register (ofst)) {
    Expand_Add (base, base, ofst, MTYPE_A4, ops);
    ofst =  Gen_Literal_TN (0, 4);
  }

  TYPE_ID desc;

#if 0
  WN *wn1 = Get_WN_From_Memory_OP(op1);

  if (wn1) {
    OPCODE opcode = WN_opcode (wn1);
    desc = OPCODE_desc(opcode);
  }
  else {
#endif
    desc = Get_TOP_desc_type(OP_code(op1));
#if 0
  }
#endif

  Expand_Store (desc, val, base, ofst, ops);
  if (is_black_hole) Set_OP_black_hole(OPS_last(ops));

#endif
}

float CGTARG_Compute_est_cost_before(CG_SCHED_EST *se1, CG_SCHED_EST *se2, CG_SCHED_EST *sehead, float taken_prob, float fallthr_prob, BOOL will_need_predicate_merge) {
    int branch_penalty = CGTARG_Branch_Taken_Penalty();
    float est_cost_before = CG_SCHED_EST_Cycles(sehead);
    if (se1) {
        est_cost_before = est_cost_before + ((CG_SCHED_EST_Cycles(se1) + branch_penalty) * taken_prob);
    }
    if (se2) {
        est_cost_before = est_cost_before + (CG_SCHED_EST_Cycles(se2) * fallthr_prob);
    }
    return est_cost_before;

}

float CGTARG_Compute_est_cost_after(CG_SCHED_EST *se1, CG_SCHED_EST *se2, CG_SCHED_EST *sehead, float taken_prob, float fallthr_prob, BOOL will_need_predicate_merge, BB* head) {
    float est_cost_after=0.0;
    if (se1) {
      CG_SCHED_EST_Append_Scheds(sehead, se1);
      CG_SCHED_EST_Subtract_Op_Resources(sehead, OP_code(BB_branch_op(head)));
    }

    if (se2) 
      CG_SCHED_EST_Append_Scheds(sehead, se2);

    // a new instruction to merge predicate will be added.
    if (will_need_predicate_merge) CG_SCHED_EST_Add_Merge_Pred(sehead);

    est_cost_after = CG_SCHED_EST_Cycles(sehead);
    return est_cost_after;
}

BOOL CGTARG_Check_Profitable_Select(CG_SCHED_EST *se1, CG_SCHED_EST *se2,int size_se1, int size_se2, float est_cost_before, float est_cost_after, float fallthr_prob) {
    return KnuthCompareLE(est_cost_after, est_cost_before);
}
