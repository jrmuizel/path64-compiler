/*
 *  Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
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


#include "basic.h"

/* values of options */

#define UNDEFINED	-1	/* for any undefined option */

#define ENDIAN_BIG	0
#define ENDIAN_LITTLE	1

#define EXTENDED_ANSI	0
#define KR_ANSI		1
#define STRICT_ANSI	2
#define POSIX		3

#define CALL_SHARED	0
#define NON_SHARED	1
#define DSO_SHARED	2
#define RELOCATABLE	4

#define NORMAL_MP	0
#define CRAY_MP		1

#ifdef TARG_ST
// [CG]: Define C standards (-std=... option)
#define C_STD_UNDEF	0
#define C_STD_C89	1
#define C_STD_C94	2
#define C_STD_C99	3
#define C_STD_GNU89	4
#define C_STD_GNU99	5
#define C_STD_CXX98	6   /* (cbr) for cplusplus */
#define C_STD_GNU98	7   /* (cbr) for cplusplus */
extern int c_std;

#define STRICT_ALIAS    1
#define NOSTRICT_ALIAS  2
#endif

/* What are the supported platforms? */
typedef enum {
  IP0,		/* Unknown processor */
  IP19,		/* 	    R4000 Challenge */
  IP20,		/* Blackjack: R4000 Indigo */
  IP21,		/* Everest: R8000 Power Challenge */
  IP22_4K,	/*	    R4x00 Indy/Indigo2 */
  IP22_5K,	/* Guiness: R5000 Indy */
  IP24,		/* Guiness: R5000 Indy (same as IP22_5K) */
  IP25,		/* Shiva:   R10000 Power Challenge */
  IP26,		/* Teton:   R8000 Indigo2 */
  IP27,		/* Lego:    R10000 Origin 2000 */
  IP28,		/* Pacecar: R10000 Indigo2 */
  IP30,		/* TopCat:  R10000 Octane, a.k.a. Speedracer */
  IP32_5K,	/* Moosehead: R5000 O2 */
  IP32_10K,	/* Moosehead: R10000 O2 */
  IP_END	/* End of platform list */
} PLATFORM;

typedef enum {
  M_CPU,
  M_ARCH,
  M_TUNE,
} m_flag;

/* What are the supported platforms (for driver): */
typedef enum {
  PROC_NONE	= 0,
  PROC_R3K	= 3,
  PROC_R4K	= 4,
  PROC_R5K	= 5,
  PROC_R8K	= 8,
  PROC_R10K	= 10,
  PROC_R12K	= 12,
  PROC_ITANIUM	= 20,
  PROC_ST100    = 30,
  PROC_ST210    = 40,
  PROC_ST220    = 41,
  PROC_ST231    = 44,
  PROC_ST240    = 46,
  PROC_stxp70_v3    = 47,
  PROC_stxp70_v3_ext= 48,
  PROC_stxp70_v4_novliw = 49,
  PROC_stxp70_v4_single = 50,
  PROC_stxp70_v4_dual = 51,
  PROC_armv5     = 52,
  PROC_armv6     = 53,
} PROCESSOR;
extern PLATFORM Platform;	/* Target platform */

/* How should we set the options? */
typedef struct {
  PLATFORM id;		/* Which IP? */
  char *name;		/* "ipxx" */
  char *pname;		/* "r10000" */
  PROCESSOR processor;	/* PROC_RxK */
  /* Add fields here for values of options which vary per processor */
  char *nickname;	/* "Shiva" */
} PLATFORM_OPTIONS;

#define POPTS_id(p)		((p)->id)
#define POPTS_name(p)		((p)->name)
#define POPTS_pname(p)		((p)->pname)
#define POPTS_processor(p)	((p)->processor)
#define POPTS_nickname(p)	((p)->nickname)

extern PLATFORM_OPTIONS * Get_Platform_Options ( char *name );


typedef enum {
  ABI_RAG32,
  ABI_N32,
  ABI_64,
  ABI_I32,
  ABI_I64,
  ABI_IA32,
  ABI_ST100,
  ABI_ST200_embedded,
  ABI_ST200_PIC,
  ABI_STxP70_embedded,
  ABI_STxP70_fpx,
  ABI_ARM_ver1,
  ABI_ARM_ver2,
} ABI;
typedef enum {
  ISA_NONE      = 0,
  ISA_MIPS1     = 1,
  ISA_MIPS2     = 2,
  ISA_MIPS3     = 3,
  ISA_MIPS4     = 4,
  ISA_MIPS5     = 5,
  ISA_MIPS6     = 6,
  ISA_MIPSX     = 9,
  ISA_MIPS64    = 10,
  ISA_IA641     = 11,
  ISA_IA32      = 12,
  ISA_X8664     = 13,
  ISA_ST100     = 14,
  ISA_ST210     = 15,
  ISA_ST220     = 16,
  ISA_stxp70_v3 = 17,
  ISA_stxp70_v3_ext = 18,
  ISA_stxp70_v4 = 19,
  ISA_armv5     = 20,
  ISA_armv6     = 21,

} ISA;
#ifdef MUMBLE_ARM_BSP
typedef enum {
  RUNTIME_NONE = 0,
  RUNTIME_BARE = 1
} RUNTIME;
extern RUNTIME arm_runtime;
#endif

extern boolean debug;		/* debugging turned on */

extern boolean nostdinc;	/* no standard include directory */
#ifdef TARG_ST
/* (cbr) don't exclude non c++ standard paths */
extern boolean nostdincc;	/* no C++ include directory */
#endif

#ifdef PATH64_ENABLE_PSCRUNTIME
extern boolean stdcxx_threadsafe; /* use thread-safe apache stdcxx library */
#endif

extern char *help_pattern;	/* pattern string for help file */

extern int inline_t;            /* toggle for inline options */

#ifdef KEY
/* Before front-end: UNDEFINED.  After front-end: TRUE if inliner will be run.
   Bug 11325. */
extern int run_inline;
#endif

extern boolean dashdash_flag;   /* when you see -- set this flag to
				   indicate the end of the options */

extern boolean read_stdin;	/* read stdin for input */

extern boolean  xpg_flag;

extern int default_olevel;	/* default optimization level */

extern int ofast;		/* Ofast has been set */

extern int ffast_math_prescan;	// Bug 14302: ffast_math set in prescan

extern int instrumentation_invoked;	/* Instrument whirl */

extern boolean ftz_crt;		/* add flush-to-zero crt */
#ifdef TARG_ST
extern char *print_name;
extern int print_kind;
#define PRINT_NONE 0
#define PRINT_FILE 1
#define PRINT_PROG 2

/* TB: to warm if olevel is not enough if uninitialized var warning is
   aked*/
extern boolean Wuninitialized_is_asked; 

#endif

#ifdef KEY
extern char *f90_module_dir;	/* value of -module option */	// bug 4210
extern int malloc_algorithm;
#endif

/* return whether has been toggled yet */
extern boolean is_toggled (int obj);

/* set obj to value; allow many toggles; last toggle is final value */
extern void toggle (int *obj, int value);

/* Options for configuring the target: */

/* Verify that the target selection is consistent and set defaults: */
extern void Check_Target ( void );

/* process -F option (different for Fortran and C++) */
extern void dash_F_option(void);

/* untoggle the object, so it can be re-toggled later */
extern void untoggle (int *obj, int value);

/* save value in string */
extern void save_name (char **obj, char *value);

/* do action associated with flag */
extern void opt_action (int optflag);

/* return whether to mark this flag as unseen */
extern boolean flag_is_superceded (int optflag);

#ifdef KEY /* bug 4260 */
extern void check_convert_name(char *name);
#endif /* KEY bug 4260 */

/* check if there is a -- and _XPG is set */
extern void check_dashdash ( void );

/* set options for DSM options */
extern void set_dsm_options (void);

extern void Process_Mp (void);
extern void Process_Cray_Mp (void);

extern void print_file_path (char *, int);	/* print path to named file */

extern int subverbose ;

extern char *target_cpu;
#ifdef TARG_ST200
typedef enum { OS21_TRACE_WRAP, OS21_TRACE_UNDEFINED } OS21_TRACE ;
extern int os21_trace_options_set() ;
extern int get_os21_trace_options_nelements(OS21_TRACE kind) ;
extern int get_os21_trace_options_elements(OS21_TRACE kind, char *elts[], size_t maxnelts) ;
extern int os21_profiler_options_set() ;
extern int get_os21_profiler_options_emit_undefined() ;
extern int get_os21_profiler_options_warn() ;
#endif
