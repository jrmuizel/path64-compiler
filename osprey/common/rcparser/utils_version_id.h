
#ifndef utils_version_id_h
#define utils_version_id_h

/*======================================================================
* Backward compatibility with old defines
*/

#if !defined( UTILS_VERSION_MAJOR ) && defined( V_MAJ )
#define UTILS_VERSION_MAJOR V_MAJ
#endif

#if !defined( UTILS_VERSION_MINOR ) && defined( V_MIN )
#define UTILS_VERSION_MINOR V_MIN
#endif

#if !defined( UTILS_VERSION_MICRO ) && defined( V_B )
#define UTILS_VERSION_MICRO V_B
#endif

#if !defined( UTILS_VERSION_STATUS )
#  if defined(__RELEASE__) || defined(RELEASE)
#    ifdef NDEBUG
#      define UTILS_VERSION_STATUS UTILS_VERSION_STATUS_RELEASE
#    else
#      define UTILS_VERSION_STATUS UTILS_VERSION_STATUS_BETA
#    endif
#  else
#    ifdef NDEBUG
#      define UTILS_VERSION_STATUS UTILS_VERSION_STATUS_ALPHA
#    else
#      define UTILS_VERSION_STATUS UTILS_VERSION_STATUS_DEVELOPMENT
#    endif
#  endif
#endif

/*======================================================================
* Set default values if not defined
*/

#ifndef UTILS_VERSION_ID
#define UTILS_VERSION_ID libUtils
#endif

#ifndef UTILS_VERSION_NAME
#define UTILS_VERSION_NAME UTILS_VERSION_STRINGIFY( UTILS_VERSION_ID )
#endif

#ifndef UTILS_VERSION_DESCRIPTION
#define UTILS_VERSION_DESCRIPTION "STS/Compilation team utilities"
#endif

#ifndef UTILS_VERSION_LEGAL
#define UTILS_VERSION_LEGAL "Copyright 2004-2006 STMicroelectronics, Inc. All Rights Reserved."
#endif

#ifndef UTILS_VERSION_MAJOR
#define UTILS_VERSION_MAJOR 0
#endif

#ifndef UTILS_VERSION_MINOR
#define UTILS_VERSION_MINOR 0
#endif

#ifndef UTILS_VERSION_MICRO
#define UTILS_VERSION_MICRO 0
#endif

#ifndef UTILS_VERSION_TAG
#define UTILS_VERSION_TAG 0
#endif

#define UTILS_VERSION_STATUS_DEVELOPMENT "Dev"
#define UTILS_VERSION_STATUS_ALPHA       "Alpha"
#define UTILS_VERSION_STATUS_BETA        "Beta"
#define UTILS_VERSION_STATUS_RELEASE     "Rel"
#define UTILS_VERSION_STATUS_FIX         "Fix"
#ifndef UTILS_VERSION_STATUS
#define UTILS_VERSION_STATUS UTILS_VERSION_STATUS_DEVELOPMENT
#endif

/*======================================================================
* Defines version container inside application
*/

typedef struct utilsVersion {
	char * name;         /**< name of application              (cf macro UTILS_VERSION_{ID/NAME} aka Pname) */
	char * description;  /**< short description of application (cf macro UTILS_VERSION_DESCRIPTION) */
	int major;           /**< major version id                 (cf macro UTILS_VERSION_MAJOR aka V_MAJ) */
	int minor;           /**< minor version id                 (cf macro UTILS_VERSION_MINOR aka V_MIN) */
	int micro;           /**< micro version id                 (cf macro UTILS_VERSION_MICRO aka V_B or V_BU) */
	int tag;             /**< tag   version id                 (cf macro UTILS_VERSION_TAG ; the subversion repository tag for example) */
	char * status;       /**< status string                    (cd macro UTILS_VERSION_STATUS) */
	char * legal;        /**< legal informatin string          (cf macro UTILS_VERSION_LEGAL) */
	char * date;         /**< date of compilation              (cf macro __DATE__) */
	char * time;         /**< time of compilation              (cf macro __TIME__) */
	char * what;         /**< 'whatstring' with all previous data in a synthetic string */
} utilsVersion;

/*======================================================================
* Some macros to fillin the container
*/

#define _UTILS_VERSION_STRINGIFY_( x ) #x
#define UTILS_VERSION_STRINGIFY( x ) _UTILS_VERSION_STRINGIFY_( x )

#define _UTILS_VERSION_CAT_( x, y ) x##y
#define UTILS_VERSION_CAT( x, y ) _UTILS_VERSION_CAT_( x, y )

#define UTILS_VERSION_STRING \
		UTILS_VERSION_STATUS \
		" V" UTILS_VERSION_STRINGIFY( UTILS_VERSION_MAJOR ) \
		"."  UTILS_VERSION_STRINGIFY( UTILS_VERSION_MINOR ) \
		"-"  UTILS_VERSION_STRINGIFY( UTILS_VERSION_MICRO ) \
		"."  UTILS_VERSION_STRINGIFY( UTILS_VERSION_TAG   ) \

#define UTILS_VERSION_WHAT( ) \
		"@(#)" UTILS_VERSION_NAME " " UTILS_VERSION_STRING \
		" (" __DATE__ " " __TIME__ ") " UTILS_VERSION_DESCRIPTION " " UTILS_VERSION_LEGAL

#define UTILS_VERSION( ) \
	{ \
		UTILS_VERSION_NAME, \
		UTILS_VERSION_DESCRIPTION, \
		UTILS_VERSION_MAJOR, \
		UTILS_VERSION_MINOR, \
		UTILS_VERSION_MICRO, \
		UTILS_VERSION_TAG, \
		UTILS_VERSION_STATUS, \
		UTILS_VERSION_LEGAL, \
		__DATE__, \
		__TIME__, \
		UTILS_VERSION_WHAT( ) \
	};

/*======================================================================
* Some externals
*/

/* system includes */
#include <stdio.h>

/* local includes */
#include "utils_hostdefs.h"
#include "utils_types.h"
#include "utils_print.h"

extern int utilsVersionInit( utilsVersion * init );

extern int utilsVersion_fprintf( FILE * output );

extern int utilsVersion_snprintf( char * buffer, int size );


#endif

