
#ifndef W_STDINT_H
#define W_STDINT_H

#include <host/config.h>

#ifndef USE_H_STDINT_H
/* use stdint.h provided with the system */
#include <stdint.h>
#else
/* use H_stdint.h wrapper */
#include "host/H_stdint.h"
#endif

#endif /* W_STDINT_H */
