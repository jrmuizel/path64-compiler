#ifndef __STRINGS_H
#define __STRINGS_H

#include_next <strings.h>

#define bzero(s, n) memset((void *)(s), 0, (size_t)(n))

#define bcopy(s, d, n) memcpy((void *)(d), (void *)(s), (size_t)(n))

#endif /* __STRINGS_H */

