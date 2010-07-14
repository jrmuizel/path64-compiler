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


//  isa_subset_gen.cxx
/////////////////////////////////////
//
//  Generate an interface for a description of the ISA subset hierarchy.
//
/////////////////////////////////////
//
//  $Revision: 1.5 $
//  $Date: 04/12/21 14:57:26-08:00 $
//  $Author: bos@eng-25.internal.keyresearch.com $
//  $Source: /home/bos/bk/kpro64-pending/common/targ_info/generate/SCCS/s.isa_subset_gen.cxx $


#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <list>
#include <vector>
#include "topcode.h"
#include "gen_util.h"
#include "isa_subset_gen.h"


struct isa_subset {
  const char* name;         // Name given for documentation and debugging
  int index;                // value in enum
  ISA_SUBSET superset;      // Parent in subset tree, NULL for roots
#ifdef TARG_ST 
  std::vector<bool> members;     // true if opcode belongs to the subset.

  bool is_dyn_extensible;   // Whether a subset can be extended dynamically
                            // This property can only be declared for subtree
                            // with NULL root.
#else
  std::vector<unsigned char> members;
                            // Bitset of opcodes that are members of the subset
#endif
};

static int isa_subset_count = 0;    // How many subsets?
static std::list<ISA_SUBSET> subsets;    // All the subsets
static size_t bit_vector_sizeof;    // How many bytes in a bit set of all
                                    //  opcodes
static std::vector<ISA_SUBSET> opcode_subset;
                                    // Which subset introduces the opcode?
// In following loops, we iterate on the number of
// TOP. This number differs following we generate
// static or dynamic TOP.
#ifdef TARG_ST
#ifndef DYNAMIC_CODE_GEN
static TOP TOP_count_limit = TOP_static_count;
#else
static TOP TOP_count_limit = TOP_dyn_count;
#endif
#endif

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   A description of the ISA subset hierarchy.  The description",
  " *   exports the following:",
  " *",
  " *   typedef (enum) ISA_SUBSET",
  " *       An enumberated type of the different subsets.",
  " *",
  " *   const ISA_SUBSET ISA_SUBSET_UNDEFINED",
  " *       Useful value guaranteed not to be a valid ISA_SUBSET.",
  " *",
  " *   extern ISA_SUBSET ISA_SUBSET_Value",
  " *       A variable containing the current subset value.",
  " *",
  " *   const char* ISA_SUBSET_Name( ISA_SUBSET subset )",
  " *       Returns a name suitable for printing.",
  " *",
  " *   int ISA_SUBSET_Member( ISA_SUBSET subset, TOP opcode )",
  " *       Is the given <opcode> a member of the given <subset>?",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};

#ifdef TARG_ST
/////////////////////////////////////
void ISA_Subset_Begin( const char* /* name */ )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
   return;
}
#else
/////////////////////////////////////
void ISA_Subset_Begin( const char* /* name */ )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  bit_vector_sizeof =   (TOP_count + 7) / 8;
  opcode_subset = std::vector<ISA_SUBSET>(TOP_count,(ISA_SUBSET)0);
  for ( int code =  0; code < TOP_count; ++code )
    opcode_subset[code] = NULL;
}
#endif

#ifdef TARG_ST
/////////////////////////////////////
ISA_SUBSET ISA_Subset_Create( ISA_SUBSET parent, const char* name )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  ISA_SUBSET result = new isa_subset;

  result->name = name;
  result->index = isa_subset_count++;
  result->superset = parent;
  result->members = std::vector<bool>(TOP_count_limit,false);
  result->is_dyn_extensible = false;

  subsets.push_front(result);

  return result;
}
#else
/////////////////////////////////////
ISA_SUBSET ISA_Subset_Create( ISA_SUBSET parent, const char* name )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  ISA_SUBSET result = new isa_subset;

  result->name = name;
  result->index = isa_subset_count++;
  result->superset = parent;
  result->members = std::vector<unsigned char>(bit_vector_sizeof,0);

  subsets.push_front(result);

  return result;
}
#endif
#ifdef TARG_ST
/////////////////////////////////////
void ISA_Subset_Is_Extensible( ISA_SUBSET subset )
/////////////////////////////////////
// Set a subset as being able to handle extensions.
/////////////////////////////////////
{
   if(subset->superset!=NULL)
    { fprintf(stderr,
              " ### Extensibility is currently a global property that cannot applied to sub-subset.\n");
      exit(EXIT_FAILURE);
    }

   subset->is_dyn_extensible=true;
   return;
}
#endif


#ifdef TARG_ST
/////////////////////////////////////
void Instruction_Group( ISA_SUBSET subset, ... )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  va_list ap;
  TOP opcode;

  /*
   * For static code generation, TOP_UNDEFINED is the
   * last item of the vararg list. In case of dynamic
   * code generation, the static value of TOP_UNDEFINED
   * cannot be used anymore.  Indeed we can define in the
   * dynamic part of the code, more instructions than
   * they are in the static instruction set.
   * To solve the problem, we suppose that in case
   * of dynamic code generation, vararg list ends with -1.
   */

  TOP stop = Is_Static_Code() ? TOP_UNDEFINED : static_cast<TOP>(-1);

  va_start(ap,subset);
  while ( (opcode = static_cast<TOP>(va_arg(ap,int))) != stop ) {
    ISA_SUBSET ss;

    for ( ss = subset; ss != NULL; ss = ss->superset )
        ss->members[(mUINT32)opcode] = true;

  }                          // End while.
  va_end(ap);
}
#else
/////////////////////////////////////
void Instruction_Group( ISA_SUBSET subset, ... )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  va_list ap;
  TOP opcode;

  va_start(ap,subset);
  while ( (opcode = static_cast<TOP>(va_arg(ap,int))) != TOP_UNDEFINED ) {
    ISA_SUBSET ss;
    int byte_index = ((unsigned int) opcode) / 8;
    int bit_index = ((unsigned int) opcode) % 8;

    for ( ss = subset; ss != NULL; ss = ss->superset )
      ss->members[byte_index] |= (1 << bit_index);
    if ( opcode_subset[opcode] != NULL ) {
      fprintf(stderr,"### attempting to add %s to ISA subset %s but "
                     "already in %s\n",
              TOP_Name(opcode),
              subset->name,
              opcode_subset[opcode]->name);
      exit(EXIT_FAILURE);
    }
    opcode_subset[opcode] = subset;
  }
  va_end(ap);
}
#endif

#ifdef TARG_ST
/////////////////////////////////////
void ISA_Subset_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////

{
// [HK]
  std::list<ISA_SUBSET>::iterator isi;

  static FILE* hfile    = NULL ;
  static FILE* cfile    = NULL ;
  static FILE* sfile    = NULL ;
  static FILE* efile    = NULL ;

  // Whether we generate code for the core (static) or for an extension.
  bool  gen_static_code = Is_Static_Code();

  // Get extension name or NULL for static code generation.
  char *extname = gen_static_code ? NULL : Get_Extension_Name();

  char *hfilename     = NULL ;    /* Header file name              */
  char *cfilename     = NULL ;    /* C file name                   */
  char *sfilename     = NULL ;    /* Generator stub in dynamic mode*/
  char *efilename     = NULL ;    /* Export file name              */

  const char * const bname = FNAME_TARG_ISA_SUBSET;

  int k ;

  // Beginning of code.
  // Opening files.
  hfilename = Gen_Build_Filename(bname,extname,gen_util_file_type_hfile);
  hfile     = Gen_Open_File_Handle(hfilename, "w");

  cfilename = Gen_Build_Filename(bname,extname,gen_util_file_type_cfile);
  cfile     = Gen_Open_File_Handle(cfilename, "w");

  if(gen_static_code) {
    efilename = Gen_Build_Filename(bname,extname,gen_util_file_type_efile);
    efile     = Gen_Open_File_Handle(efilename, "w");
  }
  else {
    sfilename = Gen_Build_Filename(FNAME_STUB_ISA_SUBSET,extname,gen_util_file_type_cfile);
    sfile     = Gen_Open_File_Handle(sfilename, "w");
  }

  if(gen_static_code) {
    fprintf(cfile,"#include <string.h>\n");
    fprintf(cfile,"#include \"%s.h\"\n", bname);
  } else {
    char *static_name;
    const char *headers[] = {
      "\"dyn_" FNAME_ISA_SUBSET ".h\"",
      "",
    };
    
    
    static_name = Gen_Build_Filename(bname,NULL,gen_util_file_type_hfile);
    
    fprintf(cfile,"#include \"%s\"\n",static_name);
    fprintf(cfile,"#include \"%s\"\n\n",hfilename);
    
    Gen_Free_Filename(static_name);
    Emit_Stub_Header(sfile,headers);
  }

  Emit_Header (hfile, bname, interface,extname);
  fprintf(hfile,"#include \"topcode.h\"\n");

  if (gen_static_code) { 
    fprintf(hfile,"\ntypedef UINT32 ISA_SUBSET;\n");
    fprintf(hfile,"\nBE_EXPORTED extern INT ISA_SUBSET_count;\n");
    fprintf(efile,"ISA_SUBSET_count\n");
    fprintf(hfile,"\n#define ISA_SUBSET_MIN ((ISA_SUBSET)0)\n");
    fprintf(hfile,"#define ISA_SUBSET_MAX (ISA_SUBSET_MIN+ISA_SUBSET_count-1)\n");
    fprintf(hfile,"#define ISA_SUBSET_UNDEFINED	(ISA_SUBSET_MIN+ISA_SUBSET_count)\n");
    fprintf(hfile,"#define ISA_SUBSET_COUNT_MAX (32) /* At most sizeof(UINT32) for masks of subsets. */\n");
  }

  if (gen_static_code) {
    fprintf(hfile,"\n");
  }
  for ( k = 0, isi = subsets.begin(); isi != subsets.end(); ++isi, ++k ) {
    ISA_SUBSET subset = *isi;
    fprintf(hfile,"#define ISA_SUBSET_%s ISA_SUBSET_MIN+%d\n", 
	    subset->name, k);
  }
  if (gen_static_code) {
    fprintf(hfile, "#define ISA_SUBSET_static_count (%d)\n", isa_subset_count);
    fprintf(hfile, "\n");
  }
  if (gen_static_code) {
    fprintf(hfile,"\n");
  }
  for ( k = 0, isi = subsets.begin(); isi != subsets.end(); ++isi, ++k ) {
    ISA_SUBSET subset = *isi;
    if (gen_static_code) {
      fprintf(hfile,"#define ISA_SUBSET_%s ISA_SUBSET_MIN+%d\n", 
	      subset->name, k);
    }
  }
  if (gen_static_code) {
    fprintf(hfile, "#define ISA_SUBSET_static_count (%d)\n", isa_subset_count);
    fprintf(hfile, "\n");
  }

  if (gen_static_code) {
    fprintf(cfile,"\nINT ISA_SUBSET_count = ISA_SUBSET_static_count;\n");
  } else {
    fprintf(cfile,"\n#define ISA_SUBSET_COUNT (%d)\n", isa_subset_count);
    fprintf(sfile,"\n#define ISA_SUBSET_COUNT (%d)\n", isa_subset_count);
    fprintf(sfile,"\nINT ISA_SUBSET_count = ISA_SUBSET_COUNT;\n");
  }
  fprintf(cfile,"\nstatic const char* isa_subset_names[%s] = {\n",
          gen_static_code ? "ISA_SUBSET_COUNT_MAX":"ISA_SUBSET_COUNT");

  for ( isi = subsets.begin(); isi != subsets.end(); ++isi ) {
    ISA_SUBSET subset = *isi;
    fprintf(cfile,"  \"%s\",", subset->name);
  }
  fprintf(cfile,"\n};\n");

  if(gen_static_code) {
    fprintf(cfile,"#define ISA_SUBSET_UNDEFINED_Name \"UNDEFINED\"\n\n");
  }
  else {
    fprintf(sfile,"#define ISA_SUBSET_UNDEFINED_Name \"UNDEFINED\"\n\n");
  }

  if(gen_static_code) {
    fprintf(hfile,"BE_EXPORTED extern const char* ISA_SUBSET_Name( ISA_SUBSET subset );\n");
    fprintf(efile,"ISA_SUBSET_Name\n");
    fprintf(cfile, "const char*\n");
    fprintf(cfile, "ISA_SUBSET_Name( ISA_SUBSET subset ) {\n");
    fprintf(cfile, "  if (subset == ISA_SUBSET_UNDEFINED) return ISA_SUBSET_UNDEFINED_Name;\n");
    fprintf(cfile, "  return isa_subset_names[(INT)subset];\n");
    fprintf(cfile, "}\n");
    fprintf(cfile, "\n");
  }
  else {
    fprintf(sfile, "const char*\n");
    fprintf(sfile, "ISA_SUBSET_Name( ISA_SUBSET subset ) {\n");
    fprintf(sfile, "  if (subset > ISA_SUBSET_count) return ISA_SUBSET_UNDEFINED_Name;\n");
    fprintf(sfile, "  return dyn_get_ISA_SUBSET_tab()[(INT)subset];\n");
    fprintf(sfile, "}\n");
    fprintf(sfile, "\n");
  }

  if(gen_static_code) {
    fprintf(hfile,"BE_EXPORTED extern ISA_SUBSET ISA_SUBSET_From_Name( const char *subset_name );\n");
    fprintf(efile,"ISA_SUBSET_From_Name\n");
    fprintf(cfile, "ISA_SUBSET\n");
    fprintf(cfile, "ISA_SUBSET_From_Name(const char *subsetname)\n");
    fprintf(cfile, "{\n");
    fprintf(cfile, "  ISA_SUBSET i;\n");
    fprintf(cfile, "  for (i = ISA_SUBSET_MIN; i <= ISA_SUBSET_MAX; i++) {\n");
    fprintf(cfile, "    if (strcmp(isa_subset_names[i], subsetname) == 0)\n");
    fprintf(cfile, "      return i;\n");
    fprintf(cfile, "  }\n");
    fprintf(cfile, "  return ISA_SUBSET_UNDEFINED;\n");
    fprintf(cfile, "}\n");
    fprintf(cfile, "\n");
  }    // gen_static_code
  

  for ( isi = subsets.begin(),k=0; isi != subsets.end(); ++isi,++k ) {
    ISA_SUBSET subset = *isi;

    fprintf(cfile,
            "static const unsigned char isa_subset_%d_opcode_table[%d] =\n",
            k, TOP_count_limit);

    fprintf(cfile,"  { /* %s */\n", subset->name);
    for ( unsigned int i = 0; i < TOP_count_limit; ++i ) {
      fprintf(cfile,
              "   %d,  /* %-30s */\n",subset->members[i]==true ? 1 : 0,
              TOP_Name((TOP)i));
    }
    fprintf(cfile,"};\n\n");
  }

  /* 
   * Now generate the global table.
   * Number of entries in table depends on
   * the number of subsets. 
   */
  if(gen_static_code) {
    fprintf(cfile,"\n\n");
    fprintf(cfile, "typedef struct \n");
    fprintf(cfile, "{\n");
    fprintf(cfile, "  UINT32 base_opcode;\n");
    fprintf(cfile, "  UINT32 count;\n");
    fprintf(cfile, "  const unsigned char *members;  \n");
    fprintf(cfile, "} ISA_SUBSET_MEMBERS;\n");
    fprintf(cfile, "\n");
    fprintf(cfile, "static ISA_SUBSET_MEMBERS empty_members = { 0, 0, (const unsigned char *)0 };\n");
    fprintf(cfile, "\n");
    fprintf(cfile, "static ISA_SUBSET_MEMBERS isa_subset_members[ISA_SUBSET_COUNT_MAX] = {\n");
    for(k=0; k < isa_subset_count; k++) { 
      fprintf(cfile, "   { %d, %d, isa_subset_%d_opcode_table} ,\n", 0, TOP_count_limit, k);
    }
    fprintf(cfile,"};\n\n\n");
  }
  else {
    fprintf(sfile,"\n\n");
    fprintf(sfile, "typedef struct \n");
    fprintf(sfile, "{\n");
    fprintf(sfile, "  UINT32 base_opcode;\n");
    fprintf(sfile, "  UINT32 count;\n");
    fprintf(sfile, "  const unsigned char *members;  \n");
    fprintf(sfile, "} ISA_SUBSET_MEMBERS;\n");
    fprintf(sfile, "\n");
    fprintf(sfile, "static ISA_SUBSET_MEMBERS empty_members = { 0, 0, (const unsigned char *)0 };\n");
    fprintf(sfile, "\n");
    fprintf(sfile, "static ISA_SUBSET_MEMBERS isa_subset_members[ISA_SUBSET_COUNT] = {\n");
    fprintf(sfile, "   { %d, %d, dyn_get_ISA_SUBSET_op_tab()[0]} ,\n", 0, TOP_count_limit);
    fprintf(sfile,"};\n\n\n");

    fprintf(cfile,
	    "\n\n"
	    "static const unsigned char *ISA_SUBSET_dyn_opcode_table[%d] = {\n",
	    isa_subset_count);
    for(k=0;k<isa_subset_count;k++) {
      fprintf(cfile,"isa_subset_%d_opcode_table,\n",k);
    }
    fprintf(cfile,"};\n\n\n");
  }
  
  if(gen_static_code) {
    fprintf(hfile,"BE_EXPORTED extern INT ISA_SUBSET_Member( ISA_SUBSET subset,\n"
	    "                              TOP opcode );\n");
    fprintf(efile,"ISA_SUBSET_Member\n");
    fprintf(cfile, "int\n");
    fprintf(cfile, "ISA_SUBSET_Member(ISA_SUBSET subset, TOP opcode)\n");
    fprintf(cfile, "{\n");
    fprintf(cfile, "  ISA_SUBSET_MEMBERS *members;\n");
    fprintf(cfile, "  if (subset == ISA_SUBSET_UNDEFINED) return 0;\n");
    fprintf(cfile, "  members = &isa_subset_members[subset-ISA_SUBSET_MIN];\n");
    fprintf(cfile, "  if (opcode < members->base_opcode || opcode >= members->base_opcode + members->count) return 0;\n");
    fprintf(cfile, "  return members->members[opcode-members->base_opcode];\n");
    fprintf(cfile, "}\n");
    fprintf(cfile, "\n");

    fprintf(hfile,"\nstruct ISA_SUBSET_LIST;\n");
    fprintf(cfile, "typedef struct ISA_SUBSET_LIST {\n");
    fprintf(cfile, "  INT count;\n");
    fprintf(cfile, "  ISA_SUBSET *begin;\n");
    fprintf(cfile, "} ISA_SUBSET_LIST;\n");
    fprintf(cfile, "\n");

    fprintf(hfile,"BE_EXPORTED extern struct ISA_SUBSET_LIST *ISA_SUBSET_List;\n");
    fprintf(efile,"ISA_SUBSET_List\n");
    fprintf(cfile, "static ISA_SUBSET ISA_SUBSET_List__[ISA_SUBSET_COUNT_MAX];\n");
    fprintf(cfile, "static ISA_SUBSET_LIST ISA_SUBSET_List_ = { 0, &ISA_SUBSET_List__[0] };\n");
    fprintf(cfile, "ISA_SUBSET_LIST *ISA_SUBSET_List = &ISA_SUBSET_List_;\n");
    fprintf(cfile, "\n");

    fprintf(hfile,"BE_EXPORTED extern void ISA_SUBSET_LIST_Add(struct ISA_SUBSET_LIST *,  ISA_SUBSET subset);\n");
    fprintf(efile,"ISA_SUBSET_LIST_Add\n");
    fprintf(cfile, "void\n");
    fprintf(cfile, "ISA_SUBSET_LIST_Add(ISA_SUBSET_LIST *list, ISA_SUBSET subset)\n");
    fprintf(cfile, "{ \n");
    fprintf(cfile, "  list->begin[list->count++] = subset;\n");
    fprintf(cfile, "}\n");

    fprintf(hfile,"BE_EXPORTED extern INT ISA_SUBSET_LIST_Contains(struct ISA_SUBSET_LIST *, ISA_SUBSET subset);\n");
    fprintf(efile,"ISA_SUBSET_LIST_Contains\n");
    fprintf(cfile, "INT\n");
    fprintf(cfile, "ISA_SUBSET_LIST_Contains(ISA_SUBSET_LIST *list, ISA_SUBSET subset)\n");
    fprintf(cfile, "{ \n");
    fprintf(cfile, "  ISA_SUBSET *it;\n");
    fprintf(cfile, "  for (it = ISA_SUBSET_LIST_Begin(list); it != ISA_SUBSET_LIST_End(list); it++) {\n");
    fprintf(cfile, "    if (*it == subset) return TRUE;\n");
    fprintf(cfile, "  }\n");
    fprintf(cfile, "  return FALSE;\n");
    fprintf(cfile, "}\n");
    fprintf(cfile, "\n");

    fprintf(hfile,"BE_EXPORTED extern INT ISA_SUBSET_LIST_Member(struct ISA_SUBSET_LIST *, TOP opcode);\n");
    fprintf(efile,"ISA_SUBSET_LIST_Member\n");
    fprintf(cfile, "INT\n");
    fprintf(cfile, "ISA_SUBSET_LIST_Member(ISA_SUBSET_LIST *list,TOP opcode)\n");
    fprintf(cfile, "{ \n");
    fprintf(cfile, "  ISA_SUBSET *it;\n");
    fprintf(cfile, "  for (it = ISA_SUBSET_LIST_Begin(list); it != ISA_SUBSET_LIST_End(list); it++) {\n");
    fprintf(cfile, "    if (ISA_SUBSET_Member(*it, opcode)) return TRUE;\n");
    fprintf(cfile, "  }\n");
    fprintf(cfile, "  return FALSE;\n");
    fprintf(cfile, "}\n");
    fprintf(cfile, "\n");

    fprintf(hfile,"BE_EXPORTED extern ISA_SUBSET *ISA_SUBSET_LIST_Begin(struct ISA_SUBSET_LIST *);\n");
    fprintf(efile,"*ISA_SUBSET_LIST_Begin\n");
    fprintf(cfile, "ISA_SUBSET *\n");
    fprintf(cfile, "ISA_SUBSET_LIST_Begin(ISA_SUBSET_LIST *list)\n");
    fprintf(cfile, "{\n");
    fprintf(cfile, "  return list->begin;\n");
    fprintf(cfile, "}\n");
    fprintf(cfile, "\n");

    fprintf(hfile,"BE_EXPORTED extern ISA_SUBSET *ISA_SUBSET_LIST_End(struct ISA_SUBSET_LIST *);\n");
    fprintf(efile,"*ISA_SUBSET_LIST_End\n");
    fprintf(cfile, "ISA_SUBSET *\n");
    fprintf(cfile, "ISA_SUBSET_LIST_End(ISA_SUBSET_LIST *list)\n");
    fprintf(cfile, "{\n");
    fprintf(cfile, "  return list->begin + list->count;\n");
    fprintf(cfile, "}\n");
    fprintf(cfile, "\n");

    fprintf(hfile,"BE_EXPORTED extern INT ISA_SUBSET_LIST_Count(struct ISA_SUBSET_LIST *);\n");
    fprintf(efile,"ISA_SUBSET_LIST_Count\n");
    fprintf(cfile, "INT\n");
    fprintf(cfile, "ISA_SUBSET_LIST_Count(ISA_SUBSET_LIST *list)\n");
    fprintf(cfile, "{\n");
    fprintf(cfile, "  return list->count;\n");
    fprintf(cfile, "}\n");
    fprintf(cfile, "\n");

    fprintf(hfile,"BE_EXPORTED extern UINT32 ISA_SUBSET_LIST_Mask(struct ISA_SUBSET_LIST *);\n");
    fprintf(efile,"ISA_SUBSET_LIST_Mask\n");
    fprintf(cfile, "UINT32\n");
    fprintf(cfile, "ISA_SUBSET_LIST_Mask(ISA_SUBSET_LIST *list)\n");
    fprintf(cfile, "{ \n");
    fprintf(cfile, "  UINT32 mask = 0;\n");
    fprintf(cfile, "  ISA_SUBSET *it;\n");
    fprintf(cfile, "  for (it = ISA_SUBSET_LIST_Begin(list); it != ISA_SUBSET_LIST_End(list); it++) {\n");
    fprintf(cfile, "    mask |= (UINT32)1 << *it;\n");
    fprintf(cfile, "  }\n");
    fprintf(cfile, "  return mask;\n");
    fprintf(cfile, "}\n");
    fprintf(cfile, "\n");

    // Interface for reconfigurability support.
    fprintf(hfile,"\nBE_EXPORTED extern ISA_SUBSET EXTENSION_ISA_SUBSET_New(const char *subsetname);\n");
    fprintf(efile,"EXTENSION_ISA_SUBSET_New\n");
    fprintf(cfile, "ISA_SUBSET\n");
    fprintf(cfile, "EXTENSION_ISA_SUBSET_New(const char *subsetname)\n");
    fprintf(cfile, "{\n");
    fprintf(cfile, "  isa_subset_names[ISA_SUBSET_count] = subsetname;\n");
    fprintf(cfile, "  isa_subset_members[ISA_SUBSET_count] = empty_members;\n");
    fprintf(cfile, "  return ISA_SUBSET_count++;\n");
    fprintf(cfile, "}\n");
    fprintf(cfile, "\n");

    fprintf(hfile,"BE_EXPORTED extern void EXTENSION_ISA_SUBSET_Set_Members(ISA_SUBSET subset, int base_top, int top_count, const unsigned char *ext_top_members);\n");
    fprintf(efile,"EXTENSION_ISA_SUBSET_Set_Members\n");
    fprintf(cfile, "void \n");
    fprintf(cfile, "EXTENSION_ISA_SUBSET_Set_Members(ISA_SUBSET subset, int base_top, int top_count, const unsigned char *ext_top_members)\n");
    fprintf(cfile, "{\n");
    fprintf(cfile, "  ISA_SUBSET_MEMBERS *members = &isa_subset_members[subset];\n");
    fprintf(cfile, "  members->base_opcode = base_top;\n");
    fprintf(cfile, "  members->count = top_count;\n");
    fprintf(cfile, "  members->members = ext_top_members;\n");
    fprintf(cfile, "}\n");
    fprintf(cfile, "\n");

  }
  else {
  const char * const fct1_name = "dyn_get_ISA_SUBSET_tab";
  const char * const fct2_name = "dyn_get_ISA_SUBSET_tab_sz";
  const char * const fct3_name = "dyn_get_ISA_SUBSET_op_tab";


  fprintf(sfile, "int\n");
  fprintf(sfile, "ISA_SUBSET_Member(ISA_SUBSET subset, TOP opcode)\n");
  fprintf(sfile, "{\n");
  fprintf(sfile, "  ISA_SUBSET_MEMBERS *members;\n");
  fprintf(sfile, "  if (subset >= ISA_SUBSET_count) return 0;\n");
  fprintf(sfile, "  members = &isa_subset_members[subset-ISA_SUBSET_MIN];\n");
  fprintf(sfile, "  if (opcode < members->base_opcode || opcode >= members->base_opcode + members->count) return 0;\n");
  fprintf(sfile, "  return members->members[opcode-members->base_opcode];\n");
  fprintf(sfile, "}\n");
  fprintf(sfile, "\n");

  fprintf(sfile,
	  "\n"
	  "/* Beginning of referenced but not called objects ===================================== */\n"
	  "\n"
	  "/*\n"
	  " * Avoids some link problems due to reference in targ_isa_registers.cxx. \n"
	  " * these variables functions are not actually used by the extension generator.\n"
	  " */\n"
	  "  ISA_SUBSET_LIST *ISA_SUBSET_List = (ISA_SUBSET_LIST *)0;\n"
	  "UINT32 ISA_SUBSET_LIST_Mask(struct ISA_SUBSET_LIST *)\n"
	  "{\n"
	  "  return 0;\n"
	  "}\n"
	  "/* End of referenced but not called objects ===================================== */\n");

  /* Declaration of routines in header files */
  fprintf(hfile,
          "\n"
          "extern const char* const *  %s ( void );\n"
          "extern const mUINT32 %s ( void );\n"
          "extern unsigned const char** %s ( void );\n"
          "\n",
          fct1_name,
          fct2_name,
          fct3_name);

  fprintf(cfile,                      /* Printing routine 1 */
          "\n"
          "const char * const * %s ( void )\n"
          "{ return %s;\n"
          "}\n"
          "\n",
          fct1_name,
          "isa_subset_names"
          );

   fprintf(cfile,                      /* Printing routine 2 */
           "const mUINT32 %s ( void )\n"
           "{ return (const mUINT32) %d;\n"
           "}\n"
           "\n",
           fct2_name,
           isa_subset_count
           );

   fprintf(cfile,                      /* Printing routine 3 */
           "unsigned const char** %s ( void )\n"
           "{ return %s;\n"
           "}\n"
           "\n",
           fct3_name,
           "ISA_SUBSET_dyn_opcode_table"
           ); 
  }

  Emit_Footer (hfile);

  // Closing file handlers.
  Gen_Close_File_Handle(hfile,hfilename);
  Gen_Close_File_Handle(cfile,cfilename);
  if(efile)
    Gen_Close_File_Handle(efile,efilename);
  if(sfile)
    Gen_Close_File_Handle(sfile,sfilename);

  // Memory deallocation.
  Gen_Free_Filename(cfilename);
  Gen_Free_Filename(hfilename);
  if(efilename)
    Gen_Free_Filename(efilename);
  if(sfilename)
    Gen_Free_Filename(sfilename);

  return;

}
#else
/////////////////////////////////////
void ISA_Subset_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  std::list<ISA_SUBSET>::iterator isi;
  bool err;
  int code;

  for ( err = false, code = 0; code < TOP_count; ++code ) {
    if ( ! opcode_subset[code] ) {
      fprintf(stderr,"### Error: no opcode subset for %s\n",
                     TOP_Name((TOP)code));
      err = true;
    }
  }
  if (err) exit(EXIT_FAILURE);

#define FNAME	"targ_isa_subset"
  char filename[1000];
  sprintf(filename,"%s.h", FNAME);
  FILE* hfile = fopen(filename,"w");
  sprintf(filename,"%s.c", FNAME);
  FILE* cfile = fopen(filename,"w");
  sprintf(filename,"%s.Exported", FNAME);
  FILE* efile = fopen(filename,"w");

  fprintf(cfile,"#include \"topcode.h\"\n");
  fprintf(cfile,"#include \"%s.h\"\n", FNAME);

  sprintf (filename, "%s", FNAME);
  Emit_Header (hfile, filename, interface);
  fprintf(hfile,"#include \"topcode.h\"\n");

  fprintf(hfile,"\ntypedef enum {\n");
  fprintf(cfile,"\nstatic const char* const isa_subset_names[] = {\n");

  for ( isi = subsets.begin(); isi != subsets.end(); ++isi ) {
    ISA_SUBSET subset = *isi;
    fprintf(hfile,"  ISA_SUBSET_%s,\n", subset->name);
    fprintf(cfile,"  \"%s\",", subset->name);
  }
  fprintf(hfile,"  ISA_SUBSET_UNDEFINED,\n"
		"  ISA_SUBSET_MIN=ISA_SUBSET_%s,\n"
		"  ISA_SUBSET_MAX=ISA_SUBSET_%s\n"
		"} ISA_SUBSET;\n",
		(*subsets.begin())->name,
		(*subsets.rbegin())->name);
  fprintf(cfile,"  \"UNDEFINED\"\n"
		"};\n");

  fprintf(hfile,"extern ISA_SUBSET ISA_SUBSET_Value;\n\n");
  fprintf(efile,"ISA_SUBSET_Value\n");
  fprintf(cfile,"ISA_SUBSET ISA_SUBSET_Value = ISA_SUBSET_UNDEFINED;\n\n");

  fprintf(hfile,"extern const char* ISA_SUBSET_Name( ISA_SUBSET subset );\n");
  fprintf(efile,"ISA_SUBSET_Name\n");
  fprintf(cfile,"const char* ISA_SUBSET_Name( ISA_SUBSET subset ) {\n");
  fprintf(cfile,"  return isa_subset_names[(INT)subset];\n");
  fprintf(cfile,"}\n");

#ifdef KEY /* Mac port */
  fprintf(cfile,"static const char isa_subset_opcode_table[%d][%ld] = {\n",
          isa_subset_count+1,(long) bit_vector_sizeof);
#else /* KEY Mac port */
  fprintf(cfile,"static const char isa_subset_opcode_table[%d][%d] = {\n",
          isa_subset_count+1,bit_vector_sizeof);
#endif /* KEY Mac port */

  for ( isi = subsets.begin(); isi != subsets.end(); ++isi ) {
    ISA_SUBSET subset = *isi;

    fprintf(cfile,"  { /* %s */\n", subset->name);
    for ( int i = 0; i < bit_vector_sizeof; ++i ) {
      int members = subset->members[i];
      fprintf(cfile,"    0%03o, /* ",members);
      for (int j = 0; j < 8; ++j) {
	if (members & (1 << j)) {
	  TOP top = (TOP)((i * 8) + j);
	  fprintf(cfile,"%s ",TOP_Name(top));
	}
      }
      fprintf(cfile,"*/\n");
    }
    fprintf(cfile,"  },\n");
  }
  fprintf(cfile,"  { /* UNDEFINED */\n"
		"    0\n"
		"  }\n");
  fprintf(cfile,"};\n");

  fprintf(hfile,"extern INT ISA_SUBSET_Member( ISA_SUBSET subset,\n"
                "                              TOP opcode );\n");
  fprintf(efile,"ISA_SUBSET_Member\n");
  fprintf(cfile,
	  "int ISA_SUBSET_Member( ISA_SUBSET subset, TOP opcode )\n"
	  "{\n"
	  "  INT byte_index = ((UINT) opcode) / 8;\n"
	  "  INT bit_index = ((UINT) opcode) %% 8;\n"
	  "  INT byte = isa_subset_opcode_table[(int) subset][byte_index];\n"
	  "  return (byte >> bit_index) & 1;\n"
	  "}\n");

  Emit_Footer (hfile);
}
#endif
