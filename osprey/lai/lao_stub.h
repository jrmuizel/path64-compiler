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

// Must be IDENTICAL to enum LOOP_OPT_ACTION in lai/cg_loop.cxx
enum LAO_SWP_ACTION {
  NO_LOOP_OPT,
  SINGLE_BB_DOLOOP_SWP,
  SINGLE_BB_DOLOOP_UNROLL,
  SINGLE_BB_WHILELOOP_SWP,
  SINGLE_BB_WHILELOOP_UNROLL,
  MULTI_BB_DOLOOP
};

void LAOS_printCGIR();

bool LAO_scheduleRegion ( BB ** entryBBs, BB ** exitBBs, BB ** regionBBs, LAO_SWP_ACTION action );

bool Perform_SWP ( CG_LOOP& cl, LAO_SWP_ACTION action );

#endif /* laostub_INCLUDED */
