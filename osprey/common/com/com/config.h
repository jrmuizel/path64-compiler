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


#ifndef config_INCLUDED
#define config_INCLUDED

/* ====================================================================
 * ====================================================================
 *
 * Module: config.h
 *
 * Revision history:
 *  09-Apr-90 - Original Version
 *  01-Feb-91 - Copied for TP/Muse
 *  15-Jun-91 - Restructuring, Josie integration
 *
 * Description:
 *
 * Declare parameters describing the current host/target configuration,
 * and configuration options selected by the user in the command line.
 *
 * ====================================================================
 * NOTE:  We want to split out the declarations for various option
 * groups, so that adding to them doesn't cause virtually everything
 * to compile.  These are declared in files named
 * common/com/config_groupname.h, and should be used instead of adding
 * to this file, although many of the -OPT group options are still
 * here (feel free to move them).
 * ====================================================================
 *
 * Most of the compiler is designed to be independent of the host
 * (the machine and system where the compiler runs) and of the target
 * (the machine and system where the compiled code will run).  This
 * file (with those it includes) is the central repository for
 * parameters and data which help isolate those parts of the compiler
 * which are not host- and/or target-independent.  Note that we assume
 * in general that the "host" for building the compiler and the "host"
 * for running the compiler are the same.  Although this need not be
 * true, it generally will be except for bootstrap phases.  Given a
 * cross-compiler on the build host which is reasonably compatible with
 * the eventual native compiler on the compilation host, the
 * differences in the compiler code should be limited to type
 * definitions in defs.h and conceivably to the tdgen programs, which
 * will be built and run on the build host.
 *
 * CONFIGURATION PARAMETER CLASSES
 *
 * We recognize several classes of such parameters -- the following
 * attempts to provide guidelines on where to declare them and where
 * to process them.
 *
 * In the following description, "TARGET" refers to the directory
 * containing the host- and target-specific sources and build
 * directories; in the Twin Peaks case, it is "tp".
 *
 *  Ubiquitous:	Some configuration parameters are used essentially
 *		everywhere in the compiler, such as the basic data
 *		type declarations INT32, TN_NUM, and the like.
 *		Notwithstanding the following classifications, these
 *		are declared in TARGET/com/defs.h, which is included
 *		in (virtually) all source files.
 *
 *  Host Machine: Some parameters reflect the capabilities of the host
 *		hardware.  The most important other than the basic
 *		types defined in defs.h involves the representation
 *		and manipulation of target constants on the host
 *		machine, declared in file TARGET/com/targ_const.h.
 *		The remainder will be defined in the file
 *		TARGET/com/config_host.h, which is included in this
 *		file.
 *
 *  Host System: Some parameters reflect the configuration of the host
 *		software, e.g. the pathnames of library files, the
 *		syntax of the assembler, etc.  Those required broadly
 *		will be declared in TARGET/com/config_host.h; those
 *		with a more restricted clientele (e.g. the assembler
 *		interface) will be maintained in more specific files.
 *
 *		Note that we include in this class parameters which
 *		depend on which compiler process is being built, i.e.
 *		driver, front end, or back end.  Definitions specific
 *		to a single process should be #ifdef'ed based on
 *		DRIVER, FRONT_END, or BACK_END.
 *
 *  Target Machine: Most parameters dependent on the target machine
 *		are defined by the Target Information facility, and
 *		are used in general for driving the code generation process
 *		throughout the compiler.  The interface to the target
 *		information is in the form of generated header files
 *		in TARGET/targ_info. The generator programs and their
 *		input are all under common/targ_info.
 *
 *		The second significant component of this class is the
 *		handling of target constants; as described above, the
 *		interface to this functionality is in the file
 *		TARGET/com/targ_const.h.
 *
 *		Other parameters in this class should be declared in
 *		TARGET/com/config_targ.h.
 *
 *  Target System: Parameters in this class, e.g. runtime library
 *		interface information, will generally have a limited
 *		clientele, and will be in files reflecting this usage.
 *		Some such information is in the TDT, either due to its
 *		effect on code generation or due to the need to
 *		preprocess it in tdgen.  Anything else should be in
 *		TARGET/com/config_targ.h.
 *
 *  Compiler:	Parameters in this class control processing choices
 *		made by the compiler, independent of host or target,
 *		e.g. which optimizations to perform, how to schedule,
 *		and the like.  They will generally be declared in the
 *		driver modules for the relevant phases (e.g.
 *		be/cg/cg.h, be/gopt/opt.h), and initialized by the
 *		configuration routines (see below).
 *
 *  Options:	Many configuration parameters are set or affected by
 *		options specified by the command line or environment.
 *		Such parameters which need to be widely available will
 *		be in this file; those required only by command line
 *		processing will be in flags.h.  NOTE: choices made
 *		based on user interface flags like optimization level
 *		should be implemented by declaring a control variable
 *		and setting it in the configuration routines.  We want
 *		to be able to modify the effect of specific flag
 *		settings in a central place where the interactions are
 *		clear, rather than by searching the compiler for who
 *		depends on something like optimization level.
 *
 *		NOTE ALSO:  There is a general interface to compiler
 *		controls, settable via either the command line or
 *		pragmas, in controls.h.  It will still be preferable
 *		to interface to a control variable declared here, but
 *		the general, direct interface is available there.
 *
 *  Global Data: Such data, of a non-configuration nature (e.g. the
 *		current file names), used to be declared in flags.h.
 *		It is now declared in the process' miscellaneous
 *		globals header (i.e. glob.h) and defined in the
 *		process driver (e.g. bedriver.c for the back end).
 *
 * CONFIGURATION PROCESSING
 *
 * Each of the configuration files described above has an associated
 * source file with three external routines (at least):
 *
 *  Preconfigure_Xxx:  Does configuration required before flag
 *		processing (Xxx = Host, Target, etc.).
 *
 *  Configure_Xxx:  Does configuration required after flag processing.
 *
 *  Configure_Source_Xxx:  Does configuration required for each source
 *		file compiled.
 *
 * Processing will involve calling, at compiler initialization time,
 * the Preconfigure routines, flag processing, and the Configure
 * routines.  Then, as each source file is processed, the driver will
 * call the Configure_Source routine.
 *
 * CONTENTS
 *
 * - Memory allocation configuration.
 *
 *
 * Exported variables:
 *
 *	TODO: Cleanup organization of the *_Level and Max_Sdata_Elt_Size
 *	controls.  For example, the Debug and Profile maximums and defaults
 *	are determined by identical code in fedriver.c and bedriver.c.
 *	Eagerness levels need their own enumerated type.
 *
 *	INT8 Opt_Level
 *	    Optimization level, as set by -0 switch.  Bounded by
 *	    MAX_OPT_LEVEL.  Default is DEF_OPT_LEVEL if -O not given,
 *	    or DEF_O_LEVEL if -O given with no argument.
 *
 *	INT8 Debug_Level
 *	    Debug level, as set by -g switch.  Bounded by MAX_DEBUG_LEVEL.
 *	    Defaults to DEF_DEBUG_LEVEL if -g not given or 2 if -g given
 *	    with no argument (see fedriver.c, bedriver.c).
 *
 *	INT32 Max_Sdata_Elt_Size
 *	    Maximum size of data elements allowed in .sdata/.sbss 
 *	    (short data/bss) sections.  Bounded by MAX_SDATA_ELT_SIZE.
 *	    Defaults to DEF_SDATA_ELT_SIZE.
 *
 *	INT32 Max_Srdata_Elt_Size
 *	    Maximum size of data elements allowed in .srdata
 *	    (short rodata) section.  Bounded by MAX_SDATA_ELT_SIZE.
 *	    Defaults to DEF_SRDATA_ELT_SIZE.
 *
 *	INT16 Eager_Level
 *	    Eagerness level for speculative operations, as set by -X
 *	    switch.  Valid levels are:
 *	     EAGER_NONE		No speculative operations
 *	     EAGER_SAFE		Only exception-safe speculative ops
 *	     EAGER_ARITH	Arithmetic exceptions off
 *	     EAGER_DIVIDE	Divide by zero exceptions off
 *	     EAGER_MEMORY	Memory exceptions off
 *	     EAGER_OTHER	All speculative ops allowed
 *	    Each level includes the levels above (in this text) it.
 *	    Default is EAGER_SAFE.
 *
 *
 * SEE ALSO
 *
 *   common/com/MIPS/config_platform.h -- target platform configuration
 *   common/com/MIPS/config_targ.h -- target processor configuration
 *   common/com/MIPS/config_host.h -- compiler host configuration
 *   common/com/defs.h -- ubiquitous host types and configuration
 *			  options.
 *   common/util/flags.h -- command-line processing utilities.
 *
 *   common/com/config_ipa.h -- -IPA/-INLINE group options.
 *   common/com/config_opt.h -- -OPT group options.
 *   common/com/config_wopt.h -- -WOPT group options.
 *
 * ====================================================================
 * ====================================================================
 */


#ifdef _KEEP_RCS_ID
#endif /* _KEEP_RCS_ID */

#include "config_host.h"	/* in TARGET/com */
/* #include "config_targ.h" */	/* in TARGET/com */
#include "mtypes.h"             /* includes config_targ.h" */

#if defined(sun) || defined(__CYGWIN__) || defined(__MINGW32__)
 #include "W_endian.h" /* for BIG_ENDIAN, LITTLE_ENDIAN */
#else 
 #ifdef linux
  #include "W_endian.h"		/* for BIG_ENDIAN, LITTLE_ENDIAN */
 #else
  #include <sys/endian.h>		/* for BIG_ENDIAN, LITTLE_ENDIAN */
 #endif
#endif

#include "mempool.h"	/* Include the "fundamental" routines */
#include "flags.h"

/* IR builder sometimes needs to know whether we're in front end,
 * either to do something differently, or to complain about attempts
 * to handle something that shouldn't be present.  Note that the
 * arguments to the *Assert_Front_End macros are parenthesized
 * parameter lists to ErrMsg or printf.
 */
#ifdef SINGLE_PROCESS
  extern INT16 In_Front_End;
# ifdef Is_True_On
#   define Assert_Front_End(x)		Assert ( In_Front_End, x )
#   define FmtAssert_Front_End(x)	FmtAssert ( In_Front_End, x )
# endif
#else
# ifdef FRONT_END
#   define In_Front_End	TRUE
# else
#   define In_Front_End	FALSE
#   ifdef Is_True_On
#     define Assert_Front_End(x)	ErrMsg x
#     define FmtAssert_Front_End(x)	FmtAssert ( FALSE, x )
#   endif
# endif
#endif

#ifndef Assert_Front_End
# define Assert_Front_End(x)	(void)0
# define FmtAssert_Front_End(x)	(void)0
#endif



/* ====================================================================
 *
 * Miscellaneous configuration options
 *
 * ====================================================================
 */

/***** Language being compiled -- initialized in flags.c *****/
#include "language.h"

#ifdef __cplusplus
extern "C" {
#endif

BE_EXPORTED extern LANGUAGE Language;

#ifndef BE_EXPORTED
#define BE_EXPORTED
#endif

/* What is the model to be used for logical values in Fortran?
 *   TRUE:	.TRUE. is 1		(i.e. standard C/Unix model).
 *		truth test is zero/non-zero.
 *   FALSE:	.TRUE. is -1		(i.e. VMS Fortran model).
 *		truth test is LSB test.
 * This variable is defaulted to TRUE; the Fortran FE must set it to
 * FALSE prior to any IRB conversions if the VMS model is desired.
 */
BE_EXPORTED extern BOOL Use_C_Like_Logicals;

/***** LANGuage group options *****/
BE_EXPORTED extern BOOL CXX_Bool_On;
BE_EXPORTED extern BOOL CXX_Bool_Set;
BE_EXPORTED extern BOOL CXX_Exceptions_On;
BE_EXPORTED extern BOOL CXX_Exceptions_Set;
BE_EXPORTED extern BOOL CXX_Alias_Const;
BE_EXPORTED extern BOOL CXX_Alias_Const_Set;
BE_EXPORTED extern BOOL LANG_Recursive;	/* Fortran program contains recursion */
BE_EXPORTED extern BOOL LANG_Recursive_Set;
BE_EXPORTED extern BOOL CXX_Wchar_On;
BE_EXPORTED extern BOOL CXX_Wchar_Set;

BE_EXPORTED extern BOOL  CXX_Namespaces_On;
BE_EXPORTED extern BOOL  CXX_Namespaces_Set;
BE_EXPORTED extern BOOL  CXX_Ansi_For_Init_Scope_On;
BE_EXPORTED extern BOOL  CXX_Ansi_For_Init_Scope_Set;
BE_EXPORTED extern BOOL  CXX_Explicit_On;
BE_EXPORTED extern BOOL  CXX_Explicit_Set;
BE_EXPORTED extern BOOL  CXX_Typename_On;
BE_EXPORTED extern BOOL  CXX_Typename_Set;
BE_EXPORTED extern BOOL  CXX_Mutable_On;
BE_EXPORTED extern BOOL  CXX_Mutable_Set;
BE_EXPORTED extern BOOL  CXX_Packed_On;
BE_EXPORTED extern BOOL  CXX_Packed_Set;

BE_EXPORTED extern BOOL  CXX_Standard_C_Plus_Plus_On;
BE_EXPORTED extern BOOL  CXX_Standard_C_Plus_Plus_Set;

BE_EXPORTED extern BOOL  LANG_Pch;
BE_EXPORTED extern BOOL  LANG_Pch_Set;
BE_EXPORTED extern char * LANG_Create_Pch;
BE_EXPORTED extern BOOL  LANG_Create_Pch_Set;
BE_EXPORTED extern char * LANG_Use_Pch;
BE_EXPORTED extern BOOL  LANG_Use_Pch_Set;
BE_EXPORTED extern char * LANG_Pchdir;
BE_EXPORTED extern BOOL  LANG_Pchdir_Set;

BE_EXPORTED extern char * LANG_cxx_dialect;
BE_EXPORTED extern BOOL  LANG_cxx_dialect_Set;

BE_EXPORTED extern BOOL  LANG_Microsoft_Mode;
BE_EXPORTED extern BOOL  LANG_Microsoft_Mode_Set;

BE_EXPORTED extern BOOL  LANG_Ansi_Setjmp_On;
BE_EXPORTED extern BOOL  LANG_Ansi_Setjmp_Set;

BE_EXPORTED extern BOOL  LANG_Ignore_Carriage_Return_On;
BE_EXPORTED extern BOOL  LANG_Ignore_Carriage_Return_Set;

BE_EXPORTED extern BOOL  C_Restrict_On;
BE_EXPORTED extern BOOL  C_Restrict_Set;

BE_EXPORTED extern char * C_Auto_Restrict;
BE_EXPORTED extern BOOL  C_Auto_Restrict_Set;

BE_EXPORTED extern BOOL  FTN_Short_Circuit_On;
BE_EXPORTED extern BOOL  FTN_Short_Circuit_Set;

BE_EXPORTED extern BOOL  WHIRL_Comma_Rcomma_On;
BE_EXPORTED extern BOOL  WHIRL_Comma_Rcomma_Set;

BE_EXPORTED extern BOOL  Macro_Expand_Pragmas_On;
BE_EXPORTED extern BOOL  Macro_Expand_Pragmas_Set;

BE_EXPORTED extern BOOL  C_VLA_On;
BE_EXPORTED extern BOOL  C_VLA_Set;

BE_EXPORTED extern BOOL  WHIRL_Merge_Types_On;
BE_EXPORTED extern BOOL  WHIRL_Merge_Types_Set;

BE_EXPORTED extern BOOL  LANG_Symtab_Verify_On;
BE_EXPORTED extern BOOL  LANG_Symtab_Verify_Set;

BE_EXPORTED extern BOOL  WHIRL_Mtype_A_On;
BE_EXPORTED extern BOOL  WHIRL_Mtype_B_On;
BE_EXPORTED extern BOOL  WHIRL_Mtype_BS_On;
BE_EXPORTED extern BOOL  WHIRL_Return_Val_On;
BE_EXPORTED extern BOOL  WHIRL_Flatten_Field_On;
BE_EXPORTED extern BOOL  WHIRL_Mldid_Mstid_On;
BE_EXPORTED extern BOOL  WHIRL_Vfcall_On;
BE_EXPORTED extern BOOL  WHIRL_Addr_Passed_On;
BE_EXPORTED extern BOOL  WHIRL_Addr_Saved_For_Passed_On;
BE_EXPORTED extern BOOL  WHIRL_Addr_Saved_On;
BE_EXPORTED extern BOOL  WHIRL_Keep_Cvt_On;
BE_EXPORTED extern BOOL  WHIRL_Return_Info_On;

BE_EXPORTED extern BOOL  Global_Pragmas_In_Dummy_PU_On;
BE_EXPORTED extern BOOL  Malloc_Free_On;
BE_EXPORTED extern BOOL  Alloca_Dealloca_On;
BE_EXPORTED extern BOOL  Barrier_Lvalues_On;

/***** The following is TRUE for C++  unless -no_exceptions is specified *****/
BE_EXPORTED extern BOOL  Allow_Exceptions;

/***** Compiler debug/trace options *****/
BE_EXPORTED extern BOOL  BE_EXPORTED Tracing_Enabled;	/* Any trace options set? */

/* Control usage of the .I and .J files: */
BE_EXPORTED extern BOOL  Open_IJ_Files;

/* For communication between driver and config routines ONLY: */
BE_EXPORTED extern INT8 Debug_Level;
# define MAX_DEBUG_LEVEL	3

typedef enum {
  EAGER_NONE,
  EAGER_SAFE,
  EAGER_ARITH,
  EAGER_DIVIDE,
  EAGER_MEMORY,
  EAGER_OTHER,
  EAGER_EXCESS
} EAGER_LEVEL;
BE_EXPORTED extern EAGER_LEVEL Eager_Level;

/***** Miscellaneous optimization options *****/
/* Should idict commute operands in seeking match? */
BE_EXPORTED extern BOOL  Idict_Commutable_Match;
BE_EXPORTED extern BOOL  Enable_FE_Optimization;	/* Enable FE (KAP) scalar opt? */
#ifdef TARG_ST
BE_EXPORTED extern BOOL  FE_Cvtl_Opt;                /* Keep CVTs for STOREs ? */
  // FdF 20050203: Fine tuning of prefetch optimizations
BE_EXPORTED extern INT32 Prefetch_Optimize;
#endif
BE_EXPORTED extern BOOL  Alias_Pointer_Parms;	/* Reference parms indep? */
BE_EXPORTED extern BOOL  Alias_Pointer_Types;	/* Ptrs to distinct basic types indep? */
BE_EXPORTED extern BOOL  Alias_Not_In_Union;          /* C++ ONLY rule: assume ptrs to objects with user-constructors are NOT in unions */
BE_EXPORTED extern BOOL  Alias_Pointer_Strongly_Typed; /* Ptrs to distinct types indep? */
BE_EXPORTED extern BOOL  Alias_Pointer_Named_Data;	/* No pointers to named data? */
BE_EXPORTED extern BOOL  Alias_Pointer_Restricted;	/* *p and *q not aliased */
BE_EXPORTED extern BOOL  Alias_Pointer_Disjoint;     /* **p and **q not aliased */
BE_EXPORTED extern BOOL  Alias_Pointer_Cray;         /* Cray pointer semantics? */
BE_EXPORTED extern BOOL  Alias_Common_Scalar;        /* Distinguish scalar from other array
                                           in a common block */

BE_EXPORTED extern BOOL  CSE_Elim_Enabled;		/* Is CSE-elim on? -- this does
					 * not control it, it just
					 * shadows the opt. level
					 */
BE_EXPORTED extern BOOL  Enable_GOT_Call_Conversion; /* %call16 -> %got_disp? */
BE_EXPORTED extern BOOL  OPT_Unroll_Analysis;	/* Enable unroll limitations? */
BE_EXPORTED extern BOOL  OPT_Unroll_Analysis_Set;	/* ... option seen? */

/***** Various Scalar Optimizer options *****/
BE_EXPORTED extern BOOL  Enable_Copy_Propagate;

/***** Put all-zero initialized file-level data in the BSS section? *****/
BE_EXPORTED extern BOOL  Zeroinit_in_bss;

/***** IEEE 754 options *****/
typedef enum {
  IEEE_STRICT = 0,	/* Conform strictly */
	/* IEEE_STRICT is not supported.  It might be useful to
	 * avoid madds, do gradual underflow, etc...
	 */
  IEEE_ACCURATE = 1,	/* Do not degrade IEEE accuracy */
  IEEE_INEXACT = 2,	/* Inexact results may not be IEEE */
#ifdef TARG_ST
  IEEE_ASSOC = 3,	/* Reassociations allowed */
  IEEE_ANY = 4		/* Anything goes */
#else
  IEEE_ANY = 3		/* Anything goes */
#endif
} IEEE_LEVEL;
BE_EXPORTED extern IEEE_LEVEL IEEE_Arithmetic;  /* IEEE arithmetic? */
BE_EXPORTED extern BOOL  IEEE_Arith_Set;	/* ... option seen? */

/***** Constant folding options *****/
typedef enum {
  ROUNDOFF_NONE,	/* No roundoff-inducing transformations */
  ROUNDOFF_SIMPLE,	/* Simple roundoff transformations */
  ROUNDOFF_ASSOC,	/* Reassociation transformations */
  ROUNDOFF_ANY		/* Anything goes */
} ROUNDOFF;
BE_EXPORTED extern ROUNDOFF Roundoff_Level;		/* -OPT_roundoff=n value */
BE_EXPORTED extern BOOL  Roundoff_Set;		/* ... option seen? */
BE_EXPORTED extern BOOL  Enable_WN_Simp;             /* Use the WHIRL simplifier? */
BE_EXPORTED extern BOOL  Regions_Around_Inner_Loops; /* Put REGIONs around inner loops */
BE_EXPORTED extern BOOL  Region_Boundary_Info;	/* calc boundary info for regions */
BE_EXPORTED extern BOOL  Cray_Ivdep;   		/* Use Cray meaning for ivdep */
BE_EXPORTED extern BOOL  Liberal_Ivdep;   		/* Use liberal meaning for ivdep */
BE_EXPORTED extern BOOL  Rsqrt_Allowed;		/* Generate RSQRT instruction? */
BE_EXPORTED extern BOOL  Recip_Allowed;		/* Generate RECIP instruction? */
BE_EXPORTED extern BOOL  Enable_Cfold_Aggressive;	/* Complex constant folding? */
BE_EXPORTED extern BOOL  Ptr_Opt_Allowed;	        /* Treat pointers as arrays */
BE_EXPORTED extern BOOL  Fast_Complex_Allowed;	/* Enable fast c_div and c_abs? */
BE_EXPORTED extern BOOL  Fast_Complex_Set;		/* ... option seen? */
BE_EXPORTED extern BOOL  Fast_Bit_Allowed;		/* Fast inlined bit intrinsics? */
BE_EXPORTED extern BOOL  Fast_Bit_Set;		/* ... option seen? */
BE_EXPORTED extern BOOL  Fast_NINT_Allowed;		/* Fast NINT and ANINT? */
BE_EXPORTED extern BOOL  Fast_NINT_Set;		/* ... option seen? */
BE_EXPORTED extern BOOL  Fast_trunc_Allowed;		/* Fast trunc of NINT/ANINT/AINT/AMOD */
BE_EXPORTED extern BOOL  Fast_trunc_Set;		/* ... option seen? */
BE_EXPORTED extern BOOL  Fast_IO_Allowed;		/* Fast printf/scanf/printw */
BE_EXPORTED extern BOOL  Inline_Intrinsics_Allowed;	/* Inline intrinsics? Or lib calls? */
BE_EXPORTED extern BOOL  Inline_Intrinsics_Set;	/* ... option seen? */
BE_EXPORTED extern BOOL  Simp_Multiply_To_Shift;	/* Change multiplies to shifts? */
BE_EXPORTED extern BOOL  Simp_Canonicalize;          /* Simple canon/reassoc */
BE_EXPORTED extern BOOL  Simp_Fold_Unsigned_Relops;  /* Simplify unsigned relops */
BE_EXPORTED extern BOOL  Simp_Unsafe_Relops;         /* Allow foldings which might cause error if overflow occurs */
BE_EXPORTED extern BOOL  Enable_NaryExpr;		/* Allow nary expr in the lowerer */
BE_EXPORTED extern BOOL  Enable_NaryExpr_Set;	/* ... option seen? */

#if defined (linux) || defined(sun) || defined(__CYGWIN__) || defined(__MINGW32__)
BE_EXPORTED extern BOOL  Enable_WFE_DFE;		/* frontend dead function elimination? */
#endif /* linux || sun */

/***** Global Code Motion (GCM) options *****/
BE_EXPORTED extern BOOL  GCM_Eager_Null_Ptr_Deref;   /* allow speculation past the NULL
					   ptr test. assumes page zero as
					   readable */
BE_EXPORTED extern BOOL  GCM_Eager_Null_Ptr_Deref_Set; /* ... option seen? */

/***** Miscellaneous GOPT options *****/
#define MAX_OPT_LEVEL	3
#define DEF_O_LEVEL	2	/* Level implied by -O */
#define DEF_OPT_LEVEL	1
BE_EXPORTED extern INT32 Opt_Level;		/* -On level */
#ifdef TARG_ST
BE_EXPORTED extern BOOL UnrollLoops;
BE_EXPORTED extern BOOL UnrollLoops_Set;
#endif
BE_EXPORTED extern INT32 OPT_unroll_times;
BE_EXPORTED extern BOOL  OPT_unroll_times_overridden;
BE_EXPORTED extern INT32 OPT_unroll_size;
BE_EXPORTED extern BOOL  OPT_unroll_size_overridden;
BE_EXPORTED extern BOOL  OPT_Lower_Speculate;
BE_EXPORTED extern BOOL  OPT_Lower_Treeheight;
BE_EXPORTED extern BOOL  OPT_Inline_Divide;
BE_EXPORTED extern BOOL  OPT_Space;
BE_EXPORTED extern INT32 Olimit; /* stop optimization or use regions at this limit */
/* FALSE => stop optimization if Olimit reached;
 * TRUE  => use regions to optimize if Olimit reached */
BE_EXPORTED extern BOOL  BE_EXPORTED Olimit_opt; 
BE_EXPORTED extern BOOL  CG_mem_intrinsics;
BE_EXPORTED extern INT32 CG_memmove_inst_count;
BE_EXPORTED extern BOOL  CG_memmove_inst_count_overridden;
BE_EXPORTED extern BOOL  CG_bcopy_cannot_overlap;
BE_EXPORTED extern BOOL  CG_memcpy_cannot_overlap;
BE_EXPORTED extern BOOL  CG_memmove_cannot_overlap;
BE_EXPORTED extern BOOL  CG_memmove_nonconst;
BE_EXPORTED extern BOOL  CG_floating_const_in_memory;
BE_EXPORTED extern BOOL  Allow_wrap_around_opt;
#define DEF_FOLD_ARITH_MAX_INS_CNT 1000
BE_EXPORTED extern INT32 Fold_Arith_Max_INS_CNT;
#define DEF_CONST_COPY_TN_CNT 10000
BE_EXPORTED extern INT32 Const_Copy_TN_CNT;
#define DEF_GOPT_TN_CNT 15000
BE_EXPORTED extern INT32 Gopt_TN_CNT;
BE_EXPORTED extern BOOL  Enable_BB_Splitting; /* Split long basic blocks? */
BE_EXPORTED extern INT32 Split_BB_Length;	/* split BBs that are > than this */
#define DEF_BBLENGTH	 300	/* default value for Split_BB_Length */
#define MIN_BBLENGTH	 100	/* don't let the value get too small */
#define MAX_BBLENGTH	5000	/* don't let the value get too big */

/***** What is the byte sex of the host and target? *****/
BE_EXPORTED extern UINT8 Host_Byte_Sex;	/* Set in config_host.c */
BE_EXPORTED extern UINT8 Target_Byte_Sex;	/* Set in config_targ.c */
BE_EXPORTED extern BOOL   Same_Byte_Sex;	/* Set in config_targ.c */

BE_EXPORTED extern INT32 iolist_reuse_limit;

/***** Misaligned memory reference control *****/
BE_EXPORTED extern INT32 Aggregate_Alignment; /* This alignment for aggregate layout */

BE_EXPORTED extern BOOL  Align_Object;	/* Try to improve the alignment of objects */
BE_EXPORTED extern BOOL  Align_Padding;	/* Pad objects to their natural alignment */
BE_EXPORTED extern BOOL  UseAlignedCopyForStructs;	/* always use aligned copy */

/***** Miscellaneous code generation options *****/
BE_EXPORTED extern BOOL  Gen_PIC_Call_Shared; /* CPIC */
BE_EXPORTED extern BOOL  Gen_PIC_Shared;	/* PIC */
#ifdef TARG_ST
BE_EXPORTED extern BOOL  Gen_PIC_Call_Shared_Set; /* CPIC */
BE_EXPORTED extern BOOL  Gen_PIC_Shared_Set;	/* PIC */
BE_EXPORTED extern BOOL  No_Shared_Warning;
#endif
#ifdef TARG_ST
BE_EXPORTED extern INT32 ENV_Symbol_Visibility;
BE_EXPORTED extern char *ENV_Symbol_Visibility_String;
BE_EXPORTED extern char *ENV_Symbol_Visibility_Spec_Filename;
#endif

BE_EXPORTED extern BOOL  Gen_PIC_Calls;	/* do calls as PIC code */
BE_EXPORTED extern BOOL  Guaranteed_Small_GOT; /* GOT < 64kB? */
BE_EXPORTED extern BOOL  Non_Volatile_GOT;	/* GOT entries volatile? */
BE_EXPORTED extern BOOL  PIC_Local_Names;	/* Names local by default? */
BE_EXPORTED extern BOOL  PIC_Protected_Names; /* Names protected by default? */
BE_EXPORTED extern BOOL  PIC_Fixed_Addresses; /* Names fixed by default? */
BE_EXPORTED extern BOOL  PIC_No_Page_Offset;	/* Don't use page/offset addressing? */
BE_EXPORTED extern BOOL  Force_Mem_Formals;	/* Always force formals to memory? */
BE_EXPORTED extern BOOL  Kernel_Code;	/* Compiling OS kernel? */
BE_EXPORTED extern INT32 Short_Data;	/* Objects of this size in .sdata */
BE_EXPORTED extern INT32 Short_Lits;	/* Literals of this size in .litX */
BE_EXPORTED extern INT32 Max_Sdata_Elt_Size;/* -Gn: sdata size */
BE_EXPORTED extern INT32 Max_Srdata_Elt_Size;/* -Gn: sdata size */
BE_EXPORTED extern INT32 Gspace_Available;	/* -Gspace: available space for gprel objects */
BE_EXPORTED extern BOOL  Force_GP_Prolog;	/* force usage of gp prolog */
BE_EXPORTED extern INT32 Heap_Allocation_Threshold; /* Allocate objects > this on the heap 
					 * (-1 means always use stack), 
					 * 0 always use heap
					 * default is 0
					 */
BE_EXPORTED extern BOOL  Strings_Not_Gprelative;	/* don't make strings gp-relative */
#ifdef TARG_ST
  /* clarkes: Allow the target to override these default settingsin config_target.h */
#  ifndef MAX_SDATA_ELT_SIZE
#    define MAX_SDATA_ELT_SIZE	32760
#  endif
#  ifndef DEF_SDATA_ELT_SIZE
#    define DEF_SDATA_ELT_SIZE	8
#  endif
#  ifndef DEF_SRDATA_ELT_SIZE
#    define DEF_SRDATA_ELT_SIZE DEF_SDATA_ELT_SIZE
#  endif
#else
#define MAX_SDATA_ELT_SIZE	32760
#define DEF_SDATA_ELT_SIZE	8
#endif
BE_EXPORTED extern BOOL  Varargs_Prototypes;	/* Varargs have prototypes for FP? */
BE_EXPORTED extern BOOL  Gen_Profile;
BE_EXPORTED extern char *Gen_Profile_Name;
BE_EXPORTED extern BOOL  Call_Mcount;	/* generate a call to mcount in pu entry */
#ifdef TARG_ST
BE_EXPORTED extern BOOL  Instrument_Functions_Enabled;	/* generate calls to instrumentation for function entries and exits. */
BE_EXPORTED extern BOOL Profile_Arcs_Enabled_Cgir; /* Create data files for the `gcov' code-coverage utility and instrument code. */
BE_EXPORTED extern BOOL Profile_Arcs_Enabled; /* Create data files for the `gcov' code-coverage utility and instrument code. */
BE_EXPORTED extern BOOL Test_Coverage_Enabled; /* Create data files for the `gcov' code-coverage utility and instrument code. */
BE_EXPORTED extern BOOL Coverage_Counter64; /* Use 64 bits counters instead of 32. */
BE_EXPORTED extern BOOL Branch_Probabilities; /* Use .gcda file ad feedback. */
#endif
BE_EXPORTED extern BOOL  Gen_GP_Relative;    /* generate GP-relative addressing ? */
BE_EXPORTED extern BOOL  GP_Is_Preserved;	/* GP is neither caller or callee-save */
BE_EXPORTED extern BOOL  Constant_GP;	/* GP never changes */

BE_EXPORTED extern char * Emit_Global_Data;	/* only emit global data */
BE_EXPORTED extern char * Read_Global_Data;	/* only read global data */

BE_EXPORTED extern char * Library_Name;              /* -TENV:io_library=xxx */
BE_EXPORTED extern INT target_io_library;

BE_EXPORTED extern BOOL  Meld_Schedule;	/* Attempt to meld basic blocks	*/
BE_EXPORTED extern BOOL  Gap_Schedule;	/* Attempt to perform gap scheduling */
BE_EXPORTED extern BOOL  Attempt_Bypass;	/* Attempt to use bypass registers */
BE_EXPORTED extern BOOL  Enable_SWP;		/* Do software pipelining */
BE_EXPORTED extern BOOL  Enable_SWP_overridden; /* override on command line */
BE_EXPORTED extern BOOL  Enable_LOH;		/* Do loop overhead */
BE_EXPORTED extern BOOL  Enable_LOH_overridden; /* Enable_LOH overridden on cmd line */
BE_EXPORTED extern BOOL  Enable_Spec_Loads;	/* Allow speculation of loads */
BE_EXPORTED extern BOOL  Isolate_Lines;	/* Don't overlap source	lines */
BE_EXPORTED extern BOOL  Fill_Delay_Slots;	/* Attempt to fill branch delay slots */
BE_EXPORTED extern BOOL  Enable_GDSE;	/* Do global dead store elimination */
#ifndef TARG_ST
/* moved this to cg_flags[h,cxx] */
BE_EXPORTED extern BOOL  Enable_CG_Peephole;	/* Enable peephole optimization in cgprep */
#endif
BE_EXPORTED extern BOOL  Optimize_CVTL_Exp;	/* Optimize expansion of CVTL operators */
BE_EXPORTED extern BOOL  Enable_CVT_Opt;	/* Optimize expansion of CVT operators */
BE_EXPORTED extern BOOL  Indexed_Loads_Allowed; /* enable generation of indexed loads/stores */
BE_EXPORTED extern BOOL  Early_MP_Processing; /* Do MP lowering before LNO/PREOPT */
BE_EXPORTED extern BOOL  Implied_Do_Io_Opt;   /* Do implied-do loop optimization for I/O */
BE_EXPORTED extern BOOL  Enable_LAI;          /* Do generate Lai_Code */

/* back end phases options */
#ifdef BACK_END
BE_EXPORTED extern BOOL  BE_EXPORTED Run_lno;   /* run loop-nest optimizer */
BE_EXPORTED extern BOOL  Run_lego;               /* run lego-lowering */
BE_EXPORTED extern BOOL  Run_lego_given;         /* was run lego-lowering given/not */
BE_EXPORTED extern BOOL  BE_EXPORTED Run_wopt;  /* run WHIRL global optimizer */
BE_EXPORTED extern BOOL  BE_EXPORTED Run_preopt;/* run WHIRL preopt optimizer */
BE_EXPORTED extern BOOL  BE_EXPORTED Run_cg;    /* run code generator */
BE_EXPORTED extern BOOL  BE_EXPORTED Run_w2c;   /* run whirl2c */
BE_EXPORTED extern BOOL  BE_EXPORTED Run_w2f;   /* run whirl2f */
BE_EXPORTED extern BOOL  BE_EXPORTED Run_w2fc_early; /* run whirl2f after LNO parallelization */
BE_EXPORTED extern BOOL  BE_EXPORTED Run_prompf;/* create prompf analysis file */
BE_EXPORTED extern BOOL  BE_EXPORTED Run_purple;/* run purple instrumenter */
BE_EXPORTED extern BOOL  BE_EXPORTED Run_ipl;   /* run summary phase of IPA */
BE_EXPORTED extern char * LNO_Path;		    /* path to lno.so */
BE_EXPORTED extern char * WOPT_Path;		    /* path to wopt.so */
BE_EXPORTED extern char * CG_Path; /* path to cg.so */
BE_EXPORTED extern char * W2C_Path;		    /* path to whirl2c.so */
BE_EXPORTED extern char * W2F_Path;		    /* path to whirl2f.so */
BE_EXPORTED extern char * Prompf_Anl_Path;	    /* path to prompf_anl.so */
BE_EXPORTED extern char * Purple_Path;	    /* path to purple.so */
BE_EXPORTED extern char * Ipl_Path;		    /* path to ipl.so */
#endif /* BACK_END */
BE_EXPORTED extern char * Inline_Path;           /* path to inline.so */
#if defined(BACK_END) || defined(QIKKI_BE)
BE_EXPORTED extern char * Targ_Path;		    /* path to targinfo .so */
#endif /* defined(BACK_END) || defined(QIKKI_BE) */

BE_EXPORTED extern char * Schedlist_Option;

/* Force EH Region offsets to be long */
BE_EXPORTED extern BOOL  Force_Long_EH_Range_Offsets;
/* Force stack frame to use large model */
BE_EXPORTED extern BOOL  Force_Large_Stack_Model;
/* put each function in its own text section */
BE_EXPORTED extern BOOL  Section_For_Each_Function;

/* list of registers that are not allocatable */
BE_EXPORTED extern OPTION_LIST *Registers_Not_Allocatable;

/* Unique ident from IPA */
BE_EXPORTED extern INT32 Ipa_Ident_Number;
#ifdef TARG_ST
// [CL] unique label suffix
BE_EXPORTED extern char *Ipa_Label_Suffix;
BE_EXPORTED extern char *Ipa_Exec_Name;
#endif
#ifdef TARG_ST
BE_EXPORTED extern BOOL Ignore_Builtin_Prefetch;
#endif

BE_EXPORTED extern BOOL  Scalar_Formal_Ref;		/* for fortran formal scalar refs */
BE_EXPORTED extern BOOL  Non_Scalar_Formal_Ref;	/* for fortran formal non_scalar refs */

/***** Maximum sizes of recursive structures we will handle *****/
#define MAXDONEST	300
#define MAXIFNEST	300

/* The following define specifies the maximum evaluation depth from
 * leaf to root in an expression containing boolean operators (AND,
 * OR, NOT).  The amount of space specified will be allocated to hold
 * the inherited attributes of boolean expressions and flow of control
 * statements.
 */
#define MAXBOOL BE_EXPORTED DEPTH    (100 + MAXIFNEST)

/* ====================================================================
 *
 *   Target-specific controls: 
 *
 *   Moved declarations common to all targets here from 
 *   ia64/config_target.h
 *
 * ====================================================================
 */

/* We need to know the machine type of the integer and floating point
 * registers for purposes of subprogram save/restore:
 */
BE_EXPORTED extern CLASS_INDEX Spill_Int_Mtype;
BE_EXPORTED extern CLASS_INDEX Spill_Ptr_Mtype;
BE_EXPORTED extern CLASS_INDEX Spill_Float_Mtype;

/* We need to know the maximum machine type corresponding directly to
 * the machine registers:
 */
BE_EXPORTED extern CLASS_INDEX Max_Int_Mtype, Max_Uint_Mtype;

/* We need to know the default machine type to be used for internally
 * generated integers, e.g. DO loop indices.  These will default to the
 * max int types above, but the FEs may override the defaults:
 */
BE_EXPORTED extern CLASS_INDEX Def_Int_Mtype, Def_Uint_Mtype;

/* =================== */
/* Target Pointer Size */
/* =================== */

/* On MIPS III targets, should we use 32-bit pointers? */
BE_EXPORTED extern BOOL  Use_32_Bit_Pointers;	/* -Tc.P32 */

/* For various targets, what are the characteristics of pointers */
BE_EXPORTED extern INT		Pointer_Size;	/* in bytes */
BE_EXPORTED extern CLASS_INDEX	Pointer_Mtype;
BE_EXPORTED extern CLASS_INDEX	Pointer_Mtype2;

/* What are pointers and booleans aliased to in WHIRL */
BE_EXPORTED extern TYPE_ID Pointer_type;
BE_EXPORTED extern TYPE_ID Pointer_type2;
BE_EXPORTED extern TYPE_ID Boolean_type;
BE_EXPORTED extern TYPE_ID Boolean_type2;
BE_EXPORTED extern TYPE_ID Integer_type;

/* ====================================================================
 *
 * Skip option interface
 *
 * For debugging purposes, we support the suppression (skipping) of
 * optimization for specific PUs in a compilation (e.g. in WOPT and in
 * IPA).  This is controlled by specifying a range (before/after) or
 * item (ident) of numbers to skip, which produces a group option list
 * from the group processing in flags.c.  The support here provides for
 * (1) converting the group option list to a form easier to query, and
 * (2) querying the resulting skip list.
 *
 * ====================================================================
 */

typedef struct skiplist SKIPLIST;
struct option_list;

/* Convert an option list to a SKIPLIST: */
BE_EXPORTED SKIPLIST *Build_Skiplist ( struct option_list *olist );

/* Query a SKIPLIST -- result TRUE means element is in list: */
BE_EXPORTED BOOL Query_Skiplist ( SKIPLIST *slist, INT32 elmt );

/* SKIPLIST for All Optimizations */
BE_EXPORTED extern SKIPLIST *Optimization_Skip_List;     /* Processed list */
BE_EXPORTED extern SKIPLIST *Region_Skip_List;	     /* regions to skip, processed */

/* ====================================================================
 *
 * Initialization interface
 *
 * ====================================================================
 */

/***** Perform configuration functions prior to flag processing *****/
BE_EXPORTED extern void Preconfigure (void);

/***** Perform configuration functions after flag processing *****/
BE_EXPORTED extern void Configure (void);

/***** Perform configuration functions for each source file *****/
BE_EXPORTED extern void Configure_Source ( char *filename );

/***** Perform configuration functions for the alias analysis options *****/
BE_EXPORTED extern void Configure_Alias_Options (struct option_list *);

BE_EXPORTED extern void Configure_Feedback_Options (struct option_list *);


/***** Process a trace option string *****/
BE_EXPORTED extern BOOL Process_Trace_Option ( char *option );

/***** List options to the given file *****/
BE_EXPORTED extern void List_Compile_Options (
  FILE *file,		/* File to which to write */
  char *pfx,		/* Prefix output with this string */
  BOOL internal,	/* Internal or user listing? */
  BOOL full_list,	/* Groups (user)?  All options (internal)? */
  BOOL update );	/* Reset option changed/modified flags? */


#ifndef Is_Target_R4K
#define Is_Target_R4K()		(0)
#endif
#ifndef Is_Target_R5K
#define Is_Target_R5K()		(0)
#endif
#ifndef Is_Target_R8K
#define Is_Target_R8K()		(0)
#endif
#ifndef Is_Target_R10K
#define Is_Target_R10K()	(0)
#endif
#ifndef Is_Target_TP
#define Is_Target_TP()		Is_Target_R8K()
#endif
#ifndef Is_Target_T5
#define Is_Target_T5()		Is_Target_R10K()
#endif
#ifndef Is_Target_Pentium
#define Is_Target_Pentium()	(0)
#endif
#ifndef Is_Target_Itanium
#define Is_Target_Itanium()	(0)
#endif

#ifndef Is_Target_ISA_M1
#define Is_Target_ISA_M1()	(0)
#endif
#ifndef Is_Target_ISA_M2
#define Is_Target_ISA_M2()	(0)
#endif
#ifndef Is_Target_ISA_M3
#define Is_Target_ISA_M3()	(0)
#endif
#ifndef Is_Target_ISA_M4
#define Is_Target_ISA_M4()	(0)
#endif
#ifndef Is_Target_ISA_M2Plus
#define Is_Target_ISA_M2Plus()	(0)
#endif
#ifndef Is_Target_ISA_M3Plus
#define Is_Target_ISA_M3Plus()	(0)
#endif
#ifndef Is_Target_ISA_M4Plus
#define Is_Target_ISA_M4Plus()	(0)
#endif
#ifndef Is_Target_ISA_I1
#define Is_Target_ISA_I1()	(0)
#endif
#ifndef Is_Target_ISA_I1Plus
#define Is_Target_ISA_I1Plus()	(0)
#endif

/* 
 * Arthur: moved some stuff here from ia64/config_targ.h that
 *         looks like it might be shared by all targets
 */

/* ================== */
/* Comparison Results */
/* ================== */

/* For various targets, what is the comparison result type? */
BE_EXPORTED extern INT		Comparison_Result_Size;	/* in bytes */
BE_EXPORTED extern CLASS_INDEX	Comparison_Result_Mtype;

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
BE_EXPORTED extern INT16 FP_Exception_Enable_Max;	/* Max FP trap enables */
BE_EXPORTED extern INT16 FP_Exception_Enable_Min;	/* Min FP trap enables */

/* Is the "char" type signed? */
BE_EXPORTED extern BOOL  Char_Type_Is_Signed;

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

BE_EXPORTED extern INT16 Symbolic_Debug_Mode;	/* Current */
BE_EXPORTED extern INT16 Max_Symbolic_Debug_Mode;	/* Maximum for any PU */

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

/* Can 64-bit values be 4-byte aligned in memory? */
BE_EXPORTED extern BOOL Allow_Word_Aligned_Doubles;

/* Should we generate position-independent code by default? */
BE_EXPORTED extern BOOL Generate_Position_Independent_Code;

/* Split 64-bit integer ops into 32-bit ops, and simulate them? */
BE_EXPORTED extern BOOL Split_64_Bit_Int_Ops;

/* Split quad-precision ops into double-precision, and simulate them? */
BE_EXPORTED extern BOOL Split_Quad_Ops;

/* Should we simulate 32-bit subprogram interface by default? */
BE_EXPORTED extern BOOL Simulate_32_Bit_Interface;

/* Workaround for TFP branch cache problem. */
BE_EXPORTED extern BOOL No_Quad_Aligned_Branch;
BE_EXPORTED extern BOOL No_Quad_Aligned_Branch;

/* Does target provides only unsigned 64-bit instructions? */
BE_EXPORTED extern BOOL Only_Unsigned_64_Bit_Ops;

#ifdef TARG_ST
/* Does target provide only 32-bit instructions? */
BE_EXPORTED extern BOOL Only_32_Bit_Ops;
/* Does target support single and double floating point */
BE_EXPORTED extern BOOL Emulate_FloatingPoint_Ops;
BE_EXPORTED extern BOOL Emulate_FloatingPoint_Ops_Set;
/* Does target support single floating point */
BE_EXPORTED extern BOOL Emulate_Single_Float_Ops;
BE_EXPORTED extern BOOL Emulate_Single_Float_Ops_Set;
/* Does target support double floating point */
BE_EXPORTED extern BOOL Emulate_Double_Float_Ops;
BE_EXPORTED extern BOOL Emulate_Double_Float_Ops_Set;
/* Does target support integer division and modulus operations */
BE_EXPORTED extern BOOL Emulate_DivRem_Integer_Ops;
BE_EXPORTED extern BOOL Emulate_DivRem_Integer_Ops_Set;
#endif

#ifdef TARG_ST
/* Enable expansion of front-end builtins. */
BE_EXPORTED extern BOOL Enable_Expand_Builtin;
BE_EXPORTED extern BOOL Enable_Expand_Builtin_Set;
#endif

BE_EXPORTED extern INT32 Align_Instructions;	/* Align frequent blocks and funcs */
#ifdef TARG_ST
BE_EXPORTED extern INT32 Align_Functions;	/* Align funcs, has priority over Align_Instructions. */
BE_EXPORTED extern INT32 Align_Loops;		/* Align loops, has priority over Align_Instructions. */
BE_EXPORTED extern INT32 Align_Labels;		/* Align all labels, has priority over Align_Instructions. */
BE_EXPORTED extern INT32 Align_Jumps;		/* Align labels reached by jump, has priority over Align_Instructions and Align_Labels. */
#endif

BE_EXPORTED extern BOOL Eager_Bottom_Load;
BE_EXPORTED extern BOOL Eager_Ptr_Deref;
BE_EXPORTED extern BOOL Eager_Null_Ptr_Deref;

/* file has non-standard gp groups, i.e. SYMTAB_gp_group(Global_Symtab) */
BE_EXPORTED extern BOOL Has_GP_Groups;

/* Does target have offsets in load and store instructions?
 * Note: CG should instead test:
 * ( TOP_Find_Operand_Use( OP_code(op), OU_offset ) >= 0 ) */
BE_EXPORTED extern BOOL Use_Load_Store_Offset;


/* ====================================================================
 *
 * Initialization interface
 *
 * ====================================================================
 */

/* Configuration prior to flag processing: */
BE_EXPORTED extern void Preconfigure_Target (void);

/* Configuration after flag processing: */
BE_EXPORTED extern void Configure_Target (void);
BE_EXPORTED extern void IPA_Configure_Target (void);

/* Reconfiguration for each source file: */
BE_EXPORTED extern void Configure_Source_Target ( char *filename );

/* return FALSE if abi mismatch */
BE_EXPORTED extern BOOL Set_Target_ABI (BOOL is_64bit, INT isa);


#ifdef __cplusplus
}
#endif
#endif /* config_INCLUDED */
