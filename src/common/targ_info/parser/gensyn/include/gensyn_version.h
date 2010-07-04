
#ifndef gensyn_version_h
#define gensyn_version_h

#include "libUtils.h"

#define PROCESSOR "STxP70"

#undef  UTILS_VERSION_ID
#define UTILS_VERSION_ID sxgensyn

#undef  UTILS_VERSION_DESCRIPTION
#ifdef __TOOLKIT__
#define UTILS_VERSION_DESCRIPTION PROCESSOR " reconfiguration toolkit, assembler/linker target specific code generator for extensions"
#else
#define UTILS_VERSION_DESCRIPTION "assembler/linker target specific code generator"
#endif

extern utilsVersion gensyn_version;

#endif
