#include <stdio.h>
#include <ctype.h>

#include "libUtils.h"

#include "gensyn_version.h"
#include "gensyn_option.h"
#include "gensyn_error.h"

#include "encinfo.h"

static int encInfoIndex ;
static int gblInfoIndex ;
static int portInfoIndex ;
static int multiplyInfoIndex ;
static char *encInfoTable[MAX_ENC_INFO] ;
static char *globDeclInfo[MAX_ENC_INFO] ;
static char *portInfo[1] ;
static char *multiplyInfo[1] ;
static int initVal ;

void InitEncInfo()
{
	encInfoIndex = 0 ;
	gblInfoIndex = 0 ;
	initVal = -1 ;
}

int GetInit()
{
	return initVal ;
}

void SetInit(int value)
{
	initVal = value ;
}

void AddNewGlobDecl(char *newGlobDeclInfo)
{
	if (gblInfoIndex>=MAX_ENC_INFO)
	{
      utilsPrintInternalError(GENSYN_INTERNALERROR_NEWGLOBAL) ;
	}
	globDeclInfo[gblInfoIndex++] =  newGlobDeclInfo ;
}

void AddNewPort(char *newPortInfo)
{
	if (portInfoIndex>=1)
	{
      utilsPrintInternalError(GENSYN_INTERNALERROR_PORT) ;
	}
	portInfo[portInfoIndex++] =  newPortInfo ;
}

void AddNewMultiply(char *newMultiplyInfo)
{
	if (multiplyInfoIndex>=1)
	{
      utilsPrintInternalError(GENSYN_INTERNALERROR_MULTIPLY) ;
	}
	multiplyInfo[multiplyInfoIndex++] =  newMultiplyInfo ;
}

/* If all characters are none, space, carriage return or tab, encoding (action) info
   is considered empty (no action) */
static int EncInfoIsEmpty( char *EncInfo ) {
  int i = 0;
  while(EncInfo[i] != '\0') {
    if(EncInfo[i] != ' '  &&
       EncInfo[i] != '\n' &&
       EncInfo[i] != '\t') {
      return 0;
    }
    i++;
  }
    return 1;
}

int AddNewEncInfo(char *newEncInfo)
{
  if (EncInfoIsEmpty(newEncInfo)) {
    /* encoding info is empty no need to generate it */
    return -1 ;
  }
  if (encInfoIndex>=MAX_ENC_INFO) {
    utilsPrintInternalError(GENSYN_INTERNALERROR_NEWENCODING) ;
  }
  encInfoTable[encInfoIndex++] =  newEncInfo ;
  return encInfoIndex - 1;
}

void DisplayPortInfo (FILE *outStream) {
   if (portInfoIndex>0)
		fprintf (outStream, "%s\n", portInfo[0]) ;
#ifdef __TOOLKIT__
   else {
      /* By default for RTK emit port 0 as used port */
      fprintf(outStream,
"/* Array defining extension ports used */\n"
"static unsigned int ExtPort[] = {1,0,0,0,0,0,0,0};\n\n"
"/* String defining port used */\n"
"#define DEF_EXT_ID_STR \"0\"\n"
      );
   }
#endif
}

void DisplayMultiplyInfo (FILE *outStream) {
   if (multiplyInfoIndex>0)
		fprintf (outStream, "%s\n", multiplyInfo[0]) ;
#ifdef __TOOLKIT__
   else {
      /* By default for RTK emit no multiply */
      fprintf(outStream,
"\n/* Function defining multiply instruction encoding:  */\n"
"/* Syntax of instruction must be \"Gx? MP Rm, Rn, Rp\" */\n"
"/* Function returns 0 if instruction not available.  */\n\n"
"static SXLDEXT_s32T ExtensionMultiply ( void * Instance, SXLDEXT_u32T * u7 ) {\n"
"   *u7 = 0;\n"
"   return 0;\n"
"}\n\n");

   }
#endif
}

void DisplayEncFunc (FILE *outStream)
{
	int i ;

	for (i=0; i<gblInfoIndex; i++)
	{
		fprintf (outStream, "%s\n", globDeclInfo[i]) ;
	}
   DisplayPortInfo(outStream);
	if (encInfoIndex>0)
	{
		fprintf (outStream, "\n\nstatic void STRASSEncode(void * Instance, int val)\n{\n") ;
		fprintf (outStream, "\tPARSER_InstanceT * PARSER_Instance = (PARSER_InstanceT *)Instance;\n") ;
		fprintf (outStream, "\tswitch (val)\n\t{\n") ;
		for (i=0; i<encInfoIndex; i++)
		{
			fprintf (outStream, "\t\tcase %d:\n\t\t\t { %s};\n\t\t\tbreak;\n", i, encInfoTable[i]) ;
		}
		fprintf (outStream, "\n\t}\n}\n") ;
	}
}
