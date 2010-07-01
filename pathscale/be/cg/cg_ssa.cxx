
#include "W_alloca.h"
// [HK]
#if __GNUC__ >= 3
#include <list>
// using std::list;
#else
#include <list.h>
#endif // __GNUC__ >= 3

#include "defs.h"
#include "tracing.h"
#include "errors.h"
#include "mempool.h"
#include "cxx_memory.h"
#include "glob.h"
#include "bitset.h"
#include "config_target.h"
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

#include "gra_live.h"
#include "cgexp.h"
#include "cg_spill.h"	// for Attach_IntConst_Remat()
#include "cg_ssa.h"
#include "cg_outssa.h"
#include "cg_affirm.h"
#include "wn_map.h"

BOOL  CG_ssa_rematerialization = TRUE;
INT32 CG_ssa_variables  = SSA_VAR_NON_DEDICATED | SSA_VAR_DEDICATED_LOCAL;
INT32 PU_ssa_variables = 0; // Per PU value of CG_ssa_variables
INT32 CG_ssa_coalescing = SSA_OUT_ON_SSA_MOVE;

//
// Memory pool for allocating things during SSA construction.
// Memory allocated from this pool is initialized to zero.
//
MEM_POOL ssa_pool;

static BOOL Trace_SSA_Build;                  /* -Wb,-tt60:0x001 */
static BOOL Trace_dom_frontier;               /* -Wb,-tt60:0x010 */

/* ================================================================
 *
 *   Mapping TNs -> BBs where they are defined
 *
 * ================================================================
 */

static TN_MAP tn_def_map = NULL;

#define TN_is_def_in(t)        ((BB_LIST *)TN_MAP_Get(tn_def_map,t))

/* ================================================================
 *
 *   TNs  use/def managment.
 *
 * ================================================================
 */
static void
OP_Attach_Rematerialization(OP *op)
{
  if (!CG_ssa_rematerialization) return;
  
  if (OP_Is_Copy(op)) {
    TN *src = OP_opnd(op, OP_Copy_Operand(op));
    TN *dst = OP_result(op, OP_Copy_Result(op));
    // FdF 20070523: dst must be an SSA var to propagate the
    // Rematerializable property.
    if (TN_is_ssa_var(dst) && !TN_is_rematerializable(dst) &&
	TN_size(src) == TN_size(dst) &&
	(TN_size(src) <= 4 || TN_size(src) == 8)) {
      if (TN_has_value(src)) {
	CGSPILL_Attach_Intconst_Remat(dst, TN_size(src) <= 4 ? MTYPE_I4 : MTYPE_I8, 
				      TN_value(src));
      } else if (TN_is_register(src) && TN_is_rematerializable(src)) {
	Set_TN_is_rematerializable(dst);
	Set_TN_remat(dst, TN_remat(src));
      }
    }
  }
}

static TN_MAP save_ssamap = NULL;
void
SSA_Disable() {
  if (tn_ssa_map != NULL) {
    save_ssamap = tn_ssa_map;
    tn_ssa_map = NULL;
  }
}

void
SSA_Enable() {
  if (save_ssamap != NULL) {
    tn_ssa_map = save_ssamap;
    save_ssamap = NULL;
  }
}

void 
SSA_setup(OP *o) 
{
  if (tn_ssa_map == NULL) return;
  
  for (int i = 0; i < OP_results(o); i++)
      Set_TN_ssa_def(OP_result(o, i), o);
  
  /* Update on the fly rematerialization information. */
  OP_Attach_Rematerialization(o);
}

void
SSA_unset(OP *o) 
{
  if (tn_ssa_map)
    for (int i = 0; i < OP_results(o); i++)
      Set_TN_ssa_def(OP_result(o, i), NULL);
}


/* ================================================================
 *
 *   Renaming stack management
 *
 * ================================================================
 */
//
// TN stack is implemented as list
//
typedef struct _stack_item {
  TN *tn;
  struct _stack_item *next;
} STACK_ITEM;

static STACK_ITEM **tn_stack; // each TN has a corresponding list

#define TN_STACK_empty(t)     (tn_stack[TN_number(t)] == NULL)
#define TN_STACK_ptr(t)       (tn_stack[TN_number(t)])
#define TN_STACK_tn(t)        ((tn_stack[TN_number(t)])->tn)
#define TN_STACK_next(t)      ((tn_stack[TN_number(t)])->next)

// Record the number of base TN before starting adding new TNs
static  INT TN_STACK_size;

// For each BB, a set of pushed TNs is used, so as to:
// - Allocate a new entry in the stack for a base TN only once per basic block
// - Pop easily at the end of the basic block
static TN_SET *TN_STACK_cur_stacked_set;

/* ================================================================
 *  initialize_tn_stack
 * ================================================================
 */
static void 
initialize_tn_stack (void)
{
  //
  // Initialize this for all TNs to NULL
  //
  TN_STACK_size = Last_TN+1;
  tn_stack = (STACK_ITEM**)TYPE_MEM_POOL_ALLOC_N(STACK_ITEM*, &MEM_local_pool, TN_STACK_size);
}

/* ================================================================
 *   tn_stack_pop
 *
 *   pop an element from the tn stack.
 * ================================================================
 */
static void
tn_stack_pop (
  TN *base
)
{
  Is_True(!TN_STACK_empty(base), ("Cannot POP on an empty stack"));
  TN_STACK_ptr(base) = TN_STACK_next(base);
}

/* ================================================================
 *  tn_stack_push
 *
 *  push an element on the region stack; 
 *  grow the stack if necessary
 *
 *  FdF 20090721: Push only once for each basic block, otherwise
 *  simply reuse the top of stack
 * ================================================================
 */
static void 
tn_stack_push (
  TN *base,
  TN *tn
)
{
  // add an item to the top of the list for 'base'
  if (!TN_SET_MemberP(TN_STACK_cur_stacked_set, base)) {
    STACK_ITEM *st = TYPE_MEM_POOL_ALLOC(STACK_ITEM, &MEM_local_pool);
    st->tn = tn;
    st->next = TN_STACK_ptr(base);
    TN_STACK_ptr(base) = st;
    TN_SET_Union1D(TN_STACK_cur_stacked_set, base, NULL);    
  }
  else {
    Is_True(!TN_STACK_empty(base), ("Cannot replace top of empty stack"));
    TN_STACK_ptr(base)->tn = tn;
  }
}

/* ================================================================
 *   tn_stack_top
 *
 *  returns the top element from the tn stack of given 'base' name
 * ================================================================
 */
static TN*
tn_stack_top (
  TN *base
)
{
  if (TN_STACK_empty(base)) {
    if (TN_is_SSA_candidate(base))
      DevWarn("Uninitialized PHI arg");
    return base;
  }
  else
    return TN_STACK_tn(base);
}

/* ================================================================
 *   Insert_Kill_op
 *
 *  Insert a Pseudo Kill operation. This is needed for uninitialized
 *  uses and for predicated code.
 * ================================================================
 */
static TN *Copy_TN (TN *tn);

static void
Insert_Kill_op (
  BB *bb,
  OP *point,
  TN *tn
)
{
  // In case this a dedicated register, the register MUST have been
  // defined, either in the prolog or on a call.
  FmtAssert(!TN_is_dedicated(tn), ("Missing definition for dedicated register in prolog or call."));

  // FdF: Create a pseudo-def for uninitialized uses.
  TN *new_tn = Copy_TN(tn);
  tn_stack_push(tn, new_tn);
  OP* kill_op = Mk_VarOP(TOP_KILL, 1, 0, &new_tn, NULL);
  if (point)
    BB_Insert_Op_Before(bb, point, kill_op);
  else
    BB_Append_Op(bb, kill_op);
}

/* ================================================================
 *      Auxilliary REGION handling stuff
 * ================================================================
 */

static BB_SET *region_entry_set;
static BB_SET *region_exit_set;

//
// This table is indexed with BB_id(bb). Each entry contains
// the DOM_TREE info for this bb
//
DOM_TREE *dom_map = NULL;

#define Set_BB_dominator(bb,dom) (dom_map[BB_id(bb)]._M_parent = dom); 

inline void Add_BB_child(BB *bb, BB *child) {
  dom_map[BB_id(bb)]._M_kids = BB_LIST_Push(child, dom_map[BB_id(bb)]._M_kids, &ssa_pool);
}

/* ================================================================
 *   DOM_TREE_Print
 * ================================================================
 */
static void
DOM_TREE_Print (
  FILE *file
)
{
  BB *bb;

  fprintf(file, "<ssa> DOMINATOR_INFO: \n\n");
  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    fprintf(file, "%s DOM_INFO for BB%d \n%s", 
	    DBar, BB_id(bb), DBar);

    // BB dominators set
    fprintf(file, "   doms: ");
    BB_SET_Print(BB_dom_set(bb), file);
    fprintf(file, "\n");

    // immediate dominator
    if (BB_dominator(bb))
      fprintf(file, "   idom: BB%d\n", BB_id(BB_dominator(bb)));
    else
      fprintf(file, "   idom: NONE\n");

    // BBs dominated by BB
    fprintf(file, "   kids: ");
    for (BB_LIST *elist = BB_children(bb); elist; elist = BB_LIST_rest(elist)) {
      BB *kid = BB_LIST_first(elist);
      fprintf(file, "%d ", BB_id(kid));
    }
    fprintf(file, "\n");

    fprintf(file, "%s\n", DBar);
  }

  return;
}

/* ================================================================
 *   DOM_TREE_Initialize
 *
 *   NOTE:  BBs are numbered from 1, so their indices in the bit 
 *          vectors start from 1. The immediate dominators of 
 *          entry BBs is set to NULL.
 *
 * ================================================================
 */
static void
DOM_TREE_Initialize ()
{
  INT i;
  BB *bb;


  // initialize dominator tree
  dom_map = (DOM_TREE *)TYPE_MEM_POOL_ALLOC_N(DOM_TREE, 
					      &ssa_pool, 
					      PU_BB_Count+2);

  for (i = 1; i < PU_BB_Count+2; i++) {
    dom_map[i]._M_kids = NULL;
  }

  //  RID *rid = BB_rid(REGION_First_BB);
  //  CGRIN *cgrin = rid ? RID_Find_Cginfo(REGION_First_BB) : NULL;

  // 
  // The immediate dominator of BBi is the BB with the same set of
  // dominators except for BBi itself:
  //
  BB_SET *match, *check;
  match = BB_SET_Create_Empty(PU_BB_Count+2, &ssa_pool);
  check = BB_SET_Create_Empty(PU_BB_Count+2, &ssa_pool);
  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    BB *cur;

    // 
    // match is the BS (set of dominators) to be matched:
    //
    match = BB_SET_CopyD(match, BB_dom_set(bb), &ssa_pool);
    match = BB_SET_Difference1D(match, bb);	       // ... except bb.

    // 
    // match is also the set of dominators to check:
    //
    check = BB_SET_CopyD(check, match, &ssa_pool);  // Check all bb's dominators ...

    //
    // Now go check them.  We start with the last (highest numbered)
    // dominator, because that will be the immediate dominator if we
    // are dealing with a topologically-sorted BB list:
    //
    //Set_BB_dominator(bb, NULL); -- alredy zeroed
    //
    FOR_ALL_BB_SET_members(check, cur) {
      if (BB_SET_EqualP(match, BB_dom_set(cur))) {

	// parent/child:
	Set_BB_dominator(bb, cur);

	// at the same time, bb is a child of cur's
	Add_BB_child(cur, bb);

	break;
      }
    }

    // FdF 20050826: Basic blocks with no immediate dominator are
    // either Entry nodes, or basic blocks that can be reached from
    // different Entry nodes. These latter nodes must also be
    // considered as entry nodes for the purpose of SSA

    if (!BB_dominator(bb) && BB_preds(bb)) {
      region_entry_set = BB_SET_Union1D(region_entry_set,
					bb,
					&ssa_pool);
    }

  }

  if (Trace_dom_frontier) 
    DOM_TREE_Print(TFile);

  return;
}

/* ================================================================
 *   DOM_TREE_Finalize
 * ================================================================
 */
static void
DOM_TREE_Finalize()
{ 
}

//
// The RID associated with the entire REGION being analyzed
//
static RID *ssa_scope_rid;

/* ================================================================
 *   Set_Entries_Exits
 *
 *   Copy of the GRA_LIVE_xxx function.
 * ================================================================
 */
static void
Set_Entries_Exits( 
  RID *rid 
)
{
  CGRIN *cgrin;
  BB *bb;
  INT i,n;

  if ( rid == NULL ) {
    /*
     * whole PU
     */
    for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
      if (BB_succs(bb) == NULL)
	region_exit_set = BB_SET_Union1D(region_exit_set,
					 bb,
					 &ssa_pool);
      if (BB_preds(bb) == NULL)
	region_entry_set = BB_SET_Union1D(region_entry_set,
					  bb,
					  &ssa_pool);
    }
    return;
  }

  /*
   * use entry and exit info from the CGRIN
   */
  cgrin = RID_cginfo( rid );
  bb = CGRIN_entry( cgrin );
  region_entry_set = BB_SET_Union1D(region_entry_set,
				    bb,
				    &ssa_pool);
  n = RID_num_exits( rid );
  for (i = 0; i < n; i++) {
    bb = CGRIN_exit_i( cgrin, i );
    region_exit_set = BB_SET_Union1D(region_exit_set,
				     bb,
				     &ssa_pool);
  }
  ssa_scope_rid = rid;
  return;
}

BOOL
BB_is_SSA_region_entry (BB *bb)
{
  return BB_SET_MemberP (region_entry_set, bb);
}

BOOL
BB_is_SSA_region_exit (BB *bb)
{
  return BB_SET_MemberP (region_exit_set, bb);
}

const BB_SET *
SSA_region_entries ()
{
  return region_entry_set;
}

const BB_SET *
SSA_region_exits ()
{
  return region_exit_set;
}


void
SSA_Dominance_init(
  RID *rid 
)
{
  /* Why + 2?  Nobody seems to know.
   */
  region_exit_set  = BB_SET_Create_Empty(PU_BB_Count + 2,&ssa_pool);
  region_entry_set = BB_SET_Create_Empty(PU_BB_Count + 2,&ssa_pool);

  // Initialize some auxilliary data structures
  Set_Entries_Exits(rid);

  // First, calculate the dominator/postdominator information:
  Calculate_Dominators();

  //initialize_tn_stack();
  DOM_TREE_Initialize();
}

void
SSA_Dominance_fini() {
  DOM_TREE_Finalize();
}

/* ================================================================
 *
 *                         SSA algorithm
 *
 * ================================================================
 */

//
// Only rename TNs that are not dedicated or a save for callee
// saves
// Temporary: Do not rename TN register class with few registers (< 4).
// This is to avoid generating useless moves in the presence of
// automodified operations. A better handling would
// involve treating automodified better in the out of ssa phase.
// Typical example is:
// A <- B
// C <- ASM A (and must be sameres)
// the current out of ssa (in repair_machine_constraints ()) will do:
// A <- B
// C <- A	
// C <- ASM C (and must be sameres)
// While if A is dead it should do:
// C <- B
// C <- ASM C
//

static BOOL
TN_need_global_pinning(TN *tn) {
  return (TN_is_sp_reg(tn) || TN_is_gp_reg(tn) || TN_is_fp_reg(tn));
}

BOOL
TN_need_operand_pinning(OP *op, TN *tn) {
  return (OP_call(op) && TN_is_dedicated(tn));
}

static TN_SET *dedicated_candidate_set = NULL;
static void
SSA_init_dedicated_TN_candidates() {

  if (!(PU_ssa_variables & SSA_VAR_DEDICATED))
    return;

  dedicated_candidate_set = TN_SET_Create_Empty (Last_Dedicated_TN+1, &ssa_pool);

  TN *tn;
  TN_NUM tn_num;
  for (tn_num = 0; tn_num <= Last_Dedicated_TN; tn_num++) {
    if ((tn = TNvec(tn_num)) == NULL)
      continue;
    
    // These registers can never be renamed as SSA variables.
    if (   TN_is_const_reg(tn)
	|| TN_is_static_link_reg(tn)
        || TN_is_link_reg(tn)
        || TN_is_pfs_reg(tn)
        || TN_is_ra_reg(tn)
        || TN_is_tp_reg(tn)
	|| TN_is_save_reg(tn))
      continue;

    // Depending on the target, some dedicated registers cannot be
    // renamed as SSA variables.
    if (!CGTARG_SSA_dedicated_candidate_tn(tn))
      continue;

    if (TN_need_global_pinning(tn)) {
      if (PU_ssa_variables & SSA_VAR_DEDICATED_GLOBAL)
	TN_SET_Union1D(dedicated_candidate_set, tn, NULL);
      
    }
    else {
      if (PU_ssa_variables & SSA_VAR_DEDICATED_LOCAL)
	TN_SET_Union1D(dedicated_candidate_set, tn, NULL);
    }
  }
}

BOOL
TN_is_SSA_candidate(TN *tn) {
  static ISA_REGISTER_CLASS Integer_Register_Class = CGTARG_Register_Class_For_Mtype(MTYPE_I4);
  static ISA_REGISTER_CLASS Float_Register_Class = CGTARG_Register_Class_For_Mtype(MTYPE_F4);
  static ISA_REGISTER_CLASS Predicate_Register_Class = CGTARG_Register_Class_For_Mtype(MTYPE_B);

  if (!TN_is_register(tn)) return FALSE;
  // FdF 20100118: Save registers cannot be renamed, they are needed
  // for register allocation in exception handlers.
  if (TN_is_save_reg(tn)) return FALSE;

  ISA_REGISTER_CLASS cl = TN_register_class(tn);
  if ((cl != Integer_Register_Class) &&
      (cl != Float_Register_Class) &&
      (cl != Predicate_Register_Class))
    return FALSE;

  if (!TN_is_dedicated(tn)) {
    if (PU_ssa_variables & SSA_VAR_NON_DEDICATED)
      return TRUE;
  }

  else if ((PU_ssa_variables & SSA_VAR_DEDICATED) &&
	   TN_SET_MemberP(dedicated_candidate_set, tn))
    return TRUE;

  return FALSE;
}

//
// 1. We need some mapping between PHI-function opnds
//    and BB predecessors of the BB where the PHI-function lives.
//
typedef struct phi_map_entry {
  BB **opnd_src;    /* table of pred BBs for each operand */
} PHI_MAP_ENTRY;

OP_MAP phi_op_map = NULL;

//
// 2. We need mapping between each TN and its definition.
//    This is exported, so everybody can have access.
//
TN_MAP tn_ssa_map = NULL;

//
// When dedicated TNs are renamed, the original dedicated must be kept
// so as to create a 'pinning' for the out-of-ssa algorithm.
OP_MAP op_ssa_pinning_map = NULL;

//
/* ================================================================
 *   Initialize_PHI_map
 * ================================================================
 */
void
Initialize_PHI_map(
  OP   *phi
)
{
  Is_True(OP_phi(phi),("not a PHI function"));
  PHI_MAP_ENTRY *entry = TYPE_MEM_POOL_ALLOC(PHI_MAP_ENTRY, 
					     &ssa_pool);

  entry->opnd_src = TYPE_MEM_POOL_ALLOC_N(BB *, 
					  &ssa_pool, 
					  OP_opnds(phi));

  OP_MAP_Set(phi_op_map, phi, entry);
  
  return;
}

/* ================================================================
 *   Get_PHI_Predecessor
 * ================================================================
 */
BB*
Get_PHI_Predecessor (
  const OP *phi,
  UINT8 i
)
{
  Is_True(OP_phi(phi),("not a PHI function"));
  PHI_MAP_ENTRY *entry = (PHI_MAP_ENTRY *)OP_MAP_Get(phi_op_map, phi);
  if (!entry) {
    DevWarn("Uninitialized PHI predecessors");
    return NULL;
  }

  return entry->opnd_src[i];
}

/* ================================================================
 *   Get_PHI_Predecessor_Idx
 * ================================================================
 */
UINT8
Get_PHI_Predecessor_Idx (
  const OP *phi,
  BB *bb
)
{
  Is_True(OP_phi(phi),("not a PHI function"));
  PHI_MAP_ENTRY *entry = (PHI_MAP_ENTRY *)OP_MAP_Get(phi_op_map, phi);
  for (INT i = 0; i < OP_opnds(phi); i++) {
    if (entry->opnd_src[i] == bb)
      return i;
  }
  FmtAssert(FALSE, ("Get_PHI_Predecessor_Idx: BB %d not a predecessor of BB %d", BB_id(bb), BB_id(OP_bb(phi))));

}

/* ================================================================
 *   Set_PHI_Predecessor
 * ================================================================
 */
void Set_PHI_Predecessor (
 const OP *phi,
 UINT8 pos,
 BB *pred
)
{
  PHI_MAP_ENTRY *entry = (PHI_MAP_ENTRY *)OP_MAP_Get(phi_op_map, phi);
  Is_True(entry != NULL,("unmapped op ?"));

  entry->opnd_src[pos] = pred;
}

/* ================================================================
 *   Change_PHI_Predecessor
 * ================================================================
 */
void Change_PHI_Predecessor (
 const OP *phi,
 BB *pred,
 BB *new_pred
)
{
  UINT8 npreds = OP_opnds(phi);

  for (UINT8 i = 0; i < npreds; i++) {
    BB *ppred = Get_PHI_Predecessor (phi, i);
    if (ppred == pred) 
      Set_PHI_Predecessor (phi, i, new_pred);
  }
}

/* ================================================================
 *   SSA_Prepend_Phi_To_BB
 * ================================================================
 */
static void SSA_Prepend_Phi_To_BB (
  OP *phi_op, 
  BB *bb
) 
{
  INT i = 0;
  BBLIST *preds;

  //
  // add 'phi_op' to 'bb' before all other insts
  //
  BB_Prepend_Op(bb, phi_op);

  //
  // Some additional bookkeeping that is not done by Mk_OP
  //
  Initialize_PHI_map(phi_op);

  PHI_MAP_ENTRY *entry = (PHI_MAP_ENTRY *)OP_MAP_Get(phi_op_map, phi_op);
  Is_True(entry != NULL,("unmapped op ?"));

  FOR_ALL_BB_PREDS(bb,preds) {
    BB *pred = BBLIST_item(preds);
    entry->opnd_src[i] = pred;
    i++;
  }
}

/* ================================================================
 * ================================================================
 * 		Interface for SSA pinning
 * ================================================================
 * ================================================================
 */

static void
OP_Set_ssa_pinning(OP *op) {

  // Allocate an array the size of the results+opnds. For each uses
  // and definitions of the operation, set either NULL is there is no
  // pinning, or a dedicated TN on which the operand must be pinned.

  if ((OP_results(op) + OP_opnds(op)) == 0)
    return;

  TN **pinning = NULL;

  INT idx = 0;
  for (INT opnd_idx = 0; opnd_idx < OP_opnds(op); opnd_idx++, idx++) {
    TN *tn = OP_opnd(op, opnd_idx);
    if (TN_is_SSA_candidate(tn) && TN_need_operand_pinning(op, tn)) {
      if (pinning == NULL)
	pinning = TYPE_MEM_POOL_ALLOC_N(TN *, &ssa_pool, OP_results(op) + OP_opnds(op));
      pinning[idx] = tn;
    }
  }

  for (INT res_idx = 0; res_idx < OP_results(op); res_idx++, idx++) {
    TN *tn = OP_result(op, res_idx);
    if (TN_is_SSA_candidate(tn) && TN_need_operand_pinning(op, tn)) {
      if (pinning == NULL)
	pinning = TYPE_MEM_POOL_ALLOC_N(TN *, &ssa_pool, OP_results(op) + OP_opnds(op));
      pinning[idx] = tn;
    }
  }

  if (pinning != NULL)
    OP_MAP_Set(op_ssa_pinning_map, op, pinning);
}

BOOL
OP_Has_ssa_pinning(const OP *op) {
  return (OP_MAP_Get(op_ssa_pinning_map, op) != NULL);
}

static TN **
OP_Get_ssa_pinning(const OP *op) {
  return (TN **)OP_MAP_Get(op_ssa_pinning_map, op);
}

TN *
OP_Get_opnd_pinning(const OP *op, INT opnd_idx) {
  TN **pinning = OP_Get_ssa_pinning(op);
  TN *pinned = NULL;
  if (pinning != NULL)
    pinned = pinning[opnd_idx];
  return pinned;
}

TN *
OP_Get_result_pinning(const OP *op, INT res_idx) {
  TN **pinning = OP_Get_ssa_pinning(op);
  TN *pinned = NULL;
  if (pinning != NULL)
    pinned = pinning[OP_opnds(op)+res_idx];
  return pinned;
}

/* ================================================================
 * ================================================================
 * 		Interface for PSI operations
 * ================================================================
 * ================================================================
 */

/* ================================================================
 *   Set_PSI_Pred
 * ================================================================
 */
void Set_PSI_Pred (
 OP *psi,
 UINT8 pos,
 BOOL pred_false
)
{
  // We use (pos<<1), so that the trace function can correctly attach
  // the Pred_False property to the guard of a PSI operand, without
  // knowing this is a PSI operation.
  if (pred_false)
    Set_OP_Pred_False(psi, pos<<1);
  else
    Set_OP_Pred_True(psi, pos<<1);
}

/* ================================================================
 *   PSI_guard
 * ================================================================
 */
TN*
PSI_guard (
  const OP *psi,
  UINT8 pos
)
{
  Is_True(OP_psi(psi),("not a PSI function"));
  return OP_opnd(psi, pos<<1);
}

/* ================================================================
 *   Set_PSI_guard
 * ================================================================
 */
void Set_PSI_guard (
 OP *psi,
 UINT8 pos,
 TN *guard
)
{
  Set_OP_opnd(psi, pos<<1, guard);
}

/* ================================================================
 *   Set_PSI_opnd
 * ================================================================
 */
void Set_PSI_opnd (
 OP *psi,
 UINT8 pos,
 TN *opnd
)
{
  Set_OP_opnd(psi, (pos<<1)+1, opnd);
}

/* ================================================================
 *   Sort_PHI_opnds
 *
 *   Sort operands of a PHI operations such that no definition of an
 *   element dominates the definition of an element on its left. We
 *   must be in PHI-conform SSA to do this, so this is equivalent to
 *   say that the associated predecessor of an operand must not
 *   dominate the associated predecessor of an operand on its left.
 *
 * ================================================================
 */
void
Sort_PHI_opnds (
 OP *phi
)
{
  // Really needed, or follow an order given in input (breadth-first
  // for example) ?

  // Use a simple insertion sort algorithm to sort the operands and
  // associated predecessors mapping.

  PHI_MAP_ENTRY *entry = (PHI_MAP_ENTRY *)OP_MAP_Get(phi_op_map, phi);
  Is_True(entry != NULL,("unmapped op ?"));

  for (int i = 1; i < OP_opnds(phi); i++) {
    TN *TN_i = OP_opnd(phi, i);
    BB *BB_i = entry->opnd_src[i];

    int j;
    // We need this loop because the dominance relation is a partial order.
    for (j = 0; !BB_SET_MemberP(BB_dom_set(entry->opnd_src[j]), BB_i); j++);
    if (i != j) {
      //      fprintf(stderr, "Reordering PHI operands (%d/%d)\n", j, OP_opnds(phi));
      for (int k = i; k > j; k--) {
	Set_OP_opnd(phi, k, OP_opnd(phi, k-1));
	entry->opnd_src[k] = entry->opnd_src[k-1];
      }
      Set_OP_opnd(phi, j, TN_i);
      entry->opnd_src[j] = BB_i;
    }
  }
}

static OP *
PSI_Create(TN *res, TN *guard1, TN *tn1, TN *guard2, TN *tn2) {
  TN *opnd[4];
  opnd[0] = guard1;
  opnd[1] = tn1;
  opnd[2] = guard2;
  opnd[3] = tn2;
  return Mk_VarOP(TOP_psi, 1, 4, &res, opnd);
}

/* ================================================================
 *   PSI_inline
 * ================================================================
 */
OP *
PSI_inline (
 OP *psi_op,
 std::list<OP*> *inlined_psis
)
{
  int extra_opnds = 0;
  int opndx;

  // Look if some arguments are defined by PSI operations
  for (opndx = 0; opndx < PSI_opnds(psi_op); opndx++) {
    TN *opnd_tn = PSI_opnd(psi_op, opndx);
    OP *def_op = TN_ssa_def(opnd_tn);
    if (def_op && OP_psi(def_op))
      // Count the additional arguments after inlining
      extra_opnds += PSI_opnds(def_op) - 1;
  }

  // Inline all PSI operations that are arguments of this PSI
  // operation
  if (extra_opnds) {
    INT num_results = 1;
    INT num_opnds = PSI_opnds(psi_op) + extra_opnds;
    int inlined_opndx;

    TN *result[1];

    TN **opnd = (TN**)alloca(sizeof(TN*)*num_opnds*2);

    result[0] = OP_result(psi_op, 0);

    // Inline arguments of PSI operations
    for (opndx = 0, inlined_opndx = 0; opndx < PSI_opnds(psi_op); opndx++) {
      TN *opnd_tn = PSI_opnd(psi_op, opndx);
      OP *def_op = TN_ssa_def(opnd_tn);
      if (def_op && OP_psi(def_op)) {
	for (int psi_opndx = 0; psi_opndx < PSI_opnds(def_op); psi_opndx++) {
	  opnd[inlined_opndx*2+1] = PSI_opnd(def_op, psi_opndx);
	  opnd[inlined_opndx*2] = PSI_guard(def_op, psi_opndx);
	  inlined_opndx ++;
	}
	if (inlined_psis)
	  inlined_psis->push_front(def_op);
      }
      else {
	opnd[inlined_opndx*2+1] = opnd_tn;
	opnd[inlined_opndx*2] = PSI_guard(psi_op, opndx);
	inlined_opndx ++;
      }
    }

    Is_True(inlined_opndx == num_opnds, ("PSI_inline: internal error"));
    
    psi_op = Mk_VarOP (TOP_psi,
		       num_results,
		       num_opnds*2,
		       result,
		       opnd);
  }

  return psi_op;
}

/* ================================================================
 *   PSI_reduce
 * ================================================================
 */
OP *
PSI_reduce (
 OP *psi_op
)
{
  // Remove duplicated arguments
  // Discard all arguments on the left of an unconditional definition

  int removed_opnds = 0;
  int opndx;

  for (opndx = PSI_opnds(psi_op)-1; opndx >= 0; opndx--) {
    TN *opnd_tn = PSI_opnd(psi_op, opndx);

    // Discard all arguments on the left of an unconditional
    // definition
    OP *def_op = TN_ssa_def(opnd_tn);
    if (!def_op || PSI_guard(psi_op, opndx) == True_TN) {
      for (int i = 0; i < opndx; i++)
	Set_PSI_opnd(psi_op, i, NULL);
      removed_opnds += opndx;
      break;
    }

    // Otherwise, look for duplicate of an argument on the right
    for (int i = opndx+1; i < PSI_opnds(psi_op); i++)
      if ((PSI_opnd(psi_op, i) == opnd_tn) &&
	  (PSI_guard(psi_op, i) == PSI_guard(psi_op, opndx)) &&
	  (PSI_Pred_False(psi_op, i) == PSI_Pred_False(psi_op, opndx))) {
	removed_opnds ++;
	Set_PSI_opnd(psi_op, i, NULL);
	break;
      }
  }

  if (removed_opnds) {
    INT num_results = 1;
    INT num_opnds = PSI_opnds(psi_op) - removed_opnds;
    int new_opndx;

    TN *result[1];

    TN **opnd = (TN**)alloca(sizeof(TN*)*num_opnds*2);
    result[0] = OP_result(psi_op, 0);

    // Remove arguments of PSI operations
    for (opndx = 0, new_opndx = 0; opndx < PSI_opnds(psi_op); opndx++) {
      TN *opnd_tn = PSI_opnd(psi_op, opndx);
      if (opnd_tn == NULL) continue;
      opnd[new_opndx] = PSI_guard(psi_op, opndx);
      opnd[new_opndx+1] = opnd_tn;
      new_opndx+=2;
    }

    Is_True(new_opndx == (num_opnds<<1), ("PSI_reduce: internal error"));

    OP *new_psi_op;
    new_psi_op = Mk_VarOP (TOP_psi,
			   num_results,
			   num_opnds*2,
			   result,
			   opnd);

    // Also copy the Pred_False attribute
    for (opndx = 0, new_opndx = 0; opndx < PSI_opnds(psi_op); opndx++) {
      if (PSI_opnd(psi_op, opndx) != NULL) {
	Set_PSI_Pred(new_psi_op, new_opndx, PSI_Pred_False(psi_op, opndx));
	new_opndx ++;
      }
    }

    return new_psi_op;
  }

  return psi_op;
}

/* ================================================================
 *   Convert_PHI_to_PSI
 * ================================================================
 */
OP *
Convert_PHI_to_PSI (
 OP *phi
)
{

#ifdef Is_True_On
  /* A prerequisite is that Sort_PHI_opnds has been called on that phi
     operation. */
  for (int i = 1; i < OP_opnds(phi); i++) {
    BB *BB_i = Get_PHI_Predecessor(phi, i);
    for (int j = 0; j < i; j++) {
      BB *BB_j = Get_PHI_Predecessor(phi, j);
      Is_True(!BB_SET_MemberP(BB_dom_set(BB_j), BB_i), 
	      ("Convert_PHI_to_PSI: Incorrect PHI operands dominance relation for PSI conversion."));
    }
  }
#endif

  TN **opnd = (TN**)alloca(sizeof(TN*)*OP_opnds(phi)*2);

  for (int i = 0; i < OP_opnds(phi); i++) {
    opnd[2*i] = NULL;
    opnd[2*i+1] = OP_opnd(phi, i);
  }

  INT num_results = 1;
  TN *result[1];
  result[0] = OP_result(phi, 0);

  OP *psi_op = Mk_VarOP (TOP_psi,
			 num_results,
			 OP_opnds(phi)*2,
			 result,
			 opnd);

  // Arguments that are defined on a PSI are replaced by the arguments
  // of this PSI
  // FdF 20050518: Will be called explicitely if needed
  //  psi_op = PSI_inline(psi_op);

  // Remove duplicated arguments or unreachable definitions.
  // FdF 20050518: Will be called explicitely if needed
  //  psi_op = PSI_reduce(psi_op);

  return psi_op;
}

//
// dominance frontier blocks for each BB in the region
//
static BB_LIST **DF;
static BB_SET *DF_bb;

/* ================================================================
 *   compute_dominance_frontier
 *
 *   recursively traverse the dominator tree in bottom-up order.
 * ================================================================
 */
static void
compute_dominance_frontier (
  BB *bb,
  BOOL *visited
)
{

  // if we've visited this BB while processing another
  // region_entry, return with peace
  if (visited[BB_id(bb)]) return;

  // allocate the DF set
  DF[BB_id(bb)] = NULL;

  for (BB_LIST *elist = BB_children(bb); elist; elist = BB_LIST_rest(elist)) {
    BB *kid = BB_LIST_first(elist);
    compute_dominance_frontier (kid, visited);
  }

  // children are done, do this one
  BB *x = bb;
  //
  // local
  //
  BB_SET_ClearD(DF_bb);
  BBLIST *succs;
  FOR_ALL_BB_SUCCS(x, succs) {
    BB* y = BBLIST_item(succs);
    if (BB_dominator(y) != x) {
      //
      // bb does not dominate it's successor => successor
      // is in dominance frontier
      //
      DF_bb = BB_SET_Union1D(DF_bb, y, &ssa_pool);
    }
  }

  //
  // up
  //
  for (BB_LIST *elist = BB_children(bb); elist; elist = BB_LIST_rest(elist)) {
    BB *z = BB_LIST_first(elist);
    Is_True(z != x, ("Inconsistent dominator tree"));
    for (BB_LIST *dflist = DF[BB_id(z)]; dflist; dflist = BB_LIST_rest(dflist)) {
      BB *y = BB_LIST_first(dflist);
      if (BB_dominator(y) != x) {
	DF_bb = BB_SET_Union1D(DF_bb, y, &ssa_pool);
      }
    }
  }

  BB *y;
  FOR_ALL_BB_SET_members(DF_bb,y) {
    DF[BB_id(x)] = BB_LIST_Push(y, DF[BB_id(x)], &ssa_pool);
  }

  // mark as visited
  visited[BB_id(bb)] = TRUE;

  return;
}


/* ================================================================
 *   SSA_Compute_Dominance_Frontier
 *
 *   Algorithm page 466, Fig.10 of Cytron et al. paper.
 *
 *   NOTE: lst - is in the reverse order of the dominator tree.
 * ================================================================
 */
static void
SSA_Compute_Dominance_Frontier ()
{
  BB *bb;
  BOOL *visited;  //  whether DF has been computed for this BB

  if (Trace_dom_frontier) {
    fprintf(TFile, "<ssa> Build Dominance Frontier\n");
  }

  //
  // Initialize the dominance frontier structure
  //
  DF = (BB_LIST **)TYPE_MEM_POOL_ALLOC_N(BB_LIST *, 
					&ssa_pool, 
					PU_BB_Count+2 );

  visited = (BOOL *)alloca(sizeof(BOOL)*(PU_BB_Count+2));
  BZERO(visited, sizeof(BOOL)*(PU_BB_Count+2));

  DF_bb = BB_SET_Create_Empty(PU_BB_Count+2, &ssa_pool);

  FOR_ALL_BB_SET_members(region_entry_set,bb) {
    compute_dominance_frontier (bb, visited);
  }

  //
  // debugging
  //
  if (Trace_dom_frontier) {
    fprintf(TFile, "<ssa> DOMINANCE FRONTIERS: \n");
    for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
      fprintf(TFile, "  BB%d : ", BB_id(bb));
      for (BB_LIST *elist = DF[BB_id(bb)]; elist; elist = BB_LIST_rest(elist)) {
	BB *df = BB_LIST_first(elist);
	fprintf(TFile, "%d ", BB_id(df));
      }
      fprintf(TFile, "\n--------------------\n");
    }
  }

  return;
}

/* ================================================================
 *   SSA_Place_Phi_In_BB ()
 *
 *   Make a PHI-function of the type 
 *
 *               tn = PHI (tn, tn, ... , tn)
 *
 *   where the number of 'tn's equals the number of 'bb's predecessors in
 *   the CFG, and each 'tn' refers to the corresponding BB predecessor.
 *
 * ================================================================
 */
OP *
SSA_Place_Phi_In_BB (
  TN *tn, 
  BB *bb
)
{
  INT i;
  INT num_results = 1;
  INT num_opnds = BBlist_Len(BB_preds(bb));

  TN *result[1];
  TN *opnd[num_opnds];

  // only 1 result possible
  result[0] = tn;
  for (i = 0; i < num_opnds; i++) {
    opnd[i] = tn;
  }

  OP *phi_op = Mk_VarOP (TOP_phi, 
			 num_results,   /* single result TN */
			 num_opnds, 
			 result, 
			 opnd);

  if (Trace_SSA_Build) {
    fprintf(TFile, "    adding phi for ");
    Print_TN(tn, FALSE);
    fprintf(TFile, " in BB%d : ", BB_id(bb));
    Print_OP(phi_op);
  }

  SSA_Prepend_Phi_To_BB (phi_op, bb);

#if 0
  fprintf(TFile, "BB%d:\n", BB_id(OP_bb(phi_op)));
  for (i = 0; i < num_opnds; i++) {
    fprintf(TFile, "  pred[%d] = BB%d\n", i, BB_id(Get_PHI_Predecessor(phi_op,i)));
  }
#endif

  return phi_op;
}

// Renaming of dedicated registers requires some work to be done:
// - Uses and definitions on call operations are made explicit
// - A PCOPY operation is inserted at the entries and exits of the
//   function for dedicated TNs
// - For operations with dedicated register constraints, the
//   information on the pinning of uses and results is associaed to
//   the operation. This will be needed during the out-of-SSA
//   algorithm.

static void
SSA_PCOPY_Prologue(BB *bb_prolog) {

  TN_SET *dedicated_set = TN_SET_Create_Empty (Last_Dedicated_TN+1,&MEM_local_pool);
  OP *pcopy_entry = Mk_VarOP(TOP_PCOPY, 0, 0, NULL, NULL);
  OP *op;
  OP *pcopy_point = NULL;
  // Look for the first use of each dedicated register
  FOR_ALL_BB_OPs_FWD(bb_prolog, op) {
    // This function has one single basic block, prologue and epilogue
    // code should not mix.
    if (OP_epilogue(op) || OP_call(op))
      break;
    for (INT i = 0; i < OP_opnds(op); i++) {
      TN *tn = OP_opnd(op,i);
      if (TN_is_SSA_candidate(tn) && TN_is_dedicated(tn) && !TN_SET_MemberP(dedicated_set, tn)) {
	if (pcopy_point == NULL)
	  pcopy_point = op;
	pcopy_entry = PCOPY_add_copy(pcopy_entry, tn, tn);
	TN_SET_Union1D(dedicated_set, tn, NULL);
      }
    }
    // When a dedicated register is defined, mark is a seen so that
    // later uses are not considered live-in
    for (INT i = 0; i < OP_results(op); i++) {
      TN *tn = OP_result(op,i);
      if (TN_is_SSA_candidate(tn) && TN_is_dedicated(tn))
	TN_SET_Union1D(dedicated_set, tn, NULL);
    }
  }
  // There is a non-empty pcopy operation, insert it just before the
  // first use of a dedicated register
  if (pcopy_point != NULL) {
    Set_OP_ssa_move(pcopy_entry);
    Set_OP_prologue(pcopy_entry);
    BB_Insert_Op_Before(bb_prolog, pcopy_point, pcopy_entry);
  }
}

static void
SSA_PCOPY_Epilogue(BB *bb_epilog) {

  TN_SET *dedicated_set = TN_SET_Create_Empty (Last_Dedicated_TN+1,&MEM_local_pool);
  OP *pcopy_exit = Mk_VarOP(TOP_PCOPY, 0, 0, NULL, NULL);
  OP *op;
  OP *pcopy_point = NULL;
  // Look for the last definition of each dedicate register
  FOR_ALL_BB_OPs_REV(bb_epilog, op) {
    // This function has one single basic block, prologue and epilogue
    // code should not mix.
    if (OP_prologue(op))
      break;
    for (INT i = 0; i < OP_results(op); i++) {
      TN *tn = OP_result(op,i);
      if (TN_is_SSA_candidate(tn) && TN_is_dedicated(tn) && !TN_SET_MemberP(dedicated_set, tn)) {
	if (pcopy_point == NULL)
	  pcopy_point = op;
	pcopy_exit = PCOPY_add_copy(pcopy_exit, tn, tn);
	TN_SET_Union1D(dedicated_set, tn, NULL);
      }
    }
    // When a dedicated register is used, set the pcopy point
    for (INT i = 0; i < OP_opnds(op); i++) {
      TN *tn = OP_opnd(op,i);
      if ((pcopy_point == NULL) && TN_is_SSA_candidate(tn) && TN_is_dedicated(tn))
	pcopy_point = op;
    }
  }
  // There is a non-empty pcopy operation, insert it just after the
  // last definition of a dedicated register
  if (pcopy_point != NULL) {
    Set_OP_ssa_move(pcopy_exit);
    Set_OP_epilogue(pcopy_exit);
    BB_Insert_Op_After(bb_epilog, pcopy_point, pcopy_exit);
  }
}

/* ================================================================
 *  SSA_Explicit_Call
 * ================================================================
 */

static void
SSA_Explicit_Call(BB *bb_call) {

  OP *op_call = BB_xfer_op(bb_call);
  Is_True(OP_call(op_call), ("Unexpected op at the end of a call BB"));

  // The implicit arguments of the call have been registered in the
  // CALLINFO annotation.
  CALLINFO *callinfo = ANNOT_callinfo(ANNOT_Get(BB_annotations(bb_call), ANNOT_CALLINFO));
  INT call_results = CALLINFO_call_results(callinfo);
  INT call_opnds = CALLINFO_call_opnds(callinfo);

  // Now, count only the dedicated registers that will be renamed
  // during SSA.

  INT SSA_results = 0, SSA_opnds = 0;

  for (INT idx = 0; idx < call_opnds; idx ++) {
    TN *tn = CALLINFO_call_opnd(callinfo)[idx];
    if (TN_is_SSA_candidate(tn))
      SSA_opnds ++;
  }

  for (INT idx = 0; idx < call_results; idx ++) {
    TN *tn = CALLINFO_call_result(callinfo)[idx];
    if (TN_is_SSA_candidate(tn))
      SSA_results ++;
  }

  if ((SSA_results == 0) && (SSA_opnds == 0))
    return;

  // Rewrite the call with explicit operands
  INT new_results = OP_results(op_call) + SSA_results;
  INT new_opnds = OP_opnds(op_call) + SSA_opnds;
  OP *new_op_call = Resize_OP(op_call, new_results, new_opnds);

  INT opnd_idx = OP_opnds(op_call);
  for (INT idx = 0; idx < call_opnds; idx ++) {
    TN *tn = CALLINFO_call_opnd(callinfo)[idx];
    if (TN_is_SSA_candidate(tn)) {
      Set_OP_opnd(new_op_call, opnd_idx, tn);
      opnd_idx ++;
    }
  }

  INT result_idx = OP_results(op_call);
  for (INT idx = 0; idx < call_results; idx ++) {
    TN *tn = CALLINFO_call_result(callinfo)[idx];
    if (TN_is_SSA_candidate(tn)) {
      Set_OP_result(new_op_call, result_idx, tn);
      result_idx ++;
    }
  }

  BB_Replace_Op(op_call, new_op_call);
}

static void
SSA_init_dedicated_renaming()
{
  BB *bb;

  dedicated_candidate_set = NULL;

  if (!(PU_ssa_variables & SSA_VAR_DEDICATED))
    return;

  // Check if dedicated registers can be renamed, currently this is
  // not possible if asm statements are used or there exist et least
  // one Entry Handler with incoming control-flow edge.
  if (PU_Has_Asm) {
    PU_ssa_variables &= ~SSA_VAR_DEDICATED;
    return;
  }

  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    if ((BB_entry(bb) || BB_handler(bb)) && (BB_preds_len(bb) > 0)) {
      //      FmtAssert(0, ("Handler with incoming flow edge"));
      PU_ssa_variables &= ~SSA_VAR_DEDICATED;
      return;
    }
  }

  SSA_init_dedicated_TN_candidates();

  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {

    // Create a parallel copy operation that defines all the dedicated
    // registers in the prolog
    if (BB_entry(bb))
      SSA_PCOPY_Prologue(bb);

    // Create a parallel copy operation that uses all the dedicated
    // registers in the epilog
    if (BB_exit(bb))
      SSA_PCOPY_Epilogue(bb);

    // Insert explicit arguments and results on CALL instructions for
    // uses and definitions of dedicated registers. Tail calls are not
    // really calls.
    if (BB_call(bb) && !BB_exit(bb))
      SSA_Explicit_Call(bb);
#if 0
    if (BB_asm(bb)) {
      ASM_OP_ANNOT* asm_info = (OP_code(BB_last_op(bb)) == TOP_asm) ?
	(ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, BB_last_op(bb)) : NULL;
      ANNOTATION *ant = ANNOT_Get (BB_annotations(bb), ANNOT_ASMINFO);
      ASMINFO *info = ANNOT_asminfo(ant);
    }
#endif
    // Finally, traverse all operations and add pinning where required.
    OP *op;
    FOR_ALL_BB_OPs_FWD(bb, op) {

      if (OP_call(op))
	OP_Set_ssa_pinning(op);

      // TBD: Also handle ASM OPs

      else if (!OP_PCOPY(op) || (!OP_prologue(op) && !OP_epilogue(op)))
	OP_Set_ssa_pinning(op);
    }
  }
}

/* ================================================================
 *  initialize_tn_def_map
 * ================================================================
 */
static TN_LIST *
initialize_tn_def_map (BOOL incremental)
{
  INT i;
  BB *bb;
  OP *op;

  TN_LIST *global_tns = NULL;
  TN_LIST *cur_list = NULL;

  GTN_SET *gtn_set = GTN_SET_Create_Empty (Last_TN + 1,&MEM_local_pool);
  tn_def_map = TN_MAP_Create();

  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    FOR_ALL_BB_OPs_FWD(bb, op) {

      //fprintf(TFile,"BB%d : ", BB_id(bb));
      //Print_OP(op);

      for (i = 0; i < OP_results(op); i++) {
	TN *tn = OP_result(op,i);
	// Already an SSA variable (for incremental renaming), or
	// cannot be an SSA variable
	if (!(TN_is_global_reg(tn) && TN_is_SSA_candidate(tn)))
	  continue;
	if (incremental && TN_is_ssa_var(tn))
	  continue;
	if (!GTN_SET_MemberP(gtn_set, tn)) {
	  gtn_set = GTN_SET_Union1D(gtn_set, tn, &MEM_local_pool);
	  // FdF 20080502: Use a list and insert at the end of the
	  // list, so as to link the TNs in the order of the occurence
	  // of their first definition in the sequential order of the
	  // function. This keeps the order of the insertion of the
	  // PHIs instructions unchanged from previous versions, and
	  // avoids some performance/size regressions observed when
	  // this order is changed.
	  TN_LIST *new_list = TN_LIST_Push(tn, NULL, &MEM_local_pool);
	  if (global_tns == NULL)
	    cur_list = global_tns = new_list;
	  else {
	    TN_LIST_rest(cur_list) = new_list;
	    cur_list = new_list;
	  }
	}
	BB_LIST *p = TN_is_def_in(tn);
	TN_MAP_Set(tn_def_map, tn, BB_LIST_Push(bb, p, &MEM_local_pool));
      }
    }
  }

  return global_tns;
}

/* ================================================================
 *  finalize_tn_def_map
 * ================================================================
 */
static void
finalize_tn_def_map ()
{
  TN_MAP_Delete(tn_def_map);
  return;
}

/* ================================================================
 *   SSA_Place_Phi_Functions ()
 *
 *   Cytron et al. Fig.11, p. 470.
 *
 *   Modifications fro the article:
 *
 *      1. a phi-function is added to the BB only if the TN is live-in
 *         for that BB.
 * ================================================================
 */
static BB_SET *Phi_Functions_work;
static BB_SET *Phi_Functions_has_already;

static BOOL
Lookup_Phi_Function_For_TN(BB *bb, TN *tn) {
  OP *phi;
  TN *opnd;

  FOR_ALL_BB_PHI_OPs(bb, phi) {
    for (INT i = 0; i < OP_opnds(phi); i++) {
      opnd = OP_opnd(phi,i);
      if (opnd == tn)
	return TRUE;
    }
  }

  return FALSE;
}

static void
SSA_Place_Phi_Functions (
  TN *tn,
  BOOL incremental
)
{
  BB *bb;
  std::list<BB*> work_lst;
  std::list<BB*>::iterator bbi;

  // push the pool so that work and has already is not
  // accumulated in memory for all TNs
  MEM_POOL_Push(&MEM_local_pool);

  Phi_Functions_work = BB_SET_ClearD(Phi_Functions_work);
  BB_LIST *p;
  for (p = TN_is_def_in(tn); p != NULL; p = BB_LIST_rest(p)) {
    Phi_Functions_work = BB_SET_Union1D(Phi_Functions_work, BB_LIST_first(p), &MEM_local_pool);
  }

  // FdF 20080331: Even with BB_SET_Size(Phi_Functions_work) == 1,
  // there waybe a PHI instruction, with an uninitialized operand.

  //BB_SET *work = BB_SET_Copy(TN_is_def_in(tn), &MEM_local_pool);
  FOR_ALL_BB_SET_members(Phi_Functions_work, bb) {
    work_lst.push_back(bb);
  }

  if (Trace_SSA_Build) {
    fprintf(TFile, "\n  --> ");
    Print_TN(tn, FALSE);
    fprintf(TFile, " ");
    BB_SET_Print(Phi_Functions_work, TFile);
    fprintf(TFile, "\n");
  }

#if 0
  fprintf(TFile, "  work_lst: ");
  for (bbi = work_lst.begin(); bbi != work_lst.end(); bbi++) {
    fprintf(TFile, "    BB%d ", BB_id(*bbi));
  }
  fprintf(TFile, "\n");
#endif

  Phi_Functions_has_already = BB_SET_ClearD(Phi_Functions_has_already);

  for (bbi = work_lst.begin(); bbi != work_lst.end(); bbi++) {
    BB *sc;
    for (BB_LIST *dflist = DF[BB_id(*bbi)]; dflist; dflist = BB_LIST_rest(dflist)) {
      sc = BB_LIST_first(dflist);
      //
      // if a phi-function for 'tn' has not yet been added to 'sc'
      // and 'tn' is actually live in 'sc', add the phi-function
      //

      if (Trace_SSA_Build) {
	fprintf(TFile, "    BB%d: df node BB%d: ", BB_id(*bbi), BB_id(sc));
	fprintf(TFile, " %s, %s\n", BB_SET_MemberP(Phi_Functions_has_already, sc) ? "has already" : "no phi yet", GTN_SET_MemberP(BB_live_in(sc), tn) ? "live in" : "not live in");
      }

      if (incremental && !BB_SET_MemberP(Phi_Functions_has_already, sc) &&
	  Lookup_Phi_Function_For_TN(sc, tn))
        Phi_Functions_has_already = BB_SET_Union1D(Phi_Functions_has_already, sc, &MEM_local_pool);

      else if (!BB_SET_MemberP(Phi_Functions_has_already, sc) &&
	       (incremental || GTN_SET_MemberP(BB_live_in(sc), tn))) {

	//fprintf(TFile, "    placing a PHI in BB%d\n", BB_id(sc));

        SSA_Place_Phi_In_BB (tn, sc);
        Phi_Functions_has_already = BB_SET_Union1D(Phi_Functions_has_already, sc, &MEM_local_pool);
        if (!BB_SET_MemberP(Phi_Functions_work, sc)) {
          Phi_Functions_work = BB_SET_Union1D(Phi_Functions_work, sc, &MEM_local_pool);
	  work_lst.push_back(sc);
        }
      }
    }
  }

  MEM_POOL_Pop(&MEM_local_pool);

  return;
}

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
  return Dup_TN_Even_If_Dedicated(tn);
}

/* ================================================================
 *   Rename_Phi_Operands
 * ================================================================
 */
static void
Rename_Phi_Operands (
  BB *bb,
  BB *sc
)
{
  INT i;
  OP *phi;

  FOR_ALL_BB_PHI_OPs(sc,phi) {
    for (i = 0; i < OP_opnds(phi); i++) {
      TN *tn = OP_opnd(phi,i);
      if (Get_PHI_Predecessor(phi,i) == bb) {
	if (TN_STACK_empty(tn))
	  Insert_Kill_op(bb, BB_branch_op(bb), tn);
	TN *new_tn = tn_stack_top(tn);
	Set_OP_opnd(phi,i,new_tn);
	break;
      }
    }
  }

  return;
}

/* ================================================================
 *   SSA_Rename_BB
 *
 *   Recursively visit BBs in the order of dominator tree. Every new 
 *   definition is renamed and the new name goes on top of the 
 *   tn_stack.
 *   Every use is renamed from the top of the tn_stack.
 *
 *   When we're done with the dominator tree subtree, the tn_stack 
 *   is poped.
 *
 *   At every BB, visit its CFG successors and rename operands of the
 *   PHI-functions.
 * ================================================================
 */
static void
SSA_Rename_BB (
  BB   *bb,
  BOOL *visited,   // has been renamed processing another entry BB in
                   // this case only need to rename phi-operands of
                   // its CFG successors
  BOOL incremental
)
{
  INT  i;
  OP  *op;
  TN  *new_tn;

  if (visited[BB_id(bb)]) return;

#if 0
  if (Trace_SSA_Build) {
    fprintf(TFile, "  BB%d: \n", BB_id(bb));
  }
#endif

  TN_SET *tn_stacked_set = TN_SET_Create_Empty (TN_STACK_size, &MEM_local_pool);
  // All push function for this BB will use and update this set
  TN_STACK_cur_stacked_set = tn_stacked_set;

  // We also need to rename LOOPINFO_trip_count_tn
  {
    ANNOTATION *annot = ANNOT_Get(BB_annotations(bb), ANNOT_LOOPINFO);
    if (annot) {
      LOOPINFO *info = ANNOT_loopinfo(annot);
      TN *trip_count_tn = LOOPINFO_primary_trip_count_tn(info);
      if ((trip_count_tn != NULL) && TN_is_register(trip_count_tn)) {
	LOOPINFO_primary_trip_count_tn(info) = tn_stack_top(trip_count_tn);
      }
    }
  }

  OP *op_next;
  for (OP *op = BB_first_op(bb); op != NULL; op = op_next) {
    op_next = OP_next(op);

    // Perform copy propagation if required
    if (OP_Is_Copy(op) && (CG_ssa_coalescing & SSA_IN_DO_COPY_PROP) &&
	!OP_Is_Affirm(op) && !OP_Has_ssa_pinning(op)) {
      TN *src_copy = OP_Copy_Operand_TN(op);
      TN *dst_copy = OP_Copy_Result_TN(op);
      if (TN_is_SSA_candidate(src_copy) &&
	  TN_is_SSA_candidate(dst_copy)) {
	BOOL do_copy = (TN_register_class(src_copy) == TN_register_class(dst_copy));
#if Is_True_On
	if (getenv("SSA_COPY") != NULL) {
	  static int count = atoi(getenv("SSA_COPY"));
	  do_copy = (count > 0);
	  if (count > 0)
	    count--;
	}
#endif
	// Do not perform copy propagation when this would transfer a
	// TN_home to another TN.
	if (do_copy &&
	    TN_is_gra_homeable(src_copy) &&
	    (!TN_is_gra_homeable(dst_copy) ||
	     (TN_home(src_copy) != TN_home(dst_copy))))
	  do_copy = FALSE;
	if (do_copy) {
	  if (src_copy != dst_copy) {
	    if (TN_STACK_empty(src_copy))
	      Insert_Kill_op(bb, op, src_copy);
	    tn_stack_push(dst_copy, tn_stack_top(src_copy));
	  }
	  // Copy_TN (src_copy); // Just for debug
	  op_next = OP_next(op);
	  BB_Remove_Op(bb, op);
	  continue;
	}
      }
    }

    //
    // if this happens to be a phi-function, operands are renamed
    // when processing corresponding CFG predecessors
    // Psi arguments are renamed at their creation
    // Also, do not rename the operands of the PCOPY operation in the
    // prologue
    //
    if (!OP_phi(op) && !OP_psi(op) && !(OP_PCOPY(op) && OP_prologue(op))) {
      //
      // rename operands
      //
      for (i = 0; i < OP_opnds(op); i++) {
	TN *tn = OP_opnd(op,i);
	//
	// don't rename not allocatable TNs
	//
	if (!TN_is_register(tn) || !TN_is_SSA_candidate(tn)) continue;

	if (TN_STACK_empty(tn))
	  Insert_Kill_op(bb, op, tn);
	new_tn = tn_stack_top(tn);
	Set_OP_opnd(op, i, new_tn);
      }  /* while opnds */
    }

    //
    // push results on stack
    //
    // Do not rename the results of the PCOPY operation in the
    // epilogue
    if (!OP_PCOPY(op) || !OP_epilogue(op)) {
      for (i = 0; i < OP_results(op); i++) {
	TN *tn = OP_result(op,i);
	//
	// result TN must be a register
	if (!TN_is_SSA_candidate(tn))
	  continue;

	if (incremental && TN_is_ssa_var(tn)) {
	  tn_stack_push(tn, tn);
	}

	else {
	  new_tn = Copy_TN (tn);
	  if (OP_cond_def(op)) {
	    if (TN_STACK_empty(tn))
	      Insert_Kill_op(bb, op, tn);
	    /* Create a PSI to merge the previous value with the new one. */
	    OP *psi_op = PSI_Create(tn, True_TN, tn_stack_top(tn), OP_guard(op), new_tn);
	    BB_Insert_Op_After(bb, op, psi_op);
	    // tn_ssa_map is set, but psi_op has not been renamed yet,
	    // psi_op will be renamed in the next iteration
	    SSA_unset(psi_op);
	    op_next = psi_op;
	  }
	  tn_stack_push(tn, new_tn);
	  Set_OP_result(op, i, new_tn);
	  Set_TN_ssa_def(new_tn, op);  // this should also include PHIs
	}

#if 0
	fprintf(TFile, "  setting TN_ssa_def for ");
	Print_TN(new_tn, FALSE);
	fprintf(TFile, " to ");
	Print_OP(op);
#endif
      }  /* while rslts */
    }
  } /* for all BB ops */

  //
  // rename phi-functions in this 'bb's CFG successors
  //
  BBLIST *succs;
  FOR_ALL_BB_SUCCS(bb,succs) {
    BB *succ = BBLIST_item(succs);
    Rename_Phi_Operands (bb, succ);
  }

  //
  // rename the dominated subtree of this BB
  //
  for (BB_LIST *elist = BB_children(bb); elist; elist = BB_LIST_rest(elist)) {
    BB *kid = BB_LIST_first(elist);
    SSA_Rename_BB (kid, visited, incremental);
  }

  //
  // pop the tn_stack for those TNs that have been pushed
  //
  for (TN *tn = TN_SET_Choose(tn_stacked_set);
       tn != TN_SET_CHOOSE_FAILURE;
       tn = TN_SET_Choose_Next(tn_stacked_set, tn)) {
    tn_stack_pop(tn);
  }

  // mark as visited
  visited[BB_id(bb)] = TRUE;

  return;
}

/* ================================================================
 *   SSA_Rename
 * ================================================================
 */
static void
SSA_Rename (BOOL incremental)
{
  BB  *bb;

  //
  // First, calculate the dominance frontiers
  //
  SSA_Compute_Dominance_Frontier ();

  // Make definition/uses of dedicated registers explicit on ENTRY/EXIT/CALL/ASM operations.

  if (!incremental) {
    op_ssa_pinning_map = OP_MAP_Create();

    PU_ssa_variables = CG_ssa_variables;
#if Is_True_On
    if ((PU_ssa_variables & SSA_VAR_DEDICATED) && (getenv("SSA_DEDIC") != NULL)) {
      static int count = atoi(getenv("SSA_DEDIC"));
      static int SSA_on_dedicated;
      if (count == 0)
	PU_ssa_variables &= ~SSA_VAR_DEDICATED;
      else
	count --;
    }
#endif
    // TBD: Add support for new call instructions in incremental mode
    SSA_init_dedicated_renaming();

    // Build live-analysis
    GRA_LIVE_Init (NULL);
  }

  TN_LIST *global_tns;
  global_tns = initialize_tn_def_map(incremental);

  if (Trace_SSA_Build) {
    fprintf(TFile, "<ssa> Placing the PHI-nodes: \n");
  }

  Phi_Functions_work = BB_SET_Create_Empty(PU_BB_Count+2, &MEM_local_pool);
  Phi_Functions_has_already = BB_SET_Create_Empty(PU_BB_Count+2, &MEM_local_pool);

  if (incremental)
    SSA_Disable();

  TN_LIST *p;
  for (p = global_tns; p != NULL; p = TN_LIST_rest(p)) {
    TN *tn = TN_LIST_first(p);
    SSA_Place_Phi_Functions (tn, incremental);
  }

  if (incremental)
    SSA_Enable();

  if (Trace_SSA_Build) {
    Trace_IR(TP_SSA, "AFTER PHI_NODES INSERTION", NULL);
  }

  finalize_tn_def_map();
  initialize_tn_stack();

  BOOL *visited = (BOOL *)alloca(sizeof(BOOL)*(PU_BB_Count+2));
  BZERO(visited, sizeof(BOOL)*(PU_BB_Count+2));

  //
  // initialize tn_ssa_map, deleted by the SSA_Remove_Pseudo_OPs()
  //
  if (!incremental)
    tn_ssa_map = TN_MAP_Create();

  //
  // visit nodes in the dominator tree order renaming TNs
  //
  FOR_ALL_BB_SET_members(region_entry_set,bb) {
    SSA_Rename_BB (bb, visited, incremental);
  }

#if 0
  fprintf(TFile, "  TN SSA MAPPING: \n\n");
  for (INT i = First_Regular_TN; i < Last_TN; i++) {
    Print_TN(TNvec(i), FALSE);
    fprintf(TFile, "  --> ");
    if (((OP *)TN_MAP_Get(tn_ssa_map, TNvec(i)))) 
      Print_OP_No_SrcLine((OP *)TN_MAP_Get(tn_ssa_map, TNvec(i)));
    else 
      fprintf(TFile, "\n");
  }
#endif
  return;
}

/* ================================================================
 *   ssa_init
 * ================================================================
 */
static void
ssa_init () {
  // Initialize ssa_pool once per CG compilation
  static BOOL initialized;

  if (!initialized) {
    MEM_POOL_Initialize (&ssa_pool, "ssa pool", TRUE);
    initialized = TRUE;
  }

  return;
}

/* ================================================================
 *   SSA_Enter
 *
 *   Can be called two ways:
 *   1) on a region (pu_dst is NULL, returns code)
 *   2) on a PU (pu_dst is no NULL, returns NULL)
 *
 *   Must be called before any other SSA routine. It will prepare the
 *   data structures and build the SSA form.
 * ================================================================
 */
void
SSA_Enter (
  RID *rid, 
  BOOL region 
)
{
  Trace_SSA_Build = Get_Trace(TP_SSA, SSA_BUILD);
  Trace_dom_frontier = Get_Trace(TP_SSA, SSA_DOM_FRONT);

  if (Trace_SSA_Build) {
    fprintf(TFile, "%s CFG Before SSA_Enter\n%s\n", DBar, DBar);
    Print_All_BBs ();
  }

  ssa_init();

  //
  // Corresponding Pop is done by SSA_Remove_Pseudo_OPs()
  //
  MEM_POOL_Push (&ssa_pool);

  //
  // initialize phi_op_map, deleted by the SSA_Remove_Pseudo_OPs()
  //
  phi_op_map = OP_MAP_Create();

  MEM_POOL_Push (&MEM_local_pool);

  SSA_Dominance_init(rid);

  SSA_Rename(FALSE);

  // Finalize dominator tree for SSA construction.
  // Note that this does not destroy Dominator information itself.
  SSA_Dominance_fini();

#if 0
  if (Get_Trace(TP_TEMP, 0x8)) {
    BB *bb;
    OP *phi;
    for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
      FOR_ALL_BB_PHI_OPs(bb,phi) {
	Sort_PHI_opnds(phi); // PSI experimentation
      }
    }
  }
#endif
  Trace_IR(TP_SSA, "After the SSA Conversion", NULL);

  MEM_POOL_Pop (&MEM_local_pool);

  // NOTE: On exit from this routine, the dominator information is valid.
  // It will have to be released when not used anymore by the caller of this function.
  
  return;
}


/* ================================================================
 *   SSA_Update
 *
 *   SSA_Update can be called at any point during SSA
 *   transformations/optimizations. It will introduce single
 *   defintions and PHI operations for any variable in the procedure
 *   that is candidate for SSA renaming and do not have an SSA def.
 *
 * ================================================================
 */
void
SSA_Update ()
{

  MEM_POOL_Push (&MEM_local_pool);

  //initialize_tn_stack();
  DOM_TREE_Initialize();

  SSA_Rename(TRUE);

  MEM_POOL_Pop (&MEM_local_pool);

  return;
}

/* ================================================================
 *   SSA_Check
 *
 *   Checks if the current function can be put in SSA form. Currently,
 *   the only limitation is that no basic block have more that 255
 *   predecessors. The reason is that the number of opnds in an OP is
 *   encoded on an uINT8, and a PHI operation has as many operands as
 *   the number of predecessors of the basic block it is in.
 *   FdF 20090507: Get rid of this limit in op.h
 * ================================================================ */
BOOL
SSA_Check (
  RID *rid, 
  BOOL region 
)
{
#if Is_True_On
  if (getenv("SSA") != NULL) {
    static int count = atoi(getenv("SSA"));
    return (count > 0) ? (count--,1) : 0;
  }
#endif
#if 0
  BB *bb;
  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    if (BBlist_Len(BB_preds(bb)) > 255)
      return FALSE;
  }
#endif
  return TRUE;
}

/* ================================================================
 *   SSA_Verify
 *
 *   Verify the invariant properties of the SSA representation
 *
 *   Should be run in debug mode after each transformation that is
 *   supposed to maintain SSA form. Only done in debug mode.
 *
 *   Current implementation verifies:
 *     1. (opnd|result)->tn->ssa_def link.
 *     2. ssa_def->bb link
 *     3. ssa_def defines tn and only once.
 *     4. op->results->tn->ssa_def returns the op itself (unicity)
 *     5. if dominators are availables check that def dominates use
 * ================================================================ */

static BOOL
SSA_Verify_TN(BB *bb, OP *op, TN *tn)
{
  OP *def_op;
  BB *def_bb;

  BOOL ok = TRUE;
  
  def_op = TN_ssa_def(tn);

  if (def_op == NULL) {
    DevWarn("Missing SSA def for TN. TN opnd: PU:%s BB:%d OP:%d TN:%d", 
	    Cur_PU_Name, BB_id(bb), OP_map_idx(op), TN_number(tn));
    return FALSE;
  }
  
  def_bb = OP_bb(def_op);
  if (def_bb == NULL) {
    DevWarn("Missing SSA def BB for OP:%d. TN opnd: PU:%s BB:%d OP:%d TN:%d", 
	    OP_map_idx(def_op), Cur_PU_Name, BB_id(bb), OP_map_idx(op), TN_number(tn));
    return FALSE;
  }
  
  int i, found = FALSE;
  for (i = 0; i < OP_results(def_op); i++) {
    if (OP_result(def_op, i) == tn) {
      if (found) {
	DevWarn("Multiple def for TN in OP:%d. TN opnd: PU:%s BB:%d OP:%d TN:%d", 
		OP_map_idx(def_op), Cur_PU_Name, BB_id(bb), OP_map_idx(op), TN_number(tn));
	ok = FALSE;
      }
      found = TRUE;
    }
  }
  if (!found) {
    DevWarn("TN not defined by OP:%d. TN opnd: PU:%s BB:%d OP:%d TN:%d", 
	    OP_map_idx(def_op), Cur_PU_Name, BB_id(bb), OP_map_idx(op), TN_number(tn));
    ok = FALSE;
  }
  return ok;
}

BOOL
SSA_Verify (
  RID *rid, 
  BOOL region 
)
{
  BB *bb;
  OP *op;
  BOOL ok = TRUE;

#ifdef Is_True_On  

  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {

    FOR_ALL_BB_OPs(bb, op) {

      if (OP_phi(op)) {
	/* Get phi map entry. */
	if (OP_opnds(op) != BB_preds_len(bb)) {
	  DevWarn("Invalid phi for bb BB%d\n\n", BB_id(bb));
	  ok = FALSE;
	}
	else {
	  PHI_MAP_ENTRY *entry = (PHI_MAP_ENTRY *)OP_MAP_Get(phi_op_map, op);
	  if (entry == NULL) {
	    DevWarn("PHI not mapped. PU:%s BB:%d OP:%d", 
		    Cur_PU_Name, BB_id(bb), OP_map_idx(op));
	    ok = FALSE;
	  }

	  else {

	    /* Check that each basic block predecessor is correctly
	       associated with a PHI operand. */

	    BBLIST *edge;
	    int opnd_idx;
	    FOR_ALL_BB_PREDS(bb, edge) {
	      BB *bp = BBLIST_item(edge);
	      opnd_idx = Get_PHI_Predecessor_Idx(op, bp);
	      if (Get_PHI_Predecessor(op, opnd_idx) != bp) {
		DevWarn("SSA_Verify: BB %d opnd %d in PHI, inconsistency in PHI map.", BB_id(bb), opnd_idx);
		ok = FALSE;
	      }
	      else {
		/* Domination check: only done if dominators are available. */
		if (PU_Has_Dominator_Info()) {
		  /* Check that the definition of a PHI operand dominates its
		     associated predecessor. */
		  TN *opnd_tn = OP_opnd(op, opnd_idx);
		  OP *def_op = TN_ssa_def(opnd_tn);
		  if (def_op && !BB_SET_MemberP(BB_dom_set(bp), OP_bb(def_op))) {
		    DevWarn("SSA_Verify: BB:%d opnd %d in PHI, definition does not dominate predecessor block.", BB_id(bb), opnd_idx);
		    ok = FALSE;
		  }
		}
	      }
	    }

	    /* Check that each PHI operand is correctly associated with a
	       basic block predecessor. */

	    for (int opnd_idx = 0; opnd_idx < OP_opnds(op); opnd_idx++) {
	      BB *bp = Get_PHI_Predecessor(op, opnd_idx);
	      if (Get_PHI_Predecessor_Idx(op, bp) != opnd_idx) {
		DevWarn("SSA_Verify: BB %d opnd %d in PHI, inconsistency in PHI map.", BB_id(bb), opnd_idx);
		ok = FALSE;
	      }
	    }
	  }
	}
      }

      /* For all operations, including PHIs. */
      // Ignore uses of PCOPY prologue
      if (!OP_PCOPY(op) || !OP_prologue(op)) {
	for (INT i = 0; i < OP_opnds(op); i++) {
	  TN *tn = OP_opnd(op, i);
	  if (TN_is_register(tn) && TN_is_SSA_candidate(tn)) {
	    /* Verify tn links. */
	    ok &= SSA_Verify_TN(bb, op, tn);
	  }
	}
      }
      // Ignore definition of PCOPY epilogue
      if (!OP_PCOPY(op) || !OP_epilogue(op)) {
	for (INT i = 0; i < OP_results(op); i++) {
	  TN *tn = OP_result(op, i);
	  if (TN_is_register(tn) && TN_is_SSA_candidate(tn)) {
	    /* Verify tn links. */
	    ok &= SSA_Verify_TN(bb, op, tn);
	    /* Verify unicity of def. */
	    OP *def_op = TN_ssa_def(tn);
	    if (def_op != NULL && def_op != op) {
	      DevWarn("OP don't match the TN ssa OP:%d. TN result: PU:%s BB:%d OP:%d TN:%d",
		      OP_map_idx(def_op), Cur_PU_Name, BB_id(bb), OP_map_idx(op), TN_number(tn));
	      ok = FALSE;
	    }
	  }
	}
      }
    }
  }


  if (!ok) DevWarn("*** SSA_Verify FAILED");
  Is_True(ok, ("*** SSA_Verify FAILED"));
#endif
  return ok;
}

static TN_SET *dedicated_call_set = NULL;
static TN_SET *call_opnd_set = NULL;
static TN_SET *call_result_set = NULL;

static void
Set_Call_Dedicated_Opnds(BB *bb, CALLINFO *call_info) {

  TN_SET_ClearD(dedicated_call_set);
  INT call_opnds = 0;
  for (OP *op = OP_prev(BB_last_op(bb)), *prev_op; op != NULL; op = prev_op) {
    prev_op = OP_prev(op);
    for (INT i = 0; i < OP_results(op); i++) {
      TN *tn = OP_result(op,i);
      if (!TN_is_dedicated(tn) || !TN_SET_MemberP(call_opnd_set, tn))
	continue;
      if (!TN_SET_MemberP(dedicated_call_set, tn)) {
	TN_SET_Union1D(dedicated_call_set, tn, NULL);
	call_opnds++;
      }
    }
  }

  TN **opnds = NULL;
  if (call_opnds > 0) {
    opnds = TYPE_MEM_POOL_ALLOC_N(TN *, &MEM_pu_pool, call_opnds);
    INT i;
    TN *call_opnd;
    for ( call_opnd = TN_SET_Choose(dedicated_call_set), i = 0;
	  call_opnd != TN_SET_CHOOSE_FAILURE;
	  call_opnd = TN_SET_Choose_Next(dedicated_call_set, call_opnd), i++ )
      opnds[i] = call_opnd;
  }
  CALLINFO_call_opnds(call_info) = call_opnds;
  CALLINFO_call_opnd(call_info) = opnds;
}

/* ======================================================================
 *   Set_Call_Dedicated_Results
 *
 *   Implement a small forward pass in order to determine
 *   the CALL results .
 *   Resemble routine Get_Intrinsic_Call_Dedicated_Tn
 * ======================================================================
 */
static BOOL
Set_Call_Dedicated_Results(BB *bb, CALLINFO *call_info) {

  TN_SET_ClearD(dedicated_call_set);
  INT call_results = 0;
  // Continue to search for uses of dedicated registers for call
  // results until a duplicated use or a def of a dedicated register
  // is found.
  for (OP *op = BB_first_op(bb), *succ_op; op != NULL; op = succ_op) {
    succ_op = OP_next(op);
    for (INT i = 0; i < OP_opnds(op); i++) {
      TN *tn = OP_opnd(op,i);
      if (!TN_is_dedicated(tn) || !TN_SET_MemberP(call_result_set, tn))
	continue;
      if (!TN_SET_MemberP(dedicated_call_set, tn)) {
	TN_SET_Union1D(dedicated_call_set, tn, NULL);
	call_results++;
      }
      else {
	// Found a dedicated already used, stop now
	succ_op = NULL;
	break;
      }
    }
    // Check if no dedicated register is defined.
    for (INT i = 0; i < OP_results(op); i++) {
      TN *tn = OP_result(op,i);
      if (TN_is_dedicated(tn)) {
	succ_op = NULL;
	break;
      }
    }
  }

  if (call_results > 0) {
    INT new_results = CALLINFO_call_results(call_info) + call_results;
    TN **results = TYPE_MEM_POOL_ALLOC_N(TN *, &MEM_pu_pool, new_results);
    INT i;
    TN *call_result;
    Is_True(new_results == TN_SET_Size(dedicated_call_set), ("Internal error"));
    for ( call_result = TN_SET_Choose(dedicated_call_set), i = 0;
	  call_result != TN_SET_CHOOSE_FAILURE;
	  call_result = TN_SET_Choose_Next(dedicated_call_set, call_result), i++ )
      results[i] = call_result;
    if (CALLINFO_call_result(call_info) != NULL)
      MEM_POOL_FREE(&MEM_pu_pool, CALLINFO_call_result(call_info));
    CALLINFO_call_results(call_info) = new_results;
    CALLINFO_call_result(call_info) = results;
  }
}

void
SSA_init_call_parms(RID *rid, BOOL region) {

  // First, init the set of possible input and output paramters for a
  // call.
  dedicated_call_set = TN_SET_Create_Empty (Last_Dedicated_TN+1, &MEM_local_pool);
  call_opnd_set = TN_SET_Create_Empty (Last_Dedicated_TN+1, &MEM_local_pool);
  call_result_set = TN_SET_Create_Empty (Last_Dedicated_TN+1, &MEM_local_pool);

  ISA_REGISTER_CLASS rc;
  FOR_ALL_ISA_REGISTER_CLASS(rc) {
    REGISTER reg;
    REGISTER_SET call_opnds = REGISTER_CLASS_function_argument(rc);
    FOR_ALL_REGISTER_SET_members(call_opnds, reg) {
      TN *tn = Build_Dedicated_TN(rc, reg, 0);
      TN_SET_Union1D(call_opnd_set, tn, NULL);
    }
    REGISTER_SET call_results = REGISTER_CLASS_function_value(rc);
    FOR_ALL_REGISTER_SET_members(call_results, reg) {
      TN *tn = Build_Dedicated_TN(rc, reg, 0);
      TN_SET_Union1D(call_result_set, tn, NULL);
    }
    if (RS_TN != NULL)
      TN_SET_Union1D(call_opnd_set, RS_TN, NULL);
  }

  // Then, look for call blocks and init the input and output
  // parameters
  BB *bb, *bb_next;
  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    if (!BB_call(bb))
      continue;
    CALLINFO *call_info = ANNOT_callinfo(ANNOT_Get(BB_annotations(bb), ANNOT_CALLINFO));
    Set_Call_Dedicated_Opnds(bb, call_info);
    if ((bb_next = BB_Fall_Thru_Successor(bb)) != NULL) 
      Set_Call_Dedicated_Results(bb_next, call_info);
  }
}
