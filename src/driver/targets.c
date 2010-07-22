/*
  Copyright (C) 2010 PathScale Inc. All Rights Reserved.
*/


#include "targets.h"
#include "main_defs.h"
#include "string_utils.h"
#include "option_names.h"
#include "opt_actions.h"
#include "file_utils.h"
#include "objects.h"
#include "targ_info.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>


// Current target
static const targ_info_t *current_target = &targ_info[0];

static boolean target_arch_X8664 = FALSE;
static boolean target_arch_MIPS = FALSE;


void init_targets() {
    // selecting current target index depending on abi options
    for(size_t i = 0; i < targ_info_size; ++i) {
        if(targ_info[i].abi == abi) {
            current_target = &targ_info[i];
            break;
        }
    }

    // setting abi option if it is not set
    if(abi == -1) {
        toggle(&abi, current_target->abi);
    }

//    printf("CURRENT TARGET: %s\n", current_target->targ_name);

    if(strcmp(current_target->arch_name, "x8664") == 0) {
        target_arch_X8664 = TRUE;
    } else if(strcmp(current_target->arch_name, "mips") == 0) {
        target_arch_MIPS = TRUE;
    }
}


// Returns current target architecture
const char *target_architecture() {
    return current_target->arch_name;
}


// Returns current target abi
const char *target_abi() {
    return current_target->abi_name;
}


char *target_library_path() {
    char *lib_path = NULL;
	char *root_prefix = directory_path(get_executable_dir());

    asprintf(&lib_path, "%s%s/%s/%s",
             root_prefix, LIBPATH, target_architecture(), target_abi());
    free(root_prefix);

    return lib_path;
}


char *target_phase_path() {
    char *lib_path = NULL;
    char *root_prefix = directory_path(get_executable_dir());
    asprintf(&lib_path, "%s%s/%s", root_prefix, LIBPATH, target_architecture());
    free(root_prefix);

    return lib_path;
}


const char *target_runtime_path() {
    return current_target->crt_path;
}


#ifdef TARG_X8664
boolean is_target_arch_X8664() {
    return target_arch_X8664;
}
#endif // TARG_X8664


#ifdef TARG_MIPS
boolean is_target_arch_MIPS() {
    return target_arch_MIPS;
}
#endif // TARG_MIPS

