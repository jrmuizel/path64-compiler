
#include "air.h"
#include "parser.h"
#include "type_defs.h"
#include "defs_exported.h"
#include "astype.h"
#include "analyzer.h"
#include "ext_engine.h"
#include "utils_print.h"
#include "alloc.h"
#include "opndstck.h"
#include "ctxstck.h"

#include "targ_isa_subset.h"
#include "targ_isa_parse.h"

static s_encodingStack paramStack;
static int numExprProcessed = 0;
static int popVal ;
static AIR_OPS *ops = NULL; /* AIR's instructions list for a given assembly instruction. */

#define TRACE_PINFO

AIR_TN *GetOperand(const char *file, int line, PARSER_u32T position);
AIR_OPS *GetOps();

void SetCallbacks ( PARSER_InstanceT * Parser ) {
  /* Fill-in pointers */
  Parser->STRASS_AXIOM=&STRASS_AXIOM;
  /* Fill-in callbacks */
  Parser->GetOperand = GetOperand;
  Parser->GetOps = GetOps;

  /* AIR minimal stuff to be able to build AIR_OP */
  Parser->air_new_tn_reg = AIR_new_tn_reg;
  Parser->air_new_tn_enum = AIR_new_tn_enum;
  Parser->air_new_tn_imm = AIR_new_tn_imm;
  Parser->air_ops_append_op = AIR_OPS_Append_Op;
  Parser->air_mk_full_op = AIR_mk_full_op;
  Parser->top_name = TOP_Name;
}

static void ProcessInfo(s_encodingInfo *sEncInfo) {
#ifdef PARSER_DEBUG
  char label[255] ;
#endif
  switch (sEncInfo->type) {
  case REGISTER:
#ifdef PARSER_DEBUG
    strncpy (label, sEncInfo->string.buf, sEncInfo->string.length) ;
    label[sEncInfo->string.length] = '\0' ;
    printf ("Register(%s)\n", label) ;
#endif
    PushEncodingInfo(&paramStack, sEncInfo) ;
    break ;
  case LABEL:
#ifdef PARSER_DEBUG
    strncpy (label, sEncInfo->string.buf, sEncInfo->string.length) ;
    label[sEncInfo->string.length] = '\0' ;
    printf ("Label(%s)\n", label) ;
#endif
    PushEncodingInfo(&paramStack, sEncInfo) ;
    break ;
  case CSTEXPR:
#ifdef PARSER_DEBUG
    printf ("Expr(%d)\n", numExprProcessed) ;
#endif
    sEncInfo->index = numExprProcessed++ ;
    PushEncodingInfo(&paramStack, sEncInfo) ;
    break ;
  case HEXCONST:
  case DECCONST:
  case BINCONST:
#ifdef PARSER_DEBUG
    strncpy (label, sEncInfo->string.buf, sEncInfo->string.length) ;
    label[sEncInfo->string.length] = '\0' ;
    printf ("Constant(%s)\n", label) ;
#endif
    PushEncodingInfo(&paramStack, sEncInfo) ;
    break ;
  case ENCODE:
#ifdef PARSER_DEBUG
    printf ("STRASSEncode_Glb(%d), Pop(%d)\n", sEncInfo->action, sEncInfo->numToken) ;
#endif
    popVal = sEncInfo->numToken ;
    STRASSEncode_Glb (currParserInstance,sEncInfo->action) ;
    DecrEncodingStack(&paramStack, popVal) ;
    break ;		
  }
}

AIR_TN *GetOperand(const char *file, int line, PARSER_u32T position) {
  s_encodingInfo *sEncInfo ;
  AIR_TN *opnd = NULL;
  int i;

  sEncInfo = GetEncInfoIndex(file,line,&paramStack, paramStack.size-popVal+position) ;
  switch (sEncInfo->type) {
  case REGISTER:
    opnd = AIR_new_tn_reg(ACTION_get_main_class(sEncInfo),ACTION_get_sub_class(sEncInfo),ACTION_get_opnd(sEncInfo));
    break;
  case CSTEXPR:
    if(ACTION_is_relocation(sEncInfo)) {
      opnd = AIR_new_tn_reloc ( ISA_RELOC_UNDEFINED, ACTION_get_sym_id(sEncInfo),
				ACTION_get_symop_id(sEncInfo),ACTION_get_opnd(sEncInfo),ACTION_get_expr(sEncInfo));
    }
    else {
      opnd = AIR_new_tn_imm(ACTION_get_opnd(sEncInfo));
    }
    break;
  default:
    utilsPrintInternalError("IE012","Not treated operand type (%d)\n",sEncInfo->type);
  }
  return opnd;
}

AIR_OPS *GetOps() {
  return ops;
}

static void
InitCurrentParser( PARSER_InstanceT *parser ) {
  currParserInstance = parser;
  STRASSInit_Glb(currParserInstance);
  CheckAxiom() ;
  /* setCallback is called during parser initialization in ParserList */
}

PARSER_ReturnT
PARSER_Initialize(PARSER_parse_registerT _parse_register,
		  PARSER_parse_expressionT _parse_expression,
		  PARSER_fatal_errorT _fatal_error,
		  PARSER_errorT _error,
		  PARSER_warningT _warning,
		  char *usual_path_to_search_dll) {
  ops = NULL;

  InitializeAnalyzer() ;

  PARSER_parse_register = _parse_register;
  PARSER_parse_expression = _parse_expression;
  PARSER_fatal_error = _fatal_error;
  PARSER_error = _error;
  PARSER_warning = _warning;
  PARSER_usual_path = usual_path_to_search_dll;

  return PARSER_OK;
}

void
PARSER_ConnectParser ( char * Name )
{
  int find_static_subset = 0;
  ISA_SUBSET subset;
  for(subset = ISA_SUBSET_MIN; subset < ISA_SUBSET_count; subset++) {
    if(strcasecmp(Name,ISA_SUBSET_Name(subset)) == 0) {
      PARSER_ParserT    *ParserLib = (PARSER_ParserT *) malloc (sizeof(PARSER_ParserT));
      PARSER_GetParserT GetParser = ISA_PARSE_GetParser(subset);
      const char *ErrMsg = 0;
      memset(ParserLib,0,sizeof(PARSER_ParserT));
      if(NULL == GetParser || PARSER_OK != GetParser(&(ParserLib->Xi))) {
	PARSER_fatal_error("Unable to load parser for subset %s\n",ISA_SUBSET_Name(subset));
      }
      if (0==ParserLib->Xi.Version_key)      ErrMsg="Bad version number";
      if (0==ParserLib->Xi.Version_str)      ErrMsg="Unable to find out version";
      if (NULL==ParserLib->Xi.Abstract)      ErrMsg="No abstract available";
      if (NULL==ParserLib->Xi.ShortAbstract) ErrMsg="No short abstract available";
      if (NULL==ParserLib->Xi.ExtConnect)    ErrMsg="No assembling entry";
      if (NULL != ErrMsg) {
	PARSER_fatal_error("%s",ErrMsg);
      }
      else {
	int NextAvailable;
	int i;
      
	for (NextAvailable=0;(NextAvailable<MAX_PARSER_NR) && (NULL!=ParserList[NextAvailable]);NextAvailable++) {
	  for (i=0;(i<ParserList[NextAvailable]->Xi.PortSize) && (i<ParserLib->Xi.PortSize);i++) {
            if ((ParserList[NextAvailable]->Xi.Port[i] & ParserLib->Xi.Port[i]) && 
                (
                 (ParserList[NextAvailable]->Xi.PortSize!=ParserLib->Xi.PortSize)
                 ||
                 (0!=memcmp(ParserList[NextAvailable]->Xi.Port,ParserLib->Xi.Port,
                            ParserList[NextAvailable]->Xi.PortSize))
                 ||
                 (0!=strcmp(ParserList[NextAvailable]->Name,ParserLib->Xi.Name))
		 )
		)
	      {
		PARSER_ParserT * FormerLib;
               
		PARSER_warning("[%s]: takes precedence on [%s] for port %d\n",
			       ParserLib->Xi.Name,ParserList[NextAvailable]->Name,i);

		/* New connexion wins. Former lib of this port is disconnected */
		FormerLib = ParserList[NextAvailable];
		ParserList[NextAvailable] = ParserLib;
		SetCallbacks(&ParserLib->Xi);
	        ParserLib->Name = (char *)malloc(strlen(Name)+1);
	        strcpy(ParserLib->Name,Name);
		if (!ISA_SUBSET_LIST_Contains(ISA_SUBSET_List,subset)) {
		  ISA_SUBSET_LIST_Add(ISA_SUBSET_List,subset);
		}

		/* Free former parser */
		free(FormerLib->Name);
		free(FormerLib);
		return;
	      }
	  }
	}
	if (NextAvailable==MAX_PARSER_NR) {
	  PARSER_fatal_error("[%s]: No more extension slot available\n",ParserLib->Name);
	  free(ParserLib);
	  return;
	}
	ParserList[NextAvailable] = ParserLib;
	SetCallbacks(&ParserLib->Xi);
	ParserLib->Name = (char *)malloc(strlen(Name)+1);
	strcpy(ParserLib->Name,Name);
	if (!ISA_SUBSET_LIST_Contains(ISA_SUBSET_List,subset)) {
	  ISA_SUBSET_LIST_Add(ISA_SUBSET_List,subset);
	}
	return;
      }
    }
  }
  PARSER_fatal_error("Unable to find parser for %s", Name);
}

void
PARSER_Finalize( void ) {
  FinalizeAnalyzer() ;
}

PARSER_ReturnT
PARSER_parse(char *buffer, AIR_OPS *_ops) {
  s_encodingStack *stack = GetEncStack();
  int i;
  int ParserId;
  int failed = 1;

  /* Then loop on connected extensions */
  /* ParserId 0 correspond to the core, others
   *   must be core extensions.
   */
  ParserId = 0;
  while (failed && (ParserId<MAX_PARSER_NR)) {
#ifdef PARSER_DEBUG
    printf ("Parser %d ",ParserId);
#endif
    if (NULL!=ParserList[ParserId]) {
#ifdef PARSER_DEBUG
      printf ("CONNECTED. Trying...(parser %s)\n", ParserList[ParserId]->Name);
#endif
      currParserInstance = &(ParserList[ParserId]->Xi);
      currParserInstance->ExtConnect(&semTable_Glb,&semTableSize_Glb,&stringTable_Glb,&stringTableSize_Glb,
				  &STRASSInit_Glb,&STRASSEncode_Glb);
      InitCurrentParser( currParserInstance );
      failed = AnalyzeString(buffer);
    } else {
#ifdef PARSER_DEBUG
      printf ("PORT %d NOT CONNECTED.\n",ParserId);
#endif
    }
    ParserId ++;
  }
  ops = _ops;
  if(failed) { return PARSER_ERR_NONFATAL; }
  numExprProcessed = 0;
  InitEncodingStack(&paramStack) ;
  for (i=0; i<stack->size; i++) {
    ProcessInfo(GetEncInfoIndex("ProcessInfo",0,stack, i)) ;
  }
  return PARSER_OK;
}

PARSER_u32T
PARSER_error_index( void ) {
  return GetErrorIndex();
}

PARSER_ParserT * PARSER_GetFeatures ( char * Name ) {
  int NextParser;
  
  for (NextParser=0;(NextParser<MAX_PARSER_NR)&&(NULL!=ParserList[NextParser]);NextParser++) {
    if(!strcasecmp(ParserList[NextParser]->Name,Name)) {
      return ParserList[NextParser];
    }
  }
  return NULL;
}

const char *PARSER_eState_Name[] = {
    "STA_CON",
    "STA_SPA",
    "STA_AND",
    "STA_OR",
    "STA_XOR",
    "STA_TER",
    "STA_NTR",
    "STA_EXP",
    "STA_UCE",
    "STA_SCE",
    "STA_RNG",
    "STA_LEX",
    "STA_OPT",
    "STA_REP",
    "STA_OPD" 
};
