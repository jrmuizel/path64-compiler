/*###INCLUDE###<copyright.in>
 * Purpose          : Header file describing Parser Dynamic Library           *
 *                    Interface for assembler.                                *
 * History          : 05/02/08 - First implementation.                        *
 *----------------------------------------------------------------------------*/
#ifndef __PARSER_H__
#define __PARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#define PARSER_DEBUG
#endif

#include <stdarg.h>

/*------------------------------------------------------------------*
 * Version of PARSER, ensures coherency between DLL and executable *
 *------------------------------------------------------------------*/
#define PARSER_Version  (0x051123A)

#ifndef JUST_WANT_API_VERSION
/*----------------*
 * Standard types *
 *----------------*/
typedef int                 PARSER_boolT;
typedef char*               PARSER_stringT;
typedef char                PARSER_s8T;
typedef unsigned char       PARSER_u8T;
typedef short               PARSER_s16T;
typedef unsigned short      PARSER_u16T;
typedef int                 PARSER_s32T;
typedef unsigned int        PARSER_u32T;
#if defined(WIN32) && !defined(__MINGW32__)
typedef __int64             PARSER_s64T;
typedef unsigned __int64    PARSER_u64T;
#else
typedef long long           PARSER_s64T;
typedef unsigned long long  PARSER_u64T;
#endif
typedef int                 PARSER_INT64[2];

/*---------------------------*
 * Lexer automaton terminals *
 *---------------------------*/
typedef enum  {
	LEX_add_sub_op,      /* Add/Sub operator */
	LEX_and_op,
	LEX_cmp_op,
	LEX_v2_4_6,
	LEX_pow2_3_16,
	LEX_mul_shift_op,
	LEX_ident,
	LEX_alpha,
	LEX_int_cst,
	LEX_float_cst,
	LEX_label,
	LEX_toeol,
	LEX_eol,
	LEX_param,
	LEX_or_xor_op,
	LEX_una_op,
	LEX_quoted_string
} PARSER_AssLexT ;

/*-------------------------*
 * Syntax automaton states *
 *-------------------------*/
typedef enum 	{
    STA_CON,	/* Concat Operator: no space allowed between dependent nodes  */
    STA_SPA,	/* Space Operator: 1 space mandatory between dependent nodes  */
    STA_AND,	/* And Operator: 0 ot n spaces between dependent nodes        */
    STA_OR,	/* Or Operator: both operands are evaluated and best matching */
                /* is choosen                                                 */
    STA_XOR,	/* Xor Operator: if first operand is valid then no evaluation */
                /* is performed on second one                                 */
    STA_TER,	/* Term Operand: A Terminal string is expected                */
    STA_NTR,	/* Non Term Operand: A Non Terminal is expected               */
    STA_EXP,	/* Expression that replace instruction operand.               */
    STA_UCE,	/* Unsigned Const Operand: An unsigned constant expression is */
                /* expected                                                   */
    STA_SCE,	/* Signed Const Operand: An signed constant expression is     */
                /* expected                                                   */
    STA_RNG,	/* Range Operand: An index is exepected                       */
    STA_LEX,	/* Lexem Operand: A Lexem is expected                         */
    STA_OPT,	/* Option Operator: node is optional                          */
    STA_REP,	/* Repeat Operator: node can be repeated 0 to n times         */
    STA_OPD     /* Mark an operand that will be recognized by GAS (GNU Assembler) */
} PARSER_eStateT;

/*------------------------------*
 * Syntax automaton state names *
 *------------------------------*/
extern const char *PARSER_eState_Name[];


/*------------------------------------------*
 * Syntax automaton semantic node structure *
 *------------------------------------------*/
typedef struct {
  PARSER_eStateT state;
  PARSER_s32T    info;
  PARSER_s32T    encInfo;
  PARSER_s32T    semInfo;
} PARSER_nodeSemT;

/*-----------------------------*
 * Structure defining a string *
 *-----------------------------*/
typedef struct {
	PARSER_stringT buf ; /* Buffer containing the string itself   */
	PARSER_u32T length ; /* Number of significant bytes in buffer */
} PARSER_StrStringT ;

/*---------------------------*
 * Type for error management *
 *---------------------------*/
typedef enum {
  PARSER_OK,
  PARSER_ERR_NOT_SUPPORTED,
  PARSER_ERR_NONFATAL,
  PARSER_ERR_FATAL
} PARSER_ReturnT;

/*------------------------------------------------*
 * Hook functions supplied by client              *
 *------------------------------------------------*/

  typedef PARSER_ReturnT ( * PARSER_parse_registerT) (char *buffer, char **endptr,
						      PARSER_u32T reg_class, PARSER_u32T reg_subclass,
						      PARSER_u32T *regid);
  typedef PARSER_ReturnT ( * PARSER_parse_expressionT) (char *buffer, char **endptr, PARSER_u32T literal_class,
							PARSER_boolT *is_reloc, void **sym_id, 
							void **symop_id, PARSER_u64T *value, void **expr);

  typedef void ( * PARSER_fatal_errorT) (const char *format, ...);
  typedef void ( * PARSER_errorT) (const char *format, ...);
  typedef void ( * PARSER_warningT) (const char *format, ...);


/*------------------------------------------------*
 * Pointer to functions supplied by parser DLL *
 *------------------------------------------------*/
typedef void (* PARSER_EncodeT)     (void *, PARSER_s32T);
typedef void (* PARSER_InitT)       (void *);
typedef void (* PARSER_ExtConnectT) (PARSER_nodeSemT *** _semTable,
                                     PARSER_u16T       * _semTableSize,
				     PARSER_stringT   ** _stringTable,
                                     PARSER_u16T       * _stringTableSize,
				     PARSER_InitT      * _strassInit,
                                     PARSER_EncodeT    * _strassEncode);

typedef void (* PARSER_Get_Register_ClassT) (char *reg_identifier, ISA_REGISTER_CLASS *rclass, ISA_REGISTER_SUBCLASS *rsclass);
typedef void (* PARSER_Get_Literal_ClassT)  (char *lit_identifier, ISA_LIT_CLASS *lclass);

typedef AIR_TN* (*AIR_new_tn_reg_T) (ISA_REGISTER_CLASS,ISA_REGISTER_SUBCLASS, PARSER_u32T);
typedef AIR_TN* (*AIR_new_tn_enum_T) (ISA_ENUM_CLASS, ISA_ENUM_CLASS_VALUE);
typedef AIR_TN* (*AIR_new_tn_imm_T) (PARSER_u64T);
typedef void    (*AIR_OPS_Append_Op_T) (AIR_OPS *ops, AIR_OP *op);
typedef AIR_OP* (*AIR_mk_full_op_T)  (TOP,mUINT32,...);
typedef const char* (*TOP_Name_T) (TOP topcode);

/*-------------------------------------------------------*
 * Callbacks supplied by assembler core to parser DLL *
 *-------------------------------------------------------*/

typedef AIR_TN *  ( * PARSER_GetOperandT) (const char *file, int line, PARSER_u32T position);
typedef AIR_OPS * ( * PARSER_GetOpsT) ( void );


/*----------------------------------------------------------*
 * Structure exchanged by parser and connected DLLs *
 *----------------------------------------------------------*/
typedef struct PARSER_InstanceT {
  /* Mandatory fields to be filled by parser DLL */
  PARSER_u32T	       Version_key;       /* Version for coherency checking  */
  PARSER_stringT       Version_str;       /* Version of parser               */
  PARSER_stringT       Abstract;          /* Description of parser (long)    */
  PARSER_stringT       ShortAbstract;     /* Description of parser (short)   */
  PARSER_ExtConnectT   ExtConnect;        /* Connect Parser Analyzer         */
  PARSER_u32T          MagicKey;          /* DLL build identifier            */
  PARSER_u32T          RelocRangeStart;   /* Offset value for 1st relocation */
  PARSER_u32T          RelocationNr;      /* Number of relocations           */
  PARSER_boolT         *Port;             /* Parser port connected           */
  PARSER_u32T          PortSize;          /* Number of entries in Port       */
  
  /* Optional fields to be filled by parser DLL */
  PARSER_stringT       Name;              /* Parser specific name (lit.)     */
  PARSER_stringT       UseMsg;            /* How to use it.                  */

  /* Mandatory fields to be filled by assembler core */
  PARSER_nodeSemT      ** STRASS_AXIOM;   /* Axiom of analyzer               */
  PARSER_Get_Register_ClassT get_reg_class;
  PARSER_Get_Literal_ClassT  get_lit_class;

  /* Mandatory callbacks to be filled by assembler core */
  PARSER_GetOperandT   GetOperand;           /* Used in rule's action to get AIR_TN */
  PARSER_GetOpsT       GetOps;               /* Used to get instruction list to fill */

  /* AIR minimal stuff to be able to build AIR_OP */
  AIR_new_tn_reg_T           air_new_tn_reg;
  AIR_new_tn_enum_T          air_new_tn_enum;
  AIR_new_tn_imm_T           air_new_tn_imm;
  AIR_OPS_Append_Op_T        air_ops_append_op;
  AIR_mk_full_op_T           air_mk_full_op;
  TOP_Name_T                 top_name;


} PARSER_InstanceT;

/*----------------------------------------------------------*
 * The only function loaded explicitly in an parser DLL.    *
 * All others are taken directly in the PARSER_InstanceT    *
 * structure after PARSER_GetParser call                    *
 *----------------------------------------------------------*/
typedef PARSER_ReturnT (*PARSER_GetParserT) (PARSER_InstanceT *Parser);

/*-----------------------------------------------*
 *  Definition of array for connected parsers *
 *-----------------------------------------------*/
typedef struct {
  char *             Name;      /* Specific name of parser, -ext cmd line */
                                /* switch value Dynamic Library name to be   */
                                /* loaded.                                   */
  void *             DllHandle; /* Dynamic Library Handle                    */
  PARSER_InstanceT   Xi;        /* Features describing parser once        */
                                /* connected                                 */
} PARSER_ParserT;

/*----------------------*
 * Parser Management *
 *----------------------*/
#define MAX_PARSER_NR 9                           /* Maximum parsers  */
extern PARSER_ParserT   * ParserList[MAX_PARSER_NR]; /* Array of parsers */

extern void PARSER_ConnectDynamicParser ( char * Name );

extern char * targetString_Glb;

extern PARSER_parse_registerT   PARSER_parse_register;
extern PARSER_parse_expressionT PARSER_parse_expression;
extern PARSER_fatal_errorT      PARSER_fatal_error;
extern PARSER_errorT            PARSER_error;
extern PARSER_warningT          PARSER_warning;
extern char                    *PARSER_usual_path;

extern PARSER_EncodeT STRASSEncode_Glb ;
extern PARSER_InitT   STRASSInit_Glb   ;

extern PARSER_InstanceT * currParserInstance;

/*----------------------------------------------*
 * Ensure matching of functions available on    *
 * Solaris hosts, Linux hosts and Windows hosts *
 *----------------------------------------------*/
#if 0
#include "utils_hostdefs.h"
#endif

/*---------------------------------*
 * Macros used in assembler parser *
 * to access fields of structures  *
 *---------------------------------*/
 
/* Setup assembler syntax axiom for an parser */
#define SETAXIOM(AXIOM)					\
  *(PARSER_Instance->STRASS_AXIOM)=&NTR_##AXIOM[0]

/* Setup routine to get register classes from identifier in grammatical rules. */
#define SET_REG_CLASS_PARSING(get_reg_class_routine)					\
  PARSER_Instance->get_reg_class=get_reg_class_routine

/* Setup routine to get literal classe from identifier in grammatical rules. */
#define SET_LIT_CLASS_PARSING(get_lit_class_routine)					\
  PARSER_Instance->get_lit_class=get_lit_class_routine

/* Returns operand at position 'pos'. This is to be used in grammatical action rules. */
#define GETOPND(pos)				\
  PARSER_Instance->GetOperand(__FILE__,__LINE__,pos)

/* Returns AIR instructions list to fill. */
#define GETOPS()				\
  PARSER_Instance->GetOps()

/* AIR minimal stuff to be able to build AIR_OP */

/* Create register TN */
#define AIR_NEW_REG_TN( reg_class, reg_subclass, idx)		\
  PARSER_Instance->air_new_tn_reg(reg_class,reg_subclass,idx)

/* Create register TN */
#define AIR_NEW_ENUM_TN( enum_class, enum_class_value)		\
  PARSER_Instance->air_new_tn_enum(enum_class,enum_class_value)

/* Create immediate TN */
#define AIR_NEW_IMM_TN( value )			\
  PARSER_Instance->air_new_tn_imm(value)

/* Appends given OP to OPS */
#define AIR_APPEND_OP(ops,op)			\
  PARSER_Instance->air_ops_append_op(ops,op)

/* Make new OP from TOP and its operands */
#define AIR_MK_OP(list)				\
  PARSER_Instance->air_mk_full_op list

/* Returns TOP name */
#define TOP_NAME(top)				\
  PARSER_Instance->top_name(top)

/* Call by parser actions to add AIR_OP to AIR_OPS list
 returned after parsing */
#define ADD_INSTR(list)				\
  AIR_APPEND_OP( GETOPS(),AIR_MK_OP(list) )

/* Set a relocation on AIR_TN. This is used in action rules 
 * before AIR_OP creation.
 */
#define SET_AIR_TN_RELOC(tn,relocid)					\
  if(! (Is_AIR_TN_expression(tn) && Get_AIR_TN_exp_kind(tn) == AIR_Expression_reloc)) {	\
    fprintf(stderr,"%s:%d Internal error: TN is not of relocation type\n",__FILE__,__LINE__); \
  }									\
  Set_AIR_TN_exp_reloc_relocid(tn,relocid);

/* Set the default relocation on AIR_TN. This is used in action rules 
 * before AIR_OP creation.
 */
#define SET_AIR_TN_DEFAULT_RELOC(tn,relocid)				\
  if(Is_AIR_TN_expression(tn) && Get_AIR_TN_exp_kind(tn) == AIR_Expression_reloc) { \
    Set_AIR_TN_exp_reloc_relocid(tn,relocid);				\
  }

/*----------------------*
 * Macro to be used for *
 * configurable IP      *
 * management           *
 *----------------------*/
#define GET_CORECFG()                     \
   ((NULL != PARSER_Instance->ConfReg) ? \
    PARSER_Instance->ConfReg->ConfVal  : \
    0xFFFFFFFF                            \
   )

#define GET_CORECFG2()                    \
   ((NULL != PARSER_Instance->ConfReg) ? \
    PARSER_Instance->ConfReg->ConfVal2 : \
    0xFFFFFFFF                            \
   )

#define CHECK_CORECFG_BOUND(Field,Low,High)        \
   ( ((Field).expr->typeExp != EXPCST) ||          \
     (MSB32((Field).expr)==0  &&                   \
      LSB32((Field).expr)>=(Low) &&                \
      LSB32((Field).expr)<=(High)                  \
     )                                             \
   )

#define CHECK_CORECFG_BIT(Field,BitField)          \
   ( ((Field).expr->typeExp != EXPCST) ||          \
     (MSB32((Field).expr)==0  &&                   \
      (LSB32((Field).expr) & (BitField))           \
     )                                             \
   )

#define CORECFG_ERROR()                                    \
   if ((NULL != PARSER_Instance->ConfReg) &&              \
       (NULL != PARSER_Instance->ConfReg->ConfigError)) { \
      PARSER_Instance->ConfReg->ConfigError();            \
   }

#endif /* JUST_WANT_API_VERSION */

#ifdef __cplusplus
}
#endif

#endif /* __PARSER_H__ */

