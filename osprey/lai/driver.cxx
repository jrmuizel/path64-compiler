/*

  Copyright (C) 2001 STMicroelectronics, Inc.  All Rights Reserved.

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

  Contact information:  MDT/STMicroelectronics, Inc., Montbonnot,

  or:

  http://www.st.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


/* ====================================================================
 * ====================================================================
 *
 * Module: cgdriver.c
 * $Source$
 *
 * Description:
 *
 * Main driver -- command line processing and file name manipulation --
 * for the code generator.
 *
 * ====================================================================
 * ====================================================================
 */

#include <elf.h>
#include <sys/elf_whirl.h>	    /* for WHIRL_REVISION */
#include <ctype.h>
#include "defs.h"
#include "config.h"
#include "config_debug.h"
#include "config_list.h"
#include "config_TARG.h"
#include "config_opt.h"
#include "controls.h"
#include "flags.h"
#include "erglob.h"
#include "erlib.h"
#include "errors.h"
#include "file_util.h"
#include "glob.h"
#include "timing.h"
#include "tracing.h"
#include "util.h"
#include "mempool.h"

#include "wn.h"			    /* for WN */
#include "dwarf_DST_mem.h"

#include "lai.h"
#include "register.h"
#include "expand.h"
#include "emit.h"               /* LAI emmiter */

/*
MEM_POOL MEM_local_region_pool;	
MEM_POOL MEM_local_region_nz_pool;
*/

/* ====================================================================
 *
 *   Back end process-specific global data from glob.h.
 *
 * ====================================================================
 */


/* ====================================================================
 *
 *   Local data.
 *
 * ====================================================================
 */

static char *Argv0;		         /* argv[0] from main */

/* Default file	extensions: */
#define	ASM_FILE_EXTENSION ".s"	         /* Assembly code file */
#define LAI_FILE_EXTENSION ".lai"        /* LAI file */
#define	OBJ_FILE_EXTENSION ".o"	         /* Relocatable object file */
#define DSTDUMP_FILE_EXTENSION ".be.dst" /* DST dump-file extension */

/* Internal flags: */
static BOOL cg_opt_level_overridden = FALSE;

static BOOL LAI_tail_call_overridden = FALSE;

/* Keep	a copy of the command line options for assembly	output:	*/
static char *option_string;

/* Generic LAI options. */
static OPTION_DESC Options_LAI[] = {
  { OVK_BOOL,	OV_INTERNAL, TRUE, "warn_bad_freqs", "",
    0, 0, 0,	&LAI_warn_bad_freqs, NULL },
  { OVK_INT32,	OV_INTERNAL, TRUE, "skip_before", "skip_b",
    0, 0, INT32_MAX, &CG_skip_before, NULL }, 
  { OVK_INT32,	OV_INTERNAL, TRUE, "skip_after", "skip_a",
    0, 0, INT32_MAX, &CG_skip_after, NULL }, 
  { OVK_INT32,	OV_INTERNAL, TRUE, "skip_equal", "skip_e",
    0, 0, INT32_MAX, &CG_skip_equal, NULL }, 
  { OVK_INT32,	OV_INTERNAL, TRUE, "local_skip_before", "local_skip_b",
    0, 0, INT32_MAX, &CG_local_skip_before, NULL }, 
  { OVK_INT32,	OV_INTERNAL, TRUE, "local_skip_after", "local_skip_a",
    0, 0, INT32_MAX, &CG_local_skip_after, NULL }, 
  { OVK_INT32,	OV_INTERNAL, TRUE, "local_skip_equal", "local_skip_e",
    0, 0, INT32_MAX, &CG_local_skip_equal, NULL }, 
  { OVK_INT32,	OV_INTERNAL, TRUE, "optimization_level", "",
    0, 0, MAX_OPT_LEVEL,
                &CG_opt_level, &cg_opt_level_overridden },

  // CG Dependence Graph related options.

  // CGPREP options.

  { OVK_BOOL,	OV_INTERNAL, TRUE, "enable_feedback", "",
    0, 0, 0,	&LAI_enable_feedback, NULL },

  // Cross Iteration Loop Optimization options.

  // CG Unrolling options - see also OPT:unroll_times_max:unroll_size.

  // Control flow optimizations (CFLOW) options.

  { OVK_BOOL,	OV_INTERNAL, TRUE, "unique_exit", "",
    0, 0, 0,	&LAI_unique_exit, NULL },
  { OVK_BOOL,	OV_INTERNAL, TRUE, "tail_call", "",
    0, 0, 0,	&LAI_tail_call, &LAI_tail_call_overridden },

  // Frequency heuristic/feedback options.

  { OVK_BOOL,	OV_INTERNAL, TRUE,"enable_frequency", "",
    0, 0, 0, &FREQ_enable, NULL },

  // Whirl2ops / Expander options.

  { OVK_BOOL,	OV_INTERNAL, TRUE,"normalize_logical", "normalize",
    0, 0, 0, &EXP_normalize_logical, NULL },
  { OVK_NAME,	OV_INTERNAL, TRUE,"sqrt_algorithm", "sqrt",
    0, 0, 0, &EXP_sqrt_algorithm, NULL },

  // Global Code Motion (GCM) options.

  // Emit options:

  { OVK_BOOL,   OV_INTERNAL, TRUE,  "emit_asm_dwarf", "",
    0,0,0,      &LAI_emit_asm_dwarf, NULL,
    "Turn on/off emission of dwarf data into .lai file [Default OFF]"
  },
  { OVK_BOOL,   OV_INTERNAL, TRUE,  "emit_unwind_directives", "",
    0,0,0,      &LAI_emit_unwind_directives, NULL,
    "Turn on/off emission of unwind directives into .lai file [Default OFF]"
  },
  { OVK_BOOL,   OV_INTERNAL, TRUE,  "emit_unwind_info", "",
    0,0,0,      &LAI_emit_unwind_info, NULL,
    "Turn on/off emission of unwind into .lai file [Default OFF]"
  },

  { OVK_COUNT }
};


OPTION_GROUP LAI_Option_Groups[] = {
  { "LAI", ':', '=', Options_LAI },
  { NULL }		/* List terminator -- must be last */
};


/* =======================================================================
 *
 *  Configure_LAI_Options
 *
 *  After the comand line has been processed and LAI_opt_level set, configure
 *  the various LAI flags that depend on these two things.
 *  This is also called per PU if the PU opt level changes.
 *
 * =======================================================================
 */
static void
Configure_LAI_Options(void)
{
  LAI_localize_tns = (CG_opt_level <= 1);

  return;
}

/* =======================================================================
 *  LAI_Configure_Opt_Level
 *
 *  See interface description.
 * =======================================================================
 */
void
LAI_Configure_Opt_Level ( 
  INT opt_level 
)
{
  static BOOL opt_level_configured = FALSE;

  if ( opt_level_configured && opt_level == CG_opt_level )
    return;

  if ( opt_level_configured && cg_opt_level_overridden ) {
    /* forget it */
    DevWarn("Attempt to override CG:opt_level=%d flag. Ignored.",CG_opt_level);
    return;
  }

  opt_level_configured = TRUE;

  if ( ! cg_opt_level_overridden )
    CG_opt_level = opt_level;

  Configure_LAI_Options();
}

/* ====================================================================
 *
 * Build_Option_String
 *
 * Just	build a	string of all the options passed to the	Common Core
 * process, so the options can be printed at the beginning of the "*.s"
 * file(s).
 *
 * ====================================================================
 */

static void
Build_Option_String (INT argc, char **argv)
{
    INT16 i;
    INT16 arg_size = 0;

    Argv0 = argv[0];		    /* save argv[0] for R_Assemble_File() */

    for (i=1; i<argc; ++i)	    /* skip arg 0 (the program name) */
	if ( argv[i][0] == '-'  && argv[i][1] != 'f')
	    arg_size += ( strlen(argv[i]) + 1 );

    if ( arg_size > 0 ) {
	register char *p;
	
	p = option_string = (char *) malloc(arg_size+1);

	if ( option_string == NULL ) {
	    ErrMsg ( EC_No_Mem, "Build_Option_String" );
	    exit ( 1 );
	}

	p[0] = '\0';
	for (i=1; i<argc; ++i)
	    if ( argv[i][0] == '-'  && argv[i][1] != 'f') {
		register INT len = strlen (argv[i]) + 1;
		if (p != option_string)
		    *p++ = ' ';
		bcopy (argv[i], p, len);
		p += len - 1;
	    }
	
    } else {			    /* no options specified */
	option_string = "none";
    }
}

/* ====================================================================
 *   Process_Command_Line
 *
 *   Process the command line arguments specific to LAI.
 * ====================================================================
 */

static void
Process_Command_Line (
  INT argc, 
  char **argv
)
{
  INT16 i;
  char *cp;

  /* Check the command line flags: */
  for ( i=0; i<argc; i++ ) {
    if ( argv[i] != NULL && *(argv[i]) == '-' ) {
      cp = argv[i]+1;	    /* Pointer to next flag character */

      /* First try to process as command-line option group */
      if (Process_Command_Line_Group(cp, LAI_Option_Groups))
	continue;

      switch ( *cp++ ) {
        case 'f':		    /* file options */
	  /* error case already handled by main driver */
	  switch (*cp) {
	    case 'L':	            /* Lai file */
	      Lai_File_Name = cp + 2;
	      break;
	  }
	  break;
        default:
	  ErrMsg (EC_Unknown_Flag, *(cp-1), argv[i]);
      }
    }
  }

  return;
}

/* ====================================================================
 *   Prepare_Source
 *
 *   Process the source argument and associated files.
 * ====================================================================
 */
static void
Prepare_Source (void)
{
  char *fname;

  /* We've got a source file name -- open other files.
   * We want them to be created in the current directory, so we
   * strip off the filename only from Src_File_Name for use:
   */
  fname = Last_Pathname_Component ( Src_File_Name );

  if (Lai_File_Name == NULL) {
    /* Replace source file extension to get assembly file name: */
    Lai_File_Name = New_Extension (fname, LAI_FILE_EXTENSION);
  }

  /* Open the LAI file for compilation: */
  if ((Lai_File = fopen (Lai_File_Name, "w")) == NULL) {
    ErrMsg (EC_Asm_Open, Lai_File_Name, errno);
    Terminate (1);
  }

  return;
}

/* ====================================================================
 *   LAI_Process_Command_Line (lai_argc, lai_argv, be_argc, be_argv)
 *
 *   Main entry point and driver for the Code Generator.
 * ====================================================================
 */
void
LAI_Process_Command_Line (
  INT lai_argc, 
  char **lai_argv, 
  INT be_argc, 
  char **be_argv
)
{
  extern char *Whirl_Revision;

  if (strcmp (Whirl_Revision, WHIRL_REVISION) != 0)
    FmtAssert (!DEBUG_Ir_Version_Check,
	       ("WHIRL revision mismatch between be.so (%s) and cg.so (%s)",
		Whirl_Revision, WHIRL_REVISION));

  /* Perform preliminary command line processing: */
  Build_Option_String (be_argc, be_argv);
  Process_Command_Line (lai_argc, lai_argv);

  LAI_Configure_Opt_Level(Opt_Level);

  Prepare_Source ();
  return;
} /* LAI_Process_Command_Line */

/* ====================================================================
 *   LAI_Init ()
 *
 *   Initialization that needs to be done after the global symtab is 
 *   read 
 * ====================================================================
 */
void
LAI_Init (void)
{
  Set_Error_Phase ( "LAI Emission Initialization" );

  MEM_POOL_Initialize (&MEM_local_region_pool, "local_region_pool", TRUE);
  MEM_POOL_Initialize (&MEM_local_region_nz_pool, "local_region_nz_pool", FALSE);

  REGISTER_Begin();	/* initialize the register package */
  Init_Dedicated_TNs ();
  Mark_Specified_Registers_As_Not_Allocatable ();
  LAI_Begin_File (Argv0, option_string);

  // Initialize TCON symbolic names table:
  Init_Tcon_Info ();

  return;
}

/* ====================================================================
 *   LAI_Fini ()
 *
 *   Terimination routines for cg 
 * ====================================================================
 */
void
LAI_Fini (void)
{
  /* List global symbols if desired: */
  if (List_Symbols) {
    Print_global_symtab (Lst_File);
  }

  Set_Error_Phase ( "LAI Emit" );

  /* Finish off the relocatable object file: */
  LAI_End_File();

  // Finish off the TCON to symbolic names table:
  Fini_Tcon_Info ();

  MEM_POOL_Delete (&MEM_local_region_pool);
  MEM_POOL_Delete (&MEM_local_region_nz_pool);

  return;
} 

