/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

   Copyright (C) STMicroelectronics All Rights Reserved.

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
#include "mtypes.h" 

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

#define Is_Target_x86_64()	(FALSE)
#define Is_Target_SSE()        (FALSE)
#define Is_Target_SSE2()       (FALSE)
#define Is_Target_SSE3()       (FALSE)
#define Is_Target_3DNow()      (FALSE)
#define Is_Target_SSE4a()      (FALSE)
#define Is_Target_Pentium4()   (FALSE)
#define Is_Target_EM64T()	(FALSE)
#define Is_Target_Core()	(FALSE)
#define Is_Target_Wolfdale()	(FALSE)
#define Is_Target_Anyx86()     (FALSE)
#define Target_x87_precision()	(FALSE)
#define Is_Target_Barcelona()  (FALSE)
#define Is_Target_Sandy_Bridge() (FALSE)
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
#define Is_Target_64bit() FALSE

/* We need to know the machine type of the integer and floating point
 * registers for purposes of subprogram save/restore:
 */
extern CLASS_INDEX Spill_Ptr_Mtype;
extern CLASS_INDEX Spill_Int_Mtype;
extern CLASS_INDEX Spill_Float_Mtype;
extern CLASS_INDEX Spill_Int32_Mtype;
extern CLASS_INDEX Spill_Float32_Mtype;

/* We need to know the maximum machine type corresponding directly to
 * the machine registers:
 */
extern CLASS_INDEX Max_Int_Mtype, Max_Uint_Mtype;

/* We need to know the default machine type to be used for internally
 * generated integers, e.g. DO loop indices.  These will default to the
 * max int types above, but the FEs may override the defaults:
 */
extern CLASS_INDEX Def_Int_Mtype, Def_Uint_Mtype;
extern BOOL Use_32_Bit_Pointers;	/* -Tc.P32 */
/* A 64-bit target always use ELF64, but sometimes we
 * might want a 32-bit target to use ELF64 instead of
 * ELF32.
 *
 * More precisely, Use_32_Bit_Pointers is true whenever the
 * size of an item pointed to by a pointer is 32 bits. In
 * some cases, ELF64 might be useful even for this kind of target.
 */
BE_EXPORTED extern BOOL  Use_ELF_32;

/* For various targets, what are the characteristics of pointers */
extern INT		Pointer_Size;	/* in bytes */
extern CLASS_INDEX	Pointer_Mtype;
extern CLASS_INDEX	Pointer_Mtype2;

/* What are pointers and booleans aliased to in WHIRL */
extern TYPE_ID Pointer_type;
extern TYPE_ID Pointer_type2;
extern TYPE_ID Boolean_type;
extern TYPE_ID Boolean_type2;
extern TYPE_ID Integer_type;

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

/* maximum sda/da sizes (for automatic mem placement)*/
#define DEFAULT_SDA_SPACE	0x0
#define DEFAULT_DA_SPACE	0x0

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
/* ================== */
/* Comparison Results */
/* ================== */

/* For various targets, what is the comparison result type? */
extern INT		Comparison_Result_Size;	/* in bytes */
extern CLASS_INDEX	Comparison_Result_Mtype;

/* ============= */
/* Miscellaneous */
/* ============= */

/* Miscellaneous exception control */
#define FPX_I	0x01	/* IEEE-754 inexact exception */
#define FPX_U	0x02	/* IEEE-754 underflow exception */
#define FPX_O	0x04	/* IEEE-754 overflow exception */
#define FPX_Z	0x08	/* IEEE-754 zero divide exception */
#define FPX_V	0x10	/* IEEE-754 invalid operation exception */
#define FPX_ALL	0x1f	/* All the IEEE-754 exceptions */
#define EXC_ALL	0x1f	/* All the exceptions */
extern INT16 FP_Exception_Enable_Max;	/* Max FP trap enables */
extern INT16 FP_Exception_Enable_Min;	/* Min FP trap enables */

/* Is the "char" type signed? */
extern BOOL Char_Type_Is_Signed;


/* ====================================================================
 *
 * Target debugging options
 *
 * ====================================================================
 */

/* Symbolic Debug mode as specified on command line.  (The mode can
 * change from PU to PU because, for example, we encounter a call to
 * the routine 'alloca()' -- we must restore the mode to the value as it
 * was specified on the command line, so we remember it.)
 */
#define SDM_NONE	0x00	/* No information */
#define SDM_LINE	0x01	/* Line numbers */
#define SDM_SYMBOL	0x02	/* Symbol table information */
#define SDM_SEQLINE	0x04	/* Force sequentail line numbers */
#define SDM_GEN_FP	0x08	/* Save/restore of fp for sdb */
#define SDM_USE_FP	0x10	/* Use fp for sdb definitions */

extern INT16 Symbolic_Debug_Mode;	/* Current */
extern INT16 Max_Symbolic_Debug_Mode;	/* Maximum for any PU */

#define Want_Line_Debugging	(Symbolic_Debug_Mode & SDM_LINE)
#define Want_Sequential_Line_Debugging	(Symbolic_Debug_Mode & SDM_SEQLINE)
#define Want_Symbol_Debugging	(Symbolic_Debug_Mode & SDM_SYMBOL)
#define Want_Any_Debugging	(Symbolic_Debug_Mode &(SDM_LINE|SDM_SEQLINE|SDM_SYMBOL))
#define Gen_Frame_Ptr		(Symbolic_Debug_Mode & SDM_GEN_FP)
#define Set_Gen_Frame_Ptr	(Max_Symbolic_Debug_Mode |= SDM_GEN_FP, \
				  Symbolic_Debug_Mode |= SDM_GEN_FP)
#define Use_Frame_Ptr_For_Sdb	(Symbolic_Debug_Mode & SDM_USE_FP)
#define Set_Use_Frame_Ptr_For_Sdb (Max_Symbolic_Debug_Mode |= SDM_USE_FP, \
				   Symbolic_Debug_Mode |= SDM_USE_FP)

/* I believe the frame pointer flags function as follows:
 *
 * The debugger may require that a frame pointer be generated and
 * available (Gen_Frame_Ptr), and/or that it be used in the external
 * symbol table definitions of variables' addressing emitted in
 * coffsdb.c (Use_Frame_Ptr_For_Sdb).  We currently treat these
 * independently, though perhaps Use_Frame_Ptr_For_Sdb should imply
 * Gen_Frame_Ptr.
 *
 * In addition, memmodel.[hc] contains variables Gen_Frame_Pointer
 * and Use_Frame_Pointer which actually control whether code is
 * generated to produce the FP on PU entry and use it for referencing
 * part of the stack frame, respectively.  Gen_Frame_Pointer is
 * initialized to Gen_Frame_Ptr, but may also be turned on by the
 * Use_Frame_Pointer requirement.  Use_Frame_Pointer is currently
 * set only if the stack frame is large enough that we want both
 * pointers to improve addressibility; it will also probably be set
 * if we need to deal with dynamic local data allocation.  It is
 * therefore completely independent of Use_Frame_Ptr_For_Sdb.
 */

/* ====================================================================
 *
 * Miscellaneous options
 *
 * ====================================================================
 */

/* Do we need to force NOOPs in empty cycles? */
#define FORCE_NOOPS	FALSE

/* Can 64-bit values be 4-byte aligned in memory? */
extern BOOL Allow_Word_Aligned_Doubles;

/* Should we generate position-independent code by default? */
extern BOOL Generate_Position_Independent_Code;

/* Split 64-bit integer ops into 32-bit ops, and simulate them? */
extern BOOL Split_64_Bit_Int_Ops;

/* Split quad-precision ops into double-precision, and simulate them? */
extern BOOL Split_Quad_Ops;

/* Should we simulate 32-bit subprogram interface by default? */
extern BOOL Simulate_32_Bit_Interface;

/* Workaround for TFP branch cache problem. */
extern BOOL No_Quad_Aligned_Branch;
extern BOOL No_Quad_Aligned_Branch;

/* Does target provides only unsigned 64-bit instructions? */
extern BOOL Only_Unsigned_64_Bit_Ops;

extern INT32 Align_Instructions;	/* Align frequent blocks and funcs */

extern BOOL Eager_Bottom_Load;
extern BOOL Eager_Ptr_Deref;
extern BOOL Eager_Null_Ptr_Deref;

/* file has non-standard gp groups, i.e. SYMTAB_gp_group(Global_Symtab) */
extern BOOL Has_GP_Groups;

/* Does target have offsets in load and store instructions?
 * Note: CG should instead test:
 * ( TOP_Find_Operand_Use( OP_code(op), OU_offset ) >= 0 ) */
extern BOOL Use_Load_Store_Offset;


/* ====================================================================
 *
 * Initialization interface
 *
 * ====================================================================
 */

/* Configuration prior to flag processing: */
extern void Preconfigure_Target (void);

/* Configuration after flag processing: */
extern void Configure_Target (void);
extern void IPA_Configure_Target (void);

/* Reconfiguration for each source file: */
extern void Configure_Source_Target ( char *filename );

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

/* ====================================================================
 *
 * Get_Extension_MTYPE_From_Asm_Constraints
 *
 * Return the extension MTYPE used in <constraint_string>, if any.
 * Otherwise return MTYPE_UNKNOWN.
 *
 * ====================================================================
 */
BE_EXPORTED extern UINT8 Get_Extension_MTYPE_From_Asm_Constraints(const char *constraint_string);

// [TTh] Maximum number of arguments for intrinsic ops
// (Added to fix codex bug #43451)
// On xp70, extension builtins can have many parameters as SFR accesses
// are explicitly defined as parameters:
// Theoretical MAX = 32 SFRs + 4 ext registers + 3 GPRs
#define INTRN_MAX_ARG 40

#endif //TARG_ST

/* ====================================================================
 *
 * Initialization interface
 *
 * ====================================================================
 */

/* Configuration prior to flag processing: */
//extern void Preconfigure_Target (void);

/* Configuration after flag processing: */
//extern void Configure_Target (void);
//extern void IPA_Configure_Target (void);

/* Reconfiguration for each source file: */
//extern void Configure_Source_Target ( char *filename );

/* return FALSE if abi mismatch */
extern BOOL Set_Target_ABI (BOOL is_64bit, INT isa);



#ifdef __cplusplus
}
#endif
#endif /* config_targ_INCLUDED */
