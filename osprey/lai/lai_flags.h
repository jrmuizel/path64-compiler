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


/* =======================================================================
 * =======================================================================
 *
 *  Module: cg_flags.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Description:
 *  ============
 *
 *  Exports variables global to all of CG.
 *
 *  Exported variables:
 *  ==================-
 *
 *  BOOL LAI_tail_call
 *	Enable tail call generation.
 *
 *  BOOL LAI_unique_exit
 *	Generate unique exit blocks.
 *
 *  BOOL LAI_warn_bad_freqs
 *	Whenever a phase notices that freq related data is wrong or
 *      inconsistent, it can warn when this flag is true.
 *
 *  BOOL CG_enable_loop_optimizations
 *	Enable the innermost loop optimizations (CGPREP). This includes
 *	loop unrolling, r/w elimination, recurrence breaking, SWP.
 *
 *  INT32 CG_skip_after
 *  INT32 CG_skip_before
 *  INT32 CG_skip_equal
 *	Controls which PUs we skip optimizing, i.e. we set CG_opt_level=0
 *	for.
 *
 *  INT32 CG_local_skip_after
 *  INT32 CG_local_skip_before
 *  INT32 CG_local_skip_equal
 *	Options to control the optimization done by any phase in CG. The
 *    	interpretation of the numbers to skip is left totally to the 
 *	local phase. For example, the local scheduler uses these options
 *	to control which basic blocks to skip scheduling for.
 *
 *  BOOL CG_skip_local_sched
 *	Enable skipping of scheduling of basic blocks based on the 
 *	-CG:skip_local_[after,before,equal] options.
 *
 *  BOOL CG_skip_local_swp
 *	Enable skipping of pipelining of inner loops based on the 
 *	-CG:skip_local_[after,before,equal] options.
 *
 *  INT CG_opt_level
 *      Gives the optimization level to use throughout CG.
 *
 *  BOOL CG_localize_tns
 *      Make sure we have no global TNs (and thus don't need GRA).
 *  
 *  CG_cond_defs_allowed
 *	Allow generation of conditional definitions (predicated insts)
 *
 *  BOOL CG_enable_reverse_if_conversion
 *  BOOL CG_enable_reverse_if_conversion_overridden
 *      Enable reverse if conversion.
 *  
 *  BOOL CG_enable_thr
 *      Enable tree-height reduction phase in CG.
 * 
 *  BOOL CG_enable_spec_imul
 *      allow loops with speculated integer mul 
 *      to be if converted.
 * 
 *  BOOL CG_enable_spec_idiv
 *      allow loops with speculated integer div 
 *      to be if converted.
 * 
 *  BOOL CG_enable_spec_fdiv
 *      allow loops with speculated fdiv 
 *      to be if converted. recips are also included
 *      with this flag.
 *
 *  BOOL CG_enable_spec_fsqrt
 *      allow loops with speculated fsqrt 
 *      to be if converted.
 *
 *  INT32 CG_maxinss
 *      Maximum number of ops in a loop to be if converted.
 *
 *  INT32 CG_maxblocks
 *      Maximum number of bbs in a loop to be if converted.
 *
 *  BOOL FREQ_enable
 *	Enable BB freqency estimates.
 *
 *  BOOL FREQ_view_cfg
 *	Indicates if daVanci should be invoked to view the frequency
 *	annotated CFG at appropriate times.
 *
 *  const char *FREQ_frequent_never_ratio
 *	The ratio in the probabilities of "frequent" to "never" successors
 *	tagged with frequency hint pragmas.
 *
 *  const char *FREQ_eh_freq
 *	The frequency (relative to the entry point) that an exception
 *	handler is executed.
 *
 *  EMIT_pjump_all
 *	Generate a PJUMP relocation for all calls, not just the 
 *	non-preemptible ones. This allows ld to change jalr to jal
 *	for calls that it can determine are non-preemptible, even
 *	though CG cannot tell. This change can be enabled by default
 *	only when the corresponding ld change is done. Defer making
 *	it default till 7.3 to prevent problems with 7.2 objects
 *	being linked with an earlier ld (default false)
 *
 *  EMIT_use_cold_section
 *	Put code region BBs into .text.cold. Turn off for debugging...
 *	(default true)
 *
 *  EMIT_interface_section
 *	Emit interface section (default true).
 *
 * =======================================================================
 * =======================================================================
 */

#ifndef lai_flags_INCLUDED
#define lai_flags_INCLUDED


extern BOOL LAI_warn_bad_freqs;
extern BOOL LAI_localize_tns;

extern INT32 CG_skip_after;
extern INT32 CG_skip_before;
extern INT32 CG_skip_equal;
extern INT32 CG_local_skip_after;
extern INT32 CG_local_skip_before;
extern INT32 CG_local_skip_equal;
extern INT CG_opt_level;

extern BOOL LAI_enable_feedback;

extern BOOL LAI_tail_call;
extern BOOL LAI_unique_exit;

/* FREQ:
 */
extern BOOL FREQ_enable;

/*
 * CGEXPand flags 
 */
extern BOOL EXP_gp_prolog_call_shared;
extern BOOL EXP_normalize_logical;
extern const char *EXP_sqrt_algorithm;

extern BOOL LAI_emit_asm_dwarf;
extern BOOL LAI_emit_unwind_directives;
extern BOOL LAI_emit_unwind_info;
extern BOOL LAI_emit_use_cold_section;

extern BOOL LAI_emit_stack_frame;

/* =======================================================================
 *   Tracing Flags
 * =======================================================================
 */
#define LAI_EXPAND    1

#endif /* lai_flags_INCLUDED */



