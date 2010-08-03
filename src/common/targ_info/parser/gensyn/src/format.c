#include <stdlib.h>
#include <string.h>

#include "gensyn_defs.h"
#include "table.h"

typedef struct {
	char name[MAX_SEM_LEN] ;
	uint32 numField[3] ;
} s_Field ;

typedef struct s_FieldList {
	s_Field *field ;
	struct s_FieldList *next ;
} s_FieldList ;

typedef struct {
	char name[MAX_SEM_LEN] ;
	int nbits;
	int nfields;
	struct s_FieldList *flist ;
} s_Format ;

typedef struct s_FormatList {
	s_Format *format ;
	struct s_FormatList *next ;
} s_FormatList ;

s_Field *currField ;
uint32 *currNumField ;
s_Format *currFormat ;
s_FormatList *formatList ;
s_FormatList *lastFormat ;

s_FormatList *NewFormatList()
{
	s_FormatList *head ;
	
	head = (s_FormatList *) malloc (sizeof(s_FormatList)) ;
	head->next = NULL ;
	head->format = NULL ;
	lastFormat = head ;
	
	return head ;
}

s_FormatList *AddFormatList(s_FormatList *tail, s_Format *format)
{
	s_FormatList *felem ;
	
	felem = (s_FormatList *) malloc (sizeof(s_FormatList)) ;
	felem->next = tail->next ;
	felem->format = format ;
	tail->next = felem ;
	
	return felem ;
}

s_FieldList *NewFieldList()
{
	s_FieldList *head ;
	
	head = (s_FieldList *) malloc (sizeof(s_FieldList)) ;
	head->next = NULL ;
	head->field = NULL ;
	
	return head ;
}

void AddFieldList(s_FieldList *flist, s_Field *field)
{
	s_FieldList *felem ;
	
	felem = (s_FieldList *) malloc (sizeof(s_FieldList)) ;
	felem->next = flist->next ;
	felem->field = field ;
	flist->next = felem ;
}


void AddNumFieldToCurrField(uint32 val)
{
	*currNumField++ = val ;
}

void SetNewCurrField(char *name)
{
	s_Field *field ;
	
	field = (s_Field*) malloc (sizeof(s_Field)) ;
	strcpy (field->name, name) ;
	(void) AddVar(name) ;
	currField = field ;
	currNumField = &field->numField[0] ;
}

void AddCurrFieldToCurrFormat()
{
	AddFieldList(currFormat->flist, currField) ;
}

int IsFormatDefined(char *name)
{
	s_FormatList *felem ;
	
	felem = formatList->next ;
	while (felem != NULL) {
		if (!strcmp(felem->format->name, name))
			return 1 ;
		felem = felem->next ;
	}
	return 0;
}

void SetNewCurrFormat(char *name)
{
	s_Format *format ;

	format = (s_Format*) malloc (sizeof(s_Format)) ;
	strcpy (format->name, name) ;
	format->nbits = -1 ;
	format->flist = NewFieldList() ;
	currFormat = format ;
}

void InitFormats()
{
	currField = NULL;
	currNumField = NULL;
	currFormat = NULL;
	formatList = NewFormatList() ;
}

void ComputeMaxBits(s_Format *format)
{
	s_FieldList *felem ;
	int max, newMax, i ;

	max = -1 ;
	i=0;
	felem = format->flist->next ;
	
	while (felem) {
		newMax = felem->field->numField[0] + felem->field->numField[2] ;
		if (newMax>max)
			max = newMax ;
		i++ ;
		felem = felem->next ;
	}
	format->nbits = max ;
	format->nfields = i ;
}

void AddCurrFormatToList()
{
	/* CheckFormat */
	ComputeMaxBits(currFormat) ;
	lastFormat = AddFormatList(lastFormat, currFormat) ;
}

int findex ;

void PrintFormatName(s_Format* format, FILE *cfd)
{
	fprintf (cfd, "#define %s %d", format->name, findex++) ;
}

void PrintFormatInfo(s_Format* format, FILE *cfd)
{
	fprintf (cfd, "{Format_%s, %d, %d}", format->name, format->nbits, format->nfields) ;
}

int  PrintFormatList (FILE *cfd, int numPerLine, void (*PrintFct) (s_Format *, FILE *fcd),
		char *sepStr, char* nlStr)
{
	s_FormatList *felem ;
	int firstPrint=1 ;
	int numPrinted=0 ;
	
	felem = formatList->next ;
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
		PrintFct(felem->format, cfd) ;
		felem = felem->next ;
	}
	return numPrinted ;
}

void PrintFieldList (FILE *cfd, int numPerLine, s_FieldList *flist)
{
	s_FieldList *felem ;
	s_Field *field ;
	int firstPrint=1 ;
	int numPrinted=0 ;
	
	felem = flist->next ;
	while (felem != NULL) {
		if (firstPrint) {
		    firstPrint = 0;
		}
		else {
		    fprintf (cfd, ", ") ;
		}

		if (!(numPrinted % numPerLine))
			fprintf (cfd, "\n\t") ;
		numPrinted++;
		field = felem->field ;
		fprintf (cfd, "{&%s, %ld, %ld, %ld}", field->name, field->numField[0],
				field->numField[1], field->numField[2]) ; 
		felem = felem->next ;
	}
}

void PrintFormatFields (FILE*cfd)
{
	s_FormatList *felem ;
	
	felem = formatList->next ;
	while (felem != NULL) {
		fprintf (cfd, "s_FormatField Format_%s[] = {", felem->format->name) ;
		PrintFieldList (cfd, 4, felem->format->flist) ;
		fprintf (cfd, "\n} ;\n\n") ;
		felem = felem->next ;
	}
}

void PrintFormatType (FILE*cfd)
{
	findex = 0;
	if (PrintFormatList (cfd, 1, PrintFormatName, "", "\n")>0)
		fprintf (cfd, "\n\n") ;
}

void PrintFormatTable (FILE*cfd)
{
	int size ;

	if (formatList->next)
	{
		fprintf (cfd, "s_Format formatTable[] = {") ;
		size = PrintFormatList (cfd, 1, PrintFormatInfo, ", ", "\n\t") ;
		fprintf (cfd, "\n} ;\n\n") ;
		fprintf (cfd, "PARSER_u16T formatTableSize = %d ;\n\n", size) ;
	}
}
