
#ifndef utils_string_buffer_h
#define utils_string_buffer_h

/* system includes */
#include <stdlib.h>
#include <stdarg.h>

/* local includes */
#include "utils_version_id.h"
#include "utils_memory.h"

#ifdef __cplusplus
extern "C" {
#endif
	// a simple replacement of C++ string class in raw C

	typedef struct utilsStringBuffer {
		int size;
		int remain;
		int index;
		char * buffer;
	} utilsStringBuffer;

#define UTILS_STRING_BUFFER_ROUND ( 1 << 4 )
#define UTILS_STRING_BUFFER_ROUND_SIZE( size ) ( ( size & - UTILS_STRING_BUFFER_ROUND ) + UTILS_STRING_BUFFER_ROUND );

	extern utilsStringBuffer * utilsStringBufferNew( int size );

	extern int utilsStringBufferDelete( utilsStringBuffer * buffer );

	extern int utilsStringBufferReset( utilsStringBuffer * buffer, int size );

	extern int utilsStringBufferResize( utilsStringBuffer * buffer, int new_size );

	extern int utilsStringBufferPrintf( utilsStringBuffer * buffer, char * format, ... );

	extern int utilsStringBufferDebug( utilsStringBuffer * buffer, char * prefix, FILE * output );

	extern char * utilsStringBufferDetach( utilsStringBuffer * buffer );

#ifdef __cplusplus
}
#endif

#endif

