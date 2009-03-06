/*
  Copyright (C) 2006, STMicroelectronics, All Rights Reserved.

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
*/

/* ====================================================================
 * ====================================================================
 *
 * Module: targ_range_analysis.cxx
 *
 * Description:
 *
 * Target-specific code for range analysis.
 *
 * ====================================================================
 */

#include "defs.h"
#include "tracing.h"
#include "config_target.h"
#include "cgir.h"
#include "range_analysis.h"
#include "lrange.h"
#include "targ_cg_private.h"

// Process result_idx'th result of op if it needs target-specific
// handling.
// Return TRUE if it has been processed, FALSE if it does not need
// target-specific processing.
BOOL
RangeAnalysis::TARG_Visit_Forward (OP *op, INT result_idx, LRange_pc &new_value,
				   BOOL *succs_done)
{
  return FALSE;
}

// Process opnd_idx'th operand of op if it needs target-specific
// handling.
// Return TRUE if it has been processed, FALSE if it does not need
// target-specific processing.
BOOL
RangeAnalysis::TARG_Visit_Backward (OP *op, INT opnd_idx, LRange_pc &new_value)
{
  return FALSE;
}
