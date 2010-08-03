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
#include <stdio.h>

#include "type_defs.h"
#include "air.h"
#include "parser.h"
#include "defs_exported.h"
#include "astype.h"
#include "opndstck.h"
#include "ctxstck.h"
#include "analyzer.h"
#include "utils_print.h"

#define _FOR_GAS

s_contextStack context ;
s_contextElem contextToRestore ;

void InitContextStack(s_encodingStack *encStackPtr)
{
  context.size = 0 ;
  context.encStackPtr = encStackPtr ;
}

void PushContext()
{
  int size = context.size ;
	
  if (size<CONTEXT_MAX_SIZE)
    {
      context.stack[size].charIndex = GetCharIndex () ;
      context.stack[size].encStackIndex = GetEncStackIndex (context.encStackPtr) ;
      context.stack[size].exprSize = GetExprSize () ;
      context.size++ ;
    }
  else
    {
      utilsPrintInternalError("IE_007","Stack full in PushContext") ;
    }		
}

void PopContext()
{
  if (context.size>0)
    {
      --context.size ;
      contextToRestore.charIndex = context.stack[context.size].charIndex ;
      contextToRestore.encStackIndex = context.stack[context.size].encStackIndex ;
      contextToRestore.exprSize = context.stack[context.size].exprSize ;
    }
  else
    {
      utilsPrintInternalError("IE_008","Stack empty in PopContext") ;
    }		
}

void RestoreContext()
{
  SetCharIndex (contextToRestore.charIndex) ;
  SetEncStackIndex (context.encStackPtr, contextToRestore.encStackIndex) ;
  SetExprSize (contextToRestore.exprSize) ;
}


s_contextElem GetContext()
{
  s_contextElem currContext ;
	
  currContext.charIndex = GetCharIndex () ;
  currContext.encStackIndex = GetEncStackIndex (context.encStackPtr) ;
  currContext.exprSize = GetExprSize () ;
  return currContext ;
}

void SetContext(s_contextElem currContext)
{
  SetCharIndex (currContext.charIndex) ;
  SetEncStackIndex (context.encStackPtr, currContext.encStackIndex) ;
  SetExprSize (currContext.exprSize) ;
}
