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


/* ====================================================================
 *
 * Module: findloops.c
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description:
 *
 * Compute frequency estimates for basic block edges.
 *
 * ====================================================================
 */
#ifdef _KEEP_RCS_ID
static const char rcs_id[] = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#include "defs.h"
#include "config.h"
#include "be_errors.h"
#include "mempool.h"
#include "tracing.h"
#include "glob.h"
#include "bitset.h"
#include "bb.h"
#include "bb_set.h"
#include "cg_sched_est.h"
#include "dominate.h"
#include "region_util.h"
#include "cg_region.h"
#include "cg_flags.h"
#include "cflow.h"	/* for tracing flags */
#include "annotations.h"
#include "whirl2ops.h"
#include "note.h"

#include "findloops.h"

BB_MAP LOOP_DESCR_map;
static BOOL loop_descr_map_initted;
static LOOP_DESCR *the_loops;
static BB_MAP dfo_map;

/* ====================================================================
 *
 * LOOP_DESCR_Print_List
 * 
 * See interface description.
 *
 * ====================================================================
 */
void
LOOP_DESCR_Print_List(void)
{
  LOOP_DESCR *cloop;
  INT i;
  FILE *tfile = TFile;

  i = 0;
  for (cloop = the_loops; cloop != NULL; cloop = LOOP_DESCR_next(cloop)) {
    BB *loophead = LOOP_DESCR_loophead(cloop);
    i++;
    fprintf (tfile, 
	"LOOP %d  (loop head: %d, nest level: %d, # exits: %d",
	i,
	loophead ? BB_id(loophead) : 0, 
	LOOP_DESCR_nestlevel(cloop), 
	LOOP_DESCR_num_exits(cloop));
    if (LOOP_DESCR_loopinfo(cloop) != NULL) {
      fprintf (tfile, ", loop-info:\n");
      Print_LOOPINFO(LOOP_DESCR_loopinfo(cloop));
    } else {
      fprintf (tfile, ", loop-info: <none>");
    }
    fprintf (tfile, ")\n");
    BB_SET_Print (LOOP_DESCR_bbset(cloop), tfile);
    fprintf (tfile, "\n\n");
  }
}


/* ====================================================================
 *
 * LOOP_DESCR_Is_Exit_Edge
 * 
 * See interface description.
 *
 * ====================================================================
 */
LOOP_DESCR *
LOOP_DESCR_Is_Exit_Edge(BB *bb, BB *succ)
{
  LOOP_DESCR *loop, *exitloop;

  DevAssert(loop_descr_map_initted, ("<LOOP_DESCR_map> not yet initialized"));
  exitloop = NULL;
  for (loop = (LOOP_DESCR *)BB_MAP_Get(LOOP_DESCR_map, bb);
       loop != NULL; loop = LOOP_DESCR_next(loop)) {
    if (BB_SET_MemberP (LOOP_DESCR_bbset(loop), bb) && 
	!BB_SET_MemberP (LOOP_DESCR_bbset(loop), succ))
    {
      if (!BB_Find_Succ(bb, LOOP_DESCR_loophead(loop))) {
	/* LNO and CG disagree about the meaning of a "trip-countable" loop.
	 * LNO considers a loop trip-countable even if there's an early exit
	 * that may cause the loop to execute fewer times than the trip count.
	 * CG considers a loop trip-countable iff the trip count expression
	 * always gives the number of trips that the loop will execute.
	 * Since LNO's definition is more broad, WOPT fills in trip counts
	 * according to its definition, and it's up to CG to get rid of the
	 * trip counts that aren't necessarily correct because of early exits!
	 */
	LOOPINFO *info = LOOP_DESCR_loopinfo(loop);
	if (info) LOOPINFO_trip_count_tn(info) = NULL;
      }

      exitloop = loop;
      if (LOOP_DESCR_nestlevel(exitloop) == 1) break;
    }
  }
  return exitloop;
}


/* ====================================================================
 * See "findloops.h" for interface description.
 * ====================================================================
 */
void 
LOOP_DESCR_Init_For_PU(void)
{
  loop_descr_map_initted = FALSE;
}


/* ====================================================================
 *
 * LOOP_DESCR_Detect_Loops
 * 
 * See interface description.
 *
 * ====================================================================
 */
LOOP_DESCR *
LOOP_DESCR_Detect_Loops (MEM_POOL *pool)
{
  LOOP_DESCR *newloop, *cloop, *lastloop, *exitloop, *encl_loop;
  BB_SET *loop_set;
  BB *bb;
  BBLIST *blst;
  BB *succ;
  ANNOTATION *annot;
  
  the_loops = NULL;
  if (loop_descr_map_initted) {
    BB_MAP_Delete(LOOP_DESCR_map);
    loop_descr_map_initted = FALSE;
  }

  dfo_map = BB_Depth_First_Map(NULL, NULL);

  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    INT32 bb_dfo_id = BB_MAP32_Get(dfo_map, bb);
    DevAssert(bb_dfo_id >= 0 && bb_dfo_id <= PU_BB_Count,
	      ("bad <dfo_map> value"));
    if (bb_dfo_id == 0) continue;	/* skip unreachable BBs */

    FOR_ALL_BB_SUCCS (bb, blst) {
      succ = BBLIST_item(blst);
      if (bb_dfo_id < BB_MAP32_Get(dfo_map, succ)) continue;
      if (!BS_MemberP(BB_dom_set(bb), BB_id(succ))) {
// NOTE: This isn't necessarily a bug:
//	DevWarn("Bad BACK EDGE (from %d to %d of %s) at line %d of %s",
//		BB_id(bb), BB_id(succ), Cur_PU_Name, __LINE__, __FILE__);
	continue;
      }

      if (CFLOW_Trace_Freq)
        fprintf (TFile, "BACK EDGE from %d to %d\n", BB_id(bb), BB_id(succ));

      newloop = TYPE_L_ALLOC (LOOP_DESCR);
      loop_set = BB_SET_Create_Empty (PU_BB_Count + 2, pool);
      BB_SET_Union1D(loop_set, succ, NULL);
      if (bb != succ) BB_Add_Ancestors(&loop_set, bb, bb, pool);
      newloop->mem_pool = pool;
      LOOP_DESCR_bbset(newloop) = loop_set;
      LOOP_DESCR_loophead(newloop) = succ;
      LOOP_DESCR_nestlevel(newloop) = 1;
      LOOP_DESCR_num_exits(newloop) = 0;
      LOOP_DESCR_next(newloop) = NULL;
      Set_BB_innermost(succ);		/* Reset below if not innermost */

      /* Find the loop info for the loop body.
       */
      LOOP_DESCR_loopinfo(newloop) = NULL;
      annot = ANNOT_Get(BB_annotations(succ), ANNOT_LOOPINFO);
      if (annot != NULL) LOOP_DESCR_loopinfo(newloop) = ANNOT_loopinfo(annot);

      lastloop = NULL;
      for (cloop = the_loops; cloop != NULL; cloop = LOOP_DESCR_next(cloop)) 
      {
	/* merge two loops that have the same entry basic block. This 
	   can happen when there are several back edges for a loop.
	   NOTE: This does not handle the case of nested loops with the 
	     same entry basic block. It treats them like a single loop.
        */
	if (LOOP_DESCR_loophead(cloop) == succ) {
	  BB_SET_UnionD(LOOP_DESCR_bbset(newloop), 
			LOOP_DESCR_bbset(cloop), NULL);
	  if (lastloop == NULL) {
	    the_loops = LOOP_DESCR_next(the_loops);
	  }
	  else {
	    LOOP_DESCR_next(lastloop) = LOOP_DESCR_next(cloop);
	  }
	}
	/* If new loop is a subset of a loop already on the list, add
	   it to the list just before that loop. 
        */
	else if (BB_SET_ContainsP (LOOP_DESCR_bbset(cloop), 
				   LOOP_DESCR_bbset(newloop))) 
        {
	  LOOP_DESCR_next(newloop) = 
		  (lastloop == NULL) ? the_loops : LOOP_DESCR_next(lastloop);
	  break;
	}
	else {
	  lastloop = cloop;
	}
      }
      if (lastloop == NULL) {
	the_loops = newloop;
      }
      else {
	LOOP_DESCR_next(lastloop) = newloop;
      }
    }
  }

  /* Figure out the nestlevel for all the loops and setup <LOOP_DESCR_map>
   * so LOOP_DESCR_Find_Loop and LOOP_DESCR_Is_Exit_Edge can work.  Also
   * reset BB_innermost flag for heads of loops enclosing other loops.
   */
  LOOP_DESCR_map = BB_MAP_Create();
  loop_descr_map_initted = TRUE;
  for (cloop = the_loops; cloop != NULL; cloop = LOOP_DESCR_next(cloop)) {
    FOR_ALL_BB_SET_members(LOOP_DESCR_bbset(cloop), bb)
      if (!BB_MAP_Get(LOOP_DESCR_map, bb))
	BB_MAP_Set(LOOP_DESCR_map, bb, cloop);
    for (encl_loop = LOOP_DESCR_next(cloop); 
	 encl_loop != NULL; 
	 encl_loop = LOOP_DESCR_next(encl_loop))
    {
      /* If encl_loop encloses cloop, increment cloop's nestlevel. */
      if (BB_SET_ContainsP (LOOP_DESCR_bbset(encl_loop),
			    LOOP_DESCR_bbset(cloop))) {
	LOOP_DESCR_nestlevel(cloop)++;
	Reset_BB_innermost(LOOP_DESCR_loophead(encl_loop));
      }
    }
  }

  /* Set the nest_level for each basic block. Also count the number of 
   * exits for each loop. If a bb is an exit through more than one level
   * of nesting, assign the exit to the outermost loop it exits.  Also
   * set/reset the BB_innermost flag and the BB_loop_head_bb attribute.
   */
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    if (BB_MAP32_Get(dfo_map, bb) == 0) continue;

    BB_nest_level(bb) = 0;
    cloop = LOOP_DESCR_Find_Loop(bb);
    if (cloop != NULL) {
      BB *head = LOOP_DESCR_loophead(cloop);
      BOOL innermost = BB_innermost(head);
      BB_nest_level(bb) = LOOP_DESCR_nestlevel(cloop);
      if (innermost) Set_BB_innermost(bb);
      else Reset_BB_innermost(bb);
      Set_BB_loop_head_bb(bb, head);
      FOR_ALL_BB_SUCCS (bb, blst) {
        exitloop = LOOP_DESCR_Is_Exit_Edge(bb, BBLIST_item(blst));
	if (exitloop != NULL) {
	  LOOP_DESCR_num_exits(exitloop)++;
        }
      }
    }
  }

  BB_MAP_Delete(dfo_map);

  return the_loops;
}


static void
LOOP_DESCR_Add_BB_Helper (LOOP_DESCR *loop, BB *bb) {

    LOOP_DESCR* l = LOOP_DESCR_Next_Enclosing_Loop(loop);
    if (l) { LOOP_DESCR_Add_BB_Helper (l, bb); }

    LOOP_DESCR_bbset(loop) = BB_SET_Union1D(LOOP_DESCR_bbset(loop), 
                            bb, loop->mem_pool);
}

/* ====================================================================
 * See "findloops.h" for interface description.
 * ====================================================================
 */
void 
LOOP_DESCR_Add_BB(LOOP_DESCR *loop, BB *bb)
{
  DevAssert(LOOP_DESCR_Find_Loop(bb) == NULL,
	    ("BB:%d already in a loop", BB_id(bb)));

  BB_MAP_Set(LOOP_DESCR_map, bb, loop);
  LOOP_DESCR_Add_BB_Helper (loop, bb);
}


/* ====================================================================
 * See "findloops.h" for interface description.
 * ====================================================================
 */
void 
LOOP_DESCR_Delete_BB(LOOP_DESCR *loop, BB *bb)
{
  DevAssert(BB_SET_MemberP(LOOP_DESCR_bbset(loop), bb),
	    ("BB:%d not in <loop>", BB_id(bb)));
  DevAssert(LOOP_DESCR_Find_Loop(bb) == loop,
	    ("<loop> is not innermost containing BB:%d", BB_id(bb)));

  BB_MAP_Set(LOOP_DESCR_map, bb, NULL);

  do {
    LOOP_DESCR_bbset(loop) = BB_SET_Difference1D(LOOP_DESCR_bbset(loop), bb);
    loop = LOOP_DESCR_Next_Enclosing_Loop(loop);
  } while (loop);
}



/* ====================================================================
 * See "findloops.h" for interface description.
 * ====================================================================
 */
LOOP_DESCR*
LOOP_DESCR_Next_Enclosing_Loop(LOOP_DESCR *loop)
{
  /* This relies on loop descriptors occurring in the list innermost first.
   */
  if (LOOP_DESCR_nestlevel(loop) > 1) {
    LOOP_DESCR *enclosing = LOOP_DESCR_next(loop);
    while (enclosing &&
	   (LOOP_DESCR_nestlevel(enclosing) >= LOOP_DESCR_nestlevel(loop) ||
	    !BB_SET_ContainsP(LOOP_DESCR_bbset(enclosing),
			      LOOP_DESCR_bbset(loop))))
      enclosing = LOOP_DESCR_next(enclosing);
    return enclosing;
  } else {
    return NULL;
  }
}

/* ====================================================================
 * See "findloops.h" for interface description.
 * ====================================================================
 */
BB* 
LOOP_DESCR_Find_Unique_Tail(LOOP_DESCR *loop)
{
  BB *tail = NULL;
  BBLIST *preds;
  FOR_ALL_BB_PREDS(LOOP_DESCR_loophead(loop), preds) {
    if (BB_SET_MemberP(LOOP_DESCR_bbset(loop), BBLIST_item(preds))) {
      if (tail == NULL) {
	tail = BBLIST_item(preds);
      } else {
	return NULL;
      }
    }
  }
  return tail;
}

/* ====================================================================
 * Requires: <to> is inserted where intended in BB chain
 *
 * Change all branches from outside of <loop> to its head (except those
 * from <to>) to branch to <to> instead, updating pred/succ lists and
 * frequency info.
 * ====================================================================
 */
void 
LOOP_DESCR_Retarget_Loop_Entrances(LOOP_DESCR *loop, BB *to)
{
  BB *head = LOOP_DESCR_loophead(loop);
  BBLIST *preds = BB_preds(head);
  while (preds) {
    BB *pred = BBLIST_item(preds);
    preds = BBLIST_next(preds);
    if (!BB_SET_MemberP(LOOP_DESCR_bbset(loop), pred))
      if (!BB_Retarget_Branch(pred, head, to))
	/* must fall-through to <head> */
	Change_Succ(pred, head, to);
  }
}

/* ====================================================================
 * See "findloops.h" for interface description.
 * ====================================================================
 */
BOOL
LOOP_DESCR_Can_Retarget_Loop_Entrances(LOOP_DESCR *loop)
{
  BB *head = LOOP_DESCR_loophead(loop);
  BBLIST *preds = BB_preds(head);
  while (preds) {
    BB *pred = BBLIST_item(preds);
    preds = BBLIST_next(preds);
    if (!BB_SET_MemberP(LOOP_DESCR_bbset(loop), pred)) {
      if (BB_next(pred) == head) continue;  // bb falls through to head
      if (!BB_Can_Retarget_Branch(pred, head)) return FALSE;
    }
  }
  return TRUE;
}

/* ====================================================================
 * See "findloops.h" for interface description.
 * ====================================================================
 */
float 
LOOP_DESCR_Estimate_Cycles(LOOP_DESCR *loop)
{
  BB *bb;
  float avg_cycles;
  BB *head = LOOP_DESCR_loophead(loop);

  BB_MAP sch_est = BB_MAP_Create();
  FOR_ALL_BB_SET_members(LOOP_DESCR_bbset(loop), bb) {
    CG_SCHED_EST *se =
      CG_SCHED_EST_Create(bb, &MEM_local_nz_pool, SCHED_EST_FOR_SWP);
    BB_MAP_Set(sch_est, bb, se);
  }

  avg_cycles = CG_SCHED_EST_Avg_Cycles_Thru(LOOP_DESCR_bbset(loop), head, sch_est, SCHED_EST_FOR_SWP);

  return avg_cycles;
}

/* ====================================================================
 * See "findloops.h" for interface description.
 * ====================================================================
 */
BOOL
LOOP_DESCR_Has_Side_Entrance(LOOP_DESCR *loop)
{
  BB* bb;
  BB* pred;
  BBLIST *lst;

  FOR_ALL_BB_SET_members(LOOP_DESCR_bbset(loop), bb) {
    BB* head = LOOP_DESCR_loophead(loop);
    if (bb == head) continue;
    FOR_ALL_BB_PREDS (bb, lst) {
      pred = BBLIST_item(lst);
      if (!BB_SET_MemberP(LOOP_DESCR_bbset(loop), pred)) {
	return TRUE;
      }
    }
  }
  return FALSE;
}
