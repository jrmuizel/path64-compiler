/** \file unilex.h
    \brief Parser lexem definition

*/
#ifndef __UNILEX_H__
#define __UNILEX_H__

#include <stdio.h>

/** \typedef ATOME
    Atomic lexems.
*/

/** \enum ATOME
    this enumeration defines atomic lexems
*/
typedef enum {
   COLUMN,     /**< */
   COMMA,      /**< */
   CONCAT,     /**< */
   OPENACC,    /**< */
   CLOSACC,    /**< */
   OPENBRA,    /**< */
   CLOSBRA,    /**< */
   IDNTER,     /**< */
   EXPR,       /**< */
   RANGE,      /**< */
   UCSTEXP,    /**< */
   SCSTEXP,    /**< */
   ELTER,      /**< */
   LINEN,      /**< */
   FILEN,      /**< */
   OR,         /**< */
   AND,        /**< */
   SPACE,      /**< */
   GLBDEC,     /**< */
   SYNKEYWORD, /**< */
   ACTKEYWORD, /**< */
   DESKEYWORD, /**< */
   LEXEM,      /**< */
   EMPTY,      /**< */
   LEXERR,     /**< */
   REPEAT,     /**< */
   OPTION,     /**< */
   FORMAT,     /**< */
   NUMVAL,     /**< */
   STAR,       /**< */
   CLASS,      /**< */
   RELOC,      /**< */
   ELF,        /**< */
   INIT,       /**< */
   GROUPING,   /**< */
   ROOT,       /**< */
   XOR,        /**< */
   PORT,       /**< */
   MULTIPLY    /**< */
} ATOME; 

/** GetLeftLex */
extern ATOME GetLeftLex(int skipEOL);
/** GetRightLex */
extern ATOME GetRightLex();
/** GetIdent */
extern void GetIdent();
/** identifie */
extern int identifie (ATOME read, ATOME expected);
/** GetEncodingInformations */
extern char *GetEncodingInformations() ;

extern FILE* inputStream ; /**< Input stream of parser */

#endif
