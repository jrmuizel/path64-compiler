/* 

  Copyright (C) 2006,2007 ST Microelectronics, Inc.  All Rights Reserved. 

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

//TB: put internal data type def common to the gcc loader and the lai
//loader


#ifndef LOADER_INTERNAL_H
#define LOADER_INTERNAL_H
#include "dyn_isa_api_access.h"
#include "dyn_dll_api_access.h"

// ==================================================================
// This structure contains information shared by the various targinfo 
// initializer functions
// ==================================================================
struct Lai_Loader_Info {
  BOOL                     trace_on; // debug trace on?
  int                      nb_ext;
  EXTENSION_ISA_Info     **ISA_tab;
  const Extension_dll_t   *dll_tab;

  // Base offsets for each element of each extension
  // (only those that need to be global are here)
  int  *base_TOP;
  int  *base_INTRINSIC;
  int  *base_LIT_CLASS;
  int  *base_ENUM_CLASS;
  int  *base_ENUM_CLASS_VALUE;
  int  *base_REGISTER_CLASS;
  int  *base_REGISTER_SUBCLASS;
  int  *base_PREG;
};

typedef struct Lai_Loader_Info Lai_Loader_Info_t;

// ===================
// Register class data
// ===================
// Table to save PREG -> reg number association
extern int *extension_Preg_To_RegNum_table;
// Table containing Rclass to preg min offset
extern INT *extension_RegClass_To_Preg_Min_Offset_table;

/*
 * Function called when an extension is incompatible with the core description
 */
extern void RaiseErrorIncompatibleLibrary(const char *name, const char *error_msg);

#endif /* LOADER_INTERNAL_H */
