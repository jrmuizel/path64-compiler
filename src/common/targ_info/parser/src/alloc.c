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
#include "alloc.h"
#include "utils_print.h"
#include <stdlib.h>


void *Malloc(size_t size)
{
  void *newElem ;

  newElem = malloc(size) ;
  if (newElem == NULL)
    {
      utilsPrintFatal("FE_001","Can't allocate memory");
    }
  return newElem ;
}

void *Realloc(void *ptr, size_t size)
{
  void *newElem ;

  newElem = realloc (ptr, size) ;
  if (newElem == NULL)
    {
      utilsPrintFatal("FE_002","Can't reallocate memory");
    }
  return newElem ;
}
