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
 * File : basic_types.h
............................................................................
*/

#ifndef utils_types_h
#define utils_types_h

/*----------------------------------------------------------------------------*
*
* C-style booleans
*
*----------------------------------------------------------------------------*/

#ifndef FALSE
#define FALSE ( 0 != 0 )
#endif

#ifndef TRUE
#define TRUE ( 0 == 0 )
#endif

/*----------------------------------------------------------------------------*
*
* Host adaptation of sized integers
*
*----------------------------------------------------------------------------*/

#include <stdint.h>

#if defined __sun
typedef signed char        sint_8;
#else
typedef int8_t             sint_8;
#endif

typedef uint8_t            uint_8;
typedef int16_t            sint_16;
typedef uint16_t           uint_16;
typedef int32_t            sint_32;
typedef uint32_t           uint_32;
typedef int64_t            sint_64;
typedef uint64_t           uint_64;

#endif /* utils_types_h */
