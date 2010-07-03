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
 * Module: cg_ivs.cxx
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

#include "W_alloca.h"
#include "defs.h"
#include "resource.h"
#include "config.h"
#include "config_TARG.h"
#include "errors.h"
#include "mempool.h"
#include "cg.h"
#include "cgir.h"
#include "tracing.h"
#include "cgtarget.h"
#include "cgprep.h"
#include "op.h"
#include "op_list.h"
#include "op_map.h"
#include "bb.h"
#include "register.h"
#include "bbregs.h"
#include "dominate.h"
#include "cg_loop.h"
#include "cgexp.h"
#include "annotations.h"
#include "gtn_universe.h"
#include "gtn_set.h"
#include "cg_cflow.h"
#include "wn_util.h"
#include "whirl2ops.h"
#include "cio.h"
#include "gra_live.h"
#include "data_layout.h"
#include "cxx_memory.h"
#include "cg_sched_est.h"
#include "cg_ivs.h"
#include "cg_affirm.h"
#include "cg_automod.h"
#include "cg_ssa.h"
#include "stblock.h" // for ST_alignment

BOOL CG_IV_offset_space = FALSE;

static INT
PHI_Loop_Operand (LOOP_IVS *loop_ivs, OP *phi) {
  Is_True(OP_phi(phi), ("PHI_Loop_Operand called on a non PHI operation"));

  // This must be a PHI in a loop head
  BB *bb = OP_bb(phi);
  if ((bb != loop_ivs->Head()) || (OP_opnds(phi) != 2))
    return -1;
  
  return Get_PHI_Predecessor_Idx(phi, loop_ivs->Tail());
}

// ======================================================================
//
//  Implementation of the interface for class LOOP_IVS, defined in cg_ivs.h
//
// ======================================================================

// TBD: Traiter les multi BB en inserant les operations qui
// definissent des GTN et qui ne sont pas sur des dominateurs de la
// queue de boucle sous forme de KILL. Ou plus simplement en mettant
// un defid 0 dans la map tn_last_op pour ces definitions.

// Create use-def links over the instructions in the loop.
#define OPDESC(nres, nopnd, defid) (((INT64)(defid)<<32) | ((nres)<<16) | (nopnd))
#define OPDESC_nres(opdesc) (((opdesc)>>16)&0xffff)
#define OPDESC_nopnd(opdesc) ((opdesc)&0xffff)
#define OPDESC_defid(opdesc) (((opdesc)>>32)&0xffffffff)

void
LOOP_IVS::Init_IVs_Table(LOOP_DESCR *loop, BB **sorted_bbs, UINT32 num_bbs) {

  // The map tn_last_op remembers, for each TN, the index (in
  // ivs_table) of the most recent OP defining that TN.
  // In addition, it also provides the keeo the number of operands and
  // results of the op, so that the IVS entry can be allocated in the
  // second pass on forward references.
  hTN_MAP64 tn_last_op = hTN_MAP64_Create( _loc_mem_pool );

  // First, deallocate the ivs_table if not NULL.
  if (ivs_table)
    del_IVS_Table();

  // Count the number of OPs in the loop body.
  ivs_count = 1;

  for (INT bb_idx = 0; bb_idx < num_bbs; bb_idx ++) {
    BB *bb = sorted_bbs[bb_idx];

    OP *op;
    FOR_ALL_BB_OPs_FWD( bb, op ) {

      // Initialize the map tn_last_op

      // Here, also collect the information on the memory size needed
      // to allocate this DEFID. This way, when there is a foward
      // reference, the size can already be allocated and the UC can
      // be computed during the forward pass. 
      // TBD: Consider only global TNs (Dependence on live-analysis ?)
      BOOL cond_defs = FALSE;
      if (OP_has_predicate(op)) {
	INT p_idx = OP_find_opnd_use(op, OU_predicate);
	if ((!OP_Pred_False(op, p_idx) && (True_TN == NULL || OP_opnd(op, p_idx) != True_TN)) ||
	    ( OP_Pred_False(op, p_idx) && (Zero_TN == NULL || OP_opnd(op, p_idx) != Zero_TN)))
	  cond_defs = TRUE;
      }
      for ( INT res = OP_results( op ) - 1; res >= 0; --res ) {
	if (!TN_is_dedicated(OP_result(op,res))) {
	  INT64 op_desc = OPDESC(OP_results(op), OP_opnds(op), cond_defs ? 0 : DEFID_make(ivs_count, res));
	  hTN_MAP64_Set( tn_last_op, OP_result( op, res ), op_desc );
	}
      }

      ivs_count++;
    }
  }

  // Allocate a table of OP_IV, with one entry for each loop OP.
  // ivs_table[0] is not used, because index 0 is used to indicate
  // constant and global operand TNs not defined in the loop.
  ivs_table = new_IVS_Table(ivs_count);

  // Initialize the entry 0
  ivs_table[0].init_OP_IV(NULL, _loc_mem_pool);
  RES_setNotIV(0);

  // Then, initialize the entries for all operations in the loop
  INT index = 1;
  for (INT bb_idx = 0; bb_idx < num_bbs; bb_idx ++) {
    BB *bb = sorted_bbs[bb_idx];

    OP *op;
    FOR_ALL_BB_OPs_FWD( bb, op ) {

      // Initialize the table entry for this OP
      ivs_table[index].init_OP_IV(op, _loc_mem_pool);
      OP_IV& entry = ivs_table[index];

      // Initialize the ivs_table entry for the operands of this OP op
      // to point to the operands' most recent definitions
      for ( INT opnd = OP_opnds( op ) - 1; opnd >= 0; --opnd ) {
	INT defid = 0;
	INT64 op_desc;
	TN *opnd_op = OP_opnd( op, opnd );
	if ( TN_is_register( opnd_op ) && !TN_is_dedicated( opnd_op )) {
	  op_desc = hTN_MAP64_Get( tn_last_op, opnd_op );
	  defid = OPDESC_defid(op_desc);
	}

	entry.UDlink[opnd] = defid;
	if (defid != 0) {
	  // In case defid refers to an entry not allocated yet,
	  // allocate it first, its initialization will be completed
	  // later.

	  if (DEFID_idx(defid) > index)
	    RES_entry(defid).alloc_OP_IV(OPDESC_nres(op_desc), OPDESC_nopnd(op_desc), _loc_mem_pool);
	  Inc_RES_UC(defid);
	}
      }

      // Update the map tn_last_op
      for ( INT res = OP_results( op ) - 1; res >= 0; --res ) {
	// Update the map tn_last_op
	if (!TN_is_dedicated(OP_result(op,res))) {
	  INT64 op_desc = OPDESC(OP_results(op), OP_opnds(op), DEFID_make(index, res));
	  hTN_MAP64_Set( tn_last_op, OP_result( op, res ), op_desc );
	}
      }
      index ++;
    }
  }
}

void LOOP_IVS::Init( LOOP_DESCR *loop, BB **sorted_bbs, UINT32 num_bbs )
{
  head = loop->loophead;

  tail = prolog = epilog = NULL;

  Is_True(BBlist_Len(BB_preds(head)) == 2, ("LOOP_IVS::Init, inconsistency with Check_Simple_Loop"));

  prolog = BBLIST_item(BB_preds(head));
  if (BB_SET_MemberP(LOOP_DESCR_bbset(loop), prolog)) {
    tail = prolog;
    prolog = BB_Other_Predecessor(head, prolog);
  }
  else
    tail = BB_Other_Predecessor(head, prolog);

  if (BBlist_Len(BB_succs(tail)) == 2)
    epilog = BB_Other_Successor(tail, head);

  Is_True(!BB_SET_MemberP(LOOP_DESCR_bbset(loop), prolog), ("LOOP_IVS::Init: Internal error"));
  Is_True(!epilog || !BB_SET_MemberP(LOOP_DESCR_bbset(loop), epilog), ("LOOP_IVS::Init: Internal error"));

  // Initialize the ivs_table entries and the map tn_last_op.
  Init_IVs_Table(loop, sorted_bbs, num_bbs);
}

// For a TN used in an operation, returns the Induction Variable from
// which it is derived. Returns 0 if not derived from an IV.
DefID_t
LOOP_IVS::OPND_IV_cycle(INT op_idx, INT opnd_idx) {
  Is_True(op_idx > 0, ("Calling OPND_IV_cycle with an invalid op."));

  DefID_t defid = OPND_deflink(op_idx, opnd_idx);
  // This def has not been analyzed yet.
  if (OPND_hasOmega(op_idx, opnd_idx))
    return Find_IV(defid, defid);
  else
    return Find_IV(defid, 0);
}

// For a TN used in an operation, returns the offset from the value of
// the IV at loop entry to this use.
INT
LOOP_IVS::OPND_IV_offset(INT op_idx, INT opnd_idx) {
  Is_True(op_idx > 0, ("Calling OPND_IV_offset with an invalid op."));
  if (OPND_hasOmega(op_idx, opnd_idx))
    return 0;
  else
    return IV_offset(OPND_deflink(op_idx, opnd_idx));
}

DefID_t
LOOP_IVS::RES_IV_cycle(DefID_t defid) {
  return Find_IV(defid, 0);
}

// For an induction variable, returns its step
INT
LOOP_IVS::IV_step(DefID_t iv_cycle) {
  Is_True(IV_cycle(iv_cycle) == iv_cycle, ("Calling IV_step on a non IV-representative op"));
  return IV_offset(iv_cycle);
}

// This function takes an (op, res_idx), and returns an (opnd_idx,
// value, (OPR_ADD|OPR_MPY)). This gives a semantics for an
// operation. This function is used when following the use-def links
// from an IV cycle. This function must support ADD/SUB/MPY/COPY/PHI
// TBD: Add support for MPY

static OPERATOR
Analyze_OP_for_IV(LOOP_IVS *loop_ivs, OP *op, INT res_idx, INT &iv_idx, INT64 &iv_inc)
{
  OPERATOR iv_opr = OPERATOR_UNKNOWN;

  // No support yet for operations with multiple results
  if (res_idx != 0) {
    // Support for auto-increment operations
    if (OP_automod(op)) {
      INT pre_idx = OP_find_result_with_usage(op, OU_preincr);
      INT post_idx = OP_find_result_with_usage(op, OU_postincr);
      if (((pre_idx == res_idx) || (post_idx == res_idx)) && (OP_Offset(op) != NULL)) {
	if (TN_Value_At_Op(OP_Offset(op), op, &iv_inc)) {
	  iv_opr = OPR_ADD;
	  iv_idx = OP_find_opnd_use (op, OU_base);
	}
      }
    }
  }

  else if (OP_Is_Copy(op)) {
    iv_opr = OPR_ADD;
    iv_inc = 0;
    iv_idx = OP_Copy_Operand(op);
  }

  else if (OP_phi(op)) {
    iv_idx = PHI_Loop_Operand(loop_ivs, op);
    if (iv_idx >= 0) {
      iv_opr = OPR_ADD;
      iv_inc = 0;
    }
  }

  else if (OP_iadd(op) || OP_isub(op)) {
    TN *opnd_incr = OP_Opnd2(op);
    iv_idx = OP_find_opnd_use(op, OU_opnd1);

    if (TN_Value_At_Op(opnd_incr, op, &iv_inc)) {
      if (OP_isub(op)) iv_inc = -iv_inc;
      iv_opr = OPR_ADD;
    }

    // Try cst+iv
    else if (OP_iadd(op)) {
      opnd_incr = OP_Opnd1(op);
      iv_idx = OP_find_opnd_use(op, OU_opnd2);
      if (TN_Value_At_Op(opnd_incr, op, &iv_inc))
	iv_opr = OPR_ADD;
    }
  }
  return iv_opr;
}

// Return the idx of the first op in the loop that belongs to the
// iv_cycle. This will return a PHI operation in loophead when in SSA.
void
LOOP_IVS::IV_first(DefID_t iv_cycle, INT &op_idx, INT &opnd_idx) {
  Is_True(iv_cycle > 0, ("Calling IV_first with an invalid IV."));

  DefID_t iv_idx = iv_cycle;
  INT64 incr;

  op_idx = DEFID_idx(iv_idx);
  OPERATOR opr = Analyze_OP_for_IV(this, Op(op_idx), DEFID_res(iv_idx), opnd_idx, incr);
  Is_True(opr != OPERATOR_UNKNOWN, ("IV_first not called on an IV cycle"));

  // Follow the use-def links until the defining OP is that the last
  // one in the loop.
  while (OPND_deflink(op_idx, opnd_idx) != iv_cycle) {
    iv_idx = OPND_deflink(op_idx, opnd_idx);
    op_idx = DEFID_idx(iv_idx);
    opr = Analyze_OP_for_IV(this, Op(op_idx), DEFID_res(iv_idx), opnd_idx, incr);
    Is_True(opr != OPERATOR_UNKNOWN, ("IV_first not called on an IV cycle"));
  }
}

// For an induction variable, returns its defining operation, if
// found, outside of the loop.
OP *
LOOP_IVS::IV_init(DefID_t iv_cycle) {
  INT op_idx, opnd_idx;
  IV_first(iv_cycle, op_idx, opnd_idx);

  if (!SSA_Active()) {
    DEF_KIND kind;
    return TN_Reaching_Value_At_Op(OP_opnd(Op(op_idx), opnd_idx), Op(op_idx), &kind, TRUE);
  }
  // In SSA mode, just return the defining OP
  else
    return TN_ssa_def(OP_opnd(Op(op_idx), opnd_idx));
}

void
LOOP_IVS::Trace_IVs_Entries( const char *message )
{
  Is_True( TFile, ( "LOOP_IVS::Trace_IVs_Entries -- TFile is NULL\n" ) );
  fprintf( TFile, "<ivs> LOOP_IVS::Trace_IVs_Entries(count %d) -- %s",
	   ivs_count, message );

  // Assumes table[0] is not used
  for ( INT index = 1; index < ivs_count; ++index ) {
    OP_IV *entry = &ivs_table[index];
    fprintf( TFile, "\n%2d: ", index );
    Print_OP_No_SrcLine( entry->op );

    fprintf( TFile, "    op 0x%p\n", entry->op);
    for (int i = 0; i < OP_results(entry->op); i++) {
      fprintf( TFile, "\tres %d, uc[%d]: ", i, entry->UC[i]);
      if (entry->notIV(i))
	fprintf(TFile, "not IV\n");
      else if (!entry->isIV(i))
	fprintf(TFile, "IV not analyzed\n");
      else {
	if (entry->IV_cycle[i] == -DEFID_make(index, i)) {
	  fprintf(TFile, "is IV, IV_step %d\n", entry->IV_offset[i] );
	}
	else
	  fprintf(TFile, "IV_cycle (%3d,%1d), IV_offset %d\n",
		  DEFID_idx(entry->IV_cycle[i]), DEFID_res(entry->IV_cycle[i]), entry->IV_offset[i] );
      }
    }

    for ( INT opnd = 0; opnd < OP_opnds( entry->op ); ++opnd ) {
      INT defid = entry->UDlink[opnd];
      fprintf( TFile, "\topnd %d: src (%3d,%1d), om %u;",
	       opnd, DEFID_idx(defid), DEFID_res(defid), OPND_hasOmega(index, opnd) );
      if (RES_isIV(defid)) {
	INT iv = OPND_IV_cycle(index, opnd);
	fprintf( TFile, " --> IV (%d,%d)+%d", DEFID_idx(iv), DEFID_res(iv), OPND_IV_offset(index, opnd));
      }
      fprintf( TFile, "\n");
    }
  }
  fprintf( TFile, "\n" );
}

/* Follow the use-def chains from def_index, until a cycle is
   found. If the cycle only contains ADD and SUB operations with an
   immediate value, return the index of the operation in the IVs table
   that is last executed in the loop. Return 0 otherwise. */

DefID_t
LOOP_IVS::Find_IV( DefID_t defid, DefID_t in_iv_cycle ) {
  // This definition has already been visited, and has been found
  // to NOT be based on an IV.
  if (RES_notIV(defid))
    return 0;

  // This definition has already been visited, and has been found to
  // be based on an IV.
  if (RES_isIV(defid)) {
    // Do not return this IV if we are in a cycle that spans more than
    // one iteration, for example in:

    // X1 = PHI X0, X2
    // Y1 = PHI Y0, Y2
    // X2 = Y1
    // Y2 = Y1 + step

    // When looking for an IV for X1, although Y2 is an IV, this does
    // not make an IV for X1 (In case X0 equals (Y0-step) that would
    // make an IV, but this is not checked for the moment)
    if ((in_iv_cycle == 0) || (in_iv_cycle == IV_cycle(defid)))
      return IV_cycle(defid);
    else
      return 0;
  }

  // This definition has never been analyzed.
  INT op_idx = DEFID_idx(defid);
  INT opnd_idx = -1;
  OP *op = Op(op_idx);

  OPERATOR iv_opr = OPERATOR_UNKNOWN;
  INT64 incr = 0;

  // TBD: Add support for dedicated registers and MPY
  if (!TN_is_dedicated(OP_result(op, DEFID_res(defid))))
    iv_opr = Analyze_OP_for_IV(this, op, DEFID_res(defid), opnd_idx, incr);

  DefID_t iv_index = 0;
  INT iv_offset = 0;

  if (iv_opr == OPR_ADD) {
    DefID_t usedef_id = OPND_deflink(op_idx, opnd_idx);

    if (OPND_hasOmega(op_idx, opnd_idx)) {

      if (in_iv_cycle == 0) {
	// We are starting an IV cycle. Continue following use-def links
	// until we take again a loop back-edge.
	iv_index = Find_IV(usedef_id, usedef_id);
	Is_True((iv_index == 0) || (iv_index == usedef_id), ("Inconsistency in Find_IV"));
      }

      else if (in_iv_cycle == usedef_id) { /* (in_iv_cycle > 0) */
	// We are just completing an IV cycle. usedef_id is the last
	// operation in a loop iteration for that IV cycle. This IV
	// cycle is referenced with index usedef_id.
	iv_index = in_iv_cycle;
      }
	
      /* otherwise (in_iv_cycle > 0) && (in_iv_cycle != usedef_id) */
      // This is an IV cycle that spans several iterations. This can
      // be produced with the following code:
      //   while (i < N) {
      //     k = i + 1;
      //     i = j + 1;
      //     j = k + 1;
      //   }
    }

    else { /* !OPND_hasOmega */
      // Continue following the use-def links.
      iv_index = Find_IV(usedef_id, in_iv_cycle);
      iv_offset = IV_offset(usedef_id);
    }
  }

  // Recursive call to Find_IV may have set the infomation on the
  // current operation, do not set it again.
  Is_True((!RES_isIV(defid) && !RES_notIV(defid)) || // Uninitialized
	  (RES_isIV(defid) && (IV_cycle(defid) == iv_index) && // It is an IV
	   (IV_offset(defid) == iv_offset+incr)) ||
	  (RES_notIV(defid) && (iv_index == 0)), // It is not an IV
	  ("Inconsistency in Find_IV"));

  if (!RES_isIV(defid) && !RES_notIV(defid))
    if (iv_index != 0)
      if (in_iv_cycle != 0)
	RES_setIV(defid, -iv_index, iv_offset+incr);
      else
	RES_setIV(defid, iv_index, iv_offset+incr);
    else
      RES_setNotIV(defid);

  return IV_cycle(defid);
}

void
LOOP_IVS::Replace_Op(INT index, OP *new_op) {

  OP_IV& entry = ivs_table[index];
  Is_True(OP_opnds(entry.op) == OP_opnds(new_op), ("Inconsistent new_op in Replace_OP"));
  Is_True(OP_results(entry.op) == OP_results(new_op), ("Inconsistent new_op in Replace_OP"));

  entry.op = new_op;
}

// ======================================================================
//
//  End of the implementation of the interface for class LOOP_IVS
//
// ======================================================================

// ======================================================================
//
//  Implementation example for optimizations based on loop induction
//  variables
//
// ======================================================================

static BOOL
Check_Simple_Loop(LOOP_DESCR *loop, BB **sorted_bbs, UINT32 num_bbs) {

  //  if (BB_SET_Size(LOOP_DESCR_bbset(loop)) != 1)
  //    return FALSE;

  BB *head = LOOP_DESCR_loophead( loop );
  if (BBlist_Len(BB_preds(head)) != 2)
    return FALSE;

  // FdF 20100322: In SSA, simply check this is an inner loop on which
  // a topological order can be made
  if (SSA_Active()) {
    if (!BB_innermost(head))
      return FALSE;
    BB_SET *bbs = LOOP_DESCR_bbset(loop);
    BB_MAP topo_map = BB_Topological_Map(bbs, head);

    BB *bb;
    FOR_ALL_BB_SET_members(bbs, bb) {
      INT32 i = BB_MAP32_Get(topo_map, bb);
      Is_True(i >= 0 && i <= num_bbs, ("bad <topo_map> value"));
      if (i == 0) {
	BB_MAP_Delete(topo_map);
	return FALSE;
      }
      sorted_bbs[i-1] = bb;
    }
    return TRUE;
  }
  
  // Check that each block in the loop has only one successor in the
  // loop.
  INT i = 0;
  BB *next = NULL;
  for (BB *bb = head; next != head; bb = next) {
    sorted_bbs[i++] = bb;
    BBLIST *succs;
    next = NULL;
    FOR_ALL_BB_SUCCS(bb, succs) {
      if (BB_SET_MemberP(LOOP_DESCR_bbset(loop), BBLIST_item(succs))) {
	if (next != NULL)
	  return FALSE;
	next = BBLIST_item(succs);
      }
    }

    // There is no back-edge, probably a fully unrolled loop
    if (next == NULL)
      return FALSE;
  }

  return TRUE;
}

// Collects the references to an IV
typedef struct {
  DefID_t iv_cycle;
  INT nb_refs;
  BOOL has_large_offset;
  BOOL can_use_automod;
  // Array of references to that IV (defined in cgtarget.h)
  IV_ref_t *iv_memrefs;
} IV_refs_t;

static BOOL
Compare_IV_Offsets(const void *p1, const void *p2) {
  const IV_ref_t *ref1 = (IV_ref_t *)p1;
  const IV_ref_t *ref2 = (IV_ref_t *)p2;
  return (ref1->iv_offset > ref2->iv_offset) ? 1 : (ref1->iv_offset == ref2->iv_offset) ? 0 : -1;
}

static TN *
New_IV_For_Refs(LOOP_IVS *loop_ivs, DefID_t iv_cycle, INT iv_dist, TN *iv_tn) {

  // Create a new induction variable, and replace nb_refs references
  // in iv_refs to use the new IV

  TN *init_iv_tn, *init_new_iv, *ref_new_iv, *inc_new_iv;
  if (!SSA_Active()) {
    init_iv_tn = iv_tn;
    init_new_iv = Dup_TN(iv_tn);
    ref_new_iv = init_new_iv;
    inc_new_iv = init_new_iv;
  }
  else {
    init_new_iv = Dup_TN(iv_tn);
    ref_new_iv = Dup_TN(iv_tn);
    inc_new_iv = Dup_TN(iv_tn);
    // Look for the PHI operation in head that is related to
    // that IV. Get the init TN.
    INT op_idx, opnd_idx;
    loop_ivs->IV_first(iv_cycle, op_idx, opnd_idx);

    Is_True(OP_phi(loop_ivs->Op(op_idx)), ("Did not find a PHI op for this IV"));
    Is_True(PHI_Loop_Operand(loop_ivs, loop_ivs->Op(op_idx)) == opnd_idx, ("Unexpected operand index in PHI"));
    init_iv_tn = OP_opnd(loop_ivs->Op(op_idx), 1-opnd_idx);
  }

  // Init: init_new_iv = iv+iv_dist-MIN_OFFSET;
  OPS prolog_ops = OPS_EMPTY;
  Exp_OP2(OPC_I4ADD, init_new_iv, init_iv_tn, Gen_Literal_TN(iv_dist, 4), &prolog_ops);
  if (!SSA_Active()) {
    // TBD: Insert just after the definition of IV
    OP *point = BB_last_op(loop_ivs->Prolog());
    BOOL before = (point != NULL) && OP_xfer(point);
    BB_Insert_Ops(loop_ivs->Prolog(), point, &prolog_ops, before);
  }
  else {
    OP *point = TN_ssa_def(init_iv_tn);
    // Insert after all PHI operations.
    while (OP_next(point) && OP_phi(OP_next(point)))
      point = OP_next(point);
    BB_Insert_Ops(OP_bb(point), point, &prolog_ops, FALSE);

    OP *phi_op = SSA_Place_Phi_In_BB(ref_new_iv, loop_ivs->Head());
    INT loop_opnd = PHI_Loop_Operand(loop_ivs, phi_op);
    Set_OP_opnd(phi_op, 1-loop_opnd, init_new_iv);
    Set_OP_opnd(phi_op, loop_opnd, inc_new_iv);
  }

  // Inc: new_iv += Step(iv)
  OPS body_ops = OPS_EMPTY;
  Exp_OP2(OPC_I4ADD, inc_new_iv, ref_new_iv,
	  Gen_Literal_TN(loop_ivs->IV_step(iv_cycle), 4),
	  &body_ops);
  OP *point = BB_last_op(loop_ivs->Tail());
  BOOL before = (point != NULL) && OP_xfer(point);
  BB_Insert_Ops(loop_ivs->Tail(), point, &body_ops, before);

  return ref_new_iv;
}

static void
Replace_IV_For_Refs(LOOP_IVS *loop_ivs, DefID_t iv_cycle, INT iv_dist,
		    IV_ref_t *ivrefs, INT nb_refs, TN *ref_new_iv) {

  // Replace OP(iv+offset) -> OP(new_iv-(iv_dist-MIN_OFFSET));
  for (int k = 0; k < nb_refs; k++) {
    OP *use_op = ivrefs[k].op;
    INT use_offset = ivrefs[k].iv_offset - iv_dist;
    // Use the literal offset to store the new offset value, if range
    // permits.
    Is_True(OP_find_opnd_use(use_op, OU_base) == ivrefs[k].opnd_idx, ("New_IV_For_Refs: Unexpected reference"));
    Is_True(OP_Offset(use_op), ("New_IV_For_Refs: Unexpected reference"));
    TN *tn_offset = OP_Offset(use_op);;
    TN *new_offset;
    // Use the offset TN to store the offset value, if possible
    if (TN_has_value(tn_offset) &&
	(TOP_opnd_value_in_range(OP_code(use_op), OP_find_opnd_use(use_op, OU_offset), use_offset))) {
      new_offset = Gen_Literal_TN(use_offset, 4);
    }
    else {
      new_offset = Dup_TN(ref_new_iv);
      OPS offset_ops = OPS_EMPTY;
      Exp_Immediate(new_offset, Gen_Literal_TN(use_offset, 4), TRUE, &offset_ops);
      BB_Insert_Ops(use_op->bb, use_op, &offset_ops, TRUE);
    }

    Set_OP_opnd(use_op, OP_find_opnd_use(use_op, OU_offset), new_offset);
    Set_OP_opnd(use_op, ivrefs[k].opnd_idx, ref_new_iv);

    // TBD: Decrement reference count on old IV
    // loop_ivs->Dec_RES_UC(iv_cycle);
    // Cancel the use-def links on new operands
    loop_ivs->Set_OPND_deflink(ivrefs[k].op_idx, ivrefs[k].opnd_idx, 0);
  }

  // TBD: Remove these references from the def-use chain on old IV
}

// Analyze the memory operations for which the base is an IV. Detects
// the cases where a large offset is used, or where an automod cannot
// be used

static INT MIN_OFFSET = 0;           /* In case of signed 9 bits -> -256    */
static INT MAX_OFFSET = 0;           /* In case of signed 9 bits -> +255    */
static INT OFFSET_ALIGNMENT = 1; /* 1 -> char (byte), 2 -> short (2 bytes), 4 -> word (4 bytes), ... */
static INT MAX_WIDTH = -1;

static INT
Round_toZero(INT v, INT mask) {
  Is_True((mask&(mask-1))==0, ("Round_toZero must be called with a 2^n mask.\n"));
  if (v >= 0)
    return v&~(mask-1);
  else
    return -((-v)&~(mask-1));
}

static void
Analyze_IV_offsets(LOOP_IVS *loop_ivs, IV_refs_t *iv_refs, MEM_POOL *mem_pool) {

  Is_True(MAX_WIDTH != -1, ("MAX_WIDTH: Uninitialized value"));

  // Check if this loop needs optimizations
  
  // For each IV
  //   - Check if there is at least on memory OP on which AUTOMOD could be performed.
  //   - Check if there are some memory OP which does not use directly the IV

  // Initialize the array
  INT idx;
  OP *op;
  FOR_ALL_LOOP_IVS_OPs_FWD( loop_ivs, idx, op ) {

    if (!OP_memory(op))
      continue;

    int base_idx = OP_find_opnd_use(op, OU_base);
    DefID_t iv_cycle = loop_ivs->OPND_IV_cycle(idx, base_idx);
    if (iv_cycle == 0)
      continue;

    // Cannot optimize accesses with non literal offsets
    TN *tn_offset = OP_Offset(op);
    INT64 offset_val;
    if ((tn_offset == NULL) || !TN_Value_At_Op(tn_offset, op, &offset_val))
      continue;

    // Consider the base+offset expression, as if memop were based on
    // IV with offset equal to IV_offset
    INT offset = loop_ivs->OPND_IV_offset(idx, base_idx);
    offset += offset_val;

    IV_refs_t *ivref = &iv_refs[DEFID_idx(iv_cycle)];
    if (ivref->nb_refs == 0) {
      // The number of memory references to that IV is at most the
      // number of memory operations in the loop, since there is at
      // most one reference per memory op. We take as an upper bound
      // the number of operations in the loop.
      ivref->iv_memrefs = CXX_NEW_ARRAY( IV_ref_t, loop_ivs->Size(), mem_pool );
      ivref->iv_cycle = iv_cycle;
    }

    // One of the following conditions must be satisfied to generate
    // an automod, assuming the IV incrementation can always be put
    // after the memory operations.
    // offset == 0 && TOP_AM_automod_variant(OP_code(op), TRUE, (step > 0)) != TOP_UNDEFINED
    // offset == step && TOP_AM_automod_variant(OP_code(op), FALSE, (step > 0)) != TOP_UNDEFINED
    // In addition, for an automod, we must have MIN_OFFSET <= step <= MAX_OFFSET

    // TBD: use check_IncrOffset from cg_automod.cxx ???
    INT IV_step = loop_ivs->IV_step(iv_cycle);
    if (CG_AutoMod && (IV_step >= MIN_OFFSET) && (IV_step <= MAX_OFFSET)) {
      // Make sure we check for automod with a variant of the OP_code
      // that uses an immediate value for the offset argument
      TOP top = TOP_opnd_immediate_variant(OP_code(op), OP_find_opnd_use(op, OU_offset), IV_step);
      if ((top != TOP_UNDEFINED) &&
	  (((offset == 0) && // post-incrementation may be possible
	    ((offset_val == 0) || (offset_val == -IV_step)) &&
	    (TOP_AM_automod_variant(top, TRUE, (IV_step > 0), ISA_REGISTER_CLASS_UNDEFINED) != TOP_UNDEFINED)) ||
	   ((offset == IV_step) && // pre-incrementation may be possible
	    ((offset_val == 0) || (offset_val == IV_step)) &&
	    (TOP_AM_automod_variant(top, FALSE, (IV_step > 0), ISA_REGISTER_CLASS_UNDEFINED) != TOP_UNDEFINED))))
	ivref->can_use_automod = TRUE;
    }

    // Mark if some access based on that IV cannot have their offset inlined into the operation
    ivref->has_large_offset |= ((offset < MIN_OFFSET) || (offset > MAX_OFFSET));

    Is_True(ivref->nb_refs < loop_ivs->Size(), ("More references than allocated"));
    ivref->iv_memrefs[ivref->nb_refs].op = op;
    ivref->iv_memrefs[ivref->nb_refs].op_idx = idx;
    ivref->iv_memrefs[ivref->nb_refs].opnd_idx = base_idx;
    ivref->iv_memrefs[ivref->nb_refs].iv_offset = offset;
    ivref->nb_refs ++;
  }
}

static BOOL
Analyze_Memop_Address(LOOP_IVS *loop_ivs, DefID_t op_idx) {

  Is_True(MAX_WIDTH != -1, ("MAX_WIDTH: Uninitialized value"));

  // Look for the expression of memory addresses. If the address
  // expression uses several loop invariant variables, try to rewrite
  // the expression using one loop invariant variable

  OP *op = loop_ivs->Op(op_idx);

  if (!OP_memory(op) || OP_automod(op))
    return FALSE;

  INT base_idx = OP_find_opnd_use(op, OU_base);
  INT offset_idx = OP_find_opnd_use(op, OU_offset);

  if ((base_idx < 0) || (offset_idx < 0))
    return FALSE;

  // When a memop is based on an IV, it will be optimized in a specific way
  if (loop_ivs->OPND_IV_cycle(op_idx, base_idx) > 0)
    return FALSE;

  // Two cases can be optimized:
  // temp = exp + inv2; memop @inv1(temp) -> inv = inv1+inv2; memop @inv(temp)
  // temp = exp + inv2; memop @temp(inv1) -> inv = inv1+inv2; memop @inv(temp)

  // Look for a loop invariant
  INT temp_idx, inv1_idx;
  INT64 inv1_val;
  if ((loop_ivs->OPND_deflink(op_idx, base_idx) == 0) ||
      TN_Value_At_Op(OP_Base(op), op, &inv1_val)) {
    inv1_idx = base_idx;
    temp_idx = offset_idx;
  }
  else if ((loop_ivs->OPND_deflink(op_idx, offset_idx) == 0) ||
	   TN_Value_At_Op(OP_Offset(op), op, &inv1_val)) {
    inv1_idx = offset_idx;
    temp_idx = base_idx;
  }
  else
    return FALSE;

  // Can only optimize INT expression
  if (TN_size(OP_opnd(op, temp_idx)) > 4)
    return FALSE;

  DefID_t defid_temp = loop_ivs->OPND_deflink(op_idx, temp_idx);

  // If the temp is a loop invariant, nothing to optimize
  if (defid_temp == 0)
    return FALSE;

  // For OPT_Size, we do not want to create different expressions for
  // identical or almost identical values.
  if (CG_IV_offset_space && (loop_ivs->RES_UC(defid_temp) > 1))
    return FALSE;

  INT temp_op_idx = DEFID_idx(defid_temp);
  OP *temp_op = loop_ivs->Op(temp_op_idx);

  INT exp_idx, inv2_idx;
  INT64 inv2_val;
  if (OP_iadd(temp_op) || OP_isub(temp_op)) {
    INT idx1 = OP_find_opnd_use(temp_op, OU_opnd1);
    INT idx2 = OP_find_opnd_use(temp_op, OU_opnd2);
    if ((loop_ivs->OPND_deflink(temp_op_idx, idx1) == 0) ||
	TN_Value_At_Op(OP_Opnd1(temp_op), op, &inv2_val)) {
      if (OP_isub(temp_op))
	return FALSE;
      inv2_idx = idx1;
      exp_idx = idx2;
    }
    else if ((loop_ivs->OPND_deflink(temp_op_idx, idx2) == 0) ||
	TN_Value_At_Op(OP_Opnd2(temp_op), op, &inv2_val)) {
      exp_idx = idx1;
      inv2_idx = idx2;
    }
    else
      return FALSE;
  }
  else
    return FALSE;

  // This load is at a loop invariant address, this will be optimized
  // later if profitable
  if ((loop_ivs->OPND_deflink(temp_op_idx, exp_idx) == 0) ||
      TN_Value_At_Op(OP_opnd(temp_op, exp_idx), op, &inv2_val))
    return FALSE;

  // No invariant to combine if one invariant value is 0
  if ((TN_Value_At_Op(OP_opnd(op, inv1_idx), op, &inv1_val) && (inv1_val == 0)) ||
      (TN_Value_At_Op(OP_opnd(temp_op, inv2_idx), temp_op, &inv2_val) && (inv2_val == 0)))
    return FALSE;

  // Create an op "inv = inv1 + inv2" outside of the loop

  TN *tn_inv1 = OP_opnd(op, inv1_idx);
  TN *tn_inv2 = OP_opnd(temp_op, inv2_idx);

  TN *tn_exp = OP_opnd(temp_op, exp_idx);
  TN *tn_inv = Dup_TN(tn_exp);

  Reset_TN_is_rematerializable(tn_inv);
  Set_TN_remat(tn_inv, NULL);
  Reset_TN_is_gra_homeable(tn_inv);
  Set_TN_home(tn_inv, NULL);

  TOP new_top = TOP_opnd_register_variant(OP_code(op), OP_find_opnd_use(op, OU_offset), TN_register_class(tn_exp));
  if (new_top == TOP_UNDEFINED)
    return FALSE;

  if (TN_Value_At_Op(tn_inv1, op, &inv1_val))
    tn_inv1 = Gen_Literal_TN(inv1_val, 4);
  if (TN_Value_At_Op(tn_inv2, temp_op, &inv2_val))
    tn_inv2 = Gen_Literal_TN(inv2_val, 4);

  OPS prolog_ops = OPS_EMPTY;
  if (OP_iadd(temp_op)) {
    if (TN_has_value(tn_inv1) && TN_has_value(tn_inv2))
      Exp_Immediate(tn_inv, Gen_Literal_TN(TN_value(tn_inv1) + TN_value(tn_inv2), 4), TRUE, &prolog_ops);
    else
      Expand_Add(tn_inv, tn_inv1, tn_inv2, MTYPE_I4, &prolog_ops);
  }
  else {
    if (TN_has_value(tn_inv1) && TN_has_value(tn_inv2))
      Exp_Immediate(tn_inv, Gen_Literal_TN(TN_value(tn_inv1) - TN_value(tn_inv2), 4), TRUE, &prolog_ops);
    else
      Expand_Sub(tn_inv, tn_inv1, tn_inv2, MTYPE_I4, &prolog_ops);
  }

  OP *point = BB_last_op(loop_ivs->Prolog());
  BOOL before = (point != NULL) && OP_xfer(point);
  BB_Insert_Ops(loop_ivs->Prolog(), point, &prolog_ops, before);

  // Update the memop

  Set_OP_opr(op, new_top);
  Set_OP_opnd(op, base_idx, OP_opnd(temp_op, exp_idx));
  Set_OP_opnd(op, offset_idx, tn_inv);

  // Update the loop_ivs info.
  loop_ivs->Set_OPND_deflink(op_idx, base_idx, defid_temp);
  loop_ivs->Set_OPND_deflink(op_idx, offset_idx, 0); // loop invariant
  loop_ivs->Dec_RES_UC(defid_temp);
  if (loop_ivs->OPND_deflink(temp_op_idx, exp_idx) > 0)
    loop_ivs->Inc_RES_UC(loop_ivs->OPND_deflink(temp_op_idx, exp_idx));

  return TRUE;
}

// Find a set of memory references, starting at j_first, that meet the
// following conditions:

// * The range of the offsets is within MAX_WIDTH

// * The set contains at least one operation that can be assigned a
// * zero offset

static INT
IV_refs_Within_MaxWidth(IV_refs_t *ivref, INT j_first, INT *first_zero, INT *last_zero) {

  INT j_last = j_first;
  *first_zero = *last_zero = j_first;

  Is_True(MAX_WIDTH != -1, ("MAX_WIDTH: Uninitialized value"));

  for (INT j = j_first+1; j < ivref->nb_refs; j_last = j, j++) {

    // Do not include this operation if this would make the range of
    // offsets larger the MAX_WIDTH
    if ((ivref->iv_memrefs[j].iv_offset - ivref->iv_memrefs[j_first].iv_offset) > MAX_WIDTH)
      break;

    // Do not include this operation if no operation in the set can be
    // adjusted to have a zero offset
    INT min_adjust_to_zero = Round_toZero(MIN_OFFSET-ivref->iv_memrefs[j_first].iv_offset, OFFSET_ALIGNMENT);
    INT max_adjust_to_zero = Round_toZero(MAX_OFFSET-ivref->iv_memrefs[j].iv_offset, OFFSET_ALIGNMENT);
    INT new_first_zero = *first_zero;
    INT new_last_zero = (*last_zero == j_last) ? j : *last_zero;
    for (INT k = new_first_zero; k <= new_last_zero; k++) {
      if ((ivref->iv_memrefs[k].iv_offset + max_adjust_to_zero) < 0) {
	Is_True(k == new_first_zero, ("Incorrect ordering of IV reference offsets"));
	new_first_zero ++;
      }
      if ((ivref->iv_memrefs[k].iv_offset + min_adjust_to_zero) > 0) {
	new_last_zero = k-1;
	break;
      }
    }
    if (new_first_zero > new_last_zero) {
      // Do not inlude this operation in the current set since
      // otherwise the set would contain no element with a 0 offset.
      break;
    }
    *first_zero = new_first_zero;
    *last_zero = new_last_zero;
  }

  return j_last;
}

// ivref is a set of memory operations that are based on a same
// induction variable. These memory references are optimized such
// that:

// * New induction variables are created if the offset on the memory
// * reference is too large to be inlined into the operation.

// * A new induction variable is created no memory operation has an
// * offset suitable for later automod optimization.

// Return TRUE if the loop was modified.

static BOOL
Optimize_IV_refs(LOOP_IVS *loop_ivs, IV_refs_t *ivref) {

  BOOL changed = FALSE;

  Is_True(MAX_WIDTH != -1, ("MAX_WIDTH: Uninitialized value"));

  // Sort the array references according to the IV offset
  qsort(ivref->iv_memrefs, ivref->nb_refs, sizeof(IV_ref_t), Compare_IV_Offsets);

  // Now, go through the IV references. If some references have large
  // offsets, introduce new IVs to remove one, or several, adds in the
  // loop. For small offsets, if no operation is candidate for
  // automod, also replace the current IV by a new IV.

  INT j_first, j_last;
  for (j_first = 0; j_first < ivref->nb_refs; j_first = j_last+1) {
    INT first_zero, last_zero;

    j_last = IV_refs_Within_MaxWidth(ivref, j_first, &first_zero, &last_zero);
  
    // Now elements in [j_first,j_last] are the maximum set of memops
    // that span within MAX_WIDTH, with at least one element that can
    // be adjusted to have a zero offset. Find the best element whose
    // offset can be adjusted to 0.

    INT min_adjust_to_zero = Round_toZero(MIN_OFFSET-ivref->iv_memrefs[j_first].iv_offset, OFFSET_ALIGNMENT);
    INT max_adjust_to_zero = Round_toZero(MAX_OFFSET-ivref->iv_memrefs[j_last].iv_offset, OFFSET_ALIGNMENT);

    INT best_idx = -1;
    INT best_count = 0;
    INT count;
    for (int k = first_zero; k <= last_zero; k += count) {
      count = 1;
      Is_True(((ivref->iv_memrefs[k].iv_offset + min_adjust_to_zero) <= 0) &&
	      ((ivref->iv_memrefs[k].iv_offset + max_adjust_to_zero) >= 0),
	      ("These elements must be candidate for a zero offset adjustement"));
      // Count how many references have the same offset
      while (((k+count) <= j_last) &&
	     (ivref->iv_memrefs[k].iv_offset == ivref->iv_memrefs[k+count].iv_offset))
	count ++;
      if ((best_idx == -1) || (count > best_count)) {
	best_idx = k;
	best_count = count;
      }
    }

    Is_True(best_count >= 0, ("A set must include a candidate that can be adjusted toward a 0 offset"));

    INT new_iv_dist = ivref->iv_memrefs[best_idx].iv_offset;
    // Evaluate the gain of introducing a new IV and replacing the
    // references
    INT gain = CGTARG_Check_Optimize_IV(&ivref->iv_memrefs[j_first], j_last-j_first+1,
					loop_ivs->IV_step(ivref->iv_cycle), new_iv_dist);
    if (gain >= 0) {
      TN *iv_tn = OP_result(loop_ivs->Op(DEFID_idx(ivref->iv_cycle)), DEFID_res(ivref->iv_cycle));
      TN *new_iv_tn = New_IV_For_Refs(loop_ivs, ivref->iv_cycle, new_iv_dist, iv_tn);
      Replace_IV_For_Refs(loop_ivs, ivref->iv_cycle, new_iv_dist,
			  ivref->iv_memrefs+j_first, j_last-j_first+1, new_iv_tn);
      changed = TRUE;
    }
    //    else {
    //      TN *iv_tn = OP_result(loop_ivs->Op(DEFID_idx(ivref->iv_cycle)), DEFID_res(ivref->iv_cycle));
    //      Replace_IV_For_Refs(loop_ivs, ivref->iv_cycle, new_iv_dist, ivref->iv_memrefs+j_first, j_last-j_first+1, iv_tn);
    //    }
  }

  return changed;
}

BOOL 
Optimize_Loop_Induction_Offsets( LOOP_DESCR *loop ) {

  if (!CG_LOOP_opt_iv_offset)
    return FALSE;

  UINT32 num_bbs = BB_SET_Size(LOOP_DESCR_bbset(loop));
  BB *sorted_bbs[num_bbs];
  if (!Check_Simple_Loop(loop, sorted_bbs, num_bbs))
    return FALSE;

  // In -Os, only optimize single BB loops, dead loop induction
  // variables may not be detected on multiple-BB loops.
  // Under SSA, a global dead code will be performed.
  if (CG_IV_offset_space && (BB_SET_Size(LOOP_DESCR_bbset(loop)) > 1) && !SSA_Active())
    return FALSE;

  // Initialize memory pool for LOOP_IVS
  MEM_POOL local_mem_pool;
  MEM_POOL_Initialize( &local_mem_pool, "LOOP_IVS local pool", FALSE );
  MEM_POOL_Push( &local_mem_pool );

  LOOP_IVS loop_ivs( &local_mem_pool );
  loop_ivs.Init( loop, sorted_bbs, num_bbs );

  // Allocate an array to hold for each operation an array of
  // operations that reference an IV.
  IV_refs_t IV_OP_references[loop_ivs.Size()];
  memset(IV_OP_references, 0, loop_ivs.Size() * sizeof(IV_refs_t));

  // Initialize range values
  CGTARG_Get_Info_For_Common_Base_Opt(&OFFSET_ALIGNMENT, &MIN_OFFSET, &MAX_OFFSET);
  // Due to scaling factor, not all values are allowed:
  MIN_OFFSET = Round_toZero(MIN_OFFSET, OFFSET_ALIGNMENT);
  MAX_OFFSET = Round_toZero(MAX_OFFSET, OFFSET_ALIGNMENT);
  MAX_WIDTH = MAX_OFFSET - MIN_OFFSET;

  Analyze_IV_offsets(&loop_ivs, IV_OP_references, &local_mem_pool);

  BOOL changed = FALSE;

  // loop_ivs.Trace_IVs_Entries("Optimize_Loop_Induction_Offsets");

  // Then, look for IV entries, and check if creating new induction
  // variables will provide some gains

  for (INT op_idx = 1; op_idx <= loop_ivs.Last_opidx(); op_idx ++) {

    IV_refs_t *ivref = &IV_OP_references[op_idx];
    if ((ivref->nb_refs > 0) &&
	(ivref->has_large_offset || (CG_AutoMod && !ivref->can_use_automod))) {
      // May create new induction variables to avoid the use of large
      // offsets or to allow the use of auto-modifying addressing mod.
      if (Optimize_IV_refs(&loop_ivs, ivref))
	changed = TRUE;
    }

    // When the computation of an address uses several loop invariant
    // variables, try to change the expression to use only one loop
    // invariant variable
    else if (OP_memory(loop_ivs.Op(op_idx)) && (ivref->nb_refs == 0)) {
      if (Analyze_Memop_Address(&loop_ivs, op_idx))
	changed = TRUE;
    }
  }

  // Dispose memory pool for LOOP_IVS
  MEM_POOL_Pop( &local_mem_pool );
  MEM_POOL_Delete( &local_mem_pool );

  return changed;
}

#if !defined(TARG_STxP70) && !defined(TARG_ARM)
// ======================================================================
//
//  An implementation of load-store packing based on the loop
//  induction variable module.
//
//  Perform_Load_Store_Packing looks for ldw and stw instructions that
//  access consecutive words in memory and pack them respectively into
//  ldp and stp instructions.  It will generate code to handle dynamic
//  alignment, or will produce more efficient code when the alignment
//  is known at compile time.
//
// ======================================================================

/*
  MemoryOP_t

  - index is the index of this memory op in the Pack32_entry table.
  - memsize is the size of the memory operation
  - offset is the offset from this memory op to the value of the
     induction variable it is based on at the beginning of an
     iteration.
*/

typedef struct {
  UINT  index;
  INT   offset;
  UINT8 memsize;
} MemoryOP_t;

/*
  MemoryStream_t

  - memop_count gives the number of load or store in the stream

  - memory_ops is a pointer into the MemoryOP_t table to the arrays of
    operations in the stream

  - IV_tn is the tn of the induction variable on which this stream is
    based on, that is live-in of the loop.

  - size is the size of the stream, equal to the absolute value of the
    step of the induction variable this stream is based on.

  - allmemsize is a bitfield that gives all the memory size in the
    stream.

  - align_offset is such that the value of the IV at loop entry plus
    this align_offset is a memory address that must be correctly
    aligned for the first packed operation of the stream. This value
    is used to check for dynamic loop peeling or specialization if the
    value of an IV is correct to perform packing.

  - increasing tells if the step for this stream is increasing or
    decreasing.

  - align_base and align_bias give the alignment of the first memory
    operation, or the first memory operation + memsize if !increasing.

  - align_kind gives the property of the stream regarding loop
    transformations that may be required for packing. TBD: There may
    be different loop transformation depending on whether packing is
    to 32b or to 64b.

  - packing_base indicates if the loop transformation enables 32b of
    64b packing.

  - unroll is the minimum unrolling factor that must be applied to
    this stream so that is can be packed. If a loop is unrolled more
    times, it must be unrolled a multiple of times of this value.
*/

// For a stream, mark its alignment property
enum {
  STREAM_NONE              = 0,   // The stream cannot be packed
  STREAM_ALIGNED           = 0x1, // The stream is well aligned
  STREAM_UNC_PEEL_ALIGNED  = 0x2, // The stream is well aligned after unconditional loop peeling
  STREAM_NEXT_ALIGNED      = 0x4, // The second load in the stream is well aligned
  STREAM_UNC_PEEL_NEXT_ALIGNED = 0x8, // The second load in the stream is well aligned after unconditional loop peeling
  STREAM_COND_PEEL_ALIGNED = 0x10, // The stream will be well aligned after conditional loop peeling
  STREAM_SPECIAL_ALIGNED   = 0x20  // The stream may be well aligned in specialized loop
};

// What transformation can be applied on a loop to enable stream
// packing.
enum {
  LOOP_TRANSFORM_NONE = 0,       // No loop transformation
  LOOP_TRANSFORM_UNC_PEEL,   // Unconditional loop peeling
  LOOP_TRANSFORM_COND_PEEL,  // Conditional loop peeling
  LOOP_TRANSFORM_SPECIALIZE, // Loop specialization
  LOOP_TRANSFORM_LAST
};

#define SET_STREAM_TRANSFORM(stream, tr) ((stream)->transform_kind |= (1<<tr))
#define RESET_STREAM_TRANSFORM(stream, tr) ((stream)->transform_kind &= ~(1<<tr))
#define STREAM_TRANSFORM(stream, tr) ((stream)->transform_kind & (1<<tr))

enum {
  MEMOP_LOAD = 0,
  MEMOP_STORE,
  MEMOP_LAST
};

typedef struct MemoryStream MemoryStream_t;
struct MemoryStream {
  INT             memop_count;
  MemoryOP_t     *memory_ops;
  MemoryStream_t *next;
  TN             *IV_tn;
  INT             size;
  INT             align_offset;
  BOOL		  increasing;
  UINT8           allmemsize;
  UINT8		  align_base;
  UINT8		  align_bias;
  UINT8		  align_kind;
  UINT8		  transform_kind;
  UINT8		  memop_kind;
  UINT8		  packing_base;
  UINT8		  unroll;
};

#define IS_POWER_OF_2(n) (((n)&((n)-1)) == 0)

// Packing has been performed on a stream. Some operations have been
// canceled, other have been replaced by a packed operation. Remove
// canceled operations, count the length of the stream, and recompute
// the allmemsize field.
static void
Update_Stream(MemoryStream_t *cur_stream) {

  // Update memop_count, and remove items with index == 0
  cur_stream->allmemsize = 0;
  INT i, j;
  for (i=0, j=0; i < cur_stream->memop_count; i++) {

    // This operation has been canceled
    if (cur_stream->memory_ops[i].index == 0)
      continue;

    // Make a continuous stream of packed operations
    if (i > j)
      cur_stream->memory_ops[j] = cur_stream->memory_ops[i];

    cur_stream->allmemsize |= cur_stream->memory_ops[j].memsize;
    j++;
  }

  cur_stream->memop_count = j;
}

static BOOL
LoadStore_Packing_Candidate_Op( OP *op )
{
  if ( OP_has_implicit_interactions( op ) ||
      (OP_opnds( op ) > OP_MAX_FIXED_OPNDS) ||
       (OP_results( op ) > OP_MAX_FIXED_RESULTS) )
    return FALSE;

  if ( !OP_load( op ) && !OP_store( op ) )
    return FALSE;

  if (OP_load(op) && ((Loop_packing_flags & PACKING_LOAD) == 0))
    return FALSE;

  if (OP_store(op) && ((Loop_packing_flags & PACKING_STORE) == 0))
    return FALSE;

  if ( OP_unalign_store( op ) ||
       OP_unalign_ld( op )    ||

       // Skip predicated operations
       OP_has_predicate( op ) )
    return FALSE;

  if (!IS_POWER_OF_2(OP_Mem_Ref_Bytes( op )))
    return FALSE;

  return TRUE;
}

static BOOL
Packing_Load_Candidate_Arc( ARC *arc )
{
  return ( ARC_kind( arc ) != CG_DEP_MEMIN &&
	   //	   ARC_kind( arc ) != CG_DEP_MEMOUT &&
	   (ARC_kind( arc ) != CG_DEP_MEMANTI || ARC_is_definite(arc)) &&
	   ARC_kind( arc ) != CG_DEP_MEMVOL);
}

static BOOL
Packing_Store_Candidate_Arc( ARC *arc )
{
  return ( ARC_kind( arc ) != CG_DEP_MEMIN );
}

// Initialize the base symbol, the associated relocation, and the
// offset of this TN.

static void
Analyse_Offset_TN(TN *offset_tn, ST **st, INT *relocs, INT64 *val) {

  if (TN_is_symbol(offset_tn)
      && Base_Offset_Is_Known (TN_var(offset_tn))) {
    ST *symbol = TN_var(offset_tn);
    Base_Symbol_And_Offset (symbol, st, val);
    *relocs = TN_relocs(offset_tn);
    *val += TN_offset(offset_tn);
    Is_True(0, ("Analyse_Offset_TN with TN_is_symbol: To be checked."));
  }
  else {
    *st = NULL;
    *relocs = 0;
    *val = TN_value(offset_tn);
  }
}

static TN *
Adjust_Offset_TN(TN *offset_tn, INT64 adjust) {

  TN *new_offset_tn = NULL;

  if (TN_is_symbol(offset_tn)
      && Base_Offset_Is_Known (TN_var(offset_tn))) {
    ST *st = TN_var(offset_tn), *symbol;
    INT64 offset_val;
    Base_Symbol_And_Offset (st, &symbol, &offset_val);
    INT32 relocs = TN_relocs(offset_tn);
    offset_val += TN_offset(offset_tn);
    offset_val += adjust;
    new_offset_tn = Gen_Symbol_TN(symbol, offset_val, relocs);
    Is_True(0, ("Adjust_Offset_TN: To Be Checked."));
  }
  else {
    INT64 offset_val;
    offset_val = TN_value(offset_tn);
    new_offset_tn = Gen_Literal_TN(offset_val + adjust, 4);
  }

  return new_offset_tn;
}

static BOOL
Get_OP_IV_info( LOOP_IVS *loop_ivs, INT opidx, OP *op,
		INT *IV_index, INT *IV_offset,
		ST **symbol, INT32 *relocs, INT64 *offset_val)
{
  int base_idx = OP_find_opnd_use(op, OU_base);
  *IV_index = loop_ivs->OPND_IV_cycle(opidx, base_idx);
  Is_True(*IV_index != 0, ("Inconsistency in Induction Variable"));

  int offset_idx = OP_find_opnd_use(op, OU_offset);
  TN *offset_tn = OP_opnd(op, offset_idx);
  *IV_offset = loop_ivs->OPND_IV_offset(opidx, base_idx);
  Analyse_Offset_TN(offset_tn, symbol, relocs, offset_val);

  return TRUE;
}

static INT
Get_Pragma_Alignment(BB *loophead) {

  INT pragma_alignment = -1;

  ANNOTATION *align_ant = ANNOT_Get(BB_annotations(loophead), ANNOT_PRAGMA);
  while (align_ant && WN_pragma(ANNOT_pragma(align_ant)) != WN_PRAGMA_STREAM_ALIGNMENT)
    align_ant = ANNOT_Get(ANNOT_next(align_ant), ANNOT_PRAGMA);
  if (align_ant) {
    WN *wn = ANNOT_pragma(align_ant);
    pragma_alignment = WN_pragma_arg1(wn);
  }
  return pragma_alignment;
}

static void
Get_Memop_Alignment(BB *prolog, OP *memop, INT64 offset, INT *base, INT *bias) {

  *base = 1;
  *bias = 0;

  // FdF 20050919: Try to collect the alignment for this memory
  // operation of the loop.

  BB *loophead = BB_loop_head_bb(OP_bb(memop));

  OP *use_base = memop;
  OP *def_base;

  TN *base_tn = OP_Base(use_base);
  INT64 base_offset = 0;
  ST *base_sym = NULL;
  DEF_KIND kind;
  BOOL in_inner_loop = TRUE;

  // Look for definitions of the base_tn used on the load/store
  // operation. The following cases can occur:
  //   def_base == NULL : No reaching definition found
  //   def_base = TN + cst : Look for definition of TN
  //   def_base = $r12 + cst : base = $r12, offset += cst, break;
  //   def_base = $r12 + $sym : base = $r12, sym = $sym, break;
  //   def_base = $sym : break; base = NULL, sym = $sym, break;
  //   def_base = <other op> : def_base = NULL, break;

  // base_offset accumulates the offset from the start of the loop to
  // the variable on which the alignment is known. Use loophead to
  // ignore offsets from operations in the current loop.

  while ((def_base = TN_Reaching_Value_At_Op(base_tn, use_base, &kind, TRUE)) != NULL) {

    TN *opnd_tn = NULL;
    INT64 val;
    BB *cur_loophead = BB_loop_head_bb(OP_bb(def_base));

    if (in_inner_loop && (cur_loophead != loophead))
      // This definition is the first one out of the initial loop
      in_inner_loop = FALSE;

    if (!in_inner_loop) {
      // Check we are in the immediately enclosing loop.
      if (cur_loophead != BB_loop_head_bb(prolog)) {
	def_base = NULL;
	break;
      }
    }

    if (OP_Is_Affirm(def_base)) {
      break;
    }
    else if (OP_Is_Copy(def_base)) {
      base_tn = OP_opnd(def_base, OP_Copy_Operand(def_base));
      opnd_tn = Zero_TN;
    }
    else if (OP_iadd(def_base) || OP_isub(def_base)) {
      base_tn = OP_Opnd1(def_base);
      opnd_tn = OP_Opnd2(def_base);
    }
    else {
      def_base = NULL;
      break;
    }

    if (base_tn == SP_TN) {
      if (TN_is_symbol(opnd_tn)) {
	base_sym = TN_var(opnd_tn);
	base_offset += TN_offset(opnd_tn);
      }
      else if (TN_Value_At_Op(opnd_tn, def_base, &val)) {
	if (OP_isub(def_base)) val = -val;
	base_offset += val;
      }
      else
	def_base = NULL;
      break;
    }

    else if (TN_is_symbol(base_tn)) {
      if (TN_Value_At_Op(opnd_tn, def_base, &val)) {
	if (OP_isub(def_base)) val = -val;
	base_offset += val;
	base_sym = TN_var(base_tn);
	base_offset += TN_offset(base_tn);
	base_tn = NULL;
      }
      else
	def_base = NULL;
      break;
    }

    // Here, def_base is either defined in this loop, or out of any
    // loop.

    if (TN_Value_At_Op(opnd_tn, def_base, &val)) {
      if (OP_isub(def_base)) val = -val;
      // Ignore offset from operations inside the loop, this offset
      // has already been taken into account
      if (!in_inner_loop)
	base_offset += val;
      use_base = def_base;
      continue;
    }

    def_base = NULL;
    break;
  }

  // Get exact alignment

  INT base_alignment = 1;
  INT bias_alignment = 0;
  if (def_base != NULL) {
    Is_True(BB_loop_head_bb(OP_bb(def_base)) != loophead, ("Get_Memop_Alignment: Incorrect base for stream"));
    if (OP_Is_Affirm(def_base)) {
      Get_Affirm_modulo(OP_Get_Affirm(def_base), NULL, &base_alignment, &bias_alignment);
      if (Get_Trace(TP_AFFIRM, 0x1)) {
	fPrint_TN(TFile, "Used AFFIRM property (", OP_result(def_base, 0));
	fprintf(TFile, "%%%d==%d) for packing.\n", base_alignment, bias_alignment);
      }
    }
    else if (base_sym == NULL) {
      Is_True(base_tn == SP_TN, ("Get_Memop_Alignment: only SP_TN+Symbol is recognized"));
      base_alignment = Stack_Alignment();
      bias_alignment = 0;
    }
    else {
      base_alignment = ST_alignment(base_sym);
      bias_alignment = 0;
    }
  }

  if (base_alignment > 1) {
    Is_True(IS_POWER_OF_2(base_alignment), ("base_alignment must be a power of 2"));
    *base = base_alignment;
    *bias = (base_offset+offset+bias_alignment)%base_alignment;
  }
}

static BOOL
Compare_Offsets(const void *p1, const void *p2) {
  const MemoryOP_t *cand1 = (MemoryOP_t *)p1;
  const MemoryOP_t *cand2 = (MemoryOP_t *)p2;
  return (cand1->offset < cand2->offset) ? -1 : (cand1->offset == cand2->offset) ? 0 : 1;
}

static BOOL
Compare_Offsets_neg(const void *p1, const void *p2) {
  const MemoryOP_t *cand1 = (MemoryOP_t *)p1;
  const MemoryOP_t *cand2 = (MemoryOP_t *)p2;
  return (cand1->offset > cand2->offset) ? -1 : (cand1->offset == cand2->offset) ? 0 : 1;
}

static BOOL
Compare_Special_Streams(const void *p1, const void *p2) {
  MemoryStream_t *stream1 = *(MemoryStream_t **)p1;
  MemoryStream_t *stream2 = *(MemoryStream_t **)p2;

  if (stream1->memop_kind < stream2->memop_kind)
    return -1;
  else if (stream1->memop_kind > stream2->memop_kind)
    return 1;
  else if (stream1->memop_count < stream2->memop_count)
    return -1;
  else
    if (stream1->memop_count > stream2->memop_count)
      return 1;
  return 0;
}

/* Extract t1|t2 from tP, with tp being a 64bit or 32bit value. */

static void
Extract_Packed2(TN *t1, TN *t2, TN *tP, TYPE_ID load_type, OPS *ops_packed) {

  if (Target_Byte_Sex == BIG_ENDIAN) {
    TN *tmp = t1;
    t1 = t2;
    t2 = tmp;
  }

  switch (load_type) {
  case MTYPE_I4:
  case MTYPE_U4:
    Expand_Extract(t1, t2, tP, ops_packed);
    break;

  case MTYPE_I2:
  case MTYPE_U2: {
    TYPE_ID ldw_type = MTYPE_signed(load_type) ? MTYPE_I4 : MTYPE_U4;
    Exp_Extract_Bits(ldw_type, load_type, 0, 16, t1, tP, ops_packed);
    Exp_Extract_Bits(ldw_type, load_type, 16, 16, t2, tP, ops_packed);
    break;
  }

  default:
    Is_True(0, ("Only Word And Half Word packing is supported."));
  }
}

static BOOL
Combine_Adjacent_Loads(LOOP_IVS *loop_ivs, MemoryStream_t *load_stream, int packed_size)
{

  Is_True(IS_POWER_OF_2(packed_size), ("Packed size must be 2^n"));

  if ((load_stream->align_base < packed_size) ||
      ((load_stream->align_bias%(packed_size/2)) != 0))
    return FALSE;

  Is_True(load_stream->allmemsize == (packed_size/2), ("Packing not implemented for 4h->1L"));

  // Current limitations
  if ((load_stream->allmemsize < 2) || (load_stream->allmemsize >= 8))
    return FALSE;

  // Only pairs of loads can be packed.
  if ((load_stream->memop_count&1) != 0)
    return FALSE;

  if (Get_Trace(TP_CGLOOP, 0x10))
      fprintf(TFile, "<ivs packing> Combine adjacent %d Loads\n", load_stream->memop_count);

  BB *body = OP_bb(loop_ivs->Op(load_stream->memory_ops[0].index));
  int cand1_idx = 0;
  TN *t0 = NULL;

  // Execute the first load operation in the loop prolog if the stream
  // is misaligned. This operation is changed
  //
  // op[t1 = load @base+offset]
  //   by
  // op[t0 = load @IV_addr_tn+(IV_offset+offset)]

  if ((load_stream->align_bias%packed_size) != 0) {
    Is_True(CG_LOOP_prolog != NULL, ("Incorrect loop: Missing loop prolog"));

    // Check if alignment for next operation is OK, in which case the
    // first operation in the stream will be moved in the loop prolog.

    INT next_bias = load_stream->align_bias + (load_stream->increasing
					       ? load_stream->memory_ops[0].memsize
					       : - load_stream->memory_ops[0].memsize);
    if ((next_bias%packed_size) != 0)
      return FALSE;

    int load_index = load_stream->memory_ops[0].index;
    OP *load_op = loop_ivs->Op(load_index);

    OP *load0_op = Dup_OP(load_op);
    t0 = Dup_TN(OP_result(load0_op, 0));

    /* We must change the base TN to be IV_addr_tn, since the TN used
       on the load operation may not be available in the prolog. We
       will not change the offset of the operation but an add will be
       added to compute a new base. */

    OPS prolog_ops = OPS_EMPTY;

    INT IV_offset = loop_ivs->OPND_IV_offset(load_index, OP_find_opnd_use(load_op, OU_base));
    TN *new_base_tn;
    if (IV_offset == 0)
      new_base_tn = load_stream->IV_tn;
    else {
      new_base_tn = Dup_TN(load_stream->IV_tn);
      Exp_OP2(OPC_I4ADD, new_base_tn, load_stream->IV_tn, Gen_Literal_TN(IV_offset, 4), &prolog_ops);
    }      
    int base_idx = OP_find_opnd_use(load0_op, OU_base);
    Set_OP_opnd(load0_op, base_idx, new_base_tn);

    /* This load operation defines a new tn t0. */

    Set_OP_result(load0_op, 0, t0);
    Copy_WN_For_Memory_OP (load0_op, load_op);
    OP_srcpos(load0_op) = OP_srcpos(load_op);

    OPS_Append_Op(&prolog_ops, load0_op);

    OP *point = BB_last_op(CG_LOOP_prolog);
    BOOL before = (point != NULL) && OP_xfer(point);
    BB_Insert_Ops(CG_LOOP_prolog, point, &prolog_ops, before);

    // Now continue packing with the next operation in the stream.
    cand1_idx ++;
    // Since the stream now starts with the next operation in the
    // stream, this changes the align_bias information.
    load_stream->align_bias = next_bias;
  }

  // Get the type of the packed load, from sign of the original load
  // and size of the packed operation.
  TYPE_ID load_type;
  if (OP_unsigned_ld(loop_ivs->Op(load_stream->memory_ops[0].index)))
    load_type = MTYPE_TransferSize(packed_size/2, MTYPE_U4);
  else
    load_type = MTYPE_TransferSize(packed_size/2, MTYPE_I4);

  for (; cand1_idx < load_stream->memop_count; cand1_idx += 2) {
    INT cand2_idx = cand1_idx+1;
    BOOL wrapped = FALSE;
    if (cand2_idx >= load_stream->memop_count) {
      wrapped = TRUE;
      cand2_idx = 0;
    }

    INT load1_index = load_stream->memory_ops[cand1_idx].index;
    INT load2_index = load_stream->memory_ops[cand2_idx].index;
    BOOL in_order = wrapped ? TRUE : (load1_index < load2_index);
    INT first_index = in_order ? load1_index : load2_index;

    OP *load1_op = loop_ivs->Op(load1_index);
    OP *load2_op = loop_ivs->Op(load2_index);
    OP *first_load = loop_ivs->Op(first_index);

    /* Insert before the first load instruction:
     * if (stream->increasing)
     *   op[t1,t2 = ldp ofst1($addr_tn)]
     * else
     *   op[t2,t1 = ldp ofst1($addr_tn)]
     */

    // We compute the address of the packed operation.
    // This address is the address of load1 if stream is increasing,
    // load2 otherwise.
    // If operations are in order, we compute this address from load1,
    // from load2 otherwise.
    // (in_order && increasing)   packed_addr = addr1
    // (!in_order && increasing)  packed_addr = addr1 == (addr2 - memsize1)
    // (in_order && !increasing)  packed_addr = addr2 == (addr1 - memsize2)
    // (!in_order && !increasing) packed_addr = addr2

    int adjust_packed_offset = 0;
    if (in_order && !load_stream->increasing)
      adjust_packed_offset = -load_stream->memory_ops[cand2_idx].memsize;
    else if (!in_order && load_stream->increasing)
      adjust_packed_offset = -load_stream->memory_ops[cand1_idx].memsize;

    TN *packed_addr_tn = OP_Base(first_load);
    INT packed_addr_omega = OP_omega(first_load, OP_find_opnd_use(first_load, OU_base));

    TN *packed_offset_tn = Adjust_Offset_TN(OP_Offset(first_load), adjust_packed_offset);

    TN *tP = Gen_Register_TN (ISA_REGISTER_CLASS_integer, packed_size);
    TN *t1 = Dup_TN(OP_result(load1_op, 0));
    TN *t2 = Dup_TN(OP_result(load2_op, 0));

    OPS ops_packed = OPS_EMPTY;
    TYPE_ID packed_type = MTYPE_TransferSize(packed_size, load_type);
    Expand_Load(OPCODE_make_op(OPR_LDID, packed_type, packed_type),
		tP,
		packed_addr_tn,
		packed_offset_tn,
		&ops_packed);
    OP_srcpos(OPS_first(&ops_packed)) = OP_srcpos(load1_op);
    OPS_Copy_Predicate (&ops_packed, load1_op);

    if (load_stream->increasing)
      Extract_Packed2(t1, t2, tP, load_type, &ops_packed);
    else
      Extract_Packed2(t2, t1, tP, load_type, &ops_packed);
    CG_LOOP_Init_OPS(&ops_packed);

    OP *packed_load = NULL;

    {
      OP *new_op;
      FOR_ALL_OPS_OPs_FWD(&ops_packed, new_op) {
	// FdF 20051003: Do not copy unroll information on packed op,
	// to avoid dependencies to be optimized.
	if (OP_load(new_op)) {
	  packed_load = new_op;
	  // FdF 20070510: Do not copy the WN information, but create
	  // a map op->(op1,op2)
	  // Copy_WN_For_Memory_OP (new_op, load1_op);
	  if (wrapped) {
	    int unrolling = OP_unrolling(load2_op) + BB_unrollings(OP_bb(load2_op));
	    if (unrolling == 0)
	      unrolling = 1;
	    Set_OP_unrolling(load2_op, unrolling);
	  }
	  Set_Packed_Ops(new_op, 2, load1_op, load2_op);
	  if (packed_addr_omega != 0) {
	    for (int i = 0; i < OP_opnds(new_op); i++) {
	      if (OP_opnd(new_op, i) == packed_addr_tn)
		Set_OP_omega(new_op, i, 1);
	    }
	  }
	}
	Set_OP_unroll_bb(new_op, OP_unroll_bb(load1_op));
	Set_OP_unrolling(new_op, OP_unrolling(load1_op));
	Set_OP_orig_idx(new_op, OP_orig_idx(load1_op));
      }
    }

    Is_True(packed_load, ("Packed load operation not found"));

    BB_Insert_Ops(body, first_load, &ops_packed, TRUE);

    /* Now replace load1_op
     *   op[tx = load ofst1($addr_tn)]
     * by
     *   op[tx = t1]
     */

    OPS ops_load1 = OPS_EMPTY;
    Exp_COPY(OP_result(load1_op, 0), t1, &ops_load1);
    CG_LOOP_Init_OPS(&ops_load1);
    BB_Insert_Ops(body, load1_op, &ops_load1, TRUE);

    // Before replacing load2_op, insert a copy at the end of the loop
    // in case load2_op is at the beginning of the next operation.

    if (wrapped) {
      OPS ops_wrapped = OPS_EMPTY;
      Exp_COPY(t0, t2, &ops_wrapped);
      CG_LOOP_Init_OPS(&ops_wrapped);

      OP *point = BB_last_op(body);
      BOOL before = (point != NULL) && OP_xfer(point);
      BB_Insert_Ops(body, point, &ops_wrapped, before);
      t2 = t0;
    }

    /* Replace load2_op
     *   op[ty = load ofst2($addr_tn)]
     * by
     *   op[ty = t2]
     */

    OPS ops_load2 = OPS_EMPTY;
    Exp_COPY(OP_result(load2_op, 0), t2, &ops_load2);
    CG_LOOP_Init_OPS(&ops_load2);

    // Also, set the omega information on this op, since t2 was defined
    // in the previous iteration
    if (wrapped) {
      OP *new_op;
      FOR_ALL_OPS_OPs_FWD(&ops_load2, new_op) {
	for (int i = 0; i < OP_opnds(new_op); i++) {
	  if (OP_opnd(new_op, i) == t0)
	    Set_OP_omega(new_op, i, 1);
	}
      }
    }
    BB_Insert_Ops(body, load2_op, &ops_load2, TRUE);

    // Replace the first operation in the stream by the packed
    // operation.

    load_stream->memory_ops[cand1_idx].memsize += load_stream->memory_ops[cand2_idx].memsize;
    load_stream->memory_ops[cand1_idx].index = first_index;
    loop_ivs->Replace_Op(first_index, packed_load);

    // In case of decreasing stream, the packed operation is based on
    // the second operation, unless we are on the last packed
    // operation of a peeled stream. So we still use the first
    // operation, but more update is needed.
    if (!load_stream->increasing) {
      load_stream->memory_ops[cand1_idx].offset -= load_stream->memory_ops[cand2_idx].memsize;
    }

    // Cancel the second operation in the stream.
    load_stream->memory_ops[cand2_idx].index = 0;

    // Remove the initial load operations
    BB_Remove_Op(body, load1_op);
    BB_Remove_Op(body, load2_op);
  }

  Update_Stream(load_stream);

  return TRUE;
}

static void
Compose_Packed2(TN *tP, TN *t1, TN *t2, TYPE_ID store_type, OPS *ops_packed) {

  if (Target_Byte_Sex == BIG_ENDIAN) {
    TN *tmp = t1;
    t1 = t2;
    t2 = tmp;
  }

  switch (store_type) {
  case MTYPE_I4:
  case MTYPE_U4:
    Expand_Compose(tP, t1, t2, ops_packed);
    break;

  case MTYPE_I2:
  case MTYPE_U2:
    Exp_SIMD_Shuffle_Bits(tP, t1, 0, 16, t2, 0, 16, ops_packed);
    break;

  default:
    Is_True(0, ("Only Word and Half Word packing is supported."));
  }
}

static BOOL
Combine_Adjacent_Stores(LOOP_IVS *loop_ivs, MemoryStream_t *store_stream, int packed_size)
{
  /* Stores must be statically correctly aligned. */
  Is_True(IS_POWER_OF_2(packed_size), ("Packed_size must be 2^n"));

  if ((store_stream->align_base < packed_size) ||
      ((store_stream->align_bias%packed_size) != 0))
    return FALSE;

  // Current limitations
  if ((store_stream->allmemsize < 2) || (store_stream->allmemsize >= 8))
    return FALSE;

  // Only pairs of stores can be packed.
  if ((store_stream->memop_count&1) != 0)
    return FALSE;

  Is_True(store_stream->allmemsize == (packed_size/2), ("Packing not implemented for 4h->1L"));

  if (Get_Trace(TP_CGLOOP, 0x10))
    fprintf(TFile, "<ivs packing> Combine adjacent %d Stores\n", store_stream->memop_count);

  Is_True(CG_LOOP_prolog != NULL, ("Incorrect loop: Missing loop prolog"));

  BB *body = OP_bb(loop_ivs->Op(store_stream->memory_ops[0].index));

  // Now, update store operations by pairs
  for (int cand1_idx = 0; cand1_idx < store_stream->memop_count; cand1_idx += 2) {
    INT cand2_idx = cand1_idx+1;
    Is_True(cand2_idx < store_stream->memop_count, ("Incorrect number of memory operation in store stream"));

    INT store1_index = store_stream->memory_ops[cand1_idx].index;
    INT store2_index = store_stream->memory_ops[cand2_idx].index;
    BOOL in_order = (store1_index < store2_index);
    INT last_index = in_order ? store2_index : store1_index;

    OP *store1_op = loop_ivs->Op(store1_index);
    OP *store2_op = loop_ivs->Op(store2_index);
    OP *last_store = loop_ivs->Op(last_index);

    // Replace the store operations in the loop.

    /* Insert after the last store instruction
     * (stream->increasing)
     *   stp ofst2($addr_tn), t1,t2
     * else
     *   stp ofst2($addr_tn), t2,t1
     */

    TN *store1_tn = OP_Storeval(store1_op);
    TN *store2_tn = OP_Storeval(store2_op);

    TN *tP = Gen_Register_TN (ISA_REGISTER_CLASS_integer, packed_size);
    TN *t1 = TN_is_const_reg(store1_tn) ? store1_tn : Dup_TN_Even_If_Dedicated(store1_tn);
    TN *t2 = TN_is_const_reg(store2_tn) ? store2_tn : Dup_TN_Even_If_Dedicated(store2_tn);
    TYPE_ID store_type = MTYPE_TransferSize(packed_size/2, MTYPE_I4);

    OPS ops_packed = OPS_EMPTY;
    if (store_stream->increasing)
      Compose_Packed2(tP, t1, t2, store_type, &ops_packed);
    else
      Compose_Packed2(tP, t2, t1, store_type, &ops_packed);

    TN *packed_addr_tn = OP_Base(last_store);
    INT packed_addr_omega = OP_omega(last_store, OP_find_opnd_use(last_store, OU_base));

    // We compute the address of the packed operation.
    // This address is the address of store1 if stream is increasing,
    // store2 otherwise.
    // If operations are in order, we compute this address from store2,
    // from store1 otherwise.
    // (in_order && increasing)   packed_addr = addr1 == (addr2 - memsize1)
    // (!in_order && increasing)  packed_addr = addr1
    // (in_order && !increasing)  packed_addr = addr2
    // (!in_order && !increasing) packed_addr = addr2 == (addr1 - memsize2)

    int adjust_packed_offset = 0;
    if (in_order && store_stream->increasing)
      adjust_packed_offset = -store_stream->memory_ops[cand1_idx].memsize;
    else if (!in_order && !store_stream->increasing)
      adjust_packed_offset = -store_stream->memory_ops[cand2_idx].memsize;

    TN *packed_offset_tn = Adjust_Offset_TN(OP_Offset(last_store), adjust_packed_offset);

    Expand_Store(MTYPE_TransferSize(packed_size, store_type),
		 tP,
		 packed_addr_tn,
		 packed_offset_tn,
		 &ops_packed);

    OP_srcpos(OPS_last(&ops_packed)) = OP_srcpos(store2_op);
    OPS_Copy_Predicate (&ops_packed, store2_op);

    /* Initialize the OP_omega for new operations. */
    OP *packed_store = NULL;
    CG_LOOP_Init_OPS(&ops_packed);
    {
      OP *new_op;
      FOR_ALL_OPS_OPs_FWD(&ops_packed, new_op) {
	// FdF 20051003: Do not copy unroll information on packed
	// operation, to avoid dependencies to be optimized.
	if (OP_store(new_op)) {
	  packed_store = new_op;
	  // FdF 20070510: Do not copy the WN information, but create
	  // a map op->(op1,op2)
	  // Copy_WN_For_Memory_OP (new_op, store2_op);
	  Set_Packed_Ops(new_op, 2, store1_op, store2_op);
	  if (packed_addr_omega != 0) {
	    for (int i = 0; i < OP_opnds(new_op); i++) {
	      if (OP_opnd(new_op, i) == packed_addr_tn)
		Set_OP_omega(new_op, i, 1);
	    }
	  }
	}
	Set_OP_unroll_bb(new_op, OP_unroll_bb(store1_op));
	Set_OP_unrolling(new_op, OP_unrolling(store1_op));
	Set_OP_orig_idx(new_op, OP_orig_idx(store1_op));
      }
    }

    BB_Insert_Ops(body, last_store, &ops_packed, FALSE);

    /* Replace the first store instruction:
     * stw ofst1($addr_tn), store1_tn
     * by
     * t1 = store1_tn
     */

    OPS ops_stw1 = OPS_EMPTY;
    Exp_COPY(t1, store1_tn, &ops_stw1);
    CG_LOOP_Init_OPS(&ops_stw1);
    BB_Insert_Ops(body, store1_op, &ops_stw1, FALSE);

    /* Replace the second store instruction:
     * stw ofst2($addr_tn), store2_tn
     * by
     * t2 = store2_tn
     */

    OPS ops_stw2 = OPS_EMPTY;
    Exp_COPY(t2, store2_tn, &ops_stw2);
    CG_LOOP_Init_OPS(&ops_stw2);
    BB_Insert_Ops(body, store2_op, &ops_stw2, FALSE);

    // Replace the first operation in the stream by the packed
    // operation.

    store_stream->memory_ops[cand1_idx].memsize += store_stream->memory_ops[cand2_idx].memsize;
    store_stream->memory_ops[cand1_idx].index = last_index;
    loop_ivs->Replace_Op(last_index, packed_store);

    // In case of decreasing stream, the packed operation is based on
    // the second operation. Since we used the first operation, more
    // update is needed.
    if (!store_stream->increasing) {
      store_stream->memory_ops[cand1_idx].offset -= store_stream->memory_ops[cand2_idx].memsize;
    }

    // Cancel the second operation in the stream.
    store_stream->memory_ops[cand2_idx].index = 0;

    /* Finally, remove the two store instructions. */

    BB_Remove_Op(body, store1_op);
    BB_Remove_Op(body, store2_op);
  }

  Update_Stream(store_stream);

  return TRUE;
}

static void
Init_Packing_Candidates(LOOP_IVS *loop_ivs) {

  OP *op;
  INT opidx;
  FOR_ALL_LOOP_IVS_OPs_FWD(loop_ivs, opidx, op ) {
    if ( LoadStore_Packing_Candidate_Op( op ) )
      Set_OP_flag1( op );
    else
      Reset_OP_flag1( op );
  }

  // Search the CG dependence graph, there must be no alias between
  // operations candidates for load/store packing.
  FOR_ALL_LOOP_IVS_OPs_FWD( loop_ivs, opidx, op ) {
    if ( OP_flag1( op ) ) {
      if (OP_load(op)) {
	ARC_LIST *arcs = OP_preds( op );
	while ( arcs ) {
	  ARC *arc = ARC_LIST_first( arcs );
	  arcs = ARC_LIST_rest( arcs );

	  OP *op_pred = ARC_pred( arc );
	  if ( (BB_loop_head_bb(OP_bb(op_pred)) == BB_loop_head_bb(OP_bb(op))) &&
	       !Packing_Load_Candidate_Arc( arc ) ) {
	    Reset_OP_flag1(op);
	    Reset_OP_flag1(op_pred);
	  }
	}
      }
      else {
	ARC_LIST *arcs = OP_succs( op );
	while ( arcs ) {
	  ARC *arc = ARC_LIST_first( arcs );
	  arcs = ARC_LIST_rest( arcs );

	  OP *op_succ = ARC_succ( arc );
	  if ( (BB_loop_head_bb(OP_bb(op_succ)) == BB_loop_head_bb(OP_bb(op))) &&
	       !Packing_Store_Candidate_Arc( arc ) ) {
	    Reset_OP_flag1(op);
	    Reset_OP_flag1(op_succ);
	  }
	}
      }
    }
  }
}

static INT
Compute_Packing_IVs(LOOP_IVS *loop_ivs) {

  INT candidate_count = 0;
  OP *op;
  INT opidx;
  FOR_ALL_LOOP_IVS_OPs_FWD( loop_ivs, opidx, op ) {
    if ( !OP_flag1( op ) )
      continue;
    int base_idx = OP_find_opnd_use(op, OU_base);
    DefID_t IV_index = loop_ivs->OPND_IV_cycle(opidx, base_idx);
    if (IV_index == 0) {
      Reset_OP_flag1(op);
      continue;
    }
    // Get the step of the induction variable
    INT64 step = loop_ivs->IV_step(IV_index);
    // It must be a multiple of the memory size
    int memsize = OP_Mem_Ref_Bytes(op);
    if (step % memsize) {
      Reset_OP_flag1(op);
      continue;
    }
    candidate_count ++;
  }

  return candidate_count;
}

static INT
Normalized_bias(INT base, INT bias) {

  while (bias < 0)
    bias += base;

  while (bias >= base)
    bias -= base;

  return bias;
}

// TBD: Consider also #pragma unroll, since more unrolling may enable
// more packing, and thus may require specialization.
static void
Get_Stream_Alignment(LOOP_IVS *loop_ivs, MemoryStream_t *stream) {

  // Set alignment information on this stream. There are two cases to
  // consider, (step>0) and (step<0). Memory accesses before and after
  // packing are shown in the figure below, for the two cases. Wx and
  // Px represent respectively memory locations before and after
  // packing. Ax represents where the address for an access points to.

  //  step > 0
  //        W0    W1    W2    W3
  //      A0~~~ A1~~~ A2~~~ A3~~~
  //           P0          P2
  //      A0~~~~~~~~~ A2~~~~~~~~~

  //  step < 0
  //        W3    W2    W1    W0
  //      A3~~~ A2~~~ A1~~~ A0~~~
  //           P2          P0
  //      A2~~~~~~~~~ A0~~~~~~~~~

  //  We must ensure that A0 and A2 are correctly aligned for the
  //  packed operation.

  //  In case (step>0), saying that P0 is correctly aligned is the
  //  same as saying that the access to W0 is correctly aligned.

  //  In case (step<0), this is the same as saying that A1 is
  //  correctly aligned, or also that (A0-size(W1)) is correctly
  //  aligned. Since we are in a stream of contiguous accesses, this
  //  is also the same as saying that A0+size(W0) is correctly
  //  aligned. The advantage of this second expression is that it only
  //  refers to the first operation.

  //  What we know is the value of the induction variable IV for the
  //  stream at the loop entry, and we have alignment property for
  //  this value in the form (base,bias). Now, we have to consider the
  //  offset from the value of IV to the address of A0. This is shown
  //  in the code below:

  //  IV = ....
  //  loop:
  //  IV += off1
  //  memref @(IV+off2) # W0

  // Thus, the condition that IV must satisfies at loop entry so that
  // a packed operation is correctly aligned is:
  //  step > 0, (IV+(align_offset = off1+off2)) % size(P0) == 0
  //  step < 0, (IV+(align_offset = off1+off2+size(A0))) % size(P0) == 0

  if (stream->increasing)
    stream->align_offset = stream->memory_ops[0].offset;
  else
    stream->align_offset = stream->memory_ops[0].offset + stream->memory_ops[0].memsize;

  // Then, compute the base and bias for the address for the first
  // packed operation in the stream.

  INT base = 1, bias = 0;

  if (Enable_Misaligned_Access ||
      ((stream->memop_kind == MEMOP_LOAD) && Enable_Misaligned_Load) ||
      ((stream->memop_kind == MEMOP_STORE) && Enable_Misaligned_Store))
    // Consider that alignment is correct, since there is no requirement
    base = 0;
  else {
    Get_Memop_Alignment(loop_ivs->Prolog(), loop_ivs->Op(stream->memory_ops[0].index),
			stream->align_offset, &base, &bias);
    // Look for an option or a #pragma stream_alignment
    if (base == 1) {
      BB *loophead = BB_loop_head_bb(OP_bb(loop_ivs->Op(stream->memory_ops[0].index)));
      int pragma_alignment = Get_Pragma_Alignment(loophead);
      if (pragma_alignment > 1) {
	base = pragma_alignment;
	bias = 0;
      }
      else if (CG_LOOP_stream_align > 1) {
	base = CG_LOOP_stream_align;
	bias = 0;
      }	  
    }
  }

  // Then, check if the stream contains some accesses that guarantee a
  // more strict alignment. For example, consider a stream with 3
  // operations, (ldw, ldw, lpd). Since the IV must be correctly
  // aligned on the ldp operation, we know that it is also correctly
  // aligned for an ldp on the first ldw.

  // There is at least one operation with a base alignment greater to
  // base.
  if (base <= (stream->allmemsize/2)) {
    do {
      base *= 2;
    } while (base <= (stream->allmemsize/2));
    // Now, find the operation in the stream with alignment
    // constraint equal to base, and use the offset of this
    // operation to adjust the bias.
    INT i;
    for (i = 0; i < stream->memop_count; i++) {
      if (stream->memory_ops[i].memsize == base) {
	bias -= (stream->memory_ops[i].offset-stream->memory_ops[0].offset);
	bias = Normalized_bias(base, bias);
	break;
      }
    }
    Is_True(i < stream->memop_count, ("Could not find an operation with the expected alignment\n"));
  }

  stream->align_base = base;
  stream->align_bias = bias;
}

static MemoryStream_t *
Find_Consecutive_Memops(LOOP_IVS *loop_ivs, MemoryOP_t *memory_ops) {

  OP *op;
  INT opidx;
  UINT8 memop_kind = 0;

  FOR_ALL_LOOP_IVS_OPs_FWD( loop_ivs, opidx, op ) {
    if (!OP_flag1(op))
      continue;

    INT IV_index, IV_offset;
    ST *symbol = NULL;
    INT32 relocs = 0;
    INT64 offset_val;

    if (!Get_OP_IV_info(loop_ivs, opidx, op,
			&IV_index, &IV_offset,
			&symbol, &relocs, &offset_val))
      continue;

    INT64 step = loop_ivs->IV_step(IV_index);
    int streamSize = step > 0 ? step : -step;
    // streamSize may not be a power of 2, for example it can be
    // 6x4byte words that will be packed in 3x8byte words.

    INT memop_idx = 0;
    memory_ops[memop_idx].index = opidx;
    memop_kind = OP_load(op) ? MEMOP_LOAD : MEMOP_STORE;

    // Offset is initialized to the offset of the base of the memory
    // from the value of the primary induction variable at loop entry
    // and the offset on this memory operation.
    memory_ops[memop_idx].offset = IV_offset + offset_val;
    memory_ops[memop_idx].memsize = OP_Mem_Ref_Bytes(op);
    memop_idx ++;
    Reset_OP_flag1(op);

    // Then, look for step/4 32 bit operations, that can be packed
    // into step/8 64 bit operations

    OP *op2;
    INT opidx2;
    for (opidx2 = opidx+1; opidx2 <= loop_ivs->Last_opidx(); opidx2 ++) {
      op2 = loop_ivs->Op(opidx2);

      // Look for a sequence of identical operations
      if (!OP_flag1(op2) ||
	  (OP_code(op) != OP_code(op2)))
	continue;

      INT IV_index2, IV_offset2;
      ST *symbol2 = NULL;
      INT32 relocs2 = 0;
      INT64 offset2_val;

      if (!Get_OP_IV_info(loop_ivs, opidx2, op2,
			  &IV_index2, &IV_offset2,
			  &symbol2, &relocs2, &offset2_val))
	continue;

      // This operation must be based on the same IV as the reference one.
      if ((IV_index != IV_index2) ||
	  (symbol != symbol2) ||
	  (relocs != relocs2))
	continue;

      // This operation is compatible with the first operation, add it
      // into memory_ops
      memory_ops[memop_idx].index = opidx2;
      memory_ops[memop_idx].offset = IV_offset2 + offset2_val;
      memory_ops[memop_idx].memsize = OP_Mem_Ref_Bytes(op2);
      memop_idx++;
      Reset_OP_flag1(op2);
    }

    // A set of operations compatible together for packing have been
    // collected. Find contiguous operations that can be packed into
    // one or more contiguous packed operations.

    // Sort the operations in increasing/decreasing order of the
    // memory accesses.

    if (step > 0)
      qsort(memory_ops, memop_idx, sizeof(MemoryOP_t), Compare_Offsets);
    else
      qsort(memory_ops, memop_idx, sizeof(MemoryOP_t), Compare_Offsets_neg);

    // Find memop_count operations that are all contiguous, set again OP_flag1 for the remaining operations
    int size = 0;
    int i, memop_count, memop_ofst;
    bool contiguous = FALSE;
    for (memop_ofst = 0; memop_ofst <memop_idx; memop_ofst++) {
      MemoryOP_t *stream_memop = &memory_ops[memop_ofst];
      size = stream_memop[0].memsize;
      for (i = 1; ((memop_ofst+i) < memop_idx) && (size < streamSize); i++) {
	// Check if contiguous
	if (step > 0) {
	  if ((stream_memop[i-1].offset + stream_memop[i-1].memsize) != stream_memop[i].offset)
	    break;
	}
	else {
	  if ((stream_memop[i-1].offset - stream_memop[i].memsize) != stream_memop[i].offset)
	    break;
	}
	size += stream_memop[i].memsize;
      }
      if (size == streamSize) {
	contiguous = TRUE;
	memop_count = i;
	break;
      }
    }

    // No sequence of contiguous memop_count operations
    if (!contiguous)
      continue;

    // Otherwise, move the memop_count contiguous operations at the
    // begining of the memory_ops, and restore OP_flag1 on the other
    // operations.

    if (memop_ofst > 0) {
      for (i = 0; i < memop_ofst; i++)
	Set_OP_flag1(loop_ivs->Op(memory_ops[i].index));
      for (i = 0; i < memop_count; i++)
	memory_ops[i] = memory_ops[memop_ofst+i];
    }
    for (i = memop_ofst + memop_count; i < memop_idx; i++)
      Set_OP_flag1(loop_ivs->Op(memory_ops[i].index));

    MemoryStream_t *cur_stream;
    cur_stream = TYPE_MEM_POOL_ALLOC(MemoryStream_t, loop_ivs->Mem_pool());

    cur_stream->size = streamSize;
    cur_stream->memop_count = memop_count;
    cur_stream->memory_ops = memory_ops;
    cur_stream->IV_tn = OP_result(loop_ivs->Op(DEFID_idx(IV_index)), DEFID_res(IV_index));
    cur_stream->next = NULL;
    cur_stream->increasing = (step > 0);

    cur_stream->allmemsize = 0;
    for (i = 0; i < memop_count; i++)
      cur_stream->allmemsize |= memory_ops[i].memsize;

    cur_stream->align_base = 1;
    cur_stream->align_bias = 0;
    cur_stream->align_offset = 0;
    cur_stream->align_kind = STREAM_NONE;
    cur_stream->memop_kind = memop_kind;
    cur_stream->transform_kind = 0;
    cur_stream->packing_base = 1;
    cur_stream->unroll = 1;

    return cur_stream;
  }

  return NULL;
}

static MemoryStream_t *
Find_All_Streams(LOOP_IVS *loop_ivs, INT candidate_count, INT allSizes ) {

  // Allocate an array to store all memory operations that have a same
  // base IV and symbolic offset
  MemoryOP_t *memop_table;
  memop_table = TYPE_MEM_POOL_ALLOC_N(MemoryOP_t, 
				      loop_ivs->Mem_pool(), 
				      candidate_count);

  MemoryStream_t *first_stream, *last_stream, *cur_stream;
  MemoryOP_t *cur_memop;

  for (first_stream = NULL, cur_memop = memop_table;
       (cur_stream = Find_Consecutive_Memops(loop_ivs, cur_memop)) != NULL;
       cur_memop += cur_stream->memop_count) {

    // Discard streams with memory items of different size
    // TBD: Do packing on such streams.
    if (!IS_POWER_OF_2(cur_stream->allmemsize))
      continue;

    // Check also that the stream contains memory operations that can
    // be packed to form an operation of the size in allSizes

    if ((cur_stream->allmemsize & (allSizes/2)) == 0)
      continue;

    // Link this stream in the list
    if (first_stream == NULL) first_stream = cur_stream;
    else last_stream->next = cur_stream;
    last_stream = cur_stream;
  }

  return first_stream;
}

/*
  A stream has several independent properties to control the loop
  transformation that can be applied for packing:

  STREAM_ALIGNED if align_base >= pack_base et align_bias%pack_base == 0

  STREAM_UNC_PEEL_ALIGNED if align_base >= pack_base et (align_bias+size)%pack_base == 0

  STREAM_COND_PEEL_ALIGNED if (align_base == pack_base/2 && (align_bias == 0) && (size%pack_base == pack_base/2))

  STREAM_SPECIAL_ALIGNED if (align_base < pack_base && align_bias == 0)

  A load stream can also be packed if it is statically aligned on the
  second operation of the stream. In this case the first operation is
  moved out of the loop and the last operation of the stream is packed
  with the first one in the next iteration. Thus for load stream the
  property STREAM_ALIGNED and STREAM_UNC_PEEL_ALIGN are also set if
  the condition holds for align_bias+memory_ops[0].memsize.

  Now, according to the stream properties, mark under which loop
  transformation this stream would be packed.

  transform_none:      ALG  // All well aligned streams

  Transform_Unc_Peel:  PEEL // All well aligned streams after unconditional peeling

  Transform_Cond_Peel: (ALG&PEEL) && // All well aligned streams with and without loop peeling,
                       COND_PEEL     // plus one of the stream COND_PEEL stream
                                   
  Transform_Special:   ALG &&  // All well aligned streams,
                       SPECIAL // plus the streams candidate for specialization

 */
static void
Initialize_Stream_Packing_Property(MemoryStream_t *cur_stream, INT pack_base) {

  INT cur_base = cur_stream->align_base;
  INT cur_bias = cur_stream->align_bias;

  Is_True(IS_POWER_OF_2(pack_base), ("pack_base must be a power of 2"));

  cur_stream->align_kind = STREAM_NONE;

  // First, analyze the alignment property to set the stream
  // properties.

  if ((cur_base >= pack_base) && ((cur_bias%pack_base) == 0))
    cur_stream->align_kind |= STREAM_ALIGNED;

  if ((cur_base >= pack_base) && (((cur_bias+cur_stream->size)%pack_base) == 0))
    cur_stream->align_kind |= STREAM_UNC_PEEL_ALIGNED;

  if ((cur_base == (pack_base/2)) && (cur_bias == 0) &&
      ((cur_stream->size%pack_base) == (pack_base/2)))
    cur_stream->align_kind |= STREAM_COND_PEEL_ALIGNED;

  if ((cur_base < pack_base) && (cur_bias == 0))
    cur_stream->align_kind |= STREAM_SPECIAL_ALIGNED;

  if (cur_stream->memop_kind == MEMOP_LOAD /* && Allow_Next_Load_Packing*/) {
    INT next_bias = cur_bias + cur_stream->memory_ops[0].memsize;
    if ((cur_base >= pack_base) && ((next_bias%pack_base) == 0))
      cur_stream->align_kind |= STREAM_NEXT_ALIGNED;

    if ((cur_base >= pack_base) && (((next_bias+cur_stream->size)%pack_base) == 0))
      cur_stream->align_kind |= STREAM_UNC_PEEL_NEXT_ALIGNED;
  }

  // Now, mark which loop transformation would allow this stream to be
  // packed.

  cur_stream->transform_kind = 0;

  if ((cur_stream->align_kind & STREAM_ALIGNED) ||
      (cur_stream->align_kind & STREAM_NEXT_ALIGNED))
    SET_STREAM_TRANSFORM(cur_stream, LOOP_TRANSFORM_NONE);

  if (cur_stream->align_kind & STREAM_UNC_PEEL_ALIGNED)
    SET_STREAM_TRANSFORM(cur_stream, LOOP_TRANSFORM_UNC_PEEL);

  if (((cur_stream->align_kind & STREAM_ALIGNED) &&
       (cur_stream->align_kind & STREAM_UNC_PEEL_ALIGNED)) ||
      ((cur_stream->align_kind & STREAM_NEXT_ALIGNED) &&
       (cur_stream->align_kind & STREAM_UNC_PEEL_NEXT_ALIGNED)) ||
      (cur_stream->align_kind & STREAM_COND_PEEL_ALIGNED))
    SET_STREAM_TRANSFORM(cur_stream, LOOP_TRANSFORM_COND_PEEL);

  if ((cur_stream->align_kind & STREAM_ALIGNED) ||
      (cur_stream->align_kind & STREAM_NEXT_ALIGNED) ||
      (cur_stream->align_kind & STREAM_SPECIAL_ALIGNED))
    SET_STREAM_TRANSFORM(cur_stream, LOOP_TRANSFORM_SPECIALIZE);

  if (Get_Trace(TP_CGLOOP, 0x10)) {
    fprintf(TFile, "<ivs packing> %s stream, %d memops, align base %d, alignment",
	    cur_stream->memop_kind == MEMOP_LOAD ? "load" : "store",
	    cur_stream->memop_count, 
	    cur_stream->align_base);
    char sep = '[';
    if (cur_stream->align_kind == STREAM_NONE) {
      fprintf(TFile, "%cnone", sep);
      sep = ' ';
    }
    if (cur_stream->align_kind & STREAM_ALIGNED) {
      fprintf(TFile, "%caligned", sep);
      sep = ' ';
    }
    if (cur_stream->align_kind & STREAM_UNC_PEEL_ALIGNED) {
      fprintf(TFile, "%cunc_peel", sep);
      sep = ' ';
    }
    if (cur_stream->align_kind & STREAM_NEXT_ALIGNED) {
      fprintf(TFile, "%caligned_next", sep);
      sep = ' ';
    }
    if (cur_stream->align_kind & STREAM_UNC_PEEL_NEXT_ALIGNED) {
      fprintf(TFile, "%cunc_peel_next", sep);
      sep = ' ';
    }
    if (cur_stream->align_kind & STREAM_COND_PEEL_ALIGNED) {
      fprintf(TFile, "%ccond_peel", sep);
      sep = ' ';
    }
    if (cur_stream->align_kind & STREAM_SPECIAL_ALIGNED) {
      fprintf(TFile, "%cspecial", sep);
      sep = ' ';
    }
    fprintf(TFile, "]\n");
  }

}

static INT
Cond_Peel_Streams(LOOP_IVS *loop_ivs, MemoryStream_t *first_stream,
		  INT cond_peel_count, INT maxAlign, INT memop_kind) {

  MemoryStream_t *peel_stream, *cur_stream;
  INT peel_stream_idx, cur_stream_idx;

  if (cond_peel_count == 0)
    return 0;

  else if (cond_peel_count == 1) {
    // Return this streams if it is of appropriate memop_kind
    for (peel_stream = first_stream; peel_stream != NULL; peel_stream = peel_stream->next) {
      if (peel_stream->align_kind & STREAM_COND_PEEL_ALIGNED)
	if (peel_stream->memop_kind == memop_kind)
	  return 1;
	else
	  return 0;
    }
    Is_True(0, ("COND_PEEL_ALIGNED stream must have been found in the loop"));
  }

  // For MEMOP_LOAD, and if there is more than one conditional peel
  // stream, select which stream(s) gives the best gain. Subsequent
  // calls to this function will just return the number of cond peel
  // streams for the current memop_kind.

  if (memop_kind == MEMOP_LOAD) {

    typedef INT memops_t[MEMOP_LAST];
    memops_t loc_peel_gain[8];
    memops_t *peel_gain;

    // Alloc an array in the pool only if more than 8 streams
    if (cond_peel_count <= 8)
      peel_gain = loc_peel_gain;
    else
      peel_gain = TYPE_MEM_POOL_ALLOC_N(memops_t, loop_ivs->Mem_pool(), cond_peel_count);

    for (int idx = 0; idx < cond_peel_count; idx ++)
      for (int kind = MEMOP_LOAD; kind < MEMOP_LAST; kind ++)
	peel_gain[idx][kind] = 0;

    peel_stream_idx = -1;

    for (peel_stream = first_stream; peel_stream != NULL; peel_stream = peel_stream->next) {
      // Not a stream candidate for dynamic loop peeling
      if (!(peel_stream->align_kind & STREAM_COND_PEEL_ALIGNED)) continue;

      peel_stream_idx ++;

      // For each stream candidate for peeling, compute the gain of
      // peeling this stream.
      TN *peel_base = peel_stream->IV_tn;

      cur_stream_idx = -1;
      for (cur_stream = first_stream; cur_stream != NULL; cur_stream = cur_stream->next) {
	// Not a stream candidate for dynamic loop peeling
	if (!(cur_stream->align_kind & STREAM_COND_PEEL_ALIGNED)) continue;

	cur_stream_idx ++;

	if (cur_stream != peel_stream) {

	  TN *cur_base = cur_stream->IV_tn;

	  // Not a stream with same base as peel_strean
	  if (cur_base != peel_base)
	    continue;

	  // Check also that if peel stream is aligned, cur_stream
	  // will also be aligned
	  if (((cur_stream->align_offset) % maxAlign) != ((peel_stream->align_offset) % maxAlign))
	    continue;

	  // peel_stream has already been processed while analyzing
	  // cur_stream. Just mark that peel_stream refers to
	  // cur_stream.
	  if (cur_stream_idx < peel_stream_idx) {
	    peel_gain[peel_stream_idx][MEMOP_LOAD] = -(cur_stream_idx+1);
	    // No need to go further
	    break;
	  }
	}

	peel_gain[peel_stream_idx][cur_stream->memop_kind] += cur_stream->memop_count;

      }
    }

    // Finally, choose one base_tn on which dynamic loop peeling would
    // be performed, and reset the dynamic peel property on all other
    // streams.

    peel_stream_idx = 0;
    for (cur_stream_idx = 1; cur_stream_idx < cond_peel_count; cur_stream_idx ++) {
      if ((peel_gain[cur_stream_idx][MEMOP_LOAD] > peel_gain[peel_stream_idx][MEMOP_LOAD]) ||
	  ((peel_gain[cur_stream_idx][MEMOP_LOAD] == peel_gain[peel_stream_idx][MEMOP_LOAD]) &&
	   (peel_gain[cur_stream_idx][MEMOP_STORE] > peel_gain[peel_stream_idx][MEMOP_STORE])))
	peel_stream_idx = cur_stream_idx;
    }

    cur_stream_idx = -1;
    for (cur_stream = first_stream; cur_stream != NULL; cur_stream = cur_stream->next) {
      // Not a stream candidate for dynamic loop peeling
      if (!(cur_stream->align_kind & STREAM_COND_PEEL_ALIGNED))	continue;

      cur_stream_idx ++;

      if (!((cur_stream_idx == peel_stream_idx) || 
	    (peel_gain[cur_stream_idx][MEMOP_LOAD] == -(peel_stream_idx+1))))
	RESET_STREAM_TRANSFORM(cur_stream, LOOP_TRANSFORM_COND_PEEL);
    }
  }

  // Streams to be peeled have already been selected, just return
  // the number of streams for current memop_kind.
  INT cond_peel_streams = 0;
  for (peel_stream = first_stream; peel_stream != NULL; peel_stream = peel_stream->next) {
    if (peel_stream->align_kind & STREAM_COND_PEEL_ALIGNED &&
	(peel_stream->memop_kind == memop_kind))
      cond_peel_streams ++;
  }

  return cond_peel_streams;
}

static BOOL
LoadStore_Packing( LOOP_IVS *loop_ivs, CG_LOOP &cg_loop, int allSizes)
{
  BOOL trace_packing_verbose = Get_Trace(TP_CGLOOP, 0x20);
  BOOL packing_done = FALSE;
  
  UINT32 num_bbs = BB_SET_Size(LOOP_DESCR_bbset(cg_loop.Loop()));
  BB *sorted_bbs[num_bbs];
  if (!Check_Simple_Loop(cg_loop.Loop(), sorted_bbs, num_bbs))
    return FALSE;

  loop_ivs->Init( cg_loop.Loop(), sorted_bbs, num_bbs );

  // Because peeling, specialization uses CG_LOOP_prolog and CG_LOOP_epilog
  if ((loop_ivs->Prolog() != CG_LOOP_prolog) ||
      (loop_ivs->Epilog() != CG_LOOP_epilog))
    return FALSE;

  // Identify (set flag1 for) all OPs that are suitable candidates for
  // 64 bit load/store packing.

  Init_Packing_Candidates(loop_ivs);

  // Make a pass to initialize the IV variables for each candidate
  // load/store, and see if there are opportunities for packing.

  INT candidate_count = Compute_Packing_IVs(loop_ivs);

  if (trace_packing_verbose) {
    fprintf(stdout, "  <packing> BB head %d\n", BB_id(loop_ivs->Head()));
    fprintf(stdout, "  <packing> found %d candidate operation(s)\n", candidate_count);
  }

  //  loop_ivs->Trace_IVs_Entries("LoadStore_Packing: after Compute_Packing_IVs");

  // Now, look for a sequence of load or store operations that can be
  // packed.
  if (candidate_count <= 1)
    return FALSE;

  //  Trace_Pack32_Entries(pack32_table, pack32_count, "Before main loop");

  // Statistics
  int load_cnt = 0;
  int mem_cnt = 0;
  OP *op;
  INT opidx;
  FOR_ALL_LOOP_IVS_OPs_FWD(loop_ivs, opidx, op) {
    if (OP_memory(op))
      mem_cnt ++;
    if (OP_load(op))
      load_cnt ++;
  }

  CG_SCHED_EST *se;
  int latency_II = 0;
  if (Get_Trace(TP_CGLOOP, 0x10)) {
    BB *bb;
    FOR_ALL_BB_SET_members(LOOP_DESCR_bbset(cg_loop.Loop()), bb) {
      se = CG_SCHED_EST_Create(bb, &MEM_local_nz_pool, SCHED_EST_FOR_UNROLL);
      latency_II += CG_SCHED_EST_Critical_Length(se);
    }
  }

  se = CG_SCHED_EST_Create_Empty(&MEM_local_nz_pool, SCHED_EST_FOR_UNROLL);
  FOR_ALL_LOOP_IVS_OPs_FWD(loop_ivs, opidx, op) {
    if (!OP_copy(op) && (OP_code(op) != TOP_extractp) && (OP_code(op) != TOP_composep))
      CG_SCHED_EST_Add_Op_Resources(se, OP_code(op));
  }
  int resource_II = CG_SCHED_EST_Resource_Cycles(se);

  if (((Loop_packing_flags & PACKING_ONLY_MEMORY_BOUNDED) != 0)
      && (mem_cnt < resource_II)) {
    if (trace_packing_verbose) {
      fprintf(stdout, "  <packing> Packing not performed because loop is not memory bounded\n");
    }
    return FALSE;
  }

  MemoryStream_t *first_stream = Find_All_Streams(loop_ivs, candidate_count, allSizes);

  if (trace_packing_verbose) {
    if (first_stream == NULL)      
      fprintf(stdout, "  <packing> no stream found\n");
    else {
      INT stream_count = 0;
      INT memop_count = 0;
      MemoryStream_t *stream;
      for (stream = first_stream; stream != NULL; stream = stream->next) {
	if (stream->allmemsize >= allSizes)
	  continue;

	// Only paired of ldw/stw operations can be packed.
	if ((stream->memop_count&1) != 0)
	  continue;

	stream_count ++;
	memop_count += stream->memop_count;
      }

      if (stream_count == 0)
	fprintf(stdout, "  <packing> no stream found\n");
      else
	fprintf(stdout, "  <packing> found %d operations for packing in %d stream(s)\n",
		memop_count, stream_count);
    }
  }

  //  fprintf(TFile, "32 bit Packing = {");
  INT load_stream_count = 0, store_stream_count = 0;
  MemoryStream_t *cur_stream;
  for (cur_stream = first_stream; cur_stream != NULL; cur_stream = cur_stream->next) {

    Get_Stream_Alignment(loop_ivs, cur_stream);

    BOOL done = FALSE;
    if (Get_Trace(TP_CGLOOP, 0x10)) {
      if (cur_stream->memop_kind == MEMOP_LOAD)
	fprintf(TFile, "<ivs packing> Load stream, %d memops, alignment static(%d)\n", cur_stream->memop_count, cur_stream->align_base);
      else
	fprintf(TFile, "<ivs packing> Store stream, %d memops, alignment static(%d)\n", cur_stream->memop_count, cur_stream->align_base);
    }

    for (INT packedSize = 2; packedSize <= allSizes; packedSize *= 2) {

      // Check if packing is enabled for this size
      if ((allSizes & packedSize) == 0)
	continue;

      // Only paired of ldw/stw operations can be packed.
      if ((cur_stream->memop_count&1) != 0)
	continue;

      // Check if the stream contains elements that can be packed into
      // a packedSize element.
      if ((cur_stream->allmemsize & (packedSize/2)) == 0)
	continue;

      if (cur_stream->memop_kind == MEMOP_LOAD) {
	if (Combine_Adjacent_Loads(loop_ivs, cur_stream, packedSize))
	  done = TRUE;
      }
      else {
	if (Combine_Adjacent_Stores(loop_ivs, cur_stream, packedSize))
	  done = TRUE;
      }
    }

    if (done) {
      if (cur_stream->memop_kind == MEMOP_LOAD)
	load_stream_count ++;
      else
	store_stream_count ++;
      packing_done = TRUE;
    }
  }

  if (trace_packing_verbose) {

    if ((load_stream_count+store_stream_count) == 0)
      fprintf(stdout, "  <packing> No packing done on this loop\n");
    else
      fprintf(stdout, "  <packing> Packing done on %d streams (%d loads, %d stores)\n", load_stream_count+store_stream_count, load_stream_count, store_stream_count);

    if ((load_stream_count != cg_loop.Load_streams()) || (store_stream_count != cg_loop.Store_streams())) {
      fprintf(stdout, "WARNING: Expected to pack %d(%d) loads and %d(%d) store streams\n", cg_loop.Load_streams(), load_stream_count, cg_loop.Store_streams(), store_stream_count);
    }
  }

  //  fprintf(TFile, "}\n");
  if (Get_Trace(TP_CGLOOP, 0x10)) {
    //    fprintf(TFile, "<ivs packing> after unrolling Load stream %d, Store stream %d, Peeling %d, Special %d\n", load_stream_count, store_stream_count, cg_loop.Peel_stream(), cg_loop.Specialize_streams());
    fprintf(TFile, "<ivs packing> after unrolling Load stream %d, Store stream %d\n", load_stream_count, store_stream_count);
    fprintf(TFile, "<ivs packing> latency_II %d, resource_II %d %s(memop=%d)\n", latency_II, resource_II, (mem_cnt == resource_II) ? "(memory bounded)" : "", mem_cnt);

    mem_cnt = 0;
    load_cnt = 0;
    FOR_ALL_LOOP_IVS_OPs_FWD(loop_ivs, opidx, op) {
      if (OP_memory(op))
	mem_cnt ++;
      if (OP_load(op))
	load_cnt ++;
    }

    latency_II = 0;
    BB *bb;
    FOR_ALL_BB_SET_members(LOOP_DESCR_bbset(cg_loop.Loop()), bb) {
      se = CG_SCHED_EST_Create(bb, &MEM_local_nz_pool, SCHED_EST_FOR_UNROLL);
      latency_II += CG_SCHED_EST_Critical_Length(se);
    }

    se = CG_SCHED_EST_Create_Empty(&MEM_local_nz_pool, SCHED_EST_FOR_UNROLL);
    FOR_ALL_LOOP_IVS_OPs_FWD(loop_ivs, opidx, op) {
      if (!OP_copy(op) && (OP_code(op) != TOP_extractp) && (OP_code(op) != TOP_composep))
	CG_SCHED_EST_Add_Op_Resources(se, OP_code(op));
    }
    resource_II = CG_SCHED_EST_Resource_Cycles(se);

    fprintf(TFile, "<ivs packing> new_latency_II %d, new_resource_II %d %s(memop=%d)\n", latency_II, resource_II, (mem_cnt == resource_II) ? "(memory bounded)" : "", mem_cnt);
  }

  return packing_done;
}

static BOOL
LoadStore_Check_Packing( LOOP_IVS *loop_ivs, CG_LOOP &cg_loop, int allSizes )
{
  BOOL trace_packing_verbose = Get_Trace(TP_CGLOOP, 0x20);

  UINT32 num_bbs = BB_SET_Size(LOOP_DESCR_bbset(cg_loop.Loop()));
  BB *sorted_bbs[num_bbs];
  if (!Check_Simple_Loop(cg_loop.Loop(), sorted_bbs, num_bbs))
    return FALSE;

  loop_ivs->Init( cg_loop.Loop(), sorted_bbs, num_bbs );

  // Because peeling, specialization uses CG_LOOP_prolog and CG_LOOP_epilog
  if ((loop_ivs->Prolog() != CG_LOOP_prolog) ||
      (loop_ivs->Epilog() != CG_LOOP_epilog))
    return FALSE;

  // Identify (set flag1 for) all OPs that are suitable candidates for
  // 64 bit load/store packing.

  Init_Packing_Candidates(loop_ivs);

  // Make a pass to initialize the IV variables for each candidate
  // load/store, and see if there are opportunities for packing.

  INT candidate_count = Compute_Packing_IVs(loop_ivs);

  if (trace_packing_verbose)
    fprintf(stdout, "  <packing check> found %d candidate operation(s)\n", candidate_count);

  // Now, look for a sequence of load or store operations that can be
  // packed.
  if (candidate_count < 1) return FALSE;

  //  Trace_Pack32_Entries(pack32_table, pack32_count, "Before main loop");

  // Compute the maximum alignment that a stream must satisfy to be
  // packed to the largest size.
  INT maxAlign;
  for (maxAlign = 2; maxAlign <= (allSizes/2); maxAlign *= 2);

  MemoryStream_t *first_stream = Find_All_Streams(loop_ivs, candidate_count, allSizes);

  /*
    Count the number of streams that can be packed according to the
    loop transformations that can be applied:

    - No loop transform: All well aligned streams will be packed == STREAM_ALIGNED

    - Unconditional loop peeling: All well aligned streams after
      unconditional peeling will be packed == STREAM_UNC_PEEL_ALIGNED

    - Conditional loop peeling: All well aligned streams with and
      without loop peeling, plus the stream on which the peel
      condition applies, will be packed == (STREAM_ALIGNED &&
      STREAM_UNC_PEEL) + (STREAM_COND_PEEL_ALIGNED > 0 ? 1 : 0)

    - Loop specialization: No stream will be packed if the specialized
      loop is not taken, or all well aligned streams plus the streams
      on which the specialize condition apply will be packed ==
      STREAM_ALIGNED + STREAM_SPECIAL_ALIGNED

    Load and store streams are counted separately because there is
    more expected gain on load packing than on store packing.

    */

  INT transform_stream_count[LOOP_TRANSFORM_LAST][MEMOP_LAST];
  INT static_stream_for_cond_peel_count[MEMOP_LAST] = {0,0};
  for (int trans_kind = 0; trans_kind < LOOP_TRANSFORM_LAST; trans_kind ++)
    transform_stream_count[trans_kind][MEMOP_LOAD] = transform_stream_count[trans_kind][MEMOP_STORE] = 0;

  // Count only streams on which packing can be performed.
  INT stream_count = 0, load_stream_count = 0, store_stream_count = 0;
  INT cond_peel_count = 0;
  
  MemoryStream_t *cur_stream;
  INT memop_count = 0;
  for (cur_stream = first_stream; cur_stream != NULL; cur_stream = cur_stream->next) {

    Get_Stream_Alignment(loop_ivs, cur_stream);

    Initialize_Stream_Packing_Property(cur_stream, maxAlign);

    INT memop_kind = cur_stream->memop_kind;

    // Count the number of streams that would be packed for each
    // possible loop transformation.

    if (STREAM_TRANSFORM(cur_stream, LOOP_TRANSFORM_NONE))
      transform_stream_count[LOOP_TRANSFORM_NONE][memop_kind] ++;

    if (STREAM_TRANSFORM(cur_stream, LOOP_TRANSFORM_UNC_PEEL))
      transform_stream_count[LOOP_TRANSFORM_UNC_PEEL][memop_kind] ++;

    if (STREAM_TRANSFORM(cur_stream, LOOP_TRANSFORM_COND_PEEL)) {
      // Split the streams on which conditional peeling can be
      // applied, from the static streams.
      if (cur_stream->align_kind & STREAM_COND_PEEL_ALIGNED) {
	transform_stream_count[LOOP_TRANSFORM_COND_PEEL][memop_kind] ++;
	cond_peel_count ++;
      }
      else
	static_stream_for_cond_peel_count[memop_kind] ++;
    }

    if (STREAM_TRANSFORM(cur_stream, LOOP_TRANSFORM_SPECIALIZE) &&
	(cur_stream->align_kind & STREAM_SPECIAL_ALIGNED))
      // Count only the streams that would be specialized, do not
      // count the static streams
      transform_stream_count[LOOP_TRANSFORM_SPECIALIZE][memop_kind] ++;

    stream_count ++;
    if (memop_kind == MEMOP_LOAD)
      load_stream_count ++;
    else
      store_stream_count ++;
    memop_count += cur_stream->memop_count;
  }

  if (trace_packing_verbose)
    if (stream_count == 0)      
      fprintf(stdout, "  <packing check> no stream found\n");
  else
    fprintf(stdout, "  <packing check> found %d operation(s) in %d stream(s), (%d load streams, %d store streams)\n",
	    memop_count, stream_count, load_stream_count, store_stream_count);

  // Check there is at least one stream candidate for packing
  if (stream_count == 0)
    return FALSE;

  INT transform_gain[LOOP_TRANSFORM_LAST][MEMOP_LAST];

  // Now, compute the expected gain for each loop transformation
  for (int memop_kind = MEMOP_LOAD; memop_kind < MEMOP_LAST; memop_kind ++) {
    for (int trans_kind = 0; trans_kind < LOOP_TRANSFORM_LAST; trans_kind ++) {

      if (trans_kind == LOOP_TRANSFORM_COND_PEEL) {

	// If there is more than one stream candidate for packing, set which
	// ones to choose and set the gain accordingly.
	INT peel_streams;
	peel_streams = Cond_Peel_Streams(loop_ivs, first_stream, cond_peel_count,
					 maxAlign, memop_kind);
	if (peel_streams > 0)
	  transform_gain[trans_kind][memop_kind] = peel_streams + static_stream_for_cond_peel_count[memop_kind];
	else
	  transform_gain[trans_kind][memop_kind] = 0;
      }

      else if (trans_kind == LOOP_TRANSFORM_SPECIALIZE) {
	INT special_streams = transform_stream_count[trans_kind][memop_kind];
	if (special_streams > MAX_SPECIAL_STREAM) special_streams = MAX_SPECIAL_STREAM;
	// Divide by two since the specialized loop may not be executed.
	// TBD: Check if specialization maybe activated for just one stream. 
	transform_gain[trans_kind][memop_kind] = (special_streams +
						  transform_stream_count[LOOP_TRANSFORM_NONE][memop_kind]/* + 1*/)/2;
      }
      
      else
	transform_gain[trans_kind][memop_kind] = transform_stream_count[trans_kind][memop_kind];
    }
  }
  
  // Check which loop transformation will provide the maximum gain.
  // Store streams will only be considered if there is no gain on load
  // streams.

  // Chek if the loop iterates enough for transformations such as
  // peeling or specialization to be profitable.
  ANNOTATION *annot = ANNOT_Get(BB_annotations(loop_ivs->Head()), ANNOT_LOOPINFO);
  LOOPINFO *info = ANNOT_loopinfo(annot);
  TN *trip_count_tn = LOOPINFO_exact_trip_count_tn(info);
  INT loop_iter = -1;

  if (trip_count_tn != NULL && TN_has_value(trip_count_tn))
    loop_iter = TN_value(trip_count_tn);
  else if (LOOPINFO_wn(info))
    loop_iter = WN_loop_trip_est(LOOPINFO_wn(info));

  INT do_transform = LOOP_TRANSFORM_NONE;
  INT gain_transform[MEMOP_LAST];

  for (int memop_kind = MEMOP_LOAD; memop_kind < MEMOP_LAST; memop_kind ++) {
    gain_transform[memop_kind] = 0;
    for (int trans_kind = LOOP_TRANSFORM_NONE; trans_kind < LOOP_TRANSFORM_LAST; trans_kind ++) {

      // No gain at doing this transformation
      if (transform_gain[trans_kind][memop_kind] <= gain_transform[memop_kind])
	continue;

      // Check if loop peeling is allowed on this stream
      if (trans_kind == LOOP_TRANSFORM_COND_PEEL) {

	// Conditional loop peeling can be performed on one stream
	// only
	if (do_transform == LOOP_TRANSFORM_COND_PEEL)
	  continue;

	if ((Loop_packing_flags & PACKING_LOOP_PEELING) == 0) {
	  if (trace_packing_verbose)
	    fprintf(stdout, "  <packing check> Packing requires loop peeling. Not enabled on this loop.\n");
	  continue;
	}

	if (((Loop_packing_flags & PACKING_LOOP_PEELING_CONST_TRIP_COUNT) == 0) &&
	    (trip_count_tn != NULL) &&
	    TN_has_value(trip_count_tn) &&
	    (TN_value(trip_count_tn) < 64)) {
	  if (trace_packing_verbose)
	    fprintf(stdout, "  <packing check> Packing requires loop peeling on constant trip count. Not enabled on this loop.\n");
	  continue;
	}

	if ((loop_iter != -1) && (loop_iter <= 16)) {
	  if (Get_Trace(TP_CGLOOP, 0x10))
	    fprintf(TFile, "<ivs packing> Peeling cannot be applied on loops with less or equal to 16 iterations.\n");
	  continue;
	}
      }

      else if (trans_kind == LOOP_TRANSFORM_SPECIALIZE) {

	if ((Loop_packing_flags & PACKING_LOOP_SPECIALIZATION) == 0) {
	  if (trace_packing_verbose)
	    fprintf(stdout, "  <packing check> Packing requires loop specialization. Not enabled on this loop.\n");
	  continue;
	}

	if ((loop_iter != -1) && (loop_iter <= 16)) {
	  if (Get_Trace(TP_CGLOOP, 0x10))
	    fprintf(TFile, "<ivs packing> Specialization cannot be applied on loops with less or equal to 16 iterations.\n");
	  continue;
	}
	// TBD: Check if this is really beneficial
	// Specialization is only activated when it is the only way to get packing.
	else
	  if (gain_transform[memop_kind] > 0)
	    continue;
      }

      // This transformation is better for this memop_kind. Check now
      // that for previous memop_kind it does not degrades the gain.
      if (memop_kind > 0) {
	// TBD: Check if this is really beneficial
	// Do not enable specialization if it was not necessary for previous memop_kind.
	if (trans_kind == LOOP_TRANSFORM_SPECIALIZE)
	  continue;
	BOOL is_better = TRUE;
	for (INT memop_idx = memop_kind-1; memop_idx >= 0; memop_idx --)
	  if (transform_gain[trans_kind][memop_idx] < gain_transform[memop_idx]) {
	    is_better = FALSE;
	    break;
	  }
	if (!is_better)
	  continue;
      }

      // This transformation is allowed and is better than previous
      // ones
      do_transform = trans_kind;
      gain_transform[memop_kind] = transform_gain[trans_kind][memop_kind];
    }
  }

  // If conditional peeling or specialization is required, register these
  // streams.
  switch (do_transform) {

  case LOOP_TRANSFORM_UNC_PEEL:
    cg_loop.Set_peel_loop();
    cg_loop.Set_remainder_after();
    break;

  case LOOP_TRANSFORM_COND_PEEL:
    {
      MemoryStream_t *peel_stream = NULL;
      for (cur_stream = first_stream; cur_stream != NULL; cur_stream = cur_stream->next) {
	if ((cur_stream->align_kind & STREAM_COND_PEEL_ALIGNED) == 0)
	  continue;
	  peel_stream = cur_stream;
	  break;
      }

      Is_True(peel_stream != NULL, ("peel_stream should not be null here"));
      // Mark the loop for peeling
      cg_loop.Set_peel_loop();
      // Use the IV TN instead of the base_tn, since base_tn may not be
      // available.
      cg_loop.Push_stream(peel_stream->IV_tn, maxAlign, peel_stream->align_offset%maxAlign);
      cg_loop.Set_remainder_after();

      // In this case, it is also necessary to insert ASSUME on loop
      // pre-header, after the peeled body, to assert the alignment of
      // the streams with ALIGNED && UNC_PEEL property. To do this, we
      // push static streams, so that ASSUME directives can be added.
      INT static_streams = static_stream_for_cond_peel_count[MEMOP_LOAD] +
	static_stream_for_cond_peel_count[MEMOP_STORE];
      if (static_streams > 0) {
	INT stream_idx = 1;
	for (cur_stream = first_stream; cur_stream != NULL; cur_stream = cur_stream->next) {
	  if ((cur_stream->align_kind & STREAM_COND_PEEL_ALIGNED) ||
	      !STREAM_TRANSFORM(cur_stream, LOOP_TRANSFORM_COND_PEEL))
	    continue;

	  stream_idx ++;

	  // There is too much static streams, cannot set ASSUME on these other streams
	  if (stream_idx > MAX_SPECIAL_STREAM) {
	    RESET_STREAM_TRANSFORM(peel_stream, LOOP_TRANSFORM_COND_PEEL);
	    continue;
	  }

	  // This stream, or the second load op in the stream, is well
	  // aligned with and without loop peeling
	  Is_True(((cur_stream->align_kind & STREAM_ALIGNED) &&
		   (cur_stream->align_kind & STREAM_UNC_PEEL_ALIGNED)) ||
		  ((cur_stream->align_kind & STREAM_NEXT_ALIGNED) &&
		   (cur_stream->align_kind & STREAM_UNC_PEEL_NEXT_ALIGNED)), 
		  ("Unexpected stream"));
	  // Use the IV TN instead of the base_tn, since base_tn may
	  // not be available.
	  cg_loop.Push_stream(cur_stream->IV_tn, maxAlign, cur_stream->align_offset%maxAlign);
	}

	Is_True(stream_idx == (static_streams+1), ("Inconsistency in the number of static streams"));
	if (trace_packing_verbose) {
	  if (stream_idx  > MAX_SPECIAL_STREAM)
	    fprintf(stdout, "  <packing check> WARNING: ASSUME not insereted for %d static streams.\n",
		    stream_idx - MAX_SPECIAL_STREAM);
	}
      }
    }
    break;

  case LOOP_TRANSFORM_SPECIALIZE:
    {
      INT special_stream_count = (transform_stream_count[LOOP_TRANSFORM_SPECIALIZE][MEMOP_LOAD] +
				  transform_stream_count[LOOP_TRANSFORM_SPECIALIZE][MEMOP_STORE]);
      Is_True(special_stream_count > 0, ("Inconsistency in number of streams to be specialized."));
      cg_loop.Set_specialize_loop();
      // Sort the streams candidate for specialization according to memop_count
      if (special_stream_count <= MAX_SPECIAL_STREAM) {
	for (cur_stream = first_stream; cur_stream != NULL; cur_stream = cur_stream->next) {
	  if (cur_stream->align_kind & STREAM_SPECIAL_ALIGNED)
	    cg_loop.Push_stream(cur_stream->IV_tn, maxAlign, cur_stream->align_offset%maxAlign);
	}
      }
      else {
	if (Get_Trace(TP_CGLOOP, 0x10))
	  fprintf(TFile, "<ivs packing> Specialization cannot handle more than %d(/%d) streams for specialization.\n", MAX_SPECIAL_STREAM, special_stream_count);
	if (trace_packing_verbose)
	  fprintf(stdout, "  <packing check> Specialization applied only on %d(/%d) streams.\n",
		  MAX_SPECIAL_STREAM, special_stream_count);
	// There are more streams to specialize than the maximum number
	// that can be specialized. Psuh only the most profitable ones.
	MemoryStream_t **special_streams;
	special_streams = TYPE_MEM_POOL_ALLOC_N(MemoryStream_t*, 
						loop_ivs->Mem_pool(), 
						special_stream_count);
	INT special_stream_idx = 0;
	for (cur_stream = first_stream; cur_stream != NULL; cur_stream = cur_stream->next) {
	  if (cur_stream->align_kind & STREAM_SPECIAL_ALIGNED)
	    special_streams[special_stream_idx++] = cur_stream;
	}
	qsort(special_streams, special_stream_count, sizeof(MemoryStream_t *), Compare_Special_Streams);

	for (special_stream_idx = 0; special_stream_idx < MAX_SPECIAL_STREAM; special_stream_idx ++) {
	  cg_loop.Push_stream(special_streams[special_stream_idx]->IV_tn,
			      maxAlign, special_streams[special_stream_idx]->align_offset%maxAlign);
	}
	for (; special_stream_idx < special_stream_count; special_stream_idx ++)
	  RESET_STREAM_TRANSFORM(special_streams[special_stream_idx], LOOP_TRANSFORM_SPECIALIZE);
      }

      cg_loop.Set_remainder_after();

      if (Get_Trace(TP_CGLOOP, 0x10))
	fprintf(TFile, "<ivs packing> Loop can be specialized\n");
    }

    break;

  case LOOP_TRANSFORM_NONE:
    // If there are streams that can be packed, set the remainder after.
    {
      INT static_streams_count = 0;
      static_streams_count += transform_stream_count[LOOP_TRANSFORM_NONE][MEMOP_LOAD];
      static_streams_count += transform_stream_count[LOOP_TRANSFORM_NONE][MEMOP_STORE];
      if (static_streams_count > 0) {
	cg_loop.Set_remainder_after();
      }
    }
    break;
  }

  // Compute the unrolling factor
  INT packed_stream_count[MEMOP_LAST] = {0,0};
  INT cond_peel_stream_count = 0;
  INT unroll_factor = 1;
  for (cur_stream = first_stream; cur_stream != NULL; cur_stream = cur_stream->next) {
    if (STREAM_TRANSFORM(cur_stream, do_transform)) {
      packed_stream_count[cur_stream->memop_kind] ++;
      if (cur_stream->align_kind & STREAM_COND_PEEL_ALIGNED) cond_peel_stream_count ++;
      if ((unroll_factor < 4) && (cur_stream->size%(maxAlign/2) != 0))
	unroll_factor = 4;
      else if ((unroll_factor < 2) && (cur_stream->size%maxAlign != 0))
	unroll_factor = 2;
    }
  }
  cg_loop.Set_packing_factor(unroll_factor);

  INT packed_streams = packed_stream_count[MEMOP_LOAD] + packed_stream_count[MEMOP_STORE];
  
  if (trace_packing_verbose) {
    if (cg_loop.Peel_loop())
      if (cg_loop.Cond_peel_loop())
	fprintf(stdout, "  <packing check> Requires conditional loop peeling for %d stream(s)\n", cond_peel_stream_count);
      else
	fprintf(stdout, "  <packing check> Requires static loop peeling for %d stream(s)\n", packed_streams);
    else if (cg_loop.Specialize_loop()) {
      fprintf(stdout, "  <packing check> Requires specialization for %d stream(s)\n", packed_streams);
    }
    else if (packed_streams > 0)
      fprintf(stdout, "  <packing check> Packing could be performed on %d stream(s), (%d load streams, %d store streams)\n", packed_streams, packed_stream_count[MEMOP_LOAD], packed_stream_count[MEMOP_STORE]);
    else
      fprintf(stdout, "  <packing check> No stream is candidate for packing\n");
    if (cg_loop.Remainder_after())
      fprintf(stdout, "  <packing check> Requires remainder after unrolled loop\n");
    if (cg_loop.Packing_factor() > 1)
      fprintf(stdout, "  <packing check> Requires unroll_times%%%d == 0\n", cg_loop.Packing_factor());
  }

  cg_loop.Set_streams(packed_stream_count[MEMOP_LOAD], packed_stream_count[MEMOP_STORE]);

  if (trace_packing_verbose)
    if (stream_count == 0)      
      fprintf(stdout, "  <packing check> no stream found\n");
  else
    fprintf(stdout, "  <packing check> found %d operation(s) in %d stream(s), (%d load streams, %d store streams)\n",
	    memop_count, stream_count, load_stream_count, store_stream_count);

  // Check if there is at least one stream candidate for packing
  if (packed_streams == 0)
    return FALSE;

  if (Get_Trace(TP_CGLOOP, 0x10)) {
    if (cg_loop.Packing_factor() > 1) {
      fprintf(TFile, "<ivs packing> Loop requires unroll_factor%d == 0\n", cg_loop.Packing_factor());
    }
    fprintf(TFile, "<ivs packing> before unrolling: Load stream %d, Store stream %d, Peeling %d, Special %d\n", load_stream_count, store_stream_count, cg_loop.Cond_peel_loop(), packed_streams);
  }

  return TRUE;
}

static BOOL
LoadStore_Packing_Discard_Peeling_Specialization(CG_LOOP &cg_loop, BB *prolog, BB *epilog) {

  // In case of loop peeling or specialization, discard the
  // additional code. Only the effect of remainder_after will be
  // left.

  if (cg_loop.Peel_loop()) {
    cg_loop.Undo_Peel_Loop(prolog);
    return TRUE;
  }

  else if (cg_loop.Specialize_loop()) {
    cg_loop.Undo_Specialize_Loop(prolog, epilog);
    return TRUE;
  }

  return FALSE;
}

BOOL IVS_Perform_Load_Store_Packing( CG_LOOP &cg_loop )
{
  int allSizes = CG_LOOP_Packing_sizes;
  
  extern BOOL Enable_64_Bits_Ops;
  if (Enable_64_Bits_Ops)
    allSizes &= 0xF;
  else
    allSizes &= 0x7;

  if (allSizes == 0)
    return FALSE;

  LOOP_DESCR *loop = cg_loop.Loop();

  // Initialize memory pool for LOOP_IVS
  MEM_POOL local_mem_pool;
  MEM_POOL_Initialize( &local_mem_pool, "LOOP_IVS local pool", FALSE );
  MEM_POOL_Push( &local_mem_pool );

  BOOL changed_loop = FALSE;
  {
    // Invoke Read/CSE/Write Removal
    BB *head = LOOP_DESCR_loophead( loop );

    Is_True(CG_LOOP_prolog == BB_Other_Predecessor(head, head), ("Illegal CG_LOOP_prolog"));
    Is_True(CG_LOOP_epilog == BB_Other_Successor(head, head), ("Illegal CG_LOOP_epilog"));
    //    Is_True(CG_LOOP_prolog == cg_loop.Prolog_end(), ("Illegal CG_LOOP_prolog"));
    //    Is_True(CG_LOOP_epilog == cg_loop.Epilog_start(), ("Illegal CG_LOOP_epilog"));


    BOOL save_CG_DEP_Addr_Analysis = CG_DEP_Addr_Analysis;
    CG_DEP_Addr_Analysis = FALSE;

    // Derive dependence graph for loop body
    CG_DEP_Compute_Graph( head, NO_ASSIGNED_REG_DEPS, CYCLIC,
			  INCLUDE_MEMREAD_ARCS, INCLUDE_MEMIN_ARCS,
			  NO_CONTROL_ARCS, NULL );

    LOOP_IVS loop_ivs( &local_mem_pool );
    changed_loop = LoadStore_Packing( &loop_ivs, cg_loop, allSizes );
    if (!changed_loop) 
      changed_loop = LoadStore_Packing_Discard_Peeling_Specialization(cg_loop, loop_ivs.Prolog(), loop_ivs.Epilog());

    CG_DEP_Addr_Analysis = save_CG_DEP_Addr_Analysis;
    CG_DEP_Delete_Graph( head );
  }

  // Dispose memory pool for LOOP_IVS
  MEM_POOL_Pop( &local_mem_pool );
  MEM_POOL_Delete( &local_mem_pool );

  if ( changed_loop )
    CG_LOOP_Recompute_Liveness( loop );

  return changed_loop;
}

BOOL IVS_Analyze_Load_Store_Packing( CG_LOOP &cg_loop )
{

  int allSizes = CG_LOOP_Packing_sizes;
  
  extern BOOL Enable_64_Bits_Ops;
  if (Enable_64_Bits_Ops)
    allSizes &= 0xF;
  else
    allSizes &= 0x7;

  if (allSizes == 0)
    return FALSE;

  BOOL can_be_packed = FALSE;
  LOOP_DESCR *loop = cg_loop.Loop();

  // Initialize memory pool for LOOP_IVS
  MEM_POOL local_mem_pool;
  MEM_POOL_Initialize( &local_mem_pool, "LOOP_IVS local pool", FALSE );
  MEM_POOL_Push( &local_mem_pool );

  {
    BB *head = LOOP_DESCR_loophead( loop );

    BOOL save_CG_DEP_Addr_Analysis = CG_DEP_Addr_Analysis;
    CG_DEP_Addr_Analysis = FALSE;

    // Derive dependence graph for loop body
    CG_DEP_Compute_Graph( head, NO_ASSIGNED_REG_DEPS, CYCLIC,
			  INCLUDE_MEMREAD_ARCS, INCLUDE_MEMIN_ARCS,
			  NO_CONTROL_ARCS, NULL );

    LOOP_IVS loop_ivs( &local_mem_pool );
    can_be_packed = LoadStore_Check_Packing( &loop_ivs, cg_loop, allSizes );

    CG_DEP_Addr_Analysis = save_CG_DEP_Addr_Analysis;
    CG_DEP_Delete_Graph( head );
  }

  // Dispose memory pool for LOOP_IVS
  MEM_POOL_Pop( &local_mem_pool );
  MEM_POOL_Delete( &local_mem_pool );

  return can_be_packed;
}

INT
Loop_Packing_Options( CG_LOOP &cg_loop )
{
  LOOP_DESCR *loop = cg_loop.Loop();
  BB *head = LOOP_DESCR_loophead( loop );

  INT loop_packing_flags = CG_LOOP_Packing_flags;

  // Now, look if some loop pragmas override this value.
  ANNOTATION *packing_ant = ANNOT_Get(BB_annotations(head), ANNOT_PRAGMA);
  while (packing_ant && WN_pragma(ANNOT_pragma(packing_ant)) != WN_PRAGMA_LOOPPACK)
    packing_ant = ANNOT_Get(ANNOT_next(packing_ant), ANNOT_PRAGMA);
  if (packing_ant) {
    WN *wn = ANNOT_pragma(packing_ant);
    INT packing_level = WN_pragma_arg1(wn);
    if (Get_Trace(TP_CGLOOP, 0x20)) {
      fprintf(stdout, "  <packing check> #pragma looppack(%d)\n", packing_level);
    }
    // FdF: Same association as in config_target.cxx
    switch (packing_level) {
    case 0:
      loop_packing_flags = 0;
      break;
    case 1:
      loop_packing_flags = 0x203;
      break;
    case 2:
      loop_packing_flags = 0x21b;
      break;
    }
  }

  if (Get_Trace(TP_CGLOOP, 0x20)) {
    fprintf(stdout, "  <packing check> Packing with flags 0x%x\n", loop_packing_flags);
  }

  return loop_packing_flags;
  }

#endif /* !TARG_STxP70 */

#endif
