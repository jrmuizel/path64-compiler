/*-------------------------------------------------------------------------*/
/*
 *      Copyright 2004, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 *      STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 *
 * File : extension.c
 *
 * Description : Connect/Disconnect an assembler extension.
............................................................................
*/
/* Used to display DLL path trials #define DISPLAY_DLL_PATH */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "air.h"
#include "parser.h"
#include "Dll.h"

#if defined(__linux) || defined(__sun)
#include <sys/param.h>
#include <unistd.h>
#define GUESSPATHLEN MAXPATHLEN
#endif

#ifdef WIN32
#define GUESSPATHLEN _MAX_PATH
#endif

#ifndef GUESSPATHLEN
#error "Undefined macro 'GUESSPATHLEN'"
#endif

#ifndef FILENAME_MAX
#error "Undefined macro 'FILENAME_MAX'"
#endif

#if defined(WIN32) || defined(__MINGW32__)
static char *
win32_get_proc_path( char *proc_name ) {
  char *pdir = (char*)malloc(GUESSPATHLEN+1);
  char *ignored ;  
  if (!SearchPath(NULL, proc_name, ".exe", GUESSPATHLEN, pdir, &ignored)) {
    free(pdir);
    pdir = 0;
  }
  return pdir;
}
#endif

#if defined(__linux)
#include <libgen.h>

static char *
linux_get_proc_path( char *proc_name ) {
  int pdir_size = GUESSPATHLEN+1+strlen(proc_name);
  char *pdir = (char*)malloc(pdir_size);
  int read_char = 0;

  if (read_char = readlink("/proc/self/exe", pdir, pdir_size)) {
    pdir[read_char] = '\0';
    pdir = dirname(pdir);
  }
  else {
    free(pdir);
    pdir = 0;
  }
  return pdir;
}
#endif

/*-------------------------------------------------------------------------*/
/*
   Routine get_proc_path

   Description : return the absolute path of the current process

   Input :     current process binary file name.

   Output :    allocated buffer (with malloc) containing the full path.
............................................................................
*/
char *
PARSER_GetProcPath( char *proc_name)
{
#if defined(WIN32) || defined(__MINGW32__)
  return win32_get_proc_path( proc_name );
#endif

#ifdef __linux
  return linux_get_proc_path( proc_name );
#endif
}

/*-------------------------------------------------------------------------*/
/*
   Routine DisconnectExtension

   Description : unload all extension DLLs.

   Input :     none.

   Output :    none.
............................................................................
*/
void PARSER_DisconnectParsers ( void ) {
   int ParserId;

   for (ParserId=0;ParserId<MAX_PARSER_NR;ParserId++) {
      if (NULL!=ParserList[ParserId]) {
         if (NULL!=ParserList[ParserId]->DllHandle) {
            DLL_CLOSE(ParserList[ParserId]->DllHandle);
            ParserList[ParserId]->DllHandle = NULL;
         }
         if (NULL!=ParserList[ParserId]->Name) {
            free(ParserList[ParserId]->Name);
            ParserList[ParserId]->Name = NULL;
         }
         free(ParserList[ParserId]);
         ParserList[ParserId]=NULL;
      }
   }
}

