#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libUtils.h"

#include "gensyn_version.h"
#include "gensyn_option.h"
#include "gensyn_error.h"

#include "arbsem.h"
#include "table.h"
#include "avl.h"

static AVLnode *termTable=NULL, *nontermTable=NULL, *lexTable=NULL, *varTable=NULL;
static int nbnid=0;

void InitAllTables ()
{
	termTable=NULL ;
	nontermTable=NULL ;
	lexTable=NULL ;
	varTable=NULL ;
}

s_StrInd * AddVar (char *val)
{
    return AVLadd (val, &varTable);
}

s_StrInd * AddLexem (char *val)
{
    return AVLadd (val, &lexTable);
}

/* procedure d'ajout d'un terminal dans la table des terminaux */

s_StrInd * AddTerminal (char *val)
{
#if 0
  printf("%s: %s\n",__FUNCTION__,val);
#endif
    return AVLadd (val, &termTable);
}

/* procedure d'ajout d'un non terminal dans la table des non terminaux */

s_StrInd * AddNonTerminal (char *val)
{
#if 0
  printf("%s: %s\n",__FUNCTION__,val);
#endif
    return AVLadd (val, &nontermTable);
}

/* procedure de parcours de la table des non terminaux                *
 * afin de determiner si toutes les regles utilisees ont ete definies */

static void VerifTableNonTerm (AVLnode *avl) {
  int ind;

  if (avl) {
    VerifTableNonTerm (avl->leftNode);
    if (RechAct (GETSTR(*avl->token), &ind)==NULL) {
      if (strcmp(GETSTR(*avl->token), "__GLOBDECL")) {
	nbnid=1;
	utilsPrintError(GENSYN_ERROR_UNDEFRULE,GETSTR(*avl->token));
      }
    }
    VerifTableNonTerm (avl->rightNode);
  }					
}

/* fonction retournant 1 si toutes les regles ont ete definies, 0 sinon */
 
int TouteRegleDefinie ()
{
	nbnid=0;
	VerifTableNonTerm (nontermTable);
	return !nbnid;
}


static int indexAVL ;

static void assignAVLindex(AVLnode *avl)
{
    if (avl) {
		assignAVLindex(avl->leftNode);
		SETIND(*avl->token, indexAVL++) ;
		assignAVLindex(avl->rightNode);
    }					
}

static int firstPrint ;
static int numPerLine ;
static int numPrinted ;

static int table_idx;

/* Display all AVL tree element using "format" as format */

static void DisplayAVL (AVLnode *avl, FILE *fd, char *format)
{
  if (avl) {
    DisplayAVL (avl->leftNode, fd, format);
    if (firstPrint) {
      firstPrint = 0;
    }
    else {
      fprintf (fd, ", /* %d */",table_idx++) ;
      if (!(numPrinted % numPerLine))
	fprintf (fd, "\n") ;
    }
    numPrinted++;
    fprintf (fd, format, GETSTR(*avl->token));		
    DisplayAVL (avl->rightNode, fd, format);
  }					
}

static void AVLSize (AVLnode *avl)
{
	if (avl)
	{
		AVLSize (avl->leftNode);
		numPrinted++;
		AVLSize (avl->rightNode);
	}					
}

void AssignTermIndex ()
{
    indexAVL = 0;
    assignAVLindex (termTable);
}

void AssignNonTermIndex ()
{
    indexAVL = 0;
    assignAVLindex (nontermTable);
}

/* Display Non Terminal Table                                 *
 * Non Terminal are rules defined by the user in grammar file *
 * i.e : <Non_Term_Name>                                      */

void PrintNonTermTable (FILE *fd)
{
  numPrinted = 0;
  AVLSize(nontermTable) ;
  if (numPrinted>0) {
    firstPrint = 1;
    numPrinted = 0;
    numPerLine = 4;
    fprintf (fd, "static PARSER_nodeSemT *semTable[] = {\n") ;
    table_idx = 0;
    DisplayAVL (nontermTable, fd, "\tNTR_%s");
    fprintf (fd, "  /* %d */",table_idx++) ;
    fprintf (fd, "\n} ;\n\n");
    fprintf (fd, "static PARSER_u16T semTableSize = %d ;\n\n", numPrinted);
  }
}

/* Display Terminal Table                                   *
 * Terminals are string defined by the user in grammar file */

void PrintTermTable (FILE *fd,char *Prefix)
{
  numPrinted = 0;
  AVLSize(termTable) ;
  if (numPrinted>0) {
    /*    fprintf (fd, "%sEXT_stringT stringTable[] = {\n", Prefix) ; */
    fprintf (fd, "static PARSER_stringT stringTable[] = {\n") ;
    firstPrint = 1;
    numPerLine = 8;
    numPrinted = 0;
    table_idx = 0;
    DisplayAVL (termTable, fd, "\t\"%s\"");
    fprintf (fd, "  /* %d */",table_idx++) ;
    fprintf (fd, "\n} ;\n\n");
    /*    fprintf (fd, "%sEXT_u16T stringTableSize = %d ;\n\n", Prefix, numPrinted); */
    fprintf (fd, "static PARSER_u16T stringTableSize = %d ;\n\n", numPrinted);
  }
}

/* Display Lexem Table                                      *
 * Lexem are terminals declared by the user in grammar file *
 * i.e : <$Lexem_Name>                                      */

void PrintLexTable(FILE *fd)
{
  firstPrint = 1;
  numPrinted = 0;
  numPerLine = 8;	
  table_idx = 0;
  DisplayAVL (lexTable, fd, "\tLEX_%s");
  fprintf (fd, "  /* %d */",table_idx++) ;
}

/* Display Field Table                                     */
/* Display Field Table as Address of field                 */
void PrintVarTable(FILE *fd)
{
  numPrinted = 0;
  AVLSize(varTable);
  if (numPrinted>0) {
    fprintf (fd, "PARSER_FieldT ") ;
    firstPrint = 1;
    numPrinted = 0;
    numPerLine = 16;
    table_idx = 0;
    DisplayAVL (varTable, fd, " %s");
    fprintf (fd, "  /* %d */",table_idx++) ;
    fprintf (fd, " ;\n\n");
    fprintf (fd, "PARSER_FieldT *ffieldTable[] = {\n") ;
    firstPrint = 1;
    numPrinted = 0;
    numPerLine = 32;
    table_idx = 0;
    DisplayAVL (varTable, fd, " &%s");
    fprintf (fd, "  /* %d */",table_idx++) ;
    fprintf (fd, "\n} ;\n\n");
    fprintf (fd, "PARSER_u16T ffieldTableSize = %d ;\n\n", numPrinted);
  }
}


