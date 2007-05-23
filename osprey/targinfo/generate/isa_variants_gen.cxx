/*


  Copyright (C) 2002, 2006 ST Microelectronics, Inc.  All Rights Reserved. 

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  ST Microelectronics, Inc., 
  , or: 

  http://www.st.com 

  For further information regarding this notice, see: 

  http: 

*/


//  isa_variants_gen.cxx
/////////////////////////////////////
//
//  Generate an interface for specifying variants of instructions
//  in the ISA.
//
/////////////////////////////////////
//


#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
// [HK]
#if __GNUC__ >=3 || defined(_MSC_VER)
#include <list>
#include <vector>
#include <utility>
using std::list;
using std::vector;
using std::pair;
#else
#include <list.h>
#include <vector.h>
#include <pair.h>
#endif // __GNUC__ >=3 || defined(_MSC_VER)

#ifdef DYNAMIC_CODE_GEN
#include "dyn_isa_topcode.h"   /* Dynamic TOPcode */
#else
#include "topcode.h"
#endif

#include "gen_util.h"
#include "isa_variants_gen.h"


// In following loops, we iterate on the number of
// TOP. This number differs following we generate
// static or dynamic TOP.
#ifndef DYNAMIC_CODE_GEN
static TOP TOP_count_limit = TOP_static_count;
#else
static TOP TOP_count_limit = TOP_dyn_count;
#endif

// ====================================================================
// ====================================================================

struct isa_variant_attribute_t {
  const char* name;         // Name given for documentation and debugging
  int bit_position;         // bit postion in flag word
  static int next_position; // Next position for bit position.
};

int isa_variant_attribute_t::next_position;

static list<ISA_VARIANT_ATT> attributes; // All the defined attributes

typedef pair<TOP,UINT64> variant_t;
typedef list<variant_t> variant_list_t;
typedef vector<variant_list_t> top_variants_list_t;
static top_variants_list_t top_variants; // List of variants for each top.

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   A description of the instruction variants in the ISA.",
  " *   The description exports the following:",
  " *",
  " *   VARATT_... identifiers",
  " *       An identifier for each variant attribute available.",
  " *",
  " *   TOP TOP_get_variant(TOP topcode, UINT64 atributes_set)",
  " *       Return the top variant matching the given set of attributes",
  " *       or TOP_UNDEFINED otherwise.",
  " *       The set of attributes is created by OR-ing VARATT_ attributes.",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};

/////////////////////////////////////
void ISA_Variants_Begin( const char* /* name */ )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  top_variants = top_variants_list_t(TOP_count_limit);
  return;
}


/////////////////////////////////////
ISA_VARIANT_ATT ISA_Variant_Attribute_Create( const char* name )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  ISA_VARIANT_ATT result = new isa_variant_attribute_t;

  result->name = name;
  result->bit_position = isa_variant_attribute_t::next_position++;
  
  attributes.push_back(result);

  return result;
}

void
ISA_Instruction_Variant(TOP src, TOP variant, ISA_VARIANT_ATT att1, ...)
{
  UINT64 attribute_mask;
  ISA_VARIANT_ATT att;

  va_list ap;
  va_start(ap,att1);

  attribute_mask = 1ULL << att1->bit_position;
  while ((att = va_arg(ap,ISA_VARIANT_ATT)) != 0 ) {
    attribute_mask |= 1ULL << att->bit_position;
  }
  top_variants[src].push_back(variant_t(variant,attribute_mask));
  
  va_end(ap);

}

/* ====================================================================
 *   ISA_Variants_End
 * ====================================================================
 */
void ISA_Variants_End(void)
{
  variant_list_t::iterator var_it;
  list<ISA_VARIANT_ATT>::iterator att_it;
  unsigned int code;

  static FILE* hfile    = NULL ;
  static FILE* cfile    = NULL ;
  static FILE* efile    = NULL ;

  // Whether we generate code for the core (static) or for an extension.
  bool  gen_static_code = Is_Static_Code();

  // Get extension name or NULL for static code generation.
  char *extname = gen_static_code ? NULL : Get_Extension_Name();

  char *hfilename     = NULL ;    /* Header file name              */
  char *cfilename     = NULL ;    /* C file name                   */
  char *efilename     = NULL ;    /* Export file name              */

  const char * const bname = FNAME_TARG_ISA_VARIANTS;

  hfilename = Gen_Build_Filename(bname,extname,gen_util_file_type_hfile);
  hfile     = Gen_Open_File_Handle(hfilename, "w");

  cfilename = Gen_Build_Filename(bname,extname,gen_util_file_type_cfile);
  cfile     = Gen_Open_File_Handle(cfilename, "w");

  if(gen_static_code) { 
    efilename = Gen_Build_Filename(bname,extname,gen_util_file_type_efile);
    efile     = Gen_Open_File_Handle(efilename, "w");
  }
  

  Emit_C_Header(cfile);
  fprintf(cfile,  
          "\n"
          "\n"
          "#include \"%s\"\n"
          "\n", 
          gen_static_code ? hfilename : "dyn_isa_variants.h");


  Emit_Header (hfile, FNAME_TARG_ISA_VARIANTS, interface, extname);
  fprintf(hfile,"#include \"topcode.h\"\n\n");
  
  char *int_type;
  char *int_suffix;
  int int_size;
  // Type used to be dynamically defined as a function of
  // the number of properties. For the sake of simplicity,
  // we work in all cases with unsigned long long.
  assert(isa_variant_attribute_t::next_position <= 64);
  int_type = "mUINT64";
  int_suffix = "ULL";
  int_size = 64;



  // Emit VARATT attribute values (only defined in static part)
  if(gen_static_code) {
    for ( att_it = attributes.begin(); att_it != attributes.end(); ++att_it ) {
      ISA_VARIANT_ATT attribute = *att_it;
      int bit_position = attribute->bit_position;
      fprintf (hfile, "#define VARATT_%-16s 0x%llx%s\n", 
	       attribute->name, 
	       (1ULL << bit_position),
	       int_suffix);
    }
    fprintf (hfile,"\n");

    // Emit Types information
    fprintf (hfile,"typedef struct { TOP top; UINT64 attributes; } ISA_TOP_VARIANT;\n");
    fprintf (hfile,"typedef struct { INT32 n_variants; const ISA_TOP_VARIANT *variants; } ISA_VARIANT_INFO;\n");
    fprintf (hfile,"typedef struct { const char *name; UINT64 attribute; } ISA_VARIANT_ATTRIBUTE;\n");
  }
  
  // Emit C file informations

  const char*top_prefix = gen_static_code ? "TOP_": "TOP_dyn_";
  const char* top_name_template = gen_static_code ? " /* TOP_%-25s */  " :  " /* TOP_dyn_%-25s */  " ;

  // Emit for each top the corresponding variant list
  int num = 0;
  for (code = 0; code < TOP_count_limit; code++) {
    if (!top_variants[code].empty()) {
      num++;
      fprintf(cfile,top_name_template, TOP_Name((TOP)code));
      fprintf (cfile,"\nstatic const ISA_TOP_VARIANT variants_%d[] = {\n", num);
      for (var_it = top_variants[code].begin(); var_it != top_variants[code].end(); var_it++) {
	variant_t variant = *var_it;
	fprintf (cfile," { /* %s%s */ %d, ", top_prefix, TOP_Name((TOP)variant.first), variant.first);
	if (variant.second != 0) {
	  const char *sep = "";
	  for ( att_it = attributes.begin(); att_it != attributes.end(); ++att_it ) {
	    ISA_VARIANT_ATT attribute = *att_it;
	    if ((1ULL<<attribute->bit_position) & variant.second) {
	      fprintf(cfile, "%sVARATT_%s", sep, attribute->name);
	      sep = "| ";
	    }
	  }
	} else {
	  fprintf(cfile, "0");
	}
	fprintf(cfile, " },\n");

      }
      fprintf (cfile,"};\n\n");
    }
  }

  // Emit Table ISA_VARIANTS_variants
  num = 0;
  fprintf (cfile,"static const ISA_VARIANT_INFO ISA_VARIANT_%s_infos[] = {\n",
	   gen_static_code? "static":"dynamic");

  for (code = 0; code < TOP_count_limit; code++) {
    fprintf(cfile,top_name_template, TOP_Name((TOP)code));
    if (!top_variants[code].empty()) {
      num++;
      fprintf (cfile,"  { %d, variants_%d},\n", top_variants[code].size(), num);
    } else {
      fprintf (cfile,"  { 0, (void *)0 }, \n");
    }
  }
  if(gen_static_code) {
    // don't forget the one for TOP_UNDEFINED !
    fprintf (cfile, top_name_template , "UNDEFINED");
    fprintf (cfile, "{0, (void *)0}\n");
  }
  fprintf (cfile,"};\n\n");
  // End Table ISA_VARIANTS_variants
  
  if(gen_static_code) { 
    fprintf(cfile,"const ISA_VARIANT_INFO *ISA_VARIANT_infos = ISA_VARIANT_static_infos ;\n");
  } else {
    // Definition of interface routine.
    const char * const name_routine = "dyn_get_ISA_VARIANT_INFO_tab";
    
    fprintf(cfile,"\n"
	    "const ISA_VARIANT_INFO * %s( void ) {\n"
	    " return (ISA_VARIANT_INFO *) ISA_VARIANT_dynamic_infos;\n"
	    "}\n\n",
	    name_routine);
    
    fprintf(hfile,"\n\n"
	    "/* API routine for dll */\n"
	    "extern const ISA_VARIANT_INFO * %s( void );\n\n",
	    name_routine);
  }

	  
  if (gen_static_code) {
    fprintf(hfile, "\n\nBE_EXPORTED extern TOP TOP_get_variant(TOP topcode, UINT64 attributes);\n");
    fprintf(hfile, "\n\nBE_EXPORTED extern const ISA_VARIANT_INFO *ISA_VARIANT_infos;\n");

    fprintf(cfile, "\n\nTOP TOP_get_variant(TOP topcode, UINT64 attributes)\n");
    fprintf(cfile, "{\n");
    fprintf(cfile, "  int i;\n");
    fprintf(cfile, "  const ISA_VARIANT_INFO info = ISA_VARIANT_infos[topcode];\n");
    fprintf(cfile, "  for (i = 0; i < info.n_variants; i++) {\n");
    fprintf(cfile, "    if (info.variants[i].attributes == attributes) return info.variants[i].top;\n");
    fprintf(cfile, "  }\n");
    fprintf(cfile, "  return TOP_UNDEFINED;\n");
    fprintf(cfile, "}\n\n");
  }


  // Generate inc.cxx file to be included by dynamic extensions.
  // The assumption is that only variant attributes defined for
  // the core can be used in extension.
  // The generated file will define for the extension all the
  // core variant attributes.
  if(gen_static_code) { 
    FILE *cincfile;
    char *cincfilename;
    
    cincfilename = Gen_Build_Filename(bname,extname,gen_util_file_type_cpp_i_file);
    cincfile     = Gen_Open_File_Handle(cincfilename, "w");
    
    fprintf(cincfile,
	    "\n\n"
	    "/* Warning, this file has been automatically generated\n"
	    " * Do not modify it.\n"
	    " * This file lists the core isa variant attributes.\n"
	    " */\n\n");

    fprintf(cincfile,
	    "#ifndef MAYBE_UNUSED\n"
	    "#ifdef __GNUC__\n"
	    "#define MAYBE_UNUSED __attribute__((unused))\n"
	    "#else\n"
	    "#define MAYBE_UNUSED \n"
	    "#endif\n"
	    "#endif\n\n");
    
    for(att_it = attributes.begin(); att_it != attributes.end(); ++att_it ) { 
      ISA_VARIANT_ATT attribute = *att_it;
      fprintf(cincfile,
	      "ISA_VARIANT_ATT att_%-15s MAYBE_UNUSED = ISA_Variant_Attribute_Create(\"%s\");\n",
	      attribute->name, attribute->name);
    }
    fprintf(cincfile,"\n\n");
    
    Gen_Close_File_Handle(cincfile,efilename);
  }


  // --------------------------------------------------------------------
  //
  //      ISA VARIANT attributes table and access functions
  //
  // Used in reconfigurability to check Core/Extension targinfo
  // compatibility and potentially allow attribute id remapping 
  // when loading extensions.
  //
  // --------------------------------------------------------------------

  fprintf (hfile,
	   "\n"
	   "/* Description of Used attributes */");
  fprintf (cfile,
	   "/* ============================================================\n"
	   " *  ISA VARIANT attributes description\n"
	   " * ============================================================\n"
	   " */\n");

  // Generate attribute description table
  fprintf (cfile, "static const ISA_VARIANT_ATTRIBUTE ISA_VARIANT_attribute_tab[] = {\n");
  for(att_it = attributes.begin(); att_it != attributes.end(); ++att_it ) { 
    ISA_VARIANT_ATT attribute = *att_it;
    fprintf (cfile, "  { \"%s\", VARATT_%s},\n", attribute->name, attribute->name);
  }
  fprintf (cfile, "};\n\n");
  
  // Generate access to attribute description table
  if (gen_static_code) {
    fprintf(hfile,
     "\n"
     "\n"
     "#define ISA_VARIANT_ATTRIBUTE_COUNT (%d)\n"
     "\n",
      attributes.size());

    fprintf (
      hfile,
     "BE_EXPORTED extern const ISA_VARIANT_ATTRIBUTE * ISA_VARIANT_get_attribute_table();\n");
    
    fprintf (
      cfile,
     "BE_EXPORTED const ISA_VARIANT_ATTRIBUTE * ISA_VARIANT_get_attribute_table() {\n"
     "  return (ISA_VARIANT_attribute_tab);\n"
     "}\n"
     "\n");
    fprintf(efile, "ISA_VARIANT_get_attribute_table\n");
  }
  else {
    fprintf (hfile,
	     "extern const mUINT32 dyn_get_ISA_VARIANT_attribute_tab_sz();\n"
	     "\n"
	     "extern const ISA_VARIANT_ATTRIBUTE* dyn_get_ISA_VARIANT_attribute_tab();\n"
	     "\n"
	     );
    
    fprintf (cfile,
	     "const mUINT32 dyn_get_ISA_VARIANT_attribute_tab_sz() {\n"
	     "  return (%d);\n"
	     "}\n"
	     "\n", attributes.size());
    fprintf (cfile,
	     "const ISA_VARIANT_ATTRIBUTE* dyn_get_ISA_VARIANT_attribute_tab() {\n"
	     "  return (ISA_VARIANT_attribute_tab);\n"
	     "}\n"
	     "\n");
  }


  Emit_Footer (hfile);
  Emit_C_Footer(cfile);               /* "C" directive   */

  // Closing file handlers.
  Gen_Close_File_Handle(hfile,hfilename);
  Gen_Close_File_Handle(cfile,cfilename);
  if(efile)
    Gen_Close_File_Handle(efile,efilename);

  // Memory deallocation.
  Gen_Free_Filename(cfilename);
  Gen_Free_Filename(hfilename);
  if(efilename)
    Gen_Free_Filename(efilename);

  return;
}
