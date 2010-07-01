/** \file format.h
    \brief Format of encoding for instructions.

*/
#ifndef __FORMAT_H__
#define __FORMAT_H__

#include <stdio.h>
#include "gensyn_defs.h"

/** AddNumFieldToCurrField */
extern void AddNumFieldToCurrField(uint32 val) ;
/** SetNewCurrField */
extern void SetNewCurrField(char *name) ;
/** AddCurrFieldToCurrFormat */
extern void AddCurrFieldToCurrFormat() ;
/** IsFormatDefined */
extern int IsFormatDefined(char *name) ;
/** SetNewCurrFormat */
extern void SetNewCurrFormat(char *name) ;
/** InitFormats */
extern void InitFormats() ;
/** AddCurrFormatToList */
extern void AddCurrFormatToList() ;
/** PrintFormatFields */
extern void PrintFormatFields (FILE*cfd) ;
/** PrintFormatType */
extern void PrintFormatType (FILE*cfd) ;
/** PrintFormatTable */
extern void PrintFormatTable (FILE*cfd) ;

#endif /* __FORMAT_H__ */
