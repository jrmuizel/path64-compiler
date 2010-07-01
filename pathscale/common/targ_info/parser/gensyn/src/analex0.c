#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "libUtils.h"

#include "gensyn_version.h"
#include "gensyn_option.h"
#include "gensyn_error.h"

#include "unilex.h"
#include "gensyn_defs.h"
#include "table.h"

#define _FOR_GAS

#if 0
#define DEBUG_PRINTF(fmt,str) printf (fmt,str)
#else
#define DEBUG_PRINTF(fmt,str)
#endif

ATOME dern_atom;
char NomLu[MAX_SEM_LEN];
FILE *inputStream ;

static int curChar;
/*static int getIdentMode = 0;*/

typedef struct {
  ATOME atom;
  char *keyword;
} s_keyWord;

static s_keyWord keyWordTable [] = {
  {FORMAT, "Format"},
  {SYNKEYWORD, "Sy"},
  {ACTKEYWORD, "Act"},
  {DESKEYWORD, "De"},
  {GROUPING, "IGroup"},
  {ELF, "Elf"},
  {INIT, "Init"},
  {RELOC, "Reloc"},
  {CLASS, "IClass"},
  {ROOT, "Root"},
  {GLBDEC, "GLOBDECL"},
  {PORT, "Port"},
  {MULTIPLY, "Multiply"}
};

/* Just ignores comment in syntax file entry */
int GetComment (void) {
  int oneChar;
  int lineComment;
   
  if (curChar=='/') {
    oneChar = curChar;
  } else {
    return 0;
  }
  curChar=fgetc(inputStream);
  if (curChar=='*') {
    lineComment = 0;
  } else if (curChar=='/') {
    lineComment = 1;
  } else {
    ungetc(curChar, inputStream);
    curChar = oneChar;
    return 0;
  }
  NomLu[0]='\0';
  if (lineComment==1) {
    DEBUG_PRINTF("%s","//");
    /* Manage C++ like comments: ignore all until eol */
    curChar=fgetc(inputStream);
    while (curChar!='\n') {
      DEBUG_PRINTF("%c",curChar);
      curChar=fgetc(inputStream);
    }
    lineNo++;
    curChar=fgetc(inputStream);
  } else {
    DEBUG_PRINTF("%s","/*");
    /* Manage C like comments: ignore all until end sequence */
    curChar=fgetc(inputStream);
    DEBUG_PRINTF("%c",curChar);
    oneChar=fgetc(inputStream);
    while ((curChar!='*') || (oneChar!='/')) {
      if (curChar=='\n') lineNo++;
      curChar=oneChar;
      DEBUG_PRINTF("%c",curChar);
      oneChar=fgetc(inputStream);
    }
    curChar=fgetc(inputStream);
  }
  return 1;
}

void GetIdent() {
  int numChar=0;

  NomLu[0] = '\0' ;
  if (isalpha (curChar)||(curChar=='_')) {
    NomLu[numChar++]=curChar;
  }
  else {
    return ;
  }
  curChar=fgetc(inputStream) ;
  while (isalnum(curChar) || (curChar=='_')) {
    if (numChar<MAX_SEM_LEN-1) {
      NomLu[numChar++]=curChar;
    }
    else {
      /* Fatal Error if identicator is too long */
      NomLu[numChar]='\0';
      utilsPrintFatal(GENSYN_ERROR_TRUNCIDF,NomLu);
    }
    curChar=fgetc(inputStream) ;
  }
  NomLu[numChar]='\0';
  ungetc(curChar, inputStream) ;	
}

/* return 1 if current lexem is an identificator                *
 * return 0 else                                                *  
 * identificator is defined as :                                *
 * ['a'-'z'|'A'-'Z'|_]['a'-'z'|'A'-'Z'|'0'-'9'|'_']*            */
 
static int IsIdent () {
  GetIdent() ;
  return (NomLu[0] != '\0') ;
}

int arraySep[] = {'<', '\n', EOF, ' ', '\t', '[', ']' } ;

/* return separator index + 1 if current char Is a separator as defined in arraySep *
 * return 0 else                                                                    */

static int IsSeparator () {
  int i;
  int numSep = sizeof(arraySep)/sizeof(arraySep[0]) ;
	
  for (i=0; i<numSep; i++) {
    if (curChar == arraySep[i])
      return i+1;
  }
  return 0;
}

/* interpret current lexem as a numeric string        *
 * return 1 if lexm is not empty and last char is '>' *
 * return 0 else                                      */

void GetNumString () {
  int numChar=0;

  while (isdigit(curChar)) {
    NomLu[numChar++]=curChar;
    curChar=fgetc(inputStream) ;
  }
  NomLu[numChar]='\0';
  ungetc (curChar, inputStream) ;
}	

static int GetTermString () {
  int numChar=0;

  while (IsSeparator() == 0) {
    if (curChar == '\\') {
      curChar = fgetc(inputStream) ;
    }
    if (numChar<MAX_SEM_LEN-1) {
      NomLu[numChar++]=curChar;
    }
    curChar = fgetc(inputStream) ;
  }
  ungetc(curChar, inputStream) ;
  NomLu[numChar]='\0';

  return numChar;
}

static ATOME IdentKeyWord () {
  int keyWordNum, ind;

  keyWordNum = sizeof(keyWordTable)/sizeof(keyWordTable[0]);
  for (ind=0; ind<keyWordNum; ind++) {
    if (!strcasecmp (keyWordTable[ind].keyword, NomLu)) {
      DEBUG_PRINTF("%s",keyWordTable[ind].keyword) ;
      return  keyWordTable[ind].atom;
    }
  }
  utilsPrintError(GENSYN_ERROR_INVALIDKEYWORD,NomLu);
  return LEXERR;
}

ATOME GetLeftLex(int skipEOL) {
  int identKeyword = 0;

  curChar=fgetc(inputStream) ;
  while (isspace(curChar) || GetComment()) {
    if (curChar == '\n') {
      lineNo++ ;
      if (!skipEOL)
	return LINEN ;
    }
    curChar=fgetc(inputStream) ;
  }
  switch (curChar) {
  case ':':
    DEBUG_PRINTF("%s",":") ;
    return COLUMN ;
  case ',':
    DEBUG_PRINTF("%s",",") ;
    return COMMA ;
  case '*':
    DEBUG_PRINTF("%s","*") ;
    return STAR ;
  case EOF:
    DEBUG_PRINTF("%s","<EOF>") ;
    return FILEN ;
  case '{': 
    DEBUG_PRINTF("%s","{") ;
    return OPENACC;
  case '}': 
    DEBUG_PRINTF("%s","}") ;
    return CLOSACC;
  case '.':
    DEBUG_PRINTF("%s",".") ;
    curChar=fgetc(inputStream) ;
    identKeyword = 1;
    break ;
  }
  if (IsIdent ()) {
    if (identKeyword)
      return IdentKeyWord() ;
    return IDNTER;
  }
	
  if (isdigit(curChar)) {
    GetNumString() ;
    return NUMVAL ;
  }
  utilsPrintError(GENSYN_ERROR_INVALIDIDF,curChar);
  return LEXERR;	
}

/* fonction de lecture de l'ATOME (lexeme) en entree */

ATOME GetRightLex () {
  int isSPACE ;
  int identAction = 0;
  int getExpression = 0;
  unsigned int maxVal, minVal ;

  curChar=fgetc(inputStream) ;
  if (curChar == EOF)
    return FILEN ;

  switch (curChar) {
  case '[': 
    return OPENBRA;
  case ']': 
    return CLOSBRA;
  case '\n':
    lineNo++;
    return LINEN;
  case '<':
    identAction = 1;
    curChar=fgetc(inputStream) ;	    
    break ;			
  }

  if (identAction) {
    ATOME retLex ;
    retLex = LEXERR ;
    
    switch (curChar) {
    case '#':
      curChar=fgetc(inputStream) ;
#ifdef _FOR_GAS
      retLex = UCSTEXP ;
      GetIdent();
      break;
#else
      switch (tolower(curChar))	{
      case 'r':
	/* Range between a and b is expected here */
	curChar=fgetc(inputStream) ;
	GetNumString() ;
	if (NomLu[0]=='\0') {
	  retLex = LEXERR ;
	  break ;
	}
	minVal = atoi (NomLu) ;
	curChar=fgetc(inputStream) ;
	if (curChar == ':') {
	  curChar=fgetc(inputStream) ;
	  GetNumString() ;
	}
	if (NomLu[0]=='\0') {
	  retLex = LEXERR ;
	  break ;
	}
	maxVal = atoi (NomLu) ;
	sprintf (NomLu, "0x%x", minVal<<16|maxVal) ;
	retLex = RANGE;
	break ;
      case 'i':
	/* n bit range is expected here */
	curChar=fgetc(inputStream) ;
	GetNumString() ;
	retLex = RANGE ;
	maxVal = atoi (NomLu) ;
	sprintf (NomLu, "0x%x", (1<<maxVal)-1) ;
	retLex = RANGE ;			            
	break ;			            
      case 'u':
	/* unsigned constant expression is expected here */
	retLex = UCSTEXP ;
	curChar=fgetc(inputStream) ;
	GetNumString() ;
	break ;
      case 's':
	/* signed constant expression is expected here */
	retLex = SCSTEXP ;
	curChar=fgetc(inputStream) ;
	GetNumString() ;
	break ;			            
      }
#endif
      if (NomLu[0]=='\0') {
	retLex = LEXERR ;
      }
      break ;
    case '$':
      curChar=fgetc(inputStream) ;
      if (IsIdent ()) {
	retLex = LEXEM ;
      }
      break ;
#ifdef _FOR_GAS
    case '<':
      curChar=fgetc(inputStream) ;
#if 0
      printf("%s: EXPR: inputStream: %c\n",__FUNCTION__,curChar);
#endif
      if (IsIdent ()) {
	retLex = EXPR;
      }
#if 0
      printf("%s: EXPR: inputStream: %c\n",__FUNCTION__,curChar);
#endif
      getExpression = 1;
      break ;
#endif
    default:
      if (IsIdent ()) {
	retLex = IDNTER ;
      }
    }
    if (retLex != LEXERR) {
      curChar=fgetc(inputStream) ;
      if (curChar != '>')
	retLex = LEXERR;
      if(getExpression) {
      curChar=fgetc(inputStream) ;
      if (curChar != '>')
	retLex = LEXERR;
      }
    }
    return retLex;
  }

  isSPACE = 0;
  if ((curChar == ' ') || (curChar == '\t')) {
    if (curChar == '\t')
      isSPACE = 1;
    curChar=fgetc(inputStream) ;
    while ((curChar == ' ') || (curChar == '\t')) {
      if (curChar == '\t')
	isSPACE = 1;
      curChar=fgetc(inputStream) ;
    }
    if (curChar == '\n') {
      lineNo++ ;
      return LINEN ;
    }
    ungetc(curChar, inputStream) ;
    if (isSPACE) {
      return SPACE ;
    }
    else {
      return AND ;
    }
  }
	
  GetTermString() ;
  return ELTER;
}

/* fonction d'identification de l'ATOME (lexeme) *
 * renvoie 1 si atomlu est egal a atomattendu    *
 * rencoie 0 sinon                               */

int identifie (ATOME atomlu, ATOME atomattendu) {
  dern_atom=(atomlu==LEXERR)?atomlu:atomattendu;
  return (atomlu==atomattendu);
}

char *GetEncodingInformations() {
  char *EncodingInfo = NULL ;
  int numChar=0;
  int blockSize = 256 ;
  int identLevel = 0 ;

  /* Skip leading spaces */
  curChar=fgetc(inputStream) ;
  while (isspace(curChar)) {
    if (curChar == '\n') {
      lineNo++ ;
    }
    curChar=fgetc(inputStream) ;
  }
  if (curChar != '{') {
    printf("1: line %d\n",lineNo);
    utilsPrintFatal(GENSYN_ERROR_SYNTAX0,"'{' expected");
  }
  curChar=fgetc(inputStream) ;
  identLevel = 1;
  while ((identLevel > 0) && (curChar != EOF)) {
    if (curChar == '\n') {
      lineNo++ ;
    }
    if (curChar == '{') {
      identLevel++;
    }
    if (curChar == '}') {
      identLevel--;
      if (identLevel == 0) {
	curChar = '\0' ;
      }
    }
    if (!(numChar%256)) {
      EncodingInfo = (char *) realloc (EncodingInfo, blockSize);
      blockSize += 256 ;
    }
    EncodingInfo[numChar++]=curChar ;
    if (identLevel>0)
      curChar=fgetc(inputStream) ;
  }
  if (identLevel != 0) {
    printf("2: line %d\n",lineNo);
    utilsPrintFatal(GENSYN_ERROR_SYNTAX0,"'}' expected") ;
  }
#if 0
  curChar=fgetc(inputStream) ;
  /* Skip spaces and newlines */
  while (isspace(curChar)) {
    if (curChar == '\n')
      lineNo++ ;
    curChar=fgetc(inputStream) ;
  }
	
  ungetc(curChar, inputStream) ;
#endif
	
  return EncodingInfo ;
}



void SkipToLineEnd () {
  while ((curChar != '\n')&&(curChar != EOF))
    curChar=fgetc(inputStream) ;
}
