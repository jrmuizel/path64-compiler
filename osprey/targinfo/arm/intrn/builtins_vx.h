
//============================================================================
//
//         Copyright 2000-2006, STMicroelectronics, Incorporated.
//                          All rights reserved.
//         STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
//
//  This software is supplied under the terms of a license agreement or
//  nondis closure agreement with STMicroelectronics and may not be copied or
//  disclosed except in accordance with the terms of that agreement.
//
//----------------------------------------------------------------------------
//
//  System           : STxP70 Toolset
//  Project Component: STxP70 Reconfiguration Toolkit
//  File Name        : builtins_vx.h
//  Purpose          : C compiler builtins
//
//----------------------------------------------------------------------------
//
//  Created on 2006/11/13 at 11:25:11
//
//============================================================================

#ifndef builtins_vx_h
#define builtins_vx_h

/*==================================================================*
 *
 * typedefs
 *
 *==================================================================*/

typedef int vx_DP  __attribute__ ((mode(vx_DP))) __attribute__ ((aligned(8)));

typedef int vx_DX __attribute__ ((mode(vx_DX))) __attribute__ ((aligned(8)));

typedef vx_DP vx_64;

typedef vx_DX vx_128;


/*------------------------------------------------------------------*
 *
 * User to compiler's builtins mapping of vx instructions
 *
 *------------------------------------------------------------------*/

#define _vx_PTR    "r"
#define _vx_GPR    "r"
#define _vx_CONST  "i"
#define _vx_IN     ""
#define _vx_OUT    "="
#define _vx_INOUT  "+"
#define _vx_MEMORY "memory"

#endif

//============================================================================
//
// END
//
//============================================================================
