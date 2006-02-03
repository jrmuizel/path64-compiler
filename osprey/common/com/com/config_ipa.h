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
 * Module: config_ipa.h
 *
 * Revision history:
 *  11-Apr-96 - Original Version
 *
 * Description:
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef config_ipa_INCLUDED
#define config_ipa_INCLUDED

struct skiplist;
struct option_list;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Defined in config.h -- here too to avoid including that: */
BE_EXPORTED extern BOOL Tracing_Enabled;	/* Any trace options set? */

/* ====================================================================
 * List of global variables that are set by the -IPA option group
 * ====================================================================
 */

/* ===== Feature enable flags: ===== */
BE_EXPORTED extern BOOL IPA_Enable_DFE;		/* Dead Function Elimination */
BE_EXPORTED extern BOOL IPA_Enable_DFE_Set;		/* ... explicitly set? */
BE_EXPORTED extern BOOL IPA_Enable_Inline;		/* Inlining */
BE_EXPORTED extern BOOL IPA_Enable_Picopt;		/* PIC optimization */
BE_EXPORTED extern BOOL IPA_Enable_AutoGnum;	/* AutoGnum optimization */
BE_EXPORTED extern BOOL IPA_Enable_BarrierFarg; /* Barrier for aliased Fortran actual */
BE_EXPORTED extern BOOL IPA_Enable_Opt_Alias;	/* WOPT uses alias/mod/ref */
BE_EXPORTED extern BOOL IPA_Enable_Simple_Alias;	/* Simple alias/mod/ref */
BE_EXPORTED extern BOOL IPA_Enable_Addressing;	/* Addr_taken analysis */
BE_EXPORTED extern BOOL IPA_Enable_Readonly_Ref;	/* find out readonly ref parameter */
BE_EXPORTED extern BOOL IPA_Enable_Cprop;		/* Constant Propagation */
BE_EXPORTED extern BOOL IPA_Enable_Cprop2;		/* Aggressive constant propagation */
BE_EXPORTED extern BOOL IPA_Enable_Assert;		/* assert cprop value */
BE_EXPORTED extern BOOL IPA_Enable_daVinci;		/* Graphical display of call graph */
BE_EXPORTED extern BOOL IPA_Enable_ipacom;		/* Call ipacom after IPA */
BE_EXPORTED extern BOOL IPA_Enable_final_link;	/* Final link step */
BE_EXPORTED extern BOOL IPA_Enable_Memtrace;	/* Memory trace */
BE_EXPORTED extern BOOL IPA_Enable_DST;		/* Generate DST */
BE_EXPORTED extern BOOL IPA_Enable_DCE;		/* Enable Dead Call Elimination */
BE_EXPORTED extern BOOL IPA_Enable_Exc;		/* Enable exception handling */
BE_EXPORTED extern BOOL IPA_Enable_Recycle;		/* Enable recycling of variables */
BE_EXPORTED extern BOOL IPA_Enable_DVE;		/* Enable Dead Variable Elimination */
BE_EXPORTED extern BOOL IPA_Enable_CGI;		/* Enable Constant Global Variable
					 * Identification */
BE_EXPORTED extern BOOL IPA_Enable_Copy_Prop;	/* Copy propagation during inlining */
BE_EXPORTED extern BOOL IPA_Enable_Padding;		/* Intra-Dimension padding 
					 * of common block variables */
BE_EXPORTED extern UINT32 IPA_Common_Pad_Size;      /* Amount by which to pad commons */
BE_EXPORTED extern BOOL IPA_Enable_Split_Common;    /* Enable split common inside IPA */
BE_EXPORTED extern BOOL IPA_Enable_Cloning;         /* Enable Cloning in conjunction
					 * with constant propagation */
					   
BE_EXPORTED extern BOOL IPA_Echo_Commands;		/* Echo back end command lines */
BE_EXPORTED extern BOOL IPA_Enable_Lang;            /* support inlining across language
					   boundaries */
BE_EXPORTED extern BOOL IPA_Enable_Preempt;         /* allow the user to specify that
				           functions are not preemptible */
BE_EXPORTED extern BOOL IPA_Enable_Flow_Analysis;	/* flow-sensitive analysis */
BE_EXPORTED extern BOOL IPA_Enable_Array_Sections;  /* array section analysis in IPA */
BE_EXPORTED extern BOOL IPA_Enable_Array_Summary;   /* array section summary in IPL */
BE_EXPORTED extern BOOL IPA_Enable_Scalar_Euse;     /* enable scalar euse  */
BE_EXPORTED extern BOOL IPA_Enable_Scalar_Kill;     /* enable scalar kill   */
BE_EXPORTED extern BOOL IPA_Enable_Common_Const;    /* enable const prop of common block variables */
BE_EXPORTED extern BOOL IPA_Enable_Relocatable_Opt; /* support -call_shared optimizations of relocatable objects */
BE_EXPORTED extern BOOL IPA_Enable_Feedback;        /* create .prg, .dfe, .dve files */
BE_EXPORTED extern BOOL IPA_Enable_Alias_Class;     /* interprocedural alias classification */
BE_EXPORTED extern BOOL IPA_Debug_AC_Temp_Files;    /* save alias class temps til done? */
BE_EXPORTED extern BOOL IPA_Enable_Reshape;         /* reshape analysis for arrays  */

BE_EXPORTED extern BOOL IPA_Enable_Preopt;		/* call preopt during IPA */
BE_EXPORTED extern BOOL IPA_Enable_Preopt_Set;

/* ===== Inlining heuristics: ===== */

/* max. bloat % of the entire program */
BE_EXPORTED extern UINT32	IPA_Bloat_Factor;
BE_EXPORTED extern BOOL	IPA_Bloat_Factor_Set;

BE_EXPORTED extern UINT32	IPA_PU_Limit;	/* Max nodes per PU after inlining */
BE_EXPORTED extern BOOL	IPA_PU_Limit_Set;	/* if IPA_PU_Limit is set by user */

/* absolute max. # of nodes per PU after inlining (1.25 * IPA_PU_Limit) */
BE_EXPORTED extern UINT32	IPA_PU_Hard_Limit;
BE_EXPORTED extern BOOL	IPA_PU_Hard_Limit_Set;

/* Size of small PU that's always inlined */
BE_EXPORTED extern UINT32	IPA_PU_Minimum_Size;

/* Callees larger than this size are not inlined, except ...  */
BE_EXPORTED extern UINT32	IPA_Small_Callee_Limit; 

BE_EXPORTED extern UINT32	IPA_Max_Depth;		/* maximum depth to inline */
BE_EXPORTED extern UINT32	IPA_Force_Depth;	/* force inlining to depth n
					 * regardless of size */
BE_EXPORTED extern BOOL	IPA_Force_Depth_Set;

/* 100th% of call freq lower than which will not inlined */
BE_EXPORTED extern UINT32	IPA_Min_Freq;	

/* % of time that an inlined callee is called by its caller */
BE_EXPORTED extern UINT32	IPA_Rela_Freq;	

/* only routines "hotter" than this will be inlined */
BE_EXPORTED extern UINT32  IPA_Min_Hotness;

/* ignore zero-freq. statements when estimating size of a PU */
BE_EXPORTED extern BOOL	IPA_Use_Effective_Size;

/* ===== Miscellaneous -IPA options: ===== */

BE_EXPORTED extern BOOL	IPA_Enable_Merge_ty;	/* merge types across files */
BE_EXPORTED extern UINT32	IPA_Max_Jobs;	/* concurrent backend compilations */
BE_EXPORTED extern BOOL	IPA_Max_Jobs_Set;

/* max. gp-relative space available for auto Gnum */
BE_EXPORTED extern UINT32	IPA_Gspace;	

/* user specified -G num */
BE_EXPORTED extern UINT32	IPA_user_gnum;	

/* user defined percentage used to multiply the estimated External GOT entries with for estimating the total .got size */
BE_EXPORTED extern UINT32   IPA_Extgot_Factor;

/* user defined number of Fortran Intrinsics used, for purpose of
   estimating the number of GOTs  */
BE_EXPORTED extern UINT32  	IPA_Num_Fortran_Intrinsics;
BE_EXPORTED extern BOOL	IPA_Has_Fortran;

/* maximum size of address space used for the entire input source to
 * be mapped in without converting to the SAVE_SPACE mode
 */
BE_EXPORTED extern UINT32	IPA_Map_Limit;

/* In case the program is too big that it needs multigot, we would
 * still like to get some of the PICOPT by partitioning the program
 * into different partitions.  Each partition is treated like a
 * seperate compilation unit that IPA can do all its optimization
 * on, sort-of like a shared object by itself.  There will be NO 
 * cross-inlining between different partitions.
 * Each "visible" entries into the partition are at best considered
 * as PROTECTED symbols.
 * There will also be a "COMMON" partition that all the other
 * partitions can inline from, but every function in the
 * "COMMON" partition are treated just like the "visible" entry
 * in the other partitions.  All defined global variables in a
 * certain partition can be marked as GP-relative, but the same
 * global variable used in another partition CANNOT be marked 
 * as GP-relative.  This option is either turned on by the user
 * or by IPA when -multigot is on or the SAVE_SPACE option is on
 */
BE_EXPORTED extern BOOL 	IPA_Enable_SP_Partition;   
BE_EXPORTED extern BOOL 	IPA_Enable_GP_Partition;   

#define DEFAULT_ACCESS_MODE     0
#define SAVE_SPACE_MODE         1
#define IPA_DEVWARN_LIMIT       2

BE_EXPORTED extern BOOL IPA_Space_Access_Mode;

BE_EXPORTED extern struct option_list *IPA_Group_Names;	/* partition groupings */
BE_EXPORTED extern struct option_list *IPA_Spec_Files;	/* Specification files for IPA options, particularly used for partition groupings */


BE_EXPORTED extern struct option_list *IPA_Skip;	/* List of skip options */
BE_EXPORTED extern BOOL	IPA_Skip_Report;	/* Report skip count */

BE_EXPORTED extern BOOL	IPA_Enable_Keeplight;	/* Only keep .I and .o under the
					 * .ipakeep directory
					 */

BE_EXPORTED extern BOOL	IPA_Enable_Cord;	/* Enable procedure reordering. */
BE_EXPORTED extern BOOL     IPA_Enable_Linearization; /* Enable linearization of array
					    expressions */ 

BE_EXPORTED extern BOOL	IPA_Use_Intrinsic;	/* Load Intrinsic libraries */
BE_EXPORTED extern BOOL     IPA_Enable_Inline_Nested_PU;   /* Enable inlining of nested PU */
BE_EXPORTED extern BOOL     IPA_Enable_Inline_Struct;   /* Enable inlining of PU with F90 structures */
BE_EXPORTED extern BOOL     IPA_Enable_Inline_Char_Array;   /* Enable inlining of PU with char arrays */
BE_EXPORTED extern BOOL     IPA_Enable_Inline_Optional_Arg;   /* Enable inlining of PU with optional arguments */
BE_EXPORTED extern BOOL     IPA_Enable_Inline_Struct_Array_Actual;   /* Enable inlining of PU with F90 structures with actuals being array type */
BE_EXPORTED extern BOOL     IPA_Enable_Inline_Var_Dim_Array;   /* Enable inlining of PU with param that is variable-dimensioned array */


/* Maximum number of clones for a call graph node */
BE_EXPORTED extern UINT32 IPA_Max_Node_Clones;
BE_EXPORTED extern BOOL   IPA_Max_Node_Clones_Set;

/* Maximum % increase of the call graph size through node cloning */
BE_EXPORTED extern UINT32 IPA_Max_Clone_Bloat;
  
/* Max. "size" of each output file */
BE_EXPORTED extern UINT32 IPA_Max_Output_File_Size;

/* percentage change of the max. output file size */
BE_EXPORTED extern INT32 IPA_Output_File_Size;

#ifdef TARG_ST
  /* [CL] Compiler command line */    
BE_EXPORTED extern char* Ipa_Compiler_Command_Line;
  // [TB} Final executable name
BE_EXPORTED extern char* Ipa_Exec_Name;
#endif

/* ====================================================================
 * List of global variables that are set by the -INLINE option group.
 * ====================================================================
 */

/* What is the default inlining behavior? */
BE_EXPORTED extern BOOL	INLINE_Enable;	/* If FALSE, disable inliner? */
BE_EXPORTED extern BOOL	INLINE_All;	/* Inline everything possible? */
#ifdef TARG_ST //CL
BE_EXPORTED extern BOOL	INLINE_All_Inline; /* Mark must-inline 'inline' functions */
BE_EXPORTED extern BOOL	INLINE_Only_Inline; /* Mark may-inline only 'inline' functions */
#endif
BE_EXPORTED extern BOOL	INLINE_Optimize_Alloca;	/* When inlining calls with alloca fix the stack and pop  */
BE_EXPORTED extern BOOL	INLINE_Enable_Copy_Prop; /* Copy Propogation during stand-alone inlining? */
BE_EXPORTED extern BOOL	INLINE_Enable_Subst_Copy_Prop; /* Aggressive substitution of actual for formal and hence Copy Propogation during stand-alone inlining */
BE_EXPORTED extern BOOL     INLINE_Enable_Split_Common;  /* Enable split common: inliner */
BE_EXPORTED extern BOOL     INLINE_F90;  /* Enable recognition of F90 in parameter type compatibility */
BE_EXPORTED extern BOOL	INLINE_None;	/* Inline nothing? */
BE_EXPORTED extern BOOL	INLINE_Exceptions;	/* Inline exception code? */
BE_EXPORTED extern BOOL	INLINE_Keep_PU_Order;	/* Retain input PU order? */
#ifdef TARG_ST
BE_EXPORTED extern BOOL	INLINE_Sort_PU_Order;	/* Sort input PU order? */
#endif
BE_EXPORTED extern BOOL	INLINE_List_Actions;	/* List inline actions? */
BE_EXPORTED extern UINT32   INLINE_Max_Pu_Size;      /* Max size of pu : default 5000 */
BE_EXPORTED extern BOOL	INLINE_Preemptible;	/* Inline preemptible PUs? */
BE_EXPORTED extern BOOL	INLINE_Static;		/* Inline static fns? */
BE_EXPORTED extern BOOL	INLINE_Static_Set;	/* explicitly set */
BE_EXPORTED extern BOOL	INLINE_Aggressive;	/* inline even non-leaf, out-of-loop
					   calls */ 
BE_EXPORTED extern BOOL     INLINE_Enable_DFE;     /* dead function elimination */
BE_EXPORTED extern BOOL     INLINE_Enable_Auto_Inlining;    /* automatic inline analysis */
BE_EXPORTED extern BOOL	INLINE_Enable_Restrict_Pointers; /* allow restrict pointers */

BE_EXPORTED extern struct option_list *INLINE_List_Names;	/* Must/never/file/library 
						   options */
BE_EXPORTED extern struct option_list *INLINE_Spec_Files;	/* Specification files     */
BE_EXPORTED extern UINT32  INLINE_Skip_After;   /* skip inline edges > specified */
BE_EXPORTED extern UINT32  INLINE_Skip_Before;   /* skip inline edges < specified */
BE_EXPORTED extern BOOL    INLINE_Array_Bounds;     /* "conforming" array bounds */
BE_EXPORTED extern BOOL    INLINE_Use_Malloc_Mempool;     /* Use Malloc Mempool for cloning */
BE_EXPORTED extern BOOL    INLINE_Free_Malloc_Mempool;     /* Free Malloc Mempool for cloning */
BE_EXPORTED extern BOOL    INLINE_Inlined_Pu_Call_Graph;   /* lightweight inliner impl 2 -- build only a call graph with PU tagged inline */
BE_EXPORTED extern BOOL    INLINE_Inlined_Pu_Call_Graph2;  /* lightweight inliner impl 3 -- build only a call graph with PU tagged inline and its callers */
BE_EXPORTED extern BOOL    INLINE_Get_Time_Info;          /* Generate timing info for different phases of the inliner */

BE_EXPORTED extern UINT32  IPA_Max_Density;

#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif /* config_ipa_INCLUDED */
