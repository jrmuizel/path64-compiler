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
#include "targ_isa_lits.h"
#include "targ_isa_registers.h"
#include "w2op.h"
#include "targ_proc_properties.h"
#include "targ_isa_enums.h"
#include "cgtarget.h"
#include "calls.h"
#include "cgexp.h"


UINT32 CGTARG_branch_taken_penalty;
BOOL CGTARG_branch_taken_penalty_overridden = FALSE;

mTOP CGTARG_Inter_RegClass_Copy_Table[ISA_REGISTER_CLASS_MAX+1][ISA_REGISTER_CLASS_MAX+1][2];

TOP CGTARG_Invert_Table[TOP_count+1];

/* ====================================================================
 *   CGTARG_Preg_Register_And_Class
 *
 *   See interface description
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

/* ====================================================================
 *   CGTARG_Mem_Ref_Bytes
 *
 *   Requires: OP_load(memop) || OP_store(memop)
 * ====================================================================
 */
UINT32 
CGTARG_Mem_Ref_Bytes (
  const OP *memop
)
{
  FmtAssert(OP_load(memop) || OP_store(memop), ("not a load or store"));

  switch (OP_code(memop)) {
  default:
    FmtAssert(FALSE, ("unrecognized op (%s) in CGTARG_Mem_Ref_Bytes",
		      TOP_Name(OP_code(memop))));
  }
  /*NOTREACHED*/
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
 * ====================================================================
 */
TY_IDX 
CGTARG_Spill_Type (TN *tn) 
{
  switch (TN_register_class(tn)) {
    case ISA_REGISTER_CLASS_guard:
    case ISA_REGISTER_CLASS_du:
      return Spill_Int_Type;

    case ISA_REGISTER_CLASS_au:
      return Spill_Ptr_Type;

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
 *   CGTARG_Copy_Operand
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
    if (TN_has_value(OP_opnd(op,2)) && TN_value(OP_opnd(op,2)) == 0) {
      return 1;
    }
    break;

    /*
  case TOP_add:
  case TOP_or:
  case TOP_xor:
    if (TN_register_and_class(OP_opnd(op,2)) == CLASS_AND_REG_zero) {
      return 1;
    }
    if (TN_register_and_class(OP_opnd(op,1)) == CLASS_AND_REG_zero) {
      return 2;
    }
    break;

  case TOP_andcm:
  case TOP_sub:
    if (TN_register_and_class(OP_opnd(op,2)) == CLASS_AND_REG_zero) {
      return 1;
    }
    break;

  case TOP_and_i:
    if (TN_has_value(OP_opnd(op,1)) && TN_value(OP_opnd(op,1)) == -1) {
      return 2;
    }
    break;

  case TOP_extr:
  case TOP_extr_u:
    if (   TN_has_value(OP_opnd(op,2)) && TN_value(OP_opnd(op,2)) == 0
	&& TN_has_value(OP_opnd(op,3)) && TN_value(OP_opnd(op,3)) == 64)
    {
      return 1;
    }
    break;

  case TOP_shl_i:
  case TOP_shr_i:
  case TOP_shr_i_u: 
    if (   (TN_register_and_class(OP_opnd(op,1)) == CLASS_AND_REG_zero)
	|| (TN_has_value(OP_opnd(op,2)) && TN_value(OP_opnd(op,2)) == 0))
    {
      return 1;
    }
    break;

  case TOP_shl:
  case TOP_shr: 
  case TOP_shr_u: 
    if (   (TN_register_and_class(OP_opnd(op,1)) == CLASS_AND_REG_zero)
	|| (TN_register_and_class(OP_opnd(op,2)) == CLASS_AND_REG_zero))
    {
      return 1;
    }
    break;

  case TOP_mov:
  case TOP_mov_f:
  case TOP_copy_br:
	return 1;
    */

  }
  return -1;
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

  FmtAssert(FALSE,("CGTARG_Adjust_Latency: not implemented"));
}

/* ====================================================================
 * ====================================================================
 *                 Initialization / Termination
 * ====================================================================
 * ====================================================================
 */

/* ====================================================================
 *   CGTARG_Initialize
 *
 *   See interface description
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
