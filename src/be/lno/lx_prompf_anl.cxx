/*
 * Copyright (C) 2007, 2008. PathScale, LLC. All Rights Reserved.
 */
/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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

   Special thanks goes to SGI for their continued support to open source

*/

#include "elf_defines.h"
#include "wn.h"                     /* for WN */
#include "defs.h"                   /* for pu_info.h */
#include "config.h"                 /* for LNO_Path, etc. */
#include "pu_info.h"                /* for PU_Info */
#include "anl_driver.h"             /* for Anl_Process_Command_Line, etc. */

// From prompl_anl directory, appropriate stubs:

extern void Prompf_Emit_Whirl_to_Source(PU_Info* current_pu,
                                        WN* func_nd)
{
}  

#if 1 /* ! defined(BUILD_OS_DARWIN) */
/* Changed this to be inline; see comment in anl_driver.h about weak syms */
extern INT64 New_Construct_Id()
{
  return 0;
}
#endif /* ! defined(BUILD_OS_DARWIN) */

  
