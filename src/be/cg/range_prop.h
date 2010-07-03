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
