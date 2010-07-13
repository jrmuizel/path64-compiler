
#ifndef utils_paths_h
#define utils_paths_h

#include <limits.h>

/* local includes */
#include "utils_version_id.h"
#include "utils_types.h"
#include "utils_hostdefs.h"
#include "utils_memory.h"

#if defined( __linux )

#include <sys/types.h>
#include <sys/stat.h>

typedef struct stat utilsPathStat;

#elif defined( __sun )

#include <sys/types.h>
#include <sys/stat.h>

typedef struct stat utilsPathStat;

#elif defined( WIN32 )

#include <sys/types.h>
#include <sys/stat.h>

typedef struct _stat utilsPathStat;

#elif defined( __CYGWIN32__ )

#include <sys/types.h>
#include <sys/stat.h>

typedef struct _stat utilsPathStat;

#else

#error "Cannot guess OS..."

#endif

#ifdef __cplusplus
extern "C" {
#endif

	extern int utilsPathGetSize( char * path );
	extern int utilsPathGetDate( char * path );
	extern int utilsPathIsDirectory( char * path );
	extern int utilsPathGetStat( char * path, utilsPathStat * stat );
	extern int utilsPathStatGetSize( utilsPathStat * stat );
	extern int utilsPathStatGetDate( utilsPathStat * stat );
	extern int utilsPathStatIsDirectory( utilsPathStat * stat );

	extern char * utilsPathGotoBase( char * path );
	extern char * utilsPathGotoFile( char * path );
	extern char * utilsPathGotoExtension( char * path );

	extern char * utilsPathCopyExtension( char * path );
	extern char * utilsPathCopyBaseDir( char * path );
	extern char * utilsPathCopyBaseName( char * path );
	extern char * utilsPathCopyFileName( char * path );
	extern char * utilsPathCopyDirName( char * path );
	extern char * utilsPathCopyRawName( char * path );

#ifdef __cplusplus
}
#endif

#endif

