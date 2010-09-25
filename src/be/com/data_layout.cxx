/*
 *  Copyright (C) 2007, 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2007 QLogic Corporation.  All Rights Reserved.
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


/* ====================================================================
 * ====================================================================
 *
 * $Revision: 1.67 $
 * $Date: 05/08/31 17:55:24-07:00 $
 * $Author: tkong@hyalite.keyresearch $
 * $Source: be/com/SCCS/s.data_layout.cxx $
 *
 * Revision history:
 *  11-Nov-94 - Original Version
 *
 * Description:
 *
 * General support for data layout functionality in the back end.
 *
 * ====================================================================
 * ====================================================================
 */
#ifdef USE_PCH
#include "be_com_pch.h"
#endif /* USE_PCH */
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#pragma hdrstop
#include <cmplrs/rcodes.h>
#include <sys/resource.h>
#include <ext/slist>
#include <stdint.h>
#include "defs.h"
#include "erglob.h"
#include "erbe.h"
#include "config.h"
#include "tracing.h"
#include "strtab.h"
#include "stab.h"
#include "const.h"
#include "glob.h"
#include "mtypes.h"
#include "targ_const.h"
#include "targ_sim.h"
#include "ttype.h"
#include "irbdata.h"
#include "util.h"
#include "stblock.h"
#include "data_layout.h"
#include "wintrinsic.h"
#include "sections.h"
#include "betarget.h"
#include "intrn_info.h"
#include "elf_stuff.h"
#ifdef TARG_X8664
#include "config_opt.h"    // for CIS_Allowed

extern void (*CG_Set_Is_Stack_Used_p)();
extern INT (*Push_Pop_Int_Saved_Regs_p)(void);
#endif
#include "be_symtab.h"
#ifdef TARG_ST
#include "cg_flags.h"
#endif

#ifdef TARG_ST

// [CG]: we don't use anymore ISA_MAX_INST_BYTES
// we use DEFAULT_TEXT_ALIGNMENT instead
//#include "targ_isa_bundle.h" // for ISA_MAX_INST_BYTES

/* Arthur: this is common to all targets */
SECTION Sections[_SEC_INDEX_MAX] = {
  {_SEC_UNKNOWN,NULL,
     0,
	0, 0, 
     0, ".unknown", 0},
  {_SEC_TEXT,	NULL,
     0|SHF_EXECINSTR|SHF_ALLOC,
	SHT_PROGBITS, DEFAULT_TEXT_ALIGNMENT, 
     TEXT_SIZE, ELF_TEXT, 0},
  {_SEC_DATA,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
#ifdef TARG_ST
	SHT_PROGBITS, DEFAULT_DATA_ALIGNMENT, 
#else
	SHT_PROGBITS, 0, 
#endif
     DATA_SIZE, ELF_DATA, 0},
  {_SEC_SDATA,	NULL,
     0|SHF_WRITE|SHF_IA_64_SHORT|SHF_ALLOC,
#ifdef TARG_ST
	SHT_PROGBITS, DEFAULT_DATA_ALIGNMENT, 
#else
	SHT_PROGBITS, 0, 
#endif
     SDATA_SIZE, SDATA_NAME, 0},
  {_SEC_RDATA,	NULL,
     0|SHF_ALLOC,
#ifdef TARG_ST
	SHT_PROGBITS, DEFAULT_DATA_ALIGNMENT, 
#else
	SHT_PROGBITS, 0, 
#endif
     RDATA_SIZE, ELF_RODATA, 0},
  {_SEC_SRDATA,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC,
#ifdef TARG_ST
	SHT_PROGBITS, DEFAULT_DATA_ALIGNMENT, 
#else
	SHT_PROGBITS, 0, 
#endif
     INT32_MAX, MIPS_SRDATA, 0},
  {_SEC_LIT4,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC|SHF_MIPS_MERGE,
	SHT_PROGBITS, 4, 
     LIT4_SIZE, LIT4_NAME, 0},
  {_SEC_LIT8,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC|SHF_MIPS_MERGE,
	SHT_PROGBITS, 8, 
     LIT8_SIZE, LIT8_NAME, 0},
  {_SEC_LIT16,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC|SHF_MIPS_MERGE,
	SHT_PROGBITS, 16, 
     LIT16_SIZE, LIT16_NAME, 0},
  {_SEC_BSS,	NULL,
     0|SHF_WRITE|SHF_ALLOC,
#ifdef TARG_ST
	SHT_NOBITS, DEFAULT_DATA_ALIGNMENT, 
#else
	SHT_NOBITS, 0, 
#endif
     BSS_SIZE, ELF_BSS, 0},
  {_SEC_SBSS,	NULL,
     0|SHF_WRITE|SHF_IA_64_SHORT|SHF_ALLOC,
#ifdef TARG_ST
	SHT_NOBITS, DEFAULT_DATA_ALIGNMENT, 
#else
	SHT_NOBITS, 0, 
#endif
     SBSS_SIZE, SBSS_NAME, 0},
  {_SEC_GOT,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     GOT_SIZE, ELF_GOT, 0},
  {_SEC_CPLINIT,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
	SHT_PROGBITS, 0, 
     CPLINIT_SIZE, CPLINIT_NAME, 0},

#ifdef TARG_ST200
  {_SEC_EH_REGION,      NULL,
     0|SHF_ALLOC|SHF_MIPS_NAMES,
        SHT_PROGBITS, 0,
     INT32_MAX, ".except_table", 0},
  {_SEC_EH_REGION_SUPP, NULL,
     0|SHF_ALLOC|SHF_MIPS_NAMES,
        SHT_PROGBITS, 0,
     INT32_MAX, ".except_table_supp", 0},
#endif

#ifdef TARG_ST // [SC] TLS support
  {_SEC_TDATA, NULL,
   0|SHF_WRITE|SHF_ALLOC|SHF_TLS,
   SHT_PROGBITS, DEFAULT_DATA_ALIGNMENT,
   INT64_MAX, ".tdata", 0},
  {_SEC_TBSS, NULL,
   0|SHF_WRITE|SHF_ALLOC|SHF_TLS,
   SHT_NOBITS, DEFAULT_DATA_ALIGNMENT,
   INT64_MAX, ".tbss", 0},
#endif
  // MIPS and IA64 specific sections
#if defined(TARG_MIPS) || defined(TARG_IA64)
  {_SEC_LDATA,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_LOCAL,
	SHT_PROGBITS, 0, 
     INT64_MAX, ".MIPS.ldata", 0},
#ifdef mips
  {_SEC_LBSS,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_LOCAL,
	SHT_NOBITS, 0, 
     INT64_MAX, MIPS_LBSS, 0},
#else
  // There is no MIPS_LBSS section on Linux, but we need a space holder
  {_SEC_LBSS,   NULL,
     0,
        0, 0,
     0, ".unknown", 0},
#endif

#ifdef mips
  {_SEC_EH_REGION,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
	SHT_PROGBITS, 0, 
     INT64_MAX, MIPS_EH_REGION, 0},
  {_SEC_EH_REGION_SUPP,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
	SHT_PROGBITS, 0, 
     INT64_MAX, MIPS_EH_REGION_SUPP, 0},
#else
  // It's not yet clear what to do about the EH_REGION sections on Linux
  {_SEC_EH_REGION,      NULL,
     0,
        0, 0,
     0, ".unknown", 0},
  {_SEC_EH_REGION_SUPP, NULL,
     0,
        0, 0,
     0, ".unknown", 0},
#endif
  {_SEC_DISTR_ARRAY,  NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
	SHT_PROGBITS, 0,
     INT64_MAX, "_MIPS_distr_array", 0},
  // I don't know what it is
  {_SEC_THREAD_PRIVATE_FUNCS,      NULL,
     0,
        0, 0,
     0, ".unknown", 0},
#endif /* TARG_MIPS || TARG_IA64 */

#if defined(TARG_ST100)
  {_SEC_XSPACE,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
	SHT_PROGBITS, 0, 
     INT32_MAX, ".xspace", 0},
  {_SEC_YSPACE,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
	SHT_PROGBITS, 0, 
     INT32_MAX, ".yspace", 0},
  {_SEC_TDATA1,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
	SHT_PROGBITS, 0, 
     TDATA1_SIZE, ".tdata1", 0},
  {_SEC_TDATA2,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
	SHT_PROGBITS, 0, 
     TDATA2_SIZE, ".tdata2", 0},
  {_SEC_TDATA4,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
	SHT_PROGBITS, 0, 
     TDATA4_SIZE, ".tdata4", 0},
  {_SEC_SDATA1,	NULL,
     0|SHF_WRITE|SHF_ST100_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     SDATA1_SIZE, ".sdata1", 0},
  {_SEC_SDATA2,	NULL,
     0|SHF_WRITE|SHF_ST100_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     SDATA2_SIZE, ".sdata2", 0},
  {_SEC_SDATA4,	NULL,
     0|SHF_WRITE|SHF_ST100_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     SDATA4_SIZE, ".sdata4", 0},
#endif /* TARG_ST100 */
};
#endif

extern void Early_Terminate (INT status);
#ifndef TARG_ST
#define ST_force_gprel(s)	ST_gprel(s)
#endif

ST *SP_Sym;
ST *FP_Sym;
ST *Local_Spill_Sym;
extern INT32 mp_io;
INT32 Current_PU_Actual_Size;

#ifdef TARG_ST

// Formal parameters homing area size for the current PU
// [JV] This is no more initialized because Default_Formal_Save_Area_Size
// is now a variable in targ_sim.h
INT Max_Formal_Save_Area_Size;
INT Vararg_Formal_Save_Area_Size;
INT Formal_Save_Area_Size;
INT Upformal_Area_Size;

// Used to account for push/pop area that may be written before the
// stack pointer is set:
INT Stack_Offset_Adjustment = STACK_OFFSET_ADJUSTMENT;

#endif

STACK_MODEL Current_PU_Stack_Model = SMODEL_UNDEF;

#define ST_NAME(st)	(ST_class(st) == CLASS_CONST ? \
	Targ_Print(NULL,STC_val(st)) : ST_name(st) )

static BOOL Frame_Has_Calls;	// whether stack frame has calls

/* ====================================================================
 * "Stack Frame" segments:
 *
 * The following enumerate sub-segments of a stack frame, which are
 * used for stack frame reorganization.
 * ====================================================================
 */

typedef enum {
  SFSEG_UNKNOWN,	/* Unknown/undefined */
  SFSEG_ACTUAL,		/* Actual arguments */
  SFSEG_FTEMP,		/* Fixed temporaries (altentry formals) */
  SFSEG_REGSAVE,        /* Register save area used with push/pops. */
  SFSEG_FORMAL,         /* Formal argument save area */
  SFSEG_UPFORMAL	/* Up-level Formal argument save area */
			/* (e.g. stack vars get put in caller's frame) */
  // note that we could define a separate callee-upformal area for
  // targets that can store to the uplevel area, but then it is hard
  // to optimize away that space when it is not used; so instead we
  // do the stack_offset_adjustment trick.
} SF_SEGMENT;

#define SFSEG_FIRST	SFSEG_ACTUAL
#define SFSEG_LAST	SFSEG_UPFORMAL

typedef struct {
  SF_SEGMENT	seg;	/* Which segment */
  ST           *block;  /* Which ST block */
  mINT64	maxsize;/* Maximum allocated size */
  const char	*name;	/* For tracing */
} SF_SEG_DESC;

#define	SFSEG_seg(s)	((s)->seg)
#define	SFSEG_block(s)	((s)->block)
#define	SFSEG_maxsize(s) ((s)->maxsize)
#define	SFSEG_name(s)	((s)->name)

/* Note:  This code requires the following machine parameters
 *
 *   Max_Small_Frame_Offset : Maximum simple addressing offset
 * 	(defined in betarget)
 *   DEFAULT_LARGE_OBJECT_BYTES	How big is a large object?
 *   DEFAULT_TEMP_SPACE_BYTES	How much space to allocate for temps?
 */
#ifdef TARG_ST
// Arthur: moved this to config_targ.h
#define MAX_FRAME_OFFSET	\
	(Current_PU_Stack_Model == SMODEL_SMALL ? \
	MAX_SMALL_FRAME_OFFSET : MAX_LARGE_FRAME_OFFSET)
#else
#define DEFAULT_LARGE_OBJECT_BYTES	64

/*defined  in config_targ.h */
//#define DEFAULT_TEMP_SPACE_BYTES 4096
//#define MAX_SFSEG_BYTES		0x7FFFFFFFFFFFFFFFLL
//#define MAX_LARGE_FRAME_OFFSET	0x7FFFFFFFFFFFFFFFLL	// 64bits on all targs
#define MAX_FRAME_OFFSET	\
	(Current_PU_Stack_Model == SMODEL_SMALL ? \
	Max_Small_Frame_Offset : MAX_LARGE_FRAME_OFFSET)

typedef enum _align {
  _BYTE_ALIGN = 1,
  _CARD_ALIGN = 2,
  _WORD_ALIGN = 4,
  _DWORD_ALIGN = 8,
  _QUAD_ALIGN = 16
} ALIGN;  
#endif

static STACK_DIR stack_direction;

#ifndef DEFAULT_STACK_ALIGNMENT
#define DEFAULT_STACK_ALIGNMENT _QUAD_ALIGN
#endif // DEFAULT_STACK_ALIGNMENT

#ifdef TARG_X8664
#define stack_align   ( Stack_Alignment() )
#else
static INT16 stack_align = DEFAULT_STACK_ALIGNMENT;
#endif


/* Define the segment descriptors we need: */
static SF_SEG_DESC SF_Seg_Descriptors [SFSEG_LAST+1] = {
  { SFSEG_UNKNOWN, NULL, MAX_SFSEG_BYTES, "Unknown" },
  { SFSEG_ACTUAL,  NULL, MAX_SFSEG_BYTES, "Actual_Arg" },
  { SFSEG_FTEMP,   NULL, MAX_SFSEG_BYTES, "Fixed_Temp" },
  { SFSEG_REGSAVE, NULL, MAX_SFSEG_BYTES, "Reg_Save" },
  { SFSEG_FORMAL,  NULL, MAX_SFSEG_BYTES, "Formal_Arg" },
  { SFSEG_UPFORMAL,  NULL, MAX_SFSEG_BYTES, "UpFormal_Arg" }
};
#define	SF_Seg_Desc(n)	(&SF_Seg_Descriptors[n])

#define SF_Block(n)	(SFSEG_block(SF_Seg_Desc(n)))
#define SF_Maxsize(n)	(SFSEG_maxsize(SF_Seg_Desc(n)))
#define SF_Name(n)	(SFSEG_name(SF_Seg_Desc(n)))

/* How big is a large object?  If we end up with too many small
 * objects, we may want to change this value and try again, so we
 * use this variable below instead of the #defined default:
 */
static INT32 Large_Object_Bytes;

static BOOL Trace_Frame = FALSE;

/* ====================================================================
 *
 * Static/Global Object Layout Related Data Structures
 *
 * ====================================================================
 */
#ifndef TARG_ST
#define Is_root_base(st) (ST_class(st) == CLASS_BLOCK && STB_root_base(st))
#define Has_No_Base_Block(st) (ST_base(st) == st)
#define Is_Allocatable(st)	(!Has_Base_Block(st) && !Is_root_base(st))
#define Is_Allocatable_Root_Block(st) \
 (ST_class(st) == CLASS_BLOCK && !Is_root_base(st) && Has_No_Base_Block(st))
#endif


/* --------------------------------------------------------------------
 * Forward references
 * --------------------------------------------------------------------
 */
#ifdef TARG_ST
// only called from Shorten_Section():
extern SECTION_IDX Corresponding_Short_Section (ST *st, SECTION_IDX sec);
#endif
static SECTION_IDX Shorten_Section (ST *, SECTION_IDX);
static void Allocate_Object_To_Section (ST *, SECTION_IDX, UINT);
static void Allocate_Label (ST *lab);

// bug fix for OSP_138
BOOL ST_has_Predefined_Named_Section (ST *, SECTION_IDX &);

extern BOOL
Is_Allocated (ST *st)
{
  ST *base;
#ifdef TARG_ST
  base = Base_Symbol (st);
#else
  INT64 ofst;
  Base_Symbol_And_Offset(st, &base, &ofst);
#endif
  // formals may be allocated to block, but not root_block
  if (ST_sclass(st) == SCLASS_FORMAL && ST_class(base) == CLASS_BLOCK) 
	return TRUE;
  return ((base != st && Is_root_base(base)) || Is_root_base(st));
}





#define ROUNDUP(val,align) 	( (-(INT64)align) & (INT64)(val+align-1) )
#define ROUNDDOWN(val,align)	( (-(INT64)align) & (INT64)(val)         )
#define IS_POW2(num)		( (~((num)-1) & (num) ) == (num) )


/* ====================================================================
 *
 * ST_on_stack
 *
 * Returns TRUE if the object is on the Stack.
 *
 * ====================================================================
 */

BOOL ST_on_stack(ST *sym)
{
  if (ST_sclass(sym) == SCLASS_AUTO)
	return TRUE;
  else if (ST_sclass(sym) == SCLASS_FORMAL)
	return TRUE;
  else if (sym == SP_Sym || sym == FP_Sym)
	return TRUE;
  else {
  	ST *base = Base_Symbol(sym);
  	if (base == SP_Sym || base == FP_Sym)
		return TRUE;
	else
		return FALSE;
  }
}




/* ====================================================================
 *
 * ST_pu_defined
 *
 * Returns TRUE if the object is defined in this PU.
 *
 * ====================================================================
 */

BOOL ST_pu_defined(ST *sym)
{
  if ((ST_class(sym) == CLASS_VAR)	||
      (ST_class(sym) == CLASS_CONST))
  {
    switch(ST_sclass(sym))
    {
    case SCLASS_AUTO:
    case SCLASS_PSTATIC:
    case SCLASS_FSTATIC:
    case SCLASS_DGLOBAL:
    case SCLASS_FORMAL:
      return TRUE;
    default:
      return FALSE;
    }
  }
  return FALSE;

}


// TODO: for efficiency should probably hash this info,
// but for now just do a linear search.
// there shouldn't be very many of these nodes....
typedef struct {
  const ST        *sym;
        ST        *base;
        PREG_NUM   preg;
          // This is ST_level(sym).  We store it because when
          // Init_ST_formal_info_for_PU() is called for any parent PU after
          // the first, the sym field is invalid (its Scope_Table has been
          // deallocated).
        SYMTAB_IDX level;
} formal_info;
static formal_info *formal_info_array;
static INT max_formal_info_index = 0;

static void
Realloc_ST_formal_info (INT min_needed)
{
	if (min_needed >= max_formal_info_index) {
		// realloc space
		formal_info_array = TYPE_MEM_POOL_REALLOC_N (
			formal_info, &MEM_src_pool, 
			formal_info_array,
			max_formal_info_index,
			min_needed + 8);
                max_formal_info_index = min_needed + 8;
	}
}

// clear out the formal_info_array.
// num_formals may be lower than real number, in case of altentries.
static void
Init_ST_formal_info_for_PU (INT num_formals)
{
	if (formal_info_array == NULL) {
		max_formal_info_index = MAX(8,num_formals);
		formal_info_array = (formal_info*) Src_Alloc (
			sizeof(formal_info) * max_formal_info_index);
	}
	else {
		// clear out the values
		// but don't clear if value is for a uplevel symbol
		// that is still visible.
		INT num_uplevel_entries = 0;
		for (INT i = 0; i < max_formal_info_index; i++) {
			if (formal_info_array[i].sym != NULL) {
				if (formal_info_array[i].level >=
				    CURRENT_SYMTAB) {
					formal_info_array[i].sym = NULL;
					formal_info_array[i].base = NULL;
					formal_info_array[i].preg = 0;
					formal_info_array[i].level =
						SYMTAB_IDX_ZERO;
				} else {
					num_uplevel_entries++;
				}
			}
		}
		Realloc_ST_formal_info (num_formals + num_uplevel_entries);
	}

#ifdef Is_True_On
	// verify formal_info_array invariant: no NULL sym may precede a
	// a non-NULL one
	Is_True(max_formal_info_index > 0, ("no elements allocated"));
	const ST *last = formal_info_array[0].sym;
	for (INT i = 0; i < max_formal_info_index; i++) {
		Is_True(last != NULL || last == formal_info_array[i].sym,
			("NULL sym precedes non-NULL sym"));
		last = formal_info_array[i].sym;
	}
#endif
}

// Look for a formal-ref-base corresponding to the sym.
ST *
Get_ST_formal_ref_base (const ST *sym)
{
  if (ST_sclass(sym) != SCLASS_FORMAL_REF)
    return NULL;
  INT i;
  for (i = 0; i < max_formal_info_index; i++) {
	if (formal_info_array[i].sym == sym)
		return formal_info_array[i].base;
	else if (formal_info_array[i].sym == NULL)
		break;
  }
  return NULL;
}

void
Set_ST_formal_ref_base (const ST *sym, ST *base)
{
  INT i;
  for (i = 0; i < max_formal_info_index; i++) {
	if (formal_info_array[i].sym == NULL) {
		formal_info_array[i].sym = sym;
		formal_info_array[i].base = base;
		formal_info_array[i].level = ST_level(sym);
		return;
	}
  }
  Realloc_ST_formal_info (max_formal_info_index);
  formal_info_array[i].sym = sym;
  formal_info_array[i].base = base;
  formal_info_array[i].level = ST_level(sym);
}

// return 0 if no formal-preg-num
PREG_NUM
Get_ST_formal_preg_num (const ST *base)
{
  INT i;
  for (i = 0; i < max_formal_info_index; i++) {
	if (formal_info_array[i].base == base)
		return formal_info_array[i].preg;
	else if (formal_info_array[i].sym == NULL)
		break;
  }
  return (PREG_NUM) 0;
}

void
Set_ST_formal_preg_num (const ST *base, PREG_NUM p)
{
  INT i;
  for (i = 0; i < max_formal_info_index; i++) {
	if (formal_info_array[i].base == base) {
		formal_info_array[i].preg = p;
		return;
	}
  }
  Is_True(FALSE, ("Set_ST_formal_preg_num didn't find base"));
}

static ST *Formal_Sym(ST *sym, BOOL onstack)
{
  ST		*base;

  if (ST_sclass(sym) != SCLASS_FORMAL_REF)
    return sym;
  
  if (Is_Allocated(sym))
  {
    return sym;
  }
  if (base = Get_ST_formal_ref_base(sym))
  {
    return base;
  }

  base =		Copy_ST(sym);
  Set_ST_name(base, Save_Str2(ST_name(base), ".base"));
  Set_ST_type(base, Make_Pointer_Type(ST_type(sym)) );
  Set_ST_sclass (base, SCLASS_FORMAL);
  Set_ST_formal_ref_base(sym, base);
 /*
  *  sym may need to be promoted, but the base is pointer type
  */
  Clear_ST_promote_parm(base);	

 /*
  *  cannot put uplevel formal in a register
  *  unfortunately , the nested_ref is set while lowering MP regions (ie. later)
  */
  /* Don't put things on the stack because of debug level 3 (PV 642999) */
  onstack |=    (Debug_Level > 0 && Debug_Level != 3)	||
#ifdef KEY
  	        // Bug 5072
		// Make -O0 and -O1 behave like -g.
		Opt_Level < 2 ||
#endif
		ST_has_nested_ref(sym)		||
		ST_declared_static(sym)		||
		PU_has_mp(Get_Current_PU());

  if (onstack == FALSE)
  {
    Set_ST_formal_preg_num(base, Create_Preg(TY_mtype(ST_type(base)), ST_name(base)));
  }

  if ( Trace_Frame )
  {
    fprintf(TFile, "SCLASS_FORMAL (%s, base %s, onstack= %d, preg= %d) allocated\n",
	    ST_name(sym), ST_name(base), onstack, Get_ST_formal_preg_num(base));
  }

  return base;
}



/* ====================================================================
 *
 * Stack_Alignment
 *
 * Return the alignment requirement for the object on the stack frame.
 *
 * ====================================================================
 */

INT32
Stack_Alignment ( void )
{
#ifdef TARG_X8664
  if( Is_Target_64bit() )
    return _QUAD_ALIGN;

  /* TODO:
     We need something like -mpreferred-stack-boundary=num.
   */
  if( Align_Double )
    return  _DWORD_ALIGN;

  return _WORD_ALIGN;
#else
  return stack_align;
#endif
}


STACK_DIR Stack_Direction(void)
{
  return stack_direction;
}

static STACK_DIR
Get_Direction (ST *blk)
{
  if (STB_decrement(blk)) return DECREMENT;
  else return INCREMENT;
}

static void
Set_Direction (STACK_DIR dir, ST *blk)
{
  if (dir == DECREMENT) Set_STB_decrement(blk);
  else Reset_STB_decrement(blk);
}

static ST*
Create_Local_Block( STACK_DIR dir, STR_IDX name )
{
  ST *new_blk = New_ST_Block(name, FALSE, SCLASS_UNKNOWN, stack_align, 0);
  Set_Direction(dir, new_blk);
  Enter_ST(new_blk);
  return new_blk;
}

static ST *
Create_Base_Reg (const char *name, STACK_DIR dir)
{
  ST *new_blk = Create_Local_Block(dir, Save_Str(name));
  Set_STB_root_base(new_blk);
  Set_STB_is_basereg(new_blk);
  return new_blk;
}


/*
 * Temporarily set offset of blk, but don't change base because blk is
 * not finished yet.  This allows us to find the correct offset of 
 * objects that are in the blk, even while the block is growing.
 * Later call Allocate_Space to reset the offset and change the base.
 *
 * Note that this assumes that the previous blocks under base are fixed;
 * This only works for the first "unfixed" block.
 */
static void
Assign_Offset (ST *blk, ST *base, INT32 lpad, INT32 rpad)
{
#ifdef TARG_ST
  //
  // Arthur: seems like alignment must be computed differently since 
  //         it is called
  //         only once for UPFORMAL SEGMENT that should have been
  //         aligned at $sp+Stack_Offset_Adjustment boundary ? 
  // TODO: how about no alignment at all but polacement of data
  //       to upformals is alignment sensitive ?
  //
  Set_ST_ofst(blk, STB_size(base) + lpad);
  if (STB_decrement(base)) {
    /* point to start of object */
    Set_ST_ofst(blk, -(INT64) (ST_ofst(blk) + ST_size(blk) + rpad));
  }
#else
  UINT align = Adjusted_Alignment(blk);
  Set_ST_ofst(blk, ROUNDUP(STB_size(base) + lpad, align));
  if (STB_decrement(base)) {
    INT32 size = ST_size(blk);
#ifdef KEY
    // C++ requires distinct addresses for empty classes
    if (ST_class(blk) == CLASS_VAR && (!Current_pu || 
       (PU_src_lang (Get_Current_PU()) & PU_CXX_LANG)) && size == 0)
      size = 1;
#endif
    /* point to start of object */
    Set_ST_ofst(blk, 
	-(INT64) ROUNDUP(ST_ofst(blk) + size + rpad, align));
  }
#endif
}

/* ====================================================================
 *
 * Allocate_Space
 *
 * Compute the new ST_ofst for blk according to the block size of
 * base, the pad, the direction.  The CROSS-ENDIAN issue should be
 * handled in this routine, when the roundup is performed.
 *
 * ====================================================================
 */
static void
Allocate_Space(ST *base, ST *blk, INT32 lpad, INT32 rpad, INT64 maxsize)
{
  UINT align = Adjusted_Alignment(blk);
  INT64 old_offset;
  INT64 size = ST_size(blk);
  // [SC]: For an initialized symbol, make sure there is enough
  // space for all of the initializer.
  if (ST_is_initialized (blk)) {
    INITV_IDX inito_idx = ST_has_inito (blk);
    if (inito_idx != 0) {
      size = MAX (size, Get_INITO_Size (inito_idx));
    }
  }
#ifdef KEY
  // C++ requires distinct addresses for empty classes
  if (ST_class(blk) == CLASS_VAR && (!Current_pu ||
     (PU_src_lang (Get_Current_PU()) & PU_CXX_LANG)) && size == 0)
    size = 1;
#endif

  Set_STB_align(base, MAX(STB_align(base), align));
  if (!STB_decrement(base)) {
    old_offset = STB_size(base);
    Set_ST_ofst(blk, ROUNDUP(old_offset + lpad, align));
    Set_STB_size(base, ROUNDUP(ST_ofst(blk) + size + rpad, align));
  }
  else {
    old_offset = STB_size(base);
    /* align object end */
    Set_ST_ofst(blk, ROUNDUP(old_offset + lpad, align));	
    Set_ST_ofst(blk,
	-(INT64) ROUNDUP(ST_ofst(blk) + size + rpad, align)); /* start */
    Set_STB_size(base, -ST_ofst(blk));
  }
  if ((base == SP_Sym && Frame_Has_Calls) || base == FP_Sym) {
	// on stack
	// All stack offsets are adjusted a certain amount,
	// such that we can use part of the space from the callers frame.
	// If based on FP then is always offset even if leaf
	// cause are using space in callers frame.
	Set_ST_ofst(blk, ST_ofst(blk) + Stack_Offset_Adjustment);
  }
  if ( Trace_Frame ) {
    if (ST_class(blk) == CLASS_CONST)
	fprintf ( TFile, "Allocate <constant: %s>", Targ_Print(NULL,STC_val(blk)));
    else
    	fprintf ( TFile, "Allocate %s", ST_name(blk));
    if (ST_class(blk) == CLASS_BLOCK || ST_class(blk) == CLASS_FUNC)
      fprintf (TFile, " to %s: offset = %" SCNd64 ", size = %" SCNd64 "\n",
	ST_name(base), ST_ofst(blk), size);
    else fprintf (TFile, " to %s: offset = %" SCNd64 ", size = %" SCNd64 ", align = %d\n",
		  ST_name(base), ST_ofst(blk), size,
#ifdef KEY
		  align
#else
		  TY_align(ST_type(blk)) 
#endif // KEY
		  );
  }
#ifdef TARG_ST
  if (STB_size(base) > maxsize
	&& Is_Local_Symbol(base)			// on stack
	&& maxsize == MAX_SMALL_FRAME_OFFSET
	&& Current_PU_Stack_Model == SMODEL_SMALL)
#else
  if (STB_size(base) > maxsize
	&& Is_Local_Symbol(base)			// on stack
	&& maxsize == Max_Small_Frame_Offset 
	&& Current_PU_Stack_Model == SMODEL_SMALL)
#endif
  {
	DevWarn("overflowed small stack frame; will try recompiling with -TENV:large_stack");
	Early_Terminate(RC_OVERFLOW_ERROR);
  }
  Is_True( old_offset <= STB_size(base) && STB_size(base) <= maxsize,
	("Block size overflowed"));
}


/* ====================================================================
 *
 * ST_Block_Merge
 *
 * Merge second ST entry into the first ST entry's container block,
 * if these two ST entries belong to the same storage class.  It
 * creates a new container block for the first ST if there isn't one
 * yet. After setting the base, it performs data layout by assigning
 * the offset in the second block with proper alignment, additional
 * padding as passed in parameter, "pad",  and check if the current
 * block exceeds the limit, "maxsize", for this block.
 *
 * ====================================================================
 */

static ST*
ST_Block_Merge (ST *block, ST *sym, INT32 lpad, INT32 rpad, INT64 maxsize)
{
  ST *base;

  Is_True(ST_class(block) == CLASS_BLOCK, ("block-merge not given a block"));
  Set_ST_base(sym, block);
  base = ST_base(sym);
  Allocate_Space(base, sym, lpad, rpad, maxsize);

  /* inherit the init attributes from the individual blocks */
  if (ST_is_initialized(sym) && !STB_section(block))
    Set_ST_is_initialized(block);

  return base;
}


static void
Initialize_Frame_Segment (SF_SEGMENT seg, ST_SCLASS sclass, STACK_DIR dir)
{
  if (SF_Block(seg) == NULL)
  {
    	SF_Block(seg) = Create_Local_Block(dir, 
		Save_Str2 (SF_Name(seg),"_StkSeg"));
	Set_ST_sclass (SF_Block(seg), sclass);
  }
}

static void
Assign_Object_To_Frame_Segment ( ST *sym, SF_SEGMENT seg, INT64 offset)
{
	if (Trace_Frame) {
		fprintf ( TFile, 
			"Assigning symbol %s to segment %s at offset %" SCNd64 "\n",
			ST_name(sym), SF_Name(seg), offset);
	}
  	Initialize_Frame_Segment (seg, ST_sclass(sym), INCREMENT);
	Set_ST_base(sym, SF_Block(seg));
	Set_ST_ofst(sym, offset);
}

/* ====================================================================
 *
 * Add_Object_To_Frame_Segment
 *
 * Add the given object to the given stack segment.
 *
 * ====================================================================
 */
#ifdef TARG_ST
// Arthur: we get the padding parameters from the PLOC and pass them
//         to this routine !
static void
Add_Object_To_Frame_Segment (
  ST *sym, 
  SF_SEGMENT seg, 
  INT lpad,
  INT rpad,
  BOOL allocate
)
#else
static void
Add_Object_To_Frame_Segment ( ST *sym, SF_SEGMENT seg, BOOL allocate )
#endif
{
  if ( Trace_Frame && !allocate )
  {
    fprintf ( TFile, "Adding symbol to %s segment -- %s \n",
              SF_Name(seg), ST_name(sym)? ST_name(sym) : "<null>");
  }

  Initialize_Frame_Segment (seg, ST_sclass(sym), INCREMENT);

  if (SF_Block(seg) == sym)
  {
   /*
    *  StkSeg will be in list of local symbols, don't merge with self
    */
    return;
  }

  if (allocate)
  {
    INT64 size;
    INT32 lpad, rpad;
    lpad = rpad = 0;

    switch (seg)
    {
    case SFSEG_ACTUAL:
    case SFSEG_UPFORMAL:
    case SFSEG_FORMAL:
      size = ST_size(sym);
#ifdef KEY
      /* The size of an object of an empty class is regarded as 1-byte-long.
	 (bug#2967)
       */
      if( size == 0 &&
	  ( Current_pu == NULL ||
	    PU_cxx_lang( Get_Current_PU() ) ) ){
	size = 1;
      }    
#endif
      // struct params are left-justified.
      if (TY_kind(ST_type(sym)) == KIND_STRUCT)
      {
	rpad = ROUNDUP(size, MTYPE_RegisterSize(Spill_Int_Mtype)) - size;
      }
      else
      {
       /*
	* for parameters, we want to right-justify the objects;
	* so add in a pad amount to do the right-justifying.
	* container is the register size
	* Except float parms are left-justified!
	*/
	if (size < MTYPE_RegisterSize(Spill_Int_Mtype)) {
	  if (Target_Is_Little_Endian ||
	      MTYPE_is_float(TY_mtype(ST_type(sym)))) {
		rpad = MTYPE_RegisterSize(Spill_Int_Mtype) - size;
	  }
	  else
	  	lpad = MTYPE_RegisterSize(Spill_Int_Mtype) - size;
	}
      }
      break;
    }
    ST_Block_Merge (SF_Block(seg), sym, lpad, rpad, SF_Maxsize(seg));

#ifndef TARG_X8664
    if (seg == SFSEG_FORMAL)
    {
      ST *formal = SF_Block(seg);

     /* formal overlaps both formal and upformal area,
      * so reset the blksizes of the segments.
      */
      if (STB_size(formal) > Formal_Save_Area_Size)
      {
	ST *upformal = SF_Block(SFSEG_UPFORMAL);

	if (Trace_Frame)
	  fprintf(TFile, "<lay> split formal between segs\n");

	Initialize_Frame_Segment (SFSEG_UPFORMAL, ST_sclass(sym), INCREMENT);
	Set_STB_size(upformal, 
	    STB_size(upformal) + STB_size(formal) - Formal_Save_Area_Size);
	Set_STB_size(formal, Formal_Save_Area_Size);
      }
    }
#endif
  }
  else
  {
    Set_ST_base(sym, SF_Block(seg));
  }
}


// Map PU idx to arg-area-size.
// The arg-area-size is stored for efficiency, 
// to avoid recomputing at each call.
static UINT32 *arg_area_size_array;
static INT max_arg_area_size_index = 0;

// initialize and possibly realloc the arg-area-size array.
static void
Init_PU_arg_area_size_array (void)
{
	INT num_pus = TY_Table_Size();
	if (arg_area_size_array == NULL) {
		max_arg_area_size_index = num_pus;
		arg_area_size_array = (UINT32*) Src_Alloc (
			sizeof(UINT32) * (max_arg_area_size_index+1));
	}
	else if (num_pus >= max_arg_area_size_index) {
		// realloc space
	        num_pus = MAX(num_pus, 2 * max_arg_area_size_index);
		arg_area_size_array = TYPE_MEM_POOL_REALLOC_N (
			UINT32, &MEM_src_pool, 
			arg_area_size_array,
			max_arg_area_size_index,
			num_pus);
                max_arg_area_size_index = num_pus;
	}
}

// Look for the arg-area-size corresponding to the pu.
UINT32
Get_PU_arg_area_size (PU_IDX pu)
{
  Is_True(TY_kind(pu) == KIND_FUNCTION, ("Get_PU_arg_area_size of non-pu"));
  INT index = TY_id(pu);
  if (index >= max_arg_area_size_index) {
	// overflowed
	Init_PU_arg_area_size_array ();
  }
  Is_True(index < max_arg_area_size_index, ("Get_PU_arg_area_size still overflows?"));
  return arg_area_size_array[index];
}

void
Set_PU_arg_area_size (PU_IDX pu, UINT32 size)
{
  Is_True(TY_kind(pu) == KIND_FUNCTION, ("Set_PU_arg_area_size of non-pu"));
  INT index = TY_id(pu);
  if (index >= max_arg_area_size_index) {
	// overflowed
	Init_PU_arg_area_size_array ();
  }
  Is_True(index < max_arg_area_size_index, ("Set_PU_arg_area_size still overflows?"));
  arg_area_size_array[index] = size;
}

// return ST for __return_address if one exists
#define RETURN_ADDRESS_SYMBOL_NAME "__return_address"
struct is_return_address
{
    BOOL operator () (UINT32, const ST *st) const {
	return (strcmp (ST_name (st), RETURN_ADDRESS_SYMBOL_NAME) == 0);
    }
}; 

ST *
Find_Special_Return_Address_Symbol (void)
{
    ST_IDX st_idx = For_all_until (St_Table, CURRENT_SYMTAB,
				   is_return_address ());

    if (st_idx == 0)
	return NULL;
    else
	return ST_ptr(st_idx);

} // Find_Special_Return_Address_Symbol

#ifdef TARG_ST
/* ====================================================================
 *
 * Get_Special_Return_Address_Symbol
 * 
 * [CG] Returns the special return address symbol.
 * The symbol is created if it does not exists.
 *
 * ====================================================================
 */
ST *
Get_Special_Return_Address_Symbol (void)
{
  ST *return_address_st;
  return_address_st = Find_Special_Return_Address_Symbol ();
  if (return_address_st == NULL) {
    return_address_st = New_ST (CURRENT_SYMTAB);
    ST_Init (return_address_st, Save_Str (RETURN_ADDRESS_SYMBOL_NAME), 
	     CLASS_VAR, SCLASS_AUTO, EXPORT_LOCAL, 
	     Make_Pointer_Type (Be_Type_Tbl (MTYPE_V), FALSE));
  }
  return return_address_st;
}
#endif
/* ====================================================================
 *
 * Get_Section_ST
 * 
 * Given a section kind, it finds its block symbol in Sections table.
 * It creates a new block symbol if the block symbol doesn't exist.
 *
 * ====================================================================
 */
static ST*
Get_Section_ST(SECTION_IDX sec, UINT align, ST_SCLASS sclass)
{
  if (SEC_block(sec) == NULL) {
#ifdef TARG_X8664 // bug 9795
    if (sec == _SEC_DATA && align != 0)
      align = MAX(align, 16);
#endif
    ST *new_blk = New_ST_Block (Save_Str(SEC_name(sec)), 
	TRUE/*is_global*/, sclass, align, 0);
    Set_STB_section_idx(new_blk, sec);
    SEC_block(sec) = new_blk;
    Set_STB_section(new_blk);
    Set_STB_root_base(new_blk);
    if (SEC_is_gprel(sec)) {
	Set_STB_is_basereg(new_blk);
	Set_ST_gprel(new_blk);
    }
    if (SEC_is_merge(sec)) 
	Set_STB_merge(new_blk);
    if (SEC_is_exec(sec))
	Set_STB_exec(new_blk);
    if (SEC_is_nobits(sec)) 
	Set_STB_nobits(new_blk);
    Enter_ST(new_blk);
  }
  return SEC_block(sec);
}

// Either reuse or create a section block st that has same properties
// as "sec" and "sclass" parameters, but with specified "name".
// Need to possibly reuse for constructor/destructor case.
static ST *
Get_Section_ST_With_Given_Name (SECTION_IDX sec, ST_SCLASS sclass, STR_IDX name)
{
	ST *newblk = NULL;
	ST *st;
	INT i;
  	FOREACH_SYMBOL (GLOBAL_SYMTAB,st,i) {
		if (ST_class(st) != CLASS_BLOCK) continue;
		if (STB_section(st) && ST_name_idx(st) == name)
		{
			if (STB_section_idx(st) != sec) {
				ErrMsg (EC_LAY_section_name, ST_name(st), SEC_name(STB_section_idx(st)), SEC_name(sec));
			}
			newblk = st;
			break;
		}
	}
	if (newblk == NULL) {
#ifdef TARG_ST
    // bug fix for OSP_129
    // bug fix for OSP_254
    // The gnu4 FE will put static variables in STL into .gnu.linkonce.d. or .gnu.linkonce.b. or .bss.
    // If the section name doesn't start with 
    //     .gnu.linkonce.b. -or-
    //     .bss.     
    //    It's a user-defineded section, we 
    //       Create Data section for user-defineded section
    // The goal of these code is to be compatible with GCC
    if ( strncmp(Index_To_Str(name), ".gnu.linkonce.b.", 16) == 0 || 
	      strncmp(Index_To_Str(name), ".bss.", 5) == 0        ||
	      Section_Is_Special_Bss_Like_Section(name)) {
      // For .gnu.linkonce.b., .bss., force to _SEC_BSS
      sec = _SEC_BSS;
    }
    else if ( sec == _SEC_BSS ) {
      // Otherwise, user defined section, force to _SEC_DATA
      sec = _SEC_DATA;
    }
#endif
            
#ifdef TARG_ST
    // (cbr) data section to be created with default alignment.
    ST *blk = Get_Section_ST(sec, SEC_entsize(sec), sclass);
#else
		ST *blk = Get_Section_ST(sec, 0, sclass);
#endif
#ifdef KEY
		// Bug 14720: If the name of the section just created
		// already matches the one requested by NAME, then
		// use this section and don't create another (creating
		// another would create two sections with same name,
		// and wrongly allocate data in both sections).
		if (ST_name_idx(blk) == name)
		  newblk = blk;
		else {
		  newblk = Copy_ST_Block(blk);
		  // bug 14909: BLK may have been used before, and may
		  // have non-zero size/align. So reset size/align in
		  // the newly created section.
		  Set_STB_size(newblk, 0);
		  Set_STB_align(newblk, 0);
		  Set_ST_name_idx(newblk, name);
		}
#else
		newblk = Copy_ST_Block(blk);
		Set_ST_name_idx(newblk, name);
#endif
                
#ifdef TARG_ST
    // (cbr) reset default alignment.
    Set_STB_align(newblk, 0);
    //  TB: reset default size.
    Set_STB_size(newblk, 0);
#endif

	}
	return newblk;
}

#ifdef KEY
static void
Initialize_ST (ST * st)
{
  if (ST_is_initialized(st) && !ST_init_value_zero(st) /* bug 14412 */)
    return;
  if (ST_sclass(st) == SCLASS_UGLOBAL)
    Set_ST_sclass(st, SCLASS_DGLOBAL);

  Set_ST_is_initialized(st);
  Clear_ST_init_value_zero(st); // bug 14412
  INITO_IDX ino = New_INITO(st);
  INITV_IDX inv = New_INITV();
  INITV_Init_Pad (inv, ST_size(st));
  Set_INITO_val(ino, inv);
}
#endif

#ifndef KEY
static
#endif
void
Assign_ST_To_Named_Section (ST *st, STR_IDX name)
{
	ST *newblk;
	if (ST_is_not_used(st))
		return;

	// bug fix for OSP_138
	SECTION_IDX sec_idx;
	/* If st has section name attribute and it's same as one of the predefined section name,
	 * it is hanled in the same way as normal st. So it isn't assigned to section here. */
	if (ST_has_Predefined_Named_Section(st, sec_idx))
		return;
	if (ST_class(st) == CLASS_FUNC) {
		if (ST_sclass(st) == SCLASS_EXTERN) {
			// can just ignore section attribute on extern func
			return;
		}
		// first assign to text like usual,
		// then copy and change the name.
		newblk = Get_Section_ST_With_Given_Name (_SEC_TEXT, 
			ST_sclass(st), name);
		Set_ST_base(st, newblk);
	}
	else if (ST_class(st) == CLASS_VAR) {
		// assign object to section, 
		// copy section block and rename,
		// allocate object to new section block.
#ifdef TARG_ST
    if (ST_sclass(st) == SCLASS_EXTERN) {
      // [SC]: section attribute on external variable.
      // This is accepted, but we do not allocate the
      // variable, so we do not take any action here.
      // However, if the variable is non-preemptible,
      // and the section it is assigned to is known
      // to be accessible gp-relative, then we can
      // access the variable gp-relative.
      if (Gen_GP_Relative
	  && ! ST_is_preemptible(st)) {
	INT i;
	const char *secname = Index_To_Str (name);
	for (i = 0; i < _SEC_INDEX_MAX; ++i) {
	  if (strcmp (SEC_name(i), secname) == 0) {
	    if (SEC_is_gprel (i)) {
	      Set_ST_gprel (st);
	    }
	    break;
	  }
	}
      }
      return;
    }
#endif
    SECTION_IDX  sec;
    ST *base_st = st;
    if (Has_Base_Block(st)) {
        base_st = Base_Symbol(st);
    }

    switch (ST_sclass(st)) {
#if TARG_ST
      case SCLASS_FSTATIC:
      case SCLASS_PSTATIC:
        sec = Assign_Static_Variable (st);
        break;
      case SCLASS_DGLOBAL:
      case SCLASS_UGLOBAL:
        sec = Assign_Global_Variable (st, base_st);
        break;
#else

      case SCLASS_DGLOBAL:
      case SCLASS_FSTATIC:
      case SCLASS_PSTATIC:
        // must be initialized
        if (ST_is_constant(st)) sec = _SEC_RDATA;
        else sec = _SEC_DATA;
#ifdef KEY
        // Bugs 14181, 14269:
        // The symbol may not be initialized, add
        // a dummy initialization then, so that
        // Process_Initos_And_Literals() expands this.
        Initialize_ST (st);
#endif
        break;
      case SCLASS_UGLOBAL:
#ifdef KEY
        if (ST_is_constant(st) &&	// bug 4743
            !ST_is_weak_symbol(st)) {	// bug 4823
            sec = _SEC_RDATA;
        } else if (strncmp (&Str_Table[name],
                            ".gnu.linkonce.b.", 16) &&
                   strncmp (&Str_Table[name],
                            ".gnu.linkonce.sb.", 17) &&
                   strncmp (&Str_Table[name], ".bss.", 5) &&
                   strncmp (&Str_Table[name], ".sbss.", 6))
            // Not an assigned BSS section
            //
            // Bugs 14181, 14269: When multiple symbols
            // have the same section name, data_layout may
            // determine the section to be BSS based on some
            // symbol, and DATA based on another. This may
            // cause unstable behavior. Based on GCC
            // behavior, don't allocate symbols with
            // assigned sections to BSS. Add a dummy
            // initialization, so that this is processed
            // by Process_Initos_And_Literals() in CG.
            //
            // Bug 14515: Also apply to bss, sbss, gnu.linkone.sb
            sec = _SEC_DATA;
        else
#endif
            sec = _SEC_BSS;
#ifdef KEY
        Initialize_ST (st);
#endif
        break;
#ifdef KEY
      case SCLASS_COMMON:
      case SCLASS_EXTERN:			// bug 4845
        return;
#endif
#endif
      default:
        FmtAssert(FALSE,
                  ("unexpected sclass %d for section attribute on %s", 
                   ST_sclass(st), ST_name(st)));
    }
    newblk = Get_Section_ST_With_Given_Name (sec, 
                                             SCLASS_UNKNOWN, name);
    Set_ST_base(st, newblk);
#ifdef TARG_ST
    // [CG] Treat all nobits sections, not only bss.
    //    if (SEC_is_nobits(sec) && !STB_nobits(newblk)) {
    // (cbr) check if old object was uninitialzed global.

    // [VL] Handling Zeroinit_in_bss to fix #30279 Step 1
    if (!STB_nobits(newblk) &&
        (!ST_is_initialized(st) || 
         (Zeroinit_in_bss && ST_is_initialized(st) && ST_init_value_zero(st))) ) {

#else
        // if object was supposed to go into bss,
        // but was assigned to initialized data section,
        // then change the symbol to be initialized to 0.
        if (sec == _SEC_BSS && STB_section_idx(newblk) != _SEC_BSS) {
            DevWarn("change bss symbol to be initialized to 0");
#endif
#ifdef TARG_ST
            // [VL] Fix for #30279 Step 2
            if(ST_init_value_zero(st)) {
                Clear_ST_init_value_zero (st);
            }
#endif
            Set_ST_sclass(st, SCLASS_DGLOBAL);
            Set_ST_is_initialized(st);
            INITO_IDX ino = New_INITO(st);
            INITV_IDX inv = New_INITV();
            INITV_Init_Pad (inv, ST_size(st));
            Set_INITO_val(ino, inv);
        }
        ST_Block_Merge (newblk, st, 0, 0, SEC_max_sec_size(sec));
    }
    else
        FmtAssert(FALSE, ("unexpected section attribute"));
} 

struct Assign_Section_Names 
{
    inline void operator() (UINT32, ST_ATTR *st_attr) const {
        ST *st;
        STR_IDX name;
        if (ST_ATTR_kind (*st_attr) != ST_ATTR_SECTION_NAME)
            return;
        st = ST_ptr(ST_ATTR_st_idx(*st_attr));
        name = ST_ATTR_section_name(*st_attr);
        Assign_ST_To_Named_Section (st, name);
    }
};

// return section name for corresponding ST via st_attr table
struct find_st_attr_secname {
        ST_IDX st;
        find_st_attr_secname (const ST *s) : st (ST_st_idx (s)) {}

        BOOL operator () (UINT, const ST_ATTR *st_attr) const {
            return (ST_ATTR_kind (*st_attr) == ST_ATTR_SECTION_NAME &&
                    ST_ATTR_st_idx (*st_attr) == st);
        }
};

STR_IDX
Find_Section_Name_For_ST (const ST *st)
{
    ST_IDX idx = ST_st_idx (st);
    ST_ATTR_IDX d;

    d = For_all_until (St_Attr_Table, ST_IDX_level (idx),
                          find_st_attr_secname(st));
    FmtAssert(d != 0, ("didn't find section name for ST %s", ST_name(st)));
    return ST_ATTR_section_name(St_Attr_Table(ST_IDX_level (idx), d));
}


/*
 * In -64 there are two areas for the parameters:
 * 1) any stack area needed for the overflow of parameters
 * is put above the FP, in the caller.
 * 2) for varargs we store all registers immediately after the FP in the callee.
 * We can also use the same space for homing the formals in -O0.
 * When RVI is done (-O2) we won't see the store into this space.
 * Also can check the st flag to see if the variable was used before allocating.
 * TODO:  don't allocate space if -O2.
 * In -32 there is only the arg build-area in the caller.
 * Calc_Actual_Area is used for area 1;
 * Min_Formal_Area_Bytes is used for area 2.
 * Calc_Actual_Area calculates the area, but does not allocate it.
 * Use Allocate_Formal_Offsets routine to allocate.
 */

static INT32
Calc_Actual_Area ( TY_IDX pu_type, WN *pu_tree )
{
  INT size;
  PLOC ploc;
  // size = (pu_type != NULL ? Get_PU_arg_area_size(pu_type) : 0);
  size = 0; // The above method fails because PU typoes may be incomplete or shared
  if (size == 0) {
	INT i;
	INT num_parms;
	INT regsize = MTYPE_RegisterSize(Spill_Int_Mtype);
	INTRINSIC	id;

	switch (WN_operator(pu_tree)) {
	case OPR_PICCALL:
	case OPR_ICALL:
	case OPR_CALL:
#ifndef TARG_ST
#ifdef KEY
	case OPR_PURE_CALL_OP:
#endif
#endif
		num_parms = WN_num_actuals(pu_tree);
       		ploc = Setup_Output_Parameter_Locations(pu_type);
       		for (i = 0; i < num_parms; i++) {
		  ploc = Get_Output_Parameter_Location (TY_Of_Parameter(WN_actual(pu_tree,i)));
#ifdef TARG_X8664
		  if (ploc.reg == 0)  // not passed in register
		    size = PLOC_total_size(ploc);
#endif
		}
#ifndef TARG_X8664
		size = PLOC_total_size(ploc);
#endif
		break;
	case OPR_INTRINSIC_OP:
	case OPR_INTRINSIC_CALL:
		id = (INTRINSIC) WN_intrinsic(pu_tree);

		switch(id) {
		case INTRN_CONCATEXPR:
			size = 5 * regsize;
			break;
#ifdef TARG_X8664
		case INTRN_F4COS:
		case INTRN_F8COS:
		case INTRN_FQCOS:
		case INTRN_F16COS:
		case INTRN_F4SIN:
		case INTRN_F8SIN:
		case INTRN_FQSIN:
		case INTRN_F16SIN:
		  if( Is_Target_32bit() &&
		      CIS_Allowed ){
		    /* A later lower_cis_intrinsic will add two more ptr
		       arguments.
		     */
		    size += 2 * MTYPE_byte_size( Pointer_Mtype );
		  }
		  /* fall thru */
#endif
		default:
			num_parms = WN_num_actuals(pu_tree);
		       /*
			*  CQ/C16 return via hidden argument
			*  Assumption:  CQ/C16 Intrinsics always have
			*  CQ/C16 parameter after the hidden argument,
			*  so pad the space.
			*/
			if (MTYPE_id(WN_rtype(pu_tree)) == MTYPE_CQ)
				size = ROUNDUP(regsize, MTYPE_align_best(MTYPE_CQ));
			if (MTYPE_id(WN_rtype(pu_tree)) == MTYPE_C16)
				size = ROUNDUP(regsize, MTYPE_align_best(MTYPE_C16));
			if (INTRN_by_value(id) == TRUE)
			{	
				for(i= 0; i<num_parms; i++) {
					size += ROUNDUP(TY_size(TY_Of_Parameter(WN_actual(pu_tree,i))), regsize);
				}
			}
			else	/* by reference */
			{
				size += num_parms * regsize;
			}
			break;
		}
		break;
	default:
		FmtAssert(FALSE, ("Calc_Actual_Area: unexpected opcode"));
	}
	/* if not varargs and have prototype, then store size in TY;
	 * else will have to recalculate it each time. */
	/* Note:  the TY_parms may be incomplete due to implicit arguments */
	if (pu_type != (TY_IDX) NULL && TY_has_prototype (pu_type) &&
	    !TY_is_varargs (pu_type)) {
		Set_PU_arg_area_size(pu_type, size);
	}
  }
  size -= Formal_Save_Area_Size;
  if (size < 0) 
    size = 0;
  else if (Trace_Frame)
	fprintf(TFile, "<lay> actual_arg_area = %d\n", size);

  return size;
}

/*
 * Verify that stack space needed for actuals doesn't overflow
 * what we initially reserved.
 */
extern void
Check_Actual_Stack_Size (WN *call_tree)
{
  INT32 actual_size;
  switch (WN_operator(call_tree)) {
  case OPR_CALL:
  case OPR_PICCALL:
    actual_size = Calc_Actual_Area ( ST_pu_type(WN_st(call_tree)), call_tree);
    break;
  case OPR_ICALL:
    actual_size = Calc_Actual_Area ( WN_ty(call_tree), call_tree);
    break;
  default:
    FmtAssert(FALSE, ("unexpected opcode in Check_Actual_Stack_Size"));
  }
#ifndef TARG_X8664
  FmtAssert(actual_size <= Current_PU_Actual_Size,
        ("size of actual area increased from %d to %d",
        Current_PU_Actual_Size, actual_size));
#else
  FmtAssert(actual_size <= Current_PU_Actual_Size || Is_Target_32bit(),  
			      // intrinsic returning complex can cause increase
			      // in actual area size after inserting fake parm
        ("size of actual area increased from %d to %d",
        Current_PU_Actual_Size, actual_size));
#endif
}

/* ====================================================================
 *
 * Max_Arg_Area_Bytes
 *
 * This functions finds the largest amount of stack space is required
 * for passing parameter to all functions within this PU.  It goes
 * through the WHIRL tree to find all function calls in this PU.
 * For each call, use PLOCs to calculate the number of bytes required
 * for that call.
 *
 * ====================================================================
 */
static INT32
Max_Arg_Area_Bytes(WN *node)
{
  OPCODE opcode;
  INT32 maxsize = 0;

  opcode = WN_opcode (node);

  switch (OPCODE_operator(opcode)) {
  case OPR_CALL:
  case OPR_PICCALL:
#ifndef TARG_ST
#ifdef KEY
  case OPR_PURE_CALL_OP:
#endif
#endif
    maxsize = Calc_Actual_Area ( ST_pu_type (WN_st (node)), node);
    Frame_Has_Calls = TRUE;
    break;
  case OPR_ICALL:
    maxsize = Calc_Actual_Area ( WN_ty(node), node );
    Frame_Has_Calls = TRUE;
    break;
  case OPR_INTRINSIC_OP:
  case OPR_INTRINSIC_CALL:
#ifdef TARG_ST
    //TB: dynamic intrinsics and dynamic MTYPE
    if (!(Inline_Intrinsics_Allowed||
          !INTRN_runtime_exists(WN_intrinsic(node))) &&
        INTRN_cg_intrinsic(WN_intrinsic(node))) {
#endif    
    maxsize = Calc_Actual_Area ( (TY_IDX) NULL, node );
    Frame_Has_Calls = TRUE;
#ifdef TARG_ST
    }
#endif
    break;
#if defined(TARG_IA32) || defined(TARG_X8664)
  case OPR_IO:
    if( Is_Target_32bit() ){
      maxsize = 16;
    }
    break;
#endif
    
#ifdef TARG_ST
  case OPR_LDID:
  case OPR_STID:
  case OPR_LDA:
    {
      ST *sym = WN_st (node);
      if (ST_is_thread_private (sym)) {
	ST_TLS_MODEL tls_model = ST_tls_model (sym);
	if (tls_model == ST_TLS_MODEL_GLOBAL_DYNAMIC
	    || tls_model == ST_TLS_MODEL_LOCAL_DYNAMIC) {
	  Frame_Has_Calls = TRUE;
	}
      }
    }
    break;
#endif

  }

  if (opcode == OPC_BLOCK) {
    WN *wn;
    for ( wn = WN_first(node); wn != NULL; wn = WN_next(wn) ) {
      INT32 wn_size;
      if (((WN_opcode(wn) == OPC_PRAGMA) || (WN_opcode(wn) == OPC_XPRAGMA)) &&
	  (WN_pragma(wn) == WN_PRAGMA_COPYIN)) {
#ifdef TARG_ST
	FmtAssert(FALSE,("Max_Arg_Area_Bytes: pragma_copyin WN ??"));
#else
	INT32 copyincount = 3;
	WN *next;
	while ((next = WN_next(wn)) &&
	       ((WN_opcode(next) == OPC_PRAGMA) ||
		(WN_opcode(next) == OPC_XPRAGMA)) &&
	       (WN_pragma(next) == WN_PRAGMA_COPYIN)) {
	  copyincount += 2;
	  wn = next;
        }
	wn_size = copyincount * MTYPE_RegisterSize(Spill_Int_Mtype);
#endif
	maxsize = MAX(maxsize, wn_size);
    	Frame_Has_Calls = TRUE;
      } else {
	wn_size = Max_Arg_Area_Bytes (wn);
	maxsize = MAX(maxsize, wn_size);
      }
    }
  }
  else if (!OPCODE_is_leaf(opcode)) {
    INT i;

    for (i = 0; i < WN_kid_count(node); i++) {
      if (WN_kid(node, i) != NULL) {
	INT32 wn_size = Max_Arg_Area_Bytes (WN_kid(node, i));
	maxsize = MAX(maxsize, wn_size);
      }
    }
  }

  return maxsize;
}

/* ====================================================================
 *
 * Reset_UPFORMAL_Segment(ST *s)
 *
 * Reset the UPFORMAL areas offset and size back to zero
 * This is to simulate the UPFORMAL offset to create an ST * to
 * store to s during ALTENTRY formals
 *
 * ====================================================================
 */
void
Reset_UPFORMAL_Segment(void)
{
  ST *block;

  if (block = SF_Block(SFSEG_UPFORMAL))
  {
    Set_ST_ofst(block, 0);
    Set_STB_size(block, 0);
  }
  else
  {
    Initialize_Frame_Segment (SFSEG_UPFORMAL, SCLASS_AUTO, INCREMENT);
  }
}

/*
 *  Allocate each formal parameter on the stack,
 * 
 */
#ifdef TARG_ST
/* This interface uses explicit parameters. */
static void Allocate_Entry_Formal(
  ST   *formal, 
  BOOL  on_stack, 
  BOOL  in_formal_reg,
  INT   lpad = 0,
  INT   rpad = 0,
  INT   lpad_overlap = 0
  );

/* This interface uses the PLOC information for formal parameters. */
static void
Allocate_Entry_Formal(ST *formal, PLOC ploc);
#endif /* TARG_ST */

/* Implementation of Allocate_Entry_Formal(). */
#ifdef TARG_ST
static void
Allocate_Entry_Formal(ST *formal, PLOC ploc)
{
  BOOL on_stack = PLOC_on_stack(ploc);
  BOOL in_formal_reg = Is_Formal_Preg(PLOC_reg(ploc));
  INT lpad = PLOC_lpad(ploc);
  INT rpad = PLOC_rpad(ploc);
  INT lpad_overlap = PLOC_lpad_overlap(ploc);

  Allocate_Entry_Formal(formal,
			PLOC_on_stack(ploc),
			Is_Formal_Preg(PLOC_reg(ploc)),
			PLOC_lpad(ploc),
			PLOC_rpad(ploc),
			PLOC_lpad_overlap(ploc));
  
}
#endif

/* Implementation of Allocate_Entry_Formal(). */
#ifdef TARG_ST
static void Allocate_Entry_Formal(
  ST   *formal, 
  BOOL  on_stack, 
  BOOL  in_formal_reg,
  INT   lpad,
  INT   rpad,
  INT   lpad_overlap
  )
#else
static void
Allocate_Entry_Formal(ST *formal, BOOL on_stack, BOOL in_formal_reg)
#endif
{
  if (Has_No_Base_Block(formal))
  {
   /*
    *  Formals that are vms static have entry points that use this address.
    *  Master kludge around this by allocating this variable to a static.
    */
    if (PU_has_altentry(Get_Current_PU()) && ST_declared_static(formal))
    {
	SECTION_IDX sec;
	// formal will not be on stack
	Set_ST_sclass(formal, SCLASS_PSTATIC);
	Clear_ST_is_value_parm(formal);
	sec = Shorten_Section(formal, _SEC_BSS);
    	Allocate_Object_To_Section(formal, sec, Adjusted_Alignment(formal));
    }
#ifndef TARG_MIPS // bug 12772
    else if (in_formal_reg)
    {
      /* parameter is in register, so put in FORMAL area */
#ifdef TARG_ST
      Add_Object_To_Frame_Segment (formal, SFSEG_FORMAL, lpad, rpad, TRUE);
#else
      Add_Object_To_Frame_Segment ( formal, SFSEG_FORMAL, TRUE );
#endif
    }
#endif
    else if (on_stack)
    {
      /* parameter is on stack, so put in either UPFORMAL or FTEMP area */
      if (PU_has_altentry(Get_Current_PU())) {
#ifdef TARG_ST
	Add_Object_To_Frame_Segment (formal, SFSEG_FTEMP, lpad, rpad, TRUE);
#else
    	Add_Object_To_Frame_Segment ( formal, SFSEG_FTEMP, TRUE );
#endif
      }
      else {
#ifdef TARG_ST
	// [CG] We must account for formals that are in UPFORMAL seg
	// while FORMAL_SEG is not yet completed due to alignment constraints for instance.
	// This information is given by the lpad_overlap value.
	if (lpad_overlap > 0) {
	  if (SF_Block(SFSEG_FORMAL) != NULL) {
	    ST *formal = SF_Block(SFSEG_FORMAL);
	    // Check that formal area size has the expected size in this case.
	    // Indeed this case assumes that the formal and upformal_sefgment are continuous,
	    // hence the lpad_overlap is used to complete the formal size up to Formal_Save_Area_Size.
	    FmtAssert(Max_Formal_Save_Area_Size == Formal_Save_Area_Size,
		      ("Unexpected formal size: Max_Formal_Save_Area_Size (%s) != Formal_Save_Area_Size (%s)", Max_Formal_Save_Area_Size, Formal_Save_Area_Size));
	    FmtAssert(STB_size(formal) == Formal_Save_Area_Size - lpad_overlap,
		      ("unexpected formal size ( STB_size(formal)[%d] + lpad[%d] !=  Formal_Save_Area_Size[%d] ) for symbol %s\n", STB_size(formal), lpad, Formal_Save_Area_Size, ST_name(formal)));
	    Set_STB_size(formal, STB_size(formal) + lpad_overlap);
	  }
	  lpad -= lpad_overlap;
	}    
	Add_Object_To_Frame_Segment (formal, SFSEG_UPFORMAL, lpad, rpad, TRUE);
#else
      	Add_Object_To_Frame_Segment ( formal, SFSEG_UPFORMAL, TRUE );
#endif
      }
    }
#ifdef TARG_MIPS // bug 12772
    else if (in_formal_reg)
    {
      /* parameter is in register, so put in FORMAL area */
      Add_Object_To_Frame_Segment ( formal, SFSEG_FORMAL, TRUE );
    }
#endif
    else 
    {
	// formal not in usual parameter reg and not on stack
	// (e.g. passing addr in return reg), so store in FTEMP.
#ifdef TARG_ST
      Add_Object_To_Frame_Segment (formal, SFSEG_FTEMP, lpad, rpad, TRUE);
#else
    	Add_Object_To_Frame_Segment ( formal, SFSEG_FTEMP, TRUE );
#endif
    }
  }
}




/* ====================================================================
 *
 * ST *formal_sym_type(WN *formal)
 *
 *  create a "base" ST for the formal ref that represents the
 *  address of the formal
 *
 * ====================================================================
 */
static TY_IDX Formal_ST_type(ST *sym)
{
  TY_IDX type= ST_type(sym);

  if (ST_sclass(sym) == SCLASS_FORMAL_REF)
  {
    return Make_Pointer_Type(type);
  }
  return type;
}
#ifdef TARG_ST
SF_VARARG_INFO sf_vararg_info;
#else /* !TARG_ST */
/* indexed list of vararg symbols */
# if MAX_NUMBER_OF_REGISTER_PARAMETERS > 0
    ST *vararg_symbols[MAX_NUMBER_OF_REGISTER_PARAMETERS];	
# else
    // zero length arrays not allowed.
#   define vararg_symbols ((ST * *)0)  // Not accessed
# endif
#endif

/* this is just so we don't have symbols dangling around in next PU */
static void
Clear_Vararg_Symbols (void)
{
#ifdef TARG_ST
	memset(&sf_vararg_info, 0, sizeof(sf_vararg_info));
#else
	INT i;
	for (i = 0; i < MAX_NUMBER_OF_REGISTER_PARAMETERS; i++) {
		vararg_symbols[i] = NULL;
	}
#endif
}

#ifdef TARG_ST
/*
 * Get_Vararg_Ploc_Symbol()
 *
 * See interface description.
 */
void
Get_Vararg_Ploc_Symbol (PLOC ploc, ST **st, INT *offset)
{
  if (Preg_Offset_Is_Int(PLOC_reg(ploc))) {
    *st = sf_vararg_info.var_int_save_base;
    *offset = (PLOC_reg(ploc) - (First_Int_Preg_Param_Offset + sf_vararg_info.fixed_int_slots)) * sf_vararg_info.int_slot_size + sf_vararg_info.var_int_save_offset;
  } else if (Preg_Offset_Is_Float(PLOC_reg(ploc))) {
    *st = sf_vararg_info.var_float_save_base;
    *offset = (PLOC_reg(ploc) - (First_Float_Preg_Param_Offset + sf_vararg_info.fixed_float_slots)) * sf_vararg_info.float_slot_size + sf_vararg_info.var_float_save_offset;
  } else {
    DevAssert(0, ("PLOC type not supported for varargs"));
  }
}

/*
 * Get_Vararg_Preg_Symbol()
 *
 * See interface description.
 */

ST *
Get_Vararg_Preg_Symbol (PLOC ploc)
{
  if (Preg_Offset_Is_Int(PLOC_reg(ploc))) {
    return Int_Preg;
  } else if (Preg_Offset_Is_Float(PLOC_reg(ploc))) {
    return Float_Preg;
  } else if (Preg_Offset_Is_Ptr(PLOC_reg(ploc))) {
    return Ptr_Preg;
  } else {
    DevAssert(0, ("PLOC type not supported for varargs"));
  }
}

/*
 * Get_Vararg_Save_Area_Info()
 *
 * See interface description.
 */
void
Get_Vararg_Save_Area_Info(SF_VARARG_INFO *vararg_info)
{
  *vararg_info = sf_vararg_info;
}
#endif

#ifndef TARG_ST

/* get vararg symbol that corresponds to ploc value */
extern ST*
Get_Vararg_Symbol (PLOC ploc)
{
#ifndef TARG_X8664
	Is_True(PLOC_reg(ploc) < 
		First_Int_Preg_Param_Offset+MAX_NUMBER_OF_REGISTER_PARAMETERS,
		("Get_Vararg_Symbol:  ploc %d out of range", PLOC_reg(ploc)));
	return vararg_symbols[PLOC_reg(ploc)-First_Int_Preg_Param_Offset];
#else
	if (Preg_Offset_Is_Int(PLOC_reg(ploc)))
	  return vararg_symbols[PLOC_reg(ploc)-First_Int_Preg_Param_Offset];
	else
	  return vararg_symbols[PLOC_reg(ploc)-First_Float_Preg_Param_Offset+MAX_NUMBER_OF_INT_REGISTER_PARAMETERS];
#endif
}
#endif

#ifdef TARG_X8664
/* used by the lower to determine the number of fixed integer and float
 * parameters passed in registers and return the symbol corresponding to the 
 * first variable register parameter save location; it assumes that the first 
 * 6 entries of vararg_symbols are for the int parameter registers and the next 
 * 8 entries are for the float parameter registers; we do not allocate save
 * area for the fixed portion of the integer parameter registers */
ST *Get_Vararg_Save_Area_Info(int &fixed_int_parms, 
			      int &fixed_float_parms,
			      ST* &upformal) {
  INT i;
  for (i = MAX_NUMBER_OF_INT_REGISTER_PARAMETERS; 
       i > 0 && vararg_symbols[i-1] != NULL;
       i--)
    ;
  fixed_int_parms = i;
  for (i = MAX_NUMBER_OF_REGISTER_PARAMETERS; 
       i > MAX_NUMBER_OF_INT_REGISTER_PARAMETERS && vararg_symbols[i-1] != NULL;
       i--)
    ;
  fixed_float_parms = i - MAX_NUMBER_OF_INT_REGISTER_PARAMETERS;
  upformal = SF_Block(SFSEG_UPFORMAL);
  if (fixed_int_parms < MAX_NUMBER_OF_INT_REGISTER_PARAMETERS)
    return vararg_symbols[fixed_int_parms];
  else if (fixed_float_parms < MAX_NUMBER_OF_FLOAT_REGISTER_PARAMETERS)
    return vararg_symbols[fixed_float_parms+MAX_NUMBER_OF_INT_REGISTER_PARAMETERS];
  else return NULL;
}
#endif
#ifdef TARG_ST
/*
 * Allocate_Vararg_Formals ()
 *
 * Allocate formals if the pu ia a vararg function.
 * It as been separated from the Allocate_Vararg_Formal() of
 * non vararg functions to simplify the code.
 * [CG] Obsoleted alt_entry code as we cannot validate it.
 */
static void
Allocate_Vararg_Formals (WN *pu)
{
  TY_IDX pu_type = ST_pu_type (WN_st (pu));
  BOOL varargs = TY_is_varargs (pu_type);

  DevAssert(varargs, ("Failed precondition."));
  DevAssert(!PU_has_altentry(Get_Current_PU()), ("Not implemented"));

  /* Simulate the parameter passing for varargs. */
  PLOC ploc;
  INT i;

  if (Trace_Frame) {
    fprintf(TFile, "<lay> %s for PU %s\n", __FUNCTION__,
	    ST_name(WN_st(pu)));
  }

  ploc = Setup_Input_Parameter_Locations(pu_type);

  for (i = 0; i < WN_num_formals(pu); i++) {
    ST *sym = WN_st(WN_formal(pu, i));
    ploc = Get_Input_Parameter_Location(Formal_ST_type(sym));
    sym =  Formal_Sym(sym, PLOC_on_stack(ploc) || varargs); 
    Allocate_Entry_Formal (sym, ploc);

    if (Trace_Frame) {
      fprintf(TFile, "<lay> %s (formal: %s): %s [preg %d]; lpad %d; size %d; rpad %d; formal_offset %d; upformal_offset %d;\n", 
	      ST_name(WN_st(WN_formal(pu, i))),
	      ST_name(sym),
	      PLOC_on_stack(ploc) ? "on stack" : "in a register",
	      PLOC_reg(ploc),
	      PLOC_lpad(ploc),
	      PLOC_size(ploc),
	      PLOC_rpad(ploc),
	      PLOC_formal_offset(ploc), PLOC_upformal_offset(ploc));
    }
  }
  
  /*
   *  For varargs, the func-entry just has the list of fixed
   *  parameters, so also have to allocate the vararg registers.
   */
  
  INT num_var_plocs = 0;
  INT int_slot_size = 0;
  INT float_slot_size = 0;
  INT num_int_slots = 0;
  INT num_float_slots = 0;
  BOOL start_int_offset_set = FALSE;
  BOOL start_float_offset_set = FALSE;
  INT start_int_offset = 0;
  INT start_float_offset = 0;
  INT next_int_offset = 0;
  INT next_float_offset = 0;
  PREG_NUM start_int_preg = First_Int_Preg_Param_Offset;
  PREG_NUM start_float_preg = First_Float_Preg_Param_Offset;
  
  /* don't do if already reached stack params */
  ploc = Get_Vararg_Input_Parameter_Location (ploc);
  while (!PLOC_on_stack(ploc)) {
    num_var_plocs++;
    PREG_NUM preg = PLOC_reg(ploc);
    if (Preg_Offset_Is_Int(preg)) {
      if (! start_int_offset_set) {
	start_int_offset_set = TRUE;
	start_int_offset = PLOC_formal_offset(ploc);
	start_int_preg = preg;
      }
      int_slot_size = PLOC_size(ploc);
      num_int_slots++;
      next_int_offset = PLOC_formal_offset(ploc) + PLOC_size(ploc) + PLOC_rpad(ploc);
    } else if (Preg_Offset_Is_Float(preg)) {
      if (! start_float_offset_set) {
	start_float_offset_set = TRUE;
	start_float_offset = PLOC_formal_offset(ploc);
	start_float_preg = preg;
      }
      float_slot_size = PLOC_size(ploc);
      num_float_slots++;
      next_float_offset = PLOC_formal_offset(ploc) + PLOC_size(ploc) + PLOC_rpad(ploc);
    } else {
      DevAssert(0, ("PLOC type not supported for varargs"));
    }
    DevAssert(PLOC_lpad(ploc) == 0 && PLOC_rpad(ploc) == 0, 
	      ("Unexpected padding for vararg parameters"));
    
    if (Trace_Frame) {
      fprintf(TFile, "<lay> %s(): vararg slot in register [preg %d] %s.\n",
	      __FUNCTION__,
	      preg,
	      Preg_Offset_Is_Int(preg) ? "is_int":
	      Preg_Offset_Is_Float(preg) ? "is_float": "is_other");
    }
    
    ploc = Get_Vararg_Input_Parameter_Location (ploc);
  }
  
  if (num_var_plocs > 0) {
    INT int_size = int_slot_size * num_int_slots;
    INT float_size = float_slot_size * num_float_slots;
    /* Some consistency checks. */
    DevAssert(int_size == next_int_offset - start_int_offset, ("slot size and ploc size mismatch for int: int_size (%d) != next_int_offset (%d) - start_int_offset (%d)", int_size, next_int_offset, start_int_offset));
    DevAssert(float_size == next_float_offset - start_float_offset, ("slot size and ploc size mismatch for float: float_size (%d) != next_float_offset(%d) - start_float_offset(%d)", float_size, next_float_offset, start_float_offset));
    
    /* Compute the total size and alignment of the register vararg area for. */
    INT size = int_size + float_size;
    INT align = MAX(int_slot_size, float_slot_size);
    TY_IDX vararg_array_ty_idx = Make_Array_Type(MTYPE_I1, 
						 1, size);
    Set_TY_align(vararg_array_ty_idx, align);
    /* Set flag for alias analyzer */
    Set_TY_no_ansi_alias (vararg_array_ty_idx);
    ST *sym = Gen_Temp_Symbol(vararg_array_ty_idx, "vararg_array");
    Set_ST_sclass (sym, SCLASS_FORMAL);
    Set_ST_is_value_parm(sym);
    Set_ST_addr_saved(sym);
    Allocate_Entry_Formal(sym, 
			  FALSE, /* Formals not on stack. */ 
			  TRUE, /* Formals in registers. */
			  0, 0 , 0 /* No padding. */);
    sf_vararg_info.flags = SF_VARARG_INFO_HAS_STACK;
    if (num_int_slots > 0) sf_vararg_info.flags |= SF_VARARG_INFO_HAS_INT;
    if (num_float_slots > 0) sf_vararg_info.flags |= SF_VARARG_INFO_HAS_FLOAT;
    sf_vararg_info.fixed_int_slots = start_int_preg - First_Int_Preg_Param_Offset;
    sf_vararg_info.int_slot_size = int_slot_size;
    sf_vararg_info.fixed_float_slots = start_float_preg - First_Float_Preg_Param_Offset;
    sf_vararg_info.float_slot_size = float_slot_size;
    /* If the float plocs came first we order the float area first. */
    if (start_float_offset < start_int_offset) {
      sf_vararg_info.var_int_save_base = sym;
      sf_vararg_info.var_int_save_offset = float_size;
      sf_vararg_info.var_float_save_base = sym;
      sf_vararg_info.var_float_save_offset = 0;
    } else {
      sf_vararg_info.var_int_save_base = sym;
      sf_vararg_info.var_int_save_offset = 0;
      sf_vararg_info.var_float_save_base = sym;
      sf_vararg_info.var_float_save_offset = int_size;
    }
    sf_vararg_info.var_stack_base = SF_Block(SFSEG_UPFORMAL);
    sf_vararg_info.var_stack_offset = STB_size(SF_Block(SFSEG_UPFORMAL));

    if (Trace_Frame) {
      fprintf(TFile, "<lay> allocated vararg area %s, size: %d, align %d, int_size: %d, int_offset: %d, float_size: %d,  float_offset: %d, fixed_int_slots: %d, fixed_float_slots: %d\n",
	      ST_name(sym),
	      size, align, 
	      int_size, sf_vararg_info.var_int_save_offset,
	      float_size, sf_vararg_info.var_float_save_offset,
	      sf_vararg_info.fixed_int_slots,
	      sf_vararg_info.fixed_float_slots
	      );
      }
  } else {
    sf_vararg_info.flags = 
      SF_VARARG_INFO_HAS_STACK;
    sf_vararg_info.var_stack_base = SF_Block(SFSEG_UPFORMAL);
    sf_vararg_info.var_stack_offset = STB_size(SF_Block(SFSEG_UPFORMAL));
  }
}
#endif

/*
 * Search for all formals, including alt-entry formals,
 * and allocate them.  This way regions can always access the formals.
 */
static void
Allocate_All_Formals (WN *pu)
{
  INT i;
  PLOC ploc;
  ST *sym;
  BOOL varargs;
  TY_IDX pu_type = ST_pu_type (WN_st (pu));
#ifdef TARG_ST
  DevAssert(!PU_has_altentry(Get_Current_PU()), ("Not implemented"));
#endif
  Init_ST_formal_info_for_PU (WN_num_formals(pu));
  varargs = TY_is_varargs (pu_type);
#ifdef TARG_ST
  /* For vararg functions, call specific routine. */
  if (varargs) {
    Allocate_Vararg_Formals (pu);
    return;
  }
#endif

  ploc = Setup_Input_Parameter_Locations(pu_type);

 /*
  *  for varargs functions do not put the scalar formal base in a preg
  */
  for (i = 0; i < WN_num_formals(pu); i++)
  {
    sym = WN_st(WN_formal(pu, i));
 
    ploc = Get_Input_Parameter_Location( Formal_ST_type(sym));

    sym =  Formal_Sym(sym, PLOC_on_stack(ploc) || varargs);
#ifdef TARG_ST
    Allocate_Entry_Formal (sym, ploc); 
#else
    Allocate_Entry_Formal (sym, PLOC_on_stack(ploc), Is_Formal_Preg(PLOC_reg(ploc)));
#endif
  }
#ifndef TARG_ST
  if ( PU_has_altentry(Get_Current_PU()))
  {
    WN *tree;

    for ( tree = WN_first(WN_func_body(pu)); tree != NULL; tree = WN_next(tree))
    {
      if (WN_opcode(tree) == OPC_ALTENTRY)
      {
	Reset_UPFORMAL_Segment();
	ploc = Setup_Input_Parameter_Locations(ST_pu_type(WN_st(tree)));
	for (i = 0; i < WN_kid_count(tree); i++)
	{
	  sym = WN_st(WN_formal(tree, i));

	  ploc = Get_Input_Parameter_Location ( Formal_ST_type(sym));

	  sym =  Formal_Sym(sym, PLOC_on_stack(ploc) || varargs);

    	  Allocate_Entry_Formal (sym, TRUE, Is_Formal_Preg(PLOC_reg(ploc)));
	}
      }
    }
  }

  if (varargs) {
	/*
	 *  For varargs, the func-entry just has the list of fixed
	 *  parameters, so also have to allocate the vararg registers.
	 *  The lowerer needs to find these symbols, so put them in list
	 *  that get_vararg_symbol can use.
	 */

	ST *last_fixed_symbol = sym;
	PLOC last_fixed_ploc = ploc;
	/* vararg registers must be integer registers */
	TY_IDX vararg_type = Copy_TY(ST_type(Int_Preg));
	/* set flag for alias analyzer */
	Set_TY_no_ansi_alias (vararg_type);
#ifdef TARG_X8664
	TY_IDX vararg_type2 = Copy_TY(MTYPE_To_TY(MTYPE_F16));
	/* set flag for alias analyzer */
	Set_TY_no_ansi_alias (vararg_type);
#endif

#ifndef TARG_X8664  // x86-64 always needs to call Get_Vararg_Parameter_Location
	if (PLOC_is_nonempty(ploc) && !PLOC_on_stack(ploc)) {
		/* don't do if already reached stack params */
#endif
		ploc = Get_Vararg_Input_Parameter_Location (ploc);
#ifndef TARG_X8664
	}
#endif

#ifdef TARG_X8664
	if (!PLOC_on_stack(ploc) && PLOC_size(ploc) == 8 && 
	    (PLOC_reg(ploc) & 1) == 0) {
	  /* need to align to a non-16-byte boundary, otherwise, there'll be a
	   * gap between the int reg and xmm reg save areas */
	  sym = Gen_Temp_Symbol (vararg_type, "vararg");
	  Set_ST_addr_saved(sym);
	  Add_Object_To_Frame_Segment ( sym, SFSEG_FTEMP, TRUE );
	}
	BOOL float_seen = FALSE;
#endif
	while (!PLOC_on_stack(ploc)) {
		/*
		 *  create a temporary for the vararg formal
		 */
#ifdef TARG_X8664
	    	if (! float_seen && PLOC_size(ploc) == 16) {
		  /* first time processing a float register */
		  float_seen = TRUE;
		  /* allocate unused XMM save locations for the fixed parms */
		  for (INT k = 0; 
		       k < (PLOC_reg(ploc)-First_Float_Preg_Param_Offset);
		       k++) {
		    sym = Gen_Temp_Symbol (vararg_type2, "vararg");
	  	    Set_ST_addr_saved(sym);
		    Add_Object_To_Frame_Segment ( sym, SFSEG_FTEMP, TRUE );
		  }
		}
	    	if (PLOC_size(ploc) == 16) 
		  sym = Gen_Temp_Symbol (vararg_type2, "vararg");
		else
#endif
		sym = Gen_Temp_Symbol (vararg_type, "vararg");
		Set_ST_sclass (sym, SCLASS_FORMAL);
		Set_ST_is_value_parm( sym);
		Set_ST_addr_saved(sym);
#ifdef TARG_X8664
		if (Preg_Offset_Is_Int(PLOC_reg(ploc)))
#endif
		vararg_symbols[PLOC_reg(ploc)-First_Int_Preg_Param_Offset] = sym;
#ifdef TARG_X8664
		else if( Preg_Offset_Is_Float(PLOC_reg(ploc)) )
		  vararg_symbols[PLOC_reg(ploc)-First_Float_Preg_Param_Offset+MAX_NUMBER_OF_INT_REGISTER_PARAMETERS] = sym;
		else
		  FmtAssert( FALSE, ("Unknown vararg symbol type.") );
#endif
#ifndef TARG_X8664
#ifdef TARG_ST
    Allocate_Entry_Formal (sym, ploc); 
#else
		Allocate_Entry_Formal(sym, PLOC_on_stack(ploc), Is_Formal_Preg(PLOC_reg(ploc)));
#endif
		/*
		 * The optimizer sees these varargs references as *ap+n,
		 * i.e. as an offset past the last fixed arg, so to help
		 * aliasing work properly we set the base of these symbols
		 * to the last fixed symbol rather than to the formal_stkseg.
		 */
		Set_ST_base(sym, last_fixed_symbol);
		Set_ST_ofst(sym, PLOC_offset(ploc) - PLOC_offset(last_fixed_ploc));
#else
		Add_Object_To_Frame_Segment ( sym, SFSEG_FTEMP, TRUE );
#endif

		ploc = Get_Vararg_Input_Parameter_Location (ploc);
	}
  }
#endif
}

/* For stack parameters in altentry functions, 
 * we need an ST that corresponds to the upformal (fp+0,fp+8,etc) area.
 * In this case we copy the formal from the upformal to the ltemp area,
 * because the upformal area will not be the same for all entries.
 * The lowerer will generate the whirl to copy from upformal to ltemp.
 * We use the ploc offset so not dependent on calling this in order
 * (with formal-ref, we process these params in separate passes).
 * Note that the upformal area doesn't need to be allocated, just assigned.
 */
extern ST*
Get_Altentry_UpFormal_Symbol (ST *formal, PLOC ploc) 
{
  ST* upformal = Copy_ST(formal);
  Set_ST_name(upformal, Save_Str2(ST_name(upformal), ".upformal."));
  Set_ST_sclass(upformal, SCLASS_FORMAL);	// this is a stack location
  Clear_ST_gprel(upformal);
#ifdef TARG_ST
  INT offset = PLOC_upformal_offset(ploc) + STACK_OFFSET_ADJUSTMENT;
#else
  INT offset = PLOC_offset(ploc) - Formal_Save_Area_Size;
#endif
  if (PUSH_FRAME_POINTER_ON_STACK && PU_has_alloca(Get_Current_PU())) {
    offset += MTYPE_byte_size(Pointer_Mtype);
  }
#ifdef TARG_X8664  // always reserve FP save location to maintain quad align
  else offset += MTYPE_byte_size(Pointer_Mtype);
#endif
  if (PUSH_RETURN_ADDRESS_ON_STACK) {
    offset += MTYPE_byte_size(Pointer_Mtype);
  }

  Assign_Object_To_Frame_Segment (upformal, SFSEG_UPFORMAL, offset);
  return upformal;
}

#ifdef TARG_ST
extern ST*
Get_UpFormal_Base_Symbol ()
{
  ST *sym = Gen_Temp_Symbol(MTYPE_To_TY (MTYPE_V), ".upformal.base");
  Set_ST_sclass (sym, SCLASS_FORMAL);
  Assign_Object_To_Frame_Segment (sym, SFSEG_UPFORMAL, 0);
  return sym;
}
#endif

#ifdef TARG_ST
/* ====================================================================
 * Calc_Formal_Size
 *
 * Calculate size needed for formals and upformals parameters.
 * ====================================================================
 */
static void
Calc_Formal_Size (WN *pu_tree, INT32 *formal_size, INT32 *upformal_size )
{
  INT maxsize;
  ST *st = WN_st (pu_tree);
  TY_IDX pu_ty = ST_pu_type (st);

  FmtAssert(WN_opcode(pu_tree) == OPC_FUNC_ENTRY, ("not a func-entry"));

  if (TY_is_varargs (pu_ty)) {
    *formal_size = Vararg_Formal_Save_Area_Size;
    *upformal_size = 0; /* Unknown apriori. */
  } 
  else {
    PLOC ploc;
    INT i;
    INT prev_offset = 0;
    *formal_size = 0;
    *upformal_size = 0;
    ploc = Setup_Input_Parameter_Locations(pu_ty);
    
    for (i = 0; i < WN_num_formals(pu_tree); i++) {
      // [CG] use Formal_ST_type()
      ST *formalST = WN_st(WN_formal(pu_tree,i));
      BOOL is_struct = Is_Struct_Input_Parameter (Formal_ST_type(formalST));
      ploc = Get_Input_Parameter_Location (Formal_ST_type(formalST));
      
      if (is_struct) Setup_Struct_Input_Parameter_Locations(Formal_ST_type(formalST));
      do {
	if (is_struct) {
	  ploc = Get_Struct_Input_Parameter_Location(ploc);
	}
	if(!PLOC_on_stack(ploc)) {
	  *formal_size = PLOC_formal_offset(ploc) + PLOC_size(ploc) + PLOC_rpad(ploc);
	} else if (PLOC_on_stack(ploc)) {
	  if (PLOC_lpad_overlap(ploc) > 0) {
	    // Padding into the formal area. See other references to lpad_overlap.
	    *formal_size = PLOC_formal_offset(ploc);
	  } 
	  *upformal_size = PLOC_upformal_offset(ploc) + PLOC_size(ploc) + PLOC_rpad(ploc);
	}
      } while (is_struct && PLOC_is_nonempty(ploc));
    }
  }
  
  /* Formal area cannot be larger than max formal save area size.
     In the case where objects overlap between formal and upformal, we
     consider that the formal size is fixed to Max_Formal_Save_Area_Size. */
  *formal_size = MIN(*formal_size, Max_Formal_Save_Area_Size);
}
#else
/* Calculate size needed for formals and upformals */
/* TODO:  account for FTEMP area needed for alt-entry */
static void 
Calc_Formal_Area (WN *pu_tree, INT32 *formal_size, INT32 *upformal_size)
{
  INT maxsize;
  ST *st = WN_st (pu_tree);
  TY_IDX pu_ty = ST_pu_type (st);
  FmtAssert(WN_opcode(pu_tree) == OPC_FUNC_ENTRY, ("not a func-entry"));

  if (TY_is_varargs (pu_ty)) {
	*formal_size = Formal_Save_Area_Size;
	*upformal_size = 0;
	/* don't set PU_arg_area_size, cause changes with each call */
	return;
  } 
  else if (Get_PU_arg_area_size(pu_ty) > 0) {
	;	/* size is known */
  } 
  else {
	PLOC ploc;
	INT i;
#ifdef TARG_X8664
	Set_PU_arg_area_size(pu_ty, 0); // initialize to 0
#endif
	ploc = Setup_Input_Parameter_Locations(pu_ty);
	for (i = 0; i < WN_num_formals(pu_tree); i++) {
	  ploc = Get_Input_Parameter_Location (TY_Of_Parameter(WN_formal(pu_tree,i)));
#ifdef TARG_X8664
	  if (ploc.reg == 0)  // not passed in register
	    Set_PU_arg_area_size(pu_ty, PLOC_total_size(ploc));
#endif
	}
#ifndef TARG_X8664
	Set_PU_arg_area_size(pu_ty, PLOC_total_size(ploc));
#endif
  }
  maxsize = Get_PU_arg_area_size(pu_ty);
  if ( PU_has_altentry(Get_Current_PU())) {
	/* need to reserve upformal space for maximum of alt-entries */
	/* Note that formal space may be smaller than save-area-size
	 * yet still have upformal space, because altentry formals are put
	 * in ftemp area. */
	WN *tree = WN_first(WN_func_body(pu_tree));
	for (; tree != NULL; tree = WN_next(tree)) {
	    if (WN_opcode(tree) == OPC_ALTENTRY) {
		PLOC ploc;
		INT i;
      		ploc = Setup_Input_Parameter_Locations(ST_pu_type(WN_st(tree)));
      		for (i = 0; i < WN_kid_count(tree); i++) {
        		ploc = Get_Input_Parameter_Location (TY_Of_Parameter(WN_formal(tree,i)));
		}
		maxsize = MAX(maxsize, PLOC_total_size(ploc));
	    }
	}
  }
  /* formal area cannot be larger than save-area size */
  *formal_size = MIN (Get_PU_arg_area_size(pu_ty), Formal_Save_Area_Size);
  /* upformal size is 0 or size > save-area size */
  *upformal_size = MAX (maxsize - Formal_Save_Area_Size, 0);
}
#endif
#ifdef TARG_ST
/* ====================================================================
 * Initialize_Formal_Area
 *
 * Calculate size needed for formals and upformals 
 * [CG] Obsoleted alt_entry code as we cannot validate it.
 *
 * ====================================================================
 */
static void 
Initialize_Formal_Area (WN *pu_tree)
{
  INT maxsize;
  ST *st = WN_st (pu_tree);
  TY_IDX pu_ty = ST_pu_type (st);
  INT32 formal_size, upformal_size;

  FmtAssert(WN_opcode(pu_tree) == OPC_FUNC_ENTRY, ("not a func-entry"));
  DevAssert(!PU_has_altentry(Get_Current_PU()), ("Not implemented"));

  Max_Formal_Save_Area_Size = Default_Max_Formal_Save_Area_Size;
  Vararg_Formal_Save_Area_Size = Default_Vararg_Formal_Save_Area_Size;
  
  Calc_Formal_Size(pu_tree, &formal_size, &upformal_size);

  Formal_Save_Area_Size = formal_size;
  Upformal_Area_Size = upformal_size;
}
#endif


/* estimate stack space needed for local variables */

static inline INT64
Calc_Local_Area (void)
{
    INT64 local_size = 0;
    ST_ITER first = Scope_tab[CURRENT_SYMTAB].st_tab->begin ();
    const ST_ITER last = Scope_tab[CURRENT_SYMTAB].st_tab->end ();

    while (++first != last) {
	ST &st = *first;
	switch (ST_sclass (st)) {

	case SCLASS_AUTO:
	    /* ignore symbols that already been merged into a block;
	     * only count the root block. */
	    if (ST_base_idx (st) != ST_st_idx (st))
		break;
	    if (Trace_Frame)
		fprintf (TFile, "local %s has size %" SCNd64 "\n", ST_name(&st),
#ifdef KEY
			 // C++ requires distinct addresses for empty classes
			 (ST_size(&st) == 0 ? 1 : 0) +
#endif
			 ST_size(&st)); 
	    local_size += ST_size(&st);
#ifdef KEY
	    if (ST_class(&st) == CLASS_VAR && (!Current_pu ||
               (PU_src_lang (Get_Current_PU()) & PU_CXX_LANG)) && ST_size(&st) == 0)
	      local_size++;
#endif
	    break;
	    
	case SCLASS_DGLOBAL:
	case SCLASS_COMMON:
	    /*  the compiler has promised the Adjusted_Alignment to earlier phases
	     */
	    if (ST_type (st) != 0) {
		Set_ST_type (st, Make_Align_Type (ST_type (st),
						  Adjusted_Alignment (&st)));
	    }
	    break;
	}
    }

    return local_size;
}



/*
 *  Clear the segment descriptors: 
 */
static void 
Init_Segment_Descriptors(void)
{
  SF_SEGMENT s;

  for (s = (SF_SEGMENT) (SFSEG_UNKNOWN+1); s <= SFSEG_LAST; s = (SF_SEGMENT) (s + 1) )
  {
   /* use positive direction by default,
    * change later when decide stack model. 
    */
    SF_Block(s) = Create_Local_Block(INCREMENT,
				     Save_Str2 (SF_Name(s),"_StkSeg"));
    SF_Maxsize(s) = MAX_SFSEG_BYTES;
  }
  // mark formal block as unused until someone references it
  Set_ST_is_not_used(SF_Block(SFSEG_FORMAL));
  #ifdef TARG_ST
  // [CG]
  // mark formal block as basereg as objects may overlap between formal and upformal
  // thus overflowing the formal block size.
  // This property avoids the warning in symtab_verify in this case.
  Set_STB_is_basereg(SF_Block(SFSEG_FORMAL));
#endif

}

/* get segment size from either the block size or maxsize */
#define SEG_SIZE(s)	\
	(SF_Block(s) ? \
	 STB_size(SF_Block(s)) : \
	((SF_Maxsize(s) == MAX_SFSEG_BYTES) ? 0 : SF_Maxsize(s) ) )


static void
Init_Formal_Segments (INT32 formal_size, INT32 upformal_size)
{
#ifndef TARG_X8664
  SF_Maxsize ( SFSEG_UPFORMAL ) = ROUNDUP(upformal_size, stack_align);
  SF_Maxsize ( SFSEG_FORMAL ) = ROUNDUP(formal_size, stack_align);
#endif
}

/* ====================================================================
 *
 * Init_Frame_For_PU
 *
 * Initialize the SF segments for a new PU.
 *
 * ====================================================================
 */

static void
Init_Frame_For_PU (INT32 actual_size)
{
  /* Alloca depends on the actual_size that we first guessed,
   * because the alloca reserves actual_size bytes below it and
   * then points above the actual_area.
   * So never change actual_size, even if turns out to be smaller.
   */
  SF_Maxsize ( SFSEG_ACTUAL ) = ROUNDUP(actual_size, stack_align);

  /* temps can use max frame size minus the fixed space required for
   * formals and actuals. */
  SF_Maxsize ( SFSEG_FTEMP ) = 
	MAX_FRAME_OFFSET - SEG_SIZE(SFSEG_FORMAL) - SEG_SIZE(SFSEG_ACTUAL);

  /* Initialize the size of a large object.  Note that if we can
   * determine in advance the size of the frame per the FE, and it is
   * small, we should use infinity instead of the default:
   */
  Large_Object_Bytes = DEFAULT_LARGE_OBJECT_BYTES;
}

/* ====================================================================
 *
 * Bind_Stack_Frame
 *
 * It binds the stack segments according to the stack model.
 *
 * ====================================================================
 */

static void
Bind_Stack_Frame ( ST *SP_baseST, ST *FP_baseST )
{
  Set_Direction (Get_Direction(SP_baseST), SF_Block(SFSEG_ACTUAL));
  Set_Direction (Get_Direction(SP_baseST), SF_Block(SFSEG_UPFORMAL));
  /* within formal seg, object grow upward, toward FP */
  Set_Direction (Get_Direction(SP_baseST), SF_Block(SFSEG_FORMAL));

  switch (Current_PU_Stack_Model) {
  case SMODEL_SMALL:
#ifdef TARG_ST
      Set_Direction (Get_Direction(SP_baseST), SF_Block(SFSEG_REGSAVE));
    Set_Direction (Get_Direction(SP_baseST), SF_Block(SFSEG_FTEMP));
    break;
#endif
  case SMODEL_LARGE:
#ifdef TARG_ST
     Set_Direction (Get_Direction(SP_baseST), SF_Block(SFSEG_REGSAVE));
#endif
    Set_Direction (Get_Direction(SP_baseST), SF_Block(SFSEG_FTEMP));
    break;
  case SMODEL_DYNAMIC:
#ifdef TARG_ST
     Set_Direction (Get_Direction(SP_baseST), SF_Block(SFSEG_REGSAVE));
#endif
    Set_Direction (Get_Direction(FP_baseST), SF_Block(SFSEG_FTEMP));
    break;
  default:
    FmtAssert ( FALSE,
	       ("UNDEFINED Stack Model in Bind_Stack_Frame" ));
  }
}

#define MERGE_SEGMENT(base, seg, mxfrm) \
  ST_Block_Merge (base, SF_Block(seg), 0, 0, mxfrm)

/* ====================================================================
 *
 * Merge_Fixed_Stack_Frame
 *
 * At this point all but the TEMP segment are allocated, so merge the
 * other segments so that the offsets are exact.
 *
 * ====================================================================
 */
static void
Merge_Fixed_Stack_Frame(ST *SP_baseST, ST *FP_baseST)
{
  INT32 orig_formal_size;
  FmtAssert(SP_baseST != NULL && FP_baseST != NULL,
	    ("Initialize_Stack_Frame: Invalid parameters"));

  Set_STB_size(SP_baseST,0);
  Set_STB_size(FP_baseST,0);

  /* the actual area doesn't have allocated objects, just a size */
  Set_STB_size(SF_Block(SFSEG_ACTUAL), SF_Maxsize(SFSEG_ACTUAL));
  /* varargs formals are not allocated yet, so temporarily fake size
   * and then reset it. */
  BOOL pu_is_varargs = TY_is_varargs (ST_pu_type (Get_Current_PU_ST()));
  if (pu_is_varargs) {
	orig_formal_size = STB_size(SF_Block(SFSEG_FORMAL));
#ifndef TARG_X8664
  	Set_STB_size(SF_Block(SFSEG_FORMAL), SF_Maxsize(SFSEG_FORMAL));
#endif
  }

  switch ( Current_PU_Stack_Model) {
  case SMODEL_SMALL:
#ifdef TARG_ST
    MERGE_SEGMENT(SP_baseST, SFSEG_ACTUAL, MAX_SMALL_FRAME_OFFSET);
    /* only altentries may use the temp area, so merge it now */
    MERGE_SEGMENT(SP_baseST, SFSEG_FTEMP, MAX_SMALL_FRAME_OFFSET);
#else
    MERGE_SEGMENT(SP_baseST, SFSEG_ACTUAL, Max_Small_Frame_Offset);
    /* only altentries may use the temp area, so merge it now */
    MERGE_SEGMENT(SP_baseST, SFSEG_FTEMP, Max_Small_Frame_Offset);
#endif

#ifndef KEY
    // don't know offset of formals and upformals,
    // so leave alone until finalized.
#else
    /* attach formals and upformal to SP, but don't assign offsets till finalize */
    Set_ST_base(SF_Block(SFSEG_FORMAL), SP_baseST);
    Set_ST_base(SF_Block(SFSEG_UPFORMAL), SP_baseST);
#endif
    break;

  case SMODEL_LARGE:
    MERGE_SEGMENT(SP_baseST, SFSEG_ACTUAL, MAX_LARGE_FRAME_OFFSET);
    MERGE_SEGMENT(SP_baseST, SFSEG_FTEMP, MAX_LARGE_FRAME_OFFSET);
    Set_ST_base(SF_Block(SFSEG_UPFORMAL), FP_baseST);
    Set_ST_ofst (SF_Block(SFSEG_UPFORMAL), Stack_Offset_Adjustment);
    MERGE_SEGMENT(FP_baseST, SFSEG_FORMAL, MAX_LARGE_FRAME_OFFSET);
    break;

  case SMODEL_DYNAMIC:
    MERGE_SEGMENT(SP_baseST, SFSEG_ACTUAL, MAX_LARGE_FRAME_OFFSET);
    Set_ST_base(SF_Block(SFSEG_UPFORMAL), FP_baseST);
    Set_ST_ofst (SF_Block(SFSEG_UPFORMAL), Stack_Offset_Adjustment);

    MERGE_SEGMENT(FP_baseST, SFSEG_FORMAL, MAX_LARGE_FRAME_OFFSET);
    MERGE_SEGMENT(FP_baseST, SFSEG_FTEMP, MAX_LARGE_FRAME_OFFSET);
    break;

  default:
    FmtAssert ( FALSE,
	       ("UNDEFINED Stack Model in Initialize_Stack_Frame" ));
  }
  if (pu_is_varargs) {
	/* reset size */
	Set_STB_size(SF_Block(SFSEG_FORMAL), orig_formal_size);
  }
}

static STACK_MODEL
Choose_Stack_Model (INT64 frame_size)
{
  if (PU_has_alloca(Get_Current_PU())) {
    return SMODEL_DYNAMIC;
  } 
#ifdef TARG_X8664
  if (Opt_Level == 0 || Force_Frame_Pointer || Call_Mcount ||
      Debug_Level > 0) {
    return SMODEL_DYNAMIC; // needed to make debugging, gprofiling work
  } 
#endif
  else if (PU_has_nested(Get_Current_PU())) {
    return SMODEL_DYNAMIC;
  } 
  else if (Force_Large_Stack_Model) {
    return SMODEL_LARGE;
  }
#ifdef TARG_ST
  else if (frame_size < MAX_SMALL_FRAME_OFFSET) {
#else
  else if (frame_size < Max_Small_Frame_Offset) {
#endif
    return SMODEL_SMALL;
  } 
  else if (frame_size < MAX_LARGE_FRAME_OFFSET) {
    return SMODEL_LARGE;
  } 
  else {
    FmtAssert ( FALSE, ("64-bit stack frame NYI"));
    return SMODEL_UNDEF;
  }
}

/* create and allocate special space for a spill offset that is guaranteed 
 * to be within 16-bits of the SP or FP. */
static void
Allocate_Local_Spill_Sym (void)
{
  if (Current_PU_Stack_Model == SMODEL_SMALL) {
	/* not needed if frame is small anyways */
	Local_Spill_Sym = NULL;
  }
  else {
	Local_Spill_Sym = Gen_Temp_Symbol (Spill_Int_Type, "reserved_spill");
	Allocate_Temp_To_Memory (Local_Spill_Sym);
  }
}

#ifdef TARG_ST
/* ====================================================================
 *    Stack_Align_Check()
 *
 *   This function checks that the alignment constraint related
 *   to local symbols is compatible with the stack alignment.
 *
 *   Basically there are two phases: 
 *   1. determine the required alignment (accounts for function level
 *	alignment attribute and alignment of automatics);
 *   2. if Auto_align_stack is true, set the required alignment, 
 *	otherwise, emit a warning if it exceeds the natural stack alignment.
 *
 *   FdF 20080212: Note that this function cannot get alignment
 *   constraints from varargs parameters
 * ====================================================================
 */
static void Stack_Align_Check() {
  // [dt25] if symbol is a local defined as having an alignment 
  // constraint > PU_aligned_stack(Get_Current_PU()) then we should force the stack allignment

  INT i,Max_Stack_Align = PU_aligned_stack(Get_Current_PU());
  bool user_stack_align = false;
  bool Update = false;
  ST *sym;

  // We detect if the stack alignment has been set by the user
  if (Max_Stack_Align != Target_Stack_Alignment) user_stack_align = true;
  FOREACH_SYMBOL(CURRENT_SYMTAB,sym,i) {
    if (ST_class(sym) == CLASS_VAR && ST_sclass(sym) == SCLASS_AUTO && TY_align(ST_type(sym))>Max_Stack_Align) {
      Update = true;
      Max_Stack_Align = TY_align(ST_type(sym));
    }
  }
  
  if (Update && !Auto_align_stack ) {
    // Warn for undefined behavior
    ErrMsg(EC_LAY_Warn_Stack_Exceeded,ST_name(Get_Current_PU_ST()),(int)PU_aligned_stack(Get_Current_PU()),Max_Stack_Align);
  }

  if (Update && Auto_align_stack) {
    if (user_stack_align) {
      ErrMsg(EC_LAY_Warn_Stack_Align,ST_name(Get_Current_PU_ST()),(int)PU_aligned_stack(Get_Current_PU()),Max_Stack_Align);
      ErrMsg (EC_LAY_Warn_Stack_Modif,ST_name(Get_Current_PU_ST()),(int)PU_aligned_stack(Get_Current_PU()),Max_Stack_Align);
    }
    //ErrMsg (EC_Warn_Stack_Modif,ST_name(Get_Current_PU_ST()),(int)PU_aligned_stack(Get_Current_PU()),Max_Stack_Align);
    Set_PU_aligned_stack(Get_Current_PU(),Max_Stack_Align);
  }

}
#endif

/* ====================================================================
 *   Initialize_Stack_Frame
 * ====================================================================
 */
#ifdef TARG_ST
extern void
Initialize_Stack_Frame (WN *PU_tree)
{
  INT32 actual_size;
  INT32 formal_size;
  INT32 upformal_size;
  INT64 frame_size = 0;

  Set_Error_Phase("Data Layout");
  FmtAssert(WN_opcode(PU_tree) == OPC_FUNC_ENTRY,
	    ("Determine_Stack_Model: The PU_tree node does not point to a OPC_FUNC_ENTRY"));

  if (ST_asm_function_st(*WN_st(PU_tree))) {
    // Do nothing for functions that merely wrap up file-scope
    // assembly code.
    return;
  }

  if (Trace_Frame)
    fprintf(TFile, "\n<lay> Determine_Stack_Model for %s\n", 
		                                 ST_name(WN_st(PU_tree)));

#ifdef TARG_ST
  // Arthur: Seems like a IA32 hack to me !!
#else
  if (PU_has_return_address(Get_Current_PU()) 
	&& MTYPE_byte_size(Pointer_Mtype) < MTYPE_byte_size(Spill_Int_Mtype) )
  {
	/* In n32 the __return_address is only 4 bytes (pointer),
	 * but need to force allocation of 8 bytes, with return_address
	 * pointing to last 4 bytes, so can do ld/sd
	 * and make kernel and dbx happy. */
	// set base of __return_address to 8-byte dummy symbol,
	// offset at 4 of the base.
	ST *ra_sym = Find_Special_Return_Address_Symbol();
	ST *st_base = New_ST ();
	ST_Init (st_base, Save_Str("return_address_base"), 
		ST_class(ra_sym), ST_sclass(ra_sym), ST_export(ra_sym), 
		MTYPE_To_TY(Spill_Int_Mtype) );
	Set_ST_base (ra_sym, st_base);
	Set_ST_ofst (ra_sym, MTYPE_byte_size(Spill_Int_Mtype) - MTYPE_byte_size(Pointer_Mtype));
  }
#endif

  Init_Segment_Descriptors();

  Initialize_Formal_Area (PU_tree);
  /* also can be ftemp size, but probably small */
  formal_size = Formal_Save_Area_Size;
  upformal_size = Upformal_Area_Size;
  
  if (Trace_Frame) {
    fprintf(TFile,"formal_size = %d, upformal_size = %d\n",formal_size,upformal_size);
  }

  Frame_Has_Calls = FALSE;
  actual_size = Max_Arg_Area_Bytes(PU_tree);

  // mp_io and alloca rely on the actual size being register-sized aligned
  actual_size = ROUNDUP(actual_size, MTYPE_byte_size(Spill_Int_Mtype));

  if (mp_io && actual_size < MTYPE_byte_size(Spill_Int_Mtype)) {
    actual_size = MTYPE_byte_size(Spill_Int_Mtype);
    Frame_Has_Calls = TRUE;
  }
  Current_PU_Actual_Size = actual_size;

  frame_size = Calc_Local_Area ();

  if (PUSH_FRAME_POINTER_ON_STACK) {
    // Reserve the space on stack for the old frame pointer (ia32)
    ST* old_fp = New_ST ();
    ST_Init (old_fp, 
             Save_Str("old_frame_pointer"),
             CLASS_VAR,
             SCLASS_AUTO,
             EXPORT_LOCAL,
             MTYPE_To_TY(Pointer_Mtype));
#ifdef TARG_ST
    Add_Object_To_Frame_Segment (old_fp, SFSEG_UPFORMAL, 0, 0, TRUE);
#else
    Add_Object_To_Frame_Segment (old_fp, SFSEG_UPFORMAL, TRUE);
#endif
    upformal_size += MTYPE_byte_size(Pointer_Mtype);
  }

  if (PUSH_RETURN_ADDRESS_ON_STACK) {
    // Reserve the space on stack for the return address (ia32)
    ST* ra_st = New_ST ();
    ST_Init (ra_st, 
             Save_Str("return_address"),
             CLASS_VAR,
             SCLASS_AUTO,
             EXPORT_LOCAL,
             MTYPE_To_TY(Pointer_Mtype));
#ifdef TARG_ST
    Add_Object_To_Frame_Segment (ra_st, SFSEG_UPFORMAL, 0, 0, TRUE);
#else
    Add_Object_To_Frame_Segment (ra_st, SFSEG_UPFORMAL, TRUE);
#endif
    upformal_size += MTYPE_byte_size(Pointer_Mtype);
  }

  if (Trace_Frame) {
    fprintf(TFile, "<lay> locals' size = %ld\n", frame_size);
    fprintf(TFile, "<lay> upformal size = %d, formal size = %d\n", 
		upformal_size, formal_size);
    fprintf(TFile, "<lay> actual size = %d\n", actual_size);
  }

  frame_size += formal_size + actual_size;

  /* add the 4K area for temporaries */
  frame_size += DEFAULT_TEMP_SPACE_BYTES;

  /* add upformal_size in case upformal is so large it needs
   * to be accessed via $fp. */
  frame_size += upformal_size;

#ifdef TARG_ST
  /* Automatic stack alignment */
  Stack_Align_Check();
#endif

  Current_PU_Stack_Model = Choose_Stack_Model(frame_size);
  if (Trace_Frame) {
    fprintf(TFile, "<lay> guess frame_size = %ld\n", frame_size);
    fprintf(TFile, "<lay> stack model = %d\n", Current_PU_Stack_Model);
  }

  /* stack grows down; sp objects grow up from sp; fp objects down from fp */
  stack_direction = DECREMENT;
  SP_Sym = Create_Base_Reg (".SP", INCREMENT);
  FP_Sym = Create_Base_Reg (".FP", DECREMENT);

  /* make sure all formals are allocated before we split into regions */
  Allocate_All_Formals (PU_tree);
  Init_Formal_Segments (formal_size, upformal_size);
}
#else

extern void
Initialize_Stack_Frame (WN *PU_tree)
{
  INT32 actual_size;
  INT32 formal_size;
  INT32 upformal_size;
  INT64 frame_size = 0;

  Set_Error_Phase("Data Layout");
  Trace_Frame = Get_Trace(TP_DATALAYOUT, 1);
  FmtAssert(WN_opcode(PU_tree) == OPC_FUNC_ENTRY,
	    ("Determine_Stack_Model: The PU_tree node does not point to a OPC_FUNC_ENTRY"));

  if (ST_asm_function_st(*WN_st(PU_tree))) {
    // Do nothing for functions that merely wrap up file-scope
    // assembly code.
    return;
  }

  if (Trace_Frame)
	fprintf(TFile, "<lay> Determine_Stack_Model for %s\n", 
		ST_name(WN_st(PU_tree)));

#ifndef TARG_X8664
  if (PU_has_return_address(Get_Current_PU()) 
	&& MTYPE_byte_size(Pointer_Mtype) < MTYPE_byte_size(Spill_Int_Mtype) )
  {
	/* In n32 the __return_address is only 4 bytes (pointer),
	 * but need to force allocation of 8 bytes, with return_address
	 * pointing to last 4 bytes, so can do ld/sd
	 * and make kernel and dbx happy. */
	// set base of __return_address to 8-byte dummy symbol,
	// offset at 4 of the base.
	ST *ra_sym = Find_Special_Return_Address_Symbol();
	ST *st_base = New_ST ();
	ST_Init (st_base, Save_Str("return_address_base"), 
		ST_class(ra_sym), ST_sclass(ra_sym), ST_export(ra_sym), 
		MTYPE_To_TY(Spill_Int_Mtype) );
	Set_ST_base (ra_sym, st_base);
	Set_ST_ofst (ra_sym, !Target_Is_Little_Endian ? 
	    MTYPE_byte_size(Spill_Int_Mtype) - MTYPE_byte_size(Pointer_Mtype) :
	    0);
  }
#endif

  Init_Segment_Descriptors();
  Init_PU_arg_area_size_array();

  Calc_Formal_Area (PU_tree, &formal_size, &upformal_size);
  /* also can be ftemp size, but probably small */

  Frame_Has_Calls = FALSE;
  actual_size = Max_Arg_Area_Bytes(PU_tree);
  // mp_io and alloca rely on the actual size being register-sized aligned
  actual_size = ROUNDUP(actual_size, MTYPE_byte_size(Spill_Int_Mtype));
  if (mp_io && actual_size < MTYPE_byte_size(Spill_Int_Mtype)) {
    actual_size = MTYPE_byte_size(Spill_Int_Mtype);
    Frame_Has_Calls = TRUE;
  }
  Current_PU_Actual_Size = actual_size;

  frame_size = Calc_Local_Area ();

  if (PUSH_FRAME_POINTER_ON_STACK) {
    // Reserve the space on stack for the old frame pointer (ia32);
    // this is needed even if FP is not used, to maintain quad align
    ST* old_fp = New_ST ();
    ST_Init (old_fp, 
             Save_Str("old_frame_pointer"),
             CLASS_VAR,
             SCLASS_FORMAL,
             EXPORT_LOCAL,
             MTYPE_To_TY(Pointer_Mtype));
    Add_Object_To_Frame_Segment (old_fp, SFSEG_UPFORMAL, TRUE);
    upformal_size += MTYPE_byte_size(Pointer_Mtype);
  }

  if (PUSH_RETURN_ADDRESS_ON_STACK) {
    // Reserve the space on stack for the return address (ia32)
    ST* ra_st;
#ifdef KEY // bug 12261: check before creating another return address symbol
    if ((ra_st = Find_Special_Return_Address_Symbol()) == NULL) {
#endif
      ra_st = New_ST ();
      ST_Init (ra_st, 
	       Save_Str("return_address"),
	       CLASS_VAR,
	       SCLASS_FORMAL,
	       EXPORT_LOCAL,
	       MTYPE_To_TY(Pointer_Mtype));
#ifdef KEY
    }
#endif
    Add_Object_To_Frame_Segment (ra_st, SFSEG_UPFORMAL, TRUE);
    upformal_size += MTYPE_byte_size(Pointer_Mtype);
  }

  if (Trace_Frame) {
	fprintf(TFile, "<lay> locals size = %" SCNd64 "\n", frame_size);
	fprintf(TFile, "<lay> upformal size = %d, formal size = %d\n", 
		upformal_size, formal_size);
	fprintf(TFile, "<lay> actual size = %d\n", actual_size);
  }
  frame_size += formal_size + actual_size;

  /* add the 4K area for temporaries */
  frame_size += DEFAULT_TEMP_SPACE_BYTES;

  /* add upformal_size in case upformal is so large it needs
   * to be accessed via $fp. */
  frame_size += upformal_size;

  Current_PU_Stack_Model = Choose_Stack_Model(frame_size);
  if (Trace_Frame) {
	fprintf(TFile, "<lay> guess frame_size = %" SCNd64 "\n", frame_size);
	fprintf(TFile, "<lay> stack model = %d\n", Current_PU_Stack_Model);
  }

  /* stack grows down; sp objects grow up from sp; fp objects down from fp */
  stack_direction = DECREMENT;
  SP_Sym = Create_Base_Reg (".SP", INCREMENT);
  FP_Sym = Create_Base_Reg (".FP", DECREMENT);

  /* make sure all formals are allocated before we split into regions */
  Allocate_All_Formals (PU_tree);
  Init_Formal_Segments (formal_size, upformal_size);
}
#endif
// this is called after lowering, when have more accurate view of code.
extern void
Calculate_Stack_Frame_Sizes (WN *PU_tree)
{
  if (ST_asm_function_st(*Get_Current_PU_ST())) {
    // Do nothing for functions that merely wrap up file-scope
    // assembly code.
    return;
  }

  INT32 actual_size;
  // calculate size of actual area.
  actual_size = Max_Arg_Area_Bytes(PU_tree);
  // mp_io and alloca rely on the actual size being register-sized aligned
  actual_size = ROUNDUP(actual_size, MTYPE_byte_size(Spill_Int_Mtype));
  if (Trace_Frame && actual_size != Current_PU_Actual_Size) {
	fprintf(TFile, "actual_size was %d, now is %d\n",
		Current_PU_Actual_Size, actual_size);
  }
  // only change size if increased
  // (with regions it may decrease if we don't see whole PU).
  Current_PU_Actual_Size = MAX(Current_PU_Actual_Size, actual_size);

  Init_Frame_For_PU (Current_PU_Actual_Size);
  Bind_Stack_Frame (SP_Sym, FP_Sym);
  /* merge fixed part of frame */
  Merge_Fixed_Stack_Frame (SP_Sym, FP_Sym);

  Allocate_Local_Spill_Sym ();	/* for later scheduler use */
}

/* ====================================================================
 *
 * Process_Stack_Variable
 *
 * Given a variable allocated to the stack, select the appropriate stack
 * frame segment for it, and deal with it.
 *
 * This routine is used for variables only!
 *
 * ====================================================================
 */

static void
Process_Stack_Variable ( ST *st )
{
   ST_SCLASS	sc;
   UINT64	size;
   BOOL		is_root_block = Is_Allocatable_Root_Block(st);
   ST 		*base;

   if (! is_root_block && ST_class(st) == CLASS_BLOCK) return;

   sc = ST_sclass(st);
   Is_True ( (sc == SCLASS_AUTO),
           ("Process_Stack_Variable: Invalid SCLASS %d\n",ST_sclass(st)) );

   size = (is_root_block) ? STB_size(st) : TY_size(ST_type(st));

   /* attach large objects to FP (if exists), else attach to SP */
   if (Current_PU_Stack_Model == SMODEL_SMALL || 
	(Current_PU_Stack_Model == SMODEL_LARGE && size < Large_Object_Bytes))
	base = SP_Sym;
   else
	base = FP_Sym;
   ST_Block_Merge (base, st, 0, 0, MAX_FRAME_OFFSET);
}

static void
Trace_Stack_Segments( const char *msg, ST *SP_baseST, ST *FP_baseST )
{
  SF_SEGMENT s;

  fprintf(TFile, "Stack Segment after \"%s\"\n", msg);
  fprintf(TFile, "SegName \toffset\tbase\tblksize\talign\tmaxsize\n");

  /* Clear the segment descriptors: */
  for ( s = SFSEG_FIRST; s <= SFSEG_LAST; s = (SF_SEGMENT) (s + 1) ) {
    ST          *block;
    INT64	size, offset;
    INT		align;
    const char	*basename;

    block = SF_Block(s);
    basename = block ? ST_name(ST_base(block)) : "<none>";
    size = block? STB_size(block): 0;
    align = block? STB_align(block): 0;
    offset = block? ST_ofst(block): 0;
    fprintf(TFile, "%s \t%" SCNd64 "\t%s\t%" SCNd64 "\t%d\t%" SCNd64 "\n",
	    SF_Name(s), offset, basename, size, align, SF_Maxsize(s));
  }
  if (SP_baseST != NULL)
	fprintf(TFile, "%s \t\t%" SCNd64 "\t\t%" SCNd64 "\t%d\n",
		ST_name(SP_baseST), ST_ofst(SP_baseST),
		STB_size(SP_baseST), STB_align(SP_baseST));
  if (FP_baseST != NULL)
	fprintf(TFile, "%s \t\t%" SCNd64 "\t\t%" SCNd64 "\t%d\n",
		ST_name(FP_baseST), ST_ofst(FP_baseST), 
		STB_size(FP_baseST), STB_align(FP_baseST));
}

#ifdef PV394813
/* go thru all initv and make sure their STs are allocated */
static void
Allocate_All_INITV (INITV *inv1)
{
	INITV *inv;
	if (inv1 == NULL) return;
	FOREACH_INITV(inv1, inv) {
		if ( INITV_kind(inv) == INITVKIND_SYMOFF) {
			/* may have BASED base, but not allocated */
			Allocate_Object(INITV_st(inv));
		}
		else if (INITV_kind(inv) == INITVKIND_BLOCK) {
			/* recurse on nested block */
			Allocate_All_INITV (INITV_blk(inv));
		}
	}
}
#endif


/* ====================================================================
 *
 * Finalize_Stack_Frame
 *
 * This be called immediately prior to emission to finalize offsets for use
 * in instructions.
 *
 * ====================================================================
 */
struct finalize_inito
{
    void operator() (UINT32, const INITO* inito) const {
	ST* st = INITO_st (*inito);
	if (Has_No_Base_Block(st))
	    Allocate_Object(st);
#ifdef PV394813
	Allocate_All_INITV (INITO_val(*inito));
#endif
    }
};
 
#ifdef NEW_FRAME_METHOD
INT64 Finalize_Stack_Frame (INT64 msize)
#else
INT64 Finalize_Stack_Frame (void)
#endif
{
  INT64 Frame_Size;
  ST *st;
  INT i;

  Set_Error_Phase("Data Layout");
  Clear_Vararg_Symbols();

  For_all (Inito_Table, CURRENT_SYMTAB, finalize_inito());

  /* if nested pu, then may not see reference in this pu, 
   * but still need to allocate for nested pu's. */
  if (PU_has_nested(Get_Current_PU())) {
    FOREACH_SYMBOL (CURRENT_SYMTAB, st, i) {
      if (ST_class(st) == CLASS_VAR && ST_has_nested_ref(st)) {
	Allocate_Object(st);
      }
    }
  }

  switch ( Current_PU_Stack_Model ) {
  case SMODEL_SMALL:
#ifdef TARG_ST
    if (!ST_is_not_used(SF_Block(SFSEG_REGSAVE)) ) {
      if (Trace_Frame) fprintf(TFile, "<lay> Before regsave merge: .SP %ld .FP %ld\n", STB_size(SP_Sym), STB_size(FP_Sym));
      MERGE_SEGMENT(SP_Sym, SFSEG_REGSAVE, MAX_SMALL_FRAME_OFFSET);
      if (Trace_Frame) fprintf(TFile, "<lay> After regsave merge: .SP %ld .FP %ld\n", STB_size(SP_Sym), STB_size(FP_Sym));
	    
    }
#endif

    // if formals are unused (and no upformals), 
    // then don't add formals to frame.
#ifdef TARG_ST
    // [SC] Only allocate formals if it is used,
    // even if upformals exists and is used
    if (ST_is_not_used(SF_Block(SFSEG_FORMAL)) )
#else
    if (SEG_SIZE(SFSEG_UPFORMAL) == 0 
	&& ST_is_not_used(SF_Block(SFSEG_FORMAL)) )
#endif
    {
	if (Trace_Frame) fprintf(TFile, "<lay> formals not used\n");
    }
    else {
#ifdef TARG_ST
      MERGE_SEGMENT(SP_Sym, SFSEG_FORMAL, MAX_SMALL_FRAME_OFFSET);
#else
	MERGE_SEGMENT(SP_Sym, SFSEG_FORMAL, Max_Small_Frame_Offset);
#endif
    }
    Frame_Size = STB_size(SP_Sym);
    Set_ST_base(SF_Block(SFSEG_UPFORMAL), SP_Sym);
#ifdef TARG_ST
    // [SC] Set the offset of the upformal area.
    // This is at the top of the stack frame, _above_ the incoming
    // push/pop area.  Normally, the top of the stack frame is overlaid on
    // the incoming push/pop area, but if the stack frame
    // is smaller than the push/pop area, we must explicitly move
    // the upformals area above the push/pop area.
    // Also take into account the outgoing push/pop area at the bottom of
    // the stack frame, which exists only if this function makes calls.
    {
      // Account for the outgoing push/pop area
      INT32 upformals_lpad = Frame_Has_Calls ? Stack_Offset_Adjustment : 0;

      if ((Frame_Size + upformals_lpad) < Stack_Offset_Adjustment) {
	// Pad up to Stack_Offset_Adjustment, because upformals area
	// must be at least Stack_Offset_Adjustment above SP.
	upformals_lpad += Stack_Offset_Adjustment
	  - (Frame_Size + upformals_lpad);
      }
      Assign_Offset(SF_Block(SFSEG_UPFORMAL), SP_Sym, upformals_lpad, 0);
    }
#else
        
    Assign_Offset(SF_Block(SFSEG_UPFORMAL), SP_Sym, 
	(Frame_Has_Calls ? Stack_Offset_Adjustment : 0), 0);
#endif

#ifdef NEW_FRAME_METHOD
    // Offset the upformals by the size of push/pop mask
    Set_ST_ofst(SF_Block(SFSEG_UPFORMAL),ST_ofst(SF_Block(SFSEG_UPFORMAL))+msize);
#endif

#ifdef TARG_X8664
    {
      int push_pop_int_saved_regs = (*Push_Pop_Int_Saved_Regs_p)();
      if (push_pop_int_saved_regs & 1)
	push_pop_int_saved_regs++;
      Set_ST_ofst(SF_Block(SFSEG_UPFORMAL), ST_ofst(SF_Block(SFSEG_UPFORMAL)) +
		    push_pop_int_saved_regs * MTYPE_byte_size(Pointer_Mtype));
    }
#endif
    break;

  case SMODEL_LARGE:
#ifdef TARG_ST
    if (!ST_is_not_used(SF_Block(SFSEG_REGSAVE)) ) {
      if (Trace_Frame) fprintf(TFile, "<lay> Before regsave merge: .SP %ld .FP %ld\n", STB_size(SP_Sym), STB_size(FP_Sym));
      MERGE_SEGMENT(SP_Sym, SFSEG_REGSAVE, MAX_LARGE_FRAME_OFFSET);
      if (Trace_Frame) fprintf(TFile, "<lay> After regsave merge: .SP %ld .FP %ld\n", STB_size(SP_Sym), STB_size(FP_Sym));
    }
#endif

    Frame_Size = STB_size(SP_Sym) + STB_size(FP_Sym);
    break;

  case SMODEL_DYNAMIC:
    Frame_Size = STB_size(SP_Sym) + STB_size(FP_Sym);
    break;

  default:
    FmtAssert ( FALSE,
	       ("UNDEFINED Stack Model in Finalize_Stack_Frame" ));
  }

#ifdef TARG_ST
  // the stack-frame-adjustment represents N bytes of free space
  // that is in the callers frame, and thus does not need to be
  // included in the new callee frame size if this is a leaf
  // (so don't need to reserve space for subsequent frames).
  if (!Frame_Has_Calls) {
    Frame_Size = MAX(0, Frame_Size - Stack_Offset_Adjustment);
  }

  if (Trace_Frame) {
    fprintf(TFile, "      Frame_Size = %ld\n", Frame_Size);
  }

  //
  // Arthur: I may need to adjust FORMALs/UPFORMALs.
  //         In case the frame is not a multiple of DEFAULT_STACK_ALIGNMENT,
  //         it will be offset by -lpad when rounded to DEFAULT_STACK_ALIGNMENT.
  //         In order for formals/upformals to be found at the
  //         right place, we need to reoffset it back by +lpad
  //
  if (Current_PU_Stack_Model == SMODEL_SMALL && 
      // [SC] The upformals area can have zero size but still be used,
      // in the case that it contains variadic arguments.  In that
      // case we still need to adjust the offset here.
      // In fact, we do not currently correctly set the used attribute
      // of the upformals block, so we can enter here even if the
      // upformals is not used; that is unnecessary but harmless.
      !(ST_is_not_used(SF_Block(SFSEG_UPFORMAL))
	&& ST_is_not_used(SF_Block(SFSEG_FORMAL)))) {
    INT32 lpad = ROUNDUP(Frame_Size, DEFAULT_STACK_ALIGNMENT)
                 - Frame_Size;
    // [SC] Only adjust SFSEG_FORMAL if it is being used, otherwise
    // it is not allocated, not given a base, and checks in symtab_verify
    // expect the offset to be zero.
    if (! ST_is_not_used(SF_Block(SFSEG_FORMAL))) {
      Set_ST_ofst(SF_Block(SFSEG_FORMAL),ST_ofst(SF_Block(SFSEG_FORMAL))+lpad);
    }
    if (! ST_is_not_used(SF_Block(SFSEG_UPFORMAL))) {
      Set_ST_ofst(SF_Block(SFSEG_UPFORMAL),ST_ofst(SF_Block(SFSEG_UPFORMAL))+lpad);
    }
    if (Trace_Frame) {
      fprintf(TFile, "      Formals adjustment = %d\n", lpad);
    }

  }

  // align
  Frame_Size = ROUNDUP(Frame_Size, DEFAULT_STACK_ALIGNMENT);

  if (Trace_Frame) {
    fprintf(TFile, "      final Frame_Size = %ld\n", Frame_Size);
  }

#else

  Frame_Size = ROUNDUP(Frame_Size, stack_align);
#ifdef TARG_X8664 // this is needed to maintain 16 bytes gap that contains
    		  // the save area for return-addr and frame-ptr
  if (Current_PU_Stack_Model == SMODEL_SMALL && PUSH_FRAME_POINTER_ON_STACK) {
    Frame_Size += MTYPE_byte_size(Pointer_Mtype);
    if ((*Push_Pop_Int_Saved_Regs_p)() & 1)
      Frame_Size += MTYPE_byte_size(Pointer_Mtype);
  }
#endif


  // the stack-frame-adjustment represents N bytes of free space
  // that is in the callers frame, and thus does not need to be
  // included in the new callee frame size if this is a leaf
  // (so don't need to reserve space for subsequent frames).
  if (!Frame_Has_Calls) {
	Frame_Size = MAX(0, Frame_Size - Stack_Offset_Adjustment);
  }
#endif
#if defined(BUILD_OS_DARWIN) || defined(__FreeBSD__)
// Darwin requires 16 byte alignment of the stack pointer even in -m32
// (otherwise, for example, dynamic linking code faults on a movdqa
// instruction). Because the "call" pushes a 4-byte return address onto the
// stack which isn't included in Frame_Size, it's not sufficient just to
// round up the Frame_Size. If the frame will use the stack pointer instead
// of the base pointer, then the offset to the arguments needs to take into
// account any padding we add.
if(! Is_Target_64bit() ) {
  int excess = MTYPE_byte_size(Pointer_Mtype) *
    ((Current_PU_Stack_Model == SMODEL_DYNAMIC) ?
    2 :
    1);
  int mod = (Frame_Size + excess) % 16;
  if (mod) {
    excess = 16 - mod;
    Frame_Size += excess;
    ST *upformal = SF_Block(SFSEG_UPFORMAL);
    if (Current_PU_Stack_Model == SMODEL_SMALL) {
      Set_ST_ofst(upformal, ST_ofst(upformal) + excess);
    }
  }
}
#endif /* defined(BUILD_OS_DARWIN) */

  if ( Trace_Frame ) {
    Trace_Stack_Segments ( "Finalize_Stack_Frame", SP_Sym, FP_Sym);
    fprintf(TFile, "<lay> final frame_size = %" SCNd64 "\n", Frame_Size);
  }
#ifdef TARG_ST
  if (Current_PU_Stack_Model == SMODEL_SMALL 
	&& ((Frame_Size + STB_size(SF_Block(SFSEG_UPFORMAL))) 
		> MAX_SMALL_FRAME_OFFSET))
#else
  if (Current_PU_Stack_Model == SMODEL_SMALL 
	&& ((Frame_Size + STB_size(SF_Block(SFSEG_UPFORMAL))) 
		> Max_Small_Frame_Offset))
#endif
  {
        DevWarn("upformals overflowed small stack frame; will try recompiling with -TENV:large_stack");
        Early_Terminate(RC_OVERFLOW_ERROR);
  }
#ifdef TARG_ST
  if (Current_PU_Stack_Model == SMODEL_LARGE && 
      Frame_Size < MAX_SMALL_FRAME_OFFSET)
	if (Trace_Frame) fprintf(TFile, "<lay> stack-model underflowed\n");
#endif
  return Frame_Size;
}


/* ====================================================================
 *
 * Allocate_Temp_To_Memory
 *
 * At the beginning if back end process for a module, the 
 *
 * ====================================================================
 */
void
Allocate_Temp_To_Memory ( ST *st )
{
  Is_True(ST_sclass(st) == SCLASS_AUTO, ("Allocate_Temp_To_Memory expect stack var"));
  Set_ST_is_temp_var(st);
  Process_Stack_Variable ( st );
#ifdef TARG_X8664
  (*CG_Set_Is_Stack_Used_p)();
#endif
}


/* ====================================================================
 *
 * STATIC/GLOBAL OBJECT LAYOUT RELATED FUNCTIONS
 *
 * ====================================================================
 */

 BOOL
Is_String_Literal (ST *st)
{
	if (ST_class(st) == CLASS_CONST && TCON_ty(STC_val(st)) == MTYPE_STR) {
		return TRUE;
	}
	/* sometimes strings are in const array vars */
	else if (ST_class(st) == CLASS_VAR && ST_is_const_var(st)
		&& ST_is_initialized(st) 
		&& TY_kind(ST_type(st)) == KIND_ARRAY
		&& TY_mtype(TY_AR_etype(ST_type(st))) == MTYPE_U1 )
	{
		return TRUE;
	}
	return FALSE;
}

#ifdef TARG_ST
/* ====================================================================
 *   Shorten_Section
 *
 *   Given the section, is there a "Short" section and can we put
 *   this object into it?  Return the section we should put this
 *   object into. We also try to figure out if the object can be put
 *   into a merged section. 
 * ====================================================================
 */
SECTION_IDX
Shorten_Section ( 
  ST *st, 
  SECTION_IDX sec 
)
{
   INT64       size;
   INT64       max_size;
   SECTION_IDX newsec;
   BOOL        is_root_block = Is_Allocatable_Root_Block(st);

   // if we are not generating GP-relative addressing, return
   if (Gen_GP_Relative == FALSE) return sec;

   if (!is_root_block && ST_class(st) == CLASS_BLOCK) return sec;

#ifdef TARG_ST
   if (! (/* clarkes 030912:
	     For  PIC code, we can put local or hidden objects
	     into GP relative section. */
	  (Gen_PIC_Shared && ! ST_is_preemptible(st))

	  /* clarkes 030912:
	     For call-PIC code, we can put a symbol into GP relative
	     section if we know it will be defined in this load
	     module. */
	  || (Gen_PIC_Call_Shared && ST_sclass(st) != SCLASS_EXTERN)

	  /* Any symbol forced gprel can be put in GP relative section. */
	  || (ST_class(st) == CLASS_VAR && ST_force_gprel(st))))
#else
   /* For PIC code, we can only put local or hidden objects into GP
      relative sections.  (unless is gprel)
   */
   if ((Gen_PIC_Shared || Gen_PIC_Call_Shared) && ST_is_preemptible(st)
   	&& ! (ST_class(st) == CLASS_VAR && ST_force_gprel(st)) )
#endif
   {
      return sec;
   }

   size = ST_size(st);
   /* size = 0 implies that we don't really know the size. e.g. int a[]; */
   if (size == 0) return sec;

   if (ST_class(st) == CLASS_VAR && ST_not_gprel(st)) 
	return sec;	/* user says not gp-relative */

   if (sec == _SEC_RDATA)
     max_size = Max_Srdata_Elt_Size;
   else
     max_size = Max_Sdata_Elt_Size;
   if (size > max_size) { 
	/* Object size is greater than the -G number.
	 * Unless user or previous phase has specified it to be GPREL,
	 * it can't be GP relative. */
	if (ST_class(st) != CLASS_VAR)
		return sec;
	/* else is CLASS_VAR */
	else if (!ST_gprel(st) && !ST_force_gprel(st))
		return sec;
   }

   if (Strings_Not_Gprelative && Is_String_Literal(st)) {
	/* don't put strings in a gp-relative section */
	return sec;
   }

   if ((ST_class(st) == CLASS_VAR || ST_class(st) == CLASS_CONST) && !ST_gprel(st)) 
   {
	/* Gspace specifies total amount of space we can use for things other
	 * than what ipa already allocated as gprel. */
	/* need to adjust size by possible alignment padding */
	size = MAX(size, Adjusted_Alignment(st));
	if (size <= Gspace_Available) {
		Gspace_Available -= size;
	}
	else {
	  /*
		if (Trace_Frame) fprintf(TFile, "<lay> not enough Gspace, so didn't assign %s to gprel section\n", ST_NAME(st));
		return sec;
	  */
	}
   }

   newsec = Corresponding_Short_Section (st, sec);

   return newsec;
}
#else

/* ====================================================================
 *
 * Shorten_Section
 *
 * Given the section, is there a "Short" section and can we put
 * this object into it?  Return the section we should put this
 * object into. We also try to figure out if the object can be put
 * into a merged section. 
 *
 * ====================================================================
 */
static SECTION_IDX
Shorten_Section ( ST *st, SECTION_IDX sec )
{
   INT64   size;
   SECTION_IDX newsec;
   BOOL     is_root_block = Is_Allocatable_Root_Block(st);

   if (! is_root_block && ST_class(st) == CLASS_BLOCK) return sec;

   /* For PIC code, we can only put local or hidden objects into GP
      relative sections.  (unless is gprel)
   */
   if ((Gen_PIC_Shared || Gen_PIC_Call_Shared) && ST_is_preemptible(st)
   	&& ! (ST_class(st) == CLASS_VAR && ST_force_gprel(st)) )
   {
      return sec;
   }

   newsec = Corresponding_Short_Section (sec);
   if (newsec == sec) return sec;	// won't be shortened

#ifdef TARG_X8664
   /* bug#539
      Do not generate gprel load/store to <newsec> if <newsec> is not gprel.
    */
   if( !SEC_is_gprel( newsec ) ){
     return sec;
   }
#endif

   size = ST_size(st);
   /* size = 0 implies that we don't really know the size. e.g. int a[]; */
   if (size == 0) return sec;

   if (ST_class(st) == CLASS_VAR && ST_not_gprel(st)) 
	return sec;	/* user says not gp-relative */
   if (size > Max_Sdata_Elt_Size) { 
	/* Object size is greater than the -G number.
	 * Unless user or previous phase has specified it to be GPREL,
	 * it can't be GP relative. */
	if (ST_class(st) != CLASS_VAR)
		return sec;
	/* else is CLASS_VAR */
	else if (!ST_gprel(st) && !ST_force_gprel(st))
		return sec;
   }
   if (Strings_Not_Gprelative && Is_String_Literal(st)) {
	/* don't put strings in a gp-relative section */
	return sec;
   }

   if ((ST_class(st) == CLASS_VAR || ST_class(st) == CLASS_CONST) && !ST_gprel(st)) 
   {
	/* Gspace specifies total amount of space we can use for things other
	 * than what ipa already allocated as gprel. */
	/* need to adjust size by possible alignment padding */
	size = MAX(size, Adjusted_Alignment(st));
	if (size <= Gspace_Available) {
		Gspace_Available -= size;
#if 0
		if (Trace_Frame) fprintf(TFile, "<lay> use Gspace for %s (size %d, align %d)\n", ST_NAME(st), TY_size(ST_type(st)), Adjusted_Alignment(st));
#endif
	}
	else {
		if (Trace_Frame) fprintf(TFile, "<lay> not enough Gspace, so didn't assign %s to gprel section\n", ST_NAME(st));
		return sec;
	}
   }
#if 0
else {
if (Trace_Frame) fprintf(TFile, "<lay> didn't check Gspace for %s\n", ST_NAME(st));
}
#endif

   if (sec == _SEC_RDATA && ST_class(st) == CLASS_CONST) {
     /* by default put all short .rodata items into .srdata, unless 
	we can put it into an appropriate merge section.
      */
       TCON tcon = STC_val(st);
       switch (TCON_ty (tcon)) {
       case MTYPE_F4:
       case MTYPE_I4:
       case MTYPE_U4:
	 newsec = _SEC_LIT4;
	 break;
       case MTYPE_F8:
       case MTYPE_I8:
       case MTYPE_U8:
	 newsec = _SEC_LIT8;
	 break;
       case MTYPE_FQ:
       case MTYPE_F16:
	 newsec = _SEC_LIT16;
	 break;
       }
   }
   Set_ST_gprel(st);
   return newsec;
}
#endif


/* ====================================================================
 *
 * Allocate object to a section
 * 
 * Assign an object to a section and then immediately allocate
 * space for it.   
 *
 * ST_ofst is set to the offset from ST_base.
 * ST_base is set to the base_sym of the section or altbase.
 *
 * Remember: "align" is minimum alignment expressed in bytes.
 *
 * ====================================================================
 */
static void
Allocate_Object_To_Section (ST *st, SECTION_IDX sec, UINT align)
{
  ST *block;
  INT64 max_size;
#ifdef TARG_ST
  align = MAX (SEC_entsize (sec), align);
#endif

  /* Everything ok? */
     Is_True( IS_POW2(align), ("Alignment %d must be power of 2",align));

  block = Get_Section_ST (sec, align, SCLASS_UNKNOWN /* cause varies */);
  max_size = SEC_max_sec_size(sec);
  if ( Trace_Frame ) {
    fprintf(TFile, "Allocating %s to %s", ST_name(st), ST_name(block));
  }
  ST_Block_Merge (block, st, 0, 0, max_size);
}


/* ====================================================================
 *
 * Assign object to section
 * 
 * This is the routine that assigns an object to a section.
 *
 * No space allocation is done.
 *
 * "align" is the minimum alignment expressed in bytes.
 *
 * ====================================================================
 */
static void
Assign_Object_To_Section( ST *st, SECTION_IDX sec, UINT align)
{
  ST *block;

  /* Everything ok? */
     Is_True( IS_POW2(align), ("Alignment %d must be power of 2",align));

  block = Get_Section_ST(sec, align, ST_sclass (st));
  if ( Trace_Frame ) {
    fprintf ( TFile, "Assigning symbol %s to %s section \n",
	     ST_name(st) ? ST_name(st) : "<null>", ST_name(block) );
  } 
  Set_ST_base(st, block);
}

static void
Allocate_Label (ST *lab)
{
  FmtAssert(ST_sclass(lab) == SCLASS_TEXT, ("non-text label?"));
  /* this is a reference only, no storage need be allocated */
  Assign_Object_To_Section ( lab, _SEC_TEXT, 0 );
}


// bug fix for OSP_138
/* =====================================================================
 *
 * ST_has_Predefined_Named_Section
 *
 * Given a symbol table element (representing a data object) with section
 * name attribute, see if its section name is same as one of the predefined
 * section's. If so, return TRUE and set sec_idx to appropriate section index.
 * Otherwise return FALSE and set sec_idx to _SEC_DISTR_ARRAY.
 *
 * =====================================================================
 */
BOOL
ST_has_Predefined_Named_Section(ST *st, SECTION_IDX &sec_idx) {

  for (sec_idx = _SEC_UNKNOWN; sec_idx <= _SEC_DISTR_ARRAY; sec_idx ++) {
  	if (SEC_name(sec_idx) &&
	    !strcmp(Index_To_Str(Find_Section_Name_For_ST(st)), SEC_name(sec_idx)))
  	  return TRUE;
  }

  return FALSE;
}

/* 
 * Auxiliary function of Allocate_Object that allocates object's
 * initializers
 * [vcdv]
 *
 * Parameter parent is used as a protection against infinite loop in
 * case some struct/array values are initialized using their own
 * adresses. [see bug #22236]
 */
static void
Allocate_Object_Initializer (ST* parent, INITV_IDX invidx)
{
  int i;
  ST* st;
  INITV_IDX ninv;
  INITV inv = Initv_Table[invidx];
  switch(INITV_kind(inv))
    {
#ifdef TARG_ST
    case INITVKIND_SYMOFF16:
#endif
    case  INITVKIND_SYMOFF:
      st = &St_Table[INITV_st(inv)];
      /* [dt25] Bug #35818 => guard not to infinite loop when cross references*/
      if (ST_is_initialized(st)) break;
      if (parent != st)
        Allocate_Object(st);
      break;
    case INITVKIND_BLOCK:
      for (i = 0; i < INITV_repeat1(inv); i++) {
        for (ninv = INITV_blk(inv); ninv; ninv = INITV_next(ninv)) {
          Allocate_Object_Initializer(parent, ninv);
        }
      }
      break;
    case INITVKIND_ZERO:
    case INITVKIND_ONE:
    case INITVKIND_VAL:
    case INITVKIND_LABEL:
#ifdef TARG_ST
    case INITVKIND_LABDIFF:
    case INITVKIND_SYMDIFF:
    case INITVKIND_SYMDIFF16:
    case INITVKIND_PAD:
#endif
    default:
      break;
    }
}


#ifdef TARG_ST
/**
 * Add an offset to upformal access
 */
extern void
Add_Offset_To_UpFormal(INT offs) {
  ST *upformal = SF_Block(SFSEG_UPFORMAL);
  if (upformal && !ST_is_not_used(upformal)) {
    Set_ST_ofst (upformal, ST_ofst(upformal)+offs) ;
  }
}
#endif

/* =====================================================================
 *
 * Allocate_Object_To_Predefined_Named_Section
 *
 * Given a symbol table element (representing a data object) with section
 * name attribute which is same as one of the predefined section's and
 * appropriate section index, chose the appropriate section, assign the ST
 * to that section, and allocate storage.
 *
 * =====================================================================
 */
static void
Allocate_Object_To_Predefined_Named_Section(ST *st, SECTION_IDX sec_idx) {
  Clear_ST_has_named_section(st);	// Clear st's flag for ST_HAS_NAMED_SECTION
  Set_ST_base_idx(st,ST_st_idx(st));	// Set st's base index equel to st index
  Set_ST_ofst(st,0);			// Set st's offset to 0

  // if object was supposed to go into bss,
  // but with section attribute which hase same
  // the same name as one of the predefined section's,
  // then change the symbol to be initialized to 0.
  // Otherwise, the order of generated assembly code may be wrong.
  if (ST_storage_class (*st) == SCLASS_UGLOBAL) {
    DevWarn("change bss symbol to be initialized to 0");
    Set_ST_sclass(st, SCLASS_DGLOBAL);
    Set_ST_is_initialized(st);
    INITO_IDX ino = New_INITO(st);
    INITV_IDX inv = New_INITV();
    INITV_Init_Pad (inv, ST_size(st));
    Set_INITO_val(ino, inv);
    Clear_ST_init_value_zero(st);
  }

  /*Assign st to appropriate section and allocate storage*/
  Allocate_Object_To_Section ( st, sec_idx, Adjusted_Alignment(st));
  return;
}


/* ====================================================================
 *
 * Allocate_Object
 *
 * Given a symbol table element (representing a data object) with valid
 * storage class and unassigned section (SEC_UNKNOWN), chose the appropriate
 * section, assign the ST to that section, and allocate storage.
 *
 * In general, if you have decided which section to allocate this in,
 * then call Allocate_Object_To_Section; if you don't know and are
 * willing to let this decide, then call Allocate_Object.
 *
 * ====================================================================
 */

extern void
Allocate_Object ( ST *st )
{
  SECTION_IDX  sec;
  ST *base_st = st;

  if ( ST_sclass(st) == SCLASS_FORMAL ) {
    /* mark that formal-stack-arg-area is needed */
#ifdef TARG_ST
    // [SC] If it is in the upformal area, then this
    // is not a requirement for the formal area.
    // Q. Why not just do
    //  Clear_ST_is_not_used(ST_base(st))
    // A. Because I am cautious.  I am not sure if
    // ST_base(st) is always finalized here.
    if (ST_base(st) != SF_Block(SFSEG_UPFORMAL))
#endif
    Clear_ST_is_not_used(SF_Block(SFSEG_FORMAL));
  }
#ifdef TARG_ST
  //[TB] When the size is not already known the object must not be
  //allocated for the moment
  if (ST_is_not_sized(st)) return ;
#endif

  if (Is_Allocated(st)) {
    // bug fix for OSP_138
    /* If st has section name attribute and the section name is same as
     * one of the predefined section, it will be processed later */
    if (!ST_has_named_section(st))
      return;
    else if (!ST_has_Predefined_Named_Section(st, sec))
      return;
  }

  if (ST_is_not_used(st)) return;

  if (ST_has_named_section(st)) {
    // bug fix for OSP_138
    if (ST_has_Predefined_Named_Section(st, sec))
      // Assign st to appropriate section
      Allocate_Object_To_Predefined_Named_Section(st, sec);
    else {
      STR_IDX name = Find_Section_Name_For_ST (st);
      Assign_ST_To_Named_Section (st, name);
    }
    return;
  }
  if (Has_Base_Block(st)) {
	base_st = Base_Symbol(st);	/* allocate the base, not the leaf */
  }

  if (ST_sclass(st) != ST_sclass(base_st))
    DevWarn("st sclass %d different from base sclass %d", 
            ST_sclass(st), ST_sclass(base_st));

  switch ( ST_sclass(base_st) ) {
  case SCLASS_UNKNOWN :
    break;
  case SCLASS_AUTO:
    Process_Stack_Variable(base_st);
    break;
  case SCLASS_FORMAL:
  case SCLASS_FORMAL_REF:
    /*
     * should have already processed formals and formal_refs: 
     */
    break;
  case SCLASS_PSTATIC :
  case SCLASS_FSTATIC :
#ifdef TARG_ST

    sec = Assign_Static_Variable (st);
#else

    if (ST_is_thread_private(st)) {
      if (ST_is_initialized(st) && !ST_init_value_zero (st))
#ifdef KEY
        sec = _SEC_LDATA_MIPS_LOCAL;	// bug 12619
#else
        sec = _SEC_LDATA;
#endif
      else
#ifdef KEY
        sec = _SEC_BSS;
#else
        sec = _SEC_LBSS;
#endif // KEY
    }
#ifdef KEY
    else if (ST_is_thread_local(st)) {
      if (ST_is_initialized(st) && !ST_init_value_zero(st))
        sec = _SEC_LDATA;
      else
        sec = _SEC_LBSS;
    }
#endif
    else if (ST_is_initialized(st) && !ST_init_value_zero (st))
#ifdef KEY
    {
      if (ST_is_constant(st))
        // GNU puts CLASS_CONST data in .rodata.
        if (Gen_PIC_Shared && ST_sym_class(st) != CLASS_CONST)
          sec = _SEC_DATA_REL_RO; // bug 10097
        else
          sec = _SEC_RDATA;
      else
        sec = _SEC_DATA;
    }
#else
        sec = (ST_is_constant(st) ? _SEC_RDATA : _SEC_DATA);
#endif
    else
	sec = _SEC_BSS;
#endif
    sec = Shorten_Section ( st, sec );
    Allocate_Object_To_Section ( base_st, sec, Adjusted_Alignment (base_st) );
    break;
  case SCLASS_REG:
        break;
  case SCLASS_CPLINIT:
	sec = _SEC_CPLINIT;
	Allocate_Object_To_Section ( base_st, sec, Adjusted_Alignment(base_st));
	break;
  case SCLASS_EH_REGION:
	sec = _SEC_EH_REGION;
	Allocate_Object_To_Section ( base_st, sec, Adjusted_Alignment(base_st));
	break;
  case SCLASS_EH_REGION_SUPP:
	sec = _SEC_EH_REGION_SUPP;
	Allocate_Object_To_Section ( base_st, sec, Adjusted_Alignment(base_st));
	break;
  case SCLASS_DISTR_ARRAY:
#ifdef TARG_ST
    FmtAssert(FALSE,("Allocate_Object: SCLASS_DISTR_ARRAY"));
#else
	sec = _SEC_DISTR_ARRAY;
	Allocate_Object_To_Section ( base_st, sec, Adjusted_Alignment(base_st));
#endif
	break;
  case SCLASS_THREAD_PRIVATE_FUNCS:
#ifdef TARG_ST
    FmtAssert(FALSE,("Allocate_Object: SCLASS_THREAD_PRIVATE_FUNCS"));
#else
	sec = _SEC_THREAD_PRIVATE_FUNCS;
	Allocate_Object_To_Section ( base_st, sec, Adjusted_Alignment(base_st));
#endif
	break;
  case SCLASS_COMMON:
   /*  The compiler has promised the Adjusted_Alignment to earlier phases, so
    *  we must change the alignment somewhere (here is good)
    *
    *  This may break compatibility with other compilers (ie. another compiler
    *  initializes data at a non Adjusted_Alignment().
    */
    if (ST_type(st) != (TY_IDX) NULL)
    {
      TY_IDX type = ST_type(st);

      Set_ST_type(st, 
	Make_Align_Type(type, Adjusted_Alignment(st)) );
    } /* fall thru */
  case SCLASS_EXTERN:
    /* if possible, mark the symbol as gp-relative. No need to do anything
     * else.  But if IPA has been run, don't want to mark these symbols
     * as gprel, because have to trust IPA's choosing of gprel symbols.
     */
    if ( ! FILE_INFO_ipa (File_info) && !ST_is_weak_symbol(st)) {
	sec = Shorten_Section ( st, _SEC_DATA );
	if (sec != _SEC_DATA) Set_ST_gprel (st);
    }
    break;
#ifdef TARG_ST
  case SCLASS_UGLOBAL :
  case SCLASS_DGLOBAL :
    sec = Assign_Global_Variable(st, base_st);
    sec = Shorten_Section ( st, sec );
    Allocate_Object_To_Section ( base_st, sec, Adjusted_Alignment(base_st));
    break;
#else

  case SCLASS_UGLOBAL :
    if (ST_is_thread_private(st)) {
#ifdef KEY
      sec = _SEC_BSS;
#else
      sec = _SEC_LBSS;
#endif // KEY
    } 
#ifdef KEY
    else if (ST_is_thread_local(st)) {
      sec = _SEC_LBSS;
    }
#endif
    else sec = _SEC_BSS;
    sec = Shorten_Section ( st, sec );
    Allocate_Object_To_Section ( base_st, sec, Adjusted_Alignment(base_st));
    break;
  case SCLASS_DGLOBAL :
    if (ST_is_thread_private(st))
#ifdef KEY
      sec = _SEC_LDATA_MIPS_LOCAL;	// bug 12619
#else
      sec = _SEC_LDATA;
#endif
#ifdef KEY
    else if (ST_is_thread_local(st)) sec = _SEC_LDATA;
#endif
    else if (ST_is_constant(st)) {
#ifdef KEY
      if (Gen_PIC_Shared)
	sec = _SEC_DATA_REL_RO;  // bug 6925
      else
#endif
      sec = _SEC_RDATA;
    }
    else sec = _SEC_DATA;
    sec = Shorten_Section ( st, sec );
    Allocate_Object_To_Section ( base_st, sec, Adjusted_Alignment(base_st));
    break;
#endif  /* TARG_ST */
  case SCLASS_TEXT :
    /* this is a reference only, no storage need be allocated */
    /* Text can't be shortened */
    sec = _SEC_TEXT;
    Assign_Object_To_Section ( base_st, sec, 0 );
    break;
  default:
    Is_True ( (FALSE)
	     , ("Allocate_Object: Invalid SCLASS %d",ST_sclass(st)) );
    break;
  } /* switch */

}



/* ====================================================================
 *
 * Allocate_File_Statics
 *
 * It goes throught the global symbols and call Allocate_Object for
 * them. 
 *
 * ====================================================================
 */
extern void 
Allocate_File_Statics (void)
{
  ST *st;
  INT i;
  Trace_Frame = Get_Trace(TP_DATALAYOUT, 1);

  // first look in global st-attr table for section assignments
  if ( ST_ATTR_Table_Size (GLOBAL_SYMTAB)) {
    For_all (St_Attr_Table, GLOBAL_SYMTAB,
        Assign_Section_Names());
  }

  // gnu assembler complains about multiple commons with same name in a file.
  // So search for commons with same name and merge them together.
  slist<ST*> common_list;
  slist<ST*>::iterator cit;
  FOREACH_SYMBOL (GLOBAL_SYMTAB,st,i) {
	if (ST_sclass(st) != SCLASS_COMMON) continue;
	if (Has_Base_Block(st)) continue;	// only merge root commons
	// search for it in list; if found, merge, if not, add.
        for (cit = common_list.begin(); cit != common_list.end(); ++cit)
        {
		if (ST_name_idx(*cit) == ST_name_idx(st)
		    && ST_export(*cit) == ST_export(st)) 
		{
			// rather than create new symbol, instead
			// just base one symbol on larger one.
			// St_Block_Union(*cit, st);
			if (TY_size(ST_type(st)) > TY_size(ST_type(*cit))) {
				Set_ST_base(*cit, st);
DevWarn("found commons with same name %s; merge together %d->%d", ST_name(st), ST_index(*cit), ST_index(st));
				// replace symbol in list with the
				// larger symbol.
				*cit = st;
			}
			else {
				Set_ST_base(st, *cit);
DevWarn("found commons with same name %s; merge together %d->%d", ST_name(st), ST_index(st), ST_index(*cit));
			}
			break;
		}
	}
	if (cit == common_list.end()) {
		common_list.push_front(st);
	}
  }

  FOREACH_SYMBOL (GLOBAL_SYMTAB,st,i) {
      if (ST_sclass(st) == SCLASS_REG) continue;	// pregs

      // will allocate constants as seen, unless IPA
      if (ST_class(st) == CLASS_CONST && !Emit_Global_Data) continue;

      /* if reshaped, lno will take care of allocation.
       * if fill/align, then be will take care of allocation.
       */
      if (ST_class(st) == CLASS_VAR &&
          (ST_is_reshaped(st) || ST_is_fill_align(st))) 
        continue;
      Allocate_Object(st);
  }
}


/* This checks for "small" offsets (< 16 bits) from sp/gp/fp */
BOOL
Uses_Small_Offset (ST *st, WN_OFFSET wn_ofst)
{
	switch(ST_sclass(st)) {

	case SCLASS_AUTO:
		switch (Current_PU_Stack_Model) {
		case SMODEL_SMALL:
			return TRUE;
		case SMODEL_LARGE:
			/* assume that only large objects
			 * will end up far away.
			 * could be wrong if lots of small objects. */
			return (ST_size(st) < Large_Object_Bytes);
		case SMODEL_DYNAMIC:
			/* don't know order with alloca frames
			 * so worse case assumption: */
			return FALSE;
		}

  	case SCLASS_FORMAL:
		return (Current_PU_Stack_Model != SMODEL_DYNAMIC);
	default:
		switch (ST_class(st)) {
		case CLASS_VAR:
		case CLASS_BLOCK:
			return ST_gprel(st);
		case CLASS_CONST:
			/* assume that const will be in gp-relative section.
			 * actually may not be, but this is a safe guess. */
			return TRUE;
		}
		return FALSE;
	}
}

// Pad global arrays whose size is too close to a multiple of the
// cache size

static ST*
Create_Global_Array_ST (TYPE_ID emtype, INT num, char *name) 
{

    TY_IDX arr_ty = Make_Array_Type (emtype, 1, num);
    ST* st = New_ST ();
    ST_Init (st, Save_Str(name), CLASS_VAR, SCLASS_UGLOBAL, EXPORT_LOCAL, arr_ty);
    Set_ST_pt_to_unique_mem (st);
    return st;
} // Create_Global_Array_ST


// make sure size is greater than 10% more than 1600 or 256K

#ifndef TARG_ST
struct pad_global_arrays
{
    inline void operator() (UINT32, ST* st) const {
	if ((ST_sclass(st) == SCLASS_UGLOBAL) && (ST_class(st) == CLASS_VAR)) {
	  const TY_IDX ty = ST_type(st);

	  if (!ST_is_not_used(st) &&
	    (TY_size (ty) > 0) &&
	    !ST_is_fill_align(st) &&
	    !ST_has_nested_ref(st) &&
	    (TY_kind(ty) == KIND_ARRAY) &&
	    (!ST_is_reshaped(st))) {

	    INT size = TY_size (ty);
	    INT set_size = 256*1024;
	    INT mod = size % set_size;
	    INT pad_size=0;
	    if ((size > 0.9*set_size) && (mod < set_size/20)) {
		pad_size = set_size/20 - mod;
	    } else if ((size > 0.9*set_size) &&
		       ((set_size - mod) < set_size/20)) {
		pad_size = set_size/20 + (set_size - mod);
	    }
	    size += pad_size;
	    set_size = 16*1024;
	    mod = size % set_size;
	    if ((size > 0.9*set_size) && (mod < set_size/20)) {
		pad_size += set_size/20 - mod;
	    } else if ((size > 0.9*set_size) &&
		       ((set_size - mod) < set_size/20)) {
		pad_size += set_size/20 + (set_size - mod);
	    }
	    size += pad_size;
	    if (pad_size) {
		static INT count;
		char name[64];
		sprintf (name, "global_pad_%d", count);
		count++;
		ST *pad_symbol = Create_Global_Array_ST (MTYPE_I1,size,name); 
		St_Block_Union(st,pad_symbol);
	    }
	  }
	}
    }
}; // pad_global_arrays
#endif

extern void
Pad_Global_Arrays()
{
#ifdef TARG_ST
  //DevWarn("Pad_Global_Arrays(): not implemented on this target");
#else
    For_all (St_Table, GLOBAL_SYMTAB, pad_global_arrays ());
#endif
}


extern INT
Stack_Offset_Adjustment_For_PU (void)
{
  if (Frame_Has_Calls) 
	return Stack_Offset_Adjustment;
  else
	return 0;
}

#ifdef TARG_X8664
BOOL
Stack_Frame_Has_Calls (void)
{
  return Frame_Has_Calls;
}
#endif
#ifdef TARG_ST
/*
 * Allocate_Reg_Save_Area()
 *
 * Allocate space for the REGSAVE located area after FTEMP.
 * The effective allocation will only be done in Finalize_Stack_Frame.
 */
BE_EXPORTED extern void
Allocate_Reg_Save_Area(INT64 size)
{
  FmtAssert(ST_is_not_used(SF_Block(SFSEG_REGSAVE)), ("Reg Save area must be allocated only once"));
  Set_STB_size(SF_Block(SFSEG_REGSAVE), size);
  Clear_ST_is_not_used(SF_Block(SFSEG_REGSAVE));
  if (Trace_Frame)
    fprintf(TFile, "<lay> allocating regsave size = %ld\n", size);
}


/*
 * Get_Reg_Save_Area()
 *
 * Returns the block symbol corresponding to the REGSAVE area.
 * Should be used by the code generator to locate the REGSAVE area
 * into the frame once Finalize_Stack_Frame has been done.
 * Returns NULL if no REGSAVE area is allocated.
 */
BE_EXPORTED extern ST *
Get_Reg_Save_Area(void)
{
  if (ST_is_not_used(SF_Block(SFSEG_REGSAVE))) return NULL;
  return SF_Block(SFSEG_REGSAVE);
}

#endif

