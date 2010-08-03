/** \file table.h
    \brief Parser table management

*/
#ifndef __TABLE_H__
#define __TABLE_H__

#include <stdio.h>

#include "strind.h"

/** InitAllTables */
extern void InitAllTables () ;
/** AddLexem */
extern s_StrInd * AddLexem (char *) ;
/** AddVar */
extern s_StrInd * AddVar (char *) ;
/** AddTerminal */
extern s_StrInd * AddTerminal (char *);
/** AddNonTerminal */
extern s_StrInd * AddNonTerminal (char *);
/** TouteRegleDefinie */
extern int TouteRegleDefinie ();
/** PrintNonTermTable */
extern void PrintNonTermTable (FILE *);
/** PrintTermTable */
extern void PrintTermTable (FILE *, char *);
/** PrintLexemTable */
extern void PrintLexemTable (FILE *);
/** PrintVarTable */
extern void PrintVarTable (FILE *);
/** AssignTermIndex */
extern void AssignTermIndex(); 
/** AssignNonTermIndex */
extern void AssignNonTermIndex(); 

#endif /* __TABLE_H__ */
