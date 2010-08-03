#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "libUtils.h"

#include "gensyn_version.h"
#include "gensyn_option.h"
#include "gensyn_error.h"

#include "gensyn_defs.h"
#include "table.h"
#include "lname.h"
#include "arbsem.h"
#include "encinfo.h"

typedef struct s_NameList {
	char name[MAX_SEM_LEN] ;
	struct s_NameList *next ;
} s_NameList ;

s_NameList *classList ;
int classIndex;

s_NameList *NewNameList()
{
	s_NameList *head ;
	
	head = (s_NameList *) malloc (sizeof(s_NameList)) ;
	head->next = NULL ;
	head->name[0] = '\0' ;
	
	return head ;
}

void AddNameList(s_NameList *nlist, char *name)
{
	s_NameList *felem ;
	
	felem = (s_NameList *) malloc (sizeof(s_NameList)) ;
	felem->next = nlist->next ;
	strcpy(felem->name, name) ;
	nlist->next = felem ;
}

void AddNewClassInfo(char *name)
{
	AddNameList(classList, name) ;
}

int IsNameDefined(s_NameList* head, char *name)
{
	s_NameList *felem ;
	
	felem = head->next ;
	while (felem != NULL) {
		if (!strcmp(felem->name, name))
			return 1 ;
		felem = felem->next ;
	}
	return 0;
}

int IsClassDefined(char *name)
{
	return IsNameDefined(classList, name) ;
}

void InitDefineLists()
{
	classList = NewNameList() ;
	classIndex = 0;
}

static int nindex ;

void PrintName(s_NameList* celem, FILE *cfd)
{
	fprintf (cfd, "#define %s %d", celem->name, nindex++) ;
}

static int firstPrint=1 ;
static int numPrinted=0 ;

void PrintRecNameList (FILE *cfd, s_NameList* head, int numPerLine, void (*PrintFct) (s_NameList *, FILE *fcd),
		char *sepStr, char* nlStr)
{
	if (head != NULL) {
		PrintRecNameList (cfd, head->next, numPerLine, PrintFct, sepStr, nlStr);
		if (firstPrint) {
		    firstPrint = 0;
		}
		else {
		    fprintf (cfd, sepStr) ;
		}

		if (!(numPrinted % numPerLine))
			fprintf (cfd, nlStr) ;
		numPrinted++;
		PrintFct(head, cfd) ;
	}
}

void PrintNameList (FILE *cfd, s_NameList* head, int numPerLine, void (*PrintFct) (s_NameList *, FILE *fcd),
		char *sepStr, char* nlStr)
{
	firstPrint=1 ;
	numPrinted=0 ;
	
	PrintRecNameList (cfd, head->next, numPerLine, PrintFct, sepStr, nlStr);
}

void PrintClassList (FILE*cfd)
{
	nindex = 0;
	PrintNameList (cfd, classList, 1, PrintName, "", "\n") ;
	fprintf (cfd, "\n\n") ;
}
