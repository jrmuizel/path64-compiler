/*

  Copyright (C) 2001 ST Microelectronics, Inc.  All Rights Reserved.

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

  Contact information:  

  http://www.st.com

  For further information regarding this notice, see:

  http://

*/

#include "model.h"

/* ====================================================================
 *   MTYPE_Needs_Regs
 *
 *   How many registers does given MTYPE require on the given target.
 * ====================================================================
 */
INT
MTYPE_Needs_Regs (
  TYPE_ID mtype
)
{
  switch (mtype) {

  case MTYPE_I1:
  case MTYPE_U1:
  case MTYPE_I2:
  case MTYPE_U2:
  case MTYPE_I4:
  case MTYPE_U4:
  case MTYPE_I5:
  case MTYPE_U5:
    return 1;

  case MTYPE_I8:
  case MTYPE_U8:
    return 2;

  case MTYPE_A4:
    return 1;

  case MTYPE_B:
    return 1;

  default:
    FmtAssert(FALSE,("MTYPE_Needs_Regs: mtype %s", MTYPE_name(mtype)));
  }
}
