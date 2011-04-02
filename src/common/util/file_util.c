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



static char *source_file = __FILE__;
static char *rcs_id = "$Source$ $Revision$";

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/param.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmplrs/rcodes.h>

#include "defs.h"
#include "file_util.h"

/* ====================================================================
 *
 * Is_File
 *
 * Determine whether a pathname represents an existing regular file.
 *
 * ====================================================================
 */

BOOL
Is_File ( const char *fname )
{
    struct stat desc;		    /* File status descriptor */

    if (fname == NULL)
	return FALSE;
  
    if ( stat ( fname, &desc ) != 0 )
	return FALSE;		    /* existing? */
    return ( (desc.st_mode & S_IFREG) != 0 ); /* regular file? */
}


/* ====================================================================
 *
 * Same_File
 *
 * Determine whether two streams are associated with the same file.
 * Returns FALSE (not the same) if there are any problems obtaining
 * status.
 *
 * ====================================================================
 */

BOOL
Same_File ( file1, file2 )
  FILE *file1, *file2;
{
  struct stat d1, d2;	/* Stream status descriptors */

  if ( file1 == NULL || file2 == NULL ) return FALSE;
  if ( fstat ( fileno(file1), &d1 ) == -1 ) return FALSE;
  if ( fstat ( fileno(file2), &d2 ) == -1 ) return FALSE;
  return ( d1.st_ino == d2.st_ino ) && ( d1.st_dev == d2.st_dev );
}



/* ====================================================================
 *
 * New_Extension
 *
 * Replace the given file name's extension with another extension and
 * return a new filename string.  The given extension should include
 * the period if desired (a period in the original name will be
 * eliminated).
 *
 * ====================================================================
 */

char *
New_Extension ( const char *name, const char *ext )
{
  char *new;
  INT16 len, i;

  /* Allocate a new name string: */
  len = strlen(name);
  new = (char *) malloc ( len + strlen(ext) + 1 );
  strcpy ( new, name );
  for ( i=len-1; i>=0; i-- ) {
    if ( IS_DIR_SLASH(new[i]) ) break;	/* Don't touch directory prefixes */
    if ( new[i] == '.' ) {
      new[i] = 0;
      break;
    }
  }
  strcat ( new, ext );
  return new;
}

/* ====================================================================
 *
 * Remove_Extension
 *
 * Remove the extension from a given file name. The period in the
 * original file name is also removed. Assumes that the directory
 * prefixes have already been removed.
 *
 * ====================================================================
 */

char *
Remove_Extension ( name )
  char *name; /* The file name with extension */
{
  char *new;
  INT16 len, i;
  /* Allocate a new name string: */
  len = strlen(name) + 1;
  new = (char *) malloc ( len );
  strcpy ( new, name );
  for ( i=len-1; i>=0; i-- ) {
    if ( new[i] == '.' ) {
      new[i] = 0;
      break;
    }
  }
  return new;
}

/* current working directory */
static char *cwd = NULL;
static INT cwd_size;

char *
Get_Current_Working_Directory (void)
{
  char *cwd;
  cwd = getcwd((char *) NULL, MAXPATHLEN);
  if (cwd == NULL) {
    cwd = getenv("PWD");
	if (cwd == NULL) {
		/* can't get path */
		cwd = ".";
        }
  }
  return cwd;
}


/* ====================================================================
 *
 * Last_Pathname_Component
 *
 * Return the last component of the pathname specified in 'pname'.
 * I.e., if the input is "/da/db/dc/f.x", return a pointer to "f.x".
 * If there are no slashes in the input, just return the input.
 *
 * Note that we return a pointer to a portion of the input string.
 * Therefore, if our caller wants to modify the returned value, the
 * caller must first make a copy.
 *
 * ====================================================================
 */

char *
Last_Pathname_Component ( char *pname )
{
  char *cp = pname + strlen(pname);

  while (cp != pname) {
    if (IS_DIR_SLASH(*cp)) return cp+1;
    --cp;
  }
  if (IS_DIR_SLASH(*cp)) return cp+1;
  return cp;

} /*end: Last_Pathname_Component */


/* Eliminates "//", "/.", and "/.." from the path, to the extent that it
 * is possible.
 * "//"      -> "/"
 * "/./"     -> "/"
 * "a/b/../" -> "a/"
 */
static char *
normalize_path(char * path)
{
  char * inp = path, *outp = path, *tmp;

  while (inp != NULL && *inp != '\0') {
    if (IS_DIR_SLASH(inp[0])) {
      if (IS_DIR_SLASH(inp[1]))
	inp+= 1;
      else if (inp[1] == '.' && IS_DIR_SLASH(inp[2]))
	inp += 2;
      else if (inp[1] == '.' && inp[2] == '.' && IS_DIR_SLASH(inp[3])) {
	/* Skip up one level up output path */
	for (tmp = outp-1;
	     tmp >= path && !IS_DIR_SLASH(*tmp);
	     tmp -= 1);
        /* Check that we skipped one level up, but not past ".." */
        if (tmp >= path && IS_DIR_SLASH(tmp[0]) && 
	  (tmp[1] != '.' || tmp[2] != '.'))
	  outp = tmp;
	else {
	  *outp++ = DIR_SLASH;
	  *outp++ = '.';
	  *outp++ = '.';
	}
	inp+= 3;
      } /* if */
      else
	*outp++ = *inp++;
    } /* if */
    else
      *outp++ = *inp++;
  } /* while */
  
  *outp = '\0';
  return path;
}

/* Make an absolute path name from the file name,
 * which means no .. or . or // in the path. */
extern char *
Make_Absolute_Path (char *filename)
{
   char  *normalized;
   INT64 cwd_length;

   if ( cwd == NULL ) {
	cwd = Get_Current_Working_Directory();
   	cwd_size = strlen (cwd);
   }
   normalized = (char *)malloc(cwd_size+strlen(filename)+2);
   if (normalized == NULL) {
      perror("malloc");
      exit(RC_SYSTEM_ERROR);
   }
   if (IS_ABSOLUTE_PATH(filename))
      (void)strcpy(normalized, filename);
   else
   {
      cwd_length = cwd_size;
      strcpy(normalized, cwd);
      if (!IS_DIR_SLASH(cwd[cwd_length - 1]))
      {
	 normalized[cwd_length++] = DIR_SLASH;
	 normalized[cwd_length] = '\0';
      }
      (void)strcpy(&normalized[cwd_length], filename);
   }
   (void)normalize_path(normalized);
   return normalized;
}

