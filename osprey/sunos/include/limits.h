
/* We use _GCC_LIMITS_H_ because we want this not to match
   any macros that the system's limits.h uses for its own purposes.  */

#ifndef _LIMITS_H
#include_next <limits.h>
#endif

#define LONGLONG_MAX LLONG_MAX
#define LONGLONG_MIN LLONG_MIN
#define ULONGLONG_MAX ULLONG_MAX

