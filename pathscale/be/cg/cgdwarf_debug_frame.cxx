/*

  Copyright (C) 2006 ST Microelectronics.  All Rights Reserved.

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

  Contact information:  , or:

  http://www.st.com

*/


/* ====================================================================
 * ====================================================================
 * Module: cgdwarf_debug_frame.cxx
 *
 * Description:
 *
 * Interface between cgemit.cxx and em_dwarf.cxx for all the
 * debug_frame related support
 *
 * ====================================================================
 * ====================================================================
 */

// [HK]
#if __GNUC__ >= 3
#include <list>
using std::list;
#else
#include <list.h>
#endif // __GNUC__ >= 3


#include <stdio.h>
#include <stdlib.h>
#include <libelf.h>
#include <map>
#include <vector>
#include <set>
#include <algorithm>

#include "W_alloca.h"
#include "tracing.h"
#include "config_asm.h"
#include "config_list.h"
#include "glob.h"

#include "cgir.h"
#include "tn_map.h"
#include "em_elf.h"
#include "dwarf_DST_mem.h"         /* for DST_IDX */
#include "em_dwarf.h"

#include "cgemit.h"
#include "cgdwarf.h"               /* [CL] for CGD_LABIDX */
#include "cgdwarf_debug_frame.h"
#include "cg.h"                    /* for PU_Has_EH_Return */
#include "mempool.h" // For MEM_POOL definition

BOOL Trace_Unwind = FALSE;

BOOL has_asm;
/**
 * Represent a boolean that must be true to allow unwind information analysis
 */
#define OPTIONS_CHECK ((CG_emit_unwind_info && CG_emit_asm_dwarf) || \
                       CXX_Exceptions_On)

/**
 * Memory pool used for the whole debug frame generation
 */
static MEM_POOL dw_Mem_Pool;

typedef list < UNWIND_ELEM > UEs; 
typedef UEs::iterator UEsIt;

// use list not slist cause append to end
static UEs ue_list;
static UEsIt ue_iter;
static UINT last_when;
static UINT last_label = 0;
static TN_MAP tn_def_op;

typedef std::map<DebugRegId, UEs> DbgIdToUEs;
typedef DbgIdToUEs::iterator DbgIdToUEsIt;

typedef std::map<DebugRegId, UNWIND_ELEM*> DbgIdToPUE;
typedef std::vector<DbgIdToPUE> VectorOfDbgIdToPUE;
typedef DbgIdToPUE::iterator DbgIdToPUEIt;
typedef DbgIdToPUE::const_iterator DbgIdToPUECIt;


BOOL PU_has_FP = FALSE;  // does the current PU use a frame pointer?
BOOL PU_has_restored_FP = FALSE; // has the current PU already restored FP?

static BOOL UE_validated; // was the Tag_Irrelevant_Saves_And_Restores
			  // pass alreadly run?
typedef CCIEInfo::SavedList SetOfDbgId;
typedef CCIEInfo::CItSavedList SetOfDbgIdCIt;
typedef std::vector<SetOfDbgId> VectorOfSetOfDbgId;
typedef CCIEInfo::LessThanDebugRegId CmpDbgId;
typedef CCIEInfo::DebugRegIdAllocator DbgIdAllocator;

static SetOfDbgId PU_saved_dbgid; // Set of DebugId to be tracked for debug_frame

void
Print_Unwind_Elem (UNWIND_ELEM ue, char *msg)
{
	fprintf(TFile, "<%s> at bb %d when %d: ", msg, ue.bb->id, ue.when);
	switch (ue.kind) {
	case UE_CREATE_FRAME:
		fprintf(TFile, " create_frame, size %lld",
			ue.offset); 
		break;
	case UE_DESTROY_FRAME:
		fprintf(TFile, " destroy_frame, size %lld",
			ue.offset);
		break;
	case UE_CREATE_FP:
		fprintf(TFile, " create_fp, size %lld",
			ue.offset); 
		break;
	case UE_DESTROY_FP:
		fprintf(TFile, " destroy_fp");
		break;
	case UE_EPILOG:
		fprintf(TFile, " epilog"); 
		break;
	case UE_LABEL:  
		fprintf(TFile, " label state %d",
			ue.label);
		break;
	case UE_COPY:  
		fprintf(TFile, " copy label state %d",
			ue.label);
		break;
	case UE_SAVE_GR:  
		fprintf(TFile, " save %lld in gr %lld",
			(long long)ue.rc_reg, (long long)ue.save_rc_reg);
		break;
	case UE_SAVE_SP:
	case UE_SAVE_FP:
		fprintf(TFile, " save %lld", (long long)ue.rc_reg);
#ifdef PROPAGATE_DEBUG
		fprintf(TFile, " to mem %lld(%s) %lld(entry-%s)", ue.offset,
			(ue.kind == UE_SAVE_SP ? "sp" : "fp"), ue.top_offset,
#else
		fprintf(TFile, " to mem %lld(%s)", ue.offset,
#endif
			(ue.kind == UE_SAVE_SP ? "sp" : "fp") );
		break;
	case UE_RESTORE_GR:  
		fprintf(TFile, " restore %lld", (long long)ue.rc_reg);
		if (ue.save_rc_reg != DebugRegId_undef) {
		  fprintf(TFile, " from %lld", (long long)ue.save_rc_reg);
		}
		break;
	case UE_RESTORE_SP:
	case UE_RESTORE_FP:
#ifdef PROPAGATE_DEBUG
		fprintf(TFile, " restore %lld from mem %lld(%s) %lld(entry-%s)",
			(long long)ue.rc_reg,
			ue.offset, (ue.kind == UE_RESTORE_SP ? "sp" : "fp"),
			ue.top_offset,
			(ue.kind == UE_RESTORE_SP ? "sp" : "fp"));
#else
		fprintf(TFile, " restore %lld from mem %s",
			(long long)ue.rc_reg,
			(ue.kind == UE_RESTORE_SP ? "sp" : "fp"));
#endif
		break;
	}

#ifdef PROPAGATE_DEBUG
	switch (ue.kind) {
	case UE_SAVE_GR:  
	case UE_SAVE_SP:
	case UE_SAVE_FP:
	case UE_RESTORE_GR:  
	case UE_RESTORE_SP:
	case UE_RESTORE_FP:
	  if (UE_validated) {
	    fprintf(TFile, " %svalid", (ue.valid == TRUE) ? "" : "in");
	  }
	  break;
	}

	if (ue.handler) {
	  fprintf(TFile, " in exception handler");
	}
#endif
	fprintf(TFile, "\n");
}

void
Print_All_Unwind_Elem (char *msg)
{
  for (ue_iter = ue_list.begin(); ue_iter != ue_list.end(); ++ue_iter) {
    	Print_Unwind_Elem (*ue_iter, msg);
  }
}

static void
Print_Set(FILE* file, const SetOfDbgId& set)
{
    SetOfDbgIdCIt it;
    for(it = set.begin(); it != set.end(); ++it) {
        if(it == set.begin()) {
            fprintf(file, "%lld", (long long)*it);
        } else {
            fprintf(file, " %lld", (long long)*it);
        }
    }
}

/* Computes the set of DebugId that should bes acked in debug frame.
 * Usually correspond to the list associated to the CIE, but some
 * exception might exist (currently it is the case for procedure
 * with eh_return()).
 */
static void
Compute_Saved_DbgId() {
  PU_saved_dbgid = CIEs[CIE_index(Get_Current_PU())].Saved();

  // [SC] In an eh_return PU, we want unwind information for eh_return registers.
  if (PU_Has_EH_Return) {
    ISA_REGISTER_CLASS rc;
    FOR_ALL_ISA_REGISTER_CLASS(rc) {
      REGISTER_SET eh_return_set;
      eh_return_set = REGISTER_CLASS_eh_return(rc);
      if (!REGISTER_SET_EmptyP(eh_return_set)) {
	int bitSize = ISA_REGISTER_CLASS_INFO_Bit_Size(ISA_REGISTER_CLASS_Info(rc));
	REGISTER r;
	r = REGISTER_SET_Choose(eh_return_set);
	do {
	  PU_saved_dbgid.insert(Get_Debug_Reg_Id(rc, r, bitSize));
	  r = REGISTER_SET_Choose_Next(eh_return_set, r);
	} while (r != REGISTER_UNDEFINED);
      }
    }
  }
}

BOOL TN_Is_Unwind_Reg (TN *tn)
{
  if (!TN_is_register(tn)) {
    return FALSE;
  }

  return (PU_saved_dbgid.find(Get_Debug_Reg_Id(tn)) != PU_saved_dbgid.end());
}

// search for save-tn that "tn" is copied from.
TN* Get_Copied_Save_TN (TN *tn, OP *cur_op, BB *bb)
{
#ifdef PROPAGATE_DEBUG
    if (TN_Is_Unwind_Reg(tn)) {
#ifdef DEBUG_UNWIND
      fprintf(TFile, "** %s is an unwind reg %lld\n", __FUNCTION__,
              (long long)Get_Debug_Reg_Id(tn));
#endif
	return tn;
    }

    // FIXME: ensure that find_def_of_tn returns an op that occurs BEFORE cur_op
    if (TN_is_dedicated(tn)) 
    {
#ifdef DEBUG_UNWIND
      fprintf(TFile, "** %s is dedicated reg %lld\n", __FUNCTION__,
              (long long)Get_Debug_Reg_Id(tn));
#endif
	return NULL;
    }
    
	// else find tn that this is a copy of.
    OP *op = OP_Find_TN_Def_In_BB (cur_op, tn);

#ifdef DEBUG_UNWIND
    if (op) {
      fprintf(TFile, "** %s def of TN %p:\n", __FUNCTION__, op);
      Print_OP_No_SrcLine(op);
    } else {
      fprintf(TFile, "** %s no def of\n", __FUNCTION__);
    }
#endif

    if (!op) return NULL;
    TN *otn = OP_Copy_Operand_TN(op);

#ifdef DEBUG_UNWIND
    if (otn) {
      fprintf(TFile, "** %s copy operand TN %p:\n", __FUNCTION__, otn);
      Print_TN(otn, FALSE);
      fprintf(TFile, "\n");
    } else {
      fprintf(TFile, "** %s no copy operand TN\n", __FUNCTION__);
    }
#endif

    if (!otn) {
	if (OP_move(op))
	    otn = OP_opnd(op,1);
    }
    if (otn && TN_Is_Unwind_Reg(otn)) 
    {
#ifdef DEBUG_UNWIND
      fprintf(TFile, "** %s TN is save reg %lld\n", __FUNCTION__,
              (long long)Get_Debug_Reg_Id(otn));
#endif
	return otn;
    }
    else 
	return NULL;


#else
	// might already be a save-tn
    if (TN_is_save_reg(tn)) 
    {
#ifdef DEBUG_UNWIND
      fprintf(TFile, "** %s is a save reg %lld\n", __FUNCTION__,
              (long long)Get_Debug_Reg_Id(tn));
#endif
	return tn;
    }

    // [CL] "temporary" fix to handle the "-O1" issue (it happens tn
    // does not always have the TN_is_save_reg() property -- could
    // also happen at any optim level). Check the ABI register
    // property when in the entry block. We should also do the same
    // thing in the exit block, but as this is a temporary fix...
    if (BB_entry(bb) && TN_Is_Unwind_Reg(tn)) {
#ifdef DEBUG_UNWIND
      fprintf(TFile, "** %s is an unwind reg %lld\n", __FUNCTION__,
              (long long)Get_Debug_Reg_Id(tn));
#endif
	return tn;
    }

    if (TN_is_dedicated(tn)) 
    {
#ifdef DEBUG_UNWIND
      fprintf(TFile, "** %s is dedicated reg %lld\n", __FUNCTION__,
              (long long)Get_Debug_Reg_Id(tn));
#endif
	return NULL;
    }
    
	// else find tn that this is a copy of.
    OP *op = Find_Def_Of_TN (tn);

#ifdef DEBUG_UNWIND
    fprintf(TFile, "** %s def of TN %p:\n", __FUNCTION__, op);
    if (op) Print_OP_No_SrcLine(op);
#endif

    if (!op) return NULL;
    TN *otn = OP_Copy_Operand_TN(op);

#ifdef DEBUG_UNWIND
    fprintf(TFile, "** %s copy operand TN %p:\n", __FUNCTION__, otn);
    if (otn) {
      Print_TN(otn, FALSE);
      fprintf(TFile, "\n");
    }
#endif

    if (!otn) {
	if (OP_move(op))
	    otn = OP_opnd(op,1);
    }
    if (otn && TN_is_register(otn) && TN_is_save_reg(otn)) 
    {
#ifdef DEBUG_UNWIND
      fprintf(TFile, "** %s TN is save reg %lld\n", __FUNCTION__,
              (long long)Get_Debug_Reg_Id(otn));
#endif
	return otn;
    }
    else 
	return NULL;
#endif // PROPAGATE_DEBUG
}

void Record_UE(OP* op, UNWIND_ELEM* ue, BB* bb, UINT when)
{
  if (ue->kind != UE_UNDEFINED) {
    ue->qp = 0;
    ue->when = when;
    ue->bb = bb;
    ue_list.push_back (*ue);
    if (Trace_Unwind) Print_Unwind_Elem (*ue, "unwind1");
  }
}

static UINT
Get_BB_When_Length (BB *bb) 
{
  UINT w = 0;
  OP *op;
  FOR_ALL_BB_OPs_FWD(bb, op) {
  	if (OP_dummy(op)) continue;
	w += OP_Real_Unit_Slots(op);
  }
  return w;
}

static void
Find_Unwind_Info (void)
{
  OP *op;
  UINT when = 0;
  BB_Mark_Unreachable_Blocks ();

  // Create mapping of TN to defining OP.
  // This is used to find spill-tns, 
  // cause add and store may be in separate blocks.
  // TN_Reaching_Value_At_Op() doesn't catch all cases
  // (partly cause live-in/out sets not up-to-date),
  // so instead we do brute-force mapping.
  tn_def_op = TN_MAP_Create();
  for (BB *bb = REGION_First_BB; bb; bb = BB_next(bb)) {
#ifndef USE_UNREACHABLE
    if (BB_unreachable(bb)) continue;
#endif
    FOR_ALL_BB_OPs_FWD(bb, op) {
	// if multiple stores, will take last one
	if (OP_has_result(op))
		TN_MAP_Set (tn_def_op, OP_result(op,0), op);
    }
  }

  for (BB *bb = REGION_First_BB; bb; bb = BB_next(bb)) {
#ifndef USE_UNREACHABLE
    if (BB_unreachable(bb)) {
	when += Get_BB_When_Length(bb); 
	continue;
    }
#endif
    if (Trace_Unwind) Print_BB(bb);
    FOR_ALL_BB_OPs_FWD(bb, op) {
	if (OP_dummy(op)) continue;
	Analyze_OP_For_Unwind_Info(op, when, bb);
	when += OP_Real_Unit_Slots(op);
    }
    // if the current BB has restored FP and is an exit BB then the
    // next BB is entered from a path where FP was not restored
    if (PU_has_FP && PU_has_restored_FP && BB_exit(bb)) {
      PU_has_restored_FP = FALSE;
    }
  }
  last_when = when - 1;
  TN_MAP_Delete (tn_def_op);
}



static inline BOOL Get_PR (const SetOfDbgId& state, DebugRegId p)
{
	return (BOOL) (state.find(p) != state.end());
}
static inline BOOL Get_PR (const VectorOfSetOfDbgId& state, BB *bb, DebugRegId p)
{
	return (BOOL) (state[bb->id].find(p) != state[bb->id].end());
}
static inline void Set_PR (SetOfDbgId& state, DebugRegId p)
{
	state.insert(p);
}
static inline void Set_PR (VectorOfSetOfDbgId& state, BB *bb, DebugRegId p)
{
	state[bb->id].insert(p);
}
static inline void Clear_PR (SetOfDbgId& state, DebugRegId p)
{
	state.erase(p);
}

static SetOfDbgId
SetOfDbgId_Difference(const SetOfDbgId& setA, const SetOfDbgId& setB)
{
    // gcc is misled with the following syntax:
    // SetOfDbgId res(CmpDbgId(), DbgIdAllocator(&dw_Mem_Pool));
    // So use this one
    SetOfDbgId res = SetOfDbgId(CmpDbgId(), DbgIdAllocator(&dw_Mem_Pool));
    std::insert_iterator<SetOfDbgId> out(res, res.begin());
    std::set_difference(setA.begin(), setA.end(), setB.begin(), setB.end(),
                        out);
    return res;
}

static SetOfDbgId
SetOfDbgId_Union(const SetOfDbgId& setA, const SetOfDbgId& setB)
{
    SetOfDbgId res = SetOfDbgId(CmpDbgId(), DbgIdAllocator(&dw_Mem_Pool));
    std::insert_iterator<SetOfDbgId> out(res, res.begin());
    std::set_union(setA.begin(), setA.end(), setB.begin(), setB.end(),
                   out);
    return res;
}

static SetOfDbgId
SetOfDbgId_Intersection(const SetOfDbgId& setA, const SetOfDbgId& setB)
{
    SetOfDbgId res = SetOfDbgId(CmpDbgId(), DbgIdAllocator(&dw_Mem_Pool));
    std::insert_iterator<SetOfDbgId> out(res, res.begin());
    std::set_intersection(setA.begin(), setA.end(), setB.begin(), setB.end(),
                          out);
    return res;
}

#ifdef PROPAGATE_DEBUG
static INT64 current_frame_size = 0;

static BOOL is_handler = FALSE;

static void Tag_Irrelevant_Saves_And_Restores_For_BB(BB* bb,
				     DbgIdToUEs& pr_stack,
						     BOOL* visited,
						     INT64* frame_size_bb)
{
  // Record the UEs pushed/popped here (in this bb), in order to
  // restore pr_stack for recursion
  UEs pushed_popped_here;
  DebugRegId p;

  if (Trace_Unwind) {
    fprintf(TFile, "Recursing to BB %d current frame size %lld\n", BB_id(bb),
	    current_frame_size);
  }

  pushed_popped_here.clear();

  // if we've already visited this BB, stop recursion
  if (visited[BB_id(bb)]) {
    return;
  }

  // mark as visited
  visited[BB_id(bb)] = TRUE;

  for (ue_iter = ue_list.begin(); ue_iter != ue_list.end(); ++ue_iter) {
    if (ue_iter->bb != bb) continue;

    // If we have already seen this UE, no need to continue
    if (ue_iter->valid == FALSE)
      break;

    switch (ue_iter->kind) {
    case UE_CREATE_FRAME:
      current_frame_size += ue_iter->offset;
      if (Trace_Unwind) {
	Print_Unwind_Elem (*ue_iter, "frame_size");
      }
      break;

    case UE_DESTROY_FRAME:
      if (ue_iter->offset != 0) {
	current_frame_size -= ue_iter->offset;
      } else {
	// [CL] if offset is 0, it means we were unable to compute the
	// amount of stack which is restored. From this point, we
	// won't be able to assert other destroy_frame values, if they
	// occur.
	current_frame_size = 0;
	if (Trace_Unwind) {
	  Print_Unwind_Elem (*ue_iter, "unknown frame size");
	}
      }
      if (Trace_Unwind) {
	Print_Unwind_Elem (*ue_iter, "frame_size");
      }
      break;

    case UE_CREATE_FP:
    case UE_DESTROY_FP:
      break;

    case UE_SAVE_GR:
    case UE_SAVE_SP:
    case UE_SAVE_FP:
      p = ue_iter->rc_reg;
      if (current_frame_size && ue_iter->kind == UE_SAVE_SP) {
	ue_iter->top_offset = ue_iter->offset - current_frame_size;
	ue_iter->after_sp_adj = TRUE;
      } else {
	ue_iter->top_offset = ue_iter->offset;
      }

      if ( ! pr_stack[p].empty() ) {
	// Only the 1st one is valid, so if pr_track[p] is not empty,
	// this ue is invalid
	ue_iter->valid = FALSE;
	if (Trace_Unwind) {
	  Print_Unwind_Elem (*ue_iter, "invalid");
	}
    break;
      } else {
	if (Trace_Unwind) {
	  Print_Unwind_Elem (*ue_iter, "valid");
	}
      }
      pr_stack[p].push_front (*ue_iter);
      ue_iter->propagated = UE_PROP_PUSHED;
      pushed_popped_here.push_front(*ue_iter);
      break;


    case UE_RESTORE_GR:
    case UE_RESTORE_SP:
    case UE_RESTORE_FP:
      p = ue_iter->rc_reg;
      // FIXME if p was saved with SAVE_FP, maybe we should not use
      // current_frame_size. But so far there is no distinction
      // between a restore from SP and from FP
      if (current_frame_size && ue_iter->kind == UE_RESTORE_SP) {
	ue_iter->top_offset = ue_iter->offset - current_frame_size;
      } else {
	ue_iter->top_offset = ue_iter->offset;
      }

      if (!is_handler || !pr_stack[p].empty()) {
          UEsIt it = pr_stack[p].begin(), itPair;
          ue_iter->valid = FALSE;
          
          for(; !ue_iter->valid && it != pr_stack[p].end(); ++it) {
              // If current element is invalid, go to the next
              if(!it->valid) {
                  continue;
              }
              // this restore is not on the path of an exception handler, OR
              // if on such a path, the corresponding store is likely to
              // have already been processed (pr_strack[p] is not empty
              
              // check that this restore corresponds to the previous store. If
              // it does not, then ignore it (it is not related to callee-save
              // handling, it may be a definition of 'p')
              if (ue_iter->kind == UE_RESTORE_FP ||
                  ue_iter->kind == UE_RESTORE_SP) {
                  // Check that restore from memory corresponds to a save
                  // relative to SP or FP, and at the same offset
                  if ( ( (it->kind == UE_SAVE_SP)
                         || (it->kind == UE_SAVE_FP) )
                       && ue_iter->top_offset == it->top_offset ) {
                      ue_iter->valid = TRUE;
                      itPair = it;
                  }
              } else if (ue_iter->kind == UE_RESTORE_GR) {
                  // Check that restore from a register corresponds to a save
                  // to the same register
                  if (it->kind == UE_SAVE_GR
                      && ue_iter->save_rc_reg == it->save_rc_reg) {
                      ue_iter->valid = TRUE;
                      itPair = it;
                  }
              }
          }
          if(!ue_iter->valid) {
              if (Trace_Unwind) {
                  Print_Unwind_Elem (*ue_iter, "invalid");
              }
          } else {
              itPair->propagated = UE_PROP_POPPED;
              pushed_popped_here.push_front(*itPair);
              pr_stack[p].erase(itPair);
              if (Trace_Unwind) {
                  Print_Unwind_Elem (*ue_iter, "valid");
              }
          }
      } else if (is_handler) {
	// this restore in of the path of an exception handler and
	// pr_stack[p] is empty: this means that we are probably
	// handling a restore which corresponds to a store made during
	// the function prologue, which is not on this path.

	// FIXME: accept it for now. Maybe we could check we are in an
	// exit_bb (implying that exception handlers propably disable
	// some optimizations
	if (Trace_Unwind) {
	  Print_Unwind_Elem (*ue_iter, "valid");
	}

      } else {
	FmtAssert(!pr_stack[p].empty(),
		  ("unable to match restoring UNWIND_ELEM\n"));
      }
      
      break;
 
    default:
      break;
    }

    ue_iter->frame_size = current_frame_size;

    ue_iter->handler = is_handler ? TRUE : FALSE;
    if (Trace_Unwind && is_handler) {
      Print_Unwind_Elem (*ue_iter, "is_handler");
    }
  }

  frame_size_bb[BB_id(bb)] = current_frame_size;

  // Recursively analyze all the successors (except oneself)
  BBLIST *blst;
  INT bbid;
  FOR_ALL_BB_SUCCS (bb, blst) {
    bbid = BB_id(BBLIST_item(blst));
    INT64 saved_frame_size = current_frame_size;

    if (bbid != BB_id(bb)) {
      Tag_Irrelevant_Saves_And_Restores_For_BB(BBLIST_item(blst), pr_stack,
					       visited, frame_size_bb);
    }
    current_frame_size = saved_frame_size;
  }

  if (Trace_Unwind && ! pushed_popped_here.empty()) {
    fprintf(TFile, "back in BB %d after recursion: restoring state\n",
	    BB_id(bb));
  }

  // Restore the previous state to resume recursion
  while (! pushed_popped_here.empty()){
    UNWIND_ELEM ue;
    ue = pushed_popped_here.front();

    p = ue.rc_reg;
    if (ue.propagated == UE_PROP_PUSHED) {
      pr_stack[p].pop_front();
      if (Trace_Unwind) {
	Print_Unwind_Elem (ue, "pop");
      }
    } else if (ue.propagated == UE_PROP_POPPED) {
      pr_stack[p].push_front(ue);
      if (Trace_Unwind) {
	Print_Unwind_Elem (ue, "push");
      }
    }
    else {
      FmtAssert(FALSE, ("UE neither pushed nor popped\n"));
    }

    pushed_popped_here.pop_front();
  }
}


static void Tag_Irrelevant_Saves_And_Restores_BB_List(BB_LIST *elist,
						      BOOL* visited,
						      INT64* frame_size_bb)
{
  // Depth first search of the CFG in order to track the 'irrelevant'
  // stores and restores.

  // Do it recursively because the head of the list contains the last
  // block: in presence of exception handlers, the handlers are first
  // in the list, the function entry point is the last. This leads to
  // cases where the EH branches to the function epilogue, which is
  // studied before the function prologue; thus it cannot match
  // restores.

  // With this recursion, the EH are handled after the function
  // prologue.
  if (BB_LIST_rest(elist)) {
    Tag_Irrelevant_Saves_And_Restores_BB_List(BB_LIST_rest(elist),
					      visited, frame_size_bb);
  }

  // Use a stack to record the state of each PR along the CFG
  DbgIdToUEs pr_stack;

  BB* bb = BB_LIST_first(elist);

  current_frame_size = 0;

  if (BB_handler(bb)) {
    is_handler = TRUE;
  } else {
    is_handler = FALSE;
  }
  Tag_Irrelevant_Saves_And_Restores_For_BB(bb, pr_stack, visited,
					   frame_size_bb);
}

static void Tag_Irrelevant_Saves_And_Restores_TOP(INT64* frame_size_bb)
{
  BB_LIST *elist;
  BOOL* visited;

  visited = (BOOL *)alloca(sizeof(BOOL)*(PU_BB_Count+1));
  BZERO(visited, sizeof(BOOL)*(PU_BB_Count+1));

  Tag_Irrelevant_Saves_And_Restores_BB_List(Entry_BB_Head, visited, frame_size_bb);
}
#endif

// iterate thru list and mark all saves/restores in local state
static void
Mark_Local_Saves_Restores (VectorOfSetOfDbgId& local_save_state, 
                           VectorOfSetOfDbgId& local_restore_state)
{
  DebugRegId p;
  int i;
  for(i = 0; i <local_save_state.size(); ++i) {
      local_save_state[i].clear();
      local_restore_state[i].clear();
  }

#ifdef PROPAGATE_DEBUG
  // remove the invalid UEs from the list
  for (ue_iter = ue_list.begin(); ue_iter != ue_list.end(); ++ue_iter) {
    while (!ue_iter->valid) {
      if (Trace_Unwind) {
	Print_Unwind_Elem(*ue_iter, "erasing");
      }
      ue_iter = ue_list.erase(ue_iter);
      if (ue_iter == ue_list.end()) {
	break;
      }
    }
  }
#endif
  if (Trace_Unwind) {
    Print_All_Unwind_Elem ("after cleanup");
  }
  for (ue_iter = ue_list.begin(); ue_iter != ue_list.end(); ++ue_iter) {
	switch (ue_iter->kind) {
	case UE_CREATE_FRAME:
	case UE_CREATE_FP:
	case UE_SAVE_GR:
	case UE_SAVE_SP:
	case UE_SAVE_FP:
  		p = ue_iter->rc_reg;
  		Set_PR(local_save_state, ue_iter->bb, p);
		break;
	case UE_DESTROY_FRAME:
	case UE_DESTROY_FP:
	case UE_RESTORE_GR:
	case UE_RESTORE_SP:
	case UE_RESTORE_FP:
  		p = ue_iter->rc_reg;
  		Set_PR(local_restore_state, ue_iter->bb, p);
		break;
	}
  }
}

static void
Propagate_Save_Restore_State (VectorOfSetOfDbgId& entry_state,
                              VectorOfSetOfDbgId& local_save_state,
                              VectorOfSetOfDbgId& local_restore_state,
                              VectorOfSetOfDbgId& exit_state
#ifdef PROPAGATE_DEBUG
	,VectorOfDbgIdToPUE& bb_entry_ue,
	VectorOfDbgIdToPUE& bb_exit_ue,
	DbgIdToUEs& pr_last_info
#endif
)
{
  BB *bb;
  BBLIST *blst;
  INT bbid;
  BOOL changed = TRUE;
  INT count = 0;
  
#ifdef PROPAGATE_DEBUG
  DebugRegId p;
#endif
  SetOfDbgId savedList = PU_saved_dbgid;
  savedList.insert(Get_Debug_Reg_Id(SP_TN)); // SP must also be handled
  int i;
  for(i = 0; i < entry_state.size(); ++i) {
      entry_state[i].clear();
      exit_state[i] = savedList;
  }

  while (changed) {
	++count;
	if (count > 100) {	// to avoid infinite loops
		DevWarn("infinite loop in propagating unwind info");
		break;
	}
	changed = FALSE;
	for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
#ifndef USE_UNREACHABLE
	        if (BB_unreachable(bb)) continue;
#endif
		if (Trace_Unwind) 
			fprintf (TFile, "curbb: %d, preds: ", BB_id(bb));
		if (BB_preds(bb) != NULL) {
			SetOfDbgId new_entry_state = savedList;
#ifdef PROPAGATE_DEBUG
			UNWIND_ELEM* new_entry_ue[PR_LAST];
			memset(new_entry_ue, 0, PR_LAST * sizeof(UNWIND_ELEM*));
#endif
			FOR_ALL_BB_PREDS (bb, blst) {
				bbid = BB_id(BBLIST_item(blst));
				new_entry_state = SetOfDbgId_Intersection(new_entry_state,
                                                          exit_state[bbid]);
				if (Trace_Unwind) {
				  fprintf (TFile, "[BB%d ",
					   bbid);
				  Print_Set(TFile, exit_state[bbid]);
				  fprintf (TFile, "], ");
				}
#ifdef PROPAGATE_DEBUG
				// Do the equivalent of the &= line
				// above, for UEs
				DbgIdToPUECIt it;
				for (it = bb_exit_ue[bbid].begin();
				     it != bb_exit_ue[bbid].end(); ++it) {
				  DbgIdToPUECIt entryIt =
				    bb_entry_ue[BB_id(bb)].find(it->first);
				  // if 'p' has an exit state in bbid
				  // and no entry state in bb, then
				  // the entry state in bb is the exit
				  // state of bbid
				  if (entryIt ==  bb_entry_ue[BB_id(bb)].end()) {
				    bb_entry_ue[BB_id(bb)][it->first] = it->second;
				    if (Trace_Unwind) 
				      Print_Unwind_Elem(*(it->second),
							"entry_state");
				    // if there is already an entry
				    // state in bb, it should match
				    // the exit state of bbid
				  } else if (entryIt->second != it->second) {
				    if (Trace_Unwind) {
				      fprintf(TFile, "**UE do not match:\n");
				      Print_Unwind_Elem(*(entryIt->second), "");
				      Print_Unwind_Elem(*(it->second), "");
				    }
				  }
				}
#endif
			}
        		entry_state[BB_id(bb)] = new_entry_state;
		}
		bbid = BB_id(bb);
		if (Trace_Unwind) {
			fprintf (TFile, "\n entry_state: ");
			Print_Set(TFile, entry_state[bbid]);
			fprintf(TFile, "\n");
		}

		// A debug id is inserted in exit state if it belongs
		// to entry or local_save state and if it is not
		// locally restored.
		SetOfDbgId new_exit_state = SetOfDbgId_Union(entry_state[bbid],
                                                     local_save_state[bbid]);
		new_exit_state =
		  SetOfDbgId_Intersection(new_exit_state,
					  SetOfDbgId_Difference(savedList,
								local_restore_state[bbid]));
		if ((count == 1) || (new_exit_state != exit_state[bbid])) {
			// This code must be reached at least once (first iteration) to
			// correctly propagate bb_entry_ue to bb_exit_ue
			changed = TRUE;
			if (Trace_Unwind) {
			  fprintf(TFile, "changed from: ");
			  Print_Set(TFile, exit_state[bbid]);
			  fprintf(TFile, "\nto          : ");
			  Print_Set(TFile, new_exit_state);
			  fprintf(TFile, "'\n");
			}
			exit_state[bbid] = new_exit_state;
#ifdef PROPAGATE_DEBUG
			// do the equivalent of the above, for UEs
			CCIEInfo::CItSavedList itSaved;
			for (itSaved = savedList.begin(); itSaved != savedList.end();
			     ++itSaved) {
			  p = *itSaved;
			  // look for the UE that corresponds to bb
			  UEsIt ue_for_p_iter;
			  for(ue_for_p_iter = pr_last_info[p].begin();
			      ue_for_p_iter != pr_last_info[p].end();
			      ue_for_p_iter++) {
			    if (ue_for_p_iter->bb == bb) break;
			  }
			  if (Get_PR(local_save_state[bbid],p)
			      || Get_PR(local_restore_state[bbid],p)) {
			    if (ue_for_p_iter == pr_last_info[p].end()) {
			      FmtAssert(FALSE, ("no UE found -- inconsistent!\n"));
			    }
			    bb_exit_ue[bbid][p] = &(*ue_for_p_iter);
			  } else if(bb_entry_ue[bbid].find(p) != bb_entry_ue[bbid].end()) {
			    bb_exit_ue[bbid][p] = bb_entry_ue[bbid][p];
			  }
			  DbgIdToPUECIt it = bb_exit_ue[bbid].find(p);
			  if (it != bb_exit_ue[bbid].end()) {
			    if (Trace_Unwind) {
			      Print_Unwind_Elem(*(it->second), "exit_state");
			    }
			  }
			}
#endif
		}
		if (Trace_Unwind) {
		  fprintf (TFile, " exit_state: ");
		  Print_Set(TFile, exit_state[bbid]);
		  fprintf(TFile, "\n");
		}

	}
  }
  if (Trace_Unwind) {
	for (INT i = 1; i <= PU_BB_Count; ++i) {
		fprintf(TFile, "BB%4d:\nentry: ", i);
		Print_Set(TFile, entry_state[i]);
		fprintf(TFile, "\nlocal_save: ");
		Print_Set(TFile, local_save_state[i]);
		fprintf(TFile, "\nlocal_restore: ");
		Print_Set(TFile, local_restore_state[i]);
		fprintf(TFile, "\nexit: ");
		Print_Set(TFile, exit_state[i]);
		fprintf(TFile, "\n");
	}
  }
}

static UNWIND_ELEM
Find_Prev_Save_UE_For_BB (UEs prev_ue, BB *bb, UINT level)
{
  BBLIST *blst;
  UEsIt prev_iter;
  FOR_ALL_BB_PREDS (bb, blst) {
	// find ue in nbb that does a save
  	for (prev_iter = prev_ue.begin(); prev_iter != prev_ue.end(); ++prev_iter) {
		if (prev_iter->bb != BBLIST_item(blst)) continue;
		if (prev_iter->kind == UE_SAVE_GR
		    || prev_iter->kind == UE_SAVE_SP
		    || prev_iter->kind == UE_SAVE_FP) {
			return *prev_iter;
		}
	}
  }
  UNWIND_ELEM ue;
  ue.kind = UE_UNDEFINED;
  ++level;
  if (level > 10) return ue;
  // if not found recurse
  FOR_ALL_BB_PREDS (bb, blst) {
	ue = Find_Prev_Save_UE_For_BB (prev_ue, BBLIST_item(blst), level);
	if (ue.kind != UE_UNDEFINED) return ue;
  }
  return ue;
}

#ifdef PROPAGATE_DEBUG
// Prepend a copy of *pue at current ue_iter position.
// This used in presence of COPY_STATE elements
// The copied UE has updated bb and when fields
static void Prepend_UE(UNWIND_ELEM* pue, UINT when, BB *bb)
{
  UNWIND_ELEM ue;
  ue.when = when;
  ue.bb = bb;
  ue.kind = pue->kind;
  ue.qp = pue->qp;
  ue.rc_reg = pue->rc_reg;
  ue.save_rc_reg = pue->save_rc_reg;
  ue.offset = pue->offset;
  ue.valid = TRUE;
  ue.top_offset = pue->top_offset;
  ue.is_copy = TRUE;
  ue.after_sp_adj = pue->after_sp_adj;
  ue.label_idx = LABEL_IDX_ZERO;
  ue.frame_changed = pue->frame_changed;
  ue.frame_size = pue->frame_size;
  ue.handler = BB_handler(bb);

  ue_list.insert(ue_iter, ue);
  if (Trace_Unwind) {
	fprintf(TFile, "prepend ue kind %d at bb %d when %d\n",
		ue.kind, BB_id(bb), when);
  }
}
#endif

// overload some routines to add unwind elements
static void
#ifdef PROPAGATE_DEBUG
Add_UE (UEs prev_ue, DebugRegId p, UINT when, BB *bb, INT64 frame_size, BOOL is_handler)
#else
Add_UE (UEs prev_ue, DebugRegId p, UINT when, BB *bb)
#endif
{
  UEsIt prev_iter;
  UNWIND_ELEM ue;
  ue.kind = UE_UNDEFINED;
#ifdef PROPAGATE_DEBUG
  ue.valid = TRUE;
  ue.top_offset = 0;
  ue.is_copy = FALSE;
  ue.after_sp_adj = FALSE;
  ue.frame_size = frame_size;
  ue.handler = is_handler;
#endif
  ue.label_idx = LABEL_IDX_ZERO;
  ue.save_rc_reg =  DebugRegId_undef;
  UINT num_found = 0;
  for (prev_iter = prev_ue.begin(); prev_iter != prev_ue.end(); ++prev_iter) {
	// look for save
	if (prev_iter->kind == UE_SAVE_GR
	    || prev_iter->kind == UE_SAVE_SP
	    || prev_iter->kind == UE_SAVE_FP) {
		ue = *prev_iter;
		++num_found;
	}
  }
  if (num_found == 0) {
	DevWarn("unwind: no pr_info found for %lld", p);
	return;
  }
  if (num_found > 1) {
	// check if all are same
  	for (prev_iter = prev_ue.begin(); prev_iter != prev_ue.end(); ++prev_iter) {
		if (prev_iter->kind == ue.kind 
		  && prev_iter->rc_reg == ue.rc_reg)
			--num_found;
	}
	++num_found;	// original still counts
  }
  if (num_found > 1) {
	UNWIND_ELEM nue;
	nue = Find_Prev_Save_UE_For_BB (prev_ue, bb, 0);
	if (nue.kind == UE_UNDEFINED) {
		// just use memory save if exists
  		for (prev_iter = prev_ue.begin(); prev_iter != prev_ue.end(); ++prev_iter) {
			if (prev_iter->kind == UE_SAVE_SP
			    || prev_iter->kind == UE_SAVE_FP) {
				nue = *prev_iter;
				break;
			}
		}
		if (nue.kind == UE_UNDEFINED) {
			DevWarn("couldn't find unwind save for %lld", p);
		}
		else
			ue = nue;
	}
	else
		ue = nue;
  }
  ue.when = when;
  ue.bb = bb;
  ue_list.insert(ue_iter, ue);
  if (Trace_Unwind) {
	fprintf(TFile, "state change for %lld at entry to bb %d when %d\n",
            (long long)p, BB_id(bb), when);
  }
}

static void
#ifdef PROPAGATE_DEBUG
Add_UE (INT8 kind, DebugRegId p, UINT when, BB *bb, INT64 frame_size, BOOL is_handler)
#else
Add_UE (INT8 kind, DebugRegId p, UINT when, BB *bb)
#endif
{
  UNWIND_ELEM ue;
#ifdef PROPAGATE_DEBUG
  ue.valid = TRUE;
  ue.top_offset = 0;
  ue.is_copy = FALSE;
  ue.after_sp_adj = FALSE;
  ue.frame_size = frame_size;
  ue.handler = is_handler;
#endif
  ue.label_idx = LABEL_IDX_ZERO;
  ue.save_rc_reg =  DebugRegId_undef;
  ue.kind = kind;
  ue.qp = 0;
  ue.rc_reg = p;
  ue.when = when;
  ue.bb = bb;
  ue.frame_changed = FALSE;

  ue_list.insert(ue_iter, ue);
  if (Trace_Unwind) 
	fprintf(TFile, "state change for %lld at entry to bb %d when %d\n",
            (long long)ue.rc_reg, BB_id(bb), when);
}

static void
#ifdef PROPAGATE_DEBUG
Add_UE (INT8 kind, UINT label, UINT when, BB *bb, INT64 frame_size, BOOL is_handler)
#else
Add_UE (INT8 kind, UINT label, UINT when, BB *bb)
#endif
{
  UNWIND_ELEM ue;
#ifdef PROPAGATE_DEBUG
  ue.valid = TRUE;
  ue.top_offset = 0;
  ue.is_copy = FALSE;
  ue.after_sp_adj = FALSE;
  ue.frame_size = frame_size;
  ue.handler = is_handler;
#endif
  ue.label_idx = LABEL_IDX_ZERO;
  ue.save_rc_reg =  DebugRegId_undef;
  ue.kind = kind;
  ue.label = label;
  ue.when = when;
  ue.bb = bb;
  ue.frame_changed = FALSE;

  ue_list.insert(ue_iter, ue);
  if (Trace_Unwind) 
	fprintf(TFile, "add ue kind %d label %d at bb %d when %d\n",
		kind, label, BB_id(bb), when);
}

static void
Do_Control_Flow_Analysis_Of_Unwind_Info (void)
{
  // we know what unwind changes happen in each block;
  // now have to propagate that info so each bb has correct info upon entry.

  // have 4 bit vectors for each bb:  
  // entry, local-save, local-restore, and exit state.
  // first fill in the local state info with changes that happen in that bb.

  VectorOfSetOfDbgId entry_state;
  VectorOfSetOfDbgId local_save_state;
  VectorOfSetOfDbgId local_restore_state;
  VectorOfSetOfDbgId exit_state;
  int i;
  for(i = 0; i < PU_BB_Count+1; ++i) {
      entry_state.push_back(SetOfDbgId(CmpDbgId(),
                                       DbgIdAllocator(&dw_Mem_Pool)));
      local_save_state.push_back(SetOfDbgId(CmpDbgId(),
                                            DbgIdAllocator(&dw_Mem_Pool)));
      local_restore_state.push_back(SetOfDbgId(CmpDbgId(),
                                               DbgIdAllocator(&dw_Mem_Pool)));
      exit_state.push_back(SetOfDbgId(CmpDbgId(),
                                      DbgIdAllocator(&dw_Mem_Pool)));
  }

#ifdef PROPAGATE_DEBUG
  INT64 frame_size_bb[PU_BB_Count+1];
  // [CL] Incorrect saves/restores may have been recorded by
  // Find_Unwind_Info(). Indeed, it has recorded all saves/restores
  // to/from callee-saved registers, some of which may not be related
  // to this ABI property.
  Tag_Irrelevant_Saves_And_Restores_TOP(frame_size_bb);
  UE_validated = TRUE;
  BOOL is_handler = FALSE;
#endif

  // mark all saves/restores in local state
  Mark_Local_Saves_Restores (local_save_state, local_restore_state);

#ifdef PROPAGATE_DEBUG
  DebugRegId p;
  // keep list of ue's for each pr.
  DbgIdToUEs pr_last_info;
  for (ue_iter = ue_list.begin(); ue_iter != ue_list.end(); ++ue_iter) {
		p = ue_iter->rc_reg;
		// put last ue for bb on list
		if ( ! pr_last_info[p].empty()
		    && pr_last_info[p].front().bb == ue_iter->bb)
		{
			pr_last_info[p].pop_front();
		}
		pr_last_info[p].push_front (*ue_iter);
  }

  // for each BB, keep ue for each pr on BB's exit
  VectorOfDbgIdToPUE bb_exit_ue(PU_BB_Count + 1);
  VectorOfDbgIdToPUE bb_entry_ue(PU_BB_Count + 1);
#endif

  // now propagate the save/restore state thru the control flow.
  // foreach pred bb, copy its exit-state to the entry-state.
#ifndef PROPAGATE_DEBUG
  Propagate_Save_Restore_State (entry_state, local_save_state,
				local_restore_state, exit_state);
#else
  Propagate_Save_Restore_State (entry_state, local_save_state,
				local_restore_state, exit_state,
				bb_entry_ue, bb_exit_ue, pr_last_info);
#endif

#ifndef PROPAGATE_DEBUG // moved above
  DebugRegId p;
  // keep list of ue's for each pr.
  DbgIdToUEs pr_last_info;
  for (ue_iter = ue_list.begin(); ue_iter != ue_list.end(); ++ue_iter) {
		p = ue_iter->rc_reg;
		// put last ue for bb on list
		if ( ! pr_last_info[p].empty()
		    && pr_last_info[p].front().bb == ue_iter->bb)
		{
			pr_last_info[p].pop_front();
		}
		pr_last_info[p].push_front (*ue_iter);
  }
#endif

  // now determine save/restore changes at each when point
  // and update ue_list with changes
  SetOfDbgId current_state =
      SetOfDbgId(CmpDbgId(), DbgIdAllocator(&dw_Mem_Pool));
  INT bbid;
  UINT lwhen = 0;
  ue_iter = ue_list.begin();
  for (BB *bb = REGION_First_BB; bb; bb = BB_next(bb)) {
#ifdef PROPAGATE_DEBUG
    INT64 last_frame_size = 0;
#endif

#ifndef USE_UNREACHABLE
	if (BB_unreachable(bb)) {
		lwhen += Get_BB_When_Length(bb); 
		continue;
	}
#endif
	if (BB_length(bb) == 0) {
		// empty, so ignore
		continue;
	}
	bbid = BB_id(bb);

	// if an alternate entry point,
	// reset current state so no implicit changes at entry.
	if (BB_entry(bb)) {
		current_state = entry_state[bbid];
	}
	// in case have exit that follows exit,
	// first copy previous label then do new label.
	if (BB_prev(bb) != NULL && BB_exit(BB_prev(bb))) {
		// in bb that follows exit, so copy above label
		// Add_UE (UE_COPY, last_label, lwhen, bb);
		// current_state = entry_state[BB_id(BB_prev(bb))];
#ifdef PROPAGATE_DEBUG
		// explicitly add all the UEs required to restore the
		// desired state

		// FIXME: for an EH BB, the state of the pred is 0.
		// TODO: propagate bottom-up the entry states
		INT pred_id;
		if (BB_First_Pred(bb)) {
		  pred_id = BB_id(BB_First_Pred(bb));
		} else {
		  FmtAssert(BB_handler(bb), ("BB %d has no predecessor and is not EH", BB_id(bb)));
		  pred_id = BB_id(BB_prev(bb));
		}
		if (Trace_Unwind) 
		  fprintf(TFile, "copying exit state from bb %d\n", pred_id);

		DbgIdToPUECIt it;
		for(it = bb_exit_ue[pred_id].begin();
		    it != bb_exit_ue[pred_id].end(); ++it) {
		  Prepend_UE(it->second, lwhen, bb);
		}
		current_state = exit_state[pred_id];
#endif
	}

	if (BB_exit(bb) && BB_next(bb) != NULL) {
		// if have an exit that is followed by another bb
		// then want to create body label before exit and
		// copy from label after exit (i.e. skip destroy frame)
		if (BB_entry(bb) && BB_prev(bb) == NULL) {
			// if in first bb, then make sure label
			// comes after create_frame.
			while (ue_iter != ue_list.end() && ue_iter->bb == bb) {
				if (ue_iter->kind == UE_CREATE_FRAME) {
				        last_frame_size = ue_iter->frame_size;
					++ue_iter;
					Add_UE (UE_LABEL, ++last_label, 
#ifdef PROPAGATE_DEBUG
						ue_iter->when, bb, last_frame_size,
						is_handler);
#else
						ue_iter->when, bb);
#endif
					break;
				}
				++ue_iter;
			}
		}
		else {
#ifdef PROPAGATE_DEBUG
		        last_frame_size = ue_iter->frame_size;
			Add_UE (UE_LABEL, ++last_label, lwhen, bb, last_frame_size,
				is_handler);
#else
			Add_UE (UE_LABEL, ++last_label, lwhen, bb);
#endif
		}
	}

  if (Trace_Unwind) {
	fprintf (TFile, "current_state for bb %d: ", bbid);
	Print_Set(TFile, current_state);
	fprintf(TFile, "\n");
  }

	// add implicit changes upon entry
#ifdef PROPAGATE_DEBUG
	if (BB_handler(bb)) {
	  // FIXME If bb is an EH entry block, keep the just-computed
	  // entry_state. Indeed, the proper entry state of such a BB
	  // has not been computed accurately (and is zero currently,
	  // as we still need a bottom-up propagation phase) //
	  // entry_state[bbid] = current_state; this is wrong because
	  // it should also be propagated to the following BBs
	  current_state = entry_state[bbid];

	  is_handler = TRUE;
	} else if (BB_entry(bb)) {
	  is_handler = FALSE;
	}
#ifndef USE_UNREACHABLE
	else if (BB_unreachable(bb)) {
	  // FIXME
	  current_state = entry_state[bbid];
	}
#endif
#endif
	if (current_state != entry_state[bbid]) {
	    CCIEInfo::CItSavedList itSaved;
	    for (itSaved = PU_saved_dbgid.begin(); itSaved != PU_saved_dbgid.end();
		 ++itSaved) {
			p = *itSaved;
			// ignore implicit sp changes,
			// as label/copy should handle those.
			if (p == Get_Debug_Reg_Id(SP_TN)) continue;
			if (Get_PR(current_state,p) != Get_PR(entry_state[bbid],p)) 
			{
				// add into ue_list
				if (Get_PR(entry_state[bbid],p) == FALSE) {
					// add restore
#ifdef PROPAGATE_DEBUG
					Add_UE (UE_RESTORE_GR, p, lwhen, bb,
						frame_size_bb[BB_id(BBLIST_item(BB_preds(bb)))],
						ue_iter->handler);
#else
					Add_UE (UE_RESTORE_GR, p, lwhen, bb);
#endif
				}
				else {
					// add save
#ifdef PROPAGATE_DEBUG
					Add_UE (pr_last_info[p], p, lwhen, bb,
						frame_size_bb[BB_id(BBLIST_item(BB_preds(bb)))],
						ue_iter->handler);
#else
					Add_UE (pr_last_info[p], p, lwhen, bb);
#endif
				}
			}
	    }
	    current_state = entry_state[bbid];
	}

	// look for changes in bb
	while (ue_iter != ue_list.end() && ue_iter->bb == bb) {
		p = ue_iter->rc_reg;
		if (Trace_Unwind) {
		  fprintf(TFile, 
			  "state change for %lld in bb %d\n",
			  (long long)ue_iter->rc_reg, bbid);
		}
		if (ue_iter->kind == UE_RESTORE_GR
		    || ue_iter->kind == UE_RESTORE_SP
		    || ue_iter->kind == UE_RESTORE_FP)
		{
			Clear_PR(current_state, p);
		}
		else {
			Set_PR(current_state, p);
		}
		++ue_iter;
	}
	lwhen += Get_BB_When_Length(bb);

	if (Trace_Unwind) {
	  fprintf (TFile, "final current_state for bb %d: ", bbid);
	  Print_Set(TFile, current_state);
	  fprintf (TFile, "\n");
	}

  }
}

static void
Insert_Epilogs (void)
{
#if 0
  UEsIt prev_ue;
  UNWIND_ELEM ue;
  for (ue_iter = ue_list.begin(); ue_iter != ue_list.end(); ++ue_iter) {
    switch (ue_iter->kind) {
    case UE_DESTROY_FRAME:
	// go backwards, until first restore in exit bb
	prev_ue = ue_iter;
	--prev_ue;
	while (prev_ue != ue_list.begin()) {
		if (prev_ue->bb != ue_iter->bb) break;
		if (prev_ue->kind != UE_RESTORE_GR && prev_ue->kind != UE_RESTORE_SP && prev_ue->kind != UE_RESTORE_FP)
			break;
		--prev_ue;
	}
	++prev_ue;
  	ue.kind = UE_EPILOG;
  	ue.when = prev_ue->when;
  	ue.bb = prev_ue->bb;
#ifdef PROPAGATE_DEBUG
	ue.is_copy = FALSE;
	ue.handler = prev_ue->handler;
#endif
  	ue_list.insert(prev_ue, ue);
  	if (Trace_Unwind) 
		fprintf(TFile, "add epilog at bb %d, when %d\n", BB_id(ue.bb), ue.when);
    }
  }
#endif
}

static UINT next_when;

static INT Idx_save_ra = 0;
static INT Idx_restore_ra = 0;
static INT Idx_adjust_sp = 0;
static INT Idx_restore_sp = 0;
static INT Idx_adjust_fp = 0;
static INT Idx_restore_fp = 0;
static INT Idx_save_csr = 0;
static INT Idx_restore_csr = 0;

// call per-PU
void 
Init_Unwind_Info (BOOL trace)
{
  if (!OPTIONS_CHECK) return;
  MEM_POOL_Initialize(&dw_Mem_Pool, "Unwind generation", TRUE);
  MEM_POOL_Push (&dw_Mem_Pool);

  Trace_Unwind = trace;
  has_asm = FALSE;
  PU_has_FP = FALSE;
  PU_has_restored_FP = FALSE;
  UE_validated = FALSE;

  if (Get_Trace (TP_EMIT, 128)) draw_flow_graph();

  Compute_Saved_DbgId();

  Find_Unwind_Info ();

  if (Trace_Unwind) {
	Print_All_Unwind_Elem ("unwind0");
  }

  last_label = 0;
  next_when = 0;

  if (Trace_Unwind) fprintf (TFile, "propagating unwind info\n");
  // need to propagate unwind info to each block,
  // and update ue_list with state changes
  Do_Control_Flow_Analysis_Of_Unwind_Info ();

  Insert_Epilogs();

  if (Trace_Unwind) {
	Print_All_Unwind_Elem ("unwind2");
  }

#ifdef DEBUG_UNWIND
  Print_All_Unwind_Elem("** unwind");
#endif

  // for use in emit_unwind
  ue_iter = ue_list.begin();

  Idx_save_ra = 0;
  Idx_restore_ra = 0;
  Idx_adjust_sp = 0;
  Idx_restore_sp = 0;
  Idx_adjust_fp = 0;
  Idx_restore_fp = 0;
  Idx_save_csr = 0;
  Idx_restore_csr = 0;
}

void 
Finalize_Unwind_Info(void)
{
  if (!OPTIONS_CHECK) return;

  ue_list.clear();
  MEM_POOL_Pop(&dw_Mem_Pool);
  MEM_POOL_Delete(&dw_Mem_Pool);

}


static void Generate_Label_For_Unwinding2(LABEL_IDX* label, INT* idx, char* txt,
				      UNWIND_ELEM& ue_iter, BOOL post_process)
{
  LABEL* tmp_lab;
  char* buf;
  if (*label == LABEL_IDX_ZERO) {
    buf = (char *)alloca(strlen(Cur_PU_Name) + 
			 /* EXTRA_NAME_LEN */ 32);
    sprintf(buf, ".LEH_%s%s_%s_%d",
	    post_process ? "post_" : "", txt, Cur_PU_Name, *idx);
    tmp_lab = &New_LABEL(CURRENT_SYMTAB, *label);
    LABEL_Init (*tmp_lab, Save_Str(buf), LKIND_DEFAULT);
    fprintf( Asm_File, "%s:\n", LABEL_name(*label));

    // once we have emitted the pre-op and post-op labels,
    // we can update the counter
    if (post_process) {
      (*idx)++;
    }
  }
  // remember the label we have generated after 'op':
  // this is the one we need to update the debug_frame info

  if (post_process) {
    ue_iter.label_idx = *label;
  }
}

static void Generate_Label_For_Unwinding(LABEL_IDX* label, INT* idx, char* txt,
				      UNWIND_ELEM& ue_iter, BOOL post_process)
{
#ifdef PROPAGATE_DEBUG
  if (!ue_iter.is_copy) {
#endif
    Generate_Label_For_Unwinding2(label, idx, txt, ue_iter, post_process);
#ifdef PROPAGATE_DEBUG
  } else {
    if (!post_process) {
      LABEL_IDX already_generated_lab = *label;
      Generate_Label_For_Unwinding2(label, idx, txt, ue_iter, FALSE);
      *label = already_generated_lab;
      Generate_Label_For_Unwinding2(label, idx, txt, ue_iter, TRUE);
    }
  }
#endif
}

void 
Emit_Unwind_Directives_For_OP(OP *op, FILE *f, BOOL post_process,
			      BOOL inserted_late)
{
  char prefix[3];
  // remember when we finish exploring a bundle
  // so that we can rewind 'next_when' before the post-pass
  static BOOL saved_state = TRUE;
  static UINT saved_next_when;
  static UINT when_bundle_start = 0; // address of the beginning of
				     // the bundle. used later in
				     // Create_Unwind_Descriptors() in
				     // order to generate
				     // DW_CFA_advance_loc insn only
				     // when necessary
  static UEsIt saved_ue_iter;

  // Remember if we create/destroy the frame inside this bundle. This
  // is necessary to handle the case where the frame is created and
  // and a callee-saved is saved to the stack in the same bundle: the
  // offset encoded in the callee-saved save insn is an offset
  // relative to SP before bundle execution, while we want to record
  // the offset after execution, when SP has changed too
  static BOOL bundle_has_frame_change = FALSE;
  INT64 frame_size = 0;

  BOOL emit_directives = TRUE;

  static LABEL_IDX Label_save_ra;
  static LABEL_IDX Label_restore_ra;
  static LABEL_IDX Label_adjust_sp;
  static LABEL_IDX Label_restore_sp;
  static LABEL_IDX Label_adjust_fp;
  static LABEL_IDX Label_restore_fp;
  static LABEL_IDX Label_save_csr;
  static LABEL_IDX Label_restore_csr;

  Label_save_ra     = LABEL_IDX_ZERO;
  Label_restore_ra  = LABEL_IDX_ZERO;
  Label_adjust_sp   = LABEL_IDX_ZERO;
  Label_restore_sp  = LABEL_IDX_ZERO;
  Label_adjust_fp   = LABEL_IDX_ZERO;
  Label_restore_fp  = LABEL_IDX_ZERO;
  Label_save_csr    = LABEL_IDX_ZERO;
  Label_restore_csr = LABEL_IDX_ZERO;

  if (!OPTIONS_CHECK) return;

  // If this OP was inserted after unwind elements computation (eg
  // simulated_op), don't take it into account
  if (inserted_late) {
    return;
  }

  if (!saved_state && post_process) {
    // we start post-processing this bundle, so rewind next_when
    next_when = saved_next_when;
    ue_iter = saved_ue_iter;
  }
  else if (saved_state && !post_process) {
    // we start pre-processing a new bundle, so update saved_next_when
    saved_next_when = next_when;
    saved_ue_iter = ue_iter;
    when_bundle_start = next_when;
    // clear the create_frame
    bundle_has_frame_change = FALSE;
  }

  saved_state = post_process;

  if ( CG_emit_unwind_directives) {
	strcpy(prefix, "");
	emit_directives = TRUE;
  } else {
	strcpy(prefix, ASM_CMNT_LINE);	// emit as comments
	emit_directives = List_Notes;
  }

  if (OP_dummy(op)) return;

  while (ue_iter != ue_list.end() && ue_iter->when == next_when) {

    ue_iter->when_bundle_start = when_bundle_start;

    switch (ue_iter->kind) {
    case UE_CREATE_FRAME:
      if (!ue_iter->is_copy) {
        bundle_has_frame_change = TRUE;
      }
      frame_size += ue_iter->offset;

      Generate_Label_For_Unwinding(&Label_adjust_sp, &Idx_adjust_sp,
				   "adjust_sp", *ue_iter, post_process);
      break;

    case UE_DESTROY_FRAME:
      if (!ue_iter->is_copy) {
        bundle_has_frame_change = TRUE;
      }
      // [CL] offset == 0 means we were unable to compute the amount
      // of stack restored here. frame_size == 0 means we have already
      // encountered a UE with offset==0, and we can no longer assert
      // partial frame restore values.
      // Thus, if either offset==0 or frame_size==0, keep frame_size==0;
      if ( (ue_iter->offset != 0) && (frame_size !=0) ) {
	frame_size -= ue_iter->offset;
      } else {
	frame_size = 0;
      }

      Generate_Label_For_Unwinding(&Label_restore_sp, &Idx_restore_sp,
				   "restore_sp", *ue_iter, post_process);
      break;

    case UE_CREATE_FP:
      Generate_Label_For_Unwinding(&Label_adjust_fp, &Idx_adjust_fp,
				   "adjust_fp", *ue_iter, post_process);
      break;

    case UE_DESTROY_FP:
      Generate_Label_For_Unwinding(&Label_restore_fp, &Idx_restore_fp,
				   "restore_fp", *ue_iter, post_process);
      break;

    case UE_SAVE_SP:
    case UE_SAVE_FP:
        if (ue_iter->rc_reg ==
            Get_Debug_Reg_Id(CLASS_REG_PAIR_ra)) {
	Generate_Label_For_Unwinding(&Label_save_ra, &Idx_save_ra,
				   "save_ra", *ue_iter, post_process);
      } else {
	Generate_Label_For_Unwinding(&Label_save_csr, &Idx_save_csr,
				   "save_csr", *ue_iter, post_process);
      }
        if (!ue_iter->is_copy) {
          // adjust offset if SP is adjusted in the same bundle
          if (post_process && bundle_has_frame_change) {
            ue_iter->offset += frame_size;
          }

          /* flag this ue in order to adjust the offset later */
          if (bundle_has_frame_change) {
            ue_iter->frame_changed = TRUE;
          }
        }
      break;

    case UE_RESTORE_SP:
    case UE_RESTORE_FP:
      if (ue_iter->rc_reg ==
          Get_Debug_Reg_Id(CLASS_REG_PAIR_ra)) {
	Generate_Label_For_Unwinding(&Label_restore_ra, &Idx_restore_ra,
				   "restore_ra", *ue_iter, post_process);
      } else {
	Generate_Label_For_Unwinding(&Label_restore_csr, &Idx_restore_csr,
				   "restore_csr", *ue_iter, post_process);
      }

      if (!ue_iter->is_copy) {
        // adjust offset if SP is adjusted in the same bundle
        if (post_process && bundle_has_frame_change) {
          ue_iter->offset -= frame_size;
        }

        /* flag this ue in order to adjust the offset later */
        if (bundle_has_frame_change) {
          ue_iter->frame_changed = TRUE;
        }
      }
      break;

    case UE_EPILOG:
      if (emit_directives) fprintf(f, "%s\t.body\n", prefix);
      break;

    case UE_LABEL:
      if (emit_directives) {
	fprintf(f, "%s\t.body\n", prefix);
	fprintf(f, "%s\t.label_state %d\n", prefix, ue_iter->label);
      }
      break;

    case UE_COPY:
      if (emit_directives) {
	fprintf(f, "%s\t.body\n", prefix);
	fprintf(f, "%s\t.copy_state %d\n", prefix, ue_iter->label);
      }
      break;

    case UE_RESTORE_GR:
      if (ue_iter->qp != 0) {
	if (emit_directives) {
	  fprintf(f, "%s\t.restorereg.p p%d, %lld\n", prefix, 
		  ue_iter->qp, ue_iter->rc_reg);
	}
      }
      else {
          if (ue_iter->rc_reg ==
              Get_Debug_Reg_Id(CLASS_REG_PAIR_ra)) {
	  Generate_Label_For_Unwinding(&Label_restore_ra, &Idx_restore_ra,
				       "restore_ra", *ue_iter, post_process);
	} else {
	  Generate_Label_For_Unwinding(&Label_restore_csr, &Idx_restore_csr,
				       "restore_csr", *ue_iter, post_process);
	}
      }
      break;
    case UE_SAVE_GR:
      if (ue_iter->qp != 0) {
	if (emit_directives) {
	  fprintf(f, "%s\t%s p%d, %lld, %lld\n", prefix, ".spillreg.p",
              ue_iter->qp, (long long)ue_iter->rc_reg,
              (long long)ue_iter->save_rc_reg);
	}
      }
      else {

          if (ue_iter->rc_reg ==
              Get_Debug_Reg_Id(CLASS_REG_PAIR_ra)) {
	  Generate_Label_For_Unwinding(&Label_save_ra, &Idx_save_ra,
				       "save_ra", *ue_iter, post_process);
	} else {
	  Generate_Label_For_Unwinding(&Label_save_csr, &Idx_save_csr,
				       "save_csr", *ue_iter, post_process);
	}
      }
      break;

    default:
      FmtAssert(FALSE, ("Unhandled UNWIND_ELEM kind (%d)\n", ue_iter->kind));
      /*NOTREACHED*/
    }

    ++ue_iter;
  }

  next_when += OP_Real_Unit_Slots(op);
}


extern char *Cg_Dwarf_Name_From_Handle(Dwarf_Unsigned idx);

// process info we've collected and create the unwind descriptors
static void
Create_Unwind_Descriptors (Dwarf_P_Fde fde, Elf64_Word	scn_index,
			   Dwarf_Unsigned begin_label, bool emit_restores)
{
  Dwarf_Error dw_error;
  INT64 frame_size = 0;
  INT64 frame_offset = 0;

  INT64 current_loc = -1;  // current address

  // record the reference label for advance_loc insns
  Dwarf_Unsigned last_label_idx = begin_label;

#ifdef DEBUG_UNWIND
  fprintf(TFile, "** %s init label %s\n", __FUNCTION__,
	  Cg_Dwarf_Name_From_Handle(begin_label));
#endif

  for (ue_iter = ue_list.begin(); ue_iter != ue_list.end(); ++ue_iter) {

#ifdef PROPAGATE_DEBUG
    // if this UE is a copy state, pretend its adress is the previous one
    if (ue_iter->is_copy) {
      ue_iter->when_bundle_start--;
    }
#endif

    frame_size = ue_iter->frame_size;

    switch (ue_iter->kind) {
    case UE_EPILOG:
    case UE_LABEL:
    case UE_COPY:
      continue;

    case UE_CREATE_FRAME:
      if (!ue_iter->handler) {
	if (frame_size <= 0 && Trace_Unwind) {
	  Print_Unwind_Elem(*ue_iter, "bad frame size");
	}
	Is_True(frame_size > 0,
		("unwind: frame size should be > 0 at creation point"));
      }

      if (ue_iter->when_bundle_start > current_loc) {
	dwarf_add_fde_inst(fde, DW_CFA_advance_loc4, last_label_idx,
			   Cg_Dwarf_Symtab_Entry(CGD_LABIDX,
						 ue_iter->label_idx,
						 scn_index),
			   &dw_error);
      }
      dwarf_add_fde_inst(fde, DW_CFA_def_cfa_offset,
			 frame_size + STACK_OFFSET_ADJUSTMENT,
			 0, &dw_error);
      if (Trace_Unwind) {
	fprintf(TFile, "create stack frame of size %lld at when %d\n",
		frame_size, ue_iter->when);
      }

#ifdef DEBUG_UNWIND
      fprintf(TFile, "** %s label %s to %s advance loc + create frame %lld\n",
	      __FUNCTION__, LABEL_name(ue_iter->label_idx),
	      Cg_Dwarf_Name_From_Handle(last_label_idx), frame_size);
#endif
      break;

    case UE_DESTROY_FRAME:
      if (!emit_restores) continue;

      if (!ue_iter->handler) {
	if (frame_size < 0 && Trace_Unwind) {
	  Print_Unwind_Elem(*ue_iter, "bad frame size");
	}
	Is_True(frame_size >= 0,
		("unwind: bad frame size at destroy point %d remaining after destruction of %d", frame_size, ue_iter->offset));
      }

      if (ue_iter->when_bundle_start > current_loc) {
	dwarf_add_fde_inst(fde, DW_CFA_advance_loc4, last_label_idx,
			   Cg_Dwarf_Symtab_Entry(CGD_LABIDX,
						 ue_iter->label_idx,
						 scn_index),
			   &dw_error);
      }
      dwarf_add_fde_inst(fde, DW_CFA_def_cfa_offset, 
			 frame_size + STACK_OFFSET_ADJUSTMENT,
			 0, &dw_error);

      if (Trace_Unwind) {
	fprintf(TFile, "destroy stack frame of size %lld at when %d, remaining %lld\n",
		ue_iter->offset, ue_iter->when, frame_size);
      }

#ifdef DEBUG_UNWIND
      fprintf(TFile, "** %s label %s to %s advance loc + destroy frame\n",
	      __FUNCTION__, LABEL_name(ue_iter->label_idx),
	      Cg_Dwarf_Name_From_Handle(last_label_idx));
#endif
      break;

    case UE_CREATE_FP:
      Is_True(frame_size == ue_iter->offset,
	      ("unwind: dynamic frame size invalid at creation point (%lld vs %lld)",
	       ue_iter->offset, frame_size));

      if (ue_iter->when_bundle_start > current_loc) {
	dwarf_add_fde_inst(fde, DW_CFA_advance_loc4, last_label_idx,
			   Cg_Dwarf_Symtab_Entry(CGD_LABIDX,
						 ue_iter->label_idx,
						 scn_index),
			   &dw_error);
      }
      dwarf_add_fde_inst(fde, DW_CFA_def_cfa, (Dwarf_Unsigned)ue_iter->rc_reg,
			 STACK_OFFSET_ADJUSTMENT,
			 &dw_error);
      if (Trace_Unwind) {
	fprintf(TFile, "create dynamic frame of size %lld at when %d\n",
		frame_size, ue_iter->when);
      }

#ifdef DEBUG_UNWIND
      fprintf(TFile, "** %s label %s to %s advance loc + create dynamic frame %lld\n",
	      __FUNCTION__, LABEL_name(ue_iter->label_idx),
	      Cg_Dwarf_Name_From_Handle(last_label_idx), frame_size);
#endif
      break;

    case UE_DESTROY_FP:
      if (!emit_restores) continue;

      if (ue_iter->when_bundle_start > current_loc) {
	dwarf_add_fde_inst(fde, DW_CFA_advance_loc4, last_label_idx,
			   Cg_Dwarf_Symtab_Entry(CGD_LABIDX,
						 ue_iter->label_idx,
						 scn_index),
			   &dw_error);
      }
      {
          DebugRegId baseId;
          UINT ofst;
          CfaDef(baseId, ofst, Get_Current_PU());
          dwarf_add_fde_inst(fde, DW_CFA_def_cfa, (Dwarf_Unsigned)baseId,
                             ofst,
			     &dw_error);
      }
      if (Trace_Unwind) {
	fprintf(TFile, "destroy dynamic frame of size %lld at when %d\n",
		frame_size, ue_iter->when);
      }

#ifdef DEBUG_UNWIND
      fprintf(TFile, "** %s label %s to %s advance loc + destroy dynamic frame\n",
	      __FUNCTION__, LABEL_name(ue_iter->label_idx),
	      Cg_Dwarf_Name_From_Handle(last_label_idx));
#endif
      break;

    case UE_SAVE_SP:
    case UE_SAVE_FP:
      frame_offset = - ue_iter->offset + STACK_OFFSET_ADJUSTMENT;
      if (ue_iter->kind == UE_SAVE_SP) {
#ifdef PROPAGATE_DEBUG
	if (ue_iter->is_copy == FALSE) {
	  frame_offset += frame_size;
	} else {
	  // if is_copy is TRUE, check if top_offset is valid
	  if (ue_iter->after_sp_adj) {
        frame_offset = -ue_iter->top_offset + STACK_OFFSET_ADJUSTMENT;
      }
	}
#else
	frame_offset += frame_size;
#endif
      }

      if (ue_iter->when_bundle_start > current_loc) {
	dwarf_add_fde_inst(fde, DW_CFA_advance_loc4, last_label_idx,
			   Cg_Dwarf_Symtab_Entry(CGD_LABIDX,
						 ue_iter->label_idx,
						 scn_index),
			   &dw_error);
      }

      dwarf_add_fde_inst(fde, DW_CFA_offset, (Dwarf_Unsigned)ue_iter->rc_reg,
                         frame_offset / abs(DataAlignmentFactor(Get_Current_PU())),
			 &dw_error);

      if (Trace_Unwind) {
	fprintf(TFile, "save reg to sp mem offset %lld at when %d\n",
		frame_offset, ue_iter->when);
	if (ue_iter->frame_changed)
	  fprintf(TFile, "  frame changed in the same bundle\n");
      }

#ifdef DEBUG_UNWIND
      fprintf(TFile,
	      "** %s label %s to %s adv loc + save reg %lld at offset %lld\n",
	      __FUNCTION__, LABEL_name(ue_iter->label_idx),
	      Cg_Dwarf_Name_From_Handle(last_label_idx),
              (long long)ue_iter->rc_reg,
	      frame_offset);
#endif

      break;

    case UE_RESTORE_SP:
    case UE_RESTORE_FP:
      if (!emit_restores) continue;

      if (ue_iter->when_bundle_start > current_loc) {
	dwarf_add_fde_inst(fde, DW_CFA_advance_loc4, last_label_idx,
			   Cg_Dwarf_Symtab_Entry(CGD_LABIDX,
						 ue_iter->label_idx,
						 scn_index),
			   &dw_error);
      }

      dwarf_add_fde_inst(fde, DW_CFA_restore, (Dwarf_Unsigned)ue_iter->rc_reg,
			 0, &dw_error);

      if (Trace_Unwind) {
	fprintf(TFile, "restore reg from %s mem at when %d\n",
		ue_iter->kind == UE_RESTORE_FP ? "fp" : "sp", ue_iter->when);
	if (ue_iter->frame_changed)
	  fprintf(TFile, "  frame changed in the same bundle\n");
      }

#ifdef DEBUG_UNWIND
      fprintf(TFile, "** %s label %s to %s adv loc + restore reg %lld\n",
	      __FUNCTION__, LABEL_name(ue_iter->label_idx),
	      Cg_Dwarf_Name_From_Handle(last_label_idx),
           (long long)ue_iter->rc_reg);
#endif

      break;

    case UE_SAVE_GR:
      if (ue_iter->when_bundle_start > current_loc) {
	dwarf_add_fde_inst(fde, DW_CFA_advance_loc4, last_label_idx,
			   Cg_Dwarf_Symtab_Entry(CGD_LABIDX,
						 ue_iter->label_idx,
						 scn_index),
			   &dw_error);
      }

      dwarf_add_fde_inst(fde, DW_CFA_register, (Dwarf_Unsigned)ue_iter->rc_reg,
                         (Dwarf_Unsigned)ue_iter->save_rc_reg,
			 &dw_error);

      if (Trace_Unwind) {
	fprintf(TFile, "save reg %lld to reg %lld at when %d\n",
		ue_iter->rc_reg, ue_iter->save_rc_reg,
		ue_iter->when);
      }

#ifdef DEBUG_UNWIND
      fprintf(TFile,
	      "** %s label %s to %s adv loc + save reg %lld to reg %lld\n",
	      __FUNCTION__, LABEL_name(ue_iter->label_idx),
	      Cg_Dwarf_Name_From_Handle(last_label_idx),
	      ue_iter->rc_reg, ue_iter->save_rc_reg);
#endif

      break;

    case UE_RESTORE_GR:
      if (!emit_restores) continue;

      if (ue_iter->when_bundle_start > current_loc) {
	dwarf_add_fde_inst(fde, DW_CFA_advance_loc4, last_label_idx,
			   Cg_Dwarf_Symtab_Entry(CGD_LABIDX,
						 ue_iter->label_idx,
						 scn_index),
			   &dw_error);
      }

      dwarf_add_fde_inst(fde, DW_CFA_restore, (Dwarf_Unsigned)ue_iter->rc_reg,
			 0, &dw_error);

      if (Trace_Unwind) {
	fprintf(TFile, "restore reg %lld from reg %lld when %d\n",
            (long long)ue_iter->rc_reg, (long long)ue_iter->save_rc_reg,
		ue_iter->when);
      }

#ifdef DEBUG_UNWIND
      fprintf(TFile,
	      "** %s label %s to %s adv loc + restore reg %lld from reg %lld\n",
	      __FUNCTION__, LABEL_name(ue_iter->label_idx),
	      Cg_Dwarf_Name_From_Handle(last_label_idx),
              (long long)ue_iter->rc_reg, (long long)ue_iter->save_rc_reg);
#endif

      break;

    default:
      FmtAssert(FALSE, ("%s: Unhandled UNWIND_ELEM kind (%d)\n", __FUNCTION__, ue_iter->kind));
      break;
    }
    last_label_idx = Cg_Dwarf_Symtab_Entry(CGD_LABIDX, ue_iter->label_idx,
					   scn_index);
#ifdef DEBUG_UNWIND
    fprintf(TFile, "** %s upgraded last label to %s\n", __FUNCTION__,
	    Cg_Dwarf_Name_From_Handle(last_label_idx));
#endif

    current_loc = ue_iter->when_bundle_start;
  }
}

/* construct the fde for the current procedure. */
extern Dwarf_P_Fde
Build_Fde_For_Proc (Dwarf_P_Debug dw_dbg, BB *firstbb,
		    Dwarf_Unsigned begin_label,
		    Dwarf_Unsigned end_label,
		    INT32     end_offset,
		    // The following two arguments need to go away
		    // once libunwind gives us an interface that
		    // supports symbolic ranges.
		    INT       low_pc,
		    INT       high_pc,
		    Elf64_Word	scn_index,
                    bool emit_restores)
{
  Dwarf_P_Fde fde;
  Dwarf_Error dw_error;

  if ( ! OPTIONS_CHECK) return NULL;
  if ( CG_emit_unwind_directives) return NULL;

  fde = dwarf_new_fde(dw_dbg, &dw_error);

  // process info we've collected and create the unwind descriptors
  Create_Unwind_Descriptors(fde, scn_index, begin_label, emit_restores);

  return fde;
}
