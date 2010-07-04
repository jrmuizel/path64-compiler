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

/* ====================================================================
 *   NOTE: should only be included by ../cg/register.h
 * ====================================================================
 */

#ifndef targ_register_INCLUDED
#define targ_register_INCLUDED
#include "targ_abi_properties.h"
#if 0
// Assembly name for this register class.
inline const char* ISA_REGISTER_CLASS_ASM_Name (ISA_REGISTER_CLASS rc) {
  FmtAssert(ISA_REGISTER_CLASS_MIN <= rc && rc <= ISA_REGISTER_CLASS_MAX,("Unknown register class %d",rc));
  return ISA_REGISTER_CLASS_INFO_Name(ISA_REGISTER_CLASS_Info(rc));
}

// Returns TRUE if the rclass is the predicate register class.
inline BOOL Is_Predicate_REGISTER_CLASS(ISA_REGISTER_CLASS rclass) {
  return FALSE;
}
#endif
// Returns TRUE if stacked register set exists for <rclass>.
inline BOOL REGISTER_Has_Stacked_Registers(ISA_REGISTER_CLASS rclass) {
  return FALSE;
}

// Returns TRUE if rotating register set exists for <rclass>.
inline BOOL REGISTER_Has_Rotating_Registers(ISA_REGISTER_CLASS rclass) {
  return FALSE;
}

extern void REGISTER_Init_Stacked(ISA_REGISTER_CLASS rclass);
extern REGISTER REGISTER_Request_Stacked_Register(INT has_abi_property,
						  ISA_REGISTER_CLASS rclass);
extern REGISTER REGISTER_Allocate_Stacked_Register(INT has_abi_property,
						   ISA_REGISTER_CLASS rclass,
						   REGISTER reg);
// unallocate a stacked register such that it is not re-used.
extern void REGISTER_Unallocate_Stacked_Register (
   ISA_REGISTER_CLASS rclass, REGISTER reg);

// return whether stacked register is allocatable
extern BOOL REGISTER_Is_Allocatable_Stacked_Register (
   ISA_REGISTER_CLASS rclass, REGISTER reg);

extern REGISTER_SET REGISTER_Get_Stacked_Avail_Set(INT has_abi_property,
						   ISA_REGISTER_CLASS rclass);
extern BOOL REGISTER_Is_Stacked_Output(ISA_REGISTER_CLASS rclass, REGISTER reg);
extern BOOL REGISTER_Is_Rotating(ISA_REGISTER_CLASS rclass, REGISTER reg);
extern BOOL REGISTER_Is_Stacked(ISA_REGISTER_CLASS rclass, REGISTER reg);
extern BOOL REGISTER_Is_Stacked_Local(ISA_REGISTER_CLASS rclass, REGISTER reg);
extern REGISTER REGISTER_Translate_Stacked_Output(REGISTER reg);
extern INT REGISTER_Number_Stacked_Output (ISA_REGISTER_CLASS);
extern INT REGISTER_Number_Stacked_Local (ISA_REGISTER_CLASS);
extern INT REGISTER_Number_Stacked_Rotating (ISA_REGISTER_CLASS);
extern INT REGISTER_Number_Stacked_Registers_Available (ISA_REGISTER_CLASS);

extern void REGISTER_Reserve_Rotating_Registers(ISA_REGISTER_CLASS rclass,
						INT n);
extern void REGISTER_Set_Stacked_Output_Minimum(ISA_REGISTER_CLASS rclass,
						INT num);

extern char * REGISTER_Stacked_Output_Name (REGISTER reg);

extern REGISTER REGISTER_First_Rotating_Registers(ISA_REGISTER_CLASS rclass);
extern REGISTER REGISTER_Last_Rotating_Registers(ISA_REGISTER_CLASS rclass);
extern void REGISTER_Request_Stacked_Rotating_Register();

inline BOOL Is_Predicate_REGISTER_CLASS(ISA_REGISTER_CLASS rclass) {
  return FALSE;
}

extern REGISTER_SET REGISTER_Get_Requested_Rotating_Registers(ISA_REGISTER_CLASS rclass);
// Declare target-specific register-class, register pairs:

#endif /* targ_register_INCLUDED */
