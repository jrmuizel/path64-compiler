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
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "air.h"
#include "parser.h"
#include "type_defs.h"
#include "defs_exported.h"
#include "astype.h"
#include "analyzer.h"
#include "ext_engine.h"
#include "utils_print.h"
#include "alloc.h"
#include "opndstck.h"
#include "ctxstck.h"

#define _FOR_GAS

#if 1
#define _XOR_OPTIM
#endif

static s_encodingStack encStack ;

nodeSEM *STRASS_AXIOM ;
s_String inLine ;
char *line ;
uint32 lineSize ;
uint32 charIndex ;
uint32 errorIndex ;
uint32 maxIndex ;
extern nodeSEM NTR_LINE[] ;
extern int32 recordingMacro ;

e_ENCINFOTYPE __idlexType ;
void *__expr;
INT64 __opnd_value;
INT32 __is_reloc;
void *__symbol_id;
void *__symop_id;
INT32 __main_class;
INT32 __sub_class;
s_String __tokenString ;
int32 __exprSize ;

typedef struct {
  e_ASSLEX lexem ;
  char *lexString ;
} lexInfo ;

lexInfo lexInfoTable[] ={
  {LEX_add_sub_op, "- +"},
  {LEX_and_op, "&"},
  {LEX_cmp_op, "== <= >= < > ule uge ult ugt"},
  {LEX_mul_shift_op, "* % / << >> ushr rotr rotl"},
  {LEX_or_xor_op, "| ^"},
  {LEX_una_op, "~ -"},
  {LEX_v2_4_6, "2 4 6"},
  {LEX_pow2_3_16, "65536 32768 16384 8192 4096 2048 1024 512 256 128 64 32 16 8"}
} ;

s_encodingStack *GetEncStack()
{
  return &encStack ;
}

int IdentString(char *instring)
{
  int i = 0;

#ifdef PARSER_DEBUG
  printf("%s: instring = '%s', line = '%s', &line[charIndex] = '%s'\n",__FUNCTION__,instring,line,&line[charIndex]);
#endif

  while ((line[charIndex+i]!='\0')&&(toupper(instring[i]) == toupper(line[charIndex+i])))
    {
      i++ ;
    }
  if (instring[i] == '\0')
    {
      __tokenString.buf = &line[charIndex] ;
      __tokenString.length = i ;
      charIndex += i ;
      return 0;
    }
  else
    {
      return 1;
    }
}

void SkipSpace()
{
  while ((line[charIndex]==' ') || (line[charIndex]=='\t'))
    charIndex++;
}

int IdentSpace()
{
  int currIndex = charIndex ;
	
  SkipSpace() ;
  /* This mod might be unsafe !!! */
  if (currIndex == charIndex)
    return (line[currIndex] != '\0') ;
  return 0;
}


int IdentLabel()
{
  char *pline = &line[charIndex];
  char curChar ;

  curChar = *pline++ ;
  if ((curChar  == '.')||(curChar  == '_'))
    {
      curChar = *pline++ ;
      if ((curChar  == '.')||(curChar  == '_')||(isalnum((unsigned char)curChar)))
	{
	  curChar = *pline++ ;
	}
      else
	{
	  return 1;
	}
    }
  else if (isalpha((unsigned char)curChar))
    {
      curChar = *pline++ ;
    }
  else
    {
      return 1;
    }
  while ((curChar  == '.')||(curChar  == '_') || isalnum((unsigned char)curChar))
    {
      curChar = *pline++ ;
    }
  __tokenString.buf = &line[charIndex] ;
  __tokenString.length = pline-__tokenString.buf-1 ;	
  charIndex += __tokenString.length ;
  return 0 ;
}

int IdentIdent()
{
  int i = 0;

  /* same as label identification without "." */
  if ((line[charIndex+i] == '_')||(isalpha((unsigned char)line[charIndex+i])))
    {
      i++ ;
    }
  else
    {
      return 1;
    }
	
  while ((line[charIndex+i] == '_') || isalnum((unsigned char)line[charIndex+i]))
    {
      i++ ;
    }
  __tokenString.buf = &line[charIndex] ;
  __tokenString.length = i ;
  charIndex += i;
  return 0 ;
}

/* Recognition of syntax token:
 * [A-Za-z][A-Za-z]*
 * returns
 * 1: if it fails
 * 0: if is succeed, __tokenString.buf is updated accordingly
 */

int IdentAlpha()
{
  int i = 0;

  if (isalpha((unsigned char)line[charIndex+i]))
    {
      i++ ;
    }
  else
    {
      /* current char is not A-Za-z return with error */
      return 1;
    }
	
  while (isalpha((unsigned char)line[charIndex+i]))
    {
      i++ ;
    }
  __tokenString.buf = &line[charIndex] ;
  __tokenString.length = i ;
  charIndex += i;
  /* success */
  return 0 ;
}

int IdentParam()
{
  int i = 0;

	
  while ((line[charIndex+i] != ',') && (line[charIndex+i] != '\0'))
    {
      i++ ;
    }
  __tokenString.buf = &line[charIndex] ;
  __tokenString.length = i ;	
  charIndex += i;
  return !i ;
}


int IdentRange(unsigned int minVal, unsigned int maxVal)
{
  int currIndex = charIndex ;	
  unsigned int retVal = 0;
  int startIndex;
	
  __idlexType = DECCONST ;
  if (line[charIndex] == '0')
    {
      if ((line[charIndex+1] == 'x') || (line[charIndex+1] == 'X'))
	{
	  __idlexType = HEXCONST ;
	} 
      else  /* Decimal */
	{
	  if (minVal<=0)
	    {
	      /* only 0 is allowed as index, not 01 for example */
	      charIndex++;
	      __tokenString.buf = &line[currIndex] ;
	      __tokenString.length = charIndex - currIndex ;
	      return 0 ;
	    }
	  else
	    {
	      return 1 ;
	    }
	}
    }
  if (__idlexType == DECCONST) {
    while (isdigit((unsigned char)line[charIndex]))
      {
	retVal*=10;
	retVal+=((unsigned int) line[charIndex] - '0');
	if (retVal>maxVal)
	  return 1;
	charIndex++;
      }
  } 
  else 
    {
      /* HEXCONST */
      char *endptr;
      startIndex = charIndex;
      charIndex++;
      while (isxdigit((unsigned char)line[++charIndex])) { }

      retVal = strtol(&(line[startIndex]),&endptr,0);
      charIndex += endptr - &(line[startIndex]) + 1;

      if (retVal>maxVal)
	return 1;
      currIndex  = currIndex +2;  /* adjust for hex const parsing */

    }
  __tokenString.buf = &line[currIndex] ;
  __tokenString.length = charIndex - currIndex ;
  return ((retVal<minVal) || (currIndex == charIndex)) ;
}

int IdentIntegerConstant()
{
  int i;
	
  __idlexType = DECCONST ;
  if (line[charIndex] == '0')
    {
      charIndex++ ;
      switch (line[charIndex])
	{
	case 'x':
	case 'X':
	  charIndex++ ;
	  __idlexType = HEXCONST ;
	  break ;
	case 'b':
	case 'B':
	  charIndex++ ;
	  __idlexType = BINCONST ;
	  break ;
	default:
	  charIndex--;
	}
    }
  i = 0;
  switch (__idlexType)
    {
    case BINCONST:
      while ((line[charIndex+i]=='0') || (line[charIndex+i]=='1')) {
	i++;
      }
      break ;
    case DECCONST:
      while (isdigit((unsigned char)line[charIndex+i])) {
	i++;
      }
      break ;
    case HEXCONST:
      while (isxdigit((unsigned char)line[charIndex+i])) {
	i++;
      }
      break ;
    default:
      break; /* Shouldn't this be an internal error? */
    }
  __tokenString.buf = &line[charIndex] ;
  __tokenString.length = i ;
  charIndex += i ;
  return (i==0) ;
}

int IdentFloatConstant()
{
  int currIndex, startIndex ;
  int digitMandatory ;

  __idlexType = LABEL ;

  currIndex = charIndex ;
  startIndex = charIndex ;
  if (((line[charIndex]=='-') || (line[charIndex]=='+')) 
      && isdigit((unsigned char)line[charIndex+1]))
    {
      charIndex++ ;
    }
  while (isdigit((unsigned char)line[charIndex]))
    {
      charIndex++ ;
    }

  digitMandatory = (currIndex == charIndex) ;
  if (line[charIndex] == '.')
    {
      charIndex++ ;
      currIndex = charIndex ;
	
      while (isdigit((unsigned char)line[charIndex]))
	{
	  charIndex++ ;
	}
      if (digitMandatory&&(currIndex == charIndex))
	return 1;
    }

  if ((line[charIndex] == 'E') || (line[charIndex] == 'e'))
    {
      charIndex++ ;

      if ((line[charIndex] == '+') || (line[charIndex] == '-'))
	charIndex++ ;	
	
      currIndex = charIndex ;
      while (isdigit((unsigned char)line[charIndex]))
	{
	  charIndex++ ;
	}
    }
  if (currIndex != charIndex)
    {
      __tokenString.buf = &line[startIndex] ;
      __tokenString.length = charIndex - startIndex ;
      return 0;
    }
  return 1 ;
}


int IdentEOL()
{
  while (isspace((unsigned char)line[charIndex]))
    {
      charIndex++ ;
    }
  return (charIndex != maxIndex) ;
}

int  IdentQuotedString()
{
  char *pline ;
  int startIndex ;

  pline = &line[charIndex] ;
  if (*pline != '\"')
    return 1 ;
  pline++ ;
  charIndex++ ;
  startIndex = charIndex ; 	/* to skip '"' */
  while (*pline != '\0' && (*pline != '\"'))
    {
      if (*pline == '\\')
	{
	  if (pline[1] == '\"')
	    {
	      pline+= 2 ;
	      charIndex+= 2 ;
	    }				
	}
      pline++;
      charIndex++ ;
    }
  if (*pline == '\"')
    {
      __idlexType = LABEL ;
      __tokenString.buf = &line[startIndex] ;
      __tokenString.length = charIndex - startIndex ;		
      charIndex++ ;
      return 0 ;
    }
  return 1 ;

}

char *CopyStringToSpace (char *dst, char *src)
{
  while ((*src!=' ') && (*src!='\0')) {
    *dst++ = *src++ ;
  }
  *dst = '\0' ;
  if (*src == ' ')
    src++ ;
  return src ;
}

int IdentLexString(char *inString)
{
  char lexString[255] ;
  char *pNextTok ;
  int error ;
	
  /* be carefull inString should be ordered so that first matching string is
     the longuest */
  pNextTok = CopyStringToSpace (lexString, inString) ;
	
  while (lexString[0] != '\0')
    {
      PushContext() ;
      error = IdentString (lexString) ;
      PopContext() ;
      if (error) {
	RestoreContext() ;
      }
      else {
	return 0;
      }		
      pNextTok = CopyStringToSpace (lexString, pNextTok) ;
    }
  return 1;
}

int IdentLexFromTable(int lexType)
{
  int lexTableSize = (sizeof(lexInfoTable)/sizeof(&lexInfoTable[0])) ;
  int i;
	
  for (i=0; i<lexTableSize; i++)
    {
      if (lexInfoTable[i].lexem == lexType) {
	return IdentLexString(lexInfoTable[i].lexString) ;
      }
    }
  utilsPrintInternalError ("IE_000","No valid lexem found in LexTable") ;
  return 1;
}

void SkipToEOL()
{
  __idlexType = LABEL ;
  __tokenString.buf = &line[charIndex] ;
  __tokenString.length = maxIndex - 1 - charIndex ;		
  charIndex = maxIndex ;
}

int IdentLexem(int lexType)
{
  switch (lexType)
    {
    case LEX_label:
      return IdentLabel() ;
    case LEX_int_cst: 
      return IdentIntegerConstant() ;
    case LEX_float_cst:
      return IdentFloatConstant() ;
    case LEX_ident:
      return IdentIdent() ;
    case LEX_toeol:
      SkipToEOL() ;
      return 0 ;
    case LEX_param:
      return IdentParam() ;
    case LEX_quoted_string:
      return IdentQuotedString() ;
    case LEX_eol:
      return IdentEOL() ;
    case LEX_alpha:
      return IdentAlpha() ;
    }
  return IdentLexFromTable(lexType) ;
}

s_encodingInfo newEncInfo ;

int SyntaxAnalysis(int level, nodeSEM NS[], int nodeIndex)
{
  int error = 0;
  int error1 = 0;
  int error2 = 0;
  uint32 startChar ;
  int pushLexem ;
  s_contextElem context1, context2, baseContext ;
  int callAction ;

  pushLexem = 0;
  __exprSize = 0;
  __idlexType = LABEL ;
  __expr = 0;
  __opnd_value = 0;
  __is_reloc = 0;
  __symbol_id = 0;
  __symop_id = 0;
  __main_class = 0;
  __sub_class = 0;
	
  callAction = (NS[nodeIndex].encInfo != -1) ;

#ifdef PARSER_DEBUG
  printf("%s: state = %s\n",__FUNCTION__,PARSER_eState_Name[NS[nodeIndex].state]);
#endif
	
  switch (NS[nodeIndex].state)
    {
    case STA_OPT:
      PushContext() ;
#ifdef PARSER_DEBUG
      printf("%s: OPT: L%d: nodeIndex = %d, should go to %s\n",__FUNCTION__,level,nodeIndex,
	     PARSER_eState_Name[NS[NS[nodeIndex].info].state]);
#endif
      error = SyntaxAnalysis(level+1, NS, NS[nodeIndex].info) ;
      PopContext() ;
      if (error) {
	RestoreContext() ;
	if (callAction) {
	  newEncInfo.type = ENCODE ;
	  newEncInfo.action = NS[nodeIndex].encInfo ;
	  newEncInfo.numToken = NS[nodeIndex].semInfo ;
	  PushEncodingInfo (&encStack, &newEncInfo) ;
	}
	error = 0;
      }
      callAction = 0;
#ifdef PARSER_DEBUG
      printf("%s: OPT: L%d: error = %#x, line[charIndex] = %s\n",__FUNCTION__,level,error,&line[charIndex]);
#endif
      break ;		
    case STA_REP:
      if (charIndex!=maxIndex) {
	PushContext() ;
	SkipSpace() ;
	error = SyntaxAnalysis(level+1, NS, NS[nodeIndex].info) ;
	PopContext() ;
      }
      while (!error && (charIndex!=maxIndex)) {
	PushContext() ;
	SkipSpace() ;
	error = SyntaxAnalysis(level+1, NS, NS[nodeIndex].info) ;
	PopContext() ;
      }
      if(error)	{
	RestoreContext() ;
      }
      callAction = 0;
      error = 0;
      break ;
    case STA_XOR:
#ifdef _XOR_OPTIM
      PushContext() ;
      error = SyntaxAnalysis(level+1, NS, GETLEFTNODE(NS[nodeIndex]));
      PopContext() ;
      if (error) {
	RestoreContext() ;
	error = SyntaxAnalysis(level+1, NS, GETRIGHTNODE(NS[nodeIndex]));
      }
      break ;	
#endif
    case STA_OR:
      PushContext() ;
      baseContext = GetContext ();
#ifdef PARSER_DEBUG
      printf("%s: OR: L%d: nodeIndex = %d, should go to %s or %s\n",__FUNCTION__,level,nodeIndex,
	     PARSER_eState_Name[NS[GETLEFTNODE(NS[nodeIndex])].state],
	     PARSER_eState_Name[NS[GETRIGHTNODE(NS[nodeIndex])].state]);
#endif
#ifdef PARSER_DEBUG
      printf("%s: OR: L%d: Try 1 %s\n",__FUNCTION__,level,PARSER_eState_Name[NS[GETLEFTNODE(NS[nodeIndex])].state]);
#endif
      error1 = SyntaxAnalysis(level+1, NS, GETLEFTNODE(NS[nodeIndex]));
#ifdef PARSER_DEBUG
      printf("%s: OR: L%d: error1 = %d, line[charIndex] = %s\n",__FUNCTION__,level,error1,&line[charIndex]);
#endif
      context1 = GetContext (); 
      PopContext() ;
      RestoreContext() ;

      if (error1 == 0) {
	SetEncStackIndex (&encStack, context1.encStackIndex ) ;
      }

      PushContext() ;
#ifdef PARSER_DEBUG
      printf("%s: OR: L%d: Try 2 %s\n",__FUNCTION__,level,PARSER_eState_Name[NS[GETRIGHTNODE(NS[nodeIndex])].state]);
#endif
      error2 = SyntaxAnalysis(level+1, NS, GETRIGHTNODE(NS[nodeIndex]));
#ifdef PARSER_DEBUG
      printf("%s: OR: L%d: error2 = %d, line[charIndex] = %s\n",__FUNCTION__,level,error2,&line[charIndex]);
#endif
      context2 = GetContext ();
      PopContext() ;
      RestoreContext() ;

      if ((error1 == 0) && (error2 == 0)) {
	/* Two rules are matching, one might be the prefix of the other */
	
	if (context1.charIndex>context2.charIndex) {
	  /* we set the context to the longest matching context */
	  SetContext(context1) ;
	}
	else if (context2.charIndex>context1.charIndex) {
	  /* we set the context to the longest matching context */
	  SetContext(context2) ;
	  MoveEncodingInfo (&encStack,
			    baseContext.encStackIndex,
			    context1.encStackIndex,
			    context2.encStackIndex) ;
	}
	else {
	  uint32 context1Size ;
	  uint32 context2Size ;
	  
#ifdef PARSER_DEBUG
	  printf("%s: L%d: context1.charIndex == context2.charIndex (%d)\n",__FUNCTION__,level,context1.charIndex);
#endif
	  
	  context1Size = GetSumExprSizeInEncodingInfo(&encStack, baseContext.encStackIndex, context1.encStackIndex);
	  context2Size = GetSumExprSizeInEncodingInfo(&encStack, context1.encStackIndex, context2.encStackIndex);
	  
	  /* both matching contexts are the same length, that means  */
	  /* one of them might be a valid expression and the other a */
	  /* valid syntax token, in this case the valid syntax token */
	  /* should be choosen                                       */
	  /* example of ambuiguity :                                 */
	  /* (1) INST RegM, REgN, RegP				   */
	  /* (2) INST RegM, RegN, #cst                               */
	  /* #cst can be replaced by a valid expresion with RegP as  */
	  /* a variable name.                                        */
	  /* #cst = RegP should be considered as a valid expression  */
	  /* but syntax (1) should be recognized */
	  if ((context2Size == 0) && (context1Size == 0)) {
	    /* Error in generated parameter files */
	    utilsPrintInternalError ("IE_001","Duplicated syntax for line: %s", line) ;
	  }
	  else if (context1Size == 0) {
#ifdef PARSER_DEBUG
	    printf("%s: L%d: context1Size == 0, set context1\n",__FUNCTION__,level);
#endif
	    
	    /* context1 is a rule of the form (1) INST RegM, REgN, RegP */
	    /* context2 is a rule of the form (2) INST RegM, REgN, #cst */
	    /* So we choose context1 */
	    SetContext(context1) ;
	  }
	  else if (context2Size == 0) {
	    /* context2 is a rule of the form (1) INST RegM, REgN, RegP */
	    /* context1 is a rule of the form (2) INST RegM, REgN, #cst */
	    /* So we choose context2 */
#ifdef PARSER_DEBUG
	    printf("%s: L%d: context2Size == 0, set context2\n",__FUNCTION__,level);
#endif
	    SetContext(context2) ;
	    MoveEncodingInfo (&encStack,
			      baseContext.encStackIndex,
			      context1.encStackIndex,
			      context2.encStackIndex) ;
	  }
	  else {
	    /* context1 is a rule of the form (1) INST RegM, REgN, #cst */
	    /* context2 is a rule of the form (2) INST RegM, REgN, #cst */
	    /* we choose the one with bigger cst                        */
	    /* It happens for ST1xx with rules like:                    */
	    /* ADDA Pm, P13, #u15                                       */
	    /* ADDA Pm, Pn, #u5                                         */
	    /* where Pn can be P13                                      */
	    
	    if (context1Size > context2Size) {
#ifdef PARSER_DEBUG
	      printf("%s: L%d: context1Size > context2Size, set context1\n",__FUNCTION__,level);
#endif
	      SetContext(context1) ;
	    }
	    else {
#ifdef PARSER_DEBUG
	      printf("%s: L%d: context2Size >= context1Size, set context2\n",__FUNCTION__,level);
#endif
	      SetContext(context2) ;
	      MoveEncodingInfo (&encStack,
				baseContext.encStackIndex,
				context1.encStackIndex,
				context2.encStackIndex) ;
	    }
	  }
	}
      }
      else {
	if (error1 == 0) {
	  SetContext(context1) ;
	}
	if (error2 == 0) {
	  SetContext(context2) ;
	}
      }
      error = error1 & error2;
      break ;
    case STA_AND:
#ifdef PARSER_DEBUG
      printf("%s: AND: L%d: nodeIndex = %d, should go to %s and %s\n",__FUNCTION__,level,nodeIndex,
	     PARSER_eState_Name[NS[GETLEFTNODE(NS[nodeIndex])].state],
	     PARSER_eState_Name[NS[GETRIGHTNODE(NS[nodeIndex])].state]);
#endif
#ifdef PARSER_DEBUG
      printf("%s: AND: L%d: Try 1 %s\n",__FUNCTION__,level,PARSER_eState_Name[NS[GETLEFTNODE(NS[nodeIndex])].state]);
#endif
      error = SyntaxAnalysis(level+1, NS, GETLEFTNODE(NS[nodeIndex]));
#ifdef PARSER_DEBUG
      printf("%s: AND: L%d: error = %d, line[charIndex] = %s\n",__FUNCTION__,level,error,&line[charIndex]);
#endif
      if (!error) {
	/* Skip 0 to N white spaces */
	SkipSpace(); 
#ifdef PARSER_DEBUG
	printf("%s: AND: L%d: Try 1 %s\n",__FUNCTION__,level,PARSER_eState_Name[NS[GETRIGHTNODE(NS[nodeIndex])].state]);
#endif
	error = SyntaxAnalysis(level+1, NS, GETRIGHTNODE(NS[nodeIndex]));
#ifdef PARSER_DEBUG
	printf("%s: AND: L%d: error = %d, line[charIndex] = %s\n",__FUNCTION__,level,error,&line[charIndex]);
#endif
      }
      break ;
    case STA_CON:
      error = SyntaxAnalysis(level+1, NS, GETLEFTNODE(NS[nodeIndex]));
      if (!error) {
	error = SyntaxAnalysis(level+1, NS, GETRIGHTNODE(NS[nodeIndex]));
      }
      break ;
    case STA_SPA:
      error = SyntaxAnalysis(level+1, NS, GETLEFTNODE(NS[nodeIndex]));
      if (!error) {
	error = IdentSpace();
      }
      if (!error) {
	error = SyntaxAnalysis(level+1, NS, GETRIGHTNODE(NS[nodeIndex]));
      }
      break ;
    case STA_TER:
#ifdef PARSER_DEBUG
      printf("%s: TER: L%d: %s\n",__FUNCTION__,level,stringTable_Glb[NS[nodeIndex].info]);
#endif
      error = IdentString(stringTable_Glb[NS[nodeIndex].info]) ;
#ifdef PARSER_DEBUG
      printf("%s: TER: L%d: %s -> error = %d\n",__FUNCTION__,level,stringTable_Glb[NS[nodeIndex].info],error);
#endif
      break ;
    case STA_RNG:
      error = IdentRange(GETMINVAL(NS[nodeIndex]), GETMAXVAL(NS[nodeIndex])) ;
      pushLexem = !error ;
      break ;
    case STA_UCE: {
      char *endptr;
      PARSER_ReturnT ret_error;
      ISA_LIT_CLASS lclass;
      startChar = charIndex ;
#ifdef PARSER_DEBUG
      printf("%s: UCE: %s\n",__FUNCTION__,stringTable_Glb[NS[nodeIndex].info]);
#endif
      currParserInstance->get_lit_class(stringTable_Glb[NS[nodeIndex].info],&lclass);
      if(lclass == ISA_LC_UNDEFINED) {
	utilsPrintInternalError("IE_010","Undefined literal class for '%s'",stringTable_Glb[NS[nodeIndex].info]) ;
      }
      ret_error = PARSER_parse_expression(&(line[GetCharIndex()]), &endptr,lclass,&__is_reloc,
				       &__symbol_id,&__symop_id,&__opnd_value,&__expr);
      __main_class = lclass;
      __sub_class = 0;
      error = (ret_error == PARSER_OK) ? 0 : 1;
      __tokenString.buf = &line[charIndex];
      __tokenString.length = endptr - __tokenString.buf;		
      __exprSize = -NS[nodeIndex].info;
      if(error == 0) {
	SetCharIndex(charIndex + __tokenString.length);
      }
      pushLexem = !error ;
#ifdef PARSER_DEBUG
      printf("%s: error = %d\n",__FUNCTION__,error);
#endif
#if 0
      error = SyntaxAnalysis(level+1, NTR_expr_Glb, 0);
      __tokenString.buf = &line[startChar] ;
      __tokenString.length = charIndex - startChar - 1;		
      __exprSize = NS[nodeIndex].info;
      pushLexem = !error ;
#endif
      __idlexType = CSTEXPR ;
      break ;		
    }
    case STA_SCE: {
      char *endptr;
      PARSER_ReturnT ret_error;
      ISA_LIT_CLASS lclass;
      startChar = charIndex ;		
      currParserInstance->get_lit_class(stringTable_Glb[NS[nodeIndex].info],&lclass);
      if(lclass == ISA_LC_UNDEFINED) {
	utilsPrintInternalError("IE_011","Undefined literal class for '%s'",stringTable_Glb[NS[nodeIndex].info]) ;
      }
      ret_error = PARSER_parse_expression(&(line[GetCharIndex()]), &endptr,lclass,&__is_reloc,
				       &__symbol_id,&__symop_id,&__opnd_value,&__expr);
      __main_class = lclass;
      __sub_class = 0;
      error = (ret_error == PARSER_OK) ? 0 : 1;
      __tokenString.buf = &line[charIndex];
      __tokenString.length = endptr - __tokenString.buf;		
      __exprSize = -NS[nodeIndex].info;
      if(error == 0) {
	SetCharIndex(charIndex + __tokenString.length);
      }
      pushLexem = !error ;
#if 0
      error = SyntaxAnalysis(level+1, NTR_expr_Glb, 0);
      __tokenString.buf = &line[startChar] ;
      __tokenString.length = charIndex - startChar - 1 ;		
      __exprSize = -NS[nodeIndex].info;
      pushLexem = !error ;
#endif
      __idlexType = CSTEXPR ;
      break ;		
    }
    case STA_NTR:
#ifdef PARSER_DEBUG
      printf("%s: NTR: L%d: %d\n",__FUNCTION__,level,NS[nodeIndex].info);
#endif
      error = SyntaxAnalysis(level+1, semTable_Glb[NS[nodeIndex].info], 0) ;
#ifdef PARSER_DEBUG
      printf("%s: NTR: L%d: %d -> error = %d\n",__FUNCTION__,level,NS[nodeIndex].info,error);
#endif
      break ;
#ifdef _FOR_GAS
    case STA_EXP: {
      char *endptr;
      PARSER_ReturnT ret_error;
      ISA_REGISTER_CLASS rclass;
      ISA_REGISTER_SUBCLASS rsclass;
      INT32 regid;
#ifdef PARSER_DEBUG
      printf("%s: EXP: %s\n",__FUNCTION__,stringTable_Glb[NS[nodeIndex].info]);
#endif
      currParserInstance->get_reg_class(stringTable_Glb[NS[nodeIndex].info],&rclass,&rsclass);
      if(rclass == ISA_REGISTER_CLASS_UNDEFINED) {
	utilsPrintInternalError("IE_009","Undefined register class for '%s'",stringTable_Glb[NS[nodeIndex].info]) ;
      }
      ret_error = PARSER_parse_register(&(line[GetCharIndex()]),&endptr,rclass,rsclass,&regid);
      __opnd_value = regid;
      __main_class = rclass;
      __sub_class = rsclass;
      error = (ret_error == PARSER_OK) ? 0 : 1;
      __tokenString.buf = &line[charIndex];
      __tokenString.length = endptr - __tokenString.buf ;		
      __exprSize = -NS[nodeIndex].info;
      if(error == 0) {
	SetCharIndex(charIndex + __tokenString.length);
      }
      __idlexType = REGISTER;
      pushLexem = !error ;
#if 0
      error = SyntaxAnalysis(level+1, semTable_Glb[NS[nodeIndex].info], 0) ;
#endif
      break ;
    }
#endif
    case STA_LEX:
      error = IdentLexem(NS[nodeIndex].info) ;
      pushLexem = !error ;
      break;
    default:
      utilsPrintInternalError("IE_002","Invalid state in SyntaxAnalysis") ;
    }

  if (error) {
    if (errorIndex<charIndex)
      errorIndex = charIndex ;		
  }

  callAction &= ~error ;
#ifdef PARSER_DEBUG
  printf("%s: L%d: error = %d\n",__FUNCTION__,level,error);
#endif
	
  if (pushLexem) {
#ifdef PARSER_DEBUG
    printf("%s: pushLexem at level %d\n",__FUNCTION__,level);
#endif
    newEncInfo.type = __idlexType ;
    newEncInfo.string = __tokenString ;
    newEncInfo.exprSize = __exprSize ;
    newEncInfo.is_reloc = __is_reloc;
    newEncInfo.sym_id = __symbol_id;
    newEncInfo.symop_id = __symop_id;
    newEncInfo.opnd = __opnd_value;
    newEncInfo.expr = __expr;
    newEncInfo.main_class = __main_class;
    newEncInfo.sub_class = __sub_class;
    PushEncodingInfo (&encStack, &newEncInfo) ;
  }

  if (callAction) {
#ifdef PARSER_DEBUG
    printf("%s: callAction at level %d\n",__FUNCTION__,level);
#endif
    newEncInfo.type = ENCODE ;
    newEncInfo.action = NS[nodeIndex].encInfo ;
    newEncInfo.numToken = NS[nodeIndex].semInfo ;
    PushEncodingInfo (&encStack, &newEncInfo) ;
  }

  return error ;	
}

static char *cStartComment = "/*" ;
static char *cEndComment = "*/" ;
static char* cppStartComment = "//" ;

extern nodeSEM NTR_LABEL_DEF[] ;
/*extern nodeSEM NTR_PREPROC_LINE[] ; */
/*extern nodeSEM NTR_MACRO_EXP[] ;    */
extern nodeSEM NTR_DIRECT_LINE[] ;
/*extern nodeSEM NTR_CONTROL_DIR[] ;  */
/*extern nodeSEM NTR_LOOP_LINE[] ;    */

/* return index from previous token in current line *
 * it looks for a string that is before pos         *
 * return index for fist character of this string   */

uint32 PrevTokenIndex(uint32 pos)
{
  while ((pos>=0)&&isspace((unsigned char)line[pos]))
    pos-- ;
  while ((pos>=0)&&!isspace((unsigned char)line[pos]))
    pos-- ;
  return pos+1 ;
}

void CopyStringToLine(char *buffer)
{
	int c ;
	char *pCPPcomment ;
	
	maxIndex = 0 ;
	c = *buffer++;
	pCPPcomment = cppStartComment ;
	while ((c != '\0') && (*pCPPcomment != '\0'))
	{
		if (c == *pCPPcomment)
			pCPPcomment++ ;
		else
			pCPPcomment = cppStartComment ;
		line[maxIndex++] = c;
		c = *buffer++;
		if (maxIndex>=lineSize)
		{
			lineSize += BLK_SIZE ;
			line = (char *) Realloc (line, lineSize);
		}
	}
	
	if (*pCPPcomment == '\0')
	{			
		maxIndex -= (pCPPcomment - cppStartComment)  ;
		while (c != '\0')
			c = *buffer++;
	}
	
	line[maxIndex] = '\0' ;
	if (maxIndex)
	{
		if (line[maxIndex-1] == '\r')
		{
			line[--maxIndex] = '\0' ;
		}
	}
}

int AnalyzeString(char *buffer)
  {
    int failed ;

    charIndex = 0;
    errorIndex = 0;
    line = (char *)inLine.buf;
    lineSize = inLine.length ;
    InitEncodingStack(&encStack) ;
    InitContextStack(&encStack) ;
    CopyStringToLine(buffer) ;
    SkipSpace();
    failed = SyntaxAnalysis(0,STRASS_AXIOM, 0);
    if (!failed)
      failed = IdentEOL() ;
    return failed ;
  }

/* Return index of error in anaylyzed string to print error message. */
uint32 GetErrorIndex() {
  return errorIndex;
}

void InitializeAnalyzer() {
  inLine.buf = (char*) Malloc (BLK_SIZE) ;
  inLine.length = BLK_SIZE ;
  STRASS_AXIOM = NULL ;
}

void FinalizeAnalyzer() {
  free(inLine.buf);
  inLine.buf = NULL;
  inLine.length = 0;
  STRASS_AXIOM = NULL ;
}

void CheckAxiom() {
  if (STRASS_AXIOM == NULL)
    utilsPrintInternalError ("IE_006","No valid value for AXIOM\n") ;
}
