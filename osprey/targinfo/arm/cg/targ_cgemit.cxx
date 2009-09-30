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

  For further information regarding this notice, see:

*/

#include <elf.h>

#define	USE_STANDARD_TYPES 1
#include "defs.h"
#include "targ_isa_lits.h"
#include "vstring.h"
#include "config_asm.h"
#include "config_list.h"
#include "em_elf.h"
#include "dwarf_DST_mem.h"         /* for DST_IDX */
#include "symtab.h"
#include "cgir.h"
#include "cgemit.h"
#include "cgemit_targ.h"
#include "data_layout.h"
#include "bb.h"
#include "op.h"
/* #include "iface_scn.h" */
#include "cg_flags.h"
#include "glob.h"

/* [JC]: Emit register list if needed */
BOOL
CGEMIT_Register_List(OP *op, INT opnd_idx, vstring *buf) {

  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(OP_code(op));
  ISA_OPERAND_USE this_use = ISA_OPERAND_INFO_Use(oinfo, opnd_idx);
  /* Given operand has not the reglist property */
  if(!(this_use & OU_reglist)) { return FALSE; }

  const ISA_REGISTER_CLASS_INFO * rc_info = ISA_REGISTER_CLASS_Info(ISA_REGISTER_CLASS_integer);  

  TN *tn = OP_opnd(op,opnd_idx);
  INT64 tn_value = TN_value(tn);

  INT base_reg_idx = -1;
  INT cur_reg_idx = 0;
  BOOL reg_already_emitted = FALSE;
  while(tn_value != 0) {
    if(tn_value & 1) {
      if(base_reg_idx == -1) {
	base_reg_idx = cur_reg_idx;
	if(reg_already_emitted) { vstr_sprintf (buf, vstr_len(*buf), ",%s",ISA_REGISTER_CLASS_INFO_Reg_Name(rc_info,base_reg_idx)); }
	else {
	  vstr_sprintf (buf, vstr_len(*buf), "%s",ISA_REGISTER_CLASS_INFO_Reg_Name(rc_info,base_reg_idx));
	  reg_already_emitted = TRUE;
	}
      }
      else {
	/* Last bit set to 1 */
	/* base_reg_idx != -1 */
	if(tn_value == 1) {
	  if(cur_reg_idx > base_reg_idx) { vstr_sprintf (buf, vstr_len(*buf), "-%s",ISA_REGISTER_CLASS_INFO_Reg_Name(rc_info,cur_reg_idx)); }
	  else { 
	    if(reg_already_emitted) {
	      vstr_sprintf (buf, vstr_len(*buf), ",%s",ISA_REGISTER_CLASS_INFO_Reg_Name(rc_info,cur_reg_idx));
	    }
	    else {
	      vstr_sprintf (buf, vstr_len(*buf), "%s",ISA_REGISTER_CLASS_INFO_Reg_Name(rc_info,cur_reg_idx));
	    }
	  }
	}
      }
    }
    else {
      if(base_reg_idx != -1) {
	if(cur_reg_idx > (base_reg_idx +  1)) { vstr_sprintf (buf, vstr_len(*buf), "-%s",ISA_REGISTER_CLASS_INFO_Reg_Name(rc_info,cur_reg_idx-1)); }
      }
      base_reg_idx = -1;
    }
    tn_value >>= 1;
    cur_reg_idx++;
  }
  return TRUE;
}

/* 
 * [CG]: Always use write qualified name instead of ST_name() when emitting
 * assembly. The symbol may be renamed, e.g. static symbols under IPA or
 * symbols that are function statics.
 */
     
/* ====================================================================
 *   CGEMIT_TN_Value_In_Hexa_Format
 * ====================================================================
 */

BOOL
CGEMIT_TN_Value_In_Hexa_Format( OP *op, TN *t )
{
  return FALSE;
}


/* ====================================================================
 *    CGEMIT_Weak_Alias (sym, stringsym)
 *
 * ====================================================================
 */
void
CGEMIT_Weak_Alias (
  ST *sym, 
  ST *strongsym
) 
{
  fprintf (Asm_File, "\t%s\t", AS_WEAK);
  EMT_Write_Qualified_Name(Asm_File, sym);
  fprintf (Asm_File, "\n");
  fprintf (Asm_File, "\t.set ");
  EMT_Write_Qualified_Name(Asm_File, sym);
  fprintf (Asm_File, ", ");
  EMT_Write_Qualified_Name(Asm_File, strongsym);
  fprintf (Asm_File, "\n");
}

/* ====================================================================
 *    CGEMIT_Alias (sym, stringsym)
 *
 * ====================================================================
 */
void
CGEMIT_Alias (ST *sym, ST *strongsym) 
{
  fprintf (Asm_File, "\t.set ");
  EMT_Write_Qualified_Name(Asm_File, sym);
  fprintf (Asm_File, ", ");
  EMT_Write_Qualified_Name(Asm_File, strongsym);
  fprintf (Asm_File, "\n");
}

/* ====================================================================
 *   CGEMIT_Prn_Ent_In_Asm
 *
 *   Generate the entry (.proc) directive.
 * ====================================================================
 */
void 
CGEMIT_Prn_Ent_In_Asm (
  ST *pu
)
{
  BB_LIST *ent_list;

#  ifdef AS_USED
//  TB: ST_is_used(pu) and PU_no_delete(Get_Current_PU()) are synonymous execpt for 
//  if (PU_no_delete(Get_Current_PU())) {
  if (ST_is_used(pu)) {
    fprintf (Asm_File, "\t%s\t", AS_TYPE);
    EMT_Write_Qualified_Name(Asm_File, pu);
    fprintf (Asm_File, ", %s, %s\n", AS_TYPE_FUNC, AS_USED);
  }
  else { 
    fprintf (Asm_File, "\t%s\t", AS_TYPE);
    EMT_Write_Qualified_Name(Asm_File, pu);
    fprintf (Asm_File, ", %s\n", AS_TYPE_FUNC);
  }
#endif

#if 0
  fprintf (Asm_File, "\t%s\t", AS_ENT);
  EMT_Write_Qualified_Name(Asm_File, pu);
#endif

  for (ent_list = Entry_BB_Head; 
       ent_list; 
       ent_list = BB_LIST_rest(ent_list)) {
    BB *bb = BB_LIST_first(ent_list);
    ANNOTATION *ant = ANNOT_Get (BB_annotations(bb), ANNOT_ENTRYINFO);
    ENTRYINFO *ent = ANNOT_entryinfo(ant);
    ST *entry_sym = ENTRYINFO_name(ent);

    if (!ST_is_not_used(entry_sym)) {
      const char *entry_name = ST_name(entry_sym);
      if (strcmp(Cur_PU_Name, entry_name ) != 0) {
	fprintf (Asm_File, ", %s", entry_name);
      }
    }
  }

  fprintf (Asm_File, "\n");

  return;
}

/* ====================================================================
 *    CGEMIT_Exit_In_Asm ()
 * ====================================================================
 */
void
CGEMIT_Exit_In_Asm (ST *pu) 
{
#if 0
  fprintf (Asm_File, "\t%s\t", AS_END);
  EMT_Write_Qualified_Name(Asm_File, pu);
  fprintf (Asm_File, "\n");
#else
  fprintf (Asm_File, "\t%s\t", AS_SIZE);
  EMT_Write_Qualified_Name(Asm_File, pu);
  fprintf (Asm_File, ", .-");
  EMT_Write_Qualified_Name(Asm_File, pu);
  fprintf (Asm_File, "\n");
#endif
}

/* ====================================================================
 *   CGEMIT_Prn_Scn_In_Asm
 * ====================================================================
 */
void
CGEMIT_Prn_Scn_In_Asm (
  FILE       *asm_file,
  const char *scn_name,
  Elf64_Word  scn_type,
  Elf64_Word  scn_flags,
  Elf64_Xword scn_entsize,
  Elf64_Word  scn_align,
  const char *cur_scn_name
)
{
  char scn_flags_string[5];
  char scn_type_string[10];  // min of strlen("progbits") + 1
  char *p = &scn_flags_string[0];

  fprintf (asm_file, "\n\t%s %s", AS_SECTION, scn_name);
  if (scn_flags & SHF_WRITE) *p++ = 'w';
  if (scn_flags & SHF_ALLOC) *p++ = 'a';
  if (scn_flags & SHF_EXECINSTR) *p++ = 'x';
  if (scn_flags & SHF_TLS) *p++ = 'T';
  // short sections are only recognized by name, not by "s" qualifier
  // if (scn_flags & SHF_IRIX_GPREL) *p++ = 's';
  *p = '\0'; // null terminate the string.
  fprintf (asm_file, ", \"%s\"", scn_flags_string);

  p = &scn_type_string[0];
  if (scn_type == SHT_NOBITS) {
#if 0
    strcpy(p, "nobits");
#else
    strcpy(p, "");
#endif
  }
  else if (scn_type == SHT_PROGBITS) {
#if 0
    strcpy(p, "progbits");
#else
    strcpy(p, "");
#endif
  }
#ifdef TARG_ST
  else if (scn_type == SHT_MIPS_DWARF) {
    strcpy(p, "");
  }
#endif
  else {
    DevWarn("Intel assembler definition inadequate for "
	    "ELF section type 0x%llx; using \"progbits\"", (UINT64)scn_type);
#if 0
    strcpy(p, "progbits");
#else
    strcpy(p, "");
#endif
  }
#ifndef TARG_ST
  fprintf (asm_file, ", \"%s\"\n", scn_type_string);
#else
  /* CLYON: change syntax for use with gas */
  if (*scn_type_string) /* Handle the case when the
				  type string is empty
				  (dwarf) */
    fprintf (asm_file, ", @%s", scn_type_string);
  fprintf (asm_file, "\n");
#endif /* TARG_ST */

    // Generate a label at the start of the .eh_frame CIE
    if (!strcmp (scn_name, ".eh_frame")) 
      fprintf (asm_file, "\n.EHCIE:");

#if 0   // contrary to document, it should be align, not power of it.
  UINT32 tmp, power;
  power = 0;
  for (tmp = scn__align; tmp > 1; tmp >>= 1) power++;
  fprintf (asm_file, "\t%s\t%d\n", AS_ALIGN, power);
#else
  fprintf (asm_file, "\t%s\t%d\n", AS_ALIGN, scn_align);  
#endif

}

/* ====================================================================
 *   CGEMIT_Prn_Scn_In_Asm
 * ====================================================================
 */
void
CGEMIT_Prn_Scn_In_Asm (
  ST *st, 
  Elf64_Word scn_type, 
  Elf64_Word scn_flags,
  Elf64_Xword scn_entsize, 
  ST *cur_section
)
{
  CGEMIT_Prn_Scn_In_Asm(Asm_File, ST_name(st), scn_type, scn_flags,
			scn_entsize, STB_align(st),
			cur_section != NULL ? ST_name(cur_section) : NULL);

}

/* ====================================================================
 *   CGEMIT_Prn_Line_Dir_In_Asm
 * ====================================================================
 */
void
CGEMIT_Prn_Line_Dir_In_Asm (
  USRCPOS usrcpos
)
{
  if (!List_Notes) return;
  if(!CG_emit_asm_dwarf) {
    fprintf (Asm_File, ASM_CMNT_LINE); //turn the rest into comment
  }
  
  fprintf (Asm_File, "\t.loc\t%d\t%d\t%d\n", 
		USRCPOS_filenum(usrcpos)-1,
		USRCPOS_linenum(usrcpos),
		USRCPOS_column(usrcpos));
}


/* ====================================================================
 *   CGEMIT_Prn_File_Dir_In_Asm
 * ====================================================================
 */
void
CGEMIT_Prn_File_Dir_In_Asm(
  USRCPOS usrcpos,
  const char *pathname,
  const char *filename
)
{
  if (!List_Notes) return;

  if(!CG_emit_asm_dwarf) {
    fprintf (Asm_File, ASM_CMNT_LINE); //turn the rest into comment
  }
  if (pathname == NULL) {
    fprintf (Asm_File, "\t%s\t\"%s\"\n", AS_FILE, 
	     filename);
  } else {
    fprintf (Asm_File, "\t%s\t\"%s/%s\"\n", AS_FILE, 
	     pathname,filename);
  }
}

/* ====================================================================
 *   CGEMIT_Prn_File_Dir_In_Asm
 *
 *   whether to use the base st for the reloc
 * ====================================================================
 */
extern BOOL
CGEMIT_Use_Base_ST_For_Reloc (
  INT reloc, 
  ST *st
)
{
  return ST_is_export_local(st);
}

/* ====================================================================
 *   CGEMIT_Asm_String_Prefix
 *
 *   Generate directives preceding an asm string
 * ====================================================================
 */
extern void
CGEMIT_Asm_String_Prefix (
			  OP *op,
			  INT32 *PC
			  )
{
}

/* ====================================================================
 *   CGEMIT_Asm_String_Suffix
 *
 *   Generate directives following an asm string
 * ====================================================================
 */
extern void
CGEMIT_Asm_String_Suffix (
			  OP *op,
			  INT32 *PC
			  )
{
}

/* ====================================================================
 *   CGEMIT_Begin_File_In_Asm
 *
 *   Outputs target dependent informations at begining of file.
 * ====================================================================
 */
void 
CGEMIT_Begin_File_In_Asm (void)
{
#if 0
  fprintf (Asm_File, "\t%s\t%s\n", AS_ASSUME, Targ_Name(Target));
  fprintf (Asm_File, "\t%s\t%s\n", AS_ASSUME, Abi_Name (Target_ABI));
#else
  char *target_name = "unknown";
  switch(Target) {
  case TARGET_armv5:
    target_name = "arm926ej-s";
    break;
  case TARGET_armv6:
    target_name = "arm1176jz-s";
    break;
  default:
    target_name = "unknown";
  }
  fprintf (Asm_File, "\t%s\t%s\n", AS_CPU, target_name);
  fprintf (Asm_File, "\t%s\t%s\n", AS_FPU, "softvfp");
#endif
}

/* ====================================================================
 *   CGEMIT_End_File_In_Asm
 *
 *   Outputs target dependent informations at end of file.
 * ====================================================================
 */
void
CGEMIT_End_File_In_Asm (void)
{
  
}

#ifdef TARG_ST
/* ====================================================================
 *   CGEMIT_Qualified_Name
 *
 * Returns into buf the qualified name for a symbol to be used in
 * the asm output. 
 * Moved from r_qualified_name in cgemit.cxx
 * ====================================================================
 */
void 
CGEMIT_Qualified_Name(ST *st, vstring *buf)
{
  if (ST_name(st) && *(ST_name(st)) != '\0') {
    *buf = vstr_concat(*buf, ST_name(st));
    if (ST_is_export_local(st) && ST_class(st) == CLASS_VAR) {
      // local var, but being written out.
      // so add suffix to help .s file distinguish names.
      // assume that statics in mult. pu's will 
      // get moved to global symtab, so don't need pu-num
      if (ST_level(st) == GLOBAL_SYMTAB) {
#ifdef TARG_ST
	// [CG] If not under -ipa mode we do not need to rename static
	// in the global symtab.
	// [CG] Force use of  '.' to ensure no name clashing
	if (Emit_Global_Data || Read_Global_Data)
	  vstr_sprintf (buf, vstr_len(*buf), 
			"%s%d", ".", ST_index(st));
#else
	vstr_sprintf (buf, vstr_len(*buf), 
		      "%s%d", Label_Name_Separator, ST_index(st));
#endif
      } else {
#ifdef TARG_ST
	// [CG] Force use of  '.' to ensure no name clashing
	vstr_sprintf (buf, vstr_len(*buf), 
		      "%s%d%s%d", ".", ST_pu(Get_Current_PU_ST()),
		      ".", ST_index(st) );
#else
	vstr_sprintf (buf, vstr_len(*buf), 
		      "%s%d%s%d", Label_Name_Separator, ST_pu(Get_Current_PU_ST()),
		      Label_Name_Separator, ST_index(st) );
#endif
      }
    }
    else if (*Symbol_Name_Suffix != '\0') {
      *buf = vstr_concat(*buf, Symbol_Name_Suffix);
    }
  } else {
    vstr_sprintf (buf, vstr_len(*buf), 
		  "%s %+lld", ST_name(ST_base(st)), ST_ofst(st));
  }
}
#endif

/** 
 * returns whether END_GROUP (;;) must be preceeded by a \n
 * 
 * 
 * @return 
 */
extern bool
CGEMIT_NewLine_Before_ISA_PRINT_END_GROUP (void)
{
  return false;
}
