/*

  Copyright (C) 2005-2006 ST Microelectronics, Inc.  All Rights Reserved.

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
  address, or:

  http://www.st.com

*/

/* ====================================================================
 *
 * Module: wn_lower_private.h
 *
 * Description:
 *
 * This file provides the private interface of the wn_lower.cxx module
 * that can be used by the different lowering action modules.
 * This interface should be only used locally to the lowering modules and is
 * not exported outside of be.so.
 *
 * For the description of the functions declared here, refer to the
 * implementation files.
 *
 */

#ifndef wn_lower_private_INCLUDED
#define wn_lower_private_INCLUDED

#include "wn_lower.h"	// For LOWER_ACTIONS

/* 
 * Implementation from wn_lower.cxx.
 */
extern WN *lower_scf(WN *, WN *, LOWER_ACTIONS);
extern WN *lower_expr(WN *, WN *, LOWER_ACTIONS);
extern WN *lower_stmt(WN *, WN *, LOWER_ACTIONS);

/* 
 * Implementation from rt_lower_wn.cxx.
 */
extern WN *rt_lower_expr(WN *, LOWER_ACTIONS);
extern void rt_lower_stmt(WN *, LOWER_ACTIONS);

#endif
