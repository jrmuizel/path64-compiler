
#ifndef _utils_option_h_
#define _utils_option_h_

#include <stdio.h>

typedef enum utilsOptionType {
	UTILS_OPTION_TYPE   = 0,
	UTILS_OPTION_TAG    = ( 1 << 0 ),
	UTILS_OPTION_TOGGLE = ( 1 << 1 ),
	UTILS_OPTION_SCALAR = ( 1 << 2 ),
	UTILS_OPTION_LIST   = ( 1 << 3 ),
	UTILS_OPTION_OPEN   = ( 1 << 4 ),
	UTILS_OPTION_CLOSE  = ( 1 << 5 ),
} utilsOptionType;

typedef struct utilsOptionValue {
	struct utilsOptionValue * next;
	char * argument;
} utilsOptionValue;

typedef enum utilsOptionFlags {
	UTILS_OPTION_FLAG   = 0,
	/* public flags */
	UTILS_OPTION_RESIST = ( 1 << 0 ), /**< option and its value is not strip off argc/argv */
	UTILS_OPTION_HIDDEN = ( 1 << 1 ), /**< option is not shown is help. Note also that hidden option lost priority when partial matching */
	UTILS_OPTION_CASE   = ( 1 << 2 ), /**< allow case insensitive matching */
	UTILS_OPTION_ONCE   = ( 1 << 3 ), /**< require that option must be parsed once by command line parsing */
	UTILS_OPTION_PREFIX = ( 1 << 4 ), /**< option and value may be stuck together in same argv (example -D<macro>, -step<int>) */
	UTILS_OPTION_LINE   = ( 1 << 5 ), /**< insert new line before this option in help */
	UTILS_OPTION_BLANK  = ( 1 << 6 ), /**< insert new line before this option in help */
	UTILS_OPTION_IGNORE = ( 1 << 7 ), /**< ignore option */
	/* internal flags */
	UTILS_OPTION_DONE   = ( 1 << 16 )  /**< tag that option is already parsed (private flag that user should not set at initialization) */
} utilsOptionFlags;

typedef struct utilsOption {
	enum utilsOptionType type;
	char * name;
	char * data;
	char * init;
	enum utilsOptionFlags flags;
	char * help;
	int ( * callback )( struct utilsOption * option, char * argument );
	void * extension;
	struct utilsOption * next;
	struct utilsOptionValue * value;
} utilsOption;

/* local includes */
#include "utils_version_id.h"
#include "utils_types.h"
#include "utils_print.h"
#include "utils_memory.h"

#define UTILS_OPTION_SIZEOF( x ) ( sizeof( x ) / sizeof( utilsOption ) )
#define UTILS_OPTION_STRUCT( x ) ( ( utilsOption * )( & ( x ) ) )

extern int utilsOptionInit( utilsOption * more, int size );
extern int utilsOptionParse( int * argc, char ** argv, utilsOption * options, int last );
extern int utilsOptionUsage( FILE * output, utilsOption * options, char * arguments, int all );
extern int utilsOptionHelp ( FILE * output, utilsOption * options, int prefix );
extern int utilsOptionDebug( FILE * output, utilsOption * options );

#define utilsOptionIsUser( option ) ( ( option->flags & UTILS_OPTION_DONE ) != 0 )
extern int utilsOptionIsDefined( utilsOption * option );
extern int utilsOptionValueCount( utilsOption * option );

extern int utilsOptionSet( utilsOption * option, char * value );
extern int                utilsOptionGetTag   ( utilsOption * option );
extern int                utilsOptionGetToggle( utilsOption * option );
extern char *             utilsOptionGetScalar( utilsOption * option );
extern utilsOptionValue * utilsOptionGetList  ( utilsOption * option );
extern utilsOptionValue * utilsOptionGetValue ( utilsOption * option, char * value );

extern int utilsOptionSetByName( utilsOption * options, char * name, char * value );
extern utilsOption * utilsOptionGetByName ( utilsOption * options, char * name );

#endif



