/* 

  Copyright (C) 2006 ST Microelectronics, Inc.  All Rights Reserved. 

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

  Contact information:  ST Microelectronics, Inc., 
  , or: 

  http://www.st.com 

  For further information regarding this notice, see: 

  http: 
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
