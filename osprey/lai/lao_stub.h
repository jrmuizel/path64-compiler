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

extern "C" {
#include "LAO_Driver.h"
}

// Optimize a LOOP_DESCR through the LAO.
bool
lao_optimize(CG_LOOP *cg_loop, unsigned lao_actions);

// Optimize a HB through the LAO.
bool
lao_optimize(HB *hb, unsigned lao_actions);

// Optimize a Function through the LAO.
bool
lao_optimize(unsigned lao_actions);

#endif /* laostub_INCLUDED */
