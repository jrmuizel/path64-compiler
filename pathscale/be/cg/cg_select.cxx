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
 * Module: cg_select.cxx
 *
 * Description:
 *
 * Simplifies successive conditional jumps into logical expressions.
 * If-converts conditional if regions using selects and speculation.
 * This optimisation works and maintains the SSA representation.
 *
 * General Flags are:
 * -CG:if_convert=TRUE    enable if conversion
 * -CG:ifc_spec_stores=TRUE   enable conditional or blackhole stores
 * -CG:ifc_merge_stores=TRUE  enable store merging optimisation
 *
 * The following flags to drive the algorithm and heuristics.
 * -CG:ifc_allow_dup=TRUE     remove side entries. duplicate blocks
 *                               might increase code size in some cases.
 *
 * -CG:ifc_spec_loads=[0,1,2]  speculate loads 
 *                              0 = no load speculation
 *                              1 = speculate safe loads (default)
 *                              2 = speculate all loads 
 *
 * -CG:ifc_factor="1.05"      factor to reduce the cost of the 
 *                              ifconverted region
 *
 * -CG:ifc_promote_mem=true  base or offset memory load promotion
 *
 * -CG:ifc_freq=TRUE         heuristic based on execution frequency (optimize
 *                              speed) or size of block (optimize space).
 *
 * -TARG:conditional_load=TRUE    enable predicated loads
 * -TARG:conditional_store=TRUE   enable predicated stores
 * -TARG:conditional_prefetch=TRUE enable predicated prefetchs
 * -TARG:dismissible_load=TRUE    enable dismissible loads
 *
 * ====================================================================
 * ====================================================================
 */

// [HK]
#if __GNUC__ >=3
#include <list>
#include <map>
#include <set>
#else
#include <list.h>
#endif // __GNUC__ >=3
#include "namespace_trans.h"

#include "defs.h"
#include "config.h"
#include "config_TARG.h"
#include "tracing.h"
#include "timing.h"
#include "cgir.h"
#include "cg.h"
#include "cgexp.h"
#include "cgtarget.h"
#include "cg_flags.h"
#include "ttype.h"
#include "bitset.h"
#include "bb_map.h"
#include "bb_set.h"
#include "label_util.h"
#include "freq.h"
#include "whirl2ops.h"
#include "dominate.h"
#include "gtn_universe.h"
#include "gtn_set.h"
#include "gtn_tn_set.h"
#include "gra_live.h"
#include "cxx_memory.h"
#include "cflow.h"
#include "opt_alias_interface.h"
#include "cg_ssa.h"
#include "cg_select.h"
#include "DaVinci.h"
#include "cg_ssaopt.h"

extern char *Cur_PU_Name;

static BB_MAP postord_map;      // Postorder ordering
static BB     **cand_vec;       // Vector of potential hammocks BBs.
static INT32  max_cand_id;	// Max value in hammock candidates.
static BB_MAP if_bb_map;        // Map fall_thru and targets of logif

// Mapping between old and new PHIs
// When we change the number of predecessors of a basic block, we record
// the new phis in this map. Old phis are then replaced all in once with 
// BB_Update_Phis.

typedef mempool_allocator<BB*>   BB_ALLOC;
typedef std::list<BB*,BB_ALLOC>  BB_Lst;
typedef BB_Lst::iterator         BB_Lst_Iter;
typedef BB_Lst::const_iterator   BB_Lst_Const_Iter;

typedef struct {
  OP *phi;
  BB_Lst bbs;
} phi_t;

static OP_MAP phi_info_map;

// List of of memory accesses found in if-then-else region. 
// Load and stores lists are used in a slightly different manner:
// Stores are mapped with their equivalent.
// Loads are just recorded. They will be replaced with their dismissible form
// in BB_Fix_Spec_Loads.
// I keep these operation in a list because we don't want to touch the basic
// blocks now. (we don't know yet if the hammock will be reduced).
// OPs will be updated in BB_Fix_Spec_Loads and BB_Fix_Spec_Stores.

// we don't want to modify ops until the final commit is done,
// so we can lazilly assign predicates during the process.

#ifdef EFFECT_PRED_FALSE
typedef struct {
  TN *tn;
  bool on_false;
} guard_t;

#define Set_PredOp(op, pred, invert) do { \
pred_i[op].tn = pred; \
pred_i[op].on_false = invert; \
} while (0)
#define PredOp_Invert(op) (pred_i[op].on_false)
#else 
typedef struct {
  TN *tn;
} guard_t;

#define Set_PredOp(op, pred, invert) (pred_i[op].tn = pred)
#define PredOp_Invert(op) false
#endif 

#define PredOp_TN(op) (pred_i[op].tn)
#define Need_Predicate_Op(op) (pred_i.find(op) != pred_i.end())

typedef std::map<OP*, guard_t, Op_Map_Cmp>    PredOp_Map;
typedef PredOp_Map::iterator PredOp_Map_Iter;
typedef PredOp_Map::const_iterator PredOp_Map_ConstIter;

static PredOp_Map         pred_i;

#define PredOp_Map_Op(iter) ((*iter).first)
#define PredOp_Map_Pred(iter) ((*iter).second)

// Mapping of register class equivalent tns
static TN_MAP btn_map = NULL;

static PredOp_Map_Iter pred_erase(PredOp_Map_Iter i_iter)
{
  PredOp_Map_Iter temp=i_iter++;
  pred_i.erase(temp);
  return i_iter;
}

/* ====================================================================
 *   flags:
 * ====================================================================
 */
BOOL CG_select_spec_stores = TRUE;
BOOL CG_select_merge_stores = TRUE;
BOOL CG_ifc_allow_dup = TRUE;
BOOL CG_select_promote_mem = TRUE;
INT32 CG_select_spec_loads = 1;
BOOL CG_ifc_freq = TRUE;
BOOL CG_ifc_cycles = TRUE;
const char* CG_ifc_factor_string = "1.05";
BOOL CG_ifc_factor_overridden = FALSE;
float CG_ifc_factor; /* in Configure_CG_Target */
static float select_factor;
static int branch_penalty;
BOOL CG_ifc_logif = TRUE;

//TB:
BOOL CG_ifc_space = FALSE;

/* ================================================================
 *
 *   Traces
 *
 * ================================================================
 */
static BOOL Trace_Select_Candidates; /* -Wb,-tt61:0x001 */
static BOOL Trace_Select_Gen;        /* -Wb,-tt61:0x002 */
static BOOL Trace_Select_Spec;       /* -Wb,-tt61:0x004 */
static BOOL Trace_Select_Merge;      /* -Wb,-tt61:0x008 */
static BOOL Trace_Select_Dup;        /* -Wb,-tt61:0x010 */
static BOOL Trace_Select_Stats;      /* -Wb,-tt61:0x020 */
static BOOL Trace_Select_daVinci;    /* -Wb,-tt61:0x4000 */
FILE *Select_TFile;

static void
Trace_Select_Init() {
  Trace_Select_Candidates = Get_Trace(TP_SELECT, Select_Trace_Candidates);
  Trace_Select_Gen        = Get_Trace(TP_SELECT, Select_Trace_Gen);
  Trace_Select_Spec       = Get_Trace(TP_SELECT, Select_Trace_Spec);
  Trace_Select_Merge      = Get_Trace(TP_SELECT, Select_Trace_Merge);
  Trace_Select_Dup        = Get_Trace(TP_SELECT, Select_Trace_Dup);
  Trace_Select_Stats      = Get_Trace(TP_SELECT, Select_Trace_Stats);
  Trace_Select_daVinci    = Get_Trace(TP_SELECT, Select_Trace_daVinci);

  if (Get_Trace(TP_SELECT, Select_Trace_stdout))
    Select_TFile = stdout;
  else
    Select_TFile = TFile;
}

MEM_POOL MEM_Select_pool;
UINT select_count;
UINT logif_count;
UINT spec_instrs_count;
UINT disloads_count;
UINT dup_bbs;

static void
CG_SELECT_Statistics()
{
  if (select_count || logif_count || dup_bbs) {
    fprintf (Select_TFile, "========= %s ======= \n",
             ST_name(Get_Current_PU_ST()));

    if (select_count) 
      fprintf (Select_TFile, "<cg_select> converted %d move%s\n",
               select_count, select_count > 1 ? "s" : "");

    if (logif_count) 
      fprintf (Select_TFile, "<cg_select> reduced %d logical expression%s \n",
               logif_count, logif_count > 1 ? "s" : "");

    if (spec_instrs_count) 
      fprintf (Select_TFile, "<cg_select> speculated %d instr%s \n",
               spec_instrs_count, spec_instrs_count > 1 ? "s" : "");

    if (disloads_count) 
      fprintf (Select_TFile, "<cg_select> speculated %d load%s \n",
               disloads_count, disloads_count > 1 ? "s" : "");

    if (dup_bbs) 
      fprintf (Select_TFile, "<cg_select> duplicated %d basic block%s \n",
               dup_bbs, dup_bbs > 1 ? "s" : "");
  }
}

#ifdef SUPPORTS_SELECT

/* ================================================================
 *
 *   Initialize/Finalize
 *
 * ================================================================
 */
void 
Select_Init()
{
  static BOOL did_init = FALSE;

  if ( ! did_init ) {
    MEM_POOL_Initialize(&MEM_Select_pool,"Select pool",FALSE);
    did_init = TRUE;
  }
  MEM_POOL_Push(&MEM_Select_pool);

}

static void
Initialize_Memory()
{
}

static void
Finalize_Memory()
{
  Free_Dominators_Memory();
  MEM_POOL_Pop(&MEM_Select_pool);
}

static void
Finalize_Select(void)
{
  Finalize_Memory();

  BB_MAP_Delete(postord_map);

  max_cand_id = 0;
  select_count = 0;
  logif_count = 0;
  spec_instrs_count = 0;
  disloads_count = 0;
  dup_bbs = 0;
}

static void
Initialize_Hammock_Memory()
{
  btn_map = TN_MAP_Create();
  phi_info_map = OP_MAP_Create();
}

static void
Finalize_Hammock_Memory()
{
  OP_MAP_Delete(phi_info_map);
  TN_MAP_Delete(btn_map);
}

static void
clear_spec_lists()
{
  pred_i.clear();
}

/* ================================================================
 *
 *   Misc routines to manipulate the CFG.
 *
 * ================================================================
 */
// Return the immediate postdominator of bb that can eventually be a
// 'then' or an 'else' side of a 'if-then-else' region.
static BB*
Find_End_Select_Region(BB* bb, BB* then_bb, BB* else_bb)
{
  BB_SET *pdom_set = BB_pdom_set(bb);
  pdom_set = BB_SET_Intersection(pdom_set,
                                 BB_pdom_set(then_bb), &MEM_Select_pool);
  pdom_set = BB_SET_Intersection(pdom_set,
                                 BB_pdom_set(else_bb), &MEM_Select_pool);

  BB *succ;
  BB *tail_bb = NULL;

  // Returns the bb in pdom_set that dominates all the others
  FOR_ALL_BB_SET_members(pdom_set, succ) {
    if (tail_bb == NULL || BB_SET_MemberP(BB_pdom_set(succ), tail_bb))
      tail_bb = succ;
  }

  // No postdom found
  if (tail_bb == NULL) return NULL;

  BB *last_succ = then_bb;
  if (last_succ != tail_bb)
    while ((succ = BB_Unique_Successor (last_succ)) && succ != tail_bb)
      last_succ = succ;

  if (last_succ != tail_bb && BB_succs_len(last_succ) != 1)
    return NULL;

  last_succ = else_bb;
  if (last_succ != tail_bb)
    while ((succ = BB_Unique_Successor (last_succ)) && succ != tail_bb)
      last_succ = succ;

  if (last_succ != tail_bb && BB_succs_len(last_succ) != 1)
    return NULL;

  return tail_bb;
}

static void
BB_Fall_Thru_and_Target_Succs(BB *bb, BB **fall_thru, BB **target)
{
  BBLIST *edge;
  logif_info *logif;

  DevAssert (BB_kind(bb) == BBKIND_LOGIF, ("BB_Fall_Thru_and_Target_Succs"));

  logif = (logif_info *)BB_MAP_Get(if_bb_map, bb);
  if (logif) {
    *fall_thru = logif->fall_thru;
    *target    = logif->target;
    return;
  }

  *fall_thru = BB_Fall_Thru_Successor(bb);
  FOR_ALL_BB_SUCCS(bb, edge) {
    *target = BBLIST_item(edge);
    if (*target != *fall_thru) break;
  }

  DevAssert (*fall_thru && *target, ("BB_Fall_Thru_and_Target_Succs"));
  
  logif = (logif_info*)MEM_POOL_Alloc(&MEM_Select_pool, sizeof(logif_info));
  logif->fall_thru = *fall_thru;
  logif->target    = *target;
  BB_MAP_Set(if_bb_map, bb, logif);

  return;
}

/* ================================================================
 *
 *   Misc routines to manipulate the SSA's Phis.
 *
 * ================================================================
 */
static UINT8 
Get_TN_Pos_in_PHI (OP *phi, BB *bb)
{
  BB *tb;

  while ((tb = BB_Unique_Successor (bb)) && tb != OP_bb (phi))
    bb = tb;

  return Get_PHI_Predecessor_Idx (phi, bb);
}

static BB*
Get_BB_Pos_in_PHI (OP *phi, BB *bb)
{
  BB *tb;

  while ((tb = BB_Unique_Successor (bb)) && tb != OP_bb (phi))
    bb = tb;

  return bb;
}

static bool
Can_Predicate_Op (OP *op)
{
  if (OP_load (op)) return PROC_has_predicate_loads();
  if (OP_store (op)) return PROC_has_predicate_stores();
  if (OP_prefetch (op)) return PROC_has_predicate_loads();

  return FALSE;
}

static TN*
Generate_Merged_Predicates(TN *pred_tn, TN *tn, VARIANT variant, OPS *ops)
{
  TN *new_tn = Dup_TN(pred_tn);
  TN *tn2 = tn;

  if (!(tn = (TN *)TN_MAP_Get(btn_map, tn2))) {
    tn = Expand_CMP_Reg (tn2, ops);
    TN_MAP_Set(btn_map, tn, tn2);
  }

  tn2 = pred_tn;
  if (!(pred_tn = (TN *)TN_MAP_Get(btn_map, tn2))) {
    pred_tn = Expand_CMP_Reg (tn2, ops);
    TN_MAP_Set(btn_map, pred_tn, tn2);
  }

  Expand_Logical_And (new_tn, pred_tn, tn, variant, ops);

  //  Print_OPS (ops);
  return new_tn;
}

// Check than the tn doesn't depend on an instruction that is beeing predicated.
static bool
Always_Locally_Defined (TN *tn, BB *bb)
{
  if (!TN_is_register (tn))
    return TRUE;

  OP *op_def = TN_ssa_def(tn);
  if (!op_def || OP_phi (op_def))
    return TRUE;

  if (Need_Predicate_Op (op_def))
    return FALSE;

  // Almost can't say anything about this op_def. 
  // be very conservative here... but good enough. too much
  // aggressiveness would harm.
  if (OP_bb (op_def) != bb) {
    for (INT i = 0; i < OP_opnds(op_def); i++) {  
      TN *tn2 = OP_opnd (op_def, i);
      if (TN_is_register (tn2) && TN_ssa_def (tn2))
        return FALSE;
    }
    return TRUE;
  }

  for (INT i = 0; i < OP_opnds(op_def); i++)
    if (! Always_Locally_Defined (OP_opnd (op_def, i), bb))
      return FALSE;

  return TRUE;
}

static bool
Can_Be_Dismissible(OP *op)
{
  return (OP_load (op) && Enable_Dismissible_Load && PROC_has_dismissible_load() && !OP_volatile(op));
}

static void
Create_PSI_or_Select (TN *target_tn, TN* test_tn, BB *head, BB* true_bb, BB* false_bb,
                      OP *phi, OPS *cmov_ops)
{
  TN *true_tn   = OP_opnd(phi, Get_TN_Pos_in_PHI (phi, true_bb));
  TN *false_tn  = OP_opnd(phi, Get_TN_Pos_in_PHI (phi, false_bb));

#ifdef TARG_ST200
    // cond_tn1 need to be a pred register
    // must improve interface.
    if (TN_ssa_def (test_tn) && OP_code(TN_ssa_def (test_tn)) == TOP_convbi_b_r)
      test_tn = OP_opnd(TN_ssa_def (test_tn), 0);
#else
    FmtAssert(Is_Predicate_REGISTER_CLASS( TN_register_class(test_tn)),
	      ("cond_tn1 is not a predicate"));
#endif

  DevAssert(true_tn && false_tn, ("Select: undef TN"));

  INT num_results = 1;
  INT num_opnds = 4;

  TN *result[num_results];
  TN *opnd[num_opnds];

  /* easy case first */
  if (TNs_Are_Equivalent (true_tn, false_tn)) {
    Expand_Copy (target_tn, True_TN, true_tn, cmov_ops);
    return;
  }

  OP *opt = TN_ssa_def(true_tn);
  OP *opf = TN_ssa_def(false_tn);

  if (PROC_is_select() &&
      (!opt || !opf ||
       (GTN_SET_MemberP(BB_live_out(head), true_tn) ||
        OP_Can_Be_Speculative (opt) || Can_Be_Dismissible (opt)) &&
       (GTN_SET_MemberP(BB_live_out(head), false_tn) ||
        OP_Can_Be_Speculative (opf) || Can_Be_Dismissible (opf)))) {
#if 1
    /* transform region guarded by p
       (p)  ? tn1 = psi(T:x, p0:b) 
       (!p) ? tn2 = psi(T:y, p1:c)
       tn3 = phi(tn1, tn2)
       if converted as:
       tn1 = psi(T:x, (p0&p):b) 
       tn2 = psi(T:y, (p1&!p):c)
       tn3 = select(p ? tn1, tn2)
       in:
       tn4 = select(p ? x : y)
       tn3 = psi(T:tn4, (p0&p):b, (p1&!p):c)
    */
 
    if (opt && opf && OP_psi (opt) && OP_psi (opf) &&
        TN_is_true(OP_opnd (opt, 0)) && TN_is_true(OP_opnd (opf, 0))) {

      true_tn = OP_opnd (opt, 1);
      false_tn = OP_opnd (opf, 1); 
      TN *sel_tn = Dup_TN (target_tn);

      INT num_opnds = OP_opnds(opt) + OP_opnds(opf) - 2;
      TN *opnd[num_opnds];

      Expand_Select (sel_tn, test_tn, true_tn, false_tn, 
                     TN_size(target_tn) == 8 ? MTYPE_I8: MTYPE_I4,
                     FALSE, cmov_ops);

      result[0] = target_tn;
      opnd[0] = True_TN;
      opnd[1] = sel_tn;

      int j = 2;

      for (int i = 2; i < OP_opnds(opt); i++) {
        opnd[j++] = OP_opnd(opt, i);
      }

      for (int i = 2; i < OP_opnds(opf); i++) {
        opnd[j++] = OP_opnd(opf, i);
      }

      OP *psi_op = Mk_VarOP (TOP_psi,
                             num_results,
                             num_opnds,
                             result,
                             opnd);

      if (OP_bb(opt) && !TN_is_global_reg(OP_result(opt, 0)))
        BB_Remove_Op(OP_bb(opt), opt);
      if (OP_bb(opf) && !TN_is_global_reg(OP_result(opf, 0)))
        BB_Remove_Op(OP_bb(opf), opf);

      OPS_Append_Op(cmov_ops, psi_op);
      return;
    }
#endif

    if (!opt) true_tn = false_tn;
    else if (!opf) false_tn = true_tn;

    Expand_Select (target_tn, test_tn, true_tn, false_tn, 
		   TN_size(target_tn) == 8 ? MTYPE_I8: MTYPE_I4,
                   FALSE, cmov_ops);
    return;
  }

  if (Trace_Select_Gen) {
    fprintf (Select_TFile, "Create_PSI_or_Select :\n");
    fprintf (Select_TFile, "true_tn:\n");
    Print_TN (true_tn, FALSE);
    if (opt) Print_OP (opt);
    fprintf (Select_TFile, "false_tn:\n");
    Print_TN (false_tn, FALSE);
    if (opf) Print_OP (opf);
  }

  TN* cond_tn=(TN*)-1;
  TN* fcond_tn=(TN*)-1;

  bool invert_cond=false;
  bool invert_fcond=false;

  if (Need_Predicate_Op (opt)) {
    cond_tn = PredOp_TN(opt);
    invert_cond = PredOp_Invert(opt);
  }
  if (Need_Predicate_Op (opf)) {
    fcond_tn = PredOp_TN(opf);
    invert_fcond = PredOp_Invert(opf);
  }

  result[0] = target_tn;
  // index for false predicate register
  int inv = -1;

  if (fcond_tn == (TN*)-1) {
    TN *pred_tn = True_TN;
    int p_i = -1;
    if (OP_has_predicate (opf)) {
      p_i = OP_find_opnd_use(opf, OU_predicate);
      pred_tn = OP_opnd(opf, p_i);
    }

    if (pred_tn != True_TN) {
      fcond_tn = Generate_Merged_Predicates (pred_tn, test_tn, 
                                             V_BR_NONE,
                                             cmov_ops);
      OP *mop = OPS_last(cmov_ops);
      Set_OP_Pred_False(mop, 2);

      if (OP_Pred_False (opf, p_i))
        Set_OP_Pred_False(mop, 1);          
    }
    else
      fcond_tn = True_TN;
  }

  if (cond_tn == (TN*)-1) {
    TN *pred_tn = True_TN;
    int p_i = -1;
    if (OP_has_predicate (opt)) {
      p_i = OP_find_opnd_use(opt, OU_predicate);
      pred_tn = OP_opnd(opt, p_i);
    }

    if (pred_tn != True_TN) {
      cond_tn = Generate_Merged_Predicates (pred_tn, test_tn, 
                                            V_BR_NONE,
                                            cmov_ops);
      OP *mop = OPS_last(cmov_ops);

      if (OP_Pred_False (opt, p_i))
        Set_OP_Pred_False(mop, 1);
    }
    else
      cond_tn = True_TN;
  }

  // FdF 20090904: Consider the case where both are True_TN, which
  // means that none of opf and opt are already predicated. Just set
  // predicate T on the op that dominates the other
  
  if ((cond_tn == True_TN) && (fcond_tn == True_TN)) {
    if (OP_Dominates(opt, opf)) {
#ifdef EFFECT_PRED_FALSE
      invert_fcond = true;
      fcond_tn = test_tn;
#else
      fcond_tn = Dup_TN (test_tn);
      Exp_Pred_Complement(fcond_tn, NULL, test_tn, cmov_ops);
#endif
    }
    else {
      cond_tn = test_tn;
    }
  }

  // if each psi operand is true make sure the first one is defined before
  //  the second one
  if (TN_is_true (fcond_tn) && !TN_is_true (cond_tn)) {
#ifdef EFFECT_PRED_FALSE
    if (invert_fcond) {
      inv = 0;
    }
    else if(invert_cond) {
      inv = 1;
    }
#endif
    opnd[0] = fcond_tn;
    opnd[1] = false_tn;
    opnd[2] = cond_tn;
    opnd[3] = true_tn;
  }
  else {
#ifdef EFFECT_PRED_FALSE
    if (invert_fcond) {
      inv = 1;
    }
    else if(invert_cond) {
      inv = 0;
    }
#endif
    opnd[0] = cond_tn;
    opnd[1] = true_tn;
    opnd[2] = fcond_tn;
    opnd[3] = false_tn;
  }

  OP *psi_op = Mk_VarOP (TOP_psi,
			 num_results,
			 num_opnds,
			 result,
			 opnd);

  if (inv >= 0) {
    Set_PSI_Pred_False(psi_op, inv);
  }
    
  if (Trace_Select_Gen) {
    fprintf (Select_TFile, "Create_PSI_or_Select\n");
    Print_OP (psi_op);
  }

  OPS_Append_Op(cmov_ops, psi_op);
}


static phi_t *
BB_Alloc_Phi ()
{
  phi_t *phi_info = (phi_t*)MEM_POOL_Alloc(&MEM_Select_pool, sizeof(phi_t));
  new (&phi_info->bbs)BB_Lst;
  return phi_info;
}

static void
BB_Create_And_Map_Phi (UINT8 nopnds, TN *result[], TN *opnd[],
                       OP *old_phi, phi_t *phi_info) 
{
  Is_True(OP_code(old_phi) == TOP_phi,("not a PHI function"));

  if (nopnds == 1) {
    OPS ops = OPS_EMPTY;
    Expand_Copy (result[0], True_TN, opnd[0], &ops);
    phi_info->phi = OPS_last(&ops);
  }
  else
    phi_info->phi = Mk_VarOP (TOP_phi, 1, nopnds, result, opnd);

  OP_MAP_Set(phi_info_map, old_phi, phi_info);
}
     
// Remove old phis or replace it with a new one from phi_info_map.
// Note that we cannot use the same list to insert selects because they
// would be inserted in the 'head' bblock.
static void
BB_Update_Phis(BB *bb)
{
  OP *phi;

  for (phi = BB_first_op(bb); phi && OP_phi(phi); ) {
    phi_t *phi_info = (phi_t *)OP_MAP_Get(phi_info_map, phi);

    OP *next = OP_next(phi);
    BB_Remove_Op(bb, phi);

    if (phi_info) {
      OP *new_phi = phi_info->phi;

      BB_Prepend_Op(bb, new_phi);
      if (OP_phi(new_phi))
        Initialize_PHI_map(new_phi);

      int i=0;
      while(!phi_info->bbs.empty())
        {
          BB *pred = phi_info->bbs.back();
          if (OP_phi(new_phi))
            Set_PHI_Predecessor (new_phi, i++, pred);
          phi_info->bbs.pop_back();
        }

      OP_MAP_Set(phi_info_map, phi, NULL);
    }

    phi = next;
  }
}

static void
BB_Recomp_Phis (BB *bb, BB *bb1, TN *cond_tn1, BB *bb2, TN *cond_tn2, BOOL true_br)
{
  OP *phi;
  OPS cmov_ops = OPS_EMPTY;
  op_list old_phis;

  OP *br1_op = BB_branch_op(bb1);

  FOR_ALL_BB_PHI_OPs(bb, phi) {
    UINT8 npreds = OP_opnds(phi);
    TN *select_tn = npreds > 2 ? Dup_TN(OP_result(phi, 0)) : OP_result(phi, 0);

    if (true_br) 
      Create_PSI_or_Select (select_tn, cond_tn1, bb1, bb1, bb2, phi, &cmov_ops);
    else
      Create_PSI_or_Select (select_tn, cond_tn1, bb1, bb2, bb1, phi, &cmov_ops);

    if (npreds > 2) {
      UINT8 j = 0;
      TN *result[1];
      UINT8 nopnds = npreds - 1;
      TN *opnd[nopnds];

      phi_t *phi_info = BB_Alloc_Phi();
      BB_Lst *bbs = &phi_info->bbs;

      Set_TN_is_global_reg (select_tn);
      
      for (UINT8 i = 0; i < npreds; i++) {
        BB *pred = Get_PHI_Predecessor (phi, i);
        if (pred == bb1) {
          bbs->push_front(bb1);
          opnd[j++] = select_tn;
        }
        else  if (pred != bb2) {
          bbs->push_front(pred);
          opnd[j++] = OP_opnd(phi, i);
        }
      }

      result[0] = OP_result(phi, 0);
      BB_Create_And_Map_Phi (nopnds, result, opnd, phi, phi_info);
    }
    else
      old_phis.push_front(phi);      
  }

  BB_Remove_Ops(bb, old_phis);
  BB_Insert_Ops_Before(bb1, br1_op, &cmov_ops);

  BB_Update_Phis(bb);
}

/* ================================================================
 *
 *   If blocks Selection
 *
 * ================================================================
 */
// Create a map of conditional blocks sorted in postorder. That makes it
// easy to iterate through possible if regions.
static void
Identify_Logifs_Candidates(void)
{
  postord_map = BB_Postorder_Map(NULL, NULL);

  // maximum size.
  cand_vec = TYPE_MEM_POOL_ALLOC_N(BB *, &MEM_Select_pool, PU_BB_Count);
  max_cand_id = 0;

  // Make a list of logifs blocks
  for (BB *bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    INT32 bb_id = BB_MAP32_Get(postord_map, bb);
    DevAssert(bb_id >= 0 && bb_id <= PU_BB_Count, ("bad <postord_map> value"));
    // we are interested only with reachable conditional head BBs
    if (bb_id > 0 && BB_kind (bb) == BBKIND_LOGIF) { 
#ifdef Is_True_On
    const char *bb_name = NULL;
    const char *pu_id_name = NULL;
    bool skip_ifc_found = false;
    if (getenv("IFC_SKIP_BB_PU"))    bb_name = getenv("IFC_SKIP_BB_PU");
    if (bb_name != NULL && strcmp(ST_name(Get_Current_PU_ST()),bb_name) == 0) {
        if (getenv("IFC_SKIP_BB_ID")) pu_id_name = getenv("IFC_SKIP_BB_ID");
        if (pu_id_name != NULL) {
            char *skip_name = strdup(pu_id_name);
            skip_name = strtok(skip_name," ");
            while (skip_name) {
                int BB_id_skip = atoi(skip_name);
                if(bb_id == BB_id_skip) {
                    skip_ifc_found = true;
                    break;
                }
                skip_name = strtok(NULL," ");
            }
        } else {
            skip_ifc_found = true;
        }
    }        
   if (skip_ifc_found)  {
       fprintf(TFile,"Identify_Logifs_Candidates : Skip BB %d\n",bb_id);
       cand_vec[bb_id - 1] = NULL;
       continue;
   }
#endif 
      max_cand_id = MAX(bb_id, max_cand_id);
      cand_vec[bb_id - 1] = bb;
    }
    else {
      cand_vec[bb_id - 1] = NULL;
    }
  }
}

// 
// Check if all the OPs of a bb can be speculated. Save memory references
// for later use.
//
static BOOL
Can_Speculate_BB(BB *bb)
{
  OP* op;
  bool pred_av = false;

  if (BB_Has_Exc_Label(bb) ||
      BB_Has_Addr_Taken_Label(bb) ||
      BB_Has_Outer_Block_Label(bb))
    return FALSE;

  // [CG]: Nothing can be speculated
  if (Eager_Level == EAGER_NONE) return FALSE;

  FOR_ALL_BB_OPs_FWD(bb, op) {
    // can't speculate hardware registers.
    for (INT opndnum = 0; opndnum < OP_results(op); opndnum++) {
      TN *res = OP_result(op, opndnum);
      if (TN_is_register(res) && TN_is_global_reg(res) && TN_is_dedicated(res))
        return FALSE;
    }
    
	// [TDR] Fix for bug #78333
	// If a operation has a predicate without being flagged as predicated op, 
	// generated if-converted code may lead to incorect computation
	// especially in case of guarded compare / guarded org/andg on STxP70
	if (!OP_has_predicate (op) && OP_Predicate(op) && OP_Predicate(op)!=True_TN) {
        return FALSE;
	}

    /* check if we are trying to speculate predicated ops */
    if (OP_has_predicate (op)) {
      int p_i = OP_find_opnd_use(op, OU_predicate);
      DevAssert(p_i >= 0, ("Invalid predicate"));
      TN *cond_tn = OP_opnd(op, p_i);
      Set_PredOp (op, cond_tn, OP_Pred_False (op, p_i));
      Set_TN_is_if_conv_cond(cond_tn);
    }

    else if (OP_load (op)) {
      if (CG_select_spec_loads == 2) 
        continue;
      else if (CG_select_spec_loads == 1) {
        WN *wn = Get_WN_From_Memory_OP(op);
        TN *base   = OP_opnd(op, OP_find_opnd_use(op, OU_base));
        bool can_speculate=true;

        if (!Always_Locally_Defined (base, OP_bb (op)) || OP_volatile (op))
          can_speculate=false;

        if (wn && Alias_Manager && Safe_to_speculate (Alias_Manager, wn) &&
            can_speculate && !pred_av) {
          continue;
	}
      }

      // loads will be optimized only if hardware support
      if (Can_Be_Dismissible (op) ||
          (Enable_Conditional_Load && PROC_has_predicate_loads())) {
        if (!OP_has_predicate (op) && !OP_Is_Speculative(op)) 
          Set_PredOp (op, 0, false);
      }
      else if (!OP_Can_Be_Speculative(op)) {
        pred_av = true;
        return FALSE;
      }
    }

    else if (OP_store (op)) {
      // stores can always be optimized
      if (CG_select_spec_stores) {
	if ((!Enable_Conditional_Store || !PROC_has_predicate_stores()) && 
	    OP_volatile(op))
	  return FALSE;	  

        if (!OP_has_predicate (op)) 
          Set_PredOp (op, 0, false);
      }

      else if (!OP_Can_Be_Speculative(op))
        return FALSE;
    }
    
    else if (OP_prefetch (op) && Enable_Conditional_Prefetch && PROC_has_predicate_loads()) {
      // FdF 20070209: If safe_to_speculate, then this will save the
      // computation of the guard register
      WN *wn = Get_WN_From_Memory_OP(op);
      if (wn && Alias_Manager && Safe_to_speculate (Alias_Manager, wn))
	continue;

      Set_PredOp (op, 0, false);
      pred_av = true;
    }
    else if (!OP_Can_Be_Speculative(op))
      return FALSE;
  }

  return TRUE;
}

static bool
Same_Reg_Or_Value (TN* tn1, TN *tn2)
{
  if (TN_has_value (tn1)) {
    if (!TN_has_value (tn2))
      return false;
    else
      return (TN_value (tn1) == TN_value(tn2));
  }

  if (TN_is_register (tn1) && TN_is_register (tn2)) {
    if (TNs_Are_Equivalent (tn1, tn2))
      return true;

    OP *op1 = TN_ssa_def(tn1);
    OP *op2 = TN_ssa_def(tn2);

    if (op1 && op2 && OPs_Are_Equivalent (op1, op2)) {
      // FdF 20090722: Must also check that tn1 in op1 and tn2 in op2
      // are defined at the same result index
      for (int i = 0; i < OP_results(op1); i++) {
	if (OP_result(op1, i) == tn1)
	  return (OP_result(op2, i) == tn2);
      }
      Is_True(0, ("tn1 not found in results for TN_ssa_def(op1)"));
    }
  }

  return false;
}

// 
// Check wether 2 memory operations differ only by one element.
//
static bool
Have_Same_Offset_Or_Base(OP* op1, OP* op2)
{
  if (! CG_select_promote_mem)
    return false;

  if (OP_memory(op1) && OP_memory(op2)) {
    TN *offsetTN1 = OP_opnd(op1, OP_find_opnd_use(op1, OU_offset));
    TN *offsetTN2 = OP_opnd(op2, OP_find_opnd_use(op2, OU_offset));
    TN *baseTN1   = OP_opnd(op1, OP_find_opnd_use(op1, OU_base));
    TN *baseTN2   = OP_opnd(op2, OP_find_opnd_use(op2, OU_base));

    if (!Same_Reg_Or_Value (offsetTN1, offsetTN2) && Same_Reg_Or_Value (baseTN1, baseTN2))
      return TN_is_constant (baseTN1);

    if (Same_Reg_Or_Value (offsetTN1, offsetTN2) && !Same_Reg_Or_Value (baseTN1, baseTN2))
      return TN_is_constant (offsetTN1); 
  }

  return false;
}

// 
// Check wether 2 memory operations are aliased memory references
//
static BOOL
Are_Same_Location(OP* op1, OP* op2)
{
  if (OP_memory(op1) && OP_memory(op2)) {
    WN *wn1 = Get_WN_From_Memory_OP(op1);
    WN *wn2 = Get_WN_From_Memory_OP(op2);
    if (wn1 != NULL && wn2 != NULL) {
      ALIAS_RESULT alias = Aliased(Alias_Manager, wn1, wn2);
      if (!OP_black_hole(op1) && !OP_black_hole(op2))
        if (alias == SAME_LOCATION)
          return TRUE;
      
      if (WN_desc (wn1) != WN_desc (wn2) || 
	  WN_field_id (wn1) != WN_field_id (wn2))
	return FALSE;

      // alias analyser doesn't seem to be able to detect obvious cases like
      // if (a) pt = tab1 else pt = tab2;
      // remove this code when Aliased code is fixed
      if (alias == POSSIBLY_ALIASED) {
        TN *offsetTN1 = OP_opnd(op1, OP_find_opnd_use(op1, OU_offset));
        TN *baseTN1   = OP_opnd(op1, OP_find_opnd_use(op1, OU_base));
        TN *offsetTN2 = OP_opnd(op2, OP_find_opnd_use(op2, OU_offset));
        TN *baseTN2   = OP_opnd(op2, OP_find_opnd_use(op2, OU_base));

        if (offsetTN1 == offsetTN2) {
	  if (Same_Reg_Or_Value(baseTN1, baseTN2))
	    return true;
        }
      }
    }
  }

  return FALSE;
}

static BOOL
Are_Not_Aliased(OP* op1, OP* op2)
{
  if (OP_memory(op1) && OP_memory(op2)) {
    WN *wn1 = Get_WN_From_Memory_OP(op1);
    WN *wn2 = Get_WN_From_Memory_OP(op2);

    if (wn1 != NULL && wn2 != NULL) {
      ALIAS_RESULT alias = Aliased(Alias_Manager, wn1, wn2);
      // OK only if we can prove that addresses are not aliases.
        if (alias == NOT_ALIASED)
        return TRUE;
    }
  }
  
  return FALSE;
}

static BOOL
Check_Profitable_Logif (BB *bb1, BB *bb2)
{
  float prob = 0.0;

  //TDR Add an option to force multiple logif 
  if (CG_force_select & 0x2) return TRUE;
#if 0
  if (BB_length(bb1) + BB_length(bb2) >= CG_split_BB_length) {
    if (Trace_Select_Candidates) {
      fprintf (Select_TFile, "expected new block too big. reject\n");
    }
    return FALSE;
  }
#endif

  //TDR Add an option to control multiple logif 
  if (!CG_ifc_logif) return FALSE;

  //TDR In size, a logif transformation is not profitable without bundles.
  if (!CG_ifc_cycles && !CGTARG_Profitable_Logif_Transformation()) {
    return FALSE;
  }


  TN *cond_tn;
  TN *dummy;
  OP *br_op = BB_branch_op(bb2);
  (void) CGTARG_Analyze_Branch(br_op, &cond_tn, &dummy);
  OP *cond_op = TN_ssa_def (cond_tn);

  if (CG_ifc_freq) {
    BBLIST *bblist;
    FOR_ALL_BB_SUCCS(bb1, bblist) {
      BB *succ = BBLIST_item(bblist);
      if (succ == bb2) {
        prob = BBLIST_prob(bblist);
        break;
      }
    }
  }
  else {
    prob = 1;
  }
  
  if (Trace_Select_Gen) {
    fprintf (Select_TFile, "Check_Profitable_Logif BB%d BB%d\n", BB_id(bb1), BB_id(bb2));
  }

  CG_SCHED_EST *se1 = CG_SCHED_EST_Create(bb1, &MEM_Select_pool,
                                          SCHED_EST_FOR_IF_CONV);
  CG_SCHED_EST *se2 = CG_SCHED_EST_Create(bb2, &MEM_Select_pool,
                                          SCHED_EST_FOR_IF_CONV);

  float cycles1 = CG_SCHED_EST_Cycles(se1);
  float cycles2 = CG_SCHED_EST_Cycles(se2);

  if (Trace_Select_Candidates) {
    fprintf (Select_TFile, "bb1 (%f cycles) = \n", cycles1);
    if (se1)
      CG_SCHED_EST_Print(Select_TFile, se1);
    fprintf (Select_TFile, "\n");

    fprintf (Select_TFile, "bb2 (%f cycles) = \n", cycles2);
    if (se2)
      CG_SCHED_EST_Print(Select_TFile, se2);
    fprintf (Select_TFile, "\n");
  }

  // ponderate cost of each region taken separatly.
  float est_cost_before = cycles1 + (cycles2 * prob);

  CG_SCHED_EST_Append_Scheds(se1, se2);

  if (BB_Fall_Thru_Successor (bb1) != bb2)
    CG_SCHED_EST_Subtract_Op_Resources(se1, OP_code(BB_branch_op(bb1)));

  // a new instruction to merge predicate will be added.
  CG_SCHED_EST_Add_Op_Resources(se1, Get_TOP_Merge_Preds());

  cycles1 = CG_SCHED_EST_Cycles(se1);

  if (Trace_Select_Candidates) {
    fprintf (Select_TFile, "ifconverted bb (%f cycles) = \n", cycles1);
    if (se1)
      CG_SCHED_EST_Print(Select_TFile, se1);
    fprintf (Select_TFile, "\n");
  }

  // if the output of the logical condition depends on the current basic block
  // we will increase the dependence height.
  if (cond_op && OP_bb (cond_op) == bb2) {
      for (INT opndnum = 0; opndnum < OP_opnds(cond_op); opndnum++) {
        TN *cond = OP_opnd(cond_op, opndnum);
        if (TN_is_register (cond)) {
        OP *resop = TN_ssa_def (OP_opnd(cond_op, opndnum));
        if (resop && OP_bb (resop) == bb2
#ifdef TARG_STxP70
	    // we don't really speculate on the stxp70
	    )
#else
	    && !OP_Can_Be_Speculative(resop))
#endif
          cycles1 += 4;
      }
    }
  }
  
  CG_SCHED_EST_Delete(se1);
  CG_SCHED_EST_Delete(se2);

  return CGTARG_Check_Profitable_Logif(cycles1, est_cost_before, select_factor, bb1, bb2);
}


static BOOL
Are_equivalent_defininition_TNs(TN *tn1, TN *tn_ref) {
	if (!tn1 || !tn_ref)  return FALSE;
	if (!TN_is_register(tn1) || !TN_is_register(tn_ref)) {
		if (TN_has_value(tn1) && TN_has_value(tn_ref) && 
		   (TN_value(tn1) == TN_value(tn_ref))) 
			return TRUE;
		OP *def_op = TN_ssa_def(tn1);
		if (!def_op) return FALSE;
		if (OP_Is_Copy_Immediate_Into_Register(def_op))  
			return Are_equivalent_defininition_TNs(OP_Copy_Operand_TN(def_op),tn_ref);
		return FALSE;
	}
	if (TNs_Are_Equivalent(tn1,tn_ref)) return TRUE;
	OP *def_op = TN_ssa_def(tn1);
	if (!def_op) return FALSE;
	if (!OP_copy(def_op)) return FALSE;
	return Are_equivalent_defininition_TNs(OP_Copy_Operand_TN(def_op),tn_ref);
}

static BOOL
Are_equivalent_defininition_negated_TNs(TN *tn1, TN *tn_ref, BOOL found_neg) {
	if (!tn1 || !tn_ref)  return FALSE;
	if (!TN_is_register(tn1) || !TN_is_register(tn_ref)) return FALSE;
	if (TNs_Are_Equivalent(tn1,tn_ref)) return found_neg;
	OP *def_op = TN_ssa_def(tn1);
	if (!def_op) return FALSE;
	if (found_neg && !OP_copy(def_op)) return FALSE;
	if (CGTARG_IsNegOP(def_op)) found_neg=!found_neg;
	return Are_equivalent_defininition_negated_TNs(OP_Opnd1(def_op),tn_ref,found_neg);
}

static BOOL
Check_min_max_abs_candidate(BB *head, BB_SET *taken_reg, BB_SET *fallthru_reg, BB *tail) {
	BB *bb1,*bb2;
	TN *cond_tn;
	TN *dummy;
	OP *br_op = BB_branch_op(head);
	BOOL modification_applied=FALSE;
	(void) CGTARG_Analyze_Branch(br_op, &cond_tn, &dummy);
	OP *cond_op = TN_ssa_def (cond_tn);
	if (!TOP_is_cmp(OP_code(cond_op))) return FALSE;
	if (!TN_is_register(OP_Opnd1(cond_op))) return FALSE;
	//If we do not compare register to a register or a constant we do not deal with min/max/abs
	if (TN_is_zero(OP_Opnd2(cond_op)) && !OP_fcmp(cond_op)) {
		// Treatment of float cmp on STxP70 V3: compare from fcmp to register then compare this register to 0
		if (OP_cmp_variant(cond_op) == V_CMP_NE) {
			OP *fpx_cond_op = TN_ssa_def(OP_Opnd1(cond_op));
			if (fpx_cond_op && OP_fcmp(fpx_cond_op)) {
				cond_op=fpx_cond_op;
			} 
		}
	} 
	OP *phi;
	FOR_ALL_BB_PHI_OPs(tail, phi) {
		if (OP_opnds(phi) == 2) {
			bb1 = Get_PHI_Predecessor (phi, 0);
			bb2 = Get_PHI_Predecessor (phi, 1);
			TN *tn1 = OP_opnd(phi,0);
			TN *tn2 = OP_opnd(phi,1);
			FmtAssert(TN_is_register(tn1) && TN_is_register(tn2),("Tns in a Phi operation must be registers\n")); 
			OP *def_op1 = TN_ssa_def(tn1);
			OP *def_op2 = TN_ssa_def(tn2);
			if (!def_op1 || !def_op2) continue;
			if (BB_SET_EmptyP(taken_reg)) {
				if (!(BB_SET_MemberP(fallthru_reg, bb2) || BB_SET_MemberP(fallthru_reg, bb1))) continue;
			} else {
				if (!(BB_SET_MemberP(taken_reg, bb1) && BB_SET_MemberP(fallthru_reg, bb2)) &&
				!(BB_SET_MemberP(fallthru_reg, bb1) && BB_SET_MemberP(taken_reg, bb2))) continue;
			}
			//min/max case : we search simple affectation (copy)
			if(Are_equivalent_defininition_TNs(tn1,OP_Opnd1(cond_op)) && Are_equivalent_defininition_TNs(tn2,OP_Opnd2(cond_op))) {
				if(CGTARG_apply_min_max_transformation(cond_op,phi,BB_SET_MemberP(fallthru_reg, bb1))) modification_applied = TRUE;
				continue;
			}
			if(Are_equivalent_defininition_TNs(tn2,OP_Opnd1(cond_op)) && Are_equivalent_defininition_TNs(tn1,OP_Opnd2(cond_op))) {
				if( CGTARG_apply_min_max_transformation(cond_op,phi,BB_SET_MemberP(fallthru_reg, bb2))) modification_applied = TRUE;
				continue;
			}

			//abs search
			BOOL Abs_Pattern = (OP_Opnd2(cond_op) && TN_is_zero(OP_Opnd2(cond_op))) || 
					(OP_fcmp(cond_op) && OP_Opnd2(cond_op) && TN_ssa_def(OP_Opnd2(cond_op)) && 
					CGTARG_OP_is_float_cst_load(TN_ssa_def(OP_Opnd2(cond_op)),0));
			if (Abs_Pattern) {
				if (Are_equivalent_defininition_TNs(tn1,OP_Opnd1(cond_op)) && Are_equivalent_defininition_negated_TNs(tn2,OP_Opnd1(cond_op),FALSE)) {
					if(CGTARG_apply_abs_transformation(cond_op,phi,BB_SET_MemberP(fallthru_reg, bb2))) modification_applied = TRUE;
				}
				if (Are_equivalent_defininition_TNs(tn2,OP_Opnd1(cond_op)) && Are_equivalent_defininition_negated_TNs(tn1,OP_Opnd1(cond_op),FALSE)) {
					if(CGTARG_apply_abs_transformation(cond_op,phi,BB_SET_MemberP(fallthru_reg, bb1))) modification_applied = TRUE;
				}
			}					
		}
	}
	return (modification_applied);
}


static BOOL
Check_Profitable_Select (BB *head, BB_SET *taken_reg, BB_SET *fallthru_reg,
                         BB *tail)
{
  BBLIST *bb1, *bb2;
  BB *bb;
  bool will_need_predicate_merge = false;
  int size_se1=0;
  int size_se2=0;
  int size_sehead=0;

  //TDR force select
  if (CG_force_select & 0x1) return TRUE;

  // Find block probs
  bb2 = BBlist_Fall_Thru_Succ(head);
  FOR_ALL_BB_SUCCS(head, bb1) {
    if (bb1 != bb2)
      break;
  }

  float taken_prob, fallthr_prob;
  if (CG_ifc_freq) {
    taken_prob = BBLIST_prob(bb1);
    fallthr_prob = BBLIST_prob(bb2);
  }
  else {
    taken_prob = 0.5;
    fallthr_prob = 0.5;
  }

  CG_SCHED_EST *sehead = CG_SCHED_EST_Create(head, &MEM_Select_pool,
                                             SCHED_EST_FOR_IF_CONV);
  float cyclesh = CG_SCHED_EST_Cycles(sehead);

  if (Trace_Select_Candidates) {
    fprintf (Select_TFile, "sehead (%f cycles) = \n", cyclesh);
    CG_SCHED_EST_Print(Select_TFile, sehead);
  }

  CG_SCHED_EST *se1 = NULL;
  CG_SCHED_EST *se2 = NULL;

  float cycles1 = 0.0;
  float cycles2 = 0.0;

  size_sehead = BB_length(head);

  // Compute schedule estimate of taken region
  if (! BB_SET_EmptyP(taken_reg)) {
    se1 = CG_SCHED_EST_Create_Empty(&MEM_Select_pool, SCHED_EST_FOR_IF_CONV);

    FOR_ALL_BB_SET_members(taken_reg, bb) {
      size_se1 += BB_length(bb);

      CG_SCHED_EST* tmp_est = CG_SCHED_EST_Create(bb, &MEM_local_pool,
                                                SCHED_EST_FOR_IF_CONV);
      CG_SCHED_EST_Append_Scheds(se1, tmp_est);
      CG_SCHED_EST_Delete(tmp_est);

      OP *op;
      FOR_ALL_BB_OPs(bb, op) {
        if (OP_has_predicate (op) && OP_opnd(op, OP_find_opnd_use(op,OU_predicate)) != True_TN) {
          will_need_predicate_merge = true;
          break;
        }
      }
    }

    cycles1 = CG_SCHED_EST_Cycles(se1);
  }

  // Compute schedule estimate of fallthru region
  if (! BB_SET_EmptyP(fallthru_reg)) {
    se2 = CG_SCHED_EST_Create_Empty(&MEM_Select_pool, SCHED_EST_FOR_IF_CONV);

    FOR_ALL_BB_SET_members(fallthru_reg, bb) {
      size_se2 += BB_length(bb);

      CG_SCHED_EST* tmp_est = CG_SCHED_EST_Create(bb, &MEM_local_pool,
                                                  SCHED_EST_FOR_IF_CONV);
      CG_SCHED_EST_Append_Scheds(se2, tmp_est);
      CG_SCHED_EST_Delete(tmp_est);

      OP *op;
      FOR_ALL_BB_OPs(bb, op) {
        if (OP_has_predicate (op) && OP_opnd(op, OP_find_opnd_use(op,OU_predicate)) != True_TN) {
          will_need_predicate_merge = true;
          break;
        }
      }
    }

    cycles2 = CG_SCHED_EST_Cycles(se2);
  }
  
  if (Trace_Select_Candidates) {
    fprintf (Select_TFile, "taken (%f cycles) = \n", cycles1);
    if (se1)
      CG_SCHED_EST_Print(Select_TFile, se1);
    fprintf (Select_TFile, "\n");

    fprintf (Select_TFile, "fallthru (%f cycles) = \n", cycles2);
    if (se2)
      CG_SCHED_EST_Print(Select_TFile, se2);
    fprintf (Select_TFile, "\n");
  }

#if 0
  //If new block is bigger than CG_bblength_max, reject.
  INT exp_len = size_sehead + size_se1 + size_se2;
    if (exp_len >= CG_split_BB_length) {
    if (Trace_Select_Candidates) {
      fprintf (Select_TFile, "expected new block too big. reject\n");
    }
    return FALSE;
  }
#endif

  float est_cost_before = CGTARG_Compute_est_cost_before(se1,se2,sehead,
                            taken_prob,fallthr_prob,will_need_predicate_merge) ;
  float ifc_regions_cycles = CGTARG_Compute_est_cost_after(se1,se2,sehead,
                            taken_prob,fallthr_prob,will_need_predicate_merge,head) ;
  float est_cost_after = ifc_regions_cycles / select_factor;

  if (Trace_Select_Candidates) {
        fprintf (Select_TFile, "Info region :\n\t cycles1: %f, cycles2: %f, cyclesh: %f, taken_prob:%f, fallthr_prob:%f\n", 
                                              cycles1,cycles2,cyclesh,taken_prob,fallthr_prob);
        fprintf (Select_TFile, "\t size_se1: %d, size_se2: %d, size_sehead: %d\n", size_se1,size_se2,size_sehead);
        int sehead_latency =  CG_SCHED_EST_Critical_Length(sehead) - CG_SCHED_EST_Resource_Cycles(sehead) - 1 ;
        int se1_latency = (se1)?(CG_SCHED_EST_Critical_Length(se1) - CG_SCHED_EST_Resource_Cycles(se1) + 1):0;
        int se2_latency = (se2)?(CG_SCHED_EST_Critical_Length(se2) - CG_SCHED_EST_Resource_Cycles(se2) + 1):0 ;
        fprintf (Select_TFile, "\t se1_latency: %d, se2_latency: %d, sehead_latency: %d\n", se1_latency,se2_latency,sehead_latency);
        fprintf (Select_TFile, "\t ifc region: BBs %f / %f\n", ifc_regions_cycles, select_factor);
        fprintf (Select_TFile, "\t Comparing without ifc:%f, with ifc:%f\n", est_cost_before, est_cost_after);
  }

  BOOL val = CGTARG_Check_Profitable_Select(se1, se2, sehead, size_se1, size_se2, size_sehead, 
                                            est_cost_before, est_cost_after, fallthr_prob, head, tail,
                                            will_need_predicate_merge);

  CG_SCHED_EST_Delete(sehead);
  if (se1)  CG_SCHED_EST_Delete(se1);
  if (se2)  CG_SCHED_EST_Delete(se2);

  if (Trace_Select_Candidates) {
      fprintf (Select_TFile, "\t Will be converted ? : %s\n", val?"YES":"NO");
  }
  return val;
}

static BOOL
Check_Suitable_Chain (BB* ipdom, BB* bb, BB_SET* path, bool allow_dup)
{
  while (bb != ipdom) {
    DevAssert(bb, ("Invalid BB chain in hammock"));

    // allow removing of side entries only on one of targets.
    if (BB_preds_len (bb) > 1 && (!allow_dup || BB_loophead(bb))) {
      return FALSE; 
    }

    if (! Can_Speculate_BB(bb)) {
      if (Trace_Select_Candidates) 
        fprintf(Select_TFile, "<select> Can't speculate BB%d\n", BB_id(bb));
      return FALSE;
    }

    path  = BB_SET_Union1D(path, bb, &MEM_Select_pool);
    bb = BB_Unique_Successor (bb);
  }

  return TRUE;
}


static BOOL 
Is_Hammock_Legacy (BB *head, BB_SET *t_set, BB_SET *ft_set, BB **tail, bool allow_dup) 
{
  BB *target;
  BB *fall_thru;

  if (Trace_Select_Candidates) {
    fprintf (Select_TFile, "<select> is hammock BB%d ? \n", BB_id(head));
  }

  // Find fall_thru and taken BBs.
  BB_Fall_Thru_and_Target_Succs(head, &fall_thru, &target);

  if (Trace_Select_Candidates) {
    fprintf (Select_TFile, "<select> target BB%d ? fall_thru BB%d\n",
             BB_id(target), BB_id(fall_thru));
  }

  // Starting from immediate post dominator, get the 2 conditional blocks.
  *tail = Find_End_Select_Region(head, target, fall_thru);
  if (*tail == NULL) {
    if (Trace_Select_Candidates) {
      fprintf (Select_TFile, "<select> hammock BB%d: reject \n", BB_id(head));
    }
    return FALSE;
  }

  if ((target == *tail || Check_Suitable_Chain (*tail, target, t_set, allow_dup)) &&
      (fall_thru == *tail || Check_Suitable_Chain (*tail, fall_thru, ft_set, allow_dup)))
    return TRUE;
  else
    return FALSE;
}


static BOOL
Is_Hammock (BB *head, BB_SET *t_set, BB_SET *ft_set, BB **tail, bool allow_dup) 
{
	if (Is_Hammock_Legacy(head,t_set,ft_set,tail,allow_dup))
		return Check_Profitable_Select(head, t_set, ft_set, *tail);
	return FALSE;
}	

static BOOL
Is_Hammock_ft (BB *head, BB_SET *t_set, BB_SET *ft_set, BB **tail, bool allow_dup) 
{
	if (Is_Hammock_Legacy(head,t_set,ft_set,tail,allow_dup)) {
		  BB *fall_thru,*target;
		  BB_Fall_Thru_and_Target_Succs(head, &fall_thru, &target);
		  BB_SET_ClearD(t_set);
		  clear_spec_lists();
		  // No need to check the return value, this has already
		  // been checked by the call above to Is_Hammock_Legacy
		  Check_Suitable_Chain (*tail, fall_thru, ft_set, allow_dup);
		  BB *my_bb,*last_bb;
		  FOR_ALL_BB_SET_members(ft_set,my_bb) {
			  if(! BBlist_Has_One_Element(BB_preds(my_bb) )) return FALSE;
			  last_bb=my_bb;
		  }
		  //Check that the order still correct
		  if(BB_next(last_bb) != target) return FALSE;
		  return Check_Profitable_Select(head, t_set, ft_set, *tail);
	}
	return FALSE;
}



/* ================================================================
 *
 *   Select If Conversion
 *
 * ================================================================
 */
#ifdef Is_True_On
static void
Sanity_Check()
{
  for (BB *bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    OP *phi;
    BBLIST *edge;

    FOR_ALL_BB_PHI_OPs(bb, phi) {
      UINT8 count = 0;

      if (OP_opnds(phi) != BB_preds_len(bb)) {
        fprintf (TFile, "for bb BB%d\n", BB_id(bb));
        Print_All_BBs();
        DevAssert(FALSE, ("ssa: invalid phi\n"));
      }

      FOR_ALL_BB_PREDS(bb, edge) {      
        BB *bp = BBLIST_item(edge);
        BB *pred = Get_PHI_Predecessor (phi, count);

        if (pred != bp) {
          fprintf (TFile, "pred BB%d, bp BB%d at pos %d\n", BB_id(pred), BB_id(bp), count);
          fprintf (TFile, "\n+++++++++++++\n");
          Print_All_BBs();
          fprintf (TFile, "\n+++++++++++++\n");
          fprintf (TFile, "bb is: \n");
          Print_BB (bb);
          fprintf (TFile, "phi is: \n");
          Print_OP (phi);
          DevAssert (FALSE, ("ssa invalid phi operands"));
        }

        count++;
      }
    }
  }
}
#endif

/////////////////////////////////////
static void
Rename_TNs(BB* bp, hTN_MAP dup_tn_map)
{
  TN *new_tn;
  OP *op;

  FOR_ALL_BB_OPs_FWD(bp, op) {
    if (!OP_phi (op)) {
      for (INT opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
        TN *res = OP_opnd(op, opndnum);
        if (TN_is_SSA_candidate(res)) {
          new_tn = (TN*) hTN_MAP_Get(dup_tn_map, res);
          if (new_tn != NULL) {
            Set_OP_opnd(op, opndnum, new_tn);
          }
        }
      }

      if (OP_has_predicate (op) || TOP_is_select (OP_code (op))) {
        if (Need_Predicate_Op(op)) {
          new_tn = (TN*) hTN_MAP_Get(dup_tn_map, pred_i[op].tn);
	  if (new_tn != NULL) {
	    pred_i[op].tn = new_tn;
	    Set_TN_is_if_conv_cond(new_tn);
	  }
        }
      }
    }
  }
}

static void 
Update_Predicates (OP *op, OP *new_op)
{
  if (Need_Predicate_Op(op)) {
    pred_i[new_op] = pred_i[op];
    pred_i.erase(op);
  }
}

static void
Rename_PHIs(hTN_MAP dup_tn_map, BB *new_bb, BB *tail, BB *dup)
{
  OP *phi;
  op_list old_phis;

  FOR_ALL_BB_PHI_OPs(tail, phi) { 
    UINT8 nopnds = OP_opnds(phi)+1;
    TN *result[1];
    TN *opnd[nopnds];

    result[0] = OP_result(phi, 0);

    phi_t *phi_info = BB_Alloc_Phi();
    BB_Lst *bbs = &phi_info->bbs;

    TN *res1 = NULL;
    BB* tpred = Get_BB_Pos_in_PHI (phi, dup);

    for (UINT8 i = 0; i < OP_opnds(phi); i++) {
      BB *pred = Get_PHI_Predecessor (phi, i);
      TN *res = OP_opnd(phi, i);

      if (pred == tpred) {
        res1 = (TN*) hTN_MAP_Get(dup_tn_map, res);
        if (!res1) res1 = res;
      }

      bbs->push_front(pred);
      opnd[i] = res;
    }

    bbs->push_front(new_bb);
    opnd[nopnds-1] = res1;
        
    BB_Create_And_Map_Phi (nopnds, result, opnd, phi, phi_info);
  }

  BB_Update_Phis(tail);
}

// Create a phi for each GTNs use if there is not. GTN needs to be renamed
static void
Force_End_Tns (BB* bb, BB *tail)
{
  OP *op;

  if (Trace_Select_Dup) {
    fprintf (Select_TFile, "<select> Force_End_Tns %d %d\n", BB_id(bb), BB_id(tail));
    Print_BB (bb);
    Print_BB (tail);
  }

  FOR_ALL_BB_OPs_FWD(bb, op) {
    for (UINT8 defnum = 0; defnum < OP_results(op); defnum++) {
      TN *res = OP_result(op, defnum);
      TN *old_tn;

      // if the GTN is alive after the hammock, need to give it a phi.
      if (TN_is_SSA_candidate(res) && GTN_SET_MemberP(BB_live_out(tail), res)) {
        OP *phi;
        bool founddef = false;

          FOR_ALL_BB_PHI_OPs(tail, phi) {
          for (UINT8 usenum = 0; usenum < OP_opnds(phi); usenum++) {
            if (OP_opnd(phi, usenum) == res) {
              founddef = true;
              break;
            }
          }
        }
   
        if (!founddef) {
          UINT8 npreds = BB_preds_len(tail);
          TN * new_tn = Dup_TN(res);
          old_tn = res;

          Set_TN_is_global_reg (new_tn);
          SSA_unset(op);
          Set_OP_result(op, defnum, new_tn);
          SSA_setup(op);
          
          // Rename uses in current BB
          OP *op2;
          for (op2 = OP_next(op); op2 != NULL; op2 = OP_next(op2)) {
            for (UINT8 usenum = 0; usenum < OP_opnds(op2); usenum++) {
              if (OP_opnd(op2, usenum) == res)
                Set_OP_opnd(op2, usenum, new_tn);
            }
          }

          BBLIST *succs;
          FOR_ALL_BB_SUCCS(bb, succs) {
            BB *succ = BBLIST_item(succs);
            if (succ != tail) {
              FOR_ALL_BB_OPs(succ, op2) { 
                for (UINT8 usenum = 0; usenum < OP_opnds(op2); usenum++) {
                  if (OP_opnd(op2, usenum) == res)
                    Set_OP_opnd(op2, usenum, new_tn);
                }
              }
            }
          }

          // Make up a phi in the tail basic block
          TN *result[1];
          TN *opnd[npreds];
          result[0] = res;

          BB_Lst *bbs = new BB_Lst;

          BBLIST *preds;
          UINT8 pos=0;

          FOR_ALL_BB_PREDS(tail,preds) {
            BB *pred = BBLIST_item(preds);
            bbs->push_front(pred);
              opnd[pos++] = pred == bb ? new_tn : old_tn;
          }
      
          phi = Mk_VarOP (TOP_phi, 1, npreds, result, opnd);

          BB_Prepend_Op(tail, phi);
          Initialize_PHI_map(phi);

          int i=0;
          while(!bbs->empty())
            {
              BB *pred = bbs->back();
              Set_PHI_Predecessor (phi, i++, pred);
              bbs->pop_back();
            }
        }
      }
    }
  }

  if (Trace_Select_Dup) {
    fprintf (Select_TFile, "<select> After Force_End_Tns %d %d\n", BB_id(bb), BB_id(tail));
    Print_BB (bb);
    Print_BB (tail);
  }
}

static bool Interferes_Ops(OP *op1, OP *op2)
{
  op2 = OP_next(op2);

  for (int j = 0; j < OP_results(op1); j++) {
    TN *result_tn = OP_result (op1, j);
    OP *op = OP_next(op1);

    while (op && op != op2) {
      if (OP_store (op) && Are_Same_Location(op1, op2))
        return false;
      if (OP_volatile (op))
        return false;

      for (int i = 0; i < OP_opnds(op); i++) {
        if (OP_opnd(op, i) == result_tn)
          return true;
      }
      op = OP_next(op);
    }
  }
  
  return false;
}

static void 
BB_Replace_Op (OP *op, OPS *ops)
{
  BB *bb = OP_bb(op);
  OP *op_point = OP_next(op);

  BB_Remove_Op (bb, op);
  if (!op_point)
    BB_Append_Ops(bb, ops);
  else
    BB_Insert_Ops_Before(bb, op_point, ops);
}

static void BB_Sort_Psi_Ops(BB *head)
{
  OP *psi = BB_first_op(head);
  
#if 0
  std::list<OP*> old_psis;

  while (psi != NULL) {
    OP *next = OP_next(psi);

    if (OP_psi (psi)) {
      OP *npsi = PSI_inline(psi, &old_psis);
      npsi = PSI_reduce(npsi);
      if (npsi != psi) {
        OPS mops = OPS_EMPTY;  
        OPS_Append_Op (&mops, npsi);
        BB_Replace_Op (psi, &mops);
      }
    }
    psi = next;
  }

  op_list::iterator op_iter = old_psis.begin();
  op_list::iterator op_end  = old_psis.end();
  for (; op_iter != op_end; ++op_iter) {
    if (!TN_is_global_reg (OP_result (*op_iter, 0))) {
      DevAssert(head == OP_bb(*op_iter), ("psi_inline"));
      BB_Remove_Op(head, *op_iter);
    }
  }
  old_psis.clear();
  
#endif

  BB_Update_OP_Order(head);

  FOR_ALL_BB_OPs_FWD(head, psi) {
    if (OP_psi (psi) && TN_is_true(OP_opnd(psi, 0))) {
      OP *dom = TN_ssa_def(OP_opnd(psi, 1));
      if (dom && OP_bb(dom) == head) {
        for (INT i = 3; i < OP_opnds(psi); i+=2) {
          OP *op = TN_ssa_def (OP_opnd(psi, i));
          if (op && OP_bb(op) == head && OP_Precedes(op, dom)) {
            if (!Interferes_Ops(op, dom)) {
              BB_Remove_Op(head, op);
              BB_Insert_Op_After(head, dom, op);
            }
            dom = op;
          }
        }
      }
    }
  }
}

//  Copy <old_bb> and all of its ops into BB.
// bp is original bb
// to_bb is copy bb in hammock.

static void
Copy_BB_For_Duplication(BB* bp, BB* pred, BB* to_bb, BB *tail, BB_SET **bset)
{
  BB *first_bb = bp;
  hTN_MAP dup_tn_map = hTN_MAP_Create(&MEM_local_pool);

  OP *branch_op = BB_Remove_Branch (to_bb);

  // add new bb to set
  *bset = BB_SET_Union1D(*bset, to_bb, &MEM_Select_pool);

  do {
    op_list old_phis;
    OPS old_ops = OPS_EMPTY;

    if (Trace_Select_Dup) {
      fprintf (Select_TFile, "<select> Duplicating BB%d into BB%d\n",
               BB_id(bp), BB_id(to_bb));

      Print_BB (bp);
      Print_BB (tail);
    }

    // remove from set
    *bset = BB_SET_Difference1D(*bset, bp);

    //
    // Copy the ops to the new block.
    //
    OP *op;
    FOR_ALL_BB_OPs_FWD(bp, op) {
      if (OP_phi (op)) {
        UINT8 nopnds = 0;
        TN *opnd[BB_preds_len(bp)];
      
        phi_t *phi_info = BB_Alloc_Phi();
        BB_Lst *bbs = &phi_info->bbs;

        for (UINT8 i = 0; i < OP_opnds(op); i++) {
          BB *ppred = Get_PHI_Predecessor (op, i);
          if (ppred == pred) {
            OPS new_ops = OPS_EMPTY;  
            TN *res = OP_result (op, 0);
            DevAssert((OP_results(op) == 1), ("wrong res count in dup bb"));
            TN *new_tn = Dup_TN(res);
            if (TN_is_global_reg(res)) Set_TN_is_global_reg (new_tn);
            hTN_MAP_Set(dup_tn_map, res, new_tn);

            Exp_COPY (new_tn, OP_opnd (op, i), &new_ops);

            OP *op_def = TN_ssa_def(OP_opnd (op, i));
            if (op_def && Need_Predicate_Op(op_def) &&
                OP_has_predicate(OPS_last(&new_ops))) {
              TN *cond_tn = PredOp_TN(op_def);
              bool on_false = PredOp_Invert(op_def);
              Set_PredOp (OPS_last(&new_ops), cond_tn, on_false);
            }

            BB_Append_Ops (to_bb, &new_ops);
          }
          else {
            bbs->push_front(ppred);
            opnd[nopnds++] = OP_opnd (op, i);
          }
        }

        // First take care of old phis that stay out of the hammock
        // If the old phi had 2 TNs, each one is made into a copy.
        // It it had more than 2 TNs, must maintain SSA by creating a new phi.
        if (nopnds == 1) {
          Exp_COPY (OP_result (op, 0), opnd[0], &old_ops);
          old_phis.push_front(op);
        }
        else {
          // phi has only one def.
          TN *result[1];
          result[0] = OP_result (op, 0);
          BB_Create_And_Map_Phi (nopnds, result, opnd, op, phi_info);
        }
      }

      else if (!OP_br (op)) {
        OP* new_op = Dup_OP (op);

        if (TOP_is_select (OP_code (op)) || OP_has_predicate(op)) {
          Update_Predicates (op, new_op);
        }
        else if (OP_memory(op)) {
          Update_Predicates (op, new_op);
          Copy_WN_For_Memory_OP(new_op, op);
        }

        // Must rename new results before BB_Append_Op because
        // we don't want to break the ssa def mapping.
        // uses will be updated in Rename_TNs. 
        for (INT opndnum = 0; opndnum < OP_results(new_op); opndnum++) {
          TN *res = OP_result(new_op, opndnum);
          if (TN_is_SSA_candidate(res)) {
            TN *new_tn = Dup_TN(res);
            if (TN_is_global_reg(res)) Set_TN_is_global_reg (new_tn);
            hTN_MAP_Set(dup_tn_map, res, new_tn);
            Set_OP_result(new_op, opndnum, new_tn);
          }
        }

        BB_Append_Op (to_bb, new_op);
      }
    }

    BB_Remove_Ops(bp, old_phis);
    old_phis.clear();

    if (OPS_length(&old_ops) != 0) {
      BB_Prepend_Ops (bp, &old_ops);
    }
    else {
      BB_Update_Phis(bp);
    }

  } while ((bp = BB_Unique_Successor (bp)) != tail);

  // go through all definitions and rename.
  Rename_TNs (to_bb, dup_tn_map);

  // put back the branch
  if (branch_op)
    BB_Append_Op (to_bb, branch_op);

  Rename_PHIs(dup_tn_map, to_bb, tail, first_bb);

  Free_Dominators_Memory();
  Calculate_Dominators();

  if (Trace_Select_Dup) {
    fprintf (Select_TFile, "<select> After Duplication BB%d into BB%d\n",  BB_id(bp), BB_id(to_bb));
    Print_All_BBs();
  }

  dup_bbs++;
}

// Delete a BB, i.e. moving all its OPs to its predecessor
// and remove it from the list of BBs for the region/PU.
static void
Promote_BB(BB *bp, BB *to_bb)
{
  BBLIST *edge;
  
  // Sanity checks. Thoses cases cannot happen inside a hammock
  if (BB_loophead(bp) || BB_entry(bp) || BB_exit(bp)) {
    DevAssert(FALSE, ("Promote_BB"));
  }

  // Statistics
  spec_instrs_count += BB_length (bp);

  // Move all OPs from BB.
  BB_Append_All (to_bb, bp);

  // Remove from BB chain.
  Remove_BB(bp);
  
  // Remove successor edges.
  FOR_ALL_BB_SUCCS(bp, edge) {
    BB *succ = BBLIST_item(edge);
    BBlist_Delete_BB(&BB_preds(succ), bp);
  }
  BBlist_Free(&BB_succs(bp));

  // Remove predecessor edges.
  FOR_ALL_BB_PREDS(bp, edge) {
    BB *pred = BBLIST_item(edge);
    BBlist_Delete_BB(&BB_succs(pred), bp);
  }
  BBlist_Free(&BB_preds(bp));

  if (BB_call(bp)) {
    BB_Copy_Annotations(to_bb, bp, ANNOT_CALLINFO);
  }

  if (BB_has_note(bp)) {
    BB_Copy_Annotations(to_bb, bp, ANNOT_NOTE);
  }

  if (BB_asm(bp)) {
    BB_Copy_Annotations(to_bb, bp, ANNOT_ASMINFO);
  }

  BB_Copy_Annotations(to_bb, bp, ANNOT_LOOPINFO);

}

static void
Promote_BB_Chain (BB *head, BB *bb, BB *tail)
{
  if (Trace_Select_Gen) {
    fprintf(Select_TFile, "<select> promote BB%d\n", BB_id (bb));
  }

  BB *bb_next;
  do {
    bb_next = BB_Unique_Successor (bb);
    BB_Remove_Branch(bb);
    Promote_BB(bb, head);
  } while ((bb = bb_next) != tail);
}

static void
Check_Psi_Uses (OP *op, BB *bb, TN *new_tn)
{
  for (int i = 0; i < OP_results(op); i++) {
    TN *res = OP_result(op, i);
    OP *opn;

    for (opn = BB_first_op(bb); opn;  opn = OP_next(opn))
      if (OP_psi(opn))
        for (int j = 0; j < OP_opnds(opn); j++) {
          if (OP_opnd(opn, j) == res)
            Set_OP_opnd (opn, j-1, new_tn);
        }
  }
}

// update predicates with merged predicate in psis. 
static void
Rename_Cond_Tns (TN *tn, TN *new_tn, OP *op, bool on_false)
{
  BB *bb = OP_bb(op);

  FOR_ALL_BB_OPs_FWD(bb, op) {
    if (OP_psi (op))
      {
        int n = PSI_opnds(op)-1;

        for (int i = 0; i < PSI_opnds(op); i++) {
          if (PSI_guard(op, i) == tn && PSI_Pred_False(op, i) == on_false)
            if (i != n) {
              TN *tmp_guard = PSI_guard(op, n);
              TN *tmp_op = PSI_opnd(op, n);
              bool tmp_false = PSI_Pred_False(op, n);
              
              Set_PSI_guard (op, n, new_tn);
              Set_PSI_opnd (op, n, PSI_opnd(op, i));
              Set_PSI_Pred_True(op, n);
              
              Set_PSI_guard (op, i, tmp_guard);
              Set_PSI_opnd (op, i, tmp_op);
              if (tmp_false)
                Set_PSI_Pred_False(op, i);
              else
                Set_PSI_Pred_True(op, i);
            }
            else {
              Set_PSI_guard (op, i, new_tn);
              Set_PSI_Pred_True(op, i);
            }
        }
      }
  }
}

static void 
Associate_Mem_Predicates(TN *cond_tn, BOOL false_br,
                         BB_SET *t_set, BB_SET *ft_set, OPS *ops)
{
  if (pred_i.empty())
    return;

  TN *tn2=Dup_TN(cond_tn);
  bool comp_needed = false;

  // we need to maintain a second map for inverted pred'tns.
  hTN_MAP and_tn_ft_map = hTN_MAP_Create(&MEM_local_pool);
  hTN_MAP and_tn_ff_map = hTN_MAP_Create(&MEM_local_pool);
  hTN_MAP and_tn_tt_map = hTN_MAP_Create(&MEM_local_pool);
  hTN_MAP and_tn_tf_map = hTN_MAP_Create(&MEM_local_pool);

  PredOp_Map_Iter i_iter;
  PredOp_Map_Iter i_end;

  i_iter = pred_i.begin();
  i_end = pred_i.end();

  while(i_iter != i_end) {

    OP* op = PredOp_Map_Op(i_iter);
    if (BB_SET_MemberP(t_set, OP_bb (op))) {

      TN *pred_tn = PredOp_Map_Pred(i_iter).tn;

      if (!pred_tn || TN_is_true(pred_tn)) {
        if (false_br) {
#ifndef EFFECT_PRED_FALSE
          TOP ftop = CGTARG_Invert(OP_code(op));
          if (ftop == TOP_UNDEFINED) {
            PredOp_Map_Pred(i_iter).tn = tn2;
            comp_needed |= true;
          }
          else {
            OP_Change_Opcode(op, ftop);
            PredOp_Map_Pred(i_iter).tn = cond_tn;
          }
#else
          PredOp_Map_Pred(i_iter).tn = cond_tn;
          PredOp_Map_Pred(i_iter).on_false = true;
#endif

        }
        else {
          PredOp_Map_Pred(i_iter).tn = cond_tn;
        }
      }
      else {
#ifdef EFFECT_PRED_FALSE
        TN *tn = cond_tn;
#else
        TN *tn = false_br ? tn2 : cond_tn;
        comp_needed |= false_br;
#endif

        if (pred_tn != tn) {
          int pred_i = OP_find_opnd_use(op,OU_predicate);
          int on_false = OP_Pred_False(op, pred_i);

          hTN_MAP *map;
          // t_set
          if (false_br)
            map = on_false ? &and_tn_ff_map : &and_tn_ft_map;
          else
            map = on_false ? &and_tn_tf_map : &and_tn_tt_map;
            
          TN *new_tn = (TN*) hTN_MAP_Get(*map, pred_tn);

          if (!new_tn) {
            OPS mops = OPS_EMPTY;  

            new_tn = Generate_Merged_Predicates (pred_tn, tn, 
                                                 false_br ? V_BR_FALSE : V_BR_NONE,
                                                 &mops);

            hTN_MAP_Set(*map, pred_tn, new_tn);
              
            OP *mop = OPS_last(&mops);
            if (OP_Pred_False(op, pred_i)) { 
              if (OP_Pred_False(mop, 1))
                abort();
              Set_OP_Pred_False(mop, 1);
            }
            // we are in t_set
            if (false_br) {
              if (OP_Pred_False(mop, 2))
                abort();
              Set_OP_Pred_False(mop, 2);
            }

            // find first use of cond_tn or pred_tn
            OP* opb = TN_ssa_def (pred_tn);
            if (!BB_Dominates(OP_bb(TN_ssa_def(cond_tn)), OP_bb(opb))) {
              opb = TN_ssa_def(cond_tn);
            }
            BB *bb = OP_bb (opb);
            OP *last_seen_op=NULL;
            FOR_ALL_BB_OPs_FWD(bb,opb) {
              for (int i = 0; i < OP_results(opb); i++) {
                if (TNs_Are_Equivalent (OP_result(opb, i), cond_tn))
                  last_seen_op = opb;
                else if (TNs_Are_Equivalent (OP_result(opb, i), pred_tn))
                  last_seen_op = opb;
              }
            }
            BB_Insert_Ops_After(bb, last_seen_op, &mops);
          }
        }
      }
    }
    else if (BB_SET_MemberP(ft_set, OP_bb (op))) {
      TN *pred_tn = PredOp_Map_Pred(i_iter).tn;

      if (!pred_tn || TN_is_true(pred_tn)) {
        if (false_br) {
          PredOp_Map_Pred(i_iter).tn = cond_tn;
        }
        else {
#ifndef EFFECT_PRED_FALSE
          TOP ftop = CGTARG_Invert(OP_code(op));
          if (ftop == TOP_UNDEFINED) {
            Set_PredOp(PredOp_Map_Op(i_iter), tn2, false);
            comp_needed |= true;
          }
          else {
            OP_Change_Opcode(op, ftop);      
            Set_PredOp(PredOp_Map_Op(i_iter), cond_tn, false);
          }
#else
          Set_PredOp(PredOp_Map_Op(i_iter), cond_tn, true);
#endif
        }
      }
      else  {
#ifdef EFFECT_PRED_FALSE
        TN *tn = cond_tn;
#else
        TN *tn = false_br ? cond_tn : tn2;
        comp_needed |= !false_br;
#endif

        if (pred_tn != tn) {
          int pred_i = OP_find_opnd_use(op,OU_predicate);
          int on_false = OP_Pred_False(op, pred_i);

          hTN_MAP *map;
          // ft_set
          if (false_br)
            map = on_false ? &and_tn_tf_map : &and_tn_tt_map;
          else
            map = on_false ? &and_tn_ff_map : &and_tn_ft_map;

          TN *new_tn = (TN*) hTN_MAP_Get(*map, pred_tn);

          if (!new_tn) {
            OPS mops = OPS_EMPTY;  

            new_tn = Generate_Merged_Predicates (pred_tn, tn, 
                                                 false_br ? V_BR_FALSE : V_BR_NONE,
                                                 &mops);

            hTN_MAP_Set(*map, pred_tn, new_tn);

            OP *mop = OPS_last(&mops);
            if (OP_Pred_False(op, pred_i)) { 
              if (OP_Pred_False(mop, 1))
                abort();
              Set_OP_Pred_False(mop, 1);
            }
            // we are in ft_set
            if (!false_br) {
              if (OP_Pred_False(mop, 2))
                abort();
              Set_OP_Pred_False(mop, 2);
            }

            // find first use of cond_tn or pred_tn
            OP* opb = TN_ssa_def (pred_tn);
            if (!BB_Dominates(OP_bb(TN_ssa_def(cond_tn)), OP_bb(opb))) {
              opb = TN_ssa_def(cond_tn);
            }            
            BB *bb = OP_bb (opb);
            OP *last_seen_op=NULL;
            FOR_ALL_BB_OPs_FWD(bb,opb) {
              for (int i = 0; i < OP_results(opb); i++) {
                if (TNs_Are_Equivalent (OP_result(opb, i), cond_tn))
                  last_seen_op = opb;
                else if (TNs_Are_Equivalent (OP_result(opb, i), pred_tn))
                  last_seen_op = opb;
              }
            }
            BB_Insert_Ops_After(bb, last_seen_op, &mops);
          }
        }
      }
    }
    else
      DevAssert(FALSE, ("couldnt map memory op to bb"));
    
    i_iter++;
  }

  // need to reiterate thru psis to update predicates with merged ones
  i_iter = pred_i.begin();
  i_end = pred_i.end();

  while(i_iter != i_end) {
    OP *op = PredOp_Map_Op(i_iter);
    int pred_i = OP_find_opnd_use(op,OU_predicate);
    int on_false = OP_Pred_False(op, pred_i);

    hTN_MAP *map;
    if (PredOp_Map_Pred(i_iter).tn) {
      if (BB_SET_MemberP(t_set, OP_bb (op))) {
        if (false_br)
          map = on_false ? &and_tn_ff_map : &and_tn_ft_map;
        else
          map = on_false ? &and_tn_tf_map : &and_tn_tt_map;
      }
      else if (BB_SET_MemberP(ft_set, OP_bb (op))) {
        if (false_br)
          map = on_false ? &and_tn_tf_map : &and_tn_tt_map;
        else
          map = on_false ? &and_tn_ff_map : &and_tn_ft_map;
      }

      TN *new_tn = (TN*) hTN_MAP_Get(*map, PredOp_Map_Pred(i_iter).tn);

      // instruction will be updated in BB_Fix_Spec_Loads 
      if (new_tn) {
        Rename_Cond_Tns(PredOp_Map_Pred(i_iter).tn, new_tn, op, on_false);
        PredOp_Map_Pred(i_iter).tn = new_tn;
#ifdef EFFECT_PRED_FALSE
        PredOp_Map_Pred(i_iter).on_false = false;
#endif
        Set_OP_Pred_True(op, OP_find_opnd_use(op, OU_predicate));
      }
    }

    i_iter++;
  }

  if (comp_needed) {
#ifdef TARG_STxP70
    /* if cannot invert into one op, build a new cmp.
       move into Exp_Pred_Complement */
    OP* cond_op = TN_ssa_def (cond_tn);
    if (cond_op ) {
      OP* new_op = CGTARG_Negate_OP(cond_op);
      Set_OP_result(new_op, 0, tn2);
      OPS_Append_Op(ops, new_op);
    }
    else
#endif
      Exp_Pred_Complement(tn2, NULL, cond_tn, ops);
  }
}

static bool
Can_Mem_Conflicts(OP *op1, OP *op2)
{
  OP *first;
  OP *last;

  if (OP_Follows(op2, op1)) {
    first = op1;
    last = op2;
  }
  else {
    first = op2;
    last = op1;
  }

  for (OP *iop = OP_next(first);
       iop!= NULL && iop != last;
       iop = OP_next(iop)) 
    if (OP_store (iop) && !Are_Not_Aliased (first, iop))
      return true;
    
  return false;
}

static void
Promote_Mem_Based_On (int index, OP *psi_op)
{
  TN *sel_tn;
  OP *op1 = TN_ssa_def (PSI_opnd (psi_op, 0));
  OP *op2 = TN_ssa_def (PSI_opnd (psi_op, 1));
  TN *cond_TN0 = OP_opnd (op1, OP_find_opnd_use(op1, index));
  TN *cond_TN1 = OP_opnd (op2, OP_find_opnd_use(op2, index));

  if (TN_is_register(cond_TN0))
    sel_tn = Dup_TN (cond_TN0);
  else
    sel_tn = CGTARG_select_TN (TN_size(cond_TN0));

  BB *bb = OP_bb(psi_op);
  OPS ops = OPS_EMPTY;  

  DevAssert(BB_id(OP_bb(psi_op)) == BB_id(OP_bb(op1)) &&
            BB_id(OP_bb(psi_op)) == BB_id(OP_bb(op2)),
            ("wrong candidate for memory promotion"));
  
  DevAssert(TN_size(cond_TN0) == TN_size(cond_TN1),
            ("wrong candidate for memory promotion"));
            
  DevAssert (PSI_opnds (psi_op) == 2,
             ("wrong candidate for memory promotion"));

  Expand_Select (sel_tn, PSI_guard(psi_op, 0), cond_TN0, cond_TN1, 
                 TN_size(cond_TN0) == 8 ? MTYPE_I8: MTYPE_I4,
                 FALSE, &ops);

  if (OP_Precedes(op1, op2)) {
    OP *tmp = op1;
    op1 = op2;
    op2 = tmp;
  }

  if (OP_no_alias (op1) && !OP_no_alias (op2))
    Reset_OP_no_alias(op1);

  OP_MAP_Set(OP_to_WN_map, op1, NULL);
  TN *psi_res = OP_result(psi_op, 0);

  // First remove psi_op, it also unsets the SSA link
  BB_Remove_Op (bb, op2);
  BB_Remove_Op (bb, psi_op);

  Set_OP_result(op1, 0, psi_res);
  Set_OP_opnd(op1, OP_find_opnd_use(op1, index), sel_tn);
  SSA_setup(op1);

  BB_Insert_Ops_Before(bb, op1, &ops);
}

static void
Optimize_Spec_Loads(BB *bb)
{
  PredOp_Map_Iter i_iter;
  PredOp_Map_Iter i_end;

  i_iter = pred_i.begin();
  i_end = pred_i.end();

  BB_Update_OP_Order(bb);

 next_load:

  while(i_iter != i_end) {

    PredOp_Map_Iter i_iter2=i_iter;
    i_iter2++;

    OP *op1 = PredOp_Map_Op(i_iter);
    TN *tn1 = PredOp_Map_Pred(i_iter).tn;

    if (OP_store (op1)) {
      i_iter++;
      continue;
    }

    while (i_iter2 != i_end) {
      OP *op2 = PredOp_Map_Op(i_iter2);
      TN *tn2 = PredOp_Map_Pred(i_iter2).tn;

      if (OP_load (op2) && 
          Are_Same_Location (op1, op2) && tn1 != tn2 &&
          !Need_Predicate_Op(op1) && !Need_Predicate_Op(op2)) {

        // check that we don't cross an aliasing memory operation
        if (Can_Mem_Conflicts(op1, op2)) {
          i_iter++;
          goto next_load;
        }

        OPS ops = OPS_EMPTY;  
        if (OP_Precedes(op1, op2)) {
          TN *res = OP_result(op1, 0);
          TN *new_res = OP_result(op2, 0);
          Exp_COPY (new_res, res, &ops);
          BB_Replace_Op (op2, &ops);
          i_iter2 = pred_erase(i_iter2);
          i_iter = pred_erase(i_iter);
          goto next_load;
        }
        else {
          TN *res = OP_result(op2, 0);
          TN *new_res = OP_result(op1, 0);
          Exp_COPY (new_res, res, &ops);
          BB_Replace_Op (op1, &ops);
          i_iter2 = pred_erase(i_iter2);
          i_iter = pred_erase(i_iter);
          goto next_load;
        }
      }

      else if (OP_load (op2) && 
               Have_Same_Offset_Or_Base (op1, op2) && tn1 != tn2 &&
               !OP_has_predicate(op1) && !OP_has_predicate(op2)) {

        // check that we don't cross an aliasing memory operation
        if (Can_Mem_Conflicts(op1, op2)) {
          i_iter++;
          goto next_load;
        }

        TN* res1 = OP_result(op1, 0);
        TN* res2 = OP_result(op2, 0);
        bool found1 = false;
        bool found2 = false;

        // Does those loads merge ?
        // (we could look at tn1 compl tn2 instead)
        OP *psi;
        FOR_ALL_BB_OPs_FWD(bb, psi) {
          if (OP_psi (psi)) {
            if (PSI_opnds(psi) == 2) {
              for (int i = 0; i < PSI_opnds(psi); i++) {
                if (PSI_opnd(psi, i) == res1) {
                  found1 = true;
                  if (found2) goto found;
                }
                else if (PSI_opnd(psi, i) == res2) {
                  found2 = true;
                  if (found1) goto found;
                }
              }

              found1 = found2 = false;
            }
            else {
              // todo: reduce phi if nops > 2
              DevWarn("promote_memory with phis. not yet");
            }
          }
          else {
            // cannot remove op1 or op2 if they are alive inside the bb,
            // beside the psi,
              for (INT i = 0; i < OP_opnds(psi); i++) {
                TN *use = OP_opnd (psi, i);
                if (TN_is_register (use)) {
                  if (TNs_Are_Equivalent (res1, use) || TNs_Are_Equivalent (res2, use)) {
                    i_iter++;
                    goto next_load;
                  }
                }
              }
          }
        }
        
      found:
        if (found1 && found2) {
          TN *offsetTN1 = OP_opnd(op1, OP_find_opnd_use(op1, OU_offset));
          TN *offsetTN2 = OP_opnd(op2, OP_find_opnd_use(op2, OU_offset));
          TN *baseTN1   = OP_opnd(op1, OP_find_opnd_use(op1, OU_base));
          TN *baseTN2   = OP_opnd(op2, OP_find_opnd_use(op2, OU_base));

          if(Same_Reg_Or_Value (offsetTN1, offsetTN2)) {
            Promote_Mem_Based_On (OU_base, psi);

            i_iter2 = pred_erase(i_iter2);
            i_iter = pred_erase(i_iter);
            goto next_load;         
          }
          else if(Same_Reg_Or_Value (baseTN1, baseTN2)) {
            Promote_Mem_Based_On (OU_offset, psi);
            
            i_iter2 = pred_erase(i_iter2);
            i_iter = pred_erase(i_iter);
            goto next_load;         
          }
        }
      }

      i_iter2++;
    }
        
    i_iter++;
  }
}

static void
BB_Fix_Spec_Loads (BB *bb)
{
  if (CGTARG_Can_Select ())
    Optimize_Spec_Loads (bb);

  PredOp_Map_ConstIter i_iter;
  PredOp_Map_ConstIter i_end;

  i_iter = pred_i.begin();
  i_end = pred_i.end();
  while(i_iter != i_end) {

    OP* op = PredOp_Map_Op(i_iter);

#ifdef TARG_ST200
    if (OP_load (op)) {

      if (Can_Be_Dismissible (op)) { 
        TOP ld_top = CGTARG_Speculative_Load (op);
        DevAssert(ld_top != TOP_UNDEFINED, ("couldnt find a speculative load"));
        DevAssert(!OP_volatile (op), ("cannot speculate a load"));

        OP_Change_Opcode(op, ld_top); 
        Set_OP_speculative(op);  
      }

      else if (PROC_has_predicate_loads()) {
        TN *btn = PredOp_Map_Pred(i_iter).tn;

        if (!TN_is_true(btn)) {
          if (OP_has_predicate (op))
            CGTARG_Predicate_OP (NULL, op, btn, false);
          else {
            TOP ld_top = CGTARG_Predicated_Load (op);
            OPS ops = OPS_EMPTY;  
        
            Build_OP (ld_top, OP_result(op,0),
                      btn,
                      OP_opnd(op, OP_find_opnd_use(op, OU_offset)),
                      OP_opnd(op, OP_find_opnd_use(op, OU_base)),
                      &ops);

            Copy_WN_For_Memory_OP(OPS_last(&ops), op);

            if (OP_volatile (op))
              Set_OP_volatile(OPS_last(&ops));

            BB_Replace_Op (op, &ops);

            Check_Psi_Uses(op, bb, btn);
          }
        }
      }
    }
    else if (OP_prefetch (op)) {
      TN *btn = PredOp_Map_Pred(i_iter).tn;

      if (OP_has_predicate (op))
        CGTARG_Predicate_OP (NULL, op, btn, false);
      else {
        OPS ops = OPS_EMPTY;  
        TOP ld_top = CGTARG_Predicated_Load (op);

        Build_OP (ld_top, btn, 
                  OP_opnd(op, OP_find_opnd_use(op, OU_offset)),
                  OP_opnd(op, OP_find_opnd_use(op, OU_base)),
                  &ops);

        Copy_WN_For_Memory_OP(OPS_last(&ops), op);
        BB_Replace_Op (op, &ops);
      }
    }
#elif TARG_STxP70
    {
      TN *btn = PredOp_Map_Pred(i_iter).tn;

      CGTARG_Predicate_OP(bb, op, btn, PredOp_Map_Pred(i_iter).on_false);
#if 0
      OP *psi;
      FOR_ALL_BB_OPs_FWD(bb, psi) {
        if (OP_psi (psi)) {
          for (int i = 0; i < OP_opnds(psi); i++) {
            if (OP_opnd(psi, i) == OP_result(op, 0)) {
              Set_OP_opnd (psi, i-1, btn);
              if (PredOp_Map_Pred(i_iter).on_false)
                Set_OP_Pred_False(psi,i-1);
            }
          }
        }
      }
#endif
    }
#endif

    i_iter++;
  }
}

static void
Optimize_Spec_Stores(BB *bb)
{
  PredOp_Map_Iter i_iter;
  PredOp_Map_Iter i_end;

  i_iter = pred_i.begin();
  i_end = pred_i.end();

  BB_Update_OP_Order(bb);

  next_store:

  while(i_iter != i_end) {

    PredOp_Map_Iter i_iter2=i_iter;
    i_iter2++;

    OP *op1 = PredOp_Map_Op(i_iter);
    TN *tn1 = PredOp_Map_Pred(i_iter).tn;

    if (OP_load (op1)) {
      i_iter++;
      continue;
    }

    while (i_iter2 != i_end) {

      OP *op2 = PredOp_Map_Op(i_iter2);
      TN *tn2 = PredOp_Map_Pred(i_iter2).tn;
      
      if (OP_store (op2) && 
          Are_Same_Location (op1, op2) && tn1 != tn2 &&
          !OP_has_predicate(op1) && !OP_has_predicate(op2)) {
        OPS ops = OPS_EMPTY;  
          
        // check that we don't cross an aliasing memory operation
	  OP *first;
	  OP *last;
          if (OP_Follows(op2, op1)) {
	    first = op1;
	    last = op2;
	  }
	  else {
	    first = op2;
	    last = op1;
	  }

          // address from op1 will be used for merge stores. Check that it is always
          // defined inside the considered region.
          TN *base1, *offset1;
          (void) OP_Base_Offset_TNs (op1, &base1, &offset1);

          // This optimisation implies to move the first store down to the last one.
	  for (OP *iop = OP_next(first);
	       iop!= NULL && iop != last;
	       iop = OP_next(iop)) {
	    if (OP_memory (iop)) {
              if (!Are_Not_Aliased (first, iop)) {
                i_iter++;
                goto next_store;
              }
            }

            // this fixes conflicts introduced when disabling the pre. see #35792
            // Check now that we are not inserting a store for which the base is 
            // conditionally defined.
            if (!Always_Locally_Defined (base1, bb) ||
                !Always_Locally_Defined (offset1, bb)) {
                i_iter++;
                goto next_store;
            }
          }

          Expand_Cond_Store (tn1, op1, op2, OP_find_opnd_use(op1, OU_storeval),
                             &ops);
          
          Copy_WN_For_Memory_OP(OPS_last(&ops), op1);

          if (Trace_Select_Gen) {
            
            fprintf(Select_TFile, "<select> Insert selects stores in BB%d", BB_id(bb));
            fprintf(Select_TFile, "<select> From OPs \n");
            Print_OP (op1); Print_OP (op2);
            fprintf(Select_TFile, "<select> From bTNs \n");
            Print_TN (tn1, FALSE); Print_TN (tn2, FALSE);
            fprintf(Select_TFile, "\n");
            Print_OPS (&ops); 
            fprintf (Select_TFile, "\n");

            fprintf(Select_TFile, "<select> True_TN is \n");
            Print_TN (True_TN, FALSE);
          }

          BB_Replace_Op (last, &ops);
          BB_Remove_Op (bb, first);

          i_iter2 = pred_erase(i_iter2);
          i_iter = pred_erase(i_iter);
          goto next_store;
        }
        else
          i_iter2++;
      }
      i_iter++;
    }
}

static void
BB_Fix_Spec_Stores (BB *bb)
{
  if (CG_select_merge_stores)
    Optimize_Spec_Stores (bb);

  PredOp_Map_ConstIter i_iter;
  PredOp_Map_ConstIter i_end;

  i_iter = pred_i.begin();
  i_end = pred_i.end();

  while(i_iter != i_end) {

    OP* op = PredOp_Map_Op(i_iter);
    TN *btn = PredOp_Map_Pred(i_iter).tn;

    if (OP_store (op)) {
      if (PROC_has_predicate_stores() && Enable_Conditional_Store) {
        if (OP_has_predicate (op))
          CGTARG_Predicate_OP (NULL, op, btn, false);
        else {
          OPS ops = OPS_EMPTY;  
          TOP st_top = CGTARG_Predicated_Store (op);

          Build_OP (st_top,
                    OP_opnd(op, OP_find_opnd_use(op, OU_offset)),
                    OP_opnd(op, OP_find_opnd_use(op, OU_base)),
                    btn,
                    OP_opnd(op, OP_find_opnd_use(op, OU_storeval)),
                    &ops);

          Copy_WN_For_Memory_OP(OPS_last(&ops), op);

          if (OP_volatile (op))
            Set_OP_volatile(OPS_last(&ops));

          BB_Replace_Op (op, &ops);
        }
      }
      else {
        OPS ops = OPS_EMPTY;  
        UINT8 opnd_idx = OP_find_opnd_use(op, OU_base);
        DevAssert(!OP_volatile (op), ("cannot speculate a store"));
        Expand_Cond_Store (btn, op, NULL, opnd_idx, &ops);

        Copy_WN_For_Memory_OP(OPS_last(&ops), op);

        BB_Replace_Op (op, &ops);
      }

    }
    i_iter++;
  }
}

static BOOL
Negate_Cmp_BB (OP *br)
{
  TN *btn = OP_opnd(br, 0);
  OPS new_ops = OPS_EMPTY;

  OP *cmp_op = TN_ssa_def(btn);
  OP *new_op,*tmp_op;

  if (cmp_op && (BB_id (OP_bb(br)) == BB_id (OP_bb (cmp_op))) &&
      (!GTN_SET_MemberP(BB_live_out(OP_bb (br)), btn)) && 
      (new_op = CGTARG_Negate_OP(cmp_op)) != NULL) {
    Set_OP_result(new_op, 0, btn);
    OPS_Prepend_Op(&new_ops, new_op);
    BB_Replace_Op (cmp_op, &new_ops);
    // [TDR] Fix for bug #92132/#93352: Check if the result TN of the compare is 
    // not used somewhere else in the BB
    for (tmp_op = OP_next(new_op); tmp_op != NULL && tmp_op != br; tmp_op = OP_next(tmp_op)) {
      for (int i = 0; i < OP_opnds(tmp_op); i++) {
        if (OP_opnd(tmp_op, i) == btn) { 
            Invert_OP_Pred(tmp_op,i);
        }
      }
    }
  }
  else {
    TN *tn2=Dup_TN(btn);
    Exp_Pred_Complement(tn2, NULL, btn, &new_ops);
    BB_Insert_Ops_Before(OP_bb(br), br, &new_ops);
    Set_OP_opnd(br, 0, tn2);
  }

  return TRUE;
}

static BOOL
Prep_And_Normalize_Jumps(BB *bb1, BB *bb2, BB *fall_thru1, BB *target1,
                         BB **fall_thru2, BB **target2, BOOL *cmp_invert)
{
  OP *br1 = BB_branch_op (bb1);
  OP *br2 = BB_branch_op (bb2);

  FmtAssert(br1 && OP_cond(br1),
            ("BB%d doesn't end in a conditional branch.", BB_id(bb1)));
  FmtAssert(br2 && OP_cond(br2),
            ("BB%d doesn't end in a conditional branch.", BB_id(bb2)));

  TN *dummy;
  VARIANT variant1 = CGTARG_Analyze_Branch(br1, &dummy, &dummy);
  VARIANT variant2 = CGTARG_Analyze_Branch(br2, &dummy, &dummy);

  BOOL needInvert = V_false_br (variant1) != V_false_br (variant2);

  BB_Fall_Thru_and_Target_Succs(bb2, fall_thru2, target2);

  if (target1 == *target2 || fall_thru1 == *fall_thru2) {
    *cmp_invert = needInvert;
    if (Trace_Select_Gen) {
      fprintf (Select_TFile, "\nNormalize Jumps OK\n");
    }
    return TRUE;
  }

  if (target1 == *fall_thru2 || fall_thru1 == *target2) {
    BB *tmpbb = *fall_thru2;
    *fall_thru2 = *target2;
    *target2 = tmpbb;

    logif_info *logif = (logif_info *)BB_MAP_Get(if_bb_map, bb2);
    DevAssert(logif, ("Select: unmapped logif"));

    logif->target = *target2;
    logif->fall_thru = *fall_thru2;

    // if needInvert, just swap the branches
    *cmp_invert = !needInvert;

    if (Trace_Select_Gen) {
      fprintf (Select_TFile, "\nNormalize Jumps OK (invert)\n");
    }

    return TRUE;
  }

  if (Trace_Select_Gen) {
    fprintf (Select_TFile, "\nNormalize Jumps failed\n");
  }

  return FALSE;
}

// Test if bb is the head of a region that can be converted with logical
// expressions. (ex: if (p1) if (p0) ... = if (p1 && p0 && ...)
// Returns the head of the second if region or null
//
// A well formed logical expression is a sequence of 2 blocks containing
// a conditional jump, for which one direct successor is in common.
// This is done regardless of the branches directions, as it is
// possible (with respect to the available ops) to transform any pattern in
// a pure and or or expression.
// Normalizejumps will try to retrieve such a pattern.
static BB*
Is_Double_Logif(BB* bb)
{
  BB *fall_thru, *target, *sec_fall_thru, *sec_target;
  BOOL cmp_invert;

  // Find fall_thru and taken BBs.
  BB_Fall_Thru_and_Target_Succs(bb, &fall_thru, &target);

  if (BB_kind(target) == BBKIND_LOGIF) {
    if (BB_preds_len(target) == 1
        && Can_Speculate_BB(target)
        && Prep_And_Normalize_Jumps(bb, target, fall_thru, target,
                                    &sec_fall_thru, &sec_target, &cmp_invert)
        && ((sec_fall_thru == fall_thru)
            || (cmp_invert && sec_target == fall_thru))) {

      if (! Check_Profitable_Logif(bb, target) ||
          (cmp_invert && ! Negate_Cmp_BB(BB_branch_op (target)))) {
        BB_MAP_Set(if_bb_map, target, NULL);
        return NULL;
      }

      return target;
    }

    BB_MAP_Set(if_bb_map, target, NULL); 
  }

  // try the other side
  if (BB_kind(fall_thru) == BBKIND_LOGIF) {
    if (BB_preds_len(fall_thru) == 1
        && Can_Speculate_BB(fall_thru)
        && Prep_And_Normalize_Jumps(bb, fall_thru, fall_thru, target,
                                    &sec_fall_thru, &sec_target, &cmp_invert)
        && ((sec_target == target)
            || (cmp_invert && sec_fall_thru == target))) {

      if (! Check_Profitable_Logif(bb, fall_thru) ||
          (cmp_invert && ! Negate_Cmp_BB(BB_branch_op (fall_thru)))) {
        BB_MAP_Set(if_bb_map, fall_thru, NULL);
        return NULL;
      }

      return fall_thru;
    }

    BB_MAP_Set(if_bb_map, fall_thru, NULL); 
  }
  
  return NULL;
}

static void 
Simplify_Logifs(BB *bb1, BB *bb2)
{
  BB *bb1_fall_thru, *bb1_target, *bb2_fall_thru, *bb2_target;
  BB *else_block, *joint_block;
  BOOL AndNeeded;

  // Get condif cached information
  BB_Fall_Thru_and_Target_Succs(bb1, &bb1_fall_thru, &bb1_target);
  BB_Fall_Thru_and_Target_Succs(bb2, &bb2_fall_thru, &bb2_target);

  /*  [VCdV]
     We need to invalidate trip_count_tn info if bb2 is the last BB of
     a loop. In fact, in this case, the condition for the loop and the
     condition for an early exit are merged and as a consequence the
     trip_count_tn is no longer valid. The loop is no longer countable !
  */
  BB* loop_head = BB_loop_head_bb(bb2);
  if (loop_head!=NULL &&
      ((loop_head==bb2_fall_thru) || (loop_head==bb2_target)))  {
    ANNOTATION *annot = ANNOT_Get(BB_annotations(loop_head), ANNOT_LOOPINFO);
    if (annot!=NULL) {
      LOOPINFO_primary_trip_count_tn(ANNOT_loopinfo(annot)) = NULL;
    }
  }

  if (Trace_Select_Gen) {
    fprintf (Select_TFile, "in function %s\n", Cur_PU_Name);
    fprintf (Select_TFile, "\nStart gen logical from BB%d \n", BB_id(bb1));
  }

  // final test variant
  VARIANT variant = V_BR_P_TRUE;

  // Check optimisation type.
  if (bb1_fall_thru == bb2_fall_thru) {
    // if (!a || !b)
    AndNeeded = TRUE;
    Set_V_false_br (variant);
    else_block = bb2_target;
    joint_block = bb2_fall_thru;
  }
  else if (bb1_target == bb2_target) {
    // if (a || b)
    AndNeeded = FALSE;
    else_block = bb2_fall_thru;
    joint_block = bb2_target;
  }
  else
    DevAssert(FALSE, ("Simplify_Logifs unknown operation"));

  // Compute probability for the new bb1->else edge
  // prob(bb1->else) = prob(bb1->bb2) * prob(bb2->else)
  // Note that for the other edge, we have:
  // prob(bb1->joint) = 1 - prob(bb1->else)
  float prob = 0.0;
  float prob2 = 0.0;

  BBLIST *bblist;
  FOR_ALL_BB_SUCCS(bb1, bblist) {
    BB *succ = BBLIST_item(bblist);
    if (succ == joint_block) 
      prob += BBLIST_prob(bblist);
    else 
      prob2 += BBLIST_prob(bblist); 
  }
  FOR_ALL_BB_SUCCS(bb2, bblist) {
    BB *succ = BBLIST_item(bblist);
    if (succ == joint_block) {
      prob2 *= BBLIST_prob(bblist);
      break;
    }
  }
  
  prob += prob2;

  if (Trace_Select_Gen) {
    fprintf (Select_TFile, "Convert if BB%d BB%d BB%d BB%d BB%d BB%d\n",
             BB_id(bb1), BB_id(bb1_fall_thru), BB_id(bb1_target),
             BB_id(bb2), BB_id(bb2_fall_thru), BB_id(bb2_target));
  }

  OP *br1_op = BB_branch_op(bb1);
  OP *br2_op = BB_branch_op(bb2);

  // get compares return values.
  TN *branch_tn1, *branch_tn2;
  TN *reg_tn1, *reg_tn2;
  TN *dummy;
  VARIANT variant1 = CGTARG_Analyze_Branch(br1_op, &branch_tn1, &dummy);
  CGTARG_Analyze_Branch(br2_op, &branch_tn2, &dummy);

  LABEL_IDX targ_label = Gen_Label_For_BB(joint_block);
  TN *label_tn = Gen_Label_TN(targ_label, 0);

  // instructions from bb2 will be promoted. check mem predicates are set.
  BB_SET *t_set = BB_SET_Create_Empty(2, &MEM_Select_pool);
  BB_SET *ft_set = BB_SET_Create_Empty(2, &MEM_Select_pool);
  t_set = BB_SET_Union1D(t_set, bb2, &MEM_Select_pool);
  ft_set = BB_SET_Union1D(ft_set, else_block, &MEM_Select_pool);

  // !a or !b -> !(a and b)
  if (V_false_br (variant1)) {
    if (V_false_br (variant))
      Set_V_true_br (variant);
    else
      Set_V_false_br (variant);
    AndNeeded = !AndNeeded;
  }

  OPS ops = OPS_EMPTY;

  // now associate with each BB the predicate tn
  // BB2 is dependent from condition computed in BB1 if computing an 'and'
  Associate_Mem_Predicates(branch_tn1,
                           (bb1_target==bb2 && V_false_br (variant1)) ||
                           (bb1_target!=bb2 && !V_false_br (variant1)),
                           t_set, ft_set, &ops);

  // add computation of !test
  BB_Insert_Ops_Before(bb1, br1_op, &ops);

  OPS_Init(&ops);

  BB_Remove_Branch(bb1);
  BB_Remove_Branch(bb2);
  Promote_BB(bb2, bb1);

  reg_tn1 = Expand_CMP_Reg (branch_tn1, &ops);
  reg_tn2 = Expand_CMP_Reg (branch_tn2, &ops);

  // we need a branch_tn that will be the result of the logical op
  TN *new_branch_tn = Build_TN_Like (OP_opnd(br1_op, 0));

  // joint_block will be the branch target.
  BOOL invert_branch = FALSE;

  if (BB_next (bb1) == joint_block) {
    if (V_false_br (variant))
      Set_V_true_br (variant);
    else
      Set_V_false_br (variant);
    invert_branch = TRUE;
  }

  // Update control flow 
  Unlink_Pred_Succ (bb1, bb2);
  Unlink_Pred_Succ (bb1, joint_block);
  Unlink_Pred_Succ (bb2, joint_block);
  Unlink_Pred_Succ (bb2, else_block);

  // insert new logical op.
  if (AndNeeded)
    Expand_Logical_And (new_branch_tn, reg_tn1, reg_tn2, V_BR_P_TRUE, &ops);
  else
    Expand_Logical_Or (new_branch_tn, reg_tn1, reg_tn2, V_BR_P_TRUE, &ops);

  // Stats
  logif_count++;

  Expand_Branch (label_tn, new_branch_tn, NULL, variant, &ops);

  BB_Append_Ops (bb1, &ops);


  if (invert_branch)
    Target_Logif_BB(bb1, else_block, 1.0F - prob, joint_block);
  else
    Target_Logif_BB(bb1, joint_block, prob, else_block);

  BB_MAP_Set(if_bb_map, bb1, NULL);
  BB_MAP_Set(if_bb_map, bb2, NULL);
    
  // replace phis in joint_block with a new reduced one.
  BB_Recomp_Phis(joint_block, bb1, branch_tn1, bb2, branch_tn2, !V_false_br(variant1) && !AndNeeded);

  // Commit dismissible loads and stores
  BB_Fix_Spec_Loads (bb1);

  if (PROC_is_select())
    BB_Fix_Spec_Stores (bb1);

  OP *phi;
  FOR_ALL_BB_PHI_OPs(else_block, phi) {
    TN *result[1];
    UINT8 nopnds = OP_opnds(phi);
    TN *opnd[nopnds];
    UINT8 j=0;

    phi_t *phi_info = BB_Alloc_Phi();
    BB_Lst *bbs = &phi_info->bbs;

    for (UINT8 i = 0; i < nopnds; i++) {
      BB *pred = Get_PHI_Predecessor (phi, i);
      opnd[j++] = OP_opnd(phi, i);
      bbs->push_front(pred == bb2 ? bb1 : pred);
    }

    result[0] = OP_result(phi, 0);
    BB_Create_And_Map_Phi (nopnds, result, opnd, phi, phi_info);
  }

  BB_Update_Phis(else_block);

  BB_Sort_Psi_Ops(bb1);

  if (Trace_Select_Gen) {
    fprintf (Select_TFile, "\nEnd gen logical from BB%d \n", BB_id(bb1));
    Print_All_BBs();
  }
}

/* ================================================================
 *
 *   If flattening
 *
 * ================================================================
 */
//TDR
static void
Select_Fold_ft (BB *head, BB_SET *ft_set, BB *tail)
{
  OP *phi;
  BB *target_bb, *fall_thru_bb;
  BB_Fall_Thru_and_Target_Succs(head, &fall_thru_bb, &target_bb);

  if (Trace_Select_Gen) {
    fprintf (TFile, "in function %s\n", Cur_PU_Name);
    fprintf (TFile, "\nStart Select_Fold FT from BB%d\n", BB_id(head));
    fprintf (TFile, "\n fall_thrus are\n");
    BB_SET_Print (ft_set, Select_TFile);
    fprintf (TFile, "\n tail is\n");
    Print_BB_Header (tail, TRUE, TRUE);
  }

  TN *cond_tn1,*cond_tn2;
  OP *br_op = BB_branch_op(head);
  VARIANT variant = CGTARG_Analyze_Branch(br_op, &cond_tn1, &cond_tn2);

  // can remove the branch op
  BB_Remove_Op(head, br_op);
  BB * my_bb;
  FOR_ALL_BB_SET_members(ft_set,my_bb) {
	  OP * op= BB_first_op(my_bb);
	  for ( ; op; op = OP_next(op)) {
	      int p_i = OP_find_opnd_use(op, OU_predicate);
	      Set_OP_opnd(op,p_i,cond_tn1);
	      Set_OP_Pred_False(op,p_i);
	  }
	  //Update Phis functions in tail block.
	  FOR_ALL_BB_PHI_OPs(tail, phi) {
        Change_PHI_Predecessor (phi, my_bb, head);
      }
	  
  }
  Promote_BB_Chain (head, BB_SET_Choose(ft_set), tail);
  //Report the conditional jump to tail block
  Add_Goto(head,tail);
  br_op = BB_branch_op(head);
  int p_i = OP_find_opnd_use(br_op, OU_condition);
  Set_OP_opnd(br_op,p_i,cond_tn1);
  Set_OP_Pred_False(br_op,p_i);

  /*Update Probability*/
  BBLIST *edge = BB_Find_Succ(head, target_bb);
  Link_Pred_Succ_with_Prob(head, tail, 1 - BBLIST_prob(edge), FALSE, TRUE);

  //Update phis
  variant = CGTARG_Analyze_Branch(br_op, &cond_tn1, &cond_tn2);

   if (Trace_Select_Gen) {
	   fprintf (TFile, "\nEnd Select_Fold_FT from BB%d\n", BB_id(head));
	   Print_All_BBs();
   }
}


static void
Select_Fold (BB *head, BB_SET *t_set, BB_SET *ft_set, BB *tail)
{
  OP *phi;
  BB *target_bb, *fall_thru_bb;

  BB_Fall_Thru_and_Target_Succs(head, &fall_thru_bb, &target_bb);

  if (Trace_Select_Gen) {
    fprintf (TFile, "in function %s\n", Cur_PU_Name);
    fprintf (TFile, "\nStart Select_Fold from BB%d\n", BB_id(head));

    fprintf (TFile, "\n fall_thrus are\n");
    BB_SET_Print (ft_set, Select_TFile);
    
    fprintf (TFile, "\n targets are\n");
    BB_SET_Print (t_set, Select_TFile);

    fprintf (TFile, "\n tail is\n");
    Print_BB_Header (tail, TRUE, TRUE);
  }
   
  // keep a list of newly created conditional move / compare
  // and phis to remove
  OPS cmov_ops = OPS_EMPTY;
  OPS ops = OPS_EMPTY;
  op_list old_phis;

  TN *cond_tn;
  TN *tn2;

  BOOL edge_needed = (target_bb != tail && fall_thru_bb != tail);

   // find last bb before tail.
   BBLIST* pedge;
   BB *last_target = target_bb;
   BB *last_fall_thru = fall_thru_bb;

   FOR_ALL_BB_PREDS (tail, pedge) {
     BB *pred = BBLIST_item(pedge);

     if (target_bb != tail)
       if (BB_SET_MemberP(t_set, pred))
         last_target = pred;

     if (fall_thru_bb != tail)
       if (BB_SET_MemberP(ft_set, pred))
         last_fall_thru = pred;
   }

  if (fall_thru_bb != tail) {
    BB *pred = last_fall_thru;

    while (pred != head) {
      BB *other_pred = head;

      FOR_ALL_BB_PREDS (pred, pedge) {
        BB *pired = BBLIST_item(pedge);

        if (BB_SET_MemberP(ft_set, pired))
          other_pred = pired;
      }

      BB *phi_pred = other_pred;

      Force_End_Tns (pred, tail);

      if (BB_preds_len (pred) > 1) {
        other_pred = Gen_And_Insert_BB_After (phi_pred);
        if (!BB_Retarget_Branch(phi_pred, pred, other_pred))
          Change_Succ(phi_pred, pred, other_pred);
        Add_Goto (other_pred, tail);
        if (pred == fall_thru_bb) {
          fall_thru_bb = other_pred;
        }

        Copy_BB_For_Duplication(pred, phi_pred, other_pred, tail, &ft_set);
      }

      pred = phi_pred;
    }
  }

  if (target_bb != tail) {
    BB *pred = last_target;

    while (pred != head) {
      BB *other_pred = head;

      FOR_ALL_BB_PREDS (pred, pedge) {
        BB *pired = BBLIST_item(pedge);

        if (BB_SET_MemberP(t_set, pired))
          other_pred = pired;
      }

      BB *phi_pred = other_pred;

      Force_End_Tns (pred, tail);

      if (BB_preds_len (pred) > 1) {
        BB *succ = BB_Fall_Thru_Successor(head);
        other_pred = Gen_And_Insert_BB_After (phi_pred);

        if (!BB_Retarget_Branch(phi_pred, pred, other_pred))
          Change_Succ(phi_pred, pred, other_pred);

        if (phi_pred == head) {
          // don't change the fallthru
          BB *new_succ = Gen_And_Insert_BB_After (head); 
          fall_thru_bb = new_succ;
          Add_Goto (new_succ, succ);
          Change_Succ(phi_pred, succ, new_succ);
          if (succ == tail) {
            OP *phi;
            FOR_ALL_BB_PHI_OPs(tail, phi) {
              Change_PHI_Predecessor (phi, phi_pred, new_succ);
            }
          }
        }

        Add_Goto (other_pred, tail);
        if (pred == target_bb) {
          target_bb = other_pred;
        }

        Copy_BB_For_Duplication(pred, phi_pred, other_pred, tail, &t_set);
      }

      pred = phi_pred;
    }
  }

  OP *br_op = BB_branch_op(head);

  VARIANT variant = CGTARG_Analyze_Branch(br_op, &cond_tn, &tn2);
  BOOL false_br = V_false_br(variant)?TRUE:FALSE;

  // now associate with each BB the predicate tn
  Associate_Mem_Predicates(cond_tn, false_br, t_set, ft_set, &ops);
  // add computation of !test
  OP *opb = br_op;
  if (OPS_length(&ops)>0) {
    BOOL stop = FALSE;
    OP *tmp_op;
    FOR_ALL_BB_OPs(head,tmp_op) {
      for (int i = 0; i < OP_opnds(tmp_op); i++) {
	if (TN_is_register(OP_opnd(tmp_op, i)) &&
	    TN_is_register(OP_result(OPS_last(&ops),0)) &&
	    TNs_Are_Equivalent (OP_opnd(tmp_op, i), 
				OP_result(OPS_last(&ops),0))) {
	  opb =tmp_op;
	  stop = TRUE;
	  break;
	}
      }
      if (stop)
	break;
    }
  }

  BB_Insert_Ops_Before(head, opb, &ops);

  // can remove the branch op
  BB_Remove_Op(head, br_op);

  FOR_ALL_BB_PHI_OPs(tail, phi) {
    UINT8 npreds = OP_opnds(phi);

    BB *true_bb = target_bb == tail ? head : target_bb;
    BB *false_bb = fall_thru_bb == tail ? head : fall_thru_bb;

    if (false_br) {
      BB *temp_bb = false_bb;
      false_bb = true_bb;
      true_bb = temp_bb;
    }

    TN *select_tn = OP_result (phi, 0);

    if (npreds > 2) {
      TN *result[1];
      UINT8 j = 0;
      UINT8 nopnds = npreds - 1;
      TN *opnd[nopnds];

      phi_t *phi_info = BB_Alloc_Phi();
      BB_Lst *bbs = &phi_info->bbs;

      select_tn = Dup_TN(select_tn);

      BB* tpred = Get_BB_Pos_in_PHI (phi, true_bb);
      BB* fpred = Get_BB_Pos_in_PHI (phi, false_bb);

      // new args goes last if we know that a new edge will be inserted.
      // else, replace old phi tn with newly create select tn.
      if (edge_needed) {
        for (UINT8 i = 0; i < npreds; i++) {
          BB *pred = Get_PHI_Predecessor (phi, i);
          if (pred != tpred && pred != fpred) {
            bbs->push_front(pred);
            opnd[j++] = OP_opnd(phi, i);
          }
        }
        bbs->push_front(head);
        opnd[j] = select_tn;
      }
      else {
        for (UINT8 i = 0; i < npreds; i++) {
          BB *pred = Get_PHI_Predecessor (phi, i);
          if (pred != fpred && pred != tpred) {
            bbs->push_front(pred);
            opnd[j++] = OP_opnd(phi, i);
          }
          else if (pred == tpred) {
            bbs->push_front(head); 
            opnd[j++] = select_tn;
          }
        }
      }
      
      result[0] = OP_result(phi, 0);
      BB_Create_And_Map_Phi (nopnds, result, opnd, phi, phi_info);
    }
    else {
      // Mark phi to be deleted.
      old_phis.push_front(phi);
    }

    Create_PSI_or_Select (select_tn, cond_tn, head, true_bb, false_bb, phi, &cmov_ops);
  
    select_count++;
  }
 
  if (target_bb != tail) {
    Promote_BB_Chain (head, target_bb, tail);
  }
  if (fall_thru_bb != tail) {
    Promote_BB_Chain (head, fall_thru_bb, tail);
  }
   if (Trace_Select_Gen) {
     fprintf(Select_TFile, "<select> Insert selects in BB%d", BB_id(head));
     Print_OPS (&cmov_ops); 
     fprintf (Select_TFile, "\n");
   }

   // Cleanup phis that are going to be replaced ...
   BB_Remove_Ops(tail, old_phis);
   // ... by the selects
   BB_Append_Ops(head, &cmov_ops);

  // Commit dismissible loads and stores
   BB_Fix_Spec_Loads (head);

  if (PROC_is_select())
    BB_Fix_Spec_Stores (head);

   // create or update the new head->tail edge.
   if (edge_needed) {
     if (tail == BB_next(head))
       Target_Simple_Fall_Through_BB(head, tail);
     else
       Add_Goto (head, tail);
   }
   else {
     if (tail != BB_Fall_Thru_Successor(head)) {
       Add_Goto (head, tail);
     }
   }
    
   // Update edge probability to 1.0
   BBLIST_prob(BB_Find_Succ(head, tail)) = 1.0;

   // Update SSA with new phis.
   BB_Update_Phis(tail);

   // make sure promote PSI True operands dominate the others
   // we know that they newly create psi ops are disjoint
   BB_Sort_Psi_Ops(head);

   if (Trace_Select_Gen) {
     fprintf (TFile, "\nEnd Select_Fold from BB%d\n", BB_id(head));
     Print_All_BBs();
   }
}


void
Convert_Min_Max(RID *rid, const BB_REGION& bb_region)
{

	INT i;
	BOOL modification_applied=FALSE;
	Identify_Logifs_Candidates();
	// make sure dominator information are correct 
	// before starting optimizing
	Calculate_Dominators();
	
	if_bb_map = BB_MAP_Create();

	for (i = 0; i < max_cand_id; i++) {
		BB *bb = cand_vec[i];
		BB *bbb;
	
		if (bb == NULL) continue;
	
		BB_SET *t_set = BB_SET_Create_Empty(PU_BB_Count+2, &MEM_Select_pool);
		BB_SET *ft_set = BB_SET_Create_Empty(PU_BB_Count+2, &MEM_Select_pool);
	
		// test of conditional expression
		if (Is_Hammock_Legacy(bb, t_set, ft_set, &bbb, true)) {
			if(Check_min_max_abs_candidate(bb, t_set, ft_set, bbb)) modification_applied = TRUE;
		}
	}
	BB_MAP_Delete(if_bb_map);
	Free_Dominators_Memory();
	if (modification_applied) SSA_DeadCode();
	clear_spec_lists();
}


/* ================================================================
 *
 *   Entry
 *
 * ================================================================
 */
void
Convert_Select(RID *rid, const BB_REGION& bb_region)
{
  INT i;

  Set_Error_Phase ("Select Region Formation");

#ifndef TARG_ST
  //TB: no more OPT_SPACE. This is done in a centralized function
  if (OPT_Space) {
    CG_ifc_freq = FALSE;
    CG_ifc_cycles = FALSE;
    CG_select_spec_stores = PROC_has_predicate_stores() && Enable_Conditional_Store;
  }
#endif

  // higher select_factor means ifc more aggressive.
  select_factor = CGTARG_Ifc_Factor();
  branch_penalty = CGTARG_Branch_Taken_Penalty();

  if (select_factor == 0.0) return;

  Trace_Select_Init();

  Initialize_Memory();

  if (Trace_Select_Candidates) {
    Trace_IR(TP_SELECT, "Before Select Region Formation", NULL);
    Print_All_BBs();
  }

  Identify_Logifs_Candidates();

  // make sure dominator and liveness information are correct 
  // before starting optimizing
  Calculate_Dominators();
  GRA_LIVE_Recalc_Liveness(rid);

  if_bb_map = BB_MAP_Create();

  bool allow_dup = false;

  if (Trace_Select_daVinci) {
    printf ("start ifc for%s\n",  ST_name(Get_Current_PU_ST()));
  }

 restart:

  bool region_changed = false;

  for (i = 0; i < max_cand_id; i++) {
    BB *bb = cand_vec[i];
    BB *bbb;
    
    if (bb == NULL) continue;
    
    // tests for logical expression
    //TDR: Be aware that Is_Double_Logif will modify the code => normalize the branch
    if (bbb = Is_Double_Logif(bb)) {
      Initialize_Hammock_Memory();

      if (Trace_Select_daVinci) {
        printf ("Simplify Logif for BB%d\n", BB_id(bb));
        //        draw_flow_graph();
        draw_CFG();
      }

      Simplify_Logifs(bb, bbb);

      GRA_LIVE_Recalc_Liveness(rid);

#ifdef Is_True_On
      //      Sanity_Check();
#endif
      region_changed = true;

      Finalize_Hammock_Memory();
      cand_vec[BB_MAP32_Get(postord_map, bbb)-1] = NULL;
    }

    clear_spec_lists();

  }

  for (i = 0; i < max_cand_id; i++) {
    BB *bb = cand_vec[i];
    BB *bbb;

    if (bb == NULL) continue;
    
    BB_SET *t_set = BB_SET_Create_Empty(PU_BB_Count+2, &MEM_Select_pool);
    BB_SET *ft_set = BB_SET_Create_Empty(PU_BB_Count+2, &MEM_Select_pool);

    // test of conditional expression
    if (Is_Hammock (bb, t_set, ft_set, &bbb, allow_dup)) {
      Initialize_Hammock_Memory();

      if (Trace_Select_daVinci) {
        printf ("Select_Fold for BB%d\n", BB_id(bb));
        //        draw_flow_graph();
        draw_CFG();
      }

      Select_Fold (bb, t_set, ft_set, bbb);

      GRA_LIVE_Recalc_Liveness(rid);

#ifdef Is_True_On
      //      Sanity_Check();
#endif
      region_changed = true;

      Finalize_Hammock_Memory();
      
      // if bb is still a logif, that means that there was a merge.
      // need to update logif map
      if (BB_kind (bb) == BBKIND_LOGIF) {
        cand_vec[BB_MAP32_Get(postord_map, bb)-1] = bb;
        cand_vec[BB_MAP32_Get(postord_map, bbb)-1] = NULL;
      }
      else {
        cand_vec[BB_MAP32_Get(postord_map, bb)-1] = NULL;
      }
    } else {
        //TDR New version to change only ft_set
        BB_SET_ClearD(t_set);BB_SET_ClearD(ft_set);
        if (CG_ifc_subpart && Is_Hammock_ft(bb, t_set,ft_set, &bbb, allow_dup)) {
            Initialize_Hammock_Memory();
            if (Trace_Select_Candidates) {
              fprintf (Select_TFile, "<select> hammock FallThru selected\n");
            }
            Select_Fold_ft (bb, ft_set, bbb);
            GRA_LIVE_Recalc_Liveness(rid);
            region_changed = true;
            Finalize_Hammock_Memory();
            if (BB_kind (bb) == BBKIND_LOGIF) {
              cand_vec[BB_MAP32_Get(postord_map, bb)-1] = bb;
              cand_vec[BB_MAP32_Get(postord_map, bbb)-1] = NULL;
            }
            else {
              cand_vec[BB_MAP32_Get(postord_map, bb)-1] = NULL;
            }
        }
    }    
    clear_spec_lists();
  }

  if (region_changed) {
    allow_dup = false;
    goto restart;
  }
  else {
    if (!allow_dup && CG_ifc_allow_dup) {
      allow_dup = true;
      goto restart;
    }
  }

  BB_MAP_Delete(if_bb_map);

  if (Trace_Select_Stats) {
    CG_SELECT_Statistics();
  }

#ifdef Is_True_On
  //  SSA_Verify(rid, 0);
#endif

  if (Trace_Select_daVinci) {
    printf ("done\n");
        draw_CFG();
        //    draw_flow_graph();

  }

  Finalize_Select();
}

/* ================================================================
 *
 * daVincy output of CFG.
 *
 * ================================================================
 */
static char *
sPrint_TN ( 
  TN *tn, 
  char *buf 
)
{
  char *result = buf;

  if (tn == NULL) {
    buf += sprintf ( buf, "--nil--");
    return result;
  }

  if (TN_is_constant(tn)) {
    if ( TN_has_value(tn)) {
      buf += sprintf ( buf, "(0x%llx)", TN_value(tn) );
      if (TN_size(tn) == 4 && 
	  TN_value(tn) >> 32 != 0 &&
	  TN_value(tn) >> 31 != -1)
	buf += sprintf ( buf, "!!! TN_value=0x%llx is too big to fit in a word",
			 TN_value(tn));
    }
    else if (TN_is_enum(tn)) {
      buf += sprintf ( buf, "(enum:%s)", ISA_ECV_Name(TN_enum(tn)) );
    }
    else if ( TN_is_label(tn) ) {
      LABEL_IDX lab = TN_label(tn);
      const char *name = LABEL_name(lab);
      INT64 offset = TN_offset(tn);
      if ( offset == 0 ) {
	buf += sprintf ( buf, "(lab:%s)", name );
      }
      else {
	buf += sprintf ( buf, "(lab:%s+%lld)", name, offset );
      }
    } 
    else if ( TN_is_tag(tn) ) {
      LABEL_IDX lab = TN_label(tn);
      const char *name = LABEL_name(lab);
      buf += sprintf ( buf, "(tag:%s)", name );
    }
    else if ( TN_is_symbol(tn) ) {
      ST *var = TN_var(tn);

      buf += sprintf ( buf, "(sym" );
      buf += sprintf ( buf, "%s",TN_RELOCS_Name(TN_relocs(tn)) );

      if (ST_class(var) == CLASS_CONST)
      	buf += sprintf ( buf, ":%s)", Targ_Print(NULL, ST_tcon_val(var)));
      else
      	buf += sprintf ( buf, ":%s%+lld)", ST_name(var), TN_offset(tn) );
    } 
  }
  else {  /* register TN */
    if (TN_is_global_reg(tn)) {
      buf += sprintf ( buf, "GTN%d", TN_number(tn) );
    }
    else {
      buf += sprintf ( buf, "TN%d", TN_number(tn) );
    }
    if (TN_register(tn) != REGISTER_UNDEFINED) {
      if (TN_register(tn) <= REGISTER_CLASS_last_register(TN_register_class(tn))) {
	buf += sprintf (buf, "(%s)", 
		REGISTER_name(TN_register_class(tn), TN_register(tn)));
      } else {
	buf += sprintf (buf, "(%d,%d)", TN_register_class(tn), TN_register(tn));
      }
    }
    if (TN_is_save_reg(tn)) {
	buf += sprintf (buf, "(sv:%s)", 
		REGISTER_name(TN_save_rclass(tn), TN_save_reg(tn)));
    }
  }
  
  return result;
}

static const char *
Node(BB* bb)
{
  static char *buffer = NULL;
  static INT buf_size = 0;

  /* estimate the size of buffer needed:
   */
  const INT size =  25	+ (BB_length (bb) * 128);

  if (buf_size == 0) {
    buffer = (char *)malloc(size);
    buf_size = size;
  } else if (size > buf_size) {
    while (size > buf_size) buf_size *= 2;
    buffer = (char *)realloc(buffer, buf_size);
  }

  char *p = buffer;
  OP* op;

  sprintf(p, "\\n");

  FOR_ALL_BB_OPs_FWD(bb, op) {  
    int i;
 
   for (i = 0; i < OP_results(op); i++) {
     p = sPrint_TN (OP_result(op, i), p);
     p += strlen (p);
     p += sprintf(p, " ");
    }

   if (OP_results(op))
     p += sprintf(p, " = ");

   p += sprintf(p, " %s ", TOP_Name(OP_code(op)));

    for (i = 0; i < OP_opnds(op); i++) {
     p = sPrint_TN (OP_opnd(op, i), p);
     p += strlen (p);
     if (OP_Pred_False(op, i))
       p += sprintf(p, "!");
     p += sprintf(p, " ");
    }

    p += sprintf(p, "\\n");
  }

  FmtAssert(p < buffer + size, ("Node_Label buffer size estimate too small"));

  return buffer;
}

void
draw_CFG(void)
{
  if (! Trace_Select_daVinci) return;
  if (! DaVinci::enabled (TRUE)) return;

  MEM_POOL dv_pool;
  dv_pool.magic_num = 0;		// force it to be unintialized
  MEM_POOL_Constructor pool (&dv_pool, "DaVinci", FALSE);

  DaVinci dv (&dv_pool, NULL);

  dv.Title (Cur_PU_Name);
  dv.Show_Status("select");

  // Now we start drawing
  NODE_TYPE nt_plain, nt_entry, nt_exit, nt_multi,nt, nt_call;
  EDGE_TYPE et_known;

  nt_entry.Color ("palegreen");
  nt_exit.Color ("pink");
  nt_exit.Name ("exit");
  nt_call.Boarder(NB_DOUBLE);
  nt_multi.Color ("lightgoldenrod");
  nt_multi.Shape(NS_CIRCLE);

  dv.Graph_Begin ();

  // add all nodes
  for (BB* bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    char buf[10];
    nt = nt_plain;
    if (BBlist_Len(BB_preds(bb)) > 1 || BBlist_Len(BB_succs(bb)) > 1) nt = nt_multi;
    if (BB_entry(bb)) nt = nt_entry;
    if (BB_exit(bb))  nt = nt_exit;
    if (BB_call(bb))  nt = nt_call;
    sprintf (buf, "%d", BB_id(bb));
    nt = nt.Name(buf);
    dv.Node_Begin (NODE_ID (bb), Node(bb), nt);
    BBLIST* sedge;
    FOR_ALL_BB_SUCCS(bb, sedge) {
      dv.Out_Edge (EDGE_ID (NODE_ID (bb), NODE_ID (BBLIST_item(sedge))),
		   et_known,
		   NODE_ID (BBLIST_item(sedge)));
    }
    dv.Node_End ();
  }
  dv.Graph_End ();

  dv.Event_Loop (NULL);
}

static void dump_pred_cands()
{
  PredOp_Map_Iter i_iter;
  PredOp_Map_Iter i_end;

  i_iter = pred_i.begin();
  i_end = pred_i.end();

  while(i_iter != i_end) {
    OP* op = PredOp_Map_Op(i_iter);
    TN *pred = PredOp_Map_Pred(i_iter).tn;
    Print_OP (op);
    Print_TN (pred, FALSE);    
    fprintf (Select_TFile, "\n");    
    i_iter++;
  }
}


#endif /* SUPPORTS_SELECT */

