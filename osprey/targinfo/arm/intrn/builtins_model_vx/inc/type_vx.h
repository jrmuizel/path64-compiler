/*
 *      Copyright 2006, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 * STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */

#ifndef _TYPE_VX_H_
#define _TYPE_VX_H_



// BUILTINS/CMODELS TYPE DEFINITIONS
#ifndef __INT128
#define __INT128
typedef struct int128 {
  long long high;
  long long low;
} INT128;
#endif 

#ifndef __FIR7RES
#define __FIR7RES
typedef struct {
  long long vk;
  long long dh_high;
  long long dh_low;
} fir7res;
#endif 

#ifndef __DOUBLEINT128
#define __DOUBLEINT128
typedef struct {
  long long dk_high;
  long long dk_low;
  long long dh_high;
  long long dh_low;
} doubleINT128;
#endif 

#endif
