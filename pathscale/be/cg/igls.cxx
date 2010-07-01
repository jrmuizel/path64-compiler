/*
 *  Copyright 2009 SiCortex, Inc.
 */

/*
 *  Copyright (C) 2007, 2008, 2009 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


// =======================================================================
// =======================================================================
//
//  Module: igls.cxx
//  $Revision: 1.14 $
//  $Date: 04/12/21 14:57:07-08:00 $
//  $Author: bos@eng-25.internal.keyresearch.com $
//  $Source: /home/bos/bk/kpro64-pending/be/cg/SCCS/s.igls.cxx $
//
//  Description:
//  ============
//
//  Integrated Global and Local Scheduling Framework (IGLS). The main driver 
//  controls the execution of the local scheduling phase (LOCS), the
//  hyperblock scheduling phase (HBS) and the global scheduling phase (GCM).
//
// =======================================================================
// =======================================================================

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#if !defined(__FreeBSD__)
#include <alloca.h>
#endif
#include <math.h>
#include "defs.h"
#include "config.h"
#include "config_targ_opt.h"
#include "mempool.h"
#include "bb.h"
#include "bb_set.h"
#include "tracing.h"
#include "timing.h"
#include "cgir.h"
#include "glob.h"
#include "tn_map.h"
#include "cg.h"
#include "cg_flags.h"
#include "ercg.h"
#include "cgtarget.h"
#include "cg_vector.h"
#include "dominate.h"
#include "findloops.h"
#include "note.h"
#include "lra.h"
#include "gcm.h"
#include "ti_res.h"
#include "ti_res_res.h"
#include "ti_latency.h"
#include "ti_errors.h"
#include "cg_region.h"
#include "gtn_universe.h"
#include "gtn_set.h"
#include "cxx_memory.h"
#include "hb_sched.h"
#include "hb_hazards.h"
#include "targ_proc_properties.h"

#ifdef KEY
#include "whirl2ops.h"
#ifdef TARG_ST
#include "freq.h"
#include "whirl2ops.h"
#include "pf_cg.h"
#include "config_cache.h"
#include "cg_dep_graph.h"
#endif
#ifdef LAO_ENABLED
#include "lao_stub.h"
#endif
#ifdef TARG_ST

/* FdF: Find a prefetch instruction in the current basic block which
   is a duplication of op_pref for the iteration number iter. */
static OP *
Get_unrolled_op(OP *op, LOOP_DESCR *cloop, int iter) {
  if (op) {
    WN *wn = Get_WN_From_Memory_OP(op);
    BB *bb;
    FOR_ALL_BB_SET_members (LOOP_DESCR_bbset(cloop), bb) {
      OP *op_next;
      FOR_ALL_BB_OPs(bb, op_next) {
	if (OP_memory(op_next) && OP_code(op_next) == OP_code(op) &&
	    (Get_WN_From_Memory_OP(op_next) == wn) && !OP_flag1(op_next) && (OP_unrolling(op_next) == iter))
	  return op_next;
      }
    }
  }
  return NULL;
}

static OP *
Next_unrolled_op(OP *op, int iter) {
  if (op) {
    WN *wn = Get_WN_From_Memory_OP(op);
    for (OP *op_next = OP_next(op); op_next; op_next = OP_next(op_next)) {
      if (OP_memory(op_next) && OP_code(op_next) == OP_code(op) &&
	  (Get_WN_From_Memory_OP(op_next) == wn) && !OP_flag1(op_next) && (OP_unrolling(op_next) == iter))
	return op_next;
    }
  }
  return NULL;
}

// Returns the number of cycles of a basic block
static int
BB_cycles(BB *bb) {

  // Get the number of cycles in this basic block.
  OP *op = BB_last_op(bb);
  int bb_cycles = op ? OP_scycle(op) : 0;

  if (bb_cycles < 0) {
    // When the last instruction in a basic block is a branch
    // operation, it may not have a valid scheduling date. In this
    // case just use the previous one.
    Is_True (OP_xfer(op), ("bb: %d, None branch instruction have no scheduling date", BB_id(bb)));

    // Assumes that prev_op is scheduled in the same cycle as op
    bb_cycles = OP_prev(op) ? OP_scycle(OP_prev(op)) : 0;
  }

  Is_True(bb_cycles >= 0, ("No valid scheduling info for bb %d\n", BB_id(bb)));

  return bb_cycles+1;
}

/* FdF: Compute the prefetch distance of PFT instructions, now that
   the number of cycles of loops is known. */

void
Schedule_Prefetch_Prepass () {
  WN_MAP WN_to_OP_map;
  BOOL Trace_PFT = Get_Trace(TP_SCHED, 4);

  if (!(Prefetch_Optimize&0x2)) return;

  Calculate_Dominators ();
  L_Save ();
  LOOP_DESCR *loop_list = LOOP_DESCR_Detect_Loops (&MEM_local_pool);
  for (LOOP_DESCR *cloop = loop_list;
       cloop != NULL;
       cloop = LOOP_DESCR_next(cloop)) {

    // Do not shedule prefetch instructions when #pragma LOOPSEQ read is
    // used.
    // FdF 20050222: Imported from cg_dep_graph.cxx
    extern UINT32 CG_Get_BB_Loopseq_Mask(BB *bb);
#define LOOPSEQ_READ	1
    BB *loop_head = LOOP_DESCR_loophead(cloop);
    if (CG_Get_BB_Loopseq_Mask(loop_head) & (1<<LOOPSEQ_READ))
      continue;

    WN_to_OP_map = WN_MAP_Create(&MEM_local_pool);

    BB *bb;
    // Compute the number of prefetchs in the loop.
    int nb_prefetch = 0;
    int nb_cycles = CGTARG_Branch_Taken_Penalty(); // Count for the loop back-edge
    BOOL is_inner_loop = TRUE;
    BOOL loop_is_scheduled = TRUE;
    float head_freq = BB_freq(LOOP_DESCR_loophead(cloop));

    /* FdF: Compute the number of prefetchs in the loop, initialize a
       MAP WN_to_OP_map, and compute a mean number of cycles for
       multi-bb loops. */

    FOR_ALL_BB_SET_members (LOOP_DESCR_bbset(cloop), bb) {
      OP *op;

      if (BB_loop_head_bb(bb) != LOOP_DESCR_loophead(cloop)) {
	is_inner_loop = FALSE;
	continue;
      }

      // Cannot perform prefetch scheduling if some block is not
      // scheduled. This is the case when a BB contains save/restore
      // callee save copy instructions since the prepass scheduler do
      // not schedule them.
      if (!BB_scheduled(bb)) {
	loop_is_scheduled = FALSE;
	break;
      }

#define FLOAT_INT_MUL(f,i) ((INT)(f * (float)i + (float)0.5))
      float freq_ratio = head_freq == 0.0 ? 0.0 : BB_freq(bb)/head_freq;
      nb_cycles +=  FLOAT_INT_MUL (freq_ratio, BB_cycles(bb));

      FOR_ALL_BB_OPs(bb, op) {
	if (OP_prefetch(op)) {
	  // FdF 20070627: There may not be a WN associated with a prefetch (codex #26872)
	  if (!Get_WN_From_Memory_OP(op)) {
	    if (Trace_PFT)
	      fprintf(TFile, "No WN associated with a PREFETCH op\n");
	    continue;
	  }
	  nb_prefetch++;

	  // In case of unrolling, there may be more than one prefetch
	  // for the same WN, from different loop iterations. Just
	  // keep the first one, the other ones will be analyzed in a
	  // specific way.
	  if (!WN_MAP_Get(WN_to_OP_map, Get_WN_From_Memory_OP(op))) {
	    WN_MAP_Set(WN_to_OP_map, Get_WN_From_Memory_OP(op), op);
	    Reset_OP_flag1(op);
	  }
	}
      }
    }

    if (!loop_is_scheduled) {
      WN_MAP_Delete(WN_to_OP_map);
      continue;
    }

    if (is_inner_loop && nb_prefetch) {

      if (Trace_PFT)
	fprintf(TFile, "loop %d cycles, %d prefetchs\n", nb_cycles, nb_prefetch);
      // Compute the maximum number of iterations the prefetch
      // instructions must be computed ahead.
      FOR_ALL_BB_SET_members (LOOP_DESCR_bbset(cloop), bb) {
	OP *op_ref;
	FOR_ALL_BB_OPs(bb, op_ref) {

	  if (!OP_load(op_ref))
	    continue;

	  WN *op_wn = Get_WN_From_Memory_OP(op_ref);
	  if (!op_wn) continue;

	  PF_POINTER *pref_info = (PF_POINTER *)WN_MAP_Get (WN_MAP_PREFETCH, op_wn);
	  if (!pref_info && Trace_PFT) fprintf(TFile, "igls: ref %p, no prefetch found\n", op_wn);
	  if (!pref_info) continue;

	  // TODO: Try to fix the warning in opt_htable.cxx:2895
	  if (!PF_PTR_wn_pref_1L(pref_info)) continue;

	  WN *pref_wn = PF_PTR_wn_pref_1L(pref_info);
	  OP *op_pref = NULL;

	  if (Trace_PFT)
	    fprintf(TFile, "igls: ref %p, found a prefetch wn %p\n", op_wn, pref_wn);

	  op_pref = (OP*) WN_MAP_Get(WN_to_OP_map, pref_wn);

#if 0 // Debug Only
	  PF_POINTER *pf_info = (PF_POINTER *)WN_MAP_Get (WN_MAP_PREFETCH, pref_wn);
	  if (pf_info != pref_info) {
	    printf("memop->memwn->prefinfo != pfwn->pfinfo\n");
	    if (!pf_info || (PF_PTR_wn_pref_1L(pf_info) != pref_wn))
	      printf("!!! prefinfo->pref1L != pfinfo->pref1L\n");
	  }
#endif
	  // Look for the first prefetch operation in this basic block
	  // with the same OP_unrolling
	  op_pref = Get_unrolled_op(op_pref, cloop, OP_unrolling(op_ref));
	  if (!op_pref) continue;
	  if (OP_flag1(op_pref)) continue; // Already visited

	  // The prefetch instruction is moved before the first memory
	  // operation that is associated to it in the following
	  // cases:

	  // - The scheduling date of the memory reference is greater
	  // than the prefetch latency. The prefetch can occur in the
	  // same iteration as the memory reference, if placed at the
	  // beginning of the loop.

	  // - The loop trip count and/or the number of cycle of the
	  // loops is so small that any prefetch in the loop will not
	  // prefetch any useful data for this loop.

	  // Otherwise, the prefetch instruction is moved after the
	  // first memory reference that uses it, and prefetch
	  // instructions are inserted in the loop entry.

	  MHD_LEVEL* Cur_Mhd = &Mhd.L[0]; 
	  FmtAssert(Cur_Mhd->Valid(), ("Not a valid MHD level"));

	  Set_OP_flag1(op_pref);
	  Set_OP_pft_scheduled(op_pref);

	  int pf_latency = (int)Cur_Mhd->Clean_Miss_Penalty;
	  if ((OP_bb(op_pref) == bb) && (OP_scycle(op_ref) > pf_latency)) {
	    // Prefetched can be scheduled in the same iteration as
	    // the memory reference.
	    if (OP_Ordering(op_ref, op_pref) < 0) {
	      BB_Remove_Op(bb, op_pref);
	      BB_Insert_Op_Before(bb, op_ref, op_pref);
	      Reset_BB_scheduled (bb);
	    }
	    Set_OP_pft_before(op_pref);
	  }
	  else {
	    // Move the prefetch instruction after the memory
	    // reference associated with it.
	    if ((OP_bb(op_pref) != bb) || (OP_Ordering(op_ref, op_pref) > 0)) {
	      // FdF 20070322: Codex #25337, in case the prefetch
	      // instruction is moved in a different BB, all of its
	      // TNs must be marked global
	      if (OP_bb(op_pref) != bb) {
		for (INT opnd = 0; opnd < OP_opnds(op_pref); opnd++) {
		  if (TN_is_register(OP_opnd(op_pref, opnd)))
		    Set_TN_is_global_reg(OP_opnd(op_pref, opnd));
		}
	      }
	      BB_Remove_Op(OP_bb(op_pref), op_pref);
	      BB_Insert_Op_After(bb, op_ref, op_pref);
	      Reset_BB_scheduled (bb);
	      Reset_BB_scheduled (OP_bb(op_pref));
	    }

	    //	    if (Trace_PFT)
	    //	      fprintf(TFile, "Insert %d prefetch at loop entry(pf_stride = %f)\n", WN_pf_stride_1L(pref_wn) ? (int)(iterAhead/pf_stride + 0.99) : 1, pf_stride);
	    // TBD: Insert iterAhead/pf_stride prefetch instructions
	    // before the loop. First, look for a place where to
	    // insert the prefetch instruction. Go up in the dominator
	    // tree.
	  }
	}
      }
    }
    WN_MAP_Delete(WN_to_OP_map);
  }
  L_Free ();
  Free_Dominators_Memory ();
}

/* FdF: Compute the prefetch distance of PFT instructions, now that
   the number of cycles of loops is known. */
void
Schedule_Prefetch_Postpass () {
  WN_MAP WN_to_OP_map;
  BOOL Trace_PFT = Get_Trace(TP_SCHED, 4);

  if (!(Prefetch_Optimize&0x2)) return;

  Calculate_Dominators ();
  L_Save ();
  LOOP_DESCR *loop_list = LOOP_DESCR_Detect_Loops (&MEM_local_pool);
  for (LOOP_DESCR *cloop = loop_list;
       cloop != NULL;
       cloop = LOOP_DESCR_next(cloop)) {

    WN_to_OP_map = WN_MAP_Create(&MEM_local_pool);

    BB *bb;
    // Compute the number of prefetchs in the loop.
    int nb_cycles = CGTARG_Branch_Taken_Penalty(); // Count for the loop back-edge
    BOOL is_inner_loop = TRUE;
    BOOL loop_is_scheduled = TRUE;
    float head_freq = BB_freq(LOOP_DESCR_loophead(cloop));
    int auto_prefetch = 0;
    int user_prefetch = 0;

    /* FdF: Compute the number of prefetchs in the loop, initialize a
       MAP WN_to_OP_map, and compute a mean number of cycles for
       multi-bb loops. */

    FOR_ALL_BB_SET_members (LOOP_DESCR_bbset(cloop), bb) {
      OP *op;

      if (BB_loop_head_bb(bb) != LOOP_DESCR_loophead(cloop)) {
	is_inner_loop = FALSE;
	continue;
      }

      if (!BB_scheduled(bb)) {
	loop_is_scheduled = FALSE;
	break;
      }

#define FLOAT_INT_MUL(f,i) ((INT)(f * (float)i + (float)0.5))
      float freq_ratio = head_freq == 0.0 ? 0.0 : BB_freq(bb)/head_freq;
      nb_cycles +=  FLOAT_INT_MUL (freq_ratio, BB_cycles(bb));

      FOR_ALL_BB_OPs(bb, op) {
	if (OP_prefetch(op)) {

	  WN *op_wn = Get_WN_From_Memory_OP(op);
	  if (op_wn == NULL) continue;

	  if (WN_pf_manual(op_wn))
	    user_prefetch++;
	  else
	    auto_prefetch ++;

	  // In case of unrolling, there may be more than one prefetch
	  // for the same WN, from different loop iterations. Just
	  // keep the first one, the other ones will be analyzed in a
	  // specific way.
	  Reset_OP_flag1(op);
	  if (!WN_MAP_Get(WN_to_OP_map, Get_WN_From_Memory_OP(op))) {
	    WN_MAP_Set(WN_to_OP_map, Get_WN_From_Memory_OP(op), op);
	  }
	}
      }
    }

    if (!loop_is_scheduled) {
      WN_MAP_Delete(WN_to_OP_map);
      continue;
    }

    int nb_prefetch = user_prefetch + auto_prefetch;
    int auto_prefetched = 0;
    int user_prefetched = 0;

    if ((is_inner_loop  && (nb_prefetch > 0)) ||
	(!is_inner_loop && (auto_prefetch > 0))) {

      if (Trace_PFT)
	fprintf(TFile, "loop %d cycles, %d prefetchs\n", nb_cycles, nb_prefetch);
      // Compute the maximum number of iterations the prefetch
      // instructions must be computed ahead.
      FOR_ALL_BB_SET_members (LOOP_DESCR_bbset(cloop), bb) {
	OP *op_ref;
	FOR_ALL_BB_OPs(bb, op_ref) {

	  //	  if (!OP_load(op_ref) || OP_unrolling(op_ref))
	  if (!OP_load(op_ref))
	    continue;

	  WN *op_wn = Get_WN_From_Memory_OP(op_ref);
	  if (!op_wn) continue;

	  PF_POINTER *pref_info = (PF_POINTER *)WN_MAP_Get (WN_MAP_PREFETCH, op_wn);
	  if (!pref_info && Trace_PFT) fprintf(TFile, "igls: ref %p, no prefetch found\n", op_wn);
	  if (!pref_info) continue;

	  // TODO: Try to fix the warning in opt_htable.cxx:2879
	  if (!PF_PTR_wn_pref_1L(pref_info)) continue;

	  Is_True (PF_PTR_wn_pref_1L(pref_info), ("wn_pref_1L is not set"));
	  WN *pref_wn = PF_PTR_wn_pref_1L(pref_info);
	  OP *op_pref = NULL;

	  if (Trace_PFT)
	    fprintf(TFile, "igls: ref %p, found a prefetch wn %p\n", op_wn, pref_wn);

	  op_pref = (OP*) WN_MAP_Get(WN_to_OP_map, pref_wn);
	  op_pref = Get_unrolled_op(op_pref, cloop, OP_unrolling(op_ref));
	  if (!op_pref) continue;

	  if (OP_flag1(op_pref)) continue;

	  int distAhead;
	  int iterAhead = -1;
	  float pf_stride = -1;

          if (!WN_pf_stride(pref_wn)) {
	    distAhead = PF_PTR_distance_1L(pref_info);
	    if (distAhead < 0) distAhead *= -1;
	  }

	  else {

	    MHD_LEVEL* Cur_Mhd = &Mhd.L[0]; 
	    FmtAssert(Cur_Mhd->Valid(), ("Not a valid MHD level"));

	    int pf_latency = (int)Cur_Mhd->Clean_Miss_Penalty;
	    if (OP_bb(op_pref) == bb)
	      pf_latency += OP_scycle(op_pref) - OP_scycle(op_ref);

	    pf_stride = (float)WN_pf_stride_1L(pref_wn) / (float)(BB_unrollings(bb) ? BB_unrollings(bb) : 1);
	    iterAhead = (int)((pf_latency + nb_cycles - 1) / nb_cycles);

	    // Do not prefetch too much iterations ahead if the loop
	    // trip count is known to be small.
	    if (LOOP_DESCR_loopinfo(cloop) != NULL) {
	      LOOPINFO *info = LOOP_DESCR_loopinfo(cloop);
	      INT32 max_iter;

#ifdef TARG_ST
		TN *trip_count_tn = LOOPINFO_exact_trip_count_tn(info);
#else
		TN *trip_count_tn = LOOPINFO_trip_count_tn(info);
#endif
	      if (trip_count_tn && TN_is_constant(trip_count_tn))
		max_iter = TN_value(trip_count_tn) / 2;
	      else
		max_iter = WN_loop_trip_est(LOOPINFO_wn(info)) / 2;
	      if (iterAhead > max_iter)
		iterAhead = max_iter;
	    }

	    // Do not prefetch too much iterations ahead if this will
	    // exceed the number of prefetch buffers.
	    if ((int)(iterAhead / pf_stride + 0.99) * nb_prefetch > Mhd.DCache_Prefetch_Buffers)
	      iterAhead = (int)(Mhd.DCache_Prefetch_Buffers * pf_stride / nb_prefetch);

	    if (iterAhead > Mhd.DCache_Prefetch_Buffers)
	      iterAhead = Mhd.DCache_Prefetch_Buffers;

	    distAhead = iterAhead * (int)(Cur_Mhd->Line_Size / pf_stride + 0.99);
	    if (distAhead < Cur_Mhd->Line_Size) distAhead = Cur_Mhd->Line_Size;
//          else if (ANNOT_Get(BB_annotations(OP_bb(op_pref)), ANNOT_REMAINDERINFO))
//	    distAhead = Cur_Mhd->Line_Size;

	  /* FdF 20061211: In order to avoid conflicts between
	     prefetch instructions and DMA accesses, a compiler option
	     may limit the distance between a load operation and the
	     associated prefetch instruction. */
	  }

	  if (Trace_PFT)
	    fprintf(TFile, "Prefetch distance is %d\n", distAhead);

	  if ((Mhd.Prefetch_Padding >= 0) && (distAhead > Mhd.Prefetch_Padding)) {
	    distAhead = Mhd.Prefetch_Padding;
	    if (Trace_PFT)
	      fprintf(TFile, "\tchanged to LNO:prefetch_padding(%d)\n", distAhead);
	  }

	  if (PF_PTR_distance_1L(pref_info) < 0) distAhead *= -1;

	  do {
	    Set_OP_flag1(op_pref);
	      
	    if (WN_pf_manual(pref_wn))
	      user_prefetched ++;
	    else
	      auto_prefetched ++;
	      

	    INT pft_offset_idx = TOP_Find_Operand_Use(OP_code(op_pref), OU_offset);;

	    if (WN_pf_stride_1L(pref_wn)/* || ANNOT_Get(BB_annotations(OP_bb(op_pref)), ANNOT_REMAINDERINFO)*/) {
	      int offset = TN_value(OP_opnd(op_pref, pft_offset_idx));
	      int fixedOffset = TN_value(OP_opnd(op_pref, pft_offset_idx)) + distAhead - PF_PTR_distance_1L(pref_info);
	      if (Trace_PFT) {
		fprintf(TFile, "Prefetch offset is %d\n", fixedOffset);
		if (WN_pf_manual(pref_wn) )
		  if (fixedOffset != offset)
		    fprintf(TFile, "Manual prefetch: Offset was %d\n", offset);
		  else
		    fprintf(TFile, "Manual prefetch: Offset unchanged\n");
	      }
	      Set_OP_opnd(op_pref, pft_offset_idx, Gen_Literal_TN(fixedOffset, 4));
	      TOP etop;
	      if (CGTARG_need_extended_Opcode(op_pref, &etop)) {
		// FdF 20061211: TBD: change the opcode to etop ??
		// Reset to previous value.
		Set_OP_opnd(op_pref, pft_offset_idx, Gen_Literal_TN(offset, 4));
		if (CGTARG_need_extended_Opcode(op_pref, &etop))
		  // Can use the new value if olf value also required
		  // extended offset
		  Set_OP_opnd(op_pref, pft_offset_idx, Gen_Literal_TN(fixedOffset, 4));
	      }
	      if (Trace_PFT)
		fprintf(TFile, "Loop cycles %d, iterAhead %d, pft stride %f, distAhead %d\n", nb_cycles, iterAhead, pf_stride, distAhead);
	    }
	    op_pref = Next_unrolled_op(op_pref, OP_unrolling(op_ref));
	  } while (op_pref);
	}
      }
    }

    if ((Mhd.Prefetch_Padding >= 0) && (auto_prefetched < auto_prefetch)) {
      char str_line[64] = "";
      ANNOTATION *annot = ANNOT_Get(BB_annotations(LOOP_DESCR_loophead(cloop)), ANNOT_LOOPINFO);
      if (annot)
	sprintf(str_line, "loop line %d, ", LOOPINFO_line(ANNOT_loopinfo(annot)));
      else {
	int line = Srcpos_To_Line(OP_srcpos(BB_first_op(LOOP_DESCR_loophead(cloop))));
	if (line != 0)
	  sprintf(str_line, "loop line %d, ", line);
	else
	  sprintf(str_line, "loop head BB%d, ", BB_id(LOOP_DESCR_loophead(cloop)));
      }
      DevWarn("%s%d automatic prefetch not mapped, discarded.", str_line, (auto_prefetch - auto_prefetched));
    }

    if ((Mhd.Prefetch_Padding >= 0) && CG_warn_prefetch_padding &&
	(user_prefetched < user_prefetch)) {
      char str_line[64] = "";
      const char *str_inner = is_inner_loop ? "" : "(non innermost) ";
      ANNOTATION *annot = ANNOT_Get(BB_annotations(LOOP_DESCR_loophead(cloop)), ANNOT_LOOPINFO);
      if (annot)
	sprintf(str_line, "loop line %d, ", LOOPINFO_line(ANNOT_loopinfo(annot)));
      else {
	int line = Srcpos_To_Line(OP_srcpos(BB_first_op(LOOP_DESCR_loophead(cloop))));
	if (line != 0)
	  sprintf(str_line, "loop line %d, ", line);
	else
	  sprintf(str_line, "loop head BB%d, ", BB_id(LOOP_DESCR_loophead(cloop)));
      }
      ErrMsg(EC_Warn_Prefetch, str_inner, str_line, (user_prefetch - user_prefetched));

      // Finally, set the OP_flag1 for these manual prefetch instruction
    }

    if ((Mhd.Prefetch_Padding >= 0) &&
	((auto_prefetched < auto_prefetch) ||
	 (CG_warn_prefetch_padding && (user_prefetched < user_prefetch)))) {

      FOR_ALL_BB_SET_members (LOOP_DESCR_bbset(cloop), bb) {
	OP *op, *op_next;

	if (BB_loop_head_bb(bb) != LOOP_DESCR_loophead(cloop))
	  continue;

	for (op = BB_first_op(bb); op != NULL; op = op_next) {
	  op_next = OP_next(op);

	  if (OP_prefetch(op)) {
	    WN *op_wn = Get_WN_From_Memory_OP(op);
	    if (op_wn == NULL) continue;

	    if (!WN_pf_manual(op_wn) && !OP_flag1(op))
	      BB_Remove_Op(bb, op);
	    else {
	      Set_OP_flag1(op);
	    }
	  }
	}
      }
    }

    WN_MAP_Delete(WN_to_OP_map);
  }
  L_Free ();
  Free_Dominators_Memory ();

  // Finally, in case prefetch_padding is activated, look in the whole
  // function for prefetch instructions that have not been processed.

  if (Mhd.Prefetch_Padding >= 0) {
    BB *bb;
    for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {

      OP *op_next;
      for (OP *op = BB_first_op(bb); op != NULL; op = op_next) {
	op_next = OP_next(op);

	if (!OP_prefetch(op) || OP_flag1(op))
	  continue;

	// This is an unvisited prefetch instruction
	WN *op_wn = Get_WN_From_Memory_OP(op);
	if (op_wn == NULL) continue;

	if (WN_pf_manual(op_wn)) {
	  if (CG_warn_prefetch_padding) {
	    char str_line[64] = "";
	    const char *str_inner = "";
	    sprintf(str_line, "line %d(outside loop), ", Srcpos_To_Line(OP_srcpos(op)));
	    ErrMsg(EC_Warn_Prefetch, str_inner, str_line, 1);
	  }
	}
	else {
	  Is_True(0, ("Automatic prefetch found outside of a loop"));
	  BB_Remove_Op(bb, op);
	}
      }
    }
  }  
}
#endif

// Delete prefetches that are dropped by the scheduler.
static void
Delete_Unscheduled_Prefetches (BB *bb)
{
  OP *op, *next_op;
  int offset = -1;

  for (op = BB_first_op(bb); op != NULL; op = next_op) {
    next_op = OP_next(op);
    if (OP_prefetch_deleted(op)) {
      FmtAssert(OP_prefetch(op),
		("Delete_Unscheduled_Prefetches: OP not a prefetch"));
      BB_Remove_Op(bb, op);
    } else {
      // The new cycle 0 is at the first non-deleted instruction.
      if (offset == -1) {
	offset = OP_scycle(op);
      }

      // Adjust scycle to account for the deleted cycles.
      OP_scycle(op) = OP_scycle(op) - offset;
    }
  }

  // Note the new schedule length.
  if (Assembly)
    Add_Scheduling_Note(bb, (void*) 0);
}

// Fabricate a BB that looks like OLD_BB for the purpose of scheduling.
static BB *
Gen_BB_For_Sched (BB *old_bb)
{
  OP *op;
  OPS new_ops;
  OPS_Init(&new_ops);

  OP *entry_sp_adj_op = NULL;
  OP *exit_sp_adj_op = NULL;

  BB *new_bb = Gen_BB_Like(old_bb);

  // Copy over the minimum amount of info needed by the scheduler.
  BB_bbregs(new_bb) = BB_bbregs(old_bb);
  BB_flag(new_bb) = BB_flag(old_bb);

  // Create entryinfo/exitinfo for new BB.  A new entryinfo/exitinfo is needed
  // to hold the new BB_entry_sp_adj_op/BB_exit_sp_adj_op.
  if (BB_entry(old_bb)) {
    entry_sp_adj_op = BB_entry_sp_adj_op(old_bb);
    ANNOTATION *old_annot = ANNOT_Get(BB_annotations(old_bb), ANNOT_ENTRYINFO);
    ENTRYINFO *old_info = ANNOT_entryinfo(old_annot);
    ENTRYINFO *new_info = TYPE_P_ALLOC(ENTRYINFO);
    // Copy over everything except ENTRYINFO_sp_adj.
    ENTRYINFO_name(new_info) = ENTRYINFO_name(old_info);
    ENTRYINFO_entry_wn(new_info) = ENTRYINFO_entry_wn(old_info);
    ENTRYINFO_srcpos(new_info) = ENTRYINFO_srcpos(old_info);
    BB_Add_Annotation(new_bb, ANNOT_ENTRYINFO, new_info);
  }
  if (BB_exit(old_bb)) {
    exit_sp_adj_op = BB_exit_sp_adj_op(old_bb);
    ANNOTATION *old_annot = ANNOT_Get(BB_annotations(old_bb), ANNOT_EXITINFO);
    EXITINFO *old_info = ANNOT_exitinfo(old_annot);
    EXITINFO *new_info = TYPE_P_ALLOC(EXITINFO);
    // Copy over everything except EXITINFO_sp_adj.
    EXITINFO_srcpos(new_info) = EXITINFO_srcpos(old_info);
    BB_Add_Annotation(new_bb, ANNOT_EXITINFO, new_info);
  }

  FOR_ALL_BB_OPs(old_bb, op) {
    OP *new_op = Dup_OP(op);
    Copy_WN_For_Memory_OP(new_op, op);
    OP_srcpos(new_op) = OP_srcpos(op);
    OPS_Append_Op(&new_ops, new_op);

    if (op == entry_sp_adj_op)
      Set_BB_entry_sp_adj_op(new_bb, new_op);
    else if (op == exit_sp_adj_op)
      Set_BB_exit_sp_adj_op(new_bb, new_op);
  }
  BB_Append_Ops(new_bb, &new_ops);
  return new_bb;
}

static BOOL
Run_One_Sched (HB_Schedule *Sched, BOOL is_fwd, BB *bb, HBS_TYPE hbs_type,
	       const char *name, INT32 *best_cycles,
	       const char **best_name, BOOL *best_is_fwd, BB **best_bb = NULL)
{
  INT32 max_cycles = *best_cycles;

  // Make a copy of the original BB for the scheduler to work with, so that the
  // scheduler can add side effects, such as copy insertion and TN renaming,
  // without disturbing the original BB.
  if (best_bb != NULL) {
    bb = Gen_BB_For_Sched(bb);
    // Force the scheduler to create an initial schedule for the BB.
    max_cycles = INT32_MAX;
  }

  Sched->Init(bb, hbs_type, max_cycles, NULL, NULL);
  BOOL optimal = Sched->Schedule_BB(bb, NULL, is_fwd);
  if (BB_length(bb) > 0 &&
      (OP_scycle(BB_last_op(bb)) + 1) < *best_cycles) {
    *best_cycles = OP_scycle(BB_last_op(bb)) + 1;
    *best_name = name;
    *best_is_fwd = is_fwd;

    // Save the new OPs ordering.
    if (best_bb != NULL) {
      *best_bb = bb;
    }
  }
  return optimal;
}

// Run the BB scheduler.  Run it multiple times if necessary to pick the best
// schedule.  Return TRUE if the schedule was generated using forward
// scheduling.
static BOOL
Run_Sched (HB_Schedule *Sched, BB *bb, HBS_TYPE hbs_type, INT32 max_sched)
{
  INT32 best_cycles = max_sched;
  const char *best_name = "none";
  BOOL best_is_fwd = FALSE;
  BOOL optimal = FALSE;
  int length = BB_length(bb);

  // Save a copy of the original OPs if scheduler is to be run mulitple times
  // with side effects, such as copy insertion.
  BB **best_bb = NULL;
  BB *the_best_bb = NULL;

  if ((hbs_type & HBS_RELAX_REGANTI) &&
      (LOCS_Best ||
       LOCS_Scheduling_Algorithm == 2 /* backward and forward */)) {
    best_bb = &the_best_bb;
  }

  if (BB_keep_prefetch(bb))
    hbs_type = hbs_type & ~HBS_DROP_UNSCHED_PREFETCHES;

  if (LOCS_Best) {

    // Run scheduler many times.
 
    HBS_TYPE base_hbs_type = hbs_type
			     & ~HBS_BALANCE_READY_TYPES
			     & ~HBS_BALANCE_UNSCHED_TYPES;
    
    optimal = Run_One_Sched(Sched, 0, bb, base_hbs_type, "backward",
			    &best_cycles, &best_name, &best_is_fwd, best_bb);

    if (length > 1) {
      // Backward scheduling.

      if (!optimal)
        optimal = Run_One_Sched(Sched, 0, bb,
				base_hbs_type | HBS_BALANCE_READY_TYPES,
				"backward balance_ready_types",
				&best_cycles, &best_name,
				&best_is_fwd, best_bb);

      if (!optimal)
	optimal = Run_One_Sched(Sched, 0, bb,
				base_hbs_type | HBS_BALANCE_UNSCHED_TYPES,
				"backward balance_unsched_types",
				&best_cycles, &best_name,
				&best_is_fwd, best_bb);

      if (!optimal)
	optimal = Run_One_Sched(Sched, 0, bb,
				base_hbs_type | HBS_BALANCE_UNSCHED_TYPES
				  | HBS_BALANCE_READY_TYPES,
				"backward balance_unsched_types balance_ready_types",
				&best_cycles, &best_name,
				&best_is_fwd, best_bb);

#ifndef TARG_MIPS
      // Forward scheduling. Dropping prefetches is not supported.
      if (!optimal)
        optimal = Run_One_Sched(Sched, 1, bb,
				base_hbs_type & ~HBS_DROP_UNSCHED_PREFETCHES,
				"forward",
				&best_cycles, &best_name,
				&best_is_fwd, best_bb);
#endif
    }
  } else if (LOCS_Scheduling_Algorithm == 2) {

    // Do backward and forward scheduling.

    optimal = Run_One_Sched(Sched, 0, bb, hbs_type, "backward",
			    &best_cycles, &best_name, &best_is_fwd, best_bb);

    if (length > 1) {
      if (!optimal)
	optimal = Run_One_Sched(Sched, 1, bb, hbs_type, "forward",
				&best_cycles, &best_name,
				&best_is_fwd, best_bb);
    }

  // Run scheduler once.
  } else {
    Sched->Init(bb, hbs_type, max_sched, NULL, NULL);
    Sched->Schedule_BB(bb, NULL, LOCS_Scheduling_Algorithm);
    best_is_fwd = (LOCS_Scheduling_Algorithm == 1) ? TRUE : FALSE;
  }

  // Transfer the best schedule to the BB.
  if (best_bb != NULL &&
      *best_bb != NULL) {
    BB_Remove_All(bb);
    BB_Append_All(bb, *best_bb);
    OP *entry_sp_adj_op = BB_entry_sp_adj_op(*best_bb);
    OP *exit_sp_adj_op = BB_exit_sp_adj_op(*best_bb);
    if (entry_sp_adj_op != NULL)
      Set_BB_entry_sp_adj_op(bb, entry_sp_adj_op);
    if (exit_sp_adj_op != NULL)
      Set_BB_exit_sp_adj_op(bb, exit_sp_adj_op);
    // Copy the register requests computed indirectly through scheduling.
    LRA_Copy_Register_Request(bb, *best_bb);
  }

  // Delete prefetches that didn't fit into idle issue slots.
  if (hbs_type & HBS_DROP_UNSCHED_PREFETCHES)
    Delete_Unscheduled_Prefetches(bb);

  if (Trace_HB &&
      best_name != NULL) {
    fprintf(TFile, "Best scheduling heuristic: %s\n", best_name);
  }

  return best_is_fwd;
}
#endif

// ======================================================================
// IGLS_Schedule_Region 
//
// The main driver for invoking all the scheduling phases in CG. They mainly
// include HBS (for single-BBs and hyperblocks) and GCM. The data-speculation
// phase is also invoked here since it's tied very closely with the 
// scheduling phase. 
// The <before_regalloc> parameter indicates whether the scheduler is being 
// invoked before or after register allocation. The amount of work done by 
// the various phases depends on the optimization level. 
//
// -O0 : insert noops to remove hazards.
// -O1 : perform HBS scheduling for local BBs (ONLY) after register allocation.
//       fill branch delay slot nops (for MIPS).
// -O2 : perform hyperblock(s) scheduling before register allocation.
//       provide accurate register estimates for GRA.
//       invoke post-GRA global scheduling (post-GCM) phase
//       invoke THR phase to perform data-speculation (after register-
//       allocation).
// -O3 : perform hyperblock(s) scheduling before register allocation.
//	 provide accurate register estimates for GRA/GCM.
//	 invoke pre-GRA global scheduling (pre-GCM) phase.
//	 invoke post-GRA global scheduling (post-GCM) phase
//
// ======================================================================
void
IGLS_Schedule_Region (BOOL before_regalloc)
{
  BB *bb;
  BOOL should_we_local_schedule;  // controls local scheduling (single BBs).
  BOOL should_we_global_schedule; // controls HB scheduling and GCM.
  BOOL should_we_schedule;        // controls all scheduling (LOCS,  HBS, GCM)
  BOOL should_we_do_thr;          // controls the THR phase in CG.

  RID *rid;
  HBS_TYPE hbs_type;
  HB_Schedule *Sched = NULL;
  CG_THR      *thr = NULL;

  Set_Error_Phase ("Hyperlock Scheduler");
  Start_Timer (T_Sched_CU);
  Trace_HB = Get_Trace (TP_SCHED, 1);
  should_we_schedule = IGLS_Enable_All_Scheduling;
  should_we_do_thr = CG_enable_thr;
  L_Save();

  // 12581: In "main" entry, always keep spadjust at top, so that debugging
  // info and ctrl register setup occur in correct order.
  if (!strcmp(Cur_PU_Name, "MAIN__") || !strcmp(Cur_PU_Name, "main")) {
    Set_BB_scheduled(REGION_First_BB);
  }

  if (before_regalloc) {

    // schedule if (-O > O1) and
    // -CG:local_sched=on && -CG:pre_local_sched=on.
    should_we_local_schedule = (   CG_opt_level > 1
				   && LOCS_Enable_Scheduling
				   && LOCS_PRE_Enable_Scheduling);

    // global schedule if (-O > O2) and either of the following below are true.
    // -CG:hb_sched=on && -CG:pre_hb_sched=on (hyperblock scheduling).
    // -CG:gcm=on && -CG:pre_gcm=on for GCM.
    should_we_global_schedule = ( CG_opt_level > 2 &&
				  ((IGLS_Enable_HB_Scheduling &&
				    IGLS_Enable_PRE_HB_Scheduling) ||
				   (GCM_PRE_Enable_Scheduling &&
				    GCM_Enable_Scheduling)));

    hbs_type = HBS_BEFORE_GRA | HBS_BEFORE_LRA | HBS_DEPTH_FIRST;
#ifdef KEY
    if (LOCS_Balance_Ready_Types)
      hbs_type |= HBS_BALANCE_READY_TYPES;
    if (LOCS_Balance_Unsched_Types)
      hbs_type |= HBS_BALANCE_UNSCHED_TYPES;
    if (LOCS_Add_Copy ||
	(!LOCS_Add_Copy_set && CG_opt_level > 2)) {
      hbs_type |= HBS_RELAX_REGANTI;
    }
#endif

    if (Trace_HB) {
      #pragma mips_frequency_hint NEVER
      fprintf (TFile, "***** HYPERBLOCK SCHEDULER (before GRA) *****\n");
    }
  }
  else {

    // schedule if (-O > O0) and
    // -CG:local_sched=on && -CG:post_local_sched=on.
    should_we_local_schedule = (   CG_opt_level > 0
				   && LOCS_Enable_Scheduling
				   && LOCS_POST_Enable_Scheduling);

    // global schedule if (-O > O1) and either of the following below are true.
    // -CG:hb_sched=on && -CG:post_hb_sched=on (hyperblock scheduling).
    // -CG:gcm=on && -CG:post_gcm=on for GCM.
    should_we_global_schedule = ( CG_opt_level > 1 &&
				  ((IGLS_Enable_HB_Scheduling &&
				   (IGLS_Enable_POST_HB_Scheduling ||
				    IGLS_Enable_PRE_HB_Scheduling)) ||
				   (GCM_Enable_Scheduling &&
				    GCM_POST_Enable_Scheduling)));
    hbs_type = HBS_CRITICAL_PATH;
    if (PROC_has_bundles()) hbs_type |= HBS_MINIMIZE_BUNDLES;
#ifdef TARG_ST
    hbs_type |= HBS_HEURISTIC_CRITICAL_PATH;
#endif

#ifdef KEY
    if (LOCS_Balance_Ready_Types)
      hbs_type |= HBS_BALANCE_READY_TYPES;
    if (LOCS_Balance_Unsched_Types)
      hbs_type |= HBS_BALANCE_UNSCHED_TYPES;
#endif

    // allow data-speculation if (-O > O1) and -OPT:space is turned off.
#ifndef TARG_ST
    should_we_do_thr = should_we_do_thr && (CG_opt_level > 1) && !OPT_Space;
#else
    // TB: When opt_space is true, CG_enable_thr is false
    should_we_do_thr = should_we_do_thr && (CG_opt_level > 1) ;
#endif
    if (Trace_HB) {
      #pragma mips_frequency_hint NEVER
      fprintf (TFile, "***** HYPERBLOCK SCHEDULER (after GRA) *****\n");
    }
  }

  // Before register allocation:
  // - Do hyperblock scheduling first to get perfect schedules at each
  //   hyperblock level (register-sensitive). 
  // - Do GCM next to extract global parallelism. Some work needs to be
  //   done, so that it strictly enforces hyperblock boundaries.
  // - Do local scheduling for BBs which are not part of any hyperblocks.

  if (before_regalloc) {
    if (!should_we_schedule) return;

    // Do HB scheduling for all HBs generated (before register allocation).
    if (IGLS_Enable_HB_Scheduling && IGLS_Enable_PRE_HB_Scheduling &&
	should_we_global_schedule) {
      HB_Remove_Deleted_Blocks();
      std::list<HB*>::iterator hbi;
      FOR_ALL_BB_STLLIST_ITEMS_FWD(HB_list, hbi) {
	if (!Sched) {
	  Sched = CXX_NEW(HB_Schedule(), &MEM_local_pool);
	}

	// Check to see if not SWP'd.
	std::list<BB*> hb_blocks;
	Get_HB_Blocks_List(hb_blocks,*hbi);
	if (Can_Schedule_HB(hb_blocks)) {
	  Sched->Init(hb_blocks, hbs_type, NULL);
	  Sched->Schedule_HB(hb_blocks);
	}
      }
    }

    // Try GCM (before register allocation).
    if (GCM_Enable_Scheduling && should_we_global_schedule) {
	Stop_Timer (T_Sched_CU);

	GCM_Schedule_Region (hbs_type);

	Set_Error_Phase ("Hyperblock Scheduler (HBS)");
	Start_Timer (T_Sched_CU);
    }

    if (!should_we_local_schedule) return;

    // Do local scheduling for BBs which are not part of HBs. 
    // (before register allocation).
    for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
      
      if (    ( rid = BB_rid(bb) )
	      && ( RID_level(rid) >= RL_CGSCHED ) )
	continue;
      
      if (!BB_scheduled(bb)) {
	if (!Sched) {
	  Sched = CXX_NEW(HB_Schedule(), &MEM_local_pool);
	}
#ifdef KEY
	Run_Sched(Sched, bb, hbs_type, INT32_MAX);
#else
	Sched->Init(bb, hbs_type, INT32_MAX, NULL, NULL);
	Sched->Schedule_BB(bb, NULL, LOCS_Scheduling_Algorithm);
#endif
      }
    }
  }
  else {

    // After register allocation:
    // - Perform data-speculation first, since it will expose more 
    //   parallelism and scheduling opportunities at the block level.
    // - Do hyperblock scheduling next to get perfect schedules at each
    //   hyperblock level (parallelism-driven).
    // - Do GCM next to extract global parallelism. Some work needs to be
    //   done, so that it strictly enforces hyperblock boundaries.
    // - Do local scheduling for BBs which are not part of any hyperblocks.

    // Perform data-speculation first, since it will expose parallelism
    // and scheduling opportunities at the block level.
    // TODO: Invoke data-speculation phase before register allocation,
    // requires GRA spill support, and conditionally invoke the phase
    // after register allocation.

#ifdef KEY
    // Drop prefetches that can't be scheduled into an unused issue slot.
    if (LOCS_Reduce_Prefetch)
      hbs_type |= HBS_DROP_UNSCHED_PREFETCHES;

    // Raise priority of some OPs and retry scheduling.
    if (LOCS_Retry)
      hbs_type |= HBS_RETRY;
#endif

    if (should_we_do_thr) {
      Stop_Timer (T_Sched_CU);
      Set_Error_Phase ("Tree-Height Reduction (THR)");
      Start_Timer (T_THR_CU);

      for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
	if (    ( rid = BB_rid(bb) )
		&& ( RID_level(rid) >= RL_CGSCHED ) )
	  continue;

	// Perform data-speculation (if profitable).
	// Avoid processing SWP scheduled blocks, all other scheduled blocks
	// are still considered as candidates for THR.

	if (BB_scheduled(bb) && !BB_scheduled_hbs(bb)) continue;
	if (!thr) {
	  thr = CXX_NEW(CG_THR(), &MEM_local_pool);
	}
	thr->Init(bb, THR_DATA_SPECULATION_NO_RB, FALSE);
	thr->Perform_THR();
	
      } /* for (bb= REGION_First_BB).. */

      Stop_Timer (T_THR_CU);
      Check_for_Dump (TP_THR, NULL);
      Start_Timer (T_Sched_CU);

    } /* should_we_do_thr */

    // Do HB scheduling for all HBs generated (after register allocation).
    if (IGLS_Enable_HB_Scheduling && IGLS_Enable_POST_HB_Scheduling &&
	should_we_schedule && should_we_global_schedule) {

      HB_Remove_Deleted_Blocks();
      std::list<HB*>::iterator hbi;
      FOR_ALL_BB_STLLIST_ITEMS_FWD(HB_list, hbi) {
	if (!Sched) {
	  Sched = CXX_NEW(HB_Schedule(), &MEM_local_pool);
	}
	// Check to see if not SWP'd.
	std::list<BB*> hb_blocks;
	Get_HB_Blocks_List(hb_blocks,*hbi);
	if (Can_Schedule_HB(hb_blocks)) {
	  Sched->Init(hb_blocks, hbs_type, NULL);
	  Sched->Schedule_HB(hb_blocks);
	}
      }
    }

    // Try GCM for the region (after register allocation).
    if (GCM_Enable_Scheduling && should_we_schedule &&
	should_we_global_schedule) {
	Stop_Timer (T_Sched_CU);

 	GCM_Schedule_Region (hbs_type);

        Set_Error_Phase ("Hyperblock Scheduler (HBS)");
	Start_Timer (T_Sched_CU);
    }
#ifdef TARG_ST
    CGTARG_Resize_Instructions ();
#endif

    // Do local scheduling for BBs which are not part of HBs. 
    // (after register allocation).
    for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
      if (    ( rid = BB_rid(bb) )
	      && ( RID_level(rid) >= RL_CGSCHED ) )
	continue;

      BOOL skip_bb = BB_scheduled(bb) && !BB_scheduled_hbs(bb);

      if (should_we_do_thr && !skip_bb) Remove_Unnecessary_Check_Instrs(bb);

#ifdef KEY_1873
      /* The original code with Reschedule_BB is meanlingless. I think the original
	 author meant BB_scheduled(bb), not Reschedule_BB(bb).
      */
      const BOOL resched = FALSE;
#else
      BOOL resched = !skip_bb && Reschedule_BB(bb); /* FALSE; */ 
#ifdef TARG_ST
      // FdF 20050502: BB_scheduled_hbs(bb) is always TRUE on
      // post-pass scheduling, so ignore it.
      skip_bb = BB_scheduled(bb);

      // FdF 20050502: resched means that post-pass scheduling is not
      // necessary. If performed, it can be ignored if it is not
      // better than the current scheduling.
      resched = skip_bb && Reschedule_BB(bb) && (BB_length(bb)>1); /* FALSE; */
#endif

#endif // KEY
      if (should_we_schedule && should_we_local_schedule &&
	  (!skip_bb || resched)) {

	// TODO: try locs_type = LOCS_DEPTH_FIRST also.
	INT32 max_sched = (resched) ?  OP_scycle(BB_last_op(bb))+1 : INT32_MAX;

#ifdef KEY
	// Reschedule if new OPs were added since the last time the BB was
	// scheduled.  This includes spill OPs added by the register allocator.
	if (max_sched < INT32_MAX) {
	  OP *op;
	  FOR_ALL_BB_OPs_FWD(bb, op) {
	    if (OP_scycle(op) == 0) {
	      max_sched = INT32_MAX;
	      break;
	    }
	  }
	}
#endif
	if (LOCS_Enable_Scheduling) {
	  if (!Sched) {
	    Sched = CXX_NEW(HB_Schedule(), &MEM_local_pool);
	  }
#ifdef KEY
	  BOOL is_fwd = Run_Sched(Sched, bb, hbs_type, max_sched);

	  // Regenerate the schedule in the forward direction to make the
	  // scycle numbers match the way the processor multi-issue the OPs.
	  // (SiCortex 5734)
	  if (!is_fwd &&
	      LOCS_Renumber_Cycles) {
	    Sched->Init(bb, HBS_NO_REORDER, INT32_MAX, NULL, NULL);
	    Sched->Schedule_BB(bb, NULL, 1);
	  }
#else
#ifdef TARG_ST
          switch(LOCS_POST_Scheduling)
            {
            default:
            case Forward_Post_Sched:
              Sched->Init(bb, hbs_type, max_sched, NULL, NULL);
              Sched->Schedule_BB(bb, NULL, TRUE);
             break;
            case Backward_Post_Sched:
              Sched->Init(bb, hbs_type, max_sched, NULL, NULL);
              Sched->Schedule_BB(bb, NULL, FALSE);
              break;
            case Double_Post_Sched:
              Sched->Init(bb, hbs_type, max_sched, NULL, NULL);
              Sched->Schedule_BB(bb, NULL, TRUE);
              Sched->Init(bb, hbs_type, max_sched, NULL, NULL);
              Sched->Schedule_BB(bb, NULL, FALSE);
              break;
            // TDR: New optimization to improve on xp70
            case Optimized_Post_Sched:
              Sched->Init(bb, hbs_type, max_sched, NULL, NULL);
              Sched->Schedule_BB(bb, NULL, TRUE);
              Sched->Init(bb, (hbs_type | HBS_CRITICAL_PATH_PREF_LOAD));
              Sched->Schedule_BB(bb, NULL, TRUE);
              break;
            case Optimized_Double_Post_Sched:
              Sched->Init(bb, hbs_type, max_sched, NULL, NULL);
              Sched->Schedule_BB(bb, NULL, TRUE);
              Sched->Init(bb, hbs_type);
              Sched->Schedule_BB(bb, NULL, FALSE);
              Sched->Init(bb, (hbs_type | HBS_CRITICAL_PATH_PREF_LOAD));
              Sched->Schedule_BB(bb, NULL, TRUE);
              break;
            case Optimized_Double_Load_Sched:
              Sched->Init(bb, hbs_type, max_sched, NULL, NULL);
              Sched->Schedule_BB(bb, NULL, TRUE);
              Sched->Init(bb, hbs_type);
              Sched->Schedule_BB(bb, NULL, FALSE);
              Sched->Init(bb, (hbs_type | HBS_CRITICAL_PATH_PREF_LOAD | HBS_PREF_LOAD));
              Sched->Schedule_BB(bb, NULL, TRUE);
              break;
            case Optimized_Load_Sched :
              Sched->Init(bb, hbs_type, max_sched, NULL, NULL);
              Sched->Schedule_BB(bb, NULL, TRUE);
              Sched->Init(bb, (hbs_type | HBS_CRITICAL_PATH_PREF_LOAD | HBS_PREF_LOAD));
              Sched->Schedule_BB(bb, NULL, TRUE);
              break;
            }
#else
	  Sched->Init(bb, hbs_type, max_sched, NULL, NULL);
	  Sched->Schedule_BB(bb, NULL, LOCS_Scheduling_Algorithm);
#endif
#endif
	}
      }
      Handle_All_Hazards (bb);
    } /* for (bb= REGION_First_BB).. */

#ifdef TARG_X8664
    {
      extern void CG_Sched( MEM_POOL*, BOOL );
      CG_Sched( &MEM_local_pool, Get_Trace( TP_SCHED, 1 ) );
    }
#endif

    // Do branch optimizations here.
    if (should_we_schedule && should_we_local_schedule) {
      if (GCM_Enable_Scheduling) GCM_Fill_Branch_Delay_Slots ();
      if (Assembly) Add_Scheduling_Notes_For_Loops ();
    }
  }

  // need to explicitly delete Sched and thr
  // so that destructors are called.
  if (Sched) {
	CXX_DELETE(Sched, &MEM_local_pool);
  }
  if (thr) {
	CXX_DELETE(thr, &MEM_local_pool);
  }
  L_Free();

  Check_for_Dump (TP_SCHED, NULL);
  Stop_Timer (T_Sched_CU);
}
#ifdef LAO_ENABLED
extern BB_MAP CG_LAO_Region_Map;
void
LAO_Schedule_Region (BOOL before_regalloc, BOOL frequency_verify)
{
  if (before_regalloc) {
    Set_Error_Phase( "LAO Prepass Optimizations" );
    if (CG_LAO_activation & OptimizeActivation_PrePass) {
      lao_optimize_pu(CG_LAO_activation & OptimizeActivation_PrePass,
		      before_regalloc);
      if (frequency_verify)
	FREQ_Verify("LAO Prepass Optimizations");
    }
  }
  else {
    // Call the LAO for postpass scheduling.
    Set_Error_Phase( "LAO Postpass Optimizations" );
    if (CG_LAO_activation & (OptimizeActivation_PostPass|
                                OptimizeActivation_Encode)) {
      lao_optimize_pu(CG_LAO_activation & (OptimizeActivation_PostPass|
                                              OptimizeActivation_Encode),
		      before_regalloc);
      if (frequency_verify)
	FREQ_Verify("LAO Postpass Optimizations");
    }
#ifdef TARG_ST
    CGTARG_Resize_Instructions ();
#endif
    // Direct call to the bundler, and bypass the IGLS.
    Set_Error_Phase( "LAO Bundling Optimizations" );
    REG_LIVE_Analyze_Region();
    Trace_HB = Get_Trace (TP_SCHED, 1);
    for (BB *bb = REGION_First_BB; bb; bb = BB_next(bb)) {
      Handle_All_Hazards(bb);
      // Handle_All_Hazards will have fixed branch operations with scheduling date -1
      // Only add notes if the postpass scheduler effectively ran
      if (Assembly && CG_LAO_Region_Map && BB_length(bb)) Add_Scheduling_Note (bb, NULL);
    }
    REG_LIVE_Finish();
    // Only add notes if the postpass scheduler effectively ran
    if (Assembly && CG_LAO_Region_Map) Add_Scheduling_Notes_For_Loops ();
  }
}
#endif


