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

#if defined __sun

typedef signed char        sint_8;
typedef unsigned char      uint_8;

typedef short              sint_16;
typedef unsigned short     uint_16;

typedef int                sint_32;
typedef unsigned int       uint_32;

typedef long long          sint_64;
typedef unsigned long long uint_64;

#elif (defined __linux__)

typedef signed char        sint_8;
typedef unsigned char      uint_8;

typedef short              sint_16;
typedef unsigned short     uint_16;

typedef int                sint_32;
typedef unsigned int       uint_32;

typedef long long          sint_64;
typedef unsigned long long uint_64;

#elif (defined __CYGWIN__)

typedef signed char        sint_8;
typedef unsigned char      uint_8;

typedef short              sint_16;
typedef unsigned short     uint_16;

typedef int                sint_32;
typedef unsigned int       uint_32;

typedef long long          sint_64;
typedef unsigned long long uint_64;

#elif (defined WIN32)

typedef signed char        sint_8;
typedef unsigned char      uint_8;

typedef short              sint_16;
typedef unsigned short     uint_16;

typedef int                sint_32;
typedef unsigned int       uint_32;

typedef __int64            sint_64;
typedef unsigned __int64   uint_64;

#else

#error "Cannot guess OS"

#endif

#endif /* utils_types_h */
