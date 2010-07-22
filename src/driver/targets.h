/*
  Copyright (C) 2010 PathScale Inc. All Rights Reserved.
*/


#ifndef __DRIVER_TARGETS_H__
#define __DRIVER_TARGETS_H__


#include "basic.h"
#include "targ_info.h"


extern const targ_info_t *current_target;


void init_targets();


// Returns path to target library path
// Returned memory should be freed by caller
char *target_library_path();


// Returns current target phase path
// Returned memory should be freed by caller
char *target_phase_path();


// Returns path to target runtime path (location of crtX.o)
const char *target_runtime_path();


#ifdef __linux__
// Returns path to dynamic linker for target
const char *target_dynamic_linker();
#endif // __linux__


#ifdef TARG_X8664
// Returns TRUE if target architecture is X8664
boolean is_target_arch_X8664();
#endif // TARG_X8664


#ifdef TARG_MIPS
// Returns TRUE if target architecture is MIPS
boolean is_target_arch_MIPS();
#endif // TARG_MIPS

#endif // __DRIVER_TARGETS_H__

