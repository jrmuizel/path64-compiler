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
/* ====================================================================
 *   Is_Predicate_REGISTER_CLASS
 *   Returns TRUE if the rclass is the predicate register class.
 * ====================================================================
 */
inline BOOL Is_Predicate_REGISTER_CLASS(ISA_REGISTER_CLASS rclass) {
  // Should get subclass cpsrcond
  return rclass == ISA_REGISTER_CLASS_cpsr;
}

extern REGISTER_SET REGISTER_Get_Requested_Rotating_Registers(ISA_REGISTER_CLASS rclass);
// Declare target-specific register-class, register pairs:
#include "defs.h"
#include "targ_isa_registers.h"
#ifdef TARG_ST
#include "targ_register_common.h"
#endif

// Initialization for targ_register.
extern void CGTARG_REGISTER_Begin(void);
extern void CGTARG_REGISTER_Pu_Begin(void);
extern void CGTARG_Initialize_Register_Class(ISA_REGISTER_CLASS rclass);

// Target dependent implemetation of some functions above.
extern ISA_REGISTER_CLASS CGTARG_Register_Class_For_Mtype(TYPE_ID mtype);
extern ISA_REGISTER_CLASS CGTARG_Register_Subclass_For_Mtype(TYPE_ID mtype);
#ifdef TARG_ST
// [TTh] Now defined in 'targ_register_common.h'
#else
extern ISA_REGISTER_CLASS CGTARG_Register_Class_Num_From_Name (char *regname, int *regnum);
#endif


// Returns the set of forbidden registers for GRA.
// We forbid some registers from being used by the GRA. Such is the
// case of the $r0.63 on st200 which is a dedicated register for 
// the igoto instruction. GRA should never allocate this register 
// to a global live-range (unless dedicated to); or otherwise LRA
// may not be able to spill a local liverange (like the one used
// for an igoto) because of "spill beyond the end of BB, etc.
extern REGISTER_SET CGTARG_Forbidden_GRA_Registers(ISA_REGISTER_CLASS rclass);
// [SC] The set of forbidden registers for LRA.
extern REGISTER_SET CGTARG_Forbidden_LRA_Registers(ISA_REGISTER_CLASS rclass);

#ifdef TARG_ST
// [TTh] The set of forbidden registers for prolog/epilog temporaries.
extern REGISTER_SET CGTARG_Forbidden_Prolog_Epilog_Registers(ISA_REGISTER_CLASS rclass);
#endif


// Returns the set of registers that will be selected in priority by
// GRA (resp. LRA).
// This may be used for instance to select some registers subjects
// to multi load/store operations first.
// This is NOT used for instance to select first caller or callee saved
// register, this is handled specially in GRA or LRA.
extern REGISTER_SET CGTARG_Preferred_GRA_Registers(ISA_REGISTER_CLASS rclass);
extern REGISTER_SET CGTARG_Preferred_LRA_Registers(ISA_REGISTER_CLASS rclass);

extern const char *CGTARG_DW_DEBUG_Get_Extension_Reloc_String(ISA_REGISTER_CLASS rclass);
extern const char *CGTARG_DW_DEBUG_Get_Extension_Name(ISA_REGISTER_CLASS rclass);
extern INT32 CGTARG_DW_DEBUG_Get_Extension_Id(ISA_REGISTER_CLASS rclass, 
					      BOOL *activated);
extern INT32 CGTARG_DW_DEBUG_Get_Reg_Id  (ISA_REGISTER_CLASS rclass,
					  INT32  index,
					  UINT32 bit_size);
extern const char *CGTARG_DW_DEBUG_Get_Reg_Name(ISA_REGISTER_CLASS rclass,
                                                INT32  index,
                                                UINT32 bit_size);
extern void CGTARG_DW_DEBUG_Begin(void);


#endif /* targ_register_INCLUDED */
