
#ifndef __DRIVER_TARG_INFO_H__
#define __DRIVER_TARG_INFO_H__

// Targets definitions
typedef struct targ_info_t_ {
    const char * targ_name;     // Target name
    const char * arch_name;     // Architecture name
    const char * abi_name;      // ABI name
    const char * abi_option;    // ABI option name
    const char * crt_path;
} targ_info_t;

extern targ_info_t targ_info[];

#endif // __DRIVER_TARG_INFO_H__

