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

// This file collects all data that targinfo need to initialize for a
// specific target.
#ifndef gccfe_targinfo_interface_INCLUDED
#define gccfe_targinfo_interface_INCLUDED
#include "gcc_config.h" // ../../gccfe/gnu
#ifdef __cplusplus
extern "C" {
#endif
//Return GCC register id
extern int GCCTARG_Gp_Arg_Regnum(void);
extern int GCCTARG_Return_Pointer_Regnum(void);
extern int GCCTARG_Stack_Pointer_Regnum(void);
extern int GCCTARG_Static_last_Regnum(void);
extern int GCCTARG_First_Branch_Regnum(void); 
extern int GCCTARG_Last_Branch_Regnum(void); 
extern int GCCTARG_First_Int_Regnum(void); 
extern int GCCTARG_Last_Int_Regnum(void);

//Funcs returning data needed by gccfe
extern gcc_register_map_t *GCCTARG_Additional_Register_Names();
extern int GCCTARG_Additional_Register_Names_Size();
extern char *GCCTARG_Initial_Call_Used_Regs();
extern char *GCCTARG_Initial_Fixed_Regs();
extern int *GCCTARG_Map_Reg_To_Preg();
extern int GCCTARG_Map_Reg_To_Preg_Size();
extern int GCCTARG_Initial_Number_Of_Registers();

//Configure the bridge between gcc and targinfo
 extern void GCCTARG_Configure_Gcc_From_Targinfo(void);

//function needed to initialize the targinfo with info comming from the extension.
// This function is called by GCCTARG_Configure_Gcc_From_Targinfo
extern void TI_Initialize_Extension_Loader(void);

#ifdef __cplusplus
}
#endif
#endif /* gccfe_targinfo_interface_INCLUDED */

