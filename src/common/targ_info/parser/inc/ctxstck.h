#ifndef __CTXSTCK_H__
#define __CTXSTCK_H__

#define CONTEXT_MAX_SIZE 1024

typedef struct  {
	int charIndex ;			/* index in analyzed string */
	int encStackIndex ;		/* stack index for encoding stack */
	int exprSize ;			/* if current token is an valid expresion */
	                                /* exprSize > 0 */
} s_contextElem ;

typedef struct {
	s_contextElem stack[CONTEXT_MAX_SIZE] ;	/* stack array */
	s_encodingStack *encStackPtr ;			/* pointer to encoding Stack */
	int size ; 								/* stack size/pointer */
} s_contextStack ;

extern void InitContextStack(s_encodingStack *encStackPtr) ;
extern void PushContext() ;
extern void PopContext() ;
extern void RestoreContext() ;
extern s_contextElem GetContext() ;
extern void SetContext(s_contextElem currContext) ;

#endif /* __CTXSTCK_H__ */
