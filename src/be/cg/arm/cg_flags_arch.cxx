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


#include "defs.h"
#include "cg_flags.h"
#include "cg_loop.h"      /* for cg_loop flags */
#include "cio.h"          /* for cio_rwtran flags */

void Configure_CG_Target() {

 
  // ??
  CG_enable_thr = FALSE;
 
  // Disable loop optimizations by default until debugged
  // CG_enable_loop_optimizations = TRUE;

  // Do not unroll fully until tested
  //  CG_LOOP_unroll_fully = TRUE;

  // Do not force if-conversion for loops, see CG_LOOP_Optimize()
  CG_LOOP_force_ifc = 0;
 
  // CFLOW optimizations: ON by default
 
  // disable CFLOW second pass after HB formation
  //CFLOW_opt_after_cgprep = FALSE;

  // Scheduling options: scheduling is ON by default -- disable if
  //                     in development

  // disable HB scheduling
  IGLS_Enable_HB_Scheduling = FALSE;
  IGLS_Enable_PRE_HB_Scheduling = FALSE;
  IGLS_Enable_POST_HB_Scheduling = FALSE;

  // disable local BB scheduling
  //LOCS_Enable_Scheduling = FALSE;
  //LOCS_PRE_Enable_Scheduling = FALSE;
  //LOCS_POST_Enable_Scheduling = FALSE;

  // Bundle formation is OFF by default
  LOCS_Enable_Bundle_Formation = TRUE;

  // GCM settings: ON by default
  GCM_Enable_Scheduling = FALSE;
  GCM_Speculative_Loads = FALSE;
  GCM_Predicated_Loads = FALSE;
  GCM_Motion_Across_Calls = FALSE;
  GCM_Min_Reg_Usage = TRUE;
  GCM_Pointer_Spec = FALSE;
  GCM_Eager_Ptr_Deref = FALSE;
  GCM_Test = FALSE;
  GCM_Enable_Cflow = FALSE;
  GCM_PRE_Enable_Scheduling = FALSE;
  GCM_POST_Enable_Scheduling = FALSE;
  GCM_Enable_Fill_Delay_Slots = FALSE;

  // Reorder while LRA
  LRA_do_reorder = FALSE;

  // HB stuff: everything's OFF by default
#ifdef IFCONV_IN_SSA
  // OFF until we know it's worth it (activate in command line)
  //HB_formation = TRUE; 
  // Superblock formation -- not implemented right now
  // HB_superblocks = TRUE;
  // Do simple hammock if-conversion
  HB_simple_ifc = TRUE;
  // Do not skip hammocks -- originally designed to go around a 
  // performance bug in IA64
  HB_skip_hammocks = FALSE;      /* do not skip hammock if-conv. */
#else
  HB_superblocks = TRUE;         /* make superblocks */
  HB_complex_non_loop = TRUE;
  HB_skip_hammocks = FALSE;      /* do not skip hammock if-conv. */
#endif

  // May set this temporary OFF for development
  //  CIO_enable_copy_removal = TRUE;
  //  CIO_enable_read_removal = TRUE;
  //  CIO_enable_write_removal = TRUE;

  /* For STXP70, we do not insert stop bit for asm statements. */
  EMIT_stop_bits_for_asm = FALSE;
  EMIT_stop_bits_for_volatile_asm = FALSE;

  return;
}
