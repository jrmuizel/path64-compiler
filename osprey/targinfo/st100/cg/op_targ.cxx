/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/

/* ====================================================================
 * ====================================================================
 *
 *       OP utility routines which include target dependencies.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "config.h"
#include "erglob.h"
#include "xstats.h"
#include "tracing.h"

#include "cgir.h"

#include "targ_isa_subset.h"
#include "targ_isa_operands.h"
#include "targ_isa_properties.h"

/* ====================================================================
 *   CGTARG_Predicate_OP
 *
 *   If a BB ends in an unconditional branch, turn it into a 
 *   conditional branch with TRUE predicate, so we can predicate with 
 *   something else later.
 *   If we can't find an unconditional branch, just give up and do 
 *   nothing
 * ====================================================================
 */
void
Make_Branch_Conditional (
  BB *bb
)
{
  OP *new_br;
  TOP new_top;
  OP* br_op = BB_branch_op(bb);

  if (!br_op) return;

  FmtAssert(FALSE,("Make_Branch_Conditional: not implemented"));

  /*
  switch (OP_code(br_op)) {
  case TOP_br:
    new_top = TOP_br_cond;
    break;
  case TOP_br_r:
    new_top = TOP_br_r_cond;
    break;
  default:
    return;
  }

  new_br = Mk_OP(new_top,
		 True_TN,
		 Gen_Enum_TN(ECV_bwh_dptk),
		 Gen_Enum_TN(ECV_ph_few),
		 Gen_Enum_TN(ECV_dh),
		 OP_opnd(br_op,2));
  */

  OP_srcpos(new_br) = OP_srcpos(br_op);
  BB_Insert_Op_After(bb, br_op, new_br);
  BB_Remove_Op(bb, br_op);
}

/* ====================================================================
 *   CGTARG_Predicate_OP
 * ====================================================================
 */
void
CGTARG_Predicate_OP (
  BB* bb, 
  OP* op, 
  TN* pred_tn
)
{
  if (OP_has_predicate(op)) {
    Set_OP_opnd(op, OP_PREDICATE_OPND, pred_tn);
  }
}

