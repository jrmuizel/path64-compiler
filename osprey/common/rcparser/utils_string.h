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
 * File : string.h
 *
 * Description : macros, defines and prototypes of string facilities
............................................................................
*/

#ifndef utils_string_h
#define utils_string_h

/* system includes */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

/* local includes */
#include "utils_version_id.h"
#include "utils_types.h"
#include "utils_hostdefs.h"
#include "utils_memory.h"

/*------------------------------------------------------------------*
*
* Preprocessing strings
*
*------------------------------------------------------------------*/

#define _UTILS_STRING_STRINGIFY_( x ) #x
#define UTILS_STRING_STRINGIFY( x ) _UTILS_STRING_STRINGIFY_( x )

#define _UTILS_STRING_CAT_( x, y ) x##y
#define UTILS_STRING_CAT( x, y ) _UTILS_STRING_CAT_( x, y )

/*------------------------------------------------------------------*
*
* What: a simple macro to use for a safe printf of string pointers
*
*------------------------------------------------------------------*/

#define UTILS_STRING_SAFE( s ) ( ( ( s ) != NULL ) ? ( s ) : "<null>" )

/*------------------------------------------------------------------*
*
* What: macro to help plural setting of messages
*
*------------------------------------------------------------------*/

#define UTILS_STRING_PLURAL( count, single, multiple ) ( ( ( unsigned )( count ) > 1 ) ? UTILS_STRING_SAFE( multiple ) : UTILS_STRING_SAFE( single ) )

#define UTILS_STRING_PLURAL_S( count ) UTILS_STRING_PLURAL( count, "", "s" )

/*------------------------------------------------------------------*
*
* What: macro to print boolean, toggle, question values
*
*------------------------------------------------------------------*/

#define UTILS_STRING_TOGGLE(   boolean ) ( ( boolean ) ? "on"   : "off"   )

#define UTILS_STRING_BOOLEAN(  boolean ) ( ( boolean ) ? "true" : "false" )

#define UTILS_STRING_QUESTION( boolean ) ( ( boolean ) ? "yes"  : "no"    )

/*------------------------------------------------------------------*
*
* What: public functions
*
*------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

	extern int utilsStringIsOneOf( char * reference, char * word );

	extern sint_32 utilsString_strtol( char * str, char ** trap,  int base );
	extern sint_64 utilsString_strtoll( char * str, char ** trap,  int base );

	extern uint_64 utilsStringToValue( char * str, char ** trap, int base, char * separators, char * escapes );
	extern uint_64 utilsStringToMask( char * str, char ** trap, int base, char * separators, char * escapes );

	extern int utilsStringGuessBaseByPrefix( char * str );
	extern char * utilsStringSkipBasePrefix( char * str );
	extern char * utilsStringGetBasePrefix( char * str );

	extern char * utilsStringAlloc( char * format, ... );
	extern char * utilsStringAppend( char * base, char * format, ... );

	extern int utilsStringTrimSpaces( char * base );

#ifdef __cplusplus
}
#endif

#endif

