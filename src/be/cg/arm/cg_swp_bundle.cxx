
/*

  Copyright (C) 2002 ST Microelectronics, Inc.  All Rights Reserved.

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

  Contact information:  

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/

#define USE_STANDARD_TYPES
// [HK]
#if __GNUC__ >= 3
#include <vector>
#else
#include <vector.h>
#endif // __GNUC__ >= 3
#include <list>
#include "defs.h"
#include "be_errors.h"
#include "tracing.h"
#include "matrix.h"
#include "mempool_allocator.h"
#include "glob.h"    // for Cur_PU_Name
#include "op.h"
#include "cg.h"
#include "cg_loop.h"
#include "cgtarget.h"
#include "ti_si.h"
#include "cgprep.h"
#include "cg_swp.h"
#include "cg_swp_options.h"
#include "cg_swp_target.h"
#include "cg_swp_bundle.h"

// Arthur: seems like some functionality is target-independent

/* ====================================================================
 *   SWP_Dont_Bundle
 * ====================================================================
 */
void 
SWP_Dont_Bundle(SWP_OP_vector& op_state)
{
  // For register allocation purposes, assume we have one slot per
  // cycle
  //
  op_state.ii_slots = op_state.ii;
   for (INT i = 0; i < op_state.size(); i++) {
     if (op_state[i].op) {
       op_state[i].modulo_cycle = op_state[i].cycle % op_state.ii;
       op_state[i].slot = op_state[i].modulo_cycle;
     }
   }
} // SWP_Dont_Bundle

/* ====================================================================
 *   SWP_Undo_Bundle
 * ====================================================================
 */
void
SWP_Undo_Bundle(SWP_OP_vector& op_state, BB *body)
{
  for (INT i = 0; i < op_state.size(); i++) {
    if (op_state[i].is_noop && op_state[i].op) {
      BB_Remove_Op(body, op_state[i].op);
      op_state[i].op = NULL;
      op_state[i].is_noop = FALSE;
    }
    if (op_state[i].op) {
      Reset_OP_end_group(op_state[i].op);
      Reset_OP_bundled(op_state[i].op);
    }
  }
}

static const INT SWP_INVALID_OP_IDX = -1024;

static bool Trace_Swp_Bundling = false;

typedef mempool_allocator<bool>          BOOL_MEMALLOC;
typedef MATRIX<INT32, INT32_MEMALLOC>    INT32_MATRIX;
typedef MATRIX<bool, BOOL_MEMALLOC>      BOOL_MATRIX;
typedef std::list<INT32, INT32_MEMALLOC> INT32_LIST;


// A non-strict weak partial ordering on operations based on dependences,
// hints from the scheduler, heuristics based on the form of bundle 
// templates, and resource constraints.
//
class SLOT_ORDER_CMP
{
private:

  const SWP_OP_vector *_op_state;
  const CG_GROUPING   *_grouping;
  const BOOL_MATRIX   *_dep_order;
  const INT32_VECTOR  *_resource_slack;

  bool _is_loop_variant(TN *tn) const
  {
    return (TN_is_register(tn) && 
	    !TN_is_dedicated(tn) && 
	    !TN_SET_MemberP(_op_state->tn_invariants, tn));
  }
  
  bool _loop_variant_lhs(OP *op) const
  {
    bool is_variant = (OP_results(op) > 0);
    for (INT k = 0; k < OP_results(op); k++)
      is_variant = (is_variant && _is_loop_variant(OP_result(op, k)));
    return is_variant;
  }

  bool _schedule_early(INT i) const 
  {
    return (*_op_state)[i].direction == SWP_TOP_DOWN;
  }

  bool _schedule_late(INT i) const 
  {
    return (*_op_state)[i].direction == SWP_BOTTOM_UP;
  }

public:

  SLOT_ORDER_CMP(const SWP_OP_vector &op_state,
		 const CG_GROUPING   &grouping,
		 const BOOL_MATRIX   &dep_order,
		 const INT32_VECTOR  &resource_slack): 
    _op_state(&op_state), 
    _grouping(&grouping), 
    _dep_order(&dep_order), 
    _resource_slack(&resource_slack)
  {}
  
  bool operator ()(INT32 i, INT32 j) const
  {
    bool cmp = (*_dep_order)(i, j);

#if 0
    if (!cmp)
    {
      // The order is not enforced by a dependency, so use some heuristics.
      //
      OP * const op1 = (*_op_state)[i].op;
      OP * const op2 = (*_op_state)[j].op;
      const TOP  top1 = OP_code(op1);
      const TOP  top2 = OP_code(op2);

      if (_grouping->is_branch(top1) || _grouping->is_branch(top2))
      {
	// Branches always break a cycle, so order them as late as possible.
	//
	cmp = _grouping->is_branch(top2) && !_grouping->is_branch(top1);
      }
      else if ((OP_l_group(op2) && !OP_l_group(op1))               ||
	       (OP_f_group(op1) && !OP_f_group(op2))               ||
	       ((*_resource_slack)[i] < (*_resource_slack)[j])     || 
#ifdef SWP_BUNDLE_APPLY_LIFETIME_HEURISTICS
	       // apply scheduling early/late heuristics might 
	       // cause inefficient bundling.
	       (_schedule_early(i) && !_schedule_early(j))         ||
	       (_schedule_late(j) && !_schedule_late(i))           ||
	       (_loop_variant_lhs(op2) && !_loop_variant_lhs(op1)) ||
#endif
	       (_grouping->bundling_order(top1) < 
		_grouping->bundling_order(top2)))

      {
	// These heuristics are based on OP attributes (must op be first or
	// last in a group), resource_slack in options of resource use,
	// scheduler heuristics, a desire to schedule stores to loop-variants
	// late in a group (thus making lifetimes smaller for the register 
	// allocator), and the order of ops most commonly seen in the 
	// bundle templates that are available.
	//
	cmp = true;
      }
    }
#endif

    return cmp;
  }
}; // SLOT_ORDER_CMP

void 
SWP_Bundle(SWP_OP_vector& op_state, bool trace)
{
  // Group and bundle the ops in the given op_state such that they can be 
  // issued in one cycle.  
  //
  MEM_POOL bundle_pool;

  Trace_Swp_Bundling = trace;

  MEM_POOL_Initialize(&bundle_pool, "CG_SWP_ALLOCATOR POOL", FALSE);
  MEM_POOL_Push(&bundle_pool);

  FmtAssert(FALSE, ("not implemented"));

  MEM_POOL_Pop(&bundle_pool);
} // SWP_Bundle
