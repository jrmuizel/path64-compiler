/*-------------------------------------------------------------------------*/
/*  
 *      Copyright 2005, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 *      STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 *  
 * File : memory.h
 *
 * Description : Header file of memory manager.
............................................................................
*/ 

#ifndef utils_memory_h
#define utils_memory_h

/* system includes */
#include <stdio.h>
#include <stdlib.h>

/* local includes */
#include "utils_version_id.h"
#include "utils_messages.h"
#include "utils_print.h"
#include "utils_debug.h"
#include "utils_types.h"
#include "utils_hostdefs.h"

#define ASSERT(test,p1,p2)  binassert((test),(p1),(p2),__FILE__,__LINE__)

extern void   *getmem_area0           (uint_32);
extern void   *getmem_area1           (uint_32);
extern void   *getmem_curarea         (uint_32);
extern void    set_curmemarea         (uint_32);
extern void    freemem_area0          (void   );
extern void    freemem_area1          (void   );
extern void    freemem_curarea        (void   );
extern void    freeallmem             (void   );
extern void   *explicit_alloc_memory  (uint_32);
extern void    explicit_dealloc_memory(void*  );
extern void    binassert              (int,char*,char*,char*,int);

/*----------------------------------------------------------------------------*
*
* memory allocation tracking utilities by debug trace
*
*----------------------------------------------------------------------------*/

#ifdef _UTILS_DEBUG_MEMORY

	#ifdef __RELEASE__
	#error INCOHERENT BUILD MIXING RELEASE & DEBUG
	#endif

	#define UTILS_MEMORY_NEW( destination, size, type ) { \
		destination = ( type * )calloc( size, sizeof( type ) ); \
		UTILS_DEBUG( "MEMORY", ( "NEW %p %d", destination, size * sizeof( type ) ) ); \
	}

	#define UTILS_MEMORY_REALLOC( destination, source, size, type ) { \
		UTILS_DEBUG( "MEMORY", ( "DELETE %p", source ) ); \
		destination = ( type * )realloc( source, size * sizeof( type ) ); \
		UTILS_DEBUG( "MEMORY", ( "NEW %p %d", destination, size * sizeof( type ) ) ); \
	}

	#define UTILS_MEMORY_STRDUP( destination, source ) { \
		destination = strdup( source ); \
		UTILS_DEBUG( "MEMORY", ( "NEW %p %d", destination, strlen( source ) ) ); \
	}

	#define UTILS_MEMORY_STRNDUP( destination, source, length ) { \
		UTILS_MEMORY_NEW( destination, length + 1 ); \
		strncpy( destination, source, length ); \
	}

	#define UTILS_MEMORY_DELETE( source ) { \
		UTILS_DEBUG( "MEMORY", ( "DELETE %p", source ) ); \
		free( source ); \
		source = NULL; \
	}

#else

	#define UTILS_MEMORY_NEW( destination, size, type )             destination = ( type * )calloc( size, sizeof( type ) )

	#define UTILS_MEMORY_REALLOC( destination, source, size, type ) destination = ( type * )realloc( source, size )

	#define UTILS_MEMORY_STRDUP( destination, source )              destination = strdup( source )

	#define UTILS_MEMORY_STRNDUP( destination, source, length )     { UTILS_MEMORY_NEW( destination, length + 1, char ); strncpy( destination, source, length ); }

	#define UTILS_MEMORY_DELETE( source )                           { free( source ); source = NULL; }


#endif

#endif
