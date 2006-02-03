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


#ifdef TARG_ST
// [CL] new interface with ipa_link
#define LD_IPA_INTERFACE
#endif

#include "linker.h"			/* linker headers */
#include "process.h"                    /* For create_tmpdir. */

#include "errors.h"			/* for Set_Error_Phase() */
#include "glob.h"			/* for Cleanup_Files() */
#include "config.h"			/* for Preconfigure() */
#include "config_list.h" 
#ifdef TARG_ST 
#include "config_target.h"
#else
#include "config_targ.h"		/* for Target_ABI */
#endif
#include "wn.h"				/* for Dont_Use_WN_Free_List() */

#include "ipc_defs.h"			/* IP_32_bit_ABI */
#include "ipa_option.h"			/* Process_IPA_Options () */
#include "ipo_main.h"		/* Perform_Interprocedural_Optimization */
#include "ipc_symtab_merge.h"		// Initialize_Auxiliary_Tables ()
#include "ipc_type_merge.h"		// for merging types
#include "ipc_main.h"
#include "ipc_pic.h"			// for Global_symbol_optimization ()
#include "ld_ipa_interface.h"		// for ld_for_all_ST ()

#include "ipc_weak.h"


/***************************************************************************/
/* gets the ABI type from the linker                */
/***************************************************************************/
static void 
IP_set_target(void)
{
#ifdef _TARG_MIPS
    switch (ld_ipa_opt[LD_IPA_TARGOS].flag) {
    case TOS_MIPS_O32:
	Target_ABI = ABI_32;
	break;
	
    case TOS_MIPS_N32:
	Target_ABI = ABI_N32;
	break;

    case TOS_MIPS_64:
	Target_ABI = ABI_64;
	break;
    default:
	Target_ABI = ABI_N32;
	break;
    }

    switch (ld_ipa_opt[LD_IPA_ISA].flag) {
    case 1:
	Target_ISA = TARGET_ISA_M1;
	break;
    case 2:
	Target_ISA = TARGET_ISA_M2;
	break;
    case 3:
	Target_ISA = TARGET_ISA_M3;
	break;
    case 4:
	Target_ISA = TARGET_ISA_M4;
	break;
    default:
	break;				// use default
    }

    Use_32_Bit_Pointers = (Target_ABI < ABI_64);
#endif

#ifdef TARG_IA64
    Target_ABI = ABI_I64;
    Target_ISA = TARGET_ISA_I1;
    Use_32_Bit_Pointers = FALSE;
#endif

#ifdef _TARG_IA32
    Target_ABI = ABI_I32;
    Target_ISA = TRUE;
#endif

    // [CL] Target_ABI and ISA have already been set up
    // when input file was opened
    IPA_Configure_Target ();
}


static MEM_POOL Type_Merge_Pool;

static BOOL ipa_dot_so_initialized = FALSE;

#ifdef TARG_ST
// [CL] new interface with ipa_link: instead of weak symbols in
// ipa.so resolved by ipa_link, we define pointer to these symbols,
// and initialize them through this function, which is called
// by ipa_link when it opens ipa.so
void ipa_initialize_external_symbols(struct external_symbols_list* ipa_external_symbol_list)
{
  ipa_external_symbols = *ipa_external_symbol_list;

  /* Sanity checks: check the size and the version of the interface
     structure */
  if (
      (ipa_external_symbol_list->size !=
       sizeof(struct external_symbols_list)) ||
      (ipa_external_symbol_list->version !=
       IPA_EXTERNAL_SYMBOLS_VERSION)
      ) {
    ErrMsg (EC_Ipa_Incompatible, ipa_external_symbol_list->size,
	    ipa_external_symbol_list->version,
	    sizeof(struct external_symbols_list),
	    IPA_EXTERNAL_SYMBOLS_VERSION);
  }
}
#endif

void
#ifdef TARG_ST
ipa_dot_so_init (INT argc, char **argv)
#else
ipa_dot_so_init ()
#endif
{
    Temporary_Error_Phase ephase ("IPA initialization");

    ipa_dot_so_initialized = TRUE;

    MEM_Initialize();
    Preconfigure ();
    IP_set_target();
    Dont_Use_WN_Free_List ();

    Init_Operator_To_Opcode_Table ();
    Initialize_Symbol_Tables (TRUE);
    Initialize_Auxiliary_Tables ();
    
    MEM_POOL_Initialize (&Type_Merge_Pool, "TY Merge Pool", 0);
    Initialize_Type_Merging_Hash_Tables (&Type_Merge_Pool);
	
    Set_FILE_INFO_ipa (File_info);	// mark the symtab IPA-generated

#ifdef TARG_ST
    // [CL] all this processing should be made
    // upon initialization, so that proper 
    // code generation options are selected before
    // opening input files. This enables sanity
    // checks at link time.

    // turn off these features until they are ported.
    IPA_Enable_Cloning = FALSE;
    IPA_Enable_AutoGnum = TRUE;
#if 1
    IPA_Enable_DST = FALSE;
#else
    IPA_Enable_DST = TRUE;
#endif

    Process_IPA_Options (argc, argv);
#endif

    if (ld_ipa_opt[LD_IPA_SHARABLE].flag == F_CALL_SHARED_RELOC)
        IPA_Enable_Relocatable_Opt = TRUE;

} /* ipa_dot_so_init */

void
ipa_driver (INT argc, char **argv)
{
    if (! ipa_dot_so_initialized)
	// not a single WHIRL object has been found.
#ifdef TARG_ST
	ipa_dot_so_init (argc, argv);
#else
	ipa_dot_so_init ();
#endif

    Verify_Common_Block_Layout ();
	
    Clear_Extra_Auxiliary_Tables ();
    
    MEM_POOL_Delete (&Type_Merge_Pool);

#ifndef TARG_ST
    // [CL] all this processing should be made
    // upon initialization, so that proper 
    // code generation options are selected before
    // opening input files. This enables sanity
    // checks at link time.

    // turn off these features until they are ported.
    IPA_Enable_Cloning = FALSE;
    IPA_Enable_AutoGnum = TRUE;
#if 1
    IPA_Enable_DST = FALSE;
#else
    IPA_Enable_DST = TRUE;
#endif

    Process_IPA_Options (argc, argv);
#endif

    create_tmpdir ( Tracing_Enabled || List_Cite );

#ifdef TODO
    if (IPA_Target_Type == IP_32_bit_ABI)
        IP_update_space_status32();
    else
        IP_update_space_status64();
#endif

    if (ld_ipa_opt[LD_IPA_SHARABLE].flag & F_STATIC) {
	IPA_Enable_Picopt = FALSE;
	IPA_Enable_AutoGnum = FALSE;
    }

    if (IPA_Enable_Picopt || IPA_Enable_Relocatable_Opt) {
	Pic_optimization ();
    } else {
	Fix_up_static_functions ();
    }

    Perform_Interprocedural_Optimization ();
   
} /* ipa_driver */



/* preempt the definition in be.so, so that we can call ld's cleanup
   routines */
/*ARGSUSED*/
void
#if defined(TARG_ST) && (defined(__CYGWIN__) || defined(__MINGW32__))
// [CL] no preemption possible under Cygwin
Signal_Cleanup_IPA (INT sig)
#else
Signal_Cleanup (INT sig)
#endif
{
    Cleanup_Files (FALSE, TRUE);

    /* now do the ld part */
    /* we fake a fatal error instead of copying all the cleanup code here */
#if defined(TARG_IA64) || defined(TARG_ST)
    fprintf(stderr,"IPA processing aborted");
    exit(1);
#else
    msg (ER_FATAL, ERN_MESSAGE, "IPA processing aborted");
#endif
} /* Signal_Cleanup */
