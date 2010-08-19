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


// isa_decode_gen.cxx
/////////////////////////////////////
//
//  Generate an interface for decoding instructions.
//
/////////////////////////////////////
//
//  $Revision: 1.5 $
//  $Date: 04/12/21 14:57:26-08:00 $
//  $Author: bos@eng-25.internal.keyresearch.com $
//  $Source: /home/bos/bk/kpro64-pending/common/targ_info/generate/SCCS/s.isa_decode_gen.cxx $

#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <list>
#include "topcode.h"

#ifdef TARG_ST
// In following loops, we iterate on the number of
// TOP. This number differs following we generate
// static or dynamic TOP.
#ifdef DYNAMIC_CODE_GEN
#include "dyn_isa_topcode.h"
static const TOP TOP_count_limit = TOP_dyn_count;
#else
#include "topcode.h"
static const TOP TOP_count_limit = TOP_static_count;
#endif
#ifdef DYNAMIC_CODE_GEN
#include "dyn_isa_subset.h"
#else
#include "targ_isa_subset.h"
#endif
#include "gen_util.h"
#include "isa_decode_gen.h"

typedef enum { INST_STATE, UNIT_STATE } STATE_TYPE;

typedef enum {
  VISIT_UNVISITED,
  VISIT_GEN_DATA,
  VISIT_GEN_CODE
} VISIT_KIND;

/* This struct is used to hold the information that describes
 * a state. The information is dependent on whether or not the
 * state is a final state or not.
 *
 * Final states are described completely by a topcode.
 *
 * Intermediate states, have one or more transitions to new states,
 * and a bitfield description of an instruction being decoded,
 * that determines which transition is taken.
 */
struct state {
  bool is_final;
  VISIT_KIND visit;
  union {
    TOP final;		// final state
    struct {		// intermediate state
      const char *tag;
      int idx;
      int pos;
      int width;
      STATE *transition;
      STATE_TYPE stype;
      int casenum;
    } i;
  } u;
};

struct decodeentry {
  int codingwidth;
  STATE initstate;
};
typedef struct decodeentry * DECODEENTRY;
static std::list <DECODEENTRY> all_decodeentries[ISA_SUBSET_MIN+ISA_SUBSET_COUNT_MAX];

static STATE initial_state;
static std::list <STATE> all_states;

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   Utilities for decoding binary instructions. The following",
  " *   are exported:",
  " *",
  " *    typedef (struct) ISA_DECODE_INIT_STATE",
  " *        Contains couples of encoding width and initial decoding",
  " *        unit."
  " *",
  " *    typedef (function) ISA_DECODE_INST",
  " *        Pointer to function decoding instruction for a particular",
  " *        subset.",
  " *",
  " *    typedef (struct) ISA_DECODE_DECODING",
  " *        Contains for a subset the pointer to decoding function ",
  " *        and and array of ISA_DECODE_INIT_STATE.",
  " *",
  " *    const ISA_DECODE_DECODING * ISA_DECODE_Decoding(ISA_SUBSET)",
  " *        Returns ISA_DECODE_DECODING of subset.",
  " *",
  " *    const ISA_DECODE_INIT_STATE * ISA_DECODE_Init_States(ISA_DECODE_DECODING *)",
  " *        Returns ISA_DECODE_INIT_STATE array of a ISA_DECODE_DECODING.",
  " *",
  " *    const ISA_DECODE_INST ISA_DECODE_Decoding_Fct(ISA_DECODE_DECODING *)",
  " *        Returns ISA_DECODE_INST of ISA_DECODE_DECODING.",
  " *",
  " *    INT ISA_DECODE_INIT_STATE_Width(ISA_DECODE_INIT_STATE *)",
  " *        Returns width of ISA_DECODE_INIT_STATE.",
  " *",
  " *    INT ISA_DECODE_INIT_STATE_Unit(ISA_DECODE_INIT_STATE *)",
  " *        Returns decoding unit of ISA_DECODE_INIT_STATE",
  " *",
  " *   INT ISA_DECODE_Decode_Unit(ISA_SUBSET subset, INT width)",
  " *       Find out the decoding unit associated to <subset> for a",
  " *       particular encoding <width>.",
  " *",
  " *   TOP ISA_Decode_Inst(const ISA_PACK_INST *pinst, INT unit)",
  " *       Decode the instruction pointed to by <pinst> in decoding unit",
  " *       <unit> and return its opcode by function return value.",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};


/////////////////////////////////////
void ISA_Decode_Begin(const char * /* name */)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
}

/////////////////////////////////////
STATE Create_Unit_State(const char *tag, int pos, int width)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  int i;
  STATE state = new struct state;
  state->is_final = false;
  state->visit = VISIT_UNVISITED;
  state->u.i.tag = tag;
  state->u.i.idx = 0;
  state->u.i.pos = pos;
  state->u.i.width = width;  
  state->u.i.transition = new STATE[1 << width];
  for (i = 0; i < (1 << width); ++i) state->u.i.transition[i] = NULL;
  state->u.i.stype = UNIT_STATE;
  all_states.push_back(state);
  return state;
}

/////////////////////////////////////
STATE Create_Inst_State(const char *tag, int idx, int pos, int width)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  int i;
  STATE state = new struct state;
  state->is_final = false;
  state->visit = VISIT_UNVISITED;
  state->u.i.tag = tag;
  state->u.i.idx = idx;
  state->u.i.pos = pos;
  state->u.i.width = width;  
  state->u.i.transition = new STATE[1 << width];
  for (i = 0; i < (1 << width); ++i) state->u.i.transition[i] = NULL;
  state->u.i.stype = INST_STATE;
  all_states.push_back(state);
  return state;
}

/////////////////////////////////////
void Transitions(STATE state, ...)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  int n;
  va_list ap;
  STATE transition;

  if (state->is_final) {
    fprintf(stderr, "### Error: can't specify transistions for a final state\n");
    exit(EXIT_FAILURE);
  }

  va_start(ap, state);
  while ((n = va_arg(ap, int)) != END_TRANSITIONS) {
    if (n < 0 || n >= (1 << state->u.i.width)) {
      fprintf(stderr, "### Error: transition %d of %s is out-of-range\n",
		      n, state->u.i.tag);
      exit(EXIT_FAILURE);
    }
    if (state->u.i.transition[n]) {
      fprintf(stderr, "### Error: transition %d of %s multiply specified\n",
		      n, state->u.i.tag);
      exit(EXIT_FAILURE);
    }
    transition = va_arg(ap, STATE);
    if ((unsigned long)transition < TOP_count_limit) {
      fprintf(stderr, "### Error: transition %d of %s looks like it should be Final()\n",
		      n, state->u.i.tag);
      exit(EXIT_FAILURE);
    }
    state->u.i.transition[n] = transition;
  }
  va_end(ap);
}

/////////////////////////////////////
void Initial_State(STATE state)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  initial_state = state;
}

/////////////////////////////////////
STATE Final(TOP topcode)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  STATE state = new struct state;
  state->is_final = true;
  state->u.final = topcode;
  all_states.push_back(state);
  return state;
}

/////////////////////////////////////
static int Compare_NonFinals(const void *p1, const void *p2)
/////////////////////////////////////
//  
/////////////////////////////////////
{
  STATE s1 = *(STATE *)p1;
  STATE s2 = *(STATE *)p2;

  return strcmp(s1->u.i.tag, s2->u.i.tag);
}

/////////////////////////////////////
static void Visit_State(STATE state, FILE *f, VISIT_KIND vk)
/////////////////////////////////////
//  Visit <state> and the states it leads to and perform the
//  actions necessary for the kind of visitation specified by <vk>.
/////////////////////////////////////
{
  static int indent;
  int i;
  int ntrans;
  int max_top;
  STATE *nonfinals;
  int n_nonfinals;

  assert(!state->is_final);

  /* If we've been here, then for data, just return so we
   * don't generate the same data twice. For code, we emit
   * the whole thing again. Alternatively we could have
   * generated a label on the first state and then just
   * goto it here.
   */
  if (state->visit == vk && vk == VISIT_GEN_DATA) return;
  state->visit = vk;

  indent += 2;
  ntrans = 1 << state->u.i.width;

  nonfinals = (STATE *)malloc(sizeof(struct state) * ntrans);
  n_nonfinals = 0;
  for (i = 0; i < ntrans; ++i) {
    STATE newstate = state->u.i.transition[i];
    if (newstate) {
      if (!newstate->is_final) {
	nonfinals[n_nonfinals++] = newstate;
      } else {
	newstate->visit = vk;  // only used for unused state detection
      }
    }
  }
  qsort(nonfinals, n_nonfinals, sizeof(STATE *), Compare_NonFinals);

  if (vk == VISIT_GEN_CODE) {
    fprintf(f, "%*sopc = (", indent, "");
    if (state->u.i.stype == INST_STATE) {
      fprintf(f, "pinst[%d]", state->u.i.idx);
    } else {
      fprintf(f, "unit");
    }
    fprintf(f, " >> %d) & 0x%x;\n", state->u.i.pos, ntrans - 1);

    fprintf(f, "%*stop = state_%s_tab[opc];\n", indent, "", state->u.i.tag);
    if (n_nonfinals) fprintf(f, "%*sswitch (top) {\n", indent, "");
  }

  max_top = TOP_count_limit/*TOP_UNDEFINED*/;
  for (i = 0; i < n_nonfinals; ++i) {
    STATE newstate = nonfinals[i];
    ++max_top;
    newstate->u.i.casenum = max_top;
    if (vk == VISIT_GEN_CODE) {
      fprintf(f, "%*scase %3d: /* %s */\n",
		 indent, "",
		 max_top,
		 newstate->u.i.tag);
    } 
    if (   i + 1 == n_nonfinals 
	|| strcmp(newstate->u.i.tag, nonfinals[i+1]->u.i.tag))
    {
      Visit_State(newstate, f, vk);
      if (vk == VISIT_GEN_CODE) fprintf(f, "%*sbreak;\n", indent + 2, "");
    }
  }

  if (vk == VISIT_GEN_DATA) {
    int col;
    const char *top_type;
    if (max_top < 256) {
      top_type = "mUINT8";
    } else if (max_top < 65536) {
      top_type = "mUINT16";
    } else {
      top_type = "mUINT32";
    }
    fprintf(f, "\nstatic const %s state_%s_tab[%d] = {",
	       top_type,
	       state->u.i.tag, 
	       ntrans);
    col = 8;
    for (i = 0; i < ntrans; ++i) {
      STATE newstate = state->u.i.transition[i];
      if (col == 8) {
	fprintf(f, "\n ");
	col = 0;
      }
      if (newstate == NULL) {
	fprintf(f, " %4d,", TOP_count_limit/*TOP_UNDEFINED*/);
      } else if (newstate->is_final) {
	fprintf(f, " %4d,", newstate->u.final);
      } else {
	fprintf(f, " %4d,", newstate->u.i.casenum);
      }

      ++col;
    }

    fprintf(f, "\n};\n");
  } else if (vk == VISIT_GEN_CODE) {
    if (n_nonfinals) fprintf(f, "%*s}\n", indent, "");
  }
  indent -= 2;
  free(nonfinals);
}

/////////////////////////////////////
void DecodeEntry(ISA_SUBSET subset, ...)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  int codingwidth;
  va_list ap;
  STATE initstate;

  va_start(ap, subset);
  while ((codingwidth = va_arg(ap, int)) != END_ENTRIES) {
    DECODEENTRY entry;
    
    initstate = va_arg(ap, STATE);
    entry = new struct decodeentry;
    entry->codingwidth = codingwidth;
    entry->initstate = initstate;
    all_decodeentries[subset].push_back(entry);
  }
  va_end(ap);
}

/////////////////////////////////////
void ISA_Decode_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  // Whether we generate code for an extension
  // or for the core.
  bool        gen_static_code = Is_Static_Code();

  const char* const extname  = gen_static_code ? NULL: Get_Extension_Name();

  std::list <STATE>::iterator state_iter;
  std::list <DECODEENTRY>::iterator entry_iter;

  ISA_SUBSET subset;

  char  *hfilename;
  char  *cfilename;
  char  *efilename = NULL;
  char  *sfilename = NULL;
  FILE  *hfile;
  FILE  *cfile;
  FILE  *efile = NULL;
  FILE  *sfile = NULL;
  char  *htopcodename;           // topcode.h
  char  *hbundlename;
  char  *hpackname;
  char  *hsubsetname;
  const char  *dynname;
  
  hfilename = Gen_Build_Filename(FNAME_TARG_ISA_DECODE,extname,
                                 gen_util_file_type_hfile);
  hfile     = Gen_Open_File_Handle(hfilename,"w");

  // C file
  cfilename = Gen_Build_Filename(FNAME_TARG_ISA_DECODE,extname,
                                 gen_util_file_type_cfile);
  cfile     = Gen_Open_File_Handle(cfilename,"w");

  // Export file
  if(gen_static_code) 
   { efilename = Gen_Build_Filename(FNAME_TARG_ISA_DECODE,extname,
                                    gen_util_file_type_efile);
     efile     = Gen_Open_File_Handle(efilename,"w");
   } else {
     sfilename = Gen_Build_Filename(FNAME_STUB_ISA_DECODE,extname,
                                    gen_util_file_type_cfile);
     sfile     = Gen_Open_File_Handle(sfilename,"w");
   }

  htopcodename = Gen_Build_Filename(FNAME_TOPCODE,extname,
                                    gen_util_file_type_hfile);
  hbundlename  = Gen_Build_Filename(FNAME_TARG_ISA_BUNDLE,extname,
                                    gen_util_file_type_hfile);
  hpackname    = Gen_Build_Filename(FNAME_TARG_ISA_PACK,extname,
                                    gen_util_file_type_hfile);
  hsubsetname  = Gen_Build_Filename(FNAME_TARG_ISA_SUBSET,extname,
                                    gen_util_file_type_hfile);
  
  fprintf(cfile, 
    "#include \"%s\"\n"
    "#include \"%s\"\n"
    "#include \"%s\"\n"
    "#include \"%s\"\n"
    "#include \"%s\"\n\n",
     gen_static_code ? htopcodename : "dyn_isa_topcode.h",
     gen_static_code ? hbundlename : "dyn_isa_bundle.h",
     gen_static_code ? hpackname : "dyn_isa_pack.h",
     gen_static_code ? hsubsetname : "dyn_isa_subset.h",
     gen_static_code ? hfilename : "dyn_isa_decode.h"); 

  Emit_Header (hfile, FNAME_TARG_ISA_DECODE, interface, extname);
  fprintf(hfile, 
    "#include \"%s\"\n"
    "#include \"%s\"\n"
    "#include \"%s\"\n"
    "#include \"%s\"\n",
    htopcodename,
    hbundlename,
    hpackname,
    hsubsetname);

  if (!gen_static_code) {
    const char *headers[] = {
      "\"dyn_" FNAME_ISA_DECODE ".h\"",
      "",
    };

    Emit_Stub_Header(sfile,headers);
  }
  
  if (initial_state == NULL) {
    fprintf(stderr, "### Error: no initial decode state specified\n");
    exit(EXIT_FAILURE);
  }

  Visit_State(initial_state, cfile, VISIT_GEN_DATA);

  if(gen_static_code) {
    fprintf(efile, "ISA_Decode_Inst\n");
    fprintf(efile, "ISA_DECODE_Decode_Unit\n");
    fprintf(efile, "ISA_Decode_Decoding\n");
  
    fprintf(hfile, "\ntypedef struct { \n");
    fprintf(hfile, "   INT width;\n");
    fprintf(hfile, "   INT unit;\n");
    fprintf(hfile, "} ISA_DECODE_INIT_STATE;\n");
  
    fprintf(hfile, "\ntypedef TOP (* ISA_DECODE_INST)(const ISA_PACK_INST *pinst, INT unit);\n");

    fprintf(hfile, "\ntypedef struct { \n");
    fprintf(hfile, "   const ISA_DECODE_INST decodingfct;\n");
    fprintf(hfile, "   const ISA_DECODE_INIT_STATE * initstates;\n");
    fprintf(hfile, "} ISA_DECODE_DECODING;\n");

    fprintf(hfile, "\nTARGINFO_EXPORTED extern const ISA_DECODE_DECODING ** ISA_Decode_Decoding;\n");

    fprintf(hfile, "\ninline const ISA_DECODE_DECODING * ISA_DECODE_Decoding(ISA_SUBSET subset)\n");
    fprintf(hfile, "{\n");
    fprintf(hfile, "  return ISA_Decode_Decoding[subset];\n");
    fprintf(hfile, "}\n");

    fprintf(hfile, "\ninline const ISA_DECODE_INIT_STATE * ISA_DECODE_Init_States(const ISA_DECODE_DECODING * decoding)\n");
    fprintf(hfile, "{\n");
    fprintf(hfile, "  return decoding->initstates;\n");
    fprintf(hfile, "}\n");

    fprintf(hfile, "\ninline const ISA_DECODE_INST ISA_DECODE_Decoding_Fct(const ISA_DECODE_DECODING * decoding)\n");
    fprintf(hfile, "{\n");
    fprintf(hfile, "  return decoding->decodingfct;\n");
    fprintf(hfile, "}\n");

    fprintf(hfile, "\ninline INT ISA_DECODE_INIT_STATE_Width(const ISA_DECODE_INIT_STATE * initstate)\n");
    fprintf(hfile, "{\n");
    fprintf(hfile, "  return initstate->width;\n");
    fprintf(hfile, "}\n");

    fprintf(hfile, "\ninline INT ISA_DECODE_INIT_STATE_Unit(const ISA_DECODE_INIT_STATE * initstate)\n");
    fprintf(hfile, "{\n");
    fprintf(hfile, "  return initstate->unit;\n");
    fprintf(hfile, "}\n");

    fprintf(hfile, "\nTARGINFO_EXPORTED extern INT ISA_DECODE_Decode_Unit(ISA_SUBSET subset, INT width);\n");

    fprintf(cfile, "\nTARGINFO_EXPORTED INT ISA_DECODE_Decode_Unit(ISA_SUBSET subset, INT width)\n");
    fprintf(cfile, "{\n");
    fprintf(cfile, "  INT i=0;\n");
    fprintf(cfile, "  const ISA_DECODE_INIT_STATE * initstates = ISA_DECODE_Init_States(ISA_DECODE_Decoding(subset));\n\n");
    fprintf(cfile, "  while (ISA_DECODE_INIT_STATE_Unit(&initstates[i]) != -1)\n");
    fprintf(cfile, "  {\n");
    fprintf(cfile, "    if (ISA_DECODE_INIT_STATE_Width(&initstates[i]) == width)\n");
    fprintf(cfile, "    {\n");
    fprintf(cfile, "      return ISA_DECODE_INIT_STATE_Unit(&initstates[i]);\n");
    fprintf(cfile, "    }\n");
    fprintf(cfile, "    i++;\n");
    fprintf(cfile, "  }\n");
    fprintf(cfile, "  return -1;\n");
    fprintf(cfile, "}\n");
  } 

  dynname = gen_static_code ? "ISA_Decode_Inst_static" : "ISA_Decode_Inst_dynamic";
  
  fprintf(cfile, "\nstatic TOP %s(const ISA_PACK_INST *pinst, INT unit) \n"
		 "{\n"
		 "  INT top;\n"
		 "  INT opc;\n",dynname);
  Visit_State(initial_state, cfile, VISIT_GEN_CODE);
  if(gen_static_code){  
    fprintf(cfile, "  return (top==TOP_static_count)?TOP_UNDEFINED:(TOP)top;\n");
    fprintf(cfile, "}\n");
  } else {
    fprintf(cfile, "  return (top==TOP_dyn_count)?%s_CORE_TOP_UNDEFINED:(TOP)(dynamic_offset_%s+top);\n",extname,extname);
    fprintf(cfile, "}\n");
  }

  if(gen_static_code) {
    fprintf(hfile, "\nTARGINFO_EXPORTED extern TOP ISA_Decode_Inst(const ISA_PACK_INST *pinst, INT unit);\n");

    fprintf(cfile, "\nTOP ISA_Decode_Inst (const ISA_PACK_INST *pinst, INT unit)\n"
	  	   "{\n"
		   "  return %s(pinst,unit);\n"
		   "}\n",dynname);
  }

  for (state_iter = all_states.begin();
       state_iter != all_states.end();
       ++state_iter)
  {
    STATE state = *state_iter;
    if (state->visit == VISIT_UNVISITED) {
      if (state->is_final) {
	fprintf(stderr, "### Warning: final state \"%s\" is unused\n",
		TOP_Name(state->u.final));
      } else {
        fprintf(stderr, "### Warning: intermediate state \"%s\" is unused\n",
		state->u.i.tag);
      }
    }
  }

  for (subset = ISA_SUBSET_MIN;
       subset < ISA_SUBSET_count;
       ++subset)
  {
    fprintf(cfile, "\nstatic const ISA_DECODE_INIT_STATE ISA_Decode_Init_State_%s [] = {", ISA_SUBSET_Name(subset));
    for(entry_iter = all_decodeentries[subset].begin();
        entry_iter != all_decodeentries[subset].end();
        ++entry_iter)
    {
      DECODEENTRY entry = *entry_iter;

      for(int i=0;i<(1 << initial_state->u.i.width);i++) {
         STATE transition;
    
         transition = initial_state->u.i.transition[i];
         if (transition && transition==entry->initstate && transition->u.i.tag != NULL) {
           fprintf(cfile, "{%d,%d /* %s */},", entry->codingwidth, i, entry->initstate->u.i.tag);
         }
      }
    }
    fprintf(cfile, "{-1,-1}};\n");
    fprintf(cfile, "\nstatic const ISA_DECODE_DECODING ISA_Decode_Decoding_%s = { %s, ISA_Decode_Init_State_%s };\n", ISA_SUBSET_Name(subset), dynname, ISA_SUBSET_Name(subset));
  }

  dynname = gen_static_code ? "ISA_Decode_Decoding_static" : "ISA_Decode_Decoding_dynamic";
  
  fprintf(cfile, "\nstatic const ISA_DECODE_DECODING * %s [] = {", dynname);
  for (subset = ISA_SUBSET_MIN;
       subset < ISA_SUBSET_count;
       ++subset)
  {
    fprintf(cfile, "&ISA_Decode_Decoding_%s,", ISA_SUBSET_Name(subset));
  }
  fprintf(cfile, "};\n");

  if (gen_static_code) {
  
    fprintf(cfile, "\nconst ISA_DECODE_DECODING ** ISA_Decode_Decoding = %s;\n", dynname);
    
  } else {
  
    fprintf(sfile, "\nTARGINFO_EXPORTED const ISA_DECODE_DECODING ** ISA_Decode_Decoding = 0;\n\n");

    fprintf(sfile, 
	    "/*\n"
	    " * Exported routine\n"
	    " */\n"
	    "\nvoid\nISA_DECODE_Initialize_Stub( void )\n{\n"
            "  ISA_Decode_Decoding = dyn_get_ISA_DECODE_decoding_tab();\n"
	    "  return;\n"
	    "}\n");

    fprintf(hfile, "TARGINFO_EXPORTED const ISA_DECODE_DECODING ** dyn_get_ISA_DECODE_decoding_tab ( void );\n");

    fprintf(cfile, "TARGINFO_EXPORTED const ISA_DECODE_DECODING ** dyn_get_ISA_DECODE_decoding_tab ( void )\n");
    fprintf(cfile, "{ return %s;\n", dynname);
    fprintf(cfile, "}\n\n");
    
    fprintf(hfile, "TARGINFO_EXPORTED const mUINT32 dyn_get_ISA_DECODE_decoding_tab_sz ( void );\n");

    fprintf(cfile, "TARGINFO_EXPORTED const mUINT32 dyn_get_ISA_DECODE_decoding_tab_sz (void )\n");
    fprintf(cfile, "{ return %d;\n", ISA_SUBSET_count);
    fprintf(cfile, "}\n\n");

  }
  
  Emit_Footer (hfile);
  Emit_C_Footer(cfile);

  // Closing all file handlers.
  Gen_Close_File_Handle(cfile ,cfilename);
  Gen_Close_File_Handle(hfile ,hfilename);
  if(efile) 
    Gen_Close_File_Handle(efile,efilename);
  if(sfile) 
    Gen_Close_File_Handle(sfile,sfilename);

  // Memory deallocation
  Gen_Free_Filename(cfilename);
  Gen_Free_Filename(hfilename);
  if(efilename)
    Gen_Free_Filename(efilename);
  if(sfilename)
    Gen_Free_Filename(sfilename);
  Gen_Free_Filename(htopcodename);
  Gen_Free_Filename(hbundlename);
  Gen_Free_Filename(hpackname);
  Gen_Free_Filename(hsubsetname);
}
#else
#include "gen_util.h"
#include "isa_decode_gen.h"

typedef enum { INST_STATE, UNIT_STATE } STATE_TYPE;

typedef enum {
  VISIT_UNVISITED,
  VISIT_GEN_DATA,
  VISIT_GEN_CODE
} VISIT_KIND;

/* This struct is used to hold the information that describes
 * a state. The information is dependent on whether or not the
 * state is a final state or not.
 *
 * Final states are described completely by a topcode.
 *
 * Intermediate states, have one or more transitions to new states,
 * and a bitfield description of an instruction being decoded,
 * that determines which transition is taken.
 */
struct state {
  bool is_final;
  VISIT_KIND visit;
  union {
    TOP final;		// final state
    struct {		// intermediate state
      const char *tag;
      int idx;
      int pos;
      int width;
      STATE *transition;
      STATE_TYPE stype;
      int casenum;
    } i;
  } u;
};

static STATE initial_state;
static std::list<STATE> all_states;

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   Utilities for decoding binary instructions. The following",
  " *   are exported:",
  " *",
  " *   TOP ISA_Decode_Inst(const ISA_PACK_INST *pinst, ISA_EXEC_UNIT unit)",
  " *       Decode the instruction pointed to by <pinst> in execution unit",
  " *       <unit> and return its opcode by function return value.",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};


/////////////////////////////////////
void ISA_Decode_Begin(const char * /* name */)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
}

/////////////////////////////////////
STATE Create_Unit_State(const char *tag, int pos, int width)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  int i;
  STATE state = new struct state;
  state->is_final = false;
  state->visit = VISIT_UNVISITED;
  state->u.i.tag = tag;
  state->u.i.idx = 0;
  state->u.i.pos = pos;
  state->u.i.width = width;  
  state->u.i.transition = new STATE[1 << width];
  for (i = 0; i < (1 << width); ++i) state->u.i.transition[i] = NULL;
  state->u.i.stype = UNIT_STATE;
  all_states.push_back(state);
  return state;
}

/////////////////////////////////////
STATE Create_Inst_State(const char *tag, int idx, int pos, int width)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  int i;
  STATE state = new struct state;
  state->is_final = false;
  state->visit = VISIT_UNVISITED;
  state->u.i.tag = tag;
  state->u.i.idx = idx;
  state->u.i.pos = pos;
  state->u.i.width = width;  
  state->u.i.transition = new STATE[1 << width];
  for (i = 0; i < (1 << width); ++i) state->u.i.transition[i] = NULL;
  state->u.i.stype = INST_STATE;
  all_states.push_back(state);
  return state;
}

/////////////////////////////////////
void Transitions(STATE state, ...)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  int n;
  va_list ap;
  STATE transition;

  if (state->is_final) {
    fprintf(stderr, "### Error: can't specify transistions for a final state\n");
    exit(EXIT_FAILURE);
  }

  va_start(ap, state);
  while ((n = va_arg(ap, int)) != END_TRANSITIONS) {
    if (n < 0 || n >= (1 << state->u.i.width)) {
      fprintf(stderr, "### Error: transition %d of %s is out-of-range\n",
		      n, state->u.i.tag);
      exit(EXIT_FAILURE);
    }
    if (state->u.i.transition[n]) {
      fprintf(stderr, "### Error: transition %d of %s multiply specified\n",
		      n, state->u.i.tag);
      exit(EXIT_FAILURE);
    }
    transition = va_arg(ap, STATE);
    if ((unsigned long)transition < TOP_count) {
      fprintf(stderr, "### Error: transition %d of %s looks like it should be Final()\n",
		      n, state->u.i.tag);
      exit(EXIT_FAILURE);
    }
    state->u.i.transition[n] = transition;
  }
  va_end(ap);
}

/////////////////////////////////////
void Initial_State(STATE state)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  initial_state = state;
}

/////////////////////////////////////
STATE Final(TOP topcode)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  STATE state = new struct state;
  state->is_final = true;
  state->u.final = topcode;
  all_states.push_back(state);
  return state;
}

/////////////////////////////////////
static int Compare_NonFinals(const void *p1, const void *p2)
/////////////////////////////////////
//  
/////////////////////////////////////
{
  STATE s1 = *(STATE *)p1;
  STATE s2 = *(STATE *)p2;

  return strcmp(s1->u.i.tag, s2->u.i.tag);
}

/////////////////////////////////////
static void Visit_State(STATE state, FILE *f, VISIT_KIND vk)
/////////////////////////////////////
//  Visit <state> and the states it leads to and perform the
//  actions necessary for the kind of visitation specified by <vk>.
/////////////////////////////////////
{
  static int indent;
  int i;
  int ntrans;
  int max_top;
  STATE *nonfinals;
  int n_nonfinals;

  assert(!state->is_final);

  /* If we've been here, then for data, just return so we
   * don't generate the same data twice. For code, we emit
   * the whole thing again. Alternatively we could have
   * generated a label on the first state and then just
   * goto it here.
   */
  if (state->visit == vk && vk == VISIT_GEN_DATA) return;
  state->visit = vk;

  indent += 2;
  ntrans = 1 << state->u.i.width;

  nonfinals = (STATE *)alloca(sizeof(struct state) * ntrans);
  n_nonfinals = 0;
  for (i = 0; i < ntrans; ++i) {
    STATE newstate = state->u.i.transition[i];
    if (newstate) {
      if (!newstate->is_final) {
	nonfinals[n_nonfinals++] = newstate;
      } else {
	newstate->visit = vk;  // only used for unused state detection
      }
    }
  }
  qsort(nonfinals, n_nonfinals, sizeof(STATE *), Compare_NonFinals);

  if (vk == VISIT_GEN_CODE) {
    fprintf(f, "%*sopc = (", indent, "");
    if (state->u.i.stype == INST_STATE) {
      fprintf(f, "pinst[%d]", state->u.i.idx);
    } else {
      fprintf(f, "unit");
    }
    fprintf(f, " >> %d) & 0x%x;\n", state->u.i.pos, ntrans - 1);

    fprintf(f, "%*stop = state_%s_tab[opc];\n", indent, "", state->u.i.tag);
    if (n_nonfinals) fprintf(f, "%*sswitch (top) {\n", indent, "");
  }

  max_top = TOP_UNDEFINED;
  for (i = 0; i < n_nonfinals; ++i) {
    STATE newstate = nonfinals[i];
    ++max_top;
    newstate->u.i.casenum = max_top;
    if (vk == VISIT_GEN_CODE) {
      fprintf(f, "%*scase %3d: /* %s */\n",
		 indent, "",
		 max_top,
		 newstate->u.i.tag);
    } 
    if (   i + 1 == n_nonfinals 
	|| strcmp(newstate->u.i.tag, nonfinals[i+1]->u.i.tag))
    {
      Visit_State(newstate, f, vk);
      if (vk == VISIT_GEN_CODE) fprintf(f, "%*sbreak;\n", indent + 2, "");
    }
  }

  if (vk == VISIT_GEN_DATA) {
    int col;
    const char *top_type;
    if (max_top < 256) {
      top_type = "mUINT8";
    } else if (max_top < 65536) {
      top_type = "mUINT16";
    } else {
      top_type = "mUINT32";
    }
    fprintf(f, "\nstatic const %s state_%s_tab[%d] = {",
	       top_type,
	       state->u.i.tag, 
	       ntrans);
    col = 8;
    for (i = 0; i < ntrans; ++i) {
      STATE newstate = state->u.i.transition[i];
      if (col == 8) {
	fprintf(f, "\n ");
	col = 0;
      }
      if (newstate == NULL) {
	fprintf(f, " %4d,", TOP_UNDEFINED);
      } else if (newstate->is_final) {
	fprintf(f, " %4d,", newstate->u.final);
      } else {
	fprintf(f, " %4d,", newstate->u.i.casenum);
      }

      ++col;
    }

    fprintf(f, "\n};\n");
  } else if (vk == VISIT_GEN_CODE) {
    if (n_nonfinals) fprintf(f, "%*s}\n", indent, "");
  }
  indent -= 2;
}

/////////////////////////////////////
void ISA_Decode_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  std::list<STATE>::iterator state_iter;
  char buf[1000];
#define FNAME "targ_isa_decode"
  sprintf (buf, "%s.h", FNAME);
  FILE* hfile = fopen(buf, "w");
  sprintf (buf, "%s.c", FNAME);
  FILE* cfile = fopen(buf, "w");
  sprintf (buf, "%s.Exported", FNAME);
  FILE* efile = fopen(buf, "w");

  fprintf(cfile, "#include \"topcode.h\"\n"
		 "#include \"targ_isa_bundle.h\"\n"
			 "#include \"targ_isa_pack.h\"\n"
	 "#include \"%s.h\"\n\n", FNAME);

  sprintf (buf, "%s", FNAME);
  Emit_Header (hfile, buf, interface);
  fprintf(hfile, "#include \"topcode.h\"\n"
		 "#include \"targ_isa_bundle.h\"\n"
		 "#include \"targ_isa_pack.h\"\n");

  if (initial_state == NULL) {
    fprintf(stderr, "### Error: no initial decode state specified\n");
    exit(EXIT_FAILURE);
  }

  Visit_State(initial_state, cfile, VISIT_GEN_DATA);

  fprintf(efile, "ISA_Decode_Inst\n");

  fprintf(hfile, "\nextern TOP ISA_Decode_Inst(const ISA_PACK_INST *pinst, ISA_EXEC_UNIT unit);\n");

  fprintf(cfile, "\nTOP ISA_Decode_Inst(const ISA_PACK_INST *pinst, ISA_EXEC_UNIT unit)\n"
		 "{\n"
		 "  INT top;\n"
		 "  INT opc;\n");
  Visit_State(initial_state, cfile, VISIT_GEN_CODE);
  fprintf(cfile, "  return (TOP)top;\n"
		 "}\n");

  for (state_iter = all_states.begin();
       state_iter != all_states.end();
       ++state_iter)
  {
    STATE state = *state_iter;
    if (state->visit == VISIT_UNVISITED) {
      if (state->is_final) {
	fprintf(stderr, "### Warning: final state \"%s\" is unused\n",
		TOP_Name(state->u.final));
      } else {
        fprintf(stderr, "### Warning: intermediate state \"%s\" is unused\n",
		state->u.i.tag);
      }
    }
  }

  Emit_Footer (hfile);
}
#endif
