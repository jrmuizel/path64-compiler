//-*-c++-*-
/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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

/** 
 * @file   opt_tailmerge.h
 * @author Quentin COLOMBET
 * @date   Wed Jan 24 13:56:36 2007
 * @brief  Defines API used to apply tailmerge algorithm to a control graph flow
 *         at WOPT level
 */

#ifndef __OPT_TAILMERGE_H__
#define __OPT_TAILMERGE_H__

// Forward declaration
class CFG;
class WN;

/**
 * Apply tailmerge algorithm to given cfg.
 *
 * @param  cfg Control graph flow of the given whirl representation
 * @param  wn_tree WOPT whirl representation
 * @param  phase current processing phase
 *
 * @pre    cfg represents wn_tree and wn_tree <> NULL
 * @post   cfg has been tailmerged
 *
 * @see    tailmerge.h for the detail of tailmerge algorithm (CTailmerge class)
 */
extern void
OPT_Tailmerge(COMP_UNIT* comp_unit, WN* wn_tree, int phase);

#endif
