/**
***			Interface to/from the LAO
***			-------------------------
***
*** Description:
***
***	This is the interface between the Open64/CGIR and the LAO/LIR
***	representations.
***
**/

#ifndef laostub_INCLUDED
#define laostub_INCLUDED

#include <math.h>
#include <stdarg.h>
#include <set.h>

#include "defs.h"
#include "config.h"
#include "errors.h"
#include "cg_loop.h"
#include "hb.h"

// Replicates enum CodeRegion_Action in LIR_CodeRegion.h
enum LAO_Action {
  LAO_NoAction = 0,
  LAO_BlockSchedule = 1,	// super block scheduling
  LAO_TraceSchedule = 2,	// trace block scheduling
  LAO_LoopSchedule = 4,		// scheduling with loop-carried dependences
  LAO_LoopPipeline = 8,		// loop pipelining with modulo scheduling
  LAO_LoopUnwind = 16,		// modulo expansion if LoopPipeline
  LAO_LoopUnroll = 32,		// counted loop unrolling
  LAO_Recurrences = 64,		// recurrence rewriting
  LAO_Reductions = 128,		// reduction splitting
  LAO_PostPass = 256,		// postpass scheduling
  LAO_RegAlloc = 512,		// register allocate
};

// Optimize a LOOP_DESCR through the LAO.
bool
LAO_optimize(CG_LOOP *cg_loop, unsigned lao_actions);

// Optimize a HB through the LAO.
bool
LAO_optimize(HB *hb, unsigned lao_actions);

// Optimize a Function through the LAO.
bool
LAO_optimize(unsigned lao_actions);

#endif /* laostub_INCLUDED */
