/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


// isa_enums_gen.cxx
/////////////////////////////////////
//
//  Generate a list of enum classes and their values.
//
/////////////////////////////////////
//
//  $Revision: 1.5 $
//  $Date: 04/12/21 14:57:26-08:00 $
//  $Author: bos@eng-25.internal.keyresearch.com $
//  $Source: /home/bos/bk/kpro64-pending/common/targ_info/generate/SCCS/s.isa_enums_gen.cxx $

#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include "gen_util.h"
#include "isa_enums_gen.h"

typedef struct {
	const char *ecv_ecname;
	const char *ecv_name;
	int ecv_int;
} ECV_struct;

typedef struct {
	const char *ec_name;
#ifdef TARG_ST
        ISA_ENUM_EMIT ec_emit;
#endif
        int first_ecv;
	int last_ecv;
} EC_struct;

static std::vector<ECV_struct> all_ecv;
static std::vector<EC_struct> all_ec;

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   A list of all the enum classes used in an ISA.",
  " *   It exports the following:",
  " *",
  " *   typedef (enum) ISA_ENUM_CLASS",
  " *       An enumeration of the enum classes.",
  " *",
  " *   typedef (enum) ISA_ENUM_CLASS_VALUE",
  " *       An enumeration of the enum class values.",
  " *",
  " *   typedef (struct) ISA_ENUM_CLASS_INFO",
  " *       Contains info about first and last ECV in the EC.",
  " *       The contents are private.",
  " *",
  " *   typedef (struct) ISA_ENUM_CLASS_VALUE_INFO",
  " *       Contains info about name and int-value of the ECV.",
  " *       The contents are private.",
  " *",
  " *   const char * ISA_EC_Name (ISA_ENUM_CLASS)",
  " *       Returns name of EC.",
  " *",
  " *   ISA_ENUM_CLASS_VALUE ISA_EC_First_Value (ISA_ENUM_CLASS)",
  " *       Returns the first ECV for the specified EC.",
  " *",
  " *   ISA_ENUM_CLASS_VALUE ISA_EC_Last_Value (ISA_ENUM_CLASS)",
  " *       Returns the last ECV for the specified EC.",
  " *       Note that it assumes all ECV for an EC are in the",
  " *       first/last range given by the above two functions.",
  " *",
  " *   const char * ISA_ECV_Name (ISA_ENUM_CLASS_VALUE)",
  " *       Returns name of ECV.",
  " *",
  " *   INT ISA_ECV_Intval (ISA_ENUM_CLASS_VALUE)",
  " *       Returns int-value of ECV.",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};

#ifdef TARG_ST
static bool gen_static_code = true;          // Whether we generate code for an
static bool gen_dyn_code    = false;         // extension or for the core.

static char *extname        = NULL;          // Extension name (NULL if no ext).



/////////////////////////////////////
void ISA_Enums_Begin (void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  gen_static_code = Is_Static_Code();
  gen_dyn_code    =!gen_static_code;

   if(gen_dyn_code)
     extname = Get_Extension_Name();

  // start with undefined value
  // when code is generated for
  // the core.
  if(gen_static_code) {
    ECV_struct current_ecv = {"","UNDEFINED",UNDEFINED};
    all_ecv.push_back (current_ecv);
    EC_struct current_ec = {"UNDEFINED",ISA_ENUM_Emit_Name,0,0};
    all_ec.push_back (current_ec);
  }

  return;
}
#else
/////////////////////////////////////
void ISA_Enums_Begin (void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  // start with undefined value
  ECV_struct current_ecv = {"","UNDEFINED",UNDEFINED};
  all_ecv.push_back (current_ecv);
  EC_struct current_ec = {"UNDEFINED",0,0};
  all_ec.push_back (current_ec);
}
#endif
#ifdef TARG_ST
/////////////////////////////////////
void ISA_Create_Enum_Class ( const char* name, ISA_ENUM_EMIT emit, ...)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  const char *ecv_name;
  va_list ap;
  EC_struct current_ec;
  ECV_struct current_ecv;
  current_ec.ec_name = name;
  current_ec.ec_emit = emit;
  current_ec.first_ecv = all_ecv.size();
  va_start(ap, emit);
  do {	// loop through ecvs
    ecv_name = va_arg(ap, char*);
    current_ecv.ecv_ecname = name;
    current_ecv.ecv_name = ecv_name ? ecv_name : "";
    current_ecv.ecv_int = va_arg(ap, int);
    if (current_ecv.ecv_int == UNDEFINED)
	break;	// no default value
    all_ecv.push_back (current_ecv);
  } while (ecv_name != NULL);
  va_end(ap);
  current_ec.last_ecv = all_ecv.size() - 1;
  all_ec.push_back (current_ec);
}
#else
/////////////////////////////////////
void ISA_Create_Enum_Class ( const char* name, ...)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  const char *ecv_name;
  va_list ap;
  EC_struct current_ec;
  ECV_struct current_ecv;
  current_ec.ec_name = name;
  current_ec.first_ecv = all_ecv.size();
  va_start(ap, name);
  do {	// loop through ecvs
    ecv_name = va_arg(ap, char*);
    current_ecv.ecv_ecname = name;
    current_ecv.ecv_name = ecv_name ? ecv_name : "";
    current_ecv.ecv_int = va_arg(ap, int);
    if (current_ecv.ecv_int == UNDEFINED)
	break;	// no default value
    all_ecv.push_back (current_ecv);
  } while (ecv_name != NULL);
  va_end(ap);
  current_ec.last_ecv = all_ecv.size() - 1;
  all_ec.push_back (current_ec);
}
#endif


static char*
Print_ECV_EName (const char *name)
{
  // will print direct to file, so can use temp buffers
  static char buf[80];
  char *p = (char*) name;
  int i = 0;
  if (name == NULL)
    return const_cast<char*>("_none");
  else if (name[0] != '\0' && name[0] != '.' && name[0] != '_') {
    // insert leading _
    buf[0] = '_';
    ++i;
  }
  for ( ; *p != '\0'; ++p) {
    switch (*p) {
    case '.':
      buf[i++] = '_';
      break;
    case '@':
      // remove from name
      break;
    default:
      buf[i++] = *p;
      break;
    }
  }
  buf[i] = '\0';
  return buf;
}
#ifdef TARG_ST
/////////////////////////////////////
void ISA_Enums_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  char *hfilename = NULL ;
  char *cfilename = NULL ;
  char *sfilename = NULL ;
  char *efilename = NULL ;
  FILE *hfile     = NULL ;
  FILE *cfile     = NULL ;
  FILE *sfile     = NULL ;
  FILE *efile     = NULL ;

  const char* const bname = FNAME_TARG_ISA_ENUMS ;      /* base name */

  const char* static_class_info = "ISA_ENUM_CLASS_static_info";
  const char* static_class_value_info = "ISA_ENUM_CLASS_VALUE_static_info";
  const char* dynamic_class_info = "ISA_ENUM_CLASS_dynamic_info";
  const char* dynamic_class_value_info = "ISA_ENUM_CLASS_VALUE_dynamic_info";
  const char* class_info_ptrname = "ISA_ENUM_CLASS_info";
  const char* class_value_info_ptrname = "ISA_ENUM_CLASS_VALUE_info";

  std::vector<EC_struct>::iterator iec;
  std::vector<ECV_struct>::iterator iecv;
  ECV_struct tecv;

  hfilename = Gen_Build_Filename(bname,extname,gen_util_file_type_hfile);
  hfile     = Gen_Open_File_Handle(hfilename, "w");

  cfilename = Gen_Build_Filename(bname,extname,gen_util_file_type_cfile);
  cfile     = Gen_Open_File_Handle(cfilename, "w");

  if(gen_static_code) {
    efilename = Gen_Build_Filename(bname,extname,gen_util_file_type_efile);
    efile     = Gen_Open_File_Handle(efilename, "w");
  }
  else {
        const char *headers[] = {
      "<stddef.h>",
      "\"dyn_" FNAME_ISA_ENUMS ".h\"",
      "",
    };

    sfilename = Gen_Build_Filename(FNAME_STUB_ISA_ENUMS,
				   extname,
				   gen_util_file_type_cfile);
    sfile     = Gen_Open_File_Handle(sfilename,"w");
    Emit_Stub_Header(sfile,headers);
  }

  Emit_Header (hfile, bname, interface,extname);
  Emit_C_Header(cfile);

  if(gen_dyn_code) {
    char *dyn_hfilename = Gen_Build_Filename(FNAME_ISA_ENUMS,NULL,
                                              gen_util_file_type_dyn_hfile);
    fprintf(cfile,"#include <stdio.h>\n");
    fprintf(cfile,"#include \"%s\"\n",dyn_hfilename);
    Gen_Free_Filename(dyn_hfilename);
  }
  else {
    fprintf(cfile,"#include \"%s\"\n\n",hfilename);
  }

  if(gen_static_code) {
    fprintf(hfile, "\nenum {\n");
    for ( iec = all_ec.begin(); iec != all_ec.end(); ++iec) {
      fprintf(hfile, "\tISA_EC%s,\n", Print_ECV_EName(iec->ec_name));
    }
    fprintf(hfile, "\tISA_EC_STATIC_MAX\n");
    fprintf(hfile, "};\n");
    fprintf(hfile, "typedef mUINT32 ISA_ENUM_CLASS;\n\n");
    fprintf(hfile, "extern mUINT32 ISA_EC_MAX;\n\n");
    fprintf(cfile, "BE_EXPORTED mUINT32 ISA_EC_MAX = ISA_EC_STATIC_MAX;\n\n");

    fprintf(hfile, "\nenum {\n");
    for ( iecv = all_ecv.begin(); iecv != all_ecv.end(); ++iecv) {
      // have to use multiple calls since Print_ECV_EName uses a static bufr
      fprintf(hfile, "\tISA_ECV%s", Print_ECV_EName (iecv->ecv_ecname));
      fprintf(hfile, "%s,\n", Print_ECV_EName (iecv->ecv_name));
    }
    fprintf(hfile, "\tISA_ECV_STATIC_MAX\n");
    fprintf(hfile, "};\n");
    fprintf(hfile, "typedef mUINT32 ISA_ENUM_CLASS_VALUE;\n\n");
    fprintf(hfile, "extern mUINT32 ISA_ECV_MAX;\n\n");
    fprintf(cfile, "BE_EXPORTED mUINT32 ISA_ECV_MAX = ISA_ECV_STATIC_MAX;\n\n");

  }
  else {
    fprintf(sfile, "BE_EXPORTED mUINT32 ISA_EC_MAX = 0;\n\n");
    fprintf(sfile, "BE_EXPORTED mUINT32 ISA_ECV_MAX = 0;\n\n");
  }

  /*==================================================================*/

  if(gen_static_code) {
    fprintf(hfile, "\ntypedef struct {\n"
	    "  char *name;\n"
	    "  BOOL emit_name; /* Emit enum name if true, else emit its value. */\n"
	    "  ISA_ENUM_CLASS_VALUE first;\n"
	    "  ISA_ENUM_CLASS_VALUE last;\n"
	    "} ISA_ENUM_CLASS_INFO;\n");
  }

  if(gen_static_code) {
    fprintf(cfile, "static const ISA_ENUM_CLASS_INFO %s[] = {\n",static_class_info);
  }
  else {
    fprintf(cfile, "static const ISA_ENUM_CLASS_INFO %s[] = {\n",dynamic_class_info);
  }
  for ( iec = all_ec.begin(); iec != all_ec.end(); ++iec) {
    fprintf(cfile, "\t{ \"ISA_EC%s\", %d,",
	    Print_ECV_EName(iec->ec_name),
	    iec->ec_emit == ISA_ENUM_Emit_Name ? 1 : 0);
    tecv = all_ecv[iec->first_ecv];
    // have to use multiple calls since Print_ECV_EName uses a static bufr
    fprintf(cfile, "\tISA_ECV%s", Print_ECV_EName(tecv.ecv_ecname));
    fprintf(cfile, "%s,", Print_ECV_EName(tecv.ecv_name));
    tecv = all_ecv[iec->last_ecv];
    // have to use multiple calls since Print_ECV_EName uses a static bufr
    fprintf(cfile, "\tISA_ECV%s", Print_ECV_EName(tecv.ecv_ecname));
    fprintf(cfile, "%s },\n", Print_ECV_EName(tecv.ecv_name));
  }
  fprintf(cfile, "};\n");

  if(gen_static_code) {
    fprintf(hfile, 
	    "BE_EXPORTED extern const ISA_ENUM_CLASS_INFO *%s;\n",
	    class_info_ptrname);
    fprintf(efile, 
	    "%s\n",
	    class_info_ptrname);
    fprintf(cfile,
	    "BE_EXPORTED const ISA_ENUM_CLASS_INFO *%s = %s;\n\n",
	    class_info_ptrname,static_class_info);
  }
  else {
    fprintf(sfile,
	    "BE_EXPORTED const ISA_ENUM_CLASS_INFO *%s = NULL;\n\n",
	    class_info_ptrname);
  }

  /*==================================================================*/
  
  if(gen_static_code) {
    fprintf(hfile, "\ntypedef struct {\n"
	    "  char *name;\n"
	    "  INT intval;\n"
	    "} ISA_ENUM_CLASS_VALUE_INFO;\n");
    fprintf(cfile, "static const ISA_ENUM_CLASS_VALUE_INFO %s[] = {\n",static_class_value_info);
  }
  else {
    fprintf(cfile, "static const ISA_ENUM_CLASS_VALUE_INFO %s[] = {\n",dynamic_class_value_info);
  }

  for ( iecv = all_ecv.begin(); iecv != all_ecv.end(); ++iecv) {
  	fprintf(cfile, "\t{ \"%s\",\t%d },\n", iecv->ecv_name, iecv->ecv_int);
  }
  fprintf(cfile, "};\n");

  if(gen_static_code) {
    fprintf(hfile, 
	    "BE_EXPORTED extern const ISA_ENUM_CLASS_VALUE_INFO *%s;\n",
	    class_value_info_ptrname);
    fprintf(efile, 
	    "%s\n",
	    class_value_info_ptrname);
    fprintf(cfile,
	    "BE_EXPORTED const ISA_ENUM_CLASS_VALUE_INFO *%s = %s;\n",
	    class_value_info_ptrname,static_class_value_info);
  }
  else {
    fprintf(sfile,
	    "BE_EXPORTED const ISA_ENUM_CLASS_VALUE_INFO *%s = NULL;\n",
	    class_value_info_ptrname);
  }

  /*==================================================================*/

  if(gen_static_code) {
    fprintf(hfile,
	    "inline const char * ISA_EC_Name (ISA_ENUM_CLASS ec)\n"
	    "{\n"
	    "  return ISA_ENUM_CLASS_info[ec].name;\n"
	    "}\n\n");
    
    fprintf(hfile,
	    "inline BOOL ISA_EC_Emit_Name (ISA_ENUM_CLASS ec)\n"
	    "{\n"
	    "  return ISA_ENUM_CLASS_info[ec].emit_name;\n"
	    "}\n\n");
    
    fprintf(hfile,
	    "inline ISA_ENUM_CLASS_VALUE ISA_EC_First_Value (ISA_ENUM_CLASS ec)\n"
	    "{\n"
	    "  return ISA_ENUM_CLASS_info[ec].first;\n"
	    "}\n\n");
    
    fprintf(hfile,
	    "inline ISA_ENUM_CLASS_VALUE ISA_EC_Last_Value (ISA_ENUM_CLASS ec)\n"
	    "{\n"
	    "  return ISA_ENUM_CLASS_info[ec].last;\n"
	    "}\n\n");

    fprintf(hfile,
	    "inline const char * ISA_ECV_Name (ISA_ENUM_CLASS_VALUE ecv)\n"
	    "{\n"
	    "  return ISA_ENUM_CLASS_VALUE_info[ecv].name;\n"
	    "}\n\n");
    
    fprintf(hfile,
	    "inline INT ISA_ECV_Intval (ISA_ENUM_CLASS_VALUE ecv)\n"
	    "{\n"
	    "  return ISA_ENUM_CLASS_VALUE_info[ecv].intval;\n"
	    "}\n\n");
  }

  if (gen_dyn_code) {
    const char* fct_name1 = "dyn_get_ISA_ENUM_CLASS_info_tab";
    const char* fct_name2 = "dyn_get_ISA_ENUM_CLASS_info_tab_sz";
    const char* fct_name3 = "dyn_get_ISA_ENUM_CLASS_VALUE_info_tab";
    const char* fct_name4 = "dyn_get_ISA_ENUM_CLASS_VALUE_info_tab_sz";


    /* Empty routines */
    fprintf(cfile,
	    "const ISA_ENUM_CLASS_INFO* %s(void)\n"
            "{\n"
            " return %s;\n"
            "};\n"
            "\n",fct_name1,dynamic_class_info);

    fprintf(cfile,
	    "const mUINT32 %s(void)\n"
            "{\n"
            " return (const mUINT32)%d;\n"
            "};\n"
            "\n",fct_name2,all_ec.size());
    fprintf(cfile,
	    "const ISA_ENUM_CLASS_VALUE_INFO* %s(void)\n"
            "{\n"
            " return %s;\n"
            "};\n"
            "\n",fct_name3,dynamic_class_value_info);

    fprintf(cfile,
	    "const mUINT32 %s(void)\n"
            "{\n"
            " return (const mUINT32)%d;\n"
            "};\n"
            "\n",fct_name4,all_ecv.size());

    fprintf(hfile,
	    "\n\n"
	    "extern const ISA_ENUM_CLASS_INFO* %s(void);\n"
	    "extern const mUINT32 %s(void);\n"
	    "extern const ISA_ENUM_CLASS_VALUE_INFO* %s(void);\n"
	    "extern const mUINT32 %s(void);\n"
            ,fct_name1,fct_name2,fct_name3,fct_name4);
  }


  fprintf(hfile, "#ifdef DYNAMIC_CODE_GEN\n");
  fprintf(hfile, "extern void ISA_ENUMS_Initialize_Stub(void);\n");
  fprintf(hfile, "#endif\n");
  

  if(!gen_static_code) {
    fprintf(sfile,
	    "\n"
	    "/*\n"
	    " * Exported routine.\n"
	    " */\n"
	    "void \n"
	    "ISA_ENUMS_Initialize_Stub( void )\n"
	    "{\n"
	    "  ISA_ENUM_CLASS_info = (ISA_ENUM_CLASS_INFO*)dyn_get_ISA_ENUM_CLASS_info_tab();\n"
	    "  ISA_EC_MAX = dyn_get_ISA_ENUM_CLASS_info_tab_sz();\n"
	    "  ISA_ENUM_CLASS_VALUE_info = (ISA_ENUM_CLASS_VALUE_INFO*)dyn_get_ISA_ENUM_CLASS_VALUE_info_tab();\n"
	    "  ISA_ECV_MAX = dyn_get_ISA_ENUM_CLASS_VALUE_info_tab_sz();\n"
	    "  return;\n"
	    "}\n");
  }

  Emit_Footer (hfile);
  Emit_C_Footer(cfile);


  // Closing file handlers.
  if(hfile) Gen_Close_File_Handle(hfile,hfilename);
  if(cfile) Gen_Close_File_Handle(cfile,cfilename);
  if(sfile) Gen_Close_File_Handle(sfile,sfilename);
  if(efile) Gen_Close_File_Handle(efile,efilename);

  // Memory deallocation.
  if(cfilename) Gen_Free_Filename(cfilename);
  if(sfilename) Gen_Free_Filename(sfilename);
  if(hfilename) Gen_Free_Filename(hfilename);
  if(efilename) Gen_Free_Filename(efilename);

  return;
}
#else

/////////////////////////////////////
void ISA_Enums_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  std::vector<EC_struct>::iterator iec;
  std::vector<ECV_struct>::iterator iecv;
  ECV_struct tecv;

#define FNAME "targ_isa_enums"
  char buf[1000];
  sprintf (buf, "%s.h", FNAME);
  FILE* hfile = fopen(buf, "w");
  sprintf (buf, "%s.c", FNAME);
  FILE* cfile = fopen(buf, "w");
  sprintf (buf, "%s.Exported", FNAME);
  FILE* efile = fopen(buf, "w");

  fprintf(cfile,"#include \"%s.h\"\n\n", FNAME);

  sprintf (buf, "%s", FNAME);
  Emit_Header (hfile, buf, interface);

  fprintf(hfile, "\ntypedef enum {\n");
  for ( iec = all_ec.begin(); iec != all_ec.end(); ++iec) {
  	fprintf(hfile, "\tEC%s,\n", Print_ECV_EName(iec->ec_name));
  }
  fprintf(hfile, "\tEC_MAX\n");
  fprintf(hfile, "} ISA_ENUM_CLASS;\n");
  fprintf(hfile, "\ntypedef enum {\n");
  for ( iecv = all_ecv.begin(); iecv != all_ecv.end(); ++iecv) {
	// have to use multiple calls since Print_ECV_EName uses a static bufr
  	fprintf(hfile, "\tECV%s", Print_ECV_EName (iecv->ecv_ecname));
  	fprintf(hfile, "%s,\n", Print_ECV_EName (iecv->ecv_name));
  }
  fprintf(hfile, "\tECV_MAX\n");
  fprintf(hfile, "} ISA_ENUM_CLASS_VALUE;\n");

  fprintf(hfile, "\ntypedef struct {\n"
		"  char *name;\n"
		"  ISA_ENUM_CLASS_VALUE first;\n"
		"  ISA_ENUM_CLASS_VALUE last;\n"
		"} ISA_ENUM_CLASS_INFO;\n");
  fprintf(hfile, "extern const ISA_ENUM_CLASS_INFO ISA_ENUM_CLASS_info[];\n");
  fprintf(efile, "ISA_ENUM_CLASS_info\n");
  fprintf(cfile, "const ISA_ENUM_CLASS_INFO ISA_ENUM_CLASS_info[] = {\n");
  for ( iec = all_ec.begin(); iec != all_ec.end(); ++iec) {
  	fprintf(cfile, "\t{ \"EC%s\",", Print_ECV_EName(iec->ec_name));
	tecv = all_ecv[iec->first_ecv];
	// have to use multiple calls since Print_ECV_EName uses a static bufr
  	fprintf(cfile, "\tECV%s", Print_ECV_EName(tecv.ecv_ecname));
  	fprintf(cfile, "%s,", Print_ECV_EName(tecv.ecv_name));
	tecv = all_ecv[iec->last_ecv];
	// have to use multiple calls since Print_ECV_EName uses a static bufr
  	fprintf(cfile, "\tECV%s", Print_ECV_EName(tecv.ecv_ecname));
  	fprintf(cfile, "%s },\n", Print_ECV_EName(tecv.ecv_name));
  }
  fprintf(cfile, "};\n\n");

  fprintf(hfile, "\ntypedef struct {\n"
		"  char *name;\n"
		"  INT intval;\n"
		"} ISA_ENUM_CLASS_VALUE_INFO;\n");
  fprintf(hfile, "extern const ISA_ENUM_CLASS_VALUE_INFO ISA_ENUM_CLASS_VALUE_info[];\n\n");
  fprintf(efile, "ISA_ENUM_CLASS_VALUE_info\n");
  fprintf(cfile, "const ISA_ENUM_CLASS_VALUE_INFO ISA_ENUM_CLASS_VALUE_info[] = {\n");
  for ( iecv = all_ecv.begin(); iecv != all_ecv.end(); ++iecv) {
  	fprintf(cfile, "\t{ \"%s\",\t%d },\n", iecv->ecv_name, iecv->ecv_int);
  }
  fprintf(cfile, "};\n\n");

  fprintf(hfile, "inline const char * ISA_EC_Name (ISA_ENUM_CLASS ec)\n"
		 "{\n"
		 "  return ISA_ENUM_CLASS_info[ec].name;\n"
		 "}\n\n");

  fprintf(hfile, "inline ISA_ENUM_CLASS_VALUE ISA_EC_First_Value (ISA_ENUM_CLASS ec)\n"
		 "{\n"
		 "  return ISA_ENUM_CLASS_info[ec].first;\n"
		 "}\n\n");

  fprintf(hfile, "inline ISA_ENUM_CLASS_VALUE ISA_EC_Last_Value (ISA_ENUM_CLASS ec)\n"
		 "{\n"
		 "  return ISA_ENUM_CLASS_info[ec].last;\n"
		 "}\n\n");

  fprintf(hfile, "inline const char * ISA_ECV_Name (ISA_ENUM_CLASS_VALUE ecv)\n"
		 "{\n"
		 "  return ISA_ENUM_CLASS_VALUE_info[ecv].name;\n"
		 "}\n\n");

  fprintf(hfile, "inline INT ISA_ECV_Intval (ISA_ENUM_CLASS_VALUE ecv)\n"
		 "{\n"
		 "  return ISA_ENUM_CLASS_VALUE_info[ecv].intval;\n"
		 "}\n\n");

  Emit_Footer (hfile);
}
#endif
