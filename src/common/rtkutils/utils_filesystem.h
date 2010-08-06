
#ifndef utils_filesystem_h
#define utils_filesystem_h

/* local includes */
#include "utils_version_id.h"
#include "utils_types.h"
#include "utils_hostdefs.h"
#include "utils_memory.h"

#include <sys/types.h>
#include <sys/stat.h>

typedef struct stat utilsFileStatus;

#ifdef __cplusplus
extern "C" {
#endif

extern int utilsGetFileSize( char * path );
extern int utilsGetFileDate( char * path );

extern int utilsGetFileStatus( utilsFileStatus * status, char * path );
extern int utilsGetFileStatusSize( utilsFileStatus * status );
extern int utilsGetFileStatusDate( utilsFileStatus * status );

#ifdef __cplusplus
}
#endif

#endif

