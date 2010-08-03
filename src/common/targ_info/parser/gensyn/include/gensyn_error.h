/*-------------------------------------------------------------------------*/
/*
 *      Copyright 2005, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 *      STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 *
 * File : gensyn_error.h
 *
 * Description : error messages dedicated to end-user specific to a tool.
 *
 * Comment : 
............................................................................
*/

/** \file gensyn_error.h
    File containing all gensyn error messages.

 */
#ifndef __GENSYN_ERROR_H__
#define __GENSYN_ERROR_H__

#include "libUtils.h"

#include "gensyn_version.h"

/* ERROR_DEFINITION
1- Gensyn error messages
------------------------

1.1- General purpose errors
---------------------------
*/

/** \ingroup GENSYN
    \defgroup GS_ERRORS Gensyn error messages.
 * Prefix of error messages are the following:
 *  - "GS-0": general purpose error messages.
 *  - "GS-1": error messages on parsing.
 *  - "GS-2": error messages on command line options given to the preprocessor.
 * @{
 */

/** \def GENSYN_ERROR_OPENFILE
\<exec\> fatal: (GS-001) Unable to open file \<file_name\>: \<system_error_message\>

	Cannot open file for parsing.

*/
#define GENSYN_ERROR_OPENFILE       "GS-001", "unable to open file %s\n"

/** \def GENSYN_ERROR_FILEERROR
\<exec\> fatal: (GS-002) file \<file_name\>: \<system_error_message\>

	File access error.

*/
#define GENSYN_ERROR_FILEERROR      "GS-002", "file %s", "%s\n"

/** \def GENSYN_INTERNALERROR_NEWGLOBAL
\<exec\> fatal: (GS-003) can't add new global declaration

	Internal error. Overflow of global declaration table.

*/
#define GENSYN_INTERNALERROR_NEWGLOBAL   "GS-003", "can't add new global declaration\n"

/** \def GENSYN_INTERNALERROR_NEWENCODING
\<exec\> fatal: (GS-004) can't add new encoding info

	Internal error. Overflow of global encoding info.

*/
#define GENSYN_INTERNALERROR_NEWENCODING "GS-004", "can't add new encoding info\n"

/** \def GENSYN_INTERNALERROR_STACKFULL
\<exec\> fatal: (GS-005) semantic stack full

	Internal error. Overflow of semantic stack.

*/
#define GENSYN_INTERNALERROR_STACKFULL   "GS-005", "semantic stack full\n"

/** \def GENSYN_INTERNALERROR_STACKEMPTY
\<exec\> fatal: (GS-006) semantic stack empty

	Internal error. Underflow of semantic stack.

*/
#define GENSYN_INTERNALERROR_STACKEMPTY  "GS-006", "semantic stack empty\n"

/** \def GENSYN_INTERNALERROR_PORT
\<exec\> fatal: (GS-007) only 1 port definition allowed  

	Internal error. Only one port definition is allowed. 

*/
#define GENSYN_INTERNALERROR_PORT   "GS-007", "only one port definition allowed\n"

/** \def GENSYN_INTERNALERROR_MULTIPLY
\<exec\> fatal: (GS-008) only 1 multiply definition allowed  

	Internal error. Only one multiply definition is allowed. 

*/
#define GENSYN_INTERNALERROR_MULTIPLY   "GS-008", "only one multiply definition allowed\n"

/** \def GENSYN_INTERNALERROR_SYNTAXNODEUNKOWN
\<exec\> fatal: (GS-009) Syntax node generation failed. Given lexem cannot be mapped on syntaxical node.

	Internal error. unable to generate syntaxical node. 

*/
#define GENSYN_INTERNALERROR_SYNTAXNODEUNKNOWN   "GS-008", "unable to generate syntaxical node\n"

/* ERROR_DEFINITION
1.2- Parsing errors
-------------------

*/

/** \def GENSYN_ERROR_RELOCDEFINED
\<exec\> fatal: (GS-101) relocation \<reloc_name\> already defined

	Relocation has already been defined.

*/
#define GENSYN_ERROR_RELOCDEFINED   "GS-101", "relocation %s already defined\n"

/** \def GENSYN_ERROR_RELOCTYPE
\<exec\> fatal: (GS-102) invalid relocation type \<reloc_type\>. Allowed types are REL or RELA

	In this context, this relocation type is not valid. Valid values are REL or RELA.

*/
#define GENSYN_ERROR_RELOCTYPE      "GS-102", "invalid relocation type %s. Allowed types are REL or RELA\n"

/** \def GENSYN_ERROR_RELOCPCREL
\<exec\> fatal: (GS-103) invalid relocation type \<reloc_type\>. Allowed types are PCREL, ABS or SYREL

	In this context, this relocation type is not valid. Valid values are PCREL, ABS or SYREL.

*/
#define GENSYN_ERROR_RELOCPCREL     "GS-103", "invalid relocation type %s. Allowed types PCREL, ABS or SYREL\n"

/** \def GENSYN_ERROR_INSTCLASS
\<exec\> fatal: (GS-104) instruction type \<inst_type\> already defined

	Instruction type has already been defined.

*/
#define GENSYN_ERROR_INSTCLASS      "GS-104", "instruction class %s already defined\n"

/** \def GENSYN_ERROR_FORMATDEF
\<exec\> fatal: (GS-105) format type \<fmt_type\> already defined

	Format type has already been defined.

*/
#define GENSYN_ERROR_FORMATDEF      "GS-105", "format %s already defined\n"

/** \def GENSYN_ERROR_GROUPINGDEF
\<exec\> fatal: (GS-106) grouping \<gp\> already defined

	Grouping has already been defined. ST100 specific.

*/
#define GENSYN_ERROR_GROUPINGDEF    "GS-106", "grouping %s already defined\n"

/** \def GENSYN_ERROR_TRUNCIDF
\<exec\> fatal: (GS-107) identifier too long. truncated to /<idf_trunc/>

	Identifier user is too long. Truncated value is /<idf_trunc/>

*/
#define GENSYN_ERROR_TRUNCIDF       "GS-107", "identifier too long. truncated to %s\n"

/** \def GENSYN_ERROR_SYNTAX0
\<exec\> fatal: (GS-108) syntax error /<more_info/>

	Syntax error at input file parsing.

*/
#define GENSYN_ERROR_SYNTAX0        "GS-108", "syntax error ", "%s\n"

/** \def GENSYN_ERROR_SYNTAX
\<exec\> fatal: (GS-109) syntax error /[/<more_info/>/]

	Syntax error at input file parsing.

*/
#define GENSYN_ERROR_SYNTAX         "GS-109", "syntax error [%s]\n"

/** \def GENSYN_ERROR_INVALIDIDF
\<exec\> fatal: (GS-110) invalid identifier /[/<idf/>/]

	This identifier is invalid in this context.

*/
#define GENSYN_ERROR_INVALIDIDF     "GS-110", "invalid identifier [%c]\n"

/** \def GENSYN_ERROR_INVALIDKEYWORD
\<exec\> fatal: (GS-111) invalid keyword /[/<kwd/>/]

	This keyword is invalid in this context.

*/
#define GENSYN_ERROR_INVALIDKEYWORD "GS-111", "invalid keyword [%s]\n"

/** \def GENSYN_ERROR_UNDEFRULE
\<exec\> fatal: (GS-112) /<rule_name/> rule is undefined

	This rule has not been defined in input files.

*/
#define GENSYN_ERROR_UNDEFRULE      "GS-112", "%s rule is undefined\n"

/* ERROR_DEFINITION
1.3- Argument parsing errors
----------------------------

*/

/** \def GENSYN_ERROR_NOFILE
\<exec\> fatal: (GS-201) no file on command line

	At least one input file is required.

*/
#define GENSYN_ERROR_NOFILE         "GS-201", "no file on command line\n"

/** \def GENSYN_ERROR_ONLYONEARG
\<exec\> fatal: (GS-202) more than one argument in option /<opt_name/>

	This option can take only one argument.

*/
#define GENSYN_ERROR_ONLYONEARG     "GS-202", "more than one argument in option %s\n"

/** \def GENSYN_ERROR_WRONGARG
\<exec\> fatal: (GS-203) invalid argument /<arg/> for option /<opt_name/>

	Argument given to option is not valid.

*/
#define GENSYN_ERROR_WRONGARG       "GS-203", "invalid argument %s for option %s\n"

/** \def GENSYN_ERROR_ARGSTATE
\<exec\> fatal: (GS-204) /<arg1/> requires /<arg2/> and vice-versa

	Arg1 requires Arg2 for a correct behavior and vice-versa.

*/
#define GENSYN_ERROR_ARGSTATE       "GS-204", "%s requires %s and vice-versa\n"

/** \def GENSYN_ERROR_ARGMISSED
\<exec\> fatal: (GS-204) /<arg1/> is required

	Arg1 is required for a correct behavior and vice-versa.

*/
#define GENSYN_ERROR_ARGMISSED       "GS-204", "%s is required\n"


/** \def GENSYN_ERROR_RELOC
\<exec\> fatal: (GS-205) Reloc not managed.

	Relocation not managed by assembly parser.

*/
#define GENSYN_ERROR_RELOC       "GS-205", "Relocation not managed\n"

/** \def GENSYN_ERROR_ELF
\<exec\> fatal: (GS-206) Elf not managed.

	Elf not managed by assembly parser.

*/
#define GENSYN_ERROR_ELF       "GS-206", "%s is required\n"

/* Error message managed at tool level */

extern int noError ;
extern int lineNo ;

extern void SetNoError(void);
extern void ClearNoError(void);
extern void DisplayErrorMessage(void);

/** @} */

#endif /* __GENSYN_ERROR_H__ */
