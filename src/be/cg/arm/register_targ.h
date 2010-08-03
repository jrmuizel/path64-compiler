/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/

/* ====================================================================
 *   NOTE: should only be included by ../cg/register.h
 * ====================================================================
 */

#ifndef targ_register_INCLUDED
#define targ_register_INCLUDED

// Assembly name for this register class.
inline const char* ISA_REGISTER_CLASS_ASM_Name (ISA_REGISTER_CLASS rc) {
  FmtAssert(ISA_REGISTER_CLASS_MIN <= rc && rc <= ISA_REGISTER_CLASS_MAX,("Unknown register class %d",rc));
  return ISA_REGISTER_CLASS_INFO_Name(ISA_REGISTER_CLASS_Info(rc));
}

// Returns TRUE if the rclass is the predicate register class.
inline BOOL Is_Predicate_REGISTER_CLASS(ISA_REGISTER_CLASS rclass) {
  return FALSE;
}

// Returns TRUE if stacked register set exists for <rclass>.
inline BOOL REGISTER_Has_Stacked_Registers(ISA_REGISTER_CLASS rclass) {
  return FALSE;
}

// Returns TRUE if rotating register set exists for <rclass>.
inline BOOL REGISTER_Has_Rotating_Registers(ISA_REGISTER_CLASS rclass) {
  return FALSE;
}

// Declare target-specific register-class, register pairs:

#endif /* targ_register_INCLUDED */
