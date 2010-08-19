/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/


// isa_gen.cxx
/////////////////////////////////////
//
//  Generate an interface to create a new ISA (actually just an enum of
//  all the opcodes).
//    
/////////////////////////////////////
//
//  $Revision$
//  $Date$
//  $Author$
//  $Source$

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "gen_util.h"
#include "isa_gen.h"

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   A description of the ISA (actually just an enum of all the opcodes).",
  " *   The description exports the following:",
  " *",
  " *   TOPCODE stands for Target OPCODE; prefix is TOP.",
  " *",
  " *   typedef (enum) TOP",
  " *      Contains all the target opcodes.  Their names have the form",
  " *      TOP_<name>.",
  " *",
  " *   typedef mTOP",
  " *      The smallest integer type that can contain all values of a TOP,",
  " *      including TOP_UNDEFINED -- useful for conserving space in tables.",
  " *",
  " *   const TOP TOP_UNDEFINED",
  " *      Useful value guaranteed not to be a valid TOP.",
  " *",
  " *   const int TOP_count",
  " *      Gives the number of topcodes.",
  " *",
  " *   const char* TOP_Name(TOP topcode)",
  " *      Returns an assembler style name for the given TOP.",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};


/////////////////////////////////////
static char* Dot_To_Line(const char* str)
/////////////////////////////////////
//  Copy <str> to newly allocated memory, replacing "." with "_" and return
//  the result.
/////////////////////////////////////
{
  char *result = (char*) malloc(strlen(str)+1);
  const char *s;
  char *r;

  for (s = str, r = result; *s != 0; ++s, ++r) {
    if (*s == '.')
      *r = '_';
    else
      *r = *s;
  }

  *r = 0;

  return result;
}

#ifdef TARG_ST
/////////////////////////////////////
void ISA_Create (const char *isa_name, ...)
/////////////////////////////////////
//  Emit the topcode header and c files.
/////////////////////////////////////
{
  // Whether we generate code for an extension
  // or for the core.
  bool        gen_static_code = Is_Static_Code();

  const char* const extname  = gen_static_code ? NULL: Get_Extension_Name();

  const char* const name_topcode_h = Gen_Build_Filename (FNAME_TOPCODE,
                                                         extname,
                                                         gen_util_file_type_hfile);
  const char* const name_topcode_c = Gen_Build_Filename (FNAME_TOPCODE,
                                                         extname,
                                                         gen_util_file_type_cfile);
  const char* const name_topcode_ex= Gen_Build_Filename(FNAME_TOPCODE,
                                                        extname,
                                                        gen_util_file_type_efile);
  const char* const name_topcode_stub= Gen_Build_Filename(FNAME_STUB FNAME_ISA_TOPCODE,
                                                        extname,
                                                        gen_util_file_type_cfile);

  const char* const prefix = gen_static_code ? "static" : "dynamic";

  // Name of top names tables generated in the file.
  char ISA_local_top_names[100];
  sprintf(ISA_local_top_names,"ISA_%s_top_names",prefix);

  const char * const ISA_top_names        = "ISA_generic_top_names";

  FILE* hfile = Gen_Open_File_Handle(name_topcode_h ,"w");
  FILE* cfile = Gen_Open_File_Handle(name_topcode_c ,"w");
  FILE* efile = gen_static_code ? Gen_Open_File_Handle(name_topcode_ex,"w") : NULL;
  FILE* sfile = gen_static_code ? NULL : Gen_Open_File_Handle(name_topcode_stub,"w");

  char* instruction_name;
  const char* start_offset_str = gen_static_code ? "STATIC_OFFSET":
                                                   "dynamic_offset_";

  int   instruction_count = 0;
  va_list ap;

  Emit_Header (hfile,FNAME_TOPCODE,interface,extname);

  // For extensions, include static header file.
  if(!gen_static_code) {
    const char * const static_name = Gen_Build_Filename(FNAME_TOPCODE,
							NULL,
							gen_util_file_type_hfile);

    const char *headers[] = {
      "\"dyn_" FNAME_ISA_TOPCODE ".h\"",
      "",
    };

    Emit_C_Header(cfile);  /* Emit #ifdef _cplusplus directive */
    fprintf(cfile,"#include \"%s\"\n",static_name);
    Gen_Free_Filename(const_cast<char*>(static_name));
    
    Emit_Stub_Header(sfile,headers);
  }

  // In any case (dynamic or static);
  fprintf(cfile,"#include \"%s\"\n\n",name_topcode_h);

  if(gen_static_code)
   { fprintf(hfile,"#define %s  0\n\n",start_offset_str);
   }
  else
   { fprintf(hfile,"extern mUINT32 dynamic_offset_%s;\n\n",
                   extname);
     fprintf(hfile,
             "extern void dyn_set_TOP_base_offset( INT );\n"
             "\n");

     fprintf(cfile,
             "\n"
             "/* dynamic offset of extension initialized to 0 \n"
             " * This value is convenient for Open64 generator process\n"
             " */\n"
             "mUINT32 %s%s=0;\n"
             "\n",
             start_offset_str,
             extname
            );
     fprintf(cfile,
             "\n"
             "/* Set the value of the base offset for TOP of this extension\n"
             " */\n"
             "void dyn_set_TOP_base_offset(INT base) {\n"
             "  %s%s = base;\n"
             "}\n"
             "\n",
             start_offset_str,
             extname
            );
   }

  fprintf(cfile,"static const char* const %s[] = {\n",ISA_local_top_names);

  va_start(ap,isa_name);
  while ((instruction_name = va_arg (ap, char *)) != NULL) {

    char * instname = Dot_To_Line(instruction_name);

    // For extensions, names of TOP is mangled by
    // extension names.
    if(gen_static_code)
     { fprintf(hfile,
               "#define TOP_%-30s (%s + %4d)\n",
                instname,
                start_offset_str,
                instruction_count);
     }
    else
     { fprintf(hfile,
               "#define TOP_dyn_%s_%-30s (%s%s + %4d)\n"
               "#define TOP_local_dyn_%s_%-24s %d\n\n",

                extname,                 /* First line */
                instname,
                start_offset_str,
                extname,
                instruction_count,

                extname,                 /* Second line */
                instname,
                instruction_count);
     }
                                
    if(gen_static_code)
      fprintf(cfile,"  \"%s\" ,\n",instruction_name);
    else
      fprintf(cfile,"  \"dyn_%s_%s\" ,\n",extname,instruction_name);

    instruction_count++;
    free(instname);
  }
  va_end(ap);

  if(gen_static_code)
    fprintf(cfile,"  \"UNDEFINED\" ,\n");

  fprintf(cfile,"\n};\n");

  if(gen_static_code)
  {
   // TOP_UNDEFINED which used to be the last item
   // of topcode enumeration, is now managed as an
   // unsigned integer.
   fprintf(hfile,"\nextern mUINT32 TOP_UNDEFINED;\n\n");
   fprintf(cfile,
          "\n"
          "/* Defining and initializing TOP_UNDEFINED */\n"
          "BE_EXPORTED mUINT32 TOP_UNDEFINED = TOP_static_count;\n");

   fprintf(cfile,
          "\n"
          "/* Defining and initializing ISA_generic top_names */\n"
          "/* If dynamics extensions are available, the loader*/\n" 
          "/* will update this pointer.                       */\n"
          "BE_EXPORTED const char* const * %s = %s;\n",
          ISA_top_names,
          ISA_local_top_names);


   fprintf(hfile,"\n");
   fprintf(hfile,"typedef mUINT32 TOP;\n");
   fprintf(hfile,"typedef mUINT32 mTOP;\n");   // Same type as TOP. 

   fprintf(hfile,"#define TOP_static_count %d\n"  , instruction_count);

   fprintf(hfile,"BE_EXPORTED extern mUINT32 TOP_count;\n");
   fprintf(cfile,"BE_EXPORTED mUINT32 TOP_count = %d;\n\n",instruction_count);

   fprintf(hfile,"BE_EXPORTED extern const char* TOP_Name(TOP topcode);\n");
   fprintf(hfile,"BE_EXPORTED extern const char* const * %s;\n",ISA_top_names);
   fprintf(efile,"TOP_Name\n");
  
   fprintf(cfile,
           "\nconst char* TOP_Name(TOP topcode)\n{\n"
           "  return %s[(int)topcode];\n"
           "}\n",
           ISA_top_names);

  fprintf(hfile,"\nBE_EXPORTED extern TOP TOP_From_Name(const char *topname);\n");
  fprintf(cfile,"\nTOP TOP_From_Name(const char *topname) {\n"
		  "  TOP top;\n"
		  "  // TODO: This is a really inefficient implementation.\n"
		  "  //       A hashtable should be used instead\n"
		  "  for (top=(TOP)0; top<TOP_count; top++) {\n"
		  "    if (!strcmp(%s[top], topname)) {\n"
		  "      return ((TOP)top);\n"
		  "    }\n"
		  "  }\n"
		  "  return (TOP_UNDEFINED);\n"
		  "}\n",
                  ISA_top_names);
  }
  else {
    /* Managing dynamic extension */
    /* First consider the number  */
    /* of dynamic TOP code        */
    fprintf(cfile,
            "\n"
            "const mUINT32 dyn_get_TOP_count( void )\n"
            "{\n"
            " return (const mUINT32)%d;\n"
            "}\n",
            instruction_count);
    fprintf(hfile,"\nTARGINFO_EXT_EXPORTED extern const mUINT32 dyn_get_TOP_count( void ); /* Used at connection time */\n");
    fprintf(hfile,"\n#define TOP_dyn_count %d /* Used at dll compile time */\n\n",
                  instruction_count);

    /* Second consider the number */
    /* of extensions.             */
    fprintf(cfile,
            "\n"
            "const char** dyn_get_TOP_name_tab( void )\n"
            "{\n"
            " return (const char**)%s;\n"
            "}\n\n",
            ISA_local_top_names);

     fprintf(hfile,
             "TARGINFO_EXT_EXPORTED extern const char** dyn_get_TOP_name_tab( void );\n");

    /* Third, define an interface */
    /* routine in order to check  */
    /* whether a TOP belong to a  */
    /* given extension.           */
    fprintf(cfile,"int dyn_belong_TOP_to_ext( TOP top )\n"
                  "{ return top>= %s%s && top<%s%s+%dU;\n"
                  "}\n\n",
                  start_offset_str,
                  extname,
                  start_offset_str,
                  extname,
                  instruction_count);
    fprintf(hfile,"TARGINFO_EXT_EXPORTED extern int dyn_belong_TOP_to_ext( TOP top );\n");

    /* Fourth, define an interface   */
    /* routine in order to get local */
    /* TOP from generic top.         */
    fprintf(cfile, "TOP dyn_get_TOP_local( TOP top )\n"
                   "{ return dyn_belong_TOP_to_ext(top) ? top - (TOP)%s%s : (TOP)-1;\n"
                   "}\n",
                   start_offset_str,
                   extname);
    fprintf(hfile,"TARGINFO_EXT_EXPORTED extern TOP dyn_get_TOP_local( TOP top );\n\n");

    /* Fifth, declaration a variable  */
    /* is a copy in extension dll     */
    /* of TOP_UNDEFINED               */
    fprintf(cfile,
      "\n"
      "/* Dll local copy of TOP_UNDEFINED            */\n"
      "/* Initialized at connection time by          */\n"
      "/* the loader thanks to dyn_set_TOP_UNDEFINED */\n"
      "/* call-back routine.                         */\n"
      "TOP %s_CORE_TOP_UNDEFINED = (TOP)-1;\n"
      "\n",
      extname);
    fprintf(hfile,
      "TARGINFO_EXT_EXPORTED extern TOP %s_CORE_TOP_UNDEFINED;\n",
      extname);

    fprintf(cfile,
      "void dyn_set_TOP_UNDEFINED(TOP value)\n"
      "{ %s_CORE_TOP_UNDEFINED = value;\n"
      "}\n"
      "\n",
      extname);
    fprintf(hfile,
      "TARGINFO_EXT_EXPORTED extern void dyn_set_TOP_UNDEFINED(TOP top);\n");


    fprintf(sfile,
	    "\n"
	    "/* Defining and initializing TOP_UNDEFINED for other targinfo generators */\n"
	    "TOP TOP_UNDEFINED = TOP_static_count;\n");

    fprintf(sfile,
	    "\n"
	    "/* Get TOP name from extension top table for other targinfo generators */\n"
	    "const char* TOP_Name(TOP topcode)\n{\n"
	    "  return (dyn_get_TOP_name_tab())[(int)topcode];\n"
	    "}\n");
  }

  Emit_Footer (hfile);
  if(!gen_static_code)
    Emit_C_Footer(cfile);

  // Closing all file handlers.
  Gen_Close_File_Handle(cfile ,name_topcode_c );
  Gen_Close_File_Handle(hfile ,name_topcode_h );
  if(efile) 
    Gen_Close_File_Handle(efile ,name_topcode_ex);
  if(sfile) 
    Gen_Close_File_Handle(sfile ,name_topcode_stub);

  // Memory deallocation
  Gen_Free_Filename(const_cast<char*> (name_topcode_c));
  Gen_Free_Filename(const_cast<char*> (name_topcode_h));
  Gen_Free_Filename(const_cast<char*> (name_topcode_ex));
  Gen_Free_Filename(const_cast<char*> (name_topcode_stub));

  return;
}
#else
/////////////////////////////////////
void ISA_Create (const char *isa_name, ...)
/////////////////////////////////////
//  Emit the topcode header and c files.
/////////////////////////////////////
{
  FILE* hfile = fopen("topcode.h","w");
  FILE* cfile = fopen("topcode.c","w");
  FILE* efile = fopen("topcode.Exported","w");
  char *instruction_name;
  int instruction_count = 0;
  va_list ap;

  fprintf(cfile,"#include \"topcode.h\"\n");

  Emit_Header (hfile, "TOPCODE", interface);

  fprintf(hfile,"typedef enum topcode {");
  fprintf(cfile,"static const char* const top_names[] = {");

  bool is_first = true;
  va_start(ap,isa_name);
  while ((instruction_name = va_arg (ap, char *)) != NULL) {
    fprintf(hfile,"%s\n  TOP_%s",is_first ? "" : ",",
                                 Dot_To_Line(instruction_name));
    fprintf(cfile,"%s\n  \"%s\"",is_first ? "" : ",",
                                 instruction_name);
    if ( is_first )
      is_first = false;

    instruction_count++;
  }
  va_end(ap);

  fprintf(hfile,",\n  TOP_UNDEFINED");
  fprintf(cfile,",\n  \"UNDEFINED\"");

  fprintf(hfile,"\n} TOP;\n");
  fprintf(cfile,"\n};\n");

  fprintf(hfile,"\ntypedef %s mTOP;\n", 
		(instruction_count+1) <= 256 ? "mUINT8" : "mUINT16");

  fprintf(hfile,"\n#define TOP_count %d\n", instruction_count);

  fprintf(hfile,"\nextern const char* TOP_Name(TOP topcode);\n");
  fprintf(efile,"TOP_Name\n");
  fprintf(cfile,"\nconst char* TOP_Name(TOP topcode)\n{\n"
                "  return top_names[(int)topcode];\n"
		"}\n");

  Emit_Footer (hfile);
}
#endif
