
#ifndef __DRIVER_TARG_INFO_H__
#define __DRIVER_TARG_INFO_H__

#include <stddef.h>
#include "opt_actions.h"

// Targets definitions
typedef struct targ_info_t_ {
    const char * targ_name;     // Target name
    const char * arch_name;     // Architecture name
    const char * abi_name;      // ABI name
    ABI abi;                    // ABI
    const char * crt_path;
} targ_info_t;

extern targ_info_t targ_info[];
extern size_t targ_info_size;

#endif // __DRIVER_TARG_INFO_H__

