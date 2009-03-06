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


/* EXP routines for loads and stores */

#include <elf.h>
// [HK]
#if __GNUC__ >= 3
#include <vector>
using std::vector;
#else
#include <vector.h>
#endif // __GNUC__ >= 3
#include "defs.h"
#include "glob.h"
#include "em_elf.h"
#include "erglob.h"
#include "ercg.h"
#include "tracing.h"
#include "config.h"
#include "config_TARG.h"
#include "config_debug.h"
#include "xstats.h"
#include "topcode.h"
#include "tn.h"
#include "op.h"
#include "targ_isa_lits.h"
#include "cg.h"
#include "cg_flags.h"
#include "cgexp.h"
#include "stblock.h"
#include "sections.h"
#include "data_layout.h"
#include "strtab.h"
#include "symtab.h"
#ifdef TARG_ST
#include "whirl2ops.h"
#include "targ_sections.h"
#endif

/*
 * [CG] temporary flag 
 * (until all the compiler is fixed concerning misaligned accesses)
 * If set this flag tells the expander to decompose a misaligned access.
 * If not set the compiler forces a misaligned access (generate an
 * access as if it were not misaligned).
 * At the end this should be a target dependent and/or user level flag.
 * However currently the compiler itself generate misaligned access, and
 * we don't know how to differenciate this with user generated misaligned
 * accesses. Thus we are obliged to force it.
 */
#define GEN_MISALIGNED_ACCESS

/* Import from exp_targ.cxx */
extern TN *Expand_Or_Inline_Immediate( OPERATION_WITH_INLINED_IMM operation, TN *src, TYPE_ID mtype, OPS *ops);
/* [JV] Defined in exp_targ.cxx */
extern ISA_LIT_CLASS Get_Lit_Class_For_Memory_Access_Size( INT32 size, BOOL is_signed, BOOL is_simulated );


/* ====================================================================
 *   Expand_Lda
 * ====================================================================
 */
void
Expand_Lda (TN *dest, TN *src, OPS *ops)
{
  FmtAssert(FALSE, ("NYI: Expand_Lda"));
}

/* ====================================================================
 *   Pick_Load_Imm_Instruction
 * ====================================================================
 */
static TOP
Pick_Load_Imm_Instruction (
  TYPE_ID rtype, 
  TYPE_ID desc,
  INT64 ofst
)
{
  TOP top = TOP_UNDEFINED;

  switch (desc) {
  case MTYPE_I1: top = TOP_ldr_i9_r_npc_sb; break;
  case MTYPE_U1: top = TOP_ldr_i13_r_npc_b; break;
  case MTYPE_I2: top = TOP_ldr_i9_r_npc_sh; break;
  case MTYPE_U2: top = TOP_ldr_i9_r_npc_h; break;
  case MTYPE_I4:
  case MTYPE_U4:
  case MTYPE_A4:
  case MTYPE_F4: top = TOP_ldr_i13_r_npc; break;

  case MTYPE_V:
    if (rtype != MTYPE_V) {
      // use rtype to pick load (e.g. if lda)
      top = Pick_Load_Imm_Instruction(rtype, rtype, ofst);
    }
    break;
  case MTYPE_F8: 
  case MTYPE_I8: 
  case MTYPE_U8: 
    break;
  default:
    break;
  }

  FmtAssert(top != TOP_UNDEFINED,("Gen_Load_Imm_Instruction: mtype"));

  if (Trace_Exp) {
    fprintf(TFile, "Pick_Load_Imm_Instruction: get top %s\n", TOP_Name(top));
  }


  return top;
}

/* ====================================================================
 *   Pick_Load_Reg_Instruction
 * ====================================================================
 */
static TOP
Pick_Load_Reg_Instruction (
  TYPE_ID rtype, 
  TYPE_ID desc
)
{
  TOP top = TOP_UNDEFINED;

  switch (desc) {
  case MTYPE_I1: top = TOP_ldr_npc_r_npc_sb; break;
  case MTYPE_U1: top = TOP_ldr_npc_r_npc_b; break;
  case MTYPE_I2: top = TOP_ldr_npc_r_npc_sh; break;
  case MTYPE_U2: top = TOP_ldr_npc_r_npc_h; break;
  case MTYPE_I4:
  case MTYPE_U4:
  case MTYPE_A4:
  case MTYPE_F4: top = TOP_ldr_npc_r_npc; break;

  case MTYPE_V:
    if (rtype != MTYPE_V) {
      // use rtype to pick load (e.g. if lda)
      top = Pick_Load_Reg_Instruction(rtype, rtype);
    }
    break;
  case MTYPE_F8: 
  case MTYPE_I8: 
  case MTYPE_U8: 
    break;
  default:
    break;
  }

  FmtAssert(top != TOP_UNDEFINED,("Gen_Load_Reg_Instruction: mtype"));

  if (Trace_Exp) {
    fprintf(TFile, "Pick_Load_Reg_Instruction: get top %s\n", TOP_Name(top));
  }


  return top;
}

void
Set_GP_rel_reloc( TN *ofst, TYPE_ID mtype ) {
  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  switch(mtype) {
  case MTYPE_I1: case MTYPE_U1: 
    Set_TN_is_reloc_da0to11(ofst);
    break;
  case MTYPE_I2: case MTYPE_U2: 
    Set_TN_is_reloc_da1to12(ofst);
    break;
  case MTYPE_I4: case MTYPE_U4: 
  case MTYPE_F4: case MTYPE_A4: 
  case MTYPE_V:
    Set_TN_is_reloc_da2to13(ofst);
    break;
  default:
    if (MTYPE_is_dynamic(mtype)) {
      switch (MTYPE_alignment(mtype)) {
      case 0: case 1:
	Set_TN_is_reloc_da0to11(ofst);
	break;
      case 2:
	Set_TN_is_reloc_da1to12(ofst);
	break;
      case 4:
	Set_TN_is_reloc_da2to13(ofst);
	break;
      case 8:  case 16:
      case 32: case 64:
	Set_TN_is_reloc_da3to14(ofst);
	break;
      default:
	DevWarn("unhandled alignment for dynamic type");
	Set_TN_is_reloc_da0to11(ofst);
	break;
      }
    }
    else {
      FmtAssert(FALSE,("Unmanaged type"));
    }
    break;
  }
#endif
}

/* ====================================================================
 *   Expand_Load
 * ====================================================================
 */
void
Expand_Load (
  OPCODE opcode,
  TN    *result, 
  TN    *base, 
  TN    *ofst, 
  OPS   *ops,
  VARIANT real_alignment
)
{
  TYPE_ID  mtype = OPCODE_desc(opcode);
  TOP      top = TOP_UNDEFINED;
  INT64    val;
  TN      *tmp;

  if(base == NULL || ofst == NULL) {
    FmtAssert(base != NULL && ofst != NULL,("nil base or offset tn given"));
  }

  Is_True (TN_is_constant(ofst), ("Illegal load offset TN"));

  if(TN_register_class(result) == ISA_REGISTER_CLASS_integer) {
    if (TN_has_value(ofst)) {
      INT64 val = TN_value(ofst);
      ISA_LIT_CLASS lc = Get_Lit_Class_For_Memory_Access_Size(MTYPE_byte_size(mtype),MTYPE_signed(mtype), TRUE);
      if(ISA_LC_Value_In_Class (val, lc)) {
	top = Pick_Load_Imm_Instruction (OPCODE_rtype(opcode), mtype,TN_value(ofst));
      }
      else {
	TN *tmp_ofst = Build_TN_Of_Mtype (Pointer_Mtype);
	top = Pick_Load_Reg_Instruction (OPCODE_rtype(opcode), mtype);
	Expand_Immediate (tmp_ofst, ofst, mtype, ops);
	FmtAssert(top != TOP_UNDEFINED,("Expand_Load: Register variant undefined for LOAD TOP"));
	Build_OP (top, result, base, tmp_ofst, ops);
	return;
      }
    } 
    else if (TN_is_symbol(ofst)) {    
      //  
      // TN must be a symbol: SP/FP or GP relative. 
      //
      INT64 base_ofst;
      ST *base_sym;
      
      // get the base_sym: GP/FP/SP and base_ofst from it.
      Base_Symbol_And_Offset_For_Addressing (TN_var(ofst), 
					     TN_offset(ofst), &base_sym, &base_ofst);
      
      // Some symbols don't have their base fixed (formals on small
      // stack) but it's OK with the large offsets:
      //
      //
      // SP/FP/GP relative
      //
      if(Gen_GP_Relative && ST_gprel(base_sym)) {
	FmtAssert(base == GP_TN, ("unexpected non GP TN in Expand_Load"));
	Set_GP_rel_reloc( ofst, mtype );
      }

      ISA_LIT_CLASS lc = Get_Lit_Class_For_Memory_Access_Size(MTYPE_byte_size(mtype),MTYPE_signed(mtype), TRUE);
      if(base == FP_TN || !ISA_LC_Value_In_Class (base_ofst, lc)) {
	/* base == FP_TN: In case of FP relative it is corresponding to large stack, we assume that 
	   immediate ranges are not sufficient, so generates immediate in register.
	*/
	/* ISA_LC_Value_In_Class: Even if in small stack, objects should fit in unsigned 12 bits,
	   developpers can write things like:
	   int main() {
	   char toto[100];
	   char titi;
	   titi = toto[5000];
	   }
	   It is wrong but must not generate assembly failure.
	*/
	TN *tmp_ofst = Build_TN_Of_Mtype (Pointer_Mtype);
	top = Pick_Load_Reg_Instruction (OPCODE_rtype(opcode), mtype);
	Expand_Immediate (tmp_ofst, ofst, mtype, ops);
	FmtAssert(top != TOP_UNDEFINED,("Expand_Load: Register variant undefined for LOAD TOP"));
	Build_OP (top, result, base, tmp_ofst, ops);
	return;
      }

      top = Pick_Load_Imm_Instruction (OPCODE_rtype(opcode), mtype,base_ofst);
    }
    else {
      FmtAssert(FALSE,("unexpected constant in Expand_Load"));
    }

    FmtAssert(top != TOP_UNDEFINED,("Expand_Load: TOP_UNDEFINED"));

    TOP tmp_top = top;
    if (TN_has_value(ofst)) {
      tmp_top = TOP_opnd_immediate_variant(top, 1, TN_value(ofst));
    }
    
    FmtAssert(tmp_top != TOP_UNDEFINED,("Expand_Load: Immediate variant undefined for LOAD %s",TOP_Name(top)));
    top = tmp_top;
    Build_OP (top, result, base, ofst, ops);
  }
  else {
    FmtAssert(FALSE,("Expand_Load: register class not managed"));
  }

  return;
}

/* ====================================================================
 *   Pick_Store_Imm_Instruction
 * ====================================================================
 */
static TOP
Pick_Store_Imm_Instruction (
  TYPE_ID  mtype,
  INT64 ofst
)
{
  TOP top = TOP_UNDEFINED;

  switch (mtype) {
  case MTYPE_I1:
  case MTYPE_U1: top = TOP_str_i13_r_npc_b; break;
  case MTYPE_I2:
  case MTYPE_U2: top = TOP_str_i9_r_npc_h; break;
  case MTYPE_I4:
  case MTYPE_U4:
  case MTYPE_F4:
  case MTYPE_A4: top = TOP_str_i13_r_npc; break;

  case MTYPE_F8: 
  case MTYPE_I8: 
  case MTYPE_U8: 
  default:
    break;
  }
  FmtAssert(top != TOP_UNDEFINED, ("Gen_Store_Imm_Instruction mtype (%s)",MTYPE_name(mtype)));

  if (Trace_Exp) {
    fprintf(TFile, "Pick_Store_Imm_Instruction: get top %s\n", TOP_Name(top));
  }

  return top;
}

/* ====================================================================
 *   Pick_Store_Reg_Instruction
 * ====================================================================
 */
static TOP
Pick_Store_Reg_Instruction (
  TYPE_ID  mtype
)
{
  TOP top = TOP_UNDEFINED;

  switch (mtype) {
  case MTYPE_I1:
  case MTYPE_U1: top = TOP_str_npc_r_npc_b; break;
  case MTYPE_I2:
  case MTYPE_U2: top = TOP_str_npc_r_npc_h; break;
  case MTYPE_I4:
  case MTYPE_U4:
  case MTYPE_F4:
  case MTYPE_A4: top = TOP_str_npc_r_npc; break;

  case MTYPE_F8: 
  case MTYPE_I8: 
  case MTYPE_U8: 
  default:
    break;
  }
  FmtAssert(top != TOP_UNDEFINED, ("Gen_Store_Reg_Instruction mtype (%s)",MTYPE_name(mtype)));

  if (Trace_Exp) {
    fprintf(TFile, "Pick_Store_Reg_Instruction: get top %s\n", TOP_Name(top));
  }

  return top;
}

/* ====================================================================
 *   Expand_Store
 * ====================================================================
 */
void
Expand_Store (
  TYPE_ID mtype,
  TN     *src, 
  TN     *base, 
  TN     *ofst, 
  OPS    *ops,
  VARIANT real_alignment
)
{
  TOP   top = TOP_UNDEFINED;
  TN   *tmp;

  Is_True (TN_is_constant(ofst), ("Illegal store offset TN"));

  if(TN_register_class(src) == ISA_REGISTER_CLASS_integer) {
    if (TN_has_value(ofst)) {
      INT64 val = TN_value(ofst);
      ISA_LIT_CLASS lc = Get_Lit_Class_For_Memory_Access_Size(MTYPE_byte_size(mtype), MTYPE_signed(mtype), TRUE);
      if(ISA_LC_Value_In_Class (val, lc)) {
	top = Pick_Store_Imm_Instruction (mtype,val);
      }
      else {
	TN *tmp_base = Build_TN_Of_Mtype (MTYPE_I4);
	Expand_Add(tmp_base,base,ofst,Pointer_Mtype, ops);
	top = Pick_Store_Imm_Instruction (mtype,0);
	FmtAssert(top != TOP_UNDEFINED,("Expand_Store: Immediate variant undefined for STORE TOP"));
	Build_OP (top, src, tmp_base, Get_Zero_TN(4), ops);
	return;
      }

    }
    else if (TN_is_symbol(ofst)) { 
      //
      // TN must be a symbol: SP/FP or GP relative. 
      //
      INT64 base_ofst;
      ST *base_sym;
      
      // get the base_sym: GP/FP/SP and base_ofst from it.
      Base_Symbol_And_Offset_For_Addressing (TN_var(ofst), 
					     TN_offset(ofst), &base_sym, &base_ofst);
      
      if(Gen_GP_Relative && ST_gprel(base_sym)) {
        //	FmtAssert(base == GP_TN, ("unexpected non GP TN in
        //	Expand_Store"));
	Set_GP_rel_reloc( ofst, mtype );
      }

      ISA_LIT_CLASS lc = Get_Lit_Class_For_Memory_Access_Size(MTYPE_byte_size(mtype),MTYPE_signed(mtype), TRUE);
      if(base == FP_TN || !ISA_LC_Value_In_Class (base_ofst, lc)) {
	/* base == FP_TN: In case of FP relative it is corresponding to large stack, we assume that 
	   immediate ranges are not sufficient, so generates immediate in register.
	*/
	/* ISA_LC_Value_In_Class: Even if in small stack, objects should fit in unsigned 12 bits,
	   developpers can write things like:
	   int main() {
	   char toto[100];
	   char titi;
	   titi = toto[5000];
	   }
	   It is wrong but must not generate assembly failure.
	*/

	TN *tmp_ofst = Build_TN_Of_Mtype (MTYPE_I4);
	TN *tmp_base = Build_TN_Like(base);
	/* AM Rn+Rp and Rn!+Rp are not available for store due register file limited read port number. */
	top = Pick_Store_Imm_Instruction (mtype,0);
	FmtAssert(top != TOP_UNDEFINED,("Expand_Store: Immediate variant undefined for STORE TOP"));
	Expand_Immediate (tmp_ofst, ofst, mtype, ops);
	FmtAssert(TN_is_register(base),("base is not a register"));
	Expand_Add(tmp_base, base, tmp_ofst, Pointer_Mtype, ops);
	Build_OP (top, src, tmp_base, Get_Zero_TN(4), ops);
	return;
      }

      top = Pick_Store_Imm_Instruction (mtype,base_ofst);
    }
    else {
      FmtAssert(FALSE,("unexpected constant in Expand_Store"));
    }
  }

  FmtAssert(top != TOP_UNDEFINED,("Expand_Store: TOP_UNDEFINED"));

  TOP tmp_top = top;
  if (TN_has_value(ofst)) {
    tmp_top = TOP_opnd_immediate_variant(top, 2, TN_value(ofst));
  }

  FmtAssert(tmp_top != TOP_UNDEFINED,("Expand_Store: Immediate variant undefined for STORE %s",TOP_Name(top)));

  top = tmp_top;
  Build_OP (top, src, base, ofst, ops);
  return;
}

/* ====================================================================
 *   INT32 get_variant_alignment(TYPE_ID rtype, VARIANT variant)
 *
 *   Given a variant compute a valid alignment return gcd of m,n;
 *   Used for alignment reasons;
 * ==================================================================== */
static INT32 
get_variant_alignment (
  TYPE_ID rtype, 
  VARIANT variant
)
{
  INT32 r;
  INT32 n= V_alignment(variant);
  INT32 m= MTYPE_alignment(rtype);

  while(r = m % n)
  {
    m=  n;
    n=  r;
  }
  return n;
}

/* ====================================================================
 *   Expand_SinglePush
 *   If Push/Pop exist, use it to store a single register, otherwise
 *   use an automodif addressing mode to store the register on the
 *   stack. The goal of this routine is to store on stack the register
 *   and decrement the stack pointer without using new registers.
 * ====================================================================
 */
void
Expand_SinglePush( TN *base, INT64 ofst, TN *src, BOOL base_auto_modified, OPS *ops) {

  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  FmtAssert(base != NULL,("base is null"));

  switch(TN_register_class(src)) {
  case ISA_REGISTER_CLASS_integer:
    if(base_auto_modified) {
      Build_OP (TOP_sw_i5_pre_dec, base, base, Gen_Literal_TN(ofst, 4), src, ops);
    }
    else {
#if 1
      // Hack to handle adressing modes as signed 9 bits
      Build_OP (TOP_sw_i8_dec, base, Gen_Literal_TN(ofst, 4), src, ops);
#else
      Build_OP (TOP_sw_i8_dec, base, Gen_Literal_TN(ofst, 4), src, ops);
#endif
    }
    break;
  case ISA_REGISTER_CLASS_fpr:
    if(base_auto_modified) {
      Build_OP (TOP_fpx_fsw_i5_pre_dec, base, base, Gen_Literal_TN(ofst, 4), src, ops);
    }
    else {
      Build_OP (TOP_fpx_fsw_i8_dec, base, Gen_Literal_TN(ofst, 4), src, ops);
    }
    break;
  default:
    FmtAssert(FALSE,("Not supported"));
  }
#endif
}

/* ====================================================================
 *   Expand_SinglePop
 *   If Push/Pop exist, use it to load a single register, otherwise
 *   use an automodif addressing mode to load the register from the
 *   stack. The goal of this routine is to load from stack the register
 *   and increment the stack pointer without using new registers.
 * ====================================================================
 */
void
Expand_SinglePop( TN *res, TN *base, INT64 ofst, BOOL base_auto_modified, OPS *ops) {

  FmtAssert(FALSE,("Not yet implemented"));
#if 0

  FmtAssert(base != NULL,("base is null"));
  switch(TN_register_class(res)) {
  case ISA_REGISTER_CLASS_integer:
    if(base_auto_modified) {
      Build_OP (TOP_lw_i5_post_inc, res, base, base, Gen_Literal_TN(ofst, 4), ops);
    }
    else {
#if 1
      // Hack to handle adressing modes as signed 9 bits
      Build_OP (TOP_lw_i8_inc, res, base, Gen_Literal_TN(ofst, 4), ops);
#else
      Build_OP (TOP_lw_i8_inc, res, base, Gen_Literal_TN(ofst, 4), ops);
#endif
    }
    break;
  case ISA_REGISTER_CLASS_fpr:
    if(base_auto_modified) {
      Build_OP (TOP_fpx_flw_i5_post_inc, res, base, base, Gen_Literal_TN(ofst, 4), ops);
    }
    else {
      Build_OP (TOP_fpx_flw_i8_inc, res, base, Gen_Literal_TN(ofst, 4), ops);
    }
    break;
  default:
    FmtAssert(FALSE,("Not supported"));
  }
#endif
}


/* ====================================================================
 *   Composed_Align_Type
 * ====================================================================
 */
static TYPE_ID 
Composed_Align_Type (
  TYPE_ID mtype, 
  VARIANT variant, 
  INT32 *alignment, 
  INT32 *partials
)
{
  INT32 new_align = get_variant_alignment(mtype, variant);

  *alignment =	new_align;
  *partials =	MTYPE_alignment(mtype) / new_align;
 //  return Mtype_AlignmentClass(new_align, (MTYPE_type_class(mtype));
  return Mtype_AlignmentClass(new_align, MTYPE_CLASS_UNSIGNED_INTEGER );
}

/* ====================================================================
 *   OPCODE_make_signed_op
 * ====================================================================
 */
static OPCODE 
OPCODE_make_signed_op (
  OPERATOR op, 
  TYPE_ID rtype, 
  TYPE_ID desc, 
  BOOL is_signed
)
{
  if (MTYPE_is_signed(rtype) != is_signed)
	rtype = MTYPE_complement(rtype);
  if (MTYPE_is_signed(desc) != is_signed)
	desc =	MTYPE_complement(desc);

  return OPCODE_make_op(op, rtype, desc);
}

/* ====================================================================
 *   Adjust_Addr_TNs
 *
 *   We have a memory reference operation, with a base and displacement,
 *   where the displacement is literal.  We want to create another memop
 *   with the displacement modified by a small amount.
 *
 *   WARNING:  If an add operation is required, it will be expanded here.
 * ====================================================================
 */
static void
Adjust_Addr_TNs (
  TOP	opcode,		/* The new memory operation */
  TN	**base_tn,	/* The base address -- may be modified */
  TN	**disp_tn,	/* The displacement -- may be modified */
  INT16	disp,		/* A displacement to add */
  OPS *ops)
{
  if (Potential_Immediate_TN_Expr (opcode, *disp_tn, disp)) {
    if ( TN_has_value(*disp_tn) ) {
      *disp_tn = Gen_Literal_TN ( TN_value(*disp_tn) + disp, 4 );
    } else {
      *disp_tn = Gen_Symbol_TN ( TN_var(*disp_tn),
				 TN_offset(*disp_tn) + disp, TN_relocs(*disp_tn));
    }
  } else {

    TN *tmp = Build_TN_Of_Mtype (Pointer_Mtype);
    // because disp may be symbolic reloc on base,
    // want to still add it with base and create new base and disp.
    Expand_Add (tmp, *disp_tn, *base_tn, Pointer_Mtype, ops);
    *base_tn = tmp;
    *disp_tn = Gen_Literal_TN (disp, 4);
  }
}

/* ============================================================
 * [CG]
 * This function add a constant to the  address TNs (base,disp).
 * The disp must be literal.
 * Returns modified (base,disp).
 * ============================================================
 */
static void
Add_Disp_To_Addr_TNs (
  TN	**base_tn,	/* The base address -- may be modified */
  TN	**disp_tn,	/* The displacement -- may be modified */
  INT16	disp)		/* A displacement to add */
{
  if (TN_has_value(*disp_tn)) {
    *disp_tn = Gen_Literal_TN(TN_value(*disp_tn) + disp, 4);
  } else if (TN_is_symbol(*disp_tn)) {
    *disp_tn = Gen_Symbol_TN(TN_var(*disp_tn), TN_offset(*disp_tn) + disp, TN_relocs(*disp_tn));
  } else {
    FmtAssert(0, ("Unexpected non literal displacement"));
  }
}

/* ====================================================================
 *   Expand_Composed_Load
 * ====================================================================
 */
static void
Expand_Composed_Load ( 
  OPCODE op, 
  TN *result, 
  TN *base, 
  TN *disp, 
  VARIANT variant, 
  OPS *ops
)
{
  TYPE_ID rtype= OPCODE_rtype(op);
  TYPE_ID desc = OPCODE_desc(op);

  if (MTYPE_is_float(rtype)) {
    switch(rtype) {
    case MTYPE_F4:
      Expand_Composed_Load (OPC_I4I4LDID, result, base, disp, variant, ops);
      break;
    case MTYPE_F8:
      Expand_Composed_Load (OPC_I8I8LDID, result, base, disp, variant, ops);
      break;
    default:
      FmtAssert(FALSE, ("Expand_Composed_Load doesn't handle %s",
			MTYPE_name(rtype)));
    }
    return;
  }
    
  INT32		alignment, nLoads, i;
  OPCODE	new_opcode, unsigned_opcode;
  TYPE_ID	new_desc, cvt_rtype, unsigned_cvt_rtype;
  TN		*tmpV[8];

  new_desc = Composed_Align_Type(desc, variant, &alignment, &nLoads);

  if (MTYPE_byte_size(rtype) > 4 &&
      MTYPE_byte_size(new_desc) <= 4) {
    cvt_rtype = Mtype_TransferSign(new_desc, MTYPE_U4);
  } else {
    cvt_rtype = rtype;
  }
  unsigned_cvt_rtype = Mtype_TransferSign(MTYPE_U4, cvt_rtype);

  // unsigned opcode for all but the most significant bits
  unsigned_opcode = OPCODE_make_signed_op(OPR_LDID, cvt_rtype, new_desc, FALSE);
  // opcode of the sign of desc for the most significant bits
  new_opcode = OPCODE_make_signed_op(OPR_LDID, cvt_rtype, new_desc, MTYPE_is_signed(desc));
  

  Is_True(nLoads > 1, ("Expand_Composed_Load with nLoads == %d", nLoads));

 /* Generate the component loads, storing the result in a vector
  * of TNs. The vector is filled in such a way that the LSB is in
  * tmpV[0] so that later code can ignore the endianess of the target.
  */
  INT endian_xor = (Target_Byte_Sex == BIG_ENDIAN) ? (nLoads-1) : 0;
  for (i=0; i < nLoads; i++) {
    INT idx = i ^ endian_xor;
    OPCODE opcode;
    TYPE_ID type;
    if (idx < nLoads-1) {
      opcode = unsigned_opcode;
      type = unsigned_cvt_rtype;
    } else {
      opcode = new_opcode;
      type = cvt_rtype;
    }
    TN *tmp = Build_TN_Of_Mtype(rtype);
    if (rtype != cvt_rtype) {
      TN *cvt_tmp = Build_TN_Of_Mtype(cvt_rtype);
      Expand_Load (opcode, cvt_tmp, base, disp, ops);
      Expand_Convert(tmp, cvt_tmp, Gen_Literal_TN(MTYPE_byte_size(rtype), 4),
		     rtype, type, ops);
    } else {
      Expand_Load (opcode, tmp, base, disp, ops);
    }
    tmpV[idx] = tmp;
    Add_Disp_To_Addr_TNs (&base, &disp, alignment);
  }

  /* 
   * Now combine the components into the desired value.
   */
  INT nLoadBits = alignment * 8;
  TN *tmp0= Build_TN_Of_Mtype(rtype);
  Expand_Shift(tmp0, tmpV[nLoads-1], Gen_Literal_TN((nLoads-1)*nLoadBits, 4),
	       rtype, shift_left, ops);
  for (i=nLoads-2; i >= 0; i--) {
    TN *tmp1= Build_TN_Of_Mtype(rtype);
    TN *tmp2= i == 0 ? result: Build_TN_Of_Mtype(rtype);
    TN *part = tmpV[i];
    Expand_Shift(tmp1, part, Gen_Literal_TN(i*nLoadBits, 4),
		 rtype, shift_left, ops);
    Expand_Binary_Or(tmp2, tmp0, tmp1, rtype, ops);
    tmp0 = tmp2;
  }
}


/* ====================================================================
 *   Expand_Forced_Misaligned_Load
 *
 *   Generate a load at the given addr + offset as if the load was
 *   correctly aligned. This should end into a runtime error for
 *   machines with unaligned access not supported.
 *   It is an alternative to Expand_Composed_Load.
 *   Called from Expand_Misaligned_Load only.
 * ====================================================================
 */
static void
Expand_Forced_Misaligned_Load (
  OPCODE op, 
  TN *result, 
  TN *base, 
  TN *disp, 
  OPS *ops
  )
{
  TN *new_base = Build_TN_Of_Mtype (Pointer_Mtype);
  TN *new_offset = Gen_Literal_TN(0, 4);
  Expand_Add (new_base, base, disp, Pointer_Mtype, ops);
  Expand_Load (op, result, new_base, new_offset, ops);
}

/* ====================================================================
 *   Expand_Misaligned_Load
 * ====================================================================
 */
void
Expand_Misaligned_Load ( 
  OPCODE op, 
  TN *result, 
  TN *base, 
  TN *disp, 
  VARIANT variant, 
  OPS *ops
)
{
  DevWarn("misaligned load encountered: TODO : differenciate between compiler an user generated");

  if (TN_register_class(result) <= ISA_REGISTER_CLASS_STATIC_MAX) {
    // Core or static extension load
    if (Enable_Misaligned_Access)
      Expand_Forced_Misaligned_Load(op, result, base, disp, ops);
    else 
      Expand_Composed_Load (op, result, base, disp, variant, ops);
  }
  else {
    // Extension misaligned load are directly handled by Expand_Load
    Expand_Load (op, result, base, disp, ops, variant);
  }
}

/* ====================================================================
 *   Expand_Composed_Store
 * ====================================================================
 */
static void
Expand_Composed_Store (
  TYPE_ID mtype, 
  TN *obj, 
  TN *base, 
  TN *disp, 
  VARIANT variant, 
  OPS *ops
)
{
  if (MTYPE_is_float(mtype)) {
    switch(mtype) {
    case MTYPE_F4:
      Expand_Composed_Store (MTYPE_I4, obj, base, disp, variant, ops);
      break;
    case MTYPE_F8:
      Expand_Composed_Store (MTYPE_I8, obj, base, disp, variant, ops);
      break;
    default:
      FmtAssert(FALSE, ("Expand_Composed_Store doesn't handle %s",
			MTYPE_name(mtype)));
    }
    return;
  }

  INT32		alignment, nStores;
  TYPE_ID	new_desc;

  new_desc =	Composed_Align_Type(mtype, variant, &alignment, &nStores);

  if (Target_Byte_Sex == BIG_ENDIAN)
    Add_Disp_To_Addr_TNs (&base, &disp, MTYPE_alignment(mtype)-alignment);

  if (MTYPE_byte_size(mtype) > 4 &&
      MTYPE_byte_size(new_desc) <= 4) {
    TN *tmp = Build_TN_Of_Mtype (MTYPE_U4);
    Expand_Convert(tmp, obj, Gen_Literal_TN(32, 4), MTYPE_U4, mtype, ops);
    Expand_Store (new_desc, tmp, base, disp, ops); 
  } else {
    Expand_Store (new_desc, obj, base, disp, ops); 
  }

  while(--nStores >0) {
    TN *tmp = Build_TN_Of_Mtype(mtype);
    Expand_Shift(tmp, obj, Gen_Literal_TN(alignment*8, 4), 
                                                mtype, shift_lright, ops);
    obj = tmp;
    if (Target_Byte_Sex == BIG_ENDIAN)
      Add_Disp_To_Addr_TNs (&base, &disp, -alignment);
    else 
      Add_Disp_To_Addr_TNs (&base, &disp, alignment);
    if (MTYPE_byte_size(mtype) > 4 &&
	MTYPE_byte_size(new_desc) <= 4) {
      TN *tmp = Build_TN_Of_Mtype (MTYPE_U4);
      Expand_Convert(tmp, obj, Gen_Literal_TN(32, 4), MTYPE_U4, mtype, ops);
      Expand_Store (new_desc, tmp, base, disp, ops); 
    } else {
      Expand_Store (new_desc, obj, base, disp, ops); 
    }
  }
}

/* ====================================================================
 *   Expand_Forced_Misaligned_Store
 *
 *   Generate a store at the given addr + offset as if the store was
 *   correctly aligned. This should end into a runtime error for
 *   machines with unaligned access not supported.
 *   It is an alternative to Expand_Composed_Store.
 *   Called from Expand_Misaligned_Store only.
 * ====================================================================
 */
static void
Expand_Forced_Misaligned_Store (
  TYPE_ID mtype, 
  TN *obj_tn, 
  TN *base_tn, 
  TN *disp_tn, 
  OPS *ops
  )
{
  TN *new_base = Build_TN_Of_Mtype (Pointer_Mtype);
  TN *new_offset = Gen_Literal_TN(0, 4);
  Expand_Add (new_base, base_tn, disp_tn, Pointer_Mtype, ops);
  Expand_Store (mtype, obj_tn, new_base, new_offset, ops);
}

static BOOL is_own_func_p (
  ST *st
  )
{
  if (ST_class (st) == CLASS_FUNC) {
    if (! ST_is_preemptible (st))
      return TRUE;
    else if (!Gen_PIC_Shared) {
      return (ST_sclass (st) != SCLASS_EXTERN)
	&& ! ST_is_weak_symbol (st);
    }
  }
  return FALSE;
}

/* ====================================================================
 *   Expand_Misaligned_Store
 * ====================================================================
 */
void
Expand_Misaligned_Store (
  TYPE_ID mtype, 
  TN *obj_tn, 
  TN *base_tn, 
  TN *disp_tn, 
  VARIANT variant, 
  OPS *ops
)
{
  DevWarn("misaligned store encountered: TODO : differenciate between compiler an user generated");

  if (TN_register_class(obj_tn) <= ISA_REGISTER_CLASS_STATIC_MAX) {
    // Core or static extension store
    if (Enable_Misaligned_Access) 
      Expand_Forced_Misaligned_Store (mtype, obj_tn, base_tn, disp_tn, ops);
    else
      Expand_Composed_Store (mtype, obj_tn, base_tn, disp_tn, variant, ops);
  }
  else {
    // Extension misaligned store are directly handled by Expand_Store
    Expand_Store (mtype, obj_tn, base_tn, disp_tn, ops, variant);
  }
}

/* ====================================================================
 *   Exp_Ldst
 * ====================================================================
 */
static void
Exp_Ldst (
  OPCODE opcode,
  TN *tn,
  ST *sym,
  INT64 ofst,
  BOOL indirect_call,
  BOOL is_store,
  BOOL is_load,
  OPS *ops,
  VARIANT variant
)
{
  ST *base_sym;
  INT64 base_ofst;
  TN *base_tn;
  TN *ofst_tn;
  BOOL is_lda = (!is_load && !is_store);
  OPS newops;
  OP *op;
  OPS_Init(&newops);

  if (Trace_Exp) {
    fprintf(TFile, "exp_ldst %s: ", OPCODE_name(opcode));
    if (tn) Print_TN(tn,FALSE);
    if (is_store) fprintf(TFile, " -> ");
    else fprintf(TFile, " <- ");
    if (ST_class(sym) == CLASS_CONST)
      fprintf ( TFile, "<constant: %s>\n", Targ_Print(NULL,STC_val(sym)));
    else
      fprintf(TFile, "%lld (%s)\n", ofst, ST_name(sym));
  }

  Allocate_Object(sym);         /* make sure sym is allocated */

  Base_Symbol_And_Offset_For_Addressing (sym, ofst, &base_sym, &base_ofst);

  if (Trace_Exp) {
    fprintf(TFile, "exp_ldst: ");
    fprintf(TFile, "base %s, ", ST_name(base_sym));
    fprintf(TFile, "ofst %lld \n", base_ofst);
  }

  if (ST_on_stack(sym)) {
    // formals on small stack are not assigned to sp/fp yet
    // cause base is not finished, but they will be assigned to SP.
    // So only use FP if already based on FP.
    base_tn = (base_sym == FP_Sym) ? FP_TN : SP_TN;

    // [JV] This is applicable on base SP or FP.
    // For varargs, in va_start, we generate an LDA on
    // upformal block symbol and fall in here if the
    // test is only sym == base_sym.
    if (sym == base_sym && (sym == SP_Sym || sym == FP_Sym)) {
      // can have direct reference to SP or FP,
      // e.g. if actual stored to stack.
      ofst_tn = Gen_Literal_TN (base_ofst, Pointer_Size);
    }
    else {
      /* Because we'd like to see symbol name in .s file, 
       * still reference the symbol rather than the sp/fp base.  
       * Do put in the offset from the symbol.  
       * We put the symbol in the TN and then
       * let cgemit replace symbol with the final offset.
       * We generate a SW reg, <sym>, <SP> rather than SW reg,<sym>
       * because cgemit and others expect a separate tn for the
       * offset and base. 
       */
      ofst_tn = Gen_Symbol_TN (sym, ofst, TN_RELOC_NONE);
    }
  }
  else if (ST_is_thread_private (sym)) {
    FmtAssert(FALSE,("Not implemented"));
#if 0
    ST_TLS_MODEL tls_model = ST_tls_model (sym);
    switch (tls_model) {
    case ST_TLS_MODEL_LOCAL_EXEC:
      {
	TN *sym_tn = Gen_Symbol_TN (base_sym, 0, TN_RELOC_TPREL);
	if (base_ofst == 0) {
	  base_tn = TP_TN;
	  ofst_tn = sym_tn;
	} else {
	  base_tn = Build_TN_Of_Mtype (Pointer_Mtype);
	  Expand_Add (base_tn, TP_TN, sym_tn, Pointer_Mtype, &newops);
	  ofst_tn = Gen_Literal_TN (base_ofst, Pointer_Size);
	}
      }
      break;
    case ST_TLS_MODEL_INITIAL_EXEC:
      {
	TN *tmp1 = Build_TN_Of_Mtype (Pointer_Mtype);
	base_tn = Build_TN_Of_Mtype (Pointer_Mtype);
	Expand_Load (OPCODE_make_signed_op(OPR_LDID,
					   Pointer_Mtype, Pointer_Mtype, FALSE),
		     tmp1, GP_TN, Gen_Symbol_TN (sym, 0, TN_RELOC_GOTOFF_TPREL), &newops);
	// GOT address should not alias
	Set_OP_no_alias (OPS_last (&newops));
	PU_References_GP = TRUE;
	Expand_Add (base_tn, TP_TN, tmp1, Pointer_Mtype, &newops);
	ofst_tn = Gen_Literal_TN (ofst, Pointer_Size);
      }
      break;
    case ST_TLS_MODEL_LOCAL_DYNAMIC:
    case ST_TLS_MODEL_GLOBAL_DYNAMIC:
      {
	TY_IDX ptype = MTYPE_To_TY (Pointer_Mtype);
	ST *get_addr_sym = Gen_Intrinsic_Function (Make_Function_Type (ptype),
						   "__tls_get_addr");
	TN *target = Gen_Symbol_TN(get_addr_sym, 0, TN_RELOC_NONE);
	TN *p1_tn;
	INT32 reloc = (tls_model == ST_TLS_MODEL_LOCAL_DYNAMIC)
	  ? TN_RELOC_GOTOFF_DTPLDM : TN_RELOC_GOTOFF_DTPNDX;
	ISA_REGISTER_CLASS p1_rclass;
	REGISTER p1_reg;
	CGTARG_Preg_Register_And_Class (First_Int_Preg_Param_Offset,
					&p1_rclass, &p1_reg);
	p1_tn = Build_Dedicated_TN (p1_rclass, p1_reg,
				    ST_size(MTYPE_To_PREG (Pointer_Mtype)));
	Expand_Add(p1_tn, GP_TN, Gen_Symbol_TN(sym, 0, reloc),
		   Pointer_type, &newops);
	PU_References_GP = TRUE;
	Exp_Call (OPR_CALL, RA_TN, target, &newops);
	PU_Has_Calls = TRUE;
	WN *wn_call = WN_Call (Pointer_type, Pointer_type, 1, get_addr_sym);
	WN_actual (wn_call, 0) = WN_Lda (Pointer_type, 0, sym);
	WN_call_flag (wn_call) = 0;
	WN_Set_Call_Default_Flags (wn_call);
	CALLINFO *call_info = TYPE_PU_ALLOC (CALLINFO);
	CALLINFO_call_st(call_info) = get_addr_sym;
	CALLINFO_call_wn(call_info) = wn_call;
	OP_MAP_Set(OP_to_callinfo_map, OPS_last(&newops), call_info);
	TN *res_tn;
	ISA_REGISTER_CLASS res_rclass;
	REGISTER res_reg;
	CGTARG_Preg_Register_And_Class (First_Int_Preg_Return_Offset,
					&res_rclass, &res_reg);
	res_tn = Build_Dedicated_TN (res_rclass, res_reg,
				     ST_size (MTYPE_To_PREG (Pointer_Mtype)));
	base_tn = Build_TN_Of_Mtype (Pointer_Mtype);
	if (tls_model == ST_TLS_MODEL_LOCAL_DYNAMIC) {
	  TN *sym_tn = Gen_Symbol_TN (sym, 0, TN_RELOC_DTPREL);
	  if (ofst == 0) {
	    Exp_COPY (base_tn, res_tn, &newops);
	    ofst_tn = sym_tn;
	  } else {
	    Expand_Add (base_tn, res_tn, sym_tn, Pointer_type, &newops);
	    ofst_tn = Gen_Literal_TN (ofst, Pointer_Size);
	  }
	} else {
	  Exp_COPY (base_tn, res_tn, &newops);
	  ofst_tn = Gen_Literal_TN (ofst, Pointer_Size);
	}
      }
      break;
    }
#endif
  }
  else if (Gen_GP_Relative && ST_gprel(base_sym)) {
    base_tn = GP_TN;
    // Use rtype for size of LDA
    ofst_tn = Gen_Symbol_TN (sym, ofst, TN_RELOC_NONE);
  }
  else {
    // Not gp-relative reference

    FmtAssert(!ST_gprel(base_sym),
                 ("Exp_Ldst: %s is set gp-relative", ST_name(base_sym)));
    FmtAssert(!Gen_PIC_Shared && ! Gen_PIC_Call_Shared,
	      ("Exp_Ldst: Absolute reference forbidden by ABI"));
#if 0
    // address TNs
    TN *tmp1 = Build_TN_Of_Mtype (Pointer_Mtype);
#endif
    // address TNs
    TN *tmp1;

    if (is_lda && ofst == 0) {
      // want to stop at address (either that or add with 0)
      tmp1 = tn;
      is_lda = FALSE;	// so nothing done
    }
    else {
      // address TNs
      tmp1 = Build_TN_Of_Mtype (Pointer_Mtype);
    }

    if (Trace_Exp) {
      fprintf(TFile, "exp_ldst: generate copy of symbol not gp relative\n");
    }

    // because it is not GP-relative, just make the address
    // the offset will be addede by an addu later.
    Expand_Symbol (tmp1, Gen_Symbol_TN (sym, 0, TN_RELOC_NONE), ops);
    
    // load is of address, not of result type
    base_tn = tmp1;
    
    // because it is not GP-relative, just use the address
    // load is of address, not of result type
    //      base_tn = Gen_Symbol_TN (sym, 0, TN_RELOC_NONE);
    
    // add offset to address
    ofst_tn = Gen_Literal_TN(ofst, Pointer_Size);
  }
  if (is_store) {
    if ((V_alignment(variant) == V_NONE) || V_overalign(variant)) {
      // variant might be used to encode overaligned access (useful for extension)
      Expand_Store (OPCODE_desc(opcode), tn, base_tn, ofst_tn, &newops, variant);
    }
    else {
      Expand_Misaligned_Store (OPCODE_desc(opcode), tn, 
			       base_tn, ofst_tn, variant, &newops);
    }
  }
  else if (is_load ) {
    if ((V_alignment(variant) == V_NONE) || V_overalign(variant)) {
      // variant might be used to encode overaligned access (useful for extension)
      Expand_Load (opcode, tn, base_tn, ofst_tn, &newops, variant);
    }
    else {
      Expand_Misaligned_Load (opcode, tn, 
			      base_tn, ofst_tn, variant, &newops);
    }
  }
  else if (is_lda) {
    Expand_Add (tn, base_tn, ofst_tn, OPCODE_rtype(opcode), &newops);
  }

  FOR_ALL_OPS_OPs (&newops, op) {
    if (is_load && ST_is_constant(sym) && OP_load(op)) {
      // If we expanded a load of a constant, 
      // nothing else can alias with the loads 
      // we have generated.
      Set_OP_no_alias(op);
    }
    if (Trace_Exp) {
      fprintf(TFile, "exp_ldst into "); Print_OP (op);
    }
  }

  /* Add the new OPs to the end of the list passed in */
  OPS_Append_Ops(ops, &newops);

  return;
}

/* ====================================================================
 *   Exp_Lda
 * ====================================================================
 */
void Exp_Lda ( 
  TYPE_ID mtype, 
  TN *tgt_tn, 
  ST *sym, 
  INT64 ofst, 
  OPERATOR call_opr,
  OPS *ops
)
{
  OPCODE opcode = OPCODE_make_op(OPR_LDA, mtype, MTYPE_V);
  Exp_Ldst (opcode, tgt_tn, sym, ofst, (call_opr == OPR_ICALL),
	                                     FALSE, FALSE, ops, V_NONE);
  return;
}

/* ====================================================================
 *   Exp_Load
 * ====================================================================
 */
void
Exp_Load (
  TYPE_ID rtype, 
  TYPE_ID desc, 
  TN *tgt_tn, 
  ST *sym, 
  INT64 ofst, 
  OPS *ops, 
  VARIANT variant
)
{
  OPCODE opcode = OPCODE_make_op (OPR_LDID, rtype, desc);
  Exp_Ldst (opcode, tgt_tn, sym, ofst, FALSE, FALSE, TRUE, ops, variant);
  return;
}

/* ====================================================================
 *   Exp_Store
 * ====================================================================
 */
void
Exp_Store (
  TYPE_ID mtype, 
  TN *src_tn, 
  ST *sym, 
  INT64 ofst, 
  OPS *ops, 
  VARIANT variant
)
{
  OPCODE opcode = OPCODE_make_op(OPR_STID, MTYPE_V, mtype);
  Exp_Ldst (opcode, src_tn, sym, ofst, FALSE, TRUE, FALSE, ops, variant);
}

/* ====================================================================
 *   Exp_Prefetch
 * ====================================================================
 */
void Exp_Prefetch (
  TOP opc, 
  TN* src1, 
  TN* src2, 
  VARIANT variant, 
  OPS* ops
)
{
  FmtAssert (FALSE, ("not yet implemented"));
#if 0
  
  FmtAssert(opc == TOP_UNDEFINED,
            ("Prefetch opcode should be selected in Exp_Prefetch"));
  
  opc = TOP_pft_i;
  if (!TN_has_value(src2)) {
    TN* tmp = Build_TN_Like(src1);
    Expand_Add (tmp, src1, src2, Pointer_Mtype, ops);
    src1 = tmp;
    src2 = Gen_Literal_TN (0, 4);
  }
  opc = TOP_opnd_immediate_variant(opc, 1, TN_value(src2));
  FmtAssert(opc != TOP_UNDEFINED,("Expand_Load: Immediate variant undefined for PREFETCH TOP"));

  Build_OP(opc, src1, src2, ops);
#endif
  return;
}

/* ======================================================================
 *   Exp_Extract_Bits
 * ======================================================================*/
void
Exp_Extract_Bits (
  TYPE_ID rtype, 
  TYPE_ID desc, 
  UINT bit_offset, 
  UINT bit_size,
  TN *tgt_tn, 
  TN *src_tn, 
  OPS *ops
)
{
  TN *tmp;
  UINT bit_ofst;
  UINT left_shift;
  UINT right_shift;

  FmtAssert(MTYPE_is_class_integer(rtype) && MTYPE_bit_size(rtype) == 32,
	    ("can't handle"));

  if (Target_Byte_Sex == LITTLE_ENDIAN) {
      // for LX as LITTLE_ENDIAN:
      bit_ofst = bit_offset;
  } else {
      // for LX as BIG_ENDIAN:
      bit_ofst = MTYPE_bit_size(desc) - bit_size - bit_offset;
  }

  // shift left -> clear the bits bit_offset+size -> 31
  left_shift = 32 - bit_ofst - bit_size;
  if (left_shift) {
    tmp = Build_RCLASS_TN (ISA_REGISTER_CLASS_integer);
    Expand_Shift(tmp,src_tn,Gen_Literal_TN(left_shift, 4), desc, shift_left, ops);
  } else {
      tmp = src_tn;
  }

  // move the bits to be extracted to position 0
  right_shift = 32 - bit_size;
  Expand_Shift(tgt_tn, tmp,Gen_Literal_TN(right_shift, 4), desc, MTYPE_signed(desc) ? shift_aright : shift_lright, ops);

  return;
}

/* ======================================================================
 *   Exp_Deposit_Bits 
 *
 *   deposit src2_tn into a field of src1_tn returning the result in 
 *   tgt_tn.
 *   Note that this code is endian dependent.
 *   
 * ======================================================================
 */
void
Exp_Deposit_Bits (
  TYPE_ID rtype, 
  TYPE_ID desc, 
  UINT bit_offset, 
  UINT bit_size,
  TN *tgt_tn, 
  TN *src1, 
  TN *src2, 
  OPS *ops
)
{
  UINT bit_ofst;

  FmtAssert(FALSE,("Not yet implemented"));
#if 0
  FmtAssert(bit_size != 0, ("size of bit field cannot be 0"));

  // Registers are always 32 bits in size
  FmtAssert(MTYPE_is_class_integer(rtype) && MTYPE_bit_size(rtype) == 32,
	  ("Exp_Deposit_Bits: mtype cannot be %s", MTYPE_name(rtype)));

  if (Target_Byte_Sex == LITTLE_ENDIAN) {
      // for LX as LITTLE_ENDIAN:
      bit_ofst = bit_offset;
  } else  {
      // for LX as BIG_ENDIAN
      bit_ofst = MTYPE_bit_size(desc) - bit_size - bit_offset;
  }

  if (Trace_Exp) {
    fprintf(TFile, "%s: bit_ofst = %d, bit_size = %d\n",__FUNCTION__,bit_ofst,bit_size);
  }

  //
  // generate the following sequence (if there is a faster one,
  // we'll find it later)
  //
  //    mask = 0..01110..0           (in the bit pos required)
  //    val  = shl src2, 32-bit_size (clear bits)
  //    val  = shru val, 32-(bit_pos+bit_size) (place value in position)
  //    tmp1 = mask and src1         (only bits in mask pos are valid)
  //    tmp1 = tmp1 xor src1         (bits in mask pos are zero)
  //    tgt_tn = tmp1 or val

  TN *mask = Build_RCLASS_TN (ISA_REGISTER_CLASS_integer);
  
  // set mask to all 1    111....111
  Expand_Immediate(mask, Gen_Literal_TN (-1, 4), MTYPE_I4, ops);
  // only keep bit_size bits to 1
  Build_OP(TOP_shru_i5, mask, mask, 
           Gen_Literal_TN(32-bit_size,4), ops);
  
  // shift left to position the bit_size bits to the required position
  Build_OP(TOP_shlu_i5, mask, mask, Gen_Literal_TN(bit_ofst,4), ops);
  
  // clear bits
  TN *val = Build_RCLASS_TN (ISA_REGISTER_CLASS_integer);
  Build_OP(TOP_shlu_i5, val, src2, Gen_Literal_TN(32-bit_size,4), ops);

  // place value in position
  Build_OP(TOP_shru_i5, val, val,Gen_Literal_TN (32-bit_size-bit_ofst, 4), ops);

  TN *tmp1 = Build_RCLASS_TN (ISA_REGISTER_CLASS_integer);
  Build_OP(TOP_and_r, tmp1, mask, src1, ops);
  Build_OP(TOP_xor_r, tmp1, tmp1, src1, ops);
      
  Build_OP(TOP_or_r, tgt_tn, tmp1, val, ops);
#endif  
  return;
}

/* ======================================================================
 *   Exp_LRotate 
 *
 *   rotate-left src1_tn from the value src2_tn
 *   
 * ======================================================================
 */
void
Exp_LRotate (
  TYPE_ID rtype, 
  TN *tgt_tn, 
  TN *src1_tn, 
  TN *src2_tn, 
  OPS *ops
)
{

  FmtAssert(FALSE,("Not yet implemented"));
#if 0

  FmtAssert(MTYPE_bit_size(rtype) == 32, ("size of bit field must be 32"));

  if (Enable_Rotate){
    TOP top = TOP_rotl_r;
    if (TN_Has_Value(src2_tn)){
      if ((TN_Value(src2_tn) & 0x1F) == 0) {
	Expand_Copy(tgt_tn, NULL, src1_tn, ops);
	return;
      }
      top = TOP_opnd_immediate_variant(top, 1, TN_value(src2_tn) & 0x1F);
    }
    Build_OP(top, tgt_tn, True_TN, src1_tn, src2_tn, ops);
  } else
    FmtAssert(FALSE, ("Exp_LRotate: unexpected attempt to generate a rotate-left instruction"));
#endif
  return;
}

/* ======================================================================
 *   Exp_RRotate 
 *
 *   rotate-right src1_tn from the value src2_tn
 *   
 * ======================================================================
 */
void
Exp_RRotate (
  TYPE_ID rtype, 
  TN *tgt_tn, 
  TN *src1_tn, 
  TN *src2_tn, 
  OPS *ops
)
{
  FmtAssert(FALSE,("Not yet implemented"));
#if 0

  FmtAssert(MTYPE_bit_size(rtype) == 32, ("size of bit field must be 32"));

  if (Enable_Rotate){
    TOP top = TOP_rotl_r;
    if (TN_Has_Value(src2_tn)){
      if ((TN_Value(src2_tn) & 0x1F) == 0) {
	Expand_Copy(tgt_tn, NULL, src1_tn, ops);
	return;
      }
      top = TOP_opnd_immediate_variant(top, 1, 32 - TN_value(src2_tn) & 0x1F);
      Build_OP(top, tgt_tn, src1_tn, Gen_Literal_TN(32 - TN_Value(src2_tn) & 0x1F, 4), ops);
    } else {
      TN *val1 = Build_RCLASS_TN (ISA_REGISTER_CLASS_gpr);
      TN *val2 = Build_RCLASS_TN (ISA_REGISTER_CLASS_gpr);
      Expand_Immediate(val1, Gen_Literal_TN(32, 4), MTYPE_I4, ops);
      Expand_Sub (val2, val1, src2_tn, MTYPE_I4, ops);
      Build_OP(top, tgt_tn, True_TN, src1_tn, val2, ops);
    }
  } else
    FmtAssert(FALSE, ("Exp_RRotate: unexpected attempt to generate a rotate-right instruction"));
#endif
  return;
}

/* ======================================================================
 *   Expand_Lda_Label
 * ======================================================================
 */
void 
Expand_Lda_Label (
  TN *dest, 
  TN *lab, 
  OPS *ops
)
{
#if 0
  Print_TN (dest, TRUE );
  Print_TN (lab, TRUE );
#endif

  // [CG]: On the STxP70 base model, address of a label
  // is the absolute address. There is also
  // no possiblity to load it from a GP relative offset
  Expand_Copy(dest, True_TN, lab, ops);
  return;
}





