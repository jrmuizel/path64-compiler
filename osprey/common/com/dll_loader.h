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
 * API used by both Front-end, Back-end and AIR to load extension 
 * dlls.
 */

#ifndef DLL_LOADER_H
#define DLL_LOADER_H

#include "defs_exported.h"
#include "config.h"
#include "tracing.h"
#include "mempool.h"
#include "erglob.h"
#include "errors.h"
#include "dso.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "extension_include.h"
#include "dyn_dll_api.h"

#if defined(__MINGW32__) || defined(__CYGWIN__)
#define SO_EXT ".dll"
#else
#define SO_EXT ".so"
#endif

extern "C" {
#include "W_dlfcn.h" /* for sgidladd(), dlerror() */
};

/*------------------------------------------------------------------------------
 * Description of an extension dll
 */
struct Extension_dll_handler {
  char *extname;
  char *dllname;
  void *handle;
};
typedef struct Extension_dll_handler Extension_dll_handler_t;

/*------------------------------------------------------------------------------
 * Table and count of loaded extension dlls
 */
extern Extension_dll_handler_t * Get_Extension_dll_handler_tab( void );
extern Extension_dll_handler_t * Get_Extension_dll_handler( int index );
extern int Get_Extension_dll_handler_count( void );

/*------------------------------------------------------------------------------
 * Load extension dlls from list of dll path given in global <ExtensionNames>
 * into global <extension_dll_handlers_tab>
 */
extern bool Load_Extension_dll_handlers(BOOL verbose);

/*------------------------------------------------------------------------------
 * Close dll handlers recorded in <extension_dll_handlers_tab>
 */
extern void Cleanup_Extension_dll_handlers();

/*------------------------------------------------------------------------------
 * Retrieve a symbol from argument dll and return a pointer to it
 * (NULL if undefined)
 */
extern void * Get_dll_Symbol(const Extension_dll_handler_t *ext_dll_handler, const char *symbol);

/*------------------------------------------------------------------------------
 * Function called when an extension is incompatible with the core description
 */
extern void RaiseErrorIncompatibleLibrary(const char *name, const char *error_msg);

/*------------------------------------------------------------------------------
 * Return TRUE if extension given by name is present in table of loaded dlls
 */
extern bool EXTENSION_Is_Extension_Present(const char* name);

#endif /* DLL_LOADER_H */
