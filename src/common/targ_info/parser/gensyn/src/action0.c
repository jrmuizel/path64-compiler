#include <stdio.h>
#include <stdlib.h>

#include "libUtils.h"

#include "gensyn_version.h"
#include "gensyn_option.h"
#include "gensyn_error.h"

#include "arbsem.h"
#include "action0.h"
#include "table.h"
#include "encinfo.h"
#include "format.h"
#include "lname.h"
#include "groups.h"

#define _FOR_GAS

extern char NomLu[]; /**< Last lexem read */

static int lexCount ; /**< Lexem counter */

    /**
    *
    *   \brief Reset lexem counter.
    *
    *   \param  none.
    *
    *   \return none.
    *
    */
void InitLexCount()
{
	lexCount = 0;
}


    /**
    *
    *   \brief Add last lexem as new class information. Emits an error if already defined.
    *
    *   \param  none.
    *
    *   \return none.
    *
    */
void AddNewClass () {
  if (IsClassDefined(NomLu)) {
    utilsPrintFatal(GENSYN_ERROR_INSTCLASS,NomLu);
  }
  AddNewClassInfo(NomLu) ;	
}

    /**
    *
    *   \brief Add parameter as init stage of automaton. Emits an error for multiple defined init stages.
    *
    *   \param  encinfo : (char*) encoding information of init stage.
    *
    *   \return none.
    *
    */
void AddInitAction (char *encInfo) {
  if (noError) {
    if (GetInit() == -1) {
      SetInit(AddNewEncInfo(encInfo)) ;
    }
    else {
      /* error multiple init sequence */
    }
  }
}

/**
 *
 *   \brief Add parameter as new action.
 *
 *   \param  encinfo: (char *) encoding information of new automaton state.
 *
 *   \return none.
 *
 */
void AddNewAction (char *encInfo) {
  noeudSEM *T1, *T2, *ActionNode;	
  
  if (noError) {
    ActionNode = T1 = DepilerSEM ();
    T2 = DepilerSEM ();
    if ((T1->action == OPTION)||(T1->action == REPEAT)) {
      /* Here we have an optional node so encoding info are *
       * transmitted to its dependent node                   */
      ActionNode = T1->fg ;
    }
    SetEncInfo(ActionNode, AddNewEncInfo(encInfo)) ;
    SetPopSemIndex (ActionNode, lexCount) ;
    if ( utilsOptionIsDefined( & gensyn_option.verbose ) )
      printf ("Adding new action : %s\n", GETSTR(*T2->token)) ;
    AjoutAct (GETSTR(*T2->token), T1);
    EmpilerSEM(T2) ;
  }
}

/**
 *
 *   \brief Set last lexem as new format. Emits an error if format already defined.
 *
 *   \param  none.
 *
 *   \return none.
 *
 */
void CreateNewFormat() {
  if (IsFormatDefined(NomLu)) {
    utilsPrintFatal(GENSYN_ERROR_FORMATDEF,NomLu);
  }
  SetNewCurrFormat(NomLu) ;
}

/**
 *
 *   \brief Set last lexem as new current field.
 *
 *   \param  none.
 *
 *   \return none.
 *
 */
void CreateNewField() {
  SetNewCurrField(NomLu) ;
}

/**
 *
 *   \brief Add numerical value of last lexem as a numeric part of current field.
 *
 *   \param  none.
 *
 *   \return none.
 *
 */
void AddNumField() {
  AddNumFieldToCurrField(atoi(NomLu)) ;
}

/**
 *
 *   \brief Add current Field to current Format.
 *
 *   \param  none.
 *
 *   \return none.
 *
 */
void AddNewField() {
  AddCurrFieldToCurrFormat() ;
}

/**
 *
 *   \brief Add current Format to list of formats.
 *
 *   \param  none.
 *
 *   \return none.
 *
 */
void AddNewFormat () {
  AddCurrFormatToList() ;
}

/**
 *
 *   \brief Create last lexem as a new grouping
 *
 *   \param  none.
 *
 *   \return none.
 *
 */
void CreateNewGrouping() {
  if (IsGroupingDefined(NomLu)) {
    utilsPrintFatal(GENSYN_ERROR_GROUPINGDEF,NomLu);
  }
  SetNewCurrGrouping(NomLu) ;
}

/**
 *
 *   \brief Set new lexem numerical value as current slot of grouping.
 *
 *   \param  none.
 *
 *   \return none.
 *
 */
void CreateNewSlot() {
  SetNewCurrSlot(atoi(NomLu)) ;
}

/**
 *
 *   \brief Add current instruction type to current slot.
 *
 *   \param  none.
 *
 *   \return none.
 *
 */
void AddInstType() {
  AddInstTypeToCurrSlot(NomLu) ;
}

/**
 *
 *   \brief Add current slot to current grouping.
 *
 *   \param  none.
 *
 *   \return none.
 *
 */
void AddNewSlot() {
  AddCurrSlotToCurrGrouping() ;
}

/**
 *
 *   \brief Add current grouping to grouping list.
 *
 *   \param  none.
 *
 *   \return none.
 *
 */
void AddNewGrouping () {
  AddCurrGroupingToList() ;
}

/**
 *
 *   \brief Unstack current non terminal lexem.
 *
 *   \param  none.
 *
 *   \return none.
 *
 */
void UnstackNTERM ()
{
    noeudSEM *T1/*, *T2*/;
	
	if (noError)
	{
		T1 = DepilerSEM ();
	}
}

    /**
    *
    *   \brief Add last lexem as new non terminal lexem.
    *
    *   \param  none.
    *
    *   \return none.
    *
    */
void CreateNewNTERM ()
{
    noeudSEM *T ;

	if (noError)
	{
		T = GenSEM (IDNTER, AddNonTerminal (NomLu)) ;
		EmpilerSEM (T);
	}
}

#ifdef _FOR_GAS
    /**
    *
    *   \brief Add last lexem as new expression.
    *
    *   \param  none.
    *
    *   \return none.
    *
    */
void CreateNewEXPR ()
{
    noeudSEM *T ;

	if (noError)
	{
		T = GenSEM (EXPR, AddTerminal (NomLu)) ;
		EmpilerSEM (T);
		lexCount++ ; 
	}
}

    /**
    *
    *   \brief Add last lexem as new expression.
    *
    *   \param  none.
    *
    *   \return none.
    *
    */
void CreateNewCstExpr() {
    noeudSEM *T ;

	if (noError)
	{
		T = GenSEM (UCSTEXP, AddTerminal (NomLu)) ;
		EmpilerSEM (T);
		lexCount++ ; 
	}
}
#endif

void CreateEmpty ()
{
    noeudSEM *T ;

    
	if (noError)
	{
		T = GenSEM (EMPTY, NULL) ;
		EmpilerSEM (T);
	}
}

void CreateNewNumVal (ATOME atomRead)
{
    noeudSEM *T ;
    s_StrInd *newNum ;

	if (noError)
	{
		newNum = (s_StrInd*) malloc (sizeof(s_StrInd)) ;
		SETSTR(*newNum, NomLu) ;
		T = GenSEM (atomRead, newNum) ;
		EmpilerSEM (T);
		lexCount++ ; 
	}
}

void CreateNewLexem (ATOME atomRead)
{
    noeudSEM *T ;

	if (noError)
	{
		T = GenSEM (atomRead, AddLexem (NomLu)) ;
		EmpilerSEM (T);
		lexCount++ ; 
	}
}

void GenDualNodeSem(ATOME atomRead)
{
    noeudSEM *T1, *T2, *T;	
	
	if (noError)
	{
		T1 = DepilerSEM ();
		T2 = DepilerSEM ();
		T=GenSEM (atomRead, NULL);
		SetGauche (T, T2);	
		SetDroite (T, T1);
		EmpilerSEM (T);
	}
}


void CreateNewTERM ()
{
    noeudSEM *T ;
    
	if (noError)
	{
		T = GenSEM (ELTER, AddTerminal (NomLu)) ;
		EmpilerSEM (T);
	}
}

void GenModeNodeSem (ATOME atomRead)
{
    noeudSEM *T1, *T;	
	
	if (noError)
	{
		T1 = DepilerSEM ();
		T=GenSEM (atomRead, NULL);
		SetGauche (T, T1);	
		EmpilerSEM (T);
	}
}


