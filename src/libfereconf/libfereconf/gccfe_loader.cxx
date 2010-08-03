/* 

  Copyright (C) 2009 ST Microelectronics, Inc.  All Rights Reserved. 

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

/* This file is in charge of providing extension specific feature
   The contents of this file are used only by the gcc side of the
   gcc/wgen frontend. */

#include "errors.h"
#include "config.h"
#include "gccfe_targinfo_interface.h" // TB: Draft of an ABI between targinfo and gccfe
#include "intrn_info.h"
#include "opcode.h"
#include "fe_loader.h"

#include "symtab_idx.h"
#include "targ_sim.h"

extern "C" {
extern int Additional_Register_Names_Size;
extern gcc_register_map_t *Additional_Register_Names;
extern char *Call_Used_Registers;
extern char *Fixed_Registers;
extern int Number_Of_Registers;
};

/* ===========================================================================
 * This function initializes all gcc arrays for register properties,
 * These arrays are filled with targinfo data.
 ===========================================================================*/
void WFE_Loader_Initialize_Register () {
  int i, j;
  //Find the last gcc id of the last register of the core For that 2
  //solutions: find the max index in Additional_Core_Register_Names or the
  //size of Map_Reg_To_Preg / sizeof(PREG_NUM) -1
  int last_gcc_id = 0;
  int add_size = GCCTARG_Additional_Register_Names_Size();
  gcc_register_map_t *add_names = GCCTARG_Additional_Register_Names();

  char *call_used = GCCTARG_Initial_Call_Used_Regs();
  int reg_size = GCCTARG_Initial_Number_Of_Registers();

  char *fixed = GCCTARG_Initial_Fixed_Regs();

  int map_reg_size = GCCTARG_Map_Reg_To_Preg_Size();
  int *map_reg = GCCTARG_Map_Reg_To_Preg();

  for (i = 0; i < add_size; i++) {
    last_gcc_id = (last_gcc_id > add_names[i].number) ? last_gcc_id : add_names[i].number;
  }
  // Check that both solutions give the same result
  if (last_gcc_id != map_reg_size - 1)
    Fail_FmtAssertion("Map_Reg_To_Preg in config_target.cxx and Additional_Register_Names are not compatible");

  // Check that last_dedicated_preg_offset == ARRAY_SIZE(Map_Reg_To_Preg)
  if (Last_Dedicated_Preg_Offset != map_reg_size)
    Fail_FmtAssertion("Last_Dedicated_Preg_Offset and ARRAY_SIZE(Map_Reg_To_Preg) are not compatible");

  // Allocate memory for the new arrays
  // Register names
#if 0
  Additional_Register_Names = TYPE_MEM_POOL_ALLOC_N(gcc_register_map_t, Malloc_Mem_Pool,
						    add_size);
  memcpy(Additional_Register_Names, add_names , add_size * sizeof(gcc_register_map_t));
#endif
  Additional_Register_Names = add_names;
  Additional_Register_Names_Size = add_size;

  // Call used register array
#if 0
  Call_Used_Registers = TYPE_MEM_POOL_ALLOC_N(char, Malloc_Mem_Pool,
					      reg_size);
  memcpy(Call_Used_Registers, call_used , reg_size * sizeof(char));
#endif
  Call_Used_Registers = call_used;
  Number_Of_Registers = reg_size;

  // Fixed register array
#if 0
  Fixed_Registers = TYPE_MEM_POOL_ALLOC_N(char, Malloc_Mem_Pool,
					      Number_Of_Registers);
  memcpy(Fixed_Registers, fixed , reg_size * sizeof(char));
#endif
  Fixed_Registers = fixed;

  // register to open64 PREG mapping
#if 0
  Map_Reg_To_Preg = TYPE_MEM_POOL_ALLOC_N(PREG_NUM, Malloc_Mem_Pool,
					  map_reg_size);
  memcpy(Map_Reg_To_Preg, map_reg , map_reg_size * sizeof(PREG_NUM));
#endif
  Map_Reg_To_Preg = map_reg;
  Map_Reg_To_Preg_Size = map_reg_size;
}

