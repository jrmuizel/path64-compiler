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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gen_util.h"
#ifdef TARG_ST
//////////////////////////////////////
// Management of core's extensions.
//////////////////////////////////////

static bool  Gen_Dyn_Code = false;   // By default, code generation is done
                                     // for the core and is "static".
static char* extension_name = NULL;

// Test whether current code generation
// is done for an extension.
bool 
Is_Dynamic_Code( void )
{
   return Gen_Dyn_Code;
}

bool 
Is_Static_Code( void )
{
   return !Gen_Dyn_Code;
}

// Return name of current extension.
char* 
Get_Extension_Name( void )
{
   if(false==Gen_Dyn_Code)
     return NULL;

   return extension_name;
}
#endif
#ifdef TARG_ST
/* We have added one optional null parameter for extension name */
/* See header file for definition of extension optional NULL    */
/* parameter.                                                   */
void Emit_Header (FILE *hfile, 
		  const char *name,
		  const char *const *interface_desc,
                  const char *extname /* Optional nul parameter */)
{
  int i;

  if (interface_desc) {
    for (i = 0; interface_desc[i] != NULL; ++i) {
      fprintf(hfile, "%s\n", interface_desc[i]);
    }
  }

  if(NULL==extname)
   { fprintf(hfile, "\n#ifndef %s_INCLUDED\n", name);
     fprintf(hfile, "#define %s_INCLUDED\n", name);
   }
  else
   { fprintf(hfile, "\n#ifndef %s_%s_INCLUDED\n",extname,name);
     fprintf(hfile, "#define %s_%s_INCLUDED\n",extname,name);
   }

  Emit_C_Header(hfile);

  /* Pull in appropriate stuff from common/com/defs.h here, so that
   * the header can be used whether defs.h has been included or not.
   *
   * For reconfigurability purpose, the file defs.h has been
   * splitted into two files:
   *   - defs.h (modified) has been modified
   *   - defs_exported.h (included in the modified version of defs.h).
   */
   fprintf(hfile,"%s\n",
     "#ifndef defs_INCLUDED\n"
     "#define defs_INCLUDED\n"
     "\n"
     "#include \"defs_exported.h\"\n"
     "\n"
     "/* Redefinition of inline keyword. Flag _FORCE_INLINE_INTO_STATIC_\n"
     " * is to be used with caution as a last chance solution!!\n"
     " */\n"
     "\n"
     "#if ((defined(_LANGUAGE_C) || defined(__GNUC__)) && !defined(inline)) || \\\n"
     "      defined(_FORCE_INLINE_INTO_STATIC_)\n"
     "#define inline static __inline\n"
     "#endif\n"
     "\n"
     "#endif  /* defs_INCLUDED */\n\n"
    );

   return;
}
#else
void Emit_Header (FILE *hfile, 
		  const char *name,
		  const char * const *interface_desc)
{
  int i;

  if (interface_desc) {
    for (i = 0; interface_desc[i] != NULL; ++i) {
      fprintf(hfile, "%s\n", interface_desc[i]);
    }
  }

  fprintf(hfile, "\n#ifndef %s_INCLUDED\n", name);
  fprintf(hfile, "#define %s_INCLUDED\n", name);

  fprintf (hfile, "#ifdef __cplusplus\n"
		  "extern \"C\" {\n"
		  "#endif\n\n");

  /* Pull in appropriate stuff from common/com/defs.h here, so that
   * the header can be used whether defs.h has been included or not.
   */
  /* mUINT32 was not portable to the LP64 abi and thus this hack 
 * which shouldn't exist does.. We should import defs.h or something
 * that would at least allow us to not duplicate defining these values
 */
  fprintf (hfile, "#include <inttypes.h>\n"
		  "#ifndef defs_INCLUDED\n"
		  "#define defs_INCLUDED\n"
		  "typedef signed int INT;\n"
		  "typedef signed int INT32;\n"
		  "typedef signed long long INT64;\n"
		  "typedef signed char mINT8;\n"
		  "typedef signed short mINT16;\n"
		  "typedef signed int mINT32;\n"
		  "typedef signed long long mINT64;\n"
		  "typedef unsigned int UINT;\n"
		  "typedef unsigned int UINT32;\n"
		  "typedef unsigned long long UINT64;\n"
		  "typedef unsigned char mUINT8;\n"
		  "typedef unsigned short mUINT16;\n"
		  "typedef uint64_t mUINT32;\n"
		  "typedef unsigned long long mUINT64;\n"
		  "typedef int BOOL;\n"
		  "typedef unsigned char mBOOL;\n"
		  "#ifndef TRUE\n"
		  "#define TRUE    ((BOOL) 1)\n"
		  "#endif\n"
		  "#ifndef FALSE\n"
		  "#define FALSE   ((BOOL) 0)\n"
		  "#endif\n"
		  "#if (defined(_LANGUAGE_C) || defined(__GNUC__)) && !defined(inline)\n"
		  "#define inline static __inline\n"
		  "#endif\n"
		  "#endif\n\n");
}
#endif
void Emit_Footer (FILE *hfile)
{
  fprintf (hfile, "\n#ifdef __cplusplus\n"
		  "}\n"
		  "#endif\n"
		  "#endif\n");    
}

typedef enum {
  DK_MACRO
} DEFINITION_KIND;

typedef struct definition {
  DEFINITION_KIND kind;
  const char *name;
  const char *s;
  struct definition *next;
} DEFINITION;

static DEFINITION *defs;
static DEFINITION *lastdef;

void Define_Macro (const char *name, const char *def)
{
  DEFINITION *newdef = new DEFINITION;
  newdef->kind = DK_MACRO;
  newdef->name = name;
  newdef->s = def;
  newdef->next = NULL;
  if (defs == NULL) {
    defs = newdef;
  } else {
    lastdef->next = newdef;
  }
  lastdef = newdef;
}

void Emit_Definitions (FILE *hfile, const char *prefix)
{
  DEFINITION *def;

  if (defs != NULL) fprintf(hfile, "\n");

  for (def = defs; def != NULL; def = def->next) {
    int c;
    int pos;
    const char *s = def->s;
    pos = fprintf(hfile, "#define %s%s ", prefix, def->name);
    while (pos++ < 40) fputc(' ', hfile);
    fprintf(hfile, "(\"");
    while (c = *s++) {
      if (c == '\\') {
	fprintf(hfile, "\\\\");
      } else if (c < ' ') {
	switch (c) {
	case '\n':
	  fprintf(hfile, "\\n");
	  break;
	case '\t':
	  fprintf(hfile, "\\t");
	  break;
	default:
	  fprintf(hfile, "\\%03o", c);
	  break;
	}
      } else {
	fputc(c, hfile);
      }
    }
    fprintf(hfile, "\")\n");
  }
}
#ifdef TARG_ST
/////////////////////////////////////
// Must be specified in each
// targinfo extension file at the 
// very beginning of main() function.
/////////////////////////////////////
void 
Set_Dynamic( char* extname )
{
   unsigned int size;

   Gen_Dyn_Code = true;

   if(NULL==extname || '\0'==extname[0])
     { fprintf(stderr,"### error: extension name is not specified or name is empty\n");
       exit(EXIT_FAILURE);
     }
   size = strlen(extname) + 1;
   extension_name = (char*)malloc(size);
   if(NULL==extension_name)
     { fprintf(stderr,"### error: unable to allocate %d bytes\n",size);
       exit(EXIT_FAILURE);
     }
   strcpy(extension_name,extname);

   return;
}

/////////////////////////////////////
FILE* 
Gen_Open_File_Handle( const char *const filename, const char *const mode )
/////////////////////////////////////
// Opening file handle.
////////////////////////////////////
{
   FILE *f = NULL;

   if(NULL==filename || '\0'==*filename)
    { fprintf(stderr,"### Missing file name, unable to open file\n");
      exit(EXIT_FAILURE);
    }

   f = fopen(filename,mode);
   if(NULL==f)
    { fprintf(stderr,"### Unable to open file %s\n",filename);
      exit(EXIT_FAILURE);
    }

   return f;
}

/////////////////////////////////////
void 
Gen_Close_File_Handle ( FILE *f, const char *const filename )
/////////////////////////////////////
// Closing file handles. Second parameter might be NULL.
/////////////////////////////////////
{
   if(NULL==f)                // Dummy case.
      return;

   if(fclose(f))
    { fprintf(stderr,"### Unable to close file handle");
      if(NULL!=filename && '\0'!=*filename)
        fprintf(stderr," (%s)\n",filename);
      else
        fprintf(stderr,"\n");
      exit(EXIT_FAILURE);
    }

   return;
}

/////////////////////////////////////
char * 
Gen_Build_Filename( const char * const fname,
                    const char * const extname,
                    const GEN_UTIL_FILE_TYPE type )
/////////////////////////////////////
// fname  : body file name (ex: targ_isa_print)
// extname: extension name. If not extension, extname is NULL or ""
/////////////////////////////////////
{
  unsigned int  length=0;
  const char   *str_end = NULL;
  char         *ret = NULL;
  bool          with_extension ; 
  const char   *true_extname;

  true_extname  = gen_util_file_type_dyn_hfile == type      ? "dyn" : extname;
  with_extension= true_extname!= NULL && *true_extname!='\0'? true  : false;

  //Firstly get temination string.
  switch(type)
   { case gen_util_file_type_cfile:
     str_end = "c";
     break;

     case gen_util_file_type_dyn_hfile:     /* Fall through */
     case gen_util_file_type_hfile:
     str_end = "h";
     break;

     case gen_util_file_type_efile:
     str_end = "Exported";
     break;

     case gen_util_file_type_pfile:
     str_end = "pr";
     break;

     case gen_util_file_type_c_i_file:
     str_end = "inc.c";
     break;

     case gen_util_file_type_cpp_i_file:
     str_end = "inc.cxx";
     break;

     default:
     fprintf(stderr,"### Internal error: unable to recognize file type\n");
     exit(EXIT_FAILURE);
     break;
   }            // end sw

  // Calculate string length.
  if(with_extension)
   { length += strlen(true_extname);
     length += strlen("_");
   }

  length+= strlen(fname);
  length+= strlen(".");
  length+= strlen(str_end);
  length++;          /* null terminating character */

  // Mem alloc.
  ret = (char*) malloc(length);
  if(NULL==ret)
   { fprintf(stderr,"### Unable to allocate %d bytes\n",length);
     exit(EXIT_FAILURE);
   }

  // Filling string.
  // Setting first character to '\0' allows to use
  // strcat in any case.
 *ret = '\0';

  if(with_extension)
   { strcat(ret,true_extname);
     strcat(ret,"_");
   }

  strcat(ret,fname);
  strcat(ret,".");
  strcat(ret,str_end);

  return ret;
}

/////////////////////////////////////
void 
Gen_Free_Filename( char* name )
/////////////////////////////////////
// name (char*) string allocated when
//              file name has been built.
/////////////////////////////////////
{
   if(name) free(name);

   return;
}


/////////////////////////////////////
void 
Emit_C_Header(FILE *hfile)
/////////////////////////////////////
// hfile (FILE*) file handle.
/////////////////////////////////////
{
  fprintf (hfile, "#ifdef __cplusplus\n"
          "extern \"C\" {\n"
           "#endif\n\n");

  return;
}

/////////////////////////////////////
void 
Emit_C_Footer(FILE *hfile)
/////////////////////////////////////
// hfile (FILE*) file handle.
/////////////////////////////////////
{
  fprintf (hfile, "\n#ifdef __cplusplus\n"
		  "}\n"
		  "#endif\n"
                  "\n");
  return;
}

/////////////////////////////////////
void
Emit_Stub_Header         (FILE *file, const char **headers)
/////////////////////////////////////
// file (FILE*): stub file handle.
// header_name : dyn name header to include
/////////////////////////////////////
{
  int i = 0;

  fprintf(file,
	  "/*\n"
	  " \n"
	  "  Copyright (C) 2006 ST Microelectronics, Inc.  All Rights Reserved.\n"
	  " \n"
	  "  This program is free software; you can redistribute it and/or modify it\n"
	  "  under the terms of version 2 of the GNU General Public License as\n"
	  "  published by the Free Software Foundation.\n"
	  "  This program is distributed in the hope that it would be useful, but\n"
	  "  WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	  "  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
	  " \n"
	  "  Further, this software is distributed without any warranty that it is\n"
	  "  free of the rightful claim of any third person regarding infringement\n"
	  "  or the like.  Any license provided herein, whether implied or\n"
	  "  otherwise, applies only to this software file.  Patent licenses, if\n"
	  "  any, provided herein do not apply to combinations of this program with\n"
	  "  other software, or any other product whatsoever.\n"
	  "  You should have received a copy of the GNU General Public License along\n"
	  "  with this program; if not, write the Free Software Foundation, Inc., 59\n"
	  "  Temple Place - Suite 330, Boston MA 02111-1307, USA.\n"
	  " \n"
	  "  Contact information:  ST Microelectronics, Inc.,\n"
	  "  , or:\n"
	  " \n"
	  "  http://www.st.com\n"
	  " \n"
	  "  For further information regarding this notice, see:\n"
	  " \n"
	  "  http:\n"
	  "*/\n"
	  " \n"
	  "/**\n"
	  " *\n"
	  " * This file is only useful for dynamic code extension.\n"
	  " *\n"
	  " * More precisely, when building the shared object (dll)\n"
	  " * the Open64 code generator needs some functions that have\n"
	  " * been processed in earlier step of its own process.\n"
	  " *\n"
	  " * The role devoted to files dyn_stubxxx.c is to emulate these\n"
	  " * functions when dynamic code generation is activated.\n"
	  " *\n"
	  " */\n"
	  "#ifndef DYNAMIC_CODE_GEN\n"
	  "#error \"File \" __FILE__ \" can only be used for dynamic code generation\\n\"\n"
	  "#endif\n"
	  " \n");
  while(*(headers[i]) != '\0') {
    fprintf(file,"#include %s\n",headers[i]);
    i++;
  }

  return;
}

/////////////////////////////////////
void 
Remove_Filename_Extension(const char *src_fname,
                          char       *tgt_fname)
/////////////////////////////////////
// src_fname (const char*) filename with extension
// tgt_fname (char*) buffer that will be filled with filename
//                   without the extension
/////////////////////////////////////
{
  while (*src_fname!='\0' && *src_fname!='.') {
	*tgt_fname++ = *src_fname++;
  }
  *tgt_fname = '\0';
  return;
}
#endif
