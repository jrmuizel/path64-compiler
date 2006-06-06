/*
  THIS FILE HAS BEEN MODIFIED OR ADDED BY STMicroelectronics, Inc. 1999-2006
*/
/* Basic Cygwin pathname support for MinGW.

   Copyright (C) 2006 STMicroelectronics

   This file is part of the libiberty library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.


   This file implements a limited amount of support for Cygwin paths.
   It is intended for use by MinGW programs that must interact with Cygwin.

   It is limited to absolute paths only.  I.e. Those beginning with Cygwin
   mounts, such as /cygdrive/...  See the comment on cygpath() below.  */

#include "libiberty.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>


/* These are all the possible settings for the ST_CYGPATH_MODE
   environment variable.  */
static enum
{
  mode_unset,
  mode_off,
  mode_normal,
  mode_full
} mode = mode_unset;


/* These are the values extracted from the registry.
   They are extracted the first time cygpath is called.  */
static const char *cygdrive = NULL;
static struct mount
{
  /* The name of the Cygwin mount point.  E.g. "/usr/bin"  */
  char *mount;

  /* The actual Windows path that the mount translates to.  */
  char *actual;

  struct mount *next;
} *mounts = NULL;


/* Read a string from the Windows Registry.
   KEY should be a valid handle from RegOpenKeyEx().
   NAME should be the name of the value within the key.
   The value should be of type REG_SZ.
   If the value does not exist, is of the wrong typei, or another error
   occurs, then NULL is returned.
   Otherwise a malloced string is returned.  */
static char *
read_string_from_registry (HKEY key, const char *name)
{
  DWORD valuetype = REG_NONE;
  DWORD valuesize = 0;
  char *value = NULL;

  if (RegQueryValueEx (key, name, NULL, &valuetype,
		       NULL, &valuesize) == ERROR_SUCCESS
      && valuetype == REG_SZ)
    {
      value = xmalloc (valuesize);
      if (RegQueryValueEx (key, name, NULL, &valuetype, (unsigned char *)value,
			   &valuesize) != ERROR_SUCCESS)
	{
	  free (value);
	  value = NULL;
	}
    }

  return value;
}


/* Fill in the mounts list (mounts is defined statically above).
   All subkeys (not values) of KEY that contain a REG_SZ value named 'native'
   are added to the start of the mounts list.  */
static void
read_mounts (HKEY key)
{
  int mountsize = 15;
  char *mount = xmalloc (mountsize);
  DWORD size = mountsize;
  int index = 0;
  int retval = 0;

  /* For each subkey ...  */
  while ((retval = RegEnumKeyEx (key, index, mount, &size, 0, NULL, 0, NULL))
	 != ERROR_NO_MORE_ITEMS)
    {
      struct mount *newmount;
      HKEY subkey;
      char *actual;

      switch (retval) {
      case ERROR_MORE_DATA:
	/* The buffer wasn't large enough for this key name.
	   Unlike RegQueryValueEx, RegEnumKeyEx won't tell us how big it
	   should be, so just make it bigger and try again.
	   Note that this code path does NOT increment index.
       	   Most of the time we will only be dealing with short strings.  */
	mountsize += 10;
	mount = xrealloc (mount, mountsize);
	break;

      case ERROR_SUCCESS:
	/* Find the actual windows path.  */
  	if (RegOpenKeyEx (key, mount, 0, KEY_READ, &subkey) != ERROR_SUCCESS)
	  {
	    index++;
	    break;
	  }
	actual = read_string_from_registry (subkey, "native");	
	RegCloseKey (subkey);
	if (actual == NULL)
	  {
	    index++;
	    break;
	  }

	/* Create the new entry in the mount table.  */
	newmount = xmalloc (sizeof (struct mount));
	newmount->mount = xstrdup (mount);
	newmount->actual = actual;
	newmount->next = mounts;
	mounts = newmount;
	index++;
	break;

      default:
	/* Don't infinite loop should any other return value occur.  */
        index++;
      }

      /* The last call to RegEnumKeyEx may have clobbered size.
         Fix it before the next call.  */
      size = mountsize;
    }

  free (mount);
}


/* The top level registry reading function.
   Open the keys, call the above functions to get the right values,
   and clean up.  */
static void
read_registry (void)
{
  HKEY hcu_key, hlm_key;

  /* Get key handles for the two places cygwin keeps its registry data.  */
  if (RegOpenKeyEx (HKEY_CURRENT_USER,
		    "Software\\Cygnus Solutions\\Cygwin\\mounts v2",
		    0, KEY_READ, &hcu_key) != ERROR_SUCCESS)
    hcu_key = NULL;

  if (RegOpenKeyEx (HKEY_LOCAL_MACHINE,
		    "SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2",
		    0, KEY_READ, &hlm_key) != ERROR_SUCCESS)
    hlm_key = NULL;

  /* Get the virtual mount point used for windows drives.  */
  if (hcu_key)
    cygdrive = read_string_from_registry (hcu_key, "cygdrive prefix");
  if (hlm_key && cygdrive == NULL)
    cygdrive = read_string_from_registry (hlm_key, "cygdrive prefix");

  /* Read the other mount points.
     Read hlm before hcu to ensure hcu settings get used by preference
     by being closer on the mounts stack.  */
  if (hlm_key)
    read_mounts (hlm_key);
  if (hcu_key)
    read_mounts (hcu_key);

  if (hlm_key)
    RegCloseKey (hlm_key);
  if (hcu_key)
    RegCloseKey (hcu_key);
}


/* Given a path of unknown variety, return the same path with any
   Cygwin mount points substituted.
   This function always returns a malloced string which should be
   freed when the the caller is finished with it.

   The mapping is affected by the ST_CYGPATH_MODE environment variable.
   See the fprintf messages below for full information.

   It can replace /cygdrive/<letter>/..... style pathnames, even if the
   user has used 'mount -c' to an alternative string.

   It can replace (if enabled) other Cygwin mount points, such as
   the usual '/', '/usr/bin', '/usr/lib', as well as any other user defined
   mount points.

   It does NOT attempt to convert any pathnames that look like native Windows
   names - such as those starting with '<letter>:' or double slash (UNC).

   It does NOT handle relative pathnames passing through cygwin mounts
   (e.g. '../cygdrive/c'), or absolute paths with repeated directory
   separators or relative elements within the mount name
   (e.g. '/usr/./bin').
   
   It does NOT allow backslash \ directory separators within the actual mount
   path (e.g. '/usr\bin').  Cygwin does not always allow them there either.  */
char *
cygpath (const char *path)
{
  char *result = NULL;

  if (path == NULL)
    return NULL;

  /* If this is the first time this function has been called then read the
     environment and registry.  */
  if (mode == mode_unset)
    {
      char *env = getenv ("ST_CYGPATH_MODE");

      if (env == NULL || strcmp (env, "normal") == 0)
    	mode = mode_normal;
      else if (strcmp (env, "full") == 0)
	mode = mode_full;
      else if (strcmp (env, "off") == 0)
	mode = mode_off;

      if (mode != mode_off)
	read_registry();

      if (mode == mode_unset)
	{
	  /* The variable was set, but not to any known value.
	     Set up a default and print an informational message
	     for the user.  */
	  mode = mode_normal;
	  fprintf (stderr, "ST_CYGPATH_MODE should be one of:\n");
	  fprintf (stderr, " off    - Disable all path translation.\n");
	  fprintf (stderr, " normal - Translate %s only.\n", cygdrive);
	  fprintf (stderr, " full   - Translate all Cygwin mounts.\n");
	}
    }

  /* First, test if this can only be a windows (non-cygwin) path.
     This includes paths that start with a drive letter or UNC double slash.  */
  if ((isalpha (path[0]) && path[1] == ':')
      || ((path[0] == '\\' || path[0] == '/')
	  && (path[1] == '\\' || path[1] == '/')))
    result = xstrdup (path);

  /* Second, handle /cygdrive/<letter>/ (or whatever) paths.  */
  if (!result && cygdrive != NULL && (mode == mode_normal || mode == mode_full))
    {
      int length = strlen (cygdrive);
      /* Note that cygwin does not allow '\\' instead of '/' in cygdrive.  */
      if (strncmp (cygdrive, path, length) == 0
	  && (path[length] == '/' || path[length] == '\\'
	      || path[length] == '\0')
	  && isalpha (path[length+1]))
        {
	  result = xmalloc (strlen (path) - length+1 + 1);
	  result[0] = path[length+1];
	  result[1] = ':';
	  strcpy (result + 2, path + length + 2);
	}
    }

  /* Third, handle other types of cygwin path.  */
  if (!result && mounts != NULL && mode == mode_full)
    {
      int matched = 0;
      struct mount *foundat = NULL;
      struct mount *mount = mounts;
      /* Find the longest matching mount point.
	 This is important. If we just used the first matching mount point
	 it would probably always match '/' when '/usr/bin' is right.
	 Use the first of equal length matches - this allows current-user
	 mounts to override 'local machine' mounts (can this happen?).
         It is a match only if the matching part is followed by a directory
         separator or the end of the path, except for the root mount point.  */
      while (mount != NULL)
	{
	  int length = strlen (mount->mount);
	  if (strncmp (mount->mount, path, length) == 0
	      && matched < length
	      && (length == 1 /* Special case for root mount point '/'.  */
		  || path[length] == '/' || path[length] == '\\'
		  || path[length] == '\0'))
	    {
	      matched = length;
	      foundat = mount;
	    }
	  mount = mount->next;
	}
      if (matched)
	{
	  /* There was a match so do the substitution.
	     If matched is 1 then it can only be the root mount point, in
	     which case we do not want to remove the matched part as it is the 
	     directory separator.  */
	  if (matched == 1)
	    matched = 0;
	  result = xmalloc (strlen (foundat->actual) + strlen (path) + 1
			    - matched);
	  strcpy (result, foundat->actual);
	  strcat (result, path + matched);
	}
    }

  if (result)
    {
      /* Ensure that the return is never just a drive letter.
	 This is not a valid directory on Windows, but code often
	 trims trailing slashes.  */
      int length = strlen(result);
      if (result[length-1] == ':')
	{
	  result = xrealloc (result, length+2);
	  result[length] = '/';
	  result[length+1] = '\0';
	}
      return result;
    }

  /* If we get here then it must have been some other kind of path.  */
  return xstrdup (path);
}


/* This is just to make inserting the conversion more convenient.
   The CYGPATH_REPLACE is conditionally compiled so it is harder to
   add clean up code to go with it without this.  */
void
cygpath_replace (char **path)
{
  char *result = cygpath (*path);
  free (*path);
  *path = result;
}
