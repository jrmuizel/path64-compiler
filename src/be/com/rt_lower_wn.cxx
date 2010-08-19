/*

  Copyright (C) 2002 ST Microelectronics, Inc.  All Rights Reserved.

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


#ifdef USE_PCH
#include "be_com_pch.h"
#endif /* USE_PCH */
#pragma hdrstop

#include "defs.h"
#include "config.h"
#include "errors.h"
#include "erglob.h"
#include "tracing.h"
#include "glob.h"
#include "timing.h"
#include "stab.h"
#include "wn.h"
#include "wn_simp.h"
#include "wn_util.h"
#include "ir_reader.h"
#include "const.h"
#include "wn_lower.h"

#include "w2op.h"

#ifdef TARG_ST
#include "config_opt.h"		/* For Finite_Math. */
#endif

/* ====================================================================
 * specialized version of support routines used in the RT_LOWER template 
 * ==================================================================== */

inline void
RT_LOWER_set_kid(WN *wn, INT i, WN *k) { WN_kid(wn, i) = k; }

inline void
RT_LOWER_set_kid0(WN *wn, WN *k) { WN_kid0(wn) = k; }

inline void
RT_LOWER_set_kid1(WN *wn, WN *k) { WN_kid1(wn) = k; }

inline void
RT_LOWER_set_kid2(WN *wn, WN *k) { WN_kid2(wn) = k; }

inline OPERATOR
RT_LOWER_operator(const WN *wn) { return WN_operator(wn); }

inline OPCODE
RT_LOWER_opcode(const WN *wn) { return WN_opcode(wn); }

inline TYPE_ID
RT_LOWER_rtype(const WN *wn) { return WN_rtype(wn); }

inline void
RT_LOWER_set_rtype(WN *wn, TYPE_ID t) { WN_set_rtype(wn, t); }

inline TYPE_ID
RT_LOWER_desc(const WN *wn) { return WN_desc(wn); }

inline void
RT_LOWER_set_desc(WN *wn, TYPE_ID t) { WN_set_desc(wn, t); }

inline INT
RT_LOWER_cvtl_bits(const WN *wn) { return WN_cvtl_bits(wn); }

inline void
RT_LOWER_copy_node(WN *newwn, WN *oldwn) {}

inline WN *
RT_LOWER_alloc_stack_copy(WN *tree) { return tree; } // because WN not shared

inline WN *
RT_LOWER_form_node(WN *new_nd, WN *old_nd) { return new_nd; }

/* ====================================================================
 * End of specialized support routines
 * ==================================================================== 
 */

/* ====================================================================
 * Lowers operations on long long, float, and double types to runtime
 * support intrinsic OPs when the given type is not supported in the
 * target ISA.
 * ==================================================================== 
 */
static WN *
RT_LOWER_expr (
  WN *tree
)
{
  INT i;
  WN *nd;
  WN *new_nd = tree;
  RT_LOWER_copy_node(new_nd, tree);
  OPERATOR opr = WN_operator(tree);
  TYPE_ID res = WN_rtype(tree);
  TYPE_ID desc = WN_desc(tree);

  WN *kids[3];
  INTRINSIC intrinsic;

#if 0
  fprintf(TFile, "--- RT lowering \n");
  fdump_tree(TFile, tree);
  fflush(TFile);
#endif

  if (opr == OPR_INTRINSIC_OP) {
    for (i = 0; i < WN_kid_count(tree); i++) { // kids must be PARMs
      WN_kid(new_nd,i) = RT_LOWER_expr(WN_kid(tree,i));
    }
    return new_nd;
  }

  if (opr == OPR_SELECT) {
    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    WN_kid1(new_nd) = RT_LOWER_expr(WN_kid1(tree));
    WN_kid2(new_nd) = RT_LOWER_expr(WN_kid2(tree));
    //RT_LOWER_set_kid(new_nd, 0, RT_LOWER_expr(WN_kid0(tree)));
    //RT_LOWER_set_kid1(new_nd, RT_LOWER_expr(WN_kid1(tree)));
    //RT_LOWER_set_kid2(new_nd, RT_LOWER_expr(WN_kid2(tree)));
    return new_nd;
  }

  switch (opr) {

  case OPR_LDID:
  case OPR_LDBITS:
  case OPR_INTCONST:
  case OPR_CONST:
  case OPR_LDA: 
  case OPR_LDA_LABEL:
    // nada
    return tree;

#ifdef TARG_ST
  case OPR_SUBPART:  // By symetry with other cases, we call recursively
                     // RT_LOWER_Expr that currently does nothing on a LDID.
    WN_kid0(new_nd) = RT_LOWER_expr (WN_kid0(tree));
    return new_nd;
#endif

  case OPR_ILOAD:
  case OPR_ILDBITS:
  case OPR_MLOAD:

    WN_kid0(new_nd) = RT_LOWER_expr (WN_kid0(tree));
    return new_nd;

  case OPR_NEG:

#ifdef TARG_ST
    if (MTYPE_is_longlong(res) && WN_Is_Emulated(tree)) 
#else
    if (Only_32_Bit_Ops && MTYPE_is_longlong(res)) 
#endif
      {
      //
      // generate an intrinsic call:
      //
      intrinsic = OPCODE_To_INTRINSIC (RT_LOWER_opcode(tree));

      kids[0] = WN_CreateParm (res,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(res),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 1, kids);

      WN_Delete(tree);
      return nd;
    }

#ifdef TARG_ST
    if (MTYPE_is_float(res) && WN_Is_Emulated(tree)) 
#else
    if ((Only_32_Bit_Ops && MTYPE_is_double(res)) ||
	(Emulate_FloatingPoint_Ops && MTYPE_is_float(res))) 
#endif
      {
      //
      // generate (or same for F4)
      //
      //         F8kid0
      //       U8TAS
      //       U8INTCONST 0x8000000000000000
      //     U8BXOR
      //   F8TAS
      //
      // and then lower the BXOR
      //

      WN *wn1, *wn2, *wn;
      //      TY_IDX   ty_idx  = WN_ty(tree);
      TYPE_ID  mtype = MTYPE_is_double(res) ? MTYPE_U8 : MTYPE_U4;

      wn1 = WN_Tas(mtype, Be_Type_Tbl(mtype), WN_kid0(tree));

      // Fabricate the right constant:
      INT64 tval = MTYPE_is_double(res) ? 0x8000000000000000LL: 0x80000000;
      wn2 = WN_Intconst(mtype, tval);

      wn = WN_Bxor(mtype, wn1, wn2);
      nd = WN_Tas(res, Be_Type_Tbl(res), wn);
      WN_Delete(tree);

      //
      // This will lower BXOR and the kid0
      //
      RT_LOWER_expr (nd);
      return nd;
    }

    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    return new_nd;

  case OPR_EXTRACT_BITS:
  case OPR_COMPOSE_BITS:
    //
    // These are not generated for our present targets, when they
    // are, we'll have to do something about it. We will need to
    // to have a intrinsic because emulating as a sequence of
    // shifts will be simplified in the wn_simp_code.h ... back
    // into extract/compose and we have an infinite loop.
    //
    // The ST240 has indeed a extract operator, thus do not generate 
    // the assert for extract operator
    //
    if (opr == OPR_EXTRACT_BITS && Enable_extract ||
        opr == OPR_COMPOSE_BITS && Enable_compose) {
      RT_LOWER_set_kid0(new_nd, RT_LOWER_expr(WN_kid0(tree)));
    } else {
      FmtAssert(FALSE,("shouldn't see extract/compose operator")); 	     
    }
    return new_nd;

  case OPR_LROTATE:
    if (Enable_Rotate){
      RT_LOWER_set_kid0(new_nd, RT_LOWER_expr(WN_kid0(tree)));
      RT_LOWER_set_kid1(new_nd, RT_LOWER_expr(WN_kid1(tree)));
    } else {
      FmtAssert(FALSE,("shouldn't see rotate-left operator")); 	     
    }
    return new_nd;
    
  case OPR_RROTATE:
    if (Enable_Rotate){
      RT_LOWER_set_kid0(new_nd, RT_LOWER_expr(WN_kid0(tree)));
      RT_LOWER_set_kid1(new_nd, RT_LOWER_expr(WN_kid1(tree)));
    } else {
      FmtAssert(FALSE,("shouldn't see rotate-right operator")); 	     
    }
    return new_nd;
    

  case OPR_BNOT: 
  case OPR_LNOT:

    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    return new_nd;

  case OPR_BAND: 
  case OPR_BIOR: 
  case OPR_BNOR: 
  case OPR_BXOR:

  case OPR_LAND: 
  case OPR_LIOR:
  case OPR_CAND: 
  case OPR_CIOR:
    // should be implemented without runtime function call
    RT_LOWER_set_kid0(new_nd, RT_LOWER_expr(WN_kid0(tree)));
    RT_LOWER_set_kid1(new_nd, RT_LOWER_expr(WN_kid1(tree)));
    return new_nd;

  case OPR_PARM:
    // just lower the kid but leave the PARM alone
    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    return new_nd;

  case OPR_TAS:
    {
      WN *wn1 = RT_LOWER_expr(WN_kid0(tree));
      // Try to simplify: we could have a TAS->TAS for example
      nd = WN_SimplifyExp1(WN_opcode(tree), wn1);
      if (nd != NULL) {
	//WN_Delete(tree);
	// may need to lower this:
	nd = RT_LOWER_expr(nd);
	return nd;
      }
      WN_kid0(new_nd) = wn1;
      return new_nd;
    }

  case OPR_CVTL:
    // can only apply to long long
    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    return new_nd;

  case OPR_CVT:
    //
    // For some reason, it is possible that we encounter I8U8CVT or
    // U8I8CVT (generated by the fornt-end. Remove it here, so that
    // we don't have to deal with it.
    //
    // TODO: The reason is probably WHIRL_Keep_Cvt_On = TRUE, check
    //       this out.
    //
    if (RT_LOWER_opcode(tree) == OPC_I8U8CVT || 
	RT_LOWER_opcode(tree) == OPC_U8I8CVT) {
      nd = RT_LOWER_expr(WN_kid0(tree));
      WN_Delete(tree);
      return nd;
    }

    //
    // I4U8CVT needs to lower the kid but leave the CVT, lower_hilo_expr()
    // will deal with it in wn_lower.cxx
    //
    if (RT_LOWER_opcode(tree) == OPC_I4U8CVT ||
	RT_LOWER_opcode(tree) == OPC_U4U8CVT ||
	RT_LOWER_opcode(tree) == OPC_I4I8CVT ||
	RT_LOWER_opcode(tree) == OPC_U4I8CVT ||
	RT_LOWER_opcode(tree) == OPC_U8U4CVT ||
	RT_LOWER_opcode(tree) == OPC_I8I4CVT ||
	RT_LOWER_opcode(tree) == OPC_I8U4CVT ||
	RT_LOWER_opcode(tree) == OPC_U8I4CVT) {
      WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
      return new_nd;
    }

#ifdef TARG_ST
    if (WN_Is_Emulated(tree)) 
#else
    if ((Only_32_Bit_Ops && (MTYPE_is_longlong(res) || MTYPE_is_longlong(desc) || MTYPE_is_double(res) || MTYPE_is_double(desc))) ||
	(Emulate_FloatingPoint_Ops && (MTYPE_is_float(res) || MTYPE_is_float(desc)))) 
#endif
      {

      intrinsic = OPCODE_To_INTRINSIC (RT_LOWER_opcode(tree));

      FmtAssert(intrinsic != INTRINSIC_INVALID, ("invalid intrinsic"));

      kids[0] = WN_CreateParm (desc,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(desc),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 1, kids);

      WN_Delete(tree);
      return nd;
    }

    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    return new_nd;

  case OPR_ABS:

    FmtAssert(MTYPE_signed_type(res), ("ABS for unsigned type"));

#ifdef TARG_ST
    if (MTYPE_is_longlong(res) && WN_Is_Emulated(tree))  
#else
    if (Only_32_Bit_Ops && res == MTYPE_I8)
#endif
      {
      intrinsic = OPCODE_To_INTRINSIC (WN_opcode(tree));

      FmtAssert(intrinsic != INTRINSIC_INVALID, ("invalid intrinsic"));

      kids[0] = WN_CreateParm (res,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(res),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 1, kids);

      WN_Delete(tree);
      return nd;
    }

#ifdef TARG_ST
    else if (MTYPE_is_float(res) && WN_Is_Emulated(tree)) 
#else
    else if (Emulate_FloatingPoint_Ops && MTYPE_is_float(res)) 
#endif
	{
      //
      // generate nd = tree & 0x7fffffff ffffffff
      //
      // (same for F4):
      //
      //        kid0
      //      U8TAS
      //      U8INTCONST 0x7fffffffffffffff
      //    F8BAND
      //  F8TAS

      WN *wn1, *wn2, *wn;
      TYPE_ID  mtype = MTYPE_is_double(res) ? MTYPE_U8 : MTYPE_U4;

      wn1 = WN_Tas(mtype, Be_Type_Tbl(mtype), WN_kid0(tree));

      // Fabricate the right constant:
      INT64 tval = MTYPE_is_double(res) ? 0x7fffffffffffffffLL: 0x7fffffff;
      wn2 = WN_Intconst(mtype, tval);

      wn = WN_Band(mtype, wn1, wn2);
      nd = WN_Tas(res, Be_Type_Tbl(res), wn);
      WN_Delete(tree);

      //
      // This will lower AND and the kid0
      //
      RT_LOWER_expr (nd);
      return nd;
    }

    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    return new_nd;

  case OPR_ADD: 
  case OPR_SUB: 
  case OPR_MPY:


#ifdef TARG_ST
    if (WN_Is_Emulated(tree)) 
#else
    if ((Emulate_FloatingPoint_Ops && MTYPE_is_float(res)) ||
	(Only_32_Bit_Ops && (MTYPE_is_longlong(res) || MTYPE_is_double(res))))
#endif
      {

#ifdef TARG_ST
      // [CG]: Call WN_To_INTRINSIC instead of OPCODE_To_INTRINSIC
      // as tree based selection may strength reduce operations
      // Defined on ST targets in targinfo.
      kids[0] = RT_LOWER_expr(WN_kid0(tree));
      kids[1] = RT_LOWER_expr(WN_kid1(tree));
      intrinsic = WN_To_INTRINSIC(RT_LOWER_opcode(tree), kids);
      kids[0] = WN_CreateParm (WN_rtype(kids[0]),
			       kids[0],
			       Be_Type_Tbl(WN_rtype(kids[0])),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      kids[1] = WN_CreateParm (WN_rtype(kids[1]),
			       kids[1],
			       Be_Type_Tbl(WN_rtype(kids[1])),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);
	
#else
      intrinsic = OPCODE_To_INTRINSIC (RT_LOWER_opcode(tree));

      kids[0] = WN_CreateParm (res,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(res),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      kids[1] = WN_CreateParm (res,
			       RT_LOWER_expr(WN_kid1(tree)),
			       Be_Type_Tbl(res),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);
#endif
      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 2, kids);

      WN_Delete(tree);
      return nd;
    }

    // If we did not generate an intrinsic, at least lower the kids
    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    WN_kid1(new_nd) = RT_LOWER_expr(WN_kid1(tree));
    return new_nd;

    // [HK] add code for MADD, MSUB
  case OPR_MADD:
  case OPR_MSUB:
#ifdef TARG_ST
    if (WN_Is_Emulated(tree))
#else
    if ((Only_32_Bit_Ops && MTYPE_is_size_double(desc)) ||
	(Emulate_FloatingPoint_Ops && MTYPE_is_float(desc)))
#endif
	{
      TYPE_ID parm0ty = WN_rtype(WN_kid0(tree));
      TYPE_ID parm1ty = WN_rtype(WN_kid1(tree));
      TYPE_ID parm2ty = WN_rtype(WN_kid2(tree));

      intrinsic = OPCODE_To_INTRINSIC (WN_opcode(tree));

      Is_True(intrinsic != INTRINSIC_INVALID,("invalid intrinsic for MADD"));

      kids[0] = WN_CreateParm (parm0ty,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(parm0ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      kids[1] = WN_CreateParm (parm1ty,
			       RT_LOWER_expr(WN_kid1(tree)),
			       Be_Type_Tbl(parm1ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      kids[2] = WN_CreateParm (parm2ty,
			       RT_LOWER_expr(WN_kid2(tree)),
			       Be_Type_Tbl(parm2ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 3, kids);

      WN_Delete(tree);
      return nd;
    }

    // If we did not generate an intrinsic, at least lower the kids
    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    WN_kid1(new_nd) = RT_LOWER_expr(WN_kid1(tree));
    WN_kid2(new_nd) = RT_LOWER_expr(WN_kid2(tree));
    return new_nd;

    // [HK] add code for NMADD, NMSUB
  case OPR_NMADD:
  case OPR_NMSUB:
#ifdef TARG_ST
    if (WN_Is_Emulated(tree))
#else
    if ((Only_32_Bit_Ops && MTYPE_is_size_double(desc)) ||
	(Emulate_FloatingPoint_Ops && MTYPE_is_float(desc)))
#endif
	{
      TYPE_ID parm0ty = WN_rtype(WN_kid0(tree));
      TYPE_ID parm1ty = WN_rtype(WN_kid1(tree));
      TYPE_ID parm2ty = WN_rtype(WN_kid2(tree));

      intrinsic = OPCODE_To_INTRINSIC (WN_opcode(tree));

      Is_True(intrinsic != INTRINSIC_INVALID,("invalid intrinsic for MADD"));

      kids[0] = WN_CreateParm (parm0ty,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(parm0ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      kids[1] = WN_CreateParm (parm1ty,
			       RT_LOWER_expr(WN_kid1(tree)),
			       Be_Type_Tbl(parm1ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      kids[2] = WN_CreateParm (parm2ty,
			       RT_LOWER_expr(WN_kid2(tree)),
			       Be_Type_Tbl(parm2ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 3, kids);

      WN_Delete(tree);
      return nd;
    }

    // If we did not generate an intrinsic, at least lower the kids
    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    WN_kid1(new_nd) = RT_LOWER_expr(WN_kid1(tree));
    WN_kid2(new_nd) = RT_LOWER_expr(WN_kid2(tree));
    return new_nd;
	

  case OPR_EQ: 
  case OPR_NE: 
  case OPR_GE: 
  case OPR_GT: 
  case OPR_LE: 
  case OPR_LT:

#ifdef TARG_ST
    if (WN_Is_Emulated(tree))
#else
    if ((Only_32_Bit_Ops && MTYPE_is_size_double(desc)) ||
	(Emulate_FloatingPoint_Ops && MTYPE_is_float(desc)))
#endif
      {
#ifdef TARG_ST
      if ( (opr == OPR_LT || opr == OPR_GE) &&
	     MTYPE_is_longlong(desc) && MTYPE_signed_type(desc) &&
	     WN_operator_is(WN_kid1(tree), OPR_INTCONST) &&
	     WN_const_val(WN_kid1(tree)) == 0)
	{
	  //
	  // for I8LT/GE with kid1 0, generate:
	  //       
	  //        kid0
	  //        I4INTCONST 32
	  //      I8ASHR
	  //    I4I8CVT
	  //    kid1
	  //  I4LT/GE

	  WN *wn1, *wn2;
	  TYPE_ID  mtype = MTYPE_I4;

	  wn1 = WN_Ashr(desc, WN_kid0(tree), WN_Intconst(mtype, 32));
	  wn2 = WN_Cvt(desc, mtype, wn1);
	  nd = WN_CreateExp2(opr, mtype, mtype, wn2, WN_Intconst(mtype, 0));
	  WN_Delete(tree);
	  
	  RT_LOWER_expr (nd);
	  return nd;
	} 
#endif

#ifdef TARG_ARM
      if((MTYPE_is_double(desc) || desc == MTYPE_F4) && opr == OPR_NE) {
	intrinsic = OPCODE_To_INTRINSIC (OPCODE_make_op(OPR_EQ,res,desc));
      }
      else {
	intrinsic = OPCODE_To_INTRINSIC (RT_LOWER_opcode(tree));
      }
#else
      intrinsic = OPCODE_To_INTRINSIC (RT_LOWER_opcode(tree));
#endif

      kids[0] = WN_CreateParm (desc,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(desc),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      kids[1] = WN_CreateParm (desc,
			       RT_LOWER_expr(WN_kid1(tree)),
			       Be_Type_Tbl(desc),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      //
      // It is possible that WHIRL operator has I8/U8 as its rtype --
      // it happens when programmer manipulates the long long type.
      // Since we're emulating long longs, we must generate I4/U4
      // instead.
      //
      TYPE_ID ty = (res == MTYPE_B) ? 
	          res : (MTYPE_signed_type(res) ? MTYPE_I4 : MTYPE_U4);
      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      ty, 
					      MTYPE_V),
			       intrinsic, 2, kids);

      if (MTYPE_byte_size(res) > MTYPE_byte_size(ty)) {
	nd = WN_Cvt(ty, res, nd);
      }

      WN_Delete(tree);
#ifdef TARG_ARM
	if((MTYPE_is_double(desc) || desc == MTYPE_F4) && opr == OPR_NE) {
	  return WN_LNOT(nd);
	}
	else {
	  return nd;
	}
#else
      return nd;
#endif
    }

    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    WN_kid1(new_nd) = RT_LOWER_expr(WN_kid1(tree));
    return new_nd;

  case OPR_DIV: 

#ifdef TARG_ST
    if (WN_Is_Emulated(tree)) {
#else
    if (Should_Call_Divide(tree)) {
#endif
      TYPE_ID parm0ty = WN_rtype(WN_kid0(tree));
      TYPE_ID parm1ty = WN_rtype(WN_kid1(tree));

      intrinsic = OPCODE_To_INTRINSIC (WN_opcode(tree));

      Is_True(intrinsic != INTRINSIC_INVALID,("invalid intrinsic for DIV"));

      kids[0] = WN_CreateParm (parm0ty,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(parm0ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      kids[1] = WN_CreateParm (parm1ty,
			       RT_LOWER_expr(WN_kid1(tree)),
			       Be_Type_Tbl(parm1ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 2, kids);

      WN_Delete(tree);

      return nd;
    }

    //
    // Otherwise lower children and ignore
    //
    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    WN_kid1(new_nd) = RT_LOWER_expr(WN_kid1(tree));
    return new_nd;

  case OPR_MOD: 
  case OPR_REM:

#ifdef TARG_ST
    if (WN_Is_Emulated(tree)) {
#else
    if (Should_Call_Remainder(tree)) {
#endif
      TYPE_ID parm0ty = WN_rtype(WN_kid0(tree));
      TYPE_ID parm1ty = WN_rtype(WN_kid1(tree));

      intrinsic = OPCODE_To_INTRINSIC (WN_opcode(tree));

      Is_True(intrinsic != INTRINSIC_INVALID,("invalid intrinsic for REM"));

      kids[0] = WN_CreateParm (parm0ty,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(parm0ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      kids[1] = WN_CreateParm (parm1ty,
			       RT_LOWER_expr(WN_kid1(tree)),
			       Be_Type_Tbl(parm1ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 2, kids);

      WN_Delete(tree);
      return nd;
    }

    //
    // Otherwise lower children and ignore
    //
    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    WN_kid1(new_nd) = RT_LOWER_expr(WN_kid1(tree));
    return new_nd;

  case OPR_DIVREM:
    FmtAssert(FALSE,("can't handle operator %s", OPERATOR_name(opr)));
    return new_nd;

  case OPR_SHL: 
  case OPR_ASHR: 
  case OPR_LSHR:
    // These are only good for long long
#ifdef TARG_ST
    if (MTYPE_is_longlong(res) && WN_Is_Emulated(tree))
#else
    if (Only_32_Bit_Ops && MTYPE_is_longlong(res)) 
#endif
      {

      WN *kid1 = WN_kid1(tree);
      TYPE_ID parm1ty = WN_rtype(kid1);
      // Arg 2 of shifts is U4, insert a cvt if needed
      if (MTYPE_size_min(parm1ty) > 32) {
	kid1 = WN_Cvt(parm1ty, MTYPE_U4, kid1);
	parm1ty = WN_rtype(kid1);
      }

      intrinsic = OPCODE_To_INTRINSIC (WN_opcode(tree));

      kids[0] = WN_CreateParm (res,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(res),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      kids[1] = WN_CreateParm (parm1ty,
			       RT_LOWER_expr(kid1),
			       Be_Type_Tbl(parm1ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 2, kids);

      WN_Delete(tree);
      return nd;
    }

    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    WN_kid1(new_nd) = RT_LOWER_expr(WN_kid1(tree));
    return new_nd;

  case OPR_MAX: 
  case OPR_MIN: 
#ifdef TARG_ST
    if (WN_Is_Emulated(tree))
#else
    if ((Only_32_Bit_Ops && (MTYPE_is_longlong(res) || MTYPE_is_double(res))) ||
	(Emulate_FloatingPoint_Ops && MTYPE_is_float(res))) 
#endif
      {

      TYPE_ID parm0ty = WN_rtype(WN_kid0(tree));
      TYPE_ID parm1ty = WN_rtype(WN_kid1(tree));

      intrinsic = OPCODE_To_INTRINSIC (WN_opcode(tree));

      Is_True(intrinsic != INTRINSIC_INVALID,("invalid intrinsic for DIV"));

      kids[0] = WN_CreateParm (parm0ty,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(parm0ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      kids[1] = WN_CreateParm (parm1ty,
			       RT_LOWER_expr(WN_kid1(tree)),
			       Be_Type_Tbl(parm1ty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 2, kids);

      WN_Delete(tree);
      return nd;
    }

    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    WN_kid1(new_nd) = RT_LOWER_expr(WN_kid1(tree));
    return new_nd;

  case OPR_SQRT: 

#ifdef TARG_ST
    if (WN_Is_Emulated(tree))
#else
    if (Emulate_FloatingPoint_Ops && MTYPE_is_float(res)) 
#endif
      {
      intrinsic = OPCODE_To_INTRINSIC (WN_opcode(tree));

      Is_True(intrinsic != INTRINSIC_INVALID,("invalid intrinsic for SQRT"));

      TYPE_ID parmty = WN_rtype(WN_kid0(tree));
      kids[0] = WN_CreateParm (parmty,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(parmty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 1, kids);

      WN_Delete(tree);
      return nd;
    }

    // At least lower the kid
    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    return new_nd;

  case OPR_RECIP: 
  case OPR_RSQRT: 

#ifdef TARG_ST
    if (WN_Is_Emulated(tree))
#else
      if (Emulate_FloatingPoint_Ops && MTYPE_is_float(res)) 
#endif
      {  
	if (WN_rtype(tree) == MTYPE_F4)
      {
      intrinsic = OPCODE_To_INTRINSIC (WN_opcode(tree));

      Is_True(intrinsic != INTRINSIC_INVALID,("invalid intrinsic for RECIP"));

      TYPE_ID parmty = WN_rtype(WN_kid0(tree));
      kids[0] = WN_CreateParm (parmty,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(parmty),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 1, kids);

      WN_Delete(tree);
      return nd;
      }
      }
    // At least lower the kid
    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    return new_nd;

  case OPR_PAREN:

  case OPR_RND: 
  case OPR_CEIL: 
  case OPR_FLOOR:

  case OPR_MINPART: 
  case OPR_MAXPART:
  case OPR_MINMAX:

  case OPR_ALLOCA:
    //
    // If this is not a hilo type or we're not emulating, get out
    //
#ifdef TARG_ST
    if (!WN_Is_Emulated(tree)) {
      WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
      return new_nd;
    }
#else
    if (!(Emulate_FloatingPoint_Ops && MTYPE_is_float(res)) && 
	!(Only_32_Bit_Ops && (MTYPE_is_longlong(res) || MTYPE_is_double(res))))
      return tree;
#endif

    FmtAssert(FALSE,("can't handle operator %s", OPERATOR_name(opr)));
    return new_nd;

//   case OPR_RECIP:
//     Is_True (Recip_Allowed, ("invalid recip opr"));

//     WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
//     return new_nd;

  case OPR_TRUNC: 
#ifdef TARG_ST
    if (MTYPE_is_float(res) && WN_Is_Emulated(tree) ||
	MTYPE_is_float(desc) && WN_Is_Emulated(tree))
#else
    if (Emulate_FloatingPoint_Ops) 
#endif
      {

      intrinsic = OPCODE_To_INTRINSIC (WN_opcode(tree));

      Is_True(intrinsic != INTRINSIC_INVALID,("invalid intrinsic for TRUNC"));

      kids[0] = WN_CreateParm (desc,
			       RT_LOWER_expr(WN_kid0(tree)),
			       Be_Type_Tbl(desc),
			       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);

      nd = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					      res, 
					      MTYPE_V),
			       intrinsic, 1, kids);

      WN_Delete(tree);
      return nd;
    }

    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    return new_nd;

  case OPR_ARRAY:
    //
    // Lower the index expressions for all dimensions
    //
    {
      INT i;
      for (i = 0; i < WN_num_dim(tree); i++) {
	WN_array_index(new_nd,i) = RT_LOWER_expr(WN_array_index(tree,i));
      }
    }
    return new_nd;

  case OPR_REALPART: 
  case OPR_IMAGPART:
#if 0
    // lower the kid 0
    WN_kid0(new_nd) = RT_LOWER_expr(WN_kid0(tree));
    return new_nd;
#endif

  case OPR_HIGHPART: 
  case OPR_LOWPART:
  case OPR_COMPLEX:
  default:	
    Is_True(FALSE,("unexpected operator %s", OPERATOR_name(opr)));
  }
  return NULL;
}

/* ====================================================================
 * lower the given statement to run-time intrinsic in target ISA.
 * ==================================================================== */
static void
RT_LOWER_stmt_wn(WN *tree)
{
  OPERATOR opr = WN_operator(tree);
  TYPE_ID desc = WN_desc(tree);
  TYPE_ID res = WN_rtype(tree);
  INT i;

  switch (opr) {

  // operators with no expression as kid
  case OPR_GOTO:
  case OPR_GOTO_OUTER_BLOCK:
  case OPR_RETURN:
  case OPR_COMMENT:
  case OPR_TRAP	:
  case OPR_FORWARD_BARRIER:
  case OPR_BACKWARD_BARRIER:
  case OPR_ALTENTRY:
  case OPR_PRAGMA:
  case OPR_LABEL:
  case OPR_REGION_EXIT:
    return;

#ifdef TARG_ST
    /* (cbr) kid2 containts region's body */
  case OPR_REGION:
    RT_LOWER_stmt_wn(WN_kid2(tree));
    break;
#endif

  // only kid 0 contains no node relevant to RT lowering
  case OPR_DEALLOCA:	
  case OPR_PREFETCH:
  case OPR_EVAL: 
  case OPR_AGOTO:
#ifdef TARG_ST
  case OPR_AFFIRM:
#endif
    // don't need to do anything
    WN_kid0(tree) = RT_LOWER_expr(WN_kid0(tree));
    return;

  case OPR_TRUEBR:
  case OPR_FALSEBR:
  case OPR_ASSERT:
    WN_kid0(tree) = RT_LOWER_expr(WN_kid0(tree));
    return;

  case OPR_ISTORE:
    WN_kid1(tree) = RT_LOWER_expr(WN_kid1(tree));
  // fall through

  case OPR_STID:
    WN_kid0(tree) = RT_LOWER_expr(WN_kid0(tree));
    //Is_True(desc != MTYPE_B, ("illegal mtype B"));
    return;

  case OPR_ISTBITS:
    WN_kid1(tree) = RT_LOWER_expr(WN_kid1(tree));
  // fall through

  case OPR_STBITS:
    WN_kid0(tree) = RT_LOWER_expr(WN_kid0(tree));
    return;

  case OPR_RETURN_VAL:
    WN_kid0(tree) = RT_LOWER_expr(WN_kid0(tree));
    return;

  case OPR_CALL:          
  case OPR_ICALL:
  case OPR_INTRINSIC_CALL:
    for (i = 0; i < WN_kid_count(tree); i++) 
      WN_kid(tree,i) = RT_LOWER_expr(WN_kid(tree,i));
    return;

  case OPR_PICCALL:
    for (i = 0; i < (WN_kid_count(tree)-1); i++) 
      WN_kid(tree,i) = RT_LOWER_expr(WN_kid(tree,i));
    return;

  case OPR_ASM_STMT:
    for (i = 2; i < WN_kid_count(tree); i++) {
      WN_kid0(WN_kid(tree,i)) =
	RT_LOWER_expr(WN_kid0(WN_kid(tree,i)));
    }
    return;

  case OPR_COMPGOTO:
  case OPR_XGOTO:
    WN_kid0(tree) = RT_LOWER_expr(WN_kid0(tree));
    return;

  case OPR_MSTORE:
    WN_kid0(tree) = RT_LOWER_expr(WN_kid0(tree));
    WN_kid1(tree) = RT_LOWER_expr(WN_kid1(tree));
    WN_kid2(tree) = RT_LOWER_expr(WN_kid2(tree));
    return;

  case OPR_XPRAGMA:
    WN_kid0(tree) = RT_LOWER_expr(WN_kid0(tree));
    return;

  case OPR_LOOP_INFO:
    if (WN_kid1(tree) != NULL) {
      WN_kid1(tree) = RT_LOWER_expr(WN_kid1(tree));
    }
    return;

  // structured control flow statements

  case OPR_BLOCK: {
    WN *stmt;
    for (stmt = WN_first(tree); stmt; stmt = WN_next(stmt)) {
      Is_True(OPERATOR_is_stmt(WN_operator(stmt)) || OPERATOR_is_scf(WN_operator(stmt)),
	      ("statement operator expected"));
      RT_LOWER_stmt_wn(stmt);
    }
    return;
  }

  case OPR_DO_LOOP:
    RT_LOWER_stmt_wn(WN_kid(tree, 1)); // the initialization statement
    WN_kid2(tree) = RT_LOWER_expr(WN_kid2(tree));
    RT_LOWER_stmt_wn(WN_kid(tree, 3)); // the increment statement
    RT_LOWER_stmt_wn(WN_kid(tree, 4)); // the block
    if (WN_kid(tree, 5) != NULL)
      RT_LOWER_stmt_wn(WN_kid(tree, 5)); // LOOP_INFO
    return;

  case OPR_DO_WHILE:
  case OPR_WHILE_DO:
    WN_kid0(tree) = RT_LOWER_expr(WN_kid0(tree));
    RT_LOWER_stmt_wn(WN_kid(tree, 1));
    return;

  case OPR_IF:
    WN_kid0(tree) = RT_LOWER_expr(WN_kid0(tree));
    RT_LOWER_stmt_wn(WN_kid(tree, 1));
    RT_LOWER_stmt_wn(WN_kid(tree, 2));
    return;

  default:	
    Is_True(FALSE,("unexpected operator %s", OPERATOR_name(opr)));
  }

  return;  
}

/* ====================================================================
 * Top level routine for lowering OPERATORs not supported in the target
 * ISA to INTRINSIC_OPs corresponding to target's runtime support.
 * L WHIRL assumed.
 * ==================================================================== */
void
RT_lower_wn(WN *tree)
{
  Start_Timer(T_Lower_CU);
  Set_Error_Phase("RT Lowering");

  WN_Lower_Checkdump("RT Lowering", tree, 0);

  if (WN_operator(tree) == OPR_FUNC_ENTRY) 
    RT_LOWER_stmt_wn(WN_func_body(tree));
  else if (WN_operator(tree) == OPR_REGION) 
    RT_LOWER_stmt_wn(WN_region_body(tree));
  else if (OPERATOR_is_stmt(WN_operator(tree)) || OPERATOR_is_scf(WN_operator(tree)))
    RT_LOWER_stmt_wn(tree);
  else Is_True(FALSE, ("unexpected WHIRL operator"));

  Stop_Timer(T_Lower_CU);

  WN_Lower_Checkdump("After RT lowering", tree, 0);   

  WN_verifier(tree);

  return;
}

#ifdef TARG_ST
/* ====================================================================
 *
 * Entry points exported for local use by wn_lower.cxx.
 * 
 * These entry points are declared in wn_lower_private.h and should
 * not be used from outside of wn_lower.cxx and related files.
 * For use outside of the wn_lower utilities, call RT_lower_wn().
 *
 * The usage of these private functiosn if for wn_lower.cxx, in particular
 * when lowering store/load of bitfields, this may generate arithmetic
 * that require a runtime lowering on some targets.
 * A typical sequence is: 
 * lower_bit_field_id()      // may generate STBITS
 *   -> lower_store_bits()   // may generate U8SHR that is not native
 *     -> rt_lower_stmt()    // may call runtime
 *       -> lower_store()    // complete lowering
 *
 * WN *rt_lower_expr(WN *, LOWER_ACTIONS)
 *   perform runtime lowering for the given expression and returns the
 *   new expression.
 *
 * void rt_lower_stmt(WN *, LOWER_ACTIONS)
 *   perform runtime lowering for the given statement, the statement
 *   is modified in place. 
 *   Currently limited to stmt only (no scf,region or function) as 
 *   the use of this function is supposed to be local to some other
 *   lowering transformation.
 *
 * In both cases the lowering is actually performed only if the 
 * LOWER_RUNTIME action is active. This action must be activated
 * by the main lowering driver at CG and HILO lowering time,
 * i.e. after and only after the actual RT_lower_wn() phase.
 * This allows client of this interface to use it in any context 
 * (pre or post runtime lowering).
 *
 * Note: currently, in both cases the actions argument is used only to
 * detect the LOWER_RUNTIME action. Other actions are not applied.
 * This is the initial philosophy or RT_lower that only does the runtime 
 * lowering and nothing else. This could be changed for better integration
 * with the lowering, but then, there may be some phasing problems.
 * Thus, for now, the client must take care of running an additional 
 * lowering phase after the runtime lowering, the typical sequence is:
 *   rt_lower_stmt(wn, OnlyAction(actions, LOWER_RUNTIME));
 *   wn = lower_stmt(wn, block, actions)
 * If integrated with the lowering, this could at the end be simplified into:
 *   wn = rt_lower_stmt(wn, block, actions); 
 *
 * ==================================================================== */
void
rt_lower_stmt(WN *stmt, LOWER_ACTIONS actions)
{
  Is_True(OPERATOR_is_stmt(WN_operator(stmt)), ("%s: stmt nodes only", __FUNCTION__));
  
  if (actions & LOWER_RUNTIME) {
    WN_Lower_Checkdump("Local expr RT Lowering", stmt, 0);
    RT_LOWER_stmt_wn(stmt);
    WN_Lower_Checkdump("After Local expr RT lowering", stmt, 0);   
    WN_verifier(stmt);
  }
  
}

WN *
rt_lower_expr(WN *tree, LOWER_ACTIONS actions)
{
  if (actions & LOWER_RUNTIME) {
    WN_Lower_Checkdump("Local stmt RT Lowering", tree, 0);
    tree = RT_LOWER_expr(tree);
    WN_Lower_Checkdump("After Local stmt RT lowering", tree, 0);   
  }
  return tree;
}
#endif
