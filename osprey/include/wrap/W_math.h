
#ifndef W_MATH_H
#define W_MATH_H

#include <host/config.h>

#ifndef USE_H_MATH_H
/* use math.h provided with the system */
#include <math.h>
#else
/* use H_math.h wrapper */
#include "host/H_math.h"
#endif

#endif /* W_MATH_H */
