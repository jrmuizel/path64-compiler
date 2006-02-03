

// Instrument the whirl for coverage purposes.  First part of this
// file is dedicated to build a CFG from the whirl The second part
// generates coverage stuffs from this CFG.  '.gcno' file genration
// and finding a minimun edge profiling come from gcc 3.4.4 source
// distribution (see profile.c and coverage.c in the gcc 3.4.4 source
// distrib). For more details contact thierry.bidault@st.com
#include "W_alloca.h"
#include "wn.h"
#include "wn_coverage.h"
#include "ir_reader.h" // for fdump_tree_with_freq
#undef DEBUG_COVERAGE
#if 1 // def DEBUG_COVERAGE
#include "tracing.h" // For debug purpose
#endif
#define GCOV_LINKAGE
#include "gcov-io.h"
#include "gcov_hwint.h"
#include "gcov-io.c"
#include "srcpos.h"
#include "data_layout.h"
#include "const.h" // for New_Const_Sym
#include <unistd.h> // for get_current_dir_name
#include "glob.h" // for Src_File_Name
#include <time.h> // for time()
#include "dwarf_DST.h"      /* for DST_IDX and DST_INFO_IDX */
#include "wn_util.h" // for WN_INSERT_BlockBefore
#include "pu_info.h"		    /* for PU_Info */
#include "clone.h" // for IPO_CLONE
#include "clone_DST_utils.h"
#include "dwarf_DST_producer.h" // for DST_mk_subprogram
#include "wn_instrument.h" // for Gen_Call
#include "wn_lower.h" // for LOWER_SCF
#include "fb_info.h" // For feedback filling
#include "fb_whirl.h" // For Cur_PU_Feedback
#include "sections.h" // For_SEC_TEXT
#include "file_util.h" // For Remove_Extension
// memory for this module
static MEM_POOL MEM_coverage_pool_val, *MEM_coverage_pool;
static MEM_POOL MEM_coverage_pool_pu_val, *MEM_coverage_pool_pu;


/* ====================================================================
 *
 *          Build CFG from the whirl
 *
 * ====================================================================
 */


static BOOL Trace_Coverage;
// Only for debug purpose: put to one to test propagation phase (in
// this case propagation passe is not done since all freq are known)
#define instrument_all_edges 0
static unsigned BB_index;
/* Unique identifier for the current function */
static unsigned current_function_funcdef_no;

typedef struct wnode
{
  WN *wn;
  /* Next whirl node.  */
  struct wnode *next;
} wnode_list_t;

typedef struct bbnode *pbasic_block_list_t;

typedef struct basic_block
{
  unsigned index;
  unsigned int is_a_call : 1;
  unsigned int is_a_label : 1;
  unsigned int is_a_branch : 1;
  unsigned int never_return : 1;
  unsigned int is_exit : 1;
  unsigned int is_entry : 1;

  // To compute grah frequencies from minimal edge profiling
  unsigned int count_valid : 1;

  // List of wn for that BB
  wnode_list_t *wn_list;
  // short cut to last wn of wn_list
  WN *last_wn;
  // Branch wn
  WN *branch_wn;

  // List of successor for that BB
  pbasic_block_list_t succs;
  // nb of successor
  unsigned n_succs;
  // nb of predecessor
  unsigned n_preds;
  // next in the control flow
  struct basic_block *next;

  /* Number of successor and predecessor edges, used to propagate
     frequencies.  */
  gcov_type succ_count;
  gcov_type pred_count;
  // List of successor edges (n_succs elt)
  int *index_of_succ_edges;
  // List of predecessor edges (n_preds element)
  int *index_of_pred_edges;
  
  // Counter = nb of time this bb is executed
  gcov_type count;
  // nb of fake successor (should be 0 or 1)
//   unsigned n_succs_fake;
} basic_block_t;

// basic block index accessor
#define BB_id(bb) ((bb)->index)
typedef struct bbnode
{
  basic_block_t *bb;
  /* Next bb in the PU.  */
  struct bbnode *next;
} basic_block_list_t;

typedef struct function
{
  unsigned index;
  // List of wn for that BB
  basic_block_list_t *bb_list;
  // Nb of bb in that PU
  unsigned bb_count;
} function_t;

struct function_list
{
  struct function_list *next;	 /* next function */
  unsigned ident;		 /* function ident */
  unsigned checksum;	         /* function checksum */
  unsigned n_ctrs[GCOV_COUNTERS];/* number of counters.  */
};


static struct function_list *functions_head = 0;
static struct function_list **functions_tail = &functions_head;

/* Cumulative counter information for whole program.  */
static unsigned prg_ctr_mask; /* Mask of counter types generated.  */
static unsigned prg_n_ctrs[GCOV_COUNTERS]; /* Total counters allocated.  */

/* Counter information for current function.  */
static unsigned fn_ctr_mask; /* Mask of counters used.  */
static unsigned fn_n_ctrs[GCOV_COUNTERS]; /* Counters allocated.  */
static unsigned fn_b_ctrs[GCOV_COUNTERS]; /* Allocation base.  */

/* Name of the output file for coverage output file.  */
static char *bbg_file_name;
static unsigned bbg_file_opened;
static int bbg_function_announced;

/* Name of the count data file.  */
static char *da_file_name;
// Current working dir
static char *cwdname;

/* Main pu */
static WN* Tree_WN;

/* A random sequence of characters, unless overridden by user.  */
static const char *flag_random_seed;

/* A local time stamp derived from the time of compilation. It will be
   zero if the system cannot provide a time.  It will be -1u, if the
   user has specified a particular random seed.  */
static unsigned local_tick;

static SRCPOS last_srcpos;
static unsigned total_num_edges_instrumented;

/* The names of the counter tables. Allocated once for the whole
   program  */
static ST* ctr_labels[GCOV_COUNTERS];

/* The names of merge functions for counters.  */
static const char *const ctr_merge_functions[GCOV_COUNTERS] = GCOV_MERGE_FUNCTIONS;
static ST * ctr_merge_functions_st[GCOV_COUNTERS];
static const char *const ctr_names[GCOV_COUNTERS] = GCOV_COUNTER_NAMES;

static ST     *gcov_init_thunk_st; //thunck symbol where __gcov_init is called
static ST *gcov_info_st; // .PBBX0 data
#define FILE_FUNCTION_FORMAT "_GLOBAL__%s_%s_GCOV"

/* 
   Since the basic blocks are expected to be numbered sequentially we
need a matching between BB_id and a gcov number
 */
#define BB_TO_GCOV_INDEX(bb)  (bb->index)
#define LAST_BB_TO_GCOV_INDEX (Cur_Function->bb_count + 1)
#define FIRST_BB_TO_GCOV_INDEX (0)


/* Control flow edge information.  */
typedef struct edge_def {
  /* The two blocks at the ends of the edge.  */
  basic_block_t *src, *dest;

  unsigned int to_exit : 1; // 1 if the edge points to the gcov exit (in this case dest == NULL)
  unsigned int from_entry : 1; // 1 if the edge is from the gcov entry (in this case src == NULL)
  unsigned int on_tree : 1;
  unsigned int fake : 1;
  unsigned int fall_through : 1;
  unsigned int critical : 1;
  // To compute grah frequencies from minimal edge profiling
  unsigned int count_valid : 1;

    // Counter= nb of time this edge is executed
  gcov_type count;

} *edge_t;

/* This structure maintains an edge list vector.  */
struct edge_list
{
  int num_edges;
  edge_t *index_to_edge;
};

/* INDEX_EDGE_PRED_BB and INDEX_EDGE_SUCC_BB return a pointer to the basic
   block which is either the pred or succ end of the indexed edge.  */
// #define INDEX_EDGE_PRED_BB(el, index)	((el)->index_to_edge[(index)]->src)
// #define INDEX_EDGE_SUCC_BB(el, index)	((el)->index_to_edge[(index)]->dest)
/* INDEX_EDGE returns a pointer to the edge.  */
#define INDEX_EDGE(el, index)           ((el)->index_to_edge[(index)])

// Current basic block being created
static basic_block_t *Cur_BB;
// Current function 
static function_t *Cur_Function;
// list of edges for current function
static  struct edge_list *edges_list;
// Some gcov specific bb
static basic_block_t *First_BB; // with index FIRST_BB_TO_GCOV_INDEX
static basic_block_t *Last_BB; // with index  LAST_BB_TO_GCOV_INDEX

// Forward static function declarion
static void Build_CFG(void);
static WN *gen_edge_profiler (unsigned);
static WN *gen_critical_edge_profiler (unsigned, basic_block_t *, basic_block_t * );
static char *get_file_function_name_long (const char *, const char *);
static void read_counts_file (void);
/* Use .gcda file to get branch prob*/
static void wn_coverage_branch_probabilities(void);
static unsigned compute_checksum (void);
static unsigned coverage_checksum_string (unsigned, const char *);
static void create_coverage (TY_IDX );
static basic_block_t *new_bb(MEM_POOL *);
static void add_fake_edge_to_Cur_Function (void);
// Walk the CFG and annotate the WHIRL with adequate info
static void wn_coverage_annotate( WN *) ;

typedef struct {
  char *filename;
  INT incl_index;
} file_info;

static file_info *file_table = NULL;
static char **incl_table;
static BOOL file_table_generated = FALSE;


#if 1// def DEBUG_COVERAGE
#define TraceCovFile TFile
/* ====================================================================
 *
 * Print_BB_Header
 *
 * Print the header information associated with a BB to the trace file.
 *
 * ====================================================================
 */
static void
Print_BB_Header ( basic_block_list_t *bpl)
{
  basic_block_t *bp = bpl->bb;
  if (bp->count_valid)
    fprintf ( TraceCovFile,
	      "BB:%d[%d] -- ",
	      bp->index, (int)bp->count);
  else
    fprintf ( TraceCovFile,
	      "BB:%d -- ",
	      bp->index);
  fprintf ( TraceCovFile, "\n");
  
  if (bp->is_entry)	fprintf ( TraceCovFile, "  Entry block\n" );
  if (bp->is_exit) {
    if (bp->is_a_call)	fprintf ( TraceCovFile, "  Tail call block\n" );
    else		fprintf ( TraceCovFile, "  Exit block\n" );
  } else if (bp->is_a_call) fprintf ( TraceCovFile, "  Call block\n" );
  if (bp->is_a_branch)	fprintf ( TraceCovFile, "  Branch block\n" );
  if (bp->is_a_label)	fprintf ( TraceCovFile, "  Label block\n" );

  /* Print predecessor/successor information: */
  fprintf ( TraceCovFile, "In linear order, BB_next == BB:%2d\n",
	    (bpl->next ? bpl->next->bb->index : 0xFFFFFFFF) );

  fprintf ( TraceCovFile, "Nb Predecessors:\t%d\n", bp->n_preds);

  fprintf ( TraceCovFile, "\nSuccessors:\t");
  basic_block_list_t *list_bb;
  int i = 0;
  for ( list_bb = bp->succs; list_bb != NULL; list_bb = list_bb->next) {
    edge_t succ_e = bp->index_of_succ_edges ? INDEX_EDGE (edges_list, bp->index_of_succ_edges[i]) : NULL;
    if (succ_e && succ_e->count_valid){
      gcov_type count;
      count = succ_e->count;
      fprintf ( TraceCovFile, "%sBB:%2d[%d]",
	      ( (i == 0) ? "" : (i%5 == 0) ? ",\n\t\t" : ", " ),
	      list_bb->bb->index, 
	      (int)count);
    } else
      fprintf ( TraceCovFile, "%sBB:%2d",
	      ( (i == 0) ? "" : (i%5 == 0) ? ",\n\t\t" : ", " ),
	      list_bb->bb->index);
    ++i;
  }
  if ( i == 0 ) fprintf ( TraceCovFile, "none" );
  fprintf ( TraceCovFile, "\n" );

  return;
}
/* ================================================================= */

static void 
Print_BB ( basic_block_list_t *bpl, BOOL dumpwh )
{
  basic_block_t *bp = bpl->bb;
  fprintf ( TraceCovFile, "%sBB:%d \n%s", SBar, bpl->bb->index, SBar );
  Print_BB_Header ( bpl );
  fprintf ( TraceCovFile, "\n" );
  if (dumpwh){
    wnode_list_t *list_wn;
    for ( list_wn = bp->wn_list; list_wn != NULL; list_wn = list_wn->next) {
      fdump_tree(TraceCovFile, list_wn->wn);
      fprintf ( TraceCovFile, "\n" );
    }
  }
  fprintf ( TraceCovFile, "\n" );
} 

/* ================================================================= */

static void 
wn_coverage_Print_All_BBs (BOOL dumpwh) 
{
  basic_block_list_t *list_bb;
//   if (TraceCovFile == NULL) TraceCovFile = stdout;
  for ( list_bb = Cur_Function->bb_list; list_bb != NULL; 
	list_bb = list_bb->next) 
    {
      Print_BB ( list_bb, dumpwh );
      fprintf ( TraceCovFile,"\n" );
    }
}

void wn_coverage_debug()
{
  wn_coverage_Print_All_BBs(1);  
}

static basic_block_list_t *
find_stmt_in_cfg(WN *wn, basic_block_list_t *from_lbb) {

  basic_block_list_t *list_bb;
  int i = 0;
  if (from_lbb == NULL) from_lbb = Cur_Function->bb_list;
  basic_block_list_t *first_list_bb = from_lbb;
  
  for ( list_bb = from_lbb; list_bb != NULL; list_bb = list_bb->next) {
    basic_block_t *bb = list_bb->bb;
    wnode_list_t *list_wn;
    for ( list_wn = bb->wn_list; list_wn != NULL; list_wn = list_wn->next) {
      if (list_wn->wn == wn)
	return list_bb;
    }
    i++;
    if (i == 2) return first_list_bb;
  }
  return NULL;
}
static void
dump_instrumented_whirl_with_bb( WN *tree ) 
{
  WN *stmt;
  WN *next_stmt;
  basic_block_list_t *lbb = NULL;
  basic_block_list_t *last_lbb = NULL;

  Is_True(WN_opcode( tree ) == OPC_FUNC_ENTRY, ("Coverage_Tree_Walk, unknown root node opcode"));

  stmt = WN_entry_first( tree );

  for( ; stmt; stmt = next_stmt ) {
    lbb = find_stmt_in_cfg(stmt, last_lbb);
    if (lbb && lbb != last_lbb)
      {
	fprintf ( TraceCovFile, "%sBB:%d \n%s", SBar, lbb->bb->index, SBar );
	Print_BB_Header ( lbb );
	fprintf ( TraceCovFile, "\n" );
      }
    if (lbb) last_lbb = lbb;
    fdump_tree(TraceCovFile, stmt);
    fprintf ( TraceCovFile, "\n" );
    next_stmt = WN_next(stmt);
  }
}
#endif
static void 
Add_Stmt_To_Cur_BB(WN *stmt) {

  wnode_list_t *list_wn, *new_list_wn;

  // Insert stmt at the end of wn_list
  for ( list_wn = Cur_BB->wn_list; list_wn != NULL && list_wn->next != NULL; list_wn = list_wn->next) 
    /* NOTHING*/ ;
  
  new_list_wn = TYPE_MEM_POOL_ALLOC_N(wnode_list_t,
				      MEM_coverage_pool_pu,
				      1);
  new_list_wn->wn = stmt;
  new_list_wn->next = NULL;
  if (list_wn)
    list_wn->next = new_list_wn;
  else
    Cur_BB->wn_list = new_list_wn;
  Cur_BB->last_wn = stmt;
}

static void 
Add_BB_To_Cur_Function(basic_block_t *bb) {
  basic_block_list_t *list_bb, *new_list_bb;

  // Add bb to the end of bb_list of current function.
  for ( list_bb = Cur_Function->bb_list; list_bb != NULL && list_bb->next != NULL; list_bb = list_bb->next) 
    /* NOTHING*/ ;
  
  new_list_bb = TYPE_MEM_POOL_ALLOC_N(basic_block_list_t,
				      MEM_coverage_pool_pu,
				      1);
  new_list_bb->bb = bb;
  new_list_bb->next = NULL;
  if (list_bb) {
    list_bb->next = new_list_bb;
    // Update next bb in the flow of the previous bb 
    list_bb->bb->next = bb;
  }
  else
    Cur_Function->bb_list = new_list_bb;

  Cur_Function->bb_count++;
  

}

static void
Link_Pred_Succ(basic_block_t *pred, basic_block_t *succ)
{
  basic_block_list_t *new_list_bb;
  pbasic_block_list_t list_bb, pre_list_bb = NULL;

  // add succ to the end of successor list of pred
  // Check that succ is not already in the list
  for ( list_bb = pred->succs; list_bb != NULL; 
	list_bb = list_bb->next) 
    {
      if (list_bb->bb == succ) return;
      pre_list_bb = list_bb;
    }
  
  list_bb = pre_list_bb;
  new_list_bb = TYPE_MEM_POOL_ALLOC_N(basic_block_list_t,
				      MEM_coverage_pool_pu,
				      1);
  new_list_bb->bb = succ;
  new_list_bb->next = NULL;
  if (list_bb) {
    list_bb->next = new_list_bb;
  }
  else
    pred->succs = new_list_bb;

  pred->n_succs++;


  // add pred to the beginning of predessor list of succ
  //no need of a predecessor list, only a count
  succ->n_preds++;
}

static void
Start_New_Basic_Block (void)
{
//   fprintf(TraceCovFile,"new BB\n");
  // Add current BB to current PU if exists (does not exist for entry
  // WN)
  if (Cur_BB)
    Add_BB_To_Cur_Function(Cur_BB);
  Cur_BB = new_bb(MEM_coverage_pool_pu);
  Cur_BB->index = ++BB_index;
}

static void
Start_New_Function (void)
{
//   fprintf(TraceCovFile,"new Function\n");
  Cur_Function = TYPE_MEM_POOL_ALLOC_N(function_t,
				       MEM_coverage_pool_pu,
				       1);
  Cur_Function->index = current_function_funcdef_no;
}

static void 
Handle_Entry (WN *entry) 
{
  FmtAssert (Cur_BB == NULL, 
	     ("Handle_Entry: Current bb is not NULL"));
  Start_New_Basic_Block ();
  Cur_BB->is_entry = 1;
}
    
static void
Handle_Branch (
  WN *branch
)
{
  // Remind the branch statement
  Cur_BB->branch_wn = branch;
  Cur_BB->is_a_branch = 1;
  Start_New_Basic_Block ();
}

static void
Handle_Call_Site (
  WN **call
)
{
//   fprintf(TraceCovFile,"call site\n");
  Cur_BB->is_a_call = 1;
  if (WN_Call_Never_Return( *call )) {
    Cur_BB->is_exit = 1;
    Cur_BB->never_return = 1;
  }
  // Attach all return value stuff to the call bb
  WN *point = WN_next(*call);
  WN *insert_point = *call;
  while(point != NULL) {
    if (!(OPERATOR_is_store(WN_operator(point)) &&
	  !stmt_is_store_of_return_value(point))) break;
    if (stmt_is_store_of_callee_return_value(point))
      insert_point = point;
    point = WN_next(point);
  }
  for ( WN *wn = *call; wn != insert_point; /* NOTHING */) {
    wn = WN_next(wn);
    Add_Stmt_To_Cur_BB(wn);
  }
  // Set the call to the next of the current statement
  *call = insert_point;
  Start_New_Basic_Block ();
}


static void
Expand_Expr (
  WN **expr
)
{
  OPERATOR opr;
  opr = WN_operator(*expr);
  switch (opr) {
  case OPR_CALL:
  case OPR_ICALL:
  case OPR_PICCALL:
  case OPR_INTRINSIC_CALL:
  case OPR_IO:
    Handle_Call_Site (expr);
    return;
  default:
    break;
  }
  return;
}

static void 
Handle_Return (void)
{
//   fprintf(TraceCovFile,"Return\n");
  Cur_BB->is_exit = 1;
  Start_New_Basic_Block ();
}

static void 
Handle_Label (void)
{
//   fprintf(TraceCovFile,"Label\n");
  // Create a new BB if previous one is not empty
  if (Cur_BB->wn_list != NULL) {
    Start_New_Basic_Block ();
  }
  Cur_BB->is_a_label = 1;
}

static void
Expand_Statement(WN **stmt)
{
  OPCODE opc = WN_opcode(*stmt);
  switch (opc) {
  case OPC_TRUEBR:
  case OPC_FALSEBR:
  case OPC_GOTO:
  case OPC_AGOTO:
  case OPC_COMPGOTO:
  case OPC_XGOTO:
  case OPC_REGION_EXIT:
    Handle_Branch (*stmt);
    break;
  case OPC_RETURN:
    Handle_Return ();
    break;
  case OPC_LABEL:
    Handle_Label ();
    break;

  default:
    Expand_Expr (stmt);
    break;
  }
}  

static void
Coverage_Tree_Walk( WN *tree ) 
{
  WN *stmt;
  WN *next_stmt;
#ifdef DEBUG_COVERAGE
  fprintf(TraceCovFile, "WHIRL tree input to Coverage_Tree_Walk:\n");
//   fdump_tree_with_freq(TraceCovFile, tree, WN_MAP_UNDEFINED);
  dump_tree(tree);
#endif
  Is_True(WN_opcode( tree ) == OPC_FUNC_ENTRY, ("Coverage_Tree_Walk, unknown root node opcode"));

  Start_New_Function();
  Handle_Entry( tree );
  stmt = WN_entry_first( tree );

  for( ; stmt; stmt = next_stmt ) {
    // For a label add stmt to next BB
    if (WN_opcode(stmt) != OPC_LABEL)
	Add_Stmt_To_Cur_BB(stmt);
    Expand_Statement( &stmt );
    if (WN_opcode(stmt) == OPC_LABEL)
	Add_Stmt_To_Cur_BB(stmt);
    next_stmt = WN_next(stmt);
  }
  /* Because of the way Start_New_Basic_Block works, we almost
   * certainly end up with one extra BB. Verify that we have an extra
   * BB, and add it if so. */
  if (Cur_BB && Cur_BB->wn_list)
    Add_BB_To_Cur_Function(Cur_BB);

  /* Build the control flow graph */
  Build_CFG();
}

static basic_block_t *
Get_BB_Label(WN *wn, basic_block_list_t **bb_list = NULL)
{
  basic_block_list_t *list_bb;
  LABEL_IDX label = WN_label_number(wn);
  FmtAssert (OPCODE_has_label(WN_opcode(wn)),
	("Get_BB_Label: invalid opcode %d ", WN_opcode(wn)));
  FmtAssert (label > 0 && label <= LABEL_Table_Size(CURRENT_SYMTAB),
	("Get_BB_Label: label %d greater than last label %d", 
		label, LABEL_Table_Size(CURRENT_SYMTAB)));
  for ( list_bb = Cur_Function->bb_list; list_bb != NULL; list_bb = list_bb->next) 
    {
      basic_block_t *bb = list_bb->bb;
      wnode_list_t *list_wn;
      for ( list_wn = bb->wn_list; list_wn != NULL; list_wn = list_wn->next) 
	{
	  WN *lwn = list_wn->wn;
	  if (WN_opcode(lwn) == OPC_LABEL && 
	      (LABEL_IDX)WN_label_number(lwn) == label) {
	    if (bb_list) *bb_list = list_bb;
	    return bb;
	  }
	}
    }
  FmtAssert (FALSE,
	     ("Get_BB_Label: invalid label %d ", label));
  return NULL;
}

/* Build the control flow graph for the code generator. */
static void Build_CFG(void)
{
  basic_block_list_t *list_bb;
  WN *branch_wn;

  for ( list_bb = Cur_Function->bb_list; list_bb != NULL; list_bb = list_bb->next) 
    {
      basic_block_t *bb = list_bb->bb;
      if (bb->is_exit) {
	/* There are no successors for a procedure exit. */
	continue;
      }
      branch_wn = bb->branch_wn;
      if (branch_wn != NULL) {
	switch (WN_opcode(branch_wn)) 
	  {
	  case OPC_FALSEBR:
	  case OPC_TRUEBR:
	    Link_Pred_Succ(bb, list_bb->next->bb);
	    /* fall through and link the branch target. */
	  case OPC_GOTO:
	  case OPC_REGION_EXIT:
	    Link_Pred_Succ(bb, Get_BB_Label(branch_wn));
	    break;
	  case OPC_COMPGOTO:
	    if (WN_kid_count(branch_wn) == 3) { /* default case */
	      Link_Pred_Succ (bb, Get_BB_Label(WN_kid(branch_wn,2)));
	    } /* fall thru */
	  case OPC_XGOTO:
	    { 
	      WN *wn;
	      int i;
	      wn = WN_first(WN_kid1(branch_wn));	/* first goto */
	      for (i = 0; i < WN_num_entries(branch_wn); i++) {
		Link_Pred_Succ (bb, Get_BB_Label(wn));
		wn = WN_next(wn);
	      }
	    }
	    if (WN_kid_count(branch_wn) == 3) {
	      Link_Pred_Succ (bb, Get_BB_Label(WN_kid(branch_wn,2)));
	    }
	    break;
	  case OPC_AGOTO:
	    DevWarn("Build_CFG for coverage: OPC_AGOTO not handled");
	    break;
	  default:
	    FmtAssert (FALSE, 
		       ("Build_CFG: unexpected opcode %s", OPCODE_name(WN_opcode(branch_wn))));
	    break;
	  }
      } else if (list_bb->next == NULL) {
	// Case of dead block
	    continue;	// no successor
      } else if (list_bb->next->bb != NULL) {
	if (bb->is_a_call
	    && bb->never_return)
	  {
	    continue;	// no successor
	  } else {
	    Link_Pred_Succ (bb, list_bb->next->bb);
	  }
      }
    }
#ifdef DEBUG_COVERAGE
  wn_coverage_Print_All_BBs(0);
#endif
}

/* ====================================================================
 *
 *          Generate coverage info and instrumentation
 *
 * ====================================================================
 */

/* Set up a default flag_random_seed and local_tick, unless the user
   already specified one.  */
static void
randomize (void)
{
  if (!flag_random_seed)
    {
      unsigned HOST_WIDE_INT value;
      static char random_seed[HOST_BITS_PER_WIDE_INT / 4 + 3];

      /* Get some more or less random data.  */
#ifdef HAVE_GETTIMEOFDAY
      {
 	struct timeval tv;

 	gettimeofday (&tv, NULL);
	local_tick = tv.tv_sec * 1000 + tv.tv_usec / 1000;
      }
#else
      {
	time_t now = time (NULL);

	if (now != (time_t)-1)
	  local_tick = (unsigned) now;
      }
#endif
      value = local_tick ^ getpid ();

      sprintf (random_seed, HOST_WIDE_INT_PRINT_HEX, value);
      flag_random_seed = random_seed;
    }
  else if (!local_tick)
    local_tick = 0xFFFFFFFF;
}

/* Generate a crc32 of a string.  */

static unsigned
crc32_string (unsigned chksum, const char *string)
{
  do
    {
      unsigned value = *string << 24;
      unsigned ix;
      
      for (ix = 8; ix--; value <<= 1)
  	{
  	  unsigned feedback;
  	  
  	  feedback = (value ^ chksum) & 0x80000000 ? 0x04c11db7 : 0;
 	  chksum <<= 1;
 	  chksum ^= feedback;
  	}
    }
  while (*string++);
  return chksum;
}

/* Generate a checksum for a string.  CHKSUM is the current
   checksum.  */

static unsigned
coverage_checksum_string (unsigned chksum, const char *string)
{
  int i;
  char *dup = NULL;

  /* Look for everything that looks if it were produced by
     get_file_function_name_long and zero out the second part
     that may result from flag_random_seed.  This is not critical
     as the checksums are used only for sanity checking.  */
  for (i = 0; string[i]; i++)
   {
      if (!strncmp (string + i, "_GLOBAL__", 9))
	for (i = i + 9; string[i]; i++)
	  if (string[i]=='_')
	    {
	      int y;
	      unsigned seed;

	      for (y = 1; y < 9; y++)
		if (!(string[i + y] >= '0' && string[i + y] <= '9')
		    && !(string[i + y] >= 'A' && string[i + y] <= 'F'))
		  break;
	      if (y != 9 || string[i + 9] != '_')
		continue;
	      for (y = 10; y < 18; y++)
		if (!(string[i + y] >= '0' && string[i + y] <= '9')
		    && !(string[i + y] >= 'A' && string[i + y] <= 'F'))
		  break;
	      if (y != 18)
		continue;
	      if (!sscanf (string + i + 10, "%X", &seed))
		FmtAssert(FALSE, ("coverage_checksum_string: don't know what to do!"));
	      if (seed != crc32_string (0, flag_random_seed))
		continue;
	      string = dup = strdup (string);
	      for (y = 10; y < 18; y++)
		dup[i + y] = '0';
	      break;
	    }
      break;
    }

  chksum = crc32_string (chksum, string);
  if (dup)
    free (dup);

  return chksum;
}


// Create a type for gcov_ctr_info type
// /* Information about counters.  */
// struct gcov_ctr_info
// {
//   gcov_unsigned_t num;		/* number of counters.  */
//   gcov_type *values;		/* their values.  */
//   gcov_merge_fn merge;  	/* The function used to merge them.  */
// };
static TY_IDX
build_ctr_info_type(TY_IDX counter_type) {
  FLD_HANDLE first_field, field;
  TY_IDX gcov_ctr_info_idx;
  TY_IDX ptr_idx1, ptr_idx2;

  TY& gcov_ctr_info = New_TY(gcov_ctr_info_idx);

  first_field = New_FLD ();
  FLD_Init (first_field, Save_Str("num"), 
	    Be_Type_Tbl(MTYPE_U4), 0);

  ptr_idx1 = Make_Pointer_Type(counter_type);
  ptr_idx2 = Make_Pointer_Type(MTYPE_To_TY(MTYPE_V));

  field = New_FLD ();
  FLD_Init (field, Save_Str("values"), 
	   ptr_idx1, 4);

  field = New_FLD ();
  FLD_Init (field, Save_Str("merge"), 
	    ptr_idx2, 8);

  Set_FLD_last_field(field);
  TY_Init (gcov_ctr_info, TY_size(Be_Type_Tbl(MTYPE_U4)) + TY_size(ptr_idx1) + 
	   TY_size(ptr_idx2),
	   KIND_STRUCT, MTYPE_M, Save_Str ("gcov_ctr_info"));
  Set_TY_align(gcov_ctr_info_idx, 4);
  Set_TY_fld(gcov_ctr_info, first_field);

  return gcov_ctr_info_idx;
}

// Create a type for gcov_fn_info type
// struct gcov_fn_info
// {
//   gcov_unsigned_t ident;	/* unique ident of function */
//   gcov_unsigned_t checksum;	/* function checksum */
//   unsigned n_ctrs[0];		/* instrumented counters */
// };
static TY_IDX
build_fn_info_type() {
  unsigned ix;
  TY_IDX gcov_fn_info_idx;
  FLD_HANDLE first_field, field;
  TY& gcov_fn_info = New_TY(gcov_fn_info_idx);

  first_field = New_FLD ();
  FLD_Init (first_field, Save_Str("ident"), 
	    Be_Type_Tbl(MTYPE_U4), 0);

  field = New_FLD ();
  FLD_Init (field, Save_Str("checksum"), 
	    Be_Type_Tbl(MTYPE_U4), 4);

  for (ix = 0; ix != GCOV_COUNTERS; ix++)
    if (prg_ctr_mask & (1 << ix))
      {
	field = New_FLD ();
	FLD_Init (field, Save_Str("n_ctrs"), 
		  Be_Type_Tbl(MTYPE_U4), 8);
      }
  Set_FLD_last_field(field);
  TY_Init (gcov_fn_info, 3*TY_size(Be_Type_Tbl(MTYPE_U4)),
	   KIND_STRUCT, MTYPE_M, Save_Str ("gcov_fn_info"));
  Set_TY_align(gcov_fn_info_idx, 4);
  Set_TY_fld(gcov_fn_info, first_field);

  return gcov_fn_info_idx;
}

// Create a type for gcov_info type
// /* Information about a single object file.  */
// struct gcov_info
// {
//   gcov_unsigned_t version;	/* expected version number */
//   struct gcov_info *next;	/* link to next, used by libgcov */
//   gcov_unsigned_t stamp;	/* uniquifying time stamp */
//   const char *filename;		/* output file name */
//   unsigned n_functions;		/* number of functions */
//   const struct gcov_fn_info *functions; /* table of functions */
//   unsigned ctr_mask;		/* mask of counters instrumented.  */
//   struct gcov_ctr_info counts[0]; /* count data. The number of bits
// 				     set in the ctr_mask field
// 				     determines how big this array
// 				     is.  */
// };

static TY_IDX
build_gcov_info_type(TY_IDX gcov_fn_info_idx, TY_IDX gcov_ctr_info_idx) {
  FLD_HANDLE first_field, field;
  TY_IDX ptr_idx1, ptr_idx2;
  TY_IDX ptr_idx3;

  first_field = New_FLD ();
  FLD_Init (first_field, Save_Str ("version"), 
	    Be_Type_Tbl(MTYPE_U4), 0);
  
  ptr_idx1 = Make_Pointer_Type(MTYPE_To_TY(MTYPE_U4));
  ptr_idx2 = Make_Pointer_Type(MTYPE_To_TY(MTYPE_I1));
  ptr_idx3 = Make_Pointer_Type(gcov_fn_info_idx);
  
  field = New_FLD ();
  FLD_Init (field, Save_Str("next"), 
	    ptr_idx1, 4);
  
  field = New_FLD ();
  FLD_Init (field, Save_Str("stamp"), 
	    Be_Type_Tbl(MTYPE_U4), 8);
  
  field = New_FLD ();
  FLD_Init (field, Save_Str("filename"), 
	    ptr_idx2, 12);

  field = New_FLD ();
  FLD_Init (field, Save_Str("n_functions"), 
	    Be_Type_Tbl(MTYPE_U4), 12);
  
  field = New_FLD ();
  FLD_Init (field, Save_Str("functions"), 
	    ptr_idx3, 16);
  
  field = New_FLD ();
  FLD_Init (field, Save_Str("ctr_mask"), 
	    Be_Type_Tbl(MTYPE_U4), 20);
  
  field = New_FLD ();
  FLD_Init (field, Save_Str("counts"), 
	    gcov_ctr_info_idx, 24);
  
  Set_FLD_last_field(field);
  
  // declare a struct with the above field list 
  TY_IDX gcov_info_idx;
  TY& gcov_info = New_TY(gcov_info_idx);
  TY_Init (gcov_info, 4*TY_size(Be_Type_Tbl(MTYPE_U4)) + TY_size(ptr_idx1) 
	   + TY_size(ptr_idx2) + TY_size(ptr_idx3)+ TY_size(gcov_ctr_info_idx),
	   KIND_STRUCT, MTYPE_M, Save_Str ("gcov_info"));
  Set_TY_align(gcov_info_idx, 4);
  Set_TY_fld(gcov_info, first_field);
  return gcov_info_idx;
}

/* Creates a CONSTRUCTOR for a gcov_fn_info. FUNCTION is
   the function being processed and TYPE is the gcov_fn_info
   RECORD_TYPE.  */
static ST *
build_fn_info_value (TY_IDX fn_info_type_idx, unsigned *nfns)
{
  INITV_IDX inv;
  INITV_IDX prev_inv;
  const struct function_list *function;
  unsigned n_fns;
  ST *first_st;
  for (function = functions_head, n_fns = 0; function; function = function->next, 
	 n_fns++) {
    // create new constant
    ST *st = New_ST (GLOBAL_SYMTAB);
    // Create a new symbol 
    char *buf;
    /* Name this symbol: */
    buf = (char *)alloca(32);
    sprintf(buf, "%s%d", ".LPFI", n_fns);
    ST_Init (st, Save_Str(buf), CLASS_VAR
	     , SCLASS_FSTATIC, EXPORT_LOCAL, fn_info_type_idx);
    Set_ST_is_initialized (st);
    Set_ST_is_const_var (st);
    INITO_IDX inito = New_INITO(st);
    unsigned ix;
    /* ident */
    inv = New_INITV ();
    INITV_Init_Integer (inv, MTYPE_U4, function->ident);
    prev_inv = Append_INITV (inv, inito, INITV_IDX_ZERO);

    /* checksum */
    inv = New_INITV ();
    INITV_Init_Integer (inv, MTYPE_U4, function->checksum);
    prev_inv = Append_INITV (inv, inito, prev_inv);

    /* counters */
    for (ix = 0; ix != GCOV_COUNTERS; ix++)
      if (prg_ctr_mask & (1 << ix))
	{
  	  inv = New_INITV ();
	  INITV_Init_Integer (inv, MTYPE_U4, function->n_ctrs[ix]);
	  prev_inv = Append_INITV (inv, inito, prev_inv);
	}

    Allocate_Object(st);
    // Keep a trace on the start of this rodata area
    if (n_fns == 0)
      first_st = st;
  }
  *nfns = n_fns;
  return first_st;
}

/* Creates a CONSTRUCTOR for a gcov_ctr_info. COUNTER is
   the counter being processed and TYPE is the gcov_ctr_info
   TYPE.  */

static INITV_IDX
build_ctr_info_value (INITV_IDX inv, INITO_IDX inito, 
		       INITV_IDX prev_inv, unsigned counter)
{
  //   gcov_unsigned_t num;		/* number of counters.  */
  inv = New_INITV ();
  INITV_Init_Integer (inv, MTYPE_U4, prg_n_ctrs[counter]);
  prev_inv = Append_INITV (inv, inito, prev_inv);

  //   gcov_type *values;		/* their values.  */
  if (prg_n_ctrs[counter])
    {
      inv = New_INITV ();
      INITV_Init_Symoff (inv, ctr_labels[counter], 0, 1);
      prev_inv = Append_INITV (inv, inito, prev_inv);
    } else {
      // Null pointer
      inv = New_INITV ();
      INITV_Init_Integer (inv, MTYPE_U4, 0);
      prev_inv = Append_INITV (inv, inito, prev_inv);
    }

  //   gcov_merge_fn merge;  	/* The function used to merge them.  */
  // Create a Label for the merge function
  inv = New_INITV ();
  INITV_Init_Symoff (inv, ctr_merge_functions_st[counter], 0);
  prev_inv = Append_INITV (inv, inito, prev_inv);
  return prev_inv;
}

// Creates a symbol of type gcov_info_idx. Insert a reference to fn_st
//   for the functions field
static void
build_gcov_info_value (TY_IDX gcov_info_idx, ST *fn_st, unsigned n_fns)
{
  unsigned ix;
  
  // gcov_info_st has already been created with a wrong type.
  // Set the good type, allocate it
  Clear_ST_is_not_sized (gcov_info_st); // => gcov_info_st will be allocated
  Set_ST_type(gcov_info_st, gcov_info_idx);

  Allocate_Object(gcov_info_st);
  
  // With will create a new entry in the inito tab
  INITO_IDX inito = New_INITO(gcov_info_st); 
  INITV_IDX inv;
  INITV_IDX prev_inv;

  /* Version ident */
  inv = New_INITV ();
  INITV_Init_Integer (inv, MTYPE_U4, GCOV_VERSION);
  prev_inv = Append_INITV (inv, inito, INITV_IDX_ZERO);

  /* next -- NULL */
  inv = New_INITV ();
  INITV_Init_Integer (inv, MTYPE_U4, 0);
  prev_inv = Append_INITV (inv, inito, prev_inv);

  /* stamp */
  inv = New_INITV ();
  INITV_Init_Integer (inv, MTYPE_U4, local_tick);
  prev_inv = Append_INITV (inv, inito, prev_inv);

  /* Filename */
  char *filename;
  int filename_len;
  if (cwdname && da_file_name[0] != '/') {
    filename = TYPE_MEM_POOL_ALLOC_N(char,
				     MEM_coverage_pool,
				     strlen(cwdname) + strlen(da_file_name) + 2);
    sprintf(filename,"%s/%s", cwdname, da_file_name);
  } else
    filename = da_file_name;  
  filename_len = strlen (filename);
  TCON tcon = Host_To_Targ_String (MTYPE_STRING,
				   filename,
				   filename_len + 1); 
  ST *const_st = New_Const_Sym(Enter_tcon(tcon), Be_Type_Tbl(TCON_ty(tcon)));
  inv = New_INITV ();
  INITV_Init_Symoff (inv, const_st, 0, 1);
  prev_inv = Append_INITV (inv, inito, prev_inv);
  
  /* number of functions */
  inv = New_INITV ();
  INITV_Init_Integer (inv, MTYPE_U4, n_fns);
  prev_inv = Append_INITV (inv, inito, prev_inv);

  /* fn_info table */
  /* add the fn_info type and initializer.  */
  inv = New_INITV ();
  INITV_Init_Symoff (inv, fn_st, 0, 1);
  prev_inv = Append_INITV (inv, inito, prev_inv);

  /* counter_mask */
  inv = New_INITV ();
  INITV_Init_Integer (inv, MTYPE_U4, prg_ctr_mask);
  prev_inv = Append_INITV (inv, inito, prev_inv);

  /* Build the ctr_info type and initializer.  */
  for (ix = 0; ix != GCOV_COUNTERS; ix++)
    if (prg_ctr_mask & (1 << ix))
      prev_inv = build_ctr_info_value(inv, inito, prev_inv, ix);
}

/* Write out the structure which libgcov uses to locate all the
   counters.  The structures used here must match those defined in
   gcov-io.h.  Write out the constructor to call __gcov_init.  */
static void
create_coverage (TY_IDX counter_type)
{
  unsigned n_fns;

  /* Create the structure gcov_fn_info*/
  TY_IDX gcov_fn_info_idx = build_fn_info_type();

  /* Create the structure gcov_ctr_info*/
  TY_IDX gcov_ctr_info_idx = build_ctr_info_type(counter_type);

  /* Create the struct gcov_info*/
  TY_IDX  gcov_info_idx = build_gcov_info_type(gcov_fn_info_idx, gcov_ctr_info_idx);

  /* Build the fn_info type and initializer.  */
  ST *fn_st = build_fn_info_value (gcov_fn_info_idx, &n_fns);

  // Create a new symbol of type gcov_info
  build_gcov_info_value(gcov_info_idx, fn_st, n_fns);

  // Create constructor section
  ST *init_st = New_ST (GLOBAL_SYMTAB);
  ST_Init (init_st, Save_Str2i ("__ctors_for_cov", "_", 0),
           CLASS_VAR, SCLASS_FSTATIC,
           EXPORT_LOCAL, Make_Pointer_Type(MTYPE_To_TY(MTYPE_V)));
  Set_ST_is_initialized (init_st);
  INITO_IDX inito = New_INITO(init_st);
  INITV_IDX initv = New_INITV ();
  INITV_IDX prev_inv;
  INITV_Init_Symoff (initv, gcov_init_thunk_st, 0, 1);
  prev_inv = Append_INITV (initv, inito, INITV_IDX_ZERO);
  ST_ATTR_IDX st_attr_idx;
  ST_ATTR&    st_attr = New_ST_ATTR (GLOBAL_SYMTAB, st_attr_idx);
  ST_ATTR_Init (st_attr, ST_st_idx (init_st), ST_ATTR_SECTION_NAME,
                Save_Str (".ctors"));
  Set_ST_has_named_section (init_st);
  // to avoid Warning: Assigned section name (.ctors) has ambiguous
  // attributes (both .data and .rodata) when a ctors section has
  // already been created, ignore Set_ST_is_const_var.
  //   Set_ST_is_const_var (init_st);
  Allocate_Object (init_st);

  // Generate the function that calls __gcov_init
//   generate_thunk_for_gcov_init();
}

/* Compute checksum for the current function.  We generate a CRC32.  */
static unsigned
compute_checksum (void)
{
/*   unsigned chksum = DECL_SOURCE_LINE (current_function_decl);
     source line of the current function */
  unsigned chksum = 3;

  FmtAssert ( Tree_WN != NULL, 
	      ("wn_coverage: Compute_Checksum Whirl node is empty! "));

  FmtAssert ( file_table != NULL, 
	      ("wn_coverage: Compute_Checksum file table is empty! "));
  chksum = coverage_checksum_string (chksum,
      				     Cur_PU_Name);

  /* read source pathnames */
  USRCPOS usrcpos;
  file_info *cur_file;
  USRCPOS_srcpos(usrcpos) = WN_Get_Linenum(Tree_WN);
  cur_file = &file_table[USRCPOS_filenum(usrcpos)];
  char *filename = cur_file->filename;

  chksum = coverage_checksum_string (chksum, filename);

  return chksum;
}
// Returns TRUE if edge src->dst is critical critical means that the
// instrumenting counter for edge profiling can be put on the edge (on
// src or dst)
static BOOL
is_critical (basic_block_t *src, basic_block_t *dst)
{
  unsigned  n_succs_src = src->n_succs;
  unsigned  n_preds_dst = dst->n_preds;
  if ((n_preds_dst == 1) || (n_succs_src == 1))
    return FALSE;
  return TRUE;
}

static basic_block_t*
new_bb(MEM_POOL *mem) {
  basic_block_t *BB = TYPE_MEM_POOL_ALLOC_N(basic_block_t,
					    mem,
					    1);
  return BB;
}

/* This functions initializes the edge list. Basically the entire
   flowgraph is processed, and all edges are assigned a number,
   and the data structure is filled in.  */
static edge_t
new_edge() {
  edge_t e = TYPE_MEM_POOL_ALLOC_N(struct edge_def,
				   MEM_coverage_pool_pu,
				   1);
  return e;
}

static int index_last = 0;
// Update list of pred and succ edges for bb1 and bb2
static void 
add_edges_to_bb(basic_block_t *bb1, basic_block_t *bb2, int num_edge, 
		unsigned index_succ)
{
  unsigned index_pred;
  if (!Branch_Probabilities) return;
  // Note First_BB and the first in the CFG has not their N-suucs and
  // n_preds counter up to date.
  // compute n_succs of bb1 and n_preds of bb2
  unsigned bb1n_succs = bb1->n_succs;
  unsigned bb2n_preds = bb2->n_preds;
  bb1n_succs += (bb1 == First_BB)? 1 : 0;
  bb2n_preds += (bb1 == First_BB)? 1 : 0;
  bb1n_succs +=  (bb1->n_succs == 0) || (bb1->is_exit)||(bb1->is_a_call) ? 1 : 0;

  //  Create List of successor of bb1
  if (bb1->index_of_succ_edges == NULL)
    bb1->index_of_succ_edges = TYPE_MEM_POOL_ALLOC_N(int,
						     MEM_coverage_pool_pu,
						     bb1n_succs);
  //bb->n_succs + 1 because some bb as a fake successor edge to exit

  // Update List of successor of bb1
  FmtAssert ( bb1 == First_BB || (index_succ < bb1n_succs), 
	      ("Create_Edge_List: wrong index_succ "));
  bb1->index_of_succ_edges[index_succ] = num_edge;
  

  // Update list of predecessor of bb2. Index is the first
  // position where index_of_pred_edges[index_pred] !=0
  if (bb2->index_of_pred_edges == NULL)
    bb2->index_of_pred_edges =  TYPE_MEM_POOL_ALLOC_N(int,
						      MEM_coverage_pool_pu,
						      bb2n_preds );
  if (bb2 != Last_BB) {
    for (index_pred = 0; 
	 (index_pred < bb2n_preds);
	 index_pred++) 
      if (bb2->index_of_pred_edges[index_pred] == 0) break;
    FmtAssert ( bb1 == First_BB || ((index_pred < bb2n_preds) && 
		bb2->index_of_pred_edges[index_pred] == 0), 
		("Create_Edge_List: wrong index_pred "));
  } else {
    // For Last_bb n_preds is not updated.
    // We need to keep track of the index for this list.
    index_pred = index_last++;
  }
  bb2->index_of_pred_edges[index_pred] = num_edge;
}

//Create list of all the edges of the CFG
// Update for each BB the list of pred and succ edges
static struct edge_list *
create_edge_list (void)
{
  struct edge_list *elist;
  edge_t e;
  int num_edges, pred_of_exit;

  num_edges = 0, pred_of_exit =0 ;

  /* Determine the number of edges in the flow graph by counting successor
     edges on each basic block.  */
  basic_block_list_t *list_bb;
  for ( list_bb = Cur_Function->bb_list; list_bb != NULL; 
	list_bb = list_bb->next) { 
    basic_block_t *bb = list_bb->bb;
    num_edges += bb->n_succs;
    if ((bb->is_a_call) || (bb->n_succs == 0) || bb->is_exit) 
      {num_edges++; pred_of_exit++;}
  }
  num_edges++; // +1 for edge between entry bb and the first in the cflow
  elist = TYPE_MEM_POOL_ALLOC_N(struct edge_list,
				MEM_coverage_pool_pu,
				1);
  elist->num_edges = num_edges;
  elist->index_to_edge = TYPE_MEM_POOL_ALLOC_N(edge_t,
					       MEM_coverage_pool_pu,
					       num_edges);
  if (Branch_Probabilities) {
    // Create predecessor list of last_bb
    FmtAssert ( (Last_BB->index_of_pred_edges == NULL), 
		("Create_Edge_List: Last_BB has already some predecessor"));
    Last_BB->index_of_pred_edges =  TYPE_MEM_POOL_ALLOC_N(int,
							   MEM_coverage_pool_pu,
							   pred_of_exit);
    // Create successor list of First_bb
    FmtAssert ( (First_BB->index_of_succ_edges == NULL), 
		("Create_Edge_List: First_BB has already some successors"));
    // Only one successor for the entry block = the entry function.
    // Function with severals entries is not handle for the moment.
    First_BB->index_of_succ_edges =  TYPE_MEM_POOL_ALLOC_N(int,
							   MEM_coverage_pool_pu,
							   1 );
  }
  num_edges = 0;

  // Create an edge between gcov entry bb and the first in the cflow
  e = new_edge();
  e->src = First_BB;
  e->dest = Cur_Function->bb_list->bb;
  e->from_entry = 1;
  e->fall_through = 1;
  add_edges_to_bb(e->src, e->dest, num_edges, 0);
  elist->index_to_edge[num_edges++] = e;
  /* Follow successors of blocks, and register these edges.  */
  for ( list_bb = Cur_Function->bb_list; list_bb != NULL; 
	list_bb = list_bb->next) { 
    basic_block_t *bb = list_bb->bb;
    basic_block_list_t *list_bb_succ;
    unsigned index_succ = 0;
    for ( list_bb_succ = bb->succs; list_bb_succ != NULL; 
	  list_bb_succ = list_bb_succ->next) {
      basic_block_t *succ = list_bb_succ->bb;
      e = new_edge();
      e->src = bb;
      e->dest = succ;
      // Add fall thru property multiple branchement (XGOTO or
      // COMPGOTO) does not fall thru the next BB
      if (bb->n_succs < 3 && list_bb->next && list_bb->next->bb == succ)
	e->fall_through = 1;
      if (is_critical(bb, succ)) 
	e->critical = 1;
      // Add some extra info needed for graph propagation
      add_edges_to_bb(bb, succ, num_edges, index_succ++);
      elist->index_to_edge[num_edges++] = e;
    }
    // if bb is a call, add a fake edge to exit
    if (bb->is_a_call) {
      e = new_edge();
      e->src = bb;
      e->dest = Last_BB;
      e->to_exit = 1;
      e->fake = 1;
#if instrument_all_edges == 1
	e->on_tree = 1;
#endif
      // Add some extra info needed for graph propagation
      FmtAssert ( (index_succ == bb->n_succs), 
		  ("Create_Edge_List: wrong last index_succ for fake edge"));
      add_edges_to_bb(e->src, e->dest, num_edges, index_succ++);
      elist->index_to_edge[num_edges++] = e;
    } else if (bb->is_exit) {
      e = new_edge();
      e->src = bb;
      e->dest = Last_BB;
      e->to_exit = 1;
      e->fall_through = 1;
      // TB: fix inexact info in file dumped by gcov 
      //      e->fake = 1;
#if instrument_all_edges == 1
      e->on_tree = 1;
#endif
      add_edges_to_bb(e->src, e->dest, num_edges, index_succ++);
      elist->index_to_edge[num_edges++] = e;
    } else if (bb->n_succs == 0)  {
      /* Add a fake edge between any block which has no
	 successors, and the exit block. Some data flow equations require these
	 edges to exist.*/  
      e = new_edge();
      e->src = bb;
      e->dest = Last_BB;
      e->to_exit = 1;
      e->fake = 1;
#if instrument_all_edges == 1
      e->on_tree = 1;
#endif
      add_edges_to_bb(e->src, e->dest, num_edges, index_succ++);
      elist->index_to_edge[num_edges++] = e;
    }
  }

  return elist;
}

/* This function free's memory associated with an edge list.  */

void
free_edge_list (struct edge_list *elist)
{
  if (elist)
    {
      free (elist->index_to_edge);
      free (elist);
    }
}

#if 0
/* This function provides debug output showing an edge list.  */

void
print_edge_list (FILE *f, struct edge_list *elist)
{
  int x;

  fprintf (f, "Compressed edge list %d edges\n",
	    elist->num_edges);

  for (x = 0; x < elist->num_edges; x++)
    {
      fprintf (f, " %-4d - edge(", x);
      if (INDEX_EDGE_PRED_BB (elist, x) == REGION_First_BB)
	fprintf (f, "entry,");
      else
	fprintf (f, "%d,", INDEX_EDGE_PRED_BB (elist, x)->id);

//       if (INDEX_EDGE_SUCC_BB (elist, x) == EXIT_BLOCK_PTR)
// 	fprintf (f, "exit)\n");
//       else
	fprintf (f, "%d)\n", INDEX_EDGE_SUCC_BB (elist, x)->id);
    }
}
#endif

static BOOL 
Change_COMPGOTO_Entries(WN *wn_compgoto,	/* WN for the switch table */
			LABEL_IDX old_label,	/* the old target label */
			LABEL_IDX new_label)	/* the new target label */
{
  Is_True( WN_opcode(wn_compgoto) == OPC_COMPGOTO,
	   ( "Change_COMPGOTO_Entries:"
	     " OPC_COMPGOTO expected as entry." ));
  BOOL res = FALSE;
  if (WN_kid_count(wn_compgoto) == 3) { /* default case */
    WN *def  = WN_kid2(wn_compgoto);
    Is_True( WN_opcode(def) == OPC_GOTO,
	     ( "Change_COMPGOTO_Entries:"
	       " OPC_GOTO expected." ));
    if ((LABEL_IDX)WN_label_number(def) == old_label) {
      res = TRUE;
      WN_label_number(def) = new_label;
    }
  } /* fall thru */

  WN *gotoTable = WN_kid1(wn_compgoto);
  INT32 num_entries = WN_num_entries(wn_compgoto); // without default cases
  WN *wn = WN_first(gotoTable);	/* first goto */
  for (int i = 0; i < num_entries; i++) {
    Is_True( WN_opcode(wn) == OPC_GOTO,
	     ( "Change_COMPGOTO_Entries:"
	       " OPC_GOTO expected." ));
    LABEL_IDX idx;
    idx = WN_label_number(wn);
    if ((LABEL_IDX)WN_label_number(wn) == old_label) {
      WN_label_number(wn) = new_label;
      res = TRUE;
    }
    wn = WN_next(wn);
  }
  return res;
}


// Return fall thru edge between bb and previous bb 
static edge_t 
get_fall_thru_edge_to(basic_block_t *bb)
{
  for (int x = 0; x < edges_list->num_edges; x++) {
    edge_t e = edges_list->index_to_edge[x];
    if (e->dest == bb && e->fall_through){
      FmtAssert(e->src->index + 1 == e->dest->index, 
		("get_fall_thru_edge_to: find a fall thru edge not between bb and previous bb! "));
      return e;
    }
  }
  return NULL;
}

// Add code for edge src->dest. num_edge is the identifier of the edge
// If src is null, insert code at start of dst
static void
insert_insn_on_edge (edge_t e,unsigned num_edge, WN *block)
{
  basic_block_t *src = e->src;
  basic_block_t *dst = e->dest;
  unsigned  n_succs_src = src->n_succs;
  unsigned  n_preds_dst =  dst->n_preds;
  // Figure out where to put these things.  
  //If the destination has one predecessor, insert there.
#ifdef DEBUG_COVERAGE
  fprintf(TraceCovFile,"func %d: instrumenting edge between BB%d and BB%d, counter %d\n", current_function_funcdef_no, src->index, dst->index, num_edge);
#endif
  char str[1024];
  sprintf(str,"Coverage: edge %d between BB%d and BB%d",
	  num_edge, src->index, dst->index);
  WN *edge_profile = NULL;;
  if (src == First_BB || n_preds_dst == 1)
    {
      // insert edge_profile at start of dst
      // Generate code to increment this edge
      sprintf(str,"%s code inserted in dest BB%d",
	      str, dst->index);
      edge_profile = gen_edge_profiler (num_edge);
      if (!dst->is_a_label){
	WN_copy_linenum(dst->wn_list->wn, edge_profile);
	WN_INSERT_BlockBefore(block, dst->wn_list->wn, edge_profile);
      }
      else if (src == First_BB) {
	// Case of Pragma PREAMBLE_END: insert inst after the pragma
	// Locate end of preamble
	wnode_list_t *lwn = dst->wn_list;
	WN *preamble_end = lwn->wn;
	while ( preamble_end
		&& ! ( WN_operator(preamble_end) == OPR_PRAGMA &&
		       WN_pragma(preamble_end) == WN_PRAGMA_PREAMBLE_END) ) {
	  lwn = lwn->next;
	  preamble_end = lwn->wn;
	}
	Is_True( preamble_end != NULL,
	       ( "FB_CFG::Walk_WN_statement found no PREAMBLE_END PRAGMA" ) );
	
	WN_copy_linenum(preamble_end, edge_profile);
	WN_INSERT_BlockAfter(block, preamble_end, edge_profile);
      } else {
	FmtAssert(dst->wn_list->next == NULL || WN_opcode(dst->wn_list->next->wn) != OPC_LABEL, 
		  ("gcov coverage: A label follows a label! "));
	WN_copy_linenum(dst->wn_list->wn, edge_profile);
	WN_INSERT_BlockAfter(block, dst->wn_list->wn, edge_profile);
      }
    }
  // If the source has one successor insert there.  
  else if (n_succs_src == 1)
    {
      sprintf(str,"%s code inserted in src BB%d",
	      str, src->index);
      // Generate code to increment this edge
      edge_profile = gen_edge_profiler (num_edge);
      // Insert edge profile at start of src
      if (!src->is_a_label) {
	WN_copy_linenum(src->wn_list->wn, edge_profile);
	WN_INSERT_BlockBefore(block, src->wn_list->wn, edge_profile);
      }
      else{
	FmtAssert(src->wn_list->next == NULL || WN_opcode(src->wn_list->next->wn) != OPC_LABEL, 
		  ("gcov coverage: A label follows a label! "));
	WN_copy_linenum(src->wn_list->wn, edge_profile);
 	WN_INSERT_BlockAfter(block, src->wn_list->wn, edge_profile);
      }
    }
  // edge is critical
  else
    {
      if (src->n_succs > 2) {
	// Case of igoto with more than 2 successors 
	// Here we need to split the edge to add the code for
	// the counter. 
	sprintf(str,"%s case of a switch code inserted in a new BB",
		str);
	// Create a WN that contain a LABEL
	LABEL_IDX label;
	(void) New_LABEL (CURRENT_SYMTAB, label);
	WN *wn_label = WN_CreateLabel (label, 0, NULL);
	// insert edge_profile at start of dst
	WN_copy_linenum(dst->wn_list->wn, wn_label);
	WN_INSERT_BlockBefore(block, dst->wn_list->wn, wn_label);
	// Insert increment code after the new label
	edge_profile = gen_edge_profiler (num_edge);
	WN_copy_linenum(dst->wn_list->wn, edge_profile);
	WN_INSERT_BlockAfter(block, wn_label, edge_profile);
	// Patch the switch table
	if (Change_COMPGOTO_Entries(src->last_wn,	/* WN for the switch table */
				    WN_label_number(dst->wn_list->wn),	/* the old target label */
				    label)	/* the new target label */
	    == FALSE)
	  FmtAssert(FALSE, 
		    ("Change_COMPGOTO_EntriesL cannot find label %d in COMPGOTO", WN_label_number(dst->wn_list->wn)));
	// Update the fall thru edge between dst and previous dst
	// (index = dst -1) if exists. Otherwise the counter is
	// exected also for the fall thru edge
	edge_t e = get_fall_thru_edge_to(dst);
	if (e) {
	  WN *branch = WN_CreateGoto((ST_IDX) 0, WN_label_number(dst->wn_list->wn));
	  WN_copy_linenum(e->src->last_wn, edge_profile);
	  WN_INSERT_BlockAfter(block, e->src->last_wn, branch);
	}

      } else {
	sprintf(str,"%s edge is critical: conditional counter added at end of BB%d",
		str, src->index);
	// Generate conditional code to increment this edge
	edge_profile = gen_critical_edge_profiler (num_edge, src, dst);
	// insert code before the branch
	WN *edge_profile0 = WN_kid0(edge_profile);
	WN_copy_linenum(src->last_wn, edge_profile);
	WN_INSERT_BlockBefore(block, src->last_wn, edge_profile);
	// Edge profile is a block, keep trace of the first kid to
	// insert a comment after.
	edge_profile = edge_profile0;
      }
    }
  // Add a comment
  WN *comment = WN_CreateComment(str);
  WN_copy_linenum(edge_profile, comment);
  WN_INSERT_BlockBefore(block, edge_profile, comment);
}
/* Add edge instrumentation code to the entire insn chain.
 */
static unsigned
instrument_edges (struct edge_list *el, WN *block)
{
  unsigned num_instr_edges = 0;
  int x;
  
  for (x = 0; x < el->num_edges; x++) {
    edge_t e = el->index_to_edge[x];
    if (!e->fake && !e->on_tree)
      insert_insn_on_edge (e, num_instr_edges++, block);
  }
  return num_instr_edges;
}

/* We use union_groups[bb_id]for standard union-find algorithm.  */
static unsigned *bb_groups;

/* Union find algorithm implementation for the basic blocks using
   aux fields.  */

static unsigned
find_group (unsigned bb_id)
{
  unsigned group_id = bb_id, bb1_id;

  while ( bb_groups[group_id] != group_id)
    group_id = bb_groups[group_id];

  /* Compress path.  */
  while (bb_groups[bb_id] != group_id)
    {
      bb1_id = bb_groups[bb_id];
      bb_groups[bb_id] =  group_id;
      bb_id = bb1_id;
    }
  return group_id;
}

static void
union_groups (unsigned bb1_id, unsigned bb2_id)
{
  unsigned bb1g_id = find_group (bb1_id);
  unsigned bb2g_id = find_group (bb2_id);

  /* ??? I don't have a place for the rank field.  OK.  Lets go w/o it,
     this code is unlikely going to be performance problem anyway.  */
  if (bb1g_id == bb2g_id)
    abort ();

  bb_groups[bb1g_id] = bb2g_id;
}

/* This function searches all of the edges in the program flow graph, and puts
   as many bad edges as possible onto the spanning tree.  Bad edges include
   abnormals edges, which can't be instrumented at the moment.  Since it is
   possible for fake edges to form a cycle, we will have to develop some
   better way in the future.  Also put critical edges to the tree, since they
   are more expensive to instrument.  */

static void
find_spanning_tree (struct edge_list *el)
{
  int i;
  int num_edges = el->num_edges;
  basic_block_t *bb;

  bb_groups = (unsigned *) alloca ((Cur_Function->bb_count + 2)* sizeof(unsigned));

  // First, each bb belongs to its group
  bb_groups[LAST_BB_TO_GCOV_INDEX] = LAST_BB_TO_GCOV_INDEX;
  bb_groups[FIRST_BB_TO_GCOV_INDEX] = FIRST_BB_TO_GCOV_INDEX;
  basic_block_list_t *list_bb;
  for ( list_bb = Cur_Function->bb_list; list_bb != NULL; 
    list_bb = list_bb->next) {
    bb = list_bb->bb;
    bb_groups[bb->index] = bb->index;
  }

  /* Add fake edge exit to entry we can't instrument.  */
  union_groups (LAST_BB_TO_GCOV_INDEX, FIRST_BB_TO_GCOV_INDEX);

  /* First add all abnormal edges to the tree unless they form a cycle. Also
     add all edges to EXIT_BLOCK_PTR to avoid inserting profiling code behind
     setting return value from function.  */
  for (i = 0; i < num_edges; i++)
    {
      edge_t e = INDEX_EDGE (el, i);
      unsigned src_id =  BB_id(e->src) ;
      unsigned dest_id = BB_id(e->dest);
      if ( (e->fake || e->to_exit)
	   && (find_group (src_id) != find_group (dest_id)))
	{
// 	    if (e->fake) 
// 	      fprintf (TraceCovFile, "Fake edge %d to %d put to tree\n",
// 		       src_id, dest_id); 
// 	    else if (e->to_exit) {
// 	      fprintf (TraceCovFile, "Edge to exit %d to %d put to tree\n",
// 		       src_id, dest_id); }
	  e->on_tree = 1;
	  union_groups (src_id, dest_id);
	}
    }

  /* Now insert all critical edges to the tree unless they form a cycle.  */
  for (i = 0; i < num_edges; i++)
    {
      edge_t e = INDEX_EDGE (el, i);
      unsigned src_id =  BB_id(e->src) ;
      unsigned dest_id = BB_id(e->dest) ;
      if (e->critical
	  && find_group (src_id) != find_group (dest_id))
	{
// 	  fprintf (TraceCovFile, "Critical edge %d to %d put to tree\n",
// 		   src_id, dest_id); 
	  e->on_tree = 1;
	  union_groups (src_id, dest_id);
	}
    }
  
  /* And now the rest.  */
  for (i = 0; i < num_edges; i++)
    {
      edge_t e = INDEX_EDGE (el, i);
      unsigned src_id =  BB_id(e->src);
      unsigned dest_id = BB_id(e->dest);
//       fprintf (TraceCovFile, "Normal edge %d [%d] to %d [%d]\n",
// 	       src_id, find_group (src_id), dest_id, find_group (dest_id)); 
      if (find_group (src_id) != find_group (dest_id))
	{
//  	  fprintf (TraceCovFile, "\tput to tree\n");
	  e->on_tree = 1;
	  union_groups (src_id, dest_id);
// 	  fprintf (TraceCovFile, "New group %d = [%d], %d = [%d]\n",
// 		   src_id, find_group (src_id), dest_id, find_group (dest_id)); 
	}
    }

  bb_groups = NULL;
}

/*========================================================================

   Get source file path name from DST (taken from cgdwarf.c)
 
 ========================================================================*/


static void 
Gen_File_Table ()
{
   /* Generate a table of the include directory names and a table
    * of the file names, based on the DST (Debugging Symbol Table).
    * The USRCPOS_filenum() is a positional index into the list of
    * file names as given by DST_get_file_names(), starting at 
    * index==1.  Similarly, the DST_FILE_NAME_dir(file_info) is a
    * positional index into the list of directory names as given by
    * DST_get_dir_names().
    * call ir_print_filename to optionally print all pathnames in the DST info
    */
  INT count;
  DST_IDX idx;
  DST_INCLUDE_DIR *incl;
  char *name;
  INT incl_table_size;
  INT new_size;
  DST_FILE_NAME *file;
  INT file_table_size;

  if (file_table_generated && file_table != NULL) {
    return;
  }

  incl_table_size = 0;
  incl_table = NULL;
  file_table = NULL;
  count = 1;
  for (idx = DST_get_include_dirs (); 
       !DST_IS_NULL(idx); 
       idx = DST_INCLUDE_DIR_next(incl))
    {
      incl = DST_DIR_IDX_TO_PTR (idx);
      name = DST_STR_IDX_TO_PTR (DST_INCLUDE_DIR_path(incl));
      if (count >= incl_table_size) {
        new_size = count + 10;
        if (incl_table == NULL)
	  incl_table = TYPE_MEM_POOL_ALLOC_N(char *,
					     MEM_coverage_pool,
					     new_size);
        else 
	  incl_table = TYPE_MEM_POOL_REALLOC_N(char *, MEM_coverage_pool, 
					       incl_table,
					       count,
					       new_size);


        if (incl_table == NULL) 
          fprintf(stderr, "Gen_File_Table: Run out of memory\n");
        incl_table_size = new_size;
      }
      incl_table[count] = name;
      count++;
    }
  
  file_table_size = 0;
  file_table = NULL;
  count = 1;
  for (idx = DST_get_file_names (); 
       !DST_IS_NULL(idx); 
       idx = DST_FILE_NAME_next(file))
    {
      file = DST_FILE_IDX_TO_PTR (idx);
      if (DST_IS_NULL(DST_FILE_NAME_name(file))) {
        name = "NULLNAME";
      }
      else {
        name = DST_STR_IDX_TO_PTR (DST_FILE_NAME_name(file));
      }
      if (count >= file_table_size) {
        new_size = count + 10;
        if (file_table == NULL)
	  file_table = TYPE_MEM_POOL_ALLOC_N(file_info,
					     MEM_coverage_pool,
					     new_size);
        else 
	  file_table = TYPE_MEM_POOL_REALLOC_N(file_info, MEM_coverage_pool, 
					       file_table,
					       count,
					       new_size);
        if (file_table == NULL) 
          fprintf(stderr, "Gen_File_Table: Run out of memory\n");
        file_table_size = new_size;
      }
      file_table[count].filename = name;
      file_table[count].incl_index = DST_FILE_NAME_dir(file);
      count++;
    }
  file_table_generated = TRUE;
} /* Gen_File_Table */




/* Begin output to the graph file for the current function.
   Opens the output file, if not already done. Writes the
   function header, if not already done. Returns nonzero if data
   should be output.  
   pu: only to get name of current PU
   bb: only to get filename where pu is defined
*/
static int
coverage_begin_output (WN *wn)
{
  if (!bbg_function_announced)
    {
      ST *pu = WN_st(wn); // Get_Current_PU_ST()
      const char *file;
      const char *path; 
      char *complete_name;
      unsigned line;
      unsigned long offset;
      
      /* Reset last_srcpos for new function */
      last_srcpos = 0;

      /* read source pathnames */
      USRCPOS usrcpos;
      file_info *cur_file;
      USRCPOS_srcpos(usrcpos) = WN_Get_Linenum(wn);

      cur_file = &file_table[USRCPOS_filenum(usrcpos)];
      path = incl_table[cur_file->incl_index];
      file = cur_file->filename;
      line = USRCPOS_linenum(usrcpos);

      // Write general info in gcno file: function filename, function
      // name, line...
      
      if (!bbg_file_opened)
	{
	  if (!gcov_open (bbg_file_name, -1)) {
	    fprintf (stderr, "cannot open %s\n", bbg_file_name);
	  }
	  else
	    {
	      gcov_write_unsigned (GCOV_NOTE_MAGIC);
	      gcov_write_unsigned (GCOV_VERSION);
	      gcov_write_unsigned (local_tick);
	    }
	  bbg_file_opened = 1;
	}

      /* Announce function */
      offset = gcov_write_tag (GCOV_TAG_FUNCTION);
      /* Write unique identificateur for the function  */
      gcov_write_unsigned ( current_function_funcdef_no);
      gcov_write_unsigned (compute_checksum ());
      gcov_write_string (ST_name(pu));

      complete_name = (char *)alloca(strlen (path) + strlen (file) + 2);
      strcpy (complete_name, path);
      strcat (complete_name, "/");
      strcat (complete_name, file);

      gcov_write_string (complete_name);
      gcov_write_unsigned (line);
      gcov_write_length (offset);

      bbg_function_announced = 1;
    }
  return !gcov_is_error ();
}

/* Finish coverage data for the current function. Verify no output
   error has occurred.  Save function coverage counts.  */

static void
coverage_end_function (void)
{
  unsigned i;

  if (bbg_file_opened > 1 && gcov_is_error ())
    {
      fprintf(stderr, "error writing `%s'", bbg_file_name);
      bbg_file_opened = 0xFFFFFFFF;
    }

  if (fn_ctr_mask)
    {
      struct function_list *item;

      item = TYPE_MEM_POOL_ALLOC_N(struct function_list,
					   MEM_coverage_pool,
					   sizeof (struct function_list));
      *functions_tail = item;
      functions_tail = &item->next;

      item->next = 0;
      /* Write unique identificateur for the function  */
      item->ident = current_function_funcdef_no;
      item->checksum = compute_checksum ();
      for (i = 0; i != GCOV_COUNTERS; i++)
	{
	  item->n_ctrs[i] = fn_n_ctrs[i];
	  prg_n_ctrs[i] += fn_n_ctrs[i];
	  fn_n_ctrs[i] = fn_b_ctrs[i] = 0;
	}
      prg_ctr_mask |= fn_ctr_mask;
      fn_ctr_mask = 0;
    }
  bbg_function_announced = 0;
}

/* 
   This function writes basic block part of '.gcno' file for the
current function.
*/
static void
coverage_blocks ()
{
  gcov_position_t offset;
  unsigned i;
  offset = gcov_write_tag (GCOV_TAG_BLOCKS);
  // BBs are expected to be sequentially numbered
  for (i = FIRST_BB_TO_GCOV_INDEX; i <= LAST_BB_TO_GCOV_INDEX; i++)
    gcov_write_unsigned (0);
  gcov_write_length (offset);
}

/*
 * Return arc (bb->succ) gcov properties
 */
static unsigned
gcov_arc_flags(edge_t e) {
  unsigned flag_bits = 0;
  if (e->fake)
    flag_bits |= GCOV_ARC_FAKE;
  if (e->fall_through)
    flag_bits |= GCOV_ARC_FALLTHROUGH;
  if (e->on_tree)
    flag_bits |= GCOV_ARC_ON_TREE;

  return flag_bits;
}

/* For each BB in the CFG, create one gcov EDGE 
 * for each successor. Returns number of instrumented edges
 */
static unsigned 
coverage_arcs (struct edge_list *edges_list) {
  gcov_position_t offset = 0;
  unsigned nb_instr = 0 ;
  int x;
  unsigned flag_bits;
  basic_block_t *last_src = NULL;

  for (x = 0; x < edges_list->num_edges; x++) {
    edge_t e = edges_list->index_to_edge[x];
#if 0
    if (!e->fake && !e->on_tree)
      fprintf(TraceCovFile,"edge %d->%d has counter number %d\n",BB_TO_GCOV_INDEX (e->src), 
	      BB_TO_GCOV_INDEX (e->dest), nb_instr);
    else
      fprintf(TraceCovFile,"edge %d->%d has no counter\n", BB_TO_GCOV_INDEX (e->src), 
	      BB_TO_GCOV_INDEX (e->dest) );
#endif
    // Compte nb of instrumented edges
    if (!e->fake && !e->on_tree)
      nb_instr++;
    if (last_src == NULL || e->src != last_src) {
      // Write previous offset
      if (offset)
	gcov_write_length (offset);

      offset = gcov_write_tag (GCOV_TAG_ARCS);
      if (e->from_entry)
	gcov_write_unsigned (FIRST_BB_TO_GCOV_INDEX);
      else
	gcov_write_unsigned (BB_TO_GCOV_INDEX (e->src));
    }
    flag_bits = gcov_arc_flags(e);
    if (e->to_exit) 
      gcov_write_unsigned (LAST_BB_TO_GCOV_INDEX);
    else
      gcov_write_unsigned (BB_TO_GCOV_INDEX (e->dest));
    gcov_write_unsigned (flag_bits);
    last_src = e->src;
  }
  if (offset)
    gcov_write_length (offset);
  return nb_instr;
}


/* for a given BB in the CFG, add gcov line info 
 */
static void
coverage_lines (basic_block_t *bb) {
  gcov_position_t offset;
  SRCPOS srcpos;
  USRCPOS usrcpos;

  offset = 0;

#if 0
  // For all wn in the bb, add line info
  wnode_list_t *list_wn;
  for ( list_wn = bb->wn_list; list_wn != NULL; 
	list_wn = list_wn->next) 
    {
      const char *file;
      const char *path; 
      unsigned line;
      WN *wn = list_wn->wn;
      USRCPOS usrcpos;
      file_info *cur_file;
      USRCPOS_srcpos(usrcpos) = WN_Get_Linenum(wn);
      line = USRCPOS_linenum(usrcpos);
      if (line == 0) 
	{
	  continue;
	}

      cur_file = &file_table[USRCPOS_filenum(usrcpos)];
      path = incl_table[cur_file->incl_index];
      file = cur_file->filename;

      fprintf(TraceCovFile,"coverage_line: %s/%s:%d\n", path, file, line);
    }
#endif
  wnode_list_t *list_wn;
  for ( list_wn = bb->wn_list; list_wn != NULL; 
	list_wn = list_wn->next) {
    srcpos = WN_Get_Linenum(list_wn->wn);
    USRCPOS_srcpos(usrcpos) = srcpos;
    unsigned line;
    line = USRCPOS_linenum(usrcpos);
    if (line == 0) 
      {
	continue;
      }
    // last_srcpos is remanent within a function, put to 0 for a new
    // function
    // If the position is different from last one.
    if (last_srcpos != srcpos) {
      if (!offset) {
	offset = gcov_write_tag (GCOV_TAG_LINES);
	gcov_write_unsigned (BB_TO_GCOV_INDEX (bb));
      }
      USRCPOS last_usrcpos;
      if (last_srcpos != 0) {
	USRCPOS_srcpos(last_usrcpos) = last_srcpos;
      }
      /* If this is a new source file, then output the
	 file's name to the .bb file.  */
      if ( last_srcpos == 0 || 
	   USRCPOS_filenum(last_usrcpos) != USRCPOS_filenum(usrcpos)) {
	char const *file_name;
	char const *path;
	char *complete_name;
	file_info *cur_file;
	cur_file = &file_table[USRCPOS_filenum(usrcpos)];
	path = incl_table[cur_file->incl_index];
	file_name = cur_file->filename;
	complete_name = (char *)alloca(strlen (path) + strlen (file_name) + 2);
	strcpy (complete_name, path);
	strcat (complete_name, "/");
	strcat (complete_name, file_name);
	gcov_write_unsigned (0);
	gcov_write_string (complete_name);
      }
      gcov_write_unsigned (line);
      last_srcpos = srcpos;
    }
  }
  if (offset)
    {
      /* A file of NULL indicates the end of run.  */
      gcov_write_unsigned (0);
      gcov_write_string (NULL);
      gcov_write_length (offset);
    }
}

/* P is a string that will be used in a symbol.  Mask out any characters
   that are not valid in that context.  */
    
static void
clean_symbol_name (char *p)
{
  for (; *p; p++)
    if ( !((*p >= 65 && *p <= 90 ) || (*p >= 97 && *p <= 122 )))
      *p = '_';
}

static char *
get_file_function_name_long (const char *first_global_object_name, const char *type)
{
  char *buf;
  const char *p;
  char *q;
  
  if (first_global_object_name)
    p = first_global_object_name;
  else {
    /* We don't have anything that we know to be unique to this translation
       unit, so use what we do have and throw in some randomness.  */
    
    const char *name = "";
    const char *file = Src_File_Name;
  
    q = (char *) alloca (1 + strlen (name) + strlen (file));
    
    sprintf (q, "%s%s", name, file);
    p = q;

  }

  buf = TYPE_MEM_POOL_ALLOC_N(char,
			      MEM_coverage_pool,
			      sizeof (FILE_FUNCTION_FORMAT) + strlen (p)
			      + strlen (type));

  /* Set up the name of the file-level functions we may need.
     Use a global object (which is already required to be unique over
     the program) rather than the file name (which imposes extra
     constraints).  */
  sprintf (buf, FILE_FUNCTION_FORMAT, type, p);

  /* Don't need to pull weird characters out of global names.  */
  clean_symbol_name (buf + 11);

  return buf;
}

/* Allocate NUM counters of type COUNTER. Returns nonzero if the
   allocation succeeded.  */
static int
coverage_counter_alloc (unsigned counter, unsigned num)
{
  if (!num)
    return 1;

  if (!ctr_labels[counter])
    {
      unsigned ix;
      /* Create a new symbol to access the counter.  */
      char *buf;
      ST *newst;
      /* Name this symbol: */
      buf = (char *)alloca(32);
      sprintf(buf, "%s%d", ".LPBX", counter + 1);

      // add a st for instrumentation with a dummy type 
      // the real symbol will be allocated at end of file
      newst = New_ST(GLOBAL_SYMTAB);
      ST_Init(newst, 
	      Save_Str(buf),
	      CLASS_VAR, SCLASS_FSTATIC, EXPORT_LOCAL_INTERNAL,  
	      Make_Pointer_Type(MTYPE_To_TY(MTYPE_U4)));
      // To tell allocate_object not to allocate it (whirl2ops call
      // allocate object for all used objects!)
      Set_ST_is_not_sized(newst);
      ctr_labels[counter] = newst;
      // Create an intrinsic symbol for each merge function
      for (ix = 0; ix != GCOV_COUNTERS; ix++) {
	TY_IDX  ty = Make_Function_Type( MTYPE_To_TY( MTYPE_V ) );
	ctr_merge_functions_st[ix] = Gen_Intrinsic_Function( ty, (char*)ctr_merge_functions[ix] );
	Clear_PU_no_side_effects( Pu_Table[ST_pu( ctr_merge_functions_st[ix] )] );
	Clear_PU_is_pure( Pu_Table[ST_pu( ctr_merge_functions_st[ix] )] );
	Set_PU_no_delete( Pu_Table[ST_pu( ctr_merge_functions_st[ix] )] );
	Set_PU_no_instrument_function(Pu_Table[ST_pu( ctr_merge_functions_st[ix] )]);
      }
    }
  fn_b_ctrs[counter] = fn_n_ctrs[counter];
  fn_n_ctrs[counter] += num;
  fn_ctr_mask |= 1 << counter;
  return 1;
}

/* Output instructions  to increment the edge execution count.  */
static WN *
gen_edge_profiler (unsigned no )
{
  WN *wn;
  FmtAssert(no < fn_n_ctrs[GCOV_COUNTER_ARCS] - fn_b_ctrs[GCOV_COUNTER_ARCS], 
	    ("gen_edge_profiler: Edge counter %d bigger than total number of edges %d", 
	     no, fn_n_ctrs[GCOV_COUNTER_ARCS] - fn_b_ctrs[GCOV_COUNTER_ARCS]));

  no += prg_n_ctrs[GCOV_COUNTER_ARCS] + fn_b_ctrs[GCOV_COUNTER_ARCS];

#ifdef DEBUG_COVERAGE
  fprintf(TraceCovFile,"\t gen_edge_profiler:counter %d\n", no);
#endif
  if (Coverage_Counter64) {
    /* Generate a WN to access COUNTER NO.  */
    // Load counter
    WN *wnld = WN_CreateLdid(OPC_U8U8LDID, no * (MTYPE_RegisterSize( MTYPE_U8 )), 
			     ctr_labels[GCOV_COUNTER_ARCS],
			     Be_Type_Tbl(MTYPE_U8));
    /* increment +1 the counter */
    WN *wnadd = WN_Add(MTYPE_U8,
		       wnld,
		       WN_Intconst(MTYPE_U8, 1));
    // Store the results in mem
    wn = WN_CreateStid (OPC_U8STID, (MTYPE_RegisterSize( MTYPE_U8 )) * no,
			ctr_labels[GCOV_COUNTER_ARCS] , 
			Be_Type_Tbl(MTYPE_U8),wnadd);
  } else {
    /* Generate a WN to access COUNTER NO.  */
    // Load counter
    WN *wnld = WN_CreateLdid(OPC_U4U4LDID, no * (MTYPE_RegisterSize( MTYPE_U8 )), 
			     ctr_labels[GCOV_COUNTER_ARCS],
			     Be_Type_Tbl(MTYPE_U4));
    /* increment +1 the counter */
    WN *wnadd = WN_Add(MTYPE_U4,
		       wnld,
		       WN_Intconst(MTYPE_U4, 1));
    // Store the results in mem
    wn = WN_CreateStid (OPC_U4STID, (MTYPE_RegisterSize( MTYPE_U8 )) * no,
			ctr_labels[GCOV_COUNTER_ARCS] , 
			Be_Type_Tbl(MTYPE_U4),wnadd);
  }
  return wn;
}

/* Output conditional instructions to increment the edge execution
   count. This used to add a conter on critical edge*/
static WN *
gen_critical_edge_profiler (unsigned no, basic_block_t *src, basic_block_t *dst )
{
  if (no >= fn_n_ctrs[GCOV_COUNTER_ARCS] - fn_b_ctrs[GCOV_COUNTER_ARCS])
    FmtAssert(FALSE, ("gen_critical_edge_profiler: Edge counter bigger than total number of edges"));
#ifdef DEBUG_COVERAGE
  fprintf(TraceCovFile,"\t gen_critical_edge_profiler:counter %d\n", no);
#endif
  WN *wn = WN_COPY_Tree(src->branch_wn);
  BOOL false_br = (WN_opcode(wn) == OPC_FALSEBR);
  WN *add_wn = gen_edge_profiler(no);
  WN *cond = WN_kid0( wn );
  WN *if_then, *if_else;
  /* Detect FALL THRU edges */
  if (src->next == dst) {
    // Fall Thru
    if (false_br) {
      if_then = add_wn;
      if_else = NULL;
    }else {
      if_then = NULL;
      if_else = add_wn;
    }
  } else {
    // Branch
    if (false_br) {
      if_then = NULL;
      if_else = add_wn;
    } else {
      if_then = add_wn;
      if_else = NULL;
    }
  }
  WN *then_block = WN_CreateBlock ();
  WN *else_block = WN_CreateBlock ();
  WN *IF = WN_CreateIf(cond, then_block, else_block);

  if (if_then) {
    WN *last = WN_last(then_block);
    WN_copy_linenum(last, if_then);
    WN_INSERT_BlockAfter (then_block, last, if_then);
  }
  if (if_else) {
    WN *last = WN_last(else_block);
    WN_copy_linenum(last, if_else);
    WN_INSERT_BlockAfter (else_block, last, if_else);
  }
  // Lover the created IF, since this lowering is expected to have
  // been done
  IF = WN_Lower(IF, LOWER_SCF, NULL, 
		"Lower structured control flow");
  return IF;
}

/* ====================================================================
 * 		 External function
 *  ====================================================================
 */

/* Initialisation of gcov module: need to be called once */
void 
wn_coverage_init ()
{
  // When be is called by ipa_link, the name of gcda and gcno filename
  // are the named of the final executal + the nb of the .s file that
  // is compiled
  const char *filename = Remove_Extension(Ipa_Exec_Name ? Ipa_Exec_Name : Src_File_Name);
  // add _x for ipa
  if (Ipa_Exec_Name) {
    char *filename_ipa = (char*) alloca( strlen(filename) + strlen(Remove_Extension(Src_File_Name)) + 1);
    sprintf(filename_ipa,"%s_%s",filename, Remove_Extension(Src_File_Name));
    filename = filename_ipa;
  }
  int len = strlen (filename);
  /* Start a new local memory allocation space
   */
  MEM_POOL_Initialize(&MEM_coverage_pool_val,"coverage pool", TRUE /* put to 0*/);
  MEM_POOL_Push(&MEM_coverage_pool_val);
  MEM_coverage_pool = &MEM_coverage_pool_val;

  Trace_Coverage = Get_Trace( TP_FEEDBACK, TP_FEEDBACK_COVERAGE );
  randomize();
  if (Branch_Probabilities || Profile_Arcs_Enabled) {
    char *pwdname = (char*) alloca(PATH_MAX);
    if (getcwd(pwdname, PATH_MAX) == NULL) {
      fprintf(stderr, "error getting current dir: pwd is too long");
      pwdname = NULL;
    }
    cwdname = TYPE_MEM_POOL_ALLOC_N(char,
				    MEM_coverage_pool,
				    strlen (pwdname)+1);
    strcpy(cwdname, pwdname);
    //   Name of da file.
    da_file_name = TYPE_MEM_POOL_ALLOC_N(char,
					 MEM_coverage_pool,
					 len + strlen (GCOV_DATA_SUFFIX) + 1);
    strcpy (da_file_name, filename);
    strcat (da_file_name, GCOV_DATA_SUFFIX);
  }
  if (Profile_Arcs_Enabled || Test_Coverage_Enabled) {
    /* Name of bbg file.  */
    bbg_file_name = TYPE_MEM_POOL_ALLOC_N(char,
					  MEM_coverage_pool,
					  len + strlen (GCOV_NOTE_SUFFIX) + 1);
    strcpy (bbg_file_name, filename);
    strcat (bbg_file_name, GCOV_NOTE_SUFFIX);
  }

  // Initialize filename and pathname table
  Gen_File_Table(); 

  if (Branch_Probabilities)
    read_counts_file ();
}

/* Finish gcno generation + instrument code: to be called once */
void 
wn_coverage_finish ()
{
  if (!prg_ctr_mask)
    goto free_mem;

  if (Profile_Arcs_Enabled ) {
    
    // Set size for gcov symbols
    // Recreate the symbol to force emition in Process_Bss_Data()
    ST *st = ctr_labels[GCOV_COUNTER_ARCS];
    TY_IDX ty_idx = Make_Array_Type(MTYPE_U8, 
				    1, total_num_edges_instrumented);
    Set_TY_align(ty_idx, 8);
    // To allow allocation:
    Clear_ST_is_not_sized (st);
    // The real type:
    Set_ST_type(st,ty_idx);
    // Since the symbol in the global table has already been processed
    // by Process_Bss_Data, we need to create a new one but with the
    // same index, otherwise the ipa will initialized it with the
    // wrong name (IPA renames all static variables).
    ST *copy_st = Copy_ST(st);
    Set_ST_st_idx(copy_st, ST_st_idx(st));
    Allocate_Object(copy_st);

    // Create gcov ctor section
    create_coverage (ty_idx);
  }

 free_mem:
  if (bbg_file_opened)
    {
      int error = gcov_close ();

      if (error)
	unlink (bbg_file_name);
    }
  // free memory
  MEM_POOL_Pop(MEM_coverage_pool);
  MEM_POOL_Delete (MEM_coverage_pool);
  MEM_coverage_pool = NULL;
}

/* Generate gcov info for the current pu. Need to be called once for
   each PU. This function writes/update a gcno file for the current
   PU. Add instrumenting counter on needed edges
*/
void 
wn_coverage_pu(WN *pu)
{
  unsigned num_instrumented = 0;

  // Set the tree WN 
  Tree_WN = pu;

  // Start by incrementing the pu counter
  current_function_funcdef_no++;

  /* Start a new local memory allocation space for this pu
   */
  MEM_POOL_Initialize(&MEM_coverage_pool_pu_val,"coverage pool", TRUE /* to zero the mem. */);
  MEM_POOL_Push(&MEM_coverage_pool_pu_val);
  MEM_coverage_pool_pu = &MEM_coverage_pool_pu_val;

  // Pu must be a func entry!!
  Is_True( WN_operator(pu) == OPR_FUNC_ENTRY,
	   ( "wn_coverage_pu:"
	     " OPR_FUNC_ENTRY expected at top of PU tree." ));

  // Create the cfg from the whirl 
  Coverage_Tree_Walk(pu); 

  // For emplty function, nothing to do
  if (Cur_Function->bb_count == 0)
    goto finish;

  // Initialize first and last bb
  First_BB = new_bb(MEM_coverage_pool_pu);
  First_BB->index = FIRST_BB_TO_GCOV_INDEX;
  Last_BB = new_bb(MEM_coverage_pool_pu);
  // Set index of last_bb
  Last_BB-> index = LAST_BB_TO_GCOV_INDEX;

  // Then create edge list from the cflow
  edges_list = create_edge_list();

  if (Trace_Coverage) {
    fprintf(TraceCovFile, "wn_coverage_pu: build CFG from the WHIRL for %s:\n", Cur_PU_Name);
    wn_coverage_Print_All_BBs(1);  
  }
  //    Create spanning tree from basic block graph, mark each edge
  //    that is on the spanning tree.  We insert as many abnormal and
  //    critical edges as possible to minimize number of edge splits
  //    necessary.
#if instrument_all_edges != 1
  find_spanning_tree (edges_list);  
#endif

  /* Write the data from which gcov can reconstruct the basic block
     graph.  */
  if (Profile_Arcs_Enabled || Test_Coverage_Enabled) {
    /* Basic block flags */
    if (coverage_begin_output (pu)) {
      coverage_blocks();
    } else {
      fprintf (stderr, "Cancel coverage instrumentation\n", bbg_file_name);
      goto finish;
    }
    
    /* Arcs */
    if (coverage_begin_output (pu)) {
      num_instrumented += coverage_arcs(edges_list);
    }
    total_num_edges_instrumented += num_instrumented;
    if (Test_Coverage_Enabled) {
      /* Line numbers.  */
      if (coverage_begin_output (pu)) {
	basic_block_list_t *list_bb;
	for ( list_bb = Cur_Function->bb_list; list_bb != NULL; 
	      list_bb = list_bb->next) { 
	  coverage_lines(list_bb->bb);
	}
      }
    }
  }
  if (Branch_Probabilities)
    {
      // Add fake edges to the control flow in order to match the gcov
      // CFG
      add_fake_edge_to_Cur_Function();
      /* Use .gcda file to get branch prob*/
      wn_coverage_branch_probabilities();

      if (Trace_Coverage) {
	fprintf(TraceCovFile, "wn_coverage_pu: CFG after freq propagation:\n");
	wn_coverage_Print_All_BBs(1);  
      }
      if (Cur_Function->bb_list->next->bb->count == 0) {
	DevWarn( "%s Cannot find expected feedback data - function not called?", Cur_PU_Name);
	goto finish ;
      }
      // Annotate whirl with counter information
      wn_coverage_annotate(pu);
    }
  if (Trace_Coverage) {
    fprintf(TraceCovFile, "wn_coverage_pu: Whirl before instrumentation:\n");
    dump_instrumented_whirl_with_bb(pu);
  }
  
  /* For each edge not on the spanning tree, add counting code.  */
  if (Profile_Arcs_Enabled && 
      coverage_counter_alloc (GCOV_COUNTER_ARCS, num_instrumented))
    {
      unsigned n_instrumented = instrument_edges (edges_list, WN_func_body(pu));
      if (n_instrumented != num_instrumented)
	FmtAssert(FALSE, ("gcov_pu: invalid instrumented nb of edges"));
      if (Trace_Coverage) {
	fprintf(TraceCovFile, "wn_coverage_pu: Whirl after instrumentation:\n");
	dump_instrumented_whirl_with_bb(pu);
      }
    }

  /* Finish for this function */
  coverage_end_function();

#if 0 /*def DEBUG_COVERAGE*/
  fprintf(TraceCovFile, "WHIRL tree output to Coverage_Tree_Walk:\n");
  //   fdump_tree_with_freq(TraceCovFile, tree, WN_MAP_UNDEFINED);
  dump_tree(pu);
#endif
 finish:
  // free memory
  MEM_POOL_Pop(MEM_coverage_pool_pu);
  MEM_POOL_Delete (MEM_coverage_pool_pu);
  MEM_coverage_pool_pu = NULL;

  edges_list = NULL;
  Cur_BB = NULL;
  Cur_Function = NULL;
  BB_index = 0;
  Last_BB = NULL;
  First_BB = NULL;
  index_last = 0;
}



/* ====================================================================
 *
 *          Branch_Probabilities option read count file and create
 *          feedback object
 *
 * ====================================================================
 */
/* Counts information for a function.  */
typedef struct counts_entry
{
  /* We hash by  */
  unsigned ident;
  unsigned ctr;

  /* Store  */
  unsigned checksum;
  gcov_type *counts;
  struct gcov_ctr_summary summary;

  /* Workspace */
  struct counts_entry *chain;

} counts_entry_t;

// [HK]
#if __GNUC__ >= 3
#include <ext/hash_map>
#else
#include "hash_map.h"
#endif // __GNUC__ >= 3
/* Hash table of count data.  */
struct eq_counts_entry_t
{
  bool operator()(const counts_entry_t * s1, const counts_entry_t* s2) const
  {
    return s1->ident == s2->ident && s1->ctr == s2->ctr;
  }
};
typedef counts_entry_t* pcounts_entry_t;
typedef __GNUEXT::hash_map<long, pcounts_entry_t, __GNUEXT::hash<long> > HASH_MAP;
static HASH_MAP counts_hash;

/* Counter summary from the last set of coverage counts read.  */
const struct gcov_ctr_summary *profile_info;

static long
counts_entry_hash (counts_entry_t entry)
{
  return entry.ident * GCOV_COUNTERS + entry.ctr;
}

/* Read in the counts file, if available.  */
static void
read_counts_file (void)
{
  gcov_unsigned_t fn_ident = 0;
  gcov_unsigned_t checksum = 0xFFFFFFFF;
  counts_entry_t *summaried = NULL;
  unsigned seen_summary = 0;
  gcov_unsigned_t tag;
  int is_error = 0;

  if (!gcov_open (da_file_name, 1))
    return;

  if (!gcov_magic (gcov_read_unsigned (), GCOV_DATA_MAGIC))
    {
      fprintf (stderr,"`%s' is not a gcov data file\n", da_file_name);
      gcov_close ();
      return;
    }
  else if ((tag = gcov_read_unsigned ()) != GCOV_VERSION)
    {
      char v[4], e[4];

      GCOV_UNSIGNED2STRING (v, tag);
      GCOV_UNSIGNED2STRING (e, GCOV_VERSION);

      fprintf (stderr, "`%s' is version `%.4s', expected version `%.4s'\n",
 	       da_file_name, v, e);
      gcov_close ();
      return;
    }

  /* Read and discard the stamp.  */
  gcov_read_unsigned ();
  
  while ((tag = gcov_read_unsigned ()))
    {
      gcov_unsigned_t length;
      gcov_position_t offset;

      length = gcov_read_unsigned ();
      offset = gcov_position ();
      if (tag == GCOV_TAG_FUNCTION)
	{
	  fn_ident = gcov_read_unsigned ();
	  checksum = gcov_read_unsigned ();
	  if (seen_summary)
	    {
	      /* We have already seen a summary, this means that this
		 new function begins a new set of program runs. We
		 must unlink the summaried chain.  */
	      counts_entry_t *entry, *chain;

	      for (entry = summaried; entry; entry = chain)
		{
		  chain = entry->chain;
		  entry->chain = NULL;
		}
	      summaried = NULL;
	      seen_summary = 0;
	    }
	}
      else if (tag == GCOV_TAG_PROGRAM_SUMMARY)
	{
	  counts_entry_t *entry;
	  struct gcov_summary summary;

	  gcov_read_summary (&summary);
	  seen_summary = 1;
	  for (entry = summaried; entry; entry = entry->chain)
	    {
	      struct gcov_ctr_summary *csum = &summary.ctrs[entry->ctr];

	      entry->summary.runs += csum->runs;
	      entry->summary.sum_all += csum->sum_all;
	      if (entry->summary.run_max < csum->run_max)
		entry->summary.run_max = csum->run_max;
	      entry->summary.sum_max += csum->sum_max;
	    }
	}
      else if (GCOV_TAG_IS_COUNTER (tag) && fn_ident)
	{
	  counts_entry_t elt;
	  unsigned n_counts = GCOV_TAG_COUNTER_NUM (length);
	  unsigned ix;

	  elt.ident = fn_ident;
	  elt.ctr = GCOV_COUNTER_FOR_TAG (tag);

	  pcounts_entry_t & entry = counts_hash[counts_entry_hash(elt)];
	  if (entry == NULL)
	    {
	      entry = TYPE_MEM_POOL_ALLOC_N(counts_entry_t,
					    MEM_coverage_pool,
					    1);
	      entry->ident = elt.ident;
	      entry->ctr = elt.ctr;
	      entry->checksum = checksum;
	      entry->summary.num = n_counts;
	      entry->counts = TYPE_MEM_POOL_ALLOC_N(gcov_type,
						    MEM_coverage_pool,
						    n_counts);
	    }
	  else if (entry->checksum != checksum)
	    {
	      fprintf (stderr, "error:coverage mismatch for function %u while reading execution counters.",
		     fn_ident);
	      fprintf (stderr, "error:checksum is %x instead of %x", entry->checksum, checksum);
	      counts_hash.clear();
	      break;
	    }
	  else if (entry->summary.num != n_counts)
	    {
	      fprintf (stderr, "error:coverage mismatch for function %u while reading execution counters.\n",
		     fn_ident);
	      fprintf (stderr, "error:number of counters is %d instead of %d\n", entry->summary.num, n_counts);
	      counts_hash.clear();
	      break;
	    }
	  else if (elt.ctr >= GCOV_COUNTERS_SUMMABLE)
	    {
	      fprintf (stderr, "error:cannot merge separate %s counters for function %u\n",
		     ctr_names[elt.ctr], fn_ident);
	      goto skip_merge;
	    }

	  if (elt.ctr < GCOV_COUNTERS_SUMMABLE
	      /* This should always be true for a just allocated entry,
		 and always false for an existing one. Check this way, in
		 case the gcov file is corrupt.  */
	      && (!entry->chain || summaried != entry))
	    {
	      entry->chain = summaried;
	      summaried = entry;
	    }
	  for (ix = 0; ix != n_counts; ix++)
	    entry->counts[ix] += gcov_read_counter ();
	skip_merge:;
	}
      gcov_sync (offset, length);
      if ((is_error = gcov_is_error ()))
	break;
    }

  if (!gcov_is_eof ())
    {
      fprintf(stderr, is_error < 0 ? "`%s' has overflowed\n" : "`%s' is corrupted\n",
	     da_file_name);
	      counts_hash.clear();
    }

  gcov_close ();
}

/* Returns the counters for a particular tag.  */

static gcov_type *
get_coverage_counts (unsigned counter, unsigned expected,
		     const struct gcov_ctr_summary **summary)
{
  counts_entry_t elt;
  gcov_unsigned_t checksum = 0xFFFFFFFF;

  /* No hash table, no counts.  */
  if (counts_hash.empty())
    {
      fprintf (stderr, "note:file %s not found, execution counts assumed to be zero\n",
	       da_file_name);
      return NULL;
    }

  elt.ident = current_function_funcdef_no;
  elt.ctr = counter;
  pcounts_entry_t & entry = counts_hash[counts_entry_hash(elt)];
//   entry = htab_find (counts_hash, &elt);
  if (entry ==  NULL)
    {
      fprintf (stderr,"warning:no coverage for function '%s' found.\n", Cur_PU_Name);
      return 0;
    }

  checksum = compute_checksum ();
  if (entry->checksum != checksum)
    {
      fprintf (stderr,"error:coverage mismatch for function '%s' while reading counter '%s'.\n",
	     Cur_PU_Name, ctr_names[counter]);
      fprintf (stderr,"error:checksum is %x instead of %x", entry->checksum, checksum);
      return 0;
    }
  else if (entry->summary.num != expected)
    {
      fprintf (stderr,"error:coverage mismatch for function '%s' while reading counter '%s'.\n",
	     Cur_PU_Name,
	     ctr_names[counter]);
      fprintf (stderr,"error:number of counters is %d instead of %d\n", entry->summary.num, expected);
      return 0;
    }

  if (summary)
    *summary = &entry->summary;

  return entry->counts;
}

/* Computes hybrid profile for all matching entries in da_file.  */

static gcov_type *
get_exec_counts (void)
{
  unsigned num_edges = 0;
  gcov_type *counts;

  /* Count the edges to be (possibly) instrumented.  */
  for (int x = 0; x < edges_list->num_edges; x++) {
    edge_t e = edges_list->index_to_edge[x];
    // Compte nb of instrumented edges
    if (!e->fake && !e->on_tree)
      num_edges++;
  }

  counts = get_coverage_counts (GCOV_COUNTER_ARCS, num_edges, &profile_info);
  if (!counts)
    return NULL;

  return counts;
}

// Add fake edge to the CFG
// Add First and Last BB to the CFG
static void 
add_fake_edge_to_Cur_Function (void)
{

  // Add an edge between gcov First_BB and the first bb in the CFG
  Link_Pred_Succ(First_BB, Cur_Function->bb_list->bb);

  // Add last_bb to the end of bb listing of the cur_function
  Add_BB_To_Cur_Function(Last_BB);
  // Add First_BB to the beginning of bb listing of the cur_function
  basic_block_list_t *new_list_bb = TYPE_MEM_POOL_ALLOC_N(basic_block_list_t,
							  MEM_coverage_pool_pu,
							  1);
  new_list_bb->bb = First_BB;
  new_list_bb->next = Cur_Function->bb_list;
  Cur_Function->bb_list = new_list_bb;

  
  for (int x = 0; x < edges_list->num_edges; x++) {
    edge_t e = edges_list->index_to_edge[x];
      Link_Pred_Succ(e->src, e->dest);
  }
}

/* Use .gcda file to get branch prob: Transform minimal edge profiling
   into block profiling. At the end of this procedure, all bb and all
   non fake edges have a valid count.
*/
static void 
wn_coverage_branch_probabilities(void)
{
  int num_edges = 0;
  int changes;
  int passes;
  gcov_type *exec_counts = get_exec_counts ();
  int exec_counts_pos = 0;

  /* Very simple sanity checks so we catch bugs in our profiling code.  */
  if (profile_info)
    {
      if (profile_info->run_max * profile_info->runs < profile_info->sum_max)
	{
	  fprintf (stderr, "corrupted profile info: run_max * runs < sum_max\n");
	  exec_counts = NULL;
	}

      if (profile_info->sum_all < profile_info->sum_max)
	{
	  fprintf (stderr, "corrupted profile info: sum_all is smaller than sum_max\n");
	  exec_counts = NULL;
	}
    }
  
#define SUCC_COUNT(bb) ((bb)->n_succs )

  /* Attach extra info block to each bb.  */
  basic_block_list_t *list_bb;
  for ( list_bb = Cur_Function->bb_list; list_bb != NULL; 
	list_bb = list_bb->next) 
    {
      basic_block_t *bb = list_bb->bb;
      bb->succ_count = (bb)->n_succs;
      bb->pred_count = bb->n_preds;
    }

  /* Avoid predicting entry on exit nodes.  */
  Last_BB->succ_count = 2;
  First_BB->pred_count = 2;

  /* For each edge not on the spanning tree, set its execution count from
     the .gcda file.  */

  /* The first count in the .gcda file is the number of times that the function
     was entered.  This is the exec_count for block zero.  */

  for (int x = 0; x < edges_list->num_edges; x++) {
    edge_t e = edges_list->index_to_edge[x];
#ifdef DEBUG_COVERAGE
    if (!e->fake && !e->on_tree)
      fprintf(TraceCovFile,"edge %d->%d has counter number %d value %d\n",BB_TO_GCOV_INDEX (e->src), 
	      BB_TO_GCOV_INDEX (e->dest) , (int)num_edges, exec_counts ? (int)exec_counts[exec_counts_pos] : 0);
    else
      fprintf(TraceCovFile,"edge %d->%d has no counter\n", BB_TO_GCOV_INDEX (e->src), 
	      BB_TO_GCOV_INDEX (e->dest));
#endif    
    if (!e->on_tree)
      {
	num_edges++;
	if (exec_counts)
	  {
	    e->count = exec_counts[exec_counts_pos++];
	    if (e->count > profile_info->sum_max)
	      {
		fprintf (stderr,"corrupted profile info: edge from %i to %i exceeds maximal count\n",
			 e->src->index, e->dest->index);
	      }
	  }
	else
	  e->count = 0;
	
	e->count_valid = 1;
	e->src->succ_count--;
	e->dest->pred_count--;
      }
  }

  if (Trace_Coverage) {
    fprintf(TraceCovFile, "wn_coverage_pu: CFG after reading %s before freq propagation:\n", da_file_name);
    wn_coverage_Print_All_BBs(0);  
  }
  /* For every block in the file,
     - if every exit/entrance edge has a known count, then set the block count
     - if the block count is known, and every exit/entrance edge but one has
     a known execution count, then set the count of the remaining edge

     As edge counts are set, decrement the succ/pred count, but don't delete
     the edge, that way we can easily tell when all edges are known, or only
     one edge is unknown.  */

  /* The order that the basic blocks are iterated through is important.
     Since the code that finds spanning trees starts with block 0, low numbered
     edges are put on the spanning tree in preference to high numbered edges.
     Hence, most instrumented edges are at the end.  Graph solving works much
     faster if we propagate numbers from the end to the start.

     This takes an average of slightly more than 3 passes.  */

  changes = 1;
  passes = 0;
  while (changes)
    {
      passes++;
      changes = 0;
      for ( list_bb = Cur_Function->bb_list; list_bb != NULL; 
	    list_bb = list_bb->next) 
	{
	  basic_block_t *bb = list_bb->bb;
	  if (! bb->count_valid)
	    {
	      if (bb->succ_count == 0)
		{
		  edge_t e;
		  gcov_type total = 0;

		  unsigned i;
		  for (i = 0; i < SUCC_COUNT(bb); i++) {
		    e = INDEX_EDGE (edges_list, list_bb->bb->index_of_succ_edges[i]);
		    total += e->count;
		  }
		  FmtAssert(total >= 0, ("wn_coverage_branch_probabilities: bb %d has negative counter: %d", bb->index, total));
		  bb->count = total;
		  bb->count_valid = 1;
		  changes = 1;
		}
	      else if (bb->pred_count == 0)
		{
		  edge_t e;
		  gcov_type total = 0;

		  unsigned i;
		  for (i = 0; i < list_bb->bb->n_preds; i++) {
		    e = INDEX_EDGE (edges_list, list_bb->bb->index_of_pred_edges[i]);
		    total += e->count;
		  }
		  FmtAssert(total >= 0, ("wn_coverage_branch_probabilities: bb %d has negative counter: %d", bb->index, total));
		  bb->count = total;
		  bb->count_valid = 1;
		  changes = 1;
		}
	    }
	  if (bb->count_valid)
	    {
	      if (bb->succ_count == 1)
		{
		  edge_t e;
		  gcov_type total = 0;

		  /* One of the counts will be invalid, but it is zero,
		     so adding it in also doesn't hurt.  */
		  unsigned i;
		  for (i = 0; i < SUCC_COUNT(bb); i++) {
		    e = INDEX_EDGE (edges_list, list_bb->bb->index_of_succ_edges[i]);
		    total += e->count;
		  }
		  /* Seedgeh for the invalid edge, and set its count.  */
		  for (i = 0; i < SUCC_COUNT(bb); i++) {
		    e = INDEX_EDGE (edges_list, list_bb->bb->index_of_succ_edges[i]);
		    if (! e->count_valid )
		      break;
		  }
		  /* Calculate count for remaining edge by conservation.  */
		  total = bb->count - total;
		  FmtAssert(total >= 0, ("wn_coverage_branch_probabilities: bb %d has negative counter: %d", bb->index, total));

		  if (! e)
		    abort ();
		  e->count_valid = 1;
		  e->count = total;
		  bb->succ_count--;

		  e->dest->pred_count--;
		  changes = 1;
		}
	      if (bb->pred_count == 1)
		{
		  edge_t e;
		  gcov_type total = 0;

		  /* One of the counts will be invalid, but it is zero,
		     so adding it in also doesn't hurt.  */
		  unsigned i;
		  for (i = 0; i < list_bb->bb->n_preds; i++) {
		    e = INDEX_EDGE (edges_list, list_bb->bb->index_of_pred_edges[i]);
		    total += e->count;
		  }
		  /* Search for the invalid edge, and set its count.  */
		  for (i = 0; i < list_bb->bb->n_preds; i++) {
		    e = INDEX_EDGE (edges_list, list_bb->bb->index_of_pred_edges[i]);
		    if (!e->count_valid)
		      break;
		  }
		  /* Calculate count for remaining edge by conservation.  */
		  total = bb->count - total + e->count;
		  FmtAssert(total >= 0, ("wn_coverage_branch_probabilities: bb %d has negative counter: %d", bb->index, total));

		  if (! e)
		    abort ();
		  e->count_valid = 1;
		  e->count = total;
		  bb->pred_count--;

		  e->src->succ_count--;
		  changes = 1;
		}
	    }
	}
    }
#ifdef DEBUG_COVERAGE
  for (int x = 0; x < edges_list->num_edges; x++) {
    edge_t e = edges_list->index_to_edge[x];
    fprintf(TraceCovFile,"edge %d->%d has value %d\n",BB_TO_GCOV_INDEX (e->src), 
	    BB_TO_GCOV_INDEX (e->dest) , (int) e->count );
  }
#endif
#ifdef Is_True_On  
  // Check
  for ( list_bb = Cur_Function->bb_list->next; list_bb->bb != Last_BB; 
	list_bb = list_bb->next) { 
    FmtAssert(list_bb->bb->count_valid, ("wn_coverage_branch_probabilities: bb %d has no valid count", list_bb->bb->index));
    unsigned total = 0;
    // check that sum of pred edge == bb count
    for (unsigned i = 0; i < list_bb->bb->n_preds; i++) {
      edge_t e = INDEX_EDGE (edges_list, list_bb->bb->index_of_pred_edges[i]);
      total += e->count;
    }
    if (list_bb->bb->count != total) {
      fprintf(stderr,"bb %d: some of predecessor edges (%ld) != count for this bb (%ld)\b", list_bb->bb->index, total, list_bb->bb->count);
      for (unsigned i = 0; i < list_bb->bb->n_preds; i++) {
	edge_t e = INDEX_EDGE (edges_list, list_bb->bb->index_of_pred_edges[i]);
	if (!e->fake && !e->on_tree)
	  fprintf(stderr,"\tedge nb:%d, global nb: %d, offset .LPBX1+%d might have invalid count : %ld\n ", list_bb->bb->index_of_pred_edges[i],  
		  list_bb->bb->index_of_pred_edges[i] + prg_n_ctrs[GCOV_COUNTER_ARCS] + fn_b_ctrs[GCOV_COUNTER_ARCS] - 1,
		  (list_bb->bb->index_of_pred_edges[i] + prg_n_ctrs[GCOV_COUNTER_ARCS] + fn_b_ctrs[GCOV_COUNTER_ARCS] - 1)
		  * (MTYPE_RegisterSize( MTYPE_U8 )), e->count);
      }
    }
    FmtAssert(list_bb->bb->count == total, ("wn_coverage_branch_probabilities: bb %d has invalid count in relation to its predecessors", list_bb->bb->index));
    total = 0;
    for (unsigned i = 0; i < SUCC_COUNT(list_bb->bb); i++) {
      edge_t e = INDEX_EDGE (edges_list, list_bb->bb->index_of_succ_edges[i]);
      total += e->count;
    }
    FmtAssert(list_bb->bb->count == total, ("wn_coverage_branch_probabilities: bb %d has invalid count in relation to its succesoors", list_bb->bb->index));
  }
#endif
}

static int
get_edge_index(basic_block_t *src, basic_block_t *dst)
{
  for (unsigned i = 0; i < src->n_succs; i++) {
    edge_t e = INDEX_EDGE (edges_list, src->index_of_succ_edges[i]);
    if (e->dest == dst)
      return src->index_of_succ_edges[i];
  }
  FmtAssert(0, ("get_edge_index: cannot find edge between BB%d and BB%a", src->index, dst->index));
  return 0;
}

// Create feedback info for an invoke
static void 
Annotate_Invoke (WN *wn, basic_block_list_t *list_bb) 
{
  basic_block_t *bb = list_bb->bb;
  FmtAssert(bb->count_valid, ("Annotate_Invoke: bb %d has no valid count", bb->index));
  gcov_type count;
  count = bb->count;
  FB_FREQ freq( count );
  // Attach profile information to node.
  Cur_PU_Feedback->Annot( wn, FB_EDGE_INCOMING, freq);
}

// Create feedback info for an entry function
static void 
Annotate_Entry (WN *wn, basic_block_list_t *list_bb) 
{
  basic_block_t *bb = list_bb->bb;
  FmtAssert(bb->count_valid, ("Annotate_Entry: bb %d has no valid count", bb->index));
  FB_FREQ freq( bb->count );
  FB_Info_Invoke info_invoke( freq );
  // Attach profile information to node.
  Cur_PU_Feedback->Annot_invoke( wn, info_invoke );
}

// Create feedback info for a branch
static void
Annotate_Branch (WN *wn, basic_block_list_t *list_bb)
{
  basic_block_t *bb = list_bb->bb;
  edge_t fall_thru, other;
  basic_block_list_t *list_bb_succ;
  FmtAssert(bb->count_valid, ("Annotate_Branch: bb %d has no valid count", bb->index));
  FmtAssert(bb->n_succs == 2, ("Annotate_Branch: bb %d has not 2 successors", bb->index));
  int i = 0;
  for ( list_bb_succ = bb->succs; list_bb_succ != NULL; 
	list_bb_succ = list_bb_succ->next) {
    basic_block_t *succ = list_bb_succ->bb;
    if (list_bb->next->bb == succ)
      fall_thru = INDEX_EDGE (edges_list, bb->index_of_succ_edges[i]);
    else
      other = INDEX_EDGE (edges_list, bb->index_of_succ_edges[i]);
    i++;
  }
  gcov_type taken = other->count; 
  gcov_type not_taken = fall_thru->count; 
  FB_FREQ freq_taken( taken );
  FB_FREQ freq_not_taken( not_taken );
  FB_Info_Branch info_branch( freq_taken, freq_not_taken );
  // Attach profile information to node.
  Cur_PU_Feedback->Annot_branch( wn, info_branch );
}

// In order to create a balance feedback, we need also to create dunny
// frequency for circuit since we have no information on circuit that
// has not been lowered
static void
Annotate_Circuit( WN *wn, basic_block_list_t *list_bb)
{
//   FmtAssert(FALSE, ("Annotate_Circuit: example of a circuit that is not lowered when reannoting it!!!"));
    
  Cur_PU_Feedback->Annot_circuit( wn, FB_Info_Circuit( FB_FREQ((list_bb->bb->count + 1)/2),
						       FB_FREQ(list_bb->bb->count/2),
						       FB_FREQ(0.0) ) );
}

// Create feedback info for a call
static void
Annotate_Call_Site (
  WN *wn, basic_block_list_t *list_bb)
{
  basic_block_t *bb = list_bb->bb;
  FmtAssert(bb->count_valid, ("Annotate_Call_Site: bb %d has no valid count", bb->index));
  gcov_type entry = bb->count;
  // exit is the freq of the only outgoing edge
  gcov_type exit = 0;
  // Case of a returning call
  // if bb->succs->bb == Last_BB, calls never returns and exit =0
  if (bb->succs->bb != Last_BB) {
    FmtAssert((bb->index +1 == bb->succs->bb->index), ("Annotate_Call_Site: successor of a call != of next bb", bb->index));
    int index_edge = get_edge_index(bb, bb->succs->bb);
    edge_t edge = INDEX_EDGE(edges_list, index_edge);
    exit = edge->count;
  } else
    FmtAssert((bb->never_return), ("Annotate_Call_Site: successor of a call != of next bb", bb->index));

  FB_FREQ freq_entry(entry);
  FB_FREQ freq_exit(exit);
  FB_Info_Call info_call( freq_entry, freq_exit, entry == exit );
 // Attach profile information to node.
  Cur_PU_Feedback->Annot_call( wn, info_call );
}

// Create feedback info for computed goto (indirect goto)
// The order of filling info_switch has to be the same as in Build_CFG 
static void 
Annotate_Compgoto (WN *b_wn, basic_block_list_t *list_bb)
{
  basic_block_t *bb = list_bb->bb;
  FmtAssert(bb->count_valid, ("Annotate_Compgoto: bb %d has no valid count", bb->index));
  FmtAssert(b_wn == bb->branch_wn, ("Annotate_Compgoto: wn and bb are incompatible"));
  FB_Info_Switch info_switch(WN_num_entries(b_wn));
  BOOL *alreay_seen = (BOOL*) alloca(edges_list->num_edges * sizeof(BOOL));
  bzero(alreay_seen, edges_list->num_edges * sizeof(BOOL));

  switch(WN_opcode(b_wn)) {
  case OPC_COMPGOTO:
    if (WN_kid_count(b_wn) == 3) { /* default case */
      basic_block_t *lab = Get_BB_Label(WN_kid(b_wn,2));
      int index_edge = get_edge_index(bb, lab);
      edge_t edge = INDEX_EDGE(edges_list, index_edge);
      alreay_seen[index_edge] = TRUE;
      FB_FREQ freq(edge->count);
      info_switch[ FB_EDGE_SWITCH_INDEX( FB_EDGE_SWITCH_DEFAULT ) ]
	= freq;

    } /* fall thru */
  case OPC_XGOTO:
    { 
      WN *wn;
      int i;
      wn = WN_first(WN_kid1(b_wn));	/* first goto */
      for (i = 0; i < WN_num_entries(b_wn); i++) {
	basic_block_t *lab = Get_BB_Label(wn);
	int index_edge = get_edge_index(bb, lab);
	if (alreay_seen[index_edge]){
	  info_switch[ FB_EDGE_SWITCH_INDEX( FB_EDGE_SWITCH( i ) ) ]
	    = FB_FREQ_UNKNOWN;
	} else {
	  edge_t edge = INDEX_EDGE(edges_list, index_edge);
	  FB_FREQ freq(edge->count);
	  info_switch[ FB_EDGE_SWITCH_INDEX( FB_EDGE_SWITCH( i ) ) ]
	    = freq;
	}
	alreay_seen[index_edge] = TRUE;
	wn = WN_next(wn);
      }
      if (WN_kid_count(b_wn) == 3) {
	// Case of a XGOTO with default????
	basic_block_t *lab = Get_BB_Label(WN_kid(b_wn,2));
	int index_edge = get_edge_index(bb, lab);
	if (!alreay_seen[index_edge]){
	  edge_t edge = INDEX_EDGE(edges_list, index_edge);
	  FB_FREQ freq(edge->count);
	  info_switch[ FB_EDGE_SWITCH_INDEX( FB_EDGE_SWITCH_DEFAULT )]
	    = freq;
	}
      }
      break;
    }
  default:
    FmtAssert(0, ("Annotate_Compgoto: invalid operator for wn"));
  }
  Cur_PU_Feedback->Annot_switch( b_wn, info_switch );
}

static void 
Annotate_Node (WN *wn, basic_block_list_t *list_bb)
{

  OPERATOR opr = WN_operator( wn );
  if ( opr == OPR_BLOCK ) {
    // Special traversal case for BLOCK structure: for compgoto add
    // frequencie for goto wn inside eack block
    WN *node;
    for ( node = WN_first( wn ); node; node = WN_next( node ) )
      if (OPCODE_has_label(WN_opcode(node))) {
	basic_block_list_t *bbl;
	Get_BB_Label(node, &bbl);
	Annotate_Node( node, bbl);
      }
  }

  // Feedback info only for stmt at this whirl lovering level, do no
  // need to Traverse the kids of the current stmt.
  for ( INT32 i = 0; i < WN_kid_count( wn ); i++ )
    Annotate_Node( WN_kid( wn, i ), list_bb);

  if (FB_valid_opr_invoke(wn))
    {
      Annotate_Invoke(wn, list_bb);
    } else {

      // Perform the instrumentation or annotation of the current node
      switch ( opr ) {
      case OPR_PRAGMA:
	if ( WN_pragma( wn ) != WN_PRAGMA_PREAMBLE_END )
	  break;
	{
	  Annotate_Entry( wn, list_bb);
	}
	break;

      case OPR_TRUEBR:
      case OPR_FALSEBR:
      case OPR_IF:
      case OPR_CSELECT:
	{
	  Annotate_Branch( wn, list_bb );
	}
	break;

      case OPR_CAND:
      case OPR_CIOR:
	{
	  Annotate_Circuit( wn, list_bb);
	}
	break;

      case OPR_PICCALL:
      case OPR_CALL:
      case OPR_ICALL:
      case OPR_INTRINSIC_CALL:
      case OPR_IO:
	{
	  Annotate_Call_Site( wn, list_bb);
	}
	break;

      case OPR_COMPGOTO:
      case OPR_XGOTO:
	{
	  Annotate_Compgoto( wn, list_bb );
	}
	break;
      default:
	// No information for other node
	break;
      }
    }
}

// Walk the CFG and annotate the WHIRL with adequate info
static void
wn_coverage_annotate( WN *tree ) 
{
  // No instrumentation for asm function 
  if (ST_asm_function_st(*WN_st(tree))) return;
  Set_Error_Phase( "WN_coverage_Annotate" );
  // Create Feedback
  if ( Cur_PU_Feedback == NULL )
    Cur_PU_Feedback = CXX_NEW( FEEDBACK( tree, &MEM_pu_pool ), &MEM_pu_pool );

  // Annotate the whirl with counter info form CFG
  Is_True(WN_opcode( tree ) == OPC_FUNC_ENTRY, ("wn_coverage_annotate: unknown root node opcode"));
  basic_block_list_t *list_bb;
  for ( list_bb = Cur_Function->bb_list; list_bb != NULL; list_bb = list_bb->next) 
    {
      basic_block_t *bb = list_bb->bb;
      wnode_list_t *list_wn;
      for ( list_wn = bb->wn_list; list_wn != NULL; list_wn = list_wn->next) 
	{
	  Annotate_Node( list_wn->wn, list_bb);
	}
    }

  // Add inokation feedback for the whole pu.
  // The count is the count of the first bb in the CFG
  Annotate_Invoke(tree, Cur_Function->bb_list->next);
  Cur_PU_Feedback->Verify("after coverage annotation");
}

// [TB] Code from PathScale 2.1. Code to generate a new pu is smarter
// than mine...

// Build the thunck function to invoke __gcov_init. It creates a New
// PU, and insert it at the beginning of pu_tree
void 
wn_coverage_Generate_Func_Start_Profiler_PU(PU_Info** _pu_tree_p)
{
    if(*_pu_tree_p == NULL)
        return; 
    
    // Create memory pool for this pu
    MEM_POOL *mem_pool = MEM_coverage_pool;

    // Create thunk name where call to __gcov_init is done
    char *_func_name = get_file_function_name_long ("__THUNK","I");

    // create new type for global naming function
    TY_IDX func_ty_idx;
    TY& func_ty = New_TY(func_ty_idx);
    TY_Init(func_ty, 0, KIND_FUNCTION, MTYPE_UNKNOWN, STR_IDX_ZERO);
 
    // create return type and parm list 
    TYLIST_IDX parm_idx;
    TYLIST& parm_list = New_TYLIST(parm_idx);
    Set_TY_tylist (func_ty, parm_idx);

    Set_TYLIST_type(parm_list, Be_Type_Tbl(MTYPE_V));  // return type
    Set_TYLIST_type(New_TYLIST(parm_idx),  0);  // end of parm list

    // creat a new PU 
    PU_IDX pu_idx;
    PU& new_pu = New_PU(pu_idx);
    PU_Init(new_pu, func_ty_idx, GLOBAL_SYMTAB + 1);

    Set_PU_no_inline(new_pu);

    // create a new ST for this PU 
    gcov_init_thunk_st = New_ST(GLOBAL_SYMTAB); 
    ST_Init(gcov_init_thunk_st, 
            Save_Str(_func_name),
            CLASS_FUNC,
            SCLASS_TEXT,
            EXPORT_LOCAL,
            TY_IDX (pu_idx));  

    Clear_PU_no_side_effects( new_pu );
    Clear_PU_is_pure( new_pu );
    Set_PU_no_delete( new_pu );
    Set_PU_no_instrument_function(new_pu);
    
    Allocate_Object(gcov_init_thunk_st);

    // creat local symbol table for global naming function
    New_Scope(GLOBAL_SYMTAB + 1, mem_pool, TRUE);    
    SYMTAB_IDX _func_level = CURRENT_SYMTAB;
    Scope_tab[_func_level].st = gcov_init_thunk_st;
    Set_PU_lexical_level(new_pu, _func_level);

    // create WHIRL tree 
    WN *_func_body = WN_CreateBlock();
    WN *_func_entry = WN_CreateEntry(0, gcov_init_thunk_st, 
				     _func_body, 
				     WN_CreateBlock(),
				     WN_CreateBlock());
  
    // add a st with a dummy type for instrumentation
    // the real symbol will be allocated at finalization of the module 
    gcov_info_st = New_ST(GLOBAL_SYMTAB);
    /* Name this symbol: */
    char *buf = (char *)alloca(32);
    sprintf(buf, "%s%d", ".LPBX", 0);
    ST_Init(gcov_info_st, 
	    Save_Str(buf),
	    CLASS_VAR, SCLASS_FSTATIC, EXPORT_LOCAL,  Be_Type_Tbl(MTYPE_U4));
    Set_ST_is_initialized(gcov_info_st);
    // to tell Allocate_Object not to allocate it:
    Set_ST_is_not_sized(gcov_info_st);

    WN *wnld = WN_Lda( Pointer_type, 0, gcov_info_st );
    WN *gcov_init_call = Gen_Call("__gcov_init", wnld);
    WN_INSERT_BlockBefore( _func_body, NULL, gcov_init_call);
    WN *wnpragma = WN_CreatePragma (WN_PRAGMA_PREAMBLE_END, (ST_IDX) NULL, 0, 0);
    WN_INSERT_BlockAfter (_func_body, WN_last(_func_body), wnpragma);
    
    // Insert a RETURN at the end if it does not exist
    WN_INSERT_BlockLast (_func_body, WN_CreateReturn ());
  

    // create PU_Info for this function
    PU_Info *_pu_info = CXX_NEW(PU_Info, mem_pool);
    PU_Info_init(_pu_info);

    Set_PU_Info_tree_ptr(_pu_info, _func_entry);
    Set_PU_Info_state(_pu_info, WT_SYMTAB, Subsect_InMem);
    Set_PU_Info_state(_pu_info, WT_TREE, Subsect_InMem);
    Set_PU_Info_state(_pu_info, WT_PROC_SYM, Subsect_InMem);
    Set_PU_Info_flags(_pu_info, PU_IS_COMPILER_GENERATED | PU_IS_PROFILER);

    // set the symbol idx
    PU_Info_proc_sym(_pu_info) = ST_st_idx(gcov_init_thunk_st);
    
    // create a wn map table
    PU_Info_maptab(_pu_info) = WN_MAP_TAB_Create(mem_pool);

    PU_Info_pu_dst (_pu_info) = DST_INVALID_IDX;
    PU_Info_cu_dst (_pu_info) = DST_INVALID_IDX;    

    // save local symbol table
    Set_PU_Info_symtab_ptr(_pu_info, NULL);
    Save_Local_Symtab(_func_level, _pu_info);

    // set language type. *_pu_tree_p point to the first pu.
    if(PU_c_lang(PU_Info_pu(*_pu_tree_p)))
        Set_PU_c_lang(new_pu);
    if(PU_cxx_lang(PU_Info_pu(*_pu_tree_p)))
        Set_PU_cxx_lang(new_pu);
    if(PU_f77_lang(PU_Info_pu(*_pu_tree_p)))
        Set_PU_f77_lang(new_pu);
    if(PU_f90_lang(PU_Info_pu(*_pu_tree_p)))
        Set_PU_f90_lang(new_pu);
    PU_Info_next(_pu_info) = *_pu_tree_p;
    *_pu_tree_p = _pu_info;
}
