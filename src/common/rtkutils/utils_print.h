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
 * File : print.h
 *
 * Description : Header file for generic routines for output of standardized
 * messages (info, warning and error)
............................................................................
*/

#ifndef utils_print_h
#define utils_print_h

/* system includes */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

/* local includes */
#include "utils_version_id.h"
#include "utils_types.h"
#include "utils_hostdefs.h"
#include "utils_memory.h"
#include "utils_string.h"
#include "utils_debug.h"
#include "libUtilsVersion.h"
#include "libUtilsOptions.h"

extern utilsVersion * utils_print_version;

/*----------------------------------------------------------------------------*
*
* What: standardized print message utilities.
*
*----------------------------------------------------------------------------*/

extern void utilsPrintInfo(    char * prefix, char * format, ... );
extern void utilsPrintWarning( char * prefix, char * format, ... );
extern void utilsPrintError(   char * prefix, char * format, ... );
extern void utilsPrintFatal(   char * prefix, char * format, ... );
extern void utilsPrintInternalError(   char * prefix, char * format, ... );

/*----------------------------------------------------------------------------*
*
* What: setup selection of messages to output
*
*----------------------------------------------------------------------------*/

extern int utilsPrintSelectInfo(    char * selection );
extern int utilsPrintSelectWarning( char * selection );
extern int utilsPrintSelectError(   char * selection );

/*----------------------------------------------------------------------------*
*
* What: Error message returned by the system
* This routine must be called to complete an error message
* given to utilsPrintError one for example.
*
*----------------------------------------------------------------------------*/

extern char * utilsPrintSystemError( void );

#endif

