/* coverage.h - Defines data exported from coverage.c
   Copyright (C) 1998, 1999, 2000, 2001, 2003 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef GCOV_COVERAGE_H
#define GCOV_COVERAGE_H
#include "pu_info.h"
#include "bb.h"

/* Control flow edge information.  */
typedef struct edge_def {
  /* The two blocks at the ends of the edge.  */
  BB *src, *dest;

  unsigned int to_exit : 1; // 1 if the edge points to the gcov exit (in this case dest == NULL)
  unsigned int from_entry : 1; // 1 if the edge is from the gcov entry (in this case src == NULL)
  unsigned int on_tree : 1;
  unsigned int fake : 1;
  unsigned int fall_through : 1;
  unsigned int critical : 1;
} *edge_t;

/* This structure maintains an edge list vector.  */
struct edge_list
{
  int num_edges;
  edge_t *index_to_edge;
};


extern void coverage_init (const char *);

/* Finish gcno generation + instrument code */
/* Performs file-level cleanup.  Generate coverage
   variables and constructor.  */
extern void coverage_finish (FILE */* file */, unsigned /* num_instr_edges */);

/* Complete the coverage information for the current function. Once
   per function.  */
extern void coverage_end_function (void);

/* Start outputting coverage information for the current
   function. Repeatable per function.  */
extern int coverage_begin_output (ST */* pu */, BB */* bb */);

/* 
   This function writes basic block part of '.gcno' file for the
current function.
*/
extern void coverage_blocks ();

/* For each BB in the CFG, create one gcov EDGE 
 * for each successor.
 */
extern unsigned coverage_arcs (struct edge_list */* ed_list */);

/* for a given BB in the CFG, add gcov line info 
 */
extern void coverage_lines (BB */* bb */);

/* Allocate NUM counters of type COUNTER. Returns nonzero if the
   allocation succeeded.  */
extern int coverage_counter_alloc (unsigned counter, unsigned num);

/* Output instructions  to increment the edge execution count.  */
extern void gen_edge_profiler (unsigned /* no */, OPS */* New_OPs */ );
/* Output conditional instructions to increment the edge execution
   count. This used to add a conter on critical edge*/
extern void gen_critical_edge_profiler (unsigned no, OPS *New_OPs, 
					BB *src, BB *dst);

#endif
