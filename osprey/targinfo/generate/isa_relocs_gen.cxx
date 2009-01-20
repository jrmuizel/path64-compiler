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
// isa_relocs_gen.cxx
/////////////////////////////////////
//
//  Generate relocation information.
//
/////////////////////////////////////
//

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#if __GNUC__ >= 3 || defined(_MSC_VER)
#include <list>
#include <vector>
using std::vector;
using std::list;
#else
#include <list.h>
#include <vector.h>
#endif //  __GNUC__ >= 3 || defined(_MSC_VER)

#include "W_limits.h"
#include "gen_util.h"
#ifdef DYNAMIC_CODE_GEN
#include "dyn_isa_subset.h"
#else
#include "targ_isa_subset.h"
#endif
#include "isa_relocs_gen.h"

// Forward declarations.
static const char* overflow_name(ISA_RELOC_OVERFLOW_TYPE overflow);
static const char* underflow_name(ISA_RELOC_UNDERFLOW_TYPE underflow);

// Declaration of structures.
struct reloc_type {
  const char *name;
  int main_symbol;
  int second_symbol;
  int addend;
  int pcrel;
  int gprel;
  int gotrel;
};

struct bitfield {
  const char *name;
  int start_bit;
  int stop_bit;
};

struct isa_virtual_reloc_type {
  int                      internal_id;  /* Internal identifier.*/
  int                      virtual_id;   /* Virtual  identifier.*/

  const char              *name;
  const char              *syntax;
  ISA_RELOC_OVERFLOW_TYPE  overflow;
  ISA_RELOC_UNDERFLOW_TYPE underflow;
  int                      right_shift;
  RELOC_TYPE               rel_type;

  int                      nb_bitfields;
  int                      elf_id[MAX_BITFIELDS];
  BITFIELD                 bitfields[MAX_BITFIELDS];
};

typedef vector<bool> Reloc_Subset;

// A dynamic bidimensional table of booleans.
// First index of the table is the subset identifier.
// Second index is the relocation internal identifier.
// An element of the table is true if relocation
// belongs to the considered subset.
// By default, all subsets support the UNDEFINED
// relocation whose index is 0 by construction.
static vector<Reloc_Subset> subset_relocations;

// A table of all relocations (including the
// UNDEFINED relocation). By construction, the
// following relation holds:
// index == internal_id
static vector<ISA_VIRTUAL_RELOC_TYPE> relocations;

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   A list of all the relocations used in an ISA.",
  " *",
  " *   The following interface is strongly related to the",
  " *   ELF standard, but could be extended to other file formats.",
  " *",
  " *   In this interface, we make a clear distinction between:",
  " *      - virtual relocations,",
  " *      - and physical relocations.",
  " *",
  " *   A virtual relocation consists of a set of physical",
  " *   relocations (also called sub relocations hereafter).",
  " *   It is characterized by its unique internal identifier.",
  " *",
  " *   The virtual identifier of a relocation is to be unique",
  " *   within a subset. But two relocations can share the same",
  " *   virtual identifier as long as they don't belong to the",
  " *   same subset. In other words, compatibility between two",
  " *   subsets can be broken down if they both have relocations",
  " *   with the same virtual identifier.",
  " *   Now, let us adopt a more dynamic point of view. No two",
  " *   uncompatible subsets are allowed to be activated at the",
  " *   same time. As a result, there must be a one-to-one",
  " *   relation between the set of the virtual identifiers",
  " *   and the set of the internal identifiers.",
  " *",
  " *   Each physical relocation is characterized by its ELF",
  " *   identifier and its bitfield.",
  " *",
  " *   As can be seen, the difference between a physical and a",
  " *   virtual relocation is only relevant for processors with splitted",
  " *   bitfields. Note that defining an ELF identifier for a sub",
  " *   relocation is not completely correct, we should better consider",
  " *   this identifier as an internal identifier of a physical relocation.",
  " *",
  " *   The big trick in what follows, is that physical relocations",
  " *   are almost nether considered (from the user's point of view).",
  " *   More precisely, they are only used at the very end of a link",
  " *   process. The rest of the time, we deal with virtual relocations.",
  " *   In particular, relocations identifiers in object files after",
  " *   assembly step are virtual ones.",
  " *",
  " *   This package exports the following data and API:",
  " *",
  " *   typedef (enum) ISA_RELOCS",
  " *       An enumeration of all relocation internal identifiers.",
  " *",
  " *   typedef (struct) ISA_RELOCS_INFO",
  " *       Identify a relocation entry.",
  " *       virtual_id      virtual identifier.",
  " *       bitfield:       gives a list of sub relocations and their",
  " *                       relocation ELF ids.",
  " *       bitfields:      number of sub relocations.",
  " *       main_symbol:    relocation is applicable on a symbol.",
  " *                       Note: some relocations can be based on GOT ",
  " *                       address or GOT offset ('G' in ELF spec.),",
  " *                       but targinfo currently support only relocation on",
  " *                       symbols.",
  " *       second_symbol:  relative to a second symbol. In this case the",
  " *                       second symbol id can be stored in immediate field.",
  " *       addend:         relocation accepts an addend value.",
  " *       pcrel:          relocation is PC relative.",
  " *       gprel:          relocation is GP (global pointer) relative.",
  " *       gotrel:         relocation is GOT (Global Offset Table) relative.",
  " *       overflow:       must check if there is an overflow regarding",
  " *                       the given bitfields.",
  " *       right_shift:    right shift operation on relocated value.",
  " *       underflow:      depending on the right shift amount, must check",
  " *                       if the lowest bits are zero.",
  " *       syntax:         relocation syntax in case of syntaxically",
  " *                       explicit relocation.",
  " *       name:           internal relocation name.",
  " *",
  " *",
  " *   const char * ISA_RELOC_Name (ISA_RELOC reloc)",
  " *       Returns name of <reloc>.",
  " *",
  " *   const char * ISA_RELOC_Syntax (ISA_RELOC reloc)",
  " *       Returns syntax of <reloc>.",
  " *",
  " *   BOOL ISA_RELOC_Is_Implicit (ISA_RELOC reloc)",
  " *       Returns if <reloc> is syntaxically implicit.",
  " *",
  " *   UINT8 ISA_RELOC_Bit_Field_Stop_Bit (ISA_RELOC reloc, INT bf)",
  " *       Returns bit field <bf> stop bit of <reloc>.",
  " *",
  " *   UINT8 ISA_RELOC_Bit_Field_Start_Bit (ISA_RELOC reloc, INT bf)",
  " *       Returns bit field <bf> start bit of <reloc>.",
  " *",
  " *   UINT8 ISA_RELOC_Bit_Field_Elf_ID (ISA_RELOC reloc, INT bf)",
  " *       Returns bit field <bf> ELF relocation ID of <reloc>.",
  " *",
  " *   UINT8 ISA_RELOC_Bit_Fields (ISA_RELOC reloc)",
  " *       Returns the number of bit field associated to <reloc>.",
  " *",
  " *   BOOL ISA_RELOC_On_Symbol (ISA_RELOC reloc)",
  " *       Returns true if <reloc> is applied on a symbol.",
  " *",
  " *   BOOL ISA_RELOC_On_Second_Symbol (ISA_RELOC reloc)",
  " *       Returns true if <reloc> is applied on a second symbol.",
  " *",
  " *   BOOL ISA_RELOC_Has_Addend (ISA_RELOC reloc)",
  " *       Returns true if <reloc> has an addend.",
  " *",
  " *   BOOL ISA_RELOC_Is_PC_Rel (ISA_RELOC reloc)",
  " *       Returns true if <reloc> is PC relative.",
  " *",
  " *   BOOL ISA_RELOC_Is_GP_Rel (ISA_RELOC reloc)",
  " *       Returns true if <reloc> is GP relative.",
  " *",
  " *   BOOL ISA_RELOC_Is_GOT_Rel (ISA_RELOC reloc)",
  " *       Returns true if <reloc> is GOT relative.",
  " *",
  " *   ISA_RELOC_OVERFLOW_TYPE ISA_RELOC_Check_Overflow (ISA_RELOC reloc)",
  " *       Returns the overflow check type for <reloc>.",
  " *",
  " *   INT32 ISA_RELOC_Right_Shift (ISA_RELOC reloc)",
  " *       Returns the right shift amount to apply on <reloc>.",
  " *",
  " *   ISA_RELOC_UNDERFLOW_TYPE ISA_RELOC_Check_Underflow (ISA_RELOC reloc)",
  " *       Returns the underflow check type for <reloc>.",
  " *",
  " *   ISA_LIT_CLASS ISA_RELOC_Lit_Class (ISA_RELOC reloc)",
  " *       Returns the literal class associated to <reloc>.",
  " *",
  " * ====================================================================",
  " *",
  " *   Relocations and subsets.",
  " *",
  " *   void ISA_RELOC_Initialize(void)",
  " *       Initialize the relocations package for use with subsets and",
  " *       ISA specified by the subset list in ISA_SUBSET_List.",
  " *       Compatibility of subsets is to be checked before calling",
  " *       this routine, since no check is performed here.",
  " *",
  " *   const INT ISA_RELOC_Member_Subset(ISA_SUBSET subset, ISA_RELOC reloc)",
  " *       Returns true(1) iff reloc is supported in the considered,",
  " *       subset.",
  " *",
  " *   ISA_RELOC ISA_RELOC_Id(ISA_VIRTUAL_RELOC virtual_reloc)",
  " *        Returns the (internal) relocation identifier of a virtual",
  " *        relocation or 0 (index of the UNDEFINED relocation) if an",
  " *        error has occured.",
  " *        Result depends on the activated subset list.",
  " *",
  " *   ISA_VIRTUAL_RELOC ISA_RELOC_Virtual_Id(ISA_RELOC reloc)",
  " *        Returns the virtual identifier of a given relocation",
  " *        or 0 (index of the virtual UNDEFINED relocation) if",
  " *        an error has occured.",
  " *", 
  " *   UINT32 ISA_RELOC_Number_Relocs(ISA_SUBSET subset)",
  " *       Returns the number of relocations supported by the",
  " *       considered subset.",
  " *",
  " *   ISA_VIRTUAL_RELOC ISA_RELOC_Max_Virtual_Id(ISA_SUBSET subset)",
  " *       Returns the highest virtual id used by the considered subset.",
  " *",
  " *   void ISA_RELOC_Set_Max_Virtual_Id(ISA_SUBSET subset,ISA_VIRTUAL_RELOC id)",
  " *       Sets the highest virtual id used by the considered subset.",
  " *",
  " * ====================================================================",
  " */",
  NULL,
};

static FILE *hfile = NULL;
static FILE *cfile = NULL;
static FILE *sfile = NULL;
static FILE *efile = NULL;

static int max_bitfields_static = 0;

static char *hfilename     = NULL;
static char *cfilename     = NULL;
static char *sfilename     = NULL;
static char *efilename     = NULL;

// Whether we generate code for the core or for
// an extension.
static bool gen_static_code = false;

// For extension, identify core subset the extension
// is leaving with.
static const char * CoreSubset = "stxp70_v3";

// Currently we don't accept more than 256 class
// of literals. In fact, this limit is too high
// since we can't have more than 65 unsigned
// ranges and 65 signed ranges (sub-ranges are
// not managed yet).
static       int relocs_count = 0;
static const int relocs_max_limit = 255;

// (Virtual and internal) id. of UNDEFINED and empty
// relocations.
static const int undefined_reloc_id = 0;

// C file generated when static code generation is done,
// included in C ISA_LIT_info table when dynamic code
// generation is done.
static FILE *cincfile = NULL;
static char *cincfilename  = NULL;

ISA_VIRTUAL_RELOC_TYPE undefined_reloc;

//////////////////////////////////////////////////////////////////////////
// Beginning of code.
//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////
static void init_structure( void )
///////////////////////////////////////////////
// Internal routine called by ISA_Relocs_begin.
// at its very beginning.
///////////////////////////////////////////////
{
  // Enter UNDEFINED relocation in the relocation table.
  undefined_reloc = new isa_virtual_reloc_type; 
  undefined_reloc->virtual_id   = undefined_reloc_id;
  undefined_reloc->internal_id  = relocs_count++;
  undefined_reloc->name         ="UNDEFINED";
  undefined_reloc->syntax       = "";
  undefined_reloc->overflow     = ISA_RELOC_NO_OVERFLOW;
  undefined_reloc->underflow    = ISA_RELOC_NO_UNDERFLOW;
  undefined_reloc->rel_type     = (RELOC_TYPE)0;
  undefined_reloc->nb_bitfields = 0;

  relocations.push_back(undefined_reloc);

  // Set correct size for subset_relocations vector.
  subset_relocations.resize(ISA_SUBSET_count);
  return;
}

/////////////////////////////////////
void ISA_Relocs_Begin (void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  gen_static_code = Is_Static_Code();

  const char* const extname  = gen_static_code ? NULL: Get_Extension_Name();
  char *file_name_isa_lits;
  char *file_name_isa_subsets;

  init_structure();

  // Opening files
  // Header file
  hfilename = Gen_Build_Filename(FNAME_TARG_ISA_RELOCS,
                                 extname,
                                 gen_util_file_type_hfile);
  hfile     = Gen_Open_File_Handle(hfilename,"w");

  // C file
  cfilename = Gen_Build_Filename(FNAME_TARG_ISA_RELOCS,
                                 extname,
                                 gen_util_file_type_cfile);
  cfile     = Gen_Open_File_Handle(cfilename,"w");

  // Export file
  if(gen_static_code) {
    efilename = Gen_Build_Filename(FNAME_TARG_ISA_RELOCS,
				   extname,
				   gen_util_file_type_efile);
    efile     = Gen_Open_File_Handle(efilename,"w");
  }
  else {
    const char *headers[] = {
      "<stddef.h>",
      "\"dyn_" FNAME_ISA_RELOCS ".h\"",
      "",
    };

    sfilename = Gen_Build_Filename(FNAME_STUB_ISA_RELOCS,
				   extname,
				   gen_util_file_type_cfile);
    sfile     = Gen_Open_File_Handle(sfilename,"w");
    Emit_Stub_Header(sfile,headers);
  }

  // C file generated when static code generation is done,
  // included in C ISA_LIT_info table when dynamic code
  // generation is done.
  cincfilename = Gen_Build_Filename(FNAME_TARG_ISA_RELOCS,
                                    NULL, /* Build at static code gen. time*/
                                    gen_util_file_type_c_i_file);
  if(gen_static_code) {
    cincfile  = Gen_Open_File_Handle(cincfilename,"w");

    fprintf(cincfile,
	    "\n\n"
	    "/* This file has been create automatically\n"
	    " *  Do not modifiy it.\n"
	    " */\n\n"
	    "/* Including the static part of literal table.\n"
	    " */\n\n");
   }

  Emit_C_Header(cfile);   /* "C" block.                      */

  fprintf(cfile,"#include <stdio.h>   /* NULL pointer */\n");
  if (gen_static_code) {
     fprintf(cfile,"#include \"%s\"\n\n",hfilename);
  } else {
     fprintf(cfile,"#include \"dyn_isa_relocs.h\"\n");
     fprintf(cfile,"#include \"dyn_isa_subset.h\"\n\n");
  }
  
  Emit_Header (hfile, FNAME_TARG_ISA_RELOCS, interface,extname);

  file_name_isa_lits    = Gen_Build_Filename(FNAME_TARG_ISA_LITS,
                             NULL, gen_util_file_type_hfile);
  file_name_isa_subsets = Gen_Build_Filename(FNAME_TARG_ISA_SUBSET,
                             NULL, gen_util_file_type_hfile);
  fprintf(hfile,
          "#include \"%s\"\n"
          "#include \"%s\"\n",
          file_name_isa_lits,
          file_name_isa_subsets);

  if(gen_static_code) {
    // UNDEFINED relocation.
    fprintf(
     hfile,
     "#define ISA_RELOC_%s\t%d\n",
     relocations[undefined_reloc_id]->name,
     relocations[undefined_reloc_id]->internal_id); 

    fprintf(
     hfile,
     "#define TN_is_reloc_none(r)\t\t(TN_relocs(r) == ISA_RELOC_%s)\n",
     relocations[undefined_reloc_id]->name); 

    fprintf(
     hfile,
     "#define Set_TN_is_reloc_none(r)\t\tSet_TN_relocs(r,ISA_RELOC_%s)\n"
     "\n",
     relocations[undefined_reloc_id]->name);
  }

  if(gen_static_code) {
    fprintf(cfile, 
	    "static ISA_RELOC_INFO ISA_RELOC_static_info[] = {\n");
  }
  else {
    fprintf(cfile, 
	    "static ISA_RELOC_INFO ISA_RELOC_dynamic_info [] = {\n");
  }

  // UNDEFINED entry is reserved to static table
  if(gen_static_code) {
    fprintf(
      cfile, 
      "  { { {   0,  0,  0, }, }, 0, 0, 0, 0, 0, 0, 0, %s, %s, 0, \"\",  \"RELOC_%s\", %d },\n",
      overflow_name (relocations[undefined_reloc_id]->overflow),
      underflow_name(relocations[undefined_reloc_id]->underflow),
      relocations[undefined_reloc_id]->name,
      relocations[undefined_reloc_id]->virtual_id);

    fprintf(
      cincfile, 
      "  { { {   0,  0,  0, }, }, 0, 0, 0, 0, 0, 0, 0, %s, %s, 0, \"\",  \"RELOC_%s\" , %d },\n",
      overflow_name (relocations[undefined_reloc_id]->overflow),
      underflow_name(relocations[undefined_reloc_id]->underflow),
      relocations[undefined_reloc_id]->name,
      relocations[undefined_reloc_id]->virtual_id);
  } else {
    fprintf(
      cfile, 
      "  { { {   0,  0,  0, }, }, 0, 0, 0, 0, 0, 0, 0, %s, %s, 0, \"\",  \"RELOC_%s\", %d },\n",
      overflow_name (relocations[undefined_reloc_id]->overflow),
      underflow_name(relocations[undefined_reloc_id]->underflow),
      relocations[undefined_reloc_id]->name,
      relocations[undefined_reloc_id]->virtual_id);
  }

  Gen_Free_Filename(file_name_isa_lits);
  Gen_Free_Filename(file_name_isa_subsets);
  return;
}


/////////////////////////////////////
BITFIELD ISA_Create_BitField(const char *name, int start_bit, int bit_size)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  BITFIELD bf = new bitfield;
  bf->name = name;
  bf->start_bit = start_bit;
  bf->stop_bit = start_bit + bit_size - 1;
  return bf;
}


/////////////////////////////////////
RELOC_TYPE ISA_Create_Reloc_Type(const char *name,
				 RELOC_MAIN_SYMBOL main_symbol,
				 RELOC_SECOND_SYMBOL second_symbol,
				 RELOC_ADDEND addend,
				 RELOC_PCREL pcrel,
				 RELOC_GPREL gprel,
				 RELOC_GOTREL gotrel )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  RELOC_TYPE rel_type = new reloc_type;
  rel_type->name = name;
  rel_type->main_symbol = main_symbol == R_MAIN_SYMBOL ? 1 : 0; 
  rel_type->second_symbol = second_symbol == R_SECOND_SYMBOL ? 1 : 0; 
  rel_type->addend = addend == R_ADDEND ? 1 : 0;
  rel_type->pcrel = pcrel == R_PCREL ? 1 : 0;
  rel_type->gprel = gprel == R_GPREL ? 1 : 0;
  rel_type->gotrel = gotrel == R_GOTREL ? 1 : 0;
  
  return rel_type;
}

///////////////////////////////////////
// Helper subroutine.
//////////////////////////////////////
static const char*
overflow_name(ISA_RELOC_OVERFLOW_TYPE overflow) {
  switch(overflow) {
  case ISA_RELOC_NO_OVERFLOW      : return "ISA_RELOC_NO_OVERFLOW"; break;
  case ISA_RELOC_OVERFLOW_BITFIELD: return "ISA_RELOC_OVERFLOW_BITFIELD"; break;
  case ISA_RELOC_OVERFLOW_SIGNED  : return "ISA_RELOC_OVERFLOW_SIGNED"; break;
  case ISA_RELOC_OVERFLOW_UNSIGNED: return "ISA_RELOC_OVERFLOW_UNSIGNED"; break;
  default                         : return "ISA_RELOC_OVERFLOW_UNKNOWN"; break;
  }
  return "ISA_RELOC_OVERFLOW_UNKNOWN";
}

///////////////////////////////////////
// Helper subroutine.
//////////////////////////////////////
static const char*
underflow_name(ISA_RELOC_UNDERFLOW_TYPE underflow) {
  switch(underflow) {
  case ISA_RELOC_NO_UNDERFLOW: return "ISA_RELOC_NO_UNDERFLOW"; break;
  case ISA_RELOC_UNDERFLOW   : return "ISA_RELOC_UNDERFLOW"; break;
  default                    : return "ISA_RELOC_UNDEFLOW_UNKNOWN"; break;
  }
  return "ISA_RELOC_UNDEFLOW_UNKNOWN";
}


/////////////////////////////////////
ISA_VIRTUAL_RELOC_TYPE ISA_Create_Reloc (int virtual_id,
                                         const char *name,
                                         const char *syntax,
                                         ISA_RELOC_OVERFLOW_TYPE overflow,
                                         ISA_RELOC_UNDERFLOW_TYPE underflow,
                                         int right_shift,
                                         RELOC_TYPE rel_type,
                                         ...)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  va_list ap;
  int elf_id;
  BITFIELD bf;
  const char *string_template;
  int cur_index;

  ///////////////////////////////////////////////////
  // A few checks before beginning the real work.
  ///////////////////////////////////////////////////
  if(NULL==name) {
    fprintf(stderr,"### Error: can't have a relocation with a NULL name.\n");
    exit(EXIT_FAILURE);
  }
  if(relocs_count==relocs_max_limit) {
     fprintf(
      stderr, 
      "### Error: number of relocations is greater than allowed limit (%d)\n",
      relocs_max_limit);
      exit(EXIT_FAILURE);
   }

  if(0==virtual_id) {
    fprintf(
      stderr,
      "### Error: virtual identifier 0 (\"%s\") is reserved\n"
      "### for UNDEFINED relocation.\n",
      relocations[undefined_reloc_id]->name);
    exit(EXIT_FAILURE);
  } else if (virtual_id<0) {
     fprintf(
      stderr,
      "### Error: can't have a negative virtual identifier for reloc. \"%s\"\n",
      name);
     exit(EXIT_FAILURE);
  }
    
  // An implicit relocation can be described either
  // with an empty string or with a NULL string.
  // Unify both cases.
  if(syntax==NULL)
    syntax="";

  // Allocate and fill in isa_virtual_reloc_type structure.
  ISA_VIRTUAL_RELOC_TYPE ret = new isa_virtual_reloc_type;

  ret->virtual_id   = virtual_id;
  ret->internal_id  = relocs_count++;
  ret->name         = name;
  ret->syntax       = syntax;
  ret->overflow     = overflow;
  ret->underflow    = underflow;
  ret->right_shift  = right_shift;
  ret->rel_type     = rel_type;
  ret->nb_bitfields = 0;

  // Find the smallest min and largest max for all ranges, and
  // count the number of ranges.
  va_start(ap,rel_type);
  while ((elf_id = va_arg(ap,int)) != BITFIELD_END && 
         (bf = va_arg(ap,BITFIELD))) {
            ++(ret->nb_bitfields);
  }
  va_end(ap);

  if (ret->nb_bitfields > max_bitfields_static) {
     if(ret->nb_bitfields > MAX_BITFIELDS) {
        fprintf(
          stderr,
          "### Error: number of bitfields (%d) for relocation %s\n"
          "is greater than the maximum number of bitfields allowed (%d)\n",
           max_bitfields_static,
           ret->syntax,
           MAX_BITFIELDS);
        exit(EXIT_FAILURE);
     }

     max_bitfields_static = ret->nb_bitfields; // Update.
  }

  if(gen_static_code) {
    fprintf(hfile,
            "#define ISA_RELOC_%s\t\t%d\n",
            ret->name,ret->internal_id);
    fprintf(hfile,
            "#define TN_is_reloc_%s(r)\t\t(TN_relocs(r) == ISA_RELOC_%s)\n",
            ret->name,ret->name); 
    fprintf(hfile,
            "#define Set_TN_is_reloc_%s(r)\t\tSet_TN_relocs(r,ISA_RELOC_%s)\n"
            "\n",
            ret->name,ret->name);
  }
  else {
    const char* const extname  = Get_Extension_Name();
    fprintf(hfile,
            "#define ISA_RELOC_dyn_%s_%-17s (dynamic_reloc_offset_%s + %d)\n",
            extname,ret->name,extname,ret->internal_id);
    fprintf(hfile,
            "#define TN_is_reloc_dyn_%s_%s(r)      (TN_relocs(r) == (ISA_RELOC_dyn_%s_%s))\n",
            extname,ret->name,extname,ret->name); 
    fprintf(hfile,
            "#define Set_TN_is_reloc_dyn_%s_%s(r)  Set_TN_relocs(r,(ISA_RELOC_dyn_%s_%s))\n"
            "\n",
            extname,ret->name,extname,ret->name); 
  }


  cur_index = 0;
  va_start(ap,rel_type);
  fprintf(cfile, "  { {");
  if(gen_static_code) {
    fprintf(cincfile, "  { {");
  }
  while ((elf_id = va_arg(ap,int)) != BITFIELD_END && 
         (bf = va_arg(ap,BITFIELD))) {

    ret->elf_id   [cur_index] = elf_id;
    ret->bitfields[cur_index] = bf;

    fprintf(cfile, " { %3d, %2d, %2d, },", 
	    ret->elf_id   [cur_index], 
            ret->bitfields[cur_index]->start_bit, 
            ret->bitfields[cur_index]->stop_bit);

    if(gen_static_code) {
      fprintf(cincfile, " { %3d, %2d, %2d, },", 
	      ret->elf_id   [cur_index], 
              ret->bitfields[cur_index]->start_bit, 
              ret->bitfields[cur_index]->stop_bit);
    }

    cur_index++;
  }
  va_end(ap);

  string_template = gen_static_code ?
" }, %d, %d, %d, %d, %d, %d, %d, %s, %s, %d, \"%s\", \"RELOC_%s\", %d },\n":
" }, %d, %d, %d, %d, %d, %d, %d, %s, %s, %d, \"%s\", \"RELOC_dyn_%s_%s\", %d },\n";

  if(gen_static_code) {
    fprintf(cfile,string_template,
            ret->nb_bitfields,
	    ret->rel_type->main_symbol, 
            ret->rel_type->second_symbol, 
            ret->rel_type->addend, 
            ret->rel_type->pcrel,
            ret->rel_type->gprel, 
            ret->rel_type->gotrel,
	    overflow_name(ret->overflow),
            underflow_name(ret->underflow),
            ret->right_shift,
            ret->syntax,
            ret->name,
            ret->virtual_id);

    fprintf(cincfile,string_template,
            ret->nb_bitfields,
	    ret->rel_type->main_symbol, 
            ret->rel_type->second_symbol,
            ret->rel_type->addend,
            ret->rel_type->pcrel,
            ret->rel_type->gprel,
            ret->rel_type->gotrel,
	    overflow_name(ret->overflow),
            underflow_name(ret->underflow),
            ret->right_shift,
            ret->syntax,
            ret->name,
            ret->virtual_id);
  }
  else {
    const char* const extname  = Get_Extension_Name();
    fprintf(cfile,string_template,
            ret->nb_bitfields,
	    ret->rel_type->main_symbol, 
            ret->rel_type->second_symbol,
            ret->rel_type->addend,
            ret->rel_type->pcrel,
            ret->rel_type->gprel,
            ret->rel_type->gotrel,
	    overflow_name(ret->overflow),
            underflow_name(ret->underflow),
            ret->right_shift,
            ret->syntax,
            extname,
            ret->name,
            ret->virtual_id);
  }

  relocations.push_back(ret);
  return ret;
}


/////////////////////////////////////
void ISA_Reloc_Set_Core_Subset(const char * subset_name)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{ CoreSubset = subset_name;
}

/////////////////////////////////////
void ISA_Reloc_Subset(ISA_SUBSET subset,
                      ...)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{ ISA_VIRTUAL_RELOC_TYPE rel;
  int size;
  va_list ap;

  if(subset >= ISA_SUBSET_count)
   { fprintf(stderr,"### Error: uncorrect subset identifier %d\n",subset);
     exit(EXIT_FAILURE);
   }

  size = subset_relocations[subset].size();

  // Find out the greater virtual identifier

  if(0==size || size < relocs_count)
    subset_relocations[subset].resize(relocs_count,false);

  va_start(ap,subset);
  while((rel=va_arg(ap,ISA_VIRTUAL_RELOC_TYPE))!= NULL)
    subset_relocations[subset][rel->internal_id] = true;
  va_end(ap);

  return;
}

//////////////////////////////////////
static void ISA_Reloc_Check_Subset(ISA_SUBSET subset,
                                   int *virtual_id_max,
                                   int *nb_subset_relocs)
//////////////////////////////////////
// ISA_Relocs_Subset_End helper
// routine. Returns the greatest virtual
// identifier.
//////////////////////////////////////
{
  int j;
  bool *tab = NULL;
  int size;

 *virtual_id_max = undefined_reloc_id;
 *nb_subset_relocs=1;

  // First, get the greatest virtual identifier. We don't
  // require virtual identifiers to be contiguous.
  size = subset_relocations[subset].size();
  for(j=1;j<size;++j) {
    if(false==subset_relocations[subset][j])
      continue;
    if(*virtual_id_max<relocations[j]->virtual_id)
      *virtual_id_max = relocations[j]->virtual_id;
    (*nb_subset_relocs)++;
  }

  // Second, check that no two relocations have the same
  // virtual identifier.
  tab = new bool[*virtual_id_max+1];
  for(j=0;j<*virtual_id_max+1;++j)
    tab[j]=false;

  for(j=1;j<size;++j) {
    if(false==subset_relocations[subset][j])
      continue;

    if(tab[relocations[j]->virtual_id]==false) {
      tab[relocations[j]->virtual_id]=true;
    }
    else {
      fprintf(stderr,
      "### Error: can't have two relocations with the same virtual id. (%d)\n",
      relocations[j]->virtual_id);
      exit(EXIT_FAILURE);
    }
  }

  return;
}


//////////////////////////////////////
static void ISA_Relocs_Subsets_End(FILE *hfile, FILE *cfile)
//////////////////////////////////////
// Subroutine called by ISA_Relocs_End
//////////////////////////////////////
{
  int i;
  int j;
  int size;
  const char *name_format = gen_static_code ? "reloc_subset_tab":
                                              "dyn_reloc_subset_tab";
  const char *format_1 = "  %d, /* %-15s */\n";
  int  *virtual_id_max;
  int   virtual_id_max_all_subsets;
  int  *subsets_nb_relocs;
  int   nb_iterations;

  nb_iterations = ISA_SUBSET_count - ISA_SUBSET_MIN;

  if(nb_iterations<=0) {
    fprintf(stderr,"### Error: unexpected case: no new subset defined.\n");
    exit(EXIT_FAILURE);
  }

  subsets_nb_relocs   = new int[nb_iterations];
  virtual_id_max      = new int[nb_iterations];
  virtual_id_max_all_subsets = 0;

  for(i=ISA_SUBSET_MIN;i<ISA_SUBSET_count;i++) {

     ISA_Reloc_Check_Subset(
       i,
      &virtual_id_max   [i-ISA_SUBSET_MIN], 
      &subsets_nb_relocs[i-ISA_SUBSET_MIN]);

     if(virtual_id_max_all_subsets<virtual_id_max[i-ISA_SUBSET_MIN])
       virtual_id_max_all_subsets = virtual_id_max[i-ISA_SUBSET_MIN];

     // By construction, relocs_count can't be 0, since
     // we have at least the UNDEFINED relocation. Therefore
     // the following table can't be empty.
     fprintf(cfile,"static mUINT32 %s_%s_%d [] = {\n",
             name_format,
             ISA_SUBSET_Name(i),
             i);

     fprintf(cfile,format_1,undefined_reloc_id,relocations[undefined_reloc_id]->name);

     size = subset_relocations[i].size();

     for(j=1;j<relocs_count;j++) {
       if(j<size && true==subset_relocations[i][j]) 
         fprintf(cfile,format_1,j,relocations[j]->name); 
      }

     fprintf(cfile,
             "}; /* %s_%s_%d */\n\n",
             name_format,
             ISA_SUBSET_Name(i),
             i);
   }

   if(gen_static_code) {
      fprintf(hfile,
      "\n"
      "\n"
      "/* -------------- Subset information --------------- */\n"
      "\n"
      "\n"
      "/* The following structure contains:\n"
      " *  - nb_subset_relocs is the number of relocations supported in\n"
      " *    the subset (at least 1 since %s relocation is always in).\n"
      " *  - max_virtual_id is the greatest virtual relocation identifier\n"
      " *    of the subset (helps build dynamic mapping from virtual to\n"
      " *    internal identifier).\n"
      " *  - a table (tab_subset reloc), where each item is set to 1 if the\n"
      " *    relocation is supported in the subset. This table size is\n"
      " *    <nb_subset_relocs>.\n"
      " */\n"
      "typedef struct {\n"
      "  UINT32            nb_subset_relocs;\n"
      "  ISA_VIRTUAL_RELOC max_virtual_id;\n"
      "  UINT32           *tab_subset_reloc;\n"
      "} ISA_RELOC_SUBSET_INFO;\n"
      "\n"
      "\n",
      relocations[undefined_reloc_id]->name
      );
   }

   fprintf(
      cfile,
      "static ISA_RELOC_SUBSET_INFO %s [%d] = {\n",
      name_format,
      ISA_SUBSET_count - ISA_SUBSET_MIN);

   for(i=ISA_SUBSET_MIN;i<ISA_SUBSET_count;i++)
      fprintf(
        cfile,
        " { %3d, %3d, %s_%s_%d  },\n",
        subsets_nb_relocs[i],
        virtual_id_max[i],
        name_format,ISA_SUBSET_Name(i),i
        );

   fprintf(cfile,"};\n\n");
   
   if (gen_static_code) {
      fprintf(cfile,
              "BE_EXPORTED ISA_RELOC_SUBSET_INFO * ISA_%s = %s;\n\n",
              name_format,name_format);
      fprintf(hfile,
              "\n"
              "BE_EXPORTED extern ISA_RELOC_SUBSET_INFO * ISA_%s;\n\n",
              name_format);

      fprintf(hfile,
           "\n"
           "inline BOOL ISA_Reloc_Member_Subset(\n"
           "    ISA_SUBSET subset,\n"
           "    ISA_RELOC reloc)\n"
           "{\n"
           " UINT32 i;\n\n"
	   " for (i=0;i<ISA_%s[subset].nb_subset_relocs;i++) {\n"
	   "     if (ISA_%s[subset].tab_subset_reloc[i] == reloc) {\n"
	   "        return TRUE;\n"
	   "     }\n"
	   " }\n"
	   " return FALSE;\n"
           "}\n"
           "\n"
           "inline UINT32 ISA_RELOC_Number_Relocs(ISA_SUBSET subset)\n"
           "{\n"
           "  return ISA_%s[subset].nb_subset_relocs;\n" 
           "}\n"
           "\n"
           "inline ISA_VIRTUAL_RELOC ISA_RELOC_Max_Virtual_Id(ISA_SUBSET subset)\n"
           "{\n"
           "  return ISA_%s[subset].max_virtual_id;\n" 
           "}\n"
           "\n"
           "inline void ISA_RELOC_Set_Max_Virtual_Id(ISA_SUBSET subset, ISA_VIRTUAL_RELOC id)\n"
           "{\n"
           "  ISA_%s[subset].max_virtual_id = id;\n" 
           "}\n"
           "\n",
           name_format,name_format,name_format,name_format,name_format); 

      fprintf(hfile,
           "inline ISA_VIRTUAL_RELOC ISA_RELOC_Virtual_Id(ISA_RELOC reloc)\n"
           "{\n"
           "  return ISA_RELOC_info[reloc].virtual_id;\n"
           "}\n"
           "\n"
           "extern void ISA_RELOC_Initialize(void);\n"
           "\n");

      fprintf(cfile,
           "#define STATIC_SZ_TAB_VIRT %d\n"
           "static ISA_RELOC static_virtual_tab [STATIC_SZ_TAB_VIRT]={\n",
           virtual_id_max_all_subsets+1);

      for(i=0;i<virtual_id_max_all_subsets+1;++i)
          fprintf(cfile,"   ISA_RELOC_%s,\n",
                  relocations[undefined_reloc_id]->name);
      fprintf(
           cfile,
           "};\n"
           "\n"
           "BE_EXPORTED ISA_RELOC *reloc_virtual_tab   = static_virtual_tab;\n"
           "BE_EXPORTED mUINT32    sz_reloc_virtual_tab= STATIC_SZ_TAB_VIRT;\n" 
           "\n"
           "\n"
           );

      fprintf(hfile, 
           "BE_EXPORTED extern ISA_RELOC *reloc_virtual_tab   ;\n"
           "BE_EXPORTED extern mUINT32    sz_reloc_virtual_tab;\n" 
           "\n");

      fprintf(
           cfile,
           "void ISA_RELOC_Initialize(void)\n"
           "{\n"
           "  UINT32    index;\n"
           "  UINT32    mask = ISA_SUBSET_LIST_Mask(ISA_SUBSET_List);\n"
           "  INT       subset;\n"
           "  ISA_RELOC intern_reloc;\n"
           "  ISA_VIRTUAL_RELOC virt_reloc;\n"
           "\n"
           "  for (subset = ISA_SUBSET_MIN, index=1U<<ISA_SUBSET_MIN;\n"
           "       subset <= ISA_SUBSET_count;\n"
           "     ++subset, index <<=1U) {\n"
           "         if((mask&index)==0U) continue;\n"
           "\n"
           "         for(intern_reloc = 1;\n"
           "             intern_reloc<= ISA_RELOC_MAX;\n"
           "             intern_reloc++) {\n" 
           "               if(!ISA_Reloc_Member_Subset(subset,intern_reloc))\n"
           "                  continue;\n"
           "               virt_reloc = ISA_RELOC_Virtual_Id(intern_reloc);\n"
           "               reloc_virtual_tab[virt_reloc]=intern_reloc;\n"
           "               \n"
           "         }\n"
           "  }\n"
           "  return;\n"
           "}\n"
           "\n");

      fprintf(
          hfile,
          "inline ISA_RELOC ISA_RELOC_Id(ISA_VIRTUAL_RELOC virtual_reloc)\n"
          "{\n"
          "  if(virtual_reloc>sz_reloc_virtual_tab)\n"
          "     return ISA_RELOC_%s;\n"
          "  return reloc_virtual_tab[virtual_reloc];\n"
          "}\n"
          "\n",
          relocations[undefined_reloc_id]->name);
  } else {
      fprintf(cfile,
              "ISA_RELOC_SUBSET_INFO* dyn_get_ISA_RELOC_SUBSET_info_tab ( void )\n"
              "{ return %s;\n"
              "}\n\n",name_format);
      fprintf(cfile,
              "const mUINT32 dyn_get_ISA_RELOC_SUBSET_info_tab_sz ( void )\n"
              "{ return %d;\n"
              "}\n\n",ISA_SUBSET_count - ISA_SUBSET_MIN);
      fprintf(hfile,
              "extern       ISA_RELOC_SUBSET_INFO* dyn_get_ISA_RELOC_SUBSET_info_tab ( void );\n");
      fprintf(hfile,
              "extern const mUINT32                dyn_get_ISA_RELOC_SUBSET_info_tab_sz ( void );\n");
  }

  delete[] subsets_nb_relocs;
  delete[] virtual_id_max;
  return;
}                             // ISA_Relocs_Subsets_End


/////////////////////////////////////
void ISA_Relocs_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  /* Closing definition of relocations in C file */
  fprintf(cfile, "};\n\n");
  if(gen_static_code) {
    fprintf(cincfile,"\n\n");
  }

  if(gen_static_code) {
    fprintf(hfile,
	    "\n"
	    "#define %-20s %d\n" 
	    "#define %-20s %d\n"
            "#define %-20s %d\n\n"
	    "typedef mUINT32 ISA_RELOC;         /* used to be an enum */\n"
            "typedef mUINT32 ISA_VIRTUAL_RELOC; /* virtual identifier */\n\n",
	    "ISA_RELOC_STATIC_MAX",relocs_count-1,"ISA_RELOCS_MAX_LIMIT",relocs_max_limit,
	    "MAX_BITFIELDS_STATIC",max_bitfields_static);

    fprintf(hfile,
	    "typedef enum {\n"
	    "  ISA_RELOC_NO_OVERFLOW,\n"
	    "  ISA_RELOC_OVERFLOW_BITFIELD,\n"
	    "  ISA_RELOC_OVERFLOW_SIGNED,\n"
	    "  ISA_RELOC_OVERFLOW_UNSIGNED,\n"
	    "} ISA_RELOC_OVERFLOW_TYPE;\n\n");
    
    fprintf(hfile,
	    "typedef enum {\n"
	    "  ISA_RELOC_NO_UNDERFLOW,\n"
	    "  ISA_RELOC_UNDERFLOW,\n"
	    "} ISA_RELOC_UNDERFLOW_TYPE;\n\n");
    
    fprintf(hfile, 
	    "typedef struct {\n"
	    "  struct { UINT8 elf_id; UINT8 start_bit; UINT8 stop_bit; } bitfield[MAX_BITFIELDS_STATIC];\n"
	    "  mUINT8 bitfields;\n"
	    "  BOOL main_symbol;\n"
	    "  BOOL second_symbol;\n"
	    "  BOOL addend;\n"
	    "  BOOL pcrel;\n"
	    "  BOOL gprel;\n"
	    "  BOOL gotrel;\n"
	    "  ISA_RELOC_OVERFLOW_TYPE overflow;\n"
	    "  ISA_RELOC_UNDERFLOW_TYPE underflow;\n"
	    "  INT32 right_shift;\n"
	    "  const char *syntax;\n"
	    "  const char *name;\n"
            "  ISA_VIRTUAL_RELOC virtual_id;\n"
	    "} ISA_RELOC_INFO;\n\n");

    fprintf(hfile,
            "BE_EXPORTED extern mUINT32 ISA_RELOC_MAX;\n\n");

    fprintf(cfile,
	    "BE_EXPORTED mUINT32 ISA_RELOC_MAX = ISA_RELOC_STATIC_MAX;\n\n");

    fprintf(hfile,
            "BE_EXPORTED extern const ISA_RELOC_INFO * ISA_RELOC_info;\n\n");

    fprintf(cfile,
            "BE_EXPORTED const ISA_RELOC_INFO * ISA_RELOC_info = ISA_RELOC_static_info;\n\n");

    fprintf(efile, "ISA_RELOC_info\n");

    fprintf(efile, "ISA_reloc_subset_tab\n");

  } else {

#if 0
    fprintf(sfile,
	    "BE_EXPORTED mUINT32 dynamic_reloc_offset_%s = 0;\n\n",Get_Extension_Name());
#endif
	    
    fprintf(sfile,
	    "BE_EXPORTED mUINT32 ISA_RELOC_MAX = 0;\n\n");
	    
    fprintf(sfile,
	    "BE_EXPORTED const ISA_RELOC_INFO * ISA_RELOC_info = NULL;\n");

    fprintf(sfile,
	    "BE_EXPORTED ISA_RELOC_SUBSET_INFO * ISA_reloc_subset_tab = NULL;\n");

  }
      
  ISA_Relocs_Subsets_End(hfile,cfile);

  if(!gen_static_code) {
    const char *fct_name1= "dyn_get_ISA_RELOC_info_tab";
    const char *fct_name2= "dyn_get_ISA_RELOC_info_tab_sz";
    const char *fct_name3= "dyn_get_ISA_RELOC_max_static_virtual_id_core_subset";
    const char *fct_name4= "dyn_set_ISA_RELOC_dynamic_reloc_offset";
    const char *fct_name5= "dyn_get_ISA_RELOC_dynamic_reloc_offset";
    const char *fct_name6= "dyn_get_ISA_RELOC_subset_info_tab";
    const char *fct_name7= "dyn_get_ISA_RELOC_subset_info_tab_sz";
    
    fprintf(cfile,
            "\nmUINT32 dynamic_reloc_offset_%s = 0;\n",
	    Get_Extension_Name());

    fprintf(cfile,
	    "\n\n"
	    "ISA_RELOC_INFO* %s ( void )\n"
	    "{ return ISA_RELOC_dynamic_info;\n"
	    "};\n\n",
	    fct_name1);
    
    fprintf(cfile,
	    "const mUINT32 %s ( void )\n"
	    "{ return (const mUINT32) (%d);\n"
	    "}\n\n",
	    fct_name2,relocs_count);
    
    if (!strcmp(CoreSubset,"stxp70_v3")) {
       /* Specific relocs are numbered for x3 & fpx for V3 architecture */
       fprintf(cfile,
	       "const ISA_SUBSET %s ( void )\n"
	       "{ ISA_SUBSET subset;\n\n"
	       "  subset = ISA_SUBSET_%s;\n"
	       "  if (ISA_RELOC_Max_Virtual_Id(ISA_SUBSET_%s_ext_x3) > ISA_RELOC_Max_Virtual_Id(subset)) {\n"
	       "     subset = ISA_SUBSET_%s_ext_x3;\n"
	       "  }\n"
	       "  if (ISA_RELOC_Max_Virtual_Id(ISA_SUBSET_%s_ext_fpx) > ISA_RELOC_Max_Virtual_Id(subset)) {\n"
	       "     subset = ISA_SUBSET_%s_ext_fpx;\n"
	       "  }\n"
	       "  return subset;\n"
	       "}\n\n",
	       fct_name3,CoreSubset,CoreSubset,CoreSubset,CoreSubset,CoreSubset);
    } else if (!strcmp(CoreSubset,"stxp70_v4")) {
       /* Specific relocs are numbered for x3 for V4 architecture */
       fprintf(cfile,
	       "const ISA_SUBSET %s ( void )\n"
	       "{ ISA_SUBSET subset;\n\n"
	       "  subset = ISA_SUBSET_%s;\n"
	       "  if (ISA_RELOC_Max_Virtual_Id(ISA_SUBSET_%s_ext_x3) > ISA_RELOC_Max_Virtual_Id(subset)) {\n"
	       "     subset = ISA_SUBSET_%s_ext_x3;\n"
	       "  }\n"
	       "  return subset;\n"
	       "}\n\n",
	       fct_name3,CoreSubset,CoreSubset,CoreSubset);
    } else {
       fprintf(cfile,
	       "const ISA_SUBSET %s ( void )\n"
	       "{ return (const ISA_SUBSET) ISA_SUBSET_%s;\n"
	       "}\n\n",
	       fct_name3,CoreSubset);
    }
    
    fprintf(cfile,"void %s ( mUINT32 offset )\n",fct_name4);
    fprintf(cfile,"{ dynamic_reloc_offset_%s = offset;\n", Get_Extension_Name());
    fprintf(cfile,"}\n");

    fprintf(cfile,"mUINT32 %s ( void )\n", fct_name5);
    fprintf(cfile,"{ return dynamic_reloc_offset_%s;\n", Get_Extension_Name());
    fprintf(cfile,"}\n");

    fprintf(hfile,
	    "\n\n"
	    "extern       mUINT32                dynamic_reloc_offset_%s;\n\n"
	    "extern       ISA_RELOC_INFO*        %s ( void );\n"
	    "extern const mUINT32                %s ( void );\n"
	    "extern const ISA_SUBSET             %s ( void );\n"
	    "extern       void                   %s ( mUINT32 offset );\n"
	    "extern       mUINT32                %s ( void );\n"
	    "extern       ISA_RELOC_SUBSET_INFO* %s ( void );\n"
	    "extern       mUINT32                %s ( void );\n",
	    Get_Extension_Name(),fct_name1,fct_name2,fct_name3,fct_name4,fct_name5,fct_name6,fct_name7);

  }

  if(gen_static_code) {
    fprintf(hfile, "inline const char * ISA_RELOC_Name (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].name;\n"
	    "}\n\n");

    fprintf(hfile, "inline const char * ISA_RELOC_Syntax (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].syntax;\n"
	    "}\n\n");

    fprintf(hfile, "inline BOOL ISA_RELOC_Is_Implicit (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return *(ISA_RELOC_info[reloc].syntax) == 0;\n"
	    "}\n\n");

    fprintf(hfile, "inline UINT8 ISA_RELOC_Bit_Field_Stop_Bit (ISA_RELOC reloc, INT bf)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].bitfield[bf].stop_bit;\n"
	    "}\n\n");

    fprintf(hfile, "inline UINT8 ISA_RELOC_Bit_Field_Start_Bit (ISA_RELOC reloc, INT bf)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].bitfield[bf].start_bit;\n"
	    "}\n\n");

    fprintf(hfile, "inline UINT8 ISA_RELOC_Bit_Field_Elf_ID (ISA_RELOC reloc, INT bf)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].bitfield[bf].elf_id;\n"
	    "}\n\n");

    fprintf(hfile, "inline UINT8 ISA_RELOC_Bit_Fields (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return (UINT8)ISA_RELOC_info[reloc].bitfields;\n"
	    "}\n\n");

    fprintf(hfile, "inline BOOL ISA_RELOC_On_Symbol (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].main_symbol;\n"
	    "}\n\n");

    fprintf(hfile, "inline BOOL ISA_RELOC_On_Second_Symbol (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].second_symbol;\n"
	    "}\n\n");

    fprintf(hfile, "inline BOOL ISA_RELOC_Has_Addend (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].addend;\n"
	    "}\n\n");

    fprintf(hfile, "inline BOOL ISA_RELOC_Is_PC_Rel (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].pcrel;\n"
	    "}\n\n");

    fprintf(hfile, "inline BOOL ISA_RELOC_Is_GP_Rel (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].gprel;\n"
	    "}\n\n");

    fprintf(hfile, "inline BOOL ISA_RELOC_Is_GOT_Rel (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].gotrel;\n"
	    "}\n\n");

    fprintf(hfile, "inline ISA_RELOC_OVERFLOW_TYPE ISA_RELOC_Check_Overflow (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].overflow;\n"
	    "}\n\n");

    fprintf(hfile, "inline INT32 ISA_RELOC_Right_Shift (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].right_shift;\n"
	    "}\n\n");

    fprintf(hfile, "inline ISA_RELOC_UNDERFLOW_TYPE ISA_RELOC_Check_Underflow (ISA_RELOC reloc)\n"
	    "{\n"
	    "  return ISA_RELOC_info[reloc].underflow;\n"
	    "}\n\n");
  } else {

    fprintf(sfile,
	    "/*\n"
	    " * Exported routine.\n"
	    " */\n"
	    "void \n"
	    "ISA_RELOCS_Initialize_Stub( void )\n"
	    "{\n"
	    "  ISA_RELOC_info = (ISA_RELOC_INFO*)dyn_get_ISA_RELOC_info_tab();\n"
	    "  ISA_reloc_subset_tab = (ISA_RELOC_SUBSET_INFO*)dyn_get_ISA_RELOC_SUBSET_info_tab();\n"
	    "  ISA_RELOC_MAX = dyn_get_ISA_RELOC_info_tab_sz();\n"
	    "  dynamic_reloc_offset_%s = dyn_get_ISA_RELOC_dynamic_reloc_offset();\n"
	    "  return;\n"
	    "}\n",Get_Extension_Name());

  }
  
  fprintf(hfile, "#ifdef DYNAMIC_CODE_GEN\n");
  fprintf(hfile, "extern       void                   ISA_RELOCS_Initialize_Stub(void);\n");
  fprintf(hfile, "#endif\n");

  Emit_Footer  (hfile);
  Emit_C_Footer(cfile);
  
  // Closing file handlers
  Gen_Close_File_Handle(cfile,cfilename);
  Gen_Close_File_Handle(hfile,hfilename);
  if(efile)    Gen_Close_File_Handle(efile,efilename);
  if(sfile)    Gen_Close_File_Handle(sfile,sfilename);
  if(cincfile) Gen_Close_File_Handle(cincfile,cincfilename);
  
  // Memory deallocation
  if(cfilename)    Gen_Free_Filename(cfilename);
  if(hfilename)    Gen_Free_Filename(hfilename);
  if(efilename)    Gen_Free_Filename(efilename);
  if(sfilename)    Gen_Free_Filename(sfilename);
  if(cincfilename) Gen_Free_Filename(cincfilename);
  
  delete undefined_reloc;
  return;
}

