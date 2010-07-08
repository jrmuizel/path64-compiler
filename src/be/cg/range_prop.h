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
 * Module: range-propagation.h
 *
 * Description:
 *
 * Interface to range propagation optimization.
 *
 * Exported functions:
 *
 *   void RangePropagate()
 *
 *     Perform range propagation on the current SSA region.
 *
 *   BOOL TARG_RangePropagate (const RangeAnalysis &, OP *op, OPS *ops)
 *
 *     Perform target-specific range propagation on instruction op.
 *     If op has a target-specific transformation, place the new
 *     instruction(s) in ops and return TRUE; otherwise return FALSE.
 *
 * ====================================================================
 */

#ifndef RANGE_PROP_H_INCLUDED
#define RANGE_PROP_H_INCLUDED

#include "range_analysis.h"

extern OP *Range_single_use(const RangeAnalysis &range_analysis, TN *tn);
extern void RangePropagate();

extern BOOL TARG_RangePropagate (const RangeAnalysis &range_analysis,
				 OP *op, OPS *ops);

#endif /* RANGE_PROP_H_INCLUDED */
