
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
#include <vector.h>
#include <list>
#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "matrix.h"
#include "mempool_allocator.h"
#include "glob.h"    // for Cur_PU_Name
#include "op.h"
#include "cg.h"
#include "cg_loop.h"
#include "cgtarget.h"
#include "ti_si.h"
#include "cgprep.h"
#include "cg_swp.h"
#include "cg_swp_options.h"
#include "cg_swp_target.h"
#include "cg_swp_bundle.h"

// Arthur: seems like some functionality is target-independent

/* ====================================================================
 *   SWP_Dont_Bundle
 * ====================================================================
 */
void 
SWP_Dont_Bundle(SWP_OP_vector& op_state)
{
  // For register allocation purposes, assume we have one slot per
  // cycle
  //
  op_state.ii_slots = op_state.ii;
   for (INT i = 0; i < op_state.size(); i++) {
     if (op_state[i].op) {
       op_state[i].modulo_cycle = op_state[i].cycle % op_state.ii;
       op_state[i].slot = op_state[i].modulo_cycle;
     }
   }
} // SWP_Dont_Bundle

/* ====================================================================
 *   SWP_Undo_Bundle
 * ====================================================================
 */
void
SWP_Undo_Bundle(SWP_OP_vector& op_state, BB *body)
{
  for (INT i = 0; i < op_state.size(); i++) {
    if (op_state[i].is_noop && op_state[i].op) {
      BB_Remove_Op(body, op_state[i].op);
      op_state[i].op = NULL;
      op_state[i].is_noop = FALSE;
    }
    if (op_state[i].op) {
      Reset_OP_end_group(op_state[i].op);
      Reset_OP_bundled(op_state[i].op);
    }
  }
}
