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


#include "lnotarget.h"
#include "w2op.h"
#include "model.h"

/* ====================================================================
 *   Mult_Latency
 *
 *   This latency numbers are calculated from C.Monat's intrinsics in
 *   the COS/lib/ST100.lai file
 * ====================================================================
 */
static INT
Mult_Latency (
  TYPE_ID rtype
)
{
  INT latency;

  FmtAssert(FALSE,("not implemented"));

  if (rtype == MTYPE_I4 || rtype == MTYPE_U4) {
    // __MULW
    latency = 6;
  }
  else if (rtype == MTYPE_I5 || rtype == MTYPE_U5) {
    // __MULE
    latency = 8;
  }
  else {
    FmtAssert(FALSE,("Mult_Latency: unknown mtype %s", 
                                                   MTYPE_name(rtype)));
  }

  return latency;
}

/* ====================================================================
 *   LNOTARGET_Opcode_Lat
 *
 *   Given a WHIRL nodeopcode , is it usefull in latency calculations ?
 *   If it is, what is its latency ?
 * ====================================================================
 */
INT
LNOTARGET_Opcode_Lat (
  OPCODE opcode
)
{
  // The switch is necessary because some of the OPCODEs are not 
  // defined by the WHIRL_To_TOP 
  // function but I'd like to be able to analyse code containing
  // them.

  // Floating point arithmetic on the ST100 is implementing using 
  // the function calls => not subject to all this ...

  switch (opcode) {
    case OPC_I4MPY:
      return Mult_Latency(MTYPE_I4);
    case OPC_U4MPY:
      return Mult_Latency(MTYPE_U4);
    case OPC_I8MPY:
      return Mult_Latency(MTYPE_I8);
    case OPC_U8MPY:
      return Mult_Latency(MTYPE_U8);

    case OPC_I4F4LT: 
    case OPC_I4F8LT:
    case OPC_I4F4GT: 
    case OPC_I4F8GT:
    case OPC_I4F4LE: 
    case OPC_I4F8LE:  
    case OPC_I4F4GE: 
    case OPC_I4F8GE:  
    case OPC_I4F4EQ: 
    case OPC_I4F8EQ:
    case OPC_I4F4NE: 
    case OPC_I4F8NE:
      FmtAssert(FALSE,("LNOTARGET_Opcode_Lat: %s should write a BOOL\n",
		       OPCODE_name(opcode)));

    case OPC_BI8LT: 
    case OPC_BU8LT: 
    case OPC_BF4GT:
    case OPC_BF8GT:
    case OPC_BF4LE:  
    case OPC_BF8LE: 
    case OPC_BF4GE:
    case OPC_BF8GE: 
    case OPC_BF4EQ:  
    case OPC_BF8EQ:
    case OPC_BF4NE:
    case OPC_BF8NE:
      FmtAssert(FALSE,("LNOTARGET_Opcode_Lat: %s not implemented\n",
		       OPCODE_name(opcode)));

    default:
      // can not use it in the latency modelling
      DevWarn("Unknown opcode in LNOTARGET_Opcode_Lat");
      //      fprintf(TFile, "LNOTARGET_Opcode_Lat: %s unknown", OPCODE_name(opcode));

      return -1;
  }

}

/* ====================================================================
 *   LNOTARGET_Whirl_To_Top
 * ====================================================================
 */
TOP
LNOTARGET_Whirl_To_Top (
  WN* wn
)
{
  OPCODE opcode = WN_opcode(wn);

  FmtAssert(FALSE,("LNOTARGET_Whirl_To_Top: obsolete"));

  // Floating point arithmetic on the ST100 is implementing using 
  // the function calls => not subject to all this ...
  switch (opcode) {
  default:
    return TOP_UNDEFINED;
  }

  return WHIRL_To_TOP(wn);
}

/* ====================================================================
 *   LNOTARGET_Loop_Inc_Test_Res
 *
 *   Since we're dealing with counted loops in the LNO, they should
 *   be easily mapped to hardware loops. Seems like this has no cost 
 *   on the ST100 ??
 * ====================================================================
 */
void
LNOTARGET_Loop_Inc_Test_Res (
  TI_RES_COUNT* resource_count
)
{
  return;
}

/* ====================================================================
 *   LNOTARGET_Cvt_Res
 *
 *   ??
 * ====================================================================
 */
double
LNOTARGET_Cvt_Res (TI_RES_COUNT* resource_count, OPCODE opcode)
{
#if 0
  switch(opcode) {
    case OPC_F4I4CVT: 
    case OPC_F4I8CVT: 
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_sig);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fnorm_s);
      return 3.0;
    case OPC_F8I4CVT: 
    case OPC_F8I8CVT: 
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_sig);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fnorm_d);
      return 3.0;
    case OPC_F4U4CVT: 
    case OPC_F4U8CVT: 
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_sig);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fnorm_s);
      return 2.0;
    case OPC_F8U4CVT: 
    case OPC_F8U8CVT: 
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_sig);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fnorm_d);
      return 2.0;
    case OPC_I4F4CVT: 
    case OPC_I4F8CVT: 
    case OPC_I8F4CVT: 
    case OPC_I8F8CVT: 
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fx);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_getf_sig);
      return 2.0;
    case OPC_U4F4CVT: 
    case OPC_U4F8CVT: 
    case OPC_U8F4CVT: 
    case OPC_U8F8CVT: 
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fxu);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_getf_sig);
      return 2.0;
    case OPC_I4F4TRUNC: 
    case OPC_I8F4TRUNC: 
    case OPC_I4F8TRUNC: 
    case OPC_I8F8TRUNC: 
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fx_trunc);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_getf_sig);
      return 2.0;
    case OPC_U4F4TRUNC:
    case OPC_U8F4TRUNC:
    case OPC_U4F8TRUNC:
    case OPC_U8F8TRUNC:
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fxu_trunc);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_getf_sig);
      return 2.0;
    case OPC_I4F4RND: 
    case OPC_I8F4RND: 
    case OPC_I4F8RND: 
    case OPC_I8F8RND: 
    case OPC_I4F4CEIL: 
    case OPC_I8F4CEIL: 
    case OPC_I4F8CEIL: 
    case OPC_I8F8CEIL: 
    case OPC_I4F4FLOOR: 
    case OPC_I8F4FLOOR: 
    case OPC_I4F8FLOOR: 
    case OPC_I8F8FLOOR: 
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fx);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fsetc);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_getf_sig);
      return 3.0;
    case OPC_U4F4RND:
    case OPC_U8F4RND:
    case OPC_U4F8RND:
    case OPC_U8F8RND:
    case OPC_U4F4CEIL:
    case OPC_U8F4CEIL:
    case OPC_U4F8CEIL:
    case OPC_U8F8CEIL:
    case OPC_U4F4FLOOR:
    case OPC_U8F4FLOOR:
    case OPC_U4F8FLOOR:
    case OPC_U8F8FLOOR:
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fxu);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fsetc);
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_getf_sig);
      return 3.0;
    case OPC_F8F4CVT:
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fnorm_d);
      return 1.0;
    case OPC_F4F8CVT:
      TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fnorm_s);
      return 1.0;
  }
#endif
  return 0.0;
}

double
LNOTARGET_FP_Madd_Res (TI_RES_COUNT* resource_count, TYPE_ID mtype)
{
  FmtAssert (FALSE,("LNOTARGET_FP_Madd_Res: not implemented"));
#if 0
  TOP fma = (mtype == MTYPE_F4 ? TOP_fma_s : TOP_fma_d);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);
#endif
  return 1.0;
}

double
LNOTARGET_FP_Min_Max_Res (TI_RES_COUNT* resource_count, TYPE_ID mtype)
{
  FmtAssert (FALSE,("LNOTARGET_FP_Min_Max_Res: not implemented"));
  /*
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcmp_lt);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_mov_f);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_mov_f);
  */
  return 3.0;
}

double
LNOTARGET_FP_Div_Res (TI_RES_COUNT* resource_count, TYPE_ID mtype)
{
  FmtAssert (FALSE,("LNOTARGET_FP_Div_Res: not implemented"));
  /*
  if (mtype == MTYPE_F8) {
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_frcpa);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fnma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fnma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma_d);
    return 10.0;
  }
  else {
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_frcpa);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fnma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma_s);
    return 8.0;
  }
  */
}

double
LNOTARGET_FP_Recip_Res (TI_RES_COUNT* resource_count, TYPE_ID mtype)
{
  FmtAssert (FALSE,("LNOTARGET_FP_Recip_Res: not implemented"));
  /*
  if (mtype == MTYPE_F8) {
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_frcpa);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fnma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fnma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma_d);
    return 9.0;
  }
  else {
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_frcpa);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fnma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma_s);
    return 7.0;
  }
  */
}

double
LNOTARGET_FP_Rsqrt_Res (TI_RES_COUNT* resource_count, TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_FP_Rsqrt_Res: not implemented"));
  /*
  BOOL is_double = (MTYPE_is_size_double(mtype) != 0);
  TOP fnma = is_double ? TOP_fnma_d : TOP_fnma_s;
  TOP fma  = is_double ? TOP_fma_d  : TOP_fma_s;
  TOP fmpy = is_double ? TOP_fmpy_d : TOP_fmpy_s;

  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_frsqrta);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_mov_i);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_exp);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fmpy);

  TI_RES_COUNT_Add_Op_Resources(resource_count, fmpy);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);

  TI_RES_COUNT_Add_Op_Resources(resource_count, fmpy);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);

  if (is_double) {
    TI_RES_COUNT_Add_Op_Resources(resource_count, fmpy);
    TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, fma);
    return 13.0;
  }
  else {
    return 10.0;
  }
  */
}

double
LNOTARGET_FP_Sqrt_Res (TI_RES_COUNT* resource_count, TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_FP_Sqrt_Res: not implemented"));
#if 0
  BOOL is_double = (MTYPE_is_size_double(mtype) != 0);
  TOP fnma = is_double ? TOP_fnma_d : TOP_fnma_s;
  TOP fma  = is_double ? TOP_fma_d  : TOP_fma_s;
  TOP fmpy = is_double ? TOP_fmpy_d : TOP_fmpy_s;
  TOP fadd = is_double ? TOP_fadd_d : TOP_fadd_s;

  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_frsqrta);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_mov_i);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_exp);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_mov_i);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_exp);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fadd);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fmpy);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fmpy);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);

  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);
  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fadd);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);
  
  if (is_double) {
    TI_RES_COUNT_Add_Op_Resources(resource_count, fma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, fadd);
    TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);
    TI_RES_COUNT_Add_Op_Resources(resource_count, fma);
  }

  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);
  return (is_double ? 24.0 : 19.0);
#endif
  return 0.0;
}
 
double
LNOTARGET_FP_Exp_Res (TI_RES_COUNT* resource_count, 
                      INTRINSIC intr,
                      INT num_multiplies)
{
  FmtAssert(FALSE,("LNOTARGET_FP_Exp_Res: not implemented"));
#if 0
  INT i;
  switch (intr) {
    case INTRN_F4I4EXPEXPR: 
    case INTRN_F4I8EXPEXPR: 
      for (i = 0; i < num_multiplies; i++) {
        TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy_s);
      }
      return num_multiplies;
    case INTRN_F8I4EXPEXPR: 
    case INTRN_F8I8EXPEXPR: 
      for (i = 0; i < num_multiplies; i++) {
        TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy_d);
      }
      return num_multiplies;
    case INTRN_C4I4EXPEXPR: 
    case INTRN_C4I8EXPEXPR: 
      for (i = 0; i < num_multiplies; i++) {
        TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma_s);
        TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma_s);
        TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy_s);
        TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy_s);
      }
      return 4*num_multiplies;
    case INTRN_C8I4EXPEXPR: 
    case INTRN_C8I8EXPEXPR: 
      for (i = 0; i < num_multiplies; i++) {
        TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma_d);
        TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fma_d);
        TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy_d);
        TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fmpy_d);
      }
      return 4*num_multiplies;
  }
#endif
  return 0.0;
}

double
LNOTARGET_Complex_Add_Res (TI_RES_COUNT* resource_count, TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_Complex_Add_Res: not implemented"));
#if 0
  TOP fadd = (mtype == MTYPE_C4 ? TOP_fadd_s : TOP_fadd_d);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fadd);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fadd);
#endif
  return 2.0;
}

double
LNOTARGET_Complex_Mult_Res (TI_RES_COUNT* resource_count, TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_Complex_Mult_Res: not implemented"));
#if 0
  TOP fmpy = (mtype == MTYPE_C4 ? TOP_fmpy_s : TOP_fmpy_d);
  TOP fma  = (mtype == MTYPE_C4 ? TOP_fma_s  : TOP_fma_d);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fmpy);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fmpy);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);
#endif
  return 4.0;
}

double
LNOTARGET_Complex_Neg_Res (TI_RES_COUNT* resource_count, TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_Complex_Neg_Res: not implemented"));
#if 0
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fneg);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fneg);
#endif
  return 2.0;
}

double
LNOTARGET_Int_Select_Res (TI_RES_COUNT* resource_count)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Select_Res: not implemented"));
#if 0
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_cmp_ne);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_mov);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_mov);
#endif
  return 3.0;
}

double
LNOTARGET_Int_Cvtl_Res (TI_RES_COUNT* resource_count)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Cvtl_Res: not implemented"));
#if 0
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_extr);
#endif
  return 1.0;
}

double
LNOTARGET_Int_Neg_Res (TI_RES_COUNT* resource_count, BOOL eight_bytes)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Neg_Res: not implemented"));
  /*
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_sub);
  return 1.0;
  */
}

double
LNOTARGET_Int_Abs_Res (TI_RES_COUNT* resource_count, BOOL eight_bytes)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Abs_Res: not implemented"));
  /*
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_cmp_lt);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_sub);
  return 1.0;
  */
}

double
LNOTARGET_Int_Bnot_Res (TI_RES_COUNT* resource_count)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Bnot_Res: not implemented"));
  /*
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_xor_i);
  return 1.0;
  */
}

double
LNOTARGET_Int_Lnot_Res (TI_RES_COUNT* resource_count)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Lnot_Res: not implemented"));
  /*
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_xor_i);
  return 1.0;
  */
}

/* ====================================================================
 *   LNOTARGET_Int_Mult_Res
 * ====================================================================
 */
double
LNOTARGET_Int_Mult_Res (TI_RES_COUNT* resource_count, TYPE_ID rtype)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Mult_Res: not implemented"));
#if 0
  if (rtype == MTYPE_I2) {
    // a 16 = 16 x 16 multiply, __MULH
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MPSSLL_GT_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_EXTH_GT_DR_DR);
    return 2.0;
  }
  else if (rtype == MTYPE_U2) {
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MPUULL_GT_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_EXTUH_GT_DR_DR);
    return 2.0;
  }
  else if (rtype == MTYPE_I4) {
    // __MULW
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MPSUHL_GT_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MAUSLH_GT_DR_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHLU_GT_DR_DR_U5);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MAUULL_GT_DR_DR_DR_DR);
    return 4.0;
  }
  else if (rtype == MTYPE_U4) {
    // __MULUW
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MPUUHL_GT_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MAUULH_GT_DR_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHLU_GT_DR_DR_U5);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MAUULL_GT_DR_DR_DR_DR);
    return 4.0;
  }
  else if (rtype == MTYPE_I5) {
    // __MULE
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHR32_GT_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHR32_GT_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MPSULL_GT_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MASULL_GT_DR_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MAUUHH_GT_DR_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHLU32_GT_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MPUUHL_GT_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MAUULH_GT_DR_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHLU_GT_DR_DR_U5);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MAUULL_GT_DR_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_ADD_GT_DR_DR_DR);
    return 11.0;
  }
  else if (rtype == MTYPE_U5) {
    // __MULUE
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHR32_GT_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHR32_GT_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MPUULL_GT_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MAUULL_GT_DR_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MAUUHH_GT_DR_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHLU32_GT_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MPUUHL_GT_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MAUULH_GT_DR_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHLU_GT_DR_DR_U5);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_MAUULL_GT_DR_DR_DR_DR);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_ADD_GT_DR_DR_DR);
    return 11.0;
  }
  else {
    FmtAssert(FALSE,("LNOTARGET_Int_Mult_Res: unknown mtype %s", 
                                                   MTYPE_name(rtype)));
  }
#endif
  return 0.0;
}

/* ====================================================================
 *   LNOTARGET_Int_Add_Res
 * ====================================================================
 */
double
LNOTARGET_Int_Add_Res (TI_RES_COUNT* resource_count, TYPE_ID rtype)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Add_Res: not implemented"));
#if 0
  switch (rtype) {
  case MTYPE_I1:
  case MTYPE_U1:
  case MTYPE_I2:
  case MTYPE_I4:
  case MTYPE_U4:
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, 
                                            TOP_GP32_ADD_GT_DR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Add_Res: unknown mtype %s", 
                                                  MTYPE_name(rtype)));
  }
#endif
  return 1.0;
}

/* ====================================================================
 *   LNOTARGET_Int_Sub_Res
 * ====================================================================
 */
double
LNOTARGET_Int_Sub_Res (TI_RES_COUNT* resource_count, TYPE_ID rtype)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Sub_Res: not implemented"));
#if 0
  switch (rtype) {
  case MTYPE_I1:
  case MTYPE_U1:
  case MTYPE_I2:
  case MTYPE_I4:
  case MTYPE_U4:
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, 
                                            TOP_GP32_SUB_GT_DR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Add_Res: unknown mtype %s", 
                                                  MTYPE_name(rtype)));
  }
#endif
  return 1.0;
}

double
LNOTARGET_Int_Div_Res (TI_RES_COUNT* resource_count, BOOL eight_bytes)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Div_Res: not implemented"));
#if 0
  TOP fnma = eight_bytes ? TOP_fnma : TOP_fnma_d;
  TOP fma  = eight_bytes ? TOP_fma  : TOP_fma_d;
  TOP fmpy = eight_bytes ? TOP_fmpy : TOP_fmpy_d;

  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_sig);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_sig);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_frcpa);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  if (eight_bytes) {
    TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
    TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  }
  TI_RES_COUNT_Add_Op_Resources(resource_count, fmpy);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fx_trunc);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_getf_sig);  
  return (eight_bytes ? 18.0 : 16.0);
#endif
  return 0.0;
}

double
LNOTARGET_Int_Mod_Res (TI_RES_COUNT* resource_count, BOOL eight_bytes)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Mod_Res: not implemented"));
#if 0
  TOP fnma = eight_bytes ? TOP_fnma   : TOP_fnma_d;
  TOP fma  = eight_bytes ? TOP_fma    : TOP_fma_d;
  TOP fmpy = eight_bytes ? TOP_fmpy   : TOP_fmpy_d;
  TOP cmp  = eight_bytes ? TOP_cmp_lt : TOP_cmp4_lt;

  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_sig);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_sig);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_frcpa);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  if (eight_bytes) {
    TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
    TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  }
  TI_RES_COUNT_Add_Op_Resources(resource_count, fmpy);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fx_trunc);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fx);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_getf_sig);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_xor);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, cmp);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_add);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_mov);  
  return (eight_bytes ? 25.0 : 23.0);
#endif
  return 0.0;
}

double
LNOTARGET_Int_Rem_Res (TI_RES_COUNT* resource_count, BOOL eight_bytes)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Rem_Res: not implemented"));
  /*
  TOP fnma = eight_bytes ? TOP_fnma : TOP_fnma_d;
  TOP fma  = eight_bytes ? TOP_fma  : TOP_fma_d;
  TOP fmpy = eight_bytes ? TOP_fmpy : TOP_fmpy_d;

  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_sig);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_sig);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_frcpa);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  if (eight_bytes) {
    TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
    TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  }
  TI_RES_COUNT_Add_Op_Resources(resource_count, fmpy);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fx_trunc);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fx);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_getf_sig);  
  return (eight_bytes ? 21.0 : 19.0);
  */
}

double
LNOTARGET_Int_DivRem_Res (TI_RES_COUNT* resource_count, BOOL eight_bytes)
{
  FmtAssert(FALSE,("LNOTARGET_Int_DivRem_Res: not implemented"));
  /*
  TOP fnma = eight_bytes ? TOP_fnma : TOP_fnma_d;
  TOP fma  = eight_bytes ? TOP_fma  : TOP_fma_d;
  TOP fmpy = eight_bytes ? TOP_fmpy : TOP_fmpy_d;

  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_sig);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_setf_sig);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_frcpa);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  if (eight_bytes) {
    TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
    TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  }
  TI_RES_COUNT_Add_Op_Resources(resource_count, fmpy);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fx_trunc);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_xf);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, fnma);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_fcvt_fx);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_getf_sig);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_getf_sig);  
  return (eight_bytes ? 22.0 : 20.0);
  */
}

double
LNOTARGET_Int_Min_Max_Res (TI_RES_COUNT* resource_count, BOOL minmax)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Min_Max_Res: not implemented"));
  /*
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_cmp_lt);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_mov);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_mov);
  if (minmax) {
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_mov);
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_mov);
    return 5.0;
  }
  else {
    return 3.0;
  }
  */
}
  
double
LNOTARGET_Int_Band_Res (TI_RES_COUNT* resource_count)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Band_Res: not implemented"));
  /*
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_and);  
  return 1.0;
  */
}

double
LNOTARGET_Int_Bior_Res (TI_RES_COUNT* resource_count)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Bior_Res: not implemented"));
  /*
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_or);  
  return 1.0;
  */
}

double
LNOTARGET_Int_Bnor_Res (TI_RES_COUNT* resource_count)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Bnor_Res: not implemented"));
  /*
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_or);  
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_xor);  
  return 2.0;
  */
}

double
LNOTARGET_Int_Bxor_Res (TI_RES_COUNT* resource_count)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Bxor_Res: not implemented"));
  /*
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_xor);  
  return 1.0;
  */
}

double
LNOTARGET_Int_Land_Res (
  TI_RES_COUNT* resource_count,
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Land_Res: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I4:
  case MTYPE_U4:
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_AND_GT_DR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Land_Res: mtype"));
  }
#endif
  return 0.0;
}

double
LNOTARGET_Int_Cand_Res (
  TI_RES_COUNT* resource_count,
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Cand_Res: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I4:
  case MTYPE_U4:
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_AND_GT_DR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Cand_Res: mtype"));
  }
#endif
  return 1.0;
}

double
LNOTARGET_Int_Lior_Res (
  TI_RES_COUNT* resource_count,
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Lior_Res: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I4:
  case MTYPE_U4:
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_OR_GT_DR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Lior_Res: mtype"));
  }
#endif
  return 1.0;
}

double
LNOTARGET_Int_Cior_Res (
  TI_RES_COUNT* resource_count,
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Cior_Res: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I4:
  case MTYPE_U4:
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_OR_GT_DR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Cior_Res: mtype"));
  }
#endif
  return 1.0;
}

double
LNOTARGET_Int_Shl_Res (
  TI_RES_COUNT* resource_count, 
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Shl_Res: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I4:
  case MTYPE_U4:
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHL_GT_DR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Shl_Res: mtype"));
  }
#endif
  return 0.0;
}

double
LNOTARGET_Int_Ashr_Res (
  TI_RES_COUNT* resource_count, 
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Ashr_Res: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I4:
  case MTYPE_U4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHRW_GT_DR_DR_DR);
    return 1.0;
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_SHR_GT_DR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Ashr_Res: mtype"));
  }
#endif
  return 0.0;
}

double
LNOTARGET_Int_Lshr_Res (TI_RES_COUNT* resource_count, BOOL eight_bytes)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Lshr_Res: not implemented"));
  /*
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_shr_u);
  if (!eight_bytes) {
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_sxt4);
    return 2.0;
  }
  else {
    return 1.0;
  }
  */
}

double
LNOTARGET_Int_Eq_Res (
  TI_RES_COUNT* resource_count,
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Eq_Res: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_EQW_GT_BR_DR_DR);
    return 1.0;
  case MTYPE_U4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_EQUW_GT_BR_DR_DR);
    return 1.0;
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_EQE_GT_BR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Eq_Res: mtype"));
  }
#endif
  return 0.0;
}

double
LNOTARGET_Int_Ne_Res (
  TI_RES_COUNT* resource_count,
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Ne_Res: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_NEW_GT_BR_DR_DR);
    return 1.0;
  case MTYPE_U4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_NEUW_GT_BR_DR_DR);
    return 1.0;
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_NEE_GT_BR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Ne_Res: mtype"));
  }
#endif
  return 0.0;
}

double
LNOTARGET_Int_Gt_Res (
  TI_RES_COUNT* resource_count,
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Gt_Res: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_GTW_GT_BR_DR_DR);
    return 1.0;
  case MTYPE_U4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_GTUW_GT_BR_DR_DR);
    return 1.0;
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_GTE_GT_BR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Gt_Res: mtype"));
  }
#endif
  return 0.0;
}

double
LNOTARGET_Int_Ge_Res (
  TI_RES_COUNT* resource_count,
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Ge_Res: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_GEW_GT_BR_DR_DR);
    return 1.0;
  case MTYPE_U4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_GEUW_GT_BR_DR_DR);
    return 1.0;
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_GEE_GT_BR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Ge_Res: mtype"));
  }
#endif
  return 0.0;
}

double
LNOTARGET_Int_Lt_Res (
  TI_RES_COUNT* resource_count,
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Lt_Res: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_LTW_GT_BR_DR_DR);
    return 1.0;
  case MTYPE_U4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_LTUW_GT_BR_DR_DR);
    return 1.0;
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_LTE_GT_BR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Lt_Res: mtype"));
  }
#endif
  return 0.0;
}

double
LNOTARGET_Int_Le_Res (
  TI_RES_COUNT* resource_count,
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Le_Res: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_LEW_GT_BR_DR_DR);
    return 1.0;
  case MTYPE_U4:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_LEUW_GT_BR_DR_DR);
    return 1.0;
  case MTYPE_I5:
  case MTYPE_U5:
    TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_GP32_LEE_GT_BR_DR_DR);
    return 1.0;
  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Le_Res: mtype"));
  }
#endif
  return 0.0;
}

double
LNOTARGET_Int_Lda_Res (TI_RES_COUNT* resource_count)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Lda_Res: not implemented"));
  /*
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_add);
  TI_RES_COUNT_Add_Op_Resources(resource_count, TOP_add);
  return 2.0;
  */
}


INT
LNOTARGET_Cvt_Lat (OPCODE opcode)
{
  FmtAssert(FALSE,("LNOTARGET_Cvt_Lat: not implemented"));
  /*
  switch(opcode) {
    case OPC_F4I4CVT: 
    case OPC_F4I8CVT: 
      return LNOTARGET_Top_Latency(TOP_setf_sig)
           + LNOTARGET_Top_Latency(TOP_fcvt_xf)
           + LNOTARGET_Top_Latency(TOP_fnorm_s);
    case OPC_F8I4CVT: 
    case OPC_F8I8CVT: 
      return LNOTARGET_Top_Latency(TOP_setf_sig)
           + LNOTARGET_Top_Latency(TOP_fcvt_xf)
           + LNOTARGET_Top_Latency(TOP_fnorm_d);
    case OPC_F4U4CVT: 
    case OPC_F4U8CVT: 
      return LNOTARGET_Top_Latency(TOP_setf_sig)
           + LNOTARGET_Top_Latency(TOP_fnorm_s);
    case OPC_F8U4CVT: 
    case OPC_F8U8CVT: 
      return LNOTARGET_Top_Latency(TOP_setf_sig)
           + LNOTARGET_Top_Latency(TOP_fnorm_d);
    case OPC_I4F4CVT: 
    case OPC_I4F8CVT: 
    case OPC_I8F4CVT: 
    case OPC_I8F8CVT: 
      return LNOTARGET_Top_Latency(TOP_fcvt_fx)
           + LNOTARGET_Top_Latency(TOP_getf_sig);
    case OPC_U4F4CVT: 
    case OPC_U4F8CVT: 
    case OPC_U8F4CVT: 
    case OPC_U8F8CVT: 
      return LNOTARGET_Top_Latency(TOP_fcvt_fxu)
           + LNOTARGET_Top_Latency(TOP_getf_sig);
    case OPC_I4F4TRUNC: 
    case OPC_I8F4TRUNC: 
    case OPC_I4F8TRUNC: 
    case OPC_I8F8TRUNC: 
      return LNOTARGET_Top_Latency(TOP_fcvt_fx_trunc)
           + LNOTARGET_Top_Latency(TOP_getf_sig);
    case OPC_U4F4TRUNC:
    case OPC_U8F4TRUNC:
    case OPC_U4F8TRUNC:
    case OPC_U8F8TRUNC:
      return LNOTARGET_Top_Latency(TOP_fcvt_fxu_trunc)
           + LNOTARGET_Top_Latency(TOP_getf_sig);
    case OPC_I4F4RND: 
    case OPC_I8F4RND: 
    case OPC_I4F8RND: 
    case OPC_I8F8RND: 
    case OPC_I4F4CEIL: 
    case OPC_I8F4CEIL: 
    case OPC_I4F8CEIL: 
    case OPC_I8F8CEIL: 
    case OPC_I4F4FLOOR: 
    case OPC_I8F4FLOOR: 
    case OPC_I4F8FLOOR: 
    case OPC_I8F8FLOOR: 
      return LNOTARGET_Top_Latency(TOP_fcvt_fx)
           + LNOTARGET_Top_Latency(TOP_fsetc)
           + LNOTARGET_Top_Latency(TOP_getf_sig);
    case OPC_U4F4RND:
    case OPC_U8F4RND:
    case OPC_U4F8RND:
    case OPC_U8F8RND:
    case OPC_U4F4CEIL:
    case OPC_U8F4CEIL:
    case OPC_U4F8CEIL:
    case OPC_U8F8CEIL:
    case OPC_U4F4FLOOR:
    case OPC_U8F4FLOOR:
    case OPC_U4F8FLOOR:
    case OPC_U8F8FLOOR:
      return LNOTARGET_Top_Latency(TOP_fcvt_fxu)
           + LNOTARGET_Top_Latency(TOP_fsetc)
           + LNOTARGET_Top_Latency(TOP_getf_sig);
    case OPC_F8F4CVT:
      return LNOTARGET_Top_Latency(TOP_fnorm_d);
    case OPC_F4F8CVT:
      return LNOTARGET_Top_Latency(TOP_fnorm_s);
  }
  return -1;
  */
}

INT
LNOTARGET_FP_Madd_Add_Lat (TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_FP_Madd_Add_Lat: not implemented"));
  /*
  TOP fma = (mtype == MTYPE_F4 ? TOP_fma_s : TOP_fma_d);
  return TI_LATENCY_Result_Available_Cycle(fma, 0) 
       - TI_LATENCY_Operand_Access_Cycle(fma, 0);
  */
}

INT
LNOTARGET_FP_Madd_Mult_Lat (TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_FP_Madd_Mult_Lat: not implemented"));
  /*
  TOP fma = (mtype == MTYPE_F4 ? TOP_fma_s : TOP_fma_d);
  return TI_LATENCY_Result_Available_Cycle(fma, 0) 
       - TI_LATENCY_Operand_Access_Cycle(fma, 1);
  */
}

INT
LNOTARGET_FP_Min_Max_Lat (TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_FP_Min_Max_Lat: not implemented"));
  /*
  return LNOTARGET_Top_Latency(TOP_fcmp_lt)
       + LNOTARGET_Top_Latency(TOP_mov_f);
  */
}

INT
LNOTARGET_FP_Div_Lat (TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_FP_Div_Lat: not implemented"));
  /*
  Lmt_DevWarn(1, ("TODO: Finish LNOTARGET_*_Lat for IA-64"));
  return 20;
  */
}

INT
LNOTARGET_FP_Recip_Lat (TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_FP_Recip_Lat: not implemented"));
  /*
  Lmt_DevWarn(1, ("TODO: Finish LNOTARGET_*_Lat for IA-64"));
  return 15;
  */
}

INT
LNOTARGET_FP_Rsqrt_Lat (TYPE_ID mtype)
{
  Lmt_DevWarn(1, ("TODO: Finish LNOTARGET_*_Lat for IA-64"));
  return 15;
}

INT
LNOTARGET_FP_Sqrt_Lat (TYPE_ID mtype)
{
  Lmt_DevWarn(1, ("TODO: Finish LNOTARGET_*_Lat for IA-64"));
  return 20;
}

INT
LNOTARGET_FP_Exp_Lat (INTRINSIC intr, INT num_multiplies)
{
  FmtAssert(FALSE,("LNOTARGET_FP_Exp_Lat: not implemented"));
  /*
  switch (intr) {
    case INTRN_F4I4EXPEXPR: 
    case INTRN_F4I8EXPEXPR:
      return num_multiplies * LNOTARGET_Top_Latency(TOP_fmpy_s);
    case INTRN_F8I4EXPEXPR: 
    case INTRN_F8I8EXPEXPR:
      return num_multiplies * LNOTARGET_Top_Latency(TOP_fmpy_d);
    case INTRN_C4I4EXPEXPR: 
    case INTRN_C4I8EXPEXPR:
      return num_multiplies * (2*LNOTARGET_Top_Latency(TOP_fmpy_s) + 
                               2*LNOTARGET_Top_Latency(TOP_fadd_s));
    case INTRN_C8I4EXPEXPR: 
    case INTRN_C8I8EXPEXPR:
      return num_multiplies * (2*LNOTARGET_Top_Latency(TOP_fmpy_d) + 
                               2*LNOTARGET_Top_Latency(TOP_fadd_d));
  }
  return -1;
  */
}

INT
LNOTARGET_Complex_Add_Lat (TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_Complex_Add_Lat: not implemented"));
  /*
  TOP add = (mtype == MTYPE_C4 ? TOP_fadd_s : TOP_fadd_d);
  return 2*LNOTARGET_Top_Latency(add);
  */
}

INT
LNOTARGET_Complex_Mult_Lat (TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_Complex_Mult_Lat: not implemented"));
  /*
  TOP add = (mtype == MTYPE_C4 ? TOP_fadd_s : TOP_fadd_d);
  TOP mul = (mtype == MTYPE_C4 ? TOP_fmpy_s : TOP_fmpy_d);
  return 2*LNOTARGET_Top_Latency(add) 
       + 2*LNOTARGET_Top_Latency(mul);
  */
}

INT
LNOTARGET_Complex_Neg_Lat (TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_Complex_Neg_Lat: not implemented"));
  /*
  return 2*LNOTARGET_Top_Latency(TOP_fneg);
  */
}

/* ====================================================================
 *   LNOTARGET_Madd_Add_Lat
 * ====================================================================
 */
INT
LNOTARGET_Madd_Add_Lat (
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Madd_Add_Lat: not implemented"));
  /*
  TOP fma = (mtype == MTYPE_F4 ? TOP_fma_s : TOP_fma_d);
  return TI_LATENCY_Result_Available_Cycle(fma, 0) 
       - TI_LATENCY_Operand_Access_Cycle(fma, 0);
  */
}

/* ====================================================================
 *   LNOTARGET_Madd_Mult_Lat
 * ====================================================================
 */
INT
LNOTARGET_Madd_Mult_Lat (TYPE_ID mtype)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Madd_Mult_Lat: not implemented"));
  /*
  TOP fma = (mtype == MTYPE_F4 ? TOP_fma_s : TOP_fma_d);
  return TI_LATENCY_Result_Available_Cycle(fma, 0) 
       - TI_LATENCY_Operand_Access_Cycle(fma, 1);
  */
}

/* ====================================================================
 *   LNOTARGET_Max_Lat
 * ====================================================================
 */
INT
LNOTARGET_Max_Lat (
  TYPE_ID mtype
)
{
  INT latency;
  FmtAssert(FALSE,("LNOTARGET_Max_Lat: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I1:
  case MTYPE_I2:
    latency = LNOTARGET_Top_Latency(TOP_GP32_GTP_GT_BR_DR_DR) +
              2*LNOTARGET_Top_Latency(TOP_GP32_EXTH_GT_DR_DR) +
              LNOTARGET_Top_Latency(TOP_GP32_MOVE_GT_DR_DR);
    break;

  case MTYPE_I4:
    latency = LNOTARGET_Top_Latency(TOP_GP32_GTW_GT_BR_DR_DR) +
              2*LNOTARGET_Top_Latency(TOP_GP32_EXTW_GT_DR_DR) +
              LNOTARGET_Top_Latency(TOP_GP32_MOVE_GT_DR_DR);
    break;

  case MTYPE_I5:
    latency = LNOTARGET_Top_Latency(TOP_GP32_GTE_GT_BR_DR_DR) +
              3*LNOTARGET_Top_Latency(TOP_GP32_MOVE_GT_DR_DR);
    break;

  case MTYPE_U1:
  case MTYPE_U2:
    latency = LNOTARGET_Top_Latency(TOP_GP32_GTUP_GT_BR_DR_DR) +
              2*LNOTARGET_Top_Latency(TOP_GP32_EXTUH_GT_DR_DR) +
              LNOTARGET_Top_Latency(TOP_GP32_MOVE_GT_DR_DR);
    break;

  case MTYPE_U4:
    latency = LNOTARGET_Top_Latency(TOP_GP32_GTUW_GT_BR_DR_DR) +
              2*LNOTARGET_Top_Latency(TOP_GP32_EXTUW_GT_DR_DR) +
              LNOTARGET_Top_Latency(TOP_GP32_MOVE_GT_DR_DR);
    break;

  case MTYPE_U5:
    latency = LNOTARGET_Top_Latency(TOP_GP32_GTUE_GT_BR_DR_DR) +
              3*LNOTARGET_Top_Latency(TOP_GP32_MOVE_GT_DR_DR);
    break;

  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Max_Lat: unknown MTYPE_%s", 
		     MTYPE_name(mtype)));
  }
#endif
  return latency;
}

/* ====================================================================
 *   LNOTARGET_Min_Lat
 * ====================================================================
 */
INT
LNOTARGET_Min_Lat (
  TYPE_ID mtype
)
{
  INT latency;
  FmtAssert(FALSE,("LNOTARGET_Min_Lat: not implemented"));
#if 0
  switch (mtype) {
  case MTYPE_I1:
  case MTYPE_I2:
    latency = LNOTARGET_Top_Latency(TOP_GP32_LTP_GT_BR_DR_DR) +
              2*LNOTARGET_Top_Latency(TOP_GP32_EXTH_GT_DR_DR) +
              LNOTARGET_Top_Latency(TOP_GP32_MOVE_GT_DR_DR);
    break;

  case MTYPE_I4:
    latency = LNOTARGET_Top_Latency(TOP_GP32_LTW_GT_BR_DR_DR) +
              2*LNOTARGET_Top_Latency(TOP_GP32_EXTW_GT_DR_DR) +
              LNOTARGET_Top_Latency(TOP_GP32_MOVE_GT_DR_DR);
    break;

  case MTYPE_I5:
    latency = LNOTARGET_Top_Latency(TOP_GP32_LTE_GT_BR_DR_DR) +
              3*LNOTARGET_Top_Latency(TOP_GP32_MOVE_GT_DR_DR);
    break;

  case MTYPE_U1:
  case MTYPE_U2:
    latency = LNOTARGET_Top_Latency(TOP_GP32_LTUP_GT_BR_DR_DR) +
              2*LNOTARGET_Top_Latency(TOP_GP32_EXTUH_GT_DR_DR) +
              LNOTARGET_Top_Latency(TOP_GP32_MOVE_GT_DR_DR);
    break;

  case MTYPE_U4:
    latency = LNOTARGET_Top_Latency(TOP_GP32_LTUW_GT_BR_DR_DR) +
              2*LNOTARGET_Top_Latency(TOP_GP32_EXTUW_GT_DR_DR) +
              LNOTARGET_Top_Latency(TOP_GP32_MOVE_GT_DR_DR);
    break;

  case MTYPE_U5:
    latency = LNOTARGET_Top_Latency(TOP_GP32_LTUE_GT_BR_DR_DR) +
              3*LNOTARGET_Top_Latency(TOP_GP32_MOVE_GT_DR_DR);
    break;

  default:
    FmtAssert(FALSE,("LNOTARGET_Int_Min_Lat: unknown MTYPE_%s", 
		     MTYPE_name(mtype)));
  }
#endif
  return latency;
}

/* ====================================================================
 *   LNOTARGET_Div_Lat
 * ====================================================================
 */
INT
LNOTARGET_Div_Lat (
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Div_Lat: not implemented"));
  /*
  Lmt_DevWarn(1, ("TODO: Finish LNOTARGET_*_Lat for IA-64"));
  return 20;
  */
}

/* ====================================================================
 *   LNOTARGET_Recip_Lat
 * ====================================================================
 */
INT
LNOTARGET_Recip_Lat (
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Recip_Lat: not implemented"));
  /*
  Lmt_DevWarn(1, ("TODO: Finish LNOTARGET_*_Lat for IA-64"));
  return 15;
  */
}

/* ====================================================================
 *   LNOTARGET_Rsqrt_Lat
 * ====================================================================
 */
INT
LNOTARGET_Rsqrt_Lat (
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Rsqrt_Lat: not implemented"));
  /*
  Lmt_DevWarn(1, ("TODO: Finish LNOTARGET_*_Lat for IA-64"));
  return 15;
  */
}

/* ====================================================================
 *   LNOTARGET_Sqrt_Lat
 * ====================================================================
 */
INT
LNOTARGET_Sqrt_Lat (
  TYPE_ID mtype
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Sqrt_Lat: not implemented"));
  /*
  Lmt_DevWarn(1, ("TODO: Finish LNOTARGET_*_Lat for IA-64"));
  return 20;
  */
}

/* ====================================================================
 *   LNOTARGET_Exp_Lat
 * ====================================================================
 */
INT
LNOTARGET_Exp_Lat (
  INTRINSIC intr, 
  INT num_multiplies
)
{
  FmtAssert(FALSE,("LNOTARGET_Int_Exp_Lat: not implemented"));
  /*
  switch (intr) {
    case INTRN_F4I4EXPEXPR: 
    case INTRN_F4I8EXPEXPR:
      return num_multiplies * LNOTARGET_Top_Latency(TOP_fmpy_s);
    case INTRN_F8I4EXPEXPR: 
    case INTRN_F8I8EXPEXPR:
      return num_multiplies * LNOTARGET_Top_Latency(TOP_fmpy_d);
    case INTRN_C4I4EXPEXPR: 
    case INTRN_C4I8EXPEXPR:
      return num_multiplies * (2*LNOTARGET_Top_Latency(TOP_fmpy_s) + 
                               2*LNOTARGET_Top_Latency(TOP_fadd_s));
    case INTRN_C8I4EXPEXPR: 
    case INTRN_C8I8EXPEXPR:
      return num_multiplies * (2*LNOTARGET_Top_Latency(TOP_fmpy_d) + 
                               2*LNOTARGET_Top_Latency(TOP_fadd_d));
  }
  return -1;
  */
}

/* ====================================================================
 *   Arthur: some stuff that I moved over from be/lno/model.cxx
 * ====================================================================
 */

/* ====================================================================
 *   LOOP_MODEL::Initialize_Machine_Model_Parameters
 * ====================================================================
 */
void
LOOP_MODEL::Initialize_Machine_Model_Parameters ()
{
  _issue_rate = 4.0;
  _base_fp_regs = 0;
  _base_int_regs = Reserved_Int_Regs;
  _base_ptr_regs = Reserved_Ptr_Regs;
  _base_bool_regs = Reserved_Bool_Regs;
  _num_mem_units = 2.0;
}

/* ====================================================================
 *   LNOTARGET_Set_Register_Model_Parameters
 * ====================================================================
 */
void
LNOTARGET_Set_Register_Model_Parameters (
  INT *issue_rate, 
  INT *base_fp_regs, 
  INT *base_int_regs,
  INT *base_ptr_regs,
  INT *base_bool_regs,
  INT *num_mem_units
)
{
  *issue_rate = 4;
  *base_fp_regs = 0;
  *base_int_regs = Reserved_Int_Regs;
  *base_ptr_regs = Reserved_Ptr_Regs;
  *base_bool_regs = Reserved_Bool_Regs;
  *num_mem_units = 2;
}

/* ====================================================================
 *   LNOTARGET_Mtype_Registers
 * ====================================================================
 */
INT
LNOTARGET_Mtype_Registers (
  TYPE_ID mtype
)
{
  switch (mtype) {

  case MTYPE_I1:
  case MTYPE_U1:
  case MTYPE_I2:
  case MTYPE_U2:
  case MTYPE_I4:
  case MTYPE_U4:
    return 1;

  case MTYPE_I8:
  case MTYPE_U8:
    return 2;

  case MTYPE_B:
    return 1;

  default:
    FmtAssert(FALSE,("LNOTARGET_Mtype_Registers: mtype %s", MTYPE_name(mtype)));
  }
}

/* ====================================================================
 *   LNOTARGET_Mtype_Access_Bytes
 * ====================================================================
 */
INT
LNOTARGET_Mtype_Access_Bytes (
  TYPE_ID mtype
)
{
  switch (mtype) {

  case MTYPE_I1:
  case MTYPE_U1:
  case MTYPE_I2:
  case MTYPE_U2:
  case MTYPE_I4:
  case MTYPE_U4:
    return 4;

  case MTYPE_I8:
  case MTYPE_U8:
    return 8;

  case MTYPE_B:
    return 1;

  default:
    FmtAssert(FALSE,("LNOTARGET_Mtype_Access_Bytes: mtype %s", MTYPE_name(mtype)));
  }
}

