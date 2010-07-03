/*
  Copyright (C) 2009, STMicroelectronics, All Rights Reserved.

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

/* =============================================================================
 * =============================================================================
 *
 * Module: cg_coalesce.cxx
 *
 * Description:
 *
 *   Perform a Briggs like coalescing on GTNs.
 *   The goal is to coalesce GTNs that are  overlapping  but not conflicting
 *   because they hold the same value on the overlapping region(s)  of their
 *   live-ranges. It happens when (at least) one definition of the first GTN
 *   is a copy of the second GTN.
 *   This pass should be done before GRA, that is currently not able to detect
 *   those false conflicts.
 *   Note that this optimization will likely increase register pressure on
 *   coalesced GTNs but live-range splitting in GRA should (partly) compensate
 *   that.
 *
 * =============================================================================
 * =============================================================================
 */
/*
 * TODO: Smarter handling of rematerializable TNs (for instance, if both TNs are
 *       rematerializable with same value, don't remove the flag. currently the
 *       flag is simply removed)
 * TODO: Try to only coalesce overlapping GTNs?
 * Limitation: EXTRACT and COMPOSE moves cannot be handled here
 *             (cannot use the same GTN, as they have different sizes).
 *             (For this task, doing the coalescing in GRA could be better).
 */
#include "defs.h"
#include "mempool.h"
#include "config.h"
#include "tracing.h"
#include "timing.h"
#include "cgir.h"
#include "tn_set.h"
#include "gtn_universe.h"
#include "gtn_set.h"
#include "gtn_tn_set.h"
#include "tn_list.h"
#include "cg_flags.h"
#include "gra_live.h"
#include <map>
#include <list>

using namespace std;

#define COALESCE_PREFIX "<coal>"

// Defines datatypes used in this module
struct TN_sort
{
  bool operator()(const TN* t1, const TN* t2) const
  {
    return TN_number(t1) < TN_number(t2);
  }
};

typedef map<TN *, TN *, TN_sort>  TN2TNMap_t;
typedef TN2TNMap_t::iterator      TN2TNMapIterator_t;

// The list of move candidate for coalescing
static list<OP*> *move_list;

// Associate a GTN with another GTN it must be replaced by.
static TN2TNMap_t *TN_Rename_map;

// CoalescingInfo_t:
// ----------------
// This structure contains coalescing related information 
// for a single GTN.
typedef struct {
  GTN_SET      *adjacent_tns;   // Set of conflicting GTNs
  TN_LIST      *coalesced_tns;  // Set of GTNs that are coalesced with current GTN
  REGISTER_SET  allowed_regs;   // Set of allowed registers (based on both current
                                //   GTN and coalesced GTNs constraints)
} CoalescingInfo_t;

typedef map<TN *, CoalescingInfo_t *, TN_sort> CoalescingInfoMap_t;
typedef CoalescingInfoMap_t::iterator          CoalescingInfoMapIterator_t;

// Associate a GTN with the information structure
static CoalescingInfoMap_t *TN_CoalInfo_map;

// Trace related data
#define TRACE_COALESCE_BUILD  1
#define TRACE_COALESCE_OPT    2
static INT Trace_Build = FALSE;
static INT Trace_Optimize = FALSE;

// Memory pool used by the optimization
static MEM_POOL coalesce_local_pool;

// Counters to store the number of transformations done
// since the start of the UNIT compilation and the start
// of PU compilation
static INT global_transfo_counter = 0;
static INT PU_transfo_counter = 0;

/* ============================================================================
 *   Get_CoalescingInfo
 *
 *   Return the CoalescingInfo instance associated with <tn>.
 * ============================================================================
 */
static CoalescingInfo_t *Get_CoalescingInfo(TN *tn) {
  CoalescingInfoMapIterator_t it;
  it = TN_CoalInfo_map->find(tn);
  FmtAssert(it != TN_CoalInfo_map->end(), ("Missing CoalescingInfo"));
  return (it->second);
}

/* ============================================================================
 *   Create_CoalescingInfo
 *
 *   Return the CoalescingInfo associated to <tn>, and if it does not exist
 *   yet, create a new one and return it.
 * ============================================================================
 */
static CoalescingInfo_t *Create_CoalescingInfo(TN *tn) {
  CoalescingInfoMapIterator_t it;
  it = TN_CoalInfo_map->find(tn);
  if (it == TN_CoalInfo_map->end()) {
    CoalescingInfo_t *info = TYPE_MEM_POOL_ALLOC (CoalescingInfo_t, &coalesce_local_pool);
    info->adjacent_tns = NULL;
    info->allowed_regs = REGISTER_CLASS_allocatable(TN_register_class(tn));
    info->coalesced_tns = NULL;
    (*TN_CoalInfo_map)[tn] = info;
    return info;
  }
  return it->second;
}

/* ============================================================================
 *   Print_Stage
 *
 *   Dump <stage> name in debug trace.
 * ============================================================================
 */
static void
Print_Stage(const char *stage) {
  fprintf (TFile,
           COALESCE_PREFIX" ===\n"
           COALESCE_PREFIX" === %s\n"
           COALESCE_PREFIX" ===\n", stage);
}

/* ============================================================================
 *   Increment_Nb_Changes
 *
     Increment by 1 the count of transformation performed.
 * ============================================================================
 */
static void
Increment_Nb_Changes() {
  PU_transfo_counter++;
  global_transfo_counter++;
}

/* ============================================================================
 *   Is_Reached_Last_Allowed_Change
 *
 *   Return TRUE if the maximum number of transformations allowed has been
 *   reached (useful for debugging).
 * ============================================================================
 */
static BOOL
Is_Reached_Last_Allowed_Change() {
  BOOL ret = (CG_coalesce_max_transfo != INT32_MAX &&
              global_transfo_counter >= CG_coalesce_max_transfo);
  if (ret && Trace_Optimize) {
    fprintf(TFile,
            "\n"COALESCE_PREFIX" Maximum number of coalescing (%d) reached\n",
            CG_coalesce_max_transfo);
  }
  return ret;
}

/* ============================================================================
 *   GTN_SET_Remove_Uses
 *
 *   Remove TNs used in <op> from <tnset> and return a pointer to the modified
 *   set.
 * ============================================================================
 */
static GTN_SET*
GTN_SET_Remove_Uses(GTN_SET *tnset, OP *op) {
  INT id;
  for (id=0; id<OP_opnds(op); id++) {
    TN *tn = OP_opnd(op, id);
    if (TN_is_register(tn)) {
      tnset = GTN_SET_Difference1D(tnset, tn);
    }
  }
  return tnset;
}

/* ============================================================================
 *   GTN_SET_Add_Uses
 *
 *   Add TNs used in <op> into <tnset> and return a pointer to the modified
 *   set.
 * ============================================================================
 */
static GTN_SET*
GTN_SET_Add_Uses(GTN_SET *tnset, OP *op) {
  INT id;
  for (id=0; id<OP_opnds(op); id++) {
    TN *tn = OP_opnd(op, id);
    if (TN_is_register(tn)) {
      tnset = GTN_SET_Union1D(tnset, tn, &coalesce_local_pool);
    }
  }
  return tnset;
}

/* ============================================================================
 *   GTN_SET_Remove_Defs
 *
 *   Remove TNs defined in <op> from <tnset> and return a pointer to the
 *   modified set.
 * ============================================================================
 */
static GTN_SET*
GTN_SET_Remove_Defs(GTN_SET *tnset, OP *op) {
  INT id;
  for (id=0; id<OP_results(op); id++) {
    TN *tn = OP_result(op, id);
    if (TN_is_register(tn)) {
      tnset = GTN_SET_Difference1D(tnset, tn);
    }
  }
  return tnset;
}

/* ============================================================================
 *   GTN_SET_Add_Defs
 *
 *   Add TNs defined in <op> into <tnset> and return a pointer to the modified
 *   set.
 * ============================================================================
 */
static GTN_SET*
GTN_SET_Add_Defs(GTN_SET *tnset, OP *op) {
  INT id;
  for (id=0; id<OP_results(op); id++) {
    TN *tn = OP_result(op, id);
    if (TN_is_register(tn)) {
      tnset = GTN_SET_Union1D(tnset, tn, &coalesce_local_pool);
    }
  }
  return tnset;
}

/* ============================================================================
 *   Add_Move_To_List
 *
 *   Add the move operation <op> into the list of coalescing candidate moves.
 *   The list is sorted based on the (estimated) frequency of the container BB.
 *   (The frequency is partly based on the loop depth)
 * ============================================================================
 */
static void
Add_Move_To_List(OP *op) {
  float op_freq = BB_freq(OP_bb(op));

  list<OP*>::iterator it = move_list->begin();
  while (it != move_list->end() && KnuthCompareGT(BB_freq(OP_bb(*it)), op_freq)) {
    it++;
  }

  if (it != move_list->end()) {
    move_list->insert(it, op);
  }
  else {
    move_list->push_back(op);
  }
}

/* ============================================================================
 *   Add_Edge
 *
 *   Add conflict between <tn> and the set of <live> GTNs.
 *   Note that dislike the standard Briggs algorithm, <tn> is not added in
 *        conflict list of all <live> GTNs. This is a speed optimization.
 *        The counterpart is that in Is_Allowed_Coalescing(), to check if 2 TNs
 *        conflicts it is required to check both adjacency lists.
 * ============================================================================
 */
static void
Add_Edge(TN *tn, GTN_SET *live) {
  CoalescingInfo_t *info = Get_CoalescingInfo(tn);
  if (info->adjacent_tns != NULL) {
    info->adjacent_tns = GTN_SET_UnionD(info->adjacent_tns, live, &coalesce_local_pool);
  }
  else {
    info->adjacent_tns = GTN_SET_Copy(live, &coalesce_local_pool);
  }
}

/* ============================================================================
 *   OP_Is_Relevant_Copy
 *
 *   Return TRUE if <op> is a move operation candidate for coalescing.
 * ============================================================================
 */
static BOOL
OP_Is_Relevant_Copy(OP *op) {
  if (!OP_Is_Preference_Copy(op)) {
    return FALSE;
  }

  TN *src_tn = OP_opnd  (op, OP_Copy_Operand(op));
  TN *dst_tn = OP_result(op, OP_Copy_Result(op));

  if (!TN_is_global_reg(src_tn) || TN_is_dedicated(src_tn) || TN_register(src_tn) != REGISTER_UNDEFINED ||
      !TN_is_global_reg(dst_tn) || TN_is_dedicated(dst_tn) || TN_register(dst_tn) != REGISTER_UNDEFINED) {
    return FALSE;
  }

  return TRUE;
}

/* ============================================================================
 *   Build_Interference_Graph
 *
 *   Build the interference graph for current PU.
 *   Perform a backward walk of each BB, 
 * ============================================================================
 */
static void
Build_Interference_Graph() {
  BB *bb;
  OP *op;

  GTN_SET *live_set = GTN_SET_Create(Last_TN + 1,&coalesce_local_pool);

  if (Trace_Build) {
    Print_Stage("Build Interference Graph");
  }

  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {

    if (Trace_Build) {
      fprintf(TFile, "\n"COALESCE_PREFIX"\t-> Parsing BB%d (freq %f)\n",
              BB_id(bb), BB_freq(bb));
      GRA_LIVE_Print_Liveness(bb);
    }

    live_set = GTN_SET_CopyD(live_set, BB_live_out(bb), &coalesce_local_pool);

    FOR_ALL_BB_OPs_REV(bb, op) {

      if (Trace_Build) {
        fprintf(TFile, COALESCE_PREFIX"\t\tOP: ");
        Print_OP_No_SrcLine(op);
        fprintf(TFile, COALESCE_PREFIX"\t\t[ liveout: ");
        GTN_TN_SET_Print(live_set, TFile);
        fprintf(TFile, " ]\n");
      }

      BOOL is_cond_def = OP_cond_def(op);
      if (!is_cond_def && OP_Is_Relevant_Copy(op)) {
        if (Trace_Build) {
          fprintf(TFile, COALESCE_PREFIX"\t\t-> Move candidate for coalescing\n");
        }
        live_set = GTN_SET_Remove_Uses(live_set, op);
        Add_Move_To_List(op);
      }
      live_set = GTN_SET_Add_Defs(live_set, op);

      INT idx;
      for (idx=0; idx<OP_results(op); idx++) {
        TN *tn = OP_result(op, idx);
        if (TN_is_register(tn)) {
          CoalescingInfo_t *tn_info = Create_CoalescingInfo(tn);

          Add_Edge(tn, live_set);

          if (!TN_is_dedicated(tn)) {
            ISA_REGISTER_SUBCLASS sc = OP_result_reg_subclass(op, idx);
            if (sc != ISA_REGISTER_SUBCLASS_UNDEFINED) {
              // Register subclass constraints
              tn_info->allowed_regs = REGISTER_SET_Intersection(tn_info->allowed_regs,
                                                                REGISTER_SUBCLASS_members(sc));
            }
          }
        }
      }

      for (idx=0; idx<OP_opnds(op); idx++) {
        TN *tn = OP_opnd(op, idx);
        if (TN_is_register(tn)) {
          CoalescingInfo_t *tn_info = Create_CoalescingInfo(tn);
          if (!TN_is_dedicated(tn)) {
            ISA_REGISTER_SUBCLASS sc = OP_opnd_reg_subclass(op, idx);
            if (sc != ISA_REGISTER_SUBCLASS_UNDEFINED) {
              // Register subclass constraints
              tn_info->allowed_regs = REGISTER_SET_Intersection(tn_info->allowed_regs,
                                                                REGISTER_SUBCLASS_members(sc));
            }
          }
        }
      }

      if (!is_cond_def) {
        live_set = GTN_SET_Remove_Defs(live_set, op);
      }
      live_set = GTN_SET_Add_Uses(live_set, op);
    }
  }
}

/* ============================================================================
 *   Merge_Node
 *
 *   Update the interference graph so that <src_tn> will be coalesced with
 *   <dst_tn>.
 *   The following actions are performed:
 *   - Update adjacent list of <dst_tn> by merging it with adjacent list
 *     of <src_tn>
 *   - Associate to <dst_tn> all TNs that will be coalesced with it.
 *     This includes already coalesced TNs + <src_tn> + TNs already
 *     coalesced with <src_tn>.
 *   - Update the rename map, so that <src_tn> and all the TNs that were
 *     already coalesced with it will now be renamed into <dst_tn>
 *   - Update rematerializable and GRA_homeable flags of the target TN.
 * ============================================================================
 */
static void
Merge_Node(TN *src_tn, TN *dst_tn) {
  CoalescingInfo_t *src_info, *dst_info;
  src_info = Get_CoalescingInfo(src_tn);
  dst_info = Get_CoalescingInfo(dst_tn);

  //
  // Merge the set of adjacent TNs
  //
  dst_info->adjacent_tns = GTN_SET_UnionD(dst_info->adjacent_tns,
                                          src_info->adjacent_tns,
                                          &coalesce_local_pool);

  //
  // Update the list of coalesced TNs
  //
  if (dst_info->coalesced_tns == NULL) {
    // Always add itself to simplify conflict checking in Is_Allowed_Coalescing()
    dst_info->coalesced_tns = TN_LIST_Push(dst_tn, NULL, &coalesce_local_pool);
  }
  if (src_info->coalesced_tns != NULL) {
    // Merge both lists
    TN_LIST *src_list;
    for ( src_list = src_info->coalesced_tns;
          src_list != NULL;
          src_list = TN_LIST_rest(src_list) ) {
      dst_info->coalesced_tns = TN_LIST_Push(TN_LIST_first(src_list),
                                             dst_info->coalesced_tns,
                                             &coalesce_local_pool);
    }
  }
  else {
    dst_info->coalesced_tns = TN_LIST_Push(src_tn,
                                           dst_info->coalesced_tns,
                                           &coalesce_local_pool);
  }

  //
  // Update renaming map
  //
  TN_LIST *tnl;
  for (tnl = dst_info->coalesced_tns;
       tnl;
       tnl = TN_LIST_rest(tnl)) {
    TN *tn = TN_LIST_first(tnl);
    if (tn != dst_tn) {
      (*TN_Rename_map)[tn] = dst_tn;
    }
  }

  //
  // Update allowed register set
  //
  dst_info->allowed_regs = REGISTER_SET_Intersection(dst_info->allowed_regs,
                                                     src_info->allowed_regs);

  //
  // Reset rematerializable flags on target TN if not compatible with source TN
  //
  if (TN_is_rematerializable(dst_tn)) {
    BOOL reset_remat = TRUE;
    if (TN_is_rematerializable(src_tn)) {
      WN *src_wn = TN_remat(src_tn);
      WN *dst_wn = TN_remat(dst_tn);
      if (src_wn == dst_wn ||
          (WN_operator_is(src_wn, OPR_INTCONST) && 
           WN_operator_is(dst_wn, OPR_INTCONST) && 
           WN_const_val(src_wn) == WN_const_val(dst_wn))) {
        reset_remat = FALSE;
      }
    }
    if (reset_remat) {
      if (Trace_Optimize) {
        fprintf(TFile, COALESCE_PREFIX"    --> Reset TN_is_rematerializable on TN%d\n", TN_number(dst_tn));
      }
      Reset_TN_is_rematerializable(dst_tn);
      Set_TN_remat(dst_tn, NULL);
    }
  }

  //
  // Reset GRA homeable flags
  //
  if (TN_is_gra_homeable(dst_tn)) {
    if (Trace_Optimize) {
      fprintf(TFile, COALESCE_PREFIX"    --> Reset TN_is_gra_homeable on TN%d\n", TN_number(dst_tn));
    }
    Reset_TN_is_gra_homeable(dst_tn);
    Set_TN_home(dst_tn, NULL);
  }
}

/* ============================================================================
 *   Is_Allowed_Coalescing
 *
 *   Return TRUE if the 2 TNs <src_tn> and <dst_tn> are not conflicting and
 *   can be coalesced.
 * ============================================================================
 */
static BOOL
Is_Allowed_Coalescing(TN *src_tn, TN *dst_tn) {
  CoalescingInfo_t *src_info, *dst_info;
  src_info = Get_CoalescingInfo(src_tn);
  dst_info = Get_CoalescingInfo(dst_tn);

  GTN_SET *src_adj_list = src_info->adjacent_tns;
  GTN_SET *dst_adj_list = dst_info->adjacent_tns;

  if (Trace_Optimize) {
    fprintf(TFile, COALESCE_PREFIX"    * TN%d Adjacency list: ", TN_number(src_tn));
    GTN_TN_SET_Print(src_adj_list, TFile);
    fprintf(TFile, "\n");
    fprintf(TFile, COALESCE_PREFIX"    * TN%d Adjacency list: ", TN_number(dst_tn));
    GTN_TN_SET_Print(dst_adj_list, TFile);
    fprintf(TFile, "\n");
  }
  
  if ( ( src_adj_list && GTN_SET_MemberP(src_adj_list, dst_tn) ) ||
       ( dst_adj_list && GTN_SET_MemberP(dst_adj_list, src_tn) ) ) {
    if (Trace_Optimize) {
      fprintf(TFile, COALESCE_PREFIX"    --> SKIP coalescing: Conflict between TNs\n");
    }
    return FALSE;
  }


  if (CG_coalesce_pair_only) {
    // Single step coalescing only
    if (TN_Rename_map->find(dst_tn) != TN_Rename_map->end() ||
        dst_info->coalesced_tns != NULL) {
      if (Trace_Optimize) {
        fprintf(TFile, COALESCE_PREFIX"    --> SKIP coalescing: TN%d has already been coalesced\n", TN_number(dst_tn));
      }
      return FALSE;
    }
    if (TN_Rename_map->find(src_tn) != TN_Rename_map->end() ||
        src_info->coalesced_tns != NULL) {
      if (Trace_Optimize) {
        fprintf(TFile, COALESCE_PREFIX"    --> SKIP coalescing: TN%d has already been coalesced\n", TN_number(src_tn));
      }
      return FALSE;
    }
  }

  // Do not coalesce if it leads to non-colorable GTN because of register subclass constraints
  if (REGISTER_SET_EmptyP(REGISTER_SET_Intersection(src_info->allowed_regs,
                                                    dst_info->allowed_regs))) {
    if (Trace_Optimize) {
      fprintf(TFile, COALESCE_PREFIX"    --> SKIP coalescing: incompatible register class constraints\n");
    }
    return FALSE;
  }

  if (dst_adj_list) {
    if (src_info->coalesced_tns != NULL) {
      TN_LIST *coalesced_tns;
      for (coalesced_tns = src_info->coalesced_tns;
           coalesced_tns != NULL;
           coalesced_tns = TN_LIST_rest(coalesced_tns)) {
        TN *tn = TN_LIST_first(coalesced_tns);
        if (GTN_SET_MemberP(dst_adj_list, tn)) {
          if (Trace_Optimize) {
            fprintf(TFile, COALESCE_PREFIX"    --> SKIP coalescing: Conflict with coalesced source TN%d\n", TN_number(tn));
          }
          return FALSE;
        }
      }
    }
  }
  if (src_adj_list) {
    if (dst_info->coalesced_tns) {
      TN_LIST *coalesced_tns;
      for (coalesced_tns = dst_info->coalesced_tns;
           coalesced_tns != NULL;
           coalesced_tns = TN_LIST_rest(coalesced_tns)) {
        TN *tn = TN_LIST_first(coalesced_tns);
        if (GTN_SET_MemberP(src_adj_list, tn)) {
          if (Trace_Optimize) {
            fprintf(TFile, COALESCE_PREFIX"    --> SKIP coalescing: Conflict with coalesced target TN%d\n", TN_number(tn));
          }
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}

/* ============================================================================
 *   Select_Candidate_Move
 *
 *   Walk through the list of moves candidate for coalescing and if there is
 *   no conflict between the source and the target TN, coalesce them together
 *   and update the interference graph.
 *   Note that effective TN renaming is not done in this stage. Instead a
 *   TN renaming map is build. In this map, TN that will be renamed are
 *   associated to the TN they will be renamed to.
 * ============================================================================
 */
static void
Select_Candidate_Move() {

  if (Trace_Optimize) {
    Print_Stage("Select Moves to coalesce");
  }

  // Exit if the maximum number of transformations is reached
  if (Is_Reached_Last_Allowed_Change()) {
    return;
  }

  list<OP*>::iterator it;
  for (it=move_list->begin(); it!= move_list->end(); it++) {
    OP *move = *it;

    TN *src_tn = OP_opnd  (move, OP_Copy_Operand(move));
    TN *dst_tn = OP_result(move, OP_Copy_Result(move));

    // For both source and target GTNs, take in consideration coalescing already
    // done by retrieving the potential GTNs they will be renamed to.
    TN2TNMapIterator_t it_rename;
    while ((it_rename = TN_Rename_map->find(src_tn)) != TN_Rename_map->end()) {
      src_tn = it_rename->second;
    }
    while ((it_rename = TN_Rename_map->find(dst_tn)) != TN_Rename_map->end()) {
      dst_tn = it_rename->second;
    }

    if (Trace_Optimize) {
      fprintf(TFile, "\n"COALESCE_PREFIX" [BB%d] Considering OP : ", BB_id(OP_bb(move)));
      Print_OP_No_SrcLine(move);
      fprintf(TFile, "\n"COALESCE_PREFIX"    * Effective target: TN%d, effective source: TN%d\n",
              TN_number(dst_tn), TN_number(src_tn));
      CoalescingInfo_t *dst_info = Get_CoalescingInfo(dst_tn);
      if (!REGISTER_SET_EqualP(dst_info->allowed_regs,
                               REGISTER_CLASS_allocatable(TN_register_class(dst_tn)))) {
        fprintf(TFile, COALESCE_PREFIX"    * Reduced set of allowed registers for target: ");
        REGISTER_SET_Print(dst_info->allowed_regs, TFile);
        fprintf(TFile, "\n");
      }
      CoalescingInfo_t *src_info = Get_CoalescingInfo(src_tn);
      if (!REGISTER_SET_EqualP(src_info->allowed_regs,
                               REGISTER_CLASS_allocatable(TN_register_class(src_tn)))) {
        fprintf(TFile, COALESCE_PREFIX"    * Reduced set of allowed registers for source: ");
        REGISTER_SET_Print(src_info->allowed_regs, TFile);
        fprintf(TFile, "\n");
      }
    }

    if (src_tn == dst_tn) {
      if (Trace_Optimize) {
        fprintf(TFile, COALESCE_PREFIX"    * Source and Target TNs already coalesced.\n");
      }
      continue;
    }

    // In order to preserve save_reg TN information:
    // - avoid coalescing if both TNs have save_reg property,
    // - if a TN has the save_reg property, keep it as the potential target of the coalescing
    if (TN_is_save_reg(src_tn)) {
      if (TN_is_save_reg(dst_tn)) {
        if (Trace_Optimize) {
          fprintf(TFile, COALESCE_PREFIX"    * Cannot coalesce 2 save_reg TNs together.\n");
        }
        continue;
      }
      if (Trace_Optimize) {
        fprintf(TFile, COALESCE_PREFIX"    * Swap source and target TNs to preserve save_reg flag\n");
      }
      TN *tmp = src_tn;
      src_tn = dst_tn;
      dst_tn = tmp;
    }

    if (Is_Allowed_Coalescing(src_tn, dst_tn)) {
      if (Trace_Optimize) {
        fprintf(TFile, COALESCE_PREFIX"    --> OK to coalesce, rename TN%d into TN%d\n",
                TN_number(src_tn), TN_number(dst_tn));
      }
      
      Merge_Node(src_tn, dst_tn);

      Increment_Nb_Changes();
      // Exit if the maximum number of transformations is reached
      if (Is_Reached_Last_Allowed_Change()) {
        break;
      }
    }
  }
}

/* ============================================================================
 *   Rename_TNs
 *
 *   Walk through the code of current PU and rename coalesced TNs.
 *   If the operation is a move from a GTN to itself, then remove it.
 * ============================================================================
 */
static void
Rename_TNs() {
  BB *bb;
  OP *op;

  if (Trace_Optimize) {
    Print_Stage("Effective TN renaming");
  }

  if (PU_transfo_counter == 0) {
    // Early exit if no coalescing done in current PU
    return;
  }

  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {

    OP *next_op = NULL;
    for (op = BB_first_op(bb); op != NULL; op = next_op) {
      next_op = OP_next(op);

      BOOL renamed = FALSE;
      INT idx;
      for (idx = 0; idx < OP_results(op); idx++) {
        TN *tn = OP_result(op, idx);
        if (TN_is_register(tn) && !TN_is_dedicated(tn) && TN_register(tn) == REGISTER_UNDEFINED) {
          TN2TNMapIterator_t it = TN_Rename_map->find(tn);
          if (it != TN_Rename_map->end()) {
            Set_OP_result(op, idx, it->second);
            renamed = TRUE;
          }
        }
      }
      
      for (idx = 0; idx < OP_opnds(op); idx++) {
        TN *tn = OP_opnd(op, idx);
        if (TN_is_register(tn) && !TN_is_dedicated(tn) && TN_register(tn) == REGISTER_UNDEFINED) {
          TN2TNMapIterator_t it = TN_Rename_map->find(tn);
          if (it != TN_Rename_map->end()) {
            Set_OP_opnd(op, idx, it->second);
            renamed = TRUE;
          }
        }
      }

      // Eliminate useless move
      if (renamed && OP_Is_Preference_Copy(op)) {
        TN *src_tn = OP_opnd  (op, OP_Copy_Operand(op));
        TN *dst_tn = OP_result(op, OP_Copy_Result(op));
        if (src_tn == dst_tn) {
          BB_Remove_Op(bb, op);
//           OP_Change_To_Noop(op);
          Reset_BB_scheduled(bb);
        }
      }
    }
  }
}


/* ============================================================================
 *   CG_Coalesce_PU
 * ============================================================================
 */
BOOL
CG_Coalesce_PU() {

  if (BB_next(REGION_First_BB) == NULL) {
    // Nothing to do in single BB function
    return 0;
  }

  //
  // Initialize
  //
  Trace_Build = Get_Trace(TP_COALESCE, TRACE_COALESCE_BUILD);
  Trace_Optimize = Get_Trace(TP_COALESCE, TRACE_COALESCE_OPT);

  Start_Timer( T_Coalesce_CU );

  if (Trace_Build || Trace_Optimize) {
    fprintf (TFile,
             COALESCE_PREFIX" ==================================\n"
             COALESCE_PREFIX" ========  Coalecing pass  ========\n"
             COALESCE_PREFIX" ==================================\n");
  }

  MEM_POOL_Initialize(&coalesce_local_pool, "COALESCE_local_pool", TRUE);
  MEM_POOL_Push(&coalesce_local_pool);

  TN_CoalInfo_map = new CoalescingInfoMap_t();
  TN_Rename_map = new TN2TNMap_t();
  move_list = new list<OP*>();

  PU_transfo_counter = 0;


  //
  // Main code
  //
  Build_Interference_Graph();
  Select_Candidate_Move();
  Rename_TNs();


  //
  // Cleanup
  //
  delete move_list;
  delete TN_Rename_map;
  delete TN_CoalInfo_map;

  MEM_POOL_Pop(&coalesce_local_pool);
  MEM_POOL_Delete(&coalesce_local_pool);

  Stop_Timer ( T_Coalesce_CU );

  return (PU_transfo_counter > 0);
}
