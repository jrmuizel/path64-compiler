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
 * Module: cg.cxx
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description:
 *
 * This	file contains the main driver and initialization,termination
 * routines for	the code generator.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "wn.h"
#include "config.h"
#include "config_list.h"
#include "tracing.h"
#include "timing.h"
#include "strtab.h"
#include "erglob.h"
#include "ercg.h"
#include "cgir.h"
#include "tn_set.h"
#include "tn_map.h"
#include "gtn_universe.h"
#include "data_layout.h"
#include "whirl2ops.h"
#include "bitset.h"
#include "bb_set.h"
#include "freq.h"
#include "fb_whirl.h"
#include "glob.h"
#include "cg_region.h"
#include "wn_util.h"
#include "opt_alias_interface.h"
#include "ir_reader.h"
#include "dwarf_DST_mem.h"
#include "region_util.h"
#include "eh_region.h"
#include "label_util.h"
#include "tag.h"

#include "lai.h"
#include "calls.h"
#include "cgtarget.h"
#include "expand.h"
#include "emit.h"

MEM_POOL MEM_local_region_pool;	/* allocations local to processing a region */
MEM_POOL MEM_local_region_nz_pool;

BOOL Trace_REGION_Interface = FALSE;

BOOL PU_Has_Calls;
BOOL PU_References_GP;

BOOL LAI_PU_Has_Feedback;

RID *Current_Rid;

TN_MAP TN_To_PREG_Map;

/* WOPT alias manager */
struct ALIAS_MANAGER *Alias_Manager;

static BOOL Orig_Enable_SWP;

/* ====================================================================
 *   LAI_PU_Initialize (wn_pu)
 *
 *   Stuff that needs to be done at the start of each PU in cg. 
 * ====================================================================
 */
void
LAI_PU_Initialize (
  WN *wn_pu
)
{
  static INT pu_num;

  MEM_POOL_Push ( &MEM_phase_pool );
  MEM_POOL_Push ( &MEM_local_pool );
  MEM_POOL_Push ( &MEM_phase_nz_pool );
  MEM_POOL_Push ( &MEM_local_nz_pool );

  PU_Has_Calls = FALSE;
  PU_References_GP = FALSE;

  LAI_Configure_Opt_Level((pu_num < CG_skip_before ||
			   pu_num > CG_skip_after  ||
			   pu_num == CG_skip_equal) ? 0 : Opt_Level);
  pu_num++;

  CGTARG_Initialize();
  BB_PU_Initialize ();
  Init_TNs_For_PU ();
  TN_MAP_Init();
  BB_MAP_Init();
  OP_MAP_Init();
  Init_Label_Info();

#ifdef EMULATE_LONGLONG
  extern void Init_TN_Pair();
  Init_TN_Pair ();
#endif

  /* initialize register package for current pu */
  REGISTER_Pu_Begin();

  Init_Entry_Exit_Code (wn_pu);
  REGISTER_Reset_FP();	// in case $fp is used, must be after entry_exit init

  /* Initialize global tn universe */
  GTN_UNIVERSE_Pu_Begin();

  Trace_REGION_Interface = Get_Trace( TP_REGION, TT_REGION_CG_DEBUG ) ||
    Get_Trace(TP_REGION, TT_REGION_BOUND_DEBUG );

  Init_gen_quad_preg(); // init statics to hold quad preg STs

  /* data layout and lowering now happens earlier, in bedriver. */
  /* don't finalize stack frame until just before emit, after all spills. */

  return;
}

/* ====================================================================
 *   LAI_PU_Finalize(void)
 * ====================================================================
 */
void
LAI_PU_Finalize(void)
{
  TAG_Finish();
  GTN_UNIVERSE_Pu_End ();
  OP_MAP_Finish();

  /* TN_To_PREG_Map is allocated from MEM_pu_pool and so can't be popped
     but it can be put on the free list and cleared at the end of the PU */
  TN_MAP_Delete(TN_To_PREG_Map);
  TN_To_PREG_Map = NULL;

  Free_BB_Memory();		    /* Free non-BB_Alloc space. */
  MEM_POOL_Pop ( &MEM_local_pool );
  MEM_POOL_Pop ( &MEM_local_nz_pool );
  MEM_POOL_Pop ( &MEM_phase_pool );
  MEM_POOL_Pop ( &MEM_phase_nz_pool );
}

/* ====================================================================
 *   LAI_Region_Initialize (rwn, alias_mgr)
 *
 *   Stuff that needs to be done at the start of each REGION in cg. 
 * ====================================================================
 */
static void
LAI_Region_Initialize (
  WN *rwn, 
  struct ALIAS_MANAGER *alias_mgr
)
{
  MEM_POOL_Push (&MEM_local_region_pool);
  MEM_POOL_Push (&MEM_local_region_nz_pool);

  EXP_Init ();
  FREQ_Region_Initialize ();
  BB_REGION_Initialize ();
  Init_TNs_For_REGION ();

  /*
   * Create Array to map PREGs into TNs
   * Must be done after Init_Entry_Exit_Code, since
   * Init_Entry_Exit_Code creates special PREGs to represent
   * save locations in WHIRL
   */
  PREG_NUM last_preg_num;
  last_preg_num = Get_Preg_Num (PREG_Table_Size(CURRENT_SYMTAB))+1;
  PREG_To_TN_Array = (TN **) Pu_Alloc (sizeof (TN *) * last_preg_num);
  PREG_To_TN_Mtype = (TYPE_ID *) Pu_Alloc (sizeof (TYPE_ID) * last_preg_num);

  PREG_To_TN_Clear();	/* this enforces different preg maps between regions */
  if (TN_To_PREG_Map == NULL)
    TN_To_PREG_Map = TN_MAP_Create();

  TN_CORRESPOND_Free(); /* remove correspondence between tns (ex. divrem) */

  GTN_UNIVERSE_REGION_Begin();

  Whirl2ops_Initialize(alias_mgr);

  Current_Rid = REGION_get_rid( rwn );

  return;
}

/* ====================================================================
 *   LAI_Region_Finalize (result_before, result_after,
 *	                            rwn, am, generate_glue_code)
 *
 *   Stuff that needs to be done at the end of each REGION in cg.
 *   This includes making glue code to map TNs in CG'd code
 *   from/to PREGs in WHIRL
 * ====================================================================
 */
static void
LAI_Region_Finalize (WN *result_before, WN *result_after,
		    WN *rwn, struct ALIAS_MANAGER *am, BOOL generate_glue_code)
{
  RID *rid;
  CGRIN *cgrin;
  WN *entry_fixup, *exit_fixup;
  INT32 i, num_exits;

  Is_True(REGION_consistency_check(rwn),("CG_Region_Finalize"));
  rid = REGION_get_rid( rwn );
  cgrin = RID_cginfo( rid );
  FmtAssert(rid != NULL && cgrin != NULL,
	    ("CG_Region_Finalize, inconsistent region"));

  REGION_set_level(rid, RL_CGSCHED);

  if (generate_glue_code) {
    /* region entry glue code */
    entry_fixup = CGRIN_entry_glue( cgrin );
    REGION_Entry_PREG_Whirl( rid, entry_fixup, CGRIN_tns_in( cgrin ), am );
    if ( Trace_REGION_Interface ) {
      fprintf( TFile, "<region> Entry glue code for RGN %d\n", RID_id(rid) );
      fdump_tree( TFile, entry_fixup );
    }
    WN_INSERT_BlockFirst( result_before, entry_fixup );

    num_exits = RID_num_exits( rid );
    for (i=0; i<num_exits; i++) {
      exit_fixup = CGRIN_exit_glue_i( cgrin, i );
      REGION_Exit_PREG_Whirl( rid, i, exit_fixup,
			     CGRIN_tns_out_i( cgrin, i ), am );
      if ( Trace_REGION_Interface ) {
	fprintf( TFile, "<region> Exit glue code for exit %d RGN %d\n",
		i, RID_id(rid) );
	fdump_tree( TFile, exit_fixup );
      }
      WN_INSERT_BlockLast( result_after, exit_fixup );
    }
  }

  Whirl2ops_Finalize();

  MEM_POOL_Pop (&MEM_local_region_pool);
  MEM_POOL_Pop (&MEM_local_region_nz_pool);
}

/* ====================================================================
 *   LAI_Emit_Code (rwn, alias_mgr, pu_dst, region)
 *
 *   Can be called two ways:
 *   1) on a region (pu_dst is NULL, returns code)
 *   2) on a PU (pu_dst is no NULL, returns NULL)
 * ====================================================================
 */
WN *
LAI_Emit_Code( 
    WN *rwn, 
    struct ALIAS_MANAGER *alias_mgr, 
    DST_IDX pu_dst, 
    BOOL region )
{
/*later:  BOOL region = DST_IS_NULL(pu_dst); */

  Alias_Manager = alias_mgr;

  Set_Error_Phase("LAI Emission");
  Start_Timer(T_CodeGen_CU);

  // Use of feedback information can be disabled in CG using the 
  // -CG:enable_feedback=off flag. The flag LAI_PU_Has_Feedback is used
  // all over CG instead of Cur_PU_Feedback for this reason.
  LAI_PU_Has_Feedback = ((Cur_PU_Feedback != NULL) && LAI_enable_feedback);

  LAI_Region_Initialize (rwn, alias_mgr);

  Set_Error_Phase ("Code_Expansion");
  Start_Timer (T_Expand_CU);

  // If this PU is simply a wrapper for assembly code to be placed
  // into the .s file, take care of that job and move on.
  if (WN_operator(rwn) == OPR_FUNC_ENTRY &&
      ST_asm_function_st(*WN_st(rwn))) {
    fprintf(Lai_File, "\n%s\n", ST_name(WN_st(rwn)));
    return rwn;
  }

  Convert_WHIRL_To_OPs (rwn);

  // If using feedback, incorporate into the CFG as early as possible.
  // This phase also fills in any missing feedback using heuristics.
  if (LAI_PU_Has_Feedback) {
    Set_Error_Phase ("FREQ");
    Start_Timer (T_Freq_CU);
    FREQ_Incorporate_Feedback ( rwn );
    Stop_Timer (T_Freq_CU);
    Set_Error_Phase ( "Code_Expansion" );
  }

  EH_Prune_Range_List();
  /* do it later ...
  Optimize_Tail_Calls( Get_Current_PU_ST() );
  */
  Init_Callee_Saved_Regs_for_REGION(Get_Current_PU_ST(), region);
  Generate_Entry_Exit_Code (Get_Current_PU_ST(), region);

  Stop_Timer (T_Expand_CU);
  Check_for_Dump (TP_CGEXP, NULL);

  if (!region) {
    /* Check that we didn't introduce a new gp reference */
    Adjust_GP_Setup_Code( Get_Current_PU_ST(), FALSE);

    /* The stack frame is final at this point, no more spilling after this.
     * We can set the Frame_Len now.
     * Then we can go through all the entry/exit blocks and fix the SP 
     * adjustment OP or delete it if the frame length is zero.
     */
    Set_Frame_Len (Finalize_Stack_Frame());
    Set_Error_Phase ("Final SP adjustment");
    Adjust_Entry_Exit_Code (Get_Current_PU_ST());
  }

  if (region) {
    /*--------------------------------------------------------------------*/
    /* old region: rwn, rid_orig					  */
    /* new region: rwn_new, rid_new (encloses old region)		  */
    /*--------------------------------------------------------------------*/
    WN	*inner_body, *outer_body, *exitBlock, *comment;
    WN  *rwn_new, *result_block_before, *result_block_after;
    RID *rid_orig;
    char str[100];

    Is_True(REGION_consistency_check(rwn),("CG_Generate_Code"));
    rid_orig = REGION_get_rid(rwn);

    /* don't delete rwn, it contains the stub that helps find the MOPS
       that the region has been lowered to */

    outer_body = WN_CreateBlock();
    /* put inner region inside outer containment block */
    WN_INSERT_BlockFirst(outer_body, rwn);
    /* we assembled the new exit block earlier in Build_CFG()		*/
    exitBlock = CGRIN_nested_exit(RID_cginfo(rid_orig));
    WN_region_exits(rwn) = exitBlock; /* PPP ??? */

    rwn_new = outer_body;

    /* put a note in the inner body that the code isn't there anymore */
    inner_body = WN_CreateBlock();
    WN_region_body(rwn) = inner_body; /* overwrite old body, now in MOPs */
    sprintf(str,"RGN %d has been lowered to MOPs, level=%s",
	    RID_id(rid_orig), RID_level_str(rid_orig));
    comment = WN_CreateComment(str);
    WN_INSERT_BlockFirst(inner_body, comment);

    /* Need to split result block for glue code into two parts: before and
       after the region body. The reason we can't just insert the glue code
       directly before or after the region directly is that we need to keep
       it separate for updating the alias info.
       If CG_LOOP has made some WHIRL glue, it is inserted in result_block. */
    result_block_before = WN_CreateBlock();
    result_block_after = WN_CreateBlock();

    /* fill-in blocks with glue code */
    Set_Error_Phase("Region Finalize");
    Start_Timer(T_Region_Finalize_CU);
    LAI_Region_Finalize( result_block_before, result_block_after,
		       rwn, alias_mgr, TRUE /* generate_glue_code */ );
    Stop_Timer(T_Region_Finalize_CU);

    /* generate alias information for glue code */
    REGION_update_alias_info(result_block_before, alias_mgr);
    REGION_update_alias_info(result_block_after, alias_mgr);

    /* insert glue code before and after */
    WN_INSERT_BlockFirst( rwn_new, result_block_before );
    WN_INSERT_BlockLast( rwn_new, result_block_after );

    Stop_Timer ( T_CodeGen_CU );
    Set_Error_Phase ("LAI Driver");

    return rwn_new;
  } /* if (region */

  else { /* PU */
    /* Write the EH range table. */
    if (PU_has_exc_scopes(Get_Current_PU())) {
      EH_Write_Range_Table(rwn);
    }

    /* Emit the code for the PU. This involves writing out the code to
     * an lai file. Additional tasks performed by this module are:
     *
     *   - convert long branches into a chain of short branches.
     *   - add nada's to quad-align branch targets for TFP.
     */
    Set_Error_Phase ("LAI Emission");
    Start_Timer (T_Emit_CU);
    LAI_Emit_PU (Get_Current_PU_ST(), pu_dst, rwn);
    Check_for_Dump (TP_EMIT, NULL);
    Stop_Timer (T_Emit_CU);

    Set_Error_Phase("Region Finalize");
    Start_Timer(T_Region_Finalize_CU);
    LAI_Region_Finalize( NULL, NULL, rwn, alias_mgr,
		                    FALSE /* generate_glue_code */ );
    Stop_Timer(T_Region_Finalize_CU);

    /* List local symbols if desired: */
    if ( List_Symbols )
	Print_symtab (Lst_File, CURRENT_SYMTAB);

    Stop_Timer ( T_CodeGen_CU );
    Set_Error_Phase ("LAI Driver");

    return rwn;
  }
}

/* =================================================================
 *                 routines for dumping/tracing the program 
 * =================================================================
 */

/* =================================================================
 *   Trace_IR
 * =================================================================
 */
void
Trace_IR (
  INT phase,		/* Phase after which we're printing */
  const char *pname,	/* Print name for phase	*/
  BB *cur_bb		/* BB to limit traces to */
)
{
  INT cur_bb_id = cur_bb ? BB_id(cur_bb) : 0;
  if (Get_Trace(TKIND_IR, phase)
      && (cur_bb_id == 0 || Get_BB_Trace(cur_bb_id))) {
    fprintf(TFile, "\n%s%s\tIR after %s\n%s%s\n",
	                       DBar, DBar, pname, DBar, DBar);
    if (cur_bb != NULL) {
      Print_BB(cur_bb);
    } else {
      BB *bb;
      for (bb = REGION_First_BB; bb; bb = BB_next(bb))	{
	if (Get_BB_Trace(BB_id(bb)) && Get_Trace(TKIND_IR, phase)) {
	  Print_BB(bb);
	}
      }
    }
    fprintf(TFile, "%s%s\n", DBar, DBar);
  }
}

/* =================================================================
 *   Trace_TN
 * =================================================================
 */
static void
Trace_TN (
  INT phase,		/* Phase after which we're printing */
  const char *pname	/* Print name for phase	*/
)
{
  if ( Get_Trace ( TKIND_TN, phase ) ) {
    fprintf ( TFile, "\n%s%s\tTNs after %s\n%s%s\n",
	      DBar, DBar, pname, DBar, DBar );
    Print_TNs ();
  }
}

/* =================================================================
 *   Trace_ST
 * =================================================================
 */
static void
Trace_ST (
  INT phase,		/* Phase after which we're printing */
  const char *pname	/* Print name for phase	*/
)
{
  if ( Get_Trace ( TKIND_SYMTAB, phase ) ) {
  	fprintf ( TFile, "\n%s%s\tSymbol table after %s\n%s%s\n",
              DBar, DBar, pname, DBar, DBar );
  	SYMTAB_IDX level = CURRENT_SYMTAB;
	while (level >= GLOBAL_SYMTAB) {
	  	Print_symtab (TFile, level);
		--level;
	}
  }
}

/* ====================================================================
 *   Check_for_Dump
 *
 *   Check whether symbol table, TN, or IR dumps have been requested for
 *   the given pass; if so, generate them to the trace file. If a BB is
 *   given, limit the dumps to that BB.
 * ====================================================================
 */
void
Check_for_Dump ( INT32 pass, BB *bb )
{
  if (bb == NULL || Get_BB_Trace(BB_id(bb))) {
    const char *s = Get_Error_Phase();

    /* Check to see if we should dump the STAB.
     */
    Trace_ST ( pass, s );

    /* Check to see if we should dump the TNs.
     */
    Trace_TN ( pass, s );

    /* Check to see if we should dump the IR.  If yes, check each BB.
     */
    Trace_IR ( pass, s, bb );

#ifdef MEM_STATS
    /* Check to see if we should give a memory allocation trace.
     */
    Trace_Memory_Allocation ( pass, s, Get_Current_PU_ST() );
#endif
  }
}

/* ====================================================================
 *   CG_Dump_Region
 * ====================================================================
 */
void
CG_Dump_Region (
  FILE *fd, 
  WN *wn
)
{
  RID	*rid = REGION_get_rid(wn);
  Is_True(rid != NULL, ("CG_Dump_Region, NULL RID"));
  if (rid && RID_level(rid) >= RL_CGSCHED) {
    CGRIN  *cgrin = RID_cginfo(rid);
    if (cgrin && CGRIN_entry(cgrin)) {
      BB *bb;
      for (bb=CGRIN_entry(cgrin); bb; bb=BB_next(bb))
	Print_BB( bb );
    }
  }
}

