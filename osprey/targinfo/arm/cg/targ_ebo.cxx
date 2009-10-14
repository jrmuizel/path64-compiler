/*

  Copyright (C) 2002, ST Microelectronics, Inc.  All Rights Reserved.

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


/* =======================================================================
 * =======================================================================
 *
 *  Module: targ_ebo.cxx
 *
 *  Revision comments:
 *
 *  17-June-1998 - Initial version
 *
 *  Description:
 *  ============
 *
 *  EBO target-specific optimizations.
 *
 * =======================================================================
 * =======================================================================
 */

#ifdef USE_PCH
#include "cg_pch.h"
#endif // USE_PCH
#pragma hdrstop

#ifdef _KEEP_RCS_ID
static const char source_file[] = __FILE__;
#endif /* _KEEP_RCS_ID */

#include <list>
#include <stdarg.h>
#include <stdarg.h>
#include <stdlib.h>
#include "W_alloca.h"
#include "defs.h"
#include "config_TARG.h"
#include "errors.h"
#include "mempool.h"
#include "tracing.h"
#include "timing.h"

#include "targ_isa_properties.h"

#include "cgir.h"
#include "tn_map.h"
#include "cg_loop.h"
#include "cg.h"
#include "cgexp.h"
#include "register.h"
#include "cg_region.h"
#include "wn.h"
#include "region_util.h"
#include "op_list.h"
#include "cgprep.h"
#include "gtn_universe.h"
#include "gtn_set.h"
#include "cg_db_op.h"
#include "whirl2ops.h"
#include "cgtarget.h"
#include "gra_live.h"
#include "reg_live.h"
#include "cflow.h"
#include "cg_spill.h"
#include "data_layout.h"

#include "ebo.h"
#include "ebo_info.h"
#include "ebo_special.h"
#include "ebo_util.h"

#include "cg_automod.h"
#include "top_properties.h"

typedef std::list<OP*> ListOfOperations;
typedef ListOfOperations::const_iterator ListOfOperationsCIt;
typedef ListOfOperations::iterator ListOfOperationsIt;

static BOOL
Find_Uses(OP* startPoint, TN* soughtTn, ListOfOperations& result,
          BOOL stopOnFirst = FALSE);

static OP*
Find_Equiv_Uses(OP* startPoint, TN* soughtTn, ListOfOperations& result,
                BOOL stopOnFirst = FALSE);

static bool
TNequiv(TN* tn1, TN* tn2);


/* =====================================================================
 *   EBO_Verify_Ops
 *     EBO_Verify_Op
 *	EBO_Verify_Operand
 *
 * Verify that the generated op list can be emitted.
 * Should be called each time an op list in on the point to be
 * inserted into a basic block.
 *
 * The verifications are done in EBO_in_peep (after reg_alloc):
 * - Verify that all operands are assigned if in peep (after reg alloc).
 * - Verify that all operands are in a valid register subclass.
 *
 * =====================================================================
 */
static BOOL
EBO_Verify_Operand(OP *op, INT opnd, BOOL is_result)
{
  FmtAssert(ISA_SUBSET_LIST_Member(ISA_SUBSET_List, OP_code(op)),
	    ("%s is a member of available subsets", 
	     TOP_Name(OP_code(op))));

  const ISA_OPERAND_INFO *oinfo  = ISA_OPERAND_Info(OP_code(op));
  const ISA_OPERAND_VALTYP *vtype = is_result 
    ? ISA_OPERAND_INFO_Result(oinfo, opnd)
    : ISA_OPERAND_INFO_Operand(oinfo, opnd);
  TN *tn = is_result ? OP_result(op, opnd) : OP_opnd(op, opnd);
  
  if (EBO_in_peep) {
    if (ISA_OPERAND_VALTYP_Is_Register(vtype)) {
      REGISTER_SET class_regs;
      ISA_REGISTER_SUBCLASS sc = ISA_OPERAND_VALTYP_Register_Subclass(vtype);
      ISA_REGISTER_CLASS rc = ISA_OPERAND_VALTYP_Register_Class(vtype);
      REGISTER reg = TN_register(tn);
      FmtAssert(TN_is_register(tn), ("expected register"));
      
      if (reg == REGISTER_UNDEFINED) goto failed;
#if 0
      FmtAssert(TN_register_class(tn) == rc, ("expected register"));
#else
      // [JV] Due to cross register moves
      if(TN_register_class(tn) != rc) goto failed;
#endif
      class_regs =   (sc == ISA_REGISTER_SUBCLASS_UNDEFINED)
	? REGISTER_CLASS_universe(rc)
	: REGISTER_SUBCLASS_members(sc);
      
      if (!REGISTER_SET_MemberP(class_regs, reg)) goto failed;
    }
    return TRUE;
  }
  else {
    // [JV] Check only if register classes are compatibles
    if (ISA_OPERAND_VALTYP_Is_Register(vtype)) {
      ISA_REGISTER_CLASS rc = ISA_OPERAND_VALTYP_Register_Class(vtype);
      if(TN_register_class(tn) != rc) goto failed;
    }
    return TRUE;
  }

 failed:
  return FALSE;

}

static BOOL
EBO_Verify_Op(OP *op)
{
  int i;

#if 0
  // [JV] Want to check coherency as move cross registers are enabled.
  // Moved in EBO_Verify_Operand
  if (!EBO_in_peep) return TRUE;
#endif
  for (i = 0; i < OP_fixed_opnds(op); i++) {
    if (!EBO_Verify_Operand(op, i, FALSE)) return FALSE;
  }
  for (i = 0; i < OP_fixed_results(op); i++) {
    if (!EBO_Verify_Operand(op, i, TRUE)) return FALSE;
  }
  return TRUE;
}

BOOL
EBO_Verify_Ops(OPS *ops)
{
  OP *op;
  if (!EBO_in_peep) return TRUE;
  for (op = OPS_first(ops); op != NULL; op = OP_next(op)) {
    if (!EBO_Verify_Op(op)) return FALSE;
  }
  return TRUE;
}

/* =====================================================================
 *   EBO_Can_Merge_Into_Offset
 *
 *   Identify OP's that contain a constant and operate in a way that
 *   will allow the constant to be added into an offset field of
 *   a load or store instruction.
 * =====================================================================
 */
BOOL 
EBO_Can_Merge_Into_Offset (
  OP *op,
  INT *index,
  INT *immed
)
{
  TOP opcode = OP_code(op);

  if (!OP_iadd(op) && !OP_isub(op) && !OP_ior(op)) {
    return FALSE;
  }
  if ((op == BB_entry_sp_adj_op(OP_bb(op))) ||
      (op == BB_exit_sp_adj_op(OP_bb(op)))) {
     return FALSE;
  }

  if ((OP_iadd(op) || OP_ior(op)) && TN_Is_Constant(OP_opnd(op,1))) {
    *index = 0;
    *immed = 1;
    return TRUE;
  }

  if (OP_isub(op) && TN_Is_Constant(OP_opnd(op,1))) {
    *index = 0;
    *immed = 1;
    return TRUE;
  }

  return FALSE;
}

/* =====================================================================
 *   EBO_condition_redundant
 * =====================================================================
 */
BOOL
EBO_condition_redundant(
  OP *elim_op,
  EBO_TN_INFO **opnd_tninfo,
  EBO_OP_INFO *prev_opinfo,
  OPS *ops
)
{
  OP *prev_op = prev_opinfo->in_op;

  TOP p_opcode = OP_code(prev_op);
  TOP e_opcode = OP_code(elim_op);

#if 0
  if ((p_opcode == e_opcode) ||
      ((p_opcode == TOP_cmpeq_r) && (e_opcode == TOP_cmpne_r))) {

    TN *pr0 = OP_result(prev_op,0);
    TN *er0 = OP_result(elim_op,0);
#if 0
    TN *prp = OP_opnd(prev_op,OP_PREDICATE_OPND);
    TN *erp = OP_opnd(elim_op,OP_PREDICATE_OPND);
#endif

    if (p_opcode != e_opcode) {
     /* We are dealing with complement compare instructions. */
      TN *save_tn = pr0;

#if 0
      if (((pr0 == True_TN) && (er0 == True_TN)) ||
          ((pr1 == True_TN) && (er1 == True_TN))) {
       /* We may be able to replace both compares with a single one. */

        if (prp != erp) goto can_not_combine_ops;

        for (INT i = 0; i < OP_results(prev_op); i++) {
          if (!TN_Is_Constant(OP_result(prev_op,i))) {
              EBO_TN_INFO *result_tninfo = prev_opinfo->actual_rslt[i];
            if ((result_tninfo == NULL) ||
                (result_tninfo->reference_count != 0)) goto can_not_combine_ops;
          }
        }

        OP *new_op = Dup_OP(elim_op);
        if (pr0 == True_TN) {
          Set_OP_result(new_op, 0, pr1);
        } else {
          Set_OP_result(new_op, 1, pr0);
        }
        if (EBO_in_loop) EBO_Copy_OP_omega (new_op, elim_op);
        OP_srcpos(new_op) = OP_srcpos(elim_op);
	if (!EBO_Verify_Op(new_op)) return FALSE;
        BB_Insert_Op_After(OP_bb(elim_op), elim_op, new_op);

        if (EBO_Trace_Optimization) {
          #pragma mips_frequency_hint NEVER
          fprintf(TFile,"%sMerge complementery compare operations.\n",EBO_trace_pfx);
        }

        return TRUE;
      }

     /* Interchange names for the results so that the opcodes can 
      * be viewed as the same. */
      save_tn = pr0;
      pr0 = pr1;
      pr1 = save_tn;

#endif

      if (EBO_Trace_Optimization) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sCompare instructions are complements.\n",EBO_trace_pfx);
      }
    }

#if 0
    if ((prp != erp) ||
        (EBO_in_pre &&
         (OP_bb(prev_op) != OP_bb(elim_op)))) {
     /* Predicates are not the same.  This could be a problem. */
     /* If BBs are not the same we could get into trouble with */
     /* predicates after hyperblock formation.                 */
     /*                                                        */
     /* There are a few special cases that could be caught,    */
     /* but it doesn't seem worth the effort.                  */
      if (EBO_Trace_Optimization) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sPredicates are different for redundant compare.\n",EBO_trace_pfx);
      }

      goto can_not_combine_ops;
    }
#endif

#if 0
   /* The result registers may conflict, if they are being re-assigned. */
    if (((er0 != True_TN) && tn_registers_identical (er0, pr1)) ||
        ((er1 != True_TN) && tn_registers_identical (er1, pr0))) {

      if (EBO_Trace_Optimization) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sRegister re-use in redundant compare.\n",EBO_trace_pfx);
      }

      goto can_not_combine_ops;
    }
#endif

    /* 
     * But if the same registers are defined, we can just 
     * delete this OP. 
     */
    if (tn_registers_identical (er0, pr0)) {
      if (EBO_Trace_Optimization) {
        #pragma mips_frequency_hint NEVER
        fprintf(TFile,"%sDelete redundant compare operation.\n",EBO_trace_pfx);
      }

      return TRUE;
    }

#if 0
    if (er0 != True_TN) {
      if (pr0 != True_TN) {
        EBO_Copy_Predicate (er0, pr0, &ops);
      } else {
        EBO_Complement_Predicate (er0,pr1, &ops);
      }
    } 
    if (er1 != True_TN) {
      if (pr1 != True_TN) {
        EBO_Copy_Predicate (er1, pr1, &ops);
      } else {
        EBO_Complement_Predicate (er1,pr0, &ops);
      }
    }

    if (EBO_Trace_Optimization) {
      #pragma mips_frequency_hsint NEVER
      fprintf(TFile,"%sRemove redundant compare operation.\n",EBO_trace_pfx);
    }
    return TRUE;
#endif
  }
#endif
  return FALSE;
}

/* =====================================================================
 *   EBO_combine_adjacent_loads
 * =====================================================================
 */
BOOL
EBO_combine_adjacent_loads(
  OP *op,
  TN **opnd_tn,
  EBO_TN_INFO **opnd_tninfo,
  EBO_OP_INFO *opinfo,
  INT64 offset_pred,
  INT64 offset_succ
)
{
  return FALSE;

#if 0


  OP *pred_op = opinfo->in_op;
  BB *bb = OP_bb(op);
  TOP opcode = OP_code(op);
  TOP new_opcode;
  TN *pred_result = OP_result(pred_op,0);
  TN *succ_result = OP_result(op,0);
  INT base_index = TOP_Find_Operand_Use(OP_code(op),OU_base);
  TN *base_tn;
  EBO_TN_INFO *base_tninfo;
  INT size_pred;
  INT size_succ;
  TN *r1;
  TN *r2;
  OPS ops = OPS_EMPTY;

  if (EBO_Trace_Data_Flow) {
    fprintf(TFile,"%s          OP in BB:%d    ",EBO_trace_pfx,BB_id(bb));
    Print_OP_No_SrcLine(op);
    fprintf(TFile," is adjacent to OP in BB:%d    ",BB_id(opinfo->in_bb));
    Print_OP_No_SrcLine(pred_op);
  }
  
  if (!OP_load(op) || !OP_load(pred_op) || 
      (opcode != OP_code(pred_op)) ||
      (OP_results(op) != 1) || 
      (OP_results(pred_op) != 1)) {
    return FALSE;
  }

  if ((Opt_Level < 2) && (bb != opinfo->in_bb)) {
   /* Global TN's aren't supported at low levels of optimization. */
    return FALSE;
  }

  size_pred = OP_Mem_Ref_Bytes(pred_op);
  size_succ = OP_Mem_Ref_Bytes(op);
  if (size_pred != size_succ) return FALSE;

  if (!EBO_in_peep &&
      (bb != opinfo->in_bb) &&
      !TN_Is_Constant(pred_result) &&
      has_assigned_reg(pred_result)) {
    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"%sShouldn't move dedicated register references across blocks.\n",
              EBO_trace_pfx);
    }
    return FALSE;
  }

  if (!has_assigned_reg(pred_result) ||
      !has_assigned_reg(succ_result) ||
      (TN_register_class(pred_result) != TN_register_class(succ_result)) ||
      ((TN_register(pred_result) & 1) == (TN_register(succ_result) & 1))) {
    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"%sParied loads requires odd/even registers.\n", EBO_trace_pfx);
    }
    return FALSE;
  }

  if ((opinfo->actual_rslt[0] == NULL) ||
      (opinfo->actual_rslt[0]->reference_count != 0) ||
      !EBO_tn_available (opinfo->in_bb, opinfo->actual_rslt[0])) {
    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"%sThe result of the predecessor load has already been used.\n", EBO_trace_pfx);
    }
    return FALSE;
  }

  return FALSE;

  if (offset_pred < offset_succ) {
    base_tn = OP_opnd(pred_op, base_index);
    base_tninfo = opinfo->actual_opnd[base_index];
    if (!TN_Is_Constant(base_tn) && !EBO_tn_available (opinfo->in_bb, base_tninfo)) {
      if (EBO_Trace_Data_Flow) {
        fprintf(TFile,"%sThe index TN of the predecessor load is not available.\n", EBO_trace_pfx);
      }
      return FALSE;
    }
    r1 = pred_result;
    r2 = succ_result;
  } else {
    base_tn = OP_opnd(op, base_index);
    base_tninfo = opnd_tninfo[base_index];
    r1 = succ_result;
    r2 = pred_result;
  }

/* NYI: The simulator does not yet support these instructions.  */
  if (EBO_Trace_Optimization) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sThe simulator does not yet support load-pair instructions.\n",EBO_trace_pfx);
  }
  return FALSE;

  Build_OP (new_opcode, r1, r2, OP_opnd(op, OP_find_opnd_use(op, OU_predicate)),
            OP_opnd(op,1), OP_opnd(op,2), base_tn, &ops);

  Copy_WN_For_Memory_OP (OPS_first(&ops), op);

  // (cbr) Support for guards on false
  OPS_Copy_Predicate (&ops, op);

  OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);
  if (EBO_in_loop) EBO_Set_OP_omega (OPS_first(&ops), opnd_tninfo[OP_PREDICATE_OPND],
                                     NULL, NULL, base_tninfo);
  if (!EBO_Verify_Ops(&ops)) return FALSE;
  BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);

  remove_op (opinfo);
  OP_Change_To_Noop(pred_op);
  opinfo->in_op = NULL;
  opinfo->in_bb = NULL;

  if (EBO_Trace_Optimization) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile,"%sReplace adjacent load with load pair.\n",EBO_trace_pfx);
  }

  return TRUE;

#endif
}

/* =====================================================================
 *   EBO_replace_subset_load_with_extract
 *
 * For a Store - Load sequence where we have
 * 1. store at offset_pred of size_pred
 * 2. load at offset_succ of size_succ
 * 
 * Currently only handle the case where:
 * offset_pred == offset_succ == 0 and size_pred == size_succ
 * In this case we must emit the load extension.
 * =====================================================================
 */
BOOL
EBO_replace_subset_load_with_extract (
  OP *op,
  TN *pred_result,
  TN *succ_result,
  INT offset_pred,
  INT offset_succ,
  INT size_pred,
  INT size_succ
)
{
  INT byte_offset = offset_succ - offset_pred;

#if 0
  if (offset_pred == 0 && offset_pred == offset_succ &&
      size_pred == size_succ) {
    if (TN_size(succ_result) > size_succ) {
      TOP new_opcode = TOP_UNDEFINED;
      TN *tn1;
      TN *tn2;
      if (size_succ == 2) {
	new_opcode = TOP_is_unsign(OP_code(op)) ? TOP_extuh : TOP_exth;
	tn1 = pred_result;
	tn2 = NULL;
      } else if (size_succ == 1) {
	if (TOP_is_unsign(OP_code(op))) {
	  new_opcode = TOP_and_i8;
	  tn1 = pred_result;
	  tn2 = Gen_Literal_TN(0xff, 4);
	} else {
	  new_opcode = TOP_extb;
	  tn1 = pred_result;
	  tn2 = NULL;
	}
      }
      if (new_opcode == TOP_UNDEFINED) return FALSE;
      OP *new_op;
      new_op = Mk_OP(new_opcode, succ_result, True_TN, tn1,tn2);

      // (cbr) Support for guards on false
      CGTARG_Predicate_OP (NULL, new_op, OP_Predicate (op),
                           OP_Pred_False (op, OP_find_opnd_use(op, OU_predicate)));

      OP_srcpos(new_op) = OP_srcpos(op);
      if (!EBO_Verify_Op(new_op)) return FALSE;
      BB_Insert_Op_After(OP_bb(op), op, new_op);
      
      return TRUE;
      
    }
  }
#endif  
  return FALSE;
}

/* =====================================================================
 *   EBO_copy_value
 *
 *   Return TRUE if we did it.
 * =====================================================================
 */
BOOL
EBO_copy_value (
  OP *op,                   // load being removed
  TN *pred_result,          // preceeding store
  TN *intervening_result,   // intervening store
  INT size                  // load size
)
{
  OPS ops = OPS_EMPTY;

  if ((size != 4) && OP_unsigned_ld(op)) {
    /* 
     * Does it make sense to use bit-extract sequence to simulate 
     * store-load sequence ?
     */
    return FALSE;
  }

  /* Use full word copy. */
  // (cbr) Support for guards on false
  if (OP_has_predicate (op)) {
    int pred_index = OP_find_opnd_use(op, OU_predicate);
    EBO_Exp_COPY(OP_opnd(op, pred_index), OP_Pred_False(op, pred_index), OP_cond_def(op), OP_result(op, 0), pred_result, &ops);
  }
  else
    EBO_Exp_COPY(NULL, false, false, OP_result(op, 0), pred_result, &ops);

  if (!EBO_Verify_Ops(&ops)) return FALSE;

  OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);

  if (!EBO_Verify_Ops(&ops)) return FALSE;
  BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);

  return TRUE;
}

/* =====================================================================
 *   EBO_select_value
 *
 *   Compare the reload address with the intervening store address.
 *   Select the stored value if the address are the same,
 *   and resuse the predecesor value if they are not the same.
 *
 * =====================================================================
 */
BOOL
EBO_select_value (
  OP *op,
  TN *pred_result,         // preceeding store's result
  TN *pred_base,           // preceeding store's base
  TN *pred_offset,
  TN *intervening_result,
  TN *intervening_base,
  TN *intervening_offset,
  INT size                 // size of load
)
{
#if 0
  OPS ops = OPS_EMPTY;

  /* 
   * Unsigned Store - Load sequence for small integers will strip off 
   * sign bit. Does it make sense to use bit-extract sequence to 
   * simulate store-load sequence ?
   */
  if ((size < 4) && OP_unsigned_ld(op)) {
    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"%sUnsigned load strips a bit\n",
              EBO_trace_pfx);
    }
    return FALSE;
  }

  /* If load/stores are of the form ldw/stw (sym, base)
   * Compare sym should suffice. Why ?
   */
  if ((TN_is_symbol(pred_offset) && !TN_is_symbol(intervening_offset)) ||
      (TN_is_symbol(intervening_offset) && !TN_is_symbol(pred_offset))) {
    if (EBO_Trace_Data_Flow) {
      fprintf(TFile,"%sdifferent kinds of memory operation\n",
              EBO_trace_pfx);
    }
    return FALSE;
  }
  else if (TN_is_symbol(pred_offset)) {
    // Both must be symbols
    ST *pred_st = TN_var(pred_offset);
    ST *inrevening_st = TN_var(intervening_offset);
    OPS ops1 = OPS_EMPTY;

    if (pred_st == inrevening_st) {
      Build_OP(TOP_mov_r, OP_result(op, 0), intervening_result, &ops1);
    }
    else {
      Build_OP(TOP_mov_r, OP_result(op, 0), pred_result, &ops1);
    }
    Set_OP_copy(OPS_last(&ops1));

    // (cbr) Support for guards on false
    OPS_Copy_Predicate (&ops1, op);

    OP_srcpos(OPS_last(&ops1)) = OP_srcpos(op);
    OPS_Append_Ops(&ops, &ops1);
  }
  else {
      FmtAssert (FALSE, ("not yet implemented"));
#if 0
    // Both are base + immed offset
    FmtAssert(TN_has_value(pred_offset) && TN_has_value(intervening_offset),
	      ("immediate offsets are wrong"));

    /* Are the immediate offsets same ? */
    if (TN_value(pred_offset) != TN_value(intervening_offset)) {
      if (EBO_Trace_Data_Flow) {
	fprintf(TFile,"%sImmediate offsets are not equal\n",
              EBO_trace_pfx);
      }
      return FALSE;
    }

    /* 
     * Compare the base addresses. 
     * Arthur: if comparison is redundant it will be cleaned up.
     */
    TN *predicate = Build_RCLASS_TN (ISA_REGISTER_CLASS_gr);

    Build_OP (TOP_cmpeq_r, predicate, True_TN, pred_base, intervening_base, &ops);
    OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);

    OPS ops1 = OPS_EMPTY;

    /* Copy the "address not equal value". */
    Build_OP(TOP_slct_r, 
	     OP_result(op, 0), 
	     predicate, 
	     intervening_result,
	     pred_result, 
	     &ops1);

    OP_srcpos(OPS_last(&ops1)) = OP_srcpos(op);
    OPS_Append_Ops(&ops, &ops1);
#endif
  }

  if (!EBO_Verify_Ops(&ops)) return FALSE;
  BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);

  if (EBO_Trace_Optimization) {
    fprintf(TFile,"%sRemove Reload across intervening store.\n",EBO_trace_pfx);
  }

  return TRUE;

#endif
  return FALSE;
}

/* =====================================================================
 *    Normalize_Immediate
 *
 *    Normalize an immediate value for use with a specific instruction.
 *    In some cases the instruction may ignore parts of the immediate,
 *    so we mimic that here.
 * =====================================================================
 */
/* Define a macro to strip off any bits outside of the left most 4 bytes. */
#define TRUNC_32(val) (val & 0x00000000ffffffffll)

/* Define a macro to sign-extend the least signficant 32 bits */
#define SEXT_32(val) (((INT64)(val) << 32) >> 32)
static INT64 
Normalize_Immediate (
  TOP opcode, 
  INT64 immed
)
{
#if 0
  switch (opcode) {

  case TOP_cmpeq_i8:
    /*
     * For signed compares sign-extend the constant to 64 bits.
     */
    return SEXT_32(immed);
  case TOP_cmpleu_i8:
    /*
     * For unsigned compares, zero-extend the constant to 64 bits.
     */
    return TRUNC_32(immed);
  }
#endif
  return immed;
}


/* =====================================================================
 *    EBO_simplify_operand0
 *
 *    Return a new_op if the opcode can be simplified to a
 *    immediate form.
 *    The operands tn0 and const_val are logically equivalent operands at index 
 *    OU_opnd1 and OU_opnd2 which may not be 0 and 1.
 *    To get the effective operands of the operation get it from op.
 *    In this function, if the operand 0 can't be inlined, we try to
 *    invert the operator and call EBO_simplify_operand1.
 * =====================================================================
 */
OP*
EBO_simplify_operand0 (
  OP *op,           // OP being simplified
  TN *tnr,          // result TN
  INT64 const_val,  // constant value of opnd TN 0
  TN *tn1           // opnd TN 1
)
{
  TOP opcode = OP_code(op);
  TOP new_opcode;
  OP *new_op;
  INT opnd1_idx, opnd2_idx;

  opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
  opnd2_idx = OP_find_opnd_use(op, OU_opnd2);
  DevAssert(opnd1_idx >= 0, ("OU_opnd1 not defined for TOP %s\n", TOP_Name(opcode)));

  if (EBO_Trace_Optimization) { 
    fprintf(TFile,"Enter EBO_simplify_operand0: %s ", TOP_Name(opcode));
    fprintf(TFile," %lld ", const_val);
    Print_TN(tn1,FALSE);
    fprintf(TFile," : tns ");
    Print_TN(OP_opnd(op, opnd1_idx),FALSE);
    if(opnd2_idx >= 0) {
        fprintf(TFile," ");
        Print_TN(OP_opnd(op, opnd2_idx),FALSE);
    }
    fprintf(TFile,"\n");
  }

  /* Inlining of immediate operand unless Zero_TN. */
  if (TN_is_register(OP_opnd(op, opnd1_idx)) &&
      OP_opnd(op, opnd1_idx) != Zero_TN) {
    new_opcode = TOP_opnd_immediate_variant(opcode, opnd1_idx, const_val);
    if (new_opcode != TOP_UNDEFINED && 
	new_opcode != opcode) {
      new_op = Dup_OP(op);
      OP_Change_Opcode(new_op, new_opcode);
      Set_OP_opnd(new_op,opnd1_idx, Gen_Literal_TN(const_val, 4));
      if (EBO_Trace_Optimization) fprintf(TFile,"replace op %s with immediate form %s\n", TOP_Name(opcode), TOP_Name(new_opcode));
      return new_op;
    }
  }
  
  /* Invert operands and try to simplify operand 1 if it exists. */
  if (TN_is_register(OP_opnd(op, opnd1_idx)) &&
      OP_opnd(op, opnd1_idx) != Zero_TN && opnd2_idx >= 0 &&
      TN_is_register(OP_opnd(op, opnd2_idx))) {
    new_opcode = OP_opnd_swapped_variant(op, opnd1_idx, opnd2_idx);
    if (new_opcode != TOP_UNDEFINED) {
      new_op = Dup_OP(op);
      OP_Change_Opcode(new_op, new_opcode);
      Set_OP_opnd(new_op, opnd1_idx, OP_opnd(op, opnd2_idx));
      Set_OP_opnd(new_op, opnd2_idx, OP_opnd(op, opnd1_idx));
      return EBO_simplify_operand1(new_op, tnr, tn1, const_val);
    }
  }

  return NULL;
}

/* =====================================================================
 *    EBO_simplify_operand1
 *
 *    Return a new_op if the opcode can be simplified to a
 *    immediate form.
 *    The operands tn0 and const_val are logically equivalent operands at index 
 *    OU_opnd1 and OU_opnd2 which may not be 0 and 1.
 *    To get the effective operands of the operation get it from op.
 * =====================================================================
 */
OP*
EBO_simplify_operand1 (
  OP *op,           // OP being simplified
  TN *tnr,          // result TN
  TN *tn0,          // opnd TN 0
  INT64 const_val   // constant value of opnd TN 1
)
{
  TOP opcode = OP_code(op);
  TOP new_opcode;
  OP *new_op;
  INT opnd1_idx, opnd2_idx;

  opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
  opnd2_idx = OP_find_opnd_use(op, OU_opnd2);
  DevAssert(opnd1_idx >= 0 && opnd2_idx >= 0, ("OU_opnd1 and/or OU_opnd2 not defined for TOP %s\n", TOP_Name(opcode)));

  if (EBO_Trace_Optimization) { 
    fprintf(TFile,"Enter EBO_simplify_operand1: %s ", TOP_Name(opcode));
    Print_TN(tn0,FALSE);
    fprintf(TFile," %lld ", const_val);
    fprintf(TFile,": tns ");
    Print_TN(OP_opnd(op, opnd1_idx),FALSE);
    if(opnd2_idx >= 0) {
        fprintf(TFile," ");
        Print_TN(OP_opnd(op, opnd2_idx),FALSE);
    }
    fprintf(TFile,"\n");
  }

  /* Inlining of immediate operand unless Zero_TN. */
  if (TN_is_register(OP_opnd(op, opnd2_idx)) && 
      opnd2_idx >= 0 &&
      OP_opnd(op, opnd2_idx) != Zero_TN) {
    new_opcode = TOP_opnd_immediate_variant(opcode, opnd2_idx, const_val);
    if (new_opcode != TOP_UNDEFINED && 
	new_opcode != opcode) {
      new_op = Dup_OP(op);
      OP_Change_Opcode(new_op, new_opcode);
      Set_OP_opnd(new_op,opnd2_idx,Gen_Literal_TN(const_val, 4));
      if (EBO_Trace_Optimization) fprintf(TFile,"replace op %s with immediate form %s\n", TOP_Name(opcode), TOP_Name(new_opcode));
      return new_op;
    }
  }

  return NULL;
}

/* =====================================================================
 *    EBO_Resolve_Conditional_Branch
 *
 *    Look at a branch expression that has all constant operands and 
 *    attempt to evaluate the expression.
 * =====================================================================
 */
BOOL
EBO_Resolve_Conditional_Branch (
  OP *op, 
  TN **opnd_tn
)
{
  BB *bb = OP_bb(op);
  BB *fall_bb;
  BB *branch_bb;

#if 0
  if (BBlist_Len(BB_succs(bb)) != 2) return FALSE;

  fall_bb = BB_next(bb);
  branch_bb = BBLIST_item(BB_succs(bb));
  if (branch_bb == fall_bb) {
      branch_bb = BBLIST_item(BBLIST_next(BB_succs(bb)));
  }

  if (OP_code(op) == TOP_jr) {
    TN *predicate = OP_opnd(op, OU_condition);

    if (EBO_Trace_Optimization) {
      INT i;
      INT opndnum = OP_opnds(op);
      fprintf(TFile, "%sin BB:%d Resolve conditional BR :- %s ",
              EBO_trace_pfx, BB_id(bb),TOP_Name(OP_code(op)));
      for (i = 0; i < opndnum; i++) {
        fprintf(TFile," ");
        Print_TN(opnd_tn[i],FALSE);
      }
      fprintf(TFile,"\n");
    }
    
    if (TN_is_true(predicate)) {
      if (OP_Pred_False(op, OP_find_opnd_use(op, OU_condition))) {
        /*
         * Branch IS NOT taken - remove the conditional branch
         */
        Unlink_Pred_Succ (bb, branch_bb);
        Change_Succ_Prob (bb, fall_bb, 1.0);
      }
      else {
        /* 
         * Branch IS taken - replace the conditional branch with a 
         * simple branch. 
         */
        OPS ops = OPS_EMPTY;
      
        Build_OP (TOP_jr, 
                  True_TN,
                  OP_opnd(op,1), 
                  &ops);
        OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);
        if (!EBO_Verify_Ops(&ops)) return FALSE;
        BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
        Unlink_Pred_Succ (bb, fall_bb);
        Change_Succ_Prob (bb, branch_bb, 1.0);
      }
      return TRUE;
    }
  }

#endif
  return FALSE;
}

/* =====================================================================
 *    EBO_Fold_Special_Opcode
 * 
 *    Target-specific folding.
 *
 * =====================================================================
 */
BOOL 
EBO_Fold_Special_Opcode (
  OP *op,
  TN **opnd_tn,
  INT64 *result_val
)
{
  TOP opcode = OP_code(op);
  TN *tn0 = NULL, *tn1 = NULL;
  INT64 tn0_val, tn1_val;

  if (EBO_Trace_Optimization) {
    fprintf(TFile, "%sFold_Special_Opcode\n", EBO_trace_pfx);
  }

  /* [CG] We could handle constant symbols (such as floating point constant),
     but we don't. */
  if (OP_opnds(op) >= 1) {
    tn0 = opnd_tn[0];
    if (TN_Has_Value(opnd_tn[0])) tn0_val = TN_Value(opnd_tn[0]);
    else return FALSE;
  }
  if (OP_opnds(op) >= 2) {
    tn1 = opnd_tn[1];
    if (TN_Has_Value(opnd_tn[1])) tn1_val = TN_Value(opnd_tn[1]);
    else return FALSE;
  }

  /* Nothing target specific to fold. */
  return FALSE;

 Folded:

  if (EBO_Trace_Optimization) {
    fprintf(TFile, "%sfolded???: %llx\n", EBO_trace_pfx, *result_val);
  }
  return TRUE;
}

/*
 * l0c()
 *
 * Returns leading zero count of 64 bits value
 */
static int
l0c(INT64 val)
{
  int n = 64;
  while(val > 0) {
    n--;
    val >>= 1;
  }
  if (val == 0) return n;
  return 0;
}

/*
 * l1c()
 *
 * Returns leading one count of 64 bits value
 */
static int
l1c(INT64 val)
{
  int n = 64;
  while(val < -1) {
    n--;
    val >>= 1;
  }
  if (val == -1) return n;
  return 0;
}

/*
 * t1c()
 * 
 * Return the number of trailing 1 bits followed by only 0 bits.
 * Return -1 if not.
 */
static int
t1c(INT64 val)
{
  int n = 0;
  while (val & 1) {
    n++;
    val = (UINT64)val >> 1;
  }
  if (val == 0) return n;
  return -1;
}


/*
 * def_bit_width
 *
 * Returns true if the defined bits of the def can
 * be guessed and returns bit width and sign extension.
 *
 * Property driven cases are:
 * - loads
 * - shift right
 * - sext/zext
 * - and
 *
 * Target specific for STxP70 are:
 * - TOP_bool (generate 1 bit register)
 *
 */
static BOOL
def_bit_width(OP *op, INT32 def_idx, INT32 *def_bits, INT32 *def_signed)
{
  INT32 opnd1_idx, opnd2_idx;
  INT64 val;
  TOP opcode = OP_code(op);

#if 0

  if (def_idx != 0) return FALSE;

  if (OP_load(op)) {
    *def_bits = OP_Mem_Ref_Bytes(op)*8;
    *def_signed = TOP_is_unsign(opcode) ? 0 : 1;
    return TRUE;
  }
  if ((OP_ishr(op) || OP_ishru(op)) &&
      (opnd1_idx = TOP_Find_Operand_Use(opcode,OU_opnd1)) >= 0 &&
      (opnd2_idx = TOP_Find_Operand_Use(opcode,OU_opnd2)) >= 0 &&
      TN_Has_Value(OP_opnd(op,opnd2_idx))) {
    val = TOP_fetch_opnd(opcode, op->res_opnd+OP_opnd_offset(op), opnd2_idx);
    INT32 use_bits = TOP_opnd_use_bits(opcode, opnd1_idx);
    *def_bits = MAX(0, use_bits - val);
    *def_signed = TOP_opnd_use_signed(opcode, opnd1_idx);
    return TRUE;
  }

  if ((OP_sext(op) || OP_zext(op)) &&
      (opnd1_idx = TOP_Find_Operand_Use(opcode,OU_opnd1)) >= 0) {
    *def_bits = TOP_opnd_use_bits(opcode, opnd1_idx);
    *def_signed = TOP_opnd_use_signed(opcode, opnd1_idx);
    return TRUE;
  }

  if (OP_iand(op) &&
      (opnd1_idx = TOP_Find_Operand_Use(opcode,OU_opnd1)) >= 0 &&
      (opnd2_idx = TOP_Find_Operand_Use(opcode,OU_opnd2)) >= 0 &&
      TN_Has_Value(OP_opnd(op,opnd2_idx))) {
    val = TOP_fetch_opnd(opcode, op->res_opnd+OP_opnd_offset(op), opnd2_idx);
    INT32 use_bits = TOP_opnd_use_bits(opcode, opnd1_idx);
    INT32 bits = 64 - l0c(val);
    if (bits < use_bits) {
      *def_bits = bits;
      *def_signed = 0;
      return TRUE;
    }
  }

  if (OP_icmp(op)) {
    *def_bits = 1;
    *def_signed = 0;
    return TRUE;
  }

  if (OP_code(op) == TOP_bool) {
    *def_bits = 1;
    *def_signed = 0;
    return TRUE;
  }

#endif
  return FALSE;
}

/*
 * use_bit_width
 *
 * Returns true if the effective used bit of the operand def can
 * be guessed and returns bit width.
 * Property driven cases are:
 * - shift left: use bits - shift amount
 * - and : use bits - leading zeros
 * - or : use bits - leading one
 *
 */
static BOOL
use_bit_width(OP *op, INT32 opnd_idx, INT32 *use_bits)
{
  INT32 opnd1_idx, opnd2_idx;
  INT64 val;
  TOP opcode = OP_code(op);
  INT32 bits = TOP_opnd_use_bits(opcode, opnd_idx);

#if 0
  if (bits < 0) {
    return FALSE;
  }
  *use_bits = bits;

  if (OP_ishl(op) &&
      (opnd1_idx = TOP_Find_Operand_Use(opcode,OU_opnd1)) >= 0 &&
      (opnd2_idx = TOP_Find_Operand_Use(opcode,OU_opnd2)) >= 0 &&
      opnd_idx == opnd1_idx &&
      TN_Has_Value(OP_opnd(op,opnd2_idx))) {
    val = TOP_fetch_opnd(opcode, op->res_opnd+OP_opnd_offset(op), opnd2_idx);
    *use_bits = MAX(0, bits - val);
  }
  if ((OP_iand(op) || OP_ior(op)) &&
      (opnd1_idx = TOP_Find_Operand_Use(opcode,OU_opnd1)) >= 0 &&
      (opnd2_idx = TOP_Find_Operand_Use(opcode,OU_opnd2)) >= 0 &&
      opnd_idx == opnd1_idx &&
      TN_Has_Value(OP_opnd(op,opnd2_idx))) {
    val = TOP_fetch_opnd(opcode, op->res_opnd+OP_opnd_offset(op), opnd2_idx);
    INT32 eff_bits = OP_iand(op) ? 64 - l0c(val) : 64 - l1c(val);
    if (eff_bits < bits) {
      *use_bits = eff_bits;
    }
  }
  return TRUE;
#endif
  return FALSE;
}

/*
 * OP_is_extension()
 *
 * Returns true if the operation acts as an extension for the given operand.
 * Set signed to true if sign extension
 * TODO: generic
 */
static BOOL
OP_is_extension(OP *op, INT32 opnd_idx, INT32 *ext_bits, INT32 *ext_signed)
{
  INT32 opnd1_idx, opnd2_idx;
  INT64 val;
  TOP opcode = OP_code(op);

#if 0  
  if (OP_results(op) != 1) return FALSE;
  
  if ((OP_sext(op) || OP_zext(op))) {
    opnd1_idx = TOP_Find_Operand_Use(opcode,OU_opnd1);
    DevAssert(opnd1_idx >= 0, ("missing OU_opnd1 for OP_sext/OP_zext"));
    if (opnd1_idx == opnd_idx) {
      *ext_bits = TOP_opnd_use_bits(opcode, opnd1_idx);
      *ext_signed = TOP_opnd_use_signed(opcode, opnd1_idx);
      return TRUE;
    }
  }

  if (OP_iand(op)) {
    opnd1_idx = TOP_Find_Operand_Use(opcode,OU_opnd1);
    opnd2_idx = TOP_Find_Operand_Use(opcode,OU_opnd2);
    DevAssert(opnd1_idx >= 0, ("missing OU_opnd1 for OP_sext/OP_zext"));
    DevAssert(opnd2_idx >= 0, ("missing OU_opnd2 for OP_sext/OP_zext"));
    if (opnd1_idx == opnd_idx &&
	TN_Has_Value(OP_opnd(op,opnd2_idx))) {
      val = TOP_fetch_opnd(opcode, op->res_opnd+OP_opnd_offset(op), opnd2_idx);
      INT32 use_bits = TOP_opnd_use_bits(opcode, opnd1_idx);
      INT32 bits = t1c(val);
      if (bits < use_bits) {
	*ext_bits = bits;
	*ext_signed = 0;
	return TRUE;
      }
    }
  }
  return FALSE;
#endif
  return FALSE;
}

/* =====================================================================
 * Get_Constant_Value
 * Returns a constant TN if the tninfo corresponds to a constsant value 
 * tn.
 * Return NULL otherwise.
 * =====================================================================
 */
static TN *
Get_Constant_Value(EBO_TN_INFO *tninfo)
{
  TN *const_tn = NULL;
  TN *tn;
  
  if (tninfo->replacement_tn != NULL)
    tn = tninfo->replacement_tn;
  else 
    tn = tninfo->local_tn;
  
  if (TN_Has_Value(tn))
    const_tn = tn;
  else if (TN_is_rematerializable(tn)) {
    WN *remat = TN_remat(tn);
    if (WN_opcode (remat) == OPC_I4INTCONST ||
	WN_opcode (remat) == OPC_U4INTCONST) {
      const_tn = Gen_Literal_TN ((INT32) WN_const_val(remat), 4);
    }
  }
  return const_tn;
}

/* =====================================================================
 * Function: iadd_special_case
 *
 * Look for integer add instructions where the first operand is
 * relocated address that is at the top of the stack.  The add
 * is unnecessary.
 * =====================================================================
 */
static
BOOL
iadd_special_case (
  OP *op,
  TN **opnd_tn,
  EBO_TN_INFO **opnd_tninfo
)
{
  INT opnd1_idx, opnd2_idx, pred_idx;

  if (!OP_iadd(op)) return FALSE;
  if (OP_code(op) == TOP_spadjust) return FALSE;
  
  opnd1_idx = TOP_Find_Operand_Use(OP_code(op),OU_opnd1);
  opnd2_idx = TOP_Find_Operand_Use(OP_code(op),OU_opnd2);
  pred_idx = TOP_Find_Operand_Use(OP_code(op),OU_predicate);
  DevAssert(opnd2_idx >= 0, ("missing OU_opnd2 for OP_iadd"));
  DevAssert(opnd1_idx >= 0, ("missing OU_opnd1 for OP_iadd"));

  // Skip if predicated
  if (pred_idx != -1 && !(OP_opnd(op, pred_idx) == True_TN)) return FALSE;

  TN *tn1 = opnd_tn[opnd1_idx];
  if (TN_is_constant(tn1) &&
      !TN_has_spill(tn1) &&
      TN_is_symbol(tn1)) {
    ST *st = TN_var(tn1);
    ST *base_st;
    INT64 base_ofst;
    INT64 val = TN_offset(tn1);
    
    Base_Symbol_And_Offset (st, &base_st, &base_ofst);
    if (ST_on_stack(st) &&
        ((ST_sclass(st) == SCLASS_AUTO) ||
         (EBO_in_peep && (ST_sclass(st) == SCLASS_FORMAL)))) {
      val += base_ofst;
      if (val == 0) {
        OPS ops = OPS_EMPTY;
        if(TN_is_register(opnd_tn[opnd2_idx])) {
          // (cbr) Support for guards on false
          if (OP_has_predicate (op)) {
            int pred_index = OP_find_opnd_use(op, OU_predicate);
            EBO_Exp_COPY(OP_opnd(op, pred_index), OP_Pred_False(op, pred_index),
                         OP_cond_def(op), OP_result(op, 0), opnd_tn[opnd2_idx], &ops);
          }
          else
            EBO_Exp_COPY(NULL, false, false, OP_result(op, 0), opnd_tn[opnd2_idx], &ops);

        } else if(TN_has_value(opnd_tn[opnd2_idx]) ||
                  TN_is_symbol(opnd_tn[opnd2_idx])) {
          Expand_Immediate(OP_result(op, 0), opnd_tn[opnd2_idx], MTYPE_I4,
                           &ops);
        }
	
        OP_srcpos(OPS_first(&ops)) = OP_srcpos(op);
	if (EBO_in_loop) 
	  EBO_OPS_omega (&ops, opnd_tn[opnd2_idx], opnd_tninfo[opnd2_idx]);
	if (!EBO_Verify_Ops(&ops)) return FALSE;
        BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
	if (EBO_Trace_Optimization) 
	  fprintf(TFile,"Replace iadd with copy\n");
        return TRUE;
      }
    }
  }

  return FALSE;
}


/* =====================================================================
 *   Function: shr_shl_sequence
 *
 * Replace (shr[u] (shl x N) N) with sxth/zxth instruction for N
 * in 16/8.
 * =====================================================================
 */
static BOOL
shr_shl_sequence (
  OP *op,
  TN **opnd_tn,
  EBO_TN_INFO **opnd_tninfo
)
{
  TOP opcode = OP_code(op);
  INT opnd2_idx;
  INT opnd1_idx;
  INT pred_idx;
  INT l2_opnd2_idx;
  INT l2_opnd1_idx;
  INT l2_pred_idx;
  INT value;

#if 0

  if (!(OP_ishr(op) || OP_ishru(op)))
    return FALSE;

  opnd1_idx = TOP_Find_Operand_Use(OP_code(op),OU_opnd1);
  opnd2_idx = TOP_Find_Operand_Use(OP_code(op),OU_opnd2);
  pred_idx = TOP_Find_Operand_Use(OP_code(op),OU_predicate);
  DevAssert(opnd2_idx >= 0, ("missing OU_opnd2 for OP_ishr/OP_ishru"));
  DevAssert(opnd1_idx >= 0, ("missing OU_opnd1 for OP_ishr/OP_ishru"));
  DevAssert(pred_idx >= 0, ("missing OU_predicate for OP_ishr/OP_ishru"));

  if (!(TN_Has_Value(OP_opnd(op, opnd2_idx)) &&
	(value = TN_Value(OP_opnd(op,opnd2_idx))) &&
	(value == 8 || value == 16))) return FALSE;
  
  // Skip if predicated
  if (!(OP_opnd(op, pred_idx) == True_TN)) return FALSE;

  // Level 1 data:
  BB *bb = OP_bb(op);
  OP *l1_op = op;
  TN *l1_res = OP_result(l1_op, 0);

  EBO_TN_INFO *l1_tninfo0 = opnd_tninfo[opnd1_idx];

  if (l1_tninfo0 == NULL || l1_tninfo0->in_op == NULL) {
    return FALSE;
  }

  // Determine the inputs to the shr instruction:
  EBO_OP_INFO *l2_opinfo = locate_opinfo_entry(l1_tninfo0);
  if ((l2_opinfo == NULL) ||
      (l2_opinfo->in_op == NULL)) return FALSE;

  OP *l2_op = l2_opinfo->in_op;

  if (!OP_ishl(l2_op))
    return FALSE;
  l2_opnd1_idx = TOP_Find_Operand_Use(OP_code(l2_op),OU_opnd1);
  l2_opnd2_idx = TOP_Find_Operand_Use(OP_code(l2_op),OU_opnd2);
  l2_pred_idx = TOP_Find_Operand_Use(OP_code(l2_op),OU_opnd2);
  DevAssert(opnd2_idx >= 0, ("missing OU_opnd2 for OP_ishl"));
  DevAssert(opnd1_idx >= 0, ("missing OU_opnd1 for OP_ishl"));
  DevAssert(pred_idx >= 0, ("missing OU_predicate for OP_ishl"));

  if (!(TN_Has_Value(OP_opnd(l2_op, l2_opnd2_idx)) &&
	(value == TN_Value(OP_opnd(l2_op,l2_opnd2_idx)))))
    return FALSE;

  // Skip if predicated
  if (!(OP_opnd(l2_op, l2_pred_idx) == True_TN)) return FALSE;

  EBO_TN_INFO *l2_tninfo0 = l2_opinfo->actual_opnd[l2_opnd1_idx];
  if (!EBO_tn_available (bb, l2_tninfo0))
    return FALSE;

  // (cbr) Check they are defined under the same predicate.
  if (!Opnds_Are_Equivalent (l1_op, l2_op,
                             OP_find_opnd_use(l1_op, OU_predicate),
                             OP_find_opnd_use(l2_op, OU_predicate)))
    return FALSE;

  TN *l2_tn0 = OP_opnd(l2_op, l2_opnd1_idx);

  // Determine new opcode:
  TOP new_opcode;
  new_opcode = TOP_is_unsign(opcode) ? 
    (value == 8 ? TOP_extub : TOP_extuh):
    (value == 8 ? TOP_extb : TOP_exth);

  // Replace the current instruction:
  OP *new_op;
  new_op = Mk_OP(new_opcode, l1_res, OP_opnd(op, pred_idx), l2_tn0);

  // (cbr) Support for guards on false
  CGTARG_Predicate_OP (NULL, new_op, OP_Predicate (op),
                       OP_Pred_False (op, OP_find_opnd_use(op, OU_predicate)));

  OP_srcpos(new_op) = OP_srcpos(op);
  if (EBO_in_loop) EBO_Set_OP_omega (new_op, l2_tninfo0);
  if (!EBO_Verify_Op(new_op)) return FALSE;
  BB_Insert_Op_After(bb, op, new_op);
  if (EBO_Trace_Optimization) 
    fprintf(TFile,"Convert shr16(shl16) to sxth/zxth\n");

#endif
  return TRUE;
}


static BOOL
find_def_opinfo(EBO_TN_INFO *input_tninfo, EBO_OP_INFO **def_opinfo)
{
  EBO_OP_INFO *opinfo;
  opinfo = locate_opinfo_entry(input_tninfo);
  if (opinfo == NULL || opinfo->in_op == NULL) return FALSE;
  *def_opinfo = opinfo;
  return TRUE;
}

static BOOL
op_match_integer_branch_copy(OP *op, EBO_TN_INFO **opnd_tninfo, TN **match_tn, EBO_TN_INFO **match_tninfo)
{
  TOP top = OP_code(op);
  EBO_TN_INFO *tninfo;
  TN *tn;
  INT opnd1_idx, opnd2_idx, pred_idx;

#if 0
  if (top == TOP_cmpne_r || top == TOP_cmpneu_r || 
      top == TOP_cmpne_i8 || top == TOP_cmpneu_i8) {
    opnd1_idx = TOP_Find_Operand_Use(top,OU_opnd1);
    opnd2_idx = TOP_Find_Operand_Use(top,OU_opnd2);
    pred_idx = TOP_Find_Operand_Use(top,OU_predicate);
    DevAssert(opnd1_idx >= 0 && opnd2_idx >= 0, ("OU_opnd1 and/or OU_opnd2 not defined for TOP %s\n", TOP_Name(top)));
    if (TN_Has_Value(OP_opnd(op, opnd2_idx)) && 
	TN_Value(OP_opnd(op, opnd2_idx)) == 0 &&
	(pred_idx < 0 || OP_opnd(op, pred_idx) == True_TN)) {
      tninfo = opnd_tninfo[opnd1_idx];
      tn = OP_opnd(op, opnd1_idx);
      goto matched;
    }
  }
  return FALSE;
  
 matched:
  if (tninfo != NULL && !EBO_tn_available (OP_bb(op), tninfo)) return FALSE;
  *match_tninfo = tninfo;
  *match_tn = tn;
  return TRUE;

#endif
  return FALSE;
}

static BOOL
op_match_integer_branch_not(OP *op, EBO_TN_INFO **opnd_tninfo, TN **match_tn, EBO_TN_INFO **match_tninfo)
{
  TOP top = OP_code(op);
  EBO_TN_INFO *tninfo;
  TN *tn;
  INT opnd1_idx, opnd2_idx, pred_idx;

#if 0
  if (top == TOP_cmpeq_r || top == TOP_cmpequ_r || 
      top == TOP_cmpeq_i8 || top == TOP_cmpequ_i8) {
    opnd1_idx = TOP_Find_Operand_Use(top,OU_opnd1);
    opnd2_idx = TOP_Find_Operand_Use(top,OU_opnd2);
    pred_idx = TOP_Find_Operand_Use(top,OU_predicate);
    DevAssert(opnd1_idx >= 0 && opnd2_idx >= 0, ("OU_opnd1 and/or OU_opnd2 not defined for TOP %s\n", TOP_Name(top)));
    if (TN_Has_Value(OP_opnd(op, opnd2_idx)) && 
	TN_Value(OP_opnd(op, opnd2_idx)) == 0 &&
	(pred_idx < 0 || OP_opnd(op, pred_idx) == True_TN)) {
      tninfo = opnd_tninfo[opnd1_idx];
      tn = OP_opnd(op, opnd1_idx);
      goto matched;
    }
  }
  return FALSE;
  
 matched:
  if (tninfo != NULL && !EBO_tn_available (OP_bb(op), tninfo)) return FALSE;
  *match_tninfo = tninfo;
  *match_tn = tn;
  return TRUE;
#endif
  return FALSE;
}

static BOOL
op_match_branch_integer_copy(OP *op, EBO_TN_INFO **opnd_tninfo, TN **match_tn, EBO_TN_INFO **match_tninfo)
{
  TOP top = OP_code(op);
  EBO_TN_INFO *tninfo;
  TN *tn;
  INT opnd1_idx, pred_idx;

#if 0  
  if (top == TOP_bool) {
    opnd1_idx = TOP_Find_Operand_Use(top,OU_opnd1);
    pred_idx = TOP_Find_Operand_Use(top,OU_predicate);
    DevAssert(opnd1_idx >= 0, ("OU_opnd1 not defined for TOP %s\n", TOP_Name(top)));
    if (pred_idx < 0 || OP_opnd(op, pred_idx) == True_TN) {
      tninfo = opnd_tninfo[opnd1_idx];
      tn = OP_opnd(op, opnd1_idx);
      goto matched;
    }
  }
  return FALSE;
  
 matched:
  if (tninfo != NULL && !EBO_tn_available (OP_bb(op), tninfo)) return FALSE;
  *match_tninfo = tninfo;
  *match_tn = tn;
  return TRUE;
#endif
  return FALSE;
}


static BOOL
op_match_branch_not(OP *op, EBO_TN_INFO **opnd_tninfo, TN **match_tn, EBO_TN_INFO **match_tninfo)
{
  TOP top = OP_code(op);
  EBO_TN_INFO *tninfo;
  TN *tn;
  INT opnd1_idx, opnd2_idx, pred_idx;

#if 0
  if (top == TOP_andng) {
    opnd1_idx = TOP_Find_Operand_Use(top,OU_opnd1);
    opnd2_idx = TOP_Find_Operand_Use(top,OU_opnd2);
    pred_idx = TOP_Find_Operand_Use(top,OU_predicate);
    DevAssert(opnd1_idx >= 0 && opnd2_idx >= 0, ("OU_opnd1 and/or OU_opnd2 not defined for TOP %s\n", TOP_Name(top)));
    if (OP_opnd(op, opnd2_idx) == True_TN &&
	(pred_idx < 0 || OP_opnd(op, pred_idx) == True_TN)) {
      tninfo = opnd_tninfo[opnd1_idx];
      tn = OP_opnd(op, opnd1_idx);
      goto matched;
    }
  }
  return FALSE;
  
 matched:
  if (tninfo != NULL && !EBO_tn_available (OP_bb(op), tninfo)) return FALSE;
  *match_tninfo = tninfo;
  *match_tn = tn;
  return TRUE;
#endif
  return FALSE;
}


static BOOL
op_match_lnot(OP *op, 
	     EBO_TN_INFO **opnd_tninfo, 
	     TN **op0_tn, 
	     EBO_TN_INFO **op0_tninfo)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  TOP top = OP_code(op);
  TN *opnd;

  if (top == TOP_cmpeq_r_b || top == TOP_cmpeq_r_r) {
    if (OP_opnd(op, 0) == Zero_TN) {
      *op0_tn = OP_opnd(op, 1);
      *op0_tninfo = opnd_tninfo[1];
      return TRUE;
    } else if (OP_opnd(op, 1) == Zero_TN) {
      *op0_tn = OP_opnd(op, 0);
      *op0_tninfo = opnd_tninfo[0];
      return TRUE;
    }
  }
  return FALSE;

#endif
  return FALSE;
}

static BOOL
op_match_lmove(OP *op, 
	       EBO_TN_INFO **opnd_tninfo, 
	       TN **op0_tn, 
	       EBO_TN_INFO **op0_tninfo)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  TOP top = OP_code(op);
  TN *opnd;

  if (top == TOP_cmpne_r_b || top == TOP_cmpne_r_r) {
    if (OP_opnd(op, 0) == Zero_TN) {
      *op0_tn = OP_opnd(op, 1);
      *op0_tninfo = opnd_tninfo[1];
      return TRUE;
    } else if (OP_opnd(op, 1) == Zero_TN) {
      *op0_tn = OP_opnd(op, 0);
      *op0_tninfo = opnd_tninfo[0];
      return TRUE;
    }
  }
  return FALSE;

#endif
  return FALSE;
}


static BOOL
is_same_bits(OP *op, EBO_TN_INFO **opnd_tninfo, int use_bits, TN **match_tn, EBO_TN_INFO **match_tninfo)
{
  TOP opcode = OP_code(op);
  int opnd1_idx = TOP_Find_Operand_Use(opcode,OU_opnd1);
  int opnd2_idx = TOP_Find_Operand_Use(opcode,OU_opnd2);
  EBO_TN_INFO *tninfo;
  TN *tn;

#if 0
  if (OP_sext(op) || OP_zext(op)) {
    int bits = TOP_opnd_use_bits(opcode, opnd1_idx); 
    if (bits >= use_bits) {
      tn = OP_opnd(op, opnd1_idx);
      tninfo = opnd_tninfo[opnd1_idx];
      goto matched;
    }
  }
  
  if (OP_iand(op)) {
    TN *opnd2_tn = OP_opnd(op, opnd2_idx);
    if (TN_Has_Value(opnd2_tn)) {
      INT64 val = TN_Value(opnd2_tn);
      INT64 bitmask = (UINT64)-1 >> 64-use_bits;
      if ((val & bitmask) == bitmask) {
	tn = OP_opnd(op, opnd1_idx);
	tninfo = opnd_tninfo[opnd1_idx];
	goto matched;
      }
    }
  }

  if (OP_ior(op) || OP_ixor(op)) {
    TN *opnd2_tn = OP_opnd(op, opnd2_idx);
    if (TN_Has_Value(opnd2_tn)) {
      INT64 val = TN_Value(opnd2_tn);
      INT64 bitmask = (UINT64)-1 >> 64-use_bits;
      if ((val ^ bitmask) == bitmask) {
	tn = OP_opnd(op, opnd1_idx);
	tninfo = opnd_tninfo[opnd1_idx];
	goto matched;
      }
    }
  }
  
  return FALSE;

 matched:
  if (tninfo != NULL && !EBO_tn_available (OP_bb(op), tninfo)) return FALSE;
  *match_tninfo = tninfo;
  *match_tn = tn;
  return TRUE;
#endif
  return FALSE;
}


static BOOL
find_equivalent_tn(OP *op, EBO_TN_INFO *input_tninfo, int use_bits, TN **equiv_tn, EBO_TN_INFO **equiv_tninfo)
{
  EBO_OP_INFO *def_opinfo;
  EBO_TN_INFO *match_tninfo;
  TN *match_tn;

  if (input_tninfo == NULL || input_tninfo->in_op == NULL) return FALSE;
  
   // Get defining op
  if (!find_def_opinfo(input_tninfo, &def_opinfo)) return FALSE;
  
  // Skip predicated definitions
  if (OP_cond_def(def_opinfo->in_op)) return FALSE;

  if (!is_same_bits(def_opinfo->in_op, def_opinfo->actual_opnd, use_bits, &match_tn, &match_tninfo))
    return FALSE;
  if (match_tninfo != NULL && !EBO_tn_available (OP_bb(op), match_tninfo)) return FALSE;
      
  *equiv_tninfo = match_tninfo;
  *equiv_tn = match_tn;
      
  return TRUE;
}


/*
 * operand_special_sequence.
 *
 * Try to discard useless operations defining an operand.
 * For instance sth(sxth(x)) -> sth(x)
 */
static BOOL
operand_special_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  TN *new_opnd_tn[OP_MAX_FIXED_OPNDS];
  int i;
  int replaced = 0;
  INT num_opnds = OP_opnds(op);
  INT same_res_opnd[OP_MAX_FIXED_OPNDS];

  /* ASM are not subject to this and must not, as the ASM_OP_Map must be preserved. */
  if (OP_code(op) == TOP_asm) return FALSE;

  if (num_opnds > OP_MAX_FIXED_OPNDS) return FALSE;

  // [JV] Don't know if there is a better solution to have information from operand idx
  // instead of result idx.
  for (i = 0; i < num_opnds; i++) {
    same_res_opnd[i] = -1;
  }

  for (i = 0; i < OP_results(op); i++) {
    INT same_opnd = -1;
    if((same_opnd = OP_same_res(op,i)) != -1) {
      if (EBO_Trace_Optimization) {
	fprintf(TFile,"Operand %d is same as result %d in OP:\n", i, same_opnd);
	Print_OP_No_SrcLine(op);
      }

      same_res_opnd[same_opnd] = i;
    }
  }


  for (i = 0; i < num_opnds; i++) {
    EBO_TN_INFO *tninfo, *new_tninfo;
    TN *new_tn;
    int use_bits;
    tninfo = opnd_tninfo[i];

    if (tninfo == NULL ||
	!use_bit_width(op, i, &use_bits) ||
	!find_equivalent_tn(op, tninfo, use_bits, &new_tn, &new_tninfo) ||
	!TN_is_register(new_tn) ||
	TN_register_class(new_tn) != TN_register_class(OP_opnd(op, i)) ||
	TN_is_dedicated(OP_opnd(op, i)) ||  /* Don't replace dedicated TNs. */
	same_res_opnd[i] != -1) { /* Don't replace TNs if there is a result identical to operand. */
      new_opnd_tn[i] = OP_opnd(op, i);
    } else {
      new_opnd_tn[i] = new_tn;
      replaced++;
    }
  }
  if (replaced == 0) return FALSE;

  /* Replace the current instruction operands. */
  OP *new_op;
  new_op = Dup_OP (op);
  OP_srcpos(new_op) = OP_srcpos(op);
  if (OP_memory(op)) Copy_WN_For_Memory_OP (new_op, op);
  if (EBO_in_loop) CG_LOOP_Init_Op(new_op);
  for (i = 0; i < num_opnds; i++) {
    Set_OP_opnd(new_op, i, new_opnd_tn[i]);
    if (EBO_in_loop) Set_OP_omega (new_op, i, OP_omega(op,i));
  }

  if (!EBO_Verify_Op(new_op)) return FALSE;
  BB_Insert_Op_After(OP_bb(op), op, new_op);

  if (EBO_Trace_Optimization) 
    fprintf(TFile,"Propagated equivalent tns into %s\n", TOP_Name(OP_code(new_op)));
  return TRUE;
}

static BOOL
op_match_select(OP *op, 
		EBO_TN_INFO **opnd_tninfo, 
		TN **op0_tn, 
		EBO_TN_INFO **op0_tninfo, 
		TN **op1_tn, 
		EBO_TN_INFO **op1_tninfo, 
		TN **op2_tn, 
		EBO_TN_INFO **op2_tninfo)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  TOP top = OP_code(op);
  VARIANT variant;

  if (!TOP_is_select(top)) return FALSE;

  variant = TOP_cond_variant(top);

  *op0_tn = OP_opnd(op, 0);
  *op0_tninfo = opnd_tninfo[0];
  *op1_tn = variant == V_COND_FALSE ? OP_opnd(op, 2): OP_opnd(op, 1);
  *op1_tninfo = variant == V_COND_FALSE ? opnd_tninfo[2] : opnd_tninfo[1];
  *op2_tn = variant == V_COND_FALSE ? OP_opnd(op, 1): OP_opnd(op, 2);
  *op2_tninfo = variant == V_COND_FALSE ? opnd_tninfo[1] : opnd_tninfo[2];
  return TRUE;

#endif
  return FALSE;
}

static BOOL
op_match_compare(OP *op, 
		 EBO_TN_INFO **opnd_tninfo, 
		 TN **op0_tn, 
		 EBO_TN_INFO **op0_tninfo, 
		 TN **op1_tn, 
		 EBO_TN_INFO **op1_tninfo)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  TOP top = OP_code(op);
  
  if (!TOP_is_cmp(top)) return FALSE;

  *op0_tn = OP_opnd(op, 0);
  *op0_tninfo = opnd_tninfo[0];
  *op1_tn = OP_opnd(op, 1);
  *op1_tninfo = opnd_tninfo[1];
  return TRUE;
#endif
  return FALSE;
}

/*
 * op_match_not:
 * matches negation
 */
static BOOL
op_match_neg(OP *op, 
	     EBO_TN_INFO **opnd_tninfo, 
	     TN **op0_tn, 
	     EBO_TN_INFO **op0_tninfo)
{
  TOP top = OP_code(op);
  TN *opnd;

#if 0
  if (top != TOP_neg) return FALSE;
  *op0_tn = OP_opnd(op, 1);
  *op0_tninfo = opnd_tninfo[1];
  return TRUE;
#endif
  return FALSE;
}

/*
 * op_match_not:
 * matches not
 */
static BOOL
op_match_not(OP *op, 
	     EBO_TN_INFO **opnd_tninfo, 
	     TN **op0_tn, 
	     EBO_TN_INFO **op0_tninfo)
{
  TOP top = OP_code(op);
  TN *opnd;

#if 0
  if (top != TOP_not) return FALSE;
  *op0_tn = OP_opnd(op, 1);
  *op0_tninfo = opnd_tninfo[0];
  return TRUE;
#endif
  return FALSE;
}


/*
 * op_match_abs:
 * matches abs
 */
static BOOL
op_match_abs(OP *op, 
	     EBO_TN_INFO **opnd_tninfo, 
	     TN **op0_tn, 
	     EBO_TN_INFO **op0_tninfo)
{
  TOP top = OP_code(op);
  TN *opnd;

#if 0
  if (top != TOP_fpx_fabs) return FALSE;
  *op0_tn = OP_opnd(op, 1);
  *op0_tninfo = opnd_tninfo[0];
  return TRUE;
#endif
  return FALSE;
}


static BOOL
op_match_unary(OP *op, 
	     EBO_TN_INFO **opnd_tninfo, 
	     TN **op0_tn, 
	     EBO_TN_INFO **op0_tninfo)
{
  TOP top = OP_code(op);
  TN *opnd;

  if (OP_results(op) != 1) return FALSE;
  if (OP_opnds(op) != 1) return FALSE;

  *op0_tn = OP_opnd(op, 0);
  *op0_tninfo = opnd_tninfo[0];
  return TRUE;
}

static BOOL
op_match_binary(OP *op, 
		EBO_TN_INFO **opnd_tninfo, 
		TN **op0_tn, 
		EBO_TN_INFO **op0_tninfo,
		TN **op1_tn, 
		EBO_TN_INFO **op1_tninfo)
{
  TOP top = OP_code(op);
  TN *opnd;

  if (OP_results(op) != 1) return FALSE;
  if (OP_opnds(op) != 2) return FALSE;

  *op0_tn = OP_opnd(op, 0);
  *op0_tninfo = opnd_tninfo[0];
  *op1_tn = OP_opnd(op, 1);
  *op1_tninfo = opnd_tninfo[1];
  return TRUE;
}

static BOOL
op_match_immediate(OP *op, 
		   EBO_TN_INFO **opnd_tninfo, 
		   TN **op0_tn, 
		   EBO_TN_INFO **op0_tninfo)
{
  INT opnd_idx = OP_Copy_Operand(op);
  if(opnd_idx == -1) { return FALSE; }

  *op0_tn = OP_opnd(op, opnd_idx);
  *op0_tninfo = opnd_tninfo[0];

  if(!TN_has_value(*op0_tn)) { return FALSE; }

  return TRUE;
}

static BOOL
op_match_shift(OP *op, 
	       EBO_TN_INFO **opnd_tninfo, 
	       TN **op0_tn, 
	       EBO_TN_INFO **op0_tninfo, 
	       TN **op1_tn, 
	       EBO_TN_INFO **op1_tninfo)
{
  if (!(OP_ishl(op) || OP_ishr(op) || OP_ishru(op))) return FALSE;

  *op0_tn = OP_opnd(op, OP_find_opnd_use(op,OU_opnd1));
  *op0_tninfo = opnd_tninfo[0];
  *op1_tn = OP_opnd(op, OP_find_opnd_use(op,OU_opnd2));
  *op1_tninfo = opnd_tninfo[1];

  if(!TN_has_value(*op1_tn)) {
    EBO_TN_INFO *limm_tninfo;
    TN *limm_tn;

    EBO_OP_INFO *def_opinfo;
    if(find_def_opinfo(opnd_tninfo[OP_find_opnd_use(op,OU_opnd2)], &def_opinfo) &&
       op_match_immediate(def_opinfo->in_op,
			  def_opinfo->actual_opnd,
			  &limm_tn,&limm_tninfo)) {
      *op1_tn = limm_tn;
      *op1_tninfo = limm_tninfo;
      return TRUE;
    }
    return FALSE;
  }

  return TRUE;
}

static BOOL
op_match_fsub(OP *op, 
	      EBO_TN_INFO **opnd_tninfo, 
	      TN **op0_tn, 
	      EBO_TN_INFO **op0_tninfo, 
	      TN **op1_tn, 
	      EBO_TN_INFO **op1_tninfo)
{
  TOP top = OP_code(op);
  
#if 0
  if (top != TOP_fpx_fsub) return FALSE;

  *op0_tn = OP_opnd(op, 1);
  *op0_tninfo = opnd_tninfo[0];
  *op1_tn = OP_opnd(op, 2);
  *op1_tninfo = opnd_tninfo[1];
  return TRUE;
#endif
  return FALSE;
}

static BOOL
op_match_fadd(OP *op, 
	      EBO_TN_INFO **opnd_tninfo, 
	      TN **op0_tn, 
	      EBO_TN_INFO **op0_tninfo, 
	      TN **op1_tn, 
	      EBO_TN_INFO **op1_tninfo)
{
  TOP top = OP_code(op);

#if 0  
  if (top != TOP_fpx_fadd) return FALSE;

  *op0_tn = OP_opnd(op, 1);
  *op0_tninfo = opnd_tninfo[0];
  *op1_tn = OP_opnd(op, 2);
  *op1_tninfo = opnd_tninfo[1];
  return TRUE;
#endif
  return FALSE;
}


/*
 * ext_move_sequence
 *
 * Replace useless extensions by move.
 * TODO: generic 
 */
static BOOL
ext_move_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  TOP opcode = OP_code(op);
  int opnd1_idx, opnd2_idx, pred_idx;
  INT32 use_bits, use_signed;
  INT32 def_bits, def_signed;
  int opnd_idx;

  opnd1_idx = TOP_Find_Operand_Use(opcode,OU_opnd1);
  opnd2_idx = TOP_Find_Operand_Use(opcode,OU_opnd2);
  pred_idx = TOP_Find_Operand_Use(opcode,OU_predicate);
  if (opnd1_idx < 0 || opnd2_idx < 0 || pred_idx < 0) return FALSE;
  
  if (!OP_is_extension(op, opnd1_idx, &use_bits, &use_signed)) return FALSE;
  
  OP *def_op;
  EBO_OP_INFO *def_opinfo;
  if (!find_def_opinfo(opnd_tninfo[opnd1_idx], &def_opinfo)) return FALSE;
  def_op = def_opinfo->in_op;

  if (OP_results(def_op) == 1 &&
      def_bit_width(def_op, 0, &def_bits, &def_signed)) {
    opnd_idx = opnd1_idx;
    if (def_signed == use_signed &&
	def_bits <= use_bits) goto matched;
    if (!def_signed && use_signed &&
	def_bits < use_bits) goto matched;
  }
  return FALSE;
 matched:
  if (OP_opnd(op,opnd_idx) != opnd_tn[opnd_idx]) return FALSE;
  OPS ops = OPS_EMPTY;

// (cbr) Support for guards on false
  if (OP_has_predicate(op)) {
    int pred_index = OP_find_opnd_use(op, OU_predicate);
    EBO_Exp_COPY(OP_opnd(op,pred_index), OP_Pred_False(op, pred_index),
                 OP_cond_def(op), OP_result(op, 0), opnd_tn[opnd_idx], &ops);
  }
  else
    Exp_COPY(OP_result(op, 0), opnd_tn[opnd_idx], &ops);

  OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
  if (EBO_in_loop) EBO_Set_OP_omega (OPS_last(&ops), opnd_tninfo[opnd_idx]);
  if (!EBO_Verify_Ops(&ops)) return FALSE;
  BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
  if (EBO_Trace_Optimization) 
    fprintf(TFile,"Convert ext to move\n");
  return TRUE;
}  

/*
 * branch_copy_op_sequence
 *
 * Convert:
 * - y = (cmpne (bool x) 0) into y = (mov x) (boolean mov)
 */
static BOOL
branch_copy_op_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  BB *bb = OP_bb(op);
  TOP top = OP_code(op);
  TN *def_tn, *copy_tn;
  EBO_TN_INFO *def_tninfo, *copy_tninfo;
  BOOL do_copy = FALSE;

  // Get source of integer->branch copy
  if (!op_match_integer_branch_copy(op, opnd_tninfo, &def_tn, &def_tninfo)) return FALSE;
  if (!EBO_tn_available (bb, def_tninfo)) return FALSE;

  EBO_OP_INFO *def2_opinfo;
  if (!find_def_opinfo(def_tninfo, &def2_opinfo)) return FALSE;

  // Case of copy 
  if (!op_match_branch_integer_copy(def2_opinfo->in_op, def2_opinfo->actual_opnd, &copy_tn, &copy_tninfo)) return FALSE;
  if (!EBO_tn_available (bb, copy_tninfo)) return FALSE;
  
  /* do the copy */
  {
    OPS ops = OPS_EMPTY;

    // (cbr) Support for guards on false
    if (OP_has_predicate(op)) {
      int pred_index = OP_find_opnd_use(op, OU_predicate);
      EBO_Exp_COPY(OP_opnd(op,pred_index), OP_Pred_False(op, pred_index),
                   OP_cond_def(op), OP_result(op, 0), copy_tn, &ops);
    }
    else
      Exp_COPY(OP_result(op,0), copy_tn, &ops);

    OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
    if (EBO_in_loop) EBO_OPS_omega (&ops, copy_tn, copy_tninfo);
    if (!EBO_Verify_Ops(&ops)) return FALSE;
    BB_Insert_Ops(bb, op, &ops, FALSE);
    if (EBO_Trace_Optimization) 
      fprintf(TFile,"Convert branch->integer->branch into branch copy\n");
  } 

  return TRUE;
}

/*
 * branch_not_op_sequence
 *
 * Convert:
 * - y = (cmpeq (bool x) 0) into y = (not x) (boolean not)
 */
static BOOL
branch_not_op_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  BB *bb = OP_bb(op);
  TOP top = OP_code(op);
  TN *def_tn, *copy_tn;
  EBO_TN_INFO *def_tninfo, *copy_tninfo;
  BOOL do_copy = FALSE;

#if 0
  // Get source of integer->branch not
  if (!op_match_integer_branch_not(op, opnd_tninfo, &def_tn, &def_tninfo)) return FALSE;
  if (!EBO_tn_available (bb, def_tninfo)) return FALSE;

  EBO_OP_INFO *def2_opinfo;
  if (!find_def_opinfo(def_tninfo, &def2_opinfo)) return FALSE;
  
  // Get the branch->integer copy
  if (!op_match_branch_integer_copy(def2_opinfo->in_op, def2_opinfo->actual_opnd, &copy_tn, &copy_tninfo)) return FALSE;
  if (!EBO_tn_available (bb, copy_tninfo)) return FALSE;
  
  /* do the not */
  {
    OPS ops = OPS_EMPTY;
    OPS_Append_Op(&ops, Mk_OP(TOP_andng, OP_result(op,0), True_TN, copy_tn, True_TN));
    // (cbr) Support for guards on false
    OPS_Copy_Predicate (&ops, op);

    OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
    if (EBO_in_loop) EBO_OPS_omega (&ops, copy_tn, copy_tninfo);
    if (!EBO_Verify_Ops(&ops)) return FALSE;
    BB_Insert_Ops(bb, op, &ops, FALSE);
    if (EBO_Trace_Optimization) 
      fprintf(TFile,"Convert branch->integer-> not branch into branch not\n");
  } 

  return TRUE;
#endif
  return FALSE;
}


static BOOL
integer_to_branch_op_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  if (branch_copy_op_sequence(op, opnd_tn, opnd_tninfo)) return TRUE;
  if (branch_not_op_sequence(op, opnd_tn, opnd_tninfo)) return TRUE;
  return FALSE;
}

/*
 * branch_condition_sequence.
 *
 * Replace branch by branch on false if the condition
 * matches a not.
 */
static BOOL
branch_condition_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  TOP opcode = OP_code(op);
  BB *bb = OP_bb(op);
  INT cond_idx;
  TN *not_tn;
  EBO_TN_INFO *not_tninfo;

  if (!OP_cond(op)) return FALSE;

  cond_idx = OP_find_opnd_use(op, OU_condition);
  DevAssert(cond_idx >= 0, ("OU_condition not defined for TOP %s\n", TOP_Name(opcode)));

  EBO_OP_INFO *cond_opinfo;
  if (!find_def_opinfo(opnd_tninfo[cond_idx], &cond_opinfo)) return FALSE;
  
  if (!op_match_branch_not(cond_opinfo->in_op, cond_opinfo->actual_opnd, &not_tn, &not_tninfo)) return FALSE;
  if (!EBO_tn_available (bb, not_tninfo)) return FALSE;
  
  /* Replace the current instruction by a branch on the invert condition. */
  OP *new_op;
  TOP new_opcode;
#if 0
  new_opcode = CGTARG_Invert(opcode);
  if (new_opcode == TOP_UNDEFINED) return FALSE;
  new_op = Dup_OP (op);
  OP_Change_Opcode(new_op, new_opcode);
#else
  new_op = CGTARG_Invert_OP(op);
  new_opcode = opcode;
#endif

  Set_OP_opnd(new_op, cond_idx, not_tn);
  OP_srcpos(new_op) = OP_srcpos(op);
  if (OP_memory(op)) Copy_WN_For_Memory_OP (new_op, op);
  if (!EBO_Verify_Op(new_op)) return FALSE;
  BB_Insert_Op_After(OP_bb(op), op, new_op);
  if (EBO_Trace_Optimization) 
    fprintf(TFile,"Inverted branch %s into %s\n", TOP_Name(opcode), TOP_Name(new_opcode));

  return TRUE;
}


/*
 * mfb_op_sequence
 *
 * Convert (mfb (op)) into an op defining an integer register
 */
static BOOL
mfb_op_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  TOP top = OP_code(op);
  TN *lhs_tn, *rhs_tn;
  EBO_TN_INFO *lhs_tninfo, *rhs_tninfo;
  EBO_OP_INFO *def_opinfo;

  if (top != TOP_mfb) return FALSE;
  if (!find_def_opinfo(opnd_tninfo[0], &def_opinfo)) return FALSE;
  if (op_match_unary(def_opinfo->in_op,
		     def_opinfo->actual_opnd, 
		     &lhs_tn, &lhs_tninfo)) {
    rhs_tn = NULL;
    rhs_tninfo = NULL;
  } else if (op_match_binary(def_opinfo->in_op,
			     def_opinfo->actual_opnd, 
			     &lhs_tn, &lhs_tninfo, &rhs_tn, &rhs_tninfo)) {
  } else return FALSE;
  
  TOP new_top;
  if (OP_code(def_opinfo->in_op) == TOP_mtb) {
    // This is a register normalization
    new_top = TOP_cmpne_r_r;
    rhs_tn = Zero_TN;
    rhs_tninfo = NULL;
  } else {
    new_top = TOP_result_register_variant(OP_code(def_opinfo->in_op), 0, ISA_REGISTER_CLASS_integer);
    if (new_top == TOP_UNDEFINED) return FALSE;
  }

  BB *bb = OP_bb(op);
  if ((lhs_tninfo != NULL && !EBO_tn_available (bb, lhs_tninfo)) ||
      (rhs_tninfo != NULL && !EBO_tn_available (bb, rhs_tninfo)))
    return FALSE;

  OP *new_op;
  new_op = Mk_OP(new_top, OP_result(op,0), lhs_tn, rhs_tn);

  // (cbr) Support for guards on false
  CGTARG_Predicate_OP (NULL, new_op, OP_Predicate (op),
                       OP_Pred_False (op, OP_find_opnd_use(op, OU_predicate)));

  OP_srcpos(new_op) = OP_srcpos(op);
  if (EBO_in_loop) EBO_Set_OP_omega (new_op, lhs_tninfo, rhs_tninfo);
  if (!EBO_Verify_Op(new_op)) return FALSE;
  BB_Insert_Op_After(bb, op, new_op);
  if (EBO_Trace_Optimization) 
    fprintf(TFile,"Convert mfb/op into %s\n", TOP_Name(new_top));
  return TRUE;

#endif
  return FALSE;
}

/*
 * and_or_sequence
 *
 * Convert :
 * - (and (not a) b) into (andc a b)
 * - (or (not a) b) into (orc a b)
 * TODO: generic?
 */
static BOOL
and_or_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  TOP top = OP_code(op);
  TN *not_tn, *lhs_tn, *rhs_tn;
  EBO_TN_INFO *not_tninfo, *lhs_tninfo, *rhs_tninfo;
  EBO_OP_INFO *def_opinfo;
  INT opnd1_idx, opnd2_idx, pred_idx;

#if 0
  if (!OP_iand(op) && !OP_ior(op)) return FALSE;

  opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
  opnd2_idx = OP_find_opnd_use(op, OU_opnd2);
  DevAssert(opnd1_idx >= 0 && opnd2_idx >= 0, ("OU_opnd1 and/or OU_opnd2 not defined for TOP %s\n", TOP_Name(top)));
  pred_idx = OP_find_opnd_use(op, OU_predicate);
  DevAssert(pred_idx >= 0, ("OU_predicate not defined for TOP %s\n", TOP_Name(top)));
  
  // Skip if predicated
  if (OP_opnd(op, pred_idx) != True_TN) return FALSE;

  if (find_def_opinfo(opnd_tninfo[opnd1_idx], &def_opinfo) &&
      op_match_not(def_opinfo->in_op,
		   def_opinfo->actual_opnd, 
		   &not_tn, &not_tninfo)) {
    lhs_tn = not_tn;
    lhs_tninfo = not_tninfo;
    rhs_tn = opnd_tn[opnd2_idx];
    rhs_tninfo = opnd_tninfo[opnd2_idx];
    goto replace;
  } 
  if (find_def_opinfo(opnd_tninfo[opnd2_idx], &def_opinfo) &&
      op_match_not(def_opinfo->in_op,
		   def_opinfo->actual_opnd, 
		   &not_tn, &not_tninfo)) {
    lhs_tn = not_tn;
    lhs_tninfo = not_tninfo;
    rhs_tn = opnd_tn[opnd1_idx];
    rhs_tninfo = opnd_tninfo[opnd1_idx];
    
    goto replace;
  } 
  return FALSE;
 replace:
  BB *bb = OP_bb(op);
  TOP new_top;
  if ((lhs_tninfo != NULL && !EBO_tn_available (bb, lhs_tninfo)) ||
      (rhs_tninfo != NULL && !EBO_tn_available (bb, rhs_tninfo)))
    return FALSE;

  // Find new top
  if (OP_iand(op)) new_top = TOP_andn;
  if (OP_ior(op)) new_top = TOP_UNDEFINED; // STxP70 does not have TOP_orn
  if (new_top == TOP_UNDEFINED) return FALSE;
  if (TN_is_symbol(lhs_tn)) return FALSE;
  if (TN_has_value(lhs_tn))
    new_top = TOP_opnd_immediate_variant(new_top, opnd1_idx, TN_value(lhs_tn));
  if (new_top == TOP_UNDEFINED) return FALSE;
  if (TN_is_symbol(rhs_tn)) return FALSE;
  if (TN_has_value(rhs_tn))
    new_top = TOP_opnd_immediate_variant(new_top, opnd2_idx, TN_value(rhs_tn));
  if (new_top == TOP_UNDEFINED) return FALSE;
  
		    
  OP *new_op;
  new_op = Mk_OP(new_top, OP_result(op,0), OP_opnd(op, pred_idx), lhs_tn, rhs_tn);

  // (cbr) Support for guards on false
  CGTARG_Predicate_OP (NULL, new_op, OP_Predicate (op),
                       OP_Pred_False (op, OP_find_opnd_use(op, OU_predicate)));

  OP_srcpos(new_op) = OP_srcpos(op);
  if (EBO_in_loop) EBO_Set_OP_omega (new_op, lhs_tninfo, rhs_tninfo);
  if (!EBO_Verify_Op(new_op)) return FALSE;
  BB_Insert_Op_After(bb, op, new_op);
  if (EBO_Trace_Optimization) {
    if (OP_iand(op))
      fprintf(TFile,"Convert (and (not a) b) into %s\n", TOP_Name(new_top));
    if (OP_ior(op))
      fprintf(TFile,"Convert (or (not a) b) into %s\n", TOP_Name(new_top));
  }
  return TRUE;
#endif
  return FALSE;
}

/*
 * shift_op_sequence
 *
 * Convert for example:
 * - (add a (shift b c)) into (addshift a b c)
 */
static BOOL
shift_op_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  TOP top = OP_code(op);
  TN *lshift_tn, *rshift_tn;
  TN *lhs_tn = NULL, *rhs_tn = NULL;
  EBO_TN_INFO *lshift_tninfo, *rshift_tninfo;
  EBO_TN_INFO *lhs_tninfo, *rhs_tninfo;
  EBO_OP_INFO *def_opinfo1, *def_opinfo2;
  INT opnd1_idx, opnd2_idx, pred_idx;
  INT match_opnd_idx = -1;
  BOOL opnd_swapped;

  opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
  opnd2_idx = OP_find_opnd_use(op, OU_opnd2);
  pred_idx = OP_find_opnd_use(op, OU_predicate);
  
  // Skip if predicated
  if (pred_idx >= 0 && OP_opnd(op, pred_idx) != True_TN) return FALSE;
  if(opnd1_idx == -1 || opnd2_idx == -1) { return FALSE; }

  if (find_def_opinfo(opnd_tninfo[opnd1_idx], &def_opinfo1) &&
      op_match_shift(def_opinfo1->in_op,
		     def_opinfo1->actual_opnd,
		     &lshift_tn, &lshift_tninfo,&rshift_tn, &rshift_tninfo)) {
    lhs_tn = lshift_tn;
    lhs_tninfo = lshift_tninfo;
    rhs_tn = rshift_tn;
    rhs_tninfo = rshift_tninfo;
    match_opnd_idx = 1;
  }
  /* If find shift on opnd1 and opnd2: prefer opnd2.*/
  if (find_def_opinfo(opnd_tninfo[opnd2_idx], &def_opinfo2) &&
      op_match_shift(def_opinfo2->in_op,
		     def_opinfo2->actual_opnd, 
		     &lshift_tn, &lshift_tninfo,&rshift_tn, &rshift_tninfo)) {

    lhs_tn = lshift_tn;
    lhs_tninfo = lshift_tninfo;
    rhs_tn = rshift_tn;
    rhs_tninfo = rshift_tninfo;
    match_opnd_idx = 2;
  } 
  if(match_opnd_idx != -1) { goto replace; }
  return FALSE;
 replace:
  BB *bb = OP_bb(op);
  TOP new_top;
  if ((lhs_tninfo != NULL && !EBO_tn_available (bb, lhs_tninfo)) ||
      (rhs_tninfo != NULL && !EBO_tn_available (bb, rhs_tninfo)))
    return FALSE;
  INT operand_prop = OU_opnd1;
  EBO_OP_INFO *def_opinfo = match_opnd_idx == 1 ? def_opinfo1 : def_opinfo2;

  // Find new top
  new_top = TOP_Combine_Opcodes(OP_code(op), OP_code(def_opinfo->in_op),
				match_opnd_idx == 1 ? opnd1_idx : opnd2_idx, &opnd_swapped);
  if (new_top == TOP_UNDEFINED) return FALSE;
  if(opnd_swapped) {
    TOP swapped_top = OP_opnd_swapped_variant(op, opnd1_idx, opnd2_idx);
    if(swapped_top == TOP_UNDEFINED) { return FALSE; }
    new_top = TOP_Combine_Opcodes(swapped_top,OP_code(def_opinfo->in_op), match_opnd_idx == 1 ? opnd2_idx : opnd1_idx, &opnd_swapped);
    if(new_top == TOP_UNDEFINED) { return FALSE; }
    if(opnd_swapped) { return FALSE; }
    operand_prop = OU_opnd2;
  }
  if (TN_is_symbol(lhs_tn)) return FALSE;
  if (TN_has_value(lhs_tn))
    new_top = TOP_opnd_immediate_variant(new_top, 1, TN_value(lhs_tn));
  if (new_top == TOP_UNDEFINED) return FALSE;
  if (TN_is_symbol(rhs_tn)) return FALSE;
  if (TN_has_value(rhs_tn))
    new_top = TOP_opnd_immediate_variant(new_top, 2, TN_value(rhs_tn));
  if (new_top == TOP_UNDEFINED) return FALSE;
  
		    
  OP *new_op;
  new_op = Mk_OP(new_top, OP_result(op,0), OP_opnd(op, OP_find_opnd_use(op, operand_prop)), lhs_tn, rhs_tn);
  OP_srcpos(new_op) = OP_srcpos(op);
  /* [JV] TODO: Fix omega with at least 3 operands. */
  if (EBO_in_loop) EBO_Set_OP_omega (new_op, lhs_tninfo, rhs_tninfo);
  if (!EBO_Verify_Op(new_op)) return FALSE;
  BB_Insert_Op_After(bb, op, new_op);
  if (EBO_Trace_Optimization) {
    fprintf(TFile,"%s returns %s\n", __FUNCTION__, TOP_Name(new_top));
  }
  return TRUE;
}

/*
 * float_add_sub_sequence
 *
 * Convert :
 * - (abs (sub a b)) into (asub a b)
 * - (add ((abs a) (abs b))) into (addaa a b)
 * - (neg (add a b)) into (addn a b))
 * TODO: generic?
 */
static BOOL
float_add_sub_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  TOP top = OP_code(op);
  TN *abs_tn, *lhs_tn, *rhs_tn;
  EBO_TN_INFO *abs_tninfo, *lhs_tninfo, *rhs_tninfo;
  EBO_OP_INFO *def_opinfo;
  INT opnd1_idx, opnd2_idx, pred_idx;

#if 0
  if (!OP_fadd(op) && !OP_fsub(op)) return FALSE;

  opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
  opnd2_idx = OP_find_opnd_use(op, OU_opnd2);
  DevAssert(opnd1_idx >= 0 && opnd2_idx >= 0, ("OU_opnd1 and/or OU_opnd2 not defined for TOP %s\n", TOP_Name(top)));
  pred_idx = OP_find_opnd_use(op, OU_predicate);
  DevAssert(pred_idx >= 0, ("OU_predicate not defined for TOP %s\n", TOP_Name(top)));
  
  // Skip if predicated
  if (OP_opnd(op, pred_idx) != True_TN) return FALSE;

  if (find_def_opinfo(opnd_tninfo[opnd1_idx], &def_opinfo) &&
      op_match_abs(def_opinfo->in_op,
		   def_opinfo->actual_opnd, 
		   &abs_tn, &abs_tninfo)) {
    lhs_tn = abs_tn;
    lhs_tninfo = abs_tninfo;
    
    if (find_def_opinfo(opnd_tninfo[opnd2_idx], &def_opinfo) &&
	op_match_abs(def_opinfo->in_op,
		     def_opinfo->actual_opnd, 
		     &abs_tn, &abs_tninfo)) {

      rhs_tn = abs_tn;
      rhs_tninfo = abs_tninfo;

      goto replace;
    }
  } 
  return FALSE;
 replace:
  BB *bb = OP_bb(op);
  TOP new_top;
  if ((lhs_tninfo != NULL && !EBO_tn_available (bb, lhs_tninfo)) ||
      (rhs_tninfo != NULL && !EBO_tn_available (bb, rhs_tninfo)))
    return FALSE;

  // Find new top
  if (OP_fadd(op)) new_top = TOP_fpx_faddaa;
  if (OP_fsub(op)) new_top = TOP_UNDEFINED; // STxP70 does not have TOP_orn
  if (new_top == TOP_UNDEFINED) return FALSE;
  if (TN_is_symbol(lhs_tn)) return FALSE;
  if (TN_has_value(lhs_tn))
    new_top = TOP_opnd_immediate_variant(new_top, opnd1_idx, TN_value(lhs_tn));
  if (new_top == TOP_UNDEFINED) return FALSE;
  if (TN_is_symbol(rhs_tn)) return FALSE;
  if (TN_has_value(rhs_tn))
    new_top = TOP_opnd_immediate_variant(new_top, opnd2_idx, TN_value(rhs_tn));
  if (new_top == TOP_UNDEFINED) return FALSE;
  
		    
  OP *new_op;
  new_op = Mk_OP(new_top, OP_result(op,0), OP_result(op,1), OP_opnd(op, pred_idx), lhs_tn, rhs_tn);

  // (cbr) Support for guards on false
  CGTARG_Predicate_OP (NULL, new_op, OP_Predicate (op),
                       OP_Pred_False (op, OP_find_opnd_use(op, OU_predicate)));

  OP_srcpos(new_op) = OP_srcpos(op);
  if (EBO_in_loop) EBO_Set_OP_omega (new_op, lhs_tninfo, rhs_tninfo);
  if (!EBO_Verify_Op(new_op)) return FALSE;
  BB_Insert_Op_After(bb, op, new_op);
  if (EBO_Trace_Optimization) {
    if (OP_fadd(op))
      fprintf(TFile,"Convert (fadd ((abs a) (abs b))) into %s\n", TOP_Name(new_top));
    if (OP_fsub(op))
      fprintf(TFile,"Convert (abs (sub a b)) into %s\n", TOP_Name(new_top));
  }
  return TRUE;
#endif
  return FALSE;
}

/*
 * float_abs_sequence
 *
 * Convert (abs (sub a b)) into asub
 */
static BOOL
float_abs_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  TOP top = OP_code(op);
  TN *lhs_tn, *rhs_tn;
  EBO_TN_INFO *lhs_tninfo, *rhs_tninfo;
  EBO_OP_INFO *def_opinfo;
  INT opnd1_idx, opnd2_idx, pred_idx;

#if 0
  if (top != TOP_fpx_fabs) return FALSE;

  opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
  DevAssert(opnd1_idx >= 0, ("OU_opnd1 not defined for TOP %s\n", TOP_Name(top)));
  pred_idx = OP_find_opnd_use(op, OU_predicate);
  DevAssert(pred_idx >= 0, ("OU_predicate not defined for TOP %s\n", TOP_Name(top)));

  if (!find_def_opinfo(opnd_tninfo[opnd1_idx], &def_opinfo)) return FALSE;
  if (op_match_fsub(def_opinfo->in_op,
		    def_opinfo->actual_opnd, 
		    &lhs_tn, &lhs_tninfo, &rhs_tn, &rhs_tninfo)) {
    goto replace;
  }
  return FALSE;

 replace:
  BB *bb = OP_bb(op);
  TOP new_top = TOP_fpx_fasub;
  if ((lhs_tninfo != NULL && !EBO_tn_available (bb, lhs_tninfo)) ||
      (rhs_tninfo != NULL && !EBO_tn_available (bb, rhs_tninfo)))
    return FALSE;


  if (TN_is_symbol(lhs_tn)) return FALSE;
  if (TN_has_value(lhs_tn)) return FALSE;
  if (TN_is_symbol(rhs_tn)) return FALSE;
  if (TN_has_value(rhs_tn)) return FALSE;
		    
  OP *new_op;
  new_op = Mk_OP(new_top, OP_result(op,0), OP_result(op,1), OP_opnd(op, pred_idx), lhs_tn, rhs_tn);

  // (cbr) Support for guards on false
  CGTARG_Predicate_OP (NULL, new_op, OP_Predicate (op),
                       OP_Pred_False (op, OP_find_opnd_use(op, OU_predicate)));

  OP_srcpos(new_op) = OP_srcpos(op);
  if (EBO_in_loop) EBO_Set_OP_omega (new_op, lhs_tninfo, rhs_tninfo);
  if (!EBO_Verify_Op(new_op)) return FALSE;
  BB_Insert_Op_After(bb, op, new_op);
  if (EBO_Trace_Optimization) {
    if (top == TOP_fpx_fabs)
      fprintf(TFile,"Convert (fabs (sub a b)) into %s\n", TOP_Name(new_top));
  }
  return TRUE;
#endif
  return FALSE;
}

/*
 * float_neg_sequence
 *
 * Convert (neg (add a b)) into addn
 */
static BOOL
float_neg_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  TOP top = OP_code(op);
  TN *lhs_tn, *rhs_tn;
  EBO_TN_INFO *lhs_tninfo, *rhs_tninfo;
  EBO_OP_INFO *def_opinfo;
  INT opnd1_idx, opnd2_idx, pred_idx;

#if 0
  if (top != TOP_fpx_fneg) return FALSE;

  opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
  DevAssert(opnd1_idx >= 0, ("OU_opnd1 not defined for TOP %s\n", TOP_Name(top)));
  pred_idx = OP_find_opnd_use(op, OU_predicate);
  DevAssert(pred_idx >= 0, ("OU_predicate not defined for TOP %s\n", TOP_Name(top)));

  if (!find_def_opinfo(opnd_tninfo[opnd1_idx], &def_opinfo)) return FALSE;
  if (op_match_fadd(def_opinfo->in_op,
		    def_opinfo->actual_opnd, 
		    &lhs_tn, &lhs_tninfo, &rhs_tn, &rhs_tninfo)) {
    goto replace;
  }

  return FALSE;

 replace:
  BB *bb = OP_bb(op);
  TOP new_top = TOP_fpx_faddn;

  if ((lhs_tninfo != NULL && !EBO_tn_available (bb, lhs_tninfo)) ||
      (rhs_tninfo != NULL && !EBO_tn_available (bb, rhs_tninfo)))
    return FALSE;


  if (TN_is_symbol(lhs_tn)) return FALSE;
  if (TN_has_value(lhs_tn)) return FALSE;
  if (TN_is_symbol(rhs_tn)) return FALSE;
  if (TN_has_value(rhs_tn)) return FALSE;
		    
  OP *new_op;
  new_op = Mk_OP(new_top, OP_result(def_opinfo->in_op,0), OP_result(op,0), OP_opnd(op, pred_idx), lhs_tn, rhs_tn);

  // (cbr) Support for guards on false
  CGTARG_Predicate_OP (NULL, new_op, OP_Predicate (op),
                       OP_Pred_False (op, OP_find_opnd_use(op, OU_predicate)));

  OP_srcpos(new_op) = OP_srcpos(op);
  if (EBO_in_loop) EBO_Set_OP_omega (new_op, lhs_tninfo, rhs_tninfo);
  if (!EBO_Verify_Op(new_op)) return FALSE;
  BB_Insert_Op_After(bb, op, new_op);
  if (EBO_Trace_Optimization) {
    if (top == TOP_fpx_fneg)
      fprintf(TFile,"Convert (fneg (add a b)) into %s\n", TOP_Name(new_top));
  }
  return TRUE;
#endif
  return FALSE;
}

/*
 * andl_orl_sequence
 *
 * Convert :
 * - (andl (notl a) (notl b)) into (norl a b)
 * - (orl (notl a) (notl b)) into (nandl a b)
 * - (nandl (notl a) (notl b)) into (orl a b)
 * - (norl (notl a) (notl b)) into (andl a b)
 * - (andl a a) into (movl a)
 * - (orl a a) into (movl a)
 * - (nandl a a) into (notl a)
 * - (norl a a) into (notl a)
 */
static BOOL
andl_orl_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  TOP top = OP_code(op);
  TN *l1_tn, *l2_tn, *lhs_tn, *rhs_tn;
  EBO_TN_INFO *l1_tninfo, *l2_tninfo, *lhs_tninfo, *rhs_tninfo;
  EBO_OP_INFO *def_opinfo;
  BB *bb = OP_bb(op);

  if (!OP_icmp(op)) return FALSE;

  if (!op_match_compare(op, 
			opnd_tninfo, 
			&lhs_tn, &lhs_tninfo, &rhs_tn, &rhs_tninfo))
    return FALSE;
  
  VARIANT variant = TOP_cmp_variant(top);
  if (variant != V_CMP_ORL &&
      variant != V_CMP_NORL &&
      variant != V_CMP_ANDL &&
      variant != V_CMP_NANDL) return FALSE;

  if (EBO_Trace_Optimization) 
    fprintf(TFile,"Enter andl_orl_sequence\n");
  

  VARIANT new_variant = variant;
  lhs_tn = opnd_tn[0];
  lhs_tninfo = opnd_tninfo[0];
  rhs_tn = opnd_tn[1];
  rhs_tninfo = opnd_tninfo[1];
  
  if (find_def_opinfo(lhs_tninfo, &def_opinfo)) {
    if (op_match_lmove(def_opinfo->in_op,
		       def_opinfo->actual_opnd, 
		       &l1_tn, &l1_tninfo)) {
      lhs_tn = l1_tn;
      lhs_tninfo = l1_tninfo;
    }
  }
  if (find_def_opinfo(rhs_tninfo, &def_opinfo)) {
    if (op_match_lmove(def_opinfo->in_op,
		       def_opinfo->actual_opnd, 
		       &l1_tn, &l1_tninfo)) {
      rhs_tn = l1_tn;
      rhs_tninfo = l1_tninfo;
    }
  } 

  if (lhs_tn == NULL || rhs_tn == NULL) return FALSE;
  
  if (!TN_is_register(lhs_tn) || !TN_is_register(rhs_tn)) return FALSE;
  
  if ((lhs_tninfo != NULL && !EBO_tn_available (bb, lhs_tninfo)) ||
      (rhs_tninfo != NULL && !EBO_tn_available (bb, rhs_tninfo)))
    return FALSE;

  if (find_def_opinfo(lhs_tninfo, &def_opinfo) &&
      op_match_lnot(def_opinfo->in_op,
		    def_opinfo->actual_opnd, 
		    &l1_tn, &l1_tninfo) &&
      find_def_opinfo(rhs_tninfo, &def_opinfo) &&
      op_match_lnot(def_opinfo->in_op,
		    def_opinfo->actual_opnd, 
		    &l2_tn, &l2_tninfo)) {
    switch (new_variant) {
    case V_CMP_ORL: new_variant = V_CMP_NANDL; break;
    case V_CMP_ANDL: new_variant = V_CMP_NORL; break;
    case V_CMP_NORL: new_variant = V_CMP_ANDL; break;
    case V_CMP_NANDL: new_variant = V_CMP_ORL; break;
    }
    lhs_tn = l1_tn;
    lhs_tninfo = l1_tninfo;
    rhs_tn = l2_tn;
    rhs_tninfo = l2_tninfo;
  } 
  
  if (lhs_tn == NULL || rhs_tn == NULL) return FALSE;
  
  if (!TN_is_register(lhs_tn) || !TN_is_register(rhs_tn)) return FALSE;
  
  if ((lhs_tninfo != NULL && !EBO_tn_available (bb, lhs_tninfo)) ||
      (rhs_tninfo != NULL && !EBO_tn_available (bb, rhs_tninfo)))
    return FALSE;

  if (tn_registers_identical(lhs_tn, rhs_tn)) {
    if (EBO_Trace_Optimization) 
      fprintf(TFile, "Registers identical %d %d\n", TN_number(lhs_tn), TN_number(rhs_tn));
    BOOL invert = FALSE;
    switch (new_variant) {
    case V_CMP_NORL: invert = TRUE; break;
    case V_CMP_NANDL: invert = TRUE; break;
    }
    OPS ops = OPS_EMPTY;
    if (invert) {
      Expand_Logical_Not(OP_result(op, 0), lhs_tn, 0, &ops);
    } else {
      Expand_Logical_Or(OP_result(op, 0), lhs_tn, Zero_TN, 0, &ops);
    }

    // (cbr) Support for guards on false
    OPS_Copy_Predicate (&ops, op);

    OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
    if (EBO_in_loop) EBO_OPS_omega (&ops, lhs_tn, lhs_tninfo);
    if (!EBO_Verify_Ops(&ops)) return FALSE;
    BB_Insert_Ops_After(bb, op, &ops);
    if (EBO_Trace_Optimization) 
      fprintf(TFile,"Convert andl/orl into copy\n");
  } else {
    if (new_variant == variant &&
	lhs_tn == OP_opnd(op, 0) &&
	rhs_tn == OP_opnd(op, 1)) return FALSE;
    
    OP *new_op;
    TOP new_top = TOP_UNDEFINED;
    switch (new_variant) {
    case V_CMP_ANDL: new_top = TOP_andl_r_r; break;
    case V_CMP_ORL: new_top = TOP_orl_r_r; break;
    case V_CMP_NANDL: new_top = TOP_nandl_r_r; break;
    case V_CMP_NORL: new_top = TOP_norl_r_r; break;
    }
    if (new_top == TOP_UNDEFINED) return FALSE;
    
    TN *result;
    result = OP_result(op, 0);
    new_top = TOP_result_register_variant(new_top, 0, 
					  TN_register_class(result));
    new_op = Mk_OP(new_top, result, lhs_tn, rhs_tn);

    // (cbr) Support for guards on false
    CGTARG_Predicate_OP (NULL, new_op, OP_Predicate (op),
                         OP_Pred_False (op, OP_find_opnd_use(op, OU_predicate)));

    OP_srcpos(new_op) = OP_srcpos(op);
    if (EBO_in_loop) EBO_Set_OP_omega (new_op, lhs_tninfo, rhs_tninfo);
    OPS ops = OPS_EMPTY;
    OPS_Append_Op(&ops, new_op);
    if (!EBO_Verify_Ops(&ops)) return FALSE;
    BB_Insert_Ops_After(bb, op, &ops);
    if (EBO_Trace_Optimization) 
      fprintf(TFile,"Convert andl/orl into  %s\n", TOP_Name(new_top));
  }
  return TRUE;
#endif
  return FALSE;
}

/*
 * andl_orl_sequence_2
 *
 * Convert :
 * - (andl (andl x y) x) into (andl x y)
 */
static BOOL
andl_orl_sequence_2(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  BB *bb = OP_bb(op);
  TOP top = OP_code(op);
  TN *lhs_tn, *rhs_tn;
  EBO_TN_INFO  *lhs_tninfo, *rhs_tninfo;
  EBO_OP_INFO *def_opinfo;
  TN *l1_lhs_tn, *l1_rhs_tn;
  EBO_TN_INFO  *l1_lhs_tninfo, *l1_rhs_tninfo;
  TN *src_tn;
  EBO_TN_INFO *src_tninfo;

  if (!OP_icmp(op)) return FALSE;

  if (!op_match_compare(op, 
			opnd_tninfo, 
			&lhs_tn, &lhs_tninfo, &rhs_tn, &rhs_tninfo))
    return FALSE;
  
  VARIANT variant = TOP_cmp_variant(top);
  if (variant != V_CMP_ORL &&
      variant != V_CMP_ANDL) return FALSE;

  if (find_def_opinfo(lhs_tninfo, &def_opinfo) &&
      op_match_compare(def_opinfo->in_op,
		       def_opinfo->actual_opnd, 
		       &l1_lhs_tn, &l1_lhs_tninfo, &l1_rhs_tn, &l1_rhs_tninfo)) {
    VARIANT l1_variant = TOP_cmp_variant(OP_code(def_opinfo->in_op));
    if ((l1_lhs_tninfo == NULL || EBO_tn_available (bb, l1_lhs_tninfo)) &&
	(l1_rhs_tninfo == NULL || EBO_tn_available (bb, l1_rhs_tninfo))) {
      if (l1_lhs_tn == rhs_tn ||
	  l1_rhs_tn == rhs_tn) {
	if (variant == l1_variant) {
	  src_tn = lhs_tn;
	  src_tninfo = lhs_tninfo;
	  goto do_copy;
	}
      }
    }
  }
  
  if (find_def_opinfo(rhs_tninfo, &def_opinfo) &&
      op_match_compare(def_opinfo->in_op,
		       def_opinfo->actual_opnd, 
		       &l1_lhs_tn, &l1_lhs_tninfo, &l1_rhs_tn, &l1_rhs_tninfo)) {
    VARIANT l1_variant = TOP_cmp_variant(OP_code(def_opinfo->in_op));
    if ((l1_lhs_tninfo == NULL || EBO_tn_available (bb, l1_lhs_tninfo)) &&
	(l1_rhs_tninfo == NULL || EBO_tn_available (bb, l1_rhs_tninfo))) {
      if (l1_lhs_tn == lhs_tn ||
	  l1_rhs_tn == lhs_tn) {
	if (variant == l1_variant) {
	  src_tn = rhs_tn;
	  src_tninfo = rhs_tninfo;
	  goto do_copy;
	}
      }
    }
  }
  return FALSE;

 do_copy:  
  if (src_tninfo != NULL && !EBO_tn_available (bb, src_tninfo)) return FALSE;

  OPS ops = OPS_EMPTY;

  // (cbr) Support for guards on false
  if (OP_has_predicate(op)) {
    int pred_index = OP_find_opnd_use(op, OU_predicate);
    EBO_Exp_COPY(OP_opnd(op,pred_index), OP_Pred_False(op, pred_index),
                 OP_cond_def(op), result(op, 0), src_tn, &ops);
  }
  else
    Exp_COPY(result(op, 0), src_tn, &ops);

  OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
  if (EBO_in_loop) EBO_OPS_omega (&ops, src_tn, src_tninfo);
  if (!EBO_Verify_Ops(&ops)) return FALSE;
  BB_Insert_Ops_After(bb, op, &ops);
  if (EBO_Trace_Optimization) 
    fprintf(TFile,"Convert andl/orl into move\n");

  return TRUE;
#endif
  return FALSE;
}

static BOOL
base_offset_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  // TODO STxP70
  return FALSE;
#if 0
  TOP opcode, new_opcode;
  INT base_idx, offset_idx;
  TN *base_tn, *offset_tn;
  ST *base_sym = NULL;
  INT32 base_relocs;
  TN *new_offset_tn, *new_base_tn;
  INT64 offset_val, base_val;
  OP *new_op;

  base_idx = OP_find_opnd_use(op, OU_base);
  offset_idx = OP_find_opnd_use(op, OU_offset);
  if (base_idx < 0 && offset_idx < 0) return FALSE;

  base_tn = opnd_tn[base_idx];
  offset_tn = opnd_tn[offset_idx];

  if (!TN_Is_Constant(base_tn) || !TN_Is_Constant(offset_tn)) return FALSE;

  if (TN_is_symbol(base_tn)) {
    if (TN_is_symbol(offset_tn)) return FALSE;
    else offset_val = TN_Value(offset_tn);
    base_val = TN_offset(base_tn);
    base_sym = TN_var(base_tn);
    base_relocs = TN_relocs(base_tn);
  } else {
    if (TN_is_symbol(offset_tn)) {
      offset_val = TN_offset(offset_tn);
      base_sym = TN_var(offset_tn);
      base_relocs = TN_relocs(offset_tn);
    } else {
      offset_val = TN_Value(offset_tn);
    }
    base_val = TN_Value(base_tn);
  }
  
  if (base_sym != NULL) {
    new_offset_tn = Gen_Symbol_TN(base_sym, base_val + offset_val, base_relocs);
  } else {
    new_offset_tn = Gen_Literal_TN(base_val+offset_val, 4);
  }
  new_base_tn = Zero_TN;
  if (new_offset_tn == OP_opnd(op, offset_idx) && new_base_tn == OP_opnd(op, base_idx)) return FALSE;

  opcode = OP_code(op);
  if (TN_has_value(new_offset_tn)) {
    new_opcode = TOP_opnd_immediate_variant(opcode, offset_idx, TN_value(new_offset_tn));
  } else {
    new_opcode = TOP_opnd_immediate_variant(opcode, offset_idx, 0xFFFFFFFF);
  }
  if (new_opcode == TOP_UNDEFINED) return FALSE;
  
  new_op = Dup_OP(op);
  OP_srcpos(new_op) = OP_srcpos(op);
  OP_Change_Opcode(new_op, new_opcode);
  Set_OP_opnd(new_op, offset_idx, new_offset_tn);
  Set_OP_opnd(new_op, base_idx, new_base_tn);
  if (OP_memory(op)) Copy_WN_For_Memory_OP (new_op, op);
  if (!EBO_Verify_Op(new_op)) return FALSE;
  BB_Insert_Op_After(OP_bb(op), op, new_op);
  if (EBO_Trace_Optimization) 
    fprintf(TFile,"Convert memory op base+offset\n");
  
  return TRUE;
#endif
  return FALSE;
}

/* ============================================================
 * EBO_literal_replacement_tn
 *
 * Returns a literal replacement TN for operations that are
 * not copy operations.
 *
 * ST200: Treat equivalent of mtb of 0 and mtb of 1.
 * STxP70: TODO
 * ============================================================
 */
TN *
EBO_literal_replacement_tn( OP *op )
{
  return NULL;
#if 0
  TOP opcode = OP_code(op);
  TN *tn0 = NULL, *tn1 = NULL;
  INT64 tn0_val, tn1_val;
  INT64 *opnd_values = TYPE_ALLOCA_N(INT64,OP_opnds(op));
  INT64 result_val;
  BOOL opnds_has_values = TRUE;

  if (EBO_Trace_Optimization) {
    fprintf(TFile, "%s%s\n", EBO_trace_pfx,__FUNCTION__);
  }

  /* Do not managed multi results. */
  FmtAssert(result_idx < OP_results(op),("Bad result idx"));

  for(INT opnd_idx = 0; opnd_idx < OP_opnds(op); opnd_idx++) {
    TN *tn = OP_opnd(op,opnd_idx);
    if(TN_has_value(tn)) {
      opnd_values[opnd_idx] = TN_value(tn);
    }
    else if(TN_has_value(opnd_tn[opnd_idx])) {
      opnd_values[opnd_idx] = TN_value(opnd_tn[opnd_idx]);
    }
    else {
      opnds_has_values = FALSE;
      break;
    }
  }

  if(! opnds_has_values) {
    return NULL;
  }

  /* Force result 0 (case of unique result) */
  if(TOP_evaluate_top(op,opnd_values,&result_val,result_idx)) { goto Evaluated; }

  /* Nothing target specific to fold. */
  return NULL;

 Evaluated:

  if (EBO_Trace_Optimization) {
    fprintf(TFile, "%sreplaced???: %llx\n", EBO_trace_pfx, result_val);
  }

  return Gen_Literal_TN(result_val,8);
#endif
}

BOOL
EBO_Special_Inline_Immediates(OP *op, EBO_OP_INFO *opinfo, int idx)
{
  return FALSE;
}

static BOOL
operand_special_immediate(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
  TOP opcode = OP_code(op);
  TOP new_opcode;
  INT64 val;
  int i;
  for (i = 0; i < OP_opnds(op); i++) {
    if (!TN_Has_Value(OP_opnd(op,i)) && 
      opnd_tninfo[i] != NULL &&
      opnd_tninfo[i]->replacement_tn != NULL &&
      TN_has_value(opnd_tninfo[i]->replacement_tn)) {
      val = TN_value(opnd_tninfo[i]->replacement_tn);
      new_opcode = TOP_opnd_immediate_variant(opcode, i, val);
      if (new_opcode != TOP_UNDEFINED) goto replace;
    }
  }
  return FALSE;
  
 replace:
  OP *new_op = Dup_OP(op);
  OP_Change_Opcode(new_op, new_opcode);
  Set_OP_opnd(new_op, i, opnd_tninfo[i]->replacement_tn);
  if (OP_memory(op)) Copy_WN_For_Memory_OP (new_op, op);
  if (!EBO_Verify_Op(new_op)) return FALSE;
  BB_Insert_Op_After(OP_bb(op), op, new_op);

  if (EBO_Trace_Optimization) fprintf(TFile,"replace op %s with immediate form %s\n", TOP_Name(opcode), TOP_Name(new_opcode));
  return TRUE;
}

static bool
TNequiv(TN* tn1, TN* tn2)
{
    bool result = tn1 == tn2;

    if(!result && tn1 && tn2)
        {
            result = (TN_is_register(tn1) &&
                      TN_is_register(tn2) &&
                      TNs_Are_Equivalent(tn1, tn2)) ||
                (TN_is_zero(tn1) && TN_is_zero(tn2));
        }
    return result;
}

/* ============================================================
 * AutoMod_CheckCombine
 *
 * Check that a memory operation and an operation can be combined into
 * a memory op with an auto-modifying addressing mode.
 * - inc_op must be an unpredicated ADD or SUB operation
 * - res(inc_op, 0) and opnd(inc_op,0) must be equal to the base of
 *   the memory operation
 * - opnd(inc_op, 1) must be a literal value, or a register value that
 *   is not redefined between inc_op and mem_op
 * ============================================================
 */

static BOOL
AutoMod_CheckCombine(OP *mem_op, OP *inc_op) {

  // Can only associate operations from a same BB
  Is_True(OP_bb(mem_op) == OP_bb(inc_op), ("AutoMod_CheckCombine: Can only associate OPs in a basic block"));

  // Check this is a non-predicated, simple add or sub operation.
  if (!(OP_iadd(inc_op) || OP_isub(inc_op)) ||
      (OP_results(inc_op) != 1) ||
      (OP_has_predicate(inc_op) && (OP_opnd(inc_op, OP_find_opnd_use(inc_op, OU_predicate)) != True_TN)))
    return FALSE;

  INT opnd1_idx = OP_find_opnd_use(inc_op, OU_opnd1);
  INT opnd2_idx = OP_find_opnd_use(inc_op, OU_opnd2);

  if ((opnd1_idx == -1) || (opnd2_idx == -1))
    return FALSE;

  INT base_idx = OP_find_opnd_use(mem_op, OU_base);
  INT offset_idx = OP_find_opnd_use(mem_op, OU_offset);

  Is_True(base_idx >=0 && offset_idx >= 0, ("Internal error"));

  TN *base_tn = OP_opnd(mem_op, base_idx);
	  
  if (!TNequiv(OP_result(inc_op, 0), base_tn))
    return FALSE;

  // automod not mappable, since base and result of the mem operation
  // are the same, so either the modification of the base will be lost or
  // the load of the address
  if(OP_Precedes(mem_op, inc_op) && OP_results(mem_op) > 0 &&
     TNequiv(OP_result(mem_op, 0), OP_result(inc_op, 0))) {
      return FALSE;
  }

  if (OP_iadd(inc_op) && TNequiv(OP_opnd(inc_op, opnd2_idx), base_tn)) {
    int swap = opnd2_idx;
    opnd2_idx = opnd1_idx;
    opnd1_idx = swap;
  }
  else if (!TNequiv(OP_opnd(inc_op, opnd1_idx), base_tn))
    return FALSE;

  TN *adjust_tn = OP_opnd(inc_op, opnd2_idx);
  if (TN_is_register(adjust_tn)) {
    OP *from, *to;
    // Check adjust_tn is not defined between mem_op and inc_op
    from = OP_Precedes(inc_op, mem_op) ? inc_op : mem_op;
    to   = OP_Precedes(inc_op, mem_op) ? mem_op : inc_op;
    for (OP *op_iter = OP_next(from); op_iter != to; op_iter = OP_next(op_iter)) {
      for (INT idx = 0; idx < OP_results(op_iter); idx++) {
        if (TNequiv(OP_result(op_iter, idx), adjust_tn))
	  // adjust_tn is redefined
	  return FALSE;
      }
    }
  }
  else if (!TN_has_value(adjust_tn))
    return FALSE;

  return TRUE;
}

extern TOP TOP_AM_automod_variant(TOP top, BOOL post_mod, BOOL inc_mod, ISA_REGISTER_CLASS reg_class);


/* ============================================================
 * Build_AutoMod_OP()
 *
 * Generate an automodified variant of the given operation.
 * The new top and base/offset tns are given.
 * The old operation is necessary to map other operands/results.
 * ============================================================
 */
static void
Build_AutoMod_OP(TOP automod_top, TN *base, TN *offset, OP *old_op, OPS *ops)
{
  INT base_result_idx;
  base_result_idx = TOP_Find_Result_With_Usage (automod_top,OU_postincr);
  if (base_result_idx < 0) base_result_idx = TOP_Find_Result_With_Usage (automod_top,OU_preincr);
  FmtAssert(base_result_idx >= 0, ("Malformed automod top, missing result postincr/preincr"));
  FmtAssert(!TOP_is_var_opnds(automod_top), ("Malformed automod top, varopnd while it should not"));

  INT old_base_opnd_idx, old_offset_opnd_idx;
  old_base_opnd_idx = OP_find_opnd_use (old_op,OU_base);
  old_offset_opnd_idx = OP_find_opnd_use (old_op,OU_offset);
    
  TN **opnds = (TN**)alloca(sizeof(TN*)*TOP_fixed_opnds(automod_top));
  TN **results = (TN**)alloca(sizeof(TN*)*TOP_fixed_results(automod_top));
  INT old_idx = 0;
  INT i;
  for (i = 0; i < TOP_fixed_results(automod_top); i++) {
    if (i == base_result_idx) {
      results[i] = base;
    } else {
      results[i] = OP_result(old_op, old_idx);
      old_idx++;
    }
  }
  for (i = 0; i < TOP_fixed_opnds(automod_top); i++) {
    if (i == old_base_opnd_idx) opnds[i] = base;
    else if (i == old_offset_opnd_idx) opnds[i] = offset;
    else opnds[i] = OP_opnd(old_op, i);
  }
  OP  *op = Mk_VarOP(automod_top, TOP_fixed_results(automod_top), TOP_fixed_opnds(automod_top), results, opnds);
  Copy_WN_For_Memory_OP(op, old_op);

  // (cbr) Support for guards on false
  CGTARG_Predicate_OP (NULL, op, OP_Predicate (old_op),
                       OP_Pred_False (old_op, OP_find_opnd_use(old_op, OU_predicate)));

  OPS_Append_Op(ops, op);
}

/* ============================================================
 * AutoMod_Combine
 *
 * Generate a memory operation that can combine mem_op and inc_op
 * - Check a machine instruction exists
 * - Check immediate value can be represented
 * ============================================================
 */

static BOOL
AutoMod_Combine(OP *mem_op, OP *inc_op, OPS *ops) {

  TN *base_tn   = OP_opnd(mem_op, OP_find_opnd_use(mem_op, OU_base));
  TN *offset_tn = OP_opnd(mem_op, OP_find_opnd_use(mem_op, OU_offset));

  INT opnd1_idx = OP_find_opnd_use(inc_op, OU_opnd1);
  INT opnd2_idx = OP_find_opnd_use(inc_op, OU_opnd2);

  if (!TNequiv(OP_opnd(inc_op, opnd1_idx), base_tn)) {
    int swap = opnd2_idx;
    opnd2_idx = opnd1_idx;
    opnd1_idx = swap;
  }

  TN *adjust_tn = OP_opnd(inc_op, opnd2_idx);
  BOOL is_inc;
  INT64 adjust_val = 0;

  if (TN_has_value(adjust_tn)) {
    is_inc = (OP_iadd(inc_op) == (TN_value(adjust_tn) >= 0));
    adjust_val = OP_iadd(inc_op) ? TN_value(adjust_tn) : -TN_value(adjust_tn);
  }
  else {
    is_inc = OP_iadd(inc_op); // Assume mem_op is inc
  }

  // Now the possible sequences are
  //  1) base_tn += cst ; @0(base_tn) ;    --> @0(base_tn+=cst) ;
  //  2) base_tn += cst ; @-cst(base_tn) ; --> @0(base_tn=+cst) ;
  //  3) @0(base_tn) ; base_tn += cst ;    --> @0(base_tn=+cst) ;
  //  4) @cst(base_tn) ; base_tn += cst ;  --> @0(base_tn+=cst) ;

  //  5) base_tn += reg ; @0(base_tn) ;    --> @0(base_tn+=reg) ;
  //  6) @0(base_tn) ; base_tn += reg ;    --> @0(base_tn=+reg) ;
  //  7) base_tn -= reg ; @reg(base_tn) ;  --> @0(base_tn=-reg) ;
  //  8) @reg(base_tn) ; base_tn += reg ;  --> @0(base_tn+=reg) ;

  TOP top_automod = TOP_UNDEFINED;
  BOOL mem_first = OP_Precedes(mem_op, inc_op);

  TN *automod_tn = NULL;
  if (TN_has_value(adjust_tn)) {
    if (TN_has_value(offset_tn)) {
      INT64 offset_val = TN_value(offset_tn); // Assume offsets are all inc
      if (offset_val == 0) {
	// Cases 1), 3)
	top_automod = TOP_AM_automod_variant(OP_code(mem_op), mem_first, is_inc, ISA_REGISTER_CLASS_UNDEFINED);
      }
      else if ((offset_val == -adjust_val) && !mem_first) {
	// Case 2)
	top_automod = TOP_AM_automod_variant(OP_code(mem_op), TRUE, is_inc, ISA_REGISTER_CLASS_UNDEFINED);
      }
      else if ((offset_val == adjust_val) && mem_first) {
	// Case 4)
	top_automod = TOP_AM_automod_variant(OP_code(mem_op), FALSE, is_inc, ISA_REGISTER_CLASS_UNDEFINED);
      }
      // [JV] On ARM, immediate offset are signed (considered signed)
#if 0
      automod_tn = Gen_Literal_TN((is_inc) ? adjust_val : -adjust_val, 4);
#else
      automod_tn = Gen_Literal_TN(adjust_val, 4);
#endif
    }
  }
  else {
    // Cases 5), 6)
    if (TN_has_value(offset_tn) && (TN_value(offset_tn) == 0)) {
      top_automod = TOP_AM_automod_variant(OP_code(mem_op), mem_first, is_inc, TN_register_class(adjust_tn));
      automod_tn = adjust_tn;
    }
    // _r_post_dec and _r_pre_inc are not defined on STxP70
    else if ((adjust_tn == offset_tn) && (mem_first == is_inc)) {
      // Cases 7), 8)
      top_automod = TOP_AM_automod_variant(OP_code(mem_op), !mem_first, is_inc, TN_register_class(adjust_tn));
    }
  }

  if (EBO_Trace_Optimization) {
    if (mem_first) {
      Print_OP_No_SrcLine(mem_op);
      Print_OP_No_SrcLine(inc_op);
    }
    else {
      Print_OP_No_SrcLine(inc_op);
      Print_OP_No_SrcLine(mem_op);
    }
  }

  if (EBO_Trace_Optimization)
    fprintf(TFile, "AutoMod_Combine: ");

  if (top_automod == TOP_UNDEFINED) {
    if (EBO_Trace_Optimization)
      fprintf(TFile, "TOP_UNDEFINED\n");
    return FALSE;
  }

  // Check if immediate range is enough

  if (TN_has_value(automod_tn)) {
    INT automod_idx = TOP_Find_Operand_Use(top_automod, OU_offset);
    if (!TOP_opnd_value_in_range(top_automod, automod_idx, TN_value(automod_tn))) {
      if (EBO_Trace_Optimization) {
		fprintf(TFile, "EBO, AutoMod_Combine: value %d out of range\n", (int)TN_value(automod_tn));
      }
      return FALSE;
    }
    // Take into account the scaling factor
    if ((TN_value(automod_tn) % TOP_Mem_Bytes(top_automod)) != 0) {
      if (EBO_Trace_Optimization) {
	fprintf(TFile, "EBO, AutoMod_Combine: value %d cannot be scaled down by %d.\n", (int)TN_value(automod_tn), TOP_Mem_Bytes(top_automod));
      }
      return FALSE;
    }
  }


  Build_AutoMod_OP(top_automod, base_tn, automod_tn, mem_op, ops);

  //TN *predicate_TN = OP_opnd(mem_op, OP_find_opnd_use(mem_op, OU_predicate));
  //
  //if (!OP_store(mem_op))
  //  Build_OP (top_automod, OP_result(mem_op, 0), base_tn, predicate_TN, base_tn, automod_tn, ops);
  //else
  //  Build_OP (top_automod, base_tn, predicate_TN, base_tn, automod_tn, OP_opnd(mem_op, OP_find_opnd_use(mem_op, OU_storeval)), ops);

  Is_True((OP_find_opnd_use(OPS_first(ops), OU_postincr) >= 0) ||
	  (OP_find_opnd_use(OPS_first(ops), OU_preincr) >= 0),
	  ("EBO, AutoMod_Combine: Missing OU_postincr/OU_preincr property on automod operator %s.", TOP_Name(top_automod)));
  Is_True(TOP_is_automod(top_automod), ("EBO, AutoMod_Combine: !TOP_is_automod(%s)\n", TOP_Name(top_automod)));

  if (EBO_Trace_Optimization)
    Print_OP(OPS_first(ops));

  return TRUE;
}

/* ============================================================
 * AutoMod_Sequence
 *
 * Generate a memory operation that can combine mem_op and inc_op
 * - Check a machine instruction exists
 * - Check immediate value can be represented
 * ============================================================
 */

static BOOL
AutoMod_Sequence(OP *mem_op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{

  // Get base and offset, check it is not already an auto-mod
  // addressing mode

  if (TOP_is_automod(OP_code(mem_op)))
    // Already an auto-mod operation
    return FALSE;

  INT base_idx = OP_find_opnd_use(mem_op, OU_base);
  INT offset_idx = OP_find_opnd_use(mem_op, OU_offset);

  if (base_idx == -1 || offset_idx == -1)
    return FALSE;

  // Look for the defining operation of base
  if (opnd_tninfo != NULL) {
    EBO_TN_INFO *base_info;
    base_info = opnd_tninfo[base_idx];
    if (base_info != NULL &&
	base_info->in_op != NULL &&
	base_info->in_bb == OP_bb(mem_op) &&
	(base_info->reference_count == 1)) {
      // Found a definition in this basic block, whose first reference
      // is on this memory operation.
      OPS ops = OPS_EMPTY;
      if (AutoMod_CheckCombine(mem_op, base_info->in_op) &&
	  AutoMod_Combine(mem_op, base_info->in_op, &ops)) {
	BB_Insert_Ops(OP_bb(mem_op), mem_op, &ops, FALSE);
	// Remove the inc_op
	OP_Change_To_Noop(base_info->in_op);
	return TRUE;
      }
    }
  }

  // Look forward to the next use of the base_tn
  TN *base_tn = OP_opnd(mem_op, base_idx);
  OP *use_op;
  for (use_op = OP_next(mem_op); use_op != NULL; use_op = OP_next(use_op)) {
    INT idx;
    for (idx = 0; idx < OP_opnds(use_op); idx++) {
      if (TNequiv(OP_opnd(use_op, idx), base_tn))
	// Found the first use
	goto loop_end;
    }
    for (idx = 0; idx < OP_results(use_op); idx++) {
      if (TNequiv(OP_result(use_op, idx), base_tn))
	// Found a def before a use, not acceptable for auto-mod
	return FALSE;
    }
  }

 loop_end:

  if (use_op != NULL) {
    OPS ops = OPS_EMPTY;
    if (AutoMod_CheckCombine(mem_op, use_op) &&
	AutoMod_Combine(mem_op, use_op, &ops)) {
      BB_Insert_Ops(OP_bb(mem_op), mem_op, &ops, FALSE);
      // Remove the inc_op
      OP_Change_To_Noop(use_op);
      return TRUE;
    }
  }

  return FALSE;
}



/**
 * Find operations which use the given TN.
 * The search is stopped on the first operation that defined given TN or if we
 * are unable to find the next operation.
 *
 * @param  startPoint [in] From where we will browse the operations
 * @param  soughtTn [in] TN for which we sought the usage
 * @param  result [out] Will contains the list of found operations
 * @param  stopOnFirst Specify to stop browsing on the first found usage
 *
 * @pre    result.empty()
 * @post   result contains the list of operations that use soughtTn or the first
 *         usage (stopOnFirst == TRUE) until the first definition of soughtTh.
 *
 * @return TRUE if search stops on a definition of soughtTh, FALSE otherwise.
 */
static BOOL
Find_Uses(OP* startPoint, TN* soughtTn, ListOfOperations& result,
          BOOL stopOnFirst)
{
    OP *use_op = NULL;
    BOOL bResult = FALSE;
    DevAssert(result.empty(), ("Pre conditions are wrong!!!"));
    for(use_op = startPoint; use_op; use_op = OP_next(use_op))
        {
            if(OP_Refs_TN(use_op, soughtTn))
                {
                    result.push_back(use_op);
                }
            if(OP_Defs_TN(use_op, soughtTn))
                {
                    bResult = TRUE;
                    break;
                }
            // We break here instead of the first operation push_back to have
            // an accurate bResult
            if(!result.empty() && stopOnFirst)
                {
                    break;
                }
        }
    return bResult;
}

static BOOL
OP_Refs_Equiv_TN(OP* op, TN* tn)
{
    int i;
    for(i = 0; i < OP_opnds(op); ++i)
        {
            if(TNequiv(OP_opnd(op, i), tn))
                {
                    return TRUE;
                }
        }
    return FALSE;
}

static BOOL
OP_Defs_Equiv_TN(OP* op, TN* tn)
{
    int i;
    for(i = 0; i < OP_results(op); ++i)
        {
            if(TNequiv(OP_result(op, i), tn))
                {
                    return TRUE;
                }
        }
    return FALSE;
}

/**
 * Find operations which use the given TN.
 * The search is stopped on the first operation that defined given TN or if we
 * are unable to find the next operation.
 *
 * @param  startPoint [in] From where we will browse the operations
 * @param  soughtTn [in] TN for which we sought the usage
 * @param  result [out] Will contains the list of found operations
 * @param  stopOnFirst Specify to stop browsing on the first found usage
 *
 * @pre    result.empty()
 * @post   result contains the list of operations that use soughtTn or the first
 *         usage (stopOnFirst == TRUE) until the first definition of soughtTh.
 *
 * @return The defining operation or NULL if not found
 */
static OP*
Find_Equiv_Uses(OP* startPoint, TN* soughtTn, ListOfOperations& result,
                BOOL stopOnFirst)
{
    OP *use_op = NULL;
    OP *def_op = NULL;
    DevAssert(result.empty(), ("Pre conditions are wrong!!!"));
    for(use_op = startPoint; use_op; use_op = OP_next(use_op))
        {
            if(OP_Refs_Equiv_TN(use_op, soughtTn))
                {
                    result.push_back(use_op);
                }
            if(OP_Defs_Equiv_TN(use_op, soughtTn))
                {
                    def_op = use_op;
                }
            // We break here instead of the first operation push_back to have
            // an accurate bResult
            if((!result.empty() && stopOnFirst) || def_op)
                {
                    break;
                }
        }
    return def_op;
}

/**
 * Check whether given operation is predicated or not.
 * We consider that an operation is predicated, when the predicat is not True_TN
 *
 * @remarks To give the right result the current operation must have an operand
 *          with OU_predicate property
 *
 * @param  op [in] Operation to be checked
 *
 * @pre    True_TN is set
 * @post   result implies op is predicated
 *
 * @return TRUE if op is predicated, FALSE otherwise
 */
static BOOL
Is_Predicated(OP* op)
{
    return !(OP_find_opnd_use(op, OU_predicate) == -1 ||
             OP_opnd(op, OP_find_opnd_use(op, OU_predicate)) == True_TN);
}

/**
 * Copy and insert operation.
 * This function copy given source operation, and insert the newly created
 * operation before (before == TRUE) or after (before == FALSE) point operation.
 * Newly created operation has its opcode set to newTop.
 *
 * @param  src [in] Operation to be copied
 * @param  point [in] Point in a basic block, used as a reference to insert the
 *         created instruction
 * @param  newTop TOP code to be set to the created operation, i.e. the copy is
 *         not a true one, since we modified the TOP code
 * @param  pdst [out] If not null, this pointer will point to the address of the
 *         created and inserted operation
 * @param  before Specify where to insert the created operation relatively to
 *         point
 *
 * @pre    OP_bb(point) <> NULL
 * @post   if result then (pdst@pre <> NULL implies *pdst = newly created 
 *         operation) and newly created operation has been inserted in
 *         OP_bb(point) else no operation have been inserted and pdst point to
 *         a non-valid address endif
 *
 * @return TRUE if an operation has been created and inserted, FALSE ohterwise
 */
static BOOL
Copy_And_Insert_OP(OP* src, OP* point, TOP newTop, OP** pdst, BOOL before)
{
    BOOL result = FALSE;
    DevAssert(OP_bb(point), ("Point operation must belong to a basic block"));
    OP* newOp = Dup_OP(src);
    OP_Change_Opcode(newOp, newTop);
    OP_srcpos(newOp) = OP_srcpos(src);
    if(OP_memory(src))
        {
            Copy_WN_For_Memory_OP(newOp, src);
        }
    if(EBO_Verify_Op(newOp))
        {
            BB_Insert_Op(OP_bb(point), point, newOp, before);
            if(pdst)
                {
                    *pdst = newOp;
                }
            result = TRUE;
        }
    return result;
}

/**
 * Check whether given andng operation can be used to optimize a jump.
 * @see branch_condition_sequence function for more details
 *
 * @param  andngOp [in] An andng operation
 *
 * @pre    OP_code(andngOp) == TOP_andng
 * @post   result implies andngOp may be used to optimize a jump
 *
 * @return TRUE if andngOp may be used to optimize a jump, FALSE otherwise
 */
static BOOL
Can_Jump_Optimize(OP* andngOp)
{
#if 0
    DevAssert(OP_code(andngOp) == TOP_andng, ("Pre conditions are wrong!!!"));
    ListOfOperations uses;
    static_cast<void>(Find_Uses(OP_next(andngOp), OP_result(andngOp, 0), uses,
                                FALSE));
    BOOL result = FALSE;
    ListOfOperationsCIt it;
    for(it = uses.begin(); it != uses.end(); ++it)
        {
            if(OP_cond(*it))
                {
                    if(EBO_Trace_Optimization)
                        {
                            fprintf(TFile,"%s Found a jump optimizable"
                                    " operation:\n", EBO_trace_pfx);
                            Print_OP_No_SrcLine(*it);
                        }
                    result = TRUE;
                    break;
                }
        }
    return result;
#endif
  return FALSE;
}

/**
 * merge_compare_and_guards optimization, in peep version, i.e. given
 * operation is a comparision not predicated.
 *
 * @see merge_compare_and_guards for more details
 */
static BOOL
merge_compare_and_guards_in_peep(OP *op,
                                 TN **opnd_tn,
                                 EBO_TN_INFO **opnd_tninfo)
{
    BOOL result = FALSE;

#if 0
    DevAssert(TOP_is_cmp(OP_code(op)) && !Is_Predicated(op),
              ("Pre conditions are wrong!!!)"));
    ListOfOperations uses;
    TN* guardTn = OP_result(op, 0);
    BOOL isDefined = Find_Uses(OP_next(op), guardTn, uses, FALSE);
    OP* useOp = *(uses.begin());
    // Constraints are the following:
    // * Guard register is used by only one instruction before beginning
    //   new defined.
    // * The user operation is a andng
    // * The result of andng is the checked guard
    // By asserting that, we are sure that we can modify the value of
    // the checked guard.
    if(isDefined && uses.size() == 1 && OP_code(useOp) == TOP_andng &&
       TNs_Are_Equivalent(guardTn, OP_result(useOp, 0)) &&
       !Is_Predicated(useOp))
        {
            if(EBO_Trace_Optimization)
                {
                    fprintf(TFile,"%smerge_compare_and_guards_in_peep "
                            "find a %s which match constraints:\n",
                            EBO_trace_pfx, TOP_Name(OP_code(useOp)));
                    Print_OP_No_SrcLine(useOp);
                }
            
            TN* op1 = OP_opnd(useOp, OP_find_opnd_use(useOp, OU_opnd1));
            TN* op2 = OP_opnd(useOp, OP_find_opnd_use(useOp, OU_opnd2));
            if(op1 != True_TN)
                {
                    // Swap operands to have true_tn in the
                    // first one and the operand in the other
                    TN* tmp = op2;
                    op2 = op1;
                    op1 = tmp;
                }
            if(op1 == True_TN &&
               TNs_Are_Equivalent(guardTn, op2))
                {
                    // We invert the comparison, instead of changing the
                    // guard register, because G3 does not have a
                    // complementary register
                    TOP new_opcode = CGTARG_Invert(OP_code(op));
                    if(TOP_UNDEFINED != new_opcode)
                        {
                            if((result = Copy_And_Insert_OP(op, useOp,
                                                            new_opcode, NULL,
                                                            FALSE)))
                                {
                                    BB_Remove_Op(OP_bb(useOp), useOp);
                                    if(EBO_Trace_Optimization)
                                        {
                                            fprintf(TFile,"%smerge_compare_and_"
                                                    "guards_in_peep insert an "
                                                    "inverted comparisons:\n",
                                                    EBO_trace_pfx);
                                        }
                                }
                        }
                }
        }
    return result;
#endif
  return FALSE;
}

/**
 * merge_compare_and_guards optimization, before peep version, i.e. given
 * operation is a andng not predicated.
 *
 * @see merge_compare_and_guards for more details
 */
static BOOL
merge_compare_and_guards_before_peep(OP *op,
                                     TN **opnd_tn,
                                     EBO_TN_INFO **opnd_tninfo)
{
    BOOL result = FALSE;
#if 0
    DevAssert(OP_code(op) == TOP_andng && !Is_Predicated(op) &&
              EBO_in_peep == FALSE, ("Pre conditions are wrong!!!)"));
    INT guardIdx = OP_find_opnd_use(op, OU_opnd1);
    if(OP_opnd(op, guardIdx) == True_TN)
        {
            guardIdx = OP_find_opnd_use(op, OU_opnd2);
        }
    EBO_OP_INFO *cmpOpInfo;
    if(find_def_opinfo(opnd_tninfo[guardIdx], &cmpOpInfo))
        {
            OP* cmpOp = cmpOpInfo->in_op;
            if(EBO_Trace_Optimization)
                {
                    fprintf(TFile,"%smerge_compare_and_guards_before_peep"
                            " find a definition for op\n", EBO_trace_pfx);
                    Print_OP_No_SrcLine(cmpOp);
                }

            if(TOP_is_cmp(OP_code(cmpOp)) && !Is_Predicated(cmpOp))
                {
                    TOP newTop = CGTARG_Invert(OP_code(cmpOp));
                    if(newTop != TOP_UNDEFINED && !Can_Jump_Optimize(op))
                        {
                            OP* newOp = NULL;
                            if((result = Copy_And_Insert_OP(cmpOp, op, newTop,
                                                            &newOp, FALSE)))
                                {
                                    Set_OP_result(newOp, 0, OP_result(op, 0));
                                    if(EBO_Trace_Optimization)
                                        {
                                            fprintf(TFile,"%smerge_compare_and_"
                                                    "guards_before_peep created"
                                                    " instruction:\n ",
                                                    EBO_trace_pfx);
                                            Print_OP_No_SrcLine(newOp);
                                            fprintf(TFile,"%smerge_compare_and_"
                                                    "guards_before_peep "
                                                    "insert it after:\n",
                                                    EBO_trace_pfx);
                                            Print_OP_No_SrcLine(op);
                                        }
                                }
                        }
                }
        }
    return result;
#endif
  return FALSE;
}

// The goal is to remplace:
// * If we are in peep:
// cmp Gx, ..., ...
// Insts that do not make use of Gx
// andng Gx, Gx, G7
// in:
// cmp Gx, ..., ...
// Insts that do not make use of Gx
// // <-- negation of the guard register is suppressed
// inv_cmp Gx, ..., ... // <-- comparison is inverted
// The removal of the old cmp is done by the generic ebo framework when this
// function return true
// 
// * If we are not in peep:
// cmp Gx, ..., ...
// Insts that do not make define of Gx
// andng Gy, Gx, G7
// in:
// cmp Gx, ..., ...
// Insts that do not make define of Gx
// inv_cmp Gy, ..., ... // <-- comparison is inverted
// The removal of the andng is done by the generic ebo framework when this
// function return true. The old cmp is remove by a automatically by a deadcode
// if Gx is not use in Insts
static BOOL 
merge_compare_and_guards(OP *op,
                         TN **opnd_tn,
                         EBO_TN_INFO **opnd_tninfo
                         )
{
    BOOL result = FALSE;
#if 0
    if(EBO_in_peep)
        {
            if(TOP_is_cmp(OP_code(op)) && !Is_Predicated(op))
                {
                    if(EBO_Trace_Optimization)
                        {
                            fprintf(TFile,"%smerge_compare_and_guards find a "
                                    "comparison:\n", EBO_trace_pfx);
                            Print_OP_No_SrcLine(op);
                        }
                    result = merge_compare_and_guards_in_peep(op, opnd_tn,
                                                              opnd_tninfo);
                }
        }
    else if(OP_code(op) == TOP_andng && !Is_Predicated(op))
        {
            if(EBO_Trace_Optimization)
                {
                    fprintf(TFile,"%smerge_compare_and_guards find an "
                            "andng:\n", EBO_trace_pfx);
                    Print_OP_No_SrcLine(op);
                }
            result = merge_compare_and_guards_before_peep(op, opnd_tn,
                                                          opnd_tninfo);
        }
#endif
    return result;
}

static BOOL
check_add_load_store_usage(OP* mem_op, OP* def_op, TN* operand,
                           TOP& new_top_mem_op, BOOL& mustMove)
{
    mustMove = FALSE;
    if(!TN_has_value(operand)) {
        ListOfOperations ops;
        OP* opndDef;
        if((opndDef = Find_Equiv_Uses(def_op, operand, ops, FALSE))) {
            // If defining op is def_op, make a check between def_op
            // and mem_op
            if(opndDef == def_op) {
                // if we are here, this means we have a pattern like:
                // Rx := OP Rx, Ry
                // Hence, generating mem_op @(Rx, Ry) will not work
                // useless we are sure we can (re)move OP. Otherwise we will
                // perform two times the displacement in address computation
                ops.clear();
                opndDef = Find_Equiv_Uses(OP_next(def_op), operand, ops,
                                          FALSE);
                mustMove = TRUE;
            }
            // Operand is killed before mem_op --> we cannot use it
            if(opndDef && OP_Precedes(opndDef, mem_op)) {
                new_top_mem_op = TOP_UNDEFINED;
            }
        }
    }
    return new_top_mem_op != TOP_UNDEFINED;
}

enum EDefOpHandling {
    KILL,
    MOVE,
    NOTHING
};

static void
set_add_load_store_def_op_handling(OP* mem_op, OP* def_op,
                                   EDefOpHandling& defOpHandling)
{
    ListOfOperations ops;
    defOpHandling = NOTHING;
    OP* resDef =
        Find_Equiv_Uses(OP_next(def_op), OP_result(def_op, 0),
                        ops, FALSE);

    // remove mem_op usage
    ListOfOperationsIt it = ops.begin();
    BOOL allAfter = TRUE;
    while(it != ops.end()) {
        if(*it == mem_op) {
            it = ops.erase(it);
        } else {
            allAfter &= OP_Precedes(mem_op, *it);
            ++it;
        }
    }
    
    if(resDef && ops.empty()) {
        // only mem_op defined it or the new definition does not use the old
        // one
	defOpHandling = KILL;
    }
    if(allAfter && defOpHandling == NOTHING) {
        // We already know that the operands of def operation are not
        // defined until mem_op. So we have to check mem_op definition
        int i;
        BOOL moveable = TRUE;
        for(i = 0; i < OP_opnds(def_op) && moveable; ++i) {
            moveable &= !OP_Defs_Equiv_TN(mem_op, OP_opnd(def_op, i));
        }
        if(moveable) {
            defOpHandling = MOVE;
        }
    }
}

static BOOL
check_add_load_store_dependencies(OP* mem_op, OP* def_op, TN*& base,
                                  TN*& offset, TOP& new_top_mem_op,
                                  EDefOpHandling& defOpHandling)
{
    defOpHandling = NOTHING;
    INT predicateIdx = OP_find_opnd_use(mem_op, OU_predicate);
    TN* memPredicate = predicateIdx != -1? OP_opnd(mem_op, predicateIdx): NULL;

    predicateIdx = OP_find_opnd_use(def_op, OU_predicate);
    TN* defPredicate = predicateIdx != -1? OP_opnd(def_op, predicateIdx): NULL;
    new_top_mem_op = TOP_UNDEFINED;

    if((OP_iadd(def_op) || OP_isub(def_op)) && OP_results(def_op) == 1 &&
       memPredicate == defPredicate) {
        INT64 adjust_val = 0;
        INT offsetIdx = OP_find_opnd_use(mem_op, OU_offset);
        TN* opnd2 = OP_opnd(def_op, OP_find_opnd_use(def_op, OU_opnd2));
        TN* memopOffset = OP_opnd(mem_op, offsetIdx);
        base = OP_opnd(def_op, OP_find_opnd_use(def_op, OU_opnd1));

        if(TN_has_value(opnd2)) {
            adjust_val = OP_iadd(def_op)? TN_value(opnd2) : -TN_value(opnd2);
            if(TN_has_value(memopOffset)) {
                adjust_val += TN_value(memopOffset);
                new_top_mem_op = OP_code(mem_op);
                ISA_LIT_CLASS lclass;
                INT immIdx = TOP_Immediate_Operand(new_top_mem_op, &lclass);
                DevAssert(immIdx != -1, ("Operand is an immediate, but we do "
                                         "not have its index!"));
                if(!ISA_LC_Value_In_Class(adjust_val, lclass)) {
                    new_top_mem_op = TOP_UNDEFINED;
                } else {
                    offset = Gen_Literal_TN(adjust_val, 4);
                }
            }
            // else merge of add and load or store is not possible
        } else if(TN_has_value(memopOffset) && TN_value(memopOffset) == 0) {
            // Here opnd2 is a register
            if(OP_iadd(def_op)) {
                // store Rn + Rp automatically rejected by this call
                new_top_mem_op =
                    TOP_opnd_register_variant(OP_code(mem_op), offsetIdx,
                                              TN_register_class(opnd2));
                offset = opnd2;
            }
            // else we have not register decrement on load store operation
        }
        // else merge of add and load or store is not possible

        if(new_top_mem_op != TOP_UNDEFINED) {
            // Check that our operands are available until the memory operation
            // This constraint is not mandatory when EBO_in_peep is wrong,
            // however it is always required if we do not want to decrease code
            // performances (otherwise, we should add some copy operations)
            BOOL moveOfst, moveBase;
            if(!check_add_load_store_usage(mem_op, def_op, offset,
                                           new_top_mem_op, moveOfst) ||
               !check_add_load_store_usage(mem_op, def_op, base,
                                           new_top_mem_op, moveBase)) {
                new_top_mem_op = TOP_UNDEFINED;
            } else {
                set_add_load_store_def_op_handling(mem_op, def_op,
                                                   defOpHandling);
                // Check if def_op can be removed
                if((moveOfst || moveBase) && defOpHandling == NOTHING) {
                    new_top_mem_op = TOP_UNDEFINED;
                }
            }
        }
    }
    return new_top_mem_op != TOP_UNDEFINED;
}


static BOOL
generate_add_load_store_sequence(OP* mem_op, OP* def_op, TN* base, TN* offset,
                                 TOP& new_top_mem_op, EDefOpHandling defHdl)
{
    OPS ops = OPS_EMPTY;
    OP* op = Dup_OP(mem_op);
    OP_Change_Opcode(op, new_top_mem_op);
    Copy_WN_For_Memory_OP(op, mem_op);
    Set_OP_opnd(op, OP_find_opnd_use(op, OU_base), base);
    Set_OP_opnd(op, OP_find_opnd_use(op, OU_offset), offset);
    OPS_Append_Op(&ops, op);
    BOOL result = TRUE;
    EBO_OP_INFO *opinfo;
    EBO_TN_INFO **tninfo = (EBO_TN_INFO **)alloca(sizeof(EBO_TN_INFO *) *
                                                  OP_opnds(def_op));
    FmtAssert(tninfo, ("No memory left"));
    for(INT i = 0; i < OP_opnds(def_op); ++i) {
        if(TN_is_register(OP_opnd(def_op, i))) {
            tninfo[i] = get_tn_info(OP_opnd(def_op, i));
        } else {
            tninfo[i] = NULL;
        }
    }

    opinfo = EBO_opinfo_table[EBO_hash_op(def_op, tninfo)];

    if(defHdl == KILL) {
        if(opinfo && (opinfo->op_must_not_be_removed ||
                      opinfo->op_must_not_be_moved)) {
            result = FALSE;
        } else {
            OP_Change_To_Noop(def_op);
        }
    } else if(defHdl == MOVE) {
        if(opinfo && opinfo->op_must_not_be_moved) {
            result = FALSE;
        } else {
            // def_op will be proceed a second time by ebo, so we have to
            // reset its information
            OP* dup = Dup_OP(def_op);
            OP_Change_To_Noop(def_op);
            BB_Insert_Op(OP_bb(mem_op), mem_op, dup, FALSE);
        }
    }
    if(result) {
        BB_Insert_Ops(OP_bb(mem_op), mem_op, &ops, FALSE);
    }
    return result;
}

static BOOL 
merge_add_load_store(OP *mem_op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{
    BOOL result = FALSE;
    INT base_idx = OP_find_opnd_use(mem_op, OU_base);
    INT offset_idx = OP_find_opnd_use(mem_op, OU_offset);

    if(base_idx != -1 && offset_idx != -1 && opnd_tninfo != NULL) {
        // Look for the defining operation of base
        EBO_TN_INFO *base_info = opnd_tninfo[base_idx];
        if(base_info != NULL && base_info->in_op != NULL &&
           base_info->in_bb == OP_bb(mem_op)) {
            TN* offset;
            TN* base;
            TOP new_top_mem_op;
            EDefOpHandling defHdl;
            result = check_add_load_store_dependencies(mem_op,
                                                       base_info->in_op,
                                                       base, offset,
                                                       new_top_mem_op, defHdl)
                && generate_add_load_store_sequence(mem_op, base_info->in_op,
                                                    base, offset,
                                                    new_top_mem_op, defHdl);
        }
    }
    return result;
}

static BOOL
Update_Ref_Count(TN* tn)
{
    BOOL result = FALSE;
    EBO_TN_INFO *tnOpndDef = get_tn_info(tn);

    EBO_OP_INFO *tnOpinfo = locate_opinfo_entry(tnOpndDef);
    if(tnOpinfo)
        {
            OP* defOp = tnOpinfo->in_op;
            for(int idx = 0; defOp && idx < OP_results(defOp); idx++)
                {
                    if(OP_result(defOp, idx) == tn)
                        {
                            inc_ref_count(tnOpinfo->actual_rslt[idx]);
                            result = TRUE;
                        }
                }
        }
    return result;
}

static bool
IsCmpEq(const OP* op)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
    return op &&
        (OP_code(op) == TOP_cmpeq_r || OP_code(op) == TOP_cmpequ_r ||
         OP_code(op) == TOP_cmpeq_i8 || OP_code(op) == TOP_cmpequ_i8 ||
         OP_code(op) == TOP_cmpeqb_r || OP_code(op) == TOP_cmpequb_r ||
         OP_code(op) == TOP_cmpeqb_i8 || OP_code(op) == TOP_cmpequb_i8 ||
         OP_code(op) == TOP_cmpeqh_r || OP_code(op) == TOP_cmpequh_r ||
         OP_code(op) == TOP_cmpeqh_i8 || OP_code(op) == TOP_cmpequh_i8);
#endif
    return FALSE;
}

static bool
IsCmpNeq(const OP* op)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
    return op &&
        (OP_code(op) == TOP_cmpne_r || OP_code(op) == TOP_cmpneu_r ||
         OP_code(op) == TOP_cmpne_i8 || OP_code(op) == TOP_cmpneu_i8 ||
         OP_code(op) == TOP_cmpneb_r || OP_code(op) == TOP_cmpneub_r ||
         OP_code(op) == TOP_cmpneb_i8 || OP_code(op) == TOP_cmpneub_i8 ||
         OP_code(op) == TOP_cmpneh_r || OP_code(op) == TOP_cmpneuh_r ||
         OP_code(op) == TOP_cmpneh_i8 || OP_code(op) == TOP_cmpneuh_i8);
#endif
    return FALSE;
}

static TN*
transform_compare_and_shift_set_make_value(OP* cmpOp, OP* makeOp,
                                           INT64 makeValue, TOP& topAnd)
{
    TN* tnMakeValue = NULL;

    FmtAssert (FALSE, ("not yet implemented"));
#if 0
    // Will this immediate fit in 'and' immediate?
    if(!ISA_LC_Value_In_Class(makeValue, LC_imm_u8))
        {
            TN* cmpPredicate = OP_opnd(cmpOp, OP_find_opnd_use(cmpOp,
                                                               OU_predicate));
            if(makeOp)
                {
                    TN* makeRes = OP_result(makeOp, 0);
                    tnMakeValue = Build_RCLASS_TN(TN_register_class(makeRes));
                    OPS opsTmp = OPS_EMPTY;
                    Expand_Copy(tnMakeValue, cmpPredicate, makeRes, &opsTmp);
            
                    EBO_TN_INFO *makeOpndDef = get_tn_info(makeRes);
            
                    EBO_OP_INFO *makeOpinfo = locate_opinfo_entry(makeOpndDef);
                    if(makeOpinfo)
                        {
                            OP* defOp = makeOpinfo->in_op;
                            for(int idx = 0; defOp && idx < OP_results(defOp);
                                idx++)
                                {
                                    if(OP_result(defOp, idx) == makeRes)
                                        {
                                            inc_ref_count(makeOpinfo->
                                                          actual_rslt[idx]);
                                        }
                                }
                        }
                    // Insert the copy after the make operation
                    BB_Insert_Ops(OP_bb(makeOp), makeOp, &opsTmp, FALSE);
                }
            else
                {
                    tnMakeValue = Build_RCLASS_TN(ISA_REGISTER_CLASS_gpr);
                    OPS opsTmp = OPS_EMPTY;
                    Expand_Immediate(tnMakeValue, Gen_Literal_TN(makeValue, 4),
                                     MTYPE_I4, &opsTmp);
                    for(OP* cur = OPS_first(&opsTmp); cur; cur = OP_next(cur))
                        {
                            Set_OP_opnd(cur,
                                        OP_find_opnd_use(cur, OU_predicate),
                                        cmpPredicate);
                        }
                    // Insert make before the cmp operation
                    BB_Insert_Ops(OP_bb(cmpOp), cmpOp, &opsTmp, TRUE);
                }
            topAnd = TOP_and_r;
        }
    else
        {
            tnMakeValue = Gen_Literal_TN(makeValue, 4);
            topAnd = TOP_and_i8;
        }

#endif
    return tnMakeValue;
}

static void
transform_compare_and_shift_transform(OP* cmpOp, OP* andOp, OP* shlOp,
                                      INT andOpndIdx, TN* tnMakeValue,
                                      TOP topAnd)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0

    TN *shlOpnd2 = OP_opnd(shlOp, OP_find_opnd_use(shlOp, OU_opnd2));
    TN *shlPredicate = OP_opnd(shlOp, OP_find_opnd_use(shlOp, OU_predicate));
    // tnMakeValue contains the immediate to be used in and instruction
    OPS opsTmp = OPS_EMPTY;
    TN* tnCpyShlOpd = NULL;
    TN* tnCpyAndOpd = NULL;

    // We are not interested by the result because we know the related tn
    // may have not a definition (eg it is a parameter)
    static_cast<void>(Update_Ref_Count(shlOpnd2));
    // Copy right operand of shift left operation
    tnCpyShlOpd = Build_RCLASS_TN(TN_register_class(shlOpnd2));
    Expand_Copy(tnCpyShlOpd, shlPredicate, shlOpnd2, &opsTmp);
    // Insert this copy just before the shift operation
    BB_Insert_Ops(OP_bb(shlOp), shlOp, &opsTmp, TRUE);

    TN* tnAndOpd = OP_opnd(andOp, andOpndIdx);
    // Same remark as previous call to Update_Ref_Count
    static_cast<void>(Update_Ref_Count(tnAndOpd));

    // Copy the mask used in the and operation
    tnCpyAndOpd = Build_RCLASS_TN(TN_register_class(tnAndOpd));
    OPS opsTmp2 = OPS_EMPTY;
    Expand_Copy(tnCpyAndOpd, shlPredicate, tnAndOpd, &opsTmp2);
    BB_Insert_Ops(OP_bb(andOp), andOp, &opsTmp2, TRUE);

    // Create shift right equivalence
    TN* shrRes =
        Build_RCLASS_TN(TN_register_class(OP_result(shlOp, 0)));
    TOP topShl = OP_code(shlOp);
    TOP topShr = topShl == TOP_shlu_i5? TOP_shru_i5:
        (topShl == TOP_shl_i5? TOP_shr_i5:
         (topShl == TOP_shlu_r? TOP_shru_r: TOP_shr_r));
    OPS opsTmp3 = OPS_EMPTY;
    Build_OP(topShr, shrRes, shlPredicate, tnCpyAndOpd, tnCpyShlOpd, &opsTmp3);
    
    // Create and equivalence
    TN* andRes = Build_RCLASS_TN(TN_register_class(shrRes));
    Build_OP(topAnd, andRes, shlPredicate, shrRes, tnMakeValue, &opsTmp3);
    
    // Create cmp equivalence
    Build_OP(OP_code(cmpOp), OP_result(cmpOp, 0), shlPredicate, andRes,
             OP_opnd(cmpOp, OP_find_opnd_use(cmpOp, OU_opnd2)), &opsTmp3);
    
    BB_Insert_Ops(OP_bb(cmpOp), cmpOp, &opsTmp3, TRUE);
#endif
}

static BOOL
transform_compare_and_shift_handle_shift(OP* cmpOp, OP* andOp, OP*& shlOp,
                                         TN*& tnMakeValue, TOP& topAnd,
                                         EBO_TN_INFO* andTnInfo)
{
    BOOL result = FALSE;
    FmtAssert (FALSE, ("not yet implemented"));
#if 0
    // SHL operation
    shlOp = andTnInfo->in_op;
    TN* shlPredicate = OP_opnd(shlOp, OP_find_opnd_use(shlOp, OU_predicate));
                    
    EBO_OP_INFO *shl_opinfo = locate_opinfo_entry(andTnInfo);
    // Look for the defining operation of SHL operand
    EBO_TN_INFO *shl_tninfo = NULL;
    INT shl_opnd = OP_find_opnd_use(shlOp, OU_opnd1);
    if (shl_opinfo != NULL)
        {
            shl_tninfo = shl_opinfo->actual_opnd[shl_opnd];
        }
    
    // Seek for a constant definition or use rematerializable flag
    OP* makeOp = NULL;
    INT64 makeValue;
    BOOL hasDefiner = FALSE;
    if (shl_tninfo != NULL &&
        shl_tninfo->in_op != NULL &&
        (shl_tninfo->reference_count == 1) &&
        (TOP_MAKE == OP_code(shl_tninfo->in_op) ||
         TOP_make == OP_code(shl_tninfo->in_op)) &&

        // (cbr) Support for guards on false
        Opnds_Are_Equivalent (shlOp, shl_tninfo->in_op,
                              OP_find_opnd_use(shlOp, OU_predicate), 
                              OP_find_opnd_use(shl_tninfo->in_op, OU_predicate)))
        {
          hasDefiner = TRUE;
          makeOp = shl_tninfo->in_op;
          makeValue = TN_value(OP_opnd(makeOp, 1));
        }
    else if(TN_is_rematerializable(OP_opnd(shlOp, shl_opnd)))
        {
          WN* remat = TN_remat(OP_opnd(shlOp, shl_opnd));
          if(WN_operator(remat) == OPR_INTCONST)
                {
                    makeValue = WN_const_val(remat);
                    hasDefiner = TRUE;
                }
        }
    // makeValue must be a power of 2
    if(hasDefiner && (makeValue & (makeValue - 1)) == 0)
        {
            tnMakeValue =
                transform_compare_and_shift_set_make_value(cmpOp, makeOp,
                                                           makeValue, topAnd);
            result = TRUE;
        }
#endif
    return result;
}

static BOOL
transform_compare_and_shift_handle_and(OP* cmpOp, OP*& andOp, OP*& shlOp,
                                       INT& andOpndIdx,
                                       TN*& tnMakeValue, TOP& topAnd,
                                       EBO_TN_INFO* cmpTnInfo)
{
    BOOL result = FALSE;
    andOp = cmpTnInfo->in_op;

    // (cbr) Support for guards on false
    TN* andPredicate = OP_opnd(andOp, OP_find_opnd_use(andOp, OU_predicate));

    // check that one of the operand is the result of a shr
    EBO_OP_INFO *andOpInfo = locate_opinfo_entry(cmpTnInfo);
                                            
    // Look for the defining operation of AND operand
    EBO_TN_INFO *andTnInfo = NULL;
    INT andOpnd1 = OP_find_opnd_use(andOp, OU_opnd1);
    INT andOpnd2 = OP_find_opnd_use(andOp, OU_opnd2);
    andOpndIdx = andOpnd1;
    int i = 0;
    // we perform a loop on the two operands of and operation
    do
        {
            if(andOpInfo != NULL)
                {
                    andTnInfo = andOpInfo->actual_opnd[andOpndIdx];
                }
            // Defining operation must be a shift left with the same predicate
            if(andTnInfo != NULL &&
               andTnInfo->in_op != NULL &&
               andTnInfo->reference_count == 1 &&
               (TOP_is_shl(OP_code(andTnInfo->in_op))) &&
               
               // (cbr) Support for guards on false
               Opnds_Are_Equivalent (andOp, andTnInfo->in_op,
                                     OP_find_opnd_use(andOp, OU_predicate),
                                     OP_find_opnd_use(andTnInfo->in_op, OU_predicate)))
                {
                    if(transform_compare_and_shift_handle_shift(cmpOp, andOp,
                                                                shlOp,
                                                                tnMakeValue,
                                                                topAnd,
                                                                andTnInfo))
                        {
                            result = TRUE;
                            break;
                        }
                }
            andOpndIdx = andOpnd2;
        }
    while(++i < 2);
    if(result)
        {
            // We have found the shl operand, but what we are interesting in
            // now, is the other.
            andOpndIdx = andOpndIdx == andOpnd1? andOpnd2: andOpnd1;
        }
    return result;
}


// Goal is to replace the pattern:
// A = cst (power of 2)
// B = A << C
// D = B & E
// D == 0 (or D != 0)
// by
// A = cst
// B = E >> C
// D = B & A (notice that the cst building disappears when it fits in & opnd)
// D == 0 (or D != 0)
//
// The interest of such transformation is to expose catchable case for txxbit
// optimisation
//
// Note: This transformation is valid only when the result is used in a
// comparison with 0
static BOOL 
transform_compare_and_shift(OP *op,
                            TN **opnd_tn,
                            EBO_TN_INFO **opnd_tninfo
                            )
{
  BOOL result = FALSE;
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
    // Operation should be an immediate compare equal/not equal to 0x0
    if((IsCmpEq(op) || IsCmpNeq(op)) &&
       TN_has_value(OP_opnd(op, OP_find_opnd_use(op, OU_opnd2))) &&
       // This tranformation is not valid for test against something else than
       // 0
       TN_value(OP_opnd(op, OP_find_opnd_use(op, OU_opnd2))) == 0 &&
       opnd_tninfo)
        {
            TN *cmpPredicate = OP_opnd(op, OP_find_opnd_use(op, OU_predicate));
            EBO_TN_INFO *cmpTnInfo =
                opnd_tninfo[OP_find_opnd_use(op, OU_opnd1)];
            // Defining operation must be a 'and' with same predicate
            if(cmpTnInfo != NULL && cmpTnInfo->in_op != NULL &&
               (cmpTnInfo->reference_count == 1) &&
               TOP_is_and(OP_code(cmpTnInfo->in_op)) &&
               // (cbr) Support for guards on false
               Opnds_Are_Equivalent (op, cmpTnInfo->in_op,
                                     OP_find_opnd_use(op, OU_predicate),
                                     OP_find_opnd_use(cmpTnInfo->in_op, OU_predicate)))
              {
                    OP* shlOp;
                    OP* andOp;
                    TN* tnMakeValue;
                    INT andOpndIdx;
                    TOP topAnd;
                    if(transform_compare_and_shift_handle_and(op, andOp, shlOp,
                                                              andOpndIdx,
                                                              tnMakeValue,
                                                              topAnd,
                                                              cmpTnInfo))
                        {
                            transform_compare_and_shift_transform(op, andOp,
                                                                  shlOp,
                                                                  andOpndIdx,
                                                                  tnMakeValue,
                                                                  topAnd);
                            result = TRUE;
                        }
                }
        }
#endif
    return result;
}


static TN*
TxxOP_sequence_set_tn_and_imm(OP* make_op, INT64 and_value, OP* and_op,
                              INT and_opnd_imm, ISA_LIT_CLASS lclass)
{
    TN *result = NULL;

    FmtAssert (FALSE, ("not yet implemented"));
#if 0

    DevAssert(and_op, ("Wrong precondition"));
    if(and_opnd_imm != -1) {
        result = OP_opnd(and_op, and_opnd_imm);
    } else if(lclass != ISA_LC_UNDEFINED &&
              ISA_LC_Value_In_Class(and_value, lclass)) {
        if(make_op) {
            result = OP_opnd(make_op, 1);
        } else {
            result = Gen_Literal_TN(and_value, 4);
        }
    } else {
        if(make_op) {
            TN* make_res = OP_result(make_op, 0);
            
            EBO_TN_INFO *makeOpndDef = get_tn_info(make_res);

            EBO_OP_INFO *makeOpinfo = locate_opinfo_entry(makeOpndDef);
            if(makeOpinfo) {
                result = Build_RCLASS_TN(TN_register_class(make_res));
                OPS opsTmp = OPS_EMPTY;
                Expand_Copy(result, OP_opnd(make_op,
                                            OP_find_opnd_use(make_op,
                                                             OU_predicate)),
                            make_res, &opsTmp);

                BB_Insert_Ops(OP_bb(make_op), make_op, &opsTmp, FALSE);

                OP* defOp = makeOpinfo->in_op;
                DevAssert(defOp == make_op,
                          ("Definer of make result is not make!"));
                inc_ref_count(makeOpinfo->actual_rslt[0]);
            }
        }
        if(!result) {
            result = Build_RCLASS_TN(ISA_REGISTER_CLASS_gpr);
            OPS opsTmp = OPS_EMPTY;
            TN* and_predicate =
                OP_opnd(and_op, OP_find_opnd_use(and_op, OU_predicate));
            Expand_Immediate(result, Gen_Literal_TN(and_value, 4), MTYPE_I4,
                             &opsTmp);
            for(OP* cur = OPS_first(&opsTmp); cur; cur = OP_next(cur)) {
                Set_OP_opnd(cur, OP_find_opnd_use(cur, OU_predicate),
                            and_predicate);
            }
            // Insert make before the and operation
            BB_Insert_Ops(OP_bb(and_op), and_op, &opsTmp, TRUE);
        }
    }

#endif
    return result;
}

// value is a power of 2
static int
Get_Pow2(INT64 value)
{
    unsigned int v = (unsigned int)value;  // Value to count zero bits on the right
    int c = 0;  // Number of zero bits on the right,

    if (!(v & 0x1)) {
        if ((v & 0xffff) == 0){
			v >>= 16;
			c += 16;
        }
        if ((v & 0xff) == 0){
			v >>= 8;
			c += 8;
        }
        if ((v & 0xf) == 0){
			v >>= 4;
			c += 4;
        }
        if ((v & 0x3) == 0){
			v >>= 2;
			c += 2;
        }
        if ((v & 0x1) == 0){
			c++;
        }
    }
    return c;
}

/* =====================================================================
   TxxOP_sequence (OP = BIT / AND / ANDN)
   - Generate TxxBIT, TxxAND and TxxANDN operations.

   - Generate <TxxBIT gtn tn value> operation from the following pattern:
     lsr tn' tn value
	 and tn" tn' 0x1
	 cmpeq gtn tn" 0x0 / cmpneq gtn tn" 0x0 / cmpeq gtn tn" 0x1 / cmpneq gtn tn" 0x1
	 where xx = F (false) for cmpeq 0x0 / cmpneq 0x1
	       xx = T (true) for cmpneq 0x0 / cmpeq 0x1

   - Generate <TxxBIT gtn tn i> operation from the following pattern:
     and tn' tn value_8_bits were value_8_bits is 2^i with 0<i<7
	 cmpeq gtn tn' 0x0 / cmpneq gtn tn' 0x0
	 where xx = F (false) for cmpeq
	       xx = T (true) for cmpneq

   - Generate <TxxAND gtn tn value_8_bits> operation from the following pattern:
     and tn' tn value_8_bits
	 cmpeq gtn tn' 0x0 / cmpneq gtn tn' 0x0
	 where xx = F (false) for cmpeq
	       xx = T (true) for cmpneq

   - Generate <TxxANDN gtn tn value_8_bits> operation from the following pattern:
     not tn' tn
	 and tn" tn' value_8_bits
	 cmpeq gtn tn" 0x0 / cmpneq gtn tn" 0x0
	 where xx = F (false) for cmpeq
	       xx = T (true) for cmpneq
   ===================================================================== */
static BOOL 
TxxOP_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo)
{

  // Predicate are not yet correctly handled
  // Transformation can be done if all pattern is under the same predicate

  INT64 tbit_value;
  TN *tbit_tn;

#if 0
  // Operation should be an immediate compare equal/not equal to 0x0 / 0x1
  if(!TOP_is_cmp(OP_code(op)))
	return FALSE;  

  INT cmp_opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
  INT cmp_opnd2_idx = OP_find_opnd_use(op, OU_opnd2);

  TN *cmp_tn1 = OP_opnd(op, cmp_opnd1_idx);
  TN *cmp_tn2 = OP_opnd(op, cmp_opnd2_idx);

  // Check that op is an immediate compare
  if(!TN_has_value(cmp_tn1) && !TN_has_value(cmp_tn2))
	return FALSE;

  INT cmp_opnd_imm = cmp_opnd2_idx;
  INT cmp_opnd = cmp_opnd1_idx;

  if(TN_has_value(cmp_tn1)){
	cmp_opnd_imm = cmp_opnd1_idx;
	cmp_opnd = cmp_opnd2_idx;
  }

  INT64 cmp_value = TN_value(OP_opnd(op, cmp_opnd_imm));

  if(cmp_value != 0 && cmp_value != 1)
	return FALSE;

  TN *cmp_predicate_tn = OP_opnd(op, OP_find_opnd_use(op, OU_predicate));

  /*
   * At this point:
   * - op is a compare
   * - cmp_opnd_imm is the immediate value 0 or 1
   * - cmp_opnd_cmp is the compare operand
   * - cmp_predicate_tn is the compare predicate
   */

  // Get the definer of compare operand. It must be an AND
  if(opnd_tninfo == NULL)
	return FALSE;

  // Look for the defining operation of compare operand

  EBO_TN_INFO *cmp_tninfo;
  cmp_tninfo = opnd_tninfo[cmp_opnd];

  INT and_opnd_imm = -1;
  INT and_opnd = -1;
  INT64 and_value = -1LL;
  TN* tn_and_value = NULL;
  OP* make_op = NULL;
  OP* and_op = NULL;

  // Find a definition in this basic block, whose first reference is on this compare operation.
  if (cmp_tninfo != NULL &&
	  cmp_tninfo->in_op != NULL &&
	  cmp_tninfo->in_bb == OP_bb(op) &&
	  (cmp_tninfo->reference_count == 1)) {

	// Check that definer is an immediate AND
	if(TOP_is_and(OP_code(cmp_tninfo->in_op))) {
        
      and_op = cmp_tninfo->in_op;
	  INT and_opnd1_idx = OP_find_opnd_use(and_op, OU_opnd1);
	  INT and_opnd2_idx = OP_find_opnd_use(and_op, OU_opnd2);

	  TN *and_tn1 = OP_opnd(and_op, and_opnd1_idx);
	  TN *and_tn2 = OP_opnd(and_op, and_opnd2_idx);

	  // Check that predicate is the same as the compare operation
          // (cbr) Support for guards on false
          if (!Opnds_Are_Equivalent (and_op, op,
                                     OP_find_opnd_use(and_op, OU_predicate),
                                     OP_find_opnd_use(op, OU_predicate)))

            return FALSE;

	  // Get immediate value
      if(TN_has_value(and_tn2)) {
          and_opnd_imm = and_opnd2_idx;
      }
	  and_opnd = and_opnd1_idx;

      if(and_opnd_imm != -1) {
          and_value = TN_value(OP_opnd(and_op, and_opnd_imm));
      } else {
          // and is not immediate we have to check definer of both tn
          // Look for the defining operation of AND operand
          EBO_TN_INFO *and_tninfo = NULL; 
          EBO_OP_INFO *and_opinfo = locate_opinfo_entry(cmp_tninfo);

          int it = 0;
          BOOL found = FALSE;
          do {
              if(and_opinfo != NULL) {
                  and_tninfo = and_opinfo->actual_opnd[and_opnd];
              }
              if(and_tninfo != NULL) {
                  TN* tmp;
                  if(and_tninfo->in_op != NULL &&
                     (and_tninfo->reference_count == 1) &&
                     (TOP_MAKE == OP_code(and_tninfo->in_op) ||
                      TOP_make == OP_code(and_tninfo->in_op)) &&
                     // (cbr) Support for guards on false
                     Opnds_Are_Equivalent (and_op, and_tninfo->in_op,
                                           OP_find_opnd_use(and_op, OU_predicate),
                                           OP_find_opnd_use(and_tninfo->in_op, OU_predicate))) {
                    found = TRUE;
                    make_op = and_tninfo->in_op;
                    tn_and_value = OP_opnd(make_op, 1);
                    and_value = TN_value(tn_and_value);
                  } else if((tmp = Get_Constant_Value(and_tninfo)) != NULL) {
                      and_value = TN_value(tmp);
                      found = TRUE;
                  }
              }
              and_opnd = and_opnd2_idx;
          } while(++it < 2 && !found);
          if(found) {
              // Here and_opnd contains the index of the immediate operand
              and_opnd = and_opnd == and_opnd2_idx? and_opnd1_idx:
                  and_opnd2_idx;
          } else {
              return FALSE;
          }
      }

	  /*
	   * When immediate value of AND is 1 two cases are present
	   * - definer of AND operand is a LSHR (logical shift right)
	   *   In this case we will map the operation sequence to TxxBIT <shift_count>
	   * - definer of AND operand is not and logical shift right operation
	   *   In this case we will map the operation sequence to TxxBIT <0x1>
	   *
	   * When immediate value of AND is 2^i
	   * - definer of AND operand is anything
	   *   In this case we will map the operation sequence to TxxBIT <0xi>
	   *
	   * When the immediate value of AND is anything else
	   * - definer of the AND operand is a NOT
	   *   In this case we will map the operation sequence to TxxANDN <and_value>
	   * - define of the AND operation is something else
	   *   In this case we will amp the operation sequence to TxxAND <and_value>
	   */

	}
	else
	  return FALSE;
  }
  else
	return FALSE;

  /*
   * At this point:
   * - cmp_tninfo->in_op is an immediate AND operation
   * - and_opnd_imm is the immediate operand of the AND operation
   * - and_opnd is the operand of the AND operation
   */

  // Get definer of AND operand. It can be a NOT (TxxANDN) or an LSHRU (TxxBIT) or anything else (TxxAND) or none (TxxAND)
  EBO_OP_INFO *and_opinfo = locate_opinfo_entry(cmp_tninfo);

  // Look for the defining operation of AND operand
  EBO_TN_INFO *and_tninfo = NULL; 
  if (and_opinfo != NULL) {
    and_tninfo = and_opinfo->actual_opnd[and_opnd];
  }

  // Find a definition in this basic block, whose first reference is on this compare operation.
  BOOL has_definer = FALSE;
  if (and_tninfo != NULL &&
	  and_tninfo->in_op != NULL &&
	  and_tninfo->in_bb == OP_bb(op) &&
	  (and_tninfo->reference_count == 1)) {

	has_definer = TRUE;
  }

  if(has_definer && (TOP_is_shru(OP_code(and_tninfo->in_op)) || TOP_is_shr(OP_code(and_tninfo->in_op)))){  // SHRU operation
	/*
	 * map [SHR(U) AND CMP] patterm to T(T)(F)BIT operation
	 */

	// The operation sequence can be mapped only if AND immediate value is 1
      if((and_value & (and_value - 1)) != 0LL)
	  return FALSE;

	// Check that predicate is the same as the compare operation

        // (cbr) Support for guards on false
      if (!Opnds_Are_Equivalent (op, and_tninfo->in_op,
                                 OP_find_opnd_use(op, OU_predicate),
                                 OP_find_opnd_use(and_tninfo->in_op, OU_predicate)))

	  return FALSE;

	// Get shift count
	INT shru_opnd1_idx = OP_find_opnd_use(and_tninfo->in_op, OU_opnd1);
	INT shru_opnd2_idx = OP_find_opnd_use(and_tninfo->in_op, OU_opnd2);

	TN *shru_tn1 = OP_opnd(and_tninfo->in_op, shru_opnd1_idx);
	TN *shru_tn2 = OP_opnd(and_tninfo->in_op, shru_opnd2_idx);

	// Get shift count and operand
	INT shru_opnd_shift_count = shru_opnd2_idx;
	INT shru_opnd = shru_opnd1_idx;

	// Check that operand of SHR operation is available at CMP operation
	OP *unavailable_at_OP = NULL;
	for (OP *op_iter = OP_next(and_tninfo->in_op); op_iter != op; op_iter = OP_next(op_iter)){
	  // Check hat operand of SHR operation has not been redefined between AND and CMP operation
	  if(OP_Defs_TN(op_iter, OP_opnd(and_tninfo->in_op, shru_opnd))){
		unavailable_at_OP = op_iter;
		break;
	  }
	}

	//TxxBIT operation will be placed before SHR(U) operand killer operation
	if(unavailable_at_OP){
	  // Check that guard register is not used between <unavailable_at_OP> and CMP operation
	  for (OP *op_iter = OP_next(unavailable_at_OP); op_iter != op; op_iter = OP_next(op_iter)){
		if(OP_Defs_TN(op_iter, OP_result(op, 0)) || OP_Refs_TN(op_iter, OP_result(op, 0))){
		  return FALSE;
		}
	  }
	}

	/*
	 * At this point:
	 * - and_tninfo->in_op is an immediate SHRU operation
	 * - shru_opnd_shift_count is the shift_count operand of the SHRU operation
	 * - shru_opnd is the operand of the SHRU operation
	 */

	/*
	 * Mapping of compare to ttbit/tfbit is done as follow:
	 * - cmpeq 0x1 / cmpneq 0x0 => ttbit
	 * - cmpeq 0x0 / cmpneq 0x0 => tfbit
	 */
    OP* shrOp = and_tninfo->in_op;
	TOP top_txxbit = TOP_UNDEFINED;
    TN* tn_shr_opnd = OP_opnd(shrOp, shru_opnd);
    TN* tn_shift_count = OP_opnd(shrOp, shru_opnd_shift_count);

    // and value must be 1 when we cannot determine shift count
    if(!TN_has_value(tn_shift_count) && and_value != 1LL) {
        return FALSE;
    } else if(and_value != 1LL) {
        if(cmp_value != 0) {
            // if we test against something else that 0, it may not be
            // equivalent
            return FALSE;
        }
        // here tn_shift_count has a value
        INT64 shift_count_value = TN_value(tn_shift_count);
        int pow_value = Get_Pow2(and_value);
        shift_count_value += pow_value;
        const ISA_REGISTER_CLASS_INFO* info =
            ISA_REGISTER_CLASS_Info(TN_register_class(OP_result(shrOp, 0)));
        if(shift_count_value > ISA_REGISTER_CLASS_INFO_Bit_Size(info) - 1) {
            if(TOP_is_shru(OP_code(shrOp))) {
                // Result of == 0 is always true
                if(OP_code(op) == TOP_cmpeq_i8 ||
                   OP_code(op) == TOP_cmpequ_i8 ||
                   OP_code(op) == TOP_cmpeqb_i8 ||
                   OP_code(op) == TOP_cmpequb_i8 ||
                   OP_code(op) == TOP_cmpeqh_i8 ||
                   OP_code(op) == TOP_cmpequh_i8) {
                    OPS opsTmp = OPS_EMPTY;
                    Expand_Copy(OP_result(op, 0), cmp_predicate_tn, True_TN,
                                &opsTmp);
                    BB_Insert_Ops(OP_bb(op), op, &opsTmp, FALSE);
                } else if(OP_code(op) == TOP_cmpne_i8 ||
                          OP_code(op) == TOP_cmpneu_i8 ||
                          OP_code(op) == TOP_cmpneb_i8 ||
                          OP_code(op) == TOP_cmpneub_i8 ||
                          OP_code(op) == TOP_cmpneh_i8 ||
                          OP_code(op) == TOP_cmpneuh_i8) {
                    OPS opsTmp = OPS_EMPTY;
                    Build_OP(TOP_andng, OP_result(op, 0), cmp_predicate_tn,
                             OP_result(op, 0), True_TN, &opsTmp);
                    BB_Insert_Ops(OP_bb(op), op, &opsTmp, FALSE);
                } else {
                    return FALSE;
                }
            } else {
                // we just have to test the bit of sign
                shift_count_value = ISA_REGISTER_CLASS_INFO_Bit_Size(info) - 1;
            }
        }
        tn_shift_count = TxxOP_sequence_set_tn_and_imm(NULL, shift_count_value,
                                                       and_op, -1, LC_imm_u5);
    }

	if((cmp_value == 1LL &&
		(OP_code(op) == TOP_cmpeq_i8 || OP_code(op) == TOP_cmpequ_i8 ||
		 OP_code(op) == TOP_cmpeqb_i8 || OP_code(op) == TOP_cmpequb_i8 ||
		 OP_code(op) == TOP_cmpeqh_i8 || OP_code(op) == TOP_cmpequh_i8
		 )) ||
	   (cmp_value == 0LL &&
		(OP_code(op) == TOP_cmpne_i8 || OP_code(op) == TOP_cmpneu_i8 ||
		 OP_code(op) == TOP_cmpneb_i8 || OP_code(op) == TOP_cmpneub_i8 ||
		 OP_code(op) == TOP_cmpneh_i8 || OP_code(op) == TOP_cmpneuh_i8))){
	  top_txxbit = TN_has_value(tn_shift_count) ? TOP_ttbit_i5 : TOP_ttbit_r;
	}
	else if((cmp_value == 0LL &&
			 (OP_code(op) == TOP_cmpeq_i8 || OP_code(op) == TOP_cmpequ_i8 ||
			  OP_code(op) == TOP_cmpeqb_i8 || OP_code(op) == TOP_cmpequb_i8 ||
			  OP_code(op) == TOP_cmpeqh_i8 || OP_code(op) == TOP_cmpequh_i8
			  )) ||
			(cmp_value == 1LL &&
			 (OP_code(op) == TOP_cmpne_i8 || OP_code(op) == TOP_cmpneu_i8 ||
			  OP_code(op) == TOP_cmpneb_i8 || OP_code(op) == TOP_cmpneub_i8 ||
			  OP_code(op) == TOP_cmpneh_i8 || OP_code(op) == TOP_cmpneuh_i8))){
	  top_txxbit = TN_has_value(tn_shift_count) ? TOP_tfbit_i5 : TOP_tfbit_r;
	}

	if(top_txxbit != TOP_UNDEFINED){

      // Fix for bug #30000. This move is to avoid generating wrong code on
      // shr Rx, Rx, cst
      TN* srcTn = Build_RCLASS_TN(TN_register_class(tn_shr_opnd));

      EBO_TN_INFO *shrOpndDef = get_tn_info(tn_shr_opnd);

      EBO_OP_INFO *shrOpinfo = locate_opinfo_entry(shrOpndDef);
      if(shrOpinfo)
          {
              OPS opsTmp = OPS_EMPTY;
              Expand_Copy(srcTn, True_TN, tn_shr_opnd, &opsTmp);

              // Insert the copy before the shift operation
              BB_Insert_Ops(OP_bb(op), shrOp, &opsTmp, TRUE);

              OP* defOp = shrOpinfo->in_op;
              for(int idx = 0; defOp && idx < OP_results(defOp); idx++)
                  {
                      if(OP_result(defOp, idx) == tn_shr_opnd)
                          {
                              inc_ref_count(shrOpinfo->actual_rslt[idx]);
                          }
                  }
          }
      else
          {
              srcTn = tn_shr_opnd;
              if(OP_result(and_tninfo->in_op, 0) == tn_shr_opnd)
                  {
                      // operand is modified (shift operation is not dead) and
                      // 'and' value is 2^i, so take the i th bit.
                      tn_shift_count = Gen_Literal_TN(Get_Pow2(and_value), 4);
                  }
          }


	  OPS ops = OPS_EMPTY;
	  Build_OP(top_txxbit, OP_result(op, 0), cmp_predicate_tn, srcTn, tn_shift_count, &ops);
	  if(unavailable_at_OP)
		BB_Insert_Ops(OP_bb(unavailable_at_OP), unavailable_at_OP, &ops, TRUE);
	  else
		BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
	}
	else
	  return FALSE;
  }
  else if(has_definer && OP_code(and_tninfo->in_op) == TOP_not){  // NOT operation
	/*
	 * map [NOT AND CMP] pattern to T(T)(F)AND operation
	 */


        // (cbr) Support for guards on false
    if (!Opnds_Are_Equivalent (op, and_tninfo->in_op,
                               OP_find_opnd_use(op, OU_predicate),
                               OP_find_opnd_use(and_tninfo->in_op, OU_predicate)))
	  return FALSE;

	INT not_opnd = OP_find_opnd_use(and_tninfo->in_op, OU_opnd1);

	// Check that operand of NOT operation is available at CMP operation
	for (OP *op_iter = OP_next(and_tninfo->in_op); op_iter != op; op_iter = OP_next(op_iter))
	  // Check hat operand of NOT operation has not been redefined between NOT and CMP operation
	  if(OP_Defs_TN(op_iter, OP_opnd(and_tninfo->in_op, not_opnd)))
		return FALSE;

	// AND immediate value must be 8 bits extended to 0
	if(and_value > 255LL || and_value < 0LL)
	  return FALSE;

    TN* tn_and_imm = TxxOP_sequence_set_tn_and_imm(make_op, and_value, and_op,
                                                   and_opnd_imm, LC_imm_u8);
    DevAssert(tn_and_imm && TN_has_value(tn_and_imm),
              ("Generated tn will not fit in immediate opnd"));
	/*
	 * At this point
	 * - immediate value of AND operation is an 8 bits value
	 * - and_tninfo->in_op is a NOT operation
	 */

	/*
	 * Mapping of compare to ttandn/tfandn is done as follow:
	 * - cmpneq 0x0 => ttandn
	 * - cmpeq 0x0 => tfandn
	 */
	TOP top_txxandn = TOP_UNDEFINED;

	if(cmp_value == 0LL &&
	   (OP_code(op) == TOP_cmpne_i8 || OP_code(op) == TOP_cmpneu_i8 ||
		OP_code(op) == TOP_cmpneb_i8 || OP_code(op) == TOP_cmpneub_i8 ||
		OP_code(op) == TOP_cmpneh_i8 || OP_code(op) == TOP_cmpneuh_i8)){
	  top_txxandn = TOP_ttandn;
	}
	else if(cmp_value == 0LL &&
			(OP_code(op) == TOP_cmpeq_i8 || OP_code(op) == TOP_cmpequ_i8 ||
			 OP_code(op) == TOP_cmpeqb_i8 || OP_code(op) == TOP_cmpequb_i8 ||
			 OP_code(op) == TOP_cmpeqh_i8 || OP_code(op) == TOP_cmpequh_i8)){
	  top_txxandn = TOP_tfandn;
	}

	if(top_txxandn != TOP_UNDEFINED){

	  INT not_opnd = OP_find_opnd_use(and_tninfo->in_op, OU_opnd1);

	  OPS ops = OPS_EMPTY;
	  Build_OP(top_txxandn, OP_result(op, 0), cmp_predicate_tn, OP_opnd(and_tninfo->in_op, not_opnd), tn_and_imm, &ops);
	  BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
	}
	else
	  return FALSE;

  }
  else{  // OTHERS
	/*
	 * map [AND CMP] pattern to T(T)(F)BIT/T(T)(F)AND operation
	 */

	// Check that operand of AND operation is available at CMP operation
	for (OP *op_iter = OP_next(cmp_tninfo->in_op); op_iter != op; op_iter = OP_next(op_iter))
	  // Check hat operand of AND operation has not been redefined between AND and compare operation
	  if(OP_Defs_TN(op_iter, OP_opnd(cmp_tninfo->in_op, and_opnd)))
		return FALSE;

	// Check if AND immediate value is a 2^i value
	BOOL f = !(and_value & (and_value - 1)) && and_value;

	if(f){
	  // Map pattern to TxxBIT

	  /*
	   * At this point
	   * - immediate value of AND operation is a 2^i value
	   */

	  /*
	   * Mappind of compare to ttbit/tfbit is done as follow:
	   * - cmpeq 0x1 / cmpneq 0x0 => ttbit
	   * - cmpeq 0x0 / cmpneq 0x0 => tfbit
	   */
	  TOP top_txxbit = TOP_UNDEFINED;

	  if((cmp_value == 1LL &&
		  (OP_code(op) == TOP_cmpeq_i8 || OP_code(op) == TOP_cmpequ_i8 ||
		   OP_code(op) == TOP_cmpeqb_i8 || OP_code(op) == TOP_cmpequb_i8 ||
		   OP_code(op) == TOP_cmpeqh_i8 || OP_code(op) == TOP_cmpequh_i8
		   )) ||
		 (cmp_value == 0LL &&
		  (OP_code(op) == TOP_cmpne_i8 || OP_code(op) == TOP_cmpneu_i8 ||
		   OP_code(op) == TOP_cmpneb_i8 || OP_code(op) == TOP_cmpneub_i8 ||
		   OP_code(op) == TOP_cmpneh_i8 || OP_code(op) == TOP_cmpneuh_i8))){
		top_txxbit = TOP_ttbit_i5;
	  }
	  else if((cmp_value == 0LL &&
			   (OP_code(op) == TOP_cmpeq_i8 || OP_code(op) == TOP_cmpequ_i8 ||
				OP_code(op) == TOP_cmpeqb_i8 || OP_code(op) == TOP_cmpequb_i8 ||
				OP_code(op) == TOP_cmpeqh_i8 || OP_code(op) == TOP_cmpequh_i8
				)) ||
			  (cmp_value == 1LL &&
			   (OP_code(op) == TOP_cmpne_i8 || OP_code(op) == TOP_cmpneu_i8 ||
				OP_code(op) == TOP_cmpneb_i8 || OP_code(op) == TOP_cmpneub_i8 ||
				OP_code(op) == TOP_cmpneh_i8 || OP_code(op) == TOP_cmpneuh_i8))){
		top_txxbit = TOP_tfbit_i5;
	  }

	  if(top_txxbit != TOP_UNDEFINED){

		// Get i in a 2^i value


        TN *tn = Gen_Literal_TN(Get_Pow2(and_value), 4);

		OPS ops = OPS_EMPTY;
		Build_OP(top_txxbit, OP_result(op, 0), cmp_predicate_tn, OP_opnd(cmp_tninfo->in_op, and_opnd), tn, &ops);
		BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
	  }
	  else
		return FALSE;
	}
	else {
	  // map pattern to TxxAND

	  // AND immediate value must be 8 bits extended to 0
	  if(and_value > 255LL || and_value < 0LL)
		return FALSE;
      TN* tn_and_imm = TxxOP_sequence_set_tn_and_imm(make_op, and_value, and_op,
                                                     and_opnd_imm, LC_imm_u8);
      DevAssert(tn_and_imm && TN_has_value(tn_and_imm),
                ("Generated tn will not fit in immediate opnd"));
	  /*
	   * At this point
	   * - immediate value of AND operation is an 8 bits value
	   */

	  /*
	   * Mapping of compare to ttand/tfand is done as follow:
	   * - cmpneq 0x0 => ttand
	   * - cmpeq 0x0 => tfand
	   */
	  TOP top_txxand = TOP_UNDEFINED;

	  if(cmp_value == 0LL &&
		  (OP_code(op) == TOP_cmpne_i8 || OP_code(op) == TOP_cmpneu_i8 ||
		   OP_code(op) == TOP_cmpneb_i8 || OP_code(op) == TOP_cmpneub_i8 ||
		   OP_code(op) == TOP_cmpneh_i8 || OP_code(op) == TOP_cmpneuh_i8)){
		top_txxand = TOP_ttand;
	  }
	  else if(cmp_value == 0LL &&
			   (OP_code(op) == TOP_cmpeq_i8 || OP_code(op) == TOP_cmpequ_i8 ||
				OP_code(op) == TOP_cmpeqb_i8 || OP_code(op) == TOP_cmpequb_i8 ||
				OP_code(op) == TOP_cmpeqh_i8 || OP_code(op) == TOP_cmpequh_i8)){
		top_txxand = TOP_tfand;
	  }

	  if(top_txxand != TOP_UNDEFINED){

		OPS ops = OPS_EMPTY;
		Build_OP(top_txxand, OP_result(op, 0), cmp_predicate_tn, OP_opnd(cmp_tninfo->in_op, and_opnd), tn_and_imm, &ops);
		BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
	  }
	  else
		return FALSE;
	}

  }
  return TRUE;
#endif
  return FALSE;
}

/* ====================================================================
 *   Is_Power_OF_2
 *   return TRUE if the val is a power of 2
 * ====================================================================
 */
static BOOL 
Is_Power_Of_2 (INT64 val)
{
  val &= 0xffffffffull;
  return ((val != 0) && ((val & (val-1)) == 0));
}

/* ====================================================================
 *   Get_Power_OF_2
 * ====================================================================
 */
static INT
Get_Power_Of_2 (INT64 val)
{
  INT i;
  INT64 pow2mask;
  val &= 0xffffffffull;
  pow2mask = 1;
  for ( i = 0; i < 64; ++i ) {
    if (val == pow2mask) return i;
    pow2mask <<= 1;
  }
  FmtAssert(FALSE, ("Get_Power_Of_2 unexpected value"));
}


static BOOL 
BxxOP_sequence(OP *op, TN **opnd_tn, EBO_TN_INFO **opnd_tninfo) {

  FmtAssert (FALSE, ("not yet implemented"));
#if 0

	if (!OP_ior(op) && !OP_ixor(op))
		return FALSE;

	INT or_opnd1_idx = OP_find_opnd_use(op, OU_opnd1);
	INT or_opnd2_idx = OP_find_opnd_use(op, OU_opnd2);

	TN *or_tn1 = OP_opnd(op, or_opnd1_idx);
	TN *or_tn2 = OP_opnd(op, or_opnd2_idx);

	// Check that op is an immediate or
	if (TN_has_value(or_tn1) || TN_has_value(or_tn2)) {

		INT or_opnd_imm = or_opnd2_idx;
		INT or_opnd = or_opnd1_idx;
	
		if (TN_has_value(or_tn1)) {
			or_opnd_imm = or_opnd1_idx;
			or_opnd = or_opnd2_idx;
		}
	
		INT64 or_value = TN_value(OP_opnd(op, or_opnd_imm));
		if (!Is_Power_Of_2(or_value))
			return FALSE;
	
		TN *or_predicate_tn = OP_opnd(op, OP_find_opnd_use(op, OU_predicate));
	
		// Get the definer of operand.
		if (opnd_tninfo == NULL)
			return FALSE;
	
		//Perform transformation 
		OPS ops = OPS_EMPTY;
	    TN *tn = Gen_Literal_TN(Get_Power_Of_2(or_value), 4);
	    TOP top_bxx = TOP_UNDEFINED;
	    if (OP_ixor(op)) top_bxx = TOP_bnot_i5;
	    if (OP_ior(op)) top_bxx = TOP_bset_i5;
	
	    OP *new_op = Mk_OP(top_bxx, OP_result(op, 0), or_predicate_tn, OP_opnd(op,or_opnd), tn);
	    if (OP_Pred_False(op, OP_find_opnd_use(op, OU_predicate))) {
	    	Set_OP_Pred_False(new_op, OP_find_opnd_use(new_op, OU_predicate));
	    }
	    OPS_Append_Op(&ops, new_op);
	    BB_Insert_Ops(OP_bb(op), op, &ops, FALSE);
	
	    return TRUE;
	}	
	
#endif
	return FALSE;
	
}

/* =====================================================================
 *    EBO_Special_Sequence
 *
 *    Look at an expression and it's inputs to identify special sequences
 *    that can be simplified.
 * =====================================================================
 */

extern BOOL EBO_in_pre;
extern BOOL EBO_in_before_unrolling;
extern BOOL EBO_in_after_unrolling;
extern BOOL EBO_in_peep;

BOOL 
EBO_Special_Sequence (
  OP *op,
  TN **opnd_tn,
  EBO_TN_INFO **opnd_tninfo
)
{
  BOOL ret;
  TOP opcode = OP_code(op);

  // Do not reselect ops with implicit interactions
  if (OP_has_implicit_interactions(op)) return FALSE;

#ifdef Is_True_On
  if (getenv("NO_EBO_SPECIAL")) return FALSE;
#endif

  if (opnd_tn == NULL || opnd_tninfo == NULL) return FALSE;

  if (EBO_Trace_Optimization) {
    fprintf(TFile,"%sEnter EBO_Special_Sequence for op %s:\n", EBO_trace_pfx, TOP_Name(opcode));
    Print_OP_No_SrcLine(op);
  }

  if (shift_op_sequence(op, opnd_tn, opnd_tninfo)) {
    if (EBO_Trace_Optimization) 
      fprintf(TFile,"%sApplied EBO_Special_Sequence: shift_op_sequence\n", EBO_trace_pfx);
    return TRUE;
  }

#ifndef TARG_ARM  
  
  /*  if (operand_special_immediate(op, opnd_tn, opnd_tninfo)) return TRUE;*/

  if (OP_ishr(op)) {
    if (shr_shl_sequence (op, opnd_tn, opnd_tninfo)) {
      if (EBO_Trace_Optimization) 
	fprintf(TFile,"%sApplied EBO_Special_Sequence: shr_shl_sequence\n", EBO_trace_pfx);
      return TRUE;
    }
  }

  if (OP_iadd(op)) {
    if (iadd_special_case (op, opnd_tn, opnd_tninfo)) {
      if (EBO_Trace_Optimization) 
	fprintf(TFile,"%sApplied EBO_Special_Sequence: iadd_special_case\n", EBO_trace_pfx);
      return TRUE;
    }
  }
  
  if (OP_icmp(op)) {
  }

  if (integer_to_branch_op_sequence(op, opnd_tn, opnd_tninfo)) return TRUE;
  if (branch_condition_sequence(op, opnd_tn, opnd_tninfo)) return TRUE;
#ifndef TARG_ARM
  if (opcode == TOP_mfb && mfb_op_sequence(op, opnd_tn, opnd_tninfo)) return TRUE;
#endif

  if (OP_memory(op) && base_offset_sequence(op, opnd_tn, opnd_tninfo)) {
    if (EBO_Trace_Optimization) 
      fprintf(TFile,"%sApplied EBO_Special_Sequence: base_offset_sequence\n", EBO_trace_pfx);
    return TRUE;
  }

  if (ext_move_sequence(op, opnd_tn, opnd_tninfo)) {
    if (EBO_Trace_Optimization) 
      fprintf(TFile,"%sApplied EBO_Special_Sequence: ext_move_sequence\n", EBO_trace_pfx);
    return TRUE;
  }

  if (and_or_sequence(op, opnd_tn, opnd_tninfo)) {
    if (EBO_Trace_Optimization) 
      fprintf(TFile,"%sApplied EBO_Special_Sequence: and_or_sequence\n", EBO_trace_pfx);
    return TRUE;
  }

#ifndef TARG_ARM
  if (Enable_Fpx && float_add_sub_sequence(op, opnd_tn, opnd_tninfo)) {
    if (EBO_Trace_Optimization) 
      fprintf(TFile,"%sApplied EBO_Special_Sequence: float_add_sub_sequence\n", EBO_trace_pfx);
    return TRUE;
  }

  if (Enable_Fpx && float_abs_sequence(op, opnd_tn, opnd_tninfo)) {
    if (EBO_Trace_Optimization) 
      fprintf(TFile,"%sApplied EBO_Special_Sequence: float_abs_sequence\n", EBO_trace_pfx);
    return TRUE;
  }

  if (Enable_Fpx && float_neg_sequence(op, opnd_tn, opnd_tninfo)) {
    if (EBO_Trace_Optimization) 
      fprintf(TFile,"%sApplied EBO_Special_Sequence: float_neg_sequence\n", EBO_trace_pfx);
    return TRUE;
  }
#endif

  if (operand_special_sequence(op, opnd_tn, opnd_tninfo))  {
    if (EBO_Trace_Optimization) 
      fprintf(TFile,"%sApplied EBO_Special_Sequence: operand_special_sequence\n", EBO_trace_pfx);
    return TRUE;
  }
#endif

  if (CG_AutoMod && !EBO_in_pre && !EBO_in_before_unrolling && !EBO_in_after_unrolling /*  && !EBO_in_peep */)
    if (OP_memory(op) && AutoMod_Sequence(op, opnd_tn, opnd_tninfo)) {
      if (EBO_Trace_Optimization) 
	fprintf(TFile,"%sApplied EBO_Special_Sequence: AutoMod_Sequence\n", EBO_trace_pfx);
      return TRUE;
    }

#ifndef TARG_ARM
  if(merge_compare_and_guards(op, opnd_tn, opnd_tninfo))
      {
          if (EBO_Trace_Optimization) 
              fprintf(TFile,"%sApplied EBO_Special_Sequence: merge_compare_and_guards\n", EBO_trace_pfx);
          return TRUE;
      }

  if(!EBO_in_pre && OP_memory(op) && !TOP_is_automod(OP_code(op)) &&
     merge_add_load_store(op, opnd_tn, opnd_tninfo)) {
      if (EBO_Trace_Optimization) 
          fprintf(TFile,"%sApplied EBO_Special_Sequence: merge_add_load_store\n", EBO_trace_pfx);
      return TRUE;
  }

  if (!EBO_in_pre && !EBO_in_before_unrolling && !EBO_in_after_unrolling && !EBO_in_peep)
      {
          if(transform_compare_and_shift(op, opnd_tn, opnd_tninfo))
              {
                  if (EBO_Trace_Optimization) 
                      fprintf(TFile,"%sApplied EBO_Special_Sequence: transform_compare_and_shift\n", EBO_trace_pfx);
                  return TRUE;
              }
      }

  if (!EBO_in_pre && !EBO_in_before_unrolling && !EBO_in_after_unrolling && !EBO_in_peep)
	if(TxxOP_sequence(op, opnd_tn, opnd_tninfo)){
	  if (EBO_Trace_Optimization) 
		fprintf(TFile,"%sApplied EBO_Special_Sequence: T(t)(f)BIT_Sequence\n", EBO_trace_pfx);

	  return TRUE;
	}

  if (!EBO_in_pre && !EBO_in_before_unrolling && !EBO_in_after_unrolling && !EBO_in_peep)
	if(BxxOP_sequence(op, opnd_tn, opnd_tninfo)){
	  if (EBO_Trace_Optimization) 
		fprintf(TFile,"%sApplied EBO_Special_Sequence: B(set)(clear)(not)\n", EBO_trace_pfx);
	  return TRUE;
	}
#endif
  return FALSE;
}

void EBO_Combine_Imm_Base_Offset()
{
    
}
