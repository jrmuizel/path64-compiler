/*
  ,Copyright (C) 2002, STMicroelectronics, All Rights Reserved.

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
 * Module: cg_ssaopt.cxx
 *
 * Description:
 *
 * General Flags are:
 *
 * ====================================================================
 * ====================================================================
 */


// [HK]
#if __GNUC__ >= 3
#include <list>
// using std::list;
#else
#include <list.h>
#endif // __GNUC__ >= 3

// Do we have to ensure compatibility with old gcc version?
#include <set>

#include "defs.h"
#include "tracing.h"
#include "errors.h"
#include "mempool.h"
#include "cxx_memory.h"
#include "glob.h"
#include "bitset.h"
#ifndef TARG_ST 
#include "config_targ.h"
#else
#include "config_target.h"
#endif
#include "config.h"

#include "symtab.h"
#include "strtab.h"
#include "be_symtab.h"
#include "targ_isa_lits.h"
#include "topcode.h"
#include "cgir.h"
#include "cg.h"
#include "region_util.h"
#include "cg_region.h"
#include "tn_set.h"
#include "tn_list.h"
#include "tn_map.h"
#include "op_map.h"
#include "bb_set.h"
#include "bb_list.h"
#include "gtn_universe.h"
#include "gtn_set.h"
#include "gtn_tn_set.h"

#include "dominate.h"
#include "gra_live.h"
#include "cgexp.h"
#include "range_prop.h"
#include "cg_ssa.h"
#include "cg_ssaopt.h"
#include "cg_ivs.h"
#include "cg_spill.h"

/* ================================================================
 *
 *   Common Base Pointer Optimization.
 *
 * ================================================================
 */

static BOOL
Retrieve_Base_Offset(OP* op, TN*& tn_base, TN*& tn_offset);

static TN *
Get_Addr_Exp(TN *tn, INT64 *offset_value);

static BOOL Trace_SSA_CBPO = 0;

static BOOL g_factorizeCstPhase = FALSE;

#if 0
#define SIGNED_BITS 9
#define MIN_OFFSET ((-1)<<(SIGNED_BITS-1))
#define MAX_OFFSET (( 1 <<(SIGNED_BITS-1))-1)
#define MAX_WIDTH  (( 1 << SIGNED_BITS  ) -1)
#else
/* Replace by target dependent info.*/
static INT MIN_OFFSET = 0;           /* In case of signed 9 bits -> -256    */
static INT MAX_OFFSET = 0;           /* In case of signed 9 bits -> +255    */
static INT MAX_WIDTH = 0;            /* MAX_WIDTH = MAX_OFFSET - MIN_OFFSET */
static INT min_offset_alignment = 1; /* 1 -> char (byte), 2 -> short (2 bytes), 4 -> word (4 bytes), ... */
#endif

static MEM_POOL extended_offset_pool;
static MEM_POOL base_offset_pool;
static TN_MAP base_offset_map;
static TN_MAP constant_map;

static BOOL Optimized_Extended_Immediate;
static float min_frequency_ratio = 1.0;

/* ====================================================================
 * Data structure for base/offset propagation
 *
 * ====================================================================
 */

class Addr_Exp_Info {
public:
  Addr_Exp_Info(TN *_base, TN *_offset) {
    base_reg = _base;
    offset_tn = _offset;
  }
  Addr_Exp_Info() {
    base_reg = NULL;
    offset_tn = NULL;
  }

  TN    *base_reg;
  TN    *offset_tn;
};

#define Addr_base_reg(addr)  ((addr).base_reg)
#define Addr_offset_tn(addr) ((addr).offset_tn)

class Base_Offset_Info {
public:
  Base_Offset_Info(OP *_op, TN *_base, TN *_offset, INT64 _base_dist, INT64 _addr_dist) {
    op        = _op;
    base_tn   = _base;
    offset_tn = _offset;
    base_dist = _base_dist;
    addr_dist = _addr_dist;
  }

  OP	*op;
  TN	*base_tn;
  TN    *offset_tn;
  INT64  base_dist;
  INT64  addr_dist;
};
  
#define BO_op(b)	((b).op)
#define BO_base_tn(b)   ((b).base_tn)
#define BO_offset_tn(b) ((b).offset_tn)
#define BO_base_dist(b) ((b).base_dist)
#define BO_addr_dist(b) ((b).addr_dist)

typedef std::vector<Base_Offset_Info> vec_bo_info;

typedef std::pair<TN *, vec_bo_info *> base_ops;
typedef std::vector<base_ops> vec_base_ops;

#define VEC_BASE_base(bv) (bv).first
#define VEC_BASE_ops(bv)  (bv).second

static void
Generate_Common_Base(vec_base_ops& Base_Ops_List);

static void
Print_base_ops(FILE *file, base_ops &base_list) {
  int i;
  vec_bo_info& op_offset = *VEC_BASE_ops(base_list);

  fPrint_TN(file, "***** Print_base_ops for %s\n", VEC_BASE_base(base_list));  
  for (i = 0; i < op_offset.size(); i++) {
    fprintf(file, "\t(op %d", OP_map_idx(BO_op(op_offset[i])));
    fPrint_TN(file, ", base %s", BO_base_tn(op_offset[i]));
    fPrint_TN(file, ", offset %s", BO_offset_tn(op_offset[i]));
    fprintf(file, ", base distance 0x%llx", BO_base_dist(op_offset[i]));
    fprintf(file, ", addr distance 0x%llx", BO_addr_dist(op_offset[i]));
    fprintf(file, ")\n");
  }
}

/* ====================================================================
 * Comparison function to sort List(OP* op, INT64 offset), in
 * increasing offset values.
 * ====================================================================
 */

inline bool offset_lt (Base_Offset_Info off1, Base_Offset_Info off2) 
{ 
  return (BO_addr_dist(off1) < BO_addr_dist(off2));
}

inline bool offset_lt_and_regions (Base_Offset_Info off1, Base_Offset_Info off2) 
{ 
  OP * op1 = BO_op(off1);
  OP * op2 = BO_op(off2);
  if (BB_loop_head_bb(OP_bb(op1)) == BB_loop_head_bb(OP_bb(op2)))
    return (BO_addr_dist(off1) < BO_addr_dist(off2));
  return (BB_loop_head_bb(OP_bb(op1)) < BB_loop_head_bb(OP_bb(op2)));
}

inline bool op_dominance (Base_Offset_Info off1, Base_Offset_Info off2)
{
  OP * op1 = BO_op(off1);
  OP * op2 = BO_op(off2);
  // If non dominnates the other, do not change the order
  if (OP_bb(op1) != OP_bb(op2))
    return !BB_SET_MemberP(BB_dom_set(OP_bb(op1)), OP_bb(op2));
  return OP_Precedes(op1, op2);
}

/* ====================================================================
 * Return the common domminator of two basic blocks.
 * ====================================================================
 */

static BB*
BB_get_common_dom(BB *dom, BB* bb) {

  if (dom == NULL)
    return bb;

  while (dom && !BB_SET_MemberP (BB_dom_set((bb)), dom)) {
    dom = BB_dominator(dom);
  }

  return dom;
}

static vec_bo_info*
Lookup_Ops_List_For_Imm(vec_base_ops& Base_Ops_List, TN *tn_base,
                        TN* tn_offset, INT64 base_dist) {
  TN* tn_offset_base = NULL;
  INT64 offset_value = 0;
  if(tn_offset) {
      tn_offset_base = Get_Addr_Exp(tn_offset, &offset_value);
  }
  for (int i = 0; i < Base_Ops_List.size(); i++) {
    base_ops& base_list = Base_Ops_List[i];
    INT64 base_offset_value;
    TN* tn_base_base = Get_Addr_Exp(BO_offset_tn((*VEC_BASE_ops(base_list))[0]),
                                    &base_offset_value);
    if (VEC_BASE_base(base_list) == tn_base &&
        (tn_offset_base == NULL ||
         (tn_offset_base == tn_base_base && offset_value == base_offset_value))
        && BO_addr_dist((*VEC_BASE_ops(base_list))[0]) == base_dist)
      return VEC_BASE_ops(base_list);
  }
  return NULL;
}

/* ====================================================================
 * Returns the list of operations in the form 'offset[base]', where
 * base == tn_base. Returns NULL if there is no such list.
 * ====================================================================
 */

static vec_bo_info*
Lookup_Ops_List(vec_base_ops& Base_Ops_List, TN *tn_base) {
  for (int i = 0; i < Base_Ops_List.size(); i++) {
    base_ops& base_list = Base_Ops_List[i];
    if (VEC_BASE_base(base_list) == tn_base)
      return VEC_BASE_ops(base_list);
  }
  return NULL;
}

/* ====================================================================
 * For a sublist of operations in the form 'offset[base_tn]', replace
 * the base_tn by a new base and update the offsets, such that they
 * are now within [MIN_OFFSET, MAX_OFFSET].
 * ====================================================================
 */

static TN *
Dup_Rematerializable_TN(TN *tn, INT64 adjust) {

  TN *new_tn;
  if (TN_is_dedicated(tn))
    new_tn = Dup_TN_Even_If_Dedicated(tn);
  else
    new_tn = Dup_TN(tn);

  if (TN_is_rematerializable(tn)) {
    Is_True(TN_is_rematerializable(new_tn), ("Dup_TN did not propagate rematerializable attribute."));
    WN *wn_remat = TN_remat(tn);
    if (wn_remat && WN_operator(wn_remat) == OPR_LDA) {
      WN *new_wn_remat = WN_CreateLda(WN_opcode(wn_remat), WN_lda_offset(wn_remat) + adjust,
				     WN_ty(wn_remat), WN_st_idx(wn_remat), WN_field_id(wn_remat));
      Set_TN_remat(new_tn, new_wn_remat);
    }
    else {
      Reset_TN_is_rematerializable(new_tn);
      Set_TN_remat(new_tn, NULL);
    }
  }

  // FdF 20061212: In any case, reset the GRA_homeable property,
  // otherwise new_tn would share the same spill location as the
  // original one.
  if (TN_is_gra_homeable(tn)) {
    Reset_TN_is_gra_homeable(new_tn);
    Set_TN_home(new_tn, NULL);
  }

  return new_tn;
}


static BOOL
Use_TN(const OPS& ops, TN *tn)
{
  OP* op;
  BOOL result = FALSE;
  for(op = OPS_first(&ops); op && !result; op = OP_next(op)) {
    for(INT i = 0; i < OP_opnds(op); ++i) {
      result |= tn == OP_opnd(op, i);
    }
  }
  return result;
}

static BOOL
Set_Remat(TN* result_tn, TN* oldBase)
{
    BOOL result = !TN_is_dedicated(result_tn);
    if(result && !TN_is_rematerializable(result_tn))
        {
            if(TN_is_symbol(oldBase))
                {
                    // [CQ1] I do not know a way to specify that result_tn will
                    // containt the constant which represents the address
                    // of TN_var(oldBase) + TN_offset(oldBase).
                    // CGSPILL_Attach_Lda_Remat represents the load result of
                    // that address

                    // Pointer are seen as unsigned int
                    // CGSPILL_Attach_Lda_Remat(result_tn, MTYPE_U4,
                    //                          TN_offset(oldBase),
                    //                          TN_var(oldBase));
                }
            else if(TN_has_value(oldBase))
                {
                    // [CQ1] Does not set the rematerialization flag because,
                    // conditions used in GRA are bugged. Hence, it exists a
                    // path where TN_home (WN type) is used as a ST!
                    // E.g. orginal tn is rematerializable and spill tn is not

                    // CGSPILL_Attach_Intconst_Remat(result_tn, MTYPE_I4,
                    //                               TN_value(oldBase));
                }
            else
                {
                    result = FALSE;
                }
        }
    return result;
}

static BOOL
Update_Expensive_Load_Im(OP *&op, INT index, INT64 offset_val, TN *new_base_tn,
                         TN *new_offset_tn)
{

  OPS ops = OPS_EMPTY;
  TN *result_tn = OP_result(op, 0);
  OP *point = OP_next(op);
  BB *bb = OP_bb(op);
  TN *oldBase = OP_opnd(op, index);
  BOOL result;
  BOOL tnIsRemat = TN_is_rematerializable(result_tn) != 0;
  if((result = Set_Remat(result_tn, oldBase)))
    {
      BB_Remove_Op(OP_bb(op), op);
      if(offset_val < 0)
          {
              Exp_SUB(MTYPE_I4, result_tn, new_base_tn,
                      Gen_Literal_TN(-offset_val, 4), &ops);
          }
      else if(offset_val > 0)
          {
              Exp_ADD(MTYPE_I4, result_tn, new_base_tn,
                      new_offset_tn, &ops);
          }
      else
          {
              Expand_Copy(result_tn, True_TN, new_base_tn, &ops);
          }
      if(!CGTARG_sequence_is_cheaper_than_load_imm(&ops, op))
          {
              result = FALSE;
              OPS_Remove_All(&ops);
              OPS_Append_Op(&ops, op);
              if(!tnIsRemat)
                  {
                      Reset_TN_is_rematerializable(result_tn);
                  }
          }
      if(point)
          {
              BB_Insert_Ops(bb, point, &ops, TRUE);
          }
      else
          {
              // else op was the last instruction of the basic block
              BB_Append_Ops(bb, &ops);
          }
      op = ops.last;
    }
  return result;
}

static TN *
Update_Op_With_New_Base(OP *&op, TN *new_base_tn, INT64 base_offset, INT64 addr_dist, INT& nbDone) {
  INT base_idx = OP_find_opnd_use(op, OU_base);
  INT offset_idx = OP_find_opnd_use(op, OU_offset);
    
  TN *base_tn = NULL;
  TN *offset_tn = NULL;
  BOOL result = Retrieve_Base_Offset(op, base_tn, offset_tn);
  DevAssert(result, ("We should not be here if Retrieve_Base_Offset returns "
                     "false!!!"));

  TN *new_offset_tn;
    
  INT64 offset_val = addr_dist - base_offset;
  new_offset_tn = Gen_Literal_TN(offset_val, 4);
  if (/*TN_is_register(offset_tn) &&*/ offset_idx >= 0) {
    TOP new_top;
    new_top = TOP_opnd_immediate_variant(OP_code(op), offset_idx, offset_val);
	//[dt] We should inform the user if the offset is not in the range or if it is a misaligned access 
    if ((new_top == TOP_UNDEFINED) &&  !TOP_opnd_value_in_range (OP_code(op), offset_idx, offset_val)) {
		ISA_LIT_CLASS lc = OP_opnd_lit_class(op, offset_idx);
		const ISA_LIT_CLASS_INFO *plc = &ISA_LIT_CLASS_info[lc];
		INT i;
		for (i = 1; i <= plc->num_ranges; ++i) {
			if ( (offset_val & plc->range[i].scaling_mask) != 0) {
				// Issue is on the scaling constraint 
				return NULL;
			}
		}
    }
    FmtAssert(new_top != TOP_UNDEFINED,("Unable to get TOP immediate variant"));
    if (Trace_SSA_CBPO) {
      fprintf(TFile, ">>> Replacing OLD op by NEW op:\n    OLD ");
      Print_OP_No_SrcLine (op);
    }
    OP_Change_Opcode(op, new_top);
  }
    
  if(base_idx < 0 || offset_idx < 0) {
    INT index = CGTARG_expensive_load_imm_immediate_index(op);
    if(index >= 0) {
      if(!Update_Expensive_Load_Im(op, index, offset_val, new_base_tn,
                                   new_offset_tn)) {
          if(Trace_SSA_CBPO) {
              fprintf(TFile, "Do not update expensive load\n");
          }
      } else {
          ++nbDone;
      }
    }
  } else {
    ++nbDone;
    Set_OP_opnd(op, base_idx, new_base_tn);
    Set_OP_opnd(op, offset_idx, new_offset_tn);
  }
  if (Trace_SSA_CBPO) {
    fprintf(TFile, "    NEW ");
    Print_OP_No_SrcLine (op);
  }

  // FdF 20080204: Perform the following transformation if allowed:
  //
  // X = PHI(X0,Y) --> X = PHI(X0,Y)
  // @X+off1	   --> @X+off1
  // Y = X+incr    --> 
  // @Y+off2       --> @X+off2+incr
  //               --> Y = X+incr
  //
  // Moving the definition of Y after, hopefully, the last use of X,
  // will avoid creating an interference between X and Y that would
  // otherwise introduce an additional copy during the out-of-SSA
  // renaming phase.

  if ((new_base_tn != base_tn) && (TN_ssa_def(base_tn) != NULL)) {
    // Look for the definition of base_tn
    OP *op_base = TN_ssa_def(base_tn);
    if ((OP_iadd(op_base) || OP_isub(op_base)) &&
	(OP_results(op_base) == 1) &&
	OP_Refs_TN(op_base, new_base_tn)) {
      // Check if op_base can be moved below op, that is, check that
      // base_tn is not used before op
      if (OP_bb(op_base) == OP_bb(op)) {
	OP *point = op_base;
	do {
	  point = OP_next(point);
	  if (OP_Refs_TN(point, base_tn)) {
	    point = NULL;
	    break;
	  }
	} while (point != op);
	if (point != NULL)
	  BB_Move_Op_After(OP_bb(point), point, OP_bb(op_base), op_base);
      }
    }
  }
    
  return new_offset_tn;
}


static TN *
Update_Op_With_New_Base(OP *&op, TN *new_base_tn, INT64 base_offset, INT64 addr_dist, BB* common_dom, float sum, INT& nbDone, const std::set<int>& alreadyDone) {
  TN *result = NULL;
  BB *bb = OP_bb(op);
  BB *common_dom_new = BB_get_common_dom(common_dom, bb);
  if(Trace_SSA_CBPO) {
    fprintf(TFile, "BB%d frequency: %f\nDominator (BB%d) frequency: %f\n",
            BB_id(bb), BB_freq(bb), BB_id(common_dom_new),
            BB_freq(common_dom_new));
  }

  if(alreadyDone.find(BB_id(bb)) == alreadyDone.end()) {
    sum += BB_freq(bb);
  }
  // CQ1 Here we check that the frequency of the basic block, that will receive
  // the initialization of the new base, does not have a too big difference
  // with the source one. Moreover, if frequencies are too different but we
  // do not change the dominator and we have already had to emit the new base
  // initialization or we do not change the destination basic block, then
  // performs the optimization.
  // FdF 20061027: In case of operations in exception handlers,
  // there will be no common dominator. Some operations will not be
  // optimized in this case. (common_dom_new == NULL)
  // [TTh] 20080220: Use KnuthCompare() function to improve
  // determinism of float comparison.
  if(common_dom_new &&
     (!(CG_cbpo_block_method & CBPO_BLOCK_LOCAL) ||
      (KnuthCompareGT((sum / BB_freq(common_dom_new)), min_frequency_ratio)) ||
      (common_dom_new == common_dom && nbDone) || common_dom_new == bb)) {
    result = Update_Op_With_New_Base(op, new_base_tn, base_offset,
                                     addr_dist, nbDone);
  } else {
    if(Trace_SSA_CBPO) {
      fprintf(TFile, "Do not update load: Frequencies are too "
              "different: Sum %f\tDominator %f\n", sum,
              BB_freq(common_dom_new));
    }
  }
  return result;
}

static INT
Check_And_Update_Ops_With_New_Base(BB *&common_dom, TN* new_base_tn,
                                   INT64 base_offset, vec_bo_info& op_offset,
                                   int first, int last) {
  common_dom = NULL;
  INT nbDone = 0;
  int redoSize;
  int nbPhase = 1;

  // FdF 20080204: Now sort in dominance order, so as to efficiently
  // move increment operation between SSA variables used as base of
  // memory accesses
  stable_sort(op_offset.begin()+first, op_offset.begin()+last+1, op_dominance);

  std::list<int> shouldRedo;
  std::set<int> alreadyDone;
  float sum = 0.0;
  BB *tmp_dom = common_dom;
  for (int i = first; i <= last; i++) {
    shouldRedo.push_back(i);
    BB* bb = OP_bb(BO_op(op_offset[i]));
    if(alreadyDone.find(BB_id(bb)) == alreadyDone.end()) {
      alreadyDone.insert(BB_id(bb));
      sum += BB_freq(bb);
      tmp_dom = BB_get_common_dom(tmp_dom, bb);
    }
  }

  // Save block method value
  INT CG_cbpo_block_method_save = CG_cbpo_block_method;

  if(CG_cbpo_block_method & CBPO_BLOCK_GLOBAL) {
    // [TTh] 20080220: Use KnuthCompare() function to improve
    // determinism of float comparison.
    if(KnuthCompareLT((sum / BB_freq(tmp_dom)), min_frequency_ratio)) {
      if(Trace_SSA_CBPO) {
        fprintf(TFile, "Do not globaly optimize: Sum %f\tDominator "
                "(BB%d) %f\n",
                sum, BB_id(tmp_dom), BB_freq(tmp_dom));
      }
      if(CG_cbpo_block_method == CBPO_BLOCK_GLOBAL) {
        return 0;
      }
      // Else: Global has failed, but user set method global then local, so
      // continue
    } else {
      // Global will work, so do not apply local block method
      CG_cbpo_block_method = CBPO_BLOCK_GLOBAL;
    }
  }

  alreadyDone.clear();
  sum = 0.0;

  // [CQ1] We perform the update of the operation while we have rejected
  // transformation and the number of rejected transformation is lower than the
  // previous iteration. The purpose is to miss not optimization opportunity,
  // since dominator may change and the evaluation of the frequency condition
  // can become true (because of nbDone for instance, which may force an
  // optimization).
  do {
    redoSize = shouldRedo.size();
    std::list<int> tmp;
    std::list<int>::const_iterator it;
    for(it = shouldRedo.begin(); it != shouldRedo.end(); ++it) {
      OP *op = BO_op(op_offset[*it]);
      INT nbDoneOld = nbDone;
      Update_Op_With_New_Base(op, new_base_tn, base_offset,
                              BO_addr_dist(op_offset[*it]), common_dom, sum,
                              nbDone, alreadyDone);
      if(nbDone != nbDoneOld) {
        if(Trace_SSA_CBPO) {
          fprintf(TFile, "Update dominator (%d)\n", nbPhase);
        }
        // Increase the sum for this group of operations unless it has been
        // done
        if(alreadyDone.find(BB_id(OP_bb(op))) == alreadyDone.end()) {
          alreadyDone.insert(BB_id(OP_bb(op)));
          sum += BB_freq(OP_bb(op));
        }
        common_dom = BB_get_common_dom(common_dom, OP_bb(op));
      } else {
        tmp.push_back(*it);
      }
    }
    ++nbPhase;
    shouldRedo = tmp;
  } while(!shouldRedo.empty() && shouldRedo.size() < redoSize);

  // Restore value defined by user
  CG_cbpo_block_method = CG_cbpo_block_method_save;
  return nbDone;
}

static OPS
Update_Ops_With_New_Base(TN *base_tn, INT64 base_adjust, TN *remat_base, INT64 remat_adjust, vec_bo_info& op_offset, int first, int last) {

  if(Trace_SSA_CBPO) {
    fprintf(TFile, "%s: Considered operations:\n", __FUNCTION__);
    int idx;
    for(idx = first; idx <= last; ++idx) {
        Print_OP_No_SrcLine(BO_op(op_offset[idx]));
    }
    fprintf(TFile, "-------------------\n");
  }

  // Compute a new base_offset, such that positive offsets are
  // preferred, and alignment is kept if possible.
  int width = BO_addr_dist(op_offset[last]) - BO_addr_dist(op_offset[first]);
  INT64 base_offset;
  INT min_offset;
  if (width <= MAX_OFFSET)
    min_offset = 0;
  else
    min_offset = MAX_OFFSET - width;
  base_offset = BO_addr_dist(op_offset[first]) - min_offset;

  /* Search biggest admissible alignment. */
  /* Start with min target alignment constraint */
  // We will increment base_offset so as to maximize its alignment,
  // such that the offset for the first and last operation is still
  // representable.
  int biggest_offset_alignment = min_offset_alignment;
  int dist_to_next_align = (-base_offset)&(biggest_offset_alignment-1);
  FmtAssert(MIN_OFFSET<0,("This algorithm works only with MIN_OFFSET < 0"));

  // [CQ1] Do not maximize offset alignment when all commonalized addresses are
  // representable (min_offset == 0) and first instruction is a load imm.
  // The purpose is to generate move instead of add/sub instruction for
  // immediate loading
  if(!CGTARG_is_expensive_load_imm(BO_op(op_offset[first])) || min_offset) {
    while ((min_offset - dist_to_next_align) >= MIN_OFFSET && biggest_offset_alignment < MAX_OFFSET) {
      min_offset -= dist_to_next_align;
      base_offset += dist_to_next_align;
      biggest_offset_alignment *= 2;
      dist_to_next_align = (-base_offset)&(biggest_offset_alignment-1);
    }
  }

  // In case the original base_tn is rematerializable, new_base_tn can
  // also be rematerializable in simple cases.

  TN *new_base_tn;
  if (base_tn && TN_is_register(base_tn))
    new_base_tn = Dup_Rematerializable_TN(base_tn, base_offset-base_adjust);
  else if (remat_base != NULL)
    new_base_tn = Dup_Rematerializable_TN(remat_base, base_offset-remat_adjust);
  else
    new_base_tn = Build_TN_Of_Mtype(MTYPE_I4);

  // FdF 20080204: Move here the initialization of new_base_tn: When a
  // copy would be generated, it is better to set new_base_tn to
  // base_tn, to avoid the EBO breaking what is done here.

  OPS ops = OPS_EMPTY;
  INT64 init_adjust = base_offset - base_adjust;

  if (base_tn == NULL)
    Expand_Immediate(new_base_tn, Gen_Literal_TN(init_adjust, 4), MTYPE_I4, &ops);
  else if (!TN_is_register(base_tn))
    Expand_Immediate(new_base_tn, Gen_Adjusted_TN(base_tn, init_adjust), MTYPE_I4, &ops);
  else if (init_adjust < 0)
    Exp_SUB(MTYPE_I4, new_base_tn, base_tn, Gen_Literal_TN(-init_adjust, 4), &ops);
  else if (init_adjust > 0)
    Exp_ADD(MTYPE_I4, new_base_tn, base_tn, Gen_Literal_TN(init_adjust, 4), &ops);
  else if(BO_offset_tn(op_offset[first]) &&
	  !TN_has_value(BO_offset_tn(op_offset[first])) &&
	  g_factorizeCstPhase) {
    DevAssert(BO_addr_dist(op_offset[first]) == 0,
	      ("Do not know how to create the new base"));
    OP* op = Dup_OP(BO_op(op_offset[first]));
    OPS_Append_Op(&ops, op);
    DevAssert(OP_results(op) == 1,
	      ("Do not know where to set the new base"));
    Set_OP_result(op, 0, new_base_tn);
  }
  else {
    // FdF 20080204
    //Expand_Copy(new_base_tn, True_TN, base_tn, &ops);
    new_base_tn = base_tn;
  }
      
  BB *common_dom;
  INT nbDone = Check_And_Update_Ops_With_New_Base(common_dom, new_base_tn,
                                                  base_offset, op_offset,
                                                  first, last);

  // Insert the initialization of the new base in the common
  // domminator of all the operations.
  if(Trace_SSA_CBPO) {
    fprintf(TFile,"*** nbDone %d\n", nbDone);
  }

  if(nbDone) {
    if (Trace_SSA_CBPO) {
      fprintf(TFile, ">>> Inserting new OP in BB %d:\n", BB_id(common_dom));
      Print_OPS_No_SrcLines(&ops);
    }
      
    // If operations to be inserted do not use base_tn, we must not insert them
    // after the definition of that tn. E.g.
    // base_tn :- load_im cst
    // Is replace by
    // base_tn :- move new_base_tn
    // So operations to be inserted define new_base_tn, so we have to insert
    // them before first usage of new_base_tn (i.e. operation which define
    // base_tn) or in dominator bb.
    // This is not true with other load operations, since new_base_tn is
    // defined from base_tn, that is why we check base_tn usage.
    OP *op_base = (base_tn && TN_is_register(base_tn) &&
                   Use_TN(ops, base_tn))? TN_ssa_def(base_tn): NULL;
    OP *point = (op_base && (common_dom == OP_bb(op_base)))? OP_next(op_base):
      BB_first_op(common_dom);

    // FdF 20061124: Be careful not to insert code before spadjust
    while (point && (OP_code(point) == TOP_phi ||
                     point == BB_entry_sp_adj_op(OP_bb(point)))) {
      point = OP_next(point);
    }

    OP* sp_adjust;
    if ((point != NULL) &&
        !(op_base && common_dom == OP_bb(op_base)) && // In this case, insertion point is after op_base. So necessary after sp_adjust.
        ( ( sp_adjust = BB_entry_sp_adj_op(OP_bb(point)) ) != NULL))  {
      // [JV] In case of double sp_adjust, the last one is marked
      // sp_adjust.
      point = OP_next(sp_adjust);
    }
    BOOL before = (point != NULL);
    BB_Insert_Ops(common_dom, point, &ops, before);
      
    Optimized_Extended_Immediate = TRUE;
  }
  return ops;
}

/* ====================================================================
 * Propagate constants and sequence of form base +/- offset
 *
 * ====================================================================
 */

static TN *
Get_Constant_Value(TN *tn) {
  if (!TN_is_constant(tn) && TN_MAP_Get(constant_map, tn))
    return (TN *)TN_MAP_Get(constant_map, tn);
  if (TN_is_zero(tn))
    return Gen_Literal_TN(0, 4);
  return tn;
}

// This function will return one of the following :
// (NULL, offset=Value), (Symbol, offset=Value), (Register,
// offset=Value), (tn, offset=0)

static TN *
Get_Addr_Exp(TN *tn, INT64 *offset_value) {

  TN *base = tn;
  *offset_value = 0;

  TN *base_def = Get_Constant_Value(tn);
  if (TN_is_constant(base_def)) {
    if (TN_is_symbol(base_def)) {
      base = Gen_Symbol_TN(TN_var(base_def), 0, TN_relocs(base_def));
      *offset_value = TN_offset(base_def);
    }
    else { // TN_has_value(base_def)
      base = NULL;
      *offset_value = TN_value(base_def);
    }
  }
  else if (TN_MAP_Get(base_offset_map,tn)) {
    Addr_Exp_Info *tn_info = (Addr_Exp_Info*) TN_MAP_Get(base_offset_map,tn);
    // Ignore the case Reg+Sym
    if (TN_has_value(Addr_offset_tn(*tn_info))) {
      base = Addr_base_reg(*tn_info);
      *offset_value = TN_value(Addr_offset_tn(*tn_info));
    }
  }

  return base;
}

// We support the following expressions, after copy propagation
// (reg+val), (reg-val), (val+reg), (sym+val), (sym-val), (val+sym),
// (val+val), (val-val), (reg+sym), (sym+reg), (reg-sym)

static BOOL
Compute_Addr_Exp(Addr_Exp_Info &addr_exp, TN *base, TN *offset, BOOL commutative, BOOL negate_offset) {

  // Propagate the constant on base and offset if available.
  INT64 base_adjust, offset_adjust;
  base = Get_Addr_Exp(base, &base_adjust);
  offset = Get_Addr_Exp(offset, &offset_adjust);

  // In case of a SUB, only an immediate value is accepted
  if (negate_offset)
    if (offset != NULL)
      return FALSE;
    else
      offset_adjust = -offset_adjust;

  if (commutative &&
      (((base == NULL) && (offset != NULL)) ||
       ((base != NULL) && (offset != NULL) && TN_is_symbol(base) && TN_is_register(offset)))) {
    TN *tmp = base;
    base = offset;
    offset = tmp;
  }

  if (base == NULL && offset == NULL) {
    addr_exp.base_reg = NULL;
    addr_exp.offset_tn = Gen_Literal_TN(base_adjust+offset_adjust, 4);
    return TRUE;
  }

  else if ((base != NULL) && (offset == NULL)) {
    if (TN_is_symbol(base)) {
      addr_exp.base_reg = NULL;
      addr_exp.offset_tn = Gen_Adjusted_TN(base, base_adjust+offset_adjust);
      return TRUE;
    }
    else if (TN_is_register(base)) {
      addr_exp.base_reg = base;
      addr_exp.offset_tn = Gen_Literal_TN(base_adjust+offset_adjust, 4);
      return TRUE;
    }
  }

  else if ((base != NULL) && (offset != NULL)) {
    if (TN_is_register(base) && (TN_is_symbol(offset))) {
      addr_exp.base_reg = base;
      addr_exp.offset_tn = Gen_Adjusted_TN(offset, base_adjust+offset_adjust);
      return TRUE;
    }
  }

  return FALSE;
}

static INT64
Get_TN_Distance(TN *common_tn, TN *base_tn) {

  INT64 base_adjust;
  base_tn = Get_Addr_Exp(base_tn, &base_adjust);

  if ((common_tn == NULL) && (base_tn == NULL))
    return 0;

  else if (common_tn == base_tn)
    return base_adjust;

  /* FdF 20061117: For symbols, pointers may be different, but symbols
     may actually be the same. (codex bug #20430) */

  else if ((common_tn != NULL) && (base_tn != NULL) &&
           TN_is_symbol(common_tn) && TN_is_symbol(base_tn) &&
           (TN_var(common_tn) == TN_var(base_tn)) &&
           (TN_offset(common_tn) == TN_offset(base_tn)) &&
           (TN_relocs(common_tn) == TN_relocs(base_tn)))
    return base_adjust;

  FmtAssert (FALSE, ("Get_TN_Distance: No distance between TNs"));
}

// TBD: Perform propagate and collect at the same time. Also,
// propagate should be performed in a depth first search of the
// dominator tree.

static void
Propagate_Base_Offset( void ) {

  for (BB *bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    OP *op;
    FOR_ALL_BB_OPs(bb, op) {

      /* Constant propagation */
      INT copy_operand_idx;
      if( (copy_operand_idx = OP_Copy_Operand(op)) != -1 ) {
        TN *constant = OP_opnd(op,copy_operand_idx);
        TN *result = OP_result(op,0);

        if (TN_is_dedicated(result)) continue;

        // FdF 20060918: Consider that a symbol requires an extended immediate
        // FdF 20061205: At this time, propagate all values, the
        // extended offset values will be checked later.
        if (TN_has_value(constant) || TN_is_symbol(constant)) {

            if (Trace_SSA_CBPO) {
                fprintf(TFile, "%s: Propagate constant from OP:\n", __FUNCTION__);
                Print_OP_No_SrcLine(op);
            }
            
            TN_MAP_Set(constant_map, result, constant);
        }
      }

      if(OP_iadd(op) || OP_isub(op)) {
        /* Suppose that base is in opnd1 and offset in opnd2 */
        TN *base = OP_opnd(op,OP_find_opnd_use(op, OU_opnd1));
        TN *offset = OP_opnd(op,OP_find_opnd_use(op, OU_opnd2));
        TN *result = OP_result(op,0);

        if(TN_is_dedicated(result)) { continue; }

        Addr_Exp_Info addr_exp;
        if (!Compute_Addr_Exp(addr_exp, base, offset, TRUE, OP_isub(op)))
          continue;

        if (addr_exp.base_reg == NULL) {

          if (Trace_SSA_CBPO) {
            fprintf(TFile, "%s: Propagate constant from OP:\n", __FUNCTION__);
            Print_OP_No_SrcLine(op);
          }

          TN_MAP_Set(constant_map, result, addr_exp.offset_tn);
        }
        else {

          if (Trace_SSA_CBPO) {
              fprintf(TFile, "%s: Propagate base/offset from OP:\n", __FUNCTION__);
              Print_OP_No_SrcLine(op);
          }

          Addr_Exp_Info *tn_info = CXX_NEW (Addr_Exp_Info, &base_offset_pool);
          memset(tn_info, 0, sizeof(Addr_Exp_Info));
          Addr_base_reg(*tn_info) = addr_exp.base_reg;
          Addr_offset_tn(*tn_info) = addr_exp.offset_tn;
          TN_MAP_Set(base_offset_map, result, tn_info);
        }
      }
    }
  }
}


static BOOL
Retrieve_Base_Offset(OP* op, TN*& tn_base, TN*& tn_offset)
{
    INT base_idx = OP_find_opnd_use(op, OU_base);
    INT offset_idx = OP_find_opnd_use(op, OU_offset);
    BOOL result = FALSE;

    if(base_idx < 0 || offset_idx < 0)
        {
            INT index;
            if((index = CGTARG_expensive_load_imm_immediate_index(op)) >= 0 &&
               CG_cbpo_optimize_load_imm)
                {
                    TN* tn = OP_opnd(op, index);
                    result = TRUE;
                    if((index = CGTARG_expensive_load_imm_base_index(op)) >= 0)
                        {
                            tn_base = OP_opnd(op, index);
                            tn_offset = tn;
                        }
                    else
                        {
                            if(TN_is_symbol(tn))
                                {
                                    tn_base = Gen_Symbol_TN(TN_var(tn), 0,
                                                            TN_relocs(tn));
                                    tn_offset = Gen_Literal_TN(TN_offset(tn),
                                                               4);
                                }
                            else
                                {
                                    tn_base = tn;
                                    tn_offset = Gen_Literal_TN(0, 4);
                                    // Currently, we do not optimize known
                                    // constant
                                    result = CG_cbpo_optimize_load_imm_cst &&
                                        g_factorizeCstPhase;
                                }
                        }
                }
        }
    else
        {
            tn_base = OP_opnd(op, base_idx);
            tn_offset = OP_opnd(op, offset_idx);
            result = TRUE;
        }
    return result;
}

static void
Commonalize_Load_Imm(TN *base_tn, vec_bo_info& op_offset, int first, int last) 
{
    int i;
    INT64 cst = BO_addr_dist(op_offset[first]);
    for(i = first; i <= last; ++i)
        {
            BO_addr_dist(op_offset[i]) -= cst;
        }
    if(TN_has_value(base_tn))
        {
            // For constant base, cst contains the value of the base, thus
            // we will add TN_value(base_tn) two times without this setting
            cst = cst - TN_value(base_tn);
        }
    Update_Ops_With_New_Base(base_tn, -cst, NULL,
                             0, op_offset, first, last);
}

static BOOL
Is_Cheaper_If_Commonalized(Base_Offset_Info& first, Base_Offset_Info& last)
{
    return CGTARG_is_expensive_load_imm(BO_op(first)) &&
        CGTARG_is_expensive_load_imm(BO_op(last)) &&
        (BO_addr_dist(last) - BO_addr_dist(first) <
         (MAX_WIDTH-(min_offset_alignment-1))) &&
        (BO_addr_dist(last) - BO_addr_dist(first) == 0||
         CGTARG_should_factorize(BO_op(first), BO_op(last)));
}

static void
Generate_Tmp(vec_bo_info& op_offset)
{
    int j, j_first;
    TN* base_tn = BO_base_tn(op_offset[0]);

    for(j = j_first = 0; j < op_offset.size(); j++)
        {
            if(!Is_Cheaper_If_Commonalized(op_offset[j_first], op_offset[j]))
                {
                    if(j - j_first >= 2)
                        {
                            Commonalize_Load_Imm(base_tn, op_offset, j_first,
                                                 j-1);
                        }
                    j_first = j;
                    base_tn = BO_base_tn(op_offset[j_first]);
                }
        }
    if(j - j_first >= 2 && Is_Cheaper_If_Commonalized(op_offset[j_first],
                                                      op_offset[j-1]))
        {
            Commonalize_Load_Imm(base_tn, op_offset, j_first, j-1);
        }
}

static void
Collect_Load_Imm_Base_Offset(vec_base_ops& Base_Ops_List)
{
    for(BB *bb = REGION_First_BB; bb != NULL; bb = BB_next(bb))
        {
            OP *op;
            FOR_ALL_BB_OPs(bb, op)
            {
                TN *tn_base = NULL;
                TN *tn_offset = NULL;
                Addr_Exp_Info addr_exp;

                if(CGTARG_is_expensive_load_imm(op) &&
                   Retrieve_Base_Offset(op, tn_base, tn_offset) &&
                   Compute_Addr_Exp(addr_exp, tn_base, tn_offset, FALSE,
                                    FALSE))
                    {
                        INT64 base_adjust = 0;
                        TN* tmp_offset = NULL;
                        if(addr_exp.base_reg == NULL)
                            {
                                tn_base = Get_Addr_Exp(addr_exp.offset_tn,
                                                       &base_adjust);
                            }
                        else
                            {
                                tmp_offset = tn_offset;
                            }

                        vec_bo_info *oplist = 
                            Lookup_Ops_List_For_Imm(Base_Ops_List,
                                                    tn_base, tmp_offset,
                                                    base_adjust);
                        if(!oplist)
                            {
                                oplist = CXX_NEW(vec_bo_info,
                                                 &extended_offset_pool);
                                Base_Ops_List.
                                    push_back(std::make_pair(tn_base,
                                                             oplist));
                            }
                        oplist->push_back(Base_Offset_Info(op, tn_base,
                                                           tn_offset,
                                                           base_adjust,
                                                           base_adjust));
                    }
            }
        }
}

static void
Collect_Op_Base_Offset(vec_base_ops& Base_Ops_List, OP* op)
{
    TN *tn_base = NULL;
    TN *tn_offset = NULL;
    if(!Retrieve_Base_Offset(op, tn_base, tn_offset)) return;

    Addr_Exp_Info addr_exp;
    if (!Compute_Addr_Exp(addr_exp, tn_base, tn_offset, FALSE, FALSE))
        return;

    INT64 base_dist, addr_dist;
    TN *common_tn = NULL;
    
    if ((addr_exp.base_reg == NULL) && (TN_is_symbol(addr_exp.offset_tn))) {
        common_tn = Gen_Symbol_TN(TN_var(addr_exp.offset_tn), 0, TN_relocs(addr_exp.offset_tn));
        addr_dist = TN_offset(addr_exp.offset_tn);
    }
    else if ((addr_exp.base_reg == NULL) && TN_has_value(addr_exp.offset_tn)) {
        common_tn = NULL;
        addr_dist = TN_value(addr_exp.offset_tn);
    }
    else if ((addr_exp.base_reg != NULL) && TN_has_value(addr_exp.offset_tn)) {
        common_tn = addr_exp.base_reg;
        addr_dist = TN_value(addr_exp.offset_tn);
    }
    else
        return;

    base_dist = Get_TN_Distance(common_tn, tn_base);

    /* Then, add the operation in the list associated with
       common_tn. Each element contains (op, tn_base, val) */
    vec_bo_info *oplist;
    if ((oplist = Lookup_Ops_List(Base_Ops_List, common_tn)) == NULL) {
        oplist = CXX_NEW (vec_bo_info, &extended_offset_pool);
        Base_Ops_List.push_back(std::make_pair(common_tn, oplist));
    }
    oplist->push_back(Base_Offset_Info(op, tn_base, tn_offset, base_dist, addr_dist));
}

static void
Generate_Load_Imm_Common_Base(vec_base_ops& Base_Ops_List)
{
    int i;
    vec_base_ops Imm_Base_Ops_List;

    for (i = 0; i < Base_Ops_List.size(); i++)
        {
            base_ops& base_list = Base_Ops_List[i];
            vec_bo_info& op_offset = *VEC_BASE_ops(base_list);
            if(op_offset.size() >= 2)
                {
                    TN* base_tn = BO_base_tn(op_offset[0]);
                    OPS ops = Update_Ops_With_New_Base(base_tn, 0, NULL, 0,
                                                       op_offset, 0,
                                                       op_offset.size() - 1);
                    if(OPS_first(&ops) != NULL &&
                       OPS_first(&ops) == OPS_last(&ops))
                        {
                            Collect_Op_Base_Offset(Imm_Base_Ops_List,
                                                   OPS_first(&ops));
                        }
                }
            else
                {
                    Collect_Op_Base_Offset(Imm_Base_Ops_List,
                                           BO_op(op_offset[0]));
                }
        }
    for(i = 0; i < Imm_Base_Ops_List.size(); ++i)
        {
            vec_bo_info& op_offset = *VEC_BASE_ops(Imm_Base_Ops_List[i]);
            stable_sort(op_offset.begin(), op_offset.end(), offset_lt);
            Generate_Tmp(op_offset);
        }
}

/* ====================================================================
 * Collect all operations in the form offset[base], where offset is a
 * constant or a symbol.
 * ====================================================================
 */

static void
Collect_Base_Offset(vec_base_ops& Base_Ops_List) {

  for (BB *bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {

    OP *op;
    FOR_ALL_BB_OPs(bb, op) {
        Collect_Op_Base_Offset(Base_Ops_List, op);
    }
  }
}

/* ====================================================================
 * Process the list of operations created by Collect_Base_Offset and
 * transformed by Generate_Common_Base.
 *
 * All operations are on the same base TN. Use one or several
 * auxiliary base pointer to replace extended offsets by a reference
 * to an auxiliary base pointer and a small offset.
 * ====================================================================
 */

static void
Generate_Common_Base(vec_base_ops& Base_Ops_List) {
  int i, j;

  for (i = 0; i < Base_Ops_List.size(); i++) {
    base_ops& base_list = Base_Ops_List[i];
    vec_bo_info& op_offset = *VEC_BASE_ops(base_list);

    if (Trace_SSA_CBPO)
      Print_base_ops(TFile, base_list);

    if (op_offset.size() <= 1) continue;

    // See if all bases are the same: If true, consider only extended
    // offets from the list. Otherwise, consider all operations.

    tn *base_tn = BO_base_tn(op_offset[0]);
    INT register_base_idx = -1;
    BOOL all_same_base = TRUE;
    INT extended_count = 0;

    for (j = 0; j < op_offset.size(); j++) {
      if (BO_base_tn(op_offset[j]) != base_tn)
        all_same_base = FALSE;
      if (register_base_idx == -1 && TN_is_register(BO_base_tn(op_offset[j])))
        register_base_idx = j;
      TN *tn_offset = BO_offset_tn(op_offset[j]);
      INT64 val;
      if (!TN_is_constant(tn_offset) || CGTARG_offset_is_extended(tn_offset, &val))
        extended_count ++;
      //TDR: fix for bug #58284 min_offset_alignment according to reg type 
      // needed by wide registers of STxP70 extensions
      INT offset_idx = OP_find_opnd_use(BO_op(op_offset[j]), OU_offset);
      if (offset_idx!=-1) {
        ISA_LIT_CLASS lc = OP_opnd_lit_class(BO_op(op_offset[j]), offset_idx);
        const ISA_LIT_CLASS_INFO *plc = &ISA_LIT_CLASS_info[lc];
        INT i;
        for (i = 1; i <= plc->num_ranges; ++i) {
          if ((int)plc->range[i].scaling_mask+1 > min_offset_alignment) 
            min_offset_alignment = (int)plc->range[i].scaling_mask+1;
        }
      }
    }

    if (all_same_base && (extended_count <= 2))
      continue;

    TN *common_base_tn;
    INT64 common_adjust;
    // In cases common_base_tn is not a register, we may be able to
    // provide information for rematerialization
    TN *remat_base = NULL;
    INT64 remat_adjust = 0;

    // All bases are the same register, use this one to create
    // auxilliary bases.
    if (all_same_base && register_base_idx != -1) {
      common_base_tn = BO_base_tn(op_offset[register_base_idx]);
      common_adjust = BO_base_dist(op_offset[register_base_idx]);
    }

    else {
      // Address expressions are derived from a common register or
      // a common symbol, or are immediate values.
      common_base_tn = VEC_BASE_base(base_list);
      common_adjust = 0;
      // In case common_base_tn is a symbol or an immediate value, if
      // a register contains this symbol or value, use it to get
      // information for rematerialization.
      if (((common_base_tn == NULL) || (!TN_is_register(common_base_tn))) &&
          (register_base_idx != -1)) {
        remat_base = BO_base_tn(op_offset[register_base_idx]);
        remat_adjust = BO_base_dist(op_offset[register_base_idx]);
      }
    }

    if ((common_base_tn != NULL) && TN_is_dedicated(common_base_tn) && !TN_is_const_reg(common_base_tn)) {
      if (Trace_SSA_CBPO) {
          fPrint_TN(TFile, "Generate_Common_Base: %s is not optimized", VEC_BASE_base(base_list));
          fprintf(TFile, " (gain would be at most %d bytes)\n", (op_offset.size()-2)*4);
      }
      continue;
    }

    if (Trace_SSA_CBPO) {
      fPrint_TN(TFile, "Generate_Common_Base: %s is optimized", VEC_BASE_base(base_list));
      fprintf(TFile, " (gain will be at most %d bytes)\n", (op_offset.size()-2)*4);
    }

    // Sort according to the offsets
    // FdF 20080204: Do not mix memory accesses from different loops.
    stable_sort(op_offset.begin(), op_offset.end(), offset_lt_and_regions);

    // Now, use a greedy algorithm
    int j_first;
    all_same_base = TRUE;
    base_tn = BO_base_tn(op_offset[0]);
    extended_count = 0;
    INT64 val;
    BB *cur_loop_head = BB_loop_head_bb(OP_bb(BO_op(op_offset[0])));

    for (j_first = 0, j = 1; j < op_offset.size(); j++) {
      if (((BO_addr_dist(op_offset[j]) - BO_addr_dist(op_offset[j_first])) >= (MAX_WIDTH-(min_offset_alignment-1))
	   // FdF 20080204: Do not mix memory accesses from different loops.
	   || (cur_loop_head != BB_loop_head_bb(OP_bb(BO_op(op_offset[j])))))) {
        if ((j - j_first > 2) &&
            (!all_same_base || (extended_count > 2)))
            Update_Ops_With_New_Base(common_base_tn, common_adjust, remat_base, remat_adjust, op_offset, j_first, j-1);
        j_first = j;
        base_tn = BO_base_tn(op_offset[j_first]);
        all_same_base = TRUE;
        extended_count = 0;
	cur_loop_head = BB_loop_head_bb(OP_bb(BO_op(op_offset[j_first])));
      }
      if (BO_base_tn(op_offset[j]) != base_tn)
        all_same_base = FALSE;
      if (!TN_is_constant(BO_offset_tn(op_offset[j])) || CGTARG_offset_is_extended(BO_offset_tn(op_offset[j]), &val))
        extended_count ++;
    }
    if ((j - j_first > 2) &&
        (!all_same_base || (extended_count > 2)))
        Update_Ops_With_New_Base(common_base_tn, common_adjust, remat_base, remat_adjust, op_offset, j_first, j-1);
  }
}

/* ====================================================================
 * Optimize operations in the form 'offset[base]'. 
 * ====================================================================
 */

static void
Optimize_Extended_Offset()
{

  if (!CG_enable_cbpo) return;

  Trace_SSA_CBPO = Get_Trace(TP_SSA, SSA_CBPO);
  min_frequency_ratio = CG_cbpo_ratio / 100.0;

  MEM_POOL_Initialize (&base_offset_pool, "base_offset map pool", TRUE);
  MEM_POOL_Push(&base_offset_pool);
  MEM_POOL_Initialize (&extended_offset_pool, "op_offset map pool", TRUE);
  MEM_POOL_Push(&extended_offset_pool);

  vec_base_ops Base_Ops_List;

  if (Trace_SSA_CBPO) {
    fprintf(TFile, "%s CFG Before Optimize_Extended_Offset %s\n%s\n", DBar, __FUNCTION__, DBar);
    Print_All_BBs ();
  }

  base_offset_map = TN_MAP_Create();
  constant_map = TN_MAP_Create();
  
  CGTARG_Get_Info_For_Common_Base_Opt(&min_offset_alignment, &MIN_OFFSET, &MAX_OFFSET);
  MAX_WIDTH = MAX_OFFSET - MIN_OFFSET;

  Optimized_Extended_Immediate = FALSE;

  Propagate_Base_Offset();

  Collect_Base_Offset(Base_Ops_List);
  
  Generate_Common_Base(Base_Ops_List);
  Base_Ops_List.clear();

  if(CG_cbpo_facto_cst)
      {
          g_factorizeCstPhase = TRUE;

          Collect_Load_Imm_Base_Offset(Base_Ops_List);
          Generate_Load_Imm_Common_Base(Base_Ops_List);

          g_factorizeCstPhase = FALSE;
      }


#ifdef TARG_ST
  // Resize instructions, otherwise EBO sees no penalty at inlining
  // extended immediates.
  if (Optimized_Extended_Immediate)
    CGTARG_Resize_Instructions ();
#endif

  if (Trace_SSA_CBPO) {
    fprintf(TFile, "%s CFG After Optimize_Extended_Offset %s\n%s\n", DBar, __FUNCTION__, DBar);
    Print_All_BBs ();
  }

  TN_MAP_Delete(base_offset_map);
  TN_MAP_Delete(constant_map);

  MEM_POOL_Pop(&extended_offset_pool);
  MEM_POOL_Delete(&extended_offset_pool);
  MEM_POOL_Pop(&base_offset_pool);
  MEM_POOL_Delete(&base_offset_pool);

}


/* ================================================================
 *
 *   Dead Code Elimination.
 *
 * ================================================================
 */
typedef Worklist<OP *> OP_Worklist;

static void
Trace_Op_Removal (OP *op, BOOL replace_with_noop)
{
  fprintf (TFile, "SSA_DeadCode: %s ",
           (replace_with_noop ? "Replace " : "Remove "));
  Print_OP_No_SrcLine (op);
  if (replace_with_noop)
    fputs (" ==> noop", TFile);
  fputc ('\n', TFile);
}

static BOOL
is_Op_Required (OP *op)
{
  for (INT i = 0; i < OP_results (op); i++) {
    TN *result = OP_result (op, i);
    if (! TN_ssa_def (result))
      return TRUE;
  }
  if (! OP_has_result (op)
      || OP_xfer (op)
      || (OP_memory (op)  && ! OP_load (op))
      || OP_has_implicit_interactions (op)
      || OP_glue (op)
      || OP_Is_Intrinsic (op)
      || op == BB_exit_sp_adj_op (OP_bb (op))
      || op == BB_entry_sp_adj_op (OP_bb (op)))
    return TRUE;
  return FALSE;
}

static TN *
Trip_Count_TN (BB *bb)
{
  ANNOTATION *annot = ANNOT_Get (BB_annotations (bb), ANNOT_LOOPINFO);
  if (annot) {
    LOOPINFO *info = ANNOT_loopinfo (annot);
    return LOOPINFO_primary_trip_count_tn (info);
  }
  return NULL;
}

class DCAnalysis {
private:
  BOOL tracing;
  OP_MAP op_useful;
  void Set_Op_Useful (OP *op) { OP_MAP32_Set(op_useful, op, TRUE); }
public:
  void Find_Useful_Ops ();
  BOOL Op_Marked_Useful (OP *op) { return OP_MAP32_Get(op_useful, op) != 0; }
  DCAnalysis () {
    op_useful = OP_MAP32_Create ();
    tracing = Get_Trace(TP_SSA, SSA_DEAD_CODE);
  }
  ~DCAnalysis () { OP_MAP_Delete (op_useful); }
};


void
DCAnalysis::Find_Useful_Ops ()
{
  OP_Worklist op_work;
  BB *bb;
  OP *op;

  FmtAssert (op_work.empty (), ("Find_Useful_Ops: worklist not empty"));
  
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    // Preserve trip counts, even if they appear unused.
    TN *trip_count_tn = Trip_Count_TN (bb);
    if (trip_count_tn != NULL && TN_is_ssa_var (trip_count_tn)) {
      Set_Op_Useful (TN_ssa_def (trip_count_tn));
      op_work.push (TN_ssa_def (trip_count_tn));
    }
    FOR_ALL_BB_OPs_FWD (bb, op) {
      if (is_Op_Required (op)) {
	Set_Op_Useful (op);
	op_work.push (op);
      }
    }
  }
  while (! op_work.empty ()) {
    OP *op = op_work.choose ();
    for (INT i = 0; i < OP_opnds(op); i++) {
      TN *opnd = OP_opnd(op, i);
      if (TN_is_register (opnd)) {
	OP *def = TN_ssa_def (opnd);
	if (def && ! Op_Marked_Useful (def)) {
	  Set_Op_Useful (def);
	  op_work.push (def);
	}
      }
    }
  }
}

static void
Optimize_IV()
{
  MEM_POOL loop_descr_pool;
  MEM_POOL_Initialize(&loop_descr_pool, "loop_descriptors", TRUE);
  MEM_POOL_Push (&loop_descr_pool);

  BOOL changed = FALSE;

  for (LOOP_DESCR *loop = LOOP_DESCR_Detect_Loops(&loop_descr_pool);
       loop;
       loop = LOOP_DESCR_next(loop)) {

    if (Optimize_Loop_Induction_Offsets(loop))
      changed = TRUE;
  }

  if (changed) SSA_DeadCode ();

  MEM_POOL_Pop (&loop_descr_pool);
  MEM_POOL_Delete(&loop_descr_pool);
}

/* ================================================================
 *
 *   Entry
 *
 * ================================================================
 */
void
SSA_Optimize()
{
  Optimize_Extended_Offset();

  Optimize_IV();

  RangePropagate();
}

void
SSA_DeadCode ()
{
  BB *bb;
  OP *op;
  OP *next;
  DCAnalysis dc_analysis;
  BOOL tracing = Get_Trace(TP_SSA, SSA_DEAD_CODE);

  dc_analysis.Find_Useful_Ops ();

  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    BOOL in_delay_slot = FALSE;
    for (op = BB_first_op (bb); op != NULL; op = next) {
      next = OP_next (op);
      if (! dc_analysis.Op_Marked_Useful (op)) {
        if (tracing) Trace_Op_Removal (op, in_delay_slot);
        if (in_delay_slot) {
          OP_Change_To_Noop (op);
        } else {
          BB_Remove_Op (bb, op);
        }
      }
      if (OP_xfer (op)) in_delay_slot = TRUE;
    }
  }
}

