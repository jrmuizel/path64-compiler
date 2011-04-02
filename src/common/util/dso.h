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


/* ====================================================================
 * ====================================================================
 *
 * Module: dso.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  16-Feb-95 - Original Version
 *
 * Description:
 *  handles all DSO-related operations
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef dso_INCLUDED
#define dso_INCLUDED

#include "file_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Load dso of given name in given path.
 * If verbose and non-standard path, then print path on stderr.
 */
extern void *dso_load(const char *dso, const char *path, BOOL verbose);
extern void *dso_get_interface(void *handle, const char *symbol);
extern void  dso_unload(void *handle);

#define dso_load_simply(dso, path, verbose) \
	    dso_load(FN_DSO(dso), path, verbose)

#ifdef __cplusplus
}
#endif
#endif /* dso_INCLUDED */
