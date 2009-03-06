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


#ifndef config_asm_INCLUDED
#define config_asm_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * ====================================================================
 *
 * Module: config_asm.h
 *
 * Revision history:
 *  17-Jun-91 - Original Version
 *  26-Sep-91 - Modified AS_CMNT_START, AS_IDENT
 *
 * Description:
 *
 * This file contains #defines which configure assembly code emission
 * to the current host assembler.  The intent is to isolate all
 * dependencies on the host's assembly language format and directives
 * here.
 *
 * ====================================================================
 * ====================================================================
 */


#ifdef _KEEP_RCS_ID
#endif /* _KEEP_RCS_ID */

#define Label_Name_Separator "_"

/* to distinguish from register names: */
#define Symbol_Name_Suffix "\0"

/* [CG] Prefix definition for all compiler generated local labels. 
   They should be discarded for example by a strip -X or the debugger. */
#define Local_Label_Prefix "L"

/* [CG] The following sprintf format is used to create a label
 * from a (name, scope level, label index) tuple. Used by the code selector.
 */
#define BB_Label_Name_Scope_Index Local_Label_Prefix "_%s_%d_%d"

/* The following sprintf format is used to create a basic block label
 * from the BB id and the PU name:
 */
#define BB_Label_Format		Local_Label_Prefix "_BB%d_%s"

#ifdef SPLIT_BCO_ENABLED
/* The following sprintf format is used to create a split label
 * from the split point id and the PU name:
 */
#define BB_Split_Label_Format		"%s$%d$split"

/* The following sprintf format is used to create a label to
 * significate end if a splitable region from the split point id and
 * the PU name:
 */
#define BB_EndSplit_Label_Format		"%s$%d$endsplit"
#endif

/* The following sprintf format is used to create a label
 * for a source line, used to emit debug info
 */
#define Source_Line_Label_Format		Local_Label_Prefix "_Debug%d_%s"

/* The following sprintf format is used to create a basic block label
 * from a user label name and the PU name:
 */
#define User_Label_Format	Local_Label_Prefix "_%s_%s"
#define User_Label_Number_Format Local_Label_Prefix "_%d_%d_%s"

/* The following sprintf format is used to create the magic label for
 * the common return point (if Only_1_Exit_Point is TRUE).
 */
#define Ret_Block_Label_Format	Local_Label_Prefix "_RET_%s"

/* The following sprintf format to ensure correct alignment of
 * entry points:
 */
#define Entry_Point_Alignment_Format	"\t.align\t4\n"

/* The following sprintf formats define the mapping of objects in
 * various storage classes to the assembly code and symbol table
 * (naming) conventions of the target:
 *	Fortran:
 *		INTEGER X, Y
 *		COMMON /CBLOCK/ X, Y
 *		    =>	loadd	_cblock$,f0		-- load X
 *			loadd	_cblock$+8,f1		-- load Y
 *
 *		CALL SUB()
 *		    =>	call	sp,_sub_
 *
 * The Intrnisic_Proc_Format describes a modification to intrinsic
 * procedure names, allowing an easy way of describing intrinsic
 * naming conventions.  Our convention: Intrinsic functions have
 * an underscore, '_', prepended to their names, so this format is
 * "_%s".  Note that the processing by Intrinsic_Proc_Format is done
 * at compiler-build time (by the program "mkintr.x").
 *
 * Note also that the Ftn_Extern_Proc_Format is *not* applied to
 * intrinsic functions, even though they are external procedures.
 *
 * There are some language-specific things here.  For example, the
 * naming convention for Fortran functions is not the same as the
 * naming convention for C functions.  Consequently, there are two
 * formats for external procedure names:
 *	Ftn_Extern_Proc_Format
 *	C_Extern_Proc_Format
 */
#define Intrinsic_Proc_Format	"%s"	   /* eg: "_exp"              */
#define Ftn_Extern_Data_Format	"%s_"	   /* eg: "cblock_"          */
#define C_Extern_Data_Format	"%s"	   /* eg: "extrn"	      */
#define Ftn_Extern_Proc_Format	"%s_"	   /* eg: "sub_"             */
#define C_Extern_Proc_Format	"%s"	   /* eg: "sub"              */
#define C_Global_Static_Format	"GS.%s"	   /* statics in global scope */
#define C_Local_Static_Format	"LS.%1d.%s"/* statics in local scope  */
#define Deferred_Ref_Format	"%s"	   /* eg: "name"              */
#define Deferred_Ref_Ofst_Format "%s%c%d"  /* eg: "name" '-' offset   */
#define Local_Ret_Buf_Format	"%s.RBUF"  /* eg: "func.RBUF"         */

#define DFLT_FTN_MAIN		"MAIN"
#define DFLT_FTN_LOWMAIN	"main"

/*
 * Predicate format -- no predication on this target
 */
#define True_Predicate_Format   "error"
#define False_Predicate_Format  "error"

#define ASM_CMNT_LINE		"##"  /* use for whole line comments */
  /* On ARM, need to distinguish '#' on immediates and the end line comment after ';' */
  /* Example: 	str lr, [sp, #0]              	;## lcl_spill_temp_0                  */
#define ASM_CMNT		"; ##"  /* other than whole line comments */

#define AS_STOP_BIT		";;"

/* Defines for assembly directives */

/* alignment directives */
#define AS_ALIGN 	".align"

/* section directives */


/* data directives */
#define AS_COM		".comm"
#define AS_DATA		".data"

#define AS_BYTE		".byte"
#define AS_HALF		".hword"
#define AS_HALF_UNALIGNED ASM_CMNT " .hword.unaligned"
#define AS_WORD		".word"
#define AS_WORD_UNALIGNED ".word" 
#define AS_DWORD	".quad"
#define AS_DWORD_UNALIGNED ASM_CMNT " .data8.ua"
#define AS_FLOAT 	".real4"
#define AS_DOUBLE 	".real8"

#define AS_SLEB128      ".sleb128"
#define AS_ULEB128      ".uleb128"

#define AS_DYNSYM	(char *)NULL
#define AS_END		".endp"
#define AS_ENT		".proc"
#define AS_AENT		ASM_CMNT " .aproc"
#define AS_FILE		".file"
#define AS_FMASK	".fmask"
#define AS_FRAME	".fframe"
#define AS_GLOBAL 	".global"
#define AS_INTERNAL     ".internal"
#define AS_HIDDEN       ".hidden"
#define AS_PROTECTED    ".protected"
#define AS_GPVALUE	".gpvalue"

#define AS_BSS		".lcomm"
#define AS_MASK		"//.mask"
#define AS_ORIGIN	".org"
#define AS_RDATA	".rdata"
#define AS_SDATA	".sdata"
#define AS_SECTION	".section"
#define AS_SIZE         ".size"
#define AS_SPACE	".space"
#define AS_ASCII	".ascii"
#define AS_STRING	".string"
#define AS_TEXT 	".text"
#define AS_TYPE         ".type"
#define AS_TYPE_FUNC	"%function"
#define AS_TYPE_OBJECT	"%object"
#define AS_WEAK		".weak"
#ifndef TARG_ARM
#define AS_MOVEABLE     "moveable"
#else
#undef AS_MOVEABLE
#endif
#define AS_USED     	"used"

#define AS_IDENT	".ident"
#define AS_COMMENT	".comment"
#define AS_ASSUME       ".assume"

#define AS_CPU          ".cpu"
#define AS_FPU          ".fpu"

#define AS_ALLOW_NOPS   ".nopinsertion"
#define AS_DISALLOW_NOPS ".nonopinsertion"

#define AS_DWARF_THREAD_DATA_RELOC "@dtprel"

/* The directive for emitting an address depends on the target pointer
 * size.  The following is defined and initialized in config_targ.c:
 */
BE_EXPORTED extern char *AS_ADDRESS;
BE_EXPORTED extern char *AS_ADDRESS_UNALIGNED;

/* Defines for emission of special relocations */
#define AS_GPREL	"@gprel"
#define AS_LTOFF	"@gotoff"
#define AS_FPTR		((Is_Caller_Save_GP \
                        && (Gen_PIC_Shared || Gen_PIC_Call_Shared)) ? "@fptr" : 0)
#define AS_GP_REL	(char *)NULL
#define AS_NEGATE	(char *)NULL

/* A printf format for emitting a function of an external name, where
 * the function is typically given by one of the above kind symbols:
 */
#define ASM_NAME_FUNC(kind,name)	"%s(%s)", kind, name
#define ASM_DIR_NOTRANSFORM()           ; /* no equivalent */
#define ASM_DIR_TRANSFORM()             ; /* no equivalent */
#define ASM_DIR_NOREORDER()             ; /* no equivalent */
#define ASM_DIR_NOAT()                  ; /* no equivalent */
#define ASM_DIR_STOINTERNAL(n)          ; /* no equivalent */
#define ASM_DIR_GPVALUE()         	fprintf(Asm_File, "//\t%s %d\n", \
						AS_GPVALUE, GP_DISP);
#define ASM_DIR_FILE(count, file, name) fprintf (Asm_File, \
						 "\t%s\t\"%s/%s\"\n", \
						 AS_FILE, \
						 incl_table[DST_FILE_NAME_dir(file)], name);
#define ASM_DIR_ALIGN(p, s)             fprintf(Asm_File, "\t%s %d\n", AS_ALIGN,STB_align(s));

/* gas .skip fills with zeroes */
#define ASM_DIR_ZERO(fl, l)	fprintf(fl, "\t%s %d\n", AS_SPACE, l) 
#define ASM_DIR_SKIP(fl, l) 	fprintf(fl, "\t%s %d\n", AS_SPACE, l)

#ifdef __cplusplus
}
#endif
#endif /* config_asm_INCLUDED */
