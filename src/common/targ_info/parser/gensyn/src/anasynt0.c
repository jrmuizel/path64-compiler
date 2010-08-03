#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "libUtils.h"

#include "gensyn_error.h"
#include "gensyn_version.h"
#include "gensyn_option.h"

#include "unilex.h"
#include "action0.h"
#include "arbsem.h"
#include "table.h"
#include "encinfo.h"
#include "format.h"
#include "lname.h"
#include "groups.h"

#define _FOR_GAS

typedef enum {IFORMAT, ICLASS, IROOT} e_typeField ;

static FILE *acStream = NULL ;          /**< Assembler C file output stream. */
static char *acFilename = NULL;         /**< Assembler C file full path. */
static char *acFilename_short = NULL;   /**< Assembler C file name. */

char *inFilename;

OutputT OutputType = OUT_FOR_NONE;

/* Entry point of syntax analyzer */
void SyntaxEntry (void);

/*-------------------------------------------------------------------------*/
/*
   Procedure usage. Displays available commands.
............................................................................
*/
static void Usage (FILE *stream)
{
  utilsOptionUsage( stream, NULL, "<file> [ <file> ... ]", 0 );
}

/*-------------------------------------------------------------------------*/
/*
   Routine printversion

   Description : print version information - called by main routine.
............................................................................
*/
static void printversion(FILE *stream)
{
  utilsVersion_fprintf( stream );
  fprintf( stream, "\n" );
}

/*-------------------------------------------------------------------------*/
/*
   Procedure InitParser. Initializes internals of syntax parser.
............................................................................
*/
void InitParser ( void ) {
   InitPile ();
   InitTab ();
   InitGroupings() ;
   InitFormats() ;
   InitDefineLists() ;
   InitEncInfo() ;
}

/*-------------------------------------------------------------------------*/
/*
   Procedure main. Main routine.
............................................................................
*/
char * shortname ( char * fname ) {
   char *ptr;

   if (NULL==fname) return NULL;

   ptr = strrchr(fname,'\\');
   if (NULL==ptr) {
      ptr = strrchr(fname,'/');
   }
   if (NULL==ptr) {
      ptr = fname;
   } else {
      ptr++;
   }
   return ptr;
}

/*-------------------------------------------------------------------------*/
/*
   Routine str_duplicate   
  
   Description : string duplication. This routine allocates memory.
............................................................................
*/
static char* str_duplicate( char *str ) 
{ 
  char *strnew = getmem_curarea(strlen(str)+1);
  strcpy(strnew,str);
  return strnew;
}   

/*-------------------------------------------------------------------------*/
/*
   Procedure main. Main routine.
............................................................................
*/
int main (int argc, char *argv[]) {

  int i;

  /**--------------------------------------------------
  *
  * Initialize utils library (set libUtils version and options, then add my version and options)
  *
  */

  if ( ! libUtilsInit( ) ) {
    utilsPrintFatal( gensyn_version.name, "Cannot initialize utils library !?\n" );
    exit( -1 );
  }

  /**--------------------------------------------------
  *
  * Initialize version of current application in utils
  *
  */

  if ( ! utilsVersionInit( & gensyn_version ) ) {
    utilsPrintFatal( gensyn_version.name, "Cannot initialize utils library !?\n" );
    exit( -1 );
  }

  /**--------------------------------------------------
  *
  * Initialize options of current application in utils
  *
  */

  #ifdef __TOOLKIT__
  gensyn_option.outtype   .flags |= UTILS_OPTION_IGNORE;
//  gensyn_option.short_name.flags |= UTILS_OPTION_IGNORE;
  gensyn_option.long_name .flags |= UTILS_OPTION_IGNORE;
  #endif

  if ( ! utilsOptionInit( ( utilsOption * )( & gensyn_option ), sizeof( gensyn_option ) / sizeof( utilsOption ) ) ) {
    utilsPrintFatal( gensyn_version.name, "Cannot initialize utils library !?\n" );
    exit( -1 );
  }

  /**--------------------------------------------------
  *
  * Parse options
  *
  */

  if ( ! utilsOptionParse( & argc, argv, NULL, TRUE ) ) {
    Usage( stderr );
    exit( -1 );
  }

#if defined(NDEBUG)
   utilsPrintSelectInfo   ("GS*");
   utilsPrintSelectWarning("GS*");
   utilsPrintSelectError  ("*");
#endif

  UTILS_DEBUG_DO( gensyn_version.name, utilsOptionDebug( stdout, NULL ); );

  /**--------------------------------------------------
  *
  * Interpret basic options help and version
  *
  */

  if ( utilsOptionIsDefined( & gensyn_option.help ) ) {
    Usage( stdout );
    exit( 0 );
  }

  if ( utilsOptionIsDefined( & gensyn_option.version ) ) {
    printversion( stdout );
    exit( 0 );
  }

#ifdef __TOOLKIT__
   OutputType = OUT_FOR_EXTENSION;
#endif

  /**--------------------------------------------------
  *
  * Check option settings coherency
  *
  */

   /* Check that ac is given on command line. */
   if ( ! utilsOptionIsDefined( & gensyn_option.acfile ) ) {
      utilsPrintFatal(GENSYN_ERROR_ARGMISSED,"-ac");
   }

   /* At least one argument is mandatory */
   if (argc<2) {
      Usage(stderr);
      utilsPrintFatal(GENSYN_ERROR_NOFILE);
   }

  /**--------------------------------------------------
  *
  * Interpret options
  *
  */

   /* Setup output stream for assembler C files */
   if ( utilsOptionIsDefined( & gensyn_option.acfile ) ) {
         acFilename = str_duplicate( gensyn_option.acfile.value->argument );
         acStream = fopen(acFilename,"w");
         if (NULL==acStream) {
            utilsPrintFatal(GENSYN_ERROR_OPENFILE,acFilename);
         }
   }
#if 0
   else {
      acFilename = str_duplicate("stdout");
      acStream = stdout;
   }
#endif

   /* Manage out type if specified */
   if ( utilsOptionIsDefined( & gensyn_option.outtype ) ) {
         if (!strcasecmp(gensyn_option.outtype.value->argument,"extension")) {
            OutputType = OUT_FOR_EXTENSION;
         } else if (!strcasecmp(gensyn_option.outtype.value->argument,"core")) {
            OutputType = OUT_FOR_CORE;
         } else {
            utilsPrintFatal(GENSYN_ERROR_WRONGARG, gensyn_option.outtype.value->argument, gensyn_option.outtype.data );
         }
   }

  /**--------------------------------------------------
  *
  * Interpret remaining argv[]
  *
  */

   /* Initialise parser and parse input files */
   InitParser();
   for( i=1; i<argc; i++ ) {
      inFilename = argv[i];
      inputStream=fopen (inFilename, "r");
      if (NULL==inputStream) {
         utilsPrintFatal(GENSYN_ERROR_OPENFILE,argv[i]);
      }
      SetNoError () ;
      SyntaxEntry();
      fclose(inputStream) ;
   }


   if ( utilsOptionIsDefined( & gensyn_option.verbose ) ) printf("\n");

   if (!TouteRegleDefinie ()) {
      freeallmem();
      exit(-1);
   }

   /* To set unique time to all generated codes. */
   /* It is also used to generate unique number for extensions. */
   set_current_time();

   /* Create short version of file names */
   acFilename_short=shortname(acFilename);

   if (NULL!=acStream) {
    /* Used rname as include file name for generated file */
      GenCode(acStream,acFilename_short);
      fclose(acStream);
   }

   if(acStream == NULL) {
     AssignTermIndex() ;
   }

   freeallmem();

   return (noError?0:-1) ;
}

/*-------------------------------------------------------------------------*/
/*
   Set of functions of the Syntax Analyzer.
............................................................................
*/
static ATOME atomlu;

void debut();
void N();
void StartSyntaxAnalysis();
void E();
void T();
void PT();
void F();
void GetSyntax () ;
void CheckColumnEOL () ;
void GetFieldList(e_typeField listType) ;
void GetSlotList() ;

void Expect(ATOME expected)
{
    if (!identifie (atomlu, expected)) {
        DisplayErrorMessage ();
        return;
    }
}

void ExpectKeywordColumn(ATOME expected)
{
    Expect(expected) ;
    atomlu=GetLeftLex(1);
    Expect(COLUMN) ;
}

void SyntaxEntry (void)
{
    atomlu=GetLeftLex (1);
    while ((atomlu == IDNTER)    ||
           (atomlu == GLBDEC)    ||
           (atomlu == FORMAT)    ||
           (atomlu == CLASS)     ||
           (atomlu == RELOC)     ||
           (atomlu == INIT)      ||
           (atomlu == ROOT)      ||
           (atomlu == ELF)       ||
           (atomlu == GROUPING)  ||
           (atomlu == PORT)      ||
           (atomlu == MULTIPLY))
    {
        switch (atomlu)
        {
            case IDNTER:
                CreateNewNTERM ();
                CheckColumnEOL ();
                atomlu=GetLeftLex(1);
                do
                {
                    InitLexCount ();
                    ExpectKeywordColumn(SYNKEYWORD) ;
                    GetSyntax () ;
                    atomlu=GetLeftLex(1);
                    ExpectKeywordColumn(ACTKEYWORD) ;
                    AddNewAction (GetEncodingInformations());
                    atomlu=GetLeftLex(1);
                } while (identifie (atomlu, SYNKEYWORD)) ;
                UnstackNTERM();
                break ;
            case  GLBDEC:
                atomlu=GetLeftLex(1);
                Expect(COLUMN) ;
                AddNewGlobDecl(GetEncodingInformations()) ;
                atomlu=GetLeftLex(1);
                break;
            case  PORT:
                atomlu=GetLeftLex(1);
                Expect(COLUMN) ;
                AddNewPort(GetEncodingInformations()) ;
                atomlu=GetLeftLex(1);
                break;
            case  MULTIPLY:
                atomlu=GetLeftLex(1);
                Expect(COLUMN) ;
                AddNewMultiply(GetEncodingInformations()) ;
                atomlu=GetLeftLex(1);
                break;
            case FORMAT:
                atomlu=GetLeftLex(1);
                Expect(COLUMN) ;
                atomlu=GetLeftLex(1);
                Expect(IDNTER) ;
                CreateNewFormat() ;
                atomlu=GetLeftLex(1);
                ExpectKeywordColumn(ACTKEYWORD) ;
                GetFieldList(IFORMAT) ;
                atomlu=GetLeftLex(1);
                break ;
            case ROOT:
	        InitLexCount ();
                atomlu=GetLeftLex(1);
                Expect(COLUMN) ;
                atomlu=GetLeftLex(1);
                Expect(IDNTER) ;
                CreateNewNTERM ();
                atomlu=GetLeftLex(1);
                ExpectKeywordColumn(DESKEYWORD) ;
                GetFieldList(IROOT) ;
                atomlu=GetLeftLex(1);
                ExpectKeywordColumn(ACTKEYWORD) ;
                AddNewAction (GetEncodingInformations());
                atomlu=GetLeftLex(1);
                UnstackNTERM();
                break ;
            case GROUPING:
                atomlu=GetLeftLex(1);
                Expect(COLUMN) ;
                atomlu=GetLeftLex(1);
                Expect(IDNTER) ;
                CreateNewGrouping() ;
                atomlu=GetLeftLex(1);
                ExpectKeywordColumn(DESKEYWORD) ;
                GetSlotList() ;
                atomlu=GetLeftLex(1);
                break ;
            case CLASS:
                CheckColumnEOL() ;
                atomlu=GetLeftLex(1) ;
                ExpectKeywordColumn(ACTKEYWORD) ;
                GetFieldList(ICLASS) ;
                atomlu=GetLeftLex(1);
                break ;
            case INIT:
                CheckColumnEOL() ;
                atomlu=GetLeftLex(1) ;
                ExpectKeywordColumn(ACTKEYWORD) ;
                AddInitAction (GetEncodingInformations());
                atomlu=GetLeftLex(1);
                break;
            case ELF:
	        utilsPrintFatal(GENSYN_ERROR_ELF);
                break;
            case RELOC:
	        utilsPrintFatal(GENSYN_ERROR_RELOC);
                break ;
            default:
                /* Do we need an internal error here or not? */
                break;
        }
    }
    if (!identifie (atomlu, FILEN)) {
        DisplayErrorMessage ();
        return;
    }
}

void GetIdField()
{
    atomlu=GetLeftLex(1);
    Expect(COLUMN) ;
    atomlu=GetLeftLex(1);
    Expect(IDNTER) ;
}

void GetElfField()
{
    atomlu=GetLeftLex(1);
    Expect(COLUMN) ;
    atomlu=GetLeftLex(1);
    if ((atomlu != IDNTER) && (atomlu != NUMVAL))
        DisplayErrorMessage() ;
}

void GetNumField()
{
    atomlu=GetLeftLex(1);
    Expect(COLUMN) ;
    atomlu=GetLeftLex(1);
    Expect(NUMVAL) ;
}

void GetFieldElem()
{
    atomlu=GetLeftLex(1);
    Expect(IDNTER) ;
    CreateNewField() ;
    GetNumField() ;
    AddNumField() ;
    GetNumField() ;
    AddNumField() ;
    GetNumField() ;
    AddNumField() ;
    atomlu=GetLeftLex(1);
}

void GetSlotElem()
{
    atomlu=GetLeftLex(1);
    Expect(NUMVAL) ;
    CreateNewSlot() ;
    atomlu=GetLeftLex(1);
    Expect(COLUMN) ;
    do {
        atomlu=GetLeftLex(1);
        Expect(IDNTER) ;
        AddInstType() ;
        atomlu=GetLeftLex(1);
    } while (atomlu == COLUMN) ;
}

void GetClass()
{
    do {
        atomlu=GetLeftLex(1);
        Expect(IDNTER) ;
        AddNewClass() ;
        atomlu=GetLeftLex(1);
    } while (atomlu == COMMA) ;
}

void GetRules()
{
    atomlu=GetLeftLex(1);
    Expect(IDNTER) ;
#if 0
    printf("%s: call newnterm (%s:%d)\n",__FUNCTION__,inFilename,lineNo);
#endif
    CreateNewNTERM ();
    atomlu=GetLeftLex(1);
    while (atomlu == COMMA)
    {
    	atomlu=GetLeftLex(1);
    	Expect(IDNTER) ;
#if 0
	printf("%s: COMMA call newnterm (%s:%d)\n",__FUNCTION__,inFilename,lineNo);
#endif
    	CreateNewNTERM ();
    	atomlu=GetLeftLex(1);
        GenDualNodeSem(XOR) ;
    }
}

void GetField()
{
    do {
        atomlu=GetLeftLex(1);
        Expect(OPENACC) ;
        GetFieldElem() ;
        Expect(CLOSACC) ;
        AddNewField() ;
        atomlu=GetLeftLex(1);
    } while (atomlu == COMMA) ;
}

void GetSlot()
{
    do {
        atomlu=GetLeftLex(1);
        Expect(OPENACC) ;
        GetSlotElem() ;
        Expect(CLOSACC) ;
        AddNewSlot() ;
        atomlu=GetLeftLex(1);
    } while (atomlu == COMMA) ;
}


void GetFieldList(e_typeField listType)
{
    atomlu=GetLeftLex(1);
    Expect(OPENACC) ;
    switch (listType)
    {
        case IFORMAT:
            GetField() ;
            break;
        case ICLASS:
            GetClass() ;
            break;
        case IROOT:
            GetRules() ;
            break;
    }
    Expect(CLOSACC) ;
    if (listType == IFORMAT)
        AddNewFormat() ;
}

void GetSlotList()
{
    atomlu=GetLeftLex(1);
    Expect(OPENACC) ;
    GetSlot() ;
    Expect(CLOSACC) ;
    AddNewGrouping() ;
}

void GetSyntax ()
{
    atomlu=GetRightLex();
    StartSyntaxAnalysis ();
    Expect(LINEN) ;
}

void CheckColumnEOL ()
{
    atomlu=GetLeftLex(1);
    Expect(COLUMN) ;
    atomlu=GetRightLex();
    Expect(LINEN) ;
}

void StartSyntaxAnalysis ()
{
    /* .Sy directive can be followed by :
     * Syntax_description     : 1 time matching pattern return true
     * [Syntax_description]     : (0 or 1) time matching pattern return true
     * [Syntax_description]* : (0 to n) time matching pattern return true
     *                          : Empty syntax description
     */
    switch (atomlu)
    {
        case LINEN:
            CreateEmpty () ;
            break ;
        case OPENBRA:
            atomlu=GetRightLex ();
            T();
            Expect(CLOSBRA) ;
            atomlu=GetLeftLex(0);
            switch (atomlu)
            {
                case STAR:
                    GenModeNodeSem (REPEAT);
                    atomlu=GetLeftLex(0);
                    break;
                case LINEN:
                    GenModeNodeSem (OPTION);
                    break;
                default:
                    /* Do we need an internal error here or not? */
                    break;
            }
            break;
        default:
            T ();
    }
}

void T ()
{
    F();
    while ((atomlu == SPACE) ||
        (atomlu == AND) ||
        (atomlu == IDNTER) ||
        (atomlu == ELTER) ||
        (atomlu == UCSTEXP) ||
        (atomlu == SCSTEXP) ||
        (atomlu == RANGE))
    {
        switch (atomlu)
        {
            case SPACE:
                atomlu = GetRightLex();
                F();
                GenDualNodeSem(SPACE) ;
                break;
            case AND:
                atomlu = GetRightLex();
                F();
                GenDualNodeSem(AND) ;
                break;
#ifdef _FOR_GAS
	    case UCSTEXP:
	      CreateNewCstExpr();
	      GenDualNodeSem(CONCAT);
	      atomlu = GetRightLex();
	      break;
#else
            case UCSTEXP:
            case SCSTEXP:
            case RANGE:
                CreateNewNumVal (atomlu) ;
                GenDualNodeSem(CONCAT) ;
                atomlu=GetRightLex ();
                break;
#endif
            case IDNTER:
#if 0
	      printf("%s: IDNTER call newnterm (%s:%d)\n",__FUNCTION__,inFilename,lineNo);
#endif
                CreateNewNTERM ();
                GenDualNodeSem(CONCAT) ;
                atomlu=GetRightLex ();
                break;
#ifdef _FOR_GAS
            case EXPR:
                CreateNewEXPR ();
                GenDualNodeSem(CONCAT) ;
                atomlu=GetRightLex ();
                break;
#endif
            case ELTER:
                CreateNewTERM ();
                GenDualNodeSem(CONCAT) ;
                atomlu=GetRightLex ();
                break;
            default:
                /* Do we need an internal error here or not? */
                break;
        }
    }
}


void F ()
{
    switch (atomlu)
    {
#ifdef _FOR_GAS
        case EXPR:
            CreateNewEXPR ();
            break;
#endif
        case IDNTER:
#if 0
	  printf("%s: IDNTER call newnterm (%s:%d)\n",__FUNCTION__,inFilename,lineNo);
#endif
            CreateNewNTERM ();
            break;
        case ELTER:
            CreateNewTERM ();
            break;
        case LEXEM:
            CreateNewLexem (atomlu) ;
            break;
#ifdef _FOR_GAS
        case UCSTEXP:
	    CreateNewCstExpr();
            break;
#else
        case UCSTEXP:
        case SCSTEXP:
        case RANGE:
            CreateNewNumVal (atomlu) ;
            break ;
#endif
        default:
            DisplayErrorMessage() ;
            return ;
    }
    atomlu=GetRightLex ();
}
