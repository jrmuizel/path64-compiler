/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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
/*
 * API used by both Front-end and Back-end to load extension dlls
 * and initialize common structures (mtypes, intrinsics)
 */
#ifndef ISA_LOADER_H
#define ISA_LOADER_H

#ifdef TARG_ST

#include "tracing.h"
#include "opcode.h"
#include "ext_info.h"


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

