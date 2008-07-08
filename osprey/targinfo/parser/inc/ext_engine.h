#ifndef __ExtEngine_H__
#define __ExtEngine_H__

#include "analyzer.h"

extern char ** stringTable_Glb;
extern uint16 stringTableSize_Glb ;
extern nodeSEM ** semTable_Glb;
extern uint16 semTableSize_Glb;

extern nodeSEM * NTR_expr_Glb;
extern nodeSEM * NTR_MACRO_EXP_Glb;
extern nodeSEM * NTR_CONTROL_DIR_Glb;
extern nodeSEM * NTR_LOOP_LINE_Glb;
extern nodeSEM * NTR_PREPROC_LINE_Glb;

extern void           PARSER_Finalize( void );
extern PARSER_ReturnT PARSER_Initialize(PARSER_parse_registerT _parse_register,
					PARSER_parse_expressionT _parse_expression,
					PARSER_fatal_errorT _fatal_error,
					PARSER_errorT _error,
					PARSER_warningT _warning,
					char *usual_path_to_search_dll);

extern PARSER_ReturnT   PARSER_parse(char *buffer, AIR_OPS *_ops);
extern PARSER_u32T      PARSER_error_index( void );
extern PARSER_ParserT * PARSER_GetFeatures (char * Name);



/* Callback update at DLL connection */
/* Defined in parser.c */
extern void SetCallbacks ( PARSER_InstanceT * Extension );

extern void PARSER_ExtensionUsage ( FILE *stream );
extern void PARSER_ConnectParser ( char * Name );
extern void PARSER_DisconnectParsers ( void );
extern char *PARSER_GetProcPath( char *proc_name);

#endif /* __ExtEngine_H__ */
