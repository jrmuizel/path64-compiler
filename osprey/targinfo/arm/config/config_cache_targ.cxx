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


// This may look like C code, but it is really -*- C++ -*-
//
// ====================================================================
// ====================================================================
//
// Module: config_targ_cache.cxx
// Revision history:
//  14-Nov-96 - Original Version, copied from cache_parameters.cxx.
//
// Description:
//
// Target specific logic for config_cache.cxx.
//
// ====================================================================
// ====================================================================

#ifdef _KEEP_RCS_ID
static const char source_file[] = __FILE__;
#endif /* _KEEP_RCS_ID */

#include "defs.h"
#include "errors.h"
#include "config_cache.h"
#include "config_lno.h"
#include "config_target.h"
#include "config_platform.h"

void MHD::Initialize()
{
  if ( Is_Target_armv5() || Is_Target_armv6() ) {

    Non_Blocking_Loads      = FALSE;
    Loop_Overhead_Base      = 1;
    Loop_Overhead_Memref    = 1;
    DCache_Prefetch_Buffers = 0;
    TLB_Trustworthiness     = 0;
    TLB_NoBlocking_Model    = FALSE;
    
    MHD_LEVEL tmp(MHD_TYPE_NONE,    // Type
		  -1,               // Size
		  -1,               // Line Size
		  -1,               // Clean Miss Penalty ??
		  -1,               // Dirty Miss Penalty ??
		  -1,               // Associativity
		  -1,               // TLB Entries
		  -1,               // Page Size
		  -1,               // TLB Clean Miss Penalty
		  -1,               // TLB Dirty Miss Penalty
		  -1.0,              // Typical Outstanding Loads
		  // (1.0 means no overlap)
		  -1.0,              // Load_OP_Overlap_1
		  // (0.0 means blocking cache)
		  -1.0,              // Load_OP_Overlap_2
		  // (0.0 means blocking cache)
		  -1);               // Pct_Excess_Writes_Nonhidable ??    

    L[0] = tmp;

#ifdef Is_True_On
    if (LNO_Verbose)
      printf ("Target Processor: TARGET_arm!. %lld (%d)\n", 
              L[0].Effective_Size, L[0].Line_Size);
#endif
  } else {
    FmtAssert(FALSE, ("Unknown target in MHD::Initialize\n"));
  }
}
