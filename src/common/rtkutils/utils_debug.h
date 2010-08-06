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
 * File : debug.h
 *
 * Description : macros and defines for debug output facilities
............................................................................
*/

#ifndef utils_debug_h
#define utils_debug_h

/* system includes */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

/* local includes */
#include "utils_version_id.h"
#include "utils_types.h"
#include "utils_hostdefs.h"
#include "utils_memory.h"
#include "utils_string.h"

/*----------------------------------------------------------------------------*
*
* What: debug trace utilities.
*
* Debug facilities are set of macros to output run-time debug messages with
* formatted prefix.
*
* These facilities are rather performance eaters, but when not activated,
* debug output code is enterely nopified and has * no * interaction with
* optimization of code.
*
* To get debug output activated, define one of preprocessor constant:
*
*     -D _UTILS_DEBUG_MEMORY
*     -D _UTILS_DEBUG_PRINT_LOCATION
*     -D _UTILS_DEBUG_PRINT
*
* Debug trace facilities are mainly printf( )-style calls. Utils gives a "prefix"
* and a message as a printf( )-style argument list. The message is formated with
* standard prefix "DEBUG", utils prefix, optional "at file:line" location prefix,
* and printf( ) interpreted message.
*
* Exemple:
*
*     UTILS_DEBUG( "TEST", ( "Here is %s", __FUNCTION__ ) );
*
* Outputs (compiled with -D_UTILS_DEBUG_PRINT_LOCATION)
*
*     DEBUG: TEST     at x.c:14       : Here is main
*
* Note that a carriage return is inserted after given message.
*
* Once debug facilities activated, output of messages can be controled
* (output/trashed) by controling the utils prefix.
*
* Exemple:
*
*     UTILS_DEBUG_SELECT( "THIS IS A TEST" );
*     UTILS_DEBUG( "TEST", ( "Here is %s", __FUNCTION__ ) );
*     UTILS_DEBUG( "THIS", ( "Hello world" ) );
*     UTILS_DEBUG( "THAT", ( "That's all folks" ) );
*
* Outputs (compiled with -D_UTILS_DEBUG_PRINT)
*
*     DEBUG: TEST     : Here is main
*     DEBUG: THIS     : Hello world
*
* Public inline macros are:
*
*     UTILS_DEBUG_SELECT( selection )
*
*         Defines message selection as a char * string.
*         Special value NULL defines < no message output > and special string
*         "*" defines < all message output > otherwise, a message is output
*         only if utils prefix is present as a word in selection.
*
*     UTILS_DEBUG( prefix, args )
*
*         Basic debug message call to output message as done by a single
*         printf( ) call.
*
*         Note that < args > are printf function call arguments including
*         brackets to work arround problems with some pre-processors that does
*         not accept variable length argument list on macros.
*
*     UTILS_DEBUG_OPEN( prefix )
*     UTILS_DEBUG_PRINT( prefix, args )
*     UTILS_DEBUG_CLOSE( prefix )
*
*         When message to output cannot be expressed as a single printf( ),
*         and if you wish to avoid automatic carriage return effects you shall
*         do OPEN, PRINT and CLOSE calls.
*
*     UTILS_DEBUG_DO( prefix, code )
*
*         This macro helps you to implement or nopified parts of code between
*         braces whenever you are in debug mode or not.
*
*----------------------------------------------------------------------------*/

#ifdef _UTILS_DEBUG_MEMORY
#ifndef _UTILS_DEBUG_PRINT_LOCATION
#define _UTILS_DEBUG_PRINT_LOCATION
#endif
#endif

#ifdef _UTILS_DEBUG_PRINT_LOCATION
#ifndef _UTILS_DEBUG_PRINT
#define _UTILS_DEBUG_PRINT
#endif
#endif

extern int utilsDebugIsSelected( char * prefix );

extern int utilsDebugSelect( char * selection );

#ifdef _UTILS_DEBUG_PRINT

	#ifdef __RELEASE__
	#error INCOHERENT BUILD MIXING RELEASE & DEBUG
	#endif

	#include "libUtilsOptions.h"

	#ifdef _UTILS_DEBUG_PRINT_LOCATION

		/*-------------------------------------------------------*
		* private inlines with "at file:line" information in prefix
		*-------------------------------------------------------*/

		#define __UTILS_DEBUG_OPEN( prefix )  printf( "DEBUG: %s\tat %s:%d\t: ", prefix, __FILE__, __LINE__ );
		#define __UTILS_DEBUG_PRINT( args )   printf args;
		#define __UTILS_DEBUG_CLOSE( prefix ) printf( "\n" );

	#else

		/*-------------------------------------------------------*
		* private inlines with simple prefix
		*-------------------------------------------------------*/

		#define __UTILS_DEBUG_OPEN( prefix )  printf( "DEBUG: %s\t: ", prefix );
		#define __UTILS_DEBUG_PRINT( args )   printf args;
		#define __UTILS_DEBUG_CLOSE( prefix ) printf( "\n" );

	#endif

	/*-------------------------------------------------------*
	* public debug facilities inlines
	*-------------------------------------------------------*/

	#define UTILS_DEBUG_SELECT( selection ) utilsDebugSelect( selection )

	#define UTILS_DEBUG_DO( prefix, code ) { if ( utilsDebugIsSelected( prefix ) ) { code } }

	#define UTILS_DEBUG_OPEN( prefix )        UTILS_DEBUG_DO( prefix, __UTILS_DEBUG_OPEN( prefix ) )

	#define UTILS_DEBUG_PRINT( prefix, args ) UTILS_DEBUG_DO( prefix, __UTILS_DEBUG_PRINT( args ) )

	#define UTILS_DEBUG_CLOSE( prefix )       UTILS_DEBUG_DO( prefix, __UTILS_DEBUG_CLOSE( prefix ) )

	#define UTILS_DEBUG( prefix, args ) \
		UTILS_DEBUG_DO( prefix, \
			__UTILS_DEBUG_OPEN( prefix ); \
			__UTILS_DEBUG_PRINT( args ); \
			__UTILS_DEBUG_CLOSE( prefix ); \
		)

#else

	/*---------------------------------------------------------------------------*
	* public debug facilities inlines when debug not activated is nopified code
	*---------------------------------------------------------------------------*/

	#define UTILS_DEBUG_DO( prefix, code )

	#define UTILS_DEBUG_SELECT( selection )

	#define UTILS_DEBUG_OPEN( prefix )

	#define UTILS_DEBUG_PRINT( prefix, args )

	#define UTILS_DEBUG_CLOSE( prefix )

	#define UTILS_DEBUG( prefix, args )

#endif

#endif

