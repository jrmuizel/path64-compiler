/*

  Copyright (C) 2005-2006 ST Microelectronics, Inc.  All Rights Reserved.

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

  Contact information:  ST Microelectronics, Inc., 
  address, or:

  http://www.st.com

*/

/*
 * Lowering of general constant multiply.
 *
 * The implementation is the simple binary implementation with
 * handling of 1-bit runs through Booth renaming. 
 * The implementation is linear and uses a cost model to determine
 * once the sequence is generated the benefit of doing the transformation.
 * It does not use the cost model to drive the sequence generation nor
 * uses factorization as in the arithmetic algorithm of [Bernstein],[Briggs]
 * that on the counterpart is exponential. 
 * Though this simple algorithm is considered effective enough compared
 * a loop based binary algorithm. 
 * The average sequence length for the set of 16 bits signed integer is:
 * - simple binary method (BM):	14.50 ops
 * - BM + Booth left to rigth:	10.75 ops
 * - BM + Booth rigth to left:   9.72 ops (this implementation)
 * - Bernstein:			 8.96 ops (ref [Lefevre])
 *
 * For instance the case n x 113:
 * 113 is 1110001 noted PPP000P
 * transformed with Booth renaming into P00N000P
 * Giving:
 * t1 = n << 3
 * t2 = t1 - n
 * t3 = t2 << 4
 * t4 = t3 + n
 *
 * For instance the case n x 27:
 * 27 is 11011 noted PP0PP
 * transformed with Booth renaming into P00N0N
 * Giving:
 * t1 = n << 3
 * t2 = t1 - n
 * t3 = t2 << 2
 * t4 = t3 - n
 *
 *
 * Interface exported to wn_lower is:
 *
 * Is_Mul_By_Constant_Candidate()
 * Returns true if the wn is candidate for constant multiply.
 * check the target dependent informations.
 *
 * Lower_Mul_By_Constant()
 * Effectively performs the lowering of a mul by constant.
 *
 */

#include "defs.h"
#include "config.h"
#include "be_errors.h"
#include "wn.h"
#include "wn_simp.h"
#include "wn_util.h"
#include "const.h"
#include "be_symtab.h"
#include "w2op.h"
#include "wn_lower.h"
#include "wn_lower_private.h"
#include "wn_lower_mul.h"
#include "tracing.h"	// For TFile
#include "ir_reader.h"	// For fdump_tree()
#include "mul_cost_model.h"

/*
 * Utility defines.
 */
#define ABS(a) ((a) < 0 ? -(a): (a))


/*
 * Tracing support.
 */
/* Define to 1 or 2 for tracing level. Trace is outputed  into TFile. */
#define TRACE 0

#define MUL_TYPE UINT64
#define MUL_BITS sizeof(MUL_TYPE)*8

typedef struct {
  INT opr;
  INT value;
} MUL_OP_T;

#define MUL_OP_UNKNOWN 0
#define MUL_OP_SHIFT 1	/* x = y << value */
#define MUL_OP_ADDN 2	/* x = y + N */
#define MUL_OP_SUBN 3	/* x = y - N */
#define MUL_OP_ZERO 4	/* x = 0 */

typedef struct {
  INT num_ops;
  MUL_OP_T *ops;
} MUL_SEQ_T;

#define bit_at(val, b) (((val)>>b) & 1)

/* 
 * Buffer maintaining mul by constant sequences.
 * Number of operations is bounded by 2*(bit_size-1).
 */
static MUL_OP_T ops_buffer[2*(MUL_BITS-1)];
static MUL_SEQ_T mul_sequence = { 0, ops_buffer };

/*
 * Build_Mul_By_Constant()
 * Builds the mul by constant sequence.
 * The returned sequence can be used to check for benefit
 * and to generate the corresponding whirl code.
 * The returned buffer is a static buffer, thus only
 * one mul at a time can be processed.
 */
static MUL_SEQ_T
Build_Mul_By_Constant(TYPE_ID rtype, MUL_TYPE value)
{
  static signed char booth[MUL_BITS];
  MUL_OP_T *ops_ptr;
  INT num_ops;
  INT b, last_b;
  INT mul_width = MTYPE_byte_size(rtype)*8;

  num_ops = 0;
  ops_ptr = mul_sequence.ops;
  last_b = -1;
  
  if (value == 0) {
    ops_ptr[num_ops++] = (MUL_OP_T){ MUL_OP_ZERO };
    mul_sequence.num_ops = num_ops;
    return mul_sequence;
  } else if (value == 1) {
    ops_ptr[num_ops++] = (MUL_OP_T){ MUL_OP_ADDN };
    mul_sequence.num_ops = num_ops;
    return mul_sequence;
  }

  /* Prepare Booth representation. */
  for (b = 0; b < mul_width; b++) {
    if (bit_at(value, b) == 0) booth[b] = 0;
    else booth[b] = 1;
  }
  for (b = 0; b < mul_width; b++) {
    if (booth[b] != 0) {
      /* Compute length of 1 bit sequence. */
      int length_1;
      int b2;
      for (b2 = b+1; b2 < mul_width; b2++) {
	if (booth[b2] == 0) break;
      }
      length_1 = b2 - b;
      if (length_1 > 1) {
	booth[b] = -1;
	for (b2 = b+1; b2 < b + length_1; b2++) {
	  booth[b2] = 0;
	}
	if (b2 < mul_width) booth[b2] = 1;
	b = b + length_1 - 1;
      } else {
	booth[b] = 1;
      }
    }
  }
  /* Force most significant bit to 1 if 1. */
  if (booth[mul_width-1] == -1) booth[mul_width-1] = 1;


  for (b = mul_width - 1; b >= 0; b--) {
    if (booth[b] != 0) {
      if (last_b != -1) {
	int shift_amount = last_b - b;
	ops_ptr[num_ops++] = (MUL_OP_T){ MUL_OP_SHIFT, shift_amount };
	if (booth[b] == -1) {
	  ops_ptr[num_ops++] = (MUL_OP_T){ MUL_OP_SUBN };
	} else {
	  ops_ptr[num_ops++] = (MUL_OP_T){ MUL_OP_ADDN };
	}
      } else if (booth[b] == -1) {
	ops_ptr[num_ops++] = (MUL_OP_T){ MUL_OP_SUBN };
      }
      last_b = b;
    }
  }
  if (last_b > 0) {
    ops_ptr[num_ops++] = (MUL_OP_T){ MUL_OP_SHIFT, last_b };
  }
  mul_sequence.num_ops = num_ops;
  return mul_sequence;
}

static BOOL
Should_Do_Mul_By_Constant(MUL_COST_MODEL_T *cost_model, MUL_SEQ_T mul_seq)
{
  INT size_cost = 0, cycle_cost = 0;
  MUL_OP_T *ops_ptr = mul_seq.ops;
  INT num = 0;
  for (num = 0; num < mul_seq.num_ops; num++) {
    if (ops_ptr[num].opr == MUL_OP_SHIFT) {
      size_cost += cost_model->size.c_shift;
      cycle_cost += cost_model->cycles.c_shift;
    } else if (ops_ptr[num].opr == MUL_OP_ADDN) {
      size_cost += cost_model->size.c_add;
      cycle_cost += cost_model->cycles.c_add;
    } else if (ops_ptr[num].opr == MUL_OP_SUBN) {
      size_cost += cost_model->size.c_sub;
      cycle_cost += cost_model->cycles.c_sub;
    } else if (ops_ptr[num].opr == MUL_OP_ZERO) {
      size_cost += cost_model->size.c_cnst;
      cycle_cost += cost_model->cycles.c_cnst;
    }
  }
#if TRACE > 0
  fprintf(TFile, "Should_Do_Mul_By_Constant: costs: s_mul %d, c_mul %d, size: %d, cycles: %d\n", 
	  cost_model->size.c_mul, cost_model->cycles.c_mul,
	  size_cost, cycle_cost);
#endif

  /* Only candidate if cycle cost benefits. */
  if (cycle_cost <= cost_model->cycles.c_mul) {
#ifdef TARG_ST
    //TB:no more OPT_Space
    if (OPT_Mul_by_cst_threshold == 0) {
      /* For size, returns true only if both costs are lower than mul. */
      return size_cost <= cost_model->size.c_mul;
    } else if (OPT_Mul_by_cst_threshold >= 3) {
      /* For -O3, returns true if cost is lower than 3 times mul. */
      return size_cost <= cost_model->size.c_mul * 3;
    } else if (OPT_Mul_by_cst_threshold >= 2) {
      /* For -O2, returns true is cost is lower than 2 times mul. */
      return size_cost <= cost_model->size.c_mul * 2;
    } else if (OPT_Mul_by_cst_threshold >= 1) {
      /* For -O1, returns true if cost is lower than 1.5 times mul. */
      return 2 * size_cost <= cost_model->size.c_mul * 3;
    }
#else
    if (OPT_Space) {
      /* For size, returns true only if both costs are lower than mul. */
      return size_cost <= cost_model->size.c_mul;
    } else if (Opt_Level >= 3) {
      /* For -O3, returns true if cost is lower than 3 times mul. */
      return size_cost <= cost_model->size.c_mul * 3;
    } else if (Opt_Level >= 2) {
      /* For -O2, returns true is cost is lower than 2 times mul. */
      return size_cost <= cost_model->size.c_mul * 2;
    } else if (Opt_Level >= 1) {
      /* For -O1, returns true if cost is lower than 1.5 times mul. */
      return 2 * size_cost <= cost_model->size.c_mul * 3;
    }
#endif
  }
  return FALSE;
}


static BOOL 
Is_Power_Of_2 (
  INT64 val, 
  TYPE_ID mtype
)
{
  if (MTYPE_is_signed(mtype) && val < 0) val = -val;
  
  if (mtype == MTYPE_U4) val &= 0xffffffffull;
  
  return ((val != 0) && ((val & (val-1)) == 0));
}

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

  DevAssert(FALSE, ("Get_Power_Of_2 unexpected value"));
  /* NOTREACHED */
}


/*
 * Tracing functions
 */
#if TRACE > 0
static void
Print_Mul_Seq(MUL_SEQ_T mul_seq, FILE *file)
{
  MUL_OP_T *ops_ptr;
  INT num;
  INT tnum = 0;
  fprintf(file, "Mul sequence (%d ops):\n", mul_seq.num_ops);
  ops_ptr = mul_seq.ops;

  for (num = 0; num < mul_seq.num_ops; num++) {
    if (ops_ptr[num].opr == MUL_OP_SHIFT) {
      if (tnum == 0) {
	fprintf(file, "t%d = N << %d\n", tnum, ops_ptr[num].value);
      }	else {
	fprintf(file, "t%d = t%d << %d\n", tnum, tnum-1, ops_ptr[num].value);
      }
    } else if (ops_ptr[num].opr == MUL_OP_ADDN) {
      if (tnum == 0) {
	fprintf(file, "t%d = 0 + N\n", tnum);
      }	else {
	fprintf(file, "t%d = t%d + N\n", tnum, tnum-1);
      }
      
    } else if (ops_ptr[num].opr == MUL_OP_SUBN) {
      if (tnum == 0) {
	fprintf(file, "t%d = 0 - N\n", tnum);
      }	else {
	fprintf(file, "t%d = t%d - N\n", tnum, tnum-1);
      }
    } else if (ops_ptr[num].opr == MUL_OP_ZERO) {
      fprintf(file, "t%d = 0 \n", tnum);
    } else {
      DevAssert(0, ("Unexpected cnst_mul operator (pos %d): %d\n", num, ops_ptr[num].opr));
    }
    tnum++;
  }
}
#endif

/*************************************************************
 *
 * Exported interfaces follows.
 *
 *************************************************************/

/*
 * WNL_Is_Int_Mul_By_Constant()
 * 
 * See interface description.
 */
extern BOOL
WNL_Is_Int_Mul_By_Constant(WN *wn)
{
  OPERATOR opr = WN_operator(wn);
  switch (opr) {
  case OPR_MPY:
    {
      TYPE_ID type = WN_rtype(wn);
      if (!MTYPE_is_integral(WN_rtype(wn))) return FALSE;

      BOOL is_const0 = Is_Intconst_Val(WN_kid0(wn));
      BOOL is_const1 = Is_Intconst_Val(WN_kid1(wn));

      if (is_const0 && is_const1) {
	/* Don't consider a mul by constant if it's really a constant, it will
	   be folded. */
	return FALSE;
      }
      else if (is_const0 || is_const1) {
	return TRUE;
      }
    }
    break;
  }
  
  return FALSE;
}


/*
 * WNL_Is_Int_Mul_By_Constant_Candidate()
 * 
 * See interface description.
 */
BOOL
WNL_Is_Int_Mul_By_Constant_Candidate(WN *wn)
{
  MUL_COST_T *mul_cost;
  if (WNL_Is_Int_Mul_By_Constant(wn) && Opt_Level > 0) {
    TYPE_ID rtype = WN_rtype(wn);
    OPERATOR opr = WN_operator(wn);
    INT64 constval;
    if (Is_Intconst_Val(WN_kid0(wn))) {
      constval = Get_Intconst_Val(WN_kid0(wn));
    } else {
      constval = Get_Intconst_Val(WN_kid1(wn));
    }
    MUL_COST_MODEL_T *cost_model = Get_Mul_Cost_Model(rtype);
    if (cost_model != NULL) {
      MUL_SEQ_T mul_seq = Build_Mul_By_Constant(rtype, constval);
      if (Should_Do_Mul_By_Constant(cost_model, mul_seq)) {
#if TRACE > 0
	fprintf(TFile, "WNL_Is_Int_Mul_By_Constant_Candidate(%lld)\n", constval);
#endif
	return TRUE;
      }
    }
  }
  return FALSE;
}

/*
 * WNL_Lower_Int_Mul_By_Constant()
 * 
 * See interface description.
 */
WN *
WNL_Lower_Int_Mul_By_Constant(WN *block, WN *wn, LOWER_ACTIONS actions)
{
  DevAssert(WNL_Is_Int_Mul_By_Constant(wn), ("Unexpected non MUL by constant"));
  
  TYPE_ID rtype = WN_rtype(wn);
  OPERATOR opr = WN_operator(wn);
  INT64 constval;
  WN *kid, *kid0, *kid1;
  kid0 = WN_kid0(wn);
  kid1 = WN_kid1(wn);
  if (Is_Intconst_Val(WN_kid0(wn))) {
    constval = Get_Intconst_Val(kid0);
    kid = kid1;
  } else {
    constval = Get_Intconst_Val(kid1);
    kid = kid0;
  }

  // Must turn other operand into a leaf.
  // We must do this before computing the mul_seq as this function
  // can be called recursively from Make_leaf which itself calls
  // lower_expr().
  LEAF leaf_kid = Make_Leaf(block, kid, rtype);
  

  MUL_SEQ_T mul_seq = Build_Mul_By_Constant(rtype, constval);
#if TRACE > 0
  fprintf(TFile, "WNL_Lower_Int_Mul_By_Constant(%lld)\n", constval);
#if TRACE > 1
  Print_Mul_Seq(mul_seq, TFile);
#endif
#endif

  MUL_OP_T *ops_ptr = mul_seq.ops;
  INT num;
  WN *tmp_wn = NULL;
  for (num = 0; num < mul_seq.num_ops; num++) {
    if (ops_ptr[num].opr == MUL_OP_SHIFT) {
      if (num == 0) {
	tmp_wn = WN_Shl(rtype, Load_Leaf(leaf_kid), WN_Intconst(rtype, ops_ptr[num].value));
      } else {
	tmp_wn = WN_Shl(rtype, tmp_wn, WN_Intconst(rtype, ops_ptr[num].value));
      }
    } else if (ops_ptr[num].opr == MUL_OP_ADDN) {
      if (num == 0) {
	tmp_wn = Load_Leaf(leaf_kid);
      } else {
	tmp_wn = WN_Add(rtype, tmp_wn, Load_Leaf(leaf_kid));
      }
    } else if (ops_ptr[num].opr == MUL_OP_SUBN) {
      if (num == 0) {
	tmp_wn = WN_Neg(rtype, Load_Leaf(leaf_kid));
      } else {
	tmp_wn = WN_Sub(rtype, tmp_wn, Load_Leaf(leaf_kid));
      }
    } else if (ops_ptr[num].opr == MUL_OP_ZERO) {
      tmp_wn = WN_Intconst(rtype, 0);
    }
  }
#if TRACE > 0
#if TRACE > 1
  fprintf(TFile, "Lower_Int_Mul_By_constant(%lld), result of lowering:\n", constval);
  fdump_tree(TFile, tmp_wn);
#endif
  fprintf(TFile, "Lower_Int_Mul_By_constant(%lld) Done.\n", constval);
#endif
  return lower_expr(block, tmp_wn, actions);
}


/*
 * WNL_Is_Fast_Int_Mul_By_Constant_Candidate()
 * 
 * See interface description.
 */
BOOL
WNL_Is_Fast_Int_Mul_By_Constant_Candidate(WN *wn)
{
  MUL_COST_T *mul_cost;
  if (WNL_Is_Int_Mul_By_Constant(wn)) {
    TYPE_ID rtype = WN_rtype(wn);
    OPERATOR opr = WN_operator(wn);
    INT64 constval;
    if (Is_Intconst_Val(WN_kid0(wn))) {
      constval = Get_Intconst_Val(WN_kid0(wn));
    } else {
      constval = Get_Intconst_Val(WN_kid1(wn));
    }
    /* Can_Do_Fast_Multiply() is defined in the targinfo part. */
    if (opr == OPR_MPY && Can_Do_Fast_Multiply(rtype, constval))
      return TRUE;
  }
  return FALSE;
}

/*
 * WNL_Lower_Fast_Int_Mul_By_Constant()
 * 
 * See interface description.
 */
WN *
WNL_Lower_Fast_Int_Mul_By_Constant(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  DevAssert(WNL_Is_Fast_Int_Mul_By_Constant_Candidate(tree), ("Unexpected non fast MUL by constant candidate"));
  TYPE_ID type;
  INT bits;
  INT64 constval, sval;
  WN *kid, *kid0, *kid1;
  WN *expr = NULL;
  
  type = WN_rtype(tree);
  kid0 = WN_kid0(tree);
  kid1 = WN_kid1(tree);
  if (Is_Intconst_Val(kid0)) {
    constval = Get_Intconst_Val(kid0);
    kid = kid1;
  } else {
    constval = Get_Intconst_Val(kid1);
    kid = kid0;
  }
  bits = MTYPE_bit_size(type);
  sval = bits < 64 ? ((INT64)(constval)<<(64-bits))>>(64-bits): constval;

  if (sval == 0) {
    /* Trivial x*0 case. */
    return WN_Intconst(type, 0);
  } else if (sval == 1) {
    /* Trivial x*1 case. */
    return kid;
  } else if (sval == -1) {
    /* Trivial x*-1 case (works for both signed or unsigned -1). */
    expr = WN_Neg(type, kid);
  } else if (Is_Power_Of_2(ABS(sval), MTYPE_U8)) {
    INT pow2 = Get_Power_Of_2 (ABS(sval), MTYPE_U8);
    if (pow2 >= MTYPE_bit_size(type)) {
      expr = WN_Intconst(type, 0);
    } else {
      expr = WN_Shl(type, kid, WN_Intconst(type, pow2));
    }
    if (sval < 0 && pow2 < MTYPE_bit_size(type)-1) {
      expr = WN_Neg(type, expr);
    }
  } else if (Is_Power_Of_2(ABS(sval-1), MTYPE_U8)) {
    INT pow2 = Get_Power_Of_2 (ABS(sval-1), MTYPE_U8);
    LEAF tmp0 = Make_Leaf(block, kid, type);
    if (pow2 >= MTYPE_bit_size(type)) {
      expr = WN_Intconst(type, 0);
    } else {
      expr = WN_Shl(type, Load_Leaf(tmp0), WN_Intconst(type, pow2));
    }
    if (sval-1 < 0) {
      expr = WN_Add(type, WN_Neg(type, expr), Load_Leaf(tmp0));
    } else {
      expr = WN_Add(type, expr, Load_Leaf(tmp0));
    }
  } else if (Is_Power_Of_2(ABS(sval+1), MTYPE_U8)) {
    INT pow2 = Get_Power_Of_2 (ABS(sval+1), MTYPE_U8);
    LEAF tmp0 = Make_Leaf(block, kid, type);
    if (pow2 >= MTYPE_bit_size(type)) {
      expr = WN_Intconst(type, 0);
    } else {
      expr = WN_Shl(type, Load_Leaf(tmp0), WN_Intconst(type, pow2));
    }
    if (sval+1 < 0) {
      expr = WN_Sub(type, WN_Neg(type, expr), Load_Leaf(tmp0));
    } else {
      expr = WN_Sub(type, expr, Load_Leaf(tmp0));
    }
  }
  DevAssert(expr != NULL, ("Unexpected non power of 2 while lowering fast multiply"));
  return lower_expr(block, expr, actions);
}

