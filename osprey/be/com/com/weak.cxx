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


// This file contains only Linux-specific code and should be entirely
// #ifdef'd out for Irix.

// [CL] use the "undefined weak symbol" workaround on all our hosts
#if defined(TARG_ST) || defined(__linux__) || defined(_NO_WEAK_SUPPORT_)

// Work around the "undefined weak symbol" bug in Linux.
//
// The run-time linker in Linux only resolves undefined weak symbols once
// at start-up time of a process.  When such symbols cannot be resolved, it
// (correctly) sets their addresses to zero.  However, if later a DSO is
// linked in via "dlopen", and if now this DSO defines some of the
// previously undefined weak symbols, the Linux run-time linker should, but
// in this case does not, update their addresses.
//
// A workaround is to *not* reference these weak symbol directly but always 
// through a pointer, and explicitly initialized the pointer when the DSO
// is linked in.  For example:
//
// In the main program where the symbol (my_data) is referenced:
//
// #ifdef __linux__
// int* my_data_p;
// #define my_data (*my_data_p)
// #else
// extern int my_data;
// #pragma weak my_data
// #endif
//
// In the dso where the symbol is defined:
//
// int my_data;
//
// #ifdef __linux__
// extern int* my_data_p;
// void __init ()
// {
//     my_data_p = &my_data;
// }
//
// __init() is a special function the Linux linker recognizes and places in 
// .init section (or equivalent?) of the dso.  The runtime linker will call 
// this function upon mapping in this dso.

#include "defs.h"
#include "wn.h"
#include "pu_info.h"
#include "ir_bwrite.h"

// ----------------------------------------------------------------------
// symbols defined in lno.so:
// from be/lno/lnodriver.h

BE_EXPORTED void (*lno_main_p) (INT, char**, INT, char**);
BE_EXPORTED void (*Lno_Init_p) ();
BE_EXPORTED void (*Lno_Fini_p) ();
BE_EXPORTED WN* (*Perform_Loop_Nest_Optimization_p) (PU_Info*, WN*, WN*, BOOL);

// ----------------------------------------------------------------------
// symbols defined in wopt.so:

#include "optimizer.h"
#include "opt_defs.h"
#include "opt_wn.h"
#include "opt_bb.h"

// from be/opt/wodriver.h

BE_EXPORTED void (*wopt_main_p) (INT argc, char **argv, INT, char **);
BE_EXPORTED void (*Wopt_Init_p) ();
BE_EXPORTED void (*Wopt_Fini_p) ();
BE_EXPORTED WN* (*Perform_Preopt_Optimization_p) (WN *, WN *);
BE_EXPORTED WN* (*Perform_Global_Optimization_p) (WN *, WN *, struct ALIAS_MANAGER *);

// from be/opt/optimizer.h
BE_EXPORTED WN* (*Pre_Optimizer_p) (INT32, WN*, DU_MANAGER*, ALIAS_MANAGER*);
BE_EXPORTED DU_MANAGER* (*Create_Du_Manager_p) (MEM_POOL *);
BE_EXPORTED void (*Delete_Du_Manager_p) (DU_MANAGER *, MEM_POOL *);

// from be/com/opt_alias_interface.h
BE_EXPORTED BOOL (*Verify_alias_p) (ALIAS_MANAGER *, WN *);

// from be/opt/opt_alias_analysis.cxx
BE_EXPORTED void (*Print_points_to_p) (FILE *fp, POINTS_TO *ptmp);

// from be/opt/opt_wn.h
BE_EXPORTED AUX_ID (*WN_aux_p) (const WN*);

// from be/opt/opt_du.h
BE_EXPORTED BOOL (DU_MANAGER::*CD_is_br_taken_p) (IDTYPE);
BE_EXPORTED BOOL (DU_MANAGER::*CD_is_fall_thru_p) (IDTYPE);
BE_EXPORTED BOOL (DU_MANAGER::*Dominate_p) (IDTYPE, IDTYPE);
BE_EXPORTED IDTYPE (DU_MANAGER::*Get_cd_p) (IDTYPE);
BE_EXPORTED WN* (DU_MANAGER::*Get_last_stmt_p) (IDTYPE);

// from be/opt/opt_htable.h
BE_EXPORTED BOOL (CODEREP::*Def_at_entry_p) () const;
BE_EXPORTED BB_NODE* (CODEREP::*Defbb_p) () const;

// ----------------------------------------------------------------------
// symbols defined in cg.so:
// from be/cg/cgdriver.h

BE_EXPORTED void (*CG_Init_p) ();
BE_EXPORTED void (*CG_Fini_p) ();
BE_EXPORTED void (*CG_foo_p) ();
BE_EXPORTED void (*CG_Process_Command_Line_p) (INT, char **, INT, char **);

// from be/cg/cg.h
BE_EXPORTED void (*CG_PU_Initialize_p) (WN*);
BE_EXPORTED void (*CG_PU_Finalize_p) ();
BE_EXPORTED void (*CG_Change_Elf_Symbol_To_Undefined_p) (ST*);
BE_EXPORTED WN* (*CG_Generate_Code_p) (WN*, ALIAS_MANAGER*, DST_IDX, BOOL);
BE_EXPORTED void (*CG_Dump_Region_p) (FILE*, WN*);

// from be/cg/eh_region.h
BE_EXPORTED void (*EH_Generate_Range_List_p) (WN *);

// ----------------------------------------------------------------------
// symbols defined in ipl.so:
// from ipa/local/ipl_main.cxx

BE_EXPORTED void (*Ipl_Extra_Output_p) (Output_File *);
BE_EXPORTED void (*Ipl_Init_p) ();
BE_EXPORTED void (*Ipl_Fini_p) ();
BE_EXPORTED void (*ipl_main_p) (INT, char **);
BE_EXPORTED void (*Perform_Procedure_Summary_Phase_p) (WN*, DU_MANAGER*, ALIAS_MANAGER*,
					   void*);

// symbols from whirl2c.so 

BE_EXPORTED void (*W2C_Cleanup_p)(void);
BE_EXPORTED void (*W2C_Fini_p)(void);
BE_EXPORTED void (*W2C_Init_p)(void);
BE_EXPORTED const char * (*W2C_Object_Name_p)(const ST *func_st);
BE_EXPORTED void (*W2C_Mark_Loads_p)(void);
BE_EXPORTED void (*W2C_Nomark_Loads_p)(void);
BE_EXPORTED void (*W2C_Outfile_Fini_p)(BOOL emit_global_decls);
BE_EXPORTED void (*W2C_Outfile_Init_p)(BOOL emit_global_decls);
BE_EXPORTED void (*W2C_Outfile_Translate_Pu_p)(WN *pu, BOOL emit_global_decls);
BE_EXPORTED void (*W2C_Pop_PU_p)(void);
BE_EXPORTED void (*W2C_Process_Command_Line_p)(INT phase_argc, char * const
  phase_argv[], INT argc, char * const argv[]);
BE_EXPORTED void (*W2C_Push_PU_p)(const WN *pu, WN *body_part_of_interest);
BE_EXPORTED void (*W2C_Set_Prompf_Emission_p)(const WN_MAP *construct_map);
BE_EXPORTED void (*W2C_Set_Frequency_Map_p)(WN_MAP frequency_map);
BE_EXPORTED const char *(*W2C_Get_Transformed_Src_Path_p)(void);
BE_EXPORTED void (*W2C_Set_Purple_Emission_p)(void);
BE_EXPORTED void (*W2C_Reset_Purple_Emission_p)(void);
BE_EXPORTED BOOL (*W2C_Should_Emit_Nested_PUs_p)(void);
BE_EXPORTED void (*W2C_Translate_Global_Defs_p)(FILE *outfile);
BE_EXPORTED void (*W2C_Translate_Global_Types_p)(FILE *outfile);
BE_EXPORTED void (*W2C_Translate_Istore_Lhs_p)(char *strbuf,
  UINT bufsize, const WN* lhs, mINT64 istore_ofst, TY_IDX istore_addr_ty,
  TYPE_ID istore_mtype);
BE_EXPORTED void (*W2C_Translate_Purple_Main_p)(FILE *outfile, const WN *pu,
  const char *region_name);
BE_EXPORTED void (*W2C_Translate_Stid_Lhs_p)(char *strbuf, UINT bufsize,
  const ST *stid_st, mINT64 stid_ofst, TY_IDX stid_ty, TYPE_ID stid_mtype);
BE_EXPORTED void (*W2C_Translate_Wn_p)(FILE *outfile, const WN *wn);
BE_EXPORTED void (*W2C_Translate_Wn_Str_p)(char *strbuf, UINT bufsize, const
  WN *wn);

// symbols from whirl2f.so  

BE_EXPORTED void (*W2F_Cleanup_p)(void);
BE_EXPORTED void (*W2F_Def_ST_p)(FILE *outfile, ST *st);
BE_EXPORTED void (*W2F_Fini_p)(void);
BE_EXPORTED void (*W2F_Init_p)(void);
BE_EXPORTED void (*W2F_Mark_Loads_p)(void);
BE_EXPORTED void (*W2F_Nomark_Loads_p)(void);
BE_EXPORTED const char * (*W2F_Object_Name_p)(ST *func_st);
BE_EXPORTED void (*W2F_Outfile_Fini_p)(void);
BE_EXPORTED void (*W2F_Outfile_Init_p)(void);
BE_EXPORTED void (*W2F_Outfile_Translate_Pu_p)(WN *pu);
BE_EXPORTED void (*W2F_Pop_PU_p)(void);
BE_EXPORTED void (*W2F_Process_Command_Line_p)(INT phase_argc, char *phase_argv[], 
  INT argc, char *argv[]);
BE_EXPORTED void (*W2F_Push_PU_p)(WN *pu, WN *body_part_of_interest);
BE_EXPORTED void (*W2F_Set_Prompf_Emission_p)(WN_MAP *construct_map);
BE_EXPORTED void (*W2F_Set_Frequency_Map_p)(WN_MAP frequency_map);
BE_EXPORTED const char * (*W2F_Get_Transformed_Src_Path_p)(void);
BE_EXPORTED void (*W2F_Reset_Purple_Emission_p)(void);
BE_EXPORTED void (*W2F_Set_Purple_Emission_p)(void);
BE_EXPORTED BOOL (*W2F_Should_Emit_Nested_PUs_p)(void);
BE_EXPORTED void (*W2F_Translate_Istore_Lhs_p)(char *strbuf, UINT bufsize,
  WN* lhs, mINT64 istore_ofst, TY_IDX istore_addr_ty, TYPE_ID istore_mtype);
BE_EXPORTED void (*W2F_Translate_Purple_Main_p)(FILE *outfile, WN *pu, 
  const char *region_name);
BE_EXPORTED void (*W2F_Translate_Stid_Lhs_p)(char *strbuf, UINT bufsize, 
  ST *stid_st, mINT64 stid_ofst, TY_IDX stid_ty, TYPE_ID stid_mtype);
BE_EXPORTED void (*W2F_Translate_Wn_p)(FILE *outfile, WN *wn);
BE_EXPORTED void (*W2F_Translate_Wn_Str_p)(char *strbuf, UINT bufsize, WN *wn);

#endif // __linux__ || _NO_WEAK_SUPPORT_

#ifdef _NO_WEAK_SUPPORT_
// used in prompf.cxx
BE_EXPORTED INT (*New_Construct_Id_p) (void);
BE_EXPORTED INT (*Get_Next_Construct_Id_p) (void);

// used in wb.cxx
BE_EXPORTED void (*s_lno_debug_p)(char*);
BE_EXPORTED void (*cg_sdebug_p)(char*);

// used in f90_lower.cxx
BE_EXPORTED const char* (*Anl_File_Path_p) (void);

// wb_browser.cxx
#include "loop_info.h" // (ipl.so)
BE_EXPORTED void (DO_LOOP_INFO_BASE::*DLIB_Print_p) (FILE *, INT);

#include "if_info.h" // (lno.so)
BE_EXPORTED void (IF_INFO::*II_Print_p) (FILE *);

#include "access_vector.h" // (lno.so)
BE_EXPORTED void (ACCESS_ARRAY::*AA_Print_p) (FILE *) const;

#include "dep_graph.h"
#include "prompf.h"
#include "wb_browser.h"
// (ipl.so)
BE_EXPORTED void (WB_BROWSER::*Summary_p) (FILE *);

// cg.so
BE_EXPORTED void (*Process_Command_Line_p) (INT32, char **);

BE_EXPORTED void (*Prp_Process_Command_Line_p) (INT, char **, INT, char **);
BE_EXPORTED BOOL (*Prp_Needs_Whirl2c_p) ();
BE_EXPORTED BOOL (*Prp_Needs_Whirl2f_p) ();
BE_EXPORTED void (*Prp_Init_p) ();
BE_EXPORTED void (*Prp_Instrument_And_EmitSrc_p) (WN *pu);
BE_EXPORTED void (*Prp_Fini_p) ();

BE_EXPORTED void   (*Anl_Process_Command_Line_p) (INT, char **, INT, char **);
BE_EXPORTED BOOL   (*Anl_Needs_Whirl2c_p) ();
BE_EXPORTED BOOL   (*Anl_Needs_Whirl2f_p) ();
BE_EXPORTED void   (*Anl_Init_p) ();
BE_EXPORTED WN_MAP (*Anl_Init_Map_p) (MEM_POOL *id_map_pool); 
BE_EXPORTED void   (*Anl_Static_Analysis_p) (WN *pu, WN_MAP id_map);
BE_EXPORTED void   (*Anl_Fini_p) ();
BE_EXPORTED void   (*Anl_Cleanup_p) ();

BE_EXPORTED void (*Prompf_Emit_Whirl_to_Source_p) (PU_Info*, WN*);
#endif // _NO_WEAK_SUPPORT_
