#ifndef __ENCSTCK_H__
#define __ENCSTCK_H__

#define ENCODING_MAX_SIZE 65535

typedef enum {ENCODE, BINCONST, DECCONST, HEXCONST, LABEL, CSTEXPR, REGISTER} e_ENCINFOTYPE ;
extern char *ENCINFO_Name[];

typedef struct {
  e_ENCINFOTYPE type;      /* Type of encoding info. */
  s_String string ;        /* Contains matched tokens. */
  PARSER_u32T main_class;  /* Main register or literal class. */
  PARSER_u32T sub_class;   /* Register subclass. */
  PARSER_u32T is_reloc ;   /* True if get relocation. Fields 'sym_id', 'symop_id' and 'opnd' are updated. */
  void       *sym_id ;     /* Contains symbol id associated with immediate relocation */
  void       *symop_id ;   /* Contains second symbol id if needed associated with immediate relocation */
  PARSER_s64T opnd ;       /* Contains the immediate value, register index or relocation addend */
  void       *expr ;       /* Client expression */
  int exprSize ;
  int action ;       /* Identifier of the action to execute. */
  int numToken ;
  uint32 index ;     /* index in analyzed line  "a <#u3> <#4>", <#u3> index = 0, <#u4> index = 1*/
} s_encodingInfo ;

#define ACTION_get_main_class(s) ((s)->main_class + 0)
#define ACTION_get_sub_class(s)  ((s)->sub_class + 0)
#define ACTION_is_relocation(s)  ((s)->is_reloc + 0)
#define ACTION_get_sym_id(s)     ((s)->sym_id + 0)
#define ACTION_get_symop_id(s)   ((s)->symop_id + 0)
#define ACTION_get_opnd(s)       ((s)->opnd + 0)
#define ACTION_get_expr(s)       ((s)->expr + 0)

typedef struct {
	s_encodingInfo array[ENCODING_MAX_SIZE] ;	/* stack array */
	int size ;					/* stack size/pointer */
} s_encodingStack ;

extern s_encodingStack *GetEncStack();
extern void InitEncodingStack(s_encodingStack *stack) ;
extern void DecrEncodingStack(s_encodingStack *stack, int decr) ;
extern void PushEncodingInfo(s_encodingStack *stack, s_encodingInfo *newInfo) ;
extern s_encodingInfo *PopEncodingInfo(s_encodingStack *stack) ;
extern int GetEncStackIndex (s_encodingStack *stack) ;
extern void SetEncStackIndex (s_encodingStack *stack, int encStackIndex) ;
extern uint32 GetSumExprSizeInEncodingInfo(s_encodingStack *stack, int stackStart, int stackEnd) ;
extern void MoveEncodingInfo(s_encodingStack *stack, int destElem, int startElem, int endElem) ;
 
extern void DisplayTypedInfo(s_encodingInfo *sEncInfo);
extern void DisplayEncodingInfo();

extern s_encodingInfo *GetEncInfoIndex (const char *file, int line, s_encodingStack *stack, int index) ;

#endif /* __ENCSTCK_H__ */
