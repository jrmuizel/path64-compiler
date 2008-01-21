/* 

  Copyright (C) 2007 ST Microelectronics, Inc.  All Rights Reserved. 

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
 * This modules defines the common API used to access the high level description of extensions.
 * It is compatible with the successives library API revisions and insure the migration
 * to the latest one, hiding the implemention details to the other parts of thecompiler.
 */
#ifndef _DYN_DLL_API_ACCESS_H_
#define _DYN_DLL_API_ACCESS_H_

#ifndef DLL_EXPORTED
#define DLL_EXPORTED 
#endif

#include "dyn_dll_api.h"

// Return TRUE if the specified revision is compatible with current compiler,
// FALSE otherwise.
BOOL EXTENSION_Is_Supported_HighLevel_Revision(INT hooks_rev);


// #############################################################################
// ##
// ## Class: EXTENSION_HighLevel_Info
// ## Descr: Define API used to access high level information (types and builtins)
// ##        of the extension
// ##
// #############################################################################
class EXTENSION_HighLevel_Info {
 public:
  EXTENSION_HighLevel_Info(const extension_hooks *input_hooks);
  ~EXTENSION_HighLevel_Info();

  /* Return magic number initially found in library description */
  inline mUINT32  get_initial_revision(void) const;

  /* ==========================
     ==  MTYPES information  ==
     ========================== */
  /* Return a pointer on the modes array.  */
  inline const extension_machine_types_t * get_modes (void) const;

  /* Return the number of modes.  */
  inline unsigned int get_modes_count (void) const;

  /* Return the base count for that extension dll. */
  inline machine_mode_t get_modes_base_count (void) const;

  /* Return the base mtype count for that extension dll. */
  inline TYPE_ID get_mtypes_base_count (void) const;

  /* ============================
     ==  Builtins information  ==
     ============================ */
  /* Return a pointer on the builtins array.  */
  inline const extension_builtins_t* get_builtins (void) const;

  /* Return the number of builtins.  */
  inline unsigned int get_builtins_count (void) const;

  /* Return the base count for that extension dll. */
  inline machine_mode_t get_builtins_base_count (void) const;

  /* Return the base intrinsic count for that extension dll. */
  inline INTRINSIC get_intrinsics_base_count (void) const;

 private:
  BOOL                   own_hooks;
  const extension_hooks *hooks;
  const extension_machine_types_t * overriden_machine_types;
};


// #############################################################################
// ##
// ## Inlined functions for "EXTENSION_HighLevel_Info"
// ##
// #############################################################################
/* Interface identification */
inline mUINT32
EXTENSION_HighLevel_Info::get_initial_revision(void) const {
  return          (hooks->magic);
}

/* Return a pointer on the modes array.  */
inline const extension_machine_types_t * 
EXTENSION_HighLevel_Info::get_modes (void) const {
  return          (overriden_machine_types);
}

/* Return the number of modes.  */
inline unsigned int 
EXTENSION_HighLevel_Info::get_modes_count (void) const {
  return          (hooks->get_modes_count());
}

/* Return the base count for that extension dll. */
inline machine_mode_t 
EXTENSION_HighLevel_Info::get_modes_base_count (void) const {
  return          (hooks->get_modes_base_count());
}

/* Return the base mtype count for that extension dll. */
inline TYPE_ID 
EXTENSION_HighLevel_Info::get_mtypes_base_count (void) const {
  return          (hooks->get_mtypes_base_count());
}

/* Return a pointer on the builtins array.  */
inline const extension_builtins_t* 
EXTENSION_HighLevel_Info::get_builtins (void) const {
  return          (hooks->get_builtins());
}

/* Return the number of builtins.  */
inline unsigned int 
EXTENSION_HighLevel_Info::get_builtins_count (void) const {
  return          (hooks->get_builtins_count());
}

/* Return the base count for that extension dll. */
inline machine_mode_t 
EXTENSION_HighLevel_Info::get_builtins_base_count (void) const {
  return          (hooks->get_builtins_base_count());
}

/* Return the base intrinsic count for that extension dll. */
inline INTRINSIC 
EXTENSION_HighLevel_Info::get_intrinsics_base_count (void) const {
  return          (hooks->get_intrinsics_base_count());
}

#endif /* _DYN_DLL_API_ACCESS_H_ */
