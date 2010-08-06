/** \file encinfo.h
    \brief Encoding information definition.

*/
#ifndef __ENCINFO_H__
#define __ENCINFO_H__

#define MAX_ENC_INFO 16384  /**< Depth of encoding info stack */

#include <stdio.h>

/** GetInit */
extern int GetInit() ;
/** SetInit */
extern void SetInit(int value) ;
/** InitEncInfo */
extern void InitEncInfo() ;
/** AddNewGlobDecl */
extern void AddNewGlobDecl(char *newGlobDeclInfo) ;
/** AddNewPort */
extern void AddNewPort(char *newPortInfo) ;
/** AddNewPort */
extern void AddNewMultiply(char *newMultiplyInfo) ;
/** AddNewEncInfo */
extern int AddNewEncInfo(char *newEncInfo) ;
/** DisplayPortInfo */
extern void DisplayPortInfo (FILE *outStream) ;
/** DisplayMultiplyEncoding */
extern void DisplayMultiplyInfo (FILE *outStream) ;
/** DisplayEncFunc */
extern void DisplayEncFunc (FILE *outStream) ;

#endif /* __ENCINFO_H__ */
