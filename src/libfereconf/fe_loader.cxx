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
   The contents of this file are used by both the gcc side and the
   wgen side of the gcc/wgen frontend. */

#include "errors.h"
#include "config.h"
#include "gccfe_targinfo_interface.h" // TB: Draft of an ABI between targinfo and gccfe
#include "intrn_info.h"
#include "opcode.h"
#include "fe_loader.h"

#include "symtab_idx.h"
#include "targ_sim.h"

// Mapping between gcc reg and open64 PREG
int *Map_Reg_To_Preg;
int Map_Reg_To_Preg_Size;

// While the loader has not been initialized, set the counter to the
// max value.  This for the GCC initializaton of opttab_table, which
// occurs before the loader initialization.
#define MAX_LIMIT_MACHINE_MODE (127 + 0)
int COUNT_MACHINE_MODE = MAX_LIMIT_MACHINE_MODE;

//GCC targinfo initialization
void TI_Initialize_Extension_Loader(void) {
  FmtAssert (! Extension_Is_Present, ("Extensions not yet supported"));
  // Here much work is required to do the work that is done in
  // wfe_loader.cxx in the old frontend.
}

BOOL EXTENSION_Are_Equivalent_Mtype(TYPE_ID src_ty, TYPE_ID tgt_ty) {
  FmtAssert (FALSE, ("Extensions not yet supported"));
  return FALSE;
}

BOOL EXTENSION_Is_Defined(const char *extname) {
  FmtAssert (FALSE, ("Extensions not yet supported"));
  return FALSE;
}

INT32 EXTENSION_Get_ExtensionId_From_ExtensionName(const char *extname) {
  FmtAssert (FALSE, ("Extensions not yet supported"));
  return -1;
}

long long
EXTENSION_Get_ExtOption_Flag_By_Name(int extid, const char* optionname) {
  FmtAssert (FALSE, ("Extensions not yet supported"));
  return -1;
}

extern void Add_Intrinsic_for_OPCODE(OPCODE opc, INTRINSIC intrn) {
  FmtAssert (FALSE, ("Reconfigurability not yet supported"));
}

void Init_Mtypes(int nb_mtype_to_add) {
  MTYPE_COUNT = MTYPE_STATIC_LAST;
  FIRST_COMPOSED_MTYPE = MTYPE_COUNT + nb_mtype_to_add + 1;
  
  if (MTYPE_STATIC_LAST + nb_mtype_to_add > MTYPE_MAX_LIMIT) {
    FmtAssert(FALSE,
	      ("Too much dynamic MTYPEs to add. Compilation aborted."));
  }
  if (nb_mtype_to_add) {
    FmtAssert (FALSE, ("Extensions not yet supported"));
  }
}

/*
 * Initialize extension related info required by front-end:
 * - dynamic machine modes,
 * - dynamic builtins,
 */
void WFE_Init_Loader(void)
{
  if (Extension_Is_Present) {
    FmtAssert (! Extension_Is_Present, ("Extensions not yet supported"));
  } else {
    Init_Mtypes    (0); // Open64 MTYPE
  }
  //Dump info
#ifdef TARGET_DUMP_INFO
  if (TARGET_DUMP_INFO) {
    dumpinfofile = fopen("info.txt", "w");
    fprintf(dumpinfofile,"Available Machine Mode\n");
    for (int j = 0; j < NUM_MACHINE_MODES; j++)
      fprintf(dumpinfofile, "\t%s\t%d\t%s\n",GET_MODE_NAME (j), GET_MODE_BITSIZE(j),VECTOR_MODE_P(j) ?"vector":"scalar");
    
    fprintf(dumpinfofile,"Available Builtins\n");
    for (int j = 0; j < BUILT_IN_COUNT; j++)
      fprintf(dumpinfofile, "\t%s\n", built_in_names[j]);

    //Print Additional_Register_Names and Map_Reg_To_Preg
    fprintf(dumpinfofile,"gcc_id\tname\tfixed\tcall_used\n");
    for (int i =0; i < Additional_Register_Names_Size; i++)
      fprintf(dumpinfofile,"[%d]\t%s\t%d\t%d\n",Additional_Register_Names[i].number, Additional_Register_Names[i].name, Fixed_Registers[i], Call_Used_Registers[i]);
  }
#endif
}
