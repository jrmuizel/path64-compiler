
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
 * File : utils_debug.c
 *
 * Description : debug messages
............................................................................
*/ 
 
#include "utils_debug.h"

/*--------------------------------------------------------------------------------------*
*
* What for: answer TRUE if given prefix is one string defined in debug message selection
* string utils_option_print_select_debug
*
*---------------------------------------------------------------------------------------*/

int utilsDebugIsSelected( char * prefix ) {
  return ( ( utils_option_print_select_debug.value == NULL ) || utilsStringIsOneOf( utils_option_print_select_debug.value->argument, prefix ) );
}

/*--------------------------------------------------------------------------------------*
*
* What for: set debug message selection string
*
* Inputs:
*
* - char * selection: The selection string will be given as reference string to search
* given debug output prefix using utilsStringIsOneOf( ). Note that given string is
* duplicated.
*
* Returns:
*
* - TRUE if setting is OK (set to NULL or duplication success)
*
*---------------------------------------------------------------------------------------*/

int utilsDebugSelect( char * selection ) {
	return utilsOptionSet( & utils_option_print_select_debug, selection );
}

