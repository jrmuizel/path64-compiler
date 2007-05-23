/*-------------------------------------------------------------------------*/
/*
 *      Copyright 2002, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 *      STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 *
 * File : utils_print.c
 *
 * Description : Routines for output of standardized messages (info, warning
 * and error)
............................................................................
*/

#include "utils_print.h"

utilsVersion * utils_print_version = & libUtils_version;

/*--------------------------------------------------------------------------------------*
*
* What: set info, warning and error output string selection
*
* Inputs:
*
* - char * set: selection string: sequence of words (sequence of alnum chars separated by
* non-alnum chars). The selection string will be given as reference string to search
* given output prefix using utilsStringIsOneOf( ). Note that given string is
* duplicated.
*
* Returns:
*
* - TRUE if setting is OK (set to NULL or duplication success)
*
*---------------------------------------------------------------------------------------*/

int utilsPrintSelectInfo( char * set ) {
  return utilsOptionSet( & utils_option_print_select_info, set );
}

int utilsPrintSelectWarning( char * set ) {
  return utilsOptionSet( & utils_option_print_select_warning, set );
}

int utilsPrintSelectError( char * set ) {
  return utilsOptionSet( & utils_option_print_select_error, set );
}

/*--------------------------------------------------------------------------------------*
*
* What: defines output stream for info, warning and error messages
*
*---------------------------------------------------------------------------------------*/

#if 0 /* Win32 does not support static definitions */
static FILE * utils_print_stream_info    = stdout;
static FILE * utils_print_stream_warning = stdout;
static FILE * utils_print_stream_error   = stderr;
#else
#define utils_print_stream_info    stdout
#define utils_print_stream_warning stdout
#define utils_print_stream_error   stderr
#endif

/*--------------------------------------------------------------------------------------*
*
* What: Prints info, warning and error message
*
* Inputs:
*
* Returns:
*
*---------------------------------------------------------------------------------------*/

void utilsPrintInfo( char * prefix, char * format, ... ) {
  va_list ap;
  va_start( ap, format );
  if ( ( utils_option_print_select_info.value == NULL ) || utilsStringIsOneOf( utils_option_print_select_info.value->argument, prefix ) ) {
    assert( format != NULL );
    fprintf( utils_print_stream_info, "%s info   : (%s) ", UTILS_STRING_SAFE( utils_print_version->name ), UTILS_STRING_SAFE( prefix ) );
    vfprintf( utils_print_stream_info, format, ap );
    fprintf( utils_print_stream_info, "\n" );
    fflush( utils_print_stream_info );
  }
  va_end( ap );
}

void utilsPrintWarning( char * prefix, char * format, ... ) {
  va_list ap;
  va_start( ap, format );
  if ( ( utils_option_print_select_warning.value == NULL ) || utilsStringIsOneOf( utils_option_print_select_warning.value->argument, prefix ) ) {
    assert( format != NULL );
    fprintf( utils_print_stream_warning, "%s warning: (%s) ", UTILS_STRING_SAFE( utils_print_version->name ), UTILS_STRING_SAFE( prefix ) );
    vfprintf( utils_print_stream_warning, format, ap );
    fprintf( utils_print_stream_warning, "\n" );
    fflush( utils_print_stream_warning );
  }
  va_end( ap );
}

void utilsPrintError( char * prefix, char * format, ... ) {
  va_list ap;
  va_start( ap, format );
  if ( ( utils_option_print_select_error.value == NULL ) || utilsStringIsOneOf( utils_option_print_select_error.value->argument, prefix ) ) {
    assert( format != NULL );
    fprintf( utils_print_stream_error, "%s error  : (%s) ", UTILS_STRING_SAFE( utils_print_version->name ), UTILS_STRING_SAFE( prefix ) );
    vfprintf( utils_print_stream_error, format, ap );
    fprintf( utils_print_stream_error, "\n" );
    fflush( utils_print_stream_error );
#ifdef _UTILS_DEBUG_PRINT
    fprintf( utils_print_stream_warning, "%s error  : (%s) ", UTILS_STRING_SAFE( utils_print_version->name ), UTILS_STRING_SAFE( prefix ) );
    vfprintf( utils_print_stream_warning, format, ap );
    fprintf( utils_print_stream_warning, "\n" );
    fflush( utils_print_stream_warning );
#endif
  }
  va_end( ap );
}

void utilsPrintFatal( char * prefix, char * format, ... ) {
  va_list ap;
  va_start( ap, format );
  assert( format != NULL );
  fprintf( utils_print_stream_error, "%s fatal  : (%s) ", UTILS_STRING_SAFE( utils_print_version->name ), UTILS_STRING_SAFE( prefix ) );
  vfprintf( utils_print_stream_error, format, ap );
  fprintf( utils_print_stream_error, "\n" );
  fflush( utils_print_stream_error );
#ifdef _UTILS_DEBUG_PRINT
  fprintf( utils_print_stream_warning, "%s fatal  : (%s) ", UTILS_STRING_SAFE( utils_print_version->name ), UTILS_STRING_SAFE( prefix ) );
  vfprintf( utils_print_stream_warning, format, ap );
  fprintf( utils_print_stream_warning, "\n" );
  fflush( utils_print_stream_warning );
#endif
  va_end( ap );
  freeallmem( );
  exit( 1 );
}

void utilsPrintInternalError( char * prefix, char * format, ... ) {
  va_list ap;
  va_start( ap, format );
  assert( format != NULL );
  fprintf( utils_print_stream_error, "%s internal error : (%s) ", UTILS_STRING_SAFE( utils_print_version->name ), UTILS_STRING_SAFE( prefix ) );
  vfprintf( utils_print_stream_error, format, ap );
  fprintf( utils_print_stream_error, "\n" );
  fflush( utils_print_stream_error );
#ifdef _UTILS_DEBUG_PRINT
  fprintf( utils_print_stream_warning, "%s internal error : (%s) ", UTILS_STRING_SAFE( utils_print_version->name ), UTILS_STRING_SAFE( prefix ) );
  vfprintf( utils_print_stream_warning, format, ap );
  fprintf( utils_print_stream_warning, "\n" );
  fflush( utils_print_stream_warning );
#endif
  va_end( ap );
  freeallmem( );
  exit( 1 );
}

/** utilsPrintSystemError
    Return system error to complete error messages.
    This routine works on Windows, Linux and Solaris.

    @param  void
    @return char* Returned system error message.
*/
char * utilsPrintSystemError( void ) {
  return strerror(errno);
}
