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


/* ====================================================================
 * ====================================================================
 *
 * Module: config_wopt.h
 *
 * Revision history:
 *  05-May-96 - Extracted from be/opt/opt_config.h.
 *
 * Description:
 *
 * Declare global flag variables for -WOPT group options.
 * This file is included in common/com/config.c, but should not be
 * otherwise used outside of WOPT.
 *
 * ====================================================================
 * WARNING: WHENEVER A NEW FLAG IS ADDED:
 * ###	- Add its definition to config_wopt.c .
 * ###	- Add it to the group description config_wopt.c .
 * ###	- UPDATE 'class WOPT_SWITCH' DEFINED IN be/opt/opt_main.cxx.
 * ====================================================================
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef config_wopt_INCLUDED
#define config_wopt_INCLUDED

/* Incomplete types to prevent unnecessary inclusion: */
struct skiplist;

/*********************************************************************
 ***
 *** Anything that may be seen outside of opt_config.c should
 *** be declared in this section.
 ***
 *********************************************************************
 */

BE_EXPORTED extern UINT32 WOPT_Alias_Class_Limit;
BE_EXPORTED extern UINT32 WOPT_Ip_Alias_Class_Limit;
BE_EXPORTED extern BOOL WOPT_Ldx_Ratio_RegIns;
BE_EXPORTED extern BOOL WOPT_Enable_Add_Do_Loop_Info;
BE_EXPORTED extern BOOL WOPT_Enable_Add_Label_Loop_Info;
BE_EXPORTED extern BOOL WOPT_Enable_Aggressive_Code_Motion;
BE_EXPORTED extern INT32 WOPT_Enable_Aggressive_CM_Limit;	
BE_EXPORTED extern INT32 WOPT_Enable_Aggressive_CM_Threshold;
BE_EXPORTED extern BOOL WOPT_Enable_Aggressive_dce;
BE_EXPORTED extern BOOL WOPT_Enable_Aggressive_Doloop_Promotion;
BE_EXPORTED extern BOOL WOPT_Enable_Aggressive_IVR;
BE_EXPORTED extern BOOL WOPT_Enable_Aggressive_Lftr;
BE_EXPORTED extern BOOL WOPT_Enable_Aggressive_Phi_Simp;
BE_EXPORTED extern BOOL WOPT_Enable_Aggstr_Reduction;
BE_EXPORTED extern BOOL WOPT_Enable_Alias_Classification;
BE_EXPORTED extern BOOL WOPT_Enable_Alias_Class_Fortran_Rule;
BE_EXPORTED extern BOOL WOPT_Enable_Avoid_Rehash;	/* SSAPRE to try to minimize rehashing*/
BE_EXPORTED extern BOOL WOPT_Enable_Backedge_Placement; /* BB on critical backedge */
BE_EXPORTED extern BOOL WOPT_Enable_Bitwise_DCE;
BE_EXPORTED extern BOOL WOPT_Enable_CSE_FP_comparison;
BE_EXPORTED extern BOOL WOPT_Enable_Call_Flag;
BE_EXPORTED extern BOOL WOPT_Enable_Calls_Break_BB;
BE_EXPORTED extern BOOL WOPT_Enable_Calls_Break_BB_Set;
BE_EXPORTED extern BOOL WOPT_Enable_Call_Zero_Version; /* allow zero versions at calls */
BE_EXPORTED extern BOOL WOPT_Enable_Canon_Expr;
BE_EXPORTED extern BOOL WOPT_Enable_Canon_Uplevel;  /* canonicalize the up level ref */
/* allow operations in expression to be combined (or possibly just
 * rearranged to allow other optimizations).
 */
BE_EXPORTED extern BOOL WOPT_Enable_Combine_Operations;
BE_EXPORTED extern BOOL WOPT_Enable_Compare_Simp;
BE_EXPORTED extern BOOL WOPT_Enable_Const_PRE;
BE_EXPORTED extern INT32 WOPT_Enable_Const_PRE_Limit;
BE_EXPORTED extern BOOL WOPT_Enable_Copy_Propagate;
BE_EXPORTED extern BOOL WOPT_Enable_Copy_Prop_Bad_Ops;
BE_EXPORTED extern BOOL WOPT_Enable_Copy_Prop_LNO_Ops;
/* copy prop certain ops into ARRAY subscripts */
BE_EXPORTED extern BOOL WOPT_Enable_Copy_Prop_Ops_Into_Array;
BE_EXPORTED extern BOOL WOPT_Enable_Copy_Prop_Ops_Into_Array_Set;
BE_EXPORTED extern BOOL WOPT_Enable_Cvt_Folding;    /* enable folding of CVT/CVTL in emitter */    
BE_EXPORTED extern BOOL WOPT_Enable_DIVREM;		/* allow DIVREM opcode */
BE_EXPORTED extern BOOL WOPT_Enable_CG_Alias;
BE_EXPORTED extern BOOL WOPT_Enable_CRSIMP;         /* simplify coderep */
BE_EXPORTED extern BOOL WOPT_Enable_DCE;
BE_EXPORTED extern BOOL WOPT_Enable_DCE_Alias;	/* extra alias analysis w/DCE */
BE_EXPORTED extern BOOL WOPT_Enable_DCE_Branch;	/* delete redundant condition */
BE_EXPORTED extern INT32 WOPT_Enable_DCE_Branch_Pred_Limit;	/* local search limit */
BE_EXPORTED extern BOOL WOPT_Enable_DCE_Global;	/* delete global stores */
BE_EXPORTED extern BOOL WOPT_Enable_DCE_Label;	/* delete unref'd labels */
BE_EXPORTED extern BOOL WOPT_Enable_Dse_Aggressive;
BE_EXPORTED extern BOOL WOPT_Enable_DU_Full;	/* full DU-info for indirects */
BE_EXPORTED extern BOOL WOPT_Enable_DU_Union;	/* fix DU w/unions */
BE_EXPORTED extern BOOL WOPT_Enable_Estr_FB_Injury; /* use feedback frequency to */
					/* decide when IV updates are */
					/* reparable injuries */
BE_EXPORTED extern BOOL WOPT_Enable_Exp_PRE;
BE_EXPORTED extern INT32 WOPT_Enable_Exp_PRE_Limit;
BE_EXPORTED extern BOOL WOPT_Enable_Dead_CR_Fix;    /* process dead CR in main_emit time */

// Tighten up assertions about interprocedural alias class
BE_EXPORTED extern BOOL WOPT_Enable_Debug_Inconsistent_Ip_Class;

BE_EXPORTED extern BOOL WOPT_Enable_Edge_Placement; /* insert BB on critical edge */
BE_EXPORTED extern BOOL WOPT_Enable_Fast_Simp;	/* temporary 377066 */
BE_EXPORTED extern BOOL WOPT_Enable_Fold2const;
BE_EXPORTED extern BOOL WOPT_Enable_Fold_Lda_Iload_Istore; // for folding in coderep
BE_EXPORTED extern BOOL WOPT_Enable_FSA;
BE_EXPORTED extern BOOL WOPT_Enable_Generate_DU;
BE_EXPORTED extern INT32 WOPT_Enable_Generate_Trip_Count;
BE_EXPORTED extern BOOL WOPT_Enable_Goto;
BE_EXPORTED extern BOOL WOPT_Enable_Hoisting;
BE_EXPORTED extern BOOL WOPT_Enable_Ivar_Hoisting;
BE_EXPORTED extern BOOL WOPT_Enable_I8_Primary_IV;  /* allow primary IV to be I8 */
BE_EXPORTED extern BOOL WOPT_Enable_Iload_Prop;	/* propagate expression containing ivars */
BE_EXPORTED extern BOOL WOPT_Enable_Improved_Addr_Taken;
BE_EXPORTED extern BOOL WOPT_Enable_Input_Prop;     /* copy prop at value number time */
BE_EXPORTED extern BOOL WOPT_Enable_Itself_Prop;     /* copy prop of t=func(t) where func
					    contains only t plus other constants */
BE_EXPORTED extern BOOL WOPT_Enable_IPAA;           /* enable the use of IPA alias analysis result */
BE_EXPORTED extern BOOL WOPT_Enable_IVE;		/* induction-var elimination */
BE_EXPORTED extern BOOL WOPT_Enable_IVE_Old;        /* use old IVE with bug in it */
BE_EXPORTED extern BOOL WOPT_Enable_Ivar_Common;
BE_EXPORTED extern BOOL WOPT_Enable_Ivar_PRE;       /* enable *p as PRE candidate */
BE_EXPORTED extern BOOL WOPT_Enable_Ivincr_Cand;
BE_EXPORTED extern BOOL WOPT_Enable_IVR;		/* induction-var recognition */
BE_EXPORTED extern INT32 WOPT_Enable_IVR_Expand_Limit;  /* limit of expr expansion to search for incr */
					
/* do ivr for outermost in ||-region */
BE_EXPORTED extern BOOL WOPT_Enable_IVR_Outermost_Loop_Parallel_Region; 
BE_EXPORTED extern BOOL WOPT_Enable_Ldx;            /* index load optimization */
BE_EXPORTED extern BOOL WOPT_Enable_Lego_Opt;       /* max optimization for lego */
BE_EXPORTED extern BOOL WOPT_Enable_LFTR;           /* linear function test replacement */
BE_EXPORTED extern BOOL WOPT_Enable_LFTR_Ivar;      /* handle expr containing ivars */
BE_EXPORTED extern BOOL WOPT_Enable_LFTR2;          /* linear function test replacement */
BE_EXPORTED extern BOOL WOPT_Enable_LFTR2_Limit;
BE_EXPORTED extern BOOL WOPT_Enable_Load_PRE;
BE_EXPORTED extern INT32 WOPT_Enable_Load_PRE_Limit;
BE_EXPORTED extern BOOL WOPT_Enable_Local_Rvi;      /* enable fast rvi of locals */    
BE_EXPORTED extern INT32 WOPT_Enable_Local_Rvi_Limit;
BE_EXPORTED extern BOOL WOPT_Enable_Loopinvarexp_Str_Reduction;
BE_EXPORTED extern BOOL WOPT_Enable_Lower_Short_Circuit;
BE_EXPORTED extern BOOL WOPT_Enable_Lower_Short_Circuit_Set;
BE_EXPORTED extern BOOL WOPT_Enable_LNO_Copy_Propagate;
BE_EXPORTED extern BOOL WOPT_Enable_MINMAX;		/* allow MINMAX opcode */
BE_EXPORTED extern BOOL WOPT_Enable_Min_Type;       /* use minimum size type in PRE PREG */
BE_EXPORTED extern BOOL WOPT_Enable_Move_Intrinsicop;
BE_EXPORTED extern BOOL WOPT_Enable_MP_varref;      /* trust the var list in the nested procedure */
BE_EXPORTED extern const BOOL WOPT_Enable_MP_Const_Prop;  /* perform const prop into MP region */
BE_EXPORTED extern BOOL WOPT_Enable_New_SR;		/* new strength-reduction */
BE_EXPORTED extern BOOL WOPT_Enable_New_SR_Limit;
BE_EXPORTED extern BOOL WOPT_Enable_Output_Copy;    /* output copy propagation */
BE_EXPORTED extern BOOL WOPT_Enable_Ocopy_Lookupstmt;
BE_EXPORTED extern BOOL WOPT_Enable_Parm;		/* insert OPTPARM over parms */
BE_EXPORTED extern char *WOPT_Enable_Process;
BE_EXPORTED extern BOOL WOPT_Enable_Phi_Simp;
BE_EXPORTED extern BOOL WOPT_Enable_Prop_Aggressive;/* use inverse function to avoid 
					   overlapping live ranges */
BE_EXPORTED extern BOOL WOPT_Enable_Prop_Ivar;	/* copy propagation thru iload's */
BE_EXPORTED extern BOOL WOPT_Enable_Prop_CSE;       /* copy propagation of CSE expressions */
BE_EXPORTED extern INT32 WOPT_Enable_Prop_Limit;	/* tree height limit in copy prop */
BE_EXPORTED extern BOOL WOPT_Enable_Prune;		/* temporary, pv 370066 */
BE_EXPORTED extern BOOL WOPT_Enable_Replace_Second_IV; /* Force replacement of secondary IV */
BE_EXPORTED extern BOOL WOPT_Enable_Replace_While_Loop_Second_IV; /* Force replacement of secondary IV */
BE_EXPORTED extern BOOL  WOPT_Enable_Restricted_Map;
BE_EXPORTED extern INT32 WOPT_Enable_Rsv_Bits;	/* reserve bit count in itable */
BE_EXPORTED extern BOOL WOPT_Enable_RVI;		/* reg-var identification */
BE_EXPORTED extern BOOL WOPT_Enable_RVI1;		/* rvi phase 1 */
BE_EXPORTED extern BOOL WOPT_Enable_RVI2;		/* rvi phase 2 */
BE_EXPORTED extern BOOL WOPT_Enable_Rviistore;	/* agg. chi-handling on istore*/
BE_EXPORTED extern char *WOPT_Enable_Rviskip;	/* skip variable during rvi */
BE_EXPORTED extern BOOL WOPT_Enable_Rvisplit;	/* split bbs at ever stmt */
BE_EXPORTED extern BOOL WOPT_Enable_Rvivsym;	/* ignore vsym in chi lists */
BE_EXPORTED extern BOOL WOPT_Enable_Second_Alias_Class; /* repeat alias class for LNO */
BE_EXPORTED extern BOOL WOPT_Enable_Second_Order;
BE_EXPORTED extern BOOL WOPT_Enable_Simp_Iload;	/* simplifier folding iload */
BE_EXPORTED extern BOOL WOPT_Enable_Simple_If_Conv; /* enable simple if-conversion at CFG build time */
BE_EXPORTED extern char *WOPT_Enable_Skip;
BE_EXPORTED extern struct option_list *WOPT_Skip;	/* Skip option list */
BE_EXPORTED extern struct skiplist *WOPT_Skip_List;	/* Preprocessed skip list */
BE_EXPORTED extern BOOL WOPT_Enable_SLT;
BE_EXPORTED extern BOOL WOPT_Enable_Small_Br_Target; /* Disable propagation into br BBs */
BE_EXPORTED extern BOOL WOPT_Enable_Source_Order;   /* trace BB's in source order */
BE_EXPORTED extern BOOL WOPT_Enable_Speculation_Defeats_LFTR;
BE_EXPORTED extern BOOL WOPT_Enable_SSA_Minimization; /* SSA minimization in SSAPRE */
BE_EXPORTED extern BOOL WOPT_Enable_SSA_PRE;
BE_EXPORTED extern BOOL WOPT_Enable_Store_PRE;
BE_EXPORTED extern INT32 WOPT_Enable_Store_PRE_Limit;
BE_EXPORTED extern BOOL WOPT_Enable_Strength_Reduction;
BE_EXPORTED extern BOOL WOPT_Enable_Tail_Recur;	/* tail recursion opt */
BE_EXPORTED extern BOOL WOPT_Enable_Update_Vsym;
BE_EXPORTED extern char *WOPT_Set_Unique_Pt;
BE_EXPORTED extern BOOL WOPT_Enable_Unique_Pt_Vsym;
BE_EXPORTED extern INT32 WOPT_Enable_Value_Numbering; /*0=OFF, 1=after_pre, 2=befr_n_aftr*/
BE_EXPORTED extern INT32 WOPT_Enable_Vn_Ivc;        /* Induction variable coalescing; 0=OFF
					 * See be/opt/opt_vn_ivc.h */
BE_EXPORTED extern UINT32 WOPT_Enable_Vnfre_After;  /* Disable vnfre after given valnum */
BE_EXPORTED extern UINT32 WOPT_Enable_Vnfre_Before; /* Disable vnfre before given valnum */
BE_EXPORTED extern BOOL WOPT_Enable_Verbose;
BE_EXPORTED extern INT32 WOPT_Enable_Verify;	/* verify data structures */
BE_EXPORTED extern BOOL WOPT_Enable_Vsym_Unique;
BE_EXPORTED extern BOOL WOPT_Enable_VN_Full;	/* full value number for ivars */
BE_EXPORTED extern BOOL WOPT_Enable_While_Loop;	/* cvt while-do to do-loop */
BE_EXPORTED extern BOOL WOPT_Enable_Worklist_Pruning;
BE_EXPORTED extern BOOL WOPT_Enable_Zero_Version;
BE_EXPORTED extern BOOL WOPT_Enable_Strong_Barrier; /* disallow any memop motion across a barrier */
BE_EXPORTED extern BOOL WOPT_Enable_Aggr_Invariant; /* aggressive invariant detection */
BE_EXPORTED extern BOOL WOPT_Enable_Shrink;         /* enable live range shrinking */
BE_EXPORTED extern INT32 WOPT_Enable_Extra_Rename_Pass;
BE_EXPORTED extern BOOL  WOPT_Enable_Extra_Rename_Pass_Set;
BE_EXPORTED extern UINT32 WOPT_Enable_Extra_Preopt_Pass; // additional iterations of preopt
BE_EXPORTED extern BOOL  WOPT_Enable_Bool_Simp; 
BE_EXPORTED extern BOOL  WOPT_Enable_Feedback_LPRE;
BE_EXPORTED extern BOOL  WOPT_Enable_Feedback_EPRE;
BE_EXPORTED extern BOOL  WOPT_Enable_CFG_Opt;
BE_EXPORTED extern BOOL  WOPT_Enable_CFG_Display;
BE_EXPORTED extern BOOL  WOPT_Enable_CFG_Merge_Multi_Zone;
BE_EXPORTED extern BOOL  WOPT_Enable_CFG_Merge_Multi_Zone_Set;
BE_EXPORTED extern BOOL  WOPT_Enable_CFG_Opt1;
BE_EXPORTED extern BOOL  WOPT_Enable_CFG_Opt2;
BE_EXPORTED extern INT32 WOPT_Enable_CFG_Opt2_Limit;
BE_EXPORTED extern BOOL  WOPT_Enable_CFG_Opt3;
BE_EXPORTED extern BOOL  WOPT_Enable_CFG_Opt4;
BE_EXPORTED extern BOOL  WOPT_Enable_CFG_Opt_Limit;
BE_EXPORTED extern BOOL  WOPT_Enable_CFG_Opt_Limit_Set;
BE_EXPORTED extern BOOL  WOPT_Enable_Bits_Load_Store;
BE_EXPORTED extern BOOL  WOPT_Enable_Epre_Before_Ivr; // For running epre early
BE_EXPORTED extern BOOL  WOPT_Enable_Lpre_Before_Ivr; // For running lpre early
BE_EXPORTED extern BOOL  WOPT_Enable_Spre_Before_Ivr; // For running spre early
BE_EXPORTED extern BOOL  WOPT_Enable_Bdce_Before_Ivr; // For running bdce early
BE_EXPORTED extern BOOL  WOPT_Enable_New_Phase_Ordering; // Enables some phases before ivr
#ifdef TARG_ST
BE_EXPORTED extern BOOL  WOPT_Enable_FPFSA; // Enables fix point flow sensitive analysis
//TB
BE_EXPORTED extern BOOL WOPT_Enable_Warn_Uninit;   // enable warning for detected uninitialized locals
#endif
#endif /* config_wopt_INCLUDED */

