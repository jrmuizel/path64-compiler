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

/* ====================================================================
 *   CGTARG_Copy_Operand
 *
 *   TODO: generate automatically ?? at leats some obvious ones
 *         coherently with the isa property ?
 * ====================================================================
 */
INT 
CGTARG_Copy_Operand (
  OP *op
)
{
  TOP opr = OP_code(op);
  switch (opr) {

  // NOTE: TOP_fandcm, TOP_for, and TOP_fxor could be handled like
  // their integer counterparts should that ever become useful.

  case TOP_GP32_ADD_GT_DR_DR_U8:
    //case TOP_or:
    //case TOP_xor:
    //case TOP_sub:
    //case TOP_shl_i:
    //case TOP_shr_i:
    if (TN_has_value(OP_opnd(op,2)) && TN_value(OP_opnd(op,2)) == 0) {
      return 1;
    }
    break;

  case TOP_GP32_COPYA_GT_AR_DR:
  case TOP_GP32_COPYC_GT_CRL_DR:
  case TOP_GP32_COPYD_GT_DR_AR:

  case TOP_GP32_MOVEA_GT_AR_AR:
  case TOP_GP32_MOVEG_GT_BR_BR:
  case TOP_GP32_MOVE_GT_DR_DR:
    //TOP_GP32_MOVEHH_GT_DR_DR,
    //TOP_GP32_MOVEHL_GT_DR_DR,
    //TOP_GP32_MOVELH_GT_DR_DR,
    //TOP_GP32_MOVELL_GT_DR_DR,
    //TOP_GP32_MOVEP_GT_DR_DR,
    return 1;

  }
  return -1;
}

/* ====================================================================
 *   CGTARG_Init_OP_cond_def_kind
 * ====================================================================
 */
void 
CGTARG_Init_OP_cond_def_kind (
  OP *op
)
{
  TOP top = OP_code(op);
  switch (top) {

    // The following OPs unconditionally define the predicate results.
    //
  case TOP_noop:

    Set_OP_cond_def_kind(op, OP_ALWAYS_UNC_DEF);
    break;

    // The following OPs do not always update the result predicates.
  case TOP_label:

    Set_OP_cond_def_kind(op, OP_ALWAYS_COND_DEF);
    break;

  default:
    if (OP_has_predicate(op))
      Set_OP_cond_def_kind(op, OP_PREDICATED_DEF);
    else
      Set_OP_cond_def_kind(op, OP_ALWAYS_UNC_DEF);
    break;
  }
}

