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


/* ====================================================================
 * ====================================================================
 *
 * Module: config_targ.c
 *
 *
 * Description:
 *
 * Configuration specific to the target machine/system.
 *
 * NOTE:  There is an approximate distinction between -TARG option
 * group flags and their configuration (in config_TARG.c), and more
 * generic target configuration (in this file).  Note that the related
 * header file config_targ.h is included in config.h, and hence in most
 * source files, whereas config_TARG.h is only included directly, so
 * putting new -TARG option-related variables in config_TARG.c is to
 * be preferred to putting them here.
 *
 * ====================================================================
 * ====================================================================
 */
#include "defs.h"
#include "config.h"
#include "config_asm.h"
#include "config_debug.h"
#include "config_TARG.h"
#include "config_opt.h"
#include "config_wopt.h"   /* what WOPT does can be target dependent */
#include "config_lno.h"
#include "config_ipa.h"    /* IPA_PU_Minimum_Size */
#include "erglob.h"
#include "tracing.h"
#include "mtypes.h"
#include "stab.h"
#include "targ_sim.h"
/* #include "ti_init.h" */

#if defined(FRONT_END_C) || defined(FRONT_END_CPLUSPLUS)
typedef unsigned char an_integer_kind;
#include "targ_abi_properties.h" // TB: For ABI_PROPERTIES_ABI_Value
#endif

/* =====================================================================
 *    Architecture specific definitions: 
 * =====================================================================
 */
BOOL ARCH_generate_nor = FALSE;       // Generate the NOR opcode
BOOL ARCH_mask_shift_counts = FALSE;  // shift counts are masked by the hardware (vs. truncated)

/* Needed todefine whether the packed arithmetic is allowed: */
BOOL INT_packed_arithmetic = FALSE;
BOOL FP_packed_arithmetic = FALSE;
BOOL PTR_packed_arithmetic = FALSE;
BOOL BOOL_packed_arithmetic = FALSE;

/* =====================================================================
 *    Target selection 
 * =====================================================================
 */
TARGET_ABI Target_ABI = ABI_UNDEF;
TARGET_PROCESSOR Target = TARGET_UNDEF;		/* -Tc */
TARGET_ISA  Target_ISA  = TARGET_ISA_UNDEF;	/* -Tc.Ic */

/* The assembler directive for emitting an address depends on the target
 * pointer size.  The following is declared in config_asm.h:
 */
char *AS_ADDRESS;
char *AS_ADDRESS_UNALIGNED;

/* Is the "char" type signed? */
BOOL Char_Type_Is_Signed = FALSE;

/* =====================================================================
 *    Miscellaneous exception control 
 * =====================================================================
 */
#define FPX_DEF EXC_ALL	                     /* Default enables */
INT16 FP_Exception_Enable_Max = 0      ;     /* Max FP trap enables */
INT16 FP_Exception_Enable_Min = 0;	     /* Min FP trap enables */

INT32 Align_Instructions = 0;	             /* 0 means hasn't been set, 1 cache line size, other is a power of 2. */
#ifdef TARG_ST
INT32 Align_Functions = 0;	             /* same as above. */
INT32 Align_Loops = 0;	             /* same as above. */
INT32 Align_Jumps = 0;	             /* same as above. */
INT32 Align_Labels = 0;	             /* same as above. */
#endif

BOOL Avoid_TFP_blikely_bug = FALSE;
BOOL Avoid_TFP_blikely_bug_overridden = FALSE;

// [HK] 20051209 this setting depens now on Finite_Math (treated in config_opt.cxx)
/***** IEEE 754 options *****/
// BOOL Force_IEEE_Comparisons = TRUE;    /* IEEE NaN comparisons? */
// BOOL Force_IEEE_Comparisons = !(Finite_Math);    /* IEEE NaN comparisons? */

/***** INTERNAL group options *****/

BOOL WHIRL_Return_Val_On  = TRUE;
BOOL WHIRL_Mldid_Mstid_On = TRUE;
BOOL WHIRL_Return_Info_On = TRUE;

/* ====================================================================
 *    Miscellaneous options
 * ====================================================================
 */

/* Can 64-bit values be 4-byte aligned in memory? */
BOOL Allow_Word_Aligned_Doubles = FALSE;

/* Should we generate position-independent code by default? */
BOOL Generate_Position_Independent_Code = FALSE;

/* Split 64-bit integer ops into 32-bit ops, and simulate them? */
BOOL Split_64_Bit_Int_Ops = TRUE;

/* Split quad-precision ops into double-precision, and simulate them? */
BOOL Split_Quad_Ops = TRUE;

/* Should we simulate 32-bit subprogram interface by default? */
BOOL Simulate_32_Bit_Interface = TRUE;

/* Work around a TFP branch cache problem. */
BOOL No_Quad_Aligned_Branch = FALSE;

/* Does target provides only unsigned 64-bit instructions? */
BOOL Only_Unsigned_64_Bit_Ops = FALSE;

BOOL Has_GP_Groups = FALSE;

/* Does target have offsets in load and store instructions?
 * Note: CG should instead test:
 * ( TOP_Find_Operand_Use( OP_code(op), OU_offset ) >= 0 ) */
BOOL Use_Load_Store_Offset = TRUE;

/* ====================================================================
 *
 * Abi_Name / Isa_Name / Targ_Name
 *
 * Produce printable names for the target choices.
 *
 * ====================================================================
 */

static struct bnm {
  char name[16];
} bnb[4];
static INT16 bnb_used = 0;

#if !defined MONGOOSE_BE || defined TARG_ST
/* clarkes: This function is used when writing the
   .assume assembler directive, so the ABI names must
   match what .assume expects.
*/
char *
Abi_Name ( TARGET_ABI b)
{
  char *r;

  switch ( b ) {
    case ABI_ARM_ver1:     return "eabi-ver1";
    case ABI_ARM_ver2:      return "eabi-ver2";
    default:
      r = bnb[bnb_used].name;
      bnb_used = (bnb_used + 1) % 4;
      sprintf (r, "ABI_%d", b);
      return r;
  }
}
#endif /* !defined MONGOOSE_BE || defined TARG_ST */

BOOL
Target_ABI_Preempts_Protected_Symbols ()
{
  return ! Is_Caller_Save_GP;
}

char *
Isa_Name ( TARGET_ISA b)
{
  char *r;

  switch ( b ) {
    case TARGET_ISA_armv5: return "armv5";
    case TARGET_ISA_armv6:  return "armv6";
    default:
      r = bnb[bnb_used].name;
      bnb_used = (bnb_used + 1) % 4;
      sprintf (r, "ISA_%d", b);
      return r;
  }
}

char *
Targ_Name ( TARGET_PROCESSOR b)
{
  char *r;

  switch ( b ) {
    case TARGET_armv5: return "armv5";
    case TARGET_armv6:  return "armv6";
    default:
      r = bnb[bnb_used].name;
      bnb_used = (bnb_used + 1) % 4;
      sprintf (r, "PROCESSOR_%d", b);
      return r;
  }
}

/* ====================================================================
 *
 * Prepare_Target
 *
 * Given target specification choices, fill in defaulted pieces and
 * check for conflicting specifications.  When this routine is done,
 * ABI, Target_ISA, Target, and Target_FPRs are all valid.  We also
 * use the target information to set the descriptive variables
 * Target_Int_Model, and Use_32_Bit_Pointers.
 *
 * TODO:  Pending final conversion of the driver, we may have incoming
 * information from either or both of -T... and -TARG:... options.  We
 * effectively give precedence to the -TARG: specifications, and after
 * final conversion should remove the -T support.  Note that we ignore
 * the pointer size and integer model specifications from -Tx,Pnn,Mm.
 *
 * ====================================================================
 */
static void
Prepare_Target ( void )
{
  TARGET_ISA isa_default = TARGET_ISA_UNDEF;
  TARGET_PROCESSOR targ_default = TARGET_UNDEF;

  /* First check the ABI from -TARG:abi=xxx: */
  if ( ABI_Name != NULL ) {
    if ( strcasecmp ( ABI_Name, "ARM" ) == 0
	 || strcmp ( ABI_Name, "eabi-ver1 ") == 0) {
      Target_ABI = ABI_ARM_ver1;
      isa_default = TARGET_ISA_armv5;
      targ_default = TARGET_armv5;
    }
    else if( strcmp ( ABI_Name, "eabi-ver2") == 0) {
      Target_ABI = ABI_ARM_ver2;
      isa_default = TARGET_ISA_armv5;
      targ_default = TARGET_armv5;
    }
    else {
      ErrMsg ( EC_Inv_TARG, "abi", ABI_Name );
    }
  }

  /* Next check the ISA from -TARG:isa=xxx: */
  if ( ISA_Name != NULL ) {
    TARGET_ISA isa;
    if ( strcasecmp ( ISA_Name, "armv5" ) == 0 ) {
      isa = TARGET_ISA_armv5;
      targ_default = TARGET_armv5;
    }
    else if ( strcasecmp ( ISA_Name, "armv6" ) == 0 ) {
      isa = TARGET_ISA_armv6;
      targ_default = TARGET_armv6;
    }
    else {
      ErrMsg ( EC_Inv_TARG, "isa", ISA_Name );
    }

    /* If there's an ISA from -Tn,Ix it must match: */
    if ( Target_ISA != TARGET_ISA_UNDEF && Target_ISA != isa ) {
      ErrMsg ( EC_Incons_TARG, "isa", ISA_Name,
	       "isa", Isa_Name(Target_ISA) );
    }
    Target_ISA = isa;
  }

  /* Now make sure ISA and ABI are consistent if both specified,
   * and if one is default the other:
   */
  switch ( Target_ISA ) {
    case TARGET_ISA_UNDEF:
      Target_ISA = isa_default;
      break;
  }

  /* Now check the target processor from -TARG:processor=xxx: */
  if ( Processor_Name != NULL ) {
    TARGET_PROCESSOR targ;
    if ( strcasecmp ( Processor_Name, "armv5" ) == 0 ) {
      targ = TARGET_armv5;
    }
    else if ( strcasecmp ( Processor_Name, "armv6" ) == 0 ) {
      targ = TARGET_armv6;
    }
    else {
      ErrMsg ( EC_Inv_TARG, "processor", Processor_Name );
      targ = TARGET_UNDEF;
    }
    
    /* If there's a processor spec from -Tn it must match: */
    if ( Target != TARGET_UNDEF && Target != targ ) {
      ErrMsg ( EC_Incons_TARG, "processor", Processor_Name,
	       "processor", Targ_Name(Target) );
    }
    Target = targ;
  }

  /* Now make sure ABI/ISA and Target are consistent if both specified,
   * and if one is default the other:
   */
  switch ( Target ) {
    case TARGET_armv5:
      if (Target_ISA == TARGET_ISA_UNDEF) Target_ISA = TARGET_ISA_armv5;
      break;
    case TARGET_armv6:
      if (Target_ISA == TARGET_ISA_UNDEF) Target_ISA = TARGET_ISA_armv6;
      break;
    case TARGET_UNDEF:
      Target = targ_default;
      if ( Target == TARGET_UNDEF ) {
        /* Default everything: */
	Target_ISA = TARGET_ISA_armv5;
	Target = TARGET_armv5;
      }
      break;
  }

  if (Target_ABI == ABI_UNDEF) {
      Target_ABI = ABI_ARM_ver2;
  }

  /* Set descriptive variables: */
  Use_32_Bit_Pointers = TRUE; /* Always true for ST200 ABIs. */

  return;
}

/* ====================================================================
 *
 * Preconfigure_Target
 *
 * Configuration of target-specific parameters, before flag processing.
 *
 * ====================================================================
 */

void
Preconfigure_Target ( void )
{
  /*
   * FE will remove CVTs of types > I4/U4 kids of STID/ISTOR unless 
   * this is undefined
   */
  FE_Cvtl_Opt = FALSE;

  // Enable tailmerge optimization
  WOPT_Enable_Tailmerge = TRUE;
  WOPT_Enable_Flow_Simplification_In_Tailmerge = TRUE;
  
  /*
   * Whether we will generate the MTYPE_B (default FALSE)
   */
  //WHIRL_Mtype_B_On = TRUE;

  // Enable tail recursion
  WOPT_Enable_Tail_Recur = TRUE;

  /* pv #297274 describes why we cannot put initialized data in .bss */
  /* (cbr) don't change a tentative definition for a definition.
   * ref iso/iec 9899 ansi C chap 6.7.2).
   * For st2x0 we only consider ourselves in "kernel" mode so that not all
   * registers are available for the appication in order to reduce context
   * switches (see st200 runtime manual).
   */
  // if ( Kernel_Code ) 
  Zeroinit_in_bss = FALSE;

  Gen_PIC_Calls = FALSE;	// ld handles pic calls for ST200
  GP_Is_Preserved = FALSE;

  Split_Quad_Ops = TRUE;

  // This flag seems to really only affect CVT/CVTL processing.
  // When it's on, the I8I4CVT is not simplified, for example.
  Split_64_Bit_Int_Ops = TRUE;

  // Target provides only 32-bit instructions
  Only_32_Bit_Ops = TRUE;


#ifndef TARG_ST
  // [CG] Defined below as a core specific value
  // Target does not support floating point arithmetic
  Emulate_FloatingPoint_Ops = TRUE;
#endif

  // Do not use the extract/compose whirl ops
  Enable_extract = FALSE;
  Enable_compose = FALSE;

  // Do not generate cis() INTRISIC_OPs
  // Otherwise, WOPT converts calls to sin()/cos() into
  // REAL/IMAG parts of a cis()
  CIS_Allowed = FALSE;

  // arithmetic ops can be safely speculated.
  Eager_Level = EAGER_ARITH;

  IPA_PU_Minimum_Size = 5;
  //  INLINE_Only_Inline = FALSE;

  // We have dismissible loads
  // do not set by default (maybe -O3 ?). potentially dangerous
  // Force_Memory_Dismiss = TRUE;

  // These are only necessary in the BACK_END
  // Can they be used elsewhere, eg. IPA ??
  //
#if defined(BACK_END)

  // overwrite some OPT defaults
  // Normally, do not inline divide sequences on this target
  // This is handled by ...
  OPT_Inline_Divide = FALSE;

  // Overwrite some WOPT defaults:

  // Do not use the extract/compose whirl ops. See Enable_extract_compose
  WOPT_Enable_Bits_Load_Store = FALSE;

  // do don't have a divrem instruction or runtime call.
  WOPT_Enable_DIVREM = FALSE;

  // do not allow minmax opcode: this is usefull when we're able to
  // set two conditions in one instruction
  WOPT_Enable_MINMAX = FALSE;

  // do not allow the test expression optimization
  //
  // TODO: should be anabled since it is usefull in the absence of
  //       hardware loop counters
  //
  WOPT_Enable_LFTR2 = FALSE;

  // This will cause WOPT to attempt to find out the real size of
  // objects (eg. ASHR >> 32 of an I8 will give I4), and use this
  // rather than the original type in creating STID/LDIDs of temps,
  // etc. We need to preserve the real types, otherwise we end up
  // with rotten WHIRL. 
  // NOTE: it is not clear to me why the IA64 version works when
  //       this is on. I suspect that everything's repeared when
  //       canonicalization up to U8 is performed.
  WOPT_Enable_Min_Type = FALSE;


  // Disable unsafe optimizations at any level. Can only be enabled on
  // command line.
  Simp_Unsafe_Relops = FALSE;
  //
  // Enable_CVT_Folding will try to remove the CVTs, specifically
  // from LDID/ILOAD/STID/ISTORE, which we can not tolerate.
  //
  //WOPT_Enable_Cvt_Folding = FALSE;

  // Overwrite some CG defaults: 

  // Generate floating point constants directly in the code
  CG_floating_const_in_memory = FALSE;

  LNO_Fission = 0;

  // FdF: Disable vector intrinsics until they are implemented.
  LNO_Run_Vintr = FALSE;

  // FdF: Disable padding, since it seems buggy (bug
  // pro-release-1-0-8-A/7)
  LNO_Local_Pad_Size = 0;


#endif

  // disable compare hoisting (bug codex/open64 #22946)
  WOPT_Enable_Compare_Hoisting = FALSE;


  // Default configuration for floating point and div/rem
  // on this machine.
  Emulate_Single_Float_Type = TRUE;
  Emulate_Double_Float_Type = TRUE;
  Emulate_DivRem_Integer_Ops = TRUE;
  Enable_Non_IEEE_Ops = FALSE;

  return;
}

/* ====================================================================
 *   Configure_Target
 *
 *   Configuration of target-specific parameters, after flag processing.
 * ====================================================================
 */
void
Configure_Target ()
{
  /* Set up the target processor and ISA: */
  Prepare_Target ();
  
  /* Set up the target register set: */

  // Ugly hack !!!
  // Normally, I do not know what this means ???
  // For now, the ST100 only uses this in symtab.cxx to set the
  // Int_Preg. Paradoxially, the Int_Preg shall definitely be
  // MTYPE_I4 on the ST100 (even MTYPE_I5 -- which may finally
  // resolve the problem by being I5 in a register and I8 in
  // memory ?). For now however, I set this to MTYPE_I4 so
  // that everything that wants Int_Preg gets MTYPE_I4.
  Spill_Int_Mtype = MTYPE_I4;
  Spill_Ptr_Mtype = MTYPE_U4;
  Spill_Float_Mtype = MTYPE_UNKNOWN;
  // Maximal MTYPE that directly corresponds to machine registers.
  // Perhaps there must be one for each type ...
  Max_Int_Mtype = Def_Int_Mtype = MTYPE_I4;
  Max_Uint_Mtype = Def_Uint_Mtype = MTYPE_U4;
  // That's too difficult now ...
  // Front-end does not know how to properly generate the MTYPE_B.
  // In WOPT there is a phase that introduces the MTYPE_B into the
  // program -- stick to this for now.
  //  Boolean_type  = WHIRL_Mtype_B_On ? MTYPE_B : MTYPE_I4;
  Boolean_type  = MTYPE_I4;
  Boolean_type2 = MTYPE_I4;
  // Ugly hack !!!
  // Same hack as above ...
  Integer_type = MTYPE_I4;

  /* Initialize pointer information */
  if (Use_32_Bit_Pointers) {
    Pointer_Size = 4;
    Pointer_Mtype  = MTYPE_U4;
    Pointer_type   = Pointer_Mtype;
    Pointer_Mtype2 = MTYPE_U4;
    Pointer_type2  = MTYPE_U4;
    Pointer_Register_Class = ISA_REGISTER_CLASS_integer;
  } else {
    Pointer_Size = 8;
    Pointer_Mtype  = MTYPE_U8;
    Pointer_type   = Pointer_Mtype;
    Pointer_Mtype2 = MTYPE_U8;
    Pointer_type2  = MTYPE_U8;
    Pointer_Register_Class = ISA_REGISTER_CLASS_UNDEFINED;
  }

  if (Use_32_Bit_Pointers) {
    AS_ADDRESS = AS_WORD;
    AS_ADDRESS_UNALIGNED = AS_WORD;
  } else {
    AS_ADDRESS = AS_DWORD;
    AS_ADDRESS_UNALIGNED = AS_DWORD_UNALIGNED;
  }

  /* If the user has requested aggregate alignment without specifying
   * a threshhold, set it to the register size.  Otherwise, make sure
   * it's a power of two.  WARNING:  The option group processing sets
   * it to -1 if no threshhold is given, and otherwise restricts it to
   * a reasonable range, so we don't worry about overflow or bad values.
   * Also, if the user has "turned down" alignment, don't try to
   * realign objects (pv 525474)
   */
  if ( Aggregate_Alignment > 0 ) {
    INT32 i = 1;
    while (i < Aggregate_Alignment) i <<= 1;
    Aggregate_Alignment = i;

    if (Aggregate_Alignment < 4) {
      Align_Object = FALSE;
    }
  }

  if (OPT_unroll_times > 0 && !OPT_unroll_times_overridden) {
    /* Disable loop unrolling at -Os, unless -funroll-loops is
       specified. */
    /* Otherwise, disable loop unrolling under option
       -fno-unroll-loops. */
    if ((Tune_Unrolling_For_Size && !(UnrollLoops_Set && UnrollLoops)) ||
	(UnrollLoops_Set && !UnrollLoops)) {
      OPT_unroll_times = 0;
    }
    /* For STxP70 there is no obvious benefit to unroll more than 2
       at -O3 as there is no instruction level parellelism.
       [JV] Keep this on ARM for now.
    */
    else if (Opt_Level == 2) {
      OPT_unroll_times = 2;
    } else if (Opt_Level > 2) {
      OPT_unroll_times = 2;
    }
  }

  if (OPT_unroll_size > 0 && !OPT_unroll_size_overridden) {
    if (Opt_Level == 2) {
      OPT_unroll_size = Tune_Unrolling_For_Size ? 10 : 32;
    } else if (Opt_Level > 2) {
      OPT_unroll_size = 64;
    }
  }

#ifdef TARG_ST
  /* For ARM enable division by constant as there is mul available. */
  if (!OPT_Cnst_DivRem_Set && Opt_Level >= 1) {
    OPT_Cnst_DivRem = TRUE;
  }
  /* On ARM, multiply by constant does not exists, so need to use this algorithm.
     TODO: Have a look on generated code and perhaps adapt it to use shift and add 
     and shift and reverse sub as specify in the doc page A3-12. */
  if (!OPT_Cnst_Mul_Set) {
    OPT_Cnst_Mul = TRUE;
  }

  // [HK] 20070924 Enable the rotate whirl op generation
  if (!Enable_Rotate_overriden) 
    Enable_Rotate = TRUE;
#endif

  Init_Targ_Sim();	/* must be done before initialize_stack_frame */

#define IS_POW2(n)              (((n) & ((n)-1))==0)
  FmtAssert (IS_POW2(Align_Instructions), 
	("-OPT:align_instructions=<n> must equal power of two"));
  if (Align_Instructions == 1) Align_Instructions = OPTIMIZED_INSTRUCTIONS_ALIGNMENT;

  FmtAssert (IS_POW2(Align_Functions), 
	("-OPT:align_instructions=<n> must equal power of two"));
  if (Align_Functions == 1) Align_Functions = OPTIMIZED_INSTRUCTIONS_ALIGNMENT;

  FmtAssert (IS_POW2(Align_Loops), 
	("-OPT:align_loops=<n> must equal power of two"));
  if (Align_Loops == 1) Align_Loops = OPTIMIZED_INSTRUCTIONS_ALIGNMENT;

  FmtAssert (IS_POW2(Align_Labels), 
	("-OPT:align_labels=<n> must equal power of two"));
  if (Align_Labels == 1) Align_Labels = OPTIMIZED_INSTRUCTIONS_ALIGNMENT;

  FmtAssert (IS_POW2(Align_Jumps), 
	("-OPT:align_jumps=<n> must equal power of two"));
  if (Align_Jumps == 1) Align_Jumps = OPTIMIZED_INSTRUCTIONS_ALIGNMENT;

  return;
}

/* ====================================================================
 *   IPA_Configure_Target
 *
 *   IPA-specific configuration.  Similar to Configure_Target but only set up
 *   those variables that IPA cares.
 * ====================================================================
 */
void
IPA_Configure_Target (void)
{
  Use_32_Bit_Pointers = TRUE; /* Always true for ST200 ABIs. */

  // Any ST200 ABI define these.
  Pointer_Size = 4;
  Pointer_Mtype  = MTYPE_U4;
  Pointer_type   = Pointer_Mtype;
  Pointer_Mtype2 = MTYPE_U4;
  Pointer_type2  = MTYPE_U4;

  Integer_type = MTYPE_I4;
  Boolean_type  = MTYPE_I4;
  Boolean_type2 = MTYPE_I4;
  return;
} /* IPA_Configure_Target */

/* ====================================================================
 *    Set_Target_ABI (is_64bit, isa)
 *
 *    return FALSE if abi is not one of the  allowed abi's.
 * ====================================================================
 */
BOOL 
Set_Target_ABI (
  BOOL is_64bit, 
  INT isa
)
{
  if (is_64bit) {
    /* No ARM ABI is 64 bits. */
    return FALSE;
  } else {	/* 32 */
    switch (Target_ABI) {
    case ABI_UNDEF:
      Target_ABI = ABI_ARM_ver2;
      break;
    default:
      /* All ARM ABIs are 32 bits. */
      break;
    }
  }

  if (Target_ISA == TARGET_ISA_UNDEF) {
    Target_ISA = TARGET_ISA_armv5;
  }

  return TRUE;
}

/* ====================================================================
 *
 * Configure_Source_Target
 *
 * Reconfiguration of target-specific parameters for each source file.
 *
 * ====================================================================
 */
void
Configure_Source_Target ( char * /* filename */ )
{
  char *option;

  /* ST200 doesn't have index loads.
   */
  Indexed_Loads_Allowed = FALSE;

  /* Miscellaneous exception control */
  if ( FP_Excp_Max != NULL ) {
    FP_Exception_Enable_Max = 0;
    option = FP_Excp_Max;
    while ( *option ) {
      switch ( *option ) {
	case 'I':	FP_Exception_Enable_Max |= FPX_I; break;
	case 'U':	FP_Exception_Enable_Max |= FPX_U; break;
	case 'O':	FP_Exception_Enable_Max |= FPX_O; break;
	case 'Z':	FP_Exception_Enable_Max |= FPX_Z; break;
	case 'V':	FP_Exception_Enable_Max |= FPX_V; break;
      }
      option++;
    }
  }
  if ( FP_Excp_Min != NULL ) {
    FP_Exception_Enable_Min = 0;
    option = FP_Excp_Min;
    while ( *option ) {
      switch ( *option ) {
	case 'I':	FP_Exception_Enable_Min |= FPX_I; break;
	case 'U':	FP_Exception_Enable_Min |= FPX_U; break;
	case 'O':	FP_Exception_Enable_Min |= FPX_O; break;
	case 'Z':	FP_Exception_Enable_Min |= FPX_Z; break;
	case 'V':	FP_Exception_Enable_Min |= FPX_V; break;
      }
      option++;
    }
  }

  if ( DEBUG_Trap_Uv )
    FP_Exception_Enable_Min |= FPX_V;

  // [FdF] Revert some settings from config.cxx with -O3
  // [CM] We also need to do it whatever the optimization level is
  // since -ffast-math and other -ffast can be set and turn these on
  // [HK] 20051122 these options should now be safely supported
  if (IEEE_Arithmetic > IEEE_ACCURATE) {
//     IEEE_Arithmetic = IEEE_ACCURATE;
//     Recip_Allowed = FALSE;
//     Rsqrt_Allowed = FALSE;
    Div_Split_Allowed = FALSE;
  }
  Recip_Allowed = FALSE;
  if (Opt_Level > 2 && Roundoff_Level > ROUNDOFF_NONE) {
    CIS_Allowed = FALSE;
  }

  return;
}

/* ====================================================================
 *
 * Check_Asm_Constraints
 *
 * Chech that asm constraints are compatible with the given MTYPE
 *
 * ====================================================================
 */
BOOL Check_Asm_Constraints(char* constraint_string, UINT8 mtype)
{
  char *s = constraint_string;
  char *mtype_name = MTYPE_name(mtype);
  
  /* Accepted modifiers.  */
  static const char modifiers[] = "+=&%";

  /* Remove modifiers appearing first.  */
  while (*s != '\0' && strchr(modifiers, *s)) {
    s++;
  }

  /* MTypes constraints, extension to standard GNU constraints. 
     MType names are prioritary compared to other constraints as it
     is a multi character constraint.*/
  if (strcmp(s, mtype_name) == 0) {
    return TRUE;
  }

  /* Accepted constraints. */
  static const char constraints[] = 
    " "			/* space, must be ignored. */
    "0123456789" 	/* matching constraints. */
    /* following are operand constraints. */
    "r"			/* general purpose register (int at least). */
    "p"			/* a memory address, generally a symbolic expression, used by load address instructions. */
    "g"			/* any register, memory or immediate operand, equivalent to "mri". */
    		/* Note that for RISC targets there is little use of this, though make it like "r". */
    "i"			/* an immediate integer operand including assembly time constant. */
    "n"			/* an immediate integer operand with a compile time known value. */
    "m"			/* a memory operand, rewritten as @(... + 0) on STxP70 or 0[...] for ST200. */
    "X"			/* any operand, i.e. possibly all of the other operand constraints. */
        	/* Note that for RISC targets there is little use of this, though make it like "r". */
    "b"			/* target depdt., boolean or predicate register if available. */
    "f"			/* target depdt., floating point register if available. */
    ;

  while (*s != '\0' && strchr(constraints, *s)) {
    s++;
  }
  
  if (*s == '\0') return TRUE;

  return FALSE;
}

#ifdef FRONT_END
//TB: Targinfo ABI initialization:for GCC
extern void 
GCC_Configure_ABI (void)
{
  switch (Target_ABI) {
  case ABI_ARM_ver1:
    /* Not yet supported */
    ABI_PROPERTIES_ABI_Value = ABI_PROPERTIES_ABI_arm_PIC;
    break;
  case ABI_ARM_ver2:
    ABI_PROPERTIES_ABI_Value = ABI_PROPERTIES_ABI_arm_PIC;
    break;
  default:
    FmtAssert(FALSE, ("targinfo for gcc doesn't handle abi: %d\n", Target_ABI));
  }
}

#endif

/* ====================================================================
 *   Apply_Opt_Size_Target
 *
 *   Set target options for size.
 * ====================================================================
 */
void
Apply_Opt_Size_Target(UINT32 level)
{
  //level = 0 means no size opt
  if (level == PU_OPTLEVEL_0 || level == PU_OPTLEVEL_UNDEF) return;
  
  FmtAssert(level == PU_OPTLEVEL_1,
	    ("Apply_Opt_Size_Target: only level 1 is implemented (asked was %d)",level));
  
  //Target specific flags
  if (!Tune_Unrolling_For_Size_Set)
    Tune_Unrolling_For_Size = TRUE;
  
  if (!Fast_Mult_For_Size_Set)
    Fast_Mult_For_Size = TRUE;

  if (!Fast_Div_For_Size_Set)
    Fast_Div_For_Size = TRUE;

  if (!Fast_Rem_For_Size_Set)
    Fast_Rem_For_Size = TRUE;

  if (!WOPT_Enable_DoWhile_Conversion_Set)
    WOPT_Enable_DoWhile_Conversion = DOWHILE_CONV_FOR_SIZE;

  if (!Opt_Prolog_For_Size_Set)
    Opt_Prolog_For_Size = TRUE;

}

