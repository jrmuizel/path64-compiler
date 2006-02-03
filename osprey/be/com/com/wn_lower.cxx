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

#ifdef USE_PCH
#include "be_com_pch.h"
#endif /* USE_PCH */
#pragma hdrstop
#include "W_values.h"
#include <isam.h>
#include "W_alloca.h"
#ifndef __MINGW32__
#include <sys/signal.h>
#else
#include <signal.h>
#endif
#include <elf.h>

#include "defs.h"
#include "config.h"
#include "config_asm.h"
#include "config_debug.h"
#include "config_opt.h"
#include "config_TARG.h"
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

/* My changes are a hack till blessed by Steve. (suneel) */
#define SHORTCIRCUIT_HACK 1

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

#ifdef TARG_ST
extern WN *RT_LOWER_expr (WN *tree);
#endif

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


/* ====================================================================
 *			 Forward Declarations
 * ====================================================================
 */

static WN *lower_scf(WN *, WN *, LOWER_ACTIONS);
static WN *lower_expr(WN *, WN *, LOWER_ACTIONS);
static WN *lower_store(WN *, WN *, LOWER_ACTIONS);
static WN *lower_call(WN *, WN *, LOWER_ACTIONS);
static WN *lower_intrinsic(WN *, WN *, LOWER_ACTIONS);
static WN *lower_intrinsic_call(WN *, WN *, LOWER_ACTIONS);
static WN *lower_intrinsic_op(WN *, WN *, LOWER_ACTIONS);
static WN *lower_if(WN *, WN *, LOWER_ACTIONS);
static WN *lower_stmt(WN *, WN *, LOWER_ACTIONS);
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


static TY_IDX coerceTY(TY_IDX, TYPE_ID);
static ST *coerceST(const ST *, TYPE_ID);
static ST *coerceST(const ST &, TYPE_ID);
static WN_OFFSET coerceOFFSET(WN *, TYPE_ID, WN_OFFSET);


static WN *lower_mload(WN *, WN *, LOWER_ACTIONS);
static void lower_mload_formal(WN *, WN *, PLOC, LOWER_ACTIONS);
#ifdef TARG_ST
static void lower_mload_actual (WN *, WN *, WN *, WN *,PLOC, LOWER_ACTIONS);
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

#ifdef TARG_ST
static WN *lower_return(WN *block, WN *tree, LOWER_ACTIONS actions);
static WN *lower_preamble_end(WN *block, WN *tree, LOWER_ACTIONS actions);
static WN *lower_cst_idiv(WN *block, WN *tree, LOWER_ACTIONS actions);
static WN *lower_cst_udiv(WN *block, WN *tree, LOWER_ACTIONS actions);
static WN *lower_cst_irem(WN *block, WN *tree, LOWER_ACTIONS actions);
static WN *lower_cst_urem(WN *block, WN *tree, LOWER_ACTIONS actions);
#endif

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

#ifdef TARG_ST
// [CG] Map of mtypes to aliasing types for ansi rules
static TY_IDX TY_alias_array[MTYPE_LAST + 1];
#endif

static BOOL traceIO              = FALSE;
static BOOL traceSpeculate       = FALSE;
static BOOL traceAlignment       = FALSE;
static BOOL traceTreeHeight      = FALSE;
static BOOL traceSplitSymOff     = FALSE;
static BOOL traceWoptFinishedOpt = FALSE;
static BOOL traceMload           = FALSE;
// static BOOL traceUplevel = FALSE;
static BOOL traceLdid            = FALSE;
static BOOL traceCall            = FALSE;
static BOOL traceRuntime         = FALSE;

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
  MSTORE_intrinsic_bcopy
} MSTORE_ACTIONS;

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
  if (TY_alias_array[mtype] == 0) {
    TY_alias_array[mtype] = Copy_TY(MTYPE_To_TY (mtype));
    Set_TY_no_ansi_alias(TY_alias_array[mtype]);
  }
  return TY_alias_array[mtype];
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

/* ====================================================================
 *   Promoted_Mtype.
 *
 *   Promote integer subtypes I{1,2} and U{1,2} to I4,U4 type.
 * ====================================================================
 */

static 
TYPE_ID Promoted_Mtype (
  TYPE_ID mtype
)
{
  switch (mtype) {
    case MTYPE_I1: 
    case MTYPE_I2: 
    case MTYPE_I4: 
      return MTYPE_I4;

    case MTYPE_U1:
    case MTYPE_U2:
    case MTYPE_U4:
      return MTYPE_U4;

    case MTYPE_I8: 
    case MTYPE_U8: 
    case MTYPE_I5:
    case MTYPE_U5:
    case MTYPE_A4:
    case MTYPE_A8:
    case MTYPE_F4:
    case MTYPE_F8:
    case MTYPE_FQ:
    case MTYPE_M:
    case MTYPE_C4:
    case MTYPE_C8:
    case MTYPE_CQ:
    case MTYPE_V:
      return mtype;

    default:
      return MTYPE_UNKNOWN;
  }
}

/* ====================================================================
 *			Private macros
 * ====================================================================
 */

#define OPCODE_is_intrinsic(op)                                 	\
		((OPCODE_operator((op)) == OPR_INTRINSIC_CALL) ||       \
		(OPCODE_operator((op)) == OPR_INTRINSIC_OP))

#define	Action(x)			(actions & (x))
#define	NotAction(x)			(Action(x)==0)
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
      {
	return offset + Preg_Increment(realTY);
      }
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

static void rename_preg(char *f, char *preg_class)
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

#if 0
/* ====================================================================
 *   Keep track of the dedicated struct return symbol
 *
 * ==================================================================== */

static ST *struct_return_sym = NULL;

static void
set_struct_return_sym(ST *sym) {
  FmtAssert(struct_return_sym == NULL,
	          ("struct_return_symbol have not been cleared"));
  struct_return_sym = sym;

  //fprintf(stderr, "setting return_sym to %s\n", ST_name(sym));
}

static ST*
get_struct_return_sym() {
  return struct_return_sym;
}

static void 
reset_struct_return_sym() {

  //fprintf(stderr, "resetting return_sym %s\n", ST_name(struct_return_sym));

  struct_return_sym = NULL;
}

#endif

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
 *
 * Create a Nary representation of the expression passed  
 *
 * ==================================================================== */

static WN *WN_Nary(WN *tree, WN *x0, WN *x1, WN *x2, WN *x3)
{
  TYPE_ID	type = WN_rtype(tree);
  OPCODE	op;
  INTRINSIC	id;

  op = OPCODE_make_op (OPR_INTRINSIC_OP, type, MTYPE_V);
 /*
  *  figure out id from tree
  */
  switch(WN_operator(tree))
  {
  case OPR_ADD:
  case OPR_SUB:
    id = INTRN_NARY_ADD;
    break;
  case OPR_MPY:
    id = INTRN_NARY_MPY;
    break;
  default:	
    Is_True(FALSE,("unexpected nary op"));
  }

  {
    WN		*args[4], **kids;
    INT16	i, n, argN;

   /*
    *  if the child is a nary op of the same type, integrate it
    */
    n= argN = 0;
    if (x0)
      args[n++] = x0;
    if (x1)
      args[n++] = x1;
    if (x2)
      args[n++] = x2;
    if (x3)
      args[n++] = x3;

    for(i=0; i<n; i++)
    {
      WN *wn = (WN *) args[i];

      if (WN_opcode(wn) == op && WN_intrinsic(wn) == id)
	argN += WN_kid_count(wn);
      else
        argN++;
    }

    kids = (WN **) alloca(argN * sizeof(WN *));

    for(i= argN= 0; i<n; i++)
    {
      WN *wn = args[i];

      if (WN_opcode(wn) == op && WN_intrinsic(wn) == id)
      {
	INT16  j;

	for(j=0; j< WN_kid_count(wn); j++)
	  kids[argN++] = WN_kid(wn, j);

	WN_Delete(wn);
      }
      else
      {
	kids[argN++] = wn;
      }
    }

    WN_Delete(tree);
    tree = WN_Create_Intrinsic(op, id, argN, kids);
  }
  return tree;
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
 * ==================================================================== */
extern WN *WN_ExprToNaryType(WN *tree, TYPE_ID type, INT count)
{
  WN	*l, *r;

  if (count > 16) {
	Lmt_DevWarn(1,("WN_ExprToNaryType more than 16 deep, so stop"));
	return tree;
  }
  switch(WN_operator(tree))
  {
  case OPR_ADD:
  case OPR_MPY:
    l = WN_kid0(tree);
    r = WN_kid1(tree);

    if ((WN_opcode(tree) == WN_opcode(l)) &&
        (WN_opcode(tree) == WN_opcode(r)))
    {
      WN_kid0(l) = WN_ExprToNaryType(WN_kid0(l), type, count+1);
      WN_kid1(l) = WN_ExprToNaryType(WN_kid1(l), type, count+1);
      WN_kid0(r) = WN_ExprToNaryType(WN_kid0(r), type, count+1);
      WN_kid1(r) = WN_ExprToNaryType(WN_kid1(r), type, count+1);

      tree = WN_Nary(tree, WN_kid0(l), WN_kid1(l), WN_kid0(r), WN_kid1(r));
      WN_Delete(l);
      WN_Delete(r);
    }
    else if (WN_opcode(tree) == WN_opcode(l))
    {
      WN_kid0(l) = WN_ExprToNaryType(WN_kid0(l), type, count+1);
      WN_kid1(l) = WN_ExprToNaryType(WN_kid1(l), type, count+1);

      tree = WN_Nary(tree, WN_kid0(l), WN_kid1(l), r, NULL);
      WN_Delete(l);
    }
    else if (WN_opcode(tree) == WN_opcode(r))
    {
      WN_kid0(r) = WN_ExprToNaryType(WN_kid0(r), type, count+1);
      WN_kid1(r) = WN_ExprToNaryType(WN_kid1(r), type, count+1);

      tree = WN_Nary(tree, WN_kid0(r), WN_kid1(r), l, NULL);
      WN_Delete(r);
    }
    break;

  case OPR_SUB:
    {
      l = WN_kid0(tree);
      r = WN_kid1(tree);

      if (WN_operator_is(l, OPR_ADD))
      {
	TYPE_ID	rtype = WN_rtype(tree);

	WN  *neg = WN_Neg(rtype, r);

	WN_kid0(l) = WN_ExprToNaryType(WN_kid0(l), type, count+1);
	WN_kid1(l) = WN_ExprToNaryType(WN_kid1(l), type, count+1);

	tree = WN_Nary(tree, WN_kid0(l), WN_kid1(l), neg, NULL);
	WN_Delete(l);
      }
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

static PREG_NUM AssignPregExprPos(WN *block, WN *tree, TY_IDX ty,
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

    if (srcpos) {
      WN_Set_Linenum (stid, srcpos);
    }

    stBlock = WN_CreateBlock();

   /*
    *	This lowering may leed to infinite regress if the
    * 	children cannot be lowered (and are allocated a temp, for example) 
    */
    if (actions)
      stid = lower_store(stBlock, stid, actions);

#ifdef TARG_ST
    // [TB] Improve line number.
    WN_copy_linenum(tree, stid);
#endif
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



static BOOL WN_unconditional_goto(WN *tree)
{
  switch(WN_operator(tree))
  {
  case OPR_GOTO:
  case OPR_REGION_EXIT:
  case OPR_RETURN:
  case OPR_RETURN_VAL:
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
 * to the TYs of the new nodes. node1 or node2  might be NULL (for cases in which we
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


/*
 * Describe a leaf expression (see Make_Leaf/Load_Leaf).
 */
typedef enum {LEAF_IS_CONST, LEAF_IS_INTCONST, LEAF_IS_PREG} LEAF_KIND;
typedef struct {
  LEAF_KIND kind;
  TYPE_ID type;
  union {
    PREG_NUM n;
    INT64 intval;
    TCON tc;
  } u;
} LEAF;

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

static LEAF Make_Leaf(WN *block, WN *tree, TYPE_ID type)
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

static WN *Load_Leaf(const LEAF &leaf)
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
  Is_True((MTYPE_is_complex(WN_rtype(tree))),
	  ("expected complex-type node, not %s",
	   OPCODE_name(WN_opcode(tree))));
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
    WN_Delete(tree);
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

      WN_Delete(tree);
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
#ifdef TARG_ST
	  // [TB] Improve line number.
	  WN_copy_linenum(tree, stid);
#endif
	  WN_INSERT_BlockLast(if_else, stid);
    
	  div = WN_Neg(type,WN_Inverse(type, Load_Leaf(scaleN)));
	  stid = WN_StidIntoPreg(type, imagpartN, preg, div);
#ifdef TARG_ST
	  // [TB] Improve line number.
	  WN_copy_linenum(tree, stid);
#endif
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
#ifdef TARG_ST
	    // [TB] Improve line number.
	    WN_copy_linenum(tree, stid);
#endif
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
#ifdef TARG_ST
	    // [TB] Improve line number.
	    WN_copy_linenum(tree, stid);
#endif
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

  if (MTYPE_is_double(orig_type)) type = MTYPE_F4;
  else if (orig_type == MTYPE_I8) type = MTYPE_I4;
  else if (orig_type == MTYPE_U8) type = MTYPE_U4;
  else {
    FmtAssert(FALSE,("can't handle MTYPE_%s", MTYPE_name(orig_type)));
  }
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

// temporary until I modify the .h !!
#define LOWER_HILO (LOWER_DOUBLE|LOWER_LONGLONG)

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
      WN *kids[10];
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

	rename_preg(INTR_intrinsic_name(tree), ".return");
	pregNo = Create_Preg(rtype, current_preg_name);
	rename_reset();

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
      
      // First lower bit extract and than lower hilo
      // Run time lowering may be necessary for IU8 shifts
      WN *extract_wn = lower_load_bits (block, tree, 0);
      extract_wn = RT_LOWER_expr(extract_wn);
      lower_hilo_expr(block, extract_wn, actions, hipart, lopart);
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
      
      // First lower bit extract and than lower hilo
      // Run time lowering may be necessary for IU8 shifts
      WN *extract_wn = lower_load_bits (block, tree, 0);
      extract_wn = RT_LOWER_expr(extract_wn);
      lower_hilo_expr(block, extract_wn, actions, hipart, lopart);
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
      *hipart = WN_Ashr(WN_rtype(WN_kid0(tree)), 
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
  // Null block also means not to delete the node.
  // that is converted (see Get_hilo_home), but not replaced.
  lower_hilo_expr(NULL, 
		  wn,
		  LOWER_HILO,
                  hipart, 
                  lopart);
  return;
}

#endif

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
  INT32		n, i;
  BOOL          is_non_contig=FALSE;
  WN_ESIZE      element_size;

  Is_True((WN_operator_is(tree,OPR_ARRAY)),
	  ("expected ARRAY node, not %s", OPCODE_name(WN_opcode(tree))));
  Is_True(Action(LOWER_ARRAY), ("actions doesn't contain LOWER_ARRAY"));
  Is_True(WN_num_dim(tree) > 0, ("WN_num_dim of ARRAY node not > 0"));

  n = WN_num_dim(tree);
  element_size = WN_element_size(tree);
  if (element_size < 0) {
     is_non_contig = TRUE;
     element_size = -element_size;
  }

#ifdef TARG_ST100
  // Arthur: if we generated the A4/A8 types, we prefer carry out
  //         the array arithmetic in integers as usual and convert
  //         it to the A4/A8 at the top of WHIRL tree.
  // NOTE: ARRAY type can only be A4/A8 or U4/U8 (see opcodes_gen_core.h)
  //
  TYPE_ID orig_rtype = rtype;
  if (rtype == MTYPE_A4) {
    rtype = MTYPE_U4;
  } else if (rtype = MTYPE_A8) {
    rtype = MTYPE_U8;
  }
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
   /*
    *  result <- index[n-1]
    */
    result = WN_Coerce(rtype, WN_array_index(tree, n-1));
     
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
      result = WN_Add(rtype, result, mpy);
    }
  }  
  
#ifdef TARG_ST100

  // we might have generated the A4/A8 type.
  //
  //
  //  result <- base + result * elm_size
  //

  result = WN_Mpy(rtype, result, WN_Intconst(rtype, element_size));

  // The LDA would have the A4/A8 type and we would like to keep
  // it this way. So, convert back to A4/A8 just before adding
  // the base.
  if (orig_rtype != rtype) {
    result = WN_Cvt(rtype, orig_rtype, result);
    rtype = orig_rtype;
  }

  result = WN_Add(rtype, WN_array_base(tree), result);

#else

  /*
   *  result <- base + result * elm_size
   */
  {
    WN  *elm_size;

    elm_size = WN_Intconst(rtype, element_size);
    result = WN_Add(rtype,
		    WN_array_base(tree),
		    WN_Mpy(rtype, result, elm_size));
  }
  
#endif

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
  } else if (src==MTYPE_FQ) {  /* Need to do this this way because there
				* is no quad floor */
     
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

    // [HK] distinguish between FP and Int reassociations
#ifdef TARG_ST
    if (Enable_NaryExpr && ( (Reassoc_Level >= REASSOC_ASSOC && MTYPE_is_float(type)) || (Roundoff_Level >= ROUNDOFF_ASSOC && MTYPE_is_integral(type))))
#else
  if (Enable_NaryExpr && Roundoff_Level >= ROUNDOFF_ASSOC)
#endif
  {

    tree = WN_ExprToNaryType(tree, type, 0);

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

 /*
  *  look for madd patterns
  */
  switch (WN_operator(tree))
  {
  case OPR_NEG:
    {
      WN	*child = WN_kid0(tree);

      switch(WN_operator(child))
      {
      case OPR_MADD:
	WN_Delete(tree);
	return WN_Nmadd(type, WN_kid0(child), WN_kid1(child), WN_kid2(child));

      case OPR_MSUB:
	WN_Delete(tree);
	return WN_Nmsub(type, WN_kid0(child), WN_kid1(child), WN_kid2(child));

      case OPR_NMADD:
	WN_Delete(tree);
	return WN_Madd(type, WN_kid0(child), WN_kid1(child), WN_kid2(child));

      case OPR_NMSUB:
	WN_Delete(tree);
	return WN_Msub(type, WN_kid0(child), WN_kid1(child), WN_kid2(child));
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
	return WN_Madd(type, r, WN_kid0(l), WN_kid1(l));
      }
      else if (WN_operator_is(r, OPR_MPY))
      {
	WN_Delete(tree);
	return WN_Madd(type, l, WN_kid0(r), WN_kid1(r));
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
	return WN_Msub(type, r, WN_kid0(l), WN_kid1(l));
      }
      else if (WN_operator_is(r, OPR_MPY))
      {
	WN_Delete(tree);
	return WN_Nmsub(type, l, WN_kid0(r), WN_kid1(r));
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
	return WN_Msub(type, child, WN_kid1(tree), WN_kid2(tree));
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
	return WN_Madd(type, child, WN_kid1(tree), WN_kid2(tree));
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
	return WN_Nmsub(type, child, WN_kid1(tree), WN_kid2(tree));
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
	return WN_Nmadd(type, child, WN_kid1(tree), WN_kid2(tree));
      }
    }
    break;
  }

  return tree;
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


  if (Recip_Allowed == FALSE)
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

  if (Rsqrt_Allowed == FALSE || MTYPE_is_quad(type))
  {
    WN	*div;

    div = WN_Div(type, WN_Floatconst(type, 1.0), WN_Sqrt(type, WN_kid0(tree)));
    WN_Delete(tree);

    return div;
  }
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

#if defined(TARG_ST100) || defined(TARG_ST200)
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

    if (ST_gprel(sym))
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
    deref = WN_RIload(WN_rtype(tree), WN_desc(tree), offset, WN_ty(tree), addr);
    break;

  case OPR_STID:
   /*
    * Convert  STID () <sym>
    * into
    *       LDID (preg) <base> 
    *     ISTORE (offset)          
    */
    deref = WN_Istore(WN_desc(tree), offset, addr_ty, addr, WN_kid0(tree));
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

  Is_True((WN_operator(tree) == OPR_LDID),
	  ("expected LDID node, not %s", OPCODE_name(WN_opcode(tree))));

  if (traceLdid) {
    fputs(DBar, TFile);
    fprintf(TFile, "Return LDID Lower: Before Lowering\n");
    fdump_tree(TFile, tree);
  }

  switch (mtype) {

    case MTYPE_I8:
    case MTYPE_U8:

      WN_st_idx(tree) = ST_st_idx(MTYPE_To_PREG(mtype));
      WN_load_offset(tree) = First_Int_Preg_Return_Offset;
      break;

    case MTYPE_I1:
    case MTYPE_I2:
    case MTYPE_I4:
    case MTYPE_I5:
    case MTYPE_U1:
    case MTYPE_U2:
    case MTYPE_U4:
    case MTYPE_U5:

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
    case MTYPE_C4:
    case MTYPE_C8:
    case MTYPE_CQ:
      WN_st_idx(tree) = ST_st_idx(Float_Preg);
      WN_load_offset(tree) = First_Float_Preg_Return_Offset;
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
#else
      WN_st_idx(tree) = ST_st_idx(Ptr_Preg);
      WN_load_offset(tree) = First_Ptr_Preg_Return_Offset;
#endif
      break;

    case MTYPE_M:
      Fail_FmtAssertion ("MLDID of Return_Val_Preg not allowed in middle"
			 " of expression");
      /*NOTREACHED*/
    default:
      Fail_FmtAssertion ("Unexpected type in lower_return_ldid");
      /*NOTREACHED*/
  }

  if (traceLdid) {
    fprintf(TFile, "\nReturn LDID Lower: After Lowering\n");
    fdump_tree(TFile, tree);
    fputs(DBar, TFile);
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
  wn  = lower_expr (block, wn, actions);

#ifdef TARG_ST
  // [TB] Improve line number.
  WN_copy_linenum(tree, wn);
#endif
  WN_Delete (tree);
  return wn;
}

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
  INT bit_ofst = Target_Byte_Sex == BIG_ENDIAN ?
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

  return lower_expr (block, tree, actions);
} // lower_load_bits


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
  
  INT shift = Target_Byte_Sex == BIG_ENDIAN ?
      MTYPE_bit_size (WN_desc (wn)) - bit_ofst - bit_size :
      bit_ofst;
  mUINT64 mask = ~((~((mUINT64)0) >> (64 - bit_size)) << shift);
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
  mask = ~((mUINT64)0) >> (64 - bit_size);
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
    if (opr == OPR_ILOAD) {
      ty_idx = WN_load_addr_ty(wn);
      new_opr = OPR_ILDBITS;
    } else if (opr == OPR_ISTORE) {
      ty_idx = WN_ty(wn);
      new_opr = OPR_ISTBITS;
    } else {
      Is_True(0, ("lower_bit_field_id(): unexpected operator %s", 
		  OPERATOR_name(opr)));
    }
    Is_True(TY_kind(ty_idx) == KIND_POINTER,
	    ("lower_bit_field_id(): addr ty not pointer type for %s", 
	     OPERATOR_name(opr)));
    struct_ty_idx = TY_pointed(ty_idx);
    indirect = TRUE;
  }
  Is_True(TY_kind(struct_ty_idx) == KIND_STRUCT,
	  ("struct type not associated with bit-field access for %s", 
	   OPERATOR_name(opr)));
  
  UINT cur_field_id = 0;
  UINT64 field_offset = 0;
  FLD_HANDLE fld = FLD_And_Offset_From_Field_Id(struct_ty_idx, 
                                                WN_field_id(wn),
                                                cur_field_id,
                                                field_offset); 
  Is_True(! fld.Is_Null(),
	  ("invalid bit-field ID for %s", OPERATOR_name(opr)));
  Is_True(FLD_is_bit_field(fld),
	  ("non-bit-field associated with bit-field access for  %s", 
	   OPERATOR_name(opr)));

  TY_IDX fld_ty_idx = FLD_type(fld);
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
  // This is the size in byte of the bit field type declaration
  UINT bytes_accessed = TY_size(fld_ty_idx);
  if (OPERATOR_is_store(new_opr))
    rtype = TY_mtype(fld_ty_idx);
  else rtype = WN_rtype(wn);

  INT ofst = field_offset;
#if 0
  BOOL unaligned_field = check_unaligned(bytes_accessed * 8, ofst,
					 TY_align(struct_ty_idx));
#endif
  // Round ofst toward -inf to a multiple of byte_accessed
  if (ofst >= 0)
    ofst = ofst / bytes_accessed * bytes_accessed;
  else 
    ofst =  (ofst - bytes_accessed + 1) / bytes_accessed * bytes_accessed;

  // Compute size and offset into byte_accessed
  UINT bsize = FLD_bsize(fld);
  UINT bofst = FLD_bofst(fld) + (field_offset-ofst) * 8;
  if ((bofst + bsize) > (bytes_accessed * 8)) {
    if (bytes_accessed == MTYPE_byte_size(Max_Int_Mtype)) { 
      // can't enlarge; reverse the adjustment
      ofst = field_offset;
      bofst = FLD_bofst(fld);
    }
    else bytes_accessed *= 2;
  }
  WN_load_offset(wn) = WN_load_offset(wn) + ofst; 

  // Optimizations of byte level accesses
#ifdef TARG_ST
  // [CG]: Optimization of memory access must be disabled
  // when crossing the Max_Int_Mtype size as
  // it would necessitate a CVT operator.
  if ((bsize & 7) == 0 && 		   // field size multiple of bytes
      (bytes_accessed * 8 % bsize) == 0 && // bytes_accessed multiple of bsize
      (bofst % bsize) == 0 &&		   // bofst multiple of bsize
      // avoid CVT generation
      !(MTYPE_bit_size(rtype) > MTYPE_bit_size(Max_Int_Mtype) &&
	bsize <= MTYPE_bit_size(Max_Int_Mtype)))
#else
  if ((bsize & 7) == 0 && 		   // field size multiple of bytes
      (bytes_accessed * 8 % bsize) == 0 && // bytes_accessed multiple of bsize
      (bofst % bsize) == 0) 		   // bofst multiple of bsize
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
    TYPE_ID new_desc = Mtype_AlignmentClass(bsize >> 3, MTYPE_type_class(rtype));
    WN_set_desc(wn, new_desc);
    WN_load_offset(wn) = WN_load_offset(wn) + (bofst >> 3);
  } else { // generate lowered-to bit-field operator
    // [CG]: Unclear benefit, disabled it.
#if defined(TARG_MIPS) || defined(TARG_IA32) /* || defined(TARG_ST200) */
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
    WN_set_desc(wn, Mtype_AlignmentClass(bytes_accessed, MTYPE_type_class(rtype)));
    if (OPERATOR_is_load(new_opr) && 
	MTYPE_byte_size(WN_rtype(wn)) < bytes_accessed)
      WN_set_rtype(wn, WN_desc(wn));
    WN_set_bit_offset_size(wn, bofst, bsize);
  }
}

static void lower_trapuv_alloca (WN *block, WN *tree, LOWER_ACTIONS actions
#ifdef TARG_ST
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
#endif
	  con = WN_UVConst(WN_rtype(size));
	WN *mstore;
	if (WN_operator(size) == OPR_INTCONST && WN_const_val(size) == 0)
		return;	// nothing to do

#ifdef TARG_ST
	// TB: Code to fix a bug -trapuv (from PathScale 2.1)
	ST *sym = &St_Table[sym_idx];
	mstore = WN_CreateMstore(0,
		pty_idx,		
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
  	mstore = lower_store (block, mstore, actions);
#ifdef TARG_ST
	// [TB] copy linenumber.
	WN_copy_linenum(tree, mstore);
#endif
	WN_INSERT_BlockLast(block, mstore);
}

#ifndef TARG_ST
// This are handled in rt_lower_wn.cxx

inline BOOL Should_Call_Divide(TYPE_ID rtype)
{
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
#endif

static BOOL Is_Fast_Divide(WN *wn)
{
  OPERATOR opr = WN_operator(wn);
  switch (opr) {
  case OPR_DIV:
  case OPR_REM:
  case OPR_MOD:
    {
	if (WN_operator(WN_kid1(wn)) == OPR_LDID &&
	    WN_class(WN_kid1(wn)) == CLASS_PREG &&
	    Preg_Home(WN_offset(WN_kid1(wn))) != NULL &&
	    WN_operator_is(Preg_Home(WN_offset(WN_kid1(wn))), OPR_INTCONST)) {
	    TYPE_ID rtype = OPCODE_rtype(WN_opcode(wn));
	    INT64 constval = WN_const_val(Preg_Home(WN_offset(WN_kid1(wn))));
	    return  opr == OPR_DIV
	       ? Can_Do_Fast_Divide(rtype, constval)
	       : Can_Do_Fast_Remainder(rtype, constval);
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

static BOOL Is_Const_Divide(WN *wn)
{
  OPERATOR opr = WN_operator(wn);
  switch (opr) {
  case OPR_DIV:
  case OPR_REM:
  case OPR_MOD:
    {
	if (WN_operator(WN_kid1(wn)) == OPR_LDID &&
	    WN_class(WN_kid1(wn)) == CLASS_PREG &&
	    Preg_Home(WN_offset(WN_kid1(wn))) != NULL &&
	    WN_operator_is(Preg_Home(WN_offset(WN_kid1(wn))), OPR_INTCONST) && 
	    (WN_const_val(Preg_Home(WN_offset(WN_kid1(wn)))) != 0)) {
	    return TRUE;
	}
	    
	
	if (WN_operator_is(WN_kid1(wn), OPR_INTCONST) && (WN_const_val(WN_kid1(wn)) != 0) ) {
	    return TRUE;
	}
    }
    break;
  }

  return FALSE;
}

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
#endif /* TARG_ST

/* ====================================================================
 *
 * WN *lower_expr(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on expression <tree>,
 * returning lowered expression tree.
 *
 * ==================================================================== */

static WN *lower_expr(WN *block, WN *tree, LOWER_ACTIONS actions)
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
  if (Action(LOWER_MADD)	&&
      Madd_Allowed		&&
#ifdef TARG_ST200 // [HK] madd not supported for double type in st200
      Fused_Madd		&&
      (MTYPE_id(type) == MTYPE_F4))
#else
     (MTYPE_id(type) == MTYPE_F4 || MTYPE_id(type) == MTYPE_F8))
#endif 
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

#ifdef TARG_ST
    //
    // Arthur: if we're not inlining intrinsics, do nothing - it will
    //         be converted into a function call at LOWER_TO_CG time.
    //
    if (Inline_Intrinsics_Allowed && INTRN_cg_intrinsic(WN_intrinsic(tree))) {
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
      lower_bit_field_id(tree);
      if (Action(LOWER_BITS_OP) && WN_operator(tree) == OPR_ILDBITS) {
	return lower_load_bits (block, tree, actions);
      }
	
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

      if (foldLdaOffset(kid, WN_load_offset(tree)))
      {
	WN_load_offset(tree) += WN_lda_offset(kid);
	WN_lda_offset(kid) = 0;
      }
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
    }
    break;

  case OPR_LDID:

    if (Action(LOWER_RETURN_VAL) && WN_st(tree) == Return_Val_Preg)
      return lower_return_ldid(block, tree, actions);

    if (Action(LOWER_MLDID_MSTID) && WN_opcode(tree) == OPC_MMLDID)
      return lower_mldid(block, tree, actions);

    if (Action(LOWER_BIT_FIELD_ID) && WN_desc(tree) == MTYPE_BS) {
      lower_bit_field_id(tree);
      if (Action(LOWER_BITS_OP) && WN_operator(tree) == OPR_LDBITS)
	return lower_load_bits (block, tree, actions);
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
    if (Action(LOWER_BITS_OP))
      return lower_load_bits (block, tree, actions);
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
     */
    if (Action(LOWER_TO_CG))
    {
      WN *load = WN_kid0(tree);
      if (   OPERATOR_is_load(WN_operator(load))
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
      if (Action(LOWER_HILO) && is_hilo_type(WN_rtype(WN_kid0(tree)))) {
	WN_kid0(tree) = RT_LOWER_expr (WN_NE (WN_rtype(WN_kid0(tree)),
				     WN_kid0(tree),
				     WN_Zerocon(WN_rtype(WN_kid0(tree)))));
      }
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
    }
    break;

  case OPR_PAREN:
    // [HK] distinguish between FP and Int reassociations
#ifdef TARG_ST
    if ((MTYPE_is_integral(type) && Roundoff_Level > ROUNDOFF_ASSOC)
	|| (MTYPE_is_float(type) && Reassoc_Level > REASSOC_ASSOC))
#else
    if (Roundoff_Level > ROUNDOFF_ASSOC)
#endif
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
    //
    // Arthur: the ST targets undergo a special rt_lower pass which
    //         decides whether it is necessary to turn it into an
    //         intrinsic
    //

    // [HK] perform division by constant optimization for I4 and U4
  if ( Action(LOWER_CST_DIV) &&
      Enable_Cst_Div && 
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
  
  case OPR_REM:

    // [HK] perform division by constant optimization for I4 and U4
  if ( Action(LOWER_CST_DIV) &&
      Enable_Cst_Div && 
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

  case OPR_MOD:
      
#else
  case OPR_DIV:
  case OPR_REM:
  case OPR_MOD:
    {

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
  }

  
  if (Action(LOWER_QUAD))
  {
    if (WN_desc(tree) == MTYPE_FQ)
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
    if (WN_rtype(tree) == MTYPE_FQ)
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
  TYPE_ID mtype;
  ST *preg_st;
  WN *n_rhs;
  WN *wn = NULL;	// init to prevent upward-exposed use
  RETURN_INFO return_info = Get_Return_Info(WN_ty(tree), Complex_Not_Simulated);

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

    Is_True(WN_rtype(call) == MTYPE_M,
	    ("call preceding MMLDID of Return_Val_Preg not type M"));
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

#if 0
  else if (RETURN_INFO_return_via_return_reg(return_info)) { 
    // get the previous MCALL statement
    WN *call = WN_last(block);
    Is_True(WN_operator(call) == OPR_CALL || WN_operator(call) == OPR_ICALL ||
	    WN_operator(call) == OPR_PICCALL,
	    ("statement preceding MMLDID of Return_Val_Preg must be a call"));
    Is_True(WN_rtype(call) == MTYPE_M,
	    ("call preceding MMLDID of Return_Val_Preg not type M"));

    //
    // add a STID into the dedicated preg
    //
    WN *awn = WN_CreateLda(OPR_LDA, Pointer_Mtype, MTYPE_V, 
			   WN_store_offset(tree), 
			   Make_Pointer_Type(WN_ty(tree)), WN_st_idx(tree));
    awn = lower_expr(block, awn, actions);
    ST *regST = MTYPE_To_PREG(Pointer_Mtype);
    WN *stid = WN_StidIntoPreg(Pointer_Mtype, 
		      Struct_Return_Preg_Offset,
		      regST,
		      awn);
    WN_Set_Linenum(stid, WN_Get_Linenum(call));
    WN_INSERT_BlockLast(block, stid);

    // 
    // replace original CALL by the same but with V type
    //
    WN *n_call;
    if (WN_operator(call) == OPR_ICALL)
      n_call = WN_Icall(MTYPE_V, MTYPE_V, WN_kid_count(call), WN_ty(call));
    else
      n_call = WN_generic_call(WN_operator(call), MTYPE_V, MTYPE_V, 
			              WN_kid_count(call), WN_st_idx(call));
    WN_call_flag(n_call) = WN_call_flag(call);
    WN_Set_Linenum(n_call, WN_Get_Linenum(call));
    if ( Cur_PU_Feedback ) {
      Cur_PU_Feedback->FB_lower_call(call, n_call);
    }
    for (INT32 i = 0; i < WN_kid_count(call); i++)
      WN_kid(n_call, i) = WN_COPY_Tree(WN_kid(call, i));

    //
    // Insert in block
    //
    WN_DELETE_FromBlock(block, call);
    WN_INSERT_BlockLast(block, n_call); 

    WN_DELETE_Tree(tree);
    return NULL; // original MSTID disappears
  }
#endif
  else { // return via 1 or more return registers
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
      if (TY_align(ST_type(WN_st(tree))) < MTYPE_alignment(mtype)) {
	DevWarn("return_info struct alignment is smaller than register size, may produce wrong results");
      }
      wn = WN_CreateStid(OPR_STID, MTYPE_V, mtype, 
		         WN_store_offset(tree)+i*MTYPE_byte_size(mtype),
			 WN_st_idx(tree), Be_Type_Tbl(mtype), n_rhs);
      wn  = lower_store (block, wn, actions);
      WN_Set_Linenum (wn, WN_Get_Linenum(tree));
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
  RETURN_INFO return_info = Get_Return_Info(WN_ty(tree), Complex_Not_Simulated);

  if (RETURN_INFO_return_via_first_arg(return_info)) { // fake first parm
    // get the previous MCALL statement
    WN *call = WN_last(block);
    Is_True(WN_operator(call) == OPR_CALL || WN_operator(call) == OPR_ICALL ||
	    WN_operator(call) == OPR_PICCALL,
	    ("statement preceding MMLDID of Return_Val_Preg must be a call"));
    Is_True(WN_rtype(call) == MTYPE_M,
	    ("call preceding MMLDID of Return_Val_Preg not type M"));
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

#if 0
  else if (RETURN_INFO_return_via_return_reg(return_info)) { 
    // get the previous MCALL statement
    WN *call = WN_last(block);
    Is_True(WN_operator(call) == OPR_CALL || WN_operator(call) == OPR_ICALL ||
	    WN_operator(call) == OPR_PICCALL,
	    ("statement preceding MMLDID of Return_Val_Preg must be a call"));
    Is_True(WN_rtype(call) == MTYPE_M,
	    ("call preceding MMLDID of Return_Val_Preg not type M"));

    FmtAssert(FALSE,("not implemented"));
#if 0
    //
    // add a STID into the dedicated preg
    //
    WN *awn = WN_CreateLda(OPR_LDA, Pointer_Mtype, MTYPE_V, 
			   WN_store_offset(tree), 
			   Make_Pointer_Type(WN_ty(tree)), WN_st_idx(tree));
    awn = lower_expr(block, awn, actions);
    ST *regST = MTYPE_To_PREG(Pointer_Mtype);
    WN *stid = WN_StidIntoPreg(Pointer_Mtype, 
		      RETURN_INFO_preg(return_info, 0),
		      regST,
		      awn);
    WN_Set_Linenum(stid, WN_Get_Linenum(call));
    WN_INSERT_BlockLast(block, stid);
#endif
    // 
    // replace original CALL by the same but with V type
    //
    WN *n_call;
    if (WN_operator(call) == OPR_ICALL)
      n_call = WN_Icall(MTYPE_V, MTYPE_V, WN_kid_count(call), WN_ty(call));
    else
      n_call = WN_generic_call(WN_operator(call), MTYPE_V, MTYPE_V, 
			              WN_kid_count(call), WN_st_idx(call));
    WN_call_flag(n_call) = WN_call_flag(call);
    WN_Set_Linenum(n_call, WN_Get_Linenum(call));
    if ( Cur_PU_Feedback ) {
      Cur_PU_Feedback->FB_lower_call(call, n_call);
    }
    for (INT32 i = 0; i < WN_kid_count(call); i++)
      WN_kid(n_call, i) = WN_COPY_Tree(WN_kid(call, i));

    //
    // Insert in block
    //
    WN_DELETE_FromBlock(block, call);
    WN_INSERT_BlockLast(block, n_call); 

    WN_DELETE_Tree(tree);
    return NULL; // original MSTID disappears
  }
#endif /* TARG_ST */

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
			   Be_Type_Tbl(mtype), n_rhs, base_expr);
      wn  = lower_store (block, wn, actions);
      WN_Set_Linenum (wn, WN_Get_Linenum(tree));
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

  awn = WN_CreateLda (OPR_LDA, Pointer_Mtype, MTYPE_V, WN_store_offset(tree),
		      pty_idx, WN_st(tree));
  swn = WN_CreateIntconst (OPC_U4INTCONST, size);
  wn  = WN_CreateMstore (0, pty_idx, WN_kid0(tree), awn, swn);
  WN_set_field_id(wn, WN_field_id(tree));
  wn  = lower_store (block, wn, actions);

#ifdef TARG_ST
  WN_Set_Linenum(wn, WN_Get_Linenum(tree));
#endif
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
			WN_COPY_Tree(WN_kid0(tree)),
                        WN_COPY_Tree(WN_kid1(tree)), swn);
  WN_set_field_id(wn, WN_field_id(tree));
  wn  = lower_store(block, wn, actions);

#ifdef TARG_ST
  WN_Set_Linenum(wn, WN_Get_Linenum(tree));
#endif
  WN_DELETE_Tree (tree);
  return wn;
}

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

    if (Action(LOWER_BIT_FIELD_ID) && WN_desc(tree) == MTYPE_BS) {
      lower_bit_field_id(tree);
      if (Action(LOWER_BITS_OP) && WN_operator (tree) == OPR_ISTBITS)
	return lower_store_bits (block, tree, actions);
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

      if (foldLdaOffset(addr_kid, WN_store_offset(tree)))
      {
	WN_store_offset(tree) += WN_lda_offset(addr_kid);
	WN_lda_offset(addr_kid) = 0;
      }
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
    }
    break;

  case OPR_STID:
    {
      PREG_NUM last_preg = Get_Preg_Num (PREG_Table_Size(CURRENT_SYMTAB));

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

    if (Action(LOWER_BIT_FIELD_ID) && WN_desc(tree) == MTYPE_BS) {
      lower_bit_field_id(tree);
      if (Action(LOWER_BITS_OP) && WN_operator (tree) == OPR_STBITS)
	return lower_store_bits (block, tree, actions);
    }

    if (Action(LOWER_COMPLEX) && MTYPE_is_complex(WN_desc(tree)))
    {
      WN	*realexp, *imagexp;
      TYPE_ID	realTY;

      realTY =	Mtype_complex_to_real( WN_desc(tree));
      lower_complex_expr(block, WN_kid0(tree), actions, &realexp, &imagexp);

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

#ifdef TARG_ST

  case OPR_ISTBITS:

#if 0 // this was never active anyway ??
    if (Action(LOWER_BITS_OP))
      return lower_store_bits (block, tree, actions);
#endif

    if (Action(LOWER_HILO) && 
#ifdef NEW_LOWER
	is_hilo_type(WN_desc(tree))
#else
	(MTYPE_is_double(WN_desc(tree)) ||
	 MTYPE_is_longlong(WN_desc(tree)))
#endif
	)
    {
      // For HILO lowering of STBITS, we first
      // lower store bits, than we lower the store
      // note that run time lowering may be necessary in-between
      // for U8 shifts.
      WN *extract_wn = lower_store_bits(block, tree, 0);
      RT_lower_wn(extract_wn);
      return lower_store(block, extract_wn, actions);
    }

  case OPR_STBITS:

#if 0 // Arthur: this was never active
    if (Action(LOWER_BITS_OP))
      return lower_store_bits (block, tree, actions);
#endif

    if (Action(LOWER_HILO) && 
#ifdef NEW_LOWER
	is_hilo_type(WN_desc(tree))
#else
	(MTYPE_is_double(WN_desc(tree)) ||
	 MTYPE_is_longlong(WN_desc(tree)))
#endif
	)
    {
      // For HILO lowering of STBITS, we first
      // lower store bits, than we lower the store
      // note that run time lowering may be necessary in-between
      // for U8 shifts.
      WN *extract_wn = lower_store_bits(block, tree, 0);
      RT_lower_wn(extract_wn);
      return lower_store(block, extract_wn, actions);
    }

#else /* not TARG_ST */

  case OPR_ISTBITS:
  case OPR_STBITS:
    if (Action(LOWER_BITS_OP))
      return lower_store_bits (block, tree, actions);

#endif

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
#ifdef TARG_ST
      // FdF 29/06/2004
      WN *rhs = WN_kid0(tree);
      if (WN_StoreIsUnused(WN_kid1(tree)) ||
	  (WN_operator(rhs) == OPR_MLOAD &&
	   WN_load_offset(rhs) == WN_store_offset(tree) && // MSTID/MLOAD
	   WN_operator(WN_kid0(rhs)) == OPR_LDA && WN_Equiv(WN_kid0(rhs), WN_kid1(tree)) && // LDA
	   WN_operator(WN_kid1(rhs)) == OPR_INTCONST && WN_Equiv(WN_kid1(rhs), WN_kid2(tree)))) // INTCONST
#else
      if (WN_StoreIsUnused(WN_kid1(tree)))
#endif      
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

  if (Action(LOWER_COMPLEX) && MTYPE_is_complex(WN_rtype(child)))
  {
    WN	*realexp, *imagexp, *eval;

    lower_complex_expr(block, child, actions, &realexp, &imagexp);

    realexp = lower_expr(block, realexp, actions);
    eval = WN_CreateEval(realexp);
#ifdef TARG_ST
    // CL: keep line numbers for debug
    WN_Set_Linenum(eval, WN_Get_Linenum(tree));
#endif
    WN_INSERT_BlockLast(block, eval);

    child = imagexp;
  }
#ifdef TARG_ST
  else if (Action(LOWER_HILO) && 
#ifdef NEW_LOWER
	   is_hilo_type(WN_rtype(child))
#else
	   (MTYPE_is_double(WN_rtype(child)) ||
	    MTYPE_is_longlong(WN_rtype(child)))
#endif
	   ) {

    WN	*hi, *lo, *eval;

    lower_hilo_expr(block, child, actions, &hi, &lo /*, WN_rtype(child)*/);

    hi = lower_expr(block, hi, actions);
    eval = WN_CreateEval(hi);

    // CL: keep line numbers for debug
    WN_Set_Linenum(eval, WN_Get_Linenum(tree));

    WN_INSERT_BlockLast(block, eval);

    child = lo;
  }
#endif
  else if (Action(LOWER_QUAD) && MTYPE_is_quad(WN_rtype(child)))
  {
    WN	*hi, *lo, *eval;

    lower_quad_expr(block, child, actions, &hi, &lo);

    hi = lower_expr(block, hi, actions);
    eval = WN_CreateEval(hi);

#ifdef TARG_ST
    // CL: keep line numbers for debug
    WN_Set_Linenum(eval, WN_Get_Linenum(tree));
#endif

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
#ifdef TARG_ST
    // CL: keep line numbers for debug
    WN_Set_Linenum(eval, WN_Get_Linenum(tree));
#endif
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
      action = MSTORE_intrinsic_bcopy;
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
	      "size %lld, nMoves %d",
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
  if (TY_size (struct_type) != MTYPE_byte_size (mtype)) {
#ifdef TARG_ST
    // [CG]: We have to force non applicability of ansi aliasing rules
    // as we lower an aggregate into parts.
    Set_TY_IDX_index (struct_type, TY_IDX_index(MTYPE_To_TY_Alias(mtype)));
#else
    Set_TY_IDX_index (struct_type, TY_IDX_index(MTYPE_To_TY (mtype)));
#endif
  }
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




/* ====================================================================
 *
 * Auxillary routine to copy aggregrate loop 
 *
 *	The size must be an integer constant
 *
 * ==================================================================== */
static void
copy_aggregate_loop_const(WN *block, TY_IDX srcAlign, TY_IDX dstAlign,
			  INT32 offset, INT32 size, TYPE_ID quantum,
			  PREG_NUM srcPreg, PREG_NUM dstPreg, WN *origLoad,
			  WN *origStore, INT32 copy_alignment,
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




/* ====================================================================
 *
 * Auxillary routine to copy aggregrate loop 
 *
 *	The size must be an expression
 *
 * ==================================================================== */

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
copy_aggregate_loop(WN *block, TY_IDX srcTY, TY_IDX dstTY, INT32 offset,
		    WN *size, TYPE_ID quantum, ST *preg, PREG_NUM srcPreg,
		    PREG_NUM dstPreg, WN *origLoad, WN *origStore,
		    INT32 copy_alignment, LOWER_ACTIONS actions)
{
  if (WN_operator_is(size, OPR_INTCONST))
  {
    if (WN_const_val(size)>0)
    {
      copy_aggregate_loop_const(block,
				srcTY, dstTY,
				offset,
				WN_const_val(size),
				quantum,
				srcPreg, dstPreg,
				origLoad, origStore,
				copy_alignment,
				actions);
    }
  }
  else
  {
    copy_aggregate_loop_n(block,
			  srcTY, dstTY,
			  offset,
			  size,
			  quantum,
			  srcPreg,
			  dstPreg,
			  origLoad, origStore,
			  copy_alignment,
			  actions);
  }
}

/*
 *  mark LDA as addr taken passed for intrinsic memory routines.
 *  Do not recurse on ILOAD (it would mark the pointer as taken_passed
 *  and not the object)
 */
static void markAddrTakenPassed(WN *expr)
{
  switch(WN_operator(expr))
  {
  case OPR_LDA:
    {
      ST *sym = WN_st(expr);
      // if (ST_class(sym) == CLASS_VAR || ST_class(sym) == CLASS_FUNC) {
      //   Do nothing here. ADDR flags should only grow more
      //   optimistic; they should never become more conservative,
      //   because the program's semantics cannot grow worse as we
      //   compile it.
      // }
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

#ifdef TARG_ST
  // [TB] Improve line number.
  WN_copy_linenum(dst, call);
#endif
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

#ifdef TARG_ST
  // [TB] Improve line number.
  WN_copy_linenum(dst, call);
#endif
  WN_INSERT_BlockLast(block, call);
}

#ifdef TARG_ST
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

  call=	WN_Generate_Intrinsic_Call(block, INTRN_MEMCPY, 3, parms);

#ifdef TARG_ST
  // [TB] Improve line number.
  WN_copy_linenum(dst, call);
#endif
  WN_INSERT_BlockLast(block, call);
}

#else

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

  Is_True((compute_offset_alignment(offset, align) == align),
	  ("compute_copy_alignment: alignment not consistent with offset"));

#ifdef TARG_ST
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

  if (traceMload) {
    fprintf(TFile, "--------- lower_mstore -----------\n");
    fdump_tree(TFile, mstore);
    fprintf(TFile, "----------------------------------\n");
  }

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
      srcTY = TY_pointed(Ty_Table[WN_ty(load)]);
      expr =	WN_kid0(load);
     /*
      *  the mload may have an offset
      */
      if (WN_load_offset(load))
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
        srcPreg = AssignExpr(newblock, expr, WN_rtype(expr));
        dstPreg = AssignExpr(newblock, addr, Pointer_type);
	copy_aggregate_loop(newblock,
			    srcTY,
			    dstTY,
			    0,
			    size,
			    quantum,
			    preg,
			    srcPreg,
			    dstPreg,
			    load,
			    mstore,
			    copy_alignment,
			    actions);
	break;

      case MSTORE_aggregate:
        srcPreg = AssignExpr(newblock, expr, WN_rtype(expr));
        dstPreg = AssignExpr(newblock, addr, Pointer_type);
	copy_aggregate(newblock,
		       srcTY,
		       dstTY,
		       0,
      		       WN_const_val(size),
		       quantum,
		       preg,
		       srcPreg,
		       dstPreg,
		       load,
		       mstore,
		       copy_alignment,
		       actions);
	break;
      }
    }
  }

  WN_Delete( WN_kid2(mstore));
  WN_Delete(mstore);

  newblock = lower_block(newblock, actions);

  if (traceMload) {
    fprintf(TFile, "--------- Expanded Mstore --------\n");
    fdump_tree(TFile, newblock);
    fprintf(TFile, "----------------------------------\n");
  }

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
 * ==================================================================== 
 */
static void 
lower_complex_actual (
  WN *block, 
  WN *val, 
  PLOC ploc,
  LOWER_ACTIONS actions
)
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


#ifdef TARG_ST
// Arthur: Some architectures (eg. ST100) need this to be calculated
//         rather than specified once in the targ description.
static INT _Formal_Save_Area_Size;
#define FIXED_FORMAL_AREA_SIZE	
#endif

/* ====================================================================
 *
 * void lower_mload_actual
 *
 * Perform lowering (see WN_Lower description) for structure actuals
 * the structure will be copied to [ regs ] and [ stack ]
 *
 *
 * ==================================================================== 
 */
static void 
lower_mload_actual (
  WN *block, 
#ifdef TARG_ST
  WN *memcpyblock,
  WN *mparm,
#endif
  WN *mload,
  PLOC ploc,
  LOWER_ACTIONS actions
)
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
#ifdef TARG_ST
  //
  // Arthur: We need to keep track of the _Formal_Save_Area_Size
  //
  INT last_ploc_ofst = PLOC_offset(ploc)-PLOC_lpad(ploc);
#endif
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
#ifdef FIXED_FORMAL_AREA_SIZE // CG
	offset = PLOC_offset(ploc) - Formal_Save_Area_Size
	  + STACK_OFFSET_ADJUSTMENT - mloadOffset; 
#else
	offset = PLOC_offset(ploc) - _Formal_Save_Area_Size
	  + STACK_OFFSET_ADJUSTMENT - mloadOffset; 
#endif
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
			      preg,
			      addrN,
			      dstPreg,
			      mload,
#ifdef TARG_ST
			      mparm,
#else
			      NULL,
#endif
			      copy_alignment,
			      actions);
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
			 preg,
			 addrN,
			 dstPreg,
			 mload,
#ifdef TARG_ST
			 mparm,
#else
			 NULL,
#endif
			 copy_alignment,
			 actions);
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
		// need to copy a smaller size than default reg-size
		DevWarn("actual_mload: switch from mtype_f8 to mtype_f4");
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

	  if (Target_Byte_Sex == BIG_ENDIAN) {
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

#ifdef TARG_ST
	// [TB] Improve line number.
	WN_copy_linenum(mload, stid);
#endif
	WN_INSERT_BlockLast(block, stid);
      }
    }
    mloadOffset += PLOC_size(ploc);
#ifdef TARG_ST
    // Arthur: update _Formal_Save_Area_Size
    if (!PLOC_on_stack(ploc)) {
      _Formal_Save_Area_Size += PLOC_total_size(ploc) - last_ploc_ofst;
    }
    last_ploc_ofst = PLOC_total_size(ploc);
#endif
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
		// need to copy a smaller size than default reg-size
		DevWarn("formal_mload: switch from mtype_f8 to mtype_f4");
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

	  if (Target_Byte_Sex == BIG_ENDIAN) {
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

#ifdef TARG_ST
	// [TB] Improve line number.
	WN_copy_linenum(mload, store);
#endif
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
  WN	*wn, *emBlock;
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
  WN_Delete(tree);

  callblock = WN_CreateBlock();
  callblock = lower_block(callblock, actions);
#ifdef TARG_ST
  // [TB] Improve line number.
  WN_copy_linenum(tree, callblock);
#endif
  WN_INSERT_BlockLast(block, callblock);

  call = lower_intrinsic_call(block, call, actions);
#ifdef TARG_ST
  // [TB] Improve line number.
  WN_copy_linenum(tree, call);
#endif
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
#ifdef TARG_ST
  // [TB] Improve line number.
  WN_copy_linenum(tree, wn);
#endif
  WN_INSERT_BlockLast(block, wn);

  {
    TYPE_ID	ty1, ty2;
    PREG_NUM	reg1, reg2;
    TYPE_ID	type = WN_rtype(tree);

    rename_preg(INTR_intrinsic_name(tree), ".return");

#ifdef TARG_ST
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
	  retReg = Create_Preg(MTYPE_F8, current_preg_name);
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
	  retReg = Create_Preg(type, current_preg_name);
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
	  retReg = Create_Preg(TY_mtype(Ty_Table[type]), current_preg_name);
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

    rename_reset();
  }

  return wn;

#else
    if (WHIRL_Return_Info_On) {

      RETURN_INFO return_info = Get_Return_Info (MTYPE_To_TY(type),
                                                 Complex_Not_Simulated);

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
#endif /* TARG_ST */
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
    TYPE_ID type;

    if (Preg_Offset_Is_Int(reg)) 
      regST = Int_Preg;
    else
      // keep float size in preg
      regST = MTYPE_To_PREG(parmType);

    type = TY_mtype(ST_type(regST));

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
 *
 * lower_profile_call()
 *
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

/* ====================================================================
 *
 * WN *lower_call(WN *block, WN *tree, LOWER_ACTIONS actions)
 *
 * Perform lowering (see WN_Lower description) on call statement <tree>,
 * returning lowered tree.  Should be called only if actions contains
 * LOWER_CALL.
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

  if (traceCall) {
    fprintf(TFile, "------ lower_call: \n");
    fdump_tree (TFile, tree);
    fflush(TFile);
  }

  for (i = 0; i < WN_kid_count(tree); i++)
    WN_actual(tree, i) = lower_expr(block, WN_actual(tree, i), actions);

#if 0
  if (traceCall) {
    fprintf(TFile, "-----> after lower_expr for actuals: \n");
    fdump_tree (TFile, tree);
    fflush(TFile);
  }
#endif

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
                                              Complex_Not_Simulated);

#ifdef TARG_ST
    /* (cbr) nrv formal is already created in the frontend */
    if (RETURN_INFO_return_via_first_arg(return_info) &&
        !TY_return_to_param(prototype)) {
#else
    if (RETURN_INFO_return_via_first_arg(return_info)) {
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
      WN          *itree = WN_convert_to_pic (tree, callee_st);

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

    ploc = Setup_Output_Parameter_Locations(call_ty);

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

  if (traceCall) {
    fprintf(TFile, "------ more lowering: \n");
    fdump_tree (TFile, tree);
    fflush(TFile);
  }

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
    WN_INSERT_BlockLast (block, profcall);
  }
#endif
  callblock = WN_CreateBlock();
#ifdef TARG_ST
  WN *memcpyBlock =  WN_CreateBlock();
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

#ifdef TARG_ST
  //
  // Arthur: during this iteration over all PLOCs, we need to
  //         compute the _Formal_Save_Area_Size.
  //
  //         lower_mload_actual() and lower_complex_actual update
  //         the _Formal_Save_Area_Size as well.
  //
  // NOTE: it only works if we are guaranteed that once a PLOC,
  //       corresponding to a structure passed by value,
  //       is on stack => all the following PLOCs are on stack.
  //       This is indeed the case with all known to us ABIs:
  //       MIPS,IA64,ST200,ST100(the most exotic but structures
  //       passed by technical reference, so OK).
  //
  INT prev_ploc_ofst = 0;
  _Formal_Save_Area_Size = 0;
#endif

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
    else
    {
      ploc = Get_Output_Parameter_Location( MTYPE_To_TY(parmType));
#ifdef TARG_ST
      if (!PLOC_on_stack(ploc)) {
	_Formal_Save_Area_Size += PLOC_total_size(ploc) + 
	                          PLOC_rpad(ploc) - prev_ploc_ofst;
      }
      prev_ploc_ofst = PLOC_total_size(ploc)+PLOC_rpad(ploc);
#endif /* TARG_ST */
    }
  }

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

    if (PLOC_on_stack(ploc))
    {
     /*
      * stack offset 
      * We can use the preg for $sp here, but a simpler way
      * is to use the SP symbol. 
      */
      WN *wn;
#ifdef TARG_ST
#ifdef FIXED_FORMAL_AREA_SIZE // CG
      wn = WN_Stid(parmType, PLOC_offset(ploc) - Formal_Save_Area_Size
		             + STACK_OFFSET_ADJUSTMENT,
      		   SP_Sym, ty, actual);
#else
      wn = WN_Stid(parmType, PLOC_offset(ploc) - _Formal_Save_Area_Size
		             + STACK_OFFSET_ADJUSTMENT,
      		   SP_Sym, ty, actual);
#endif
#else
      wn = WN_Stid(parmType, PLOC_offset(ploc) - Formal_Save_Area_Size
		             + Stack_Offset_Adjustment,
		   SP_Sym, ty, actual);
#endif
      WN_Set_Linenum(wn, srcpos);
      WN_INSERT_BlockLast(callblock, wn);

#if 1
      if (traceCall) {
	fprintf(TFile, "  ------ actual after lowering: \n");
	fdump_tree (TFile, wn);
      }
#endif

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
    else
    {
      actual = lower_actual(callblock, actual, parmType, PLOC_reg(ploc));

#if 1
      if (traceCall) {
	fprintf(TFile, "  ------ actual after lowering: \n");
	fdump_tree (TFile, actual);
      }
#endif
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

  if (MTYPE_is_complex(WN_rtype(tree)))
  {
    INT16       n = WN_kid_count(tree);
    WN          *itree;
    TYPE_ID	rtype, desc;
    
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

  if (traceCall) {
    fprintf(TFile, "------> callblock: \n");
    fdump_tree (TFile, callblock);
    fflush(TFile);
  }

  callblock = lower_block(callblock, actions);

  if (traceCall) {
    fprintf(TFile, "------> lowered callblock: \n");
    fdump_tree (TFile, callblock);
    fflush(TFile);
  }

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

#ifndef TARG_ST
  // [CG] Indeed this cannot be done here, it is done in post_lower_call().

  /******************************************************
	STUPID: cannot generate call here as it will affect the return value
	for this function
  if (Gen_Profile)
  {
    WN_INSERT_BlockLast (block, tree);

    return lower_profile_call(block, tree, 1, actions);
  }
  ******************************************************/

  //  if (traceCall) {
  //    fprintf(TFile, "------> into: \n");
  //    fdump_tree (TFile, tree);
  //    fflush(TFile);
  //  }
#endif

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
			     WN_Coerce(Pointer_type, WN_kid0(tree)),
			     actions);

  if (NotAction(LOWER_COMPGOTO))
    return tree;

  index = WN_kid0(tree);

  indexN = Make_Leaf(block, WN_Coerce(Pointer_type, index), Pointer_type);

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
#ifndef BRK_RANGE
	fprintf(stderr, "Don't know how to do BRK_RANGE\n");
	exit(-1);
#else
	kids[0] = WN_Intconst ( MTYPE_I4, BRK_RANGE );
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
#ifndef BRK_RANGE
	fprintf(stderr, "Don't know how to do BRK_RANGE\n");
	exit(-1);
#else   
	trap = WN_CreateTrap ( BRK_RANGE );
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

  if (WN_operator(kid) == OPR_LNOT)
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
    return_info = Get_Return_Info (return_ty_idx, Use_Simulated);

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

      mtype   = RETURN_INFO_mtype (return_info, 0);
      preg    = RETURN_INFO_preg (return_info, 0);

#ifdef TARG_ST
      //
      // Arthur: if we're passing floating point results in integer
      //         registers, need to make a TAS
      // TODO: perhaps we'll need to generalize mapping of floating
      //       point types to integer types in mtypes.cxx ??
      //
      preg_st = MTYPE_To_PREG(mtype);

      if (MTYPE_is_float(return_mtype) && Preg_Offset_Is_Int(preg)) {
	wn  = WN_CreateStid (OPR_STID, MTYPE_V, Promoted_Mtype(mtype), 
	  		   preg, preg_st, Be_Type_Tbl(mtype), 
			   WN_Tas(mtype, Be_Type_Tbl(mtype), WN_kid0(tree)));
      }
      else {
	wn  = WN_CreateStid (OPR_STID, MTYPE_V, Promoted_Mtype(mtype), 
	  		   preg, preg_st, Be_Type_Tbl(mtype), 
			   WN_kid0(tree));
      }
#else
      preg_st = (MTYPE_is_float(mtype) ? Float_Preg :
                ((mtype==MTYPE_I8 || mtype==MTYPE_U8) ? MTYPE_To_PREG(mtype) :
      	   Int_Preg));
      wn  = WN_CreateStid (OPR_STID, MTYPE_V, Promoted_Mtype(mtype), 
	  		   preg, preg_st, Be_Type_Tbl(mtype), WN_kid0(tree));
#endif
    }
#ifdef TARG_ST
    // CL: keep line numbers for debug
    WN_Set_Linenum(wn, WN_Get_Linenum(tree));
#endif
    WN_INSERT_BlockLast (block, wn);
  }
  else { 
    //
    // MTYPE_M; rhs is one of MLDID or MILOAD or MLOAD
    //
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

    return_info = Get_Return_Info(ty_idx, Complex_Not_Simulated);

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

#ifdef TARG_ST
      // CL: keep line numbers for debug
      WN_Set_Linenum(wn, WN_Get_Linenum(tree));
#endif
      WN_INSERT_BlockLast (block, wn);
    }

#if 0
    else if (RETURN_INFO_return_via_return_reg(return_info)) {

      WN *n_rhs;

      // fix rhs
      if (WN_operator(o_rhs) == OPR_LDID)
        n_rhs = lower_mldid(block, o_rhs, LOWER_MLDID_MSTID);
      else if (WN_operator(o_rhs) == OPR_ILOAD) 
        n_rhs = lower_miload(block, o_rhs, LOWER_MLDID_MSTID);
      else n_rhs = o_rhs; 		// MLOAD

      // create an mstore
      ST *ret_st = get_struct_return_sym();
      FmtAssert(ret_st != NULL, ("return symbol not defined ?"));
      TY_IDX tidx = ST_type(ret_st);
      WN *awn = WN_CreateLdid(OPR_LDID, 
			      TY_mtype(Ty_Table[tidx]), 
			      TY_mtype(Ty_Table[tidx]),
			      0, 
			      ret_st, 
			      tidx);
      WN *swn = WN_CopyNode(WN_kid1(n_rhs));
      wn  = WN_CreateMstore (0, tidx, n_rhs, awn, swn);

#ifdef TARG_ST
      // CL: keep line numbers for debug
      WN_Set_Linenum(wn, WN_Get_Linenum(tree));
#endif
      WN_INSERT_BlockLast (block, wn);
    }
#endif /* TARG_ST */
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
	  n_rhs = WN_CreateLdid (OPR_LDID, mtype, mtype, 
				 WN_load_offset(o_rhs)
				   + i * MTYPE_byte_size(mtype),
				 WN_st_idx(o_rhs), ty_idx_used);
	  preg    = RETURN_INFO_preg (return_info, i);
	  preg_st = MTYPE_is_float(mtype) ? Float_Preg : Int_Preg;
	  wn = WN_CreateStid(OPR_STID, MTYPE_V, mtype, preg, preg_st, 
			     Be_Type_Tbl(mtype), n_rhs);
#ifdef TARG_ST
	  // CL: keep line numbers for debug
	  WN_Set_Linenum(wn, WN_Get_Linenum(tree));
#endif
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
#ifdef TARG_ST
	  // CL: keep line numbers for debug
	  WN_Set_Linenum(wn, WN_Get_Linenum(tree));
#endif
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
#ifdef TARG_ST
	  // CL: keep line numbers for debug
	  WN_Set_Linenum(wn, WN_Get_Linenum(tree));
#endif
	  WN_INSERT_BlockLast (block, wn);
	}
      }

      WN_Delete (o_rhs);
    }
  }

  WN *wn_return = WN_CreateReturn ();
  if ( Cur_PU_Feedback )
    Cur_PU_Feedback->FB_lower_return_val( tree, wn_return );
#ifdef TARG_ST
      // CL: keep line numbers for debug
  WN_Set_Linenum(wn_return, WN_Get_Linenum(tree));
#endif
  tree = wn_return;
  return tree;
}


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

static WN *lower_stmt(WN *block, WN *tree, LOWER_ACTIONS actions)
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
#endif


#ifdef TARG_ST
  case OPR_XGOTO:
    // We can come here from lower_hilo. kids have already been lowered from COMGOTO.
    if (Action(LOWER_HILO)) {
      break;
    }
    // else fall through.
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
	(WN_load_offset(child) == reg))
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
                                                 Complex_Not_Simulated);

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

    WN_INSERT_BlockLast(out, node);
  
    // if STID(ALLOCA) and trapuv, insert a mstore
    if ( Action(LOWER_INL_STACK_INTRINSIC) && 
	 (DEBUG_Trap_Uv
#ifdef TARG_ST
	  // TB Add -zerouv option from PathScale 2.1
	  || DEBUG_Zero_Uv
#endif
	  )
        && WN_operator(node) == OPR_STID
        && WN_operator(WN_kid0(node)) == OPR_ALLOCA)
    {
#ifdef TARG_ST
	  // TB Add -zerouv option from PathScale 2.1
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
#if 0
      if(traceRuntime) {
	fprintf(TFile,"=================================================\n");
	fdump_tree(TFile, node);
      }
#endif
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
#ifdef TARG_ST
      // [TB] Improve line number.
      WN_copy_linenum(tree, shortcircuit);
#endif
      WN_INSERT_BlockLast(block, shortcircuit);
    }
    else
    {
      left_branch = WN_Falsebr(falseLabel, WN_kid0(tree));
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
	WN_INSERT_BlockLast(block, right_branch);
      }
      else {
	/* branchType == FALSE */
	right_branch = WN_Falsebr(falseLabel, WN_kid1(tree));
	WN_INSERT_BlockLast(block, right_branch);
      }
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
#ifdef TARG_ST
      // [TB] Improve line number.
      WN_copy_linenum(tree, shortcircuit);
#endif
      WN_INSERT_BlockLast(block, shortcircuit);
    }
    else
    {
      left_branch = WN_Truebr(trueLabel, WN_kid0(tree));
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
	WN_INSERT_BlockLast(block, right_branch);
      }
      else {
	/* branchType == FALSE */
	right_branch = WN_Falsebr(falseLabel, WN_kid1(tree));
	WN_INSERT_BlockLast(block, right_branch);
      }
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
#ifdef TARG_ST
	// [TB] Improve line number.
	WN_copy_linenum(tree, falsebr_block);
#endif
	WN_INSERT_BlockFirst(body, falsebr_block);

	setCurrentStateBlockLast(WN_then(tree), actions);
	WN_INSERT_BlockLast(body, lower_block(WN_then(tree), actions));
	WN *wn_goto = WN_Goto(cont_lbl);
#ifdef TARG_ST
	// [TB] Improve line number.
	WN_copy_linenum(tree, wn_goto);
#endif
	WN_INSERT_BlockLast(body, wn_goto);

	WN *wn_else_lbl = WN_Label(else_lbl);
#ifdef TARG_ST
	// [TB] Improve line number.
	WN_copy_linenum(tree, wn_else_lbl);
#endif
	WN_INSERT_BlockLast(body, wn_else_lbl);
	setCurrentStateBlockLast(WN_else(tree), actions);
	WN_INSERT_BlockLast(body, lower_block(WN_else(tree), actions));
	WN *wn_cont_lbl = WN_Label(cont_lbl);
#ifdef TARG_ST
	// [TB] Improve line number.
	WN_copy_linenum(tree, wn_cont_lbl);
#endif
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
#ifdef TARG_ST
	// [TB] Improve line number.
	WN_copy_linenum(tree, falsebr_block);
#endif
	WN_INSERT_BlockFirst(body, falsebr_block);

	setCurrentStateBlockLast(WN_then(tree), actions);
	WN_INSERT_BlockLast(body, lower_block(WN_then(tree), actions));
	WN *wn_cont_lbl = WN_Label(cont_lbl);
#ifdef TARG_ST
	// [TB] Improve line number.
	WN_copy_linenum(tree, wn_cont_lbl);
#endif
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
#ifdef TARG_ST
      // [TB] Improve line number.
      WN_copy_linenum(tree, truebr_block);
#endif
      WN_INSERT_BlockFirst(body, truebr_block);

      setCurrentStateBlockLast(WN_else(tree), actions);
      WN_INSERT_BlockLast(body, lower_block(WN_else(tree), actions));
      WN *wn_cont_lbl = WN_Label(cont_lbl);
#ifdef TARG_ST
      // [TB] Improve line number.
      WN_copy_linenum(tree, wn_cont_lbl);
#endif
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

  loop_info = WN_do_loop_info(tree);  
  loop_nest_depth = loop_info ? WN_loop_depth(loop_info) : loop_nest_depth+1;

  if (Action(LOWER_DO_LOOP))
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
#ifdef TARG_ST
    // [TB] Improve line number.
    WN_copy_linenum(tree, top_lbl);
#endif
    WN_INSERT_BlockLast(body, top_lbl);

    WN_INSERT_BlockLast(body, lower_block(WN_do_body(tree), actions));

    setCurrentState(WN_step(tree), actions);
    WN_INSERT_BlockLast(body, lower_stmt(block, WN_step(tree), actions));
    WN *back_branch_block = lower_truebr(WN_label_number(top_lbl),
					 WN_end(tree), &wn_back_branch,
					 actions);
    WN_INSERT_BlockLast(body, back_branch_block);

    if (nz_trip == FALSE) {
#ifdef TARG_ST
      // [TB] Improve line number.
      WN_copy_linenum(tree, cont_lbl);
#endif
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
    WN_do_body(tree) = lower_block(WN_do_body(tree), actions);
  }

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
#ifdef TARG_ST
    // [TB] Improve line number.
    WN_copy_linenum(tree, top_lbl);
#endif
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
      WN_PRAGMA_ID pragma = (WN_PRAGMA_ID)WN_pragma(wn);
      switch (pragma) {
      case WN_PRAGMA_IVDEP:
      case WN_PRAGMA_UNROLL:
      case WN_PRAGMA_LOOPDEP:
      case WN_PRAGMA_LOOPMOD:
      case WN_PRAGMA_LOOPTRIP:
      case WN_PRAGMA_PIPELINE:
      case WN_PRAGMA_LOOPSEQ:
	WN_EXTRACT_FromBlock(block_orig, wn);
	WN_INSERT_BlockLast(block_dest, wn);
	break;
      }
    }
    else
      break;
  }
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
   /*
    *	(LABEL <top_lbl>)
    *	(FALSEBR <test> <cont_lbl>)
    *	<body>
    *	(GOTO <top_lbl>)
    *  (LABEL <cont_lbl>)
    */
    if (OPT_Space) {
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
    WN *top_branch_block = lower_falsebr(cont_lbl, test, &wn_top_branch,
					 actions);
    WN_INSERT_BlockLast(block, top_branch_block);

#ifdef TARG_ST
    // FdF 21/11/2005 Move loop pragmas close to the new loop head
    Move_Loop_Pragmas(tree, block, block);
#endif
    setCurrentState(WN_while_body(tree), actions);
    WN *wn_top_lbl = WN_Label(top_lbl);
    WN_INSERT_BlockLast(body, wn_top_lbl);

    setCurrentStateBlockLast(WN_while_body(tree), actions);
    WN_INSERT_BlockLast(body, lower_block(WN_while_body(tree), actions));

    WN *wn_back_branch = NULL;
    WN *back_branch_block = lower_truebr(top_lbl, WN_while_test(tree),
					 &wn_back_branch, actions);
    WN_INSERT_BlockLast(body, back_branch_block);
    WN *wn_cont_lbl = WN_Label(cont_lbl);
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
  cvt = RT_LOWER_expr (cvt);

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

  if (PLOC_on_stack(ploc))
  {
    /* on stack already */
    if (altentry)
    {
      ST	*upformal;
      WN	*ldid, *stid;

      upformal = Get_Altentry_UpFormal_Symbol (sym, ploc);

      ldid = WN_Ldid(TY_mtype(Ty_Table[formalTY]), 0, upformal, formalTY);

      stid = WN_Stid(TY_mtype(Ty_Table[formalTY]), 0, sym, formalTY, ldid);

      stid = lower_store(block, stid, actions);
      WN_Set_Linenum (stid, current_srcpos);
      WN_INSERT_BlockLast(block, stid);
    }
    else if (ST_promote_parm(sym))
    {
      if (MTYPE_is_float(TY_mtype(Ty_Table[formalTY])))
      {
	WN	*cvt;
	WN	*stid;

#ifdef TARG_ST
	cvt  = lower_promoted_formal(block, ploc, sym);
#else
	cvt  = lower_promoted_formal(ploc, sym);
#endif
	stid = WN_Stid(TY_mtype(Ty_Table[formalTY]), 0, sym,
				formalTY, cvt);

	WN_Set_Linenum (stid, current_srcpos);
	WN_INSERT_BlockLast(block, stid);
      }
    }
  }
  else if (MTYPE_is_m(TY_mtype(Ty_Table[formalTY])))
  {
    /* structure parameter */
    lower_mload_formal (block, formal, ploc, actions);
  }
  else
  {
    WN	*cvt, *stid;

#ifdef TARG_ST
    cvt  = lower_promoted_formal(block, ploc, sym);
#else
    cvt  = lower_promoted_formal(ploc, sym);
#endif

    stid = WN_Stid(TY_mtype(Ty_Table[formalTY]), 0, sym, formalTY, cvt);

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

      ST_Init (formalST, Save_Str(ST_name(formalST)), CLASS_VAR, SCLASS_AUTO,
	       EXPORT_LOCAL, formalTY);

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
#else
static WN *
Create_Mcount_Call (void)
{
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
  return Instrument_Functions_Enabled && WN_has_sym(current_function) &&
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
      WN *func_enter_call = Create_Profile_Func_Enter_Call();
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
    WN *func_exit_call = Create_Profile_Func_Exit_Call();
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

#if 0
  BOOL return_via_return_reg = FALSE;
  BOOL return_via_first_arg = FALSE;
  if (WN_operator(tree) == OPR_FUNC_ENTRY) {
    ST_IDX func_stidx = WN_st_idx(tree);
    PU_IDX puidx = ST_pu(St_Table[func_stidx]);
    TY_IDX prototype = PU_prototype(Pu_Table[puidx]);
    RETURN_INFO return_info = Get_Return_Info(TY_ret_type(prototype), 
					      Complex_Not_Simulated);

    return_via_first_arg = RETURN_INFO_return_via_first_arg(return_info);
    return_via_return_reg = RETURN_INFO_return_via_return_reg(return_info);
  }

  if (WHIRL_Return_Val_On && Action(LOWER_RETURN_VAL) && 
      WN_operator(tree) == OPR_FUNC_ENTRY) {
    ST_IDX func_stidx = WN_st_idx(tree);
    PU_IDX puidx = ST_pu(St_Table[func_stidx]);
    TY_IDX prototype = PU_prototype(Pu_Table[puidx]);

    // create fake first parameter for struct return if needed 
    if (return_via_first_arg) {
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
      WN_map_id(n_tree) = WN_map_id(tree);
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
    }
    else if (return_via_return_reg) {
      ST *return_st = Gen_Temp_Symbol(
			 Make_Pointer_Type(TY_ret_type(prototype), FALSE),
			 Index_To_Str(Save_Str2(".return.",
						ST_name(func_stidx))));
      set_struct_return_sym(return_st);
    }
  }

#else

  if (WHIRL_Return_Val_On && Action(LOWER_RETURN_VAL) && 
      WN_operator(tree) == OPR_FUNC_ENTRY)
  { // create fake first parameter for struct return if needed 
    ST_IDX func_stidx = WN_st_idx(tree);
    PU_IDX puidx = ST_pu(St_Table[func_stidx]);
    TY_IDX prototype = PU_prototype(Pu_Table[puidx]);
    RETURN_INFO return_info = Get_Return_Info(TY_ret_type(prototype), 
					      Complex_Not_Simulated);

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
      WN_map_id(n_tree) = WN_map_id(tree);
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
    }
  }
#endif /* TARG_ST */

  setCurrentState(tree, actions);
  block =		WN_CreateBlock();
  
#ifdef TARG_ST //[CG]
  if (Action(LOWER_ENTRY_PROMOTED) &&
      WN_operator(tree) == OPR_FUNC_ENTRY) {
    lower_entry_promoted_formals(block, tree);
  }
#endif

  if (Action(LOWER_ENTRY_EXIT))
  {
#if 0
    if (return_via_return_reg) {
      ST *return_st = get_struct_return_sym();

      // can reset now, no more use of this
      reset_struct_return_sym();

      //
      // Save struct_return_reg
      //
      ST *regST = MTYPE_To_PREG(Pointer_Mtype);
      WN *ldid = WN_Ldid(Pointer_Mtype, 
			 Struct_Return_Preg_Offset, 
			 regST, 
			 ST_type(regST));

      WN *stid = WN_Stid(TY_mtype(ST_type(return_st)), 0, 
			 return_st, ST_type(return_st), ldid);
      WN_Set_Linenum (stid, current_srcpos);
      WN_INSERT_BlockLast(block, stid);
    }
#endif

    ploc = lower_entry_formals(block, tree, actions);

    if (TY_is_varargs(Ty_Table[PU_prototype(Pu_Table[ST_pu(WN_st(tree))])]))
    {
     /*
      *  For varargs, the func-entry just has the list of fixed
      *  parameters, so also have to store the vararg registers. 
      */
      TYPE_ID	type = Def_Int_Mtype;

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
    }

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
  }
  else if (Action(LOWER_ENTRY_FORMAL_REF))
  {
    ploc = lower_entry_formals(block, tree, actions);
  }

  if (Action(LOWER_SPLIT_SYM_ADDRS))
  {
    /*
     *  Initialize the static link if required ($2 should have callers $fp)
     */
    ST *slink = Find_Slink_Symbol(CURRENT_SYMTAB);
    if (slink)
    {
      WN	*ld, *wn;

      ld = WN_LdidPreg(Pointer_type, Static_Link_Preg_Offset);

      wn = WN_Stid (Pointer_type, 0, slink, ST_type(slink), ld);

      WN_Set_Linenum (wn, current_srcpos);
      WN_INSERT_BlockFirst(block, wn);
    }
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
    // [CG] Moved into LOWER_INSTRTUMENT in function lower_preamble_end
    if (Call_Mcount && Action(LOWER_ENTRY_EXIT)) {
      WN *profcall = Create_Mcount_Call ();
      if (profcall) {	// possible that we won't create call yet
	WN_Set_Linenum (profcall, current_srcpos);
	WN_INSERT_BlockLast(block, profcall);
      }
    }
#endif

    WN_INSERT_BlockFirst(WN_func_body(tree), block);

    /* Lower statements in function body. */
    WN_func_body(tree) = lower_block(WN_func_body(tree), actions);
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
static WN *lower_landing_pad_entry(WN *tree)
{
  WN * block = WN_CreateBlock();
  WN_INSERT_BlockAfter (block, NULL, tree);

#ifdef TARG_ST200
  /* (cbr) only needed if used scratch registesrs. on st200 r8 r9 are used */
  ST_IDX exc_ptr_param = TCON_uval (INITV_tc_val (INITO_val (Get_Current_PU().unused)));
  ST exc_ptr_st = St_Table[exc_ptr_param];
  // Store rax into exc_ptr variable
  WN *exc_ptr_rax = WN_LdidPreg (Pointer_Mtype, Exc_Ptr_Param_Offset);
  WN *exc_ptr_stid = WN_Stid (Pointer_Mtype, 0, &exc_ptr_st, 
			ST_type(exc_ptr_st), exc_ptr_rax);

  ST_IDX filter_param = TCON_uval (INITV_tc_val (INITV_next (INITO_val (Get_Current_PU().unused))));
  ST filter_st = St_Table[filter_param];
  // Store rdx into filter variable
  WN *filter_rdx = WN_LdidPreg (MTYPE_U4, Exc_Filter_Param_Offset);
  WN *filter_stid = WN_Stid (MTYPE_U4, 0, &filter_st, ST_type(filter_st), filter_rdx);
#else
  ST_IDX exc_ptr_param = TCON_uval (INITV_tc_val (INITO_val (Get_Current_PU().unused)));
  ST exc_ptr_st = St_Table[exc_ptr_param];
  // Store rax into exc_ptr variable
  WN *exc_ptr_rax = WN_LdidPreg (Pointer_Mtype, 0);
  WN *exc_ptr_stid = WN_Stid (Pointer_Mtype, 0, &exc_ptr_st, 
			ST_type(exc_ptr_st), exc_ptr_rax);

  ST_IDX filter_param = TCON_uval (INITV_tc_val (INITV_next (INITO_val (Get_Current_PU().unused))));
  ST filter_st = St_Table[filter_param];
  // Store rdx into filter variable
  WN *filter_rdx = WN_LdidPreg (MTYPE_U4, 1);
  WN *filter_stid = WN_Stid (MTYPE_U4, 0, &filter_st, ST_type(filter_st), filter_rdx);
#endif
  WN_INSERT_BlockLast (block, exc_ptr_stid);

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
    if (RID_lowered(rid) == (LOWER_ACTIONS) NULL)
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

static WN *lower_scf(WN *block, WN *tree, LOWER_ACTIONS actions)
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
#ifdef TARG_ST
	  // TB Add -zerouv option from PathScale 2.1
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

      if (Has_Base_Block(st))
	continue;

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
	  // TB Add -zerouv option from PathScale 2.1
	  if (DEBUG_Zero_Uv) {
	    con = MTYPE_is_integral(btype) ?
	      WN_Intconst(Mtype_comparison(btype), 0) : WN_Floatconst(btype, 0.0);
	  } else if (TY_kind(type) == KIND_POINTER) {
	    con = WN_UVConstPointer(btype);
	  } else
#endif
	    con = WN_UVConst(btype);
	  Is_True(con != NULL, ("expected Uninitialized Variable const type"));
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
#ifdef TARG_ST
		// TB Add -zerouv option from PathScale 2.1
		if (DEBUG_Zero_Uv) {
		  con = WN_Intconst(q, 0);
		} else if (TY_kind(type) == KIND_ARRAY) {
		  // [TB] Get the appropriate 32/64-bit unitialized
		  // value from the type.
		  TY_IDX etype_idx = Ty_Table[type].Etype();
		  TYPE_ID etype_idx_id = TY_mtype(etype_idx);
		  // For non structure (con != NULL) get the right
		  // uninitialized value
		  con = WN_UVConst(TY_mtype(etype_idx));
		  if (con == NULL) {
		    // [TB] Array of struct
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
		Is_True(con != NULL, ("expected Uninitialized Variable const type"));

  		if ( DEBUG_Trap_Uv_Rjustify )
  		{
		  con = WN_RotateIntconst(con, residue*8);
  		}

		lda = WN_Lda(Pointer_type, 0, st);
		num = WN_Intconst(MTYPE_I4, nMoves * qSize);

		store = WN_CreateMstore(offset, ptype, con, lda, num);

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
#ifdef TARG_ST
		  // TB Add -zerouv option from PathScale 2.1
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
    DevWarn("LOWER_ACTION_name(0x%llx): expected only one flag at a time",
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

#ifdef TARG_ST
  case LOWER_DOUBLE:			return "LOWER_DOUBLE";
  case LOWER_LONGLONG:			return "LOWER_LONGLONG";
  case LOWER_ENTRY_PROMOTED:		return "LOWER_ENTRY_PROMOTED";
  case LOWER_NESTED_FN_PTRS:            return "LOWER_NESTED_FN_PTRS";
  case LOWER_INSTRUMENT:            	return "LOWER_INSTRUMENT";
  case LOWER_CST_DIV:			return "LOWER_CST_DIV";
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
  case MSTORE_intrinsic_bcopy:		return "intrinsic bcopy";
  default:				return "<unrecognized>";
  }
}


void WN_Lower_Checkdump(char *msg, WN *tree, LOWER_ACTIONS actions)
{
  traceAlignment   = Get_Trace(TP_LOWER, 0x004);
  traceSplitSymOff = Get_Trace(TP_LOWER, 0x010);
  traceIO          = Get_Trace(TP_LOWER, 0x020);
  traceSpeculate   = Get_Trace(TP_LOWER, 0x040);
  traceTreeHeight  = Get_Trace(TP_LOWER, 0x080);
  traceMload       = Get_Trace(TP_LOWER, 0x100);
  // traceUplevel    = Get_Trace(TP_LOWER, 0x200);
  traceLdid        = Get_Trace(TP_LOWER, 0x400);
  traceCall        = Get_Trace(TP_LOWER, 0x800);
  traceRuntime     = Get_Trace(TP_LOWER, 0x1000);

  if (Get_Trace(TP_LOWER, 0x008))
  {
    enable_tree_freq_display();
  }

  if (Get_Trace(TKIND_IR, TP_LOWER) ||
      Get_Trace(TP_LOWER, 0x2000))
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
				    LOWER_SPLIT_SYM_ADDRS	|
				    LOWER_CALL			|
#ifdef TARG_ST
				    LOWER_ENTRY_PROMOTED	|
				    LOWER_NESTED_FN_PTRS        |
				    LOWER_INSTRUMENT        	|
#endif
				    LOWER_RETURN_VAL);

   /*
    * remove these always
    *
    * disabling SPLIT_CONST will allow the lowerer to put together addresses
    * that have been pulled apart
    */
    actions &=  ~(LOWER_BASE_INDEX | LOWER_SPLIT_CONST_OFFSETS);

   /*
    * these form the major LOWER_TO_CG actions
    */
    actions |=	LOWER_SCF		  |
		LOWER_ARRAY		  |
		LOWER_MP		  |
		LOWER_IO_STATEMENT	  |
		LOWER_MSTORE		  |
		LOWER_CVT		  |
		LOWER_COMPGOTO		  |
		LOWER_COMPLEX		  |
		LOWER_QUAD		  |
		LOWER_MADD		  |
		LOWER_INTRINSIC		  |
		LOWER_ASSERT		  |
		LOWER_PICCALL		  |
		LOWER_ALL_MAPS		  |
		LOWER_SHORTCIRCUIT	  |
		LOWER_INL_STACK_INTRINSIC |
		LOWER_INLINE_INTRINSIC	  |
		LOWER_BIT_FIELD_ID;

   /*
    *  do not split divides into mul/recip at the CG lowering
    */
    save_Div_Split_Allowed = Div_Split_Allowed;
    Div_Split_Allowed = FALSE;;
  }
  if (WN_opcode(pu) == OPC_FUNC_ENTRY)
  {
    lowering_actions |= actions;
  }

  if (Action(LOWER_BITS_OP))
    actions |= LOWER_BIT_FIELD_ID;

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
   BZERO (TY_alias_array, sizeof(TY_IDX) * (MTYPE_LAST + 1));
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

/* ====================================================================
 *
 * WN *WN_Lower(WN *tree, LOWER_ACTIONS actions)
 *
 * Exported.  See interface description in "wn_lower.h".
 *
 * ==================================================================== */

WN *WN_Lower(WN *tree, LOWER_ACTIONS actions, struct ALIAS_MANAGER *alias,
	     char *msg)
{
#ifdef BACK_END
  Start_Timer(T_Lower_CU);
#endif
  alias_manager =	alias;
  loop_nest_depth =	0;

  // Don't do any lowering on trees that merely wrap up file-scope
  // assembly language code.
  if (WN_operator(tree) == OPR_FUNC_ENTRY &&
      ST_asm_function_st(*WN_st(tree))) {
    return tree;
  }

  actions = lower_actions(tree, actions);

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
 
  WN_verifier(tree);

  return tree;
}

