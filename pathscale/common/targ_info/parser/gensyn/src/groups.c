#include <stdlib.h>
#include <string.h>

#include "gensyn_defs.h"
#include "table.h"

typedef struct s_Slot {
	int index ;
	int size ;
	char name[MAX_SEM_LEN] ;
	char iType[2048] ;
} s_Slot ;
	
typedef struct s_SlotList {	
	s_Slot *slot;
	struct s_SlotList *next ;
} s_SlotList ;

typedef struct {
	char name[MAX_SEM_LEN] ;
	int size ;
	struct s_SlotList *slist ;
} s_Grouping ;

typedef struct s_GroupingList {
	s_Grouping *grouping ;
	struct s_GroupingList *next ;
} s_GroupingList ;

s_Slot *currSlot ;
uint32 *currNumField ;
s_Grouping *currGrouping ;
s_GroupingList *groupingList ;
s_GroupingList *lastGrouping ;

s_GroupingList *NewGroupingList()
{
	s_GroupingList *head ;
	
	head = (s_GroupingList *) malloc (sizeof(s_GroupingList)) ;
	head->next = NULL ;
	head->grouping = NULL ;
	lastGrouping = head ;
	
	return head ;
}

s_GroupingList *AddGroupingList(s_GroupingList *tail, s_Grouping *grouping)
{
	s_GroupingList *felem ;
	
	felem = (s_GroupingList *) malloc (sizeof(s_GroupingList)) ;
	felem->next = tail->next ;
	felem->grouping = grouping ;
	tail->next = felem ;
	
	return felem ;
}

s_SlotList *NewSlotList()
{
	s_SlotList *head ;
	
	head = (s_SlotList *) malloc (sizeof(s_SlotList)) ;
	head->next = NULL ;
	
	return head ;
}

void AddSlotList(s_SlotList *slist, s_Slot *slot)
{
	s_SlotList *selem ;
	
	selem = slist ;
	while (selem->next != NULL)
	{
		selem = selem->next ;
	}
	selem->next = (s_SlotList *) malloc (sizeof(s_SlotList)) ;
	selem->next->slot = slot ;
	selem->next->next = NULL ;
}

void AddInstTypeToCurrSlot(char *typeName)
{
	char *pdst;

	pdst = currSlot->iType ;
	if (currSlot->size)
	{
		while(*pdst != '\0')
			pdst++ ;

		*pdst++ = ',' ;
		*pdst++ = ' ' ;
	}
	strcpy (pdst, typeName) ;
	currSlot->size++ ;
}

void SetNewCurrSlot(uint32 index)
{
	s_Slot *slot ;
	
	slot = (s_Slot *) malloc (sizeof(s_Slot)) ;
	sprintf (slot->name, "%s_%d", currGrouping->name, currGrouping->size) ;
	slot->index = index;
	slot->size = 0;
	slot->iType[0] = '\0' ;
	currSlot = slot ;
}

void AddCurrSlotToCurrGrouping()
{
	AddSlotList(currGrouping->slist, currSlot) ;
	currGrouping->size++ ;
}

int IsGroupingDefined(char *name)
{
	s_GroupingList *gelem ;
	
	gelem = groupingList->next ;
	while (gelem != NULL) {
		if (!strcmp(gelem->grouping->name, name))
			return 1 ;
		gelem = gelem->next ;
	}
	return 0;
}

void SetNewCurrGrouping(char *name)
{
	s_Grouping *grouping ;

	grouping = (s_Grouping*) malloc (sizeof(s_Grouping)) ;
	strcpy (grouping->name, name) ;
	grouping->size = 0 ;
	grouping->slist = NewSlotList() ;
	currGrouping = grouping ;
}

void InitGroupings()
{
	currSlot = NULL;
	currGrouping = NULL;
	groupingList = NewGroupingList() ;
}


void AddCurrGroupingToList()
{
	lastGrouping = AddGroupingList(lastGrouping, currGrouping) ;
}

int findex ;

void PrintGroupingInfo(s_Grouping* grouping, FILE *cfd)
{
	fprintf (cfd, "{0, %d, %s}", grouping->size, grouping->name) ;
}

int PrintGroupingList (FILE *cfd, int numPerLine, void (*PrintFct) (s_Grouping *, FILE *fcd),
		char *sepStr, char* nlStr)
{
	s_GroupingList *felem ;
	int firstPrint=1 ;
	int numPrinted=0 ;
	
	felem = groupingList->next ;
	while (felem != NULL) {
		if (firstPrint) {
		    firstPrint = 0;
		}
		else {
		    fprintf (cfd, sepStr) ;
		}

		if (!(numPrinted % numPerLine))
			fprintf (cfd, nlStr) ;
		numPrinted++;
		PrintFct(felem->grouping, cfd) ;
		felem = felem->next ;
	}
	return numPrinted ;
}

void PrintSlotList (FILE *cfd, int numPerLine, s_SlotList *slist)
{
	s_SlotList *selem ;
	s_Slot *slot ;
	int firstPrint=1 ;
	int numPrinted=0 ;
	
	selem = slist->next ;
	while (selem != NULL)
	{
		if (firstPrint)
		{
		    firstPrint = 0;
		}
		else
		{
		    fprintf (cfd, ", ") ;
		}

		if (!(numPrinted % numPerLine))
			fprintf (cfd, "\n\t") ;
		numPrinted++;
		slot = selem->slot ;
		fprintf (cfd, "{%d, %d, %s}", slot->index, slot->size, slot->name );
		selem = selem->next ;
	}
}

void PrintInstTypes (FILE *cfd, int numPerLine, s_SlotList *slist)
{
	s_SlotList *selem ;
	s_Slot *slot ;
	int numPrinted=0 ;
	
	selem = slist->next ;
	while (selem != NULL)
	{
		if (!(numPrinted % numPerLine))
			fprintf (cfd, "\n") ;
		numPrinted++;
		slot = selem->slot ;
		fprintf (cfd, "int %s[] = {%s} ;", slot->name, slot->iType);
		selem = selem->next ;
	}
}

void PrintGroupings (FILE*cfd)
{
	s_GroupingList *gelem ;
	
	gelem = groupingList->next ;
	while (gelem != NULL)
	{
		PrintInstTypes (cfd, 1, gelem->grouping->slist) ;
		fprintf (cfd, "\n\ns_Group %s[] = {", gelem->grouping->name) ;
		PrintSlotList (cfd, 1, gelem->grouping->slist) ;
		fprintf (cfd, "\n} ;\n\n") ;
		gelem = gelem->next ;
	}
}

void PrintGroupingTable (FILE*cfd)
{
	int size = 0;

	if (groupingList->next)
	{
		fprintf (cfd, "s_Grouping groupingTable[] = {") ;
		size = PrintGroupingList (cfd, 1, PrintGroupingInfo, ", ", "\n\t") ;
		fprintf (cfd, "\n} ;\n\n") ;
		fprintf (cfd, "uint32 groupingTableSize = %d ;\n\n", size) ;
	}
}
