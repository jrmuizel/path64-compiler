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
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description:
 *
 * Support routines for target-specific code generator functionality.
 *
 * ====================================================================
 * ====================================================================
 */

#include <ctype.h>

#include "defs.h"
#include "util.h"
#include "config.h"
#include "config_TARG.h"
#include "erglob.h"
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


UINT32 CGTARG_branch_taken_penalty;
BOOL CGTARG_branch_taken_penalty_overridden = FALSE;

mTOP CGTARG_Inter_RegClass_Copy_Table[ISA_REGISTER_CLASS_MAX+1][ISA_REGISTER_CLASS_MAX+1][2];

TOP CGTARG_Invert_Table[TOP_count+1];

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
  ISA_REGISTER_CLASS rclass;
  INT regnum;
  REGISTER reg;

  /* Get the target register number and class associated with the
   * preg, if there is one that is.
   */
  if (Preg_Offset_Is_Int(preg)) {
    regnum = preg - Int_Preg_Min_Offset;
    rclass = ISA_REGISTER_CLASS_du;
  }
  else if (Preg_Offset_Is_Ptr(preg)) {
    regnum = preg - Ptr_Preg_Min_Offset;
    rclass = ISA_REGISTER_CLASS_au;
  }
  else if (preg == 0) {
    FmtAssert(FALSE, ("preg = 0"));
    /* 0 not considered part of normal int group for error purposes,
     * but in our case it can be zero_tn. */
    regnum = 0;
    rclass = ISA_REGISTER_CLASS_du;
  }
  else {
    return FALSE;
  }

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
  BB *bb
)
{
  OP *new_br;
  TOP new_top;
  OP* br_op = BB_branch_op(bb);

  if (!br_op) return;

  FmtAssert(FALSE,("Make_Branch_Conditional: not implemented"));

#if 0
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
#endif

  OP_srcpos(new_br) = OP_srcpos(br_op);
  BB_Insert_Op_After(bb, br_op, new_br);
  BB_Remove_Op(bb, br_op);
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
  //
  // None for floats.  For gp registers, only support "eq" and "ne"
  // for arbitrary register compares.  Others must have one operand
  // be the zero register.
  //
  if (OP_flop(cmp_op)) {
    return TOP_UNDEFINED;
  }

  FmtAssert(FALSE,("CGTARG_Parallel_Compare: not implemented"));

  TOP cmp_top = OP_code(cmp_op);
  if (OP_opnd(cmp_op, 1) != Zero_TN && OP_opnd(cmp_op, 2) != Zero_TN) {
    switch (cmp_top) {
      /*
    case TOP_cmp_ne:
    case TOP_cmp4_ne:
    case TOP_cmp_i_ne:
    case TOP_cmp4_i_eq:
    case TOP_cmp4_eq:
    case TOP_cmp_i_eq:
    case TOP_cmp_eq:
      break;
      */
    default:
      return TOP_UNDEFINED;
    }
  }

  /*
  switch (cmp_top) {

  }
  */

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
  // There is no unconditional compares on the ST100 for the
  // moment:
  switch (top) {

  default:
    *uncond_ver = TOP_UNDEFINED;
  }
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

  if (Is_Target_ST120_v101())
  {
    *mispredict= 8; *fixed= 1; *brtaken= 1; *factor = 1.0;
  }
  else
  {
    FmtAssert(FALSE, ("CGTARG_Compute_Branch_Parameters: invalid target"));
  }

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
 * ====================================================================
 */
INT 
CGTARG_Analyze_Branch (
  OP *br,
  TN **tn1,
  TN **tn2
)
{
  INT variant;

  *tn1 = NULL;
  *tn2 = NULL;

  switch (OP_code(br)) {
  case TOP_noop:
    {
      TN* pred_tn = OP_opnd(br, OP_PREDICATE_OPND);

      if (TN_register_and_class(pred_tn) != CLASS_AND_REG_true) {
	*tn1 = pred_tn;
	variant = V_BR_P_TRUE;
	break;
      }
    }
    /*FALLTHROUGH*/

  case TOP_label:

    variant = V_BR_ALWAYS;

    variant = V_BR_CLOOP;

    variant = V_BR_CTOP;

    variant = V_BR_CEXIT;

    variant = V_BR_WTOP;

    variant = V_BR_WEXIT;

  default:
    variant = V_BR_NONE;
    break;
  }

  return variant;
}

/* ====================================================================
 *   CGTARG_Analyze_Compare
 * ====================================================================
 */
INT 
CGTARG_Analyze_Compare (
  OP *br,
  TN **tn1,
  TN **tn2,
  OP **compare_op
)
{
  INT variant;
  TN *cond_tn1;
  TN *cond_tn2;
  BOOL is_double;
  DEF_KIND kind;

  /* Classify the condition based on the branch instruction.
   */
  variant = CGTARG_Analyze_Branch(br, &cond_tn1, &cond_tn2);

  /*
   * Check for double precision float compare.
   */
  if (cond_tn1 && TN_is_float(cond_tn1) && TN_size(cond_tn1) == 8) {
    is_double = TRUE;
  } else {
    is_double = FALSE;
  }

  /* If the branch was predicated, the value being tested
   * was probably the result of a cmp*, in which case we
   * we can refine the actual condition.
   */
  if (variant == V_BR_P_TRUE) {
    OP *def_op;

    /* Attempt to find the defining OP for the tested value.
     */
    def_op = TN_Reaching_Value_At_Op(cond_tn1, br, &kind, TRUE);

    // TODO: <kind != VAL_KNOWN> may be too conservative. Analyze_Compare
    // need to work with compare defs which could be predicated themselves.
    if (!def_op || kind != VAL_KNOWN) goto no_cmp;

    /* Verify the defining OP is a cmp* and determine the actual
     * condition being tested.
     */
    switch (OP_code(def_op)) {

    default:
      // unc?
      // parallel form?
      goto no_cmp;
    }

    /* If the branch is predicated by the false result of the cmp,
     * then the comparison needs to be negated.
     */
    if (OP_result(def_op, 1) == cond_tn1) {
      variant = Negate_BR_Variant(variant);
    }

    /* The arguments being compared are the args of the cmp,
     * and the comparison OP is the cmp.
     */
    cond_tn1 = OP_opnd(def_op, 2);
    cond_tn2 = OP_opnd(def_op, 3);

    *compare_op = def_op;
  } else {

    /* No cmp: 
     */
  no_cmp:
    *compare_op = NULL;
  }

  *tn1 = cond_tn1;
  *tn2 = cond_tn2;
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

  CGTARG_Unconditional_Compare_Helper(top, &result);
  
  if (result == TOP_UNDEFINED) {
    result = top;
  }
  
  return (result);
}

/* ====================================================================
 *   CGTARG_Dependence_Required
 * ====================================================================
 */
BOOL
CGTARG_Dependence_Required (
  OP *pred_op, 
  OP *succ_op
)
{

  BOOL read_write_predicate,	   // all TOPs which read/write predicate regs
       write_predicate,		   // all TOPs which write predicate regs
       read_write_status_field;    // all TOPs which read/write a specific
                                   // status field regs

  read_write_predicate = FALSE;
  write_predicate = FALSE;
  read_write_status_field = FALSE;

  OP *hazard_op = NULL;

  // The dependences that need to be preserved are:

  // (1) fsetc.xx
  // reads/writes to status field register 'xx' cannot move above/below
  // this instruction.

  /*
  switch (OP_code(pred_op)) {
    case TOP_fsetc:
      read_write_status_field = TRUE;
      hazard_op = succ_op;
      break;
    }
    break;
  }
  */

  if (hazard_op) {
    OP *other_op = (hazard_op == pred_op) ? succ_op : pred_op;

    // Special case other instruction being an fsetc instruction as 
    // it has an implicit read of .s0 

    TN *status_field; // the status field reg written by hazard_op
    if (read_write_status_field) {
      status_field = OP_opnd(hazard_op, 1);
      //      if (OP_code(other_op) == TOP_fsetc &&
      //	  TN_enum(status_field) == ECV_sf_s0) return TRUE;
    }

    INT k;
    for (k = 0; k < OP_opnds(other_op); k++) {
      TN *opnd_tn = OP_opnd(other_op, k);

      if (TN_is_constant(opnd_tn)) {
	if (read_write_status_field &&
	    TN_is_enum(opnd_tn) &&
	    TN_enum(opnd_tn) == TN_enum(status_field)) return TRUE;
	continue;
      }

      if (TN_is_const_reg(opnd_tn)) continue;

      REGISTER reg = TN_register(opnd_tn);
      ISA_REGISTER_CLASS reg_class = TN_register_class(opnd_tn);

      if (read_write_predicate &&
	  reg_class == ISA_REGISTER_CLASS_guard) return TRUE;

      if (reg == REGISTER_UNDEFINED) continue;
    }

    for (k = 0; k < OP_results(other_op); k++) {
      TN *result_tn = OP_result(other_op, k);

      if (TN_is_const_reg(result_tn)) continue;

      REGISTER reg = TN_register(result_tn);
      ISA_REGISTER_CLASS reg_class = TN_register_class(result_tn);

      // can have conflict with op that writes to predicate,
      // even though haven't assigned registers yet.
      if (write_predicate &&
	   reg_class == ISA_REGISTER_CLASS_guard) return TRUE;

      if (reg == REGISTER_UNDEFINED) continue;
    }
  }

  // The following descriptions below present target-specific ordering
  // constraints that must be respected. They can't fit in the same
  // instruction group.
  //
  // alloc
  //
  // WITH ANY OF THE BELOW
  //
  // flushrs, 
  // move from ar.bpstore, 
  // move from ar.rnat, 
  // br.cexit, 
  // br.ctop,
  // br.wexit, 
  // br.wtop, 
  // br.call, 
  // br.ia, 
  // br.ret,
  // clrrrb.

  /*
  if (OP_code(pred_op)  == TOP_alloc &&
      (OP_code(succ_op) == TOP_flushrs ||
       OP_code(succ_op) == TOP_br_cexit,
       OP_code(succ_op) == TOP_br_ctop ||
       OP_code(succ_op) == TOP_br_wexit ||
       OP_code(succ_op) == TOP_br_wtop ||
       OP_code(succ_op) == TOP_br_call ||
       OP_code(succ_op) == TOP_br_ia ||
       OP_code(succ_op) == TOP_br_ret ||
       OP_code(succ_op) == TOP_clrrrb ||
       OP_code(succ_op) == TOP_clrrrb_pr)) return TRUE;
  */
  return FALSE;
}

/* ====================================================================
 *                          Local ASMs stuff:
 * ====================================================================
 */
static TN* asm_constraint_tn[10];
static ISA_REGISTER_SUBCLASS asm_constraint_sc[10];
static char asm_constraint_name[10][8];
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
  for (INT i = 0; i < 10; ++i) {
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
  static const char* modifiers = "=&%";
  while (strchr(modifiers, *constraint)) {
    constraint++;
  }
  
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
  static const char* immediates = "in";
  while (strchr(immediates, *constraint) && *(constraint+1)) {
    constraint++;
  }

  TN* ret_tn;
  
  // TODO: check that the operand satisifies immediate range constraint
  if (strchr(immediates, *constraint)) {
    if (load && WN_operator(load)==OPR_LDID && WN_class(load)==CLASS_PREG) {
      // immediate could have been put in preg by wopt
      load = Preg_Is_Rematerializable(WN_load_offset(load), NULL);
    }
    FmtAssert(load && WN_operator(load) == OPR_INTCONST, 
              ("Cannot find immediate operand for ASM"));
    ret_tn = Gen_Literal_TN(WN_const_val(load), 
                            MTYPE_bit_size(WN_rtype(load))/8);
  }

  // digit constraint means that we should reuse a previous operand
  else if (isdigit(*constraint)) {
    INT prev_index = *constraint - '0';
    FmtAssert(asm_constraint_tn[prev_index], 
              ("numeric matching constraint refers to NULL value"));
    ret_tn = asm_constraint_tn[prev_index];
  }
    
  else if (strchr("gmr", *constraint)) {
    TYPE_ID rtype = (load != NULL ? WN_rtype(load) : MTYPE_I8);
    FmtAssert(MTYPE_is_integral(rtype),
              ("ASM operand does not satisfy its constraint"));
    ret_tn = (pref_tn ? pref_tn : Build_TN_Of_Mtype(rtype));
  }
  
  else if (*constraint == 'f') {
    TYPE_ID rtype = (load != NULL ? WN_rtype(load) : MTYPE_F8);
    FmtAssert(MTYPE_is_float(rtype),
              ("ASM operand does not satisfy its constraint"));
    ret_tn = (pref_tn ? pref_tn : Build_TN_Of_Mtype(rtype));
  }

  else {
    FmtAssert(FALSE, ("ASM constraint <%s> not supported", constraint));
  }

  asm_constraint_tn[asm_constraint_index] = ret_tn;
  asm_constraint_index++;
  
  return ret_tn;
}

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
  while ((p = strstr(in, from)) != NULL) {
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
char* 
CGTARG_Modify_Asm_String (
  char* asm_string, 
  UINT32 position, 
  bool memory, 
  TN* tn
)
{
  char* name;
  if (TN_is_register(tn)) {
    ISA_REGISTER_CLASS cl = TN_register_class(tn);
    REGISTER reg = TN_register(tn);
    name = (char*) REGISTER_name(cl, reg);
    if (memory) {
      char* buf = (char*) alloca(strlen(name)+3);
      sprintf(buf, "[%s]", name);
      name = buf;
    }
  }
  else {
    FmtAssert(!memory && TN_is_constant(tn) && TN_has_value(tn),
              ("ASM operand must be a register or a numeric constant"));
    char* buf = (char*) alloca(32);
    sprintf(buf, "%lld", TN_value(tn));
    name = buf;
  }
  
  char pattern[4];
  sprintf(pattern, "%%%c", '0'+position);
  
  asm_string =  Replace_Substring(asm_string, pattern, name);

  if (TN_is_register(tn)) {
    for (INT i = 0; i < CGTARG_Num_Asm_Opnd_Modifiers; i++) {
      char modifier = CGTARG_Asm_Opnd_Modifiers[i];
      sprintf(pattern, "%%%c%c", modifier, '0'+position);
      name = (char*) CGTARG_Modified_Asm_Opnd_Name(modifier, tn, name);
      asm_string  = Replace_Substring(asm_string, pattern, name);
    }
  }
  
  return asm_string;
}

/* ====================================================================
 *
 * CGTARG_Postprocess_Asm_String: remove %Pn postincrement modifiers
 * %Pn says that operand n "may" be a postincrement, and if the
 * compiler wants to use a postincrement form, then put the postincrement
 * value here.  But we don't want to bother ever trying to do this,
 * so we'll just ignore it.
 *
 * ====================================================================
 */
void 
CGTARG_Postprocess_Asm_String (
  char *s
)
{
  char *p;
  while (TRUE) {
    p = strstr(s, "%P");
    if (p == NULL) break;
    // insert blanks cause easier than removing spaces
    *p = ' ';	// %
    ++p;
    *p = ' ';	// P
    ++p;
    *p = ' ';	// <number>
  }
}

/* ====================================================================
 * ====================================================================
 *                            ISA Helpers:
 * ====================================================================
 * ====================================================================
 */

/* ====================================================================
 *   CGTARG_Spill_Type
 *
 *   Given a TN to spill and the precision range of its contents,
 *   return the high-level type for the spill operation.
 * ====================================================================
 */
TY_IDX 
CGTARG_Spill_Type (
  TN *tn,
  INT range
) 
{
  // For now range is always '0'; return I4 by default:
  switch (TN_register_class(tn)) {
    case ISA_REGISTER_CLASS_guard:
      return MTYPE_To_TY(MTYPE_I1);
    case ISA_REGISTER_CLASS_du:
      //      return Spill_Int_Type;
      switch (range) {
	//      case 0: return MTYPE_To_TY(MTYPE_I5);
      default: return MTYPE_To_TY(MTYPE_I4);
      }

    case ISA_REGISTER_CLASS_au:
      //      return Spill_Ptr_Type;
      return MTYPE_To_TY(MTYPE_A4);

    default:
      FmtAssert(FALSE,("CGTARG_Spill_Type: wrong TN register class"));
  }
}

/* ====================================================================
 *   CGTARG_Load_From_Memory
 * ====================================================================
 */
void CGTARG_Load_From_Memory (
  TN *tn, 
  ST *mem_loc, 
  OPS *ops
)
{
  TYPE_ID mtype = TY_mtype(ST_type(mem_loc));

  if (TN_register_class(tn) == ISA_REGISTER_CLASS_guard) {
    /* 
     * Since we can't directly load a predicate TN, first load into
     * an integer register and then set the predicate by checking for
     * a non-zero value.
     */
    TN *temp_tn = Build_TN_Of_Mtype (MTYPE_I4);
    Exp_Load (mtype, mtype, temp_tn, mem_loc, 0, ops, V_NONE);
    Build_OP (TOP_GP32_NEW_GT_BR_DR_U8, tn, True_TN, 
                          temp_tn, Gen_Literal_TN(0, 4), ops);
  }
  else {
    /* 
     * Non-predicate TNs are just a simple load.
     */
    Exp_Load (mtype, mtype, tn, mem_loc, 0, ops, V_NONE);
  }

  return;
}

/* ====================================================================
 *   CGTARG_Store_To_Memory
 * ====================================================================
 */
void CGTARG_Store_To_Memory(TN *tn, ST *mem_loc, OPS *ops)
{
  TYPE_ID mtype = TY_mtype(ST_type(mem_loc));

  if (TN_register_class(tn) == ISA_REGISTER_CLASS_guard) {
    /* 
     * Since we can't directly store a predicate TN, first copy to
     * an integer register and then store.
     */
    TN *temp_tn = Build_TN_Of_Mtype (MTYPE_I4);
    Build_OP (TOP_GP32_BOOL_GT_DR_BR, temp_tn, True_TN, tn, ops);
    Exp_Store (mtype, temp_tn, mem_loc, 0, ops, V_NONE);
  }
  else {
    /* 
     * Non-predicate TNs are just a simple store.
     */
    Exp_Store (mtype, tn, mem_loc, 0, ops, V_NONE);
  }

  return;
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
  return ISA_LC_Value_In_Class (immed, LC_u8);
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
	return ISA_LC_Value_In_Class (immed, LC_s16);
}

/* ====================================================================
 *   CGTARG_Is_OP_Advanced_Load
 * ====================================================================
 */
BOOL
CGTARG_Is_OP_Advanced_Load( OP *memop )
{
  if (!OP_load(memop)) return FALSE;

  INT i;
  for (i = 0; i < OP_opnds(memop); ++i) {
    TN *opnd_tn = OP_opnd(memop, i);
    //    if (TN_is_enum(opnd_tn)) {
    //      if (TN_enum(opnd_tn) == ECV_ldtype_a ||
    //	  TN_enum(opnd_tn) == ECV_ldtype_sa) return TRUE;
    //    }
  }

  return FALSE;
}

/* ====================================================================
 *
 * CGTARG_Is_OP_Speculative_Load
 *
 * See interface description
 *
 * ====================================================================
 */
BOOL
CGTARG_Is_OP_Speculative_Load ( 
  OP *memop 
)
{
  if (!OP_load(memop)) return FALSE;

  INT i;
  for (i = 0; i < OP_opnds(memop); ++i) {
    TN *opnd_tn = OP_opnd(memop, i);
    //    if (TN_is_enum(opnd_tn)) {
    //      if (TN_enum(opnd_tn) == ECV_ldtype_s ||
    //	  TN_enum(opnd_tn) == ECV_ldtype_sa) return TRUE;
    //    }
  }

  return FALSE;
}

/* ====================================================================
 *   CGTARG_Is_OP_Check_Load
 *
 *   no check loads on ST100.
 * ====================================================================
 */
BOOL
CGTARG_Is_OP_Check_Load ( 
  OP *memop 
)
{
  if (!OP_load(memop)) return FALSE;

  return FALSE;
}

/* ====================================================================
 *   CGTARG_Is_OP_Speculative
 * ====================================================================
 */
BOOL
CGTARG_Is_OP_Speculative (
  OP *op
)
{
  if (!OP_load(op)) return FALSE;

  // speculative and advanced loads are safe to speculate.
  if (CGTARG_Is_OP_Advanced_Load(op) || CGTARG_Is_OP_Speculative_Load(op))
    return TRUE;

  return FALSE;
}

/* ====================================================================
 *   CGTARG_Can_Be_Speculative
 * ====================================================================
 */
BOOL
CGTARG_Can_Be_Speculative ( 
  OP *op 
)
{
  WN *wn;

  /* not allowed to speculate anything. */
  if (Eager_Level == EAGER_NONE) return FALSE;

  /* don't speculate volatile memory references. */
  if (OP_volatile(op)) return FALSE;

  if (TOP_Can_Be_Speculative(OP_code(op))) return TRUE;

  if (!OP_load(op)) return FALSE;

  /* Try to identify simple scalar loads than can be safely speculated:
   *  a) read only loads (literals, GOT-loads, etc.)
   *  b) load of a fixed variable (directly referenced)
   *  c) load of a fixed variable (base address is constant or
   *     known to be in bounds)
   *  d) speculative, advanced and advanced-speculative loads are safe.
   */

  /*  a) read only loads (literals, GOT-loads, etc.)
   */
  if (OP_no_alias(op)) goto scalar_load;

  /*  b) load of a fixed variable (directly referenced); this
   *     includes spill-restores.
   *  b') exclude cases of direct loads of weak symbols (#622949).
   */
  if (TN_is_symbol(OP_opnd(op, 1)) &&
      !ST_is_weak_symbol(TN_var(OP_opnd(op, 1)))) goto scalar_load;

  /*  c) load of a fixed variable (base address is constant or
   *     known to be in bounds), comment out the rematerizable bit check 
   *     since it doesn;t guarantee safeness all the time.
   */
  if (/*   TN_is_rematerializable(OP_opnd(op, 0)) || */
      (   (wn = Get_WN_From_Memory_OP(op))
	  && Alias_Manager->Safe_to_speculate(wn))) goto scalar_load;

  /* d) speculative, advanced, speculative-advanced loads are safe to 
   *    speculate. 
   */
  if (CGTARG_Is_OP_Speculative(op)) goto scalar_load;

  /* If we got to here, we couldn't convince ourself that we have
   * a scalar load -- no speculation this time...
   */
  return FALSE;

  /* We now know we have a scalar load of some form. Determine if they
   * are allowed to be speculated.
   */
scalar_load:
  return TRUE; 
}

/* ====================================================================
 *   CGTARG_OP_is_counted_loop
 * ====================================================================
 */
BOOL 
CGTARG_OP_is_counted_loop (
  OP *op
) 
{
  return FALSE;
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
  if (OP_opnd(br_op, OP_PREDICATE_OPND) == OP_result(cmp_op, 0)) {
    return TRUE;
  }
  return FALSE;
}

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

  FmtAssert(FALSE,("CGTARG_Generate_Remainder_Branch: not implemented"));

  LC_Used_In_PU = TRUE;
  if (TN_is_constant(trip_count)) {
    TN *tmp_tn = Gen_Register_TN (ISA_REGISTER_CLASS_du,
				  trip_size);
    Exp_COPY(tmp_tn, 
        Gen_Literal_TN(TN_value(trip_count)-1, trip_size), prolog_ops);
    Exp_COPY(LC_TN, tmp_tn, prolog_ops);
  } else {
    TN *trip_counter_minus_1 = 
                    Gen_Register_TN (ISA_REGISTER_CLASS_du, trip_size);
    Exp_OP2(OPC_I5SUB,
	    trip_counter_minus_1,
	    trip_count,
	    Gen_Literal_TN(1, trip_size),
	    prolog_ops);
    Exp_COPY(LC_TN, trip_counter_minus_1, prolog_ops);
  }

  /*
  Build_OP (TOP_br_cloop, 
  	    LC_TN,
  	    Gen_Enum_TN(ECV_bwh_dptk),
  	    Gen_Enum_TN(ECV_ph_few),
  	    Gen_Enum_TN(ECV_dh),
	    label_tn, 
	    LC_TN,
	    body_ops);
  */
}

/* ====================================================================
 *
 * CGTARG_Generate_Branch_Cloop
 *
 * See interface description
 *
 * ====================================================================
 */
void
CGTARG_Generate_Branch_Cloop(OP *br_op,
			     TN *unrolled_trip_count,
			     TN *trip_count_tn,
			     INT32 ntimes,
			     TN *label_tn,
			     OPS *prolog_ops,
			     OPS *body_ops)
{ 

  FmtAssert(FALSE,("CGTARG_Generate_Branch_Cloop: not implemented"));

  LC_Used_In_PU = TRUE;

  if (!CGTARG_OP_is_counted_loop(br_op)) {
    /*
    Build_OP (TOP_br_cloop, 
	      LC_TN,
	      Gen_Enum_TN(ECV_bwh_dptk),
	      Gen_Enum_TN(ECV_ph_few),
	      Gen_Enum_TN(ECV_dh),
	      label_tn, 
	      LC_TN,
	      body_ops);
    */
  }

  INT32 trip_size = TN_size(trip_count_tn);
  TN *unrolled_trip_count_minus_1;

  if (TN_is_constant(trip_count_tn)) {
    unrolled_trip_count_minus_1 = 
         Gen_Literal_TN(TN_value(trip_count_tn) / ntimes - 1, trip_size);
  } else {
    if (unrolled_trip_count == NULL) {
      unrolled_trip_count = Build_TN_Like(trip_count_tn);
      Exp_OP2(trip_size == 4 ? OPC_U4DIV : OPC_U8DIV,
	      unrolled_trip_count,
	      trip_count_tn,
	      Gen_Literal_TN(ntimes, trip_size),
	      prolog_ops);
    }
    unrolled_trip_count_minus_1 = 
                      Gen_Register_TN (ISA_REGISTER_CLASS_du, trip_size);
    Exp_OP2(OPC_I8SUB,
	    unrolled_trip_count_minus_1,
	    unrolled_trip_count,
	    Gen_Literal_TN(1, trip_size),
	    prolog_ops);
  } 

  // workaround a Exp_COPY bug?
  if (TN_is_constant(unrolled_trip_count_minus_1)) {
    TN *tmp_tn = Gen_Register_TN (ISA_REGISTER_CLASS_du, trip_size);
    Exp_COPY(tmp_tn, unrolled_trip_count_minus_1, prolog_ops);
    unrolled_trip_count_minus_1 = tmp_tn;
  }

  Exp_COPY(LC_TN, unrolled_trip_count_minus_1, prolog_ops);
}

/* ====================================================================
 * ====================================================================
 *                 Scheduling related stuff:
 * ====================================================================
 * ====================================================================
 */

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
  INT *latency
)
{
  const TOP pred_code = OP_code(pred_op);
  const TOP succ_code = OP_code(succ_op);

  //  DevWarn("CGTARG_Adjust_Latency: not implemented");
  return;
}

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

/* ======================================================================
 *   Delay_Scheduling_OP
 *
 *   Placeholder routine to check if placement of <op> at <slot_pos> in
 *   a <bundle> can be delayed.
 *
 * Arthur: I moved it from hb_hazards.cxx for two reasons:
 *         1. it is target dependent;
 *         2. I want to change the functionality: once function
 *            CGTARG_Bundle_Slot_Available() returns TRUE, I want
 *            the op to be guaranteed to be bundled. In this case,
 *            I can reserve stop bits (and eventually everything else
 *            in CGTARG_Bundle_Slot_Available().
 *
 * ======================================================================
 */
static BOOL
Delay_Scheduling_OP (
  OP *op, 
  INT slot_pos, 
  TI_BUNDLE *bundle
)
{

  // If <op> is a <xfer_op>, we would like to not greedily bundle it.
  // Rather, delay the same, so that nops (if necessary) can be 
  // inserted before (instead of after). As a result, any <xfer_op>
  // will be the last_op in a legal bundle.

#ifdef TARG_IA64
  if (BB_last_op(OP_bb(op)) == op && OP_xfer(op) && 
      (slot_pos != (ISA_MAX_SLOTS - 1))) 
    return TRUE;
#endif

  // If <op> needs to be the last member of the group, check for slot
  // positions and any prior stop bits present.
  if (OP_l_group(op) && (slot_pos != (ISA_MAX_SLOTS - 1)) &&
      !TI_BUNDLE_stop_bit(bundle, slot_pos))
    return TRUE;

  return FALSE;

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
			     const CG_GROUPING      *grouping)
{
  // If slot already filled, return FALSE.
  if (TI_BUNDLE_slot_filled(bundle, slot)) return FALSE;

  INT  inst_words = ISA_PACK_Inst_Words(OP_code(op));

  if (EXEC_PROPERTY_is_E0_Unit(OP_code(op)) &&
      EXEC_PROPERTY_is_E1_Unit(OP_code(op))) {
    if (slot == 0)
      *prop = ISA_EXEC_PROPERTY_E0_Unit;
    if (slot == 1)
      *prop = ISA_EXEC_PROPERTY_E1_Unit;
  }
  else if (EXEC_PROPERTY_is_E1_Unit(OP_code(op))) {
    // for example GOTO_GF_S21, etc.
    *prop = ISA_EXEC_PROPERTY_E1_Unit;
  }
  else if (EXEC_PROPERTY_is_S0_Unit(OP_code(op)) &&
	   EXEC_PROPERTY_is_S1_Unit(OP_code(op))) {
    if (slot == 0)
      *prop = ISA_EXEC_PROPERTY_S0_Unit;
    if (slot == 1)
      *prop = ISA_EXEC_PROPERTY_S1_Unit;
  }
  else if (EXEC_PROPERTY_is_S1_Unit(OP_code(op))) {
    // LSR scheduling class:
    *prop = ISA_EXEC_PROPERTY_S1_Unit;
  }
  else if (EXEC_PROPERTY_is_G_Unit(OP_code(op))) {
    *prop = ISA_EXEC_PROPERTY_G_Unit;
  }
  else if (EXEC_PROPERTY_is_B_Unit(OP_code(op))) {
    *prop = ISA_EXEC_PROPERTY_B_Unit;
  }
  else if (EXEC_PROPERTY_is_A0_Unit(OP_code(op)) &&
	   EXEC_PROPERTY_is_E1_Unit(OP_code(op))) {
    if (slot == 0)
      *prop = ISA_EXEC_PROPERTY_A0_Unit;
    if (slot == 1)
      *prop = ISA_EXEC_PROPERTY_E1_Unit;
  }
  else if (EXEC_PROPERTY_is_E0ND_Unit(OP_code(op)) &&
	   EXEC_PROPERTY_is_A1_Unit(OP_code(op))) {
    if (slot == 0)
      *prop = ISA_EXEC_PROPERTY_E0ND_Unit;
    if (slot == 1)
      *prop = ISA_EXEC_PROPERTY_A1_Unit;
  }
  else if (EXEC_PROPERTY_is_A2D_Unit(OP_code(op))) {
    *prop = ISA_EXEC_PROPERTY_A2D_Unit;
  }
  else if (EXEC_PROPERTY_is_D2A_Unit(OP_code(op))) {
    *prop = ISA_EXEC_PROPERTY_D2A_Unit;
  }
  else if (EXEC_PROPERTY_is_T_Unit(OP_code(op))) {
    *prop = ISA_EXEC_PROPERTY_T_Unit;
  }
  else {
    FmtAssert(FALSE, 
      ("CGTARG_Bundle_Slot_Available: unknown OP_code property 0x%x",
             ISA_EXEC_Unit_Prop(OP_code(op))));
  }

  // If there is a need to delay the scheduling of <op>...
  // It is done after determination of prop because apparently
  // prop is needed to be returned ?
  if (Delay_Scheduling_OP(op, slot, bundle)) return FALSE;

  // Need to check if extra slots are required and available within the
  // bundle (eg. movl)
  BOOL extra_slot_reqd = (inst_words == 2) ?
    TI_BUNDLE_Slot_Available(bundle, *prop, slot + 1) : TRUE;

  // if <stop_bit_reqd>, check for the availability of STOP bit being
  // available at position <slot - 1>.
  BOOL stop_bit_avail = (stop_bit_reqd) ? 
    CGTARG_Bundle_Stop_Bit_Available(bundle, slot - 1) : TRUE;
  
  // If stop bit required, reserve it
  if (slot > 0) {
    if (stop_bit_reqd && stop_bit_avail) {
      TI_BUNDLE_Reserve_Stop_Bit(bundle, slot - 1);
    }
  }
   
  // All bundles have a stop at the end, reserve it
  TI_BUNDLE_Reserve_Stop_Bit(bundle, 1);

  BOOL slot_avail = extra_slot_reqd && stop_bit_avail &&
    TI_BUNDLE_Slot_Available (bundle, *prop, slot);

  if (slot_avail) {
    // Arthur: I prefer to reserve stuff here rather than in hb_hazards.cxx
    TI_BUNDLE_Reserve_Slot (bundle, slot, *prop);
  }
  else {
    // Need to unreserve the stop bits that I have reserved:
    if (slot > 0) {
      if (stop_bit_reqd && stop_bit_avail) {
	TI_BUNDLE_Unreserve_Stop_Bit(bundle, slot - 1);
      }
    }

    TI_BUNDLE_Unreserve_Stop_Bit(bundle, 1);
  }

  return slot_avail;
}

/* ====================================================================
 *   CGTARG_Handle_Bundle_Hazard
 *
 *   Handle all bundle hazards in this routine.
 *   Seemed pretty target independent to me, so moved its body to
 *   hb_hazards.cxx. However, for all target-dependent stuff this
 *   stub is left as a possibility.
 * ====================================================================
 */
void
CGTARG_Handle_Bundle_Hazard (OP                          *op, 
			     TI_BUNDLE                   *bundle, 
			     VECTOR                      *bundle_vector,
			     BOOL                        can_fill, 
			     INT                         slot_pos, 
			     INT                         max_pos,
			     BOOL                        stop_bit_reqd,
			     ISA_EXEC_UNIT_PROPERTY      prop) 
{
  INT ti_err = TI_RC_OKAY;
  INT template_bit = TI_BUNDLE_Return_Template(bundle);
  FmtAssert (template_bit != -1, ("Illegal template encoding"));

  // Adjust the slot_pos for TOPs which occupy more than 1 slot position,
  // eg. ??
  INT adjusted_slot_pos = (can_fill && ISA_PACK_Inst_Words(OP_code(op)) > 1) ? 
    max_pos + ISA_PACK_Inst_Words(OP_code(op)) - 1 : max_pos;

  INT i;
  OP *prev_op = NULL;
  FOR_ALL_SLOT_MEMBERS(bundle, i) {
    if (i > adjusted_slot_pos) break;
    if (!TI_BUNDLE_slot_filled(bundle, i)) {
      if (i <= max_pos) {
	OP *noop = Mk_OP(CGTARG_Noop_Top(ISA_EXEC_Slot_Prop(template_bit, i)));

	// Check for conditions if noops need to be inserted before (or
	// after) <op>.
	if ((can_fill && !OP_xfer(op) && i >= slot_pos)) { 
	  BB_Insert_Op_After(OP_bb(op), (prev_op) ? prev_op : op, noop);
 	  OP_scycle(noop) = -1;
	  prev_op = noop;
        }
	else {
	  BB_Insert_Op_Before(OP_bb(op), op, noop);
 	  OP_scycle(noop) = -1;
	  prev_op = (can_fill) ? op : noop;
	}
	Set_OP_bundled (noop);
	TI_BUNDLE_Reserve_Slot (bundle, i, ISA_EXEC_Slot_Prop(template_bit, i));
      } else {
	TI_BUNDLE_Reserve_Slot (bundle, i, prop);
	prev_op = op;
      }
    }
  }

  // if the <bundle> is full, set the <end_group> marker appropriately.
  if (TI_BUNDLE_Is_Full(bundle, &ti_err)) {
    FmtAssert(ti_err != TI_RC_ERROR, ("%s", TI_errmsg));

#if 0
    if (OP_xfer(op)) {
      if (BB_last_real_op(OP_bb(op)) == op)	Set_OP_end_group(op);
      if (!can_fill && stop_bit_reqd) {
	Set_OP_end_group(prev_op);
	VECTOR_Reset (*bundle_vector);
      }
    }
    else if (prev_op && 
	     ((prev_op != op && stop_bit_reqd) || 
	      (BB_last_op(OP_bb(prev_op)) == prev_op))) {
      Set_OP_end_group(prev_op);
      VECTOR_Reset (*bundle_vector);
    }
    else if (can_fill && BB_last_op(OP_bb(op)) == op) {
      Set_OP_end_group(op);
      VECTOR_Reset (*bundle_vector);
    }
#else
    // On ST100 always set <end_group> marker when the bundle is full
    if (prev_op && prev_op) {
      // if we added a noop:
      Set_OP_end_group(prev_op);
      VECTOR_Reset (*bundle_vector);
    }
    else {
      Set_OP_end_group(op);
      VECTOR_Reset (*bundle_vector);
    }
#endif

  }

  return;
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
#if 0
  // check if appropriate flag is enabled.
  if (!Itanium_a0_step) return;

  INT slot = 0;	// position in bundle, assumes bb starts new bundle
  OP *op;
  // Assumes that bundles have already been laid out.
  FOR_ALL_BB_OPs_FWD (bb, op) {

    if (Is_OP_fp_op1(op)) {
      OP *noop1, *noop2, *noop3;

      // Need to find last op in bundle;
      // are 3 cases:  mmf, mfi, mfb.
      // never have m in last slot, so previous mm means mmf bundle.
      OP *last_op_in_bundle;
      if (slot == 2) {
	// mmf bundle, so in last slot of bundle
	last_op_in_bundle = op;
      }
      else {
	// mfi or mfb bundle, so in middle of bundle.
      	// Advance to the next OP, set the end_of_group marker.
      	last_op_in_bundle = OP_next(op);
      	Set_OP_end_group(last_op_in_bundle);
      }

      // Add an empty bundle <nop.m, nop.i, nop.i>;; with the stop bit
      // inserted.
      noop1 = Mk_OP (TOP_nop_m, True_TN, Gen_Literal_TN(0, 4));
      BB_Insert_Op_After(bb, last_op_in_bundle, noop1);
      noop2 = Mk_OP (TOP_nop_i, True_TN, Gen_Literal_TN(0, 4));
      BB_Insert_Op_After(bb, noop1, noop2);
      noop3 = Mk_OP (TOP_nop_i, True_TN, Gen_Literal_TN(0, 4));
      BB_Insert_Op_After(bb, noop2, noop3);

      Set_OP_end_group(noop3);
    }

    if (OP_dummy(op)) continue;
    if (OP_simulated(op)) continue;
    slot += ISA_PACK_Inst_Words(OP_code(op));
    if (slot >= ISA_MAX_SLOTS) slot = 0;
  }
#endif
  return;
}

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
    /*
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
    */

  } else if (type & THR_DATA_SPECULATION_NO_RB_CLEANUP) {
    /*
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
    */
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

#if 0
  if ( OP_defs_fp(op) ) {
    INT32 reg_write_cyc;
    reg_write_cyc = TI_LATENCY_Result_Available_Cycle(OP_code(op), 0 /*???*/)
		    + (OP_load(op) ? 0 : 1);

    if ( is_loop )
      reg_write_cyc = Mod(reg_write_cyc + cycle + offset,cycle_count);
    else {
      reg_write_cyc += cycle + offset;

      if ( reg_write_cyc >= cycle_count + assumed_longest_latency ) {
        /* Our assumed longest latency was too low, it seems */
        Increase_Assumed_Longest_Latency(1 + reg_write_cyc - cycle_count);
      }
    }

    for ( l = writing[reg_write_cyc]; l != NULL; l = VOID_LIST_rest(l) )
      make_interference(lrange,VOID_LIST_first(l));

    writing[reg_write_cyc] = VOID_LIST_Push(lrange,writing[reg_write_cyc],
                                            &interference_pool);
  }
#endif
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
    ratio[0] = 6;
    break;
  case PRC_MADD:
  case PRC_MEMREF:
    ratio[0] = 2;
    break;
  case PRC_FLOP:
  case PRC_FADD:
  case PRC_FMUL:
    ratio[0] = 2;
    break;
  case PRC_IOP:
    ratio[0] = 4;
    break;
  default:
    ratio[0] = 2;
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

  bzero (info, sizeof (PRC_INFO));

  for ( op = BB_first_op(bb); op != NULL; op = OP_next(op) ) {
    INT num_insts = OP_Real_Ops (op);

    if (num_insts == 0) continue;

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

  /* Initialize CGTARG_Inter_RegClass_Copy_Table: */
  for (i = 0; i <= ISA_REGISTER_CLASS_MAX; ++i) {
    INT j;
    for (j = 0; j <= ISA_REGISTER_CLASS_MAX; ++j) {
      CGTARG_Inter_RegClass_Copy_Table[i][j][FALSE] = TOP_UNDEFINED;
      CGTARG_Inter_RegClass_Copy_Table[i][j][TRUE] = TOP_UNDEFINED;
    }
  }

  /* Init all table entries to TOP_UNDEFINED.
   */
  for(i = 0; i <= TOP_count; ++i) {
    CGTARG_Invert_Table[i] = TOP_UNDEFINED;
  }

  /* Init table for CGTARG_Invert:
   */

  return;
}
