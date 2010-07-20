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

#include <malloc.h>
#include <stdio.h>


// Returns current target architecture
char * target_architecture() {
    return NULL;
//#ifdef TARG_X8664
//    return "x8664";
//#else
//#error "Only X8664 multilib is supported for now"
//#endif
}


// Returns current target abi
char * target_abi() {
    return NULL;
//#ifdef TARG_X8664
//    return abi == ABI_N32 ? "32" : "64";
//#else
//#error "Only X8664 multilib is supported for now"
//#endif
}


char *target_library_path() {
    char *lib_path = NULL;
	char *root_prefix = directory_path(get_executable_dir());

    asprintf(&lib_path, "%s%s/%s/%s",
             root_prefix, LIBPATH, target_architecture(), target_abi());
    free(root_prefix);

    return lib_path;
}


char * target_runtime_path() {
    return get_crt_path();
}


