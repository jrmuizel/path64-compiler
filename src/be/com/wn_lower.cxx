/*
 * Copyright (C) 2008-2009 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*
 * Copyright (C) 2008, 2009 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
 */

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


//-*-c++-*-

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#ifdef USE_PCH
#include "be_com_pch.h"
#endif /* USE_PCH */
#pragma hdrstop
#include <limits.h>
#include <isam.h>
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <signal.h>
#include "elf_defines.h"

#include "defs.h"
#include "config.h"
#include "config_asm.h"
#include "config_debug.h"
#include "config_opt.h"
#include "config_targ_opt.h"
#include "errors.h"
#include "erglob.h"
#include "tracing.h"
#include "glob.h"
#include "timing.h"
#include "stab.h"
#include "strtab.h"
#include "util.h"
#include "wn.h"
#include "wn_util.h"
#include "stblock.h"
#include "data_layout.h"
#include "ir_reader.h"
#include "targ_sim.h"
#include "targ_const.h"
#include "const.h"
#include "ttype.h"
#include "wio.h"
#include "wn_mp.h"
#include "wn_pragmas.h"
#include "wn_simp.h"
#include "opt_alias_interface.h"
#include "wn_lower.h"
#include "region_util.h"
#include "wutil.h"
#include "wn_map.h"
#include "wn_fio.h"
#include "wn_trap.h"
#include "pu_info.h"
#include "w2op.h"
#include "be_symtab.h"
#include "betarget.h"
#include "be_util.h"
#include "opt_cvtl_rule.h"
#include "fb_whirl.h"
#include "intrn_info.h"
#ifdef KEY
#include "config_vho.h"		// for VHO_Enable_Simple_If_Conv
#include "targ_const_private.h" // for TCON_R4, TCON_R8, ..
#endif
#ifdef TARG_X8664
#include <ext/hash_set>
#endif
#ifdef TARG_ST
#include "wn_lower_private.h"
#include "wn_lower_mul.h"
#endif
#ifdef TARG_ST
#define NOT_SUPPORT_ST(expr)
#else
#define NOT_SUPPORT_ST(expr) expr
#endif

/* My changes are a hack till blessed by Steve. (suneel) */
#define SHORTCIRCUIT_HACK 1
#define MMCOPY_MULT 1.2 
#define MEMLIB_THRESHOLD_BYTES OPT_Threshold_To_Memlib  //Parameter to adjust
#define MEMLIB_STRUCT_BYTES 128  //Parameter to adjust
#define MEMLIB_NUM_STORE 5  //Parameter to adjust
#define MEMLIB_MAX_NUM 6
#ifdef KEY
#define DELETED_MAX 100 
#else
#define DELETED_MAX 20 
#endif

#ifdef TARG_ST
#define NEW_LOWER // [CG] Enables paired support
#endif

#ifdef TARG_ST
/* [CG]: If true, disable wn_simplification that would undo some
   transformations such as LOWER_TREEHEIGHT.
*/
static BOOL wn_lower_no_wn_simplify;
#endif

/* this next header should be after the external declarations in the others */
#include "pragma_weak.h"	/* Alias routines defined in wopt.so */

/* ====================================================================
 *			 Exported Functions
 * ====================================================================
 */
extern PREG_NUM AssignExpr(WN *, WN *, TYPE_ID);

extern BOOL lower_is_aliased(WN *, WN *, INT64 size, BOOL defalt);

extern INT32 compute_copy_alignment(TY_IDX, TY_IDX, INT32 offset);

extern TYPE_ID compute_copy_quantum(INT32 );

extern WN *WN_I1const(TYPE_ID, INT64 con);

/*
 * defined in config.c (should be in config.h)
 */
extern INT32	MinStructCopyLoopSize;
extern INT32	MinStructCopyMemIntrSize;
/*
 * defined in config.c (should be in config.h)
 */
extern INT32	MinStructCopyLoopSize;
extern INT32	MinStructCopyMemIntrSize;
extern INT32	MinStructCopyParallel;

/* ====================================================================
 *			 Imported Declarations
 * ====================================================================
 */
extern WN *emulate(WN *, WN *);

#ifdef KEY // bug 6938
extern WN *emulate_fast_exp(WN *, WN *);
#endif

extern WN *intrinsic_runtime(WN *, WN *);

extern WN *make_pointer_to_node(WN *, WN *);

extern char *INTR_intrinsic_name( WN *);

extern void fdump_dep_tree(FILE *, WN *, struct ALIAS_MANAGER *);

extern void InitParityMaps(void);

extern "C" void LNOPreserveMapPair(WN *, WN *, WN *);

extern "C" void LNOPruneMapsUsingParity(void);

extern "C" void LNOPrintDepGraph(FILE *);

extern void enable_tree_freq_display(void);

extern TYPE_ID INTR_return_mtype(INTRINSIC id);

extern BE_ST_TAB   Be_st_tab;

#ifdef TARG_X8664
// Nesting depth is insanely high (120 or so) for bug 599
#define MAX_LOOP_NEST_DEPTH 150
/* <loop_info_stack> has its own <current_loop_nest_depth>, since <loop_nest_depth>
   will be set to 0 by WN_loop_depth(). Most of the time, WN_loop_depth will
   give non-zero value inside a do_loop, but IPA could make it be zero, which could
   cause some other problems later.
   ToDo: Look into IPA to find out what is going on.
*/
static UINT8 current_loop_nest_depth;
static WN* loop_info_stack[MAX_LOOP_NEST_DEPTH];
static BOOL last_call_ff2c_abi; // whether the last outgoing call use this abi
#endif

/* ====================================================================
 *			 Forward Declarations
 * ====================================================================
 */
#ifdef TARG_ST
/* Export it to other wn_lower files. */
WN *lower_scf(WN *, WN *, LOWER_ACTIONS);
WN *lower_expr(WN *, WN *, LOWER_ACTIONS);
WN *lower_stmt(WN *, WN *, LOWER_ACTIONS);
#else
static WN *lower_scf(WN *, WN *, LOWER_ACTIONS);
static WN *lower_expr(WN *, WN *, LOWER_ACTIONS);
static WN *lower_stmt(WN *, WN *, LOWER_ACTIONS);
#endif

static WN *lower_store(WN *, WN *, LOWER_ACTIONS);
static WN *lower_call(WN *, WN *, LOWER_ACTIONS);
static WN *lower_intrinsic(WN *, WN *, LOWER_ACTIONS);
static WN *lower_intrinsic_call(WN *, WN *, LOWER_ACTIONS);
static WN *lower_intrinsic_op(WN *, WN *, LOWER_ACTIONS);
static WN *lower_if(WN *, WN *, LOWER_ACTIONS);
static WN *lower_entry(WN *, LOWER_ACTIONS);
#ifdef KEY
static WN *lower_landing_pad_entry(WN *);
#endif // KEY
static WN *lower_eval(WN *, WN *, LOWER_ACTIONS);
static WN *lower_copy_tree(WN *, LOWER_ACTIONS);
static WN *lower_emulation(WN *, WN *, LOWER_ACTIONS, BOOL &intrinsic_lowered);
static WN *lower_mstore(WN *, WN *, LOWER_ACTIONS);
static WN *lower_nary_madd(WN *, WN *, LOWER_ACTIONS);
static WN *lower_madd(WN *, WN *, LOWER_ACTIONS);
static WN *lower_assert(WN *, WN *, LOWER_ACTIONS);
static WN *lower_trapuv(WN *, WN *, LOWER_ACTIONS);
static WN *lower_base_reference(WN *, ST *, INT64, LOWER_ACTIONS);
static WN *lower_base_register(WN *, ST *, INT64, LOWER_ACTIONS);
static WN *lower_dereference(WN *, INT64, ST *, PREG_NUM, LOWER_ACTIONS);
static WN *lower_split_sym_addrs(WN *, INT64, LOWER_ACTIONS);
static WN *improve_Malignment(WN *, WN *, WN *, INT64);
static WN *lower_branch(WN *, WN *, LOWER_ACTIONS);
static WN *lower_branch_condition(BOOL, LABEL_IDX, WN *, WN **, LOWER_ACTIONS);
static WN *lower_conditional(WN *, WN *, LABEL_IDX, LABEL_IDX, BOOL,
			     LOWER_ACTIONS);
static WN *lower_tree_height(WN *, WN *, LOWER_ACTIONS);
#ifdef KEY
static void lower_madd_tree_height(WN *, WN *, LOWER_ACTIONS);
#endif

static WN *Lower_Memlib(WN *block, WN *tree, LOWER_ACTIONS actions);
static WN *Lower_Mistore_Memlib(WN *block, WN *tree, LOWER_ACTIONS actions);
static WN *Lower_STID_Memlib(WN *block, WN *tree, LOWER_ACTIONS actions);

static TY_IDX coerceTY(TY_IDX, TYPE_ID);
static ST *coerceST(const ST *, TYPE_ID);
static ST *coerceST(const ST &, TYPE_ID);
static WN_OFFSET coerceOFFSET(WN *, TYPE_ID, WN_OFFSET);


static WN *lower_mload(WN *, WN *, LOWER_ACTIONS);
static void lower_mload_formal(WN *, WN *, PLOC, LOWER_ACTIONS);
#ifdef TARG_ST
static void lower_mload_actual (WN *, WN *, WN *, WN *,PLOC, LOWER_ACTIONS);
static void lower_actions_fprintf(FILE *f, LOWER_ACTIONS actions);
#else
static void lower_mload_actual (WN *, WN *, PLOC, LOWER_ACTIONS);
#endif
static void lower_complex_emulation(WN *, WN *, LOWER_ACTIONS, WN **, WN **);
static void lower_complex_expr(WN *, WN *, LOWER_ACTIONS, WN **, WN **);


static void lower_copy_maps(WN *, WN *, LOWER_ACTIONS);
static void lower_tree_copy_maps(WN *, WN *, LOWER_ACTIONS);

static INT32 compute_alignment(WN *, INT64);
static TYPE_ID compute_next_copy_quantum(TYPE_ID , INT32);
static WN*lower_load_bits (WN* block, WN* wn, LOWER_ACTIONS actions);
static WN*lower_store_bits (WN* block, WN* wn, LOWER_ACTIONS actions);

#ifdef KEY
static WN *lower_malloc_alg(WN *block, WN *tree, LOWER_ACTIONS actions);
static ST_IDX find_trampoline(ST_IDX func_st_idx);
#endif
#ifdef TARG_ST
static void lower_bit_field_id(WN *wn);
static WN*lower_load_bitfield (WN* block, WN* wn, LOWER_ACTIONS actions);
static WN*lower_store_bitfield (WN* block, WN* wn, LOWER_ACTIONS actions);
#endif

#ifdef TARG_ST
static WN *lower_return(WN *block, WN *tree, LOWER_ACTIONS actions);
static WN *lower_preamble_end(WN *block, WN *tree, LOWER_ACTIONS actions);
static WN *lower_cst_idiv(WN *block, WN *tree, LOWER_ACTIONS actions);
static WN *lower_cst_udiv(WN *block, WN *tree, LOWER_ACTIONS actions);
static WN *lower_cst_irem(WN *block, WN *tree, LOWER_ACTIONS actions);
static WN *lower_cst_urem(WN *block, WN *tree, LOWER_ACTIONS actions);
#endif

#ifdef TARG_ST
static BOOL Is_Fast_Divide(WN *wn);
static BOOL Is_Const_Divide(WN *wn);
#endif

static WN *lower_hugepage_limit(WN *block, WN *tree, LOWER_ACTIONS actions);

/* ====================================================================
 *			 private variables
 * ====================================================================
 */
static INT   max_region;
static char *current_preg_name;
static UINT16 loop_nest_depth;
static BOOL contains_a_loop;
static struct ALIAS_MANAGER *alias_manager;
#define PARITY_MAP_ARRAY_SIZE 32
static WN_MAP parity_map_array[PARITY_MAP_ARRAY_SIZE];
static WN_MAP lowering_parity_map = 0;
static INT32 parity_map_index = -1;
static LOWER_ACTIONS lowering_actions= 0;
static BOOL save_Div_Split_Allowed ;
static INT cur_memlib_idx        = 0;

#ifdef TARG_ST
// [CG] Map of mtypes to aliasing types for ansi rules
//TB: extension reconfiguration: check that array accesses do not
//overlap static counter
#define TY_alias_array(t) \
     ((t > MTYPE_STATIC_LAST) ? \
       FmtAssert (FALSE, ("TY_alias_array: no access for dynamic MTYPE %d", (t))), 0 \
     : \
       TY_alias_array[t])
#define TY_alias_array_set(t) \
       FmtAssert (t <= MTYPE_STATIC_LAST, ("MTYPE_TO_PREG: no access for dynamic MTYPE %d", (t))), TY_alias_array[t]
static TY_IDX TY_alias_array[MTYPE_STATIC_LAST + 1];
#endif


static BOOL traceIO              = FALSE;
static BOOL traceSpeculate       = FALSE;
static BOOL traceAlignment       = FALSE;
static BOOL traceTreeHeight      = FALSE;
static BOOL traceSplitSymOff     = FALSE;
static BOOL traceWoptFinishedOpt = FALSE;
static BOOL traceMload           = FALSE;
static BOOL traceMemlib          = FALSE;
static BOOL traceLdid            = FALSE;
static BOOL traceCall            = FALSE;
static BOOL traceRuntime         = FALSE;

// static BOOL traceUplevel = FALSE;

typedef struct CURRENT_STATE
{
  SRCPOS	srcpos;
  WN		*stmt;
  WN		*function;
  LOWER_ACTIONS	actions;
} CURRENT_STATE, *CURRENT_STATEp;

CURRENT_STATE	current_state;

#define	current_srcpos		current_state.srcpos
#define	current_stmt		current_state.stmt
#define	current_actions		current_state.actions
#define current_function	current_state.function

typedef enum MSTORE_ACTIONS
{
  MSTORE_aggregate,
  MSTORE_loop,
  MSTORE_intrinsic_bzero,
  MSTORE_intrinsic_memset,
#ifdef KEY
  MSTORE_intrinsic_memcpy,
#endif
  MSTORE_intrinsic_bcopy
} MSTORE_ACTIONS;

enum MEMLIB_ACTIONS
{
  IGNORE,
  ABORT_OPT,
  CONT_OPT
};

static const char * MSTORE_ACTIONS_name(MSTORE_ACTIONS);
#ifdef TARG_ST
/*
 * MTYPE_To_TY_Alias
 *
 * Returns for the given mtype a type that
 * is not subject to ANSI alias rules.
 * This must be used for instance when lowering
 * aggregates copies into a sequence of copies.
 * In this case the type for the new copies must
 * not be subject to aliasing rules as the initial
 * aggregate type information is lost.
 */
static TY_IDX
MTYPE_To_TY_Alias(TYPE_ID mtype)
{
  if (TY_alias_array(mtype) == 0) {
    TY_alias_array_set(mtype) = Copy_TY(MTYPE_To_TY (mtype));
    Set_TY_no_ansi_alias(TY_alias_array(mtype));
  }
  return TY_alias_array(mtype);
}
#endif

#ifdef TARG_ST
/* ====================================================================
 * Some support functions for finding load/stores of parameters.
 * ====================================================================
 */
BOOL
stmt_is_store_of_return_value(WN *stmt)
{
  return OPERATOR_is_store(WN_operator(stmt)) && WN_has_sym(stmt) &&
    ST_class(WN_st(stmt)) == CLASS_PREG && 
    Preg_Is_Dedicated(WN_offset(stmt));
}

BOOL
expr_loads_dedicated_preg(WN *expr)
{
  BOOL result = FALSE;
  if (WN_kid_count(expr) > 0) {
    for (INT i = 0; i < WN_kid_count(expr); i++)
      result |= expr_loads_dedicated_preg(WN_kid(expr,i));
  } else if (OPERATOR_is_load(WN_operator(expr)) &&
	     WN_has_sym(expr) &&
	     ST_class(WN_st(expr)) == CLASS_PREG && 
	     Preg_Is_Dedicated(WN_offset(expr))) {
    result = TRUE;
  }
  return result;
}

BOOL
stmt_is_store_of_callee_return_value(WN *stmt)
{
  return OPERATOR_is_store(WN_operator(stmt)) &&
    expr_loads_dedicated_preg(WN_kid0(stmt));
}
#endif


#ifdef TARG_ST
static TYPE_ID Promoted_Mtype[MTYPE_STATIC_LAST + 1] = {
#else
static TYPE_ID Promoted_Mtype[MTYPE_LAST + 1] = {
#endif
  MTYPE_UNKNOWN,  /* MTYPE_UNKNOWN */
  MTYPE_UNKNOWN,  /* MTYPE_B */
  MTYPE_I4,       /* MTYPE_I1 */
  MTYPE_I4,       /* MTYPE_I2 */
  MTYPE_I4,       /* MTYPE_I4 */
  MTYPE_I8,       /* MTYPE_I8 */
  MTYPE_U4,       /* MTYPE_U1 */
  MTYPE_U4,       /* MTYPE_U2 */
  MTYPE_U4,       /* MTYPE_U4 */
  MTYPE_U8,       /* MTYPE_U8 */
#ifdef TARG_ST
  MTYPE_I5,       /* MTYPE_I5 */
  MTYPE_U5,       /* MTYPE_U5 */
  MTYPE_A4,       /* MTYPE_A4 */
  MTYPE_A8,       /* MTYPE_A8 */
#endif
  MTYPE_F4,       /* MTYPE_F4 */
  MTYPE_F8,       /* MTYPE_F8 */
  MTYPE_UNKNOWN,  /* MTYPE_F10 */
#ifdef TARG_ST
  MTYPE_UNKNOWN,  /* MTYPE_F16 */
#else
  MTYPE_F16,      /* MTYPE_F16 */
#endif
  MTYPE_UNKNOWN,  /* MTYPE_STR */
  MTYPE_FQ,       /* MTYPE_FQ */
  MTYPE_M,        /* MTYPE_M */
  MTYPE_C4,       /* MTYPE_C4 */
  MTYPE_C8,       /* MTYPE_C8 */
  MTYPE_CQ,       /* MTYPE_CQ */
  MTYPE_V         /* MTYPE_V */
 ,MTYPE_UNKNOWN,  /* MTYPE_BS */
#ifndef TARG_ST
  MTYPE_UNKNOWN,  /* MTYPE_A4 */
  MTYPE_UNKNOWN,  /* MTYPE_A8 */
#endif
  MTYPE_UNKNOWN,  /* MTYPE_C10 */
#ifdef TARG_ST
  MTYPE_UNKNOWN,  /* MTYPE_C16 */
#else
  MTYPE_C16,      /* MTYPE_C16 */
#endif
  MTYPE_UNKNOWN,  /* MTYPE_I16 */
  MTYPE_UNKNOWN   /* MTYPE_U16 */
#ifdef TARG_MIPS
 ,MTYPE_V8I4,     /* MTYPE_V8I4 */
  MTYPE_V8F4      /* MTYPE_V8F4 */
#endif
#ifdef TARG_X8664
 ,MTYPE_V16C4,    /* MTYPE_V16C4 */
  MTYPE_V16C8,    /* MTYPE_V16C8 */
  MTYPE_V16I1,    /* MTYPE_V16I1 */
  MTYPE_V16I2,    /* MTYPE_V16I2 */
  MTYPE_V16I4,    /* MTYPE_V16I4 */
  MTYPE_V16I8,    /* MTYPE_V16I8 */
  MTYPE_V16F4,    /* MTYPE_V16F4 */
  MTYPE_V16F8,    /* MTYPE_V16F8 */
  MTYPE_V8I1,     /* MTYPE_V8I1 */
  MTYPE_V8I2,     /* MTYPE_V8I2 */
  MTYPE_V8I4,     /* MTYPE_V8I4 */
  MTYPE_V8F4,     /* MTYPE_V8F4 */
  MTYPE_M8I1,     /* MTYPE_M8I1 */
  MTYPE_M8I2,     /* MTYPE_M8I2 */
  MTYPE_M8I4,     /* MTYPE_M8I4 */
  MTYPE_M8F4      /* MTYPE_M8F4 */
#endif // TARG_X8664
};

/* ====================================================================
 *			Private macros
 * ====================================================================
 */

#define OPCODE_is_intrinsic(op)                                 	\
		((OPCODE_operator((op)) == OPR_INTRINSIC_CALL) ||       \
		(OPCODE_operator((op)) == OPR_INTRINSIC_OP))

#define	Action(x)			(actions & (x))
#define	NotAction(x)			(Action(x)==0)
#ifdef TARG_ST
#define	OnlyAction(x, action)		((x) & (action))
#define	RemoveAction(x, action)		((x) & ~(action))
#endif
#define	RemoveScfAction(x)		(x & ~(LOWER_SCF))
#define	RemoveShortCircuitAction(x)	(x & ~(LOWER_SHORTCIRCUIT))


#define	WN_has_alias_info(x)	(OPCODE_is_load(WN_opcode(x))	||	\
				 OPCODE_is_store(WN_opcode(x))	||	\
				 WN_operator_is(x, OPR_PARM))
#define	WN_has_offset(x)	(OPCODE_has_offset(WN_opcode(x)))

#define	WN_nary_intrinsic(x)	(WN_operator_is(x, OPR_INTRINSIC_OP) &&	\
      				((WN_intrinsic(x)== INTRN_NARY_ADD) ||	\
      				 (WN_intrinsic(x)== INTRN_NARY_MPY)))
#define	WN_nary_add(x)		(WN_operator_is(x, OPR_INTRINSIC_OP) &&	\
      				 (WN_intrinsic(x)== INTRN_NARY_ADD))
#define	WN_nary_mpy(x)		(WN_operator_is(x, OPR_INTRINSIC_OP) &&	\
      				 (WN_intrinsic(x)== INTRN_NARY_MPY))

#define	WN_is_block(x)		(WN_opcode(x) == OPC_BLOCK)

#define INTRN_is_nary(x)	(((x)==INTRN_NARY_ADD) || ((x)==INTRN_NARY_MPY))

#define	WN_is_commutative(x)	(WN_opcode(x) == OPCODE_commutative_op(WN_opcode(x)))

#define	TY_is_pointer(x)	(TY_kind(x) == KIND_POINTER)

#define	lower_truebr(l,c,b,a)	lower_branch_condition(TRUE,l,c,b,a)
#define	lower_falsebr(l,c,b,a)	lower_branch_condition(FALSE,l,c,b,a)
#define WN_same_id(a,b)         (WN_st(a)==WN_st(b) && WN_offset(a)==WN_offset(b))

/* ====================================================================
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
 * ==================================================================== */

#define mem_offset_hi(offset) ((offset) & ~0x7fff)
#define mem_offset_lo(offset) ((offset) & 0x7fff)

#ifdef TARG_ST
//
// Arthur: machine-dependent parameters specified in 
//         targinfo/<target>/be/betarget.cxx
//
extern BOOL mem_offset_must_be_split(INT64 offset);
#else
#define mem_offset_must_be_split(offset)		\
        (!(-32768 <= (offset) && (offset) < 32768))
#endif /* TARG_ST */

#define mem_offset_2GB(offset)				\
	(!(INT32_MIN <= offset && offset <= INT32_MAX))

#define	PIC_SHARED		(Gen_PIC_Shared || Gen_PIC_Call_Shared)
#define	PIC_NONSHARED		(!PIC_SHARED)

#define ABS(x)			(((x)<0) ? -(x) : (x))
#define IS_POWER_OF_2(val)      ((val != 0) && ((val & (val-1)) == 0))


/* ====================================================================
 *
 * UINT32 compute_offset_alignment(INT32 offset, UINT32 align)
 *
 * return gcd of offset,align;
 * Used for alignment reasons;
 * For maximum efficiency, offset should be >= align
 *
 * ==================================================================== */
UINT32 compute_offset_alignment(INT32 offset, UINT32 align)
{
  UINT32 m = ABS(offset);
  UINT32 n = align;
  while (n != 0) {
    INT32 new_n = m % n;
    m = n;
    n = new_n;
  }
  return m;
}


/* ====================================================================
 *
 *  The semantics of memset require replicating the byte constant
 *  so replicate the constant into a I8/U8 depending on WN_rtype(con)
 *
 * ==================================================================== */
extern WN *WN_I1const(TYPE_ID type, INT64 con)
{
  // assume con is a byte constant, so clear the other bits
  // (otherwise replicate will "or" with sign bits).
  INT64	n=	  con & 0xff;
  INT64	maxAlign= MTYPE_alignment(Max_Uint_Mtype);

  if (con)
  {
    INT64 i;
    for(i=1; i<maxAlign; i++)
    {
      n |=	(n << 8);
    }
  }

  return WN_Intconst(Mtype_AlignmentClass(maxAlign,
					  MTYPE_type_class(type)), n);
}

/* ====================================================================
 *
 * void push_current_state(WN *tree, STATE *state)
 *
 * return and set current state
 *
 * ==================================================================== */
static void setCurrentState(WN *tree, LOWER_ACTIONS actions)
{
  if (tree) {
    current_stmt =	tree;
    current_srcpos =	WN_Get_Linenum(tree);
    current_actions =	actions;

    if (WN_opcode(tree) == OPC_FUNC_ENTRY)
      current_function = tree;
  }
}

static void setCurrentStateBlockFirst(WN *tree, LOWER_ACTIONS actions)
{
  Is_True(WN_opcode(tree) == OPC_BLOCK, ("expected BLOCK node"));

  setCurrentState(WN_first(tree), actions);
}

static void setCurrentStateBlockLast(WN *tree, LOWER_ACTIONS actions)
{
  Is_True(WN_opcode(tree) == OPC_BLOCK, ("expected BLOCK node"));

  setCurrentState(WN_last(tree), actions);
}


static CURRENT_STATE pushCurrentState(WN *tree, LOWER_ACTIONS actions)
{
  CURRENT_STATE saveState = current_state;

  setCurrentState(tree, actions);

  return saveState;
}

static void popCurrentState(CURRENT_STATE state)
{
  current_state = state;
}

/* ====================================================================
 *
 * BOOL foldConstOffset(WN *con, INT64 offset)
 *
 * BOOL foldLdaOffset(WN *lda, INT64 offset)
 *
 * can con and offset be folded to "fit" into a WN_OFFSET (INT32?)
 *
 * ==================================================================== */

static BOOL foldConstOffset(WN *con, INT64 offset)
{
  if (WN_operator_is(con, OPR_INTCONST))
  {
    INT64 sum= offset + WN_const_val(con);

    if (INT32_MIN <= sum && sum <= INT32_MAX)
      return TRUE;
  }
  return FALSE;
}

static BOOL foldLdaOffset(WN *lda, INT64 offset)
{
  if (WN_operator_is(lda, OPR_LDA))
  {
    INT64 sum= offset + WN_lda_offset(lda);

    Is_True((WN_class(lda) != CLASS_PREG), ("lda class is PREG!!!"));

#ifdef TARG_ST
    // Arthur: this seems more clean, target-independent way to
    //         say the same thing.
    if (Can_Be_Immediate(WN_operator(lda), sum, WN_rtype(lda), -1, NULL));
#else
    if (INT32_MIN <= sum && sum <= INT32_MAX)
#endif
    {
      ST  *sym= WN_st(lda);

     /*
      *	if the offset is greater than the symbol size, we may generate
      * a relocation that is out of bounds (ex LFTR). See pv 482353 for
      * this rare condition
      */
      if (ST_class(sym) == CLASS_BLOCK && STB_size(sym) < sum)
	return FALSE;

      return TRUE;
    }
  }
  return FALSE;
}



#ifdef TARG_ST
static BOOL WN_Pass_Low_First (WN *tree)
{
  switch (WN_operator(tree)) {
  case OPR_LDID:
  case OPR_STID:
  case OPR_LDBITS:
  case OPR_STBITS:
    if (WN_class(tree) == CLASS_PREG
	&& Preg_Is_Dedicated (WN_load_offset (tree))) {
      // Allow target to specify different handling order
      // when a value is in a dedicated preg.
      return Pass_Low_First (WN_rtype(tree));
    }
    break;
  }
  return Target_Byte_Sex == LITTLE_ENDIAN;
}
#endif



/* ====================================================================
 *
 * WN_OFFSET coerceOFFSET(WN *tree, TYPE_ID realTY, WN_OFFSET offset)
 *
 * The offset may either be an offset or preg number.
 *
 * There is an amazing kludge for complex return values where we
 * return F0, F2
 *
 * ==================================================================== */
static WN_OFFSET coerceOFFSET(WN *tree, TYPE_ID realTY, WN_OFFSET offset)
{

  switch (WN_operator(tree))
  {
  case OPR_ILOAD:
  case OPR_ILOADX:
  case OPR_ISTORE:
  case OPR_ISTOREX:
#ifdef TARG_ST
  case OPR_ISTBITS:
  case OPR_ILDBITS:
#endif
    return offset + MTYPE_RegisterSize(realTY);

  case OPR_LDID:
  case OPR_STID:
#ifdef TARG_ST
  case OPR_LDBITS:
  case OPR_STBITS:
#endif
    if (WN_class(tree) == CLASS_PREG)
    {
#ifdef TARG_MIPS
     /*
      *  amazing kludge
      *  for dedicated return register (F0) the ABI defines [F0,F2]
      *  as the return values
      */
      if (Preg_Is_Dedicated(offset) && offset == Float_Preg_Min_Offset)
      {
	  return Float_Preg_Min_Offset + 2;
      }
      else
#endif
	return offset + Preg_Increment(realTY);
    }
    return offset + MTYPE_RegisterSize(realTY);
  }
  Fail_FmtAssertion("unexpected complex op (%s)",
		    OPCODE_name(WN_opcode(tree)));
  /*NOTREACHED*/
}

/* ====================================================================
 *
 *
 * ==================================================================== */

static void rename_preg(const char *f, const char *preg_class)
{
  static char name[41];

  name[0] = '\0';
  
  strncat(name, f, 30);

  if (preg_class)
  {
    strncat(name, preg_class, 10);
  }
  current_preg_name = name;
}

static void rename_reset(void)
{
  current_preg_name = NULL;
}


static void WN_Set_Flags(WN *src, WN *dst)
{
  if (OPCODE_has_flags(WN_opcode(src)))
  {
    Is_True(OPCODE_has_flags(WN_opcode(dst)), ("expected wn with flags"));
    WN_set_flag(dst, WN_flag(src));
  }
}

extern void WN_annotate_intrinsic_flags(INTRINSIC id, ST *sym)
{
  if (INTRN_is_pure(id)) {
    Set_PU_is_pure(Get_Current_PU());
  }
  if (INTRN_has_no_side_effects(id)) {
    Set_PU_no_side_effects(Get_Current_PU());
  }
}

extern void WN_annotate_call_flags(WN *call, ST *sym)
{
  WN_Set_Call_Default_Flags(call);

  if (PU_no_side_effects(Pu_Table[ST_pu(sym)]))
  {   
    WN_Reset_Call_Non_Data_Mod(call);
    WN_Reset_Call_Non_Parm_Mod(call);
    WN_Reset_Call_Parm_Mod(call);

  }
  if (PU_is_pure(Pu_Table[ST_pu(sym)]))
  {
    WN_Reset_Call_Non_Data_Mod(call);
    WN_Reset_Call_Non_Parm_Mod(call);
    WN_Reset_Call_Parm_Mod(call);

    WN_Reset_Call_Non_Data_Ref(call);
    WN_Reset_Call_Non_Parm_Ref(call);
  }
}

/* ====================================================================
 *
 *
 * Create a new label with linenum info
 *
 * ==================================================================== */
static LABEL_IDX NewLabel(void)
{
#ifdef TARG_ST
  // [CG] same as Gen_Temp_Label() from label_util.cxx. could we merge?
  // [CG] We have BB_Label_Name_Scope_Index in config_asm.h for ST targets
  char name[128];
  LABEL_IDX labx;
  LABEL &label = New_LABEL(CURRENT_SYMTAB, labx);

  /* [TB]: For IPA's sake, do not generate a specific label name, which will
     conflict with some other basic blocks of other PUs. Fix taken from PathScale
  */
  if( Run_ipl ){
    return labx;
  }

  sprintf(name, BB_Label_Name_Scope_Index, "", Current_PU_Count(), labx);
  LABEL_Init (label, Save_Str(name), LKIND_DEFAULT);
  return labx;
#else

  LABEL_IDX label;
  LABEL& lab = New_LABEL(CURRENT_SYMTAB, label);

#ifdef KEY
  /* For IPA's sake, do not generate a specific label name, which will
     conflict with some other basic blocks of other PUs.
  */

  if( Run_ipl ){
    return label;
  }
#endif

  // create label name
  char *name = (char *) alloca (strlen(".L..") + 8 + 8 + 1);
  sprintf(name, ".L%s%d%s%d", Label_Name_Separator, Current_PU_Count(), 
	Label_Name_Separator, label);
  LABEL_Init (lab, Save_Str(name), LKIND_DEFAULT);
  return label;
#endif
}

static WN *WN_Label(LABEL_IDX l)
{
  WN *label = WN_CreateLabel(ST_IDX_ZERO, l, 0, NULL);
  WN_Set_Linenum(label, current_srcpos);
  return label;
}

static WN *WN_NewLabel(void)
{
  LABEL_IDX label;
  label = NewLabel();
  return WN_Label(label);
}



/* ====================================================================
 *
 *
 * Create false/true branch with line info
 *
 * ==================================================================== */
static WN *WN_Falsebr(LABEL_IDX label, WN *cond)
{
  WN *branch = WN_CreateFalsebr(label, cond);
  WN_Set_Linenum(branch, current_srcpos);
  return branch;
}

static WN *WN_Truebr(LABEL_IDX label, WN *cond)
{
  WN *branch = WN_CreateTruebr(label, cond);
  WN_Set_Linenum(branch, current_srcpos);
  return branch;
}

static WN *WN_Goto(LABEL_IDX label)
{
  WN *branch =	WN_CreateGoto((ST_IDX) 0, label);
  WN_Set_Linenum(branch, current_srcpos);
  return branch;
}

static BOOL expr_is_speculative(WN *tree)
{
  if (OPT_Lower_Speculate)
  {
    BOOL  speculate;

    speculate = WN_Expr_Can_Be_Speculative(tree, alias_manager);
    if (traceSpeculate && speculate)
    {
      DevWarn("WN_lower: found speculative expression: line %d",
	      Srcpos_To_Line(current_srcpos));
    }
    return speculate;
  }
  return FALSE;
}


/* ====================================================================
 *
 * Helper procedures for WN_ExprToNary.
 *
 * Nary_List_Mpy traverses a tree of OPR_MPY and accumulates the MPY
 * operands in an array.
 *
 * Nary_Count_Mpy traverses the same tree but merely counts the number
 * of MPY operands.
 *
 * Nary_List_AddSub traverses a tree of OPR_ADD and OPR_SUB and
 * accumulates the ADD operands in an array, negating operands when
 * necessary due to the SUBs.
 *
 * Nary_Count_AddSub traverses the same tree but merely counts the
 * number of ADD operands.
 *
 * The limit operators limit the depth of the traversals.
 *
 * ==================================================================== */


static INT Nary_Count_Mpy(WN *wn, INT limit)
{
  Is_True(WN_operator(wn) == OPR_MPY,
	  ("Nary_Count_Mpy expected OPR_MPY"));
  INT count = 1;
  OPCODE opc_mpy = WN_opcode(wn);
  for (--limit; WN_opcode(wn) == opc_mpy && limit >= 0; --limit) {
    WN *wn_kid0 = WN_kid0(wn);
    if (WN_opcode(wn_kid0) == opc_mpy) {
      count += Nary_Count_Mpy(wn_kid0, limit);
    } else {
      count += 1;
    }
    wn = WN_kid1(wn);
  }
  return count;
}


static INT Nary_List_Mpy(WN *wn, WN **args, INT idx, INT limit)
{
  Is_True(WN_operator(wn) == OPR_MPY,
	  ("Nary_List_Mpy expected OPR_MPY"));
  OPCODE opc_mpy = WN_opcode(wn);
  for (--limit; WN_opcode(wn) == opc_mpy && limit >= 0; --limit) {
    WN *wn_kid0 = WN_kid0(wn);
    if (WN_opcode(wn_kid0) == opc_mpy) {
      idx = Nary_List_Mpy(wn_kid0, args, idx, limit);
    } else {
      args[idx++] = wn_kid0;
    }
    WN *wn_mpy = wn;
    wn = WN_kid1(wn);
    WN_Delete(wn_mpy);
  }
  if (WN_opcode(wn) == opc_mpy) {
    Lmt_DevWarn(1, ("WN_ExprToNaryType MPY too deep, so stop"));
  }
  args[idx++] = wn;
  return idx;
}


static INT Nary_Count_AddSub(WN *wn, INT limit)
{
  Is_True(WN_operator(wn) == OPR_ADD || WN_operator(wn) == OPR_SUB ||
	  WN_operator(wn) == OPR_NEG,
	  ("Nary_Count_AddSub expected OPR_ADD, OPR_SUB, or OPR_NEG"));
  TYPE_ID rtype = WN_rtype(wn);
  OPCODE opc_add = OPCODE_make_op(OPR_ADD, rtype, MTYPE_V);
  OPCODE opc_sub = OPCODE_make_op(OPR_SUB, rtype, MTYPE_V);
  OPCODE opc_neg = OPCODE_make_op(OPR_NEG, rtype, MTYPE_V);
  INT count = 1;
  while (WN_opcode(wn) == opc_neg) {
    --limit;
    wn = WN_kid0(wn);
  }
  for (--limit; (WN_opcode(wn) == opc_add ||
		 WN_opcode(wn) == opc_sub) && limit >= 0; --limit) {
    WN *wn_kid0 = WN_kid0(wn);
    if (WN_opcode(wn_kid0) == opc_add || WN_opcode(wn_kid0) == opc_sub) {
      count += Nary_Count_AddSub(wn_kid0, limit);
    } else {
      count += 1;
    }
    wn = WN_kid1(wn);
    while (WN_opcode(wn) == opc_neg) {
      --limit;
      wn = WN_kid0(wn);
    }
  }
  return count;
}


static INT Nary_List_AddSub(WN *wn, WN **args, INT idx, BOOL negate, INT limit)
{
  Is_True(WN_operator(wn) == OPR_ADD || WN_operator(wn) == OPR_SUB ||
	  WN_operator(wn) == OPR_NEG,
	  ("Nary_List_AddSub expected OPR_ADD, OPR_SUB, or OPR_NEG"));
  TYPE_ID rtype = WN_rtype(wn);
  OPCODE opc_add = OPCODE_make_op(OPR_ADD, rtype, MTYPE_V);
  OPCODE opc_sub = OPCODE_make_op(OPR_SUB, rtype, MTYPE_V);
  OPCODE opc_neg = OPCODE_make_op(OPR_NEG, rtype, MTYPE_V);
  while (WN_opcode(wn) == opc_neg) {
    --limit;
    negate = ! negate;
    wn = WN_kid0(wn);
  }
  for (--limit; (WN_opcode(wn) == opc_add ||
		 WN_opcode(wn) == opc_sub) && limit >= 0; --limit) {
    WN *wn_kid0 = WN_kid0(wn);
    if (WN_opcode(wn_kid0) == opc_add || WN_opcode(wn_kid0) == opc_sub) {
      idx = Nary_List_AddSub(wn_kid0, args, idx, negate, limit);
    } else {
      if (negate) wn_kid0 = WN_Neg(rtype, wn_kid0);
      args[idx++] = wn_kid0;
    }
    if (WN_opcode(wn) == opc_sub) negate = ! negate;
    WN *wn_addsub = wn;
    wn = WN_kid1(wn);
    WN_Delete(wn_addsub);
    while (WN_opcode(wn) == opc_neg) {
      --limit;
      negate = ! negate;
      wn_addsub = wn;
      wn = WN_kid0(wn);
      WN_Delete(wn_addsub);
    }
  }
  if (WN_opcode(wn) == opc_add || WN_opcode(wn) == opc_sub) {
    Lmt_DevWarn(1, ("WN_ExprToNaryType ADD/SUB/NEG too deep, so stopping"));
  }
  if (negate) wn = WN_Neg(rtype, wn);
  args[idx++] = wn;
  return idx;
}


/* ====================================================================
 *
 * WN *WN_ExprToNary(WN *tree, TYPE_ID type)
 *
 * Create a Nary representation of the expression passed  
 *
 * The nary representation looks like an INTRINSIC_OP 	 
 * with an id = INTRN_NARY_ADD | INTRN_NARY_MPY	
 *
 * 14831: Handle all nested ADD/SUB combinations	
 *
 * ==================================================================== */


static WN *WN_ExprToNaryType(WN *tree)
{
  const INT depth_limit = 16;
  INT nkids, nkids0;
  WN **kids;
  OPCODE opc;
  TYPE_ID rtype;

  switch (WN_operator(tree)) {

  case OPR_MPY:
    nkids = Nary_Count_Mpy(tree, depth_limit);
    if (nkids > 2) {
      rtype = WN_rtype(tree);
      kids = (WN **) alloca(nkids * sizeof(WN *));
      nkids0 = Nary_List_Mpy(tree, kids, 0, depth_limit);
      Is_True(nkids == nkids0,
	      ("WN_ExprToNaryType: MPY Kids count mismatch"));
      opc = OPCODE_make_op(OPR_INTRINSIC_OP, rtype, MTYPE_V);
      tree = WN_Create_Intrinsic(opc, INTRN_NARY_MPY, nkids, kids);
    }
    break;

  case OPR_ADD:
  case OPR_SUB:
  case OPR_NEG:
    nkids = Nary_Count_AddSub(tree, depth_limit);
    if (nkids > 2) {
      rtype = WN_rtype(tree);
      kids = (WN **) alloca(nkids * sizeof(WN *));
      nkids0 = Nary_List_AddSub(tree, kids, 0, FALSE, depth_limit);
      Is_True(nkids == nkids0,
	      ("WN_ExprToNaryType: ADD/SUB/NEG Kids count mismatch"));
      opc = OPCODE_make_op(OPR_INTRINSIC_OP, rtype, MTYPE_V);
      tree = WN_Create_Intrinsic(opc, INTRN_NARY_ADD, nkids, kids);
    }
    break;
  }

  return tree;
}


/* ====================================================================
 *
 * WN *WN_NaryToExpr(WN *tree)
 *
 * The nary representation looks like an INTRINSIC_OP 	 
 * with an id = INTRN_NARY_ADD | INTRN_NARY_MPY
 *
 * ==================================================================== */


extern WN *WN_NaryToExpr(WN *tree)
{
  if (WN_nary_intrinsic(tree))
  {
    INT16	i;
    WN		*wn = WN_kid0(tree);
    INTRINSIC	id = (INTRINSIC) WN_intrinsic(tree);
    TYPE_ID	rtype = WN_rtype(tree);
    INT 	num_parms = WN_kid_count(tree);

    for (i = 1; i < num_parms; i++)
    {
      WN *actual = WN_kid(tree, i);

      actual = WN_NaryToExpr(actual);

      switch(id)
      {
      case INTRN_NARY_ADD:
	if (WN_operator_is(actual, OPR_NEG))
	{
	  wn = WN_Sub(rtype, wn, WN_kid0(actual));
	  WN_Delete(actual);
	}
	else
	{
	  wn = WN_Add(rtype, wn, actual);
	}
        break;
      case INTRN_NARY_MPY:
	wn = WN_Mpy(rtype, wn, actual);
        break;
      }
    }
    return wn;
  }

  return tree;
}


/* ====================================================================
 *
 * WN *WN_NaryDelete(WN *tree, INT32 n)
 *
 * Delete the nth kid of a nary intrinsic op
 * The rest of the children get moved and the num_kids are updated
 *
 * ==================================================================== */

static WN *WN_NaryDelete(WN *tree, INT32 n)
{
  INT32	i;
  INT32	num_kids= WN_kid_count(tree);

  Is_True((n<num_kids),("cannot delete nth kid"));
  Is_True(WN_nary_intrinsic(tree),("expected nary op"));

  for(i=n+1; i<num_kids; i++)
  {
    WN_actual(tree, i-1) = WN_actual(tree, i);
  }
  WN_set_kid_count(tree, WN_kid_count(tree)-1);

  return tree;
}


/* ====================================================================
 *
 * Get offset field (avoid preg offsets)
 * amazing I cannot find anything like this
 *
 * ==================================================================== */

extern INT64 lower_offset(WN *tree, INT64 offset)
{
  if (WN_has_offset(tree))
  {
    switch(WN_operator(tree))
    {
    case OPR_LDA:
      offset +=	WN_lda_offset(tree);
      break;
    case OPR_MSTORE:
    case OPR_ISTORE:
    case OPR_ISTOREX:
      offset +=	WN_store_offset(tree);
      break;
    case OPR_STID:
      if (WN_class(tree) != CLASS_PREG)
        offset +=	WN_store_offset(tree);
      break;
    case OPR_LDID:
      if (WN_class(tree) != CLASS_PREG)
	offset +=	WN_load_offset(tree);
      break;
    case OPR_MLOAD:
    case OPR_ILOAD:
    case OPR_ILOADX:
      offset +=	WN_load_offset(tree);
      break;
    }
  }
  return offset;
}


/* ====================================================================
 *
 * Compute alignment consistent with address and offset
 *
 * The new alignment may be improved (or not. see pv [559228])
 *
 * ==================================================================== */
extern TY_IDX compute_alignment_type(WN *tree, TY_IDX type, INT64 offset)
{
  INT32 newAlign;

  newAlign=	compute_alignment(tree, lower_offset(tree, 0));

  newAlign=	compute_offset_alignment(offset, newAlign);

  if (TY_align(type) != newAlign)
    Set_TY_align(type, newAlign);

  return type;
}

static INT32 compute_alignment(WN *tree, INT64 offset)
{
  WN	  *addr;
  TY_IDX   type;
  INT32	   align, align0, align1;

  switch(WN_operator(tree))
  {
  case OPR_MSTORE:
    type = TY_pointed(Ty_Table[WN_ty(tree)]);
    addr = WN_kid1(tree);

    if (WN_has_sym(addr) && WN_ty(addr))
    {
      return compute_alignment(addr, offset+WN_lda_offset(addr));
    }
    align = TY_align(type);
    break;

  case OPR_MLOAD:
    type = TY_pointed(Ty_Table[WN_ty(tree)]);
    addr = WN_kid0(tree);

    if (WN_has_sym(addr) && WN_ty(addr))
    {
      return compute_alignment(addr, offset+WN_lda_offset(addr));
    }
    align = TY_align(type);
    break;

  case OPR_ILOAD:
  case OPR_ILOADX:
  case OPR_LDA:
  case OPR_LDID:
    type = WN_ty(tree);
    if (TY_is_pointer(Ty_Table[type]))
    {
      type = TY_pointed(Ty_Table[type]);
    }
    else
    {
      return 1;
    }
    align = TY_align(type);
    break;

  case OPR_ARRAY:
    align=	compute_alignment(WN_array_base(tree), offset);
    offset=	WN_element_size(tree);
    break;

  case OPR_ADD:
  case OPR_SUB:
    align0=	compute_alignment(WN_kid0(tree), 0);
    align1=	compute_alignment(WN_kid1(tree), 0);
    align=	MIN(align0, align1);
    break;

  case OPR_INTCONST:
    offset=	WN_const_val(tree);
    align=	MTYPE_alignment(Max_Uint_Mtype);
    break;

  default:
    if (traceAlignment)
    {
      DevWarn("compute_alignment(): unrecognized WN returning alignment of 1");
    }
    return 1;
  }

  align=	compute_offset_alignment(offset, MAX(1, align));

  if (WN_has_sym(tree))
  {
    INT32	newAlign = align;
    ST		*sym = WN_st(tree);

    if (WN_operator_is(tree, OPR_LDA))
    {
      newAlign=	ST_alignment(sym);
    }
    else if (WN_operator_is(tree, OPR_LDID)	&&
	     ST_type(sym)			&&
	     TY_is_pointer(Ty_Table[ST_type(sym)]))
    {
      newAlign = TY_align( TY_pointed(Ty_Table[ST_type(sym)]));
    }

    align = compute_offset_alignment(offset, MAX(newAlign, align));
  }
  return align;
}


/* ====================================================================
 *
 * BOOL lower_is_aliased(WN *wn1, WN *wn2, INT64 size)
 *
 * Are these addresses aliased? (used in bcopy/memcpy/memmove)
 *
 * ==================================================================== */

extern BOOL lower_is_aliased(WN *wn1, WN *wn2, INT64 size)
{
  if (alias_manager &&
      Valid_alias(alias_manager, wn1) &&
      Valid_alias(alias_manager, wn2) &&
      (Aliased(alias_manager, wn1, wn2) == NOT_ALIASED))
  {
    return FALSE;
  }

  if (WN_operator_is(wn1, OPR_LDA) && WN_operator_is(wn2, OPR_LDA)) {
    ST	*sym1 = WN_st(wn1);
    ST	*sym2 = WN_st(wn2);
    ST	*base1, *base2;
    INT64 newoffset1, newoffset2;

    if (sym1 != sym2) return FALSE;

    Base_Symbol_And_Offset_For_Addressing(
		    sym1, WN_lda_offset(wn1), &base1, &newoffset1);
    Base_Symbol_And_Offset_For_Addressing(
		    sym2, WN_lda_offset(wn2), &base2, &newoffset2);

    if (ABS(newoffset1 - newoffset2) >= size) return FALSE;
  }

  if ((Alias_Pointer_Disjoint || Alias_Pointer_Restricted ||
      PU_ftn_lang(Get_Current_PU())) &&
      (WN_operator_is(wn1, OPR_LDID) || WN_operator_is(wn1, OPR_LDA)) &&
      (WN_operator_is(wn2, OPR_LDID) || WN_operator_is(wn2, OPR_LDA))) {
    ST *sym1 = WN_st(wn1);
    ST *sym2 = WN_st(wn2);
    if (sym1 != sym2 &&
	ST_class(sym1) == CLASS_VAR && ST_class(sym2) == CLASS_VAR &&
	! ST_is_temp_var(sym1) && ! ST_is_temp_var(sym2) &&
	! TY_is_f90_pointer(ST_type(sym1)) && ! ST_is_f90_target(sym1) &&
	! TY_is_f90_pointer(ST_type(sym2)) && ! ST_is_f90_target(sym2)) {
      return FALSE;
    }
  }

  return TRUE;
}


/* ====================================================================
 *
 * WN *lower_copy_tree(WN *tree, INT32 n)
 *
 * Copy the tree and duplicate the maps
 *
 * ==================================================================== */

static void lower_tree_copy_maps(WN *tree, WN *dup, LOWER_ACTIONS actions)
{
  if (tree == NULL)
  {
    Is_True((dup == NULL),("inconsistency while copying trees"));
  }
  Is_True((WN_opcode(tree) == WN_opcode(dup)),
	  ("inconsistency while copying trees"));

  if (WN_has_map_id(tree) && WN_has_alias_info(tree))
  {
    lower_copy_maps(tree, dup, actions);
  }
	
  if (WN_opcode(tree) == OPC_BLOCK)
  {
    WN  *treeStmt = WN_first(tree);
    WN  *dupStmt  = WN_first(dup);

    while(treeStmt)
    {
      lower_tree_copy_maps(treeStmt, dupStmt, actions);
      treeStmt = WN_next(treeStmt);
      dupStmt  = WN_next(dupStmt);
    }
  }
  else
  {
    INT	n;
    for(n = 0; n < WN_kid_count(tree); n++)
    {
      if (WN_kid(tree, n))
      {
	lower_tree_copy_maps(WN_kid(tree,n), WN_kid(dup,n), actions);
      }
    }
  }
}

static WN *lower_copy_tree(WN *tree, LOWER_ACTIONS actions)
{
  WN  *dup;

  dup = WN_COPY_Tree(tree);

  lower_tree_copy_maps(tree, dup, actions);

  return dup;
}

/* ====================================================================
 *
 * PREG_NUM AssignPregExprPos(WN *block, WN *tree, TY_IDX ty, SRCPOS srcpos,
 *                            LOWER_ACTIONS actions)
 *
 * PREG_NUM AssignExprPos(WN *block, WN *tree, TYPE_ID type, SRCPOS srcpos,
 *                        LOWER_ACTIONS)
 *
 * PREG_NUM AssignExpr(WN *block, WN *tree, TYPE_ID type)
 *
 * PREG_NUM AssignExprTY(WN *block, WN *tree, TY_IDX ty)
 *
 * Allocate a preg of type ST_type(preg) and assign expression tree to it
 * and attach it to block. 
 *
 * Assign srcpos (if not NULL)
 *
 * ==================================================================== */

 PREG_NUM AssignPregExprPos(WN *block, WN *tree, TY_IDX ty,
				  SRCPOS srcpos, LOWER_ACTIONS actions)
{
  PREG_NUM	pregNo;
  TYPE_ID	type;
  ST		*preg = MTYPE_To_PREG(TY_mtype(Ty_Table[ty]));

  Is_True((WN_operator_is(tree, OPR_PARM)==FALSE),("bad parm"));

  type = TY_mtype(Ty_Table[ty]);
  pregNo = Create_Preg(type, current_preg_name);

  {
    WN	*stBlock, *stid;

    stid = WN_Stid(type, pregNo, preg, ty, tree);

    if (srcpos)
      WN_Set_Linenum (stid, srcpos);

    stBlock = WN_CreateBlock();

   /*
    *	This lowering may lead to infinite regress if the childred
    * 	cannot be lowered (and are allocated a temp, for example) 
    */
    if (actions)
      stid = lower_store(stBlock, stid, actions);

    WN_INSERT_BlockLast(stBlock, stid);

    WN_INSERT_BlockLast(block, stBlock);
  }

  return pregNo;
}


extern PREG_NUM AssignExprTY(WN *block, WN *tree, TY_IDX type)
{
  return AssignPregExprPos(block, tree, type, current_srcpos,
			   current_actions);
}

extern PREG_NUM AssignExpr(WN *block, WN *tree, TYPE_ID type)
{
  return AssignPregExprPos(block, tree, MTYPE_To_TY(type), current_srcpos,
			   current_actions);
}


/*
 * AssignToPregExprPos()
 *
 * See Interface Description.
 */
void
AssignToPregExprPos(PREG_NUM pregNo, 
		    WN *block, WN *tree, TY_IDX ty,
		    SRCPOS srcpos, LOWER_ACTIONS actions)
{
  TYPE_ID	type;
  ST		*preg = MTYPE_To_PREG(TY_mtype(Ty_Table[ty]));

  Is_True((WN_operator_is(tree, OPR_PARM)==FALSE),("bad parm"));

  type = TY_mtype(Ty_Table[ty]);

  {
    WN	*stBlock, *stid;

    stid = WN_Stid(type, pregNo, preg, ty, tree);

    if (srcpos) {
      WN_Set_Linenum (stid, srcpos);
    }

    stBlock = WN_CreateBlock();

   /*
    *	This lowering may leed to infinite regress if the
    * 	children cannot be lowered (and are allocated a temp, for example) 
    */
    if (actions)
      stid = lower_stmt(stBlock, stid, actions);

    WN_INSERT_BlockLast(stBlock, stid);

    WN_INSERT_BlockLast(block, stBlock);
  }
}

/*
 * AssignToPregExprTY()
 *
 * See Interface Description.
 */
void
AssignToPregExprTY(PREG_NUM pregNo, WN *block, WN *tree, TY_IDX type)
{
  AssignToPregExprPos(pregNo, block, tree, type, current_srcpos,
		      current_actions);
}

/*
 * AssignToPregExpr()
 *
 * See Interface Description.
 */
void
AssignToPregExpr(PREG_NUM pregNo, WN *block, WN *tree, TYPE_ID type)
{
  AssignToPregExprPos(pregNo, block, tree, MTYPE_To_TY(type), current_srcpos,
			   current_actions);
}

static BOOL WN_unconditional_goto(WN *tree)
{
  switch(WN_operator(tree))
  {
  case OPR_GOTO:
  case OPR_REGION_EXIT:
  case OPR_RETURN:
  case OPR_RETURN_VAL:
#ifdef KEY
  case OPR_GOTO_OUTER_BLOCK:
#endif
    return TRUE;
  }
  return FALSE;
}




/* ====================================================================
 *
 * PARITY WN_parity(WN *tree)
 *
 * return the PARITY associated with a tree.
 *
 * Parity encapsulates dependence information, like complex real, imag
 * ==================================================================== */

PARITY WN_parity(WN *tree)
{
  if (WN_map_id(tree) != -1)
  {
    INT32	map;

    Is_True((lowering_parity_map != 0), ("parity map not initialized"));
#ifdef KEY
    // Node may be deleted in which case the corresponding vertex and the 
    // associated edges should be deleted in the dependence graph but there
    // are lots of places in wn_lower.cxx that delete WNs without doing the
    // appropriate update to the dependence graph. So, assume the worst here.
    if (WN_operator(tree) == OPERATOR_UNKNOWN) return PARITY_UNKNOWN;
#endif
    map = WN_MAP32_Get(lowering_parity_map, tree);
   
    if (map!=0)
      return (PARITY) map;
  }
  return PARITY_UNKNOWN;
}

BOOL WN_parity_independent(WN *wn1, WN *wn2)
{

  if (wn1 == NULL		||
      wn2 == NULL		||
      WN_map_id(wn1) == -1	||
      WN_map_id(wn2) == -1)
    return FALSE;

  {
    PARITY p1 = WN_parity(wn1);
    PARITY p2 = WN_parity(wn2);

   return (p1 & p2) ? FALSE : TRUE;
  }
}




/* ====================================================================
 *
 *	MAP PRESERVATION
 *
 *
 * void lower_copy_maps(WN *orig, WN *tree, LOWER_ACTIONS action)
 *
 * void lower_complex_maps(WN *orig, WN *real, WN *imag, LOWER_ACTIONS action)
 *
 * void lower_quad_maps(WN *orig, WN *hipart, WN *lopart, LOWER_ACTIONS action)
 *
 *
 *  copy alias information to tree
 *
 * ==================================================================== */

static void lower_maps_init(LOWER_ACTIONS actions)
{

}

static void lower_maps_reset(LOWER_ACTIONS actions)
{
  if (Action(LOWER_DEPGRAPH_MAPS))
  {
    LNOPruneMapsUsingParity();
  }
}

static void lower_map(WN *tree, LOWER_ACTIONS actions)
{
  if (Action(LOWER_ALIAS_MAPS))
  {
    if (alias_manager)
    {
      if (Valid_alias(alias_manager, tree) == FALSE)
	Create_alias(alias_manager, tree);
    }
  }
}

static void lower_copy_maps(WN *orig, WN *tree, LOWER_ACTIONS actions)
{
  if (orig == NULL)
    return;

 /*
  *	The tree may no longer be valid at the point of call
  *	(ie. may be deleted) so we must check validity
  */
  if (WN_has_map_id(orig))
  {
    if (WN_has_alias_info(orig)	&&
        WN_has_alias_info(tree))
    {
      if (Action(LOWER_PREFETCH_MAPS))
      {
        WN_CopyMap(tree, WN_MAP_PREFETCH, orig);
      }
      if (Action(LOWER_ALIAS_MAPS))
      {
#ifdef TARG_ST
	/* FdF 20050407: An alias will be created for tree by
	   lower_map. In case of OPR_PARM for parameters passed by
	   value, we would not have information to say NO_ALIAS. So
	   create a points_to on OPR_PARM, and copy it to tree. */
	if (alias_manager && Valid_alias(alias_manager, orig) == FALSE &&
	    WN_operator(orig) == OPR_PARM && WN_Parm_By_Value(orig)) {
	  Create_alias(alias_manager, orig);
	}
#endif
        if (alias_manager)
	  Copy_alias_info(alias_manager, orig, tree);
      }
    }
  }
#ifdef TARG_ST
  if (WN_operator(orig) != OPR_PARM)
#endif
  WN_Set_Flags(orig, tree);
}


/*
 * If an original node has a TY with an f90_pointer attribute on it, copy it
 * to the TYs of the new nodes. node2 might be NULL (for cases in which we
 * only produce one new node)
 */
static void lower_copy_tys (WN *orig, WN *node1, WN *node2) 
{
  TY_IDX  ty;

  if (WN_operator_is(orig, OPR_ILOAD))
  {
    ty =	WN_load_addr_ty(orig);

    if (TY_is_f90_pointer(Ty_Table[ty]))
    {
      if (node1 && WN_operator_is(node1, OPR_ILOAD))
	WN_set_load_addr_ty(node1, ty);

      if (node2 && WN_operator_is(node2, OPR_ILOAD))
	WN_set_load_addr_ty(node2, ty);
    }
  }
  else if (WN_operator_is(orig, OPR_ISTORE))
  {
    ty =	WN_ty(orig);
    if (TY_is_f90_pointer(Ty_Table[ty]))
    {
      if (node1 && WN_operator_is(node1, OPR_ISTORE))
	WN_set_ty(node1, ty);
      if (node2 && WN_operator_is(node2, OPR_ISTORE))
	WN_set_ty(node2, ty);
    }
  }
}

#ifdef TARG_ST
/* ==================================================================== 
 *   lower_hilo_maps
 *
 *   Update mappings for hipart and lopart.
 *   One of both may be NULL in case where the lowering only
 *   generated 1 subpart.
 * ==================================================================== 
 */ 
static void lower_hilo_maps (
  WN *orig, 
  WN *hipart, 
  WN *lopart,
  LOWER_ACTIONS actions,
  TYPE_ID orig_type
)
{
  if (hipart)
    lower_copy_maps(orig, hipart, actions);
  if (lopart)
    lower_copy_maps(orig, lopart, actions);
  lower_copy_tys(orig,hipart,lopart);

  if (Action(LOWER_DEPGRAPH_MAPS))
  {
    // Only work when both are not NULL, if false
    // we loose the mapping.
    if (hipart != NULL && lopart != NULL)
      LNOPreserveMapPair(orig, hipart, lopart);
  }

  if (Action(LOWER_PARITY_MAPS))
  {
    if (MTYPE_is_double(orig_type)){
      if (hipart)
	WN_MAP32_Set(lowering_parity_map, hipart, PARITY_DOUBLE_HI);
      if (lopart)
	WN_MAP32_Set(lowering_parity_map, lopart, PARITY_DOUBLE_LO);
    }
    else {
      // must be long long
      if (hipart)
	WN_MAP32_Set(lowering_parity_map, hipart, PARITY_LONGLONG_HI);
      if (lopart)
	WN_MAP32_Set(lowering_parity_map, lopart, PARITY_LONGLONG_LO);
    }
  }
}

#endif


static void lower_quad_maps(WN *orig, WN *hipart, WN *lopart,
			    LOWER_ACTIONS actions)
{
  lower_copy_maps(orig, hipart, actions);
  lower_copy_maps(orig, lopart, actions);
  lower_copy_tys(orig,hipart,lopart);

  if (Action(LOWER_DEPGRAPH_MAPS))
  {
    LNOPreserveMapPair(orig, hipart, lopart);
  }

  if (Action(LOWER_PARITY_MAPS))
  {
    WN_MAP32_Set(lowering_parity_map, hipart, PARITY_QUAD_HI);
    WN_MAP32_Set(lowering_parity_map, lopart, PARITY_QUAD_LO);
  }
}

static void lower_complex_maps(WN *orig, WN *real, WN *imag,
			       LOWER_ACTIONS actions)
{
  lower_copy_maps(orig, real, actions);
  lower_copy_maps(orig, imag, actions);
  lower_copy_tys(orig,real,imag);

  if (Action(LOWER_DEPGRAPH_MAPS))
  {
    LNOPreserveMapPair(orig, real, imag);
  }

  if (Action(LOWER_PARITY_MAPS))
  {
    WN_MAP32_Set(lowering_parity_map, real, PARITY_COMPLEX_REAL);
    WN_MAP32_Set(lowering_parity_map, imag, PARITY_COMPLEX_IMAG);
  }

}

static WN *add_to_base(WN **base, WN *tree)
{
  if (*base)
  {
    return WN_Add(Pointer_type, *base, tree);
  }
  return tree;
}

static WN *sub_from_base(WN **base, WN *tree)
{
  if (*base)
  {
    return WN_Sub(Pointer_type, *base, tree);
  }
  return tree;
}

static BOOL baseAddress(WN *tree)
{
  switch(WN_operator(tree))
  {
  case OPR_LDA:
  case OPR_LDID:
  case OPR_ILOAD:
  case OPR_ILOADX:
    return TRUE;
  }
  return FALSE;
}


/* ====================================================================
 *
 * void lower_to_base_index(WN *addr, WN **base, WN **index) 
 *
 * Pattern match an address and create a bad/index for it
 *
 * Before this routine we would store the address in a preg
 * and use it (twice).  This would require the ST being marked
 * addr_taken_stored (very bad)
 *
 * This is getting worse and worse. Now the routine is recursive
 * for OPR_ADD.
 * 
 * 17 Dec 1998, R. Shapiro - Add a default case so that this routine never 
 *   fails. At worst, the trees get a little bigger. 
 *
 * ==================================================================== */

static void lower_to_base_index(WN *addr, WN **base, WN **index) 
{
  WN	*l, *r;

  switch (WN_operator(addr))
  {
  case OPR_ARRAY:
    *base  = add_to_base(base, WN_array_base(addr));
    *index = add_to_base(index, addr);
    WN_array_base(addr) = WN_Zerocon(WN_rtype(addr));
    break;

  case OPR_ADD:
    l = WN_kid0(addr);
    r = WN_kid1(addr);

    if (baseAddress(l))
    {
      *base  = add_to_base(base, l);
      *index = add_to_base(index, r);
    }
    else if (baseAddress(r))
    {
      *base  = add_to_base(base, r);
      *index = add_to_base(index, l);
    }
    else if (WN_operator_is(r, OPR_ARRAY))
    {
      *base  = add_to_base(base, WN_array_base(r));
      WN_array_base(r) = WN_Zerocon(WN_rtype(r));
      *index = add_to_base(index, addr);
    }
    else if (WN_operator_is(l, OPR_ARRAY))
    {
      *base  = add_to_base(base, WN_array_base(l));
      WN_array_base(l) = WN_Zerocon(WN_rtype(l));
      *index = add_to_base(index, addr);
    }
    else if ((WN_operator_is(r, OPR_ADD)  ||  WN_operator_is(r, OPR_SUB))
	     && WN_operator_is(l, OPR_INTCONST))
    {
      lower_to_base_index(r, base, index);
      *index = add_to_base(index, l);
    }
    else if ((WN_operator_is(l, OPR_ADD)  ||  WN_operator_is(l, OPR_SUB)) 
	     && WN_operator_is(r, OPR_INTCONST))
    {
      lower_to_base_index(l, base, index);
      *index = add_to_base(index, r);
    }
    else
    {
      // Give up
      *base  = add_to_base(base, addr);
      *index = add_to_base(index, WN_Zerocon(WN_rtype(addr)));
    }
    break;

  case OPR_SUB:
    l = WN_kid0(addr);
    r = WN_kid1(addr);

    if (baseAddress(l))
    {
      *base  = add_to_base(base, l);
      *index = sub_from_base(index, r);
    }
    else if (baseAddress(r))
    {
      *base  = sub_from_base(base, r);
      *index = add_to_base(index, l);
    }
    else if (WN_operator_is(r, OPR_ARRAY))
    {
      *base  =	sub_from_base(base, WN_array_base(r));
      WN_array_base(r) = WN_Zerocon(WN_rtype(r));
      *index =	add_to_base(index, addr);
    }
    else if (WN_operator_is(l, OPR_ARRAY))
    {
      *base  = add_to_base(base, WN_array_base(l));
      WN_array_base(l) = WN_Zerocon(WN_rtype(l));
      *index = add_to_base(index, addr);
    }
    else if ((WN_operator_is(r, OPR_ADD) ||  WN_operator_is(r, OPR_SUB))
	     && WN_operator_is(l, OPR_INTCONST))
    {
      lower_to_base_index(r, base, index);
      *base  = WN_Neg(WN_rtype(*base),*base);
      *index = sub_from_base(&l,*index);
    }
    else if ((WN_operator_is(l, OPR_ADD) || WN_operator_is(l, OPR_SUB)) 
	     && WN_operator_is(r, OPR_INTCONST))
    {
      lower_to_base_index(l, base, index);
      *index = sub_from_base(index, r);
    }
    else
    {
      // Give up
      *base  = add_to_base(base, addr);
      *index = add_to_base(index, WN_Zerocon(WN_rtype(addr)));
    }
    break;
  default:
    // Give up
    *base  = add_to_base(base, addr);
    *index = add_to_base(index, WN_Zerocon(WN_rtype(addr)));
    break;
  }
}



/* ====================================================================
 *
 * LEAF Make_Leaf(WN *block, WN *tree, TYPE_ID type)
 *
 * Make an aribtrary expression tree into a leaf.
 * If the expression is an integer or floating point constant
 * no transformation is made. However, other expressions are stored
 * into a PREG.
 *
 * Make_Leaf is used in place of AssignExpr when performing a sort
 * of "poor man's" CSE, and you want to avoid creating unnecessary
 * PREGs for constants (which can also thwart the simplifier).
 *
 * ==================================================================== */

 LEAF Make_Leaf(WN *block, WN *tree, TYPE_ID type)
{
  LEAF leaf;
  leaf.type = type;
  switch (WN_operator(tree)) {
  case OPR_CONST:
    leaf.kind = LEAF_IS_CONST;
    leaf.u.tc = Const_Val(tree);
    WN_Delete(tree);
    break;
  case OPR_INTCONST:
    leaf.kind = LEAF_IS_INTCONST;
    leaf.u.intval = WN_const_val(tree);
    WN_Delete(tree);
    break;
  default:
    leaf.kind = LEAF_IS_PREG;
    leaf.u.n = AssignExpr(block, tree, type);
    break;
  }
  return leaf;
}

/* ====================================================================
 *
 * WN *Load_Leaf(const LEAF &leaf)
 *
 * Generate whirl to load the value of a leaf expression created by
 * Make_Leaf().
 *
 * ==================================================================== */

 WN *Load_Leaf(const LEAF &leaf)
{
  switch (leaf.kind) {
  case LEAF_IS_CONST:
    return Make_Const(leaf.u.tc);
  case LEAF_IS_INTCONST:
    return WN_CreateIntconst(OPR_INTCONST, leaf.type, MTYPE_V, leaf.u.intval);
  case LEAF_IS_PREG:
    return WN_LdidPreg(leaf.type, leaf.u.n);
  }
  FmtAssert(FALSE, ("unhandled leaf kind in Load_Leaf"));
  /*NOTREACHED*/
}

/* ====================================================================
 *
 * void lower_complex_expr(WN *block, WN *tree, LOWER_ACTIONS actions,
 *				 WN **realpart, WN **imagpart)
 *
 * Split complex-type expression <tree> into its lowered real and
 * imaginary parts.  <actions> must include LOWER_COMPLEX.  Note that
 * like the other lowering functions, this one destroys <tree>.  Upon
 * return, *<realpart> points to the real part, and *<imagpart> points
 * to the imaginary part.
 *
 * ==================================================================== */

static void lower_complex_expr(WN *block, WN *tree, LOWER_ACTIONS actions,
			       WN **realpart, WN **imagpart)
{
  TYPE_ID	type;


  Is_True(OPCODE_is_expression(WN_opcode(tree)),
	  ("expected expression node, not %s", OPCODE_name(WN_opcode(tree))));
  Is_True(Action(LOWER_COMPLEX), ("actions does not contain LOWER_COMPLEX"));
#ifndef TARG_X8664 // could be MTYPE_F8 that contains a MTYPE_C4
  Is_True((MTYPE_is_complex(WN_rtype(tree))),
	  ("expected complex-type node, not %s",
	   OPCODE_name(WN_opcode(tree))));
#endif
  type = Mtype_complex_to_real(WN_rtype(tree));

 /*
  *  Complex Arithmetic Notation
  *
  *  Let R(c) = real part of c
  *      I(c) = imaginary part of c
  *
  *  then, if z is complex it can be represented as follows
  *
  *      z= R(z) + I(z)i
  *
  */
  switch (WN_operator(tree))
  {
  case OPR_LDID:
    {
     /*
      *  we must create a new ST of type real
      *  we implicitly assume the storage of complex is
      *		 z =  {  R(z) , I(z) }
      */
      TY_IDX    beTY   = MTYPE_To_TY(type);
      WN_OFFSET offset = WN_load_offset(tree);

      if (WN_class(tree) == CLASS_CONST && offset == 0)
      {
	TCON	val = WN_val(tree);
	TYPE_ID	valType = WN_val_type(tree);

	if (WN_rtype(tree) == valType)
	{
	  *realpart = Make_Const( Extract_Complex_Real(val));
	  *imagpart = Make_Const( Extract_Complex_Imag(val));
 	  break;
	}
      }

      *realpart = WN_Ldid(type, offset,
			  coerceST(WN_st(tree), type),
			  beTY);

      *imagpart = WN_Ldid(type,
			  coerceOFFSET(tree, type, offset),
			  coerceST(WN_st(tree), type),
			  beTY);
    }
    break;

  case OPR_ILOAD:
    {
     /*
      *  we implicitly assume the storage of complex is
      *    z =  {  R(z) , I(z) }
      *
      *  The LOWER_BASE_INDEX will try to split the address into a
      *  base and index. The index is put in a preg (and reused) while
      *  the base is cloned.
      */
      WN_OFFSET offset = WN_load_offset(tree);

      if (Action(LOWER_BASE_INDEX))
      {
	WN	*addr, *base, *index;
	LEAF	indexN;

	base = index=	NULL;
	lower_to_base_index(WN_kid0(tree), &base, &index) ;

	base = lower_expr(block, base, actions);
	index = lower_expr(block, index, actions);

	indexN = Make_Leaf(block, index, Pointer_type);
	
	addr = WN_Add(Pointer_type,
		      Load_Leaf(indexN),
		      lower_copy_tree(base, actions)); 

	*realpart = WN_Iload(type,
			     offset,
			     coerceTY(WN_ty(tree), type),
			     addr);

	addr = WN_Add(Pointer_type, Load_Leaf(indexN), base);

	*imagpart = WN_Iload(type,
			     coerceOFFSET(tree, type, offset),
			     coerceTY(WN_ty(tree), type),
			     addr);
      }
      else
      {
	WN	*addr;
	LEAF	addrN;
	
	addr = lower_expr(block, WN_kid0(tree), actions);
	
	addrN = Make_Leaf(block, addr, Pointer_type);
	
	*realpart = WN_Iload(type,
			     offset,
			     coerceTY(WN_ty(tree), type),
			     Load_Leaf(addrN));
	
	*imagpart = WN_Iload(type,
			     coerceOFFSET(tree, type, offset),
			     coerceTY(WN_ty(tree), type),
			     Load_Leaf(addrN));
      }
      lower_complex_maps(tree, *realpart, *imagpart, actions);
    }
    break;

  case OPR_ILOADX:
    Is_True(FALSE, ("unexpected complex ILOADX"));
    break;

  case OPR_NEG:
    {
     /*
      *    -z  = -R(z) + -I(z)i
      */
      WN	*rz, *iz;

      lower_complex_expr(block, WN_kid0(tree), actions, &rz, &iz);
      *realpart = WN_Neg( type, rz);
      *imagpart = WN_Neg( type, iz);

    }
    break;

  case OPR_ADD:
    {
      WN	*rz, *rw, *iz, *iw;
     /*
      *    z + w = (R(z) + R(w)) + (I(z) + I(w))i
      */

      lower_complex_expr(block, WN_kid0(tree), actions, &rz, &iz);
      lower_complex_expr(block, WN_kid1(tree), actions, &rw, &iw);
 
      *realpart = WN_Add( type, rz, rw);
      *imagpart = WN_Add( type, iz, iw);
    }
    break;

  case OPR_SUB:
    {
     /*
      *    z - w = (R(z) - R(w)) + (I(z) - I(w))i
      */
      WN	*rz, *rw, *iz, *iw;

      lower_complex_expr(block, WN_kid0(tree), actions, &rz, &iz);
      lower_complex_expr(block, WN_kid1(tree), actions, &rw, &iw);
 
      *realpart = WN_Sub( type, rz, rw);
      *imagpart = WN_Sub( type, iz, iw);
    }
    break;

  case OPR_MPY:
    {
     /*
      *    z * w = (R(z)*R(w) - I(z)*I(w)) + (R(z)*I(w)+ R(w)*I(z))i
      *
      */
      WN	*rz, *rw, *iz, *iw;
      LEAF	rzN, rwN, izN, iwN;

      lower_complex_expr(block, WN_kid0(tree), actions, &rz, &iz);
      lower_complex_expr(block, WN_kid1(tree), actions, &rw, &iw);

      rzN = Make_Leaf(block, rz, type);
      rwN = Make_Leaf(block, rw, type);
      izN = Make_Leaf(block, iz, type);
      iwN = Make_Leaf(block, iw, type);

      *realpart = WN_Sub(type,
			 WN_Mpy(type, Load_Leaf(rzN), Load_Leaf(rwN)),
			 WN_Mpy(type, Load_Leaf(izN), Load_Leaf(iwN)));

      *imagpart = WN_Add(type,
			 WN_Mpy(type, Load_Leaf(rzN), Load_Leaf(iwN)),
			 WN_Mpy(type, Load_Leaf(rwN), Load_Leaf(izN)));
    }
    break;

  case OPR_COMPLEX:
    /*
     *	Create a complex number from two real children, ie.
     *    z = CMPLX(x,y)
     *    z = (x) + (y) i
     */
    *realpart = lower_expr(block, WN_kid0(tree), actions);
    *imagpart = lower_expr(block, WN_kid1(tree), actions);
    // Fix for bug# 414
    //WN_Delete(tree);
    break;

  case OPR_MADD:
  case OPR_MSUB:
  case OPR_NMADD:
  case OPR_NMSUB:
    Is_True( FALSE, ("unexpected complex madd"));
    break;

  case OPR_CVT:
    {
      WN	*rz, *iz;
      TYPE_ID	desc = WN_desc(tree);

      lower_complex_expr(block, WN_kid0(tree), actions, &rz, &iz);
 
      *realpart = WN_Cvt( type, Mtype_complex_to_real(desc), rz);
      *imagpart = WN_Cvt( type, Mtype_complex_to_real(desc), iz);
    }
    break;

  case OPR_CONST:
    {
     /*
      *	extract the real and imaginary parts of the complex number
      */
      TCON	val = Const_Val(tree);

      *realpart = Make_Const( Extract_Complex_Real(val));
      *imagpart = Make_Const( Extract_Complex_Imag(val));

      // Fix for Bug# 496
      //WN_Delete(tree);
    }
    break;

  case OPR_RSQRT:
    {
      TYPE_ID	desc = WN_desc(tree);
      WN	*div;

      div = WN_Div(desc,
		   WN_Floatconst(desc, 1.0),
		   WN_Sqrt(desc, WN_kid0(tree)));

      lower_complex_expr(block, div, actions, realpart, imagpart);

      WN_Delete(tree);
    }
    break;

  case OPR_SQRT:
    lower_complex_emulation(block, tree, actions, realpart, imagpart);
    break;


  case OPR_PAREN:
    {
      lower_complex_expr(block, WN_kid0(tree), actions, realpart, imagpart);

      *realpart = WN_Paren(type, *realpart);
      *imagpart = WN_Paren(type, *imagpart);
      WN_Delete(tree);
    }
    break;

  case OPR_ARRAY:
    break;

  case OPR_RECIP:
    {
      /* TODO_FEEDBACK: Get frequencies right, especially when we
       * create an IF statement. We have to assume half the frequency
       * goes to the then part and half goes to the else part; there's
       * no other information available, is there?
       */

      LEAF	rzN, izN;
      {
       /*
	*  assign pregs to their corresponding expressions
	*  Since the expressions will be reused, this avoids building a DAG
	*/
	WN	*rz, *iz; 
    
	lower_complex_expr(block, WN_kid0(tree), actions, &rz, &iz);
      
	rzN = Make_Leaf(block, rz, type);
	izN = Make_Leaf(block, iz, type);
      }
    
      if (Fast_Complex_Allowed)
      {
       /*
	*   1 / z
	*    real =	(   R(z) ) /  ( R(z)**2 + I(z)**2 )
	*    imag =	( - I(z) ) /  ( R(z)**2 + I(z)**2 )
	*
	*/
	LEAF	denomN;
	WN	*rz2, *iz2, *add;
    
	rz2 = WN_Mpy(type, Load_Leaf(rzN), Load_Leaf(rzN));
	iz2 = WN_Mpy(type, Load_Leaf(izN), Load_Leaf(izN));
	add = WN_Add(type, rz2, iz2);
	denomN = Make_Leaf(block, add, type);
    
	*realpart = WN_Div(type, Load_Leaf(rzN), Load_Leaf(denomN));
	*imagpart = WN_Neg(type,
			   WN_Div(type, Load_Leaf(izN), Load_Leaf(denomN)));
      }
      else
      {
       /*
	*   1 / z
	*    real =	(   R(z) ) /  ( R(z)**2 + I(z)**2 )
	*    imag =	( - I(z) ) /  ( R(z)**2 + I(z)**2 )
	*
	*    After factoring out max( |R(z)| , |I(z)| )
	*
	*	| R(z) | >  | I(z) |
	*
	*	Let x = I(z) / R(z)
	*	real = ( 1 )    /  ( R(z) + I(z)*x)
	*	imag = ( - x )  /  ( R(z) + I(z)*x)
	*
	*	| I(z) | >  | R(z) |
	*
	*	Let x = R(z) / I(z)
	*	real = ( x )   /  ( R(z)*x + I(z) )
	*	imag = (-1 )   /  ( R(z)*x + I(z) )
	*
	*/
	WN		*if_then, *if_else, *IF;
	PREG_NUM	realpartN, imagpartN;
	LEAF		xN, arzN, aizN;
    
	{
	  WN	*numer, *denom, *div;
    
	  arzN = Make_Leaf(block,
			   WN_Abs(type, Load_Leaf(rzN)),
    			   type);
    
	  aizN = Make_Leaf(block,
    			   WN_Abs(type, Load_Leaf(izN)),
    			   type);
    
	 /*
	  *  numer =  | R(w) | >  | I(w) |  ?  I(w) : R(x)
	  *  denom =  | R(w) | >  | I(w) |  ?  R(w) : I(x)
	  *
	  *  Let x = numer / denom
	  */
	  numer = WN_Select(type,
			    WN_GT(type, Load_Leaf(arzN), Load_Leaf(aizN)),
    			    Load_Leaf(izN),
    			    Load_Leaf(rzN));
	  denom = WN_Select(type,
			    WN_GT(type, Load_Leaf(arzN), Load_Leaf(aizN)),
    			    Load_Leaf(rzN),
    		  	    Load_Leaf(izN));

	  div = WN_Div(type, numer, denom);

	  xN = Make_Leaf(block, div, type);
	}

	if_then = WN_CreateBlock();
	{
	 /*
	  *	scale = ( R(z) + I(z)*x )
	  *	real = ( 1 )    /  scale
	  *	imag = ( - x )  /  scale
	  */
	  WN	*scale, *div;
	  LEAF	scaleN;
    
	  scale = WN_Add(type,
			 Load_Leaf(rzN),
			 WN_Mpy(type, Load_Leaf(izN), Load_Leaf(xN)));
	  scaleN = Make_Leaf(if_then, scale, type);

	  div = WN_Inverse(type, Load_Leaf(scaleN));

	  realpartN = AssignExpr(if_then, div, type);

	  div =  WN_Div(type,
			WN_Neg(type, Load_Leaf(xN)),
			Load_Leaf(scaleN));
    
	  imagpartN = AssignExpr(if_then, div, type);
	}

	if_else = WN_CreateBlock();
	{
	 /*
	  *	scale =	( R(z)*x + I(z) )
	  *	real = ( x )   /  scale
	  *	imag = ( 1 )   /  scale
	  */
	  WN	*scale, *div, *stid;
	  LEAF	scaleN;
	  ST	*preg =	MTYPE_To_PREG(type);
    
	  scale = WN_Add(type,
			 WN_Mpy(type, Load_Leaf(rzN), Load_Leaf(xN)),
			 Load_Leaf(izN));
	  scaleN = Make_Leaf(if_else, scale, type);

	  div =  WN_Div(type, Load_Leaf(xN), Load_Leaf(scaleN));
	  stid = WN_StidIntoPreg(type, realpartN, preg, div);
	  WN_Set_Linenum(stid, current_srcpos);  // Bug 1268
	  WN_INSERT_BlockLast(if_else, stid);
    
	  div = WN_Neg(type,WN_Inverse(type, Load_Leaf(scaleN)));
	  stid = WN_StidIntoPreg(type, imagpartN, preg, div);
	  WN_Set_Linenum(stid, current_srcpos);  // Bug 1268
	  WN_INSERT_BlockLast(if_else, stid);
	}
    
	IF =  WN_CreateIf(WN_GT(type, Load_Leaf(arzN), Load_Leaf(aizN)),
			  if_then, if_else);

	if ( Cur_PU_Feedback ) {
	  Cur_PU_Feedback->Annot( IF, FB_EDGE_BRANCH_TAKEN, FB_FREQ_UNKNOWN );
	  Cur_PU_Feedback->Annot( IF, FB_EDGE_BRANCH_NOT_TAKEN,
				  FB_FREQ_UNKNOWN );
	}

	WN_INSERT_BlockLast(block, lower_if(block, IF, actions));
    
	*realpart = WN_LdidPreg(type, realpartN);
	*imagpart = WN_LdidPreg(type, imagpartN);
      }
      WN_Delete(tree);
    }
    break;

  case OPR_DIV:
    {
     /*
      *  assign pregs to their corresponding expressions
      *  Since the expressions will be reused, this avoids building a DAG
      */
      WN	*rz, *rw, *iz, *iw; 
      LEAF	rzN, rwN, izN, iwN;

      lower_complex_expr(block, WN_kid0(tree), actions, &rz, &iz);
      lower_complex_expr(block, WN_kid1(tree), actions, &rw, &iw);
  
      rzN = Make_Leaf(block, rz, type);
      izN = Make_Leaf(block, iz, type);
      rwN = Make_Leaf(block, rw, type);
      iwN = Make_Leaf(block, iw, type);

      if (Fast_Complex_Allowed)
      {
       /*
	*   z / w
	*    real =	(R(z)*R(w) + I(z)*I(w) /  ( R(w)**2 + I(w)**2 )
	*    imag =	(I(z)*R(w) - R(z)*I(w) /  ( R(w)**2 + I(w)**2 )
	*
	*/
	LEAF	denomN;
	{
	  WN	*rw2, *iw2, *add;
    
	  rw2 = WN_Mpy(type,Load_Leaf(rwN), Load_Leaf(rwN));
	  iw2 = WN_Mpy(type, Load_Leaf(iwN), Load_Leaf(iwN));
	  add = WN_Add(type, rw2, iw2);
	  denomN = Make_Leaf(block, add, type);
	}
	{
	  WN	*rzrw, *iziw;
    
	  rzrw = WN_Mpy(type, Load_Leaf(rzN), Load_Leaf(rwN));
	  iziw = WN_Mpy(type, Load_Leaf(izN), Load_Leaf(iwN));
	  *realpart = WN_Div(type,
    			 WN_Add(type, rzrw, iziw),
    			 Load_Leaf(denomN));
	}
	{
	  WN	*rziw, *izrw;
    
	  izrw = WN_Mpy(type, Load_Leaf(izN), Load_Leaf(rwN));
	  rziw = WN_Mpy(type, Load_Leaf(rzN), Load_Leaf(iwN));
	  *imagpart = WN_Div(type,
    			 WN_Sub(type, izrw, rziw),
    			 Load_Leaf(denomN));
	}
      }
      else
      {
       /*
	*   z / w
	*    real =	(R(z)*R(w) + I(z)*I(w) /  ( R(w)**2 + I(w)**2 )
	*    imag =	(I(z)*R(w) - R(z)*I(w) /  ( R(w)**2 + I(w)**2 )
	*
	*    After factoring out max( |R(w)| , |I(w)| )
	*
	*	| R(w) | >  | I(w) |
	*
	*	Let x = I(w) / R(w)
	*	  real = ( R(z) + I(z)*x ) /  ( R(w) + I(w)*x )
	*	  imag = ( I(z) - R(z)*x ) /  ( R(w) + I(w)*x )
	*
	*	| I(w) | >  | R(w) |
	*
	*	Let x = R(w) / I(w)
	*	  real = ( R(z)*x + I(z) ) /  ( R(w)*x + I(w) )
	*	  imag = ( I(z)*x - R(z) ) /  ( R(w)*x + I(w) )
	*
	*/
	WN		*if_then, *if_else, *IF;
	LEAF		xN, arwN, aiwN;
	PREG_NUM	realpartN, imagpartN;
	{
	  WN	*numer, *denom, *div;
    
	  arwN = Make_Leaf(block,
    			   WN_Abs(type, Load_Leaf(rwN)),
    			   type);
	  aiwN = Make_Leaf(block,
    			   WN_Abs(type, Load_Leaf(iwN)),
    			   type);
    
	 /*
	  *  numer =  | R(w) | >  | I(w) |  ?  I(w) : R(x)
	  *  denom =  | R(w) | >  | I(w) |  ?  R(w) : I(x)
	  *
	  *  Let x = numer / denom
	  */
	  numer = WN_Select(type,
			    WN_GT(type, Load_Leaf(arwN), Load_Leaf(aiwN)),
    			    Load_Leaf(iwN),
			    Load_Leaf(rwN));
	  denom = WN_Select(type,
			    WN_GT(type, Load_Leaf(arwN), Load_Leaf(aiwN)),
    			    Load_Leaf(rwN),
    		 	    Load_Leaf(iwN));
	  div = WN_Div(type, numer, denom);
	  xN = Make_Leaf(block, div, type);
	}

	if_then = WN_CreateBlock();
	{
	  WN		*scale;
	  LEAF	scaleN;
    
	  scale = WN_Add(type,
			 Load_Leaf(rwN),
			 WN_Mpy(type, Load_Leaf(iwN), Load_Leaf(xN)));
	  scaleN = Make_Leaf(if_then, scale, type);
	  {
	   /*
    	    *  real = ( R(z)   + I(z)*x )  / scale
    	    */
    	    WN	*numer, *div;
    
    	    numer = WN_Add(type,
  			   Load_Leaf(rzN),
  			   WN_Mpy(type, Load_Leaf(izN), Load_Leaf(xN)));
	    div = WN_Div(type, numer, Load_Leaf(scaleN));
	    realpartN = AssignExpr(if_then, div, type);
	  }
	  {
	   /*
	    *  imag = ( I(z) - R(z)*x ) /  scale
    	    */
    	    WN	*numer, *div;
    
	    numer = WN_Sub(type,
    			   Load_Leaf(izN),
			   WN_Mpy(type, Load_Leaf(rzN), Load_Leaf(xN)));
	    div = WN_Div(type, numer, Load_Leaf(scaleN));
    	    imagpartN = AssignExpr(if_then, div, type);
	  }
	}
    
	if_else = WN_CreateBlock();
	{
	  WN	*scale;
	  LEAF	scaleN;
	  ST	*preg =	MTYPE_To_PREG(type);

	  scale = WN_Add(type,
			 WN_Mpy(type, Load_Leaf(rwN), Load_Leaf(xN)),
    			 Load_Leaf(iwN));
	  scaleN = Make_Leaf(if_else, scale, type);
	  {
	   /*
    	    *  real = ( R(z)*x + I(z) ) /  scale
    	    *  store away result in an already defined preg
    	    */
    	    WN	*numer, *div, *stid;
    
    	    numer =  WN_Add(type,
    			    WN_Mpy(type, Load_Leaf(rzN), Load_Leaf(xN)),
    			    Load_Leaf(izN));
	    div = WN_Div(type, numer, Load_Leaf(scaleN));
	    stid = WN_StidIntoPreg(type, realpartN, preg, div);
	    WN_Set_Linenum(stid, current_srcpos);  // Bug 1268
	    WN_INSERT_BlockLast(if_else, stid);
	  }
	  {
	   /*
	    *  imag = ( I(z)*x - R(z) ) /  scale
    	    *  store away result in an already defined preg
    	    */
    	    WN	*numer, *div, *stid;
    
    	    numer = WN_Sub(type,
    			   WN_Mpy(type, Load_Leaf(izN), Load_Leaf(xN)),
    			   Load_Leaf(rzN));
	    div = WN_Div(type, numer, Load_Leaf(scaleN));
	    stid = WN_StidIntoPreg(type, imagpartN, preg, div);
	    WN_Set_Linenum(stid, current_srcpos);  // Bug 1268
	    WN_INSERT_BlockLast(if_else, stid);
	  }
	}
    
	IF =  WN_CreateIf(WN_GT(type, Load_Leaf(arwN), Load_Leaf(aiwN)),
			  if_then, if_else);

	if ( Cur_PU_Feedback ) {
	  Cur_PU_Feedback->Annot( IF, FB_EDGE_BRANCH_TAKEN, FB_FREQ_UNKNOWN );
	  Cur_PU_Feedback->Annot( IF, FB_EDGE_BRANCH_NOT_TAKEN,
				  FB_FREQ_UNKNOWN );
	}
    
	WN_INSERT_BlockLast(block, lower_if(block, IF, actions));
    
	*realpart = WN_LdidPreg(type, realpartN);
	*imagpart = WN_LdidPreg(type, imagpartN);
      }
      WN_Delete(tree);
    }
    break;

  case OPR_RND:
  case OPR_TRUNC:
  case OPR_MOD:
  case OPR_REM:
  case OPR_ABS:
    Fail_FmtAssertion("unexpected complex op (%s)",
		      OPCODE_name(WN_opcode(tree)));
    /*NOTREACHED*/

  case OPR_INTRINSIC_OP:
    {
      INTRINSIC     id = (INTRINSIC) WN_intrinsic(tree);

      switch(id)
      {
      case INTRN_C4CONJG:
      case INTRN_C8CONJG:
      case INTRN_CQCONJG:
      NOT_SUPPORT_ST(case INTRN_C16CONJG:)
	{
	  WN	*iz;

	  lower_complex_expr(block, WN_actual(tree, 0), actions,
			     realpart, &iz);

	  *imagpart = WN_Neg(type, iz);

	  WN_Delete(tree);
	}
        break;

	//*****************************************************************
	//
	// N.B. Any complex intrinsic which does not have an emulation must
	// appear in the list below.
	//

	//       case INTRN_F4CIS:
	//case INTRN_F8CIS:
	//case INTRN_FQCIS:
	//  actions |= LOWER_INTRINSIC;
	  /* Fall Through */

      default:
	if (INTRN_is_actual(WN_intrinsic(tree)))
	{
	  tree = lower_intrinsic(block, tree, actions);
	}
	else
	{
	  lower_complex_emulation(block, tree, actions, realpart, imagpart);
	}
        break;
      }
    }
    break;

  case OPR_SELECT:
    {
       WN *r1, *i1;
       WN *r2, *i2;
       LEAF cond;
       
       cond = Make_Leaf(block, WN_kid0(tree), WN_rtype(WN_kid0(tree)));
       
       lower_complex_expr(block, WN_kid1(tree), actions, &r1, &i1);
       lower_complex_expr(block, WN_kid2(tree), actions, &r2, &i2);
       
       *realpart = WN_Select(type, Load_Leaf(cond), r1, r2);
       *imagpart = WN_Select(type, Load_Leaf(cond), i1, i2);
    }
    break;

  case OPR_PARM:
    lower_complex_expr(block, WN_kid0(tree), actions, realpart, imagpart);
    break;
  }
}




/* ====================================================================
 *
 * void lower_quad_expr(WN *block, WN *tree, LOWER_ACTIONS actions,
 *				 WN **hipart, WN **lopart)
 *
 * ==================================================================== */

static void lower_quad_expr(WN *block, WN *tree, LOWER_ACTIONS actions,
				     WN **hipart, WN **lopart)
{
  TYPE_ID	type =	MTYPE_F8;


  Is_True(OPCODE_is_expression(WN_opcode(tree)),
	  ("expected expression node, not %s", OPCODE_name(WN_opcode(tree))));
  Is_True(actions & LOWER_QUAD, ("actions does not contain LOWER_QUAD"));
  Is_True((MTYPE_is_quad(WN_rtype(tree))),
	  ("expected quad-type node, not %s", OPCODE_name(WN_opcode(tree))));
 /*
  *  first lower the quad as expressions.
  *
  *  This will create and serialize a call chain
  *  that should always return a leaf that can be decomposed
  */
  tree = lower_expr(block, tree, actions);

  switch (WN_operator(tree))
  {
  case OPR_LDID:
    {
     /*
      *  we must create a new ST of type real*8
      */
      TY_IDX    beTY   = MTYPE_To_TY(type);
      WN_OFFSET offset = WN_load_offset(tree);

      *hipart = WN_Ldid(type, offset,
			coerceST(WN_st(tree), type), beTY);

      *lopart = WN_Ldid(type,
			coerceOFFSET(tree, type, offset),
			coerceST(WN_st(tree), type),
			beTY);

      lower_quad_maps(tree, *hipart, *lopart, actions);
    }
    break;

  case OPR_ILOAD:
    {
      WN	*addr;
      WN_OFFSET offset = WN_load_offset(tree);
      LEAF	addrN;

      addr = lower_expr(block, WN_kid0(tree), actions);

      addrN = Make_Leaf(block, addr, Pointer_type);

      *hipart = WN_Iload(type,
			 offset,
			 coerceTY(WN_ty(tree), type),
			 Load_Leaf(addrN));

      *lopart = WN_Iload(type,
			 coerceOFFSET(tree, type, offset),
			 coerceTY(WN_ty(tree), type),
			 Load_Leaf(addrN));

      lower_quad_maps(tree, *hipart, *lopart, actions);
    }
    break;

  case OPR_CONST:
    {
      TCON	val = Const_Val(tree);

      *hipart = Make_Const( Extract_Quad_Hi(val));
      *lopart = Make_Const( Extract_Quad_Lo(val));

    }
    break;

  case OPR_PAREN:
    {
     /*
      *	preserve the parens
      */
      lower_quad_expr(block, WN_kid0(tree), actions, hipart, lopart);

      *hipart = WN_Paren(type, *hipart);
      *lopart = WN_Paren(type, *lopart);
    }
    break;

  case OPR_PARM:
    lower_quad_expr(block, WN_kid0(tree), actions, hipart, lopart);
    break;

  case OPR_ILOADX:
    Is_True( FALSE, ("unexpected QUAD ILOADX"));
    break;

  default:
    Is_True((FALSE),
	    ("lower_quad_expr didn't %s", OPCODE_name(WN_opcode(tree))));
  }
}




static WN *WN_Coerce(TYPE_ID dst, WN *expr)
{
  TYPE_ID src= WN_rtype(expr);

  if (MTYPE_size_min(dst) == MTYPE_size_min(src))
    return expr;
#ifdef KEY // avoid ambiguity
  if (src == MTYPE_I4 && dst == MTYPE_U8)
    dst = MTYPE_I8;
  if (src == MTYPE_U4 && dst == MTYPE_I8)
    src = MTYPE_I4;
#endif
  return WN_Cvt(src, dst, expr);
}




/* ====================================================================
 *
 * WN *lower_linearize_array_addr(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on ARRAY expr <tree>,
 * returning linearized address expression tree.  <actions> must
 * contain LOWER_ARRAY.
 *
 *  We are assuming that the index and array_dim information are unique
 *
 * Note: If the element size is < 0, it means that we have a non-contiguous F90
 *       array. These are linearized slightly differently. Instead of
 *       the extent children being extents, they are stride multiplier factors. 
 *       The sum of the products of the index and stride multiplier is scaled
 *       by -element_size to get the offset from the base.  
 * ==================================================================== */

static WN *lower_linearize_array_addr(WN *block, WN *tree,
				      LOWER_ACTIONS actions)
{
  TYPE_ID rtype = WN_rtype(tree);
  WN		*result, *product;
  INT32	        i;
  BOOL          is_non_contig=FALSE;
  WN_ESIZE      element_size;

  Is_True((WN_operator_is(tree,OPR_ARRAY)),
	  ("expected ARRAY node, not %s", OPCODE_name(WN_opcode(tree))));
  Is_True(Action(LOWER_ARRAY), ("actions doesn't contain LOWER_ARRAY"));
  Is_True(WN_num_dim(tree) > 0, ("WN_num_dim of ARRAY node not > 0"));

  const INT32 n = WN_num_dim(tree);
  element_size = WN_element_size(tree);
  if (element_size < 0) {
     is_non_contig = TRUE;
     element_size = -element_size;
  }

#ifdef TARG_X8664
  BOOL do_reassociate = FALSE;
#endif

  if (is_non_contig)
  {
     WN *stride_mult;
     
     result = WN_Coerce(rtype, WN_array_index(tree, n-1));
     stride_mult = WN_Coerce(rtype, WN_array_dim(tree, n-1));
     result = WN_Mpy(rtype,result,stride_mult);

     for (i = n-2; i >= 0; i--) {
	product = WN_Coerce(rtype, WN_array_index(tree, i));
	stride_mult = WN_Coerce(rtype, WN_array_dim(tree, i));
	product = WN_Mpy(rtype,product,stride_mult);
	result = WN_Add(rtype,result,product);
     }
  }
  else
  {
#ifdef TARG_X8664
    result = NULL;
    do_reassociate = FALSE;

    if( n > 1 && loop_info_stack[current_loop_nest_depth] != NULL ){
      WN* idv = WN_array_index( tree, n-1 );
      while( idv != NULL ){
	if( WN_has_sym(idv) )
	  break;
	if( WN_kid_count(idv) == 0 ){
	  idv = NULL;
	  break;
	}
	idv = WN_kid0( idv );
      }

      if( idv != NULL ){
	do_reassociate = 
	  WN_st_idx( idv ) == WN_st_idx( loop_info_stack[current_loop_nest_depth] );
	// for stride cases ...
	do_reassociate |= ( WN_class(idv) == CLASS_PREG );

	// You can turn me off if you don't like me :(
	//do_reassociate = FALSE;
      }
    }

    if( !do_reassociate ){
      result = WN_Coerce(rtype, WN_array_index(tree, n-1));
    }
#else
   /*
    *  result <- index[n-1]
    */
    result = WN_Coerce(rtype, WN_array_index(tree, n-1));
#endif
     
   /*
    *  result <- result + index[i] * ( dim[n-1] * dim[n-2] ... dim[i+1] )
    */
    for (i = n-2; i >= 0; i--)
    {
      INT32	m;
      WN	*mpy;
	     
      product = WN_Coerce(rtype, lower_copy_tree(WN_array_dim(tree, n-1),
						 actions));
      for (m=n-2; m>i; m--)
      {
	product = WN_Mpy(rtype,
			 product,
			 WN_Coerce(rtype, lower_copy_tree(WN_array_dim(tree, m),
							  actions)));
      }

      mpy = WN_Mpy(rtype,
		   WN_Coerce(rtype, WN_array_index(tree,i)),
		   product);
#ifdef TARG_X8664
      if( result == NULL ){
	result = mpy;
	continue;
      }
#endif
      result = WN_Add(rtype, result, mpy);
    }
  }  
  

  /*
   *  result <- base + result * elm_size
   */
  {
    WN  *elm_size;

    elm_size = WN_Intconst(rtype, element_size);
    result = WN_Add(rtype,
		    WN_array_base(tree),
		    WN_Mpy(rtype, result, elm_size));
#ifdef TARG_X8664
    if( do_reassociate ){
      /*
       *  result <- result +  index[n-1] * elm_size
       */
      WN* elm_size1 = WN_Intconst(rtype, element_size);
      WN* inv_wn = WN_Coerce(rtype, WN_array_index( tree, n-1 ));
      result = WN_Add(rtype,
		      result,
		      WN_Mpy(rtype, inv_wn, elm_size1) );
    }
#endif
  }

  result = lower_expr(block, result, actions);

  WN_Delete(tree);	    /* ARRAY node not used */

  return result;
}




/* ====================================================================
 *
 * WN *lower_unsigned_to_float(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on conversions <tree>,
 * returning lowered expression tree.
 *
 * ==================================================================== */

static WN *lower_unsigned_to_float(WN *block, WN *expr, TYPE_ID src,
				   TYPE_ID dst, LOWER_ACTIONS actions)
{
  LEAF	srcNo, dstNo;


  /* Special case of U4. Don't do this special case if the cvt.d.l 
   * instruction is slow. This is true on the R5000 which emulates
   * this in software due to a chip bug.
   */
  if (src == MTYPE_U4 && !Slow_CVTDL)
  {
     BOOL  simp=	WN_Simplifier_Enable(FALSE);
     WN * r;

     r = WN_Cvt(MTYPE_I8,dst,WN_Cvt(MTYPE_U4, MTYPE_U8, expr));

     WN_Simplifier_Enable(simp);
     return (r);
  }


  /*
   *  store the expr into a preg to avoid building a dag
   *	src = expr;
   */
  srcNo = Make_Leaf(block, expr, src);

  /*
   *  dst = (signed cvt) src ;
   */
  {
    WN	*ldid, *cvt;

    ldid = Load_Leaf(srcNo);

    cvt  = WN_Cvt(MTYPE_complement(src), dst, ldid);

    dstNo = Make_Leaf(block, cvt, dst);
  }

  /*
   *  build the select tree that looks like
   *	(src < 0) ? dst + 2**N : dst
   *	 where N is the src size
   */
  {
    WN	*rel, *add, *select;

    rel = WN_LT(MTYPE_complement(src),
		Load_Leaf(srcNo),
		WN_Zerocon(MTYPE_complement(src)));

    add = WN_Add(dst,
		 Load_Leaf(dstNo),
		 WN_ConstPowerOf2( dst, MTYPE_size_reg(src)));

    select = WN_Select(dst, rel, add, Load_Leaf(dstNo));
    return select;
  }
}




/* ====================================================================
 *
 * WN *lower_float_to_unsigned(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on conversions <tree>,
 * returning lowered expression tree.
 *
 * ==================================================================== */

static WN *lower_float_to_unsigned(WN *block, WN *expr, TYPE_ID src,
				   TYPE_ID dst, LOWER_ACTIONS actions)
{
  LEAF srcNo;
  WN *trunc1,*r;
  WN *rel, *sub, *trunc2,*add;


  /* Two cases, dest type = U8, and dest type = U4 */
  if (dst == MTYPE_U4) {
     r = WN_Cvt(MTYPE_I8,dst,WN_Trunc(src,MTYPE_I8,expr));
  } else if (src==MTYPE_FQ || src==MTYPE_F16) {
     /* Need to do this this way because there is no quad floor */
     
     /*
      *  store the expr into a preg to avoid building a dag
      *	src = expr;
      */
     srcNo = Make_Leaf(block, expr, src);
     
     /*
      *  build the select tree that looks like
      *
      * (2**(N-1) <= src) ? : 2**(N-1) + TRUNC(src-2**(N-1)) : TRUNC(src)
      *
      *	where N is the unsigned dst size
      */


     trunc1 = WN_Trunc(src, MTYPE_complement(dst), Load_Leaf(srcNo));
     
     sub = WN_Sub(src, Load_Leaf(srcNo),
		  WN_ConstPowerOf2(src, MTYPE_size_reg(dst)-1));

     trunc2 =  WN_Trunc(src, MTYPE_complement(dst),sub);

     add = WN_Add(dst,trunc2,WN_ConstPowerOf2(dst, MTYPE_size_reg(dst)-1));
     
     rel = WN_LE(src,
		 WN_ConstPowerOf2(src, MTYPE_size_reg(dst)-1),
		 Load_Leaf(srcNo));
     
     r = WN_Cselect(dst,rel,add,trunc1);
  } else {
     
     /*
      *  store the expr into a preg to avoid building a dag
      *	src = expr;
      */
     srcNo = Make_Leaf(block, expr, src);
     
     /*
      *  build the select tree that looks like
      *
      * (2**(N-1) <= src) ? : FLOOR(src-2**(N)) : TRUNC(src)
      *
      *	where N is the unsigned dst size
      */


     trunc1 = WN_Trunc(src, MTYPE_complement(dst), Load_Leaf(srcNo));
     
     sub = WN_Sub(src, Load_Leaf(srcNo),
		  WN_ConstPowerOf2(src, MTYPE_size_reg(dst)));

     trunc2 =  WN_Floor(src, MTYPE_complement(dst),sub);

     rel = WN_LE(src,
		 WN_ConstPowerOf2(src, MTYPE_size_reg(dst)-1),
		 Load_Leaf(srcNo));
     
     r = WN_Cselect(dst,rel,trunc2,trunc1);
  }
  return (r);
}




/* ====================================================================
 *
 * WN *lower_cvt(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on conversions <tree>,
 * returning lowered expression tree.
 *
 * ==================================================================== */

static WN *lower_cvt(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  WN	*expr, *cvt;
  TYPE_ID dst = WN_rtype(tree);
  TYPE_ID src = WN_desc(tree);

  expr = lower_expr(block, WN_kid0(tree), actions);
  WN_kid0(tree) = expr;

  if (   Targ_Lower_Unsigned_To_Float 
      && MTYPE_is_unsigned(src) && MTYPE_is_float(dst))
  {
    WN_Delete(tree);

    cvt = lower_unsigned_to_float(block, expr, src, dst, actions);

    return lower_expr(block, cvt, actions);
  }
  else if (   Targ_Lower_Float_To_Unsigned 
	   && MTYPE_is_float(src) && MTYPE_is_unsigned(dst))
  {
    WN_Delete(tree);

    cvt = lower_float_to_unsigned(block, expr, src, dst, actions);

    return lower_expr(block, cvt, actions);
  }
  else if (   OPERATOR_is_compare(WN_operator(expr))
	   && WN_rtype(expr) == MTYPE_B
	   && (MTYPE_is_integral(dst) && src == MTYPE_B))
  {

    /* Optimize converts of MTYPE_B compares to integral values.
     */
    WN_Delete(tree);

    WN_set_rtype(expr, dst);

    return lower_expr(block, expr, actions);
  }
  
  return tree;
}



#ifdef TARG_ST

/* ====================================================================
 *   get_hilo_type
 * ====================================================================
 */
static TYPE_ID
get_hilo_type (
  TYPE_ID orig_type
  )
{
  TYPE_ID type;

#ifdef TARG_STxP70
  if (MTYPE_is_double(orig_type)) {
    if(Enable_Fpx) {
      type = MTYPE_I4;
    }
    else {
      type = MTYPE_F4;
    }
  }
  else if (orig_type == MTYPE_I8) type = MTYPE_I4;
  else if (orig_type == MTYPE_U8) type = MTYPE_U4;
  else {
    FmtAssert(FALSE,("can't handle MTYPE_%s", MTYPE_name(orig_type)));
  }
#else
  if (MTYPE_is_double(orig_type)) type = MTYPE_F4;
  else if (orig_type == MTYPE_I8) type = MTYPE_I4;
  else if (orig_type == MTYPE_U8) type = MTYPE_U4;
  else {
    FmtAssert(FALSE,("can't handle MTYPE_%s", MTYPE_name(orig_type)));
  }
#endif

  return type;
}

#ifdef NEW_LOWER
/* ===================================================================
 *   is_hilo_type
 * ===================================================================
 */
static BOOL
is_hilo_type (TYPE_ID type)
{
  return !Enable_64_Bits_Ops &&
    (MTYPE_is_double(type) || MTYPE_is_longlong(type));
}
#endif

/* ====================================================================
 *   lower_hilo_expr
 *
 *   This is supposed to factorise the code for lowering doubles and
 *   long longs. 
 *
 *   TODO: factorize with complex.
 * ==================================================================== 
 */
static void 
lower_hilo_expr (
  WN *block, 
  WN *tree, 
  LOWER_ACTIONS actions,
  WN **hipart, 
  WN **lopart
  /* TYPE_ID orig_type */
)
{
  INT i;
  OPERATOR opr = WN_operator(tree);
  TYPE_ID res = WN_rtype(tree);
  TYPE_ID desc = WN_desc(tree);

  WN *wn = NULL;
  INTRINSIC intrinsic;
  WN *kids[3];

  Is_True(OPCODE_is_expression(WN_opcode(tree)),
	  ("expected expression node, not %s", OPCODE_name(WN_opcode(tree))));

#if 1
  if (traceRuntime) {
    fprintf(TFile, " { >> \n");
    fprintf(TFile, "   >> lower_hilo: \n");
    fdump_tree(TFile, tree);
  }
#endif
  switch (opr) {

  case OPR_INTRINSIC_OP:
    {
      //
      // intrinsic op has not been lowered to a call => an inlinable
      // intrinsic. Lower kids. We need to make a new INTRINSIC_OP
      // with a different number of kids. There will also be kids
      // that correspond to the values returned - PREGs.
      // All of this makes a EVAL statement.
      //
      INT nkids = 0;
      PREG_NUM pregNo;
      TYPE_ID rtype = WN_rtype(tree);
      WN *kids[INTRN_MAX_ARG];
      WN *intrn_op;

      //
      // If consumer needs to see it returning a pair lo/hi
      // 
      if (
#ifdef NEW_LOWER
	  is_hilo_type(rtype)
#else
	  MTYPE_is_longlong(rtype) || MTYPE_is_double(rtype)
#endif
	  ) {
	ST *preg = MTYPE_To_PREG(rtype);
	TYPE_ID ltype = get_hilo_type(rtype);

	AssignPregName(INTR_intrinsic_name(tree), ".return");
	pregNo = Create_Preg(rtype, CurrentPregName());
	ResetPregName();

	WN *lo = WN_LdidPreg (ltype, pregNo);
	WN *hi = WN_LdidPreg (ltype, pregNo+1);

	// Add kids for returned values
	if (Pass_Low_First(rtype)) {
	  kids[nkids++] = WN_CreateParm (ltype,
				       lo,
				       Be_Type_Tbl(ltype),
				       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);
	  kids[nkids++] = WN_CreateParm (ltype,
				       hi,
				       Be_Type_Tbl(ltype),
				       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);
	}
	else {
	  kids[nkids++] = WN_CreateParm (ltype,
				       hi,
				       Be_Type_Tbl(ltype),
				       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);
	  kids[nkids++] = WN_CreateParm (ltype,
				       lo,
				       Be_Type_Tbl(ltype),
				       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);
	}
      }

      for (i = 0; i < WN_kid_count(tree); i++) { 
	TYPE_ID pmtype = WN_rtype(WN_kid(tree,i));
	// kids must be PARMs
	// If PARM type is 64 bits - lower it
	if (
#ifdef NEW_LOWER
	    is_hilo_type(pmtype)
#else
	    MTYPE_is_longlong(pmtype) || MTYPE_is_double(pmtype)
#endif
	    ) {
	  WN *lo;
	  WN *hi;
	  lower_hilo_expr (block, 
			   WN_kid0(WN_kid(tree,i)), 
			   actions, 
			   &hi, /* hipart */
			   &lo /* lopart */
			   /* pmtype */);

	  // make new parameters
	  if (Pass_Low_First(pmtype)) {
	    kids[nkids++] = WN_CreateParm (get_hilo_type(pmtype),
				       lo,
				       Be_Type_Tbl(get_hilo_type(pmtype)),
				       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);
	    kids[nkids++] = WN_CreateParm (get_hilo_type(pmtype),
				       hi,
				       Be_Type_Tbl(get_hilo_type(pmtype)),
				       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);
	  }
	  else {
	    kids[nkids++] = WN_CreateParm (get_hilo_type(pmtype),
				       hi,
				       Be_Type_Tbl(get_hilo_type(pmtype)),
				       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);
	    kids[nkids++] = WN_CreateParm (get_hilo_type(pmtype),
				       lo,
				       Be_Type_Tbl(get_hilo_type(pmtype)),
				       WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);
	  }

	  // Remove the old PARM node
	  WN_Delete(WN_kid(tree,i));
	}
	else {
	  // The existing PARM node will do, don't forget to lower
	  WN_kid(tree,i) = lower_expr(block, WN_kid(tree,i), actions);
	  kids[nkids++] = WN_kid(tree,i);
	}
      }

      // Make a new INTRINSIC_OP - same rtype, rtype just for the info
      intrn_op = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
						    WN_rtype(tree), 
						    MTYPE_V),
				     WN_intrinsic(tree), nkids, kids);

      // Delete old intrinsic OP
      WN_Delete(tree);

      if (
#ifdef NEW_LOWER
	  is_hilo_type(rtype)
#else
	  MTYPE_is_longlong(rtype) || MTYPE_is_double(rtype)
#endif
	  ) {
	// Make an EVAL stmt and attach to WHIRL tree
	WN *wn = WN_CreateEval(intrn_op);

	WN_Set_Linenum(wn, current_srcpos); // [CL] don't forget to record the
					    // linenum of this new statement
	
#if 1
        if (traceRuntime) {
	  fprintf(TFile, "EVAL: \n");
	  fdump_tree(TFile, wn);
	}
#endif

	WN_INSERT_BlockLast(block, wn);

	// return a hi/lo pair of PREGs
	*lopart = WN_LdidPreg (get_hilo_type(rtype), pregNo);
	*hipart = WN_LdidPreg (get_hilo_type(rtype), pregNo+1);

#if 0
	if (traceRuntime) {
	  fprintf(TFile, "lo: \n");
	  fdump_tree(TFile, *lopart);
	  fprintf(TFile, "hi: \n");
	  fdump_tree(TFile, *hipart);
	}
#endif

      }
      else {
	// hipart shouldn't matter
	*lopart = intrn_op;
	*hipart = NULL;
      }

    }
    break;

  case OPR_LDID:
    {
      // shadow the parameter
      TYPE_ID orig_type = WN_rtype(tree);

#ifdef NEW_LOWER
      /* Should not be called for non hilo_type. */
      Is_True(is_hilo_type(orig_type), ("lower_hilo_expr(): mtype not hilo: opr: %s, mtype: %s", OPERATOR_name(opr), MTYPE_name(orig_type)));
#else
      Is_True(MTYPE_is_longlong(orig_type) || 
	      MTYPE_is_double(orig_type), ("mtype not HILO"));
#endif
      // We should never have U8U4LOAD, always an explicit CVT
      FmtAssert(MTYPE_size_min(WN_rtype(tree)) == MTYPE_size_min(WN_desc(tree)), ("Unexpected LOAD types for hilo lowering"));

      TYPE_ID type = get_hilo_type(orig_type);
      /*
       *  we must create a new ST of type F4/I4/U4
       *  Depending on target endianness, the storage of a double
       *  is either { Hi, Lo } or { Lo, Hi }
       */
      TY_IDX    beTY   = MTYPE_To_TY(type);
      WN_OFFSET offset = WN_load_offset(tree);

      if (WN_class(tree) == CLASS_CONST && offset == 0)
      {
	TCON	val = WN_val(tree);
	TYPE_ID	valType = WN_val_type(tree);

	if (WN_rtype(tree) == valType)
	{
	  if (MTYPE_is_double(WN_rtype(tree))) {
	    *hipart = Make_Const( Extract_Double_Hi(val));
	    *lopart = Make_Const( Extract_Double_Lo(val));
	  }
	  else {
	    // must be longlong
	    *hipart = Make_Const( Extract_LongLong_Hi(val));
	    *lopart = Make_Const( Extract_LongLong_Lo(val));
	  }
 	  break;
	}
      }

      if (! WN_Pass_Low_First (tree)) {
	// hi then lo
	*hipart = WN_Ldid(type, offset,
			  coerceST(WN_st(tree), type), beTY);
	
	*lopart = WN_Ldid(type, coerceOFFSET(tree, type, offset),
			  coerceST(WN_st(tree), type), beTY);
      } else {
	// lo then hi
	*lopart = WN_Ldid(type, offset,
			  coerceST(WN_st(tree), type), beTY);
	
	*hipart = WN_Ldid(type, coerceOFFSET(tree, type, offset),
			  coerceST(WN_st(tree), type), beTY);
      }

      // need this ??
      lower_hilo_maps(tree, *hipart, *lopart, actions, orig_type);
    }

    break;

  case OPR_LDBITS:
    {
      TYPE_ID orig_type = WN_rtype(tree);
#ifdef NEW_LOWER
      /* Should not be called for non hilo_type. */
      Is_True(is_hilo_type(orig_type), ("lower_hilo_expr(): mtype not hilo: opr: %s, mtype: %s", OPERATOR_name(opr), MTYPE_name(orig_type)));
#else
      Is_True(MTYPE_is_longlong(orig_type) || 
	      MTYPE_is_double(orig_type), ("mtype not HILO"));
#endif
      
      // We should never have U8U4LOAD, always an explicit CVT
      Is_True(MTYPE_size_min(WN_rtype(tree)) == MTYPE_size_min(WN_desc(tree)), ("Unexpected LOAD types for hilo lowering"));
      
#ifdef TARG_ST
      // We must first lower load bits before performing HILO lowering.
      // This is an implementation choice; it is simpler to implement like this.
      // Note that RT lowering is now the responsibility of lower_load_bits.
      WN *extract_wn = lower_load_bits (block, tree, RemoveAction(actions, LOWER_HILO));
      lower_hilo_expr(block, extract_wn, actions, hipart, lopart);
#else
      // First lower bit extract and then lower hilo
      // Run time lowering may be necessary for IU8 shifts
      WN *extract_wn = lower_load_bits (block, tree, 0);
      extract_wn = RT_LOWER_expr(extract_wn);
      lower_hilo_expr(block, extract_wn, actions, hipart, lopart);
#endif
    }
    break;

  case OPR_ILOAD:
    {
      // shadow the parameter
      TYPE_ID orig_type = WN_rtype(tree);
#ifdef NEW_LOWER
      /* Should not be called for non hilo_type. */
      Is_True(is_hilo_type(orig_type), ("lower_hilo_expr(): mtype not hilo: opr: %s, mtype: %s", OPERATOR_name(opr), MTYPE_name(orig_type)));
#else
      Is_True(MTYPE_is_longlong(orig_type) || 
	      MTYPE_is_double(orig_type), ("mtype not HILO"));
#endif

      // We should never have U8U4LOAD, always an explicit CVT
      FmtAssert(MTYPE_size_min(WN_rtype(tree)) == MTYPE_size_min(WN_desc(tree)), ("Unexpected LOAD types for hilo lowering"));

      WN_OFFSET offset = WN_load_offset(tree);
      WN	*addr;
      LEAF	addrN;

      // Address expr. can not be U8/A8 if lower_hilo_expr is
      // required (Only_32_Bit_Ops on).
      addr = lower_expr(block, WN_kid0(tree), actions);

      addrN = Make_Leaf(block, addr, Pointer_type);

      //
      // Should be OK to lower both, the hi and the lo
      //
      TYPE_ID type = get_hilo_type(orig_type);

      /*
       *  Depending on target endianness, the storage of a double
       *  is either { Hi, Lo } or { Lo, Hi }
       */
      if (Target_Byte_Sex == BIG_ENDIAN) {
	// big endian : hi the lo
	*hipart = WN_Iload(type, 
			   offset,
			   coerceTY(WN_ty(tree), type),
			   Load_Leaf(addrN));
	
	*lopart = WN_Iload(type,
			   coerceOFFSET(tree, type, offset),
			   coerceTY(WN_ty(tree), type),
			   Load_Leaf(addrN)); 
      } else {
	// little endian : lo then hi
	*lopart = WN_Iload(type,
			   offset,
			   coerceTY(WN_ty(tree), type),
			   Load_Leaf(addrN));
	
	*hipart = WN_Iload(type,
			   coerceOFFSET(tree, type, offset),
			   coerceTY(WN_ty(tree), type),
			   Load_Leaf(addrN));
      }

#ifdef TARG_ST
      // (cbr) bug #19331 propagate volatile
      if (WN_Is_Volatile_Mem(tree)) {
        TY_IDX pointedlo = TY_pointed (WN_load_addr_ty(*lopart));
        TY_IDX pointedhi = TY_pointed (WN_load_addr_ty(*hipart));

        Set_TY_is_volatile(pointedlo);
        Set_TY_is_volatile(pointedhi);

        Set_TY_pointed (WN_load_addr_ty(*lopart), pointedlo); 
        Set_TY_pointed (WN_load_addr_ty(*hipart), pointedhi); 
      }
#endif

      lower_hilo_maps(tree, *hipart, *lopart, actions, orig_type);
    }
    break;

  case OPR_ILDBITS:
    {
      // shadow the parameter
      TYPE_ID orig_type = WN_rtype(tree);
#ifdef NEW_LOWER
      /* Should not be called for non hilo_type. */
      Is_True(is_hilo_type(orig_type), ("lower_hilo_expr(): mtype not hilo: opr: %s, mtype: %s", OPERATOR_name(opr), MTYPE_name(orig_type)));
#else
      Is_True(MTYPE_is_longlong(orig_type) || 
	      MTYPE_is_double(orig_type), ("mtype not HILO"));
#endif
      
      // We should never have U8U4LOAD, always an explicit CVT
      FmtAssert(MTYPE_size_min(WN_rtype(tree)) == MTYPE_size_min(WN_desc(tree)), ("Unexpected LOAD types for hilo lowering"));
      
#ifdef TARG_ST
      // We must first lower load bits before performing HILO lowering.
      // This is an implementation choice; it is simpler to implement like this.
      // Note that RT lowering is now performed the responsibility of lower_load_bits.
      WN *extract_wn = lower_load_bits (block, tree, RemoveAction(actions, LOWER_HILO));
      lower_hilo_expr(block, extract_wn, actions, hipart, lopart);
#else
      // First lower bit extract and than lower hilo
      // Run time lowering may be necessary for I8/U8 shifts
      WN *extract_wn = lower_load_bits (block, tree, 0);
      extract_wn = RT_LOWER_expr(extract_wn);
      lower_hilo_expr(block, extract_wn, actions, hipart, lopart);
#endif
  }
  break;
  
  case OPR_CONST:
    {
      // shadow the parameter
      TYPE_ID orig_type = WN_rtype(tree);
#ifdef NEW_LOWER
      /* Should not be called for non hilo_type. */
      Is_True(is_hilo_type(orig_type), ("lower_hilo_expr(): mtype not hilo: opr: %s, mtype: %s", OPERATOR_name(opr), MTYPE_name(orig_type)));
#else
      Is_True(MTYPE_is_longlong(orig_type) || 
	      MTYPE_is_double(orig_type), ("mtype not HILO"));
#endif

      TYPE_ID type = get_hilo_type(orig_type);
      TCON	val = Const_Val(tree);

      if (MTYPE_is_double(orig_type)){
	*hipart = Make_Const(Extract_Double_Hi(val));
	*lopart = Make_Const(Extract_Double_Lo(val));
      }
      else {
	// must be long long
	*hipart = Make_Const(Extract_LongLong_Hi(val));
	*lopart = Make_Const(Extract_LongLong_Lo(val));
      }

      //      lower_hilo_maps (tree, *hipart, *lopart, actions, orig_type);
      if (block)
        WN_Delete(tree);
    }
    break;

  case OPR_INTCONST:
    {
      // shadow the parameter
      TYPE_ID orig_type = WN_rtype(tree);
#ifdef NEW_LOWER
      /* Should not be called for non hilo_type. */
      Is_True(is_hilo_type(orig_type), ("lower_hilo_expr(): mtype not hilo: opr: %s, mtype: %s", OPERATOR_name(opr), MTYPE_name(orig_type)));
#else
      Is_True(MTYPE_is_longlong(orig_type) || 
	      MTYPE_is_double(orig_type), ("mtype not HILO"));
#endif

      TYPE_ID type = get_hilo_type(orig_type);

      /*
       * Must be a longlong
       * First, force it to long long in case it is not
       */
      TCON  val = Host_To_Targ (WN_rtype(tree), WN_const_val(tree));
      TCON  new_val = Targ_Conv (orig_type, val);

      INT64 hi = Targ_To_Host(Extract_LongLong_Hi(new_val));
      INT64 lo = Targ_To_Host(Extract_LongLong_Lo(new_val));

      *hipart = WN_Intconst(type, hi);
      *lopart = WN_Intconst(type, lo);

      if (block)
        WN_Delete(tree);
    }
    break;

  case OPR_PAREN:
    {
      // shadow the parameter
      TYPE_ID orig_type = WN_rtype(tree);
#ifdef NEW_LOWER
      /* Should not be called for non hilo_type. */
      Is_True(is_hilo_type(orig_type), ("lower_hilo_expr(): mtype not hilo: opr: %s, mtype: %s", OPERATOR_name(opr), MTYPE_name(orig_type)));
#else
      Is_True(MTYPE_is_longlong(orig_type) || 
	      MTYPE_is_double(orig_type), ("mtype not HILO"));
#endif

      TYPE_ID type = get_hilo_type(orig_type);

      /*
       *	preserve the parens
       */
      lower_hilo_expr(block, 
		      WN_kid0(tree), 
		      actions, 
		      hipart, 
		      lopart 
		      /* orig_type */);

      *hipart = WN_Paren(type, *hipart);
      *lopart = WN_Paren(type, *lopart);
      if (block)
        WN_Delete(tree);
    }
    break;

  case OPR_PARM:
    FmtAssert(FALSE,("lower_hilo_expr(): unexpected operator: %s", OPCODE_name(WN_opcode(tree))));
    break;

  case OPR_ILOADX:
    FmtAssert(FALSE,("lower_hilo_expr(): unexpected operator: %s", OPCODE_name(WN_opcode(tree))));
    break;

  case OPR_RND: 
  case OPR_TRUNC: 
  case OPR_CEIL: 
  case OPR_FLOOR:
#ifdef NEW_LOWER
    {
      TYPE_ID orig_type = WN_rtype(tree);
      /* Should not be called for non hilo_type. */
      Is_True(is_hilo_type(orig_type), ("lower_hilo_expr(): mtype not hilo: opr: %s, mtype: %s", OPERATOR_name(opr), MTYPE_name(orig_type)));
    }
#endif
    FmtAssert(FALSE,("lower_hilo_expr(): unexpected operator, should have been run-time lowered: %s", OPCODE_name(WN_opcode(tree))));
    break;

  case OPR_ARRAY:
    break;

  // unary ops
  case OPR_ABS:
  case OPR_SQRT: 
  case OPR_RSQRT: 
  case OPR_RECIP:

  case OPR_ALLOCA:

  case OPR_MAX: 
  case OPR_MIN: 
  case OPR_MINMAX:

  case OPR_MINPART: 
  case OPR_MAXPART:
#ifdef NEW_LOWER
    {
      TYPE_ID orig_type = WN_rtype(tree);
      /* Should not be called for non hilo_type. */
      Is_True(is_hilo_type(orig_type), ("lower_hilo_expr(): mtype not hilo: opr: %s, mtype: %s", OPERATOR_name(opr), MTYPE_name(orig_type)));
    }
#endif
    FmtAssert(FALSE,("lower_hilo_expr(): unexpected operator, should have been run-time lowered: %s", OPCODE_name(WN_opcode(tree))));
    break;

  case OPR_REALPART: 
  case OPR_IMAGPART:
  case OPR_HIGHPART: 
  case OPR_LOWPART:

  case OPR_COMPLEX:
#ifdef NEW_LOWER
    {
      TYPE_ID orig_type = WN_rtype(tree);
      /* Should not be called for non hilo_type. */
      Is_True(is_hilo_type(orig_type), ("lower_hilo_expr(): mtype not hilo: opr: %s, mtype: %s", OPERATOR_name(opr), MTYPE_name(orig_type)));
    }
#endif
    FmtAssert(FALSE,("lower_hilo_expr(): unexpected operator: %s", OPCODE_name(WN_opcode(tree))));
    break;

  case OPR_CVT:
    //
    // Some CVTs have been left to be dealt with here
    //
    if (
#ifdef NEW_LOWER
	MTYPE_is_longlong(WN_desc(tree)) &&
	is_hilo_type(WN_desc(tree)) &&
	(WN_rtype(tree) == MTYPE_I4 ||
	 WN_rtype(tree) == MTYPE_U4)
#else
	WN_opcode(tree) == OPC_I4U8CVT ||
	WN_opcode(tree) == OPC_U4U8CVT ||
	WN_opcode(tree) == OPC_I4I8CVT ||
	WN_opcode(tree) == OPC_U4I8CVT
#endif
	) {
      WN *hi, *lo;
      lower_hilo_expr(block, WN_kid0(tree), actions, &hi, &lo);
      //
      // only use the lo part of it
      //
      *lopart = lo;
      *hipart = NULL;

      if (block) WN_Delete(tree);
      break;
    }

    if (
#ifdef NEW_LOWER
	MTYPE_is_longlong(WN_rtype(tree)) &&
	is_hilo_type(WN_rtype(tree)) &&
	WN_desc(tree) == MTYPE_U4
#else
	WN_opcode(tree) == OPC_U8U4CVT ||
	WN_opcode(tree) == OPC_I8U4CVT
#endif
	) {

      TYPE_ID type = WN_rtype(WN_kid0(tree));
      //
      // I need to lower the kid first
      //
      WN *kid0 = lower_expr(block, WN_kid0(tree), actions);

      //
      // lo = kid0, hi = 0
      //

      //
      // note that U8I4 is undefined if I4 is negative
      //
      *hipart = WN_Intconst(type,0);
      //*lopart = WN_kid0(tree);
      *lopart = kid0;

      if (block) WN_Delete(tree);
      break;
    }

    if (
#ifdef NEW_LOWER
	MTYPE_is_longlong(WN_rtype(tree)) &&
	is_hilo_type(WN_rtype(tree)) &&
	WN_desc(tree) == MTYPE_I4
#else
	WN_opcode(tree) == OPC_I8I4CVT ||
	WN_opcode(tree) == OPC_U8I4CVT
#endif
	) {

      TYPE_ID type = WN_rtype(WN_kid0(tree));
      //
      // I need to lower the kid first
      //
      WN *kid0 = lower_expr(block, WN_kid0(tree), actions);

      //
      //  lo = kid0; hi = ASHR(kid0),31
      //
      // However, we need to store into a preg because we can not
      // reuse the kid0 expression in two calculations
      //

      // prepare stuff for the hi part
      PREG_NUM preg = AssignExpr (block, 
				  //WN_kid0(tree), 
				  kid0,
				  type);

      // load this preg twice
      WN *lo = WN_LdidPreg(type, preg);
      *lopart = lo;

      WN *hi = WN_LdidPreg(type, preg);
      // [TTh] Fix for codex bug #39445:
      // Use shadowed type, as tree pointer and its kid0 might be invalid
      *hipart = WN_Ashr(type, //WN_rtype(WN_kid0(tree))), 
			hi, 
			WN_Intconst(type,31));

      if (block) WN_Delete(tree);
      break;
    }

    // FALLTHROUGH

  case OPR_NEG:

  case OPR_ADD: 
  case OPR_SUB: 
  case OPR_MPY:
  case OPR_DIV: 
  case OPR_DIVREM:

  case OPR_EQ: 
  case OPR_NE: 
  case OPR_GE: 
  case OPR_GT: 
  case OPR_LE: 
  case OPR_LT:

  case OPR_SHL: 
  case OPR_ASHR: 
  case OPR_LSHR:

    FmtAssert (FALSE,("lower_hilo_expr(): unexpected operator, should have been run time lowered: %s", OPCODE_name(WN_opcode(tree))));
    break;

  case OPR_MOD:
  case OPR_REM:
    FmtAssert (FALSE,("lower_hilo_expr(): unexpected operator, should have been run time lowered: %s", OPCODE_name(WN_opcode(tree))));
    break;

  case OPR_EXTRACT_BITS:
  case OPR_COMPOSE_BITS:
    FmtAssert (FALSE,("lower_hilo_expr(): unexpected operator: %s", OPCODE_name(WN_opcode(tree))));
    break;

  case OPR_BNOT: 
    {
      WN *hi, *lo;
      lower_hilo_expr(block, WN_kid0(tree), actions, &hi, &lo);
      *hipart = WN_Bnot(get_hilo_type(WN_rtype(tree)), hi);
      *lopart = WN_Bnot(get_hilo_type(WN_rtype(tree)), lo);
      if (block != NULL) WN_Delete(tree);
    }
    break;
  case OPR_BAND: 
    {
      WN *hi1, *lo1, *hi2, *lo2;
      lower_hilo_expr(block, WN_kid0(tree), actions, &hi1, &lo1);
      lower_hilo_expr(block, WN_kid1(tree), actions, &hi2, &lo2);
      *hipart = WN_Band(get_hilo_type(WN_rtype(tree)), hi1, hi2);
      *lopart = WN_Band(get_hilo_type(WN_rtype(tree)), lo1, lo2);
      if (block != NULL) WN_Delete(tree);
    }
    break;

  case OPR_BIOR: 
    {
      WN *hi1, *lo1, *hi2, *lo2;

      lower_hilo_expr(block, WN_kid0(tree), actions, &hi1, &lo1);
      lower_hilo_expr(block, WN_kid1(tree), actions, &hi2, &lo2);
      *hipart = WN_Bior(get_hilo_type(WN_rtype(tree)), hi1, hi2);
      *lopart = WN_Bior(get_hilo_type(WN_rtype(tree)), lo1, lo2);
      if (block != NULL) WN_Delete(tree);
    }
    break;

  case OPR_BNOR: 
    {
      WN *hi1, *lo1, *hi2, *lo2;
      lower_hilo_expr(block, WN_kid0(tree), actions, &hi1, &lo1);
      lower_hilo_expr(block, WN_kid1(tree), actions, &hi2, &lo2);
      //
      // Because the Bnor macro does not exist for some reason ??
      //
      *hipart = WN_Binary (OPR_BNOR, get_hilo_type(WN_rtype(tree)), hi1, hi2);
      *lopart = WN_Binary (OPR_BNOR, get_hilo_type(WN_rtype(tree)), lo1, lo2);
      if (block != NULL) WN_Delete(tree);
    }
    break;

  case OPR_BXOR:
    {
      WN *hi1, *lo1, *hi2, *lo2;

      lower_hilo_expr(block, WN_kid0(tree), actions, &hi1, &lo1);
      lower_hilo_expr(block, WN_kid1(tree), actions, &hi2, &lo2);
      *hipart = WN_Bxor(get_hilo_type(WN_rtype(tree)), hi1, hi2);
      *lopart = WN_Bxor(get_hilo_type(WN_rtype(tree)), lo1, lo2);
      if (block != NULL) WN_Delete(tree);
    }
    break;

  case OPR_TAS:
    {
      TYPE_ID hilo_type = WN_rtype(tree);
      TYPE_ID type = get_hilo_type(hilo_type);
      WN *hi, *lo;
      //
      // TAS may be casting a type that does not need HILO lowering
      //
      if (
#ifdef NEW_LOWER
	  is_hilo_type(WN_rtype(WN_kid0(tree)))
#else
	  MTYPE_is_longlong(WN_rtype(WN_kid0(tree))) || MTYPE_is_double(WN_rtype(WN_kid0(tree)))
#endif
	  ) 
      {
	lower_hilo_expr(block, WN_kid0(tree), actions, &hi, &lo);

	*hipart = WN_Tas(type, Be_Type_Tbl(type), hi);
	*lopart = WN_Tas(type, Be_Type_Tbl(type), lo);
	if (block) WN_Delete(tree);
      }
      else {
	FmtAssert(FALSE,("can't handle cast"));
      }
    }
    break;

  case OPR_CVTL:
    //
    // Lower the kid0.
    // If CVTL is to size < 32, do CVTL on the lo part, otherwise
    // just return the lo part.
    //
    {
      // shadow the parameter
      TYPE_ID orig_type = WN_rtype(tree);
#ifdef NEW_LOWER
      Is_True(is_hilo_type(orig_type), ("lower_hilo_expr(): mtype not hilo: opr: %s, mtype: %s", OPERATOR_name(opr), MTYPE_name(orig_type)));
#else
      Is_True(MTYPE_is_longlong(orig_type) || 
	      MTYPE_is_double(orig_type), ("mtype not HILO"));
#endif

      WN *hi, *lo;
      lower_hilo_expr(block, WN_kid0(tree), actions, &hi, &lo /*, orig_type */);
      if (WN_cvtl_bits(tree) < 32) {
	OPCODE opc = OPCODE_make_op (OPR_CVTL, orig_type, MTYPE_V);
	*lopart = WN_CreateCvtl (opc, WN_cvtl_bits(tree), lo);
      }
      else {
	*lopart = lo;
      }
      if (block)
        WN_Delete(tree);
    }
    break;

  case OPR_SELECT:
    {
      // shadow the parameter
      TYPE_ID orig_type = WN_rtype(tree);
#ifdef NEW_LOWER
      Is_True(is_hilo_type(orig_type), ("lower_hilo_expr(): mtype not hilo: opr: %s, mtype: %s", OPERATOR_name(opr), MTYPE_name(orig_type)));
#else
      Is_True(MTYPE_is_longlong(orig_type) || 
	      MTYPE_is_double(orig_type), ("mtype not HILO"));
#endif

      TYPE_ID type = get_hilo_type(orig_type);
      WN *hi1, *lo1;
      WN *hi2, *lo2;
      LEAF cond;
       
      cond = Make_Leaf(block, WN_kid0(tree), WN_rtype(WN_kid0(tree)));

      lower_hilo_expr(block, WN_kid1(tree), actions, &hi1, &lo1);
      lower_hilo_expr(block, WN_kid2(tree), actions, &hi2, &lo2);
       
      *hipart = WN_Select(type, Load_Leaf(cond), hi1, hi2);
      *lopart = WN_Select(type, Load_Leaf(cond), lo1, lo2);
    }
    break;

  case OPR_LNOT:
  case OPR_LAND: 
  case OPR_LIOR:
  case OPR_CAND: 
  case OPR_CIOR:
    FmtAssert(FALSE,("lower_hilo_expr(): unexpected operator: %s", OPCODE_name(WN_opcode(tree))));
    break;

  case OPR_MADD:
  case OPR_MSUB:
  case OPR_NMADD:
  case OPR_NMSUB:
    FmtAssert(FALSE,("lower_hilo_expr(): unexpected madd operator: %s", OPCODE_name(WN_opcode(tree))));
    break;

  default:
    Is_True(FALSE,("lower_hilo_expr(): unexpected operator: %s", OPCODE_name(WN_opcode(tree))));
  }
#if 1
  if (traceRuntime) {
    fprintf(TFile, " >> into (%s):\n", OPERATOR_name(opr));
    if (*lopart != NULL) {
      fprintf(TFile, "   lo: ");
      fdump_tree(TFile, *lopart);
    }
    if (*hipart != NULL) {
      fprintf(TFile, "   hi: ");
      fdump_tree(TFile, *hipart);
    }
    fprintf(TFile, " >> }\n");
    fflush(TFile);
  }
#endif
  return;
}

/* ====================================================================
 *   HILO_lower_wn 
 *
 *   A wrapper to call lower_hilo_expr on a WN* node
 * ==================================================================== 
 */
void
HILO_lower_wn (
  WN *wn,
  WN **hipart,
  WN **lopart
)
{
  // LOWER_HILO implies LOWER_RUNTIME
  // Null block also means not to delete the node.
  // that is converted (see Get_hilo_home), but not replaced.
  lower_hilo_expr(NULL, 
		  wn,
		  LOWER_HILO | LOWER_RUNTIME,
                  hipart, 
                  lopart);
  return;
}

#endif

/* ====================================================================
 *
 * WN *lower_nary_madd(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 *  Create nary expression reassociate to produce MADDS
 *
 * ==================================================================== */

static WN *lower_nary_madd(WN *block, WN *tree, LOWER_ACTIONS actions)
{

   TYPE_ID  type = WN_rtype(tree);

#ifdef TARG_ST
    if (Enable_NaryExpr && ( (Reassoc_Level >= REASSOC_ASSOC && MTYPE_is_float(type)) || (Roundoff_Level >= ROUNDOFF_ASSOC && MTYPE_is_integral(type))))
#else
  if (Enable_NaryExpr && Roundoff_Level >= ROUNDOFF_ASSOC)
#endif
  {

    tree = WN_ExprToNaryType(tree);

    if (WN_nary_add(tree))
    {
      INT32	i;
      BOOL	found= TRUE;

     /*
      *  Find mpy then a non mpy under nary tree.
      *  The tree may change (ex. to a binary tree)
      */
      while(found)
      {
	INT32	mpyI, exprI, nmpyI, narympyI;
	WN	*mpy, *nmpy, *expr, *narympy;
        INT32	num_kids = WN_kid_count(tree);

	found=	FALSE;
	mpy  =  nmpy  = expr = narympy  = NULL;
	mpyI =  exprI = nmpyI = narympyI = 0;

	for(i = 0; i < num_kids; i++)
	{
	  WN  *actual= WN_actual(tree, i);

	  if (WN_operator_is(actual, OPR_MPY))
	  {
	    mpyI = i;
	    mpy = actual;
	  }
	  else if (WN_operator_is(actual, OPR_NEG)	&&
		   WN_operator_is(WN_kid0(actual), OPR_MPY))
	  {
	    nmpyI = i;
	    nmpy = WN_kid0(actual);
	  }
	  else if (WN_nary_mpy(actual) && WN_kid_count(actual) >= 2)
	  {
	    narympyI = i;
	    narympy = actual;
	  }
	  else 
	  {
	    exprI = i;
	    expr = actual;
	  }

	  if (mpy && expr)
	  {
	    WN_actual(tree, mpyI)= WN_Madd(type, expr, WN_kid0(mpy),
					   WN_kid1(mpy));
	    tree = WN_NaryDelete(tree, exprI);
	    found=	TRUE;
	    break;
	  }
	  else if (nmpy && expr)
	  {
	    WN_actual(tree, nmpyI)= WN_Nmsub(type, expr, WN_kid0(nmpy),
					     WN_kid1(nmpy));
	    tree = WN_NaryDelete(tree, exprI);
	    found=	TRUE;
	    break;
	  }
	  else if (narympy && expr)
	  {
	    mpy=	WN_kid0(narympy);
	    narympy=	WN_NaryDelete(narympy, 0);

	    WN_actual(tree, narympyI)= WN_Madd(type, expr, mpy, narympy);
	    tree = WN_NaryDelete(tree, exprI);
	    found=	TRUE;
	    break;
	  }
	}
      }
     /*
      *  There may be madd opportunites at the children
      */
      for(i = 0; i < WN_kid_count(tree); i++)
      {
	WN_actual(tree, i)= lower_madd(block, WN_actual(tree,i), actions);
      }

    }
    
    if (WN_nary_intrinsic(tree))
    {
      tree = WN_NaryToExpr(tree);
    }
  }

  return tree;
}




/* ====================================================================
 *
 * WN *lower_madd(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on expression <tree>,
 * returning lowered expression tree.
 *
 * ==================================================================== */

static WN *lower_madd(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TYPE_ID  type = WN_rtype(tree);
#ifdef TARG_ST
  WN* return_wn = tree;
#endif
#ifdef TARG_ST
  // WN that carries the feedback
  WN *carry_fb = NULL;
#endif 

 /*
  *  look for madd patterns
  */
  switch (WN_operator(tree))
  {
  case OPR_NEG:
    {
      WN	*child = WN_kid0(tree);
#ifdef TARG_ST
      //TB: force the lowering of child to get more opportunity to catch WN such as:
      //  F4MPY
      // F4ADD
      //F4NEG
      child = lower_madd(block, child, actions);
#endif
      switch(WN_operator(child))
      {
      case OPR_MADD:
	WN_Delete(tree);
#ifdef TARG_ST
	carry_fb = child;
	return_wn = WN_Nmadd(type, WN_kid0(child), WN_kid1(child), WN_kid2(child));
#else
	return WN_Nmadd(type, WN_kid0(child), WN_kid1(child), WN_kid2(child));
#endif

      case OPR_MSUB:
	WN_Delete(tree);
#ifdef TARG_ST
	carry_fb = child;
	return_wn = WN_Nmsub(type, WN_kid0(child), WN_kid1(child), WN_kid2(child));
#else
	return WN_Nmsub(type, WN_kid0(child), WN_kid1(child), WN_kid2(child));
#endif

      case OPR_NMADD:
	WN_Delete(tree);
#ifdef TARG_ST
	carry_fb = child;
	return_wn = WN_Madd(type, WN_kid0(child), WN_kid1(child), WN_kid2(child));
#else
	return WN_Madd(type, WN_kid0(child), WN_kid1(child), WN_kid2(child));
#endif

      case OPR_NMSUB:
	WN_Delete(tree);
#ifdef TARG_ST
	carry_fb = child;
	return_wn = WN_Msub(type, WN_kid0(child), WN_kid1(child), WN_kid2(child));
#else
	return WN_Msub(type, WN_kid0(child), WN_kid1(child), WN_kid2(child));
#endif
      }
    }
    break;

  case OPR_ADD:
    {
      WN	*l= WN_kid0(tree);
      WN	*r= WN_kid1(tree);

      if (WN_operator_is(l, OPR_MPY))
      {
	WN_Delete(tree);
#ifdef TARG_ST
	return_wn = WN_Madd(type, r, WN_kid0(l), WN_kid1(l));
	carry_fb = l;
#else
	return WN_Madd(type, r, WN_kid0(l), WN_kid1(l));
#endif
      }
      else if (WN_operator_is(r, OPR_MPY))
      {
	WN_Delete(tree);
#ifdef TARG_ST
	return_wn = WN_Madd(type, l, WN_kid0(r), WN_kid1(r));
	carry_fb = r;
#else
	return WN_Madd(type, l, WN_kid0(r), WN_kid1(r));
#endif
      }
    }
    break;

  case OPR_SUB:
    {
      WN	*l= WN_kid0(tree);
      WN	*r= WN_kid1(tree);

      if (WN_operator_is(l, OPR_MPY))
      {
	WN_Delete(tree);
#ifdef TARG_ST
	return_wn = WN_Msub(type, r, WN_kid0(l), WN_kid1(l));
	carry_fb = l;
#else
	return WN_Msub(type, r, WN_kid0(l), WN_kid1(l));
#endif
      }
      else if (WN_operator_is(r, OPR_MPY))
      {
	WN_Delete(tree);
#ifdef TARG_ST
	return_wn = WN_Nmsub(type, l, WN_kid0(r), WN_kid1(r));
	carry_fb = r;
#else
	return WN_Nmsub(type, l, WN_kid0(r), WN_kid1(r));
#endif
      }
    }
    break;

  case OPR_MADD:
    {
      WN	*neg= WN_kid0(tree);

      if (WN_operator_is(neg, OPR_NEG))
      {
	WN	*child= WN_kid0(neg);

	WN_Delete(neg);
#ifdef TARG_ST
	return_wn = WN_Msub(type, child, WN_kid1(tree), WN_kid2(tree));
	carry_fb = tree;
#else
	return WN_Msub(type, child, WN_kid1(tree), WN_kid2(tree));
#endif
      }
    }
    break;

  case OPR_MSUB:
    {
      WN	*neg= WN_kid0(tree);

      if (WN_operator_is(neg, OPR_NEG))
      {
	WN	*child= WN_kid0(neg);

	WN_Delete(neg);
#ifdef TARG_ST
	return_wn = WN_Madd(type, child, WN_kid1(tree), WN_kid2(tree));
	carry_fb = tree;
#else
	return WN_Madd(type, child, WN_kid1(tree), WN_kid2(tree));
#endif
      }
    }
    break;

  case OPR_NMADD:
    {
      WN	*neg= WN_kid0(tree);

      if (WN_operator_is(neg, OPR_NEG))
      {
	WN	*child= WN_kid0(neg);

	WN_Delete(neg);
#ifdef TARG_ST
	return_wn = WN_Nmsub(type, child, WN_kid1(tree), WN_kid2(tree));
	carry_fb = tree;
#else
	return WN_Nmsub(type, child, WN_kid1(tree), WN_kid2(tree));
#endif
      }
    }
    break;

  case OPR_NMSUB:
    {
      WN	*neg= WN_kid0(tree);

      if (WN_operator_is(neg, OPR_NEG))
      {
	WN	*child= WN_kid0(neg);

	WN_Delete(neg);
#ifdef TARG_ST
	return_wn = WN_Nmadd(type, child, WN_kid1(tree), WN_kid2(tree));
	carry_fb = tree;
#else
	return WN_Nmadd(type, child, WN_kid1(tree), WN_kid2(tree));
#endif
      }
    }
    break;

     default:
#ifdef TARG_ST
    // TB: Add a recusive call to get more MADD transformation
    for (UINT kidno=0; kidno<WN_kid_count(tree); kidno++) {
      WN_kid(tree, kidno) = lower_madd(block, WN_kid(tree, kidno), actions);
    }
#endif
    break;
  }

#ifdef TARG_ST
  if (Cur_PU_Feedback && carry_fb) {
    const FB_Info_Value_FP_Bin &info = Cur_PU_Feedback->Query_value_fp_bin( carry_fb );

    Cur_PU_Feedback->Annot_value_fp_bin( return_wn, info );
  }
  return return_wn;
#else
  return tree;
#endif
}

#ifdef TARG_ST

// [HK] 2005/10 Division by constant algorithm implementation
//      based on the implementation of Henry S. Warren, Jr. in "Hacker's Delight"
//      of the Granlund & Montgomery algorithm "Division by Invariant Integers
//      Using Multiplication".

struct ms {
    INT32 M;
    INT32 s;
};
struct mu {
    UINT32 M;
    INT32 s;
    INT32 a;
};

// [HK] Performs "magic number" computation (signed and unsigned versions)
//      which gives an approximation of the divisor's inverse.
//      Returns both magic number 32-bit constant and shift amount used after
//      multiplication.
//      In the unsigned case, a boolean "a" is also returned, indicating whether
//      or not a correction must be applied after performing the magic number multiplication.

static struct ms magic(INT32 d){
    struct ms mag;
    INT32 p;
    UINT32 ad, anc, delta, q1, r1, q2, r2, t;
    const UINT32 two31 = 0x80000000;

    ad = abs(d);
    t = two31 + ( (UINT32) d >> 31);
    anc = t - 1 - t%ad;
    p = 31;
    q1 = two31/anc;
    r1 = two31 - q1*anc;
    q2 = two31/ad;
    r2 = two31 - q2*ad;
    do {
	p = p + 1;
	q1 = 2*q1;
	r1 = 2*r1;
	if (r1 >= anc) {
	    q1 = q1 + 1;
	    r1 = r1 - anc;
	}
	q2 = 2*q2;
	r2 = 2*r2;
	if (r2 >= ad){
	    q2 = q2 + 1;
	    r2 = r2 - ad;
	}
	delta = ad - r2;
    } while (q1 < delta || (q1 == delta && r1 == 0));

    mag.M = q2 + 1;
    if (d < 0) 
	mag.M = - mag.M;
    mag.s = p - 32;

    return mag;
}

static struct mu magicu(UINT32 d){
    struct mu magu;
    INT32 p;
    UINT32 p32, q, r, delta;
    
    magu.a = 0;
    p = 31;
    q = 0x7fffffff/d;
    r = 0x7fffffff - q*d;
    do {
	p = p + 1;
	if (p == 32) p32 = 1;
	else p32 = 2*p32;
	if ( r + 1 >= d - r ) {
	    if ( q >= 0x7fffffff ) 
		magu.a = 1;
	    q = 2*q + 1;
	    r = 2*r + 1 - d;
	}
	else {
	    if ( q >= 0x80000000 ) 
		magu.a = 1;
	    q = 2*q;
	    r = 2*r + 1;
	}
	delta = d - 1 - r;
    } while ( p < 64 && p32 < delta );

    magu.M = q + 1;
    magu.s = p - 32;
    return magu;
}

// [HK] Performs lowering of division by constant, generating the transformed
//      tree.

static WN *lower_cst_idiv(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TYPE_ID  type = WN_rtype(tree);
  LEAF tmp1, tmp2, tmp3;
  struct ms mag;
  INT32 M, s, d;
  WN *wn_M, *wn_s, *wn_tmp1, *wn_tmp2, *wn_tmp3, *wn_tmp4;

  if (WN_operator(WN_kid1(tree)) == OPR_LDID ) 
      d = (INT32)WN_const_val(Preg_Home(WN_offset(WN_kid1(tree))));
  else
      d = (INT32)WN_const_val(WN_kid1(tree));
  mag = magic(d);
  M = mag.M;
  s = mag.s;
  
  wn_M = WN_CreateIntconst(OPR_INTCONST, type, MTYPE_V, (INT64)M);
  tmp1 = Make_Leaf(block, WN_kid0(tree), type);
  tmp2 = Make_Leaf(block, wn_M, type);
  wn_tmp1 = WN_Cvt(type, MTYPE_I8, Load_Leaf(tmp1));
  wn_tmp2 = WN_Cvt(type, MTYPE_I8, Load_Leaf(tmp2));
  wn_tmp3 = WN_Mpy(MTYPE_I8, wn_tmp1, wn_tmp2);
  wn_tmp3 = WN_Ashr(MTYPE_I8, wn_tmp3, WN_Intconst(MTYPE_I4, 32));
  wn_tmp3 = WN_Cvt(MTYPE_I8, type, wn_tmp3);

  if ( (d > 0) && (M < 0) )
      wn_tmp3 = WN_Add(type, wn_tmp3, Load_Leaf(tmp1));
  else if ( (d < 0) && (M > 0) )
      wn_tmp3 = WN_Sub(type, wn_tmp3, Load_Leaf(tmp1));   
  
  wn_tmp3 = lower_expr(block, wn_tmp3, actions);
  tmp3 = Make_Leaf(block, wn_tmp3, type);

  if ( s > 0 ) {
      wn_s = WN_CreateIntconst(OPR_INTCONST, type, MTYPE_V, (INT64)s);
      wn_tmp2 = WN_Ashr(type, Load_Leaf(tmp3), wn_s);
  } else
      wn_tmp2 = Load_Leaf(tmp3);

  if (d > 0){
      wn_tmp3 = WN_Cvt(type, MTYPE_U4, Load_Leaf(tmp1));
      wn_tmp4 = WN_Lshr(MTYPE_U4, wn_tmp3, WN_Intconst(MTYPE_I4, 31));
  }
  else {
      wn_tmp3 = WN_Cvt(type, MTYPE_U4, Load_Leaf(tmp3));
      wn_tmp4 = WN_Lshr(MTYPE_U4, wn_tmp3, WN_Intconst(MTYPE_I4, 31));
  }
  wn_tmp3 = WN_Add(MTYPE_U4, wn_tmp2, wn_tmp4);
  wn_tmp3 = WN_Cvt(MTYPE_U4, type, wn_tmp3);
 
  return lower_expr(block, wn_tmp3, actions);
} 

static WN *lower_cst_udiv(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TYPE_ID  type = WN_rtype(tree);
  LEAF tmp1, tmp2;
  struct mu mag;
  INT32 M, s, d, a;
  WN *wn_M, *wn_s, *wn_tmp1, *wn_tmp2, *wn_tmp3, *wn_tmp4;

  if (WN_operator(WN_kid1(tree)) == OPR_LDID ) 
      d = (UINT32)WN_const_val(Preg_Home(WN_offset(WN_kid1(tree))));
  else
      d = (UINT32)WN_const_val(WN_kid1(tree));
  mag = magicu(d);
  M = mag.M;
  s = mag.s;
  a = mag.a;
  
  wn_M = WN_CreateIntconst(OPR_INTCONST, type, MTYPE_V, (INT64)M);
  tmp1 = Make_Leaf(block, WN_kid0(tree), type);
  tmp2 = Make_Leaf(block, wn_M, type);
  wn_tmp1 = WN_Cvt(type, MTYPE_I8, Load_Leaf(tmp1));
  wn_tmp2 = WN_Cvt(type, MTYPE_I8, Load_Leaf(tmp2));
  wn_tmp3 = WN_Mpy(MTYPE_I8, wn_tmp1, wn_tmp2);
  wn_tmp3 = WN_Ashr(MTYPE_I8, wn_tmp3, WN_Intconst(MTYPE_I4, 32));
  wn_tmp3 = WN_Cvt(MTYPE_I8, type, wn_tmp3);
  wn_tmp3 = lower_expr(block, wn_tmp3, actions);
  tmp2 = Make_Leaf(block, wn_tmp3, type);

  if ( a ){
      wn_s = WN_CreateIntconst(OPR_INTCONST, type, MTYPE_V, (INT64)(s-1));
      wn_tmp4 = WN_Sub(type, Load_Leaf(tmp1), Load_Leaf(tmp2));   
      wn_tmp4 = WN_Lshr(type, wn_tmp4, WN_Intconst(MTYPE_U4, 1));
      wn_tmp3 = WN_Add(type, wn_tmp4, Load_Leaf(tmp2)); 
      wn_tmp3 = WN_Lshr(type, wn_tmp3, wn_s);
  }
  else {
      wn_s = WN_CreateIntconst(OPR_INTCONST, type, MTYPE_V, (INT64)s);
      wn_tmp3 = WN_Lshr(type, Load_Leaf(tmp2), wn_s);
  }
  
  return lower_expr(block, wn_tmp3, actions);
} 


// [HK] Performs lowering of Remainder by constant, generating the transformed
//      tree.

static WN *lower_cst_irem(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TYPE_ID  type = WN_rtype(tree);
  LEAF tmp1, tmp2, tmp3;
  struct ms mag;
  INT32 M, s, d;
  WN *wn_M, *wn_s, *wn_tmp1, *wn_tmp2, *wn_tmp3, *wn_tmp4;

  if (WN_operator(WN_kid1(tree)) == OPR_LDID ) 
      d = (INT32)WN_const_val(Preg_Home(WN_offset(WN_kid1(tree))));
  else
      d = (INT32)WN_const_val(WN_kid1(tree));
  mag = magic(d);
  M = mag.M;
  s = mag.s;
  
  wn_M = WN_CreateIntconst(OPR_INTCONST, type, MTYPE_V, (INT64)M);
  tmp1 = Make_Leaf(block, WN_kid0(tree), type);
  tmp2 = Make_Leaf(block, wn_M, type);
  wn_tmp1 = WN_Cvt(type, MTYPE_I8, Load_Leaf(tmp1));
  wn_tmp2 = WN_Cvt(type, MTYPE_I8, Load_Leaf(tmp2));
  wn_tmp3 = WN_Mpy(MTYPE_I8, wn_tmp1, wn_tmp2);
  wn_tmp3 = WN_Ashr(MTYPE_I8, wn_tmp3, WN_Intconst(MTYPE_I4, 32));
  wn_tmp3 = WN_Cvt(MTYPE_I8, type, wn_tmp3);

  if ( (d > 0) && (M < 0) )
      wn_tmp3 = WN_Add(type, wn_tmp3, Load_Leaf(tmp1));
  else if ( (d < 0) && (M > 0) )
      wn_tmp3 = WN_Sub(type, wn_tmp3, Load_Leaf(tmp1));   
  
  wn_tmp3 = lower_expr(block, wn_tmp3, actions);
  tmp3 = Make_Leaf(block, wn_tmp3, type);

  if ( s > 0 ) {
      wn_s = WN_CreateIntconst(OPR_INTCONST, type, MTYPE_V, (INT64)s);
      wn_tmp2 = WN_Ashr(type, Load_Leaf(tmp3), wn_s);
  } else
      wn_tmp2 = Load_Leaf(tmp3);

  if (d > 0){
      wn_tmp3 = WN_Cvt(type, MTYPE_U4, Load_Leaf(tmp1));
      wn_tmp4 = WN_Lshr(MTYPE_U4, wn_tmp3, WN_Intconst(MTYPE_I4, 31));
  }
  else {
      wn_tmp3 = WN_Cvt(type, MTYPE_U4, Load_Leaf(tmp3));
      wn_tmp4 = WN_Lshr(MTYPE_U4, wn_tmp3, WN_Intconst(MTYPE_I4, 31));
  }
  wn_tmp3 = WN_Add(MTYPE_U4, wn_tmp2, wn_tmp4);
  wn_tmp3 = WN_Cvt(MTYPE_U4, type, wn_tmp3);
 
  wn_tmp1 = lower_expr(block, wn_tmp3, actions);
   
  tmp2 = Make_Leaf(block, wn_tmp1, type);
  tmp3 = Make_Leaf(block, WN_kid1(tree), type);
  wn_tmp1 = WN_Mpy(type, Load_Leaf(tmp2), Load_Leaf(tmp3));
  tmp3 = Make_Leaf(block, wn_tmp1, type);
  wn_tmp1 = WN_Sub(type, Load_Leaf(tmp1), Load_Leaf(tmp3));

  return lower_expr(block, wn_tmp1, actions);
}

static WN *lower_cst_urem(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TYPE_ID  type = WN_rtype(tree);
  LEAF tmp1, tmp2, tmp3;
  struct mu mag;
  INT32 M, s, d, a;
  WN *wn_M, *wn_s, *wn_tmp1, *wn_tmp2, *wn_tmp3, *wn_tmp4;

  if (WN_operator(WN_kid1(tree)) == OPR_LDID ) 
      d = (UINT32)WN_const_val(Preg_Home(WN_offset(WN_kid1(tree))));
  else
      d = (UINT32)WN_const_val(WN_kid1(tree));
  mag = magicu(d);
  M = mag.M;
  s = mag.s;
  a = mag.a;
  
  wn_M = WN_CreateIntconst(OPR_INTCONST, type, MTYPE_V, (INT64)M);
  tmp1 = Make_Leaf(block, WN_kid0(tree), type);
  tmp2 = Make_Leaf(block, wn_M, type);
  wn_tmp1 = WN_Cvt(type, MTYPE_I8, Load_Leaf(tmp1));
  wn_tmp2 = WN_Cvt(type, MTYPE_I8, Load_Leaf(tmp2));
  wn_tmp3 = WN_Mpy(MTYPE_I8, wn_tmp1, wn_tmp2);
  wn_tmp3 = WN_Ashr(MTYPE_I8, wn_tmp3, WN_Intconst(MTYPE_I4, 32));
  wn_tmp3 = WN_Cvt(MTYPE_I8, type, wn_tmp3);
  wn_tmp3 = lower_expr(block, wn_tmp3, actions);
  tmp2 = Make_Leaf(block, wn_tmp3, type);

  if ( a ){
      wn_s = WN_CreateIntconst(OPR_INTCONST, type, MTYPE_V, (INT64)(s-1));
      wn_tmp4 = WN_Sub(type, Load_Leaf(tmp1), Load_Leaf(tmp2));   
      wn_tmp4 = WN_Lshr(type, wn_tmp4, WN_Intconst(MTYPE_U4, 1));
      wn_tmp3 = WN_Add(type, wn_tmp4, Load_Leaf(tmp2)); 
      wn_tmp3 = WN_Lshr(type, wn_tmp3, wn_s);
  }
  else {
      wn_s = WN_CreateIntconst(OPR_INTCONST, type, MTYPE_V, (INT64)s);
      wn_tmp3 = WN_Lshr(type, Load_Leaf(tmp2), wn_s);
  }
  
  
  wn_tmp1 = lower_expr(block, wn_tmp3, actions);
   
  tmp2 = Make_Leaf(block, wn_tmp1, type);
  tmp3 = Make_Leaf(block, WN_kid1(tree), type);
  wn_tmp1 = WN_Mpy(type, Load_Leaf(tmp2), Load_Leaf(tmp3));
  tmp3 = Make_Leaf(block, wn_tmp1, type);
  wn_tmp1 = WN_Sub(type, Load_Leaf(tmp1), Load_Leaf(tmp3));

  return lower_expr(block, wn_tmp1, actions);
}
#endif // TARG_ST


/* ====================================================================
 *
 * WN *lower_tree_height(WN *block, WN *wn, LOWER_ACTIONS actions)
 *
 *  Reassociate binary commutative operator at this level 
 *  (ie no recursion)
 *
 * ==================================================================== */

#ifdef TARG_ST
/* 
 * Returns an equivalent form for opcode that are not dependent on the
 * signedness of the rtype.
 * This function is used for reassociation.
 * Returns the original opcode if the opcode is dependent on signedness.
 */
static OPCODE 
OPCODE_equivalent( OPCODE opc )
{
   
  OPCODE rop = (OPCODE) 0;
  TYPE_ID type;
  OPERATOR opr = OPCODE_operator(opc);
  TYPE_ID rtype = OPCODE_rtype(opc);
  
  switch (opr) {
  case OPR_ADD:
  case OPR_SUB:
  case OPR_MPY:
  case OPR_BAND:
  case OPR_BIOR:
  case OPR_BNOR:
  case OPR_BXOR:
  case OPR_BNOT:
    rop = opc;
    break;
  default:
    return opc;
  }
  
  switch (rtype) {
  case MTYPE_I4:
  case MTYPE_U4:
    type = MTYPE_I4;
    break;
  case MTYPE_I8:
  case MTYPE_U8:
    type = MTYPE_I8;
    break;
  default:
    return opc;
  }
  return OPCODE_make_op(opr, type, MTYPE_V);
}
#endif

static WN *lower_tree_height(WN *block, WN *wn, LOWER_ACTIONS actions)
{
  OPCODE  opcode = WN_opcode(wn);
  WN	  *l, *r;

  if (NotAction(LOWER_TREEHEIGHT))
    return wn;

  Is_True(WN_is_commutative(wn),("expected commutative op"));

  l = WN_kid0(wn);
  r = WN_kid1(wn);

  OPCODE opcode_l = WN_opcode(l);
  OPCODE opcode_r = WN_opcode(r);
#ifdef TARG_ST
  opcode = OPCODE_equivalent(opcode);
  opcode_l = OPCODE_equivalent(opcode_l);
  opcode_r = OPCODE_equivalent(opcode_r);
#endif

 /*
  *  do not transform an already balanced tree
  */
  if (opcode == opcode_l ^ opcode == opcode_r)
  {
    WN *wn1 =  (opcode == opcode_l) ? l : r;
    WN *wn1X = (opcode == opcode_l) ? r : l;

    WN *wn1_l = WN_kid0(wn1);
    WN *wn1_r = WN_kid1(wn1);

    OPCODE opcode1_l = WN_opcode(wn1_l);
    OPCODE opcode1_r = WN_opcode(wn1_r);
#ifdef TARG_ST
    opcode1_l = OPCODE_equivalent(opcode1_l);
    opcode1_r = OPCODE_equivalent(opcode1_r);
#endif
    if (opcode == opcode1_l ^ opcode == opcode1_r)
    {

      WN *wn2 =  (opcode == opcode1_l) ? wn1_l : wn1_r;
      WN *wn2X = (opcode == opcode1_l) ? wn1_r : wn1_l;
     /*
      *  rearrange tree
      */
      WN_kid0(wn1) =	wn2X;
      WN_kid1(wn1) =	wn1X;

      WN_kid0(wn) =	wn2;
      WN_kid1(wn) =	wn1;

      if (traceTreeHeight)
      {
	DevWarn("lower_tree_height: trace (%s) has been reassociated (line %d)",
	        OPCODE_name(WN_opcode(wn)), Srcpos_To_Line(current_srcpos));
      }
    }
  }
  return wn;
}



#ifdef KEY
/* ====================================================================
 *
 * WN *sum_madd_segments(WN **segments, INT low, INT high)
 *
 *  Sum the MADD chains.
 *
 * ==================================================================== */
static WN *sum_madd_segments(WN **segments, INT low, INT high)
{
  WN *tree;

  if (high == low) {
    tree = segments[high];
  } else if (high - low == 1) {
    WN *opnd0 = segments[low];
    WN *opnd1 = segments[high];
    tree = WN_Add(WN_rtype(opnd0), opnd0, opnd1);
  } else {
    INT mid = (high + low) / 2;
    WN *sum0 = sum_madd_segments(segments, low, mid);
    WN *sum1 = sum_madd_segments(segments, mid + 1, high);
    tree = WN_Add(WN_rtype(sum0), sum0, sum1);
  }
  return tree;
}

/* ====================================================================
 *
 * WN *lower_one_madd_tree_height(WN *block, WN *wn)
 *
 *  Reassociate MADD operator at this level 
 *  (ie no recursion)
 *
 * ==================================================================== */

static WN *lower_one_madd_tree_height(WN *block, WN *wn)
{
  // Tree is a MADD and potentially starts a MADD chain.
  Is_True(WN_operator(wn) == OPR_MADD,
	  ("lower_one_madd_tree_height: MADD operator not found"));

  if (OPT_Madd_Height <= 0)
    return wn;

  // Count the MADDs in the chain.
  INT madd_count = 0;
  WN *this_wn;
  for (this_wn = wn;
       WN_operator(this_wn) == OPR_MADD;
       this_wn = WN_kid0(this_wn)) {
    madd_count++;
  }

  if (madd_count <= OPT_Madd_Height)
    return wn;

  // 14657: Balance the chain lengths, and limit the number of chains
  INT segment_count = madd_count / (OPT_Madd_Height + 1) + 1;
  if (segment_count > OPT_Madd_Chains) segment_count = OPT_Madd_Chains;
  INT segment_length = (madd_count + 1) / segment_count - 1;  // min length
  INT remainder_idx = segment_count - (madd_count + 1) % segment_count;

  WN **segments = (WN **) alloca(segment_count * sizeof(WN *));
  memset(segments, 0, segment_count * sizeof(WN *));

  // Split up the MADD chain.
  this_wn = wn;
  segments[0] = this_wn;
  for (INT idx = 1; idx < segment_count; ++idx) {
    for (INT count = 0; count < segment_length - 1; ++count) {
      Is_True(WN_operator(this_wn) == OPR_MADD,
	      ("lower_one_madd_tree_height: Wrong chain lengths 1"));
      this_wn = WN_kid0(this_wn);
    }
    WN *next_wn = WN_kid0(this_wn);
    Is_True(WN_operator(next_wn) == OPR_MADD,
	    ("lower_one_madd_tree_height: Wrong chain lengths 2"));
    WN *mpy = WN_Mpy(WN_rtype(next_wn), WN_kid1(next_wn), WN_kid2(next_wn));
    WN_kid0(this_wn) = mpy;
    this_wn = WN_kid0(next_wn);
    WN_Delete(next_wn);
    if (idx == remainder_idx) ++segment_length;
    segments[idx] = this_wn;
  }

  // Add the partial sums.
  WN *tree = sum_madd_segments(segments, 0, segment_count - 1);
  return tree;
}


/* ====================================================================
 *
 * WN *lower_madd_tree_height(WN *block, WN *tree)
 *
 *  Traverse tree to find subtrees that are MADD chains.  Lower the height
 *  of these chains.
 *
 * ==================================================================== */

static void lower_madd_tree_height(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  if (NotAction(LOWER_TREEHEIGHT))
    return;

  if (WN_operator(tree) != OPR_MADD) {
    INT16 i;
    for (i = 0; i < WN_kid_count(tree); i++) {
      WN *kid = WN_kid(tree, i);
      if (WN_operator(kid) == OPR_MADD) {
	// Kid potentially begins a MADD chain.  Reduce its height.
	WN_kid(tree, i) = lower_one_madd_tree_height(block, kid);
      } else {
	lower_madd_tree_height(block, kid, actions);
      }
    }
  }
}
#endif




/* ====================================================================
 *
 * WN *lower_recip(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * WN *lower_rsqrt(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * lower recip/rsqrt based on ISA flags
 *
 * Quad is lowered unconditionally, as there is no runtime support
 * (kludge alert: turn off simplifer when creating quad divide !!)
 *
 * ==================================================================== */

static WN *lower_recip(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TYPE_ID  type = WN_rtype(tree);
  WN	  *kid0 = WN_kid0(tree);
  WN	  *div =  NULL;

  Is_True(WN_operator_is(tree, OPR_RECIP), ("expected recip"));
  Is_True(MTYPE_float(type), ("expected float type"));


#ifndef KEY
  if (Recip_Allowed == FALSE)
#else
  if (Recip_Allowed == FALSE && Div_Split_Allowed == FALSE)
#endif
  {
    div = WN_Div(type, WN_Floatconst(type, 1.0), kid0);
  }
  else if (Action(LOWER_QUAD) && MTYPE_is_quad(type))
  {
    BOOL  simp=	WN_Simplifier_Enable(FALSE);

    div = WN_Div(type, WN_Floatconst(type, 1.0), kid0);

    WN_Simplifier_Enable(simp);
  }

  if (div)
  {
    WN_Delete(tree);
    return div;
  }

  return tree;
}


static WN *lower_rsqrt(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TYPE_ID  type = WN_rtype(tree);

  Is_True(WN_operator_is(tree, OPR_RSQRT), ("expected rsqrt"));
  Is_True(MTYPE_float(WN_rtype(tree)), ("expected float type"));

#ifdef TARG_X8664	// bug 6123
  if (Rsqrt_Allowed == 0 ||
      // x86-64 rsqrt supports single-precision only.
      !(type == MTYPE_F4 || type == MTYPE_V16F4) ||
      !Is_Target_SSE2())
#else
  if (Rsqrt_Allowed == FALSE || MTYPE_is_quad(type))
#endif
  {
    WN	*div;

    div = WN_Div(type, WN_Floatconst(type, 1.0), WN_Sqrt(type, WN_kid0(tree)));
    WN_Delete(tree);

    return div;
  }

#ifdef TARG_X8664
  if (Rsqrt_Allowed == 1) {
    /* Rsqrt followed by one iteration of Newton-Raphson to refine the result:
	 a = rsqrt(x)      ;; get approximate rsqrt(x)
	 b = x * a
	 c = b * a         ;; c has error E
	 d = c - 3.0
	 e = d * a
	 f = e * (-0.5)
       f now has 1/sqrt(x).  Bug 6123.

       Change the operator from RSQRT to ATOMIC_RSQRT to prevent infinite
       recursion.
     */
    WN_set_operator(tree, OPR_ATOMIC_RSQRT);
    LEAF x = Make_Leaf(block, WN_kid0(tree), type);
    WN_kid0(tree) = Load_Leaf(x);
    PREG_NUM a = AssignExpr(block, tree, type);
    WN *b = WN_Mpy(type, Load_Leaf(x), WN_LdidPreg(type, a));
    WN *c = WN_Mpy(type, b, WN_LdidPreg(type, a));
    WN *d = WN_Sub(type, c, WN_Floatconst(type, 3.0));
    WN *e = WN_Mpy(type, d, WN_LdidPreg(type, a));
    WN *f = WN_Mpy(type, e, WN_Floatconst(type, -0.5));
    return f;
  }
#endif

  return tree;
}


/* IPA can mark variables gp-relative, even if they are not allocated
 * in this source file. To deal with them, we explicitly check if a 
 * variable is gp-relative.
 */
static BOOL Symbol_Is_Base_Register (ST *sym)
{
  return ((ST_class(sym) == CLASS_BLOCK && STB_is_basereg(sym)) ||
          (ST_class(sym) == CLASS_VAR && ST_gprel(sym)));
}



/* ====================================================================
 *
 * WN *lower_split_sym_addrs(WN *tree, INT64 offset, LOWER_ACTIONS actions)
 *
 * Split symbols into base/offset depending on type/class etc.
 *
 *
 * ==================================================================== */

static WN *lower_split_sym_addrs(WN *tree, INT64 offset, LOWER_ACTIONS actions)
{
  ST *sym  = WN_st(tree);
  ST *base = ST_base(sym);
  INT64	newoffset = 0;
  
#ifdef TARG_ST
  //
  // Arthur:
  //
  // I am not sure how to handle this for now -- do not do split sym
  // for the ST targets ?
  //
  // WOPT/LNO shouldn't be affected ?? 
  //
  // Problem:
  //
  // ST100 assembler can not handle %reloc(.data + 8),
  // it needs symbol name, eg. ptr = %reloc(y), ptr = $gp + ptr,
  // adr = ptr + 8
  // TODO: have a config variable that specifies whether to do this
  //       for a given target or not.

  // We need some of it though:
  //
  //    1. struct foo x;
  //       struct foo { int i; }
  //
  //       Alignment of x = 1 (uncomplete type), whereas x.i
  //       is loaded via I4I4LDID 0 <x> <field_id:1>
  //
  //       Need to convert it into LDA ... in order to avoid
  //       unaligned load/stores.
  //
#endif

  #ifndef TARG_ST
  // [CG]: What is this!? We exit if we are in tracing mode! 
  // Seems to be a bug.
  if (traceSplitSymOff)
    return NULL;
#endif

  switch(ST_class(sym))
  {
#ifndef TARG_ST
  case CLASS_CONST:
   /*
    *  for non_shared, non-gprelative constants we want to expose the
    *  base for RVI
    */
    if (PIC_NONSHARED && (ST_size(sym) > Max_Sdata_Elt_Size))
    {
      Allocate_Object(sym);
      Base_Symbol_And_Offset_For_Addressing(sym, offset, &base, &newoffset);
      return lower_base_reference(tree, base, newoffset, actions);
    }
    return NULL;
#endif

  case CLASS_PREG:
    return NULL;

#ifndef TARG_ST
  case CLASS_FUNC:
    if (PIC_SHARED && ST_is_preemptible(sym))
    {
      return NULL;
    }
    break;

  case CLASS_BLOCK:
#endif
  case CLASS_VAR:
    Base_Symbol_And_Offset_For_Addressing(sym, offset, &base, &newoffset);

    if (ST_is_uplevelTemp(sym))
    {
      return NULL;
    }
    if (ST_assigned_to_dedicated_preg(sym)) 
    {
	return NULL;
    }

   /*
    *  We cannot keep lowering LDA's or we will forever recurse
    */
    if (WN_operator_is(tree, OPR_LDA) && base == sym) 
      return NULL;

#ifdef TARG_ST
    //
    // In order to avoid splitting into .section + offset (see note
    // earlier), don't do if:
    //
    if (base != sym)
      return NULL;
#endif

    switch(ST_sclass(sym))
    {
    case SCLASS_REG:
    case SCLASS_TEXT:
      return NULL;

    case SCLASS_FORMAL_REF:	
     /*
      *	 If we expand the base, we will lose the FORMAL_REF'ness of the ST
      *  and will not be able to dereference it later
      */
      if (base != sym)
      {
 	base =	sym;
      }
      break;

    case SCLASS_FORMAL:
     /*
      *  Do not allocate Formal_Arg_StkSeg, as it is not correct yet !!
      *  (it will eventually be .sp + <offset> )
      */
      return NULL;

    case SCLASS_AUTO:
      /* 
       * For now, never split a stack variable.
       * We only see stack variables with bases when we do regions,
       * and for now we want to keep those cases looking like the
       * regular cases for correctness.  But at a later date we should
       * try to split these in the region case, as that may allow a
       * base register to be allocated for things like FP+32000,
       * so we could then get 1-instruction accesses to large offsets.
       * But that requires a cgexp change too, so wait on it.
       */
#if 0
      if (Uses_Small_Offset(sym, offset))
        break;
#endif
      return NULL;

    case SCLASS_EXTERN:
     /*
      *	okay to split these. A dlopen() can never redefine these (unless
      * they are weak)
      */
      if (ST_is_weak_symbol(sym))
	return NULL;
      break;

    case SCLASS_COMMON:
     /*
      *	commons may be preempted by a dlopen() (if the new definition is
      * initialized) see wilson/lillian for details of this rather obscure
      * point suneel says not to worry about this though, so please direct
      * bugs to him
      *
      * In the new symbol table, we need to also split common symbols
      * here. The reason is that now both members and the common itself
      * are SCLASS_COMMON, whereas before they used to be SCLASS_BASED.
      * We no longer have to worry about the ST_Full that was present in
      * the old symbol table
      */
      sym = base;
      break;

    case SCLASS_PSTATIC:
    case SCLASS_UGLOBAL:
    case SCLASS_DGLOBAL:
    case SCLASS_FSTATIC:
     /*
      *  okay to split these. We will never split incorrectly as
      *	 Base_Symbol_And_Offset_For_Addressing() will not split a preemptible
      *  symbol
      */
      break;

    default:
      return NULL;
    }

    if (ST_gprel(sym)
#ifdef TARG_ST
	|| (Is_Structure_Type(Ty_Table[ST_type (sym)]) &&
	    TY_is_packed(Ty_Table[ST_type (sym)]))
#endif
       )
    {
      return NULL;
    }
    tree = lower_base_reference(tree, base, newoffset, actions);
    return tree;

  default:
    return NULL;
  }

  return NULL;
}




/* ====================================================================
 *
 * WN *lower_uplevel_reference(WN *tree, INT64 offset, LOWER_ACTIONS actions)
 *
 * Split uplevel symbols into indirect of slink
 *
 * ==================================================================== */

static WN *lower_uplevel_reference(WN *tree, INT64 offset,
				   LOWER_ACTIONS actions)
{
  ST	*sym = WN_st(tree);
  ST	*base;
  INT64	newoffset;

  Is_True(ST_is_uplevelTemp(sym), ("expected uplevel %s",ST_name(sym)));

  Base_Symbol_And_Offset_For_Addressing(sym, offset, &base, &newoffset);
  base = Find_Slink_For_ST(sym);

  tree = lower_dereference(tree, newoffset, base, 0, actions);
  return tree;
}




/* ====================================================================
 *
 * WN *lower_formal_ref(WN *tree, INT64 offset, ST *base,
 *                      LOWER_ACTIONS actions)
 *
 * lower an SCLASS_FORMAL_REF into a dereference of the new base.
 * or
 * lower an SCLASS_FORMAL into a preg, already computed
 *
 * ==================================================================== */
static WN *lower_formal_ref(WN *tree, INT64 offset, ST *base,
			    LOWER_ACTIONS actions)
{
  PREG_NUM	preg;

  switch(ST_sclass(base))
  {
  case SCLASS_FORMAL_REF:
    base = Get_ST_formal_ref_base(base);
    if (preg = Get_ST_formal_preg_num(base))
    {
      Is_True((ST_has_nested_ref(WN_st(tree))==FALSE),
	      ("lower_formal_ref: cannot use preg for nested ref %s",
	       ST_name(WN_st(tree))));
      base = MTYPE_To_PREG(TY_mtype(Ty_Table[ST_type(base)]));
    }
    return lower_dereference(tree, offset, base, preg, actions);

  case SCLASS_FORMAL:
    if (preg= Get_ST_formal_preg_num(base))
    {
      base = MTYPE_To_PREG(TY_mtype(Ty_Table[ST_type(base)]));
      tree = lower_base_register(tree, base, preg, actions);
      return tree;
    }
    break;
  }
  return NULL;
}




/* ====================================================================
 *
 * WN *lower_base_register(WN *tree, ST *base, INT64 offset,
 *                         LOWER_ACTIONS actions)
 *
 * common routine for lowering to a base register for LDA, LDID, STID
 * into using a new base/offset
 *
 * ==================================================================== */

static WN *lower_base_register(WN *tree, ST *base, INT64 offset,
			       LOWER_ACTIONS actions)
{
  Is_True(WN_st(tree) != base, ("lower_base_register(): possible recursion"));

  switch (WN_operator(tree))
  {
  case OPR_LDA:
   /*
    * Convert
    *     LDA (offset) <sym> into
    *
    *     LDA (offset) <base>
    */
    WN_st_idx(tree) = ST_st_idx(base);
    WN_lda_offset(tree) = mem_offset_hi(offset);
    return tree;

  case OPR_LDID:
   /*
    * Convert
    *	  LDID (offset> <sym>  into
    * 	  LDID (offset <base>
    */
    WN_st_idx(tree) = ST_st_idx(base);
    WN_load_offset(tree) = offset;
    break;

  case OPR_STID:
   /*
    * Convert
    *	  STID (offset> <sym>  into
    * 	  STID (offset <base>
    */
    WN_st_idx(tree) = ST_st_idx(base);
    WN_store_offset(tree) = offset;
    break;

  default:
    Is_True(FALSE,("expected lda/ldid/stid"));
    return NULL;
  }

  return tree;
}


// replace the type of LDID/STID nodes with non-zero field_id to the type
// of the field
static void
lower_field_id (WN* tree)
{
  OPERATOR opr = WN_operator(tree);

  Is_True (opr == OPR_LDID || opr == OPR_STID || opr == OPR_MLOAD ||
	   opr == OPR_MSTORE, ("expecting LDID or STID nodes"));

  if (WN_field_id (tree) == 0)
    return;

  BOOL is_ptr_type = (opr == OPR_MLOAD || opr == OPR_MSTORE);

  TY_IDX ty_idx = is_ptr_type ? TY_pointed (WN_ty (tree)) : WN_ty (tree);

  Is_True (TY_kind (ty_idx) == KIND_STRUCT,
	   ("non-zero field id must have KIND_STRUCT"));

  UINT field_id = 0;
  FLD_HANDLE fld = FLD_get_to_field (ty_idx, WN_field_id (tree), field_id); 
  
  Is_True (! fld.Is_Null (), ("invalid bit-field ID for %s",
			      OPERATOR_name(opr)));

#ifdef TARG_ST
  // [CG] When converting struct type to type of the field,
  // we must propagate the volatile attribute
  TY_IDX fld_ty_idx = FLD_type (fld);
  TY_IDX iload_ty_idx;
  if (WN_Is_Volatile_Mem(tree)) Set_TY_is_volatile(fld_ty_idx);
  WN_set_ty (tree, (is_ptr_type ? Make_Pointer_Type (fld_ty_idx) :
		    fld_ty_idx));
#else
  WN_set_ty (tree, (is_ptr_type ? Make_Pointer_Type (FLD_type (fld)) :
		    FLD_type (fld)));
#endif
  WN_set_field_id (tree, 0);
  return;
} // lower_field_id


/* ====================================================================
 *
 * WN *lower_base_reference(WN *tree, ST *base, INT64 offset,
 *                          LOWER_ACTIONS actions)
 *
 * common routine for lowering to a base reference for LDA, LDID, STID
 *
 * ==================================================================== */

static WN *lower_base_reference(WN *tree, ST *base, INT64 offset,
				LOWER_ACTIONS actions)
{
  WN    *addr, *wn;

  switch (WN_operator(tree))
  {
  case OPR_LDA:
   /*
    * Convert
    *     LDA (offset) <sym>
    * 
    * (lo > 0)
    *       LDA (hi) <base>
    *       CONST (lo)
    *     ADD
    *
    * (offset >= 2GB)
    *       LDA (0) <base>
    *       CONST (offset)
    *     ADD
    */

    WN_st_idx(tree) = ST_st_idx(base);

    if (mem_offset_2GB(offset))
    {
      addr =	WN_Add(Pointer_type, tree, WN_Intconst(Pointer_type, offset));
      WN_lda_offset(tree)=  0;
      return addr;
    }
    else if (Action(LOWER_SPLIT_CONST_OFFSETS)	&&
	     mem_offset_lo(offset))
    {
     /* turn off simplifier else ADD may be removed */
      BOOL  simp=        WN_Simplifier_Enable(FALSE);

      addr =	WN_Add(Pointer_type,
		       tree,
		       WN_Intconst(Pointer_type, mem_offset_lo(offset)));
      WN_lda_offset(tree)=	mem_offset_hi(offset);

      WN_Simplifier_Enable(simp);
      return addr;
    }
    WN_lda_offset(tree)=  offset;
    return tree;

  case OPR_LDID:
  case OPR_STID:
  case OPR_LDBITS:
  case OPR_STBITS:
   /*
    * Process the common address
    *
    * if (offset > 2GB)
    *      LDA (hi) <sym>
    *	   CONST offset
    *    ADD
    *
    * if (offset > 16K)
    *      LDA (hi) <sym>
    *    ILOAD | ISTORE (lo)
    */
    addr =	WN_Lda(Pointer_type, 0, base);

    if (mem_offset_2GB(offset))
    {
      addr =	WN_Add(Pointer_type, addr, WN_Intconst(Pointer_type, offset));
      offset =	0;
    }
    if (Action(LOWER_SPLIT_CONST_OFFSETS) && mem_offset_must_be_split(offset))
    {
      WN_lda_offset(addr) =	mem_offset_hi(offset);
      offset =			mem_offset_lo(offset);
    }
    break;

  default:
    Is_True(FALSE,("expected lda/ldid/stid"));
    return NULL;
  }	

  switch (WN_operator(tree))
  {
  case OPR_LDID:
   /*
    * Convert (LDID () <sym>) into
    *      LDA (0) <base>
    *    ILOAD (offset+ofst)
    */
    if (WN_field_id (tree) != 0)
      lower_field_id (tree);
    
    wn =	WN_RIload(WN_rtype(tree), WN_desc(tree), offset, WN_ty(tree),
			  addr);
    break;
  case OPR_LDBITS:
    wn =	WN_RIload(WN_rtype(tree), WN_desc(tree), offset, WN_ty(tree),
			  addr);
    WN_set_operator(wn, OPR_ILDBITS);
    WN_set_bit_offset_size(wn, WN_bit_offset(tree), WN_bit_size(tree));
    break;

  case OPR_STID:
   /*
    * Convert   STID (offset) <sym>
    *  into
    *       LDA (0) <base>
    *     ISTORE (offs+ofst)
    */
    if (WN_field_id (tree) != 0)
      lower_field_id (tree);
    
    wn =	WN_Istore(WN_desc(tree),
			  offset,
			  // The resulting TY is not an f90 pointer
			  // type because we're converting an STID.
			  // In the case where this STID
			  // came about because of conversion of
			  // ISTORE(something) in the past, and that
			  // ISTORE was through an f90 pointer, the
			  // destination of the STID will have the
			  // F90_TARGET attribute, and we're OK.
			  Make_Pointer_Type(WN_ty(tree)),
			  addr,
			  WN_kid0(tree));
    WN_Set_Linenum(wn, WN_Get_Linenum(tree));
    break;
  case OPR_STBITS:
    wn =	WN_Istore(WN_desc(tree),
			  offset,
			  Make_Pointer_Type(WN_ty(tree)),
			  addr,
			  WN_kid0(tree));
    WN_set_operator(wn, OPR_ISTBITS);
    WN_set_bit_offset_size(wn, WN_bit_offset(tree), WN_bit_size(tree));
    WN_Set_Linenum(wn, WN_Get_Linenum(tree));
    break;
  }	

  lower_copy_maps(tree, wn, actions);
  WN_Delete(tree);

  return wn;
}



/* ====================================================================
 *
 * WN *lower_dereference(WN *tree, INT64 offset, ST *base,
 *                       PREG_NUM *preg, LOWER_ACTIONS actions)
 *
 * Perform address dereferencing for SCLASS_FORMAL_REF and uplevel variable
 * Caller is responsible for any further lowering.
 *
 *  For uplevel references the idea is to replace the st with a reference
 *  of the slink_sym at level ST_level(st) + 1
 *  This will only work recursively
 *  (ie. that sym must be referenced via the next level's scope
 *
 * ==================================================================== */
static WN
*lower_dereference(WN *tree, INT64 offset, ST *base, PREG_NUM preg,
		   LOWER_ACTIONS actions)
{
  WN	 *addr, *deref;
  TY_IDX  addr_ty;

  switch (WN_operator(tree))
  {
  case OPR_LDA:
   /*
    * Convert uplevel reference
    *     LDA (offset) <sym>
    *  into 
    *       LDID (0) <base>
    *       CONST (offset)    
    *     ADD
    */
    deref =	WN_Ldid(Pointer_type, preg, base, WN_ty(tree));
    addr =	WN_Add(Pointer_type, deref, WN_Intconst(Pointer_type, offset));
    return addr;

  case OPR_LDID:
  case OPR_STID:
   /*
    * Process the common address
    *
    */
    addr_ty = Make_Pointer_Type(WN_ty(tree));
    addr =	WN_Ldid(Pointer_type, preg, base, addr_ty);

    if (mem_offset_2GB(offset))
    {
      addr =	WN_Add(Pointer_type, addr, WN_Intconst(Pointer_type, offset));
      offset =	0;
    }
    break;

  default:
    Is_True(FALSE,("expected lda/ldid/stid"));
    return NULL;
  }	

  switch (WN_operator(tree))
  {
  case OPR_LDID:
   /*
    * Convert uplevel reference  LDID () <sym>
    * into
    *      LDID (preg) <base>
    *    ILOAD (offset)
    */
    deref = WN_RIload(WN_rtype(tree), WN_desc(tree), offset, WN_ty(tree), addr,
    		      WN_field_id(tree));
    break;

  case OPR_STID:
   /*
    * Convert  STID () <sym>
    * into
    *       LDID (preg) <base> 
    *     ISTORE (offset)          
    */
    deref = WN_Istore(WN_desc(tree), offset, addr_ty, addr, WN_kid0(tree)
#ifdef KEY // bug 14036
                      ,WN_field_id(tree)
#endif
                     );
    WN_Set_Linenum(deref, WN_Get_Linenum(tree));
    break;
  }	

  lower_copy_maps(tree, deref, actions);
  WN_Delete(tree);

  return deref;
}

/* ====================================================================
 *
 * WN *lower_return_ldid(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on 'LDID Return_Val_Preg'
 * nodes returning a lowered 'LDID normal_PREG' node.
 *
 * ==================================================================== */

static WN *lower_return_ldid(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TY_IDX   ty_idx  = WN_ty(tree);
  TY&      ty      = Ty_Table[ty_idx];
  TYPE_ID  mtype   = TY_mtype (ty);
#ifdef TARG_X8664
  BOOL sseregparm  = FALSE; // sseregparm attribute on called function?
#endif

  Is_True((WN_operator(tree) == OPR_LDID),
	  ("expected LDID node, not %s", OPCODE_name(WN_opcode(tree))));

#ifdef TARG_X8664
  if (Is_Target_32bit())
  {
    // Determine if the called function has the "sseregparm" attribute
    WN * call = WN_last(block);
    if (call && WN_operator(call) == OPR_CALL)
    {
      TY_IDX pu_ty = PU_prototype(Pu_Table[ST_pu(WN_st(call))]);
      sseregparm = TY_has_sseregister_parm (pu_ty);
    }
  }
#endif

  switch (mtype) {

    case MTYPE_I8:
    case MTYPE_U8:
      WN_st_idx(tree) = ST_st_idx(MTYPE_To_PREG(mtype));
      WN_load_offset(tree) = First_Int_Preg_Return_Offset;
      break;

    case MTYPE_I1:
    case MTYPE_I2:
    case MTYPE_I4:
    case MTYPE_U1:
    case MTYPE_U2:
    case MTYPE_U4:
#ifdef TARG_ST
    case MTYPE_U5:
#endif
#ifdef TARG_MIPS
    case MTYPE_V8I4:
#endif
      WN_st_idx(tree) = ST_st_idx(Int_Preg);
      WN_load_offset(tree) = First_Int_Preg_Return_Offset;
      break;

#ifdef TARG_ST
    case MTYPE_FQ:
    case MTYPE_C4:
    case MTYPE_C8:
    case MTYPE_CQ:
      FmtAssert(FALSE,("can't handle this"));
      break;

    case MTYPE_F4:
      // Check if floating point return value is passed into an int preg.
      if (Preg_Offset_Is_Int(RETURN_INFO_preg(Get_Return_Info(MTYPE_To_TY(mtype), Use_Simulated), 0))) {
	//
	// replace F4 LDID with I4 LDID because this is dedicated
	// hardware register and can't be a F4.
	//
	WN *new_tree = WN_CreateLdid(OPR_LDID, 
				     MTYPE_I4, 
				     MTYPE_I4, 
				     First_Int_Preg_Return_Offset,
				     MTYPE_To_PREG (MTYPE_I4),
				     Be_Type_Tbl(MTYPE_I4),
				     WN_field_id(tree));
	WN_Delete(tree);
	tree = WN_Tas(mtype, Be_Type_Tbl(mtype), new_tree);
      }
      else {
	WN_st_idx(tree) = ST_st_idx(Float_Preg);
	WN_load_offset(tree) = First_Float_Preg_Return_Offset;
      }
      break;

    case MTYPE_F8:
      // Check if floating point return value is passed into an int preg.
      if (Preg_Offset_Is_Int(RETURN_INFO_preg(Get_Return_Info(MTYPE_To_TY(mtype), Use_Simulated), 0))) {
	//
	// replace F8 LDID with I8 LDID because this is dedicated
	// hardware register and can't be a F8.
	//
	WN *new_tree = WN_CreateLdid(OPR_LDID, 
				     MTYPE_I8, 
				     MTYPE_I8, 
				     First_Int_Preg_Return_Offset,
				     MTYPE_To_PREG (MTYPE_I8),
				     Be_Type_Tbl(MTYPE_I8),
				     WN_field_id(tree));
	WN_Delete(tree);
	tree = WN_Tas(mtype, Be_Type_Tbl(mtype), new_tree);
      }
      else {
	WN_st_idx(tree) = ST_st_idx(Float_Preg);
	WN_load_offset(tree) = First_Float_Preg_Return_Offset;
      }
      break;
#else
    case MTYPE_F4:
    case MTYPE_F8:
    case MTYPE_FQ:
    case MTYPE_F16:
    case MTYPE_C4:
    case MTYPE_C8:
    case MTYPE_CQ:
    case MTYPE_C16:
#ifdef TARG_MIPS
    case MTYPE_V8F4:
#endif
      WN_st_idx(tree) = ST_st_idx(Float_Preg);
      WN_load_offset(tree) = First_Float_Preg_Return_Offset;
#ifdef TARG_X8664 // MTYPE_C4 is returned in one SSE register
      if (Is_Target_64bit() && mtype == MTYPE_C4) {
	WN_set_rtype(tree, MTYPE_F8);
	WN_set_desc(tree, MTYPE_F8);
	WN_set_ty(tree, MTYPE_To_TY(MTYPE_F8));
      }

      if( Is_Target_32bit() && ! sseregparm && ! MTYPE_is_complex(mtype) ||
	  MTYPE_is_quad(mtype) ){
	WN_st_idx(tree) = ST_st_idx( MTYPE_To_PREG(mtype) );
	WN_load_offset(tree) = First_X87_Preg_Return_Offset;
      }
#endif
      break;
#endif
    case MTYPE_A4:
#ifdef TARG_ST
      /*
       * Arthur: if Pointer_Mtype is not pointer, we map ptr regs
       *         onto integer regs
       */
      if (MTYPE_is_class_pointer(Pointer_Mtype)) {
	WN_st_idx(tree) = ST_st_idx(Ptr_Preg);
	WN_load_offset(tree) = First_Ptr_Preg_Return_Offset;
      }
      else {
	WN_set_rtype(tree, MTYPE_I4);
	WN_set_desc(tree, MTYPE_I4);
	WN_st_idx(tree) = ST_st_idx(Int_Preg);
	WN_load_offset(tree) = First_Int_Preg_Return_Offset;
	tree = WN_Tas(mtype, ty_idx, tree);
      }
#endif
      break;

#ifdef TARG_X8664
    case MTYPE_V16I1:
    case MTYPE_V16I2:
    case MTYPE_V16I4:
    case MTYPE_V16I8:
    case MTYPE_V16F4:
    case MTYPE_V16F8:
    case MTYPE_V8I1:
    case MTYPE_V8I2:
    case MTYPE_V8I4:

    case MTYPE_M8I1:
    case MTYPE_M8I2:
    case MTYPE_M8I4:
    case MTYPE_M8F4:
      WN_st_idx(tree) = ST_st_idx(MTYPE_To_PREG (mtype));
      WN_load_offset(tree) = First_Float_Preg_Return_Offset;
      break;
#endif

    case MTYPE_M:
      Fail_FmtAssertion ("MLDID of Return_Val_Preg not allowed in middle"
			 " of expression");
      /*NOTREACHED*/
    default:
#ifdef TARG_ST
     // YJ: for dynamic mtypes, we cancel lowering, 
     // LDID operator is used as a return
     // result of an intrinsic call.
     // This mechanism could be extended in order to manage
     // other types.

     // TODO: search for INTRINSIC_CALL operator. 
     // For multi-results builtins, relation
     // WN_operator(WN_last(block)) == OPR_INTRINSIC_CALL is not always
     // true.
     if( MTYPE_is_dynamic(mtype) ) { 

         if(traceLdid) {
           fprintf(TFile,"-->Lowering dynamic mtype \"%s\" ldid return preg\n",
                         MTYPE_name(mtype));
         }

         WN_st_idx(tree) = ST_st_idx(MTYPE_To_PREG(mtype));
         WN_load_offset(tree) = -1;  // By convention
     }
     else {
      Fail_FmtAssertion ("Unexpected type in lower_return_ldid");
     }
#else
      Fail_FmtAssertion ("Unexpected type in lower_return_ldid");
#endif
      /*NOTREACHED*/
  }
  return tree;
}

static TY_IDX
get_field_type (TY_IDX struct_type, UINT field_id)
{
  Is_True (TY_kind (struct_type) == KIND_STRUCT, ("expecting KIND_STRUCT"));
  UINT cur_field_id = 0;
  FLD_HANDLE fld = FLD_get_to_field (struct_type, field_id, cur_field_id);
  Is_True (! fld.Is_Null(), ("Invalid field id %d for type 0x%x",
			     field_id, struct_type));
  return FLD_type (fld);
}

/* ====================================================================
 *
 * WN *lower_mldid(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on MLDID nodes returning
 * an equivalent MLOAD node.  Note that MLDID's of Return_Val_Preg are
 * not lowered here.  They are handled by lower_return_ldid.
 *
 * ==================================================================== */

static WN *lower_mldid(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TY_IDX ty_idx  = WN_ty(tree);
  TY_IDX pty_idx;
  UINT64 size    = WN_field_id(tree) == 0 ?
      TY_size(ty_idx) :
      TY_size(get_field_type (ty_idx, WN_field_id (tree)));
  WN*    wn;
  WN*    awn;
  WN*    swn;

  Is_True((WN_opcode(tree) == OPC_MMLDID),
	  ("expected mldid node, not %s", OPCODE_name(WN_opcode(tree))));

  pty_idx = Make_Pointer_Type (ty_idx, FALSE);

  swn = WN_CreateIntconst(OPC_U4INTCONST, size);
  awn = WN_CreateLda(OPR_LDA, Pointer_Mtype, MTYPE_V, WN_load_offset(tree),
		     pty_idx, WN_st(tree));
  wn  = WN_CreateMload(0, pty_idx, awn, swn);
  WN_set_field_id(wn, WN_field_id(tree));
  lower_copy_maps(tree, wn, actions);  // 15113: Preserve alias info
  wn  = lower_expr(block, wn, actions);

#ifdef TARG_ST
  // [TB] Improve line number.
  WN_copy_linenum(tree, wn);
#endif
  WN_Delete(tree);
  return wn;
}

/* ====================================================================
 *
 * WN *lower_miload(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on MILOAD nodes returning
 * an equivalent MLOAD node.
 *
 * ==================================================================== */

static WN *lower_miload(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TY_IDX ty_idx  = WN_field_id (tree) == 0 ?
    WN_ty(tree) :
    get_field_type (WN_ty (tree), WN_field_id (tree));
  TY_IDX pty_idx = WN_load_addr_ty(tree);
  UINT64 size    = TY_size(Ty_Table[ty_idx]);
  WN*    wn;
  WN*    swn;

  Is_True((WN_opcode(tree) == OPC_MMILOAD),
	  ("expected miload node, not %s", OPCODE_name(WN_opcode(tree))));

  swn = WN_CreateIntconst (OPC_U4INTCONST, size);
  wn  = WN_CreateMload (WN_offset(tree), pty_idx, WN_kid0(tree), swn);
  WN_set_field_id(wn, WN_field_id(tree));
  lower_copy_maps(tree, wn, actions);  // 15113: Preserve alias info
  wn  = lower_expr (block, wn, actions);

#ifdef TARG_ST
  // [TB] Improve line number.
  WN_copy_linenum(tree, wn);
#endif
  WN_Delete (tree);
  return wn;
}

#ifdef TARG_ST
/* ====================================================================
 *
 * lower_load_bitfield()
 *
 * lower bitfield load. BSILOAD/BSLDID
 *
 * ==================================================================== */
static WN*
lower_load_bitfield (WN* block, WN* wn, LOWER_ACTIONS actions)
{
  Is_True((WN_operator (wn) == OPR_ILOAD || WN_operator (wn) == OPR_LDID) &&
	  WN_desc(wn) == MTYPE_BS,
	  ("expected BSILOAD or BSLDID, not %s",
	   OPERATOR_name (WN_operator (wn))));
  
  lower_bit_field_id(wn);
  return lower_expr(block, wn, actions);
}
#endif

/* ====================================================================
 *
 * lower_load_bits
 *
 * lower LDBITS and ILDBITS into shifts
 *
 * ==================================================================== */
static WN*
lower_load_bits (WN* block, WN* wn, LOWER_ACTIONS actions)
{
  Is_True (WN_operator (wn) == OPR_LDBITS || WN_operator (wn) == OPR_ILDBITS,
	   ("expected LDBITS or ILDBITS, not %s",
	    OPERATOR_name(WN_operator(wn))));

  TYPE_ID rtype = WN_rtype (wn);
  TYPE_ID desc = WN_desc (wn);
  INT delta = MTYPE_bit_size(rtype) - MTYPE_bit_size(desc);
  if (delta < 0) {
    rtype = Mtype_TransferSize (desc, rtype);
    delta = 0;
  }

  WN* tree = wn;

  INT bit_size = WN_bit_size (wn);
  INT bit_ofst = !Target_Is_Little_Endian ?
    WN_bit_offset (wn) :
    MTYPE_bit_size(desc) - bit_size - WN_bit_offset (wn);
  BOOL bits_signed = MTYPE_signed(rtype);
  
  if (bit_ofst == 0)
    bit_size += delta;
  else {
    bit_ofst += delta;
    if (bits_signed)
      tree = WN_Shl (rtype, tree, WN_Intconst (MTYPE_I4, bit_ofst));
    else {
      INT shift_count = 64 - (MTYPE_bit_size(rtype) - bit_ofst);
      mUINT64 mask = (~(mUINT64)0) >> shift_count;
      tree = WN_Band (rtype, tree,
		      WN_Intconst (Mtype_TransferSign (MTYPE_U4, rtype),
				   mask));
      bit_size += bit_ofst;
    }
  }

  INT right_shift = MTYPE_bit_size(rtype) - bit_size;

  if (right_shift > 0) {
    OPERATOR opr = bits_signed ? OPR_ASHR : OPR_LSHR;
    tree = WN_Binary (opr, rtype, tree, WN_Intconst (MTYPE_I4, right_shift));
  }

  TYPE_ID orig_rtype = WN_rtype (wn);
  
  WN_set_rtype (wn, rtype);
  WN_set_bit_offset_size (wn, 0, 0);
  WN_set_operator (wn, WN_operator (wn) == OPR_LDBITS ? OPR_LDID : OPR_ILOAD);

  if (rtype != orig_rtype)
    tree = WN_Type_Conversion (tree, orig_rtype);
#ifdef TARG_ST
  // Call runtime lowering here as extraction may have generated U8 shifts
  tree = rt_lower_expr (tree, OnlyAction(actions, LOWER_RUNTIME));
  return lower_expr (block, tree, actions);
#else
  return lower_expr (block, tree, actions);
#endif
} // lower_load_bits

#ifdef TARG_ST
/* ====================================================================
 *
 * lower_store_bitfield()
 *
 * lower bitfield stores. BSISTORE/BSSTID 
 *
 * ==================================================================== */
static WN*
lower_store_bitfield (WN* block, WN* wn, LOWER_ACTIONS actions)
{
  Is_True((WN_operator (wn) == OPR_ISTORE || WN_operator (wn) == OPR_STID) &&
	  WN_desc(wn) == MTYPE_BS,
	  ("expected BSISTORE or BSSTID, not %s",
	   OPERATOR_name (WN_operator (wn))));
  
  lower_bit_field_id(wn);
  return lower_store(block, wn, actions);
}
#endif


/* ====================================================================
 *
 * lower_store_bits
 *
 * lower STBITS and ISTBITS
 *
 * ==================================================================== */
static WN*
lower_store_bits (WN* block, WN* wn, LOWER_ACTIONS actions)
{
  Is_True (WN_operator (wn) == OPR_STBITS || WN_operator (wn) == OPR_ISTBITS,
	   ("expected STBITS or ISTBITS, not %s",
	    OPERATOR_name (WN_operator (wn))));

  INT bit_size = WN_bit_size (wn);
  INT bit_ofst = WN_bit_offset (wn); 

  WN* orig_value;

  if (WN_operator (wn) == OPR_ISTBITS) {
    WN* load_address = lower_copy_tree (WN_kid1 (wn), actions);
#ifdef TARG_ST
    orig_value = WN_Iload (Mtype_TransferSign (MTYPE_U4, WN_desc (wn)),
			   WN_offset (wn), WN_object_ty (wn), load_address, 0);
#else
    orig_value = WN_Iload (Mtype_TransferSign (MTYPE_U4, WN_desc (wn)),
			   WN_offset (wn), WN_ty (wn), load_address, 0);
#endif
  } else 
    orig_value = WN_Ldid (Mtype_TransferSign (MTYPE_U4, WN_desc (wn)),
			  WN_offset (wn), WN_st_idx (wn), WN_ty (wn), 0);
  
  TYPE_ID cmp_type = WN_rtype (orig_value);
  
  INT shift = !Target_Is_Little_Endian ?
      MTYPE_bit_size (WN_desc (wn)) - bit_ofst - bit_size :
      bit_ofst;
#ifdef TARG_ST
  mUINT64 mask = ~((bit_size ? (~((mUINT64)0) >> (64 - bit_size)) : (mUINT64)0)
		   << shift);
#else
  mUINT64 mask = ~((~((mUINT64)0) >> (64 - bit_size)) << shift);
#endif
  orig_value = WN_Band (cmp_type, orig_value, WN_Intconst (cmp_type, mask));

  WN* new_value = WN_kid0 (wn);

  // check if we need to sign-extend the value
  if (bit_size > MTYPE_bit_size (WN_rtype (new_value)))
    new_value =
      WN_CreateCvtl (OPR_CVTL,
		     Mtype_TransferSign (WN_rtype (new_value), cmp_type),
		     WN_rtype (new_value),
		     MTYPE_bit_size (WN_rtype (new_value)),
		     new_value);
  
  // now, truncate to the right bit size for store
#ifdef TARG_ST
  mask = bit_size ? (~((mUINT64)0) >> (64 - bit_size)) : (mUINT64)0;
#else
  mask = ~((mUINT64)0) >> (64 - bit_size);
#endif
  new_value =
    WN_Band (cmp_type, new_value,
	     WN_Intconst (Mtype_TransferSize (WN_rtype (new_value), cmp_type),
			  mask));

  // move the bits to the right position
  if (shift > 0)
    new_value = WN_Shl (cmp_type, new_value, WN_Intconst (cmp_type, shift));

  // set the value with bitwise or
  new_value = WN_Bior (cmp_type, orig_value, new_value);

  WN_kid0 (wn) = new_value;
  WN_set_bit_offset_size (wn, 0, 0);
  WN_set_operator (wn, WN_operator(wn) == OPR_STBITS ? OPR_STID : OPR_ISTORE);
#ifdef TARG_ST
  // U8 shifts may require RT lowering
  rt_lower_stmt(wn, OnlyAction(actions, LOWER_RUNTIME));
#endif

  return lower_store (block, wn, actions);
} // lower_store_bits

#if 0
/* ====================================================================
 *
 * check_unaligned
 *
 * required_alignment is the natural alignment; offset is the actual offset
 * used in the current access; ty_align is the alignment in the TY of the
 * current access.  Return whether the access is unaligned.
 *
 * ==================================================================== */
static bool check_unaligned(INT required_alignment, INT offset, INT ty_align)
{
  if (required_alignment <= 1)
    return FALSE;
  INT align = ty_align;
  if (offset) {
    INT offset_align = offset % required_alignment;
    if (offset_align)
      align = MIN(align, offset_align);
  }
  return align < required_alignment;
}
#endif


// --------------------------------------------------------------------
// This function mimics FLD_get_to_field from common/com/symtab.cxx,
// but it also computes the offset of <field_id> within <struct_ty_idx>
// We need this because FLD_ofst only gives the offset within the first
// enclosing struct.
// --------------------------------------------------------------------
FLD_HANDLE
FLD_And_Offset_From_Field_Id (TY_IDX  struct_ty_idx, 
                              UINT    field_id, 
                              UINT&   cur_field_id,
                              UINT64& offset)
{
  FLD_ITER fld_iter = Make_fld_iter(TY_fld(struct_ty_idx));
  do {
    FLD_HANDLE fld(fld_iter);
    cur_field_id++;
    if (cur_field_id == field_id) {
      offset += FLD_ofst(fld);
      return fld;
    }
    if (TY_kind(FLD_type(fld)) == KIND_STRUCT &&
        TY_fld(FLD_type(fld)) != FLD_HANDLE()) {
      UINT64 nested_offset = offset + FLD_ofst(fld);
      fld = FLD_And_Offset_From_Field_Id(FLD_type(fld), field_id, 
                                         cur_field_id, nested_offset);
      if (cur_field_id == field_id) {
        offset = nested_offset;
        return fld;
      }
    }
  } while (!FLD_last_field(fld_iter++));
  
  return FLD_HANDLE();
} 

/* ====================================================================
 *
 * lower_bit_field_id
 *
 * The given LDID/STID/ILOAD/ISTORE node has desc type MTYPE_BS.  Lower
 * the node by changing the field_id to bit_offset/bit_size combination.
 * The desc field is changed to reflect the unit in memory to load, and
 * the offset field may need to be updated.
 *
 * ==================================================================== */
static void lower_bit_field_id(WN *wn)
{
  TY_IDX struct_ty_idx;
  TY_IDX ty_idx;
  TYPE_ID rtype;
  OPERATOR opr = WN_operator(wn);
  OPERATOR new_opr;
  BOOL indirect;
  if (opr == OPR_LDID || opr == OPR_STID) {
    ST_IDX st_idx = WN_st_idx(wn);
    struct_ty_idx = WN_ty(wn);
    new_opr = (opr == OPR_LDID) ? OPR_LDBITS : OPR_STBITS;
    indirect = FALSE;
  }
  else {
    if (WN_operator(wn) == OPR_ILOAD) {
      ty_idx = WN_load_addr_ty(wn);
      new_opr = OPR_ILDBITS;
    }
    else {	// ISTORE
      ty_idx = WN_ty(wn);
      new_opr = OPR_ISTBITS;
    }
    Is_True(TY_kind(ty_idx) == KIND_POINTER,
	    ("addr ty not pointer type for %s", OPERATOR_name(opr)));
    struct_ty_idx = TY_pointed(ty_idx);
    indirect = TRUE;
  }
  Is_True(TY_kind(struct_ty_idx) == KIND_STRUCT,
	  ("struct type not associated with bit-field access for %s", OPERATOR_name(opr)));
  UINT cur_field_id = 0;
  UINT64 field_offset = 0;
  FLD_HANDLE fld = FLD_And_Offset_From_Field_Id(struct_ty_idx, 
                                                WN_field_id(wn),
                                                cur_field_id,
                                                field_offset); 
  Is_True(! fld.Is_Null(),
	  ("invalid bit-field ID for %s", OPERATOR_name(opr)));
  TY_IDX fld_ty_idx = FLD_type(fld);
  
  Is_True(FLD_is_bit_field(fld),
	  ("non-bit-field associated with bit-field access for  %s", OPERATOR_name(opr)));

#ifndef TARG_ST
  // [CG]: Modification of wn type will occur later
  WN_set_ty (wn, (opr == OPR_ISTORE ?
		  Make_Pointer_Type (fld_ty_idx, FALSE) :
		  fld_ty_idx));
#endif


#ifdef TARG_ST
  // [CG] When converting struct type to type of the field,
  // we must propagate the volatile attribute
  if (WN_Is_Volatile_Mem(wn)) Set_TY_is_volatile(fld_ty_idx);
#endif

  // analyze bit field accessed
  UINT bytes_accessed = TY_size(fld_ty_idx);
  if (OPERATOR_is_store(new_opr))
    rtype = TY_mtype(fld_ty_idx);
  else rtype = WN_rtype(wn);
  INT ofst = field_offset;
#ifdef TARG_ST
  if (TY_is_packed(Ty_Table[struct_ty_idx])) {
    INT required_alignment = bytes_accessed * 8;
    INT align = TY_align(struct_ty_idx);
    if (ofst) {
      INT offset_align = ofst % required_alignment;
      if (offset_align)
	align = MIN(align, offset_align);
    }
    if (align < required_alignment) {
      Set_TY_align(fld_ty_idx,align);
    }
  }
#endif
#if 0
  BOOL unaligned_field = check_unaligned(bytes_accessed * 8, ofst,
					 TY_align(struct_ty_idx));
#endif
  if (ofst >= 0)
    ofst = ofst / bytes_accessed * bytes_accessed;
  else ofst =  (ofst - bytes_accessed + 1) / bytes_accessed * bytes_accessed;
  UINT bsize = FLD_bsize(fld);
  UINT bofst = FLD_bofst(fld) + (field_offset-ofst) * 8;
  if ((bofst + bsize) > (bytes_accessed * 8)) {
#ifdef TARG_X8664
    if (bytes_accessed == MTYPE_byte_size(MTYPE_I8)){ 
#else
    if (bytes_accessed == MTYPE_byte_size(Max_Int_Mtype)){ 
#endif // TARG_X8664
      // can't enlarge; reverse the adjustment
      ofst = field_offset;
      bofst = FLD_bofst(fld);
    }
    else bytes_accessed *= 2;
  }
  WN_load_offset(wn) = WN_load_offset(wn) + ofst;
  TYPE_ID mtype;

#ifdef TARG_ST
  // [CG]: Optimization of memory access must be disabled
  // when crossing the Max_Int_Mtype size as
  // it would necessitate a CVT operator.
  if ((bsize & 7) == 0 && 		   // field size multiple of bytes
#ifdef KEY // bug 11076
      bsize &&                             // field size non-zero
#endif
      (bytes_accessed * 8 % bsize) == 0 && // bytes_accessed multiple of bsize
      (bofst % bsize) == 0 &&		   // bofst multiple of bsize
      // avoid CVT generation
      !(MTYPE_bit_size(rtype) > MTYPE_bit_size(Max_Int_Mtype) &&
	bsize <= MTYPE_bit_size(Max_Int_Mtype)))
#else
  if ((bsize & 7) == 0 && 		   // field size multiple of bytes
#ifdef KEY // bug 11076
      bsize &&				   // field size non-zero
#endif
      (bytes_accessed * 8 % bsize) == 0 && // bytes_accessed multiple of bsize
#ifdef TARG_X8664
      (bofst & 7) == 0
#else
      (bofst % bsize) == 0		   // bofst multiple of bsize
#endif // TARG_X8664
      ) 
#endif
  {
    // bit-field operation not needed; leave operator as previous one
#ifdef TARG_ST
    // Replace hi level type by bit field type.
    WN_set_ty (wn, (opr == OPR_ISTORE ?
		    Make_Pointer_Type (fld_ty_idx, FALSE) :
		    fld_ty_idx));
    if (opr == OPR_ILOAD)
      WN_set_load_addr_ty (wn, Make_Pointer_Type (fld_ty_idx, FALSE));
#endif
    WN_set_field_id(wn, 0);
    mtype = Mtype_AlignmentClass(bsize >> 3, MTYPE_type_class(rtype));
    WN_set_desc(wn, mtype);
    WN_load_offset(wn) = WN_load_offset(wn) + (bofst >> 3);
  }
  else { // generate lowered-to bit-field operator
#if defined(TARG_MIPS) || defined(TARG_IA32)
    if ((indirect || WN_class(wn) != CLASS_PREG) && 
	bofst % 8 == 0 &&		// bit offset on byte boundary
	compute_offset_alignment(bytes_accessed*8, bofst) >= bsize) {
      // adjust bytes accessed to get rid of the left-shift
      WN_load_offset(wn) = WN_load_offset(wn) + (bofst >> 3);
      bytes_accessed = compute_offset_alignment(bytes_accessed, bofst >> 3);
      bofst = 0;
    }
#endif
#ifdef TARG_ST
    WN_set_operator(wn, new_opr);
    WN_set_ty (wn, (new_opr == OPR_ISTBITS ?
		    Make_Pointer_Type (fld_ty_idx, FALSE) :
		    fld_ty_idx));
    if (new_opr == OPR_ILDBITS)
      WN_set_load_addr_ty (wn, Make_Pointer_Type (fld_ty_idx, FALSE));
#else
    WN_set_operator(wn, new_opr);
#endif
    mtype = Mtype_AlignmentClass(bytes_accessed, MTYPE_type_class(rtype));
    Is_True( mtype != MTYPE_UNKNOWN, ("Unknown mtype encountered.") );
    WN_set_desc(wn, mtype);

    if (OPERATOR_is_load(new_opr) && 
	MTYPE_byte_size(WN_rtype(wn)) < bytes_accessed)
      WN_set_rtype(wn, WN_desc(wn));
    WN_set_bit_offset_size(wn, bofst, bsize);
  }

  // fix the TYs
  if (MTYPE_byte_size(mtype) > MTYPE_byte_size(TY_mtype(fld_ty_idx)))
    fld_ty_idx = MTYPE_To_TY(mtype);
  WN_set_ty (wn, (opr == OPR_ISTORE ?
		  Make_Pointer_Type (fld_ty_idx, FALSE) :
		  fld_ty_idx));
#ifdef KEY // bug 12394
  if (new_opr == OPR_ILDBITS)
    WN_set_load_addr_ty(wn, Make_Pointer_Type(fld_ty_idx));
#endif
}

static void lower_trapuv_alloca (WN *block, WN *tree, LOWER_ACTIONS actions
#ifdef KEY
				 , ST_IDX sym_idx, INT64 offset, TY_IDX pty_idx
				 )
#endif
{
	WN *size = WN_kid0(tree);
	WN *con;
#ifdef TARG_ST
	// 	Is_True(TY_kind(pty_idx) == KIND_POINTER,
	// 		("lower_trapuv_alloca: pointer type not found"));
	// [TB]: sometimes, here pty_idx is not of pointer type whereas
	// it should.  This seems to come from WOPT.  in opt_cse.cxx,
	// function Do_cse_pass_2, occur->Get_temp_cr(_worklist,
	// _etable->Htable()) creates a temp left hand side with a bad
	// type....  I don't know how to fix that. my understanding of
	// wopt is to weeak...  To fix that, I create a pointer type
	// from the type when the type is not a pointer.
	if (TY_kind(pty_idx) != KIND_POINTER)
	  pty_idx = Make_Pointer_Type(pty_idx, TRUE);

	TY_IDX ty_idx = TY_pointed(pty_idx);

	TYPE_ID type = TY_mtype(ty_idx);
	if (DEBUG_Zero_Uv)			// Initialize vars to 0.
	  con = WN_Intconst(MTYPE_byte_size(type)==4 ? MTYPE_U4 : MTYPE_U8, 0);
	else
	// Get 32/64-bit unitialized value from type.
	  con = WN_UVConst(type);
	if (con == NULL)
#else        
#ifdef KEY
	TY_IDX ty_idx;
	if (TY_kind(pty_idx) == KIND_POINTER)
	  ty_idx = TY_pointed(pty_idx);
	else if (TY_kind(pty_idx) == KIND_SCALAR &&
		 TY_size(pty_idx) == Pointer_Size) {
	  // No info on the pointed-to type.  Take a wild guess and make it U4.
	  // Bug 8282.
	  ty_idx = MTYPE_To_TY(MTYPE_U4);
	} else {
	  ty_idx = 0;
	  Is_True(FALSE, ("lower_trapuv_alloca: pointer type not found"));
	}
	TYPE_ID type = TY_mtype(ty_idx);
	if (DEBUG_Zero_Uv)			// Initialize vars to 0.
	  con = WN_Intconst(MTYPE_byte_size(type)==4 ? MTYPE_U4 : MTYPE_U8, 0);
	else if (MTYPE_byte_size(type) == 4)	// Get 32/64-bit NaN from type.
	  con = WN_UVConst(MTYPE_U4);
	else if (MTYPE_byte_size(type) == 8)
	  con = WN_UVConst(MTYPE_U8);
	else
#endif
#endif
	con = WN_UVConst(WN_rtype(size));

	WN *mstore;
	if (WN_operator(size) == OPR_INTCONST && WN_const_val(size) == 0)
		return;	// nothing to do

#ifdef KEY
	ST *sym = &St_Table[sym_idx];
	mstore = WN_CreateMstore(0,
		pty_idx,		// bug 6007
		con,
		WN_Ldid(Pointer_type, offset, sym, ST_type(sym)),
		WN_COPY_Tree(size) );
#else
	mstore = WN_CreateMstore(0,
		Make_Pointer_Type(MTYPE_To_TY(WN_rtype(size)),TRUE),
		con,
		WN_LdidPreg(Pointer_type, Stack_Pointer_Preg_Offset),
		WN_COPY_Tree(size) );
#endif
	WN_Set_Linenum(mstore, current_srcpos);  // Bug 1268
	mstore = lower_store (block, mstore, actions);
#ifdef TARG_ST
	// [TB] copy linenumber.
	WN_copy_linenum(tree, mstore);
#endif
	WN_INSERT_BlockLast(block, mstore);
}

/* ====================================================================
 * WHIRL Tree Queries
 * ====================================================================
 */

/*
 * Get_Intconst_Val()
 *
 * See interface description.
 */
INT64 
Get_Intconst_Val(WN *wn)
{
  WN *intconst = NULL;
  if (WN_operator(wn) == OPR_LDID &&
      WN_class(wn) == CLASS_PREG &&
      Preg_Home(WN_offset(wn)) != NULL &&
      WN_operator_is(Preg_Home(WN_offset(wn)), OPR_INTCONST))
    intconst = Preg_Home(WN_offset(wn));
  else if (WN_operator(wn) == OPR_INTCONST)
    intconst = wn;
  DevAssert(intconst != NULL, ("unexpected call to Get_Intconst_Val"));
  return WN_const_val(intconst);
}
#ifdef TARG_ST
BOOL
WN_Is_Emulated_Operator (OPERATOR opr, TYPE_ID rtype, TYPE_ID desc)
{
  // If floating point emulation is requested explicitly.
  if (Emulate_FloatingPoint_Ops && 
      (MTYPE_is_float(rtype) || MTYPE_is_float(desc)))
    return TRUE;
  
  // Tests to know if operator must be emulated is moved to betarget.cxx
  // in targinfo.
  return BETARG_is_emulated_operator(opr,rtype,desc);
}
#endif
/*
 * Is_Intconst_Val()
 *
 * See interface description.
 */
BOOL 
Is_Intconst_Val(WN *wn)
{
  WN *intconst = NULL;
  if (WN_operator(wn) == OPR_LDID &&
      WN_class(wn) == CLASS_PREG &&
      Preg_Home(WN_offset(wn)) != NULL &&
      WN_operator_is(Preg_Home(WN_offset(wn)), OPR_INTCONST))
    intconst = Preg_Home(WN_offset(wn));
  else if (WN_operator(wn) == OPR_INTCONST)
    intconst = wn;
  return intconst != NULL ? TRUE: FALSE;
}
#ifdef TARG_ST
static BOOL
Should_Call_Divide (WN *tree) 
{
  TYPE_ID rtype = WN_rtype(tree);
  if (Is_Intconst_Val(WN_kid1(tree))) {
    INT64 constval = Get_Intconst_Val(WN_kid1(tree));

    if (Can_Do_Fast_Divide(rtype, constval))  return FALSE;
    /*TDR : New optimisation for div by 3 5 6 7 9 10 11 12 13 */
    if (Target_Inlines_Divide(rtype, constval))  return FALSE;
  }

  /* In all other cases, we fall back to per operator check. */
  return WN_Is_Emulated_Operator(WN_operator(tree), rtype, WN_desc(tree));
}
#endif
#ifdef TARG_ST
static BOOL
Should_Call_Remainder (WN *tree) 
{
  TYPE_ID rtype = WN_rtype(tree);
  if (Is_Intconst_Val(WN_kid1(tree))) {
    INT64 constval = Get_Intconst_Val(WN_kid1(tree));
    
    if (Can_Do_Fast_Remainder(rtype, constval)) return FALSE;
#ifdef TARG_ST
    /* Allows target-specific easy way to handle remainders */
    /* (translated into asm sequence) */
    if (Target_Inlines_Remainder(rtype, constval))  return FALSE;
#endif
  }

  /* In all other cases, we fall back to per operator check. */
  return WN_Is_Emulated_Operator(WN_operator(tree), rtype, WN_desc(tree));
}
#endif
#ifdef TARG_ST
BOOL
WN_Is_Emulated(WN *tree)
{
  OPERATOR opr = WN_operator(tree);

  /* MOD/REM/DIV are treated specifically. */
  switch(opr) {
  case OPR_MOD: 
  case OPR_REM:
    return Should_Call_Remainder(tree);
  case OPR_DIV:
    return Should_Call_Divide(tree);
  default:
    break;
  }

  TYPE_ID res = WN_rtype(tree);
  TYPE_ID desc = WN_desc(tree);
  return WN_Is_Emulated_Operator(opr, res, desc);
}
BOOL
WN_Is_Emulated_Type (TYPE_ID type)
{
  /* This generic conditions are sufficient to decide
     when the type is emulated. */
  if (Only_32_Bit_Ops && MTYPE_is_longlong(type)) return TRUE;
  if (Emulate_Single_Float_Type && type == MTYPE_F4) return TRUE;
  if (Emulate_Double_Float_Type && MTYPE_is_double(type)) return TRUE;

  /* Otherwise call a target dependent function to find more emulated types. */
  return BETARG_is_emulated_type(type);
}

BOOL
WN_Madd_Allowed (TYPE_ID type)
{
  if (!Madd_Allowed) return FALSE;
  if (MTYPE_is_float(type) && (!Fused_FP || !Fused_Madd)) return FALSE;
						
  /* We check availability of all MADD operators there.
     It means that all of them must be enabled to activate
     MADD selection.
  */
  return BETARG_is_enabled_operator(OPR_MADD, type, type) &&
    BETARG_is_enabled_operator(OPR_MSUB, type, type) &&
    BETARG_is_enabled_operator(OPR_NMADD, type, type) &&
    BETARG_is_enabled_operator(OPR_NMSUB, type, type);
}
#endif
BOOL
WN_STBITS_Allowed (TYPE_ID type)
{
  /* For now we never allow STBITS to go through the middle end
     as it is not supported by WOPT. 
     At low level of optimization we could let it go through as
     it was done before, but I don't see benefit for this.
     The benefit of always returning false here is that the code
     selector does not have to implement it.
  */
  return FALSE;
  /* return BETARG_is_enabled_operator(OPR_STBITS, MTYPE_V, type); */
}

BOOL
WN_LDBITS_Allowed (TYPE_ID type)
{
  /* For the same reason as WN_STBITS_Allowed(), this function return always
     FALSE.
  */
  return FALSE;
  /* return BETARG_is_enabled_operator(OPR_LDBITS, type, type); */
}


#ifdef TARG_ST
/*
 * For Handling of LOWER_FAST_DIV/LOWER_CNST_DIV.
 */
static BOOL Is_Const_Divide(WN *wn)
{
  OPERATOR opr = WN_operator(wn);
  switch (opr) {
  case OPR_DIV:
  case OPR_REM:
  case OPR_MOD:
    if (Is_Intconst_Val(WN_kid1(wn))) {
      INT64 constval = Get_Intconst_Val(WN_kid1(wn));
      if (constval != 0) return TRUE;
    }
    break;
  }
  
  return FALSE;
}

#endif /* TARG_ST */

#ifdef TARG_ST
/* ====================================================================
 * Lowering of fast (power of 2) div/rem/mod.
 *
 * This falls under the Is_Fast_Divide condition.
 * It handles the cases:
 * - type is MTYPE_I4 or MTYPE_U4
 * - 1 (signed/unsigned)
 * - -1 (signed)
 * - power of two ([+-]2^n)
 *
 * This code has been moved to the target independent part as it is
 * quite common and there is no strong motivation to put it in the
 * target dependent part as it was done in open64-0.13
 *  ====================================================================
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

// Generate a sequence for division by a power of 2 without SELECT.
// From Hacker's delight p 155.
static WN *
lower_div_power_of_two_no_predication(WN *block, WN *tree)
{
  TYPE_ID type;
  BOOL is_signed;
  INT64 constval;
  WN *kid0, *kid1;
  INT pow2;
  WN *tmp1, *tmp2;
  LEAF tmp0;

  type = WN_rtype(tree);
  is_signed = MTYPE_is_signed(type) ? TRUE: FALSE;
  DevAssert(is_signed==TRUE,
            ("unexpected type in lower_div_power_of_two_no_predication"));

  kid0 = WN_kid0(tree);
  kid1 = WN_kid1(tree);
  constval = Get_Intconst_Val(kid1);
  pow2 = Get_Power_Of_2 (constval, type);

  // Add 2**k-1 if kid0<0. Add 0 otherwise.
  tmp0 = Make_Leaf(block, kid0, type);
  tmp1 = WN_Ashr(type, Load_Leaf(tmp0), WN_Intconst(type,pow2-1 ));
  tmp1 = WN_Lshr(type,            tmp1, WN_Intconst(type,32-pow2));
  tmp2 = WN_Add (type, Load_Leaf(tmp0), tmp1);

  // Final arithmetic right shift.
  tmp2 = WN_Ashr(type,            tmp2, WN_Intconst(type,pow2));
  
  if (constval < 0) {
    tmp2 = WN_Neg(type, tmp2);
  }
 
  return tmp2;
}

static WN *
lower_fast_div(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TYPE_ID type;
  BOOL is_signed;
  INT64 constval;
  WN *kid0, *kid1;
  WN *expr = NULL;

  DevAssert(Is_Fast_Divide(tree) && MTYPE_is_integral(WN_rtype(tree)),
	    ("unexpected call to lower_fast_div"));

  type = WN_rtype(tree);
  kid0 = WN_kid0(tree);
  kid1 = WN_kid1(tree);
  is_signed = MTYPE_is_signed(type) ? TRUE: FALSE;
  constval = Get_Intconst_Val(kid1);
  
  if (constval == 1) {
    return kid0;
  } else if (is_signed && constval == -1) {
    expr = WN_Neg(type, kid0);
  } else if (Is_Power_Of_2(constval, type)) {
    INT pow2 = Get_Power_Of_2 (constval, type);
    if (!is_signed) {
      expr = WN_Lshr(type, kid0, WN_Intconst(type, pow2));
    } else {
      INT64 absdvsr = constval < 0 ? -constval : constval;
      if (absdvsr == 2) {
	/* Optimize for abs(divisor) == 2:
	   If abs(divisor) == 2 we have the equivalence:
	   divisor < 0 ? divisor + abs(divisor)-1: divisor == 
	     divisor + bit(divisor, signbit)
	   Thus we can write:
	   expr = (divisor + bit(divisor, signbit)) >> 1
	   if (divisor < 0) expr  = -expr 
	*/
	INT signbit = MTYPE_bit_size(type)-1;
	LEAF tmp0 = Make_Leaf(block, kid0, type);
	expr = WN_Ashr(type,
		       WN_Add(type,
			      WN_Lshr(type, 
				      Load_Leaf(tmp0), 
				      WN_Intconst(type, signbit)),
			      Load_Leaf(tmp0)),
		       WN_Intconst(type, 1));
	if (constval < 0) {
	  expr = WN_Neg(type, expr);
	}
      } else {
        if(Expand_Power_Of_Two_Div_Without_Pred(type,constval)) {
          expr = lower_div_power_of_two_no_predication(block, tree);
        } else {
	/* Optimize for abs(divisor) != 2:
	   if (kid0 < 0)  expr = (kid0 + abs(divisor)-1)
	   else           expr = kid0
	   expr = expr >> pow2(abs(divisor))
	   if (divisor < 0) expr = - expr
	*/
	LEAF tmp0 = Make_Leaf(block, kid0, type);
	WN *expr0 = WN_Add(type, 
			   Load_Leaf(tmp0), 
			   WN_Intconst(type, absdvsr-1));
	WN *expr1 = WN_Select(type, 
			      WN_LT(type, 
				    Load_Leaf(tmp0), WN_Intconst(type, 0)),
			      expr0,
			      Load_Leaf(tmp0));
	expr = WN_Ashr(type, expr1, WN_Intconst(type, pow2));
	if (constval < 0) {
	  expr = WN_Neg(type, expr);
         }
	}
      }
    }
  } 
  FmtAssert(expr != NULL, ("Unexpected non power of 2 while lowering fast divide: %lld", constval));
  return lower_expr(block, expr, actions);
}

static WN *
lower_fast_rem(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TYPE_ID type;
  BOOL is_signed;
  INT64 constval;
  WN *kid0, *kid1;
  WN *expr = NULL;

  DevAssert(Is_Fast_Divide(tree) && MTYPE_is_integral(WN_rtype(tree)),
	    ("unexpected call to lower_fast_rem"));

  type = WN_rtype(tree);
  kid0 = WN_kid0(tree);
  kid1 = WN_kid1(tree);
  is_signed = MTYPE_is_signed(type) ? TRUE: FALSE;
  constval = Get_Intconst_Val(kid1);
  
  if (constval == 1 ||
      (is_signed && constval == -1)) {
    return WN_Intconst(type, 0);
  } else if (Is_Power_Of_2(constval, type)) {
    INT pow2 = Get_Power_Of_2 (constval, type);
    INT64 mask = (1LL << pow2) - 1;
    if (!is_signed) {
      expr = WN_Band(type, kid0, WN_Intconst(type, mask));
    } else {
      /*
       * We use the properties:
       *  rem(x,y) =  rem( x,-y)
       *  rem(x,y) = -rem(-x, y)
       * Thus:
       * if (kid0 < 0) expr = - (-kid0 & mask)
       * else 	expr = kid0 & mask
       */
      INT signbit = MTYPE_bit_size(type)-1;
      LEAF tmp0 = Make_Leaf(block, kid0, type);
      expr = WN_Select(type,
		       WN_LT(type, 
			     Load_Leaf(tmp0), 
			     WN_Intconst(type, 0)),
		       WN_Neg(type, WN_Band(type,
					    WN_Neg(type,Load_Leaf(tmp0)),
					    WN_Intconst(type, mask))),
		       WN_Band(type, Load_Leaf(tmp0), WN_Intconst(type, mask)));
    }
  }
  FmtAssert(expr != NULL, ("Unexpected non power of 2 while lowering fast remainder"));
  return lower_expr(block, expr, actions);
}

#endif /* TARG_ST */

#ifdef TARG_ST
// [SC] Support for nested function trampolines
struct is_tramp_sym {
private:
  const char *tramp_name;
  int len;
public:
  is_tramp_sym(const char *nm) : tramp_name(nm) { len = strlen(tramp_name); }
  BOOL operator()(UINT32, const ST *st) const {
      return strncmp(tramp_name, ST_name (st), len) == 0;
  }
};

struct fix_preg_home
{
private:
  ST *sym;
  ST *tramp_sym;
public:
  fix_preg_home(ST *s, ST *ts): sym(s), tramp_sym(ts) {}
  void operator() (UINT32 idx, const PREG *preg) const {
    WN *home = Preg_Home (Last_Dedicated_Preg_Offset + idx);
    if (home != NULL
	&& WN_operator (home) == OPR_LDA
	&& WN_st (home) == sym) {
      WN *new_home = WN_Lda (WN_rtype (home), WN_load_offset (home),
			     tramp_sym);
      Set_Preg_Home (Last_Dedicated_Preg_Offset + idx, new_home);
    }
  }
};
    
static  ST *
find_nested_function_trampoline (ST *sym)
{
  ST *tramp_sym;
  const char *sym_name = ST_name (sym);
  char *tramp_name = (char *) alloca (strlen (sym_name) + 8);
  sprintf (tramp_name, ".tramp.%s", sym_name);
  ST_IDX st_idx = For_all_until (St_Table, CURRENT_SYMTAB,
				 is_tramp_sym (tramp_name));
  if (st_idx != 0)
  {
    tramp_sym = &St_Table[st_idx];
  }
  else
  {
    TYPE_ID Nested_fn_tramp_type = Make_Array_Type (MTYPE_U8, 1, 5);
    tramp_sym = Gen_Temp_Symbol (Nested_fn_tramp_type, tramp_name);
    // There may be PREGs which have sym as their home location,
    // change these to have tramp_sym as the home location.
    //    forall preg idx
    For_all (*Scope_tab[CURRENT_SYMTAB].preg_tab, fix_preg_home (sym, tramp_sym));
  }
  return tramp_sym;
}
#endif /* TARG_ST */


inline BOOL Should_Call_Divide(TYPE_ID rtype)
{
#ifdef TARG_X8664
  if( Is_Target_32bit() &&
      ( rtype == MTYPE_I8 || rtype == MTYPE_U8 ) ){
    return TRUE;
  }
#endif // TARG_X8664

#if defined(TARG_IA64)
  if (!OPT_Inline_Divide) {
    if (   rtype == MTYPE_I8 || rtype == MTYPE_U8
	|| rtype == MTYPE_I4 || rtype == MTYPE_U4
	|| rtype == MTYPE_F4 || rtype == MTYPE_F8) return TRUE;
  }
#elif defined(EMULATE_LONGLONG)
  if (rtype == MTYPE_I8 || rtype == MTYPE_U8) return TRUE;
#endif
  return FALSE;
}

#ifdef TARG_ST
static BOOL Is_Fast_Divide(WN *wn)
{
  if (Is_Const_Divide(wn)) {
    INT64 constval = Get_Intconst_Val(WN_kid1(wn));
    TYPE_ID rtype = WN_rtype(wn);
    OPERATOR opr = WN_operator(wn);
    if (opr == OPR_DIV && Can_Do_Fast_Divide(rtype, constval))
      return TRUE;
    if (opr == OPR_REM && Can_Do_Fast_Remainder(rtype, constval))
      return TRUE;
  }
  return FALSE;
}
#else
static BOOL Is_Fast_Divide(WN *wn)
{
  OPERATOR opr = WN_operator(wn);
  switch (opr) {
  case OPR_DIV:
  case OPR_REM:
  case OPR_MOD:
    {
      TYPE_ID rtype = WN_rtype(wn);
      if(Is_Target_32bit() && (rtype == MTYPE_I8 || rtype == MTYPE_U8)) {
          // 64bit contants are not rematerialized in 32bit mode
          return FALSE;
      }

      if (WN_operator_is(WN_kid1(wn), OPR_INTCONST)) {
	TYPE_ID rtype = OPCODE_rtype(WN_opcode(wn));
	INT64 constval = WN_const_val(WN_kid1(wn));

	return   opr == OPR_DIV
	       ? Can_Do_Fast_Divide(rtype, constval)
	       : Can_Do_Fast_Remainder(rtype, constval);
      }
    }
    break;
  }

  return FALSE;
}
#endif

#ifdef KEY
/* Convert stmt
      a = a / b
   into
      if( b == N1 ){
         a = a / N1;
      } else if( b == ... ){
         ...;
      } else {
         a = a / b;
      }
   via feedback information.
*/
static WN* simp_remdiv( WN* block, WN* tree,  LOWER_ACTIONS actions  )
{
  if( Cur_PU_Feedback == NULL )
    return NULL;

  if( MTYPE_is_float( OPCODE_rtype(WN_opcode(tree) ) ) )
    return NULL;

  const FB_Info_Value& info = Cur_PU_Feedback->Query_value( tree );

  if( info.num_values == 0 )
    return NULL;

  if( info.exe_counter < (float)Div_Exe_Counter )
    return NULL;

  /* Sum up the freq of the top <Div_Exe_Candidates> entries. And perform div
     conversion only if the total freq is not less than <Div_Exe_Ratio>.
  */
  int items = 0;
  const float cutoff_ratio = (float)Div_Exe_Ratio / 100;
  const int num_candidates = MIN( info.num_values, Div_Exe_Candidates );
  FB_FREQ freq(0.0);

  for( items = 1; items <= num_candidates; items++ ){
    freq += info.freq[items-1];
    if( freq / info.exe_counter >= cutoff_ratio )
      break;
  }

  if( freq / info.exe_counter < cutoff_ratio )
    return NULL;

  const TYPE_ID type = WN_rtype( WN_kid(tree,1) );
  const PREG_NUM result = Create_Preg( type, ".remdiv_value" );

  for( int entry = 0; entry < items; entry++ ){
    WN* div_tree = NULL;

    WN* if_then = WN_CreateBlock();
    {
      WN* value = WN_CreateIntconst( OPCODE_make_op(OPR_INTCONST, type, MTYPE_V),
				     info.value[entry] );
      WN* tmp = WN_Binary( WN_operator(tree), type,
			   WN_COPY_Tree( WN_kid(tree,0) ),
			   value );
      WN* stid = WN_StidIntoPreg( type, result, MTYPE_To_PREG( type ), tmp );
      WN_Set_Linenum(stid, current_srcpos);
      WN_INSERT_BlockLast( if_then, stid );
    }
	  
    WN* if_else = WN_CreateBlock();
    {
      div_tree = WN_Binary( WN_operator(tree), type,
			    WN_COPY_Tree( WN_kid(tree,0) ),
			    WN_COPY_Tree( WN_kid(tree,1) ) );
      if( entry+1 == items ){
	WN* stid = WN_StidIntoPreg( type, result, MTYPE_To_PREG( type ), div_tree );
	WN_Set_Linenum(stid, current_srcpos);
	WN_INSERT_BlockLast( if_else, stid );
      }
    }

    WN* value = WN_CreateIntconst( OPCODE_make_op(OPR_INTCONST, type, MTYPE_V),
				   info.value[entry] );
    WN* if_tree = WN_CreateIf( WN_EQ( type, WN_COPY_Tree(WN_kid(tree,1)),
				      value ),
			       if_then, if_else );

    Cur_PU_Feedback->Annot( if_tree, FB_EDGE_BRANCH_TAKEN, info.freq[entry] );
    Cur_PU_Feedback->Annot( if_tree, FB_EDGE_BRANCH_NOT_TAKEN,
			    ( info.exe_counter - info.freq[entry] ) );

    WN_Set_Linenum(if_tree, current_srcpos);
    WN_INSERT_BlockLast( block, if_tree );
	  
    WN_Delete( WN_kid(tree,0) );
    WN_Delete( WN_kid(tree,1) );
    WN_Delete( tree );

    tree = div_tree;
    block = if_else;
  }
      
  return WN_LdidPreg( type, result );
}
#endif
static BOOL Is_Const_Float(WN *wn)
{
  if (WN_operator(wn) == OPR_LDID &&
      WN_class(wn) == CLASS_PREG &&
      Preg_Home(WN_offset(wn)) != NULL &&
      WN_operator_is(Preg_Home(WN_offset(wn)), OPR_CONST)) {
    return TRUE;
  }
  if (WN_operator_is(wn, OPR_INTCONST)) {
    return TRUE;
  }

  return FALSE;
}


/* ====================================================================
 *
 * WN *lower_expr(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on expression <tree>,
 * returning lowered expression tree.
 *
 * ==================================================================== */

#ifdef TARG_ST
WN *
lower_expr(WN *block, WN *tree, LOWER_ACTIONS actions)
#else
static WN *lower_expr(WN *block, WN *tree, LOWER_ACTIONS actions)
#endif
{
  BOOL kids_lowered = FALSE;	/* becomes TRUE when kids are lowered */
  BOOL intrinsic_lowered;

  TYPE_ID type = WN_rtype(tree);

  Is_True(OPCODE_is_expression(WN_opcode(tree)),
	  ("expected expression node, not %s", OPCODE_name(WN_opcode(tree))));

  if (OPCODE_is_load(WN_opcode(tree)))
    lower_map(tree, actions);

 /*
  *  before children are processed reassociate for madd oportunities
  */
  if ( Action(LOWER_MADD)	&&
#ifdef TARG_ST
      // [CG]: This test is not at the right place. Moved to lower_madd() and lower_nary_madd()
      1
#else
      Madd_Allowed		&&
#ifndef TARG_MIPS
     (MTYPE_id(type) == MTYPE_F4 || MTYPE_id(type) == MTYPE_F8)
#else
     (MTYPE_id(type) == MTYPE_F4 || MTYPE_id(type) == MTYPE_F8 ||
      MTYPE_id(type) == MTYPE_V8F4)
#endif
#endif
         )
  {
    tree = lower_nary_madd(block, tree, actions);
    tree = lower_madd(block, tree, actions);
  }
  if (Action(LOWER_TREEHEIGHT)	&&
      WN_is_commutative(tree))
  {
      // [HK] here, use Reassoc_Level instead of Roundoff_Level as it is specific to FP
#ifdef TARG_ST
    if (MTYPE_is_integral(type) || Reassoc_Level >= REASSOC_ASSOC)
#else
    if (MTYPE_is_integral(type) || Roundoff_Level >= ROUNDOFF_ASSOC)
#endif
      tree = lower_tree_height(block, tree, actions);
  }

 /* Note: We must split constant offsets after lowering complex exprs
  * and splitting symbol addresses since these may create new offsets
  * that need to be split.
  */
  switch (WN_operator(tree))
  {
  case OPR_INTRINSIC_OP:
    if (INTRN_is_nary(WN_intrinsic(tree)))
      break;

#ifndef TARG_MIPS
    if (INTRN_cg_intrinsic(WN_intrinsic(tree)))
      break;
#else
    if (INTRN_cg_intrinsic(WN_intrinsic(tree)) &&
	(WN_intrinsic(tree) < INTRN_ISGREATER ||
	 WN_intrinsic(tree) > INTRN_ISUNORDERED ||
	 ! Action(LOWER_QUAD) ||
	 ! MTYPE_is_quad(WN_rtype(WN_kid0(tree)))))
      break;
#endif

#ifdef TARG_X8664
    if( ( (INTRINSIC)WN_intrinsic(tree) == INTRN_F8ANINT ) &&
	Is_Target_SSE2() ){
      /* The code implemented in em_nearest_aint will introduce conversions
	 between long long and float, which will slow down the performance
	 under -m32.
      */
      if( Is_Target_32bit() )
	break;

      /* cg can generate a more accurate and faster version when
	 -OPT:fast_dnint is ON.
	 Expand the fast version as late as possible to avoid being
	 screwed up by cfold reassociation.
      */
      if( Fast_ANINT_Allowed )
        break;
    }

    if((INTRINSIC)WN_intrinsic(tree) == INTRN_POPCOUNT && Is_Target_SSE4_2()) {
      // popcount intrinsic can be expanded into popcnt instruction
      // if sse4.2 is enabled
      break;
    }
#endif

#if defined( KEY) && !defined(TARG_ST)
    if ( (INTRINSIC) WN_intrinsic (tree) == INTRN_CONSTANT_P)
    {
	WN * old = tree;
	WN * parm = WN_kid0 (WN_kid0 (old)); // child of OPR_PARM

	// Check if the argument is a compile-time constant, replace the
	// intrn op by true/false
	if (WN_operator (parm) == OPR_INTCONST ||
	    (OPCODE_has_sym (WN_opcode (parm)) && 
	     ST_class (WN_st (parm)) == CLASS_CONST)) {
	  // Simplify if it is already a constant
	  tree = WN_Intconst (MTYPE_I4, 1);
	  WN_Delete (old);
	  kids_lowered = TRUE;
	  break;
	}
	else if (Action (LOWER_TO_CG)) {
	  // Still not a constant, replace it with false
	  tree = WN_Intconst (MTYPE_I4, 0);
	  WN_Delete (old);
	  kids_lowered = TRUE;
	  break;
	}
    }
#endif
#ifdef TARG_ST
    //
    // Arthur: if we're not inlining intrinsics, do nothing - it will
    //         be converted into a function call at LOWER_TO_CG time.
    //
    if ((Inline_Intrinsics_Allowed  ||
         !INTRN_runtime_exists(WN_intrinsic(tree))) &&
        INTRN_cg_intrinsic(WN_intrinsic(tree))) {
      // Arthur: we may need to HILO lower parameters for cg_intrinsics
      //         When the cg-intrinsic returns not a HILO type, but may
      //         require HILO parameters, such intrinsic only returns 
      //         loexp.
      if (Action(LOWER_HILO)) {
	WN *hiexp, *loexp;
	lower_hilo_expr(block, 
			tree, 
			actions, 
			&hiexp, 
			&loexp 
			/* WN_desc(tree) */);

	tree = loexp;
	kids_lowered = TRUE;
      }

      break;
    }
#endif


#ifdef TARG_ST
    if (Enable_LAI)
      break;
#endif
    
    if (INTRN_is_actual(WN_intrinsic(tree)))
    {
      if (Action(LOWER_INTRINSIC))
      {
	tree = lower_intrinsic(block, tree, actions);
        kids_lowered = TRUE;
      }
      break;
    }
    if (Action(LOWER_INTRINSIC) ||
	Action(LOWER_INLINE_INTRINSIC) ||
#ifndef TARG_ST
#ifdef KEY // bug 6938
	Action(LOWER_FAST_EXP) ||
#endif
#endif
	Action(LOWER_INL_STACK_INTRINSIC))
    {
      tree = lower_emulation(block, tree, actions, intrinsic_lowered);
      kids_lowered = TRUE;
    }
    break;

  case OPR_ARRAY:
    if (Action(LOWER_ARRAY))
    {
      tree = lower_linearize_array_addr(block, tree, actions);
      kids_lowered = TRUE;
    }
    break;

  case OPR_ADD:
    if (Action(LOWER_SPLIT_CONST_OFFSETS))
    {
     /*
      * Split
      *       LDA   (c1) <sym>
      *       CONST (c2)
      *     ADD
      * Into
      *       LDA   (hi) <sym>
      *       CONST (low)
      *     ADD
      */
      WN *lda = WN_kid0(tree);
      WN *con = WN_kid1(tree);

      if (WN_operator_is(con, OPR_INTCONST) &&
	  foldLdaOffset(lda, WN_const_val(con)))
      {
        WN_OFFSET  offset = WN_lda_offset(lda) + WN_const_val(con);

	if (mem_offset_must_be_split(offset))
	{
	  WN_lda_offset(lda) = mem_offset_hi(offset);
	  WN_const_val(con) =  mem_offset_lo(offset);
	}
      }
    }
    else
    {
     /*
      * Fold
      *       LDA   (c1) <sym>
      *       CONST (c2)
      *     ADD
      * Into
      *     LDA (c1+c2) <sym>
      */
      WN *lda = WN_kid0(tree);
      WN *con = WN_kid1(tree);

      if (WN_operator_is(con, OPR_INTCONST) &&
	  foldLdaOffset(lda, WN_const_val(con)))
      {
	WN_lda_offset(lda) += WN_const_val(con);
	WN_Delete(tree);
	WN_Delete(con);
	tree = lower_expr(block, lda, actions);;
      }
    }
    break;

  case OPR_MLOAD:
    if (Align_Object)
    {
      WN_kid0(tree)=	lower_expr(block, WN_kid0(tree), actions);
      kids_lowered = 	TRUE;

      tree = improve_Malignment(tree, WN_kid0(tree), WN_kid1(tree),
				WN_load_offset(tree));
      break;
    }  /* fall thru */

  case OPR_ILOAD:
    if (Action(LOWER_MLDID_MSTID) && WN_opcode(tree) == OPC_MMILOAD)
      return lower_miload(block, tree, actions);

    if (Action(LOWER_BIT_FIELD_ID) && WN_desc(tree) == MTYPE_BS) {
#ifdef TARG_ST
       // Performs full lowering of bitfield load.
       return lower_load_bitfield(block, tree, actions);
#else
      lower_bit_field_id(tree);
      if (Action(LOWER_BITS_OP) && WN_operator(tree) == OPR_ILDBITS){
	return lower_load_bits (block, tree, actions);
      }
#endif
    }

    if (Action(LOWER_SPLIT_CONST_OFFSETS))
    {
     /*
      * Convert
      *      EXPR
      *    ILOAD (offset>16bits)
      * Into
      *        CONST (hi)
      *        EXPR
      *      ADD
      *    ILOAD (lo)
      */
      WN_OFFSET  offset = WN_load_offset(tree);
      if (mem_offset_must_be_split(offset))
      {
	WN_kid0(tree) =  WN_Add(Pointer_type,
				WN_kid0(tree),
				WN_Intconst(Pointer_type,
					    mem_offset_hi(offset)));
	WN_load_offset(tree) = mem_offset_lo(offset);
      }
    }
    else
    {
     /*
      * Fold
      *       LDA (c1) <sym>
      *     ILOAD (c2)
      * Into
      *       LDA (0)  <sym>
      *     ILOAD (c1+c2)
      */
      WN *kid = WN_kid0(tree);

#ifdef TARG_MIPS
      /* We don't want this for MIPS since (c1+c2) could likely be
         out-of-range for the ILOAD, and we have the global
         canonicalization optimization to assign c1 and c2
         appropriately. */
      if (FALSE)
#else
      if (foldLdaOffset(kid, WN_load_offset(tree)))
#endif
      {
	WN_load_offset(tree) += WN_lda_offset(kid);
	WN_lda_offset(kid) = 0;
      }
#ifndef TARG_MIPS
     /*
      * Fold
      *           EXPR 
      *           CONST (c1)
      *       ADD expr
      *     ILOAD (c1)
      * Into
      *       EXPR
      *     ILOAD (c1+c2)
      */
      else if (WN_operator_is(kid, OPR_ADD) &&
	       foldConstOffset(WN_kid1(kid), WN_load_offset(tree)))
      {
	WN_load_offset(tree) += WN_const_val(WN_kid1(kid));
	WN_kid0(tree) = WN_kid0(kid);
	WN_Delete(WN_kid1(kid));
	WN_Delete(kid);
      }
#endif
    }
    break;

  case OPR_LDID:
    if (Action(LOWER_RETURN_VAL) && WN_st(tree) == Return_Val_Preg)
      return lower_return_ldid(block, tree, actions);

    if (Action(LOWER_MLDID_MSTID) && WN_opcode(tree) == OPC_MMLDID)
      return lower_mldid(block, tree, actions);

    if (Action(LOWER_BIT_FIELD_ID) && WN_desc(tree) == MTYPE_BS) {
#ifdef TARG_ST
       // Performs full lowering of bitfield load.
      return lower_load_bitfield(block, tree, actions);
#else
      lower_bit_field_id(tree);
      if (Action(LOWER_BITS_OP) && WN_operator(tree) == OPR_LDBITS){
	return lower_load_bits (block, tree, actions);
      }
#endif
    }

    if ((WN_class(tree) == CLASS_CONST)	&& (WN_load_offset(tree) == 0))
    {
      TCON	val = WN_val(tree);
      TYPE_ID	valType = WN_val_type(tree);
      WN	*con;

      if (MTYPE_is_integral(type) && MTYPE_is_integral(valType))
      {
	con = WN_Intconst(type, Targ_To_Host( val));
        WN_Delete(tree);
        return con;
      }
     /*
      *  check for real (complex constant) conversion not handled by Targ_Conv
      */
      else if ((MTYPE_is_float(type) && MTYPE_is_float(valType))	&&
	      !(!MTYPE_is_complex(type) && MTYPE_is_complex(valType)))
      {
	if (type != valType)
	{
	  val = Targ_Conv(type, val);
	}
	con =	Make_Const(val);
	WN_Delete(tree);
	return con;
      }
    }
    {
      PREG_NUM last_preg = Get_Preg_Num(PREG_Table_Size(CURRENT_SYMTAB));
      if ((WN_class(tree) == CLASS_PREG) &&
	  (WN_load_offset(tree) > last_preg))
      {
	  DevWarn("lower_expr() pregno %d > SYMTAB_last_preg(%d)",
		  WN_load_offset(tree), last_preg);
      }
    }
   /*
    * Exposes the LDA for RVI usage
    */
    if (Action(LOWER_SPLIT_SYM_ADDRS))
    {
      WN	*iload;
      iload = lower_split_sym_addrs(tree, WN_load_offset(tree), actions);
      if (iload)
      {
	return lower_expr(block, iload, actions);
      }
    }
    if ( Action(LOWER_FORMAL_REF) && WN_class(tree) == CLASS_VAR)
    {
      WN	   *iload;

      iload = lower_formal_ref(tree, WN_load_offset(tree),
			       WN_st(tree), actions);
      if (iload)
      {
        return lower_expr(block, iload, actions);
      }
    }
    if ( Action(LOWER_UPLEVEL))
    {
      ST *sym = WN_st(tree);

      if (ST_is_uplevelTemp(sym))
      {
        WN	   *iload;

        iload = lower_uplevel_reference(tree, WN_load_offset(tree), actions);
	tree = lower_expr(block, iload, actions);
	return tree;
      }
    }
    break;

  case OPR_ILDBITS:
  case OPR_LDBITS:
#ifdef TARG_ST
    if (Action(LOWER_BITS_OP))
      return lower_load_bits (block, tree, actions);
#else
    if (Action(LOWER_BITS_OP))
      return lower_load_bits (block, tree, actions);
#endif
    break;
    
  case OPR_LDA:
   /*
    *  use of LDA should mark STFL_ADDR_USED_LOCALLY
    */
    {
      ST *sym = WN_st(tree);

      // if ((ST_class(sym) == CLASS_VAR) ||
      //     (ST_class(sym) == CLASS_FUNC)) {
      //   Do nothing here. ADDR flags should only grow more
      //   optimistic; they should never become more conservative,
      //   because the program's semantics cannot grow worse as we
      //   compile it.
      // }

      if (ST_class(sym) == CLASS_BLOCK && STB_merge(sym))
      {
	DevWarn("LDA (%s) potential bad exposed use of a mergeable symbol",
		ST_name(sym));
      }
    }
    if (Action(LOWER_SPLIT_SYM_ADDRS))
    {
      WN  *lda;
      lda =	lower_split_sym_addrs(tree, WN_lda_offset(tree), actions);
      if (lda)
      {
	return lower_expr(block, lda, actions);
      }
    }
    if ( Action(LOWER_FORMAL_REF) && WN_class(tree) == CLASS_VAR)
    {
      WN	   *ldid;

      ldid =	lower_formal_ref(tree, WN_lda_offset(tree), WN_st(tree),
				 actions);
      if (ldid)
      {
	return lower_expr(block, ldid, actions);
      }
    }
    if ( Action(LOWER_UPLEVEL))
    {
      ST *sym = WN_st(tree);

#ifdef TARG_ST
    // [SC] Lower nested function pointers.
      if (Action(LOWER_NESTED_FN_PTRS)
	  && ST_sym_class (sym) == CLASS_FUNC
	  && PU_is_nested_func (Pu_Table[ST_pu(sym)]))
      {
	ST *tramp_sym = find_nested_function_trampoline (sym);
	ST *init_tramp_sym = Gen_Intrinsic_Function(
			       Make_Function_Type( MTYPE_To_TY (MTYPE_V)),
			       "__init_trampoline");
	// [CG]: Disable instrumentation for this function
	Set_PU_no_instrument_function(Pu_Table[ST_pu(init_tramp_sym)]);

	WN *link;
	if (PU_lexical_level(&St_Table[WN_st_idx(current_function)]) <
	    PU_lexical_level(sym))
	{
	    link = WN_LdidPreg(Pointer_type, Frame_Pointer_Preg_Offset);
	}
	else
	{
	    ST *slink = Find_Slink_For_Scope(WN_st(current_function), sym);
	    link = WN_Ldid (Pointer_type, 0, slink, ST_type(slink));
	}
	WN *call, *callblock;

	call = WN_Call (MTYPE_V, MTYPE_V, 3, init_tramp_sym);
	WN_actual(call, 0) = WN_Lda (Pointer_type, 0, tramp_sym);
	WN_actual(call, 1) = WN_CopyNode(tree);
	WN_actual(call, 2) = link;
	WN_call_flag(call) = 0; // Not pure, not NSE
	WN_annotate_call_flags(call, init_tramp_sym);
	LOWER_ACTIONS new_actions = actions & ~LOWER_NESTED_FN_PTRS;
	if (Action(LOWER_TO_CG))
	  new_actions |= LOWER_CALL;

	callblock = WN_CreateBlock();
	WN_INSERT_BlockLast (callblock, call);
	callblock = lower_block (callblock, new_actions);

	WN_INSERT_BlockLast (block, callblock);

	tree = WN_Lda (Pointer_type, 0, tramp_sym);
      }
#endif
      if (ST_is_uplevelTemp(sym))
      {
        WN	   *ldid;

        ldid = lower_uplevel_reference(tree, WN_lda_offset(tree), actions);
	tree = lower_expr(block, ldid, actions);
	return tree;
      }
    }
#ifdef KEY // if taking address of a nested function, replace it by the address
    // of the corresopnding trampoline
    if (Action(LOWER_UPLEVEL) && WN_class(tree) == CLASS_FUNC) {
      ST *sym = WN_st(tree);
      PU &pu = Pu_Table[ST_pu(sym)];
      if (PU_need_trampoline(pu)) {
	Allocate_Object(sym); // because it may not appear again
	ST_IDX tramp_st_idx = find_trampoline(ST_st_idx(sym));
        WN_st_idx(tree) = tramp_st_idx;
	return lower_expr(block, tree, actions | LOWER_UPLEVEL);
      }
    }
#endif
    break;

  case OPR_CVT:
  case OPR_TRUNC:
  case OPR_RND:
    if (Action(LOWER_CVT))
    {
      tree = lower_cvt(block, tree, actions);
      kids_lowered = TRUE;
    }
#ifdef TARG_ST
    //
    // If a double type is being converted to single, we must
    // hilo lower the subtree
    //
    if (Action(LOWER_HILO) &&
#ifdef NEW_LOWER
	is_hilo_type(WN_desc(tree))
#else
        (MTYPE_is_double(WN_desc(tree)) || MTYPE_is_longlong(WN_desc(tree)))
#endif
	) {
      WN *hi, *lo;
      lower_hilo_expr(block, 
		      //WN_kid0(tree), 
		      tree,
		      actions, 
		      &hi, 
		      &lo 
		      /* WN_desc(tree) */);
      tree = lo;
      kids_lowered = TRUE;
    }
    // else we'll have to lower the kids
#endif
    break;

  case OPR_TAS:

    /* If the operand of the TAS is a load from memory, try to
     * replace with a load matching the result type of the TAS.
     * Doing so may avoid a move from one register set to another.
     * 14525: Can't convert TAS(F8F8ILOAD) to I8I8ILOADX
     */
    if (Action(LOWER_TO_CG))
    {
      WN *load = WN_kid0(tree);
      if (   OPERATOR_is_load(WN_operator(load))
	  && WN_operator(load) != OPR_ILOADX  // 14525
	  && (!WN_has_sym(load) || WN_class(load) != CLASS_PREG))
      {
	TYPE_ID tas_rtype = WN_rtype(tree);
	if (MTYPE_byte_size(tas_rtype) == MTYPE_byte_size(WN_desc(load))) {
	  WN_set_rtype(load, tas_rtype);
	  WN_set_desc(load, tas_rtype);
	  WN_Delete(tree);
	  return lower_expr(block, load, actions);
	}
      }
    }
    break;

  case OPR_IMAGPART:
    if (Action(LOWER_COMPLEX))
    {
      WN	*realexp, *imagexp;
      lower_complex_expr(block, WN_kid0(tree), actions, &realexp, &imagexp);
      WN_Delete(tree);
      tree = lower_expr(block, imagexp, actions);
    }
    break;

  case OPR_REALPART:
    if (Action(LOWER_COMPLEX))
    {
      WN	*realexp, *imagexp;

      lower_complex_expr(block, WN_kid0(tree), actions, &realexp, &imagexp);
      WN_Delete(tree);
      tree = lower_expr(block, realexp, actions);
    }
    break;
    
  case OPR_EQ:
    if (Action(LOWER_COMPLEX) && MTYPE_is_complex(WN_desc(tree)))
    {
      /*
       *  x == y
       *    R(x)==R(y) && I(x)==I(y)
       */
      WN	*rx, *ry, *ix, *iy;
      TYPE_ID	realTY = Mtype_complex_to_real( WN_desc(tree));

      lower_complex_expr(block, WN_kid0(tree), actions, &rx, &ix);
      lower_complex_expr(block, WN_kid1(tree), actions, &ry, &iy);
 
      tree = WN_LAND(WN_EQ(realTY, rx, ry),
		     WN_EQ(realTY, ix, iy));

      return lower_expr(block, tree, actions);
    }
#ifdef TARG_ST
    //TB: Change float cmp by an integer cmp
    //[HK] fix for codex bug #35763, treat the case where one
    // of the operand is +/-0.0f separately
    if  (Float_Eq_Simp && WN_desc(tree) == MTYPE_F4)
      {
	if (Is_Const_Float(WN_kid0(tree))) {
	  TCON tc;
	  if (WN_operator(WN_kid0(tree)) == OPR_LDID ) 
	    tc = Const_Val(Preg_Home(WN_offset(WN_kid0(tree))));
	  else
	    tc = Const_Val(WN_kid0(tree));
	  float val = (float)Targ_To_Host_Float(tc);
	  union cvt {
	    float f;
	    INT32 i;
	  } c;
	  c.f = val;
	  if ((c.i << 1) == 0) { // special treatment for +-zero cases	    
	    WN *shiftval =  WN_CreateIntconst(OPC_I4INTCONST, 1);
	    WN *value = WN_CreateIntconst(OPC_I4INTCONST, 0);
	    return WN_EQ(MTYPE_I4, value, WN_Tas(MTYPE_I4, Be_Type_Tbl(MTYPE_I4), 
						 WN_Shl(MTYPE_I4, WN_kid1(tree), shiftval)));
	  }
	  else { 
	    WN *value = WN_CreateIntconst(OPC_I4INTCONST, c.i);
	    return WN_EQ(MTYPE_I4, value, WN_Tas(MTYPE_I4, Be_Type_Tbl(MTYPE_I4), WN_kid1(tree)));
	  }
	} else 	if (Is_Const_Float(WN_kid1(tree))) {
	  TCON tc;
	  if (WN_operator(WN_kid1(tree)) == OPR_LDID ) 
	    tc = Const_Val(Preg_Home(WN_offset(WN_kid1(tree))));
	  else
	    tc = Const_Val(WN_kid1(tree));
	  float val = (float)Targ_To_Host_Float(tc);
	  union cvt {
	    float f;
	    INT32 i;
	  } c;
	  c.f = val;
	  if ((c.i << 1)  == 0) { // special treatment for +-zero cases	    
	    WN *shiftval =  WN_CreateIntconst(OPC_I4INTCONST, 1);
	    WN *value = WN_CreateIntconst(OPC_I4INTCONST, 0);
	    return WN_EQ(MTYPE_I4, value, WN_Tas(MTYPE_I4, Be_Type_Tbl(MTYPE_I4), 
						 WN_Shl(MTYPE_I4, WN_kid0(tree), shiftval)));
	  }
	  else { 
	    WN *value = WN_CreateIntconst(OPC_I4INTCONST, c.i);
	    return WN_EQ(MTYPE_I4, value, WN_Tas(MTYPE_I4, Be_Type_Tbl(MTYPE_I4), WN_kid0(tree)));
	  }
	}
      }
#endif //TARG_ST    
    break;

  case OPR_NE:
    if (Action(LOWER_COMPLEX) && MTYPE_is_complex(WN_desc(tree)))
    {
      /*
       *  x != y
       *    ! ( R(x)==R(y)  &&  I(x)==I(y) )
       */
      WN	*rx, *ry, *ix, *iy;
      TYPE_ID	realTY = Mtype_complex_to_real( WN_desc(tree));

      lower_complex_expr(block, WN_kid0(tree), actions, &rx, &ix);
      lower_complex_expr(block, WN_kid1(tree), actions, &ry, &iy);
 
      tree = WN_LNOT(WN_LAND(WN_EQ(realTY, rx, ry),
			     WN_EQ(realTY, ix, iy)));

      return lower_expr(block, tree, actions);
    }
    break;

  case OPR_MADD:
    if (Action(LOWER_QUAD) && MTYPE_is_quad(type))
    {
     /*
      *   kid1 * kid2 + kid0
      */
      WN	*wn;

      wn = WN_Add(type,
		  WN_Mpy(type, WN_kid1(tree), WN_kid2(tree)),
		  WN_kid0(tree));
      WN_Delete(tree);

      tree = wn;
    }
    break;

  case OPR_MSUB:
    if (Action(LOWER_QUAD) && MTYPE_is_quad(type))
    {
     /*
      *   kid1 * kid2 - kid0
      */
      WN	*wn;

      wn = WN_Sub(type,
		  WN_Mpy(type, WN_kid1(tree), WN_kid2(tree)),
		  WN_kid0(tree));
      WN_Delete(tree);

      tree = wn;
    }
    break;

  case OPR_NMADD:
    if (Action(LOWER_QUAD) && MTYPE_is_quad(type))
    {
     /*
      *   - (kid1 * kid2 + kid0)
      */
      WN	*wn, *madd;

      madd = WN_Add(type,
		    WN_Mpy(type, WN_kid1(tree), WN_kid2(tree)),
		    WN_kid0(tree));

      wn = WN_Neg(type, madd);
      WN_Delete(tree);

      tree = wn;
    }
    break;

  case OPR_NMSUB:
    if (Action(LOWER_QUAD) && MTYPE_is_quad(type))
    {
     /*
      *   - (kid1 * kid2 - kid0)  -->   (kid0 - kid1 * kid2)
      */
      WN	*wn;

      wn = WN_Sub(type,
		  WN_kid0(tree),
		  WN_Mpy(type, WN_kid1(tree), WN_kid2(tree)));
      WN_Delete(tree);

      tree = wn;
    }
    break;

  case OPR_RSQRT:
    tree = lower_rsqrt(block, tree, actions);
    break;

  case OPR_RECIP:
    tree = lower_recip(block, tree, actions);
    break;

  case OPR_SELECT:
    {
#ifdef TARG_ST
      // [SC] Ensure the condition will be fully hilo-lowered.
      // This may happen at HILO lowering time that the condition
      // has not been runtime lowered yet. In this case we call directly
      // the runtime lowering as HILO lowering assume that conditions
      // is runtime lowered.
      if (Action(LOWER_HILO) && is_hilo_type(WN_rtype(WN_kid0(tree)))) {
	WN_kid0(tree) = rt_lower_expr (WN_NE (WN_rtype(WN_kid0(tree)),
					      WN_kid0(tree),
					      WN_Zerocon(WN_rtype(WN_kid0(tree)))), 
				       OnlyAction(actions, LOWER_RUNTIME));
      }
#endif
#ifdef KEY // f90 front-end sometimes generate inconsistent types; fix them
      if (MTYPE_size_min(type) != MTYPE_size_min(WN_rtype(WN_kid1(tree))))
	WN_kid1(tree) = WN_Cvt(WN_rtype(WN_kid1(tree)), type, WN_kid1(tree));
      if (MTYPE_size_min(type) != MTYPE_size_min(WN_rtype(WN_kid2(tree))))
	WN_kid2(tree) = WN_Cvt(WN_rtype(WN_kid2(tree)), type, WN_kid2(tree));
#endif
      WN * const kid0 = WN_kid0(tree);	// the condition expression
      if (WN_operator_is(kid0, OPR_INTCONST))
      {
	INT64 flag = WN_const_val(kid0);
	WN * const kid = flag ? WN_kid1(tree) : WN_kid2(tree);
	return lower_expr(block, kid, actions);
      } else if (WN_operator(kid0) == OPR_LNOT) {
	// swap the select inputs and replace the condition with
	// operand of the LNOT. Then lower the whole tree again
	// as there may be further lowerings that may occur with the
	// new operands.
	WN * const new_kid1 = WN_kid2(tree);
	WN * const new_kid2 = WN_kid1(tree);
	WN * const new_kid0 = WN_kid0(kid0);
	TYPE_ID new_desc = (WN_rtype(new_kid0) == MTYPE_B) ? MTYPE_B : MTYPE_V;
	WN_kid0(tree) = new_kid0;
	WN_kid1(tree) = new_kid1;
	WN_kid2(tree) = new_kid2;
	WN_set_desc(tree, new_desc);
	WN_Delete(kid0);
	return lower_expr(block, tree, actions);
      }
#ifdef TARG_ST
      // Emulate select if not available
      else if (Action(LOWER_SELECT) && !Can_Do_Select(WN_rtype(tree))) {
	WN *cond = WN_kid0(tree);
	WN *true_part = WN_kid1(tree);
	WN *false_part = WN_kid2(tree);
	WN *test, *then_block, *else_block;
	WN *if_stmt;
	PREG_NUM result;
	then_block = WN_CreateBlock ();
	result = AssignExpr(then_block, true_part, WN_rtype(true_part));
	else_block = WN_CreateBlock ();
	AssignToPregExpr(result, else_block, false_part, WN_rtype(false_part));
	if_stmt    = WN_CreateIf (cond, then_block, else_block);
	if_stmt = lower_scf(block, if_stmt, actions);
	WN_INSERT_BlockLast(block, if_stmt);
	return lower_expr(block, WN_LdidPreg(WN_rtype(true_part), result), actions);
      }
#endif
    }
    break;

  case OPR_PAREN:
    // [HK] distinguish between FP and Int reassociations
#ifdef TARG_ST
    if ((MTYPE_is_integral(type) && Roundoff_Level > ROUNDOFF_ASSOC)
	|| (MTYPE_is_float(type) && Reassoc_Level > REASSOC_ASSOC)
#else
    if (Roundoff_Level > ROUNDOFF_ASSOC
#ifdef TARG_X8664 // Such useless PARENs are used by LNO's Simd phase as markers
        || WN_kid_count(tree) < 2
#endif
#endif
       )
    {
     /*
      *  At suitable roundoff we may remove these parens
      *  This will allow better MADD generation latter (pv 316380)
      */
      WN *kid0 = WN_kid0(tree);

      WN_Delete(tree);
      return lower_expr(block, kid0, actions);
    }
    break;
#ifdef TARG_ST

  case OPR_DIV:
  case OPR_REM:
  case OPR_MOD:

    // [CG] perform lowering of fast division (power of 2) at whirl level.
    // Under the control of Is_Fast_Divide which itself is controlled
    // by Can_Do_Fast_Divide.
    if (Action(LOWER_FAST_DIV) && 
	(WN_operator(tree) == OPR_DIV || WN_operator(tree) == OPR_REM) &&
	(type ==  MTYPE_I4 || type == MTYPE_U4 ||
	 type ==  MTYPE_I8 || type == MTYPE_U8) &&
	Is_Const_Divide(tree) && Is_Fast_Divide(tree)) {
      WN *fast_div_tree;
      if (WN_operator(tree) == OPR_DIV)
	fast_div_tree = lower_fast_div(block, tree, actions);
      else // OPR == OPR_REM
	fast_div_tree = lower_fast_rem(block, tree, actions);
      WN_Delete(tree);
      return fast_div_tree;
    }

    // [HK] perform division by constant optimization for I4 and U4
    if (Action(LOWER_CNST_DIV) && WN_operator(tree) == OPR_DIV &&
	OPT_Cnst_DivRem &&
	((type == MTYPE_I4) || (type == MTYPE_U4)) &&
	Is_Const_Divide(tree) && !Is_Fast_Divide(tree)) {
      WN *cst_div_tree;
      if (type == MTYPE_I4)
	  cst_div_tree = lower_cst_idiv(block, tree, actions);
      else // type == MTYPE_U4
	  cst_div_tree = lower_cst_udiv(block, tree, actions);
      WN_Delete(tree);
      return cst_div_tree;
  }

    // [HK] perform remainder by constant optimization for I4 and U4
  if ( Action(LOWER_CNST_DIV) && WN_operator(tree) == OPR_REM &&
      OPT_Cnst_DivRem &&
      ((type == MTYPE_I4) || (type == MTYPE_U4)) &&
      Is_Const_Divide(tree) && !Is_Fast_Divide(tree)) {
      WN *cst_rem_tree;
      if (type == MTYPE_I4)
	  cst_rem_tree = lower_cst_irem(block, tree, actions);
      else // type == MTYPE_U4
	  cst_rem_tree = lower_cst_urem(block, tree, actions);
      WN_Delete(tree);
      return cst_rem_tree;
  }
  // TB: division specialization with feedback info
#ifdef KEY
 {
   // If tree includes ARRAY, first lower it
   for (INT j = 0; j < WN_kid_count(tree); j++)
    WN_kid(tree,j) = lower_expr(block, WN_kid(tree,j), actions);
   WN* simp = simp_remdiv( block, tree, actions );
   if( simp != NULL ){
     return simp;
   }
 }
#endif      

    // fallthrough to complex case below (after #endif)

#else

  case OPR_DIV:
  case OPR_REM:
  case OPR_MOD:

    {
#ifdef KEY
      WN* simp = simp_remdiv( block, tree, actions );
      if( simp != NULL ){
	return simp;
      }
#endif      

      /* If not inlining divides, then generate an INTRINSIC_OP that is
       * later lowered to a call
       */
      TYPE_ID rtype = OPCODE_rtype(WN_opcode(tree));
      if (Should_Call_Divide(rtype) && !Is_Fast_Divide(tree)) {

#ifdef EMULATE_LONGLONG
        if (rtype == MTYPE_I8 || rtype == MTYPE_U8) {
	  FmtAssert (OPCODE_rtype(WN_opcode(WN_kid0(tree))) == rtype,
		     ("DIV/REM/MOD: kid0 should be %d, is %d",
		     rtype, OPCODE_rtype(WN_opcode(WN_kid0(tree)))));
	  FmtAssert (OPCODE_rtype(WN_opcode(WN_kid1(tree))) == rtype,
		     ("DIV/REM/MOD: kid1 should be %d, is %d",
		     rtype, OPCODE_rtype(WN_opcode(WN_kid1(tree)))));
	}
#endif

	WN *kids[2];
	WN *iwn;
	LEAF tmpY;
	INTRINSIC intrinsic;
	BOOL is_unsigned = MTYPE_is_unsigned(rtype);
	BOOL is_float = MTYPE_is_float(rtype);
	BOOL is_double = MTYPE_is_size_double(rtype);
	switch (WN_operator(tree)) {
	case OPR_DIV:
	  if (is_float) {
	    intrinsic = (is_double ? INTRN_DIVDF3 : INTRN_DIVSF3);
	  } else if (is_double) {
	    intrinsic = (is_unsigned ? INTRN_UDIVDI3 : INTRN_DIVDI3);
	  } else {
	    intrinsic = (is_unsigned ? INTRN_UDIVSI3 : INTRN_DIVSI3);
	  }
	  break;
	case OPR_MOD:
	  FmtAssert(!is_float, ("Unexpected MOD operator on float"));
	  // Unsigned MOD is the same as REM.
	  // Signed MOD is a REM followed by an adjustment which
	  // uses the divisor, so save it to a temp and replace the
	  // divisor operand of the tree with a load from the temp.
	  if (!is_unsigned) {
	    tmpY = Make_Leaf(block, WN_kid1(tree), type);
	    WN_kid1(tree) = Load_Leaf(tmpY);
	  }
	  /*FALLTHROUGH*/
	case OPR_REM:
	  FmtAssert(!is_float, ("Unexpected REM operator on float"));
	  if (is_double) {
	    intrinsic = (is_unsigned ? INTRN_UMODDI3 : INTRN_MODDI3);
	  } else {
	    intrinsic = (is_unsigned ? INTRN_UMODSI3 : INTRN_MODSI3);
	  }
	  break;
	default:
	  #pragma mips_frequency_hint NEVER
	  FmtAssert (FALSE, ("Unexpected division operator"));
	  /*NOTREACHED*/
	}
	kids[0] = WN_CreateParm (rtype,
				 lower_expr(block, WN_kid0(tree), actions),
				 Be_Type_Tbl(rtype),
				 WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);
	kids[1] = WN_CreateParm (rtype, 
				 lower_expr(block, WN_kid1(tree), actions),
				 Be_Type_Tbl(rtype),
				 WN_PARM_BY_VALUE | WN_PARM_READ_ONLY);
	iwn = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
						 rtype, MTYPE_V),
				  intrinsic, 2, kids);

	if (WN_operator(tree) == OPR_MOD && !is_unsigned) {
	  // For signed MOD, we need to add the divisor to the result
	  // of the REM if both of the operands are negative.
	  WN *t2, *t3, *t4;
	  PREG_NUM t1;
	  t1 = AssignExpr(block, iwn, type);
	  t2 = WN_Bxor(type, WN_LdidPreg(type, t1), Load_Leaf(tmpY));
	  t3 = WN_Ashr(type, t2, WN_Intconst(type, MTYPE_size_reg(type) - 1));
	  t4 = WN_Band(type, Load_Leaf(tmpY), t3);
	  iwn = WN_Add(type, WN_LdidPreg(type, t1), t4);
	  iwn = lower_expr(block, iwn, actions);
	}

#ifdef TARG_X8664
	if( Action( LOWER_TO_CG ) &&
	    Action( LOWER_INTRINSIC) ){
	  BOOL intrinsic_lowered = FALSE;
	  iwn = lower_emulation( block, iwn, actions, intrinsic_lowered );
	}
#endif

	WN_Delete(tree);
	return iwn;
      }
    }
#endif /* TARG_ST */

    if (Action(LOWER_COMPLEX) && MTYPE_is_complex(WN_rtype(tree))) {
	// complex div creates if-then-else structure,
	// so want to expand this early, then just return C*LDID preg here
	// (e.g. in case is under a PARM node, and is not first parm).
	// Note that fortran fe moves this out from under call, 
	// but C doesn't.  Apparently only get here for C PARM case.
	TYPE_ID	cdiv_mtype = WN_rtype(tree);
	// using a preg causes wopt to get confused and not
	// connect the F* preg numbers with the C* preg number.
	// so use temp symbol instead.
	TY_IDX cdiv_ty = MTYPE_To_TY(cdiv_mtype);
	ST *cdiv_st = Gen_Temp_Symbol (cdiv_ty, ".complex_div");
	WN *stid = WN_Stid(cdiv_mtype, 0, cdiv_st, cdiv_ty, tree);
      	WN_Set_Linenum (stid, current_srcpos);
	stid = lower_store(block, stid, actions);
	WN_INSERT_BlockLast(block, stid);
	WN *ldid = WN_Ldid(cdiv_mtype, 0, cdiv_st, cdiv_ty);
	return ldid;
    }
    break;

#ifdef TARG_ST
  case OPR_MPY:
    // [CG] perform lowering of fast multiply (power of 2) multiply.
    // This lowering is supposed to be always profitable and thus is
    // not controlled by external option.
    if (Action(LOWER_FAST_MUL)  &&
	WNL_Is_Fast_Int_Mul_By_Constant_Candidate(tree)) {
      WN *fast_mul_tree;
      fast_mul_tree = WNL_Lower_Fast_Int_Mul_By_Constant(block, tree, actions);
      WN_Delete(tree);
      return fast_mul_tree;
    }

    // [CG] perform lowering of general constant multiply.
    // This lowering can be enabled/disabled by OPT_Cnst_Mul option.
    if (Action(LOWER_CNST_MUL) && OPT_Cnst_Mul &&
	WNL_Is_Int_Mul_By_Constant_Candidate(tree)) {
      WN *cnst_mul_tree;
      cnst_mul_tree = WNL_Lower_Int_Mul_By_Constant(block, tree, actions);
      WN_Delete(tree);
      return cnst_mul_tree;
    }

    break;
#endif
  case OPR_COMMA:
    {
      WN *commaBlock;
      commaBlock = lower_block(WN_kid0(tree), actions);

      DevWarn("lower_expr(): comma operator seen, line %d",
	      Srcpos_To_Line(current_srcpos));

#ifdef TARG_ST
      // [TB] Improve line number
      WN_copy_linenum(tree, commaBlock);
#endif
      WN_INSERT_BlockLast(block, commaBlock);
    }
    return lower_expr(block, WN_kid1(tree), actions);

  case OPR_CSELECT:
   /*
    *  
    */
    if (Action(LOWER_SHORTCIRCUIT))
    {
      /*******************************************************************

      DevWarn("lower_expr(): cselect operator seen, line %d",
              Srcpos_To_Line(current_srcpos));
      *******************************************************************/

      if (expr_is_speculative(tree))
      {
	WN *select = WN_Select(type, WN_kid0(tree), WN_kid1(tree),
			       WN_kid2(tree));
  
	WN_Delete(tree);
	return select;
      }
      else
      {
	PREG_NUM  tmpN;
	WN *if_tree, *if_then, *if_else, *stid;
	WN *body = WN_CreateBlock();

	if_then = WN_CreateBlock();
	tmpN = AssignExpr(if_then, WN_kid1(tree), type);

	if_else = WN_CreateBlock();
	stid = WN_StidIntoPreg(type, tmpN, MTYPE_To_PREG(type), WN_kid2(tree));
	WN_Set_Linenum(stid, current_srcpos);  // Bug 1268
	WN_INSERT_BlockLast(if_else, stid);

	if_tree = WN_CreateIf( WN_kid0(tree), if_then, if_else );
	if ( Cur_PU_Feedback )
	  Cur_PU_Feedback->FB_lower_branch( tree, if_tree );
	WN_INSERT_BlockLast( block, lower_if( body, if_tree, actions ) );

	return WN_LdidPreg(type, tmpN);
      }
    }
    break;

  case OPR_CAND:
  case OPR_CIOR:
   /*
    *  return boolean 0/1 (build CSELECT)
    */
    if (Action(LOWER_SHORTCIRCUIT))
    {
      if (expr_is_speculative(tree))
      {
	WN *cond;

	if (WN_operator_is(tree, OPR_CAND))
	  cond = WN_LAND( WN_kid0(tree), WN_kid1(tree));
	else
	  cond = WN_LIOR( WN_kid0(tree), WN_kid1(tree));

	WN_Delete(tree);
	return lower_expr(block, cond, actions);
      }
      else
      {
	WN *select = WN_Cselect(type,
				tree,	
				WN_Intconst(Boolean_type, 1),
				WN_Intconst(Boolean_type, 0));
	return lower_expr(block, select, actions);
      }
    }
    break;

#ifndef TARG_ST
#ifdef KEY
  case OPR_PURE_CALL_OP:
    if (Action(LOWER_TO_CG))
    {
      // create and insert call
      WN * call = WN_Create (OPR_CALL,
                             WN_rtype (tree),
			     WN_desc (tree),
			     WN_kid_count (tree));
      WN_st_idx (call) = WN_st_idx (tree);
      for (int i=0; i<WN_kid_count (call); i++)
        WN_kid (call, i) = WN_kid (tree, i);
      WN_Set_Linenum(call, current_srcpos);

      call = lower_call (block, call, actions);
      WN_INSERT_BlockLast (block, call);

      // ldid return val
      WN * ldid = WN_Ldid (WN_rtype (call), -1, Return_Val_Preg, MTYPE_TO_TY_array[WN_rtype (call)]);
      ldid = lower_return_ldid (block, ldid, actions);

      // stid return val into temporary
      ST * sym = Gen_Temp_Symbol (WN_ty (ldid), "pure_call_ret");
      WN * stid = WN_Stid (WN_rtype (ldid), 0, sym, WN_ty (ldid), ldid);
      WN_Set_Linenum(stid, current_srcpos);
      stid = lower_store (block, stid, actions);
      WN_INSERT_BlockLast (block, stid);

      // ldid val from temporary
      ldid = WN_Ldid (WN_desc (stid), 0, sym, WN_ty (ldid));
      WN_Delete (tree);

      tree = lower_expr (block, ldid, actions);

    }
    break;
#endif
#endif
  }

  
  if (Action(LOWER_QUAD))
  {
    if (WN_desc(tree) == MTYPE_FQ || WN_desc(tree) == MTYPE_F16)
    {
      switch (WN_operator(tree))
      {
      case OPR_CONST:
      case OPR_LDID:
      case OPR_ILOAD:
	break;
      case OPR_EQ:
      case OPR_NE:
      case OPR_LE:
      case OPR_LT:
      case OPR_GT:
      case OPR_GE:
      case OPR_CVT:
      case OPR_TRUNC:
      case OPR_RND:
      case OPR_CEIL:
      case OPR_FLOOR:
	tree = lower_emulation(block, tree, actions,intrinsic_lowered);
	break;
      default:
	break;
      }
    }
    if (WN_rtype(tree) == MTYPE_FQ || WN_rtype(tree) == MTYPE_F16)
    {
      switch (WN_operator(tree))
      {
      case OPR_CONST:
      case OPR_LDID:
      case OPR_ILOAD:
	break;

      case OPR_SELECT:
      case OPR_NEG:
      case OPR_ABS:
      case OPR_SQRT:
      case OPR_ADD:
      case OPR_SUB:
      case OPR_MPY:
      case OPR_DIV:
      case OPR_MOD:
      case OPR_REM:
      case OPR_MAX:
      case OPR_MIN:
      case OPR_CVT:
      case OPR_TRUNC:
      case OPR_RND:
	tree = lower_emulation(block, tree, actions, intrinsic_lowered);
	break;
      default:
	break;
      }
    }
  }

  if (WN_nary_intrinsic(tree))
  {
    tree = WN_NaryToExpr(tree);
  }

  /* Lower kids if not done already. */
  if (! kids_lowered)
  {
     INT16 i;
     for (i = 0; i < WN_kid_count(tree); i++)
       WN_kid(tree,i) = lower_expr(block, WN_kid(tree,i), actions);
#ifdef TARG_ST
     /* [CG]: We disable simplifications if specified. */
     if (!wn_lower_no_wn_simplify)
       tree = WN_Simplify_Rebuild_Expr_Tree(tree,alias_manager);

     /* We rerun lower treeheight on the bbottom up pass, as
	simplification may have undone it. */
     if (Action(LOWER_TREEHEIGHT) &&
	 WN_is_commutative(tree))
       {
	   // [HK] here, use Reassoc_Level instead of Roundoff_Level as it is specific to FP
// 	 if (MTYPE_is_integral(WN_rtype(tree)) || Roundoff_Level >= ROUNDOFF_ASSOC)
	 if (MTYPE_is_integral(WN_rtype(tree)) || Reassoc_Level >= REASSOC_ASSOC)
	   tree = lower_tree_height(block, tree, actions);
       }
#else
     tree = WN_Simplify_Rebuild_Expr_Tree(tree,alias_manager);
#endif
  }

  return tree;
}


/* ====================================================================
 *
 *  static TY_IDX coerceTY(TY_IDX type, TYPE_ID btype)
 *
 * return TY corresponding to the btype
 * (type might be pointer -> btype)
 *
 * ==================================================================== */

static TY_IDX coerceTY(TY_IDX type, TYPE_ID btype)
{
  TY &ty = Ty_Table[type];

  if (TY_is_pointer(ty))
    return Make_Pointer_Type(coerceTY(TY_pointed(ty), btype));

  return MTYPE_To_TY(btype);
}




/* ====================================================================
 *
 *  static ST * coerceST(const ST *st, TYPE_ID type)
 *
 * return ST corresponding to the type
 *
 * ==================================================================== */

static ST *coerceST(const ST *st, TYPE_ID type)
{
  if (ST_class(st) == CLASS_PREG)
  {
   /*
    *  for now, only pregs must correspond to the type
    */
    return MTYPE_To_PREG(type);
  }

  return (ST *) st;
}

static ST *coerceST(const ST &st, TYPE_ID type)
{
  if (ST_class(&st) == CLASS_PREG)
  {
   /*
    *  for now, only pregs must correspond to the type
    */
    return MTYPE_To_PREG(type);
  }

  return (ST *) &st;
}


/* ====================================================================
 *
 * static BOOL WN_StoreIsUnused(WN *tree)
 *
 * Find if store has been marked by IPA as unused
 * This may require traversal of the address expression to find
 * an LDA or ARRAY
 * ==================================================================== */
static BOOL WN_StoreIsUnused(WN *tree)
{
  ST  *sym;

  switch(WN_operator(tree))
  {
  case OPR_LDA:
  case OPR_STID:
    sym = WN_st(tree);

    if (ST_class(sym) != CLASS_PREG  &&
        ST_class(sym) != CLASS_BLOCK &&
	ST_is_not_used(sym))
      return TRUE;
    break;

  case OPR_ARRAY:
    return WN_StoreIsUnused(WN_array_base(tree));
  }

  return FALSE;
}

/* ====================================================================
 *
 * WN *add_fake_parm(WN *o_call, WN *fake_actual)
 *
 * Add the fake actual parameter as the first parameter to the original call.
 * All original parameters are shifted down by 1.
 *
 * ==================================================================== */
static WN *add_fake_parm(WN *o_call, WN *fake_actual, TY_IDX ty_idx)
{
  WN *n_call;
  if (WN_operator(o_call) == OPR_ICALL)
    n_call = WN_Icall(MTYPE_V, MTYPE_V, WN_kid_count(o_call)+1, WN_ty(o_call));
  else
    n_call = WN_generic_call(WN_operator(o_call), MTYPE_V, MTYPE_V, 
			     WN_kid_count(o_call)+1, WN_st_idx(o_call));
  WN_call_flag(n_call) = WN_call_flag(o_call);
  WN_Set_Linenum(n_call, WN_Get_Linenum(o_call));
  if ( Cur_PU_Feedback ) {
    Cur_PU_Feedback->FB_lower_call( o_call, n_call );
  }
  WN_kid0(n_call) = WN_CreateParm(Pointer_Mtype, fake_actual, ty_idx,
			      WN_PARM_BY_REFERENCE | WN_PARM_PASSED_NOT_SAVED);
  for (INT32 i = 0; i < WN_kid_count(o_call); i++)
    WN_kid(n_call, i+1) = WN_COPY_Tree(WN_kid(o_call, i));
  return n_call;
}

/* ====================================================================
 *
 * WN *lower_return_mstid(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * "tree" must be an MSTID whose kid is MLDID of Return_Val_Preg (-1).
 * Perform lowering of MSTID whose rhs is Return_Val_Preg; translate to
 * either MSTORE or a sequence of STIDs.
 *
 * ==================================================================== */
static WN *lower_return_mstid(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  ST *preg_st;
  WN *n_rhs;
  WN *wn = NULL;	// init to prevent upward-exposed use
  RETURN_INFO return_info = Get_Return_Info(WN_ty(tree), Complex_Not_Simulated
#ifdef TARG_X8664
					    , last_call_ff2c_abi
#endif
  					   );
  #ifdef TARG_ST
  /* FdF 20050127: Check if next statement is a ( MSTID sym2 ( MMLID ( WN_st tree ) ) ). */
  WN *wn_next = WN_next(tree);
  if (wn_next && WN_operator(wn_next) == OPR_STID && WN_desc(wn_next) == MTYPE_M &&
      ST_class(WN_st(wn_next)) == CLASS_VAR && ST_sclass(WN_st(wn_next)) == SCLASS_AUTO &&
      WN_opcode(WN_kid0(wn_next)) == OPC_MMLDID && WN_st_idx(WN_kid0(wn_next)) == WN_st_idx(tree) &&
      !WN_store_offset(tree) && !WN_load_offset(WN_kid0(wn_next)) && !WN_store_offset(wn_next) &&
      WN_ty(WN_kid0(wn_next)) == WN_ty(WN_kid0(tree)) && WN_ty(wn_next) == WN_ty(tree)) {

    /* In this case, replace
       ( MSTID sym1 ( MMLID  symRet ) )
       ( MSTID sym2 ( MMLID sym1 ) )
       by
       ( MSTID sym2 ( MMLID  symRet ) )
       ( MSTID sym1 ( MMLID sym2 ) ) // In case sym1 is still used
    */

    ST_IDX sym1_idx = WN_st_idx(tree);
    WN_st_idx(WN_kid0(wn_next)) = WN_st_idx(tree) = WN_st_idx(wn_next);
    WN_st_idx(wn_next) = sym1_idx;
  }
#endif

  if (RETURN_INFO_return_via_first_arg(return_info)) { // fake first parm
    // get the previous MCALL statement
    WN *call = WN_last(block);
    Is_True(WN_operator(call) == OPR_CALL || WN_operator(call) == OPR_ICALL ||
	    WN_operator(call) == OPR_PICCALL,
	    ("statement preceding MMLDID of Return_Val_Preg must be a call"));
    Is_True(WN_rtype(call) == MTYPE_M
#if defined(TARG_X8664) || defined(TARG_MIPS)
	    || MTYPE_is_complex(WN_rtype(call))
#endif
#if defined(TARG_X8664)
	    || MTYPE_is_mmx_vector(WN_rtype(call))
#endif
	    , ("call preceding LDID of Return_Val_Preg has unexpected type"));

    WN *awn = WN_CreateLda(OPR_LDA, Pointer_Mtype, MTYPE_V, 
			   WN_store_offset(tree), 
			   Make_Pointer_Type(WN_ty(tree)), WN_st_idx(tree));
    awn = lower_expr(block, awn, actions);
    WN *n_call = add_fake_parm(call, awn, WN_ty(awn));
    WN_DELETE_FromBlock(block, call);
#ifdef TARG_ST
      // [TB] Improve line number
      WN_copy_linenum(tree, n_call);
#endif
    WN_INSERT_BlockLast(block, n_call); 

    WN_DELETE_Tree(tree);
    return NULL; // original MSTID disappears
  }
  else { // return via 1 or more return registers
    TYPE_ID mtype, desc;
    for (INT32 i = 0; i < RETURN_INFO_count(return_info); i++) {
      if (i != 0)
        WN_INSERT_BlockLast (block, wn); // insert the last STID created 
#ifdef TARG_ST
      // [TB] Improve line number
      WN_copy_linenum(tree, wn);
#endif
      mtype = RETURN_INFO_mtype(return_info, i);
      desc = mtype;
#ifdef KEY
      if (MTYPE_byte_size(mtype) > TY_size(WN_ty(tree)) && !MTYPE_float(mtype)){
	desc = Mtype_AlignmentClass(1, MTYPE_type_class(mtype));
	while (MTYPE_byte_size(desc) < TY_size(WN_ty(tree)))
	  desc = Mtype_next_alignment(desc);
      }
#endif
      preg_st = MTYPE_is_float(mtype) ? Float_Preg : Int_Preg;
      n_rhs = WN_CreateLdid(OPR_LDID, mtype, mtype, 
			    RETURN_INFO_preg(return_info, i), preg_st,
			    Be_Type_Tbl(mtype));
#ifndef KEY
      if (TY_align(ST_type(WN_st(tree))) < MTYPE_alignment(mtype)) {
	DevWarn("return_info struct alignment is smaller than register size, may produce wrong results");
      }
#endif
      wn = WN_CreateStid(OPR_STID, MTYPE_V, desc, 
		         WN_store_offset(tree)+i*MTYPE_byte_size(mtype),
			 WN_st_idx(tree), Be_Type_Tbl(desc), n_rhs);
      wn  = lower_store (block, wn, actions);
      WN_Set_Linenum(wn, current_srcpos);
    }
    WN_DELETE_Tree(tree);
    return wn;
  }
}

/* ====================================================================
 *
 * WN *lower_return_mistore(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * "tree" must be an MISTORE whose rhs is MLDID of Return_Val_Preg (-1).
 * Perform lowering of MISTORE whose rhs is Return_Val_Preg; translate to
 * either MSTORE or a sequence of ISTOREs.
 *
 * ==================================================================== */
static WN *lower_return_mistore(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TYPE_ID mtype;
  ST *preg_st;
  WN *n_rhs;
  WN *wn = NULL;	// init to prevent upward-exposed use
  RETURN_INFO return_info = Get_Return_Info(WN_ty(tree), Complex_Not_Simulated
#ifdef TARG_X8664
					    , last_call_ff2c_abi
#endif
					   );
  if (RETURN_INFO_return_via_first_arg(return_info)) { // fake first parm
    // get the previous MCALL statement
    WN *call = WN_last(block);
    Is_True(WN_operator(call) == OPR_CALL || WN_operator(call) == OPR_ICALL ||
	    WN_operator(call) == OPR_PICCALL,
	    ("statement preceding MMLDID of Return_Val_Preg must be a call"));
#ifndef TARG_X8664
    Is_True(WN_rtype(call) == MTYPE_M,
	    ("call preceding MMLDID of Return_Val_Preg not type M"));
#else
    Is_True(WN_rtype(call) == MTYPE_M || MTYPE_is_mmx_vector(WN_rtype(call)),
	    ("call preceding MMLDID of Return_Val_Preg not type M"));
#endif
    WN *awn = WN_COPY_Tree(WN_kid1(tree));
    if (WN_store_offset(tree) != 0) { // generate an ADD node for the offset
      WN *iwn = WN_CreateIntconst(OPR_INTCONST, Pointer_Mtype, MTYPE_V, 
				  WN_store_offset(tree));
      awn = WN_CreateExp2(OPR_ADD, Pointer_Mtype, Pointer_Mtype, awn, iwn);
    }
    awn = lower_expr(block, awn, actions);
    WN *n_call = add_fake_parm(call, awn, WN_ty(tree));
    WN_DELETE_FromBlock(block, call);
#ifdef TARG_ST
      // [TB] Improve line number
      WN_copy_linenum(tree, n_call);
#endif

    WN_INSERT_BlockLast(block, n_call); 

    WN_DELETE_Tree(tree);
    return NULL; // original MSTID disappears
  }
  else { // return via 1 or more return registers
    WN *base_expr;
    for (INT32 i = 0; i < RETURN_INFO_count(return_info); i++) {
      if (i != 0)
        WN_INSERT_BlockLast (block, wn); // insert the last STID created 
#ifdef TARG_ST
      // [TB] Improve line number
      WN_copy_linenum(tree, wn);
#endif
      mtype = RETURN_INFO_mtype(return_info, i);
      preg_st = MTYPE_is_float(mtype) ? Float_Preg : Int_Preg;
      n_rhs = WN_CreateLdid(OPR_LDID, mtype, mtype, 
			    RETURN_INFO_preg(return_info, i), preg_st,
			    Be_Type_Tbl(mtype));
      base_expr = WN_COPY_Tree(WN_kid1(tree));
      wn = WN_CreateIstore(OPR_ISTORE, MTYPE_V, mtype, 
		           WN_store_offset(tree)+i*MTYPE_byte_size(mtype),
			   Make_Pointer_Type(Be_Type_Tbl(mtype), FALSE), 
			   n_rhs, base_expr);
      WN_Set_Linenum(wn, WN_Get_Linenum(tree));
      wn  = lower_store (block, wn, actions);
    }
    WN_DELETE_Tree(tree);
    return wn;
  }
}

/* ====================================================================
 *
 * WN *lower_mstid(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on MSTID nodes returning
 * an equivalent MSTORE node.
 *
 * ==================================================================== */

static WN *lower_mstid(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TY_IDX ty_idx  = WN_ty(tree);
  TY_IDX pty_idx;
  UINT64 size    = WN_field_id(tree) == 0 ?
      TY_size(ty_idx):
      TY_size(get_field_type (ty_idx, WN_field_id(tree)));
  WN*    wn;
  WN*    awn;
  WN*    swn;

  Is_True((WN_operator(tree) == OPR_STID && WN_desc(tree) == MTYPE_M),
	  ("expected mstid node, not %s", OPCODE_name(WN_opcode(tree))));

  pty_idx = Make_Pointer_Type (ty_idx, FALSE);

  WN *rhs = WN_kid0(tree);
  if (WN_opcode(rhs) == OPC_MMLDID && WN_st(rhs) == Return_Val_Preg) {
    // handle lowering of MLDID of Return_Val_Preg followed by MSTID
    Is_True(Action(LOWER_RETURN_VAL),
	    ("LOWER_RETURN_VAL action must be specified"));
    return lower_return_mstid(block, tree, actions);
  }

#ifdef KEY
  // Don't do struct copy if src and dest are the same.
  if (WN_opcode(rhs) == OPC_MMLDID &&
      WN_st(rhs) == WN_st(tree) &&
      WN_load_offset(rhs) == WN_store_offset(tree)) {
    WN_Delete(tree);
    return NULL;
  }
#endif

  awn = WN_CreateLda (OPR_LDA, Pointer_Mtype, MTYPE_V, WN_store_offset(tree),
		      pty_idx, WN_st(tree));
  swn = WN_CreateIntconst (OPC_U4INTCONST, size);
  wn  = WN_CreateMstore (0, pty_idx, WN_kid0(tree), awn, swn);
  lower_copy_maps(tree, wn, actions);  // 15113: Preserve alias info
  WN_Set_Linenum(wn, current_srcpos);
  WN_set_field_id(wn, WN_field_id(tree));
  wn  = lower_store (block, wn, actions);

  WN_Delete(tree);
  return wn;
}


/* ====================================================================
 *
 * WN *lower_mistore(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on MISTORE nodes returning
 * an equivalent MSTORE node.
 *
 * ==================================================================== */

static WN *lower_mistore(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TY_IDX pty_idx  = WN_ty(tree);
  TY_IDX ty_idx  = TY_pointed(pty_idx);

  if (WN_field_id (tree) != 0)
    ty_idx = get_field_type (ty_idx, WN_field_id (tree));
  
  UINT64 size    = TY_size(Ty_Table[ty_idx]);
  WN*    wn;
  WN*    swn;

  Is_True((WN_operator(tree) == OPR_ISTORE && WN_desc(tree) == MTYPE_M),
	  ("expected mistore node, not %s", OPCODE_name(WN_opcode(tree))));
  Is_True(TY_kind(pty_idx) == KIND_POINTER,
	  ("type specified in MISTORE not pointer"));
//Is_True(size > 0, ("type in MISTORE cannot be zero size"));
  if (size == 0)
    DevWarn ("type in MISTORE cannot be zero size");
  WN *rhs = WN_kid0(tree);
  if (WN_opcode(rhs) == OPC_MMLDID && WN_st(rhs) == Return_Val_Preg) {
    // handle lowering of MLDID of Return_Val_Preg followed by MISTORE
    Is_True(Action(LOWER_RETURN_VAL),
	    ("LOWER_RETURN_VAL action must be specified"));
    return lower_return_mistore(block, tree, actions);
  }

  swn = WN_CreateIntconst(OPC_U4INTCONST, size);
  wn  = WN_CreateMstore(WN_offset(tree), pty_idx, 
			WN_kid0(tree), WN_kid1(tree), swn);  // 15130
  lower_copy_maps(tree, wn, actions);  // 15113: Preserve alias info
  WN_Set_Linenum(wn, current_srcpos);
  WN_set_field_id(wn, WN_field_id(tree));
  wn  = lower_store(block, wn, actions);
#ifdef TARG_ST
  WN_Set_Linenum(wn, WN_Get_Linenum(tree));
#endif
  WN_Delete(tree);
  return wn;
}


#ifdef KEY
static BOOL Equiv (WN* wn1, WN* wn2) {
  if (!WN_Equiv(wn1,wn2)) return(FALSE);
  for (INT kidno=0; kidno<WN_kid_count(wn1); kidno++) {
    if (!Equiv(WN_kid(wn1,kidno),WN_kid(wn2,kidno))) {
      return(FALSE);
    }
  }
  return(TRUE);
}
#endif

#ifdef KEY // bug 12787
/* ====================================================================
 * walk expr looking for an INTRINSIC_OP that will become a function call;
 * return TRUE if found.
   ==================================================================== */
static BOOL has_call(WN *expr)
{
  if (WN_operator(expr) == OPR_INTRINSIC_OP &&
      ! INTRN_cg_intrinsic(WN_intrinsic(expr)))
    return TRUE;
  for (INT i = 0; i < WN_kid_count(expr); i++) {
    if (has_call(WN_kid(expr, i)))
      return TRUE;
  }
  return FALSE;
}
#endif

/* ====================================================================
 *
 * WN *lower_store(block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on store statement <tree>,
 * returning lowered tree.
 *
 * ==================================================================== */

static WN *lower_store(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  BOOL kids_lowered = FALSE;	/* becomes TRUE when kids are lowered */

  Is_True(OPCODE_is_store(WN_opcode(tree)),
	  ("expected store node, not %s", OPCODE_name(WN_opcode(tree))));

  /* If the store is to memory and the expression begins with a TAS, try to
   * replace with a store matching the result type of the TAS's expression.
   * Doing so may avoid a move from one register set to another.
   */
  if (Action(LOWER_TO_CG))
  {
    WN *tas = WN_kid0(tree);
    if (   WN_operator(tas) == OPR_TAS
	&& (!WN_has_sym(tree) || WN_class(tree) != CLASS_PREG))
    {
      WN *tas_kid0 = WN_kid0(tas);
      TYPE_ID tas_kid0_rtype = WN_rtype(tas_kid0);
      if (MTYPE_byte_size(WN_rtype(tas)) == MTYPE_byte_size(tas_kid0_rtype)) {
	WN_set_desc(tree, tas_kid0_rtype);
	WN_kid0(tree) = tas_kid0;
	WN_Delete(tas);
      }
    }
  }

#ifdef KEY // bug 11938
  // If OPT_Float_Via_Int is on, and both rhs and lhs are indirect memory ops,
  // change to use integer type for the copy
  if (Action(LOWER_TO_CG) && OPT_Float_Via_Int && ! WN_has_sym(tree)) {
    WN *rhs = WN_kid0(tree);
    if (MTYPE_is_float(WN_rtype(rhs)) && MTYPE_byte_size(WN_rtype(rhs)) <= 8 &&
        OPERATOR_is_load(WN_operator(rhs)) && ! WN_has_sym(rhs)) {
      TYPE_ID newtyp = Mtype_TransferSize(WN_rtype(rhs), MTYPE_I4);
      WN_set_rtype(rhs, newtyp);
      WN_set_desc(rhs, newtyp);
      WN_set_desc(tree, newtyp);
    }
  }
#endif

#ifdef TARG_X8664 // bug 11800
  // If the store is to memory and the expression is a load from memory, both
  // of MMX vector type, change to use integer types so it does not need to
  // involve MMX registers
  if (Action(LOWER_TO_CG) || Action(LOWER_BIT_FIELD_ID)) {
    WN *rhs = WN_kid0(tree);
    if (MTYPE_is_mmx_vector(WN_rtype(rhs)) && 
        OPERATOR_is_load(WN_operator(rhs))) {
      if (! (WN_has_sym(rhs) && WN_class(rhs) == CLASS_PREG) &&
 	  ! (WN_has_sym(tree) && WN_class(tree) == CLASS_PREG)) {
	TYPE_ID newtyp = Mtype_TransferSize(WN_rtype(rhs), MTYPE_I4);
	WN_set_rtype(rhs, newtyp);
	WN_set_desc(rhs, newtyp);
	WN_set_desc(tree, newtyp);
      }
    }
  }
#endif

  /*
   * create any maps that need to be present
   */
  lower_map(tree, actions);


  /* Note: We must split constant offsets after lowering complex stores
   * and splitting symbol addresses since these may create new offsets
   * that need to be split.
   */
  
  switch (WN_operator(tree))
  {
  case OPR_ISTORE:
#ifndef TARG_ST
    if (Action(LOWER_TO_MEMLIB) && WN_operator(tree) == OPR_ISTORE) {
        tree = Lower_Mistore_Memlib(block,tree,actions);
        if (tree == NULL || WN_operator(tree)!=OPR_ISTORE) 
           return tree; //otherwise continue
    } 
#endif
    if (WN_StoreIsUnused(WN_kid1(tree)))
    {
      WN	*eval;

      eval = lower_eval(block, WN_CreateEval(WN_kid0(tree)), actions);
#ifdef TARG_ST
      // CL: keep line numbers for debug
      WN_Set_Linenum(eval, WN_Get_Linenum(tree));
#endif

      WN_Delete(tree);

      return eval;
    }

    if (Action(LOWER_MLDID_MSTID) && WN_desc(tree) == MTYPE_M)
      return lower_mistore(block, tree, actions);

    if (Action(LOWER_RETURN_VAL) && WN_desc(tree) == MTYPE_M &&
       WN_opcode(WN_kid0(tree)) == OPC_MMLDID && 
       WN_st(WN_kid0(tree)) == Return_Val_Preg) {
       return lower_return_mistore(block, tree, actions);
    }
#ifdef TARG_X8664 // MMX vector return type also uses fake parameters (bug 7733)
    if (Action(LOWER_RETURN_VAL) && MTYPE_is_mmx_vector(WN_desc(tree)) &&
        WN_operator(WN_kid0(tree)) == OPR_LDID && 
        WN_st(WN_kid0(tree)) == Return_Val_Preg) {
       return lower_return_mistore(block, tree, actions);
    }
#endif


    if (Action(LOWER_BIT_FIELD_ID) && WN_desc(tree) == MTYPE_BS) {
#ifdef TARG_ST
      // Performs full lowering of bitfield store.
      return lower_store_bitfield(block, tree, actions);
#else
      lower_bit_field_id(tree);
      if (Action(LOWER_BITS_OP) && WN_operator (tree) == OPR_ISTBITS)
	return lower_store_bits (block, tree, actions);
#endif
    }

    if (Action(LOWER_COMPLEX) && MTYPE_is_complex(WN_desc(tree)))
    {
      WN	*realstore, *imagstore;
      LEAF	realexpN, imagexpN;
      WN_OFFSET	offset = WN_store_offset(tree);
      TYPE_ID	realTY = Mtype_complex_to_real( WN_desc(tree));
      {
       /*
	* create the real/imaginary stores
	* load the temporary values into a preg before the store (pv314583)
	* as the store may interfere with the expression.
	*/
	WN	*realexp, *imagexp;

	lower_complex_expr(block, WN_kid0(tree), actions, &realexp, &imagexp);
	
	realexpN = Make_Leaf(block, realexp, realTY);
	imagexpN = Make_Leaf(block, imagexp, realTY);
      }

      if (Action(LOWER_BASE_INDEX))
      {
	WN	*base, *index, *addr;
	LEAF	indexN;

	base = index=	NULL;
	lower_to_base_index(WN_kid1(tree), &base, &index) ;

	base = lower_expr(block, base, actions);
	index = lower_expr(block, index, actions);

	indexN = Make_Leaf(block, index, Pointer_type);

	addr = WN_Add(Pointer_type,
		      Load_Leaf(indexN),
		      lower_copy_tree(base, actions));

	realstore = WN_Istore(realTY,
			      offset,
			      coerceTY(WN_ty(tree), realTY),
			      addr,
			      Load_Leaf(realexpN));

	addr = WN_Add(Pointer_type, Load_Leaf(indexN), base);

	imagstore = WN_Istore(realTY,
			      coerceOFFSET(tree, realTY, offset),
			      coerceTY(WN_ty(tree), realTY),
			      addr,
			      Load_Leaf(imagexpN));
      }
      else
      {
	WN	*addr;
        LEAF	addrN;

	addr = lower_expr(block, WN_kid1(tree), actions);
	addrN = Make_Leaf(block, addr, Pointer_type);

        realstore =  WN_Istore(realTY,
			       offset,
			       coerceTY(WN_ty(tree), realTY),
			       Load_Leaf(addrN),
			       Load_Leaf(realexpN));

        imagstore =  WN_Istore(realTY,
			       coerceOFFSET(tree, realTY, offset),
			       coerceTY(WN_ty(tree), realTY),
			       Load_Leaf(addrN),
			       Load_Leaf(imagexpN));
      }
      realstore = lower_store(block, realstore, actions);
      WN_Set_Linenum (realstore, WN_Get_Linenum(tree));
      WN_INSERT_BlockLast(block, realstore);

      imagstore = lower_store(block, imagstore, actions);
      lower_complex_maps(tree, realstore, imagstore, actions);
      WN_Delete(tree);

      return imagstore;
    }

#ifdef TARG_ST
    else if (Action(LOWER_HILO) && 
#ifdef NEW_LOWER
	     is_hilo_type(WN_desc(tree))
#else
	     (MTYPE_is_double(WN_desc(tree)) ||
	      MTYPE_is_longlong(WN_desc(tree)))
#endif
	     )
    {
      WN	*histore, *lostore;
      LEAF	 hiexpN, loexpN;
      WN_OFFSET	 offset = WN_store_offset(tree);
      TYPE_ID	 realTY = get_hilo_type(WN_desc(tree));

      {
       /*
	* create the hi/lo stores
	* load the temporary values into a preg before the store (pv314583)
	* as the store may interfere with the expression.
	*/
	WN	*hiexp, *loexp;

	if (traceRuntime) {
	  fprintf(TFile, "------> lower hilo ISTORE: \n");
	  fdump_tree(TFile, tree);
	}

	lower_hilo_expr(block, 
			WN_kid0(tree), 
			actions, 
			&hiexp, 
			&loexp 
			/* WN_desc(tree) */);
	
	if (traceRuntime) {
	  fprintf(TFile, "------> lowered hilo ISTORE: \n");
	  fdump_tree(TFile, hiexp);
	  fdump_tree(TFile, loexp);
	}

	hiexpN = Make_Leaf(block, hiexp, realTY);
	loexpN = Make_Leaf(block, loexp, realTY);
      }

      if (Action(LOWER_BASE_INDEX))
      {
	WN	*base, *index, *addr;
	LEAF	indexN;

	base = index=	NULL;
	lower_to_base_index(WN_kid1(tree), &base, &index) ;

	base = lower_expr(block, base, actions);
	index = lower_expr(block, index, actions);

	indexN = Make_Leaf(block, index, Pointer_type);

	addr = WN_Add(Pointer_type,
		      Load_Leaf(indexN),
		      lower_copy_tree(base, actions));

	/*
	 * Depending on target's endianness -- store order
	 */
	if (Target_Byte_Sex == BIG_ENDIAN) {
	  // big endian : hi then lo
	  histore = WN_Istore(realTY,
			      offset,
			      coerceTY(WN_ty(tree), realTY),
			      addr,
			      Load_Leaf(loexpN));
	  
	  addr = WN_Add(Pointer_type, Load_Leaf(indexN), base);
	  
	  lostore = WN_Istore(realTY,
			      coerceOFFSET(tree, realTY, offset),
			      coerceTY(WN_ty(tree), realTY),
			      addr,
			      Load_Leaf(hiexpN));
	  
	} else {
	  // little endian : lo then hi
	  lostore = WN_Istore(realTY,
			      offset,
			      coerceTY(WN_ty(tree), realTY),
			      addr,
			      Load_Leaf(loexpN));

	  addr = WN_Add(Pointer_type, Load_Leaf(indexN), base);

	  histore = WN_Istore(realTY,
			      coerceOFFSET(tree, realTY, offset),
			      coerceTY(WN_ty(tree), realTY),
			      addr,
			      Load_Leaf(hiexpN));
	}
      }
      else
      {
	WN	*addr;
        LEAF	addrN;

	addr = lower_expr(block, WN_kid1(tree), actions);
	addrN = Make_Leaf(block, addr, Pointer_type);

	/*
	 * Depending on target's endianness -- store order
	 */
	if (Target_Byte_Sex == BIG_ENDIAN) {
	  // big endian : hi then lo
	  histore =  WN_Istore(realTY,
			       offset,
			       coerceTY(WN_ty(tree), realTY),
			       Load_Leaf(addrN),
			       Load_Leaf(hiexpN));
	  
	  lostore =  WN_Istore(realTY,
			       coerceOFFSET(tree, realTY, offset),
			       coerceTY(WN_ty(tree), realTY),
			       Load_Leaf(addrN),
			       Load_Leaf(loexpN));
	  
	}
	else {
	  // little endian : lo then hi
	  lostore =  WN_Istore(realTY,
			       offset,
			       coerceTY(WN_ty(tree), realTY),
			       Load_Leaf(addrN),
			       Load_Leaf(loexpN));
	  
	  histore =  WN_Istore(realTY,
			       coerceOFFSET(tree, realTY, offset),
			       coerceTY(WN_ty(tree), realTY),
			       Load_Leaf(addrN),
			       Load_Leaf(hiexpN));
	}
      }

      lostore = lower_store(block, lostore, actions);
      WN_Set_Linenum (lostore, WN_Get_Linenum(tree));
      WN_INSERT_BlockLast(block, lostore);

      histore = lower_store(block, histore, actions);
      lower_hilo_maps(tree, histore, lostore, actions, WN_desc(tree));
      WN_Delete(tree);

      return histore;
    }

#endif

    else if (Action(LOWER_QUAD) && MTYPE_is_quad(WN_desc(tree)))
    {
      WN	*hi, *lo, *hipart, *lopart;
      TYPE_ID	realTY = MTYPE_F8;
      WN_OFFSET	offset = WN_load_offset(tree);

      lower_quad_expr(block, WN_kid0(tree), actions, &hi, &lo);

      if (Action(LOWER_BASE_INDEX))
      {
	WN	*addr, *base, *index;
        LEAF	indexN;

	base = index = NULL;
	lower_to_base_index(WN_kid1(tree), &base, &index) ;

	base  = lower_expr(block, base, actions);
	index = lower_expr(block, index, actions);

	indexN = Make_Leaf(block, index, Pointer_type);

	addr   = WN_Add(Pointer_type,
		        Load_Leaf(indexN),
		        lower_copy_tree(base, actions));

	hipart = WN_Istore(realTY,
			   offset,
			   coerceTY(WN_ty(tree), realTY),
			   addr,
			   hi);

	addr   = WN_Add(Pointer_type, Load_Leaf(indexN), base);

	lopart = WN_Istore(realTY,
			   coerceOFFSET(tree, realTY, offset),
			   coerceTY(WN_ty(tree), realTY),
			   addr,
			   lo);
      }
      else
      {
        WN	*addr;
        LEAF	addrN;

	addr   = lower_expr(block, WN_kid1(tree), actions);

	addrN  = Make_Leaf(block, addr, Pointer_type);

	hipart = WN_Istore(realTY,
			   offset,
			   coerceTY(WN_ty(tree), realTY),
			   Load_Leaf(addrN),
			   hi);

	lopart = WN_Istore(realTY,
			   coerceOFFSET(tree, realTY, offset),
			   coerceTY(WN_ty(tree), realTY),
			   Load_Leaf(addrN),
			   lo);
      }
      hipart = lower_store(block, hipart, actions);
      WN_Set_Linenum (hipart, WN_Get_Linenum(tree));
      WN_INSERT_BlockLast(block, hipart);

      lopart = lower_store(block, lopart, actions);
      lower_quad_maps(tree, hipart, lopart, actions);
      WN_Delete(tree);
      return lopart;
    }
    else if (Action(LOWER_SPLIT_CONST_OFFSETS))
    {
      /*
       * Split
       *       ADDR
       *     ISTORE (offset>16bits)
       * into
       *         ADDR
       *         CONST (hi)
       *       ADD
       *     ISTORE (lo)
       */
      WN_OFFSET offset = WN_store_offset(tree);

      if (mem_offset_must_be_split(offset))
      {
	WN_kid1(tree) = WN_Add(Pointer_type, WN_kid1(tree),
			       WN_Intconst(Pointer_type,
					   mem_offset_hi(offset)));
	WN_store_offset(tree) = mem_offset_lo(offset);
      }
    }
    else
    {
     /*
      * Split
      *       LDA  (c1) <sym> 
      *	    ISTORE (c2)
      * into
      *       LDA  (0) <sym>
      *     ISTORE (c1+c2)
      *
      * provided sym is not a PREG.
      */
      WN  *addr_kid = WN_kid1(tree);

#ifdef TARG_MIPS
      /* We don't want this for MIPS since (c1+c2) could likely be
         out-of-range for the ISTORE, and we have the global
         canonicalization optimization to assign c1 and c2
         appropriately. */
      if (FALSE)
#else
      if (foldLdaOffset(addr_kid, WN_store_offset(tree)))
#endif
      {
	WN_store_offset(tree) += WN_lda_offset(addr_kid);
	WN_lda_offset(addr_kid) = 0;
      }
#ifndef TARG_MIPS
     /*
      * Fold
      *         EXPR
      *         CONST (c1)
      *       ADD
      *     ISTORE c2
      * Into
      *	      EXPR
      *     ISTORE c1+c2
      */
      if (WN_operator_is(addr_kid, OPR_ADD) &&
	  foldConstOffset(WN_kid1(addr_kid), WN_store_offset(tree)))
      {
	WN_store_offset(tree) += WN_const_val(WN_kid1(addr_kid));
	WN_kid1(tree) = WN_kid0(addr_kid);
	WN_Delete(WN_kid1(addr_kid));
	WN_Delete(addr_kid);
      }
#endif
    }
    break;

  case OPR_STID:
    {
      PREG_NUM last_preg = Get_Preg_Num (PREG_Table_Size(CURRENT_SYMTAB));
#ifndef TARG_ST
      if (Action(LOWER_TO_MEMLIB)) {
         tree = Lower_STID_Memlib(block,tree,actions);
         if (tree == NULL || WN_operator(tree) != OPR_STID) // Deleted or transformed already
            return tree; 
      }
#endif 
      if ((WN_class(tree) == CLASS_PREG) &&
	  (WN_store_offset(tree) > last_preg))
      {
	  DevWarn("lower_store() pregno %d > SYMTAB_last_preg(%d)",
		  WN_load_offset(tree), last_preg);
      }
    }

    if (WN_StoreIsUnused(tree))
    {
      WN	*eval;

      eval = lower_eval(block, WN_CreateEval(WN_kid0(tree)), actions);
#ifdef TARG_ST
      // CL: keep line numbers for debug
      WN_Set_Linenum(eval, WN_Get_Linenum(tree));
#endif


      WN_Delete(tree);

      return eval;
    }

    if (Action(LOWER_MLDID_MSTID) && WN_desc(tree) == MTYPE_M)
      return lower_mstid(block, tree, actions);

    if (Action(LOWER_RETURN_VAL) && WN_desc(tree) == MTYPE_M &&
       WN_opcode(WN_kid0(tree)) == OPC_MMLDID && 
       WN_st(WN_kid0(tree)) == Return_Val_Preg) {
       return lower_return_mstid(block, tree, actions);
    }
#ifdef TARG_X8664 // fortran complex treated as structs (bug 1664)
    if (Action(LOWER_RETURN_VAL) && MTYPE_is_complex(WN_desc(tree)) &&
       (Is_Target_32bit() || last_call_ff2c_abi || F2c_Abi) &&
       WN_operator(WN_kid0(tree)) == OPR_LDID && 
       WN_st(WN_kid0(tree)) == Return_Val_Preg) {
       return lower_return_mstid(block, tree, actions);
    }
#endif
#ifdef TARG_MIPS // Bug 12946: Return complex quad via first arg
    if (Action(LOWER_RETURN_VAL) && WN_desc(tree) == MTYPE_CQ &&
       WN_operator(WN_kid0(tree)) == OPR_LDID &&
       WN_st(WN_kid0(tree)) == Return_Val_Preg) {
       return lower_return_mstid(block, tree, actions);
    }
#endif
#ifdef TARG_X8664 // MMX vector return type also uses fake parameters (bug 7733)
    if (Action(LOWER_RETURN_VAL) && MTYPE_is_mmx_vector(WN_desc(tree)) &&
        WN_operator(WN_kid0(tree)) == OPR_LDID && 
        WN_st(WN_kid0(tree)) == Return_Val_Preg) {
       return lower_return_mstid(block, tree, actions);
    }
#endif

    if (Action(LOWER_BIT_FIELD_ID) && WN_desc(tree) == MTYPE_BS) {
#ifdef TARG_ST
      // Performs full lowering of bitfield store.
      return lower_store_bitfield(block, tree, actions);
#else
      lower_bit_field_id(tree);
      if (Action(LOWER_BITS_OP) && WN_operator (tree) == OPR_STBITS)
	return lower_store_bits (block, tree, actions);
#endif
    }

    if (Action(LOWER_COMPLEX) && MTYPE_is_complex(WN_desc(tree)))
    {
      WN	*realexp, *imagexp;
      TYPE_ID	realTY;

      realTY =	Mtype_complex_to_real( WN_desc(tree));
      lower_complex_expr(block, WN_kid0(tree), actions, &realexp, &imagexp);

#ifdef TARG_X8664 // MTYPE_C4 is returned in one SSE register
      if (WN_rtype(WN_kid0(tree)) == MTYPE_F8) {
	// the kid is xmm0
        Is_True(WN_desc(tree) == MTYPE_C4, 
		("lower_store: error in handling return of 32-bit complex"));
	WN_set_desc(tree, MTYPE_F8);
	return tree;
      }
      else if (WN_desc(tree) == MTYPE_C4 && WN_st(tree) == Float_Preg && 
	       WN_load_offset(tree) == First_Float_Preg_Return_Offset) {
	// the store target is xmm0
	ST *c4temp_st = Gen_Temp_Symbol(MTYPE_To_TY(MTYPE_F8), ".c4");

        // bug 7290 - lower the rhs node correctly.
	WN *realexp_copy, *imagexp_copy;
	PREG_NUM realexpN, imagexpN;
	if (WN_operator(realexp) == OPR_CONST) {
	  realexp_copy = realexp;
	} else {
	  realexpN = AssignExpr(block, realexp, realTY);
	  realexp_copy = WN_LdidPreg(realTY, realexpN);
	}
	if (WN_operator(imagexp) == OPR_CONST) {
	  imagexp_copy = imagexp;
	} else {
	  imagexpN = AssignExpr(block, imagexp, realTY);
	  imagexp_copy = WN_LdidPreg(realTY, imagexpN);
	}

	// store the real part
	WN *stid = WN_Stid(WN_rtype(realexp), 0, c4temp_st, realTY, 
	                   realexp_copy);
        WN_Set_Linenum (stid, WN_Get_Linenum(tree));
        WN_INSERT_BlockLast(block, stid);

	// store the imag part
	stid = WN_Stid(WN_rtype(imagexp), 4, c4temp_st, realTY, imagexp_copy);
        WN_Set_Linenum (stid, WN_Get_Linenum(tree));
        WN_INSERT_BlockLast(block, stid);

        WN_Delete(WN_kid0(tree));
	WN_kid0(tree) = WN_Ldid(MTYPE_F8, 0, c4temp_st, MTYPE_To_TY(MTYPE_F8));
	WN_set_desc(tree, MTYPE_F8);
	return tree;

      } else if( Is_Target_32bit()         &&
		 WN_desc(tree) == MTYPE_C4 &&
		 WN_st(tree) == Float_Preg && 
		 WN_load_offset(tree) == First_X87_Preg_Return_Offset &&
		 (
#if defined(BUILD_OS_DARWIN)
		   /* Darwin -m32 returns Fortran SP complex in registers too */
		   1
#else /* defined(BUILD_OS_DARWIN) */
		   PU_c_lang(Get_Current_PU()) ||
		   PU_cxx_lang(Get_Current_PU())
#endif /* defined(BUILD_OS_DARWIN) */
		   )){
	/* For C/C++ under -m32, if the return value is type
	   "__complex__ float", the real part will be stored at %eax, and
	   the imag part will be stored at %edx.  (bug#2842)
	 */
	const TYPE_ID mtype = MTYPE_I4;
	// the store target is %eax
	ST *c4temp_st = Gen_Temp_Symbol(MTYPE_To_TY(MTYPE_C4), ".c4");

	WN *realexp_copy = realexp;
	WN *imagexp_copy = imagexp;
#ifdef KEY // bug 12787
        if (has_call(realexp)) {
	  PREG_NUM realexpN = AssignExpr(block, realexp, realTY);
          realexp_copy = WN_LdidPreg(realTY, realexpN);
        }
        if (has_call(imagexp)) {
	  PREG_NUM imagexpN = AssignExpr(block, imagexp, realTY);
          imagexp_copy = WN_LdidPreg(realTY, imagexpN);
        } 
#endif

	// store the real part
	WN *stid = WN_Stid( WN_rtype(realexp), 0, c4temp_st, realTY, realexp_copy);
        WN_Set_Linenum( stid, WN_Get_Linenum(tree) );
        WN_INSERT_BlockLast( block, stid );

	WN* ldid = WN_Ldid( mtype, 0, c4temp_st, MTYPE_To_TY(mtype) );
	stid = WN_Stid( mtype, First_Int_Preg_Return_Offset,
			Int_Preg, mtype, ldid );
        WN_Set_Linenum( stid, WN_Get_Linenum(tree) );
        WN_INSERT_BlockLast( block, stid );

	// store the imag part
	stid = WN_Stid( WN_rtype(imagexp), 4, c4temp_st, realTY, imagexp_copy);
        WN_Set_Linenum (stid, WN_Get_Linenum(tree));
        WN_INSERT_BlockLast(block, stid);

        WN_Delete(WN_kid0(tree));
	WN_Delete( tree );

	ldid = WN_Ldid( mtype, 4, c4temp_st, MTYPE_To_TY(mtype) );
	tree = WN_Stid( mtype, Last_Int_Preg_Return_Offset,
			Int_Preg, mtype, ldid );

	return tree;	
      }
      else
#endif
     /*
      * create the real/imaginary stores
      * load the temporary values into a preg before the store (pv314583)
      * as the store may interfere with the expression.
      */
      {
        WN_OFFSET	offset = WN_store_offset(tree);
        PREG_NUM	realexpN, imagexpN;
        WN		*wn;
	WN *realexp_copy,*imagexp_copy;

	if (WN_operator(realexp) == OPR_CONST) {
	   realexp_copy = realexp;
	} else {
	   realexpN = AssignExpr(block, realexp, realTY);
	   realexp_copy = WN_LdidPreg(realTY, realexpN);
	}

	if (WN_operator(imagexp) == OPR_CONST) {
	   imagexp_copy = imagexp;
	} else {
	   imagexpN = AssignExpr(block, imagexp, realTY);
	   imagexp_copy = WN_LdidPreg(realTY, imagexpN);
	}

	wn = WN_Stid(realTY,
		     offset, 
		     coerceST(WN_st(tree), realTY),
		     MTYPE_To_TY(realTY),
		     realexp_copy);

        realexp = lower_store(block, wn, actions);
        WN_Set_Linenum (realexp, WN_Get_Linenum(tree));
        WN_INSERT_BlockLast(block, realexp);

	wn = WN_Stid(realTY,
		     coerceOFFSET(tree, realTY, offset),
		     coerceST(WN_st(tree), realTY),
		     MTYPE_To_TY(realTY),
		     imagexp_copy);

        imagexp = lower_store(block, wn, actions);

        lower_complex_maps(tree, realexp, imagexp, actions);

        WN_Delete(tree);

        return imagexp;
      }
    }
    
#ifdef TARG_ST
    else if (Action(LOWER_HILO) && 
#ifdef NEW_LOWER
	     is_hilo_type(WN_desc(tree))
#else
	     (MTYPE_is_double(WN_desc(tree)) || MTYPE_is_longlong(WN_desc(tree)))
#endif
	     )
    {
      WN      *wn;
      WN      *hiexp, *loexp;
      TYPE_ID  realTY = get_hilo_type(WN_desc(tree));

      if (traceRuntime) {
	fprintf(TFile, "------> lower hilo STID: \n");
	fdump_tree(TFile, tree);
      }

      //
      // Arthur: we allow I8STID (I4LDID) for example, thus need to
      //         lower_hilo_expr() of the kid if it is double type.
      //         Else, add a CVT node and HILO lower it.
      //
      // TODO: we shouldn't ??
      //
      if (MTYPE_is_double(WN_rtype(WN_kid0(tree))) ||
	  MTYPE_is_longlong(WN_rtype(WN_kid0(tree)))) {
	wn = WN_kid0(tree);
      }
      else {
	//
	// Add a CVT node and HILO lower it
	//
	wn = WN_Cvt(WN_rtype(WN_kid0(tree)), WN_desc(tree), WN_kid0(tree));
      }

      // HILO lower
      lower_hilo_expr(block, 
		      wn, 
		      actions, 
		      &hiexp, 
		      &loexp);

      //
      // create the hi/lo stores
      // load the temporary values into a preg before the store (pv314583)
      // as the store may interfere with the expression.
      //
      {
        WN_OFFSET	offset = WN_store_offset(tree);
        PREG_NUM	hiexpN, loexpN;
        WN	       *wn1, *wn2;
	WN             *hiexp_copy,*loexp_copy;

#if 1
	/*
	 * Arthur: I do not want no copies ! but why are they
	 *         there to begin with ??
	 */
	hiexp_copy = hiexp;
	loexp_copy = loexp;
#else
	if (WN_operator(hiexp) == OPR_CONST) {
	   hiexp_copy = hiexp;
	} else {
	   hiexpN = AssignExpr(block, hiexp, realTY);
	   hiexp_copy = WN_LdidPreg(realTY, hiexpN);
	}

	if (WN_operator(loexp) == OPR_CONST) {
	   loexp_copy = loexp;
	} else {
	   loexpN = AssignExpr(block, loexp, realTY);
	   loexp_copy = WN_LdidPreg(realTY, loexpN);
	}

	if (traceRuntime) {
	  fprintf(TFile, "------> save LDID: \n");
	  fdump_tree(TFile, hiexp_copy);
	  fdump_tree(TFile, loexp_copy);
	}
#endif

	/*
	 * Take care of endianness
	 */
	if (! WN_Pass_Low_First (tree)) {
	  // big endian : high order comes first
	  wn1 = WN_Stid(realTY,
			offset, 
			coerceST(WN_st(tree), realTY),
			MTYPE_To_TY(realTY),
			hiexp_copy);
	  
	  wn2 = WN_Stid(realTY,
			coerceOFFSET(tree, realTY, offset),
			coerceST(WN_st(tree), realTY),
			MTYPE_To_TY(realTY),
			loexp_copy);
	  
	  loexp = lower_store(block, wn2, actions);
	  WN_Set_Linenum (loexp, WN_Get_Linenum(tree));
	  WN_INSERT_BlockLast(block, loexp);
	  hiexp = lower_store(block, wn1, actions);
            
	}
	else {
	  // little endian : low order comes first
	  wn1 = WN_Stid(realTY,
			offset, 
			coerceST(WN_st(tree), realTY),
			MTYPE_To_TY(realTY),
			loexp_copy);
	  
	  wn2 = WN_Stid(realTY,
			coerceOFFSET(tree, realTY, offset),
			coerceST(WN_st(tree), realTY),
			MTYPE_To_TY(realTY),
			hiexp_copy);
	  
	  loexp = lower_store(block, wn1, actions);
	  WN_Set_Linenum (loexp, WN_Get_Linenum(tree));
	  WN_INSERT_BlockLast(block, loexp);
	  hiexp = lower_store(block, wn2, actions);
	  WN_Set_Linenum (hiexp, WN_Get_Linenum(tree));
	}

        lower_hilo_maps(tree, hiexp, loexp, actions, WN_desc(tree));

	if (traceRuntime) {
	  fprintf(TFile, "------> lowered hilo STID: \n");
	  fprintf(TFile, "lo part: \n");
	  fdump_tree(TFile, loexp);
	  fprintf(TFile, "hi part: \n");
	  fdump_tree(TFile, hiexp);
	}

        WN_Delete(tree);
        return hiexp;
      }
    }

#endif /* TARG_ST */

    else if (Action(LOWER_QUAD) && MTYPE_is_quad(WN_desc(tree)))
    {
      WN_OFFSET	offset = WN_store_offset(tree);
      WN	*hiexp, *loexp, *wn, *hipart, *lopart;
      TYPE_ID	realTY = MTYPE_F8;

      lower_quad_expr(block, WN_kid0(tree), actions, &hiexp, &loexp);

      /*
       * create the hi/lo stores
       */
      wn = WN_Stid(realTY, 
		   offset, 
		   coerceST(WN_st(tree), realTY),
		   MTYPE_To_TY(realTY), hiexp);

      hipart = lower_store(block, wn, actions);
      WN_Set_Linenum (hipart, WN_Get_Linenum(tree));
      WN_INSERT_BlockLast(block, hipart);

      wn = WN_Stid(realTY,
		   coerceOFFSET(tree, realTY, offset),
		   coerceST(WN_st(tree), realTY),
		   MTYPE_To_TY(realTY), loexp);
      lopart = lower_store(block, wn, actions);

      lower_quad_maps(tree, hipart, lopart, actions);

      WN_Delete(tree);

      return lopart;
    }

    if (Action(LOWER_SPLIT_SYM_ADDRS))
    {
     /*
      * Convert   (STID (offset) <sym>) into
      *       LDA (0) <base> 
      *     ISTORE (offs+ofst)          
      */
      WN  *istore;

      istore =	lower_split_sym_addrs(tree, WN_store_offset(tree), actions);
      if (istore)
      {
	return lower_store(block, istore, actions);
      }
    }

    if ( Action(LOWER_FORMAL_REF) && WN_class(tree) == CLASS_VAR)
    {
      WN  *istore;

      istore =  lower_formal_ref(tree, WN_store_offset(tree),
				 WN_st(tree), actions);
      if (istore)
      {
	return lower_store(block, istore, actions);
      }
    }

    if ( Action(LOWER_UPLEVEL))
    {
     ST *sym = WN_st(tree);

      if (ST_is_uplevelTemp(sym))
      {
        WN	   *istore;

        istore = lower_uplevel_reference(tree, WN_store_offset(tree), actions);
	return lower_store(block, istore, actions);
      }
    }

    break;

  case OPR_ISTBITS:
  case OPR_STBITS:
    if (Action(LOWER_BITS_OP))
      return lower_store_bits (block, tree, actions);
    break;

  case OPR_MSTORE:
    {
      WN *rhs = WN_kid0(tree);
      Is_True(!(WN_opcode(rhs) == OPC_MMLDID && WN_st(rhs) == Return_Val_Preg),
	      ("MMLDID of Return_Val_Preg cannot be rhs of MSTORE"));
    }

    if (Align_Object)
    {
      INT16 i;
      for (i = 0; i < WN_kid_count(tree); i++)
        WN_kid(tree,i) = lower_expr(block, WN_kid(tree,i), actions);

      tree = improve_Malignment(tree, WN_kid1(tree), WN_kid2(tree),
				WN_store_offset(tree));
    }

    if (Action(LOWER_MSTORE))
    {
      if (WN_StoreIsUnused(WN_kid1(tree)))
      {
	WN	*eval;

	eval = lower_eval(block, WN_CreateEval(WN_kid0(tree)), actions);

	WN_Delete(tree);

	return eval;
      }
     /*
      *  rewrite
      *		MSTORE (MCSELECT (expr, MLOAD, MLOAD)
      *  into
      *		MSTORE (MLOAD (CSELECT expr, MLOAD, MLOAD), size)
      */
      if (WN_opcode(WN_kid0(tree)) == OPC_MCSELECT)
      {
	WN	*select, *expr, *mload0, *mload1, *cselect;

	select = WN_kid0(tree);
	expr = WN_kid0(select);
	mload0 = WN_kid1(select);
	mload1 = WN_kid2(select);

	Is_True(WN_operator_is(mload0, OPR_MLOAD),
		("unexpected MSTORE (MCSELECT) pattern"));
	Is_True(WN_operator_is(mload1, OPR_MLOAD),
		("unexpected MSTORE (MCSELECT) pattern"));

	cselect = WN_Cselect(Pointer_type, expr, WN_kid0(mload0),
			     WN_kid0(mload1));
	WN_kid0(mload0) = cselect;
	WN_kid0(tree) = mload0;
      }
      tree = lower_mstore(block, tree, actions);
      kids_lowered = TRUE;
    }

    break;

  }

#ifdef KEY
  // If VHO lowerer if-converts, we would be left with speculated scalar 
  // versions (possibly remainder or pre-peel loops after vectorization
  // or even the main loop in the absence of LNO or vectorizer).
  // Convert back the SELECT to If-Then to avoid any overhead.
  // Convert: 
  // 	a[i] = <compare> ? <expr> : a[i]
  // back to:
  //    if <compare>
  //      a[i] = <expr>
  //    end if
  if (VHO_Enable_Simple_If_Conv && 
      !Action(LOWER_RETURN_VAL) /* Lower after LNO */ &&
      //bug 13853: these three lowers happen before lno, so should
      //           not transform select back to if
#ifndef TARG_ST
      !Action(LOWER_FAST_EXP)                         &&
#endif
      !Action(LOWER_TREEHEIGHT)                       &&
      !Action(LOWER_INLINE_INTRINSIC)                 &&
      OPCODE_is_store(WN_opcode(tree)) &&
      WN_operator(WN_kid0(tree)) == OPR_SELECT &&
      !MTYPE_is_vector(WN_desc(tree)) && MTYPE_is_float(WN_desc(tree))) {
    WN* store = tree;
    WN* load = WN_kid2(WN_kid0(tree));
    WN* array_l = WN_kid0(load);
    WN* array_s = WN_kid1(store);
    if ((WN_operator(load) == OPR_ILOAD &&
	 WN_operator(store) == OPR_ISTORE &&
	 WN_store_offset(store) == WN_load_offset(load) &&
	 Equiv(array_l, array_s) &&
	 WN_field_id(load) == WN_field_id(store)) ||
	(WN_operator(load) == OPR_LDID &&
	 WN_operator(store) == OPR_STID &&
	 WN_offset(store) == WN_offset(load) &&
	 ST_base(WN_st(store)) == ST_base(WN_st(load)) &&
	 ST_ofst(WN_st(store)) == ST_ofst(WN_st(load)))) {
      WN* select = WN_kid0(tree);
      WN* test = WN_COPY_Tree(WN_kid0(select));
      WN* then_block = WN_CreateBlock();
      WN* else_block = WN_CreateBlock();
      WN* istore = WN_COPY_Tree(tree);
      WN_kid0(istore) = WN_COPY_Tree(WN_kid1(select));
      tree = WN_CreateIf (test, then_block, else_block);
      WN_INSERT_BlockLast(then_block, istore);
      tree = lower_if(block, tree, actions);
      return tree;
    } 
  }
#endif
  /* Lower kids if not done already. */
  if (! kids_lowered)
  {
    INT16 i;

    for (i = 0; i < WN_kid_count(tree); i++)
      WN_kid(tree,i) = lower_expr(block, WN_kid(tree,i), actions);
  }

  return tree;
}




/* ====================================================================
 *
 * WN *lower_eval(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on eval statement <tree>,
 * returning lowered tree.
 *
 * ==================================================================== */

static WN *lower_eval(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  WN	*child = WN_kid0(tree);

  Is_True(WN_opcode(tree) == OPC_EVAL,
	  ("expected EVAL node, not %s", OPCODE_name(WN_opcode(tree))));

 if (Action(LOWER_MLDID_MSTID) && WN_opcode(child) == OPC_MMLDID) {
     child = lower_mldid(block, child, actions);
 }

  if (Action(LOWER_COMPLEX) && MTYPE_is_complex(WN_rtype(child)))
  {
    WN	*realexp, *imagexp, *eval;

    lower_complex_expr(block, child, actions, &realexp, &imagexp);

    realexp = lower_expr(block, realexp, actions);
    eval = WN_CreateEval(realexp);
    WN_Set_Linenum(eval, current_srcpos);  // Bug 1268
    WN_INSERT_BlockLast(block, eval);

    child = imagexp;
  }
  else if (Action(LOWER_QUAD) && MTYPE_is_quad(WN_rtype(child)))
  {
    WN	*hi, *lo, *eval;

    lower_quad_expr(block, child, actions, &hi, &lo);

    hi = lower_expr(block, hi, actions);
    eval = WN_CreateEval(hi);
    WN_Set_Linenum(eval, current_srcpos);  // Bug 1268
    WN_INSERT_BlockLast(block, eval);

    child = lo;
  }
  else if (Action(LOWER_MSTORE) && WN_operator_is(child, OPR_MLOAD))
  {
    TY_IDX mloadTY = TY_pointed(Ty_Table[WN_ty(child)]);

    if (TY_is_volatile(mloadTY))
    {
      DevWarn("eval of volatile (mload) seen. I hoped to never see this");
    }
    else if (Action(LOWER_TO_CG) && traceWoptFinishedOpt)
    {
      DevWarn("eval of (mload) processed (wopt should have removed this)");
    }
    block = lower_mload(block, child, actions);
    return block;
  }

  {
    WN	*eval;

    child = lower_expr(block, child, actions);
    eval = WN_CreateEval(child);
    WN_Delete(tree);

    return eval;
  }
  
}



static INT32
calculateLoadStore(INT64 size, INT64 offset, TYPE_ID quantum, WN *src)
{
  INT32 n = (size-offset) / MTYPE_RegisterSize(quantum);

  return WN_operator_is(src, OPR_INTCONST) ? n : 2*n;
}

/* ====================================================================
 *
 * static MSTORE_ACTIONS GenerateMstoreAction(WN *size, INT32 offset, 
 * TYPE_ID quantum, WN *expr)
 * 
 * Generate loop/inline or memory intrinsic code base on size and expr
 * The number of moves is relative to the quantum.
 * Acount for ld/st based on whether expr is constant
 *
 * for size nonconstant
 *	generate loop 
 *	we could generate a call BTW, but based on what criteria ? 
 *
 * for size constant
 *	nMoves >= MinStructCopyMemIntrSize
 *		generate intrinsic based on expr
 *		special case for memset as the expr must be a char.
 *		this means the expr must be same in all n bytes
 *	nMoves >= MinStructCopyLoopSize
 *		generate loop code
 *	else generate aggregate moves
 * 

 * ==================================================================== */
static MSTORE_ACTIONS
GenerateMstoreAction(WN *size, INT32 offset, TYPE_ID quantum, WN *expr)
{
  MSTORE_ACTIONS action;

  INT32 nMoves;
  BOOL sizeIsConstant =  WN_operator_is(size, OPR_INTCONST);

  if (sizeIsConstant)
  {
    nMoves = calculateLoadStore(WN_const_val(size), offset, quantum, expr);
  }

  if (MinStructCopyMemIntrSize	&&
     ((sizeIsConstant==FALSE)		||
      (sizeIsConstant==TRUE && MinStructCopyMemIntrSize <= nMoves)))
  {
    if (WN_operator_is(expr, OPR_INTCONST))
    {
      INT64	val=	WN_const_val(expr);

      if (val == 0)
	action = MSTORE_intrinsic_bzero;
      else
      {
	WN *i1con= WN_I1const(WN_rtype(expr), val);

	action = (val == WN_const_val(i1con)) ? MSTORE_intrinsic_memset
	  : MSTORE_loop;
	WN_Delete(i1con);
      }
    }
    else if (WN_operator_is(expr, OPR_MLOAD))
    {
#ifdef KEY
      int sizenum = WN_const_val(size);
      if (sizeIsConstant &&
	  ((sizenum-offset)%8 == 0 || 
	   (sizenum - offset) > MinStructCopyMemIntrSize * 1.5))
	action = MSTORE_intrinsic_memcpy;
      else
	action = MSTORE_loop;
#else
      action = MSTORE_intrinsic_bcopy;
#endif
    }
    else
    {
      action = MSTORE_loop;
    }
  }
  else if (sizeIsConstant==TRUE		&&
	   MinStructCopyLoopSize	&&
	   MinStructCopyLoopSize <= nMoves)
  {
      action = MSTORE_loop;
  }
  else
  {
    action = (sizeIsConstant) ? MSTORE_aggregate : MSTORE_loop;
  }

  if (traceMload)
  {
    if (sizeIsConstant) {
      DevWarn("GenerateMstoreAction: %s : line %d: quantum %d, "
	      "size %" SCNd64 ", nMoves %d",
	      MSTORE_ACTIONS_name(action),
	      Srcpos_To_Line(current_srcpos),
	      MTYPE_alignment(quantum),
	      WN_const_val(size),
	      nMoves);
    } else {
      DevWarn("GenerateMstoreAction: %s : line %d: quantum %d, "
	      "size unknown",
	      MSTORE_ACTIONS_name(action),
	      Srcpos_To_Line(current_srcpos),
	      MTYPE_alignment(quantum));
    }
  }

  return action;
}

// If the size of the mtype (used in load/store) is the same as the size of 
// the struct, return the struct's type, otherwise, return a predefined
// type corresponding to mtype.
static inline TY_IDX
struct_memop_type (TYPE_ID mtype, TY_IDX struct_type)
{
    if (TY_size (struct_type) != MTYPE_byte_size (mtype))
	Set_TY_IDX_index (struct_type, TY_IDX_index(MTYPE_To_TY (mtype)));
    return struct_type;
}


/* ====================================================================
 *
 * Auxillary routine to copy aggregrate
 * 
 *  Copy size bytes, starting at offset
 * 
 *   There are so many formals that you probably deserve an explanation
 *	WN *block
 *
 *	TY_IDX srcAlign
 *	TY_IDX dstAlign
 *		alignment restrictions of the generated load/stores as cg
 *		needs to know whether to generate unaligned 
 *	INT32 offset
 *		start copying at (byte) offset
 *	INT32 size (or WN *)
 *		struct (byte) size 
 *		for loop code , this is a size expression
 *	TYPE_ID quantum
 *		unit to load/store
 *	ST *preg
 *	PREG_NUM srcPreg
 *	PREG_NUM dstPreg
 *		if srcAlign is NULL
 *		    srcPreg contains an expression (probably zero)
 *		else
 *		    srcPreg contains the address of the src to copy
 *		dstPreg contains the address of the dst to copy
 *		preg should be Int_Preg.
 *	WN *origLoad
 *	WN *origStore
 *		original load/store nodes (or NULL) needed to preserve alias
 *		maps.
 *
 * ==================================================================== */
#ifdef TARG_ST
static void
copy_aggregate(WN *block, TY_IDX srcAlign, TY_IDX dstAlign, INT32 offset,
	       INT32 size, TYPE_ID quantum, ST *preg, PREG_NUM srcPreg,
	       PREG_NUM dstPreg, WN *origLoad, WN *origStore,
	       INT32 copy_alignment, LOWER_ACTIONS actions)
{
  INT32	stride = MTYPE_RegisterSize(quantum);
  INT32	nMoves = size / stride;

  if (size <= 0)
    return;
#ifdef TARG_ST
  if (nMoves>0) {
    if (srcAlign && MinStructCopyParallel > 1 && nMoves > 1) {
      /*
       *  generate  overlapped unrolled load/store
       */
#define MAX_PARALLEL 6
      PREG_NUM pregs[2*MAX_PARALLEL-1];
      INT j, i;
      INT32 l_offset;
      INT32 nBlocks = nMoves / MinStructCopyParallel;
      INT32 residu = nMoves - nBlocks * MinStructCopyParallel;
      INT32 lastSize = nBlocks == 0 ? residu : residu + MinStructCopyParallel;
      if (nBlocks == 0) nBlocks++;
      FmtAssert(MAX_PARALLEL >= MinStructCopyParallel, ("MinStructCopyParallel set too high (%d), maximum is %d", MinStructCopyParallel, MAX_PARALLEL));
      TY_IDX quantum_ty_idx = struct_memop_type (quantum, srcAlign);
      for (j = 0; j < nBlocks; j++) {
	INT overlap = j == nBlocks-1 ? lastSize : MinStructCopyParallel;
	l_offset = offset;
	for (i = 0; i < overlap; i++) {
	  WN *preg_store, *value;
	  pregs[i] = Create_Preg (quantum, NULL);
	  value = WN_IloadLdid(quantum, l_offset, quantum_ty_idx,
			       preg, srcPreg); 
	  lower_copy_maps(origLoad, value, actions);
	  preg_store = WN_StidIntoPreg(quantum, pregs[i], MTYPE_To_PREG(quantum), value);
	  WN_INSERT_BlockLast(block, preg_store);
	  l_offset  += stride;
	}
	l_offset = offset;
	for (i = 0; i < overlap; i++) {
	  WN *store, *preg_load, *addr;
	  preg_load = WN_LdidPreg(quantum, pregs[i]);
	  addr = WN_LdidPreg(Pointer_type, dstPreg);
	  store = WN_Istore(quantum,
			    l_offset,
			    Make_Pointer_Type (quantum_ty_idx),
			    addr,
			    preg_load);
	  lower_copy_maps(origStore, store, actions);
	  WN_INSERT_BlockLast(block, store);
	  l_offset += stride;
	}
	offset  += stride * overlap;
	size -= stride * overlap;
	nMoves -= overlap;
      }
    } else {
      /*
       *  generate  unrolled load/store
       */
      while(nMoves--) {
	/*
	 *  semantics are similar to the following:
	 *      (quantum) *(dst + offset) =   (quantum) *(src + offset);
	 *		or
	 *      (quantum) *(dst + offset) =   srcPreg;
	 */
	WN 	*value, *addr, *store;
	
	if (srcAlign)
	  {
	    value = WN_IloadLdid(quantum, offset,
				 struct_memop_type (quantum, srcAlign),
				 preg, srcPreg);  
	    lower_copy_maps(origLoad, value, actions);
	  }
	else
	  {
	    value = WN_LdidPreg(quantum, srcPreg);
	  }
	
	addr = WN_LdidPreg(Pointer_type, dstPreg);
	
	store = WN_Istore(quantum,
			  offset,
			  Make_Pointer_Type (struct_memop_type (quantum,
							      dstAlign)),
			  addr,
			  value);
	lower_copy_maps(origStore, store, actions);
	
	WN_INSERT_BlockLast(block, store);
	
	offset  += stride;
	size -= stride;
      }
    }
  }
#else
  if (nMoves>0)
  {
   /*
    *  generate  unrolled load/store
    */
    while(nMoves--)
    {
     /*
      *  semantics are similar to the following:
      *      (quantum) *(dst + offset) =   (quantum) *(src + offset);
      *		or
      *      (quantum) *(dst + offset) =   srcPreg;
      */
      WN 	*value, *addr, *store;

      if (srcAlign)
      {
	value = WN_IloadLdid(quantum, offset,
			     struct_memop_type (quantum, srcAlign),
			     preg, srcPreg);  
	lower_copy_maps(origLoad, value, actions);
      }
      else
      {
	value = WN_LdidPreg(quantum, srcPreg);
      }

      addr = WN_LdidPreg(Pointer_type, dstPreg);

      store = WN_Istore(quantum,
			offset,
			Make_Pointer_Type (struct_memop_type (quantum,
							      dstAlign)),
			addr,
			value);
      lower_copy_maps(origStore, store, actions);

#ifdef TARG_ST
      // [TB] Improve line number
      WN_copy_linenum(origStore, store);
#endif
      WN_INSERT_BlockLast(block, store);

      offset  += stride;
      size -= stride;
    }
  }
#endif
  if (size > 0)
  {
   /*
    *  If there is a residue we must recompute a new quantum
    *  and generate a copy for that.
    */
    quantum = compute_next_copy_quantum(quantum, copy_alignment);

    copy_aggregate(block,
		   srcAlign, dstAlign,
		   offset,
		   size,
		   quantum,
		   preg,
		   srcPreg, dstPreg,
		   origLoad, origStore,
		   copy_alignment,
		   actions);
  }
}


#else
static void
copy_aggregate(WN *block, TY_IDX srcAlign, TY_IDX dstAlign, INT32 offset,
	       INT32 size, TYPE_ID quantum, PREG_NUM srcPreg,
	       PREG_NUM dstPreg, WN *origLoad, WN *origStore,
	       INT32 copy_alignment, LOWER_ACTIONS actions)
{
  BOOL unweave = UnweaveCopyForStructs;
  WN *storeBlock = unweave ? WN_CreateBlock() : block;

  while (size > 0) {

    INT32 stride = MTYPE_RegisterSize(quantum);

    for ( ; size >= stride; size -= stride, offset += stride) {
      // Generate unrolled load/store with semantics similar to:
      //   (quantum) *(dst + offset) = (quantum) *(src + offset);
      //         or
      //   (quantum) *(dst + offset) = srcPreg;
      WN *wn_value, *wn_addr, *wn_store;

      rename_preg("mstore_tmp", NULL);
      if (! srcAlign) {
	wn_value = WN_LdidPreg(quantum, srcPreg);
      } else {
	TY_IDX ty_src = struct_memop_type(quantum, srcAlign);
	wn_value = WN_LdidPreg(Pointer_type, srcPreg);
	wn_value = WN_Iload(quantum, offset, ty_src, wn_value);
	lower_copy_maps(origLoad, wn_value, actions);
	if (unweave) {
	  PREG_NUM preg = AssignExpr(block, wn_value, quantum);
	  wn_value = WN_LdidPreg(quantum, preg);
	}
      }

      wn_addr = WN_LdidPreg(Pointer_type, dstPreg);
      TY_IDX ty_dst = struct_memop_type(quantum, dstAlign);
      wn_store = WN_Istore(quantum, offset,
			   Make_Pointer_Type(ty_dst), wn_addr, wn_value);
      lower_copy_maps(origStore, wn_store, actions);
      WN_Set_Linenum(wn_store, current_srcpos);  // Bug 1268
      WN_INSERT_BlockLast(storeBlock, wn_store);
    }

    // If there is a residue we must recompute a new, smaller quantum
    // and generate a copy for that.
    if (size > 0) {
      quantum = compute_next_copy_quantum(quantum, copy_alignment);
    }
  }

  if (unweave) WN_INSERT_BlockLast(block, storeBlock);
}
#endif

#ifdef TARG_ST
static void
copy_element_and_increment(WN *block, TY_IDX srcAlign, TY_IDX dstAlign,
			   PREG_NUM offsetN, TYPE_ID quantum, PREG_NUM srcPreg,
			   PREG_NUM dstPreg, WN *origLoad, WN *origStore,
			   LOWER_ACTIONS actions)
{
 /*
  *   (quantum) *(dst + offset) =   (quantum) *(src + offset);
  *		or
  *   (quantum) *(dst + offset) =   srcPreg;
  *    		
  *  and increment offset
  *
  *		offset		+=  stride;
  */
  INT32  stride = MTYPE_RegisterSize(quantum);
  WN 	*value, *addr, *store, *add, *inc;
#ifdef TARG_ST
  // Arthur: calculations involving stride and offset are done in
  //         pointer type preg.
  ST	*ptrPreg = MTYPE_To_PREG(Pointer_type);
#else
  ST	*intPreg = MTYPE_To_PREG(Integer_type);
#endif

  if (srcAlign)
  {
#ifdef TARG_ST
    // Arthur: it only worked because int and pointer were the
    //         same types.
    addr = WN_Add(Pointer_type,
		  WN_LdidPreg(Pointer_type, srcPreg),
		  WN_LdidPreg(Pointer_type, offsetN));
#else
    addr = WN_Add(Pointer_type,
		  WN_LdidPreg(Pointer_type, srcPreg),
		  WN_LdidPreg(Integer_type, offsetN));
#endif
    value = WN_CreateIload (OPR_ILOAD, Mtype_comparison(quantum), quantum,
			    0, struct_memop_type (quantum, srcAlign),
			    Make_Pointer_Type (srcAlign), addr);

    lower_copy_maps(origLoad, value, actions);
  }
  else
  {
    value = WN_LdidPreg(quantum, srcPreg);
  }

#ifdef TARG_ST
  // Arthur: it only worked because int and pointer
  //         was the same type.
  addr = WN_Add(Pointer_type,
		WN_LdidPreg(Pointer_type, dstPreg),
		WN_LdidPreg(Pointer_type, offsetN));
#else
  addr = WN_Add(Pointer_type,
		WN_LdidPreg(Pointer_type, dstPreg),
		WN_LdidPreg(Integer_type, offsetN));
#endif
  store = WN_Istore(quantum,
		    0,
		    Make_Pointer_Type(struct_memop_type (quantum, dstAlign)),
		    addr,
		    value);

  lower_copy_maps(origStore, store, actions);

#ifdef TARG_ST
  // [TB] Improve line number
  WN_copy_linenum(origStore, store);
#endif
  WN_INSERT_BlockLast(block, store);

  /*
   *  offset += stride
   */
#ifdef TARG_ST
  add  = WN_Add(Pointer_type,
 		WN_LdidPreg(Pointer_type, offsetN),
  		WN_Intconst(Pointer_type, stride));
  inc = WN_StidIntoPreg(Pointer_type, offsetN, ptrPreg, add);
#else
  add  = WN_Add(Integer_type,
 		WN_LdidPreg(Integer_type, offsetN),
  		WN_Intconst(Integer_type, stride));
  inc = WN_StidIntoPreg(Integer_type, offsetN, intPreg, add);
#endif
#ifdef TARG_ST
  // [TB] Improve line number
  WN_copy_linenum(origStore, inc);
#endif
  WN_INSERT_BlockLast(block, inc);
}
#else
/* ====================================================================
 *
 * Auxillary routine used by copy_aggregate_loop_const and
 * copy_aggregate_loop_n
 *
 * ==================================================================== */

static void
copy_element_and_increment(WN *block, TY_IDX srcAlign, TY_IDX dstAlign,
			   INT32 offset, INT32 size, TYPE_ID quantum,
			   PREG_NUM srcPreg, PREG_NUM dstPreg,
			   WN *origLoad, WN *origStore,
			   INT32 copy_alignment, LOWER_ACTIONS actions)
{
  // Generate unrolled load/stores to perform copy
  copy_aggregate(block, srcAlign, dstAlign, offset, size, quantum,
		 srcPreg, dstPreg, origLoad, origStore,
		 copy_alignment, actions);

  // Increment: src += size;
  WN *wn;
  ST *pointerPreg = MTYPE_To_PREG(Pointer_type);
  if (srcAlign) {
    wn = WN_Add(Pointer_type,
		WN_LdidPreg(Pointer_type, srcPreg),
		WN_Intconst(Integer_type, size));
    wn = WN_StidIntoPreg(Pointer_type, srcPreg, pointerPreg, wn);
    WN_Set_Linenum(wn, current_srcpos);  // Bug 1268
    WN_INSERT_BlockLast(block, wn);
  }

  // Increment: dst += size;
  wn = WN_Add(Pointer_type,
	      WN_LdidPreg(Pointer_type, dstPreg),
	      WN_Intconst(Integer_type, size));
  wn = WN_StidIntoPreg(Pointer_type, dstPreg, pointerPreg, wn);
  WN_Set_Linenum(wn, current_srcpos);  // Bug 1268
  WN_INSERT_BlockLast(block, wn);
}
#endif

#ifdef TARG_ST
static void
copy_aggregate_loop_const(WN *block, 
                          TY_IDX srcAlign, 
                          TY_IDX dstAlign,
			  INT32 offset, 
                          INT32 size, 
                          TYPE_ID quantum,
			  PREG_NUM srcPreg, 
                          PREG_NUM dstPreg, 
                          WN *origLoad,
			  WN *origStore, 
                          INT32 copy_alignment,
			  LOWER_ACTIONS actions)
{
 /*
  *  generate the following
  *    n = nMoves;
  *    index = 0;
  *    do
  *    {
  *	(quantum) *(dst + offset) =   (quantum) *(src + offset);
  *	       n--;
  *    } while(n>0);
  *
  * Try generating a DO loop instead of a WHILE loop
  *
  *	(TBD)	we should really build an array expression 
  *		dst[offset] = src[offset]
  */
  PREG_NUM		offsetN;
  ST		*intPreg = MTYPE_To_PREG(Integer_type);
  INT32		stride   = MTYPE_RegisterSize(quantum);
  INT64		nMoves   = size  / stride;
  INT64		residue  = size - ( nMoves * stride );

  /*
   *  Bail out if there is nothing to move and no residue
   */
  if ((nMoves <= 0) && residue == 0)
    return;
#ifdef TARG_ST
  // Arthur: address calculations should be conducted over the
  //         pointer types. The fact that the pointer type maps
  //         on an int on a given target is not a good reason 
  //         to use integer type here.
  offsetN = AssignExpr(block, WN_Intconst(Pointer_type, offset), Pointer_type);
#else
  offsetN = AssignExpr(block, WN_Intconst(Integer_type, offset), Integer_type);
#endif

 /*
  *	create loop count variable, traditionally called n
  * 	offset is most likely zero
  */
  if (nMoves > 0)
  {
    PREG_NUM	n;
    WN	 	*body;
    WN          *incr;
    WN          *start;
    WN          *test;
    WN	        *doLoop;

    n = Create_Preg(Integer_type,"mstore_loopcount");
    body= WN_CreateBlock();
    start = WN_StidIntoPreg( Integer_type, n, intPreg,
			     WN_Intconst(Integer_type, nMoves));
    incr  = WN_StidIntoPreg( Integer_type, n, intPreg,
			     WN_Sub(Integer_type,
				    WN_LdidPreg(Integer_type, n),
				    WN_Intconst(Integer_type, 1)));
    test = WN_GT(Integer_type,
		 WN_LdidPreg(Integer_type, n),
		 WN_Zerocon(Integer_type));
    
   /*
    *   (quantum) *(dst + offset) =   (quantum) *(src + offset);
    *		or
    *   (quantum) *(dst + offset) =   srcPreg;
    *   and increment offset
    *			offset    +=  stride;
    */
    copy_element_and_increment(body,
			       srcAlign, dstAlign,
			       offsetN,
			       quantum,
			       srcPreg, dstPreg,
			       origLoad, origStore,
			       actions);
    doLoop = WN_CreateDO(WN_CreateIdname(n, intPreg),
			 start, test, incr, body, NULL);
#ifdef TARG_ST
    // [TB] Improve line number
    WN_copy_linenum(origStore, doLoop);
#endif
    WN_INSERT_BlockLast(block, doLoop);
    if ( Cur_PU_Feedback && origStore) 
#ifdef TARG_ST
      //TB: fix bug #52831 Sometimes copy_aggregate_loop() is call
      //with a PARM WN instead of a MSTORE (origStore parameter). I
      //think this has been done to fix a bug. I can't find why. Search
      //for TBwhy patterm to find where this is done.
      if (WN_operator( origStore ) != OPR_PARM)
#endif
	Cur_PU_Feedback->FB_lower_mstore_to_loop( origStore, doLoop, nMoves );
  }

 /*
  *  If there is a residue we must recompute a new quantum
  *  and generate a copy for that.
  *
  *	if (residue > 0)
  *	{
  *	    if (residue >= 4)
  *	    {
  *  		(int) *(dst + offset) =   (int) *(src + offset);
  *		offset  += 4;
  *		residue -= 4;
  *	    }
  *	    if (residue >= 2)
  *	    {
  *  		 (short) *(dst + offset) =   (short) *(src + offset);
  *		offset  += 2;
  *		residue -= 2;
  *	    }
  *	    etc.
  *	}
  */
  if (residue)
  {
    WN	  *residue_block=  WN_CreateBlock();

    while(residue>0)
    {
      quantum = compute_next_copy_quantum(quantum, copy_alignment);

      while (residue >= MTYPE_alignment(quantum))
      {
	copy_element_and_increment(residue_block, srcAlign, dstAlign,
				   offsetN, quantum, srcPreg, dstPreg,
				   origLoad, origStore, actions);
	residue -= MTYPE_alignment(quantum);
      }
    }
#ifdef TARG_ST
    // [TB] Improve line number
    WN_copy_linenum(origStore, residue_block);
#endif

    WN_INSERT_BlockLast(block, residue_block);
  }
}
#else
/* ====================================================================
 *
 * Auxillary routine to copy aggregrate loop 
 *
 *	The size must be an integer constant
 *
 * ==================================================================== */
static void
copy_aggregate_loop_const(WN *block, 
                          TY_IDX srcAlign,
                          TY_IDX dstAlign,
			  INT32 offset, 
                          INT32 size, 
                          TYPE_ID quantum,
			  PREG_NUM srcPregInit,
                          PREG_NUM dstPregInit,
			  WN *origLoad, 
                          WN *origStore,
			  INT32 copy_alignment, 
                          LOWER_ACTIONS actions)
{
  // Generate the following loop:
  //   src = srcInit + offset;
  //   dst = dstInit + offset;
  //   for (n = nMoves; n > 0; --n) {
  //     (quantum) *(dst) = (quantum) *(src);
  //     (quantum) *(dst + stride) = (quantum) *(src + stride);
  //     ...
  //     (quantum) *(dst + (k-1)* stride) = (quantum) *(src + (k-1)* stride);
  //     src += bigstride;
  //     dst += bigstride;
  //   }

  WN *wn;
  ST   *intPreg = MTYPE_To_PREG(Integer_type);
  INT32 stride  = MTYPE_RegisterSize(quantum);
  INT32 bigstride = Aggregate_UnrollFactor * stride;
  INT64 nMoves  = size / bigstride;
  INT64 residue = size - ( nMoves * bigstride );
  PREG_NUM srcPreg, dstPreg;

  // Bail out if there is nothing to move and no residue
  if (nMoves <= 0 && residue == 0)
    return;

  // src = srcInit + offset;
  rename_preg("mstore_src", NULL);
  if (srcAlign == 0) {
    // srcPreg is single value, not pointer to values
    srcPreg = srcPregInit;
  } else {
    wn = WN_LdidPreg( Pointer_type, srcPregInit );
    if (offset != 0) {
      wn = WN_Add( Pointer_type, wn,
		   WN_Intconst(Integer_type, offset) );
    }
    srcPreg = AssignExpr( block, wn, Pointer_type );
  }

  // dst = dstInit + offset;
  rename_preg("mstore_dst", NULL);
  wn = WN_LdidPreg( Pointer_type, dstPregInit );
  if (offset != 0) {
    wn = WN_Add( Pointer_type, wn,
		 WN_Intconst(Integer_type, offset) );
  }
  dstPreg = AssignExpr( block, wn, Pointer_type );
  rename_reset();

  if (nMoves > 0)
  {
    // create loop count variable, traditionally called n
    PREG_NUM n;
    WN *body, *incr, *start, *test, *doLoop;
    n = Create_Preg( Integer_type, "mstore_loopcount" );
    body = WN_CreateBlock();
    start = WN_StidIntoPreg( Integer_type, n, intPreg,
			     WN_Intconst(Integer_type, nMoves) );
    incr  = WN_StidIntoPreg( Integer_type, n, intPreg,
			     WN_Sub(Integer_type,
				    WN_LdidPreg(Integer_type, n),
				    WN_Intconst(Integer_type, 1)) );
    test = WN_GT( Integer_type,
		  WN_LdidPreg(Integer_type, n),
		  WN_Zerocon(Integer_type) );

    // Generate body of loop
    copy_element_and_increment( body, srcAlign, dstAlign, 0 /*offset*/,
				bigstride, quantum,
				srcPreg, dstPreg, origLoad, origStore,
				copy_alignment, actions );

    doLoop = WN_CreateDO( WN_CreateIdname(n, intPreg),
			  start, test, incr, body, NULL );
    WN_Set_Linenum(doLoop, current_srcpos);  // Bug 1268
    WN_INSERT_BlockLast(block, doLoop);
    if ( Cur_PU_Feedback && (origStore != NULL) )
      Cur_PU_Feedback->FB_lower_mstore_to_loop( origStore, doLoop, nMoves );
  }

  // Generate code to copy any residue.  Example for residue of size 7:
  //	(int) *(dst) = (int) *(src);
  //	(short) *(dst + 4) = (short) *(src + 4);
  //	(char) *(dst + 6) = (char) *(src + 6);
  if (residue > 0) {
    WN *residue_block = WN_CreateBlock();
    copy_aggregate(residue_block, srcAlign, dstAlign, 0 /*offset*/,
		   residue /*size*/, quantum, srcPreg, dstPreg,
		   origLoad, origStore, copy_alignment, actions);
    WN_INSERT_BlockLast(block, residue_block);
  }
}
#endif


/* ====================================================================
 *
 * Auxillary routine to copy aggregrate loop 
 *
 *	The size must be an expression
 *
 * ==================================================================== */
#ifdef TARG_ST
static void
copy_aggregate_loop_n(WN *block, TY_IDX srcAlign, TY_IDX dstAlign,
		      INT32 offset, WN *size, TYPE_ID quantum,
		      PREG_NUM srcPreg, PREG_NUM dstPreg, WN *origLoad,
		      WN *origStore, INT32 copy_alignment,
		      LOWER_ACTIONS actions)
{
 /*
  *  generate the following
  *    n = nMoves;
  *    index = 0;
  *    while(n>0)
  *    {
  *	(quantum) *(dst + offset) =   (quantum) *(src + offset);
  *	       n--;
  *    }
  *
  *	(TBD)	we should really build an array expression 
  *		dst[offset] = src[offset]
  */
  PREG_NUM	offsetN, nMovesN, residueN;
  ST		*intPreg = MTYPE_To_PREG(Integer_type);
  INT32		stride   = MTYPE_RegisterSize(quantum);

  Is_True((WN_operator(size)!=OPR_INTCONST),("unexpected const"));

 /*
  *  Create the following expressions
  *
  *	nMovesn = ( size ) / stride
  *
  *	residue =   size - ( nMoves * stride )
  */
  {
    WN  *nMoves, *residue;

    nMoves   = WN_Div(Integer_type, WN_COPY_Tree(size),
		      WN_Intconst(Integer_type, stride));

    nMovesN  = AssignExpr(block, nMoves, Integer_type);

    residue  = WN_Sub(Integer_type,
		      WN_COPY_Tree(size),
		      WN_Mpy(Integer_type,
			     WN_LdidPreg(Integer_type, nMovesN),
			     WN_Intconst(Integer_type, stride)));

    residueN = AssignExpr(block, residue, Integer_type);
  }

 /*
  *	create loop count variable, traditionally called n
  * 	offset is most likely zero
  */
#if 0
  n = AssignExpr(block, WN_LdidPreg(Integer_type, nMovesN), Integer_type);

  offsetN = AssignExpr(block, WN_Intconst(Integer_type, offset), Integer_type);

  {
    WN	*body, *sub, *dec, *test, *whileDo;

    body = WN_CreateBlock();
   /*
    *	while(n>0)
    *   {
    *
    *	  (quantum) *(dst + offset) =   (quantum) *(src + offset);
    *		or
    *	  (quantum) *(dst + offset) =   srcPreg;
    *
    *			offset    +=  stride;
    *
    * 			n --;
    *   }
    */
    copy_element_and_increment(body,
			       srcAlign, dstAlign,
			       offsetN, quantum,
			       srcPreg, dstPreg,
			       origLoad, origStore,
			       actions);

    sub  = WN_Sub(Integer_type,
		  WN_LdidPreg(Integer_type, n),
		  WN_Intconst(Integer_type, 1));
    dec  = WN_StidIntoPreg(Integer_type, n, intPreg, sub);

#ifdef TARG_ST
    // [TB] Improve line number
    WN_copy_linenum(tree, dec);
#endif
    WN_INSERT_BlockLast(body, dec);


    test = WN_GT(Integer_type,
		 WN_LdidPreg(Integer_type, n),
		 WN_Zerocon(Integer_type));
  	
    whileDo = WN_CreateWhileDo(test, body);
    // ADD FEEDBACK INFO !!!
#ifdef TARG_ST
    // [TB] Improve line number
    WN_copy_linenum(tree, whileDo);
#endif
    WN_INSERT_BlockLast(block, whileDo);
  }
#else
  {
    PREG_NUM	n;
    WN	 	*body;
    WN          *incr;
    WN          *start;
    WN          *test;
    WN	        *doLoop;

    n = Create_Preg(Integer_type,"mstore_loopcount");
#ifdef TARG_ST
    offsetN = AssignExpr(block, WN_Intconst(Pointer_type, offset),
			 Pointer_type);
#else
    offsetN = AssignExpr(block, WN_Intconst(Integer_type, offset),
			 Integer_type);
#endif
    body= WN_CreateBlock();
    start = WN_StidIntoPreg(Integer_type, n, intPreg,
			    WN_LdidPreg(Integer_type, nMovesN));
    incr  = WN_StidIntoPreg(Integer_type, n, intPreg,
			    WN_Sub(Integer_type,
				   WN_LdidPreg(Integer_type, n),
				   WN_Intconst(Integer_type, 1)));
    test = WN_GT(Integer_type,
		 WN_LdidPreg(Integer_type, n),
		 WN_Zerocon(Integer_type));
    
   /*
    *   (quantum) *(dst + offset) =   (quantum) *(src + offset);
    *		or
    *   (quantum) *(dst + offset) =   srcPreg;
    *   and increment offset
    *			offset    +=  stride;
    */
    copy_element_and_increment(body,
			       srcAlign, dstAlign,
			       offsetN,
			       quantum,
			       srcPreg, dstPreg,
			       origLoad, origStore,
			       actions);
    doLoop = WN_CreateDO(WN_CreateIdname(n,intPreg),start,test,incr,body,NULL);
#ifdef TARG_ST
    // [TB] Improve line number
    WN_copy_linenum(origStore, doLoop);
#endif
    WN_INSERT_BlockLast(block, doLoop);
  }
  
#endif





 /*
  *  If there is a residue we must recompute a new quantum
  *  and generate a copy for that.
  *
  *	if (residue > 0)
  *	{
  *	    if (residue >= 4)
  *	    {
  *  		(int) *(dst + offset) =   (int) *(src + offset);
  *		offset  += 4;
  *		residue -= 4;
  *	    }
  *	    if (residue >= 2)
  *	    {
  *  		 (short) *(dst + offset) =   (short) *(src + offset);
  *		offset  += 2;
  *		residue -= 2;
  *	    }
  *	    etc.
  *	}
  *   We supply an incorrect alignment to compute_next_copy_quantum()
  *   because we cant skip the unaligned halfword 
  */
  {
    WN	*if_then, *unused, *cond, *IF;

    if_then = WN_CreateBlock();
    unused = WN_CreateBlock();	/* will be empty */

    cond = WN_GT(Integer_type,
		 WN_LdidPreg(Integer_type, residueN),
		 WN_Zerocon(Integer_type));

    IF = WN_CreateIf(cond, if_then, unused);
    // ADD FEEDBACK INFO !!

    quantum = compute_next_copy_quantum(quantum,
					MTYPE_alignment(Max_Uint_Mtype));
  	
    while(MTYPE_alignment(quantum) > 0)
    {
      WN	*if_residue_block, *unused, *test, *IF_residue;


      if_residue_block = WN_CreateBlock();
      unused = WN_CreateBlock();

      test = WN_GE(Integer_type,
		   WN_LdidPreg(Integer_type, residueN),
		   WN_Intconst(Integer_type, MTYPE_alignment(quantum)));

      IF_residue = WN_CreateIf(test, if_residue_block, unused);
      // ADD FEEDBACK INFO !!
      lower_copy_maps(origStore, IF_residue, actions);
	
      copy_element_and_increment(if_residue_block, srcAlign, dstAlign,
				 offsetN, quantum, srcPreg, dstPreg,
				 origLoad, origStore, actions);

     /*
      *  residue -= stride
      */
      {
	WN	*sub, *dec;

	sub  = WN_Sub(Integer_type,
		      WN_LdidPreg(Integer_type, residueN),
		      WN_Intconst(Integer_type, MTYPE_alignment(quantum)));
	dec = WN_StidIntoPreg(Integer_type, residueN, intPreg, sub);
        lower_copy_maps(origStore, dec, actions);
#ifdef TARG_ST
	// [TB] Improve line number
	WN_copy_linenum(origStore, dec);
#endif
	WN_INSERT_BlockLast(if_residue_block, dec);
      }

#ifdef TARG_ST
      // [TB] Improve line number.
      WN_copy_linenum(origStore, IF_residue);
#endif
      WN_INSERT_BlockLast(if_then, IF_residue);

      quantum = compute_next_copy_quantum(quantum,
					  MTYPE_alignment(Max_Uint_Mtype));
    }
#ifdef TARG_ST
    // [TB] Improve line number.
    WN_copy_linenum(origStore, IF);
#endif
    WN_INSERT_BlockLast(block, IF);
  }
}
#else
static void
copy_aggregate_loop_n(WN *block, TY_IDX srcAlign, TY_IDX dstAlign,
		      INT32 offset, WN *size, TYPE_ID quantum,
		      PREG_NUM srcPregInit, PREG_NUM dstPregInit,
		      WN *origLoad, WN *origStore,
		      INT32 copy_alignment, LOWER_ACTIONS actions)
{
  // Generate the following loop:
  //   n = nMoves;
  //   src = srcInit + offset;
  //   dst = dstInit + offset;
  //   while (n > 0) {
  //     (quantum) *(dst) = (quantum) *(src);
  //     src += stride;  dst += stride;
  //	 n--;
  //   }

  WN       *wn;
  PREG_NUM  nMovesN, residueN, srcPreg, dstPreg;
  ST	   *intPreg = MTYPE_To_PREG(Integer_type);
  INT32     stride  = MTYPE_RegisterSize(quantum);

  Is_True( (WN_operator(size) != OPR_INTCONST), ("unexpected const") );

  // src = srcInit + offset;
  rename_preg("mstore_src", NULL);
  if (srcAlign == 0) {
    // srcPreg is single value, not pointer to values
    srcPreg = srcPregInit;
  } else {
    wn = WN_LdidPreg( Pointer_type, srcPregInit );
    if (offset != 0) {
      wn = WN_Add( Pointer_type, wn,
		   WN_Intconst(Integer_type, offset) );
    }
    srcPreg = AssignExpr( block, wn, Pointer_type );
    // TODO: Fix WN_linenum
  }

  // dst = dstInit + offset;
  rename_preg("mstore_dst", NULL);
  wn = WN_LdidPreg( Pointer_type, dstPregInit );
  if (offset != 0) {
    wn = WN_Add( Pointer_type, wn,
		 WN_Intconst(Integer_type, offset) );
  }
  dstPreg = AssignExpr( block, wn, Pointer_type );
  // TODO: Fix WN_linenum
  rename_reset();

  // nMovesN = ( size ) / stride
  wn = WN_Div( Integer_type, WN_COPY_Tree(size),
	       WN_Intconst(Integer_type, stride) );
  nMovesN = AssignExpr( block, wn, Integer_type );

  // residueN = size - ( nMoves * stride )
  wn = WN_Sub( Integer_type, WN_COPY_Tree(size),
	       WN_Mpy( Integer_type,
		       WN_LdidPreg(Integer_type, nMovesN),
		       WN_Intconst(Integer_type, stride) ) );
  residueN = AssignExpr( block, wn, Integer_type );

  // create loop count variable, traditionally called n
  PREG_NUM n = Create_Preg( Integer_type, "mstore_loopcount" );

  WN *body, *incr, *start, *test, *doLoop;
  body = WN_CreateBlock();
  start = WN_StidIntoPreg( Integer_type, n, intPreg,
			   WN_LdidPreg(Integer_type, nMovesN) );
  incr  = WN_StidIntoPreg( Integer_type, n, intPreg,
			   WN_Sub( Integer_type,
				   WN_LdidPreg(Integer_type, n),
				   WN_Intconst(Integer_type, 1) ) );
  test = WN_GT( Integer_type,
		WN_LdidPreg(Integer_type, n),
		WN_Zerocon(Integer_type) );

  // (quantum) *(dst) = (quantum) *(src);  OR  (quantum) *(dst) = src;
  // then increment src and dst:  srcPreg += stride;  dstPreg += stride;
  copy_element_and_increment( body, srcAlign, dstAlign, 0 /*offset*/,
			      stride, quantum, srcPreg, dstPreg,
			      origLoad, origStore,
			      copy_alignment, actions);

  doLoop = WN_CreateDO( WN_CreateIdname(n, intPreg),
			start, test, incr, body, NULL );
  WN_Set_Linenum(doLoop, current_srcpos);  // Bug 1268
  WN_INSERT_BlockLast( block, doLoop );

  // If there is a residue we must recompute a new quantum
  // and generate a copy for that.
  //
  //	if (residue > 0) {
  //	    if (residue >= 4) {
  //		(int) *(dst) = (int) *(src);
  //		dst += 4;  src += 4;
  //		residue -= 4;
  //	    }
  //	    if (residue >= 2) {
  //		(short) *(dst) = (short) *(src);
  //		dst += 2;  src += 2;
  //		residue -= 2;
  //	    }
  //	    if (residue >= 1) {
  //		(short) *(dst) = (short) *(src);
  //		dst += 1;  src += 1;
  //		residue -= 1;
  //	    }
  //	}
  //
  // We supply an incorrect alignment to compute_next_copy_quantum()
  // because we cant skip the unaligned halfword

  // Collect residue checks into block_checks
  WN *block_checks = WN_CreateBlock();
  quantum = compute_next_copy_quantum( quantum,
				       MTYPE_alignment(Max_Uint_Mtype) );
  while (MTYPE_alignment(quantum) > 0) {

    // Block for this residue load/store
    WN *block_residue = WN_CreateBlock();

    // (quantum) *(dst) = (quantum) *(src);  OR  (quantum) *(dst) = src;
    // then increment src and dst:  srcPreg += stride;  dstPreg += stride;
    INT32 stride = MTYPE_RegisterSize(quantum);
    copy_element_and_increment( block_residue, srcAlign, dstAlign,
				0 /*offset*/, stride, quantum,
				srcPreg, dstPreg, origLoad, origStore,
				copy_alignment, actions );

    // residue -= stride
    wn = WN_Sub( Integer_type,
		 WN_LdidPreg( Integer_type, residueN ),
		 WN_Intconst( Integer_type, MTYPE_alignment(quantum) ) );
    wn = WN_StidIntoPreg( Integer_type, residueN, intPreg, wn );
    lower_copy_maps( origStore, wn, actions );
    WN_Set_Linenum(wn, current_srcpos);  // Bug 1268
    WN_INSERT_BlockLast( block_residue, wn );

    // Wrap "if (residue >= quantum)" around residue_block
    wn = WN_GE( Integer_type,
		WN_LdidPreg( Integer_type, residueN ),
		WN_Intconst( Integer_type, MTYPE_alignment(quantum) ) );
    wn = WN_CreateIf( wn, block_residue, WN_CreateBlock() );
    // ADD FEEDBACK INFO !!
    lower_copy_maps( origStore, wn, actions );
    WN_Set_Linenum(wn, current_srcpos);  // Bug 1268
    WN_INSERT_BlockLast( block_checks, wn );

    quantum = compute_next_copy_quantum( quantum,
					 MTYPE_alignment(Max_Uint_Mtype) );
  }

  // Wrap "if (residue >= 0)" around all residue checks
  wn = WN_GT( Integer_type, WN_LdidPreg( Integer_type, residueN ),
	      WN_Zerocon(Integer_type) );
  wn = WN_CreateIf( wn, block_checks, WN_CreateBlock() );
  // ADD FEEDBACK INFO !!
  WN_Set_Linenum(wn, current_srcpos);  // Bug 1268
  WN_INSERT_BlockLast( block, wn );
}
#endif

/* ====================================================================
 *
 * Auxillary routine to copy aggregrate (loop version)
 *
 *  The size can be an expression or integer constant of any value
 *  so it must be tested
 *
 *  loop_const:	will generate a
 *		n= CONST;	do {} while(n<0);
 *
 *  loop_n:	will generate a
 *		n= size;	while(n<0) {};
 *
 * ==================================================================== */

static void
copy_aggregate_loop(WN *block, 
                    TY_IDX srcTY, 
                    TY_IDX dstTY, 
                    INT32 offset,
		    WN *size, 
                    TYPE_ID quantum, 
#ifdef TARG_ST
                    ST *preg, 
#endif
                    PREG_NUM srcPreg,
		    PREG_NUM dstPreg,
                    WN *origLoad, 
                    WN *origStore,
		    INT32 copy_alignment,
                    LOWER_ACTIONS actions)
{
  if (WN_operator_is(size, OPR_INTCONST))
  {
    if (WN_const_val(size)>0)
    {
      copy_aggregate_loop_const(block, 
                                srcTY, 
                                dstTY,
                                offset,
				WN_const_val(size), 
                                quantum,
				srcPreg, 
                                dstPreg, 
                                origLoad, 
                                origStore,
				copy_alignment, 
                                actions);
    }
  }
  else
  {
    copy_aggregate_loop_n(block, 
                          srcTY, 
                          dstTY, 
                          offset, 
                          size, 
                          quantum,
			  srcPreg, 
                          dstPreg, 
                          origLoad, 
                          origStore,
			  copy_alignment, 
                          actions);
  }
}

/*
 *  mark LDA as addr taken passed for intrinsic memory routines.
 *  Do not recurse on ILOAD (it would mark the pointer as taken_passed
 *  and not the object)
 *  14979: Must also mark addr_saved.  Can we avoid this if we fix WOPT?
 */
static void markAddrTakenPassed(WN *expr)
{
  switch(WN_operator(expr))
  {
  case OPR_LDA:
    {
      ST *sym = WN_st(expr);
#ifndef TARG_ST
      //if TARG_ST
      //   Do nothing here. ADDR flags should only grow more
      //   optimistic; they should never become more conservative,
      //   because the program's semantics cannot grow worse as we
      //   compile it.
      //endif

      if (ST_class(sym) == CLASS_VAR || ST_class(sym) == CLASS_FUNC) {
	Set_ST_addr_passed(sym);  // 14795
	Set_ST_addr_saved(sym);   // 14979
      }
#endif
    }
    return;
  case OPR_ILOAD:
  case OPR_ILOADX:
    return;
  }

  {
    INT32	i;
    for (i = 0; i < WN_kid_count(expr); i++)
      markAddrTakenPassed(WN_actual(expr,i));
  }
}


/* ====================================================================
 * 
 * Generate a call to an intrinsic -- NOT an intrinsic!
 * (currently from lower_mstore)
 *
 * ==================================================================== */

inline WN *WN_Generate_Intrinsic_Call(WN *block, INTRINSIC id, 
				      INT32 nparm, WN *parm[])
{
  WN *call = WN_Create_Intrinsic(OPC_VINTRINSIC_CALL, id, nparm, parm);
  return intrinsic_runtime(block, call);
}


static void copy_aggregate_bzero(WN *block, TY_IDX dstTY, WN *dst, WN *size)
{
  WN  *call, *parms[2];

  markAddrTakenPassed(dst);

  parms[0]=  WN_CreateParm(Pointer_type,
			   dst,
			   dstTY,
			   WN_PARM_BY_VALUE);

  parms[1]=  WN_CreateParm(WN_rtype(size),
			   WN_COPY_Tree(size),
			   MTYPE_To_TY(WN_rtype(size)),
			   WN_PARM_BY_VALUE);

  call=	WN_Generate_Intrinsic_Call(block, INTRN_BZERO, 2, parms);
  WN_Set_Linenum(call, current_srcpos);  // Bug 1268
  WN_INSERT_BlockLast(block, call);
}

static void copy_aggregate_memset(WN *block, TY_IDX dstTY, WN *src, WN *dst,
				  WN *size)
{
  WN  *call, *parms[3];

  markAddrTakenPassed(dst);
  parms[0]=  WN_CreateParm(Pointer_type,
			   dst,
			   dstTY,
			   WN_PARM_BY_VALUE);

  parms[1]=  WN_CreateParm(WN_rtype(src),
			   src,
			   MTYPE_To_TY(WN_rtype(src)),
			   WN_PARM_BY_VALUE);

  parms[2]=  WN_CreateParm(WN_rtype(size),
			   WN_COPY_Tree(size),
			   MTYPE_To_TY(WN_rtype(size)),
			   WN_PARM_BY_VALUE);

  call=	WN_Generate_Intrinsic_Call(block, INTRN_MEMSET, 3, parms);
  WN_Set_Linenum(call, current_srcpos);  // Bug 1268
  WN_INSERT_BlockLast(block, call);
}

#ifdef KEY
static void copy_aggregate_memcpy(WN *block, INT32 offset, TY_IDX srcTY,
				  TY_IDX dstTY, WN *src, WN *dst, WN *size)
{
  WN  *call, *parms[3];
  
  if (offset)
    {
      src = WN_Add(Pointer_type, src, WN_Intconst(Pointer_type, offset));
      dst = WN_Add(Pointer_type, dst, WN_Intconst(Pointer_type, offset));
    }
  markAddrTakenPassed(src);
  markAddrTakenPassed(dst);
  
  parms[0]=  WN_CreateParm(Pointer_type,
                           dst,
                           dstTY ? dstTY : MTYPE_To_TY(WN_rtype(dst)),
                           WN_PARM_BY_VALUE);
  
  parms[1]=  WN_CreateParm(WN_rtype(src),
                           src,
                           srcTY ? srcTY : MTYPE_To_TY(WN_rtype(src)),
                           WN_PARM_BY_VALUE);
  
  parms[2]=  WN_CreateParm(WN_rtype(size),
                           WN_COPY_Tree(size),
                           MTYPE_To_TY(WN_rtype(size)),
                           WN_PARM_BY_VALUE);
  
  call= WN_Generate_Intrinsic_Call(block, INTRN_MEMCPY, 3, parms);
  WN_Set_Linenum(call, current_srcpos);  // Bug 1268
  WN_INSERT_BlockLast(block, call);
}
			   
#endif
#ifndef TARG_ST
static void copy_aggregate_bcopy(WN *block, INT32 offset, TY_IDX srcTY,
				 TY_IDX dstTY, WN *src, WN *dst, WN *size)
{
  WN  *call, *parms[3];

  if (offset)
  {
    src = WN_Add(Pointer_type, src, WN_Intconst(Pointer_type, offset));
    dst = WN_Add(Pointer_type, dst, WN_Intconst(Pointer_type, offset));
  }
  markAddrTakenPassed(src);
  markAddrTakenPassed(dst);

  parms[0]=  WN_CreateParm(WN_rtype(src),
			   src,
			   srcTY ? srcTY : MTYPE_To_TY(WN_rtype(src)),
			   WN_PARM_BY_VALUE);

  parms[1]=  WN_CreateParm(Pointer_type,
			   dst,
			   dstTY ? dstTY : MTYPE_To_TY(WN_rtype(dst)),
			   WN_PARM_BY_VALUE);


  parms[2]=  WN_CreateParm(WN_rtype(size),
			   WN_COPY_Tree(size),
			   MTYPE_To_TY(WN_rtype(size)),
			   WN_PARM_BY_VALUE);

  call=	WN_Generate_Intrinsic_Call(block, INTRN_BCOPY, 3, parms);
  WN_Set_Linenum(call, current_srcpos);  // Bug 1268
  WN_INSERT_BlockLast(block, call);
}
#endif



/* ====================================================================
 *
 * These routine compute alignment used by the MSTORE/MLOAD routines
 *
 *	aligned memory access
 *		return minimum required alignment
 *
 *	otherwise
 *		form a ratio based on align vrs unaligned reference.
 *		Each unaligned ld/st requires 2 instructions.
 *		In addition, some processors may support 2 memory operations
 *		per clock, but only one unaligned reference (r8k and alien)
 *
 * When we can use unaligned quantums, do not return halfword
 * quantums as there is no hardware support (pv544367)
 *
 * ==================================================================== */

extern INT32 compute_copy_alignment(TY_IDX src, TY_IDX dst, INT32 offset)
{
  INT32	srcAlign, dstAlign, align;
  INT32 max=	MTYPE_alignment(Max_Uint_Mtype);

  srcAlign= (src) ? TY_align(src) : max;
  dstAlign= (dst) ? TY_align(dst) : max;

  align= MIN(srcAlign, dstAlign);
  align= MIN(align, max);

#ifndef KEY // bug 1990
  Is_True((compute_offset_alignment(offset, align) == align),
	  ("compute_copy_alignment: alignment not consistent with offset"));
#endif

#ifdef TARG_ST
  // FdF 20100108: A MLOAD or a MSTORE may be converted into a
  // sequence of load/store. When -TENV:align_aggregates is used, it
  // may be the case that offset is not a multiple of the
  // alignment. In that case, the alignment must be reduced to be
  // compatible with offset.
  if (compute_offset_alignment(offset, align) < align)
    align = compute_offset_alignment(offset, align);
  return align;
#else
  return MIN(align, max);
#endif
}


extern TYPE_ID compute_copy_quantum(INT32 alignment)
{
  if (UseAlignedCopyForStructs==FALSE)
  {
    INT32  maxAlign= MTYPE_alignment(Max_Uint_Mtype);
    INT32  ratio = Copy_Quantum_Ratio();
    
    if (alignment*ratio < maxAlign) 
      return Max_Uint_Mtype;
  }

  return Mtype_AlignmentClass(alignment, MTYPE_CLASS_UNSIGNED_INTEGER);
}

static TYPE_ID compute_next_copy_quantum(TYPE_ID quantum, INT32 alignment)
{
  TYPE_ID next = Mtype_prev_alignment(quantum);

  if (UseAlignedCopyForStructs==FALSE)
  {
    if ((MTYPE_alignment(next) > alignment)	&&
        (next == MTYPE_U2 || next == MTYPE_I2))
      next = Mtype_prev_alignment(next);
  }
  return next;
}




/* ====================================================================
 *
 * TY_IDX computeNewAlignmentTY(addr, type, WN_const_val(size), offset);
 *
 * Compute a new quantum (alignment), subject to size restrictions.
 * For the new quantum to apply we must have an ST of the appropriate
 * type/offset to align/pad
 *
 * ==================================================================== */

static TY_IDX computeNewAlignmentTY(WN *addr, TY_IDX type, INT32 size,
				    INT64 offset)
{
  TY_IDX newType;
  INT32	align =	TY_align(type);
  INT32	maxAlign = MTYPE_alignment(Max_Uint_Mtype);

  if ((size <= align) || (align >= maxAlign))
    return TY_IDX_ZERO;

 /*
  *  create offset consistent with the offset of addr and offset
  */
  newType = compute_alignment_type(addr, type, offset);
 
  if (WN_operator_is(addr, OPR_LDA) && ST_class(WN_st(addr)) != CLASS_BLOCK)
  {
    ST	   *sym     = WN_st(addr);
    TY_IDX  symType = compute_alignment_type(addr, ST_type(sym), offset);
   /*
    *  If the objected is defined in the current module but not allocated yet
    *  we are free to redefine the alignment so compute largest possible
    *  consistent alignment. The alignment should be consistent with the
    *  offset of course
    */
    if (ST_pu_defined(sym) && !Is_Allocated(sym))
    {
      INT32  stAlign;

      stAlign = compute_offset_alignment(WN_lda_offset(addr), maxAlign);
      stAlign = compute_offset_alignment(offset, stAlign);

      if (TY_align(symType) < stAlign)
      {
        if (traceAlignment)
        {
          DevWarn("realign ST %s (from %d to %d) line %d", ST_name(sym),
		  TY_align(symType), stAlign, Srcpos_To_Line(current_srcpos));
        }
	// Convert Make_Align_Type once real iterators are implemented
	// to iterate over the TY table.
	symType = Make_Align_Type(symType, stAlign);

        if ((ST_class(sym) == CLASS_CONST)              &&
            (TCON_ty(STC_val(sym)) == MTYPE_STR)        &&
             TY_is_pointer(Ty_Table[ST_type(sym)]))
        {
          symType = Make_Pointer_Type(newType);
        }
        Set_ST_type(sym, symType);

        if (stAlign > align)
        {
          return Make_Align_Type(newType, stAlign);
        }
        return TY_IDX_ZERO;
      }
    }
  }

  if (TY_align(newType) > align)
    return newType;

  return TY_IDX_ZERO;
}




static WN *compute_new_size(WN *tree, WN *size, INT32 align)
{
  if (Align_Padding)
  {
    INT64  sizeN   = WN_const_val(size);
    INT64  residue = sizeN % align;
    INT32  popcnt  = TARG_INT_Pop_Count(residue);

    if (popcnt > 1)
    {
      INT64  newsize = sizeN + (align-residue);

      return WN_Intconst(WN_rtype(size), newsize);
    }
  }
  return size;
}




/* ====================================================================
 *
 * WN *improve_Malignment(WN *tree, WN *addr, WN *size, INT64 offset)
 *
 * Given an mstore or mload try to improve the alignment (if possible)
 *
 *
 * ==================================================================== */
static WN *improve_Malignment(WN *tree, WN *addr, WN *size, INT64 offset)
{
  TY_IDX type, newType;

  if (Align_Object == FALSE)
    return tree;

  if (!WN_operator_is(size, OPR_INTCONST))
  {
    return tree;
  }

  TY &tree_ty = Ty_Table[WN_ty(tree)];

  if (TY_is_f90_pointer(tree_ty)) {
     /* It's not safe to realign F90 pointers */
     return tree;
  }

  type = TY_pointed(tree_ty);

  newType = computeNewAlignmentTY(addr, type, WN_const_val(size), offset);

  if (newType)
  {
    if (traceAlignment)
    {
      DevWarn("realign OPCODE %s (from %d to %d) line %d",
	      OPCODE_name(WN_opcode(tree)), TY_align(type), TY_align(newType),
	      Srcpos_To_Line(current_srcpos));
    }

    WN_set_ty(tree, Make_Pointer_Type(newType));

    switch(WN_operator(tree))
    {
    case OPR_MSTORE:
      WN_kid2(tree)= compute_new_size(tree, size, TY_align(newType));
      break;

    case OPR_MLOAD:
      WN_kid1(tree)= compute_new_size(tree, size, TY_align(newType));
      break;
    }
  }

  return tree;
}




/* ====================================================================
 *
 * WN *lower_mstore(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on mstore statement <tree>,
 * returning lowered tree.
 *
 * The following control the expansion
 *    
 *    BOOL GenerateLoopCode(INT32 size, TYPE_ID quantum)
 *	  See notes on this function for more information
 *
 *    BOOL UseAlignedCopyForStructs
 *	  TRUE
 *		expand using minimum common alignment 
 *	  FALSE 
 *		expand using maximum integer type
 *
 *    BOOL UnweaveCopyForStructs
 *	  TRUE
 *		clump the loads together before the stores, eg:
 *		  preg1 = ILOAD 0
 *		  preg2 = ILOAD 8
 *		    preg1
 *		  ISTORE 0
 *		    preg2
 *		  ISTORE 8
 *	  FALSE 
 *		Interweave loads and stores, eg:
 *		   ILOAD 0
 *		  ISTORE 0
 *		   ILOAD 8
 *		  ISTORE 8
 *
 * ==================================================================== */

static WN *lower_mstore(WN * /*block*/, WN *mstore, LOWER_ACTIONS actions)
{
  WN		*newblock;
  WN		*load, *addr, *size;
  SRCPOS	srcpos = WN_Get_Linenum(mstore);
  CURRENT_STATE	mstate = pushCurrentState(mstore, actions);

  load =	WN_kid0(mstore);
  addr =	WN_kid1(mstore);
  size =	WN_kid2(mstore);

  Is_True((WN_opcode(mstore) == OPC_MSTORE),
	  ("expected MSTORE node, not %s", OPCODE_name(WN_opcode(mstore))));

  if (WN_field_id (mstore) != 0)
      lower_field_id (mstore);

  newblock = WN_CreateBlock();
  {
    TY_IDX  dstTY = TY_pointed(Ty_Table[WN_ty(mstore)]);
    TY_IDX  srcTY = TY_IDX_ZERO;
    WN     *expr = load;

   /*
    *  The "load" may be any expression (ex. constant 0)
    *  If so, there should be no alignment restrictions
    */
    if (WN_opcode(load) == OPC_MLOAD)
    {
#ifdef KEY
      if (WN_field_id(load) != 0)
	lower_field_id (load);
#endif

      srcTY = TY_pointed(Ty_Table[WN_ty(load)]);
      expr =	WN_kid0(load);
     /*
      *  the mload may have an offset
      */
      if (WN_load_offset(load))
#ifdef KEY
      if (WN_operator(expr) == OPR_LDA || WN_operator(expr) == OPR_ILDA) {
	WN_lda_offset(expr) += WN_load_offset(load);
      }
      else
#endif
      {
        expr = WN_Add(Pointer_type,
		      expr,
		      WN_Intconst(Pointer_type, WN_load_offset(load)));
	WN_kid0(load)=	expr;
      }
    }
    else
    {
      load =		NULL;
    }

    {
      INT32	copy_alignment;
      PREG_NUM	dstPreg, srcPreg;
      TYPE_ID	quantum;
      ST	*preg=    MTYPE_To_PREG(Pointer_type);

      if (WN_store_offset(mstore))
#ifdef KEY
      if (WN_operator(addr) == OPR_LDA || WN_operator(addr) == OPR_ILDA) {
	WN_lda_offset(addr) += WN_store_offset(mstore);
      }
      else
#endif
      {
        addr = WN_Add(Pointer_type,
		      addr,
		      WN_Intconst(Pointer_type, WN_store_offset(mstore)));
      }

      copy_alignment = compute_copy_alignment(srcTY, dstTY,
					      WN_store_offset(mstore));
      quantum =	compute_copy_quantum(copy_alignment);

      switch(GenerateMstoreAction(size, 0, quantum, WN_kid0(mstore)))
      {
      case MSTORE_intrinsic_bzero:
#ifdef TARG_ST
	copy_aggregate_memset(newblock, WN_ty(mstore), WN_CreateIntconst(OPC_U4INTCONST, 0), addr, size);	
#else
	copy_aggregate_bzero(newblock, WN_ty(mstore), addr, size);
#endif
	break;

      case MSTORE_intrinsic_memset:
	copy_aggregate_memset(newblock, WN_ty(mstore), expr, addr, size);
	break;

#ifdef KEY
      case MSTORE_intrinsic_memcpy:
        copy_aggregate_memcpy(newblock, 0, 
			      TY_pointer(srcTY), TY_pointer(dstTY),
			      expr, addr, size);
        break;	

#endif
      case MSTORE_intrinsic_bcopy:
#ifdef TARG_ST
	copy_aggregate_memcpy(newblock, 0, TY_pointer(srcTY), TY_pointer(dstTY),
			     expr, addr, size);
#else
	copy_aggregate_bcopy(newblock, 0, TY_pointer(srcTY), TY_pointer(dstTY),
			     expr, addr, size);
#endif
	break;

      case MSTORE_loop:
#ifdef KEY // if the source and dest overlap, use smaller quantum
        if ((WN_operator(expr) == OPR_LDA || WN_operator(expr) == OPR_ILDA) &&
	    WN_operator(expr) == WN_operator(addr) && 
	    WN_st_idx(expr) == WN_st_idx(addr)) {
	  WN_OFFSET diff = abs(WN_lda_offset(expr) - WN_lda_offset(addr));
	  if (diff < MTYPE_byte_size(quantum) && diff != 0)
  	    quantum = Mtype_AlignmentClass(diff, MTYPE_CLASS_UNSIGNED_INTEGER);
	}
#endif
        srcPreg = AssignExpr(newblock, expr, WN_rtype(expr));
        dstPreg = AssignExpr(newblock, addr, Pointer_type);
	copy_aggregate_loop(newblock, srcTY, dstTY, 0, size, quantum,
#ifdef TARG_ST
                            preg,
#endif
			    srcPreg, dstPreg, load, mstore,
			    copy_alignment, actions);
	break;

      case MSTORE_aggregate:
#ifdef KEY // if the source and dest overlap, use smaller quantum
        if (! UnweaveCopyForStructs &&
	    (WN_operator(expr) == OPR_LDA || WN_operator(expr) == OPR_ILDA) &&
	    WN_operator(expr) == WN_operator(addr) &&
	    WN_st_idx(expr) == WN_st_idx(addr)) {
	  WN_OFFSET diff = abs(WN_lda_offset(expr) - WN_lda_offset(addr));
	  if (diff < MTYPE_byte_size(quantum) && diff != 0)
  	    quantum = Mtype_AlignmentClass(diff, MTYPE_CLASS_UNSIGNED_INTEGER);
	}
#endif
        srcPreg = AssignExpr(newblock, expr, WN_rtype(expr));
        dstPreg = AssignExpr(newblock, addr, Pointer_type);
	copy_aggregate(newblock, srcTY, dstTY, 0, WN_const_val(size),
		       quantum, 
#ifdef TARG_ST
                       preg,
#endif
                       srcPreg, dstPreg, load, mstore,
		       copy_alignment, actions);
	break;
      }
    }
  }

  WN_Delete( WN_kid2(mstore));
  WN_Delete(mstore);

  newblock = lower_block(newblock, actions);

  popCurrentState(mstate);
  return newblock;
}



/* ====================================================================
 *
 * WN *lower_mload(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * This expansion is generated only for eval (mload), where the TY is volatile
 * I hope this never happens in practice
 * With gccfe this can happen when TY is not volatile and we don't optimize.
 *
 * ==================================================================== */

static WN *lower_mload(WN * /*block*/, WN *mload, LOWER_ACTIONS actions)
{
  PREG_NUM  srcPreg;
  TYPE_ID   quantum;
  INT32     size, copy_alignment;
  WN        *newblock;

  Is_True((WN_opcode(mload) == OPC_MLOAD),
	  ("expected MLOAD node, not %s", OPCODE_name(WN_opcode(mload))));
  Is_True((WN_operator_is(WN_kid1(mload), OPR_INTCONST)),
	  ("expected MLOAD size constant  "));

  if (WN_field_id (mload) != 0)
      lower_field_id (mload);

  WN       *addr   = WN_kid0(mload);
  INT32     offset = WN_load_offset(mload);
  TY_IDX    srcTY  = TY_pointed(Ty_Table[WN_ty(mload)]);

  newblock = WN_CreateBlock();

  size = WN_const_val( WN_kid1(mload) );
  copy_alignment = compute_copy_alignment( srcTY, TY_IDX_ZERO, offset );
  quantum = compute_copy_quantum( copy_alignment );

  srcPreg = AssignExpr(newblock, addr, WN_rtype(addr));

  while (size > 0)
  {
    INT32 stride = MTYPE_RegisterSize(quantum);
    INT32 nMoves = size / stride;

   /*
    *  generate  unrolled eval (load)
    */
    while (nMoves > 0)
    {
      WN	*value, *eval;

      // use quantum TY rather than srcTY since want quantum-sized loads.
      value = WN_IloadLdid(quantum, offset, MTYPE_To_TY(quantum),
			   MTYPE_To_PREG(Pointer_type), srcPreg);

      lower_copy_maps(mload, value, actions);

      eval = WN_CreateEval(value);

      WN_INSERT_BlockLast(newblock, eval);

      offset += stride;
      size   -= stride;
      nMoves--;
    }
   /*
    *  If there is a residue we must recompute a new quantum
    *  and generate a copy for that.
    */
    if (size > 0)
    {
      quantum = compute_next_copy_quantum(quantum, copy_alignment);
    }
  }

  WN_Delete( WN_kid1(mload));
  WN_Delete(mload);

  newblock = lower_block(newblock, actions);

  return newblock;
}




/* ====================================================================
 *
 * void lower_complex_actual
 *
 * Perform lowering (see WN_Lower description) for complex actuals
 * This is used in fortran only , and only by value (%val)
 *
 *
 * ==================================================================== */

static void lower_complex_actual (WN *block, WN *val, PLOC ploc,
				  LOWER_ACTIONS actions)
{
  WN     *size, *mload, *addr;
  TY_IDX  complexType = TY_Of_Expr(val);

  size = WN_Intconst(Integer_type, TY_size(Ty_Table[complexType]));

  addr = make_pointer_to_node(block, val);

  mload = WN_CreateMload(0, Make_Pointer_Type(complexType), addr, size);

#ifdef TARG_ST
  lower_mload_actual (block, NULL, NULL, mload, ploc, actions);
#else
  lower_mload_actual (block, mload, ploc, actions);
#endif
}




/* ====================================================================
 *
 * void lower_mload_actual
 *
 * Perform lowering (see WN_Lower description) for structure actuals
 * the structure will be copied to [ regs ] and [ stack ]
 *
 *
 * ==================================================================== */

static void lower_mload_actual (WN *block, 
                               
#ifdef TARG_ST
                                WN *memcpyblock,
                                WN *mparm,
#endif
                                WN *mload, PLOC ploc,
                                LOWER_ACTIONS actions)
{
  ST       *preg;
  INT32     size, mloadOffset = 0; 
  PREG_NUM  addrN;

  if (WN_field_id(mload) != 0)
      lower_field_id (mload);

  TY_IDX    mloadTY = TY_pointed(Ty_Table[WN_ty(mload)]);
  WN       *mloadSize = WN_kid1(mload);

  if (WN_operator(mloadSize) != OPR_INTCONST) {
	DevWarn("mload_actual size is not INTCONST");
	if (WN_operator(mloadSize) == OPR_CVTL) {
		mloadSize = WN_kid0(mloadSize);
	}
  }
  size = WN_const_val(mloadSize);
  if (size<=0 || WN_operator(mloadSize) != OPR_INTCONST)
  {
    DevWarn("size of mload actual should be > 0");
    size = TY_size(Ty_Table[mloadTY]);
    DevWarn("adjusting size of (%s) to TY_size= %d",
	    TY_name(Ty_Table[mloadTY]), size);
    mloadSize = WN_Intconst(Integer_type, size);
  }

  Is_True((WN_opcode(mload) == OPC_MLOAD),
	  ("expected MLOAD node, not %s", OPCODE_name(WN_opcode(mload))));

  Setup_Struct_Output_Parameter_Locations(mloadTY);
  ploc = Get_Struct_Output_Parameter_Location(ploc);

 /*
  *  create addrN assignment to hold the address of the mload
  *  Adjust the address if there is an offset in the mload
  */
  preg = MTYPE_To_PREG(Pointer_type);
  {
    WN	*addr = WN_COPY_Tree(WN_kid0(mload));

    if (WN_load_offset(mload))
    {
      addr = WN_Add(Pointer_type,
                    addr,
                    WN_Intconst(Pointer_type, WN_load_offset(mload)));
    }
#ifdef TARG_ST
    addrN = AssignExpr(memcpyblock ? memcpyblock : block, addr, Pointer_type);
#else
    addrN = AssignExpr(block, addr, Pointer_type);
#endif
  }

#ifdef TARG_ST
  if(PLOC_by_reference(ploc)) {
    if(PLOC_on_stack(ploc)) {
     /*
      *  create WN to copy @mload to sp+offset
      *
      *  dstPreg = SP + offset
      */
      PREG_NUM	dstPreg;
      PREG_NUM	srcPreg = addrN;
      WN 	*add;
      INT64	offset;
      TY_IDX srcTY =	mloadTY;
      TY_IDX dstTY =	MTYPE_To_TY(Max_Uint_Mtype);
      TYPE_ID		quantum;
      INT32		copy_alignment;

      
#ifdef TARG_ST
      //      offset = PLOC_offset(ploc) - Formal_Save_Area_Size
      //	+ STACK_OFFSET_ADJUSTMENT;
      offset = PLOC_offset(ploc) + STACK_OFFSET_ADJUSTMENT;
#else
      offset = PLOC_offset(ploc) - Formal_Save_Area_Size
	+ Stack_Offset_Adjustment;
#endif
      add = WN_Add(Pointer_type,
		   WN_LdidPreg(Pointer_type, Stack_Pointer_Preg_Offset),
		   WN_Intconst(Pointer_type, offset));
      
#ifdef TARG_ST
      dstPreg = AssignExpr(memcpyblock ? memcpyblock : block, add, Pointer_type);
#else
      dstPreg = AssignExpr(block, add, Pointer_type);
#endif
      
      /*
       *  copy ref of object to add.
       */

      WN *value = WN_LdidPreg(Pointer_type, srcPreg);
      WN *addr = WN_LdidPreg(Pointer_type, dstPreg);
      WN *store = WN_Istore(Pointer_type, 0, Make_Pointer_Type (struct_memop_type (Pointer_type, dstTY)), addr,	value);
      WN_INSERT_BlockLast(block, store);
    }
    else {
      PREG_NUM regNo = PLOC_reg(ploc);
      ST	*reg = Preg_Offset_Is_Float(regNo) ? Float_Preg : Int_Preg;

      WN *addr = WN_LdidPreg(Pointer_type, addrN);
      WN *stid= WN_Stid(Pointer_type, regNo, reg, ST_type(reg), addr);
      WN_INSERT_BlockLast(block, stid);
    }
  }
  else
#endif
  while (PLOC_is_nonempty(ploc))
  {
    if (PLOC_on_stack(ploc))
    {
      PREG_NUM	dstPreg;
     /*
      *  create WN to copy mload to sp+offset
      *
      *  dstPreg = SP + offset - mloadoffset
      *  copy_aggregate() will add mloadoffset later
      */
      {
        WN 	*add;
	INT64	offset;

#ifdef TARG_ST
	//	offset = PLOC_offset(ploc) - Formal_Save_Area_Size
	//	  + STACK_OFFSET_ADJUSTMENT - mloadOffset; 
	offset = PLOC_offset(ploc) + STACK_OFFSET_ADJUSTMENT - mloadOffset; 
#else
	offset = PLOC_offset(ploc) - Formal_Save_Area_Size
	  + Stack_Offset_Adjustment - mloadOffset; 
#endif

	add = WN_Add(Pointer_type,
		     WN_LdidPreg(Pointer_type, Stack_Pointer_Preg_Offset),
		     WN_Intconst(Pointer_type, offset));
#ifdef TARG_ST
	dstPreg = AssignExpr(memcpyblock ? memcpyblock : block, add, Pointer_type);
#else
	dstPreg = AssignExpr(block, add, Pointer_type);
#endif
      }
      {
	TY_IDX srcTY =	mloadTY;
	TY_IDX dstTY =	MTYPE_To_TY(Max_Uint_Mtype);
	TYPE_ID		quantum;
	WN		*con;
	INT32		copy_alignment, nMoves;
	INT32		todo= size - mloadOffset;

       /*
	* the dest is guaranteed to be Max_Uint aligned
	*/
	copy_alignment= compute_copy_alignment(srcTY, dstTY, 0);
	quantum =	compute_copy_quantum(copy_alignment);

	/*
	 *  we cannot (now) generate an intrinsics as we are in the middle
	 *  of setting call registers!!
         */
	nMoves= calculateLoadStore(todo, mloadOffset, quantum, WN_kid0(mload));

#ifdef TARG_ST
	// FdF 20050407: use memcpy to pass struct parameter by value
	if (memcpyblock && MinStructCopyMemIntrSize && MinStructCopyMemIntrSize <= nMoves)
        {
	  con = WN_Intconst(Integer_type, todo);
	  copy_aggregate_memcpy(memcpyblock, mloadOffset, TY_pointer(srcTY), TY_pointer(dstTY),
				WN_LdidPreg(quantum, addrN), WN_LdidPreg(quantum, dstPreg), con);
	  WN_Delete(con);
	}
	else
#endif
	if (MinStructCopyLoopSize && MinStructCopyLoopSize <= nMoves)
	{
	  con = WN_Intconst(Integer_type, todo);

	  copy_aggregate_loop(block, 
                              srcTY, 
                              dstTY,
                              mloadOffset,
                              con,
			      quantum,
#ifdef TARG_ST
                              preg,
#endif
                              addrN,
                              dstPreg, 
                              mload, 
#ifdef TARG_ST
			      // TBwhy: I don't know why passing mparm here?
			      mparm,
#else
                              NULL,
#endif
			      copy_alignment, actions);
	  WN_Delete(con);
	}
	else
	{
	  copy_aggregate(block, 
                         srcTY, 
                         dstTY, 
                         mloadOffset, 
                         todo,
			 quantum,
#ifdef TARG_ST
                         preg,
#endif
                         addrN, 
                         dstPreg, 
                         mload, 
#ifdef TARG_ST
			 // TBwhy: I don't know why passing mparm here?
			 mparm,
#else
                         NULL,
#endif
			 copy_alignment, actions);
	}
      }
      return;
    }
    else
    {
     /*
      *  copy structure element to register
      */
      PREG_NUM regNo = PLOC_reg(ploc);
      ST	*reg = Preg_Offset_Is_Float(regNo) ? Float_Preg : Int_Preg;
      TYPE_ID   type = TY_mtype(Ty_Table[ST_type(reg)]);

      {
	WN      *load, *stid;
	INT32	todo = size - mloadOffset;

	if (PLOC_size(ploc) < MTYPE_size_reg(type)
		&& type == MTYPE_F8 && PLOC_size(ploc) == 4)
	{
#ifndef TARG_X8664 // can happen in reality for X86-64
		// need to copy a smaller size than default reg-size
		DevWarn("actual_mload: switch from mtype_f8 to mtype_f4");
#endif
		type = MTYPE_F4;
		reg = MTYPE_To_PREG(type);
	}

       /*
	*  special case "small" structs (or remainder of struct)
	*  we will try not to run off the end of the structure (as bad)
	*/
	if (todo < MTYPE_alignment(type))
	{
	  TYPE_ID	quantum;
	  INT32		newAlign, shiftn;

	  Is_True(Preg_Offset_Is_Int(regNo),
		  ("mload actual->reg(size/alignment problem"));
	  newAlign = nearest_power_of_two(todo);

	  quantum = Mtype_AlignmentClass(newAlign, MTYPE_type_class(type));

	  load = WN_IloadLdid(quantum, mloadOffset,
			      struct_memop_type (quantum, mloadTY), preg,
			      addrN); 

	  lower_copy_maps(mload, load, actions);

	  if (!Target_Is_Little_Endian) {
	    shiftn = MTYPE_size_reg(type) - MTYPE_size_reg(quantum);
  
	    load = WN_Shl(type, load, WN_Intconst(type, shiftn));
	  }
	}
	else
	{
	  load = WN_IloadLdid(type, mloadOffset,
			      struct_memop_type (type, mloadTY), preg,
			      addrN); 

	  lower_copy_maps(mload, load, actions);
	}

	stid= WN_Stid(type, regNo, reg, ST_type(reg), load);
	WN_Set_Linenum(stid, current_srcpos);  // Bug 1268
	WN_INSERT_BlockLast(block, stid);
      }
    }
    mloadOffset += PLOC_size(ploc);
    ploc = Get_Struct_Output_Parameter_Location(ploc);
  }
}


/* ====================================================================
 *
 * static void lower_mload_formal
 *
 * Perform lowering (see WN_Lower description) for structure formals
 * registers will be copied to [ stack ]
 *
 * ==================================================================== */

static void lower_mload_formal(WN *block, WN *mload, PLOC ploc,
			       LOWER_ACTIONS actions)
{
  INT32   size, offset = 0; 
  ST     *sym = WN_st(mload);
  TY_IDX  symTY = ST_type(sym);

  Setup_Struct_Input_Parameter_Locations(symTY);
  ploc = Get_Struct_Input_Parameter_Location(ploc);
  size = TY_size(Ty_Table[symTY]);

  while (PLOC_is_nonempty(ploc))
  {
    if (PLOC_on_stack(ploc))
    {
     /*
      *  structure is already on the stack, nothing to do
      */
      return;
    }
    else
    {
     /*
      *	copy register to stack
      */
      PREG_NUM 	regNo =	PLOC_reg(ploc);
      ST	*reg = Preg_Offset_Is_Float(regNo) ? Float_Preg : Int_Preg;
      TYPE_ID   type = TY_mtype(ST_type(reg));
      INT32	todo = size - offset;
      {
	WN        *ldid, *store;

	if (PLOC_size(ploc) < MTYPE_size_reg(type)
		&& type == MTYPE_F8 && PLOC_size(ploc) == 4)
	{
#ifndef TARG_X8664 // can happen in reality for X86-64
		// need to copy a smaller size than default reg-size
		DevWarn("formal_mload: switch from mtype_f8 to mtype_f4");
#endif
		type = MTYPE_F4;
		reg = MTYPE_To_PREG(type);
	}

	ldid = WN_LdidPreg(type, regNo);
       /*
	*  special case "small" structs (or remainder of struct)
	*  to use type closer to size of remaining todo
	*/
	if (todo < MTYPE_alignment(type))
	{
	  TYPE_ID	quantum;
          INT32         newAlign, shiftn;

          Is_True(Preg_Offset_Is_Int(regNo),
		  ("mload actual->reg(size/alignment problem"));
          newAlign = nearest_power_of_two(todo);

          quantum = Mtype_AlignmentClass(newAlign, MTYPE_type_class(type));

          shiftn = MTYPE_size_reg(type) - MTYPE_size_reg(quantum);

	  if (!Target_Is_Little_Endian) {
	   /*
	    *  fix (kludge) for kernel bug pv 369702
	    *  Since U4 and I4 are both sign extended we could use
	    *  an arithmetic shift
	    */
	    if (MTYPE_alignment(quantum) == 4)
	    {
	      ldid= WN_Ashr(type, ldid, WN_Intconst(type, shiftn));
	    }
	    else
	    {
	      ldid= WN_Lshr(type, ldid, WN_Intconst(type, shiftn));
	    }
	  }

	  store = WN_Stid (quantum, offset, sym,
			   struct_memop_type (quantum, symTY) , ldid);
        }
	else
	{
	  store = WN_Stid (type, offset, sym, struct_memop_type (type, symTY),
			   ldid);   
	}
	lower_copy_maps(mload, store, actions);
	WN_Set_Linenum(store, current_srcpos);  // Bug 1268
        WN_INSERT_BlockLast(block, store);
      }
      offset += PLOC_size(ploc);
    }
    ploc = Get_Struct_Input_Parameter_Location(ploc);
  }
}




/* ====================================================================
 *
 *  void lower_complex_emulation(WN *block, WN *tree, LOWER_ACTIONS actions,
 *				WN **realpart, WN **imagpart)
 *
 * Perform lowering (see WN_Lower description) on an expression
 * or intrinsic op, returning lowered tree.  
 *
 * ==================================================================== */
static void lower_complex_emulation(WN *block, WN *tree, LOWER_ACTIONS actions,
				   WN **realpart, WN **imagpart)
{
  WN		*wn;
  BOOL          intrinsic_lowered;

  Is_True(MTYPE_is_complex(WN_rtype(tree)),
	 ("expected complex type in lower_complex_emulation"));

 /*
  *  there is no way to turn off intrinsics consistently
  *  ie. the best we could do is 
  *		z = c4intrinsic(...)
  *  and then we would try to lower that !!
  */

  wn = lower_emulation(block, tree, actions, intrinsic_lowered);

  // Don't do this. It causes the optimizer to miss a lot of CSE's. 
  // R. Shapiro 10/5/98
  if (!intrinsic_lowered) {
       actions |= LOWER_INTRINSIC;
   }
#ifdef TARG_X8664 // for C4INTRINSIC_OP, wn is type MTYPE_F8
  if (WN_rtype(wn) == MTYPE_F8) { // store to a temp and return two halves
    ST *c4temp_st = Gen_Temp_Symbol(MTYPE_To_TY(MTYPE_F8), ".c4");
    WN *stid = WN_Stid(MTYPE_F8, 0, c4temp_st, MTYPE_To_TY(MTYPE_F8), wn);
    WN_Set_Linenum (stid, WN_Get_Linenum(tree));
    WN_INSERT_BlockLast(block, stid);

    *realpart = WN_Ldid(MTYPE_F4, 0, c4temp_st, MTYPE_To_TY(MTYPE_F4));
    *imagpart = WN_Ldid(MTYPE_F4, 4, c4temp_st, MTYPE_To_TY(MTYPE_F4));
  }
  else
#endif
  lower_complex_expr(block, wn, actions, realpart, imagpart);
}


/* ====================================================================
 *
 * WN *lower_emulation(WN *block, WN *tree, LOWER_ACTIONS actions,
 *                     BOOL & intrinsic_lowered)
 *
 * Perform lowering (see WN_Lower description) on an expression
 * or intrinsic op, returning lowered tree.  
 * 
 * intrinsic_lowered is set to TRUE if lowereing occurred on an intrinsic
 *
 * Should be called only if actions contains LOWER_INTRINSIC.
 *
 * ==================================================================== */
static WN *lower_emulation(WN *block, WN *tree, LOWER_ACTIONS actions,
			   BOOL & intrinsic_lowered)
{
  WN	*wn, *emBlock, *node;
  OPCODE op = WN_opcode(tree);

  intrinsic_lowered = FALSE;
 /*
  *  try experiment with lowering children first
  *  (remember these are most likely complex expressions)
  */
  {
    INT32	i;

    for (i = 0; i < WN_kid_count(tree); i++)
      WN_actual(tree,i) = lower_expr(block, WN_actual(tree,i), actions);
  }

 /*
  *  Create a callblock and try to emulate call
  */
  wn = NULL;
  emBlock = WN_CreateBlock();

  if (OPCODE_is_intrinsic(WN_opcode(tree)))
  {
    Is_True((INTRN_is_nary(WN_intrinsic(tree))==FALSE),("nary slipped by"));
    if (Action(LOWER_INLINE_INTRINSIC) || Action(LOWER_INL_STACK_INTRINSIC))
    {
      wn = emulate(emBlock , tree);
    }
#ifndef TARG_ST
#ifdef KEY // bug 6938
    else if (Action(LOWER_FAST_EXP) && OPCODE_is_intrinsic(WN_opcode(tree)))
    {
      wn = emulate_fast_exp(emBlock , tree);
    }
#endif
#endif
    if (wn == NULL && NotAction(LOWER_INTRINSIC))
    {
       return tree;
    }
    intrinsic_lowered = TRUE;
  }
 /*
  *  most likely quad emulation
  */
  else
  {
    wn = emulate(emBlock , tree);
  }


  if (wn)
  {
#ifdef KEY
    // Bug 1268: Emulate may generate statements without line numbers.
    // Fix the linenumbers now.
    for (node = WN_first(emBlock); node; node = WN_next(node)) {
      WN_Set_Linenum(node, current_srcpos);
    }
    if (Action(LOWER_TO_CG)) actions |= LOWER_RETURN_VAL;
#endif
    emBlock = lower_block(emBlock, actions);
#ifdef TARG_ST
    // [TB] Improve line number.
    WN_copy_linenum(tree, emBlock);
#endif

    WN_INSERT_BlockLast(block, emBlock);

    /*
     *  the emulation may contain nodes that may need to be lowered
     */
    if (OPCODE_is_stmt(WN_opcode(wn)))
    {
      WN_Set_Linenum(wn, current_srcpos);
      wn = lower_stmt(block, wn, actions);
    }
    else if (OPCODE_is_expression(WN_opcode(wn)))
    {
      wn = lower_expr(block, wn, actions);
    }
  }
  else
  {
    WN_Delete(emBlock);

    if (OPCODE_is_call(op))
    {
      wn = lower_intrinsic(block, tree, actions);
    }
    else
    {
      wn = lower_intrinsic_op(block, tree, actions);
    }
  }
  return wn;
}


#ifdef linux
/* Note no sincos() on Darwin */
/* ====================================================================
 *
 * WN *lower_cis_intrinsic(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering on CIS intrinsics for linux. On linux we'ld like
 * so use the standard sincos() library routines rather than the irix
 * *cis() routines. The trouble is that sincos returns its results
 * through pointers passed as arguments. Therefore it must be an
 * INSTRINSIC_CALL and not an INTRINSIC_OP (we want the later in order for 
 * wopt to perform the CSEs of the combined function, which operaters on
 * expressions only). Therefore we generate the CIS INTRINSIC_OP and
 * lower it here to a call to sincos(). The lowering looks like this:
 *
 * Before:
 *	C*INTRINSIC_OP INTRN_*CIS (angle)
 *
 * After:
 *      create C* temp
 *	VINTRINSIC_CALL INTRN_SINCOS (angle, &temp+x, &temp+0)
 *	C*C*LDID temp
 *
 * Note that the INTRINSIC_CALL is lowered here as well.
 *
 * ==================================================================== */
static WN *lower_cis_intrinsic(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  WN *callblock, *call, *parms[3];
  INTRINSIC sincos_id;
  TYPE_ID real_mtyp, complex_mtyp;
  TY_IDX real_ty, complex_ty;
  ST *return_sincos;
  WN *asinx, *acosx;

  complex_mtyp = WN_rtype(tree);
  real_mtyp = Mtype_complex_to_real(complex_mtyp);
  complex_ty = MTYPE_To_TY(complex_mtyp);
  real_ty = MTYPE_To_TY(real_mtyp);

  return_sincos = Gen_Temp_Symbol(complex_ty, ".sincos");
  acosx = WN_CreateLda(OPR_LDA, Pointer_Mtype, MTYPE_V, 
		       0,
		       Make_Pointer_Type(real_ty), return_sincos);
  asinx = WN_CreateLda(OPR_LDA, Pointer_Mtype, MTYPE_V, 
		       MTYPE_byte_size(real_mtyp),
		       Make_Pointer_Type(real_ty), return_sincos);

  switch (real_mtyp) {
  case MTYPE_F4:  sincos_id = INTRN_SINCOSF; break;
  case MTYPE_F8:  sincos_id = INTRN_SINCOS;  break;
  case MTYPE_F10: sincos_id = INTRN_SINCOSL; break;
  default:        sincos_id = INTRINSIC_INVALID; break;
  }

  parms[0] = WN_kid0(tree);
  parms[1] = WN_CreateParm(Pointer_type, asinx, MTYPE_To_TY(Pointer_type),
			   WN_PARM_BY_VALUE);
  parms[2] = WN_CreateParm(Pointer_type, acosx, MTYPE_To_TY(Pointer_type),
			   WN_PARM_BY_VALUE);
  call = WN_Create_Intrinsic(OPC_VINTRINSIC_CALL, sincos_id, 3, parms);
  WN_Set_Linenum(call, current_srcpos);  // Bug 1268
#ifndef KEY // cannot delete because it needs to be around to be looked at
	    // after call to lower_complex_expr in lower_store (bug 3048)
  WN_Delete(tree);
#endif

  call = lower_intrinsic_call(block, call, actions);
  WN_INSERT_BlockLast(block, call);

  WN *ldid = WN_Ldid(complex_mtyp, 0, return_sincos, complex_ty);
  return lower_expr(block, ldid, actions);
}
#endif


/* ====================================================================
 *
 * WN *lower_intrinsic_op(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on intrinsic statement <tree>,
 * returning lowered tree.  Should be called only if actions contains
 * LOWER_INTRINSIC.
 *
 * Since we are calling a runtime routine from a non-statement level
 * we need to
 *    [1]	insert call (lower_intrinsic)
 *    [2]	assign return value to preg temp
 *    [3]	return preg temp
 *
 * The return preg value may require special processing after
 * (see below for examples)
 * ==================================================================== */
static WN *lower_intrinsic_op(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  WN		*wn;
  OPCODE	op = WN_opcode(tree);
  INTRINSIC	id = WN_intrinsic(tree);

  Is_True(Action(LOWER_INTRINSIC),
	  ("actions does not contain LOWER_INTRINSIC"));
  Is_True(OPCODE_is_intrinsic(op),
	  ("expression is not intrinsic"));

#ifdef linux
  switch (id) {
  case INTRN_F4CIS:
  case INTRN_F8CIS:
//  case INTRN_F10CIS:
#ifdef KEY
    if (! Is_Target_R10K()) // TODO (for non-linux runs only; remove afterwards)
#endif
    return lower_cis_intrinsic(block, tree, actions);
  }
#endif

 /*
  *  To evaluate the intrinsic call we need to save the return type 
  *  and create a temporary to hold the return value
  */
  wn = lower_intrinsic(block, tree, actions);

  Is_True(OPCODE_is_call(WN_opcode(wn)), 
	  ("lowered instrinsic op is not a call"));
  WN_Set_Linenum(wn, current_srcpos);  // Bug 1268
  WN_INSERT_BlockLast(block, wn);

  {
    TYPE_ID	ty1, ty2;
    PREG_NUM	reg1, reg2;
    TYPE_ID	type = WN_rtype(tree);
#ifdef TARG_ST
    AssignPregName(INTR_intrinsic_name(tree), ".return");

    // can have more than 2 return regs
    // Also don't need to check for WHIRL_Return_Info_On

    RETURN_INFO return_info = Get_Return_Info (MTYPE_To_TY(type),
					       Complex_Not_Simulated);

    switch(type)
    {
    case MTYPE_C4:
    case MTYPE_C8:
      {
	PREG_NUM	rzN, izN;
	TYPE_ID		real = Mtype_complex_to_real(type);

	Is_True(RETURN_INFO_count(return_info) == 2,
		("expected complex return value from intrinsic_op"));

	ty1  = RETURN_INFO_mtype (return_info, 0);
        ty2  = RETURN_INFO_mtype (return_info, 1);
        reg1 = RETURN_INFO_preg (return_info, 0);
        reg2 = RETURN_INFO_preg (return_info, 1);

	rzN = AssignExpr(block, WN_LdidPreg(real, reg1), real);
	izN = AssignExpr(block, WN_LdidPreg(real, reg2), real);

	wn = WN_Complex(type,
			WN_LdidPreg(real, rzN),
			WN_LdidPreg(real, izN));
      }
      break;

    case MTYPE_CQ:
     /*
      *  call is RSTYLE_VIA_FIRST_ARG
      *
      *  hopefully our intrinsic lowerer has inserted the address of the
      *  temporary area.
      */
      {
	TYPE_ID   real = Mtype_complex_to_real(type);
	TY_IDX    realTY = MTYPE_To_TY(real);
	WN       *temp,*temp1, *actual = WN_kid0(wn);

	if (WN_operator_is(actual, OPR_PARM))
	  actual = WN_kid0(actual);

	temp = lower_copy_tree( actual, actions);
	temp1 = lower_copy_tree( actual, actions);

	Is_True((OPCODE_is_call(WN_opcode(wn))), ("expected call opcode"));

	wn = WN_Complex(type,
			WN_Iload(real, 0, realTY, temp),
			WN_Iload(real, MTYPE_RegisterSize(real),
				 realTY, temp1));
      }
      break;

    case MTYPE_F8:

      //
      // if we need to return floating-point in integer registers
      //
      // NOTE: avoid creating FP pregs in Low WHIRL
      //
      if (Preg_Offset_Is_Int(RETURN_INFO_preg (return_info, 0))) {

	PREG_NUM retReg = AssignExpr(block, 
			     WN_Tas(MTYPE_F8,
				    Be_Type_Tbl(MTYPE_F8),
				    WN_LdidPreg(MTYPE_I8, 
						First_Int_Preg_Return_Offset
						)
				    ),
				    
		       MTYPE_F8);

	wn = WN_LdidPreg(MTYPE_F8, retReg);
	//wn = WN_Tas(type, Be_Type_Tbl(type), wn);

#if 0
	// [CG] This is useless has the CreatePreg in AssignExpr
	// will already have allocated 2 numbers if necessary.
	if (Only_32_Bit_Ops) {
	  /*
	   * Arthur: if we're Only_32_Bit_Ops, we need to reserve
	   *         2 PREGs for this types
	   */
	  /* reserve the second PREG */
	  retReg = Create_Preg(MTYPE_F8, CurrentPregName());
	}
#endif
      }
      else {
	PREG_NUM retReg = AssignExpr(block, 
			    WN_LdidPreg(type, First_Float_Preg_Return_Offset),
			    type);
	wn = WN_LdidPreg(type, retReg);

#if 0
	// [CG] This is useless has the CreatePreg in AssignExpr
	// will already have allocated 2 numbers if necessary.
	if (Only_32_Bit_Ops) {
	  /*
	   * Arthur: if we're Only_32_Bit_Ops, we need to reserve
	   *         2 PREGs for this types
	   */
	  /* reserve the second PREG */
	  retReg = Create_Preg(type, CurrentPregName());
	}
#endif
      }

      /*
       *  The preg may need map processing
       */
      wn = lower_expr(block, wn, actions);
      break;

    case MTYPE_U8:
    case MTYPE_I8:
      {
	PREG_NUM retReg = AssignExpr(block, 
			    WN_LdidPreg(type, First_Int_Preg_Return_Offset),
			    type);
	wn = WN_LdidPreg(type, retReg);

#if 0
	// [CG] This is useless has the CreatePreg in AssignExpr
	// will already have allocated 2 numbers if necessary.
	if (Only_32_Bit_Ops) {
	  /*
	   * Arthur: if we're Only_32_Bit_Ops, we need to reserve
	   *         2 PREGs for this types
	   */
	  /* reserve the second PREG */
	  retReg = Create_Preg(TY_mtype(Ty_Table[type]), CurrentPregName());
	}
#endif

	/*
	 *  The preg may need map processing
	 */
	wn = lower_expr(block, wn, actions);
	break;
      }
      /* FALLTHROUGH */

    case MTYPE_F4:
      //
      // If floating-point goes into integer register, need a TAS
      //
      if (Preg_Offset_Is_Int(RETURN_INFO_preg (return_info, 0))) {

	PREG_NUM retReg = AssignExpr(block, 
			     WN_Tas(MTYPE_F4,
				    Be_Type_Tbl(MTYPE_F4),
				    WN_LdidPreg(MTYPE_I4, 
						First_Int_Preg_Return_Offset
						)
				    ),
			     MTYPE_F4);

	wn = WN_LdidPreg(MTYPE_F4, retReg);
	//wn = WN_Tas(type, Be_Type_Tbl(type), wn);
      }
      else {
	PREG_NUM retReg = AssignExpr(block, 
			    WN_LdidPreg(type, First_Float_Preg_Return_Offset),
			    type);

	wn = WN_LdidPreg(type, retReg);
      }

      /*
       *  The preg may need map processing
       */
      wn = lower_expr(block, wn, actions);
      break;

    default:
      {
	Is_True(MTYPE_is_class_integer(type),("unknown mtype"));
	PREG_NUM retReg = AssignExpr(block, 
			    WN_LdidPreg(type, First_Int_Preg_Return_Offset),
			    type);

	wn = WN_LdidPreg(type, retReg);

	/*
	 *  The preg may need map processing
	 */
	wn = lower_expr(block, wn, actions);
      }

    }

    ResetPregName();
  }

  return wn;

#else /* !TARG_ST */

#ifdef TARG_X8664
    if (Is_Target_64bit() && type ==  MTYPE_C4)
      type = MTYPE_F8;
#endif

    rename_preg(INTR_intrinsic_name(tree), ".return");

    if (WHIRL_Return_Info_On) {

      RETURN_INFO return_info = Get_Return_Info (MTYPE_To_TY(type),
                                                 Complex_Not_Simulated
#ifdef TARG_X8664
						 , FALSE
#endif
						 );

      if (RETURN_INFO_count(return_info) <= 2) {

        ty1  = RETURN_INFO_mtype (return_info, 0);
        ty2  = RETURN_INFO_mtype (return_info, 1);
        reg1 = RETURN_INFO_preg (return_info, 0);
        reg2 = RETURN_INFO_preg (return_info, 1);
      }

      else
        Fail_FmtAssertion("lower_intrinsic_op: more than 2 return registers");
	/*NOTREACHED*/
    }

    else
      Get_Return_Mtypes(MTYPE_To_TY(type), Complex_Not_Simulated, &ty1, &ty2);

    switch(type)
    {
    case MTYPE_C4:
    case MTYPE_C8:
#ifdef TARG_X8664
      if (Is_Target_64bit())
#endif
      {
	PREG_NUM	rzN, izN;
	TYPE_ID		real = Mtype_complex_to_real(type);

	if (!WHIRL_Return_Info_On) 
	  Get_Return_Pregs ( ty1, ty2, &reg1, &reg2 );
	Is_True((reg1 && reg2),
		("expected complex return value from intrinsic_op"));

	rzN = AssignExpr(block, WN_LdidPreg(real, reg1), real);
	izN = AssignExpr(block, WN_LdidPreg(real, reg2), real);

	wn = WN_Complex(type,
			WN_LdidPreg(real, rzN),
			WN_LdidPreg(real, izN));
        break;
      }
#ifdef TARG_X8664
      // fall thru
#endif

    case MTYPE_CQ:
    case MTYPE_C16:
     /*
      *  call is RSTYLE_VIA_FIRST_ARG
      *
      *  hopefully our intrinsic lowerer has inserted the address of the
      *  temporary area.
      */
      {
	TYPE_ID   real = Mtype_complex_to_real(type);
	TY_IDX    realTY = MTYPE_To_TY(real);
	WN       *temp,*temp1, *actual = WN_kid0(wn);

	if (WN_operator_is(actual, OPR_PARM))
	  actual = WN_kid0(actual);

	temp = lower_copy_tree( actual, actions);
	temp1 = lower_copy_tree( actual, actions);

	Is_True((OPCODE_is_call(WN_opcode(wn))), ("expected call opcode"));

	wn = WN_Complex(type,
			WN_Iload(real, 0, realTY, temp),
			WN_Iload(real, MTYPE_RegisterSize(real),
				 realTY, temp1));
      }
      break;

    default:
      {
	PREG_NUM	retReg;

	if (!WHIRL_Return_Info_On) 
	  Get_Return_Pregs ( ty1, ty2, &reg1, &reg2 );

	Is_True((reg1 != 0), ("expected return value from intrinsic_op"));
	Is_True((reg2 == 0), ("cannot evaluate 2 regs into an expression"));

	retReg = AssignExpr(block, 
			    WN_LdidPreg(ty1, reg1),
			    type);

	wn = WN_LdidPreg(type, retReg);
      }
      break;
    }
    rename_reset();
  }

  /*
   *  These require special processing as the function return
   *  value must be interpreted (i think)
   *
   *	    s_cmp(a, b, len_a, len_b)
   *		a <  b		negative
   *		a == 0		0
   *		a >  0		positive
   */
  switch (id) {
  case INTRN_CEQEXPR:
    wn = WN_EQ(Boolean_type, wn, WN_Zerocon(Boolean_type));
    break;
  case INTRN_CNEEXPR:
    wn = WN_NE(Boolean_type, wn, WN_Zerocon(Boolean_type));
    break;
  case INTRN_CGEEXPR:
    wn = WN_GE(Boolean_type, wn, WN_Zerocon(Boolean_type));
    break;
  case INTRN_CGTEXPR:
    wn = WN_GT(Boolean_type, wn, WN_Zerocon(Boolean_type));
    break;
  case INTRN_CLEEXPR:
    wn = WN_LE(Boolean_type, wn, WN_Zerocon(Boolean_type));
    break;
  case INTRN_CLTEXPR:
    wn = WN_LT(Boolean_type, wn, WN_Zerocon(Boolean_type));
    break;
  }

 /*
  *  The preg may need map processing
  */
  return lower_expr(block, wn, actions);
#endif /* !TARG_ST */

}




/* ====================================================================
 *
 * WN *lower_intrinsic(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on an expression or,
 * intrinsic returning lowered tree.  
 *
 * Should be called only if actions contains LOWER_INTRINSIC.
 *
 * ==================================================================== */
static WN *lower_intrinsic(WN *block, WN *tree, LOWER_ACTIONS actions)
{

  Is_True(Action(LOWER_INTRINSIC),
	  ("actions does not contain LOWER_INTRINSIC"));
  Is_True(OPCODE_is_intrinsic(WN_opcode(tree)),
	  ("expression is not intrinsic"));

  {
    INTRINSIC	id = (INTRINSIC) WN_intrinsic(tree);

    if (INTRN_is_actual(id))
    {
     /*
      *  weird fortran anachronism, we need to create an ST entry
      * 	and pass an LDA of the address
      */
      TYPE_ID	rtype = WN_rtype(tree);
      WN	*lda;
      {
	TY_IDX  ty = Make_Function_Type(MTYPE_To_TY(rtype));
	ST     *st = Gen_Intrinsic_Function(ty, INTRN_rt_name(id));
	lda = WN_Lda(Pointer_type, 0, st);
      }

      WN_Delete(tree);
      return lower_expr(block, lda, actions);
    }
  }
  {
   /*
    *  Create a callblock and generate call.
    *  The arguments may be expanded to match the runtime routine.
    */
    WN	*call, *callblock;

    callblock = WN_CreateBlock();

    call = intrinsic_runtime(callblock, tree);

    callblock = lower_block(callblock, actions);
#ifdef TARG_ST
    // [TB] Improve line number.
    WN_copy_linenum(tree, callblock);
#endif
    WN_INSERT_BlockLast(block, callblock);

    if (Action(LOWER_TO_CG))
	actions |= LOWER_CALL;
    call = lower_call(block, call, actions);

    return call;
  }
}


/* ====================================================================
 *
 * WN *lower_actual(WN *block, WN *actual, TYPE_ID parmType, INT32 reg)
 *
 * Auxilary function to lower_call to put and actual in a register
 * This was motivated by -DEBUG:Varargs_Prototypes=off, where for
 * unprototyped calls, all floating point registers need to go in the
 * integer registers as well
 *
 * ==================================================================== */
static WN *lower_actual(WN *block, WN *actual, TYPE_ID parmType, INT32 reg)
{

#ifdef TARG_ST

  WN	*stid;
  ST	*regST;
  TYPE_ID type = parmType;

  regST = MTYPE_To_PREG(type);

  // parms type should be promoted by the front-end, but
  // just in case:
  type = Mtype_comparison(type);

  stid = WN_StidIntoPreg (type, reg, regST, actual);
  WN_Set_Linenum(stid, current_srcpos);
  WN_INSERT_BlockLast(block, stid);

#else
 /*
  * float parm goes in int register, so convert
  */
  if (MTYPE_float(parmType) && Preg_Offset_Is_Int(reg))
  {
    TYPE_ID type = TY_mtype(ST_type(Int_Preg));

    actual = WN_Tas(type, ST_type(Int_Preg), actual);
  }
  
  {
    WN	*stid;
    ST	*regST;

#ifdef TARG_X8664
    if (Is_Target_64bit() && parmType == MTYPE_C4) {
	regST = MTYPE_To_PREG(MTYPE_F8);
	WN_set_rtype(actual, MTYPE_F8);
	WN_set_desc( actual,
		     WN_operator_is(actual,OPR_CONST) ? MTYPE_V : MTYPE_F8 );
    }
    else
#endif
    if (Preg_Offset_Is_Int(reg)) 
	regST = Int_Preg;
    else
    	// keep float size in preg
	regST=	MTYPE_To_PREG(parmType);

    TYPE_ID type = TY_mtype(ST_type(regST));

    stid = WN_StidIntoPreg (type, reg, regST, actual);
    WN_Set_Linenum(stid, current_srcpos);
    WN_INSERT_BlockLast(block, stid);
  }
#endif

  return actual;
}

#ifdef TARG_ST
/* ====================================================================
 *
 * lower_profile_call_enabled(WN *tree)
 *
 * [CG 2004/11/30]
 * The call instrumentation is enabled if the profile is on and 
 * none of the caller or callee is marked with no_instrument_function.
 * The tree argument is the call node to instrument.
 * ==================================================================== */
static BOOL
lower_profile_call_enabled(WN *tree) {
  return Gen_Profile && WN_has_sym(current_function) &&
    !PU_no_instrument_function(Pu_Table[ST_pu(WN_st(current_function))]) &&
    !(WN_has_sym(tree) && PU_no_instrument_function(Pu_Table[ST_pu(WN_st(tree))]));
}
#endif
/* ====================================================================
 *  For each function generate a call to a profiler function
 *	__prof( &func, &call, funcname, callname, state);
 *  where
 *	state==0	before call
 *     state==1	after call
 *
 * ==================================================================== */

static WN *lower_profile_call(WN *block, WN *tree, INT32 state,
			      LOWER_ACTIONS actions)
{
#ifdef TARG_ST
  if (lower_profile_call_enabled(tree))
#else
  if (Gen_Profile && WN_has_sym(current_function))
#endif
  {
    WN		*profcall;
    char	*name;

    {
      TY_IDX  ty = Make_Function_Type(MTYPE_To_TY(MTYPE_V));
      ST     *st = Gen_Intrinsic_Function(ty, Gen_Profile_Name);

      Set_PU_no_side_effects(Pu_Table[ST_pu(st)]);
      Set_PU_is_pure(Pu_Table[ST_pu(st)]);
#ifdef TARG_ST
      // [CG] Must set it to avoid instrumenting the instrumentation call
      // itself.
      Set_PU_no_instrument_function(Pu_Table[ST_pu(st)]);
#endif

      profcall = WN_Call(MTYPE_V, MTYPE_V, 5, st);
    }

    ST *current_fcn_st = WN_st(current_function);

    WN_actual(profcall, 0) = WN_Lda(Pointer_type, 0, current_fcn_st);
    name = ST_name(current_fcn_st);
    WN_actual(profcall, 2) = WN_LdaString(name, 0, strlen(name)+1);
    WN_actual(profcall, 4) = WN_Intconst(MTYPE_I4, state);

    if (WN_has_sym(tree))
    {
      ST *st = WN_st(tree);

      WN_actual(profcall, 1) = WN_Lda(Pointer_type, 0, st);
      name = ST_name(st);
      WN_actual(profcall, 3) = WN_LdaString(name, 0, strlen(name)+1);
    }
    else
    {
      Is_True((WN_operator_is(tree, OPR_ICALL)),
	      ("expected icall node, not %s", OPCODE_name(WN_opcode(tree))));

      WN_actual(profcall, 1)
	= lower_copy_tree(WN_actual(tree, WN_num_actuals(tree)), actions);
      WN_actual(profcall, 3) = WN_Zerocon(Pointer_type);
    }

#ifdef TARG_ST
    // No need do disable Gen_Profile as function is marked with
    // no_instrument_function.
    // Disable lowering of nested function pointers as we
    // need to pass the real functions addresses.
    LOWER_ACTIONS new_actions = actions & ~LOWER_NESTED_FN_PTRS;
    profcall = lower_call(block, profcall, new_actions);
#else
    Gen_Profile = FALSE;
    profcall = lower_call(block, profcall, actions);
    Gen_Profile = TRUE;
#endif

    return profcall;
  }
  return NULL;
}

/* ====================================================================
 *   WN_convert_to_pic
 *
 *   convert a CALL to PICCALL for the following cases:
 *
 *     1. we are generating PIC code.
 *     2. -TARG:force_jalr
 *     3. We need to setup $t9.
 *     4. We are generating CPIC code and the callee is visible 
 *        outside the dso.
 * ====================================================================
 */
WN *
WN_convert_to_pic (
  WN *tree,
  ST *callee_st
)
{
  INT16        n = WN_kid_count(tree);
  SRCPOS       srcpos = WN_Get_Linenum(tree);
  WN          *itree;

  /*
   *  convert (CALL sym args ...) into (PICCALL args ... (LDA sym)). 
   *  The LDA does not have an OP_PARM
   */
  itree = WN_Piccall( WN_rtype(tree), WN_desc(tree), n+1, callee_st);
  
  Is_True(callee_st == WN_st(tree),
	      ("lower_call: something changed that Robert didn't expect!"));
  WN_actual(itree, n) = WN_Lda(Pointer_type, 0, callee_st);
  WN_Set_Linenum(itree, srcpos);
  WN_Set_Flags(tree, itree);
  
  while (--n >= 0)
    WN_actual(itree, n) = WN_actual(tree, n);

  return itree;
}

#ifdef TARG_X8664
/* ====================================================================
 * take_out_special_ia32_ops -
 *
 * Descend the tree to find and unnest divide/rem/shift-by-variable-amount out
 * of parameter passing code because these operations require registers rdx 
 * or rcx which are parameter registers.  Also applying to F8U8CVT,
 * U8F4CVT and U8F8CVT because its expansion requires creating new basic blocks.
 *
 * ==================================================================== */
static WN *take_out_special_ia32_ops(WN *block, WN *tree)
{
  if (MTYPE_is_integral(WN_rtype(tree)) &&
       (WN_operator(tree) == OPR_DIV || 
        WN_operator(tree) == OPR_REM ||
        WN_operator(tree) == OPR_MOD ||
        ((WN_operator(tree) == OPR_ASHR ||
          WN_operator(tree) == OPR_LSHR ||
          WN_operator(tree) == OPR_SHL ||
          WN_operator(tree) == OPR_RROTATE) && 
         WN_operator(WN_kid(tree,1)) != OPR_INTCONST)) ||
      WN_operator(tree) == OPR_CVT && MTYPE_is_float(WN_rtype(tree)) &&
       (WN_desc(tree) == MTYPE_U8 && Is_Target_64bit() ||
        WN_desc(tree) == MTYPE_U4 && Is_Target_32bit()) ||
      Is_Target_64bit() && 
       (WN_opcode(tree) == OPC_U8F4TRUNC || WN_opcode(tree) == OPC_U8F8TRUNC)) {
      TYPE_ID mtype = WN_rtype(tree);
      TY_IDX ty_idx = MTYPE_To_TY(mtype);
      ST *temp_st = Gen_Temp_Symbol(ty_idx, ".unnested_parm");
      WN *stid = WN_Stid(mtype, 0, temp_st, ty_idx, tree);
      WN_Set_Linenum (stid, current_srcpos);
      WN_INSERT_BlockLast(block, stid);
      WN *ldid = WN_Ldid(mtype, 0, temp_st, ty_idx);
    return ldid;
  }
  /* continue looking */
  for (INT i = 0; i < WN_kid_count(tree); i++)
    WN_kid(tree,i) = take_out_special_ia32_ops(block, WN_kid(tree,i));
  return tree;
}
#endif

/* ====================================================================
 *
 * WN *lower_call(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on call statement <tree>,
 * returning lowered tree.
 *
 * ==================================================================== */

static WN *lower_call(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  INT16	       i;
  PLOC	       ploc;
  TY_IDX       call_ty;
  WN	      *callblock;
  SRCPOS       srcpos = WN_Get_Linenum(tree);
  INT	       num_actuals = WN_num_actuals(tree);
  ST          *callee_st = NULL;

  Is_True(OPERATOR_is_call(WN_operator(tree)),
	  ("expected call node, not %s", OPERATOR_name(WN_operator(tree))));
#ifdef TARG_ST
  // Fix for bug #23772. If lowering of actuals is done at that lower hilo
  // time, this means actuals are lowered two times and code are duplicated.
  // Thus, we block that lower for lower hilo action
  // [SC] Since later we only lower the kids from 0..num_actuals-1,
  // here lower the remaining kids, if any.  Note bug #51835: for an
  // indirect call, the address expression is also a kid that appears after
  // num_actuals; here we must do hilo-lowering on that kid.
  for (i = Action(LOWER_HILO) ? num_actuals : 0; i < WN_kid_count(tree); i++)
#else
  for (i = 0; i < WN_kid_count(tree); i++) 
#endif
  {
#ifdef KEY // handle actual parameter being a complex under LOWER_COMPLEX action
    if (WN_operator(WN_actual(tree,i)) == OPR_PARM &&
	MTYPE_is_complex(WN_rtype(WN_actual(tree,i))) && 
	!OPERATOR_is_leaf(WN_operator(WN_kid0(WN_actual(tree,i)))) &&
	Action(LOWER_COMPLEX))
    {
      TYPE_ID mtype = WN_rtype(WN_actual(tree,i));
      WN *parm_tree = WN_kid0(WN_actual(tree,i));
      TY_IDX ty_idx = MTYPE_To_TY(mtype);
      ST *temp_st = Gen_Temp_Symbol(ty_idx, ".complex_parm");
      WN *stid = WN_Stid(mtype, 0, temp_st, ty_idx, parm_tree);
      WN_Set_Linenum (stid, current_srcpos);
      stid = lower_stmt(block, stid, actions);

      WN_Set_Linenum (stid, current_srcpos);
      WN_INSERT_BlockLast(block, stid);
      WN_kid0(WN_actual(tree,i)) = WN_Ldid(mtype, 0, temp_st, ty_idx);
    }
    else
#endif
    WN_actual(tree, i) = lower_expr(block, WN_actual(tree, i), actions);
  }

  /* Get the ST of the callee if available */
  if (WN_has_sym(tree)) {
    callee_st = WN_st(tree); 
  }

#ifdef TARG_ST
  // [CL] If the statement following the call is NOT
  // a store of the return value, it means the call
  // must be turned into a void call
  WN *next=WN_next(tree);
  BOOL should_be_void = TRUE;

  if (next) {
    if ( (WN_operator(next) == OPR_STID) ||
	 (WN_operator(next) == OPR_ISTORE) ) {
      WN *rhs = WN_kid0(next);
      if (WN_opcode(rhs) == OPC_MMLDID && WN_st(rhs) == Return_Val_Preg) {
	should_be_void = FALSE;
      }
    }
  }
#endif
  
#ifdef TARG_X8664
  last_call_ff2c_abi = FALSE;
  if (WN_operator(tree) == OPR_CALL) 
    last_call_ff2c_abi =PU_ff2c_abi(Pu_Table[ST_pu(St_Table[WN_st_idx(tree)])]);
#endif
  if (WHIRL_Return_Val_On && Action(LOWER_RETURN_VAL) && 
#ifdef TARG_ST
      // [CL] handle the case where the call is not void,
      // but the return value is not used and the corresponding
      // store has been deleted by ipl
      (WN_rtype(tree) == MTYPE_V || should_be_void) &&
#else
      WN_rtype(tree) == MTYPE_V &&
#endif
      (WN_operator(tree) == OPR_CALL || WN_operator(tree) == OPR_ICALL ||
       WN_operator(tree) == OPR_PICCALL)) {
    // if it is a call returning struct coerced to void, may need to introduce
    // a fake first parameter
    TY_IDX prototype;
    if (WN_operator(tree) == OPR_ICALL) 
      prototype = WN_ty(tree);
    else {
      ST_IDX func_stidx = WN_st_idx(tree);
      PU_IDX puidx = ST_pu(St_Table[func_stidx]);
      prototype = PU_prototype(Pu_Table[puidx]);
    }
    RETURN_INFO return_info = Get_Return_Info(TY_ret_type(prototype),
                                              Complex_Not_Simulated
#ifdef TARG_X8664
					      , last_call_ff2c_abi
#endif
					      );
#ifdef TARG_ST
    /* (cbr) nrv formal is already created in the frontend */
    if (RETURN_INFO_return_via_first_arg(return_info) &&
        !TY_return_to_param(prototype)) {
#else
    if (RETURN_INFO_return_via_first_arg(return_info)
#ifdef KEY // bug 3852 temporary workaround
	&& (callee_st == NULL || strncmp(ST_name(callee_st),"_TRANSFER",9) != 0)
#endif
	) {
#endif
      ST *return_st = Gen_Temp_Symbol(TY_ret_type(prototype), ".vcall");
      WN *awn = WN_CreateLda(OPR_LDA, Pointer_Mtype, MTYPE_V, 0,
                             Make_Pointer_Type(TY_ret_type(prototype)), 
			     return_st);
      awn = lower_expr(block, awn, actions);
      WN *n_call = add_fake_parm(tree, awn, WN_ty(awn));
      WN_Delete(tree);
      tree = n_call;
    }
  }

  if (Action(LOWER_PICCALL))
  {
   /* convert a CALL to PICCALL for the following cases:
    *
    *  1. we are generating PIC code.
    *  2. -TARG:force_jalr
    *  3. We need to setup $t9.
    *  4. We are generating CPIC code and the callee is visible outside the
    *     dso.
    */
    if (WN_operator_is(tree, OPR_CALL)  &&
        (Force_Jalr                     ||
         PU_needs_t9(Pu_Table[ST_pu(callee_st)]) ||
         (Gen_PIC_Calls &&
          (Gen_PIC_Shared                       ||
          (Gen_PIC_Call_Shared &&
           ST_visible_outside_dso(*callee_st)) )))) 
    {
      INT16       n = WN_kid_count(tree);
      WN          *itree;
      /*
       *  convert (CALL sym args ...) into (PICCALL args ... (LDA sym)). 
       *  The LDA does not have an OP_PARM
       */
      itree = WN_Piccall( WN_rtype(tree), WN_desc(tree), n+1, callee_st);
  
      Is_True(callee_st == WN_st(tree),
	      ("lower_call: something changed that Robert didn't expect!"));
      WN_actual(itree, n) = WN_Lda(Pointer_type, 0, callee_st);
      WN_Set_Linenum(itree, srcpos);
      WN_Set_Flags(tree, itree);
  
      while (--n >= 0)
        WN_actual(itree, n) = WN_actual(tree, n);

      if (Cur_PU_Feedback) {
	Cur_PU_Feedback->FB_lower_call( tree, itree );
      }
      WN_Delete(tree);
      tree = itree;
    }
  }
#ifdef TARG_ST
  // 
  // Arthur: it seems necessary to insert the TAS nodes for function
  //         parameters as soon as possible, so that they are well
  //         exposed to subsequent lowerings (in part. RT_Lower).
  //

  //
  // Do it with LOWER_RETURN_VAL as it happens early enough
  // and we need the final "look" of this call
  //

  // 
  // The code bellow is duplicated further
  // TODO: clean up
  //
  call_ty = (WN_operator_is(tree,OPR_ICALL) ? WN_ty(tree) :
	     ST_pu_type(callee_st));
  Is_True(WN_operator_is(tree, OPR_ICALL) ||
	  callee_st == WN_st(tree),
	  ("lower_call: something changed that Robert didn't expect!"));

  if (Action(LOWER_RETURN_VAL)) {

#ifdef TARG_ST
    // [JV] When return struct via first hidden parameter, this parameter
    // is not yet lowered at this point. So we inform the parameter passing
    // simulation (targ_sim) in this case
    ploc = Setup_Output_Parameter_Locations(call_ty,FALSE);
#else
    ploc = Setup_Output_Parameter_Locations(call_ty);
#endif

    for (i = 0; i < num_actuals; i++) {
      WN       *parm = WN_actual(tree, i);
      TYPE_ID	parmType = WN_rtype(parm);
      WN       *actual = WN_operator_is(parm, OPR_PARM) ? WN_kid0(parm)
                                                         : parm;

      if (MTYPE_is_m(parmType)) {
	/*
	 * structure parameter
	 */
	ploc = Get_Output_Parameter_Location( TY_Of_Parameter(parm));
      }
      else if (MTYPE_is_complex(parmType) && PU_ftn_lang(Get_Current_PU())) {
	/*
	 * Fortran by value can generate this construct
	 * we must make it look like a structure parameter
	 * Note that GCC has complex type, which should go to
	 * normal processing (like quads).
	 */
	ploc = Get_Output_Parameter_Location( TY_Of_Parameter(parm));
      }
      else {
	TY_IDX ty_idx;
	TYPE_ID type;

	ploc = Get_Output_Parameter_Location( MTYPE_To_TY(parmType));
	/*
	 * float parm goes in int register, so convert
	 */
	if (MTYPE_float(parmType) && Preg_Offset_Is_Int(PLOC_reg(ploc))) {
	  /*
	   * if parmType is F4, generate a I4TAS(F4LOAD), else
	   * if parmType is F8, generate a I8TAS(F8LOAD)
	   */
	  if (parmType == MTYPE_F4) {
	    type = MTYPE_I4;
	    ty_idx = Be_Type_Tbl(MTYPE_I4);
	  }
	  else if (parmType == MTYPE_F8) {
	    type = MTYPE_I8;
	    ty_idx = Be_Type_Tbl(MTYPE_I8);
	  }
	  else {
	    FmtAssert(FALSE,("can't handle"));
	  }

	  // make PARM -> TAS
	  WN_kid0(parm) = WN_Tas(type, ty_idx, WN_kid0(parm));
	  WN_set_rtype(parm, type);
	  WN_set_ty(parm, ty_idx);
	}
      }
    }
  }
#endif

  
  if (NotAction(LOWER_CALL))
    return tree;

#ifdef TARG_ST
  /* [CG] Instrumentation of call entries. */
  if (Action(LOWER_INSTRUMENT) && lower_profile_call_enabled(tree)) {
    WN  *profcall = lower_profile_call(block, tree, 0, actions);
    DevAssert(profcall != NULL, ("lower_call(): unexpected NULL tree after lower_profile_call()"));
#ifdef TARG_ST
    // [TB] Improve line number.
    WN_copy_linenum(tree, profcall);
#endif
    WN_INSERT_BlockLast(block, profcall);
  }
#else
 /*
  *  For each function generate a call to a profiler function
  *	__prof( &func, &call, funcname, callname);
  */
  if (Gen_Profile)
  {
    WN  *profcall = lower_profile_call(block, tree, 0, actions);
    WN_Set_Linenum(profcall, current_srcpos);
    WN_INSERT_BlockLast (block, profcall);
  }
#endif
  callblock = WN_CreateBlock();
#ifdef TARG_ST
  WN *memcpyBlock =  WN_CreateBlock();
#endif
#ifdef TARG_X8664
  // extra pass to unnest divide/rem/shift-by-variable-amount because these
  // operations require registers rdx or rcx which are parameter registers
  for (i = 0; i < num_actuals; i++) 
    WN_actual(tree, i) = take_out_special_ia32_ops(callblock, WN_actual(tree, i));
#endif

  /*
   * TODO:
   * (a) Just before each call passing a struct by value, if (part of) the
   *     struct can be passed in register(s), convert MLOAD into multiple
   *     ILOADs.  MEDIUM PRIORITY (at least detect condition)
   * (b) Just before each call, pass static link to nested calls.
   * (d) For -o32 -shared, just after returning from a call, restore $gp
   *     (but see bug 159929).
   */

  /*
   * Create block containing stores of actual loads into pregs.
   * The call can continue to have the original actuals, because 
   * we don't use that except to see the number of parameters. 
   *
   * New:  Go through the list twice to give localize an easier job
   *	   First handle structures (there may be loop code to move the
   *       structure)
   */
  call_ty = (WN_operator_is(tree,OPR_ICALL) ? WN_ty(tree) :
	     ST_pu_type(callee_st));
  Is_True(WN_operator_is(tree, OPR_ICALL) ||
	  callee_st == WN_st(tree),
	  ("lower_call: something changed that Robert didn't expect!"));
  ploc = Setup_Output_Parameter_Locations(call_ty);

  for (i = 0; i < num_actuals; i++)
  {
    WN		*parm = WN_actual(tree, i);
    TYPE_ID	parmType = WN_rtype(parm);
    WN		*actual = WN_operator_is(parm, OPR_PARM) ? WN_kid0(parm)
                                                         : parm;

    if (MTYPE_is_m(parmType))
    {
     /*
      * structure parameter
      */
      ploc = Get_Output_Parameter_Location( TY_Of_Parameter(parm));
#ifdef TARG_ST
      lower_mload_actual (callblock, memcpyBlock, parm, actual, ploc, actions);
#else
      lower_mload_actual (callblock, actual, ploc, actions);
#endif
    }
#ifndef TARG_X8664
    else if (MTYPE_is_complex(parmType) && PU_ftn_lang(Get_Current_PU()))
    {
     /*
      * Fortran by value can generate this construct
      * we must make it look like a structure parameter
      * Note that GCC has complex type, which should go to
      * normal processing (like quads).
      */
      ploc = Get_Output_Parameter_Location( TY_Of_Parameter(parm));
      lower_complex_actual (callblock, actual, ploc, actions);
    }
#endif
    else
    {
      ploc = Get_Output_Parameter_Location( MTYPE_To_TY(parmType));
    }
  }

#ifdef TARG_X8664
  INT sse_args = 0;	/* count number of parameters passed in SSE regs */
#endif

  ploc = Setup_Output_Parameter_Locations(call_ty);
  for (i = 0; i < num_actuals; i++)
  {
   /*
    * We need to get the TY of the parameter: 
    * if no prototype, then we don't have a list of formals;
    * the actual may need to be converted if there is a prototype;
    * the rtype of the opcode is the converted type, so use it. 
    */
    WN         *parm = WN_actual(tree, i);
    TYPE_ID     parmType = WN_rtype(parm);
    WN         *actual = WN_operator_is(parm, OPR_PARM) ? WN_kid0(parm) : parm;
    TY_IDX      ty;

    if (MTYPE_is_m(parmType) 
	|| (MTYPE_is_complex(parmType) && PU_ftn_lang(Get_Current_PU())) )
    {
     /*
      * already processed
      */
      ploc = Get_Output_Parameter_Location( TY_Of_Parameter(parm));
#ifdef TARG_X8664
      if (Preg_Offset_Is_Float(PLOC_reg(ploc))) 
        sse_args++;
      if (Preg_Offset_Is_Float(PLOC_reg2(ploc))) 
	sse_args++;
#endif
      continue;
    }

    ty =  TY_Of_Parameter(parm);
    ploc = Get_Output_Parameter_Location( MTYPE_To_TY(parmType));
    {
     /*
      *  canonicalize [I,U][1,2] types (to [I,U]4
      */
      TYPE_ID  type = Mtype_comparison( Fix_TY_mtype(ty));

      if (parmType != type)
      {
	DevWarn("lower_call(): line %d, parm #%d type mismatch (WN_rtype(parm)"
		" = %s) (cannonical TY_mtype(parm))) %s)",
		Srcpos_To_Line(WN_Get_Linenum(tree)), i,
		Mtype_Name(parmType), Mtype_Name(type));
      }
    }

#ifdef TARG_X8664
    if (Preg_Offset_Is_Float(PLOC_reg(ploc))) 
      sse_args++;
    if (Preg_Offset_Is_Float(PLOC_reg2(ploc))) 
      sse_args++;
#endif

    if (PLOC_on_stack(ploc))
    {
     /*
      * stack offset 
      * We can use the preg for $sp here, but a simpler way
      * is to use the SP symbol. 
      */
      WN *wn;
#ifdef TARG_ST
      wn = WN_Stid(parmType, PLOC_offset(ploc) + STACK_OFFSET_ADJUSTMENT,
     		   SP_Sym, ty, actual);
#else
      wn = WN_Stid(parmType, PLOC_offset(ploc) - Formal_Save_Area_Size
		             + Stack_Offset_Adjustment,
		   SP_Sym, ty, actual);
#endif
      WN_Set_Linenum(wn, srcpos);
      WN_INSERT_BlockLast(callblock, wn);
    }
   /*
    * special case (as quad -> integer doesn't make much sense).
    * Store argument into temp preg float parm goes in int register, so convert
    */
    else if (MTYPE_is_quad(parmType) && Preg_Offset_Is_Int(PLOC_reg(ploc)))
    {
      PREG_NUM qN;

      qN = AssignExpr(callblock, actual, parmType);

      lower_actual(callblock, WN_LdidPreg(MTYPE_F8, qN),   MTYPE_F8,
		   PLOC_reg(ploc));

      lower_actual(callblock, WN_LdidPreg(MTYPE_F8, qN+1), MTYPE_F8,
		   PLOC_reg(ploc)+1);
    }
#ifdef TARG_X8664
    else if ((parmType == MTYPE_U8 || parmType == MTYPE_I8) &&
             Preg_Offset_Is_Int(PLOC_reg2(ploc)))
    {
      TYPE_ID type = Mtype_TransferSign (parmType, MTYPE_U4);
      ST * actual_sym;
      if (!WN_has_sym(actual) ||
          ST_sym_class(WN_st(actual)) != CLASS_VAR)
      {
        ST * sym = Gen_Temp_Symbol (MTYPE_TO_TY_array[type], "_actual_");
        WN_INSERT_BlockFirst (callblock,
                              WN_Stid(type, 0, sym, ST_type(sym), actual));
        actual_sym = sym;
      }
      else
        actual_sym = WN_st(actual);
      lower_actual(callblock,
                   WN_Ldid(type, 0, actual_sym, MTYPE_TO_TY_array[type]),
                   type, PLOC_reg(ploc));
      lower_actual(callblock,
                   WN_Ldid(type, 4, actual_sym, MTYPE_TO_TY_array[type]),
                   type, PLOC_reg2(ploc));
    }
#endif
    else
    {
      actual = lower_actual(callblock, actual, parmType, PLOC_reg(ploc));

      if (WN_operator_is(parm, OPR_PARM))
	WN_kid0(parm) = actual;
      else
	WN_actual(tree, i) = actual;

      if (! TY_has_prototype(call_ty)        &&
	  DEBUG_Varargs_Prototypes == FALSE  &&
	  Preg_Offset_Is_Float(PLOC_reg(ploc)))
      {
	actual = lower_copy_tree(actual, actions);

	if (MTYPE_is_quad(parmType))
	{
	  PREG_NUM  qN;

	  qN = AssignExpr(callblock, actual, parmType);

	  lower_actual(callblock, WN_LdidPreg(MTYPE_F8, qN),   MTYPE_F8,
		       PLOC_vararg_reg(ploc));
	  lower_actual(callblock, WN_LdidPreg(MTYPE_F8, qN+1), MTYPE_F8,
		       PLOC_vararg_reg(ploc)+1);
	}
	else
	{
	  lower_actual(callblock, actual, parmType, PLOC_vararg_reg(ploc));
	}
      }
    }
  }

#ifdef TARG_X8664
  if( Is_Target_64bit() ){
    // ST_sclass(callee_st) would be SCLASS_TEXT if the function defined earlier
    if (TY_is_varargs(call_ty) ||
	(! TY_has_prototype(call_ty) && 
	 (callee_st == NULL || ST_sclass(callee_st) != SCLASS_TEXT))) {
      WN *sse_cnt_wn = WN_Intconst(MTYPE_I4, MIN(8, sse_args));
      WN *sse_cnt_stid = WN_StidIntoPreg(MTYPE_I4, RAX, Int32_Preg, sse_cnt_wn);
	
      WN_Set_Linenum(sse_cnt_stid, srcpos);
      WN_INSERT_BlockLast(callblock, sse_cnt_stid);
    }
  }
#endif

 /*
  * store param size in tree for later re-use 
  * unless has no prototype (then could have varying # args). 
  */
  if (num_actuals > 0 &&
      TY_has_prototype(call_ty) &&
      Get_PU_arg_area_size(call_ty) == 0 &&
      ! TY_is_varargs(call_ty))
  {
      Set_PU_arg_area_size(call_ty, PLOC_total_size(ploc));
  }


 /*
  *  copy the arguments to avoid building a dag
  *  (region code might delete these nodes)
  *  CG will use these arguments to build register usage information
  */
  {
    INT16       n= num_actuals;

    while (--n >= 0)
      WN_actual(tree, n) = lower_copy_tree( WN_actual(tree, n), actions);
  }

  if (MTYPE_is_complex(WN_rtype(tree))
#ifdef TARG_X8664 // fortran complex treated as structs (bug 1664)
      && !(last_call_ff2c_abi || F2c_Abi)
#endif
     )
  {
    INT16       n = WN_kid_count(tree);
    WN          *itree;
    TYPE_ID	rtype, desc;
    
#ifdef TARG_X8664
    if (Is_Target_64bit() && WN_rtype(tree) == MTYPE_C4)
      rtype = MTYPE_F8;
    else
#endif
    rtype = Mtype_complex_to_real(WN_rtype(tree));
    desc = Mtype_complex_to_real(WN_desc(tree));

   /*
    *  For complex the call type will be changed by WN_Call()
    *  into (PICCALL args ... (LDA sym)).
    */
    if (WN_operator_is(tree, OPR_CALL))
    {
      itree = WN_Call( rtype, desc, n, WN_st(tree));
    }
    else if (WN_operator_is(tree, OPR_ICALL))
    {
      itree = WN_Icall( rtype, desc, n, WN_ty(tree));
    }
    WN_Set_Linenum(itree, srcpos);
    WN_Set_Flags(tree, itree);

    while (--n >= 0)
      WN_actual(itree, n) = WN_actual(tree, n);

    if (Cur_PU_Feedback) {
      Cur_PU_Feedback->FB_lower_call( tree, itree );
    }
    WN_Delete(tree);
    tree = itree;
  }

  /*
   *  F90 needs this 
   *  The callee will require the current FP or Slink in $2
   */
  if (Action(LOWER_UPLEVEL))
  {
    if (callee_st && PU_is_nested_func(Pu_Table[ST_pu(callee_st)]))
    {
      WN  *wn, *link;
     
     /*
      *  These are "downlevel" calls for F90 (ex mp procedures),
      *  or uplevel (child calls parent)
      *  This should only happen for F90.
      */
      if (PU_lexical_level(&St_Table[WN_st_idx(current_function)]) <
	  PU_lexical_level(callee_st))
      {
	link = WN_LdidPreg(Pointer_type, Frame_Pointer_Preg_Offset);
      }

     /*
      *  Need to set up the register for "sideways" (internal calling internal)
      *  and uplevel calls. This should only happen for F90.
      */
      else 
      {
	ST  *slink = Find_Slink_For_Scope(WN_st(current_function),
					  callee_st);

	link = WN_Ldid (Pointer_type, 0, slink, ST_type(slink));
      }

      wn = WN_StidIntoPreg(Pointer_type,
			   Static_Link_Preg_Offset,
			   MTYPE_To_PREG(Pointer_type), link);
	  
      WN_Set_Linenum(wn, srcpos);
      WN_INSERT_BlockLast(callblock, wn);
     }
  }

  /* 
   * Check that stack space needed for actuals doesn't overflow 
   * what we initially reserved. 
   */
  Check_Actual_Stack_Size (tree);

  callblock = lower_block(callblock, actions);
#ifdef TARG_ST
  if (memcpyBlock && WN_block_nonempty(memcpyBlock)) {
    memcpyBlock = lower_block(memcpyBlock, actions);
    WN_INSERT_BlockLast (block, memcpyBlock);
  }
#endif

#ifdef TARG_ST
  // [TB] Improve line number.
  WN_copy_linenum(tree, callblock);
#endif
  WN_INSERT_BlockLast (block, callblock);

  /******************************************************
	STUPID: cannot generate call here as it will affect the return value
	for this function
  if (Gen_Profile)
  {
    WN_INSERT_BlockLast (block, tree);

    return lower_profile_call(block, tree, 1, actions);
  }
  ******************************************************/

  return tree;
}




/* ====================================================================
 *
 * WN *lower_compgoto(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on switch val in COMPGOTO
 * node <tree>, returning XGOTO.
 *
 * If there is a default label, code is generated to test the range
 * of switch values
 * ==================================================================== */

static WN *lower_compgoto(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  INT32		pointerSize = MTYPE_RegisterSize(Pointer_type);
  INT32		num_entries = WN_num_entries(tree);
  WN		*index, *add;
  ST		*table_st;
  LEAF		indexN, addN;

  Is_True(WN_opcode(tree) == OPC_COMPGOTO,
	  ("expected COMPGOTO node, not %s", OPCODE_name(WN_opcode(tree))));
  Is_True(WN_num_entries(tree) > 0,
	  ("expected COMPGOTO nu,_entries > 0 not %d", WN_num_entries(tree)));

  WN_kid0(tree) = lower_expr(block,
#ifndef KEY
			     WN_Coerce(Pointer_type, WN_kid0(tree)),
#else // avoid generating U8I4CVT which is problematic
			     WN_Coerce(Mtype_TransferSign(WN_rtype(WN_kid0(tree)), Pointer_type), WN_kid0(tree)),
#endif
			     actions);

  if (NotAction(LOWER_COMPGOTO))
    return tree;

  index = WN_kid0(tree);

  indexN = Make_Leaf(block, 
#ifndef KEY
		     WN_Coerce(Pointer_type, index), 
#else // avoid generating U8I4CVT which is problematic
		     WN_Coerce(Mtype_TransferSign(WN_rtype(index), Pointer_type), index),
#endif
		     Pointer_type);

 /*
  * create fake, incomplete array type so table_st has correct size
  *
  *  generate jump for computed goto
  *    agoto ( & table + (i * sizeof(Pointer_type)));
  *
  *  create
  *    addN = table + (i * sizeof(Pointer_type));
  */
  {
    TY_IDX table;
    WN	*mpy;

    table = Make_Array_Type(Pointer_type, 1, num_entries);

    table_st = Gen_Read_Only_Symbol (table, "jump_table");
    Set_ST_is_initialized(table_st);	/* so goes in rdata section */
    
    mpy = WN_Mpy(Pointer_type, 
		 Load_Leaf(indexN),
		 WN_Intconst(Pointer_type, pointerSize));

    add = WN_Add(Pointer_type,
		 WN_Lda (Pointer_type, 0, table_st),
		 mpy);
    addN = Make_Leaf(block, add, Pointer_type);
  }
  
 /*
  *  for default cases we must verify the index fall in range
  */
  WN *wn_truebr = NULL;
  if (WN_kid_count(tree) == 3)
  {
   /*
    *  branch to default if (index >= n)
    *  because the indexN is unsigned, we do not have to compare 
    *  greater then zero
    */
    WN  *ge, *truebr;
    WN	*def  = WN_kid2(tree);

    ge =  WN_GE(Pointer_type,
		Load_Leaf(indexN),
		WN_Intconst(Pointer_type, num_entries));

    truebr = lower_truebr(WN_label_number(def), ge, &wn_truebr, actions);
#ifdef TARG_ST
    // [TB] Improve line number.
    WN_copy_linenum(tree, truebr);
#endif
    WN_INSERT_BlockLast(block, truebr);
  }

 /*
  * create fake, incomplete array type so table_st has correct size
  *
  *  generate jump for computed goto
  *    agoto ( & table + (i * sizeof(Pointer_type)));
  */
  {
    WN  *xgoto;
    WN	*gotoTable = WN_kid1(tree);

    index = WN_Iload(Pointer_type,
		     0,
		     MTYPE_To_TY(Pointer_type),
		     Load_Leaf(addN));

    index = lower_expr(block, index, actions);

    xgoto = WN_CreateXgoto(num_entries, index, gotoTable, table_st);
    WN_Set_Linenum (xgoto, WN_Get_Linenum(tree));

    // Lower feedback info
    if ( Cur_PU_Feedback )
      Cur_PU_Feedback->FB_lower_compgoto( tree, xgoto, wn_truebr );

    return xgoto;
  }
}




/* ====================================================================
 *
 * WN *lower_assert(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on OPC_ASSERT
 * node <tree>, returning structured if
 *
 * ==================================================================== */

static WN *lower_assert(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  WN  *if_else, *if_then, *IF, *trap, *cond;

  Is_True(WN_opcode(tree) == OPC_ASSERT,
	  ("expected ASSERT node, not %s", OPCODE_name(WN_opcode(tree))));

  WN_kid0(tree) = lower_expr(block, WN_kid0(tree), actions);

  if (NotAction(LOWER_ASSERT))
    return tree;

  if_then = WN_CreateBlock();
  if_else = WN_CreateBlock();
  cond = lower_expr(block, WN_kid0(tree), actions);

 /*
  * generate intrinsic call 
  */
  switch ( WN_offset(tree) ) {
    case WN_TRAP_F77_BOUNDS_ERROR:
      {
	char *name;
	WN **kids = (WN **) alloca(4 * sizeof(WN *));

	/* add blank at end of string cause s_rnge wants one */
	kids[0] = WN_LdaString ( "? ", 0, 2 );
	kids[1] = WN_Intconst ( MTYPE_I4, -1 );	/* cause of s_rnge weirdness */

	/* s_rnge expects name with _ at end */
	name = ST_name(&St_Table[PU_Info_proc_sym(Current_PU_Info)]);
	kids[2] = WN_LdaString ( name, 0, strlen(name)+1 );

	kids[3] = WN_Intconst(MTYPE_I4, Srcpos_To_Line(WN_Get_Linenum(tree)));
	trap = WN_Create_Intrinsic ( OPC_VINTRINSIC_CALL,
				     INTRN_F77_BOUNDS_ERR, 4, kids );
      } 
      break;

    case WN_TRAP_C_BOUNDS_ERROR:
      /* Check DEBUG_Verbose_Runtime.  If set, call __C_runtime_error;
       * otherwise emit a BRK_RANGE trap.
       */
      /* TODO PV 443095:  This doesn't have descriptive parameters due
       * to limitations on the ASSERT node.  Fix that eventually.
       */
      if ( DEBUG_Verbose_Runtime ) {
	char *name;
	WN **kids = (WN **) alloca(4 * sizeof(WN *));

	/* __C_runtime_error ( BRK_RANGE, PU_name, line_no, fmt, ...);
	 */
#ifdef ORIGINAL_SGI_CODE
	kids[0] = WN_Intconst ( MTYPE_I4, BRK_RANGE );
#else
	fprintf(stderr, "Don't know how to do BRK_RANGE\n");
	exit(-1);
#endif

	/* Source routine where error occurs: */
	name = ST_name(&St_Table[PU_Info_proc_sym(Current_PU_Info)]);
	kids[1] = WN_LdaString ( name, 0, strlen(name)+1 );

	/* Source line where error occurs: */
	kids[2] = WN_Intconst ( MTYPE_I4,
				Srcpos_To_Line(WN_Get_Linenum(tree)) );

	/* Eventually, this should be a printf format and arguments,
	 * describing the case that fails (i.e. array and index).
	 * For now, the ASSERT node doesn't have the right kids.
	 */
	kids[3] = WN_LdaString ( "unknown array", 0, 14 );

	/* Make the call: */
	trap = WN_Create_Intrinsic ( OPC_VINTRINSIC_CALL,
				     INTRN_RT_ERR, 4, kids );
      } else {
#ifdef ORIGINAL_SGI_CODE
	trap = WN_CreateTrap ( BRK_RANGE );
#else   
	fprintf(stderr, "Don't know how to do BRK_RANGE\n");
	exit(-1);
#endif  

	// ADD FEEDBACK INFO ??
      }
      break;

    default:
      trap = WN_CreateTrap ( WN_offset(tree) );
      // ADD FEEDBACK INFO ??
      break;
  }

  WN_Set_Linenum(trap, WN_Get_Linenum(tree));
  WN_INSERT_BlockLast(if_else, trap);

  IF = WN_CreateIf(cond, if_then, if_else);
  // ADD FEEDBACK INFO !!
  WN_Set_Linenum(IF, WN_Get_Linenum(tree));
  WN_Delete(tree);

  IF = lower_scf(block, IF, actions);
  return IF;
}


static WN *lower_branch(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  Is_True((WN_opcode(tree) == OPC_TRUEBR || WN_opcode(tree) == OPC_FALSEBR),
	  ("expected true/false"));

#ifdef TARG_ST
  // 
  // Arthur: we may have the branch cond I8/U8, in this case
  //         lower hilo and only use the lo part
  //
  WN *kid;
  if (Action(LOWER_HILO) && 
#ifdef NEW_LOWER
      MTYPE_is_longlong(WN_rtype(WN_kid0(tree))) &&
      is_hilo_type(WN_rtype(WN_kid0(tree)))
#else
      MTYPE_is_longlong(WN_rtype(WN_kid0(tree)))
#endif
      ) {
    WN *lo, *hi;
    lower_hilo_expr(block, 
		    WN_kid0(tree), 
		    actions, 
		    &hi, 
		    &lo 
		    /* WN_rtype(WN_kid0(tree))*/);
    kid = lo;
  }
  else {
    kid = lower_expr(block, WN_kid0(tree), actions);
  }
#else
  WN * const kid = lower_expr(block, WN_kid0(tree), actions);
#endif
  WN_kid0(tree) = kid;

#ifdef TARG_ST
  // [CG] In the case where we have a convert just before the branch
  // to a larger type we can remove it.
  // For instance FLASBR(U8U4CVT U4expr) can be replaced by FLASEBR(U4expr),
  // this sequence can be generated by the RT lower phase on comparisons
  // that are expansed into intrinsic call.
  if (WN_operator(kid) == OPR_CVT &&
      MTYPE_is_integral(WN_rtype(kid)) &&
      MTYPE_is_integral(WN_desc(kid)) &&
      MTYPE_byte_size(WN_rtype(kid)) >=
      MTYPE_byte_size(WN_desc(kid))) {
    WN_kid0(tree) = WN_kid0(kid);
    WN_Delete(kid);
    kid = WN_kid0(tree);
  }
#endif

#ifndef KEY
  if (WN_operator(kid) == OPR_LNOT)
#else
    // Bugs 5220, 5286 (preserve ordered comparisons at WHIRL level because
    // CG can not recognize the intended order).
  if (WN_operator(kid) == OPR_LNOT &&      
      (!Force_IEEE_Comparisons ||
       !OPCODE_is_compare(WN_opcode(WN_kid0(kid))) ||
       MTYPE_is_integral(WN_desc(WN_kid0(kid)))))
#endif
  {
    switch(WN_opcode(tree))
    {
    case OPC_TRUEBR:
      WN_set_opcode(tree, OPC_FALSEBR);
      break;

    case OPC_FALSEBR:
      WN_set_opcode(tree, OPC_TRUEBR);
      break;

    default:
      break;
    }

    WN_kid0(tree) = WN_kid0(kid);
    WN_Delete(kid);
  }

  return tree;
}
 
 
/* ====================================================================
 *
 * WN *lower_return_val(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on RETURN_VAL nodes
 * turning kid0 expression into an appropriate store and the RETURN_VAL
 * into a RETURN.
 *
 * ==================================================================== */

static WN *lower_return_val(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  PREG_NUM    preg;
  ST         *preg_st;
  TYPE_ID     mtype;
  RETURN_INFO return_info;
  WN         *wn;

  Is_True((WN_operator(tree) == OPR_RETURN_VAL),
	  ("expected RETURN_VAL node, not %s", OPCODE_name(WN_opcode(tree))));

  TYPE_ID return_mtype = WN_rtype(tree);

  if (return_mtype != MTYPE_M) {
    TY_IDX return_ty_idx = Be_Type_Tbl(return_mtype);
    return_info = Get_Return_Info (return_ty_idx, Use_Simulated
#ifdef TARG_X8664
    				              ,	PU_ff2c_abi(Get_Current_PU())
#endif
    				  );

    WN_kid0(tree) = lower_expr (block, WN_kid0(tree), actions);
    if (RETURN_INFO_return_via_first_arg(return_info)) { // fake first parm
      // This happens, for example, if a complex result is returned for ia32.
      // Create an istore; an mstore with a single complex value seemed
      // to confuse things later on.
      WN *first_formal = WN_formal(current_function, 0);
      TY_IDX tidx = ST_type(WN_st(first_formal));
      OPCODE const_op = OPCODE_make_op(OPR_INTCONST, MTYPE_I8, MTYPE_V);
      // WN *swn = WN_CreateIntconst (const_op, TY_size(return_ty_idx));
      WN *awn = WN_CreateLdid(OPR_LDID, 
			      TY_mtype(Ty_Table[tidx]), 
			      TY_mtype(Ty_Table[tidx]),
			      WN_idname_offset(first_formal), 
			      WN_st(first_formal), 
			      tidx);
      // wn  = WN_CreateMstore (0, tidx, WN_kid0(tree), awn, swn);
      wn = WN_CreateIstore(OPR_ISTORE, MTYPE_V,
			   return_mtype, 0, tidx, WN_kid0(tree), awn);
    } else {  // Returned in preg
      Is_True(RETURN_INFO_count(return_info) == 1,
	      ("expected return_info count to be 1"));

#ifdef TARG_ST
      //
      // Arthur: if we're passing floating point results in integer
      //         registers, need to make a TAS
      // TODO: perhaps we'll need to generalize mapping of floating
      //       point types to integer types in mtypes.cxx ??
      //
      preg_st = MTYPE_To_PREG(mtype);

      if (MTYPE_is_float(return_mtype) && Preg_Offset_Is_Int(preg)) {
	wn  = WN_CreateStid (OPR_STID, MTYPE_V, Promoted_Mtype[mtype], 
	  		   preg, preg_st, Be_Type_Tbl(mtype), 
			   WN_Tas(mtype, Be_Type_Tbl(mtype), WN_kid0(tree)));
      }
      else {
	wn  = WN_CreateStid (OPR_STID, MTYPE_V, Promoted_Mtype[mtype], 
	  		   preg, preg_st, Be_Type_Tbl(mtype), 
			   WN_kid0(tree));
      }
#else
      mtype   = RETURN_INFO_mtype (return_info, 0);
      preg    = RETURN_INFO_preg (return_info, 0);
      if (MTYPE_is_float(mtype))
#ifndef TARG_X8664
	preg_st = Float_Preg;
#else
	preg_st = (mtype==MTYPE_FQ || mtype==MTYPE_F16 ||
		   mtype==MTYPE_CQ || mtype==MTYPE_C16)
	  ? MTYPE_To_PREG(mtype) : Float_Preg;
#endif
      else preg_st = (mtype==MTYPE_I8 || mtype==MTYPE_U8) ? 
	  		MTYPE_To_PREG(mtype) : Int_Preg;

#ifdef TARG_X8664
      if( Is_Target_SSE2()         &&
	  Preg_Offset_Is_X87(preg) &&
	  mtype != MTYPE_C4 &&
	  !MTYPE_is_quad( mtype ) ){
	WN_kid0(tree) = WN_Cvt( Promoted_Mtype[mtype], MTYPE_FQ, WN_kid0(tree) );
	wn = WN_CreateStid( OPR_STID, MTYPE_V, MTYPE_FQ,
			    preg, preg_st, Be_Type_Tbl(mtype), WN_kid0(tree) );
      } else
#endif // TARG_X8664
      wn  = WN_CreateStid (OPR_STID, MTYPE_V, Promoted_Mtype[mtype], 
	  		   preg, preg_st, Be_Type_Tbl(mtype), WN_kid0(tree));
#endif
    }

    WN_Set_Linenum(wn, current_srcpos);
    WN_INSERT_BlockLast (block, wn);
  }
  else { // MTYPE_M; rhs is one of MLDID or MILOAD or MLOAD
    WN *o_rhs = WN_kid0(tree); 
    OPERATOR rhs_opr = WN_operator(o_rhs);
    TY_IDX ty_idx = WN_ty(o_rhs);
    if (OPERATOR_is_load(rhs_opr) && WN_field_id(o_rhs) != 0) {
      if (rhs_opr == OPR_ILOAD || rhs_opr == OPR_LDID) 
        ty_idx = get_field_type(ty_idx, WN_field_id(o_rhs));
      else { // OPR_MLOAD
        ty_idx = TY_pointed(Ty_Table[ty_idx]);
        ty_idx = get_field_type(ty_idx, WN_field_id(o_rhs));
      }
    }
    return_info = Get_Return_Info(ty_idx, Complex_Not_Simulated
#ifdef TARG_X8664
				  , PU_ff2c_abi(Get_Current_PU())
#endif
    				 );

    if (RETURN_INFO_return_via_first_arg(return_info)) { // fake first parm
      Is_True(RETURN_INFO_count(return_info) == 0, 
	      ("expected RETURN_INFO to have 0 count"));
      WN *n_rhs;

      // fix rhs
      if (WN_operator(o_rhs) == OPR_LDID)
        n_rhs = lower_mldid(block, o_rhs, LOWER_MLDID_MSTID);
      else if (WN_operator(o_rhs) == OPR_ILOAD) 
        n_rhs = lower_miload(block, o_rhs, LOWER_MLDID_MSTID);
      else n_rhs = o_rhs; 		// MLOAD

      // create an mstore
      WN *first_formal = WN_formal(current_function, 0);
      TY_IDX tidx = ST_type(WN_st(first_formal));
      WN *awn = WN_CreateLdid(OPR_LDID, 
			      TY_mtype(Ty_Table[tidx]), 
			      TY_mtype(Ty_Table[tidx]),
			      WN_idname_offset(first_formal), 
			      WN_st(first_formal), 
			      tidx);
      WN *swn = WN_CopyNode(WN_kid1(n_rhs));
      wn  = WN_CreateMstore (0, tidx, n_rhs, awn, swn);
      WN_Set_Linenum(wn, current_srcpos);  // Bug 1268
      WN_INSERT_BlockLast (block, wn);
    }
    else { // return in return registers
      INT32 i;
      WN *n_rhs;
      UINT algn;
      TY_IDX ty_idx_used;

      if (WN_operator(o_rhs) == OPR_LDID) {
	Is_True(WN_rtype(o_rhs) == MTYPE_M,
		("expected RETURN_VAL kid not type M"));

	algn = TY_align(ty_idx);
	for (i = 0; i < RETURN_INFO_count(return_info); i++) {
	  mtype = RETURN_INFO_mtype(return_info, i);
	  ty_idx_used = Be_Type_Tbl(mtype);
	  Set_TY_align(ty_idx_used, algn);
	  preg    = RETURN_INFO_preg (return_info, i);
	  preg_st = MTYPE_is_float(mtype) ? Float_Preg : Int_Preg;
#ifdef KEY // bug 12812
  	  if (WN_opcode(o_rhs) == OPC_MMLDID && WN_st(o_rhs) == Return_Val_Preg)
	    n_rhs = WN_CreateLdid(OPR_LDID, mtype, mtype, preg, preg_st, 
			          Be_Type_Tbl(mtype));
	  else
#endif
	  n_rhs = WN_CreateLdid (OPR_LDID, mtype, mtype, 
				 WN_load_offset(o_rhs)
				   + i * MTYPE_byte_size(mtype),
				 WN_st_idx(o_rhs), ty_idx_used);
	  wn = WN_CreateStid(OPR_STID, MTYPE_V, mtype, preg, preg_st, 
			     Be_Type_Tbl(mtype), n_rhs);
	  WN_Set_Linenum(wn, current_srcpos);  // Bug 1268
	  WN_INSERT_BlockLast (block, wn);
	}
      }
      else if (WN_operator(o_rhs) == OPR_ILOAD) {
	Is_True(WN_rtype(o_rhs) == MTYPE_M,
		("expected RETURN_VAL kid not type M"));

	algn = TY_align(ty_idx);
	for (i = 0; i < RETURN_INFO_count(return_info); i++) {
          mtype = RETURN_INFO_mtype(return_info, i);
	  ty_idx_used = Be_Type_Tbl(mtype);
	  Set_TY_align(ty_idx_used, algn);
	  if (i == 0)
	    n_rhs = WN_kid0(o_rhs);
	  else n_rhs = WN_COPY_Tree(WN_kid0(o_rhs));
	  n_rhs = WN_CreateIload(OPR_ILOAD, mtype, mtype,
				 WN_load_offset(o_rhs)
				   + i * MTYPE_byte_size(mtype),
				 ty_idx_used,
				 Make_Pointer_Type(ty_idx_used), n_rhs);
	  n_rhs = lower_expr(block, n_rhs, actions);
	  preg    = RETURN_INFO_preg (return_info, i);
          preg_st = MTYPE_is_float(mtype) ? Float_Preg : Int_Preg;
	  wn = WN_CreateStid(OPR_STID, MTYPE_V, mtype, preg, preg_st, 
			     Be_Type_Tbl(mtype), n_rhs);
	  WN_Set_Linenum(wn, current_srcpos);  // Bug 1268
	  WN_INSERT_BlockLast (block, wn);
	}
      }
      else { // MLOAD
	Is_True(WN_operator(WN_kid1(o_rhs)) == OPR_INTCONST,
		("expected RETURN_VAL's MLOAD kid to be of constant size"));
	algn = TY_align(TY_pointed(WN_load_addr_ty(o_rhs)));
	for (i = 0; i < RETURN_INFO_count(return_info); i++) {
          mtype = RETURN_INFO_mtype(return_info, i);
	  ty_idx_used = Be_Type_Tbl(mtype);
	  Set_TY_align(ty_idx_used, algn);
	  if (i == 0)
	    n_rhs = WN_kid0(o_rhs);
	  else n_rhs = WN_COPY_Tree(WN_kid0(o_rhs));
	  n_rhs = WN_CreateIload(OPR_ILOAD, mtype, mtype,
				 WN_load_offset(o_rhs)
				   + i * MTYPE_byte_size(mtype),
				 ty_idx_used,
				 Make_Pointer_Type(ty_idx_used), n_rhs);
	  n_rhs = lower_expr(block, n_rhs, actions);
	  preg    = RETURN_INFO_preg (return_info, i);
          preg_st = MTYPE_is_float(mtype) ? Float_Preg : Int_Preg;
	  wn = WN_CreateStid(OPR_STID, MTYPE_V, mtype, preg, preg_st, 
			     Be_Type_Tbl(mtype), n_rhs);
	  WN_Set_Linenum(wn, current_srcpos);  // Bug 1268
	  WN_INSERT_BlockLast (block, wn);
	}
      }

      WN_Delete (o_rhs);
    }
  }

  WN *wn_return = WN_CreateReturn ();
  WN_Set_Linenum(wn_return, current_srcpos);  // Bug 1268
  if ( Cur_PU_Feedback )
    Cur_PU_Feedback->FB_lower_return_val( tree, wn_return );
  #ifdef TARG_ST
  // CL: keep line numbers for debug
  WN_Set_Linenum(wn_return, WN_Get_Linenum(tree));
  //TB: Set the is_return_val_lowered flag to TRUE
  WN_is_return_val_lowered(wn_return) = TRUE;
#endif

  tree = wn_return;
  return tree;
}

#ifdef TARG_ST
// Inside an AFFIRM node, use non control-flow versions of conditional
// AND and OR operator.
static WN *
lower_affirm_expr (WN * wn)
{
  int i;
  for ( i = 0; i < WN_kid_count(wn); i++ )
    WN_kid(wn, i) = lower_affirm_expr ( WN_kid(wn, i) );
  
  if (WN_operator(wn) == OPR_CAND) {
    wn = WN_LAND( WN_kid0(wn), WN_kid1(wn) );
  }
  else if (WN_operator(wn) == OPR_CIOR) {
    wn = WN_LIOR( WN_kid0(wn), WN_kid1(wn) );
  }

  return wn;
}
#endif


/* ====================================================================
 *
 * WN *lower_stmt(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on statement <tree>,
 * returning lowered tree.  Usually a single statement is returned, but
 * in some cases, lowering (e.g. of complex stores) produces a BLOCK of
 * statements.
 *
 * ==================================================================== */
#ifdef TARG_ST
WN *lower_stmt(WN *block, WN *tree, LOWER_ACTIONS actions)
#else
static WN *lower_stmt(WN *block, WN *tree, LOWER_ACTIONS actions)
#endif
{
  CURRENT_STATE stmtState = pushCurrentState(tree, actions);

  Is_True(OPCODE_is_stmt(WN_opcode(tree)),
	  ("expected statement node, not %s", OPCODE_name(WN_opcode(tree))));

  switch(WN_operator(tree))
  {
  case OPR_CALL:
  case OPR_ICALL:
  case OPR_PICCALL:
    tree = lower_call(block, tree, actions);
    break;

  case OPR_INTRINSIC_CALL:
    tree = lower_intrinsic_call(block, tree, actions);
    break;
    
  case OPR_ISTORE:
  case OPR_STID:
  case OPR_MSTORE:
#ifdef TARG_ST
  case OPR_STBITS:
  case OPR_ISTBITS:
#endif
    tree = lower_store(block, tree, actions);
    break;

  case OPR_COMPGOTO:
    tree = lower_compgoto(block, tree, actions);
    break;

  case OPR_IO:
    if (Action(LOWER_IO_STATEMENT))
    {
      if (traceIO)
      {
          fputs(DBar, TFile);
          fprintf(TFile, "Io Lower: Before Lowering\n");
          fdump_tree(TFile, tree);
          fputs(DBar, TFile);
      }

      tree = lower_io_statement(tree, actions);

      if (traceIO)
      {
          fputs(DBar, TFile);
          fprintf(TFile, "Io Lower: After Lowering\n");
          fdump_tree(TFile, tree);
          fputs(DBar, TFile);
      }
    }
    break;
  case OPR_ALTENTRY:
    tree = lower_entry(tree, actions);
    break;

  case OPR_EVAL:
    tree = lower_eval(block, tree, actions);
    break;

  case OPR_LABEL:
   /*
    *  lower loop info for CG (it will process these opt>0
    */
    if (WN_label_loop_info(tree))
    {
      WN  *infoblock= WN_CreateBlock();
      WN  *info=      WN_label_loop_info(tree);

      info = lower_expr(infoblock, info, actions);
      WN_set_label_loop_info(tree, info);
    }
#ifdef KEY
    if (Action(LOWER_ENTRY_EXIT) && WN_Label_Is_Handler_Begin (tree))
      tree = lower_landing_pad_entry (tree);
#endif // KEY
    break;

  case OPR_EXC_SCOPE_BEGIN:
   /*
    *  We don't want to lower the kids, which are not used to generate code,
    * but only to provide information to the optimizer. 
    */
    break;

  case OPR_REGION_EXIT:
   /*
    *  convert REGION_EXIT to GOTO
    */
    if (Action(LOWER_REGION_EXITS)) 
      WN_set_opcode(tree, OPC_GOTO);
    break;

  case OPR_ASSERT:
    tree = lower_assert(block, tree, actions);
    break;

  case OPR_TRUEBR:
  case OPR_FALSEBR:
    tree = lower_branch(block, tree, actions);
    break;

  case OPR_RETURN_VAL:
    if (Action(LOWER_RETURN_VAL))
      tree = lower_return_val(block, tree, actions);
    else
      WN_kid0(tree) = lower_expr(block, WN_kid0(tree), actions);
    break;

  case OPR_ASM_STMT:
    {
      // Lower only the input expressions
      INT16 i;
      for (i = 2; i < WN_kid_count(tree); i++) {
	WN_kid0(WN_kid(tree,i)) = lower_expr(block, WN_kid0(WN_kid(tree,i)), actions);
      }
      break;
    }

#ifdef TARG_ST
    // [CG] Lower on return, used for instrumentation
  case OPR_RETURN:
    tree = lower_return(block, tree, actions);
    break;
  case OPR_PRAGMA:
    if (WN_pragma(tree) == WN_PRAGMA_PREAMBLE_END) {
      tree = lower_preamble_end(block, tree, actions);
    }
    break;

  case OPR_AFFIRM:
    // FdF 20080306: Do not generate control flow for AFFIRM expressions
    if (Action(LOWER_SHORTCIRCUIT))
      WN_kid0(tree) = lower_affirm_expr(WN_kid0(tree));
    WN_kid0(tree) = lower_expr(block, WN_kid0(tree), actions);
    break;
#endif

#ifdef TARG_ST
  case OPR_XGOTO:
    // We can come here from lower_hilo. kids have already been lowered from COMGOTO.
    if (Action(LOWER_HILO)) {
      break;
    }
    // else fall through.
#endif

#ifdef KEY
  case OPR_GOTO_OUTER_BLOCK:
    if (Action(LOWER_TO_CG)) { 
      WN *wn;
      LABEL_IDX label_idx = WN_label_number(tree);
 
      // restore SP before the GOTO_OUTER_BLOCK
      ST *spsave_sym = Find_SPSave_Symbol(LABEL_IDX_level(label_idx));
      wn = WN_Ldid(Pointer_type, 0, spsave_sym, ST_type(spsave_sym));
      wn = lower_expr(block, wn, actions); // expose the static link indirection
      wn = WN_StidIntoPreg(Pointer_type, Stack_Pointer_Preg_Offset,
			   MTYPE_To_PREG(Pointer_type), wn);
      WN_Set_Linenum (wn, current_srcpos);
      WN_INSERT_BlockLast(block, wn);

      // restore FP before the GOTO_OUTER_BLOCK
      ST *fpsave_sym = Find_FPSave_Symbol(LABEL_IDX_level(label_idx));
      wn = WN_Ldid(Pointer_type, 0, fpsave_sym, ST_type(fpsave_sym));
      wn = lower_expr(block, wn, actions); // expose the static link indirection
      wn = WN_StidIntoPreg(Pointer_type, Frame_Pointer_Preg_Offset,
			   MTYPE_To_PREG(Pointer_type), wn);
      WN_Set_Linenum (wn, current_srcpos);
      WN_INSERT_BlockLast(block, wn);
      }
#endif

  default: 
    {
      INT16 i;
      for (i = 0; i < WN_kid_count(tree); i++)
  	WN_kid(tree,i) = lower_expr(block, WN_kid(tree,i), actions);
    }
  }

  popCurrentState(stmtState);
  return tree;
}

/* ====================================================================
 *
 * WN *replace_ldidPreg(WN *expr, PREG_NUM reg, WN *replacement)
 * 
 *  walk expr looking for an ldid of preg reg.
 *  if found (expect only one of them), replace the ldid with replacement
 *
 *  if replacement is NULL we are doing a dryrun to test if there is a value
 *
 * ==================================================================== */

static WN *replace_ldidPreg(WN *expr, PREG_NUM reg, WN *replacement)
{
  INT16 i;

  if (expr==NULL)
    return NULL;

  for (i = 0; i < WN_kid_count(expr); i++)
  {
    WN	*child= WN_kid(expr, i);

    if ((WN_operator_is(child, OPR_LDID))	&&
	(WN_class(child) == CLASS_PREG)		&&
	(WN_load_offset(child) == reg
#ifdef KEY // need to make this work under -OPT:early_intrinsics=on
	 || WN_load_offset(child) == -1
#endif
	 ))
    {
      if (replacement)
      {
	WN_kid(expr, i) = replacement;
      }
      return child;
    }

    {
      WN  *tree;
      if (tree = replace_ldidPreg(child, reg, replacement))
	return tree;
    }
  }
  return NULL;
}

/* ====================================================================
 *
 * 
 * WN *lower_intrinsic_call(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * intrinsic calls have the special problem of defining a return
 * register that is used in the next stmt
 *
 * It is required that the emulation routine either
 *  [1]		passes the corresponding call node back
 *  	or
 *  [2]		returns a value (typically ldid preg)
 *		this value must replace the return register which
 *		should be in the next whirl statement (fchow says so)
 *
 * ==================================================================== */
static WN *lower_intrinsic_call(WN *block, WN *tree, LOWER_ACTIONS actions)
{ 
  WN		*next, *em, *newBlock;
  INTRINSIC	id = (INTRINSIC) WN_intrinsic(tree);
  SRCPOS srcpos = WN_Get_Linenum(tree);
  TYPE_ID	type;
  PREG_NUM	reg1, reg2;
  BOOL intrinsic_lowered;

  Is_True(OPCODE_is_stmt(WN_opcode(tree)),
	  ("expected statement node, not %s", OPCODE_name(WN_opcode(tree))));
  {
   /*
    *  first lower the children
    */
    INT16 i;
    for (i = 0; i < WN_kid_count(tree); i++)
      WN_kid(tree,i) = lower_expr(block, WN_kid(tree,i), actions);
  }

  if (INTRN_cg_intrinsic(id))
    return tree;

  if (NotAction(LOWER_INTRINSIC |
		LOWER_INLINE_INTRINSIC |
		LOWER_INL_STACK_INTRINSIC)) {
    return tree;
  }
  else if (NotAction(LOWER_INL_STACK_INTRINSIC)) {
    INTRINSIC intr = (INTRINSIC) WN_intrinsic(tree);
    if (intr == INTRN_U4I4SETSTACKPOINTER ||
	intr == INTRN_U8I8SETSTACKPOINTER ||
	intr == INTRN_U4READSTACKPOINTER ||
	intr == INTRN_U8READSTACKPOINTER ||
#if defined( KEY) && !defined(TARG_ST)
	intr == INTRN_U4READFRAMEPOINTER ||
	intr == INTRN_U8READFRAMEPOINTER ||
#endif
	intr == INTRN_U4I4ALLOCA ||
	intr == INTRN_U8I8ALLOCA) {
      return tree;
    }
  }

  next = WN_next(tree);
  type = WN_rtype(tree);
  newBlock = WN_CreateBlock();

 /*
  *  see if statement return value exists in the next statement
  *  but don't replace it
  */
  {
    TYPE_ID	ty1, ty2;
    BOOL	returnValueUnused = FALSE;

    if (WHIRL_Return_Info_On) {

      RETURN_INFO return_info = Get_Return_Info (MTYPE_To_TY(type),
                                                 Complex_Not_Simulated
#ifdef TARG_X8664
					         , FALSE
#endif
						);

      if (RETURN_INFO_count(return_info) <= 2) {

        ty1  = RETURN_INFO_mtype (return_info, 0);
        ty2  = RETURN_INFO_mtype (return_info, 1);
        reg1 = RETURN_INFO_preg (return_info, 0);
        reg2 = RETURN_INFO_preg (return_info, 1);
      }

      else
        ErrMsg (EC_Unimplemented,
                "lower_intrinsic_call: more than 2 return registers");
    }

    else {
#ifdef TARG_ST
      FmtAssert(FALSE,("Get_Return_Mtypes/Pregs shouldn't be called"));
#else
      Get_Return_Mtypes(MTYPE_To_TY(type), Complex_Not_Simulated, &ty1, &ty2);
      Get_Return_Pregs(ty1, ty2, &reg1, &reg2);
#endif
    }

    if (MTYPE_is_void(type))
    {
      returnValueUnused = TRUE;
    }
    if (replace_ldidPreg(next, reg1, NULL)==NULL)
    {
      returnValueUnused = TRUE;
    }
    if (returnValueUnused && INTRN_has_no_side_effects(id))
    {
      DevWarn("lower_intrinsic_call(): function %s is void or unused and has"
	      " no_side_effects. It will be removed", get_intrinsic_name(id));

      return newBlock;
    }
  }

  em = lower_emulation(newBlock, tree, actions, intrinsic_lowered);

 /*
  *  Nothing happened (ie. the intrinsic was not an INLINE
  *  and we should go no further
  */
  if ((Action(LOWER_INLINE_INTRINSIC) ||
       Action(LOWER_INL_STACK_INTRINSIC)) && 
      NotAction(LOWER_INTRINSIC)	  &&
      tree == em)
  {
    WN_Delete(newBlock);
    return tree;
  }

  WN_Delete(tree);

  if (OPCODE_is_call(WN_opcode(em)))
  {
    WN_Set_Linenum (em, srcpos);
    WN_INSERT_BlockLast(block, newBlock);
    return em;
  }
  else if (WN_is_block(em))
  {
    WN_INSERT_BlockLast(block, newBlock);
    return em;
  }
  else if (MTYPE_is_void(type))
  {
    if (OPCODE_is_stmt(WN_opcode(em)))
    {
      WN_INSERT_BlockLast(newBlock, em);
    }
    return newBlock;
  }
  else
  {
    /*
     *	so far we only expand __builtin_alloca() and the memset routines.
     *  This code will need to change based on different return sequences
     */
    Is_True((reg1 != 0), ("expected return value from intrinsic_call"));
    Is_True((reg2 == 0), ("cannot evaluate 2 regs into an expression"));
    Is_True((OPCODE_is_expression(WN_opcode(em))), ("expected expression"));
    {
      WN	*replaced = NULL;

      /*
       *  In the next statement replace an ldid of preg reg1 with em
       *  The use of the function may have been deleted, but the function
       *  still may have side effects!
       *
       *  We are guaranteed the result is in the "next" statement BTW
       */
      replaced = replace_ldidPreg(next, reg1, em);

      if (replaced)
      { /* CVTL-RELATED */
        Is_True(Types_are_compatible(WN_rtype(replaced),type),
		("return reg mismatch type"));
      }
      return newBlock;
    }
  }
}

#ifdef TARG_ST
/*
 * Forward declarations of post lowering functions.
 */
static BOOL post_lower_required(WN *func_wn, LOWER_ACTIONS actions);
static void post_lower_entry(WN *tree, LOWER_ACTIONS);
static void post_lower_block(WN *block, LOWER_ACTIONS actions);
static void post_lower_scf(WN *block, WN *tree, LOWER_ACTIONS actions);
static void post_lower_region(WN *tree, LOWER_ACTIONS actions);
static void post_lower_if(WN *block, WN *tree, LOWER_ACTIONS actions);
static void post_lower_do_loop(WN *block, WN *tree, LOWER_ACTIONS actions);
static void post_lower_while_do(WN *block, WN *tree, LOWER_ACTIONS actions);
static void post_lower_do_while(WN *block, WN *tree, LOWER_ACTIONS actions);
static void post_lower_expr(WN *block, WN *tree, LOWER_ACTIONS actions);
static void post_lower_stmt(WN *block, WN *tree, LOWER_ACTIONS actions);
static void post_lower_call(WN *block, WN *tree, LOWER_ACTIONS actions);
static void post_lower_call(WN *block, WN *tree, LOWER_ACTIONS actions);

/*
 * post_lower_call()
 *
 * The cases where the lowering should be done there and not in lower_call()
 * are when:
 * - the instructions following the call must be inspected.
 * - the lowering consists mainly in inserting new statements around the call.
 *
 * Currently the post lowering performed here are:
 * - instrumentation of call exits.
 */
static void
post_lower_call(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  
  /* ========== Instrumentation of call exits. */
  if (Action(LOWER_INSTRUMENT) && lower_profile_call_enabled(tree)) {
    WN *point = WN_next(tree);
    WN *insert_point = tree;
    while(point != NULL) {
      if (!(OPERATOR_is_store(WN_operator(point)) &&
	    !stmt_is_store_of_return_value(point))) break;
      if (stmt_is_store_of_callee_return_value(point))
	insert_point = point;
      point = WN_next(point);
    }
    WN *new_block = WN_CreateBlock();
    WN *profcall = lower_profile_call(new_block, tree, 1, actions);
    DevAssert(profcall != NULL, ("post_lower_call(): unexpected NULL tree after lower_profile_call()"));
    WN_INSERT_BlockLast(new_block, profcall);
    WN_INSERT_BlockAfter(block, insert_point, new_block);
  }
}

/*
 * post_lower_stmt()
 *
 * Performs the post lowering of a statement, called by post_lower_block().
 * This post lowering will only be done if the statement is lowered 
 * as part of a block with lower_block().
 *
 * Currently, only the following statements are processed:
 * - call statements.
 */
static void
post_lower_stmt(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  CURRENT_STATE stmtState = pushCurrentState(tree, actions);
  
  Is_True(OPCODE_is_stmt(WN_opcode(tree)),
	  ("expected statement node, not %s", OPCODE_name(WN_opcode(tree))));

  switch(WN_operator(tree)) {
  case OPR_CALL:
  case OPR_ICALL:
  case OPR_PICCALL:
    post_lower_call(block, tree, actions);
    break;
  case OPR_ALTENTRY:
    post_lower_entry(tree, actions);
    break;
  default:
    break;
  }
  popCurrentState(stmtState);
}


/*
 * post_lower_expr()
 */
static void
post_lower_expr(WN *block, WN *tree, LOWER_ACTIONS actions)
{
}

/*
 * post_lower_do_while()
 */
static void
post_lower_do_while(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  post_lower_block(WN_while_body(tree), actions);
  post_lower_expr(NULL, WN_while_test(tree), actions);
}

/*
 * post_lower_while_do()
 */
static void
post_lower_while_do(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  post_lower_expr(NULL, WN_while_test(tree), actions);
  post_lower_block(WN_while_body(tree), actions);
}

/*
 * post_lower_do_loop()
 */
static void
post_lower_do_loop(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  post_lower_stmt(NULL, WN_start(tree), actions);
  post_lower_expr(NULL, WN_end(tree), actions);
  post_lower_stmt(NULL, WN_step(tree), actions);
  post_lower_block(WN_do_body(tree), actions);
}

/*
 * post_lower_if()
 */
static void
post_lower_if(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  post_lower_expr(NULL, WN_if_test(tree), actions);
  post_lower_block(WN_then(tree), actions);
  post_lower_block(WN_else(tree), actions);
}

/*
 * post_lower_region()
 */
static void
post_lower_region(WN *tree, LOWER_ACTIONS actions)
{
  post_lower_block(WN_region_body(tree), actions);
}

/*
 * post_lower_scf()
 */
static void
post_lower_scf(WN *block, WN *tree, LOWER_ACTIONS actions)
{

  CURRENT_STATE scfState = pushCurrentState(tree, actions);
  
  switch (WN_opcode(tree)) {
  case OPC_DO_WHILE:
    post_lower_do_while(block, tree, actions);
    break;
    
  case OPC_WHILE_DO:
    post_lower_while_do(block, tree, actions);
    break;

  case OPC_DO_LOOP:
    post_lower_do_loop(block, tree, actions);
    break;
      
  case OPC_IF:
    post_lower_if(block, tree, actions);
    break;

  case OPC_BLOCK:
    post_lower_block(tree, actions);
    break;
    
  case OPC_REGION:
    post_lower_region(tree, actions);
    break;
  }
  popCurrentState(scfState);
}

/*
 * post_lower_block()
 *
 * This function is called from lower_block() once the block has been fully
 * lowered.
 *
 * It is used to perform some additional lowering that requires a full
 * block context.
 */
static void
post_lower_block(WN *block, LOWER_ACTIONS actions)
{
  WN *node;
  CURRENT_STATE blockState;

  blockState = pushCurrentState(block, actions);

  for (node = WN_first(block); node; node = WN_next(node)) {
    setCurrentState(node, actions);
    if (OPCODE_is_stmt(WN_opcode(node))) {
      post_lower_stmt(block, node, actions);
    } else if (OPCODE_is_scf(WN_opcode(node))) {
      post_lower_scf(block, node, actions);
    } else {
      Fail_FmtAssertion("expected statement or SCF node, not %s",
			OPCODE_name(WN_opcode(block)));
      /*NOTREACHED*/
    }
  }
  
  popCurrentState(blockState);
}


/*
 * post_lower_required()
 *
 * Must return TRUE is the function requires a post lowering phase,
 *
 * Currently the only context where we want post lowering is when
 * LOWER_INSTRUMENT and Gen_Profile are enabled.
 */
static BOOL
post_lower_required(WN *func_wn, LOWER_ACTIONS actions)
{
  if (Action(LOWER_INSTRUMENT) && Gen_Profile)
    return TRUE;
  return FALSE;
}

/*
 * post_lower_entry()
 *
 * Post lowering phase called from WN_Lower().
 */
static void
post_lower_entry(WN *tree, LOWER_ACTIONS actions)
{
  Is_True(((WN_opcode(tree) == OPC_FUNC_ENTRY)
	   || (WN_opcode(tree) == OPC_ALTENTRY)),
          ("expected ENTRY/ALTENTRY node, not %s",
	   OPCODE_name(WN_opcode(tree))));

  if (WN_opcode(tree) == OPC_FUNC_ENTRY) {
    post_lower_block(WN_func_body(tree), actions);
  }
}

#endif


/* ====================================================================
 *
 * WN *lower_block(WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on statements in BLOCK
 * node <tree>, returning (possibly) modified BLOCK.
 *
 * ==================================================================== */

WN *lower_block(WN *tree, LOWER_ACTIONS actions)
{
  WN *out, *node, *next_node;
  CURRENT_STATE blockState;

  Is_True(WN_opcode(tree) == OPC_BLOCK,
	  ("expected BLOCK node, not %s", OPCODE_name(WN_opcode(tree))));

  blockState = pushCurrentState(tree, actions);

  out = WN_CreateBlock();
  WN_Set_Linenum (out, current_srcpos);

  for (node = WN_first(tree); node; node = next_node)
  {

    /*	MP pragmas are handled in a very special manner.  There are three forms
	of parallel constructs - standalone parallel directive, parallel loop
	(doacross) and parallel region.
	A standalone parallel directive starts with an MP pragma node and is
	followed by (depending on the directive) a series of statements ended
	with another, specific MP pragma node.
	Parallel loops start with an MP pragma node followed by a series of MP
	pragmas and assignment statements ended with a DO_LOOP node.
	A parallel region is an OPC_REGION created by the FE which encapsulates
	all statements within the region.  It will always have a func_pragma
	list containing MP pragmas.  To all but the MP lowerer, this region is
	transparent.
	If MP lowering is in effect, then this entire set of nodes needs to be
	removed from the statement chain, translated and replaced with one or
	more statement/scf nodes.  Because a set of nodes must be processed
	as a unit, the usual lowering scheme of passing in a single node (tree)
	and returning a single node (tree) won't work.  */

    while (Action(LOWER_MP) && node &&
      ((((WN_opcode(node) == OPC_PRAGMA) || (WN_opcode(node) == OPC_XPRAGMA))
	&& (WN_pragmas[WN_pragma(node)].users & PUSER_MP)) ||
       ((WN_opcode(node) == OPC_REGION) && WN_first(WN_region_pragmas(node)) &&
        ((WN_opcode(WN_first(WN_region_pragmas(node))) == OPC_PRAGMA) ||
	 (WN_opcode(WN_first(WN_region_pragmas(node))) == OPC_XPRAGMA)) &&
        (WN_pragmas[WN_pragma(WN_first(WN_region_pragmas(node)))].users &
	 PUSER_MP))))
	node = lower_mp(out, node, actions);
    if (node == NULL) break;

    /*
     *  Must read WN_next(node) now since it may be overwritten
     */
    next_node = WN_next(node);
    setCurrentState(node, actions);

    if (OPCODE_is_stmt(WN_opcode(node)))
    {
	node = lower_stmt(out, node, actions);
    }
    else if (OPCODE_is_scf(WN_opcode(node)))
    {
	node = lower_scf(out, node, actions);
    }
    else
    {
	Fail_FmtAssertion("expected statement or SCF node, not %s",
			  OPCODE_name(WN_opcode(tree)));
	/*NOTREACHED*/
    }

    if (node == NULL) // lowering an MSTID of Return_Val_Preg can cause NULL
      continue;

#ifdef KEY
    if (Action(LOWER_MADD) &&
	Madd_Allowed &&
	Roundoff_Level >= ROUNDOFF_ASSOC) {
      lower_madd_tree_height(out, node, actions);
    }
#endif

    WN_INSERT_BlockLast(out, node);
  
    // if STID(ALLOCA) and trapuv, insert a mstore
    if ( Action(LOWER_INL_STACK_INTRINSIC)
	&& (DEBUG_Trap_Uv
#ifdef KEY
	    || DEBUG_Zero_Uv
#endif
	   )
        && WN_operator(node) == OPR_STID
        && WN_operator(WN_kid0(node)) == OPR_ALLOCA)
    {
#ifdef KEY
        lower_trapuv_alloca (out, WN_kid0(node), actions,
			     WN_st_idx(node), WN_offset(node), WN_ty(node));
#else
        lower_trapuv_alloca (out, WN_kid0(node), actions);
#endif
    }
  }

 /*
  *  Once we have the integrated block, remove any stmt after
  *  a return or goto, until we hit a label or ALTENTRY
  *  only if we are not called by lower_scf_non_recursive()
  *
  *  Curiously enough, this is needed to prevent dead blocks (-O0)
  *  which causes exception handling to fail ...
  *  
  */
  if (NotAction(LOWER_TOP_LEVEL_ONLY))
  {
    for (node = WN_first(out); node; node = next_node)
    {
      next_node = WN_next(node);
  
      if (WN_unconditional_goto(node))
      {
        for(node = next_node; node; node = next_node)
        {
	  next_node = WN_next(node);
  
	  if (WN_operator_is(node, OPR_LABEL))
	    break;
	  else if (WN_operator_is(node, OPR_ALTENTRY))
	    break;
	  else if (WN_operator_is(node, OPR_EXC_SCOPE_BEGIN))
	    continue;
	  else if (WN_operator_is(node, OPR_EXC_SCOPE_END))
	    continue;
	  else if (WN_operator_is(node, OPR_PRAGMA))
	    continue;
	  else if (WN_operator_is(node, OPR_XPRAGMA))
	    continue;
	  else if (WN_operator_is(node, OPR_REGION_EXIT))
	    continue;
	  if (OPCODE_is_stmt(WN_opcode(node)))
	  {
	    WN_DELETE_FromBlock(out, node);
	  }
	  else
	    break;
        }
      }
    }
  }

  WN_Delete(tree);

  popCurrentState(blockState);
  return out;
}


/* ====================================================================
 *
 * WN *lower_speculate(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * try to speculate CAND/CIOR to LAND/LIOR
 * Edge frequency data is discarded
 *
 * ==================================================================== */

static WN *lower_speculate(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  INT32	n;
  BOOL  speculate = TRUE;

  if (OPT_Lower_Speculate == FALSE)
    return tree;

  switch(WN_operator(tree))
  {
  case OPR_CAND:
  case OPR_CIOR:
  case OPR_CSELECT:
    for (n = 0; n < WN_kid_count(tree); n++)
    {
      WN_kid(tree, n) =	lower_speculate(block, WN_kid(tree, n), actions);
      speculate &=	expr_is_speculative(WN_kid(tree, n));
    }

    if (speculate)
    {
      if (WN_operator_is(tree, OPR_CAND))
      {
	WN *land = WN_LAND( WN_kid0(tree), WN_kid1(tree) );
	WN_Delete(tree);
	return land;
      }
      else if (WN_operator_is(tree, OPR_CIOR))
      {
	WN *lior = WN_LIOR( WN_kid0(tree), WN_kid1(tree) );
	WN_Delete(tree);
	return lior;
      }
      else if (WN_operator_is(tree, OPR_CSELECT))
      {
	WN *select = WN_Select(WN_rtype(tree), WN_kid0(tree), WN_kid1(tree),
			       WN_kid2(tree));
  
	WN_Delete(tree);
	return select;
      }
    }
    break;
  }

  return tree;
}

/* ====================================================================
 *
 * WN *lower_conditional(WN *block, WN *tree, BOOL branchType,
 *                       LOWER_ACTIONS actions)
 *
 * lower CAND/CIOR to scf
 *
 * ==================================================================== */

static WN *
lower_conditional(WN *block, WN *tree, LABEL_IDX trueLabel,
		  LABEL_IDX falseLabel, BOOL branchType,
		  LOWER_ACTIONS actions)
{
  WN *shortcircuit, *left_branch = NULL, *right_branch = NULL;

  switch(WN_operator(tree))
  {
  case OPR_CAND:
   /*
    *	Process the left child.
    *   We need a label for the children to branch to
    */
    if (WN_operator_is(WN_kid0(tree), OPR_CAND)	||
	WN_operator_is(WN_kid0(tree), OPR_CIOR))
    {
      shortcircuit = WN_NewLabel();

      lower_conditional(block, WN_kid0(tree), WN_label_number(shortcircuit),
			falseLabel, FALSE, actions);
      WN_INSERT_BlockLast(block, shortcircuit);
    }
    else
    {
      left_branch = WN_Falsebr(falseLabel, WN_kid0(tree));
      WN_Set_Linenum(left_branch, current_srcpos);  // Bug 1268
      WN_INSERT_BlockLast(block, left_branch);
    }

   /*
    *	Process the right child.
    */
    if (WN_operator_is(WN_kid1(tree), OPR_CAND) ||
	WN_operator_is(WN_kid1(tree), OPR_CIOR))
    {
      lower_conditional(block, WN_kid1(tree), trueLabel, falseLabel,
			branchType, actions);
    }
    else {
      if ( branchType /*==TRUE*/ ) {
	right_branch = WN_Truebr(trueLabel, WN_kid1(tree));
      }
      else {
	/* branchType == FALSE */
	right_branch = WN_Falsebr(falseLabel, WN_kid1(tree));
      }
      WN_Set_Linenum(right_branch, current_srcpos);  // Bug 1268
      WN_INSERT_BlockLast(block, right_branch);
    }
    if (Cur_PU_Feedback) {
      Cur_PU_Feedback->FB_lower_circuit( tree, left_branch, right_branch );
    }
    return NULL;

  case OPR_CIOR:
   /*
    *	Process the left child.
    *   We need a label for the children to branch to
    */
    if (WN_operator_is(WN_kid0(tree), OPR_CAND)	||
	WN_operator_is(WN_kid0(tree), OPR_CIOR))
    {
      shortcircuit = WN_NewLabel();

      lower_conditional(block, WN_kid0(tree), trueLabel,
			WN_label_number(shortcircuit), TRUE, actions);
      WN_INSERT_BlockLast(block, shortcircuit);
    }
    else
    {
      left_branch = WN_Truebr(trueLabel, WN_kid0(tree));
      WN_Set_Linenum(left_branch, current_srcpos);  // Bug 1268
      WN_INSERT_BlockLast(block, left_branch);
    }

   /*
    *	Process the right child.
    */
    if (WN_operator_is(WN_kid1(tree), OPR_CAND) ||
	WN_operator_is(WN_kid1(tree), OPR_CIOR))
    {
      lower_conditional(block, WN_kid1(tree), trueLabel, falseLabel,
			branchType, actions);
    }
    else {
      if ( branchType /*==TRUE*/ ) {
	right_branch = WN_Truebr(trueLabel, WN_kid1(tree));
      }
      else {
	/* branchType == FALSE */
	right_branch = WN_Falsebr(falseLabel, WN_kid1(tree));
      }
      WN_Set_Linenum(right_branch, current_srcpos);  // Bug 1268
      WN_INSERT_BlockLast(block, right_branch);
    }
    if (Cur_PU_Feedback) {
      Cur_PU_Feedback->FB_lower_circuit( tree, left_branch, right_branch );
    }
    return NULL;

  default:
    tree = lower_expr(block, tree, actions);
    break;
  }

  return tree;
}

static WN *lower_branch_condition(BOOL branchType, LABEL_IDX label, WN *cond,
				  WN **branch, LOWER_ACTIONS actions)
{
  WN *condBlock = WN_CreateBlock();

  cond = lower_speculate(condBlock, cond, actions);

  *branch = NULL;

  switch(WN_operator(cond))
  {
  case OPR_CAND:
  case OPR_CIOR:
    {
      LABEL_IDX shortcircuit_lbl;
      shortcircuit_lbl = NewLabel();

      if (branchType == TRUE)
      {
	cond = lower_conditional(condBlock, cond, label, shortcircuit_lbl,
				 branchType, actions);
      }
      else
      {
	cond = lower_conditional(condBlock, cond, shortcircuit_lbl, label,
				 branchType, actions);
      }

      condBlock = lower_block(condBlock, actions);
      WN_INSERT_BlockLast(condBlock, WN_Label(shortcircuit_lbl));
    }
    break;
  default:
    {
      cond = lower_expr(condBlock, cond, actions);

      if (branchType)
	*branch = WN_Truebr(label, cond);
      else
	*branch = WN_Falsebr(label, cond);
      WN_Set_Linenum(*branch, current_srcpos);  // Bug 1268
      WN_INSERT_BlockLast(condBlock, *branch);
    }
  }
  return condBlock;
}

#ifdef SHORTCIRCUIT_HACK
/* return TRUE if the expression <tree> has a CAND/CIOR that cannot
 * be converted into LAND/LIOR respectively.
 */
static BOOL tree_has_cand_cior (WN *tree)
{
  WN_ITER *wni;
  WN *wn;

  for (wni = WN_WALK_TreeIter (tree); 
       wni != NULL;
       wni = WN_WALK_TreeNext (wni))
  {
    wn = WN_ITER_wn (wni);
    /* TODO: check if the CAND/CIOR can be converted to LAND/LIOR */
    if (WN_operator_is(wn, OPR_CAND)	||
	WN_operator_is(wn, OPR_CIOR)	||
	WN_operator_is(wn, OPR_CSELECT))
      return TRUE;
  }
  return FALSE;
}
#endif

/* ====================================================================
 *
 * WN *lower_if(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on statements in IF
 * node <tree>, returning lowered statements.  Returned tree will always
 * have a structured control flow node (at least a BLOCK) at the top.
 *
 * ==================================================================== */

static WN *lower_if(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  Is_True(WN_opcode(tree) == OPC_IF,
	  ("expected IF node, not %s", OPCODE_name(WN_opcode(tree))));

  if (WN_Is_If_MpVersion(tree))
    return lower_block(lower_mp(block, tree, actions), actions);

#ifndef SHORTCIRCUIT_HACK
  if (Action(LOWER_IF))
#else
  if (Action(LOWER_IF) || 
      (Action(LOWER_SHORTCIRCUIT) && tree_has_cand_cior(WN_if_test(tree))))
#endif
  {
   /*
    *  Lower IF.  This is done differently depending on
    *  whether the "then" or "else" clauses are empty.
    * 
    *  Pay close attention to the block scope and state while creating nodes
    */
    WN *body = WN_CreateBlock();
    WN *wn_branch = NULL;
    LABEL_IDX cont_lbl;
    cont_lbl = NewLabel();

    if (Action(LOWER_TOP_LEVEL_ONLY))
    {
      actions = RemoveScfAction(actions);
    }

    if (WN_block_nonempty(WN_then(tree)))
    {
      if (WN_block_nonempty(WN_else(tree)))
      {
       /*
        * Both "then" and "else" clauses exist.  Generate:
        * 	(FALSEBR <cond> <else_lbl>)
        *	<then_clause>
        *	(GOTO <cont_lbl>)
        *	(LABEL <else_lbl>)
        *	<else_clause>
        *	(LABEL <cont_lbl>)
        */
	LABEL_IDX else_lbl;
	else_lbl = NewLabel();

	WN *falsebr_block = lower_falsebr(else_lbl, WN_if_test(tree),
					  &wn_branch, actions);
	WN_INSERT_BlockFirst(body, falsebr_block);

	setCurrentStateBlockLast(WN_then(tree), actions);
	WN_INSERT_BlockLast(body, lower_block(WN_then(tree), actions));
	WN *wn_goto = WN_Goto(cont_lbl);
	WN_Set_Linenum(wn_goto, current_srcpos);  // Bug 1268
	WN_INSERT_BlockLast(body, wn_goto);

	WN *wn_else_lbl = WN_Label(else_lbl);
	setCurrentStateBlockFirst(WN_else(tree), actions);
	WN_Set_Linenum(wn_else_lbl, current_srcpos);  // Bug 1268
	WN_INSERT_BlockLast(body, wn_else_lbl);

	setCurrentStateBlockLast(WN_else(tree), actions);
	WN_INSERT_BlockLast(body, lower_block(WN_else(tree), actions));
	WN *wn_cont_lbl = WN_Label(cont_lbl);
	WN_Set_Linenum(wn_cont_lbl, current_srcpos);  // Bug 1268
	WN_INSERT_BlockLast(body, wn_cont_lbl);
      } 
      else
      {
       /*
        * Only "then" clause exists.  Generate:
        *	(FALSEBR <cond> <cont_lbl>)
        *	<then_clause>
        *	(LABEL <cont_lbl>)
        */
	WN *falsebr_block = lower_falsebr(cont_lbl, WN_if_test(tree),
					  &wn_branch, actions);
	WN_INSERT_BlockFirst(body, falsebr_block);

	setCurrentStateBlockLast(WN_then(tree), actions);
	WN_INSERT_BlockLast(body, lower_block(WN_then(tree), actions));
	WN *wn_cont_lbl = WN_Label(cont_lbl);
	WN_Set_Linenum(wn_cont_lbl, current_srcpos);  // Bug 1268
	WN_INSERT_BlockLast(body, wn_cont_lbl);
      }

      if (Cur_PU_Feedback) {
	Cur_PU_Feedback->FB_lower_branch( tree, wn_branch );
      }

      WN_Delete(tree);
      return body;
    } 
    else if (WN_block_nonempty(WN_else(tree)))
    {
     /*
      * Only "else" clause exists.  Generate:
      *		(TRUEBR <cond> <cont_lbl>)
      *		<else_clause>
      *		(LABEL <cont_lbl>)
      */
      WN *truebr_block = lower_truebr(cont_lbl, WN_if_test(tree),
				      &wn_branch, actions);
      WN_INSERT_BlockFirst(body, truebr_block);

      setCurrentStateBlockLast(WN_else(tree), actions);
      WN_INSERT_BlockLast(body, lower_block(WN_else(tree), actions));
      WN *wn_cont_lbl = WN_Label(cont_lbl);
      WN_Set_Linenum(wn_cont_lbl, current_srcpos);  // Bug 1268
      WN_INSERT_BlockLast(body, wn_cont_lbl);

      if (Cur_PU_Feedback) {
	Cur_PU_Feedback->FB_lower_branch( tree, wn_branch );
      }

      WN_Delete(tree);
      return body;
    }
    else
    {
     /*
      * Neither "then" or "else" clause exists.  Generate:
      *		(EVAL <cond>)
      */
      WN *eval;

      eval = WN_CreateExp1(OPC_EVAL, lower_expr(block, WN_if_test(tree),
						actions));
      WN_Set_Linenum (eval, current_srcpos);

      if (Cur_PU_Feedback) {
	Cur_PU_Feedback->FB_lower_branch( tree, NULL );
      }

      WN_DELETE_Tree(WN_then(tree));
      WN_DELETE_Tree(WN_else(tree));
      WN_Delete(tree);
      return eval;
    }
  }
  else if (NotAction(LOWER_TOP_LEVEL_ONLY))
  {
    WN_if_test(tree) = lower_expr(block, WN_if_test(tree),
				  RemoveShortCircuitAction(actions));
    WN_then(tree) = lower_block(WN_then(tree), actions);
    WN_else(tree) = lower_block(WN_else(tree), actions);
  }

  return tree;
}

#ifdef KEY
static void Substitute_Constant_Multiplicands ( WN* tree, WN* copy, 
						WN** condition,
						FB_FREQ* freq, 
						BOOL* optimized, 
						BOOL* okay )
 /* 
    'tree' is an input tree and 'copy' its copy. 
 
    '*condition' holds a list of conditions in the form 'cond1 && cond2'
    where each 'cond' is an equality of the form 'id == constant' or 
    'cond1 && cond2' itself.
    
    'freq' will hold the frequency (execution counter) for which 
    '*condition' holds true.
    
    If the 'copy' is modified then '*optimized' is set to indicate the caller.

    If there are unsuitable operators (notably a CALL) then '*okay' is set
    to FALSE to indicate it is not okay to constant substitute multiplicands.
    
    This recursive function does the following:
    (a) For any MPY inside tree, for all of the operands that is a load, check
        whether there is a high probability of 1.0 or 0.0. If there is a high
	chance then constant substitute the operand with the 1.0 or 0.0,
	attach a condition to '*condition', update the frequency information.
	The frequency information is a guess. You can find out why.
    (b) For any CALL, mark '*okay' that it is not okay to do this transformation
        and return. 
    (c) For LABELs, this function also creates a new label so that the caller
        does not have to worry about renaming blocks inside the new block.
    (d) For all kinds of STOREs, process only the kids.
    (e) For BLOCKs and DO_LOOPs consider appropriate kid statements.
    
  */
{
  OPERATOR opr = WN_operator( tree );

  if (opr == OPR_DO_LOOP)
    Substitute_Constant_Multiplicands( WN_do_body( tree ), WN_do_body( copy ),
				       condition, freq, optimized, okay );

  else if (opr == OPR_BLOCK) {
    WN *stmt, *stmt_copy;

    stmt = WN_first( tree );
    stmt_copy = WN_first( copy );
    for (; stmt;) {
      WN *next_stmt, *next_stmt_copy;
      next_stmt = WN_next(stmt);
      next_stmt_copy = WN_next( stmt_copy );
      Substitute_Constant_Multiplicands( stmt, stmt_copy, condition, freq, 
					 optimized, okay );
      if ( !*okay )
	return;
      stmt = next_stmt;
      stmt_copy = next_stmt_copy;
    }
  } 

  else if ( opr == OPR_CALL ) {
    *okay = FALSE;
    return;
  }

  else if ( OPCODE_is_store( WN_opcode( tree ) ) )
    Substitute_Constant_Multiplicands( WN_kid0(tree), WN_kid0(copy), 
				       condition, freq, optimized, okay );

  else if ( opr == OPR_MPY &&
	    ( OPERATOR_is_load( WN_operator( WN_kid0(tree) ) ) ||
	      OPERATOR_is_load( WN_operator( WN_kid1(tree) ) ) )  &&
	    WN_rtype( tree ) == MTYPE_F8 ) {

    const FB_Info_Value_FP_Bin& info = 
      Cur_PU_Feedback->Query_value_fp_bin( tree );
    const float cutoff_ratio = (float)Mpy_Exe_Ratio / 100;

    if( info.exe_counter < (float)Mpy_Exe_Counter )
      return;

    if ( OPERATOR_is_load( WN_operator( WN_kid(tree,0) ) ) ) {
      BOOL substitute = FALSE;
      FB_FREQ this_freq = info.zopnd0;
      WN* id = WN_COPY_Tree(WN_kid(tree, 0));
      double val = 0.0;
      WN* value;
      
      if ( info.uopnd0 / info.exe_counter >= cutoff_ratio ||
	   info.zopnd0 / info.exe_counter >= cutoff_ratio )
	substitute = TRUE;
	
      if ( info.uopnd0 > info.zopnd0 ) {
	this_freq = info.uopnd0;
	val = 1.0;
      }

      if ( substitute ) {
	WN_Delete( WN_kid(copy,0) );
	WN_kid(copy, 0) = Make_Const( Host_To_Targ_Float (MTYPE_F8, val) );
	
	// Update frequency.
	if ( *freq < this_freq )
	  *freq = this_freq;

	// Update condition list.	
	value = Make_Const(Host_To_Targ_Float(MTYPE_F8,val));
	if ( *condition )
	  *condition = WN_CAND( *condition, WN_EQ( MTYPE_F8, id, value ) );
	else 
	  *condition = WN_EQ( MTYPE_F8, id, value );

	*optimized = TRUE;
      }
    } else
      Substitute_Constant_Multiplicands( WN_kid0(tree), WN_kid0(copy), 
					 condition, freq, optimized, okay );
    if ( !*okay )
      return;

    if ( OPERATOR_is_load( WN_operator( WN_kid(tree,1) ) ) ) {
      BOOL substitute = FALSE;
      FB_FREQ this_freq = info.zopnd1;
      WN* id = WN_COPY_Tree(WN_kid(tree, 1));
      double val = 0.0;
      WN* value;

      if ( info.uopnd1 / info.exe_counter >= cutoff_ratio ||
	   info.zopnd1 / info.exe_counter >= cutoff_ratio )
	substitute = TRUE;

      if ( info.uopnd1 > info.zopnd1 ) {
	this_freq = info.uopnd1;
	val = 1.0;
      }

      if ( substitute ) {
	
	// Update WN.
	WN_Delete( WN_kid(copy,1) );
	WN_kid(copy, 1) = Make_Const( Host_To_Targ_Float (MTYPE_F8, val) );
	
	// Update frequency.
	if ( *freq < this_freq )
	  *freq = this_freq;

	// Update condition list.	
	value = Make_Const(Host_To_Targ_Float(MTYPE_F8,val));
	if ( *condition )
	  *condition = WN_CAND( *condition, WN_EQ( MTYPE_F8, id, value ) );
	else 
	  *condition = WN_EQ( MTYPE_F8, id, value );

	*optimized = TRUE;
      }
    } else
      Substitute_Constant_Multiplicands( WN_kid1(tree), WN_kid1(copy), 
					 condition, freq, optimized, okay );
  }

  else if ( opr == OPR_LABEL ) {
    LABEL_IDX new_label = NewLabel();
    WN_label_number( copy ) =  new_label;
  }
  
  else {
    for (UINT kidno=0; kidno<WN_kid_count(tree); kidno++) {
      Substitute_Constant_Multiplicands( WN_kid(tree, kidno), 
					 WN_kid(copy, kidno), 
					 condition, freq, optimized, okay );
      if ( !*okay )
	return;
    }
  }
}

static WN* simplify_multiply ( WN* do_loop, LOWER_ACTIONS actions ) 

 /* Optimize multiplications inside a loop. 
    For example, 
   
      do i 
        ... a * b ...
        ... c * d ...
      enddo
   
    use the feedback information to optimize the multiplications as follows.

      do i
        if (a == const1 && d == const2) then
	  ... const1 * b ...
	  ... c * const2 ...
        else
	  ... a * b ...
	  ... c * d ...
        endif
      enddo

    We consider only the cases where the multiplicands are 0.0s or 1.0s.
    Returns the new body of the original loop.
 */
{
  WN* condition;
  WN* newblock;
  FB_FREQ* freq;
  WN* block = WN_do_body( do_loop );
  FB_Info_Loop info_loop;
  BOOL* optimized;
  BOOL* okay;

  if ( !Cur_PU_Feedback ) 
    return block;

  // IEEE_Arithmetic should be set to IEEE_INEXACT for this optimization.
  // This is because we need to opimize away '* 0.0' and '1.0 *' by calling the
  // simplifier.
#ifdef TARG_ST
  if ( !Finite_Math )
#else
  if ( IEEE_Arithmetic < IEEE_INEXACT )
#endif
    return block;
    
  // If the loop iteration count is not high enough then we do not attempt to 
  // simplify the loop.
  info_loop = Cur_PU_Feedback->Query_loop( do_loop );
  if ( info_loop.freq_iterate < (float)Mpy_Exe_Counter )
    return block;

  // Initialize
  condition = NULL;
  newblock = lower_copy_tree(block, actions);         
  freq = (FB_FREQ *)malloc(sizeof(FB_FREQ));
  *freq = info_loop.freq_iterate;    
  optimized = (BOOL *)malloc(sizeof(BOOL));
  *optimized = FALSE;
  okay = (BOOL *)malloc(sizeof(BOOL));
  *okay = TRUE;

  // Substitute the multiplicands that are gleaned to be 0.0 and 1.0
  // in block and add the list of conditions to condition. Also, update 
  // the frequency information for this set of conditions.
  Substitute_Constant_Multiplicands( block, newblock, &condition, 
				     freq, optimized, okay );

  if ( !*optimized || !*okay ) {
    WN_Delete( newblock );
    return block;
  }

  // Simplify new block (optimize away 0.0s and 1.0s).
  newblock = WN_Simplify_Tree( newblock );

  // Now, create the If-Then-Else block with the optimized block 'newblock' 
  // in the Then part and the original 'block' in the Else part.
  WN* orig_block = block;
  block = WN_COPY_Tree( block ); // Should erase the map to the feedback info.
  WN_Delete( orig_block );
  WN* IF = WN_CreateIf( condition, newblock, block );
  Cur_PU_Feedback->Annot( IF, FB_EDGE_BRANCH_TAKEN, *freq );
  Cur_PU_Feedback->Annot( IF, FB_EDGE_BRANCH_NOT_TAKEN, 
                          info_loop.freq_iterate - *freq );
  WN_Set_Linenum( newblock, current_srcpos );
  WN* optimized_block = WN_CreateBlock();
  WN_INSERT_BlockLast( optimized_block, IF );
  WN_Set_Linenum( optimized_block, current_srcpos );

  // Return the optimized block.
  return optimized_block;
}
#endif

/* ====================================================================
 *
 * WN *lower_do_loop(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on statements in DO_LOOP
 * node <tree>, returning lowered statements.  Returned tree will always
 * have a structured control flow node (at least a BLOCK) at the top.
 *
 * ==================================================================== */

static WN *lower_do_loop(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  WN *loop_info;

  Is_True(WN_opcode(tree) == OPC_DO_LOOP,
	  ("expected DO_LOOP node, not %s", OPCODE_name(WN_opcode(tree))));
#ifndef TARG_ST
  if (Action(LOWER_TO_MEMLIB)) {
     tree = Lower_Memlib(block, tree, actions);
     // Continue if Lower_Memlib returns do_loop
     if (WN_opcode(tree) != OPC_DO_LOOP) 
         return tree; 
  }
#endif
  loop_info = WN_do_loop_info(tree);  
  loop_nest_depth = loop_info ? WN_loop_depth(loop_info) : loop_nest_depth+1;

#ifdef TARG_X8664
  loop_info_stack[++current_loop_nest_depth] = NULL;
  FmtAssert( current_loop_nest_depth < MAX_LOOP_NEST_DEPTH,
	     ("loop nest depth too deep") );
  if( loop_info != NULL ){
    loop_info_stack[current_loop_nest_depth] = WN_loop_induction( loop_info );
    FmtAssert( WN_has_sym( loop_info_stack[current_loop_nest_depth] ),
	       ("loop_induction has no sym") );
  }
#endif  

#ifdef KEY // do this to see if the STID gets lowered to ISTORE
  if (NotAction(LOWER_TOP_LEVEL_ONLY))
    WN_start(tree) = lower_stmt(block, WN_start(tree), actions);
#endif  
  if (Action(LOWER_DO_LOOP)
#ifdef KEY // if not STID, would cause wopt assertion (bug 565)
      || WN_operator(WN_start(tree)) != OPR_STID
#endif
      )
  {
   /*
    *  Lower DO_LOOP.  Generate:
    *	<start>
    *	(FALSEBR <end> <cont_lbl>)		; unless nz_trip flag set
    *	(LABEL <top_lbl> <loop_info>)
    *	<body>
    *	<step>
    *	(TRUEBR <end> <top_lbl>)
    *  (LABEL <cont_lbl>)			; unless nz_trip flag set
    */
    BOOL nz_trip = loop_info && WN_Loop_Nz_Trip(loop_info);
    WN *wn_top_branch = NULL, *wn_back_branch = NULL;
    WN *body = WN_CreateBlock();

    if (Action(LOWER_TOP_LEVEL_ONLY))
    {
      actions = RemoveScfAction(actions);
    }

    contains_a_loop = FALSE;
   /*
    *  create loop info for CG
    *  it must be lowered as CG processes this
    */
    if (loop_info == NULL)
    {
      WN *infoblock = WN_CreateBlock();
      WN *trip_count = WN_LOOP_TripCount(tree);

      /* Set the nz_trip if we can */
      if (trip_count && WN_operator_is(trip_count, OPR_INTCONST) &&
	  WN_const_val(trip_count) > 0) {
	 nz_trip = TRUE;
      }
      loop_info = WN_CreateLoopInfo(WN_index(tree),
				    trip_count,
				    0,
				    loop_nest_depth,
				    contains_a_loop ? 0 : WN_LOOP_INNERMOST);
      loop_info = lower_expr(infoblock, loop_info, actions);
      WN_Set_Loop_Nz_Trip(loop_info);
      WN_DELETE_Tree(infoblock);
    }
    else if (WN_loop_induction(loop_info) != WN_index(tree))
    {
      WN_DELETE_Tree(WN_index(tree));
    }

    WN_INSERT_BlockLast(body, lower_stmt(block, WN_start(tree), actions));

    WN *cont_lbl;
    if (nz_trip == FALSE)
    {
      WN *end = lower_copy_tree(WN_end(tree), actions);

      if (Cur_PU_Feedback)
	Cur_PU_Feedback->FB_clone_loop_test( WN_end(tree), end, tree );

      cont_lbl = WN_NewLabel();
      WN *top_branch_block = lower_falsebr(WN_label_number(cont_lbl), end,
					   &wn_top_branch, actions);
      WN_INSERT_BlockLast(body, top_branch_block);
    }

    setCurrentState(WN_do_body(tree), actions);

    LABEL_IDX top_lbl_idx;
    top_lbl_idx = NewLabel();

    WN *top_lbl = WN_CreateLabel(ST_IDX_ZERO, top_lbl_idx, 0, loop_info);
    WN_Set_Linenum(top_lbl, current_srcpos);  // Bug 1268
    WN_INSERT_BlockLast(body, top_lbl);

    WN_INSERT_BlockLast(body, lower_block(WN_do_body(tree), actions));

    setCurrentState(WN_step(tree), actions);
    WN_INSERT_BlockLast(body, lower_stmt(block, WN_step(tree), actions));
    WN *back_branch_block = lower_truebr(WN_label_number(top_lbl),
					 WN_end(tree), &wn_back_branch,
					 actions);
    WN_INSERT_BlockLast(body, back_branch_block);

    if (nz_trip == FALSE) {
      WN_Set_Linenum(cont_lbl, current_srcpos);  // Bug 1268
      WN_INSERT_BlockLast(body, cont_lbl);
    }

    if ( Cur_PU_Feedback )
      Cur_PU_Feedback->FB_lower_loop( tree, wn_top_branch, wn_back_branch );

    WN_Delete(tree);

    tree = body;
  }
  else if (NotAction(LOWER_TOP_LEVEL_ONLY))
  {
    WN_start(tree) = lower_stmt(block, WN_start(tree), actions);
    WN_end(tree) = lower_expr(block, WN_end(tree), actions);
    WN_step(tree) = lower_stmt(block, WN_step(tree), actions);
#ifndef TARG_ST
#ifdef KEY
    // Bug 4731 - simplify_multiply creates a WN_COPY that clones a region
    // and an associated INITO. Now (1) when new node is deleted, the cloned
    // INITO is not deleted (2) New region created does not have a valid region
    // id. Due to (2) CG does not process the region and hence an unused 
    // variable (cloned copy) does not get 'not_used'. This leads CG to emit
    // unused variables. 
    if ( !PU_has_region(Get_Current_PU()) )
      if (NotAction(LOWER_FAST_EXP) /*bug 9676*/)
	WN_do_body(tree) = simplify_multiply( tree, actions );
#endif
#endif
    WN_do_body(tree) = lower_block(WN_do_body(tree), actions);
  }

#ifdef TARG_X8664
  loop_info_stack[current_loop_nest_depth--] = NULL;

  // Evaluate and mark loop direction
  if (WN_start(tree) != NULL) {
    WN *init_exp = WN_start(tree);
    if ( WN_operator(init_exp) == OPR_STID ) {
      if (WN_kid(init_exp,0) != NULL) {
        WN *store_val = WN_kid(init_exp,0);
        if ( WN_operator(store_val) == OPR_INTCONST ) {
          if (loop_info != NULL) {
            int trip_est = WN_loop_trip_est(loop_info);
            int init_val = WN_const_val(store_val);
            // annotate a up counting loop
            if (init_val < trip_est) {
              WN_Set_Loop_Up_Trip(loop_info);
            }
          }
        }
      }
    }
  }
#endif

  --loop_nest_depth;
  contains_a_loop = TRUE;
  return tree;
}



/* ====================================================================
 *
 * WN *lower_do_while(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on statements in DO_WHILE
 * node <tree>, returning lowered statements.  Returned tree will always
 * have a structured control flow node (at least a BLOCK) at the top.
 *
 * ==================================================================== */

static WN *lower_do_while(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  Is_True(WN_opcode(tree) == OPC_DO_WHILE,
	  ("expected DO_WHILE node, not %s", OPCODE_name(WN_opcode(tree))));

  ++loop_nest_depth;
#ifndef SHORTCIRCUIT_HACK
  if (Action(LOWER_DO_WHILE))
#else
  if (Action(LOWER_DO_WHILE) || 
      (Action(LOWER_SHORTCIRCUIT) && tree_has_cand_cior (WN_while_test(tree))))
#endif
  {
   /*
    * Lower DO_WHILE.  Generate:
    *	(LABEL <top_lbl>)
    *	<body>
    *	(TRUEBR <test> <top_lbl>)
    */
    WN *body = WN_CreateBlock();
    WN *top_lbl = WN_NewLabel();

    if (Action(LOWER_TOP_LEVEL_ONLY))
    {
      actions = RemoveScfAction(actions);
    }
    WN_Set_Linenum(top_lbl, current_srcpos);  // Bug 1268
    WN_INSERT_BlockFirst(body, top_lbl);
    WN_INSERT_BlockLast(body, lower_block(WN_while_body(tree), actions));

    WN *wn_back_branch = NULL;
    WN *back_branch_block
      = lower_truebr(WN_label_number(top_lbl), WN_while_test(tree),
		     &wn_back_branch, actions);
    WN_INSERT_BlockLast(body, back_branch_block);

    if ( Cur_PU_Feedback )
      Cur_PU_Feedback->FB_lower_loop( tree, NULL, wn_back_branch );

    WN_Delete(tree);
    tree = body;
  }
  else if (NotAction(LOWER_TOP_LEVEL_ONLY))
  {
    WN *testBlock = WN_CreateBlock();

    WN_while_body(tree) = lower_block(WN_while_body(tree), actions);

    WN_while_test(tree) = lower_expr(testBlock, WN_while_test(tree), actions);

    WN_INSERT_BlockLast(WN_while_body(tree), testBlock);
  }

  --loop_nest_depth;
  contains_a_loop = TRUE;
  return tree;
}

#ifdef TARG_ST
// FdF 21/11/2005: When a while-do loop is converted in a do-while
// form, loop pragmas are moved down just before the loop head. The
// reason is to keep loop pragmas closer to the loop, so as to be able
// to attach them to the right loop header when converting from WHIRL
// to CGIR.

static void
Move_Loop_Pragmas(WN *wn_orig, WN *block_orig, WN *block_dest)
{
  WN *wn, *prev_wn = NULL;
  for (wn = WN_prev(wn_orig); wn; wn = prev_wn) {
    prev_wn = WN_prev(wn);
    if (WN_operator(wn) == OPR_PRAGMA) {
      if (WN_Pragma_Scope(WN_pragma(wn)) == WN_PRAGMA_SCOPE_LOOP) {
	WN_EXTRACT_FromBlock(block_orig, wn);
	WN_INSERT_BlockLast(block_dest, wn);
      } 
    }
    else
      break;
  }
}

static BOOL
Loop_Pragma_Nz_Trip(WN *wn_orig) {
  WN *wn;
  int loopmin = 0;
  for (wn = WN_prev(wn_orig); wn; wn = WN_prev(wn)) {
    if (WN_operator(wn) != OPR_PRAGMA)
      break;
    if ((WN_PRAGMA_ID)WN_pragma(wn) == WN_PRAGMA_LOOPMINITERCOUNT)
      loopmin = MAX(loopmin, WN_pragma_arg1(wn));
    // FdF 20070914: LOOPMOD also defines a minimum iteration count
    // with its second argument
    else if ((WN_PRAGMA_ID)WN_pragma(wn) == WN_PRAGMA_LOOPMOD)
      loopmin = MAX(loopmin, WN_pragma_arg2(wn));
  }
  return (loopmin > 0);
}

#endif

/* ====================================================================
 *
 * WN *lower_while_do(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Lower WHILE_DO.  Generate:
 *	(FALSEBR (<test>) <cont_lbl>)
 *	(LABEL <top_lbl>)
 *	<body>
 *	(TRUEBR <test> <top_lbl>)
 *  (LABEL <cont_lbl>)
 *
 *
 * Perform lowering (see WN_Lower description) on statements in WHILE_DO
 * node <tree>, returning lowered statements.  Returned tree will always
 * have a structured control flow node (at least a BLOCK) at the top.
 *
 * ==================================================================== */

static WN *lower_while_do(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  Is_True(WN_opcode(tree) == OPC_WHILE_DO,
	  ("expected WHILE_DO node, not %s", OPCODE_name(WN_opcode(tree))));

  ++loop_nest_depth;
#ifndef SHORTCIRCUIT_HACK
  if (Action(LOWER_WHILE_DO))
#else
  if (Action(LOWER_WHILE_DO) || 
      (Action(LOWER_SHORTCIRCUIT) && tree_has_cand_cior (WN_while_test(tree))))
#endif
  {
#ifdef TARG_ST
    /* FdF 20050304: For OPT_Space, generates instead:
    *
    *	(LABEL <top_lbl>)
    *	(FALSEBR <test> <cont_lbl>)
    *	<body>
    *	(GOTO <top_lbl>)
    *  (LABEL <cont_lbl>)
    */
    if (OPT_Lower_While_Do_For_Space && !Loop_Pragma_Nz_Trip(tree)) {
#if 1
      LABEL_IDX top_lbl, cont_lbl;
      top_lbl  = NewLabel();
      cont_lbl = NewLabel();

      WN *test = lower_copy_tree( WN_while_test(tree), actions);
      WN *body = WN_CreateBlock();
      if (Cur_PU_Feedback)
	Cur_PU_Feedback->FB_clone_loop_test( WN_while_test(tree), test, tree );

      if (Action(LOWER_TOP_LEVEL_ONLY))
	{
	  actions = RemoveScfAction(actions);
	}

      WN *wn_top_lbl = WN_Label(top_lbl);
#ifdef TARG_ST
    // [TB] Improve line number.
    WN_copy_linenum(tree, wn_top_lbl);
#endif
      WN_INSERT_BlockLast(body, wn_top_lbl);

      WN *wn_top_branch = NULL;
      WN *top_branch_block = lower_falsebr(cont_lbl, test, &wn_top_branch,
					   actions);
      WN_INSERT_BlockLast(body, top_branch_block);

      setCurrentState(WN_while_body(tree), actions);

      setCurrentStateBlockLast(WN_while_body(tree), actions);
      WN_INSERT_BlockLast(body, lower_block(WN_while_body(tree), actions));

      WN *wn_back_branch = NULL;
      /* Generate GOTO <top_lbl> */
      WN *back_branch_block = WN_Goto(top_lbl);
      WN_INSERT_BlockLast(body, back_branch_block);
      WN *wn_cont_lbl = WN_Label(cont_lbl);
#ifdef TARG_ST
    // [TB] Improve line number.
    WN_copy_linenum(tree, wn_cont_lbl);
#endif
      WN_INSERT_BlockLast(body, wn_cont_lbl);

      if ( Cur_PU_Feedback )
	Cur_PU_Feedback->FB_lower_loop( tree, wn_top_branch, wn_back_branch );

      WN_Delete(tree);

      tree = body;
#else
   /*
    *	goto <test_lbl>) into block
    *	(LABEL <top_lbl>)
    *	<body>
    *   (LABEL <test_lbl>)
    *	(TRUEBR <test> <top_lbl>)
    */
    LABEL_IDX top_lbl, test_lbl;
    top_lbl  = NewLabel();
    test_lbl = NewLabel();

    WN *test = lower_copy_tree( WN_while_test(tree), actions);
    WN *body = WN_CreateBlock();
    if (Cur_PU_Feedback)
      Cur_PU_Feedback->FB_clone_loop_test( WN_while_test(tree), test, tree );

    if (Action(LOWER_TOP_LEVEL_ONLY))
    {
      actions = RemoveScfAction(actions);
    }

    WN *wn_top_branch = NULL;
    WN *top_branch_block = WN_Goto(test_lbl);
    WN_INSERT_BlockLast(block, top_branch_block);

    setCurrentState(WN_while_body(tree), actions);
    WN *wn_top_lbl = WN_Label(top_lbl);
    WN_INSERT_BlockLast(body, wn_top_lbl);

    setCurrentStateBlockLast(WN_while_body(tree), actions);
    WN_INSERT_BlockLast(body, lower_block(WN_while_body(tree), actions));

    WN *wn_test_lbl = WN_Label(test_lbl);
    WN_INSERT_BlockLast(body, wn_test_lbl);

    WN *wn_back_branch = NULL;
    WN *back_branch_block = lower_truebr(top_lbl, WN_while_test(tree),
					 &wn_back_branch, actions);
    WN_INSERT_BlockLast(body, back_branch_block);

    if ( Cur_PU_Feedback )
      Cur_PU_Feedback->FB_lower_loop( tree, wn_top_branch, wn_back_branch );

    WN_Delete(tree);

    tree = body;
#endif
    }
  else {
#endif
   /*
    *	(FALSEBR <test> <cont_lbl>) into block
    *	(LABEL <top_lbl>)
    *	<body>
    *	(TRUEBR <test> <top_lbl>)
    *  (LABEL <cont_lbl>)
    */
    LABEL_IDX top_lbl, cont_lbl;
    top_lbl  = NewLabel();
    cont_lbl = NewLabel();

    WN *test = lower_copy_tree( WN_while_test(tree), actions);
    WN *body = WN_CreateBlock();
    if (Cur_PU_Feedback)
      Cur_PU_Feedback->FB_clone_loop_test( WN_while_test(tree), test, tree );

    if (Action(LOWER_TOP_LEVEL_ONLY))
    {
      actions = RemoveScfAction(actions);
    }

    WN *wn_top_branch = NULL;
 #ifdef TARG_ST
    // FdF 20070830: If a loopminitercount says the loop iters at
    // least once, do not create the FALSEBR
    if (!Loop_Pragma_Nz_Trip(tree)) {
#endif
    WN *top_branch_block = lower_falsebr(cont_lbl, test, &wn_top_branch,
					 actions);
    WN_INSERT_BlockLast(block, top_branch_block);
#ifdef TARG_ST
    // FdF 21/11/2005 Move loop pragmas close to the new loop head
    Move_Loop_Pragmas(tree, block, block);
    }
#endif
    setCurrentStateBlockFirst(WN_while_body(tree), actions);
    WN *wn_top_lbl = WN_Label(top_lbl);
    WN_Set_Linenum(wn_top_lbl, current_srcpos);  // Bug 1268
    WN_INSERT_BlockLast(body, wn_top_lbl);

    setCurrentStateBlockLast(WN_while_body(tree), actions);
    WN_INSERT_BlockLast(body, lower_block(WN_while_body(tree), actions));

    WN *wn_back_branch = NULL;
    WN *back_branch_block = lower_truebr(top_lbl, WN_while_test(tree),
					 &wn_back_branch, actions);
    WN_INSERT_BlockLast(body, back_branch_block);
    WN *wn_cont_lbl = WN_Label(cont_lbl);
    // TODO:  WN_Set_Linenum(wn_cont_lbl, current_srcpos);  // Bug 1268
    WN_INSERT_BlockLast(body, wn_cont_lbl);

    if ( Cur_PU_Feedback )
      Cur_PU_Feedback->FB_lower_loop( tree, wn_top_branch, wn_back_branch );

    WN_Delete(tree);

    tree = body;
#ifdef TARG_ST
  }
#endif
  }
 /*
  * We're not lowering WHILE_DOs, so just lower children.
  * The semantics of the WHILE_DO require any statement level whirl
  * created during the lowering of the while test be copied to both the
  * block (before the WHILE_DO) and in the end of while_body
  */
  else if (NotAction(LOWER_TOP_LEVEL_ONLY))
  {
    WN *copytestBlock;
    WN *testBlock = WN_CreateBlock();

    WN_while_body(tree) = lower_block(WN_while_body(tree), actions);

    WN_while_test(tree) = lower_expr(testBlock, WN_while_test(tree), actions);
    copytestBlock = lower_copy_tree(testBlock, actions);

    if (Cur_PU_Feedback)
      Cur_PU_Feedback->FB_clone_loop_test( testBlock, copytestBlock, tree );

#ifdef TARG_ST
    // [TB] Improve line number.
    WN_copy_linenum(tree, copytestBlock);
#endif
    WN_INSERT_BlockLast(block, copytestBlock);
#ifdef TARG_ST
    // [TB] Improve line number.
    WN_copy_linenum(tree, testBlock);
#endif
    WN_INSERT_BlockLast(WN_while_body(tree), testBlock);
  }

  --loop_nest_depth;
  contains_a_loop = TRUE;
  return tree;
}



/* ====================================================================
 *
 *  The parameter types may not be correct, and as usual, we have to
 *  make up for it. Integral types are canonicalized [I,U][1,2] types
 *  (to [I,U]4) and do not require explicit conversion
 *  (the conversion happens when the value is loaded)
 *
 *  Mostly floats are passed as doubles and must be converted 
 *
 * ==================================================================== */
#ifdef TARG_ST
static WN *lower_promoted_formal(WN *block, PLOC ploc, ST *formalST)
#else
static WN *lower_promoted_formal(PLOC ploc, ST *formalST)
#endif
{
  WN		*ldid, *cvt;
  TYPE_ID	regType, formalType;

  formalType = TY_mtype(Ty_Table[ST_type(formalST)]);

  regType = Mtype_comparison(TY_mtype(Promoted_Parm_Type(formalST)));

  if (!PLOC_on_stack(ploc))
  {
#ifdef TARG_X8664
    if (Is_Target_64bit() && formalType == MTYPE_C4) 
      formalType = regType = MTYPE_F8;
#endif
    formalST = MTYPE_To_PREG(regType);
  }

  ldid = WN_Ldid(regType, PLOC_reg(ploc), formalST, ST_type(formalST));

#ifdef TARG_ST
  //
  // Arthur: this seems to happen after the RunTime lowering.
  //         If we need to "run-time" something, do it here
  //

  // If we may need some sort of type conversion, store this PREG
  // first and then reload
  if (WN_rtype(ldid) == formalType) return ldid;

  // Some sort of type conversion necessary, store this PREG
  // first and then reload
  PREG_NUM preg = AssignExpr (block, 
			      ldid,
			      WN_rtype(ldid));

  ldid = WN_LdidPreg (WN_rtype(ldid), preg);
  cvt =	WN_Type_Conversion(ldid, formalType);
  cvt = rt_lower_expr (cvt, LOWER_RUNTIME);

#else
  cvt =	WN_Type_Conversion(ldid, formalType);
#endif

  return cvt;
}

/* ====================================================================
 *
 * PLOC lower_formals(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 *  lower formal tree returning a ploc
 *
 * ==================================================================== */

static void lower_formals(WN *block, WN *formal, PLOC ploc,
			  LOWER_ACTIONS actions)
{
  BOOL    altentry = PU_has_altentry(Get_Current_PU());
  ST     *sym      = WN_st(formal);
  TY_IDX  formalTY = WN_type(formal);
  TYPE_ID mtype = TY_mtype(Ty_Table[formalTY]);

  if (PLOC_on_stack(ploc))
  {
    /* on stack already */
    if (altentry)
    {
      ST	*upformal;
      WN	*ldid, *stid;

      upformal = Get_Altentry_UpFormal_Symbol (sym, ploc);

      ldid = WN_Ldid(mtype, 0, upformal, formalTY);

      stid = WN_Stid(mtype, 0, sym, formalTY, ldid);

      stid = lower_store(block, stid, actions);
      WN_Set_Linenum (stid, current_srcpos);
      WN_INSERT_BlockLast(block, stid);
    }
    else if (ST_promote_parm(sym))
    {
      if (MTYPE_is_float(mtype))
      {
	WN	*cvt;
	WN	*stid;

#ifdef TARG_ST
	cvt  = lower_promoted_formal(block, ploc, sym);
#else
	cvt  = lower_promoted_formal(ploc, sym);
#endif
	stid = WN_Stid(mtype, 0, sym, formalTY, cvt);

	WN_Set_Linenum (stid, current_srcpos);
	WN_INSERT_BlockLast(block, stid);
      }
    }
  }
  else if (MTYPE_is_m(mtype))
  {
    /* structure parameter */
    lower_mload_formal (block, formal, ploc, actions);
  }
#ifdef TARG_X8664
  else if ((mtype == MTYPE_U8 || mtype == MTYPE_I8) &&
           Preg_Offset_Is_Int(PLOC_reg2(ploc)))
  {
    TYPE_ID type = Mtype_TransferSign(mtype, MTYPE_U4);
    WN * ldid, * stid;

    ldid = WN_LdidPreg (type, PLOC_reg(ploc));
    stid = WN_Stid(type, 0, sym, MTYPE_TO_TY_array[mtype], ldid);
    WN_Set_Linenum (stid, current_srcpos);
    WN_INSERT_BlockLast (block, stid);

    ldid = WN_LdidPreg (type, PLOC_reg2(ploc));
    stid = WN_Stid(type, 4, sym, MTYPE_TO_TY_array[mtype], ldid);
    WN_Set_Linenum (stid, current_srcpos);
    WN_INSERT_BlockLast (block, stid);
  }
#endif
  else
  {
    WN	*cvt, *stid;

#ifdef TARG_ST
    cvt  = lower_promoted_formal(block, ploc, sym);
#else
    cvt  = lower_promoted_formal(ploc, sym);
#endif

    stid = WN_Stid(mtype, 0, sym, formalTY, cvt);

    WN_Set_Linenum (stid, current_srcpos);
    WN_INSERT_BlockLast(block, stid);
  }
}



/* ====================================================================
 *
 * PLOC lower_entry_formals(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering on ENTRY and ALTENTRY nodes
 *
 * for LOWER_ENTRY_EXIT
 *		lower all but sclass formal refs
 * for LOWER_ENTRY_FORMAL_REF
 *		lower just sclass formal refs
 *
 * ==================================================================== */

static PLOC lower_entry_formals(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  PLOC	ploc;
  INT32	i, n;

  ploc = Setup_Input_Parameter_Locations(ST_pu_type(WN_st(tree)));

  if (WN_opcode(tree) == OPC_ALTENTRY)
  {
    n =	WN_kid_count(tree);
    Reset_UPFORMAL_Segment();
  }
  else
  {
    n =	WN_num_formals(tree);
  }

  for (i = 0; i < n; i++)
  {
    WN *formal = WN_formal(tree, i);
	
    if (WN_sclass(formal) == SCLASS_FORMAL_REF)
    {
      ST *base;

      base = Get_ST_formal_ref_base(WN_st(formal));
      ploc = Get_Input_Parameter_Location(ST_type(base));

      if (NotAction(LOWER_ENTRY_FORMAL_REF))
      {
	continue;
      }
      else
      {
	formal = WN_CreateIdname(WN_idname_offset(formal), base);
      }
    }
    else
    {
      ploc = Get_Input_Parameter_Location(WN_type(formal));

      if (NotAction(LOWER_ENTRY_EXIT))
      {
	continue;
      }
    }
    lower_formals(block, formal, ploc, actions);
  }

  return ploc;
}

#ifdef TARG_ST //[CG]
/* ====================================================================
 *
 * void lower_entry_promoted_formals(WN *block, WN *tree)
 *
 * [CG]
 * Perform lowering on ENTRY and ALTENTRY nodes of PROMOTED formals parameters
 * Called in lower_entry when LOWER_ENTRY_PROMOTED action is on.
 * MUST BE CALLED BEFORE RT_LOWERING (for lowering of double->float cvts)
 * AND BEFORE LOWER_ENTRY_EXIT (as it change formal parameters).
 *
 * A pass through the promoted formal parameters generate an explicit
 * promoted type->formal type conversion:
 * 1. create a new formal symbol (promotedST) with the promoted type
 *    named ".caller.<name>";
 * 2. redefine the initial formal symbol (formalST) to be AUTO 
 *    instead of FORMAL;
 * 3. replace idname entry with the new promoted formal symbol;
 * 4. generate a converstion from promotedST to formalST.
 *
 * The RT_LOWERING will eventually lower the conversion later.
 * The LOWER_ENTRY_EXIT will see the new (promotedST) formal and
 * generate correct sequence.
 * The data_layout phase will see the new (promotedST) formal and
 * generate a correct layout.
 * For instance:
 * int fadd(a,b) float a,b; { ... } 
 * input tree is:
 * FUNC_ENTRY <fadd>
 *  IDNAME 0 <a>
 *  IDNAME 0 <b>
 * BODY
 *  BLOCK
 *   PRAGMA # PREAMBLE_END
 *   ...
 *
 * output is:
 * FUNC_ENTRY <fadd>
 *  IDNAME 0 <.caller.a>
 *  IDNAME 0 <.caller.b>
 * BODY
 *  BLOCK
 *     F8F8LDID 0 <.caller.a> T<.predef_F8,8>
 *    F4F8CVT
 *   F4STID 0 <a> T<.predef_F4,4>
 *     F8F8LDID 0 <.caller.b> T<.predef_F8,8>
 *    F4F8CVT
 *   F4STID 0 <b> T<.predef_F4,4>
 *   PRAGMA # PREAMBLE_END
 * ...
 * ==================================================================== */
static WN *lower_caller_callee_formal(ST *promotedST, ST *formalST)
{
  WN		*ldid, *cvt;
  TYPE_ID	promotedType, formalType;

  promotedType = TY_mtype(Ty_Table[ST_type(promotedST)]);
  formalType = TY_mtype(Ty_Table[ST_type(formalST)]);

  ldid = WN_Ldid(promotedType, 0, promotedST, ST_type(promotedST));
  cvt =	WN_Type_Conversion(ldid, formalType);
  return cvt;
}

static void
lower_entry_promoted_formals(WN *block, WN *tree)
{
  INT32	i, n;
  
  if (WN_opcode(tree) == OPC_ALTENTRY) {
    n =	WN_kid_count(tree);
  } else {
    n =	WN_num_formals(tree);
  }
  
  for (i = 0; i < n; i++) {
    WN *formal = WN_formal(tree, i);
    
    ST     *formalST = WN_st(formal);
    TY_IDX  formalTY = WN_type(formal);
    TY_IDX  regTY;
    if (ST_promote_parm(formalST)) {
      regTY = Promoted_Parm_Type(formalST);
      ST *promotedST = New_ST (ST_level(formalST));
      ST_Init (promotedST, Save_Str2(".caller.", ST_name(formalST)),
	       CLASS_VAR, SCLASS_FORMAL, EXPORT_LOCAL, regTY);
      Set_ST_is_value_parm(promotedST);

      /* Transform original formal into automatic. */
      Set_ST_sclass(formalST, SCLASS_AUTO);
      Clear_ST_is_value_parm(formalST);

      WN *cvt = lower_caller_callee_formal(promotedST, formalST); 
      WN *stid = WN_Stid(TY_mtype(formalTY), ST_ofst(formalST), formalST, ST_type(formalST), cvt);
      WN_Set_Linenum (stid, current_srcpos);
      WN_INSERT_BlockLast(block, stid);

      // Replace the funcentry kid by the promoted formal
      WN *idname = WN_CreateIdname(0, promotedST);
      WN_kid(tree, i) = idname;
      WN_Delete(formal);
    }
  }
}
#endif

#ifdef TARG_ST //[JV]
static WN *lower_caller_hidden_reference_formal(ST *ptrST, ST *formalST)
{
  TYPE_ID ptrType = TY_mtype(Ty_Table[ST_type(ptrST)]);
  TYPE_ID formalType = TY_mtype(Ty_Table[ST_type(formalST)]);
  INT32 size = TY_size(Ty_Table[ST_type(formalST)]);

  WN *ldid_wn = WN_Ldid(ptrType, 0, ptrST, ST_type(ptrST));
  WN *size_wn = WN_Intconst(Integer_type, size);
  WN *mload_wn = WN_CreateMload(0, Make_Pointer_Type(ST_type(formalST)), ldid_wn, size_wn);
  return mload_wn;
}



static void
lower_entry_hidden_ref_formals( WN *block, WN *tree, LOWER_ACTIONS actions)
{
  INT32	i, n;
  PLOC ploc;
  
  if (WN_opcode(tree) == OPC_ALTENTRY) {
    n =	WN_kid_count(tree);
  } else {
    n =	WN_num_formals(tree);
  }

  ploc = Setup_Input_Parameter_Locations(ST_pu_type(WN_st(tree)));

  for (i = 0; i < n; i++) {
    WN *formal = WN_formal(tree, i);
    
    ST     *formalST = WN_st(formal);
    TY_IDX  formalTY = WN_type(formal);
    TY_IDX  regTY;

    ploc = Get_Input_Parameter_Location(formalTY);
  
    if (PLOC_by_reference(ploc)) {
      regTY = Make_Pointer_Type(formalTY);
      ST *ptrST = New_ST (ST_level(formalST));
      ST_Init (ptrST, Save_Str2(".caller.ptr.", ST_name(formalST)),
	       CLASS_VAR, SCLASS_FORMAL, EXPORT_LOCAL, regTY);
      Set_ST_is_value_parm(ptrST);

      /* Transform original formal into automatic. */
      Set_ST_sclass(formalST, SCLASS_AUTO);
      Clear_ST_is_value_parm(formalST);

      WN *mload = lower_caller_hidden_reference_formal(ptrST, formalST);
      WN *stid = WN_Stid(TY_mtype(Ty_Table[formalTY]), 0, formalST, formalTY, mload);
      WN_Set_Linenum (stid, current_srcpos);

#if 0
      lower_copy_maps(formal, stid, actions);
#endif
      stid = lower_store(block, stid, actions);

      WN_INSERT_BlockLast(block, stid);

      // Replace the funcentry kid by the promoted formal
      WN *idname = WN_CreateIdname(0, ptrST);
      WN_kid(tree, i) = idname;
      WN_Delete(formal);
    }
  }
}
#endif

#ifdef TARG_ST
/*
 * [CG 2004/11/26]
 * Create_Return_Address_Symbol ()
 * This function creates the temporary symbol that will hold the return 
 * address of the current function.
 * It marks the function has having a return address symbol.
 * The initialization of the return address symbol is done later in
 * the code generator (calls.cxx).
 */
static ST *return_address_st;
static ST *
Create_Return_Address_Symbol ()
{
  return_address_st = Get_Special_Return_Address_Symbol();
  Set_PU_has_return_address (Get_Current_PU ());
  Set_PU_no_inline (Get_Current_PU ());
  return return_address_st;
}

/*
 * Create_MCount_Func_Call()
 * Create a call to extern void fname(void *return_pc, void * self_pc).
 * 
 */
inline WN *
Gen_Param( WN *arg, UINT32 flag )
{
  return WN_CreateParm( WN_rtype( arg ), arg,
			MTYPE_To_TY( WN_rtype( arg ) ), flag );
}
static WN *
Create_MCount_Func_Call (const char *fname)
{
  TY_IDX ty = Make_Function_Type(MTYPE_To_TY(MTYPE_V));
  ST *st = Gen_Intrinsic_Function(ty, (char *)fname);

  Clear_PU_no_side_effects(Pu_Table[ST_pu(st)]);
  Clear_PU_is_pure(Pu_Table[ST_pu(st)]);
  Set_PU_no_delete(Pu_Table[ST_pu(st)]);
  Set_PU_no_instrument_function(Pu_Table[ST_pu(st)]);
  
  WN *wn_call = WN_Call(MTYPE_V, MTYPE_V, 2, st);

  WN_Set_Call_Default_Flags(wn_call);
  // WN_Reset_Call_Non_Parm_Mod( wn_call );
  // WN_Reset_Call_Non_Parm_Ref( wn_call );

  WN *arg1 = WN_Ldid(Pointer_type, 0, return_address_st, 
		     ST_type(return_address_st));
  WN *arg2 = WN_Lda(Pointer_type, 0, WN_st(current_function));

  WN_actual(wn_call, 0) = WN_CreateParm(WN_rtype( arg1 ), arg1,
					MTYPE_To_TY(WN_rtype(arg1)), 
					WN_PARM_BY_VALUE);
  WN_actual(wn_call, 1) = WN_CreateParm(WN_rtype( arg1 ), arg2,
					MTYPE_To_TY(WN_rtype(arg2)), 
					WN_PARM_BY_VALUE);

  return wn_call;
}


/*
 * Create_Profile_Func_Call()
 * Create a call to extern void fname(void *self_pc, void *return_pc).
 * The same as Create_Mcount_Func_Call() except that the two args
 * are swapped.
 */
static WN *
Create_Profile_Func_Call (const char *fname)
{
  TY_IDX ty = Make_Function_Type(MTYPE_To_TY(MTYPE_V));
  ST *st = Gen_Intrinsic_Function(ty, (char *)fname);

  Clear_PU_no_side_effects(Pu_Table[ST_pu(st)]);
  Clear_PU_is_pure(Pu_Table[ST_pu(st)]);
  Set_PU_no_delete(Pu_Table[ST_pu(st)]);
  Set_PU_no_instrument_function(Pu_Table[ST_pu(st)]);

  WN *wn_call = WN_Call(MTYPE_V, MTYPE_V, 2, st);

  WN_Set_Call_Default_Flags(wn_call);
  // WN_Reset_Call_Non_Parm_Mod( wn_call );
  // WN_Reset_Call_Non_Parm_Ref( wn_call );

  WN *arg2 = WN_Ldid(Pointer_type, 0, return_address_st, 
		     ST_type(return_address_st));
  WN *arg1 = WN_Lda(Pointer_type, 0, WN_st(current_function));

  WN_actual(wn_call, 0) = WN_CreateParm(WN_rtype( arg1 ), arg1,
					MTYPE_To_TY(WN_rtype(arg1)), 
					WN_PARM_BY_VALUE);
  WN_actual(wn_call, 1) = WN_CreateParm(WN_rtype( arg1 ), arg2,
					MTYPE_To_TY(WN_rtype(arg2)), 
					WN_PARM_BY_VALUE);

  return wn_call;
}

/*
 * Create_Mcount_Call()
 * Create a call to the mcount function (-pg option).
 */
static WN *
Create_Mcount_Call ()
{
  return Create_MCount_Func_Call ("_mcount");
}

/*
 * Create_Profile_Func_Enter_Call ()
 * Create_Profile_Func_Exit_Call ()
 * Create a call to the enter and exit functions respectivelly.
 * Support for -finstrument-functions.
 */
static WN *
Create_Profile_Func_Enter_Call ()
{
  return Create_Profile_Func_Call ("__cyg_profile_func_enter");
}

static WN *
Create_Profile_Func_Exit_Call ()
{
  return Create_Profile_Func_Call ("__cyg_profile_func_exit");
}

/*
 * Create_Profile_Func_Enter_Call_For_PG ()
 * Create_Profile_Func_Exit_Call_For_PG ()
 * Create a call to the enter and exit functions respectivelly.
 * Used for profiling. These functions are defined in libgprof.a for stxp70 target
 */
static WN *
Create_Profile_Func_Enter_Call_For_PG ()
{
  return Create_Profile_Func_Call ("__profile_func_enter");
}

static WN *
Create_Profile_Func_Exit_Call_For_PG ()
{
  return Create_Profile_Func_Call ("__profile_func_exit");
}

#else

static WN *
Create_Mcount_Call (void)
{
#ifdef TARG_X8664
  WN* wn = WN_Create ( OPC_VCALL, 0);
  static ST* mcount_st = NULL;

  if (!mcount_st) {
    PU_IDX pu_idx;
    PU& pu = New_PU (pu_idx);
    char mcount_name[20];

    TY_IDX func_ty_idx;
    TY &func_ty = New_TY(func_ty_idx);
    TY_Init( func_ty, 0, KIND_FUNCTION, MTYPE_UNKNOWN, STR_IDX_ZERO );
    Set_TY_align( func_ty_idx, 1 );
    uint32_t tylist_idx;
    Set_TYLIST_type( New_TYLIST (tylist_idx), MTYPE_To_TY(Pointer_Mtype) );
    Set_TY_tylist( func_ty, tylist_idx );
    Set_TYLIST_type( New_TYLIST (tylist_idx), 0 );

    Set_TY_has_prototype(func_ty_idx);

    mcount_st = New_ST ( GLOBAL_SYMTAB );
    PU_Init (pu, func_ty_idx, CURRENT_SYMTAB);
    sprintf(mcount_name, "mcount");
    ST_Init (mcount_st, Save_Str(mcount_name),
    	     CLASS_FUNC, SCLASS_EXTERN, 
	     EXPORT_PREEMPTIBLE,
	     TY_IDX(pu_idx));
  }
  WN_st_idx(wn) = ST_st_idx(mcount_st);
  return wn;
#endif
    // don't do anything here,
    // because ia64 mcount call is non-standard calling convention,
    // so implement it in target-dependent cg.
    return NULL;
}
#endif

#ifdef TARG_ST
/*
 * Return true if the current function entry/exit must be instrumented.
 * The no_instrument_function attribute is checked.
 */
static BOOL
Instrument_Function_Enabled() {
  return (Instrument_Functions_Enabled || Instrument_Functions_Enabled_For_PG ) && WN_has_sym(current_function) &&
    !PU_no_instrument_function(Pu_Table[ST_pu(WN_st(current_function))]);
}

/*
 * Return true if the current function must be instrumented with mcount.
 */
static BOOL
Instrument_MCount_Enabled() {
  return Call_Mcount && WN_has_sym(current_function);
}

/*
 * lower_preamble_end()
 * Lowering at the point of preamble end
 *
 * Currently instrumentation at function entry is inserted at this point.
 */
static WN *lower_preamble_end(WN *block, WN *tree, LOWER_ACTIONS actions)
{

  if (!Action(LOWER_INSTRUMENT)) return tree;

  // Support for -finstrument-function and -pg.
  // Do the creation of the return address temporary before 
  // the call to mcount and to cyg_profile functions.
  // We let mcount be called before the cyg_profile calls in case it
  // directly use dedicated register, as in this case it must be the first
  // call in the function.
  // 
  if (Instrument_MCount_Enabled() || Instrument_Function_Enabled()) {
    Create_Return_Address_Symbol();
    
    WN *new_block = WN_CreateBlock();
    
    if (Instrument_MCount_Enabled()) {
      WN *profcall = Create_Mcount_Call ();
      if (profcall) {	// possible that we won't create call yet
	WN_Set_Linenum (profcall, current_srcpos);
	WN_INSERT_BlockLast(new_block, profcall);
      }
    }
    
    if (Instrument_Function_Enabled()) {
      WN *func_enter_call;
      if (Instrument_Functions_Enabled_For_PG)
	func_enter_call = Create_Profile_Func_Enter_Call_For_PG();
      else
	func_enter_call = Create_Profile_Func_Enter_Call();

      if (func_enter_call) {	// possible that we won't create call yet
	WN_Set_Linenum (func_enter_call, current_srcpos);
	WN_INSERT_BlockLast(new_block, func_enter_call);
      }
    }
    /* The block is lowered and inserted just before the preamble end. */
    if (WN_first(new_block) != NULL) {
      // Disable lowering of nested function pointers as we
      // need to pass the real functions addresses.
      LOWER_ACTIONS new_actions = actions & ~LOWER_NESTED_FN_PTRS;
      new_block = lower_block(new_block, new_actions);
      WN_INSERT_BlockLast(block, new_block);
    }
  }
  return tree;
}

/*
 * lower_return()
 * Lowering of operator OPR_RETURN
 *
 * Currently the only use is for inserting instrumenation for the
 * function exit.
 */
static WN *lower_return(WN *block, WN *tree, LOWER_ACTIONS actions)
{

  if (!Action(LOWER_INSTRUMENT)) return tree;

  // [CG] support for -finstrument-function
  if (Instrument_Function_Enabled()) {
    WN *new_block = WN_CreateBlock();
    WN *func_exit_call;
    if (Instrument_Functions_Enabled_For_PG)
      func_exit_call = Create_Profile_Func_Exit_Call_For_PG();
    else
      func_exit_call = Create_Profile_Func_Exit_Call();
      
    if (func_exit_call) { // possible that we won't create call yet
      WN_Set_Linenum (func_exit_call, current_srcpos);
      WN_INSERT_BlockFirst(new_block, func_exit_call);
    }
    
    /* Lower and insert the new block if not empty. */
    if (WN_first(new_block) != NULL) {
      // Disable lowering of nested function pointers as we
      // need to pass the real functions addresses.
      LOWER_ACTIONS new_actions = actions & ~LOWER_NESTED_FN_PTRS;
      new_block = lower_block(new_block, new_actions);
      
      /* Find the insertion point of the block.
       * It must be inserted before all return values initializations.
       * The return values initializations are identified by store to
       * dedicated pregs.
       */
      WN *point = WN_last(block);
      while(point != NULL) {
	if (!stmt_is_store_of_return_value(point))
	  break;
	point = WN_prev(point);
      }
      if (point == NULL) {
	WN_INSERT_BlockFirst(block, new_block);
      } else {
	WN_INSERT_BlockAfter(block, point, new_block);
      }
    }
  }
  return tree;
}
#endif

/*
 * stuff function for the following call.
 * csc. 2002/11/13
 */
                                                                                                                                                             
struct is_omp_slink
{
    BOOL operator () (UINT32, const ST *st) const
    {
        return (strncmp (ST_name (st), "__ompv_slink_a", 14) == 0);
    }
};
                                                                                                                                                             
/* ===================================================================
 *
 * BOOL Is_Omp_Gen_Slink( ST * slink )
 *
 * For OpenMP Lower generated slink, return TRUE
 * else return FALSE
 *
 * Since The OpenMP lower generates a call not compatible with
 * the one in orginal implementation, the OpenMP lower has generate
 * the slink initialization code yet. The following call is used to
 * decide whether the current PU is a OpenMP lower generated PU.
 * the arg slink is not used indeed.
 * csc. 2002/11/13.
 *
 * ================================================================= */
static BOOL Is_Omp_Gen_Slink( ST * slink )
{
    if( PU_mp( Get_Current_PU( )) && PU_is_nested_func( Get_Current_PU( )))
    {
        // Every OpenMP Lower generated function has a arg "__ompv_slink_a"
        ST_IDX st_idx = For_all_until( St_Table,
                                       CURRENT_SYMTAB, is_omp_slink( ));
        if( st_idx != 0 )
            return TRUE;
    }
                                                                                                                                                             
    return FALSE;
}

#ifdef KEY // for handling taking address of nested functions
struct NESTED_FUNC_TRAMPOLINE_PAIR {
  ST_IDX nested_func_st_idx;
  ST_IDX trampoline_st_idx;
  NESTED_FUNC_TRAMPOLINE_PAIR(void) {}
  NESTED_FUNC_TRAMPOLINE_PAIR(ST_IDX nested_func, ST_IDX trampoline):
	      nested_func_st_idx(nested_func), trampoline_st_idx(trampoline) {}
};

STACK<NESTED_FUNC_TRAMPOLINE_PAIR> nested_func_trampoline_map(Malloc_Mem_Pool);

static ST_IDX find_trampoline(ST_IDX func_st_idx) {
  INT i;
  for (i = 0; i < nested_func_trampoline_map.Elements(); i++) {
    if (nested_func_trampoline_map.Top_nth(i).nested_func_st_idx == func_st_idx)
      return nested_func_trampoline_map.Top_nth(i).trampoline_st_idx;
  }
  Fail_FmtAssertion("cannot find trampoline for nested function (%s)",
		    ST_name(func_st_idx));
}

static void generate_trampoline_symbol(ST *st) {
  PU &pu = Pu_Table[ST_pu(st)];
  if (PU_need_trampoline(pu)) {
    ST *tramp_st = Gen_Temp_Symbol(
		    MTYPE_To_TY(Is_Target_64bit() ? MTYPE_C10: MTYPE_F10), 
		    ST_name(st));
    Set_ST_addr_saved(tramp_st);
    Set_ST_is_shared_auto(*tramp_st);
    // remember the trampoline symbol generated for this nested function
    nested_func_trampoline_map.Push(
	  NESTED_FUNC_TRAMPOLINE_PAIR(ST_st_idx(st), ST_st_idx(tramp_st)));
  }
}
#ifndef TARG_ST
/* ====================================================================
 *
 * void allocate_trampoline_symbols(void)
 *
 * For each function nested within current function marked PU_NEED_TRAMPOLINE,
 * generate a corresponding local symbol of size 20 bytes that contains the
 * code for its trampoline that will be initialized at run-time
 *
 * ================================================================= */
static void allocate_trampoline_symbols(void)
{
  // first, throw away nodes on the stack for trampolines in previous PUs
  // at same level as current PU
  while (! nested_func_trampoline_map.Is_Empty() &&
	 PU_lexical_level(ST_pu(St_Table[nested_func_trampoline_map.Top().nested_func_st_idx]))
	      >= CURRENT_SYMTAB+1)
    nested_func_trampoline_map.Pop();

  if (! PU_c_lang(Get_Current_PU())) // bug 14271
    return;

  if (PU_uplevel(Get_Current_PU())) {
    PU &pu = Get_Current_PU();
    INITO_IDX pu_inito = PU_misc_info(pu);
    INITV_IDX nested_fn_iv;
    ST_IDX st_idx;
    if (pu_inito != INITO_IDX_ZERO) {
      nested_fn_iv = INITO_val(pu_inito);
      do {
	st_idx = INITV_st(nested_fn_iv);
	generate_trampoline_symbol(&St_Table[st_idx]);
	nested_fn_iv = INITV_next(nested_fn_iv);
      } while (nested_fn_iv != INITV_IDX_ZERO);
    }
  }
}
#endif
BOOL PU_has_trampoline = FALSE; // for communicating to cgemit

/* ====================================================================
 *
 * void generate_trampoline_code(void)
 *
 * For each trampoline allocated in current PU, generate the code
 * sequence that initialize the code content of the trampoline at run time
 * at the entry point of current PU
 *
 * ================================================================= */
static void generate_trampoline_code(WN *block)
{
  INT i;
  NESTED_FUNC_TRAMPOLINE_PAIR cur;
  for (i = 0; i < nested_func_trampoline_map.Elements(); i++) {
    cur = nested_func_trampoline_map.Top_nth(i);
    if (PU_lexical_level(ST_pu(St_Table[cur.nested_func_st_idx])) != CURRENT_SYMTAB+1)
      break;
    // generate the runtime initialization code for this trampoline and
    // insert at beginning of block
    WN *stid, *link;
    if (Is_Target_64bit()) {
      stid = WN_CreateStid(OPC_I2STID, 0, cur.trampoline_st_idx, 
			   MTYPE_To_TY(MTYPE_I2), WN_Intconst(MTYPE_I4, -17599));
      WN_INSERT_BlockFirst(block, stid);
      stid = WN_CreateStid(OPC_I4STID, 2, cur.trampoline_st_idx, 
			   MTYPE_To_TY(MTYPE_I4), 
		     WN_Lda(Pointer_Mtype, 0, &St_Table[cur.nested_func_st_idx]));
      WN_INSERT_BlockFirst(block, stid);
      stid = WN_CreateStid(OPC_I2STID, 6, cur.trampoline_st_idx, 
			   MTYPE_To_TY(MTYPE_I2), WN_Intconst(MTYPE_I4, -17847));
      WN_INSERT_BlockFirst(block, stid);
      link = WN_LdidPreg(Pointer_Mtype, Frame_Pointer_Preg_Offset);
      stid = WN_CreateStid(OPC_U8STID, 8, cur.trampoline_st_idx, 
			   MTYPE_To_TY(Pointer_Mtype), link);
      WN_INSERT_BlockFirst(block, stid);
      stid = WN_CreateStid(OPC_I2STID, 16, cur.trampoline_st_idx, 
			   MTYPE_To_TY(MTYPE_I2), WN_Intconst(MTYPE_I4, -183));
      WN_INSERT_BlockFirst(block, stid);
      stid = WN_CreateStid(OPC_I1STID, 18, cur.trampoline_st_idx, 
			   MTYPE_To_TY(MTYPE_I1), WN_Intconst(MTYPE_I4, -29));
      WN_INSERT_BlockFirst(block, stid);
    }
    else {
      stid = WN_CreateStid(OPC_I1STID, 0, cur.trampoline_st_idx, 
			   MTYPE_To_TY(MTYPE_I1), WN_Intconst(MTYPE_I4, -71));
      WN_INSERT_BlockFirst(block, stid);
      link = WN_LdidPreg(Pointer_Mtype, Frame_Pointer_Preg_Offset);
      stid = WN_CreateStid(OPC_U4STID, 1, cur.trampoline_st_idx, 
			   MTYPE_To_TY(Pointer_Mtype), link);
      WN_INSERT_BlockFirst(block, stid);
      stid = WN_CreateStid(OPC_I1STID, 5, cur.trampoline_st_idx, 
			   MTYPE_To_TY(MTYPE_I1), WN_Intconst(MTYPE_I4, -23));
      WN_INSERT_BlockFirst(block, stid);
      stid = WN_CreateStid(OPC_U4STID, 6, cur.trampoline_st_idx, 
			   MTYPE_To_TY(Pointer_Mtype),
			   WN_Sub(Pointer_Mtype,
		   WN_Lda(Pointer_Mtype, 0, &St_Table[cur.nested_func_st_idx]),
		   WN_Lda(Pointer_Mtype, 10, &St_Table[cur.trampoline_st_idx])));
      WN_INSERT_BlockFirst(block, stid);
    }

    PU_has_trampoline = TRUE;
  }
}
#endif

/* ====================================================================
 *
 * WN *lower_entry(WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering on ENTRY and ALTENTRY nodes
 *
 * for ENTRY
 *		return entry node
 * for ALTENTRY
 *		return block with lowered formals 
 *
 * for LOWER_ENTRY_EXIT
 *		create trapuv code (uninitialized variable)
 *		create varargs register assignment
 *		create slink sym initialization
 *
 * for LOWER_RETURN_VAL
 *		create fake first parameter for struct return if needed
 *
 * ==================================================================== */

static WN *lower_entry(WN *tree, LOWER_ACTIONS actions)
{
  PLOC	ploc;
  WN	*block;

  Is_True(((WN_opcode(tree) == OPC_FUNC_ENTRY)
	   || (WN_opcode(tree) == OPC_ALTENTRY)),
          ("expected ENTRY/ALTENTRY node, not %s",
	   OPCODE_name(WN_opcode(tree))));

  if (WHIRL_Return_Val_On && Action(LOWER_RETURN_VAL) && 
      WN_operator(tree) == OPR_FUNC_ENTRY)
  { // create fake first parameter for struct return if needed 
    ST_IDX func_stidx = WN_st_idx(tree);
    PU_IDX puidx = ST_pu(St_Table[func_stidx]);
    TY_IDX prototype = PU_prototype(Pu_Table[puidx]);
    RETURN_INFO return_info = Get_Return_Info(TY_ret_type(prototype), 
					      Complex_Not_Simulated
#ifdef TARG_X8664
    				              ,	PU_ff2c_abi(Pu_Table[puidx])
#endif
					     );
#ifdef TARG_ST
    /* (cbr) nrv formal is already created in the frontend */
    if (RETURN_INFO_return_via_first_arg(return_info) &&
        !TY_return_to_param(prototype)) {
#else
    if (RETURN_INFO_return_via_first_arg(return_info)) {
#endif
      ST *return_st = Gen_Temp_Symbol(
			      Make_Pointer_Type(TY_ret_type(prototype), FALSE),
			      Index_To_Str(Save_Str2(".return.",
						       ST_name(func_stidx))));
      Set_ST_sclass(return_st, SCLASS_FORMAL);
      Set_ST_is_value_parm(return_st);
      WN *idname = WN_CreateIdname(0, return_st);
      // create the new func_entry node
      WN *n_tree = WN_CreateEntry(WN_num_formals(tree)+1, func_stidx,
				  WN_func_body(tree), WN_func_pragmas(tree),
				  WN_func_varrefs(tree));
      WN_Set_Linenum(n_tree, WN_Get_Linenum(tree));
      WN_set_map_id(n_tree, WN_map_id(tree));
      WN_kid0(n_tree) = idname;
      for (INT32 i = 0; i < WN_kid_count(tree); i++)
	WN_kid(n_tree, i+1) = WN_kid(tree, i);
      // fix pu pointer from RID
      if (RID_map != WN_MAP_UNDEFINED) {
        RID *rid = (RID *)WN_MAP_Get(RID_map, tree);
        if (RID_rwn(rid) == tree)
	  RID_rwn(rid) = n_tree;
      }
      WN_Delete(tree);
      tree = n_tree;
#ifdef KEY
      // Update pu_info (bug 6071)
      Set_PU_Info_tree_ptr (Current_PU_Info, tree);
#endif
    }
  }

  setCurrentState(tree, actions);
  block =		WN_CreateBlock();

#ifdef TARG_ST //[CG]
  if (Action(LOWER_ENTRY_PROMOTED) &&
      WN_operator(tree) == OPR_FUNC_ENTRY) {
    lower_entry_promoted_formals(block, tree);
  }

  if (Action(LOWER_FORMAL_HIDDEN_REF) &&
      WN_operator(tree) == OPR_FUNC_ENTRY) {
    lower_entry_hidden_ref_formals(block, tree, actions);
  }

#endif

  if (Action(LOWER_ENTRY_EXIT))
  {
    ploc = lower_entry_formals(block, tree, actions);

#ifdef TARG_ST
    if (TY_is_varargs(Ty_Table[PU_prototype(Pu_Table[ST_pu(WN_st(tree))])]))
    {
      /*
      *  For varargs, the func-entry just has the list of fixed
      *  parameters, so also have to store the vararg registers. 
      */
      TYPE_ID	type = Def_Int_Mtype;
      
      ploc = Get_Vararg_Input_Parameter_Location (ploc);

      while (!PLOC_on_stack(ploc))
      {
       /*
        *  vararg registers must be integer registers
        */
	WN	*wn;
	WN 	*var_addr;
	ST	*st;
	INT 	offset;
	ST      *preg_sym = Get_Vararg_Preg_Symbol(ploc);
	type = ST_mtype(preg_sym);
	FmtAssert(type != MTYPE_UNKNOWN,("PLOC mtype unknown"));

	wn = WN_Ldid (type, PLOC_reg(ploc), preg_sym, ST_type(preg_sym));
	/*
	*  Get the symbol address for the vararg register.
	*/
	Get_Vararg_Ploc_Symbol(ploc, &st, &offset);
	TY_IDX vararg_type = Copy_TY(ST_type(preg_sym));
	Set_TY_no_ansi_alias (vararg_type);
	var_addr = WN_Lda (Pointer_type, 0, st);
	wn = WN_Istore(type,
		       offset,
		       Make_Pointer_Type(vararg_type),
		       var_addr,
		       wn);
	WN_Set_Linenum (wn, current_srcpos);
	WN_INSERT_BlockLast(block, wn);
	ploc = Get_Vararg_Input_Parameter_Location (ploc);
      }
    }
#else /* !TARG_ST */

    if (TY_is_varargs(Ty_Table[PU_prototype(Pu_Table[ST_pu(WN_st(tree))])]))
    {
     /*
      *  For varargs, the func-entry just has the list of fixed
      *  parameters, so also have to store the vararg registers. 
      */
      TYPE_ID	type = Def_Int_Mtype;

#ifdef TARG_X8664
      INT num_vararg_xmms = 0;
      ST *xmm_save_temp_st;

      // x86-64 always needs to call Get_Vararg_Parameter_Location
      if( Is_Target_64bit() ){
        ploc = Get_Vararg_Input_Parameter_Location (ploc);

      } else
#endif
	if (PLOC_is_nonempty(ploc) && !PLOC_on_stack(ploc)) {
	  /* don't do if already reached stack params */
	  ploc = Get_Vararg_Input_Parameter_Location (ploc);
	}

      while (!PLOC_on_stack(ploc))
      {
       /*
        *  vararg registers must be integer registers
        */
	WN	*wn;
	ST	*st;

#ifdef TARG_X8664
	/* bug 14869: Disable generation of SAVE_XMMS if -mno-sse
	   is used */
	if (Preg_Offset_Is_Float(PLOC_reg(ploc))) {
	  if (Is_Target_SSE()) {
	    if (num_vararg_xmms == 0) 
	      xmm_save_temp_st = Get_Vararg_Symbol (ploc);
	    num_vararg_xmms++;
	  }

	  ploc = Get_Vararg_Input_Parameter_Location (ploc);
	  continue;
	}
#endif

	wn = WN_Ldid (type, PLOC_reg(ploc), Int_Preg, ST_type(Int_Preg));
       /*
	*  get the symbol for the vararg formal
	*/
	st = Get_Vararg_Symbol (ploc);

	wn = WN_Stid (type, 0, st, ST_type(st), wn);
	WN_Set_Linenum (wn, current_srcpos);
	WN_INSERT_BlockLast(block, wn);

	ploc = Get_Vararg_Input_Parameter_Location (ploc);
      }

#ifdef TARG_X8664
      if (num_vararg_xmms) {
	WN *ikids[3];
	WN *wn;
	wn = WN_Ldid(MTYPE_I4, RAX, Int_Preg, ST_type(Int_Preg));
	ikids[0] = WN_CreateParm(MTYPE_I4, wn, MTYPE_To_TY(MTYPE_I4),
				 WN_PARM_BY_VALUE);
	wn = WN_Intconst(MTYPE_I4, num_vararg_xmms);
	ikids[1] = WN_CreateParm(MTYPE_I4, wn, MTYPE_To_TY(MTYPE_I4),
				 WN_PARM_BY_VALUE);
	wn = WN_Lda(Pointer_type, 0, xmm_save_temp_st);
	ikids[2] = WN_CreateParm(Pointer_type, wn, MTYPE_To_TY(Pointer_type),
				 WN_PARM_BY_VALUE);
	wn = WN_Create(OPR_INTRINSIC_CALL, MTYPE_V, MTYPE_V, 3);
	WN_intrinsic(wn) = INTRN_SAVE_XMMS;
	WN_kid0(wn) = ikids[0];
	WN_kid1(wn) = ikids[1];
	WN_kid2(wn) = ikids[2];
	WN_Set_Linenum (wn, current_srcpos);
	WN_INSERT_BlockLast(block, wn);
      }
#endif
    }
#endif /* !TARG_ST */

   /*
    * add initialization code for trapuv
    */
    {
      WN *trapuvBlock = WN_CreateBlock();

      trapuvBlock = lower_trapuv(trapuvBlock, tree, actions);

      trapuvBlock= lower_block(trapuvBlock, actions);

#ifdef TARG_ST
      // [TB] Improve line number.
      WN_copy_linenum(tree, trapuvBlock);
#endif

      WN_INSERT_BlockLast(block, trapuvBlock);
    }

#ifdef TARG_X8664
    /*
     * Optimize the malloc algorithm.  For non-x8664 targets, silently ignore
     * the -OPT:malloc_alg option since the driver would have warned already.
     */
    if (OPT_Malloc_Alg >= 1 &&
	(!strcmp(Cur_PU_Name, "main") ||
	 !strcmp(Cur_PU_Name, "MAIN__"))) {
      WN *mallocBlock = WN_CreateBlock();
      mallocBlock = lower_malloc_alg(mallocBlock, tree, actions);
      mallocBlock = lower_block(mallocBlock, actions);
      WN_INSERT_BlockLast(block, mallocBlock);
    }
#endif

    /* Insert a call to a routine inside libhugetlbfs to set heap huge page limit.
     */
    if (OPT_Hugepage_Heap_Set
	&& (!strcmp(Cur_PU_Name, "main") ||
	    !strcmp(Cur_PU_Name, "MAIN__"))) {
      WN *hugepageBlock = WN_CreateBlock();
      hugepageBlock = lower_hugepage_limit(hugepageBlock, tree, actions);
      hugepageBlock = lower_block(hugepageBlock, actions);
      WN_INSERT_BlockLast(block, hugepageBlock);
    }
  }
  else if (Action(LOWER_ENTRY_FORMAL_REF))
  {
    ploc = lower_entry_formals(block, tree, actions);
  }

#if !defined(TARG_IA32) && !defined(TARG_X8664)
  if (Action(LOWER_SPLIT_SYM_ADDRS))
#else
  // We do not do LOWER_SPLIT_SYM_ADDRS action for TARG_X8664 and TARG_IA32
  // but we need to initialize static link register for handling nested
  // PUs. So do this based on the new action LOWER_SLINK_SAVE.
  if (Action(LOWER_SLINK_SAVE))
#endif
  {
    /*
     *  Initialize the static link if required ($2 should have callers $fp)
     */
    ST *slink = Find_Slink_Symbol(CURRENT_SYMTAB);
    if (slink)
    {
       if( Is_Omp_Gen_Slink( slink ))
       {
        //The initialization code has been removed by RVI?
        //So we have to re-inserted it here.
        //TODO: Remove this kind of Redo work.
         WN *ld, *wn;
#if defined(TARG_IA32) || defined(TARG_X8664)
	 if (Is_Target_32bit()) {
	   WN *formal = WN_formal(current_function, 1);// 2nd incoming parameter
	   ld = WN_Ldid(Pointer_type, 0, WN_st(formal), WN_type(formal));
	 }
	 else
#endif
         ld = WN_LdidPreg( Pointer_type, First_Int_Preg_Param_Offset+1 ); //$r33
                                                                                                                                                             
         wn = WN_Stid( Pointer_type, 0, slink, ST_type( slink ), ld );
                                                                                                                                                             
         WN_Set_Linenum( wn, current_srcpos );
         WN_INSERT_BlockFirst( block, wn );
       }
       else
       {
         WN	*ld, *wn;

         ld = WN_LdidPreg(Pointer_type, Static_Link_Preg_Offset);

         wn = WN_Stid (Pointer_type, 0, slink, ST_type(slink), ld);

         WN_Set_Linenum (wn, current_srcpos);
         WN_INSERT_BlockFirst(block, wn);
      }
    }

#ifdef KEY
    if (PU_has_nonlocal_goto_label(Get_Current_PU())) {
      // save the FP
      ST *fpsave_sym = Create_FPSave_Symbol();
      WN *fpld = WN_LdidPreg(Pointer_type, Frame_Pointer_Preg_Offset);
      WN *fpsave = WN_Stid(Pointer_type, 0, fpsave_sym, ST_type(fpsave_sym), 
      			   fpld);
      WN_Set_Linenum (fpsave, current_srcpos);
      WN_INSERT_BlockFirst(block, fpsave);

      // save the SP
      ST *spsave_sym = Create_SPSave_Symbol();
      WN *spld = WN_LdidPreg(Pointer_type, Stack_Pointer_Preg_Offset);
      WN *spsave = WN_Stid(Pointer_type, 0, spsave_sym, ST_type(spsave_sym), 
      			   spld);
      WN_Set_Linenum (spsave, current_srcpos);
      WN_INSERT_BlockFirst(block, spsave);
    }
#endif
  }

  if (WN_opcode(tree) == OPC_FUNC_ENTRY)
  {
    /* Process any PU-scope MP pragmas */
    if (WN_func_pragmas(tree) && Action(LOWER_MP)) {
      WN *wn;
      for (wn = WN_first(WN_func_pragmas(tree)); wn; wn = WN_next(wn)) {
	if (((WN_opcode(wn) == OPC_PRAGMA) || (WN_opcode(wn) == OPC_XPRAGMA))
	    && (WN_pragmas[WN_pragma(wn)].users & PUSER_MP)) {
	  (void) lower_mp(NULL, wn, actions);
	}
      }
    }
    
#ifndef TARG_ST
    if (Call_Mcount && Action(LOWER_ENTRY_EXIT)) {
	WN *profcall = Create_Mcount_Call ();
	if (profcall) {	// possible that we won't create call yet
		WN_Set_Linenum (profcall, current_srcpos);
		WN_INSERT_BlockLast(block, profcall);
	}
    }
#endif
#ifndef TARG_ST
#ifdef KEY
    if (Action(LOWER_RETURN_VAL))
      allocate_trampoline_symbols();
#endif
#endif
    WN_INSERT_BlockFirst(WN_func_body(tree), block);

    /* Lower statements in function body. */
    WN_func_body(tree) = lower_block(WN_func_body(tree), actions);

#ifdef KEY
    if (Action(LOWER_TO_CG)) {
      block = WN_CreateBlock();
      generate_trampoline_code(block);
      WN_INSERT_BlockFirst(WN_func_body(tree), block);
    }
#endif

    return tree;
  }
  else
  {
    block  = lower_block(block, actions);
    WN_INSERT_BlockFirst(block, tree);
  }
  return block;
}


#ifdef KEY
// Figure out if we need anything similar for non-X8664
static WN *lower_landing_pad_entry(WN *tree)
{
  WN * block = WN_CreateBlock();
  WN_INSERT_BlockAfter (block, NULL, tree);
  ST_IDX exc_ptr_param = TCON_uval (INITV_tc_val (INITO_val (PU_misc_info (Get_Current_PU()))));
  ST exc_ptr_st = St_Table[exc_ptr_param];
#ifdef TARG_X8664
  // Store rax into exc_ptr variable
  WN *exc_ptr_rax = WN_LdidPreg (Pointer_Mtype, RAX);
#else
  // Store $4 into exc_ptr variable
  WN *exc_ptr_rax = WN_LdidPreg (Pointer_Mtype, 4);
#endif
  WN *exc_ptr_stid = WN_Stid (Pointer_Mtype, 0, &exc_ptr_st, 
			ST_type(exc_ptr_st), exc_ptr_rax);
  ST_IDX filter_param = TCON_uval (INITV_tc_val (INITV_next (INITO_val (PU_misc_info (Get_Current_PU())))));
  ST filter_st = St_Table[filter_param];
#ifdef TARG_X8664
  // Store rdx into filter variable
  WN *filter_rdx = WN_LdidPreg (Is_Target_64bit() ? MTYPE_U8 : MTYPE_U4, RDX);
#else
  // Store $5 into filter variable
  WN *filter_rdx = WN_LdidPreg (Is_Target_64bit() ? MTYPE_U8 : MTYPE_U4, 5);
#endif
  WN *filter_stid = WN_Stid (Is_Target_64bit() ? MTYPE_U8 : MTYPE_U4, 0, &filter_st, 
			ST_type(filter_st), filter_rdx);

  WN_Set_Linenum(exc_ptr_stid, current_srcpos);  // Bug 1268
  WN_INSERT_BlockLast (block, exc_ptr_stid);
  WN_Set_Linenum(filter_stid, current_srcpos);  // Bug 1268
  WN_INSERT_BlockLast (block, filter_stid);
  return block;
}
#endif // KEY


/* ====================================================================
 *
 * WN *lower_region(WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering on REGION node
 * <tree> returning lowered REGION tree.
 *
 * ==================================================================== */

static WN *lower_region(WN *tree, LOWER_ACTIONS actions)
{
  Is_True(WN_opcode(WN_region_body(tree)) == OPC_BLOCK,
	  ("kid of REGION should be OPC_BLOCK, not %s",
	   OPCODE_name(WN_opcode(WN_region_body(tree)))));

  setCurrentState(tree, actions);
  if (current_function == NULL) {
    // haven't seen FUNC_ENTRY yet
    current_function = PU_Info_tree_ptr(Current_PU_Info);
  }

  if (Action(LOWER_REGION))
  {
    RID *rid = REGION_get_rid( tree );

    Is_True(rid, ("expected valid region id"));

    max_region++;

   /*
    *  first time thru. Set region lowered flags
    *  otherwise remove flags that have already been processed.
    */
    if (RID_lowered(rid) == (LOWER_ACTIONS) LOWER_NULL)
    {
      RID_lowered(rid) = actions;
    }
    else
    {
      actions ^= (RID_lowered(rid) & (LOWER_CALL | LOWER_ENTRY_EXIT));
    }

    if (actions)
    {
      WN_region_body(tree) = lower_block(WN_region_body(tree), 
					 actions | LOWER_REGION);
    }
  }
  else
  {
    WN_region_body(tree) = lower_block(WN_region_body(tree), actions);
  }

  return tree;
}

WN *lower_scf_non_recursive(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  return lower_scf(block,tree,actions | LOWER_TOP_LEVEL_ONLY);
}

/* ====================================================================
 *
 * WN *lower_scf(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on structured control
 * flow node <tree>.  Returned tree will always have a structured
 * control flow node (at least a BLOCK) at the top.
 *
 * ==================================================================== */
#ifdef TARG_ST
WN *lower_scf(WN *block, WN *tree, LOWER_ACTIONS actions)
#else
static WN *lower_scf(WN *block, WN *tree, LOWER_ACTIONS actions)
#endif
{
  CURRENT_STATE scfState = pushCurrentState(tree, actions);

  switch (WN_opcode(tree))
  {
  case OPC_DO_WHILE:
    block = lower_do_while(block, tree, actions);
    break;

  case OPC_WHILE_DO:
    block = lower_while_do(block, tree, actions);
    break;

  case OPC_DO_LOOP:
    block = lower_do_loop(block, tree, actions);
    break;
      
  case OPC_IF:
    block = lower_if(block, tree, actions);
    break;

  case OPC_BLOCK:
    block = lower_block(tree, actions);
    break;
    
  case OPC_REGION:
    block = lower_region(tree, actions);
    break;
  }

  popCurrentState(scfState);
  return block;
}


/* ====================================================================
 *
 * WN *lower_trapuv(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Initialize stack variables as per trapuv
 *
 * ==================================================================== */

static WN *lower_trapuv(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  if ( DEBUG_Trap_Uv == FALSE
#ifdef KEY
       && DEBUG_Zero_Uv == FALSE
#endif
     )
    return block;

  {
    ST	*st;
    ST  *slink = Find_Slink_Symbol(CURRENT_SYMTAB);

    INT32 i;
    FOREACH_SYMBOL(CURRENT_SYMTAB, st, i)
    {
      TY_IDX  type;
      TYPE_ID btype;
      INT32   size;

      if (ST_class(st) != CLASS_VAR)
	continue;

      if (ST_sclass(st) != SCLASS_AUTO)
	continue;

      if (Has_Base_Block(st)) {
#ifdef KEY
	// -apo changes array A to the block A.BLOCK.  Bug 8303.
	char *name = ST_name(st);
	char *basename = ST_name(ST_base(st));
	char *buf = (char *) alloca(strlen(name) + 10);
	strcpy(buf, name);
	strcat(buf + strlen(name), ".BLOCK");
	if (strcmp(basename, buf))	// Continue if base is not "A.BLOCK".
#endif
	continue;
      }

      if (ST_is_uplevelTemp(st) || st == slink)
	continue;

      if (ST_is_not_used(st)) 
	continue;
      if (Is_Allocated(st))
	continue;

      type  = ST_type(st);
      btype = TY_mtype(Ty_Table[type]);
      size  = TY_size(type);

      Is_True(ST_pu_defined(st), ("trapuv auto or temp not ST_pu_defined"));
      
#ifdef TARG_ST
      // [TTh] When neither Trap_Uv nor Zero_Uv, a KILL initialization is introduce
      //       only for unaliased locals with scalar, pointer and function types.
      if (!DEBUG_Zero_Uv                &&
	  !DEBUG_Trap_Uv                &&
	  (ST_addr_saved(st)    ||
	   (TY_kind(type) != KIND_SCALAR  &&
	    TY_kind(type) != KIND_POINTER &&
	    TY_kind(type) != KIND_FUNCTION)
	   )
	  ) {
	continue;
      }
#endif

      switch(TY_kind(type))
      {
      case KIND_SCALAR:
      case KIND_POINTER:
      case KIND_FUNCTION:
        {
	  WN  *con, *stid;

	  Is_True((MTYPE_RegisterSize(btype) == size),
		  ("bad size for scalar/pointer"));;
#ifdef TARG_ST
	  // [TTh] By default, add pseudo KILL initialization
	  if (!DEBUG_Zero_Uv && !DEBUG_Trap_Uv) {
	    TYPE_ID ret_mtype = btype;
	    if (MTYPE_byte_size(ret_mtype) < MTYPE_byte_size(Max_Int_Mtype) &&
		(!MTYPE_is_dynamic(ret_mtype))) {
	      // Note: INTRINSIC_OP rtype cannot be < I4/U4, except for extension type.
	      ret_mtype = MTYPE_signed(ret_mtype) ? Max_Int_Mtype : Max_Uint_Mtype;
	    }
	    // Each KILL instance has a unique id parameter, to insure they are
	    // not factorize, except with copy of themselves. This is required to:
	    // (1) Avoid useless move introduce after factorization
	    //         KILL a ; KILL b     -> no code generated
	    //     but KILL a ; move a->b  -> 1 useless move generated
	    // (2) Avoid factorization of instance returning different MTYPE,
	    //     that will lead to inconsistant IR afterwards
	    static INT kill_id = 0;
	    con = WN_Intconst(MTYPE_I4, kill_id++);
	    con = WN_CreateParm(MTYPE_I4, con, MTYPE_To_TY(MTYPE_I4), WN_PARM_BY_VALUE);
	    WN *param[1] = {con};
	    con = WN_Create_Intrinsic(OPR_INTRINSIC_OP, ret_mtype, MTYPE_V, INTRN_KILL, 1, param);
	  }
	  // TB Add -zerouv option from PathScale 2.1
	  else if (DEBUG_Zero_Uv) {
	    con = MTYPE_is_integral(btype) ?
	      WN_Intconst(Mtype_comparison(btype), 0) : WN_Floatconst(btype, 0.0);
	  } else if (TY_kind(type) == KIND_POINTER) {
	    con = WN_UVConstPointer(btype);
	  } else
#else

#ifdef KEY
	  if (DEBUG_Zero_Uv) {
	    if (MTYPE_is_vector(btype)) {
	      con = Make_Const(Create_Simd_Const(btype,
						 Host_To_Targ(MTYPE_I4, 0)));
	    } else {
	      if (MTYPE_is_integral(btype)) {
		con = WN_Intconst(size < 4 ?
				    Mtype_TransferSign(btype, MTYPE_I4) : btype,
				  0);
	      } else {
		con = WN_Floatconst(btype, 0.0);
	      }
	    }
	  } else
#endif
#endif
	  con = WN_UVConst(btype);

#ifdef KEY
	  // Load the constant using integer registers instead of floating
	  // point registers, since storing NaN from x87 registers will trap.
	  if (btype == MTYPE_FQ || btype == MTYPE_F16) {
	    TYPE_ID ty = MTYPE_U8;
	    TCON c = ST_tcon_val(WN_st(con));
	    WN *intconst = WN_Intconst(ty, TCON_k0(c));
	    stid = WN_Stid(ty, 0, st, type, intconst);
#ifdef TARG_MIPS
	    WN_Set_Linenum(stid, WN_Get_Linenum(tree));
	    WN_INSERT_BlockLast(block, stid);
	    intconst = WN_Intconst(ty, TCON_k0(c));
	    stid = WN_Stid(ty, 8, st, type, intconst);
#endif
	    // TODO: 4 more bytes for TARG_X8664?
	  }
	  else if (!MTYPE_is_integral(btype) && !MTYPE_is_vector(btype)) {
	    Is_True(size == 4 || size == 8,
		    ("lower_trapuv: unexpected type size"));
	    TYPE_ID ty = (size == 8) ? MTYPE_U8 : MTYPE_U4;
	    TCON c = ST_tcon_val(WN_st(con));
	    WN *intconst = WN_Intconst(ty, TCON_k0(c));
	    stid = WN_Stid(ty, 0, st, type, intconst);
	  }
	  else
#endif
	  stid = WN_Stid(btype, 0, st, type, con);
	  WN_Set_Linenum(stid, WN_Get_Linenum(tree));
	  WN_INSERT_BlockLast(block, stid);
        }
	break;

      case KIND_ARRAY:
      case KIND_STRUCT:
	{
	 /*
	  *  Assign bit pattern just on basis of size.
	  *  We cannot truncate the pattern, so we set ST_use_reg_align();
	  *  We always start at offset 0 (we know it is aligned)
	  *  and replicate the following pattern  (0xFFFA5A5A)
	  *
	  *	size	right-justify		left-justify
	  *	----	-------------		------------
	  *	1	5A			FF
	  *	2	5A5A			FFFA
	  *	3	FA5A5A			FFFA5A
	  *	4	FFFA5A5A		FFFA5A5A
	  *	5	5AFFFA5A 5A		FFFA5A5A FF
	  *	6	5A5AFFFA 5A5A		FFFA5A5A FFFA
	  *	7	FA5A5AFF FA5A5A		FFFA5A5A FFFA5A
	  *
	  *  We do the assignment in chunks, special casing the
	  *  last assigment.
	  *  These weird patterns are only relevant for character
	  *  and structure assignment
	  */
	  TY_IDX	ptype = Make_Pointer_Type(type);
	  WN           *conUV4 = NULL;
	  INT32		todo = size;
	  INT32		offset = 0;
	  UINT32        ncon;
	  TYPE_ID       q;

	  for (q= Max_Uint_Mtype; q!=MTYPE_UNKNOWN; q= Mtype_prev_alignment(q))
	  {
	    WN	   *con, *lda, *num, *store;
	    INT32  qSize   = MTYPE_RegisterSize(q);
	    INT32  qBits   = MTYPE_size_reg(q);
	    INT32  nMoves  = todo / qSize;
	    INT32  residue = todo % qSize;

	    if (q >= MTYPE_U4)
	    { 
	      if (nMoves)
	      {
#ifdef KEY
		if (DEBUG_Zero_Uv) {
		  con = WN_Intconst(q, 0);
		} else if (TY_kind(type) == KIND_ARRAY) {
		  // Get the appropriate 32/64-bit NaN from the type.
		  TY_IDX etype_idx = Ty_Table[type].Etype();
		  if (TY_mtype(etype_idx) == MTYPE_F8 ||
		      TY_mtype(etype_idx) == MTYPE_C8) {
		    con = WN_UVConst(MTYPE_U8);
		  } else {
		    con = WN_UVConst(MTYPE_U4);
		    // Replicate 32-bit value to create 64-bit SIMD version,
		    // since we are storing 64 bits at a time.
		    Is_True(WN_operator(con) == OPR_INTCONST,
			    ("lower_trapuv: not an INTCONST WN"));
		    if (q > MTYPE_U4) {
		      UINT64 val = WN_const_val(con);
		      con = WN_Intconst(q, (val << 32) | val );
		    }
		  }
		} else
#endif
		con = WN_UVConst(q);

  		if ( DEBUG_Trap_Uv_Rjustify )
  		{
		  con = WN_RotateIntconst(con, residue*8);
  		}

		lda = WN_Lda(Pointer_type, 0, st);
		num = WN_Intconst(MTYPE_I4, nMoves * qSize);

#ifdef KEY
		// The number of bytes stored may or may not be a multiple of
		// the struct size.  To prevent the WN verifier from
		// complaining, store through a byte pointer.  Bug 12676.
		TY_IDX byte_ptype = Make_Pointer_Type(MTYPE_To_TY(MTYPE_U1));
		store = WN_CreateMstore(offset, byte_ptype, con, lda, num);
#else
		store = WN_CreateMstore(offset, ptype, con, lda, num);
#endif

		WN_Set_Linenum(store, WN_Get_Linenum(tree));
		WN_INSERT_BlockLast(block, store);

		todo   -= (nMoves * qSize);
		offset += (nMoves * qSize);
	      }
	    }
	    else
	    {
	     /*
	      *  very tricky residue code (size 1,2,3).
	      */
	      if (todo > 0)
	      {
		if (conUV4==NULL)
		{
#ifdef KEY
		  if (DEBUG_Zero_Uv)
		    conUV4 = WN_Intconst(MTYPE_U4, 0);
		  else
#endif
 		  conUV4 = WN_UVConst(MTYPE_U4);
		  ncon   = WN_const_val(conUV4);
		}
		if (nMoves)
		{
		  if ( DEBUG_Trap_Uv_Rjustify )
		  {
		    con = WN_Intconst(MTYPE_U4, ncon>>(todo*8 - qBits));
		  }
		  else
		  {
		    con = WN_Intconst(MTYPE_U4,
				      ncon>>(MTYPE_size_reg(MTYPE_U4)-qBits));
		    ncon <<= qBits;
		  }

		  store = WN_Stid(q, offset, st, type, con);
		  WN_Set_Linenum(store, WN_Get_Linenum(tree));
		  WN_INSERT_BlockLast(block, store);
		
		  todo   -= (nMoves * qSize);
		  offset += (nMoves * qSize);
		}
	      }
	    }
	  }
	  if (conUV4)
	    WN_Delete(conUV4);

	}
	break;
      }
    }
  }
  return block;
}

#ifdef KEY
/* ====================================================================
 *
 * WN *lower_malloc_alg(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Insert code to optimize malloc.
 *
 * ==================================================================== */
static WN *
lower_malloc_alg(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  WN *call;
  TY_IDX ty = Make_Function_Type(MTYPE_To_TY(MTYPE_V));
  ST *st = Gen_Intrinsic_Function(ty, "__pathscale_malloc_alg");

  Set_PU_no_side_effects(Pu_Table[ST_pu(st)]);
  Set_PU_is_pure(Pu_Table[ST_pu(st)]);

  call = WN_Call(MTYPE_V, MTYPE_V, 1, st);	// bug 10736

  switch(OPT_Malloc_Alg) { //extend malloc_alg
   case 1:
      WN_kid0(call) = WN_CreateParm(MTYPE_I4, WN_Intconst(MTYPE_I4, 1),
                                MTYPE_To_TY(MTYPE_I4), WN_PARM_BY_VALUE);
      break;
   case 2:
      WN_kid0(call) = WN_CreateParm(MTYPE_I4, WN_Intconst(MTYPE_I4, 2),
                                MTYPE_To_TY(MTYPE_I4), WN_PARM_BY_VALUE);
      break;
   case 3:
      WN_kid0(call) = WN_CreateParm(MTYPE_I4, WN_Intconst(MTYPE_I4, 3),
                                MTYPE_To_TY(MTYPE_I4), WN_PARM_BY_VALUE);
      break;
   default:
      FmtAssert(0, ("Lower_Malloc_Alg: -OPT:malloc_alg should be less than or equal to 3!"));
      break;
  }

  WN_Set_Linenum(call, current_srcpos);
  call = lower_call(block, call, actions);
  WN_INSERT_BlockLast(block, call);

 return block;
}
#endif

/* ====================================================================
 *
 * WN *lower_hugepage_limit(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Insert code to set huge page heap limit.
 *
 * ==================================================================== */

static WN *
lower_hugepage_limit(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  WN * call;
  TY_IDX ty = Make_Function_Type(MTYPE_To_TY(MTYPE_V));
  ST *st = Gen_Intrinsic_Function(ty, "__setup_hugepage");
  Set_PU_no_side_effects(Pu_Table[ST_pu(st)]);
  Set_PU_is_pure(Pu_Table[ST_pu(st)]);

  call = WN_Call(MTYPE_V, MTYPE_V, 2, st);	// bug 10736
  
  WN_kid0(call) = WN_CreateParm(MTYPE_I4, WN_Intconst(MTYPE_I4, OPT_Hugepage_Heap_Limit),
				MTYPE_To_TY(MTYPE_I4), WN_PARM_BY_VALUE);
  WN_kid1(call) = WN_CreateParm(MTYPE_I4, WN_Intconst(MTYPE_I4, OPT_Hugepage_Attr),
				MTYPE_To_TY(MTYPE_I4), WN_PARM_BY_VALUE);
  WN_Set_Linenum(call, current_srcpos);
  call = lower_call(block, call, actions);
  WN_INSERT_BlockLast(block, call);

  return block;
}

static void lower_actions_fprintf(FILE *f, LOWER_ACTIONS actions)
{
  LOWER_ACTIONS i = 1;

  while (actions)
  {
    if (Action(i))
    {
      fprintf(f, "%s ", LOWER_ACTIONS_name(i));
      actions = actions ^ i;
    }
    i <<= 1;
  }
  fprintf(f, "\n");
}


/* ====================================================================
 *
 * const char * LOWER_ACTIONS_name(LOWER_ACTIONS actions)
 *
 * Exported.  See interface description in "wn_lower.h".
 *
 * ==================================================================== */
const char * LOWER_ACTIONS_name(LOWER_ACTIONS actions)
{
  if ((actions-1) & actions)
  {
    DevWarn("LOWER_ACTION_name(0x%" SCNx64 "): expected only one flag at a time",
	    actions);
  }

  switch(actions)
  {
  case LOWER_NULL:			return "LOWER_NULL";
  case LOWER_DO_LOOP:			return "LOWER_DO_LOOP";
  case LOWER_DO_WHILE:			return "LOWER_DO_WHILE";
  case LOWER_WHILE_DO:			return "LOWER_WHILE_DO";
  case LOWER_IF:			return "LOWER_IF";
  case LOWER_COMPLEX:			return "LOWER_COMPLEX";
  case LOWER_ARRAY:			return "LOWER_ARRAY";
  case LOWER_SPLIT_CONST_OFFSETS:	return "LOWER_SPLIT_CONST_OFFSETS";
  case LOWER_ENTRY_EXIT:		return "LOWER_ENTRY_EXIT";
  case LOWER_CALL:			return "LOWER_CALL";
  case LOWER_SPLIT_SYM_ADDRS:		return "LOWER_SPLIT_SYM_ADDRS";
  case LOWER_IO_STATEMENT:		return "LOWER_IO_STATEMENT";
  case LOWER_MSTORE:			return "LOWER_MSTORE";
  case LOWER_CVT:			return "LOWER_CVT";
  case LOWER_MP:			return "LOWER_MP";
  case LOWER_8X_ARRAY:			return "LOWER_8X_ARRAY";
  case LOWER_INTRINSIC:			return "LOWER_INTRINSIC";
  case LOWER_INLINE_INTRINSIC:		return "LOWER_INLINE_INTRINSIC";
  case LOWER_INL_STACK_INTRINSIC:	return "LOWER_INL_STACK_INTRINSIC";
  case LOWER_REGION:			return "LOWER_REGION";
  case LOWER_QUAD:			return "LOWER_QUAD";
  case LOWER_COMPGOTO:			return "LOWER_COMPGOTO";
  case LOWER_MADD:			return "LOWER_MADD";
  case LOWER_TOP_LEVEL_ONLY:		return "LOWER_TOP_LEVEL_ONLY";
  case LOWER_PREFETCH_MAPS:		return "LOWER_PREFETCH_MAPS";
  case LOWER_ALIAS_MAPS:		return "LOWER_ALIAS_MAPS";
  case LOWER_DEPGRAPH_MAPS:		return "LOWER_DEPGRAPH_MAPS";
  case LOWER_PARITY_MAPS:		return "LOWER_PARITY_MAPS";
    // NOTE: Delete LOWER_FREQUENCY_MAPS
  case LOWER_FREQUENCY_MAPS:		return "LOWER_FREQUENCY_MAPS";
  case LOWER_PICCALL:			return "LOWER_PICCALL";
  case LOWER_BASE_INDEX:		return "LOWER_BASE_INDEX";
  case LOWER_TO_CG:			return "LOWER_TO_CG";
  case LOWER_ASSERT:			return "LOWER_ASSERT";
  case LOWER_FORMAL_REF:		return "LOWER_FORMAL_REF";
  case LOWER_UPLEVEL:			return "LOWER_UPLEVEL";
  case LOWER_ENTRY_FORMAL_REF:		return "LOWER_ENTRY_FORMAL_REF";
  case LOWER_SHORTCIRCUIT:		return "LOWER_SHORTCIRCUIT";
  case LOWER_TREEHEIGHT:		return "LOWER_TREEHEIGHT";
  case LOWER_RETURN_VAL:		return "LOWER_RETURN_VAL";
  case LOWER_MLDID_MSTID:		return "LOWER_MLDID_MSTID";
  case LOWER_BIT_FIELD_ID:		return "LOWER_BIT_FIELD_ID";
  case LOWER_BITS_OP:			return "LOWER_BITS_OP";
#ifndef TARG_ST
  case LOWER_TO_MEMLIB:                 return "LOWER_TO_MEMLIB";
#endif
#ifdef TARG_ST
  case LOWER_DOUBLE:			return "LOWER_DOUBLE";
  case LOWER_LONGLONG:			return "LOWER_LONGLONG";
  case LOWER_ENTRY_PROMOTED:		return "LOWER_ENTRY_PROMOTED";
  case LOWER_NESTED_FN_PTRS:            return "LOWER_NESTED_FN_PTRS";
  case LOWER_INSTRUMENT:            	return "LOWER_INSTRUMENT";
  case LOWER_CNST_DIV:			return "LOWER_CNST_DIV";
  case LOWER_FORMAL_HIDDEN_REF:		return "LOWER_FORMAL_HIDDEN_REF";
  case LOWER_SELECT:			return "LOWER_SELECT";
  case LOWER_FAST_DIV:			return "LOWER_FAST_DIV";
  case LOWER_FAST_MUL:			return "LOWER_FAST_MUL";
  case LOWER_CNST_MUL:			return "LOWER_CNST_MUL";
  case LOWER_RUNTIME:			return "LOWER_RUNTIME";
#endif

  default:				return "<unrecognized>";
  }
}

static const char * MSTORE_ACTIONS_name(MSTORE_ACTIONS actions)
{
  switch(actions)
  {
  case MSTORE_aggregate:		return "scalar moves    ";
  case MSTORE_loop:			return "generate loop   ";
  case MSTORE_intrinsic_bzero:		return "intrinsic bzero ";
  case MSTORE_intrinsic_memset:		return "intrinsic memset";
  case MSTORE_intrinsic_memcpy:		return "intrinsic memcpy";
  default:				return "<unrecognized>";
  }
}


void WN_Lower_Checkdump(const char *msg, WN *tree, LOWER_ACTIONS actions)
{
  traceAlignment   = Get_Trace(TP_LOWER, 0x004);
  traceSplitSymOff = Get_Trace(TP_LOWER, 0x010);
  traceIO          = Get_Trace(TP_LOWER, 0x020);
  traceSpeculate   = Get_Trace(TP_LOWER, 0x040);
  traceTreeHeight  = Get_Trace(TP_LOWER, 0x080);
  traceMload       = Get_Trace(TP_LOWER, 0x100);
  // traceUplevel    = Get_Trace(TP_LOWER, 0x200);

  if (Get_Trace(TP_LOWER, 0x008))
  {
    enable_tree_freq_display();
  }

  if (Get_Trace(TKIND_IR, TP_LOWER))
  {
    fputs(DBar, TFile);
    fprintf(TFile, "WN_Lower: \"%s\"\n", msg);
    if (actions)
    {
      fprintf(TFile, "flags are:\n");
      lower_actions_fprintf(TFile, actions);
    }
    fdump_tree(TFile, tree);
    fputs(DBar, TFile);
  }

  if (Get_Trace(TP_LOWER, 0x001))
  {
    IR_dump_map_info = TRUE;

    fprintf(TFile, "WN_Lower: LNO DEP GRAPH\n");
    LNOPrintDepGraph(TFile);
  }
  if (Get_Trace(TP_LOWER, 0x002))
  {
    IR_dump_map_info = TRUE;

    fprintf(TFile, "WN_Lower: WOPT ALIAS INFO\n");
    fdump_dep_tree(TFile, tree, alias_manager);
  }

  if (Get_Trace(TKIND_SYMTAB,TP_LOWER)) {
    fprintf(TFile,"\n\n========== Symbol tables after Lowering ==========\n");
    Print_symtab (TFile, GLOBAL_SYMTAB);
    Print_symtab (TFile, CURRENT_SYMTAB);
  }

  /*
   * these options can lead to infinite regress
   */
  if (Action(LOWER_SPLIT_SYM_ADDRS))
  {
    if (Enable_WN_Simp && WN_Simp_Fold_ILOAD)
    {
      DevWarn("disabling option WN_Simp_Fold_ILOAD"
	      " while lowering action LOWER_SPLIT_SYM_ADDRS");
      WN_Simp_Fold_ILOAD = FALSE;
    }
  }
  /*
   * get any relevant pragmas
   * I need to know if wopt was run to generate some eval warnings
   */
  {
    WN	*pragma=	NULL;
	  
    traceWoptFinishedOpt=	FALSE;

    switch(WN_operator(tree))
    {
    case OPR_FUNC_ENTRY:
      pragma=	WN_func_pragmas(tree);
      break;
    case OPR_REGION:
      pragma=	WN_region_pragmas(tree);
      break;
    }
    if (pragma)
    {
      WN	*wn;
      for(wn= WN_first(pragma); wn; wn= WN_next(wn))
      {
	if (WN_pragma(wn) == WN_PRAGMA_WOPT_FINISHED_OPT)
	  traceWoptFinishedOpt=	TRUE;
      }
    }
  }
}


/* ====================================================================
 *
 * LOWER_ACTIONS lower_to_cg(LOWER_ACTIONS, LOWER_ACTIONS)
 *
 * The last lowering is special in that it may require lowering
 * already specified and lowering that has been avoided
 * 
 * Some lowering should not be done more than once (ENTRY_EXIT, CALL)
 *
 * Keep track and add/subtract any lowering already processed
 *
 * ==================================================================== */

static LOWER_ACTIONS lower_actions(WN *pu, LOWER_ACTIONS actions)
{
  if (OPT_Lower_Treeheight && Action(LOWER_TO_CG))
    actions |= LOWER_TREEHEIGHT;

  if (Action(LOWER_TO_CG))
  {
   /*
    *  remove/add one-time-only and must-apply flags
    */
    actions |=  lowering_actions ^ (LOWER_ENTRY_EXIT		|
				    LOWER_ENTRY_FORMAL_REF	|
				    LOWER_FORMAL_REF		|
				    LOWER_UPLEVEL		|
#if !defined(TARG_IA32) && !defined(TARG_X8664)
				    LOWER_SPLIT_SYM_ADDRS	|
#else
				    LOWER_SLINK_SAVE		|
#endif
#ifdef TARG_MIPS // need LOWER_SPLIT_CONST_OFFSETS for correctness in -O0 compiles
				    LOWER_SPLIT_CONST_OFFSETS	|
#endif
				    LOWER_CALL			|
				    LOWER_MLDID_MSTID		|
#ifdef TARG_ST
				    LOWER_ENTRY_PROMOTED	|
				    LOWER_NESTED_FN_PTRS        |
				    LOWER_INSTRUMENT        	|
#endif
				    LOWER_RETURN_VAL);

#ifdef TARG_MIPS // need LOWER_SPLIT_CONST_OFFSETS for correctness in -O0 compiles
    actions &=  ~LOWER_BASE_INDEX;
#else
   /*
    * remove these always
    *
    * disabling SPLIT_CONST will allow the lowerer to put together addresses
    * that have been pulled apart
    */
    actions &=  ~(LOWER_BASE_INDEX | LOWER_SPLIT_CONST_OFFSETS);
#endif

   /*
    * these form the major LOWER_TO_CG actions
    */
    actions |=	
#ifdef TARG_ST
      		LOWER_SELECT		  |
#endif
                LOWER_SCF		  |
		LOWER_ARRAY		  |
		LOWER_MP		  |
		LOWER_IO_STATEMENT	  |
		LOWER_MSTORE		  |
		LOWER_CVT		  |
		LOWER_COMPGOTO		  |
		LOWER_COMPLEX		  |
#ifndef TARG_X8664
		LOWER_QUAD		  |
#endif
		LOWER_MADD		  |
		LOWER_INTRINSIC		  |
		LOWER_ASSERT		  |
		LOWER_PICCALL		  |
		LOWER_ALL_MAPS		  |
		LOWER_SHORTCIRCUIT	  |
		LOWER_INL_STACK_INTRINSIC |
		LOWER_INLINE_INTRINSIC	  |
#ifdef TARG_ST
	        LOWER_BITS_OP |	/* Force lowering of bit ops before CG. */
      		LOWER_RUNTIME | /* Force lowering of runtime before CG. */
#endif
		LOWER_BIT_FIELD_ID;
   /*
    *  do not split divides into mul/recip at the CG lowering
    */
    save_Div_Split_Allowed = Div_Split_Allowed;
    // KEY: bug 1449: Don't comment out the following line
    Div_Split_Allowed = FALSE;;
  }
  if (WN_opcode(pu) == OPC_FUNC_ENTRY)
  {
    lowering_actions |= actions;
  }

  if (Action(LOWER_BITS_OP))
    actions |= LOWER_BIT_FIELD_ID;
  
#ifdef TARG_ST
  /* LOWER_HILO implies LOWER_RUNTIME as:
     - LOWER_HILO is called after the CG and the runtime lowering
     - LOWER_RUNTIME should be active as soon as any transformation is done. */
  if (Action(LOWER_HILO)) {
    actions |= LOWER_RUNTIME;
  }
#endif

  current_actions = actions;

  lower_maps_init(actions);

  setCurrentState(pu, actions);

#ifdef TARG_ST
  /* [CG] If LOWER_TREEHEIGHT was already run previously,
     we disable WN_simplification as it may undo LOWER_TREEHEIGHT.
  */
  if ((lowering_actions & LOWER_TREEHEIGHT) &&
      !(actions & LOWER_TREEHEIGHT)) {
    /* LOWER_TREEHEIGTH was already run. */
    if (OPT_Lower_Treeheight) {
      wn_lower_no_wn_simplify = TRUE;
    }
  } else {
    wn_lower_no_wn_simplify = FALSE;
  }
#endif

#ifdef TARG_ST
  /* [CG] We add some check here for PU level lowering. 
   * Note that actions contains the current actions and lowering_actions
   * all actions performed up to now including the current actions.
   */
  if (WN_opcode(pu) == OPC_FUNC_ENTRY) {
    /* Ensure that LOWER_CALL is not done strictly before LOWER_INSTRUMENT.
     * Instrumentation must be done before the call parameters are lowered.
     */
    if (actions & LOWER_CALL) {
      DevAssert(lowering_actions & LOWER_INSTRUMENT, 
		("LOWER_CALL done before LOWER_INSTRUMENT"));
    }

    /* Ensure that LOWER_INSTRUMENT is done strictly before 
     * LOWER_NESTED_FN_PTRS.
     * Instrumentation may generated load address of nested function
     * pointer, that must not be processed by LOWER_NESTED_FN_PTRS.
     */
    if (actions & LOWER_INSTRUMENT) {
      DevAssert(lowering_actions & LOWER_NESTED_FN_PTRS, 
		("LOWER_INSTRUMENT done before LOWER_NESTED_FN_PTRS"));
    }
  }
#endif

  return actions;
}




/* ====================================================================
 *
 * Lower_Init(void)
 *
 * lowering specific initialization
 * ==================================================================== */

void Lower_Init(void)
{
   static CURRENT_STATE current_state_NULL;
   /*
    *  create map for marking parity
    */
   lowering_parity_map = WN_MAP32_Create(MEM_pu_pool_ptr);

   lowering_actions = 0;
   current_state = current_state_NULL;

   // save parity_map in an array becuse it may change in nested PUs
   parity_map_index++;
   FmtAssert(0 <= parity_map_index && parity_map_index < PARITY_MAP_ARRAY_SIZE,
             ("Lower_Init: Index into parity map array is out of range"));
   parity_map_array[parity_map_index] = lowering_parity_map;
#ifdef TARG_ST
   // [CG] Map of mtypes to aliasing types for ansi rules
   memset(TY_alias_array, 0, sizeof(TY_IDX) * (MTYPE_STATIC_LAST + 1));
#endif

}

void Lowering_Finalize(void)
{
  /* free lowering_parity_map */
  WN_MAP_Delete(lowering_parity_map);
  
  parity_map_index--;
  if (parity_map_index >= 0) {
    // if there is a saved parity map, then restore it
    lowering_parity_map = parity_map_array[parity_map_index];
  }
  else {
    // otherwise, set it to undefined
    lowering_parity_map = WN_MAP_UNDEFINED;
  }
}




static void lower_end(WN *tree, LOWER_ACTIONS actions)
{
  lower_maps_reset(actions);

  if (Action(LOWER_TO_CG))
  {
    Div_Split_Allowed = save_Div_Split_Allowed; /* reset */
  }

  popCurrentState(current_state);
}

#ifdef TARG_X8664
/* ===================================================================
 * statement walk the entire PU transforming any C8 type operation to 
 * V16C8 type operation.
 * =================================================================== */

static BOOL test_vcast_complex_types_stmt_expr(WN* tree) 
{
  OPCODE opcode = WN_opcode(tree);
  OPERATOR opr = WN_operator(tree);

  if (OPCODE_is_compare(opcode) ||
      opr == OPR_PAIR ||
      opr == OPR_CONST ||
      (opr == OPR_INTRINSIC_OP &&
       (WN_intrinsic(tree) != INTRN_C8CONJG ||
	!Is_Target_64bit())) ||
      opr == OPR_CVT ||
      opr == OPR_DIV ||
      opr == OPR_SQRT ||
      opr == OPR_FIRSTPART ||
      opr == OPR_SECONDPART ||
      (!OPCODE_is_expression(opcode) && 
       !OPCODE_is_load(opcode) &&
       !OPCODE_is_store(opcode)))
    return FALSE;

  if ((opr == OPR_LDID || opr == OPR_STID) && WN_st(tree) && 
      ST_class(WN_st(tree)) == CLASS_PREG &&
      (WN_rtype(tree) == MTYPE_C8 ||
       WN_desc(tree) == MTYPE_C8))
    return FALSE;    

  for (UINT kidno=0; kidno<WN_kid_count(tree); kidno++)
    if (!test_vcast_complex_types_stmt_expr(WN_kid(tree,kidno)))
      return FALSE;

  return TRUE;
}

static void vcast_complex_types_stmt_expr(WN* tree, WN* parent, INT kid) 
{
  OPCODE opcode = WN_opcode(tree);
  OPERATOR opr = WN_operator(tree);

  for (UINT kidno=0; kidno<WN_kid_count(tree); kidno++)
    vcast_complex_types_stmt_expr(WN_kid(tree,kidno), tree, kidno);

  if (OPCODE_rtype(opcode) == MTYPE_C8)
    WN_set_rtype(tree, MTYPE_V16C8);
  if (OPCODE_desc(opcode) == MTYPE_C8)
    WN_set_desc(tree, MTYPE_V16C8);

  if (opr == OPR_MPY && WN_rtype(tree) == MTYPE_V16C8) {
    WN* kids[3];
    WN* wn;
    if (WN_operator(WN_kid0(tree)) == OPR_LDID) {
      kids[0] = WN_COPY_Tree(WN_kid0(tree));
      kids[1] = WN_CreateExp1(OPC_V16C8V16C8SHUFFLE,
			      WN_COPY_Tree(WN_kid0(tree)));
      kids[2] = WN_kid1(tree);
    } else {
      kids[0] = WN_COPY_Tree(WN_kid1(tree));
      kids[1] = WN_CreateExp1(OPC_V16C8V16C8SHUFFLE,
			      WN_COPY_Tree(WN_kid1(tree)));
      kids[2] = WN_kid0(tree);
    }
    WN_offset(kids[1]) = 0; /* reverse */
    // Create parm nodes for intrinsic op
    kids[0] = WN_CreateParm (MTYPE_V16C8, kids[0], 
			     Be_Type_Tbl(MTYPE_V16C8),
			     WN_PARM_BY_VALUE);
    kids[1] = WN_CreateParm (MTYPE_V16C8, kids[1], 
			     Be_Type_Tbl(MTYPE_V16C8),
			     WN_PARM_BY_VALUE);
    kids[2] = WN_CreateParm (MTYPE_V16C8, kids[2], 
			     Be_Type_Tbl(MTYPE_V16C8),
			     WN_PARM_BY_VALUE);
    wn = WN_Create_Intrinsic(OPCODE_make_op(OPR_INTRINSIC_OP,
					    MTYPE_V16C8, MTYPE_V),
			     INTRN_V16C8MPY_ADDSUB, 3, kids);
    WN_kid(parent, kid) = wn;
  } 

  else if (opr == OPR_INTRINSIC_OP) {
    FmtAssert(WN_intrinsic(tree) == INTRN_C8CONJG, ("NYI"));
    WN_intrinsic(tree) = INTRN_V16C8CONJG;
  }

  return;
}

static void vcast_complex_types(WN* tree) 
{
  OPCODE opcode = WN_opcode(tree);
  OPERATOR opr = WN_operator(tree);
  
  if (opcode == OPC_BLOCK) {
    for (WN* stmt=WN_first(tree); stmt;) {
      WN* next_stmt=WN_next(stmt);
      if (!OPCODE_is_call(WN_opcode(stmt))) {
	if (!OPCODE_is_scf(WN_opcode(stmt))) {
	  if (test_vcast_complex_types_stmt_expr(stmt))
	    vcast_complex_types_stmt_expr(stmt, tree, 0);
	} else
	  vcast_complex_types(stmt);
      }
      stmt=next_stmt;
    }
    return;
  }
  for (UINT kidno=0; kidno<WN_kid_count(tree); kidno++)
    vcast_complex_types(WN_kid(tree,kidno));
  return;
}
#endif

/* ====================================================================
 *
 * WN *WN_Lower(WN *tree, LOWER_ACTIONS actions)
 *
 * Exported.  See interface description in "wn_lower.h".
 *
 * ==================================================================== */

WN *WN_Lower(WN *tree, LOWER_ACTIONS actions, struct ALIAS_MANAGER *alias,
	     const char *msg)
{
#ifdef BACK_END
  Start_Timer(T_Lower_CU);
#endif
  alias_manager =	alias;
  loop_nest_depth =	0;

#ifdef TARG_X8664
  current_loop_nest_depth = 0;
  memset( loop_info_stack, 0, sizeof(loop_info_stack) );
#endif

  // Don't do any lowering on trees that merely wrap up file-scope
  // assembly language code.
  if (WN_operator(tree) == OPR_FUNC_ENTRY &&
      ST_asm_function_st(*WN_st(tree))) {
    return tree;
  }

  actions = lower_actions(tree, actions);

#ifdef TARG_X8664
  if (Is_Target_SSE3() && Vcast_Complex &&
      Action(LOWER_COMPLEX) &&
      WN_operator(tree) == OPR_FUNC_ENTRY)
    vcast_complex_types(tree);
#endif

  if (Action(LOWER_MP)) {
    /* Initialize the MP lowerer.
     * Note: We're assuming that for MP-lowering,
     * this routine (WN_Lower) is called once per PU.
     */
    LowerMP_PU_Init ();
  }

  WN_Lower_Checkdump(msg, tree, actions);

  if (WN_opcode(tree) == OPC_FUNC_ENTRY)
  {
    tree = lower_entry(tree, actions);
  }
  else if (OPCODE_is_scf(WN_opcode(tree)))
  {
    tree = lower_scf(NULL, tree, actions);
  }
  else if (OPCODE_is_stmt(WN_opcode(tree)))
  {
    tree = lower_stmt(NULL, tree, actions);
  }
  else if (OPCODE_is_expression(WN_opcode(tree)))
  {
    tree = lower_expr(NULL, tree, actions);
  }

#ifdef TARG_ST
  // [CG] Do the post lowering phase on the tree.
  // This is required for some kinds of lowering where
  // the full block context is required.
  if (WN_opcode(tree) == OPC_FUNC_ENTRY &&
      post_lower_required(tree, actions)) {
    post_lower_entry(tree, actions);
  }
#endif
  lower_end(tree, actions);

  WN_Lower_Checkdump("After lowering", tree, 0);

#ifdef BACK_END
  Stop_Timer(T_Lower_CU);
#endif

#ifdef TARG_X8664
  FmtAssert( current_loop_nest_depth == 0, ("loop nest depth is not 0") );
#endif
 
  WN_verifier(tree);

  return tree;
}


/********** Beginning of  memory library transformation  **********/


/* ====================================================================
 *
 * Transform_To_Memset generates an instrinsic call to INTRN_MEMSET.
 *
 * Transform_To_Memcpy generates an instrinsic call to INTRN_MEMCPY.
 *
 * ==================================================================== */

static WN *Transform_To_Memset(WN *dst, INT32 offset,
			       TY_IDX dstTY, WN *src, WN *size)
{
  WN  *call, *parms[3];

  if (offset && WN_offset(dst) != offset)
    dst = WN_Add(Pointer_type, dst, WN_Intconst(Pointer_type, offset));
  markAddrTakenPassed(dst);  // 14795

  parms[0] = WN_CreateParm(Pointer_type,
			   dst,
			   dstTY ? dstTY : MTYPE_To_TY(WN_rtype(dst)),
			   WN_PARM_BY_VALUE);

  parms[1] = WN_CreateParm(MTYPE_U4,
			   src,
			   MTYPE_To_TY(MTYPE_U4),
			   WN_PARM_BY_VALUE);

  parms[2] = WN_CreateParm(WN_rtype(size),
			   WN_COPY_Tree(size),
			   MTYPE_To_TY(WN_rtype(size)),
			   WN_PARM_BY_VALUE);

  call = WN_Create_Intrinsic(OPC_VINTRINSIC_CALL, INTRN_MEMSET, 3, parms);
  WN_Set_Linenum(call, current_srcpos);
  return call;
}


static WN *Transform_To_Memcpy(WN *dst, WN *src,
			       INT32 offset_dst, INT32 offset_src,
			       TY_IDX dstTY, TY_IDX srcTY, WN *size)
{
  WN  *call, *parms[3];

  if (offset_src && WN_offset(src) != offset_src)
    src = WN_Add(Pointer_type, src, WN_Intconst(Pointer_type, offset_src));
  if (offset_dst && WN_offset(dst) != offset_dst)
    dst = WN_Add(Pointer_type, dst, WN_Intconst(Pointer_type, offset_dst));
  markAddrTakenPassed(src);  // 14795
  markAddrTakenPassed(dst);  // 14795

  parms[0] = WN_CreateParm(Pointer_type,
			   dst,
			   dstTY ? dstTY : MTYPE_To_TY(WN_rtype(dst)),
			   WN_PARM_BY_VALUE);

  parms[1] = WN_CreateParm(Pointer_type,
			   src,
			   srcTY ? srcTY : MTYPE_To_TY(WN_rtype(src)),
			   WN_PARM_BY_VALUE);

  parms[2] = WN_CreateParm(WN_rtype(size),
			   WN_COPY_Tree(size),
			   MTYPE_To_TY(WN_rtype(size)),
			   WN_PARM_BY_VALUE);

  call = WN_Create_Intrinsic(OPC_VINTRINSIC_CALL, INTRN_MEMCPY, 3, parms);
  WN_Set_Linenum(call, current_srcpos);
  return call;

} // Transform_To_Memcpy


extern WN *Transform_To_Memcpy(WN *dst, WN *src, INT32 offset,
			       TY_IDX dstTY, TY_IDX srcTY, WN *size)
{
  return Transform_To_Memcpy(dst, src, offset, offset, dstTY, srcTY, size);
}


/* ====================================================================
 *
 * For Lower_Mistore_Memlib and Lower_STID_Memlib below, we maintain
 * a sorted list of inodes.  Each inode represents one istore or stid,
 * and keeps track of the offset and length of the accessed memory.
 *
 * ==================================================================== */


typedef struct inode {
   WN *wn;
   INT start_offset;
   INT length;
   struct inode *next;
} inode_type;


// Insert node into list, sorted by increasing node->start_offset.
// Return pointer to head of list.
static inode_type *Insert_Inode(struct inode *head, struct inode *node)
{
  // Handle empty list
  if (head == NULL) return node;

  // Find place to insert node (between prev and current)
  struct inode *current, *prev = NULL;
  INT offset = node->start_offset;
  for (current = head; current; current = current->next) {
    if (offset <= current->start_offset) break;
    prev = current;
  }

  // Insert the node between prev and current,
  // then return pointer to head of list
  node->next = current;
  if (prev) {
    prev->next = node;
  } else {
    head = node;
  }
  return head;
}


/* ====================================================================
 *
 * Helper procedures
 *
 * ==================================================================== */


static INT32 Array_Offset(WN *wn)
{
  INT64 result, product;
  BOOL is_non_contig = FALSE;
  WN_ESIZE element_size;
  INT32	i, m;

  Is_True((WN_operator_is(wn, OPR_ARRAY)),
	  ("expected ARRAY node, not %s", OPCODE_name(WN_opcode(wn))));
  Is_True(WN_num_dim(wn) > 0, ("WN_num_dim of ARRAY node not > 0"));

  // All kids but the base must be INTCONST
  for (i = 1; i < WN_kid_count(wn); i++) {
    WN *kid = WN_kid(wn, i);
    if (! WN_operator_is(kid, OPR_INTCONST)) return -1;
  }
  const INT32 ndims = WN_num_dim(wn);

  element_size = WN_element_size(wn);
  if (element_size < 0) {
    is_non_contig = TRUE;
    element_size = - element_size;
  }

  if (is_non_contig) {
    result = WN_const_val(WN_array_index(wn, ndims - 1));
    result *= WN_const_val(WN_array_dim(wn, ndims - 1));
    for (i = ndims - 2; i >= 0; --i) {
      product = WN_const_val(WN_array_index(wn, i));
      product *= WN_const_val(WN_array_dim(wn, i));
      result += product;
    }
  } else {
    // result <- index[n-1]
    result = WN_const_val(WN_array_index(wn, ndims - 1));
    // result <- result + index[i] * ( dim[n-1] * dim[n-2] ... dim[i+1] )
    for (i = ndims - 2; i >= 0; --i) {
      product = WN_const_val(WN_array_dim(wn, ndims - 1));
      for (m = ndims - 2; m > i; --m) {
	product *= WN_const_val(WN_array_dim(wn, m));
      }
      result += WN_const_val(WN_array_index(wn, i)) * product;
    }
  }

  // result <- base + result * elm_size
  result *= element_size;
  return result;
}


// Invoke with offset = 0.
static WN* Analyze_Address(WN *wn, INT64 &offset)
{
  INT64 t;

  if (WN_operator(wn) == OPR_ILOAD) {
    offset += WN_load_offset(wn);
    wn = WN_kid0(wn);
  } else if (WN_operator(wn) == OPR_ISTORE) {
    offset += WN_store_offset(wn);
    wn = WN_kid1(wn);
  } else return NULL;

  while (TRUE) {
    switch (WN_operator(wn)) {
    case OPR_LDID:
      return (WN_class(wn) != CLASS_PREG ? wn : NULL);
    case OPR_LDA:
      // offset += WN_lda_offset(wn);
      return wn;
    case OPR_ARRAY:
      t = Array_Offset(wn);
      if (t < 0) return NULL;
      offset += t;
      wn = WN_array_base(wn);
      break;
    case OPR_ADD:
      if (WN_operator(WN_kid0(wn)) == OPR_INTCONST) {
	offset += WN_const_val(WN_kid0(wn));
	wn = WN_kid1(wn);
      }
      else if (WN_operator(WN_kid1(wn)) == OPR_INTCONST) {
	offset += WN_const_val(WN_kid1(wn));
	wn = WN_kid0(wn);
      }
      else return NULL;
      break;
    case OPR_SUB:
      if (WN_operator(WN_kid1(wn)) == OPR_INTCONST) {
	offset -= WN_const_val(WN_kid1(wn));
	wn = WN_kid0(wn);
      }
      else return NULL;
      break;
    default:
      return NULL;
    }
  }
  return NULL;
}


// Search wn for an LDA or pointer LDID.
static WN *Find_Pointer(WN *wn, BOOL is_array)
{
  WN *return_wn = NULL;
  if (WN_operator_is(wn, OPR_LDA) ||
      (WN_operator_is(wn, OPR_LDID) &&
       (is_array || TY_is_pointer(WN_ty(wn))))) {
    return_wn = wn;
  } else {
    for (INT i = 0; i < WN_kid_count(wn); ++i) {
      return_wn = Find_Pointer(WN_kid(wn, i), is_array);
      if (return_wn) break;
    }
  }
  return return_wn;
}


// Return TRUE only if we are sure the memory ranges of length bsize
// starting at wn1 and wn2 do not alias.
static BOOL Memlib_Two_Aliased(WN *wn1, WN *wn2, INT bsize)
{
  BOOL is_array = FALSE;
  if (WN_operator_is(wn1, OPR_ARRAY) && WN_operator_is(wn2, OPR_ARRAY)) {
    is_array = TRUE;
    wn1 = WN_kid0(wn1);
    wn2 = WN_kid0(wn2);
  }
  WN *base1 = Find_Pointer(wn1, is_array);
  WN *base2 = Find_Pointer(wn2, is_array);
  if (base1 && base2) return lower_is_aliased(base1, base2, bsize);

  return TRUE; // Assume the worst
}


/* ====================================================================
 *
 * WN *Lower_Mistore_Memlib(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Attempts to convert a sequence of MISTORE nodes into a single
 * equivalent call to memcpy.  Returns tree if unsuccessful.
 *
 * ==================================================================== */


static WN *Lower_Mistore_Memlib(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  TY_IDX pty_idx = WN_ty(tree);
  TY_IDX dstTY, srcTY;
  TY_IDX ty_idx = TY_pointed(pty_idx);
  WN *store_base, *load_base, *st_base, *ld_base;
  WN *load_pp, *store_pp;
  struct inode *one_node, *istore_list = NULL;
  WN *stmt = tree;
  INT i, size, start_offset, end_offset, total_size, total_num = 0;
  INT64 const_val;
  WN *return_block = WN_CreateBlock();
  BOOL is_const = FALSE;
  BOOL st_iload, ld_iload, pp_iload;
  INT64 load_offset, store_offset, ld_offset, st_offset;

  static INT delete_num = 0;
  static INT delete_bound = 0;
  static WN *deleted_wn[DELETED_MAX];
  static WN *delete_base = NULL;
  // TODO: Add skip_wn (like deleted_wn) to reduce unnecessary work.

  if (! OPT_Lower_To_Memlib) return tree;  // Abort if optimization is off

  Is_True(WN_operator(tree) == OPR_ISTORE,
	  ("Expected mistore node, not %s", OPCODE_name(WN_opcode(tree))));

  Is_True(TY_kind(pty_idx) == KIND_POINTER,
	  ("type specified in MISTORE not pointer"));

  // Was tree marked for deletion by a previous call to Lower_Mistore_Memlib?
  for (i = 0; i < delete_bound; ++i) {
    if (deleted_wn[i] == tree) break;
  }
  if (i < delete_bound) {
    delete_num--;
    return NULL;
  }

  if (WN_operator_is(WN_kid0(tree), OPR_INTCONST)) {
    is_const = TRUE;
    return tree;  // Don't handle constant
  }

  if (WN_operator(WN_kid0(tree)) != OPR_ILOAD) return tree;  // Expect ILOAD

  load_offset = 0;
  store_offset = 0;
  load_base = Analyze_Address(WN_kid0(tree), load_offset);
  store_base = Analyze_Address(tree, store_offset);

  if (store_base == NULL || (!is_const && load_base == NULL))
    return tree;

  if (delete_num) { // when some deleted nodes exist, check if the same base
    Is_True(store_pp, ("Expecting non-null saved deleted base "));
    if (WN_st(delete_base) != WN_st(store_base))
      DevWarn("Inconsistent base in Mistore_Memlib");
    return tree; //if same as the deleted base, don't process the node
  }

  if (OPT_Enable_Lower_To_Memlib_Limit != -1 &&
      cur_memlib_idx >= OPT_Enable_Lower_To_Memlib_Limit)
    return tree;

  // Start a new round of optimziation
  Is_True(delete_num == 0, ("Some unclear deleted WN"));
  delete_num = 0;
  delete_bound = 0;
  delete_base = NULL;

  // Termination condition: function calls or block end
  for ( ; stmt && ! OPERATOR_is_endsbb(WN_operator(stmt)) &&
	  WN_operator(stmt) != OPR_LABEL ; stmt = WN_next(stmt) ) {

    if (WN_operator(stmt) != OPR_ISTORE) continue;

    pty_idx = WN_ty(stmt);
    ty_idx = TY_pointed(pty_idx);
    if (WN_field_id (stmt) != 0)
      ty_idx = get_field_type (ty_idx, WN_field_id(stmt));
    size = TY_size(Ty_Table[ty_idx]);
    // Is_True(size > 0, ("type in MISTORE cannot be zero size"));
    if (size == 0)
      DevWarn ("Type in MISTORE cannot be zero size");

    total_num++;

    // Get LDID
    if (is_const) {
      if (!WN_operator_is(WN_kid0(stmt), OPR_INTCONST)
	  || const_val != WN_const_val(WN_kid0(stmt))) {
	// continue; // process the next statement
	return tree; // save compiling time
      }
    } else {
      if (WN_operator(WN_kid0(stmt)) != OPR_ILOAD) return tree; // Expect ILOAD
    }

    ld_offset = 0;
    st_offset = 0;
    ld_base = Analyze_Address(WN_kid0(stmt), ld_offset);
    st_base = Analyze_Address(stmt, st_offset);

    if (st_base == NULL || WN_st(store_base) != WN_st(st_base)) continue;
    if (! is_const) {
      if (ld_base == NULL || WN_st(load_base) != WN_st(ld_base)) continue;
      if (st_offset - store_offset != ld_offset - load_offset) continue;
    }

    // Record the ISTORE info
    struct inode *node = (struct inode *) alloca(sizeof(struct inode));
    node->wn = stmt;
    node->start_offset = st_offset;
    node->length = size;
    node->next = NULL;
    istore_list = Insert_Inode(istore_list, node);
  }

  // Retrieve data from head node
  if (istore_list == NULL) return tree;
  stmt = istore_list->wn;
  load_offset = store_offset = 0;
  load_base = Analyze_Address(WN_kid0(stmt), load_offset);
  store_base = Analyze_Address(stmt, store_offset);

  // Iterate through inode list (sorted in order of increasing offsets).
  // Halt if non-consecutive stores are encountered.
  total_num = 0;
  start_offset = end_offset = store_offset;
  for (one_node = istore_list; one_node; one_node = one_node->next) {
    if (one_node->start_offset != end_offset) break;
    end_offset += one_node->length;
    if (++total_num >= DELETED_MAX) break;
  }
  total_size = end_offset - start_offset;

  // Abort if size or number of stores don't qualify
  if (total_size < MEMLIB_STRUCT_BYTES || total_num < MEMLIB_NUM_STORE) {
    return tree;
  }

  if (Memlib_Two_Aliased(store_base, load_base, total_size)) {
    if (traceMemlib) DevWarn("Memlib: aliased addresses");
    return tree;
  }

  ++cur_memlib_idx;

  // Transform get the starting address correctly
  if (!TY_is_pointer(WN_ty(store_base))) {
    store_base = WN_CreateLda(OPR_LDA, Pointer_Mtype, MTYPE_V,
			      store_offset, Make_Pointer_Type(WN_ty(stmt)),
			      WN_st_idx(stmt));
  }
  dstTY = TY_pointed(Ty_Table[WN_ty(store_base)]);

  WN *wn_intrinsic = NULL;
  if (is_const) {
    wn_intrinsic
      = Transform_To_Memset(WN_COPY_Tree(store_base), store_offset,
			    TY_pointer(dstTY), WN_kid0(stmt),
			    WN_CreateIntconst(OPC_I4INTCONST, total_size));
  }
  else {
    if (!TY_is_pointer(Ty_Table[WN_ty(load_base)])) {
      load_base = WN_CreateLda(OPR_LDA, Pointer_Mtype, MTYPE_V, load_offset,
			       Make_Pointer_Type(WN_ty(load_base)),
			       WN_st_idx(load_base));
    }
    srcTY = TY_pointed(Ty_Table[WN_ty(load_base)]);

    wn_intrinsic
      = Transform_To_Memcpy(WN_COPY_Tree(store_base),
			    WN_COPY_Tree(load_base),
			    store_offset, load_offset,
			    TY_pointer(dstTY), TY_pointer(srcTY),
			    WN_CreateIntconst(OPC_I4INTCONST, total_size));
  }

  if (traceMemlib) {
    DevWarn("TRANSFORMED MEMCPY (%s: %d) of:"
	    " Struct (%d bytes of %d istore)\n", Src_File_Name,
	    Srcpos_To_Line(current_srcpos), total_size, total_num);
  }

  // Mark for deletion the subsequent concatentated stores
  one_node = istore_list;
  for (i = 0; i < total_num; ++i) {
    deleted_wn[delete_num++] = one_node->wn;
    Is_True(delete_num < DELETED_MAX, ("Overflow in deleted_wn\n"));
    one_node = one_node->next;
  }
  delete_bound = delete_num;
  --delete_num;  // Exclude the current tree itself
  delete_base = store_base;

#ifdef KEY // bug 11360
  wn_intrinsic = lower_intrinsic_call(return_block, wn_intrinsic, actions);
#endif
  WN_INSERT_BlockLast(return_block, wn_intrinsic);
  return return_block;

}  // Lower_Mistore_Memlib


/* ====================================================================
 *
 * WN *Lower_STID_Memlib(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * ==================================================================== */

#ifdef KEY
// Verify that each of const_val's bytes has the same value;
// nbytes is the number of bytes to check
static BOOL Repetitive_byte_vals(INT64 const_val, INT nbytes)
{
#if 1
  const_val ^= (const_val >> 8);
  const_val &= ((INT64) 1 << ((nbytes - 1) * 8)) - 1;  // 14981
  return (const_val == 0);
#else
  // Slower Implementation:
  if (nbytes-- == 0) return FALSE;
  unsigned char bval = const_val;
  while (nbytes--) {
    const_val >>= 8;
    if (bval != (const_val & 0xff)) return FALSE;
  }
  return TRUE;
#endif
}
#endif


static WN *Lower_STID_Memlib(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  struct inode *one_node, *stid_list = NULL;
  INT i, size, start_offset, end_offset, total_size, total_num = 0;
  INT64 const_val;
  BOOL is_const = FALSE;
  WN *return_block = WN_CreateBlock();
  WN *stmt = tree;
  WN *store_base;
  TY_IDX  dstTY;
  static WN *deleted_wn[DELETED_MAX];
  static INT delete_num = 0;
  static INT delete_bound = 0;
  static WN *delete_stid = NULL;

  if (!OPT_Lower_To_Memlib) return tree;  // Abort if optimization is off

  Is_True(WN_operator(tree) == OPR_STID,
	  ("expected STID node, not %s", OPCODE_name(WN_opcode(tree))));

  // Was tree marked for deletion by a previous call to Lower_STID_Memlib?
  for (i = 0; i < delete_bound; ++i) {
    if (deleted_wn[i] == tree) break;
  }
  if (i < delete_bound) {
    delete_num--;
    return NULL;
  }

  if (delete_num) {  // when some deleted nodes exist, check if the same base
    Is_True(delete_stid, ("Expecting non-null saved deleted STID"));
    if (WN_st(stmt) != WN_st(delete_stid))
      DevWarn("Inconsistent base in STID_Memlib");
    return tree;
  }

  // Start a new round of optimization
  delete_num = 0;
  delete_bound = 0;
  delete_stid = NULL;

#ifdef KEY
  // Storing to non-negative SP offsets means we are pushing function
  // parameters onto the stack.  Can't use memset for this.  Bug 3942.
  if (WN_st(tree) == SP_Sym && WN_store_offset(tree) >= 0)
    return tree;
#endif

  // For now, only handle memset.
  if (WN_operator_is(WN_kid0(tree), OPR_INTCONST)) {
    const_val = WN_const_val(WN_kid0(tree)) & 0xff;
  }
  else return tree;

  if (OPT_Enable_Lower_To_Memlib_Limit != -1 &&
      cur_memlib_idx >= OPT_Enable_Lower_To_Memlib_Limit)
    return tree;

  // Scan this and subsequent STIDs until reaching a call or block end
  for ( ; stmt && ! OPERATOR_is_endsbb(WN_operator(stmt)) &&
	  WN_operator(stmt) != OPR_LABEL;
	stmt = WN_next(stmt) ) {

    if (WN_operator(stmt) != OPR_STID) continue;

    // Check size
    // if (WN_field_id (stmt) != 0) lower_field_id (stmt);

    size = MTYPE_byte_size(OPCODE_desc(WN_opcode(stmt)));

    if (! WN_operator_is(WN_kid0(stmt), OPR_INTCONST) ||
	const_val != (WN_const_val(WN_kid0(stmt)) & 0xff)
#ifdef KEY // bug 4895
	|| ! Repetitive_byte_vals(WN_const_val(WN_kid0(stmt)), size)
#endif
	) {
      continue;
      // return tree; // save compiling time
    }

    if (WN_st(stmt) != WN_st(tree)) {
      continue; // process the next statement
      // return tree; // Note: to save compile time
    }

    // Record the STID info
    struct inode *node = (struct inode *) alloca(sizeof(struct inode));
    node->wn = stmt;
    node->start_offset = WN_store_offset(stmt);
    node->length = size;
    node->next = NULL;
    stid_list = Insert_Inode(stid_list, node);
  }

  // Retrieve data from head node
  if (stid_list == NULL) return tree;
  stmt = stid_list->wn;

  // Iterate through inode list (sorted in order of increasing offsets).
  // Halt if non-consecutive stores are encountered.
  total_num = 0;
  start_offset = end_offset = WN_store_offset(stmt);
  for (one_node = stid_list; one_node; one_node = one_node->next) {
    if (one_node->start_offset != end_offset) break;
    end_offset += one_node->length;
    if (++total_num >= DELETED_MAX) break;
  }
  total_size = end_offset - start_offset;

  // Abort if size or number of stores don't qualify
  if (total_size < MEMLIB_STRUCT_BYTES || total_num < MEMLIB_NUM_STORE) {
    return tree;
  }

  ++cur_memlib_idx;

  store_base = WN_CreateLda(OPR_LDA, Pointer_Mtype, MTYPE_V, start_offset,
			    Make_Pointer_Type(WN_ty(stmt)), WN_st_idx(stmt));

  dstTY = TY_pointed(Ty_Table[WN_ty(store_base)]);

  WN *wn_intrinsic
    = Transform_To_Memset(WN_COPY_Tree(store_base), start_offset,
			  TY_pointer(dstTY),
			  WN_CreateIntconst(OPC_I4INTCONST, const_val),
			  WN_CreateIntconst(OPC_I4INTCONST, total_size) );

  DevWarn("TRANSFORMED MEMSET (%s: %d):"
	  " STID (%d bytes of %d stids)\n", Src_File_Name,
	  Srcpos_To_Line(current_srcpos), total_size, total_num);

  // Mark for deletion the subsequent concatentated stores
  one_node = stid_list;
  for (i = 0; i < total_num; ++i) {
    deleted_wn[delete_num++] = one_node->wn;
    Is_True(delete_num < DELETED_MAX, ("Overflow in deleted_wn\n"));
    one_node = one_node->next;
  }
  delete_bound = delete_num;
  --delete_num;  // Exclude the current tree itself
  delete_stid = tree;

#ifdef KEY // bug 11360
  wn_intrinsic = lower_intrinsic_call(return_block, wn_intrinsic, actions);
#endif
  WN_INSERT_BlockLast(return_block, wn_intrinsic);
  return return_block;

}  // Lower_STID_Memlib


/* ====================================================================
 *
 * Helper procedures
 *
 * ==================================================================== */


// Count the number of times "LDID ind" appears in the expression wn
static INT Find_Nums_Induction(WN *wn, WN *ind)
{
  INT i, total = 0;
  if (OPCODE_is_leaf(WN_opcode(wn))) {
    if (WN_operator(wn) == OPR_LDID && WN_same_id(wn, ind)) total = 1;
  } else {
    for (i = 0; i < WN_kid_count(wn); ++i) {
      total = total + Find_Nums_Induction(WN_kid(wn, i), ind);
    }
  }
  return total;
}


// Returns a pointer to LDID ind within exp.  Sets scaled to TRUE
// if the LDID ind occurs within a MPY operation.
static WN *Locate_Ind_Path(WN *wn, WN *ind, BOOL &scaled)
{
  WN *wn_ind = NULL;
  OPERATOR opr = WN_operator(wn);
  if (opr == OPR_LDID && WN_same_id(wn, ind)) return wn;

  if (opr == OPR_MPY || opr == OPR_ADD || opr == OPR_CVT) {
    for (INT i = 0; i < WN_kid_count(wn); ++i) {
      wn_ind = Locate_Ind_Path(WN_kid(wn, i), ind, scaled);
      if (wn_ind) break;
    }
    if (wn_ind && opr == OPR_MPY) scaled = TRUE;
  }
  return wn_ind;
}


// Returns TRUE iff the given address is composed solely of CVT,
// ADD, MUL, and leaf operators, and any scalar multiple of the
// induction variable matches msize.
static BOOL Valid_Induction_In_Addr(WN *addr, WN *ind, INT msize)
{
  WN *wn_mult = NULL;
  BOOL scaled = FALSE;
  OPERATOR opr = WN_operator(addr);

  if (opr == OPR_MPY) {
    // Check to see if kid0 contains the induction
    if (Locate_Ind_Path(WN_kid0(addr), ind, scaled)) {
      wn_mult = WN_kid1(addr);
    } else if (Locate_Ind_Path(WN_kid1(addr), ind, scaled)) {
      wn_mult = WN_kid0(addr);
    } else return FALSE;
    if (scaled) return FALSE;

    // Check that the multiplier matches msize
    while (WN_operator(wn_mult) == OPR_CVT) wn_mult = WN_kid0(wn_mult);
    return ( WN_operator_is(wn_mult, OPR_INTCONST) &&
	     WN_const_val(wn_mult) == msize );
    // NOTE: If there is no MPY, we don't check that msize must be 1,
    // but if such program exists, it would have run-time misaligned
    // memory access.
  }

  if (OPERATOR_is_leaf(opr)) return TRUE;

  if (opr != OPR_ADD && opr != OPR_CVT) return FALSE;

  // TODO: allow OPR_SUB, in which case have to make sure it does not
  //       negate the induction variable

  // Check if all the kids are valid
  for (INT i = 0; i < WN_kid_count(addr); ++i)
    if (! Valid_Induction_In_Addr(WN_kid(addr, i), ind, msize))
      return FALSE;
  return TRUE;

}  // Valid_Induction_In_Addr


// Return if wn contains only LDID ind and loop invariant components.
static BOOL Is_Invaried_Value(WN *wn, WN *ind)
{
  switch(WN_operator(wn)) {
  case OPR_INTCONST:  return TRUE;
  case OPR_ILOAD:
  case OPR_LDA:       return FALSE;
  case OPR_LDID:      return ! WN_same_id(wn, ind);
  }

  // Are the kids loop invariant or LDID ind?
  for (INT i = 0; i < WN_kid_count(wn); ++i) {
    if (! Is_Invaried_Value(WN_kid(wn, i), ind)) return FALSE;
  }
  return TRUE;
}


/* IGNORE: opcode to ignore
 * ABORT_OPT: opcode to abort the transformation
 * CONT_OPT: opcode to continue the transformation
 * Currently we deal with only ISTORE to perform memmove/memset optimization
 */
static MEMLIB_ACTIONS  Memlib_Filter(WN *stmt)
{
  switch(WN_operator(stmt)) {
  case OPR_ISTORE:
  case OPR_DO_LOOP:    return CONT_OPT;
  case OPR_PREFETCH:
  case OPR_PREFETCHX:  return IGNORE;
  default:             return ABORT_OPT;
  }
}


// Returns TRUE if the LDID ind appears in the second operand
// of a subtraction within addr.
static BOOL Is_Preceding_Subtract(WN *addr, WN *ind)
{
  WN *wn = NULL;
  INT i, num = 0;
  BOOL scaled = FALSE;

  // Check index of ARRAY expression
  if (WN_operator(addr) == OPR_ARRAY)
    return Is_Preceding_Subtract(WN_kid2(addr), ind);

  // Find a subtraction, check the second kid containing any ind
  if (WN_operator(addr) == OPR_SUB) {
    //check if any inducton in the right subtree and under
    return ( Locate_Ind_Path(WN_kid1(addr), ind, scaled) ? TRUE : FALSE );
  }

  for (i = 0; i < WN_kid_count(addr); ++i)
    if (Is_Preceding_Subtract(WN_kid(addr, i), ind)) return TRUE;
  return FALSE;
}


static WN *Start_Addr_To_Transform(WN *block, WN* addr, TY_IDX &desc_idx)
{
  WN *target_addr = NULL;
  if (WN_operator(addr) == OPR_ARRAY) {
    // Lower ARRAY to an address expression
    desc_idx = Make_Pointer_Type(desc_idx);
    target_addr = lower_expr(block, addr, LOWER_ARRAY);
  }
  else {
    // General address expression
    desc_idx = WN_ty(addr);
    target_addr = addr;
  }
  return target_addr;
}


static BOOL Valid_Addr_Expr(WN *addr, WN *ind, int bsize)
{
  Is_True(WN_operator(addr) != OPR_ARRAY,
	  ("Valid_Addr_Expr: Unexpected ARRAY"));
  if (Find_Nums_Induction(addr, ind) != 1) return FALSE;
  else return Valid_Induction_In_Addr(addr, ind, bsize);
}


// Search the expression index_wn for one LDID of any of the induction
// variables in the list inds.  Return the index of the found variable.
// Return -1 if no such LDID is found.
// Return -2 if more than one such LDID is found.
static INT Find_Ind_Var(WN *wn, WN *inds[], INT upper)
{
  INT i, idx = -2;
  if (WN_operator(wn) == OPR_LDID) {
    for (idx = 0; idx < upper; ++idx) {
      if (WN_same_id(wn, inds[idx])) return idx;
    }
  }
  if (OPCODE_is_leaf(WN_opcode(wn))) return -2;

  // Search for first instance of an induction variable
  for (i = 0; i < WN_kid_count(wn); ++i) {
    idx = Find_Ind_Var(WN_kid(wn, i), inds, upper);
    if (idx != -2) break;
  }

  if (idx >= 0) {
    // Check remaining kids for extra induction variables
    for (++i; i < WN_kid_count(wn); ++i) {
      INT idx_kid = Find_Ind_Var(WN_kid(wn, i), inds, upper);
      if (idx_kid != -2) { idx = -1; break; }
    }
  }

  return idx;
}


/* ====================================================================
 *
 * WN *Memset_MD_Array(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * The routine transforms a multi-dim array initialization to memset
 * Currently only a normalized a[i][j][k] = CONST is transformed
 *
 * ==================================================================== */

static WN *Memset_MD_Array(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  INT num_stmt, mbyte_stmt, loop_level = 0;
  INT total_size = 1;
  INT trips[MEMLIB_MAX_NUM];
  OPCODE ub_compare;
  TY_IDX desc_idx;
  WN *ind_vars[MEMLIB_MAX_NUM];
  WN *stmt, *body = WN_do_body(tree);
  WN *load_value_wn, *store_addr_wn, *target_addr;
  WN *loop_indvar,  *trip_count;
  WN *lower_bound, *upper_bound;
  WN *current_wn = tree;
  WN *loop_info;
  WN *return_block = WN_CreateBlock();

  Is_True(WN_opcode(tree) == OPC_DO_LOOP,
	  ("expected DO_LOOP node, not %s", OPCODE_name(WN_opcode(tree))));

  // Descend through the loop nest
  while (WN_opcode(current_wn) == OPC_DO_LOOP) {

    if (loop_level >= MEMLIB_MAX_NUM) {
      // 15195: Don't overflow trips and ind_vars
      DevWarn("Memlib: mult-dim: loop_level too deep");
      return tree;
    }

    loop_info = WN_do_loop_info(current_wn);
    if (loop_info == NULL) {
      if (traceMemlib) DevWarn("NULL loop info in Lower_Memlib: MD_array");
      return tree;
    }
    loop_indvar = WN_loop_induction(loop_info);
    if ( !loop_indvar ) return tree;
    lower_bound = WN_LOOP_LowerBound(current_wn);
    if ( !lower_bound ) return tree;
    upper_bound = WN_LOOP_UpperBound(current_wn, &ub_compare);
    if ( !upper_bound ) return tree;
    trip_count = WN_LOOP_TripCount(current_wn);
    if ( !trip_count ) return tree;
    if (!WN_operator_is(trip_count, OPR_INTCONST)) return tree;

    trips[loop_level] = WN_const_val(trip_count);
    ind_vars[loop_level] = loop_indvar;
    loop_level++;

    if (WN_const_val(lower_bound) != 0) {
      DevWarn("Memlib: mult-dim: lower bound is not 0");
      return tree;
    }

    // Scan the loop body, which should contain only one relevant stmt
    // an ISTORE or a nested DO_LOOP.
    num_stmt = 0;
    body = WN_do_body(current_wn);
    for (stmt = WN_first(body); stmt; stmt = WN_next(stmt)) {
      if (Memlib_Filter(stmt) == ABORT_OPT) return tree;
      if (Memlib_Filter(stmt) == IGNORE) continue;
      if (num_stmt++) return tree;
      current_wn = stmt;
    }

  } // while

  // Now, current_wn should be innermost ISTORE.
  if (WN_operator(current_wn) != OPR_ISTORE) {
    DevWarn("Memlib: mult-dim: is not an ISTORE");
    return tree;
  }

  mbyte_stmt = MTYPE_byte_size(OPCODE_desc(WN_opcode(current_wn)));
  load_value_wn = WN_kid0(current_wn);
  target_addr = WN_kid1(current_wn);
  store_addr_wn = WN_kid1(current_wn);

  // If assigned a constant 0 or -1, or of size 1, it is allowed to optimize as memset
  if (!(WN_operator_is(load_value_wn, OPR_INTCONST) &&
	(WN_const_val(load_value_wn) == 0 ||
	 WN_const_val(load_value_wn) == -1))
      && mbyte_stmt != 1) {
    DevWarn("Memlib: multi-dim memset: size is not 1");
    return tree;
  }

  if (!Is_Invaried_Value(load_value_wn, loop_indvar)) {
    DevWarn("Memlib: multi-dim memset:  not constant");
    return tree;
  }

#ifdef KEY // bug 13219: things fall apart without this check
  if (! WN_operator_is(store_addr_wn, OPR_ARRAY))
    return tree;
#endif

  // Examine if eligible for full memset
  INT num_dim = 0;
  INT indx;
  while (WN_operator_is(store_addr_wn, OPR_ARRAY)) {
    num_dim++;
    indx = Find_Ind_Var(WN_kid2(store_addr_wn), ind_vars, loop_level);
    if (!WN_operator_is(WN_kid1(store_addr_wn), OPR_INTCONST) ||
	trips[indx] != WN_const_val(WN_kid1(store_addr_wn))) {
      return tree;
    }
    if (Is_Preceding_Subtract(WN_kid2(store_addr_wn), ind_vars[indx])) {
      DevWarn("Memlib: mult-dim: preceding minus of indunction");
      return tree;
    }
    total_size *= trips[indx];
    store_addr_wn = WN_kid0(store_addr_wn);
  } //while

  desc_idx = WN_ty(current_wn); //initialization
  target_addr = Start_Addr_To_Transform(block, WN_COPY_Tree(target_addr),
					desc_idx);
  WN *call = Transform_To_Memset(target_addr, 0, desc_idx,
				 WN_COPY_Tree(load_value_wn),
				 WN_CreateIntconst(OPC_I4INTCONST,
						   total_size));
  DevWarn("TRANSFORMED MEMSET (%s: %d): mult-dim",
	  Src_File_Name, Srcpos_To_Line(current_srcpos));

  if (call) {
    WN_Delete(current_wn);
#ifdef KEY // bug 11360
    call = lower_intrinsic_call(return_block, call, actions);
#endif
    WN_INSERT_BlockLast(return_block, call);
  }
  else
    WN_INSERT_BlockLast(return_block, tree);

  return return_block;
}


/* ====================================================================
 *
 * WN *Lower_Memlib(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform memset/memcpy transformation on statements in DO_LOOP node <tree>,
 * returning correspinding intrinsic calls.  Returned tree will always
 * have a structured control flow node (at least a BLOCK) at the top.
 *
 * ==================================================================== */

static WN *Lower_Memlib(WN *block, WN *tree, LOWER_ACTIONS actions)
{
  WN *loop_info;
  WN *length_wn, *ind_init_val = NULL;
  WN *load_value_wn, *store_addr_wn, *load_addr_wn, *load_wn;
  WN *target_addr, *source_addr;
  WN *stmt, *body = WN_do_body(tree);
  WN *return_block = WN_CreateBlock();
  WN *nzcond = NULL;
  TY_IDX desc_idx, source_idx;
  OPCODE ub_compare;
  BOOL target_minus_ind, source_minus_ind;
  BOOL opt_qualified;
  INT i, elm_size, mbyte_stmt, istore_num = 0;
  INT freq_count = (INT) INT32_MAX;

  Is_True(WN_opcode(tree) == OPC_DO_LOOP,
	  ("expected DO_LOOP node, not %s", OPCODE_name(WN_opcode(tree))));

  if (!OPT_Lower_To_Memlib) return tree;  // Abort if optimization is off

  loop_info = WN_do_loop_info(tree);
  if (loop_info == NULL) {
    if (traceMemlib) DevWarn("NULL loop info in Lower_Memlib");
    return tree;
  }

  WN *loop_indvar = WN_loop_induction(loop_info);
  if ( !loop_indvar ) return tree;
  BOOL is_incr;
  WN *loop_stride = WN_LOOP_Increment(tree, &is_incr);
  if (!loop_stride || is_incr == FALSE ||
      !WN_operator_is(loop_stride, OPR_INTCONST)  ||
      WN_const_val(loop_stride) != 1)
    return tree;
  WN *upper_bound = WN_LOOP_UpperBound(tree, &ub_compare, TRUE);
  if ( !upper_bound ) return tree;
  WN *trip_count = WN_LOOP_TripCount(tree, TRUE);
  if ( !trip_count ) return tree;
  WN *lower_bound = WN_LOOP_LowerBound(tree);
  if ( !lower_bound ) return tree;

  Is_True(WN_opcode(loop_indvar), ("expected a non-NULL loop induction"));

  // If no count for this loop, return tree to ensure the benefit
  if (!OPT_Enable_Memlib_Aggressive &&
      !Cur_PU_Feedback && !WN_operator_is(trip_count, OPR_INTCONST))
    return tree;

  // Set freq_count if feedback available
  if (Cur_PU_Feedback) {
    freq_count
      = (INT) Cur_PU_Feedback->Query(tree, FB_EDGE_LOOP_ITERATE).Value();
  }

  // If the trip count is constant and too small, don't optimize
  if (WN_operator_is(trip_count, OPR_INTCONST)) {
    if (Cur_PU_Feedback && WN_const_val(trip_count) != freq_count) {
      DevWarn("Inconsistent constant trip count and feedback");
    }
    freq_count = WN_const_val(trip_count);
  }

  // If too small, < threshold, don't transform
  // If small, < 2*threshold, and not divisible by 8, don't transform
  if (freq_count < MEMLIB_THRESHOLD_BYTES ||
      (freq_count < MEMLIB_THRESHOLD_BYTES * 2 && freq_count % 8 != 0))
    return tree;

  // Try to find the memset opportunity first
  // Check if only ISTORE of a constant or an invariant is in the loop body
  istore_num = 0;
  opt_qualified = TRUE;
  for (stmt = WN_first(body); stmt; stmt = WN_next(stmt)) {
    if (Memlib_Filter(stmt) == ABORT_OPT) return tree;
    else if (Memlib_Filter(stmt) == IGNORE) continue; // go to next statement
    Is_True(Memlib_Filter(stmt) == CONT_OPT, ("Memlib: expect CONT_OPT"));

    // Deal with multi-dim array memset; Exit if transformation succeeds
    if (WN_operator_is(stmt, OPR_DO_LOOP)) {
      WN *return_wn = Memset_MD_Array(block, tree, actions);
      if (! WN_operator_is(return_wn, OPR_DO_LOOP)) return return_wn;
    }

    // Currently deal with only non-float types
    if (MTYPE_is_float(WN_desc(stmt)))
      return tree;

    load_value_wn = WN_kid0(stmt);
    if (WN_operator(load_value_wn) == OPR_PAREN)
      load_value_wn = WN_kid0(load_value_wn);
    store_addr_wn = WN_kid1(stmt);

    // If assigned a constant 0 or -1, or of size 1,
    // it is allowed to optimize as memset
    mbyte_stmt = MTYPE_byte_size(OPCODE_desc(WN_opcode(stmt)));
    if ( mbyte_stmt != 1 &&
	 ( ! WN_operator_is(load_value_wn, OPR_INTCONST) ||
	   ( WN_const_val(load_value_wn) != 0 &&
	     WN_const_val(load_value_wn) != -1 ) ) ) {
      opt_qualified = FALSE;
      break; //exit memset try
    }

    if (!Is_Invaried_Value(load_value_wn, loop_indvar)) {
      opt_qualified = FALSE;
      break; //exit memset try
    }

    // Lower the array expr
    if (WN_operator(store_addr_wn) == OPR_ARRAY)
      store_addr_wn = lower_expr(block,
				 WN_COPY_Tree(store_addr_wn), LOWER_ARRAY);

    // If illegal address format, abort optimization
    if (!Valid_Addr_Expr(store_addr_wn, loop_indvar, mbyte_stmt))
      return tree;
    ++istore_num;

  }  // for loop for memset identification

  // Transform the stmt to be instrinsic memset call
  if (opt_qualified) {
    if (OPT_Enable_Lower_To_Memlib_Limit != -1 &&
	cur_memlib_idx >= OPT_Enable_Lower_To_Memlib_Limit)
      return tree;
    cur_memlib_idx ++;

    BOOL prev_sign, current_sign;

    istore_num = 0;

    for (stmt = WN_first(body); stmt; stmt = WN_next(stmt)) {
      if (Memlib_Filter(stmt) == ABORT_OPT) return tree;
      else if (Memlib_Filter(stmt)== IGNORE ) continue; // go to next statement

#ifdef KEY // bug 8604
      mbyte_stmt = MTYPE_byte_size(OPCODE_desc(WN_opcode(stmt)));
#endif
      Is_True(mbyte_stmt > 0,
	      ("expected positive size not %d",
	       MTYPE_byte_size(OPCODE_desc(WN_opcode(stmt)))));

      if (traceMemlib) DevWarn("Original STMT");

      load_value_wn = WN_kid0(stmt);
      store_addr_wn = WN_kid1(stmt);

      // Create the size wn to be passed to the intrinsic call
      if ( mbyte_stmt > 1) {
	if (WN_intrinsic(trip_count) >= INTRN_I4DIVFLOOR &&
	    WN_intrinsic(trip_count) <= INTRN_U8DIVCEIL &&
	    WN_operator(WN_kid0(WN_kid1(trip_count))) == OPR_INTCONST &&
	    WN_const_val(WN_kid0(WN_kid1(trip_count))) == mbyte_stmt) {
	  length_wn = WN_COPY_Tree(WN_kid0(WN_kid0(trip_count)));
	} else {
	  OPCODE mpyopcode = OPCODE_make_op(OPR_MPY, WN_rtype(store_addr_wn),
					    MTYPE_V);
	  length_wn = WN_CreateExp2( mpyopcode, WN_COPY_Tree(trip_count),
				     WN_CreateIntconst (OPC_U4INTCONST,
							mbyte_stmt));
	}

      }
      else length_wn = trip_count;

#ifdef KEY
      // 15215: If length_wn is an expression that could sometimes be
      // negative, generate an "length_wn > 0" condition.
      if (! WN_operator_is(trip_count, OPR_INTCONST) &&
	  MTYPE_is_signed(WN_rtype(length_wn))) {
	WN *wn0 = NULL, *wn1 = NULL;
	if (WN_operator_is(length_wn, OPR_SUB)) {
	  // If Simp_Unsafe_Relops is off, simplify anyway.
	  wn0 = WN_COPY_Tree(WN_kid0(length_wn));
	  wn1 = WN_COPY_Tree(WN_kid1(length_wn));
	} else {
	  wn0 = WN_COPY_Tree(length_wn);
	  wn1 = WN_CreateIntconst(OPC_I4INTCONST, 0);
	}
	nzcond = WN_GT(WN_rtype(length_wn), wn0, wn1);
      }
#endif

      desc_idx = WN_ty(stmt); //initialization

      store_addr_wn = WN_COPY_Tree(store_addr_wn);
      if (WN_operator(store_addr_wn) == OPR_ARRAY)
	store_addr_wn = Start_Addr_To_Transform(return_block,
						store_addr_wn, desc_idx);

      // initialize the induction as needed
      OPCODE opc_stid = OPCODE_make_op(OPR_STID, MTYPE_V,
				       TY_mtype(ST_type(WN_st(loop_indvar))));
      current_sign = Is_Preceding_Subtract(store_addr_wn, loop_indvar);
      if (current_sign) {

#ifndef KEY
	Is_True(trip_count != NULL, ("Memlib: expecting non-null trip count"));
	OPCODE mpyopcode
	  = OPCODE_make_op(OPR_SUB, WN_rtype(trip_count), MTYPE_V);
	WN *sub_wn = WN_CreateExp2(mpyopcode, WN_COPY_Tree(trip_count),
				   WN_CreateIntconst(OPC_I4INTCONST, 1));
	// Bug 4789
	ind_init_val = WN_CreateStid(opc_stid, WN_idname_offset(loop_indvar),
				     WN_st(loop_indvar), Be_Type_Tbl(MTYPE_I4),
				     sub_wn, 0);
	// to trip_count - 1 if minus induction

#else
	// Bug 5299
	ind_init_val = WN_CreateStid(opc_stid, WN_idname_offset(loop_indvar),
				     WN_st(loop_indvar), Be_Type_Tbl(MTYPE_I4),
				     WN_COPY_Tree(upper_bound), 0);
	// to upper bound
#endif
      } else if (lower_bound != NULL) {
	//Bug 4789
	ind_init_val = WN_CreateStid(opc_stid, WN_idname_offset(loop_indvar),
				     WN_st(loop_indvar), Be_Type_Tbl(MTYPE_I4),
				     WN_COPY_Tree(lower_bound), 0);
	// to lower bound
      }

      if (ind_init_val) {
	DevWarn("Initial induction variable");
	WN_Set_Linenum(ind_init_val, current_srcpos);
	WN_INSERT_BlockLast(return_block, ind_init_val);
      }

      WN *call = Transform_To_Memset(store_addr_wn, 0, desc_idx,
				     WN_COPY_Tree(load_value_wn), length_wn);

      DevWarn("TRANSFORMED MEMSET Memlib (%s, %d) of %d\n",
	      Src_File_Name, Srcpos_To_Line(current_srcpos), freq_count);

#ifdef KEY
      // 15215: If length_wn is an expression that could sometimes be
      // negative, wrap an "if (length_wn > 0)" around the MEMSET.
      if (nzcond) {
	WN *then_block = WN_CreateBlock();
	WN_INSERT_BlockLast(then_block, call);
	WN *wn_if = WN_CreateIf(nzcond, then_block, WN_CreateBlock());
	wn_if = lower_if(return_block, wn_if, actions);
	WN_INSERT_BlockLast(return_block, wn_if);
      } else
#endif
      {
#ifdef KEY // bug 11360
	call = lower_intrinsic_call(return_block, call, actions);
#endif
	WN_INSERT_BlockLast(return_block, call);
      }
      WN_DELETE_FromBlock(body, stmt);
    } /*for*/

#ifdef KEY
    // 15102: Return the empty loop, so that the induction variable
    // will be properly updated.
    WN_INSERT_BlockLast(block, return_block);
    return tree;
#else
    return return_block;
#endif
  } /* Done with memset optimization*/

  // Try to find the memmove opportunity then
  // Check if only one indirect assignment (ISTORE) is in the loop body
  istore_num = 0;
  for (stmt = WN_first(body); stmt; stmt = WN_next(stmt)) {
    if (Memlib_Filter(stmt) == ABORT_OPT) return tree;
    else if (Memlib_Filter(stmt) == IGNORE) continue;

    if (++istore_num > 1)
      return tree; // no transformation if more than one Istore

    //if the first kid is not ILOAD, then return
    if (WN_operator(WN_kid0(stmt)) != OPR_ILOAD &&
	!(WN_operator(WN_kid0(stmt)) == OPR_PAREN &&
	  WN_operator(WN_kid0(WN_kid0(stmt))) == OPR_ILOAD))
      return tree;

    if (WN_operator(WN_kid0(stmt)) == OPR_ILOAD)
      load_wn = WN_kid0(stmt);
    else
      load_wn = WN_kid0(WN_kid0(stmt));

    load_addr_wn = WN_kid0(load_wn);
    store_addr_wn = WN_kid1(stmt);

    if (MTYPE_byte_size(WN_desc(load_wn)) != MTYPE_byte_size(WN_desc(stmt))) {
      if (traceMemlib) DevWarn(" Memlib no match  on size for memmove \n");
      return tree; //don't do any optimization
    }

    mbyte_stmt = MTYPE_byte_size(OPCODE_desc(WN_opcode(stmt)));

    if (WN_operator(store_addr_wn) == OPR_ARRAY)
      store_addr_wn = lower_expr(block, WN_COPY_Tree(store_addr_wn),
				 LOWER_ARRAY);
    if (WN_operator(load_addr_wn) == OPR_ARRAY)
      load_addr_wn = lower_expr(block, WN_COPY_Tree(load_addr_wn),
				LOWER_ARRAY);

    // Need to check legal addresses for optimization
    if (! Valid_Addr_Expr(store_addr_wn, loop_indvar, mbyte_stmt) ||
	! Valid_Addr_Expr(load_addr_wn, loop_indvar, mbyte_stmt)) {
      return tree;
    }

  } //for

  if (OPT_Enable_Lower_To_Memlib_Limit != -1 &&
      cur_memlib_idx >= OPT_Enable_Lower_To_Memlib_Limit)
    return tree;
  cur_memlib_idx ++;
  // Start transforming to instrinsic  call
  for (stmt = WN_first(body); stmt; stmt = WN_next(stmt)) {
    if (Memlib_Filter(stmt) == ABORT_OPT) return tree;
    else if (Memlib_Filter(stmt) == IGNORE) continue;

    // if the freq is too low, don't optimize
    if (Cur_PU_Feedback &&
	Cur_PU_Feedback->Query(tree, FB_EDGE_LOOP_ITERATE).Value()
	< MMCOPY_MULT  * MEMLIB_THRESHOLD_BYTES ||
	WN_operator_is(trip_count, OPR_INTCONST) &&
	WN_const_val(trip_count) < MMCOPY_MULT * MEMLIB_THRESHOLD_BYTES)
      return tree;

    if (traceMemlib) DevWarn("Original STMT \n");

    if (WN_operator(WN_kid0(stmt)) == OPR_ILOAD)
      load_wn = WN_kid0(stmt);
    else
      load_wn = WN_kid0(WN_kid0(stmt));
    load_addr_wn = WN_kid0(load_wn);
    store_addr_wn = WN_kid1(stmt);
    mbyte_stmt = MTYPE_byte_size(OPCODE_desc(WN_opcode(stmt)));

    if (Memlib_Two_Aliased(store_addr_wn, load_addr_wn, mbyte_stmt)) {
      if (traceMemlib) DevWarn("Memlib: aliased addresses");
      return tree;
    }

    // Create the size wn as trip_count * sizeOfType
    if ( mbyte_stmt > 1) {
      if (WN_intrinsic(trip_count) >= INTRN_I4DIVFLOOR &&
	  WN_intrinsic(trip_count) <= INTRN_U8DIVCEIL &&
	  WN_operator(WN_kid0(WN_kid1(trip_count))) == OPR_INTCONST &&
	  WN_const_val(WN_kid0(WN_kid1(trip_count))) == mbyte_stmt) {
	length_wn = WN_COPY_Tree(WN_kid0(WN_kid0(trip_count)));
      } else {
	OPCODE mpyopcode = OPCODE_make_op(OPR_MPY, WN_rtype(trip_count),
					  MTYPE_V);
	length_wn = WN_CreateExp2( mpyopcode, WN_COPY_Tree(trip_count),
				   WN_CreateIntconst( OPC_I4INTCONST,
						      mbyte_stmt ) );
      }
    }
    else length_wn = trip_count;

#ifdef KEY
    // 15215: If length_wn is an expression that could sometimes be
    // negative, generate an "length_wn > 0" condition.
    if (! WN_operator_is(trip_count, OPR_INTCONST) &&
	MTYPE_is_signed(WN_rtype(length_wn))) {
      WN *wn0 = NULL, *wn1 = NULL;
      if (WN_operator_is(length_wn, OPR_SUB)) {
	// If Simp_Unsafe_Relops is off, simplify anyway.
	wn0 = WN_COPY_Tree(WN_kid0(length_wn));
	wn1 = WN_COPY_Tree(WN_kid1(length_wn));
      } else {
	wn0 = WN_COPY_Tree(length_wn);
	wn1 = WN_CreateIntconst(OPC_I4INTCONST, 0);
      }
      nzcond = WN_GT(WN_rtype(length_wn), wn0, wn1);
    }
#endif

    desc_idx = WN_ty(stmt); //initialization
    source_idx = WN_ty(stmt); //initialization

    store_addr_wn = WN_COPY_Tree(store_addr_wn);
    if (WN_operator(store_addr_wn) == OPR_ARRAY)
      store_addr_wn = Start_Addr_To_Transform(block, store_addr_wn,
					      desc_idx);
    load_addr_wn = WN_COPY_Tree(load_addr_wn);
    if (WN_operator(load_addr_wn) == OPR_ARRAY)
      load_addr_wn = Start_Addr_To_Transform(block, load_addr_wn,
					     source_idx);

    target_minus_ind = Is_Preceding_Subtract(store_addr_wn, loop_indvar);
    source_minus_ind = Is_Preceding_Subtract(load_addr_wn, loop_indvar);

    // The two should be consistent to be transformed
    if (traceMemlib && target_minus_ind != source_minus_ind) {
      DevWarn("Inconsistent preceding sign for induction");
      return tree;
    }

    // Initialize the induction variable
    OPCODE opc_stid = OPCODE_make_op(OPR_STID, MTYPE_V,
				     TY_mtype(ST_type(WN_st(loop_indvar))));
    if (target_minus_ind) {   //  if minus, set to trip_count-1
      Is_True(trip_count != NULL, ("Memlib: expecting non-null upperbound"));
      OPCODE mpyopcode = OPCODE_make_op(OPR_SUB, WN_rtype(trip_count), MTYPE_V);
      WN *sub_wn = WN_CreateExp2(mpyopcode, WN_COPY_Tree(trip_count),
				 WN_CreateIntconst(OPC_I4INTCONST, 1));
      // Bug 4789
      ind_init_val = WN_CreateStid(opc_stid, WN_idname_offset(loop_indvar),
				   WN_st(loop_indvar), Be_Type_Tbl(MTYPE_I4),
				   sub_wn, 0);
      // to trip_count - 1 if minus induction
    } else {
      // Bug 4789
      ind_init_val = WN_CreateStid(opc_stid, WN_idname_offset(loop_indvar),
				   WN_st(loop_indvar), Be_Type_Tbl(MTYPE_I4),
				   WN_COPY_Tree(lower_bound), 0);
    }
 
    if (ind_init_val) {
      DevWarn("Initial induction variable");
      WN_Set_Linenum(ind_init_val, current_srcpos);
      WN_INSERT_BlockLast(return_block, ind_init_val);
    }

    WN *wn_intrinsic = Transform_To_Memcpy(store_addr_wn, load_addr_wn, 0,
					   desc_idx, source_idx, length_wn);

    DevWarn("TRANSFORMED MEMCPY (%s: %d) of %d\n",
	    Src_File_Name, Srcpos_To_Line(current_srcpos), freq_count);

#ifdef KEY
    // 15215: If length_wn is an expression that could sometimes be
    // negative, wrap an "if (length_wn > 0)" around the MEMCPY.
    if (nzcond) {
      WN *then_block = WN_CreateBlock();
      WN_INSERT_BlockLast(then_block, wn_intrinsic);
      WN *wn_if = WN_CreateIf(nzcond, then_block, WN_CreateBlock());
      wn_if = lower_if(return_block, wn_if, actions);
      WN_INSERT_BlockLast(return_block, wn_if);
    } else
#endif
    {
#ifdef KEY // bug 11360
      wn_intrinsic = lower_intrinsic_call(return_block, wn_intrinsic, actions);
#endif
      WN_INSERT_BlockLast(return_block, wn_intrinsic);
    }
    WN_DELETE_FromBlock(body, stmt);
  } // for

#ifdef KEY
  // 15102: Return the empty loop, so that the induction variable
  // will be properly updated.
  WN_INSERT_BlockLast(block, return_block);
  return tree;
#else
  return return_block;
#endif
}

/* ====================================================================
 * Assignment Support
 * ====================================================================
 */

/*
 * AssignPregName()
 *
 * See interface description.
 */
void
AssignPregName(const char *part1, const char *part2)
{
  static char name[128];
  if (part2 == NULL)
    snprintf(name, sizeof(name), "%s", part1);
  else
    snprintf(name, sizeof(name), "%s%s", part1, part2);
  current_preg_name = name;
}

/*
 * ResetPregName()
 *
 * See interface description.
 */
void
ResetPregName(void)
{
  current_preg_name = NULL;
}

/*
 * CurrentPregName()
 *
 * See interface description.
 */
char *
CurrentPregName(void)
{
  return current_preg_name;
}


