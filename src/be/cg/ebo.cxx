/*
 * Copyright (C) 2008-2009 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2007, 2008, 2009 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
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
 *  Module: ebo.cxx
 *  $Revision: 1.89 $
 *  $Date: 05/11/10 18:45:11-08:00 $
 *  $Author: tkong@hyalite.keyresearch $
 *  $Source: be/cg/SCCS/s.ebo.cxx $
 *
 *  Revision comments:
 *
 *  29-May-1998 - Initial version
 *
 *  Description:
 *  ============
 *
 *  EBO implementation.  See "ebo.h" for interface.
 *
 * =======================================================================
 * =======================================================================
 */

/* =======================================================================
 * =======================================================================
 *
 * This is the Extended Block Optimizer (EBO).
 *
 * It is a peep hole optimizer that works on a sequence of blocks that may
 * contain branches out, but can only be execxuted from the start of the
 * first block in the sequence.  These sequences are recognized during
 * processing.
 *
 * Instructions are processed in the forward direction through each block
 * and in the forward direction through the block's successor list.  New
 * blocks are processed until a "branched to" label is encountered, at
 * which time processing backs up and attempts to take a different path
 * down another successor list.
 *
 * The optimizations performed include forward propagation, common expression
 * elimination, constant folding, dead code elimination and a host of special
 * case transformations that are unique to the arcitecture of a particular
 * machine.
 *
 * In order to perform the optimizations it is nececssary to recognize the
 * definition and use connections between values.  Since this routine may be
 * called several times during compilation, and may have different information
 * available each time it is called, it is necessary to abstract the code
 * representation of the original program so that data values can be easily
 * tracked by common routines.
 *
 * The data structure used to abstract the values is the EBO_TN_INFO.  One is
 * created the first time a value is encoutered.  The EBO_TN_INFO entry is
 * associated with the register, if one has been assigned, or the TN.  This
 * allows us to support special, hard coded registers that are assigned early
 * in the code generation process, provided that references to the value
 * always use the register name to refer to that particular value.
 *
 * The data structure used to abstract each OP is the EBO_OP_INFO.  A unique
 * one is created for each OP that is encountered and contain pointers to
 * the unique EBO_TN_INFO entries that represent the values used and defined
 * by the original OP.
 *
 * These data structures make it simple to track predicated values and allow
 * for the redefintion of TNs and regeisters in a block, while providing
 * access to the previous definition.  It also simplifies the task of tracking
 * predicated code that reuses TN names and registers but define unique values.
 *
 * The construction of this abstraction is done as early s possible, allowing
 * the rest of EBO to work only with these *_INFO entries.  The design supports
 * tracking of constants, even when the original instructions can not directly
 * reference constants in their operand fields.
 *
 */

#ifdef USE_PCH
#include "cg_pch.h"
#endif // USE_PCH
#pragma hdrstop

#ifdef _KEEP_RCS_ID
static const char source_file[] = __FILE__;
#endif /* _KEEP_RCS_ID */

#if !defined(__FreeBSD__)
#include <alloca.h>
#endif
#include <stdarg.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include "defs.h"
#include "errors.h"
#include "mempool.h"
#include "tracing.h"
#include "timing.h"
#include "cgir.h"
#include "tn_map.h"
#include "cg_loop.h"
#include "cg.h"
#include "cgexp.h"
#include "register.h"
#include "cg_region.h"
#include "wn.h"
#include "region_util.h"
#include "op_list.h"
#include "cgprep.h"
#include "gtn_universe.h"
#include "gtn_set.h"
#include "cg_db_op.h"
#include "whirl2ops.h"
#include "cgtarget.h"
#include "gra_live.h"
#include "reg_live.h"
#include "cflow.h"
#include "cg_spill.h"
#include "targ_proc_properties.h"
#include "top_properties.h"
#include "data_layout.h"

#include "ebo.h"
#include "ebo_info.h"
#include "ebo_special.h"
#include "ebo_util.h"
#ifdef TARG_ST
#include "cg_affirm.h"
#endif

#ifdef TARG_X8664
#include "targ_sim.h"
#include "config_wopt.h"
#include "config_lno.h"
#include "xstats.h"	// Spill_Var_Cnt
#include "dominate.h"
#include "glob.h"

static void Init_Remove_Dead_LRA_Stores(BS **bs, MEM_POOL *pool);
static void Mark_LRA_Spill_Reference(OP *op, BS **bs, MEM_POOL *pool);
static BOOL Delete_Dead_LRA_Spill(OP *op, BS **bs);
#endif
#include "config_opt.h" // [HK] for Finite_Math
#ifdef TARG_ST
#include "tn.h"
#endif

#ifdef KEY
static void EBO_Adjust_Pred_Branch_Target (BB *bb);
#endif

/* ===================================================================== */
/* Global Data:								 */
/* ===================================================================== */

#ifdef KEY
INT32 EBO_Opt_Level_Default = 6;
INT32 EBO_Opt_Level = 6;
#else
INT32 EBO_Opt_Level_Default = 5;
INT32 EBO_Opt_Level = 5;
#endif
#ifdef KEY
INT32 EBO_Opt_Mask = -1;
#endif
BOOL  CG_skip_local_ebo = FALSE;

INT EBO_tninfo_number = 0;
EBO_TN_INFO *EBO_free_tninfo = NULL;
EBO_TN_INFO *EBO_first_tninfo = NULL;
EBO_TN_INFO *EBO_last_tninfo = NULL;
EBO_OP_INFO *EBO_free_opinfo = NULL;
EBO_OP_INFO *EBO_first_opinfo = NULL;
EBO_OP_INFO *EBO_last_opinfo = NULL;
EBO_OP_INFO *EBO_opinfo_table[EBO_MAX_OP_HASH];

/* Entry point indicators. */
BOOL EBO_in_pre  = FALSE;
BOOL EBO_in_before_unrolling = FALSE;
BOOL EBO_in_after_unrolling = FALSE;
LOOP_DESCR *EBO_loop = NULL;
INT EBO_loop_size = 0;
BOOL EBO_flow_safe = FALSE;
BOOL EBO_doms_calculated = FALSE;
BOOL EBO_in_peep = FALSE;
#define EBO_in_main (!EBO_in_pre && !EBO_in_peep && \
		     !EBO_in_before_unrolling && !EBO_in_after_unrolling)
/* Are OMEGA entries present? */
BOOL EBO_in_loop = FALSE;

#ifdef KEY
BOOL EBO_can_delete_branch_delay_OP = TRUE;
BOOL EBO_no_liveness_info_available = FALSE;
BOOL EBO_optimize_single_BB = FALSE;
#endif

TN_MAP EBO_tninfo_table;
MEM_POOL EBO_pool;

INT EBO_num_tninfo_entries = 0;
INT EBO_tninfo_entries_reused = 0;
INT EBO_num_opinfo_entries = 0;
INT EBO_opinfo_entries_reused = 0;

const char *EBO_trace_pfx;
BOOL EBO_Trace_Execution    = FALSE;
BOOL EBO_Trace_Optimization = FALSE;
BOOL EBO_Trace_Block_Flow   = FALSE;
BOOL EBO_Trace_Data_Flow    = FALSE;
BOOL EBO_Trace_Hash_Search  = FALSE;

#ifdef TARG_ST
static BOOL EBO_Extract_Compose_Sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo);
#endif

#ifdef TARG_ST
// [CG] Use alias information from cg_dep
#define CG_DEP_ALIAS
#endif
#ifdef TARG_ST
/* ==============================================================
 *   Arthur: Some utility functions that I moved here from the
 *           target-specific part. These seem pretty generic.
 * ==============================================================
 */

/* =====================================================================
 *   Get_Right_Mask_Length
 *
 *   Determine the number of consecutive right justified bits that are 
 *   in the mask. Return -1, if the constant is not a properly 
 *   constructed mask.
 * =====================================================================
 */
INT
Get_Right_Mask_Length (
  UINT64 mask
)
{
  INT64 i;

  if (mask == 0) return -1;
  if ((mask & 1) == 0) return -1;

  for ( i = 1; i < 64; i++)
  {
    if (((mask >> i) & 1) == 0) return ((mask >> i) == 0) ? i : -1;
  }

  return -1;
}

/* =====================================================================
 *   Get_Mask_Shift_Count
 *
 *   Determine the number of trailing zeros after a mask of all "1" 
 *   bits.  Return -1, if the constant is not a properly constructed 
 *   mask.
 * =====================================================================
 */
INT
Get_Mask_Shift_Count (
UINT64 mask
)
{
  INT64 i;

  if (mask == 0) return -1;

  for ( i = 0; i < 63; i++)
  {
    if ((mask & (1 << i)) != 0) return i;
  }

  return -1;
}

/* =====================================================================
 *   EBO_Set_OP_omega
 * =====================================================================
 */
void
EBO_Set_OP_omega (
  OP *op, 
  ...
)
{
  INT opnds = OP_opnds(op);
  INT i;
  va_list tninfos;

  va_start(tninfos, op);
  CG_LOOP_Init_Op(op);
  for (i = 0; i < opnds; i++) {
    EBO_TN_INFO *tninfo = va_arg(tninfos, EBO_TN_INFO *);
    Set_OP_omega (op, i, ((tninfo != NULL) ? tninfo->omega : 0));
  }

  va_end(tninfos);
  return;
}

/* =====================================================================
 *   EBO_Copy_OP_omega
 * =====================================================================
 */
void
EBO_Copy_OP_omega (
  OP *new_op, 
  OP *old_op
)
{
  INT opnds = OP_opnds(new_op);
  INT i;

  CG_LOOP_Init_Op(new_op);
  for (i = 0; i < opnds; i++) {
    Set_OP_omega (new_op, i, OP_omega(old_op,i));
  }

  return;
}

/* =====================================================================
 *   EBO_Set_Predicate_omega
 * =====================================================================
 */
void
EBO_Set_Predicate_omega (
  OP *new_op, 
  EBO_TN_INFO *pred_tninfo
)
{
  if (pred_tninfo != NULL && OP_find_opnd_use(new_op,OU_predicate) >= 0) {
    Set_OP_omega (new_op, (UINT32)OP_find_opnd_use(new_op,OU_predicate), pred_tninfo->omega);
  }
  return;
}


/* =====================================================================
 *   EBO_OPS_omega
 * =====================================================================
 */
void
EBO_OP_omega (
  OP *op, 
  TN *opnd,
  EBO_TN_INFO *opnd_tninfo
)
{
  INT opnds = OP_opnds(op);
  INT i;

  if (opnd_tninfo == NULL) return;


  CG_LOOP_Init_Op(op);
  for (i = 0; i < opnds; i++) {
    if (OP_opnd(op, i) == opnd) {
      Set_OP_omega (op, i, opnd_tninfo->omega);
    }
  }
}

/* =====================================================================
 *   EBO_OPS_omega
 * =====================================================================
 */
void
EBO_OPS_omega (
  OPS *ops, 
  TN *opnd,
  EBO_TN_INFO *opnd_tninfo
)
{
  OP *next_op = OPS_first(ops);
  if (opnd_tninfo == NULL) return;

  while (next_op != NULL) {
    EBO_OP_omega(next_op, opnd, opnd_tninfo);
    next_op = OP_next(next_op);
  }

  return;
}

/* =====================================================================
 *   EBO_OP_predicate_dominates(op1, op1_tninfo, op2, op2_tninfo)
 *
 *   Check whether the execution condition p1 for op1 dominates
 *   the execution condition p2 for op2 by checking whether
 *   EBO_predicate_domintes(p1, p2).
 *
 *   EBO_OP_predicate_complements(op1, op1_tninfo, op2, op2_tninfo)
 *
 *   Check whether the execution condition p1 for op1 and
 *   the execution condition p2 for op2 are complement by checking 
 *   whether EBO_predicate_complements(p1, p2).
 *
 *   EBO_OP_predicate_always(op, op_tninfo)
 *
 *   Check whether the operation is not predicated or the 
 *   predicate condition for the operation is such that it is always true.
 *      	
 *   EBO_OP_predicate_never(op, op_tninfo)
 *
 *   Check whether the operation is predicated and the predicate condition
 *   for the operation is such that it is never true.
 *      	
 *   All functions handle the specific cases where the operations are 
 *   not predicated considering that a non predicated op is equivalent
 *   to an op predicated by an always true predicate.
 *
 *   Note that these functions consider that all side effects of
 *   the operations are under the predicate condition given
 *   by the OU_predicate property.
 *   Thus the result of these queries must be used only for transformations
 *   on the operations results actually under the OU_predicate condition.
 *   Special cases such as automodification or hidden effect
 *   that may be unconditional must be handled differently by the
 *   client of this function.
 *   
 *   Note that the implementation may change or be improved by the
 *   use of the PQS queries when available.
 * =====================================================================  
 */
static BOOL
EBO_OP_predicate_always(OP *op, EBO_TN_INFO **op_opnd_tninfo)
{
  INT p_idx = OP_find_opnd_use(op, OU_predicate);
  return !OP_has_predicate(op) ||
    (True_TN != NULL && OP_opnd(op, p_idx) == True_TN && !OP_Pred_False(op, p_idx)) ||
    (Zero_TN != NULL && OP_opnd(op, p_idx) == Zero_TN && OP_Pred_False(op, p_idx));
}

static BOOL
EBO_OP_predicate_never(OP *op, EBO_TN_INFO **op_opnd_tninfo)
{
  INT p_idx = OP_find_opnd_use(op, OU_predicate);
  return OP_has_predicate(op) &&
    ((Zero_TN != NULL && OP_opnd(op, p_idx) == Zero_TN && !OP_Pred_False(op, p_idx)) ||
     (True_TN != NULL && OP_opnd(op, p_idx) == True_TN && OP_Pred_False(op, p_idx)));
}

#ifndef TARG_ST
static
#endif
BOOL
EBO_OP_predicate_dominates(OP *op1, EBO_TN_INFO **op1_opnd_tninfo,
			   OP *op2, EBO_TN_INFO **op2_opnd_tninfo)
{
  INT p1_idx = OP_find_opnd_use(op1, OU_predicate);
  INT p2_idx = OP_find_opnd_use(op2, OU_predicate);
  return 
    EBO_OP_predicate_always(op1, op1_opnd_tninfo) || 
    (OP_has_predicate(op2) && OP_has_predicate(op1) && 
     EBO_predicate_dominates(OP_opnd(op1, p1_idx), OP_Pred_False(op1, p1_idx), op1_opnd_tninfo[p1_idx],
			     OP_opnd(op2, p2_idx), OP_Pred_False(op2, p2_idx), op2_opnd_tninfo[p2_idx]));
}

static BOOL
EBO_OP_predicate_complements(OP *op1, EBO_TN_INFO **op1_opnd_tninfo,
			     OP *op2, EBO_TN_INFO **op2_opnd_tninfo)
{
  INT p1_idx = OP_find_opnd_use(op1, OU_predicate);
  INT p2_idx = OP_find_opnd_use(op2, OU_predicate);
  return 
    (EBO_OP_predicate_always(op1, op1_opnd_tninfo) &&
     EBO_OP_predicate_never(op1, op1_opnd_tninfo)) ||
    (EBO_OP_predicate_never(op1, op1_opnd_tninfo) &&
     EBO_OP_predicate_always(op1, op1_opnd_tninfo)) ||
    (OP_has_predicate(op1) && OP_has_predicate(op2) &&
     EBO_predicate_complements(OP_opnd(op1, p1_idx), OP_Pred_False(op1, p1_idx), op1_opnd_tninfo[p1_idx],
			       OP_opnd(op2, p2_idx), OP_Pred_False(op2, p2_idx), op2_opnd_tninfo[p2_idx]));
}

/* =====================================================================
 * Additional helper functions
 * =====================================================================
 */
static INT64 
sext(INT64 value, int bits)
{
  return (value << 64-bits) >> 64-bits;
}

static INT64 
zext(INT64 value, int bits)
{
  return (UINT64)(value << 64-bits) >> 64-bits;
}

static TN **
OP_opnds_ptr(OP *op)
{
  return &op->res_opnd[OP_opnd_offset(op)];
}

#endif //TARG_ST

/* ===================================================================== */
/* Local Data:								 */
/* ===================================================================== */

static BOOL in_delay_slot = FALSE;
static BOOL rerun_cflow = FALSE;

#if defined( KEY) && !defined(TARG_ST)
/* ===================================================================== */
/* Track register pressure within a basic block:                         */
/* ===================================================================== */

// Number of registers available in each register class.
static int avail_regs_count[ISA_REGISTER_CLASS_MAX+1];

// Map OP to the change in the number of registers used at OP.
static OP_MAP Regs_Delta_Map[ISA_REGISTER_CLASS_MAX+1];

// Map TN to EBO_REG_ENTRY for the current BB.
static hTN_MAP regs_map = NULL;

// Based on code in hb_sched.h.
typedef union {
  void *ptr;
  struct {
    unsigned int def_count : 15;
    unsigned int last_use_op_num : 15;	// OP where TN is last used in BB
    unsigned int reg_assigned : 1;
  }s;
} EBO_REG_ENTRY;

#define EBO_REG_ENTRY_ptr(re)			(re.ptr)
#define EBO_REG_ENTRY_def_count(re)		(re.s.def_count)
#define EBO_REG_ENTRY_reg_assigned(re)		(re.s.reg_assigned)
#define EBO_REG_ENTRY_last_use_OP_num(re)	(re.s.last_use_op_num)

// Determine the change in register pressure at each OP in the BB.  Based on
// code in hb_sched.cxx.
static void
Estimate_Reg_Usage (BB *bb, MEM_POOL *pool)
{
  OP *op;
  EBO_REG_ENTRY reginfo;
  INT32 global_regs_used[ISA_REGISTER_CLASS_MAX+1],
	regs_delta[ISA_REGISTER_CLASS_MAX+1];
  ISA_REGISTER_CLASS cl;
  INT i, op_num = 0;

  regs_map = hTN_MAP_Create(pool);

  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    global_regs_used[cl] = 0;
  }

  // Build regs_map.  Count the number of GTNs and dedicated TNs referenced in
  // the basic block.
  FOR_ALL_BB_OPs_FWD (bb, op) {
    op_num++;
    for (i = 0; i < OP_results(op); i++) {
      TN *result_tn = OP_result(op, i);
      EBO_REG_ENTRY_ptr(reginfo) =  hTN_MAP_Get (regs_map, result_tn);
      EBO_REG_ENTRY_def_count(reginfo)++;
      if (TN_is_global_reg(result_tn) || TN_is_dedicated(result_tn)) {
	if (!EBO_REG_ENTRY_reg_assigned(reginfo)) {
	  cl = TN_register_class(result_tn);
	  global_regs_used[cl]++;
	  EBO_REG_ENTRY_reg_assigned(reginfo) = TRUE;
	}
      }
      hTN_MAP_Set (regs_map, result_tn, EBO_REG_ENTRY_ptr(reginfo));
    }
    for (i = 0; i < OP_opnds(op); i++) {
      TN *opnd_tn = OP_opnd(op,i);
      if (TN_is_constant(opnd_tn)) continue;
      if (TN_is_global_reg(opnd_tn) || TN_is_dedicated(opnd_tn)) {
        EBO_REG_ENTRY_ptr(reginfo) = hTN_MAP_Get (regs_map, opnd_tn);
	if (!EBO_REG_ENTRY_reg_assigned(reginfo)) {
	  cl = TN_register_class(opnd_tn);
	  global_regs_used[cl]++;
	  EBO_REG_ENTRY_reg_assigned(reginfo) = TRUE;
	  hTN_MAP_Set (regs_map, opnd_tn, EBO_REG_ENTRY_ptr(reginfo));
	}
      }
    }
  }

  // Determine the change in register pressure at each OP.
  FOR_ALL_BB_OPs_REV (bb, op) {
    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      regs_delta[cl] = 0;
    }
    for (i = 0; i < OP_results(op); i++) {
      TN *result_tn = OP_result(op, i);
      EBO_REG_ENTRY_ptr(reginfo) = hTN_MAP_Get (regs_map, result_tn);
      EBO_REG_ENTRY_def_count(reginfo)--;
      if (EBO_REG_ENTRY_def_count(reginfo) == 0 &&
	  EBO_REG_ENTRY_reg_assigned(reginfo)) 
	{
	  ISA_REGISTER_CLASS cl = TN_register_class(result_tn);
	  regs_delta[cl]++;	// delta for forward BB traversal
	  EBO_REG_ENTRY_reg_assigned(reginfo) = FALSE;
	}
      hTN_MAP_Set (regs_map, result_tn, EBO_REG_ENTRY_ptr(reginfo));
    }
    for (i = 0; i < OP_opnds(op); i++) {
      TN *opnd_tn = OP_opnd(op,i);
      if (TN_is_constant(opnd_tn)) continue;
      EBO_REG_ENTRY_ptr(reginfo) = hTN_MAP_Get (regs_map, opnd_tn);
      if (!EBO_REG_ENTRY_reg_assigned(reginfo)) {
	ISA_REGISTER_CLASS cl = TN_register_class(opnd_tn);
	regs_delta[cl]--;	// delta for forward BB traversal
	EBO_REG_ENTRY_reg_assigned(reginfo) = TRUE;
	if (EBO_REG_ENTRY_last_use_OP_num(reginfo) == 0)
	  EBO_REG_ENTRY_last_use_OP_num(reginfo) = op_num;
	hTN_MAP_Set (regs_map, opnd_tn, EBO_REG_ENTRY_ptr(reginfo));
      }
    }
    // Account for global register usage by adding the globals to the first
    // OP's delta.
    if (op_num == 1) {
      FOR_ALL_ISA_REGISTER_CLASS(cl) {
	regs_delta[cl] += global_regs_used[cl];
      }
    }
    // Record the deltas.
    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      OP_MAP32_Set(Regs_Delta_Map[cl], op, regs_delta[cl]);
    }
    op_num--;
  }
}

// Extend the live range of TN to OP_NUM.
static void
Extend_Live_Range (int *regs_used, int *last_fat_point, int op_num,
		   EBO_TN_INFO *tninfo, EBO_REG_ENTRY reginfo, BB *bb)
{
  int i;
  ISA_REGISTER_CLASS cl = TN_register_class(tninfo->local_tn);
  int regs_avail = avail_regs_count[cl];

  // Determine the OP that previously last uses the live range.
  int last_use_op = (tninfo->in_bb == bb) ? 
		      EBO_REG_ENTRY_last_use_OP_num(reginfo) : 1;
  // Update register usage and fat point.
  for (i = op_num - 1; i >= last_use_op; i--) {
    EBO_REG(regs_used, cl, i)++;
    if (EBO_REG(regs_used, cl, i) > regs_avail) {
      last_fat_point[cl] = i;
      // Don't update register usage before the fat point since we don't care
      // about those usages.
      break;
    }
  }

  // Extend live range to include the OP at op_num.
  EBO_REG_ENTRY_last_use_OP_num(reginfo) = op_num;
  hTN_MAP_Set(regs_map, tninfo->local_tn, EBO_REG_ENTRY_ptr(reginfo));
}

// Update the register usage.  Adjust the fat points to indicate register file
// overflow.  CSE will not extend a TN's live range beyond a fat point.
static void
Update_Reg_Usage (OP *op, int op_num, int *regs_used, int *last_fat_point,
		  BOOL op_is_deleted_load)
{
  ISA_REGISTER_CLASS cl;

  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    int regs_delta_at_op = OP_MAP32_Get(Regs_Delta_Map[cl], op);
    int regs_avail = avail_regs_count[cl];
    int regs_used_after_op =
	  EBO_REG(regs_used, cl, op_num-1) + regs_delta_at_op;
    if (regs_used_after_op > regs_avail)
      last_fat_point[cl] = op_num;
    EBO_REG(regs_used, cl, op_num) = regs_used_after_op;
  }

  // If OP is a deleted load, then no register is needed for the result.
  if (op_is_deleted_load) {
    cl = TN_register_class(OP_result(op, 0));
    EBO_REG(regs_used, cl, op_num)--;
  }
}
#endif	// KEY

/* ===================================================================== */

/* The BB flag: <local_flag1> is overloaded temporarily in this routine
 * as <visited> to keep track of the fact that we have seen this block
 * during processing. The bit will NOT be cleared by the time we exit.
 */
#define BB_visited          BB_local_flag1
#define Set_BB_visited      Set_BB_local_flag1
#define Reset_BB_visited    Reset_BB_local_flag1

inline void clear_bb_flag(BB *first_bb)
{
  BB *bb;
  for (bb = first_bb; bb != NULL; bb = BB_next(bb)) {
    BBLIST *succ_list;

    Reset_BB_visited(bb);

    FOR_ALL_BB_SUCCS(bb, succ_list) { 
      BB *succ = BBLIST_item(succ_list);
      Reset_BB_visited(succ);
    }

  }
}

/* ===================================================================== */

static
BOOL EBO_Fix_Select_Same_Args (OP *op,
			       TN **opnd_tn,
			       EBO_TN_INFO **opnd_tninfo)
{
  TN *res = OP_result(op, 0);
  TN *tn1;
  TN *tn2;

  /* For special case optimizations, check the OPTIMAL operands. */
  tn1 = opnd_tn[1];
  tn2 = opnd_tn[2];

  if (tn_registers_identical(tn1, tn2) || (TN_is_zero(tn1) && TN_is_zero(tn2))) {
    /* We can optimize this! But return the ACTUAL operand. */
    OPS ops = OPS_EMPTY;
#ifdef TARG_ST
    EBO_Exp_COPY(OP_Predicate (op), OP_PredOnFalse(op), OP_cond_def(op), res, OP_opnd(op, 1), &ops);
#else
    Exp_COPY(res, OP_opnd(op, 1), &ops);
#endif
    BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
    if (EBO_Trace_Optimization) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile,"Optimize select - operands are the same\n");
    }
    return TRUE;
  }

  return FALSE;
}

/* ===================================================================== */


/* ===================================================================== */

static
BOOL EBO_Fix_Same_Res_Op (OP *op,
                          TN **opnd_tn,
                          EBO_TN_INFO **opnd_tninfo)
{
  if (EBO_in_loop) return FALSE;;

  if (OP_unalign_ld (op)) {
#ifndef TARG_ST
    UINT8 idx = OP_find_opnd_use (op, OU_same_res);
#endif
    TN   *res = OP_result (op, 0);
#ifdef TARG_ST
    TN *tnl = OP_opnd(op, OP_opnds(op)-1);
#else
    TN   *tnl = OP_opnd (op, idx);
#endif
    if (!TN_is_zero_reg(tnl) && !tn_registers_identical(res, tnl)) {
     /* Allocate a new TN for the result. */
      OPS ops = OPS_EMPTY;
      TN *new_res = Dup_TN (res);
      OP *new_op = Dup_OP (op);
#ifdef TARG_ST
      EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), new_res, tnl, &ops);
#else
      Exp_COPY(new_res, tnl, &ops);
#endif
      Set_OP_result(new_op, 0, new_res);
      Set_OP_opnd(new_op, OP_opnds(op)-1, new_res);
      OPS_Append_Op(&ops, new_op);
#ifdef TARG_ST
      EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), res, new_res, &ops);
#else
      Exp_COPY(res, new_res, &ops);
#endif
      BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      return TRUE;
    }
  }
  else if (OP_select(op)) {
    TN *res = OP_result(op, 0);
    TN *tn0 = OP_opnd(op, 0);
    TN *tn1;
    TN *tn2;

   /* For special case optimizations, check the OPTIMAL operands. */
    tn1 = opnd_tn[1];
    tn2 = opnd_tn[2];

    if (tn_registers_identical(tn1, tn2)) {
     /* We can optimize this! But return the ACTUAL operand. */
      OPS ops = OPS_EMPTY;
      Exp_COPY(res, OP_opnd(op, 1), &ops);
      BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      if (EBO_Trace_Optimization) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"Optimize select - operands are the same\n");
      }
      return TRUE;
    }

    if (TN_is_global_reg(tn1) &&
        (opnd_tninfo[1] != NULL) &&
        (opnd_tninfo[1]->in_op == NULL) &&
        (opnd_tninfo[1]->in_bb != NULL) &&
	(opnd_tninfo[1]->in_bb != OP_bb(op)) &&
        !tn_has_live_def_into_BB(tn1, opnd_tninfo[1]->in_bb)) {
     /* Assume that this value will not be used -
        turn this instruction into a copy of the other operand.
        But return the ACTUAL operand. */
      OPS ops = OPS_EMPTY;
#ifdef TARG_ST
      EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), res, OP_opnd(op, 2), &ops);
#else
      Exp_COPY(res, OP_opnd(op, 2), &ops);
#endif
      BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      if (EBO_Trace_Optimization) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"Optimize select - op1 %s can't be used: ",
                EBO_in_peep ? "REG" : "TN");
        Print_TN(OP_opnd(op, 2), FALSE);
        fprintf(TFile,"\n");
      }
      return TRUE;
    }

    if (TN_is_global_reg(tn2) &&
        (opnd_tninfo[2] != NULL) &&
        (opnd_tninfo[2]->in_op == NULL) &&
        (opnd_tninfo[2]->in_bb != NULL) &&
        (opnd_tninfo[2]->in_bb != OP_bb(op)) &&
        !tn_has_live_def_into_BB(tn2, opnd_tninfo[2]->in_bb)) {
    /* Assume that this value will not be used -
        turn this instruction into a copy of the other operand.
        But return the ACTUAL operand. */
      OPS ops = OPS_EMPTY;
#ifdef TARG_ST
      EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), res, OP_opnd(op, 1), &ops);
#else
      Exp_COPY(res, OP_opnd(op, 1), &ops);
#endif
      BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      if (EBO_Trace_Optimization) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"Optimize select - op2 %s can't be used: ",
                EBO_in_peep ? "REG" : "TN");
        Print_TN(OP_opnd(op, 1), FALSE);
        fprintf(TFile,"\n");
      }
      return TRUE;
    }

   /* For ensuring that the result operand matches one of the input operands,
      test the ACTUAL operands used in the expression. */
    tn0 = OP_opnd(op, 0);
    tn1 = OP_opnd(op, 1);
    tn2 = OP_opnd(op, 2);
    if ((TN_Is_Constant(tn1) || !tn_registers_identical(res, tn1)) &&
        (TN_Is_Constant(tn2) || !tn_registers_identical(res, tn2))) {
      OPS ops = OPS_EMPTY;
      OP *new_op = Dup_OP (op);

      if (has_assigned_reg(res)) {
       /* Use the existing result TN as the duplicate input. */
        TN *new_res = OP_result(op,0);
        FmtAssert((TN_Is_Constant(tn0) || !tn_registers_identical(res, tn0)),
                  ("Condition code also used as result of select"));
        if (TN_Is_Constant(tn2)) {
#ifdef TARG_ST
          EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), new_res, tn2, &ops);
#else
          Exp_COPY(new_res, tn2, &ops);
#endif
          Set_OP_opnd(new_op, 2, new_res);
        } else {
#ifdef TARG_ST
          EBO_Exp_COPY(OP_Predicate (op), OP_PredOnFalse(op), OP_cond_def(op), new_res, tn1, &ops);
#else
          Exp_COPY(new_res, tn1, &ops);
#endif
          Set_OP_opnd(new_op, 1, new_res);
        }
        OPS_Append_Op(&ops, new_op);
      } else {
       /* Allocate a new TN for the result. */
        TN *new_res = Dup_TN (res);
#ifdef TARG_ST
        EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), new_res, tn1, &ops);
#else
        Exp_COPY(new_res, tn1, &ops);
#endif
        Set_OP_result(new_op, 0, new_res);
        Set_OP_opnd(new_op, 1, new_res);
        OPS_Append_Op(&ops, new_op);
#ifdef TARG_ST
        EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), res, new_res, &ops);
#else
        Exp_COPY(res, new_res, &ops);
#endif
      }

      BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      if (EBO_Trace_Optimization) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"Rewrite select\n");
      }
      return TRUE;
    }
  }

  return FALSE;

}

BOOL TN_live_out_of(TN *tn, BB *bb)
/* -----------------------------------------------------------------------
 * Requires: global liveness info up-to-date
 * Return TRUE iff <tn> is live out of <bb>.
 * -----------------------------------------------------------------------
 */
{
  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter TN_live_out_of BB:%d ",EBO_trace_pfx,BB_id(bb));
    Print_TN(tn, FALSE);
    fprintf(TFile,"\n");
  }

#ifdef KEY
  if (EBO_no_liveness_info_available)
    return TRUE;
#endif

  if (EBO_in_peep) {
    if (EBO_Trace_Data_Flow) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile,"REG_LIVE_Outof_BB %s\n",
         REG_LIVE_Outof_BB (TN_register_class(tn), TN_register(tn), bb)?"TRUE":"FALSE");
    }
    return REG_LIVE_Outof_BB (TN_register_class(tn), TN_register(tn), bb);
  }
  else {
    if (CG_localize_tns) return (TN_is_dedicated(tn) || TN_is_global_reg(tn));
    return GRA_LIVE_TN_Live_Outof_BB (tn, bb);
  }
}



inline BOOL op_is_needed_globally(OP *op)
/* -----------------------------------------------------------------------
 * Requires: global liveness info
 * Return TRUE iff the result of <op> is necessary past the end of its BB.
 * ----------------------------------------------------------------------- */
{
  BB *bb = OP_bb(op);

  if (OP_copy(op)) {
   /* Copies don't have side effects unless a save_reg is involved. */
    if (OP_glue(op) && !EBO_in_peep)
      return TRUE;
#ifdef TARG_ST
    int result_num = OP_Copy_Result(op);
    int opnd_num = OP_Copy_Operand(op);
    if ((TN_is_save_reg(OP_result(op,result_num)) || 
	 TN_is_save_reg(OP_opnd(op,opnd_num))) &&
        !tn_registers_identical(OP_result(op,result_num), OP_opnd(op,opnd_num))) {
#else
    if ((TN_is_save_reg(OP_result(op,0)) || TN_is_save_reg(OP_opnd(op,OP_COPY_OPND))) &&
        !tn_registers_identical(OP_result(op,0), OP_opnd(op,OP_COPY_OPND))) {
#endif
      return TRUE;
    }
#ifdef TARG_ST
    // FdF Copy operation with AFFIRM property are also needed, until
    // EBO_main (codex #47949)
    if (EBO_in_pre && OP_Is_Affirm(op))
      return TRUE;
#endif
    return FALSE;
  }
#ifdef TARG_ST
  // [CG]: iterate over all results
  for (int i = 0; i < OP_results(op); i++) {
    if (TN_is_save_reg(OP_result(op,i)))
      return TRUE;
  }
#else
  if (TN_is_save_reg(OP_result(op,0)))
    return TRUE;
#endif
  if (OP_glue(op) && !has_assigned_reg(OP_result(op,0)))
    return TRUE;
  if (CGTARG_Is_OP_Intrinsic(op))
  {
   /* Intrinsic ops may have side effects we don't understand */
    return TRUE;
  }
  if (OP_call(op)) 
   /* Calls may have side effects we don't understand */
    return TRUE;
#ifdef TARG_ST
  // FdF 20060810: Because jrgtudec instructions in STxP70 are not
  // considered otherwise.
  if (OP_xfer(op))
    return TRUE;
  // TDR check that we deal with an instruction already inserted in a bb
  if (bb)
#endif
  if (op == BB_exit_sp_adj_op(bb) || op == BB_entry_sp_adj_op(bb))
    return TRUE;
  return FALSE;
}
#ifdef TARG_ST
/* 
 * [CG]: Live analysis is now incrementally updated to enabled better dead code.
 * It is also required for redundancy and dead code elimination correctness
 * as soon as redundancy elimination applies to global TNs.
 *
 * For instance:
 * X = opA
 * --- end of block (X is not liveout, as it is dead)
 * X = opA
 * Will be tranformed by the forward pass into
 * X = opA
 * --- end of block (X must now be liveout, otherwise the operation will be removed)
 * nop
 * 
 * Thus in the backward pass, the livein of the second block and the livout
 * of the first block must be updated for correctness
 * Ref codex bug #37014.
 */

/* 
 * EBO_Update_Livein(BB *bb)
 *
 * This function does the update of the live in for the current bb given the
 * current live out set.
 * This function must be called in the backward pass on the extended blocks.
 */
static void
EBO_Update_Livein(BB *bb)
{
  if (EBO_in_peep) {
    if (EBO_Trace_Data_Flow) {
#pragma mips_frequency_hint NEVER
      fprintf(TFile,"EBO_Update_Livein REG_LIVE for BB:%d\n", BB_id(bb));
    }
    REG_LIVE_Update_Livein_From_Liveout(bb);
  } else {
    if (!CG_localize_tns) {
      if (EBO_Trace_Data_Flow) {
#pragma mips_frequency_hint NEVER
	fprintf(TFile,"EBO_Update_Livein GRA_LIVE for BB:%d\n", BB_id(bb));
      }
      GRA_LIVE_Compute_Liveness_For_BB(bb);
    }
  }
}
/* ===================================================================== */
/* 
 * EBO_Update_Liveout(BB *bb)
 *
 * This function updates the livout set from the existing livein sets of the successors.
 * This function must be called in the backward pass on the extended blocks.
 */
static void
EBO_Update_Liveout(BB *bb)
{
  if (EBO_in_peep) {
    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"EBO_Update_Liveout REG_LIVE for BB:%d\n", BB_id(bb));
    }
    // In Peep, there is no need to update live out as the liveout
    // information is not stored.
    // The query REG_LIVE_Outof_BB() recomputes it each time it is called.
  } else {
    if (EBO_Trace_Data_Flow) {
#pragma mips_frequency_hint NEVER
      fprintf(TFile,"EBO_Update_Liveout GRA_LIVE for BB:%d\n", BB_id(bb));
    }
    if (!CG_localize_tns) {
      // Before allocation in global tn mode, the liveout
      // is stored. Thus, it must be recomputed.
      GRA_LIVE_Compute_Liveness_For_BB(bb);
    }
  }
}
#endif


/* ===================================================================== */


void
tn_info_entry_dump (EBO_TN_INFO *tninfo)
{
  fprintf(TFile,"entry %d\tBB:%d, use count = %d, redefined = %s, same as %d, predicate %d:  ",
          tninfo->sequence_num,tninfo->in_bb?BB_id(tninfo->in_bb):0,
          tninfo->reference_count,
          tninfo->redefined_before_block_end?"TRUE":"FALSE",
          tninfo->same?tninfo->same->sequence_num:0,
          tninfo->predicate_tninfo?tninfo->predicate_tninfo->sequence_num:0);
  Print_TN (tninfo->local_tn, TRUE);
  fprintf(TFile,"[%d]",tninfo->omega);
  if (tninfo->replacement_tn != NULL) {
    fprintf(TFile,"\n\treplace TN with: ");
    Print_TN (tninfo->replacement_tn, TRUE);
    if (tninfo->replacement_tninfo != NULL) {
      fprintf(TFile," (Entry Number %d)",
              tninfo->replacement_tninfo->sequence_num);
    }
  }
  fprintf(TFile,"\n");

#ifdef TARG_ST
  if (tninfo->copy_tn != NULL) {
    fprintf(TFile,"\n\tcopy of TN: ");
    Print_TN (tninfo->copy_tn, TRUE);
    if (tninfo->copy_tninfo != NULL) {
      fprintf(TFile," (Entry Number %d)",
              tninfo->copy_tninfo->sequence_num);
    }
  }
  fprintf(TFile,"\n");
#endif

  if (tninfo->in_op) {
    fprintf(TFile,"\t");
    Print_OP_No_SrcLine(tninfo->in_op);
  }

  FmtAssert((TN_number(tninfo->local_tn) <= Last_TN),
                  ("TN number exceeds allowed range"));

}


void
tn_info_table_dump ()
{
  EBO_TN_INFO *tninfo = EBO_first_tninfo;

  fprintf(TFile,"\n>>>>>> EBO INFO DUMP <<<<<\n");

  while (tninfo != NULL) {
    tn_info_entry_dump(tninfo);
    tninfo = tninfo->next;
  }

  fprintf(TFile,">>>>>> EBO INFO DUMP COMPLETE <<<<<\n\n");

}



void EBO_Init(void)
/* -----------------------------------------------------------------------
 * See "ebo.h" for interface.
 * -----------------------------------------------------------------------
 */
{

  EBO_Trace_Execution    = FALSE;
  EBO_Trace_Optimization = FALSE;
  EBO_Trace_Block_Flow   = FALSE;
  EBO_Trace_Data_Flow    = FALSE;
  EBO_Trace_Hash_Search  = FALSE;

  MEM_POOL_Initialize(&EBO_pool, "ebo", FALSE);
  MEM_POOL_Push(&EBO_pool);
  EBO_tninfo_table = NULL;

  EBO_tninfo_number = 0;
  EBO_free_tninfo = NULL;
  EBO_first_tninfo = NULL;
  EBO_last_tninfo = NULL;

  EBO_free_opinfo = NULL;
  EBO_first_opinfo = NULL;
  EBO_last_opinfo = NULL;

  memset(EBO_opinfo_table, 0, sizeof(EBO_opinfo_table));

  EBO_num_tninfo_entries = 0;
  EBO_tninfo_entries_reused = 0;
  EBO_num_opinfo_entries = 0;
  EBO_opinfo_entries_reused = 0;
  EBO_trace_pfx = "<ebo> ";

#if defined( KEY) && !defined(TARG_ST)
  // Find the number of registers available in each register class.
  ISA_REGISTER_CLASS cl;
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    REGISTER_SET reg_set = REGISTER_CLASS_allocatable(cl);
    REGISTER reg;
    avail_regs_count[cl] = 0;
    for (reg = REGISTER_SET_Choose(reg_set);
	 reg != REGISTER_UNDEFINED;
	 reg = REGISTER_SET_Choose_Next(reg_set, reg)) {
      avail_regs_count[cl]++;
    }
    // Tune the number of registers available for CSE.
    if (cl == ISA_REGISTER_CLASS_integer)
      avail_regs_count[cl] += CG_cse_regs;
    else if (cl == ISA_REGISTER_CLASS_float)
      avail_regs_count[cl] += CG_sse_cse_regs;
  }
#endif
}




static void EBO_Start()
/* -----------------------------------------------------------------------
 * -----------------------------------------------------------------------
 */
{
 /* Initialize data structures.  */
  MEM_POOL_Push(&MEM_local_pool);
  EBO_tninfo_table = TN_MAP_Create();

#ifdef TARG_X8664
  if (!EBO_in_peep && !EBO_in_after_unrolling && !Are_Dominators_Calculated()) {
    EBO_doms_calculated = TRUE;
    Calculate_Dominators();
  }
#endif

#if defined( KEY) && !defined(TARG_ST)
  ISA_REGISTER_CLASS cl;
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    Regs_Delta_Map[cl] = OP_MAP32_Create();
  }

  EBO_Special_Start( &MEM_local_pool );
#endif // KEY
}



static void EBO_Finish(void)
/* -----------------------------------------------------------------------
 * -----------------------------------------------------------------------
 */
{
#if defined( KEY) && !defined(TARG_ST)
  ISA_REGISTER_CLASS cl;
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    OP_MAP_Delete(Regs_Delta_Map[cl]);
    Regs_Delta_Map[cl] = NULL;
  }

  EBO_Special_Finish();
#endif

#ifdef TARG_X8664
  if (!EBO_in_peep && !EBO_in_after_unrolling && EBO_doms_calculated) {
    EBO_doms_calculated = FALSE;
    Free_Dominators_Memory();
  }
#endif

  TN_MAP_Delete (EBO_tninfo_table);
  EBO_tninfo_table = NULL;
  MEM_POOL_Pop(&MEM_local_pool);
}




void EBO_Finalize(void)
/* -----------------------------------------------------------------------
 * See "ebo.h" for interface.
 * -----------------------------------------------------------------------
 */
{
  MEM_POOL_Pop(&EBO_pool);
  MEM_POOL_Delete(&EBO_pool);
}


/* ===================================================================== */

  
static INT copy_operand(OP *op)
/* -------------------------------------------------------------------------
 *
 * Return the operand index for the operand that is copied.
 * -------------------------------------------------------------------------
 */
{
#ifdef TARG_ST
  return OP_Copy_Operand(op);
#else
  INT opnd;

  if (OP_copy(op)) {
    return OP_COPY_OPND;
  }
  opnd = CGTARG_Copy_Operand(op);
  if (opnd >= 0) {
    return opnd;
  }
  opnd = EBO_Copy_Operand(op);
  if (opnd >= 0) {
    return opnd;
  }
  return -1;
#endif
}



static void
find_index_and_offset (EBO_TN_INFO *original_tninfo,
                       TN **base_tn, EBO_TN_INFO **base_tninfo,
                       TN **offset_tn, EBO_TN_INFO **offset_tninfo)
{
 /* Look for an offset descriptor in the index. */
  EBO_OP_INFO *indx_opinfo = locate_opinfo_entry(original_tninfo);

#ifdef TARG_ST
  //
  // Use target-specific routine to get the indices
  //
  INT op1_idx;
  INT op2_idx;

  if ((indx_opinfo != NULL) &&
      (indx_opinfo->in_op != NULL) &&
      (OP_iadd(indx_opinfo->in_op) || 
       EBO_Can_Merge_Into_Offset(indx_opinfo->in_op, &op1_idx, &op2_idx))) {
#else
  if ((indx_opinfo != NULL) &&
      (indx_opinfo->in_op != NULL) &&
      (OP_iadd(indx_opinfo->in_op) || 
       EBO_Can_Merge_Into_Offset(indx_opinfo->in_op))) {
#ifdef TARG_X8664
    INT op1_idx = 0;
    INT op2_idx = 1;
#else
    INT op1_idx = TOP_Find_Operand_Use(OP_code(indx_opinfo->in_op),OU_opnd1);
    INT op2_idx = TOP_Find_Operand_Use(OP_code(indx_opinfo->in_op),OU_opnd2);
#endif
#endif
    if ((op1_idx >= 0) && (op2_idx >= 0)) {
      EBO_TN_INFO *op1_tninfo = indx_opinfo->actual_opnd[op1_idx];
      EBO_TN_INFO *op2_tninfo = indx_opinfo->actual_opnd[op2_idx];
      TN *op1_tn;
      TN *op2_tn;
      if (op1_tninfo != NULL) {
        if ((op1_tninfo->replacement_tn) &&
            (TN_is_symbol(op1_tninfo->replacement_tn) || TN_Is_Constant(op1_tninfo->replacement_tn))) {
          op1_tn = op1_tninfo->replacement_tn;
          op1_tninfo = op1_tninfo->replacement_tninfo;
        } else {
          op1_tn = op1_tninfo->local_tn;
        }
      } else {
        op1_tn = OP_opnd(indx_opinfo->in_op,op1_idx);
      }
      if (op2_tninfo != NULL) {
        if ((op2_tninfo->replacement_tn) &&
            (TN_is_symbol(op2_tninfo->replacement_tn) || TN_Is_Constant(op2_tninfo->replacement_tn))) {
          op2_tn = op2_tninfo->replacement_tn;
          op2_tninfo = op2_tninfo->replacement_tninfo;
        } else {
          op2_tn = op2_tninfo->local_tn;
        }
      } else {
        op2_tn = OP_opnd(indx_opinfo->in_op,op2_idx);
      }

      if (TN_is_symbol(op1_tn) || TN_Is_Constant(op1_tn)) {
        TN *save = op1_tn;
        op1_tn = op2_tn;
        op2_tn = save;
        op1_tninfo = op2_tninfo;
        op2_tninfo = NULL;
      }

      *base_tn = op1_tn;
      *base_tninfo = op1_tninfo;
      *offset_tn = op2_tn;
      *offset_tninfo = op2_tninfo;

      if (EBO_Trace_Hash_Search) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sUse inputs to index: ",EBO_trace_pfx);
        Print_TN(*base_tn,FALSE);
        fprintf(TFile," ");
        Print_TN(*offset_tn,FALSE);
        fprintf(TFile,"\n");
      }
    }
  }
}



#ifndef TARG_ST
  // [CG]: Obsolete. This functionality is handled by EBO_Address_Sequence

/* =======================================================================
 *   merge_memory_offsets
 * =======================================================================
 */

static void
merge_memory_offsets( OP *op,
                      INT index_opnd,
                      TN **opnd_tn,
                      EBO_TN_INFO **opnd_tninfo,
                      EBO_TN_INFO **actual_tninfo)
{
  EBO_TN_INFO *index_tninfo = opnd_tninfo[index_opnd];
  OP *index_op = (index_tninfo != NULL) ? index_tninfo->in_op : NULL;
  TN *immed_tn = opnd_tn[index_opnd+1];
  ST *immed_sym = TN_is_symbol(immed_tn) ? TN_var(immed_tn) : NULL;
  INT64 immed_offset = TN_is_symbol(immed_tn) ? TN_offset(immed_tn) : TN_value(immed_tn);
  EBO_OP_INFO *index_opinfo;
  TN *additive_index_tn;
  TN *additive_immed_tn;
  EBO_TN_INFO *additive_index_tninfo;

  TN *adjust_tn = NULL;
  ST *adjust_sym = NULL;
  INT64 adjust_offset = 0;
  TN *new_tn;

  if ((index_tninfo == NULL) ||
      (index_op == NULL) ||
      (!EBO_Can_Merge_Into_Offset (index_op))) {
    return;
  }

  index_opinfo = locate_opinfo_entry (index_tninfo);
  if (index_opinfo == NULL) return;

  additive_index_tn = OP_opnd(index_op,0);
  additive_index_tninfo = index_opinfo->actual_opnd[0];
#ifdef TARG_X8664
  // Handling -fPIC in exp_loadstore.cxx exposes this bug when assembling bug 274
  if ( additive_index_tn == Rip_TN() )
    return;
#endif

  additive_immed_tn = OP_opnd(index_op,1);
  if (!TN_Is_Constant(additive_immed_tn)) return;

 /* Would the new index value be available for use? */
  if (!TN_Is_Constant(additive_index_tn) &&
      !EBO_tn_available(OP_bb(op), additive_index_tninfo)) {
    return;
  }

  if (TN_is_symbol(additive_immed_tn)) {
    if (immed_sym != NULL) return;
    if (OP_isub(index_op)) return;
    if (immed_offset != 0) {
     /* Don't move relocation from an ADD instruction to a memory
        offset field if there is a chance that the (relocation +
        offset) value might overflow the field. */
      return;
    }
    adjust_sym = TN_var(additive_immed_tn);
    adjust_tn = additive_immed_tn;
    adjust_offset = immed_offset + TN_offset(additive_immed_tn);
  } else {
    adjust_sym = immed_sym;
    adjust_tn = immed_tn;
    adjust_offset = TN_value(additive_immed_tn);
    if (OP_isub(index_op)) adjust_offset = -adjust_offset;
    adjust_offset += immed_offset;
  }

  if (!TOP_Can_Have_Immediate( adjust_offset, OP_code(op))) {\
    if (EBO_Trace_Optimization) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile, "%sin BB:%d combined index expressions do not fit in the offset field\n",
              EBO_trace_pfx, BB_id(OP_bb(op)));
      Print_OP_No_SrcLine(op);
    }
    return;
  }

  if (EBO_Trace_Optimization) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile, "%sin BB:%d merge memory index expression (from BB:%d) with offset (in BB:%d)\n",
            EBO_trace_pfx, BB_id(OP_bb(op)),BB_id(OP_bb(index_op)),BB_id(OP_bb(op)));
    Print_OP_No_SrcLine(index_op);
    Print_OP_No_SrcLine(op);
  }

 /* Create a combined TN and upated the op and other data structures. */
  if (adjust_sym != NULL) {
    new_tn = Gen_Symbol_TN(adjust_sym, adjust_offset, TN_relocs(adjust_tn));
  } else {
    new_tn = Gen_Literal_TN (adjust_offset, TN_size(adjust_tn));
  }

  if (actual_tninfo[index_opnd] != NULL) {
    dec_ref_count(actual_tninfo[index_opnd]);
  }
  if (additive_index_tninfo != NULL) {
    inc_ref_count(additive_index_tninfo);
  }
  Set_OP_opnd(op, index_opnd, additive_index_tn);
  if (EBO_in_loop) {
    Set_OP_omega (op, index_opnd, (additive_index_tninfo != NULL) ? additive_index_tninfo->omega : 0);
  }
  Set_OP_opnd(op, index_opnd+1, new_tn);
  if (EBO_in_loop) {
    Set_OP_omega (op, index_opnd+1, 0);
  }
  opnd_tn[index_opnd] = additive_index_tn;
  opnd_tn[index_opnd+1] = new_tn;
  opnd_tninfo[index_opnd] = additive_index_tninfo;
  opnd_tninfo[index_opnd+1] = NULL;
  actual_tninfo[index_opnd] = additive_index_tninfo;
  actual_tninfo[index_opnd+1] = NULL;

  if (EBO_Trace_Optimization) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile, "  to produce the new expression:\n");
    Print_OP_No_SrcLine(op);
  }

}
#endif // !TARG_ST
#ifdef TARG_ST  // ends at the end of function EBO_Fold_Constant_Expression

/* =====================================================================
 *   EBO_delete_subset_mem_op
 *
 *   For a (store|load) / load sequence, attempt to simplify the 
 *   load if redundant.
 *   Handle the case where the load is a subset of the first memory op.
 *   Return TRUE if the load has been simplified.
 *
 *   On a predicated architecture, the transformation looks like
 *     1: pA? X = load @A   or  1: store @A, X
 *     2: pC? Y = load @A
 *
 *   if pA DOM pC, it can be transformed into:
 *     1: pA? X = load @A   or  1: store @A, X
 *     2: pC? Y = extract_subset X
 *
 * =====================================================================
 */
BOOL
EBO_delete_subset_mem_op(
  OP *op,
  EBO_TN_INFO **opnd_tninfo,
  EBO_OP_INFO *opinfo,
  INT64 offset_pred,
  INT64 offset_succ
)
{
  OP *pred_op = opinfo->in_op;
  BB *bb = OP_bb(op);
  TN *pred_result = OP_store(pred_op) 
                      ? OP_opnd(pred_op, TOP_Find_Operand_Use(OP_code(pred_op),OU_storeval))
                      : OP_result(pred_op,0);
  TN *succ_result = OP_store(op)
                      ? OP_opnd(op, TOP_Find_Operand_Use(OP_code(op),OU_storeval))
                      : OP_result(op,0);
  INT size_pred;
  INT size_succ;
  BOOL succ_is_subset = FALSE;
  INT byte_offset;
  OPS ops = OPS_EMPTY;

  if (EBO_Trace_Data_Flow) {
    fprintf(TFile,"%ssubset    OP in BB:%d    ",EBO_trace_pfx,BB_id(bb));
    Print_OP_No_SrcLine(op);
    fprintf(TFile,"      Matches   OP in BB:%d    ",BB_id(opinfo->in_bb));
    Print_OP_No_SrcLine(pred_op);
  }
  
  if ((Opt_Level < 2) && (bb != opinfo->in_bb)) {
   /* Global TN's aren't supported at low levels of optimization. */
    return FALSE;
  }

  if (!EBO_in_peep &&
      (bb != opinfo->in_bb) &&
      !TN_Is_Constant(pred_result) &&
      has_assigned_reg(pred_result)) {
    if (EBO_Trace_Data_Flow) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile,"%sShouldn't move dedicated register references across blocks.\n",
              EBO_trace_pfx);
    }
    return FALSE;
  }

  // Here we test the predicate preconditions for the transformation.
  if (!(EBO_OP_predicate_dominates(pred_op, opinfo->optimal_opnd,
				   op, opnd_tninfo))) {
    if (EBO_Trace_Hash_Search) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile,"%sDelet subset memop predicates do not match\n\t", EBO_trace_pfx);
      Print_OP_No_SrcLine(pred_op);
    }
    return FALSE;
  }

  size_pred = OP_Mem_Ref_Bytes(pred_op);
  size_succ = OP_Mem_Ref_Bytes(op);
  if (size_pred < size_succ) return FALSE;

  if ((offset_pred <= offset_succ) &&
      ((offset_pred + size_pred) >= (offset_succ + size_succ))) {
    succ_is_subset = TRUE;
  }

  if (!succ_is_subset) {
    return FALSE;
  }

  byte_offset = offset_succ - offset_pred;

  // This code is specific to ST200
  if (OP_load(op) && OP_load(pred_op) &&
      (OP_results(op) == 1) && (OP_results(pred_op) == 2) &&
      (2*size_succ == size_pred)) {
    // Assign pred_result
    
    // fix for codex bug #38550
    // Just use one of the 2 results for the predecessor. 
    if (OP_loadval_byte_offset (pred_op, 0) == byte_offset) {
      pred_result = OP_result (pred_op, 0);
    } else if (OP_loadval_byte_offset (pred_op, 1) == byte_offset) {
      pred_result = OP_result (pred_op, 1);
    } else {
      FmtAssert (FALSE, ("cannot match byte_offset"));
    }

    if (EBO_Trace_Optimization) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile,"%sRemove LoadPair - Load combination\n",EBO_trace_pfx);
    }

#ifdef TARG_ST
    EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), succ_result, pred_result, &ops);
#else
    EBO_Exp_COPY((OP_has_predicate(op)?OP_opnd(op,OP_PREDICATE_OPND):NULL),
                 succ_result, pred_result, &ops);
#endif

    if (EBO_in_loop) 
#ifdef TARG_ST
      EBO_OPS_omega (&ops, 
		     OP_Predicate(op),
		     (OP_has_predicate(op)? opnd_tninfo[OP_find_opnd_use(op,OU_predicate)]:NULL));
#else
      EBO_OPS_omega (&ops, 
		     (OP_has_predicate(op)? OP_opnd(op,OP_PREDICATE_OPND):NULL),
		     (OP_has_predicate(op)? opnd_tninfo[OP_PREDICATE_OPND]:NULL));
#endif
    OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);
    BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
    return TRUE;
  }

  if (!OP_store(pred_op) || !OP_load(op)) {
   /* Can only optimize Store - Load pattern. */
    return FALSE;
  }

  if (TOP_Find_Operand_Use(OP_code(op), OU_postincr) >= 0) {
   /* The increment must be preserved. */
    return FALSE;
  }

  return EBO_replace_subset_load_with_extract (op,
					       pred_result,
					       succ_result,
					       offset_pred,
					       offset_succ,
					       size_pred,
					       size_succ);
}

/* =====================================================================
 *   hoist_predicate_of_duplicate_complement
 * =====================================================================
 */
static BOOL
hoist_predicate_of_duplicate_complement (
  OP *op,
  EBO_TN_INFO **opnd_tninfo,
  EBO_OP_INFO *opinfo
)
{
  OP *pred_op = opinfo->in_op;
#ifdef TARG_ST
  TN *predicate1_tn = OP_Predicate(pred_op);
  TN *predicate2_tn = OP_Predicate(op);
  EBO_TN_INFO *predicate1_info = opinfo->actual_opnd[OP_find_opnd_use(pred_op,OU_predicate)];
  EBO_TN_INFO *predicate2_info = opnd_tninfo[OP_find_opnd_use(op,OU_predicate)];
#else
  TN *predicate1_tn = OP_opnd(pred_op,OP_PREDICATE_OPND);
  TN *predicate2_tn = OP_opnd(op,OP_PREDICATE_OPND);
  EBO_TN_INFO *predicate1_info = opinfo->actual_opnd[OP_PREDICATE_OPND];
  EBO_TN_INFO *predicate2_info = opnd_tninfo[OP_PREDICATE_OPND];
#endif
  BB *bb = OP_bb(op);
  INT i;
  INT val_idx;

  if (!OP_has_predicate(op) || !OP_has_predicate(pred_op))
    return FALSE;

  if (EBO_Trace_Data_Flow) {
    fprintf(TFile,"Enter hoist_predicate_of_duplicate_complement\n");
    Print_OP_No_SrcLine(pred_op);
    Print_OP_No_SrcLine(op);
    Print_TN(predicate1_tn,false);fprintf(TFile,"  ");
    Print_TN(predicate2_tn,false);fprintf(TFile,"\n");
    if (predicate1_info != NULL) tn_info_entry_dump(predicate1_info);
    if (predicate2_info != NULL) tn_info_entry_dump(predicate2_info);
  }

  if (OP_code(op) != OP_code(pred_op)) return FALSE;

 /* Pick up the new predicate that dominates both of those in the common expressions. */
  EBO_OP_INFO *new_predicate_opinfo = locate_opinfo_entry(predicate1_info);
  if (new_predicate_opinfo == NULL) return FALSE;

#ifdef TARG_ST
  EBO_TN_INFO *new_predicate_tninfo;
  TN *new_predicate_tn;
  BOOL new_predicate_tn_is_false;
  if (OP_has_predicate (predicate1_info->in_op)) {
    INT idx = OP_find_opnd_use(predicate1_info->in_op, OU_predicate);
    new_predicate_tninfo = new_predicate_opinfo->actual_opnd[idx];
    new_predicate_tn = OP_opnd(predicate1_info->in_op, idx);
    new_predicate_tn_is_false = OP_Pred_False(predicate1_info->in_op, idx);
    if (!(TN_Is_Constant(new_predicate_tn) ||
	  EBO_tn_available (bb, new_predicate_tninfo))) return FALSE;
  } else {
    new_predicate_tn = True_TN;
    new_predicate_tninfo = NULL;
    new_predicate_tn_is_false = FALSE;
  }
#else
  EBO_TN_INFO *new_predicate_tninfo = new_predicate_opinfo->actual_opnd[OP_PREDICATE_OPND];
  TN *new_predicate_tn = OP_opnd(predicate1_info->in_op,OP_PREDICATE_OPND);

  if (!(TN_Is_Constant(new_predicate_tn) ||
        EBO_tn_available (bb, new_predicate_tninfo))) return FALSE;
#endif

  if (OP_store(op)) {
   /* For stores, we will replace both stores with a new one. */
   /* Note that the location of the store will be moved!      */
    if (OP_bb(op) != OP_bb(pred_op)) return FALSE;
    if (opinfo->op_must_not_be_removed) return FALSE;

    val_idx = TOP_Find_Operand_Use(OP_code(op),OU_storeval);
    EBO_TN_INFO *value_tninfo = opinfo->actual_opnd[val_idx];
   /* We will need to copy the value that is to be stored. */

   /* First, be sure that it will be available this point. */
    if ((value_tninfo == NULL) ||
        !EBO_tn_available (bb, value_tninfo)) return FALSE;
   /* Second, be sure that the target of the copy will be a valid result. */
    if (TN_is_const_reg(OP_opnd(op, val_idx))) return FALSE;
   /* Third, be sure that any results have not been used prior to the current OP. */
    if (OP_results(pred_op) != 0) {
      for (i = 0; i < OP_results(pred_op); i++) {
        EBO_TN_INFO *result_tninfo = opinfo->actual_rslt[i];
        if ((result_tninfo == NULL) ||
            (result_tninfo->reference_count != 0)) return FALSE;
      }
    }
   /* Fourth, the index that we need to use must also be generated
      with a predicate that dominates the new store. */
    INT base_idx = TOP_Find_Operand_Use(OP_code(op),OU_base);
    EBO_TN_INFO *input_info = opinfo->actual_opnd[base_idx];
    if ((input_info == NULL) ||
#ifdef TARG_ST
	// [SC] If both stores use exactly the same index, then since they are
	// complements under the new predicate, then the index must be
	// defined when the new predicate is true.
        (input_info != opnd_tninfo[base_idx] && input_info->in_op == NULL) ||
#else
        (input_info->in_op == NULL) ||
#endif
        (input_info->local_tn == NULL) ||
        (input_info->predicate_tninfo != new_predicate_tninfo) ||
        OP_Defs_TN(pred_op, input_info->local_tn)) {
     /* The inputs to the hoisted expression must use the same predicate
        as the new (or modified) expression will. */
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"predicates not hoisted because address not hoisted\n");
      }
      return FALSE;
    }
  } else if (OP_effectively_copy(op)) {
#if 0
    INT val_indx = EBO_Copy_Operand(op);
#else
    INT val_indx = OP_Copy_Operand(op);
#endif
    if ((val_indx <= 0) ||
        !TNs_Are_Equivalent(OP_opnd(op,val_indx), OP_opnd(pred_op,val_indx))) {
     /* The operands are different. We don't handle this and it may not be worthwhile. */
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"copy operands are different\n");
      }
      return FALSE;
    }
  } else {
   /* For non-stores, we will use the results of the predecessor OP. */
    for (i = 0; i < OP_results(pred_op); i++) {
      if (opinfo->actual_rslt[i] == NULL) return FALSE;
      if (!EBO_tn_available (bb, opinfo->actual_rslt[i])) {
        if (EBO_Trace_Data_Flow) {
          fprintf(TFile,"previous result is not available\n");
        }
        return FALSE;
      }
    }
   /* If we are to leave the first OP in place but hoist it's predicate,
      the inputs to the OP must not be conditionally generated. */
#ifdef TARG_ST
    for (i = 0; i < OP_opnds(pred_op); i++) {
      if (i == OP_find_opnd_use(pred_op,OU_predicate)) continue;
#else
    for (i = OP_find_opnd_use(pred_op,OU_predicate)+1; i < OP_opnds(pred_op); i++) {
#endif
      EBO_TN_INFO *input_info = opinfo->actual_opnd[i];
      if ((input_info == NULL) ||
          (input_info->in_op == NULL) ||
          (input_info->local_tn == NULL) ||
          (input_info->predicate_tninfo != new_predicate_tninfo) ||
          OP_Defs_TN(pred_op, input_info->local_tn)) {
       /* The inputs to the hoisted expression must use the same predicate
          as the new (or modified) expression will. */
        if (EBO_Trace_Data_Flow) {
          fprintf(TFile,"predicates not hoisted because inputs not hoisted\n");
        }
        return FALSE;
      }
    }
  }

  OPS ops = OPS_EMPTY;
  if (OP_store(op)) {
   /* Create copies of the different values that are to be stored. */
    TN *value_tn = OP_opnd(op, val_idx);
#ifdef TARG_STxP70
    if (!Opnds_Are_Equivalent(op, pred_op, val_idx, val_idx)) {
#else
    if (!TNs_Are_Equivalent(OP_opnd(op, val_idx),OP_opnd(pred_op, val_idx))) {
#endif
     /* Need to merge the two values that are to be stored. */

     /* Note: it is probably not worthwhile making this transformation because
        it will only increase the tree height of the expressions. */
      if (has_assigned_reg(value_tn) || EBO_in_loop) {
        if (EBO_Trace_Data_Flow) {
          fprintf(TFile,"predicates not hoisted because inputs can not be merged\n");
        }
        return FALSE;
      }

      OPS ops1 = OPS_EMPTY;
      value_tn = Dup_TN(value_tn);

#ifdef TARG_ST
      if (PROC_is_select() && new_predicate_tn == True_TN) {
	// [SC] Use select where possible to get better code.
	EBO_Exp_SELECT (value_tn, predicate1_tn,
			OP_opnd(pred_op, val_idx),
			OP_opnd(op, val_idx),
			&ops1);
	OP_srcpos(OPS_last(&ops1)) = OP_srcpos(pred_op);
	if (EBO_in_loop) EBO_Set_OP_omega (OPS_last(&ops1), predicate1_info, opinfo->actual_opnd[val_idx]);
	OPS_Append_Ops( &ops, &ops1);
      } else {

      EBO_Exp_COPY (predicate1_tn,
                    OP_PredOnFalse(pred_op),
		    OP_cond_def(pred_op), 
                    value_tn,
                    OP_opnd(pred_op, val_idx),
                    &ops1);
#else
      EBO_Exp_COPY (predicate1_tn,
                    value_tn,
                    OP_opnd(pred_op, val_idx),
                    &ops1);
#endif
      OP_srcpos(OPS_last(&ops1)) = OP_srcpos(pred_op);
      //TDR - Fix for bug #50656: Set the property OP_ALWAYS_UNC_DEF to the first definition
      Set_OP_cond_def_kind(OPS_last(&ops1), OP_ALWAYS_UNC_DEF);
      if (EBO_in_loop) EBO_Set_OP_omega (OPS_last(&ops1), predicate1_info, opinfo->actual_opnd[val_idx]);
      OPS_Append_Ops( &ops, &ops1);

      OPS_Init(&ops1);
#ifdef TARG_ST
      EBO_Exp_COPY (predicate2_tn,
                    OP_PredOnFalse(op),
		    OP_cond_def(op),
                    value_tn,
                    OP_opnd(op, val_idx),
                    &ops1);
#else
      EBO_Exp_COPY (predicate2_tn,
                    value_tn,
                    OP_opnd(op, val_idx),
                    &ops1);
#endif
      OP_srcpos(OPS_last(&ops1)) = OP_srcpos(op);

      if (EBO_in_loop) EBO_Set_OP_omega (OPS_last(&ops1), predicate2_info, opnd_tninfo[val_idx]);
      OPS_Append_Ops( &ops, &ops1);
#ifdef TARG_ST
      }
#endif
    }

   /* Create and insert a new store OP. */
#ifdef TARG_ST
    OP *new_op = CGTARG_Dup_OP_Predicate (op, new_predicate_tn);
    val_idx = TOP_Find_Operand_Use(OP_code(new_op),OU_storeval);

    if (OP_has_predicate (new_op)) {
      INT idx = OP_find_opnd_use(new_op, OU_predicate);
      Set_OP_Pred_True (new_op, idx);
    }
#else    
    OP *new_op = Dup_OP(op);
    Set_OP_opnd (new_op, OP_find_opnd_use(new_op,OU_predicate), new_predicate_tn);
#endif
    Set_OP_opnd (new_op, val_idx, value_tn);
    if (OP_memory(op)) {
      Copy_WN_For_Memory_OP (new_op, op);
    }
    OP_srcpos(new_op) = OP_srcpos(op);
    if (EBO_in_loop) {
      EBO_Copy_OP_omega( new_op, op);
      EBO_Set_Predicate_omega ( new_op, new_predicate_tninfo);
    }
    OPS_Append_Op( &ops, new_op);

    if (OP_results(op) != 0) {
      OPS ops1 = OPS_EMPTY;
      for (i = 0; i < OP_results(pred_op); i++) {
#ifdef TARG_ST
        EBO_Exp_COPY (predicate1_tn,
                      OP_PredOnFalse(pred_op),
		      OP_cond_def(pred_op),
                      OP_result(pred_op, i),
                      OP_result(op, i),
                    &ops1);
#else
        EBO_Exp_COPY (predicate1_tn,
                      OP_result(pred_op, i),
                      OP_result(op, i),
                      &ops1);
#endif
        OP_srcpos(OPS_last(&ops1)) = OP_srcpos(pred_op);
        if (EBO_in_loop) EBO_Set_OP_omega (OPS_last(&ops1), predicate1_info, NULL);
      }
      OPS_Append_Ops( &ops, &ops1);
    }

   /* Remove the first OP. */
    remove_op (opinfo);
    OP_Change_To_Noop(opinfo->in_op);
    opinfo->in_op = NULL;
    opinfo->in_bb = NULL;
  } else {
   /* Replace the predicate of the previous op. */
#ifdef TARG_ST
    INT pred_opnd_idx = OP_find_opnd_use(pred_op,OU_predicate);
    OP *new_pred_op = CGTARG_Dup_OP_Predicate (pred_op, new_predicate_tn);
    if (OP_has_predicate (new_pred_op)) {
      if (new_predicate_tn_is_false) {
	Set_OP_Pred_False(new_pred_op, pred_opnd_idx);
      } else {
	Set_OP_Pred_True(new_pred_op, pred_opnd_idx);
      }
    }
    Copy_WN_For_Memory_OP (new_pred_op, pred_op);
    BB_Insert_Op_After (bb, pred_op, new_pred_op);
    BB_Remove_Op (bb, pred_op);
    opinfo->in_op = new_pred_op;
    if (OP_opnds (new_pred_op) < OP_opnds (pred_op)) {
      // The predicate has been removed, so fix up opinfo.
      FmtAssert (OP_opnds (new_pred_op) == (OP_opnds (pred_op) - 1),
		 ("Unsupported predicate removal"));
      for (INT opndnum = pred_opnd_idx; opndnum < OP_opnds (pred_op) - 1;
	   opndnum++) {
	FmtAssert (OP_opnd (new_pred_op, opndnum)
		   == OP_opnd (pred_op, opndnum + 1),
		   ("Unsupported predicate removal"));
	opinfo->actual_opnd[opndnum] = opinfo->actual_opnd[opndnum + 1];
      }
      opinfo->actual_opnd[OP_opnds (pred_op) - 1] = NULL;
    }
#else
    Set_OP_opnd (pred_op, OP_find_opnd_use(pred_op,OU_predicate), new_predicate_tn);
    opinfo->actual_opnd[OP_PREDICATE_OPND] = new_predicate_tninfo;
#endif
    for (i = 0; i < OP_results(pred_op); i++) {
      if (opinfo->actual_rslt[i] != NULL) {
        if (opinfo->actual_rslt[i]->predicate_tninfo != NULL) {
          dec_ref_count(opinfo->actual_rslt[i]->predicate_tninfo);
        }
        opinfo->actual_rslt[i]->predicate_tninfo = new_predicate_tninfo;
        if (new_predicate_tninfo != NULL) {
          inc_ref_count(new_predicate_tninfo);
        }
      }
    }

    if (OP_effectively_copy(op)) {
     /* Leave the second OP the way it was. */
if (EBO_Trace_Optimization) fprintf(TFile,"hoist predicate for complementary copy\n");
      return FALSE;
    }

   /* Create copies to the different outputs. */
    OPS ops1 = OPS_EMPTY;
    for (i = 0; i < OP_results(op); i++) {
      if (!TNs_Are_Equivalent(OP_result(op, i),OP_result(pred_op, i))) {
#ifdef TARG_ST
        EBO_Exp_COPY (predicate2_tn,
                      OP_PredOnFalse(op),
		      OP_cond_def(op),
                      OP_result(op, i),
                      OP_result(pred_op, i),
                    &ops1);
#else
        EBO_Exp_COPY (predicate2_tn,
                      OP_result(op, i),
                      OP_result(pred_op, i),
                      &ops1);
#endif
        OP_srcpos(OPS_last(&ops1)) = OP_srcpos(op);
        if (EBO_in_loop) EBO_Set_OP_omega (OPS_last(&ops1), predicate2_info, NULL);
      }
    }
    OPS_Append_Ops( &ops, &ops1);
  }

  BB_Insert_Ops(bb, op, &ops, FALSE);
if (EBO_Trace_Optimization) fprintf(TFile,"hoist predicate for complementary operations\n");
#ifdef TARG_ST
 if (EBO_Trace_Optimization) {
   fprintf (TFile, "Replaced by:\n");
   Print_OPS_No_SrcLines(&ops);
 }
#endif
  return TRUE;
}

/* =====================================================================
 *   EBO_delete_reload_across_dependency
 *
 *   For a given load or store and one it matches, attempt to replace 
 *   one of them.
 *   Return TRUE if this op is no longer needed.
 *
 *   On a predicated architecture, the transformation looks like
 *     1: pA? X = load @A   or  1: store @A, X
 *     2: pB? store @B = Z
 *     3: pC? Y = load @A
 *
 *   if pA DOM pC and pB DOM pC, it can be transformed into:
 *     1: pA? X = load @A   or  1: store @A, X
 *     2: pB? store @B = Z
 *     3: pC? Y = (A == B ? Z: X)
 *
 * =====================================================================
 */
BOOL
EBO_delete_reload_across_dependency (
  OP *op,
  EBO_TN_INFO **opnd_tninfo,
  EBO_OP_INFO *opinfo,
  EBO_OP_INFO *intervening_opinfo
)
{
  BB *bb = OP_bb(op);
  OP *pred_op = opinfo->in_op;
  OP *intervening_op = intervening_opinfo->in_op;
  TOP pred_opcode = OP_code(pred_op);
  TOP intervening_opcode = OP_code(intervening_op);

  INT size_pred;
  INT size_succ;
  INT size_intervening;

  TN *pred_result;
  TN *intervening_result;

  /* 
   * We can't assign registers, so don't optimize if it's 
   * already been done. 
   */
  if (EBO_in_loop) return FALSE;
  if (EBO_in_peep) return FALSE;

  if (EBO_Trace_Execution) {
    fprintf(TFile,"%sEnter delete_reload_across_dependency.\n",EBO_trace_pfx);
    Print_OP_No_SrcLine(pred_op);
    Print_OP_No_SrcLine(intervening_op);
    Print_OP_No_SrcLine(op);
  }

 /* Be sure we have a "Store/Load .. Store .. Load" pattern. */
  if ((pred_op == NULL) ||
      (intervening_op == NULL) ||
      !OP_load(op) ||
      !(OP_load(pred_op) || OP_store(pred_op)) ||
      !OP_store(intervening_op)) return FALSE;

  pred_op = opinfo->in_op;
  intervening_op = intervening_opinfo->in_op;

  if (OP_prefetch(op) ||
      OP_prefetch(pred_op) ||
      OP_prefetch(intervening_op)) return FALSE;

  if (OP_unalign_mem(op) ||
      OP_unalign_mem(pred_op) ||
      OP_unalign_mem(intervening_op)) return FALSE;

#ifdef TARG_ST
  // FdF 20060517: No support for preinc addressing mode.
  if ((OP_automod(op) && OP_find_opnd_use(op, OU_preincr)) ||
      (OP_automod(pred_op) && OP_find_opnd_use(pred_op, OU_preincr)))
    return FALSE;
#endif


  INT pred_base_idx = OP_find_opnd_use(pred_op, OU_base);
  INT intervening_base_idx =  OP_find_opnd_use(intervening_op, OU_base);
  INT pred_offset_idx = OP_find_opnd_use(pred_op, OU_offset);
  INT intervening_offset_idx = OP_find_opnd_use(intervening_op, OU_offset);
  INT pred_inc_idx = OP_find_opnd_use(pred_op, OU_postincr);
  INT intervening_inc_idx = OP_find_opnd_use(intervening_op, OU_postincr);

  TN *pred_base = OP_opnd(pred_op, pred_base_idx);
  TN *intervening_base = OP_opnd(intervening_op, intervening_base_idx);
  TN *pred_offset = OP_opnd(pred_op, pred_offset_idx);
  TN *intervening_offset = OP_opnd(intervening_op, intervening_offset_idx);

  size_succ = OP_Mem_Ref_Bytes(op);
  size_pred = OP_Mem_Ref_Bytes(pred_op);
  size_intervening = OP_Mem_Ref_Bytes(intervening_op);

#ifdef TARG_ST
  // Here we test the condition for the transformatiuon done in
  // EBO_select_value().
  if (!(EBO_OP_predicate_dominates(pred_op, opinfo->optimal_opnd,
				   op, opnd_tninfo) &&
	EBO_OP_predicate_dominates(intervening_op, intervening_opinfo->optimal_opnd,
				   op, opnd_tninfo))) {
    if (EBO_Trace_Hash_Search) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile,"%sRe-load intervening op predicates do not match\n\t", EBO_trace_pfx);
      Print_OP_No_SrcLine(pred_op);
    }
    return FALSE;
  }
#endif

  if ((size_succ != size_pred) ||
      (size_succ != size_intervening)) return FALSE;

  /* [CG] if size of the reloaded TN is greater. We return. We may generate
     a select with extract however. */
  if (TN_size(OP_result(op, 0)) != size_succ) {
    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"%sSize mismatch for Store - Store - Load combination: load size %d for tn size %d\n",
	      EBO_trace_pfx,size_succ,TN_size(OP_result(op, 0)));
    }
    return FALSE;
  }

  if (TOP_Find_Operand_Use(OP_code(op), OU_postincr) >= 0) {
   /* The increment must be preserved. */
    if (EBO_Trace_Data_Flow) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile,"%sIncremented Load for intervening store combination\n",
              EBO_trace_pfx);
    }
    return FALSE;
  }

  /* Paired loads are not supported. */
  if (OP_results(op) == 2) return FALSE;

  if ((OP_store(pred_op) &&
      ((OP_results(pred_op) > 1) ||
       ((OP_results(pred_op) > 0) &&
        (pred_inc_idx < 0)))) ||
      (OP_store(intervening_op) &&
      ((OP_results(intervening_op) > 1) ||
       ((OP_results(intervening_op) > 0) &&
        (intervening_inc_idx < 0))))) return FALSE;
  if ((OP_load(pred_op) &&
      ((OP_results(pred_op) > 2) ||
       ((OP_results(pred_op) > 1) &&
        (pred_inc_idx < 0)))) ||
      (OP_load(intervening_op) &&
      ((OP_results(intervening_op) > 2) ||
       ((OP_results(intervening_op) > 1) &&
        (intervening_inc_idx < 0))))) return FALSE;

  /* Capture the values in the preceeding memory OPs. */
  pred_result = OP_store(pred_op) ? 
    OP_opnd(pred_op, TOP_Find_Operand_Use(pred_opcode, OU_storeval)) :
    OP_result(pred_op,0);
  intervening_result = OP_opnd(intervening_op,
    TOP_Find_Operand_Use(intervening_opcode, OU_storeval));

  if ((TN_register_class(intervening_result) != TN_register_class(pred_result)) ||
      (TN_register_class(intervening_result) != TN_register_class(OP_result(op,0)))) {
    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"%sInter-register copies are not supported\n",
              EBO_trace_pfx);
    }
    return FALSE;
  }

  if (TNs_Are_Equivalent(pred_result, intervening_result)) {
    /* 
     * It doesn't matter if the addresses are the same or different
     * because the value will always be the same! Just Copy the value. 
     */
    if (EBO_copy_value(op, pred_result, intervening_result, size_succ)) {

      if (EBO_Trace_Optimization) {
	fprintf(TFile,"%sReload across intervening store is not needed.\n",EBO_trace_pfx);
	Print_OP_No_SrcLine(pred_op);
	Print_OP_No_SrcLine(intervening_op);
	Print_OP_No_SrcLine(op);
      }

      return TRUE;
    }
  }

  /* 
   * We need to compare the addresses BEFORE they were incremented.
   * If both OPs are incremented by the same value, we can compare
   * the addresses AFTER the increment.
   */
  if ((pred_inc_idx >= 0) || (intervening_inc_idx >= 0)) {
    if ((pred_inc_idx < 0) || (intervening_inc_idx < 0)) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sBase address not available for compare because of increment\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }

    TN *pred_inc_tn = OP_opnd(pred_op,pred_inc_idx);
    TN *intervening_inc_tn = OP_opnd(intervening_op,intervening_inc_idx);

    if ((TN_Is_Constant(pred_inc_tn) ||
         TN_Is_Constant(intervening_inc_tn)) &&
        (pred_inc_tn != intervening_inc_tn)) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sBase address not available for compare because of different constant increment\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }

    if (opinfo->actual_opnd[pred_inc_idx] != intervening_opinfo->actual_opnd[intervening_inc_idx]) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sBase address not available for compare because of different increment\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }
  }

  /* Are the bases available from each store? */
  if (!EBO_tn_available (bb, opinfo->actual_opnd[pred_base_idx]) ||
      !EBO_tn_available (bb, intervening_opinfo->actual_opnd[intervening_base_idx])) {
    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"%sBase address not available for compare\n",
              EBO_trace_pfx);
    }
    return FALSE;
  }

  return EBO_select_value(op, 
			  pred_result, 
			  pred_base,
			  pred_offset,
			  intervening_result, 
			  intervening_base,
			  intervening_offset,
			  size_succ);

}

/* =====================================================================
 *   delete_memory_op
 *
 *   For a given load or store and one it matches, attempt to replace 
 *   one of them. Return TRUE if this op is no longer needed.
 * =====================================================================
 */
static BOOL
delete_memory_op (
  OP *op,
  EBO_TN_INFO **opnd_tninfo,
  EBO_OP_INFO *opinfo
)
{
  INT i;
  OPS ops = OPS_EMPTY;
  TOP opcode = OP_code(op);
  OP *pred_op = opinfo->in_op;

  INT size_pred;
  INT size_succ;
#ifdef TARG_ST
  /* (cbr) predicate operand # is not necessary constant */
  TN *predicate_tn = OP_Predicate(op);
#else
  TN *predicate_tn = OP_has_predicate(op)?OP_opnd(op,OP_PREDICATE_OPND):NULL;
#endif

  if (OP_prefetch(op)) {
    /* 
     * Remove the second OP for:
     *    Prefetch - Prefetch,
     *    Load - Prefetch,
     *    Store - Prefetch
     */
    if (EBO_Trace_Optimization) {
      fprintf(TFile,"%sRemove following Prefetch combination\n",EBO_trace_pfx);
    }
    return TRUE;
  }

  if (OP_prefetch(opinfo->in_op)) {
   /* 
    * Don't optimize:
    *     Prefetch - Load,
    *     Prefetch - Store,
    */
    return FALSE;
  }

  if (OP_unalign_mem(op) || OP_unalign_mem(opinfo->in_op)) return FALSE;


  size_pred = OP_Mem_Ref_Bytes(opinfo->in_op);
  size_succ = OP_Mem_Ref_Bytes(op);

#ifdef TARG_ST
  // Handle only the case where both are non-multi ops
  // or both are multi ops.
  if (OP_multi(op) != OP_multi(opinfo->in_op)) return FALSE;

  // For multi ops handle only the case of same size ops
  if (OP_multi(op) && size_pred != size_succ) return FALSE;
#endif

  if (OP_load(op) && OP_load(opinfo->in_op)) {
   /* 
    * Replace the result tn of the second OP for:
    *     Load - Load,
    */
    if (TOP_Find_Operand_Use(opcode, OU_postincr) >= 0) {
      /* The increment must be preserved. */
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sIncremented Load for Load - Load combination\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }

#ifdef TARG_ST
    // Predicate preconditions. The last op must be redundant.
    if (!EBO_OP_predicate_dominates(pred_op, opinfo->optimal_opnd,
				    op, opnd_tninfo)) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sPredicate mismatch for  Load for Load combination\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }
#endif
    
    if ((TN_register_class(OP_result(op,0)) !=
                 TN_register_class(OP_result(opinfo->in_op, 0))) ||
        (TN_is_fpu_int(OP_result(op,0)) !=
                 TN_is_fpu_int(OP_result(opinfo->in_op, 0)))) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sType mismatch for Load - Load combination\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }

    if ((size_pred != size_succ) ||
        (OP_results(op) != OP_results(opinfo->in_op)) ||
        (TN_size(OP_result(opinfo->in_op, 0)) != TN_size(OP_result(op, 0)))) {
      /* Size of the data item loaded by the two loads is different,
	 but the starting memory address is the same.  There is a chance
	 that the predecessor load is a load-pair and that the new load
	 matches one of the words that is loaded. */

      if (EBO_Trace_Data_Flow) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sSize mismatch for Load - Load combination: %d:%d %d:%d \n",
                EBO_trace_pfx,size_pred,size_succ,
                TN_size(OP_result(opinfo->in_op, 0)),TN_size(OP_result(op, 0)));
      }

      return EBO_delete_subset_mem_op (op, opnd_tninfo, opinfo, 0, 0);
    }

    if (!EBO_in_peep &&
        (OP_bb(op) != OP_bb(opinfo->in_op)) &&
        !TN_Is_Constant(OP_result(opinfo->in_op, 0)) &&
        has_assigned_reg(OP_result(opinfo->in_op, 0))) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sShouldn't move dedicated register references across blocks.\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }

    if (EBO_Trace_Optimization) {
      fprintf(TFile,"%sRemove Load - Load combination\n",EBO_trace_pfx);
    }

    for (i = 0; i < OP_results(op); i++) {
#ifdef TARG_ST
      EBO_Exp_COPY(predicate_tn, OP_PredOnFalse(op), OP_cond_def(op), OP_result(op, i), OP_result(opinfo->in_op, i), &ops);
#else
      EBO_Exp_COPY(predicate_tn, OP_result(op, i), OP_result(opinfo->in_op, i), &ops);
#endif
    }

    if (EBO_in_loop) 
      EBO_OPS_omega (&ops, 
#ifdef TARG_ST
      /* (cbr) predicate operand # is not necessary constant */
		     OP_Predicate(op),
		     (OP_has_predicate(op)? opnd_tninfo[OP_find_opnd_use(op, OU_predicate)]:NULL));
#else
		     (OP_has_predicate(op)? OP_opnd(op,OP_PREDICATE_OPND):NULL),
		     (OP_has_predicate(op)? opnd_tninfo[OP_PREDICATE_OPND]:NULL));
#endif
    OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);
    BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
    return TRUE;
  } 

  else if (OP_load(op) && OP_store(opinfo->in_op)) {
   /* 
    * Replace the result tn of the second OP for:
    *     Store - Load
    */
    INT storeval_idx = 
           TOP_Find_Operand_Use(OP_code(opinfo->in_op),OU_storeval);
    TN *storeval_tn = OP_opnd(opinfo->in_op, storeval_idx);

    if (TOP_Find_Operand_Use(OP_code(op), OU_postincr) >= 0) {
      /* The increment must be preserved. */
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sIncremented Load for Store - load combination\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }

#ifdef TARG_ST
    // Predicate preconditions. The last op must be redundant.
    if (!EBO_OP_predicate_dominates(pred_op, opinfo->optimal_opnd,
				    op, opnd_tninfo)) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sPredicate mismatch for  Store - Load combination\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }
#endif
    
    if ((TN_register_class(OP_result(op,0)) !=
                 TN_register_class(storeval_tn)) ||
        (TN_is_fpu_int(OP_result(op,0)) !=
                 TN_is_fpu_int(storeval_tn))) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sType mismatch for Store - Load combination\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }

    if (!EBO_in_peep &&
        (OP_bb(op) != OP_bb(opinfo->in_op)) &&
        !TN_Is_Constant(storeval_tn) &&
        has_assigned_reg(storeval_tn)) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sShouldn't move dedicated register references across blocks.\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }

    if (size_pred == size_succ) {
      /* Size of source and target are the same. */
      if (TN_size(storeval_tn) > size_succ) {
       /* 
	* Although the size of the data moved to and from memory is
	* the same, the size of the generated value is larger than
	* the size of the value we want to load.  Call another
	* routine to mask off the upper portion of the stored value. 
	*/

        if (EBO_Trace_Data_Flow) {
          fprintf(TFile,"%sSize mismatch for Store - Load combination: %d %d %d\n",
                  EBO_trace_pfx,size_pred,TN_size(storeval_tn),size_succ);
        }
        return EBO_delete_subset_mem_op (op, opnd_tninfo, opinfo, 0, 0);
      }

      if (EBO_Trace_Optimization) {
        fprintf(TFile,"%sRemove Store - Load combination\n",EBO_trace_pfx);
      }

#ifdef TARG_ST
    // Handle multi ops.
    for (i = 0; i < OP_results(op); i++) {
      EBO_Exp_COPY(predicate_tn, OP_PredOnFalse(op), OP_cond_def(op),
		   OP_result(op, i), OP_opnd(opinfo->in_op, storeval_idx+i), &ops);
    }
#else
      EBO_Exp_COPY(predicate_tn, OP_result(op, 0), storeval_tn, &ops);
#endif

      if (EBO_in_loop) 
	EBO_Set_OP_omega (OPS_first(&ops),
#ifdef TARG_ST
      /* (cbr) predicate operand # is not necessary constant */
			  (OP_has_predicate(op)?opnd_tninfo[OP_find_opnd_use(op, OU_predicate)]:NULL),
#else
			  (OP_has_predicate(op)?opnd_tninfo[OP_PREDICATE_OPND]:NULL),
#endif
			  opinfo->actual_opnd[storeval_idx]);

      OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);
      BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      return TRUE;
    } 
    else {
     /* 
      * Size of source and target are different, but the starting 
      * memory address is the same. Call another routine to do the 
      * work. 
      */
      return EBO_delete_subset_mem_op (op, opnd_tninfo, opinfo, 0, 0);
    }
  } 

  else if (OP_store(op) && 
	   OP_store(opinfo->in_op) &&
	   (OP_bb(op) == OP_bb(opinfo->in_op))) {
   /* 
    * Remove the first OP for:
    *     Store - Store
    */
#ifdef TARG_ST
    if (size_pred > size_succ) return FALSE;
#else
    if (size_pred != size_succ) return FALSE;
#endif
    if (opinfo->op_must_not_be_removed) return FALSE;
    if (TOP_Find_Operand_Use(OP_code(opinfo->in_op), OU_postincr) >= 0) {
      /* The increment must be preserved. */
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sIncremented Store for Store - Store combination\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }

#ifdef TARG_ST
    // Predicate preconditions. The first store must be redundant.
    if (!EBO_OP_predicate_dominates(op, opnd_tninfo,
				    pred_op, opinfo->optimal_opnd)) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sPredicate mismatch for  Store - Store combination\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }
#endif

    if (EBO_Trace_Optimization) {
      fprintf(TFile,"%sRemove Store - Store combination\n",EBO_trace_pfx);
    }

    remove_op (opinfo);
    OP_Change_To_Noop(opinfo->in_op);
    opinfo->in_op = NULL;
    opinfo->in_bb = NULL;
    return FALSE;
  } 

  else if (OP_load(opinfo->in_op) && OP_store(op)) {
   /* 
    * The store may not be needed if the same value is put back for:
    *     Load - Store
    */
    INT storeval_idx = TOP_Find_Operand_Use(OP_code(op),OU_storeval);
    TN *storeval_tn = OP_opnd(op, storeval_idx);

    if (TOP_Find_Operand_Use(OP_code(op), OU_postincr) >= 0) {
      /* The increment must be preserved. */
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sIncremented Store for Load - Store combination\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }

    if ((TN_register_class(OP_result(opinfo->in_op,0)) !=
                 TN_register_class(storeval_tn)) ||
        (TN_is_fpu_int(OP_result(opinfo->in_op,0)) !=
                 TN_is_fpu_int(storeval_tn))) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sType mismatch for Load - Store combination\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }

#ifdef TARG_ST
    // Predicate preconditions. The last op must be redundant.
    if (!EBO_OP_predicate_dominates(pred_op, opinfo->optimal_opnd,
				    op, opnd_tninfo)) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sPredicate mismatch for  Store - Store combination\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }
#endif

    if (!EBO_in_peep &&
        (OP_bb(op) != OP_bb(opinfo->in_op)) &&
        !TN_Is_Constant(storeval_tn) &&
        has_assigned_reg(storeval_tn)) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sShouldn't move dedicated register references across blocks.\n",
                EBO_trace_pfx);
      }
      return FALSE;
    }

    if ((size_pred != size_succ) || (TN_size(storeval_tn) != size_succ)) {

      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sSize mismatch for Load - Store combination: %d %d %d\n",
                EBO_trace_pfx,size_pred,TN_size(storeval_tn),size_succ);
      }

      return FALSE;
    }

    if ((opnd_tninfo[storeval_idx] != NULL) &&
        (opnd_tninfo[storeval_idx]->in_op != NULL) &&
        (opinfo->in_op != NULL) &&
        (opnd_tninfo[storeval_idx]->in_op == opinfo->in_op)) {
     /* The Store is not needed! */

      if (EBO_Trace_Optimization) {
        fprintf(TFile,"%sRemove Load - Store combination\n",EBO_trace_pfx);
      }

      return TRUE;
    }
  }

  return FALSE;
}

/* =====================================================================
 *   EBO_delete_duplicate_op
 *
 *   For a given op and one it matches, attempt to replace one of them.
 *   Return TRUE if this op is no longer needed.
 * =====================================================================
 */
BOOL
EBO_delete_duplicate_op (
  OP *op,
  EBO_TN_INFO **opnd_tninfo,
  EBO_OP_INFO *opinfo
)
{
  INT resnum;
  TOP opcode = OP_code(op);
  OPS ops = OPS_EMPTY;
  OP *pred_op = opinfo->in_op;

  if (EBO_Trace_Data_Flow) {
    fprintf(TFile,"%sDuplicate OP in BB:%d    ",EBO_trace_pfx,BB_id(OP_bb(op)));
    Print_OP_No_SrcLine(op);
    fprintf(TFile,"      Matches   OP in BB:%d    ",BB_id(opinfo->in_bb));
    Print_OP_No_SrcLine(opinfo->in_op);
  }

  if ((Opt_Level < 2) && (OP_bb(op) != opinfo->in_bb)) {
    /* Global TN's aren't supported at low levels of optimization. */
    return FALSE;
  }

#ifdef TARG_ST
  // [CG] Predicate preconditions checks.
  // Note that the former code used opinfo->actual_opnd while
  // we now use opinfo->optimal_opnd.

  // Complement case, for stores:
  // pA ? store @P, X
  // pB ? store @P, Y
  // and there exist pC such that pA and pB are complement 
  // under predicate condition pC. Transformed into: 
  // pA ? t = X
  // pB ? t = Y
  // pC ? store @P, t
  // Complement case, otherwise:
  // pA ? X = exp 
  // pB ? Y = exp
  // and there exist pC such that that pA and pB are complement 
  // under predicate condition pC. Transformed into: 
  // pC ? X = exp
  // pB ? Y = X
  if (OP_code(op) == OP_code(pred_op) &&
      EBO_OP_predicate_complements(op, opnd_tninfo,
				   pred_op, opinfo->optimal_opnd)) {
    return hoist_predicate_of_duplicate_complement (op, opnd_tninfo, opinfo);
  } 

  // Store - Store case:
  // pA? store @P, X
  // pB? store @P, Y
  // where pB DOM pA. Transformed into:
  // pB? store @P, Y
  if (OP_store(pred_op) && OP_store(op) &&
      !EBO_OP_predicate_dominates(op, opnd_tninfo,
				  pred_op, opinfo->optimal_opnd)) {
    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"%sStores can not be combined because predicates do not match\n",EBO_trace_pfx);
    }
    return FALSE;
  }

  // Other cases, for instance:
  // pA? X = load @P or store @P, X
  // pB? Y = load @P
  // where pA DOM pB. Transformed into:
  // pA? X = load @P or store @P, X
  // pB? Y = X
  if (!EBO_OP_predicate_dominates(pred_op, opinfo->optimal_opnd,
				  op, opnd_tninfo)) {
    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"%sExpressions can not be combined because predicates do not match\n",EBO_trace_pfx);
    }
    return FALSE;
  }

#else // !TARG_ST
  if (OP_has_predicate(op) &&
      (OP_opnd(op,OP_PREDICATE_OPND) != OP_opnd(opinfo->in_op,OP_PREDICATE_OPND))) {
    OP *pred_op = opinfo->in_op;
    TN *predicate1_tn = OP_opnd(pred_op,OP_PREDICATE_OPND);
    TN *predicate2_tn = OP_opnd(op,OP_PREDICATE_OPND);
    EBO_TN_INFO *predicate1_info = opinfo->actual_opnd[OP_PREDICATE_OPND];
    EBO_TN_INFO *predicate2_info = opnd_tninfo[OP_PREDICATE_OPND];

    if ((OP_code(op) == OP_code(pred_op)) &&
        EBO_predicate_complements(predicate1_tn, predicate1_info,
                                  predicate2_tn, predicate2_info)) {
      return hoist_predicate_of_duplicate_complement (op, opnd_tninfo, opinfo);
    }
    if (OP_store(pred_op) && OP_store(op)) {
      if (!EBO_predicate_dominates(predicate2_tn, predicate2_info,
                                   predicate1_tn, predicate1_info)) {
        if (EBO_Trace_Data_Flow) {
          fprintf(TFile,"%sStores can not be combined because predicates do not match\n",EBO_trace_pfx);
        }
        return FALSE;
      }
    } else {
      if (!EBO_predicate_dominates(predicate1_tn, predicate1_info,
                                   predicate2_tn, predicate2_info)) {
        if (EBO_Trace_Data_Flow) {
          fprintf(TFile,"%sExpressions can not be combined because predicates do not match\n",EBO_trace_pfx);
        }
        return FALSE;
      }
    }
  }
#endif // !TARG_ST

  if (OP_memory(op)) {
   /* 
    * Separate load/store processing, but logically it's just a 
    * special case. 
    */
    return delete_memory_op (op, opnd_tninfo, opinfo);
  } 

#ifdef TARG_ST
  // [CG]: skip special compare processing for now
#else
  else if (OP_icmp(op) || OP_fcmp(op)) {
   /* Integer and float comparison operations. */
    if (EBO_condition_redundant (op, opnd_tninfo, opinfo, &ops)) {
      if (OPS_length(&ops) != 0) {
	if (EBO_in_loop) EBO_OPS_omega (&ops, NULL,NULL);
        OP *next_op = OPS_first(&ops);
        while (next_op != NULL) {
          OP_srcpos(next_op) = OP_srcpos(op);
          next_op = OP_next(next_op);
        }
	BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      }
      return TRUE;
    }
    else {
      if (EBO_Trace_Optimization) {
#pragma mips_frequency_hint NEVER
	fprintf(TFile,"%sCan not remove compare operation.\n",EBO_trace_pfx);
      }
      return FALSE;
    }
  } /* compare OP */
#endif

  else {

#if 0
    // [CG] Commented out.
    if (OP_has_predicate(op) &&
#ifdef TARG_ST
	(OP_Predicate(op) != True_TN) {
#else
        (OP_opnd(op,OP_PREDICATE_OPND) != True_TN)) {
#endif
      /* 
       * Check for required copies of predicates that can not 
       * be supported. 
       */
      for (resnum = 0; resnum < OP_results(op); resnum++) {
        if (TN_register_class(OP_result(op, resnum)) == ISA_REGISTER_CLASS_predicate) {
          if (EBO_Trace_Data_Flow) {
            fprintf(TFile,"%sStores can not be combined because predicate copies can not be predicated\n",
                   EBO_trace_pfx);
          }
          return FALSE;
        }
      }
    }
#endif

    /* Create copies of the result TN's. */
    for (resnum = 0; resnum < OP_results(op); resnum++) {
      TN *rslt = OP_result(op, resnum);
      TN *src1 = OP_result(opinfo->in_op, resnum);
      
#ifdef TARG_ST
      int pred_idx = OP_find_opnd_use(op, OU_predicate);

      if (OP_has_predicate(op) && OP_opnd(op, pred_idx) != True_TN) 
        EBO_Exp_COPY(OP_opnd(op, pred_idx), OP_Pred_False(op, pred_idx), OP_cond_def(op), rslt, src1, &ops);
      else
        EBO_Exp_COPY(NULL, false, false, rslt, src1, &ops);

      /* No need to replace if same op, avoids infinite loops. */
      if (OP_cond_def (op) && OP_code (op) == OP_code (OPS_first (&ops)))
        return FALSE;

#else
      EBO_Exp_COPY(NULL, rslt, src1, &ops);
#endif
#ifdef TARG_ST
      if (EBO_in_peep && OP_next(OPS_first(&ops)) != NULL) {
	/* If the copy needs multiple instructions, we 
	   may have generated non dedicated tns while register allocation is already done. */
	return FALSE;
      }
#endif

      OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
    }

    if (EBO_in_loop) 
      EBO_OPS_omega (&ops, NULL, NULL);

    BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
    return TRUE;
  }

  return FALSE;
}

#ifdef TARG_ST
static
/* =====================================================================
 *    EBO_Constant_Offset_Propagate
 *
 *    Look for constant propagation in offsets
 * =====================================================================
 */
BOOL
EBO_Constant_Offset_Propagate(
    OP *op,
    TN **opnd_tn,
    EBO_TN_INFO **opnd_tninfo
)
{
    INT l0_offset_idx;
    TN *tn_offset;
    BB *bb = OP_bb(op);
    INT opndnum = OP_opnds(op);
    TOP opcode = OP_code(op);

#ifdef TARG_ST200
    if (!OP_address(op)) return FALSE;
#else
    if (!OP_load(op) && !OP_store(op) && !OP_prefetch(op))
        return FALSE;
#endif

    if (EBO_Trace_Execution) {
        INT i;
        fprintf(TFile, "%sin BB:%d Propagate offset constant value OP :- %s ", EBO_trace_pfx, BB_id(bb), TOP_Name(OP_code(op)));
        for (i = 0; i < opndnum; i++) {
            fprintf(TFile, " ");
            Print_TN(opnd_tn[i], FALSE);
        }
        fprintf(TFile, "\n");
    }
    if (OP_automod(op))    return FALSE;

    l0_offset_idx = OP_find_opnd_use(op, OU_offset);
    if (l0_offset_idx < 0) return FALSE; 

    tn_offset = opnd_tn[l0_offset_idx];
    if(!TN_Has_Value(tn_offset)) return FALSE; 
    if(!TN_Is_Constant(tn_offset)) return FALSE;
    if(!TN_is_register(OP_Offset(op))) return FALSE;
    OP *new_op = Dup_OP(op);
    TOP new_opcode = TOP_opnd_immediate_variant(opcode, l0_offset_idx, TN_value(tn_offset));
    if (new_opcode == TOP_UNDEFINED) return FALSE;
    Set_OP_opr(new_op, new_opcode);
    OP_srcpos(new_op) = OP_srcpos(op);
    Set_OP_opnd (new_op, l0_offset_idx, tn_offset);
    if (OP_memory(op)) Copy_WN_For_Memory_OP(new_op, op);
    BB_Insert_Op_After(bb, op, new_op);
    if (EBO_Trace_Optimization) fprintf(TFile, "Propagate offset constant value\n");
    return TRUE;
}
  
/* =====================================================================
 *    EBO_Address_Sequence
 *
 *    Look for sequence such as add/(load|store).
 * =====================================================================
 */
static
BOOL
EBO_Address_Sequence (
  OP *op,
  TN **opnd_tn,
  EBO_TN_INFO **opnd_tninfo
)
{
  BB *bb = OP_bb(op);
  INT opndnum = OP_opnds(op);
  EBO_TN_INFO *tninfo;

  TOP opcode = OP_code(op);
  INT l0_base_idx;
  INT l0_offset_idx;
  TN *tn_base;
  TN *tn_offset;
  INT64 offset_val;
  ST *offset_sym = NULL;
  INT32 offset_relocs;
  

  OP *pred_op;
  TOP pred_opcode;
  EBO_OP_INFO *pred_opinfo;

#ifdef TARG_ST200
  if (!OP_address(op)) return FALSE ;
#else
  if (!OP_load(op) && !OP_store(op) && !OP_prefetch(op)) return FALSE;
#endif


  if (EBO_Trace_Execution) {
    INT i;
    fprintf(TFile, "%sin BB:%d load|store sequence OP :- %s ",
            EBO_trace_pfx, BB_id(bb),TOP_Name(OP_code(op)));
    for (i = 0; i < opndnum; i++) {
      fprintf(TFile," ");
      Print_TN(opnd_tn[i],FALSE);
    }
    fprintf(TFile,"\n");
  }

  // FdF 20060517: No support for automod
  if (OP_automod(op)) return FALSE;

  l0_base_idx = OP_find_opnd_use(op, OU_base);
  l0_offset_idx = OP_find_opnd_use(op, OU_offset);

  if (l0_base_idx < 0 || l0_offset_idx < 0) { return FALSE; }

  tn_base = opnd_tn[l0_base_idx];
  tn_offset = opnd_tn[l0_offset_idx];

  if (TN_size(tn_base) != TN_size(tn_offset)) { return FALSE; }

  if (TN_Has_Value(tn_offset) || TN_is_symbol(tn_offset)) {
    // [HK] TOP_fetch_opnd treats both TN_Has_Value and TN_is_symbol
    // cases, but truncate the value according to the encoding space.
    // Thus, it may produce an incorrect value, in the case the encoding space
    // is too small at this time.
    offset_val = TOP_fetch_opnd(opcode, opnd_tn, l0_offset_idx);
    offset_sym = TN_is_symbol(tn_offset)? TN_var(tn_offset): NULL;
    offset_relocs = TN_is_symbol(tn_offset)? TN_relocs(tn_offset): 0;
  } else return FALSE;
  
  tninfo = opnd_tninfo[l0_base_idx];
  if (tninfo == NULL) return FALSE;
  pred_op = tninfo->in_op;
  if (pred_op == NULL) return FALSE;
  pred_opcode = OP_code(pred_op);
  pred_opinfo = locate_opinfo_entry (tninfo);
  if (pred_opinfo == NULL) return FALSE;

  if (!OP_iadd(pred_op) && !OP_isub(pred_op)) return FALSE;
  if (pred_opcode == TOP_spadjust) return FALSE;

  INT ptn0_idx = TOP_Find_Operand_Use(pred_opcode,OU_opnd1);
  INT ptn1_idx = TOP_Find_Operand_Use(pred_opcode,OU_opnd2);
  FmtAssert(ptn0_idx >= 0 && ptn1_idx >= 0, ("add/sub opcode %shas no operand properties", TOP_Name(pred_opcode)));

  TN *ptn0 = OP_opnd(pred_op, ptn0_idx);
  TN *ptn1 = OP_opnd(pred_op, ptn1_idx);
  EBO_TN_INFO *ptn0_tninfo;
  INT64 pred_val;
  ST *pred_sym = NULL;
  INT32 pred_relocs;

  if (TN_size(ptn1) != TN_size(ptn0)) { return FALSE; }
  if (TN_size(ptn1) != TN_size(tn_offset)) { return FALSE; }
      
  if (TN_Has_Value(ptn1) || TN_is_symbol(ptn1)) {
    pred_val = TOP_fetch_opnd(pred_opcode, OP_opnds_ptr(pred_op), ptn1_idx);
    pred_sym = TN_is_symbol(ptn1)? TN_var(ptn1): NULL;
    pred_relocs = TN_is_symbol(ptn1)? TN_relocs(ptn1): 0;
    ptn0_tninfo = pred_opinfo->actual_opnd[ptn0_idx];
  } else if (OP_iadd(pred_op) && (TN_Has_Value(ptn0) || TN_is_symbol(ptn0))) {
    pred_val = TOP_fetch_opnd(pred_opcode, OP_opnds_ptr(pred_op), ptn0_idx);
    pred_sym = TN_is_symbol(ptn0)? TN_var(ptn0): NULL;
    pred_relocs = TN_is_symbol(ptn0)? TN_relocs(ptn0): 0;
    ptn0 = ptn1;
    ptn0_tninfo = pred_opinfo->actual_opnd[ptn1_idx];
  } else return FALSE;
  
  // [CG]: It seems that we can't modify symbol offsets
  // as Base_Symbol_And_Offset_For_Addressing will change

  // FdF 20050114: symbol offset can be changed if the symbol is on
  // the stack.
  //  if (pred_sym != NULL || offset_sym != NULL) return FALSE;
  if (pred_sym == NULL || !ST_on_stack(pred_sym))
    if (pred_sym != NULL || offset_sym != NULL) return FALSE;

  if (!EBO_tn_available (bb, ptn0_tninfo)) return FALSE;

  TOP new_opcode;
  OP *new_op;
  UINT64 new_offset_val;
  ST *new_sym = NULL;
  INT32 new_relocs;
  TN *new_offset_tn;

  if (pred_sym != NULL && offset_sym != NULL) return FALSE;
  if (offset_sym != NULL) {
    new_sym = offset_sym;
    new_relocs = offset_relocs;
  }
  if (pred_sym != NULL) {
    new_sym = pred_sym;
    new_relocs = pred_relocs;
  }

  // [VL] If the operand use is a OU_negoffset in memory access, then the 
  // offset must be a negative one (case encountered in bug #80774).
  if (OP_find_opnd_use(op, OU_negoffset)>=0) 
    FmtAssert( offset_val<=0, ("Negoffset in memory access with positive offset"));
  
  if (OP_iadd(pred_op)) 
    new_offset_val = offset_val + pred_val;
  else if (OP_isub(pred_op) && pred_sym == NULL)
    new_offset_val = offset_val - pred_val;
  else return FALSE;

  new_offset_val = sext(new_offset_val, TN_size(OP_result(pred_op, 0))*8);
  
  if (new_sym != NULL) {
    new_offset_tn = Gen_Symbol_TN(new_sym, new_offset_val, new_relocs);
    new_opcode = TOP_opnd_immediate_variant(opcode, l0_offset_idx, 0xFFFFFFFF);
  } else {
    new_offset_tn = Gen_Literal_TN(new_offset_val, TN_size(ptn0));
    new_opcode = TOP_opnd_immediate_variant(opcode, l0_offset_idx, new_offset_val);
  }
  if (new_opcode == TOP_UNDEFINED) return FALSE;

  if (ISA_PACK_Inst_Words(new_opcode) > ISA_PACK_Inst_Words(opcode))
    return FALSE;
  
  new_op = Dup_OP(op);
  // [HK] ISO C++ forbids cast to non reference type on lvalues
  //        OP_code(new_op) = new_opcode;
  Set_OP_opr(new_op, new_opcode);
  OP_srcpos(new_op) = OP_srcpos(op);
  Set_OP_opnd (new_op, l0_base_idx, ptn0);
  Set_OP_opnd (new_op, l0_offset_idx, new_offset_tn);
  if (OP_memory(op)) Copy_WN_For_Memory_OP (new_op, op);
  if (EBO_in_loop) {
    EBO_OP_omega(new_op, ptn0, ptn0_tninfo);
  }
  BB_Insert_Op_After(bb, op, new_op);
  if (EBO_Trace_Optimization) fprintf(TFile,"combined add/load|store sequence\n");
  return TRUE;
}
#endif

/* =====================================================================
 *    EBO_Try_To_Simplify_Operand0
 *
 *    Look at an instruction that has a constant first operand and attempt
 *    to simplify the computations.
 * =====================================================================
 */
BOOL
EBO_Try_To_Simplify_Operand0 (OP *op,
			      TN *tnr,
			      TN *tn0,
			      TN *tn1,
			      OPS *ops)
{
  TOP opcode = OP_code(op);
  INT l0_opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
  INT l0_opnd2_idx = OP_find_opnd_use(op, OU_opnd2);

  // Arthur: spadjust shouldn't be subject to this
  if (OP_code(op) == TOP_spadjust) return FALSE;

#if 0
  // Don't optimize predicated operations
  if (OP_cond_def(op)) return FALSE;
#endif

  // Floating -point simplification:
  if (tn0 == FZero_TN) {
    // [HK] this is now activated by the -ffinite-math-only flag
    //    if ((IEEE_Arithmetic >= IEEE_INEXACT) && OP_fmul(op)) {
    if ( Finite_Math && OP_fmul(op)) {

      if (EBO_Trace_Optimization) 
	fprintf(TFile,"replace fmpy of 0.0 with 0.0\n");

 #ifdef TARG_ST
      EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), tnr, FZero_TN, ops);
 #else
      EBO_Exp_COPY(OP_Predicate(op), tnr, FZero_TN, ops);
#endif
      return TRUE;
    }

    if (OP_fadd(op)) {
      if (EBO_Trace_Optimization) 
	fprintf(TFile,"replace fadd of 0.0 with tn1\n");

#ifdef TARG_ST
      EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), tnr, tn1, ops);
#else
      EBO_Exp_COPY(OP_Predicate(op), tnr, tn1, ops);
 #endif

      return TRUE;
    }
  }

  if (tn0 == FOne_TN) {
    if (OP_fmul(op)) {

      if (EBO_Trace_Optimization) 
	fprintf(TFile,"replace fmpy of 1.0 with tn1\n");

#ifdef TARG_ST
      EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), tnr, tn1, ops);
#else
      EBO_Exp_COPY(OP_Predicate(op), tnr, tn1, ops);
#endif
      return TRUE;
    }
  }

  // Integer arithmetic simplification

  if (TN_is_symbol(tn0)) {
   /* Re-located constants not supported. */
    return FALSE;
  }

  INT64 const_val = TN_Value(tn0);

  //
  // First of all, check if there is a target specific simplification
  //
  OP *new_op = EBO_simplify_operand0(op, tnr, const_val, tn1);
  if (new_op != NULL) {
    OPS_Append_Op(ops, new_op);
    return TRUE;
  }

  // shifts
  if (OP_ishl(op) || OP_ishr(op) || OP_ishru(op)) {
    INT op1_bits = TOP_opnd_use_bits(opcode, l0_opnd1_idx);
    BOOL op1_signed = TOP_opnd_use_signed(opcode, l0_opnd1_idx);
    const_val = op1_signed ? ((INT64)const_val << 64-op1_bits) >> 64-op1_bits:
      ((UINT64)const_val << 64-op1_bits) >> 64-op1_bits;
    if (const_val == 0) {
      TN *tnc = Gen_Literal_TN (const_val, TN_size(tnr));
      Expand_Immediate (tnr, tnc, 0, ops);
#ifdef TARG_ST
      EBO_OPS_predicate (OP_Predicate (op), OP_PredOnFalse(op), OP_cond_def(op), ops);
#endif
      return TRUE;
    } else if (const_val == -1 && OP_ishr(op)) {
      TN *tnc = Gen_Literal_TN (const_val, TN_size(tnr));
      Expand_Immediate (tnr, tnc, 0, ops);
#ifdef TARG_ST
      EBO_OPS_predicate (OP_Predicate (op), OP_PredOnFalse(op), OP_cond_def(op), ops);
#endif
      return TRUE;
    }
  }

  // [HK] replace integer mul by 0 with 0
  if (TN_is_zero(tn0) && OP_imul(op)) {
    TN *tnc = Gen_Literal_TN(0, TN_size(tnr));
    Expand_Immediate (tnr, tnc, 0, ops);
#ifdef TARG_ST
    if (OP_has_predicate (op)) {
      int pred_index = OP_find_opnd_use(op, OU_predicate);
      EBO_OPS_predicate (OP_opnd (op, pred_index), OP_Pred_False(op, pred_index), OP_cond_def(op), ops);
    }
#endif
    return TRUE;
  }
  
  // [HK] replace integer mul by 1 with tn1
  if ((const_val == 1) && OP_imul(op)) 
    if (TN_is_register(tn1)) {
#ifdef TARG_ST
      EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), tnr, tn1, ops);
#else
      Exp_COPY(tnr, tn1, ops);
#endif
      return TRUE;
    }

  // [HK] replace integer div of 0 by !0 with 0 
  if (TN_is_zero(tn0) && OP_idiv(op) && !TN_is_zero(tn1)) {
    TN *tnc = Gen_Literal_TN(0, TN_size(tnr));
    Expand_Immediate (tnr, tnc, 0, ops);
#ifdef TARG_ST
    if (OP_has_predicate (op)) {
      int pred_index = OP_find_opnd_use(op, OU_predicate);
      EBO_OPS_predicate (OP_opnd (op, pred_index), OP_Pred_False(op, pred_index), OP_cond_def(op), ops);
    }
#endif
    return TRUE;
  }
  
  return FALSE;
}

/* =====================================================================
 *    EBO_Constant_Operand0
 *
 *    Look at an exression that has a constant first operand and attempt
 *    to simplify the computations.
 * =====================================================================
 */
BOOL
EBO_Constant_Operand0 (
  OP *op,
  TN **opnd_tn,
  EBO_TN_INFO **opnd_tninfo
)
{
  BB *bb = OP_bb(op);
  INT opndnum = OP_opnds(op);
  EBO_TN_INFO *tninfo;

  TOP opcode = OP_code(op);
  TN *tnr = OP_result(op,0);
  INT l0_opnd1_idx;
  INT l0_opnd2_idx;
  TN *tn0;
  TN *tn1;

  OP *pred_op;
  TOP pred_opcode;
  EBO_OP_INFO *pred_opinfo;

  if (EBO_Trace_Execution) {
    INT i;
    fprintf(TFile, "%sin BB:%d special0 OP :- %s",
            EBO_trace_pfx, BB_id(OP_bb(op)),TOP_Name(OP_code(op)));
    for (i = 0; i < opndnum; i++) {
      fprintf(TFile," ");
      Print_TN(opnd_tn[i],FALSE);
    }
    fprintf(TFile,"\n");
  }

#ifdef TARG_ST
  if (OP_has_implicit_interactions(op)) return FALSE;
  if (OP_results(op) > 1) return FALSE;
#endif

  l0_opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
  l0_opnd2_idx = OP_find_opnd_use(op, OU_opnd2);

  // [CG]: return if opnd1 and opnd2 semantic undefined
  if (l0_opnd1_idx < 0 || l0_opnd2_idx < 0) return FALSE;

  tn0 = opnd_tn[l0_opnd1_idx];
  tn1 = opnd_tn[l0_opnd2_idx];

  if (EBO_Trace_Execution) {
    fprintf(TFile, "%s operands OU_opnd1: const: %d, has_val: %d, , val: %"SCNd64"\n",
	    EBO_trace_pfx, TN_Is_Constant(tn0), TN_Has_Value(tn0), TN_Has_Value(tn0) ? TN_Value(tn0): 0);
    if (tn1) {
      fprintf(TFile, "%s operands OU_opnd2: const: %d, has_val: %d, , val: %"SCNd64"\n",
	      EBO_trace_pfx, TN_Is_Constant(tn1), TN_Has_Value(tn1), TN_Has_Value(tn1) ? TN_Value(tn1): 0);
    }
  }

  OPS ops = OPS_EMPTY;
  if (! EBO_Try_To_Simplify_Operand0(op, tnr, tn0, tn1, &ops)) {
    return FALSE;
  }

  FmtAssert(OPS_first(&ops) != NULL, ("ops empty after simplify operand 0"));

  if (OP_next(OPS_first(&ops)) != NULL) {
    /* What's the point in replacing one instruction with several? */
    return FALSE;
  }
  /* No need to replace if same op, avoids infinite loops. */
  if (OPs_Are_Equivalent(op, OPS_first(&ops))) return FALSE;

  OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);
  if (EBO_in_loop) {
    EBO_OPS_omega(&ops, OP_Predicate(op),
		  (OP_has_predicate(op)? opnd_tninfo[OP_find_opnd_use(op, OU_predicate)]:NULL));
    EBO_OPS_omega(&ops, opnd_tn[l0_opnd2_idx], opnd_tninfo[l0_opnd2_idx]);
  }

  BB_Insert_Ops_After(OP_bb(op), op, &ops);
  return TRUE;
}


/* =====================================================================
 *    EBO_Try_To_Simplify_Operand1
 *
 *    Look at an instruction that has a constant first operand and attempt
 *    to simplify the computations.
 * =====================================================================
 */
BOOL
EBO_Try_To_Simplify_Operand1 (OP *op,
			      TN *tnr,
			      TN *tn0,
			      TN *tn1,
			      OPS *ops)
{
  TOP opcode = OP_code(op);
  INT l0_opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
  INT l0_opnd2_idx = OP_find_opnd_use(op, OU_opnd2);


  // Arthur: spadjust shouldn't be subject to this
  if (OP_code(op) == TOP_spadjust) return FALSE;

#if 0
  /* (cbr) enable */
  // Don't optimize predicated operations
  if (OP_cond_def(op)) return FALSE;
#endif

  // Integer arithmetic simplification

  if (TN_is_symbol(tn1)) {
   /* Re-located constants not supported. */
    return FALSE;
  }

  INT64 const_val = TN_Value(tn1);

  //
  // First of all, check if there is a target specific simplification
  //
  OP *new_op = EBO_simplify_operand1 (op, tnr, tn0, const_val);
  if (new_op != NULL) {
    OPS_Append_Op(ops, new_op);
    return TRUE;
  }

  // shifts
  if (OP_ishl(op) || 
      OP_ishr(op) ||
      OP_ishru(op)) {
    INT op1_bits = TOP_opnd_use_bits(opcode, l0_opnd1_idx);
    INT op2_bits = TOP_opnd_use_bits(opcode, l0_opnd2_idx);
    const_val = ((UINT64)const_val << 64-op2_bits) >> 64-op2_bits;
    if (const_val == 0) {
      if (TN_is_register(tn0)) {
#ifdef TARG_ST
        EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), tnr, tn0, ops);
#else
	Exp_COPY(tnr, tn0, ops);
#endif
	return TRUE;
      }
    } else if (const_val >= op1_bits &&
	       (OP_ishl(op) || OP_ishru(op))) {
      TN *tnc = Gen_Literal_TN(0, TN_size(tnr));
      Expand_Immediate (tnr, tnc, 0, ops);
#ifdef TARG_ST
    if (OP_has_predicate (op)) {
      int pred_index = OP_find_opnd_use(op, OU_predicate);
      EBO_OPS_predicate (OP_opnd (op, pred_index), OP_Pred_False(op, pred_index), OP_cond_def(op), ops);
    }
#endif
      return TRUE;
    } else if (TN_has_value(OP_opnd(op, l0_opnd2_idx)) && 
	       TN_value(OP_opnd(op, l0_opnd2_idx)) != const_val) {
      OP *new_op = Dup_OP (op);
      Set_OP_opnd(new_op, l0_opnd2_idx, Gen_Literal_TN(const_val, TN_size(tnr)));
      OPS_Append_Op(ops, new_op);
      return TRUE;
    }
  }
  
  /* Value 0 -> copy. */
  if ((const_val == 0) &&
      (OP_isub(op) || 
       OP_iadd(op) ||
       OP_ior(op) ||
       OP_ixor(op))) {
    if (TN_is_register(tn0)) {
#ifdef TARG_ST
      EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), tnr, tn0, ops);
#else
      Exp_COPY(tnr, tn0, ops);
#endif
      return TRUE;
    }
  }

  /* Value 0 -> 0. */
  if ((const_val == 0) &&
      (OP_iand(op))) {
    TN *tnc = Gen_Literal_TN(0, TN_size(tnr));
    Expand_Immediate (tnr, tnc, 0, ops);
#ifdef TARG_ST
    if (OP_has_predicate (op)) {
      int pred_index = OP_find_opnd_use(op, OU_predicate);
      EBO_OPS_predicate (OP_opnd (op, pred_index), OP_Pred_False(op, pred_index), OP_cond_def(op), ops);
    }
#endif
    return TRUE;
  }

  // [HK] replace integer mul by 0 with 0
  if (TN_is_zero(tn1) && OP_imul(op)) {
      TN *tnc = Gen_Literal_TN(0, TN_size(tnr));
      Expand_Immediate (tnr, tnc, 0, ops);
#ifdef TARG_ST
    if (OP_has_predicate (op)) {
      int pred_index = OP_find_opnd_use(op, OU_predicate);
      EBO_OPS_predicate (OP_opnd (op, pred_index), OP_Pred_False(op, pred_index), OP_cond_def(op), ops);
    }
#endif
      return TRUE;
  }

  // [HK] replace integer mul by 1 with tn0
  if ((const_val == 1) && OP_imul(op)) 
      if (TN_is_register(tn0)) {
#ifdef TARG_ST
	  EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), tnr, tn0, ops);
#else
	  Exp_COPY(tnr, tn0, ops);
#endif
	  return TRUE;
      }
 
  return FALSE;
}

			      
/* =====================================================================
 *    EBO_Constant_Operand1
 *
 *    Look at an exression that has a constant second operand and 
 *    attempt to simplify the computations.
 * =====================================================================
 */
BOOL
EBO_Constant_Operand1 (
  OP *op,
  TN **opnd_tn,
  EBO_TN_INFO **opnd_tninfo
)
{
  BB *bb = OP_bb(op);
  INT opndnum = OP_opnds(op);
  EBO_TN_INFO *tninfo;

  TOP opcode = OP_code(op);
  TN *tnr = OP_result(op,0);
  INT l0_opnd1_idx;
  INT l0_opnd2_idx;
  TN *tn0;
  TN *tn1;

  OP *pred_op;
  TOP pred_opcode;
  EBO_OP_INFO *pred_opinfo;

  if (EBO_Trace_Execution) {
    INT i;
    fprintf(TFile, "%sin BB:%d special1 OP :- %s ",
            EBO_trace_pfx, BB_id(bb),TOP_Name(OP_code(op)));
    for (i = 0; i < opndnum; i++) {
      fprintf(TFile," ");
      Print_TN(opnd_tn[i],FALSE);
    }
    fprintf(TFile,"\n");
  }

#ifdef TARG_ST
  if (OP_has_implicit_interactions(op)) return FALSE;
  if (OP_results(op) > 1) return FALSE;
#endif

  l0_opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
  l0_opnd2_idx = OP_find_opnd_use(op, OU_opnd2);

  // [CG]: return if opnd1 or opnd2 semantic undefined
  if (l0_opnd1_idx < 0 || l0_opnd2_idx < 0) return FALSE;

  tn0 = opnd_tn[l0_opnd1_idx];
  tn1 = opnd_tn[l0_opnd2_idx];

  if (EBO_Trace_Execution) {
    fprintf(TFile, "%s operands OU_opnd1: const: %d, has_val: %d, , val: %"SCNd64"\n",
	    EBO_trace_pfx, TN_Is_Constant(tn0), TN_Has_Value(tn0), TN_Has_Value(tn0) ? TN_Value(tn0): 0);
    if (tn1) {
      fprintf(TFile, "%s operands OU_opnd2: const: %d, has_val: %d, , val: %"SCNd64"\n",
	      EBO_trace_pfx, TN_Is_Constant(tn1), TN_Has_Value(tn1), TN_Has_Value(tn1) ? TN_Value(tn1): 0);
    }
  }

#if 0
  // Floating-point simplification
  // ia64 stuff was here: ref to be/bg/ia64
#endif

  if (TN_is_symbol(tn1)) {
   /* Re-located constants not supported. */
    return FALSE;
  }

  INT64 const_val = TN_Value(tn1);

  OPS ops = OPS_EMPTY;
  if (EBO_Try_To_Simplify_Operand1(op, tnr, tn0, tn1, &ops)) {
    goto OPS_Created;
  }

 /* Sequence optimizations. */

  tninfo = opnd_tninfo[l0_opnd1_idx];
  if (tninfo == NULL) return FALSE;
  pred_op = tninfo->in_op;
  if (pred_op == NULL) return FALSE;
  pred_opcode = OP_code(pred_op);
  pred_opinfo = locate_opinfo_entry (tninfo);
  if (pred_opinfo == NULL) return FALSE;

#if 0
 /* First, look for some special cases involving loads. */
  // Removed from here
  // Ref to be/cg/ia64
#endif

 /* Now, look into optimizations that depend on specific
    values for the actual operands of the predecessor op. */
  if (TOP_Find_Operand_Use(OP_code(pred_op),OU_opnd2) >= 0) {
    TOP pred_opcode = OP_code(pred_op);
    INT ptn0_idx = TOP_Find_Operand_Use(pred_opcode,OU_opnd1);
    INT ptn1_idx = TOP_Find_Operand_Use(pred_opcode,OU_opnd2);
    TN *ptn0 = OP_opnd(pred_op, ptn0_idx);
    TN *ptn1 = OP_opnd(pred_op, ptn1_idx);
    EBO_TN_INFO *ptn0_tninfo;
    EBO_TN_INFO *ptn1_tninfo;
    INT64 pred_val = 0;

    OP *new_op;
    TOP new_opcode;
    UINT64 new_const_val;

    ptn0_tninfo = pred_opinfo->actual_opnd[ptn0_idx];
    ptn1_tninfo = pred_opinfo->actual_opnd[ptn1_idx];

   /* Are the operands available for use? */
    if (!(TN_Is_Constant(ptn0) || EBO_tn_available (bb, ptn0_tninfo)) ||
        !(TN_Is_Constant(ptn1) || EBO_tn_available (bb, ptn1_tninfo))) return FALSE;
    if (pred_opcode == TOP_spadjust) return FALSE;

    if (TN_Is_Constant(ptn1) && !TN_is_symbol(ptn1)) {
    } else if (TN_Is_Constant(ptn0) && !TN_is_symbol(ptn0) &&
               (OP_iadd(pred_op) || OP_ior(pred_op)  || OP_iand(pred_op))) {
      TN *s = ptn0;
      ptn0 = ptn1;
      ptn0_tninfo = ptn1_tninfo;
      ptn1 = s;
    } else return FALSE;

   /* Is the non-constant operand available for use? */
    if (!EBO_tn_available (bb, ptn0_tninfo)) {
      return FALSE;
    }

    pred_val = TN_Value(ptn1);

   /* Look for identical operations where the constants can be combined. */
#ifdef TARG_ST
    // We combine currently identical add/sub only for unrolled ops to avoid enlarging live ranges.
    if ((opcode == pred_opcode) &&
	/*OP_unrolling(op) &&*/
	(OP_iadd(op) || OP_isub(op))) {

      new_const_val = sext(pred_val + const_val,  TN_size(tnr)*8);
      new_opcode = TOP_opnd_immediate_variant(opcode, l0_opnd2_idx, new_const_val);

      if (new_opcode != TOP_UNDEFINED) {
	  if (OP_has_predicate (op)) {
	    new_op = Mk_OP (new_opcode, tnr, OP_opnd(op, 0), ptn0, Gen_Literal_TN(new_const_val, TN_size(tnr)));
            CGTARG_Predicate_OP (NULL, new_op, OP_Predicate (op), OP_PredOnFalse(op));
	    // FdF 20081015
	    if (!OP_cond_def(op))
	      Set_OP_cond_def_kind(new_op, OP_ALWAYS_UNC_DEF);
          }
	  else 
	    new_op = Mk_OP (new_opcode, tnr, ptn0, Gen_Literal_TN(new_const_val, TN_size(tnr)));
          OP_Copy_Properties(new_op, op);

	  OP_srcpos(new_op) = OP_srcpos(op);
	  if (EBO_in_loop) EBO_Set_OP_omega ( new_op, opnd_tninfo[OP_find_opnd_use(new_op, OU_predicate)], ptn0_tninfo, NULL);
	  BB_Insert_Op_After(bb, op, new_op);
	  if (EBO_Trace_Optimization) fprintf(TFile,"combine identical add/sub\n");
	  return TRUE;
      }
    }

    if ((opcode == pred_opcode) &&
	(OP_ishl(op) || OP_ishr(op) || OP_ishru(op))) {
      INT op2_bits = TOP_opnd_use_bits(opcode, l0_opnd2_idx);
      const_val = ((UINT64)const_val << 64-op2_bits) >> 64-op2_bits;
      pred_val = ((UINT64)pred_val << 64-op2_bits) >> 64-op2_bits;
      new_const_val = const_val + pred_val;
      
      // Verify that we are in the known semantic domain
      if (new_const_val == ((UINT64)new_const_val << 64-op2_bits) >> 64-op2_bits) {
	new_opcode = TOP_opnd_immediate_variant(opcode, l0_opnd2_idx, new_const_val);
	if (new_opcode != TOP_UNDEFINED) {
          if (OP_has_predicate (op)) {
            new_op = Mk_OP (new_opcode, tnr, OP_opnd(op, 0), ptn0, Gen_Literal_TN(new_const_val, TN_size(tnr)));
            CGTARG_Predicate_OP (NULL, new_op, OP_Predicate (op), OP_PredOnFalse(op));
	    // FdF 20081015
	    if (!OP_cond_def(op))
	      Set_OP_cond_def_kind(new_op, OP_ALWAYS_UNC_DEF);
          }
          else
            new_op = Mk_OP (new_opcode, tnr, ptn0, Gen_Literal_TN(new_const_val, TN_size(tnr)));

	    OP_srcpos(new_op) = OP_srcpos(op);
	    if (EBO_in_loop) EBO_Set_OP_omega ( new_op, opnd_tninfo[OP_find_opnd_use(new_op, OU_predicate)], ptn0_tninfo, NULL);
	    BB_Insert_Op_After(bb, op, new_op);
	    if (EBO_Trace_Optimization) fprintf(TFile,"combine identical shifts\n");
	    return TRUE;
	}
      }
    }

#else
    if ((opcode == pred_opcode) &&
        ((opcode == TOP_shl)    ||
         (opcode == TOP_shl_i)  ||
         (opcode == TOP_shr_u)  ||
         (opcode == TOP_shr_i_u))) {
      new_const_val = pred_val + const_val;

      if (new_const_val <= 63) {

        switch (opcode) {
        case TOP_shl:
        case TOP_shl_i: new_opcode = TOP_shl_i; break;
        case TOP_shr_u:
        case TOP_shr_i_u: new_opcode = TOP_shr_i_u; break;
        default: return FALSE;
        }

        new_op = Mk_OP (new_opcode, tnr, OP_Predicate(op),
                        ptn0, Gen_Literal_TN(new_const_val, TN_size(tn0)));

        OP_srcpos(new_op) = OP_srcpos(op);
        if (EBO_in_loop) EBO_Set_OP_omega ( new_op, opnd_tninfo[OP_find_opnd_use(new_op, OU_predicate)], ptn0_tninfo, NULL);
        BB_Insert_Op_After(bb, op, new_op);
if (EBO_Trace_Optimization) fprintf(TFile,"combine identical shifts\n");
        return TRUE;
      }
    }
#endif

    if (pred_val == const_val) {

      /* Look for identical operations that are redundant. */
      if ((OP_iand(op) && OP_iand(pred_op) && (const_val != ~0)) ||
          (OP_ior(op) && OP_ior(pred_op) && (const_val != 0))) {
        OPS ops = OPS_EMPTY;
	if (EBO_Trace_Optimization) fprintf(TFile,"replace redundant and/or\n");
#ifdef TARG_ST
        EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), tnr, OP_result(pred_op, 0), &ops);
#else
        EBO_Exp_COPY((OP_has_predicate(op)?OP_opnd(op,OP_find_opnd_use(op, OU_predicate)):NULL),
                     tnr, OP_result(pred_op, 0), &ops);
#endif
        if (EBO_in_loop) EBO_OPS_omega (&ops, 
					OP_Predicate(op),
					(OP_has_predicate(op)?opnd_tninfo[OP_find_opnd_use(op, OU_predicate)]:NULL));

        OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);

        BB_Insert_Ops(bb, op, &ops, FALSE);
        return TRUE;
      }

     /* Look for complementary operations. */
      if ((OP_iadd(op) && OP_isub(pred_op)) ||
          (OP_isub(op) && OP_iadd(pred_op))) {
       /* Copy the original input. */
        OPS ops = OPS_EMPTY;
	if (EBO_Trace_Optimization) fprintf(TFile,"replace complementary operations\n");
#ifdef TARG_ST
        EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), tnr, ptn0, &ops);
#else
        EBO_Exp_COPY((OP_has_predicate(op)?OP_opnd(op,OP_find_opnd_use(op, OU_predicate)):NULL),
                     tnr, ptn0, &ops);
#endif
        if (EBO_in_loop) EBO_Set_OP_omega (OPS_first(&ops), opnd_tninfo[OP_find_opnd_use(OPS_first(&ops), OU_predicate)], ptn0_tninfo);

        OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);

        BB_Insert_Ops(bb, op, &ops, FALSE);
        return TRUE;
      }
    
    } /* end : if (pred_val == const_val) */

  } /* end: Sequence optimizations. */
  
  return FALSE;

#ifdef TARG_ST
  OPS_Created:
  FmtAssert(OPS_first(&ops) != NULL, ("ops empty after simplify operand 0"));

  if (OP_next(OPS_first(&ops)) != NULL) {
    /* What's the point in replacing one instruction with several? */
    return FALSE;
  }

  /* No need to replace if same op, avoids infinite loops. */
  if (OPs_Are_Equivalent(op, OPS_first(&ops))) return FALSE;

  OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);

  if (EBO_in_loop) {
    EBO_OPS_omega(&ops, OP_Predicate(op),
		  (OP_has_predicate(op)? opnd_tninfo[OP_find_opnd_use(op, OU_predicate)]:NULL));
    EBO_OPS_omega(&ops, opnd_tn[l0_opnd1_idx], opnd_tninfo[l0_opnd1_idx]);
  }
  
  BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
  return TRUE;
#endif
}

/* =====================================================================
 *    EBO_Fold_Constant_Compare
 * =====================================================================
 */
BOOL
EBO_Fold_Constant_Compare (
  OP *op, 
  TN **opnd_tn,
  INT64 *result_val_ptr
)
{
  TOP opcode = OP_code(op);
  TN *tn0, *tn1;
  INT64 tn0_val, tn1_val;
  UINT64 tn0_uval, tn1_uval;
  INT op1_idx, op2_idx;
  INT64 result_val;

  /* Currently only handle integer compares. */
  if (!OP_icmp(op)) return FALSE;

#ifdef TARG_ST
  if (OP_has_implicit_interactions(op)) return FALSE;
  if (OP_results(op) > 1) return FALSE;
#endif

  op1_idx = OP_find_opnd_use(op, OU_opnd1);
  op2_idx = OP_find_opnd_use(op, OU_opnd2);
  FmtAssert(op1_idx >= 0,("operand OU_opnd1 not defined for cmp TOP %s", TOP_Name(opcode)));
  FmtAssert(op2_idx >= 0,("operand OU_opnd2 not defined for cmp TOP %s", TOP_Name(opcode)));

  tn0 = opnd_tn[op1_idx];
  tn1 = opnd_tn[op2_idx];

  /* We could handle constant symbols (sush as floating point constant),
     but we don't. */
  if (TN_is_symbol(tn0) || TN_is_symbol(tn1)) return FALSE;
  

  tn0_val = TN_Value (tn0);
  tn1_val = TN_Value (tn1);
  
  // Then sign extend to compare bits
  INT bit_size = TOP_opnd_use_bits(opcode, op1_idx); // same as for op2_idx
  
  tn0_val = (tn0_val << 64-bit_size) >> 64-bit_size;
  tn1_val = (tn1_val << 64-bit_size) >> 64-bit_size;
  tn0_uval = (UINT64)(tn0_val << 64-bit_size) >> 64-bit_size;
  tn1_uval = (UINT64)(tn1_val << 64-bit_size) >> 64-bit_size;

#ifdef TARG_ST
      VARIANT variant = OP_cmp_variant(op);
#else
      VARIANT variant = TOP_cmp_variant(opcode);
#endif

  switch (variant) {
  case V_CMP_EQ: result_val = (tn0_val == tn1_val); break;
  case V_CMP_NE: result_val = (tn0_val != tn1_val); break;
  case V_CMP_LT: result_val = (tn0_val < tn1_val); break;
  case V_CMP_GT: result_val = (tn0_val > tn1_val); break;
  case V_CMP_LE: result_val = (tn0_val <= tn1_val); break;
  case V_CMP_GE: result_val = (tn0_val >= tn1_val); break;
  case V_CMP_LTU: result_val = (tn0_uval < tn1_uval); break;
  case V_CMP_GTU: result_val = (tn0_uval > tn1_uval); break;
  case V_CMP_LEU: result_val = (tn0_uval <= tn1_uval); break;
  case V_CMP_GEU: result_val = (tn0_uval >= tn1_uval); break;
  case V_CMP_ANDL: result_val = (tn0_val != 0 && tn1_val != 0); break;
  case V_CMP_NANDL: result_val = !(tn0_val != 0 && tn1_val != 0); break;
  case V_CMP_ORL: result_val = (tn0_val != 0 || tn1_val != 0); break;
  case V_CMP_NORL: result_val = !(tn0_val != 0 || tn1_val != 0); break;
  default: return FALSE; break;
  }

  *result_val_ptr = result_val;
  return TRUE;
}

#ifdef TARG_ST
// [HK] 20060316
/* =====================================================================
 *    EBO_Simplify_Special_Compare
 *    
 *    Look for special constant results cases for comparison:
 *    1) cmpgeu tn0, 0 = 1, cmpltu tn0, 0 = 0
 *    2) cmpleu 0, tn1 = 1, cmpgtu 0, tn1 = 0
 *    3) When TNs_Are_Equivalent(tn0, tn1), cmp tn0, tn1 = 0 or 1 
 *       depending on the cmp variant
 * =====================================================================
 */
BOOL
EBO_Simplify_Special_Compare (
  OP *op, 
  TN **opnd_tn,
  EBO_TN_INFO **opnd_tninfo
)
{
  TOP opcode = OP_code(op);
  TN *tn0, *tn1;
  INT op1_idx, op2_idx;
  INT64 result_val;

  /* Currently only handle integer compares. */
  if (!OP_icmp(op)) return FALSE;

  if (OP_has_implicit_interactions(op)) return FALSE;
  if (OP_results(op) > 1) return FALSE;

  op1_idx = OP_find_opnd_use(op, OU_opnd1);
  op2_idx = OP_find_opnd_use(op, OU_opnd2);
  FmtAssert(op1_idx >= 0,("operand OU_opnd1 not defined for cmp TOP %s", TOP_Name(opcode)));
  FmtAssert(op2_idx >= 0,("operand OU_opnd2 not defined for cmp TOP %s", TOP_Name(opcode)));

  tn0 = opnd_tn[op1_idx];
  tn1 = opnd_tn[op2_idx];

  VARIANT variant = OP_cmp_variant(op);
  if (!TN_is_constant(tn0) && (tn1 == Zero_TN)){
	  switch (variant) {
	  case V_CMP_GEU: result_val = 1; break;
	  case V_CMP_LTU: result_val = 0; break;
	  default: return FALSE; break;
	  }
	  goto  Constant_Created;
  }         
  else if (!TN_is_constant(tn1) && (tn0 == Zero_TN)){ 
	  switch (variant) {
	  case V_CMP_LEU: result_val = 1; break;
	  case V_CMP_GTU: result_val = 0; break;
	  default: return FALSE; break;
	  }
	  goto  Constant_Created;
  }
  else if (!TN_is_constant(tn0) && !TN_is_constant(tn1)){
      if (TNs_Are_Equivalent(tn0, tn1)){
	  switch (variant) {
	  case V_CMP_NE: 
	  case V_CMP_LT: 
	  case V_CMP_GT: 
	  case V_CMP_LTU: 
	  case V_CMP_GTU: 
	      result_val = 0; break;
	  case V_CMP_LE: 
	  case V_CMP_GE: 
	  case V_CMP_EQ: 
	  case V_CMP_LEU: 
	  case V_CMP_GEU:
	      result_val = 1; break;
	  default: return FALSE; break;
	  }
	  goto  Constant_Created;
      }
  }

  return FALSE;

Constant_Created:
  TN *tnr = OP_result(op,0);
  OPS ops = OPS_EMPTY;
  TN *tnc;

  result_val = sext(result_val, TN_size(tnr)*8);

  tnc = Gen_Literal_TN(result_val, TN_size(tnr));

  Expand_Immediate (tnr, tnc, OP_result_is_signed(op,0), &ops);

  if (OP_next(OPS_first(&ops)) != NULL) {
   /* What's the point in replacing one instruction with several? */
    return FALSE;
  }
  if (OP_has_predicate(op)) {
    int pred_index = OP_find_opnd_use(op, OU_predicate);
    EBO_OPS_predicate (OP_opnd(op, pred_index), OP_Pred_False(op, pred_index), OP_cond_def(op), &ops);
  }

  /* No need to replace if same op, avoids infinite loops. */
  if (OPs_Are_Equivalent(op, OPS_first(&ops))) return FALSE;

  if (EBO_in_loop) EBO_OPS_omega (&ops,
				  OP_Predicate(op),
				  (OP_has_predicate(op)? opnd_tninfo[OP_find_opnd_use(op, OU_predicate)]:NULL));

  if (OP_has_predicate (op)) {
    int pred_index = OP_find_opnd_use(op, OU_predicate);
    EBO_OPS_predicate (OP_opnd (op, pred_index), OP_Pred_False(op, pred_index), OP_cond_def(op), &ops);
  }

  OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);

  BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);

  if (EBO_Trace_Optimization) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile, "%sin BB:%d Redefine ",
            EBO_trace_pfx, BB_id(OP_bb(op)));
    Print_TN(tnr,TRUE);
    fprintf(TFile," with load of ");
    Print_TN(tnc,FALSE);
    fprintf(TFile, "\n");
  }

  return TRUE;
}

// [HK] 20070208
/* =====================================================================
 *    EBO_Simplify_Compare_Sequence
 *    
 *    Look for simplification of comparison sequences, such as:
 *    cmp tn0, tn1 = tnr1; cmp tnr1, cst = tnr2 
 *    This type of sequence can always be simplified,
 *    either to a single compare operation, or to a move to the
 *    immediate values 0 or 1.
 *   
 * =====================================================================
 */
BOOL
EBO_Simplify_Compare_Sequence (
  OP *op, 
  TN **opnd_tn,
  EBO_TN_INFO **opnd_tninfo
)
{
  TOP opcode = OP_code(op);
  TN *tn1, *tn2, *tnr;
  INT op1_idx, op2_idx;
  INT64 result_val, const_val;
  OP *in_op1, *in_op2, *in_op;
  TOP in_opcode1, in_opcode2, in_opcode;
  EBO_TN_INFO *cmp_tninfo1, *cmp_tninfo2, *cmp_tninfo;
  EBO_TN_INFO *l2_tninfo1, *l2_tninfo2;
  EBO_OP_INFO *l2_opinfo;
  VARIANT variant, variant_in;
  OPS ops = OPS_EMPTY;
  TN *tnc=NULL;
  BOOL op_created, inv_var, const_created;
 
  /* Currently only handle integer compares. */
  if (!OP_icmp(op)) return FALSE;

  if (OP_has_implicit_interactions(op)) return FALSE;
  if (OP_results(op) > 1) return FALSE;

  tnr = OP_result(op,0);

   op1_idx = OP_find_opnd_use(op, OU_opnd1);
  op2_idx = OP_find_opnd_use(op, OU_opnd2);
  FmtAssert(op1_idx >= 0,("operand OU_opnd1 not defined for cmp TOP %s", TOP_Name(opcode)));
  FmtAssert(op2_idx >= 0,("operand OU_opnd2 not defined for cmp TOP %s", TOP_Name(opcode)));

  tn1 = opnd_tn[op1_idx];
  tn2 = opnd_tn[op2_idx];

  cmp_tninfo1 = opnd_tninfo[op1_idx];
  cmp_tninfo2 = opnd_tninfo[op2_idx];

  in_op1 = cmp_tninfo1 ? cmp_tninfo1->in_op : NULL;
  in_op2 = cmp_tninfo2 ? cmp_tninfo2->in_op : NULL;

  if (in_op1)
    in_opcode1 = OP_code(in_op1);
  if (in_op2)
    in_opcode2 = OP_code(in_op2);

  if ((in_op1 == NULL) && (in_op2 == NULL)) return FALSE;
  /*
  if (((cmp_tninfo1 == NULL) ||
      (in_op1 == NULL)) &&
      ((cmp_tninfo2 == NULL) ||
       (in_op2 == NULL)))
    return FALSE;
  */
  if (TN_Has_Value(tn1) && in_op2 && OP_icmp(in_op2)){
    const_val = TN_Value (tn1);
    in_op = in_op2;
    in_opcode = in_opcode2;
    cmp_tninfo = cmp_tninfo2;
    // fix for codex bug #38449
    // if the constant is on the left-hand side
    // the compare variant has to be inverted
    opcode = OP_opnd_swapped_variant(op, op1_idx, op2_idx);
  }
  else if (TN_Has_Value(tn2) && in_op1 && OP_icmp(in_op1)){
    const_val = TN_Value (tn2);
    in_op = in_op1;
    in_opcode = in_opcode1;
    cmp_tninfo = cmp_tninfo1;
  }
  else {
    return FALSE;
  }
	   
  // get level 2 informations 
  op1_idx = OP_find_opnd_use(in_op, OU_opnd1);
  op2_idx = OP_find_opnd_use(in_op, OU_opnd2);

  tn1 = OP_opnd(in_op, op1_idx);
  tn2 = OP_opnd(in_op, op2_idx);

  l2_opinfo = locate_opinfo_entry(cmp_tninfo);
  if (l2_opinfo == NULL) return FALSE;

  l2_tninfo1 = l2_opinfo->actual_opnd[op1_idx];
  l2_tninfo2 = l2_opinfo->actual_opnd[op2_idx];

  if (((l2_tninfo1 != NULL) && !EBO_tn_available (OP_bb(op), l2_tninfo1)) ||
      ((l2_tninfo2 != NULL) && !EBO_tn_available (OP_bb(op), l2_tninfo2))) 
    return FALSE;

  variant = OP_cmp_variant(op);
  if (opcode != OP_code(op)) variant=Invert_CMP_Variant(variant);
  switch (variant) {
  case V_CMP_NE: 
    if (const_val == 0){
      inv_var = FALSE;
      op_created = TRUE;
      const_created = FALSE;
    }
    else if (const_val == 1) {
      inv_var = TRUE;
      op_created = TRUE;
      const_created = FALSE;
    }
    else {
      result_val = 1;
      op_created = FALSE;
      const_created = TRUE;
    }
    break;
  case V_CMP_LT: 
    if (const_val <= 0) {
      result_val = 0;
      op_created = FALSE;
      const_created = TRUE;
    }     
    else if (const_val > 1) {
      result_val = 1;
      op_created = FALSE;
      const_created = TRUE;
    }
    else /* const_val == 1 */ {
      inv_var = TRUE;
      op_created = TRUE;
      const_created = FALSE;
    }
    break;
  case V_CMP_GT: 
    if (const_val < 0) {
      result_val = 1;
      op_created = FALSE;
      const_created = TRUE;
    }     
    else if (const_val >= 1) {
      result_val = 0;
      op_created = FALSE;
      const_created = TRUE;
    }
    else /* const_val == 0 */ {
      inv_var = FALSE;
      op_created = TRUE;
      const_created = FALSE;
    }
    break;
  case V_CMP_LTU: 
    if (const_val == 0) {
      result_val = 0;
      op_created = FALSE;
      const_created = TRUE;
    }     
    else if (const_val > 1 || const_val < 0) {
      result_val = 1;
      op_created = FALSE;
      const_created = TRUE;
    }
    else /* const_val == 1 */ {
      inv_var = TRUE;
      op_created = TRUE;
      const_created = FALSE;
    }
    break;
  case V_CMP_GTU: 
    if (const_val >= 1 || const_val < 0) {
      result_val = 0;
      op_created = FALSE;
      const_created = TRUE;
    }
    else /* const_val == 0 */ {
      inv_var = FALSE;
      op_created = TRUE;
      const_created = FALSE;
    }
    break;
  case V_CMP_LE: 
    if (const_val < 0) {
      result_val = 0;
      op_created = FALSE;
      const_created = TRUE;
    }     
    else if (const_val >= 1) {
      result_val = 1;
      op_created = FALSE;
      const_created = TRUE;
    }
    else /* const_val == 0 */ {
      inv_var = TRUE;
      op_created = TRUE;
      const_created = FALSE;
    }
    break;
  case V_CMP_GE: 
    if (const_val <= 0) {
      result_val = 1;
      op_created = FALSE;
      const_created = TRUE;
    }     
    else if (const_val > 1) {
      result_val = 0;
      op_created = FALSE;
      const_created = TRUE;
    }
    else /* const_val == 1 */ {
      inv_var = FALSE;
      op_created = TRUE;
      const_created = FALSE;
    }
    break;
  case V_CMP_EQ: 
    if (const_val == 1) {
      inv_var = FALSE;
      op_created = TRUE;
      const_created = FALSE;
    }
    else if (const_val == 0) {
      inv_var = TRUE;
      op_created = TRUE;
      const_created = FALSE;
    }
    else {
      result_val = 0;
      op_created = FALSE;
      const_created = TRUE;
    }
    break;
  case V_CMP_LEU: 
    if (const_val >= 1 || const_val < 0){
      result_val = 1;
      op_created = FALSE;
      const_created = TRUE;
    }
    else /* const_val == 0 */ {
      inv_var = TRUE;
      op_created = TRUE;
      const_created = FALSE;
    }
  case V_CMP_GEU:
    if (const_val == 0) {
      result_val = 1;
      op_created = FALSE;
      const_created = TRUE;
    }     
    else if (const_val > 1 || const_val < 0) {
      result_val = 0;
      op_created = FALSE;
      const_created = TRUE;
    }
    else /* const_val == 1 */ {
      inv_var = FALSE;
      op_created = TRUE;
      const_created = FALSE;
    }
    break;
  default: return FALSE; break;
  }

  if (op_created) {
      variant_in = OP_cmp_variant(in_op);
    switch (variant_in) {
    case V_CMP_NE: 
      if (inv_var)
	Expand_Int_Equal(tnr, tn1, tn2, MTYPE_I4, &ops);
      else
	Expand_Int_Not_Equal(tnr, tn1, tn2, MTYPE_I4, &ops);
      break;
    case V_CMP_LT: 
      if (inv_var)
	Expand_Int_Greater_Equal(tnr, tn1, tn2, MTYPE_I4, &ops);
      else
	Expand_Int_Less(tnr, tn1, tn2, MTYPE_I4, &ops);	
      break;
    case V_CMP_GT: 
      if (inv_var)
	Expand_Int_Less_Equal(tnr, tn1, tn2, MTYPE_I4, &ops);
      else
	Expand_Int_Greater(tnr, tn1, tn2, MTYPE_I4, &ops);	
      break;
    case V_CMP_LTU: 
      if (inv_var)
	Expand_Int_Greater_Equal(tnr, tn1, tn2, MTYPE_U4, &ops);
      else
	Expand_Int_Less(tnr, tn1, tn2, MTYPE_U4, &ops);	
      break;
    case V_CMP_GTU: 
      if (inv_var)
	Expand_Int_Less_Equal(tnr, tn1, tn2, MTYPE_U4, &ops);
      else
	Expand_Int_Greater(tnr, tn1, tn2, MTYPE_U4, &ops);	
      break;
    case V_CMP_LE: 
      if (inv_var)
	Expand_Int_Greater(tnr, tn1, tn2, MTYPE_I4, &ops);
      else
	Expand_Int_Less_Equal(tnr, tn1, tn2, MTYPE_I4, &ops);	
      break;
    case V_CMP_GE: 
      if (inv_var)
	Expand_Int_Less(tnr, tn1, tn2, MTYPE_I4, &ops);
      else
	Expand_Int_Greater_Equal(tnr, tn1, tn2, MTYPE_I4, &ops);	
      break;
    case V_CMP_EQ: 
      if (inv_var)
	Expand_Int_Not_Equal(tnr, tn1, tn2, MTYPE_I4, &ops);
      else
	Expand_Int_Equal(tnr, tn1, tn2, MTYPE_I4, &ops);	
      break;
    case V_CMP_LEU: 
      if (inv_var)
	Expand_Int_Greater(tnr, tn1, tn2, MTYPE_U4, &ops);
      else
	Expand_Int_Less_Equal(tnr, tn1, tn2, MTYPE_U4, &ops);	
      break;
    case V_CMP_GEU:
      if (inv_var)
	Expand_Int_Less(tnr, tn1, tn2, MTYPE_U4, &ops);
      else
	Expand_Int_Greater_Equal(tnr, tn1, tn2, MTYPE_U4, &ops);	
      break;
    default: return FALSE; break;
    }
  }
  else if (const_created) {
    result_val = sext(result_val, TN_size(tnr)*8);
    tnc = Gen_Literal_TN(result_val, TN_size(tnr));
    Expand_Immediate (tnr, tnc, OP_result_is_signed(op,0), &ops);
  }

  if (OP_next(OPS_first(&ops)) != NULL) {
   /* What's the point in replacing one instruction with several? */
    return FALSE;
  }
  if (OP_has_predicate(op)) {
    int pred_index = OP_find_opnd_use(op, OU_predicate);
    EBO_OPS_predicate (OP_opnd(op, pred_index), OP_Pred_False(op, pred_index), OP_cond_def(op), &ops);
  }

  /* No need to replace if same op, avoids infinite loops. */
  if (OPs_Are_Equivalent(op, OPS_first(&ops))) return FALSE;

  if (EBO_in_loop) EBO_OPS_omega (&ops, 
				  OP_Predicate(op),
				  (OP_has_predicate(op) ? opnd_tninfo[OP_find_opnd_use(op, OU_predicate)] : NULL));

  OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);

  BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);

  if (EBO_Trace_Optimization) {
    #pragma mips_frequency_hint NEVER
      fprintf(TFile, "%sin BB:%d Simplify comparison, replacing ",
	      EBO_trace_pfx, BB_id(OP_bb(op)));
      Print_TN(tnr,TRUE);
    if (tnc) {
      fprintf(TFile," by definition of ");
      Print_TN(tnc,FALSE);
    }
      
      
      
    fprintf(TFile, "\n");
  }

  return TRUE;
}

// [HK] 20070327
/* =====================================================================
 *    EBO_Simplify_Compare_MinMax_Sequence
 *    
 *    Look for simplification of minmax/compare sequences, such as:
 *    min/max tn0, cst1 = tnr1; cmp tnr1, cst2 = tnr2 
 *    This type of sequence can always be simplified,
 *    either to a single compare operation, or to a move to the
 *    immediate values 0 or 1.
 *   
 * =====================================================================
 */
BOOL
EBO_Simplify_Compare_MinMaxSequence (
  OP *op, 
  TN **opnd_tn,
  EBO_TN_INFO **opnd_tninfo
)
{
  TOP opcode = OP_code(op);
  TN *tn1, *tn2, *tnr, *tn_minmax, *tn_cst;
  INT op1_idx, op2_idx;
  INT64 result_val, const_val1, const_val2;
  OP *in_op1, *in_op2, *in_op;
  TOP in_opcode1, in_opcode2, in_opcode;
  EBO_TN_INFO *minmax_tninfo1, *minmax_tninfo2, *minmax_tninfo;
  EBO_TN_INFO *l2_tninfo1, *l2_tninfo2;
  EBO_OP_INFO *l2_opinfo;
  VARIANT variant;
  OPS ops = OPS_EMPTY;
  TN *tnc=NULL;
  BOOL op_created, const_created;
 
  /* Currently only handle integer compares. */
  if (!OP_icmp(op)) return FALSE;

  if (OP_has_implicit_interactions(op)) return FALSE;
  if (OP_results(op) > 1) return FALSE;

  tnr = OP_result(op,0);

  op1_idx = OP_find_opnd_use(op, OU_opnd1);
  op2_idx = OP_find_opnd_use(op, OU_opnd2);
  FmtAssert(op1_idx >= 0,("operand OU_opnd1 not defined for cmp TOP %s", TOP_Name(opcode)));
  FmtAssert(op2_idx >= 0,("operand OU_opnd2 not defined for cmp TOP %s", TOP_Name(opcode)));

  tn1 = opnd_tn[op1_idx];
  tn2 = opnd_tn[op2_idx];

  minmax_tninfo1 = opnd_tninfo[op1_idx];
  minmax_tninfo2 = opnd_tninfo[op2_idx];

  in_op1 = minmax_tninfo1 ? minmax_tninfo1->in_op : NULL;
  in_op2 = minmax_tninfo2 ? minmax_tninfo2->in_op : NULL;

  if (in_op1)
    in_opcode1 = OP_code(in_op1);
  if (in_op2)
    in_opcode2 = OP_code(in_op2);

  if ((in_op1 == NULL) && (in_op2 == NULL)) return FALSE;

  if (TN_Has_Value(tn1) && in_op2 
      && (OP_imax(in_op2) || OP_imin(in_op2))){
    const_val1 = TN_Value (tn1);
    tn_cst = tn1;
    in_op = in_op2;
    in_opcode = in_opcode2;
    minmax_tninfo = minmax_tninfo2;
    // fix for codex bug #38449
    // if the constant is on the left-hand side
    // the compare variant has to be inverted
    opcode = OP_opnd_swapped_variant(op, op1_idx, op2_idx);
  }
  else if (TN_Has_Value(tn2) && in_op1 
	   && (OP_imax(in_op1) || OP_imin(in_op1))){
    const_val1 = TN_Value (tn2);
    tn_cst = tn2;
    in_op = in_op1;
    in_opcode = in_opcode1;
    minmax_tninfo = minmax_tninfo1;
  }
  else {
    return FALSE;
  }
	   
  // get level 2 informations 
  op1_idx = OP_find_opnd_use(in_op, OU_opnd1);
  op2_idx = OP_find_opnd_use(in_op, OU_opnd2);

  tn1 = OP_opnd(in_op, op1_idx);
  tn2 = OP_opnd(in_op, op2_idx);

  l2_opinfo = locate_opinfo_entry(minmax_tninfo);
  if (l2_opinfo == NULL) return FALSE;

  l2_tninfo1 = l2_opinfo->actual_opnd[op1_idx];
  l2_tninfo2 = l2_opinfo->actual_opnd[op2_idx];

  if (((l2_tninfo1 != NULL) && !EBO_tn_available (OP_bb(op), l2_tninfo1)) ||
      ((l2_tninfo2 != NULL) && !EBO_tn_available (OP_bb(op), l2_tninfo2))) 
    return FALSE;

  // check that one of the operand of the min/max op has a constant value
  if (TN_Value_At_Op(tn1, in_op, &const_val2)){
    tn_minmax = tn2;
  }
  else if ((l2_tninfo1->replacement_tn != NULL)
	   && TN_Has_Value(l2_tninfo1->replacement_tn)){
    const_val2 = TN_Value(l2_tninfo1->replacement_tn);
    tn_minmax = tn2;    
  }
  else if (TN_Value_At_Op(tn2, in_op, &const_val2)){
    tn_minmax = tn1;
  }
  else if ((l2_tninfo2->replacement_tn != NULL)
	   && TN_Has_Value(l2_tninfo2->replacement_tn)){
    const_val2 = TN_Value(l2_tninfo2->replacement_tn);
    tn_minmax = tn1;    
  }
  else
    return FALSE;

  if (TOP_is_unsign (in_opcode) && TOP_is_unsign (opcode))
    if ((OP_imax(in_op) && ((UINT64)const_val1 < (UINT64)const_val2))
	||(OP_imin(in_op) && ((UINT64)const_val1 > (UINT64)const_val2))){
      const_created = TRUE;
      op_created = FALSE;
    }
    else if ((OP_imax(in_op) && ((UINT64)const_val1 > (UINT64)const_val2))
	||(OP_imin(in_op) && ((UINT64)const_val1 < (UINT64)const_val2))){
      const_created = FALSE;
      op_created = TRUE;
    }
    else
      return FALSE;
  else if (!TOP_is_unsign (in_opcode) && !TOP_is_unsign (opcode))
    if ((OP_imax(in_op) && (const_val1 < const_val2))
	||(OP_imin(in_op) && (const_val1 > const_val2))){
      const_created = TRUE;
      op_created = FALSE;
    }
    else if ((OP_imax(in_op) && (const_val1 > const_val2))
	||(OP_imin(in_op) && (const_val1 < const_val2))){
      const_created = FALSE;
      op_created = TRUE;
    }
    else
      return FALSE;
  else
    return FALSE;
    
    
  if (op_created) {
      variant = OP_cmp_variant(op);
      if (opcode != OP_code(op)) variant=Invert_CMP_Variant(variant);
    switch (variant) {
    case V_CMP_NE: 
      Expand_Int_Not_Equal(tnr, tn_minmax, tn_cst, MTYPE_I4, &ops);
      break;
    case V_CMP_LT: 
      Expand_Int_Less(tnr, tn_minmax, tn_cst, MTYPE_I4, &ops);	
      break;
    case V_CMP_GT: 
      Expand_Int_Greater(tnr, tn_minmax, tn_cst, MTYPE_I4, &ops);	
      break;
    case V_CMP_LTU: 
      Expand_Int_Less(tnr, tn_minmax, tn_cst, MTYPE_U4, &ops);	
      break;
    case V_CMP_GTU: 
      Expand_Int_Greater(tnr, tn_minmax, tn_cst, MTYPE_U4, &ops);	
      break;
    case V_CMP_LE: 
      Expand_Int_Less_Equal(tnr, tn_minmax, tn_cst, MTYPE_I4, &ops);	
      break;
    case V_CMP_GE: 
      Expand_Int_Greater_Equal(tnr, tn_minmax, tn_cst, MTYPE_I4, &ops);	
      break;
    case V_CMP_EQ: 
      Expand_Int_Equal(tnr, tn_minmax, tn_cst, MTYPE_I4, &ops);	
      break;
    case V_CMP_LEU: 
      Expand_Int_Less_Equal(tnr, tn_minmax, tn_cst, MTYPE_U4, &ops);	
      break;
    case V_CMP_GEU:
      Expand_Int_Greater_Equal(tnr, tn_minmax, tn_cst, MTYPE_U4, &ops);	
      break;
    default: return FALSE; break;
    }
  }
  else if (const_created) {
      variant = OP_cmp_variant(op);
      if (opcode != OP_code(op)) variant=Invert_CMP_Variant(variant);
    switch (variant) {
    case V_CMP_NE: 
      result_val = 1;
      break;
    case V_CMP_LT: 
      result_val = const_val2 < const_val1;
      break;
    case V_CMP_GT: 
      result_val = const_val2 > const_val1;
       break;
    case V_CMP_LTU: 
      result_val = (UINT64)const_val2 < (UINT64)const_val1;
      break;
    case V_CMP_GTU: 
      result_val = (UINT64)const_val2 > (UINT64)const_val1;
      break;
    case V_CMP_LE: 
      result_val = const_val2 <= const_val1;
      break;
    case V_CMP_GE: 
      result_val = const_val2 >= const_val1;
      break;
    case V_CMP_EQ: 
      result_val = 0;
      break;
    case V_CMP_LEU: 
      result_val = (UINT64)const_val2 <= (UINT64)const_val1;
      break;
    case V_CMP_GEU:
      result_val = (UINT64)const_val2 >= (UINT64)const_val1;
      break;
    default: return FALSE; break;
    }
    result_val = sext(result_val, TN_size(tnr)*8);
    tnc = Gen_Literal_TN(result_val, TN_size(tnr));
    Expand_Immediate (tnr, tnc, OP_result_is_signed(op,0), &ops);
  }

  if (OP_next(OPS_first(&ops)) != NULL) {
   /* What's the point in replacing one instruction with several? */
    return FALSE;
  }
  if (OP_has_predicate(op)) {
    int pred_index = OP_find_opnd_use(op, OU_predicate);
    EBO_OPS_predicate (OP_opnd(op, pred_index), OP_Pred_False(op, pred_index), OP_cond_def(op), &ops);
  }

  /* No need to replace if same op, avoids infinite loops. */
  if (OPs_Are_Equivalent(op, OPS_first(&ops))) return FALSE;

  if (EBO_in_loop) EBO_OPS_omega (&ops, 
				  OP_Predicate(op),
				  (OP_has_predicate(op) ? opnd_tninfo[OP_find_opnd_use(op, OU_predicate)] : NULL));

  OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);

  BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);

  if (EBO_Trace_Optimization) {
    #pragma mips_frequency_hint NEVER
      fprintf(TFile, "%sin BB:%d Simplify minmax/compare sequence, replacing ",
	      EBO_trace_pfx, BB_id(OP_bb(op)));
      Print_TN(tnr,TRUE);
    if (tnc) {
      fprintf(TFile," by definition of ");
      Print_TN(tnc,FALSE);
    }
      
      
      
    fprintf(TFile, "\n");
  }

  return TRUE;
}
#endif



#ifdef TARG_ST
/* ============================================================================
 *    EBO_OP_dont_fold(op)
 *
 *    Returns false if the OP is marked as not beneficial to fold.
 *
 *    For now, this is not an attribute of the OP itself but a global phasing
 *    status. In particular CBPO (ref cbpo.cxx) will split address computation
 *    and this split must not be reversed (i.e. re-folded).
 *    For this we use a marker on the operation, actually the rematerializable
 *    property. The rule is thus:
 *    An operation must not be folded if:
 *    1. EBO runs after CBPO (approximated by !EBO_in_pre), and,
 *    2. the operation is rematerializable.
 *    
 *    A typical case is:
 *      MAKE Y = symbol + 8 
 *      MAKE Z = symbol + 12
 *    CBPO generates:
 *      MAKE Y = symbol + 8 
 *      MAKE Z = Y + 4 	  (set remat flag) 
 *    We want to avoid EBO re-generating the original code. 
 *    Hence EBO_OP_dont_fold() must return true.
 *
 * ============================================================================
 */
static BOOL
EBO_OP_dont_fold(OP *op)
{
  BOOL check_remat = OP_has_result(op);
  int i;
  
  if (EBO_in_pre) return FALSE;
  
  for (i = 0; i < OP_results(op) && check_remat; i++) {
    check_remat = TN_is_rematerializable(OP_result(op, i));
  }
  if (!check_remat) return FALSE;

  return TRUE;
}
#endif


/* =====================================================================
 *    EBO_Fold_Constant_Expression
 *
 *    Look at an exression that has all constant operands and attempt to
 *    evaluate the expression.
 *
 *    Supported operations are:
 *      add, sub, mult, and, or, xor, nor, sll, srl, slt
 * =====================================================================
 */
BOOL
EBO_Fold_Constant_Expression (
  OP *op,
  TN **opnd_tn,
  EBO_TN_INFO **opnd_tninfo
)
{

#ifdef TARG_ST
  // [SC] We can also enter this function to fold a store with false
  // predicate, so allow that case.
  FmtAssert((OP_results(op) > 0
	     || (OP_has_predicate(op)
		 && TN_is_zero (opnd_tn[OP_find_opnd_use(op, OU_predicate)]))),
	    ("Folding an operator with no result"));
#else
  FmtAssert(OP_results(op) > 0, ("Folding an operator with no result"));
#endif

  TOP opcode = OP_code(op);

  TN *tnr = OP_result(op,0);
  TN *tn0 = NULL;
  TN *tn1 = NULL;
  INT64 tn0_val = 0;
  INT64 tn1_val = 0;
  INT64 result_val;
  ST *result_sym = NULL;
  INT32 result_relocs;
  INT op1_idx, op2_idx;

#ifdef TARG_ST
  if (OP_has_implicit_interactions(op)) return FALSE;
  if (OP_results(op) > 1) return FALSE;
  if (OP_icmp(op) && OP_Predicate(op) && OP_Pred_False(op, OP_find_opnd_use(op, OU_predicate)) && OP_Predicate(op)==True_TN) return FALSE;
#endif

  if (EBO_Trace_Execution) {
    INT i;
    INT opndnum = OP_opnds(op);
    fprintf(TFile, "%sin BB:%d Constant OP :- %s ",
            EBO_trace_pfx, BB_id(OP_bb(op)),TOP_Name(opcode));
    for (i = 0; i < opndnum; i++) {
      fprintf(TFile," ");
      Print_TN(opnd_tn[i],TRUE);
    }
    fprintf(TFile,"\n");
  }

#ifdef TARG_ST
  if (EBO_OP_dont_fold(op)) {
    if (EBO_Trace_Execution) {
      fprintf(TFile,"<ebo> %s: ops marked dont_fold -> do nothing:\n",
	      __FUNCTION__);
    }
    return FALSE;
  }
#endif

#ifdef TARG_ST
  if (OP_has_predicate(op) && TN_is_zero (opnd_tn[OP_find_opnd_use(op, OU_predicate)])) {
#else
  if (OP_has_predicate(op) && (opnd_tn[OP_find_opnd_use(op, OU_predicate)] == Zero_TN)) {
#endif

    if (OP_Is_Unconditional_Compare(op)) {
      /* 
       * Unconditional compares define results even if the predicate 
       * is "FALSE". 
       */
      OPS ops = OPS_EMPTY;

#ifdef TARG_ST
      FmtAssert(FALSE,("TODO: not implemented"));
#else
      if (OP_result(op,0) != True_TN) {
	Build_OP (TOP_cmp_eq_unc, True_TN, OP_result(op,0), True_TN, Zero_TN, Zero_TN, &ops);
      }
      if (OP_result(op,1) != True_TN) {
	Build_OP (TOP_cmp_eq_unc, True_TN, OP_result(op,1), True_TN, Zero_TN, Zero_TN, &ops);
      }
#endif

      if (OP_glue(op)) {
	OP *next_op = OPS_first(&ops);
	while (next_op != NULL) {
	  Set_OP_glue(next_op);
	  next_op = OP_next(next_op);
	}
      }

      if (EBO_Trace_Optimization) {
	INT i;
	fprintf(TFile, "%sin BB:%d Result of compare is FALSE for all: ", EBO_trace_pfx,BB_id(OP_bb(op)));
	for (i = 0; i < OP_results(op); i++) {
	  Print_TN(OP_result(op,i),FALSE); fprintf(TFile,", ");
	}
      }

      if (OPS_length(&ops) != 0) {
#ifdef TARG_ST
        EBO_OPS_predicate (OP_Predicate (op), OP_PredOnFalse(op), OP_cond_def(op), &ops);
#endif
	OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);

	BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
      }
      return TRUE;
    }

    /* The OP can be deleted. */
    return (!OP_glue(op));
  }

  //
  // First of all, check if this is a target-specific opcode
  //
  if (EBO_Fold_Special_Opcode(op, opnd_tn, &result_val)) {
    goto Constant_Created;
  }

  

  // [CG]: use opnd1, opnd2 semantic to get effective operands
  // opnd2 may be undefined 
  op1_idx = OP_find_opnd_use(op, OU_opnd1);
  op2_idx = OP_find_opnd_use(op, OU_opnd2);
  if (op1_idx < 0) return FALSE;
  tn0 = opnd_tn[op1_idx];
  tn1 = op2_idx < 0 ? NULL : opnd_tn[op2_idx];

  if (TN_Has_Value(tn0) && (tn1 == NULL || TN_Has_Value(tn1))) {

    if (OP_iadd(op)) {
      result_val = TOP_fold_iadd(opcode, opnd_tn);
      goto Constant_Created;
    }

    else if (OP_isub(op)) {
      result_val = TOP_fold_isub(opcode, opnd_tn);
      goto Constant_Created;
    }

    else if (OP_imul(op)) {
      result_val = TOP_fold_imul(opcode, opnd_tn);
      goto Constant_Created;
    }

    else if (OP_imin(op)) {
      result_val = TOP_fold_imin(opcode, opnd_tn);
      goto Constant_Created;
    }

    else if (OP_imax(op)) {
      result_val = TOP_fold_imax(opcode, opnd_tn);
      goto Constant_Created;
    }

    else if (OP_iand(op)) {
      result_val = TOP_fold_and(opcode, opnd_tn);
      goto Constant_Created;
    }

    else if (OP_ior(op)) {
      result_val = TOP_fold_or(opcode, opnd_tn);
      goto Constant_Created;
    }

    else if (OP_ixor(op)) {
      result_val = TOP_fold_xor(opcode, opnd_tn);
      goto Constant_Created;
    }

    else if (OP_ishl(op)) {
      result_val = TOP_fold_shl(opcode, opnd_tn);
      goto Constant_Created;
    }
    
    else if (OP_ishr(op)) {
      result_val = TOP_fold_shr(opcode, opnd_tn);
      goto Constant_Created;
    }
    
    else if (OP_ishru(op)) {
      result_val = TOP_fold_shru(opcode, opnd_tn);
      goto Constant_Created;
    }
    
    else if (OP_zext(op)) {
      result_val = TOP_fold_zext(opcode, opnd_tn);
      goto Constant_Created;
    }

    else if (OP_sext(op)) {
      result_val = TOP_fold_sext(opcode, opnd_tn);
      goto Constant_Created;
    }

    else if (OP_icmp(op)) {
#ifdef TARG_ST
      result_val = OP_fold_icmp(op, opnd_tn);
#else
      result_val = TOP_fold_icmp(OP_code(op), opnd_tn);
#endif
      goto Constant_Created;
    }

    else if (OP_select(op)) {
      INT64 op1 = TOP_fetch_opnd(opcode, opnd_tn, op1_idx);
      INT64 op2 = TOP_fetch_opnd(opcode, opnd_tn, op2_idx);
      if (op1 == op2) {
	result_val = op1;
	goto Constant_Created;
      }
    }
  
  } else if (tn1 != NULL && (TN_is_symbol(tn0) || TN_is_symbol(tn1))) {
    if (TN_is_symbol(tn0) && TN_is_symbol(tn1)) return FALSE;
    if (OP_iadd(op) || OP_isub(op)) {
      if (OP_iadd(op) && TN_is_symbol(tn1)) { 
	/* Swap opnds. */
	INT tmp_idx = op1_idx; TN *tmp = tn0; 
	op1_idx = op2_idx; tn0 = tn1; 
	op2_idx = tmp_idx; tn1 = tmp; 
      }
      if (TN_is_symbol(tn1)) return FALSE;
      tn0_val = TOP_fetch_opnd(opcode, opnd_tn, op1_idx);
      tn1_val = TOP_fetch_opnd(opcode, opnd_tn, op2_idx);
      result_val = OP_iadd(op) ? tn0_val + tn1_val : tn0_val - tn1_val;
      result_sym = TN_var(tn0);
      result_relocs = TN_relocs(tn0);
      goto Constant_Created;
    }
  }


  return FALSE;

Constant_Created:

  OPS ops = OPS_EMPTY;
  TN *tnc;

  if (EBO_Trace_Execution) {
    fprintf(TFile,"<ebo> %s: constant created\n",__FUNCTION__);
  }

  result_val = sext(result_val, TN_size(tnr)*8);

  if (result_sym != NULL) {
    tnc = Gen_Symbol_TN(result_sym, result_val, result_relocs);
  } else {
    tnc = Gen_Literal_TN(result_val, TN_size(tnr));
  }

  Exp_Immediate (tnr, tnc, OP_result_is_signed(op,0), &ops);
  if (OP_next(OPS_first(&ops)) != NULL) {
    /* What's the point in replacing one instruction with several? */
    if (EBO_Trace_Execution) {
      fprintf(TFile,"<ebo> %s: try to replace one instruction by several instructions -> do nothing:\n",
	      __FUNCTION__);
      Print_OPS_No_SrcLines(&ops);
    }
    return FALSE;
  }
#ifdef TARG_ST
  EBO_OPS_predicate (OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), &ops);
#else
  if (OP_has_predicate(op)) {
    EBO_OPS_predicate (OP_Predicate(op), &ops);
  }
#endif

  /* No need to replace if same op, avoids infinite loops. */
  if (OPs_Are_Equivalent(op, OPS_first(&ops))) {
    if (EBO_Trace_Execution) {
      fprintf(TFile,"<ebo> %s: ops are equivalent -> do nothing:\n",
	      __FUNCTION__);
      Print_OPS_No_SrcLines(&ops);
    }
    return FALSE;
  }

#ifdef TARG_ST
  if(OPS_first(&ops) == OPS_last(&ops) &&
     CGTARG_is_expensive_load_imm(OPS_first(&ops))) {
      OPS tmpOps = OPS_EMPTY;
      OP* prev = OP_prev(op);
      OP* next = OP_next(op);
      OPS_Append_Op(&tmpOps, op);
      op->next = next;
      op->prev = prev;
      if(CGTARG_sequence_is_cheaper_than_load_imm(&tmpOps, OPS_first(&ops))) {
          INT op1_idx= OP_find_opnd_use(op,OU_opnd1);
          EBO_TN_INFO *op_tninfo = opnd_tninfo[op1_idx];
          if ((opnd_tn[op1_idx] && TN_is_symbol(opnd_tn[op1_idx])) || !op_tninfo || (op_tninfo && op_tninfo->reference_count>1)) {
              if (EBO_Trace_Execution) {
                  fprintf(TFile,"<ebo> %s: ops are more expensive -> do nothing:\n",
                           __FUNCTION__);
                  Print_OPS_No_SrcLines(&ops);
              }
              return FALSE;
          }
      }
  }
#endif

  if (EBO_in_loop) EBO_OPS_omega (&ops, 
				  OP_Predicate(op),
				  (OP_has_predicate(op) ? opnd_tninfo[OP_find_opnd_use(op, OU_predicate)] : NULL));
  OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);

  BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);

  if (EBO_Trace_Optimization) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile, "%sin BB:%d Redefine ",
            EBO_trace_pfx, BB_id(OP_bb(op)));
    Print_TN(tnr,TRUE);
    fprintf(TFile," with load of ");
    Print_TN(tnc,FALSE);
    fprintf(TFile, "\n");
  }

  return TRUE;
}

#endif /* TARG_ST , starting at function EBO_delete_subset_mem_op */




/* 
 * find_duplicate_mem_op
 *
 * For a given memory op, look for a preceeding memory op to 
 * the same location and attempt to replace one of them.
 * Return TRUE if this memory op is no longer needed.
 */
static BOOL
find_duplicate_mem_op (BB *bb,
                       OP *op,
                       TN **opnd_tn,
                       EBO_TN_INFO **opnd_tninfo,
                       EBO_TN_INFO **actual_tninfo
#ifndef TARG_ST
                       , int op_num,
		       int *regs_used,
		       int *last_fat_point
#endif
                       )
/* -----------------------------------------------------------------------
 * Requires: 
 * Returns TRUE if the operands of each OP are identical.
 * -----------------------------------------------------------------------
 */
{
  INT hash_value = 0;
  INT hash_search_length = 0;
  EBO_OP_INFO *opinfo;
  EBO_OP_INFO *intervening_opinfo = NULL;
  EBO_OP_INFO *adjacent_location = NULL;
  INT64 adjacent_offset_pred;
  INT64 adjacent_offset_succ;
#if defined( KEY) && !defined(TARG_ST)
  // TRUE if OP is inside the live range of the replacement TN.
  BOOL inside_lr = TRUE;
  // If extending the live range of the replacement TN, this is the replacement
  // TN's tninfo.
  EBO_TN_INFO *extend_lr_tninfo = NULL;
  EBO_REG_ENTRY reginfo;
#endif

  if (EBO_in_loop) {
    if ((CG_LOOP_unroll_level == 2) && (Is_Multibb_Loop(EBO_loop))) {
      // It is possible to have no loop info on the unrolled multi block loop
      if (_CG_LOOP_info(op) == NULL)
        return FALSE;
    }
  }

  if (op == NULL) return FALSE;

#ifdef TARG_X8664
  // Load-exe-store is always needed because it modifies memory.  Bug 15061.
  if (OP_load_exe_store(op))
    return FALSE;
#endif

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter find_duplicate_mem_op\n",EBO_trace_pfx);
  }
#ifdef TARG_ST
  // FdF 20060517: No support for automod addressing mode
  if (OP_automod(op)) return FALSE;
#endif

#if defined( KEY) && !defined(TARG_ST)
  // Take OP_prefetch into account.
  if( OP_prefetch(op) ){
    return Combine_L1_L2_Prefetches( op, opnd_tn, opnd_tninfo );
  }
#endif

  if (!(OP_load(op) || OP_store(op))) return FALSE;

 /* Determine the indexes of the address components of this memory op. */
  INT succ_base_idx = TOP_Find_Operand_Use(OP_code(op),OU_base);
  INT succ_offset_idx = TOP_Find_Operand_Use(OP_code(op),OU_offset);

#ifndef TARG_ST
  // [CG]: Obsolete
  if ((succ_base_idx >= 0) && (succ_offset_idx >= 0) &&
      TN_Is_Constant(opnd_tn[succ_offset_idx])) {
   /* Look for merge-able expressions. */
    merge_memory_offsets (op, succ_base_idx, opnd_tn, actual_tninfo, opnd_tninfo);
  }
#endif

 /* Determine the address components of this memory op. */
  TN *succ_base_tn = (succ_base_idx >= 0) ? opnd_tn[succ_base_idx] : NULL;
  EBO_TN_INFO *succ_base_tninfo = (succ_base_idx >= 0) ? opnd_tninfo[succ_base_idx] : NULL;
  TN *succ_offset_tn = (succ_offset_idx >= 0) ? opnd_tn[succ_offset_idx] : NULL;
  EBO_TN_INFO *succ_offset_tninfo = (succ_offset_idx >= 0) ? opnd_tninfo[succ_offset_idx] : NULL;

#ifdef KEY
  TN *succ_index_tn = NULL;
  TN *succ_scale_tn = NULL;
  EBO_TN_INFO *succ_index_tninfo = NULL;
  EBO_TN_INFO *succ_scale_tninfo = NULL;
#endif

#if defined(TARG_X8664) || defined(TARG_MIPS)
  const INT succ_index_idx = TOP_Find_Operand_Use(OP_code(op),OU_index);
  succ_index_tn = (succ_index_idx >= 0) ? opnd_tn[succ_index_idx] : NULL;
  succ_index_tninfo = (succ_index_idx >= 0) ? opnd_tninfo[succ_index_idx] : NULL;
#endif

#ifdef TARG_X8664
  const INT succ_scale_idx = TOP_Find_Operand_Use(OP_code(op),OU_scale);
  succ_scale_tn = (succ_scale_idx >= 0) ? opnd_tn[succ_scale_idx] : NULL;
  succ_scale_tninfo = (succ_scale_idx >= 0) ? opnd_tninfo[succ_scale_idx] : NULL;

  // For bug 5809, don't CSE "xmm = mov offset(base)" because this increases
  // xmm registers pressure, causing LRA to spill a lot.
  if (CG_sse_load_execute != 0 &&
      OP_load(op) &&
      succ_index_idx == -1 &&
      TN_register_class(OP_result(op, 0)) == ISA_REGISTER_CLASS_float) {
    return FALSE;
  }
#endif

  if ((succ_offset_tn == NULL) && (succ_base_tn != NULL)) {
    find_index_and_offset(succ_base_tninfo,
                          &succ_base_tn, &succ_base_tninfo,
                          &succ_offset_tn, &succ_offset_tninfo);
  }

 /* Determine the proper hash value. */
  hash_value = EBO_hash_op( op, opnd_tninfo);

  if (EBO_Trace_Hash_Search) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sLook for redundant memory ops in hash chain %d for\n\t",
                  EBO_trace_pfx,hash_value);
    Print_OP_No_SrcLine(op);
  }

  opinfo = (OP_has_implicit_interactions(op) || OP_unalign_mem(op)) ? NULL : EBO_opinfo_table[hash_value];

  while (opinfo) {
    OP *pred_op = opinfo->in_op;
    INT64 offset_pred = 0;
    INT64 offset_succ = 0;

   /* Identify the address components of the predecessor memory op. */
    INT pred_base_idx = 0;
    INT pred_offset_idx = 0;
    TN *pred_base_tn = NULL;
    EBO_TN_INFO *pred_base_tninfo = NULL;
    TN *pred_offset_tn = NULL;
    EBO_TN_INFO *pred_offset_tninfo = NULL;
#ifdef KEY
    INT pred_index_idx = 0;
    TN* pred_index_tn = NULL;
    EBO_TN_INFO* pred_index_tninfo = NULL;
    INT pred_scale_idx = 0;
    TN* pred_scale_tn = NULL;
    EBO_TN_INFO* pred_scale_tninfo = NULL;
#endif

#ifdef KEY
    if (pred_op && OP_code(pred_op) == TOP_intrncall) break;  // 14955
#endif

#ifdef TARG_X8664
    // Load-exe-store is always needed because it modifies memory.
    // Furthermore, unlike load or store, load-exe-store does not have a
    // register copy of the memory value for reuse.  Bug 15061.
    if (pred_op &&
	OP_load_exe_store(pred_op))
      break;
#endif
#ifdef TARG_ST
    // FdF 200605017: No support for automod addresing mode
    if (pred_op && OP_automod(pred_op))
      pred_op = NULL;
#endif

    if ((pred_op != NULL) &&
        (OP_load(pred_op) || OP_store(pred_op)
#ifdef TARG_X8664
	 || OP_load_exe(pred_op)
#endif
	 )) {
     /* Determine the address components of the predecessor memory op. */
      pred_base_idx = TOP_Find_Operand_Use(OP_code(pred_op),OU_base);
      pred_offset_idx = TOP_Find_Operand_Use(OP_code(pred_op),OU_offset);
#ifdef TARG_ST
      // FdF 20070402
      pred_base_tn = (pred_base_idx >= 0) ? Opinfo_optimal_tn(opinfo, pred_base_idx) : NULL;
#else
      pred_base_tn = (pred_base_idx >= 0) ? OP_opnd(pred_op,pred_base_idx) : NULL;
#endif
      pred_base_tninfo = (pred_base_idx >= 0) ? opinfo->optimal_opnd[pred_base_idx] : NULL;
#ifdef TARG_ST
      // FdF 20070402
      pred_offset_tn = (pred_offset_idx >= 0) ? Opinfo_optimal_tn(opinfo, pred_offset_idx) : NULL;
#else
      pred_offset_tn = (pred_offset_idx >= 0) ? OP_opnd(pred_op,pred_offset_idx) : NULL;
#endif
      pred_offset_tninfo = (pred_offset_idx >= 0) ? opinfo->optimal_opnd[pred_offset_idx] : NULL;
#if defined(TARG_X8664) || defined(TARG_MIPS)
      pred_index_idx = TOP_Find_Operand_Use(OP_code(pred_op),OU_index);
      pred_index_tn = (pred_index_idx >= 0) ?
			OP_opnd(pred_op,pred_index_idx) : NULL;
      pred_index_tninfo = (pred_index_idx >= 0) ?
			    opinfo->optimal_opnd[pred_index_idx] : NULL;
#endif
#ifdef TARG_X8664
      pred_scale_idx = TOP_Find_Operand_Use(OP_code(pred_op),OU_scale);
      pred_scale_tn = (pred_scale_idx >= 0) ?
		        OP_opnd(pred_op,pred_scale_idx) : NULL;
      pred_scale_tninfo = (pred_scale_idx >= 0) ?
			    opinfo->optimal_opnd[pred_scale_idx] : NULL;
#endif      
    }

    if ((pred_offset_tn == NULL) && (pred_base_tn != NULL)) {
      find_index_and_offset(pred_base_tninfo,
                            &pred_base_tn, &pred_base_tninfo,
                            &pred_offset_tn, &pred_offset_tninfo);
    }

    BOOL hash_op_matches = ((pred_op != NULL) &&
#ifdef TARG_X8664
                            (OP_memory(pred_op) || OP_load_exe(pred_op)) &&
#else
                            OP_memory(pred_op) &&
#endif
#ifdef TARG_X8664
			    (CGTARG_Is_Thread_Local_Memory_OP(op) ==
			     CGTARG_Is_Thread_Local_Memory_OP(pred_op)) &&
#endif
#if defined( KEY) && !defined(TARG_ST)
			    (pred_index_tn == succ_index_tn)         &&
			    (pred_index_tninfo == succ_index_tninfo) &&
			    (pred_scale_tn == succ_scale_tn)         &&
			    (pred_scale_tninfo == succ_scale_tninfo) &&
#endif
                            (pred_base_tn == succ_base_tn) &&           /* The base  index must match */
                            (pred_base_tninfo == succ_base_tninfo) &&   /* The base   info must match */
                            (pred_offset_tninfo == succ_offset_tninfo)) /* The offset info must match */
                            ? TRUE : FALSE;
    BOOL op_is_subset = FALSE;
    BOOL offsets_may_overlap = TRUE;

    hash_search_length++;

    if (hash_op_matches &&
        (pred_offset_tn != succ_offset_tn)) {
     /* The offset tn's need to be looked at in more detail. */

      ST *symbol_pred = ((pred_offset_tn != NULL) && TN_is_symbol(pred_offset_tn)) ?TN_var(pred_offset_tn) : NULL;
      ST *symbol_succ = ((succ_offset_tn != NULL) && TN_is_symbol(succ_offset_tn)) ?TN_var(succ_offset_tn) : NULL;
      mUINT8 relocs_pred = (pred_offset_tn != NULL) ? TN_relocs(pred_offset_tn) : 0;
      mUINT8 relocs_succ = (succ_offset_tn != NULL) ? TN_relocs(succ_offset_tn) : 0;
      offset_pred = (pred_offset_tn != NULL) ? TN_offset(pred_offset_tn) : 0;
      offset_succ = (succ_offset_tn != NULL) ? TN_offset(succ_offset_tn) : 0;

#ifdef TARG_ST
      // FdF 20070418: Use the base symbol and offset for comparison
      if (symbol_pred != NULL && Base_Offset_Is_Known (symbol_pred)) {
	ST *base_st;
	INT64 base_offset;
	Base_Symbol_And_Offset(symbol_pred, &base_st, &base_offset);
	symbol_pred = base_st;
	offset_pred += base_offset;
      }
      if (symbol_succ != NULL && Base_Offset_Is_Known (symbol_succ)) {
	ST *base_st;
	INT64 base_offset;
	Base_Symbol_And_Offset(symbol_succ, &base_st, &base_offset);
	symbol_succ = base_st;
	offset_succ += base_offset;
      }
#endif

     /* This time, the relocations must be the same. */
      hash_op_matches = (symbol_pred == symbol_succ) && (relocs_pred == relocs_succ);

      if ((OP_prefetch(op) || OP_prefetch(pred_op)) &&
          (offset_pred != offset_succ)) {
        hash_op_matches = FALSE;
      } else if (hash_op_matches) {
       /* If the relocations are the same, we need to examine the offsets and sizes. */
#ifdef TARG_ST
        INT size_pred = OP_Mem_Ref_Bytes(pred_op);
        INT size_succ = OP_Mem_Ref_Bytes(op);
#else
        INT size_pred = CGTARG_Mem_Ref_Bytes(pred_op);
        INT size_succ = CGTARG_Mem_Ref_Bytes(op);
#endif

        if ((offset_pred == offset_succ) &&
            (size_pred == size_succ)) {
         /* The perfect match: location and size. */
        }  else if ((offset_pred <= offset_succ) &&
                    ((offset_pred + size_pred) >= (offset_succ + size_succ))) {
         /* The current reference is a subset of the preceeding one. */
          op_is_subset = TRUE;
        } else if (OP_load(op) && OP_load(pred_op) && (size_pred == size_succ) &&
                   (((offset_pred + size_pred) == offset_succ) ||
                    ((offset_succ + size_succ) == offset_pred))) {
          offsets_may_overlap = FALSE;
          if (adjacent_location == NULL) {
            adjacent_location = opinfo;
            adjacent_offset_pred = offset_pred;
            adjacent_offset_succ = offset_succ;
          }
        } else if (((offset_pred + size_pred) <= offset_succ) ||
                   ((offset_succ + size_succ) <= offset_pred)) {
         /* There is no potential overlap. */
          offsets_may_overlap = FALSE;
        } else {
         /* Any other case may be a potential conflict. */
          hash_op_matches = FALSE;
        }
      }
    }

#ifdef TARG_ST
    /*  op_is_subset is set to TRUE  when op and op_pred are load operations
	with a different sign extension */
    if (hash_op_matches &&
	pred_offset_tn == succ_offset_tn &&
        OP_load(op) && OP_load(pred_op) &&
	TOP_is_unsign (OP_code(op)) != TOP_is_unsign (OP_code(pred_op))) {
          op_is_subset = TRUE;
    }
#endif

    if (hash_op_matches && !offsets_may_overlap ) {
      if (EBO_Trace_Hash_Search) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sMemory overlap not possible with\n\t",
                      EBO_trace_pfx);
        Print_OP_No_SrcLine(pred_op);
      }
      opinfo = opinfo->same;
      continue;
    }

#ifdef TARG_ST
    if (hash_op_matches &&
	(OP_has_predicate(op) || OP_has_predicate(pred_op))) {
#else
    if (hash_op_matches && OP_has_predicate(op)) {
#endif
     /* Check predicates for safety. */

      if (OP_store(op) && OP_store(pred_op)) {
#ifdef TARG_ST
        if (!EBO_OP_predicate_dominates(op, actual_tninfo,
					pred_op, opinfo->optimal_opnd) &&
            !EBO_OP_predicate_complements(op, actual_tninfo,
					  pred_op, opinfo->optimal_opnd))
#else
            if (!EBO_predicate_dominates(OP_opnd(op,OP_PREDICATE_OPND),
                                         actual_tninfo[OP_PREDICATE_OPND],
                                         OP_opnd(pred_op,OP_PREDICATE_OPND),
                                         opinfo->optimal_opnd[OP_PREDICATE_OPND]) &&
                !EBO_predicate_complements(OP_opnd(op,OP_PREDICATE_OPND),
                                           actual_tninfo[OP_PREDICATE_OPND],
                                           OP_opnd(pred_op,OP_PREDICATE_OPND),
                                           opinfo->optimal_opnd[OP_PREDICATE_OPND])) 
#endif
                {
                    if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sStore predicates do not match\n\t", EBO_trace_pfx);
            Print_OP_No_SrcLine(pred_op);
          }

         /* We need to be extra safe with stores.  Assume a conflict. */
          hash_op_matches = FALSE;
          break;
        }
      } else if (!OP_store(op) && !OP_store(pred_op)) {
        if ((intervening_opinfo != NULL) &&
#ifdef TARG_ST
            !EBO_OP_predicate_dominates(pred_op,
					opinfo->optimal_opnd,
					op,
					actual_tninfo))
#else
            !EBO_predicate_dominates(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                     OP_opnd(op,OP_PREDICATE_OPND),
                                     actual_tninfo[OP_PREDICATE_OPND])) 
#endif
        {
          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sRe-load predicates do not match\n\t", EBO_trace_pfx);
            Print_OP_No_SrcLine(pred_op);
          }

         /* A mixture of predicates on the loads may cause problems. */
          hash_op_matches = FALSE;
          break;
        }
#ifdef TARG_ST
        if (!EBO_OP_predicate_dominates(pred_op,
					opinfo->optimal_opnd,
					op,
					actual_tninfo) &&
            !EBO_OP_predicate_complements(op,
					  actual_tninfo,
					  pred_op,
					  opinfo->optimal_opnd))
#else
        if (!EBO_predicate_dominates(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                     OP_opnd(op,OP_PREDICATE_OPND),
                                     actual_tninfo[OP_PREDICATE_OPND]) &&
            !EBO_predicate_complements(OP_opnd(op,OP_PREDICATE_OPND),
                                       actual_tninfo[OP_PREDICATE_OPND],
                                       OP_opnd(pred_op,OP_PREDICATE_OPND),
                                       opinfo->optimal_opnd[OP_PREDICATE_OPND])) 
#endif
        {

          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sLoad predicates do not match\n\t", EBO_trace_pfx);
            Print_OP_No_SrcLine(pred_op);
          }

         /* We can't use this load, but we can keep looking. */
          hash_op_matches = FALSE;

        }
      } else if (!OP_store(op) && OP_store(pred_op)) {
#ifdef TARG_ST
        if (!EBO_OP_predicate_dominates(pred_op,
					opinfo->optimal_opnd,
					op,
					actual_tninfo) &&
            !EBO_OP_predicate_complements(op,
					  actual_tninfo,
					  pred_op,
					  opinfo->optimal_opnd))
#else
        if (!EBO_predicate_dominates(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                     OP_opnd(op,OP_PREDICATE_OPND),
                                     actual_tninfo[OP_PREDICATE_OPND]) &&
            !EBO_predicate_complements(OP_opnd(op,OP_PREDICATE_OPND),
                                       actual_tninfo[OP_PREDICATE_OPND],
                                       OP_opnd(pred_op,OP_PREDICATE_OPND),
                                       opinfo->optimal_opnd[OP_PREDICATE_OPND])) 
#endif
        {

          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sStore/load predicates do not match\n\t", EBO_trace_pfx);
            Print_OP_No_SrcLine(pred_op);
          }

         /* We need to be extra safe with stores.  Assume a conflict. */
          hash_op_matches = FALSE;
          break;
        }
      }
    }

    if ((pred_op != NULL) &&
        (hash_value == EBO_DEFAULT_MEM_HASH) &&
        (OP_store(pred_op) != OP_store(op)) &&
        (!OP_prefetch(op))) {
     /* Need to be careful about alias issues. */
      WN *pred_wn;
      WN *succ_wn;
      ALIAS_RESULT result;

      if (EBO_Trace_Hash_Search) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sPotential Alias problem with\n\t",EBO_trace_pfx);
        Print_OP_No_SrcLine(pred_op);
      }
#ifdef CG_DEP_ALIAS
      {
	{
	  BOOL definite;
	  int aliased = CG_DEP_Mem_Ops_Alias(pred_op, op, &definite);
          result = aliased ? POSSIBLY_ALIASED: NOT_ALIASED;
          result = definite ? SAME_LOCATION: result;
#else
      result = POSSIBLY_ALIASED;
      if (Alias_Manager != NULL) {
        pred_wn = OP_hoisted(pred_op) ? NULL : Get_WN_From_Memory_OP(pred_op);
        succ_wn = OP_hoisted(op) ? NULL : Get_WN_From_Memory_OP(op);
        if ((pred_wn != NULL) && (succ_wn != NULL)) {
          result = Aliased(Alias_Manager, pred_wn, succ_wn);
#endif
          if ((!hash_op_matches) && (result == SAME_LOCATION)) {
           /* This also implies that the size of the items is the same. */
            hash_op_matches = TRUE;
            if (EBO_Trace_Hash_Search) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sAlias_Manager identifies SAME_LOCATION\n",
                             EBO_trace_pfx);    
            }
           /* The Alias_Manager may think that the locations are the same,
              but we know that they may not be.  This is because if-conversion
              may create something called a "black hole".  It is hard to believe
              that the optimizations we do will be OK in this situation. */
            if (OP_store(op)) opinfo->op_must_not_be_moved = TRUE;
            break;
          }
        }
      }

      if ((result == POSSIBLY_ALIASED) && (!hash_op_matches)) {
        if ((intervening_opinfo == NULL) && OP_store(pred_op) && OP_load(op)) {
          intervening_opinfo = opinfo;
          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sContinue looking for duplicate memory op across interving dependency %d.\n",
                          EBO_trace_pfx,result);
          }
          goto do_next;
        }
        if (OP_store(pred_op)) opinfo->op_must_not_be_removed = TRUE;

        if (EBO_Trace_Hash_Search) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sAlias information(%d) prevents us from continuing.\n",
                        EBO_trace_pfx,result);
        }
        break;
      }
      if (hash_op_matches && op_is_subset &&
          OP_store(op) && OP_store(pred_op)) {
        opinfo->op_must_not_be_removed = TRUE;
        if (EBO_Trace_Hash_Search) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sOverlapping store prevents us from continuing.\n",
                        EBO_trace_pfx);
        }
        break;
      }
      if ((intervening_opinfo != NULL) && (!hash_op_matches)) {
        opinfo->op_must_not_be_removed = TRUE;
        if (EBO_Trace_Hash_Search) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sMultiple possible intervening stores prevents us from continuing.\n",
                        EBO_trace_pfx);
        }
        break;
      }

      if (EBO_Trace_Hash_Search) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sAlias information allows us to continue.\n",EBO_trace_pfx);
      }
    }

    if (in_delay_slot && !OP_store(op)) {
     /* In general, we can't remove an OP from a delay slot, but
        duplicate stores are removed by noop'ing the pred_op. */
      break;
    }

    if (hash_op_matches && 
        (pred_op != NULL) &&
        (OP_load(pred_op) ||
         (OP_store(pred_op) && !OP_store(op)))) {

      if (OP_store(pred_op)) {
        INT pred_stored_idx = TOP_Find_Operand_Use(OP_code(pred_op),OU_storeval);
#ifdef TARG_ST
	// [CG] Handle Multi ops
	BOOL not_available = FALSE;
	INT i;
	for (i = pred_stored_idx; i < OP_opnds(pred_op); i++) {
	  if (!TN_Is_Constant(OP_opnd(pred_op, i)) &&
	      !EBO_tn_available(bb,opinfo->actual_opnd[i])) {
	    not_available = TRUE;
	  }
	}
	if (not_available) {
	  opinfo->op_must_not_be_removed = TRUE;
	  if (EBO_Trace_Hash_Search) {
	    #pragma mips_frequency_hint NEVER
	    fprintf(TFile,"%sMemory match found, but stored value is not available\n\t",
		    EBO_trace_pfx);
	    Print_OP_No_SrcLine(pred_op);
	  }
	  break;
	}
#else
        TN *pred_tn = OP_opnd(pred_op,pred_stored_idx);
        if (!TN_Is_Constant(pred_tn)) {
         /* The stored register needs to be available at this point. */
          if (!EBO_tn_available(bb,opinfo->actual_opnd[pred_stored_idx])) {
            opinfo->op_must_not_be_removed = TRUE;
            if (EBO_Trace_Hash_Search) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sMemory match found, but stored value is not available\n\t",
                            EBO_trace_pfx);
              Print_OP_No_SrcLine(pred_op);
            }
            break;
          }
#ifdef KEY
	  // See if a register is available to hold the stored value.
	  EBO_REG_ENTRY_ptr(reginfo) = hTN_MAP_Get(regs_map, pred_tn);
	  if (!TN_is_global_reg(pred_tn) &&
	      // OP is not inside the live range of the replacement TN?
	      !(inside_lr = (EBO_REG_ENTRY_last_use_OP_num(reginfo)>=op_num)) &&
	      // No register available to hold the loaded value?
	      opinfo->op_num < last_fat_point[TN_register_class(pred_tn)]) {
            if (EBO_Trace_Hash_Search) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sMemory match found, but no register available to keep stored value\n\t",
                            EBO_trace_pfx);
              Print_OP_No_SrcLine(pred_op);
            }
            break;
	  }
	  if (!inside_lr)
	    extend_lr_tninfo = opinfo->actual_opnd[pred_stored_idx];
#endif
        }
#endif
      } else {
/* The previous result needs to be available at this point. */
#ifdef TARG_ST
	// [CG] Handle Multi ops
	BOOL not_available = FALSE;
	INT i;
	for (i = 0; i < OP_results(pred_op); i++) {
	  if (!TN_is_constant(OP_result(pred_op,i)) &&
	      !EBO_tn_available(bb,opinfo->actual_rslt[i])) {
	    not_available = TRUE;
	  }
	}
	if (not_available) {
	  if (EBO_Trace_Hash_Search) {
	    #pragma mips_frequency_hint NEVER
	    fprintf(TFile,"%sMemory match found, but loaded value is not available\n\t",
		    EBO_trace_pfx);
	    Print_OP_No_SrcLine(pred_op);
	  }
	  break;
	}
#else

        TN *pred_tn = OP_result(pred_op,0);
        if (!TN_Is_Constant(pred_tn)) {
          if (!EBO_tn_available(bb,opinfo->actual_rslt[0])) {
            if (EBO_Trace_Hash_Search) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sMemory match found, but loaded value is not available\n\t",
                            EBO_trace_pfx);
              Print_OP_No_SrcLine(pred_op);
            }
            break;
          }
#ifdef KEY
	  // See if a register is available to hold the loaded value.
	  EBO_REG_ENTRY_ptr(reginfo) = hTN_MAP_Get(regs_map, pred_tn);
	  if (!TN_is_global_reg(pred_tn) &&
	      // OP is not inside the live range of the replacement TN?
	      !(inside_lr = (EBO_REG_ENTRY_last_use_OP_num(reginfo)>=op_num)) &&
	      // No register available to hold the loaded value?
	      opinfo->op_num < last_fat_point[TN_register_class(pred_tn)]) {
            if (EBO_Trace_Hash_Search) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sMemory match found, but no register available to keep loaded value\n\t",
                            EBO_trace_pfx);
              Print_OP_No_SrcLine(pred_op);
            }
            break;
	  }
	  if (!inside_lr)
	    extend_lr_tninfo = opinfo->actual_rslt[0];
#endif
        }
#endif
      }
    }

    if (hash_op_matches) {
      BOOL op_replaced = FALSE;

      if (OP_volatile(pred_op)) {
       /* If we match a volatile memory op, this
          one should have been volatile, too. */
        break;
      }

      if (EBO_Trace_Hash_Search) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sMatch after searching %d items on hash chain %d\n",
                      EBO_trace_pfx,hash_search_length,hash_value);
      }
#ifdef TARG_ST
      if (intervening_opinfo != NULL) {
        op_replaced = EBO_delete_reload_across_dependency (op, opnd_tninfo, opinfo, intervening_opinfo);
      } else if (op_is_subset) {
        op_replaced = EBO_delete_subset_mem_op (op, opnd_tninfo, opinfo, offset_pred, offset_succ);
      } else {
        op_replaced = EBO_delete_duplicate_op (op, opnd_tninfo, opinfo);
      }
#else
      if (intervening_opinfo != NULL) {
        op_replaced = delete_reload_across_dependency (op, opnd_tninfo, opinfo, intervening_opinfo);
      } else if (op_is_subset) {
        op_replaced = delete_subset_mem_op (op, opnd_tninfo, opinfo, offset_pred, offset_succ);
      } else {
        op_replaced = delete_duplicate_op (op, opnd_tninfo, opinfo
#ifdef TARG_X8664
					   , actual_tninfo
#endif
					  );
      }
#endif
      if (op_replaced) {
#if defined( KEY) && !defined(TARG_ST)
	if (extend_lr_tninfo != NULL)
	  Extend_Live_Range(regs_used, last_fat_point, op_num, extend_lr_tninfo,
			    reginfo, bb);
#endif
        return TRUE;
      } else {
       /* If we matched once and failed to eliminate it,
          we may need to keep both around. */
        if (intervening_opinfo != NULL) {
          opinfo = intervening_opinfo;
          pred_op = intervening_opinfo->in_op;
        }
        if (OP_store(op)) opinfo->op_must_not_be_moved = TRUE;
        if (op_is_subset || (hash_value == EBO_DEFAULT_MEM_HASH)) {
          if (OP_store(pred_op)) opinfo->op_must_not_be_removed = TRUE;
          break;
        }
        if (OP_store(pred_op) || OP_store(op)) {
          break;
        }
      }
    }

do_next:
  opinfo = opinfo->same;
  }

  if (adjacent_location != NULL) {
#ifdef TARG_ST
     BOOL op_replaced = EBO_combine_adjacent_loads 
#else
    BOOL op_replaced = combine_adjacent_loads 
#endif
    (op, 
#ifdef TARG_ST
						   opnd_tn,
#endif
                                               opnd_tninfo, 
                                               adjacent_location,
                                               adjacent_offset_pred, 
                                               adjacent_offset_succ);
    if (op_replaced) {
      return TRUE;
    }
  }

  if (EBO_Trace_Hash_Search) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sFail after searching %d items on memory hash chain %d\n",
                  EBO_trace_pfx,hash_search_length,hash_value);
  }

  return FALSE;
}




/* 
 * find_duplicate_op
 *
 * For a given expression op, look for a preceeding indentical
 * expressionn and attempt to replace the new one.
 * Return TRUE if this expression is no longer needed.
 */
static BOOL
find_duplicate_op (BB *bb,
                   OP *op,
                   TN **opnd_tn,
                   EBO_TN_INFO **opnd_tninfo,
                   EBO_TN_INFO **actual_tninfo)
/* -----------------------------------------------------------------------
 * Requires: 
 * Returns TRUE if the operands of each OP are identical.
 * -----------------------------------------------------------------------
 */
{
  INT opcount;
  INT opndnum;
  INT hash_value = 0;
  INT hash_search_length = 0;
  EBO_OP_INFO *opinfo;
  BOOL hash_op_matches = FALSE;

  if (op == NULL) return FALSE;

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter find_duplicate_op\n",EBO_trace_pfx);
  }
  
  opcount = OP_opnds(op);

  if (OP_memory(op)) return FALSE;

#ifdef TARG_X8664
  if( OP_load_exe(op) )    return FALSE;

  if( TOP_is_change_rflags( OP_code(op) ) ){
    for( OP* next_op = OP_next(op); next_op != NULL; next_op = OP_next( next_op ) ){
      if( OP_reads_rflags( next_op ) )
	return FALSE;

      if( TOP_is_change_rflags( OP_code(next_op) ) )
	break;
    }
  }
#endif

  if (!OP_results(op)) return FALSE;
#ifdef TARG_ST
  if (OP_has_implicit_interactions(op) ||
      OP_code(op) == TOP_asm) return FALSE;
#endif

  /* Bug#459.
     We cannot replace a "duplicated" cond_def operation by
     a mov operation without knowing the condition.
   */
#if defined(TARG_MIPS) || defined(TARG_X8664)
  // For MIPS, we can not eliminate duplicate conditionally defined ops
  // Example:
  //   TNa :- ld TNz($sp)
  //   TNa :- movn TNy1 TNz
  //   TNa :- movz TNx TNz    // pred_op
  // and,
  //   TNb :- ld TNz($sp)
  //   TNb :- movn TNy2 TNz
  //   TNb :- movz TNx TNz    // op
  // op can not be eliminated by using pred_op
  if (OP_cond_def(op)) return FALSE;
#endif
 /* Compute a hash value for the OP. */
  hash_value = EBO_hash_op( op, opnd_tninfo);

  if (EBO_Trace_Hash_Search) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sLook for duplicates at hash value %d of\n\t",EBO_trace_pfx,hash_value);
    Print_OP_No_SrcLine(op);
  }

#ifdef TARG_X8664
  opinfo = (opcount == 0 && !EBO_Can_Eliminate_Zero_Opnd_OP(op)) ?
	      NULL : EBO_opinfo_table[hash_value];
#else
  opinfo = (opcount == 0) ? NULL : EBO_opinfo_table[hash_value];
#endif

  while (opinfo) {
    OP *pred_op = opinfo->in_op;

    hash_search_length++;
    hash_op_matches =    (pred_op != NULL)
#ifdef KEY
      /* bug#480
	 Do not consider a wild op whose owner bb is unknown.
	 TODO:
	 Routines inside ebo_special.cxx should not call BB_Remove_Op() directly.
	 Try using remove_op() and OP_Change_To_Noop() instead.
       */
                      && OP_bb(pred_op) != NULL
#endif
                      && (OP_results(op) == OP_results(pred_op))
		      && (OP_opnds(op) == OP_opnds(pred_op))
#ifdef TARG_ST
      // Remove hack for double predicate define on IA64
                      && (OP_code(op) == OP_code(pred_op));
#else
		      && (OP_results(op) == 2 || OP_code(op) == OP_code(pred_op));
#endif

    if (hash_op_matches) {

      for (opndnum = 0; opndnum < opcount; opndnum++) {
#ifdef TARG_ST
        // (cbr) operands are inversed guards.
        if (OP_Pred_False(op, opndnum) != OP_Pred_False(pred_op, opndnum)) {
          hash_op_matches = FALSE;
          break;
        }
#endif
        if (OP_has_predicate(op) && (opndnum == OP_PREDICATE_OPND)) {
         /* Check predicates later. */
          continue;
        }
        if (opinfo->optimal_opnd[opndnum] == opnd_tninfo[opndnum]) {
          if (!TN_Is_Constant(opnd_tn[opndnum])) {
           /* If operands are not constants and the tninfo_entries match,
              then the values represented must also be identical. */
            continue;
          }
#ifdef TARG_ST
         // TDR: To cope with new compare model, we must check the condition 
         if (TOP_is_cmp(OP_code(op)) 
        		  && (TOP_Find_Operand_Use(OP_code(op), OU_condition) == opndnum) 
        		  && (TOP_Find_Operand_Use(OP_code(pred_op), OU_condition) == opndnum)
        		  && (TN_enum(OP_opnd(op,opndnum)) == TN_enum(OP_opnd(pred_op,opndnum)))) {
        	continue;
          }
#endif

         /* Items that are constant (i.e. have a NULL tninfo_entry pointer)
            must be checked to verify that the constants are the same.
            Note that there are several "reasonable" combinations that
            can come up:
              1. The constants have identical TNs.
              2. The OPs have identical TNs and the predecessor hasn't changed.
              3. The TNs have been resolved to the same constant, but the
                 operand of the OP (for some reason) could not be changed to
                 reference a constant.
         */
          if (/* case 1 */(opnd_tn[opndnum] == OP_opnd(pred_op, opndnum)) ||
              /* case 2 */((OP_opnd(op, opndnum) == OP_opnd(pred_op, opndnum)) &&
                           EBO_tn_available(bb, opinfo->actual_opnd[opndnum])) ||
              /* case 3 */((opinfo->actual_opnd[opndnum] != NULL) && 
                           (opnd_tn[opndnum] == opinfo->actual_opnd[opndnum]->replacement_tn))) {
            continue;
          }
        }

       /* Operands don't match - get out of inner loop and try next OP. */
        hash_op_matches = FALSE;
        break;
      }

    }

    if (hash_op_matches && 
        (pred_op != NULL)) {
      int resnum;

      for (resnum = 0; resnum < OP_results(op); resnum++) {
       /* All of the results need to be available at this point. */
        if (!TN_is_const_reg(OP_result(op,resnum)) &&
            !EBO_tn_available(bb,opinfo->actual_rslt[resnum])
#ifdef TARG_ST
            || (!CGTARG_registerclass_may_be_copied
                (TN_register_class(OP_result(op,resnum))))
#endif
            ) {

          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sExpression match found, but the result[%d] value is not available\n\t",
                           EBO_trace_pfx,resnum);
            Print_OP_No_SrcLine(pred_op);
          }

          hash_op_matches = FALSE;
          break;
        }
      }

    }

    if (in_delay_slot) {
     /* We can't insert or remove items from the delay slot. */
      break;
    }

    if (hash_op_matches && OP_has_predicate(op)) {
     /* Check predicates for safety. */
      /* We check that either:
       * - pred_op predicate dominates the current op predicate, such that
       *   the current op can be replaced by the preceding op,
       * - or both predicates are complement under some predicate, such that both ops can
       *   be replaced by a single op with some conditional move.
       * Thus the function EBO_delete_duplicate_op() must handle both cases.
       */
#ifdef TARG_ST
        if (!EBO_OP_predicate_dominates(pred_op,
					opinfo->optimal_opnd,
					op,
					actual_tninfo) &&
            !EBO_OP_predicate_complements(pred_op,
					  opinfo->optimal_opnd,
					  op,
					  actual_tninfo))
#else
        if (!EBO_predicate_dominates(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                     OP_opnd(op,OP_PREDICATE_OPND),
                                     actual_tninfo[OP_PREDICATE_OPND]) &&
            !EBO_predicate_complements(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                       opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                       OP_opnd(op,OP_PREDICATE_OPND),
                                       actual_tninfo[OP_PREDICATE_OPND]))
#endif
        {
          hash_op_matches = FALSE;

          if (EBO_Trace_Hash_Search) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sExpression match found, but the predicates do not match\n\t",
                           EBO_trace_pfx);
            Print_OP_No_SrcLine(pred_op);
          }

        }
    }

    if (hash_op_matches) {

      if (EBO_Trace_Hash_Search) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sMatch after searching %d items on expression hash chain %d\n",
                      EBO_trace_pfx,hash_search_length,hash_value);
      }
#ifdef TARG_ST
      if (EBO_delete_duplicate_op (op, opnd_tninfo, opinfo)) 
#else
      if (delete_duplicate_op (op, opnd_tninfo, opinfo)) 
#endif
      {
        return TRUE;
      } else {
       /* If we matched once and failed to eliminate it,
          we need to keep both around. */
        break;
      }
    }

  opinfo = opinfo->same;
  }

  if (EBO_Trace_Hash_Search) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sFail after searching %d items on hash chain %d\n",
                  EBO_trace_pfx,hash_search_length,hash_value);
  }

  return FALSE;
}




/* 
 * find_previous_constant
 *
 * For a given expression op, look for a preceeding indentical
 * expressionn and attempt to replace the new one.
 * Return TRUE if this expression is no longer needed.
 */
static BOOL
find_previous_constant (OP *op,
                        EBO_TN_INFO **actual_tninfo)
{
  TN *const_tn = OP_opnd(op,copy_operand(op));
  EBO_TN_INFO *predicate_tninfo = (OP_has_predicate(op)?actual_tninfo[OP_PREDICATE_OPND]:NULL);
  EBO_TN_INFO *check_tninfo;

  if (!TN_is_constant(const_tn)) return FALSE;
  if (TN_is_const_reg(const_tn)) return FALSE;
  if (OP_cond_def(op)) return FALSE;

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter find_previous_constant %d\n",
                  EBO_trace_pfx,(INT32)TN_value(const_tn));
  }

  for (check_tninfo = EBO_last_tninfo;
       check_tninfo != NULL;
       check_tninfo = check_tninfo->prior) {
    if (check_tninfo->replacement_tn == const_tn) {
       /* The asigned register needs to be available at this point. */
      TN *pred_tn = check_tninfo->local_tn;
      OP *pred_op = check_tninfo->in_op;

      if (TN_register_class(OP_result(op, 0)) != TN_register_class(pred_tn)) {
        continue;
      }
      if ((pred_op != NULL) && OP_has_predicate(op) && OP_has_predicate(pred_op)) {
       /* Check predicates for safety. */
        EBO_OP_INFO *opinfo = locate_opinfo_entry(check_tninfo);
#ifdef TARG_ST
        if (opinfo == NULL ||
            !EBO_OP_predicate_dominates(pred_op,
					opinfo->optimal_opnd,
					op,
					actual_tninfo)) {
          if (OP_code(op) == OP_code(pred_op) &&
              EBO_OP_predicate_complements(pred_op,
					   opinfo->optimal_opnd,
					   op,
					   actual_tninfo) &&
              EBO_delete_duplicate_op (op, actual_tninfo, opinfo))
#else
        if ((opinfo == NULL) ||
            !EBO_predicate_dominates(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                     OP_opnd(op,OP_PREDICATE_OPND),
                                     predicate_tninfo)) {
          if ((OP_code(op) == OP_code(pred_op)) &&
              EBO_predicate_complements(OP_opnd(pred_op,OP_PREDICATE_OPND),
                                     opinfo->optimal_opnd[OP_PREDICATE_OPND],
                                     OP_opnd(op,OP_PREDICATE_OPND),
                                     predicate_tninfo) &&
              delete_duplicate_op (op, actual_tninfo, opinfo)) 
#endif
          {
            return TRUE;
          }

         /* This previous definition is not always available, keep looking. */
          continue;
        }
      }
 
#ifdef TARG_ST
      /* [JV] TN should not be rematerializable !? */
      if (EBO_tn_available(OP_bb(op),check_tninfo))
#else
      if (EBO_tn_available(OP_bb(op),check_tninfo) &&
          (TN_is_rematerializable(pred_tn))) 
#endif
      {
          OPS ops = OPS_EMPTY;
#ifdef TARG_ST
          EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), OP_result(op, 0), pred_tn, &ops);
#else
          EBO_Exp_COPY((OP_has_predicate(op)?OP_opnd(op,OP_PREDICATE_OPND):NULL),
                       OP_result(op, 0), pred_tn, &ops);
#endif
          if (EBO_in_loop) {
            CG_LOOP_Init_Op(OPS_first(&ops));
#ifdef TARG_ST
	    if (OP_has_predicate(OPS_first(&ops))) {
	      Set_OP_omega (OPS_first(&ops),
			    OP_find_opnd_use(OPS_first(&ops), OU_predicate),
			    (predicate_tninfo != NULL)?predicate_tninfo->omega:0);
	    }
#else
            Set_OP_omega (OPS_first(&ops),
                          OP_PREDICATE_OPND,
                          (predicate_tninfo != NULL)?predicate_tninfo->omega:0);
#endif
          }
          OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);
          BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
#ifdef TARG_X8664
	  // On x86, "tn = ld const" can be deleted by folding the const into
	  // the OP that uses the const.  Since the new copy uses the TN, make
	  // sure the TN is defined.
	  inc_ref_count(check_tninfo);
#endif

          if (EBO_Trace_Optimization) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sAvoid reloading constant ",EBO_trace_pfx);
            Print_TN(const_tn,FALSE);
            fprintf(TFile," into ");
            Print_TN(OP_result(op, 0),FALSE);
            fprintf(TFile,"\n");
          }
        return TRUE;
      }
      return FALSE;
    }
  }

  return FALSE;
}


/*
 * Check if a load/cmp load exec peep is flow safe
 */
static BOOL
EBO_Load_Flow_Safe(OP *ld_op, BB* ld_bb, BB *cmp_bb)
{
  TN *ld_tn = OP_result(ld_op,0);
  BB *other_bb = NULL;
  BB *search_bb = NULL;
  BBLIST *lst, *slst;
  BOOL flow_safe = FALSE;
  int ld_bb_succ_len = BB_succs_len(ld_bb);
  int cmp_bb_pred_len = BB_preds_len(cmp_bb);

  // exclude handlers
  if (BB_handler(ld_bb) || BB_handler(cmp_bb))
    return FALSE;

  // Since we are motioning a load, no calls may be present
  // in our analysis pass of the related flow, sides effects can
  // exist where the memory location is changed.
  if (BB_call(ld_bb) || BB_call(cmp_bb))
    return FALSE;

  // We are safe if both ops are in the same block and ld_tn does not escape
  if (ld_bb == cmp_bb)
    return (TN_is_global_reg(ld_tn) == FALSE);

  // Only evaluate global tn candidates
  if (TN_is_global_reg(ld_tn) == FALSE)
    return FALSE;

  // Only 1 pred block allowed from cmp block, this ensures
  // nobody else is providing a value for the global ld_tn.
  if (cmp_bb_pred_len != 1)
    return FALSE;
  
  // Now examine the flow we have
  for (lst = BB_preds(cmp_bb); lst != NULL; lst = BBLIST_next(lst)) {
    BB *bb = BBLIST_item(lst);
    if (bb != ld_bb) {
      // iff a single block exists between the ld_bb and the cmp_bb
      // with nothing but ft flow from the ld_bb to it.
      if (ld_bb_succ_len == 1) {
        slst = BB_succs(bb);
        if (BBLIST_item(slst) != cmp_bb) {
          return FALSE;
        } else {
          search_bb = bb;
          flow_safe = TRUE;
        }
      }
    } else {
      flow_safe = TRUE;
      search_bb = ld_bb;
    }
  }
 
  // presence of handlers or calls means we cannot proceed
  if (search_bb && (BB_handler(search_bb) || BB_call(search_bb)))
    return FALSE;

  if (flow_safe == FALSE)
    return FALSE;

  // check for simple flow
  for (lst = BB_succs(search_bb); lst != NULL; lst = BBLIST_next(lst)) {
    BB *bb = BBLIST_item(lst);
    if (bb != cmp_bb)
      other_bb = bb;
  }

  // if only the cmp_bb exists as a successor ld_bb, we need only
  // check if our ld_tn is live out.
  if (other_bb == NULL) {
    // If the tn is live out of the cmp_bb we must fail also
    if (GRA_LIVE_TN_Live_Outof_BB(ld_tn, cmp_bb))
      return FALSE;
    else
      return TRUE;
  }

#ifdef KEY
  if (EBO_no_liveness_info_available)
    return FALSE;
#endif

  // If the tn is live out of the cmp_bb we must fail also
  if (GRA_LIVE_TN_Live_Outof_BB(ld_tn, cmp_bb))
    return FALSE;

  // The final constraint is that the load's tn must not be live in here
  return (GRA_LIVE_TN_Live_Into_BB(ld_tn, other_bb) == FALSE);
}

#ifdef TARG_ST
/* [TTh]
 * Return TRUE if <tn_replace> can be used to replace the TN  currently used
 * by operand <opnd_i> of operation <op>. It mainly checks that the register
 * class and subclass constraints are met.
 * Return FALSE otherwise.
 */
static BOOL
Is_Valid_Opnd_Replacement_Candidate(TN* tn_replace, OP *op, INT opnd_i)
{
  ISA_REGISTER_CLASS    rc = OP_opnd_reg_class(op, opnd_i);
  ISA_REGISTER_SUBCLASS sc = OP_opnd_reg_subclass(op, opnd_i);

  if (rc == TN_register_class(tn_replace) &&
      (!has_assigned_reg(tn_replace) ||
       (sc == ISA_REGISTER_SUBCLASS_UNDEFINED ||
	REGISTER_SET_MemberP(REGISTER_SUBCLASS_members(sc), TN_register(tn_replace))))) {
    return TRUE;
  }
  return FALSE;
}
#endif

/* 
 * Iterate through a Basic Block and build EBO_TN_INFO entries.
 */
static BOOL
Find_BB_TNs (BB *bb)
{
  OP *op;
  BOOL no_barriers_encountered = TRUE;
  BOOL scalar_replacement_performed = FALSE;
#ifdef KEY
  OP *op_with_reg_usage_info, *next_op_with_reg_usage_info;
  INT op_count = 0;
#endif

#ifdef TARG_X8664
  if (EBO_in_loop) {
    if ((CG_LOOP_unroll_level == 2) && (Is_Multibb_Loop(EBO_loop))) {
      // Only process blocks in our loop
      if (BB_SET_MemberP(LOOP_DESCR_bbset(EBO_loop),bb) == FALSE)
        return no_barriers_encountered;
    }
  }
#endif

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter Find_BB_TNs BB:%d%s\n",
            EBO_trace_pfx,BB_id(bb),EBO_in_peep?" - peep ":" ");
    Print_BB(bb);
  }

#if defined( KEY) && !defined(TARG_ST)
  Estimate_Reg_Usage(bb, &MEM_local_pool);
#endif

#ifdef TARG_X8664
  // Delete dead LRA spills.
  BS *LRA_spilled_value_is_used = NULL;
  if (EBO_in_peep) {
    Init_Remove_Dead_LRA_Stores(&LRA_spilled_value_is_used, &MEM_local_pool);
  }

  // x87
  BOOL in_x87_state = FALSE;
  OP *maybe_redundant_EMMS_OP = NULL;

#ifdef TARG_X8664
  BOOL do_load_execute;
  if (CG_cmp_load_exec)
    do_load_execute = ( CG_load_execute > 0 ) && !EBO_in_loop;
  else
    do_load_execute = ( CG_load_execute > 0 ) && !EBO_in_pre && !EBO_in_loop;
#else
  BOOL do_load_execute = ( CG_load_execute > 0 ) && !EBO_in_pre && !EBO_in_loop;
#endif

  if( do_load_execute ){
    Init_Load_Exec_Map( bb, &MEM_local_pool );
  }
#endif

 /* Allocate the dynamic arrays for various operand info. The minimum
    size we allocate is large enough for all OPs with a fixed number
    of operands. If we just allocated the size based on the BB OP with the
    greatest number of operands, then a transformation could produce
    an OP with more operands. It is expected that these new OPs will 
    always have fixed operands, which is why we use OP_MAX_FIXED_OPNDS
    (we of course verify this assumption). */
  INT max_opnds = OP_MAX_FIXED_OPNDS;
  FOR_ALL_BB_OPs (bb, op) {
    INT nopnds = OP_opnds(op);
    if (nopnds > max_opnds) max_opnds = nopnds;
#ifdef KEY
    op_count++;
#endif
  }
  TN **opnd_tn = TYPE_ALLOCA_N(TN *, max_opnds);
  EBO_TN_INFO **opnd_tninfo = TYPE_ALLOCA_N(EBO_TN_INFO *, max_opnds);
  EBO_TN_INFO **orig_tninfo = TYPE_ALLOCA_N(EBO_TN_INFO *, max_opnds);

  in_delay_slot = FALSE;

#if defined( KEY) && !defined(TARG_ST)
  int op_num = 0;
  int *regs_used;
  // For CSE purposes, a fat point is where register demand exceeds the number
  // of registers available.  It is represented as a OP number.
  int last_fat_point[ISA_REGISTER_CLASS_MAX+1];
  ISA_REGISTER_CLASS cl;

  // Initialize register usage and fat points.
  regs_used = (int*)alloca((ISA_REGISTER_CLASS_MAX+1)*(op_count+1)*sizeof(int));
  memset (regs_used, 0,
	  (ISA_REGISTER_CLASS_MAX+1) * (op_count+1) * sizeof(int));
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    last_fat_point[cl] = 0;
  }

  // Identify the next OP that has register usage info generated by
  // Estimate_Reg_Usage.  EBO opts may create replacement OPs, whose register
  // info we haven't compute.
  op_with_reg_usage_info = BB_first_op(bb);
#endif

   FOR_ALL_BB_OPs (bb, op) {
    TN *tn;
    INT opndnum;
    INT resnum;
    EBO_TN_INFO *tninfo;
    TN *tn_replace;
    INT num_opnds = OP_opnds(op);
    TN *rslt_tn = NULL;
    INT rslt_num = 0;
    INT cmp_merge_idx = -1;
    BOOL opnds_constant = TRUE;
    BOOL op_replaced = FALSE;
    BOOL op_is_predicated = OP_has_predicate(op)?TRUE:FALSE;
    BOOL op_is_cmp_merge_cand = FALSE;
    TN *op_predicate_tn = NULL;
    EBO_TN_INFO *op_predicate_tninfo = NULL;
    BOOL check_omegas = (EBO_in_loop && _CG_LOOP_info(op))?TRUE:FALSE;

   /* The assumption is that this can never occur, but make sure it doesn't! */
    FmtAssert(num_opnds <= max_opnds, ("dynamic array allocation was too small!"));

#if defined( KEY) && !defined(TARG_ST)
    // Must sync op_num with the op_num in Estimate_Reg_Usage.
    if (op == op_with_reg_usage_info) {
      op_num++;
      next_op_with_reg_usage_info = OP_next(op);
    }
#endif
#ifdef TARG_ST
    if (OP_Is_Barrier(op) || OP_access_reg_bank(op)) {
#else
    if (CGTARG_Is_OP_Barrier(op) || OP_access_reg_bank(op)) {
#endif
#ifdef TARG_ST
      if (EBO_Special_Sequence(op, NULL, NULL)) 
#else
      if (Special_Sequence(op, NULL, NULL))
#endif
      {
       /* We were able to restrict propagation of the specific registers. */
        if (EBO_Trace_Execution) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sWe were able to restrict propagation of the specific registers in BB:%d\t",
                  EBO_trace_pfx,BB_id(OP_bb(op)));
          Print_OP_No_SrcLine(op);
        }  
      } else {
       /* We could not identify the specific registers involved. */
        if (EBO_Trace_Execution) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sFind_BB_TNs Barrier OP encountered\t",EBO_trace_pfx);
          Print_OP_No_SrcLine(op);
        }
        no_barriers_encountered = FALSE;
      }
    }

#ifdef TARG_ST
    if (OP_code(op) == TOP_asm) {
      ASM_OP_ANNOT* asm_info = (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op);
      ISA_REGISTER_CLASS rc;
      FOR_ALL_ISA_REGISTER_CLASS(rc) {
	REGISTER_SET regset = ASM_OP_clobber_set(asm_info)[rc];
	for (REGISTER reg = REGISTER_SET_Choose(regset);
	     reg != REGISTER_UNDEFINED;
	     reg = REGISTER_SET_Choose_Next(regset, reg)) {
	  TN* ctn = Build_Dedicated_TN (rc, reg,0);
	  set_tn_info(ctn, NULL);

	  if (EBO_Trace_Data_Flow) {
	    fprintf(TFile,"%sASM clobbers tninfo for\n\t",EBO_trace_pfx); Print_TN(ctn,FALSE);fprintf(TFile,"\n");
	  }

	}
      }
      /* Early clobber results which are pre-assigned also kill the value in the assigned register. */
      for (INT resnum = 0; resnum < OP_results(op); resnum++) {
	TN *res_tn = OP_result(op, resnum);
	if (ASM_OP_result_clobber(asm_info)[resnum] &&
	    has_assigned_reg(res_tn)) {
	  REGISTER reg;
	  FOR_ALL_REGISTER_SET_members (TN_registers (res_tn), reg) {
	    TN* ctn = Build_Dedicated_TN (TN_register_class(res_tn), reg,0);
	    set_tn_info(ctn, NULL);
	    
	    if (EBO_Trace_Data_Flow) {
	      fprintf(TFile,"%sASM clobbers tninfo for\n\t",EBO_trace_pfx); Print_TN(ctn,FALSE);fprintf(TFile,"\n");
	    }
	  }
	}
      }
    }
#endif /* TARG_ST */

#ifdef TARG_X8664
    // Remove unnecessary EMMS OPs.  An EMMS is unnecessary if the x87/MMX
    // registers are already in the x87 state (as opposed to MMX state), or if
    // the next OP is an MMX OP (which clobbers the x87 state).
    if (OP_code(op) == TOP_emms) {
      if (in_x87_state)
	OP_Change_To_Noop(op);
      else
	maybe_redundant_EMMS_OP = op;
      in_x87_state = TRUE;
    } else if (OP_x87(op)) {
      in_x87_state = TRUE;
      maybe_redundant_EMMS_OP = NULL;
    } else if (OP_mmx(op)) {
      if (maybe_redundant_EMMS_OP != NULL) {
	OP_Change_To_Noop(maybe_redundant_EMMS_OP);
	maybe_redundant_EMMS_OP = NULL;
      }
      in_x87_state = FALSE;
    }
#endif

    if ((num_opnds == 0) && (OP_results(op) == 0))
#if defined( KEY) && !defined( TARG_ST)
      goto finish;
#else
      continue;
#endif

    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"%sProcess OP\n\t",EBO_trace_pfx); Print_OP_No_SrcLine(op);
    }

   /* Process all the operand TNs. */
    for (opndnum = 0; opndnum < num_opnds; opndnum++) {
      opnd_tn[opndnum] = NULL;
      opnd_tninfo[opndnum] = NULL;
      orig_tninfo[opndnum] = NULL;
    }

    for (opndnum = 0; opndnum < num_opnds; opndnum++) {
      BOOL replace_result = FALSE;
      mUINT8 operand_omega = 0;
       TN *new_tn = NULL;
      EBO_TN_INFO *new_tninfo = NULL;
      TN *tn_replace = NULL;
      EBO_TN_INFO *tninfo_replace = NULL;
      TN *actual_tn = NULL;
      EBO_TN_INFO *actual_tninfo = NULL;
#ifdef TARG_ST
      TN *tn_copy = NULL;
      EBO_TN_INFO *tninfo_copy = NULL;
      // [TTh] On ST side, an operation can have several same_res operands.
      // It is then required to reset the description of associated result
      // for each operand.
      rslt_tn  = NULL;
      rslt_num = 0;
#endif

#ifdef KEY
      BOOL dont_replace = FALSE;      
#endif

      actual_tn = OP_opnd(op, opndnum);
      actual_tninfo = NULL;
      
      tn = OP_opnd(op, opndnum);
      tninfo = NULL;
      tn_replace = NULL;
#ifdef TARG_ST
          opnd_tn[opndnum] = actual_tn;
      opnd_tninfo[opndnum] = actual_tninfo;
      orig_tninfo[opndnum] = actual_tninfo;
#else
      opnd_tn[opndnum] = tn;
      opnd_tninfo[opndnum] = NULL;
      orig_tninfo[opndnum] = NULL;
#endif
      operand_omega = check_omegas ? OP_omega(op,opndnum) : 0;
#ifdef TARG_ST
         if (actual_tn == NULL || TN_is_constant(actual_tn) || TN_is_label(actual_tn)) {
        continue;
      }
#else
      if (tn == NULL || TN_is_constant(tn) || TN_is_label(tn)) {
        continue;
      }
#endif
#if defined( KEY) && !defined(TARG_ST)
      // Don't handle asm OPs where a result may have to be the same as a src
      // operand.  The asm can have multiple such results, but the EBO code
      // expects at most one.
      if (OP_code(op) == TOP_asm) {
	UINT i;
	for (i = 0; i < OP_results(op) && !dont_replace; i ++)	  
	  if (OP_result(op, i) == tn)
	    dont_replace = TRUE;

	// Bug 2408 - asm constraints may force us to use particular registers.
	// To really optimize this case, we have to look at the constraints for
	// each of the operand(s) and result(s) from the ASM WN that generated
	// this asm 'op'.
	ASM_OP_ANNOT *asm_info = (ASM_OP_ANNOT *) OP_MAP_Get(OP_Asm_Map, op);
	for (i = 0; i < OP_opnds(op) && !dont_replace; i ++)
	  if (OP_opnd(op, i) == tn &&
	      // Memory operands don't have special register class requirement.
	      !ASM_OP_opnd_memory(asm_info)[i]) {
	    dont_replace = TRUE;	
	  }
      }

      /* Don't replace a gra homeable gtn; otherwise, gra will do the wrong
	 spilling.  (bug#2913)
      */
      if( !EBO_in_peep     &&
	  !CG_localize_tns &&
	  TN_is_gra_homeable(tn) &&
	  TN_is_global_reg(tn)   &&
	  OP_store( op ) ){
	WN* wn = Get_WN_From_Memory_OP( op );
	if( wn != NULL     &&
	    WN_has_sym(wn) &&
	    /* (Aliased( Alias_Manager, TN_home(tn), wn ) != NOT_ALIASED) */
	    WN_st(wn) == WN_st(TN_home(tn)) ){
	  dont_replace = TRUE;
	}
      }
#endif

#ifdef TARG_MIPS
      if (OP_code(op) == TOP_jalr) {
	dont_replace = TRUE;    // Bug 12505: Don't replace RA_TN in jalr
      }
#endif

#ifdef TARG_ST
         if (actual_tn != True_TN) {
	mUINT8 operand_omega = check_omegas ? OP_omega(op,opndnum) : 0;
        actual_tninfo = tn_info_use (bb, op, actual_tn, op_predicate_tn, op_predicate_tninfo, operand_omega);
        tn_replace = actual_tninfo->replacement_tn;
	tninfo_replace = actual_tninfo->replacement_tninfo;
	tn_copy = actual_tninfo->copy_tn;
      }
#else
      if (tn != True_TN) {
#ifdef TARG_X8664
	TN* tmp_tn = CGTARG_Gen_Dedicated_Subclass_TN( op, opndnum, FALSE );
	if( tmp_tn == NULL )
	  tmp_tn = tn;

        tninfo = tn_info_use ( bb, op, tmp_tn, op_predicate_tn,
			       op_predicate_tninfo, operand_omega );

	// We need to update the tninfo for both TNs if they are different.
	// This is because of cases like the one below:
	// Case I.
	//       TN10      :- copy TN1 # may be an incoming parameter
	//       ...       :- div      # updates TN1 (rax)
	//                 :- TN10     # EBO may propagate the copy TN1
	//
	// Case II.
	//       TN62      :-          # some def
	//       TN63      :-          # some def
	//       TN66      :- sar32 TN62 (0x1f)
	//       TN65 TN64 :- div TN62 TN63 TN66
	// 
	// In case II, the sar32 op may get deleted by EBO because reference 
	// count is not updated for TN62 when processing the div. That is
	// why we need to update the ref count for both TN1 and TN62.

	if (tn != tmp_tn)
	  tninfo = tn_info_use ( bb, op, tn, op_predicate_tn,
				 op_predicate_tninfo, operand_omega );
#else
        tninfo = tn_info_use ( bb, op, tn, op_predicate_tn,
			       op_predicate_tninfo, operand_omega );
#endif

        orig_tninfo[opndnum] = tninfo;
	tn_replace = tninfo->replacement_tn;

#ifdef TARG_X8664
	/* Make sure the low 8-bit part is addressable under -m32
	 */
	if (Is_Target_32bit()  &&
	    tn_replace != NULL &&
	    TN_is_register(tn_replace) &&
	    TN_register(tn_replace) != REGISTER_UNDEFINED &&
	    OP_opnd_size(op, opndnum) == 8) {
	  const REGISTER reg = TN_register(tn_replace);
	  const REGISTER_SET regs =
		 REGISTER_SUBCLASS_members(ISA_REGISTER_SUBCLASS_m32_8bit_regs);
	  Is_True(TN_register_class(tn) == ISA_REGISTER_CLASS_integer,
		  ("Find_BB_TNs: TN not integer register class"));
	  if (!REGISTER_SET_MemberP(regs, reg))
	    tn_replace = NULL;
	}

	if( tn_replace != NULL ){
	  if( EBO_in_peep        &&
	      opndnum == 0       &&
	      OP_x86_style( op ) &&
	      !tn_registers_identical( tn_replace, OP_result(op,0) ) ){
	    tn_replace = NULL;
	  }

	  if( tn_replace != NULL               &&
	      TN_is_register( tn_replace )     &&
	      TN_register( tn_replace ) == RSP &&
	      TOP_Find_Operand_Use( OP_code(op), OU_index ) == opndnum ){
	    tn_replace = NULL;
	  }
	}

	// Before register allocation, do not copy propagate 
	// dedicated TNs.
	// An example:
	//     TN200 :- copy TN8 (%rcx)
	//         ...
	//     TN100 :- shr TN68 TN69
	//     TN201 :- use TN200
	// We can not do copy propagation for the use of TN200.
	// TODO: Look at the live range of the dedicated TNs and see if 
	// there are intervening shift/multiply/divide or any such instructions
	// that need preallocated registers. If there are such instructions, 
	// then we can not do copy propagation.
	if ( tn_replace != NULL &&
	     !EBO_in_peep       &&
	     TN_is_dedicated( tn_replace ) &&
	     TN_register_class(tn_replace) == ISA_REGISTER_CLASS_integer ){
	  const REGISTER reg = TN_register(tn_replace);
	  if( reg == RAX || reg == RCX || reg == RDX ){
	    tn_replace = NULL;
	  }
	}

        // Find cmp merge cands if only neither of the cmp opnds are memops.
        if( Is_Target_32bit() &&
            CG_cmp_load_exec &&
            !EBO_in_peep &&
            !op_is_cmp_merge_cand &&
            do_load_execute &&
            ( num_opnds == 2 ) &&
            ( OP_code(op) == TOP_cmp32 ) && 
            ( tninfo->in_op ) &&
            ( TN_is_register(tn) ) &&
            ( OP_load( tninfo->in_op ) ) &&
            ( tninfo->reference_count == 1 ) &&
            ( OP_code(tninfo->in_op) == TOP_ld32 ) ){
          // If the load cannot be folded into the cmp keep looking.
          // We can only do this step on local loads within the bb,
          // else we rely on the EBO_Load_Flow_Safe.
          if( (tninfo->in_op->bb == bb) &&
              EBO_Not_Load_Exec_Opnd( tninfo->in_op ) ) 
            continue;

          // If the load is from another block apply flow criteria or reject it
          if( !EBO_Load_Flow_Safe( tninfo->in_op, tninfo->in_op->bb, bb ) ) 
            continue;

          // We found a candidate
          cmp_merge_idx = opndnum;
          op_is_cmp_merge_cand = TRUE;
        } else if( Is_Target_64bit() &&
                   CG_cmp_load_exec &&
                   !EBO_in_peep &&
                   !op_is_cmp_merge_cand &&
                   do_load_execute &&
                   ( num_opnds == 2 ) &&
                   ( OP_code(op) == TOP_cmp64 ) && 
                   ( tninfo->in_op ) &&
                   ( TN_is_register(tn) ) &&
                   ( OP_load( tninfo->in_op ) ) &&
                   ( tninfo->reference_count == 1 ) &&
                   ( OP_code(tninfo->in_op) == TOP_ld64 ) ){
          // If the load cannot be folded into the cmp keep looking.
          // We can only do this step on local loads within the bb,
          // else we rely on the EBO_Load_Flow_Safe.
          if( (tninfo->in_op->bb == bb) &&
              EBO_Not_Load_Exec_Opnd( tninfo->in_op ) ) 
            continue;

          // If the load is from another block apply flow criteria or reject it
          if( !EBO_Load_Flow_Safe( tninfo->in_op, tninfo->in_op->bb, bb ) ) 
            continue;

          // We found a candidate
          cmp_merge_idx = opndnum;
          op_is_cmp_merge_cand = TRUE;
        }
#endif

#ifdef KEY
	if( dont_replace ){
	  tn_replace = NULL;
	}
#endif
      }
#endif
#ifdef KEY
      if (!dont_replace)
#endif
#ifdef TARG_ST
          opnd_tninfo[opndnum] = actual_tninfo;
      orig_tninfo[opndnum] = actual_tninfo;
#endif

#ifdef TARG_ST
      /* Arthur: OP_same_res() return what is needed
       *         This logic assumes that only one result matches 
       *         an operand. 
       */
      for (INT i = 0; i < OP_results(op); i++) {
	if (OP_same_res(op, i) == opndnum) {
	  rslt_num = i;
	  replace_result = TRUE;

	  if (EBO_Trace_Data_Flow) {
	    fprintf(TFile, "%sresult %d is the same as opnd %d\n", 
		                    EBO_trace_pfx, rslt_num, opndnum);
	  }
	}
      }
#else
      if (OP_same_res(op)) {
        INT i;
        for (i=0; i<OP_results(op); i++) {
          if (tn_registers_identical(tn, OP_result(op,i))) {
           /* This logic assumes that only one result matches an operand. */
            replace_result = TRUE;
            rslt_num = i;
            break;
          }
        }
      }
#endif

      if ((tn_replace != NULL) &&
          (TN_Is_Constant(tn_replace) ||
#ifdef TARG_ST
           (EBO_tn_available(bb,tninfo_replace) &&
	    Is_Valid_Opnd_Replacement_Candidate(tn_replace, op, opndnum)) ||
#else
           EBO_tn_available(bb,tninfo->replacement_tninfo) ||
#endif
           ((tn_registers_identical(tn, tn_replace)) && !check_omegas)) &&
          (TN_Is_Constant(tn_replace) ||
           ((tninfo->replacement_tninfo != NULL) &&
            (tninfo->replacement_tninfo->in_bb == bb)) ||
           ((has_assigned_reg(tn) == has_assigned_reg(tn_replace)) &&
            (EBO_in_peep || (!BB_reg_alloc(bb) && !TN_is_dedicated(tn_replace))))) ) {
       /* The original TN can be "logically" replaced with another TN. */

        if (EBO_Trace_Data_Flow) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile, "%sin BB:%d propagate value for opnd[%d] ",
                  EBO_trace_pfx, BB_id(OP_bb(op)),opndnum);
          Print_TN(tn, FALSE);
          fprintf(TFile," with ");
          Print_TN(tn_replace, FALSE);
          fprintf(TFile,"\n");
        }

#ifdef TARG_ST
      if(tn_replace == True_TN)  {
        if ((OP_has_predicate(op) && OP_Pred_False(op, OP_find_opnd_use(op, OU_predicate)) && OP_Predicate(op)==actual_tn ) ||
        (OP_Condition(op) && OP_Pred_False(op, OP_find_opnd_use(op, OU_condition)) && OP_Condition(op)==actual_tn )) {
	    if (OP_xfer(op)) {
	        // [TDR] Bug #71342 In case of conditional branche, we need to links 
		Set_OP_opnd(op,OP_find_opnd_use(op, OU_condition),True_TN);
		opnd_tn[opndnum] = True_TN;
		EBO_Resolve_Conditional_Branch(op, opnd_tn);
	    }
            OP_Change_To_Noop(op);
            break;    	  
        }
      }
#endif


        TN *old_tn = tn;
        tn = tninfo->replacement_tn;
        tninfo = tninfo->replacement_tninfo;

#if defined( KEY) && !defined(TARG_ST)
	EBO_REG_ENTRY reginfo;
	BOOL inside_lr = TRUE;
	// opinfo of OP defining the replacement TN
	EBO_OP_INFO *pred_opinfo = locate_opinfo_entry(tninfo);
#endif
#ifdef TARG_ST
	// [CG]: if tn_replace is constant, it will not
	// be actually replaced, so use tn_copy instead if possible
	if (TN_is_constant(tn_replace)) {
	  if (tn_copy != NULL) {
	    FmtAssert(TN_is_register(tn_copy) && tninfo_copy != NULL, 
		      ("Invalid tn_copy in BB %d", BB_id(bb)));
	    /* Ensure:
	     * 1.(available) to ensure validity and 
	     * 2.(same bb or in peep) to not enlarge live ranges
	     */
	    if (EBO_tn_available(bb,tninfo_copy) &&
		(tninfo_copy->in_bb == bb || EBO_in_peep)
		) {
	      tn_replace = tn_copy;
	      tninfo_replace = tninfo_copy;
	    }
	  }
	}
#endif

        if (!TN_is_constant(tn) &&
            // [CG]: unecessary as now TN_has_spill is obsolete

#ifndef TARG_ST
            (!OP_store(op) ||
             (opndnum != TOP_Find_Operand_Use(OP_code(op),OU_storeval)) ||
             !TN_has_spill(old_tn)) &&
#endif
            (!TN_save_reg(tn)) &&
#if defined(KEY) && defined(TARG_MIPS)
	    // 14863: Don't propagate into a conditional move
	    (!replace_result || OP_code(op) == TOP_asm) &&
#endif
            (!replace_result || tn_registers_identical(old_tn, tn_replace)) &&
            (EBO_in_peep ||
             (has_assigned_reg(old_tn) == has_assigned_reg(tn_replace)) ||
             (TN_is_const_reg(tn)) ||
             (!OP_copy(op))) &&
            (EBO_in_peep ||
             !TN_is_gra_homeable(tn_replace) ||
             (tninfo->in_bb == bb)) &&
            (TN_register_class(old_tn) == TN_register_class(tn_replace)) &&
            (!has_assigned_reg(old_tn) ||
             (ISA_OPERAND_VALTYP_Register_Subclass(ISA_OPERAND_INFO_Operand(ISA_OPERAND_Info(OP_code(op)),opndnum)) == ISA_REGISTER_SUBCLASS_UNDEFINED) ||
#ifdef TARG_ST
	    ! (EBO_in_peep && OP_opnd_is_multi(op, opndnum)) &&
	    // Don't allow propagation of non dedicated into a dedicated single register subclass operand
	    (!(!TN_is_dedicated(tn_replace) && TN_is_dedicated(OP_opnd(op, opndnum)) &&
               opndnum < OP_fixed_opnds(op) &&
               OP_opnd_reg_subclass(op, opndnum) != ISA_REGISTER_SUBCLASS_UNDEFINED &&
	       ISA_REGISTER_SUBCLASS_INFO_Count (
                 ISA_REGISTER_SUBCLASS_Info (
                   OP_opnd_reg_subclass(op, opndnum))) == 1)) &&
	    /* Do not replace pre-assigned registers in asm operands. */
	    (!(has_assigned_reg(actual_tn) && OP_code(op) == TOP_asm)) &&
#endif
            (has_assigned_reg(tn_replace) &&
//#ifdef TARG_ST
//              (REGISTER_SET_MemberP(REGISTER_SUBCLASS_members(OP_opnd_reg_subclass(op,opndnum)), TN_register(tn_replace)))
//#else
             (REGISTER_SET_MemberP(REGISTER_SUBCLASS_members(ISA_OPERAND_VALTYP_Register_Subclass(ISA_OPERAND_INFO_Operand(ISA_OPERAND_Info(OP_code(op)),opndnum))), TN_register(tn_replace))))) &&
//#endif
            (TN_size(old_tn) <= TN_size(tn_replace)) &&
            (TN_is_float(old_tn) == TN_is_float(tn_replace)) &&
            (TN_is_fpu_int(old_tn) == TN_is_fpu_int(tn_replace)) &&
            ((OP_results(op) == 0) ||
#ifdef TARG_ST
	     // Arthur: the entire logic escapes me ??
	     !OP_uniq_res(op, 0) ||
#else
             !OP_uniq_res(op) ||
#endif
             !tn_registers_identical(tn, OP_result(op,0)))
#if defined( KEY) && !defined(TARG_ST)
            && (TN_is_global_reg(tn_replace) ||
	    	// OP is inside the live range of the replacement TN?
		(EBO_REG_ENTRY_ptr(reginfo) = hTN_MAP_Get(regs_map,tn_replace),
		 inside_lr =
		   (EBO_REG_ENTRY_last_use_OP_num(reginfo) >= op_num)) ||
		// Register available for CSE?
		(pred_opinfo == NULL ||
		 (pred_opinfo->op_num >=
		    last_fat_point[TN_register_class(tn_replace)])))
#endif
	   ) {
         /* The original TN can be "physically" replaced with another TN. */
         /* Put the new TN in the expression,           */
         /* decrement the use count of the previous TN, */
         /* increment the use count of the new TN.      */

          if (EBO_Trace_Optimization) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile, "%sin BB:%d replace opnd[%d] ",
                    EBO_trace_pfx, BB_id(OP_bb(op)),opndnum);
            Print_TN(OP_opnd(op, opndnum), FALSE);
            fprintf(TFile," with ");
            Print_TN(tn_replace, FALSE);
            if (!TN_Is_Constant(tn_replace) &&
                (tninfo != NULL) &&
                (tninfo->in_bb != NULL)) {
              fprintf(TFile," from BB:%d",BB_id(tninfo->in_bb));
            }
            fprintf(TFile,"\n");
          }

          dec_ref_count(orig_tninfo[opndnum]);
          Set_OP_opnd(op, opndnum, tn);
          if (check_omegas) {
            Set_OP_omega (op, opndnum, (tninfo != NULL) ? tninfo->omega : 0);
          }

          if (tninfo != NULL) {
            inc_ref_count(tninfo);
          }

          if (replace_result) {
           /* This use is also the new result. */
            rslt_tn = tn;
          }

         /* Update information about the actual expression. */
          orig_tninfo[opndnum] = tninfo;

#if defined( KEY) && !defined(TARG_ST)
	  // Extend the live range of the replacement TN if OP is outside of
	  // this live range.
	  if (!inside_lr)
	    Extend_Live_Range(regs_used, last_fat_point, op_num, tninfo,
			      reginfo, bb);
#endif
        } /* replace the operand with another TN. */
      }

      opnd_tn[opndnum] = tn;
      opnd_tninfo[opndnum] = tninfo;
      if (!TN_Is_Constant(tn) &&
          (!op_is_predicated ||
           #ifdef TARG_ST
           /* (cbr) predicate operand # is not necessary constant */
           (opndnum != OP_find_opnd_use(op, OU_predicate))
#else
           (opndnum != OP_PREDICATE_OPND)
#endif
           )) {
        opnds_constant = FALSE;
      }

      if (op_is_predicated && 
#ifdef TARG_ST
           /* (cbr) predicate operand # is not necessary constant */
          (opndnum == OP_find_opnd_use(op, OU_predicate))
#else
          (opndnum == OP_PREDICATE_OPND)
#endif
          ) {
#ifdef TARG_ST
        if (TN_is_zero (opnd_tn[opndnum]) && !OP_xfer(op)) {
#else
        if ((tn == Zero_TN) && !OP_xfer(op)) {
#endif
#ifdef TARG_X8664
	  FmtAssert( false, ("X86 has no Zero_TN") );
#endif
         /* The instruction will not be executed - it can be deleted!
            However, Branch instructions should go through
            Resolve_Conditional_Branch so that links between blocks can be updated. */
#ifdef TARG_ST
           op_replaced = EBO_Fold_Constant_Expression (op, opnd_tn, opnd_tninfo);
#else
          op_replaced = Fold_Constant_Expression (op, opnd_tn, opnd_tninfo);
#endif
  #ifdef TARG_ST
	  // FdF 20090126: This operation is not executed because
	  // predicate is always FALSE. The definitions are replaced
	  // by a KILL if there is no reaching definition.
	  if (op_replaced) {
	    for (int res_idx = 0; res_idx < OP_results(op); res_idx++) {
	      if (!TN_is_constant(OP_result(op,res_idx)) &&
		  !EBO_tn_available(bb, get_tn_info(OP_result(op, res_idx))) &&
		  !tn_has_live_def_into_BB(OP_result(op, res_idx), bb)) {
		TN *kill_tn = OP_result(op, res_idx);
		OP* kill_op = Mk_VarOP(TOP_KILL, 1, 0, &kill_tn, NULL);
		BB_Insert_Op_After(bb, op, kill_op);	 
	      }
	    }
	  }

	  // Do not set num_opnds to the last processed operand,
	  // since there is code that goes through the opnd_tn array
	  // using the OP_opnds(op) value, for example
	  // EBO_Constant_Offset_Propagate.
	  if (op_replaced)
#endif
          num_opnds = opndnum + 1;

          if (EBO_Trace_Optimization) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile, "%sin BB:%d Op can not execute with 0 predicate\n",
                    EBO_trace_pfx, BB_id(OP_bb(op)));
          }
          if (op_replaced) break;
        }

        op_predicate_tn = tn;
        op_predicate_tninfo = tninfo;
      }

    } /* End: Process all the operand TNs. */

#ifdef TARG_X8664
    if (EBO_in_pre && !EBO_in_loop && CG_cmp_load_exec )
      do_load_execute = op_is_cmp_merge_cand;
#endif
#ifdef TARG_ST
    // Arthur: it looks inefficient but really the entire logic
    //         should be replaced. I need the same_res to replace
    //         the old OP_same_res(op)
    BOOL same_res = FALSE;
    for (INT i = 0; i < OP_results(op); i++) {
      if (OP_same_res(op,i) >= 0) {
	same_res = TRUE;
	break;
      }
    }
#endif

    if (OP_memory(op)) {
#ifdef TARG_ST
      if (!op_replaced && same_res)
#else
      if (!op_replaced &&
          OP_same_res(op))
#endif
      {
        op_replaced = EBO_Fix_Same_Res_Op (op, opnd_tn, opnd_tninfo);
      }
#ifdef TARG_ST
      // TDR - Fix for bug #32432
      if (!op_replaced) {
          op_replaced = EBO_Constant_Offset_Propagate(op, opnd_tn, opnd_tninfo);
      }
      if (!op_replaced) {
          op_replaced = EBO_Address_Sequence (op, opnd_tn, opnd_tninfo);
      }
#endif
      if (!op_replaced &&
          !in_delay_slot &&
          OP_store(op)) {
#ifdef TARG_ST
         op_replaced = EBO_Special_Sequence ( op, opnd_tn, orig_tninfo);
#else
        op_replaced = Special_Sequence ( op, opnd_tn, orig_tninfo);
#endif
      }
      if (!op_replaced &&
          no_barriers_encountered) {
        op_replaced = find_duplicate_mem_op (bb, op, opnd_tn, opnd_tninfo,
					     orig_tninfo
#ifndef TARG_ST
                                             , op_num, regs_used,
					     last_fat_point
#endif
                                             );
      }

#ifdef TARG_MIPS //bug 14905
     if(!op_replaced && OP_prefetch(op) && !EBO_in_peep)
      op_replaced = Delete_Redundant_Prefetches(op);
#endif   

#ifdef TARG_X8664
      if (WOPT_Enable_Autoaggstr_Reduction_Threshold > 0 && 
	  LNO_Run_Prefetch != AGGRESSIVE_PREFETCH &&
	  !EBO_in_peep && !op_replaced && OP_prefetch(op)) {
	op_replaced = Delete_Unwanted_Prefetches(op);
      }
      if( !op_replaced    &&
	  do_load_execute &&
	  OP_store(op) ){
	Update_op_must_not_be_moved( op, opnd_tninfo );
      }
#endif
#if defined(TARG_X8664) || defined(TARG_MIPS)
      if( !op_replaced && !in_delay_slot ) {
	op_replaced = EBO_Merge_Memory_Addr( op, opnd_tn, opnd_tninfo, orig_tninfo );
      }
      if ( !op_replaced &&
            (CG_LOOP_unroll_level == 2) &&
            OP_load(op) &&
            EBO_in_loop &&
            BB_unrolled_fully(bb) &&
            (EBO_loop != NULL)) {
        op_replaced = EBO_Opt_Const_Array( op, EBO_loop, EBO_loop_size );
        if (op_replaced)
          scalar_replacement_performed = op_replaced;
      }
#endif
    } else if (OP_effectively_copy(op)) {
      if (!op_replaced &&
          opnds_constant && !in_delay_slot) {
        op_replaced = find_previous_constant(op, opnd_tninfo);
      }
      if (!op_replaced) {
#ifdef TARG_ST
         op_replaced = EBO_Special_Sequence ( op, opnd_tn, orig_tninfo);
#else
        op_replaced = Special_Sequence (op, opnd_tn, opnd_tninfo);
#endif
      }
    } else if ( op_is_cmp_merge_cand &&
                !op_replaced ) {
#ifdef TARG_X8664
      /* we already know the cmp is not a memop and that we have a
       * load feeding one of the src operands. Allow load execute to
       * handle the operation. */
      EBO_flow_safe = TRUE;
      op_replaced = EBO_Load_Execution( op, opnd_tn, orig_tninfo, cmp_merge_idx );
      if( op_replaced ){
        /* we need to cleanup here, as we will not be doing so below. */
        OP_Change_To_Noop(orig_tninfo[cmp_merge_idx]->in_op);
      }
      EBO_flow_safe = FALSE;
#endif
    } else if (!op_replaced &&
               !OP_effectively_copy(op) &&
               !OP_glue(op) 
#ifdef TARG_ST
	       // Side effects checks have been delegated to the functions called below
	       && !OP_has_implicit_interactions(op)
#else
               && !OP_side_effects(op) &&
               !OP_access_reg_bank(op)
#endif
               ) {
      if (!in_delay_slot) {
       /* Can we evaluate the expression at compile time? */
        if (opnds_constant && (num_opnds > 1)) {
          if (OP_xfer(op)) {
           /* If we remove a conditional branch and alter the flow, we
              may have created dead code that could cause later processing
              to get into trouble. This needs to be looked into. */
#ifdef TARG_ST
            op_replaced = EBO_Resolve_Conditional_Branch (op, opnd_tn);
#else
            op_replaced = Resolve_Conditional_Branch (op, opnd_tn);
#endif
            rerun_cflow |= op_replaced;
          } else if (OP_results(op) >= 1) {
#ifdef TARG_ST
            op_replaced = EBO_Fold_Constant_Expression (op, opnd_tn, orig_tninfo);
#else
            op_replaced = Fold_Constant_Expression (op, opnd_tn, opnd_tninfo);
#endif
          }
        } else if (num_opnds > 1) {
          if (OP_results(op) > 0) {
           /* Consider special case optimizations. */
#ifdef TARG_ST
	    // The ST target description takes care of this
            INT o1_idx = OP_find_opnd_use(op, OU_opnd1);
            INT o2_idx = OP_find_opnd_use(op, OU_opnd2);
#else
            INT o2_idx; /* TOP_Find_Operand_Use(OP_code(op),OU_opnd2) won't work for all the cases we care about */
            INT o1_idx; /* TOP_Find_Operand_Use(OP_code(op),OU_opnd1) won't work for all the cases we care about */
            if (op_is_predicated) {
              if (OP_fadd(op) || OP_fsub(op) || OP_fmul(op) || OP_fdiv(op)) {
               /* The first operand is not wnat we are looking for. */
                o1_idx = (num_opnds > 2) ? 2 : -1;
                o2_idx = (num_opnds > 3) ? 3 : -1;
              } else {
                o1_idx = (num_opnds > 1) ? 1 : -1;
                o2_idx = (num_opnds > 2) ? 2 : -1;
              }
            } else {
              o1_idx = (num_opnds > 0) ? 0 : -1;
              o2_idx = (num_opnds > 1) ? 1 : -1;
            }
#endif

#ifdef TARG_ST
            if (same_res) 
#else
            if (OP_same_res(op))
#endif
            {
              op_replaced = EBO_Fix_Same_Res_Op (op, opnd_tn, opnd_tninfo);
            }
            if (o2_idx >= 0) {
              tn = opnd_tn[o2_idx];
              if (!op_replaced &&
                  (tn != NULL) &&
                  TN_Is_Constant(tn) && TN_Has_Value(tn)) {
#ifdef TARG_ST
                op_replaced = EBO_Constant_Operand1 (op, opnd_tn, opnd_tninfo);
#else
                op_replaced = Constant_Operand1 (op, opnd_tn, opnd_tninfo);
#endif
              }
            }
            if (o1_idx >= 0) {
              tn = opnd_tn[o1_idx];
              if (!op_replaced &&
                  (tn != NULL) &&
                  TN_Is_Constant(tn) && TN_Has_Value(tn)) {
#ifdef TARG_ST
                  op_replaced = EBO_Constant_Operand0 (op, opnd_tn, opnd_tninfo);
#else
                  op_replaced = Constant_Operand0 (op, opnd_tn, opnd_tninfo);
#endif
              }
            }
          }
#ifdef TARG_ST
        if (!op_replaced) {
          op_replaced = EBO_Simplify_Special_Compare (op, opnd_tn, orig_tninfo);
        }
        if (!op_replaced) {
          op_replaced = EBO_Simplify_Compare_Sequence (op, opnd_tn, orig_tninfo);
        }
        if (!op_replaced) {
          op_replaced = EBO_Simplify_Compare_MinMaxSequence (op, opnd_tn, orig_tninfo);
        }
#endif
#ifdef TARG_ST
    // TDR - Fix for bug #32432
    if (!op_replaced) {
      op_replaced = EBO_Constant_Offset_Propagate (op, opnd_tn, opnd_tninfo);
    }
	if (!op_replaced) {
	  op_replaced = EBO_Address_Sequence (op, opnd_tn, opnd_tninfo);
	}
#endif
          if (!op_replaced) {
#ifdef TARG_ST
             op_replaced = EBO_Special_Sequence ( op, opnd_tn, orig_tninfo);
#else
            op_replaced = Special_Sequence (op, opnd_tn, orig_tninfo);
#endif
          }
#ifdef TARG_ST
	if (!op_replaced && OP_select(op))
	  op_replaced = EBO_Fix_Select_Same_Args(op, opnd_tn, opnd_tninfo);
#endif
#ifdef TARG_ST
	if (!op_replaced)
	  op_replaced = EBO_Extract_Compose_Sequence(op, opnd_tn, opnd_tninfo);
#endif
        }
#if defined(TARG_X8664) || defined(TARG_MIPS)
	else if (num_opnds == 1) {
	  if (OP_results(op) > 0) {
	    if (!op_replaced) {
	      op_replaced = Special_Sequence (op, opnd_tn, orig_tninfo);
	    }
	  }
	}
#endif
      }

      if (no_barriers_encountered && !op_replaced && !OP_effectively_copy(op)) {
       /* Look for redundant OPs. */
        op_replaced = find_duplicate_op(bb, op, opnd_tn, opnd_tninfo, orig_tninfo);
      }

#ifdef TARG_X8664
      if( do_load_execute  &&
	  !op_replaced     &&
	  !OP_effectively_copy(op) ){
	op_replaced = EBO_Load_Execution( op, opnd_tn, orig_tninfo, cmp_merge_idx );
      }

      if ( !op_replaced &&
            (CG_LOOP_unroll_level == 2) &&
            (OP_code(op) == TOP_leax32) &&
            scalar_replacement_performed &&
            EBO_in_loop &&
            BB_unrolled_fully(bb) &&
            (EBO_loop != NULL)) {
        op_replaced = EBO_Fold_Lea_Const_Component( op );
      }
      if( !op_replaced     &&
	  !OP_effectively_copy(op) ){
	op_replaced = EBO_Lea_Insertion( op, opnd_tn, orig_tninfo );
      }
      if (!op_replaced &&  
	  !OP_effectively_copy(op) ) {
	op_replaced = EBO_Fold_Load_Duplicate( op, opnd_tn, orig_tninfo );
      }
      if (!op_replaced && TOP_is_move_ext( OP_code(op) ))
	op_replaced = Special_Sequence( op, opnd_tn, orig_tninfo );
#endif
    }

#ifdef TARG_X8664
    if (!op_replaced &&
	OP_code(op) == TOP_asm) {
      op_replaced = Special_Asm_Sequence(op, opnd_tn, orig_tninfo);
    }
#endif

    if (op_replaced) {
      if (EBO_Trace_Optimization) {
        fprintf(TFile,"%sin BB:%d remove simplified op - ",EBO_trace_pfx,BB_id(bb));
        Print_OP_No_SrcLine(op);
      }
#if defined( KEY) && !defined(TARG_ST)
      // If the deleted OP is a load, then update the source register usage (in
      // case the src live range(s) ends here) but don't reserve a register for
      // the result.
      // If the deleted OP is an ALU OP being replaced by a load-execute, then
      // update the src and dest as usual.
      Update_Reg_Usage(op, op_num, regs_used, last_fat_point, OP_load(op));

      // If the deleted OP is the only restore for a spilled value, then delete
      // the corresponding spill OP.
      if (OP_load(op)) {
      	ST *spill_loc = CGSPILL_OP_Spill_Location(op);
	if (spill_loc != (ST *)0) {		// It's a spill OP.
	  SPILL_SYM_INFO &info = CGSPILL_Get_Spill_Sym_Info(spill_loc);
	  if (info.Restore_Count() == 1 &&
	      info.Spill_Count() == 1 &&
	      !info.Used_By_Load_Exe()) {	// store required for load-exe
	    OP *spill_op = info.Spill_Op();
	    Is_True(spill_op != NULL, ("Find_BB_TNs: missing spill OP"));
	    OP_Change_To_Noop(spill_op);
	  }
	}
      }
#endif
      remove_uses (num_opnds, orig_tninfo);
      OP_Change_To_Noop(op);
    } else {
     /* Add this OP to the hash table and define all the result TN's. */
#ifdef TARG_ST
      // FdF 20070402
      add_to_hash_table (in_delay_slot, op, opnd_tn, orig_tninfo, opnd_tninfo);
#else
      add_to_hash_table (in_delay_slot, op, orig_tninfo, opnd_tninfo);
#endif

      FmtAssert(((EBO_last_opinfo != NULL) && (EBO_last_opinfo->in_op == op)),
                  ("OP wasn't added to hash table"));

#if defined( KEY) && !defined(TARG_ST)
      EBO_last_opinfo->op_num = op_num;
#endif

     /* Special processing for the result TNs */
      resnum = OP_results(op);
      if (OP_effectively_copy(op) || (resnum && OP_glue(op) && !OP_memory(op))
	  ) {
	/* Propagate copy assignements. */
        INT cix = copy_operand(op);
        TN *tnr = OP_result(op, 0);

        if ((tnr != NULL) && (tnr != True_TN) && (tnr != Zero_TN)) {
          tninfo = EBO_last_opinfo->actual_rslt[0];
#ifdef TARG_ST
	  // [CG 2005/02/28] Don't propagate copy over a dedicated result.
	  // This may lead to replacing a dedicated use into a non dedicated one.
	  if (TN_is_dedicated(tnr)) {
	    if (EBO_Trace_Data_Flow) {
	      #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sCopy not propagated for dedicated result ",EBO_trace_pfx);
              Print_TN(tnr,FALSE);
	      fprintf(TFile,"\n");
	    }
	  } else 
#endif
          if (!OP_glue(op) && (cix >= 0)
#ifdef TARG_ST
              && !(EBO_in_pre && OP_Is_Affirm(op))
#endif
              ) {
            tninfo->replacement_tn = opnd_tn[cix];
            tninfo->replacement_tninfo = opnd_tninfo[cix];
#ifdef TARG_ST
	    if (!TN_is_constant(OP_opnd(op, cix)) &&
		TN_register_class(tnr) == TN_register_class(OP_opnd(op, cix))) {
	      // We propagated a replacement TN, however we're not sure that 
	      // the replacement TN can be a physical replacement, so we keep track
	      // of an alternative reg class copy tn that may be substituted.
	      tninfo->copy_tn = OP_opnd(op, cix);
	      tninfo->copy_tninfo = orig_tninfo[cix];
	    } else {
	      tninfo->copy_tn = NULL;
	      tninfo->copy_tninfo = NULL;
	    }
#endif

#ifdef TARG_X8664
	    /* Do not propagate the constant part here, since most of the time
	       a constant can not serve as a real operand. To enable
	       constant propagation, this constant will be picked up
	       later from opnd_tninfo[].
	    */
	    if( TN_Is_Constant( tninfo->replacement_tn ) &&
		OP_code(op) != TOP_ldc64 &&
		OP_code(op) != TOP_ldc32 ){
	      tninfo->replacement_tn = OP_opnd( op, cix );
	      tninfo->replacement_tninfo = orig_tninfo[cix];
	    }
#endif

            if (EBO_Trace_Data_Flow) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sPropagate Copy of ",EBO_trace_pfx);
              Print_TN(tninfo->replacement_tn,FALSE);
              fprintf(TFile,"[%d] into ",(tninfo->replacement_tninfo != NULL)?tninfo->replacement_tninfo->omega:0);
              Print_TN(tnr,FALSE); fprintf(TFile,"\n");
            }
          }
        }

        if ((resnum == 2) && ((tnr=OP_result(op,1)) != NULL) && (tnr != True_TN)  && (tnr != Zero_TN)) {
         /* This logic must be in sync with what ebo_special calls a "copy".       
            This instruction must actually be placing a "FALSE" condition in a predicate. */
          tninfo = EBO_last_opinfo->actual_rslt[1];
          tninfo->replacement_tn = Zero_TN;
#ifdef TARG_X8664
	  FmtAssert( false, ("X86 does not have Zero_TN") );
#endif

          if (EBO_Trace_Data_Flow) {
            #pragma mips_frequency_hint NEVER
            fprintf(TFile,"%sPropagate Copy of ",EBO_trace_pfx);
            Print_TN(tninfo->replacement_tn,FALSE); fprintf(TFile," into ");
            Print_TN(tnr,FALSE); fprintf(TFile,"\n");
          }
        }
#ifdef TARG_ST
      } else if (rslt_tn == NULL) {
	TN *replacement_tn;
	if ((replacement_tn = EBO_literal_replacement_tn(op)) != NULL) {
	  TN *tnr = OP_result(op, 0);
	  if ((tnr != NULL) && (tnr != True_TN) && (tnr != Zero_TN)) {
	    EBO_TN_INFO *tninfo = EBO_last_opinfo->actual_rslt[0];
	    tninfo->replacement_tn = replacement_tn;
	    tninfo->replacement_tninfo = NULL;
	    // FdF 20090827: Also, set the rematerialization flag if
	    // not already.
	    if (!TN_is_rematerializable(tnr)) {
	      if (TN_register_class(tnr) == CGTARG_Register_Class_For_Mtype(MTYPE_I4))
		CGSPILL_Attach_Intconst_Remat(tnr, MTYPE_I4, TN_value(replacement_tn));
	      else if (TN_register_class(tnr) == CGTARG_Register_Class_For_Mtype(MTYPE_I8))
		CGSPILL_Attach_Intconst_Remat(tnr, MTYPE_I8, TN_value(replacement_tn));
	      else if (TN_register_class(tnr) == CGTARG_Register_Class_For_Mtype(MTYPE_B))
		CGSPILL_Attach_Intconst_Remat(tnr, MTYPE_B, TN_value(replacement_tn));
	    }
	    if (EBO_Trace_Data_Flow) {
		#pragma mips_frequency_hint NEVER
	      fprintf(TFile,"%sPropagate Replacement TN ",EBO_trace_pfx);
	      Print_TN(tninfo->replacement_tn,FALSE);
	      fprintf(TFile," into ");
	      Print_TN(tnr,FALSE); fprintf(TFile,"\n");
	    }
	  }
	}
#endif

      }else if (rslt_tn != NULL) {
#if defined( KEY) && !defined(TARG_ST)
        if (op->opr == TOP_asm && TN_is_constant(rslt_tn))
	  ;
	else {
#endif
       /* A result tn needs to be replaced. */
        TN *tnr = OP_result(op, rslt_num);
        tninfo = EBO_last_opinfo->actual_rslt[rslt_num];

       /* This is subtle - yes we do want the replacement_tninfo
          entry to point to the tninfo entry we just created. Yes,
          it does create a circular link in the chain. Code that
          searches the chain will need to be aware of this. */
        tninfo->replacement_tn = rslt_tn;
        tninfo->replacement_tninfo = tninfo;
        Set_OP_result (op, rslt_num, rslt_tn);

        if (EBO_Trace_Data_Flow) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sReplace result[%d] tn ",EBO_trace_pfx,rslt_num);
          Print_TN(tnr,FALSE); fprintf(TFile," with ");
          Print_TN(rslt_tn, FALSE); fprintf(TFile,"\n");
        }
#if defined( KEY) && !defined(TARG_ST)
        }
#endif

      }
#ifdef TARG_ST
    if ((op_replaced || no_barriers_encountered) && BB_scheduled(bb))  Reset_BB_scheduled(bb);
#endif
    }

    if (PROC_has_branch_delay_slot()) in_delay_slot = OP_xfer(op);

#if defined( KEY) && !defined(TARG_ST)
finish:

#ifdef TARG_X8664
    // If OP is a memory OP that references a spilled LRA value, mark it so.
    // Later we will delete dead LRA spills.
    if (EBO_in_peep &&
	!op_replaced) {
      Mark_LRA_Spill_Reference(op, &LRA_spilled_value_is_used, &MEM_local_pool);
    }
#endif

    // Update the register usage.  Identify the next OP with register usage
    // info.
    Is_True(op_num <= op_count, ("Find_BB_TNs: wrong op_num"));
    if (op == op_with_reg_usage_info) {
      op_with_reg_usage_info = next_op_with_reg_usage_info;
      if (!op_replaced)
	Update_Reg_Usage(op, op_num, regs_used, last_fat_point, FALSE);
    }
#endif
  }

#ifdef TARG_X8664
  // Remove dead LRA stores.  A LRA store becomes dead when all the restores
  // are eliminated by EBO, which can happen when the spilled register is
  // unused between the store and the load, which can happen when intervening
  // instructions are changed from load/use to load-exe.
  if (EBO_in_peep) {
    FOR_ALL_BB_OPs (bb, op) {
      Delete_Dead_LRA_Spill(op, &LRA_spilled_value_is_used);
    }
  }
#endif

  return no_barriers_encountered;
}
#ifdef TARG_ST
extern BOOL EBO_Special_Inline_Immediates(OP *op, EBO_OP_INFO *opinfo, int idx);

static
void EBO_Inline_Immediates (BB *bb, BOOL BB_completely_processed)
{
  EBO_OP_INFO *opinfo;
  
  /* 
     Only do it at main phase.
     if-conversion and unrolling phases augment
     the use count of immediates.
     As we inline if the use count is low, we have to
     wait for these phases to complete.
  */
  if (!EBO_in_main && !EBO_in_peep) return;
  if (!BB_completely_processed) return;

  if (EBO_first_opinfo == NULL) return;
  
  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter EBO_Inline_Immediate in BB:%d\n",EBO_trace_pfx,BB_id(bb));
    tn_info_table_dump();
  }

  /* Walk bottom up over the opinfo entires for this bb. */
  for (opinfo = EBO_last_opinfo; opinfo != NULL; opinfo = opinfo->prior) {
    OP *op = opinfo->in_op;
    
    if (op == NULL) continue;

    if (OP_bb(op) != bb) {
      if (EBO_Trace_Block_Flow) {
	#pragma mips_frequency_hint NEVER
        fprintf(TFile,"Stop looking for ops, prior op is in BB:%d\n",
                OP_bb(op) ? BB_id(OP_bb(op)) : -1);
        Print_OP_No_SrcLine(op);
      }
      break;  /* get out of loop over opinfo entries. */
    }

    /* If op result is a replacement tn, we check if
       we can remove it, as EBO_Special_Inline_Immediate
       may have decreased its ref count. */
    if (OP_results(op) == 1 &&
	opinfo->actual_rslt[0] != NULL &&
	opinfo->actual_rslt[0]->replacement_tn != NULL &&	
	TN_is_constant(opinfo->actual_rslt[0]->replacement_tn)) {
      if (EBO_Trace_Data_Flow) {
        #pragma mips_frequency_hint NEVER
	fprintf(TFile,"%sConsider removing immediate definition OP:\n\t",
		EBO_trace_pfx);
	Print_OP_No_SrcLine(op);
      }
      EBO_TN_INFO *tninfo = opinfo->actual_rslt[0];
      TN *tn = tninfo->local_tn;
      if (tninfo->in_bb == bb &&
	  tninfo->in_op == op &&
	  tninfo->reference_count == 0 &&
	  !op_is_needed_globally(op) &&
#ifdef TARG_ST
          /* (cbr) definition must be preserved.
             next write might not redefine this op if predicated */
          !opinfo->op_must_not_be_removed &&
#endif
	  BB_completely_processed &&
	  !(!tninfo->redefined_before_block_end &&
	    TN_live_out_of(tn, tninfo->in_bb))) {

	if (EBO_Trace_Data_Flow) {
	  #pragma mips_frequency_hint NEVER
	  fprintf(TFile,"%sRemoved unused OP:\n\t",
		  EBO_trace_pfx);
	  Print_OP_No_SrcLine(op);
	}
	
	remove_op (opinfo);
	
	if (opinfo->in_delay_slot) {
	  OP_Change_To_Noop(op);
	} else {
	  BB_Remove_Op(bb, op);
	}
	opinfo->in_op = NULL;
	opinfo->in_bb = NULL;

	/* Propagate "reaches block end" information. */
	if (tninfo->redefined_before_block_end &&
	    (tninfo->same != NULL) &&
	    (tninfo->same->in_bb == bb)) {
	  EBO_TN_INFO *next_tninfo = tninfo->same;
	  next_tninfo->redefined_before_block_end = TRUE;
	}
	
	/* If op was removed, we skip the rest of the loop. */
	continue;
      }
    } 
    
    /* This part tries to inline immediate operands into operations. */
    {
      INT idx;
      /* Not sure we can transform such operations.*/
      if (OP_glue(op) || OP_no_move_before_gra(op) ||
	  OP_code(op) == TOP_spadjust) {
	continue;
      }
      
      /* Try to inline immediate for each operand of the operation. */
      for (idx = 0; idx < OP_opnds(op); idx++) {
	EBO_TN_INFO *tninfo;
	TN *replacement_tn;
	
	tninfo = opinfo->actual_opnd[idx];
	if (tninfo == NULL || tninfo->replacement_tn == NULL) continue;
	replacement_tn = tninfo->replacement_tn;
	if (!TN_is_constant(replacement_tn)) continue;
	if (EBO_Trace_Data_Flow) {
          #pragma mips_frequency_hint NEVER
	  fprintf(TFile,"%sConsider inlining immediate into op:opnd %d:%d\n",
		  EBO_trace_pfx, OP_map_idx(op), idx);
	  fprintf(TFile," tninfo op %d:",
		  OP_map_idx(tninfo->in_op));
	  tn_info_entry_dump (tninfo);
	}
	if (op_is_needed_globally(tninfo->in_op)) {
	  if (EBO_Trace_Data_Flow) {
	    #pragma mips_frequency_hint NEVER
	    fprintf(TFile," not inlined: needed globally\n");
	  }
	  continue;
	}
	
	if (EBO_Special_Inline_Immediates(op, opinfo, idx)) {
	  dec_ref_count(tninfo);
	  break;
	} else {
	  if (EBO_Trace_Data_Flow) {
	    #pragma mips_frequency_hint NEVER
	    fprintf(TFile," not inlined: special inline failed\n");
	  }
	}
      }
    }
  }
}

/* -----------------------------------------------------------------------
 * Backward pass to removed useless copy operations
 * Must be called last in the processing as tninfo of
 * uses are not updated. More preciselly, if a copy tn1<-tn2 
 * the uses of tn1 are not updated, so the corresponding tninfo point
 * to the removed op instead of the defining op of tn2.
 * Note that this pass was done in the EBO_Remove_Unused_Ops functions,
 * as now the EBO_Inline_Immediate pass is done after EBO_REmove_Unused_Ops
 * and relies on coherent tninfos, this pass as been separeted.
 * -----------------------------------------------------------------------
 */
static
void EBO_Remove_Copy_Ops (BB *bb, BOOL BB_completely_processed)
{
  EBO_OP_INFO *opinfo;
  EBO_TN_INFO *tninfo;
  TN *tn;

  if (EBO_first_opinfo == NULL) return;

  /* Only done after reg alloc. */
  if (!EBO_in_peep) return;

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter EBO_Remove_Copy_Ops in BB:%d\n",EBO_trace_pfx,BB_id(bb));
    tn_info_table_dump();
  }

  for (opinfo = EBO_last_opinfo; opinfo != NULL; opinfo = opinfo->prior) {
    INT rslt_count = 0;
    INT idx, opnd_idx;
    OP *op = opinfo->in_op;

    if (op == NULL) continue;

    if (OP_bb(op) != bb) {
      if (EBO_Trace_Block_Flow) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"Stop looking for copy ops, next op is in BB:%d\n",
                OP_bb(op) ? BB_id(OP_bb(op)) : -1);
        Print_OP_No_SrcLine(op);
      }
      break;  /* get out of  loop over opinfo entries. */
    }
    
    rslt_count = OP_results(op);
    if (rslt_count == 0) continue;
    if (op_is_needed_globally(op)) continue;

    /* Copies to and from the same register are not needed. */
    if (OP_effectively_copy(op) &&
	(idx = OP_Copy_Result(op)) >= 0 &&
	(tninfo = opinfo->actual_rslt[idx]) != NULL &&
	tninfo->in_bb == bb &&
	tninfo->in_op != NULL &&	
	(tn = tninfo->local_tn) != NULL &&
	has_assigned_reg(tn) &&
	(opnd_idx = copy_operand(op)) >= 0 &&
	has_assigned_reg(OP_opnd(op,opnd_idx)) &&
	tn_registers_identical(tn, OP_opnd(op,opnd_idx))) {
      
      if (EBO_Trace_Data_Flow) {
	#pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sTry to remove definition of copy entry[%d] ",
                EBO_trace_pfx,tninfo->sequence_num);
        Print_TN(tn,FALSE);
        fprintf(TFile,"\n");
      }
      /* We may be able to get rid of the copy, but be
	 sure that the TN is marked live into this block. */
      if ((opinfo->actual_opnd[opnd_idx] != NULL) &&
	  (bb != opinfo->actual_opnd[opnd_idx]->in_bb)) {
	mark_tn_live_into_BB (tn, bb, opinfo->actual_opnd[opnd_idx]->in_bb);
      }
      
      /* Propagate use count for this TN to it's input TN. */
      if (tninfo->same != NULL) {
	tninfo->same->reference_count += tninfo->reference_count;
      }

      if (!tninfo->redefined_before_block_end &&
	  (tninfo->same != NULL) &&
	  (tninfo->same->in_bb == bb)) {
	/* Removing the copy causes the previous definition
	   of the TN (or reg) to reach the end of the block. */
	tninfo->same->redefined_before_block_end = FALSE;
      }

      /* FdF 20050217: Propagate the dedicated property to the source
	 of the copy (bug 1-6-0-B/30) */
      if (TN_is_dedicated(tn))
	Set_TN_is_dedicated(OP_opnd(op,opnd_idx));

      remove_op (opinfo);

      if (EBO_Trace_Optimization) {
	#pragma mips_frequency_hint NEVER
	fprintf(TFile, "%sin BB:%d removing copy    ",
              EBO_trace_pfx, BB_id(bb));
	Print_OP_No_SrcLine(op);
      }

      if (opinfo->in_delay_slot) {
	OP_Change_To_Noop(op);
      } else {
	BB_Remove_Op(bb, op);
      }
      opinfo->in_op = NULL;
      opinfo->in_bb = NULL;
      
    } /* end: removable copy */
    
  } /* end: for each opinfo entry */
  
}

/*
 * Make a quick scan of the OPS in a BB and remove noops. 
 * Note that it was done in EBO_Remove_Unused_Ops, it is now separated.
 */
static void 
EBO_Remove_Noops(BB *bb)
{
  OP *op;
  OP * next_op = NULL;
  in_delay_slot = FALSE;
#ifdef TARG_ST
  // Arthur: return the simulated OP anyway
  TOP noop_top = TOP_noop;
#else
  TOP noop_top = CGTARG_Noop_Top();
#endif
  
  for (op = BB_first_op(bb); op != NULL; op = next_op) {
    next_op = OP_next(op);
    if (   (OP_code(op) == noop_top || OP_code(op) == TOP_noop) 
	   && !in_delay_slot)
      {
        if (EBO_Trace_Optimization) {
	  #pragma mips_frequency_hint NEVER
          fprintf(TFile, "%sin BB:%d removing noop    ",
                  EBO_trace_pfx, BB_id(bb));
          Print_OP_No_SrcLine(op);
        }
	
        BB_Remove_Op(bb, op);
      } else if (PROC_has_branch_delay_slot()) {
	if (in_delay_slot && OP_code(op) == TOP_noop) {
	  // ugly hack for mips
	  OP_Change_Opcode(op, noop_top);
	}
        in_delay_slot = OP_xfer(op);
      }
  }
}


static void 
EBO_Replace_Memmove(BB *bb)
{
  OP *op;
  OP * next_op = NULL;
  if (!CG_safe_memmove) return;
  for (op = BB_first_op(bb); op != NULL; op = next_op) {
    next_op = OP_next(op);
    if(OP_call(op) || OP_jump(op) || OP_ijump(op)) {
        ANNOTATION *ant = ANNOT_Get (BB_annotations(bb), ANNOT_CALLINFO);
        if (ant != NULL) {
            ST *call_sym = CALLINFO_call_st(ANNOT_callinfo(ant));
            if (call_sym != NULL) {
                if (!strcmp(ST_name(call_sym),"memmove")) {
                    // With a memmove, we should check dependency between pointers
                    // If there is no overlap => can use memcpy instead
                    //   - If pointer are not on sam type
                    //   - If pointers come from malloc without modif
                    //   - Restricted pointer
                    ST *newst = New_ST(GLOBAL_SYMTAB);
                    ST_Init(newst, Save_Str("memcpy"), ST_sym_class(call_sym), ST_sclass(call_sym), ST_export(call_sym), (TY_IDX)ST_pu_type(call_sym));
                    Set_ST_pu(newst,ST_pu(call_sym));
                    CALLINFO_call_st(ANNOT_callinfo(ant)) = newst;
                    TN *new_tn = Gen_Symbol_TN(newst, TN_offset(OP_opnd(op,1)), TN_relocs(OP_opnd(op,1)));
                    Set_OP_opnd(op,1,new_tn);
                }
            }      
        }
    }
    if (EBO_Trace_Optimization) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile, "%sin BB:%d replace memmove ", EBO_trace_pfx, BB_id(bb));
      Print_OP_No_SrcLine(op);
    }
  }
}

#endif

#ifdef KEY
static int removed_ops = 0;
#endif
static
void EBO_Remove_Unused_Ops (BB *bb, BOOL BB_completely_processed)
/* -----------------------------------------------------------------------
 * -----------------------------------------------------------------------
 */
{
  EBO_OP_INFO *opinfo;
  EBO_TN_INFO *tninfo;
  TN *tn;

  if (EBO_first_opinfo == NULL) return;

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter EBO_Remove_Unused_Ops in BB:%d\n",EBO_trace_pfx,BB_id(bb));
    tn_info_table_dump();
  }

  for (opinfo = EBO_last_opinfo; opinfo != NULL; opinfo = opinfo->prior) {
    INT rslt_count = 0;
    INT idx;
    OP *op = opinfo->in_op;

#ifndef KEY
    if (op == NULL) continue;
#else
    // If op was replaced, then op->bb could be NULL. Just skip this op.
    if (op == NULL || op->bb == NULL) continue;

    // Don't delete OP in the branch delay slot if EBO is run after delay slot
    // filling.
    if (!EBO_can_delete_branch_delay_OP &&
	PROC_has_branch_delay_slot() &&
	BB_last_op(bb) == op &&
	OP_prev(op) != NULL &&
	OP_xfer(OP_prev(op))) {
      goto op_is_needed;
    }
#endif

    if (OP_bb(op) != bb) {
      if (EBO_Trace_Block_Flow) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"Stop looking for unused ops, next op is in BB:%d\n",
                OP_bb(op) ? BB_id(OP_bb(op)) : -1);
        Print_OP_No_SrcLine(op);
      }
      break;  /* get out of  loop over opinfo entries. */
    }

    if (EBO_Trace_Data_Flow) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile,"%sConsider removing OP:\n\t",
              EBO_trace_pfx);
      Print_OP_No_SrcLine(op);
    }

#ifdef TARG_ST
    // [TTh] Remove instructions marked with !True_TN
    if (OP_has_predicate(op)) {
      INT idx = OP_find_opnd_use(op, OU_predicate);
      if (OP_opnd(op, idx) == True_TN && OP_Pred_False(op, idx)) {
	goto can_be_removed;
      }
    }
#endif
    rslt_count = OP_results(op);
    if (rslt_count == 0) goto op_is_needed;
    if (op_is_needed_globally(op)) goto op_is_needed;

#ifdef TARG_ST
    // [CG]: skip early implicit interactions
    if (OP_has_implicit_interactions(op))
      goto op_is_needed;
#endif

   /* Check that all the result operands can be safely removed. */
    for (idx = 0; idx < rslt_count; idx++) {
      tninfo = opinfo->actual_rslt[idx];

     /* A couple of safety checks. */
      if (tninfo == NULL) continue;
      if (tninfo->in_bb != bb) goto op_is_needed;
      if (tninfo->in_op == NULL) goto op_is_needed;
      tn = tninfo->local_tn;

      if (EBO_Trace_Data_Flow) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sTry to remove definition of entry[%d] ",
                EBO_trace_pfx,tninfo->sequence_num);
        Print_TN(tn,FALSE);
        fprintf(TFile,"\n");
      }

     /* Zero_TN or True_TN for a result is a no-op. */
      FmtAssert( tn != Zero_TN, ("NYI") );
#ifdef TARG_ST
      if (TN_is_zero (tn)) continue;
#else
      if (tn == Zero_TN) continue;
#endif
      if (tn == True_TN) continue;

#ifndef TARG_ST
     /* Copies to and from the same register are not needed. */
      if (EBO_in_peep &&
          OP_effectively_copy(op) &&
          has_assigned_reg(tn) &&
          (copy_operand(op) >= 0) &&
          has_assigned_reg(OP_opnd(op,copy_operand(op))) &&
          (tn_registers_identical(tn, OP_opnd(op,copy_operand(op))))) {
        INT cpo = copy_operand(op);

       /* We may be able to get rid of the copy, but be
          sure that the TN is marked live into this block. */
        if ((opinfo->actual_opnd[cpo] != NULL) &&
            (bb != opinfo->actual_opnd[cpo]->in_bb)) {
          mark_tn_live_into_BB (tn, bb, opinfo->actual_opnd[cpo]->in_bb);
        }

       /* Propagate use count for this TN to it's input TN. */
        if (tninfo->same != NULL) {
          tninfo->same->reference_count += tninfo->reference_count;
        }

        if (!tninfo->redefined_before_block_end &&
            (tninfo->same != NULL) &&
            (tninfo->same->in_bb == bb)) {
         /* Removing the copy causes the previous definition
            of the TN (or reg) to reach the end of the block. */
          tninfo->same->redefined_before_block_end = FALSE;
        }
        goto can_be_removed;
      }
#endif

     /* There must be no direct references to the TN. */
      if (!BB_completely_processed) goto op_is_needed;
      if (tninfo->reference_count != 0) goto op_is_needed;
      if (OP_has_implicit_interactions(tninfo->in_op))
	goto op_is_needed;

     /* Check for indirect and global references.   */
      if (!tninfo->redefined_before_block_end &&
          TN_live_out_of(tn, tninfo->in_bb)) goto op_is_needed;

      if (TN_is_pfs_reg(tn)) goto op_is_needed;
      if (TN_is_lc_reg(tn)) goto op_is_needed;
      if (TN_is_ec_reg(tn)) goto op_is_needed;
    }

   /* None of the results are needed. */
    if (opinfo->op_must_not_be_removed) goto op_is_needed;
    if (OP_store(op)) goto op_is_needed;

#ifdef TARG_X8664
    if( TOP_is_change_rflags( OP_code(op) ) ){
      for( OP* next = OP_next(op); next != NULL; next = OP_next(next) ){
	if( OP_reads_rflags( next) )
	  goto op_is_needed;
	if( TOP_is_change_rflags( OP_code(next) ) )
	  break;
      }
    }
#endif 

can_be_removed:

    remove_op (opinfo);

    if (EBO_Trace_Optimization) {
      #pragma mips_frequency_hint NEVER
      fprintf(TFile, "%sin BB:%d removing    ",
              EBO_trace_pfx, BB_id(bb));
      Print_OP_No_SrcLine(op);
    }

    if (opinfo->in_delay_slot) {
      OP_Change_To_Noop(op);
    } else {
#ifdef KEY
      // If deleting the first OP in the BB, make sure all predecessor BBs
      // branch to the correct OP.
      if (BB_first_op(bb) == op &&
	  PROC_has_branch_delay_slot()) {
	EBO_Adjust_Pred_Branch_Target(bb);
      }
#endif
      BB_Remove_Op(bb, op);
    }
    opinfo->in_op = NULL;
    opinfo->in_bb = NULL;

   /* Propagate "reaches block end" information. */
    for (idx = 0; idx < rslt_count; idx++) {
      tninfo = opinfo->actual_rslt[idx];
      if ((tninfo != NULL) &&
          tninfo->redefined_before_block_end &&
          (tninfo->same != NULL) &&
          (tninfo->same->in_bb == bb)) {
        EBO_TN_INFO *next_tninfo = tninfo->same;
        next_tninfo->redefined_before_block_end = TRUE;
      }
    }

#ifdef KEY
    if (EBO_in_peep)
      removed_ops ++;
#endif
    continue;

op_is_needed:

   /* Predicate resolution may not have been possible, and */
   /* preceeding definitions of the same result TN's may   */
   /* need to be saved.                                    */
    for (idx = 0; idx < rslt_count; idx++) {
      tninfo = opinfo->actual_rslt[idx];
      if ((tninfo != NULL) &&
          (tninfo->local_tn != NULL) &&
          (tninfo->same != NULL)) {
        EBO_TN_INFO *next_tninfo = tninfo->same;

        while (next_tninfo != NULL) {
          if ((next_tninfo->in_op != NULL) &&
#ifdef TARG_ST
              // (cbr)
              (!EBO_predicate_dominates((tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                        OP_PredOnFalse(tninfo->in_op),
                                        tninfo->predicate_tninfo,
                                        (next_tninfo->predicate_tninfo != NULL)?
                                        next_tninfo->predicate_tninfo->local_tn:True_TN,
                                        OP_PredOnFalse(next_tninfo->in_op),
                                        next_tninfo->predicate_tninfo)) &&
              (!EBO_predicate_complements((tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                          OP_PredOnFalse(tninfo->in_op),
                                          tninfo->predicate_tninfo,
                                          (next_tninfo->predicate_tninfo != NULL)?
                                               next_tninfo->predicate_tninfo->local_tn:True_TN,
                                          OP_PredOnFalse(next_tninfo->in_op),
                                          next_tninfo->predicate_tninfo))) {
#else
              (!EBO_predicate_dominates((tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                         tninfo->predicate_tninfo,
                                         (next_tninfo->predicate_tninfo != NULL)?
                                               next_tninfo->predicate_tninfo->local_tn:True_TN,
                                         next_tninfo->predicate_tninfo)) &&
              (!EBO_predicate_complements((tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                          tninfo->predicate_tninfo,
                                          (next_tninfo->predicate_tninfo != NULL)?
                                               next_tninfo->predicate_tninfo->local_tn:True_TN,
                                          next_tninfo->predicate_tninfo))){
#endif

           /* A store into an unresolved predicate is a potential problem     */
           /* because the last store might not completely redefine the first. */
           /* The predicates could be completely independant.  But we         */
           /* don't know how to check for that, currently.                    */

           /* Stop searching and preserve the preceeding definition. */
            EBO_OP_INFO *opinfo = locate_opinfo_entry(next_tninfo);
            if (opinfo != NULL) {
              opinfo->op_must_not_be_removed = TRUE;
            } else {
             /* Couldn't find the opinfo entry.  Make sure that the TN has
                a use count so that the defining OP entry will not be deleted. */
              next_tninfo->reference_count += tninfo->reference_count;
            }

            if (EBO_Trace_Data_Flow) {
              #pragma mips_frequency_hint NEVER
              fprintf(TFile,"%sMark result same_tn as needed - original [%d]: ",
                      EBO_trace_pfx,tninfo->sequence_num);
              Print_TN(tninfo->local_tn,FALSE);
              fprintf(TFile," same as [%d]: ",next_tninfo->sequence_num);
              Print_TN(next_tninfo->local_tn,FALSE);
              fprintf(TFile,"\n");
            }
#ifdef TARG_ST
            if (EBO_predicate_dominates((next_tninfo->predicate_tninfo != NULL)?next_tninfo->predicate_tninfo->local_tn:True_TN,
                                        OP_PredOnFalse(next_tninfo->in_op),
                                        next_tninfo->predicate_tninfo,
                                        (tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                        OP_PredOnFalse(tninfo->in_op),
                                        tninfo->predicate_tninfo)) {
#else
            if (EBO_predicate_dominates((next_tninfo->predicate_tninfo != NULL)?
                                               next_tninfo->predicate_tninfo->local_tn:True_TN,
                                         next_tninfo->predicate_tninfo,
                                        (tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                         tninfo->predicate_tninfo)) {
#endif
              /* If the first store dominates the last, there can be no       */
              /* preceeding definitions that are partially redefined by       */
              /* the last store.  We can stop searching for other dominators. */
              break;
            }
          }
          next_tninfo = next_tninfo->same;
        }

      }
    }

   /* Predicate resolution may not have been possible, and */
   /* preceeding inputs to this instruction may be defined */
   /* within the current extended block, and must be saved.*/
    for (idx = 0; idx < OP_opnds(op); idx++) {
      tninfo = opinfo->actual_opnd[idx];
      if ((tninfo != NULL) &&
          (tninfo->local_tn != NULL) &&
          (tninfo->same != NULL)) {
        EBO_TN_INFO *next_tninfo = tninfo->same;

#ifdef TARG_ST
	// FdF 20081113, Codex 52447: There is a specific case that is
	// not handled by this code:
	//
        //   P? x =  // uncond def
	//      z = P ? x : y
	//    Where the PQS knows that P is always FALSE
	//
	// The use of x refers to a DUMMY TNINFO, since there is no
	// definition with a predicate including the predicate on the
	// use. This TNINFO links to the TNINFO for the real
	// definition of x. But since the predicate on the real
	// definition (FALSE) is disjoint with the predicate on the
	// use (TRUE), the definition for x is not marked with
	// op_must_not_be_removed, and its reference_count is 0. The
	// definition for x will thus be removed. To avoid a reference
	// to an undefined variable, the use of x is replaced by a
	// dummy variable, since there is no reaching definition here.

	BOOL operand_has_real_def = (tninfo->in_op != NULL);
#endif

        while (next_tninfo != NULL) {
          if ((next_tninfo->in_op != NULL) &&
              (next_tninfo->omega == tninfo->omega)) {
#ifdef TARG_ST
            if (EBO_predicate_dominates((next_tninfo->predicate_tninfo != NULL)?
                                        next_tninfo->predicate_tninfo->local_tn:True_TN,
                                        next_tninfo->in_op ? OP_PredOnFalse(next_tninfo->in_op) : false, 
                                        next_tninfo->predicate_tninfo,
                                        (tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                        tninfo->in_op ? OP_PredOnFalse(tninfo->in_op) : false, 
                                        tninfo->predicate_tninfo)) {
	      // FdF 20081113: Mark that one of the defining operation
	      // for this operand is reaching.
	      operand_has_real_def = TRUE;
              /* This predicate dominates the OP we need to save. It's   */
              /* use count should be sufficiant to cause it to be saved. */
              /* We can stop searching for other dominators.             */
              break;
            } else if (EBO_predicate_complements((next_tninfo->predicate_tninfo != NULL)?next_tninfo->predicate_tninfo->local_tn:True_TN,
                                                 next_tninfo->in_op ? OP_PredOnFalse(next_tninfo->in_op) : false, 
                                                 next_tninfo->predicate_tninfo,
                                                 (tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                                 tninfo->in_op ? OP_PredOnFalse(tninfo->in_op) : false, 
                                                 tninfo->predicate_tninfo)) {
#else
            if (EBO_predicate_dominates((next_tninfo->predicate_tninfo != NULL)?
                                               next_tninfo->predicate_tninfo->local_tn:True_TN,
                                         next_tninfo->predicate_tninfo,
                                         (tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                         tninfo->predicate_tninfo)) {
              /* This predicate dominates the OP we need to save. It's   */
              /* use count should be sufficiant to cause it to be saved. */
              /* We can stop searching for other dominators.             */
              break;
            } else if (EBO_predicate_complements((next_tninfo->predicate_tninfo != NULL)?
                                               next_tninfo->predicate_tninfo->local_tn:True_TN,
                                         next_tninfo->predicate_tninfo,
                                         (tninfo->predicate_tninfo != NULL)?tninfo->predicate_tninfo->local_tn:True_TN,
                                         tninfo->predicate_tninfo)) {
#endif
              /* These predicates are mutually exclusive.  There is no   */
              /* need to mark it as used, but se need to keep looking.   */
            } else {
             /* A store into an unresolved predicate is a potential problem. */
             /* The predicates could be completely independant.  But we      */
             /* don't know how to check for that, currently.      */

#ifdef TARG_ST
	      // FdF 20081113: Mark that one of the defining operation for this
	      // operand is reaching.
	      operand_has_real_def = TRUE;
#endif
             /* Stop searching and preserve the preceeding definition. */
              EBO_OP_INFO *opinfo = locate_opinfo_entry(next_tninfo);
              if (opinfo != NULL) {
                opinfo->op_must_not_be_removed = TRUE;
              } else {
               /* Couldn't find the opinfo entry.  Make sure that the TN has
                  a use count so that the defining OP entry will not be deleted. */
                next_tninfo->reference_count += tninfo->reference_count;
              }

             /* Check for block spanning values and make sure they are marked global. */
              if ((next_tninfo != NULL) &&
                  (bb != next_tninfo->in_bb)) {
                mark_tn_live_into_BB (tninfo->local_tn, bb, next_tninfo->in_bb);
              }

              if (EBO_Trace_Data_Flow) {
                #pragma mips_frequency_hint NEVER
                fprintf(TFile,"%sMark operand same_tn as needed - original [%d]: ",
                        EBO_trace_pfx,tninfo->sequence_num);
                Print_TN(tninfo->local_tn,FALSE);
                fprintf(TFile," same as [%d]: ",next_tninfo->sequence_num);
                Print_TN(next_tninfo->local_tn,FALSE);
                fprintf(TFile,"\n");
              }

            }
          }
          next_tninfo = next_tninfo->same;
        }
#ifdef TARG_ST
	// FdF 20081113: If there is no reaching definition, replace
	// the use by a dummy operand.
	if (!operand_has_real_def &&
	    !tn_has_live_def_into_BB(OP_opnd(op, idx), tninfo->in_bb)) {
	  if ((Zero_TN != NULL) &&
	      (TN_register_class(Zero_TN) == TN_register_class(OP_opnd(op, idx)))) {
	    Set_OP_opnd(op, idx, Zero_TN);
	  }
	  else {
	    TN *dup_tn = Dup_TN(OP_opnd(op, idx));
	    Set_OP_opnd(op, idx, dup_tn);
	    OP* kill_op = Mk_VarOP(TOP_KILL, 1, 0, &dup_tn, NULL);
	    BB_Insert_Op_Before(bb, op, kill_op);	 
	  }
	}
#endif
      }
    }


   /* Check for newly created references that cross a block
      boundary.  If one is found, mark it as global and carry
      registers in/out of blocks. */
    for (idx = 0; idx < OP_opnds(op); idx++) {
      tninfo = opinfo->actual_opnd[idx];
      if ((tninfo != NULL) &&
          (bb != tninfo->in_bb)) {
        mark_tn_live_into_BB (tninfo->local_tn, bb, tninfo->in_bb);
      }
    }

  } /* end: for each opinfo entry */

#ifndef TARG_ST
 /* [CG]: Now done in EBO_Remove_Noops. */
 /* Make a quick scan of the OPS in a BB and remove noops. */
  {
    OP *op;
    OP * next_op = NULL;
    in_delay_slot = FALSE;
#ifdef TARG_ST
    // Arthur: return the simulated OP anyway
    TOP noop_top = TOP_noop;
#else
    TOP noop_top = CGTARG_Noop_Top();
#endif

    for (op = BB_first_op(bb); op != NULL; op = next_op) {
      next_op = OP_next(op);
      if ((OP_code(op) == noop_top || OP_code(op) == TOP_noop)
	  && !in_delay_slot)
      {
        if (EBO_Trace_Optimization) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile, "%sin BB:%d removing noop    ",
                  EBO_trace_pfx, BB_id(bb));
          Print_OP_No_SrcLine(op);
        }
#ifdef KEY
	// If deleting the first OP in the BB, make sure all predecessor BBs
	// branch to the correct OP.
	if (BB_first_op(bb) == op &&
	    PROC_has_branch_delay_slot()) {
	  EBO_Adjust_Pred_Branch_Target(bb);
	}
#endif
        BB_Remove_Op(bb, op);
      } else if (PROC_has_branch_delay_slot()) {
	if (in_delay_slot && OP_code(op) == TOP_noop) {
	   // ugly hack for mips
	   OP_Change_Opcode(op, noop_top);
	}
        in_delay_slot = OP_xfer(op);
      }
    }

  }
#endif

  return;
}
 
#ifdef TARG_ST
/* ===================================================================== *
 * Some Target independent optimizations.
 * ===================================================================== */

/*
 * EBO_Extract_Compose_Sequence
 *
 * Perform some copy optimizations on across compose/extract and
 * extract/compose sequences.
 */
static BOOL
EBO_Extract_Compose_Sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  if (OP_compose(op)) {
    /* Find defining extract and verify that operands/results match. */
    EBO_OP_INFO *def_opinfo = NULL;
    for (INT i = 0; i < OP_opnds(op); i++) {
      EBO_TN_INFO *tninfo = opnd_tninfo[i];
      EBO_OP_INFO *opinfo = locate_opinfo_entry(tninfo);
      if (opinfo == NULL || opinfo->in_op == NULL) return FALSE;
      if (def_opinfo == NULL) def_opinfo = opinfo;
      if (!OP_extract(def_opinfo->in_op)) return FALSE;
      if (def_opinfo->actual_rslt[i] != tninfo) return FALSE;
    }
    if (def_opinfo == NULL) return FALSE;
    EBO_TN_INFO *tninfo = def_opinfo->actual_opnd[0];
    if (tninfo == NULL) return FALSE;
    if (!EBO_tn_available(OP_bb(op), tninfo)) return FALSE;
    /* Reconfigurability: compose/extract must operate on same sub-TN count */
    if (OP_results(def_opinfo->in_op) != OP_opnds(op)) return FALSE;
    OPS ops = OPS_EMPTY;
    EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), OP_result(op, 0), tninfo->local_tn, &ops);
    BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
    if (EBO_Trace_Optimization) {
#pragma mips_frequency_hint NEVER
      fprintf(TFile,"Extract_Compose_Sequence - replaced extract/compose by widemove\n");
    }
    return TRUE;
  }
  if (OP_extract(op)) {
    /* Find defining compose. */
    EBO_TN_INFO *tninfo = opnd_tninfo[0];
    EBO_OP_INFO *def_opinfo = locate_opinfo_entry(tninfo);
    if (def_opinfo == NULL || def_opinfo->in_op == NULL) return FALSE;
    if (!OP_compose(def_opinfo->in_op)) return FALSE;
    /* Reconfigurability: compose/extract must operate on same sub-TN count */
    if (OP_opnds(def_opinfo->in_op) != OP_results(op)) return FALSE;
    OPS ops = OPS_EMPTY;
    for (INT i = 0; i < OP_results(op); i++) {
      EBO_TN_INFO *tninfo = def_opinfo->actual_opnd[i];
      if (!EBO_tn_available(OP_bb(op), tninfo)) return FALSE;
      // Suppress the expansion into a series of copies if a copy needs to
      // read a value that has been overwritten by an earlier copy in the
      // sequence.
      OP *prev_copy;
      FOR_ALL_OPS_OPs_FWD (&ops, prev_copy) {
	if ((has_assigned_reg (tninfo->local_tn) &&
	     OP_Defs_Regs (prev_copy, TN_register_class(tninfo->local_tn),
			   TN_register (tninfo->local_tn),
			   TN_nhardregs (tninfo->local_tn)))
	    || OP_Defs_TN (prev_copy, tninfo->local_tn)) {
	  return FALSE;
	}
      }
      EBO_Exp_COPY(OP_Predicate(op), OP_PredOnFalse(op), OP_cond_def(op), OP_result(op, i), tninfo->local_tn, &ops);
    }
    BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
    if (EBO_Trace_Optimization) {
#pragma mips_frequency_hint NEVER
      fprintf(TFile,"Extract_Compose_Sequence - replaced compose/extract by copies\n");
    }
    return TRUE;
  }
  return FALSE;
}
#endif

  
/* ===================================================================== */

  
/* 
 */
static
void
EBO_Add_BB_to_EB (BB * bb)
{
  EBO_TN_INFO *save_last_tninfo = EBO_last_tninfo;
  EBO_OP_INFO *save_last_opinfo = EBO_last_opinfo;
  BBLIST *succ_list;
  BOOL normal_conditions;
#if defined( KEY) && !defined(TARG_ST)
  hTN_MAP save_regs_map = regs_map;
#endif

  if (EBO_Trace_Execution) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEnter EBO_Add_BB_to_EB BB:%d. It has %d successors\n",
            EBO_trace_pfx,BB_id(bb),BB_succs(bb)?BB_succs_len(bb):0);
  }

  Set_BB_visited(bb);

  if (   CG_skip_local_ebo 
      && (   (BB_id(bb) < CG_local_skip_before) 
          || (BB_id(bb) > CG_local_skip_after)  
          || (BB_id(bb) == CG_local_skip_equal)
         )
     ) 
  {
    DevWarn ("Skipping EBO for BB=%d", BB_id (bb));
    
    return;
  }

 /* Add this block to the current Extended Block (EB). */
  normal_conditions = Find_BB_TNs (bb);
  if (normal_conditions &&
#ifdef KEY
      !EBO_optimize_single_BB &&
#endif
      !CG_localize_tns) {
   /* Walk through the successors, trying to grow the EB. */
    FOR_ALL_BB_SUCCS(bb, succ_list) { 
      BB *succ = BBLIST_item(succ_list);

      if (EBO_Trace_Block_Flow) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sCheck successor BB:%d. It has %d preds and has%s been visited.\n",
               EBO_trace_pfx,BB_id(succ),BB_preds_len(succ),BB_visited(succ)?" ":" not");
      }

      if (!BB_call(bb) &&
#ifdef KEY
	  !BB_asm(bb)  && 	  /* bug#1777 Don't cross asm bb. */
#endif
          (BB_preds_len(succ) == 1) &&
          !BB_visited(succ) &&
          (BB_rid(bb) == BB_rid(succ))) {
        EBO_Add_BB_to_EB (succ);
      }
    }
  }

 /* When we are unable to grow the EB any more, optimize what we have. */
  if (EBO_Trace_Block_Flow) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sEBO optimization at BB:%d\n",EBO_trace_pfx,BB_id(bb));
  }

#ifdef TARG_ST
  // [CG]: Update live out information that may have change due to redundancy 
  // elimination. See comments in  EBO_Update_Liveout().
  EBO_Update_Liveout(bb);
#endif

#ifdef TARG_ST
  /* [CG]: Now done in four passes:
   * 1. remove dead code preserving tninfo->opinfo (use->def) links
   * 2. try to inline long immediate definitions.
   * 3. remove useless copies
   * 4. remove inserted or existing noops.
   */
  EBO_Remove_Unused_Ops(bb, normal_conditions);
  EBO_Inline_Immediates(bb, normal_conditions);
  EBO_Remove_Copy_Ops (bb, normal_conditions);
  EBO_Remove_Noops(bb);
  EBO_Replace_Memmove(bb);
#else
  EBO_Remove_Unused_Ops(bb, normal_conditions);
#endif

#ifdef TARG_ST
  // [CG]: Update live in information
  EBO_Update_Livein(bb);
#endif

 /* Remove information about TN's and OP's in this block. */
  backup_tninfo_list(save_last_tninfo);
  backup_opinfo_list(save_last_opinfo);
#if defined( KEY) && !defined(TARG_ST)
  regs_map = save_regs_map;
#endif

  return;
}
  
  
/* 
 * Perform EBO
 */
static
void
EBO_Process ( BB *first_bb )
{
  BB *bb;

  rerun_cflow = FALSE;

  EBO_Trace_Execution    = Get_Trace(TP_EBO, 0x001);
  EBO_Trace_Optimization = Get_Trace(TP_EBO, 0x002);
  EBO_Trace_Block_Flow   = Get_Trace(TP_EBO, 0x004);
  EBO_Trace_Data_Flow    = Get_Trace(TP_EBO, 0x008);
  EBO_Trace_Hash_Search  = Get_Trace(TP_EBO, 0x010);

  FmtAssert(((EBO_first_tninfo == NULL) && (EBO_first_opinfo == NULL)),
                  ("Initial pointers not NULL %o %o",EBO_first_tninfo,EBO_first_opinfo));

  EBO_Start();

  if (EBO_Trace_Data_Flow || EBO_Trace_Optimization) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,">>>> Before EBO");
    if (EBO_in_peep) {
      fprintf(TFile," - in peep ");
    } else if (EBO_in_before_unrolling) {
      fprintf(TFile," - before unrolling ");
    } else if (EBO_in_after_unrolling) {
      fprintf(TFile," - after unrolling ");
    } else if (EBO_in_pre) {
      fprintf(TFile," - preprocessing ");
    } else {
      fprintf(TFile," - main ");
    }
    fprintf(TFile,"<<<<\n");

    if (EBO_Trace_Data_Flow) {
      Print_All_BBs ();
    }
  }

 /* TEMPORARY - EBO doesn't understand rotating registers, so skip blocks that use them. */
  for (bb = first_bb; bb != NULL; bb = BB_next(bb)) {
    if (BB_rotating_kernel(bb)) Set_BB_visited(bb);
    #ifdef KEY
      if (BB_swp_kernel(bb)) Set_BB_visited(bb);
    #endif
  }

#ifdef TARG_MIPS
  if (EBO_in_peep) // Can do this once after all transformations
    Redundancy_Elimination();
#endif
  for (bb = first_bb; bb != NULL; bb = BB_next(bb)) {
    RID *bbrid;
    if (( bbrid = BB_rid( bb )) &&
        ( RID_level( bbrid ) >= RL_CGSCHED ) ) {
     /*
      * There is no overlap in TN's between the current REGION
      * and REGIONs which have already been through CG
      */
      continue;
    }
    if (!BB_visited(bb)) {
      EBO_Add_BB_to_EB (bb);
#ifdef TARG_X8664
      if (EBO_in_loop) {
        if (CG_LOOP_unroll_level == 2) {
          if (Is_Multibb_Loop(EBO_loop) == FALSE)
            break;
        } else {
          break;
        }
      }
#else
      if (EBO_in_loop) break;
#endif
    }
  }

 /* Clear the bb flag, in case some other phase uses it. */
  clear_bb_flag (first_bb);

  if (rerun_cflow) {
#ifdef TARG_ST
    CFLOW_Optimize(CFLOW_BRANCH | CFLOW_UNREACHABLE, "CFLOW (from ebo)", !EBO_in_peep);
#else
    CFLOW_Optimize(CFLOW_BRANCH | CFLOW_UNREACHABLE, "CFLOW (from ebo)");
#endif
  }

  EBO_Finish();

  if (EBO_Trace_Data_Flow) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,">>>> After EBO");
    if (EBO_in_peep) {
      fprintf(TFile," - in peep ");
    } else if (EBO_in_before_unrolling) {
      fprintf(TFile," - before unrolling ");
    } else if (EBO_in_after_unrolling) {
      fprintf(TFile," - after unrolling ");
    } else if (EBO_in_pre) {
      fprintf(TFile," - preprocessing ");
    } else {
      fprintf(TFile," - main ");
    }
    fprintf(TFile,"<<<<\n");
    fprintf(TFile,">>>> %d tninfo entries allocated and reused %d times\n",
    EBO_num_tninfo_entries,EBO_tninfo_entries_reused);
    fprintf(TFile,">>>> %d opinfo entries allocated and reused %d times\n",
    EBO_num_opinfo_entries,EBO_opinfo_entries_reused);
    Print_All_BBs ();
  }

}

/* =======================================================================
 * =======================================================================
 *
 * There are a number of different entry points to EBO.  They differ because
 * the specific information available changes throughout the compilation.
 * The flags that are set at each entry are used during EBO if a use is to
 * be made of information that is only available at certain times.
 *
 * =======================================================================
 * =======================================================================
 */

/* 
 * perform EB optimizations right after instruction translation.
 */
void
EBO_Pre_Process_Region ( RID *rid )
{
  BB *first_bb = (rid) ? CGRIN_first_bb(RID_cginfo( rid )) : REGION_First_BB;

  EBO_in_pre  = TRUE;
  EBO_in_before_unrolling = FALSE;
  EBO_in_after_unrolling = FALSE;
  EBO_in_peep = FALSE;

  EBO_in_loop = FALSE;

#ifdef KEY
  EBO_can_delete_branch_delay_OP = TRUE;
  EBO_no_liveness_info_available = FALSE;
  EBO_optimize_single_BB = FALSE;
#endif

  if ((EBO_Opt_Level < 5) && ((EBO_Opt_Level > 0) || (EBO_Opt_Level != -5))) return;

  clear_bb_flag (first_bb);
  EBO_Process (first_bb);
}

  
/* 
 * perform EBO optimizations during unrolling and pipelining
 */
void
EBO_before_unrolling(BB_REGION *bbr )
{
  INT i;
  EBO_in_pre  = FALSE;
  EBO_in_before_unrolling = TRUE;
  EBO_in_after_unrolling = FALSE;
  EBO_in_peep = FALSE;

  EBO_in_loop = TRUE;

#ifdef KEY
  EBO_can_delete_branch_delay_OP = TRUE;
  EBO_no_liveness_info_available = FALSE;
  EBO_optimize_single_BB = FALSE;
#endif

  if ((EBO_Opt_Level < 4) && ((EBO_Opt_Level > 0) || (EBO_Opt_Level != -4))) return;

#if defined(TARG_IA64) || defined(KEY)
  for (i = 0; i < bbr->entries.size(); i++) {
    clear_bb_flag (bbr->entries[i]);
  }
  for (i = 0; i < bbr->exits.size(); i++) {
    Set_BB_visited (bbr->exits[i]);
  }
  EBO_Process (bbr->entries[0]);
#endif
}

  
/* 
 * perform EBO optimizations after unrolling and pipelining
 */
void
EBO_after_unrolling(BB_REGION *bbr, LOOP_DESCR *loop, int loop_iter_size)
{
  INT i;

  EBO_in_pre  = FALSE;
  EBO_in_before_unrolling = FALSE;
  EBO_in_after_unrolling = TRUE;
  EBO_loop = loop;
  EBO_loop_size = loop_iter_size;
  EBO_in_peep = FALSE;

  EBO_in_loop = TRUE;

#ifdef KEY
  EBO_can_delete_branch_delay_OP = TRUE;
  EBO_no_liveness_info_available = FALSE;
  EBO_optimize_single_BB = FALSE;
#endif

  if ((EBO_Opt_Level < 3) && ((EBO_Opt_Level > 0) || (EBO_Opt_Level != -3))) return;

#if defined(TARG_IA64) || defined(KEY)
  for (i = 0; i < bbr->entries.size(); i++) {
    clear_bb_flag (bbr->entries[i]);
  }
  for (i = 0; i < bbr->exits.size(); i++) {
    Set_BB_visited (bbr->exits[i]);
  }
  EBO_Process (bbr->entries[0]);
#endif

  EBO_loop = NULL;
}

  
/* 
 * perform EB optimizations on a region
 */
void
EBO_Process_Region ( RID *rid )
{
  BB *first_bb = (rid) ? CGRIN_first_bb(RID_cginfo( rid )) : REGION_First_BB;

  EBO_in_pre  = FALSE;
  EBO_in_before_unrolling = FALSE;
  EBO_in_after_unrolling = FALSE;
  EBO_in_peep = FALSE;

  EBO_in_loop = FALSE;

#ifdef KEY
  EBO_can_delete_branch_delay_OP = TRUE;
  EBO_no_liveness_info_available = FALSE;
  EBO_optimize_single_BB = FALSE;
#endif

  if ((EBO_Opt_Level < 2) && ((EBO_Opt_Level > 0) || (EBO_Opt_Level != -2))) return;

  clear_bb_flag (first_bb);
  EBO_Process (first_bb);
}


/* 
 * perform EB optimizations after register assignment.
 */
void
EBO_Post_Process_Region ( RID *rid )
{
  BB *first_bb = (rid) ? CGRIN_first_bb(RID_cginfo( rid )) : REGION_First_BB;

  EBO_in_pre  = FALSE;
  EBO_in_before_unrolling = FALSE;
  EBO_in_after_unrolling = FALSE;
  EBO_in_peep = TRUE;

  EBO_in_loop = FALSE;

#ifdef KEY
  EBO_can_delete_branch_delay_OP = TRUE;
  EBO_no_liveness_info_available = FALSE;
  EBO_optimize_single_BB = FALSE;
#endif

  if ((EBO_Opt_Level < 1) && ((EBO_Opt_Level >= 0) || (EBO_Opt_Level != -1))) return;

  /* compute live-in sets for physical registers */
  MEM_POOL_Push(&MEM_local_pool);
  REG_LIVE_Analyze_Region();

  clear_bb_flag (first_bb);
#ifdef KEY
  removed_ops = 0;
#endif
  EBO_Process (first_bb);
  REG_LIVE_Finish();
#ifdef KEY
  // We need to recreate liveness info and run EBO again to delete op
  // that is useless after previous EBO run.
  // Also, we need to run reg life analyze after copy propagation.
  //if (removed_ops)
  {
    REG_LIVE_Analyze_Region();
    clear_bb_flag (first_bb);
    EBO_Process (first_bb);
    REG_LIVE_Finish();
  }
#endif
  MEM_POOL_Pop(&MEM_local_pool);
}


#if defined( KEY) && !defined(TARG_ST)
// Run EBO after second instruction scheduling phase (after register
// allocation).
void
EBO_Post_Process_Region_2 ( RID *rid )
{
  BB *first_bb = (rid) ? CGRIN_first_bb(RID_cginfo( rid )) : REGION_First_BB;

  EBO_in_pre  = FALSE;
  EBO_in_before_unrolling = FALSE;
  EBO_in_after_unrolling = FALSE;
  EBO_in_peep = TRUE;

  EBO_in_loop = FALSE;

  EBO_can_delete_branch_delay_OP = FALSE;
  EBO_no_liveness_info_available = TRUE;
  EBO_optimize_single_BB = TRUE;

  if ((EBO_Opt_Level < 6) && ((EBO_Opt_Level >= 0) || (EBO_Opt_Level != -6)))
    return;

  /* compute live-in sets for physical registers */
  MEM_POOL_Push(&MEM_local_pool);
  REG_LIVE_Analyze_Region();

  clear_bb_flag (first_bb);
  removed_ops = 0;
  EBO_Process (first_bb);
  REG_LIVE_Finish();

  MEM_POOL_Pop(&MEM_local_pool);
}

#ifdef TARG_X8664
// If OP references a symbol called lra_spill_temp_xxx, return xxx; else return
// -1.
static int
Get_LRA_Spill_Temp_Number(OP *op)
{
  int base_idx, offset_idx;

  base_idx = OP_find_opnd_use(op, OU_base);
  if (base_idx >= 0 &&
      OP_opnd(op, base_idx) == SP_TN) {
    offset_idx = OP_find_opnd_use(op, OU_offset);
    if (offset_idx >= 0) {
      TN *offset_tn = OP_opnd(op, offset_idx);
      if (offset_tn != NULL &&
	  TN_is_symbol(offset_tn)) {
	ST *st = TN_var(offset_tn);
	char *name = ST_name(st);
	if (strstr(name, "lra_spill_temp_") != NULL) {
	  char *p = name + 15;	// get the number part
	  int id = atoi(p);
	  return id;
	}
      }
    }
  }
  return -1;
}

static void
Init_Remove_Dead_LRA_Stores(BS **bs, MEM_POOL *pool)
{
  // Indexed by xxx in lra_spill_temp_xxx.  Set to 1 if the corrsponding
  // spilled value is used.
  *bs = BS_Create_Empty(Spill_Var_Cnt + 1, pool);
}

static BOOL
Delete_Dead_LRA_Spill(OP *op, BS **bs)
{
  if (!OP_store(op))
    return FALSE;

  int id = Get_LRA_Spill_Temp_Number(op);

  // Delete store if the stored value was never used.
  if (id > 0 &&
      !BS_MemberP(*bs, id)) {
    OP_Change_To_Noop(op);
    return TRUE;
  }
  return FALSE;
}

// If a memory OP loads from lra_spill_temp_xxx, then mark the spilled value
// used.  Delete stores whose spilled value is never used.
static void
Mark_LRA_Spill_Reference(OP *op, BS **bs, MEM_POOL *pool)
{
  if (OP_store(op))
    return;

  // The spilled value is used.  Mark it so.
  int id = Get_LRA_Spill_Temp_Number(op);
  if (id > 0) {
    *bs = BS_Union1D(*bs, id, pool);
  }
}
#endif	// TARG_X8664

#if 0
// We can not say if a particular address will be taken outside of the 
// current extended basic block unless we look at the Symbol Table. 
// Currently, this support is not provided. But, this routine does some 
// useful processing and may be useful at a later point.
// Before a "store" op leaves the opinfo table,
// look for any dependent op and if none exist, then delete
// this "store" op
void
delete_useless_store_op (EBO_OP_INFO *opinfo)
{
  EBO_TN_INFO *save_last_tninfo = EBO_last_tninfo;

  BOOL found_dependent_op = FALSE;

  OP *store_op = opinfo->in_op;

  /* Determine operand TN info */
  BB *bb = opinfo->in_bb; 
  EBO_TN_INFO *tninfo;
  TN *op_predicate_tn = NULL;
  EBO_TN_INFO *op_predicate_tninfo = NULL;
  BOOL check_omegas = FALSE;
  INT max_opnds = OP_MAX_FIXED_OPNDS;
  EBO_TN_INFO **opnd_tninfo = TYPE_ALLOCA_N(EBO_TN_INFO *, max_opnds);
  INT opndnum;
  mUINT8 operand_omega;
  check_omegas = (EBO_in_loop && _CG_LOOP_info(store_op))?TRUE:FALSE;
  TN *tn;
  for (opndnum = 0; opndnum < OP_opnds(store_op); opndnum ++) {
    tn = OP_opnd(store_op, opndnum);
    if (tn == NULL || TN_is_constant(tn) || TN_is_label(tn)) {
      opnd_tninfo[opndnum] = NULL;
      continue;
    }
    operand_omega = check_omegas ? OP_omega(store_op,opndnum) : 0;
    tninfo = tn_info_use (bb, store_op, tn,
			  op_predicate_tn, op_predicate_tninfo,
			  operand_omega);
    opnd_tninfo[opndnum] = tninfo;
  }

  /* Determine the indexes of the address components of this "store" op. */
  INT store_base_idx = TOP_Find_Operand_Use(OP_code(store_op),OU_base);
  INT store_offset_idx = TOP_Find_Operand_Use(OP_code(store_op),OU_offset);

  /* Determine the address components of this "store" op. */
  TN *store_base_tn = 
    (store_base_idx >= 0) ? OP_opnd(store_op, store_base_idx) : NULL;
  EBO_TN_INFO *store_base_tninfo = 
    (store_base_idx >= 0) ? opnd_tninfo[store_base_idx] : NULL;
  TN *store_offset_tn = 
    (store_offset_idx >= 0) ? OP_opnd(store_op, store_offset_idx) : NULL;
  EBO_TN_INFO *store_offset_tninfo = 
    (store_offset_idx >= 0) ? opnd_tninfo[store_offset_idx] : NULL;

  if ((store_offset_tn == NULL) && (store_base_tn != NULL)) {
    find_index_and_offset(store_base_tninfo,
                          &store_base_tn, &store_base_tninfo,
                          &store_offset_tn, &store_offset_tninfo);
  }

  for (; bb != NULL; bb = BB_next(bb)) {
    OP *op;
    FOR_ALL_BB_OPs (bb, op) {
      if (!OP_load(op))
        continue;	
      check_omegas = (EBO_in_loop && _CG_LOOP_info(op))?TRUE:FALSE;
      TN *tn;
      for (opndnum = 0; opndnum < OP_opnds(op); opndnum ++) {
	tn = OP_opnd(op, opndnum);
	if (tn == NULL || TN_is_constant(tn) || TN_is_label(tn)) {
	  opnd_tninfo[opndnum] = NULL;
	  continue;
	}
    	operand_omega = check_omegas ? OP_omega(op,opndnum) : 0;
        tninfo = tn_info_use (bb, op, tn,
                              op_predicate_tn, op_predicate_tninfo,
                              operand_omega);
        opnd_tninfo[opndnum] = tninfo;
      }
      if (EBO_hash_op(op, opnd_tninfo) == opinfo->hash_index) {
	INT base_idx = 0;
	INT offset_idx = 0;
	TN *base_tn = NULL;
	EBO_TN_INFO *base_tninfo = NULL;
	TN *offset_tn = NULL;
	EBO_TN_INFO *offset_tninfo = NULL;

	/* Determine the address components of this "load" op. */
	base_idx = TOP_Find_Operand_Use(OP_code(op),OU_base);
	offset_idx = TOP_Find_Operand_Use(OP_code(op),OU_offset);
	base_tn = (base_idx >= 0) ? OP_opnd(op,base_idx) : NULL;
	base_tninfo = (base_idx >= 0) ? opnd_tninfo[base_idx] : NULL;
	offset_tn = (offset_idx >= 0) ? OP_opnd(op,offset_idx) : NULL;
	offset_tninfo = (offset_idx >= 0) ? opnd_tninfo[offset_idx] : NULL;

	if ((offset_tn == NULL) && (base_tn != NULL)) {
	  find_index_and_offset(base_tninfo,
				&base_tn, &base_tninfo,
				&offset_tn, &offset_tninfo);
	}

	BOOL hash_op_matches = ((base_tn == store_base_tn) &&           /* The base  index must match */
				(base_tninfo == store_base_tninfo) &&   /* The base   info must match */
				(offset_tninfo == store_offset_tninfo)) /* The offset info must match */
	  ? TRUE : FALSE;
	if (!hash_op_matches) // bases do not match
	  continue;
	BOOL op_is_subset = FALSE;
	BOOL offsets_may_overlap = TRUE;
	
 	if (offset_tn == store_offset_tn) {
	  // can not delete this "store" op
	  found_dependent_op = TRUE;
	  break;
	}
	
	/* The offset tn's need to be looked at in more detail. */	
	ST *symbol = ((offset_tn != NULL) && TN_is_symbol(offset_tn)) ?TN_var(offset_tn) : NULL;
	ST *symbol_store = ((store_offset_tn != NULL) && TN_is_symbol(store_offset_tn)) ?TN_var(store_offset_tn) : NULL;
	mUINT8 relocs = (offset_tn != NULL) ? TN_relocs(offset_tn) : 0;
	mUINT8 relocs_store = (store_offset_tn != NULL) ? TN_relocs(store_offset_tn) : 0;
	INT64 offset = 0;
	INT64 offset_store = 0;
	offset = (offset_tn != NULL) ? TN_offset(offset_tn) : 0;
	offset_store = (store_offset_tn != NULL) ? TN_offset(store_offset_tn) : 0;

	/* This time, the relocations must be the same. */
	hash_op_matches = (symbol == symbol_store) && (relocs == relocs_store);
	if (!hash_op_matches) // relocations do not match
	  continue;

	/* If the relocations are the same, we need to examine the offsets and sizes. */
	INT size = CGTARG_Mem_Ref_Bytes(op);
	INT size_store = CGTARG_Mem_Ref_Bytes(store_op);
	
	if ((offset == offset_store) &&
	    (size == size_store)) {
	  /* The perfect match: location and size. */
	  // can not delete
	  found_dependent_op = TRUE;
	  break;
	} else if ((offset >= offset_store) &&
		    ((offset + size) <= (offset_store + size_store))) {
	  /* The current reference is a subset of the preceeding one. */
	  // can not delete
	  found_dependent_op = TRUE;
	  break;
	} else if (((offset_store + size_store) <= offset) ||
		   ((offset + size) <= offset_store)) {
	  /* There is no potential overlap. */
	} else {
	  /* Any other case may be a potential conflict. */
	  hash_op_matches = FALSE;
	}
      }
    }
    if (found_dependent_op)
      break; 
  }
  if (found_dependent_op == FALSE) {
    OP_Change_To_Noop(store_op);
    printf("store op %#x does not have any dep op\n", store_op);
  }
  backup_tninfo_list(save_last_tninfo);
  return;
}
#endif

// For each of BB's predecessors, if it branches to any OP other than the first
// OP in BB, retarget it to branch to one OP earlier.
static void
EBO_Adjust_Pred_Branch_Target (BB *bb)
{
  BB *fall_thru_pred = BB_Fall_Thru_Predecessor(bb);
  BBLIST *edge;

  FOR_ALL_BB_PREDS(bb, edge) {
    BB *pred = BBLIST_item(edge);
    if (pred != fall_thru_pred) {
      OP *xfer_op = BB_last_op(pred);
      if (!OP_xfer(xfer_op))
	xfer_op = OP_prev(xfer_op);
      Is_True(xfer_op != NULL && OP_xfer(xfer_op),
	      ("EBO_Adjust_Pred_Branch_Target: cannot find xfer_op"));
      for (int i = 0; i < OP_opnds(xfer_op); i++) {
	TN *opnd_tn = OP_opnd(xfer_op, i);
	if (TN_is_label(opnd_tn) &&
	    TN_offset(opnd_tn) > 0) {
	  TN *new_label_tn = Gen_Adjusted_TN(opnd_tn, -4);
	  Set_OP_opnd(xfer_op, i, new_label_tn);
	}
      }
    }
  }
}
#endif
