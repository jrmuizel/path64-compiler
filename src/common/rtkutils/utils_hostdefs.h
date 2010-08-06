/*###INCLUDE###<copyright.in>
 * Purpose          : Header file defining function names depending on the    *
 *                    running host (Solaris, Linux, Windows).                 *
 * History          : 04/10/04 - First implementation.                        *
 *----------------------------------------------------------------------------*/
#ifndef utils_hostdefs_h
#define utils_hostdefs_h

/* system includes */
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

/*--------------------------------------------------------*
 * Names used in source files are from Solaris and Linux. *
 * Define macros to match Windows corresponding ones.     *
 *--------------------------------------------------------*/
#ifdef WIN32 /* Windows Host */

#ifdef strtoll
#undef strtoll
#endif
//HC-060710 MINGW #define strtoll _wcstoi64

#ifdef strcasecmp
#undef strcasecmp
#endif
#define strcasecmp _stricmp

#ifdef strncasecmp
#undef strncasecmp
#endif
#define strncasecmp _strnicmp

#ifdef strdup
#undef strdup
#endif
#define strdup _strdup

#ifdef snprintf
#undef snprintf
#endif
#define snprintf _snprintf

#ifdef vnsprintf
#undef vsnprintf
#endif
#define vsnprintf _vsnprintf

#ifdef stat
#undef stat
#endif
#define stat _stat

#ifdef chmod
#undef chmod
#endif
#define chmod _chmod

#ifdef umask
#undef umask
#endif
#define umask _umask

#ifdef getpid
#undef getpid
#endif
#define getpid _getpid

/* Warning, this function is to be used*/
/* with caution.                       */
/* May favor security leaks.           */
#ifdef mktemp
#undef mktemp
#endif
#define mktemp _mktemp

#ifdef sleep
#undef sleep
#endif
#define sleep( x ) Sleep( 1000 * x )

#endif /* WIN32 */

#endif /* utils_hostdefs_h */
