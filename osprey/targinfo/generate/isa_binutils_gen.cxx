/*

  Copyright (C) 2007-2008 STMicroelectronics, Inc.  All Rights Reserved.

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

*/

/*
 * This file has been added by STMicroelectronics.
 *
 */


/////////////////////////////////////
// isa_binutils_gen.cxx
/////////////////////////////////////
//
//  Generate an interface for connecting GNU Binary Utilities to targinfo.
//
/////////////////////////////////////
//

#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#if __GNUC__ >=3 || defined(_MSC_VER)
#include <list>
#include <vector>
using std::list;
using std::vector;
#else
#include <list.h>
#include <vector.h>
#endif // __GNUC__ >=3 || defined(_MSC_VER)

#include "gen_util.h"
#ifdef DYNAMIC_CODE_GEN
#include "dyn_isa_subset.h"
#include "dyn_isa_relocs.h"
#include "dyn_isa_pack.h"
#include "dyn_isa_operands.h"
#else
#include "targ_isa_subset.h"
#include "targ_isa_relocs.h"
#include "targ_isa_pack.h"
#include "targ_isa_operands.h"
#endif
#include "isa_binutils_gen.h"

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   A description of how to connect the GNU Binary Utilities to ",
  " *   targinfo. The description exports the following:",
  " *",
  " *   typedef (function) ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING",
  " *       A function to export the extension relocated encoding if an ",
  " *       extension provides the support.",
  " *",
  " *   typedef (struct) ISA_BINUTILS_INFO",
  " *       Identify the extension features from a binary utility point of",
  " *       view:",
  " *       name:                   Extension name.",
  " *       abstract:               Description of extension (long).",
  " *       short_abstract:         Description of extension (short).",
  " *       magic_key:              Unique key identifier.",
  " *       port_size:              Number of entry in port.",
  " *       port:                   Ports used ('port_size' entries).",
  " *       last_scratch:           Id of last scratch register. -1 for all",
  " *                               all register scratch (default).",
  " *       ext_rel_enc:            Function to manage extension relocate encoding.",
  " *       emul_byte_nr:           Number of bytes reserved in memory for",
  " *                               emulation purposes.",
  " *       DWARF_reg_nr:           Number of DWARF registers defined by",
  " *                               extension.",
  " *       xversion:               Version of extension out of CORXpert.",
  " *",
  " *   typedef (struct) ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING",
  " *       Identify the :",
  " *       symbol:    Unique symbol identifying the relocated instruction.",
  " *       top:       Top associated to the relocated instruction.",
  " *       mask:      Mask to be applied on instruction.",
  " *       encoding:  Encoding bits to be applied for instruction.",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};

int PORT_SIZE = 0;
#define PORT_END -1
typedef struct {
   const char *abstract;
   const char *short_abstract;
   mUINT32 magic_key;
     INT   last_scratch;
   mUINT32 emul_byte_nr;
   mUINT32 DWARF_reg_nr;
   mUINT32 xversion;
   mUINT32 *port;
   const char ** name;
   mUINT32 ext_rel_enc;
} BINUTILS_TYPE;

#define MAX_OPERANDS 10
typedef struct {
  char * symbol;
  TOP top;
  mUINT64 mask;
  mUINT64 encoding;
} EXTENSION_RELOCATE_ENCODING;

static vector<BINUTILS_TYPE*> binutils;
typedef vector<EXTENSION_RELOCATE_ENCODING*> encoding;
static vector<encoding> extension_relocate_encoding;

/////////////////////////////////////
void ISA_Binutils_Begin( int portsize )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
#ifdef DYNAMIC_CODE_GEN
  ISA_RELOCS_Initialize_Stub();
#endif
  
  binutils.resize(ISA_SUBSET_count);
  extension_relocate_encoding.resize(ISA_SUBSET_count);
  PORT_SIZE = portsize;
}

/////////////////////////////////////
void ISA_Binutils_Create(ISA_SUBSET subset,
			 const char *abstract,
			 const char *short_abstract,
			 mUINT32 magic_key,
			   INT   last_scratch,
			 mUINT32 emul_byte_nr,
			 mUINT32 DWARF_reg_nr,
			 mUINT32 xversion,
			 ... ) 
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  va_list ap;
  UINT32 port;
  char * name;
  BINUTILS_TYPE * bu_type;
  
  if (subset >= ISA_SUBSET_count) {
     fprintf(stderr,"### Error: wrong subset identifier %d\n",subset);
     exit(EXIT_FAILURE);
  }

  bu_type = new BINUTILS_TYPE;
  bu_type->abstract = strdup(abstract);
  bu_type->short_abstract = strdup(short_abstract);
  bu_type->magic_key = magic_key;
  bu_type->last_scratch = last_scratch;
  bu_type->emul_byte_nr = emul_byte_nr;
  bu_type->DWARF_reg_nr = DWARF_reg_nr;
  bu_type->xversion = xversion;
  bu_type->ext_rel_enc = 0;
  bu_type->port = (mUINT32*)malloc(PORT_SIZE*sizeof(mUINT32));
  memset(bu_type->port,0,PORT_SIZE*sizeof(mUINT32));
  bu_type->name = (const char**)malloc(PORT_SIZE*sizeof(const char*));
  va_start(ap,xversion);
  while( ((port=va_arg(ap,UINT32)) != PORT_END) &&
         ((name=va_arg(ap,char*)) != NULL) ) {
    if (port >= PORT_SIZE) {
       fprintf(stderr,"### Error: unexpected port index (%d).\n",port);
       exit(EXIT_FAILURE);
    }
    bu_type->port[port] = 1;
    bu_type->name[port] = strdup(name);
  }
  va_end(ap);

  binutils[subset] = bu_type;
  return;
}

/////////////////////////////////////
static mUINT64 ISA_Binutils_Pack ( TOP top )
/////////////////////////////////////
// Extracts encoding for top assuming
// encoding for each operand and results is 0
/////////////////////////////////////
{
  mUINT64 encoding = 0;
  INT comp;
  INT words;
  INT w;
  const ISA_PACK_OPND_INFO *oinfo = ISA_PACK_OPND_Info(top);
  const ISA_PACK_INFO *pinfo = ISA_PACK_Info(top);
  
  words = ISA_PACK_Inst_Words(top);
  for (w = 0; w < words; ++w) {
    ISA_PACK_INST inst = ISA_PACK_INFO_Init_Mask(pinfo, w);
    do {
      UINT64 mask = ISA_PACK_OPND_INFO_Mask(oinfo);
      UINT opndpos = ISA_PACK_OPND_INFO_OpndPos(oinfo);
      UINT instpos = ISA_PACK_OPND_INFO_InstPos(oinfo);
      comp = ISA_PACK_OPND_INFO_Comp(oinfo);
      
      if(ISA_PACK_COMP_opnd <= comp &&
	 comp < ISA_PACK_COMP_opnd + ISA_OPERAND_max_operands) {
	INT n = comp - ISA_PACK_COMP_opnd;
	inst |= ((/*bopnd[n]*/ 0 >> opndpos) & mask) << instpos;
      }
      else if(ISA_PACK_COMP_result <= comp &&
	      comp < ISA_PACK_COMP_result + ISA_OPERAND_max_results) {
	INT n = comp - ISA_PACK_COMP_result;
	inst |= ((/*bresult[n]*/ 0 >> opndpos) & mask) << instpos;
      }
      else if(comp == ISA_PACK_COMP_end) {
        encoding |= inst << (16*w);
      }
      else {
        fprintf(stderr,"### Error: Unhandled packing component %d for %s",
		comp, TOP_Name(top));
        exit(EXIT_FAILURE);
      }
    } while (++oinfo, comp != ISA_PACK_COMP_end);
  }

  return encoding;
}

/////////////////////////////////////
void ISA_Binutils_Extension_Relocate_Encoding(ISA_SUBSET subset, ...)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  va_list ap;
  EXTENSION_RELOCATE_ENCODING * ext_rel_encoding;
  
  char * symbol;
  mUINT32 nb_op;
  
  if (subset >= ISA_SUBSET_count) {
     fprintf(stderr,"### Error: wrong subset identifier %d\n",subset);
     exit(EXIT_FAILURE);
  }
  
  va_start(ap,subset);
  while(((symbol=va_arg(ap,char*)) != NULL)) {
    ext_rel_encoding = new EXTENSION_RELOCATE_ENCODING;
    ext_rel_encoding->symbol = strdup(symbol);
    ext_rel_encoding->top=va_arg(ap,TOP);
    ext_rel_encoding->mask=va_arg(ap,mUINT64);
    ext_rel_encoding->encoding=ISA_Binutils_Pack(ext_rel_encoding->top);
    extension_relocate_encoding[subset].push_back(ext_rel_encoding);
  }
  va_end(ap);

  binutils[subset]->ext_rel_enc = 1;
}

/////////////////////////////////////
void ISA_Binutils_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  FILE* hfile = NULL ;
  FILE* cfile = NULL ;
  FILE* sfile = NULL ;
  FILE* efile = NULL ;

  int   nb_subsets;
  ISA_SUBSET subset;
  
  // Whether we generate code for the core (static) or for an extension.
  bool  gen_static_code = Is_Static_Code();

  // Get extension name or NULL for static code generation.
  char *extname = gen_static_code ? NULL : Get_Extension_Name();

  // File handler initialization.
  char *hfilename     = NULL ;    /* Header file name              */
  char *cfilename     = NULL ;    /* C file name                   */
  char *sfilename     = NULL ;    /* Stub file name                */
  char *efilename     = NULL ;    /* Export file name              */

  const char * const bname = FNAME_TARG_ISA_BINUTILS;


  hfilename = Gen_Build_Filename(bname,extname,gen_util_file_type_hfile);
  hfile     = Gen_Open_File_Handle(hfilename, "w");

  cfilename = Gen_Build_Filename(bname,extname,gen_util_file_type_cfile);
  cfile     = Gen_Open_File_Handle(cfilename, "w");

  if (gen_static_code) { 
     efilename = Gen_Build_Filename(bname,extname,gen_util_file_type_efile);
     efile     = Gen_Open_File_Handle(efilename, "w");
  } else {
    const char *headers[] = {
      "<stddef.h>",
      "\"dyn_" FNAME_ISA_BINUTILS ".h\"",
      "",
    };

    sfilename = Gen_Build_Filename(FNAME_STUB_ISA_BINUTILS,
				   extname,
				   gen_util_file_type_cfile);
    sfile     = Gen_Open_File_Handle(sfilename,"w");
    Emit_Stub_Header(sfile,headers);
  }

  /* For dynamic extensions, we want to emit C and not C++     */
  Emit_C_Header(cfile);     /* Emit ifdef _cplusplus directive */
  fprintf(cfile,
          "\n"
          "#include <string.h>\n");

  fprintf(cfile,"#include \"%s\"\n\n", 
          gen_static_code ? "topcode.h" : "dyn_isa_topcode.h");
  fprintf(cfile,"#include \"%s\"\n\n", 
          gen_static_code ? "targ_isa_subset.h" : "dyn_isa_subset.h");
  fprintf(cfile,"#include \"%s\"\n\n", 
          gen_static_code ? hfilename : "dyn_isa_binutils.h");

  Emit_Header (hfile, bname, interface,extname);

  if (gen_static_code) {
    fprintf(hfile,
            "#define MAX_OPERANDS 10\n"
            "typedef struct {\n"
            "  char * symbol;\n"
            "  TOP top;\n"
	    "  mUINT64 mask;\n"
	    "  mUINT64 encoding;\n"
            "} ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO;\n\n");
    fprintf(hfile,
            "typedef mUINT32 (* ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING) (ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO**, mUINT32);\n\n");
    fprintf(hfile,
            "typedef mUINT32 (* ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_NR) (void);\n\n");
    fprintf(hfile,
            "typedef struct {\n"
	    "  const char * name;\n"
	    "  const char * abstract;\n"
	    "  const char * short_abstract;\n"
	    "  mUINT32 magic_key;\n"
	    "  mUINT32 port_size;\n"
	    "  mUINT32 *port;\n"
	    "    INT   last_scratch;\n"
	    "  ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING ext_rel_enc;\n"
	    "  ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_NR ext_rel_enc_nr;\n"
	    "  mUINT32 emul_byte_nr;\n"
	    "  mUINT32 DWARF_reg_nr;\n"
	    "  mUINT32 xversion;\n"
	    "} ISA_BINUTILS_INFO;\n\n");
    fprintf(hfile,
            "inline const char * ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO_symbol (ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO * ext_enc_rel) {\n"
	    "  return ext_enc_rel->symbol;\n"
	    "}\n\n"
            "inline TOP ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO_top (ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO * ext_enc_rel) {\n"
	    "  return ext_enc_rel->top;\n"
	    "}\n\n"
            "inline mUINT64 ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO_mask (ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO * ext_enc_rel) {\n"
	    "  return ext_enc_rel->mask;\n"
	    "}\n\n"
            "inline mUINT64 ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO_encoding (ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO * ext_enc_rel) {\n"
	    "  return ext_enc_rel->encoding;\n"
	    "}\n\n"
            "extern ISA_BINUTILS_INFO* ISA_BINUTILS_info;\n\n"
            "inline const char * ISA_BINUTILS_INFO_name (ISA_SUBSET subset) {\n"
	    "  return ISA_BINUTILS_info[subset].name;\n"
	    "}\n\n"
            "inline const char * ISA_BINUTILS_INFO_abstract (ISA_SUBSET subset) {\n"
	    "  return ISA_BINUTILS_info[subset].abstract;\n"
	    "}\n\n"
            "inline const char * ISA_BINUTILS_INFO_short_abstract (ISA_SUBSET subset) {\n"
	    "  return ISA_BINUTILS_info[subset].short_abstract;\n"
	    "}\n\n"
            "inline mUINT32 ISA_BINUTILS_INFO_magic_key (ISA_SUBSET subset) {\n"
	    "  return ISA_BINUTILS_info[subset].magic_key;\n"
	    "}\n\n"
            "inline mUINT32 ISA_BINUTILS_INFO_port_size (ISA_SUBSET subset) {\n"
	    "  return ISA_BINUTILS_info[subset].port_size;\n"
	    "}\n\n"
            "inline mUINT32 * ISA_BINUTILS_INFO_port (ISA_SUBSET subset) {\n"
	    "  return ISA_BINUTILS_info[subset].port;\n"
	    "}\n\n"
            "inline mUINT32 ISA_BINUTILS_INFO_last_scratch (ISA_SUBSET subset) {\n"
	    "  return ISA_BINUTILS_info[subset].last_scratch;\n"
	    "}\n\n"
            "inline ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING ISA_BINUTILS_INFO_ext_rel_enc (ISA_SUBSET subset) {\n"
	    "  return ISA_BINUTILS_info[subset].ext_rel_enc;\n"
	    "}\n\n"
            "inline ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_NR ISA_BINUTILS_INFO_ext_rel_enc_nr (ISA_SUBSET subset) {\n"
	    "  return ISA_BINUTILS_info[subset].ext_rel_enc_nr;\n"
	    "}\n\n"
            "inline mUINT32 ISA_BINUTILS_INFO_emul_byte_nr (ISA_SUBSET subset) {\n"
	    "  return ISA_BINUTILS_info[subset].emul_byte_nr;\n"
	    "}\n\n"
            "inline mUINT32 ISA_BINUTILS_INFO_DWARF_reg_nr (ISA_SUBSET subset) {\n"
	    "  return ISA_BINUTILS_info[subset].DWARF_reg_nr;\n"
	    "}\n\n"
            "inline mUINT32 ISA_BINUTILS_INFO_xversion (ISA_SUBSET subset) {\n"
	    "  return ISA_BINUTILS_info[subset].xversion;\n"
	    "}\n\n"
	   );
  } else {
  }

  fprintf(cfile,
          "static mUINT32 No_Extension_Relocate_Encoding ( ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO ** Encoding, mUINT32 Size ) {\n"
          "  return 0;\n"
	  "}\n\n"
          "static mUINT32 No_Extension_Relocate_Encoding_Nr ( void ) {\n"
          "  return 0;\n"
	  "}\n\n");

  nb_subsets = ISA_SUBSET_count - ISA_SUBSET_MIN;

  if(nb_subsets<=0) {
    fprintf(stderr,"### Error: unexpected case: no new subset defined.\n");
    exit(EXIT_FAILURE);
  }

  for (subset=ISA_SUBSET_MIN;subset<ISA_SUBSET_count;subset++) {
    if (binutils[subset]!=NULL) {
      int pindex;
      int encindex;
      
      fprintf(cfile,
             "static mUINT32 ISA_BINUTILS_port_%s[] = { ",
	     ISA_SUBSET_Name(subset));
      for (pindex=0;pindex<PORT_SIZE;pindex++) {
        fprintf(cfile,
	        "%d%s",
	        binutils[subset]->port[pindex],
		(pindex<PORT_SIZE-1)?", ":"");
      }
      fprintf(cfile,"};\n");
      if (binutils[subset]->ext_rel_enc) {
         int size;
	 
         size = extension_relocate_encoding[subset].size();
         fprintf(cfile,
                 "static ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO Extension_Relocate_Encoding_info_tab_%s [] = {\n",
   	         ISA_SUBSET_Name(subset));
	 for (encindex=0;encindex<size;encindex++) {
	    unsigned int nb_op;
	    
	    fprintf(cfile,"  { \"%s\", TOP_%s, ",
	            extension_relocate_encoding[subset][encindex]->symbol,
		    TOP_Name(extension_relocate_encoding[subset][encindex]->top));
            fprintf(cfile,"0x%llXULL, 0x%llXULL }%s\n",
	            extension_relocate_encoding[subset][encindex]->mask,
		    extension_relocate_encoding[subset][encindex]->encoding,
		    encindex!=size-1?",":"");
         }
         fprintf(cfile,"};\n\n");
	 
         fprintf(cfile,
                 "static mUINT32 Extension_Relocate_Encoding_%s ( ISA_BINUTILS_EXTENSION_RELOCATE_ENCODING_INFO ** Encoding, mUINT32 Size ) {\n",
   	         ISA_SUBSET_Name(subset));
         size = extension_relocate_encoding[subset].size();
         fprintf(cfile,
  	        "  mUINT32 l_size;\n\n"
		"  if (Size<%d) return 0;\n",
		size);
	 for (encindex=0;encindex<size;encindex++) {
            fprintf(cfile,
                   "  Encoding[%d] = &Extension_Relocate_Encoding_info_tab_%s[%d];\n",
	           encindex,ISA_SUBSET_Name(subset),encindex);
         }	 
         fprintf(cfile,
                 "  for (l_size=%d; l_size<Size; l_size++) {\n"
		 "    Encoding[l_size] = NULL;\n"
		 "  }\n",size);
         fprintf(cfile,
                 "  return 1;\n"
   	         "}\n\n");

         fprintf(cfile,
                 "static mUINT32 Extension_Relocate_Encoding_Nr_%s ( void ) {\n",
   	         ISA_SUBSET_Name(subset));
         fprintf(cfile,
	         "  return %d;\n"
		 "}\n\n",extension_relocate_encoding[subset].size());
      }
    }
  }
  
  if (gen_static_code) {
    fprintf(cfile,
           "static ISA_BINUTILS_INFO ISA_BINUTILS_static_info[] = {\n");
  } else {
    fprintf(cfile,
           "static ISA_BINUTILS_INFO ISA_BINUTILS_dynamic_info[] = {\n");
  }
  for (subset=ISA_SUBSET_MIN;subset<ISA_SUBSET_count;subset++) {
    if (binutils[subset]!=NULL) {
      fprintf(cfile,
              "  { \"%s\", \"%s\", \"%s\", 0x%8.8x, %d, ISA_BINUTILS_port_%s, %d, ",
	      extname!=NULL?extname:binutils[subset]->short_abstract,
	      binutils[subset]->abstract,
	      binutils[subset]->short_abstract,
	      binutils[subset]->magic_key,
	      PORT_SIZE,
	      ISA_SUBSET_Name(subset),
	      binutils[subset]->last_scratch);
      if (binutils[subset]->ext_rel_enc) {
         fprintf(cfile,
	         "Extension_Relocate_Encoding_%s, Extension_Relocate_Encoding_Nr_%s, ",
		 ISA_SUBSET_Name(subset),ISA_SUBSET_Name(subset));
      } else {
         fprintf(cfile,
	         "No_Extension_Relocate_Encoding, No_Extension_Relocate_Encoding_Nr, ");
      }
      fprintf(cfile,
             "%d, %d, 0x%8.8x},\n",
	     binutils[subset]->emul_byte_nr,
	     binutils[subset]->DWARF_reg_nr,
	     binutils[subset]->xversion);
    }
  }
  fprintf(cfile,
         "};\n\n");

  if (gen_static_code) {
    fprintf(cfile,
           "ISA_BINUTILS_INFO* ISA_BINUTILS_info = ISA_BINUTILS_static_info;\n\n");
  } else {
    fprintf(hfile,
           "extern ISA_BINUTILS_INFO* dyn_get_ISA_BINUTILS_info_tab ( void );\n\n");

    fprintf(hfile,
           "extern mUINT32 dyn_get_ISA_BINUTILS_info_tab_sz ( void );\n\n");

    fprintf(cfile,
           "ISA_BINUTILS_INFO* dyn_get_ISA_BINUTILS_info_tab ( void ) {\n"
	   "  return ISA_BINUTILS_dynamic_info;\n"
	   "}\n\n");

    fprintf(cfile,
           "mUINT32 dyn_get_ISA_BINUTILS_info_tab_sz ( void ) {\n"
	   "  return %d;\n"
	   "}\n\n",ISA_SUBSET_count-ISA_SUBSET_MIN);

    fprintf(sfile,
           "ISA_BINUTILS_INFO* ISA_BINUTILS_info = NULL;\n\n");

    fprintf(sfile,
	    "/*\n"
	    " * Exported routine.\n"
	    " */\n"
	    "void \n"
	    "ISA_BINUTILS_Initialize_Stub( void )\n"
	    "{\n"
	    "  ISA_BINUTILS_info = (ISA_BINUTILS_INFO*)dyn_get_ISA_BINUTILS_info_tab();\n"
	    "  return;\n"
	    "}\n");
  }

  fprintf(hfile, "#ifdef DYNAMIC_CODE_GEN\n");
  fprintf(hfile, "extern       void                   ISA_BINUTILS_Initialize_Stub(void);\n");
  fprintf(hfile, "#endif\n");

  Emit_Footer (hfile);
  Emit_C_Footer(cfile);
  if (sfile) Emit_C_Footer(sfile);

  /* Closing file handlers */
  Gen_Close_File_Handle(hfile,hfilename);
  Gen_Close_File_Handle(cfile,cfilename);
  if(efile)
    Gen_Close_File_Handle(efile,efilename);
  if(sfile)    
    Gen_Close_File_Handle(sfile,sfilename);

  Gen_Free_Filename(cfilename);
  Gen_Free_Filename(hfilename);
  if(efilename) Gen_Free_Filename(efilename);
  if(sfilename) Gen_Free_Filename(sfilename);
}
