/* 

  Copyright (C) 2008 ST Microelectronics, Inc.  All Rights Reserved. 

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
#ifndef ISA_LOADER_API_H
#define ISA_LOADER_API_H

#ifdef BACK_END

#include "../gccfe/extension_include.h"
#include "dyn_isa_api_access.h"
#include "ext_info.h"

/* Return a pointer to the description of the extension register class <rc> */
BE_EXPORTED extern const EXTENSION_Regclass_Info *EXTENSION_get_REGISTER_CLASS_info(ISA_REGISTER_CLASS rc);

/*
 * Return TRUE if the parameter TOP Id belongs to an extension
 */
inline BOOL EXTENSION_Is_Extension_TOP(TOP id) {
  return (id >= TOP_static_count && id < TOP_count);
}

/*
 * Return TRUE if the parameter register class rc belongs to an extension
 */
inline BOOL EXTENSION_Is_Extension_REGISTER_CLASS(ISA_REGISTER_CLASS rc) {
  return (rc > ISA_REGISTER_CLASS_STATIC_MAX && rc <= ISA_REGISTER_CLASS_MAX);
}

/**
 * Get the bit size of a register of the given sub class.
 *
 * @param  subclass 
 *
 * @pre    subclass is a valid subclass index
 * @post   true
 *
 * @return The number of bit of a register of given subclass
 */
BE_EXPORTED extern INT EXTENSION_get_REGISTER_SUBCLASS_bit_size(ISA_REGISTER_SUBCLASS subclass);

/**
 * Get the maximum bit size for given register class.
 * The maximum bit size corresponds to maximum bit size of the register
 * class and all its subclasses. For core registers, sub classes are not
 * checked
 *
 * @param  rc A register class
 *
 * @pre    rc is a valid register class index
 * @post   true
 *
 * @return The maximum bit size for given register class
 */
BE_EXPORTED extern INT EXTENSION_get_REGISTER_CLASS_max_bit_size(ISA_REGISTER_CLASS rclass);

#endif  /* defined (BACK_END) */

/* Map between preg and register in the extension */
BE_EXPORTED extern ISA_REGISTER_CLASS EXTENSION_PREG_to_REGISTER_CLASS(PREG_NUM preg);

BE_EXPORTED extern EXTENSION_ISA_Info *EXTENSION_Get_ISA_Info_From_TOP(TOP id);

#endif /* ISA_LOADER_API_H */

