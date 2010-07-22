
#ifndef __DRIVER_TARG_INFO_H__
#define __DRIVER_TARG_INFO_H__

#include <stddef.h>
#include "opt_actions.h"

// Targets definitions
typedef struct targ_info_t_ {
    const char *targ_name;      // Target name
    const char *arch_name;      // Architecture name
    const char *abi_name;       // ABI name
    ABI abi;                    // ABI
    const char *crt_path;       // Path to CRT runtime
#ifdef __linux__
    const char *dynamic_linker; // Dynamic linker for target
#endif // __linux__
#ifndef PATH64_ENABLE_PSCRUNTIME
    const char *libsupcpp_path;     // Path to supc++
    const char *libstdcpp_path;     // Path to stdc++
    const char *libgcc_path;        // Path to libgcc
    const char *libgcc_eh_path;     // Path to libgcc_eh
    const char *libgcc_s_path;      // Path to libgcc_s
#endif // PATH64_ENABLE_PSCRUNTIME
} targ_info_t;

extern targ_info_t targ_info[];
extern size_t targ_info_size;

#endif // __DRIVER_TARG_INFO_H__

