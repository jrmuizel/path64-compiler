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
#include "lai.h"
#include "void_list.h"
#include "whirl2ops.h"
#include "targ_isa_lits.h"
#include "targ_isa_registers.h"
#include "w2op.h"
#include "targ_proc_properties.h"
#include "targ_isa_enums.h"
#include "cgtarget.h"
#include "expand.h"



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
 *   CGTARG_Unconditional_Compare
 *
 *   See interface description
 * ====================================================================
 */
static BOOL 
CGTARG_Unconditional_Compare_Helper (
  TOP top, 
  TOP* uncond_ver
)
{
  *uncond_ver = TOP_UNDEFINED;

  return FALSE;
}

/* ====================================================================
 *    CGTARG_Parallel_Compare
 *
 *    See interface description
 *
 *    None for floats. For gp registers, only support "eq" and "ne"
 *    for arbitrary register compares.  Others must have one operand
 *    be the zero register.
 * ====================================================================
 */
TOP
CGTARG_Parallel_Compare (
  OP* cmp_op, 
  COMPARE_TYPE ctype
)
{
  TOP cmp_top = OP_code(cmp_op);

  if (OP_opnd(cmp_op, 1) != Zero_TN && OP_opnd(cmp_op, 2) != Zero_TN) {
    switch (cmp_top) {

      default:
	return TOP_UNDEFINED;
    }
  }

  switch (cmp_top) {

    default:
      return TOP_UNDEFINED;
  }

  return TOP_UNDEFINED;
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
