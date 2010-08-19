#ifndef __DEFS_H__
#define __DEFS_H__

#if defined(__sun)
typedef unsigned short uint16;
typedef short int16;
typedef long int32;
typedef unsigned long uint32;
typedef long long int64;
typedef unsigned long long uint64;
#elif defined (linux)
typedef unsigned short uint16;
typedef short int16;
typedef long int32;
typedef unsigned long uint32;
typedef long long int64;
typedef unsigned long long uint64;
#elif defined (WIN32)
typedef unsigned short uint16;
typedef short int16;
typedef long int32;
typedef unsigned long uint32;
#ifndef __MINGW32__
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else
typedef long long int64;
typedef unsigned long long uint64;
#endif
#else
#error "Unknown target, please adapt defs.h to your platform"
#endif

#endif /* __DEFS_H__ */
