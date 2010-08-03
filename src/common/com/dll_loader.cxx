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

#include "dll_loader.h"

/*------------------------------------------------------------------------------
 * Table and count of loaded extension dlls
 */

static Extension_dll_handler_t * extension_dll_handler_tab = NULL;

static int extension_dll_handler_count = 0;

Extension_dll_handler_t * Get_Extension_dll_handler_tab() { return extension_dll_handler_tab; }

Extension_dll_handler_t * Get_Extension_dll_handler(int index) {
	FmtAssert((extension_dll_handler_tab!=NULL), ("Extension handlers not initialized\n"));
	FmtAssert((index < extension_dll_handler_count), ("Request for extension handler index out of limits\n"));
	return &( extension_dll_handler_tab[index] );
}

int Get_Extension_dll_handler_count() { return extension_dll_handler_count; }

/*------------------------------------------------------------------------------
 * Close dll and free strings from dll handler
 */
static void Cleanup_Extension_dll_handler( Extension_dll_handler_t *ext_dll_handler ) {
  if ( ext_dll_handler ) {
    close_so(ext_dll_handler->handle);
    ext_dll_handler->handle=NULL;
    if(ext_dll_handler->dllname) {
      free(ext_dll_handler->dllname);
      ext_dll_handler->dllname=NULL;
    }
    if(ext_dll_handler->extname) {
      free(ext_dll_handler->extname);
      ext_dll_handler->extname=NULL;
    }
  }
  return;
}

/*------------------------------------------------------------------------------
 * Close dll handlers recorded in <extension_dll_handlers_tab>
 */
void Cleanup_Extension_dll_handlers() {
  if ( extension_dll_handler_tab ) {
    int i;
    for (i=0; i<extension_dll_handler_count; i++) {
      Cleanup_Extension_dll_handler(&extension_dll_handler_tab[i]);
    }
    free(extension_dll_handler_tab);
    extension_dll_handler_tab=NULL;
    extension_dll_handler_count=0;
  }
}

/*------------------------------------------------------------------------------
 * Load the extension dll specified by its basename and access path.
 */
static bool Load_Extension_dll_handler(Extension_dll_handler_t * ext_dll_handler, const char *basename, char *path, BOOL verbose) {

  static const char *const str_suffix = SO_EXT;

  /* Build dll name */
  ext_dll_handler->dllname = TYPE_MEM_POOL_ALLOC_N(char, Malloc_Mem_Pool, strlen(basename) + strlen(str_suffix) + 1 );
  FmtAssert((ext_dll_handler->dllname!=NULL), ("Unable to allocate memory\n"));
  strcpy(ext_dll_handler->dllname, basename);
  strcat(ext_dll_handler->dllname, str_suffix);

  /* Connect dll */
  ext_dll_handler->handle = load_so_no_RTLD_GLOBAL(ext_dll_handler->dllname,path,verbose);

  /* Get extension name */
  get_extension_name_t get_name;
  const char *extname_in_dll;
  get_name = (get_extension_name_t)Get_dll_Symbol(ext_dll_handler, "get_extension_name");
  if (get_name==NULL) { return FALSE; }
  extname_in_dll = (*get_name)();
  ext_dll_handler->extname = TYPE_MEM_POOL_ALLOC_N(char, Malloc_Mem_Pool, strlen(extname_in_dll)+1);
  FmtAssert((ext_dll_handler->dllname!=NULL), ("Unable to allocate memory\n"));
  strcpy(ext_dll_handler->extname, extname_in_dll);

  return TRUE;
}

/*------------------------------------------------------------------------------
 * Load extension dlls from list of dll path given in global <ExtensionNames>
 * into global <extension_dll_handlers_tab>
 */
bool Load_Extension_dll_handlers(BOOL verbose) {
  int i;
  int nb_ext_mtypes;
  int nb_ext_intrinsics;
  int base_mtypes;
  int base_mmodes;
  int base_builtins;
  int base_intrinsics;
  char *ext_fullpath_ptr;
  char *next_ext_fullpath_ptr;

  /* forget previous package if any */
  if (extension_dll_handler_tab!=NULL) {
    DevWarn("Multiple call to Load_Extension_dll_handlers()\n");
    return TRUE;
  }

  // Get count of extensions
  ext_fullpath_ptr = Extension_Names;
  if ( ext_fullpath_ptr == NULL ) { // actualy no extension, set zeros and exit
    extension_dll_handler_tab = NULL;
    extension_dll_handler_count = 0;
    return TRUE;
  }
  extension_dll_handler_count = 1;
  while (*ext_fullpath_ptr != '\0') { if (*ext_fullpath_ptr++ == ',') extension_dll_handler_count+=1; }
  extension_dll_handler_tab = (Extension_dll_handler_t*)calloc(sizeof(Extension_dll_handler_t),extension_dll_handler_count);
  FmtAssert((extension_dll_handler_tab!=NULL), ("Unable to allocate memory for %d * Extension_dll_handler_t\n",extension_dll_handler_count));

  // Load extension dll handlers from global variable 'Extension_Names'
  ext_fullpath_ptr = Extension_Names;
  extension_dll_handler_count = 0;
  while (NULL!=ext_fullpath_ptr) {

    // Cut path in path list
    next_ext_fullpath_ptr = strchr(ext_fullpath_ptr,',');
    if (next_ext_fullpath_ptr) *next_ext_fullpath_ptr++=0;

    // Set handler in table
    if (!Load_Extension_dll_handler(&extension_dll_handler_tab[extension_dll_handler_count], ext_fullpath_ptr,"",verbose)) {
      return FALSE;
    }

    // Check that extension has not yet been loaded
    for (i=0; i<(extension_dll_handler_count); i++) {
      if (!strcmp(extension_dll_handler_tab[extension_dll_handler_count].extname, extension_dll_handler_tab[i].extname)) {
        break;
      }
    }
    if (i < extension_dll_handler_count) { // Extension already loaded, forget it
      DevWarn("Extension '%s' specified several times in command line (dll '%s' and '%s')! First one loaded only.\n",
        extension_dll_handler_tab[extension_dll_handler_count].extname,
        extension_dll_handler_tab[i].dllname,
        extension_dll_handler_tab[extension_dll_handler_count].dllname
      );
      Cleanup_Extension_dll_handler(&extension_dll_handler_tab[extension_dll_handler_count]);
    }
    else { // New extension handler loader, go next
      extension_dll_handler_count ++;
    }

    ext_fullpath_ptr = next_ext_fullpath_ptr;
  }
  return TRUE;
}

/*------------------------------------------------------------------------------
 * Retrieve a symbol from argument dll and return a pointer to it
 * (NULL if undefined)
 */
void * Get_dll_Symbol(const Extension_dll_handler_t *ext_dll_handler, const char *symbol) {

  void *symb_ptr;

  FmtAssert((symbol != NULL), ("Unexpected NULL string for symbol name in Get_dll_Symbol()"));

  symb_ptr = dlsym(ext_dll_handler->handle, symbol);
  if(symb_ptr == NULL) {
    char err_msg[256];
    snprintf(err_msg, sizeof(err_msg)-1, "Unable to find symbol %s (%s)", symbol, dlerror());
    RaiseErrorIncompatibleLibrary(ext_dll_handler->dllname, err_msg);
  }

  return (symb_ptr);
}

/*------------------------------------------------------------------------------
 * Function called when an extension is incompatible with the core description
 */
void RaiseErrorIncompatibleLibrary(const char *name, const char *error_msg) {
  char err_msg[1024];
  snprintf(err_msg, sizeof(err_msg),
    "Failure.\n"
    "### %s\n"
    "### Please provide a compatible library.\n",
    error_msg
  );
  ErrMsg(EC_Lib_Ext_Load, name, err_msg);
}

/*------------------------------------------------------------------------------
 * Return TRUE if extension given by name is present in table of loaded dlls
 */
bool EXTENSION_Is_Extension_Present(const char* name) {
  int i;
  for (i=0; i<extension_dll_handler_count; i++) {
    if (!(strcmp(name, extension_dll_handler_tab[i].extname))) {
      return TRUE;
    }
  }
  return FALSE;
}

