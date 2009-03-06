/*

  Copyright (C) 2002 ST Microelectronics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/

#define USE_STANDARD_TYPES
// [HK]
#if __GNUC__ >= 3
#include <map>
using std::map;
#else
#include <map.h>
#endif // __GNUC__ >= 3
#include "defs.h"
#include "mempool.h"
#include "tn.h"
#include "tn_set.h"
#include "bb.h"
#include "op.h"
#include "op_list.h"
#include "op_map.h"
#include "cgexp.h"
#include "cgtarget.h"
#include "register.h"
#include "cg_loop.h"
#include "cg_swp_options.h"
#include "cg_swp.h"
#include "cg_swp_target.h"
#include "tracing.h"
#include "pf_cg.h"
#include "cg_loop.h"
#include "calls.h"
#include "tag.h"

/* ====================================================================
 *   OP_base_update_kind
 *
 *   TODO: see if this can be shared with the code in targ_op.cxx
 * ====================================================================
 */
BASE_UPDATE 
OP_base_update_kind (
  OP *op
)
{
  return NO_BASE_UPDATE;
}

/* ====================================================================
 *   Expand_Simulated_Ops
 *
 *   Get rid of OPs that are not real.
 * ====================================================================
 */
static BOOL Expand_Simulated_Ops(
  CG_LOOP& cl, 
  bool trace
)
{
  BB *body = cl.Loop_header();
  OP *op;
  FOR_ALL_BB_OPs(body, op) {
    TOP top = OP_code(op);
    if (OP_simulated(op) && top != TOP_asm) {
      if (trace) 
	fprintf(TFile, "<swp> expand simulated op %s\n", TOP_Name(top));
      switch (top) {
      default:
	DevWarn("SWP: support of simulated OP %s not implemented.", TOP_Name(top));
	return FALSE;
      }
    }
  }
  return TRUE;
}

/* ====================================================================
 *
 *  Prepare_Loop_For_SWP
 *    See interface in cg_swp_target.h
 *    This function is called before building CG_LOOP_INFO
 *
 * ====================================================================
 */
BOOL Prepare_Loop_For_SWP_1(CG_LOOP& cl, bool trace)
{
  if (!Expand_Simulated_Ops(cl, trace))
    return FALSE;

  if (!Remove_Non_Definite_Dependence(cl, false/*no CG_LOOP_Init_OP*/, trace))
    return FALSE;

#if 0
  Remove_Invariant_Predicates(cl, trace);

  Unc_Promotion(cl, trace);

  Hoist_MOVL(cl, trace);

  Init_OP_no_ci_alias(cl, trace);
#endif

  return TRUE;
}


/* ====================================================================
 *
 *  Prepare_Loop_For_SWP
 *    See interface in cg_swp_target.h
 *    This function is called after building CG_LOOP_INFO
 *
 * ====================================================================
 */
BOOL Prepare_Loop_For_SWP_2(CG_LOOP& cl, bool trace)
{
#if 0
  if (SWP_Options.Enable_Post_Incr)
    Gen_Post_Incr_Memop(cl, trace);
#endif

  if (!Remove_Non_Definite_Dependence(cl, true/*need CG_LOOP_Init_OP*/, trace))
    return FALSE;

  return TRUE;
}

/* ====================================================================
 *   SWP_Loop_Init_Fini
 *
 *   Generate the branch operation needed by SWP. Other target-specific
 *   init/fini stuff (rotating registers stuff), etc.
 * ====================================================================
 */
void
SWP_Loop_Init_Fini(bool is_doloop,
		   INT stage_count,
		   OPS *prolog_ops,
		   OPS *body_ops,
		   OPS *epilog_ops)
{ 
  FmtAssert(FALSE,("not implemented"));
}
