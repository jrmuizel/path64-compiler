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
 * Module: cgtarget.cxx
 *
 * Description:
 *
 * Support routines for target-specific code generator functionality.
 *
 * ====================================================================
 * ====================================================================
 */

#include <ctype.h>
#include "W_alloca.h"

#include "defs.h"
#include "util.h"
#include "config.h"
#include "config_TARG.h"
#include "config_asm.h"
#include "erglob.h"
#include "ercg.h"
#include "tracing.h"
#include "data_layout.h"
#include "const.h"
#include "wn.h"
#include "import.h"
#include "opt_alias_interface.h"
#include "opt_alias_mgr.h"
#include "cgir.h"
#include "cg.h"
#include "void_list.h"
#include "whirl2ops.h"
#include "w2op.h"
#include "cgtarget.h"
#include "calls.h"
#include "cgexp.h"
#include "config_asm.h"
#include "hb_sched.h"
#include "targ_isa_variants.h"
#include "cg_loop.h"
#include "dominate.h"
#include "targ_sections.h"

#ifdef TARG_ST
/* for reconfigurability */
#include "lai_loader_api.h"
#include "stblock.h"
#include "register_preg.h"

#include "../../../gccfe/gnu/arm/insn-config.h" /* for MAX_RECOG_OPERANDS */
#endif

/* Import from targ_cgemit.cxx. */
extern void CGEMIT_Qualified_Name(ST *st, vstring *buf);

UINT32 CGTARG_branch_taken_penalty;
BOOL CGTARG_branch_taken_penalty_overridden = FALSE;

#ifdef TARG_ST
UINT32 CGTARG_max_issue_width;
BOOL CGTARG_max_issue_width_overriden = FALSE;
#endif

UINT32 CGTARG_NUM_SPILL_TYPES = 0;
TY_IDX CGTARG_Spill_Type[MTYPE_MAX_LIMIT+1];
CLASS_INDEX CGTARG_Spill_Mtype[MTYPE_MAX_LIMIT+1];
static TOP *CGTARG_Invert_Table;

static ISA_EXEC_UNIT_PROPERTY template_props[ISA_MAX_BUNDLES][ISA_BUNDLE_MAX_SLOTS];

static BOOL earliest_regclass_use_initialized = FALSE;
#ifdef TARG_ST
static INT earliest_regclass_use[ISA_REGISTER_CLASS_MAX_LIMIT+1];
static INT earliest_regsubclass_use[ISA_REGISTER_SUBCLASS_MAX_LIMIT+1];
static INT earliest_dedicated_reg_use[ISA_REGISTER_CLASS_MAX_LIMIT+1][ISA_REGISTER_MAX+1];
static INT earliest_assigned_reg_use[ISA_REGISTER_CLASS_MAX_LIMIT+1][ISA_REGISTER_MAX+1];
#else
static INT earliest_regclass_use[ISA_REGISTER_CLASS_MAX+1];
static INT earliest_regsubclass_use[ISA_REGISTER_SUBCLASS_MAX+1];
static INT earliest_dedicated_reg_use[ISA_REGISTER_CLASS_MAX+1][ISA_REGISTER_MAX+1];
static INT earliest_assigned_reg_use[ISA_REGISTER_CLASS_MAX+1][ISA_REGISTER_MAX+1];
#endif

BOOL saveHWloopRegisterLR0 = FALSE;

/* ====================================================================
 *   CGTARG_Preg_Register_And_Class
 * ====================================================================
 */
BOOL
CGTARG_Preg_Register_And_Class (
  WN_OFFSET preg,
  ISA_REGISTER_CLASS *p_rclass,
  REGISTER *p_reg
)
{
  ISA_REGISTER_CLASS rclass = 0, rclassi;
  INT regnum;
  REGISTER reg;
  
  /* Get the target register number and class associated with the
   * preg, if there is one that is.
   */
  FOR_ALL_ISA_REGISTER_CLASS( rclassi ) {
    int preg_min = CGTARG_Regclass_Preg_Min(rclassi);
    int preg_max = CGTARG_Regclass_Preg_Max(rclassi);
    if (preg >= preg_min && preg <= preg_max) {
      regnum = preg - preg_min;
      rclass = rclassi;
      break;
    }
  }
  if (rclass == 0)
    return FALSE;

  /* Find the CG register for the target register and class.
   */
  for (reg = REGISTER_MIN; 
       reg <= REGISTER_CLASS_last_register(rclass); 
       reg++ ) {
    if (REGISTER_machine_id(rclass,reg) == regnum) {
      *p_reg = reg;
      *p_rclass = rclass;
      return TRUE;
    }
  }

  FmtAssert(FALSE, ("failed to map preg %d", preg));
  /*NOTREACHED*/
}

/* ====================================================================
 *   TOP_br_variant
 *
 *   TODO: isa_properties ...
 * ====================================================================
 */
VARIANT
TOP_br_variant (
  TOP opcode
)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  switch (opcode) {
  case TOP_cmpeq_r:
  case TOP_cmpeq_i8:
    return V_BR_I4EQ;

  case TOP_cmpge_r:
  case TOP_cmpge_i8:
    return V_BR_I4GE;

  case TOP_cmpgeu_r:
  case TOP_cmpgeu_i8:
    return V_BR_U4GE;

  case TOP_cmpgt_r:
  case TOP_cmpgt_i8:
    return V_BR_I4GT;

  case TOP_cmpgtu_r:
  case TOP_cmpgtu_i8:
    return V_BR_U4GT;

  case TOP_cmple_r:
  case TOP_cmple_i8:
    return V_BR_I4LE;

  case TOP_cmpleu_r:
  case TOP_cmpleu_i8:
    return V_BR_U4LE;

  case TOP_cmplt_r:
  case TOP_cmplt_i8:
    return V_BR_I4LT;

  case TOP_cmpltu_r:
  case TOP_cmpltu_i8:
    return V_BR_U4LT;

  case TOP_cmpne_r:
  case TOP_cmpne_i8:
    return V_BR_I4NE;
  }

#endif
  return V_BR_NONE;
}


/* ====================================================================
 *   TOP_cmp_variant
 *
 *   Returns the variant for the interpretation of the semantic of
 *   opcodes having the TOP_is_cmp property.
 *   Must be interpreted as:
 *   result is true if OU_opnd1 <variant> OU_opnd2 is true
 * ====================================================================
 */
VARIANT
OP_cmp_variant(const OP *op)
{
  /* The condition is not applied on comparison on ARM */
  return V_CMP_NONE;
}

/* ====================================================================
 *   OP_br_variant
 *
 *   TODO: isa_properties ...
 * ====================================================================
 */
VARIANT
OP_br_variant (const  OP *op)
{
  return V_BR_NONE;
}


/* ====================================================================
 *   TOP_cond_variant
 *
 *   Returns the variant for the interpretation of the OU_condition operand
 *   for an opcode having the TOP_is_select or TOP_is_cond semantic.
 *
 *   Possible values of variants are V_COND_TRUE or V_COND_FALSE.
 *   The select semantic is:
 *   (OU_condition == 0 ? OU_opnd1: OU_opnd2) if variant = V_COND_FALSE
 *   else OU_condition != 0 ? OU_opnd1: OU_opnd2) if variant != V_COND_FALSE
 *   The cond branch semantic is:
 *   branch taken if OU_condition == 0 if variant == V_COND_FALSE
 *   branch taken if OU_condition != 0 if variant != V_COND_FALSE
 *
 * ====================================================================
 */

VARIANT
TOP_cond_variant(TOP top)
{
  DevWarn("TOP_cond_variant undefined for TOP %s", TOP_Name(top));
  return V_COND_NONE;
}


/* ====================================================================
 *   Make_Branch_Conditional
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
  BB *bb,
  TN *pred_tn,     // cond TN for branching
  BOOL cond        // whether branches on cond TN TRUE or FALSE
)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0

  OP *new_br;
  TOP new_top;
  OP* br_op = BB_branch_op(bb);
  OPS ops = OPS_EMPTY;
  
  if (!br_op) return;

  if (TN_register_class(pred_tn) != ISA_REGISTER_CLASS_branch) {
    TN *br_tn = Build_RCLASS_TN (ISA_REGISTER_CLASS_branch);
    Exp_COPY(br_tn, pred_tn, &ops);
    pred_tn = br_tn;
  }

  switch (OP_code(br_op)) {
  case TOP_goto:
    new_top = cond ? TOP_br : TOP_brf;
    break;
  case TOP_igoto:
    FmtAssert(FALSE,("Can't handle igoto -> contitional"));
    new_top = TOP_UNDEFINED;
    break;
  default:
    FmtAssert(FALSE,("Can't handle %s", TOP_Name(OP_code(br_op))));
    return;
  }

  // Make a tmp cond register so we can replace it later
  new_br = Mk_OP(new_top, pred_tn, OP_opnd(br_op,0));
  OP_srcpos(new_br) = OP_srcpos(br_op);
  OPS_Append_Op(&ops, new_br);
  BB_Insert_Ops_After(bb, br_op, &ops);
  BB_Remove_Op(bb, br_op);
#endif
}

/* ====================================================================
 *   CGTARG_Parallel_Compare
 * ====================================================================
 */
TOP
CGTARG_Parallel_Compare (
  OP* cmp_op, 
  COMPARE_TYPE ctype
)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  //
  // None for floats.  For gp registers, only support "eq" and "ne"
  // for arbitrary register compares.  Others must have one operand
  // be the zero register.
  //
  if (OP_flop(cmp_op)) {
    return TOP_UNDEFINED;
  }

  FmtAssert(FALSE,("CGTARG_Parallel_Compare: not implemented"));

#endif
  return TOP_UNDEFINED;
}

/* ====================================================================
 *   CGTARG_Unconditional_Compare
 * ====================================================================
 */
static BOOL 
CGTARG_Unconditional_Compare_Helper (
  TOP top, 
  TOP* uncond_ver
)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  // There is no unconditional compares on the ST100 for the
  // moment:
  switch (top) {

  default:
    *uncond_ver = TOP_UNDEFINED;
  }
#endif
  return FALSE;
}

BOOL CGTARG_Unconditional_Compare(OP *op, TOP* uncond_ver)
{
  return (CGTARG_Unconditional_Compare_Helper(OP_code(op),uncond_ver));
}

/* ====================================================================
 *   CGTARG_Compute_Branch_Parameters
 * ====================================================================
 */
void 
CGTARG_Compute_Branch_Parameters (
  INT32 *mispredict, 
  INT32 *fixed, 
  INT32 *brtaken, 
  double *factor
)
{
  *mispredict = 0;
  *fixed = 0;
  *brtaken = 0;
  *factor = 0.0;

  // For ST220/ST231 targets we have:
  // - no hardware prediction
  // - fixed branch cost is null
  // - taken branch cost is 1
  *mispredict= 0; *fixed= 0; *brtaken= 1; *factor = 1.0;

 /*
  * override for command line options
  *	-CG:mispredicted_branch=N
  *	-CG:mispredicted_factor=N
  */
  if (CG_branch_mispredict_penalty >= 0)
    *mispredict= CG_branch_mispredict_penalty ;

  if (CG_branch_mispredict_factor >= 0)
    *factor= CG_branch_mispredict_factor * (.01);
}

/* ====================================================================
 *   CGTARG_Branch_Info
 * ====================================================================
 */
void
CGTARG_Branch_Info ( 
  const OP  *op,
  INT *tfirst,            /* Which operand is the first target? */
  INT *tcount             /* How many target operands are there? */
)
{
  INT i;
  TN *tn;

  /* Initialize results: */
  *tfirst = -1;
  *tcount = 0;

  /* Find the first target: */
  for ( i = 0; ; i++ ) {
    if ( i >= OP_opnds(op) ) return;
    tn = OP_opnd(op,i);
    if ( tn != NULL && TN_is_label(tn) ) break;
  }
  *tfirst = i;

  /* Count the targets: */
  *tcount = 1;
  for ( i++; i < OP_opnds(op); i++ ) {
    tn = OP_opnd(op,i);
    if ( tn == NULL || ! TN_is_label(tn) ) return;
    (*tcount)++;
  }
  return;
}

/* ====================================================================
 *   CGTARG_Analyze_Branch
 *
 *   'tn1' is TN that is set to TRUE if the branch is taken;
 *   'tn2' is TN that is set to TRUE is the branch falls through.
 *   TODO: but CFLOW crushes, so for now always return the cond. TN
 *         in 'tn1'.
 * ====================================================================
 */
VARIANT
CGTARG_Analyze_Branch (
  OP *br,
  TN **tn1,
  TN **tn2
)
{
  VARIANT variant = V_BR_NONE;
  TOP top = OP_code(br);

  FmtAssert(OP_cond(br),("Not conditional branch"));

  INT32 cond_idx = OP_find_opnd_use(br,OU_condvariant);
  TN *condvariant_tn = OP_opnd(br,cond_idx);
  FmtAssert(cond_idx != -1,("cond variant operand not defined"));
  INT32 pred_idx;
  if(OP_cond(br)) {
    pred_idx = OP_find_opnd_use(br, OU_condition);
  }
  else {
    pred_idx = OP_find_opnd_use(br, OU_predicate);
  }

  /* Set tn1 to this operand, because it contains flags and can be used to reach def. */
  *tn1 = pred_idx != -1 ? OP_opnd(br,pred_idx) : NULL;
  *tn2 = NULL;

  switch (TN_enum(condvariant_tn)) {
  case ISA_ECV_cond_eq:
    variant = V_BR_PEQ;
    Set_V_true_br(variant);
    break;
  case ISA_ECV_cond_ne:
    variant = V_BR_PNE;
    Set_V_true_br(variant);
    break;
  case ISA_ECV_cond_gt:
    variant = V_BR_PGT;
    Set_V_true_br(variant);
    break;
  case ISA_ECV_cond_pl:
  case ISA_ECV_cond_ge:
    variant = V_BR_PGE;
    Set_V_true_br(variant);
    break;
  case ISA_ECV_cond_lt:
    variant = V_BR_PLT;
    Set_V_true_br(variant);
    break;
  case ISA_ECV_cond_le:
    variant = V_BR_PLE;
    Set_V_true_br(variant);
    break;
  case ISA_ECV_cond_hi:
    variant = V_BR_PUGT;
    Set_V_true_br(variant);
    break;
  case ISA_ECV_cond_hs:
  case ISA_ECV_cond_cs:
    variant = V_BR_PUGE;
    Set_V_true_br(variant);
    break;
  case ISA_ECV_cond_lo:
  case ISA_ECV_cond_cc:
  case ISA_ECV_cond_mi:
    variant = V_BR_PULT;
    Set_V_true_br(variant);
    break;
  case ISA_ECV_cond_ls:
    variant = V_BR_PULE;
    Set_V_true_br(variant);
    break;
  case ISA_ECV_cond_vs:
  case ISA_ECV_cond_vc:
    variant = V_BR_NONE;
    break;
  default:
    FmtAssert(FALSE, ("not sure what to do"));
    variant = V_BR_NONE;
    break;
  }
  return variant;
}

/* ====================================================================
 *   CGTARG_Analyze_Compare
 * ====================================================================
 */
VARIANT
CGTARG_Analyze_Compare (
  OP *br,
  TN **tn1,
  TN **tn2,
  OP **compare_op
)
{
  VARIANT variant = V_BR_NONE;
  TN *cond_tn1;
  TN *cond_tn2;
  BOOL is_double;
  OP *def_op;
  DEF_KIND kind;
  BOOL false_br;
  
  /* Classify the condition based on the branch instruction. */
  /* [CG]: on ST200 branch variant may have V_BR_FALSE set. */
  variant = CGTARG_Analyze_Branch(br, &cond_tn1, &cond_tn2);
  false_br = V_false_br(variant);

  *compare_op = NULL;
  *tn1 = NULL;
  *tn2 = NULL;

  if (cond_tn1 == NULL) {
    /* FdF 20060608: Support for HWBRANCH pseudo instruction. */
    Is_True(OP_Is_Counted_Loop(br), ("Unxpected NULL value for TOP %s\n", TOP_Name(OP_code(br))));
    return variant;
  }

  variant = V_BR_NONE;

  /* Attempt to find the defining OP for the tested value.
   */
  def_op = TN_Reaching_Value_At_Op(cond_tn1, br, &kind, TRUE);
  
  if (def_op != NULL) {
    variant = TOP_br_variant(OP_code(def_op));
    if (variant != V_BR_NONE) {
      INT opnd1_idx = OP_find_opnd_use(def_op, OU_opnd1);
      INT opnd2_idx = OP_find_opnd_use(def_op, OU_opnd2);
      TN *opnd1_tn = opnd1_idx != -1 ? OP_opnd(def_op,opnd1_idx) : (TN*)NULL;
      TN *opnd2_tn = opnd2_idx != -1 ? OP_opnd(def_op,opnd2_idx) : (TN*)NULL;

      /* [CG] : Set false branch variant if needed. */
      if (false_br) { Set_V_false_br(variant); }
      *compare_op = def_op;
      *tn1 = opnd1_tn;
      *tn2 = opnd2_tn;
    }
  }
  return variant;
}

/* ====================================================================
 *   CGTARG_Get_unc_Variant
 * ====================================================================
 */
TOP 
CGTARG_Get_unc_Variant (
  TOP top
)
{
  TOP result;
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  CGTARG_Unconditional_Compare_Helper(top, &result);
  
  if (result == TOP_UNDEFINED) {
    result = top;
  }

#endif  
  return (result);
}

/* ====================================================================
 *   CGTARG_Can_Negate_Branch
 * ====================================================================
 */
BOOL
CGTARG_Can_Negate_Branch(OP *br)
{
  return TRUE;
}
 
/* ====================================================================
 *                           Properties:
 * ====================================================================
 */

TOP 
CGTARG_Invert(TOP opr)
{
  return CGTARG_Invert_Table[(INT)opr];
}

// [CL] try to invert an operation, able to deal with cases where it
// is not sufficient to invert the TOP, but also change operands
OP*
CGTARG_Invert_OP(OP* op)
{
  OP* new_op = NULL;

  TOP new_top = CGTARG_Invert(OP_code(op));
  TN *result = OP_result(op,0);
  TN* new_result = Build_TN_Like(result);

  if (new_top != TOP_UNDEFINED) {
    DevAssert(!OP_iand (op) && !OP_ior (op), ("try to invert a logif"));
    FmtAssert (FALSE, ("not yet implemented"));
#if 0
    new_op = Dup_OP(op);
    OP_Change_Opcode(new_op, new_top);
    Set_OP_result(new_op, 0, new_result);
  } else if (OP_code(op) == TOP_convib_r_b) {
    TN *opnd = OP_opnd(op, 0);
    new_op = Mk_OP(TOP_cmpeq_r_r_b, new_result, opnd, Zero_TN);
#endif
  }
  return new_op;
}

OP*
CGTARG_Negate_OP(OP* op) {
  return CGTARG_Invert_OP(op);
}

BOOL
CGTARG_Is_OP_Inter_RegClass_Bool_Copy(OP *op)
{
  /* TODO: To be implemented. */
  return FALSE;
}


/*
 * Helper function for push/pop identification on STxP70.
 * It Could be described by a property in targinfo.
 */
static BOOL
OP_push(OP *op)
{
  TOP top = OP_code(op);
  switch (top) {
    /* Only this multi store used in pop sequence for now. */
  case TOP_stm_reglist_pre_npc_amode4s:
    return TRUE;
  }
  return FALSE;
}
static BOOL
OP_pop(OP *op)
{
  TOP top = OP_code(op);
  switch (top) {
    /* Only this multi load used in pop sequence for now. */
  case TOP_ldm_reglist_pre_npc_amode4l:
    return TRUE;
  }
  return FALSE;
}

/** 
 * GCM code ignores (by default) dummy operation.
 *
 * Some dummy operations (like HWBRANCH on stxp70) have dependencies to
 * non-dummy instructions (setl?) and thus cannot be ignored by GCM.
 * 
 * @param op 
 * 
 * @return 
 */
BOOL
CGTARG_dummy_op_has_sideeffects(OP *op) {
  return FALSE;
}

BOOL 
CGTARG_gcm_should_not_move_op(OP *op) {

    if (OP_epilogue(op)) return TRUE;

    return FALSE;
}

/* ====================================================================
 *   CGTARG_Dependence_Required
 * ====================================================================
 */
BOOL
CGTARG_Dependence_Required (
  OP *pred_op, 
  OP *succ_op,
  INT *latency
)
{

  // Force asm to stay at end of block
  if (OP_code(succ_op) == TOP_asm) {
    *latency = 1;
    return TRUE;
  }

  // Force push/pop to not be reordered.
  // The effective dependencies implied by push/pop cannot be described
  // statically (except for SP). Thus we force them to not be reordered.
  // We could be more precise here and effectively assert dependency only
  // when required but it is not necessary on this machine.
  if (OP_push(pred_op) || OP_pop(pred_op) || OP_push(succ_op) || OP_pop(succ_op)) {
    *latency = 1;
    return TRUE;
  }

   // Side effects instructions mus not be scheduled.
  if (OP_side_effects(pred_op) || OP_side_effects(succ_op)) {
    *latency = 1;
    return TRUE;
  }
    
      
  // For STxP70 some instructions write or read the carry flag SR.C
  // This is an hidden resource usage
  // that we must account for in the depencies. 
  // It is not described in the architectural description but has a flag
  // effect.
  if (OP_Get_Flag_Effects(pred_op) != OP_FE_NONE &&
      OP_Get_Flag_Effects(succ_op) != OP_FE_NONE) {
    if ((OP_Get_Flag_Effects(pred_op) & OP_FE_WRITE) ||
	(OP_Get_Flag_Effects(succ_op) & OP_FE_WRITE)) {
      *latency = 1;
      return TRUE;
    }
  }

  // Arcs between an OP that defines a dedicated call register and the
  // call OP. I do it here because the usual REGIN/OUT mechanism
  // want me to tell it to which opnd the dependence is attached.
  // There is no opnd corresponding to dedicated call registers, so ...
  // I just hope that associating all of these to opnd 0 using MISC
  // arcs will work.
  if (!OP_call(succ_op)) return FALSE;

  // WE GET THERE  ONLY IF OP_call(succ_op)

  INT i;
  BOOL need_dependence = FALSE;
  INT max_latency = 0;

  // First compute the max of result latencies for pred_op.
  for (i = 0; i < OP_results(pred_op); i++) {
    TN *result = OP_result(pred_op,i);
    if (TN_is_register(result) && TN_is_dedicated(result)) {
      REGISTER reg = TN_register(result);
      ISA_REGISTER_CLASS rclass = TN_register_class (result);
      INT cur_latency;
      
      // regs which have implicit meaning.
      if(REGISTER_SET_MemberP(REGISTER_CLASS_function_argument(rclass), reg) ||
	 REGISTER_SET_MemberP(REGISTER_CLASS_caller_saves(rclass), reg)) {
	
	need_dependence = TRUE;
	cur_latency = CGTARG_Max_RES_Latency(pred_op, i);
	// FdF 20041117: If not st220, set latency to 0 to allow
	// bundling with the call instruction (ddts 19668).
	if (!FORCE_NOOPS)
	  cur_latency = 0;
	if (cur_latency > max_latency) max_latency = cur_latency;
      }
    }
  }

  // Remove cyles lost by branch taken.
  max_latency -= 1 + CGTARG_Branch_Taken_Penalty();
  if (max_latency < 0) max_latency = 0;
  *latency = max_latency;

  return need_dependence;
}

/* ====================================================================
 *                          Local ASMs stuff:
 * ====================================================================
 */
#ifdef TARG_ST
// [TTh] Use gccfe limit for max ASM operands
static TN* asm_constraint_tn[MAX_RECOG_OPERANDS];
static ISA_REGISTER_SUBCLASS asm_constraint_sc[MAX_RECOG_OPERANDS];
static char asm_constraint_name[MAX_RECOG_OPERANDS][8];
#else
static TN* asm_constraint_tn[10];
static ISA_REGISTER_SUBCLASS asm_constraint_sc[10];
static char asm_constraint_name[10][8];
#endif
static INT asm_constraint_index;

/* ====================================================================
 *   CGTARG_Init_Asm_Constraints ()
 * ====================================================================
 */
void
CGTARG_Init_Asm_Constraints (void)
{
  // can use any type; it will be ignored
  Setup_Output_Parameter_Locations (MTYPE_To_TY(MTYPE_I8));
#ifdef TARG_ST
  for (INT i = 0; i < MAX_RECOG_OPERANDS; ++i)
#else
  for (INT i = 0; i < 10; ++i)
#endif
  {
    asm_constraint_tn[i] = NULL;
    asm_constraint_sc[i] = ISA_REGISTER_SUBCLASS_UNDEFINED;
    asm_constraint_name[i][0] = '\0';
  }
  asm_constraint_index = 0;
}

/* ====================================================================
 *   CGTARG_Modified_Asm_Opnd_Name (modifier, tn, tn_name)
 * ====================================================================
 */
char CGTARG_Asm_Opnd_Modifiers[] = { 'r' };
INT  CGTARG_Num_Asm_Opnd_Modifiers = 1;

const char* 
CGTARG_Modified_Asm_Opnd_Name(
  char modifier, 
  TN* tn, 
  char *tn_name
)
{
  if (modifier == 'r') {
    return tn_name;
  }
  else {
    FmtAssert(FALSE, ("Unknown ASM operand modifier '%c'", modifier));
  }
  /*NOTREACHED*/
}

// -----------------------------------------------------------------------
// Given a constraint for an ASM parameter, and the load of the matching
// argument passed to ASM (possibly NULL), choose an appropriate TN for it
// This function may return NULL, in this case the TN was not matched
// and the caller must emit a fatal error message.
// See also function Check_Asm_Constraints() in config_targ.cxx which
// check constraint and mtype compatibility. 
// -----------------------------------------------------------------------
TN* 
CGTARG_TN_For_Asm_Operand (
  const char* constraint,            
  const WN* load,
  TN* pref_tn,
  ISA_REGISTER_SUBCLASS* subclass
)
{
  // skip constraint modifiers:
  // = input and output parameters are separated in the WHIRL for ASM
  // & early_clobber flag is set in Handle_ASM
  // % commutativity of operands is ignored for now
  static const char modifiers[] = "=&%+";
  TN* ret_tn;
  
  while (*constraint != '\0' && strchr(modifiers, *constraint)) {
    constraint++;
  }
  
#ifdef TARG_ST
  //TB: Dynamic register files: scan first because the register
  //nickname might contain any character with a specific meaning
  if (Extension_Is_Present && strlen(constraint) > 1) {
	if (load == NULL) {
	  TYPE_ID mtype;
	  for (mtype = MTYPE_STATIC_LAST+1; mtype <= MTYPE_LAST; mtype++) {
		if (!strcmp(constraint, MTYPE_name(mtype))) {
		  *subclass = Register_Subclass_For_Mtype(mtype);
		  ret_tn = (pref_tn ? pref_tn : Build_TN_Of_Mtype(mtype));
		  goto finish;
		}
	  }
	  FmtAssert(load != NULL,
				("CGTARG_TN_For_Asm_Operand: load instruction is null,"
				 " cannot choose an appropriate TN for <%s>", constraint));
	}
    TYPE_ID rtype = WN_rtype(load);
    *subclass = Register_Subclass_For_Mtype(rtype);
    char *mtype_name = MTYPE_name(rtype);
    if (strcmp(constraint, mtype_name) == 0) {
      ret_tn = (pref_tn ? pref_tn : Build_TN_Of_Mtype(rtype));
      // Skip the general asm treatment
      goto finish;
    } else {
      ErrMsg(EC_CG_Generic_Error, "operand type to 'asm' does not match constraint");
      return NULL;
    }
  }
#endif
  // TODO: we should really look at multiple specified constraints
  // and the load in order to select the best TN, but for now we
  // assume that when we get here we can safely pick a TN

  // if 'm' is one of the choices, always prefer that one
  // TODO: we decide this in the front end, but it's not optimal
  if (*constraint != 'm') {
    const char* m = constraint;
    while (*++m) {
      if (*m == 'm') {
        constraint = m;
        break;
      }
    }
  }
  
  // prefer register/memory over immediates; this isn't optimal, 
  // but we may not always be able to generate an immediate
  static const char immediates[] = "in";
  while (*constraint != '\0' && strchr(immediates, *constraint) && *(constraint+1) != '\0') {
    constraint++;
  }

  // The 'n' constraints only accepts constant literal values.
  if (strchr("n", *constraint)) {
    if (load && WN_operator(load)==OPR_LDID && WN_class(load)==CLASS_PREG &&
	(WN_rtype(load) == MTYPE_I4 || WN_rtype(load) == MTYPE_U4)) {
      // immediate could have been put in preg by wopt
      load = Preg_Is_Rematerializable(WN_load_offset(load), NULL);
    }
    if (load && WN_operator(load) == OPR_INTCONST &&
	(WN_rtype(load) == MTYPE_I4 || WN_rtype(load) == MTYPE_U4)) {
      *subclass = Register_Subclass_For_Mtype(WN_rtype(load));
      ret_tn = Gen_Literal_TN(WN_const_val(load), 
			      MTYPE_bit_size(WN_rtype(load))/8);
    } else {
      ErrMsg(EC_CG_Generic_Error, "input operand to 'asm' does not match constraint 'n'");
      return NULL;
    }
  }
  
  // The 'i' constraints accepts literals and symbolic values as the
  // address of a symbol.
  // We also treat 'p' as 'i'. 'p' must be a valid memory adress.
  else if (strchr("pi", *constraint)) {
    if (load && WN_operator(load)==OPR_LDID && WN_class(load)==CLASS_PREG &&
	(WN_rtype(load) == MTYPE_I4 || WN_rtype(load) == MTYPE_U4)) {
      // immediate could have been put in preg by wopt
      load = Preg_Is_Rematerializable(WN_load_offset(load), NULL);
    }
    if (load && WN_operator(load) == OPR_INTCONST &&
	(WN_rtype(load) == MTYPE_I4 || WN_rtype(load) == MTYPE_U4)) {
      *subclass = Register_Subclass_For_Mtype(WN_rtype(load));
      ret_tn = Gen_Literal_TN(WN_const_val(load), 
                            MTYPE_bit_size(WN_rtype(load))/8);
    } else if (load && WN_operator(load) == OPR_LDA &&
	       (WN_rtype(load) == MTYPE_I4 || WN_rtype(load) == MTYPE_U4)) {
      ST *sym = WN_st(load);
      *subclass = Register_Subclass_For_Mtype(WN_rtype(load));
      ret_tn = Gen_Symbol_TN (sym, 0, TN_RELOC_NONE);
    } else {
      ErrMsg(EC_CG_Generic_Error, "input operand to 'asm' does not match constraint 'i'");
      return NULL;
    }
  }

  // digit constraint means that we should reuse a previous operand
  else if (isdigit(*constraint)) {
    INT prev_index = *constraint - '0';
    FmtAssert(asm_constraint_tn[prev_index], 
              ("numeric matching constraint refers to NULL value"));
    ret_tn = asm_constraint_tn[prev_index];
  }
    
  else if (strchr("Xgmr", *constraint)) {
    TYPE_ID rtype = (load != NULL ? WN_rtype(load) : MTYPE_I4);
    if (rtype == MTYPE_I4 || rtype == MTYPE_U4) {
      ret_tn = (pref_tn ? pref_tn : Build_TN_Of_Mtype(rtype));
    } else {
      ErrMsg(EC_CG_Generic_Error, "operand type to 'asm' does not match constraint");
      return NULL;
    }
  }
  
  else if (strchr("b", *constraint)) {
    TYPE_ID rtype = MTYPE_B;
    TYPE_ID load_rtype = (load != NULL ?  WN_rtype(load) : MTYPE_I4);
    if (load_rtype == MTYPE_I4 || load_rtype == MTYPE_U4 ||
	load_rtype == MTYPE_B) {
      // whirl2ops.cxx is responsible of the [IU]4 -> B conversion if needed.
      *subclass = Register_Subclass_For_Mtype(load_rtype);
      ret_tn = (pref_tn ? pref_tn : Build_TN_Of_Mtype(rtype));
    } else {
      ErrMsg(EC_CG_Generic_Error, "operand type to 'asm' does not match constraint");
      return NULL;
    }      
  }
  else {
    FmtAssert(FALSE, ("ASM constraint <%s> not supported", constraint));
  }
 finish:
  asm_constraint_tn[asm_constraint_index] = ret_tn;
  asm_constraint_index++;
  
  return ret_tn;
}

#ifdef TARG_ST
/* ====================================================================
 *   strstr_No_Digit_After
 *
 *   Find the first occurrence of the substring needle in the string
 *   haystack, that is not directly followed by a digit [0-9].
 * ====================================================================
 */
static char *
strstr_No_Digit_After(char *haystack, const char *needle) {
  char *p = haystack;
  while ((p = strstr(p, needle)) != NULL) {
    char* leftover = p + strlen(needle);
    if (!(*leftover >= '0' && *leftover <= '9')) {
      return (p);
    }
    p = leftover;
  }
  return (NULL);
}
#endif

/* ====================================================================
 *   Replace_Substring
 * ====================================================================
 */
static char* 
Replace_Substring (
  char* in, 
  char* from, 
  char* to
)
{
  UINT  buflen = strlen(in) + 64;
  char* buf = (char*) alloca(buflen);
  char* p;
#ifdef TARG_ST
  // [TTh] To handle more than 10 parameters in asm stmt, we must
  //       ignore, for instance, %10, %11, .. when searching for %1
  while ((p = strstr_No_Digit_After(in, from)) != NULL) {
#else
  while ((p = strstr(in, from)) != NULL) {
#endif
    char* leftover = p + strlen(from);
    *p = '\0';
    while (strlen(in) + strlen(to) + strlen(leftover) >= buflen) {
      buflen = buflen * 2;
      buf = (char*) alloca(buflen);
    }
    sprintf(buf, "%s%s%s", in, to, leftover);
    in = strdup(buf);
  }
  return in;
}

/* ====================================================================
 *   CGTARG_Modify_Asm_String
 * ====================================================================
 */
//TB: add subclass parameter to know the exact register name
char* 
CGTARG_Modify_Asm_String (
  char* asm_string, 
  UINT32 position, 
  bool memory, 
  TN* tn,
  ISA_REGISTER_SUBCLASS rs
)
{
  char* name;
  if (TN_is_register(tn)) {
#ifdef TARG_ST
    name = (char*) REGISTER_extended_name(tn, rs);
#else
    ISA_REGISTER_CLASS cl = TN_register_class(tn);
    REGISTER reg = TN_register(tn);
    name = (char*) REGISTER_name(cl, reg);
#endif
    if (memory) {
      char *fmt = "@( %s + 0 )";
      char *buf = (char*) alloca(strlen(name)+strlen(fmt)+1);
      sprintf(buf, fmt, name);
      name = buf;
    }
  }
  else {
    FmtAssert(!memory && (TN_is_symbol(tn) || TN_has_value(tn)),
              ("ASM operand must be a register, a literal constant or a symbolic constant"));
    if (TN_has_value(tn)) {
      char* buf = (char*) alloca(32);
      sprintf(buf, "%lld", TN_value(tn));
      name = buf;
    } else if (TN_is_symbol(tn) && 
	       ST_name(TN_var(tn)) &&
	       *(ST_name(TN_var(tn))) != '\0' &&
	       TN_offset(tn) == 0 && 
	       TN_relocs(tn) == TN_RELOC_NONE) {
      ST *st = TN_var(tn);
#ifdef TARG_ST
      // +128 is enough for all separators and suffixes added to the name
      vstring buf = vstr_begin(strlen(ST_name(st))+128+1);
      CGEMIT_Qualified_Name(st, &buf);
      name = (char*) alloca(strlen(vstr_str(buf))+1);
      strcpy(name, vstr_str(buf));
      vstr_end(buf);
#else
      const char *st_name = ST_name(st);
      // +128 is enough for all separators and suffixes added to the name
      char* buf = (char*) alloca(strlen(st_name)+128+1);
      // Code extracted from r_qualified_name() function in cgemit.cxx.
      if (ST_is_export_local(st) && ST_class(st) == CLASS_VAR) {
	if (ST_level(st) == GLOBAL_SYMTAB)
	  sprintf (buf, "%s%s%d", st_name,
		   Label_Name_Separator, ST_index(st));
	else
	  sprintf (buf, "%s%s%d%s%d", st_name,
		   Label_Name_Separator, ST_pu(Get_Current_PU_ST()),
		   Label_Name_Separator, ST_index(st));
      } else {
	sprintf (buf, "%s%s", st_name, Symbol_Name_Suffix);
      }
      name = buf;
#endif
    } else {
      FmtAssert(!memory && (TN_is_symbol(tn) || TN_has_value(tn)),
		("ASM operand is not a valid symbolic constant"));
    }
  }
  
  char pattern[5];
#ifdef TARG_ST
  // [TTh] Handle more than 10 parameters
  sprintf(pattern, "%%%d", position);
#else
  sprintf(pattern, "%%%c", '0'+position);
#endif
  
  asm_string =  Replace_Substring(asm_string, pattern, name);

  if (TN_is_register(tn)) {
    for (INT i = 0; i < CGTARG_Num_Asm_Opnd_Modifiers; i++) {
      char modifier = CGTARG_Asm_Opnd_Modifiers[i];
#ifdef TARG_ST
      // [TTh] Handle more than 10 parameters
      sprintf(pattern, "%%%c%d", modifier, position);
#else
      sprintf(pattern, "%%%c%c", modifier, '0'+position);
#endif
      name = (char*) CGTARG_Modified_Asm_Opnd_Name(modifier, tn, name);
      asm_string  = Replace_Substring(asm_string, pattern, name);
    }
  }
  
    
  return asm_string;
}

/* ====================================================================
 *
 * CGTARG_Postprocess_Asm_String
 *
 * ====================================================================
 */
char *
CGTARG_Postprocess_Asm_String (
  char *s
)
{
  return s;
}

/* ====================================================================
 * ====================================================================
 *                            ISA Helpers:
 * ====================================================================
 * ====================================================================
 */

/* ====================================================================
 *   CGTARG_Load_From_Memory
 * ====================================================================
 */
void CGTARG_Load_From_Memory (
  TN *tn, 
  ST *mem_loc, 
  OPS *ops,
  VARIANT variant
)
{
  TYPE_ID mtype = TY_mtype(ST_type(mem_loc));

  Exp_Load (mtype, mtype, tn, mem_loc, 0, ops, variant);

  return;
}

/* ====================================================================
 *   CGTARG_Store_To_Memory
 * ====================================================================
 */
void CGTARG_Store_To_Memory(
  TN *tn,
  ST *mem_loc,
  OPS *ops,
  VARIANT variant
)
{
  TYPE_ID mtype = TY_mtype(ST_type(mem_loc));

  Exp_Store (mtype, tn, mem_loc, 0, ops, variant);
  return;
}

/* ====================================================================
 *   CGTARG_Speculative_Load
 * ====================================================================
 */
TOP
CGTARG_Speculative_Load (OP *op)
{
  TOP opcode = OP_code(op);
  TOP ld = TOP_UNDEFINED;

  return ld;
}

/* ====================================================================
 *   CGTARG_Predicated_Store
 * ====================================================================
 */
TOP
CGTARG_Predicated_Store (OP *op)
{
  TOP opcode = OP_code(op);
  TOP stw = TOP_UNDEFINED;
  return stw;
}

/* ====================================================================
 *   CGTARG_Predicated_Load
 * ====================================================================
 */
TOP
CGTARG_Predicated_Load (OP *op)
{
  FmtAssert(OP_load(op),("Op is not a load"));
  return OP_code(op);
}

/* ====================================================================
 *   CGTARG_Can_Fit_Immediate_In_Add_Instruction
 * ====================================================================
 */
BOOL
CGTARG_Can_Fit_Immediate_In_Add_Instruction (
  INT64 immed
)
{
  /* [CG]: Here we don't make assumption on how
     the code selector will treat extended immediate.
     So we assume that add does not have extended immediate
     form.
  */
  return ISA_LC_Value_In_Class (immed, LC_immed8);
}

/* ====================================================================
 *   CGTARG_Can_Load_Immediate_In_Single_Instruction
 * ====================================================================
 */
BOOL
CGTARG_Can_Load_Immediate_In_Single_Instruction (
  INT64 immed
)
{
  return ISA_LC_Value_In_Class (immed, LC_immed8);
}

/* ====================================================================
 * CGTARG_Can_Change_To_Brlikely
 * ====================================================================
 */
BOOL CGTARG_Can_Change_To_Brlikely(OP *xfer_op, TOP *new_opcode)
{
  return FALSE;
}

/* ====================================================================
 *   CGTARG_Branches_On_True
 * ====================================================================
 */
BOOL
CGTARG_Branches_On_True (
  OP* br_op, 
  OP* cmp_op
)
{
  FmtAssert(FALSE,("CGTARG_Branches_On_True: not implemented"));

  //  if (OP_opnd(br_op, OP_PREDICATE_OPND) == OP_result(cmp_op, 0)) {
  //    return TRUE;
  //  }
  return FALSE;
}

#ifdef TARG_ST
/* ====================================================================
 *   CGTARG_Able_To_Calculate_Remainder
 *   Return TRUE as being able to calculate the remainder number of
 *   iterations after unrolling
 * ====================================================================
 */
BOOL
CGTARG_Able_To_Calculate_Remainder (UINT32 ntimes)
{
  return TRUE;
}
#endif

/* ====================================================================
 *   CGTARG_Generate_Remainder_Branch
 * ====================================================================
 */
void
CGTARG_Generate_Remainder_Branch (
  TN *trip_count, 
  TN *label_tn,
  OPS *prolog_ops, 
  OPS *body_ops
)
{
  INT32 trip_size = TN_size(trip_count);

  if (TN_is_constant(trip_count)) {
    TN *lit_trip_count = trip_count;
    trip_count = CGTARG_gen_trip_count_TN(trip_size);
    Exp_Immediate(trip_count, lit_trip_count, TRUE, prolog_ops);
  }

  Exp_OP2(trip_size == 4 ? OPC_I4ADD : OPC_I8ADD,
	  trip_count,
	  trip_count,
	  Gen_Literal_TN(-1, trip_size),
	  body_ops);

  Exp_OP3v(OPC_TRUEBR,
	   NULL,
	   label_tn,
	   trip_count,
	   Get_Zero_TN(trip_size),
	   trip_size == 4 ? V_BR_I4NE : V_BR_I8NE,
	   body_ops);
}

/* ====================================================================
 *   CGTARG_Generate_Branch_Cloop
 * ====================================================================
 */

static BOOL Trace_CLOOP;
extern int Activate_Hwloop;


/** 
 * Auxiliary function that computes n with x = 2^n.
 * return -1 if n is not an integer.
 * 
 * @param x 
 * 
 * @return 
 */
static int
is_2_exp_n(int x) 
{
  int r1= -1;
  int y = x;
  // x&(x-1) == 0 if and only if x==2^n
  if (x>0 && (x&(x-1)) == 0) {
    for (r1=0; x>1; x>>=1, r1++);
  }
  return r1;
}

/** 
 * Auxiliary function that build the division operation (shr)
 * by dividing src by ntimes.
 * 
 * @bug times must have the form 2^n
 * 
 * @param dest 
 * @param src 
 * @param ntimes 
 * @param ops 
 * 
 * @return 
 */
static BOOL
Build_div(TN *dest, TN *src, INT32 ntimes, OPS *ops)
{
  int exponent2;
  FmtAssert(FALSE,("Not implemented"));
#if 0
  exponent2 = is_2_exp_n(ntimes);
  if (exponent2>=0 && exponent2<32)
    {
      // generate shift right
      Build_OP(TOP_shr_i5, dest, True_TN, src,
               Gen_Unique_Literal_TN(exponent2,4), ops);
    }
  else
    {
      // generate division
      FmtAssert(FALSE, ("ntimes != 2^n not implemented !!!\n"));
    }

#endif
  return TRUE;
}

static BOOL
BB_HWLoop_head(BB *bb) {
  FmtAssert(FALSE,("Not implemented"));
#if 0
  if (BB_loophead(bb)) {
    ANNOTATION *annot = ANNOT_Get(BB_annotations(bb), ANNOT_LOOPINFO);
    if (annot) {
      LOOPINFO *info = ANNOT_loopinfo(annot);
      if (info && LOOPINFO_is_HWLoop(info))
	return TRUE;
    }
  }
#endif
  return FALSE;
}

static BOOL
BB_HWLoop_tail(BB *bb) {
  FmtAssert(FALSE,("Not implemented"));
#if 0
  OP *br_op = BB_branch_op(bb);
  if (br_op && OP_Is_Counted_Loop(br_op))
    return TRUE;
#endif
  return FALSE;
}

static BOOL
Check_HWLoop_nest(LOOP_DESCR *loop) {
  FmtAssert(FALSE,("Not implemented"));
#if 0

  BB *loop_head = LOOP_DESCR_loophead(loop);
  BB *loop_tail = LOOP_DESCR_Find_Unique_Tail(loop);
  BB *bb, *inner_head;

  if (loop_tail == NULL) {
    if (Trace_CLOOP)
      fprintf(TFile, "Check_HWLoop_nest: No unique tail found for loop\n");
    return FALSE;
  }
  else if (loop_head == loop_tail)
    return TRUE;

  inner_head = NULL;
  for (bb = BB_next(loop_head); bb && (bb != loop_tail); bb = BB_next(bb)) {

    // Ignore blocks not nested into this loop
    if (!BB_SET_MemberP(LOOP_DESCR_bbset(loop), bb))
      continue;

    // This bb starts an hardware loop
    if (BB_HWLoop_head(bb)) {
      LOOP_DESCR *inner_loop = LOOP_DESCR_Find_Loop(bb);
      // Ignore more deeply nested loops, they have already been
      // checked for correct nesting
      if (LOOP_DESCR_nestlevel(inner_loop) == (LOOP_DESCR_nestlevel(loop)+1)) {
	// Starts a new hwloop
	if (inner_head == NULL)
	  inner_head = bb;
        else {
	  // An hardware loop is already started
	  if (Trace_CLOOP)
	    fprintf(TFile, "Check_HWLoop_nest: start HWLoop before end of previous one\n");
	  return FALSE;
	}
      }
    }

    // An hardware may contain a single basic block
    if (BB_HWLoop_tail(bb)) {
      LOOP_DESCR *inner_loop = LOOP_DESCR_Find_Loop(bb);
      // This is the tail of an immediately inner loop
      if (LOOP_DESCR_nestlevel(inner_loop) == (LOOP_DESCR_nestlevel(loop)+1)) {
	BB *head = LOOP_DESCR_loophead(inner_loop);
	// This tail ends an inner hardware loop
	if (head == inner_head)
	  inner_head = NULL;
	else {
	  // This ends an hardware loop for which we did not see the
	  // start
	  if (Trace_CLOOP)
	    fprintf(TFile, "Check_HWLoop_nest: end of HWLoop with no start\n");
	  return FALSE;
	}
      }
    }
  }

  if ((bb == loop_tail) && (inner_head == NULL))
    return TRUE;

  if (Trace_CLOOP) {
    if (bb == NULL)
      fprintf(TFile, "Check_HWLoop_nest: loop end address is before loop start address\n");
    else
      fprintf(TFile, "Check_HWLoop_nest: inner hardware loop do not end before enclosing loop\n");
  }

#endif
  return FALSE;
}

static REGISTER
Allocate_hwloop_reg(REGISTER_SET free_hwloop_regs) {
  // Choose a register among the free caller-save registers, or then
  // among the free callee-save register.
  FmtAssert(FALSE,("Not implemented"));
#if 0

  REGISTER_SET regset;
  REGISTER reg;

  regset = REGISTER_CLASS_caller_saves(ISA_REGISTER_CLASS_lr);
  regset = REGISTER_SET_Intersection(regset, free_hwloop_regs);
  for (reg = REGISTER_SET_Choose(regset);
       reg != REGISTER_UNDEFINED;
       reg = REGISTER_SET_Choose_Next(regset, reg)) {
    return reg;
  }

  regset = REGISTER_CLASS_callee_saves(ISA_REGISTER_CLASS_lr);
  regset = REGISTER_SET_Intersection(regset, free_hwloop_regs);
  for (reg = REGISTER_SET_Choose(regset);
       reg != REGISTER_UNDEFINED;
       reg = REGISTER_SET_Choose_Next(regset, reg)) {
    return reg;
  }
#endif
  return REGISTER_UNDEFINED;
}

static REGISTER
Allocate_jrgtudec_reg(REGISTER_SET free_jrgtudec_regs) {
  // Choose a register among the free caller-save registers, or then
  // among the free callee-save register.
  FmtAssert(FALSE,("Not implemented"));
#if 0

  REGISTER_SET regset;
  REGISTER reg;

  regset = REGISTER_CLASS_caller_saves(ISA_REGISTER_CLASS_gr);
  regset = REGISTER_SET_Intersection(regset, free_jrgtudec_regs);
  for (reg = REGISTER_SET_Choose(regset);
       reg != REGISTER_UNDEFINED;
       reg = REGISTER_SET_Choose_Next(regset, reg)) {
    return reg;
  }

  regset = REGISTER_CLASS_callee_saves(ISA_REGISTER_CLASS_gr);
  regset = REGISTER_SET_Intersection(regset, free_jrgtudec_regs);
  for (reg = REGISTER_SET_Choose(regset);
       reg != REGISTER_UNDEFINED;
       reg = REGISTER_SET_Choose_Next(regset, reg)) {
    return reg;
  }

#endif
  return REGISTER_UNDEFINED;
}

static REGISTER_SET
Reserve_regs(INT from, INT num) {

  REGISTER_SET reserved_regs = REGISTER_SET_EMPTY_SET;

  FmtAssert(FALSE,("Not implemented"));
#if 0

  // We consider that a register for a branch is local. We use the
  // second caller_save or callee_save register to reserve one
  // resource.

  REGISTER_SET regset;
  REGISTER_SET reserved_regs = REGISTER_SET_EMPTY_SET;
  REGISTER reg;
  INT to = from+num-1;
  int count = 0;

  regset = REGISTER_CLASS_caller_saves(ISA_REGISTER_CLASS_gr);
  for (reg = REGISTER_SET_Choose(regset);
       reg != REGISTER_UNDEFINED;
       reg = REGISTER_SET_Choose_Next(regset, reg)) {
    if (REGISTER_allocatable(ISA_REGISTER_CLASS_gr, reg)) {
      if ((count >= from) && (count <= to)) {
	reserved_regs = REGISTER_SET_Union1(reserved_regs, reg);
      }
      count ++;
    }
  }

  regset = REGISTER_CLASS_callee_saves(ISA_REGISTER_CLASS_gr);
  for (reg = REGISTER_SET_Choose(regset);
       reg != REGISTER_UNDEFINED;
       reg = REGISTER_SET_Choose_Next(regset, reg)) {
    if (REGISTER_allocatable(ISA_REGISTER_CLASS_gr, reg)) {
      if ((count >= from) && (count <= to)) {
	reserved_regs = REGISTER_SET_Union1(reserved_regs, reg);
      }
      count ++;
    }
  }
#endif
  return reserved_regs;
}

#define SCRATCH_REGS 1
#define BRANCH_REGS  1
#define GUARD_REGS   1

static REGISTER_SET
Reserve_scratch_regs() {
  FmtAssert(FALSE,("Not implemented"));
#if 0

  // Reserve SCRATCH_REGS scratch register, from caller_save and
  // callee_save registers.

  return Reserve_regs(0, SCRATCH_REGS);
#endif
}

static REGISTER_SET
Reserve_branch_regs() {

  FmtAssert(FALSE,("Not implemented"));
#if 0

  // Reserve BRANCH_REGS registers, after the SCRATCH registers, from
  // caller_save and callee_save registers. Since we consider that a
  // register for a branch is local, BRANCH_REGS is set to 1.

#endif
  return Reserve_regs(0+SCRATCH_REGS, BRANCH_REGS);
}

static REGISTER_SET
Reserve_guard_regs() {

  FmtAssert(FALSE,("Not implemented"));
#if 0

  // Reserve GUARD_REGS registers, after the SCRATCH and BRANCH
  // registers, from caller_save and callee_save registers. Since we
  // consider that a register for predicated code is local, GUARD_REGS
  // is set to 1.
#endif
  return Reserve_regs(0+SCRATCH_REGS+BRANCH_REGS, GUARD_REGS);
}

/** 
 * Try to generate "real" hw loop (based on setle/setls/setlc
 * instructions)
 * 
 * 
 * @param loop 
 * @param br_op 
 * @param trip_count_tn 
 * @param label_tn 
 * @param prolog_ops 
 * @param body_ops 
 * 
 * @return TRUE if hwloop generated
 */
static BOOL
Generate_Real_HWLoop(LOOP_DESCR *loop,
		     REGISTER_SET hwloop_regs,
                     OP *br_op,
                     TN *trip_count_tn,
                     TN *label_tn,
                     OPS *prolog_ops,
                     OPS *body_ops)
{
  FmtAssert(FALSE,("Not implemented"));
#if 0

  BB *head = LOOP_DESCR_loophead(loop);
  INT32 kind = -1;
  REGISTER hwreg = REGISTER_UNDEFINED;
  ANNOTATION *annot;
  for (annot = ANNOT_Get(BB_annotations(head), ANNOT_PRAGMA);
       annot != NULL;
       annot = ANNOT_Get(ANNOT_next(annot), ANNOT_PRAGMA)) {
    if (WN_pragma(ANNOT_pragma(annot)) == WN_PRAGMA_HWLOOP) {
      WN *pragma = ANNOT_pragma(annot);
      kind = (INT32)WN_pragma_arg1(pragma);
      if (kind == 1) {
	hwreg = REGISTER_MIN+(INT32)WN_pragma_arg2(pragma);
      }
      break;
    } 
  }
  

  Is_True(trip_count_tn, ("trip_count_tn cannot be NULL for a hardware loop.\n"));

  if (hwreg == REGISTER_UNDEFINED) {
    hwreg = Allocate_hwloop_reg(hwloop_regs);
    if (hwreg == REGISTER_UNDEFINED) {
      if (Trace_CLOOP)
	fprintf(TFile, "Could not allocate a hwloop register for loophead %d\n", BB_id(LOOP_DESCR_loophead(loop)));
      return FALSE;
    }
  }

  /* Check that HWloops are correctly statically nested. */
  if (!Check_HWLoop_nest(loop)) {
    if (Trace_CLOOP)
      fprintf(TFile, "No hardware loop for loophead %d: incorrect loop nest\n", BB_id(LOOP_DESCR_loophead(loop)));
    return FALSE;
  }

  if (hwreg == REGISTER_MIN+0) {
    saveHWloopRegisterLR0= TRUE;
  }
  TN *tn_hwreg = Build_Dedicated_TN(ISA_REGISTER_CLASS_lr, hwreg, 0);

  if (TN_has_value(trip_count_tn)) {
    INT64 tmp_lit = TN_value(trip_count_tn);
    if (tmp_lit >= 1<<16) {
      TN *nbiter = Gen_Register_TN(ISA_REGISTER_CLASS_gpr , 4);
      Expand_Immediate (nbiter, Gen_Literal_TN(TN_value(trip_count_tn), 4),
			FALSE, prolog_ops);
      trip_count_tn = nbiter;
    }
  }
  if (TN_has_value(trip_count_tn)) {
    Build_OP(TOP_setlc_i16, tn_hwreg, True_TN, tn_hwreg, trip_count_tn, prolog_ops);
  }
  else {
    Build_OP(TOP_setlc_r, tn_hwreg, True_TN, tn_hwreg, trip_count_tn, prolog_ops);
  }

  // setls
  Build_OP(TOP_setls, tn_hwreg, tn_hwreg, label_tn, prolog_ops);
  Set_LABEL_addr_saved(TN_label(label_tn));

  // setle
  BB* curbb = OP_bb(br_op);
  BB* nextbb = BB_Fall_Thru_Successor(curbb);
  LABEL_IDX label_nextbb = Gen_Label_For_BB(nextbb);
  
  TN* dstlab = Gen_Label_TN(label_nextbb, -4);

  Build_OP(TOP_setle, tn_hwreg, tn_hwreg, dstlab, prolog_ops);
  Set_LABEL_addr_saved(label_nextbb);

  LOOPINFO *info = LOOP_DESCR_loopinfo(loop);
  Is_True(info, ("LOOPINFO cannot be NULL for a loop to be mapped into an hardware loop.\n"));
  LOOPINFO_is_HWLoop(info) = TRUE;

  // Build instruction HWBRANCH LR0, label
  Build_OP(TOP_HWBRANCH, tn_hwreg, label_tn, body_ops);

#endif
  return TRUE;
}


/** 
 * Try to generate jrgtudec based hwloop.
 * 
 * 
 * @param loop 
 * @param br_op 
 * @param trip_count_tn 
 * @param label_tn 
 * @param prolog_ops 
 * @param body_ops 
 * 
 * @return TRUE if hwloop generated
 */
static BOOL
Generate_Jrgtudec_HWLoop(LOOP_DESCR* loop,
			 REGISTER_SET jrgtudec_regs,
                         OP *br_op,
                         TN *trip_count_tn,
                         TN *label_tn,
                         OPS *prolog_ops,
                         OPS *body_ops)
{
  FmtAssert(FALSE,("Not implemented"));
#if 0
  
  TN* gx;  /* loop guard register */
  TN* counter;  /* loop counter */
  TN* one;

  REGISTER jrgtudec_reg = Allocate_jrgtudec_reg(jrgtudec_regs);
  if (jrgtudec_reg == REGISTER_UNDEFINED) {
    if (Trace_CLOOP)
      fprintf(TFile, "Could not allocate a jrgtudec register for loophead %d\n", BB_id(LOOP_DESCR_loophead(loop)));
    return FALSE;
  }

  Is_True(trip_count_tn, ("trip_count_tn cannot be NULL for a Jrgtudec loop.\n"));

  gx = Gen_Register_TN(ISA_REGISTER_CLASS_gr , 1);
  counter = Gen_Register_TN(ISA_REGISTER_CLASS_gpr , 4);

  FmtAssert(label_tn!=NULL, ("NULL label_tn..cannot compile !\n"));

  if (Trace_CLOOP)
    fprintf(TFile, "trip_count_tn [const %d, reg "
           " %d], label_tn %d\n",
           TN_is_constant(trip_count_tn),
           TN_is_register(trip_count_tn),
           TN_is_label(label_tn));


  // build counter register from register or integer constant,
  if ( TN_is_constant(trip_count_tn))
    {
      // for numerical constant number of iterations. let's precompute
      // the division.
      if (TN_has_value(trip_count_tn))
        {
          Expand_Immediate
            (counter,
             Gen_Unique_Literal_TN(TN_value(trip_count_tn),4),
             FALSE,
             prolog_ops);
        }
      else
       // non-numerical constant. (is it possible ??) maybe symbolic.
       // the division (shr) is generated in the code.
        {
          Expand_Immediate(counter, trip_count_tn, FALSE, prolog_ops);
        }
    }
  else
    // loop count is unknown so the coutner value must be generated by
    // a division.
    {
      Exp_COPY(counter, trip_count_tn, prolog_ops);
    }
  
 // build cmp to prepare gx register.
  one = Gen_Unique_Literal_TN(1, 4);
  Build_OP(TOP_cmpgt_i8, gx, True_TN, counter, one, prolog_ops);
  // decrement counter from 1
  Build_OP(TOP_subu_i8, counter, True_TN, counter, one, prolog_ops);

  // add jrgtudec op to body_ops
  Build_OP(TOP_jrgtudec, gx, counter, gx, counter, label_tn, body_ops);

#endif
  return TRUE;
}

// This function sets in free_hwloops and free_jrgtudecs the
// registers, respectively for hardware loops and jrgtudec
// instructions, that are not used in a loop nest

static void
Analyze_loop_nest(LOOP_DESCR *loop, REGISTER_SET *free_hwloops, REGISTER_SET *free_jrgtudecs) {

  FmtAssert(FALSE,("Not implemented"));
#if 0

  REGISTER_SET init_hwloops = *free_hwloops;
  REGISTER_SET init_jrgtudecs = *free_jrgtudecs;

  BB *bb;

  BB *tail = LOOP_DESCR_Find_Unique_Tail(loop);

  FOR_ALL_BB_SET_members(LOOP_DESCR_bbset(loop), bb) {

    LOOP_DESCR *loop_inner = LOOP_DESCR_Find_Loop(bb);

    if (loop_inner == loop) {

      // This is a BB in the current loop
      if (BB_call(bb)) {
	// Mark caller save registers as used
	*free_hwloops = REGISTER_SET_Difference(*free_hwloops, REGISTER_CLASS_caller_saves(ISA_REGISTER_CLASS_lr));
	*free_jrgtudecs = REGISTER_SET_Difference(*free_jrgtudecs, REGISTER_CLASS_caller_saves(ISA_REGISTER_CLASS_gr));
      }
      // Look for branch instruction, except the one for the loop tail
      // that may be replaced by a hwloop/jrgtudec instruction.
      else if (bb != tail) {
	OP *br_op = BB_branch_op(bb);
	if (br_op && OP_cond(br_op)) {
	  // This loop contains control-flow inside. One local
	  // register is reserved for the branch instruction. However,
	  // since we have no precise idea of the register pressure,
	  // reserve a scratch register to avoid spilling (bug codex
	  // #19405).
	  *free_jrgtudecs = REGISTER_SET_Difference(*free_jrgtudecs, Reserve_scratch_regs());
	  *free_jrgtudecs = REGISTER_SET_Difference(*free_jrgtudecs, Reserve_branch_regs());
	}
      }

      OP *op;
      FOR_ALL_BB_OPs_FWD(bb, op) {
	// This loop contains predicated instructions. One local
	// register is reserved for the predicated instructions.
	// However, since we have no precise idea of the register
	// pressure, reserve an addition scratch register to avoid
	// spilling.
	if (OP_cond_def(op) || OP_select(op)) {
	  *free_jrgtudecs = REGISTER_SET_Difference(*free_jrgtudecs, Reserve_scratch_regs());
	  *free_jrgtudecs = REGISTER_SET_Difference(*free_jrgtudecs, Reserve_guard_regs());
	}
	// Take into account the clobber list of an asm statement
	if (OP_code(op) == TOP_asm) {
	  ASM_OP_ANNOT* asm_info = (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op);
	  *free_hwloops = REGISTER_SET_Difference(*free_hwloops,
					     ASM_OP_clobber_set(asm_info)[ISA_REGISTER_CLASS_lr]);
	  *free_jrgtudecs = REGISTER_SET_Difference(*free_jrgtudecs,
					       ASM_OP_clobber_set(asm_info)[ISA_REGISTER_CLASS_gr]);
	}
      }
    }

    // This is the head of an immediately inner loop
    else if (LOOP_DESCR_loophead(loop_inner) == bb &&
	     LOOP_DESCR_nestlevel(loop_inner) == (LOOP_DESCR_nestlevel(loop)+1)) {
      REGISTER_SET local_hwloop_set = init_hwloops;
      REGISTER_SET local_jrgtudec_set = init_jrgtudecs;
      Analyze_loop_nest(loop_inner, &local_hwloop_set, &local_jrgtudec_set);

      // Analyze the loop tail: is it an hardware loop ? with which
      // register ? Is it a jrgtudec ?
      BB *tail = LOOP_DESCR_Find_Unique_Tail(loop_inner);
      OP *br_op = tail ? BB_branch_op(tail) : NULL;
      if (br_op) {
	if (OP_Is_Counted_Loop(br_op)) {
	  TN *hwreg = OP_opnd(br_op, 0);
	  local_hwloop_set = REGISTER_SET_Difference1(local_hwloop_set, TN_register(hwreg));
	}
	else if (OP_code(br_op) == TOP_jrgtudec ||
		 OP_code(br_op) == TOP_jagtudec) {
	  // Must use the same rule here and in
	  // Generate_Jrgtudec_HWLoop to virtually allocate a guard
	  // register for the jrgtudec instruction.
	  REGISTER reg = Allocate_jrgtudec_reg(local_jrgtudec_set);
	  Is_True(reg != REGISTER_UNDEFINED, ("Inconsistent allocation on jrgtudec instruction.\n"));
	  local_jrgtudec_set = REGISTER_SET_Difference1(local_jrgtudec_set, reg);
	}
	else {
	  // This loop was not mapped to an hardware loop nor a
	  // jrgtudec. Just make sure that one local branch register
	  // is reserved.
	  local_jrgtudec_set = REGISTER_SET_Difference(local_jrgtudec_set, Reserve_branch_regs());
	}
      }

      *free_hwloops = REGISTER_SET_Intersection(*free_hwloops, local_hwloop_set);
      *free_jrgtudecs = REGISTER_SET_Intersection(*free_jrgtudecs, local_jrgtudec_set);
    }
  }
#endif
}

/** 
 * build hwloop at unroll time.
 * 
 * @param loop 
 * @param br_op 
 * @param trip_count_tn 
 * @param label_tn 
 * @param prolog_ops 
 * @param body_ops 
 * 
 * @return 
 */
BOOL
CGTARG_Generate_Branch_Cloop(LOOP_DESCR *loop,
                             OP *br_op,
			     TN *trip_count_tn,
			     TN *label_tn,
			     OPS *prolog_ops,
			     OPS *body_ops)
{ 

  Trace_CLOOP = Get_Trace(TP_CGLOOP, 0x8);

  BOOL res = FALSE;

  FmtAssert(FALSE,("Not implemented"));
#if 0

  if (Trace_CLOOP)
    fprintf(TFile, "Entering xp70 CGTARG_Generate_Branch_Cloop %d\n", Activate_Hwloop);

  REGISTER_SET free_hwloops;
  REGISTER_SET free_jrgtudecs;

  free_hwloops = REGISTER_CLASS_universe(ISA_REGISTER_CLASS_lr);
  free_hwloops = REGISTER_SET_Intersection(free_hwloops, REGISTER_CLASS_allocatable(ISA_REGISTER_CLASS_lr));

  free_jrgtudecs = REGISTER_CLASS_universe(ISA_REGISTER_CLASS_gr);
  free_jrgtudecs = REGISTER_SET_Intersection(free_jrgtudecs, REGISTER_CLASS_allocatable(ISA_REGISTER_CLASS_gr));

  // Examine the loop and its inner loops, to see if there are calls,
  // hardware loops or jrgtudec.

  Analyze_loop_nest(loop, &free_hwloops, &free_jrgtudecs);

  if (Trace_CLOOP) {
    fprintf(TFile, "Nesting %d, free hwregs=", LOOP_DESCR_nestlevel(loop));
    REGISTER_SET_Print(free_hwloops, TFile);
    fprintf(TFile, ", free jrgtudecregs=");
    REGISTER_SET_Print(free_jrgtudecs, TFile);
    fprintf(TFile, "\n");
  }
  
  BB *head = LOOP_DESCR_loophead(loop);
  INT32 kind = -1;
  ANNOTATION *annot;
  for (annot = ANNOT_Get(BB_annotations(head), ANNOT_PRAGMA);
       annot != NULL;
       annot = ANNOT_Get(ANNOT_next(annot), ANNOT_PRAGMA)) {
    if (WN_pragma(ANNOT_pragma(annot)) == WN_PRAGMA_HWLOOP) {
      WN *pragma = ANNOT_pragma(annot);
      kind = (INT32)WN_pragma_arg1(pragma);
    } 
  }

  if ((kind == 1) || (kind == -1 && (Activate_Hwloop&1)))
    res = Generate_Real_HWLoop(loop, free_hwloops, br_op, trip_count_tn, label_tn, prolog_ops, body_ops);

  if (res)
    PU_Has_Hwloops = TRUE;

  if (res && Trace_CLOOP)
    fprintf(TFile, "hardware loop has been generated for loop head %d\n", BB_id(LOOP_DESCR_loophead(loop)));
  
  if ((kind == 2) || (kind == -1 && !res && (Activate_Hwloop&2)))  // "real" hwloop failed.
    // try jrgtudec loop. ( Check about jagtudec !!)
    {
      res = Generate_Jrgtudec_HWLoop(loop, free_jrgtudecs, br_op, trip_count_tn, label_tn, prolog_ops, body_ops);
      if (res && Trace_CLOOP)
          fprintf(TFile, "jrgtudec loop has been generated for loop head %d\n", BB_id(LOOP_DESCR_loophead(loop)));
    }

  if (Trace_CLOOP)
    fprintf(TFile, "Exiting xp70 CGTARG_Generate_Branch_Cloop with res %d\n",
           res);
#endif
  return res;
}

/* ====================================================================
 * ====================================================================
 *                 Scheduling related stuff:
 * ====================================================================
 * ====================================================================
 */

/* ====================================================================
 *   CGTARG_Max_OP_Latency
 * ====================================================================
 */
INT32
CGTARG_Max_OP_Latency (
  OP *op
)
{
  INT i;
  INT max_latency;

  max_latency = 0;

  for (i = 0; i < OP_results(op); i++) {
    INT latency = CGTARG_Max_RES_Latency(op, i); 
    if (latency > max_latency) max_latency = latency;
  }
  return max_latency;
}

static BOOL
is_constant_register (mUINT16 class_n_reg)
{
  return class_n_reg == CLASS_AND_REG_zero
    || class_n_reg == CLASS_AND_REG_fzero
    || class_n_reg == CLASS_AND_REG_fone;
}

static void
init_register_use (void)
{
  INT i;
  ISA_REGISTER_CLASS c;
  ISA_REGISTER_SUBCLASS sc;

  // Initialize earliest_regclass_use/earliest_regsubclass_use,
  // arrays containing the earliest cycle on which a register of
  // each class/subclass may be accessed as an operand.
  for (i = 0; i <= ISA_REGISTER_CLASS_MAX; i++) {
    earliest_regclass_use[i] = INT_MAX;
  }
  for (i = 0; i <= ISA_REGISTER_SUBCLASS_MAX; i++) {
    earliest_regsubclass_use[i] = INT_MAX;
  }

  // Iterate over all operations, setting
  // earliest_regclass_use and earliest_regsubclass use
  // to the minimum operand access times found.
  for (i = 0; i < TOP_count; i++) {
    if (ISA_SUBSET_LIST_Member (ISA_SUBSET_List, (TOP)i)
	&& !TOP_is_dummy ((TOP)i)) {
      const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info ((TOP)i);
      const INT n_opnds = ISA_OPERAND_INFO_Operands (oinfo);
      for (INT opnd = 0; opnd < n_opnds; opnd++) {
	const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand (oinfo, opnd);
	if (ISA_OPERAND_VALTYP_Is_Register (otype)) {
	  ISA_REGISTER_CLASS c = ISA_OPERAND_VALTYP_Register_Class (otype);
	  ISA_REGISTER_SUBCLASS sc =
	    ISA_OPERAND_VALTYP_Register_Subclass (otype);
	  INT usetime = TSI_Operand_Access_Time ((TOP)i, opnd);
	  if (sc == ISA_REGISTER_SUBCLASS_UNDEFINED) {
	    if (usetime < earliest_regclass_use[c]) {
	      earliest_regclass_use[c] = usetime;
	    }
	  } else {
	    if (usetime < earliest_regsubclass_use[sc]) {
	      earliest_regsubclass_use[sc] = usetime;
	    }
	  }
	}
      }
    }
  }

  // Now set earliest_dedicated_reg_use and earliest_assigned_reg_use,
  // based on the class information.
  // For a register that lives in multiple subclasses, we have
  // to assume the earliest-use of all those subclasses.
  // For earliest_assigned_reg_use, singleton subclasses are filtered out:
  // dedicated registers must always be used for singleton subclass operands.
  FOR_ALL_ISA_REGISTER_CLASS (c) {
    REGISTER_SET rset = REGISTER_CLASS_universe (c);
    REGISTER reg;
    FOR_ALL_REGISTER_SET_members (rset, reg) {
      INT i = reg - REGISTER_MIN;
      earliest_dedicated_reg_use[c][i] = earliest_regclass_use[c];
      earliest_assigned_reg_use[c][i] = earliest_regclass_use[c];
    }
  }
  FOR_ALL_ISA_REGISTER_SUBCLASS(sc) {
    const ISA_REGISTER_SUBCLASS_INFO *info =
      ISA_REGISTER_SUBCLASS_Info (sc);
    ISA_REGISTER_CLASS c = ISA_REGISTER_SUBCLASS_INFO_Class (info);
    BOOL singleton = (ISA_REGISTER_SUBCLASS_INFO_Count (info) == 1);
    REGISTER_SET rset = REGISTER_SUBCLASS_members (sc);
    REGISTER reg;
    FOR_ALL_REGISTER_SET_members (rset, reg) {
      INT i = reg - REGISTER_MIN;
      if (earliest_dedicated_reg_use[c][i] > earliest_regsubclass_use[sc]) {
	earliest_dedicated_reg_use[c][i] = earliest_regsubclass_use[sc];
      }
      if (! singleton
	  && earliest_assigned_reg_use[c][i] > earliest_regsubclass_use[sc]) {
	earliest_assigned_reg_use[c][i] = earliest_regsubclass_use[sc];
      }
    }      
  }

  // Now recalculate earliest_regclass_use and
  // earliest_regsubclass_use as the minimum
  // earliest_assigned_reg_use for each of the members.
  FOR_ALL_ISA_REGISTER_CLASS (c) {
    REGISTER_SET rset = REGISTER_CLASS_universe (c);
    REGISTER reg;
    FOR_ALL_REGISTER_SET_members (rset, reg) {
      INT i = reg - REGISTER_MIN;
      if (earliest_assigned_reg_use[c][i] < earliest_regclass_use[c]) {
	earliest_regclass_use[c] = earliest_assigned_reg_use[c][i];
      }
    }
  }
  FOR_ALL_ISA_REGISTER_SUBCLASS(sc) {
    const ISA_REGISTER_SUBCLASS_INFO *info =
      ISA_REGISTER_SUBCLASS_Info (sc);
    ISA_REGISTER_CLASS c = ISA_REGISTER_SUBCLASS_INFO_Class (info);
    REGISTER_SET rset = REGISTER_SUBCLASS_members (sc);
    REGISTER reg;
    FOR_ALL_REGISTER_SET_members (rset, reg) {
      INT i = reg - REGISTER_MIN;
      if (earliest_assigned_reg_use[c][i] < earliest_regsubclass_use[sc]) {
	earliest_regsubclass_use[sc] = earliest_assigned_reg_use[c][i];
      }
    }
  }

#if 0
  // Debug
  FOR_ALL_ISA_REGISTER_CLASS (c) {
    printf ("Class %d: earliest use = %d\n", c, earliest_regclass_use[c]);
    REGISTER_SET rset = REGISTER_CLASS_universe (c);
    REGISTER reg;
    FOR_ALL_REGISTER_SET_members (rset, reg) {
      INT i = reg - REGISTER_MIN;
      printf ("  %d: earliest dedicated use = %d, assigned use = %d\n",
	      i, earliest_dedicated_reg_use[c][i],
	      earliest_assigned_reg_use[c][i]);
    }
  }
  FOR_ALL_ISA_REGISTER_SUBCLASS (sc) {
    printf ("Subclass %d: earliest use = %d\n", sc, earliest_regsubclass_use[sc]);
    const ISA_REGISTER_SUBCLASS_INFO *info =
      ISA_REGISTER_SUBCLASS_Info (sc);
    ISA_REGISTER_CLASS c = ISA_REGISTER_SUBCLASS_INFO_Class (info);
    REGISTER_SET rset = REGISTER_SUBCLASS_members (sc);
    REGISTER reg;
    FOR_ALL_REGISTER_SET_members (rset, reg) {
      INT i = reg - REGISTER_MIN;
      printf ("  %d: earliest dedicated use = %d, assigned use = %d\n",
	      i, earliest_dedicated_reg_use[c][i],
	      earliest_assigned_reg_use[c][i]);
    }
  }
  // End of debug
#endif

  earliest_regclass_use_initialized = TRUE;
}


static INT
get_earliest_regclass_use (ISA_REGISTER_CLASS c,
			   ISA_REGISTER_SUBCLASS sc)
{
  INT usetime;
  if (!earliest_regclass_use_initialized) {
    init_register_use();
  }
  if (sc == ISA_REGISTER_SUBCLASS_UNDEFINED) {
    usetime = earliest_regclass_use[c];
  } else {
    usetime = earliest_regsubclass_use[sc];
  }
  FmtAssert (usetime != INT_MAX, ("earliest_regclass_use not set for class = %d, subclass = %d\n", (int)c, (int)sc));
  return usetime;
}

static INT
get_earliest_dedicated_reg_use (ISA_REGISTER_CLASS rclass, REGISTER reg) {
  if (!earliest_regclass_use_initialized) {
    init_register_use();
  }
  INT usetime = earliest_dedicated_reg_use[rclass][reg - REGISTER_MIN];
  FmtAssert (usetime != INT_MAX, ("earliest_reg_use not set for class = %d, register = %d\n", (int)rclass, (int)reg));
  return usetime;
}

static INT
get_earliest_assigned_reg_use (ISA_REGISTER_CLASS rclass, REGISTER reg) {
  if (!earliest_regclass_use_initialized) {
    init_register_use();
  }
  INT usetime = earliest_assigned_reg_use[rclass][reg - REGISTER_MIN];
  FmtAssert (usetime != INT_MAX, ("earliest_reg_use not set for class = %d, register = %d\n", (int)rclass, (int)reg));
  return usetime;
}

/* ====================================================================
 *   CGTARG_Max_RES_Latency
 * ====================================================================
 */
INT32
CGTARG_Max_RES_Latency (
			OP *op,
			INT idx
			)
{
  //	asm latency always forced to 1.
  if (OP_code(op) == TOP_asm) {
    return 1;
  }
  //    Instructions writing into regiser r0.0 have a 0 latency.
  if (is_constant_register (TN_register_and_class (OP_result (op, idx)))) {
    return 0;
  }
  INT result_avail = TSI_Result_Available_Time (OP_code (op), idx);
  INT opnd_used;
  if (TN_register(OP_result(op,idx)) != REGISTER_UNDEFINED) {
    if (TN_is_dedicated (OP_result(op, idx))) {
      opnd_used = get_earliest_dedicated_reg_use(TN_register_class(OP_result(op,idx)),
						 TN_register(OP_result(op,idx)));
    } else {
      opnd_used = get_earliest_assigned_reg_use(TN_register_class(OP_result(op,idx)),
						TN_register(OP_result(op,idx)));
    }
  } else {
    ISA_REGISTER_CLASS c = OP_result_reg_class (op, idx);
    ISA_REGISTER_SUBCLASS sc = OP_result_reg_subclass (op, idx);
    opnd_used = get_earliest_regclass_use (c, sc);
  }
  INT latency = result_avail - opnd_used;
  
  return latency;
}

/* ====================================================================
 *   CGTARG_Same_Register
 * Helper to know if two TN are equal for dependency purpose.
 * ====================================================================
 */
static BOOL
CGTARG_Same_Register( TN *tn1, TN *tn2 ) {
  if(TN_register(tn1) == REGISTER_UNDEFINED && 
     TN_register(tn2) == REGISTER_UNDEFINED && 
     tn1 == tn2) {
    return TRUE;
  }
  else if(TN_register(tn1) != REGISTER_UNDEFINED &&
	  TN_register(tn2) != REGISTER_UNDEFINED &&
	  TN_register_and_class(tn1) == TN_register_and_class(tn2)) {
    return TRUE;
  }
  return FALSE;
}

/* ====================================================================
 *   CGTARG_Adjust_Latency
 * ====================================================================
 */
void
CGTARG_Adjust_Latency (
  OP *pred_op, 
  OP *succ_op, 
  CG_DEP_KIND kind, 
  UINT8 opnd, 
  INT16 *latency
)
{
  INT i;
  const TOP pred_code = OP_code(pred_op);
  const TOP succ_code = OP_code(succ_op);

#ifdef Is_True_On
  // For experiments to add the value of ADD_LATENCY to all regin lantecies.
  if (getenv("ADD_LATENCY"))
    if (kind == CG_DEP_REGIN) *latency += atoi(getenv("ADD_LATENCY"));
#endif

#ifdef Is_True_On
  // Exeperimental for debugging alias. Force additional latencies
  // to load, in order to have more exposed alias problems.
  if (getenv("LOAD_LATENCY"))
    if (OP_load(pred_op) && kind == CG_DEP_REGIN) {*latency += atoi(getenv("LOAD_LATENCY")); }
#endif

  // 1. Special asm latencies
  //    For input treated by the standard READ/WRITE latencies (is is done?)
  //	For output, force to 1.
  if (pred_code == TOP_asm) *latency = 1;

  // 2. Any REGOUT latency forced to 1 at least
  if (kind == CG_DEP_REGOUT && *latency < 1) *latency = 1;
  
  // 3. Special latency on constant registers, even on output dependence.
  /* FdF: MBTst15896 Fixed operand for REGOUT. */
  if ((kind == CG_DEP_REGIN &&
       is_constant_register (TN_register_and_class (OP_opnd(succ_op, opnd))))
      || (kind == CG_DEP_REGANTI
	  && is_constant_register (TN_register_and_class (OP_opnd(pred_op, opnd))))
      || (kind == CG_DEP_REGOUT
	  && is_constant_register (TN_register_and_class (OP_result(succ_op, opnd)))))
    *latency = 0;

  return;
}

#if 0
/* ====================================================================
 *   CGTARG_ARC_Sched_Latency
 *
 *    	  Wrapper function for ARC_latency to let us fix up some cases 
 *	  where it returns a result that just doesn't make sence.  In
 *	  particular a latency of -1 for the pre-branch latency makes no
 *	  scheduling sense for CPUs which have same-cycle branch shadows.
 *	  Should be 0.
 *
 *   TODO: see if this functionality is redundant ?
 * ====================================================================
 */
INT CGTARG_ARC_Sched_Latency(
  ARC *arc
)
{
  if ( ARC_kind(arc) == CG_DEP_PREBR && PROC_has_same_cycle_branch_shadow() )
    return 0;
  else
    return ARC_latency(arc);
}
#endif

/* ====================================================================
 *   CGTARG_Bundle_Stop_Bit_Available
 * ====================================================================
 */
BOOL 
CGTARG_Bundle_Stop_Bit_Available(TI_BUNDLE *bundle, INT slot)
{
  // Return TRUE the stop-bit is already set.
  if (TI_BUNDLE_stop_bit(bundle, slot)) return TRUE;

  return TI_BUNDLE_Stop_Bit_Available(bundle, slot);
}

/* ====================================================================
 *   Get_Extended_Opcode
 * ====================================================================
 */
static TOP
Get_Extended_Opcode (
  TOP opcode
)
{
  // Arthur: a hack -- I use the fact that all _ii opcodes follow
  //         immediately corresponding _i opcodes ...
  return (TOP) (opcode+1);
}

/* =========================================================================
 *   CGTARG_Get_Info_For_Common_Base_Opt
 *   Get information on memory op used in common base pointer optimisation.
 * =========================================================================
 */
void
CGTARG_Get_Info_For_Common_Base_Opt( INT *min_offset_alignment, INT *min_offset, INT *max_offset) {
  *min_offset_alignment = 4; /* word alignment due to scaling factor on AM offset. */
  *min_offset = ISA_LC_Min (LC_s9); /* LC_imm_scf0_s9 for byte memory accesses. */
  *max_offset = ISA_LC_Max (LC_s9);
}


/* ====================================================================
 *   CGTARG_offset_is_extended
 *   Does this offset TN is an extended immediate operand ?
 * ====================================================================
 */
BOOL
CGTARG_offset_is_extended(TN *offset, INT64 *val) {

  return FALSE;
}
  
/* ====================================================================
 *   CGTARG_need_extended_Opcode
 *   Does this OP have extended immediate operand ?
 * ====================================================================
 */
BOOL
CGTARG_need_extended_Opcode(OP *op, TOP *etop) {
  INT i;

  BOOL extra_slot_reqd = FALSE;

  return extra_slot_reqd;

}

static int compare_prop (const void *p1, const void *p2)
{
  ISA_EXEC_UNIT_PROPERTY prop1 = *(ISA_EXEC_UNIT_PROPERTY *)p1;
  ISA_EXEC_UNIT_PROPERTY prop2 = *(ISA_EXEC_UNIT_PROPERTY *)p2;
  return (prop2 > prop1) - (prop1 > prop2);
}

static void
init_templates (void)
{
  for (INT t = 0; t < ISA_MAX_BUNDLES; ++t) {
    INT n_props = ISA_EXEC_Slot_Count(t);
    for (INT i = 0; i < n_props; ++i) {
      template_props[t][i] = ISA_EXEC_Slot_Prop(t, i);
    }
    qsort(template_props[t], n_props, sizeof(ISA_EXEC_UNIT_PROPERTY),
	  compare_prop);
  }
}

static INT
find_matching_template (ISA_EXEC_UNIT_PROPERTY bundle_props[ISA_BUNDLE_MAX_SLOTS],
			INT n_bundle_props,
			BOOL match_alignment,
			UINT32 pc,
			BOOL allow_subset)
{
  for (INT t = 0; t < ISA_MAX_BUNDLES; ++t) {
    if (match_alignment
	&& ((pc % (ISA_EXEC_Base(t)/8)) != (ISA_EXEC_Bias(t)/8))) {
      continue;
    }
    const ISA_EXEC_UNIT_PROPERTY *t_props = template_props[t];
    INT n_t_props = ISA_EXEC_Slot_Count(t);
    if (n_bundle_props == n_t_props
	&& memcmp(bundle_props, t_props,
		  sizeof(ISA_EXEC_UNIT_PROPERTY) * n_bundle_props) == 0) {
      // Perfect match.
      return t;
    } else if (allow_subset) {
      if (n_t_props < n_bundle_props) {
	// This template is too small to match.
	continue;
      }
      INT bundle_i = 0;
      INT template_i = 0;
      while (bundle_i < n_bundle_props) {
        // [vcdv] loop early exits added to avoid accessing 
        // t_props[template_i] with template_i>=n_t_props
        if (template_i == n_t_props) {
          bundle_i = n_bundle_props+1;
          break;
        }
	while (bundle_props[bundle_i] < t_props[template_i]) {
	  template_i++;
	  if (template_i == n_t_props) {
	    // Failed to find a match.
	    break;
	  }
	}
	if (template_i == n_t_props) {
	  break;
	}
	if (bundle_props[bundle_i] > t_props[template_i]) {
	  // Failed to find a match.
	  break;
	}
	bundle_i++;
	template_i++;
      }
      if (bundle_i == n_bundle_props) {
	// Match found.
	return t;
      }
    }
  }
  return -1;
}

/* ====================================================================
 *   CGTARG_Bundle_Slot_Available
 * ====================================================================
 */
BOOL 
CGTARG_Bundle_Slot_Available(TI_BUNDLE              *bundle,
			     OP                     *op,
			     INT                     slot,
			     ISA_EXEC_UNIT_PROPERTY *prop, 
			     BOOL                    stop_bit_reqd,
			     const CG_GROUPING      *grouping
			     )
{
  INT i;

  if (TI_BUNDLE_slot_filled(bundle, slot)) {
    return FALSE;
  }
  if (slot > 0 && !TI_BUNDLE_slot_filled(bundle, slot - 1)) {
    return FALSE;
  }

  *prop = ISA_EXEC_Unit_Prop(OP_code(op));

  ISA_EXEC_UNIT_PROPERTY bundle_props[ISA_BUNDLE_MAX_SLOTS];
  INT n_bundle_props = 0;
  
  FOR_ALL_SLOT_MEMBERS(bundle, i) {
    if (TI_BUNDLE_slot_filled(bundle, i)) {
      bundle_props[n_bundle_props++] = TI_BUNDLE_exec_property(bundle, i);
    } else if (i == slot) {
      bundle_props[n_bundle_props++] = *prop;
    }
  }
  qsort (bundle_props, n_bundle_props, sizeof(ISA_EXEC_UNIT_PROPERTY),
	 compare_prop);
  INT t = find_matching_template(bundle_props, n_bundle_props,
			     FALSE, 0, TRUE);
  if (t != -1) {
    Set_TI_BUNDLE_pack_code(bundle, t);
    return TRUE;
  }

  return FALSE;
}

/* ====================================================================
 *   CGTARG_Handle_Bundle_Hazard
 *
 *   Handle all target-specific bundle hazards in this routine.
 * ====================================================================
 */
void
CGTARG_Handle_Bundle_Hazard (OP                          *op, 
			     TI_BUNDLE                   *bundle, 
			     VECTOR                      *bundle_vector,
			     BOOL                        slot_avail, 
			     INT                         slot_pos, 
			     INT                         max_pos,
			     BOOL                        stop_bit_reqd,
			     ISA_EXEC_UNIT_PROPERTY      prop)
{
  INT ti_err = TI_RC_OKAY;
  INT template_bit = TI_BUNDLE_Return_Template(bundle);
  FmtAssert (template_bit != -1, ("Illegal template encoding"));

  //
  // fill with nops
  //
  OP *prev_op = NULL;
  BOOL bundle_full = TI_BUNDLE_Is_Full(bundle, &ti_err);
  FmtAssert(ti_err != TI_RC_ERROR, ("%s", TI_errmsg));

  // if the <bundle> is full, set the <end_group> marker appropriately.
  if (bundle_full) {

    // set <end_group> marker if the bundle is full
    if (prev_op) {
      // if we added a noop after:
      Set_OP_end_group(prev_op);
      VECTOR_Reset (*bundle_vector);
    }
    else {
      Set_OP_end_group(op);
      VECTOR_Reset (*bundle_vector);
    }
    //    *clock = *clock + 1;
  }

  return;
}

/* ====================================================================
 *   CGTARG_Finish_Bundle: fill empty slots with nops
 * ====================================================================
 */
void CGTARG_Finish_Bundle(OP                          *op, 
			  TI_BUNDLE                   *bundle)
{
  INT template_bit = TI_BUNDLE_Return_Template(bundle);
  FmtAssert (template_bit != -1, ("Illegal template encoding"));
  // [SC] No work to do here.  We perform a post-pass to
  // insert padding nops.
}

// Create a label '.'
static LABEL_IDX
Gen_Label_PC() {
  LABEL_IDX lab;
  LABEL *label;

  label = &New_LABEL(CURRENT_SYMTAB, lab);
  // FdF: Need the extra space, because cgemit checks if the first
  // character is a digit
  LABEL_Init (*label, Save_Str(" 1"), LKIND_DEFAULT);

  return lab;
}

// Replace the sequence ;; NOP ;; NOP ;; by ;; GOTO 1 ;;
static OP*
NOPs2Goto (
  OP *op,
  LABEL_IDX lab_PC
)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  OP *goto_op;

  goto_op = Mk_OP(TOP_goto, Gen_Label_TN(lab_PC, 0));
  BB_Insert_Op(OP_bb(op), op, goto_op, FALSE);
  
  // It is guarantee that OP_scyle(op) is correct in case it is the
  // last OP of the basic block, while OP_scycle(OP_prev(op)) may be
  // -1.
  OP_scycle(goto_op) = OP_scycle(op)-1;
  Set_OP_bundled (goto_op);
  Set_OP_end_group(goto_op);

  BB_Remove_Op(OP_bb(op), OP_prev(op));
  BB_Remove_Op(OP_bb(op), op);

  return goto_op;
#endif
}

#define Bundle_is_NOP(op, start) (start && OP_end_group(op) && OP_noop(op))

// ======================================================================
//  Post-pass to replace a sequence of ;; nop ;; nop ;; by ;; goto 1 ;;
//  This is activated under option -CG:nop2goto=1
// ======================================================================
static void NOPs_to_GOTO (
  BB *bb
)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0

  OP *op = BB_first_op(bb);
  bool bundle_NOP; // Did we find a NOP bundle
  bool bundle_start; // Is the current OP the first one in a bundle
  LABEL_IDX label_PC;

  // Create a new label for each basic block. It would be possible to
  // create only one by PU, but the bundler works currently at the
  // basic block level.
  label_PC = Gen_Label_PC();

  // Initial state
  bundle_NOP = FALSE;
  bundle_start = TRUE;
  
  while (op != NULL) {

    // Previous bundle is a NOP, and the current one also
    if (bundle_NOP && Bundle_is_NOP(op, bundle_start))
      op = NOPs2Goto(op, label_PC);

    // Set the state for the next operation.
    bundle_NOP = (Bundle_is_NOP(op, bundle_start));
    bundle_start = OP_end_group(op);
    op = OP_next(op);
  }
#endif
}

static UINT32
update_bundle_pc (
		  OP *op,
		  UINT32 pc
		  )
{
  if (Trace_HB) {
    fprintf (TFile, "update_bundle_pc: %lx: ", (long)pc);
    Print_OP_No_SrcLine (op);
    fprintf (TFile, "\n");
  }
	     
  if (OP_code(op) == TOP_asm) {
    pc = (pc + DEFAULT_FUNCTION_ALIGNMENT - 1) & ~(DEFAULT_FUNCTION_ALIGNMENT - 1);
  } else {
    const ISA_PACK_INFO *pinfo = ISA_PACK_Info(OP_code(op));
    pc += ISA_PACK_INFO_Bit_Size(pinfo) / 8;
  }
  return pc;
}

static OP *
Pad_Bundle (
	    UINT32 *pc,
	    OP *bundle_start,
            INT *free_slots_in_previous_bundle,
	    INT *scycle_delta
	    )
{
  ISA_EXEC_UNIT_PROPERTY bundle_props[ISA_BUNDLE_MAX_SLOTS];
  INT n_bundle_props = 0;
  UINT32 bundle_start_pc = *pc;

  OP *last_in_bundle = bundle_start;
  while (!OP_end_group(last_in_bundle)) {
    last_in_bundle = OP_next(last_in_bundle);
  }
  OP *bundle_end = OP_next(last_in_bundle);
  OP *op;
  for (op = bundle_start; op != bundle_end; op = OP_next(op)) {
    ISA_EXEC_UNIT_PROPERTY prop = ISA_EXEC_Unit_Prop(OP_code(op));
    for (INT w = 0; w < ISA_PACK_Inst_Words(OP_code(op)); w++) {
      bundle_props[n_bundle_props++] = prop;
    }
    *pc = update_bundle_pc(op, *pc);
    if (OP_scycle(op) != -1) { OP_scycle(op) += *scycle_delta; }
  }
  qsort(bundle_props, n_bundle_props, sizeof(ISA_EXEC_UNIT_PROPERTY),
	compare_prop);
  
  // Find a template that matches bundle_props.
  INT t;
  while ((t = find_matching_template(bundle_props, n_bundle_props,
				     TRUE, bundle_start_pc, FALSE)) == -1) {
    // No match, so insert a nop.
    OP *noop = Mk_OP(TOP_nop);
    Set_OP_end_group(noop);
    Set_OP_bundled(noop);
    if (n_bundle_props == ISA_MAX_SLOTS) {
      // No room for nops in current bundle, so insert them in
      // previous bundle.
      if (*free_slots_in_previous_bundle == 0) {
	// No room for nops in previous bundle either, so create
	// a new bundle between current and previous.
	DevWarn("Creating nop bundle to satisfy alignment\n");
	*free_slots_in_previous_bundle = ISA_MAX_SLOTS - 1;
	BB_Insert_Op_Before(OP_bb(bundle_start), bundle_start, noop);
	OP_scycle(noop) = OP_scycle(bundle_start);
	// Inserting a nop bundle bumps all the start cycles by 1.
	(*scycle_delta)++;
	for (op = bundle_start; op != bundle_end; op = OP_next(op)) {
	  if (OP_scycle(op) != -1) { OP_scycle(op)++; }
	}
      } else {
	OP *last_in_prev_bundle = OP_prev(bundle_start);
	OP_scycle(noop) = OP_scycle(last_in_prev_bundle);
	Reset_OP_end_group(last_in_prev_bundle);
	DevWarn("Inserting nop in previous bundle to satisfy alignment\n");
	BB_Insert_Op_After(OP_bb(last_in_prev_bundle), last_in_prev_bundle, noop);
      }
      bundle_start_pc = update_bundle_pc(noop, bundle_start_pc);
    } else {
      OP_scycle(noop) = OP_scycle(last_in_bundle);
      Reset_OP_end_group(last_in_bundle);
      BB_Insert_Op_After(OP_bb(last_in_bundle), last_in_bundle, noop);
      last_in_bundle = noop;
      for (INT w = 0; w < ISA_PACK_Inst_Words(OP_code(noop)); w++) {
	FmtAssert(n_bundle_props < ISA_MAX_SLOTS, ("Pad_Bundle: too many nops inserted\n"));
	bundle_props[n_bundle_props++] = ISA_EXEC_Unit_Prop(OP_code(noop));
	qsort(bundle_props, n_bundle_props, sizeof(ISA_EXEC_UNIT_PROPERTY),
	      compare_prop);
      }
    }
    *pc = update_bundle_pc(noop, *pc);
  }
  *free_slots_in_previous_bundle = (ISA_MAX_SLOTS - n_bundle_props);
  return bundle_end;
}

static void Pad_Bundles_With_NOPs (
				   BB *bb,
				   INT *scycle_delta
)
{
  static UINT32 hot_addr = 0;
  static UINT32 cold_addr = 0;
  static PU_IDX pad_bundles_current_pu = 0;
  UINT32 *pc;

  if (pad_bundles_current_pu != ST_pu(Get_Current_PU_ST())) {
    cold_addr = 0;
    hot_addr = 0;
    pad_bundles_current_pu = ST_pu(Get_Current_PU_ST());
  }
  if (BB_Is_Cold(bb)) {
    pc = &cold_addr;
  } else {
    pc = &hot_addr;
  }

  OP *bundle_start = BB_first_op(bb);
  INT free_slots_in_previous_bundle = 0;
  while (bundle_start) {
    bundle_start = Pad_Bundle (pc, bundle_start, &free_slots_in_previous_bundle,
			       scycle_delta);
  }

}

/* ====================================================================
 *   CGTARG_Make_Bundles_Postpass
 * ====================================================================
 */
void CGTARG_Make_Bundles_Postpass(BB *bb)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  if (FORCE_NOOPS && CG_NOPs_to_GOTO) {
    NOPs_to_GOTO(bb);
  }
  INT scycle_delta = 0;
  Pad_Bundles_With_NOPs (bb, &scycle_delta);
#endif
}  

/* ====================================================================
 *   CGTARG_Fixup_Immediates
 * ====================================================================
 */
void CGTARG_Fixup_Immediates ()
{
  FmtAssert(FALSE,("Not implemented"));
#if 0

  /* Transform load/store opcodes addressing modes in order to match
     the offset sign for the offset operand. */
  for (BB *bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    for (OP *op = BB_first_op(bb); op != NULL; op = OP_next(op)) {
      TOP etop;
      INT old_size = OP_inst_words(op), new_size = 1;
      if ((OP_load(op) || OP_store(op)) &&
	  // FdF 20060518: Support for automod addressing mode
	  !OP_automod(op)) {
	TOP top = OP_code(op);
	INT ofst_idx = OP_find_opnd_use(op, OU_offset);
	DevAssert(ofst_idx >= 0, ("missing OU_offset in load/store, top: %s", TOP_Name(top)));
	const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(top);
	const ISA_OPERAND_VALTYP *vtype= ISA_OPERAND_INFO_Operand(oinfo, ofst_idx);
	if (ISA_OPERAND_VALTYP_Is_Literal(vtype)) {
	  ISA_LIT_CLASS lc = ISA_OPERAND_VALTYP_Literal_Class(vtype);
	  TN *ofst_tn = OP_opnd(op, ofst_idx);
	  DevAssert(TN_is_constant(ofst_tn), ("non constant operand %d in top: %s", ofst_idx,TOP_Name(top)));
	  if (TN_has_value(ofst_tn)) {
	    INT64 ofst_val = TN_value(ofst_tn), new_ofst_val;
	    TOP new_top = TOP_UNDEFINED;
	    /* First, if the offset is negative we must use the negoffset addressing mode variant if
	       it exists. */
	    if (ofst_val < 0) {
	      new_top = TOP_get_variant(top, VARATT_addr_negoffset);
	      if (new_top != TOP_UNDEFINED) {
		new_ofst_val = -ofst_val;
		const ISA_OPERAND_INFO *new_oinfo = ISA_OPERAND_Info(new_top);
		const ISA_OPERAND_VALTYP *new_vtype= ISA_OPERAND_INFO_Operand(new_oinfo, ofst_idx);
		DevAssert(ISA_OPERAND_VALTYP_Is_Literal(new_vtype), ("Unexpected non literal in negoffset variant"));
		ISA_LIT_CLASS lc = ISA_OPERAND_VALTYP_Literal_Class(new_vtype);
		DevAssert(ISA_LC_Value_In_Class(new_ofst_val, lc), ("Unexpected range for neoffset variant"));
	      }
	    }
	    /* Replace the opcode and offset value. */
	    if (new_top != TOP_UNDEFINED) {
#if 0
	      fprintf(TFile, "%s - Operation:", __FUNCTION__);
	      Print_OP_No_SrcLine(op);
	      fprintf(TFile, "%s - Changed to:", __FUNCTION__);
#endif
	      
	      OP_Change_Opcode(op, new_top);
	      Set_OP_opnd(op, ofst_idx, Gen_Literal_TN(new_ofst_val, TN_size(ofst_tn)));
#if 0
	      Print_OP_No_SrcLine(op);
#endif
	    } 
	  } else if (TN_is_symbol(ofst_tn) && TN_relocs(ofst_tn) == TN_RELOC_NONE) {
	    ST *base_st, *st;
	    INT64 ofst_val, new_ofst_val;
	    TOP new_top = TOP_UNDEFINED;
	    st = TN_var(ofst_tn);
	    Base_Symbol_And_Offset (st, &base_st, &ofst_val);
	    ofst_val += TN_offset(ofst_tn);

	    if (base_st == FP_Sym) {
	      /* First, if the offset is negative we must use the negoffset addressing mode variant if
		 it exists. */
	      if (ofst_val < 0) {
		new_top = TOP_get_variant(top, VARATT_addr_negoffset);
		if (new_top != TOP_UNDEFINED) {
		  new_ofst_val = -ofst_val;
		  const ISA_OPERAND_INFO *new_oinfo = ISA_OPERAND_Info(new_top);
		  const ISA_OPERAND_VALTYP *new_vtype= ISA_OPERAND_INFO_Operand(new_oinfo, ofst_idx);
		  DevAssert(ISA_OPERAND_VALTYP_Is_Literal(new_vtype), ("Unexpected non literal in negoffset variant"));
		  ISA_LIT_CLASS lc = ISA_OPERAND_VALTYP_Literal_Class(new_vtype);
		  DevAssert(ISA_LC_Value_In_Class(new_ofst_val, lc), ("Unexpected range for negoffset variant"));
		}
	      }
	      /* Replace the opcode and offset value. */
	      if (new_top != TOP_UNDEFINED) {
#if 0
		fprintf(TFile, "%s - Operation:", __FUNCTION__);
		Print_OP_No_SrcLine(op);
		fprintf(TFile, "%s - Changed to:", __FUNCTION__);
#endif
		OP_Change_Opcode(op, new_top);
		Set_OP_opnd(op, ofst_idx, 
			    Gen_Symbol_TN(st, TN_offset(ofst_tn) + new_ofst_val - ofst_val,
					  TN_RELOC_NONE));
		
#if 0
		Print_OP_No_SrcLine(op);
#endif
	      } 
	    } else if (base_st == SP_Sym) {
	      /* Nothing to do as SP is always accessed by positive offsets. */
	      DevAssert(ofst_val >= 0, ("Negative offset for SP access"));
	    } else {
	      DevAssert(0, ("Non SP/FP relative operand offset for operand  %d for top %s\n", ofst_idx, TOP_Name(top)));
	    }
	  } else {
	    DevWarn("Non valued operand %d for top %s\n", ofst_idx, TOP_Name(top));
	  }
	}
      }
    }
  }
#endif
}

/* ====================================================================
 *   CGTARG_Pseudo_Make_Expand
 *
 *   Generate sequence that correspond to pseudo MAKE.
 * ====================================================================
 */
BOOL
CGTARG_Pseudo_Make_Expand( ) {

  OP *op;
  OP *op_save_next;
  OP *op_save;
  BB *bb;
  BOOL modified=FALSE;

  for (BB *bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    FOR_ALL_BB_OPs (bb, op) {
      if(OP_code(op) == TOP_MAKE) {
	TN *src = OP_opnd(op,0);
	TN *res = OP_result(op,0);
	INT64 val = 0;
	OPS ops = OPS_EMPTY;
        modified=TRUE;

	if(TN_is_symbol(src) && !ST_on_stack(TN_var(src))) {
	  TN *pc = Build_Dedicated_TN(ISA_REGISTER_CLASS_integer,16,0);
	  TN *src1 = Dup_TN(src);
	  Set_TN_is_reloc_offset12PC(src1);

	  Build_OP(TOP_ldr_i12_npc, res, pc, src1, &ops);
	  OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
	  op_save_next = OP_next(op);
	  op_save = op;
	  BB_Insert_Ops_Before(OP_bb(op),op,&ops);
	  FmtAssert(op == op_save && op_save_next == OP_next(op),("Changes ..."));
	  OP_Change_To_Noop(op);
	}
	else {
	  if(TN_has_value(src)) {
	    val = TN_value(src);
	  }
	  else if(TN_is_symbol(src) && ST_on_stack(TN_var(src))) {
	    ST *base;
	    Base_Symbol_And_Offset_For_Addressing (TN_var(src),
						   TN_offset(src), &base, &val);
	  }
	  else {
	    FmtAssert(FALSE,("How to manage this ..."));
	  }
      
	  if(ISA_LC_Value_In_Class (val, LC_immed8)) {
	    Build_OP(TOP_mov_i8_npc, res, src, &ops);
	    OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
	  }
	  else if(ISA_LC_Value_In_Class (~val, LC_immed8)) {
	    Build_OP(TOP_mvn_i8_npc, res, Gen_Literal_TN(~val, 4), &ops);
	    OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
	  }
	  else {
	    Build_OP(TOP_mov_i8_npc, res, Gen_Literal_TN(val&0xff, 4), &ops);
	    OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
	    if( val & 0xff00 ) {
	      Build_OP(TOP_add_i8_r_npc, res, res, Gen_Literal_TN(val&0xff00, 4), &ops);
	      OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
	    }
	    if( val & 0xff0000 ) {
	      Build_OP(TOP_add_i8_r_npc, res, res, Gen_Literal_TN(val&0xff0000, 4), &ops);
	      OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
	    }
	    if( val & 0xff000000 ) {
	      Build_OP(TOP_add_i8_r_npc, res,  res, Gen_Literal_TN(val&0xff000000, 4), &ops);
	      OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
	    }
	  }

	  op_save_next = OP_next(op);
	  op_save = op;
	  BB_Insert_Ops_Before(OP_bb(op),op,&ops);
	  FmtAssert(op == op_save && op_save_next == OP_next(op),("Changes ..."));
	  OP_Change_To_Noop(op);
	}
      }
    }
  }
  return modified;
}

/*
 *******************************************************************
 * Auxiliary functions for hwloop fixing that are target specific
 *******************************************************************
*/

/** 
 * returns numbers of operations required between op and the
 * end of the hwloop.
 * 
 * @bug Note that n>1 is not yet implemented.
 *
 * @param op 
 * 
 * @return 
 */
static int
nbOpBeforeEndOfHwloop(OP* op)
{
  FmtAssert(FALSE,("Not implemented"));
#if 0

  TOP t = OP_code(op);
  switch (t) {
  case TOP_trap:
  case TOP_jr:
  case TOP_ja:
  case TOP_jrf:
  case TOP_jaf:
  case TOP_jagtudec:
  case TOP_jrgtudec:
  case TOP_calla:
  case TOP_callr:
  case TOP_rts:
  case TOP_rte:
  case TOP_pushsh:
  case TOP_pushrl:
  case TOP_pushrh:
  case TOP_popsh:
  case TOP_poprl:
  case TOP_poprh:
  case TOP_barrier:
  case TOP_lsetub_i8_dec:
  case TOP_lsetub_r_inc:
  case TOP_lsetub_i12_inc:
  case TOP_lsetub_i8_inc:
  case TOP_lsetub_i5_pre_dec:
  case TOP_lsetub_i5_post_dec:
  case TOP_lsetub_r_post_inc:
  case TOP_lsetub_i5_post_inc:
    return 1;
  default:
    return 0;
  }
#endif
}

/**
 * max number of op between an operation ant hwloop end
 */
#define MAXNOPINHWLOOP 1

/** 
 * minimal space between 2 hwloop end.
 */
#define MINIMAL_SPACE_BETWEEN_HWLOOP_END 1 

/**
 * number of nops to add at maximum before exit instruction.
 */
#define MAXNOPINEPILOGUE 1

/** 
 * return minimal number of operations between op and return instruction.
 * 
 * @param op 
 * 
 * @return 
 */
static int
nbOpBeforeEnfOfFunc(OP* op)
{
#if 0
  TOP t = OP_code(op);
  switch(t)
    {
    case TOP_popsh:
      return 1;
    default:
      return 0;
    }
#else
  return 0;
#endif
}

#define IS_RETURN_OP(op) (OP_code(op) == TOP_rts || OP_code(op) == TOP_rte)


/** 
 * This auxiliary function checks that tail basic BB dos not end with a
 * forbidden isntruction and by a branch to the end.
 * 
 *  Rule 8 and 10 in 1.1.1 paragraph of Bilbocc Core Reference Guide 1.0
 *
 * @param bb 
 * 
 * @return 
 */
static int
ChecklastHwloopInstruction(BB* bb)
{
  FmtAssert(FALSE,("Not implemented"));
#if 0

  FmtAssert((MAXNOPINHWLOOP==1), ("MAXNOPINHWLOOP larger than 1 not yet implemented"));

  // handle empty bb case
  //  only top_hwbranch instruction is equivalent to empty
  while (BB_length(bb) == 0  ||
         (BB_length(bb) == 1 &&
          OP_Is_Counted_Loop(BB_last_op(bb))))
    {
      // if multiple preds we have to add a nop anyway !
      if (BB_preds_len(bb) > 1) 
        {
          return MAXNOPINHWLOOP;
        }
      bb= BB_Unique_Predecessor(bb);
    }

  OP* lastop = BB_last_op(bb);

  FmtAssert((lastop!=NULL),("lastop cannot be NULL!!!"));
  
  /* check hwbranch case */
  if (OP_Is_Counted_Loop(lastop))
    {
      lastop = OP_prev(lastop);
    }

  return nbOpBeforeEndOfHwloop(lastop);
#endif
}

/** 
 * fix invalid sequence popsh, rts(or rte) by inserting a nop.
 *
 *  Rule 3 in 1.1.1 paragraph of Bilbocc Core Reference Guide 1.0
 */
static void 
fixFuncEpilogueforHwloop()
{
  FmtAssert(FALSE,("Not implemented"));
#if 0

  FmtAssert((MAXNOPINEPILOGUE==1), ("MAXNOPINEPILOGUE larger than 1 not yet implemented"));
  
  BB_LIST *elist;  
  for (elist = Exit_BB_Head; elist; elist = BB_LIST_rest(elist)) {
    BB* bb = BB_LIST_first(elist);
    FmtAssert((bb!=NULL && BB_length(bb)!=0), ("Exit BB cannot be empty."));
    OP* lastop =  BB_last_op(bb);
    // popsh is in exit BB by construction !!
    if ((IS_RETURN_OP(lastop)) &&
        (BB_length(bb)>1 &&  nbOpBeforeEnfOfFunc(OP_prev(lastop)) >0 ))
      {
        // add nop between popsh and rts/rte
        OPS ops = OPS_EMPTY;
        int i;
        for (i=0 ; i<nbOpBeforeEnfOfFunc(OP_prev(lastop)); i++)
          Build_OP(TOP_nop, &ops);
        BB_Insert_Ops_Before(bb,  lastop, &ops);
      }
  }
#endif
}


/** 
 * Rule 7 in 1.1.1 paragraph of Bilbocc Core Reference Guide 1.0
 * 
 * @param tail 
 * 
 * @return 
 */
static void
fixHwloopTail(BB* tail)
{
  FmtAssert(FALSE,("Not implemented"));
#if 0
  BBLIST *succlist = BBlist_Fall_Thru_Succ(tail);
  BB *succbb;
  FmtAssert((MINIMAL_SPACE_BETWEEN_HWLOOP_END<=1), ("MINIMAL_SPACE_BETWEEN_HWLOOP_END larger than 1 not yet implemented"));

  FmtAssert(((succlist!=NULL) && (BBLIST_item(succlist)!=NULL)),
            ("an hwloop must have a BB fall trough successor"));
  
  // search first non empty successor BB for loop tail
  succbb= BBLIST_item(succlist);
  while (BB_length(succbb) == 0)
    {
      succbb= BB_next(succbb);
      FmtAssert((succbb!=NULL),("succbb cannot be NULL!!!"));
    }
  
  // check wether first non empty successor BB is a single operation
  // tail BB of embedding hwloop.
  if (BB_length(succbb) == 1 && OP_Is_Counted_Loop(BB_last_op(succbb)))
    {
      OPS ops = OPS_EMPTY;
      int i;
      for (i=0; i<MINIMAL_SPACE_BETWEEN_HWLOOP_END; i++)
        Build_OP(TOP_nop, &ops);
      BB_Prepend_Ops(succbb, &ops);
    }
#endif
 }

/** 
 * function checks that hwloops obey HW constraints and
 * possibly add a nop in tail BB to fix theses constraints.
 *
 * 3 constraints are checked:
 * * certain operations are forbidden as last op for an hwloop.
 * * 2 hwloops may not share their last operation.
 * * popsh is forbidden as last instruction before rts/rte.
 * 
 */
void
Perform_HwLoop_Checking()
{
  FmtAssert(FALSE,("Not implemented"));
#if 0
  if ((Activate_Hwloop&1) == 0)
    return;

  MEM_POOL loop_descr_pool;
  MEM_POOL_Initialize(&loop_descr_pool, "loop_descriptors", TRUE);
  MEM_POOL_Push (&loop_descr_pool);
  Calculate_Dominators();		/* needed for loop recognition */
  
  for (LOOP_DESCR *loop = LOOP_DESCR_Detect_Loops(&loop_descr_pool);
       loop;
       loop = LOOP_DESCR_next(loop))
    {
      BB *tail = LOOP_DESCR_Find_Unique_Tail(loop);

      // DETECTS HWLOOP
      if (tail!=NULL && BB_HWLoop_tail(tail))
        {

	  // FdF 20060811: Add checks on correctly formed HWLoops
	  // Check that head and tail belong to the same HWLoop
	  FmtAssert(BB_HWLoop_head(LOOP_DESCR_loophead(loop)),
		    ("HwLoop_Check: Incorrect HWLoop at loop head BB:%d\n", BB_id(LOOP_DESCR_loophead(loop))));
	  // Check that fall-thru successor of tail has a label
	  BB *next_bb = BB_Fall_Thru_Successor(tail);
	  FmtAssert(next_bb && BB_Has_Addr_Taken_Label(next_bb),
		    ("HwLoop_Check: Incorrect fall-thru BB for loop tail BB:%d\n", BB_id(tail)));
	  // Check that nesting is correct
	  FmtAssert(Check_HWLoop_nest(loop),
		    ("HwLoop_Check: Incorrect HWLoop nesting at loop head BB:%d\n", BB_id(LOOP_DESCR_loophead(loop))));

	  // The loop is correct, but may require some fixing
	    
          int  addnop = 0;

          fixHwloopTail(tail); /* Rule 7 */

          /* Now do some checking on tail BB. It must not end with a
             few selected instructions ... */
          /* Rule 8 */
          addnop = ChecklastHwloopInstruction(tail); 

          if (addnop>0)
            {
              OPS ops = OPS_EMPTY;
              int i;
              for (i=0; i<addnop; i++)
                Build_OP(TOP_nop, &ops);
              BB_Insert_Ops_Before(tail,  BB_last_op(tail), &ops);
            }
        }
    }

  MEM_POOL_Pop (&loop_descr_pool);
  MEM_POOL_Delete(&loop_descr_pool);
  
  Free_Dominators_Memory (); 

  fixFuncEpilogueforHwloop(); /* Rule 3 */
#endif
}

/* ====================================================================
 *   CGTARG_Resize_Instructions
 * ====================================================================
 */
void CGTARG_Resize_Instructions ()
{

}

/* ====================================================================
 *   CGTARG_Handle_Errata_Hazard
 * ====================================================================
 */
void
CGTARG_Handle_Errata_Hazard (OP *op, INT erratnum, INT ops_to_check)
{
}

/* ====================================================================
 *   CGTARG_Insert_Stop_Bits
 * ====================================================================
 */
void
CGTARG_Insert_Stop_Bits(BB *bb)
{
  return;
}

#if 0
/* ====================================================================
 *   CGTARG_Special_Min_II
 *
 *   Check for target specific (tail stepping, and other?) special
 *	  cases that might force a higher Min II. If a case applies, the
 *	  target specific MII is returned, otherwise 0 is returned.
 *
 * ====================================================================
 */
INT32 CGTARG_Special_Min_II(BB* loop_body, BOOL trace)
{
  return 0;
}
#endif

/* ====================================================================
 *   CGTARG_Perform_THR_Code_Generation (OP load_op, THR_TYPE type)
 *
 *   Perform THR (and target-specific) code generation tasks per
 *   THR_TYPE and OP. Depending on <type>, <op> can now be 
 *   substituted (or replaced with) new ops.
 * ====================================================================
 */
void 
CGTARG_Perform_THR_Code_Generation (
  OP *load_op, 
  THR_TYPE type
)
{
  if (type & THR_DATA_SPECULATION_NO_RB) {
    FmtAssert(OP_load(load_op), ("CGTARG_Perform_THR_Code_Generation : not a load OP"));
    INT enum_pos = -1;
#if 0
    BOOL float_op = OP_Is_Float_Mem(load_op) ? TRUE : FALSE;
    CGTARG_Return_Enum_TN(load_op, (float_op) ? ECV_fldtype : ECV_ldtype, &enum_pos);

    FmtAssert(enum_pos != -1, ("OP doesn't have enum operand"));
    
    TN *enum1_tn = (float_op) ?  Gen_Enum_TN(ECV_fldtype_a) :
                                 Gen_Enum_TN(ECV_ldtype_a);
    Set_OP_opnd(load_op, enum_pos, enum1_tn);
    Set_OP_speculative(load_op);               // set the OP_speculative flag.
    
    OP *check_load = Dup_OP(load_op);
    TN *enum2_tn;
    enum2_tn = (float_op) ? Gen_Enum_TN(ECV_fldtype_c_nc) : 
                            Gen_Enum_TN(ECV_ldtype_c_nc);
    Set_OP_opnd(check_load, enum_pos, enum2_tn); // set the enum TN, need to 
                                                 // add generic enum accessor.
    Set_OP_cond_def_kind(check_load, OP_ALWAYS_COND_DEF); 

    BB_Insert_Op_After(OP_bb(load_op), load_op, check_load);
#endif
  } else if (type & THR_DATA_SPECULATION_NO_RB_CLEANUP) {
#if 0
    INT check_base_idx = TOP_Find_Operand_Use(OP_code(load_op), OU_base);
    TN *check_base_tn = OP_opnd(load_op, check_base_idx);
    TN *adv_load_base_tn = OP_opnd(chk_load, check_base_idx);
    
    TN *check_result_tn = OP_result(load_op, 0);
    TN *adv_load_result_tn = OP_result(chk_load, 0);

    if (TNs_Are_Equivalent(check_result_tn, adv_load_result_tn) &&
	TNs_Are_Equivalent(check_base_tn, adv_load_base_tn)) {
      BB_Remove_Op(OP_bb(load_op), load_op);
      
      INT enum_pos = -1;
      BOOL float_op = OP_Is_Float_Mem(chk_load) ? TRUE : FALSE;
      TN *enum_tn = 
	CGTARG_Return_Enum_TN(chk_load,  
			      (float_op) ? ECV_fldtype_c_nc : ECV_ldtype_c_nc, 
			      &enum_pos); 

      FmtAssert(enum_tn, ("OP doesn't have enum operand"));
      Set_TN_enum(enum_tn, (float_op) ? ECV_fldtype : ECV_ldtype);   
    }
#endif
  }
}



/**********************************************************************
 *
 *             CGTARG_Interference implementation:
 *
 **********************************************************************
 */

static MEM_POOL interference_pool;
static VOID_LIST** writing;     /* writing[i] is a list of live ranges being
                                   written into registers in cycle i */
static BOOL is_loop;            /* Are we working on a loop? */
static INT32 assumed_longest_latency = 40;
                                /* We need to allocate <writing> to be somewhat
                                   longer than the number of cycles in the
                                   schedule in order to accommodate writes
                                   initiated near the end of the schedule.
                                   We'll check and grow this number as
                                   necessary. */
static INT32 cycle_count;       /* Number of cycles in the schedule under
                                   consideration. */
static void (*make_interference)(void*,void*);
                                /* Client's interference call back. */

/* ====================================================================
 *   Increase_Assumed_Longest_Latency
 *
 *   We need to increase our assumptions about the longest latency 
 *   operation in our target.  Also reallocate <writing>.
 *
 * ====================================================================
 */
static void
Increase_Assumed_Longest_Latency(INT32 new_longest_latency )
{
  DevWarn("Assumed longest latency should be at least %d",
          new_longest_latency);
  writing = TYPE_MEM_POOL_REALLOC_N(VOID_LIST*,&interference_pool,writing,
                                    cycle_count + assumed_longest_latency,
                                    cycle_count + new_longest_latency);
  assumed_longest_latency = new_longest_latency;
}

/* ====================================================================
 *   CGTARG_Interference_Required
 * ====================================================================
 */
BOOL CGTARG_Interference_Required(void)
{
  return FALSE;
}

/* ====================================================================
 *   CGTARG_Interference_Initialize
 * ====================================================================
 */
void
CGTARG_Interference_Initialize ( 
  INT32 cycle_count_local, 
  BOOL is_loop_local,
  void (*make_interference_local)(void*,void*) 
)
{
  static BOOL interference_pool_initialized = FALSE;

  if ( ! interference_pool_initialized ) {
    MEM_POOL_Initialize(&interference_pool,"Target_specific_interference",
                        TRUE);
    interference_pool_initialized = TRUE;
  }

  MEM_POOL_Push(&interference_pool);

  is_loop = is_loop_local;
  make_interference = make_interference_local;
  cycle_count = cycle_count_local;

  if ( is_loop )
    writing = TYPE_MEM_POOL_ALLOC_N(VOID_LIST*,&interference_pool,cycle_count);
  else
    writing = TYPE_MEM_POOL_ALLOC_N(VOID_LIST*,&interference_pool,
                                    cycle_count + assumed_longest_latency);
}

/* ====================================================================
 *   CGTARG_Result_Live_Range
 * ====================================================================
 */
void
CGTARG_Result_Live_Range ( 
  void* lrange, 
  OP* op, 
  INT32 offset 
)
{
  VOID_LIST* l;
  INT32 cycle = OP_scycle(op);

  FmtAssert(FALSE, ("CGTARG_Result_Live_Range: not implemented"));

  return;
}

/* ====================================================================
 *   CGTARG_Operand_Live_Range
 * ====================================================================
 */
void
CGTARG_Operand_Live_Range ( 
  void* lrange, 
  INT   opnd, 
  OP*   op, 
  INT32 offset 
)
{
  VOID_LIST* l;
  INT32 cycle = OP_scycle(op);
  INT32 reg_acc_cyc = TI_LATENCY_Operand_Access_Cycle(OP_code(op), opnd);

  if ( is_loop )
    reg_acc_cyc = Mod(reg_acc_cyc + cycle + offset,cycle_count);
  else {
    reg_acc_cyc += cycle + offset;

    if ( reg_acc_cyc >= cycle_count + assumed_longest_latency )
      return;   /* Nothing writing in this cycle anyway */
  }

  for ( l = writing[reg_acc_cyc]; l != NULL; l = VOID_LIST_rest(l) )
    make_interference(lrange,VOID_LIST_first(l));
}

/* ====================================================================
 *   CGTARG_Interference_Finalize
 * ====================================================================
 */
void
CGTARG_Interference_Finalize(void)
{
  MEM_POOL_Pop(&interference_pool);
  writing = (VOID_LIST **) -1;
}

/**********************************************************************
 *
 *                 Peak Rate Class (PRC):
 *
 **********************************************************************
 */

/* ====================================================================
 *
 * Reduce_Fraction
 *
 * Half hearted attempt to reduce a fraction. If we don't succeed
 * the only problem will be that we might round incorrectly on a
 * instruction rate.
 *
 * The algorithm is to first try the denominator as a factor and
 * then a few small primes.
 *
 * ====================================================================
 */
static void
Reduce_Fraction(INT frac[2])
{
  INT i;
  static const INT primes[] = {2, 3, 5, 7, 11, 13};
  INT n = frac[0];
  INT d = frac[1];
  INT p = d;

  if (d < -1 || d > 1) {
    for (i = sizeof(primes) / sizeof(primes[0]); ; p = primes[--i]) {
      while (n % p == 0 && d % p == 0) {
	n = n / p;
	d = d / p;
      }
      if (i == 0) break;
    }
  }

  frac[0] = n;
  frac[1] = d;
}

/* ====================================================================
 *
 * Harmonic_Mean
 *
 * Compute the harmonic weighted mean of two rates as follows:
 *
 *	  1        a                    b
 *	---- = ( ----- * a_rate ) + ( ----- * b_rate )
 *	mean     a + b                a + b
 *
 * Where:
 *
 *	"a" is the number of operations of class "a"
 *	"b" is the number of operations of class "b"
 *
 * ====================================================================
 */
static void
Harmonic_Mean(
  INT mean[2],
  INT a,
  const INT a_rate[2],
  INT b,
  const INT b_rate[2]
) {
  if (a == 0) {
    mean[0] = b_rate[0];
    mean[1] = b_rate[1];
  } else if (b == 0) {
    mean[0] = a_rate[0];
    mean[1] = a_rate[1];
  } else {
    mean[1] =   (a * a_rate[1] * b_rate[0]) 
	      + (b * b_rate[1] * a_rate[0]);
    mean[0] = (a + b) * a_rate[0] * b_rate[0];
    Reduce_Fraction(mean);
  }
}

/* ====================================================================
 *   CGTARG_Peak_Rate
 * ====================================================================
 */
void CGTARG_Peak_Rate ( 
  PEAK_RATE_CLASS prc, 
  PRC_INFO *info, 
  INT ratio[2] 
)
{
  ratio[0] = 1;
  ratio[1] = 1;
  
  switch (prc) {
  case PRC_INST:
    ratio[0] = 4;
    break;
  case PRC_MADD:
    ratio[0] = 2;
    break;
  case PRC_MEMREF:
    ratio[0] = 1;
    break;
  case PRC_FLOP:
  case PRC_FADD:
  case PRC_FMUL:
    ratio[0] = 4;
    break;
  case PRC_IOP:
    ratio[0] = 4;
    break;
  default:
    ratio[0] = 4;
    break;
  }
}

/* =======================================================================
 *   Plural
 *
 *   Return "s" if i != 1, "" otherwise.  Used to get the number of nouns
 *   right when printing.
 * =======================================================================
 */
#define Plural(i) ((i) != 1 ? "s" : "")

/* =======================================================================
 *   Percent_Of_Peak
 *
 *   Compute the percentage of peak instructions executed. Both the
 *   actual number of instructions executed and the peak attainable
 *   are expressed as a fraction of insts/cycle.
 * =======================================================================
 */
static INT
Percent_Of_Peak(INT numer, INT denom, INT peak[2])
{
  if (numer == 0) return 0;
  return (numer * peak[1] * 100) / ((denom * peak[0]) + peak[1] - 1);
}

/* =======================================================================
 *   CGTARG_Print_PRC_INFO
 *
 *   Print statistics for the PRC_INFO to a 'file'.
 * =======================================================================
 */
void
CGTARG_Print_PRC_INFO(
  FILE       *file,
  PRC_INFO   *info,
  INT32      ii,
  const char *prefix,
  const char *suffix
)
{
  char *s;
  INT madds_per_cycle[2];
  INT memrefs_per_cycle[2];
  INT flops_per_cycle[2];
  INT fadds_per_cycle[2];
  INT fmuls_per_cycle[2];
  INT iops_per_cycle[2];
  INT insts_per_cycle[2];
  INT insts = info->refs[PRC_INST];
  INT memrefs = info->refs[PRC_MEMREF];
  INT flops = info->refs[PRC_FLOP];
  INT madds = info->refs[PRC_MADD];
  INT fadds = info->refs[PRC_FADD];
  INT fmuls = info->refs[PRC_FMUL];
  INT iops = info->refs[PRC_IOP];

  CGTARG_Peak_Rate(PRC_INST, info, insts_per_cycle);
  CGTARG_Peak_Rate(PRC_MEMREF, info, memrefs_per_cycle);
  CGTARG_Peak_Rate(PRC_FLOP, info, flops_per_cycle);
  CGTARG_Peak_Rate(PRC_MADD, info, madds_per_cycle);
  CGTARG_Peak_Rate(PRC_FADD, info, fadds_per_cycle);
  CGTARG_Peak_Rate(PRC_FMUL, info, fmuls_per_cycle);
  CGTARG_Peak_Rate(PRC_IOP, info, iops_per_cycle);

  if (flops != 0) {
    BOOL unbalanced_fpu = FALSE;

    if ( madds_per_cycle[0] != 0 ) {
      fprintf(file,"%s%5d flop%1s        (%3d%% of peak) (madds count as 2)%s"
                   "%s%5d flop%1s        (%3d%% of peak) (madds count as 1)%s"
                   "%s%5d madd%1s        (%3d%% of peak)%s",
		 prefix,
		 flops + madds,
		 Plural(flops + madds),
		 Percent_Of_Peak(flops + madds, ii * 2, madds_per_cycle),
		 suffix,
		 prefix,
		 flops,
		 Plural(flops),
		 Percent_Of_Peak(flops, ii, flops_per_cycle),
		 suffix,
		 prefix,
		 madds,
		 Plural(madds),
		 Percent_Of_Peak(madds, ii, madds_per_cycle),
		 suffix);
    }
    else {
      fprintf(file,"%s%5d flop%1s        (%3d%% of peak)%s",
		 prefix,
		 flops,
		 Plural(flops),
		 Percent_Of_Peak(flops, ii, flops_per_cycle),
		 suffix);
    }

    if ( unbalanced_fpu ) {
      INT fmuls2_per_cycle[2]; /* combined fmul/madd peak rate */
      INT fadds2_per_cycle[2]; /* combined fadd/madd peak rate */
      INT fadds2 = fadds + madds;
      INT fmuls2 = fmuls + madds;

      Harmonic_Mean(fmuls2_per_cycle,
		    fmuls, fmuls_per_cycle,
		    madds, madds_per_cycle);
      Harmonic_Mean(fadds2_per_cycle,
		    fadds, fadds_per_cycle,
		    madds, madds_per_cycle);

      fprintf(file,"%s%5d fmul%1s        (%3d%% of peak)%s%s",
		 prefix,
		 fmuls2,
		 Plural(fmuls2),
		 Percent_Of_Peak(fmuls2, ii, fmuls2_per_cycle),
		 madds_per_cycle[0] ? " (madds count as 1)" : "",
		 suffix);
      fprintf(file,"%s%5d fadd%1s        (%3d%% of peak)%s%s",
		 prefix,
		 fadds2,
		 Plural(fadds2),
		 Percent_Of_Peak(fadds2, ii, fadds2_per_cycle),
		 madds_per_cycle[0] ? " (madds count as 1)" : "",
		 suffix);
    }
  }

  s = "";
  if (FALSE) {
    iops += memrefs;
    s = " (mem refs included)";
  }

  fprintf(file,"%s%5d mem ref%1s     (%3d%% of peak)%s"
               "%s%5d integer op%1s  (%3d%% of peak)%s%s"
               "%s%5d instruction%1s (%3d%% of peak)%s",
               prefix,
               memrefs,
               Plural(memrefs),
               Percent_Of_Peak(memrefs, ii, memrefs_per_cycle),
	       suffix,
               prefix,
               iops,
               Plural(iops),
               Percent_Of_Peak(iops, ii, iops_per_cycle),
	       s,
	       suffix,
               prefix,
               insts,
               Plural(insts),
               Percent_Of_Peak(insts, ii, insts_per_cycle),
	       suffix);
}



/* =======================================================================
 *   CGTARG_Compute_PRC_INFO
 *
 *   Compute some basic information about the given 'bb'. 
 * =======================================================================
 */
void
CGTARG_Compute_PRC_INFO(
  BB *bb,
  PRC_INFO *info
)
{
  OP *op;

  BZERO (info, sizeof (PRC_INFO));

  for ( op = BB_first_op(bb); op != NULL; op = OP_next(op) ) {
    INT num_insts = OP_Real_Ops (op);

    if (num_insts == 0) continue;

    // FdF: Because for LAO we add the annotation after the bundling.
    if (OP_noop(op)) continue;      

    info->refs[PRC_INST] += num_insts;

    if ( OP_flop(op) ) {
      BOOL is_single = (OP_result_size(op,0) == 32);

      ++info->refs[PRC_FLOP];
      info->refs[PRC_FLOP_S] += is_single;
      if (OP_madd(op)) {
        ++info->refs[PRC_MADD];
	info->refs[PRC_MADD_S] += is_single;
      }
      else if (OP_fadd(op) || OP_fsub(op)) {
	++info->refs[PRC_FADD];
	info->refs[PRC_FADD_S] += is_single;
      }
      else if (OP_fmul(op)) {
	++info->refs[PRC_FMUL];
	info->refs[PRC_FMUL_S] += is_single;
      }
    }
    else if (OP_memory(op))
      ++info->refs[PRC_MEMREF];
    else {
      INT k;

      /* Conditional moves and m[tf]c1 are not tagged as flops.
       * We certainly don't want to call them integer ops, so assume
       * anything that uses FP regs isn't an integer instruction.
       */
      if (OP_has_result(op) && TN_is_float(OP_result(op,0))) goto not_iop;

      for (k = 0; k < OP_opnds(op); k++) {
	if (TN_is_float(OP_opnd(op,k))) goto not_iop;
      }

      info->refs[PRC_IOP] += num_insts;

    not_iop:
      ;
    }
  }
}

/**********************************************************************
 *
 *                  Load immediate information
 *
 **********************************************************************
 */
BOOL CGTARG_is_expensive_load_imm(OP* op)
{
    return op && OP_code(op) == TOP_MAKE;
}

INT CGTARG_expensive_load_imm_immediate_index(OP* op)
{
    return CGTARG_is_expensive_load_imm(op)? 1: -1;
}

INT CGTARG_expensive_load_imm_base_index(OP* op)
{
    return -1;
}

BOOL CGTARG_sequence_is_cheaper_than_load_imm(OPS* ops, OP* op)
{
    return CGTARG_is_expensive_load_imm(op) && OPS_length(ops) < 2;
}

BOOL CGTARG_should_factorize(OP* first, OP* last)
{
    return FALSE;
}

/**********************************************************************
 *
 * Init_TOP_Variant_Tables()
 *
 * Initialize some TOP variant tables:
 *  - TOP to/from Multi conversion
 *  - TOP immediate to register conversion
 *
 **********************************************************************
 */
static TOP *TOP_To_Multi_Table = NULL;
static TOP *TOP_From_Multi_Table = NULL;
static TOP *TOP_Register_Variant_Table = NULL;

static void
Init_TOP_Variant_Tables() {
  TOP top, top_multi, top_immediate;
  TOP_To_Multi_Table   = TYPE_MEM_POOL_ALLOC_N(TOP, Malloc_Mem_Pool, (TOP_count + 1));
  TOP_From_Multi_Table = TYPE_MEM_POOL_ALLOC_N(TOP, Malloc_Mem_Pool, (TOP_count + 1));
  TOP_Register_Variant_Table = TYPE_MEM_POOL_ALLOC_N(TOP, Malloc_Mem_Pool, (TOP_count + 1));

  TOP_To_Multi_Table[TOP_UNDEFINED] = TOP_UNDEFINED;
  TOP_From_Multi_Table[TOP_UNDEFINED] = TOP_UNDEFINED;
  TOP_Register_Variant_Table[TOP_UNDEFINED] = TOP_UNDEFINED;
  
  for (top = 0; top < TOP_count; top++) {
	TOP_From_Multi_Table[top] = top;
	TOP_Register_Variant_Table[top] = TOP_UNDEFINED;
  }
  
  for (top = 0; top < TOP_count; top++) {
	top_multi = TOP_get_variant(top, VARATT_multi);
	
	if (top_multi != TOP_UNDEFINED) {
	  TOP_To_Multi_Table[top] = top_multi;
	  TOP_From_Multi_Table[top_multi] = top;
	}
	else {
	  TOP_To_Multi_Table[top] = top;
	}
	
	top_immediate = TOP_get_variant(top, VARATT_immediate);
	// In some description the immediate form reference itself. 
	// We must skip this.
	if (top != top_immediate) {
	  // Iterate over all the immediate forms.
	  while (top_immediate != TOP_UNDEFINED) {
	    DevAssert(TOP_Register_Variant_Table[top_immediate] == TOP_UNDEFINED, ("Unexpected duplicate register variant: immediate form: %s, register_form1: %s, register_form2: %s ", TOP_Name(top_immediate), TOP_Name(TOP_Register_Variant_Table[top_immediate]), TOP_Name(top)));
	    TOP_Register_Variant_Table[top_immediate] = top;
	    top_immediate = TOP_get_variant(top_immediate, VARATT_next_immediate);
	  }
	}
  }
}

/* =======================================================================
 *   CGTARG_TOP_To_Multi
 *
 *   Return the multi version of a given TOP
 * =======================================================================
 */
TOP CGTARG_TOP_To_Multi(TOP top) {
  return (TOP_To_Multi_Table[top]);
}

/* =======================================================================
 *   CGTARG_TOP_From_Multi
 *
 *   Return the standard version of a given multi TOP
 * =======================================================================
 */
TOP CGTARG_TOP_From_Multi(TOP multi_top) {
  return (TOP_From_Multi_Table[multi_top]);
}


/* =======================================================================
 *   CGTARG_TOP_Register_Variant
 *
 *   Return the register variant of the given immediate form top.
 *   This function must be kept private to targinfo.
 *   The user level function is TOP_opnd_register_variant().
 * =======================================================================
 */
TOP CGTARG_TOP_Register_Variant(TOP immform_top) {
  return TOP_Register_Variant_Table[immform_top];
}


/**********************************************************************
 *
 *                  Hardware Workarounds ??
 *
 **********************************************************************
 */
/* ====================================================================
 *   Hardware_Workarounds
 *
 *   Placeholder for all Hardware workarounds. 
 * ====================================================================
 */
void
Hardware_Workarounds(void)
{
}

/* ====================================================================
 * ====================================================================
 *                 Initialization / Termination
 * ====================================================================
 * ====================================================================
 */

/* ====================================================================
 *   CGTARG_Initialize
 * ====================================================================
 */
void 
CGTARG_Initialize ()
{
  INT32 i;

#ifdef TARG_ST
  CGTARG_NUM_SPILL_TYPES = 0;

  // Spill type for integer registers
  CGTARG_Spill_Type [CGTARG_NUM_SPILL_TYPES] = Spill_Int_Type;
  CGTARG_Spill_Mtype[CGTARG_NUM_SPILL_TYPES] = Spill_Int_Mtype;
  CGTARG_NUM_SPILL_TYPES++;

  // Spill type for branch registers
  CGTARG_Spill_Type [CGTARG_NUM_SPILL_TYPES] = MTYPE_To_TY (MTYPE_I1);
  CGTARG_Spill_Mtype[CGTARG_NUM_SPILL_TYPES] = MTYPE_I1;
  CGTARG_NUM_SPILL_TYPES++;

  // Spill type for paired registers
  CGTARG_Spill_Type [CGTARG_NUM_SPILL_TYPES] = MTYPE_To_TY (MTYPE_I8);
  CGTARG_Spill_Mtype[CGTARG_NUM_SPILL_TYPES] = MTYPE_I8;
  CGTARG_NUM_SPILL_TYPES++;

  CGTARG_Spill_Type [CGTARG_NUM_SPILL_TYPES] = Spill_Int_Type;
  CGTARG_Spill_Mtype[CGTARG_NUM_SPILL_TYPES] = Spill_Int_Mtype;
  CGTARG_NUM_SPILL_TYPES++;

  // Spill type for extension registers (reconfigurability)
  for (i = MTYPE_STATIC_LAST+1; i <= MTYPE_LAST; ++i) {
    CGTARG_Spill_Type [CGTARG_NUM_SPILL_TYPES] = MTYPE_To_TY ((TYPE_ID)i);
    CGTARG_Spill_Mtype[CGTARG_NUM_SPILL_TYPES] = (TYPE_ID)i;
    CGTARG_NUM_SPILL_TYPES++;
  }
#endif

  /* TODO: tabulate in the arch data base */

  /* Allocate table
   */
  CGTARG_Invert_Table = TYPE_MEM_POOL_ALLOC_N(TOP, Malloc_Mem_Pool, (TOP_count + 1));

  /* Init all table entries to TOP_UNDEFINED.
   */
  for(i = 0; i <= TOP_count; ++i) {
    CGTARG_Invert_Table[i]        = TOP_UNDEFINED;
  }

  /* Init table for CGTARG_Invert:
   */
#if 0
  CGTARG_Invert_Table[TOP_jrf]         = TOP_jr;
  CGTARG_Invert_Table[TOP_jr]          = TOP_jrf;
  CGTARG_Invert_Table[TOP_jaf]         = TOP_ja;
  CGTARG_Invert_Table[TOP_ja]          = TOP_jaf;
  CGTARG_Invert_Table[TOP_cmpeq_r]     = TOP_cmpne_r;
  CGTARG_Invert_Table[TOP_cmpeq_i8]    = TOP_cmpne_i8;
  CGTARG_Invert_Table[TOP_cmpge_r]     = TOP_cmplt_r;
  CGTARG_Invert_Table[TOP_cmpge_i8]    = TOP_cmplt_i8;
  CGTARG_Invert_Table[TOP_cmpgeu_r]  = TOP_cmpltu_r;
  CGTARG_Invert_Table[TOP_cmpgeu_i8]  = TOP_cmpltu_i8;
  CGTARG_Invert_Table[TOP_cmpgt_r]   = TOP_cmple_r;
  CGTARG_Invert_Table[TOP_cmpgt_i8]   = TOP_cmple_i8;
  CGTARG_Invert_Table[TOP_cmpgtu_r]  = TOP_cmpleu_r;
  CGTARG_Invert_Table[TOP_cmpgtu_i8]  = TOP_cmpleu_i8;
  CGTARG_Invert_Table[TOP_cmple_r]   = TOP_cmpgt_r;
  CGTARG_Invert_Table[TOP_cmple_i8]   = TOP_cmpgt_i8;
  CGTARG_Invert_Table[TOP_cmpleu_r]  = TOP_cmpgtu_r;
  CGTARG_Invert_Table[TOP_cmpleu_i8]  = TOP_cmpgtu_i8;
  CGTARG_Invert_Table[TOP_cmplt_r]   = TOP_cmpge_r;
  CGTARG_Invert_Table[TOP_cmplt_i8]   = TOP_cmpge_i8;
  CGTARG_Invert_Table[TOP_cmpltu_r]  = TOP_cmpgeu_r;
  CGTARG_Invert_Table[TOP_cmpltu_i8]  = TOP_cmpgeu_i8;
  CGTARG_Invert_Table[TOP_cmpne_r]   = TOP_cmpeq_r;
  CGTARG_Invert_Table[TOP_cmpne_i8]   = TOP_cmpeq_i8;
#endif

  init_templates();

  Init_TOP_Variant_Tables();

  saveHWloopRegisterLR0 = FALSE;

  return;
}

extern void
CGTARG_Spill_on_Xfer(TN *tn, BB *bb, OPS *ops) {
  OP *br_op;

  FmtAssert(FALSE,("Not implemented"));
#if 0

  br_op = BB_last_op(bb);
  FmtAssert((OP_code(br_op) == TOP_jrgtudec) || (OP_code(br_op) == TOP_jagtudec),
	    ("Spill on Xfer only implemented for jr/agtudec"));

  OPS pre_ops = OPS_EMPTY;
  OPS post_ops = OPS_EMPTY;

  if (TN_register_class(tn) == ISA_REGISTER_CLASS_gr) {
    Is_True(OP_result(br_op, 0) == tn, ("Unexpected"));
    TN *gr_def = tn;
    TN *guard = OP_opnd(br_op, 0);
    if (gr_def == guard) {
      TN *spill_reg = Build_RCLASS_TN(ISA_REGISTER_CLASS_gpr);
      Exp_COPY(spill_reg, gr_def, &pre_ops);
      Exp_COPY(gr_def, spill_reg, &post_ops);
    }
    Build_OP(TOP_cmpgtu_i8, gr_def, True_TN, OP_result(br_op, 1), Gen_Literal_TN(1, 4), &pre_ops);
  }
  else {
    TN *Rm_def = OP_result(br_op, 1);
    Expand_Add(Rm_def, Rm_def, Gen_Literal_TN(-1, 4), MTYPE_U4, &pre_ops);
    Expand_Add(Rm_def, Rm_def, Gen_Literal_TN(1, 4), MTYPE_U4, &post_ops);
  }

  OPS_Prepend_Ops(ops, &pre_ops);
  OPS_Append_Ops(ops, &post_ops);

  // Must actually insert the OPS, otherwise CGSPILL_Append_Ops will
  // modify this code.
  BB_Insert_Ops_Before (bb, br_op, ops);
  OPS_Init(ops);
#endif
}

BOOL
CGTARG_Is_Simple_Jump(const OP* op)
{
    TOP opc = OP_code(op);
    return opc == TOP_b || opc == TOP_bx;
}

/** 
 * Returns whether basic bloc contain loop init.
 * 
 * Note that this api is called from CFLOW 
 * @param bb 
 * 
 * @return Bool
 */

BOOL
CGTARG_bb_is_hwloop_init(const BB* bb)
{
   return FALSE;
}


/** 
 * Returns whether op may alias with a call.
 * 
 * Note that this api is called from LICM on non register allocated
 * code.
 * To avoid a strong degradation of performance this api should only
 * return false for instruction that do not have an impact on register
 * pressure and thus do not result in spill !
 *
 *
 * @param op 
 * 
 * @return 
 */
BOOL
CGTARG_op_may_alias_with_call(const OP* op)
{
  return TRUE;
}

BOOL
CGTARG_registerclass_may_be_copied(ISA_REGISTER_CLASS cl)
{
  return TRUE;
}

/** 
 * check target specific rules on whether op may be moved
 * into targetbb
 * 
 * @param op 
 * @param targetbb 
 * 
 * @return 
 */
BOOL
CGTARG_Code_Motion_To_BB_Is_Legal(const OP* op, BB* targetbb)
{
  return TRUE;
}

/** 
 *  Target specific function that does minor local changes to the asm
 * just before asm output.
 *
 * empty on st200.
 * 
 */
void
CGTARG_Fixup_ASM_Code ()
{
  
}

/** 
 * Verify that op has no loop sideeffects that avoids it to be moved
 * upward.
 * 
 * @param _loop 
 * @param bb
 * @param op 
 * 
 * @return true/false
 */
BOOL
CGTARG_OP_Has_Loop_Sideeffects(BB* bb, OP* op) {
  return false;
}

/** 
 * This function finds out the bundle template id corresponding to
 * the slot-mask. order_change variable specifies whether the order had
 * to be changed.
 * 
 * @param mask 
 * @param order_changed 
 * 
 * @return  bundle_id
 */
int CGTARG_Detect_Bundle_Id(ISA_EXEC_MASK slot_mask, BOOL* order_changed) {
  *order_changed = FALSE;
  return 0;
}

/** 
 * fix op in bundle slots before code emission if slot_mask does not
 * conform to bundle template.
 * 
 * @param slot_mask 
 * @param slot_op 
 * @param ibundle 
 */
void
CGTARG_FixBundle(ISA_EXEC_MASK slot_mask, OP *slot_op[], INT ibundle){ 
}


/** 
 * Verify that loop does not contain if-converted code
 * @param bb
 *
 * @return true/false
 */
BOOL
CGTARG_do_not_unroll_p(BB* bb) {
    return FALSE;
}

INT
CGTARG_Check_Optimize_IV(IV_ref_t *iv_refs, INT ref_count, INT iv_step, INT iv_adjust) {
  // Never do the transformation
  return -ref_count;
}

/**
 * Return TRUE if it exists a single non-simulated instruction to copy
 * a composite register of class <rc> made of <nhardregs> atomic regs.
 */
BOOL
CGTARG_Exist_Single_OP_Copy(ISA_REGISTER_CLASS rc, INT nhardregs)
{
  return (nhardregs == 1);
}


/**
 * Return TRUE if the 2 TNs compose a single composite register.
 * All parameter TNs are expected to be allocated register TNs.
 */
BOOL
CGTARG_Is_Register_Pair(TN *tn1, TN * tn2)
{
  return FALSE;
}


/*
 * Return TRUE if the 4 TNs compose a single composite register.
 * All parameter TNs are expected to be allocated register TNs.
 */
BOOL
CGTARG_Is_Register_Quad(TN *tn1, TN *tn2, TN *tn3, TN *tn4)
{
  return FALSE;
}

#ifdef TARG_ST
BOOL
CGTARG_SSA_dedicated_candidate_tn(TN *tn) {
  ISA_REGISTER_CLASS cl = TN_register_class(tn);
  // The PC is explicitely used and defined on calls, but must not be
  // renamed
  if (!REGISTER_allocatable(cl, TN_register(tn)) && !TN_is_sp_reg(tn))
    return FALSE;
  return TRUE;
}
#endif

BOOL CGTARG_Should_Hoist_Return(BB* ret_bb) {
   return FALSE;
}
