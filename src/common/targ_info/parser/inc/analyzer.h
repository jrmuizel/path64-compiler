#ifndef  __ANALYZER_H__
#define  __ANALYZER_H__

#define BLK_SIZE	1024

#define SKIP_TO_ELSE_ENDIF		0x0001
#define SKIP_TO_ENDIF			0x0002
#define EXPAND_TO_ELSE_ENDIF	0x0004
#define EXPAND_TO_ENDIF			0x0008
#define RECORD_REPEAT			0x0010
#define RECORD_LOOP				0x0020

#define IF			0x0001
#define ELSE		0x0002
#define ELSIF		0x0003
#define ENDIF		0x0004
#define REPEAT		0x0005
#define ENDR		0x0006
#define LOOP		0x0007
#define ENDL		0x0008

typedef PARSER_eStateT e_State;

typedef PARSER_AssLexT e_ASSLEX;
typedef PARSER_nodeSemT nodeSEM;

extern nodeSEM *STRASS_AXIOM ;

extern uint32 charIndex ;
extern int32 __exprSize ;
extern int16 controlState ;

#define IS_RECORDING_REPEAT	(controlState&RECORD_REPEAT)
#define IS_RECORDING_LOOP	(controlState&RECORD_LOOP)
#define IS_EXPECTING_ENDIF	(controlState&(SKIP_TO_ELSE_ENDIF|SKIP_TO_ENDIF|EXPAND_TO_ELSE_ENDIF|EXPAND_TO_ENDIF))

#define	GETLEFTNODE(NS)		((NS).info>>16)
#define	GETRIGHTNODE(NS)	(((NS).info<<16)>>16)

#define	GETMINVAL(NS)		((NS).info>>16)
#define	GETMAXVAL(NS)		(((NS).info<<16)>>16)

#define CHECKFORMATRELOC(formatValue,ffield,idnum) CHECKTAGRELOC(Format,formatValue,ffield,idnum)
#define SETFORMATSCALING(formatValue,ffield,scf) SETTAGSCALING(Format,formatValue,ffield,scf)
#define GetCharIndex()	charIndex
#define SetCharIndex(newCharIndex)	charIndex = (newCharIndex)
#define GetExprSize()	__exprSize
#define SetExprSize(newExprSize)	__exprSize = (newExprSize)

extern int IdentLex(char *, char*);
extern int IdentSpace();
extern void SkipSpace();
extern void PushContext();
extern void PopContext();
extern void RestoreContext();
extern void CheckAxiom() ;
extern void InitializeAnalyzer();
extern void FinalizeAnalyzer();
extern uint32 PrevTokenIndex(uint32 pos) ;
extern int AnalyzeString(char *buffer) ;
extern uint32 GetErrorIndex();

#endif /* __ANALYZER_H__ */
