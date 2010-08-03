/** \file strind.h
    \brief string index management

*/
#ifndef __STRIND_H__
#define __STRIND_H__

#include "gensyn_defs.h"

/** \typedef s_StrInd
    String management.
*/

/** \struct s_StrInd
    this structure is used to manage a string inside a table
*/
typedef struct s_StrInd {
    char valStr[MAX_SEM_LEN];		/**< string value */
    int indStr ;					/**< index in table */
} s_StrInd ;

#define	SETSTR(sstr, valstr)	(strcpy((sstr).valStr, (valstr)))
#define GETSTR(sstr)		((sstr).valStr)
#define	SETIND(sstr, valind)	((sstr).indStr = valind)
#define GETIND(sstr)		((sstr).indStr)

#endif /* __STRIND_H__ */
