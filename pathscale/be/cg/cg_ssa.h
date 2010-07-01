
/* ========================================================================
 *
 *   Module that deals with the SSA form.
 *
 *   The entire program or a region of a program can be converted to the
 *   SSA form.
 *
 *   In the case of a region, the region -- a single entry, multiple
 *   exits entity, possesses the following properties:
 *
 *     1. every node in the region is dominated by the region entry;
 *     2. since we add post-exits to the nodes through which the region
 *        may be exited, such post-exits constitute a natural border
 *        of the region with the property that every post-exit is
 *        immediately dominated by a node from the region.
 *
 *   As a result of these properties the following holds:
 *
 *   Traversing the dominator tree starting at the entry node for a region
 *   will visit all nodes within this region's border without going out of
 *   this region's limits.
 *
 *   It basically means that :
 *
 *      (1) I can start traversing the dominator tree at the entry, 
 *      (2) I will necessary encounter one of the border (post-exit) nodes
 *          before I left the region -- therefore I can stop exploring the
 *          given tree branch as soon as I see a border node,
 *      (3) No node in the region will be missed by such traversal.
 *          Indeed, if a node X is not visited then its dominator tree
 *          predecessor is not visited or is not in the region. If it is
 *          not in the region, X may not be in the region because the
 *          only way to leave the region is through the border nodes
 *          that are immediately dominated by some node in the region.
 *          If X's predecessor is in the region and is not visited, then
 *          we can repeat the explanation. But eventually nodes in the
 *          region will be exhausted, which would mean that the entry
 *          node has not been visited -- nonsence, I started with it.
 *
 *   




 *   For building the SSA form, I use the algorithm by Cytron et al.
 *   ACM Transactions on Programming Languages and Systems, vol. 13,
 *   N. 4, October 1991, pp. 451 - 490.
 *
 *   The algorithm does:
 *
 *     1. compute the dominance frontier of the nodes in the program
 *        (region),
 *     2. place the phi-functions,
 *     3. rename variables.
 *
 *   Dominance frontier computation requires the bottom-up traversal of
 *   the dominator tree. The bottom-up list of nodes is constructed as
 *   a reversal of the DFS traversal of the dominator tree of the program
 *   (region).
 *   
 * ========================================================================
 */

#ifndef cg_ssa_INCLUDED
#define cg_ssa_INCLUDED

#include "tn_map.h"
#include "dominate.h"

/* ========================================================================
 *   SSA flags:
 * ========================================================================
 */
extern BOOL CG_ssa_rematerialization;

/* ========================================================================
 *   Mapping of TNs to their SSA definitions
 * ========================================================================
 */

extern MEM_POOL ssa_pool;
extern TN_MAP tn_ssa_map;
extern OP_MAP op_ssa_pinning_map;

BOOL TN_is_SSA_candidate(TN *tn);

/*
 * TN_ssa_def()
 *
 * Returns the single defining op for the TN.
 * Returns NULL if the TN is not an SSA variable.
 * This function can be called either in the SSA CGIR or in the non-SSA CGIR.
 *
 * Note that it must be used carrefully during SSA construction and SSA destruction,
 * because tn_ssa_map may not be up-to-date there, but this is a problem private to cg_ssa.cxx.
 */
inline OP *
TN_ssa_def (const TN *tn)
{
  DevAssert(tn != NULL, ("failed precondition"));
  return TN_is_register (tn) ? (OP *)TN_MAP_Get (tn_ssa_map, tn): NULL;
}

/*
 * TN_is_ssa_var()
 *
 * Returns true if the TN is a valid SSA variable.
 * This is equivalent to TN_ssa_def(tn) != NULL.
 */
inline BOOL
TN_is_ssa_var (const TN *tn)
{
  return TN_ssa_def(tn) != NULL;
}


/* The following function creates the SSA use-def link. It is
   automatically called when an operation is inserted in a
   block. However, this function MUST BE EXPLICITELY CALLED when the
   result of an operation is changed, in case of renaming for
   example. */
inline void Set_TN_ssa_def(TN *t, OP *o) {
  if (tn_ssa_map != NULL && TN_is_SSA_candidate(t)) {
#ifdef Is_True_On
    if (o && TN_ssa_def(t))
      Is_True(!o || !TN_ssa_def(t), ("Set_TN_ssa_def cannot be called on a TN with an SSA def."));
#endif
    TN_MAP_Set(tn_ssa_map, t, o);
  }
}


/* The two following functions update the SSA use-def link when an
   operation is inserted into or removed from a basicblock. 
*/
extern void SSA_Disable();
extern void SSA_Enable();
extern void SSA_setup(OP *o);
extern void SSA_unset(OP *o);

//
// Iterate over PHI-nodes in BB
//
#define FOR_ALL_BB_PHI_OPs(bb,op)    \
  for (op = BB_first_op(bb); op != NULL && OP_phi(op); op = OP_next(op))

inline BOOL SSA_Active (void) {
  return tn_ssa_map != NULL;
}

extern void SSA_init_call_parms(RID *rid, BOOL region);
extern BOOL SSA_Check (RID *rid, BOOL region);
extern BOOL SSA_Verify (RID *rid, BOOL region);
extern void SSA_Enter (RID *rid, BOOL region);
extern void SSA_Update ();

// Insert a PHI operation in a BB, without renaming
OP *SSA_Place_Phi_In_BB(TN *tn, BB *bb);

// which BB corresponds to PHI-node operand 'opnd_idx' ?
extern BB*  Get_PHI_Predecessor (const OP *phi, UINT8 opnd_idx);
extern void Set_PHI_Predecessor (const OP *phi, UINT8 pos, BB *pred);
extern void Change_PHI_Predecessor (const OP *phi, BB *pred, BB *new_pred);

// which opnd_idx corresponds to PHI-node predecessor BB  ?
extern UINT8 Get_PHI_Predecessor_Idx (const OP *phi, BB *);

extern BOOL OP_Has_ssa_pinning(const OP *op);
extern TN * OP_Get_opnd_pinning(const OP *op, INT opnd_idx);
extern TN * OP_Get_result_pinning(const OP *op, INT res_idx);

// which guard TN is associated to PSI-node operand 'opnd_idx' ?
#define PSI_opnds(psi)    (OP_opnds(psi)>>1)
#define PSI_opnd(psi, i)  (OP_opnd(psi, ((i)<<1)+1))
extern void Set_PSI_opnd(OP *, UINT8, TN *);
extern TN *PSI_guard(const OP *, UINT8);
extern void Set_PSI_guard(OP *, UINT8, TN *);
extern void Set_PSI_Pred(OP *, UINT8, BOOL);

#ifdef EFFECT_PRED_FALSE
// (cbr) Support for guards on false
#define PSI_Pred_False(psi, i) OP_Pred_False(psi, (i)<<1)
#define Set_PSI_Pred_True(psi, i) Set_PSI_Pred(psi, i, FALSE)
#define Set_PSI_Pred_False(psi, i) Set_PSI_Pred(psi, i, TRUE)
#else
#define PSI_Pred_False(psi, i) 0
#define Set_PSI_Pred_True(psi, i) 
#define Set_PSI_Pred_False(psi, i)
#endif

// Sort PHI operands according to the dominance relation of the
// argument's definition
extern void Sort_PHI_opnds (OP *phi);

// Create a PSI operation from a PHI
extern OP * Convert_PHI_to_PSI (OP *phi);

// Simplify a PSI operation
extern OP * PSI_inline (OP *psi);
extern OP * PSI_reduce (OP *psi);

// Conditional move operations
extern void OP_Make_movc (TN *guard, TN *dst, TN *src, OPS *cmov_ops, bool on_false);

#define OP_guard(op) OP_has_predicate(op) ? OP_Predicate(op) : True_TN

//
// Initialise the 'phi_op' mapping
//
extern OP_MAP phi_op_map;
extern void Initialize_PHI_map(OP   *phi);

// Tracing flags
#define SSA_BUILD        0x00000001  /* trace SSA build */
#define SSA_MAKE_CONST   0x00000002  /* trace make conventional */
#define SSA_IGRAPH       0x00000004  /* trace interference graph */
#define SSA_REMOVE_PHI   0x00000008  /* trace PHI-removal */
#define SSA_DOM_FRONT    0x00000010  /* trace dominance frontier */
#define SSA_COLLECT_INFO 0x00000020  /* collect statistics for SSA */
#define SSA_DEAD_CODE    0x00000040  /* SSA dead code removal */
#define SSA_CBPO	 0x00000080  /* common base pointer optimization */
#define SSA_

/* FdF 20050309: Externalized for use in SSA_Optimize(). */

/* ================================================================
 *
 *   Dominator routines
 *
 *   In addition to BB_dom_set(), BB_pdom_set() provided by the
 *   dominate.h interface, we need things such as BB_children(),
 *   BB_idom(), and the bottom-up traversal of dominator tree.
 *
 *   TODO: we should really generalize this and merge into
 *         the dominate.[h,cxx] interface.
 * ================================================================
 */

typedef struct _Dom_Tree_node {
  BB *_M_data;
  BB *_M_parent;
  BB_LIST *_M_kids;
} DOM_TREE;

#define DOM_TREE_node(t)   (t->_M_data)
#define DOM_TREE_kids(t)   (t->_M_kids)
#define DOM_TREE_parent(t) (t->_M_parent)

//
// This table is indexed with BB_id(bb). Each entry contains
// the DOM_TREE info for this bb
//
extern DOM_TREE *dom_map;

#define BB_dominator(bb)         (dom_map[BB_id(bb)]._M_parent)
#define BB_children(bb)          (dom_map[BB_id(bb)]._M_kids)

/* [SC] Externalized for use in range analysis. */

extern BOOL BB_is_SSA_region_exit (BB *bb);
extern BOOL BB_is_SSA_region_entry (BB *bb);
extern const BB_SET *SSA_region_entries ();
extern const BB_SET *SSA_region_exits ();

extern void SSA_Dominance_init(RID *rid);
extern void SSA_Dominance_fini();

extern INT32 CG_ssa_variables;
extern INT32 PU_ssa_variables;
#define SSA_VAR_NON_DEDICATED    0x1
#define SSA_VAR_DEDICATED_LOCAL  0x2
#define SSA_VAR_DEDICATED_GLOBAL 0x4
#define SSA_VAR_DEDICATED (SSA_VAR_DEDICATED_LOCAL | SSA_VAR_DEDICATED_GLOBAL)

extern INT32 CG_ssa_coalescing;
#define SSA_IN_DO_COPY_PROP    0x1
#define SSA_OUT_ON_SSA_MOVE    0x2
#define SSA_OUT_ON_ALL_SAMERES 0x4
#define SSA_OUT_ON_COPY        0x8
#define SSA_OUT_COALESCING    (SSA_OUT_ON_SSA_MOVE | SSA_OUT_ON_ALL_SAMERES | SSA_OUT_ON_COPY)

// Target specific function to check for dedicated registers that can
// be renamed under SSA
BOOL CGTARG_SSA_dedicated_candidate_tn(TN *tn);

#endif /* cg_ssa_INCLUDED */
