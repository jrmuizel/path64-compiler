

#ifndef LIB_VERSION_H_INCLUDED
#define LIB_VERSION_H_INCLUDED


#include <math.h>


#if defined(__linux)

#define PATH64_LIB_VERSION_SVID (_LIB_VERSION == _SVID_)
#define PATH64_LIB_VERSION_POSIX (_LIB_VERSION == _POSIX_)

#elif defined(__FreeBSD__)

#define PATH64_LIB_VERSION_SVID 0
#define PATH64_LIB_VERSION_POSIX 1

struct exception {
    int type;
    char *name;
    double arg1;
    double arg2;
    double retval;
};

/* Types of exceptions in the `type' field.  */
# define DOMAIN         1
# define SING           2
# define OVERFLOW       3
# define UNDERFLOW      4
# define TLOSS          5
# define PLOSS          6

#define matherr(x) (1)

#elif defined(__NetBSD__)

#define PATH64_LIB_VERSION_SVID 0
#define PATH64_LIB_VERSION_POSIX 1

#elif defined(__sun)

#define PATH64_LIB_VERSION_SVID 0
#define PATH64_LIB_VERSION_POSIX 0

#else

#error "You should define lib version macros for your platform"

#endif

#endif // LIB_VERSION_H_INCLUDED

