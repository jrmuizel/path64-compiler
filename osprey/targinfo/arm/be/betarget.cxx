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
 * Module: betarget.cxx
 *
 * Description:
 *
 * Support routines for target-specific functionality.
 *
 * ====================================================================
 * ====================================================================
 */

#include <stdlib.h>
#include "defs.h"
#include "errors.h"
#include "util.h"
#include "tracing.h"
#include "topcode.h"
#include "wn.h"
#include "opcode.h"
#include "intrn_info.h"
#include "config_target.h"
#include "config_TARG.h"
#include "targ_isa_lits.h"
#include "targ_isa_properties.h"
#include "betarget.h"
#include "w2op.h"
#include "wutil.h"
#ifdef TARG_ST
#include "config_TARG.h"	/* For Enable_Non_IEEE_Ops. */
#include "wn_lower_util.h"
#endif

BOOL Targ_Lower_Float_To_Unsigned = FALSE;
BOOL Targ_Lower_Unsigned_To_Float = FALSE;

/* ============================================================
 *
 * BOOL mem_offset_must_be_split(WN_OFFSET offset)
 * WN_OFFSET mem_offset_hi(WN_OFFSET offset)
 * WN_OFFSET mem_offset_lo(WN_OFFSET offset)
 *
 * Returns TRUE iff <offset> must be split for a target-machine memory
 * reference.  If so, mem_offset_hi(offset) returns the high part of
 * the split offset, and mem_offset_lo(offset) returns the low part of
 * the split offset.
 * 
 * ============================================================
 */
BOOL 
mem_offset_must_be_split (INT64 offset)
{
  return FALSE;
}

/* ============================================================
 *    OPCODE_To_TOP (opcode)
 *
 *    only return machine_ops, TOP_UNDEFINED if not an exact 
 *    correspondence 
 * ============================================================
 */
TOP
OPCODE_To_TOP (
  OPCODE opcode
)
{
  OPERATOR opr   = OPCODE_operator (opcode);
  TYPE_ID  rtype = OPCODE_rtype (opcode);
  TYPE_ID  desc  = OPCODE_desc  (opcode);

  switch (opr) {

    case OPR_FORWARD_BARRIER:
      return TOP_fwd_bar;

    case OPR_BACKWARD_BARRIER:
      return TOP_bwd_bar;

    case OPR_INTRINSIC_CALL:
      if (rtype == MTYPE_V) return TOP_intrncall;
      else                  return TOP_UNDEFINED;

    case OPR_NEG:

    case OPR_ABS:
      return TOP_UNDEFINED;

    case OPR_PAREN:
           if (rtype == MTYPE_F4) return TOP_noop;
      else if (rtype == MTYPE_F8) return TOP_noop;
      else                        return TOP_UNDEFINED;

    case OPR_PARM:
      return TOP_noop;

    case OPR_TRAP:

    default:
      return TOP_UNDEFINED;
  }
}

/* ============================================================
 *   TAS_To_TOP (tas_wn)
 *
 *   pick the opcode corresponding to the TAS, which will either
 *   be a float<->int move or a no-op. 
 * ============================================================
 */
TOP
TAS_To_TOP (WN *tas_wn)
{
  TYPE_ID kid_mtype = WN_rtype(WN_kid0(tas_wn));

  switch (WN_opcode(tas_wn)) {
    case OPC_I8TAS:
    case OPC_U8TAS:

    case OPC_I4TAS:
    case OPC_U4TAS:

    case OPC_F8TAS:

    case OPC_F4TAS:
        return TOP_noop;

    default:
        return TOP_UNDEFINED;
  }
}

/* =============================================================
 * BETARG_is_emulated_type
 * 
 * Returns true when the emulation is forced for the given type.
 * When true, the operators using this type should be emulated.
 * This is only for target specific emulation as generic
 * support in is_emulated_type() will have find generic types
 * that requires emulation.
 *
 * =============================================================
 */
BOOL
BETARG_is_emulated_type(TYPE_ID type)
{
  return FALSE;
}

/* =============================================================
 * BETARG_is_emulated_operator
 * 
 * used in rt_lower_wn to know if an operator must be lowered 
 * as runtime call or inlined by code expansion.
 * Flags like 'Emulate_Single_Float_Type' or 'Enable_Non_IEEE_Ops'
 * initialised in config_target should be used here.
 * Some generic code have already filtered some opr before this call
 * see the generic is_emulated_operator() function.
 *
 * =============================================================
 */
BOOL
BETARG_is_emulated_operator( OPERATOR opr, TYPE_ID rtype, TYPE_ID desc)
{

  if(MTYPE_is_integral(rtype) || MTYPE_is_integral(desc)) {
    // [VCdV] add test on presence of enable_mx option activating
    // integer multiplier.
    switch (opr) {
    case OPR_MOD:
    case OPR_REM:
    case OPR_DIV:
      if (Emulate_DivRem_Integer_Ops) {
	return TRUE;
      }
      break;
    }
  }
    

  /* All long long operators require emulation if emulation is on. */
  if (WN_Is_Emulated_Type(MTYPE_I8) &&
      (MTYPE_is_longlong(rtype) || MTYPE_is_longlong(desc))) return TRUE;


  /* All double operators require emulation if emulation is on. */
  if (WN_Is_Emulated_Type(MTYPE_F8) && 
      (MTYPE_is_double(rtype) || MTYPE_is_double(desc))) return TRUE;
  
  /* If the target options may allow non-ieee architectural support. 
   * In this case even if emulation is requested, 
   * some of the basic operators do not require emulation. */
  if (WN_Is_Emulated_Type(MTYPE_F4) && Enable_Non_IEEE_Ops &&
      (rtype == MTYPE_F4 || desc == MTYPE_F4)) {
    switch (opr) {
    case OPR_ADD: 
    case OPR_SUB: 
    case OPR_MPY:
    case OPR_DIV:
    case OPR_ABS:
    case OPR_NEG:
    case OPR_MADD: 
    case OPR_MSUB: 
    case OPR_NMADD: 
    case OPR_NMSUB: 
    case OPR_LT:
    case OPR_LE:
    case OPR_EQ:
    case OPR_NE:
    case OPR_GE:
    case OPR_GT:
    case OPR_MIN:
    case OPR_MAX:
	return FALSE;
      break;
    case OPR_CVT:
    case OPR_TRUNC:
      if (rtype == MTYPE_I4 || rtype == MTYPE_U4 || desc == MTYPE_I4 || desc == MTYPE_U4 )
	return FALSE;
      break;
    default:
      break;
    }
  }

  /* Otherwise, other single operators require emulation if emulation is on. */
  if (WN_Is_Emulated_Type(MTYPE_F4) && 
      (rtype == MTYPE_F4 || desc == MTYPE_F4)) return TRUE;

  /* In other cases, no emulation is requested. */

  return FALSE;
}

/* =============================================================
 * BETARG_is_enabled_operator
 * 
 * Used by wn_lower to know if the given WHIRL operator is
 * allowed for this target.
 * If this function returns false, wn_lower will never generate
 * a WHIRL tree containing this operator.
 *
 * If the function returns true, the operator is supposed to either:
 * - be lowered by the runtime lowering in call/intrinsic,
 * - be selected by the code selector (expand).
 *
 * By default, all operators are activated. So this function 
 * should proceed by exclusion.
 * Currently this function is used for MADD and alike operations.
 * =============================================================
 */
BOOL
BETARG_is_enabled_operator(OPERATOR opr, TYPE_ID rtype, TYPE_ID desc)
{

  BOOL is_madd = opr == OPR_MADD || opr == OPR_MSUB ||
    opr == OPR_NMADD || opr == OPR_NMSUB;
  
  /* Madd activation. For the target independent flags see
     function WN_Madd_Allowed() in wn_lower_util.cxx. */
  if (is_madd && MTYPE_is_integral(rtype))  return TRUE;
  if (is_madd && rtype == MTYPE_F8)  return FALSE;
  if (is_madd && rtype == MTYPE_F4)  return FALSE;

  return TRUE;
}



/* =============================================================
 *   Is_Power_Of_2 (val, mtype)
 *
 *   return TRUE if the val is a power of 2 
 *
 *   WARNING:  these routines must be kept in sync with cg's 
 *             exp_divrem
 * =============================================================
 */

#define IS_POWER_OF_2(val)      ((val != 0) && ((val & (val-1)) == 0))

static BOOL Is_Power_Of_2(INT64 val, TYPE_ID mtype)
{
  if (MTYPE_signed_type(mtype) && val < 0)
    val=        -val;

  if (mtype == MTYPE_U4)
    val &= 0xffffffffull;

  return IS_POWER_OF_2(val);
}

/* =============================================================
*   Target_Inlines_Divide (mtype, dividend)
*   return whether DIV will be translated into asm sequence 
*   Related to bug #52129
* 
*   TODO: To be checked on ARM if must return TRUE depending on mtype.
* =============================================================
*/
extern BOOL
Target_Inlines_Divide(TYPE_ID mtype, INT64 dividend){
  return FALSE;
}

/* =============================================================
 *   Can_Do_Fast_Divide (mtype, dividend)
 *
 *   return whether DIV will be translated into shifts 
 * =============================================================
 */
extern BOOL
Can_Do_Fast_Divide (TYPE_ID mtype, INT64 dividend)
{
#ifdef Is_True_On
  /* In debug mode, allow skipping of this. */
  if (getenv("NO_FAST_DIV")) return FALSE;
#endif

  if (Is_Power_Of_2(dividend, mtype)) {
    if (!Fast_Div_For_Size) return TRUE;

    /* For this target the following scheme is reasonable for code size:
       - division by unsigned power of two is always ok
       - division of type MTYPE_I4 with abs(dividend) <= 2 is ok
       - division of type MTYPE_I8 with abs(dividend) <= 1 is ok
    */
    else if (MTYPE_is_unsigned(mtype)) {
      return TRUE;
    } else if (mtype == MTYPE_I4) {
      if (dividend >= -2 && dividend <= 2) {
	return TRUE;
      } else {
	return FALSE;
      }
    } else if (mtype == MTYPE_I8) {
      if (dividend >= -1 && dividend <= 1) {
	return TRUE;
      } else {
	return FALSE;
      }
    }
  }
  return FALSE;
}

/* =============================================================
*  Target_Inlines_Remainder (mtype, dividend)
*  return whether REM will be translated into asm sequence 
*  Related to bug #65153
* 
*   TODO: To be checked on ARM if must return TRUE depending on mtype.
* =============================================================
*/
extern BOOL
Target_Inlines_Remainder(TYPE_ID mtype, INT64 dividend)
{
  return FALSE;
}

/* =============================================================
 *   Can_Do_Fast_Remainder (mtype, dividend)
 *
 *   return whether REM or MOD will be translated into shifts 
 * =============================================================
 */
extern BOOL
Can_Do_Fast_Remainder (TYPE_ID mtype, INT64 dividend)
{
#ifdef Is_True_On
  /* In debug mode, allow skipping of this. */
  if (getenv("NO_FAST_DIV")) return FALSE;
#endif

  if (Is_Power_Of_2(dividend, mtype)) {
    if (!Fast_Rem_For_Size) return TRUE;
    /* For this target the following scheme is reasonable for code size:
       - remainder by unsigned power of two is always ok
       - remainder of type MTYPE_I4 with abs(dividend) <= 1 is ok
       - remainder of type MTYPE_I8 with abs(dividend) <= 1 is ok
    */
    else if (MTYPE_is_unsigned(mtype)) {
      return TRUE;
    } else if (mtype == MTYPE_I4) {
      if (dividend >= -1 && dividend <= 1) {
	return TRUE;
      } else {
	return FALSE;
      }
    } else if (mtype == MTYPE_I8) {
      if (dividend >= -1 && dividend <= 1) {
	return TRUE;
      } else {
	return FALSE;
      }
    }
    
  }
  return FALSE;
}

/* =============================================================
 *   Can_Do_Select (mtype)
 *
 *   return whether select can be translated.
 *   Case where we don't want select translated if for instance on
 *   machines with no predication nor pertial predication.
 * =============================================================
 */
extern BOOL
Can_Do_Select (TYPE_ID mtype)
{
  /* For stxp70, we don't generate select if Enable_Conditional_Op is
     FALSE. */
  if (!Enable_Conditional_Op) {
    return FALSE;
  }
  return TRUE;
}


/* =============================================================
 *   Can_Do_Fast_Multiply (mtype, val)
 *
 *   return whether MPY will be translated into shifts and adds
 *   NOTE:  this routine must stay in sync with wn_lower lowering.
 *   Currently we handle the cases:
 *   - 0, 1,  -1
 *   - abs val is power of 2
 *   - abs val is power of two +1
 *   - abs val is power of two -1
 * =============================================================
 */
extern BOOL
Can_Do_Fast_Multiply (TYPE_ID mtype, INT64 val)
{
  INT bits;
  INT64 sval;

#ifdef Is_True_On
  /* In debug mod, allow skipping of this. */
  if (getenv("NO_FAST_MUL")) return FALSE;
#endif

  /* First form sign extended version of value on 64 bits. */
  bits = MTYPE_bit_size(mtype);
  sval = bits < 64 ? ((INT64)(val)<<(64-bits))>>(64-bits): val;
  
  if (sval == 0 || 
      sval == 1 ||
      sval == -1) {
    return TRUE;
  }

  if ((sval > 0 || sval == (1LL<<(MTYPE_bit_size(mtype)-1))) && 
      Is_Power_Of_2(sval, MTYPE_U8)) {
    return TRUE;
  }

  if (!Fast_Mult_For_Size && -sval > 0 && Is_Power_Of_2(-sval, MTYPE_U8)) {
    return TRUE;
  }
  
  if (!Fast_Mult_For_Size && sval-1 > 0 && Is_Power_Of_2(sval-1, MTYPE_U8)) {
    return TRUE;
  }

  if (!Fast_Mult_For_Size && -sval-1 > 0 && Is_Power_Of_2(-sval-1, MTYPE_U8)) {
    return TRUE;
  }

  if (!Fast_Mult_For_Size && sval+1 > 0 && Is_Power_Of_2(sval+1, MTYPE_U8)) {
    return TRUE;
  }

  if (!Fast_Mult_For_Size && -sval+1 > 0 && Is_Power_Of_2(-sval+1, MTYPE_U8)) {
    return TRUE;
  }
  
  return FALSE;
}


/* =============================================================
 *   Copy_Quantum_Ratio ()
 * =============================================================
 */
INT Copy_Quantum_Ratio(void)
{
  INT32  ratio;

  ratio = 4;
  return ratio;    
}

/* =============================================================
 *   Is_Signed_Bits(val, bits)
 *
 *   Does <val> fit in a signed word of length <bits>?
 * =============================================================
 */
inline BOOL Is_Signed_Bits(INT64 val, INT bits)
{
  INT64 hibit = 1LL << (bits - 1);
  return val >= -hibit && val <= (hibit - 1);
}


/* =============================================================
 *   Is_Unsigned_Bits(UINT64 val, INT bits)
 *
 *   Does <val> fit in a unsigned word of length <bits>?
 * =============================================================
 */
inline BOOL Is_Unsigned_Bits(UINT64 val, INT bits)
{
  return val < (1ULL << bits);
}


/* =============================================================
 *   Can_Be_Immediate(opr, val, dtype, whichkid, stid_st)
 *
 *   Indicate if the specified operation can have an immediate 
 *   operand.
 * =============================================================
 */
BOOL Can_Be_Immediate(OPERATOR opr,
		      INT64 val,
		      TYPE_ID dtype,
		      INT whichkid,
		      ST *stid_st)
{
  switch (opr) {
  case OPR_AGOTO:	// leave a constant condition here alone
  case OPR_LOOP_INFO:	// leave the constant trip-count alone
    return TRUE;
    
  case OPR_EQ:
  case OPR_NE:
  case OPR_GE:
  case OPR_GT:
  case OPR_LE:
  case OPR_LT:
  case OPR_MAX:		// treat as OPR_LT without regard to which kid
  case OPR_MIN:		// treat as OPR_LT without regard to which kid
    // an 9-bit literal is sign-extended and then its value is
    // taken in the context of the data size and type.
    if (!MTYPE_is_size_double(dtype)) val = (INT32)val;

    switch (opr) {
    case OPR_LE:
    case OPR_GT:
      whichkid = !whichkid;
      /*FALLTHROUGH*/
    case OPR_LT:
    case OPR_GE:
      if (whichkid == 1) val = val - 1;
      break;
    }

    return Is_Signed_Bits(val, 9);

  case OPR_ASHR:
  case OPR_LSHR:
  case OPR_MLOAD:
  case OPR_SHL:
    // can handle any constant as long as it's the second kid
    return whichkid == 1;

  case OPR_MSTORE:
    // can handle any constant as long as it's the third kid
    return whichkid == 2;

  case OPR_BAND:
  case OPR_BIOR:
  case OPR_BXOR:
    // can the value fit in signed 9 bits?
    return Is_Signed_Bits(val, 9);

  case OPR_SUB:
    // If the constant is kid #0 (left operand) then we can use
    // the subtract instruction which takes an immediate 8-bit first/left
    // operand. If the constant is kid #1 (right operand) then
    // we can negate the constant and use an add.
    if (whichkid == 0) return Is_Signed_Bits(val, 9);
    return Is_Signed_Bits(-val, 9);

  case OPR_ADD:
    // can the value fit in signed 9 bits?
    return Is_Signed_Bits(val, 9);

  case OPR_DIV:
    // can the second kid be handled with shifts?
    return whichkid == 1 && Can_Do_Fast_Divide(dtype, val);

  case OPR_REM:
  case OPR_MOD:
    // can the second kid be handled with shifts?
    return whichkid == 1 && Can_Do_Fast_Remainder(dtype, val);

  case OPR_DIVREM:
    // can the second kid be handled with shifts?
    return    whichkid == 1 
	   && Can_Do_Fast_Remainder(dtype, val)
	   && Can_Do_Fast_Divide(dtype, val);

  case OPR_MPY:
    // can the value be handled with shifts?
    return Can_Do_Fast_Multiply(dtype, val);
    
  case OPR_SELECT:
    // On ARM, we can use conditional moves.
    return whichkid > 0 && Is_Signed_Bits(val, 9);

  case OPR_CALL:
  case OPR_ICALL:
  case OPR_INTRINSIC_CALL:
  case OPR_PARM:
    // calls end up storing their constant parameters to dedicated
    // registers, which can be quicker if they're left in place as
    // ldimm's.
    return TRUE;
    
  case OPR_STID:
    // is this in a store to a register, which usually
    // means in preparation for a call, or return value, so just 
    // let us generate the stid/load-immediate in place if it fits
    return ST_class(stid_st) == CLASS_PREG;

 case OPR_LDA:
   // 
   // Arthur: risking that this will perturb WOPT
   //
   if (INT32_MIN <= val && val <= INT32_MAX) return TRUE;

  } /* switch */

  return FALSE;
}

/* ====================================================================
 *   Target_Has_Immediate_Operand (parent, expr)
 * ====================================================================
 */
BOOL
Target_Has_Immediate_Operand (
  WN *parent,
  WN *expr
)
{
  if (WN_operator(parent) == OPR_INTRINSIC_CALL
	&& (((INTRINSIC) WN_intrinsic (parent) == INTRN_FETCH_AND_ADD_I4)
	 || ((INTRINSIC) WN_intrinsic (parent) == INTRN_FETCH_AND_ADD_I8))) {
    // can optimize for some constants
    return TRUE;
  }

  // Operators that can have immediate as first or second operands
  // handled in exp_targ.cxx
  if (WN_operator(parent) == OPR_ADD ||
      WN_operator(parent) == OPR_SUB ||
      WN_operator(parent) == OPR_MIN ||
      WN_operator(parent) == OPR_MAX ||
      WN_operator(parent) == OPR_BAND ||
      WN_operator(parent) == OPR_BIOR ||
      WN_operator(parent) == OPR_BXOR ||
      WN_operator(parent) == OPR_EQ ||
      WN_operator(parent) == OPR_NE ||
      WN_operator(parent) == OPR_LE ||
      WN_operator(parent) == OPR_LT ||
      WN_operator(parent) == OPR_GE ||
      WN_operator(parent) == OPR_GT) {
    return TRUE;
  }

  // default to false, which really means "don't know"
  return FALSE;
}

/* ===============================================================
 *   OPCODE_To_INTRINSIC
 *
 *   TODO: generate gcc intrinsics for some operators, eg. DIV,
 *         depending on the flag OPT_generate_gcc_intrinsics.
 * ===============================================================
 */
INTRINSIC
OPCODE_To_INTRINSIC (
  OPCODE opcode
)
{
  OPERATOR opr   = OPCODE_operator (opcode);
  TYPE_ID  rtype = OPCODE_rtype (opcode);
  TYPE_ID  desc  = OPCODE_desc  (opcode);
  INTRINSIC id = INTRINSIC_INVALID;

  switch (opr) {

    case OPR_ADD:
      switch (rtype) {
      case MTYPE_I8: id = INTRN_ADDL; break;
      case MTYPE_U8: id = INTRN_ADDUL; break;
      case MTYPE_F4: id = INTRN_ADDS; break;
      case MTYPE_F8: id = INTRN_ADDD; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for ADD", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_SUB:
      switch (rtype) {
      case MTYPE_I8: id = INTRN_SUBL; break;
      case MTYPE_U8: id = INTRN_SUBUL; break;
      case MTYPE_F4: id = INTRN_SUBS; break;
      case MTYPE_F8: id = INTRN_SUBD; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for SUB", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_NEG:
      switch (rtype) {
      case MTYPE_I8: id = INTRN_NEGL; break;
      case MTYPE_U8: id = INTRN_NEGUL; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for NEG", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_SHL:
      switch (rtype) {
      case MTYPE_I8: 
      case MTYPE_U8: id = INTRN_SHLL; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for SHL", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_LSHR:
      switch (rtype) {
      case MTYPE_I8: 
      case MTYPE_U8: id = INTRN_SHRUL; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for LSHR", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_ASHR:
      switch (rtype) {
      case MTYPE_I8: 
      case MTYPE_U8: id = INTRN_SHRL; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for ASHR", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_DIV:
      switch (rtype) {
      case MTYPE_I8: id = INTRN_DIVL; break;
      case MTYPE_U8: id = INTRN_DIVUL; break;
      case MTYPE_F4: id = INTRN_DIVS; break;
      case MTYPE_F8: id = INTRN_DIVD; break;
      case MTYPE_U4: id = INTRN_DIVUW; break;
      case MTYPE_I4: id = INTRN_DIVW; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for DIV", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_MPY:
      switch (rtype) {
      case MTYPE_I8: id = INTRN_MULL; break;
      case MTYPE_U8: id = INTRN_MULUL; break;
      case MTYPE_F4: id = INTRN_MULS; break;
      case MTYPE_U4: id = INTRN_MULUW; break;
      case MTYPE_I4: id = INTRN_MULW; break;
      case MTYPE_F8: id = INTRN_MULD; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for MPY", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_MADD:
      switch (rtype) {
      case MTYPE_F4: id = INTRN_MADDS; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for MADD", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_MSUB:
      switch (rtype) {
      case MTYPE_F4: id = INTRN_MSUBS; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for MSUB", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_NMADD:
      switch (rtype) {
      case MTYPE_F4: id = INTRN_NMADDS; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for NMADD", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_NMSUB:
      switch (rtype) {
      case MTYPE_F4: id = INTRN_NMSUBS; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for NMSUB", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_MIN:
      switch (rtype) {
      case MTYPE_I8: id = INTRN_MINL; break;
      case MTYPE_U8: id = INTRN_MINUL; break;
      case MTYPE_F4: id = INTRN_MINS; break;
      case MTYPE_F8: id = INTRN_MIND; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for MIN", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_MAX:
      switch (rtype) {
      case MTYPE_I8: id = INTRN_MAXL; break;
      case MTYPE_U8: id = INTRN_MAXUL; break;
      case MTYPE_F4: id = INTRN_MAXS; break;
      case MTYPE_F8: id = INTRN_MAXD; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for MAX", 
			 MTYPE_name(rtype)));
      }
      break;

  case OPR_ABS:
    switch (rtype) {
    case MTYPE_I8: id = INTRN_ABSL; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for ABS", 
			 MTYPE_name(rtype)));
      }
      break;

    case OPR_EQ:
      switch (desc) {
      case MTYPE_I8: id = INTRN_EQL; break;
      case MTYPE_U8: id = INTRN_EQUL; break;
      case MTYPE_F4: id = INTRN_EQS; break;
      case MTYPE_F8: id = INTRN_EQD; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for EQ", 
			 MTYPE_name(desc)));
      }
      break;

    case OPR_NE:
      switch (desc) {
      case MTYPE_I8: id = INTRN_NEL; break;
      case MTYPE_U8: id = INTRN_NEUL; break;
      case MTYPE_F4: id = INTRN_NES; break;
      case MTYPE_F8: id = INTRN_NED; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for NE", 
			 MTYPE_name(desc)));
      }
      break;

    case OPR_LT:
      switch (desc) {
      case MTYPE_I8: id = INTRN_LTL; break;
      case MTYPE_U8: id = INTRN_LTUL; break;
      case MTYPE_F4: id = INTRN_LTS; break;
      case MTYPE_F8: id = INTRN_LTD; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for LT", 
			 MTYPE_name(desc)));
      }
      break;

    case OPR_LE:
      switch (desc) {
      case MTYPE_I8: id = INTRN_LEL; break;
      case MTYPE_U8: id = INTRN_LEUL; break;
      case MTYPE_F4: id = INTRN_LES; break;
      case MTYPE_F8: id = INTRN_LED; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for LE", 
			 MTYPE_name(desc)));
      }
      break;

    case OPR_GT:
      switch (desc) {
      case MTYPE_I8: id = INTRN_GTL; break;
      case MTYPE_U8: id = INTRN_GTUL; break;
      case MTYPE_F4: id = INTRN_GTS; break;
      case MTYPE_F8: id = INTRN_GTD; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for GT", 
			 MTYPE_name(desc)));
      }
      break;

    case OPR_GE:
      switch (desc) {
      case MTYPE_I8: id = INTRN_GEL; break;
      case MTYPE_U8: id = INTRN_GEUL; break;
      case MTYPE_F4: id = INTRN_GES; break;
      case MTYPE_F8: id = INTRN_GED; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: mtype %s for GE", 
			 MTYPE_name(desc)));
      }
      break;

    case OPR_CVT:
      // to/from float:
      if (rtype == MTYPE_F4) {
	switch (desc) {
	case MTYPE_I4: id = INTRN_WTOS; break;
	case MTYPE_U4: id = INTRN_UWTOS; break;
	case MTYPE_I8: id = INTRN_LTOS; break;
	case MTYPE_U8: id = INTRN_ULTOS; break;
	case MTYPE_F8: id = INTRN_DTOS; break;
	default:
	  FmtAssert(FALSE,("OPERATOR_To_Intrinsic: desc %s for F4CVT", 
			 MTYPE_name(desc)));
	}
      }
      else if (desc == MTYPE_F4) {
	switch (rtype) {
	case MTYPE_I4: id = INTRN_STOW; break;
	case MTYPE_U4: id = INTRN_STOUW; break;
	case MTYPE_I8: id = INTRN_STOL; break;
	case MTYPE_U8: id = INTRN_STOUL; break;
	case MTYPE_F8: id = INTRN_STOD; break;
	default:
	  FmtAssert(FALSE,("OPERATOR_To_Intrinsic: rtype %s for CVTF4", 
			 MTYPE_name(desc)));
	}
      }
      else if (rtype == MTYPE_F8) {
	switch (desc) {
	case MTYPE_I4: id = INTRN_WTOD; break;
	case MTYPE_U4: id = INTRN_UWTOD; break;
	case MTYPE_I8: id = INTRN_LTOD; break;
	case MTYPE_U8: id = INTRN_ULTOD; break;
	default:
	  FmtAssert(FALSE,("OPERATOR_To_Intrinsic: desc %s for F8CVT", 
			 MTYPE_name(desc)));
	}
      }
      else if (desc == MTYPE_F8) {
	switch (rtype) {
	case MTYPE_I4: id = INTRN_DTOW; break;
	case MTYPE_U4: id = INTRN_DTOUW; break;
	case MTYPE_I8: id = INTRN_DTOL; break;
	case MTYPE_U8: id = INTRN_DTOUL; break;
	default:
	  FmtAssert(FALSE,("OPERATOR_To_Intrinsic: rtype %s for CVTF8", 
			 MTYPE_name(desc)));
	}
      }

      break;

    case OPR_REM:
      switch (rtype) {
      case MTYPE_U4: id = INTRN_MODUW; break;
      case MTYPE_I4: id = INTRN_MODW; break;
      case MTYPE_U8: id = INTRN_MODUL; break;
      case MTYPE_I8: id = INTRN_MODL; break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: unknown REM opcode"));
      }
      break;

    case OPR_SQRT:
      Is_True(rtype == MTYPE_F4 || rtype == MTYPE_F8,
	      ("OPR_SQRT: unknown rtype type"));
      switch (rtype) {
      case MTYPE_F4:
	id = INTRN_SQRTS;
	break;
      case MTYPE_F8: 
	id = INTRN_SQRTD;
	break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: %s ??", 
                                            OPCODE_name(opcode)));
      }
      break;

    case OPR_RECIP:
      Is_True(rtype == MTYPE_F4,
	      ("OPR_RECIP: unknown rtype type %s", MTYPE_name(desc)));
      switch (rtype) {
      case MTYPE_F4:
	id = INTRN_RECIPS;
	break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: %s ??", 
                                            OPCODE_name(opcode)));
      }
      break;

    case OPR_RSQRT:
      Is_True(rtype == MTYPE_F4,
	      ("OPR_RSQRT: unknown rtype type %s", MTYPE_name(desc)));
      switch (rtype) {
      case MTYPE_F4:
	id = INTRN_RSQRTS;
	break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: %s ??", 
                                            OPCODE_name(opcode)));
      }
      break;


    case OPR_TRUNC:
      Is_True(desc == MTYPE_F4 || desc == MTYPE_F8,
	      ("OPR_TRUNC: unknown desc type"));
      switch (rtype) {
      case MTYPE_I4: 
	id = (desc == MTYPE_F4) ? INTRN_STOW : INTRN_DTOW;
	break;
      case MTYPE_U4: 
	id = (desc == MTYPE_F4) ? INTRN_STOUW : INTRN_DTOUW;
	break;
      case MTYPE_I8:
	id = (desc == MTYPE_F4) ? INTRN_STOL : INTRN_DTOL;
	break;
      case MTYPE_U8: 
	id = (desc == MTYPE_F4) ? INTRN_STOUL : INTRN_DTOUL;
	break;
      default:
	FmtAssert(FALSE,("OPERATOR_To_Intrinsic: %s ??", 
                                            OPCODE_name(opcode)));
      }
      break;

    default:
      FmtAssert(FALSE,("OPERATOR_To_Intrinsic: can't handle %s",
		                                  OPERATOR_name(opr)));
  }

  return id;
}


/* Utility procedure which does a comparison on two symbols */
static INT32 WN_Compare_Symbols(WN *t1, WN *t2)
{
   ST_IDX s1 = WN_st_idx(t1);
   ST_IDX s2 = WN_st_idx(t2);

   if (s1 < s2)
       return -1;
   else if (s1 > s2)
       return 1;
   else
       return 0;
}

// [HK] static function added to check for tree equality
// needed for x^2 opportunities check

static INT32 WN_Compare_Trees(WN *t1, WN *t2)
{
   INT32  i;
   INT32  rv;
   
   /* Trivial comparison, shouldn't ever happen for WHIRL nodes, but
    * will happen for the WOPT IR.
    */
   if (t1 == t2) return (0);

   /* First compare opcodes */
   if (WN_opcode(t1) < WN_opcode(t2))
     return (-1);
   else if (WN_opcode(t1) > WN_opcode(t2))
     return (1);
   
   /* Opcodes are the same, switch on operator class */
   switch (WN_operator(t1)) {
    case OPR_INTCONST:
      if (WN_const_val(t1) < WN_const_val(t2)) return(-1);
      if (WN_const_val(t1) > WN_const_val(t2)) return(1);
      return (0);

    case OPR_CVTL:
      if (WN_cvtl_bits(t1) < WN_cvtl_bits(t2)) return (-1);
      if (WN_cvtl_bits(t1) > WN_cvtl_bits(t2)) return (1);
      return (WN_Compare_Trees(WN_kid0(t1),
					  WN_kid0(t2)));
      

    case OPR_EXTRACT_BITS:
      if (WN_bit_offset(t1) < WN_bit_offset(t2)) return (-1);
      if (WN_bit_offset(t1) > WN_bit_offset(t2)) return (1);
      if (WN_bit_size(t1) < WN_bit_size(t2)) return (-1);
      if (WN_bit_size(t1) > WN_bit_size(t2)) return (1);
      return (WN_Compare_Trees(WN_kid0(t1),
					  WN_kid0(t2)));


    case OPR_COMPOSE_BITS:
      if (WN_bit_offset(t1) < WN_bit_offset(t2)) return (-1);
      if (WN_bit_offset(t1) > WN_bit_offset(t2)) return (1);
      if (WN_bit_size(t1) < WN_bit_size(t2)) return (-1);
      if (WN_bit_size(t1) > WN_bit_size(t2)) return (1);
      rv = WN_Compare_Trees(WN_kid0(t1), WN_kid0(t2));
      if (rv == 0) {
	rv = WN_Compare_Trees(WN_kid1(t1), WN_kid1(t2));
      }
      return rv;


    case OPR_CONST:
      return WN_Compare_Symbols(t1,t2);
      
    case OPR_ILOAD:
      if (WN_load_offset(t1) < WN_load_offset(t2)) return(-1);
      if (WN_load_offset(t1) > WN_load_offset(t2)) return(1);
      if (WN_desc(t1) == MTYPE_BS || WN_desc(t2) == MTYPE_BS) {
	if (WN_field_id(t1) < WN_field_id(t2)) return(-1);
	if (WN_field_id(t1) > WN_field_id(t2)) return(1);
      }
      return (WN_Compare_Trees(WN_kid0(t1),
					  WN_kid0(t2)));

    case OPR_ILDBITS:
      if (WN_load_offset(t1) < WN_load_offset(t2)) return(-1);
      if (WN_load_offset(t1) > WN_load_offset(t2)) return(1);
      if (WN_bit_offset(t1) < WN_bit_offset(t2)) return(-1);
      if (WN_bit_offset(t1) > WN_bit_offset(t2)) return(1);
      return (WN_Compare_Trees(WN_kid0(t1),
					  WN_kid0(t2)));

    case OPR_MLOAD:  /* Same procedure as ILOADX, though the children */
    case OPR_ILOADX:  /* have very different meanings */    
      if (WN_load_offset(t1) < WN_load_offset(t2)) return(-1);
      if (WN_load_offset(t1) > WN_load_offset(t2)) return(1);
      rv = WN_Compare_Trees(WN_kid0(t1),WN_kid0(t2));
      if (rv != 0) return (rv);
      return (WN_Compare_Trees(WN_kid1(t1),
					  WN_kid1(t2)));

    case OPR_LDID:
      if (WN_load_offset(t1) < WN_load_offset(t2)) return(-1);
      if (WN_load_offset(t1) > WN_load_offset(t2)) return(1);
      if (WN_desc(t1) == MTYPE_BS || WN_desc(t2) == MTYPE_BS) {
	if (WN_field_id(t1) < WN_field_id(t2)) return(-1);
	if (WN_field_id(t1) > WN_field_id(t2)) return(1);
      }
      return WN_Compare_Symbols(t1,t2);

    case OPR_LDBITS:
      if (WN_load_offset(t1) < WN_load_offset(t2)) return(-1);
      if (WN_load_offset(t1) > WN_load_offset(t2)) return(1);
      if (WN_bit_offset(t1) < WN_bit_offset(t2)) return(-1);
      if (WN_bit_offset(t1) > WN_bit_offset(t2)) return(1);
      return WN_Compare_Symbols(t1,t2);
      
    case OPR_IDNAME:
      if (WN_idname_offset(t1) < WN_idname_offset(t2)) return(-1);
      if (WN_idname_offset(t1) > WN_idname_offset(t2)) return(1);
      return WN_Compare_Symbols(t1,t2);

    case OPR_LDA:
      if (WN_lda_offset(t1) < WN_lda_offset(t2)) return(-1);
      if (WN_lda_offset(t1) > WN_lda_offset(t2)) return(1);
      return WN_Compare_Symbols(t1,t2);

#ifdef TARG_ST
      /* [CG]: Handle LDA_LABEL comparison. */
   case OPR_LDA_LABEL:
     return WN_label_number(t1) - WN_label_number(t2);
#endif
    case OPR_ARRAY:
      if (WN_num_dim(t1) < WN_num_dim(t2)) return (-1);
      if (WN_num_dim(t1) > WN_num_dim(t2)) return (1);
      if (WN_element_size(t1) < WN_element_size(t2)) return (-1);
      if (WN_element_size(t1) > WN_element_size(t2)) return (1);
      /* Compare bases */
      rv = WN_Compare_Trees(WN_array_base(t1),
				       WN_array_base(t2));
      if (rv != 0) return (rv);
      
      /* Compare array_index and array_dim */
      for (i=0; i < WN_num_dim(t1); i++) {
	 rv = WN_Compare_Trees(WN_array_index(t1,i),
					  WN_array_index(t2,i));
	 if (rv != 0) return (rv);
	 rv = WN_Compare_Trees(WN_array_dim(t1,i),
					  WN_array_dim(t2,i));
	 if (rv != 0) return (rv);
      }
      /* everything compares */
      return (0);

    case OPR_INTRINSIC_OP:
      if (WN_intrinsic(t1) < WN_intrinsic(t2)) return (-1);
      if (WN_intrinsic(t1) > WN_intrinsic(t2)) return (1);
      if (WN_kid_count(t1) < WN_kid_count(t2)) return (-1);
      if (WN_kid_count(t1) > WN_kid_count(t2)) return (1);
      
      for (i=0; i<WN_kid_count(t1); i++) {
	rv = WN_Compare_Trees(WN_kid(t1,i),
					 WN_kid(t2,i));
	if (rv != 0) return (rv);
      }
      return (0);

    case OPR_COMMA:
    case OPR_RCOMMA:
    case OPR_CSELECT:

      return ((INTPS)t1 - (INTPS)t2);

#ifdef TARG_ST
   case OPR_SUBPART:
     if (WN_subpart_index(t1) < WN_subpart_index(t2)) return (-1);
     if (WN_subpart_index(t1) > WN_subpart_index(t2)) return (1);
     return (WN_Compare_Trees(WN_kid0(t1), WN_kid0(t2)));
#endif

    default:
       if (OPCODE_is_expression(WN_opcode(t1))) {
	  for (i=0; i<WN_kid_count(t1); i++) {
	     rv = WN_Compare_Trees(WN_kid(t1,i),
					      WN_kid(t2,i));
	     if (rv != 0) return (rv);
	  }
	  return (0);
       } else {
	  /* Non-expression opcode. Return arbitrary */
	  return ((INTPS)t1 - (INTPS)t2);
       }
   }
}


/* ===============================================================
 *   WN_To_INTRINSIC
 *
 *   Tree based selection of intrinsics.
 *   Pass opcode and WN kids.
 *   Return INTRINSIC and modified kids.
 *   Call OPCODE_To_INTRINSIC when no optimization
 *   is possible on kids.
 * ===============================================================
 */
INTRINSIC
WN_To_INTRINSIC (
 OPCODE opcode, WN *kids[]
)
{
  OPERATOR opr   = OPCODE_operator (opcode);
  TYPE_ID  rtype = OPCODE_rtype (opcode);
  TYPE_ID  desc  = OPCODE_desc  (opcode);
  INTRINSIC id = INTRINSIC_INVALID;

  // [CG]: select 32x32->64 multiplies
  // [HK] added selection for INTCONSTI8 not overflowing 32 bits
  if (opcode == OPC_I8MPY || opcode == OPC_U8MPY) {
    if (WN_opcode(kids[0]) == OPC_I8I4CVT &&
	WN_opcode(kids[1]) == OPC_I8I4CVT) {
      id = INTRN_MULN;
      kids[0] = WN_kid0(kids[0]);
      kids[1] = WN_kid0(kids[1]);
    } else if (WN_opcode(kids[0]) == OPC_U8U4CVT &&
	       WN_opcode(kids[1]) == OPC_U8U4CVT) {
      id = INTRN_MULUN;
      kids[0] = WN_kid0(kids[0]);
      kids[1] = WN_kid0(kids[1]);
    } else if (WN_opcode(kids[0]) == OPC_I8I4CVT &&
	WN_opcode(kids[1]) == OPC_I8INTCONST) {
	if ( WN_const_val(kids[1]) == (INT64)((INT32)WN_const_val(kids[1]))){
	    id = INTRN_MULN;
	    kids[0] = WN_kid0(kids[0]);
	    kids[1] = WN_CreateIntconst(OPR_INTCONST, MTYPE_I4, MTYPE_V, WN_const_val(kids[1]));
	}
    } else if (WN_opcode(kids[1]) == OPC_I8I4CVT &&
	WN_opcode(kids[0]) == OPC_I8INTCONST) {
	if ( WN_const_val(kids[0]) == (INT64)((INT32)WN_const_val(kids[0]))){
	    id = INTRN_MULN;
	    kids[0] = WN_kid0(kids[1]);
	    kids[1] = WN_CreateIntconst(OPR_INTCONST, MTYPE_I4, MTYPE_V, WN_const_val(kids[0]));
	} 
    } else if (WN_opcode(kids[0]) == OPC_U8U4CVT &&
	WN_opcode(kids[1]) == OPC_U8INTCONST) {
	if ( WN_const_val(kids[1]) == (UINT64)((UINT32)WN_const_val(kids[1]))){
	    id = INTRN_MULUN;
	    kids[0] = WN_kid0(kids[0]);
	    kids[1] = WN_CreateIntconst(OPR_INTCONST, MTYPE_U4, MTYPE_V, WN_const_val(kids[1]));
	}
    } else if (WN_opcode(kids[1]) == OPC_U8U4CVT &&
	WN_opcode(kids[0]) == OPC_U8INTCONST) {
	if ( WN_const_val(kids[0]) == (UINT64)((UINT32)WN_const_val(kids[0]))){
	    id = INTRN_MULUN;
	    kids[0] = WN_kid0(kids[1]);
	    kids[1] = WN_CreateIntconst(OPR_INTCONST, MTYPE_U4, MTYPE_V, WN_const_val(kids[0]));
	}
    }

  }

#if 0
  // TODO: add sqaures in STxP70 runtime
  // [HK]: select x^2 for F4 type
  if (opcode == OPC_F4MPY)
      if (WN_Compare_Trees(kids[0],kids[1]) == 0) {
	  id = INTRN_SQUARES;
      }
#endif

  if (id == INTRINSIC_INVALID) {
    return OPCODE_To_INTRINSIC(opcode);
  }
  return id;
}

BOOL
Pass_Low_First (TYPE_ID type)
{
  /* ST200 always passes low order word of multi-register
     parameters/results in the lowest numbered register.
  */
  return TRUE;
}

/* FdF 20081126: Check if an MTYPE is allocatable, i.e a temporary
   register can be allocated to hold a value of type t. */
BOOL
CGTARG_Can_Allocate_Reg_For_Mtype(TYPE_ID t) {
  // On ARM, all MTYPEs are allocatables.
  return TRUE;
}
