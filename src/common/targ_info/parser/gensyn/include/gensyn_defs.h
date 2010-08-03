/** \file gensyn_defs.h
    \brief types definition depending on host platform.

*/
#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdint.h>

typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

#if defined(WIN32) || defined(__CYGWIN__)
#ifdef strcasecmp
   #undef strcasecmp
#endif
#define strcasecmp _stricmp

#ifdef snprintf
   #undef snprintf
#endif
#define snprintf _snprintf
#endif // WIN32 || __CYGWIN__

#define MAX_SEM_LEN 256 /**< Maximum identifier length (in character) */
#endif /* __DEFS_H__ */
