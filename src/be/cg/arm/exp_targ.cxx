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
 * Module: expand.c
 *
 * Description:
 *
 * This file contains the internals of code expansion. Its interface
 * is 'Exp_OP', which takes an OP, expands it into a list of OPs which
 * are appended to the oplist passed in.
 *
 * It handles all the macro expansions, special handling during
 * expansion and all the nitty gritty stuff that goes along with it.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "config.h"
#include "erglob.h"
#include "ercg.h"
#include "glob.h"
#include "tracing.h"
#include "util.h"

#include "symtab.h"
#include "opcode.h"
#include "intrn_info.h"
#include "const.h"	/* needed to manipulate target/host consts */
#include "targ_const.h"	/* needed to manipulate target/host consts */
#include "cgir.h"
#include "data_layout.h"
#include "stblock.h"
#include "cg_flags.h"
#include "cgtarget.h"
#include "cgexp.h"
#include "w2op.h"
#include "label_util.h"
#include "whirl2ops.h"
#include "dwarf_DST_mem.h"
#include "cgemit.h"
#include "cg_spill.h"	/* for rematerialization */

#include "topcode.h"
#include "targ_isa_lits.h"
#include "targ_isa_properties.h"
#include "config_TARG.h"

#include "symtab_defs.h"  /* for memory space enum */
#include "targ_sections.h"

/* Do we force inlining of extended immediates. */
static BOOL Inline_Extended_Immediate = FALSE;

/* Do we force long offset for stack access. */
BOOL Force_Long_Stack_Offset = TRUE;

/* Helpers for const_Reg/value tns. */
#define TN_Has_Value(tn) (TN_is_const_reg(tn) ? TRUE : TN_has_value(tn))
#define TN_Value(tn) ((tn == Zero_TN) ? 0 : TN_value(tn))

/* Dup_TN won't dup a dedicated tn, but for our purposes we
 * can just re-use the dedicated tn.  Don't want to re-use a
 * symbolic tn or it will mess up live ranges. */
/* DOESN'T WORK:  causes problems in Create_lvs because it causes
 * a use of a parm reg at the call-site, so it looks like the
 * parm-reg is incoming at the call?  This probably should work,
 * but for now we can use other routine that create a real dup tn. */
#define DUP_TN(tn)	Dup_TN_Even_If_Dedicated(tn)

/* ====================================================================
 * check_opd_rclass
 *
 * Check if source operand is well of the given register class. If not
 * expand a copy and return new tn.
 *
 * ====================================================================
 */

static inline TN*
check_opd_rclass(TN *opd, ISA_REGISTER_CLASS rc, OPS *ops ) {
  if(TN_is_register(opd) && TN_register_class(opd) != rc) {
    TN *tn_rc = Build_RCLASS_TN (rc);
    Expand_Copy(tn_rc,True_TN,opd,ops);
    return tn_rc;
  }
  else {
    return opd;
  }
}

/* ====================================================================
 * check_res_rclass
 *
 * Check if destination operand is well of the given register class. If not
 * expand a copy after OP is build.
 *
 * ====================================================================
 */

#define check_res_rclass(result,rc,ops,func)		\
  if(TN_register_class(result) != rc) {			\
    TN *tn_rc = Build_RCLASS_TN (rc);			\
    TN *old_res = result;				\
    result = tn_rc;					\
    func;						\
    Expand_Copy(old_res,True_TN,result,ops);		\
  }							\
  else {						\
    func;						\
  }

/* ====================================================================
 *   Get_Lit_Class_For_Memory_Access_Size
 *
 *   Returns the literal class of memory immediate offset depending on
 *   memory access size in bytes.
 * ====================================================================
 */
ISA_LIT_CLASS
Get_Lit_Class_For_Memory_Access_Size( INT32 size, BOOL is_signed, BOOL is_simulated ) {
  if(!is_signed && (size == 1 || size == 4)) {
    // Only for unsigned byte and signed/unsigned word
    if(is_simulated) {
      return LC_s13;
    }
    else {
      return LC_offset12;
    }
  }

  if(is_simulated) {
    return LC_s9;
  }
  else {
    return LC_offset8;
  }
}

/* ====================================================================
 *   Expand_Or_Inline_Immediate
 *
 *   This function should be called for each TN_is_constant() tn.
 *   Based on some criterion it may:
 *   - return an equivalent register TN
 *   - generate code for moving it into a register TN
 *   - return the tn itself (in this case it can be inlined)
 *   One of the criterion is Disable_Extended_Imm, if true an
 *   extended immediate is never generated.
 * ====================================================================
 */
TN *
Expand_Or_Inline_Immediate( OPERATION_WITH_INLINED_IMM operation, TN *src, TYPE_ID mtype, OPS *ops)
{
  TN *result= NULL;
  FmtAssert(TN_is_constant(src), ("Expected constant integer TN"));

  switch(operation) {
  case INL_COMPAR:
  case INL_ADD:
  case INL_SUB:
  case INL_MIN:
  case INL_MAX:
  case INL_SHIFT:
  case INL_OR:
  case INL_AND:
  case INL_XOR:
    if(ISA_LC_Value_In_Class (TN_value(src), LC_immed8)) {
      return src;
    }
    else {
      return Expand_Immediate_Into_Register (mtype, src, ops);
    }
    break;

  case INL_STD_LOAD:
  case INL_STD_STORE: 
    {
      ISA_LIT_CLASS lc = Get_Lit_Class_For_Memory_Access_Size(MTYPE_byte_size(mtype), MTYPE_signed(mtype), TRUE);
      if(ISA_LC_Value_In_Class (TN_value(src), lc)) {
	return src;
      }
      else {
	return Expand_Immediate_Into_Register (mtype, src, ops);
      }
    }
    break;

  case INL_UNKNOWN:
  default:
    return Expand_Immediate_Into_Register (mtype, src, ops);

  }

  FmtAssert(FALSE,("result ?"));
  return result;
}

/* ====================================================================
 *   Expand_Compose (tgt_tn, low_tn, high_tn, ops)
 * ====================================================================
 */
void
Expand_Compose (
  TN *tgt_tn,
  TN *low_tn,
  TN *high_tn,
  OPS *ops
)
{
  FmtAssert(FALSE,("Paired registers not supported on STxP70"));
}

/* ====================================================================
 *   Expand_Extract (low_tn, high_tn, src_tn, ops)
 * ====================================================================
 */
void
Expand_Extract (
  TN *low_tn,
  TN *high_tn,
  TN *src_tn,
  OPS *ops
)
{
  FmtAssert(FALSE,("Paired registers not supported on STxP70"));
}

/* ====================================================================
 *   Expand_Var_Compose provides an interface for generating compose
 *      instructions of arbitrary width, so that machine-independent
 *      parts of the compiler can abstract the width.
 * ====================================================================
 */
void
Exp_Var_Compose (
		 INT count,
		 TN *dst,
		 TN **srcs,
		 OPS *ops)
{
  FmtAssert (count == 2, ("Unsupported compose width (%d)", count));
  Expand_Compose (dst, srcs[0], srcs[1], ops);
}

/* ====================================================================
 *   Expand_Var_Extract provides an interface for generating extract
 *      instructions of arbitrary width, so that machine-independent
 *      parts of the compiler can abstract the width.
 * ====================================================================
 */
void
Exp_Var_Extract (
		 INT count,
		 TN **dsts,
		 TN *src,
		 OPS *ops)
{
  FmtAssert (count == 2, ("Unsupported extract width (%d)", count));
  Expand_Extract (dsts[0], dsts[1], src, ops);
}

/* ====================================================================
 *   Expand_Multi (tgt_tn, low_tn, high_tn, ops)
 * ====================================================================
 */
void
Expand_Multi (
  TN *tgt_tn,
  TN *src_tn,
  OPS *ops
)
{
  FmtAssert(FALSE,("Expand_Multi: unsupported multi copy"));
}

/* ====================================================================
 *   Expand_Copy (tgt_tn, guard, src_tn, ops)
 * ====================================================================
 */
void
Expand_Copy (
  TN *tgt_tn,
  TN *guard,
  TN *src_tn,
  OPS *ops
)
{
  TN *tmp;

  ISA_REGISTER_CLASS tgt_rc = TN_register_class(tgt_tn);
  ISA_REGISTER_CLASS src_rc = TN_register_class(src_tn);

  INT tgt_size = TN_size(tgt_tn);
  INT src_size = TN_size(src_tn);
    
  if (Trace_Exp) {
    fprintf(TFile, "Expand_Copy:\n");
    if (tgt_tn) { fprintf(TFile,"Target tn:"); Print_TN(tgt_tn,TRUE); fprintf(TFile,"\n"); }
    if (src_tn) { fprintf(TFile,"Source tn:"); Print_TN(src_tn,TRUE); fprintf(TFile,"\n"); }
  }

  FmtAssert (TN_is_register(src_tn),("not a register"));
  FmtAssert (guard == True_TN,("Guard not null"));
  
    
  switch (src_rc) {
    
  case ISA_REGISTER_CLASS_integer:
    
    if (tgt_rc == ISA_REGISTER_CLASS_integer) {
      if (tgt_size == 4 && src_size == 4) {
	Build_OP(TOP_mov_r_npc, tgt_tn, src_tn, ops);
	Set_OP_copy (OPS_last(ops));
      } else {
	goto unsupported;
      }
    }
    else goto unsupported;
    break;

  case ISA_REGISTER_CLASS_cpsr:
    if (tgt_rc == ISA_REGISTER_CLASS_cpsr) {
      TN *integer = Build_RCLASS_TN (ISA_REGISTER_CLASS_integer);
      Build_OP(TOP_mrs_cpsr,integer,src_tn,ops);
      Build_OP(TOP_msr_r_cpsr,tgt_tn,integer,ops);
    }
    else goto unsupported;
    break;
    

  default:
    FmtAssert(FALSE,("Not managed register class in expand_copy: %s",
		     ISA_REGISTER_CLASS_INFO_Name(ISA_REGISTER_CLASS_Info(src_rc))));
    goto unsupported; 
  }
  return;
 unsupported:
  FmtAssert(FALSE,("Expand_Copy: unsupported copy (%s -> %s)",
		   ISA_REGISTER_CLASS_INFO_Name(ISA_REGISTER_CLASS_Info(TN_register_class(src_tn))),
		   ISA_REGISTER_CLASS_INFO_Name(ISA_REGISTER_CLASS_Info(TN_register_class(tgt_tn))) ));
  return;
}

/* ====================================================================
 *   Expand_Copy (tgt_tn, guard, src_tn, ops)
 * ====================================================================
 */
void
Expand_Copy_On_False_Predicate (
  TN *tgt_tn,
  TN *guard,
  TN *src_tn,
  OPS *ops
)
{
  TN *tmp;

  ISA_REGISTER_CLASS tgt_rc = TN_register_class(tgt_tn);
  ISA_REGISTER_CLASS src_rc = TN_register_class(src_tn);

  INT tgt_size = TN_size(tgt_tn);
  INT src_size = TN_size(src_tn);
    
  if (Trace_Exp) {
    fprintf(TFile, "Expand_Copy:\n");
    if (tgt_tn) { fprintf(TFile,"Target tn:"); Print_TN(tgt_tn,TRUE); fprintf(TFile,"\n"); }
    if (src_tn) { fprintf(TFile,"Source tn:"); Print_TN(src_tn,TRUE); fprintf(TFile,"\n"); }
  }

  FmtAssert (TN_is_register(src_tn),("not a register"));
  FmtAssert (guard == NULL,("Guard not null"));
  
  switch (src_rc) {
    
  case ISA_REGISTER_CLASS_integer:
    
    if (tgt_rc == ISA_REGISTER_CLASS_integer) {
      if (tgt_size == 4 && src_size == 4) {
	FmtAssert(0,("Expand_Copy: unsupported copy on false guard"));
	
	if (TN_is_true_pred(guard)) Set_OP_copy (OPS_last(ops));
      } else {
	goto unsupported;
      }
    }
    else goto unsupported;
    break;
    
  default:
    goto unsupported;
  }
  return;
 unsupported:
  FmtAssert(FALSE,("Expand_Copy: unsupported copy"));
  return;
}

/* ====================================================================
 *   Expand_Convert_Length
 *
 *   This does truncation or extension of the int.
 * ====================================================================
 */
void
Expand_Convert_Length (
  TN *dest,
  TN *src,
  TN *length_tn,       // length to convert to
  TYPE_ID mtype,       // type converting from
  BOOL signed,         // obsolete for IA64
  OPS *ops
)
{
  INT16 new_length, old_length;     /* Length to convert to/from */
  // What length do we convert to?  It's the maximum of the target
  // register size, and the operator's result size:
  INT16 from_length;
  INT16 to_length;	/* Size that we have to extend to */
  TOP opc = TOP_UNDEFINED;
  // Do we sign-extend, or zero-extend?
  // If it's an extension, the signdedness of extension
  // depends on the signededness of rtype, else on the
  // signededness of desc
  BOOL signed_extension = MTYPE_is_signed(mtype);

  FmtAssert(   TN_register_class(dest) == ISA_REGISTER_CLASS_integer
	    && TN_register_class(src) == ISA_REGISTER_CLASS_integer
	       ,("Length convertion only supported on general purpose registers"));

  // This may be a truncation as well as extension.

  to_length = TN_value (length_tn);
  from_length = MTYPE_bit_size(mtype);

  if (from_length > to_length) {
    // truncation
    new_length = to_length;
    old_length = from_length;
  }
  else {
    // extension
    new_length = from_length;
    old_length = to_length;
  }

  if (old_length <= 32) {
    if (signed_extension) {
      TN *tmp = Build_TN_Like(dest);
      TN *tn_cst = Gen_Literal_TN(32-new_length, 4);
      Expand_Shift (tmp, src, tn_cst, mtype, shift_left, ops);
      Expand_Shift (dest, tmp, tn_cst, mtype, shift_aright, ops);
    }
    else {
      TN *tn_mask;
      INT64 mask = 1;
      INT32 i;
      
      for ( i = 0; i < new_length-1; ++i ) {
	mask |= mask << 1;
      }

      tn_mask =  Gen_Literal_TN(mask, 4);
      
      Expand_Binary_And (dest, src, tn_mask, mtype, ops);
    }
  }
  else
      FmtAssert(FALSE, ("Expand_Convert_Length: unknown src extension"));

  return;
}

/* ====================================================================
 *   Expand_Int_To_Ptr
 * ====================================================================
 */
void
Expand_Int_To_Ptr (
  TN *result,
  TYPE_ID rtype,
  TN *src,
  TYPE_ID desc,
  OPS *ops
)
{
  FmtAssert (FALSE,("Convert_Int_To_Ptr: unsupported"));

  return;
}

/* ====================================================================
 *   Expand_Ptr_To_Int
 *
 *   source is pointer, result is integer
 * ====================================================================
 */
void
Expand_Ptr_To_Int (
  TN *result,
  TYPE_ID rtype,
  TN *src,
  TYPE_ID desc,
  OPS *ops
)
{
  FmtAssert (FALSE,("Convert_Ptr_To_Int: unsupported"));

  return;
}

/* ====================================================================
 *   Expand_Convert
 *
 *   op2 is the length to convert to.
 * ====================================================================
 */
void
Expand_Convert (
  TN *result,
  TN *op1,
  TN *op2,
  TYPE_ID rtype,
  TYPE_ID desc,
  OPS  *ops
)
{
  /* conversions to int: */
  if (MTYPE_is_class_integer(rtype)) {

    if (desc == MTYPE_B) {
      Expand_Bool_To_Int (result, op1, rtype, ops);
      return;
    }
    else if (MTYPE_is_class_integer(desc)) {
      Expand_Convert_Length (result, op1, op2, desc, TRUE, ops);
      return;
    }
  }


  FmtAssert(0,("Expand_Convert: unsupported conversion %s -> %s",
		               MTYPE_name(desc), MTYPE_name(rtype)));
  return;
}

/* ====================================================================
 *   Fixup_32_Bit_Op (result, src, dest_type, ops)
 *
 *   Helper routine to do proper sign extension
 * ====================================================================
 */
static void
Fixup_32_Bit_Op(
  TN *result,
  TN *src,
  TYPE_ID dest_type,
  OPS *ops
)
{
  FmtAssert(FALSE,("Not Implemented"));

  if (dest_type == MTYPE_I8 || dest_type == MTYPE_U8) {
    Expand_Copy(result, True_TN, src, ops);
  } else {
    Expand_Convert_Length (result, src,
             Gen_Literal_TN(MTYPE_size_reg(dest_type), 4),
			   dest_type, MTYPE_is_signed(dest_type),ops);
  }
}

/* ====================================================================
 *   Exp_Immediate (dest, src, is_signed, ops)
 *
 * ====================================================================
 */
void
Exp_Immediate (
  TN *dest,
  TN *src,
  BOOL is_signed,
  OPS *ops
)
{
  INT64 val;
  TOP   top;
  TN    *tmp;

  if (TN_has_value(src)) {
    val = TN_value(src);
  }

  FmtAssert(TN_is_register(dest),("dest is not a register"));

  switch (TN_register_class(dest)) {
  case ISA_REGISTER_CLASS_integer:
    if(TN_has_value(src) && ISA_LC_Value_In_Class (val, LC_immed8)) {
      Build_OP (TOP_mov_i8_npc, dest, src, ops);
    } else if(TN_is_symbol(src) && ST_on_stack(TN_var(src))) {
      Build_OP(TOP_MAKE, dest, src, ops);
    } else if(TN_is_symbol(src)) {
      Expand_Symbol(dest, src, ops);      
    } else {
      Build_OP(TOP_MAKE, dest, src, ops);
    }
    break;
    
  default:
    FmtAssert(0, ("Exp_Immediate: unknown ISA_REGISTER_CLASS %s",
		  ISA_REGISTER_CLASS_INFO_Name(ISA_REGISTER_CLASS_Info(TN_register_class(dest)))));
  }


  return;
}

/* ====================================================================
 *   Expand Symbol into register
 * ====================================================================
 */
void
Expand_Symbol (
	       TN *dest,
	       TN *src,
	       OPS *ops
	       )
{
  TN *src1 = NULL;
  TN *src2 = NULL;

  FmtAssert(TN_is_symbol(src),("Not a symbol as source"));

  ST* st = TN_var(src);

  src1 = Dup_TN(src);
  src2 = Dup_TN(src);

  if(TN_register_class(dest) != ISA_REGISTER_CLASS_integer) {
    TN *integer = Build_RCLASS_TN (ISA_REGISTER_CLASS_integer);
    Build_OP(TOP_MAKE, integer, src, ops);
    Expand_Copy(dest,True_TN,integer,ops);
  }
  else {
    Build_OP(TOP_MAKE, dest, src, ops);
  }
}

/* ====================================================================
 *   Expand Immediate
 * ====================================================================
 */
void
Expand_Immediate (
  TN *dest, 
  TN *src, 
  TYPE_ID mtype, 
  OPS *ops
)
{
  FmtAssert((TN_is_constant(src)),
	    ("unexpected non-constant in Expand_Immediate"));
  FmtAssert((TN_has_value(src) || TN_is_symbol(src)),
	    ("expected value or const in Expand_Immediate"));
  //Exp_Immediate (dest, src, mtype, ops);
  Exp_Immediate (dest, src, MTYPE_signed(mtype) ? TRUE : FALSE, ops);
}

/* ====================================================================
 *   Expand_Immediate_Into_Register (src, ops)
 * ====================================================================
 */
TN*
Expand_Immediate_Into_Register (
  TYPE_ID  mtype,
  TN    *src,
  OPS   *ops
)
{
  /* load into reg and do reg case */
  TN *tmp = Build_TN_Of_Mtype (mtype);
  Expand_Immediate (tmp, src, mtype, ops);
  if (TN_has_value(src) && MTYPE_is_integral(mtype))
    CGSPILL_Attach_Intconst_Remat(tmp, mtype, TN_value(src));
  else if (TN_has_value(src) && MTYPE_is_float(mtype))
    CGSPILL_Attach_Floatconst_Remat(tmp, mtype, TN_value(src));
  return tmp;
}

/* ====================================================================
 *   Expand_Add
 * ====================================================================
 */
void
Expand_Add (
  TN *result,
  TN *src1,
  TN *src2,
  TYPE_ID mtype,
  OPS *ops
)
{
  TOP    new_opcode;
  TN    *tmp;
  TN    *tmp_result = NULL;

  FmtAssert (TN_is_register(src1) || TN_is_register(src2),
	     ("Expand_Add: both operands const ?"));
  FmtAssert (Register_Class_For_Mtype(mtype) == ISA_REGISTER_CLASS_integer,
	     ("Unexpected MTYPE: %s", MTYPE_name(mtype)));
  FmtAssert (MTYPE_is_integral(mtype) && MTYPE_byte_size(mtype) == 4,
	     ("Unexpected MTYPE: %s", MTYPE_name(mtype)));

  if (TN_is_constant(src1)) {
    // switch order of src so immediate is second
    Expand_Add (result, src2, src1, mtype, ops);
    return;
  }

  src1 = check_opd_rclass( src1, ISA_REGISTER_CLASS_integer, ops);
  src2 = check_opd_rclass( src2, ISA_REGISTER_CLASS_integer, ops);

  if (TN_Has_Value(src2)) {
    if (TN_Value(src2) == 0) {
      Expand_Copy(result, True_TN, src1, ops);
      return;
    }
    if(ISA_LC_Value_In_Class(TN_value(src2), LC_immed8)) {
      check_res_rclass( result, ISA_REGISTER_CLASS_integer, ops, Build_OP (TOP_add_i8_r_npc, result,
									   src1, src2, ops));
      return;
    }
    if (TN_Value(src2) < 0 && ISA_LC_Value_In_Class(-TN_Value(src2),LC_immed8)) {
      TN *tmp = Gen_Literal_TN(-TN_Value(src2), 4);
      check_res_rclass( result, ISA_REGISTER_CLASS_integer, ops, Build_OP(TOP_sub_i8_r_npc, result,
									  src1, tmp, ops));
      return;
    }
  } 

  // Process immediate operand
  // [CG]: Note that for the add, we only call Expand_Or_Inline_Immediate()
  // for TN_has_value() as TN_is_symbol is treated below in a special
  // way.
  if (TN_has_value(src2)) {
    src2 = Expand_Or_Inline_Immediate(INL_ADD,src2, mtype, ops);
  }

  if (TN_is_constant(src2)) {

    if (TN_has_value(src2)) {
      INT64 val = TN_value(src2);
      new_opcode = TOP_opnd_immediate_variant(TOP_add_r_r_npc, 1, val);
    }
    // symbolic constant, gp-relative or sp-relative
    else if (TN_is_symbol(src2)) {
      //
      // src1 is the base, src2 is the ofst
      //
      INT64 ofst;
      ST *base;

      tmp = Build_TN_Of_Mtype (Pointer_Mtype);
      Base_Symbol_And_Offset_For_Addressing (TN_var(src2),
                                       TN_offset(src2), &base, &ofst);

      // We must allow GOT offset operands to always be the operand of an addi,
      // because the ABI requires that the src1 is a register containing GP.
      if (Gen_GP_Relative  && ST_gprel(base)) {
	FmtAssert(FALSE,("GP relative not supported"));
#if 0
	new_opcode = TOP_addugp;
	Set_TN_is_reloc_da0to15(src2);
	if(ofst < 0) {
	  /* Need to generate a sub because we cannot garantee that
	     the symbol do not start at the begining of the GP area.
	  */
	  TN *tmp = Gen_Literal_TN(-ofst, 4);
	  Set_TN_offset(src2,TN_offset(src2)-ofst);
	  check_res_rclass(result, ISA_REGISTER_CLASS_integer, ops, Build_OP (new_opcode, result, GP_TN, True_TN, src2, ops));
	  Expand_Sub (result, result,tmp, Pointer_Mtype, ops);
	}
	else {
	  /* TODO: fix LC_imm_s16 to LC_imm_u16 in MDS. */
	  if(ISA_LC_Value_In_Class(ofst, LC_imm_s16)) {
	    check_res_rclass(result, ISA_REGISTER_CLASS_integer, ops, Build_OP (new_opcode, result, GP_TN, True_TN, src2, ops));
	  }
	  else {
	    TN *tmp = Gen_Literal_TN(ofst, 4);
	    Set_TN_offset(src2,TN_offset(src2)-ofst);
	    check_res_rclass(result, ISA_REGISTER_CLASS_integer, ops, Build_OP (new_opcode, result, GP_TN, True_TN, src2, ops));
	    Expand_Add (result, result,tmp, Pointer_Mtype, ops);
	  }
	}
#endif
	return;
      }
#if 0
      else if(TN_is_reloc_neg_got_disp (src2)
	      || TN_is_reloc_gotoff_tprel (src2)
	      || TN_is_reloc_gotoff_dtpldm (src2)
	      || TN_is_reloc_gotoff_dtpndx (src2)) {
	FmtAssert(FALSE,("Unmanaged relocation"));
      }
#endif
      else if (ST_on_stack(TN_var(src2))) {
#if 1
        // (cbr) if the offset is 0 and the symbol is on the stack
        // then just do a copy.
        if (base == SP_Sym && ofst == 0) {
          Expand_Copy (result, True_TN, src1, ops);
          return;
        }
#endif
	// On stack symbolic offset becomes an immediate
	// in the final code if it fit and if not force long stack offset.
	if(ISA_LC_Value_In_Class(ofst, LC_immed8) && !Force_Long_Stack_Offset) {
	  new_opcode = TOP_add_i8_r_npc;
	}
	else {
	  src2 = Expand_Immediate_Into_Register (mtype, src2, ops);
	  new_opcode = TOP_add_r_r_npc;
	}
      }
      else {
	Expand_Immediate(tmp, src2, mtype, ops);
        src2 = tmp;
	new_opcode = TOP_add_r_r_npc;
      }
    }
    else {
      FmtAssert(FALSE,("unexpected constant in Expand_Add"));
    }
  }
  else {
    // both registers:
    FmtAssert (TN_is_register(src2),
	              ("Expand_Add: unknown 2nd operand type"));
    FmtAssert(Register_Class_For_Mtype(mtype) == TN_register_class(src2),
	      ("Expand_Add: mtype screwed"));

    new_opcode = TOP_add_r_r_npc;
  }

  check_res_rclass(result, ISA_REGISTER_CLASS_integer, ops, Build_OP (new_opcode, result, src1, src2, ops));
  return;
}

/* ====================================================================
 *   Expand_Sub
 * ====================================================================
 */
void
Expand_Sub (
  TN *result,
  TN *src1,
  TN *src2,
  TYPE_ID mtype,
  OPS *ops
)
{
  TOP new_opcode;
  TN    *tmp;
  ST    *base;
  INT64  ofst, val;

  FmtAssert (TN_is_register(src1) || TN_is_register(src2),
	     ("Expand_Sub: both operands const ?"));
  FmtAssert (Register_Class_For_Mtype(mtype) == ISA_REGISTER_CLASS_integer,
	     ("Unexpected MTYPE: %s", MTYPE_name(mtype)));
  FmtAssert (MTYPE_is_integral(mtype) && MTYPE_byte_size(mtype) == 4,
	     ("Unexpected MTYPE: %s", MTYPE_name(mtype)));

  src1 = check_opd_rclass( src1, ISA_REGISTER_CLASS_integer, ops);
  src2 = check_opd_rclass( src2, ISA_REGISTER_CLASS_integer, ops);

  if (TN_Has_Value(src2)) {
    if(ISA_LC_Value_In_Class(TN_value(src2), LC_immed8)) {
      check_res_rclass(result, ISA_REGISTER_CLASS_integer, ops, Build_OP (TOP_sub_i8_r_npc, result, src1, src2, ops));
      return;
    }
    if (TN_Value(src2) == 0) {
      Expand_Copy(result, True_TN, src1, ops);
      return;
    }
  } 

  // Process immediate operand
  if (TN_is_constant(src1)) {
    src1 = Expand_Immediate_Into_Register (mtype, src1, ops);
  }
  if (TN_is_constant(src2)) {
    src2 = Expand_Or_Inline_Immediate(INL_SUB, src2, mtype, ops);
  }

  if (TN_has_value(src1)) {
    Expand_Immediate(tmp, src1, mtype, ops);
    src1 = tmp;
    new_opcode = TOP_sub_r_r_npc;
  }
  else if (TN_has_value(src2)) {
    INT64 val = (INT64)(INT32)(-TN_value(src2));
    TN *tmp = Gen_Literal_TN(val, 4);
    Expand_Add(result, src1, tmp, mtype, ops);
    return;
  }
  else {
    // default both registers
    new_opcode = TOP_sub_r_r_npc;
  }

  check_res_rclass(result, ISA_REGISTER_CLASS_integer, ops, Build_OP (new_opcode, result, src1, src2, ops));
  return;
}

/* ====================================================================
 *   Expand_Neg
 *
 *   Use the opcode that does not generate sticky flags effects.
 * ====================================================================
 */
void
Expand_Neg (
  TN *dest,
  TN *src,
  TYPE_ID mtype,
  OPS *ops
)
{
  FmtAssert (TN_is_register(src),
	     ("Expand_Neg: operand const ?"));

  if (MTYPE_is_float(mtype)) {
    if (mtype == MTYPE_F8) {
      if (Enable_64_Bits_Ops) {
	TN *low1 = Build_TN_Of_Mtype (MTYPE_U4);
	TN *high1 = Build_TN_Of_Mtype (MTYPE_U4);
	TN *value = Gen_Literal_TN(0x80000000, 4);
	TN *lowdest = low1;
	TN *highdest = Build_TN_Of_Mtype (MTYPE_U4);
	Expand_Extract(low1, high1, src, ops);
	Expand_Binary_Xor(highdest, high1, value, MTYPE_U4, ops);
	Expand_Compose(dest, lowdest, highdest, ops);
	return;
      } else {
	FmtAssert(0, ("Unexpected 8 bytes type"));
      }
    } else if (mtype == MTYPE_F4) {
      TN *value = Gen_Literal_TN(0x80000000LL, 4) ;
      Expand_Binary_Xor(dest, src, value, MTYPE_U4, ops);
      return ;
    } else {
	FmtAssert(0, ("Unexpected floating point type"));
    }
  }
  FmtAssert (Register_Class_For_Mtype(mtype) == ISA_REGISTER_CLASS_integer,
	     ("Unexpected MTYPE: %s", MTYPE_name(mtype)));
  FmtAssert (MTYPE_is_integral(mtype) && MTYPE_byte_size(mtype) == 4,
	     ("Unexpected MTYPE: %s", MTYPE_name(mtype)));

  Build_OP (TOP_rsb_i8_r_npc, dest, src, Get_Zero_TN(4), ops);
  return;
}

/* ====================================================================
 *   Expand_Abs
 * ====================================================================
 */
void
Expand_Abs (
  TN *dest,
  TN *src,
  TYPE_ID mtype,
  OPS *ops
)
{
  FmtAssert (TN_is_register(src),
	     ("Expand_Abs: operand const ?"));

  if (MTYPE_is_float(mtype)) {
    if (mtype == MTYPE_F8) {
      if (Enable_64_Bits_Ops) {
	TN *low1 = Build_TN_Of_Mtype (MTYPE_U4);
	TN *high1 = Build_TN_Of_Mtype (MTYPE_U4);
	TN *value = Gen_Literal_TN(0x7FFFFFFF, 4);
	TN *lowdest = low1;
	TN *highdest = Build_TN_Of_Mtype (MTYPE_U4);
	Expand_Extract(low1, high1, src, ops);
	Expand_Binary_And(highdest, high1, value, MTYPE_U4, ops);
	Expand_Compose(dest, lowdest, highdest, ops);
	return;
      } else {
	FmtAssert(0, ("Unexpected 8 bytes type"));
      }
    }
  }

  FmtAssert (Register_Class_For_Mtype(mtype) == ISA_REGISTER_CLASS_integer,
	     ("Unexpected MTYPE: %s", MTYPE_name(mtype)));
  

  if (MTYPE_is_integral(mtype)) {
    FmtAssert (MTYPE_byte_size(mtype) == 4,
	       ("Unexpected MTYPE: %s", MTYPE_name(mtype)));
      //
      // For dest = abs (src) generate:
      //  
      //            sub   negx = $r0.0, src
      //            max   dest = negx, src
      //
    TN *negx = Build_RCLASS_TN (ISA_REGISTER_CLASS_integer);
    Expand_Neg(negx,src,mtype,ops);
    Expand_Max(dest,negx,src,mtype,ops);
  } else if (MTYPE_is_float(mtype)) {
    FmtAssert (MTYPE_byte_size(mtype) == 4,
	       ("Unexpected MTYPE: %s", MTYPE_name(mtype)));
    TN *mask = Gen_Literal_TN(0x7fffffff, 4);
    Expand_Binary_And(dest, src, mask, MTYPE_I4, ops);
  } else {
    Is_True(FALSE,("abs doesn't handle MTYPE %s\n", MTYPE_name(mtype)));
  }
}

/* ====================================================================
 *   Expand_Shift
 * ====================================================================
 */
void
Expand_Shift (
  TN *result,
  TN *src1,
  TN *src2,
  TYPE_ID mtype,
  SHIFT_DIRECTION kind,
  OPS *ops
)
{
  TOP new_opcode = TOP_UNDEFINED;

  FmtAssert (TN_is_register(src1) || TN_is_register(src2),
	     ("Expand_Shift: operand const ?"));
  FmtAssert (Register_Class_For_Mtype(mtype) == ISA_REGISTER_CLASS_integer,
	     ("Unexpected MTYPE: %s", MTYPE_name(mtype)));

  // On this target shift value is mod 32
  if (TN_Has_Value(src2)) {
    if ((TN_Value(src2) & 0x1F) == 0) {
      Expand_Copy(result, True_TN, src1, ops);
      return;
    }
  } 

  // Process immediate operand
  if (TN_is_constant(src1)) {
    src1 = Expand_Immediate_Into_Register(mtype, src1, ops);
  }
  else {
    src1 = check_opd_rclass( src1, ISA_REGISTER_CLASS_integer, ops);
  }

  if (TN_is_constant(src2)) {
    src2 = Expand_Or_Inline_Immediate(INL_SHIFT,src2, mtype, ops);
  }
  else {
    src2 = check_opd_rclass( src2, ISA_REGISTER_CLASS_integer, ops);
  }
  
  switch (kind) {
    case shift_left:
      new_opcode = TOP_mov_npc_lsl_npc_npc;
      break;
    case shift_aright:
      new_opcode = TOP_mov_npc_asr_npc_npc;
      break;
    case shift_lright:
      new_opcode = TOP_mov_npc_lsr_npc_npc;
      break;
  }

  if (TN_has_value(src2))
    new_opcode = TOP_opnd_immediate_variant(new_opcode, 1, TN_value(src2));

  FmtAssert(new_opcode != TOP_UNDEFINED,
	                  ("Expand_Shift: shift is not handled"));

  Build_OP (new_opcode, result, src1, src2, ops);
  return;
}

/* ====================================================================
 *   Is_Power_OF_2
 *
 *   return TRUE if the val is a power of 2
 * ====================================================================
 */
#define IS_POWER_OF_2(val)	((val != 0) && ((val & (val-1)) == 0))

static BOOL
Is_Power_Of_2 (
  INT64 val,
  TYPE_ID mtype
)
{
  if (MTYPE_is_signed(mtype) && val < 0) val = -val;

  if (mtype == MTYPE_U4) val &= 0xffffffffull;

  return IS_POWER_OF_2(val);
}

/* ====================================================================
 *   Get_Power_OF_2
 * ====================================================================
 */
static INT
Get_Power_Of_2 (
  INT64 val,
  TYPE_ID mtype
)
{
  INT i;
  INT64 pow2mask;

  if (MTYPE_is_signed(mtype) && val < 0) val = -val;

  if (mtype == MTYPE_U4) val &= 0xffffffffull;

  pow2mask = 1;
  for ( i = 0; i < MTYPE_size_reg(mtype); ++i ) {
    if (val == pow2mask) return i;
    pow2mask <<= 1;
  }

  FmtAssert(FALSE, ("Get_Power_Of_2 unexpected value"));
  /* NOTREACHED */
}

/* ====================================================================
 *   Expand_Shift_Multiply
 *
 *   Make a shift if possible. src2 is a constant TN.
 * ====================================================================
 */
static BOOL
Expand_Shift_Multiply (
  TN *result,
  TYPE_ID mtype,
  TN *src1,
  TN *src2,
  OPS *ops
)
{
  INT val;
  TOP opcode = TOP_UNDEFINED;

  FmtAssert (TN_is_register(src1) && TN_has_value(src2),
	     ("Expand_Shift_Multiply: not register/const operands"));
  FmtAssert (Register_Class_For_Mtype(mtype) == ISA_REGISTER_CLASS_integer,
	     ("Unexpected MTYPE: %s", MTYPE_name(mtype)));
  FmtAssert (MTYPE_is_integral(mtype),
	     ("Unexpected MTYPE: %s", MTYPE_name(mtype)));

  // see if I can do a shift instead of the multiply
  if (Is_Power_Of_2(TN_value(src2), mtype)) {
    val = Get_Power_Of_2(TN_value(src2), mtype);
    TN *tmp = Gen_Literal_TN (val, MTYPE_byte_size(mtype));
    Expand_Shift (result, src1, tmp, mtype, shift_left, ops);
    
    // do we need to negate the result ?
    if (TN_value(src2) < 0) {
      Expand_Neg (result, result, mtype, ops);      
    }

    return TRUE;
  }

  return FALSE;

  FmtAssert(FALSE,("Cannot expand multiply using shift for power of 2 immediate"));
#if 0  
  // Special cases handled by shadd instructions
  val = TN_value (src2);
  switch (val) {
  case 3:
    opcode = TOP_sh1add_r;
    break;
  case 5:
    opcode = TOP_sh2add_r;
    break;
  case 9:
    opcode = TOP_sh3add_r;
    break;
  case 17:
    if (ISA_SUBSET_Member (ISA_SUBSET_Value, TOP_sh4add_r)) {
      opcode = TOP_sh4add_r;
    } else {
      opcode = TOP_UNDEFINED;
    }
    break;
  default:
    opcode = TOP_UNDEFINED;
  }

  if (opcode != TOP_UNDEFINED) {
    Build_OP(opcode, result, src1, src1, ops);
    return TRUE;
  }

#endif
  return FALSE;
}

/* ====================================================================
 *   Expand_Multiply
 * ====================================================================
 */
void
Expand_Multiply (
  TN *result,
  TYPE_ID rmtype,
  TN *src1,
  TYPE_ID s1mtype,
  TN *src2,
  TYPE_ID s2mtype,
  OPS *ops
)
{
  BOOL swapped = FALSE;
  TOP opcode = TOP_UNDEFINED;

  FmtAssert (TN_is_register(src1) || TN_is_register(src2),
 	     ("Expand_Multiply: both operands const ?"));
  FmtAssert (Register_Class_For_Mtype(rmtype) == ISA_REGISTER_CLASS_integer,
	     ("Unexpected MTYPE: %s", MTYPE_name(rmtype)));
  FmtAssert (MTYPE_is_integral(rmtype) && MTYPE_byte_size(rmtype) <= 4,
	     ("Unexpected MTYPE: %s", MTYPE_name(rmtype)));

  src1 = check_opd_rclass( src1, ISA_REGISTER_CLASS_integer, ops);
  src2 = check_opd_rclass( src2, ISA_REGISTER_CLASS_integer, ops);

  // If one of the operands is a constant, it must be the second one.
  if (TN_has_value(src1)) {
    Expand_Multiply(result, rmtype, src2, s2mtype, src1, s1mtype, ops);
    return;
  }
  
  if (TN_Has_Value(src2)) {
    if (TN_Value(src2)== 0) {
      Expand_Copy(result, True_TN, src2, ops);
      return;
    }
  } 

  if (TN_Has_Value(src2)) {
    if (TN_Value(src2)== 1) {
      Expand_Copy(result, True_TN, src1, ops);
      return;
    }
  } 

  // Try to generate shift or shift add instead of mul
  if (TN_has_value(src2)) {
    if (Expand_Shift_Multiply (result, rmtype, src1, src2, ops)) 
      return;
  }


  // Get range for value
  if (TN_has_value(src2)) {
    INT64 value = TN_value(src2);
    if (value >= -(1<<15) && value < (1<<15)-1) {
      s2mtype = MTYPE_I2;
    } else if (value >= 0 && value < (1<<16)-1) {
      s2mtype = MTYPE_U2;
    }
  }

  // Process immediate operand
  if (TN_is_constant(src2)) {
    src2 = Expand_Or_Inline_Immediate(INL_UNKNOWN,src2, rmtype, ops);
  }

  opcode = TOP_mul;

  if (opcode == TOP_UNDEFINED)
    FmtAssert(FALSE, ("Expand_Multiply: failed"));

  TN *tmp1 = src1;
  TN *tmp2 = src2;
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(opcode);
  if(ISA_OPERAND_INFO_Conflicts(oinfo, 0)) {
    /* Has conflict between res 0 and opnd 0 ? */
    if(ISA_OPERAND_INFO_Has_Conflict(oinfo, 0, 0) && src1 == result) {
      tmp1 = Build_TN_Like(src1);
      Expand_Copy(tmp1,True_TN,src1,ops);
    }
    /* Has conflict between res 0 and opnd 1 ? */
    if(ISA_OPERAND_INFO_Has_Conflict(oinfo, 0, 1) && src2 == result) {
      tmp2 = Build_TN_Like(src2);
      Expand_Copy(tmp2,True_TN,src2,ops);
    }
  }
  check_res_rclass(result, ISA_REGISTER_CLASS_integer, ops,
		   Build_OP(opcode, result, tmp1, tmp2, ops));
  
  return;
}

/* ====================================================================
 *   Expand_Madd
 * ====================================================================
 */
void
Expand_Madd (
  TN *result,
  TYPE_ID rmtype,
  TN *src0,
  TYPE_ID s0mtype,
  TN *src1,
  TYPE_ID s1mtype,
  TN *src2,
  TYPE_ID s2mtype,
  BOOL is_neg,
  OPS *ops
)
{
  FmtAssert((rmtype == MTYPE_I4 || rmtype == MTYPE_U4) &&
	    MTYPE_is_integral(s0mtype) &&
	    MTYPE_is_integral(s1mtype) &&
	    MTYPE_is_integral(s2mtype),("Not integral type"));

  TOP opcode = TOP_mla;

  switch(rmtype) {
  case MTYPE_U4:
  case MTYPE_I4: {
    switch(s0mtype) {
    case MTYPE_I4:
    case MTYPE_U4: {
      if((s1mtype == MTYPE_I2 || s1mtype == MTYPE_I1 || s1mtype == MTYPE_U1) &&
	 (s2mtype == MTYPE_I2 || s2mtype == MTYPE_I1 || s2mtype == MTYPE_U1)) {
	opcode = TOP_smlabb;
      }
      break;
    }
    }
    break;
  }
  }

  if (opcode == TOP_UNDEFINED)
    FmtAssert(FALSE, ("Expand_Madd: failed"));

  TN *tmp2 = src0;
  TN *tmp0 = src1;
  TN *tmp1 = src2;
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(opcode);
  if(ISA_OPERAND_INFO_Conflicts(oinfo, 0) && src1 == result) {
    /* Has conflict between res 0 and opnd 0 ? */
    if(ISA_OPERAND_INFO_Has_Conflict(oinfo, 0, 0) && src1 == result) {
      tmp0 = Build_TN_Like(src1);
      Expand_Copy(tmp0,True_TN,src1,ops);
    }
    /* Has conflict between res 0 and opnd 1 ? */
    if(ISA_OPERAND_INFO_Has_Conflict(oinfo, 0, 1) && src2 == result) {
      tmp1 = Build_TN_Like(src2);
      Expand_Copy(tmp1,True_TN,src2,ops);
    }
    /* Has conflict between res 0 and opnd 2 ? */
    if(ISA_OPERAND_INFO_Has_Conflict(oinfo, 0, 2) && src0 == result) {
      tmp2 = Build_TN_Like(src0);
      Expand_Copy(tmp2,True_TN,src0,ops);
    }
  }

  TN *tmp_res = result;

  if(is_neg) {
    tmp_res = Build_RCLASS_TN (ISA_REGISTER_CLASS_integer);
  }

  if(opcode == TOP_smlabb) {
    TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr,1,0);
    check_res_rclass(tmp_res, ISA_REGISTER_CLASS_integer, ops,
		     Build_OP(opcode, cpsr, tmp_res, tmp0, tmp1, tmp2, ops));
  }
  else {
    check_res_rclass(tmp_res, ISA_REGISTER_CLASS_integer, ops,
		     Build_OP(opcode, tmp_res, tmp0, tmp1, tmp2, ops));
  }

  if(is_neg) {
    Expand_Neg(result, tmp_res, rmtype, ops);
  }
  return;
}

/* ====================================================================
 *   Expand_Msub
 * ====================================================================
 */
void
Expand_Msub (
  TN *result,
  TYPE_ID rmtype,
  TN *src0,
  TYPE_ID s0mtype,
  TN *src1,
  TYPE_ID s1mtype,
  TN *src2,
  TYPE_ID s2mtype,
  BOOL is_neg,
  OPS *ops
)
{
  FmtAssert((rmtype == MTYPE_I4 || rmtype == MTYPE_U4) &&
	    MTYPE_is_integral(s0mtype) &&
	    MTYPE_is_integral(s1mtype) &&
	    MTYPE_is_integral(s2mtype),("Not integral type"));

  TOP opcode = TOP_mul;

  switch(rmtype) {
  case MTYPE_U4:
  case MTYPE_I4: {
    switch(s0mtype) {
    case MTYPE_I4:
    case MTYPE_U4: {
      if((s1mtype == MTYPE_I2 || s1mtype == MTYPE_I1 || s1mtype == MTYPE_U1) &&
	 (s2mtype == MTYPE_I2 || s2mtype == MTYPE_I1 || s2mtype == MTYPE_U1)) {
	opcode = TOP_smulbb;
      }
      break;
    }
    }
    break;
  }
  }

  if (opcode == TOP_UNDEFINED)
    FmtAssert(FALSE, ("Expand_Msub: failed"));

  TN *tmp0 = src1;
  TN *tmp1 = src2;
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(opcode);
  if(ISA_OPERAND_INFO_Conflicts(oinfo, 0)) {
    /* Has conflict between res 0 and opnd 0 ? */
    if(ISA_OPERAND_INFO_Has_Conflict(oinfo, 0, 0) && src1 == result) {
      tmp0 = Build_TN_Like(src1);
      Expand_Copy(tmp0,True_TN,src1,ops);
    }
    /* Has conflict between res 0 and opnd 1 ? */
    if(ISA_OPERAND_INFO_Has_Conflict(oinfo, 0, 1) && src2 == result) {
      tmp1 = Build_TN_Like(src2);
      Expand_Copy(tmp1,True_TN,src2,ops);
    }
  }

  TN *tmp_res = Build_RCLASS_TN (ISA_REGISTER_CLASS_integer);
  Build_OP(opcode, tmp_res, tmp0, tmp1, ops);
  if(is_neg) {
    Expand_Sub(result, src0, tmp_res, rmtype, ops);
  }
  else {
    Expand_Sub(result, tmp_res, src0, rmtype, ops);
  }
  
  return;
}

/* ====================================================================
 *   Expand_High_Multiply
 *
 *   return high part of multiply result
 * ====================================================================
 */
void
Expand_High_Multiply (
  TN *result,
  TN *src1,
  TN *src2,
  TYPE_ID mtype,
  OPS *ops
)
{
  FmtAssert(FALSE,("Not Implemented"));

  return;
}

/* ====================================================================
 *   Expand_Normalize_Logical
 * ====================================================================
 */

static void
Expand_Normalize_Logical (
  TN *src,
  OPS *ops
)
{
  // [CG]: Should not be called on this target as normalization is
  // not necessary
  // 
  FmtAssert(0,("Expand_Normalize_Logical should not be called"));
  
  return;
}

static void
Expand_Logical_Move (
  TN *dest,
  TN *src,
  VARIANT variant,
  OPS *ops
)
{
  /* dest = (src != 0) ? 1 : 0 */

  TOP opcode = TOP_UNDEFINED;

  // Trivial operand
  if (TN_Has_Value(src)) {
    if (TN_Value(src) == 0) { 
      Expand_Immediate(dest, Get_Zero_TN(4), MTYPE_I4, ops);
    } else {
      Expand_Immediate(dest, Gen_Literal_TN(1, 4), MTYPE_I4, ops);
    }
    return;
  }
  
  // Process immediate operand
  if (TN_is_constant(src)) {
    src = Expand_Or_Inline_Immediate(INL_COMPAR,src, MTYPE_I4, ops);
  }

  if(TN_is_register(src)) {
    src = check_opd_rclass( src, ISA_REGISTER_CLASS_integer, ops);
  }
  
  if (TN_register_class(dest) == ISA_REGISTER_CLASS_integer) {
    opcode = TOP_cmp_i8_r;
  }

  FmtAssert(opcode != TOP_UNDEFINED, ("Expand_Logical_Move"));
  
  if(TN_register_class(dest) == ISA_REGISTER_CLASS_integer) {
    TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr,1,0);
    Build_OP(opcode, cpsr, src, Get_Zero_TN(4), ops);
    Build_OP (TOP_mov_i8_npc, dest, Get_Zero_TN(4), ops);
    Build_OP (TOP_mov_i8_npc_cond, dest, cpsr, Gen_Enum_TN(ISA_ECV_cond_ne), Gen_Literal_TN(1, 4), ops);
  }

  return;
}

/* ====================================================================
 *   Expand_Logical_Not
 * ====================================================================
 */
void
Expand_Logical_Not (
  TN *dest,
  TN *src,
  VARIANT variant,
  OPS *ops
)
{
  FmtAssert(TN_size(dest) <= 4, ("Result TN size unexpected"));

  /* dest = (src == 0) ? 1 : 0 */

  TOP opcode = TOP_UNDEFINED;

  // Trivial operand
  if (TN_Has_Value(src)) {
    if (TN_Value(src) == 0) { 
      Expand_Immediate(dest, Gen_Literal_TN(1, 4), MTYPE_I4, ops);
    } else {
      Expand_Immediate(dest, Get_Zero_TN(4), MTYPE_I4, ops);
    }
    return;
  }
  
  // Process immediate operand
  if (TN_is_constant(src)) {
    src = Expand_Or_Inline_Immediate(INL_COMPAR,src, MTYPE_I4, ops);
  }

  if(TN_is_register(src)) {
    src = check_opd_rclass( src, ISA_REGISTER_CLASS_integer, ops);
  }
  
  if (TN_register_class(dest) == ISA_REGISTER_CLASS_integer) {
    opcode = TOP_cmp_i8_r;
  }
  
  FmtAssert(opcode != TOP_UNDEFINED, ("Expand_Logical_Not"));
  
  if(TN_register_class(dest) == ISA_REGISTER_CLASS_integer) {
    TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr,1,0);
    Build_OP(opcode, cpsr, src, Get_Zero_TN(4), ops);
    Build_OP (TOP_mov_i8_npc, dest, Get_Zero_TN(4), ops);
    Build_OP (TOP_mov_i8_npc_cond, dest, cpsr, Gen_Enum_TN(ISA_ECV_cond_eq), Gen_Literal_TN(1, 4), ops);
  }

  return;
}

/* ====================================================================
 *   Expand_Logical_And_Or
 *
 *
 *	dest = (src1 != 0 && src2 != 0) ? 1 : 0  (normalization)
 *	sltu	t1, 0, s1		(if not normalized)
 *	sltu	t2, 0, s2		(if not normalized)
 *	and/or	d, t1, t2
 * ====================================================================
 */
static void
Expand_Logical_And_Or (
  TOP action,
  TN *dest,
  TN *src1,
  TN *src2,
  VARIANT variant,
  OPS *ops
)
{
 /*
  *  if CG_EXP_normalize is true we must normalized the operands
  *  (if not already normalized)
  */
  if (!V_normalized_op1(variant) && CGEXP_normalize_logical)
  {
    Expand_Normalize_Logical (src1, ops);
  }
  if (!V_normalized_op2(variant) && CGEXP_normalize_logical)
  {
    Expand_Normalize_Logical (src2, ops);
  }

  Build_OP (action, dest, src1, src2, ops);
}

/* ====================================================================
 *   Expand_Logical_And
 * dest = (src1 != 0 && src2 != 0) ? 1 : 0
 * ====================================================================
 */
void
Expand_Logical_And (
  TN *dest,
  TN *src1,
  TN *src2,
  VARIANT variant,
  OPS *ops
)
{
  TOP opcode = TOP_UNDEFINED;
  TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr,1,0);
  
  FmtAssert(TN_size(dest) <= 4, ("Result TN size unexpected"));
  FmtAssert (TN_is_register(src1) || TN_is_register(src2),
	     ("Expand_Logical_And: both operands const ?"));

  if (TN_is_constant(src1)) {
    // switch order of src so immediate is second
    Expand_Logical_And (dest, src2, src1, variant, ops);
    return;
  }

  // Process immediate operand
  if (TN_is_constant(src2)) {
    src2 = Expand_Or_Inline_Immediate(INL_UNKNOWN,src2, MTYPE_I4, ops);
  }
  if (TN_Has_Value(src2)) {
    if (TN_Value(src2) == 0) 
      Expand_Immediate(dest, Get_Zero_TN(4), MTYPE_I4, ops);
    else
      Expand_Logical_Move(dest, src1, 0, ops);
    return;
  }

  if(TN_is_register(src1)) {
    src1 = check_opd_rclass( src1, ISA_REGISTER_CLASS_integer, ops);
  }

  if(TN_is_register(src2)) {
    src2 = check_opd_rclass( src2, ISA_REGISTER_CLASS_integer, ops);
  }

  if (TN_register_class(dest) == ISA_REGISTER_CLASS_integer) {
    opcode = TOP_cmp_i8_r;
    Build_OP(opcode, cpsr, src1, Get_Zero_TN(4), ops);
    opcode = TOP_cmp_i8_r_cond;
    Build_OP(opcode, cpsr, cpsr, Gen_Enum_TN(ISA_ECV_cond_ne), src2, Get_Zero_TN(4), ops);
  }
  
  FmtAssert(opcode != TOP_UNDEFINED, ("Expand_Logical_And"));

  if(TN_register_class(dest) == ISA_REGISTER_CLASS_integer) {
    Build_OP (TOP_mov_i8_npc, dest, Get_Zero_TN(4), ops);
    Build_OP (TOP_mov_i8_npc_cond, dest, cpsr, Gen_Enum_TN(ISA_ECV_cond_ne), Gen_Literal_TN(1, 4), ops);
  }

  return;
}

/* ========================================================================================
 *   Expand_Logical_Or
 * dest = (src1 != 0 || src2 != 0) ? 1 : 0 <=> dest = (src1 == 0 && src2 == 0) ? 0 : 1
 * ========================================================================================
 */
void
Expand_Logical_Or (
  TN *dest,
  TN *src1,
  TN *src2,
  VARIANT variant,
  OPS *ops
)
{
  TOP opcode = TOP_UNDEFINED;
  TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr,1,0);

  FmtAssert(TN_size(dest) <= 4, ("Result TN size unexpected"));
  FmtAssert (TN_is_register(src1) || TN_is_register(src2),
	     ("Expand_Logical_Or: both operands const ?"));

  if (TN_is_constant(src1)) {
    // switch order of src so immediate is second
    Expand_Logical_Or (dest, src2, src1, variant, ops);
    return;
  }

  // Process immediate operand
  if (TN_is_constant(src2)) {
    src2 = Expand_Or_Inline_Immediate(INL_UNKNOWN,src2, MTYPE_I4, ops);
  }
  
  if (TN_Has_Value(src2)) {
    if (TN_Value(src2) == 0) 
      Expand_Logical_Move(dest, src1, 0, ops);
    else
      Expand_Immediate(dest, Gen_Literal_TN(1, 4), MTYPE_I4, ops);
    return;
  }

  if(TN_is_register(src1)) {
    src1 = check_opd_rclass( src1, ISA_REGISTER_CLASS_integer, ops);
  }

  if(TN_is_register(src2)) {
    src2 = check_opd_rclass( src2, ISA_REGISTER_CLASS_integer, ops);
  }

  if (TN_register_class(dest) == ISA_REGISTER_CLASS_integer) {
    opcode = TOP_cmp_i8_r;
    Build_OP(opcode, cpsr, src1, Get_Zero_TN(4), ops);
    opcode = TOP_cmp_i8_r_cond;
    Build_OP(opcode, cpsr, cpsr, Gen_Enum_TN(ISA_ECV_cond_eq), src2, Get_Zero_TN(4), ops);
  }
  
  FmtAssert(opcode != TOP_UNDEFINED, ("Expand_Logical_And"));

  if(TN_register_class(dest) == ISA_REGISTER_CLASS_integer) {
    Build_OP (TOP_mov_i8_npc, dest, Gen_Literal_TN(1, 4), ops);
    Build_OP (TOP_mov_i8_npc_cond, dest, cpsr, Gen_Enum_TN(ISA_ECV_cond_eq), Get_Zero_TN(4), ops);
  }

  return;
}

/* ====================================================================
 *   Expand_Binary_Compliment
 * ====================================================================
 */
void
Expand_Binary_Complement (
  TN *dest,
  TN *src,
  TYPE_ID mtype,
  OPS *ops
)
{
  Is_True(MTYPE_is_class_integer(mtype),("not integer for complement"));

  src = check_opd_rclass( src, ISA_REGISTER_CLASS_integer, ops);

  if (MTYPE_byte_size(mtype) == 8) {
    if (Enable_64_Bits_Ops) {
      TN *low1 = Build_TN_Of_Mtype (MTYPE_U4);
      TN *high1 = Build_TN_Of_Mtype (MTYPE_U4);
      TN *lowres = Build_TN_Of_Mtype (MTYPE_U4);
      TN *highres = Build_TN_Of_Mtype (MTYPE_U4);
      Expand_Extract(low1, high1, src, ops);
      Expand_Binary_Complement (lowres, low1, MTYPE_U4, ops);
      Expand_Binary_Complement (highres, high1, MTYPE_U4, ops);
      Expand_Compose(dest, lowres, highres, ops);
      return;
    } else {
      FmtAssert(0, ("Unexpected 8 bytes type"));
    }
  }

  check_res_rclass(dest, ISA_REGISTER_CLASS_integer, ops, Build_OP (TOP_mvn_r_npc, dest, src, ops));
}


/* ====================================================================
 *   Expand_Special_And_Immed
 *
 * Expand special cases of AND with an immediate. These are cases
 * where the constant is too big for TOP_and_i, but can be handled
 * more simply than loading the constant into a register and using TOP_and.
 *
 * NOTE: that 'mix' could be used to zero fixed patterns of bytes and
 * shorts, but I couldn't find a case to trigger it so I left it out -- Ken
 * ====================================================================
 */
BOOL
Expand_Special_And_Immed (
  TN *dest,
  TN *src1,
  INT64 imm,
  OPS *ops
)
{
  UINT len;
  UINT start;
  UINT64 v;

  FmtAssert (TN_is_register(src1),
	     ("Expand_Spacial_And_Immed: operand const ?"));

  if (imm == 0) {
    Expand_Immediate(dest, Gen_Literal_TN(0, 4), MTYPE_I4, ops);
    return TRUE;
  } else if (((INT32)imm & -1) == -1) {
    Expand_Copy(dest, True_TN, src1, ops);
    return TRUE;
  } else if (imm == 0xFFFF && Target_ISA == TARGET_ISA_armv6) {
    Build_OP(TOP_uxth_npc_npc, dest, src1, ops);
    return TRUE;
  }

  return FALSE;
}

/* ====================================================================
 *   Expand_Binary_And_Or
 * ====================================================================
 */
static void
Expand_Binary_And_Or (
  TOP action,
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID mtype,
  OPS *ops
)
{
  FmtAssert(MTYPE_is_class_integer(mtype),("can't handle non integer mtype"));

  src1 = check_opd_rclass( src1, ISA_REGISTER_CLASS_integer, ops);
  src2 = check_opd_rclass( src2, ISA_REGISTER_CLASS_integer, ops);
  check_res_rclass(dest, ISA_REGISTER_CLASS_integer, ops, Build_OP (action, dest, src1, src2, ops));
  return;
}

/* ====================================================================
 *   Expand_Binary_And
 * ====================================================================
 */
void
Expand_Binary_And (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID mtype,
  OPS *ops
)
{
  TOP opcode = TOP_UNDEFINED;

  FmtAssert (TN_is_register(src1) || TN_is_register(src2),
	     ("Expand_Binary_And: both operands const ?"));

  if (TN_is_constant(src1)) {
    // switch order of src so immediate is second
    Expand_Binary_And (dest, src2, src1, mtype, ops);
    return;
  }

  // Try special and with imm cases
  if (TN_Has_Value(src2))
    if (Expand_Special_And_Immed(dest, src1, TN_Value(src2), ops))
      return;

  // Process immediate operand
  if (TN_is_constant(src2)) {
    src2 = Expand_Or_Inline_Immediate(INL_AND,src2, mtype, ops);
  }
  
  opcode = TOP_and_r_r_npc;
  
  FmtAssert(opcode != TOP_UNDEFINED, ("Expand_Binary_And"));

  if (TN_has_value(src2))
    opcode = TOP_opnd_immediate_variant(opcode, 1, TN_value(src2));
  
  Expand_Binary_And_Or (opcode, dest, src1, src2, mtype, ops);

  return;
}

/* ====================================================================
 *   Expand_Binary_Or
 * ====================================================================
 */
void
Expand_Binary_Or (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID mtype,
  OPS *ops
)
{
  TOP opcode = TOP_UNDEFINED;

  FmtAssert (TN_is_register(src1) || TN_is_register(src2),
	     ("Expand_Binary_Or: both operands const ?"));

  if (TN_is_constant(src1)) {
    // switch order of src so immediate is second
    Expand_Binary_Or (dest, src2, src1, mtype, ops);
    return;
  }


  // Try special or with imm cases
  if (TN_Has_Value(src2)) {
    INT64 val = TN_Value(src2);
    if (val == 0) {
      Expand_Copy(dest, True_TN, src1, ops);
      return;
    } else if (((INT32)val & -1) == -1) {
      Expand_Immediate(dest, Gen_Literal_TN(-1, 4), MTYPE_I4, ops);
      return;
    }
  }


  // Process immediate operand
  if (TN_is_constant(src2)) {
    src2 = Expand_Or_Inline_Immediate(INL_OR,src2, mtype, ops);
  }

  opcode = TOP_orr_r_r_npc;
  
  if (TN_has_value(src2))
    opcode = TOP_opnd_immediate_variant(opcode, 1, TN_value(src2));
  
  Expand_Binary_And_Or (opcode, dest, src1, src2, mtype, ops);
  return;
}

/* ====================================================================
 *   Expand_Binary_Xor
 * ====================================================================
 */
void
Expand_Binary_Xor (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID mtype,
  OPS *ops
)
{
  TOP opcode = TOP_UNDEFINED;

  FmtAssert (TN_is_register(src1) || TN_is_register(src2),
	     ("Expand_Binary_Or: both operands const ?"));

  if (MTYPE_byte_size(mtype) == 8) {
    FmtAssert(0, ("Unexpected 8 bytes type"));
  }

  if (TN_is_constant(src1)) {
    // switch order of src so immediate is second
    Expand_Binary_Xor (dest, src2, src1, mtype, ops);
    return;
  }
  // Try special xor with imm cases
  if (TN_Has_Value(src2)) {
    INT64 val = TN_Value(src2);
    if (val == 0) {
      Expand_Copy(dest, True_TN, src1, ops);
      return;
    } 
  }

  // Process immediate operand
  if (TN_is_constant(src2)) {
    src2 = Expand_Or_Inline_Immediate(INL_XOR,src2, mtype, ops);
  }

  opcode = TOP_eor_r_r_npc;

  if (TN_has_value(src2))
    opcode = TOP_opnd_immediate_variant(opcode, 1, TN_value(src2));

  Expand_Binary_And_Or (opcode, dest, src1, src2, mtype, ops);
  return;
}

/* ====================================================================
 *   Expand_Binary_Nor
 * ====================================================================
 */
void
Expand_Binary_Nor (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID mtype,
  OPS *ops
)
{
  FmtAssert(FALSE,("Not Implemented"));
}

/* ====================================================================
 *   Expand_Bool_Comparison
 * ====================================================================
 */
static void
Expand_Bool_Comparison (
  BOOL equals,
  TN *dest,
  TN *src1,
  TN *src2,
  OPS *ops
)
{
  FmtAssert(FALSE,("Expand_Bool_Comparison: not implemented"));
}

/* ====================================================================
 *   Expand_Int_Compares
 * ====================================================================
 */
static void
Expand_Int_Compares (
  TN *dest,			    
  TN *src1,
  TN *src2,
  VARIANT variant,
  OPS *ops
  ) 
{
  TOP action;
  if (TN_register_class(dest) == ISA_REGISTER_CLASS_integer) {
    BOOL is_integer = FALSE;
    TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr,1,0);
    TN *cond = Gen_Enum_TN(ISA_ECV_cond_al);
    action = Pick_Compare_TOP (&variant, &src1, &src2, &cond, ops);
    FmtAssert(action != TOP_UNDEFINED, ("Expand_Int_Compares: unhandled variant"));
    Build_OP (action, cpsr, src1, src2, ops);
    Build_OP (TOP_mov_i8_npc, dest, Get_Zero_TN(4), ops);
    Build_OP (TOP_mov_i8_npc_cond, dest, cpsr, cond, Gen_Literal_TN(1, 4), ops);
  }
  else {
    FmtAssert(FALSE,("Not yet implemented"));
  }

  return;
}
/* ====================================================================
 *   Expand_Int_Less
 * ====================================================================
 */
void
Expand_Int_Less (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID desc,
  OPS *ops
)
{
  VARIANT variant;
  TOP action;
  
  switch (desc) {
  case MTYPE_I4: variant = V_BR_I4LT; break;
  case MTYPE_U4: variant = V_BR_U4LT; break;
  default:
    Is_True(FALSE, ("Expand_Int_Less: MTYPE_%s is not handled",
		                               Mtype_Name(desc)));
  }
  Expand_Int_Compares(dest, src1, src2, variant, ops);

  return;
}

/* ====================================================================
 *   Expand_Int_Less_Equal
 * ====================================================================
 */
void
Expand_Int_Less_Equal (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID desc,
  OPS *ops
)
{
  VARIANT variant;
  TOP action;
  switch (desc) {
  case MTYPE_I4: variant = V_BR_I4LE; break;
  case MTYPE_U4: variant = V_BR_U4LE; break;
  default:
    Is_True(FALSE, ("Expand_Int_Less_Equal: MTYPE_%s is not handled",
                                                  Mtype_Name(desc)));
  }

  Expand_Int_Compares(dest, src1, src2, variant, ops);
  return;
}

/* ====================================================================
 *   Expand_Int_Equal
 * ====================================================================
 */
void
Expand_Int_Equal (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID desc,
  OPS *ops
)
{
  VARIANT variant;
  TOP action;
  switch (desc) {
  case MTYPE_I4: variant = V_BR_I4EQ; break;
  case MTYPE_U4: variant = V_BR_U4EQ; break;

  default:
    Is_True(FALSE, ("Expand_Int_Equal: MTYPE_%s is not handled",
                                                  Mtype_Name(desc)));
  }

  Expand_Int_Compares(dest, src1, src2, variant, ops);
  return;
}

/* ====================================================================
 *   Expand_Int_Not_Equal
 * ====================================================================
 */
void
Expand_Int_Not_Equal (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID desc,
  OPS *ops
)
{
  VARIANT variant;
  TOP   action;
  TN   *tmp1, *tmp2;

  switch (desc) {
  case MTYPE_I4: variant = V_BR_I4NE; break;
  case MTYPE_U4: variant = V_BR_U4NE; break;
  default:
    #pragma mips_frequency_hint NEVER
    Is_True(FALSE, ("Expand_Int_Not_Equal: MTYPE_%s is not handled",
                                                   Mtype_Name(desc)));
  }

  Expand_Int_Compares(dest, src1, src2, variant, ops);
  return;
}

/* ====================================================================
 *   Expand_Int_Greater_Equal
 * ====================================================================
 */
void
Expand_Int_Greater_Equal (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID desc,
  OPS *ops
)
{
  VARIANT variant;
  TOP action;
  switch (desc) {
  case MTYPE_I4: variant = V_BR_I4GE; break;
  case MTYPE_U4: variant = V_BR_U4GE; break;
  default:
    Is_True(FALSE, ("Expand_Int_Greater_Equal: MTYPE_%s is not handled",
                                                     Mtype_Name(desc)));
  }

  Expand_Int_Compares(dest, src1, src2, variant, ops);
  return;
}

/* ====================================================================
 *   Expand_Int_Greater
 * ====================================================================
 */
void
Expand_Int_Greater (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID desc,
  OPS *ops
)
{
  VARIANT variant;
  TOP action;

  switch (desc) {
    case MTYPE_I4: variant = V_BR_I4GT; break;
    case MTYPE_U4: variant = V_BR_U4GT; break;
    default:
      Is_True(FALSE, ("Expand_Int_Greater: MTYPE_%s is not handled",
                                                  Mtype_Name(desc)));
  }

  Expand_Int_Compares(dest, src1, src2, variant, ops);
  return;
}

/* ====================================================================
 *   Expand_Ptr_Not_Equal
 *
 *   src1 and src2 are of MTYPE desc, one of pointer MTYPEs;
 * ====================================================================
 */
void
Expand_Ptr_Not_Equal (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID desc,
  OPS *ops
)
{

  FmtAssert(FALSE,("not supported"));
  return;
}

/* ====================================================================
 *   Expand_Bool_Equal
 * ====================================================================
 */
void
Expand_Bool_Equal (
  TN *dest,
  TN *src1,
  TN *src2,
  OPS *ops
)
{
  FmtAssert(FALSE,("Not Implemented"));

}

/* ====================================================================
 *   Expand_Bool_Not_Equal
 * ====================================================================
 */
void
Expand_Bool_Not_Equal (
  TN *dest,
  TN *src1,
  TN *src2,
  OPS *ops
)
{
  FmtAssert(FALSE,("Not Implemented"));

}

/* ====================================================================
 *   Expand_Bool_To_Int
 * ====================================================================
 */
void
Expand_Bool_To_Int (
  TN *dest,
  TN *src,
  TYPE_ID rtype,
  OPS *ops
)
{
  TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr,1,0);

  Build_OP (TOP_cmp_i8_r, cpsr, src, Get_Zero_TN(4), ops);
  Build_OP (TOP_mov_i8_npc, dest, Gen_Literal_TN(1, 4), ops);
  Build_OP (TOP_mov_i8_npc_cond, dest, cpsr, Gen_Enum_TN(ISA_ECV_cond_eq), Get_Zero_TN(4), ops);
  return;
}


/* ====================================================================
 *   Expand_Float_Add
 * ====================================================================
 */
static void
Expand_Float_Add(
  TN *result,
  TN *src1, 
  TN *src2, 
  TYPE_ID fmtype,
  OPS *ops)
{
  TOP top = TOP_UNDEFINED;
  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  TN *fsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_fsr,1,0);

  FmtAssert(Enable_Fpx, ("Expand_Float_Add: fpx extension not selected"));
  if (Enable_Non_IEEE_Ops && fmtype == MTYPE_F4)
      top = TOP_fpx_fadd;

  FmtAssert(top != TOP_UNDEFINED, ("Expand_Float_Add: unexpected MTYPE %s", MTYPE_name(fmtype)));

  src1 = check_opd_rclass(src1, ISA_REGISTER_CLASS_fpr, ops);
  src2 = check_opd_rclass(src2, ISA_REGISTER_CLASS_fpr, ops);

  check_res_rclass(result, ISA_REGISTER_CLASS_fpr, ops, Build_OP (top, fsr, result, True_TN, src1, src2, ops));
#endif
}


/* ====================================================================
 *   Expand_Float_Sub
 * ====================================================================
 */
static void
Expand_Float_Sub(
  TN *result,
  TN *src1, 
  TN *src2, 
  TYPE_ID fmtype,
  OPS *ops)
{
  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  TOP top = TOP_UNDEFINED;
  TN *fsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_fsr,1,0);

  FmtAssert(Enable_Fpx, ("Expand_Float_Sub: fpx extension not selected"));
  if (Enable_Non_IEEE_Ops && fmtype == MTYPE_F4)
      top = TOP_fpx_fsub;

  FmtAssert(top != TOP_UNDEFINED, ("Expand_Float_Sub: unexpected MTYPE %s", MTYPE_name(fmtype)));

  src1 = check_opd_rclass(src1, ISA_REGISTER_CLASS_fpr, ops);
  src2 = check_opd_rclass(src2, ISA_REGISTER_CLASS_fpr, ops);

  check_res_rclass(result, ISA_REGISTER_CLASS_fpr, ops, Build_OP (top, fsr, result, True_TN, src1, src2, ops));
#endif
}

/* ====================================================================
 *   Expand_Float_Multiply
 * ====================================================================
 */
static void
Expand_Float_Multiply(
  TN *result,
  TN *src1, 
  TN *src2, 
  TYPE_ID fmtype,
  OPS *ops)
{
  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  TOP top = TOP_UNDEFINED;
  TN *fsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_fsr,1,0);

  FmtAssert(Enable_Fpx, ("Expand_Float_Multiply: fpx extension not selected"));
  if (Enable_Non_IEEE_Ops && fmtype == MTYPE_F4)
      top = TOP_fpx_fmul;

  FmtAssert(top != TOP_UNDEFINED, ("Expand_Float_Multiply: unexpected MTYPE %s", MTYPE_name(fmtype)));

  src1 = check_opd_rclass(src1, ISA_REGISTER_CLASS_fpr, ops);
  src2 = check_opd_rclass(src2, ISA_REGISTER_CLASS_fpr, ops);

  check_res_rclass(result, ISA_REGISTER_CLASS_fpr, ops, Build_OP (top, fsr, result, True_TN, src1, src2, ops));
#endif
}


/* ====================================================================
 *   Expand_Float_Div
 * ====================================================================
 */
static void
Expand_Float_Div(
  TN *result,
  TN *src1, 
  TN *src2, 
  TYPE_ID fmtype,
  OPS *ops)
{
  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  TOP top = TOP_UNDEFINED;
  TN *fsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_fsr,1,0);

  FmtAssert(Enable_Fpx, ("Expand_Float_Div: fpx extension not selected"));
  if (Enable_Single_Float_Ops && fmtype == MTYPE_F4) 
      top = TOP_fpx_fdiv;

  FmtAssert(top != TOP_UNDEFINED, ("Expand_Float_Div: unexpected MTYPE %s", MTYPE_name(fmtype))); 

  src1 = check_opd_rclass(src1, ISA_REGISTER_CLASS_fpr, ops);
  src2 = check_opd_rclass(src2, ISA_REGISTER_CLASS_fpr, ops);

  check_res_rclass(result, ISA_REGISTER_CLASS_fpr, ops, Build_OP (top, fsr, result, True_TN, src1, src2, ops));
#endif
}

/* ====================================================================
 *   Expand_Float_Macc_Op
 * ====================================================================
 */
static void
Expand_Float_Macc_Op(
		     BOOL add_not_sub,
		     BOOL negate_result,
		     TN *result,
		     TN *src1, 
		     TN *src2, 
		     TN *src3, 
		     TYPE_ID fmtype,
		     OPS *ops)
{
  FmtAssert(FALSE,("Not yet implemented"));
#if 0

  src1 = check_opd_rclass(src1, ISA_REGISTER_CLASS_fpr, ops);
  src2 = check_opd_rclass(src2, ISA_REGISTER_CLASS_fpr, ops);
  src3 = check_opd_rclass(src3, ISA_REGISTER_CLASS_fpr, ops);

  /* [-] ((src2 * src3) (+|-) src1) */
  TN *fsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_fsr,1,0);
  TN *src1_bis = Build_TN_Like(src1);

  /* We must ensure that op/res use the same tn as the result 1
     is Op_Same_Res(1).
  */
  if (negate_result) {
    if(add_not_sub) {
      /* - [src1 + (src2 * src3)] */
      Expand_Copy(src1_bis,True_TN,src1,ops);
      Build_OP(TOP_fpx_fmac,fsr,src1_bis,True_TN,src1_bis,src2,src3,ops);
      Expand_Neg (result, src1_bis, fmtype, ops);
    }
    else {
      /* [src1 - (src2 * src3)] */
      Expand_Copy(src1_bis,True_TN,src1,ops);
      Build_OP(TOP_fpx_fmacn,fsr,src1_bis,True_TN,src1_bis,src2,src3,ops);
      Expand_Copy(result, True_TN, src1_bis, ops);
    }
  }
  else {
    if(add_not_sub) {
      /* [src1 + (src2 * src3)] */
      Expand_Copy(src1_bis,True_TN,src1,ops);
      Build_OP(TOP_fpx_fmac,fsr,src1_bis,True_TN,src1_bis,src2,src3,ops);
      Expand_Copy(result, True_TN, src1_bis, ops);
    }
    else {
      /* [(src2 * src3) - src1] */
      Expand_Copy(src1_bis,True_TN,src1,ops);
      Build_OP(TOP_fpx_fmacn,fsr,src1_bis,True_TN,src1_bis,src2,src3,ops);
      Expand_Neg (result, src1_bis, fmtype, ops);
    }
  }
#endif
}

/* ====================================================================
 *   Expand_Flop
 * ====================================================================
 */
void 
Expand_Flop (
  OPCODE opcode, 
  TN *result, 
  TN *src1, 
  TN *src2, 
  TN *src3, 
  OPS *ops)
{
  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  OPERATOR opr = OPCODE_operator(opcode);
  TYPE_ID fmtype = OPCODE_rtype(opcode);
  switch (opr) {
  case OPR_ADD:
    Expand_Float_Add(result, src1, src2, fmtype, ops);
    break;
  case OPR_SUB:
    Expand_Float_Sub(result, src1, src2, fmtype, ops);
    break;
  case OPR_MPY:
    Expand_Float_Multiply(result, src1, src2, fmtype, ops);
    break;
  case OPR_DIV:
    Expand_Float_Div(result, src1, src2, fmtype, ops);
    break;
  case OPR_MADD:
    /* src2 * src3 + src1 */
    Expand_Float_Macc_Op(TRUE, FALSE, result, src1, src2, src3, fmtype, ops);
    break;
  case OPR_NMADD:
    /* -((src2 * src3) + src1)  */
    Expand_Float_Macc_Op(TRUE, TRUE, result, src1, src2, src3, fmtype, ops);
    break;
  case OPR_MSUB:
    /*  src2 * src3 - src1 */
    Expand_Float_Macc_Op(FALSE, FALSE, result, src1, src2, src3, fmtype, ops);
    break;
  case OPR_NMSUB:
    /* - ((src2 * src3) - src1 */
    Expand_Float_Macc_Op(FALSE, TRUE, result, src1, src2, src3, fmtype, ops);
    break;
  case OPR_RECIP:
  case OPR_RSQRT:
    FmtAssert(FALSE,("Not Implemented: %s", OPCODE_name(opcode)));
    break;
  default:
    FmtAssert(FALSE,("Not Implemented: %s", OPCODE_name(opcode)));
  }
  return;
#endif
}

/* ====================================================================
 *   Expand_Float_To_Float
 * ====================================================================
 */
void 
Expand_Float_To_Float (
  TN *dest, 
  TN *src, 
  TYPE_ID mtype, 
  OPS *ops)
{    
  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  FmtAssert(Enable_Fpx, ("Expand_Float_To_Float: fpx extension not selected"));
  if (Enable_Single_Float_Ops) {
    if (mtype == MTYPE_F4) {
      TOP top = TOP_fpx_fmvf2f ;
      Build_OP (top, dest, True_TN, src, ops);
      Set_OP_copy (OPS_last(ops));
      return ;
    }
    else
      FmtAssert(FALSE, ("NYI: Expand_Float_To_Float mtype"));      
  }
#endif
}


/* ====================================================================
 *   Expand_Int_To_Float
 * ====================================================================
 */
void 
Expand_Int_To_Float (
  TN *dest, 
  TN *src, 
  TYPE_ID imtype, 
  TYPE_ID fmtype, 
  OPS *ops)
{
  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  TOP top = TOP_UNDEFINED ;
  TN *fsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_fsr,1,0);
  TN *local_dest = dest;

  FmtAssert(Enable_Fpx, ("Expand_Int_To_Float: fpx extension not selected"));
  if (Enable_Non_IEEE_Ops && fmtype == MTYPE_F4 && imtype == MTYPE_I4) {
//    FmtAssert(TN_register_class(dest) == ISA_REGISTER_CLASS_fpr,("Not fpr register"));
      if(TN_register_class(local_dest) != ISA_REGISTER_CLASS_fpr)
          {
              local_dest = Build_RCLASS_TN(ISA_REGISTER_CLASS_fpr);
          }
    if(TN_register_class(src) == ISA_REGISTER_CLASS_integer) {
      top = TOP_fpx_fi2f_r;
    }
    else if(TN_register_class(src) == ISA_REGISTER_CLASS_fpr) {
      top = TOP_fpx_fi2f_f;
    }
    else {
      FmtAssert(FALSE, ("Expand_Int_To_Float : unexpected source register class"));
    }
  }

  FmtAssert(top != TOP_UNDEFINED, ("Expand_Int_To_Float: unexpected IMTYPE or FMTYPE %s", MTYPE_name(imtype), MTYPE_name(fmtype)));

  Build_OP (top, fsr, local_dest, True_TN, src, ops);
  if(local_dest != dest)
      {
          Expand_Copy(dest, True_TN, local_dest, ops);
      }
#endif
}

// [HK]
/* ====================================================================
 *   Expand_Unsigned_To_Float
 * ====================================================================
 */
void 
Expand_Unsigned_To_Float (
  TN *dest, 
  TN *src, 
  TYPE_ID imtype, 
  TYPE_ID fmtype, 
  OPS *ops)
{
  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  TOP top = TOP_UNDEFINED ;
  TN *fsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_fsr,1,0);
  TN *local_dest = dest;

  FmtAssert(Enable_Fpx, ("Expand_Unsigned_To_Float: fpx extension not selected"));
  if (Enable_Non_IEEE_Ops && fmtype == MTYPE_F4 && imtype == MTYPE_U4) {

//    FmtAssert(TN_register_class(dest) == ISA_REGISTER_CLASS_fpr,("Not fpr register"));
      if(TN_register_class(local_dest) != ISA_REGISTER_CLASS_fpr)
          {
              local_dest = Build_RCLASS_TN(ISA_REGISTER_CLASS_fpr);
          }
    if(TN_register_class(src) == ISA_REGISTER_CLASS_integer) {
      top = TOP_fpx_fu2f_r;
    }
    else if(TN_register_class(src) == ISA_REGISTER_CLASS_fpr) {
      top = TOP_fpx_fu2f_f;
    }
    else {
      FmtAssert(FALSE, ("Expand_Float_To_Int : unexpected source register class"));
    }

    Build_OP(top, local_dest, True_TN, src, ops);
    if(local_dest != dest)
        {
            Expand_Copy(dest, True_TN, local_dest, ops);
        }
  }
  FmtAssert(top != TOP_UNDEFINED, ("Expand_Unsigned_To_Float: unexpected IMTYPE %s or FMTYPE %s", MTYPE_name(imtype), MTYPE_name(fmtype)));
#endif
}

/* ====================================================================
 *   Expand_Float_To_Int
 *
 *   TODO how do you trap on float val too big for [u]int32?
 * ====================================================================
 */
static void
Expand_Float_To_Int (
  ROUND_MODE rm,
  TN *dest,
  TN *src,
  TYPE_ID imtype,
  TYPE_ID fmtype,
  OPS *ops
)
{
  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  TOP top = TOP_UNDEFINED;
  TN *fsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_fsr,1,0);
  TN *local_src = src;

  FmtAssert(Enable_Fpx, ("Expand_Float_To_Int: fpx extension not selected"));

  if (Enable_Non_IEEE_Ops && fmtype == MTYPE_F4 && imtype == MTYPE_I4) {
    switch (rm) {
    case ROUND_CHOP:
//      FmtAssert(TN_register_class(src) == ISA_REGISTER_CLASS_fpr,("Not fpr register"));
        if(TN_register_class(src) != ISA_REGISTER_CLASS_fpr)
            {
                local_src = Build_RCLASS_TN(ISA_REGISTER_CLASS_fpr);
                Expand_Copy(local_src, True_TN, src, ops);
            }
      if(TN_register_class(dest) == ISA_REGISTER_CLASS_integer) {
	top = TOP_fpx_ff2i_r;
      }
      else if(TN_register_class(dest) == ISA_REGISTER_CLASS_fpr) {
	top = TOP_fpx_ff2i_f;
      }
      else {
	FmtAssert(FALSE, ("Expand_Float_To_Int : unexpected source register class"));
      }
      break;
      /* All others are not supported. */
    case ROUND_USER:
    case ROUND_NEAREST:
    case ROUND_NEG_INF:
    case ROUND_PLUS_INF:
    default:
      FmtAssert(FALSE, ("Expand_Float_To_Int : unexpected rounding mode"));
	break;
    }
  }
  
  FmtAssert(top != TOP_UNDEFINED, ("Expand_Float_To_Int: unexpected IMTYPE %s of FMTYPE %s", MTYPE_name(imtype), MTYPE_name(fmtype)));
  Build_OP (top, fsr, dest, True_TN, local_src, ops);
  return;
#endif
}

/* ====================================================================
 *   Expand_Float_To_Unsigned
 *
 *   TODO how do you trap on float val too big for [u]int32?
 * ====================================================================
 */
static void
Expand_Float_To_Unsigned (
  ROUND_MODE rm,
  TN *dest,
  TN *src,
  TYPE_ID imtype,
  TYPE_ID fmtype,
  OPS *ops
)
{
  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  TOP top = TOP_UNDEFINED;
  TN *fsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_fsr,1,0);
  TN *local_src = src;

  FmtAssert(Enable_Fpx, ("Expand_Float_To_Unsigned: fpx extension not selected"));
  if (Enable_Non_IEEE_Ops && fmtype == MTYPE_F4 && imtype == MTYPE_U4) {
    switch (rm) {
    case ROUND_CHOP:
//    FmtAssert(TN_register_class(src) == ISA_REGISTER_CLASS_fpr,("Not fpr register"));
        if(TN_register_class(src) != ISA_REGISTER_CLASS_fpr)
            {
                local_src = Build_RCLASS_TN(ISA_REGISTER_CLASS_fpr);
                Expand_Copy(local_src, True_TN, src, ops);
            }
      if(TN_register_class(dest) == ISA_REGISTER_CLASS_integer) {
	top = TOP_fpx_ff2u_r;
      }
      else if(TN_register_class(dest) == ISA_REGISTER_CLASS_fpr) {
	top = TOP_fpx_ff2u_f;
      }
      else {
	FmtAssert(FALSE, ("Expand_Float_To_Unsigned : unexpected source register class"));
      }
      break;
      /* All others are not supported. */
    case ROUND_USER:
    case ROUND_NEAREST:
    case ROUND_NEG_INF:
    case ROUND_PLUS_INF:
    default:
      FmtAssert(FALSE, ("Expand_Float_To_Unsigned : unexpected rounding mode"));
	break;
    }
  }
  
  FmtAssert(top != TOP_UNDEFINED, ("Expand_Float_To_Unsigned: unexpected IMTYPE %s of FMTYPE %s", MTYPE_name(imtype), MTYPE_name(fmtype)));

  Build_OP (top, fsr, dest, True_TN, local_src, ops);
  return;
#endif
}

/* ====================================================================
 *   Expand_Float_To_Int_Cvt
 * ====================================================================
 */
void
Expand_Float_To_Int_Cvt (
  TN *dest,
  TN *src,
  TYPE_ID imtype,
  TYPE_ID fmtype,
  OPS *ops
)
{
  Expand_Float_To_Int (ROUND_USER, dest, src, imtype, fmtype, ops);
}

/* ====================================================================
 *   Expand_Float_To_Unsigned_Cvt
 * ====================================================================
 */
void
Expand_Float_To_Unsigned_Cvt (
  TN *dest,
  TN *src,
  TYPE_ID imtype,
  TYPE_ID fmtype,
  OPS *ops
)
{
  Expand_Float_To_Unsigned (ROUND_USER, dest, src, imtype, fmtype, ops);
}


/* ====================================================================
 *   Expand_Float_To_Int_Round
 * ====================================================================
 */
void
Expand_Float_To_Int_Round (
  TN *dest,
  TN *src,
  TYPE_ID imtype,
  TYPE_ID fmtype,
  OPS *ops
)
{
  Expand_Float_To_Int (ROUND_NEAREST, dest, src, imtype, fmtype, ops);
}

/* ====================================================================
 *   Expand_Float_To_Unsigned_Round
 * ====================================================================
 */
void
Expand_Float_To_Unsigned_Round (
  TN *dest,
  TN *src,
  TYPE_ID imtype,
  TYPE_ID fmtype,
  OPS *ops
)
{
  Expand_Float_To_Unsigned (ROUND_NEAREST, dest, src, imtype, fmtype, ops);
}

/* ====================================================================
 *   Expand_Float_To_Int_Trunc
 * ====================================================================
 */
void
Expand_Float_To_Int_Trunc (
  TN *dest,
  TN *src,
  TYPE_ID imtype,
  TYPE_ID fmtype,
  OPS *ops
)
{
  Expand_Float_To_Int (ROUND_CHOP, dest, src, imtype, fmtype, ops);
}

/* ====================================================================
 *   Expand_Float_To_Unsigned_Trunc
 * ====================================================================
 */
void
Expand_Float_To_Unsigned_Trunc (
  TN *dest,
  TN *src,
  TYPE_ID imtype,
  TYPE_ID fmtype,
  OPS *ops
)
{
  Expand_Float_To_Unsigned (ROUND_CHOP, dest, src, imtype, fmtype, ops);
}

/* ====================================================================
 *   Expand_Float_To_Int_Floor
 * ====================================================================
 */
void
Expand_Float_To_Int_Floor (
  TN *dest,
  TN *src,
  TYPE_ID imtype,
  TYPE_ID fmtype,
  OPS *ops
)
{
  Expand_Float_To_Int (ROUND_NEG_INF, dest, src, imtype, fmtype, ops);
}

/* ====================================================================
 *   Expand_Float_To_Unsigned_Floor
 * ====================================================================
 */
void
Expand_Float_To_Unsigned_Floor (
  TN *dest,
  TN *src,
  TYPE_ID imtype,
  TYPE_ID fmtype,
  OPS *ops
)
{
  Expand_Float_To_Unsigned (ROUND_NEG_INF, dest, src, imtype, fmtype, ops);
}

/* ====================================================================
 *   Expand_Float_To_Int_Ceil
 * ====================================================================
 */
void
Expand_Float_To_Int_Ceil (
  TN *dest,
  TN *src,
  TYPE_ID imtype,
  TYPE_ID fmtype,
  OPS *ops
)
{
  Expand_Float_To_Int (ROUND_PLUS_INF, dest, src, imtype, fmtype, ops);
}

/* ====================================================================
 *   Expand_Float_To_Unsigned_Ceil
 * ====================================================================
 */
void
Expand_Float_To_Unsigned_Ceil (
  TN *dest,
  TN *src,
  TYPE_ID imtype,
  TYPE_ID fmtype,
  OPS *ops
)
{
  Expand_Float_To_Unsigned (ROUND_PLUS_INF, dest, src, imtype, fmtype, ops);
}

/* ====================================================================
 *   Optimize_Select
 * ====================================================================
 */
static BOOL
Optimize_Select (
  VARIANT variant,
  TN *cond1,
  TN *cond2,
  TN *dest,
  TN *dest2,
  TN *src1,
  TN *src2,
  BOOL is_float,
  OPS *ops
)
{
  return FALSE;
}

/* ====================================================================
 *   Expand_Compare_And_Select
 * ====================================================================
 */
static void
Expand_Compare_And_Select (
  VARIANT variant,
  TN *cond1,
  TN *cond2,
  TN *dest,
  TN *opposite_dest,
  TN *true_tn,
  TN *false_tn,
  BOOL is_float,
  OPS *ops
)
{
  TYPE_ID mtype;

  // Look for special cases to optimize
  if (Optimize_Select(variant, cond1, cond2, dest, opposite_dest,
		      true_tn, false_tn, is_float, ops)) return;

  // Expand
  //
  //      dst, opposite_dst = (cond1 cmp cond2) ? true_tn : false_tn
  //
  // On this target we only support integer:
  // dst = (cond1 cmp cond2) ? true_tn:false_tn
  FmtAssert(opposite_dest == NULL, ("Not Implemented"));

  /* (cbr) can be
  FmtAssert(!is_float, ("Not Implemented"));
  */

  TN *cond = Gen_Enum_TN(ISA_ECV_cond_al);
  TOP cmp1 = Pick_Compare_TOP (&variant, &cond1, &cond2, &cond, ops);
  // If first try failed, try to invert it.
  if (cmp1 == TOP_UNDEFINED && (variant == V_BR_FNE || variant == V_BR_DNE)) {
    variant = (variant == V_BR_FNE) ? V_BR_FEQ : V_BR_DEQ;
    TN *tmp = true_tn; true_tn = false_tn; false_tn = tmp;
    cmp1 = Pick_Compare_TOP (&variant, &cond1, &cond2, &cond, ops);
  }
  FmtAssert (cmp1 != TOP_UNDEFINED,
	     ("Expand_Compare_And_Select: unexpected comparison"));

  TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr,1,0);
  Build_OP (cmp1, cpsr, cond1, cond2, ops);
  Build_OP (TOP_mov_r_npc, dest, false_tn, ops);
  Build_OP (TOP_mov_r_npc_cond, dest, cpsr, cond, true_tn, ops);
  return;
}

/* ====================================================================
 *   Expand_Select
 * ====================================================================
 */
void
Expand_Select (
  TN *dest_tn,
  TN *cond_tn,
  TN *true_tn,
  TN *false_tn,
  TYPE_ID mtype,
  BOOL float_cond,
  OPS *ops
)
{
  TOP top = TOP_UNDEFINED;
  //const BOOL is_float = MTYPE_is_float(mtype);

  FmtAssert (MTYPE_byte_size(mtype) == 4,
	     ("Unexpected mtype in Expand_Select"));
  FmtAssert (TN_register_class(cond_tn) == ISA_REGISTER_CLASS_integer,("Not integer register class %s",
       ISA_REGISTER_CLASS_INFO_Name(ISA_REGISTER_CLASS_Info(TN_register_class(cond_tn)))));

  if (true_tn == false_tn) {
    Exp_COPY (dest_tn, true_tn, ops);
    return;
  }

  // For generating the select we do an unconditional copy of the false part,
  // followed by a conditional copy of the true part.
  // We should use select instruction on armv6 (SIMD?).
  TN *tmp_tn = dest_tn;
  if (dest_tn == true_tn) {
    tmp_tn = Build_TN_Like(dest_tn);
  }

  TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr,1,0);

  Build_OP (TOP_cmp_i8_r, cpsr, cond_tn, Get_Zero_TN(4), ops);
  Build_OP (TOP_mov_r_npc, tmp_tn, false_tn, ops);
  Build_OP (TOP_mov_r_npc_cond, tmp_tn, cpsr, Gen_Enum_TN(ISA_ECV_cond_ne), true_tn, ops);
  if (dest_tn != tmp_tn) {
    Exp_COPY(dest_tn, tmp_tn, ops);
  }
}

/* ====================================================================
 *   Check_Select_Expansion
 *
 *   check whether to eval condition before select
 * ====================================================================
 */
BOOL
Check_Select_Expansion (
  OPCODE compare
)
{
  // in order to get optimal code,
  // don't evaluate the condition first,
  // but pass the condition and kids to exp_select,
  // which will do the compare and use the predicate results.
  return FALSE;
}

/* ====================================================================
 *   Exp_Select_And_Condition
 * ====================================================================
 */
void
Exp_Select_And_Condition (
  OPCODE select,
  TN *result,
  TN *true_tn,
  TN *false_tn,
  OPCODE compare,
  TN *cmp_kid1,
  TN *cmp_kid2,
  VARIANT variant,
  OPS *ops
)
{
  OPS newops = OPS_EMPTY;

  switch (variant) {
  case V_BR_PEQ:
  case V_BR_PNE:
    {
      //      Is_True(cmp == TOP_UNDEFINED,
      //      ("unexpected compare op for V_BR_PEQ/V_BR_PNE"));

      FmtAssert(FALSE,("Not Implemented"));
    }
    break;
  case V_BR_NONE:
    #pragma mips_frequency_hint NEVER
    FmtAssert(FALSE, ("Exp_Select_And_Condition given br_none variant"));
    /*NOTREACHED*/
  default:
    //    FmtAssert(cmp != TOP_UNDEFINED,
    //                ("Exp_Select_And_Condition: unexpected comparison"));
    break;
  }

  Expand_Compare_And_Select (variant, cmp_kid1, cmp_kid2,
			     result, NULL, true_tn, false_tn,
			     MTYPE_is_float(OPCODE_rtype(select)), &newops);

  if (Trace_Exp) {
    #pragma mips_frequency_hint NEVER
    OP *op;
    fprintf(TFile, "Exp_Select_And_Condition:\n");
    FOR_ALL_OPS_OPs(&newops, op) {
      fprintf(TFile, " into ");
      Print_OP (op);
    }
  }

  OPS_Append_Ops(ops, &newops);
}

/* ====================================================================
 *   Expand_Min
 * ====================================================================
 */
void
Expand_Min (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID mtype,
  OPS *ops
)
{
  TOP top = TOP_UNDEFINED;

  FmtAssert (TN_is_register(src1) || TN_is_register(src2),
	     ("Expand_Min: both operands const ?"));
  
  if (TN_is_constant(src1)) {
    Expand_Min(dest, src2, src1, mtype, ops);
    return;
  }
   
  // Process immediate operand
  if (TN_is_constant(src2)) {
    src2 = Expand_Or_Inline_Immediate(INL_MIN,src2, mtype, ops);
  }

  TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr,1,0);
  TN *src = NULL;
  TN *cond_tn = NULL;

  if(mtype == MTYPE_I4 || mtype == MTYPE_U4) {
    if(TN_has_value(src2)) {
      Build_OP (TOP_cmp_i8_r, cpsr, src1, src2, ops);
    }
    else {
      Build_OP (TOP_cmp_r_r, cpsr, src1, src2, ops);
    }

    if(dest == src1) {
      src = src2;
      switch (mtype) {
      case MTYPE_I4:
	cond_tn =  Gen_Enum_TN(ISA_ECV_cond_gt);
	break;
      case MTYPE_U4:
	cond_tn =  Gen_Enum_TN(ISA_ECV_cond_hi);
	break;
      default:
	FmtAssert(FALSE, ("Expand_Min: unexpected MTYPE %s", MTYPE_name(mtype)));
      }
    }
    else if(dest == src2) {
      src = src1;
      switch (mtype) {
      case MTYPE_I4:
	cond_tn =  Gen_Enum_TN(ISA_ECV_cond_le);
	break;
      case MTYPE_U4:
	cond_tn =  Gen_Enum_TN(ISA_ECV_cond_ls);
	break;
      default:
	FmtAssert(FALSE, ("Expand_Min: unexpected MTYPE %s", MTYPE_name(mtype)));
      }
    }
    else {
      if(TN_has_value(src2)) {
	Build_OP (TOP_mov_i8_npc, dest, src2, ops);
      }
      else {
	Build_OP (TOP_mov_r_npc, dest, src2, ops);
      }
      src = src1;
      switch (mtype) {
      case MTYPE_I4:
	cond_tn =  Gen_Enum_TN(ISA_ECV_cond_le);
	break;
      case MTYPE_U4:
	cond_tn =  Gen_Enum_TN(ISA_ECV_cond_ls);
	break;
      default:
	FmtAssert(FALSE, ("Expand_Min: unexpected MTYPE %s", MTYPE_name(mtype)));
      }
    }
  }
  else {
    FmtAssert(FALSE, ("Expand_Min: unexpected MTYPE %s", MTYPE_name(mtype)));
  }

  FmtAssert(src != NULL,("src is null"));
  FmtAssert(cond_tn != NULL,("cond_tn is null"));
  Build_OP (TOP_mov_r_npc_cond, dest, cpsr, cond_tn, src, ops);

  return;
}

/* ====================================================================
 *   Expand_Max
 * ====================================================================
 */
void
Expand_Max (
  TN *dest,
  TN *src1,
  TN *src2,
  TYPE_ID mtype,
  OPS *ops
)
{
  TOP top = TOP_UNDEFINED;

  FmtAssert (TN_is_register(src1) || TN_is_register(src2),
	     ("Expand_Max: both operands const ?"));
  
  if (TN_is_constant(src1)) {
    Expand_Max(dest, src2, src1, mtype, ops);
    return;
  }
   
  // Process immediate operand
  if (TN_is_constant(src2)) {
    src2 = Expand_Or_Inline_Immediate(INL_MAX,src2, mtype, ops);
  }

  TN *cpsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_cpsr,1,0);
  TN *src = NULL;
  TN *cond_tn = NULL;

  if(mtype == MTYPE_I4 || mtype == MTYPE_U4) {
    if(TN_has_value(src2)) {
      Build_OP (TOP_cmp_i8_r, cpsr, src1, src2, ops);

      
    }
    else {
      Build_OP (TOP_cmp_r_r, cpsr, src1, src2, ops);
    }

    if(dest == src1) {
      src = src2;
      switch (mtype) {
      case MTYPE_I4:
	cond_tn = Gen_Enum_TN(ISA_ECV_cond_lt);
	break;
      case MTYPE_U4:
	cond_tn = Gen_Enum_TN(ISA_ECV_cond_lo);
	break;
      default:
	FmtAssert(FALSE, ("Expand_Min: unexpected MTYPE %s", MTYPE_name(mtype)));
      }
    }
    else if(dest == src2) {
      src = src1;
      switch (mtype) {
      case MTYPE_I4:
	cond_tn = Gen_Enum_TN(ISA_ECV_cond_ge);
	break;
      case MTYPE_U4:
	cond_tn = Gen_Enum_TN(ISA_ECV_cond_hs);
	break;
      default:
	FmtAssert(FALSE, ("Expand_Min: unexpected MTYPE %s", MTYPE_name(mtype)));
      }
    }
    else {
      src = src1;
      switch (mtype) {
      case MTYPE_I4:
	cond_tn = Gen_Enum_TN(ISA_ECV_cond_ge);
	break;
      case MTYPE_U4:
	cond_tn = Gen_Enum_TN(ISA_ECV_cond_hs);
	break;
      default:
	FmtAssert(FALSE, ("Expand_Min: unexpected MTYPE %s", MTYPE_name(mtype)));
      }
      
      if(TN_has_value(src2)) {
	Build_OP (TOP_mov_i8_npc, dest, src2, ops);
      }
      else {
	Build_OP (TOP_mov_r_npc, dest, src2, ops);
      }
    }
  }
  else {
    FmtAssert(FALSE, ("Expand_Min: unexpected MTYPE %s", MTYPE_name(mtype)));
  }

  FmtAssert(src != NULL,("src is null"));
  FmtAssert(cond_tn != NULL,("cond_tn is null"));
  Build_OP (TOP_mov_r_npc_cond, dest, cpsr, cond_tn, src, ops);

  return;
}

/* ====================================================================
 *   Expand_MinMax
 *
 *   t = s1 < s2; d = select t ? s1 : s2; d2 = select t ? s2 : s1
 * ====================================================================
 */
void
Expand_MinMax (
  TN *dest,
  TN *dest2,
  TN *src1,
  TN *src2,
  TYPE_ID mtype,
  OPS *ops
)
{

  FmtAssert(FALSE,("MINMAX shouldn't have been reached"));

}

/* ====================================================================
 *   Expand_Float_Compares
 * ====================================================================
 */
static void
Expand_Float_Compares(
  TN *dest,
  TN *src1,
  TN *src2,
  VARIANT variant,
  OPS *ops
)
{
  TOP action;
  BOOL is_integer = FALSE;

  FmtAssert(FALSE, ("Not implemented"));
#if 0

  FmtAssert(Enable_Fpx, ("Expand_Float_Compares: fpx extension not selected"));
  FmtAssert(TN_register_class(dest) == ISA_REGISTER_CLASS_integer,("Expand_Float_Compares: dest is not a INTEGER"));

  action = Pick_Compare_TOP (&variant, &src1, &src2, &is_integer, ops);
  FmtAssert(action != TOP_UNDEFINED, ("Expand_Float_Compares: unhandled variant"));
  if (is_integer) {
    TN *fsr = Build_Dedicated_TN(ISA_REGISTER_CLASS_fsr,1,0);
    Build_OP (action, fsr, dest, True_TN, src1, src2, ops);
  }
  else {
    Build_OP (action, dest, True_TN, src1, src2, ops);
  }
#endif
  return;
}

/* ====================================================================
 *   Expand_Float_Less
 * ====================================================================
 */
void
Expand_Float_Less (
  TN *dest,
  TN *src1,
  TN *src2,
  VARIANT variant,
  TYPE_ID mtype,
  OPS *ops
)
{

  FmtAssert(FALSE, ("Not implemented"));
#if 0

  FmtAssert(Enable_Fpx, ("Expand_Float_Less: fpx extension not selected"));
  FmtAssert(variant == V_NONE, ("Unexpected float compare variant"));

  switch (mtype) {
  case MTYPE_F4: variant = Enable_Single_Float_Ops ? V_BR_FLT: V_NONE;
    break;
  case MTYPE_F8: variant = Enable_Double_Float_Ops ? V_BR_DLT: V_NONE;
    break;
  default:
    break;
  }
  FmtAssert(variant != V_NONE, ("unimplemented"));

  Expand_Float_Compares(dest, src1, src2, variant, ops);

#endif
  return;
}

/* ====================================================================
 *   Expand_Float_Greater
 * ====================================================================
 */
void
Expand_Float_Greater (
  TN *dest,
  TN *src1,
  TN *src2,
  VARIANT variant,
  TYPE_ID mtype,
  OPS *ops
)
{
  FmtAssert(FALSE, ("Not implemented"));
#if 0

  FmtAssert(Enable_Fpx, ("Expand_Float_Greater: fpx extension not selected"));
  FmtAssert(variant == V_NONE, ("Unexpected float compare variant"));

  switch (mtype) {
  case MTYPE_F4: variant = Enable_Single_Float_Ops ? V_BR_FGT: V_NONE;
    break;
  case MTYPE_F8: variant = Enable_Double_Float_Ops ? V_BR_DGT: V_NONE;
    break;
  default:
    break;
  }
  FmtAssert(variant != V_NONE, ("unimplemented"));

  Expand_Float_Compares(dest, src1, src2, variant, ops);
#endif
  return;
}

/* ====================================================================
 *   Expand_Floa_Less_Equal
 * ====================================================================
 */
void
Expand_Float_Less_Equal (
  TN *dest,
  TN *src1,
  TN *src2,
  VARIANT variant,
  TYPE_ID mtype,
  OPS *ops
)
{
  FmtAssert(FALSE, ("Not implemented"));
#if 0
  FmtAssert(Enable_Fpx, ("Expand_Float_Equal: fpx extension not selected"));
  FmtAssert(variant == V_NONE, ("Unexpected float compare variant"));

  switch (mtype) {
  case MTYPE_F4: variant = Enable_Single_Float_Ops ? V_BR_FLE: V_NONE;
    break;
  case MTYPE_F8: variant = Enable_Double_Float_Ops ? V_BR_DLE: V_NONE;
    break;
  default:
    break;
  }
  FmtAssert(variant != V_NONE, ("unimplemented"));

  Expand_Float_Compares(dest, src1, src2, variant, ops);
#endif
  return;
}

/* ====================================================================
 *   Expand_Float_Greater_Equal
 * ====================================================================
 */
void
Expand_Float_Greater_Equal (
  TN *dest,
  TN *src1,
  TN *src2,
  VARIANT variant,
  TYPE_ID mtype,
  OPS *ops
)
{
  FmtAssert(FALSE, ("Not implemented"));
#if 0
  FmtAssert(Enable_Fpx, ("Expand_Float_Greater_Equal: fpx extension not selected"));
  FmtAssert(variant == V_NONE, ("Unexpected float compare variant"));

  switch (mtype) {
  case MTYPE_F4: variant = Enable_Single_Float_Ops ? V_BR_FGE: V_NONE;
    break;
  case MTYPE_F8: variant = Enable_Double_Float_Ops ? V_BR_DGE: V_NONE;
    break;
  default:
    break;
  }
  FmtAssert(variant != V_NONE, ("unimplemented"));

  Expand_Float_Compares(dest, src1, src2, variant, ops);
#endif
  return;
}

/* ====================================================================
 *   Expand_Float_Equal
 * ====================================================================
 */
void
Expand_Float_Equal (
  TN *dest,
  TN *src1,
  TN *src2,
  VARIANT variant,
  TYPE_ID mtype,
  OPS *ops
)
{

  FmtAssert(FALSE, ("Not implemented"));
#if 0
  FmtAssert(Enable_Fpx, ("Expand_Float_Equal: fpx extension not selected"));
  FmtAssert(variant == V_NONE, ("Unexpected float compare variant"));

  switch (mtype) {
  case MTYPE_F4: variant = Enable_Single_Float_Ops ? V_BR_FEQ: V_NONE;
    break;
  case MTYPE_F8: variant = Enable_Double_Float_Ops ? V_BR_DEQ: V_NONE;
    break;
  default:
    break;
  }
  FmtAssert(variant != V_NONE, ("unimplemented"));

  Expand_Float_Compares(dest, src1, src2, variant, ops);
#endif
  return;
}

/* ====================================================================
 *   Expand_Float_Not_Equal
 * ====================================================================
 */
void
Expand_Float_Not_Equal (
  TN *dest,
  TN *src1,
  TN *src2,
  VARIANT variant,
  TYPE_ID mtype,
  OPS *ops
)
{
  FmtAssert(FALSE, ("Not implemented"));
#if 0
  FmtAssert(Enable_Fpx, ("Expand_Float_Not_Equal: fpx extension not selected"));
  FmtAssert(variant == V_NONE, ("Unexpected float compare variant"));

  switch (mtype) {
  case MTYPE_F4: variant = Enable_Single_Float_Ops ? V_BR_FNE: V_NONE;
    break;
  case MTYPE_F8: variant = Enable_Double_Float_Ops ? V_BR_DNE: V_NONE;
    break;
  default:
    break;
  }
  FmtAssert(variant != V_NONE, ("unimplemented"));

  Expand_Float_Compares(dest, src1, src2, variant, ops);
#endif
  return;
}

/* ====================================================================
 *   Exp_Sqrt
 * ====================================================================
 */
void
Expand_Sqrt (
  TN *result,
  TN *src,
  TYPE_ID mtype,
  OPS *ops
)
{
  FmtAssert(FALSE, ("Not implemented"));
#if 0
  TOP opcode = TOP_UNDEFINED;
#if 0
  switch (mtype) {
  case MTYPE_F4: 
    if (Enable_Single_Float_Ops) opcode = TOP_sqrtf;
    break;
  case MTYPE_F8: 
    if (Enable_Double_Float_Ops) opcode = TOP_sqrtd;
    break;
  default:
    break;
  }
#endif
  FmtAssert(opcode != TOP_UNDEFINED,("Not Implemented"));

  Build_OP (opcode, result, src, ops);
#endif
  return;
}

/* ====================================================================
 *   Exp_Recip_Sqrt
 * ====================================================================
 */
void
Expand_Recip_Sqrt (
  TN *result,
  TN *src,
  TYPE_ID mtype,
  OPS *ops
)
{
  FmtAssert(FALSE,("Not Implemented"));

  return;
}

/* ======================================================================
 *   Exp_COPY
 *
 *   Generate a register transfer copy from 'src_tn' to 'tgt_tn'.
 * ======================================================================*/
void
Exp_COPY (
  TN *tgt_tn,
  TN *src_tn,
  OPS *ops
)
{
  Expand_Copy (tgt_tn, True_TN, src_tn, ops);
  return;
}

/* ======================================================================
 *   Expand_TOP_intrncall
 *
 *   Given a TOP_intrncall <op>, expand it into the sequence of instructions
 *   that must be generated. If <get_sequence_length> is TRUE, return only
 *   the number of instructions in the sequence and don't actually do the
 *   expansion.
 * ======================================================================
 */
static INT
Expand_TOP_intrncall (
  const OP *op,
  OPS *ops,
  BOOL get_sequence_length,
  INT pc_value
)
{
  FmtAssert(FALSE, ("Expand_TOP_intrncall NYI"));
  /*NOTREACHED*/
}

/* ======================================================================
 *   Exp_Intrinsic_Call
 *
 *   initial expansion of intrinsic call (may not be complete lowering).
 *   return result TN (if set).
 *   If the intrinsic requires a label and loop (2 bb's)
 *   then ops is for first bb and ops2 is for bb after the label.
 *   Otherwise only ops is filled in.
 * ======================================================================
 */
void
Exp_Intrinsic_Call (
  INTRINSIC id,
  INT num_results,
  INT num_opnds,
  TN **result,
  TN **opnd,
  OPS *ops,
  LABEL_IDX *label,
  OPS *loop_ops,
  SRCPOS srcpos
) {
    /*
     * Currently treated as intrinsics calls:
     *  - D-cache intrinsics management
     * See WHIRL documentation for difference between ops and calls.
     */

    switch(id) {    
      /* Add here intrinsic that will be expanded effectively as intrinsic call */
    default:
      /* Default is intrinsic op */
      Exp_Intrinsic_Op(id,num_results,num_opnds,result,opnd,ops,srcpos,NULL);
    }
}

/* ======================================================================
 *   Exp_Simulated_Op
 *
 *   Given a simulated <op>, expand it into the sequence of instructions
 *   supported by the target.
 * ======================================================================
 */
void
Exp_Simulated_Op (
  const OP *op,
  OPS *ops,
  INT pc_value,
  ST **symbol_def_after_group
)
{
  OP *newop;

  if (Trace_Exp) {
    fprintf(TFile, "Expand_Simulated_Op:\n");
    Print_OP(op);
    fprintf(TFile, "\n");
  }

  switch (OP_code(op)) {

  case TOP_intrncall:
    FmtAssert(FALSE,("Should not be reached"));
    Expand_TOP_intrncall(op, ops, FALSE, pc_value);
    break;

  case TOP_spadjust:
    FmtAssert(FALSE,("Should not be reached"));
    // spadjust should only show up for alloca/dealloca
    if (OP_variant(op) == V_SPADJUST_PLUS) {
      Expand_Add (OP_result(op,0), OP_opnd(op,0),
                                    OP_opnd(op,1), Pointer_Mtype, ops);
    }
    else {
      Expand_Sub (OP_result(op,0), OP_opnd(op,0),
                                    OP_opnd(op,1), Pointer_Mtype, ops);
    }

    FOR_ALL_OPS_OPs(ops, newop) {
      OP_srcpos(newop) = OP_srcpos(op);
    }
    break;

  default:
    FmtAssert(FALSE, ("simulated OP %s not handled", TOP_Name(OP_code(op))));
    /*NOTREACHED*/
  }
}

/* ======================================================================
 * Simulated_Op_Real_Ops
 *
 * Return the number of instructions that will be generated by Exp_Simulated_Op
 * ======================================================================*/
INT
Simulated_Op_Real_Ops(const OP *op)
{
  switch (OP_code(op)) {
  case TOP_intrncall:
    return Expand_TOP_intrncall (op, NULL, TRUE, 0);
  case TOP_spadjust:
    return 1;
  default: {
      /* Anything other than the above is presumed to be removed by
       * emit time, therefore we just say the expansion generates 0 ops.
       * (we used to assert, but that isn't a good solution -- see pv 652898).
       */
      return 0;
    }

  }
}

/* ======================================================================
 * Simulated_Op_Real_Inst_Words
 *
 * Return the number of instruction words that will ultimately be emitted
 * for the expansion generated by Exp_Simulated_Op
 * ======================================================================*/
INT
Simulated_Op_Real_Inst_Words(const OP *op)
{
  switch (OP_code(op)) {
  case TOP_spadjust:
  case TOP_asm:
    return 1;
  default: {
    // We should never emit a simulated OP, so just assert.
    FmtAssert(FALSE, ("shouldn't be calling Simulated_Op_Real_Inst_Words for %s", TOP_Name(OP_code(op)) ));
    /*NOTREACHED*/
  }
  }
}

/* ======================================================================
 *   Exp_Is_Large_Stack_Sym
 *
 *   determine if a given symbol is a stack relative reference that will
 *   require multiple instructions to load or store.
 * ======================================================================*/
BOOL
Exp_Is_Large_Stack_Sym (
  ST* sym,
  INT64 ofst
)
{
  ST *base_sym;
  INT64 base_ofst;

  if (sym == NULL) {
    return FALSE;
  }

  Allocate_Object(sym);		/* make sure sym is allocated */

  Base_Symbol_And_Offset_For_Addressing (sym, ofst, &base_sym, &base_ofst);

  if ( base_sym == SP_Sym && (base_ofst < 0 ||  !ISA_LC_Value_In_Class(base_ofst, LC_immed8)) ) {
    return TRUE;
  }
  /* [JV] With FP, generates always value in register. */
  if (base_sym == FP_Sym) {
    return TRUE;
  }
  return FALSE;
}

/* ====================================================================
 *   Exp_Spadjust
 * ====================================================================
 */
void
Exp_Spadjust (
  TN *dest,
  TN *size,
  VARIANT variant,
  OPS *ops
)
{
  Build_OP (TOP_spadjust, dest, SP_TN, size, ops);
  OP_variant(OPS_last(ops)) = variant;
}


/* ====================================================================
 *   Exp_Noop
 * ====================================================================
 */
void
Exp_Noop (OPS *ops)
{
  // On this machine there is only one noop:
  Build_OP (TOP_nop, ops);
}

/* ====================================================================
 *   Exp_GP_Init
 * ====================================================================
 */
void
Exp_GP_Init (
  TN *dest,
  ST *fn_st,
  OPS *ops
)
{

return;
// FmtAssert (FALSE, ("not yet implemented"));

#if 0
  static INT Temp_Index = 0;

  TN *call_dest_tn;
  TN *neg_gprel_tn;
 
  // Generate a symbol for the label to call.  It has
  // to be a symbol (not a label) because we need to
  // apply a reloc to it later.
  ST *st = New_ST (CURRENT_SYMTAB);
  STR_IDX str_idx = Save_Str2i ("L?", ".gpinit_", Temp_Index++);
  ST_Init (st, str_idx, CLASS_NAME, SCLASS_UNKNOWN, EXPORT_LOCAL,
	  ST_pu (fn_st));
  call_dest_tn = Gen_Symbol_TN (st, 0, 0);
  Build_OP (TOP_getpc, RA_TN, call_dest_tn, ops);

  neg_gprel_tn = Gen_Symbol_TN (st, 0, TN_RELOC_NEG_GOT_DISP);
  Expand_Add (dest, RA_TN, neg_gprel_tn, Pointer_Mtype, ops);
#endif
}

/* ====================================================================
 *   Expand_Const
 * ====================================================================
 */
void
Expand_Const (
  TN *dest,
  TN *src,
  TYPE_ID mtype,
  OPS *ops
)
{
  FmtAssert(TN_is_symbol(src), ("Expand_Const: src not a symbol TN"));
  FmtAssert(TN_register_class(dest) == ISA_REGISTER_CLASS_integer,("Not integer register class"));

  TCON tc = ST_tcon_val(TN_var(src));
  //
  // This is called normally for floating-point and company
  // Since we keep them in integer registers, just make a mov
  //

  if ((mtype == MTYPE_I4 || MTYPE_is_float(mtype)) &&
      mtype == TCON_ty(tc) &&
      (mtype == MTYPE_F4 || mtype == MTYPE_F8 || mtype == MTYPE_I4)) {
    if (mtype == MTYPE_F4 || mtype == MTYPE_I4) {
      union {
	INT32 i;
	float f;
      } val;
      
      val.f = TCON_fval(tc);
      Build_OP(TOP_MAKE, dest, Gen_Literal_TN(val.i, 4),ops);
    }
    else {
      goto unsupported;
    }
  } else {
    goto unsupported;
  }
  return;
 unsupported:
  extern void dump_tn (TN*);
  dump_tn (dest);
  dump_tn (src);
  FmtAssert(FALSE,("unsupported type %s", MTYPE_name(TCON_ty(tc))));
}

/*
 * Exp_Enable_Allocate_Object()
 *
 * Set/Unset actual object allocation.
 */
static BOOL can_allocate_object = TRUE;
void
Exp_Enable_Allocate_Object(BOOL onoff)
{
  can_allocate_object = onoff;
}

/*
 * Exp_Can_Allocate_Object()
 *
 * See interface description in exp_private.h.
 */
BOOL
Exp_Can_Allocate_Object(void)
{
  return can_allocate_object;
}
