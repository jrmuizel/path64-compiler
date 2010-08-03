/*-------------------------------------------------------------------------*/
/*
 *      Copyright 2004, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 *      STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 *
 * File : extension.c
 *
 * Description : Display information about assembler extension connected.
............................................................................
*/

#include <stdio.h>
#include <string.h>

#include "air.h"
#include "parser.h"
#include "type_defs.h"
#include "defs_exported.h"
#include "astype.h"
#include "analyzer.h"
#include "ext_engine.h"

char ** stringTable_Glb        = NULL;
uint16 stringTableSize_Glb     = 0;

nodeSEM ** semTable_Glb        = NULL;
uint16 semTableSize_Glb        = 0;

nodeSEM * NTR_expr_Glb         = NULL;
nodeSEM * NTR_MACRO_EXP_Glb    = NULL;
nodeSEM * NTR_CONTROL_DIR_Glb  = NULL;
nodeSEM * NTR_LOOP_LINE_Glb    = NULL;
nodeSEM * NTR_PREPROC_LINE_Glb = NULL;

char * targetString_Glb        = NULL;

PARSER_parse_registerT   PARSER_parse_register = NULL;
PARSER_parse_expressionT PARSER_parse_expression = NULL;
PARSER_fatal_errorT      PARSER_fatal_error = NULL;
PARSER_errorT            PARSER_error = NULL;
PARSER_warningT          PARSER_warning = NULL;
char * PARSER_usual_path = NULL;

PARSER_EncodeT STRASSEncode_Glb = NULL;
PARSER_InitT   STRASSInit_Glb   = NULL;

PARSER_ParserT * ParserList[MAX_PARSER_NR];
PARSER_InstanceT * currParserInstance = NULL;

/*-------------------------------------------------------------------------*/
/*
   Routine center_arg

   Description : 

   Input  :    str       (char * )   string to be displayed
               sz_format (uint_32)   size of space available to dislay this
                                     string
               buf       (char*  )   buffer of size sz_buf. Buffer is
               sz_buf    (int    )   allocated but is not empty.

   Output :    buf contains a string, possibly preceded by ' ' characters.

   Note : we assume sz_format < sz_buf.
............................................................................
*/
static void center_arg ( char *str, uint32 sz_format, char *buf, 
                         uint32 sz_buf ) {
   uint32 start;

   if (strlen(str)>sz_format) {
      if ((strlen(str)+1)<=sz_buf) {
         strcpy(buf,str);
      } else {
         memcpy(buf,str,sz_buf-1);
         buf[sz_buf-1] = '\0';
      }
   } else {
      start = (sz_format - strlen(str))/2;
      if (start) {
         memset(buf,' ',start);
      }
      strcpy(buf+start,str);
   }
}

/*-------------------------------------------------------------------------*/
/*
   Routine extension_usage

   Description : display some information related to extension.

   Input  : stream (FILE*) stream where information is to be written
............................................................................
*/
void PARSER_ExtensionUsage ( FILE *stream ) {
   const char * const head_msg = "\n\nInformation on STxP70 extensions\n\n";
   const char * const str1 = " Name ";
   const char * const str2 = "     Port(s)      ";
   const char * const str3 = " Abstract ";
         char         format[200];
         int          i;

         char         arg1[200],
                      arg2[200],
                      arg3[200];

#define BASIC_MOTIF_1 "%%-%ds"
#define BASIC_MOTIF_2 "|"
     
   /* First, build format string */
   sprintf (format, 
            BASIC_MOTIF_1 BASIC_MOTIF_2 
            BASIC_MOTIF_1 BASIC_MOTIF_2 
            BASIC_MOTIF_1 "\n",
            (int) strlen(str1),
            (int) strlen(str2),
            (int) strlen(str3));
#define PRINT_SEP()                \
   fprintf(stream,format,          \
   "------" ,"------------------", \
   "------------------------------------------------------");

   /* Print header */
   fprintf(stream,head_msg);
   if (NULL==ParserList[0]) {
      fprintf(stream,"No extension connected\n\n");
      return;
   }

   PRINT_SEP();
   fprintf(stream,format,str1,str2,str3);
   PRINT_SEP();

   /* For each coprocessor */
   for(i=0;(i<MAX_PARSER_NR) && (NULL!=ParserList[i]);i++) { 
      int id;
      static const char * str_port[MAX_PARSER_NR] = {"-","0","1","2","3","4","5","6","7"};
      char port_str[20] = {0};
      
      if (!strcasecmp(ParserList[i]->Xi.Name,"Core")) continue;
      
      /* Build arg1 : name of extension */
      center_arg(NULL == ParserList[i]->Xi.Name    ?
                 "---" : ParserList[i]->Xi.Name ,
                 strlen(str1),
                 arg1,sizeof arg1);

      /* Build arg2 : port - Connection to core */
      for (id=0;(id<MAX_PARSER_NR) && (id<ParserList[i]->Xi.PortSize);id++) {
         if (ParserList[i]->Xi.Port[id]) {
            if (port_str[0]) strcat(port_str,",");
            strcat(port_str,str_port[id]);
         }
      }
      center_arg(0 == port_str[0] ?
                 "---" : port_str ,
                 strlen(str2),
                 arg2,sizeof arg2);

      /* Build arg3 - Abstract */
      center_arg(NULL == ParserList[i]->Xi.ShortAbstract ? 
                 "---" : ParserList[i]->Xi.ShortAbstract,
                 strlen(str3),
                 arg3,sizeof arg3);

      /* Display final result */
      fprintf(stream,format, arg1, arg2, arg3);
   }

   PRINT_SEP();
   fprintf(stream,"\n");
}

