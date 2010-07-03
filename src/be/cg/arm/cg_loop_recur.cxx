
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
*/

#include "defs.h"
#include "tracing.h"
#include "cg_flags.h"
#include "tn.h"
#include "tn_set.h"
#include "op.h"
#include "bb.h"
#include "cg.h"
#include "cgprep.h"
#include "cg_loop.h"
#include "cg_loop_mii.h"
#include "cg_loop_recur.h"
#include "cg_dep_graph.h"
#include "cg_sched_est.h"
#include "cg_swp.h"
#include "cg_swp_target.h"
#include "cgexp.h"
#include "const.h"
#include "ti_res_count.h"

// Arthur: some target-specific stuff from be/cg/ia64/cg_loop_recur.cxx

/* =======================================================================
 *   Mtype_from_opc
 * =======================================================================
 */
// Return mtype
TYPE_ID 
Mtype_from_opc(TOP top)
{
  FmtAssert(FALSE,("Not implemented"));
#if 0
  
  switch (top) {
  case TOP_sub_r:
  case TOP_add_r:
    return (MTYPE_I4);
    break;
  }

  Is_True(FALSE, ("Mtype_from_opc:  unknown opcode."));

#endif
  return MTYPE_UNKNOWN;
}

/* =======================================================================
 *   RECUR_OP_DESC::Init_OP
 *
 *   Initialize the target-specific recurrence info for a given 'op'
 * =======================================================================
 */
void
RECUR_OP_DESC::Init_OP ()
#if 0
  OP *op,
  INT *has_one_invar_opnd, 
  BOOL *is_add,
  INT *invar_opnd_num,
  INT *second_invar_opnd_num,
  INT *reg_opnd_num,
  INT *res_num
) 
#endif
{

  FmtAssert(FALSE,("Not implemented"));
#if 0


  switch (OP_code(op)) {
#if 0
    case TOP_fma:
    case TOP_fma_d:
    case TOP_fma_s:
      has_one_invar_opnd = false;
      if (Roundoff_Level >= ROUNDOFF_ASSOC) {
	is_add = true;
	if (OP_result(op, 0) == OP_opnd(op, 4)) {
	  invar_opnd_num = 2;
	  second_invar_opnd_num = 3;
	  reg_opnd_num = 4;
	  res_num = 0;
	}
	mtype = Mtype_from_opc(OP_code(op));
      }
      break;
	
    case TOP_fnma:
    case TOP_fnma_d:
    case TOP_fnma_s:
      has_one_invar_opnd = false;
      if (Roundoff_Level >= ROUNDOFF_ASSOC) {
	if (OP_result(op, 0) == OP_opnd(op, 4)) {
	  invar_opnd_num = 2;
	  second_invar_opnd_num = 3;
	  reg_opnd_num = 4;
	  res_num = 0;
	}
	mtype = Mtype_from_opc(OP_code(op));
      }
      break;
	
    case TOP_fadd:
    case TOP_fadd_d:
    case TOP_fadd_s:
      allow_back_sub_variant = true;
      if (Roundoff_Level >= ROUNDOFF_ASSOC) {
	is_add = true;
	if (OP_result(op, 0) == OP_opnd(op, 3)) {
	  invar_opnd_num = 2;
	  reg_opnd_num = 3;
	  res_num = 0;
	} else if (OP_result(op, 0) == OP_opnd(op, 2)) {
	  invar_opnd_num = 3;
	  reg_opnd_num = 2;
	  res_num = 0;
	}
	mtype = Mtype_from_opc(OP_code(op));
      }
      break;
	
    case TOP_fsub:
    case TOP_fsub_d:
    case TOP_fsub_s:
      allow_back_sub_variant = true;
      if (Roundoff_Level >= ROUNDOFF_ASSOC) {
	is_add = false;
	if (OP_result(op, 0) == OP_opnd(op, 2)) {
	  invar_opnd_num = 3;
	  reg_opnd_num = 2;
	  res_num = 0;
	} 
	mtype = Mtype_from_opc(OP_code(op));
      } 
      break;
#endif

    case TOP_sub_r:
    //is_add = false; 
      if (OP_result(op, 0) == OP_opnd(op, 1)) {
	invar_opnd_num = 2;
	reg_opnd_num = 1;
	res_num = 0;
      }
      mtype = Mtype_from_opc(OP_code(op));
      break;

    case TOP_add_r:
    //is_add = true;
      if (OP_result(op, 0) == OP_opnd(op, 2)) {
	invar_opnd_num = 1;
	reg_opnd_num = 2;
	res_num = 0;
      } else if (OP_result(op, 0) == OP_opnd(op, 1)) {
	invar_opnd_num = 2;
	reg_opnd_num = 1;
	res_num = 0;
      }
      mtype = Mtype_from_opc(OP_code(op));
      break;
  }

#endif
  return;
}

/* =======================================================================
 *   OP_Apply_Back_Sub_Invariant
 * =======================================================================
 */
void
OP_Apply_Back_Sub_Invariant (
  OP *op
)
{
  // Update loop body
  switch (OP_code(op)) {
#if 0
  case TOP_add:
  case TOP_sub:
    {
      TN *new_invar_tn = Dup_TN(OP_result(op, res_num));
      Exp_MPY(mtype, new_invar_tn, invar_tn, Gen_Literal_TN(new_omega, 4), &prolog_ops);
      Set_OP_opnd(op, invar_opnd_num, new_invar_tn);
      break;
    }

  case TOP_adds:
    {
      INT64 imm = TN_value(invar_tn);
      TN *new_imm_tn = Gen_Literal_TN(imm * new_omega, 4);
      if (TOP_Can_Have_Immediate(imm * new_omega, OP_code(op)))
	Set_OP_opnd(op, invar_opnd_num, new_imm_tn);
      else {
	// generate TOP_add and initialization at prolog
	OP_Change_Opcode(op, TOP_add);
	TN *new_tn = Dup_TN(OP_result(op, res_num));
	Set_OP_opnd(op, invar_opnd_num, new_tn);
	Exp_COPY(new_tn, new_imm_tn, &prolog_ops);
      }
      break;
    }
#endif

  default:
    Is_True(FALSE, ("Interleave_Associative_Op: op not supported."));
  }

  return;
}

/* =======================================================================
 *   OP_Apply_Back_Sub_Variant
 * =======================================================================
 */
BOOL
OP_Apply_Back_Sub_Variant (
  OP *op
)
{
  // Update loop body
  switch (OP_code(op)) {
#if 0
  case ??:
    return TRUE; // BB changed
#endif
  default:
    Is_True(FALSE, ("Apply_Back_Sub_Variant: op not supported."));
  }

  return FALSE;
}

