/** \file gensyn_defs.h
    \brief types definition depending on host platform.

*/
#ifndef __DEFS_H__
#define __DEFS_H__

#if defined(__sun)
typedef unsigned short uint16;               /**< unsigned 16-bit word */
typedef short int16;                         /**< signed 16-bit word */
typedef long int32;                          /**< signed 32-bit word */
typedef unsigned long uint32;                /**< unsigned 32-bit word */
typedef long long int64;                     /**< signed 64-bit word */
typedef unsigned long long uint64;           /**< unsigned 64-bit word */
#elif defined(linux) || defined(__MINGW32__)
typedef unsigned short uint16;               /**< unsigned 16-bit word */
typedef short int16;                         /**< signed 16-bit word */
typedef long int32;                          /**< signed 32-bit word */
typedef unsigned long uint32;                /**< unsigned 32-bit word */
typedef long long int64;                     /**< signed 64-bit word */
typedef unsigned long long uint64;           /**< unsigned 64-bit word */
#elif defined(WIN32) || defined(__CYGWIN__)
typedef unsigned short uint16;               /**< unsigned 16-bit word */
typedef short int16;                         /**< signed 16-bit word */
typedef long int32;                          /**< signed 32-bit word */
typedef unsigned long uint32;                /**< unsigned 32-bit word */
typedef __int64 int64;                       /**< signed 64-bit word */
typedef unsigned __int64 uint64;             /**< unsigned 64-bit word */
#ifdef strcasecmp
   #undef strcasecmp
#endif
#define strcasecmp _stricmp

#ifdef snprintf
   #undef snprintf
#endif
#define snprintf _snprintf
#else
#error "Unknown target, please adapt gensyn_defs.h to your platform"
#endif

#define MAX_SEM_LEN 256 /**< Maximum identifier length (in character) */
#endif /* __DEFS_H__ */
