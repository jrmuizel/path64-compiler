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
 *  Module: cg_flags.c
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Description:
 *  ============
 *
 *  Definition of variables global to all of CG.
 *
 * =======================================================================
 * =======================================================================
 */


#include "defs.h"
#include "lai_flags.h"

BOOL LAI_warn_bad_freqs = FALSE;
BOOL LAI_localize_tns;

INT32 CG_skip_after = INT32_MAX;
INT32 CG_skip_before = 0;
INT32 CG_skip_equal = -1;
INT32 CG_local_skip_after = INT32_MAX;
INT32 CG_local_skip_before = 0;
INT32 CG_local_skip_equal = -1;
INT   CG_opt_level;
BOOL CG_localize_tns;

BOOL LAI_enable_feedback = TRUE;
BOOL LAI_tail_call = TRUE;
BOOL LAI_unique_exit = TRUE;

BOOL CFLOW_Enable = TRUE;

BOOL FREQ_enable = TRUE;
BOOL FREQ_view_cfg = FALSE;
const char *FREQ_frequent_never_ratio = "1000.0";
const char *FREQ_eh_freq = "0.1";

BOOL EXP_gp_prolog_call_shared = TRUE;
BOOL EXP_normalize_logical = TRUE;
const char *EXP_sqrt_algorithm = "st100";

BOOL LAI_emit_asm_dwarf    = FALSE;
BOOL LAI_emit_unwind_info  = TRUE;
BOOL LAI_emit_unwind_directives = FALSE;
BOOL LAI_emit_use_cold_section = FALSE;

// .lai does not need to have the stack layout be emitted.
// when generating executable code, we must do it.
BOOL LAI_emit_stack_frame = TRUE;
