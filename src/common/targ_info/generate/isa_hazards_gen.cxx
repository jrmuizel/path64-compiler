/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


//  isa_hazards_gen.cxx
/////////////////////////////////////
//
//  Description:
//
//      Generate a description of the ISA hazards. 
//
/////////////////////////////////////
//
//  $Revision: 1.6 $
//  $Date: 04/12/21 14:57:26-08:00 $
//  $Author: bos@eng-25.internal.keyresearch.com $
//  $Source: /home/bos/bk/kpro64-pending/common/targ_info/generate/SCCS/s.isa_hazards_gen.cxx $


#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <strings.h>
#include <assert.h>
#include <list>
#include <vector>
#ifdef TARG_ST
#ifdef DYNAMIC_CODE_GEN
#include "dyn_isa_topcode.h"
#else
#include "topcode.h"
#endif

#include "gen_util.h"
#include "targ_isa_subset.h"
#include "isa_hazards_gen.h"

struct isa_hazard {
  const char *name;         // hazard name
};

struct haz_desc {
  isa_hazard *type;
  int data;
  int pre_ops;
  int post_ops;
  UINT32 subsets[ISA_SUBSET_MIN+ISA_SUBSET_COUNT_MAX];
};

struct op_haz {
  haz_desc *desc;
  struct op_haz *next;
  int index;
};

// In following loops, we iterate on the number of
// TOP. This number differs whether we generate
// static or dynamic TOPs.
#ifndef DYNAMIC_CODE_GEN
static mUINT32 TOP_count_limit = TOP_static_count;
#else
static mUINT32 TOP_count_limit = TOP_dyn_count;
#endif

// ISA_SUBSETS iterators overrides.
#ifndef DYNAMIC_CODE_GEN
#define LOCAL_ISA_SUBSET_MAX (ISA_SUBSET_MAX)
#else
// For extension generatores, we allow only the use of static subsets.
#define LOCAL_ISA_SUBSET_MAX (ISA_SUBSET_MIN+ISA_SUBSET_static_count-1)
#endif


static std::list<ISA_HAZARD> hazards;    // All the hazards
static std::vector <op_haz*> op_hazards(TOP_count_limit,static_cast<op_haz*>(NULL));
static std::list<op_haz *> op_hazards_list;
static haz_desc *current_haz_desc;
static int haz_index;

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   A description of the ISA hazards. The description exports",
  " *   the following:",
  " *",
  " *   typedef (enum) ISA_HAZARD",
  " *       An enumeration of the hazard types, and ISA_HAZARD_UNDEFINED.",
  " *",
  " *   typedef (struct) ISA_HAZARD_INFO",
  " *       Describes a particular hazard. The contents are private.",
  " *",
  " *   BOOL ISA_HAZARD_TOP_Has_Hazard(TOP topcode)",
  " *       Returns TRUE if the instruction specified by 'topcode'",
  " *       has a hazard.",
  " *",
  " *   ISA_HAZARD_INFO *ISA_HAZARD_First(TOP topcode)",
  " *       Get the first hazard description for 'topcode'.",
  " *",
  " *   ISA_HAZARD_INFO *ISA_HAZARD_Next(ISA_HAZARD_INFO *info)",
  " *       Gets the next hazard description when a 'topcode' has",
  " *       more than one hazard.",
  " *",
  " *   ISA_HAZARD ISA_HAZARD_Type(ISA_HAZARD_INFO *info)",
  " *       Returns the type of the hazard.",
  " *",
  " *   INT ISA_HAZARD_Data(ISA_HAZARD_INFO *info)",
  " *       Returns the hazard specific data.",
  " *",
  " *   INT ISA_HAZARD_Pre_Ops(ISA_HAZARD_INFO *info)",
  " *       Returns the number of OPs that must precede the instruction",
  " *       with the hazard.",
  " *",
  " *   INT ISA_HAZARD_Post_Ops(ISA_HAZARD_INFO *info)",
  " *       Returns the number of OPs that must follow the instruction",
  " *       with the hazard.",
  " *",
  " *   void ISA_HAZARD_Initialize(void)",
  " *       Initializes the hazard description data for ISA_SUBSET_Value.",
  " *       This may only be called once (if not called at all the description",
  " *       contains the hazards for all ISAs).",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};

///////////////////////////////////////////////////
// Actual status of hazards for dynamic extension is
// very limited. These limitations will be removed
// in forthcoming releases.
//////////////////////////////////////////////////


/////////////////////////////////////
void ISA_Hazards_Begin( const char* /* name */ )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
}

/////////////////////////////////////
ISA_HAZARD Hazard_Create( const char *name )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  // Limitation to be removed
  if(Is_Dynamic_Code()) {
    fprintf(stderr,
      "### Error: call to routine %s is not supported yet for dynamic extensions\n",
      "Hazard_Create");
    exit(EXIT_FAILURE);
   }

  ISA_HAZARD result = new isa_hazard;
  bzero(result, sizeof(isa_hazard));
  hazards.push_back(result);
  result->name = name;
  return result;
}

/////////////////////////////////////
void Hazard_Group( TOP topcode, ... )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  va_list ap;
  TOP opcode;
  int count = 0;

  TOP TOP_limit;

  // Limitation to be removed
  if(Is_Dynamic_Code()) {
    fprintf(stderr,
     "### Error: call to routine %s is not supported yet for dynamic extensions\n",
     "Hazard_Group");
    exit(EXIT_FAILURE);
   }


  TOP_limit= Is_Static_Code() ? TOP_UNDEFINED : 
             static_cast<TOP>(-1);

  current_haz_desc = new haz_desc;
  bzero(current_haz_desc, sizeof(haz_desc));

  va_start(ap,topcode);
  for (opcode = topcode;
       opcode != TOP_limit;
       opcode = static_cast<TOP>(va_arg(ap,int))) {
    op_haz *op_hazard = new op_haz;
    op_hazards_list.push_back(op_hazard);
    op_hazard->desc = current_haz_desc;
    op_hazard->next = op_hazards[(int)opcode];
    op_hazard->index = ++haz_index;
    op_hazards[(int)opcode] = op_hazard;
    ++count;
  }
  va_end(ap);

  if (count == 0) {
    fprintf(stderr, "### Warning: hazard group is empty\n");
  }
}


/////////////////////////////////////
void Hazard_Type( ISA_HAZARD isa_hazard )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  // Limitation to be removed
  if(Is_Dynamic_Code()) {
    fprintf(stderr,
     "### Error: call to routine %s is not supported yet for dynamic extensions\n",
     "Hazard_Type");
    exit(EXIT_FAILURE);
   }

  current_haz_desc->type = isa_hazard;
}


/////////////////////////////////////
void Hazard_Data( int data )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if(Is_Dynamic_Code()) {
    fprintf(stderr,
     "### Error: call to routine %s is not supported yet for dynamic extensions\n",
     "Hazard_Data");
    exit(EXIT_FAILURE);
   }

  current_haz_desc->data = data;
}


/////////////////////////////////////
void Hazard_Post_Ops( int ops )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if(Is_Dynamic_Code()) {
    fprintf(stderr,
     "### Error: call to routine %s is not supported yet for dynamic extensions\n",
     "Hazard_Post_Ops");
    exit(EXIT_FAILURE);
   }

  current_haz_desc->post_ops = ops;
}


/////////////////////////////////////
void Hazard_Pre_Ops( int ops )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if(Is_Dynamic_Code()) {
    fprintf(stderr,
     "### Error: call to routine %s is not supported yet for dynamic extensions\n",
     "Hazard_Post_Ops");
    exit(EXIT_FAILURE);
   }

  current_haz_desc->pre_ops = ops;
}


/////////////////////////////////////
void Hazard_ISA( ISA_SUBSET isa )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if(Is_Dynamic_Code()) {
    fprintf(stderr,
     "### Error: call to routine %s is not supported yet for dynamic extensions\n",
     "Hazard_ISA");
    exit(EXIT_FAILURE);
   }

  if ((unsigned)isa > (unsigned)LOCAL_ISA_SUBSET_MAX) {
    fprintf(stderr, "### Error: isa value (%d) out of static isa subsets range (%d..%d)\n",
	(int)isa, ISA_SUBSET_MIN, LOCAL_ISA_SUBSET_MAX);
    exit(EXIT_FAILURE);
  }

  current_haz_desc->subsets[(int)isa] = true;
}


/////////////////////////////////////
void ISA_Hazards_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  int top;
  bool first;
  std::list<ISA_HAZARD>::iterator isi;
  std::list<op_haz *>::iterator ophaz_iter;
  const char *isa_hazard_info_format = 
	"  { ISA_HAZARD_%-9s, %d, %d, %2d, 0x%02x, %d }, /* %2d */\n";

  // Whether we generate code for an extension
  // or for the core.
  bool        gen_static_code = Is_Static_Code();
  bool        gen_dyn_code    =!gen_static_code;

  const char* const extname  = gen_static_code ? NULL: Get_Extension_Name();
  const char* const bname    = FNAME_TARG_ISA_HAZARDS;

  const char* const name_hazards_h = Gen_Build_Filename (bname,
                                                         extname,
                                                         gen_util_file_type_hfile);
  const char* const name_hazards_c = Gen_Build_Filename (bname,
                                                         extname,
                                                         gen_util_file_type_cfile);
  const char* const name_hazards_ex= Gen_Build_Filename(bname,
                                                        extname,
                                                        gen_util_file_type_efile);

  FILE* hfile = Gen_Open_File_Handle(name_hazards_h ,"w");
  FILE* cfile = Gen_Open_File_Handle(name_hazards_c ,"w");
  FILE* efile = gen_static_code ? Gen_Open_File_Handle(name_hazards_ex,"w") : NULL;

  const char* tabname;
  bool  has_one_item;

  // Include files
  if(gen_dyn_code) {
    fprintf(cfile,"#include <stdio.h>\n");
  }
  fprintf(cfile,"#include \"topcode.h\"\n");
  fprintf(cfile,"#include \"targ_isa_subset.h\"\n");

  if(gen_dyn_code) {
    char *tmp = Gen_Build_Filename(FNAME_ISA_HAZARDS,NULL,
                                   gen_util_file_type_dyn_hfile);
    fprintf(cfile,"#include \"%s\"\n",tmp);
    Gen_Free_Filename(tmp);
   }
  else {
    fprintf(cfile,"#include \"%s\"\n",name_hazards_h);
  }
  fprintf(cfile,"\n\n");

  Emit_Header (hfile, bname, interface,extname);
  fprintf(hfile,"#include \"targ_isa_subset.h\"\n");

  if(gen_static_code) {
    fprintf(hfile,"typedef enum {");
    first = true;
    for ( isi = hazards.begin(); isi != hazards.end(); ++isi ) {
      ISA_HAZARD hazard = *isi;
      fprintf(hfile,"%c\n  ISA_HAZARD_%s",first ? ' ' : ',',
                                          hazard->name);
      first = false;
    }
    fprintf(hfile,",\n  ISA_HAZARD_UNDEFINED");
    fprintf(hfile,"\n} ISA_HAZARD;\n");

    fprintf(hfile, "\ntypedef struct {\n"
                   "  ISA_HAZARD type;\n"
                   "  mUINT16 data;\n"
                   "  mUINT16 pre_ops;\n"
                   "  mUINT16 post_ops;\n"
                   "  mUINT32 isa_mask;\n"
                   "  mUINT8 next;\n"
                   "} ISA_HAZARD_INFO;\n"
                   "\n");

    fprintf(efile, "ISA_HAZARD_hazard_info\n");
  }

  tabname = gen_static_code ? "ISA_HAZARD_hazard_static_info" :
                              "ISA_HAZARD_hazard_dyn_info";

  // Determine whether the following table is empty or not.
  // By construction, the static case can't be empty (because of UNDEFINED entry).
  has_one_item = gen_static_code ? true : op_hazards_list.begin()!=op_hazards_list.end();
                 
  if(has_one_item) {
    fprintf(cfile, 
            "\nstatic ISA_HAZARD_INFO %s[%d] = {\n", 
            tabname,haz_index + 1);
  }
  else {
    fprintf(cfile, 
            "static const ISA_HAZARD_INFO * %s = NULL;\n",
            tabname);
  }

  // First item in the static case.
  if(gen_static_code)
    fprintf(cfile, isa_hazard_info_format,
            "UNDEFINED", 0, 0, 0, 0, 0, 0);

  for ( ophaz_iter = op_hazards_list.begin();
	ophaz_iter != op_hazards_list.end();
	++ophaz_iter
  ) {
    UINT32 mask;
    ISA_SUBSET subset;
    op_haz *op_hazard = *ophaz_iter;
    haz_desc *haz = op_hazard->desc;
    op_haz *next = op_hazard->next;

    mask = 0;
    for (subset = ISA_SUBSET_MIN;
	 subset <= LOCAL_ISA_SUBSET_MAX; 
	 subset = (ISA_SUBSET)((int)subset + 1)
    ) {
      if ( haz->subsets[(int)subset] ) mask |= (UINT32)1 << (int)subset;
    }

    fprintf(cfile, isa_hazard_info_format,
	    haz->type->name,
	    haz->data,
	    haz->pre_ops,
	    haz->post_ops,
	    mask,
	    next ? next->index : 0,
	    op_hazard->index);
  }

  if(has_one_item)         // End of table (if we don't have a NULL pointer).
    fprintf(cfile, "};\n");

  if(gen_static_code) {
    fprintf(cfile,
           "BE_EXPORTED ISA_HAZARD_INFO * ISA_HAZARD_hazard_info = %s;\n"
           "\n",
           tabname);
    fprintf(hfile,
           "BE_EXPORTED extern ISA_HAZARD_INFO * ISA_HAZARD_hazard_info;\n"
           "\n");
   }
  else {
   // Nothing done yet in the dynamic case.
   // To be done in forthcoming releases.
  }

  tabname = gen_static_code ? "ISA_HAZARD_hazard_static_index" :
                              "ISA_HAZARD_hazard_dyn_index";

  fprintf(cfile, "\nstatic mUINT8 %s[%d] = {\n", 
                 tabname,
                 TOP_count_limit);
  for ( top = 0; top < TOP_count_limit; ++top ) {
    op_haz *op_hazard = op_hazards[top];
    fprintf(cfile, "  %3d, ", op_hazard ? op_hazard->index : 0);
    fprintf(cfile, "/* %-20s */\n", TOP_Name((TOP)top));
  }
  fprintf(cfile, "};\n");

  if(gen_static_code) {
    fprintf(efile, "ISA_HAZARD_hazard_index\n");
    fprintf(cfile, 
            "BE_EXPORTED mUINT8 *ISA_HAZARD_hazard_index = %s;\n\n",
            tabname);
    fprintf(hfile, 
            "BE_EXPORTED extern mUINT8 *ISA_HAZARD_hazard_index;\n\n");
  }
  else {
    // Dynamic case: export the table.
    fprintf(cfile,
       "const mUINT8 * dyn_get_ISA_HAZARDS_index_tab( void ) {\n"
       "    return %s;\n"
       "}\n",
       tabname);
    fprintf(hfile,
       "extern const mUINT8 * dyn_get_ISA_HAZARDS_index_tab ( void );\n");
  }
 
 
  //////////////////////////////////////////////////////////////////////////
  // The following code makes it possible to handle hazard data in
  // compiler.
  //////////////////////////////////////////////////////////////////////////

  if(gen_static_code) {
    fprintf(hfile,
      "\ninline BOOL ISA_HAZARD_TOP_Has_Hazard(TOP topcode)\n"
      "{\n"
      "  return ISA_HAZARD_hazard_index[(INT)topcode] != 0;\n"
      "}\n");

  fprintf(hfile, 
      "\ninline ISA_HAZARD_INFO *ISA_HAZARD_First(TOP topcode)\n"
      "{\n"
      "  INT index = ISA_HAZARD_hazard_index[(INT)topcode];\n"
      "  return index ? &ISA_HAZARD_hazard_info[index] : (ISA_HAZARD_INFO *)0;\n"
      "}\n");

  fprintf(hfile, 
     "\ninline ISA_HAZARD_INFO *ISA_HAZARD_Next(ISA_HAZARD_INFO *info)\n"
     "{\n"
     "  INT index = info->next;\n"
     "  return index ? &ISA_HAZARD_hazard_info[index] : (ISA_HAZARD_INFO *)0;\n"
     "}\n");

  fprintf(hfile, 
     "\ninline ISA_HAZARD ISA_HAZARD_Type(ISA_HAZARD_INFO *info)\n"
     "{\n"
     "  return info->type;\n"
     "}\n");

  fprintf(hfile, 
     "\ninline INT ISA_HAZARD_Data(ISA_HAZARD_INFO *info)\n"
     "{\n"
     "  return info->data;\n"
     "}\n");

  fprintf(hfile, 
     "\ninline INT ISA_HAZARD_Pre_Ops(ISA_HAZARD_INFO *info)\n"
     "{\n"
     "  return info->pre_ops;\n"
     "}\n");

  fprintf(hfile, 
     "\ninline INT ISA_HAZARD_Post_Ops(ISA_HAZARD_INFO *info)\n"
     "{\n"
     "  return info->post_ops;\n"
     "}\n");

  fprintf(hfile, 
     "\nBE_EXPORTED extern void ISA_HAZARD_Initialize(void);\n");

  fprintf(efile, 
     "ISA_HAZARD_Initialize\n");

  fprintf(cfile, 
     "\nvoid ISA_HAZARD_Initialize(void)\n"
     "{\n"
     "  INT top;\n"
     "  UINT32 mask = ISA_SUBSET_LIST_Mask(ISA_SUBSET_List);\n"
     "  for ( top = 0; top < TOP_count; ++top ) {\n"
     "    INT j, k;\n"
     "    INT i = ISA_HAZARD_hazard_index[top];\n"
     "    for (j = i; j != 0; j = k) {\n"
     "      for (k = ISA_HAZARD_hazard_info[j].next;\n"
     "           k != 0 && (ISA_HAZARD_hazard_info[k].isa_mask & mask) == 0;\n"
     "           k = ISA_HAZARD_hazard_info[k].next\n"
     "      );\n"
     "      ISA_HAZARD_hazard_info[j].next = k;\n"
     "    }\n"
     "    if ((ISA_HAZARD_hazard_info[i].isa_mask & mask) == 0) {\n"
     "      ISA_HAZARD_hazard_index[top] = ISA_HAZARD_hazard_info[i].next;\n"
     "    }\n"
     "  }\n"
     "}\n");
  }

  Emit_Footer (hfile);

  // Closing all file handlers
  Gen_Close_File_Handle(cfile ,name_hazards_c );
  Gen_Close_File_Handle(hfile ,name_hazards_h );
  if(efile) 
    Gen_Close_File_Handle(efile ,name_hazards_ex);

  // Memory deallocation
  Gen_Free_Filename(const_cast<char*> (name_hazards_c));
  Gen_Free_Filename(const_cast<char*> (name_hazards_h));
  Gen_Free_Filename(const_cast<char*> (name_hazards_ex));

  return;
}
#else
#include "topcode.h"
#include "gen_util.h"
#include "targ_isa_subset.h"
#include "isa_hazards_gen.h"


struct isa_hazard {
  const char *name;         // hazard name
};

struct haz_desc {
  isa_hazard *type;
  int data;
  int pre_ops;
  int post_ops;
  int subsets[ISA_SUBSET_MAX+1];
};

struct op_haz {
  haz_desc *desc;
  struct op_haz *next;
  int index;
};

static std::list<ISA_HAZARD> hazards;    // All the hazards
static op_haz *op_hazards[TOP_count+1];
static std::list<op_haz *> op_hazards_list;
static haz_desc *current_haz_desc;
static int haz_index;

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   A description of the ISA hazards. The description exports",
  " *   the following:",
  " *",
  " *   typedef (enum) ISA_HAZARD",
  " *       An enumeration of the hazard types, and ISA_HAZARD_UNDEFINED.",
  " *",
  " *   typedef (struct) ISA_HAZARD_INFO",
  " *       Describes a particular hazard. The contents are private.",
  " *",
  " *   BOOL ISA_HAZARD_TOP_Has_Hazard(TOP topcode)",
  " *       Returns TRUE if the instruction specified by 'topcode'",
  " *       has a hazard.",
  " *",
  " *   ISA_HAZARD_INFO *ISA_HAZARD_First(TOP topcode)",
  " *       Get the first hazard description for 'topcode'.",
  " *",
  " *   ISA_HAZARD_INFO *ISA_HAZARD_Next(ISA_HAZARD_INFO *info)",
  " *       Gets the next hazard description when a 'topcode' has",
  " *       more than one hazard.",
  " *",
  " *   ISA_HAZARD ISA_HAZARD_Type(ISA_HAZARD_INFO *info)",
  " *       Returns the type of the hazard.",
  " *",
  " *   INT ISA_HAZARD_Data(ISA_HAZARD_INFO *info)",
  " *       Returns the hazard specific data.",
  " *",
  " *   INT ISA_HAZARD_Pre_Ops(ISA_HAZARD_INFO *info)",
  " *       Returns the number of OPs that must precede the instruction",
  " *       with the hazard.",
  " *",
  " *   INT ISA_HAZARD_Post_Ops(ISA_HAZARD_INFO *info)",
  " *       Returns the number of OPs that must follow the instruction",
  " *       with the hazard.",
  " *",
  " *   void ISA_HAZARD_Initialize(void)",
  " *       Initializes the hazard description data for ISA_SUBSET_Value."
  " *       This may only be called once (if not called at all the description",
  " *       contains the hazards for all ISAs).",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};


/////////////////////////////////////
void ISA_Hazards_Begin( const char* /* name */ )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
}

/////////////////////////////////////
ISA_HAZARD Hazard_Create( const char *name )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  ISA_HAZARD result = new isa_hazard;
  bzero(result, sizeof(isa_hazard));
  hazards.push_back(result);
  result->name = name;
  return result;
}

/////////////////////////////////////
void Hazard_Group( TOP topcode, ... )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  va_list ap;
  TOP opcode;
  int count = 0;

  current_haz_desc = new haz_desc;
  bzero(current_haz_desc, sizeof(haz_desc));

  va_start(ap,topcode);
  for (opcode = topcode;
       opcode != TOP_UNDEFINED;
       opcode = static_cast<TOP>(va_arg(ap,int))) {
    op_haz *op_hazard = new op_haz;
    op_hazards_list.push_back(op_hazard);
    op_hazard->desc = current_haz_desc;
    op_hazard->next = op_hazards[(int)opcode];
    op_hazard->index = ++haz_index;
    op_hazards[(int)opcode] = op_hazard;
    ++count;
  }
  va_end(ap);

  if (count == 0) {
    fprintf(stderr, "### Warning: hazard group is empty\n");
  }
}


/////////////////////////////////////
void Hazard_Type( ISA_HAZARD isa_hazard )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  current_haz_desc->type = isa_hazard;
}


/////////////////////////////////////
void Hazard_Data( int data )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  current_haz_desc->data = data;
}


/////////////////////////////////////
void Hazard_Post_Ops( int ops )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  current_haz_desc->post_ops = ops;
}


/////////////////////////////////////
void Hazard_Pre_Ops( int ops )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  current_haz_desc->pre_ops = ops;
}


/////////////////////////////////////
void Hazard_ISA( ISA_SUBSET isa )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if ((unsigned)isa > (unsigned)ISA_SUBSET_MAX) {
    fprintf(stderr, "### Error: isa value (%d) out of range (%d..%d)\n",
	(int)isa, ISA_SUBSET_MIN, ISA_SUBSET_MAX);
    exit(EXIT_FAILURE);
  }

  current_haz_desc->subsets[(int)isa] = true;
}


/////////////////////////////////////
void ISA_Hazards_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  int top;
  bool first;
  std::list<ISA_HAZARD>::iterator isi;
  std::list<op_haz *>::iterator ophaz_iter;
  const char * const isa_hazard_info_format = 
	"  { ISA_HAZARD_%-9s, %d, %d, %2d, 0x%02x, %d }, /* %2d */\n";

#define FNAME "targ_isa_hazards"
  char filename[1000];
  sprintf(filename,"%s.h",FNAME);
  FILE* hfile = fopen(filename,"w");
  sprintf(filename,"%s.c",FNAME);
  FILE* cfile = fopen(filename,"w");
  sprintf(filename,"%s.Exported",FNAME);
  FILE* efile = fopen(filename,"w");

  fprintf(cfile,"#include \"topcode.h\"\n");
  fprintf(cfile,"#include \"targ_isa_subset.h\"\n");
  fprintf(cfile,"#include \"%s.h\"\n",FNAME);

  sprintf (filename, "%s", FNAME);
  Emit_Header (hfile, filename, interface);
  fprintf(hfile,"#include \"targ_isa_subset.h\"\n");

  fprintf(hfile,"typedef enum {");
  first = true;
  for ( isi = hazards.begin(); isi != hazards.end(); ++isi ) {
    ISA_HAZARD hazard = *isi;
    fprintf(hfile,"%c\n  ISA_HAZARD_%s",first ? ' ' : ',',
                                        hazard->name);
    first = false;
  }
  fprintf(hfile,",\n  ISA_HAZARD_UNDEFINED");
  fprintf(hfile,"\n} ISA_HAZARD;\n");

  fprintf(hfile, "\ntypedef struct {\n"
  		 "  ISA_HAZARD type;\n"
  		 "  mUINT16 data;\n"
  		 "  mUINT16 pre_ops;\n"
  		 "  mUINT16 post_ops;\n"
		 "  mUINT8 isa_mask;\n"
		 "  mUINT8 next;\n"
  		 "} ISA_HAZARD_INFO;\n");

  fprintf(efile, "ISA_HAZARD_hazard_info\n");

  fprintf(cfile, "\nISA_HAZARD_INFO ISA_HAZARD_hazard_info[%d] = {\n", 
	  haz_index + 1);
  fprintf(cfile, isa_hazard_info_format,
	  "UNDEFINED", 0, 0, 0, 0, 0, 0);
  for ( ophaz_iter = op_hazards_list.begin();
	ophaz_iter != op_hazards_list.end();
	++ophaz_iter
  ) {
    int mask;
    ISA_SUBSET subset;
    op_haz *op_hazard = *ophaz_iter;
    haz_desc *haz = op_hazard->desc;
    op_haz *next = op_hazard->next;

    mask = 0;
    for (subset = ISA_SUBSET_MIN;
	 subset <= ISA_SUBSET_MAX; 
	 subset = (ISA_SUBSET)((int)subset + 1)
    ) {
      if ( haz->subsets[(int)subset] ) mask |= 1 << (int)subset;
    }

    fprintf(cfile, isa_hazard_info_format,
	    haz->type->name,
	    haz->data,
	    haz->pre_ops,
	    haz->post_ops,
	    mask,
	    next ? next->index : 0,
	    op_hazard->index);
  }
  fprintf(cfile, "};\n");

  fprintf(efile, "ISA_HAZARD_hazard_index\n");

  fprintf(cfile, "\nmUINT8 ISA_HAZARD_hazard_index[%d] = {\n", TOP_count);
  for ( top = 0; top < TOP_count; ++top ) {
    op_haz *op_hazard = op_hazards[top];
    fprintf(cfile, "  %3d, ", op_hazard ? op_hazard->index : 0);
    fprintf(cfile, "/* %-9s */\n", TOP_Name((TOP)top));
  }
  fprintf(cfile, "};\n");

  fprintf(hfile, "\ninline BOOL ISA_HAZARD_TOP_Has_Hazard(TOP topcode)\n"
		 "{\n"
		 "  extern mUINT8 ISA_HAZARD_hazard_index[%d];\n"
		 "  return ISA_HAZARD_hazard_index[(INT)topcode] != 0;\n"
		 "}\n",
		 TOP_count);

  fprintf(hfile, "\ninline ISA_HAZARD_INFO *ISA_HAZARD_First(TOP topcode)\n"
		 "{\n"
		 "  extern mUINT8 ISA_HAZARD_hazard_index[%d];\n"
		 "  extern ISA_HAZARD_INFO ISA_HAZARD_hazard_info[%d];\n"
		 "  INT index = ISA_HAZARD_hazard_index[(INT)topcode];\n"
		 "  return index ? ISA_HAZARD_hazard_info + index : (ISA_HAZARD_INFO *)0;\n"
		 "}\n",
		 TOP_count,
		 haz_index + 1);

  fprintf(hfile, "\ninline ISA_HAZARD_INFO *ISA_HAZARD_Next(ISA_HAZARD_INFO *info)\n"
		 "{\n"
		 "  extern ISA_HAZARD_INFO ISA_HAZARD_hazard_info[%d];\n"
		 "  INT index = info->next;\n"
		 "  return index ? ISA_HAZARD_hazard_info + index : (ISA_HAZARD_INFO *)0;\n"
		 "}\n",
		 haz_index + 1);

  fprintf(hfile, "\ninline ISA_HAZARD ISA_HAZARD_Type(ISA_HAZARD_INFO *info)\n"
		 "{\n"
		 "  return info->type;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT ISA_HAZARD_Data(ISA_HAZARD_INFO *info)\n"
		 "{\n"
		 "  return info->data;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT ISA_HAZARD_Pre_Ops(ISA_HAZARD_INFO *info)\n"
		 "{\n"
		 "  return info->pre_ops;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT ISA_HAZARD_Post_Ops(ISA_HAZARD_INFO *info)\n"
		 "{\n"
		 "  return info->post_ops;\n"
		 "}\n");

  fprintf(hfile, "\nextern void ISA_HAZARD_Initialize(void);\n");

  fprintf(efile, "ISA_HAZARD_Initialize\n");

  fprintf(cfile, "\nvoid ISA_HAZARD_Initialize(void)\n"
		 "{\n"
		 "  INT top;\n"
		 "  INT mask = 1 << (INT)ISA_SUBSET_Value;\n"
		 "  for ( top = 0; top < TOP_count; ++top ) {\n"
		 "    INT j, k;\n"
		 "    INT i = ISA_HAZARD_hazard_index[top];\n"
		 "    for (j = i; j != 0; j = k) {\n"
		 "      for (k = ISA_HAZARD_hazard_info[j].next;\n"
		 "           k != 0 && (ISA_HAZARD_hazard_info[k].isa_mask & mask) == 0;\n"
		 "           k = ISA_HAZARD_hazard_info[k].next\n"
		 "      );\n"
		 "      ISA_HAZARD_hazard_info[j].next = k;\n"
		 "    }\n"
		 "    if ((ISA_HAZARD_hazard_info[i].isa_mask & mask) == 0) {\n"
		 "      ISA_HAZARD_hazard_index[top] = ISA_HAZARD_hazard_info[i].next;\n"
		 "    }\n"
		 "  }\n"
		 "}\n");

  Emit_Footer (hfile);
}
#endif
