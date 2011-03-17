/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/


#ifndef variants_INCLUDED
#define variants_INCLUDED
#ifdef TARG_ST
#   include "be_errors.h"
#endif
/* ====================================================================
 * ====================================================================
 *
 * Module: variants.h
 * $Revision: 1.8 $
 * $Date: 05/08/30 17:25:50-07:00 $
 * $Author: tkong@hyalite.keyresearch $
 * $Source: be/cg/SCCS/s.variants.h $
 *
 * Revision history:
 *  01-Nov-89 - Original Version (STAPUFT GE/PE)
 *  01-Feb-91 - Copied for TP/Muse
 *  14-Feb-91 - Revised to add other cond. branch types 
 *  21-Apr-93 - Added quad branch types
 *
 * Description:
 *
 * This file defines the target's OP variant field contents.
 *
 * ====================================================================
 * ====================================================================
 */

#ifdef _KEEP_RCS_ID
static const char variants_rcs_id[] = "$Source: be/cg/SCCS/s.variants.h $ $Revision: 1.8 $";
#endif /* _KEEP_RCS_ID */

/* Type used to hold a variant:
 */
typedef UINT64 VARIANT;

#define V_NONE		0	/* default empty variant */

/* ====================================================================
 *
 * Conditions for conditional branch operators
 *
 * ====================================================================
 */

#define V_BR_NONE	0	/* No branch variant */

#define V_BR_I8EQ0	1	/* Signed integer A = 0 */
#define V_BR_I8NE0	2	/* Signed integer A != 0 */
#define V_BR_I8GT0	3	/* Signed integer A > 0 */
#define V_BR_I8GE0	4	/* Signed integer A >= 0 */
#define V_BR_I8LT0	5	/* Signed integer A < 0 */
#define V_BR_I8LE0	6	/* Signed integer A <= 0 */

#define V_BR_I8EQ	7	/* Signed integer A = B */
#define V_BR_I8NE	8	/* Signed integer A != B */
#define V_BR_I8GT	9	/* Signed integer A > B */
#define V_BR_I8GE	10	/* Signed integer A >= B */
#define V_BR_I8LT	11	/* Signed integer A < B */
#define V_BR_I8LE	12	/* Signed integer A <= B */

#define V_BR_U8EQ0	13	/* Unsigned integer A = 0 */
#define V_BR_U8NE0	14	/* Unsigned integer A != 0 */
#define V_BR_U8GT0	15	/* Unsigned integer A > 0 */
#define V_BR_U8GE0	16	/* Unsigned integer A >= 0 */
#define V_BR_U8LT0	17	/* Unsigned integer A < 0 */
#define V_BR_U8LE0	18	/* Unsigned integer A <= 0 */

#define V_BR_U8EQ	19	/* Unsigned integer A = B */
#define V_BR_U8NE	20	/* Unsigned integer A != B */
#define V_BR_U8GT	21	/* Unsigned integer A > B */
#define V_BR_U8GE	22	/* Unsigned integer A >= B */
#define V_BR_U8LT	23	/* Unsigned integer A < B */
#define V_BR_U8LE	24	/* Unsigned integer A <= B */

#define V_BR_FEQ	31	/* Floating point A = B */
#define V_BR_FNE	32	/* Floating point A != B */
#define V_BR_FGT	33	/* Floating point A > B */
#define V_BR_FGE	34	/* Floating point A >= B */
#define V_BR_FLT	35	/* Floating point A < B */
#define V_BR_FLE	36	/* Floating point A <= B */

#define V_BR_FOR	37	/* Floating point ordered compare */
#define V_BR_FUO	38	/* Floating point unordered compare */
#define V_BR_DOR	39	/* Double floating ordered compare */
#define V_BR_DUO	40	/* Double floating unordered compare */

#define V_BR_DEQ	43	/* Double floating point A = B */
#define V_BR_DNE	44	/* Double floating point A != B */
#define V_BR_DGT	45	/* Double floating point A > B */
#define V_BR_DGE	46	/* Double floating point A >= B */
#define V_BR_DLT	47	/* Double floating point A < B */
#define V_BR_DLE	48	/* Double floating point A <= B */

#define V_BR_QEQ	49	/* Quad floating point A = B */
#define V_BR_QNE	50	/* Quad floating point A != B */
#define V_BR_QGT	51	/* Quad floating point A > B */
#define V_BR_QGE	52	/* Quad floating point A >= B */
#define V_BR_QLT	53	/* Quad floating point A < B */
#define V_BR_QLE	54	/* Quad floating point A <= B */

#ifdef KEY
#define V_BR_I4EQ0	55	/* Signed integer A = 0 */
#define V_BR_I4NE0	56	/* Signed integer A != 0 */
#define V_BR_I4GT0	57	/* Signed integer A > 0 */
#define V_BR_I4GE0	58	/* Signed integer A >= 0 */
#define V_BR_I4LT0	59	/* Signed integer A < 0 */
#define V_BR_I4LE0	60	/* Signed integer A <= 0 */
#endif

#define V_BR_I4EQ	61	/* 4-byte signed integer A = B */
#define V_BR_I4NE	62	/* 4-byte signed integer A != B */
#define V_BR_I4GT	63	/* 4-byte signed integer A > B */
#define V_BR_I4GE	64	/* 4-byte signed integer A >= B */
#define V_BR_I4LT	65	/* 4-byte signed integer A < B */
#define V_BR_I4LE	66	/* 4-byte signed integer A <= B */

#ifdef KEY
#define V_BR_U4EQ0	67	/* Unsigned integer A = 0 */
#define V_BR_U4NE0	68	/* Unsigned integer A != 0 */
#define V_BR_U4GT0	69	/* Unsigned integer A > 0 */
#define V_BR_U4GE0	70	/* Unsigned integer A >= 0 */
#define V_BR_U4LT0	71	/* Unsigned integer A < 0 */
#define V_BR_U4LE0	72	/* Unsigned integer A <= 0 */
#endif

#define V_BR_U4EQ	73	/* 4-byte unsigned integer A = B */
#define V_BR_U4NE	74	/* 4-byte unsigned integer A != B */
#define V_BR_U4GT	75	/* 4-byte unsigned integer A > B */
#define V_BR_U4GE	76	/* 4-byte unsigned integer A >= B */
#define V_BR_U4LT	77	/* 4-byte unsigned integer A < B */
#define V_BR_U4LE	78	/* 4-byte unsigned integer A <= B */

#define V_BR_F_FALSE	79	/* Floating point (fcc) false */
#define V_BR_F_TRUE	80	/* Floating point (fcc) true */

#define V_BR_P_TRUE	81	/* Predicate true */
#define V_BR_PEQ	82	/* Predicate A = B */
#define V_BR_PNE	83	/* Predicate A != B */

/* Julien: add cond variant aliased for now on 4-bytes type */
#define V_BR_PGT 	V_BR_I4GT /* Predicate signed A > B */
#define V_BR_PGE 	V_BR_I4GE /* Predicate signed A >= B */
#define V_BR_PLT 	V_BR_I4LT /* Predicate signed A < B */
#define V_BR_PLE 	V_BR_I4LE /* Predicate signed A <= B */
#define V_BR_PUGT	V_BR_U4GT /* Predicate unsigned A > B */
#define V_BR_PUGE	V_BR_U4GE /* Predicate unsigned A >= B */
#define V_BR_PULT	V_BR_U4LT /* Predicate unsigned A < B */
#define V_BR_PULE	V_BR_U4LE /* Predicate unsigned A <= B */

#define V_BR_CLOOP	84	/* Counted loop */
#define V_BR_CTOP	85	/* Mod-sched counted loop (top) */
#define V_BR_CEXIT	86	/* Mod-sched counted loop (exit) */
#define V_BR_WTOP	87	/* Mod-sched while loop (top) */
#define V_BR_WEXIT	88	/* Mod-sched while loop (exit) */
#ifdef TARG_ST
/* Arthur: add branch variants for pointer arithmetic: */
#define V_BR_A4EQ	89	/* 4-byte pointer A = B */
#define V_BR_A4NE	90	/* 4-byte pointer A != B */
#define V_BR_A4GT	91	/* 4-byte pointer A > B */
#define V_BR_A4GE	92	/* 4-byte pointer A >= B */
#define V_BR_A4LT	93	/* 4-byte pointer A < B */
#define V_BR_A4LE	94	/* 4-byte pointer A <= B */

#define V_BR_A8EQ	95	/* 8-byte pointer A = B */
#define V_BR_A8NE	96	/* 8-byte pointer A != B */
#define V_BR_A8GT	100	/* 8-byte pointer A > B */
#define V_BR_A8GE	101	/* 8-byte pointer A >= B */
#define V_BR_A8LT	102	/* 8-byte pointer A < B */
#define V_BR_A8LE	103	/* 8-byte pointer A <= B */

/* Arthur: add branch variants for 40-bit arithmetic: */

#define V_BR_I5EQ	104	/* 4-byte signed integer A = B */
#define V_BR_I5NE	105	/* 4-byte signed integer A != B */
#define V_BR_I5GT	106	/* 4-byte signed integer A > B */
#define V_BR_I5GE	107	/* 4-byte signed integer A >= B */
#define V_BR_I5LT	108	/* 4-byte signed integer A < B */
#define V_BR_I5LE	109	/* 4-byte signed integer A <= B */

#define V_BR_U5EQ	110	/* 4-byte unsigned integer A = B */
#define V_BR_U5NE       111	/* 4-byte unsigned integer A != B */
#define V_BR_U5GT	112	/* 4-byte unsigned integer A > B */
#define V_BR_U5GE	113	/* 4-byte unsigned integer A >= B */
#define V_BR_U5LT	114	/* 4-byte unsigned integer A < B */
#define V_BR_U5LE	115	/* 4-byte unsigned integer A <= B */
#define V_BR_ALWAYS	200	/* Unconditional branch */
#define V_BR_NEVER	201	/* Never branch */
#define V_BR_LAST	202	/* Last one defined */

#else
#define V_BR_ALWAYS	89	/* Unconditional branch */
#define V_BR_NEVER	90	/* Never branch */
#define V_BR_LAST	91	/* Last one defined */
#endif // TARG_ST
/* V_BR_MASK *must* be 2^n - 1, and be at least as large as  */
/* V_BR_LAST */
#define V_BR_MASK	0x7f	/* Mask for branch condition */

#define V_br_condition(v)	((v) & V_BR_MASK)

/*
 * Store whether doing true or false branch in the variant,
 * separate from the branch condition.
 * True-branch is the default, 0 value.
 */
#define V_BR_FALSE	0x0080	/* do false branch rather than true branch */

#define V_false_br(v)		((v) & V_BR_FALSE)
#define Set_V_false_br(v)	((v) |= V_BR_FALSE)
#define Set_V_true_br(v)	((v) &= ~V_BR_FALSE)

/*
 * Negate a branch variant (also see Invert_BR_Variant)
 */
extern VARIANT Negate_BR_Variant(VARIANT variant);

/*
 * Invert a branch variant (also see Negate_BR_Variant)
 */
extern VARIANT Invert_BR_Variant(VARIANT variant);

/*
 * Return the name of a branch variant
 */
extern const char *BR_Variant_Name(VARIANT variant);

/*
 * Store select variants to generate select on fcc
 */
#define V_SELECT_FCC		0x0080	/* do not generate integer cc */

#define V_select_fcc_only(v)		((v) & V_SELECT_FCC)
#define Set_V_select_fcc_only(v)	((v) |= V_SELECT_FCC)
#define Reset_V_select_fcc_only(v)	((v) &= ~V_SELECT_FCC)

/* 
 * Variants for spadjust
 */
#define V_ADJUST_PLUS	0x0001
#define V_ADJUST_MINUS	0x0002


/* ====================================================================
 *
 * Variants for memory operations
 *
 * ====================================================================
 */

/* Misaligned data flags: If the V_ALIGN_ALL field of a memop 
 * (load/store) is zero, it is assumed to reference properly-aligned 
 * data.  For (possibly) misaligned data, the V_ALIGN_ASSUME field will 
 * specify the alignment which may be assumed (always a power of two, 
 * less than 16).  If the actual alignment of the referenced datum is 
 * known, V_ALIGN_ACTUAL gives it, i.e. its address modulo 16; otherwise 
 * V_ALIGN_UNKNOWN is set.
 */
#define V_ALIGNMENT		0x000f	/* Assume this alignment (2**n) */
#ifdef TARG_ST
#define V_ALIGN_OFFSET		0x1ff0	/* Actual alignment if known */
#define V_ALIGN_OFFSET_UNKNOWN	0x2000	/* Is actual alignment unknown? */
#define V_ALIGN_OVERALIGN       0x4000  /* Is an overalignment info? */
#define V_ALIGN_ALL		0x7fff	/* All alignment variant fields */
#else
#define V_ALIGN_OFFSET		0x00f0	/* Actual alignment if known */
#define V_ALIGN_OFFSET_UNKNOWN	0x0100	/* Is actual alignment unknown? */
#define V_ALIGN_ALL		0x01ff	/* All alignment variant fields */
#endif

#ifdef TARG_ST
// Alignment encoded as 2^(n-1) (special case if n=0)
#define	V_alignment(v)			(((v) & V_ALIGNMENT)?(1 << (((v) & V_ALIGNMENT)-1)):0)
#define V_overalign(v)                  ((v) & V_ALIGN_OVERALIGN)
#define V_misalign(v)                   (!V_overalign(v))
#else
#define	V_alignment(v)			(((v) & V_ALIGNMENT))
#endif

#define V_align_offset(v)		(((v) & V_ALIGN_OFFSET) >> 4)
#define V_align_offset_unknown(v)	((v) & V_ALIGN_OFFSET_UNKNOWN)
#define V_align_offset_known(v)		(!V_align_offset_unknown(v))
#define V_align_all(v)			((v) & V_ALIGN_ALL)
#ifdef TARG_ST
// Alignment encoded as 2^(n-1) (special case if n=0)
inline UINT Compute_alignment_exponent(INT align) {
  UINT result = 0;
  if (!align) {
    return (0);
  }
  FmtAssert(align > 0, ("An alignment cannot be negative"));
  while ((align & 0x7) == 0) {
    result += 3;
    align >>= 3;
  }
  return result + (align >> 1) + 1;
}
#define	Set_V_alignment(v,a)		((v) = ((v) & ~V_ALIGNMENT) | (Compute_alignment_exponent(a)&V_ALIGNMENT))
#define Set_V_overalign(v)              ((v) |= V_ALIGN_OVERALIGN)
#define Set_V_misalign(v)               ((v) &= ~V_ALIGN_OVERALIGN)
#else
#define	Set_V_alignment(v,a)		((v) = ((v) & ~V_ALIGNMENT) | ((a)&V_ALIGNMENT))
#endif
#define Set_V_align_offset(v,a)		((v) = ((v) & ~V_ALIGN_OFFSET) | (((a)&V_ALIGNMENT)<<4))
#define	Set_V_align_offset_unknown(v)	((v) |= V_ALIGN_OFFSET_UNKNOWN)
#define Set_V_align_offset_known(v)	((v) &= ~V_ALIGN_OFFSET_UNKNOWN)
#define Set_V_align_all(v,a)		((v) = ((v) & ~V_ALIGN_ALL) | ((a)&V_ALIGN_ALL))
#define Reset_V_align_all(v)		((v) &= ~V_ALIGN_ALL)

/* Volatile flag: If the load/store is volatile, then this flag is set.
 */
#ifdef TARG_ST
#define V_VOLATILE		0x8000	/* MemOp is volatile */
#else
#define V_VOLATILE		0x0200	/* MemOp is volatile */
#endif

#define V_volatile(v)			((v) & V_VOLATILE)
#define Set_V_volatile(v)		((v) |= V_VOLATILE)
#define Reset_V_volatile(v)		((v) &= ~V_VOLATILE)

/* Prefetch flags: The prefetch flags, if any, for a memory OP are
 * stored in the V_PF_FLAGS field.
 */
#define V_PF_FLAGS		0xffffffff00000000ULL /* Prefetch flags */

#define V_pf_flags(v)			((UINT32)(((v) & V_PF_FLAGS) >> 32))
#define Set_V_pf_flags(v,f)		((v) = ((v) & ~V_PF_FLAGS) | ((VARIANT)(f) << 32))
#define Reset_V_pf_flags(v)		((v) &= ~V_PF_FLAGS)


/* ====================================================================
 *
 * Variants for logical operations
 *
 * If the variant field of a logical operation is zero, we may assume the
 * operand must be normalized
 *
 * ====================================================================
 */
#define	V_NORMALIZED_OP1	0x0200	/* operand is normalized */
#define	V_NORMALIZED_OP2	0x0400	/* operand is normalized */

#define V_normalized_op1(v)		((v) & V_NORMALIZED_OP1)
#define	V_normalized_op2(v)		((v) & V_NORMALIZED_OP2)
#define Set_V_normalized_op1(v)		((v) |= V_NORMALIZED_OP1)
#define Set_V_normalized_op2(v)		((v) |= V_NORMALIZED_OP2)
#define Reset_V_normalized_op1(v)	((v) &= ~V_NORMALIZED_OP1)
#define Reset_V_normalized_op2(v)	((v) &= ~V_NORMALIZED_OP2)


/* ====================================================================
 *
 * Variants for selects
 *
 * ====================================================================
 */
#define V_SELECT_USES_FCC	0x0100  /* use float condition */

#define V_select_uses_fcc(v)            ((v) & V_SELECT_USES_FCC)
#define Set_V_select_uses_fcc(v)        ((v) |= V_SELECT_USES_FCC)
#define Reset_V_select_uses_fcc(v)	((v) &= ~V_SELECT_USES_FCC)


/* ====================================================================
 *
 * Variants for spadjust
 *
 * ====================================================================
 */
#define V_SPADJUST_PLUS		0x0001	/* spadjust is plus */
#define V_SPADJUST_MINUS	0x0002	/* spadjust is minus */

#define V_spadjust_plus(v)		((v) & V_SPADJUST_PLUS)
#define V_spadjust_minus(v)		((v) & V_SPADJUST_MINUS)
#define Set_V_spadjust_plus(v)		((v) |= V_SPADJUST_PLUS)
#define Set_V_spadjust_minus(v)		((v) |= V_SPADJUST_MINUS)
#define Reset_V_spadjust_plus(v)	((v) &= ~V_SPADJUST_PLUS)
#define Reset_V_spadjust_minus(v)	((v) &= ~V_SPADJUST_MINUS)

#ifdef KEY
/* ====================================================================
 *
 * Variants for Shuffle
 *
 * ====================================================================
 */
#define V_SHUFFLE_REVERSE	0x0000	/* Reverse */
// TODO : add more shuffle operations
#endif
#ifdef TARG_ST
/* ====================================================================
 *
 * Variants for TOP_is_cond() and TOP_is_select() operations
 * as returned by TOP_cond_variant(top)
 *
 * V_COND_TRUE : applies if comparison is condition != 0
 * V_COND_FALSE : applies if comparison is condition == 0
 * V_COND_NONE : can be used if comparison condition as a non obvious
 * or target dependent semantic.
 *
 * Currently, cond property exists for conditional branch and select
 * operation.
 * The select (TOP_is_select()) semantic is:
 *   (OU_condition == 0 ? OU_opnd1: OU_opnd2) if variant = V_COND_FALSE
 *   else OU_condition != 0 ? OU_opnd1: OU_opnd2) if variant != V_COND_FALSE
 * The cond branch (TOP_is_cond()) semantic is:
 *   branch taken if OU_condition == 0 if variant == V_COND_FALSE
 *   branch taken if OU_condition != 0 if variant != V_COND_FALSE

 * ====================================================================
 */
#define V_COND_NONE		0
#define V_COND_TRUE		1
#define V_COND_FALSE		2

/* ====================================================================
 *
 * Variants for is_cmp operations
 * as returned by OP_cmp_variant(op)
 *
 * ====================================================================
 */
#define V_CMP_NONE		0
#define V_CMP_EQ		1	/* opnd1 == opnd2 */
#define V_CMP_NE		2	/* opnd1 != opnd2 */
#define V_CMP_GT		3	/* opnd1 > opnd2 */
#define V_CMP_GTU		4	/* opnd1 u> opnd2 */
#define V_CMP_GE		5	/* opnd1 >= opnd2 */
#define V_CMP_GEU		6	/* opnd1 u>= opnd2 */
#define V_CMP_LT		7	/* opnd1 < opnd2 */
#define V_CMP_LTU		8	/* opnd1 u< opnd2 */
#define V_CMP_LE		9	/* opnd1 <= opnd2 */
#define V_CMP_LEU	       10	/* opnd1 u<= opnd2 */
#define V_CMP_ANDL	       11	/* (opnd1 != 0) && (opnd2 != 0) */
#define V_CMP_NANDL	       12	/* !((opnd1 != 0) && (opnd2 != 0)) */
#define V_CMP_ORL	       13	/* (opnd1 != 0) || (opnd2 != 0) */
#define V_CMP_NORL	       14	/* !((opnd1 != 0) || (opnd2 != 0)) */

/*
 * Negate a compare variant (also see Invert_CMP_Variant)
 */
extern VARIANT Negate_CMP_Variant(VARIANT variant);

/*
 * Invert a compare variant (also see Negate_CMP_Variant)
 */
extern VARIANT Invert_CMP_Variant(VARIANT variant);

/*
 * Return the name of a compare variant
 */
extern const char *CMP_Variant_Name(VARIANT variant);

#endif /*TARG_ST*/

#endif /* variants_INCLUDED */
