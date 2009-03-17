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
#include "whirl2ops.h"

#include "cg.h"
#include "opt_alias_mgr.h"

// Reconfigurability
#include "lai_loader_api.h"

#include "cgtarget.h"

#include "exp_targ.h"

/* [JV] Defined in exp_targ.cxx */
extern ISA_LIT_CLASS Get_Lit_Clas_For_Memory_Access_Size( INT32 size, BOOL is_signed );

/* ====================================================================
 *   OP_is_ext_op
 * ====================================================================
 */
BOOL OP_is_ext_op(OP *op) 
{
  return FALSE;
}

/* ====================================================================
 *   OP_Is_Barrier
 *
 * ====================================================================
 */
BOOL OP_Is_Barrier(OP *op) 
{
  TOP top = OP_code(op);

  if (top == TOP_asm) {
    ASM_OP_ANNOT* asm_info = (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op);
    if (WN_Asm_Clobbers_Mem(ASM_OP_wn(asm_info)))
      return TRUE;
  }

  if (TOP_is_barrier(top)) {
    return TRUE;
  }

  return FALSE;
}

/* ====================================================================
 *   OP_is_counted_loop
 * ====================================================================
 */
BOOL 
OP_Is_Counted_Loop(OP *op) 
{
  return FALSE;
}

/* ====================================================================
 *   OP_Is_Copy_Immediate_Into_Register
 * ====================================================================
 */
BOOL 
OP_Is_Copy_Immediate_Into_Register(OP *op) {
  return (OP_Is_Copy(op) && TN_has_value(OP_Copy_Operand_TN(op)));
}                                                                                
/* ====================================================================
 *   OP_Has_Latency
 * ====================================================================
 */
BOOL 
OP_Has_Latency(OP *op) 
{
  return (CGTARG_Max_OP_Latency (op)>1);
}

/* ====================================================================
 *   OP_Is_Advanced_Load
 * ====================================================================
 */
BOOL
OP_Is_Advanced_Load( OP *memop )
{
  // no advanced loads for st200
  return FALSE;
}

/* ====================================================================
 *   OP_Is_Speculative_Load
 * ====================================================================
 */
BOOL
OP_Is_Speculative_Load ( 
  OP *memop 
)
{
  if (!OP_load(memop)) return FALSE;

  return FALSE;
}

/* ====================================================================
 *   OP_Is_Check_Load
 * ====================================================================
 */
BOOL
OP_Is_Check_Load ( 
  OP *memop 
)
{
  if (!OP_load(memop)) return FALSE;

  return FALSE;
}


/* ====================================================================
 *   OP_Is_Speculative
 * ====================================================================
 */
BOOL
OP_Is_Speculative (
  OP *op
)
{
  if (!OP_load(op)) return FALSE;

  // speculative and advanced loads are safe to speculate.
  if (OP_Is_Advanced_Load(op) || OP_Is_Speculative_Load(op))
    return TRUE;

  return FALSE;
}

/* ====================================================================
 *   OP_Performance_Effects
 *
 *   True if the op has performance effects that make its
 *   speculation undesirable.
 *   False otherwise.
 * ====================================================================
 */
BOOL OP_Performance_Effects (
  OP *op
)
{
  TOP opcode = OP_code(op);

  return FALSE;
}

/* ====================================================================
 *   OP_Safe_Effects
 *
 *   True if the op normally has performance effects that make its
 *   speculation undesirable, but these should be ignored in this case.
 *   False otherwise.
 * ====================================================================
 */
BOOL OP_Safe_Effects (
  OP *op
)
{
  TOP opcode = OP_code(op);

  return FALSE;
}

/* ====================================================================
 *   OP_Can_Be_Speculative
 *
 *   determines if the TOP can be speculatively executed taking 
 *   into account eagerness level
 * ====================================================================
 */
BOOL OP_Can_Be_Speculative (
  OP *op
)
{
  TOP opcode = OP_code(op);

  // can never speculate a call.
  if (OP_call (op))
    return FALSE;

  // [CG] Not clear if we can speculate an asm
  if (OP_code(op) == TOP_asm) return FALSE;

  if (OP_Is_Barrier(op)) return FALSE;

  switch (Eager_Level) {
   case EAGER_NONE:

     /* not allowed to speculate anything
     */
     return FALSE;
     break;

  case EAGER_SAFE:

    /* Only exception-safe speculative ops are allowed
     */
#if 0
    if (TOP_is_ftrap(opcode) || TOP_is_itrap(opcode)) return FALSE;
#endif
    /*FALLTHROUGH*/

  case EAGER_ARITH:

    /* Arithmetic exceptions allowed
     */
    if (OP_fdiv(op)) return FALSE;
    /*FALLTHROUGH*/

  case EAGER_DIVIDE:

    /* Divide by zero exceptions allowed 
     */
#if 0
    if (TOP_is_memtrap(opcode)) return FALSE;
#endif
    /*FALLTHROUGH*/

  case EAGER_MEMORY:

    /* Memory exceptions allowed / All speculative ops allowed
     */
    if (TOP_is_unsafe(opcode)) return FALSE;
    break;

  default:
    DevWarn("unhandled eagerness level: %d", Eager_Level);
    return FALSE;
  }

  if (!OP_memory (op)) return TRUE;

  /* prefetch are speculative. */
  if (OP_prefetch (op)) return TRUE;

  /* This is a memory reference */

  /* don't speculate volatile memory references. */
  if (OP_volatile(op)) return FALSE;

  if (!OP_load(op)) return FALSE;

  /* we can't speculate a load unless it is marked as dismissable */
  /* it is the client's responsability to do that. */
  if (OP_Is_Speculative(op)) return TRUE;
  
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
 *   CGTARG_Predicate_OP
 * ====================================================================
 */
void
CGTARG_Predicate_OP (
  BB* bb, 
  OP* op, 
  TN* pred_tn,
  bool on_false
)
{
  /* warning bb is not necessarily available when calling
     CGTARG_Predicate_OP().
     in this case, bb= NULL !!!
  */

  if(pred_tn == NULL) { return; }

  if (OP_has_predicate(op)) {
    Set_OP_opnd(op, OP_find_opnd_use(op,OU_predicate), pred_tn);
    /* copy flag not allowed on predicated operations */
    if (OP_copy(op) && (! TN_is_true_pred(pred_tn))) {
      Reset_OP_copy(op);
    }
    if (on_false) {
      FmtAssert(pred_tn != True_TN,("CGTARG_Predicate_OP: invert True TN ?"));
      Set_OP_Pred_False(op, OP_find_opnd_use(op, OU_predicate)); 
    }
    else 
      Set_OP_Pred_True(op, OP_find_opnd_use(op, OU_predicate)); 
  }
  else {
    fPrint_TN(stderr, "pred_tn = %s", pred_tn);
    FmtAssert(FALSE,("CGTARG_Predicate_OP: cannot predicate this op (%s)",TOP_Name(OP_code(op))));
  }
}

/* ====================================================================
 *   CGTARG_Dup_OP_Predicate
 *
 *     Duplicate OP and give the copy the predicate NEW_PRED.
 *     Note that OP may be unpredicated, in which case we need to
 *     change it to the predicated form.
 *     Note that NEW_PRED may be True_TN, in which case we should
 *     change OP to the unpredicated form.
 * ====================================================================
 */
OP *
CGTARG_Dup_OP_Predicate (OP *op, TN *new_pred)
{
  OP *result = Dup_OP (op);
  Set_OP_opnd (result, OP_find_opnd_use(result, OU_predicate), new_pred);
  return result;
}

/* ====================================================================
 *   OP_Copy_Operand
 *
 *   Test and return operand for copy operations.
 *   Return the operand index if the operation is a copy from an
 *   immediate value or a register to a register of the same class.
 *   Returns -1 if the operation is not a copy.
 *   This function returns -1 also if the copy is predicated.
 * ====================================================================
 */
INT 
OP_Copy_Operand (
  OP *op
)
{
  TOP opcode = OP_code(op);
  INT pred_idx = OP_find_opnd_use(op, OU_predicate);

  if (opcode == TOP_spadjust) {
    return -1;
  }

  if (!OP_cond_def(op)) {
    if (OP_iadd(op) || OP_ior(op) || OP_ixor(op)) {
      INT opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
      INT opnd2_idx = OP_find_opnd_use(op, OU_opnd2);
      
      if (TN_has_value(OP_opnd(op,opnd2_idx)) && TN_value(OP_opnd(op,opnd2_idx)) == 0) {
	return opnd1_idx;
      }
      if (TN_has_value(OP_opnd(op,opnd1_idx)) && TN_value(OP_opnd(op,opnd1_idx)) == 0) {
	return opnd2_idx;
      }
    }
    
    if (OP_iand(op)) {
      INT opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
      INT opnd2_idx = OP_find_opnd_use(op, OU_opnd2);
      if ((TN_has_value(OP_opnd(op,opnd2_idx)) && TN_value(OP_opnd(op,opnd2_idx)) == ~0)) {
	return opnd1_idx;
      }
      if ((TN_has_value(OP_opnd(op,opnd1_idx)) && TN_value(OP_opnd(op,opnd1_idx)) == ~0)) {
	return opnd2_idx;
      }
      if (OP_opnd(op,opnd2_idx) == True_TN) return opnd1_idx;
      if (OP_opnd(op,opnd1_idx) == True_TN) return opnd2_idx;
    }
    
    if (TOP_is_move(opcode)) {
      return OP_find_opnd_use(op, OU_opnd1);
    }

#if 0
    if (TOP_is_xmove(opcode)) {
      return OP_find_opnd_use(op, OU_opnd1);
    }
    
#endif
    if (opcode == TOP_MAKE) {
      /* Returns the immediate that is copied. */
      return 0;
    }
  }

  /* Ensure that if OP_copy() is true we return a valid operand. Otherwise
     there is a mismatch. */
  FmtAssert(!OP_copy(op), ("operation marked copy, but operand not found on %s",TOP_Name(OP_code(op))));

  return -1;
}

/* ====================================================================
 *   OP_Copy_Result
 *
 *   Returns the result operand index for operations
 *   that have a defined OP_Copy_Operand.
 * ====================================================================
 */
INT 
OP_Copy_Result (
  OP *op
)
{
  return 0;
}

/* ====================================================================
 *   CGTARG_Noop_Top
 * ====================================================================
 */
TOP 
CGTARG_Noop_Top (ISA_EXEC_UNIT_PROPERTY unit) { return TOP_nop; } 

/* ====================================================================
 *   OP_save_predicates/OP_restore_predicates
 * ====================================================================
 */
BOOL OP_save_predicates(OP *op) {
  
 return FALSE; 
}

BOOL OP_restore_predicates(OP *op) {
  return FALSE;
}

/* ====================================================================
 *   OP_is_associative
 *
 *   The list of TOPs that will be handled by the reassociation algorithm.
 * ====================================================================
 */
BOOL
OP_is_associative(OP *op)
{
  TOP top = OP_code(op);

  switch (top) {
  case TOP_add_r_r_npc:
  case TOP_armv5e_add_r_r_npc:
  case TOP_sub_r_r_npc:
  case TOP_armv5e_sub_r_r_npc:
    return true;
  }

  return false;
}

/* ====================================================================
 *   TOP_opposite
 *
 *   Give the opposite form, e.g,  - => +,  + => -.
 *
 *   TODO: belongs to the targ_info.
 * ====================================================================
 */
TOP 
TOP_opposite(TOP top)
{
  FmtAssert(FALSE,("Not implemented"));
#if 0
  switch (top) {
  case TOP_add_r:
    return TOP_sub_r;

  case TOP_sub_r:
    return TOP_add_r;
  }
#endif
  return TOP_UNDEFINED;
}

/* ====================================================================
 *   TOP_immediate
 *
 *   Give the immediate form.
 *
 *   TODO: belongs to the targ_info.
 * ====================================================================
 */
TOP 
TOP_immediate(TOP top)
{
  FmtAssert(FALSE,("Not implemented"));
#if 0
  switch (top) {
  case TOP_add_r:
    return TOP_add_i8;

  case TOP_sub_r:
    return TOP_sub_i8;
  }

#endif
  return TOP_UNDEFINED;
}


/* ====================================================================
 *   TOP_equiv_nonindex_memory
 *
 *   TODO: belongs to the targ_info.
 * ====================================================================
 */
TOP 
TOP_equiv_nonindex_memory(TOP top)
{
  return TOP_UNDEFINED;
}

/* ====================================================================
 *   CGTARG_Which_OP_Select
 * ====================================================================
 */
TOP
CGTARG_Which_OP_Select ( 
  UINT16 bit_size, 
  BOOL is_float, 
  BOOL is_fcc 
)
{
  return TOP_UNDEFINED;
}

/*
 * STxP70 instructions that read/write carry bit (SR.C).
 * The instructions that access it are:
 * add, addc, addu, addcu,sub,subc, subu, subcu
 * We restrict the code generator to never use add[u],sub[u] for
 * generating a useful flag.
 * In this case we only have to handle:
 * - output dependencies between addc <-> add
 * - flow dependencies between addc -> addc
 * - anti dependencies between addc -> add (redundant with output deps)
 * I.e. the output dependency between add or sub can be ignored.
 */

OP_Flag_Effects
CGTARG_OP_Get_Flag_Effects(const OP *op)
{

#if 0

  TOP opc = OP_code(op);
  switch (opc) {
  case TOP_addu_r:
  case TOP_addu_i8:
  case TOP_add_r:
  case TOP_add_i8:
  case TOP_subu_r:
  case TOP_subu_i8:
  case TOP_sub_r:
  case TOP_sub_i8:
    return OP_FE_CLOBBER;
  case TOP_addcu:
  case TOP_addc:
  case TOP_subc:
  case TOP_subcu:
    return (OP_Flag_Effects)(OP_FE_WRITE|OP_FE_READ);
  }

#endif
  return OP_FE_NONE;
}


/* ====================================================================
 *   OP_opnd_can_be_reassociated
 *
 *   Test whether the OPND can be reassociated with the OP.
 * ====================================================================
 */
BOOL
OP_opnd_can_be_reassociated (
  OP *op, 
  INT opnd
)
{
  TOP top = OP_code(op);

  switch (top) {
  case TOP_add_r_r_npc:
    return (opnd == 0 || opnd == 1);

  case TOP_sub_r_r_npc:
    return (opnd == 0);
  default:
    FmtAssert(FALSE,("TOP '%s' not treated",TOP_Name(OP_code(op))));
  }

  return false;
}

/* ====================================================================
 *   OP_other_opnd
 *
 *   The other opnd involved in reassociation
 * ====================================================================
 */
INT 
OP_other_opnd(OP *op, INT this_opnd)
{
  FmtAssert(FALSE,("Not implemented"));
#if 0
  switch (OP_code(op)) {
  case TOP_add_r:
  case TOP_sub_r:
    if (this_opnd == 2) return 1;
    if (this_opnd == 1) return 2;
    break;
  }
  Is_True(FALSE, ("Other_opnd: wrong opnd num"));
#endif
  return 0;
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

  default:
    if (OP_has_predicate(op))
      Set_OP_cond_def_kind(op, OP_PREDICATED_DEF);
    else
      Set_OP_cond_def_kind(op, OP_ALWAYS_UNC_DEF);
    break;
  }
}

/* =====================================================================
 *   OP_Is_Unconditional_Compare
 * =====================================================================
 */
BOOL
OP_Is_Unconditional_Compare (
  OP *op
)
{
  return (FALSE);
}


/*
 * TOP_opnd_immediate_variant_default
 *
 * Default target independent implementation for
 * TOP_opnd_immediate_variant.
 * The implementation is driven by the targinfo variant 
 * description (see isa_variant.cxx).
 * VARATT_immediate gives for any register or immediate operator the first immediate form.
 * VARATT_next_immediate gives the next larger immediate form.
 * Note that this model works only when 1 operand varies for the operator.
 * The matching test is done on the literal class for the given operand.
 */
static TOP
TOP_opnd_immediate_variant_default(TOP regform, int opnd, INT64 imm)
{
  TOP immform;

  // VCDV - bug #63273. avoid immediate variant for var-arg TOPs like top_asm
  if (opnd>=ISA_OPERAND_INFO_Operands(ISA_OPERAND_Info(regform))) {
    return TOP_UNDEFINED;
  }

  // Check if it is already an immediate form
  if (ISA_SUBSET_LIST_Member (ISA_SUBSET_List, regform)) {
    const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info (regform);
    const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
    if (ISA_OPERAND_VALTYP_Is_Literal(otype)) {
      immform = regform;
      // Get to the first immediate variant.
      TOP prevform = TOP_get_variant(immform, VARATT_prev_immediate);
      while (prevform != TOP_UNDEFINED && ISA_SUBSET_LIST_Member (ISA_SUBSET_List, prevform)) {
	immform = prevform;
	prevform = TOP_get_variant(prevform, VARATT_prev_immediate);
      }
    } else {
      immform = TOP_get_variant(regform, VARATT_immediate);
    }
  }  
  
  // Get the first immediate form that fits the operand
  while (immform != TOP_UNDEFINED && ISA_SUBSET_LIST_Member (ISA_SUBSET_List, immform)) {
    const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info (immform);
    const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
    if (ISA_OPERAND_VALTYP_Is_Literal(otype)) {
      ISA_LIT_CLASS lit_class = ISA_OPERAND_VALTYP_Literal_Class(otype);
      if (ISA_LC_Value_In_Class (imm, lit_class))
	break;
    }
    immform = TOP_get_variant(immform, VARATT_next_immediate);

  }
  return immform;
}

/*
 * TOP_opnd_immediate_variant
 *
 * Returns the TOP immediate variant, depending on the immediate value.
 * Target dependent.
 * The reg form may be a register or immediate form opcode.
 * opnd is the operand number that may be replaced (0..2).
 * imm is the immediate value that should be encoded.
 * Returns TOP_UNDEFINED, if no immediate variant is available.
 */
TOP
TOP_opnd_immediate_variant(TOP regform, int opnd, INT64 imm)
{
  // No target dependent specificities.
  return TOP_opnd_immediate_variant_default(regform, opnd, imm);
}


/*
 * TOP_opnd_register_variant_default
 *
 * Default target independent implementation for
 * TOP_opend_register_variant.
 * The implementation is driven by the targinfo variant 
 * description (see isa_variant.cxx).
 * VARATT_immediate gives for any register or immediate operator the first immediate form.
 * VARATT_next_immediate gives the next larger immediate form.
 * We use these two variants to generate the reverse mapping which
 * gives for each top immediate or immediate next the initial register variant.
 */
extern TOP CGTARG_TOP_Register_Variant(TOP immform_top); /* Defined in targ_cg.cxx. */
static TOP
TOP_opnd_register_variant_default(TOP immform, int opnd, ISA_REGISTER_CLASS regclass)
{
  TOP regform;
 
  // Check if already a register variant.
  if (ISA_SUBSET_LIST_Member (ISA_SUBSET_List, immform)) {
    const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info (immform);
    const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
    if (ISA_OPERAND_VALTYP_Is_Register(otype) &&
	regclass == ISA_OPERAND_VALTYP_Register_Class(otype))
      return immform;
  }
  regform = CGTARG_TOP_Register_Variant(immform);
  
  if (regform != TOP_UNDEFINED && ISA_SUBSET_LIST_Member (ISA_SUBSET_List, regform)) {
    const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info (regform);
    const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);
    if (ISA_OPERAND_VALTYP_Is_Register(otype)) {
      if (regclass == ISA_OPERAND_VALTYP_Register_Class(otype))
	return regform;
    }
  }
  return TOP_UNDEFINED;
}

/*
 * TOP_opnd_register_variant
 *
 * Returns the TOP register variant, matching the given register class.
 * Target dependent.
 * opnd is the operand number that may be replaced (0..2).
 * regclass is the requested register class.
 * Returns TOP_UNDEFINED, if no register variant is available.
 */
TOP
TOP_opnd_register_variant(TOP regform, int opnd, ISA_REGISTER_CLASS regclass)
{
  // No target dependent specificities.
  return TOP_opnd_register_variant_default(regform, opnd, regclass);
}

/*
 * TOP_opnd_swapped_variant
 * Returns the TOP corresponding to an invertion of the 2 operands index.
 * For commutative tops on the index, return the same top.
 * For inversible tops, return the inversed top.
 */
TOP
OP_opnd_swapped_variant(OP *op, int opnd1, int opnd2)
{

#define CASE_TOP_R_R_NPC(top) case TOP_##top##_r_r_npc: \
  return TOP_##top##_r_r_npc;
#define CASE_TOP_R_R(top) case TOP_##top##_r_r: \
  return TOP_##top##_r_r;
#define CASE_TOP_INV(top,newtop) case TOP_##top##_r_r_npc: \
  return TOP_##newtop##_r_r_npc;
#define CASE_TOP_UNDEF(top) case TOP_##top: \
  return TOP_UNDEFINED;

  TOP top = OP_code(op);

  if (opnd1 > opnd2) {
    int tmp = opnd1;
    opnd1 = opnd2;
    opnd2 = tmp;
  }

  if(opnd1 == 0 && opnd2 == 1) {
    switch(top) {
      CASE_TOP_R_R_NPC(add);
      CASE_TOP_R_R_NPC(and);
      CASE_TOP_R_R_NPC(eor);
      CASE_TOP_R_R_NPC(orr);
      CASE_TOP_UNDEF(cmp_r_r);
      CASE_TOP_INV(sub,rsb);
      CASE_TOP_INV(rsb,sub);
      CASE_TOP_UNDEF(mov_npc_lsl_npc_npc);
      CASE_TOP_UNDEF(mov_npc_lsr_npc_npc);
      CASE_TOP_UNDEF(mov_npc_asr_npc_npc);
#if 0
      CASE_TOP(add);
      CASE_TOP(addu);
      CASE_TOP(and);
      CASE_TOP(or);
      CASE_TOP(xor);
      CASE_TOP(max);
      CASE_TOP(maxu);
      CASE_TOP(min);
      CASE_TOP(minu);
      CASE_TOP(cmpeq);
      CASE_TOP(cmpne);
      CASE_TOP_INV(cmpge,cmple);
      CASE_TOP_INV(cmpgeu,cmpleu);
      CASE_TOP_INV(cmpgt,cmplt);
      CASE_TOP_INV(cmpgtu,cmpltu);
      CASE_TOP_INV(cmple,cmpge);
      CASE_TOP_INV(cmpleu,cmpgeu);
      CASE_TOP_INV(cmplt,cmpgt);
      CASE_TOP_INV(cmpltu,cmpgtu);
#endif
    }
  }
#ifdef Is_True_On
  FmtAssert(FALSE,("Not managed top %s",TOP_Name(top)));
#endif

  return TOP_UNDEFINED;
#undef CASE_TOP_R_R_NPC
#undef CASE_TOP_R_R
#undef CASE_TOP_INV
#undef CASE_TOP_UNDEF

}

/* TOP_Find_Shift_Variant
   Returns variant with shifted operand.
*/
TOP
TOP_Combine_Opcodes ( TOP opcode1, TOP opcode2, INT opnd_idx, BOOL *opnd_swapped ) {

#define CASE_R_R_NPC(topcode)				\
  case TOP_##topcode##_r_r_npc:				\
    if(opnd_idx == 0) { *opnd_swapped = TRUE; }		\
    switch(kind) {					\
      case shift_left:					\
      top = TOP_##topcode##_i5_lsl_r_r_npc;		\
      break;						\
    case shift_aright:					\
      top = TOP_##topcode##_i5_asr_r_r_npc;		\
      break;						\
    case shift_lright:					\
      top = TOP_##topcode##_i5_lsr_r_r_npc;		\
      break;						\
    default:						\
      FmtAssert(FALSE,("Unknown kind of shift"));	\
    }							\
    break;

#define CASE_R_R(topcode)				\
  case TOP_##topcode##_r_r:				\
    if(opnd_idx == 0) { *opnd_swapped = TRUE; }		\
    switch(kind) {					\
    case shift_left:					\
      top = TOP_##topcode##_i5_lsl_r_r;			\
      break;						\
    case shift_aright:					\
      top = TOP_##topcode##_i5_asr_r_r;			\
      break;						\
    case shift_lright:					\
      top = TOP_##topcode##_i5_lsr_r_r;			\
      break;						\
    default:						\
      FmtAssert(FALSE,("Unknown kind of shift"));	\
    }							\
    break;


  TOP top = TOP_UNDEFINED;
  SHIFT_DIRECTION kind;

  *opnd_swapped = FALSE;

  if(TOP_is_shl(opcode2)) { kind = shift_left; }
  else if(TOP_is_shr(opcode2)) { kind = shift_aright; }
  else if(TOP_is_shru(opcode2)) { kind = shift_lright; }

  switch(opcode1) {
  case TOP_mov_i5_asr_r_npc:
  case TOP_mov_npc_asr_npc_npc:
  case TOP_mov_i5_lsr_r_npc:
  case TOP_mov_npc_lsr_npc_npc:
  case TOP_mov_i5_lsl_r_npc:
  case TOP_mov_npc_lsl_npc_npc:
    /* Unable to combine several shifts. */
    break;
  case TOP_mul:
    /* Unable to combine multiply and shift. */
    break;
  case TOP_cmp_i8_r:
  case TOP_rsb_i8_r_npc:
  case TOP_sub_i8_r_npc:
  case TOP_add_i8_r_npc:
  case TOP_and_i8_r_npc:
  case TOP_orr_i8_r_npc:
  case TOP_eor_i8_r_npc:
    /* In this case shift is applied on OU_opnd1. We haven't any 'shifter_operand'
     * with shifted immediate (only applied on register).
     */
    break;
  CASE_R_R_NPC(add);
  CASE_R_R_NPC(sub);				
  CASE_R_R_NPC(rsb);				
  CASE_R_R_NPC(and);				
  CASE_R_R_NPC(eor);				
  CASE_R_R_NPC(orr);				
  CASE_R_R(cmp);
  default:
    FmtAssert(FALSE,("Not combined top %s and top %s",TOP_Name(opcode1),TOP_Name(opcode2)));
  }

  return top;

#undef CASE_R_R_NPC
#undef CASE_R_R
}

/*
 * TOP_result_register_variant
 * Returns the TOP variant for generating a result in the given register
 * class with the same operand types.
 * The reg form may be a register or immediate opcode.
 * rslt is the result number that may be replaced (0..1).
 */
TOP
TOP_result_register_variant(TOP regform, int rslt, ISA_REGISTER_CLASS regclass)
{

  return TOP_UNDEFINED;
}

/*
 * TOP_opnd_use_bits
 * Return the effective bits used for the given operand.
 * In case of immediate operand, returns the used bits after the
 * optional sign extension.
 * Return -1 for undefined semantic
 */
INT
TOP_opnd_use_bits(TOP top, int opnd)
{
  int use_bits;
  const ISA_OPERAND_INFO *oinfo;
  const ISA_OPERAND_VALTYP *vtype;
  
  // Default cases depend on register class.
  // ISA_REGISTER_CLASS_integer defaults to 32 bits, signed
  // Non registers default to 32 bit signed
  oinfo = ISA_OPERAND_Info(top);
  vtype = ISA_OPERAND_INFO_Operand(oinfo, opnd);

  use_bits = ISA_OPERAND_VALTYP_Size(vtype);
  if (TOP_is_store(top) && opnd == TOP_Find_Operand_Use(top,OU_storeval)) {
    use_bits = TOP_Mem_Bytes(top)*8;
  }
    
  // The cases below 
  switch(top) {
  case TOP_sxth_npc_npc:
  case TOP_sxth_i2_ror_npc_npc:
  case TOP_sxth_npc_npc_cond:
  case TOP_sxth_i2_ror_npc_npc_cond:
  case TOP_uxth_npc_npc:
  case TOP_uxth_i2_ror_npc_npc:
  case TOP_uxth_npc_npc_cond:
  case TOP_uxth_i2_ror_npc_npc_cond:
    if(opnd == TOP_Find_Operand_Use(top,OU_opnd1)) { use_bits = 16; };
    break;
  case TOP_uxtb_npc_npc:
  case TOP_uxtb_i2_ror_npc_npc:
  case TOP_uxtb_npc_npc_cond:
  case TOP_uxtb_i2_ror_npc_npc_cond:
  case TOP_sxtb_npc_npc:
  case TOP_sxtb_i2_ror_npc_npc:
  case TOP_sxtb_npc_npc_cond:
  case TOP_sxtb_i2_ror_npc_npc_cond:
    if(opnd == TOP_Find_Operand_Use(top,OU_opnd1)) { use_bits = 8; };
    break;

  case TOP_mov_i5_lsl_r_npc:
  case TOP_mov_npc_lsl_npc_npc:
  case TOP_mov_i5_asr_r_npc:
  case TOP_mov_npc_asr_npc_npc:
  case TOP_mov_i5_lsr_r_npc:
  case TOP_mov_npc_lsr_npc_npc:
    if(opnd == TOP_Find_Operand_Use(top,OU_opnd2)) { use_bits = 5; };
    break;
  }
  
  return use_bits;
}

/*
 * TOP_opnd_use_signed
 * Returns true if the extension os the effective use bits is
 * signed for the semantic of the TOP.
 * For instance a 32x16->32 unsigned multiply should have the following 
 * properties:
 * TOP_opnd_use_bits(top, opnd1) == 32
 * TOP_opnd_use_signed(top, opnd1) == FALSE
 * TOP_opnd_use_bits(top, opnd2) == 16
 * TOP_opnd_use_signed(top, opnd2) == FALSE
 *
 * Default is to return -1 for undefined semantic.
 */
INT
TOP_opnd_use_signed(TOP top, int opnd)
{
  INT is_signed;
  const ISA_OPERAND_INFO *oinfo;
  const ISA_OPERAND_VALTYP *vtype;
  ISA_REGISTER_CLASS rc;
  
  // Default cases depend on operand value type.
  oinfo = ISA_OPERAND_Info(top);
  vtype = ISA_OPERAND_INFO_Operand(oinfo, opnd);

  is_signed =  ISA_OPERAND_VALTYP_Is_Signed(vtype);

  switch(top) {
  case TOP_uxth_npc_npc:
  case TOP_uxth_i2_ror_npc_npc:
  case TOP_uxth_npc_npc_cond:
  case TOP_uxth_i2_ror_npc_npc_cond:
  case TOP_uxtb_npc_npc:
  case TOP_uxtb_i2_ror_npc_npc:
  case TOP_uxtb_npc_npc_cond:
  case TOP_uxtb_i2_ror_npc_npc_cond:
    if(opnd == TOP_Find_Operand_Use(top,OU_opnd1)) { is_signed = FALSE; };
    break;

  case TOP_mov_i5_lsl_r_npc:
  case TOP_mov_npc_lsl_npc_npc:
  case TOP_mov_i5_asr_r_npc:
  case TOP_mov_npc_asr_npc_npc:
    // Shift amount is interpreted unsigned
    if(opnd == TOP_Find_Operand_Use(top,OU_opnd2)) { is_signed = FALSE; };
    break;
  case TOP_mov_i5_lsr_r_npc:
  case TOP_mov_npc_lsr_npc_npc:
    // Shift amount and shifted value are interpreted unsigned
    if(opnd == TOP_Find_Operand_Use(top,OU_opnd2) ||
       opnd == TOP_Find_Operand_Use(top,OU_opnd1)) { is_signed = FALSE; };
    break;
  default:
    break;
  }

  return is_signed;
}

TOP
TOP_AM_automod_variant(TOP top, BOOL post_mod, BOOL inc_mod, ISA_REGISTER_CLASS regclass) {

#define TOP_AM_i13_r_w(x)						\
  case TOP_##x##_i13_r_npc:						\
    if(regclass == ISA_REGISTER_CLASS_UNDEFINED) {			\
      top_am_automod = post_mod ? TOP_##x##_i13_post_npc_npc : TOP_##x##_pre_i13_npc_npc; \
      break;								\
    }									\
  /* fall through */							\
  case TOP_##x##_npc_r_npc:						\
  case TOP_##x##_npc_sub_r_npc:						\
    if(regclass == ISA_REGISTER_CLASS_integer) {			\
      top_am_automod = post_mod ?					\
	( inc_mod ? TOP_##x##_npc_post_npc_npc : TOP_##x##_npc_sub_post_npc_npc) : \
	(inc_mod ? TOP_##x##_pre_npc_npc_npc : TOP_##x##_pre_npc_sub_npc_npc); \
    }									\
    break;

#define TOP_AM_i13_r(x,y)						\
  case TOP_##x##_i13_r_npc ## y :					\
    if(regclass == ISA_REGISTER_CLASS_UNDEFINED) {			\
      top_am_automod = post_mod ? TOP_##x##_i13_post_npc_npc ## y : TOP_##x##_pre_i13_npc_npc ## y; \
      break;								\
    }									\
  /* fall through */							\
  case TOP_##x##_npc_r_npc ## y :					\
  case TOP_##x##_npc_sub_r_npc ## y :					\
    if(regclass == ISA_REGISTER_CLASS_integer) {			\
      top_am_automod = post_mod ?					\
	( inc_mod ? TOP_##x##_npc_post_npc_npc ## y : TOP_##x##_npc_sub_post_npc_npc ## y) : \
	( inc_mod ? TOP_##x##_pre_npc_npc_npc ## y : TOP_##x##_pre_npc_sub_npc_npc ## y) ; \
    }									\
    break;

#define TOP_AM_i9_r(x,y)						\
  case TOP_##x##_i9_r_npc ## y :					\
    if(regclass == ISA_REGISTER_CLASS_UNDEFINED) {			\
      top_am_automod = post_mod ? TOP_##x##_i9_post_npc_npc ## y : TOP_##x##_pre_i9_npc_npc ## y; \
      break;								\
    }									\
  /* fall through */							\
  case TOP_##x##_npc_r_npc ## y :					\
  case TOP_##x##_npc_sub_r_npc ## y :					\
    if(regclass == ISA_REGISTER_CLASS_integer) {			\
      top_am_automod = post_mod ?					\
	( inc_mod ? TOP_##x##_npc_post_npc_npc ## y : TOP_##x##_npc_sub_post_npc_npc ## y) : \
	( inc_mod ? TOP_##x##_pre_npc_npc_npc ## y : TOP_##x##_pre_npc_sub_npc_npc ## y) ; \
    }									\
    break;
    
    
  TOP top_am_automod = TOP_UNDEFINED;

  // Reconfigurability: Extension TOPs are not handled here
  if (top >= TOP_static_count) {
	top_am_automod = EXTENSION_TOP_AM_automod_variant(top, post_mod, inc_mod, regclass);
	return (top_am_automod);
  }

  switch (top)
    {
      TOP_AM_i13_r_w(ldr)
      TOP_AM_i13_r(ldr,_b)
      TOP_AM_i13_r(ldr,_b_cond)

      TOP_AM_i13_r_w(str)
      TOP_AM_i13_r(str,_b)
      TOP_AM_i13_r(str,_b_cond)

      TOP_AM_i9_r(ldr,_sb);
      TOP_AM_i9_r(ldr,_sb_cond);
      TOP_AM_i9_r(ldr,_h);
      TOP_AM_i9_r(ldr,_h_cond);
      TOP_AM_i9_r(ldr,_sh);
      TOP_AM_i9_r(ldr,_sh_cond);

      TOP_AM_i9_r(str,_h);
      TOP_AM_i9_r(str,_h_cond);
    }

  return top_am_automod;
}

/* Functions to evaluate expressions (TOP). */

static void Set_VFlag( INT64 *flags ) { (*flags) |= 0x1LL; }
static void Clr_VFlag( INT64 *flags ) { (*flags) &= ~0x1LL; }
static BOOL Is_VFlag( INT64 flags )  { return flags & 0x1LL; }

static void Set_CFlag( INT64 *flags ) { (*flags) |= 0x2LL; }
static void Clr_CFlag( INT64 *flags ) { (*flags) &= ~0x2LL; }
static BOOL Is_CFlag( INT64 flags )  { return flags & 0x2LL; }

static void Set_ZFlag( INT64 *flags ) { (*flags) |= 0x4LL; }
static void Clr_ZFlag( INT64 *flags ) { (*flags) &= ~0x4LL; }
static BOOL Is_ZFlag( INT64 flags )  { return flags & 0x4LL; }

static void Set_NFlag( INT64 *flags ) { (*flags) |= 0x8LL; }
static void Clr_NFlag( INT64 *flags ) { (*flags) &= ~0x8LL; }
static BOOL Is_NFlag( INT64 flags )  { return flags & 0x8LL; }

/* Unsigned 32-bits underflow when unsigned result should be less than 0. */
static inline BOOL BorrowFrom32( INT64 result ) { return result < 0; }
/* Signed 32-bits overflow */
static inline BOOL OverflowFrom32( INT64 result ) { return result >= (1<<31); }
static inline BOOL IsNegative( INT64 result ) { return result < 0; }
static inline BOOL IsZero( INT64 result ) { return result == 0; }

static INT64 S_sub64( INT64 opnd_0, INT64 opnd_1 ) {
  return opnd_0 - opnd_1;
}

static INT64 S_add64( INT64 opnd_0, INT64 opnd_1 ) {
  return opnd_0 + opnd_1;
}

static INT64 S_or64( INT64 opnd_0, INT64 opnd_1 ) {
  return opnd_0 | opnd_1;
}

static INT64 S_and64( INT64 opnd_0, INT64 opnd_1 ) {
  return opnd_0 & opnd_1;
}

static INT64 S_extract32( INT64 opnd_0, INT from, INT to ) {
  INT64 mask = 0;
  FmtAssert(from-to<=32,("Extract more than 32 bits"));
  for(INT i=0; i < ((to-from)+1); i++) { mask |= 1 << (from+i); }
  return ((unsigned int)(opnd_0 & mask)) >> from;
}

static INT64 S_sign64( INT64 opnd_0, INT from_size ) {
  INT shift_amount = 64 - from_size;
  return opnd_0 << shift_amount >> shift_amount;
}

static INT64 S_lsl32( INT64 opnd_0, INT64 opnd_1 ) {
  return S_sign64((opnd_0 << S_extract32(opnd_1,0,5)) & 0xffffffff,32);
}

static INT64 S_lsr32( UINT64 opnd_0, INT64 opnd_1 ) {
  return S_sign64((opnd_0 >> S_extract32(opnd_1,0,5)) & 0xffffffff,32);
}

static INT64 S_asr32( INT64 opnd_0, INT64 opnd_1 ) {
  return S_sign64((opnd_0 >> S_extract32(opnd_1,0,5)) & 0xffffffff,32);
}

static INT64 S_mul32( INT64 opnd_0, INT64  opnd_1 ) {
  return S_sign64(opnd_0 * opnd_1,32);
}

static INT64 rsb_i8_r_npc( INT64 opnd_0, INT64 opnd_1) {
  return S_sign64(S_sub64(opnd_1, opnd_0),32);
}

static INT64 sub_i8_r_npc( INT64 opnd_0, INT64 opnd_1) {
  return S_sign64(S_sub64(opnd_0, opnd_1),32);
}

static INT64 add_i8_r_npc( INT64 opnd_0, INT64 opnd_1) {
  return S_sign64(S_add64(opnd_1, opnd_0),32);
}

static INT64 orr_lsl( INT64 opnd_0, INT64 opnd_1, INT64 opnd_2 ) {
  return S_sign64(S_or64(S_extract32(opnd_0,0,31),S_lsl32(opnd_1,opnd_2)),32);
}

static INT64 and_i8_r_npc( INT64 opnd_0, INT64 opnd_1 ) {
  return S_sign64(S_and64(S_extract32(opnd_0,0,31),S_extract32(opnd_1,0,31)),32);
}

static INT64 sub_sflags_0( INT64 opnd_0, INT64 opnd_1) {
  INT64 result = 0;
  INT64 tmp = S_sign64(S_sub64(S_extract32(opnd_0,0,31), S_extract32(opnd_1,0,31)),32);
  IsNegative(tmp) ? Set_NFlag(&result) : Clr_NFlag(&result);
  IsZero(tmp) ? Set_ZFlag(&result) : Clr_ZFlag(&result);
  BorrowFrom32(tmp) ? Clr_CFlag(&result) : Set_CFlag(&result);
  OverflowFrom32(tmp) ? Set_VFlag(&result) : Clr_VFlag(&result);
  return result;
}

/*
 * TOP_evaluate_op
 * Return the evaluated expression corresponding to given TOP and
 * the given result number.
 * Return FALSE if not able to evaluate.
 */
BOOL
TOP_evaluate_top( OP *op, INT64 *opnd_values, INT64 *result_val, int result_idx ) {

  if(OP_load(op) || OP_store(op) ||
     OP_xfer(op)) {
    return FALSE;
  }

  /* This part is target dependent and should be generated by a targinfo generator. */
  switch(OP_code(op)) {
  case TOP_and_i8_r_npc:
  case TOP_and_r_r_npc:
    FmtAssert(result_idx == 0,("Not result 0"));
    *result_val = and_i8_r_npc(opnd_values[0],opnd_values[1]);
    return TRUE;
  case TOP_sub_i8_r_npc:
  case TOP_sub_r_r_npc:
    FmtAssert(result_idx == 0,("Not result 0"));
    *result_val = sub_i8_r_npc(opnd_values[0],opnd_values[1]);
    return TRUE;
  case TOP_rsb_i8_r_npc:
  case TOP_rsb_r_r_npc:
    FmtAssert(result_idx == 0,("Not result 0"));
    *result_val = rsb_i8_r_npc(opnd_values[0],opnd_values[1]);
    return TRUE;
  case TOP_add_i8_r_npc:
  case TOP_add_r_r_npc:
    FmtAssert(result_idx == 0,("Not result 0"));
    *result_val = add_i8_r_npc(opnd_values[0],opnd_values[1]);
    return TRUE;
  case TOP_orr_i5_lsl_r_r_npc:
  case TOP_orr_npc_lsl_npc_npc_npc:
    FmtAssert(result_idx == 0,("Not result 0"));
    *result_val = orr_lsl(opnd_values[0],opnd_values[1],opnd_values[2]);
    return TRUE;
  case TOP_mov_i5_lsl_r_npc:
  case TOP_mov_npc_lsl_npc_npc:
    *result_val = S_lsl32(opnd_values[0],opnd_values[1]);
    return TRUE;
  case TOP_mov_i5_lsr_r_npc:
  case TOP_mov_npc_lsr_npc_npc:
    *result_val = S_lsr32(opnd_values[0],opnd_values[1]);
    return TRUE;
  case TOP_mov_i5_asr_r_npc:
  case TOP_mov_npc_asr_npc_npc:
    *result_val = S_asr32(opnd_values[0],opnd_values[1]);
    return TRUE;
  case TOP_mul:
    *result_val = S_mul32(opnd_values[0], opnd_values[1]);
    return TRUE;
  case TOP_cmp_i8_r:
  case TOP_cmp_r_r:
    FmtAssert(result_idx == 0,("Not result 0"));
    *result_val = sub_sflags_0(opnd_values[0],opnd_values[1]);
    return TRUE;
  case TOP_sub_i8_r_npc_sflags:
  case TOP_sub_r_r_npc_sflags:
    switch(result_idx) {
    case 0:
      *result_val = sub_sflags_0(opnd_values[0],opnd_values[1]);
      break;
    case 1:
      *result_val = sub_i8_r_npc(opnd_values[0],opnd_values[1]);
    default:
      FmtAssert(result_idx == 1,("Not result 1"));
    }
    return TRUE;
  default:
    FmtAssert(FALSE,("TOP:Not evaluated top: %s",TOP_Name(OP_code(op))));
  }

  return FALSE;
}

/*
 * OP_get_unconditional_variant
 * Return the unpredicated variant if given op.
 * TODO: Make the opposite form to get a predicated variant (if-conversion).
 */
OP *
OP_get_unconditional_variant( OP *op ) {

  TOP new_top = TOP_UNDEFINED;
  OP *new_op = NULL;

  switch(OP_code(op)) {
  case TOP_mov_npc_asr_npc_npc_cond:
    new_top = TOP_mov_npc_asr_npc_npc;
    break;
  case TOP_mov_i5_asr_r_npc_cond:
    new_top = TOP_mov_i5_asr_r_npc;
    break;
  default:
    FmtAssert(FALSE,("TOP:No unconditional variant for %s",TOP_Name(OP_code(op))));
  }

  if(new_top == TOP_UNDEFINED) { return new_op; }

  switch(new_top) {
  case TOP_mov_npc_asr_npc_npc:
  case TOP_mov_i5_asr_r_npc:
    new_op = Mk_OP(new_top,OP_result(op,0),OP_opnd(op,2),OP_opnd(op,3));
    break;
  }

  return new_op;
}

/*
 * OP_condition_is_true
 * In case of true/false predication model,
 * returns TRUE if predicate not equal to zero, else returns FALSE.
 * On ARM, we have to get the condvariant on the predicate to evaluate
 * if the condition.
 */
BOOL
OP_condition_is_true( OP *op, INT64 pred_val ) {

  BOOL condition = TRUE;

  if(!OP_is_predicated(op)) { return TRUE; }

  INT condvar_idx = OP_find_opnd_use(op,OU_condvariant);
  FmtAssert(condvar_idx != -1,("condvar idx not found"));
  TN *condvar_tn = OP_opnd(op,condvar_idx);
  FmtAssert(condvar_tn != NULL,("condvar tn is null"));
  FmtAssert(TN_is_enum(condvar_tn),("Not enum tn"));
  
  switch(TN_enum(condvar_tn)) {
  case ISA_ECV_cond_eq:
    condition = Is_ZFlag(pred_val);
    break;
  case ISA_ECV_cond_ne:
    condition = !Is_ZFlag(pred_val);
    break;
  case ISA_ECV_cond_cs:
  case ISA_ECV_cond_hs:
    condition = Is_CFlag(pred_val);
    break;
  case ISA_ECV_cond_cc:
  case ISA_ECV_cond_lo:
    condition = !Is_CFlag(pred_val);
    break;
  case ISA_ECV_cond_mi:
    condition = Is_NFlag(pred_val);
    break;
  case ISA_ECV_cond_pl:
    condition = !Is_NFlag(pred_val);
    break;
  case ISA_ECV_cond_vs:
    condition = Is_VFlag(pred_val);
    break;
  case ISA_ECV_cond_vc:
    condition = !Is_VFlag(pred_val);
    break;
  case ISA_ECV_cond_hi:
    condition = Is_CFlag(pred_val) && !Is_ZFlag(pred_val);
    break;
  case ISA_ECV_cond_ls:
    condition = !Is_CFlag(pred_val) && Is_ZFlag(pred_val);
    break;
  case ISA_ECV_cond_ge:
    condition = Is_NFlag(pred_val) == Is_VFlag(pred_val);
    break;
  case ISA_ECV_cond_lt:
    condition = Is_NFlag(pred_val) != Is_VFlag(pred_val);
    break;
  case ISA_ECV_cond_gt:
    condition = !Is_ZFlag(pred_val) && (Is_NFlag(pred_val) == Is_VFlag(pred_val));
    break;
  case ISA_ECV_cond_le:
    condition = Is_ZFlag(pred_val) || (Is_NFlag(pred_val) != Is_VFlag(pred_val));
    break;
  case ISA_ECV_cond_al:
    condition = TRUE;
    break;
  default:
    FmtAssert(FALSE,("Unknown condition"));
  }

  return condition;
}
