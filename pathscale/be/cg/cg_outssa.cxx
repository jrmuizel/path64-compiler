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

/* ====================================================================
 * ====================================================================
 *
 * Module: cg_outssa.cxx
 *
 * Description:
 *
 * Perform the out-of-SSA transformation
 *
 * ====================================================================
 * ====================================================================
 */


#include <stdlib.h>
#include "tracing.h"
#include "cxx_memory.h"
#include "glob.h"
#include "bb.h"
#include "op.h"
#include "tn_map.h"
#include "op_map.h"
#include "tn_list.h"
#include "gtn_universe.h"
#include "gtn_set.h"
#include "cgexp.h"
#include "dominate.h"
#include "gra_live.h"
#include "cg_ssa.h"
#include "cg_outssa.h"

static MEM_POOL outssa_pool;

static BOOL Trace_SSA_Out;                    /* -Wb,-tt60:0x002 */
static BOOL Trace_phi_removal;                /* -Wb,-tt60:0x008 */

/* ================================================================
 * ================================================================
 *                     TN SSA Info
 * ================================================================
 * ================================================================
 */

// TBD: Merge this with the ssa_def ??
// TN_SSA_Info

typedef struct SSA_Info SSA_Info_t;

struct SSA_Info {
  UINT8       PCopy_isDef:1;
  UINT8       PCopy_isUse:1;
  UINT32      ssaIdx;
  TN	     *tn;
  TN         *EquiValue;
  SSA_Info_t *RootInfo;
  INT         Rank;
  //  SSA_Info_t *RootInfo;
};

INT SSA_Info_SIZE;
INT SSA_Info_COUNT;
std::list<SSA_Info_t *> *SSA_Info_ARRAY;
// SSA_Info_ARRAY_CHUNK must be a power of 2
#define SSA_Info_ARRAY_CHUNK 256

static TN_MAP tn_ssa_info_map = NULL;
static UINT32 Last_ssaIdx;

// For all variables occuring as an argument of PSI operations,
// collects its actual last use.
static OP_MAP map_psi_use = NULL;

static void
TN_SSA_Info_init() {

  SSA_Info_SIZE = 0;
  SSA_Info_COUNT = 0;
  SSA_Info_ARRAY = CXX_NEW(std::list<SSA_Info_t *>, &outssa_pool);
    
  tn_ssa_info_map = TN_MAP_Create();
  Last_ssaIdx = 0;
}

static void
TN_SSA_Info_fini() {

  if (SSA_Info_ARRAY != NULL) {
    // First, delete the individual arrays
    std::list<SSA_Info_t *>::iterator ARRAY_CHUNK_iter;
    for (ARRAY_CHUNK_iter = SSA_Info_ARRAY->begin(); ARRAY_CHUNK_iter != SSA_Info_ARRAY->end(); ARRAY_CHUNK_iter++) {
      CXX_DELETE_ARRAY(*ARRAY_CHUNK_iter, &outssa_pool);
    }
    
    // Then, delete the list
    SSA_Info_ARRAY->clear();
    CXX_DELETE(SSA_Info_ARRAY, &outssa_pool);
    SSA_Info_ARRAY = NULL;
  }
    
  SSA_Info_SIZE = 0;
  SSA_Info_COUNT = 0;

  TN_MAP_Delete(tn_ssa_info_map);
  tn_ssa_info_map = NULL;
}

static SSA_Info_t *
TN_SSA_Info_new(TN *tn) {

  if (SSA_Info_COUNT == SSA_Info_SIZE) {
    // Allocate a new chunk of memory
    INT new_size = SSA_Info_SIZE+SSA_Info_ARRAY_CHUNK;
    SSA_Info_t *new_array = CXX_NEW_ARRAY(SSA_Info_t, SSA_Info_ARRAY_CHUNK, &outssa_pool);

    SSA_Info_ARRAY->push_back(new_array);
    SSA_Info_SIZE = new_size;
  }
  
  Is_True(SSA_Info_COUNT == Last_ssaIdx, ("TN_SSA_Info_new: Internal error"));

  SSA_Info_t *array_chunk = SSA_Info_ARRAY->back();
  SSA_Info_t *ssa_info = &array_chunk[SSA_Info_COUNT&(SSA_Info_ARRAY_CHUNK-1)];
  SSA_Info_COUNT ++;

  TN_MAP_Set(tn_ssa_info_map, tn, ssa_info);
  ssa_info->tn = tn;

  ssa_info->ssaIdx = Last_ssaIdx++;
  ssa_info->EquiValue = tn;

  // Initialize union-find 
  ssa_info->RootInfo = NULL;
  ssa_info->Rank = 0;

  return ssa_info;
}

static inline SSA_Info_t *
SSA_Info_TN_get(const TN *tn) {
  return TN_is_register (tn) ? (SSA_Info_t *)TN_MAP_Get(tn_ssa_info_map, tn) : NULL;
}

static SSA_Info_t *
SSA_Info_TN_add(TN *tn) {
  SSA_Info_t *ssa_info;
  // tn may be a non-ssa variable
  if ((ssa_info = SSA_Info_TN_get(tn)) == NULL)
    ssa_info = TN_SSA_Info_new(tn);
  return ssa_info;
}

#define SSA_Info_idx(ssa_info) (ssa_info)->ssaIdx
#define SSA_Info_tn(ssa_info) (ssa_info)->tn
#define SSA_Info_TN_idx(tn) SSA_Info_idx(SSA_Info_TN_get(tn))

static SSA_Info_t *
SSA_Info_idx_get(INT idx) {

  INT chunk_idx = 0;
  INT base_idx = idx&~(SSA_Info_ARRAY_CHUNK-1);

  std::list<SSA_Info_t *>::iterator ARRAY_CHUNK_iter;
  for (ARRAY_CHUNK_iter = SSA_Info_ARRAY->begin(), chunk_idx = 0;
       chunk_idx < base_idx;
       ARRAY_CHUNK_iter++, chunk_idx += SSA_Info_ARRAY_CHUNK);

  Is_True(chunk_idx == base_idx, ("Internal error on SSA_Info_idx"));

  return &(*ARRAY_CHUNK_iter)[idx-chunk_idx];
}

#define PCopy_isDef(tn) (SSA_Info_TN_get(tn)->PCopy_isDef)
#define PCopy_isUse(tn) (SSA_Info_TN_get(tn)->PCopy_isUse)

#define PCopy_resetDef(tn) SSA_Info_TN_get(tn)->PCopy_isDef = 0
#define PCopy_resetUse(tn) SSA_Info_TN_get(tn)->PCopy_isUse = 0
#define PCopy_setDef(tn) SSA_Info_TN_get(tn)->PCopy_isDef = 1
#define PCopy_setUse(tn) SSA_Info_TN_get(tn)->PCopy_isUse = 1

/* ================================================================
 * ================================================================
 *                     Copies and Parallel Copies
 * ================================================================
 * ================================================================
 */

/* =======================================================================
 *   Copy_TN
 *
 *   a little wrapper here to make things simpler ...
 *
 *   make a copy of 'tn' and some bookkeeping here :
 * =======================================================================
 */
static TN *
Copy_TN (
  TN *tn
)
{
  Is_True(TN_is_register(tn),("not a register tn"));

  //
  // Is supposed to return a different TN_number but else
  // identical
  //
  return Dup_TN(tn);
}

OP *
PCOPY_add_copy(OP* par_copy, TN *dst, TN *src) {
  Is_True(OP_results(par_copy) == OP_opnds(par_copy), ("Inconsistent parallel copy op"));

  // TBD: Can be optimized by allocating more space, and let NULL
  // arguments at the end that will be filled later
  INT new_results = OP_results(par_copy)+1;
  OP *new_par_copy = Resize_OP(par_copy, new_results, new_results);
  Set_OP_opnd(new_par_copy, new_results-1, src);
  Set_OP_result(new_par_copy, new_results-1, dst);

  // Replace the old pcopy by the new pcopy, this also updates the SSA
  // def links
  if (OP_bb(par_copy) != NULL)
    BB_Replace_Op(par_copy, new_par_copy);
  return new_par_copy;
}

static inline BOOL
BB_has_PHI(BB *bb) {
  OP *first_op = BB_first_op(bb);
  return (first_op != NULL) && (OP_phi(first_op));
}

// Return the PCOPY at the beginning of a BB
static OP *
BB_phi_res_PCOPY(BB *bb) {

  OP *op;
  for (op = BB_first_op(bb); (op != NULL) && (OP_phi(op)); op = OP_next(op));

  if ((op != NULL) && OP_PCOPY(op))
    return op;

  return NULL;
}

// Return the PCOPY at the end of a BB
static OP *
BB_phi_args_PCOPY(BB *bb) {

  OP *op = BB_last_op(bb);
  // In case of a call, the PCOPY is placed after the call
  if ((op != NULL) && OP_br(op))
    op = OP_prev(op);

  if ((op != NULL) && OP_PCOPY(op))
    return op;

  return NULL;
}

// Insert parallel copies in a BB. Because we need to distinguish
// between the PCOPY and the entry and at the exit of the basic block,
// we must insert these two pcopies at once. However, no PCOPY is
// generated at the entry of the basic block if the basic block has no
// PHI, since we know in this case that the unique PCOPY must be at
// the exit of the basic block.
static void
BB_insert_pcopies(BB *bb) {

  // If the basic block contains PHI operations, insert a PCOPY after
  // the last PHI.
  if (BB_has_PHI(bb)) {
    // Find the last PHI OP
    OP *op = BB_first_op(bb);
    Is_True(op && (OP_phi(op)), ("Found a BB with a PHI that is not the first operation"));

    for (OP *next_op = OP_next(op);
	 (next_op != NULL) && (OP_phi(next_op));
	 next_op = OP_next(op))
      op = next_op;

    OP *pcopy_first = Mk_VarOP(TOP_PCOPY, 0, 0, NULL, NULL);
    Set_OP_ssa_move(pcopy_first);
    BB_Insert_Op_After(bb, op, pcopy_first);
  }

  // Also always insert a PCOPY at the end of the BB, for PHIs in
  // successors.
  if (BB_succs_len(bb) > 0) {
    OP *point = BB_last_op(bb);
    // Insert the pcoy before a branch but after a call
    BOOL before = (point != NULL) && OP_br(point);
    OP *pcopy_last = Mk_VarOP(TOP_PCOPY, 0, 0, NULL, NULL);
    Set_OP_ssa_move(pcopy_last);
    BB_Insert_Op(bb, point, pcopy_last, before);
  }
}

/* ================================================================
 *   insert_operand_pcopy
 *
 *   Insert a copy of given TN in the PCOPY at the end of given BB.
 * ================================================================
 */
static TN *
insert_PHI_operand_pcopy (
  OP   *phi_op,
  INT8  opnd_idx,
  BB   *in_bb
)
{
  TN *tn = OP_opnd(phi_op, opnd_idx);
  //FdF 20041014: Fix for DDTS MBTst19376. Register must be
  //global. Also, use Copy_TN instead of Dup_TN, to reset the
  //gra_homeable value.
  TN *new_tn = Copy_TN(tn);
  Is_True(TN_is_global_reg(tn), ("Unexpected NON-global TN on PHI argument"));
  Set_TN_is_global_reg(new_tn);

  // replace old tn in the phi OP
  Set_OP_opnd(phi_op, opnd_idx, new_tn);

  OP *pcopy_op = BB_phi_args_PCOPY(in_bb);
  Is_True(pcopy_op, ("No PCOPY in BB %d", BB_id(in_bb)));

  PCOPY_add_copy(pcopy_op, new_tn, tn);
  SSA_Info_TN_add(tn);
  SSA_Info_TN_add(new_tn);

  return new_tn;
}

/* ================================================================
 *   insert_result_pcopy
 *
 *   Insert a copy of PHI result TN at the beginning of given BB
 *   (right after the PHI-nodes).
 * ================================================================
 */
static TN *
insert_PHI_result_pcopy (
  OP   *phi_op,
  BB   *in_bb
)
{
  TN *tn = OP_result(phi_op, 0);
  //FdF 20041014: Fix for DDTS MBTst19376. Register must be
  //global. Also, use Copy_TN instead of Dup_TN, to reset the
  //gra_homeable value.
  TN *new_tn = Copy_TN(tn);
  if (TN_is_global_reg(tn))
    Set_TN_is_global_reg(new_tn);

  // replace old tn in the phi OP
  Set_OP_result(phi_op, 0, new_tn);

  // Update the SSA use-def link.
  Set_TN_ssa_def(tn , NULL);
  Set_TN_ssa_def(new_tn, phi_op);

  OP *pcopy_op = BB_phi_res_PCOPY(in_bb);
  Is_True(pcopy_op, ("No PCOPY in BB %d", BB_id(in_bb)));

  PCOPY_add_copy(pcopy_op, tn, new_tn);
  SSA_Info_TN_add(tn);
  SSA_Info_TN_add(new_tn);
  return new_tn;
}

/* ================================================================
 *   PCOPY_sequentialize
 *
 *   Replace a parallele COPY by a sequence of copies, breaking cycles
 *   when needed. The algorithm is the following:
 *   for each copy (dst <- src) {
 *     if ((there is already a copy (src <- ...)) &&
 *         (there is already a copy (... <- dst))) { // break cycle
 *        insert (tmp <- src) before first copy
 *        append (dst <- tmp) after last copy
 *     }
 *     else if (there is already a copy (src <- ...)) {
 *         // there is no copy (... <- dst))
 *        insert (dst <- src) before first copy
 *     }
 *     else {
 *        // there is no copy (src <- ...)
 *        // there may be a copy (... <- dst)
 *       append (dst <- src) after last copy
 *     }
 *  }
 *
 *   Also, ignore multiple copies with the same dst, we assume that
 *   the PCOPY is consistent and thus they all have the same src.
 *   Finally, do not generate copies when src and dst are the same
 * ================================================================
 */

static void
PCOPY_insert_copy(TN *dst, TN *src, OP *par_copy, OPS *seq_copies, BOOL before) {
  OPS tmp_ops = OPS_EMPTY;
  Exp_COPY(dst, src, &tmp_ops);
  Set_OP_ssa_move(OPS_last(&tmp_ops));
  if (OP_prologue(par_copy)) Set_OP_prologue(OPS_last(&tmp_ops));
  if (OP_epilogue(par_copy)) Set_OP_epilogue(OPS_last(&tmp_ops));
  OPS_Insert_Ops(seq_copies, NULL, &tmp_ops, before);
}

static void
PCOPY_sequentialize(OP *par_copy) {

  Is_True(OP_results(par_copy) == OP_opnds(par_copy), ("Inconsistent parallel copy op"));

  OPS seq_copies = OPS_EMPTY;
  SSA_unset(par_copy);

  for (int i = 0; i < OP_results(par_copy); i++) {

    TN *dst = OP_result(par_copy, i);
    PCopy_resetDef(dst);
    PCopy_resetUse(dst);
    
    TN *src = OP_opnd(par_copy, i);
    PCopy_resetDef(src);
    PCopy_resetUse(src);
  }
    
  for (int i = 0; i < OP_results(par_copy); i++) {
    TN *dst = OP_result(par_copy, i);
    TN *src = OP_opnd(par_copy, i);
    // A TN can occur several times as a dst with each time the same
    // src
    if (PCopy_isDef(dst) || (src == dst))
      continue;

    if (PCopy_isDef(src) && PCopy_isUse(dst)) {
      // break cycle
      TN *tmp = Dup_TN_Even_If_Dedicated(dst);
      PCOPY_insert_copy(tmp, src, par_copy, &seq_copies, TRUE);
      PCOPY_insert_copy(dst, tmp, par_copy, &seq_copies, FALSE);
    }
    else if (PCopy_isDef(src))
      // Insert copy before all
      PCOPY_insert_copy(dst, src, par_copy, &seq_copies, TRUE);
    else
      // Insert copy after all
      PCOPY_insert_copy(dst, src, par_copy, &seq_copies, FALSE);

    PCopy_setDef(dst);
    PCopy_setUse(src);
  }

  BB_Insert_Ops_Before(OP_bb(par_copy), par_copy, &seq_copies);
  BB_Remove_Op(OP_bb(par_copy), par_copy);
}

static void
SSA_Sequentialize_PCopies() {
  BB *bb;
  OP *pcopy;

  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    OP *op, *op_next;
    for (op = BB_first_op(bb); op != NULL; op = op_next) {
      op_next = OP_next(op);
      if (OP_PCOPY(op))
	PCOPY_sequentialize(op);
    }
  }
}

/* ================================================================
 * ================================================================
 *                     SSA Value Equivalence
 * ================================================================
 * ================================================================
 */

static void
SSA_EquiValues_Copy(TN *dst, TN *src) {
  if (TN_is_ssa_var(dst) && TN_is_ssa_var(src)) {
    // Add an SSA_Info only for TNs that will or may be coalesced, so
    // as to reduce the size of the interference graph.
    SSA_Info_t *dst_ssa_info = SSA_Info_TN_add(dst);
    SSA_Info_t *src_ssa_info = SSA_Info_TN_add(src);
    dst_ssa_info->EquiValue = src_ssa_info->EquiValue;
  }
}

static void
SSA_EquiValues_BB(BB *bb, BOOL *visited) {

  if (visited[BB_id(bb)]) return;

  OP *op;
  FOR_ALL_BB_OPs_FWD(bb, op) {

    if (OP_copy(op)) {
      TN *dst = OP_result(op, OP_Copy_Result(op));
      TN *src = OP_opnd(op, OP_Copy_Operand(op));
      SSA_EquiValues_Copy(dst, src);
    }

    else if (OP_PCOPY(op)) {
      for (INT i = 0; i < OP_results(op); i++) {
	TN *dst = OP_result(op, i);
	TN *src = OP_opnd(op, i);
	SSA_EquiValues_Copy(dst, src);
      }
    }
  }

  for (BB_LIST *elist = BB_children(bb); elist; elist = BB_LIST_rest(elist)) {
    BB *kid = BB_LIST_first(elist);
    SSA_EquiValues_BB (kid, visited);
  }

  visited[BB_id(bb)] = TRUE;
}

static void
SSA_EquiValues() {

  BOOL visited[PU_BB_Count+2];
  BZERO(visited, sizeof(BOOL)*(PU_BB_Count+2));

  //
  // visit nodes in the dominator tree order, propagating values
  //
  const BB_SET *region_entry_set = SSA_region_entries();
  BB *bb;
  FOR_ALL_BB_SET_members(region_entry_set, bb) {
    SSA_EquiValues_BB (bb, visited);
  }
}

/* ================================================================
 * ================================================================
 *                     Interference Graph
 * ================================================================
 * ================================================================
 */

//
// IGRAPH -- Interference Graph.
//
// Only edge information is necessary unlike the real interference
// graph with node degrees, costs, etc.
// 
// We only keep interferences for SSA TNs.
//

// We set a 'tn_imap' entry for each PHI-function resource.
// There may be at most as many copies made as there are operands +
// results of the PHI functions (one copy for each operand/result in
// the worst case for each PHI-function present at the beginnig of
// the SSA_Make_Conventional() process). 

//
// PHI resources interference map, IGRAPH
//
static BS **tn_imap;    // IGRAPH map, implemented as a bitset

static UINT32 Igraph_Size; // Number of TNs registered in the Interference Graph

/* ================================================================
 *   IGRAPH_init_tn
 *
 *   Initialize the tn_map -- bitvector indicating interference TNs
 * ================================================================
 */
inline void IGRAPH_init_tn (TN *tn) {
  INT idx = SSA_Info_TN_idx(tn);
  tn_imap[idx] = BS_Create_Empty(Igraph_Size, &MEM_local_pool);
  return;
}

// Interference Graph Tracing
static BOOL Trace_Igraph;

// Whether IGRAPH is in use
static BOOL Igraph_Used = FALSE;

// working BS used to coalesce two columns in the interference graph
static BS *BS_diff_2_1 = NULL;

/* ================================================================
 *   IGRAPH_print_tn
 * ================================================================
 */
static void
IGRAPH_print_tn (
  INT i,
  FILE *file
)
{
  fprintf(file, "ID%d -> {", i);

  //
  // Interference info found ??
  //
  if (tn_imap[i] != NULL) {
    BS_ELT idx;
    for (idx = BS_Choose(tn_imap[i]);
	 idx != BS_CHOOSE_FAILURE;
	 idx = BS_Choose_Next(tn_imap[i], idx)) {

      fprintf(file, " ID%d", idx);
    }
  }

  fprintf(file, " }\n");

  return;
}

/* ================================================================
 *   IGRAPH_Print
 * ================================================================
 */
static void
IGRAPH_Print (
  FILE *file
)
{
  INT i;

  fprintf (file, "  --- Interference Graph --- \n");

  fprintf (file, "   Igraph Size = %d\n", Igraph_Size);

  for (i = 1; i <= Last_TN; i ++) {
    TN *tn = TNvec(i);
    if ((tn != NULL) && (SSA_Info_TN_get(tn) != NULL)) {
      fprintf(file, "  %cTN%d: ", TN_is_global_reg(tn) ? 'G' : ' ', TN_number(tn));
      IGRAPH_print_tn(SSA_Info_TN_idx(tn), file);
    }
  }

  return;
}

/* ================================================================
 *   IGRAPH_Initialize
 * ================================================================
 */
static void
IGRAPH_Initialize ()
{
  // The size of the Igraph is the number of SSA TNs. No more SSA TNs
  // must be created from this point on.
  Igraph_Size = SSA_Info_COUNT;

  tn_imap = (BS **)TYPE_MEM_POOL_ALLOC_N(BS *, &MEM_local_pool, Igraph_Size);

  // initialize interference to NULL
  BZERO(tn_imap, sizeof(BS*)*Igraph_Size);

  Igraph_Used = TRUE;

  // Initialize the working BS to merge columns in the interference graph
  BS_diff_2_1 = BS_Create(Igraph_Size, &MEM_local_pool);

  return;
}

/* ================================================================
 *   IGRAPH_Clean
 * ================================================================
 */
static void
IGRAPH_Clean ()
{

  // normally everything will go away with the MEM_local_pool.

  Igraph_Used = FALSE;

  BS_diff_2_1 = NULL;

  return;
}

/* ================================================================
 *   IGRAPH_Add_Interference
 * ================================================================
 */
static void
IGRAPH_Add_Interference (
  TN *tn1, 
  TN *tn2
) 
{
  Is_True(Igraph_Used, ("IGRAPH is not available "));
  INT32 idx1 = SSA_Info_TN_idx(tn1);
  INT32 idx2 = SSA_Info_TN_idx(tn2);

  if (Trace_Igraph) {
    fprintf(TFile, "    IGRAPH_Add_Interference: ");
    Print_TN(tn1, FALSE);
    fprintf(TFile, " [%d] and ", idx1);
    Print_TN(tn2, FALSE);
    fprintf(TFile, " [%d] \n", idx2);
    fflush(TFile);
  }

  //
  // If no interferences have been found before, initialize
  // the structure
  //
  if (tn_imap[idx1] == NULL) IGRAPH_init_tn(tn1);
  if (tn_imap[idx2] == NULL) IGRAPH_init_tn(tn2);

  tn_imap[idx1] = BS_Union1D (tn_imap[idx1], idx2, &MEM_local_pool);
  tn_imap[idx2] = BS_Union1D (tn_imap[idx2], idx1, &MEM_local_pool);

  return;
}

/* ================================================================
 *   SSA_Build_IGraph_tn
 * ================================================================
 */
static void
SSA_Build_IGraph_tn (
  TN *def_tn,
  TN_SET *live_tns
)
{
  // Get the SSA_info
  SSA_Info_t *tn_ssa_info = SSA_Info_TN_get(def_tn);
  Is_True(tn_ssa_info, ("SSA_Build_IGraph_tn must be called with tn with SSA_Info"));

  // interference with 'live_tns' TNs
  for (TN *live_tn = TN_SET_Choose(live_tns);
       live_tn != TN_SET_CHOOSE_FAILURE;
       live_tn = TN_SET_Choose_Next(live_tns, live_tn)) {

    // do not count interference with itself
    if (live_tn == def_tn) continue;

    SSA_Info_t *cur_tn_ssa_info = SSA_Info_TN_get(live_tn);

    // Do not add interferences when values are the same
    if ((cur_tn_ssa_info != NULL) && (tn_ssa_info->EquiValue != cur_tn_ssa_info->EquiValue))
      IGRAPH_Add_Interference(def_tn, live_tn);
  }
}

/* ================================================================
 *   SSA_Build_IGraph_bb
 * ================================================================
 */
static void
SSA_Build_IGraph_bb (
  BB *bb,
  BOOL *visited
)
{
  visited[BB_id(bb)] = TRUE;

  //
  // It is a bottom-up traversal, so check the successors first
  //
  if (!BB_is_SSA_region_exit(bb) ) {
    BBLIST *succs;
    FOR_ALL_BB_SUCCS(bb, succs) {
      BB *succ = BBLIST_item(succs);
      if (!visited[BB_id(succ)])
	SSA_Build_IGraph_bb (succ, visited);
    }
  }

  if (Trace_Igraph) {
    fprintf(TFile, "====== BB%d: \n", BB_id(bb));
    GRA_LIVE_Print_Liveness(bb);
    fprintf(TFile, "====== \n");
  }

  TN_SET *current = TN_SET_Create_Empty (Last_TN + 1,&MEM_local_pool);
  for (TN *tn = GTN_SET_Choose(BB_live_out(bb));
       tn != GTN_SET_CHOOSE_FAILURE;
       tn = GTN_SET_Choose_Next(BB_live_out(bb), tn)) {
    // Add only SSA TNs
    if (SSA_Info_TN_get(tn))
      current = TN_SET_Union1D(current, tn, &MEM_local_pool);
  }

  //
  // At this point, the 'current' holds all TNs exposed
  // down from this 'bb' (still live)
  //
  OP *op;
  FOR_ALL_BB_OPs_REV(bb,op) {
    TN *res;

    if (Trace_Igraph) {
      fprintf(TFile, "  ");
      Print_OP_No_SrcLine(op);
      fprintf(TFile, "  current before op: ");
      TN_SET_Print(current, TFile);
      fprintf(TFile, "\n");
    }

    if (OP_phi(op)) {
      res = OP_result(op,0);
      Is_True(SSA_Info_TN_get(res), ("PHI result must be a ssa var"));

      for (TN *tn = TN_SET_Choose(current);
	   tn != TN_SET_CHOOSE_FAILURE;
	   tn = TN_SET_Choose_Next(current, tn)) {

	if ((tn != res) && SSA_Info_TN_get(tn))
	  IGRAPH_Add_Interference(res, tn);
      }

      // Nothing more to do on a PHi. In particular, do not remove the
      // def of a PHI in the set of current live, they all are
      // evaluated in parallel.
    }

    else {

      if (OP_code(op) == TOP_asm) {
	// Consider that all results are live to create interferences
	// between results
	for (INT res_idx = 0; res_idx < OP_results(op); res_idx++) {
	  res = OP_result(op, res_idx);
	  current = TN_SET_Union1D(current, res, &MEM_local_pool);
	}
      }

      // results
      for (INT res_idx = 0; res_idx < OP_results(op); res_idx++) {
	res = OP_result(op, res_idx);

	if (SSA_Info_TN_get(res) == NULL)
	  continue;

	SSA_Build_IGraph_tn(res, current);

	// In case of a Conflict constraint on a result, add
	// interferences with the operands on which there is a
	// constraint.
	if (OP_uniq_res(op, res_idx)) {
	  for (INT opnd_idx = 0; opnd_idx < OP_opnds(op); opnd_idx++) {
	    if (SSA_Info_TN_get(OP_opnd(op, opnd_idx)) &&
		OP_conflict(op, res_idx, opnd_idx))
	      IGRAPH_Add_Interference(res, OP_opnd(op, opnd_idx));
	  }
	}

	// remove res from 'current'
	if (Trace_Igraph) {
	  fprintf(TFile, "  remove res from current: ");
	  Print_TN(res, FALSE);
	  fprintf(TFile, "\n");
	}
	current = TN_SET_Difference1D(current, res);
      }
    }

    // FdF 20090901: For call operations, also consider interferences
    // with clobbered registers.
    if (OP_call(op)) {
      ISA_REGISTER_CLASS rc;
      FOR_ALL_ISA_REGISTER_CLASS(rc) {
	REGISTER_SET clobber_set = BB_call_clobbered(bb, rc);
	REGISTER reg;
	FOR_ALL_REGISTER_SET_members(clobber_set, reg) {
	  TN *clobber_tn = Build_Dedicated_TN(rc, reg, 0);
	  if (SSA_Info_TN_get(clobber_tn) != NULL)
	    SSA_Build_IGraph_tn(clobber_tn, current);
	}
      }
    }

    // Do not analyze the operand of PHI and PSI operantions, only
    // implicit uses on predicated operations must be considered.
    if (!OP_phi(op) && !OP_psi(op)) {

      // operands

      // We add the operands to the 'current' of this BB.
    
      for (INT opnd_idx = 0; opnd_idx < OP_opnds(op); opnd_idx++) {
	TN *opnd = OP_opnd(op, opnd_idx);

	// add to live_out set if it's a global, non dedicated
	// register TN
	if (SSA_Info_TN_get(opnd)) {
	  if (Trace_Igraph) {
	    fprintf(TFile, "  add opnd to current: ");
	    Print_TN(opnd, FALSE);
	    fprintf(TFile, "\n");
	  }
	  current = TN_SET_Union1D(current, opnd, &MEM_local_pool);
	}
      }

      if (Trace_Igraph) {
	fprintf(TFile, "  current after op: ");
	TN_SET_Print(current, TFile);
	fprintf(TFile, "\n");
      }
    }

    // Then, for all operations, consider extra uses from PSI operations.
    for (TN_LIST *p = (TN_LIST *)OP_MAP_Get(map_psi_use, op); p; p = TN_LIST_rest(p)) {
      TN *tn_psi_use = TN_LIST_first(p);

      // add to live_out set if it's a global, non dedicated
      // register TN
      if (SSA_Info_TN_get(tn_psi_use)) {
	if (Trace_Igraph) {
	  fprintf(TFile, "  add psi_use to current: ");
	  Print_TN(tn_psi_use, FALSE);
	  fprintf(TFile, "\n");
	}
	current = TN_SET_Union1D(current, tn_psi_use, &MEM_local_pool);
      }
    }

    if (Trace_Igraph) {
      fprintf(TFile, "  current after op: ");
      TN_SET_Print(current, TFile);
      fprintf(TFile, "\n");
    }
  } // FOR_ALL_BB_OPs_REV

  return;
}

/* ================================================================
 *   IGRAPH_Build
 * ================================================================
 */
static void
SSA_Build_IGraph ()
{
  BB *bb;

  if (Trace_Igraph) {
    fprintf(TFile, "%s\t\t\t IGRAPH_Build \n%s\n", DBar, DBar);
  }

  //
  // Initialize IGRAPH data structures
  //
  IGRAPH_Initialize ();

  // calculate interference based on the liveness
  BOOL visited[PU_BB_Count+2];
  // when we arrive at a BB, we set this to TRUE and go see its
  // successors. When all successors have been seen and the BB
  // processed, we unset to FALSE, so we know that only those BBs with
  // visited set to TRUE are left.
  BZERO(visited, sizeof(BOOL)*(PU_BB_Count+2));

  const BB_SET *region_entry_set = SSA_region_entries();
  FOR_ALL_BB_SET_members(region_entry_set,bb) {
    if (visited[BB_id(bb)] == FALSE)
      SSA_Build_IGraph_bb(bb, visited);
  }

  // Add the click for all dedicated registers in the representation
  TN *tni;
  TN_NUM tni_num;
  for (tni_num = 0; tni_num <= Last_Dedicated_TN; tni_num++) {
    if (((tni = TNvec(tni_num)) != NULL) && (SSA_Info_TN_get(tni) != NULL)) {
      TN *tnj;
      TN_NUM tnj_num;
      for (tnj_num = 0; tnj_num <= Last_Dedicated_TN; tnj_num++) {
	if (tni_num == tnj_num) continue;
	if (((tnj = TNvec(tnj_num)) != NULL) && (SSA_Info_TN_get(tnj) != NULL)) {
	  IGRAPH_Add_Interference(tni, tnj);
	}
      }
    }
  }

  if (Trace_SSA_Out) {
    IGRAPH_Print(TFile);
  }

  return;
}

/* ================================================================
 *   IGRAPH_TNs_Interfere
 * ================================================================
 */
static BOOL
IGRAPH_TNs_Interfere (
  TN *tn1,
  TN *tn2
)
{
  Is_True(Igraph_Used, ("IGRAPH is not available "));
  Is_True(SSA_Info_TN_get(tn1) && SSA_Info_TN_get(tn2),
	  ("Checking interferences between TNs not in the interference graph"));
  INT32 idx1 = SSA_Info_TN_idx(tn1);
  INT32 idx2 = SSA_Info_TN_idx(tn2);

  //
  // tn_imap[idx] == NULL means no interference found !
  //
  if (tn_imap[idx1] == NULL || tn_imap[idx2] == NULL)
    return FALSE;

  if (BS_MemberP(tn_imap[idx1], idx2)) {
    return TRUE;
  }

  return FALSE;
}

/* ================================================================
 *   IGRAPH_TNs_Coalesce
 *   Coalesce root2 interferences into root1 interfences
 * ================================================================
 */

static void
IGRAPH_TNs_Coalesce (
  TN *root1,
  TN *root2
)
{
  INT32 idx1 = SSA_Info_TN_idx(root1);
  INT32 idx2 = SSA_Info_TN_idx(root2);

  if (Trace_Igraph) {
    fprintf(TFile, "    IGRAPH_TNs_Coalesce: ");
    Print_TN(root1, FALSE);
    fprintf(TFile, " [%d] <- ", idx1);
    Print_TN(root2, FALSE);
    fprintf(TFile, " [%d] \n", idx2);
    fflush(TFile);
  }

  // No interference to coalesce
  if (tn_imap[idx2] == NULL)
    return;

  if (tn_imap[idx1] == NULL) IGRAPH_init_tn(root1);

  // This merges the column for idx2 into the column for idx1
  BS_diff_2_1 = BS_CopyD(BS_diff_2_1, tn_imap[idx2], NULL);
  BS_diff_2_1 = BS_DifferenceD(BS_diff_2_1, tn_imap[idx1]);

  BS_ELT idx;
  for (idx = BS_Choose(BS_diff_2_1);
       idx != BS_CHOOSE_FAILURE;
       idx = BS_Choose_Next(BS_diff_2_1, idx))
    BS_Union1D(tn_imap[idx], idx1, NULL);

  // This merges the line for idx2 into the line for idx1
  BS_UnionD(tn_imap[idx1], tn_imap[idx2], NULL);
}

/* ================================================================
 * ================================================================
 * 		Explicit Copies for PHI, SameRes, Dedicateds
 * ================================================================
 * ================================================================
 */

// Insert copies for PHis as in Sreedhar I
static void
SSA_insert_PHI_pcopies(BB *bb, BOOL *has_pcopy) {

  if (!BB_has_PHI(bb))
    return;

  if (!has_pcopy[BB_id(bb)]) {
    BB_insert_pcopies(bb);
    has_pcopy[BB_id(bb)] = TRUE;
  }
      
  BBLIST *edge;
  FOR_ALL_BB_PREDS(bb, edge) {
    BB *pred = BBLIST_item(edge);
    if (!has_pcopy[BB_id(pred)]) {
      BB_insert_pcopies(pred);
      has_pcopy[BB_id(pred)] = TRUE;
    }
  }

  OP *phi;
  FOR_ALL_BB_PHI_OPs(bb, phi) {

    // Add an SSA_Info only for TNs that will or may be coalesced, so
    // as to reduce the size of the interference graph.

    // insert copies for operands
    for (INT i = 0; i < OP_opnds(phi); i++) {
      TN *old_opnd = OP_opnd(phi, i);
      TN *new_opnd = insert_PHI_operand_pcopy(phi, i, Get_PHI_Predecessor(phi,i));
    }

    // insert copies for result
    TN *old_res = OP_result(phi, 0);
    TN *new_res = insert_PHI_result_pcopy(phi, bb);
  }

  return;
}

// Take into account operations properties:
// - Insert copies for same_res property: automod
// - Create entries for the IGraph on conflict property
static void
SSA_operation_properties(OP *op) {

  if (OP_phi(op) || OP_psi(op) || OP_PCOPY(op))
    return;

  INT res_idx, opnd_idx;
  for (res_idx = 0; res_idx < OP_results(op); res_idx++) {

    if (!TN_is_ssa_var(OP_result(op, res_idx)))
      continue;

    // Check if res and opnd are not already the same, otherwise this
    // means that at least one of them has not been renamed under SSA.
    if (((opnd_idx = OP_same_res(op, res_idx)) != -1) &&
	(OP_result(op, res_idx) != OP_opnd(op, opnd_idx))) {

      TN *tn_opnd =  OP_opnd(op, opnd_idx);
      TN *new_tn = Copy_TN(tn_opnd);

      OPS copy_ops = OPS_EMPTY;
      Exp_COPY(new_tn, tn_opnd, &copy_ops);
      Set_OP_ssa_move(OPS_last(&copy_ops));
      BB_Insert_Ops_Before(OP_bb(op), op, &copy_ops);
      Set_OP_opnd(op, opnd_idx, new_tn);

      Set_OP_sameres(op);

      // Add an SSA_Info only for TNs that will or may be coalesced,
      // so as to reduce the size of the interference graph.

      SSA_Info_TN_add(new_tn);
      SSA_Info_TN_add(OP_result(op, res_idx));
    }

    // Check if res is in conflict with one or more operands, which
    // means they cannot use the same register
    if (OP_uniq_res(op, res_idx)) {
      for (INT opnd_idx = 0; opnd_idx < OP_opnds(op); opnd_idx++) {
	if (OP_conflict(op, res_idx, opnd_idx) && 
	    TN_is_ssa_var(OP_opnd(op, opnd_idx))) {

	  // Add an SSA_Info for these TNs so as to force an
	  // interference between these two registers
	  SSA_Info_TN_add(OP_opnd(op, opnd_idx));
	  SSA_Info_TN_add(OP_result(op, res_idx));
	}
      }
    }

  }
}

static void
SSA_Rename_TN_opnds(OP *first_op, OP *last_op, TN *old_tn, TN *new_tn) {

  OP *op;
  for (op = first_op; op != last_op; op = OP_next(op)) {
    for (INT i = 0; i < OP_opnds(op); i++) {
      if (OP_opnd(op, i) == old_tn)
	Set_OP_opnd(op, i, new_tn);
    }
  }
}

// Create SSA variables for dedicated registers used on Entry, Exit, Call
// Replace copies Rn=Vn by V'n=Vn; add Rn=V'n; before the call or return
// Replace copies Vn=Rn by Vn=V'n; add V'n=Rn; after a call or entry
static void
SSA_insert_dedicated_copies(OP *op) {

  // For the PCOPY in the prologue and in the epilogue of the
  // function, coalesing is mandatory. A new PCOPY is added in the
  // prologue after the last 'no_move op' and in the epilogue before
  // the first 'no_move' op so as to handle renaming constraints.

  // Now, entry and exit BBs are split such that the new PCOPY can be
  // added as the last/first operation of a entry/exit block.

  BB *bb = OP_bb(op);

  if (OP_PCOPY(op) && (OP_prologue(op) || OP_epilogue(op))) {
    // First, add TN_info for operands and results
    for (INT i = 0; i < OP_results(op); i++) {
      SSA_Info_TN_add(OP_result(op, i));
      SSA_Info_TN_add(OP_opnd(op, i));
    }
    
    if (OP_prologue(op)) {
      // Perform the following transformation
      // X <- Rx (PCOPY)         X' <- Rx (PCOPY) Coalesce (Rx,X')
      //   <- X  (no_move)          <- X' (no_move)
      //                         X  <- X' (PCOPY)
      OP *new_pcopy = NULL;
      for (INT i = 0; i < OP_results(op); i++) {
	TN *tn = OP_result(op, i);
	if (new_pcopy == NULL)
	  new_pcopy = Mk_VarOP(TOP_PCOPY, 0, 0, NULL, NULL);
	TN *new_tn = Copy_TN(tn);
	SSA_Info_TN_add(new_tn);
	Set_TN_ssa_def(tn, NULL);
	Set_OP_result(op, i, new_tn);
	Set_TN_ssa_def(new_tn, op);
	new_pcopy = PCOPY_add_copy(new_pcopy, tn, new_tn);
	// Replace uses of tn by new_tn in the operation between the
	// two PCOPY operations
	SSA_Rename_TN_opnds(OP_next(op), NULL, tn, new_tn);
      }
      if (new_pcopy) {
	Reset_OP_prologue(new_pcopy);
	Set_OP_ssa_move(new_pcopy);
	OP *point = BB_last_op(bb);
	BOOL before = (point != NULL) && OP_xfer(point);
	BB_Insert_Op(bb, point, new_pcopy, before);
      }
    }
    else if (OP_epilogue(op)) {
      // Perform the following transformation
      //                         X' <- X (PCOPY)
      //    <- X                    <- X'
      // Rx <- X (PCOPY)         Rx <- X' (PCOPY) Coalesce (Rx,X')
      OP *new_pcopy = NULL;
      for (INT i = 0; i < OP_opnds(op); i++) {
	TN *tn = OP_opnd(op, i);
	if (!TN_ssa_def(tn))
	  continue;
	if (OP_bb(TN_ssa_def(tn)) == bb) {
	  // If this opnd appears more than once in the PCOPY, make it
	  // unique
	  for (INT j = 0; j < i; j++) {
	    if (OP_opnd(op, j) == tn) {
	      TN *new_tn = Copy_TN(tn);
	      SSA_Info_TN_add(new_tn);
	      Set_OP_opnd(op, i, new_tn);
	      OPS tmp_ops = OPS_EMPTY;
	      Exp_COPY(new_tn, tn, &tmp_ops);
	      BB_Insert_Ops(bb, op, &tmp_ops, TRUE);
	      break;
	    }
	  }
	  continue;
	}
	if (new_pcopy == NULL)
	  new_pcopy = Mk_VarOP(TOP_PCOPY, 0, 0, NULL, NULL);
	TN *new_tn = Copy_TN(tn);
	SSA_Info_TN_add(new_tn);
	Set_OP_opnd(op, i, new_tn);
	new_pcopy = PCOPY_add_copy(new_pcopy, new_tn, tn);
	SSA_Rename_TN_opnds(BB_first_op(bb), op, tn, new_tn);
      }
      if (new_pcopy) {
	Reset_OP_epilogue(new_pcopy);
	Set_OP_ssa_move(new_pcopy);
	BB_Prepend_Op(bb, new_pcopy);
      }
    }
  }


  if (!OP_Has_ssa_pinning(op))
    return;

  // There is a special case for the SP and the FP registers. The uses
  // of these registers that are pinned must refer directly to the
  // pinned definition so as to preserve the whole live-range. The
  // traversal in dominance order ensure that definitions have already
  // been processed.

  OP *pcopy_opnd = Mk_VarOP(TOP_PCOPY, 0, 0, NULL, NULL);
  for (INT opnd_idx = 0; opnd_idx < OP_opnds(op); opnd_idx ++) {
    TN *pinned = OP_Get_opnd_pinning(op, opnd_idx);
    if (pinned != NULL) {
      TN *tn_opnd =  OP_opnd(op, opnd_idx);
      TN *new_tn = Copy_TN(tn_opnd);
      pcopy_opnd = PCOPY_add_copy(pcopy_opnd, new_tn, tn_opnd);
      Set_OP_opnd(op, opnd_idx, new_tn);
      SSA_Info_TN_add(new_tn);
      SSA_Info_TN_add(tn_opnd);
      SSA_Info_TN_add(pinned);
    }
  }
  if (OP_results(pcopy_opnd) > 0) {
    Set_OP_ssa_move(pcopy_opnd);
    BB_Insert_Op_Before(bb, op, pcopy_opnd);
  }

  OP *pcopy_result = Mk_VarOP(TOP_PCOPY, 0, 0, NULL, NULL);
  for (INT res_idx = 0; res_idx < OP_results(op); res_idx++) {
    TN *pinned = OP_Get_result_pinning(op, res_idx);
    if (pinned != NULL) {
      TN *tn_res =  OP_result(op, res_idx);
      TN *new_tn = Copy_TN(tn_res);
      Set_OP_result(op, res_idx, new_tn);
      Set_TN_ssa_def(tn_res, NULL);
      Set_TN_ssa_def(new_tn, op);
      pcopy_result = PCOPY_add_copy(pcopy_result, tn_res, new_tn);
      SSA_Info_TN_add(tn_res);
      SSA_Info_TN_add(new_tn);
      SSA_Info_TN_add(pinned);
    }
  }
  if (OP_results(pcopy_result) > 0) {
    Set_OP_ssa_move(pcopy_result);
    BB_Insert_Op_After(bb, op, pcopy_result);
  }
}

static void
SSA_expose_copies_BB(BB *bb, BOOL *has_pcopy, BOOL *visited) {

  if (visited[BB_id(bb)]) return;

  SSA_insert_PHI_pcopies(bb, has_pcopy);

  OP *op_next;
  for (OP *op = BB_first_op(bb); op; op = op_next) {
    op_next = OP_next(op);

    SSA_operation_properties(op);
    SSA_insert_dedicated_copies(op);
  }

  for (BB_LIST *elist = BB_children(bb); elist; elist = BB_LIST_rest(elist)) {
    BB *kid = BB_LIST_first(elist);
    SSA_expose_copies_BB (kid, has_pcopy, visited);
  }

  visited[BB_id(bb)] = TRUE;
}

static void
SSA_expose_copies() {

  BOOL visited[PU_BB_Count+2];
  BZERO(visited, sizeof(BOOL)*(PU_BB_Count+2));

  BOOL has_pcopy[PU_BB_Count+2];
  BZERO(has_pcopy, sizeof(BOOL)*(PU_BB_Count+2));

  //
  // visit nodes in the dominator tree order
  //
  const BB_SET *region_entry_set = SSA_region_entries();
  BB *bb;
  FOR_ALL_BB_SET_members(region_entry_set, bb) {
    SSA_expose_copies_BB (bb, has_pcopy, visited);
  }
}

/* ================================================================
 * ================================================================
 * 		Interface for PSI operations
 * ================================================================
 * ================================================================
 */

// Look at if the two predicates are disjoints.
static BOOL
Disjoint_Predicates(TN *guard1, bool on_false1, TN *guard2, bool on_false2) {
  // Get the definition of the two predicates, and see if one is the
  // opposite of the other.
  OP *def_guard1 = TN_ssa_def(guard1);
  OP *def_guard2 = TN_ssa_def(guard2);

  if (def_guard1 == NULL || def_guard2 == NULL)
    return FALSE;

  // (cbr) if guarded on inverted guard, they are disjoint
  if (guard1 == guard2 && on_false1 != on_false2)
    return TRUE;

  // Currently we recognize only the two following patterns:
  // 				b1 = <exp>;
  // 	b1 = tn1;		tn1 = b1;
  //	b2 = (tn1 == 0);	b2 = (tn1 == 0);
  // TBD: Add support for
  //    b1 = <exp>;
  //    b2 = <!exp>;

  if (OP_Dominates(def_guard2, def_guard1)) {
    // Permute them
    TN *tn_swap = guard1; guard1 = guard2; guard2 = tn_swap;
    OP *op_swap = def_guard1; def_guard1 = def_guard2; def_guard2 = op_swap;
  }

  // Look for b2 = (cmp_opnd == 0);
  if (OP_icmp(def_guard2) && OP_cmp_variant(def_guard2) == V_CMP_EQ) {
    TN *cmp_opnd1 = OP_opnd(def_guard2, OP_find_opnd_use(def_guard2, OU_opnd1));
    TN *cmp_opnd2 = OP_opnd(def_guard2, OP_find_opnd_use(def_guard2, OU_opnd2));
    TN *cmp_opnd = NULL;
    if (TN_is_zero(cmp_opnd1))
      cmp_opnd = cmp_opnd2;
    else if (TN_is_zero(cmp_opnd2))
      cmp_opnd = cmp_opnd1;

    if (cmp_opnd != NULL) {
      // Look for b1 = cmp_opnd;
      if (CGTARG_Is_OP_Inter_RegClass_Bool_Copy(def_guard1) && OP_opnd(def_guard1, 0) == cmp_opnd)
	return TRUE;

      // Look for cmp_opnd = b1;
      OP *def_cmp = TN_ssa_def(cmp_opnd);
      if (def_cmp && CGTARG_Is_OP_Inter_RegClass_Bool_Copy(def_cmp) && OP_opnd(def_cmp, 0) == guard1)
	return TRUE;
    }
  }

  return FALSE;
}

/* Move opnd1 before opnd2 in op. */

static void
PSI_move_opnd(
  OP *psi_op,
  int opnd1,
  int opnd2)
{
  Is_True(opnd2 <= opnd1, ("PSI_move_opnd moves an operand opnd1 at a place opnd2 on its left."));

  if (opnd1 == opnd2) return;

  TN *tn_guard1 = PSI_guard(psi_op, opnd1);
  TN *tn_opnd1 = PSI_opnd(psi_op, opnd1);
  BOOL on_false1 = PSI_Pred_False(psi_op, opnd1);

  for (int opndi = opnd1-1; opndi >= opnd2; opndi--) {
    TN *tn_guardi = PSI_guard(psi_op, opndi);
    TN *tn_opndi = PSI_opnd(psi_op, opndi);
    BOOL on_falsei = PSI_Pred_False(psi_op, opndi);
    Set_PSI_guard(psi_op, opndi+1, tn_guardi);
    Set_PSI_opnd(psi_op, opndi+1, tn_opndi);
    Set_PSI_Pred(psi_op, opndi+1, on_falsei);
  }

  Set_PSI_guard(psi_op, opnd2, tn_guard1);
  Set_PSI_opnd(psi_op, opnd2, tn_opnd1);
  Set_PSI_Pred(psi_op, opnd2, on_false1);
}

/* ================================================================
 *   insert_psi_operand_copy
 * ================================================================
 */
static void
insert_psi_operand_copy (
  OP   *psi_op,
  INT8  opnd_idx,
  OP *point)
{
  TN *tn = PSI_opnd(psi_op, opnd_idx);
  bool on_false =  PSI_Pred_False(psi_op, opnd_idx);

  TN *new_tn = Copy_TN(tn);

  // replace old tn in the psi OP
  Set_PSI_opnd(psi_op, opnd_idx, new_tn);

  // Finally, append the copy op
  OPS cmov_ops = OPS_EMPTY;
  CGTARG_OP_Make_movc(PSI_guard(psi_op, opnd_idx), new_tn, tn, &cmov_ops, on_false);
  Is_True(point && !OP_phi(point), ("Internal Error"));
  BB_Insert_Ops_Before(OP_bb(point), point, &cmov_ops);
  Set_OP_ssa_move(OPS_last(&cmov_ops));

  return;
}

/* ================================================================
 *   append_psi_operand_copy
 * ================================================================
 */
static void
append_psi_operand_copy (
  OP   *psi_op,
  INT8  opnd_idx,
  OP *point)
{
  TN *tn = PSI_opnd(psi_op, opnd_idx);
  bool on_false =  PSI_Pred_False(psi_op, opnd_idx);

  TN *new_tn = Copy_TN(tn);

  // replace old tn in the psi OP
  Set_PSI_opnd(psi_op, opnd_idx, new_tn);

  // Finally, append the copy op
  OPS cmov_ops = OPS_EMPTY;
  CGTARG_OP_Make_movc(PSI_guard(psi_op, opnd_idx), new_tn, tn, &cmov_ops, on_false);
  if (point) {
    // FdF 20050831: Be careful to insert after all PHI operations.
    while (OP_next(point) && OP_phi(OP_next(point)))
      point = OP_next(point);
    // FdF 20061110: Be careful that the guard for opnd_idx may not be
    // defined at the point of definition of opnd_idx.
    BB_Insert_Ops_After(OP_bb(point), point, &cmov_ops);
  }
  else
    BB_Insert_Ops_Before(OP_bb(psi_op), psi_op, &cmov_ops);
  Set_OP_ssa_move(OPS_last(&cmov_ops));

  return;
}

static void
PSI_Live_Info_Init(OP *psi_op) {

  TN *tn_left = PSI_opnd(psi_op, 0);
  for (int opnd_right = 1; opnd_right <= PSI_opnds(psi_op); opnd_right++) {
    TN *tn_right = NULL;
    OP *def_right = psi_op;
    if ( opnd_right < PSI_opnds(psi_op) ) {
      tn_right = PSI_opnd(psi_op, opnd_right);
      def_right = TN_ssa_def(tn_right);
    }
    Is_True(OP_Dominates(TN_ssa_def(tn_left), def_right), ("Inconsistent PSI operation."));
    
    // Then, the last argument is live on the PSI operation
    TN_LIST *op_psi_uses = (TN_LIST *)OP_MAP_Get(map_psi_use, def_right);
    OP_MAP_Set(map_psi_use, def_right, TN_LIST_Push(tn_left, op_psi_uses, &MEM_local_pool));

    tn_left = tn_right;
  }
}

/* ================================================================
 *   Normalize_Psi_Operations
 *
 *   This function reorder or change the arguments of PSI operations
 *   such that the execution order of the definitions of the PSI
 *   arguments matches the order of appearance in the PSI
 *   operation.
 *
 *   It also adds conditional MOV operations for instructions that
 *   cannot be guarded but which are associated with a guard in the
 *   PSI operation.
 *
 *   Finally, it records in map all the TNs used or defined on PSI
 *   operations. For each TN used in a PSI operation, it records its
 *   last point of use.
 *   ================================================================
 */
static void
Normalize_Psi_Operations()
{
  BB *bb;
  OP *op;

  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {

    FOR_ALL_BB_OPs(bb, op) {

      // A predicated definition can be used on a predicated use under
      // the same predicate without going through a PSI. We need to
      // register the definitions so as to set the unc_def property or
      // insert a KILL when needed.
      if (OP_cond_def(op)) {
	for (INT i = 0; i < OP_results(op); i++) {
	  TN *res = OP_result(op, i);
	  if (TN_is_ssa_var(res))
	    SSA_Info_TN_add(res);
	}
      }

      if (!OP_psi(op))
	continue;

      // Remove arguments that are later overriden by other arguments
      OP *new_psi_op;
      new_psi_op = PSI_reduce(op);
      if (new_psi_op != op) {
	// Must remove the current OP before inserting the new one.
	BB_Replace_Op(op, new_psi_op);
	op = new_psi_op;
      }

      /* Check interferences dues to code order. Fix it either by
	 reordering the PSI opnds if guards are disjoints, or by
	 introducing a repair variable otherwise. Also introduce a
	 predicated mov operations when the guard used on the
	 definition and in the guard are not equal. */
    
      for (int opndi = 0; opndi < PSI_opnds(op); opndi++) {
	BOOL reorder_psi_args = TRUE;
	TN *tn_opndi = PSI_opnd(op, opndi);
	TN *tn_guardi = PSI_guard(op, opndi);
        bool on_falsei = PSI_Pred_False(op, opndi);

	OP *def_opndi = TN_ssa_def(tn_opndi);
	FmtAssert(def_opndi, ("Illegal PSI argument %d", opndi));

	// First, repair the guards.

	TN *op_guardi = OP_guard(def_opndi);

	/* The definition cannot be guarded or has been
	   speculated. Introduce a predicated move instruction. */
	if (Trace_SSA_Out) {
	  fprintf(TFile, "PSI Normalize: Introduce predicated move to match predicate in PSI operation\n");
	}

	if (tn_guardi != op_guardi) {
	  // Insert the operation at the lowest point between def_opndi, def(tn_guardi) and def(opndi-1)
	  if (tn_guardi != True_TN) {
	    OP *def_guardi = TN_ssa_def(tn_guardi);
	    FmtAssert(def_guardi, ("Illegal PSI guard %d", opndi));
	    OP *point = !OP_Dominates(def_opndi, def_guardi) ? def_opndi : def_guardi;
	    if (opndi > 0) {
	      OP *def_opndi_m1 = TN_ssa_def(PSI_opnd(op, opndi-1));
	      if (OP_Dominates(point, def_opndi_m1))
		point = def_opndi_m1;
	    }
	    append_psi_operand_copy(op, opndi, point);
	  }
	  else {
	    Set_PSI_guard(op, opndi, op_guardi);
	    Set_PSI_Pred(op, opndi, OP_Pred_False(def_opndi, OP_find_opnd_use(def_opndi, OU_predicate)));
	  }

	  tn_opndi = PSI_opnd(op, opndi);
	  tn_guardi = op_guardi;
	  on_falsei = PSI_Pred_False(op, opndi);
	  def_opndi = TN_ssa_def(tn_opndi);
        }

	int opndj;
	for (opndj = opndi-1; opndj >= 0; opndj --) {
	  TN *tn_opndj = PSI_opnd(op, opndj);
	  OP *def_opndj = TN_ssa_def(tn_opndj);
	  FmtAssert(def_opndj, ("Illegal PSI argument %d", opndj));
	  
	  if (!OP_Dominates(def_opndi, def_opndj)) {
	    // opndj is the last operand that is dominated by opndi
	    opndj++;
	    break;
	  }
	  // FdF 20051010: On stxp70, guardi and guardj may be the
	  // same, but defi and defj can be guarded one on true and
	  // the other on false.
          
	  if (!((tn_guardi == PSI_guard(op, opndj)) &&
		((OP_is_guard_t(def_opndi) && OP_is_guard_f(def_opndj)) ||
		 (OP_is_guard_f(def_opndi) && OP_is_guard_t(def_opndj))))) {
	    if (!Disjoint_Predicates(tn_guardi, on_falsei, PSI_guard(op, opndj), PSI_Pred_False(op, opndj))) {

	      // opndj is the first operand with non disjoint predicate
	      // with opndi
	      reorder_psi_args = FALSE;
	      break;
	    }
	  }
	  
	  // FdF 20051107: Must consider also the case where opndi
	  // dominates the first operand, so as to return 0 instead of
	  // -1.
	  if (opndj == 0)
	    break;
	}

	if (opndi == 0)	{
	  // Nothing to do on the operand order, just repair the gard
	  // on the def if needed.
	}
	else if (opndj == opndi) {
	  // Nothing to do. Arguments are in correct order compared to
	  // the dominance order of their definitions.
	}

	else if (reorder_psi_args) {
	  // Reorder the arguments in the PSI, since predicate domains
	  // of predicates for operands from opndj to opndi-1 are
	  // disjoints with predicate for opndi.
	  if (Trace_SSA_Out)
	    fprintf(TFile, "PSI Normalize: Move operand %d before operand %d.\n", opndi, opndj);

	  PSI_move_opnd(op, opndi, opndj);
	}

	/* Definitions for operands opndj to opndi-1 do not dominate
	   definition for operand opndi, and at least one of the
	   predicates for these operands intersect with the predicate
	   for opndi. So, dominance order must be repaired, to match
	   the order in the PSI operation. This can be done in three
	   different ways:

	   1/ Create a duplicate of defi that is moved below defi-1
	   2/ if (opndj == opndi-1), create a duplicate of defj
	      that is moved above defi.
	   3/ Insert a conditional move of defi below defi-1
	  */

	// [CG]: Memory operations can not be moved without checking
	// aliasing with all crossed operations. For the moment
	// disable move of any memory operation
	// FdF 20070528: Cannot duplicate an operation for repair if
	// the op has multiple or implicit results.
	else if (!OP_volatile(def_opndi) /*[CG]*/&& !OP_memory(def_opndi) &&
		 /*FdF*/ !OP_phi(def_opndi) &&
		 (OP_results(def_opndi) == 1) && !OP_has_implicit_interactions(def_opndi)) {
	  /* 1/ Move duplicate of defi below defi-1. */

	  if (Trace_SSA_Out && opndi != opndj)
	    fprintf(TFile, "PSI Normalize: 1/ Duplicate operation to match order in PSI operation\n");

	  TN *tn_repair = Copy_TN(tn_opndi);
	  OP *op_repair = Dup_OP(def_opndi);
	  OP *op_prev = TN_ssa_def(PSI_opnd(op, opndi-1));

	  Set_OP_result(op_repair, 0, tn_repair);
	  Set_PSI_opnd(op, opndi, tn_repair);
	  BB_Insert_Op_After(OP_bb(op_prev), op_prev, op_repair);
	}

	else {
	  OP *def_opndj = TN_ssa_def(PSI_opnd(op, opndj));
          TN *copy_tn;
	  if ((opndj == (opndi-1)) &&
	      OP_move(def_opndj) &&
	      ((copy_tn = OP_Copy_Operand_TN(def_opndj)) != NULL &&
               (TN_is_constant(copy_tn) ||
                TN_is_zero(copy_tn))) &&
	      (OP_guard(def_opndj) != True_TN)) {
	    /* 2/ Create duplicate of defj before defi. */

	    if (Trace_SSA_Out && opndi != opndj)
	      fprintf(TFile, "PSI Normalize: 2/ Duplicate operation to match order in PSI operation\n");

	    TN *tn_repair = Copy_TN(tn_opndi);
	    OP *op_repair = Dup_OP(def_opndj);

	    Set_OP_result(op_repair, 0, tn_repair);
	    Set_PSI_opnd(op, opndj, tn_repair);
	    BB_Insert_Op_Before(OP_bb(def_opndi), def_opndi, op_repair);
	  }
	  else {
	    /* 3/ The original instruction cannot be duplicated.
	          Introduce a new TN defined by a predicated MOVE
	          instruction. */
	    if (Trace_SSA_Out && opndi != opndj)
	      fprintf(TFile, "PSI Normalize: 3/ Create copy operation to match order in PSI operation\n");

	    TN *tn_repair = Copy_TN(tn_opndi);
	    OP *op_prev = TN_ssa_def(PSI_opnd(op, opndi-1));
	    OPS cmov_ops = OPS_EMPTY;

	    // FdF 20061110: Be careful that the guard tn_guardi may
	    // not be computed at def(opndi-1)
	    OP *op_defguardi = TN_ssa_def(tn_guardi);
	    if (op_defguardi && (OP_Dominates(op_prev, op_defguardi)))
	      op_prev = op_defguardi;

            // (cbr) Support for guards on false
	    CGTARG_OP_Make_movc(tn_guardi, tn_repair, tn_opndi, &cmov_ops, on_falsei);
	    Set_PSI_opnd(op, opndi, tn_repair);
	    BB_Insert_Ops_After(OP_bb(op_prev), op_prev, &cmov_ops);
	  }
	}
      }
      //      Set_OP_cond_def_kind(TN_ssa_def(PSI_opnd(op, 0)), OP_ALWAYS_UNC_DEF);

      // Add TNs for interference checking
      for (int opndi = 0; opndi < PSI_opnds(op); opndi++)
	SSA_Info_TN_add(PSI_opnd(op, opndi));
      SSA_Info_TN_add(OP_result(op, 0));

      // Collect the actual use of each PSI argument.
      PSI_Live_Info_Init(op);
    }
  }
}

/* ================================================================
 * ================================================================
 *                     SSA Coalescing
 * ================================================================
 * ================================================================
 */

static SSA_Info_t *
SSA_Info_Find_Root(SSA_Info_t *ssa_info) {

  if (ssa_info->RootInfo == NULL)
    return ssa_info;

  SSA_Info_t *root_info = ssa_info;
  do {
    root_info = root_info->RootInfo;
  } while (root_info->RootInfo != NULL);

  if (ssa_info->RootInfo != root_info) {
    SSA_Info_t *cur_info = ssa_info;
    do {
      SSA_Info_t *next = cur_info->RootInfo;
      cur_info->RootInfo = root_info;
      cur_info = next;      
    } while (cur_info->RootInfo != NULL);
  }

  return root_info;
}

static TN *
SSA_Info_Find_TN_Root(TN *tn) {
  Is_True(SSA_Info_TN_get(tn) != NULL, ("SSA_Info_Find_TN_Root: Must only be called on SSA TNs"));

  return SSA_Info_Find_Root(SSA_Info_TN_get(tn))->tn;
}

static SSA_Info_t *
SSA_Info_Union_Root(SSA_Info_t *info1, SSA_Info_t *info2) {
  Is_True(info1 != info2, ("SSA_Info_Union_Root must be called with disjoint equivalence classes"));
  Is_True(SSA_Info_idx(info1) < Igraph_Size, ("Cannot coalesce a variable with no entry in IGraph"));
  Is_True(SSA_Info_idx(info2) < Igraph_Size, ("Cannot coalesce a variable with no entry in IGraph"));

  SSA_Info_t *root_info;

  // Consider the case where one of the root is a dedicated register
  if (TN_is_dedicated(info1->tn)) {
    Is_True(!TN_is_dedicated(info2->tn), ("Cannot coalesce two different dedicated registers"));
    info2->RootInfo = root_info = info1;
  }
  else if (TN_is_dedicated(info2->tn)) {
    Is_True(!TN_is_dedicated(info1->tn), ("Cannot coalesce two different dedicated registers"));
    info1->RootInfo = root_info = info2;
  }
  else if (info1->Rank < info2->Rank) {
    info1->RootInfo = root_info = info2;
  }
  else {
    info2->RootInfo = root_info = info1;
    if (info1->Rank == info2->Rank)
      info1->Rank ++;
  }

  return root_info;
}

static void
SSA_Info_Union_TN_Root(TN *root1, TN *root2) {
  Is_True(root1 != root2, ("SSA_Info_Union_TN_Root must be called with disjoint equivalence classes"));

  SSA_Info_t *info1 = SSA_Info_TN_get(root1);
  SSA_Info_t *info2 = SSA_Info_TN_get(root2);

  TN *root = SSA_Info_Union_Root(info1, info2)->tn;
  if (root == root1)
    IGRAPH_TNs_Coalesce(root1, root2);  
  else
    IGRAPH_TNs_Coalesce(root2, root1);  

  // The root TN must be marked global if it represents a global TN
  if (TN_is_global_reg(root1) || TN_is_global_reg(root2))
    Set_TN_is_global_reg(root);
}

static void
SSA_TN_Coalesce(TN *tn1, TN *tn2) {

  TN *root_tn1 = SSA_Info_Find_TN_Root(tn1);
  TN *root_tn2 = SSA_Info_Find_TN_Root(tn2);
  TN *root;

  if (root_tn1 == root_tn2)
    return;

  FmtAssert(!IGRAPH_TNs_Interfere (root_tn1, root_tn2), ("Cannot UNION interfering nodes"));
  SSA_Info_Union_TN_Root(root_tn1, root_tn2);
}

static void
SSA_Mandatory_Coalescing() {

  BB *bb;
  OP *op;

  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    FOR_ALL_BB_OPs(bb, op) {

      // All PHI results and operands must be coalesced into a single
      // variable
      if (OP_phi(op)) {
	TN *res = OP_result(op, 0);
	for (int opnd_idx = 0; opnd_idx < OP_opnds(op); opnd_idx++) {
	  SSA_TN_Coalesce(res, OP_opnd(op, opnd_idx));
	}
      }

      // A result and an operand that must be allocated into the same
      // variable must be coalesced
      if (OP_sameres(op)) {
	INT res_idx, opnd_idx;
	for (res_idx = 0; res_idx < OP_results(op); res_idx++) {
	  if (((opnd_idx = OP_same_res(op, res_idx)) != -1) &&
	      (OP_result(op, res_idx) != OP_opnd(op, opnd_idx)))
	    SSA_TN_Coalesce(OP_result(op, res_idx), OP_opnd(op, opnd_idx));
	}
      }

      // When pinning is required, coalescing must be done with the
      // dedicated register.
      if (OP_Has_ssa_pinning(op)) {
	for (INT opnd_idx = 0; opnd_idx < OP_opnds(op); opnd_idx++) {
	  TN *pinned = OP_Get_opnd_pinning(op, opnd_idx);
	  if (pinned != NULL)
	    SSA_TN_Coalesce(pinned, OP_opnd(op, opnd_idx));
	}
	for (INT res_idx = 0; res_idx < OP_results(op); res_idx++) {
	  TN *pinned = OP_Get_result_pinning(op, res_idx);
	  if (pinned != NULL)
	    SSA_TN_Coalesce(pinned, OP_result(op, res_idx));
	}
      }

      // Also, there is a mandatory coalescing for prologue and
      // epilogue PCOPY
      if (OP_PCOPY(op) && (OP_prologue(op) || OP_epilogue(op))) {
	for (INT opnd_idx = 0; opnd_idx < OP_opnds(op); opnd_idx++) {
	  SSA_TN_Coalesce(OP_result(op, opnd_idx), OP_opnd(op, opnd_idx));
	}
      }

    }
  }
}

/* ================================================================
 *   SSA_Psi_Coalescing
 *
 *   This function look for interferences between variables connected
 *   through PSI operations and repair them.
 *   ================================================================
 */
static void
SSA_Psi_Coalescing ()
{
  BB *bb;
  OP *op;

  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    FOR_ALL_BB_OPs_FWD(bb, op) {

      if (!OP_psi(op))
	continue;

      TN *res_root = SSA_Info_Find_TN_Root(OP_result(op,0));
      for (INT i = PSI_opnds(op)-1; i >= 0; i--) {
	TN *opnd_root = SSA_Info_Find_TN_Root(PSI_opnd(op,i));

	// if (phi_resources_interfere(ccPsi, ccOpnd)) {
	if (IGRAPH_TNs_Interfere (res_root, opnd_root)) {
	  // FdF 20090907: Put the copy just before the definition of
	  // the argument on the right, or just before the PSI
	  // operation in case of the last PSI operand
	  OP *point = (i == (PSI_opnds(op)-1)) ? op : TN_ssa_def(PSI_opnd(op, i+1));
	  insert_psi_operand_copy(op, i, point);

	  // Set the root info so as to force renaming into a common
	  // name, and update the interference graph. Cannot do it the
	  // generic way since we cannot add new entries in the IGRAPH
	  // at this point.
	  TN *new_opnd = PSI_opnd(op,i);
	  SSA_Info_t *ssa_info = SSA_Info_TN_add(new_opnd);
	  ssa_info->RootInfo = SSA_Info_TN_get(res_root);
	  // Pessimistic here
	  IGRAPH_TNs_Coalesce(res_root, opnd_root);
	}
	else
	  SSA_TN_Coalesce(res_root, opnd_root);
      }

    } // for all BB PHI OPs
  } // for all BBs

  return;
}

// Perform coalescing on SSA and non-SSA copies, depending on the
// CG_ssa_coalescing option:
// 0: No coalescing, already checked before calling this function
// 1: SSA-copies can be coalesced, except for auto-mod copies that cross a loop
// 2: All SSA copies can be coalesced
// 3: All SSA and non-SSA copies can be coalesced

static void
SSA_Aggressive_Coalescing_Copy(TN *tn1, TN *tn2) {

  if (TN_register_class(tn1) != TN_register_class(tn2))
    return;

  TN *root_tn1 = SSA_Info_Find_TN_Root(tn1);
  TN *root_tn2 = SSA_Info_Find_TN_Root(tn2);
  TN *root;

  if (root_tn1 == root_tn2)
    return;

  if (IGRAPH_TNs_Interfere (root_tn1, root_tn2))
    return;

  // Do not coalesce a dedicated register with a global TNs, uses of
  // dedicated TNs must remain local after the out-of-SSA phase
  if ((TN_is_dedicated(root_tn1) && TN_is_global_reg(root_tn2)) ||
      (TN_is_dedicated(root_tn2) && TN_is_global_reg(root_tn1)))
    return;

  SSA_Info_Union_TN_Root(root_tn1, root_tn2);
}

static void
SSA_Aggressive_Coalescing_BB(BB *bb) {

  Is_True(CG_ssa_coalescing & SSA_OUT_COALESCING,
	  ("SSA_Aggressive_Coalescing_BB must not be called when no coalescing is required"));

  BOOL after_call = FALSE;

  OP *op;
  FOR_ALL_BB_OPs(bb, op) {

    if (OP_copy(op) &&
	((CG_ssa_coalescing & SSA_OUT_ON_COPY) ||
	 (OP_ssa_move(op) && (CG_ssa_coalescing & SSA_OUT_ON_SSA_MOVE)))) {
      TN *dst = OP_result(op, OP_Copy_Result(op));
      TN *src = OP_opnd(op, OP_Copy_Operand(op));
      if (SSA_Info_TN_get(dst) && SSA_Info_TN_get(src)) {

	// FdF 20080402: Check if coalescing RES and OPND will remove
	// a split-point accross different loops. There are examples
	// where this coalescing contraints too much the register
	// allocator (STxP70 QMX mixer)
	BOOL coalesce_copy = TRUE;
	if (!(CG_ssa_coalescing & SSA_OUT_ON_ALL_SAMERES) && OP_next(op) && OP_sameres(OP_next(op))) {
	  OP *defop;
	  TN *tn_opnd;
	  for (tn_opnd = src, defop = TN_ssa_def(tn_opnd);
	       (defop != NULL) && OP_copy(defop);
	       tn_opnd = OP_Copy_Operand_TN(defop), defop = TN_ssa_def(tn_opnd));
	  coalesce_copy = !defop || (BB_loop_head_bb(OP_bb(defop)) == BB_loop_head_bb(OP_bb(op)));
	}
	if (coalesce_copy)
	  SSA_Aggressive_Coalescing_Copy(dst, src);
      }
    }

    else if (OP_PCOPY(op) &&
	     ((CG_ssa_coalescing & SSA_OUT_ON_COPY) ||
	      (OP_ssa_move(op) && (CG_ssa_coalescing & SSA_OUT_ON_SSA_MOVE)))) {

      if (after_call) {
	// Reset the global TN property on defs, if the PCOPY can be
	// moved in the fall-thru successor and these defs are only
	// locally used in this successor. This will allow more
	// coalescing to be performed. The PCOPY cannot be moved in
	// the fallthru block if the fallthru block may require the
	// definition of a dedicated registers. This is the case if
	// the fallthru block is a call or an exit.
	BB *fallthru = BB_Fall_Thru_Successor(bb);
	if (fallthru && (BB_preds_len(fallthru) == 1) &&
	    !BB_call(fallthru) && !BB_exit(fallthru)) {
	  for (INT i = 0; i < OP_results(op); i++) {
	    TN *dst = OP_result(op, i);
	    if (!GTN_SET_MemberP(BB_live_out(fallthru), dst))
	      Reset_TN_is_global_reg(dst);
	  }
	}
      }

      for (INT i = 0; i < OP_results(op); i++) {
	TN *dst = OP_result(op, i);
	TN *src = OP_opnd(op, i);
	if (SSA_Info_TN_get(dst) && SSA_Info_TN_get(src))
	  SSA_Aggressive_Coalescing_Copy(dst, src);
      }
    }

    if (OP_call(op))
      after_call = TRUE;

  }
}

static BOOL
Compare_BB_freq(const void *p1, const void *p2) {
  const BB *bb1 = *(BB **)p1;
  const BB *bb2 = *(BB **)p2;
  return (BB_freq(bb1) < BB_freq(bb2)) - (BB_freq(bb1) > BB_freq(bb2));
}

static void
SSA_Aggressive_Coalescing() {

  if (!(CG_ssa_coalescing & SSA_OUT_COALESCING))
    return;

  // First, sort BBs into decreasing frequency order

  // Allocate an array of BBs
  BB *BBs_freq_order[PU_BB_Count+2];
  
  BB *bb;
  INT bb_count = 0;
  for (bb = REGION_First_BB; bb; bb = BB_next(bb))
    BBs_freq_order[bb_count++] = bb;

  Is_True(bb_count <= (PU_BB_Count+2), ("Found more BBs than expected"));

  qsort(BBs_freq_order, bb_count, sizeof(BB *), Compare_BB_freq);

  for (INT i=0; i < bb_count; i++)
    SSA_Aggressive_Coalescing_BB(BBs_freq_order[i]);
}

/* ================================================================
 * ================================================================
 *                     Build Congruence classes
 * ================================================================
 * ================================================================
 */

static TN_LIST **congruenceClasses;

// FdF 20070829: Keep track of registers with a
// TN_home. A same TN_home must be used for only one TN. So, associate
// on the first TN, the Home with the TN, and on further references,
// if Home is used with another TN, reset the gra_homeable property.
static WN_MAP tn_home_map = WN_MAP_UNDEFINED;
static BOOL TN_home_Used(TN *tn) {
  TN *tn_use = (TN *)WN_MAP_Get(tn_home_map, TN_home(tn));
  return (tn_use != NULL) && (tn_use != tn);
}
#define Set_TN_home_Used(tn) (WN_MAP_Set(tn_home_map, TN_home(tn), tn))

static void
SSA_CongruenceClasses_init() {

  congruenceClasses = CXX_NEW_ARRAY(TN_LIST *, SSA_Info_COUNT, &outssa_pool);
  memset(congruenceClasses, 0, sizeof(TN_LIST *) * SSA_Info_COUNT);

  for (INT idx = 0; idx < SSA_Info_COUNT; idx ++) {
    SSA_Info_t *ssa_info = SSA_Info_idx_get(idx);
    INT root_idx = SSA_Info_idx(SSA_Info_Find_Root(ssa_info));
    congruenceClasses[root_idx] = TN_LIST_Push(SSA_Info_tn(ssa_info), congruenceClasses[root_idx], &outssa_pool);
  }

  tn_home_map = WN_MAP_Create(&outssa_pool);
}

static void
SSA_CongruenceClasses_fini() {

  if (congruenceClasses != NULL) {
    CXX_DELETE_ARRAY(congruenceClasses, &outssa_pool);
    congruenceClasses = NULL;
  }

  WN_MAP_Delete(tn_home_map);
}

#define SSA_CongruenceClass(tn) congruenceClasses[SSA_Info_idx(SSA_Info_Find_Root(SSA_Info_TN_get(tn)))]

static void
SSA_CongruenceClasses_properties() {

  // Reset the rematerializable or gra_homeable when needed

  for (INT idx = 0; idx < SSA_Info_COUNT; idx ++) {
    TN_LIST *congruenceClass = congruenceClasses[idx];
    // Check if this is a root tn
    if (congruenceClass == NULL)
      continue;

    TN *root_tn = SSA_Info_tn(SSA_Info_idx_get(idx));

    // Discard the property unless all TNs are rematerializable with
    // the same value.
    if (TN_is_rematerializable(root_tn)) {
      TN_LIST *p;
      for (p = congruenceClass; p != NULL; p = TN_LIST_rest(p)) {
	TN *cc_tn = TN_LIST_first(p);
	if (!TN_is_rematerializable(cc_tn) ||
	    TN_remat(cc_tn) != TN_remat(root_tn)) {
	  Reset_TN_is_rematerializable(root_tn);
	  Set_TN_remat(root_tn, NULL);
	  break;
	}
      }
    }

    // Discard the property unless all TNs are homeable with
    // the same value.
    if (TN_is_gra_homeable(root_tn)) {
      TN_LIST *p;
      for (p = congruenceClass; p != NULL; p = TN_LIST_rest(p)) {
	TN *cc_tn = TN_LIST_first(p);
	if (!TN_is_gra_homeable(cc_tn) ||
	    TN_home(cc_tn) != TN_home(root_tn)) {
	  Reset_TN_is_gra_homeable(root_tn);
	  Set_TN_home(root_tn, NULL);
	  break;
	}
      }
      if (TN_is_gra_homeable(root_tn))
	if (!TN_home_Used(root_tn)) {
	  Set_TN_home_Used(root_tn);
	}
	else {
	  Reset_TN_is_gra_homeable(root_tn);
	  Set_TN_home(root_tn, NULL);
	}
    }

    // FdF 20070515: Insert KILL, or set the property UNC_DEF, on
    // conditional definitions that are not dominated by another one
    // in the same congruence class
    if (!TN_is_dedicated(root_tn)) {
      TN_LIST *p;
      for (p = congruenceClass; p != NULL; p = TN_LIST_rest(p)) {
	TN *tn = TN_LIST_first(p);
	OP *op = TN_ssa_def(tn);
      
	if (OP_cond_def(op)) {
	  BOOL dominated = FALSE;
	  TN_LIST *q;
	  for (q = congruenceClass; q != NULL; q = TN_LIST_rest(q)) {
	    OP *tn_def = TN_ssa_def(TN_LIST_first(q));
	    if ((tn_def != op) && OP_Dominates(tn_def, op)) {
	      dominated = TRUE;
	      break;
	    }
	  }
	  // FdF 20090206: For an OP with multiple results the UNC_DEF
	  // property could be set only if all definitions have the
	  // property !dominated. This is not checked, and KILL
	  // definitions are just added for individual definitions when
	  // required.
	  if (!dominated) {
	    if (OP_results(op) == 1)
	      Set_OP_cond_def_kind(op, OP_ALWAYS_UNC_DEF);
	    else {
	      SSA_Disable();
	      OP* kill_op = Mk_VarOP(TOP_KILL, 1, 0, &root_tn, NULL);
	      BB_Insert_Op_Before(OP_bb(op), op, kill_op);
	      SSA_Enable();
	    }
	  }
	}
      }
    }

  }

}

/* ================================================================
 *   PHI_CONGRUENCE_CLASS_Print
 * ================================================================
 */
static void
SSA_CongruenceClass_Print (TN_LIST *cc)
{
  TN_LIST *p;
  fprintf(TFile, "{");
  for (p = cc; p != NULL;p = TN_LIST_rest(p)) {
    Print_TN(TN_LIST_first(p), FALSE);
    fprintf(TFile," ");
  }
  fprintf(TFile, "}");
  return;
}

static void
SSA_CongruenceClasses_Print ()
{
  fprintf(TFile, "==================== Print Congruence Classes ====================\n");
  for (INT idx = 0; idx < SSA_Info_COUNT; idx ++) {
    SSA_Info_t *ssa_info = SSA_Info_idx_get(idx);
    INT root_idx = SSA_Info_idx(SSA_Info_Find_Root(ssa_info));
    fprintf(TFile, "TN%d = ", TN_number(SSA_Info_tn(SSA_Info_idx_get(root_idx))));
    SSA_CongruenceClass_Print(congruenceClasses[root_idx]);
  }
}

/* ================================================================
 * ================================================================
 *                     SSA Undo Renaming
 * ================================================================
 * ================================================================
 */

static void
SSA_Undo_Renaming_BB(BB *bb, BOOL *visited) {

  if (visited[BB_id(bb)]) return;

  // We need to update the LOOPINFO_trip_count_tn
  ANNOTATION *annot = ANNOT_Get(BB_annotations(bb), ANNOT_LOOPINFO);
  if (annot) {
    LOOPINFO *info = ANNOT_loopinfo(annot);
    TN *trip_count_tn = LOOPINFO_primary_trip_count_tn(info);
    if ((trip_count_tn != NULL) && (SSA_Info_TN_get(trip_count_tn) != NULL))
      LOOPINFO_primary_trip_count_tn(info) = SSA_Info_Find_TN_Root(trip_count_tn);
  }

  OP *op, *next_op;
  for (op = BB_first_op(bb); op != NULL; op = next_op) {
    next_op = OP_next(op);

    if (OP_phi(op) || OP_psi(op)) {
      BB_Remove_Op(bb, op);
      continue;
    }

    for (INT i = 0; i < OP_results(op); i++) {
      TN *res = OP_result(op, i);
      SSA_Info_t *ssa_info = SSA_Info_TN_get(res);
      TN *root_tn = ssa_info ? SSA_Info_tn(SSA_Info_Find_Root(ssa_info)) : NULL;

      if (root_tn == NULL) {
	// FdF 20070829: For SSA registers with no congruence class,
	// check that a TN_home is used for only one TN. (#31388)
	if (TN_is_gra_homeable(res)) {
	  if (TN_home_Used(res)) {
	    Reset_TN_is_gra_homeable(res);
	    Set_TN_home(res, NULL);
	  }
	  else
	    Set_TN_home_Used(res);
	}
      }
      else
	Set_OP_result(op, i, root_tn);
    }

    for (INT i = 0; i < OP_opnds(op); i++) {
      TN *opnd = OP_opnd(op, i);
      if (SSA_Info_TN_get(opnd) != NULL)
	Set_OP_opnd(op, i, SSA_Info_Find_TN_Root(opnd));
    }

    if (OP_copy(op)) {
      TN *dst = OP_result(op, OP_Copy_Result(op));
      TN *src = OP_opnd(op, OP_Copy_Operand(op));
      // Do not remove a copy on dedicated TNs, unless it is marked ssa_move
      if ((dst == src) && (!TN_is_dedicated(dst) || OP_ssa_move(op)))
	BB_Remove_Op(bb, op);
    }
    else {
      BOOL done;
      OPS ops = OPS_EMPTY;
      done = CGTARG_OP_Lower_movc(op, &ops);
      if (done) {
        // FdF 20080704: Do not lose the UNC_DEF property that may
        // have been set when building the congruence classes.
        if (OP_cond_def_kind(op) == OP_ALWAYS_UNC_DEF) {
          Set_OP_ssa_move(OPS_last(&ops));
        }

        if (Trace_phi_removal) {
          fprintf(TFile, "  replacing a conditional move \n\n");
          //	  Print_OP_No_SrcLine(op);
        }
        BB_Remove_Op(bb, op);
        OP *prev_next = next_op;
        if (prev_next != NULL)
          BB_Insert_Ops_Before(bb, prev_next, &ops);
        else
          BB_Append_Ops(bb, &ops);
      }
    }
  }

  for (BB_LIST *elist = BB_children(bb); elist; elist = BB_LIST_rest(elist)) {
    BB *kid = BB_LIST_first(elist);
    SSA_Undo_Renaming_BB (kid, visited);
  }

  visited[BB_id(bb)] = TRUE;
}

static void
SSA_Undo_Renaming() {

  // Build congruence classes so as to:
  // - Insert KILL operations or set UNC_DEF properties
  // - Reset the rematerializable property when needed
  // - Reset the gra_homeable property when needed

  SSA_CongruenceClasses_init();

  SSA_CongruenceClasses_properties();

  // Discard all SSA information

  TN_MAP_Delete(tn_ssa_map);
  tn_ssa_map = NULL;         /* so we knew we're out of the SSA */

  OP_MAP_Delete(op_ssa_pinning_map);
  op_ssa_pinning_map = NULL;

  OP_MAP_Delete(phi_op_map);
  phi_op_map = NULL;

  BOOL visited[PU_BB_Count+2];
  BZERO(visited, sizeof(BOOL)*(PU_BB_Count+2));

  //
  // visit nodes in the dominator tree order
  //
  const BB_SET *region_entry_set = SSA_region_entries();
  BB *bb;
  FOR_ALL_BB_SET_members(region_entry_set, bb) {
    SSA_Undo_Renaming_BB (bb, visited);
  }

  SSA_CongruenceClasses_fini();
}

static void
SSA_split_call_bb(BB *bb) {
  // The copies after the call have not all been coalesced. We have to
  // move the copies in the next basic block, or insert a new basic
  // block if the successor has several predecessors.
  BB *bb_next = BB_Fall_Thru_Successor(bb);
  Is_True(bb_next != NULL, ("No successor after a call."));
  if (BB_preds_len(bb_next) != 1) {
    BB *new_bb = Gen_And_Insert_BB_After(bb);
    Change_Succ(bb, bb_next, new_bb);
    Add_Goto(new_bb, bb_next);
    bb_next = new_bb;
  }
  // Now, move all operations after the call in this BB. 
  for (OP *op_copy = BB_last_op(bb); !OP_call(op_copy); op_copy = BB_last_op(bb))
    BB_Move_Op_To_Start(bb_next, bb, op_copy);
}

static void
SSA_Clean_Calls() {

  BB *bb;
  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {

    if (!BB_call(bb) || BB_exit(bb))
      continue;

    if (!OP_call(BB_last_op(bb)))
      SSA_split_call_bb(bb);

    // Remove the extra arguments on a CALL
    OP *op_call = BB_last_op(bb);
    Is_True(OP_call(op_call), ("Unexpected op at the end of a call BB"));
    INT ISA_results = OP_fixed_results(op_call);
    INT ISA_opnds = OP_fixed_opnds(op_call);

    if ((ISA_results < OP_results(op_call)) || (ISA_opnds < OP_opnds(op_call))) {
      OP *new_op_call = Resize_OP(op_call, ISA_results, ISA_opnds);
      BB_Replace_Op(op_call, new_op_call);
    }
  }
}

/* ================================================================
 * ================================================================
 *                     Out Of SSA 
 * ================================================================
 * ================================================================
 */

static void
Split_Entry(BB *entry) {

  Is_True((BB_entry(entry) || BB_handler(entry)), ("Split_Entry called with a non Entry bb"));

  if (!BB_call(entry) && !BB_exit(entry))
    return;

  BB *split_entry = Gen_And_Insert_BB_After(entry);
  BB_freq(split_entry) = BB_freq(entry);
  // If there are incomming edges to a handler bb, then move them to
  // split_entry.
  BBLIST *preds = BB_preds(entry);
  while (preds) {
    BB *pred = BBLIST_item(preds);
    preds = BBLIST_next(preds);
    if (!BB_Retarget_Branch(pred, entry, split_entry)) {
      Add_Goto(pred, split_entry);
      Change_Succ(pred, entry, split_entry);
    }
  }
  // outgoing edges from entry becomes outgoing edges from split_entry
  BBLIST* nxt;
  BBLIST* succ;
  for (succ = BB_succs(entry); succ; succ = nxt) {
    BB* bb_succ = BBLIST_item(succ);
    nxt = BBLIST_next(succ);
    float prob = BBLIST_prob(succ);
    Unlink_Pred_Succ(entry, bb_succ);
    Link_Pred_Succ_with_Prob(split_entry, bb_succ, prob);
    // Also, change the PHI predecessor for all PHIs in bb_succ
    OP *phi;
    FOR_ALL_BB_PHI_OPs(bb_succ, phi) {
      Change_PHI_Predecessor (phi, entry, split_entry);
    }
  }
  Link_Pred_Succ_with_Prob (entry, split_entry, 1.0F);
  // Move all non-prolog operations to split_entry
  OP *op;
  while (((op = BB_last_op(entry)) != NULL) &&
	 (OP_epilogue(op) || OP_call(op) ||
	  !(OP_prologue(op) || OP_no_move_before_gra(op) || (OP_code(op) == TOP_spadjust)))) {
    BB_Move_Op_To_Start(split_entry, entry, op);
  }
  if (BB_exit(entry)) {
    BB_Transfer_Exitinfo(entry, split_entry);
    Exit_BB_Head = BB_LIST_Delete(entry, Exit_BB_Head);
    Exit_BB_Head = BB_LIST_Push(split_entry, Exit_BB_Head, &MEM_pu_pool);
  }
  if (BB_call(entry))
    BB_Transfer_Callinfo (entry, split_entry);
  if (BB_asm(entry))
    BB_Transfer_Asminfo (entry, split_entry);
}

static void
Initialize(RID *rid) {

  static BOOL initialized = FALSE;

  if (!initialized) {
    MEM_POOL_Initialize (&outssa_pool, "out-of-ssa pool", TRUE);
    initialized = TRUE;
  }

  MEM_POOL_Push (&outssa_pool);

  tn_ssa_info_map = NULL;
  Trace_phi_removal = Get_Trace(TP_SSA, SSA_REMOVE_PHI);
  Trace_Igraph = Get_Trace(TP_SSA, SSA_IGRAPH);
  Trace_SSA_Out = Get_Trace(TP_SSA, SSA_MAKE_CONST);

  // Split entry BBs such that they contain only prolog code. This is
  // needed to correctly rename the SSA when dedicated registers are
  // used:
  // - For calls in entry BBs
  // - For secondary entries that have control-flow predecessors

  for (BB_LIST *entries = Entry_BB_Head; entries; entries = BB_LIST_rest(entries))
    Split_Entry(BB_LIST_first(entries));

  SSA_Dominance_init(rid);
}

static void
Finalize() {
  SSA_Dominance_fini();
  Free_Dominators_Memory();
  MEM_POOL_Pop(&outssa_pool);
  MEM_POOL_Pop(&ssa_pool);
}

void SSA_Exit (
  RID *rid, 
  BOOL region 
)
{
  Initialize(rid);

  // Create a map to associate SSA_Info_t * to TNs
  TN_SSA_Info_init();

  // Insert copies around PHI, automod, conflict operands, pinning,
  // ..., such that mandatory coalescing is always possible.
  SSA_expose_copies();

  // Prepass on PSI operations. Repair incorrect order in PSI
  // arguments. Add conditional MOV operations for speculated
  // operations that must be predicated.
  // Initialize virtual use on PSI operations

  map_psi_use = OP_MAP_Create();
  Normalize_Psi_Operations();

  // Propagate values through copies
  SSA_EquiValues();

  // Compute live information, taking into account special rules on
  // PHI and PSI operations.
  GRA_LIVE_Recalc_Liveness(rid);

  // Create the interference graph between SSA variables, ignoring
  // interferences between variables with identical values.
  SSA_Build_IGraph();

  // Apply mandatory coalescing
  SSA_Mandatory_Coalescing();

  // Apply standard algorithm for PSI operations
  SSA_Psi_Coalescing();

  // Perform aggressive coalescing
  SSA_Aggressive_Coalescing();

  // IGraph is not needed anymore
  IGRAPH_Clean();

  // Rename the code
  SSA_Undo_Renaming();

  // Sequentialize parallel copies for PHI operation at beginning and
  // end of basic blocks.
  SSA_Sequentialize_PCopies();

  // Remove the extra arguments on call operations. Also, move copy
  // operations after a call in the next basic block, maybe inserting
  // a new one
  SSA_Clean_Calls();

  // Delete the map for SSA_Info_t
  TN_SSA_Info_fini();

  Finalize();
}

/* ================================================================
 *   SSA_Collect_Info
 *
 *   Collect some statistics related to the SSA:
 *
 *   1. number of SSA moves that remain in the code after
 *      the register allocation;
 *
 * ================================================================
 */
void
SSA_Collect_Info (
  RID *rid, 
  BOOL region,
  INT phase
)
{
  BB *bb;
  OP *op;

  if (!Get_Trace(TP_SSA, SSA_COLLECT_INFO)) return;

  // statistics are dumped to TFile
  fprintf(TFile, "%s SSA Statistics for function \"%s\"\n%s\n", DBar, Cur_PU_Name, DBar);

  if (phase == TP_ALLOC) {
    INT ssa_move_count = 0, copy_count = 0;
    for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
      FOR_ALL_BB_OPs_FWD (bb, op) {
	if (OP_ssa_move(op)) ssa_move_count++;
	if (OP_copy(op)) copy_count++;
      }
    }

    fprintf(TFile, "  SSA moves after LRA: ssa=%d copy=%d\n", ssa_move_count, copy_count);
  }

  fprintf(TFile, "%s\t End SSA Statistics \n%s\n", DBar, DBar);
  
  return;
}
