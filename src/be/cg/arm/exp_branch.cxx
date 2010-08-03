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


/* EXP routines for expanding branches */

#include "defs.h"
#include "erglob.h"
#include "ercg.h"
#include "tracing.h"
#include "config.h"
#include "config_TARG.h"
#include "topcode.h"
#include "tn.h"
#include "targ_isa_lits.h"
#include "cg_flags.h"
#include "op.h"
#include "cgexp.h"
#include "targ_sim.h"

/* ====================================================================
 *   Initialize_Branch_Variants
 * ====================================================================
 */
void
Initialize_Branch_Variants(void)
{
	// nothing to do
}

/* Import from exp_targ.cxx */
extern TN *Expand_Or_Inline_Immediate( OPERATION_WITH_INLINED_IMM operation, TN *src, TYPE_ID mtype, OPS *ops);

/* ====================================================================
 *   Pick_Compare_TOP
 *
 *   Check that compare is of proper form, and return TOP to use for 
 *   the compare.
 *   May modify the variant and src tns.
 *
 *   Modify the is_integer parameter to reflect if the top result is
 *   a branch or an int.
 * ====================================================================
 */
TOP
Pick_Compare_TOP (
  VARIANT *variant, 
  TN **src1, 
  TN **src2, 
  TN **cond,
  OPS *ops
)
{
  TOP     cmp = TOP_UNDEFINED;
  TOP     cmp_i = TOP_UNDEFINED;
  TYPE_ID mtype;
  ISA_REGISTER_CLASS src1_rc = ISA_REGISTER_CLASS_UNDEFINED;
  ISA_REGISTER_CLASS src2_rc = ISA_REGISTER_CLASS_UNDEFINED;

  /* 
   * If first operand is an immediate value, swap it with the
   * second:
   */
  if (*src1 != NULL && TN_has_value(*src1)) {
    // swap operands and change variant
    TN *tmp = *src1;
   *src1 = *src2;
    *src2 = tmp;
    *variant = Invert_BR_Variant(*variant);
  }

  if (*src1 != NULL && TN_is_constant(*src1)) {
    *src1 = Expand_Immediate_Into_Register (MTYPE_I4, *src1, ops);
  }
  if (*src2 != NULL && TN_is_constant(*src2)) {
    *src2 = Expand_Or_Inline_Immediate(INL_COMPAR,*src2, MTYPE_I4, ops);
  }

  src1_rc = *src1 != NULL ? TN_register_class(*src1) : ISA_REGISTER_CLASS_UNDEFINED;
  src2_rc = *src2 != NULL && TN_is_register(*src2) ? TN_register_class(*src2) : ISA_REGISTER_CLASS_UNDEFINED;


  // On ARM, comparison result is stored in condition code flags N, Z, C and V
  switch (*variant) {
    // Float Variant
    case V_BR_FGE:
    case V_BR_FGT:
    case V_BR_FLE:
    case V_BR_FLT:
    case V_BR_FEQ:
    case V_BR_FNE:
#if 0
      if(src1_rc != ISA_REGISTER_CLASS_UNDEFINED && src1_rc != ISA_REGISTER_CLASS_fpr) {
	TN *tmp = Build_RCLASS_TN(ISA_REGISTER_CLASS_fpr);
	Expand_Copy(tmp, True_TN, *src1, ops);
	*src1 = tmp;
      }
      if(src2_rc != ISA_REGISTER_CLASS_UNDEFINED && src2_rc != ISA_REGISTER_CLASS_fpr) {
	TN *tmp = Build_RCLASS_TN(ISA_REGISTER_CLASS_fpr);
	Expand_Copy(tmp, True_TN, *src2, ops);
	*src2 = tmp;
      }
      *is_integer = TRUE;
      break;
#endif
  default:
    // Int variant
      if(src1_rc != ISA_REGISTER_CLASS_UNDEFINED && src1_rc != ISA_REGISTER_CLASS_integer) {
	TN *tmp = Build_RCLASS_TN(ISA_REGISTER_CLASS_integer);
	Expand_Copy(tmp, True_TN, *src1, ops);
	*src1 = tmp;
      }
      if(src2_rc != ISA_REGISTER_CLASS_UNDEFINED && src2_rc != ISA_REGISTER_CLASS_integer) {
	TN *tmp = Build_RCLASS_TN(ISA_REGISTER_CLASS_integer);
	Expand_Copy(tmp, True_TN, *src2, ops);
	*src2 = tmp;
      }
    break;
  }

  
  // pick tops
  switch (*variant) {

    case V_BR_I4GE:
      cmp = TOP_cmp_r_r;
      *cond = Gen_Enum_TN(ISA_ECV_cond_ge);
      mtype = MTYPE_I4;
      break;

    case V_BR_I4GT:
      cmp = TOP_cmp_r_r;
      *cond = Gen_Enum_TN(ISA_ECV_cond_gt);
      mtype = MTYPE_I4;
      break;

    case V_BR_I4LE:
      cmp = TOP_cmp_r_r;
      *cond = Gen_Enum_TN(ISA_ECV_cond_le);
      mtype = MTYPE_I4;
      break;

    case V_BR_I4LT:
      cmp = TOP_cmp_r_r;
      *cond = Gen_Enum_TN(ISA_ECV_cond_lt);
      mtype = MTYPE_I4;
      break;

    case V_BR_I4EQ:
      cmp = TOP_cmp_r_r;
      *cond = Gen_Enum_TN(ISA_ECV_cond_eq);
      mtype = MTYPE_I4;
      break;

    case V_BR_I4NE:
      cmp = TOP_cmp_r_r;
      *cond = Gen_Enum_TN(ISA_ECV_cond_ne);
      mtype = MTYPE_I4;
      break;

    case V_BR_FGE:
    case V_BR_FGT:
    case V_BR_FLE:
    case V_BR_FLT:
    case V_BR_FEQ:
    case V_BR_FNE:
      break;

    case V_BR_U4GE:	
      cmp = TOP_cmp_r_r;
      *cond = Gen_Enum_TN(ISA_ECV_cond_pl);
      mtype = MTYPE_U4;
      break;

    case V_BR_U4LE:
    case V_BR_A4LE:
      cmp = TOP_cmp_r_r;
      *cond = Gen_Enum_TN(ISA_ECV_cond_ls);
      mtype = MTYPE_U4;
      break;

    case V_BR_U4LT:
    case V_BR_A4LT:
      cmp = TOP_cmp_r_r;
      *cond = Gen_Enum_TN(ISA_ECV_cond_mi);
      mtype = MTYPE_U4;
      break;

    case V_BR_U4EQ:
    case V_BR_A4EQ:
      cmp = TOP_cmp_r_r;
      *cond = Gen_Enum_TN(ISA_ECV_cond_eq);
      mtype = MTYPE_U4;
      break;

    case V_BR_U4NE:
    case V_BR_A4NE:
      cmp = TOP_cmp_r_r;
      *cond = Gen_Enum_TN(ISA_ECV_cond_ne);
      mtype = MTYPE_U4;
      break;

    case V_BR_U4GT:
    case V_BR_A4GT:
      cmp = TOP_cmp_r_r;
      *cond = Gen_Enum_TN(ISA_ECV_cond_hi);
      mtype = MTYPE_U4;
      break;

    case V_BR_DGE:
    case V_BR_DGT:
    case V_BR_DLE:
    case V_BR_DLT:
    case V_BR_DEQ:
    case V_BR_DNE:
      /* Undefined on STxP70. */
      break;

    case V_BR_I8GE:
    case V_BR_I8GT:	
    case V_BR_I8LE:	
    case V_BR_I8LT:	
    case V_BR_U8GE:	
    case V_BR_U8GT:	
    case V_BR_U8LE:	
    case V_BR_U8LT:	
    case V_BR_I8EQ:	
    case V_BR_U8EQ:	
    case V_BR_I8NE:	
    case V_BR_U8NE:	
      /* Undefined on STxP70. */
      break;
    default:	
      break;
  }
  
  if (cmp != TOP_UNDEFINED) {
    // if src2 is immediate, get the immediate form
    if (*src2 != NULL && TN_has_value(*src2)) {
      TOP cmp_i;
      cmp_i = TOP_opnd_immediate_variant(cmp, 1, TN_value(*src2));
      if (cmp_i != TOP_UNDEFINED) {
        return cmp_i;
      } else {
	*src2 = Expand_Immediate_Into_Register(mtype, *src2, ops);
	return cmp;
      }
    }
  }

  return cmp;
}

/* ====================================================================
 *   Expand_Branch
 * ====================================================================
 */
void
Expand_Branch ( 
  TN *targ, 
  TN *src1, 
  TN *src2, 
  VARIANT variant, 
  OPS *ops
)
{
  TOP   cmp;

  // If the branch is conditional FALSEBR, we must invert the condition 
  // because ST200 branches are active on TRUE.
  //  if (V_br_condition(variant) != V_BR_ALWAYS &&
  //      V_br_condition(variant) != V_BR_NEVER &&
  //      V_false_br(variant) ) 
  //    variant = Negate_BR_Variant(variant);

  BOOL  false_br = V_false_br(variant);
  VARIANT cond = V_br_condition(variant);

  /* Trace if required: */
  if (Trace_Exp) {
    fprintf ( TFile, "<cgexp> Translating %s branch:\n",
                                    (false_br ? "false" : "true") );
  }

  /* default branch instruction on: */
  if (cond == V_BR_NONE) {
    FmtAssert(FALSE,("Expand_Branch: default branch ??"));
    return;
  }

  FmtAssert( cond <= V_BR_LAST, ("unexpected variant in Expand_Branch"));
  FmtAssert( cond != V_BR_NONE, ("BR_NONE variant in Expand_Branch"));

  if (Trace_Exp) {
    fprintf (TFile, "<cgexp> branch cond = %s\n", BR_Variant_Name(cond));
  }

  // check for special case of second arg being zero.
  if (src2 != NULL && TN_is_zero(src2)) {
    switch (cond) {
      case V_BR_U8LT:	
      case V_BR_U4LT:	
      case V_BR_FLT:	
	cond = V_BR_NEVER; break;
      case V_BR_U8GE:
      case V_BR_U4GE:
      case V_BR_FGE:
	cond = V_BR_ALWAYS; break;
    }
  }

  if(false_br) {
    switch(cond) {
    case V_BR_NEVER: cond = V_BR_ALWAYS; break;
    case V_BR_ALWAYS: cond = V_BR_NEVER; break;
    case V_BR_I4EQ: cond = V_BR_I4NE; break;
    case V_BR_I4NE: cond = V_BR_I4EQ; break;
    case V_BR_I4GT: cond = V_BR_I4LE; break;
    case V_BR_I4GE: cond = V_BR_I4LT; break;
    case V_BR_I4LT: cond = V_BR_I4GE; break;
    case V_BR_I4LE: cond = V_BR_I4GT; break;
    case V_BR_U4EQ: cond = V_BR_U4NE; break;
    case V_BR_U4NE: cond = V_BR_U4EQ; break;
    case V_BR_U4GT: cond = V_BR_U4LE; break;
    case V_BR_U4GE: cond = V_BR_U4LT; break;
    case V_BR_U4LT: cond = V_BR_U4GE; break;
    case V_BR_U4LE: cond = V_BR_U4GT; break;
    case V_BR_FEQ: cond = V_BR_FNE; break;
    case V_BR_FNE: cond = V_BR_FEQ; break;
    case V_BR_FGT: cond = V_BR_FLE; break;
    case V_BR_FGE: cond = V_BR_FLT; break;
    case V_BR_FLT: cond = V_BR_FGE; break;
    case V_BR_FLE: cond = V_BR_FGT; break;
    case V_BR_DNE: cond = V_BR_DEQ; break;
    case V_BR_DEQ: cond = V_BR_DNE; break;
    default:
      FmtAssert(FALSE,("Unimplemented condition"));
    }
  }


  // compare should calculate a guard reg result, integer result 
  // supported only for floating point comparisons.

  TN *cond_tn = Gen_Enum_TN(ISA_ECV_cond_al);
  cmp = Pick_Compare_TOP (&cond, &src1, &src2, &cond_tn, ops);

  if (Trace_Exp) {
    fprintf (TFile, "<cgexp> transformed branch cond = %s\n",BR_Variant_Name(cond));
  }

  switch (cond) {
  case V_BR_ALWAYS:
  case V_BR_NEVER:
    Is_True(cmp == TOP_UNDEFINED, 
	    ("unexpected compare op for %s", BR_Variant_Name(cond)));
    if (cond == V_BR_ALWAYS) {
      // Unconditional branch for ALWAYS/!false_br and NEVER/false_br
      FmtAssert(!TN_is_register(targ),("targ is a register"));
      TN *pc = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,16,0);
      Build_OP (TOP_b, pc, pc, targ, ops);
    }
    break;

  case V_BR_P_TRUE:
    Is_True(cmp == TOP_UNDEFINED, ("unexpected compare op for V_BR_P_TRUE"));
    FmtAssert(FALSE,("Thing to see..."));
    /*    Build_OP (false_br ? TOP_brf : TOP_br, src1, targ, ops); */
    break;

  default:
    {
      TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr,1,0);
      TN *pc = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,16,0);

      FmtAssert(cmp != TOP_UNDEFINED, 
                                  ("Expand_Branch: unexpected comparison"));
      if(TN_has_value(src2)) {

	Build_OP (cmp, cpsr, src1, src2, ops);
      }
      else {
	Build_OP (cmp, cpsr, src1, src2, ops);
      }

      FmtAssert(TN_is_label(targ), ("Expand_Branch: expected a label"));
      /*
       * For now I just trust it that it fits into 23 bits
       */
      Build_OP (TOP_b_cond, pc, cpsr, pc, cond_tn, targ, ops);
    }
    break;
  }

  return;
}

/* ====================================================================
 *   Exp_Indirect_Branch
 * ====================================================================
 */
void 
Exp_Indirect_Branch (
  TN *targ_reg, 
  OPS *ops
)
{
  FmtAssert(TN_register_class(targ_reg) == ISA_REGISTER_CLASS_integer,("Not integer register class"));
  TN *cpsrt = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr, 1, 0);
  TN *pc = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,16,0);
  Build_OP (TOP_bx, cpsrt, pc, targ_reg, ops);
  return;
}

/* ====================================================================
 *   Exp_Local_Branch
 * ====================================================================
 */
void 
Exp_Local_Jump (
  BB *bb, 
  INT64 offset, 
  OPS *ops
)
{
#ifdef TARG_ST  
  TN *targ;
  ANNOTATION *ant;
  LABEL_IDX lab;

  FmtAssert(offset == 0, ("Offset is non null in Exp_Local_Jump"));
  /* first get a label attached to this BB */
  ant = ANNOT_First (BB_annotations(bb), ANNOT_LABEL);
  lab = ANNOT_label(ant);

  targ = Gen_Label_TN (lab, offset);
  TN *pc = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,16,0);
  Build_OP (TOP_b, pc, pc, targ, ops);
#else
   FmtAssert(FALSE, ("NYI: Exp_Local_Jump"));
#endif
}

/* ====================================================================
 *   Exp_Return
 * ====================================================================
 */
void 
Exp_Return (
  TN *return_address, 
  OPS *ops
)
{
  TN *cpsrt = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr, 1, 0);
  TN *pc = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,16,0);
  Build_OP (TOP_bx, cpsrt, pc, RA_TN, ops);
  return;
}

void 
Exp_Return_Interrupt (
  TN *return_address, 
  OPS *ops
)
{
  // See architecture manual page A6-3 on different interrupt returns.
  TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr, 1, 0);
  TN *spsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_spsr, 1, 0);
  TN *pc = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,16,0);
  Build_OP (TOP_sub_i8_r_pc_s, cpsr, pc, spsr, Gen_Enum_TN(ISA_ECV_s_s), RA_TN, Gen_Literal_TN(4, 4), ops);
  return;
}

/* ====================================================================
 *   Exp_Call
 *
 *   There are three types of calls:
 *     OPR_CALL    -- direct call;
 *     OPR_ICALL   -- indirect call;
 *     OPR_PICCALL -- pic call.
 * ====================================================================
 */
void 
Exp_Call (
  OPERATOR opr, 
  TN *return_address, 
  TN *target, 
  OPS *ops
)
{
  TOP top;
  TN *br_tmp;

  if (Trace_Exp) {
    fprintf(TFile, "exp_call %s :- ", OPERATOR_name(opr));
    Print_TN (target, FALSE);
    fprintf(TFile, "\n");
  }

  FmtAssert(!TN_relocs(target),("target TN has relocation"));

  switch (opr) {
  case OPR_CALL: {
    TN *pc = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,16,0);
    Build_OP (TOP_bl, RA_TN, pc, pc, target, ops);
    break;
  }

  case OPR_ICALL: {
    if (Trace_Exp) {
      fprintf(TFile,"exp_call ICALL into ");
      Print_OP (OPS_last(ops));
    }
    
    FmtAssert(TN_register_class(target) == ISA_REGISTER_CLASS_integer,("not a integer register"));
    TN *cpsrt = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr, 1, 0);
    TN *pc = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,16,0);
    Build_OP(TOP_blx_npc, cpsrt, RA_TN, pc, target, ops);
    break;
  }

    case OPR_PICCALL:

      FmtAssert(FALSE, ("Not Implemented"));
      top = TOP_noop;
      break;

    default:
      FmtAssert(FALSE, ("unexpected opr in Exp_Call"));
      /*NOTREACHED*/
  }

  if (Trace_Exp) {
    fprintf(TFile,"exp_call into ");
    Print_OP (OPS_last(ops));
  }

  return;
}
