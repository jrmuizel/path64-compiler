#ifndef __ASTYPE_H__
#define __ASTYPE_H__

typedef PARSER_StrStringT s_String;

/* conversion routine from a char* to s_String * */
/* be carefull no memory is allocated            */

extern s_String *Char2String(char *) ;
extern uint32 String2Xint32(s_String string, uint16 base) ;

#endif /* __ASTYPE_H__ */

