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
 * File : copyright.h
 *
 * Description : Provides a macro that emits a copyright message in a file.
............................................................................
*/
#ifndef utils_copyright_h
#define utils_copyright_h

/* system includes */
#include <stdio.h>

/* local includes */
#include "utils_version_id.h"

/*---------------------------------------------------------*
 * To be printed once on top of automatically generated    *
 * files.                                                  *
 *---------------------------------------------------------*
 * Common parameters are:                                  *
 *  system    = PROCESSOR " reconfigurable toolset"        *
 *  component = PROCESSOR " extension assembler"           *
 *  filename  = <name of generated file>                   *
 *  purpose   = "assembler syntax description"             *
 *  history   = "year/month/day - Automatically generated" *
 *---------------------------------------------------------*
 * To build history string, one can use the following C    *
 * sequence:                                               *
 *  #include <time.h>                                      *
 *  char history[64];                                      *
 *  time_t loctime;                                        *
 *  struct tm * loctm;                                     *
 *                                                         *
 *  time(&loctime);                                        *
 *  loctm = localtime(&loctime);                           *
 *  strftime(history,sizeof(history),"%y/%m/%d - "         *
 *           "Automatically generated",loctm);             *
 *---------------------------------------------------------*/
#define PrintCopyright(stream,system,component,filename,purpose,history)                        \
   fprintf(stream,                                                                              \
            "/*----------------------------------------------------------------------------*\n" \
            " *      Copyright 2005, STMicroelectronics, Incorporated.                     *\n" \
            " *      All rights reserved.                                                  *\n" \
            " *                                                                            *\n" \
            " *        STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION            *\n" \
            " * This software is supplied under the terms of a license agreement or nondis-*\n" \
            " * closure agreement with STMicroelectronics and may not be copied or disclo- *\n" \
            " * sed except in accordance with the terms of that agreement.                 *\n" \
            " *----------------------------------------------------------------------------*\n" \
            " * System           : %-56s*\n"                                                    \
            " * Project Component: %-56s*\n"                                                    \
            " * File Name        : %-56s*\n"                                                    \
            " * Purpose          : %-56s*\n"                                                    \
            " * History          : %-56s*\n"                                                    \
            " *----------------------------------------------------------------------------*/\n"\
            ,system,component,filename,purpose,history);

#define PrintPurposeHistory(filedesc,purpose,history) \
   fprintf(filedesc,                      \
"/*###INCLUDE###<copyright.in>\n" \
" * Purpose          : %-56s*\n" \
" * History          : %-56s*\n" \
" *----------------------------------------------------------------------------*/\n"\
,purpose,history);

/* The following macro is equivalent to , and is used for assembly files*/
#define PrintAsmCopyright(stream,system,component,filename,purpose,history)                     \
    fprintf(stream,                                                                             \
            "//----------------------------------------------------------------------------*\n" \
            "//      Copyright 2005, STMicroelectronics, Incorporated.                     *\n" \
            "//      All rights reserved.                                                  *\n" \
            "//                                                                            *\n" \
            "//        STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION            *\n" \
            "// This software is supplied under the terms of a license agreement or nondis-*\n" \
            "// closure agreement with STMicroelectronics and may not be copied or disclo- *\n" \
            "// sed except in accordance with the terms of that agreement.                 *\n" \
            "//----------------------------------------------------------------------------*\n" \
            "// System           : %-56s*\n"                                                    \
            "// Project Component: %-56s*\n"                                                    \
            "// File Name        : %-56s*\n"                                                    \
            "// Purpose          : %-56s*\n"                                                    \
            "// History          : %-56s*\n"                                                    \
            "//----------------------------------------------------------------------------*\n" \
            ,system,component,filename,purpose,history);

/* The following macro is equivalent to , and is used for assembly files*/
#define PrintMakefileCopyright(stream,system,component,filename,purpose,history)               \
    fprintf(stream,                                                                            \
            "#----------------------------------------------------------------------------*\n" \
            "#      Copyright 2005, STMicroelectronics, Incorporated.                     *\n" \
            "#      All rights reserved.                                                  *\n" \
            "#                                                                            *\n" \
            "#        STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION            *\n" \
            "# This software is supplied under the terms of a license agreement or nondis-*\n" \
            "# closure agreement with STMicroelectronics and may not be copied or disclo- *\n" \
            "# sed except in accordance with the terms of that agreement.                 *\n" \
            "#----------------------------------------------------------------------------*\n" \
            "# System           : %-56s*\n"                                                    \
            "# Project Component: %-56s*\n"                                                    \
            "# File Name        : %-56s*\n"                                                    \
            "# Purpose          : %-56s*\n"                                                    \
            "# History          : %-56s*\n"                                                    \
            "#----------------------------------------------------------------------------*\n" \
            ,system,component,filename,purpose,history);



#endif /* utils_copyright_h */
