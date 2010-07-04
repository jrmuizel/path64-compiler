/*-------------------------------------------------------------------------*/
/*
 *      Copyright 2002, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 *      STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 *
 * File : toolerror.c
 *
 * Description : specific routines for error message emission.
............................................................................
*/
#include <stdio.h>
#include <string.h>

#include "libUtils.h"

#include "gensyn_version.h"
#include "gensyn_option.h"
#include "gensyn_error.h"

#include "unilex.h"
#include "gensyn_defs.h"

extern ATOME dern_atom;
extern char *inFilename ;

int noError ;
int lineNo ;

void SetNoError ( void ) {
   noError = 1;
	lineNo = 1;
}

void ClearNoError ( void ) {
	noError = 0 ;
}

static char Message[1024];
void DisplayErrorMessage ( void ) {
   
	noError = 0;
	snprintf(Message,sizeof(Message),"File %s, Line %d : ",inFilename,lineNo) ;
	switch (dern_atom) {	
		case LEXERR:
			strncat(Message, "Lexical error\n",sizeof(Message));
			break;
		case COMMA: 
			strncat(Message, "',' expected\n",sizeof(Message));
			break;
		case COLUMN: 
         strncat(Message, "':' expected\n",sizeof(Message));
			break;
		case OPENACC:
			strncat(Message, "'{' expected\n",sizeof(Message));
			break;
		case CLOSACC:
			strncat(Message, "'}' expected\n",sizeof(Message));
			break;
		case OPENBRA:
			strncat(Message, "'[' expected\n",sizeof(Message));
			break;
		case CLOSBRA:
			strncat(Message, "']' expected\n",sizeof(Message));
			break;
		case IDNTER:
			strncat(Message, "Non terminal expected\n",sizeof(Message));
			break;
		case ELTER:
			strncat(Message, "Terminal expected\n",sizeof(Message));
			break;
		case LINEN:
			strncat(Message, "<EOL> expected\n",sizeof(Message));
			break;
		case FILEN:
			strncat(Message, "<EOF> expected\n",sizeof(Message));
			break;
		case OR:
			strncat(Message, "'.' expected\n",sizeof(Message));
			break;
		case AND:
			strncat(Message, "'|' expected\n",sizeof(Message));
			break;
		case SYNKEYWORD:
			strncat(Message, "\"Sy\" keyword expected\n",sizeof(Message));
			break;
		case ACTKEYWORD:
			strncat(Message, "\"Act\" keyword expected\n",sizeof(Message));
			break;
      default:
      	strncat(Message,"Non terminal, terminal, '(', '[' or '{' expected\n",sizeof(Message));
         break;
	}
   utilsPrintError(GENSYN_ERROR_SYNTAX,Message);
}

