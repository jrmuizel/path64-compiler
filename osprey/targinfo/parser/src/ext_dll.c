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
#if 0
#include "utils_version.h"
#endif

#ifdef __linux
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

/*-------------------------------------------------------------------------*/
/*
   Routine ConnectExtension

   Description : Open and connect an extension DLL

   Input :     

   Output :
............................................................................
*/

#define LIBRARY_PATH_SIZE GUESSPATHLEN

void PARSER_ConnectDynamicParser ( char * Name ) {
  char *ErrMsg;
  PARSER_GetParserT  GetParser;
  PARSER_ParserT    *ParserLib;
  int dll_try = 0;
  char *lib_path_to_try;
  char lib_path_buf[LIBRARY_PATH_SIZE];
  int PARSER_usual_path_has_no_sep = 0;
  const char *dir_separator = DLL_DIR_SEPARATOR;

  if(PARSER_usual_path[strlen(PARSER_usual_path)-1] != *dir_separator) {
    PARSER_usual_path_has_no_sep = 1;
  }

  if (Name == NULL) {
    fprintf(stderr,
	    "Extension plug-in F-1001: No name specified for extension\n");
    return;
  } else {
    ParserLib = (PARSER_ParserT *) malloc (sizeof(PARSER_ParserT));
    ParserLib->Name = (char *)malloc(strlen(Name)+1);
    strcpy(ParserLib->Name,Name);
  }
  /* TODO: Check if already open */      
  
  /* Try to open library */      
  
  /* ./name */
  lib_path_buf[0] = '\0';
  strncat(lib_path_buf,"." DLL_DIR_SEPARATOR,LIBRARY_PATH_SIZE);
  strncat(lib_path_buf,ParserLib->Name,LIBRARY_PATH_SIZE);
  lib_path_to_try = lib_path_buf;
  
#ifdef DISPLAY_DLL_PATH
  printf("Trying [%s]\n",lib_path_to_try);
#endif
  
  while(dll_try < 8 && DLL_OPEN(ParserLib->DllHandle,lib_path_to_try) == NULL) {
    switch(dll_try) {
    case 0:
      /* ./name.<platformext> */
      lib_path_buf[0] = '\0';
      strncat(lib_path_buf,"." DLL_DIR_SEPARATOR,LIBRARY_PATH_SIZE);
      strncat(lib_path_buf,ParserLib->Name,LIBRARY_PATH_SIZE);
      strncat(lib_path_buf,PLATFORM_EXT,LIBRARY_PATH_SIZE);
      lib_path_to_try = lib_path_buf;
      break;
    case 1:
      /* ./name<asm>.<platformext> */
      lib_path_buf[0] = '\0';
      strncat(lib_path_buf,"." DLL_DIR_SEPARATOR,LIBRARY_PATH_SIZE);
      strncat(lib_path_buf,ParserLib->Name,LIBRARY_PATH_SIZE);
      strncat(lib_path_buf,"asm" PLATFORM_EXT,LIBRARY_PATH_SIZE);
      lib_path_to_try = lib_path_buf;
      break;
    case 2:
      /* PARSER_usual_path/name */
      lib_path_buf[0] = '\0';
      strncat(lib_path_buf,PARSER_usual_path,LIBRARY_PATH_SIZE);
      if(PARSER_usual_path_has_no_sep) {
	strncat(lib_path_buf,DLL_DIR_SEPARATOR,LIBRARY_PATH_SIZE);
      }
      strncat(lib_path_buf,ParserLib->Name,LIBRARY_PATH_SIZE);
      lib_path_to_try = lib_path_buf;
      break;
    case 3:
      /* PARSER_usual_path/name.<platformext> */
      lib_path_buf[0] = '\0';
      strncat(lib_path_buf,PARSER_usual_path,LIBRARY_PATH_SIZE);
      if(PARSER_usual_path_has_no_sep) {
	strncat(lib_path_buf,DLL_DIR_SEPARATOR,LIBRARY_PATH_SIZE);
      }
      strncat(lib_path_buf,ParserLib->Name,LIBRARY_PATH_SIZE);
      strncat(lib_path_buf,PLATFORM_EXT,LIBRARY_PATH_SIZE);
      lib_path_to_try = lib_path_buf;
      break;
    case 4:
      /* PARSER_usual_path/name<asm>.<platformext> */
      lib_path_buf[0] = '\0';
      strncat(lib_path_buf,PARSER_usual_path,LIBRARY_PATH_SIZE);
      if(PARSER_usual_path_has_no_sep) {
	strncat(lib_path_buf,DLL_DIR_SEPARATOR,LIBRARY_PATH_SIZE);
      }
      strncat(lib_path_buf,ParserLib->Name,LIBRARY_PATH_SIZE);
      strncat(lib_path_buf,"asm" PLATFORM_EXT,LIBRARY_PATH_SIZE);
      lib_path_to_try = lib_path_buf;
      break;
    case 5:
      /* name */
      lib_path_to_try = ParserLib->Name;
      break;
    case 6:
      /* name.<platformext> */
      lib_path_buf[0] = '\0';
      strncat(lib_path_buf,ParserLib->Name,LIBRARY_PATH_SIZE);
      strncat(lib_path_buf,PLATFORM_EXT,LIBRARY_PATH_SIZE);
      lib_path_to_try = lib_path_buf;
      break;
    case 7:
      /* name<asm>.<platformext> */
      lib_path_buf[0] = '\0';
      strncat(lib_path_buf,ParserLib->Name,LIBRARY_PATH_SIZE);
      strncat(lib_path_buf,"asm" PLATFORM_EXT,LIBRARY_PATH_SIZE);
      lib_path_to_try = lib_path_buf;
      break;
    default:
      fprintf(stderr,
	      "Extension plug-in F-1002: Unable to load extension [%s]\n",ParserLib->Name);
      return;
    }

#ifdef DISPLAY_DLL_PATH
    printf("Trying last [%s]\n",lib_path_to_try);
#endif
    dll_try++;
  }
   
  if( ParserLib->DllHandle == NULL ) {
    DLL_GET_ERROR(ErrMsg);
    fprintf(stderr,
	    "Extension plug-in F-1002: %s [%s]\n",
	    ErrMsg,ParserLib->Name);
    free(ParserLib->Name);
    free(ParserLib);
    return;
  }
      
ContinueDLLopening:

   /* Find out PARSER_GetParser symbol */
   if (NULL == DLL_GET_TYPED_SYMBOL(ParserLib->DllHandle,GetParser,
                                    "PARSER_GetParser",
                                    PARSER_GetParserT)) {
      ErrMsg="";
      DLL_GET_ERROR(ErrMsg);
      fprintf(stderr,
          "Extension plug-in F-1003: %s [Symbol PARSER_GetParser not found]\n",ErrMsg);
      DLL_CLOSE(ParserLib->DllHandle);
      free(ParserLib->Name);
      free(ParserLib);
      return;
   }

   /* Connect extension */
   if (PARSER_OK!=GetParser(&(ParserLib->Xi))) {
      fprintf(stderr,
         "Extension plug-in F-1004: GetParser call failure [%s]\n",
         ParserLib->Name);
      DLL_CLOSE(ParserLib->DllHandle);
      free(ParserLib->Name);
      free(ParserLib);
      return;
   }

   /* Check for SXAS Features */
   ErrMsg = NULL;
   if (ParserLib->Xi.Version_key < PARSER_Version) {
      fprintf(stderr,
         "Extension plug-in F-1005: DLL version too "
         "old [%#x vs %#x]\n",ParserLib->Xi.Version_key,
         PARSER_Version );
      DLL_CLOSE(ParserLib->DllHandle);
      free(ParserLib->Name);
      free(ParserLib);
      return;
   }
   ErrMsg = NULL;
   if (0==ParserLib->Xi.Version_key)      ErrMsg="Extension plug-in F-1006: Bad version number";
   if (0==ParserLib->Xi.Version_str)      ErrMsg="Extension plug-in F-1007: Unable to find out version";
   if (NULL==ParserLib->Xi.Abstract)      ErrMsg="Extension plug-in F-1008: No abstract available";
   if (NULL==ParserLib->Xi.ShortAbstract) ErrMsg="Extension plug-in F-1009: No short abstract available";
   if (NULL==ParserLib->Xi.ExtConnect)    ErrMsg="Extension plug-in F-1010: No assembling entry";
   if (NULL != ErrMsg) {
      fprintf(stderr,"%s [%s]\n",ErrMsg,ParserLib->Name);
      DLL_CLOSE(ParserLib->DllHandle);
      free(ParserLib->Name);
      free(ParserLib);
      return;
   } else {
      int NextAvailable;
      int i;
      
      for (NextAvailable=0;(NextAvailable<MAX_PARSER_NR) && (NULL!=ParserList[NextAvailable]);NextAvailable++) {
         for (i=0;(i<ParserList[NextAvailable]->Xi.PortSize) && (i<ParserLib->Xi.PortSize);i++) {
            if (
                (ParserList[NextAvailable]->Xi.Port[i] & ParserLib->Xi.Port[i]) && 
                (
                 (ParserList[NextAvailable]->Xi.PortSize!=ParserLib->Xi.PortSize)
                 ||
                 (0!=memcmp(ParserList[NextAvailable]->Xi.Port,ParserLib->Xi.Port,
                            ParserList[NextAvailable]->Xi.PortSize))
                 ||
                 (0!=strcmp(ParserList[NextAvailable]->Name,ParserLib->Name))
                )
               )
               {
               PARSER_ParserT * FormerLib;
               
               fprintf(stderr,
                  "Extension plug-in F-1011: [%s]: takes precedence on [%s] for port %d\n",
                  ParserLib->Name,ParserList[NextAvailable]->Name,i-1);

               /* New connexion wins. Former lib of this port is disconnected */
               FormerLib = ParserList[NextAvailable];
               ParserList[NextAvailable] = ParserLib;
               SetCallbacks(&ParserLib->Xi);

               /* Free former DLL */
               DLL_CLOSE(FormerLib->DllHandle);
               free(FormerLib->Name);
               free(FormerLib);
               return;
            }
         }
      }
      if (NextAvailable==MAX_PARSER_NR) {
         fprintf(stderr,
            "Extension plug-in F-1012: [%s]: No more extension slot available\n",
            ParserLib->Name);
         DLL_CLOSE(ParserLib->DllHandle);
         free(ParserLib->Name);
         free(ParserLib);
         return;
      }
      ParserList[NextAvailable] = ParserLib;
      SetCallbacks(&ParserLib->Xi);
#if DEBUG
      fprintf(stderr,"Extension plug-in: %s\n",ParserLib->Xi.Abstract);
#endif
   }
}

/*-------------------------------------------------------------------------*/
/*
   Routine print_version

   Description : Display version of assembler itself plus all connected DLLs

   Input :     stream (FILE *) output stream for message

   Output :    none.
............................................................................
*/
void print_version ( FILE * stream ) {
   int ParserId;

#if 0
   /* No utils_version.h for now */
   fprintf(stream,version_str);
#endif

   for (ParserId=0;ParserId<MAX_PARSER_NR;ParserId++) {
      if (NULL!=ParserList[ParserId]) {
         if (NULL!=ParserList[ParserId]->Xi.Abstract) {
            fprintf(stream,"%s\n\n",ParserList[ParserId]->Xi.Abstract);
         }
      }
   }
}

/*-------------------------------------------------------------------------*/
/*
   Routine print_API_version

   Description : Display version of assembler API

   Input :     stream (FILE *) output stream for message

   Output :    none.
............................................................................
*/
void print_API_version ( FILE * stream ) {
   int ParserId;

#if 0
   /* No utils_version.h for now */
   fprintf(stream,version_API_str,PARSER_Version);
#else
   fprintf(stream,"Parser version %d\n",PARSER_Version);
#endif

}
