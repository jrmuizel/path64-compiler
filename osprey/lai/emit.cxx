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

#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <bstring.h>
#include <elf.h>
#include <libelf.h>
#include <elfaccess.h>
#include <alloca.h>
#include <stdlib.h>
#include <cmplrs/rcodes.h>
#include <stamp.h>
#include <vector>

#define	USE_STANDARD_TYPES 1
#include "defs.h"
#include "tracing.h"
#include "hash_map.h"
#include "config.h"
#include "config_asm.h"
#include "config_TARG.h"
#include "config_debug.h"
#include "config_list.h"
#include "vstring.h"
#include "glob.h"
#include "targ_const.h"
#include "strtab.h"
#include "symtab.h"
#include "tag.h"
#include "wn.h"

#include "stblock.h"
#include "data_layout.h"
#include "sections.h"
#include "label_util.h"
#include "note.h"
#include "freq.h"

#include "erglob.h"
#include "erlib.h"
#include "em_elf.h"
#include "dwarf_DST_mem.h"         /* for DST_IDX */

#include "whirl2ops.h"		   /* for Get_WN_From_Memory_OP */
#include "lai.h"
#include "calls.h"                 /* for Frame_Len */
#include "cgexp.h"                /* for Exp_Simulated_OP */

#include "cg_targ.h"

#include "ti_errors.h"
#include "ti_asm.h"

#include "targ_abi_properties.h"   /* ABI_PROPERTY_RegName */

#define PAD_SIZE_LIMIT	2048	/* max size to be padded in a section */

/* c++ mangled names can be any arbitrary length,
 * so this is just a first guess */ 
#define LBUF_LEN	(OP_MAX_FIXED_OPNDS*1024)

/* Instructions go into one of two ELF text sections depending on
 * whether the BB is in a hot or cold region. Hot BBs go into
 * ".text" (or ".text<pu-name>" for -TENV:section_for_each_function).
 * Cold BBs go into ".text.cold". As a result, numerous cgemit
 * routines need to track the PC of both sections, and 2 element
 * arrays are typically used. Here we define the indices for
 * the arrays, with the values chosen to coincide with the result
 * of BB_Is_Cold.
 */
enum { IHOT=FALSE, ICOLD=TRUE };

/* Overload BB flag <local_flag1> to indicate if a BB is cold or hot --
 * it's less overhead than BB_Is_Cold.
 */
#define BB_cold		BB_local_flag1
#define Set_BB_cold	Set_BB_local_flag1
#define Reset_BB_cold	Reset_BB_local_flag1

extern const char __Release_ID[];

/* ====================================================================
 *    Global data
 * ====================================================================
 */

/* ====================================================================
 *    Local data
 * ====================================================================
 */

static BOOL Trace_Emit    = FALSE;

static BOOL generate_dwarf = FALSE;
static BOOL generate_elf_symbols = FALSE;

static BOOL Use_Prefetch = FALSE;

// For dwarf output in assembly we need to keep track of the 
// offset of the current instruction from a known label.
// If Last_Label is LABEL_IDX_ZERO, it is assumed to be invalid.
static LABEL_IDX Last_Label = LABEL_IDX_ZERO;
static INT32 Offset_From_Last_Label = 0;

static LABEL_IDX Initial_Pu_Label;

/* NOTE: PCs are actually a bundle address and slot number pair. The 
 * slot number is in the low bits. 
 */
static INT32 PC = 0;			/* PC for current text section */
static INT32 text_PC = 0;		/* PC for hot text */
static INT32 cold_PC = 0;		/* PC for cold text */

static ST *PU_base = NULL;		/* base for current text section */
static ST *text_base = NULL;		/* base for hot text section */
static ST *cold_base = NULL;		/* base for cold text section */

static pSCNINFO PU_section = NULL;	/* current text section */
static pSCNINFO text_section = NULL;	/* hot text section */
static pSCNINFO cold_section = NULL;	/* cold text section */

static INT current_rid = 0;	/* current rid id */

typedef struct {
  pSCNINFO scninfo;
  Elf64_Word scn_ofst;
  ST *sym;
} AUX_SCN;

static AUX_SCN *em_scn;
static INT last_scn = 0;	
static INT max_scn = 0;
/* 0 index to em_scn will be null values */

static ST *cur_section = NULL;

static PU_IDX current_pu = 0;

/* ====================================================================
 *    Use_Separate_PU_Section
 *
 *    put PU in separate elf text section? (pu could be null if no PUs)
 * ====================================================================
 */
#define Use_Separate_PU_Section(pu,st) \
	((pu != (PU_IDX) NULL) \
	&& (Section_For_Each_Function || PU_in_elf_section(pu)) \
	&& (ST_class(st) == CLASS_BLOCK) \
	&& (strcmp(ST_name(st), ELF_TEXT) == 0))


static char ism_name[40];

/* ====================================================================
 *    Get_Ism_Name ()
 *
 *    get ism name from __Release_ID 
 *    TODO: same code as in ../cg/cgemit.cxx -- factorize ?
 * ====================================================================
 */
static void
Get_Ism_Name (void)
{
  char *s = strchr(__Release_ID, ':');
  char *p;

  if (s == NULL) {
    ism_name[0] = '\0';
    return;
  }
  s++;	/* skip : */
  s = strchr(s, ':');
  if (s == NULL) {
    ism_name[0] = '\0';
    return;
  }
  s++;	/* skip : */
  p = strchr(s, ' ');
  strncpy (ism_name, s, p-s);
  ism_name[p-s] = '\0';

  return;
}

/* ====================================================================
 *    Routines from ../cg/cgemit.cxx -- factorize
 * ====================================================================
 */

/*
 * For the elf indexes, we can reuse the temp field for local STs,
 * Need to map from ST's to the elf index for the ST.
 * So create array of index arrays, where the ST_level
 * gets you to the index array, and then the ST_index 
 * gets you to the elf index for that ST.
 */

/*
 * Maximum nesting depth includes these scopes, in the deepest case:
 *  0 : not used
 *  1 : global scope
 *  2 : top-level PU
 *  3 : F90 internal procedure
 *  4 : MP PU from internal procedure
 *  5 : Fortran I/O in MP PU
 */

#define MAX_SYMTAB_DEPTH	6

static INT32 *elf_index_array[MAX_SYMTAB_DEPTH] =
  {NULL,NULL,NULL,NULL,NULL,NULL};
static INT max_elf_index[MAX_SYMTAB_DEPTH] = {0,0,0,0,0,0};

/* ====================================================================
 *   Allocate_Elf_Index_Space
 * ====================================================================
 */
static void
Allocate_Elf_Index_Space (
  UINT level
)
{
  INT num_sts = ST_Table_Size(level);

  if (elf_index_array[level] == NULL) {
    max_elf_index[level] = num_sts + 100;
    /* for the size take the #st's + extra space for new symbols */
    elf_index_array[level] = 
          (INT32*) Src_Alloc(sizeof(INT32) * max_elf_index[level]);
  } else if (max_elf_index[level] < num_sts + 10) {  
    /* # globals has grown, so realloc. */
    /* we realloc even if close and not yet overflowed,
     * because may alloc new symbols as part of cg. */
    elf_index_array[level] = TYPE_MEM_POOL_REALLOC_N (INT32, 
		    &MEM_src_pool, elf_index_array[level], 
			      max_elf_index[level], num_sts + 100);
    max_elf_index[level] = num_sts + 100;
  }
}

/* ====================================================================
 *   Init_ST_elf_index
 * ====================================================================
 */
static void
Init_ST_elf_index (
  UINT stab
)
{
  UINT level = stab;
  INT i;

  Allocate_Elf_Index_Space(GLOBAL_SYMTAB);
  Is_True((level < MAX_SYMTAB_DEPTH), ("Init_ST_elf_index overflow"));
  if (level > GLOBAL_SYMTAB && elf_index_array[level] != NULL) {
    /* need to clear the values in case leftover 
     * from previous uplevel */
    for (i = 0; i < max_elf_index[level]; i++) {
      elf_index_array[level][i] = 0;
    }
  }
  Allocate_Elf_Index_Space(stab);
}

/* ====================================================================
 *   ST_elf_index
 * ====================================================================
 */
static INT32
ST_elf_index (
  ST *st
)
{
  INT level = ST_level(st);

  Is_True((level < MAX_SYMTAB_DEPTH), ("ST_elf_index overflow"));
  Is_True((ST_index(st) < max_elf_index[level]), 
                                      ("ST_elf_index overflow"));
  return elf_index_array[level][ST_index(st)];
}

/* ====================================================================
 *   Set_ST_elf_index
 * ====================================================================
 */
static void
Set_ST_elf_index (
  ST *st, 
  INT32 v
)
{
  INT level = ST_level(st);

  Is_True((level < MAX_SYMTAB_DEPTH), ("Set_ST_elf_index overflow"));
  Is_True((ST_index(st) < max_elf_index[level]),
		                      ("Set_ST_elf_index overflow"));
  elf_index_array[level][ST_index(st)] = v;
}

/* ====================================================================
 *   ST_is_gp_relative
 *
 *   Determine if the symbol has a base of gp 
 * ====================================================================
 */
static BOOL
ST_is_gp_relative (
  ST *st
)
{
  ST *base_st = Base_Symbol (st);

  return ((ST_class(base_st) == CLASS_BLOCK || 
	   ST_class(base_st) == CLASS_VAR) && 
	  ST_gprel(base_st));
}

/***********************************************************************
 *
 * Instruction-PC utilities.
 *
 * On architectures that support bundling, we use a PC representation
 * that enables us to point at instructions within the bundle.
 * We accomplish this by using the low bits of the bundle address
 * (which are normally zero because of alignment constraints) to
 * hold the instruction offset, i.e. slot number, within the bundle.
 *
 * The following utility routines provide for updating and accessing
 * the composite PC values.
 *
 ***********************************************************************
 */

#define ISA_MAX_SLOTS 1

/* Given a composite PC, return the bundle address component.
 */
inline INT32 PC_Bundle(INT32 pc)
{
  return pc & ~(INST_BYTES - 1);
}

/* Given a composite PC, return the slot number component.
 */
inline INT32 PC_Slot(INT32 pc)
{
  return pc & (INST_BYTES - 1);
}

/* ====================================================================
 *   PC_Incr
 *
 *   Increment a composite PC by 1. The slot number is incremented, 
 *   and if it overflows, the bundle address is adjusted as well.
 * ====================================================================
 */
inline INT32 
PC_Incr (
  INT32 pc
)
{
  ++pc;

  if (PC_Slot(pc) == ISA_MAX_SLOTS) {
    pc += INST_BYTES - ISA_MAX_SLOTS;
  }

  return pc;
}

/* ====================================================================
 *   PC_Incr_N
 *
 *   Same as PC_Incr except the increment is an arbitrary constant.
 * ====================================================================
 */
inline INT32 
PC_Incr_N (
  INT32 pc, 
  UINT32 incr
)
{
  UINT slots = PC_Slot(pc) + incr;
  UINT bundles = slots / ISA_MAX_SLOTS;
  pc = PC_Bundle(pc) + (bundles * INST_BYTES) + (slots % ISA_MAX_SLOTS);
  return pc;
}

/* ====================================================================
 *    Write_Weak_Alias (sym, stringsym)
 * ====================================================================
 */
static void
Write_Weak_Alias (
  ST *sym, 
  ST *strongsym
) 
{
  fprintf (Lai_File, "\t%s\t%s#\n", AS_WEAK, ST_name(sym));
  fprintf (Lai_File, "\t.set %s#, %s#\n", ST_name(sym), ST_name(strongsym));
}

/* ====================================================================
 *    Write_Alias (sym, stringsym)
 * ====================================================================
 */
static void
Write_Alias (ST *sym, ST *strongsym) 
{
  fprintf (Lai_File, "\t.set %s#, %s#\n", ST_name(sym), ST_name(strongsym));
}

/* ====================================================================
 *    r_qualified_name
 * ====================================================================
 */
static void
r_qualified_name (
  ST *st,
  vstring *buf       /* buffer to format it into */
)
{
  if (ST_name(st) && *(ST_name(st)) != '\0') {
    *buf = vstr_concat(*buf, ST_name(st));
    if (ST_is_export_local(st) && ST_class(st) == CLASS_VAR) {
      // local var, but being written out.
      // so add suffix to help .s file distinguish names.
      // assume that statics in mult. pu's will 
      // get moved to global symtab, so don't need pu-num
      if (ST_level(st) == GLOBAL_SYMTAB)
	vstr_sprintf (buf, vstr_len(*buf), 
                    "%s%d", Label_Name_Separator, ST_index(st));
      else
	vstr_sprintf (buf, vstr_len(*buf), 
           "%s%d%s%d", Label_Name_Separator, ST_pu(Get_Current_PU_ST()),
		           Label_Name_Separator, ST_index(st) );
    }
    else if (*Symbol_Name_Suffix != '\0') {
      *buf = vstr_concat(*buf, Symbol_Name_Suffix);
    }
  } else {
    vstr_sprintf (buf, vstr_len(*buf), 
                 "%s %+lld", ST_name(ST_base(st)), ST_ofst(st));
  }
}

/* ====================================================================
 *    Write_Qualified_Name (FILE *f, ST *st)
 * ====================================================================
 */
static void
Write_Qualified_Name (
  FILE *f, 
  ST *st
)
{
  vstring buf = vstr_begin(LBUF_LEN);
  r_qualified_name (st, &buf);
  fprintf (Lai_File, "%s", vstr_str(buf));
  vstr_end(buf);

  /*
  if (ST_name(st) && *(ST_name(st)) != '\0') {
    fprintf (f, "%s", ST_name(st));
    if ( ST_is_export_local(st) && ST_class(st) == CLASS_VAR) {
  */
      // local var, but being written out.
      // so add suffix to help .s file distinguish names.
      // assume that statics in mult. pu's will 
      // get moved to global symtab, so don't need pu-num
  /*
      if (ST_level(st) == GLOBAL_SYMTAB)
	fprintf (f, "%s%d", Label_Name_Separator, ST_index(st));
      else
	fprintf (f, "%s%d%s%d", Label_Name_Separator, 
		         ST_pu(Get_Current_PU_ST()),
		           Label_Name_Separator, ST_index(st) );
    }
    else if (*Symbol_Name_Suffix != '\0') {
      fprintf (f, "%s", Symbol_Name_Suffix);
    }
  } else {
    fprintf (f, "%s %+lld", ST_name(ST_base(st)), ST_ofst(st));
  }
  */
}

/* ====================================================================
 *    Print_Dynsym (pfile, st)
 *
 *    print the internal, hidden or protected attributes if present 
 *    TODO: same code as in ../cg/cgemit.cxx -- factorixe
 * ====================================================================
 */
static void 
Print_Dynsym (
  FILE *pfile, 
  ST *st
)
{
  if (AS_DYNSYM) {
    fprintf (pfile, "\t%s\t", AS_DYNSYM);
    Write_Qualified_Name (pfile, st);
    switch (ST_export(st)) {
      case EXPORT_INTERNAL:
	fprintf (pfile, "\tsto_internal\n");
	break;
      case EXPORT_HIDDEN:
	fprintf (pfile, "\tsto_hidden\n");
	break;
      case EXPORT_PROTECTED:
	fprintf (pfile, "\tsto_protected\n");
	break;
      case EXPORT_OPTIONAL:
	fprintf (pfile, "\tsto_optional\n");
	break;
      default:
	fprintf (pfile, "\tsto_default\n");
	break;
    }
  }
}

/* ====================================================================
 *    Print_Label (pfile, st, size)
 *
 *    TODO: same code as in ../cg/cgemit.cxx -- factorixe
 * ====================================================================
 */
static void 
Print_Label (
  FILE *pfile, 
  ST *st, 
  INT size
)
{
  ST *base_st;
  INT64 base_ofst;

  if (ST_is_weak_symbol(st)) {
    fprintf (pfile, "\t%s\t", AS_WEAK);
    Write_Qualified_Name(pfile, st);
    fprintf(pfile, "\n");
  }
  else if (!ST_is_export_local(st)) {
    fprintf (pfile, "\t%s\t", AS_GLOBAL);
    Write_Qualified_Name(pfile, st);
    fprintf(pfile, "\n");
  }
  if (ST_class(st) == CLASS_VAR) {
    fprintf (pfile, "\t%s\t", AS_TYPE);
    Write_Qualified_Name (pfile, st);
    fprintf (pfile, ", %s\n", AS_TYPE_OBJECT);
  }
  if (size != 0) {
    /* if size is given, then emit value for asm */
    fprintf (pfile, "\t%s\t", AS_SIZE);
    Write_Qualified_Name(pfile, st);
    fprintf (pfile, ", %d\n", size);
  }
  Base_Symbol_And_Offset (st, &base_st, &base_ofst);
  Write_Qualified_Name (pfile, st);
  fprintf (pfile, ":\t%s 0x%llx\n", ASM_CMNT, base_ofst);
  Print_Dynsym (pfile, st);
}

/* ====================================================================
 *    Print_Common
 *
 *    TODO: same code as in ../cg/cgemit.cxx -- factorize
 * ====================================================================
 */
static void
Print_Common (
  FILE *pfile, 
  ST *st
)
{
  ST    *base_st;
  INT64  base_ofst;

  Base_Symbol_And_Offset (st, &base_st, &base_ofst);
  if (st != base_st && ST_sclass(base_st) == SCLASS_COMMON) {
    // use base common
    if (ST_elf_index(base_st) == 0) {
      Print_Common (pfile, base_st);
    }
    return;
  }
  
  if (TY_size(ST_type(st)) > 0) {

    if (ST_is_weak_symbol(st)) {
      fprintf ( pfile, "\t%s\t", AS_WEAK);
      Write_Qualified_Name(pfile, st);
      fprintf ( pfile, "\n");
    }
    fprintf ( pfile, "\t%s\t", AS_COM);
    Write_Qualified_Name(pfile, st);
    fprintf (pfile, ", %lld, %d\n", 
		     TY_size(ST_type(st)), TY_align(ST_type(st)));
    Print_Dynsym (pfile, st);

    // this is needed so that we don't emit commons more than once
    Set_ST_elf_index(st, 1);
  }
}

/* ====================================================================
 *    LAI_Put_Elf_Symbol (sym)
 * 
 *    do dummy mark that we have seen this symbol and emitted 
 *    any type info for it.
 * ====================================================================
 */
static void
LAI_Put_Elf_Symbol (
  ST *sym
)
{
  if (ST_class(sym) == CLASS_FUNC) {
    fprintf (Lai_File, "\t%s\t", AS_TYPE);
    Write_Qualified_Name (Lai_File, sym);
    fprintf (Lai_File, ", %s\n", AS_TYPE_FUNC);
  }
  else if (ST_class(sym) == CLASS_VAR && ST_sclass(sym) == SCLASS_COMMON) {
    Print_Common (Lai_File, sym);
  }

  Set_ST_elf_index(sym, 1);
  return;
}

/* ====================================================================
 *    Write_TCON
 *
 *    Emit a TCON value to the assembly/object file.
 * ====================================================================
 */
static Elf64_Word
Write_TCON (
  TCON	*tcon,		/* Constant to emit */
  INT scn_idx,		/* Section to emit it into */
  Elf64_Xword scn_ofst,	/* Section offset to emit it at */
  INT32	repeat		/* Repeat count */
)
{
  BOOL add_null = TCON_add_null(*tcon);
  pSCNINFO scn = em_scn[scn_idx].scninfo;

  INT32 scn_ofst32 = (INT32)scn_ofst;
  FmtAssert(scn_ofst32 == scn_ofst, ("section offset exceeds 32 bits: 0x%llx",
				       (INT64)scn_ofst));

  Targ_Emit_Const (Lai_File, *tcon, add_null, repeat, scn_ofst32);

  if ( TCON_ty(*tcon) == MTYPE_STRING )
    scn_ofst += (Targ_String_Length (*tcon) + (add_null ? 1 : 0)) * repeat;
  else
    scn_ofst += TY_size(Be_Type_Tbl(TCON_ty(*tcon))) * repeat;

  return scn_ofst;
}

/* ====================================================================
 *    Write_Symbol
 *
 *    Emit a symbol value to the assembly/object file.
 * ====================================================================
 */
static Elf64_Word
Write_Symbol (
  ST * sym,		/* Emit the address of this symbol */
  Elf64_Sxword sym_ofst,/*   ... plus this offset */
  INT scn_idx,		/* Section to emit it in */
  Elf64_Word scn_ofst,	/* Section offset to emit it at */
  INT32	repeat		/* Repeat count */
)
{
  INT32 i;
  ST *basesym;
  INT64 base_ofst = 0;
  pSCNINFO scn = em_scn[scn_idx].scninfo;
  INT address_size = ((Use_32_Bit_Pointers) ? 4 : 8);

  /* make sure is allocated */
  if (!Has_Base_Block(sym)) {
    Allocate_Object(sym);
    /* if did allocate on local stack, that messes up frame length */
    Is_True(!Has_Base_Block(sym) || Is_Global_Symbol(Base_Symbol(sym)),
		("Write_Symbol:  too late to allocate object on stack"));
  }

  /* If the symbol is a local label that has not been assigned an 
   * address, it is a label in a basic block that has been deleted. 
   * Emit zeros for this case instead of the address.
   */
  if (ST_sclass(sym) == SCLASS_TEXT && !Has_Base_Block(sym)) {
    INT32 padding;
    padding = repeat * address_size;
    if (padding > 0) {
      ASM_DIR_ZERO(Lai_File, padding);
    }
    scn_ofst += padding;
    return scn_ofst;
  }

  /* 
   * For local static symbols that do not have their own elf entry,
   * use the base symbol; funcs always have own elf entry. 
   */
  basesym = sym;
  if (Has_Base_Block(sym) && 
      ST_is_export_local(sym) && 
      ST_class(sym) != CLASS_FUNC) {
    Base_Symbol_And_Offset (sym, &basesym, &base_ofst);
  }

  base_ofst += sym_ofst;

  for ( i = 0; i < repeat; i++ ) {
    // do object code first so base section initialized

    fprintf (Lai_File, "\t%s\t", 
		(scn_ofst % address_size) == 0 ? 
		AS_ADDRESS : AS_ADDRESS_UNALIGNED);
    if (ST_class(sym) == CLASS_CONST) {
      Write_Qualified_Name (Lai_File, basesym);
      fprintf (Lai_File, " %+lld\n", base_ofst);
    }
    else if (ST_class(sym) == CLASS_FUNC && AS_FPTR) {
      fprintf (Lai_File, " %s(", AS_FPTR);
      Write_Qualified_Name (Lai_File, sym);
      fprintf (Lai_File, " %+lld)\n", sym_ofst);
    }
    else {
      Write_Qualified_Name (Lai_File, sym);
      fprintf (Lai_File, " %+lld\n", sym_ofst);
    }
    if (ST_class(sym) == CLASS_FUNC) {
      fprintf (Lai_File, "\t%s\t", AS_TYPE);
      Write_Qualified_Name (Lai_File, sym);
      fprintf (Lai_File, ", %s\n", AS_TYPE_FUNC);
    }

    scn_ofst += address_size;
  }
  return scn_ofst;
}

/* ====================================================================
 *   Write_Label
 *
 *   Emit a label value to the assembly/object file.
 * ====================================================================
 */

static Elf64_Word
Write_Label (
  LABEL_IDX lab,	/* Emit the address of this label */
  Elf64_Sxword lab_ofst,/*   ... plus this offset */
  INT scn_idx,		/* Section to emit it in */
  Elf64_Word scn_ofst,	/* Section offset to emit it at */
  INT32	repeat		/* Repeat count */
)
{
  INT32 i;
  INT address_size = ((Use_32_Bit_Pointers) ? 4 : 8);

  for (i = 0; i < repeat; i++) {
    fprintf (Lai_File, "\t%s\t", (scn_ofst % address_size) == 0 ? 
		                  AS_ADDRESS : AS_ADDRESS_UNALIGNED);
    fprintf (Lai_File, "%s", LABEL_name(lab));
    if (lab_ofst != 0)
      fprintf (Lai_File, " %+lld", lab_ofst);
    fprintf (Lai_File, "\n");
    scn_ofst += address_size;
  }
  return scn_ofst;
}

/* ====================================================================
 *    Write_Symdiff (lab1, sym2idx, scn_idx, scn_ofst, repeat, size)
 * ====================================================================
 */
static Elf64_Word
Write_Symdiff (
  LABEL_IDX lab1,	/* left symbol */
  ST_IDX sym2idx,	/* right symbol */
  INT scn_idx,		/* Section to emit it in */
  Elf64_Word scn_ofst,	/* Section offset to emit it at */
  INT32	repeat,		/* Repeat count */
  INT size		/* 2 or 4 bytes */
)
{
  INT32 i;
  ST *sym2 = &St_Table[sym2idx];

  /* symbols must have an address */
  Is_True (lab1, ("lai_emit: Symdiff lab1 is null"));
  Is_True (sym2, ("lai_emit: Symdiff sym2 is null"));
  Is_True (Has_Base_Block(sym2), ("lai_emit: Symdiff sym2 not allocated"));

  for ( i = 0; i < repeat; i++ ) {
    fprintf (Lai_File, "\t%s\t", (size == 2 ? AS_HALF : AS_WORD));
    fprintf(Lai_File, "%s", LABEL_name(lab1));
    fprintf (Lai_File, "-");
    Write_Qualified_Name (Lai_File, sym2);
    fprintf (Lai_File, "\n");
    scn_ofst += size;
  }
  return scn_ofst;
}

/* ====================================================================
 *    Write_INITV
 *
 *    Emit an initial value record to the assembly/object file.
 * ====================================================================
 */
static Elf64_Word
Write_INITV (
  INITV_IDX invidx, 
  INT scn_idx, 
  Elf64_Word scn_ofst
)
{
  INT32 i;
  INITV_IDX ninv;
  INITV inv = Initv_Table[invidx];
  LABEL_IDX lab;
  TCON tcon;
  ST *st;
  pSCNINFO scn = em_scn[scn_idx].scninfo;

  switch ( INITV_kind(inv) ) {
    case INITVKIND_ZERO:
      tcon = Host_To_Targ (INITV_mtype (inv), 0);
      scn_ofst = Write_TCON (&tcon, scn_idx, scn_ofst, INITV_repeat2 (inv));
      break;

    case INITVKIND_ONE:
      tcon = Host_To_Targ (INITV_mtype (inv), 1);
      scn_ofst = Write_TCON (&tcon, scn_idx, scn_ofst, INITV_repeat2 (inv));
      break;
    case INITVKIND_VAL:
      scn_ofst = Write_TCON (&INITV_tc_val(inv), scn_idx, scn_ofst, 
			      INITV_repeat2(inv));
      break;

    case INITVKIND_SYMOFF:
      st = &St_Table[INITV_st(inv)];
      switch (ST_sclass(st)) {
	case SCLASS_AUTO:
	{ /* EH stack variable */
	  tcon = Host_To_Targ(MTYPE_I4, 0);
	  scn_ofst = Write_TCON (&tcon, scn_idx, scn_ofst, INITV_repeat1(inv));
	  break;
	}

	case SCLASS_FORMAL:
	{ /* EH this-pointer */
	  ST * base = ST_base(st);
	  INT  ofst = ST_ofst(st);
	  tcon = Host_To_Targ(MTYPE_I4, ofst);
	  scn_ofst = Write_TCON (&tcon, scn_idx, scn_ofst, INITV_repeat1(inv));
	  break;
	}
      
	default:
          scn_ofst = Write_Symbol ( st, INITV_ofst(inv),
	  			      scn_idx, scn_ofst, INITV_repeat1(inv));
	   break;
      }
      break;

    case INITVKIND_LABEL:
	lab = INITV_lab(inv);
	scn_ofst = Write_Label (lab, 0, scn_idx, scn_ofst, INITV_repeat1(inv));
	break;
    case INITVKIND_SYMDIFF:
      scn_ofst = Write_Symdiff ( INITV_lab1(inv), INITV_st2(inv),
				scn_idx, scn_ofst, INITV_repeat1(inv), 4);
      break;
    case INITVKIND_SYMDIFF16:
      scn_ofst = Write_Symdiff ( INITV_lab1(inv), INITV_st2(inv),
				scn_idx, scn_ofst, INITV_repeat1(inv), 2);
      break;

    case INITVKIND_BLOCK:
      for (i = 0; i < INITV_repeat1(inv); i++) {
	for (ninv = INITV_blk(inv); ninv; ninv = INITV_next(ninv)) {
          scn_ofst = Write_INITV (ninv, scn_idx, scn_ofst);
	}
      }
      break;

    case INITVKIND_PAD:
      if (INITV_pad(inv)*INITV_repeat1(inv) > 0) {
        ASM_DIR_ZERO(Lai_File, INITV_pad(inv) * INITV_repeat1(inv));
      }
      scn_ofst += INITV_pad(inv) * INITV_repeat1(inv);
      break;

    default:
      break;
  }
  return scn_ofst;
}

/* ====================================================================
 *    Write_INITO (inop, scn_idx, scn_ofst)
 *
 *    Emit the initialized object to the object file 
 * ====================================================================
 */
static void
Write_INITO (
  INITO* inop,		/* Constant to emit */
  INT scn_idx,		/* Section to emit it into */
  Elf64_Xword scn_ofst)	/* Section offset to emit it at */
{
  pSCNINFO scn = em_scn[scn_idx].scninfo;
  Elf64_Xword inito_ofst;
  ST *sym;
  ST *base;
  INITO ino = *inop;

  Base_Symbol_And_Offset(INITO_st(ino), &base, (INT64*)&inito_ofst);

  if (inito_ofst > scn_ofst) {
    fprintf(Lai_File, "\t%s %d\n", AS_SPACE, inito_ofst - scn_ofst);
      /*    ASM_DIR_ZERO(Lai_File, inito_ofst - scn_ofst); */
    scn_ofst = inito_ofst;
  } else {
    FmtAssert ( inito_ofst >= scn_ofst, 
      ("Write_INITO: DATA overlap 1, inito ofst @ %lld, scn ofst @ %lld",
	  inito_ofst, scn_ofst));
  }

  sym = INITO_st(ino);

  char *name = ST_name(sym);
  if (name != NULL && *name != 0) {
    Print_Label (Lai_File, sym, TY_size(ST_type(sym)));
  }

  /* If there's no initial value, this should be a constant symbol,
   * and the represented constant is the initial value:
   */
  if ( INITO_val(ino) == (INITO_IDX) NULL ) {
    if ( ST_class(sym) == CLASS_CONST ) {
      scn_ofst = Write_TCON (&ST_tcon_val(sym), scn_idx, scn_ofst, 1);
    }
  } else {
    INITV_IDX inv;
    FOREACH_INITV (INITO_val(ino), inv) {
      scn_ofst = Write_INITV (inv, scn_idx, scn_ofst);
    }
  }

  fprintf (Lai_File, "\t%s end of initialization for %s\n", ASM_CMNT, ST_name(sym));

  return;
}

/* ====================================================================
 *    Change_Section_Origin (base, ofst)
 *
 *    change to a new section and new origin 
 * ====================================================================
 */
static void
Change_Section_Origin (
  ST *base, 
  INT64 ofst
)
{
  if (base != cur_section) {
    /* switch to new section. */
    fprintf (Lai_File, "\n\t%s %s\n", AS_SECTION, ST_name(base));
  }

  // Do not generate .org for now ...
  //  fprintf (Lai_File, "\t%s 0x%llx\n", AS_ORIGIN, ofst);
  fprintf (Lai_File, "\n");


  /* generate a '.align 0' to make sure we don't autoalign */
  // I can't for now since GHS assembler does not recognize
  // .align 0
  // fprintf (Lai_File, "\t%s\t0\n", AS_ALIGN);
  cur_section = base;

  return;
}

/* ====================================================================
 *    section_lt (s1, s2)
 * ====================================================================
 */
inline bool section_lt (ST *s1, ST* s2) 
{ 
  // order doesn't really matter, just that grouped by base section
  return Base_Symbol(s1) < Base_Symbol(s2); 
}

/* ====================================================================
 *    offset_lt (s1, s2)
 * ====================================================================
 */
inline bool offset_lt (ST *s1, ST* s2) 
{ 
  return Offset_From_Base_Symbol(s1) < Offset_From_Base_Symbol(s2); 
}

/* ====================================================================
 *    size_lt (s1, s2)
 * ====================================================================
 */
inline bool size_lt (ST *s1, ST* s2) 
{ 
  // This is only needed so that we get silly 0-sized structs
  // at the correct offset.
  return TY_size(ST_type(s1)) < TY_size(ST_type(s2)); 
}

/* ====================================================================
 *    LAI_print_section
 * ====================================================================
 */
static void
LAI_print_section (
  ST *st, 
  Elf64_Word scn_type, 
  Elf64_Word scn_flags,
  Elf64_Xword scn_entsize, 
  ST *cur_section
)
{
  const char *scn_name = ST_name(st);
  Elf64_Word  scn_align = STB_align(st);
  const char *cur_scn_name = (cur_section != NULL) ? 
                                         ST_name(cur_section) : NULL;
  char scn_flags_string[5];
  char scn_type_string[10];  // min of strlen("progbits") + 1
  char *p = &scn_flags_string[0];

  fprintf (Lai_File, "\n\t%s \"%s\",", AS_SECTION, scn_name);

  if (scn_flags & SHF_WRITE) *p++ = 'w';
  if (scn_flags & SHF_ALLOC) *p++ = 'a';
  if (scn_flags & SHF_EXECINSTR) *p++ = 'x';

  *p = '\0'; // null terminate the string.
  fprintf (Lai_File, " \"%s\"", scn_flags_string);

  /*
  fprintf (Lai_File, " \"%s\",", scn_flags_string);

  p = &scn_type_string[0];
  if (scn_type == SHT_NOBITS) {
    strcpy(p, "nobits");
  }
  else if (scn_type == SHT_PROGBITS) {
    strcpy(p, "progbits");
  }
  else {
    DevWarn("Intel assembler definition inadequate for "
	    "ELF section type 0x%lx; using \"progbits\"", scn_type);
    strcpy(p, "progbits");
  }
  fprintf (Lai_File, " \"%s\"\n", scn_type_string);
  */

  fprintf (Lai_File, "\n");
  fprintf (Lai_File, "\t%s\t%d\n", AS_ALIGN, scn_align);  

  return;
}

/* ====================================================================
 *    LAI_init_section (st)
 *
 *    Initialize a section.
 * ====================================================================
 */
#define Is_Text_Section(st) (STB_exec(st) && strncmp(ST_name(st), ELF_TEXT,5)==0)

static void
LAI_init_section (
  ST *st
)
{
  Elf64_Word scn_type;
  Elf64_Word scn_flags;
  Elf64_Xword scn_entsize;
  char sname[32];

  if (ST_elf_index(st) != 0) {
    /* already created */
    return;
  }

  if (last_scn >= (max_scn-1)) {
    /* allocate new block of sections */
    max_scn += 30;
    if (em_scn == NULL)
      em_scn = (AUX_SCN *)Src_Alloc(sizeof(AUX_SCN)*max_scn);
    else
      em_scn = TYPE_MEM_POOL_REALLOC_N (AUX_SCN, 
		         &MEM_src_pool, em_scn, (max_scn-30), max_scn);
  }
  last_scn++;
  Set_STB_scninfo_idx(st, last_scn);

  /* hack for .text section */
  if (Is_Text_Section(st)) {
    if (Align_Instructions) 
      Set_STB_align(st, Align_Instructions);
    else if (OPT_Space)
      Set_STB_align(st, INST_BYTES);
    else
      Set_STB_align(st, TARG_Text_Alignment());
  }

  /* save symbol for later reference */
  em_scn[last_scn].sym = st;

  // Make a label:
  /*
  sprintf(sname, "L_SECTION_%d", last_scn);
  em_scn[last_scn].label = strdup(sname);
  */

  /* assume st is CLASS_BLOCK */
  scn_type = Get_Section_Elf_Type(STB_section_idx(st));
  scn_flags = Get_Section_Elf_Flags(STB_section_idx(st));
  if (Is_Text_Section(st) && 
      current_pu != (PU_IDX) NULL && PU_in_elf_section(current_pu)) {
    scn_flags |= SHF_MIPS_NODUPE;
  }
	
  scn_entsize = Get_Section_Elf_Entsize(STB_section_idx(st));

  /* set dummy value just so don't redo this */
  Set_ST_elf_index(st, 1);

  LAI_print_section(st, scn_type, scn_flags, scn_entsize, cur_section);

  return;
}

/* ====================================================================
 *    Process_Initos_And_Literals (stab)
 *
 *    This routine can be called multiple times for the global symtab;
 *    we do this so that objects are emitted in order.
 *    For each section, some objects are in local symtab, and some in global.
 *    We keep track of the last global symbol that has been processed
 *    so that we only process new symbols.
 *
 * ====================================================================
 */
static void
Process_Initos_And_Literals (
  SYMTAB_IDX stab
)
{
  static vector<bool> st_processed;
  if (st_processed.size() != ST_Table_Size(GLOBAL_SYMTAB)) {
    st_processed.resize(ST_Table_Size(GLOBAL_SYMTAB), false);
  }

  vector<ST*> st_list;
  vector<ST*>::iterator st_iter;

  typedef 
  hash_map < ST_IDX, INITO*, hash<ST_IDX>, equal_to<ST_IDX> > ST_INITO_MAP;
  ST_INITO_MAP st_inito_map;

  UINT i;
  static UINT last_inito = 1;
  
  // First walk the INITOs from the global table
  for (i = last_inito; i < INITO_Table_Size(GLOBAL_SYMTAB); ++i) {
    INITO* ino = &Inito_Table(GLOBAL_SYMTAB,i);
    ST* st = INITO_st(ino);
    /* don't emit initialization if st not used or extern */
    if (ST_is_not_used(st) ||
        ST_sclass(st) == SCLASS_EXTERN ||
        ST_sclass(st) == SCLASS_DISTR_ARRAY) {
      continue;
    }
    st_list.push_back(st);
    st_inito_map[ST_st_idx(st)] = ino;
  }

  last_inito = INITO_Table_Size(GLOBAL_SYMTAB);

  // Then walk the INITOs from the local table
  if (stab != GLOBAL_SYMTAB) {
    for (i = 1; i < INITO_Table_Size(stab); ++i) {
      INITO* ino = &Inito_Table(stab,i);
      ST* st = INITO_st(ino);
      /* don't emit initialization if st not used or extern */
      if (ST_is_not_used(st) ||
          ST_sclass(st) == SCLASS_EXTERN) {
        continue;
      }
      st_list.push_back(st);
      st_inito_map[ST_st_idx(st)] = ino;
    }
  }

  // Then walk the CONSTANTs from the global table
  for (i = 1; i < ST_Table_Size(GLOBAL_SYMTAB); ++i) {
    ST* st = &St_Table(GLOBAL_SYMTAB,i);
    if (ST_class(st) == CLASS_CONST && !st_processed[ST_index(st)]) {
      INT64 ofst;
      ST* base;
      Base_Symbol_And_Offset(st, &base, &ofst);
      if (ST_class(base) != CLASS_BLOCK || !STB_section(base)) {
        continue; // not allocated
      }
      if (Emit_Global_Data && SEC_is_merge(STB_section_idx(base))) {
        continue; // allocate in each .o
      }
      st_list.push_back(st);
    }
  }

  // Print_ST_List(st_list, "UNSORTED");
  stable_sort (st_list.begin(), st_list.end(), size_lt);
  // Print_ST_List(st_list, "SORTED BY SIZE");
  stable_sort (st_list.begin(), st_list.end(), offset_lt);
  // Print_ST_List(st_list, "SORTED BY OFFSET");
  stable_sort (st_list.begin(), st_list.end(), section_lt);
  // Print_ST_List(st_list, "SORTED BY SECTION");

  for (st_iter = st_list.begin(); st_iter != st_list.end(); ++st_iter) {

    INT64 ofst;
    ST* base;
    ST* st = *st_iter;
    ST_INITO_MAP::iterator st_inito_entry = st_inito_map.find(ST_st_idx(st));

    if (st_inito_entry != st_inito_map.end()) {
      INITO* ino = (*st_inito_entry).second;
      Base_Symbol_And_Offset(st, &base, &ofst);
      if (ST_sclass(base) == SCLASS_EXTERN) {
        // ipa can cause it to be based on global extern,
        // in which case it is already emitted. 
        continue;
      }
      FmtAssert(ST_class(base) == CLASS_BLOCK && STB_section(base),
                ("inito (%s) not allocated?", ST_name(st)));
      LAI_init_section(base); //make sure base is inited 
      // may need padding between objects in same section,
      // so always change origin
      Change_Section_Origin (base, ofst);
      Write_INITO (ino, STB_scninfo_idx(base), ofst);
    }

    else {
      st_processed[ST_index(st)] = TRUE;
      Base_Symbol_And_Offset(st, &base, &ofst);
      LAI_init_section(base); // make sure base is inited
      // we cannot assume that constants are sequentially ordered
      // by offset, because they are allocated on the fly as we
      // expand the whirl nodes.  So always reset the origin.
      Change_Section_Origin (base, ofst);

      // emit TCON associated symbolic name as label
      char *cname = Get_TCON_name (ST_tcon(st));
      fprintf(Lai_File, "%s:\n", cname);
      Write_TCON (&ST_tcon_val(st), STB_scninfo_idx(base), ofst, 1);
    }
  }

  return;
}

/* ====================================================================
 *    Process_Distr_Array ()
 * 
 *    Write all distr_array INITOs from the global symtab
 * ====================================================================
 */
static void
Process_Distr_Array ()
{
  for (UINT i = 1; i < INITO_Table_Size(GLOBAL_SYMTAB); ++i) {
    INITO* ino = &Inito_Table(GLOBAL_SYMTAB,i);
    ST* st = INITO_st(ino);
    if (!ST_is_not_used(st) &&
        ST_sclass(st) == SCLASS_DISTR_ARRAY) {
      INT64 ofst;
      ST* base;
      Base_Symbol_And_Offset(st, &base, &ofst);
      FmtAssert(ST_class(base) == CLASS_BLOCK && STB_section(base),
                ("inito (%s) not allocated?", ST_name(st)));
      LAI_init_section(base);
      Change_Section_Origin(base, ofst);
      Write_INITO(ino, STB_scninfo_idx(base), ofst);
    }
  }

  return;
}

/* ====================================================================
 *    Process_Bss_Data (stab)
 *
 *    iterate through the global symbols and write (s)bss symbols 
 *    to sections 
 * ====================================================================
 */
static void
Process_Bss_Data (
  SYMTAB_IDX stab
)
{
  /* 
   * To guarantee in increasing order, create vector of st*, 
   * then stable_sort on section, then stable_sort on offset. 
   */
  vector< ST* > bss_list;
  vector< ST* >::iterator bssp;

  // This routine can be called multiple times for the global symtab;
  // we do this so that objects are emitted in order.
  // For each section, some objects are in local symtab, and some in global.
  // We keep track of the last global symbol that has been processed
  // so that we only process new symbols.

  static UINT last_global_index = 1;
  UINT first_index = (stab == GLOBAL_SYMTAB ? last_global_index : 1);
  
  for (UINT i = first_index; i < ST_Table_Size(stab); ++i) {
    ST* sym = &St_Table(stab,i);
    if (ST_class(sym) == CLASS_BLOCK)
      continue;	// not a leaf symbol
    if (!Has_Base_Block(sym))
      continue;	// not a data symbol
    if (ST_sclass(sym) == SCLASS_UGLOBAL ||
        ST_sclass(sym) == SCLASS_FSTATIC ||
        ST_sclass(sym) == SCLASS_PSTATIC) {
      bss_list.push_back (sym);	// bss symbol
    }
  }

  if (stab == GLOBAL_SYMTAB) {
    last_global_index = ST_Table_Size(GLOBAL_SYMTAB);
  }

  // It's a bit counter-intuitive, but to get the list sorted
  // by section and then by offset within section,
  // should stable_sort in reverse order (offset then section).
  stable_sort (bss_list.begin(), bss_list.end(), size_lt);
  stable_sort (bss_list.begin(), bss_list.end(), offset_lt);
  stable_sort (bss_list.begin(), bss_list.end(), section_lt);

  ST*   sym;
  ST*   next_sym;
  ST*   base;
  ST*   next_base;
  INT64 ofst;
  INT64 next_ofst;
  INT64 size;

  for (bssp = bss_list.begin(); bssp != bss_list.end(); ++bssp) {

    sym = *bssp;
    Base_Symbol_And_Offset(sym, &base, &ofst);
    if (ST_class(base) != CLASS_BLOCK || !STB_section(base))
      continue;	/* not allocated */
    if (!STB_nobits(base))
      continue;	/* not a bss symbol */

    Change_Section_Origin (base, ofst);

    size = TY_size(ST_type(sym));
    Print_Label (Lai_File, sym, size);
    // before emitting space,
    // first check whether next symbol has same offset.
    // can have overlapping symbols that are equivalenced.
    if (bssp+1 != bss_list.end()) {
      next_sym = *(bssp+1);
      Base_Symbol_And_Offset(next_sym, &next_base, &next_ofst);
      if (next_base == base && next_ofst == ofst) {
	// skip to next iteration so label printed before space emitted.
	continue;
      }
      else if (next_base == base && next_ofst < (ofst+size)) {
	// have label in middle of structure,
	// so don't emit space for full structure.
	size = next_ofst - ofst;
      }
    }
    // assume here that if multiple symbols with same offset,
    // are sorted so that largest size is last.
    if (size > 0)
      fprintf(Lai_File, "\t%s %lld\n", AS_SPACE, size);
	/*      ASM_DIR_SKIP(Lai_File, size); */

    if (!ST_is_export_local(sym)) {
      LAI_Put_Elf_Symbol (sym);
    }
  }
}

/* ====================================================================
 *   Branch_Skips_First_Op
 *
 *   Check if <pred> is a branch to <succ> that skips the first instruction
 *   in the <succ>. This typically happens after filling the branch delay
 *   slot with the first instruction in the target basic block.
 * ====================================================================
 */
static BOOL
Branch_Skips_First_Op (
  BB *pred, 
  BB *succ
) 
{
  OP *op;

  if (BB_next(pred) == succ) return FALSE;

  op = BB_branch_op (pred);
  if (op != NULL) {
    INT i;
    for (i = 0; i < OP_opnds(op); i++) {
      TN *opnd_tn = OP_opnd(op,i);
      if (TN_is_label(opnd_tn) && TN_offset(opnd_tn) != 0) return TRUE;
    }
  }
  return FALSE;
}

/* ====================================================================
 *   Check_If_Should_Align_BB
 *
 *   Check if bb should be aligned, 
 *   and return number of instructions it should be aligned with.
 * ====================================================================
 */
BOOL
Check_If_Should_Align_BB (
  BB *bb, 
  INT32 curpc, 
  INT *num_of_ops
)
{
  BBLIST *preds;
  INT32 targpc;
  float skip_freq = 0.01;	    /* not zero so can divide */
  float noskip_freq = 0.01;
  INT targ_alignment;
  *num_of_ops = 0; 		    /* zero to begin with */
#define FREQUENT_BB_DIFF 5.0

  /*
   * Align loops for best processor efficiency.
   * Originally checked if bb_loophead, but now just
   * check frequency of block (in case doesn't look like proper loop,
   * but still has high-frequency).
   */
  if (OPT_Space)
    return FALSE;	/* don't align */
  if (BB_freq(bb) <= 1.0)
    return FALSE;	/* not frequent enough, so don't align */
  if (BB_prev(bb) == NULL) 
    return FALSE;	/* nowhere to put nops */

  /* don't want to add nops to previous bb 
   * unless current bb is significantly more frequent. */
  if (BB_freq(bb) / BB_freq(BB_prev(bb)) < FREQUENT_BB_DIFF)
    return FALSE;

  /* first check whether target is always label+4 */
  FOR_ALL_BB_PREDS (bb, preds) {
    if (Branch_Skips_First_Op (BBLIST_item(preds), bb)) {
      skip_freq += BB_freq(BBLIST_item(preds));
    } else {
      noskip_freq += BB_freq(BBLIST_item(preds));
    }
  }
  if ((skip_freq / noskip_freq) > FREQUENT_BB_DIFF)
    targpc = PC_Incr(curpc);
  else if ((noskip_freq / skip_freq) > FREQUENT_BB_DIFF)
    targpc = curpc;
  else {
    /* mixture of skip and noskip branches, 
     * or just not frequent enough,
     * so don't align */
    return FALSE;
  }

  if (Trace_Emit) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile, "align_bb %d:  bb_freq = %f, prev_bb_freq = %f, skip_freq = %f, noskip_freq = %f\n",
	    BB_id(bb), BB_freq(bb), BB_freq(BB_prev(bb)), 
	    skip_freq, noskip_freq);
  }

  if (Align_Instructions)
    targ_alignment = Align_Instructions;
  else
    targ_alignment = CGTARG_Text_Alignment();

  targ_alignment /= INST_BYTES;	                 /* so word-sized */
  targpc /= INST_BYTES;		                 /* so word-sized */
  *num_of_ops = 
       ((targ_alignment - (targpc % targ_alignment)) % targ_alignment);

  return TRUE;
}

/* ====================================================================
 *   Check_If_Should_Align_PU
 * ====================================================================
 */
static INT
Check_If_Should_Align_PU (
  INT curpc
)
{
  INT q;

  if (Align_Instructions) {
    q = Align_Instructions;
  }
  else if (OPT_Space) {
    return 0;
  }
  else {
    q = CGTARG_Text_Alignment();
  }
  q /= INST_BYTES;	                   /* so word-sized */
  return (q - ((curpc/INST_BYTES) % q)) % q;
}

/* ====================================================================
 *   Create_Cold_Text_Section
 *
 *    Scan the BBs in the region looking for cold BBs. If one is found,
 *    create the cold text section if it hasn't already been created.
 *    Also set the BB_cold BB flag accordingly.
 * ====================================================================
 */
static void
Create_Cold_Text_Section (void)
{
  BB *bb;

  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    if (LAI_emit_use_cold_section && BB_Is_Cold(bb)) {
      if (cold_base == NULL) {
	ST *st = Copy_ST(text_base);
	Set_ST_blk(st, Copy_BLK(ST_blk(text_base)));
	Set_ST_name (st, Save_Str2(ELF_TEXT, ".cold"));
	Set_STB_size (st, 0);
	Set_STB_scninfo_idx(st, 0);
	Set_STB_section_idx(st, STB_section_idx(text_base));
	LAI_init_section (st);
	cold_base = st;
      }

      /* Check the remaining BBs in the region to verify they are
       * are cold. cgemit doesn't require this attribute, but currently
       * this is how the cold region is generated, so it's helpful
       * to verify it since someone forgetting to set the rid on a
       * new BB will cause trouble.
       */
      do {
	FmtAssert(BB_Is_Cold(bb),
		  ("emit: hot BB:%d found in cold region", BB_id(bb)));
	Set_BB_cold(bb);
      } while (bb = BB_next(bb));

      return;
    }

    Reset_BB_cold(bb);
  }
}

/* ====================================================================
 *   Setup_Text_Section_For_BB
 *
 *   Set PU_base, PU_section and PC according to whether <bb> is
 *   in the hot or cold region.
 * ====================================================================
 */
static void
Setup_Text_Section_For_BB (
  BB *bb
)
{
  BOOL cold_bb = BB_cold(bb);
  PU_base = cold_bb ? cold_base : text_base;
  if (cur_section != PU_base) {
    fprintf (Lai_File, "\n\t%s %s\n", AS_SECTION, ST_name(PU_base));
    if (cold_bb) {
      PU_section = cold_section;
      text_PC = PC;
      PC = cold_PC;
    } else {
      PU_section = text_section;
      cold_PC = PC;
      PC = text_PC;
    }
    cur_section = PU_base;
  }
}

/* ====================================================================
 *   Setup_Text_Section_For_PU
 *
 *   Setup, and create if necessary, the text section of the PU.  
 * ====================================================================
 */
static void
Setup_Text_Section_For_PU (
  ST *pu
)
{
  static ST *orig_text_base = NULL;
  ST        *old_base = PU_base;
  INT        i;

  if (text_base == NULL) {
    text_base = SEC_block(SEC_text());
  }
  orig_text_base = text_base;

  current_pu = ST_pu(pu);

  Initial_Pu_Label = LABEL_IDX_ZERO;

  if (generate_elf_symbols) {
    // didn't actually write instructions,
    // but want the offset to be up-to-date.
  }

  if (Section_For_Each_Function || PU_in_elf_section(current_pu)) {
    /* create new text section */
    text_base = Copy_ST(orig_text_base);
    Set_ST_blk(text_base, Copy_BLK(ST_blk(orig_text_base)));

    // use same .text name for each section
    /*
    Set_ST_name (text_base, ST_name_idx(orig_text_base));
    */

    char buf[16];
    sprintf(buf, "%s.", ELF_TEXT);
    Set_ST_name (text_base, Save_Str2(buf, ST_name(pu)));
    Set_STB_size (text_base, 0);
    Set_STB_scninfo_idx(text_base, 0);
    Set_STB_section_idx(text_base, STB_section_idx(orig_text_base));
    LAI_init_section (text_base);
    text_PC = 0;	/* starting new offsets */
  }
  else if (ST_base(pu) != text_base) {
    text_base = ST_base(pu);
    text_PC = STB_size(text_base);
  }

  Set_STB_scninfo_idx(SP_Sym, STB_scninfo_idx(text_base));
  Set_STB_scninfo_idx(FP_Sym, STB_scninfo_idx(text_base));

  cur_section = text_base;
  if (generate_elf_symbols) {
    ;
  }

  fprintf (Lai_File, "\n\t%s %s\n", AS_SECTION, ST_name(text_base));

  i = Check_If_Should_Align_PU (text_PC);
  while (i > 0) {
    fprintf (Lai_File, "\t%s\n", CGTARG_Top_Name(CGTARG_Noop_Top()));
    text_PC = PC_Incr(text_PC);
    i--;
  }

  // hack for supporting dwarf generation in assembly (suneel)
  Last_Label = Gen_Label_For_BB (REGION_First_BB);
  Offset_From_Last_Label = 0;
  if (Initial_Pu_Label == LABEL_IDX_ZERO) {
    Initial_Pu_Label = Last_Label;
  }

  /* check if we are changing sections. */
  if (text_base != old_base) {
    if (generate_elf_symbols && old_base != NULL) {
      /*
      pSCNINFO old_section = em_scn[STB_scninfo_idx(old_base)].scninfo;
      */
      // Arange is symbolic; line number entries (if used) are not.
      /*
      end_previous_text_region(old_section, 
                                   Em_Get_Section_Offset(old_section));
      */
    }
    if (generate_dwarf) {
      /*
    	Em_Dwarf_Start_Text_Region_Semi_Symbolic (PU_section, text_PC,
			Cg_Dwarf_Symtab_Entry(CGD_LABIDX,
			Last_Label,
			ST_elf_index(text_base)),
			Offset_From_Last_Label);
      */
    }
  }

  PC = text_PC;
  PU_base = text_base;

  return;
}

/* ====================================================================
 *   put_TN_comment
 *
 *   Add comment about the TN to comment buffer.
 * ====================================================================
 */
static void
put_TN_comment (
  TN *t, 
  BOOL add_name, 
  vstring *comment
)
{
  if (!add_name) return;	// don't duplicate name
  if (!TN_is_constant(t)) return;

  INT64 val = TN_offset(t);

  if (TN_is_symbol(t)) {
    if (ST_class(TN_var(t)) == CLASS_CONST) {
      *comment = vstr_concat (*comment, 
		   Targ_Print (NULL, ST_tcon_val(TN_var(t))) );
    }
    else {
      *comment = vstr_concat (*comment, ST_name(TN_var(t)));
    }
    if (TN_offset(t) != 0) {
      vstr_sprintf (comment, vstr_len(*comment), "%+lld", val);
    }
  }
  else if (TN_is_label(t) && val != 0) {
    *comment = vstr_concat (*comment, LABEL_name(TN_label(t)));
    vstr_sprintf (comment, vstr_len(*comment), "%+lld", val); 
  }
}

/* ====================================================================
 *   r_apply_l_const
 *
 *   Format the given constant TN's "value" into the given buffer.
 *   Return whether the symbol name should be added to comments.
 * ====================================================================
 */
static BOOL
r_apply_l_const (
  OP *op,		/* OP with constant operand */
  INT opidx,		/* OP index of constant TN of some sort */
  vstring *buf		/* A buffer to format it into */
)
{
  TN *t = OP_opnd(op, opidx);
  INT paren = 0;
  BOOL hexfmt = FALSE;
  BOOL print_TN_offset = TRUE;
  BOOL add_name = FALSE;
  ST *st;
  INT64 val;

  /* special case for stack symbols */
  if (TN_is_symbol(t)) {
    ST *base_st;
    INT64 base_ofst;

    st = TN_var(t);
    Base_Symbol_And_Offset (st, &base_st, &base_ofst);
    if (base_st == SP_Sym || base_st == FP_Sym) {
      TN_format_sp_relative(t, base_ofst, buf);
      return TRUE;
    }
  }

  val = TN_offset(t);
  TN_format_reloc (t, buf);

  if (TN_is_symbol(t)) {
    st = TN_var(t);

    if (ST_class(st) == CLASS_CONST) {
      char *cname = Get_TCON_name (ST_tcon(st));
      *buf = vstr_concat(*buf, cname);
    }
    else {

      if (ST_sclass(st) != SCLASS_COMMON) {
	// call put_symbol so that we emit .type info, once per symbol
	LAI_Put_Elf_Symbol (st);
      }

      if (TN_relocs(t) != 0) {
	// use base if referring to current pu or local data
	/* don't do this
	if (TN_Use_Base_ST_For_Reloc (TN_relocs(t), st)) {
	  ST *base_st;
	  INT64 base_ofst;

	  Base_Symbol_And_Offset (st, &base_st, &base_ofst);

	  val += base_ofst;
	  st = base_st;
	}
	*/

	if (Use_Separate_PU_Section(current_pu,st)) {
	  /* use PU text section rather than generic one */
	  st = PU_base;
	}

	hexfmt = TRUE;
	print_TN_offset = TRUE;
	// add name to comments in case was confused by gp_disp.
	add_name = TRUE;
	paren = TN_Relocs_In_Asm (t, st, buf, &val);
	*buf = vstr_concat (*buf, "(" );
	r_qualified_name (st, buf);
	/*
	*buf = vstr_concat (*buf, ST_name(st));
	*buf = vstr_concat (*buf, Symbol_Name_Suffix);
	*/
      } 
      else {
	r_qualified_name (st, buf);
	/*
	*buf = vstr_concat(*buf, ST_name(st));
	if (*Symbol_Name_Suffix != '\0') {
	  *buf = vstr_concat(*buf, Symbol_Name_Suffix);
	}
	*/
      }
    } /* not CLASS_CONST */
  }
  else if ( TN_is_label(t) ) {
    if (val != 0) {
      // don't use "." cause that can have varying meaning
      // when have multiple instruction slots.
      // Instead just do label+offset.
      *buf = vstr_concat (*buf, LABEL_name(TN_label(t)));
      vstr_sprintf (buf, vstr_len(*buf), "%+lld", val); 
    }
    else {
      *buf = vstr_concat(*buf, LABEL_name(TN_label(t)));
      if (isdigit(LABEL_name(TN_label(t))[0])) {
      	*buf = vstr_concat(*buf, 
                (PC > Get_Label_Offset(TN_label(t))) ? "b" : "f");
      }
    }
    print_TN_offset = FALSE;
  }
  else if (TN_is_tag(t)) {
    *buf = vstr_concat(*buf, LABEL_name(TN_label(t)));
    print_TN_offset = FALSE;
  }
  else if (TN_is_enum(t)) {
    if (ISA_PRINT_Operand_Is_Part_Of_Name(OP_code(op), opidx)) {
      vstr_sprintf (buf, vstr_len(*buf), "%s", ISA_ECV_Name(TN_enum(t)) );
    } else {
      vstr_sprintf (buf, vstr_len(*buf), "%d", ISA_ECV_Intval(TN_enum(t)) );
    }
    print_TN_offset = FALSE;	/* because value used instead */
  }
  else if ( TN_has_value(t) ) {
    if ( TN_size(t) <= 4 )
      vstr_sprintf (buf, vstr_len(*buf), 
		(hexfmt ? "0x%x" : "%d"), (mINT32)TN_value(t) );
    else
      vstr_sprintf (buf, vstr_len(*buf), 
      		(hexfmt ? "0x%llx" : "%lld"), TN_value(t) );
    print_TN_offset = FALSE;	/* because value used instead */
  }
  else {
    #pragma mips_frequency_hint NEVER
    FmtAssert (FALSE, ("r_apply_l_const: illegal constant TN"));
  }

  if (print_TN_offset && (val != 0)) {
    vstr_sprintf (buf, vstr_len(*buf), "%+lld", val );
  }

  while ( paren > 0 ) {
    *buf = vstr_concat(*buf, ")");
    --paren;
  }
  return add_name;
}

/* ====================================================================
 *   print_prefetch_info(char *comment, WN *wn)
 *
 *   add prefetch info comments to the assembler listing.
 * ====================================================================
 */
static void 
print_prefetch_info(
  vstring *comment, 
  WN *wn)
{

  if (!wn) return;

  if (WN_pf_stride_1L (wn) != 0) {
    *comment = vstr_concat(*comment, "L1");
    return;
  }

  if (WN_pf_stride_2L (wn) != 0 ) {
    *comment = vstr_concat(*comment, "L2");
  }
}

/* ====================================================================
 *   r_assemble_opnd
 * ====================================================================
 */
static char*
r_assemble_opnd (
  OP      *op,
  INT      i,
  vstring *buf,
  BOOL    *add_name
)
{
  char  vname[10];
  INT   start = vstr_len(*buf);	// start of operand string
  TN   *t = OP_opnd(op,i);

  if (TN_is_constant(t)) {
    *add_name |= r_apply_l_const (op, i, buf);
  }
  else if (TN_is_true_pred(t)) {
    /* nada */ ;
  }
  else {
    const char *rname;
    ISA_REGISTER_CLASS rc = TN_register_class(t);
    REGISTER reg = TN_register(t);

    // If register is physical, print its real name, otherwise
    // virtual:
    if (reg != REGISTER_UNDEFINED) {

      ISA_REGISTER_SUBCLASS sc = OP_opnd_reg_subclass(op,i);

      if (REGISTER_SET_MemberP(REGISTER_SUBCLASS_members(sc), reg)
	      && REGISTER_SUBCLASS_reg_name(sc, reg)) {
	rname = REGISTER_SUBCLASS_reg_name(sc, reg);
      } 
      else if (List_Software_Names) {
	rname = ABI_PROPERTY_Reg_Name(rc, REGISTER_machine_id(rc, reg));
      } 
      else {
	rname = REGISTER_name(rc, reg);
      }
    }
    else {
      sprintf(vname, "%s%d", ISA_REGISTER_CLASS_Symbol(rc), TN_number(t));
      rname = vname;
    }

    if (i == OP_PREDICATE_OPND && OP_has_predicate(op)) {
      vstr_sprintf(buf, start, ISA_PREDICATE_FORMAT(op), rname);
    } 
    else {
      *buf = vstr_concat(*buf, rname);
    }
  }

  // need end-of-string between each operand
  *buf = vstr_append(*buf, '\0');

  return vstr_str(*buf)+start;
}

/* ====================================================================
 *   r_assemble_result
 * ====================================================================
 */
static char*
r_assemble_result (
  OP *op,
  INT i,
  vstring *buf
)
{
  char  vname[10];
  INT start = vstr_len(*buf);	// start of operand string
  TN *t = OP_result(op,i);

  const char *rname;
  ISA_REGISTER_SUBCLASS sc = OP_result_reg_subclass(op,i);
  ISA_REGISTER_CLASS rc = TN_register_class(t);
  REGISTER reg = TN_register(t);

  FmtAssert (t != NULL && !TN_is_constant(t),
	                     ("r_assemble_list: illegal result tn"));

  if (reg != REGISTER_UNDEFINED) {
    if (REGISTER_SET_MemberP(REGISTER_SUBCLASS_members(sc), reg)
	  && REGISTER_SUBCLASS_reg_name(sc, reg)) {
      rname = REGISTER_SUBCLASS_reg_name(sc, reg);
    } 
    else if (List_Software_Names) {
      rname = ABI_PROPERTY_Reg_Name(rc, REGISTER_machine_id(rc, reg));
    } 
    else {
      rname = REGISTER_name(rc, reg);
    }
  }
  else {
    sprintf(vname, "%s%d", ISA_REGISTER_CLASS_Symbol(rc), TN_number(t));
    rname = vname;
  }

  *buf = vstr_concat(*buf, rname);
  *buf = vstr_append(*buf, '\0');	// increment vstr length
  return vstr_str(*buf)+start;
}

/* ====================================================================
 *   r_assemble_list
 *
 *   Emit a pseudo-assembly listing for the given OP.
 * ====================================================================
 */
static void 
r_assemble_list (
  OP *op,		/* The operation being listed */
  BB *bb
)
{
  const char *result[ISA_OPERAND_max_results];
  const char *opnd[ISA_OPERAND_max_operands];
  vstring buf = vstr_begin(LBUF_LEN);
  INT i;
  INT lc = 0;
  BOOL add_name = FALSE;

  for (i = 0; i < OP_opnds(op); i++) {
    opnd[i] = r_assemble_opnd(op, i, &buf, &add_name);
  }

  for (i = 0; i < OP_results(op); i++) {
    result[i] = r_assemble_result (op, i, &buf);
  }

  fputc ('\t', Lai_File);
  lc = TI_ASM_Print_Inst(OP_code(op), result, opnd, Lai_File);
  FmtAssert (lc != TI_RC_ERROR, ("%s", TI_errmsg));
  vstr_end(buf);

  while (lc < 30) {
    fputc (' ', Lai_File);
    lc++;
  }

  /* emit TN comments: */
  vstring comment = vstr_begin(LBUF_LEN);
  for (i = 0; i < OP_results(op); i++) {
    TN *tn = OP_result(op, i);
    if (TN_is_save_reg(tn)) {
      vstr_sprintf (&comment, vstr_len(comment), "(sv:%s)", 
                  REGISTER_name(TN_save_rclass(tn), TN_save_reg(tn)));
    }
  }
  for (i = 0; i < OP_opnds(op); i++) {
    put_TN_comment (OP_opnd(op,i), add_name, &comment);
  }

  fprintf (Lai_File, "\t%s", ASM_CMNT);
  if (vstr_len(comment) == 0) {
    WN *wn = Get_WN_From_Memory_OP (op);
    if (wn && Alias_Manager) {
      char tbuf[LBUF_LEN];
      tbuf[0] = '\0';
      Print_alias_info (tbuf, Alias_Manager, wn);
      comment = vstr_concat(comment, tbuf);
    }
    if (wn && OP_prefetch(op) && vstr_len(comment) == 0) {
      print_prefetch_info(&comment, wn);
    }
  }

  /* Add target PU name as comment for normal call and tail call OPs.
   */
  if ((OP_call(op) || OP_tail_call(op)) && vstr_len(comment) == 0) {
    ANNOTATION *ant = ANNOT_Get (BB_annotations(bb), ANNOT_CALLINFO);
    if (ant != NULL) {
	ST *call_sym = CALLINFO_call_st(ANNOT_callinfo(ant));
	if (call_sym != NULL) {
		comment = vstr_concat(comment, ST_name(call_sym));
	}
    }
  }
  fprintf (Lai_File, "  %s\n", vstr_str(comment));
  vstr_end(comment);

  return;
}

/* ====================================================================
 *   Verify_Operand
 *
 *   Verify that the specified operand is valid for the particular
 *   instruction operand.
 * ====================================================================
 */
static void 
Verify_Operand (
  const ISA_OPERAND_INFO *oinfo,
  OP *op, 
  INT opnd, 
  BOOL is_result
)
{
  const ISA_OPERAND_VALTYP *vtype =   is_result 
				    ? ISA_OPERAND_INFO_Result(oinfo, opnd)
				    : ISA_OPERAND_INFO_Operand(oinfo, opnd);
  TN *tn = is_result ? OP_result(op, opnd) : OP_opnd(op, opnd);
  const char * const res_or_opnd = is_result ? "result" : "operand";

  if (Trace_Emit) {
    fprintf(TFile, "checking %s ", res_or_opnd);
    Print_TN(tn, TRUE);
    fprintf(TFile, "\n");
  }

  if (ISA_OPERAND_VALTYP_Is_Register(vtype)) {
    REGISTER_SET class_regs;
    ISA_REGISTER_SUBCLASS sc = ISA_OPERAND_VALTYP_Register_Subclass(vtype);
    ISA_REGISTER_CLASS rc = ISA_OPERAND_VALTYP_Register_Class(vtype);
    REGISTER reg = TN_register(tn);

    FmtAssert(TN_is_register(tn),
	      ("%s %d is not a register", res_or_opnd, opnd));

    FmtAssert(TN_register_class(tn) == rc,
	      ("incorrect register class for %s %d", res_or_opnd, opnd));

    if (reg != REGISTER_UNDEFINED) {
      class_regs =   (sc == ISA_REGISTER_SUBCLASS_UNDEFINED)
		 ? REGISTER_CLASS_universe(rc)
		 : REGISTER_SUBCLASS_members(sc);
      FmtAssert(REGISTER_SET_MemberP(class_regs, reg),
	      ("incorrect register for %s %d", res_or_opnd, opnd));
    }
  } else if (ISA_OPERAND_VALTYP_Is_Literal(vtype)) {
    FmtAssert(TN_is_constant(tn),
	     ("%s %d is not a constant", res_or_opnd, opnd));

    if (TN_has_value(tn)) {
      ISA_LIT_CLASS lc = ISA_OPERAND_VALTYP_Literal_Class(vtype);
      INT64 imm = TN_value(tn);

      if ((TFile != stdout) && !ISA_LC_Value_In_Class(imm, lc)) {
        Print_OP_No_SrcLine (op);
      }
      FmtAssert(ISA_LC_Value_In_Class(imm, lc),
	        ("literal for %s %d is not in range", res_or_opnd, opnd));
    } else if (TN_is_label(tn)) {

#if 0
      LABEL_IDX lab = TN_label(tn);
      INT64 offset = TN_offset(tn);
      INT64 val = Get_Label_Offset(lab) + offset;
      if (CG_opt_level > 0 && CFLOW_Enable && !OP_likely(op)) {
	INT nextpc = PC + sizeof(ISA_BUNDLE);
	if (PROC_has_branch_delay_slot()) nextpc += sizeof(ISA_PACK_INST);

	if (val == nextpc) {
	  DevWarn("branch to next instruction at PC=0x%x", PC);
	}
      }
      if (Get_Label_Offset(lab) == 0 && offset == 0) {
	BBLIST *item;
	BB *label_bb = Get_Label_BB(lab);
	BOOL okay = FALSE;
	FOR_ALL_BB_SUCCS(OP_bb(op), item) {
	  if (BBLIST_item(item) == label_bb) okay = TRUE;
	}
	if (!okay) {
	  DevWarn("branch to 0? (possible bug at PC=0x%x, label %s)", 
		   PC, LABEL_name(lab));
	}
      }
#endif
    }
  } else if (ISA_OPERAND_VALTYP_Is_Enum(vtype)) {
    ISA_ENUM_CLASS_VALUE ecv;
    ISA_ENUM_CLASS ec = ISA_OPERAND_VALTYP_Enum_Class(vtype);
    ISA_ENUM_CLASS_VALUE first_ecv = ISA_EC_First_Value(ec);
    ISA_ENUM_CLASS_VALUE last_ecv = ISA_EC_Last_Value(ec);

    FmtAssert(TN_is_enum(tn),
	      ("%s %d is not an enum", res_or_opnd, opnd));

    ecv = TN_enum(tn);
    FmtAssert(ecv >= first_ecv && ecv <= last_ecv,
	      ("enum for %s %d is not in range", res_or_opnd, opnd));
  } else {
    FmtAssert(FALSE, ("unhandled vtype in Verify_Operand"));
  }
}

/* ====================================================================
 *   Verify_Instruction
 *
 *   Verify that the specified OP contains valid information for
 *   the instruction it represents.
 * ====================================================================
 */
void 
Verify_Instruction (
  OP *op
)
{
  INT i;
  const ISA_OPERAND_INFO *oinfo;
  TOP top = OP_code(op);

  // ??? check for valid topcode?

  FmtAssert(ISA_SUBSET_Member(ISA_SUBSET_Value, top),
	    ("%s is a member of ISA %s", 
	     TOP_Name(top), 
	     ISA_SUBSET_Name(ISA_SUBSET_Value)));

  oinfo = ISA_OPERAND_Info(top);

  INT results = OP_results(op);
  if (results != TOP_fixed_results(top)) {
    FmtAssert(TOP_is_var_opnds(top) && results > TOP_fixed_results(top),
	      ("wrong number of results (%d) for %s",
	       results,
	       TOP_Name(top)));
    results = TOP_fixed_results(top); // can only verify fixed results
  }
  for (i = 0; i < results; ++i) {
    Verify_Operand(oinfo, op, i, TRUE);
  }

  INT opnds = OP_opnds(op);
  if (opnds != TOP_fixed_opnds(top)) {
    FmtAssert(TOP_is_var_opnds(top) && opnds > TOP_fixed_opnds(top),
	      ("wrong number of operands (%d) for %s",
	       opnds,
	       TOP_Name(top)));
    opnds = TOP_fixed_opnds(top); // can only verify fixed operands
  }
  for (i = 0; i < opnds; ++i) {
    Verify_Operand(oinfo, op, i, FALSE);
  }
}

/* ====================================================================
 *   print_source
 * ====================================================================
 */
static void
print_source (
  SRCPOS srcpos
)
{
  USRCPOS usrcpos;

  USRCPOS_srcpos(usrcpos) = srcpos;

  /* TODO: we don't handle this yet. */
  if (USRCPOS_filenum(usrcpos) == 0) return;

  return;
}

/* ====================================================================
 *   add_line_entry
 *
 *   This adds line info and, as a side effect,
 *   builds tables in dwarf2 for the file numbers
 * =====================================================================
 */
void
add_line_entry (
  INT code_address, 
  SRCPOS srcpos
)
{
  static SRCPOS last_srcpos = 0;
  USRCPOS usrcpos;

  if (srcpos == 0 && last_srcpos == 0)
	DevWarn("no valid srcpos at PC %d\n", code_address);
  if (srcpos == 0 || srcpos == last_srcpos) return;

  // TODO:  figure out what to do about line changes in middle of bundle ???
  // for now, avoid that situation by a hack;
  // otherwise it dies in libdwarf because libdwarf expects addresses
  // to be aligned with instructions.
  if ((code_address % INST_BYTES) != 0) return;

  USRCPOS_srcpos(usrcpos) = srcpos;

  // now do line number:
  if(!LAI_emit_asm_dwarf) {
    fprintf (Lai_File, "// "); //turn the rest into comment
  }

  fprintf (Lai_File, "\t.loc\t%d\t%d\t%d\n", 
		USRCPOS_filenum(usrcpos)-1,
		USRCPOS_linenum(usrcpos),
		USRCPOS_column(usrcpos));

  if (List_Source)
    print_source (srcpos);

  last_srcpos = srcpos;
}

/* ====================================================================
 *   Assemble_OP
 *
 *   Write out the 'op' into the object file and/or into the assembly 
 *   file.
 * ====================================================================
 */
static INT
Assemble_OP (
  OP *op, 
  BB *bb, 
  INT slot
)
{
  INT words;
  INT i;

  if (Trace_Emit) {
    Print_OP(op);
  }

  Verify_Instruction(op);

  if (OP_prefetch(op)) Use_Prefetch = TRUE;

  add_line_entry (PC, OP_srcpos(op));

  r_assemble_list (op, bb);

  PC = PC_Incr_N(PC, words);

  // hack to keep track of last label and offset for assembly dwarf (suneel)
  // If the current op is a TOP_asm, mark the Last_Label as invalid.
  if (OP_code(op) == TOP_asm) {
    Last_Label = LABEL_IDX_ZERO;
  }
  else {
    Offset_From_Last_Label = PC_Incr_N(Offset_From_Last_Label, words);
  }

  return words;
}

/* ====================================================================
 *   Generate_Asm_String
 * ====================================================================
 */
static char*
Generate_Asm_String (
  OP* asm_op
)
{
  UINT i;
  ASM_OP_ANNOT* asm_info = (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, asm_op);
  char* asm_string = strdup(WN_asm_string(ASM_OP_wn(asm_info)));

  for (i = 0; i < OP_results(asm_op); i++) {
    asm_string = CGTARG_Modify_Asm_String(asm_string, 
                                   ASM_OP_result_position(asm_info)[i], 
                                   ASM_OP_result_memory(asm_info)[i], 
                                   OP_result(asm_op, i));
  }

  for (i = 0; i < OP_opnds(asm_op); i++) {
    asm_string = CGTARG_Modify_Asm_String(asm_string, 
                                   ASM_OP_opnd_position(asm_info)[i], 
                                   ASM_OP_opnd_memory(asm_info)[i], 
                                   OP_opnd(asm_op, i));
  }

  CGTARG_Postprocess_Asm_String(asm_string);

  return asm_string;
}

/* ====================================================================
 *   gen_asm_frame
 *
 *   Generate the .frame, .mask and the .fmask directives for 
 *   the assembler. 
 * ====================================================================
 */
void
gen_asm_frame (
  INT64 frame_len
)
{
  // .fframe is only used for unwind info,
  // and we plan on emitting that info directly.
#if 0
  if (Current_PU_Stack_Model != SMODEL_SMALL)
    fprintf (Lai_File, "\t%s\t%lld\n", AS_FRAME, frame_len);
  else
    fprintf (Lai_File, "\t%s\t%lld\n", AS_FRAME, frame_len);
#endif
}


/* ====================================================================
 *   print_entry_in_asm
 *
 *   Generate the entry (.proc) directive.
 * ====================================================================
 */
void 
print_entry_in_asm (
  ST *pu
)
{
  BB_LIST *ent_list;

  fprintf (Lai_File, "\t%s ", AS_ENT);

  /*
  Write_Qualified_Name(Lai_File, pu);
  */

  for (ent_list = Entry_BB_Head; ent_list; ent_list = BB_LIST_rest(ent_list)) {
    BB *bb = BB_LIST_first(ent_list);
    ANNOTATION *ant = ANNOT_Get (BB_annotations(bb), ANNOT_ENTRYINFO);
    ENTRYINFO *ent = ANNOT_entryinfo(ant);
    ST *entry_sym = ENTRYINFO_name(ent);

    if (!ST_is_not_used(entry_sym)) {
      const char *entry_name = ST_name(entry_sym);
      if (strcmp(Cur_PU_Name, entry_name ) != 0) {
	fprintf (Lai_File, ", %s", entry_name);
      }
    }
  }

  fprintf (Lai_File, "\n");
}

/* ====================================================================
 *   Assemble_Simulated_OP
 *
 *   Assemble a simulated OP.
 * ====================================================================
 */
static void
Assemble_Simulated_OP (
  OP *op, 
  BB *bb
)
{
  /* ASM is an odd case so we handle it specially. It doesn't expand
   * into a series of OPs, but rather we just emit the asm string into
   * the assembly output.
   */
  if (OP_code(op) == TOP_asm) {
    fprintf(Lai_File, "\t%s\n", Generate_Asm_String(op));
    return;
  }

  if (Trace_Emit) {
    fprintf (TFile, "<emit> transform simulated OP: ");
    Print_OP (op);
  }

  if (OP_code(op) == TOP_intrncall) {
    ASM_DIR_NOTRANSFORM();
    FmtAssert(FALSE, ("Assemble_Simulated_OP: intrinsics ??"));

    ASM_DIR_TRANSFORM();
    return;
  }

  OPS ops = OPS_EMPTY;

  Exp_Simulated_Op (op, &ops, PC);

  if (Trace_Emit) {
    fprintf (TFile, "... to: ");
    Print_OPS (&ops);
  }

  FOR_ALL_OPS_OPs_FWD (&ops, op) {
    INT words;
    /*    Perform_Sanity_Checks_For_OP(op, FALSE); */
    words = Assemble_OP (op, bb, 0);
  }

  return;
}

/* ====================================================================
 *   Assemble_Ops
 *
 *   Assemble the OPs in a BB an OP at a time.
 * ====================================================================
 */
static void
Assemble_Ops (
  BB *bb
)
{
  OP *op;

  FOR_ALL_BB_OPs_FWD(bb, op) {
    INT words;

    if (OP_dummy(op)) continue;		// these don't get emitted

    if (OP_simulated(op)) {
      Assemble_Simulated_OP(op, bb);
      continue;
    }

    /*    Perform_Sanity_Checks_For_OP(op, TRUE); */
    words = Assemble_OP (op, bb, 0);
  }

  return;
}

/* ====================================================================
 *   Emit_Loop_Note
 *
 *   Emit a loop note to the .s file, anl file, etc.
 * ====================================================================
 */
static void
Emit_Loop_Note (
  BB *bb, 
  FILE *file
)
{
  /*  BOOL anl_note = file == anl_file; */
  BB *head = BB_loop_head_bb(bb);
  UINT16 unrollings = BB_unrollings(bb);
  ANNOTATION *info_ant = ANNOT_Get(BB_annotations(head), ANNOT_LOOPINFO);
  LOOPINFO *info = info_ant ? ANNOT_loopinfo(info_ant) : NULL;
  BOOL unroll_pragma = FALSE;
  ANNOTATION *unroll_ant = ANNOT_Get(BB_annotations(head), ANNOT_PRAGMA);

  while (unroll_ant && WN_pragma(ANNOT_pragma(unroll_ant)) != WN_PRAGMA_UNROLL)
    unroll_ant = ANNOT_Get(ANNOT_next(unroll_ant), ANNOT_PRAGMA);
  if (unroll_ant) {
    WN *wn = ANNOT_pragma(unroll_ant);
    if (WN_pragma_arg1(wn) > 1) {
      if (WN_pragma_arg1(wn) == unrollings)
	unroll_pragma = TRUE;
      else if (BB_innermost(bb))
	DevWarn("BB:%d unrolled %d times but pragma says to unroll %d times",
		BB_id(bb), unrollings, WN_pragma_arg1(wn));
    }
  }

  if (bb == head) {
    SRCPOS srcpos = BB_Loop_Srcpos(bb);
    INT32 lineno = SRCPOS_linenum(srcpos);

    /*
    if (anl_note) {
      INT32 fileno = SRCPOS_filenum(srcpos);
      INT32 colno = SRCPOS_column(srcpos);
      fprintf (anl_file,
	       "\nmsg loop lines [%d %d %d]",
	       fileno,
	       lineno,
	       colno);
    } else {
    */
      fprintf (file, "%s<loop> Loop body line %d", ASM_CMNT_LINE, lineno);
      /*
    }
      */

    if (info) {
      WN *wn = LOOPINFO_wn(info);
      TN *trip_tn = LOOPINFO_trip_count_tn(info);
      BOOL constant_trip = trip_tn && TN_is_constant(trip_tn);
      INT depth = WN_loop_depth(wn);
      const char *estimated = constant_trip ? "" : "estimated ";
      INT64 trip_count = constant_trip ? TN_value(trip_tn) :
					 (INT64)WN_loop_trip_est(wn);
      /*
      const char *fmt =   anl_note
			? " \"nesting depth: %d, %siterations: %lld\""
			: ", nesting depth: %d, %siterations: %lld";
      */
      const char *fmt = ", nesting depth: %d, %siterations: %lld";

      fprintf (file, fmt, depth, estimated, trip_count);
    }

    fprintf (file, "\n");
  } else {
    /*
          if (anl_note) {
    */
    /* Only interested in loop head messages for anl file
     */
    /*
    return;
    } else 
*/

    ANNOTATION *lbl_ant = ANNOT_Get(BB_annotations(head), ANNOT_LABEL);
    DevAssert(lbl_ant, ("loop head BB:%d has no label", BB_id(head)));
    fprintf(file,
	    "%s<loop> Part of loop body line %d"
	    ", head labeled %s\n",
	    ASM_CMNT_LINE, BB_Loop_Lineno(head), LABEL_name(ANNOT_label(lbl_ant)));
  }

  if (unrollings > 1) {
    /*
    if (anl_note) {
      fprintf(anl_file, "\"unrolled %d times%s%s\"\n", unrollings,
	      BB_unrolled_fully(bb) ? " (fully)" : "",
	      unroll_pragma ? " (pragma)" : "");
    } else {
    */
      fprintf(file, "%s<loop> unrolled %d times%s%s\n", 
	      ASM_CMNT_LINE,
	      unrollings,
	      BB_unrolled_fully(bb) ? " (fully)" : "",
	      unroll_pragma ? " (pragma)" : "");
      /*
    }
      */
  }
}

/* ====================================================================
 *   LAI_Assemble_BB
 *
 *   Assemble the contents of the given BB.
 * ====================================================================
 */
static void
LAI_Assemble_BB ( 
  BB *bb, 
  WN *rwn 
)
{
  ST *st;
  ANNOTATION *ant;
  RID *rid = BB_rid(bb);

  if (Trace_Emit) {
    #pragma mips_frequency_hint NEVER
    fprintf(TFile, "assemble BB %d\n", BB_id(bb));
  }

  if (rid != NULL && RID_cginfo(rid) != NULL) {
    if (current_rid == RID_id(rid)) {
      /* same, so do nothing */
    }
    else if (current_rid > 0) {
      /* changing regions */
      fprintf (Lai_File, "%s END REGION %d\n",
				         ASM_CMNT_LINE, current_rid);
      if (RID_id(rid) > 0 && !RID_is_glue_code(rid)) {
	fprintf (Lai_File, "%s BEGIN REGION %d\n",
					ASM_CMNT_LINE, 	RID_id(rid));
	current_rid = RID_id(rid);
      }
      else if (RID_is_glue_code(rid)) {
	current_rid = 0;	/* pretend not a region */
      }
      else {
	current_rid = RID_id(rid);
      }
    }
    else if (RID_id(rid) > 0 && !RID_is_glue_code(rid)) {
      /* beginning a region */
      fprintf (Lai_File, "%s BEGIN REGION %d\n", 
				         ASM_CMNT_LINE, RID_id(rid));
      current_rid = RID_id(rid);
    }
  }
  else if (current_rid > 0) {
    /* ending a region */
    fprintf (Lai_File, "%s END REGION %d\n", ASM_CMNT_LINE, current_rid);
    current_rid = 0;	/* no rid */
  }

  if (BB_entry(bb)) {
    char *entry_name;
    ST *entry_sym; 
    ENTRYINFO *ent;
    SRCPOS entry_srcpos;
    ant = ANNOT_Get (BB_annotations(bb), ANNOT_ENTRYINFO);
    ent = ANNOT_entryinfo(ant);
    entry_srcpos = ENTRYINFO_srcpos(ent);
    entry_sym = ENTRYINFO_name(ent);
    entry_name = ST_name(entry_sym);

    /* Set an initial line number so that if the first inst in the BB
     * has no srcpos, then we'll be ok.
     */
    if (entry_srcpos)
      add_line_entry (PC, entry_srcpos);

    if (ST_is_not_used(entry_sym)) {
      // don't emit alt-entry if marked as not-used
      DevWarn("CG reached entry marked as unused; will ignore it (%s)\n", 
		entry_name);
    }
    else {
      Set_ST_ofst(entry_sym, PC);
      if (strcmp(Cur_PU_Name, entry_name) != 0) {
	// alt-entry

	fprintf (Lai_File, "\t%s\t%s\n", AS_AENT, entry_name);
	Print_Label (Lai_File, entry_sym, 0 );

	LAI_Put_Elf_Symbol (entry_sym);
      }
    }
  }

  /* List labels attached to BB: */
  for (ant = ANNOT_First (BB_annotations(bb), ANNOT_LABEL);
       ant != NULL;
       ant = ANNOT_Next (ant, ANNOT_LABEL)) {
    LABEL_IDX lab = ANNOT_label(ant);

    fprintf (Lai_File, "%s:\t%s 0x%llx\n", 
		   LABEL_name(lab), ASM_CMNT, Get_Label_Offset(lab) );
  }

  // hack to keep track of last label and offset for assembly dwarf (suneel)
  if (Last_Label == LABEL_IDX_ZERO) {
    Last_Label = Gen_Label_For_BB (bb);
    Offset_From_Last_Label = 0;
    if (Initial_Pu_Label == LABEL_IDX_ZERO) {
      Initial_Pu_Label = Last_Label;
    }
  }

  st = BB_st(bb);
  if (st) {
    fprintf (Lai_File, "%s:\t%s 0x%llx\n", ST_name(st), ASM_CMNT, ST_ofst(st));

    Is_True (ST_ofst(st) == PC, ("st %s offset %lld doesn't match PC %d", 
	ST_name(st), ST_ofst(st), PC));
    Is_True (!Has_Base_Block(st) 
	     || (ST_base(st) == (BB_cold(bb) ? cold_base : text_base)),
	     ("sym %s base doesn't match BB:%d",
	     ST_name(st), BB_id(bb)));
    FmtAssert(ST_is_export_local(st),
	      ("ST for BB:%d not EXPORT_LOCAL", BB_id(bb)));
  }

  /* write out all the notes for this BB */
  if (List_Notes) {
    if (BB_loop_head_bb(bb)) {
      Emit_Loop_Note(bb, Lai_File);
    }

    if (BB_has_note(bb)) {
      NOTE_BB_Act (bb, NOTE_PRINT_TO_FILE, Lai_File);
    }

    FREQ_Print_BB_Note(bb, Lai_File);
  }

#if Is_True_On
  /*  Init_Sanity_Checking_For_BB (); */
#endif

  Assemble_Ops(bb);

  return;
}

/* ====================================================================
 *   Recompute_Label_Offset
 * ====================================================================
 */
static void
Recompute_Label_Offset (
  INT32 pcs[2]
)
{
  BB *bb;
  INT32 cur_pcs[2];

  /* recompute the addresses of all labels in the procedure. */
  cur_pcs[IHOT] = text_PC;
  cur_pcs[ICOLD] = cold_PC;
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    ANNOTATION *ant;
    OP *op;
    INT isect = BB_cold(bb) ? ICOLD : IHOT;
    INT32 cur_pc = cur_pcs[isect];
    LABEL_IDX lab;
    ST *st = BB_st(bb);
    if (st) {
      Set_ST_ofst(st, cur_pc);
      Set_ST_base(st, BB_cold(bb) ? cold_base : text_base);
    }
    for (ant = ANNOT_First (BB_annotations(bb), ANNOT_LABEL);
         ant != NULL;
         ant = ANNOT_Next (ant, ANNOT_LABEL))
    {
      lab = ANNOT_label(ant);
      Set_Label_Offset(lab, cur_pc);
    }
    for (op = BB_first_op(bb); op; op = OP_next(op)) {
      if (OP_has_tag(op)) {
	lab = Get_OP_Tag(op);
      	Set_Label_Offset(lab, cur_pc);
      }
      INT num_inst_words = 1;
      cur_pc = PC_Incr_N(cur_pc, num_inst_words);
    }
    cur_pcs[isect] = cur_pc;
  }

  Is_True(cur_pcs[IHOT] == pcs[IHOT],
	 ("Recompute_Label_Offsets: hot region PC mismatch (%d vs %d).",
	  cur_pcs[IHOT], pcs[IHOT]));
  Is_True(cur_pcs[ICOLD] == pcs[ICOLD],
	 ("Recompute_Label_Offsets: cold region PC mismatch (%d vs %d).",
	  cur_pcs[ICOLD], pcs[ICOLD]));
}

/* ====================================================================
 *   Fixup_Long_Branches
 * ====================================================================
 */
static void
Fixup_Long_Branches (
  INT32 *hot_size, 
  INT32 *cold_size
)
{
  FmtAssert(FALSE,
	    ("Fixup_Long_Branches: not implemented"));
}

/* ====================================================================
 *   R_Resolve_Branches
 *
 *   Assign addresses to all local labels. Fixup long branches.
 * ====================================================================
 */
static void
R_Resolve_Branches (
  ST *pu_st
)
{
  BB *bb;
  BB *prev_nonempty_bbs[2] = { NULL, NULL };
  INT32 curpcs[2];
  INT32 hot_size, cold_size;

  curpcs[IHOT] = text_PC;
  curpcs[ICOLD] = cold_PC;
  for (bb = REGION_First_BB; bb; bb = BB_next(bb)) {
    INT32 bb_start_pc;
    OP *op;
    INT32 isect = BB_cold(bb) ? ICOLD : IHOT;
    INT32 curpc = curpcs[isect];
    BB *prev_nonempty_bb = prev_nonempty_bbs[isect];

    /* need prev bb to align */
    if (prev_nonempty_bb != NULL) {
      INT32 num;
      Check_If_Should_Align_BB (bb, curpc, &num);
      if (num != 0) {
	OP *new_op;
	OPS new_ops = OPS_EMPTY;
	if (Trace_Emit) {
	  #pragma mips_frequency_hint NEVER
	  fprintf(TFile, "insert %d noops at bb %d\n", num, BB_id(bb));
	}
	curpc = PC_Incr_N(curpc, num);
	do {
      	  Exp_Noop(&new_ops);
          new_op = OPS_last(&new_ops);
	  OP_scycle(new_op) = OP_scycle(BB_last_op(prev_nonempty_bb));
	  BB_Append_Op (prev_nonempty_bb, new_op);
	} while (--num);
      }
    }

    bb_start_pc = curpc;

    /* If there is no label, make one: */
    Gen_Label_For_BB ( bb );

    for (op = BB_first_op(bb); op; op = OP_next(op)) {
      INT num_inst_words = 1;
      curpc = PC_Incr_N(curpc, num_inst_words);
    }
    
    if (curpc != bb_start_pc) prev_nonempty_bbs[isect] = bb;

    curpcs[isect] = curpc;
  }

  Recompute_Label_Offset(curpcs);

  hot_size = curpcs[IHOT] - text_PC;
  cold_size = curpcs[ICOLD] - cold_PC;

  // if large text size or has branch predicts (which are limited in size),
  // then check for long branches.
  if (hot_size >= MIN_BRANCH_DISP || cold_size >= MIN_BRANCH_DISP
    || CGTARG_Has_Branch_Predict()) 
  {
    Fixup_Long_Branches (&hot_size, &cold_size);
  }

  if (generate_elf_symbols) {
    ;
  }

  if (Trace_Emit) {
    #pragma mips_frequency_hint NEVER
    fprintf (TFile, "INSTS IN PROCEDURE: %d\n", hot_size + cold_size);
  }
}

/* ====================================================================
 *    emit_global_symbols
 *
 *    initialize any new global sections.
 *    We don't do this in LAI_Begin_File because may not have done
 *    data layout at that point.
 * ====================================================================
 */
static void
emit_global_symbols ()
{
  INT i;
  static UINT last_global_index = 1;

  for (i = last_global_index; i < ST_Table_Size(GLOBAL_SYMTAB); ++i) {
    ST* sym = &St_Table(GLOBAL_SYMTAB,i);

    if (ST_class(sym) == CLASS_BLOCK && STB_section(sym)) {
      LAI_init_section(sym);
    }

    // emit commons here so order is preserved for datapools
    if (ST_sclass(sym) == SCLASS_COMMON) {
      if (ST_is_not_used (sym)) continue;
      LAI_Put_Elf_Symbol (sym);
    }
  }
  last_global_index = ST_Table_Size(GLOBAL_SYMTAB);

  fprintf(Lai_File,"\n");

  return;
}

/* ====================================================================
 *    emit_function_prototypes
 * ====================================================================
 */
static void
emit_function_prototypes ()
{
  INT i, j;
  static UINT last_global_index = 1;

  for (i = last_global_index; i < ST_Table_Size(GLOBAL_SYMTAB); ++i) {
    ST* sym = &St_Table(GLOBAL_SYMTAB,i);

    if (ST_is_not_used(sym))
      continue;

    if (ST_class(sym) == CLASS_FUNC) {
      ISA_REGISTER_CLASS rc;

      TY_IDX prototype = PU_prototype(Pu_Table[ST_pu(sym)]);

      if (!TY_has_prototype(prototype)) continue;

      FmtAssert(prototype != 0,
		("emit_function_prototypes: empty function prototype"));

      fprintf(Lai_File,"\t.proto\t%s", ST_name(sym));

      // parameters:
      TYLIST_ITER fn_iter = Make_tylist_iter(TY_parms(prototype));
      while (*fn_iter) {
	rc = Register_Class_For_Mtype(TY_mtype(Ty_Table[*fn_iter]));
	fprintf(Lai_File,", %s", ISA_REGISTER_CLASS_ASM_Name (rc));
	*fn_iter++;
      }
      if (TY_is_varargs(prototype))
	fprintf(Lai_File,", ...");


      fprintf(Lai_File," => "); //, ST_name(sym));
      // return info:
      RETURN_INFO return_info = Get_Return_Info(TY_ret_type(prototype),
                                              Complex_Not_Simulated);
      for (j = 0; j < RETURN_INFO_count(return_info)-1; j++) {
	rc = Register_Class_For_Mtype(RETURN_INFO_mtype (return_info, j));
	fprintf(Lai_File,"%s, ", ISA_REGISTER_CLASS_ASM_Name (rc));
      }

      if (RETURN_INFO_count(return_info)) {
	rc = Register_Class_For_Mtype(RETURN_INFO_mtype (return_info, 
                                      RETURN_INFO_count(return_info)-1));
	fprintf(Lai_File,"%s\n", ISA_REGISTER_CLASS_ASM_Name (rc));
      }
      else {
	fprintf(Lai_File,"void\n");
      }
    }
  }
  last_global_index = ST_Table_Size(GLOBAL_SYMTAB);

  return;
}

/* ====================================================================
 *    emit_input_directives
 * ====================================================================
 */
static void
emit_input_directives ()
{
  INT    i;
  ST    *sym;
  ST    *base;
  INT64  ofst;

  FOREACH_SYMBOL (CURRENT_SYMTAB, sym, i) {
    if (ST_is_not_used(sym)) continue;
    if (ST_sclass(sym) == SCLASS_FORMAL) {
      Base_Symbol_And_Offset (sym, &base, &ofst);
      /*
      base = ST_base(sym);
      ofst = ST_ofst(sym);
      */

      fprintf (Lai_File, "\t%s\t.input %s at %s, %lld\n", 
                   ASM_CMNT, ST_name(sym), ST_name(base), ofst);
    }
  }

  return;
}

/* ====================================================================
 *    emit_output_directives
 * ====================================================================
 */
static void
emit_output_directives ()
{
  BB *bb;
  OP *op;

  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    if (!BB_exit(bb)) continue;
    FOR_ALL_BB_OPs_FWD(bb, op) {
	TN *tn = OP_opnd(op, 0);
	ISA_REGISTER_CLASS rc = TN_register_class(tn);
	fprintf(Lai_File, "\t.output %s%d\t\t\t\t%s\n",
             ISA_REGISTER_CLASS_Symbol(rc), TN_number(tn), ASM_CMNT);
    }
  }

  return;
}

/* ====================================================================
 *    emit_virtual_registers
 *
 *    Check the TN_Vec[] to see what TNs exist.
 * ====================================================================
 */
static void
emit_virtual_registers ()
{
  INT i, j;
  TN *tn;
  ISA_REGISTER_CLASS rc;
  INT tct[ISA_REGISTER_CLASS_MAX+1];
  TN *tns[ISA_REGISTER_CLASS_MAX+1][Last_TN - First_Regular_TN + 1];

  if (Trace_Emit) {
    Print_TNs ();
  }

  FOR_ALL_ISA_REGISTER_CLASS(rc) {
    tct[rc] = 0;
  }

  for (i = First_Regular_TN; i <= Last_TN; i++) {
    tn = TNvec(i);

    if (TN_is_register(tn)) {
      rc = TN_register_class(tn);
      tns[rc][tct[rc]] = tn;
      tct[rc] += 1;
    }
  }

  FOR_ALL_ISA_REGISTER_CLASS(rc) {

    if (tct[rc] == 0) continue;

    //    fprintf(Lai_File, "\t.%s\t", ISA_REGISTER_CLASS_ASM_Name (rc));
    for (j = 0; j < tct[rc]; j++) {
      tn = tns[rc][j];

      if (j % 6 == 0)
	fprintf(Lai_File, "\n\t.%s\t %s%d",
           ISA_REGISTER_CLASS_ASM_Name (rc),
		ISA_REGISTER_CLASS_Symbol(rc), TN_number(tn));
      else
	fprintf(Lai_File, ", %s%d",
                     ISA_REGISTER_CLASS_Symbol(rc), TN_number(tn));
    }
    fprintf(Lai_File, "\n");
    //    tn = tns[rc][tct[rc]-1];
    //    fprintf(Lai_File, "%s%d \n",
    //                     ISA_REGISTER_CLASS_Symbol(rc), TN_number(tn));
  }

  return;
}

/* ====================================================================
 *    LAI_Emit_PU (pu, pu_dst, rwn)
 * ====================================================================
 */
void
LAI_Emit_PU (
  ST      *pu,
  DST_IDX  pu_dst, 
  WN      *rwn
)
{
  ST *sym;
  ST *base;
  BB *bb;
  INT Initial_Pu_PC;
  INT64 ofst;
  INT i;

  Init_ST_elf_index(CURRENT_SYMTAB);

  cur_section = NULL;

  // I cannot do this while it is constantly changing !
  // When they learn to work out the specs and abide by them,
  // and communicate, I might enable this again, Arthur.
  /*
  emit_function_prototypes ();
  */

  // initialize any new global sections:
  // We don't do this in LAI_Begin_File because may not have done
  // data layout at that point.
  emit_global_symbols ();

  // emit global bss first so .org is correct
  Process_Bss_Data (GLOBAL_SYMTAB);

  /* Initialize any sections that might have been created by the backend. */
  FOREACH_SYMBOL (CURRENT_SYMTAB, sym, i) {
    base = Base_Symbol(sym);
    if (ST_class(base) == CLASS_BLOCK && STB_section(base)) {
      LAI_init_section(base);
    }
  }

  /*
  emit_function_prototypes ();
  */

  Create_Cold_Text_Section();
  Setup_Text_Section_For_PU (pu);

  Initial_Pu_PC = PC;
  Set_ST_ofst(pu, PC);

  /* 
   * Assign addresses to all local labels. Fixup long branches.
   * Perform branch target optimization.
   */
  R_Resolve_Branches (pu);

  fprintf (Lai_File, "\n\t%s Program Unit: %s\n", ASM_CMNT, ST_name(pu) );
  if (AS_ENT) 
    print_entry_in_asm (pu);

  Print_Label (Lai_File, pu, 0);
  gen_asm_frame (Frame_Len);

  if (cur_section != PU_base) {
    /* reset to text section */
    fprintf (Lai_File, "\n\t%s %s\n", AS_SECTION, ST_name(PU_base));
    cur_section = PU_base;
  }

  emit_input_directives ();

  FOREACH_SYMBOL (CURRENT_SYMTAB, sym, i) {
    if (ST_is_not_used(sym)) continue;
    if (ST_sclass(sym) == SCLASS_COMMON) {
      LAI_Put_Elf_Symbol (sym);
    }

    /* put all extern symbols into the elf symbol table. */
    if ((PU_src_lang(Get_Current_PU()) == PU_F77_LANG 
	|| PU_src_lang(Get_Current_PU()) == PU_F90_LANG) &&
	ST_sclass(sym) == SCLASS_EXTERN && 
	! ST_is_export_local(sym)) {

      fprintf (Lai_File, "\t%s\t %s\n", AS_GLOBAL, ST_name(sym));
    }

    if (ST_class(sym) == CLASS_VAR && ST_sclass(sym) == SCLASS_AUTO) {
      if (Has_Base_Block(sym)) {
	Base_Symbol_And_Offset(sym, &base, &ofst);
	fprintf (Lai_File, "\t%s\t.auto %s at %s, %lld\n", 
                        ASM_CMNT, ST_name(sym), ST_name(base), ofst);
      }
    }
  }

  /* emit vertual register declarations */
  emit_virtual_registers ();

  /* Assemble each basic block in the PU */
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    Setup_Text_Section_For_BB(bb);
    LAI_Assemble_BB (bb, rwn);
  }

  /* Revert back to the text section to end the PU. */
  Setup_Text_Section_For_BB(REGION_First_BB);
  Is_True(PU_base == text_base, ("first region BB was not in text section"));

  /* Emit the stuff needed at the end of the PU. */
  if (AS_END) {
    fprintf (Lai_File, "\t%s\t", AS_END);
    Write_Qualified_Name(Lai_File, pu);
    fprintf (Lai_File, "\n");
  }

  /* Emit the initialized data associated with this PU. */
  Process_Initos_And_Literals (CURRENT_SYMTAB);
  Process_Bss_Data (CURRENT_SYMTAB);

  if (generate_dwarf) {
    ;
  }

  Set_STB_size (PU_base, PC);
  text_PC = PC;

  if (generate_dwarf) {
    ;
  }

  return;
}

/* ====================================================================
 *    LAI_Begin_File (process_name, options)
 * ====================================================================
 */
void
LAI_Begin_File (
  char *process_name,	/* Back end process name */
  char *options 	/* Option string used for compilation */
)
{
  char *buff;

  Trace_Emit = Get_Trace (TP_EMIT, 1);

  text_PC = 0;
  cold_PC = 0;
  Get_Ism_Name();
  if (FILE_INFO_gp_group (File_info)) {
    Has_GP_Groups = TRUE;
  }

  if (LAI_emit_asm_dwarf) {
    generate_dwarf = TRUE;
    generate_elf_symbols = TRUE;
  }

  Init_ST_elf_index(GLOBAL_SYMTAB);
  /*  Cg_Dwarf_Gen_Asm_File_Table (); */

  fprintf (Lai_File, "\t%s  %s::%s\n", ASM_CMNT, process_name, 
			    INCLUDE_STAMP);
  if (*ism_name != '\0')
    fprintf (Lai_File, "\t%s%s\t%s\n", ASM_CMNT, "ism", ism_name);

  List_Compile_Options (Lai_File, "\t"ASM_CMNT, FALSE, TRUE, TRUE);

  /* TODO: do we need to do .interface stuff for asm files? */

  return;
}

/* ====================================================================
 *    LAI_End_File ()
 * ====================================================================
 */
void
LAI_End_File( void )
{
  INT16 i;
  ST *sym;

  cur_section = NULL;
  Init_ST_elf_index(GLOBAL_SYMTAB);

  /* make sure all global symbols are initialized */
  FOREACH_SYMBOL (GLOBAL_SYMTAB, sym, i) {
    if (ST_class(sym) == CLASS_BLOCK && STB_section(sym)) {
      if (Emit_Global_Data && SEC_is_merge(STB_section_idx(sym)) )
	continue;	// merge sections go in each .o
      LAI_init_section(sym);
    }
    // emit commons here so order is preserved for datapools
    if (ST_sclass(sym) == SCLASS_COMMON) {
      /* check if sym is used */
      if (ST_is_not_used (sym)) continue;
      /* check if symbol is already there. */
      if (ST_elf_index(sym) != 0) continue;
      LAI_Put_Elf_Symbol (sym);
    }
  }

  if (Emit_Global_Data) {
    char *newname;
    // create dummy symbol to represent the section
    FOREACH_SYMBOL (GLOBAL_SYMTAB, sym, i) {
      if (ST_class(sym) != CLASS_BLOCK) continue;
      if (!STB_section(sym)) continue;
      // mergeable sections will be emitted into each .o
      if (SEC_is_merge(STB_section_idx(sym))) continue;
      newname = Index_To_Str(Save_Str2(ST_name(sym), "_symbol"));

      Change_Section_Origin (sym, 0);
      fprintf (Lai_File, "\t%s\t%s\n", AS_GLOBAL, newname);
      ASM_DIR_STOINTERNAL(newname);
      fprintf (Lai_File, "%s:\n", newname);
    }
  }

  /* 
   * If there weren't any PUs, we may have data initialization
   * associated with file scope data here:
   */
  Process_Bss_Data (GLOBAL_SYMTAB);
  Process_Initos_And_Literals (GLOBAL_SYMTAB);
  // We need two calls to  Process_Initos_And_Literals (GLOBAL_SYMTAB)
  // because while writing out INITOs new literals may be allocated
  Process_Initos_And_Literals (GLOBAL_SYMTAB);
  Process_Distr_Array ();

  FOREACH_SYMBOL (GLOBAL_SYMTAB, sym, i) {
    if (ST_class(sym) == CLASS_NAME) {
      if (ST_emit_symbol(sym)) {
	/* may be notype symbol */
	LAI_Put_Elf_Symbol(sym);
      }
    }

    if (ST_class(sym) == CLASS_VAR &&
        ST_is_fill_align(sym) &&
        !Has_Base_Block(sym)) {
      /* fill/align symbols are supposed to be allocated in be
       * but are not done if there were no PUs in the file.
       * Report that error here.
       */

      ErrMsg (EC_Is_Bad_Pragma_Abort, 
              "Fill/Align symbol",
              ST_name(sym),
              "requires the file to contain at least one function");
    }
	
    if (Has_Strong_Symbol(sym)) {
      ST *strongsym = ST_strong(sym);
      unsigned char symtype;

      if (!Has_Base_Block(strongsym))
	continue;	/* strong not allocated, so ignore weak */

      if (ST_class(sym) == CLASS_FUNC) {
	symtype = STT_FUNC;
      }
      else {
	symtype = STT_OBJECT;
      }

      Write_Weak_Alias (sym, strongsym);
      Print_Dynsym (Lai_File, sym);
    }
    else if (Has_Base_Block(sym) && ST_class(ST_base(sym)) != CLASS_BLOCK
	&& ST_emit_symbol(sym)) {
      // alias
      if (!ST_is_export_local(sym)) {
	fprintf (Lai_File, "\t%s\t %s\n", AS_GLOBAL, ST_name(sym));
      }
      Write_Alias (sym, ST_base(sym));
    }
    else if (ST_class(sym) == CLASS_FUNC && ST_emit_symbol(sym)
	// possible to have local not-used emit_symbols,
	// cause mp does that to mark them as unused,
	// and don't want to emit those.
	&& ST_sclass(sym) == SCLASS_EXTERN) {
      // some unreferenced fortran externs need to be emitted
      LAI_Put_Elf_Symbol(sym);
      fprintf (Lai_File, "\t%s\t %s\n", AS_GLOBAL, ST_name(sym));
    }
  }

  /* Write out the initialized data to the object file. */
  for (i = 1; i <= last_scn; i++) {
    UINT32 tmp, power;

    sym = em_scn[i].sym;
    power = 0;
    for (tmp = STB_align(sym); tmp > 1; tmp >>= 1) power++;
    fprintf (Lai_File, "\t%s %s\n", AS_SECTION, ST_name(sym));
    fprintf(Lai_File, "\t%s %d\n", AS_ALIGN,STB_align(sym));
  }

  fprintf(Lai_File, "//\t%s %d\n", AS_GPVALUE, GP_DISP);

  /* do I need this ? */

  if (Emit_Global_Data) {
    // prepare block data to be written to .G file.
    // need to remove section info so can be reset when read.
    FOREACH_SYMBOL (GLOBAL_SYMTAB, sym, i) {
      if (ST_class(sym) == CLASS_CONST
		  && SEC_is_merge(STB_section_idx(ST_base(sym))) ) {
	// reallocate in each file
	Set_ST_base(sym,sym);	
	Set_ST_ofst(sym,0);	
      }
      else if (ST_class(sym) == CLASS_VAR ||
	       ST_class(sym) == CLASS_CONST ||
	       ST_class(sym) == CLASS_FUNC) {
	if (ST_sclass (sym) != SCLASS_COMMON && !ST_is_weak_symbol(sym) ) {
	  Set_ST_sclass(sym, SCLASS_EXTERN);
	}
      }
      if (ST_class(sym) != CLASS_BLOCK) continue;
      Set_STB_scninfo_idx(sym,0);
      Set_STB_compiler_layout(sym);
      if (STB_section(sym)) {
	Reset_STB_section(sym);
	Reset_STB_root_base(sym);
	Set_STB_section_idx(sym,0);
	Set_ST_name(sym, Save_Str2(ST_name(sym), "_symbol"));
	Set_ST_sclass(sym, SCLASS_EXTERN);
	Set_ST_export(sym, EXPORT_INTERNAL);
      }
      else {
	// in case non-section block
	Set_ST_sclass(sym, SCLASS_EXTERN);
      }
    }
  }
}
