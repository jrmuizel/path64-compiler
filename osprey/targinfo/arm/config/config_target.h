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

#ifndef config_targ_INCLUDED
#define config_targ_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * ====================================================================
 *
 * Module: config_targ.h
 *
 * Revision history:
 *  15-Jun-91 - Original Version
 *  25-Sep-91 - Added subprogram interface pointers.
 *  27-Nov-91 - Reconfiguration of target representation for TP.
 *  21-Apr-93 - Set TARG_NEEDS_QUAD_OP to 1
 *  14-Mar-97 - Removed most -TARG group flags to config_TARG.h.
 *
 * Description:
 *
 * This file defines general configuration parameters which are
 * specific to the compiler's target machine and system.
 * There is an approximate distinction between -TARG option group
 * flags and their configuration (in config_TARG.[hc]), and more
 * generic target configuration (in this file).  Note that this file
 * is included in config.h, and hence indirectly in most source files,
 * whereas config_TARG.h is only included directly, so putting new
 * -TARG option-related variables in config_TARG.[hc] is to be
 * preferred to putting them here.
 *
 * See com/config.h for a detailed description of configuration
 * parameters, where they are declared, and how they are processed.
 *
 * See also:
 *	TARGET/com/targ_const.h -- Manipulation of target constants, as
 *		appropriate on the host system.
 *
 * ====================================================================
 * ====================================================================
 */

  /* #include "mtypes.h" */

/* ====================================================================
 *
 * Target selection
 *
 * Note that the current target is also given by a #define name of the
 * form TARG_xxx.  However, since a single compiler may support several
 * target variants, this symbol (defined in the TDT) will generally be
 * less specific than the variable Target below.
 *
 * ====================================================================
 */

/* ========== */
/* Target ABI */
/* ========== */

typedef enum {
  ABI_UNDEF,	/* Undefined */
  ABI_ARM_ver1, /* EABI version 1 */
  ABI_ARM_ver2, /* EABI version 2 */
} TARGET_ABI;

BE_EXPORTED extern TARGET_ABI Target_ABI;

/* return the target name for ABI */
BE_EXPORTED extern char *Abi_Name (TARGET_ABI abi);

/* [SC] TRUE if a protected symbol may be preempted at link or
 * load time.  For example the .dynbss hack causes a protected
 * data symbol to be preempted by a data symbol in the main
 * program.  Similarly, the address of a protected function may
 * be changed to be the address of a PLT entry in the main program.
 * Both these can happen on ST200 in the callee-sets-gp ABI.
 */
BE_EXPORTED extern BOOL Target_ABI_Preempts_Protected_Symbols();

/* ================ */
/* Target processor */
/* ================ */

/* Define an enum representing all current target processors.
 * For variants and modifiers of these, see tp/com/config_targ.h.
 */
typedef enum {
  TARGET_UNDEF,		/* Undefined */
  TARGET_armv5,	
  TARGET_armv6,	
} TARGET_PROCESSOR;

BE_EXPORTED extern TARGET_PROCESSOR Target;		/* -Tc */

/* return the target name for <target> */
BE_EXPORTED extern char *Targ_Name (TARGET_PROCESSOR target);

#define Is_Target_armv5()	(Target==TARGET_armv5)
#define Is_Target_armv6()	(Target==TARGET_armv6)

/* ========== */
/* Target ISA */
/* ========== */

typedef enum {
  TARGET_ISA_UNDEF,	/* Undefined */
  TARGET_ISA_armv5,
  TARGET_ISA_armv6
} TARGET_ISA;

BE_EXPORTED extern TARGET_ISA Target_ISA;	/* -Tc.Rc */

/* return the ISA name corresponding to <target_isa> */
BE_EXPORTED extern char *Isa_Name (TARGET_ISA target_isa);

#define Is_Target_ISA_armv5() (Target_ISA==TARGET_ISA_armv5)
#define Is_Target_ISA_armv6() (Target_ISA==TARGET_ISA_armv6)

/* What is the floating point format? */
#define IEEE_FP_FORMAT	TRUE
#define IBM_FP_FORMAT	FALSE

/* Do we need to implement quad float? */
/* #define TARG_NEEDS_QUAD_OPS */

/* On every target, there is an upper bound on the size pf data object
 * that can be defined.  The following two definitions define this
 * upper bound for 32 and 64 bits mode compilations respectively.
 */
#define MAX_64BITS_OBJECT_SIZE 0x000000007fffffffULL /* 31 bits */
#define MAX_32BITS_OBJECT_SIZE 0x000000007fffffffULL /* 31 bits */

/* On most targets, it will be necessary to decide whether a text
 * address is usable in an instruction literal field.  The following
 * specifies the maximum value which we may assume a function or label
 * address will take on.
 *
 * NOTE:  On the MIPS, jumps are relative to the current 256MB segment,
 * and we always assume that we can reach anything we want to jump/call
 * to, since the linker will insert stubs if necessary.  So we treat
 * the limit as 256MB here.  We make it smaller than necessary to allow
 * for some arithmetic without overflowing the limits.
 */
#define MAX_LABEL_SYMVAL	0x03fffff0
#define MAX_FUNC_SYMVAL		0x03fffff0

/* maximum gspace (gp-relative) size */
#define DEFAULT_GSPACE		0x3fffff

/* The ST200 can handle arbitrary sized short data sections, so place
 * as much as possible in them.  Set these to their maximum values.
 */
#define MAX_SDATA_ELT_SIZE	((INT32)0x7fffffff)
#define DEF_SDATA_ELT_SIZE      ((INT32)0x7fffffff)

/* On ST200, the short rodata section is .srdata, the normal rodata section is
   .rodata.
   In PIC ABI, .srdata can be accessed GP-relative but cannot be shared,
   .rodata cannot be accessed GP-relative but can be shared.
   So in PIC ABI, to take advantage of the faster access for .srdata, but the
   space-saving of .rodata, we should only place small objects in .srdata.
   For embedded ABI, there is no problem: we can put everything in .rodata
   and access it GP-relative.
 */
#define DEF_SRDATA_ELT_SIZE     ((INT32)8)

/* How big is a large object? */
#define DEFAULT_LARGE_OBJECT_BYTES	64

/*
 * fixed part of the stack offset adjustment
 */
#define STACK_OFFSET_ADJUSTMENT         0

/* 
 * largest signed offset possible in small-frame stack model:
 */
#define MAX_SMALL_FRAME_OFFSET  0x7ff    // 8 bits addressing

/*
 * Since the largest offset allowed is 9 bits
 */
#define DEFAULT_TEMP_SPACE_BYTES        256

/*
 * Default, maximal area that may be needed to home formal arguments
 * passed in registers 
 */
#ifndef TARG_ST
  /* Moved to targ_sim.h under name Default_Formal_Save_Area_Size */
#define DEFAULT_FORMAL_SAVE_AREA_SIZE   24      // 24 bytes
#endif

#define MAX_SFSEG_BYTES		0x7FFFFFFFL     // 32 bits

#define MAX_LARGE_FRAME_OFFSET	0x7FFFFFFFL	// 32 bits

/* 
 * Alignment is target dependent and should be specified as such on
 * each target for each MTYPE. This would require changes to the
 * compiler including FE changes. For now, I'll just use the
 * strongest alignment.
 */
#define TARG_NONE_ALIGN  0
#define TARG_BYTE_ALIGN  1
#define TARG_CARD_ALIGN  2
#define TARG_WORD_ALIGN  4
#define TARG_DWORD_ALIGN  8
#define TARG_QUAD_ALIGN  16

/*
 * Alignment used for aligning the stack segments
 */
#define DEFAULT_STACK_ALIGNMENT PU_aligned_stack(Get_Current_PU())
BE_EXPORTED extern INT32 Target_Stack_Alignment; /* -TARG:stack_alignment=nn */

/*
 * [CG] Alignment for the code segments.
 * This should be the more effective alignment for the code sections
 * given the bundling constraints of the code.
 */
#define DEFAULT_TEXT_ALIGNMENT 2

/*
 * [CL] Alignment for function entry points.
 * As alignment constraints of the code only
 * handle two sections per function, we must ensure
 * that each first contribution of a function to
 * a section is properly aligned.
 */
#define DEFAULT_FUNCTION_ALIGNMENT 2

/*
 * [CG] Alignment for the data segment
 * Should be the minimal alignment for ensuring correct access
 * to all data types on the target.
 * On ST200 the abi specify that for cache efficiency
 * the alignment should be 32. More than this it is
 * stated that we must ensure provision for double word accesses
 * and interleaved memory. Thus the alignement is 32.
 */
/*
 * [clarkes] 030515: The ST200 ABI requirement for data alignment
 * does not apply to the whole section, just to objects within the
 * section.  A section is already given the alignment of the most
 * strictly aligned object within the section, so
 * there is no additional alignment required here.
 * This is specified by setting DEFAULT_DATA_ALIGNMENT to zero.
 */
#define DEFAULT_DATA_ALIGNMENT 0

/*
 * [CG] Alignment for optimized instructions block alignment.
 * This should be the size of the instruction cache line. Or the minimal
 * size of instruction cache line for multiple cache conficurations.
 */
#define OPTIMIZED_INSTRUCTIONS_ALIGNMENT 0

/* ====================================================================
 *
 * Miscellaneous options
 *
 * ====================================================================
 */

/* Do we need to force NOOPs in empty cycles? */
#define FORCE_NOOPS	FALSE

/*================================================================*/
/* Architecture specific options. Variables are set in config_targ.cxx,
   #defines are set in config_targ.h
*/

BE_EXPORTED extern BOOL ARCH_mask_shift_counts;
BE_EXPORTED extern BOOL ARCH_generate_nor;


BE_EXPORTED extern BOOL INT_packed_arithmetic;
BE_EXPORTED extern BOOL PTR_packed_arithmetic;
BE_EXPORTED extern BOOL BOOL_packed_arithmetic;
BE_EXPORTED extern BOOL FP_packed_arithmetic;

// don't try to canonalize 1/a. On st200 recip is implemented with a divide.
// [HK] 20050906 1/a now available as an intrinsic function
// #define ARCH_recip_is_exact FALSE
#define ARCH_recip_is_exact TRUE
#define ARCH_has_bit_tests TRUE

// [HK] 20051025 1/sqrt(a) now available as an intrinsic function
#define ARCH_rsqrt_is_exact TRUE

/* default value for WHIRL_Keep_Cvt */
#define DEFAULT_KEEP_CVT	TRUE

/* ====================================================================
 * Target Specific Miscellany Declarations which include target 
 * dependencies.
 * ====================================================================
 */
#ifdef TARG_ST

  //[TB] Support for asm stmt with dynamic mtype
/* ====================================================================
 *
 * Check_Asm_Constraints
 *
 * Check that asm constraints are compatible with the given MTYPE
 *
 * ====================================================================
 */
BE_EXPORTED extern BOOL Check_Asm_Constraints(char* constraint_string, UINT8 mtype);

// [TTh] Maximum number of arguments for intrinsic ops
// (Added to fix codex bug #43451)
// On xp70, extension builtins can have many parameters as SFR accesses
// are explicitly defined as parameters:
// Theoretical MAX = 32 SFRs + 4 ext registers + 3 GPRs
#define INTRN_MAX_ARG 40

#endif //TARG_ST

#ifdef __cplusplus
}
#endif
#endif /* config_targ_INCLUDED */
