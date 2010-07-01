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
#include <string.h>

#include "air.h"
#include "parser.h"
#include "type_defs.h"
#include "defs_exported.h"
#include "astype.h"
#include "opndstck.h"
#include "analyzer.h"

#include "ext_engine.h"

char *ENCINFO_Name[] = {
  "ENCODE",
  "BINCONST",
  "DECCONST",
  "HEXCONST",
  "LABEL",
  "CSTEXPR",
  "REGISTER",
};

void InitEncodingStack(s_encodingStack *stack)
{
	stack->size = 0 ;
}

void DecrEncodingStack(s_encodingStack *stack, int decr)
{
	stack->size -= decr ;
	if (stack->size<0)
	  utilsPrintInternalError("IE007","Negative encoding stack index in DecrEncodingStack\n") ;
#ifdef PARSER_DEBUG
	printf ("%s: New stack->size = %d\n",__FUNCTION__,stack->size) ;
#endif

}

void DisplayTypedInfo(s_encodingInfo *sEncInfo)
{
	char label[255] ;
	
	switch (sEncInfo->type)
	{
		case LABEL:
			strncpy (label, sEncInfo->string.buf, sEncInfo->string.length) ;
			label[sEncInfo->string.length] = '\0' ;
			printf ("Label(\"%s\")\n", label) ;
			break ;
		case HEXCONST:
			strncpy (label, sEncInfo->string.buf, sEncInfo->string.length) ;
			label[sEncInfo->string.length] = '\0' ;
			printf ("Hexa Constant(%s)\n", label) ;
			break ;
		case DECCONST:
			strncpy (label, sEncInfo->string.buf, sEncInfo->string.length) ;
			label[sEncInfo->string.length] = '\0' ;
			printf ("Decimal Constant(%s)\n", label) ;
			break ;
		case BINCONST:
			strncpy (label, sEncInfo->string.buf, sEncInfo->string.length) ;
			label[sEncInfo->string.length] = '\0' ;
			printf ("Binary Constant(%s)\n", label) ;
			break ;
		case ENCODE:
			printf ("STRASSEncode(%d)\n", sEncInfo->action) ;
			break ;
		case CSTEXPR:
			printf ("Expr(%d)\n", sEncInfo->exprSize) ;
			break ;
		case REGISTER:
			printf ("Reg(%d)\n", sEncInfo->exprSize) ;
			break ;
	}
}


void DisplayEncodingInfo()
{
	s_encodingStack *encStack ;
	encStack = GetEncStack() ;
	if (encStack->size) {
		DisplayTypedInfo(PopEncodingInfo(encStack)) ;
	}
	while (encStack->size) {
		DisplayTypedInfo(PopEncodingInfo(encStack)) ;
	}
	printf ("\n") ;
}

void PushEncodingInfo(s_encodingStack *stack, s_encodingInfo *newInfo)
{
  int size ;
  
  size = stack->size ;
#ifdef PARSER_DEBUG
  printf("%s: Begin: type = %s, stack->size = %d\n",__FUNCTION__,ENCINFO_Name[newInfo->type], stack->size);
#endif

  if (size<ENCODING_MAX_SIZE) {
    stack->array[size] = *newInfo ;
    stack->size++ ;
  }
  else {
    utilsPrintInternalError("IE008","Stack full in PushEncodingInfo\n") ;
  }		
#ifdef PARSER_DEBUG
  printf("%s: End: type = %s, stack->size = %d\n",__FUNCTION__,ENCINFO_Name[newInfo->type], stack->size);
#endif
}

s_encodingInfo *PopEncodingInfo(s_encodingStack *stack)
{
  s_encodingInfo *retVal=0 ;
	
  if (stack->size>0) {
    retVal = &stack->array[--stack->size] ;
  }
  else {
    utilsPrintInternalError("IE009","Stack empty in PopEncodingInfo\n") ;
  }
#ifdef PARSER_DEBUG
  printf("%s: type = %s\n",__FUNCTION__,ENCINFO_Name[retVal->type]);
#endif
  return retVal ;
}

uint32 GetSumExprSizeInEncodingInfo(s_encodingStack *stack, int stackStart, int stackEnd)
{
	int i ;
	uint32 sum ;

	sum = 0;
	for (i=stackStart; i<stackEnd; i++)
	{
		if (stack->array[i].type == CSTEXPR)
		{
			if (stack->array[i].exprSize <0)
				sum -= stack->array[i].exprSize ;
			else
				sum += stack->array[i].exprSize ;
		}
	}
	return sum ;
}

void MoveEncodingInfo(s_encodingStack *stack, int destElem, int startElem, int endElem)
{
	int i ;

#ifdef PARSER_DEBUG
	printf("%s: start = %d, end = %d, dest from = %d\n",__FUNCTION__,startElem,endElem,destElem);
#endif

	for (i=startElem ; i<endElem; i++)
	{
		stack->array[destElem++] = stack->array[i] ;
	}
	stack->size = destElem ;
}

int GetEncStackIndex (s_encodingStack *stack)
{
	return stack->size ;
}

void SetEncStackIndex (s_encodingStack *stack, int encStackIndex)
{
  if ((encStackIndex < 0) || (encStackIndex>=ENCODING_MAX_SIZE))
    utilsPrintInternalError("IE010","Invalid stack index restoration in SetEncStackIndex\n") ;
  stack->size = encStackIndex ;
#ifdef PARSER_DEBUG
  printf("%s: index = %d\n",__FUNCTION__,encStackIndex);
#endif

}

s_encodingInfo *GetEncInfoIndex(const char *file, int line, s_encodingStack *stack, int index)
{
  if ((index < 0) || (index>=stack->size))
    PARSER_fatal_error("%s:%d Invalid stack index access in GetEncInfoIndex (0<=%d<%d)\n",
		       file, line, index,stack->size) ;
  return &stack->array[index] ;
}
