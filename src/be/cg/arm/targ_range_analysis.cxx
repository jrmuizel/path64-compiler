/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Copyright (C) STMicroelectronics All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

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
#define __STDC_LIMIT_MACROS
#include <stdint.h>


#include "defs.h"
#include "tracing.h"
#include "config_targ.h"
#include "cgir.h"
#include "range_analysis.h"
#include "lrange.h"

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
