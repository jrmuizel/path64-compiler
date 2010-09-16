/*
  Copyright (C) 2005, STMicroelectronics, All Rights Reserved.

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
 * Module: range_analysis.h
 *
 * Description:
 *
 *   External interface to the range analysis module.
 *
 * Entry-point:
 *
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef RANGE_ANALYSIS_H_INCLUDED
#define RANGE_ANALYSIS_H_INCLUDED

#ifndef WORKLIST_H_INCLUDED
#include "worklist.h"
#endif

#ifndef TN_SET_H_INCLUDED
#include "tn_set.h"
#endif

#ifndef TN_MAP_H_INCLUDED
#include "tn_map.h"
#endif

#ifndef OP_LIST_H_INCLUDED
#include "op_list.h"
#endif

#ifndef LRANGE_H_INCLUDED
#include "lrange.h"
#endif

// Trace flags:
// -Wb,-ttran:flags
#define RANGE_ANALYSIS_FORWARD     1
#define RANGE_ANALYSIS_BACKWARD    2
#define RANGE_PROPAGATION          4
#define RANGE_ANALYSIS_EQUIVALENCE 8

typedef Worklist<const TN *> TN_Worklist;
typedef Worklist<BB *> BB_Worklist;

// TN_Lattice_Info holds the lattice information for a TN.
struct TN_Lattice_Info {
  LRange_p value;        // Lattice value of TN.
  INT lowered_count;  // Count the number of times this TN has been
                      // lowered.
};

/* ====================================================================
 *
 *   EquivClass
 *
 *     Simple class for creating equivalence classes.
 *
 * ====================================================================
 */
class EquivClass {
private:
  EquivClass *parent;
  INT count;
  UINT id;
  void Make_Parent_Of (EquivClass *child) {
    child->parent = this;
    count += child->count;
  }
public:
  EquivClass *Find ();
  BOOL Equivalent (EquivClass &e2) {
    return Find () == e2.Find ();
  }
  void Merge (EquivClass *e2) {
    if (e2 != this) {
      if (this->count > e2->count)
	Make_Parent_Of (e2);
      else
	e2->Make_Parent_Of (this);
    }
  }
  UINT Id () { return id; }
  EquivClass (UINT id) : id (id), parent (NULL), count (1) {}
};

/* ====================================================================
 *
 *   TN_Equivalence
 *
 *     Track SSA TN equivalence.
 *
 * ====================================================================
 */
class TN_Equivalence {
private:
  static MEM_POOL pool;
  TN_MAP tn_equivalences;
  // Find equivalence class of (tn, reg).  Create if non-existant.
  EquivClass *Equivalence_Class (const TN *tn, INT reg);
  void Merge_Equivalence_Classes (const TN *tn1, INT reg1,
				  const TN *tn2, INT reg2);
  const TN **equiv_class_root;
  UINT equiv_class_id;
  mBOOL trace_equivalence;
public:
  BOOL Equivalent (const TN *tn1, const TN *tn2) const;
  BOOL Equivalent (const TN *tn1, INT reg1, const TN *tn2, INT reg2) const;
  void Build_Equivalence_Classes ();
  void SetTracing (BOOL tracing) { trace_equivalence = tracing; }
  void Trace_Equiv_Class_Merge (const TN *tn1, INT reg1,
				const TN *tn2, INT reg2) const;
  const TN *Root_Equivalent (const TN *tn) const {
    EquivClass *e = (EquivClass *)TN_MAP_Get (tn_equivalences, tn);
    if (e) {
      const TN *e_tn = equiv_class_root[e->Id ()];
      if (e_tn) return e_tn;
    }
    return tn;
  }
  TN_Equivalence ()  {
    MEM_POOL_Initialize (&pool, "TN_Equivalence pool", TRUE);
    tn_equivalences = NULL;
    equiv_class_id = 0;
    trace_equivalence = FALSE;
  }
  ~TN_Equivalence () {
    if (tn_equivalences) TN_MAP_Delete (tn_equivalences);
    MEM_POOL_Delete (&pool);
  }
};
  
/* ====================================================================
 *
 *   Opnd_Info
 *
 *     Information on operands used by backwards analysis.
 *
 * ====================================================================
 */
class Opnd_Info {
 private:
  OP *op;             // The OP to which this is an operand.
  INT opnd_idx;       // The operand number for OP.
  LRange_p value;        // Lattice value of operand during analysis.
  INT lowered_count;  // Count the number of times this value has been
                      // lowered.
  
 public:
  LRange_p Value () const { return value; }
  void Set_Value (LRange_p v) { value = v; }
  INT LoweredCount () const { return lowered_count; }
  void Set_LoweredCount (INT v) { lowered_count = v; }
  INT IncrementLoweredCount () { return ++lowered_count; }
  BB *Bb () const { return OP_bb(op); }
  OP *Op () const { return op; }
  void Set_Op (OP *p) { op = p; }
  INT Opnd_idx () const { return opnd_idx; }
  void Set_Opnd_idx (INT idx) { opnd_idx = idx; }
  TN *Tn () const { return OP_opnd(op, opnd_idx); }
};

class RangeAnalysis {
 private:
  static MEM_POOL pool;
//   Lattice<Range> *lattice;
  LRange_p lattice;
  TN_Worklist tn_work;
  BB_Worklist bb_work;
  TN_SET *ssa_tns;
  TN_MAP tn_value;  // Used for value calculation during fwd and bwd analysis.
  TN_MAP tn_value_forward; // After fwd analysis, contains the results.
  TN_MAP tn_value_backward; // After bwd analysis, contains the results.
  BB_SET *executed;  // Set of executed BBs.
  BB_SET *visited_backward;  // Set of BBs visited in backward analysis.
  TN_MAP tn_uses;    // Map from TN to set of uses;
  OP_MAP opnd_info_map; // Map from op to its opnd_info.
  BB **topological_order; // Array of BBs in topological order.
  INT n_topological_order; // Size of topological_order array.
  BB_MAP ordered_succs; // Map from BB to ordered list of its successors.
  enum { None, Forward, Backward } analysis;
  mBOOL forward_valid, backward_valid;
  mBOOL handler_seen;
  mBOOL trace_forward;
  mBOOL trace_backward;
  mBOOL trace_equivalence;
  TN_Equivalence tn_equivalence;

  void Trace_SSA_TNs () const;
  void Trace_SetValue (const TN* tn, const TN_Lattice_Info &new_value,
		       const LRange_p old_value) const;
  void Trace_Recompute (BB *bb) const;
  void Trace_Recompute (const TN *tn) const;
  void Trace_Recompute (const Opnd_Info *use) const;
  void Trace_Visit (OP *op) const;
  void Trace_Visit (BB *bb) const;
  void Trace_Visit (const TN *tn) const;
  void Trace_Analyze_Forward_Start () const;
  void Trace_Analyze_Forward_End () const;
  void Trace_Analyze_Backward_Start () const;
  void Trace_Analyze_Backward_End () const;

  void Add_Use (const TN *, OP *);
  void Initialize_TN_Uses ();
  void Initialize_SSA_TNs ();
  BB *BB_ordered_succ (BB *bb, INT i) {
    return ((BB **)BB_MAP_Get (ordered_succs, bb))[i];
  }
  INT LoweredCount (const TN *);
  INT IncrementLoweredCount (const TN *);
  LRange_p Value (const TN *);  // Get current value of TN.
  void SetValue (const TN *tn, LRange_p v); // Set current value of TN.
  void Create_Opnd_Info ();
  void Create_Topological_Ordering ();
  void Recompute_Forward (const TN *tn, LRange_p &v); 
  void Recompute_Forward (BB *bb);
  void Recompute_Backward (OP *op, INT opnd_idx, LRange_p &new_value);
  void Recompute_Backward (BB *bb);
  BOOL isExecuted (BB *bb) const { return BB_SET_MemberP (executed, bb); }
  void SetExecuted (BB *bb) { executed = BB_SET_Union1 (executed, bb, &pool); }
  BOOL isVisitedBackward (BB *bb) const {
    return BB_SET_MemberP (visited_backward, bb);
  }
  void SetVisitedBackward (BB *bb) {
    visited_backward = BB_SET_Union1 (visited_backward, bb, &pool);
  }
  Opnd_Info *Find_Opnd_Info (OP *op, INT opnd_idx) {
    return ((Opnd_Info *)OP_MAP_Get (opnd_info_map, op)) + opnd_idx;
  }
  void Visit_Forward (OP *op, BOOL *succs_done);
  void Visit_Forward (OP_LIST *uselist);
  void Visit_Backward (OP *op);
  BOOL TARG_Visit_Forward (OP *op, INT result_idx, LRange_pc &new_value,
			   BOOL *succs_done);
  BOOL TARG_Visit_Backward (OP *op, INT opnd_idx, LRange_pc &new_value);

public:
  void Initialize (LRange_p instance, MEM_POOL pool);       // Set up for Analyze.
  void Initialize_Backward (LRange_p instance);       // Set up for Analyze.
  BOOL Can_Analyze () { return !handler_seen; }
  void Finalize ();         // Close down after use.
  void Build_Equivalence_Classes ();
  void Analyze_Forward ();  // Perform forward range analysis.
  void Analyze_Backward (); // Perform backward range analysis.
  // After Analyze_Forward(), this gives
  // the computed range based on reaching defs.
  LRange_p Get_Value (const TN *) const;
  BOOL Forward_Valid () const { return forward_valid; }
  // After Analyze_Backward(), this
  // gives the required range based on reached
  // uses.
  LRange_p Get_Value_Backward (const TN *) const;
  BOOL Backward_Valid () const { return backward_valid; }
  // After Analyze_Forward(), this gives a list of
  // the instructions that use the TN.
  OP_LIST *Uses (const TN *) const;
  BOOL Equivalent (const TN *tn1, const TN *tn2) const {
    return tn_equivalence.Equivalent (tn1, tn2);
  }
  BOOL Equivalent (const TN *tn1, INT reg1, const TN *tn2, INT reg2) const {
    return tn_equivalence.Equivalent (tn1, reg1, tn2, reg2);
  }
  const TN *Root_Equivalent (const TN *match_tn) const {
    return tn_equivalence.Root_Equivalent (match_tn);
  }
  LRange_p getLattice () const {
    return lattice;
  }
};

#endif /* RANGE_ANALYSIS_H_INCLUDED */
