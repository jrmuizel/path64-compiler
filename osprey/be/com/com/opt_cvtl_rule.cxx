/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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


//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_cvtl_rule.cxx
//
// Description: Conversion rules
//
// ====================================================================
// ====================================================================

#include "opt_cvtl_rule.h"
#include "wn.h"

#define  nop {NOT_AT_ALL,OPCODE_LAST+1}

// from A4:
#define I4A4 {NEED_CVT, OPC_I4A4CVT}
#define U4A4 {NEED_CVT, OPC_U4A4CVT}
#define I8A4 {NEED_CVT, OPC_I8A4CVT}
#define U8A4 {NEED_CVT, OPC_U8A4CVT}

// to A4:
#define A4I8 {NEED_CVT, OPC_A4I8CVT}
#define A4I4 {NEED_CVT, OPC_A4I4CVT}
#define A4U4 {NEED_CVT, OPC_A4U4CVT}
#define A4U8 {NEED_CVT, OPC_A4U8CVT}
#define A4A8 {NEED_CVT, OPC_A4A8CVT}

// from A8:
#define I4A8 {NEED_CVT, OPC_I4A8CVT}
#define U4A8 {NEED_CVT, OPC_U4A8CVT}
#define I8A8 {NEED_CVT, OPC_I8A8CVT}
#define U8A8 {NEED_CVT, OPC_U8A8CVT}

// to A8:
#define A8I4 {NEED_CVT, OPC_A8I4CVT}
#define A8U4 {NEED_CVT, OPC_A8U4CVT}
#define A8I8 {NEED_CVT, OPC_A8I8CVT}
#define A8U8 {NEED_CVT, OPC_A8U8CVT}
#define A8A4 {NEED_CVT, OPC_A8A4CVT}

// to I5:
// Arthur: treat {I,U}5 same way as {I,U}8
#define I5I4 {NEED_CVT, OPC_I5I4CVT}
#define I5A4 {NEED_CVT, OPC_I5A4CVT}
#define I5A8 {NEED_CVT, OPC_I5A8CVT}

// to U5:
#define U5U4 {NEED_CVT, OPC_U5U4CVT}
#define U5U8 {NEED_CVT, OPC_U5U8CVT}
#define U5A4 {NEED_CVT, OPC_U5A4CVT}
#define U5A8 {NEED_CVT, OPC_U5A8CVT}


// from I5:
// The I1I5 and I2I5 should never be used.
// This conversion should always go through I4
// There is no way at this level to check for this ;-(
// I will use a nop in the table bellow.
/*
#define I1I5 {NEED_CVT, OPC_I1I5CVT}
#define I2I5 {NEED_CVT, OPC_I2I5CVT}
*/
#define I4I5 {NEED_CVT, OPC_I4I5CVT}
#define I8I5 {NEED_CVT, OPC_I8I5CVT}
#define U8I5 {NEED_CVT, OPC_U8I5CVT}
#define A4I5 {NEED_CVT, OPC_A4I5CVT}
#define A8I5 {NEED_CVT, OPC_A8I5CVT}

// from U5:
// The U1U5 and U2U5 should never be used.
// This conversion should always go through U4
// There is no way at this level to check for this ;-(
// I will use a nop in the table bellow.
/*
#define U1U5 {NEED_CVT, OPC_U1U5CVT}
#define U2U5 {NEED_CVT, OPC_U2U5CVT}
*/
#define U4U5 {NEED_CVT, OPC_U4U5CVT}
#define U8U5 {NEED_CVT, OPC_U8U5CVT}
#define A4U5 {NEED_CVT, OPC_A4U5CVT}
#define A8U5 {NEED_CVT, OPC_A8U5CVT}

#define I8I4 {NEED_CVT,  OPC_I8I4CVT}
#define U8I4 {NEED_CVT,  OPC_U8I4CVT}
#define I4I8 {NEED_CVT,  OPC_I4I8CVT}
#define U4I8 {NEED_CVT,  OPC_U4I8CVT}
#define I8U4 {NEED_CVT,  OPC_I8U4CVT}
#define U8U4 {NEED_CVT,  OPC_U8U4CVT}
#define I4U8 {NEED_CVT,  OPC_I4U8CVT}
#define U4U8 {NEED_CVT,  OPC_U4U8CVT}

#define I4I2 {NEED_CVTL, OPC_I4CVTL}
#define I4I1 {NEED_CVTL, OPC_I4CVTL}
#define U4U2 {NEED_CVTL, OPC_U4CVTL}
#define U4U1 {NEED_CVTL, OPC_U4CVTL}
#define I4U4 {NEED_CVTL, OPC_U4CVTL}
#define U4I4 {NEED_CVTL, OPC_I4CVTL}
#define I8I2 {NEED_CVTL, OPC_I8CVTL}
#define I8I1 {NEED_CVTL, OPC_I8CVTL}
#define U8U2 {NEED_CVTL, OPC_U8CVTL}
#define U8U1 {NEED_CVTL, OPC_U8CVTL}

#define I1I4 {NEED_CVTL, OPC_I4CVTL}
#define I2I4 {NEED_CVTL, OPC_I4CVTL}
#define I1I8 {NEED_CVTL, OPC_I8CVTL}
#define I2I8 {NEED_CVTL, OPC_I8CVTL}
#define U1U4 {NEED_CVTL, OPC_U4CVTL}
#define U2U4 {NEED_CVTL, OPC_U4CVTL}
#define U1U8 {NEED_CVTL, OPC_U8CVTL}
#define U2U8 {NEED_CVTL, OPC_U8CVTL}

#define I4B  {NEED_CVT, OPC_I4BCVT}
#define I8B  {NEED_CVT, OPC_I8BCVT}
#define U4B  {NEED_CVT, OPC_U4BCVT}
#define U8B  {NEED_CVT, OPC_U8BCVT}

// This table reflexs the table 3 in whirl definition document.  There
// is minor differences, the convert in the original table is removed
// if it does not require an instruction in MIPS architecture.  all
// macros for conversion is ToFrom sequence.
static struct cvt_rule {
  char   _cvt_kind;
  mINT32 _cvt_opcode;
} cvt_rule[MTYPE_A8+1][MTYPE_A8+1] = {
  //UNK, B,   I1,  I2,  I4,  I8,  U1,  U2,  U4,  U8,  I5,  U5,  A4,   A8   From
  { nop, nop, nop, nop, nop, nop, nop, nop, nop, nop, nop, nop, nop,  nop },//to UNK
  { nop, nop, nop, nop, nop, nop, nop, nop, nop, nop, nop, nop, nop,  nop },//to B
  { nop, nop, nop, nop,I1I4,I1I8, nop, nop,I1I4,I1I8, nop, nop, nop, nop},//to I1
  { nop, nop, nop, nop,I2I4,I2I8, nop, nop,I2I4,I2I8, nop, nop, nop, nop},//to I2
  { nop, I4B,I4I1,I4I2, nop,I4I8, nop, nop, nop,I4U8, I4I5, I4I5, I4A4, I4A8},//to I4
#ifdef TARG_MIPS
  { nop, nop,I8I1,I8I2, nop, nop, nop, nop,I8U4, nop, nop,  nop, nop, nop },//to I8
#elif defined(TARG_IA32)
  { nop, nop,I8I1,I8I2,I8I4, nop, nop, nop,I8U4, nop, nop,  nop, nop, nop },//to I8
#elif defined(TARG_ST)
  { nop, I8B, nop, nop, I8I4, nop, nop, nop, I8I4, nop, I8I5, I8I5, I8A4, I8A8},//to I8
#else
  { nop, I8B,I8I1,I8I2,I8I4, nop, nop, nop, nop, nop, I8I5, nop, I8A4, I8A8},//to I8
#endif
#if defined(TARG_ST)
  // Arthur: depends whether we are doing Only_32_Bit_Ops or not
  { nop, nop, nop, nop,U1U4, nop, nop, nop, U1U4, nop, nop, nop,  nop, nop},//to U1
  { nop, nop, nop, nop,U2U4, nop, nop, nop, U2U4, nop, nop, nop,  nop, nop},//to U2
  { nop, U4B, nop, nop, nop,U4U8, nop, nop, nop, U4U8, U4U5, U4U5,  U4A4, U4A8},//to U4
#else
  { nop, nop, nop, nop,U1U4,U1U8, nop, nop, nop, U1U8, nop, nop,  nop, nop},//to U1
  { nop, nop, nop, nop,U2U4,U2U8, nop, nop,U2U4,U2U8, nop, nop,  nop, nop},//to U2
  { nop, U4B, nop, nop, nop,U4I8,U4U1,U4U2, nop,U4U8, nop, U4U5,  U4A4, U4A8},//to U4
#endif

#ifdef TARG_MIPS
  { nop, nop, nop, nop, nop, nop,U8U1,U8U2,U8U4, nop, nop, nop, nop, nop}, //to U8
#elif defined(TARG_IA32)
  { nop, nop, nop, nop,U8I4, nop,U8U1,U8U2,U8U4, nop, nop, nop, nop, nop}, //to U8
#elif defined(TARG_ST)
  { nop, nop, nop, nop,U8I4, nop, nop, nop,U8U4, nop, U8I5, U8U5, U8A4, U8A8}, //to U8
#else
  { nop, U8B, nop, nop, U8I4, nop, U8U1, U8U2, nop, nop, nop, U8U5, U8A4, U8A8}, //to U8
#endif

  { nop, nop, nop, nop, I5I4, nop, nop, nop, I5I4, nop, nop, nop, I5A4, I5A8},//to I5
  { nop, nop, nop, nop, U5U4, nop, nop, nop, U5U4, nop, nop, nop, U5A4, U5A8}, //to U5
  { nop, nop, nop, nop, A4I4, A4I8,nop,nop, A4U4, A4U8, A4I5, A4U5, nop, A4A8},//to A4
  { nop, nop, nop, nop,A8I4, A8I8,nop,nop, A8U4, A8U8, A8I5, A8U5, A8A4, nop } //to A8
};

// return NOT_AT_ALL, NEED_CVT, or NEED_CVTL.  The reference parameter
// opc returns the opcode for the conversion, if it is either NEED_CVT
// or NEED_CVTL.
INT Need_type_conversion(TYPE_ID from_ty, TYPE_ID to_ty, OPCODE *opc)
{
  if (!(MTYPE_is_integral(from_ty) && MTYPE_is_integral(to_ty))) {
    if (from_ty == to_ty) return NOT_AT_ALL;
    if (opc != NULL) 
      *opc = OPCODE_make_op(OPR_CVT, to_ty, from_ty);
    return NEED_CVT;
  }

  if (opc != NULL)
    *opc = (OPCODE)cvt_rule[to_ty][from_ty]._cvt_opcode;

  return (INT)cvt_rule[to_ty][from_ty]._cvt_kind;
}

// ==========================================================
// Load/Iload convertion rule for desc type if size 4
//            hi32 sign extended
// U8U4ILOAD        0
// I8I4ILOAD        1
// U4U4ILOAD        1
// I4I4ILOAD        1
// need conversion between {U8U4ILOAD} and {I8I4ILOAD,U4U4ILOAD,I4I4ILOAD}
// ==========================================================
// static struct load_convert_rule_4 {
//   char   _cvt_kind;
//   mINT32 _cvt_opcode;
// } load_convert_rule_4[2][2] = {
//   // 0      1  From
//   {  nop, U8U4},//to 0
//   { U4U8,  nop} //to 1
// };
//
// ==========================================================
// Load/Iload convertion rule for desc type if size < 4
//            lo32 sign extended
// U8U{1,2}ILOAD        0
// I8I{1,2}ILOAD        1
// U4U{1,2}ILOAD        0
// I4I{1,2}ILOAD        1
// need conversion between {U{8,4}U{1,2}ILOAD} and {I{8,4}I{1,2}ILOAD}
// ==========================================================
// static struct load_convert_rule_1_2 {
//   char   _cvt_kind;
//   mINT32 _cvt_opcode;
// } load_convert_rule_1_2[2][2] = {
//   // 0      1 From
//   { nop, U4U1 },// to 0
//   {I4I1,  nop } // to 1
// };
//
// ===================================================================
// Implementing Load/Iload conversion rule as scratched above.
// return NOT_AT_ALL, NEED_CVT, or NEED_CVTL.  The reference parameter
// opc returns the opcode for the conversion, if it is either NEED_CVT
// or NEED_CVTL.
// ===================================================================

#ifdef TARG_ST

//   Arthur:
//        The IA-64 routine seems to generate U8 conversions whatever.
//        I tried to generate a more precise thing. Because it's C++
//        the two routines are overloaded for the ST targets for now.
//        Later the old one may disappear.
//
//   We have two loads: <from_ty><xxx>LOAD1 and <to_ty><dsc_ty>LOAD2.
//   <xxx> and <dsc_ty> are of the same size.
//
//   Source load is being replaced with the target load -- call this
//   routine checks whether there is a conversion between <from_ty> 
//   and <to_ty>.
#if 0
INT 
Need_load_type_conversion(
  TYPE_ID to_ty,           // rtype of source load
  TYPE_ID from_ty,         // rtype of target load
  TYPE_ID dsc_ty,          // desc of the target load
  BOOL source_sign_extd,   // is source load sign-extended
  BOOL target_sign_extd,   // is target load sign-extended
  OPCODE *opc
)
{
  Is_True(MTYPE_is_integral(to_ty),("Need_load_type_conversion: non integral type"));
  Is_True(to_ty != MTYPE_BS,("Need_load_type_conversion: illegal MTYPE_BS"));
  Is_True(MTYPE_size_min(to_ty) >= MTYPE_size_min(MTYPE_I4), 
	  ("Need_load_type_conversion: data type size less than I4"));
  
  if (source_sign_extd == target_sign_extd || 
      MTYPE_size_min(dsc_ty) > MTYPE_size_min(MTYPE_I5) && dsc_ty != MTYPE_BS) 
    return NOT_AT_ALL;

  switch (MTYPE_size_min(dsc_ty)) {
    // For now no tricks - just make a CVT of all
    case 40:
    case 32:
      *opc = OPCODE_make_op(OPR_CVT, to_ty, from_ty);
      return NEED_CVT;

    default: // I{1,2}/U{1,2}
      *opc = OPCODE_make_op(OPR_CVTL, to_ty, MTYPE_V);
      return NEED_CVTL;
  }
}
#endif

INT Need_load_type_conversion(BOOL source_sign_extd, BOOL target_sign_extd, 
			      TYPE_ID to_ty, TYPE_ID dsc_ty, OPCODE *opc)
{

  Is_True(MTYPE_is_integral(to_ty),("Need_load_type_conversion: non integral type"));
  Is_True(to_ty != MTYPE_BS,("Need_load_type_conversion: illegal MTYPE_BS"));
  Is_True(MTYPE_size_min(to_ty)>=MTYPE_size_min(MTYPE_I4), 
	  ("Need_load_type_conversion: data type size less than I4"));

  if (source_sign_extd == target_sign_extd || 
      MTYPE_size_min(dsc_ty) > MTYPE_size_min(MTYPE_I5) && dsc_ty != MTYPE_BS) 
    return NOT_AT_ALL;
  
  if (MTYPE_size_min(dsc_ty) == MTYPE_size_min(MTYPE_I5)) {
    // XXX ST100 not tested
    *opc = OPCODE_make_op(OPR_CVT, to_ty, dsc_ty);
    return NEED_CVT;
  }
  else if (MTYPE_size_min(dsc_ty) == MTYPE_size_min(MTYPE_I4) && 
      dsc_ty != MTYPE_BS) {
    if (! Split_64_Bit_Int_Ops) {
      if (source_sign_extd) {            // !targ_sign_extd
        *opc = (OPCODE) OPC_U8U4CVT;
      } else {                           // targ_sign_extd && !source_sign_extd
        *opc = (OPCODE) OPC_U4U8CVT;
      }
      return NEED_CVT;
    }
    else {
      return NOT_AT_ALL;
    }
  }
  else { // I{1,2}/U{1,2}
    if (! Split_64_Bit_Int_Ops) {
      if (source_sign_extd) {            // !targ_sign_extd
        *opc = (OPCODE) (MTYPE_size_min(to_ty) == MTYPE_size_min(MTYPE_I4)) ? 
          OPC_U4CVTL : OPC_U8CVTL;
      } else {                           // targ_sign_extd && !source_sign_extd
        *opc = (OPCODE) (MTYPE_size_min(to_ty) == MTYPE_size_min(MTYPE_I4)) ? 
          OPC_I4CVTL : OPC_I8CVTL;
      }
    }
    else {
      if (source_sign_extd) {            // !targ_sign_extd
        *opc = (OPCODE) OPC_U4CVTL;
      } else {                           // targ_sign_extd && !source_sign_extd
        *opc = (OPCODE) OPC_I4CVTL;
      }
    }
    return NEED_CVTL;
  }
}
#endif /* TARG_ST */

// Check if TYPE_ID lhs_type is compatible with the RHS of assignment stmt
BOOL
Types_are_compatible(TYPE_ID lhs_type, TYPE_ID rhs_type)
{
  BOOL   compatible;
  OPCODE opc;

  if ((MTYPE_type_class(rhs_type)&MTYPE_CLASS_UNSIGNED_INTEGER) == 0 ||
      (MTYPE_type_class(lhs_type)&MTYPE_CLASS_UNSIGNED_INTEGER) == 0) {
        if (rhs_type == lhs_type) return FALSE;
        else
          return TRUE;
      }
  compatible = Need_type_conversion(rhs_type, lhs_type, &opc) != NEED_CVT;
  return compatible;
}

// Check if TYPE_ID lhs_type is compatible with the RHS of assignment stmt
BOOL
Types_are_compatible(TYPE_ID lhs_type, WN *rhs_wn)
{
  TYPE_ID  rhs_type = Actual_result_type(rhs_wn);

  return Types_are_compatible(lhs_type, rhs_type);
}
  
  

TYPE_ID Rebuild_rtype(TYPE_ID rtype, INT bits)
{
  if ( rtype == MTYPE_I4 || rtype == MTYPE_I8 ) {
    if ( bits <= 8 )
      rtype = MTYPE_I1;
    else if ( bits <= 16 )
      rtype = MTYPE_I2;
    else if ( bits <= 32 )
      rtype = MTYPE_I4;
    else
      rtype = MTYPE_I8;
  }
  else  {
    if ( bits <= 8 )
      rtype = MTYPE_U1;
    else if ( bits <= 16 )
      rtype = MTYPE_U2;
    else if ( bits <= 32 )
      rtype = MTYPE_U4;
    else
      rtype = MTYPE_U8;
  }
  return rtype;
}  

// Actual_data_size calculates the number of bits for the data
// carried by the tree, ie do not count those bits contains 0 in the
// lsb.  This function is ENDIAN dependent!
INT Actual_data_size(WN *wn)
{
  OPERATOR opr = WN_operator(wn);
  TYPE_ID rtype = WN_rtype(wn);
  INT    actual_size;

  if ((MTYPE_type_class(rtype) & MTYPE_CLASS_INTEGER) == 0)
    return MTYPE_size_min(rtype);

  switch ( opr ) {
  case OPR_INTCONST:
    {
      INT64 val;
      for (val = WN_const_val(wn), actual_size = 0;
           actual_size < 64 && val != 0; actual_size++)
        val = val >> 1;
      if (MTYPE_is_integral(rtype) && !MTYPE_is_unsigned(rtype) && MTYPE_size_min(rtype) == actual_size) {
        // must assume sign extended
        actual_size = MTYPE_size_min(Pointer_type);
      }
      return actual_size;
    }

  case OPR_CVTL:
    if (MTYPE_is_signed(rtype)) {
      actual_size = Actual_data_size(WN_kid0(wn));
      if (MTYPE_size_min(rtype) == actual_size)
        return actual_size;
      else
        return WN_cvtl_bits(wn);
    }
    else
      return WN_cvtl_bits(wn);

  case OPR_LDBITS:
  case OPR_ILDBITS:
      if (MTYPE_is_signed(rtype))
	return MTYPE_size_min(rtype);
      else
	return WN_bit_size (wn);

  case OPR_LDID:
  case OPR_ILOAD:
    if (MTYPE_is_signed(rtype))  // pv 361929
      return MTYPE_size_min(rtype);
    else
      return MTYPE_size_min(WN_desc(wn));

  case OPR_BAND:
  case OPR_BIOR:
    {
      INT kid0_size = Actual_data_size(WN_kid0(wn));
      INT kid1_size = Actual_data_size(WN_kid1(wn));
      if (MTYPE_is_unsigned(rtype)) {
        if (opr == OPR_BIOR)
          return (kid0_size > kid1_size)? kid0_size : kid1_size;
        else return (kid0_size > kid1_size)? kid1_size : kid0_size;
      }
      else return MTYPE_size_min(rtype);
    }

  case OPR_SHL:
  case OPR_ASHR:
  case OPR_LSHR:
    {
      // The kid1 contains the number of bits.  
      WN *bits = WN_kid1(wn);

      // skip if kid 1 is not a constant.
      if ( WN_operator(bits) == OPR_INTCONST ) {
        UINT bit_cnt = WN_const_val(bits);
        if (MTYPE_size_min(rtype) == 32)
          bit_cnt &= 0x1F;  // use the low 5 bits
        else
          bit_cnt &= 0x3F;

        actual_size = Actual_data_size(WN_kid0(wn));
        if (opr == OPR_SHL) {
	  if (MTYPE_is_unsigned(rtype)) {
            actual_size += bit_cnt;
            if (actual_size > MTYPE_size_min(rtype)) // pv 364274
              actual_size = MTYPE_size_min(rtype);
	  }
	  else actual_size = MTYPE_size_min(rtype);
        }
        else if (MTYPE_is_unsigned(rtype)) {
          actual_size -= bit_cnt;
          if (actual_size < 0) actual_size = 0;
        }
        else actual_size = MTYPE_size_min(rtype);
        return actual_size;
      }
    }
    break;
  default:
    ;
  }
  return MTYPE_size_min(rtype);
}


// given a OPR_CVTL opcode and the number of bits it converting to,
// return the actual type.
TYPE_ID Actual_cvtl_type(OPCODE opc, INT bits)
{
  return Rebuild_rtype(OPCODE_rtype(opc), bits);
}

// Actual_result_type returns the data type of the tree with the
// actual number of bytes that we can determine at this point.  This
// function helps to determine a CVTL node is required or can be
// deleted by the optimizer emitter.
TYPE_ID Actual_result_type(WN *wn)
{
  OPCODE opc = WN_opcode(wn);
  TYPE_ID  rtype = OPCODE_rtype(opc);
  INT    bits;

  // only do it for an integer type
  if ((MTYPE_type_class(rtype) & MTYPE_CLASS_INTEGER) == 0)
    return rtype;

  switch ( OPCODE_operator(opc) ) {
  case OPR_CVTL:
  case OPR_LDID:
  case OPR_LDBITS:
  case OPR_ILOAD:
  case OPR_ILDBITS:
  case OPR_BIOR:
  case OPR_BAND:
  case OPR_SHL:
  case OPR_ASHR:
  case OPR_LSHR:
  case OPR_INTCONST:
    bits = Actual_data_size(wn);
    rtype = Rebuild_rtype(rtype, bits);
  }

  return rtype;
}

TYPE_ID Adjust_signed_type(TYPE_ID rtype, INT size, WN *wn)
{
  INT bits = Actual_data_size(wn);

  if (MTYPE_is_integral(rtype) && !MTYPE_is_unsigned(rtype) && size == bits) {
    // need sign extension
    // 32 is the largest size CVTL would convert to
    return Rebuild_rtype(rtype, 32);
  }
  return MTYPE_UNKNOWN;
}
