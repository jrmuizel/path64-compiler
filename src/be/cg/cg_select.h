/*
  Copyright (C) 2001, STMicroelectronics, All Rights Reserved.

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
 * Module: cg_select.h
 *
 * Description:
 *
 *   External interface to the select transformations.
 *
 * Utilities:
 *
 *   void HB_Form_Select(RID *rid, const BB_REGION& bb_region)
 *	Perform select optimizations
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef SELECT_H_INCLUDED
#define SELECT_H_INCLUDED

#include "cg_sched_est.h"

/* ====================================================================
 *   flags:
 * ====================================================================
 */
extern INT32 CG_select_spec_loads;
extern BOOL CG_select_spec_stores;
extern BOOL CG_select_merge_stores;
extern BOOL CG_ifc_allow_dup;
extern BOOL CG_select_promote_mem;
extern const char* CG_ifc_factor_string;
extern BOOL CG_ifc_factor_overridden;
extern float CG_ifc_factor;

//TB: Export variable
extern BOOL CG_ifc_freq; 
extern BOOL CG_ifc_cycles; 
extern BOOL CG_ifc_space;
extern BOOL CG_ifc_logif;

inline float CGTARG_Ifc_Factor(void)
{
  return CG_ifc_factor_overridden ?
    atof(CG_ifc_factor_string) : CG_ifc_factor;
}

extern void Optimize_Conditional_Branches();
extern void Convert_Select(RID *, const BB_REGION&);
extern void Convert_Min_Max(RID *, const BB_REGION&);
extern void Select_Init(void);
extern void draw_CFG(void);

// Traces
// -Wb,-tt61:flags
#define Select_Trace_Candidates 1 
#define Select_Trace_Gen        2 
#define Select_Trace_Spec       4 
#define Select_Trace_Merge      8 
#define Select_Trace_Dup        16 
#define Select_Trace_Stats      32
#define Select_Trace_daVinci    0x4000
#define Select_Trace_stdout  	0x8000

// op list management
typedef std::list<OP*>  op_list;

static inline void
BB_Remove_Ops (BB* bb, op_list ops) 
{
  op_list::iterator op_iter = ops.begin();
  op_list::iterator op_end  = ops.end();
  for (; op_iter != op_end; ++op_iter) {
    BB_Remove_Op(bb, *op_iter);
  }
  ops.clear();
}

typedef struct {
  BB *fall_thru;
  BB *target;
  float prob;
} logif_info;

//
// targ_select.cxx
//
extern TN * Expand_CMP_Reg (TN*, OPS *);
extern void Expand_Cond_Store (TN *, OP *, OP *, UINT8, OPS *ops);
extern void Expand_BlackHole (OP *, TN *, OPS *);
extern void Expand_CondStoreAddr (OP *, TN *, OPS *);
extern void Expand_CondStoreOP (OP *, TN *, OPS *);

/* TOP used to merge guards. used for heuristics. */
extern TOP Get_TOP_Merge_Preds();
extern void CG_SCHED_EST_Add_Merge_Pred(CG_SCHED_EST *se);

extern float CGTARG_Compute_est_cost_before(CG_SCHED_EST *se1, CG_SCHED_EST *se2, CG_SCHED_EST *sehead, 
                                            float taken_prob, float fallthr_prob, BOOL will_need_predicate_merge) ;
extern float CGTARG_Compute_est_cost_after(CG_SCHED_EST *se1, CG_SCHED_EST *se2, CG_SCHED_EST *sehead, 
                                           float taken_prob, float fallthr_prob, 
                                           BOOL will_need_predicate_merge,BB* head) ;
extern BOOL CGTARG_Check_Profitable_Select(CG_SCHED_EST *se1, CG_SCHED_EST *se2, CG_SCHED_EST *sehead, 
                                    int size_se1, int size_se2, int size_sehead, 
                                    float est_cost_before, float est_cost_after, float fallthr_prob, 
                                    BB *head, BB *tail,
                                    BOOL will_need_predicate_merge);
extern BOOL CGTARG_Check_Profitable_Logif(float cycles1, float est_cost_before, float select_factor, 
                                          BB *bb1, BB *bb2);
extern BOOL CGTARG_apply_min_max_transformation(OP *cmp, OP* phi, BOOL cmp_order);
extern BOOL CGTARG_IsNegOP(OP* op);
extern BOOL CGTARG_apply_abs_transformation(OP *cmp, OP* phi, BOOL cmp_order);
extern BOOL CGTARG_OP_is_float_cst_load(OP *op, INT32 val);
#endif /* SELECT_H_INCLUDED */
