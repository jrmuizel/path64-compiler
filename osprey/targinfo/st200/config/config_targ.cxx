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
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
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
#include "erglob.h"
#include "tracing.h"
#include "mtypes.h"
#include "stab.h"
#include "targ_sim.h"
/* #include "ti_init.h" */

#if defined(FRONT_END_C) || defined(FRONT_END_CPLUSPLUS)
typedef unsigned char an_integer_kind;
#include "c_int_model.h"
#endif

/* =====================================================================
 *    Architecture specific definitions: 
 * =====================================================================
 */
BOOL ARCH_generate_nor = FALSE;       // Generate the NOR opcode
BOOL ARCH_mask_shift_counts = FALSE;  // shift counts are masked by the hardware (vs. truncated)

/* Needed todefine whether the packed arithmetic is allowed: */
BOOL INT_packed_arithmetic = TRUE;
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
INT16 FP_Exception_Enable_Max = FPX_DEF;     /* Max FP trap enables */
INT16 FP_Exception_Enable_Min = 0;	     /* Min FP trap enables */

INT32 Align_Instructions = 0;	             /* 0 means hasn't been set */
BOOL Avoid_TFP_blikely_bug = FALSE;
BOOL Avoid_TFP_blikely_bug_overridden = FALSE;

/***** IEEE 754 options *****/
BOOL Force_IEEE_Comparisons = TRUE;    /* IEEE NaN comparisons? */

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
BOOL Use_Load_Store_Offset = FALSE;

#if defined (FRONT_END_C) || defined (FRONT_END_CPLUSPLUS)

PREG_NUM Map_Reg_To_Preg [] = {
  /* DU registers. */
  0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007,
  0x008, 0x009, 0x00a, 0x00b, 0x00c, 0x00d, 0x00e, 0x00f,
  /* AU registers. */
  0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x017,
  0x018, 0x019, 0x01a, 0x01b, 0x01c, 0x01d, 0x01e, 0x01f,
  /* Predicate registers. */
     -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
  /* Control registers. */
  0x020, 0x021, 0x022, 0x023, 0x024, 0x025, 0x026, 0x027,
  0x028, 0x029, 0x02a, 0x02b, 0x02c, 0x02d, 0x02e, 0x02f,
  0x030, 0x031, 0x032, 0x033, 0x034, 0x035, 0x036, 0x037,
  0x038, 0x039, 0x03a, 0x03b, 0x03c, 0x03d, 0x03e, 0x03f,
};

#endif /* defined (FRONT_END_C) || defined (FRONT_END_CPLUSPLUS) */

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

#ifndef MONGOOSE_BE
char *
Abi_Name ( TARGET_ABI b)
{
  char *r;

  switch ( b ) {
    case ABI_ST200:     return "st200";
    default:
      r = bnb[bnb_used].name;
      bnb_used = (bnb_used + 1) % 4;
      sprintf (r, "ABI_%d", b);
      return r;
  }
}
#endif /* MONGOOSE_BE */

char *
Isa_Name ( TARGET_ISA b)
{
  char *r;

  switch ( b ) {
    case TARGET_ISA_ST200: return "ST200";
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
    case TARGET_st220: return "st220";
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
    if ( strcmp ( ABI_Name, "st32" ) == 0 ) {
      Target_ABI = ABI_ST200;
      isa_default = TARGET_ISA_ST200;
      targ_default = TARGET_st220;
    }
    else {
      ErrMsg ( EC_Inv_TARG, "abi", ABI_Name );
    }
  }

  /* Next check the ISA from -TARG:isa=xxx: */
  if ( ISA_Name != NULL ) {
    TARGET_ISA isa;

    if ( strcasecmp ( ISA_Name, "ST200" ) == 0 ) {
      isa = TARGET_ISA_ST200;
      targ_default = TARGET_st220;
    } else
    {
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

    if ( strcasecmp ( Processor_Name, "st220" ) == 0 ) {
      targ = TARGET_st220;
    } else {
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
    case TARGET_st220:
	Target_ABI = ABI_ST200;
	Target_ISA = TARGET_ISA_ST200;
	Target = TARGET_st220;
	break;
    case TARGET_UNDEF:
      Target = targ_default;
      if ( Target == TARGET_UNDEF ) {
	/* Default everything: */
	Target_ABI = ABI_ST200;
	Target_ISA = TARGET_ISA_ST200;
	Target = TARGET_st220;
      }
      break;
  }

  /* Now deal with FP register count: */
  switch ( Target_FPRs ) {
    default:	/* Invalid value */
      ErrMsg ( EC_Inv_FPRs, Target_FPRs );
      /* fall through */
    case 0:	/* Unspecified */
      Target_FPRs = 0;
      break;
  }

  /* Set descriptive variables: */
  Use_32_Bit_Pointers = (Target_ABI == ABI_ST200);

#if defined(FRONT_END_C) || defined(FRONT_END_CPLUSPLUS)
  Target_Int_Model = TARGET_INT_ILP32;
  Make_Int_Model_Consistent ();
#endif

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

  /* pv #297274 describes why we cannot put initialized data in .bss */
  /* (cbr) don't change a tentative definition for a definition.
   * ref iso/iec 9899 ansi C chap 6.7.2).
   * For st2x0 we only consider ourselves in "kernel" mode so that not all
   * registers are available for the appication in order to reduce context
   * switches (see st200 runtime manual).
   */
  // if ( Kernel_Code ) 
  Zeroinit_in_bss = FALSE;

  Gen_PIC_Calls = FALSE;	// ld handle's pic calls for IA-64
  GP_Is_Preserved = FALSE;

  Split_Quad_Ops = TRUE;

  // This flag seems to really only affect CVT/CVTL processing.
  // When it's on, the I8I4CVT is not simplified, for example.
  Split_64_Bit_Int_Ops = TRUE;

  // Target provides only 32-bit instructions
  Only_32_Bit_Ops = TRUE;

  // Target does not support floating point arithmetic
  Emulate_FloatingPoint_Ops = TRUE;

  // Do not use the extract/compose whirl ops
  Enable_extract_compose = FALSE;

  // These are only necessary in the BACK_END
  // Can they be used elsewhere, eg. IPA ??
  //
#if defined(BACK_END)

  // overwrite some OPT defaults
  // Normally, do not inline divide sequences on this target
  // This is handled by ...
  OPT_Inline_Divide = FALSE;

  // Overwrite some WOPT defaults:

  // do not allow minmax opcode: this is usefull when we're able to
  // set two conditions in one instruction
  WOPT_Enable_MINMAX = FALSE;

  // do not allow the test expression optimization
  //
  // TODO: should be anabled since it is usefull in the absence of
  //       hardware loop counters
  //
  WOPT_Enable_LFTR2 = FALSE;

  // simple if-conversion at CFG build time 
  //  WOPT_Enable_Simple_If_Conv = TRUE;

  // Overwrite some CG defaults: 

  // Generate floating point constants directly in the code
  CG_floating_const_in_memory = FALSE;

  // temporarily until I can enable it
  Enable_CG_Peephole = FALSE;
#endif

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

  /* Unrolling defaults */
  if (OPT_unroll_times > 0 && !OPT_unroll_times_overridden)
    OPT_unroll_times = 4;

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
  // This also seems to define the border between using CVT or CVTL ?
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

#if defined(FRONT_END_C) || defined(FRONT_END_CPLUSPLUS)
#ifndef EDG_FORTRAN
  Make_Int_Model_Consistent();
#endif
#endif

  /* Initialize pointer information */
  if (Use_32_Bit_Pointers) {
    Pointer_Size = 4;
    Pointer_Mtype  = MTYPE_U4;
    Pointer_type   = Pointer_Mtype;
    Pointer_Mtype2 = MTYPE_U4;
    Pointer_type2  = MTYPE_U4;
  } else {
    Pointer_Size = 8;
    Pointer_Mtype  = MTYPE_U8;
    Pointer_type   = Pointer_Mtype;
    Pointer_Mtype2 = MTYPE_U8;
    Pointer_type2  = MTYPE_U8;
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

#if defined(BACK_END)
  Init_Targ_Sim();	/* must be done before initialize_stack_frame */
#endif

#define IS_POW2(n)              (((n) & ((n)-1))==0)
  FmtAssert (IS_POW2(Align_Instructions), 
	("-OPT:align_instructions=<n> must equal power of two"));

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
  if (Target_ABI == ABI_ST200) {
    Pointer_Size = 4;
    Pointer_Mtype  = MTYPE_U4;
    Pointer_type   = Pointer_Mtype;
    Pointer_Mtype2 = MTYPE_U4;
    Pointer_type2  = MTYPE_U4;
  } else {
    FmtAssert (0, ("IPA_Configure_Target: unhandled ABI"));
  }

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
    switch (Target_ABI) {
    case ABI_UNDEF:
      return FALSE;
    default:
      return FALSE;
    }
  } else {	/* 32 */
    switch (Target_ABI) {
    case ABI_UNDEF:
      Target_ABI = ABI_ST200;
      break;
    case ABI_ST200:
      break;
    default:
      return FALSE;
    }
  }

  if (Target_ISA == TARGET_ISA_UNDEF) {
    Target_ISA = TARGET_ISA_ST200;
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

// TMP: ignore cpic until we figure out what to do with it
  if (Gen_PIC_Call_Shared)
	Gen_PIC_Call_Shared = FALSE;

  return;
}
