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

/*
 * API used by both Front-end and Back-end to load extension dlls
 * and initialize common structures (mtypes, intrinsics)
 */
#ifndef ISA_LOADER_H
#define ISA_LOADER_H

#ifdef TARG_ST

#include "tracing.h"
#include "opcode.h"
#include "extension_include.h"
#include "ext_info.h"

#include "dyn_isa_api_access.h"
#include "dll_loader.h"

#include "symtab_idx.h"
#include "targ_sim.h"
#include "targ_isa_parse.h"
#include "targ_isa_decode.h"
#include "targ_isa_relocs.h"
#include "register_preg.h"
#include "targ_isa_binutils.h"

#include "ti_si.h"

// ==================================================================
// This structure contains information shared by the various targinfo 
// initializer functions
// ==================================================================
struct Lai_Loader_Info {
  BOOL                             trace_on; // debug trace on?
  int                              nb_ext;
  EXTENSION_ISA_Info            ** ISA_tab;
  const Extension_dll_handler_t  * handler_tab;

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

/* Return a pointer to the newly allocated API used to access to the 
   ISA description of the specified extension. */
extern EXTENSION_ISA_Info *Generate_EXTENSION_ISA_Info(const Extension_dll_handler_t *dll_instance, BOOL verbose);

#include "isa_loader_api.h"

// Table to save PREG -> reg number association
extern int *extension_Preg_To_RegNum_table;
// Table containing Rclass to preg min offset
extern INT *extension_RegClass_To_Preg_Min_Offset_table;

/* Isa register init */
BE_EXPORTED extern void Initialize_ISA_RegisterClasses(Lai_Loader_Info_t &ext_info);

/* ABI init */
BE_EXPORTED extern void Initialize_ABI_Properties(Lai_Loader_Info_t &ext_info);

#ifdef BACK_END

extern Lai_Loader_Info_t * Get_Lai_Loader_Info_Table();

/* Load RAW targinfo */
extern bool Load_Lai_Loader_Info();

#endif  /* defined (BACK_END) */

#endif /* TARG_ST */

#endif /* ISA_LOADER_H */

