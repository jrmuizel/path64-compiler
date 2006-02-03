/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


#include "W_alloca.h"
#include <stdio.h>		    /* for fprintf() */
#include <string.h>
#include "W_dlfcn.h"		    /* for sgidladd(), dlerror() */
#include <cmplrs/rcodes.h>
#include "SYS.h"		    /* for sgidladd(), dlerror() */

#include "defs.h"
#include "dso.h"

/*
 * WARNING:  dso is used outside of main mongoose compiler (via targ_info)
 * and should not depend on other mongoose-specific files 
 * (e.g. this is why verbose is a parameter rather than a global variable).
 */

/*
 * Assume that LD_LIBRARY_PATH has already been set up correctly.
 */
BE_EXPORTED void* 
load_so (char *soname, char *path, BOOL verbose)
{
#ifdef STATIC_BACKEND
  return NULL;
#else
  register char *full_path;
  void* handler = NULL;
  
  if (path != 0) {
    full_path = SYS_makePath(path, soname);
    
    if (verbose) {
      fprintf (stderr, "\nReplacing default %s with %s (path:%s)\n", soname, full_path, path);
    }
  } else {
    full_path = soname;
  }
  
#ifdef sgi
  if (sgidladd (full_path, RTLD_LAZY) == NULL)
#else
    if (! (handler = dlopen (full_path, RTLD_NOW | RTLD_GLOBAL)) )
#endif
    {
      fprintf (stderr, "error while loading shared library: %s: %s\n", full_path, dlerror());
      exit (RC_SYSTEM_ERROR);
    }

#ifdef sgi
    return NULL;
#else
    return handler;
#endif

#endif // STATIC_BACKEND

} /* load_so */

BE_EXPORTED void close_so (void * handler)
{
#ifndef STATIC_BACKEND
  if (handler) 
    dlclose(handler);
#endif
}
