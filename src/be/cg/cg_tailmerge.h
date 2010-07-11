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
/** 
 * @file   opt_tailmerge.h
 * @author Quentin COLOMBET
 * @date   Wed Jan 24 13:56:36 2007
 * @brief  Defines API used to apply tailmerge algorithm PU at CGIR level
 */
#ifndef __CG_TAILMERGE_H__
#define __CG_TAILMERGE_H__

/**
 * Apply tailmerge algorithm to current program unit
 *
 * @param  phase Specify current phase. Useful to apply several times tailmerge
 *         and acitivate or block it with a command line switch
 *         (-CG:tailmerge=..)
 *
 * @pre    PU <> NULL
 * @post   CG_tailmerge & phase implies PU has been tailmerged
 *
 * @see    tailmerge.h for the detail of tailmerge algorithm (CTailmerge class)
 */
extern void Tailmerge(INT phase);

#endif
