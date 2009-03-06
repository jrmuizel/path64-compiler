/*
 *      Copyright 2005, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 * STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 * Version Date 2005/Oct/19
 */

#ifndef __builtins_model_common_h
#define __builtins_model_common_h

#ifndef __SX

#include <stdio.h>

/* 32 bits manipulations */
#define EXT32(x, n) (((int)(x)<<(32-(n)))>>(32-(n)))
#define EXTU32(x, n) (((unsigned int)(x)<<(32-(n)))>>(32-(n)))
#define CLAMP32(x, n) (EXT32(x, n) == (int)(x) ? (x) : \
    -(int)1<<((n)-1) ^ -(int)((x) >= 0))
#define SAMESIGNS32(a,b)  (!(((int)(a)>>31) ^ ((int)(b)>>31)))


/* 64 bits manipulations */
#define EXT64(x, n) (((long long)(x)<<(64-(n)))>>(64-(n)))
#define EXTU64(x, n) (((unsigned long long)(x)<<(64-(n)))>>(64-(n)))
#define CLAMP64(x, n) (EXT64(x, n) == (long long)(x) ? (x) : \
    -(long long)1<<((n)-1) ^ -(long long)((x) >= 0))
#define SAMESIGNS64(a,b)  (!(((long long)(a)>>63) ^ ((long long)(b)>>63)))


/* 34 bits manipulations */
#define EXTU34(x, n) (((unsigned long long)(EXTU64(x, n))<<(33-(n)))>>(33-(n)))

/* Others Functions */

#define ABS(a)		( (a) < 0 ? -(a) : (a) )

#define abs(x) ABS(x)
#define llabs(x) ABS(x)

/* Carry Overflow management */

#define __PRINT_CARRY_OVERFLOW__  1

#if __PRINT_CARRY_OVERFLOW__
#define _PRINT_OVERFLOW printf("builtin_model: WARNING OVERFLOW detected \n")
#define _PRINT_CARRY printf("builtin_model: WARNING CARRY detected \n")
#else
#define _PRINT_OVERFLOW  0
#define _PRINT_CARRY 	 0
#endif

#define _isOVFLOW(x,y)	( ((x) != (y)) ? (_PRINT_OVERFLOW) : 0 )
#define _CARRY_POSITION	 32
#define _isCARRY(x)		( ( ( (unsigned long long) (x) << (_CARRY_POSITION-1) ) >> 63 ) ? (_PRINT_CARRY) : 0 )

#endif   /* __SX */

#endif	/* __builtins_model_common_h */
