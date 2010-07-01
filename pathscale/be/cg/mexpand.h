/*
  Copyright (C) 2005, STMicroelectronics, All Rights Reserved.

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
 * Module: mexpand.h
 *
 * Description:
 *
 *   External interface to the "multi-op" transformations.
 *
 * Entry-point:
 *
 *   void Convert_To_Multi_Ops
 *	Perform "multi-op" conversion on the current region.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef MEXPAND_H_INCLUDED
#define MEXPAND_H_INCLUDED

#include "bb.h"

extern void Convert_To_Multi_Ops ();

extern void Convert_BB_To_Multi_Ops (BB *);

#endif /* MEXPAND_H_INCLUDED */
