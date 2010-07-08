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
#include "targ_isa_registers.h"

/* ====================================================================
 *   CGTARG_Register_Class_Num_From_Name
 *
 *   given register name (asm), get its register class.
 *   returns ISA_REGISTER_CLASS_UNDEFINED if not register match the name.
 * ====================================================================
 */
ISA_REGISTER_CLASS 
CGTARG_Register_Class_Num_From_Name (const char *regname, int *regnum);

/* ====================================================================
 *   CGTARG_DW_DEBUG_Get_Reg_Id 
 *   Returns the name of the dwarf debug identifier of a register.
 * ====================================================================
 */
extern INT32 CGTARG_DW_DEBUG_Get_Reg_Id  (ISA_REGISTER_CLASS rclass,
					  INT32  index,
					  UINT32 bit_size);
