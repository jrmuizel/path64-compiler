/*
 * Copyright 2009 SiCortex, Inc.
 */

/* 
 *  Copyright (C) 2008  PathScale, LLC. All Rights Reserved.
 *
   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
 */

#ifndef cg_swp_options_INCLUDED
#define cg_swp_options_INCLUDED "cg_swp_options.h"

#include "defs.h"
#include "cg_flags.h"



#ifdef TARG_ST
struct SWP_OPTIONS {

  // Options accessible from command line (See cgdriver.cxx)
  //
  INT32 Sched_Direction;
  INT32 Heuristics;
  INT32 Opt_Level;
  BOOL  Enable_While_Loop;
  BOOL  Enable_Bundling;
  BOOL  Enable_Post_Incr;
  INT32 Min_Unroll_Times;
  INT32 Max_Unroll_Times;
  INT32 Min_Unroll_Times_Set;
  INT32 Max_Unroll_Times_Set;
  INT32 Critical_Threshold;
  BOOL  Prep_Only;
  BOOL  Min_Retry; 
  BOOL  Implicit_Prefetch;
  BOOL  Implicit_Prefetch_Set;
  BOOL  Predicate_Promotion;
  BOOL  Enable_BRP;

  // Options not accessible from command line
  //  -  some are basically compile-time constants;
  //  -  some are derivatives of the command line options.
  //     e.g. opt_level controls the Budget and Max_II, II_Incr.
  //
  INT32 Budget;
  INT32 Max_Schedule_Incr;
  double Max_II_Alpha;
  double Max_II_Beta;
  double II_Incr_Alpha;
  double II_Incr_Beta;
  INT32 Grainy_Resources_Length;

  // Debugging options
  BOOL Enable_Workaround;
  INT32 Starting_II;

  // Constructor to setup default values
  //
  SWP_OPTIONS() 
  {
    // Scheduling Direction
    //   0 - bidirection
    //   1 - top-down
    //   2 - bottom-up
    Sched_Direction = 0; 

    // SWP Heuristics
    //   0 - resource-scaled slack 
    //   1 - lstart
    //   2 - (-estart)
    Heuristics = 0;

    // SWP Opt Level
    //   0 - fast (cannot exceed the initial estart/lstart range)
    //   1 - slow (can push start/stop to earlier/later cycles)
    //   2 - even slower
    //   3 - 
    //    
    //   The amount of trials for each OP is SWP_Budget * Opt_Level
    //  
    Opt_Level = 2;

    // Enable SWP of while-loop
    //  - will be disabled in PU_Configure() if the architecture
    //    does not support while-loop SWP.
    //
    Enable_While_Loop = TRUE;

    // Enable SWP bundling and grouping of operations to minimize the number
    // of cycles in a modulo scheduled loop-kernel:
    //  - has no effect for architectures where bundling is not an issue.
    //
    Enable_Bundling = TRUE;

    // Budget
    Budget = 10;

    // Maximum changes of START/STOP cycles
    Max_Schedule_Incr = 20;
  
    // Max II limit and II increments for failed SWP
    Max_II_Alpha = 2;
    Max_II_Beta  = 2.0;

    // control II Incr amount
    II_Incr_Alpha = -10;
    II_Incr_Beta = 1.1;

    // Grainy Resources
    //   OPs that uses more than 'Grainy_Resources_Length' are considered
    //   difficult to schedule.  Therefore they are given priority to schedule.
    Grainy_Resources_Length = 10;

    // default max unrolling == 8
    Min_Unroll_Times = 1;
    Max_Unroll_Times = (CG_opt_level > 2) ? 8 : 4;  

    // enable postincr form
    Enable_Post_Incr = TRUE;

    // For debugging - hardware/simulator workaround
    Enable_Workaround = FALSE;

    // For debugging - use Starting_II as the first II for scheduling
    Starting_II = 0;

    // if a resource is 90% utilized, it is 
    // considered critical
    Critical_Threshold = 90;

    // Execute the SWP preparation, but skip the modulo scheduler
    Prep_Only = FALSE;

    // Minimize retry / backtracking at the slight expense of
    // register presure
    Min_Retry = TRUE;

    // Use implicit prefetch (will be disabled if target doesn't support)
    Implicit_Prefetch = TRUE;
    
    // Predicate promotion
    Predicate_Promotion = TRUE;

    // Generation of branch predict instructions (brp.loop.imp)
    Enable_BRP = TRUE;
  }

  // PU Configure:
  //   Modify options based on target and loop information
  //
  void PU_Configure();

};
#else

// SWP_Default is initialized by the default SWP_OPTION constructor
// and then modified by the command line options
//
class SWP_OPTIONS {
  public:
  BOOL  Enable_Verbose_Mode;
  INT32 Sched_Direction;
  //INT32 Heuristics;
  //INT32 Opt_Level;
  BOOL  Enable_While_Loop;
  BOOL  Enable_Jump_Block;
  BOOL  Enable_Offset_Adjustment;
  INT32 Offset_Skip_Before;
  INT32 Offset_Skip_Equal;
  INT32 Offset_Skip_After;
  INT32 Min_Unroll_Times;
  INT32 Max_Unroll_Times;
  INT32 Min_Unroll_Times_Set;
  INT32 Max_Unroll_Times_Set;
  BOOL  Pow2_Unroll_Times;
  INT32 Critical_Threshold;
  BOOL  Implicit_Prefetch;
  BOOL  Implicit_Prefetch_Set;
  BOOL  Predicate_Promotion;
  INT32 Starting_II;
  INT   Max_II;
  INT32 Budget;
  INT32 Budget_Set;
  INT32 Min_Trip_Count;
  INT32 Min_Trip_Count_Set;
  INT32 Linear_Search_Limit;
  INT32 MaxII_Factor;
  

  public:
  SWP_OPTIONS (void);
  ~SWP_OPTIONS (void) { return; }

  void 
  PU_Configure (void);
};
#endif

extern SWP_OPTIONS SWP_Options;

#endif /* cg_swp_options_INCLUDED */

