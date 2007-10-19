
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

#if 0
/*
 * This stub is not needed anymore as the extension build will directly use
 * dyn_get_TOP_prop_tab( ). Thus this file is obsolete.
 * Thoug, we keep this file anyway in order to keep bacward compatibility
 * with the reconfigurable toolkit installation that expects this file to
 * be present in the open64 sources.
*/

/**
 *
 * This file is only useful for dynamic code extension.
 *
 * More precisely when building the shared object (dll),
 * the Open64 code generator needs some functions that have
 * been processed in earlier steps of its own process.
 *
 * The role devoted to file dyn_stubxxx.c is to emulate these
 * functions when dynamic code generation is activated.
 * 
 */
#ifndef DYNAMIC_CODE_GEN
#error "File " __FILE__ " can only be used for dynamic code generation\n"
#endif

#include "dyn_isa_properties.h"

const mUINT64* ISA_PROPERTIES_flags = dyn_get_TOP_prop_tab( );

#endif /* 0 */
