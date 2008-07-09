#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "libUtils.h"

#include "gensyn_version.h"
#include "gensyn_option.h"
#include "gensyn_error.h"

#include "gensyn_defs.h"
#include "arbsem.h"
/*#include "gencode.h"*/
#include "table.h"
#include "encinfo.h"
#include "format.h"
#include "lname.h"
#include "groups.h"

#define _FOR_GAS

#define MAX_SEM_STACK_SIZE 600

typedef struct {
	noeudSEM *Pile[MAX_SEM_STACK_SIZE];
	int nbelem;
} s_pileSEM;

typedef struct {
	noeudSEM *Arbre;
	char	 NomAction[MAX_SEM_LEN];
} s_ACT;

typedef struct {
	s_ACT *Table;
	int nbelem;
} s_tabACT;

/* Globals */
time_t current_time = 0;
char   datestr[64]  = {0};

/* Statics */
static noeudSEM *Axiome=NULL;
static s_pileSEM PileSEM;
s_tabACT  TabACT;

noeudSEM *GetAxiome ()
{
	return Axiome;
}

void SetPopSemIndex (noeudSEM *Racine, int index)
{
	Racine->popSemIndex = index ;
}

void SetGauche (noeudSEM *Racine, noeudSEM *fg)
{
	Racine->fg=fg;
}

void SetDroite (noeudSEM *Racine, noeudSEM *fd)
{
	Racine->fd=fd;
}

void InitTab ()
{
	TabACT.nbelem=0;
}

static int currInd ;

static void SetPrefixIndex(noeudSEM *Arbre)
{
    if (Arbre!=NULL)
    {
		Arbre->semIndex = currInd ++ ;
		SetPrefixIndex (Arbre->fg) ;
		SetPrefixIndex (Arbre->fd) ;
    }
}

/* fonction d'affichage en infixe de chaque arbre associe a une action */

static int node_idx;

static void PrintNodeSem (FILE *outFile, noeudSEM *Arbre, int firstCall )
{
    int index ;
    if (Arbre!=NULL) {
      if (!firstCall) {
	fprintf (outFile, ", \t/* %x */", node_idx++) ;
      }
      fprintf (outFile, "\n\t{ STA_") ;
      switch (Arbre->action) {
      case OPTION:
	fprintf (outFile, "OPT") ;
	index = Arbre->fg->semIndex ;
	fprintf (outFile, ", 0x%x", index) ;
	break ;
      case REPEAT:
	fprintf (outFile, "REP") ;
	index = Arbre->fg->semIndex ;
	fprintf (outFile, ", 0x%x", index) ;
	break ;
      case XOR:
	fprintf (outFile, "X");
      case OR:
	fprintf (outFile, "OR");
	index = (Arbre->fg->semIndex << 16) | Arbre->fd->semIndex ;
	fprintf (outFile, ", 0x%x", index) ;
	break;
      case CONCAT:
	fprintf (outFile, "CON");
	index = (Arbre->fg->semIndex << 16) | Arbre->fd->semIndex ;
	fprintf (outFile, ", 0x%x", index) ;
	break;
      case SPACE:
	fprintf (outFile, "SPA");
	index = (Arbre->fg->semIndex << 16) | Arbre->fd->semIndex ;
	fprintf (outFile, ", 0x%x", index) ;
	break;
      case AND:
	fprintf (outFile, "AND");
	index = (Arbre->fg->semIndex << 16) | Arbre->fd->semIndex ;
	fprintf (outFile, ", 0x%x", index) ;
	break;
#ifdef _FOR_GAS
      case EXPR:
	fprintf (outFile, "EXP, %d", GETIND(*Arbre->token));
	break ;
#endif
      case IDNTER:
	fprintf (outFile, "NTR, %d", GETIND(*Arbre->token));
	break ;
      case ELTER:
	fprintf (outFile, "TER, %d", GETIND(*Arbre->token));
	break ;
      case RANGE:
	fprintf (outFile, "RNG, %s", GETSTR(*Arbre->token));
	break ;
      case UCSTEXP:
	fprintf (outFile, "UCE, %d", GETIND(*Arbre->token));
	break ;
      case SCSTEXP:
	fprintf (outFile, "SCE, %s", GETSTR(*Arbre->token));
	break ;
      case LEXEM:
	fprintf (outFile, "LEX, LEX_%s", GETSTR(*Arbre->token));
	break ;
      default:
	utilsPrintInternalError(GENSYN_INTERNALERROR_SYNTAXNODEUNKNOWN);
	break;
      }
      
      fprintf (outFile, ", %d", Arbre->encIndex) ;
      fprintf (outFile, ", %d", Arbre->popSemIndex) ;
      fprintf (outFile, "}") ;
      
      PrintNodeSem (outFile, Arbre->fg, 0);	
      PrintNodeSem (outFile, Arbre->fd, 0);
    }
}

#if 0
/* fonction d'affichage de la table des Action */
/* procedure generant le code du header de l'analyseur : analyse.h */

static void GenCodeH (FILE *hfd)
{
	fprintf (hfd, "#ifndef  ANALYSE_H\n");
	fprintf (hfd, "#define  ANALYSE_H\n\n");
	fprintf (hfd, "int erreur;\n");
	fprintf (hfd, "int NumLigne;\n");
	fprintf (hfd, "extern void IdentLex(char *, int, int);\n");
	fprintf (hfd, "extern void Error();\n");
	fprintf (hfd, "extern void IdentSpace();\n");
	fprintf (hfd, "extern void SkipSpace();\n");
	fprintf (hfd, "extern void PushContext();\n");
	fprintf (hfd, "extern void PopContext();\n");
	fprintf (hfd, "extern void RestoreContext();\n");
	fprintf (hfd, "\n#endif\n");
	fclose (hfd);
}

#endif

void set_current_time( void ) {
  struct   tm * currtm;
  
  assert(current_time == 0);
  time(&current_time);

  /* Create history string for automatically generated files */
  currtm = localtime(&current_time);
  strftime(datestr,sizeof(datestr),"%y/%m/%d - Automatically generated",currtm);
}

/* Emit extension connection interface with the parser. */
static void
emitExtInterface(FILE *cfd) {

  const char *if_func = 
    "\n"
    "/* Abstract names definition */\n"
    "#ifndef DEF_EXT_LONG_NAME\n"
    "#define DEF_EXT_LONG_NAME      \"Extension Long Name\"\n"
    "#endif\n"
    "#ifndef DEF_EXT_SHORT_NAME\n"
    "#define DEF_EXT_SHORT_NAME     \"Extension Short Name\"\n"
    "#endif\n"
    "\n"
    "/* Extension port Id string identifier */\n"
    "#ifndef DEF_EXT_ID_STR\n"
    "#define DEF_EXT_ID_STR \"??\"\n"
    "#endif\n"
    "\n"
    "/* Extension relocation number */\n"
    "#ifndef DEF_RELOC_NR\n"
    "#define DEF_RELOC_NR 0\n"
    "#endif\n"
    "\n"
    "/* Default value for DLL versioning */\n"
    "#ifndef V_MA\n"
    "#define V_MA 0\n"
    "#endif\n"
    "#ifndef V_MI\n"
    "#define V_MI 0\n"
    "#endif\n"
    "#ifndef V_BU\n"
    "#define V_BU 0\n"
    "#endif\n"
    "\n"
    "/* Default value for Architecture versioning */\n"
    "#ifndef A_MA\n"
    "#define A_MA 0\n"
    "#endif\n"
    "#ifndef A_MI\n"
    "#define A_MI 0\n"
    "#endif\n"
    "#ifndef A_BU\n"
    "#define A_BU 0\n"
    "#endif\n"
    "\n"
    "#define DEF_EXT_VERSION_1(x,y,z) #x \".\" #y \"-\" #z \" \"\n"
    "#define DEF_EXT_VERSION_0(x,y,z) DEF_EXT_VERSION_1(x,y,z)\n"
    "\n"
    "/* Version number is V_MA.V_MI-V_BU */\n"
    "#define DEF_EXT_VERSION DEF_EXT_VERSION_0(V_MA,V_MI,V_BU)\n"
    "\n"
    "/* Architecture number is V_MA.V_MI.V_BU */\n"
    "#define DEF_EXT_ARCHI_VERSION DEF_EXT_VERSION_0(A_MA,A_MI,A_BU)\n"
    "\n"
    "/* Version mode is release or development */\n"
    "#ifndef __RELEASE__\n"
    "#define DEF_EXT_MODE       \"Dev\"\n"
    "#else\n"
    "#define DEF_EXT_MODE       \"Rel\"\n"
    "#endif\n"
    "\n"
    "#define DEF_EXT_SHORT_ABSTRACT \" \" DEF_EXT_LONG_NAME \" Assembler Dynamic Library\"\n"
    "#define DEF_EXT_ABSTRACT       DEF_EXT_LONG_NAME \" assembler \" DEF_EXT_MODE \" \" \\\n"
    "DEF_EXT_VERSION \"- Architecture \" DEF_EXT_ARCHI_VERSION \" - Port \" DEF_EXT_ID_STR \\\n"
    "\"\\nLast build: \" __DATE__ \" \" __TIME__ \"\\000\"\n"
    "\n"
    "const char *const sccsid =\n"
    "\"@(#)STMicroelectronics \" DEF_EXT_LONG_NAME \" assembler shared object\\n@(#)\"\n"
    "\"Rel \" DEF_EXT_VERSION \" - Architecture \" DEF_EXT_ARCHI_VERSION \" - Port \"\n"
    "DEF_EXT_ID_STR \" (\" __DATE__ \" / \" __TIME__ \")\";\n"
    "\n"
    "/*-------------------------------------------------------------------------*/\n"
    "/*\n"
    "Routine PARSER_GetParser\n"
    "\n"
    "Description : connect a parser.\n"
    "............................................................................\n"
    "*/\n"
    "\n"
    "BE_EXPORTED PARSER_ReturnT PARSER_GetParser (PARSER_InstanceT *Ext) {\n"
    "\n"
    "  if (NULL != Ext) {\n"
    "    Ext->Version_key         = PARSER_Version;         /* Version for coherency checking  */\n"
    "    Ext->Version_str         = DEF_EXT_VERSION;        /* Version string of extension     */\n"
    "    Ext->Abstract            = DEF_EXT_ABSTRACT;       /* Description of extension (long) */\n"
    "    Ext->ShortAbstract       = DEF_EXT_SHORT_ABSTRACT; /* Description of extension (short)*/\n"
    "    Ext->ExtConnect          = ExtensionConnect;       /* Connect Extension Analyzer      */\n"
    "    Ext->Name                = DEF_EXT_SHORT_NAME;     /* Extension specific name (lit.)  */\n"
    "    Ext->UseMsg              = NULL;                   /* How to use it.                  */\n"
    "    Ext->MagicKey            = MagicKey;               /* Unique identifier               */\n"
    "    Ext->RelocRangeStart     = 0;                      /* Offset value for 1st relocation */\n"
    "    Ext->RelocationNr        = DEF_RELOC_NR;           /* Relocation number               */\n"
    "    Ext->Port                = ExtPort;                /* Ports used                      */\n"
    "    Ext->PortSize = sizeof(ExtPort)/sizeof(ExtPort[0]);/* Number of entries in Ext->Port  */\n"
    "\n"
    "    /* Shall be filled-in by caller */\n"
    "    Ext->STRASS_AXIOM             = NULL;\n"
    "    Ext->get_reg_class            = NULL;\n"
    "    Ext->get_lit_class            = NULL;\n"
    "    Ext->GetOperand               = NULL;\n"
    "    Ext->GetOps                   = NULL;\n"
    "    return PARSER_OK;\n"
    "  }\n"
    "  return PARSER_ERR_FATAL;\n"
    "}\n";

  fprintf(cfd,"%s",if_func);
}

/* procedure generant le code de l'analyseur syntaxique *
 * associe a la grammaire definie par l'utilisateur     */

void GenCode (FILE *cfd, char *cname) {
    int i ;
#ifdef __TOOLKIT__
    PrintCopyright(cfd,PROCESSOR " reconfigurable toolset",PROCESSOR " extension assembler",cname,"Finite State Machine that is an extension add-on of assembler syntax parser",datestr);
#else
    PrintPurposeHistory(cfd,"Finite State Machine that is an assembler syntax parser",datestr);
#endif
    if (OutputType == OUT_FOR_EXTENSION) {
       fprintf (cfd, "#include \"air.h\"\n\n") ;
       fprintf (cfd, "#include \"parser.h\"\n\n") ;
       if (utilsOptionIsDefined ( & gensyn_option.rctype ) && !strcmp(utilsOptionGetScalar( & gensyn_option.rctype ),"corxpert")) {
          GENERATE_INCLUDE_SEQUENCE(cfd,"id")
       }
    } else {
      fprintf(cfd, "#include \"air.h\"\n");
      fprintf(cfd, "#include \"parser.h\"\n");
      fprintf(cfd, "#include \"type_defs.h\"\n");
      fprintf(cfd, "#include \"defs_exported.h\"\n");
      fprintf(cfd, "#include \"astype.h\"\n");
      fprintf(cfd, "#include \"analyzer.h\"\n\n") ;
    }

    if (OutputType == OUT_FOR_EXTENSION) {
#ifdef __TOOLKIT__
       if (utilsOptionIsDefined ( & gensyn_option.rctype ) && !strcmp(utilsOptionGetScalar( & gensyn_option.rctype ),"corxpert")) {
          fprintf( cfd, "static PARSER_u32T MagicKey = %s_XID;\n\n", utilsOptionGetScalar( & gensyn_option.short_name ));
//          fprintf( cfd, "static PARSER_u32T XVersion = %s_XVR;\n\n", utilsOptionGetScalar( & gensyn_option.short_name ));
       } else {
          fprintf( cfd, "static PARSER_u32T MagicKey = %#08x;\n\n", (int)current_time);
//          fprintf( cfd, "static PARSER_u32T XVersion = 0;\n\n");
       }
#else
       fprintf( cfd, "static PARSER_u32T MagicKey = 0;\n\n");
//       fprintf( cfd, "static PARSER_u32T XVersion = 0;\n\n");
#endif
    } else {
#ifdef __TOOLKIT__
       if (utilsOptionIsDefined ( & gensyn_option.rctype ) && !strcmp(utilsOptionGetScalar( & gensyn_option.rctype ),"corxpert")) {
          fprintf( cfd, "PARSER_u32T MagicKey = %s_XID;\n\n",utilsOptionGetScalar( & gensyn_option.short_name ));
//          fprintf( cfd, "PARSER_u32T XVersion = %s_XVR;\n\n",utilsOptionGetScalar( & gensyn_option.short_name ));
       } else {
          fprintf( cfd, "PARSER_u32T MagicKey = %#08x;\n\n",(int)current_time);
//          fprintf( cfd, "PARSER_u32T XVersion = 0;\n\n");
       }
#else
       fprintf( cfd, "PARSER_u32T MagicKey = 0;\n\n");
//       fprintf( cfd, "PARSER_u32T XVersion = 0;\n\n");
#endif
    }

    PrintTermTable (cfd,"static SXAS") ;
    AssignTermIndex() ;
    AssignNonTermIndex() ; 
    for (i=0; i<TabACT.nbelem; i++) {
		currInd = 0;
		SetPrefixIndex(TabACT.Table[i].Arbre) ;
		fprintf (cfd, "static PARSER_nodeSemT NTR_%s[] = {", TabACT.Table[i].NomAction);
		node_idx = 0;
		PrintNodeSem (cfd, TabACT.Table[i].Arbre, 1);
		fprintf (cfd, " \t/* %x */", node_idx++) ;
		fprintf (cfd, "\n} ;\n\n");
    }
    PrintNonTermTable (cfd) ;	
    PrintVarTable(cfd) ;
    PrintFormatFields(cfd) ;
    PrintFormatType(cfd) ;
    PrintFormatTable(cfd) ;
    PrintClassList(cfd) ;
    PrintGroupings(cfd) ;
    PrintGroupingTable(cfd) ;
    DisplayEncFunc(cfd) ;
    if (GetInit() != -1) {
      fprintf (cfd, "\n\nstatic void STRASSInit ( void * Instance )\n{\n") ;
      fprintf (cfd, "\tSTRASSEncode(Instance,%d) ;\n", GetInit()) ;
      fprintf (cfd, "}\n") ;
    }
    if (OutputType == OUT_FOR_CORE) {
      fprintf (cfd, "\n\nvoid CoreConnect ( PARSER_nodeSemT *** _semTable,\n");
      fprintf (cfd, "\tPARSER_u16T * _semTableSize, PARSER_stringT ** _stringTable,\n");
      fprintf (cfd, "\tPARSER_u16T * _stringTableSize, PARSER_stringT * _targetString,\n");
      fprintf (cfd, "\tPARSER_InitT * _STRASSInit, PARSER_EncodeT * _STRASSEncode) {\n");
      fprintf (cfd, "\t* _semTable         = &semTable[0];\n");
      fprintf (cfd, "\t* _semTableSize     = semTableSize;\n");
      fprintf (cfd, "\t* _stringTable      = &stringTable[0];\n");
      fprintf (cfd, "\t* _stringTableSize  = stringTableSize;\n");
      fprintf (cfd, "\t* _targetString     = targetString;\n");
      fprintf (cfd, "\t* _STRASSInit       = &STRASSInit;\n");
      fprintf (cfd, "\t* _STRASSEncode     = &STRASSEncode;\n");
      fprintf (cfd, "}\n");
   }
   if (OutputType == OUT_FOR_EXTENSION)
   {
     fprintf (cfd, "\n\nstatic void ExtensionConnect (PARSER_nodeSemT *** _semTable,\n");
     fprintf (cfd, "\tPARSER_u16T * _semTableSize,\n");
     fprintf (cfd, "\tPARSER_stringT ** _stringTable,\n");
     fprintf (cfd, "\tPARSER_u16T * _stringTableSize,\n");
     fprintf (cfd, "\tPARSER_InitT * _STRASSInit,\n");
     fprintf (cfd, "\tPARSER_EncodeT * _STRASSEncode ) {\n");
     fprintf (cfd, "\t* _semTable         = &semTable[0];\n");
     fprintf (cfd, "\t* _semTableSize     = semTableSize;\n");
     fprintf (cfd, "\t* _stringTable      = &stringTable[0];\n");
     fprintf (cfd, "\t* _stringTableSize  = stringTableSize;\n");
     fprintf (cfd, "\t* _STRASSInit       = &STRASSInit;\n");
     fprintf (cfd, "\t* _STRASSEncode     = &STRASSEncode;\n");
     fprintf (cfd, "}\n");
#ifndef __TOOLKIT__
      fprintf (cfd, "#ifndef DEF_EXT_LONG_NAME\n");
      if (NULL==utilsOptionGetScalar( & gensyn_option.long_name )) {
         fprintf (cfd, "#define DEF_EXT_LONG_NAME \"(null)\"\n");
      } else {
         fprintf (cfd, "#define DEF_EXT_LONG_NAME \"%s\"\n", utilsOptionGetScalar( & gensyn_option.long_name ) );
      }
      fprintf (cfd, "#endif\n");
      fprintf (cfd, "#ifndef DEF_EXT_SHORT_NAME\n");
      if (NULL==utilsOptionGetScalar( & gensyn_option.short_name )) {
         fprintf (cfd, "#define DEF_EXT_SHORT_NAME \"(null)\"\n");
      } else {
         fprintf (cfd, "#define DEF_EXT_SHORT_NAME \"%s\"\n", utilsOptionGetScalar( & gensyn_option.short_name ) );
      }
      fprintf (cfd, "#endif\n");
#endif
      emitExtInterface(cfd);
   }

}

/* fonction recherchant l'action nom dans la table des Actions             *
 * retourne un pointeur vers l'arbre associee et met a jour la variable    *
 * d'indice dans le cas ou l'action existe                                 *
 * sinon retourne NULL et donne l'indice ou l'action nom doit etre inseree */

noeudSEM *RechAct (char *nom, int *ind)
{
	int cmp, deb, mil, fin, pl;

	deb=0;
	fin=TabACT.nbelem-1;
	while (deb<=fin) {
		mil=(deb+fin)/2;
		cmp=strcmp(nom, TabACT.Table[mil].NomAction);
		if (cmp<=0) 
			fin = mil-1;
		if (cmp>=0)
			deb = mil+1;
	}
	pl=fin+1;
	*ind=pl;
	if ((TabACT.nbelem!=pl)&&(!strcmp(nom, TabACT.Table[pl].NomAction)))
		return TabACT.Table[pl].Arbre;
	else
		return NULL;
}

/* fonction d'ajout d'une action dans la table des actions       *
 * si celle-ci a deja ete definie on construit un nouvel arbre   *
 * en rajoutant un OU entre l'ancienne definition et la nouvelle */

void AjoutAct (char *nom, noeudSEM *Action)
{
    int ind, nb;
    noeudSEM *fg, *fd, *rac;

    if (RechAct(nom, &ind))
	{
		fg=TabACT.Table[ind].Arbre; 
		if (Action->action == EMPTY)
		{
			Action->action = OPTION ;
			SetGauche (Action, fg);
			TabACT.Table[ind].Arbre = Action ;
		}
		else if (fg->action == EMPTY) 
		{
			fg->action = OPTION ;
			SetGauche (fg, Action);
		}
		else
		{
			fd=Action;
			rac=GenSEM (OR, NULL);
			SetGauche (rac, fg);
			SetDroite (rac, fd);
			TabACT.Table[ind].Arbre=rac;
		}
    }
    else
	{
		if (Axiome==NULL)
			Axiome=Action;
		nb=++TabACT.nbelem;
		TabACT.Table=(s_ACT *)realloc(TabACT.Table, nb*sizeof(s_ACT));
		while ((--nb)>ind) 
			TabACT.Table[nb]=TabACT.Table[nb-1];
    	TabACT.Table[ind].Arbre = Action; 
    	strcpy(TabACT.Table[ind].NomAction, nom); 
    }
}

void InitPile ()
{
	PileSEM.nbelem=0;
}

void EmpilerSEM (noeudSEM *Elem)
{
	if (PileSEM.nbelem<MAX_SEM_STACK_SIZE-1)
	{
		PileSEM.Pile[PileSEM.nbelem++]=Elem;
	}
	else
	{
		utilsPrintInternalError(GENSYN_INTERNALERROR_STACKFULL);
	}
}

noeudSEM *DepilerSEM()
{
	noeudSEM *aux=NULL;

	PileSEM.nbelem--;
	if (PileSEM.nbelem>=0)
	{
		aux=PileSEM.Pile[PileSEM.nbelem]; 
	}
	else
	{
		utilsPrintInternalError(GENSYN_INTERNALERROR_STACKEMPTY);
	}
	return aux;
}

/* fonction de creation d'un nouveau noeud de l'arbre semantique */

noeudSEM *GenSEM(ATOME lexeme, s_StrInd *val)
{
	noeudSEM *aux;

	aux=(noeudSEM *) malloc (sizeof (noeudSEM));
	aux->action=lexeme;
	aux->token = val ;
	aux->semIndex=-1 ;
	aux->encIndex=-1 ;
	aux->popSemIndex = 0 ;
	aux->fg=NULL;
	aux->fd=NULL;

	return aux;
}

void SetEncInfo (noeudSEM *T, int encIndex)
{
	T->encIndex = encIndex ;
}
