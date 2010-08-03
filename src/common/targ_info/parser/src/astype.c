/*
 *      Copyright 2002-2004, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 *        STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */
#include "type_defs.h"
#include "air.h"
#include "parser.h"
#include "defs_exported.h"
#include "astype.h"

static s_String stringName ;

s_String *Char2String(char *name)
{
  stringName.buf = (char*) name ;
  stringName.length = strlen(name) ;
  return &stringName ;
}

uint32 String2Xint32(s_String string, uint16 base)
{
  uint32 retVal ;
  atoxi(string.buf, (INT*) &retVal, string.length, base) ;
  return retVal ;
}
