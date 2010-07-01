#include "cg_region.h"
#include "gcov_coverage.h"

/* Collect statistics on the performance of this pass for the entire source
   file.  */

static int total_num_edges_instrumented;

/* INDEX_EDGE_PRED_BB and INDEX_EDGE_SUCC_BB return a pointer to the basic
   block which is either the pred or succ end of the indexed edge.  */
#define INDEX_EDGE_PRED_BB(el, index)	((el)->index_to_edge[(index)]->src)
#define INDEX_EDGE_SUCC_BB(el, index)	((el)->index_to_edge[(index)]->dest)
/* INDEX_EDGE returns a pointer to the edge.  */
#define INDEX_EDGE(el, index)           ((el)->index_to_edge[(index)])

// Returns TRUE if edge src->dst is critical critical means that the
// instrumenting counter for edge profiling can be put on the edge (on
// src or dst)
static BOOL
is_critical (BB *src, BB *dst)
{
  unsigned  n_succs_src = BB_succs_len(src);
  unsigned  n_preds_dst = BB_preds_len(dst);
  if ((n_preds_dst == 1) || (n_succs_src == 1))
    return FALSE;
  return TRUE;
}

/* This functions initializes the edge list. Basically the entire
   flowgraph is processed, and all edges are assigned a number,
   and the data structure is filled in.  */
static edge_t
new_edge() {
  edge_t e = (edge_t) malloc (sizeof(struct edge_def));
  e->src = NULL;
  e->dest = NULL;
  e->from_entry = 0;
  e->to_exit = 0;
  e->on_tree = 0;
  e->fake = 0;
  e->fall_through = 0;
  e->critical = 0;
  return e;
}

static struct edge_list *
create_edge_list (void)
{
  struct edge_list *elist;
  edge_t e;
  int num_edges;
  BB *bb;

  num_edges = 0;

  /* Determine the number of edges in the flow graph by counting successor
     edges on each basic block.  */
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    BBLIST *slst;
    FOR_ALL_BB_SUCCS(bb, slst) {
      num_edges++;
    }
    if ((BB_kind(bb) == BBKIND_CALL) || (BB_succs_len(bb) == 0) || BB_exit(bb)) 
      num_edges++;
  }
  num_edges++; // +1 for edge between entry bb and the first in the cflow
  elist = (struct edge_list *)malloc (sizeof (struct edge_list));
  elist->num_edges = num_edges;
  elist->index_to_edge = (edge_t *) malloc (sizeof (edge_t) * num_edges);

  num_edges = 0;

  // Create an edge between entry bb and the first in the cflow
  e = new_edge();
  e->dest = REGION_First_BB;
  e->from_entry = 1;
  e->fall_through = 1;
  elist->index_to_edge[num_edges++] = e;

  /* Follow successors of blocks, and register these edges.  */
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    BBLIST *slst;
    FOR_ALL_BB_SUCCS(bb, slst) {
      BB *succ = BBLIST_item(slst);
      e = new_edge();
      e->src = bb;
      e->dest = succ;
      if (BB_next(bb) == succ)
	e->fall_through = 1;
      if (is_critical(bb, succ)) 
	e->critical = 1;
      elist->index_to_edge[num_edges++] = e;
    }
    // if bb is a call, add a fale edge to exit
    if ((BB_kind(bb) == BBKIND_CALL)) {
      e = new_edge();
      e->src = bb;
      e->to_exit = 1;
      e->fake = 1;
      //       e->on_tree = 1;
      elist->index_to_edge[num_edges++] = e;
    } else if (BB_exit(bb)) {
      e = new_edge();
      e->src = bb;
      e->to_exit = 1;
      e->fall_through = 1;
      //      e->on_tree = 1;
      elist->index_to_edge[num_edges++] = e;
    } else if (BB_succs_len(bb) == 0)  {
      /* Add a fake edge between any block which has no
	 successors, and the exit block. Some data flow equations require these
	 edges to exist.*/  
      e = new_edge();
      e->src = bb;
      e->to_exit = 1;
      e->fake = 1;
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

// Add code for edge src->dst. num_edge is the identifier of the edge
// If src is null, insert code at start of dst
static void
insert_insn_on_edge (BB *src, BB *dst, unsigned num_edge)
{
  unsigned  n_succs_src = (src == NULL) ? 0 : BB_succs_len(src);
  unsigned  n_preds_dst = (dst == NULL) ? 0 : BB_preds_len(dst);
  // Figure out where to put these things.  
  //If the destination has one predecessor, insert there.
  if (src == NULL || n_preds_dst == 1)
    {
      // Generate code to increment this edge
      OPS edge_profile = OPS_EMPTY;
      gen_edge_profiler (num_edge, &edge_profile);
      // Attach srcline info to new ops
      OP *op;
      if (src && BB_last_op(src)) {
	FOR_ALL_OPS_OPs(&edge_profile, op) {
	  OP_srcpos(op) = OP_srcpos(BB_last_op(src));
	}
      } else if (BB_first_op(dst)) {
	FOR_ALL_OPS_OPs(&edge_profile, op) {
	  OP_srcpos(op) = OP_srcpos(BB_first_op(dst));
	}
      }
      // insert ops at start of dst
      BB_Insert_Ops(dst, NULL, &edge_profile, TRUE);
    }
  // If the source has one successor insert there.  
  else if (n_succs_src == 1)
    {
      // Generate code to increment this edge
      OPS edge_profile = OPS_EMPTY;
      gen_edge_profiler (num_edge, &edge_profile);
      // Attach srcline info to new ops
      OP *op;
      if (BB_last_op(src)) {
	FOR_ALL_OPS_OPs(&edge_profile, op) {
	  OP_srcpos(op) = OP_srcpos(BB_last_op(src));
	}
      }
      // insert ops at end of src
      OP *br = BB_xfer_op(src);
      if (!br)
	BB_Insert_Ops(src, NULL, &edge_profile, FALSE);
      else
	// if src is a branch bb, insert code before the branch,
	// otherwise the code will not be executed.
	BB_Insert_Ops(src, br, &edge_profile, TRUE);
    }
  // edge is critical
  else
    {
      // Case of igoto with more than 2 successors 
      if (OP_ijump(BB_branch_op(src))) {
	// Here we would need to split the edge to add the code for
	// the counter. For the moment, this edge will not be instrumented.
	// Gen_And_Insert_BB_After: update the jump table + add succesor
	BB* insert_bb = Gen_And_Insert_BB_After(src);
	if (!BB_Retarget_Branch(src, dst, insert_bb))
	  FmtAssert(0, ("gcov coverage: unable to retarget jump table"));
	Add_Goto(insert_bb, dst);
	//fprintf(stdout,"edge between %d and %d, add a bb %d\n", BB_id(src), BB_id(dst), BB_id(insert_bb));
	// Generate code to increment this edge
	OPS edge_profile = OPS_EMPTY;
	gen_edge_profiler (num_edge, &edge_profile);
	// Attach srcline info to new ops
	OP *op;
	if (BB_last_op(src)) {
	  FOR_ALL_OPS_OPs(&edge_profile, op) {
	    OP_srcpos(op) = OP_srcpos(BB_last_op(src));
	  }
	}
	BB_Insert_Ops(insert_bb, BB_xfer_op(insert_bb), &edge_profile, TRUE);
      } else {

	// Generate conditional code to increment this edge
	OPS edge_profile = OPS_EMPTY;
	gen_critical_edge_profiler (num_edge, &edge_profile, src, dst);
	// Attach srcline info to new ops
	if (BB_last_op(src)) {
	  OP *op;
	  FOR_ALL_OPS_OPs(&edge_profile, op) {
	    OP_srcpos(op) = OP_srcpos(BB_last_op(src));
	  }
	}
	// insert ops before the branch
	if (BB_xfer_op(src))
	  BB_Insert_Ops(src, BB_xfer_op(src), &edge_profile, TRUE);
	else
	  BB_Insert_Ops(src, NULL, &edge_profile, FALSE);
      }
    }
}

/* Add edge instrumentation code to the entire insn chain.
 */
static unsigned
instrument_edges (struct edge_list *el)
{
  unsigned num_instr_edges = 0;
  int x;
  
  for (x = 0; x < el->num_edges; x++) {
    edge_t e = el->index_to_edge[x];
    if (!e->fake && !e->on_tree)
      insert_insn_on_edge (e->src, e->dest, num_instr_edges++);
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
  BB *bb;

  bb_groups = (unsigned *) malloc ((PU_BB_Count + 2)* sizeof(unsigned));
  const unsigned EXIT_BLOCK_ID = PU_BB_Count +1;
  const unsigned ENTRY_BLOCK_ID = 0;

  // First, each bb belongs to its group
  bb_groups[EXIT_BLOCK_ID] = EXIT_BLOCK_ID;
  bb_groups[ENTRY_BLOCK_ID] = ENTRY_BLOCK_ID;
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    bb_groups[BB_id(bb)] = BB_id(bb);
  }

  /* Add fake edge exit to entry we can't instrument.  */
  union_groups (EXIT_BLOCK_ID, ENTRY_BLOCK_ID);

  /* First add all abnormal edges to the tree unless they form a cycle. Also
     add all edges to EXIT_BLOCK_PTR to avoid inserting profiling code behind
     setting return value from function.  */
  for (i = 0; i < num_edges; i++)
    {
      edge_t e = INDEX_EDGE (el, i);
      unsigned src_id =  (e->src != NULL) ? BB_id(e->src) : ENTRY_BLOCK_ID;
      unsigned dest_id = (e->dest != NULL) ? BB_id(e->dest) : EXIT_BLOCK_ID;
      if ( (e->fake || e->to_exit)
	   && (find_group (src_id) != find_group (dest_id)))
	{
// 	    if (e->fake) 
// 	      fprintf (stdout, "Fake edge %d to %d put to tree\n",
// 		       src_id, dest_id); 
// 	    else if (e->to_exit) {
// 	      fprintf (stdout, "Edge to exit %d to %d put to tree\n",
// 		       src_id, dest_id); }
	  e->on_tree = 1;
	  union_groups (src_id, dest_id);
	}
    }

  /* Now insert all critical edges to the tree unless they form a cycle.  */
  for (i = 0; i < num_edges; i++)
    {
      edge_t e = INDEX_EDGE (el, i);
      unsigned src_id =  (e->src != NULL) ? BB_id(e->src) : ENTRY_BLOCK_ID;
      unsigned dest_id = (e->dest != NULL) ? BB_id(e->dest) : EXIT_BLOCK_ID;
      if (e->critical
	  && find_group (src_id) != find_group (dest_id))
	{
// 	  fprintf (stdout, "Critical edge %d to %d put to tree\n",
// 		   src_id, dest_id); 
	  e->on_tree = 1;
	  union_groups (src_id, dest_id);
	}
    }
  
  /* And now the rest.  */
  for (i = 0; i < num_edges; i++)
    {
      edge_t e = INDEX_EDGE (el, i);
      unsigned src_id =  (e->src != NULL) ? BB_id(e->src) : ENTRY_BLOCK_ID;
      unsigned dest_id = (e->dest != NULL) ? BB_id(e->dest) : EXIT_BLOCK_ID;
//       fprintf (stdout, "Normal edge %d [%d] to %d [%d]\n",
// 	       src_id, find_group (src_id), dest_id, find_group (dest_id)); 
      if (find_group (src_id) != find_group (dest_id))
	{
//  	  fprintf (stdout, "\tput to tree\n");
	  e->on_tree = 1;
	  union_groups (src_id, dest_id);
// 	  fprintf (stdout, "New group %d = [%d], %d = [%d]\n",
// 		   src_id, find_group (src_id), dest_id, find_group (dest_id)); 
	}
    }

  free (bb_groups);
  bb_groups = NULL;
}

void
gcov_init (const char *filename)
{
  FmtAssert(total_num_edges_instrumented == 0, ("gcov_init: total_num_edges_instrumented not null"));
  coverage_init(filename);
}

/* Function called at the all end to finalyze gcov stuff */
void
gcov_finish (FILE *file)
{
  coverage_finish(file, total_num_edges_instrumented);
}

/*
  This function writes/update a gcno file for the current PU. Add
  instrumenting counter on needed edges
*/
void 
gcov_pu()
{
  int profile_arc_flag = 1;
  unsigned num_instrumented = 0;
  struct edge_list *edges_list;

  // first create edge list from the cflow
  edges_list = create_edge_list();

  //    Create spanning tree from basic block graph, mark each edge
  //    that is on the spanning tree.  We insert as many abnormal and
  //    critical edges as possible to minimize number of edge splits
  //    necessary.
    
  find_spanning_tree (edges_list);  

  /* Write the data from which gcov can reconstruct the basic block
     graph.  */

  /* Basic block flags */
  if (coverage_begin_output (Get_Current_PU_ST(), REGION_First_BB)) {
    coverage_blocks();
  }

  /* Arcs */
  if (coverage_begin_output (Get_Current_PU_ST(), REGION_First_BB)) {
    num_instrumented += coverage_arcs(edges_list);
  }
  total_num_edges_instrumented += num_instrumented;
  /* Line numbers.  */
  if (coverage_begin_output (Get_Current_PU_ST(), REGION_First_BB)) {
    BB *bb;
    for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
      coverage_lines(bb);
    }
  }

  /* For each edge not on the spanning tree, add counting code.  */
  if (profile_arc_flag
      && coverage_counter_alloc (// GCOV_COUNTER_ARCS 
				 0
				 , num_instrumented))
    {
      unsigned n_instrumented = instrument_edges (edges_list);
      if (n_instrumented != num_instrumented)
	FmtAssert(FALSE, ("gcov_pu: invalid instrumented nb of edges"));
    }
  /* Finish for this function */
  coverage_end_function();
  free_edge_list(edges_list);
  edges_list = NULL;
}
