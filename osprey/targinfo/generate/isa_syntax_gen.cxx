/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/
/*
 * 
 * This file has been modified by STMicroelectronics.
 *
 */

// isa_syntax_gen.cxx
/////////////////////////////////////////////
//
// Generate information for assembly parsing and
// assembly printing.
//
/////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#if __GNUC__ >=3 || defined(_MSC_VER)
#include <list>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
using std::list;
using std::vector;
using std::map;
#include <sstream>
using std::ostringstream;
using std::string;
#else
#include <list.h>
#include <map.h>
#include <vector.h>
#include <algo.h>
#include <string.h>
#include <sstream.h>
#endif // __GNUC__ >=3 || defined(_MSC_VER)

#ifdef DYNAMIC_CODE_GEN
#include "dyn_isa_topcode.h"  // Specific adaptation
#else
#include "topcode.h"
#endif

#include "gen_util.h"
#include "isa_syntax_gen.h"
#include "targ_isa_lits.h"
#include "targ_isa_registers.h"
#include "targ_isa_enums.h"
#include "targ_isa_operands.h"



// In following loops, we iterate on the number of
// TOP. This number differs whether we generate
// static or dynamic TOPs.
#ifdef DYNAMIC_CODE_GEN
static TOP TOP_count_limit = TOP_dyn_count;
#else
static TOP TOP_count_limit = TOP_static_count;
#endif


static const char * const isa_print_interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   A description of how to print the operands of ISA instructions",
  " *   in ascii. The description exports the following:",
  " *",
  " *   typedef (struct) ISA_PRINT_OPND_INFO",
  " *       Description of an operand name and value.",
  " *       The 'name' field is register name or formatted literal value",
  " *       or enum name.",
  " *       The 'value' field is register index or literal value or",
  " *       enum value. This value is used in conditional printing.",
  " *",
  " *   typedef (function) ISA_PRINT_INFO",
  " *       Describes how one particular instruction is printed.",
  " *",
  " *   const ISA_PRINT_INFO *ISA_PRINT_Info(TOP topcode)",
  " *       Returns a pointer to the printing description for the",
  " *       instruction specified by 'topcode'.",
  " *",
  " *   const char *ISA_PRINT_AsmName(TOP topcode)",
  " *       Returns the assembly language name for <topcode>.",
  " *",
  " *   INT ISA_PRINT_Inst(int (* print), void * stream, TOP topcode,",
  " *                                  const char *asmname,",
  " *                                  ISA_PRINT_OPND_INFO *results,",
  " *                                  ISA_PRINT_OPND_INFO *opnds)",
  " *       Print in 'file' the instruction specified by its 'topcode',",
  " *       'asmname','results' and 'opnds'.",
  " *       This function will call ISA_PRINT_Info to get the corresponding",
  " *       ISA_PRINT_INFO routine and will call it by passing all its ",
  " *       parameters 'file', 'topcode', 'asmname', 'results' and 'opnds'.",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};

static const char * const parser_driver_interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " *  Description",
  " *",
  " *  A description of how to get parser entry.",
  " *",
  " *   type PARSER_GetParserT",
  " *       This type is defined in targinfo/parser/inc/parser.h",
  " *       It specify the entry point of parser interface.",
  " *",
  " *   PARSER_GetParserT ISA_PARSE_GetParser(ISA_SUBSET subset)",
  " *       Returns the parser interface entry point associated with",
  " *       given ISA_SUBSET 'subset'.",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};

// Whether we generate code for the core (static) or for an extension.
bool  gen_static_code;

// File handlers
static FILE* hfile    = NULL ;
static FILE* cfile    = NULL ;
/* Driver of parsers in static mode */
static FILE* dcfile   = NULL ;
static FILE* dhfile   = NULL ;

static FILE* efile    = NULL ;
static FILE**pfile    = NULL ;

/* The number of different parsers to generate */
static int parsers = 0;

static char *hfilename     = NULL ;    /* Header file name              */
static char *cfilename     = NULL ;    /* C file name                   */
static char *dcfilename    = NULL ;    /* Parser's driver C file name   */
static char *dhfilename    = NULL ;    /* Parser's driver Header file name   */
static char *efilename     = NULL ;    /* Export file name              */
static char **pfilename    = NULL ;    /* Parser file names             */


#define CHECK_OPENED_FILE(file)						\
  if(file == 0) {							\
    fprintf(stderr,"%s:%d: "						\
            "### Error: "						\
            "%s must be called after ISA_Syntax_Begin routine\n",	\
	    __FILE__,__LINE__,						\
            __FUNCTION__);						\
    exit(EXIT_FAILURE);							\
  }

#define CHECK_CURRENT_SYNTAX_RULE()					\
  if(current_syntax_rule == 0) {					\
    fprintf(stderr,"%s:%d: "						\
            "### Error: "						\
            "%s must call ISA_Syntax_Rule_Create before\n",		\
	    __FILE__,__LINE__,						\
            __FUNCTION__);						\
    exit(EXIT_FAILURE);							\
  }

#define CHECK_CURRENT_SYNTAX()						\
  if(current_syntax == 0) {						\
    fprintf(stderr,"%s:%d: "						\
            "### Error: "						\
            "%s must call ISA_Syntax before\n",				\
	    __FILE__,__LINE__,						\
            __FUNCTION__);						\
    exit(EXIT_FAILURE);							\
  }

#define CHECK_IF_ASSIGNED(pointer)					\
  if(pointer != 0) {							\
    fprintf(stderr,"%s:%d: "						\
            "### Error: "						\
            "%s can be called only once\n",				\
	    __FILE__,__LINE__,						\
            __FUNCTION__);						\
    exit(EXIT_FAILURE);							\
  }


#define MAX_OPNDS   (ISA_OPERAND_max_operands)
#define MAX_RESULTS (ISA_OPERAND_max_results)

typedef enum {
  COMP_END         = 0,
  COMP_NAME        = 1,
  COMP_OPND        = 2,
  COMP_RESULT      = COMP_OPND + MAX_OPNDS,
  COMP_MAX         = COMP_RESULT + MAX_RESULTS,
} COMP_TYPE;

#define COMP_TYPES (COMP_MAX)

struct isa_syntax_comp_type {
  COMP_TYPE            type;
  char                 *prs_fd_pos; /* '%s' position in printed syntax */
  char                 *pas_fd_pos; /* '%s' position in parsed syntax */
  const char           *print_action;
  const char           *parse_operand;
  const char           *parse_rule;
};

typedef isa_syntax_comp_type *ISA_SYNTAX_COMP;

struct isa_syntax_type {
  char                  *printed_syntax;
  char                  *parsed_syntax;
  list <ISA_SYNTAX_COMP> comps;
  const char            *print_condition;
  const char            *parse_action;
};

typedef isa_syntax_type      *ISA_SYNTAX;

struct isa_syntax_rule_type {
  const char            *name;
  bool                  must_be_parsed;
  list <ISA_SYNTAX>     syntaxes;
};

static list <ISA_SYNTAX_RULE> all_syntax_rules;
static ISA_SYNTAX_RULE current_syntax_rule = 0;
static ISA_SYNTAX      current_syntax = 0;

static char *current_prs_fd_pos = 0; /* current position of '%s' in printed syntax */
static char *current_pas_fd_pos = 0; /* current position of '%s' in parsed syntax */

static const char *(*asmname)(TOP topcode);
static const char *mspace = "";
static const char *ospace = "";
static const char *pspace = "";
static const char *tab = "";

static const char **parse_reg_classes = 0;
static const char **parse_lit_classes = 0;

struct isa_syntax_group {
  const char     *name;
  int             group_id;
  ISA_SYNTAX_RULE rule;
  list<TOP>       tops;
};

typedef isa_syntax_group *ISA_SYNTAX_GROUP;

static list <ISA_SYNTAX_GROUP> all_syntax_groups;
static ISA_SYNTAX_GROUP *top2group = 0;
static bool            *top_specified = 0;
static int current_group_id = 1;

/* ISA_OPERAND_VALTYP identifiers */

static char  **ISA_OPERAND_VALTYP_identifiers;
static char  current_identifier_base = 'a';
static char  current_identifier_index = 'a';

static void
NEXT_Identifier( ) {
  current_identifier_index++;
  if(current_identifier_index == '{') { /* after 'z' */
    current_identifier_index = 'A';
  }
  if(current_identifier_index == '[') { /* after 'Z' */
    current_identifier_base++;
    current_identifier_index = 'A';
  }
  if(current_identifier_base == '{') { /* after 'z' */
    current_identifier_base = 'A';
  }
  if(current_identifier_base == '[' &&  /* after 'Z' */
     current_identifier_base == current_identifier_index) {
    fprintf(stderr,"%s:%d: ### Error: Max identifier encountered\n",
	    __FILE__,__LINE__);
    exit(EXIT_FAILURE);
  }
}

static char *
GET_Literal_Identifier() {
  ostringstream identifier;
  identifier << "<#" << current_identifier_base << current_identifier_index << ">";
  NEXT_Identifier();
  return strdup((char*)identifier.str().data());
}

static char *
GET_Register_Identifier() {
  ostringstream identifier;
  identifier << "<<" << current_identifier_base << current_identifier_index << ">>";
  NEXT_Identifier();
  return strdup((char*)identifier.str().data());
}

static char *
GET_Operand_Identifier( const ISA_OPERAND_INFO *oinfo, INT opnd )
{
  INT index = oinfo->opnd[opnd];
  const ISA_OPERAND_VALTYP *valtype = ISA_OPERAND_INFO_Operand(oinfo,opnd);

  if(ISA_OPERAND_VALTYP_identifiers[index] == 0) {
    if(ISA_OPERAND_VALTYP_Is_Literal(valtype)) {
      ISA_OPERAND_VALTYP_identifiers[index] = GET_Literal_Identifier();
    }
    else {
      ISA_OPERAND_VALTYP_identifiers[index] = GET_Register_Identifier();
    }
  }
  return ISA_OPERAND_VALTYP_identifiers[index];
}

static char *
GET_Operand_Identifier_From_ValTyp( const ISA_OPERAND_VALTYP *valtyp, INT index )
{
  if(ISA_OPERAND_VALTYP_identifiers[index] == 0) {
    if(ISA_OPERAND_VALTYP_Is_Literal(valtyp)) {
      ISA_OPERAND_VALTYP_identifiers[index] = GET_Literal_Identifier();
    }
    else {
      ISA_OPERAND_VALTYP_identifiers[index] = GET_Register_Identifier();
    }
  }
  return ISA_OPERAND_VALTYP_identifiers[index];
}

static char *
GET_Result_Identifier( const ISA_OPERAND_INFO *oinfo, INT result )
{
  INT index = oinfo->result[result];

  if(ISA_OPERAND_VALTYP_identifiers[index] == 0) {
    ISA_OPERAND_VALTYP_identifiers[index] = GET_Register_Identifier();
  }
  return ISA_OPERAND_VALTYP_identifiers[index];
}

/* Operand rules */
typedef struct {
  char *rule;
  char *name;
} Rule_Opnd_Type;

static Rule_Opnd_Type **ISA_OPERAND_VALTYP_relocs;
static char *reloc_opnd_base_name = "relop";
static int reloc_opnd_idx = 0;

static Rule_Opnd_Type **ISA_OPERAND_VALTYP_enums;
static char *enum_opnd_base_name = "enum";
static int enum_opnd_idx = 0;

static Rule_Opnd_Type *
GET_Reloc_Identifier()
{
  Rule_Opnd_Type *reloc_opnd = (Rule_Opnd_Type*)malloc(sizeof(Rule_Opnd_Type));
  ostringstream rule;
  ostringstream name;
  rule << "<" << reloc_opnd_base_name << "_" << reloc_opnd_idx << ">";
  reloc_opnd->rule = strdup((char*)rule.str().data());
  name << reloc_opnd_base_name << "_" << reloc_opnd_idx++;
  reloc_opnd->name = strdup((char*)name.str().data());
  return reloc_opnd;
}

static char *
GET_Relocatable_Operand_Rule( const ISA_OPERAND_INFO *oinfo, INT opnd )
{
  INT index = oinfo->opnd[opnd];
  const ISA_OPERAND_VALTYP *valtype = ISA_OPERAND_INFO_Operand(oinfo,opnd);

  if(ISA_OPERAND_VALTYP_relocs[index] == 0) {
    ISA_OPERAND_VALTYP_relocs[index] = GET_Reloc_Identifier();
  }
  return ISA_OPERAND_VALTYP_relocs[index]->rule;
}

static char *
GET_Relocatable_Operand_Name( const ISA_OPERAND_INFO *oinfo, INT opnd )
{
  INT index = oinfo->opnd[opnd];
  const ISA_OPERAND_VALTYP *valtype = ISA_OPERAND_INFO_Operand(oinfo,opnd);

  if(ISA_OPERAND_VALTYP_relocs[index] == 0) {
    ISA_OPERAND_VALTYP_relocs[index] = GET_Reloc_Identifier();
  }
  return ISA_OPERAND_VALTYP_relocs[index]->name;
}

static Rule_Opnd_Type *
GET_Enum_Identifier()
{
  Rule_Opnd_Type *enum_opnd = (Rule_Opnd_Type*)malloc(sizeof(Rule_Opnd_Type));
  ostringstream rule;
  ostringstream name;
  rule << "<" << enum_opnd_base_name << "_" << enum_opnd_idx << ">";
  enum_opnd->rule = strdup((char*)rule.str().data());
  name << enum_opnd_base_name << "_" << enum_opnd_idx++;
  enum_opnd->name = strdup((char*)name.str().data());
  return enum_opnd;
}

static char *
GET_Enum_Operand_Rule( const ISA_OPERAND_INFO *oinfo, INT opnd )
{
  INT index = oinfo->opnd[opnd];
  const ISA_OPERAND_VALTYP *valtype = ISA_OPERAND_INFO_Operand(oinfo,opnd);

  if(ISA_OPERAND_VALTYP_enums[index] == 0) {
    ISA_OPERAND_VALTYP_enums[index] = GET_Enum_Identifier();
  }
  return ISA_OPERAND_VALTYP_enums[index]->rule;
}

static char *
GET_Enum_Operand_Name( const ISA_OPERAND_INFO *oinfo, INT opnd )
{
  INT index = oinfo->opnd[opnd];
  const ISA_OPERAND_VALTYP *valtype = ISA_OPERAND_INFO_Operand(oinfo,opnd);

  if(ISA_OPERAND_VALTYP_enums[index] == 0) {
    ISA_OPERAND_VALTYP_enums[index] = GET_Enum_Identifier();
  }
  return ISA_OPERAND_VALTYP_enums[index]->name;
}

static map<string,list<ISA_SYNTAX_GROUP> >
sort_isa_subsetgroup( list<ISA_SYNTAX_GROUP> &isa_subset_groups ) {
  map<string,list<ISA_SYNTAX_GROUP> > sorted_isa_subset_group;
  string signature;
  list<ISA_SYNTAX_GROUP>::iterator group_iter;
  
  for(group_iter = isa_subset_groups.begin();
      group_iter != isa_subset_groups.end();
      group_iter++) {
    ISA_SYNTAX_GROUP group = *group_iter;
    ISA_SYNTAX_RULE rule = group->rule;
    list<ISA_SYNTAX>::iterator syntax_iter;
    list<TOP>::iterator top_iter;
    ISA_SYNTAX syntax;
    int syntax_length;
    
    syntax_iter = rule->syntaxes.begin();
    syntax = *syntax_iter;
#if 1 /* [HC] This is a "bad" workaround waiting for sort done at MDS level *
       *      We need gp48 instructions first, then gp32 and finally gp16   */
    top_iter = group->tops.begin();
    if (NULL!=strstr(TOP_Name(*top_iter),"v4_gp48")) {
      signature = "3-";
      signature = signature + syntax->parsed_syntax;
    } else if (NULL!=strstr(TOP_Name(*top_iter),"v4_x3_gp48")) {
      signature = "3-";
      signature = signature + syntax->parsed_syntax;
    } else if (NULL!=strstr(TOP_Name(*top_iter),"v4_gp32")) {
      signature = "2-";
      signature = signature + syntax->parsed_syntax;
    } else if (NULL!=strstr(TOP_Name(*top_iter),"v4_x3_gp32")) {
      signature = "2-";
      signature = signature + syntax->parsed_syntax;
    } else if (NULL!=strstr(TOP_Name(*top_iter),"v4_gp16")) {
      signature = "1-";
      signature = signature + syntax->parsed_syntax;
    } else if (NULL!=strstr(TOP_Name(*top_iter),"v4_x3_gp16")) {
      signature = "1-";
      signature = signature + syntax->parsed_syntax;
    } else {
      signature = syntax->parsed_syntax;
    }
#else
    signature = syntax->parsed_syntax;
#endif
    syntax_length = strlen(syntax->parsed_syntax);

    /* check that every alternative syntax has the same length */
    for(syntax_iter = rule->syntaxes.begin();
        syntax_iter != rule->syntaxes.end();
        syntax_iter++) {
      syntax = *syntax_iter;
      
      if (strlen(syntax->parsed_syntax)>syntax_length) {
        fprintf(stderr,"%s:%d: ### Warning: Unable to manage more than one syntaxe length per group [%s]\n",
	             __FILE__,__LINE__,group->name);
#if 1
      if (NULL!=strstr(TOP_Name(*top_iter),"v4_gp48")) {
        signature = "3-";
        signature = signature + syntax->parsed_syntax;
      } else if (NULL!=strstr(TOP_Name(*top_iter),"v4_x3_gp48")) {
        signature = "3-";
        signature = signature + syntax->parsed_syntax;
      } else if (NULL!=strstr(TOP_Name(*top_iter),"v4_gp32")) {
        signature = "2-";
        signature = signature + syntax->parsed_syntax;
      } else if (NULL!=strstr(TOP_Name(*top_iter),"v4_x3_gp32")) {
        signature = "2-";
        signature = signature + syntax->parsed_syntax;
      } else if (NULL!=strstr(TOP_Name(*top_iter),"v4_gp16")) {
        signature = "1-";
        signature = signature + syntax->parsed_syntax;
      } else if (NULL!=strstr(TOP_Name(*top_iter),"v4_x3_gp16")) {
        signature = "1-";
        signature = signature + syntax->parsed_syntax;
      } else {
        signature = syntax->parsed_syntax;
      }
#else
      signature = syntax->parsed_syntax;
#endif
//[TODO: Should have one group per syntax length, or check potential ambiguities]        exit(EXIT_FAILURE);
      }
    }
    sorted_isa_subset_group[signature].push_back(group);
  }
  return sorted_isa_subset_group;
}

static void
print_parser_root( FILE *file, ISA_SUBSET subset, list<ISA_SYNTAX_GROUP> isa_subset_groups )
{
  map<string,list<ISA_SYNTAX_GROUP> > map_isa_subset_groups = sort_isa_subsetgroup(isa_subset_groups);
  map<string,list<ISA_SYNTAX_GROUP> >::reverse_iterator map_iter;
  
  fprintf(file,
	  ".Root:%s_CORE_ROOT_INST\n"
	  ".De:{",ISA_SUBSET_Name(subset));

  int rule_nb = 0;
  const char *comma = "";
  for(map_iter=map_isa_subset_groups.rbegin();
      map_iter!=map_isa_subset_groups.rend();
      map_iter++) {
    list<ISA_SYNTAX_GROUP> groups = map_iter->second;
    
    for(list<ISA_SYNTAX_GROUP>::iterator group_iter=groups.begin();
        group_iter != groups.end();
        group_iter++) {
      ISA_SYNTAX_GROUP grp = *group_iter;
      ISA_SYNTAX_RULE rule = grp->rule;
    
      fprintf(file,"%s",comma);
      comma = ",";
      rule_nb++;
      if(!(rule_nb % 5)) {
        fprintf(file,"\n     ");
      }
      fprintf(file," %s",rule->name);
    }
  }

  if(rule_nb % 5) { fprintf(file,"\n"); }
  fprintf(file,
	  "}\n"
	  ".Act:{}\n"
	  "\n"
	  ".Root:CORE_ROOT_INST\n"
	  ".De:{%s_CORE_ROOT_INST}\n"
	  ".Act:{}\n"
	  "\n"
	  ".init:\n"
	  ".Act:{\n"
	  "  SET_REG_CLASS_PARSING(GET_REGISTER_CLASS);\n"
	  "  SET_LIT_CLASS_PARSING(GET_LITERAL_CLASS);\n"
	  "  SETAXIOM(CORE_ROOT_INST);\n"
	  "}\n\n",ISA_SUBSET_Name(subset));
}


/* ====================================================================
 *   Emit_Valtypes_Identifiers
 * ====================================================================
 */
static void
Emit_Valtypes_Identifiers( ISA_SUBSET subset, FILE *file )
{
  map<string,int> identifiers;

  fprintf(file,"\n\n");
  fprintf(file,"\n/* ********************************************* */");
  fprintf(file,"\n/* Begin of valtypes identifier parsing          */");
  fprintf(file,"\n/* ********************************************* */");
  fprintf(file,"\n\n");


  fprintf(file,".globdecl: {\n");
  if(gen_static_code) {
    fprintf(file,
	    "#include \"targ_isa_registers.h\"\n"
	    "#include \"targ_isa_lits.h\"\n"
	    "#include \"targ_isa_relocs.h\"\n"
	    "#include \"topcode.h\"\n\n");
  }
  else {
    fprintf(file,
	    "#include \"dyn_isa_registers.h\"\n"
	    "#include \"dyn_isa_lits.h\"\n"
	    "#include \"dyn_isa_relocs.h\"\n"
	    "#include \"dyn_isa_topcode.h\"\n\n");
  }

  fprintf(file,"static int extension_index = -1;\n");

  for(int i=0; i<ISA_OPERAND_types_count;i++) {
    if(ISA_OPERAND_VALTYP_identifiers[i] != 0) {
      map<string,int>::iterator ident_iter = identifiers.find(ISA_OPERAND_VALTYP_identifiers[i]);
      if(ident_iter != identifiers.end()) {
	fprintf(stderr, "%s:%d: ### Error: duplicated identifier '%s' on index %d and %d\n",
		__FILE__,__LINE__,
		ISA_OPERAND_VALTYP_identifiers[i],i,ident_iter->second);
	exit(EXIT_FAILURE);
      }
      identifiers[ISA_OPERAND_VALTYP_identifiers[i]] = i;
    }
  }

  fprintf(file,
	  "static void\n"
	  "get_register_class(char *reg_name, ISA_REGISTER_CLASS *rclass, ISA_REGISTER_SUBCLASS *rsclass) {\n"
	  "  char *curChar = reg_name;\n"
	  "\n"
	  "#define SET_RCLASS(rc)			\\\n"
	  "  if(*(curChar+1) != '\\0') { return; }	\\\n"
	  "  *rclass = rc;  \n"
	  "\n"
	  "#define SET_RSCLASS(rsc)			\\\n"
	  "  if(*(curChar+1) != '\\0') { return; }	\\\n"
	  "  *rsclass = rsc;  \n"
	  "\n"
	  "  *rclass = ISA_REGISTER_CLASS_UNDEFINED;\n"
	  "  *rsclass = ISA_REGISTER_SUBCLASS_UNDEFINED;\n"
	  "\n"
	  "  switch(*curChar) {\n");

  char current_base = '\0';
  for(map<string,int>::iterator ident_iter = identifiers.begin();
      ident_iter != identifiers.end();
      ident_iter++) {
    const char *ident = ident_iter->first.data();
    int index = ident_iter->second;
    const ISA_OPERAND_VALTYP *valtype = &ISA_OPERAND_operand_types[index];
    ident++; /* skip first '<' */
    if(*ident == '<') { /* get a register like '<<aa>>' */
      ident++; /* skip second '<' */
      if(*ident != current_base) {
	if(current_base != '\0') {
	  fprintf(file,
		  "    default:\n"
		  "    return;\n"
		  "  }\n"
		  "  break;\n");
	}
	current_base = *ident;
	fprintf(file,
		"  case '%c':\n"
		"    curChar++;\n"
		"    switch(*curChar) {\n",current_base);
      }
      const ISA_REGISTER_CLASS_INFO *rc_info = ISA_REGISTER_CLASS_Info(ISA_OPERAND_VALTYP_Register_Class(valtype));
      const ISA_REGISTER_SUBCLASS_INFO *sc_info = ISA_REGISTER_SUBCLASS_Info(ISA_OPERAND_VALTYP_Register_Subclass(valtype));
      fprintf(file,
	      "    case '%c':\n"
	      "      SET_RCLASS(ISA_REGISTER_CLASS_%s);\n"
	      "      SET_RSCLASS(ISA_REGISTER_SUBCLASS_%s);\n"
	      "      break;\n",
	      *(ident+1),
	      ISA_REGISTER_CLASS_INFO_Name(rc_info),
	      ISA_REGISTER_SUBCLASS_INFO_Name(sc_info)
	      );
    }
  }

  fprintf(file,
	  "    default:\n"
	  "      return;\n"
	  "    }\n"
	  "  default:\n");
  if(parse_reg_classes[subset] != 0) {
    fprintf(file,"    %s(reg_name,rclass,rsclass);\n",
	    parse_reg_classes[subset]);
  }
  fprintf(file,
	  "    return;\n"
	  "  }\n"
	  "#undef SET_RCLASS\n"
	  "#undef SET_RSCLASS\n"
	  "}\n\n");

  fprintf(file,
	  "static void\n"
	  "get_literal_class(char *lit_name, ISA_LIT_CLASS *lclass) {\n"
	  "  char *curChar = lit_name;\n"
	  "\n"
	  "  *lclass = ISA_LC_UNDEFINED;\n"
	  "\n"
	  "  switch(*curChar) {\n");

  current_base = '\0';
  for(map<string,int>::iterator ident_iter = identifiers.begin();
      ident_iter != identifiers.end();
      ident_iter++) {
    const char *ident = ident_iter->first.data();
    int index = ident_iter->second;
    const ISA_OPERAND_VALTYP *valtype = &ISA_OPERAND_operand_types[index];
    ident++; /* skip '<' */
    if(*ident == '#') { /* get a literal like '<#aa>' */
      ident++; /* skip '#' */
      if(*ident != current_base) {
	if(current_base != '\0') {
	  fprintf(file,
		  "    default:\n"
		  "    return;\n"
		  "  }\n"
		  "  break;\n");
	}
	current_base = *ident;
	fprintf(file,
		"  case '%c':\n"
		"    curChar++;\n"
		"    switch(*curChar) {\n",current_base);
      }
      ISA_LIT_CLASS lc = ISA_OPERAND_VALTYP_Literal_Class(valtype);
      fprintf(file,
	      "    case '%c':\n"
	      "      *lclass = %s;\n"
	      "      break;\n",
	      *(ident+1),
	      ISA_LC_Name(lc)
	      );
    }
  }
  fprintf(file,
	  "    default:\n"
	  "      return;\n"
	  "    }\n"
	  "  default:\n");
  if(parse_lit_classes[subset] != 0) {
    fprintf(file,"    %s(lit_name,lclass);\n",
	    parse_lit_classes[subset]);
  }
  fprintf(file,
	  "    return;\n"
	  "  }\n"
	  "\n"
	  "  return;\n"
	  "}\n\n");

  fprintf(file,
	  "/* To give to the parser routines to get classes. */\n"
	  "#define GET_REGISTER_CLASS get_register_class\n"
	  "#define GET_LITERAL_CLASS get_literal_class\n"
	  "\n"
	  "/* some more C code */\n"
	  "static char *targetString = \"%s\" ;\n",
	  ISA_SUBSET_Name(subset));

  fprintf(file,"}\n");


  fprintf(file,"\n\n");
  fprintf(file,"\n/* ********************************************* */");
  fprintf(file,"\n/* End of valtypes identifier parsing            */");
  fprintf(file,"\n/* ********************************************* */");
}

/* ====================================================================
 *   Emit_Parser_Driver
 * ====================================================================
 */
static void
Emit_Parser_Driver( void )
{
  if (gen_static_code) {
    if(dhfile == 0 || dcfile == 0) { return; }

    fprintf(dcfile,"\n");
    fprintf(dcfile,"#include \"air.h\"\n");
    fprintf(dcfile,"#include \"parser.h\"\n");
    fprintf(dcfile,"\n");

    fprintf(dhfile,
	    "BE_EXPORTED extern PARSER_GetParserT * ISA_PARSE_tab;\n\n");

    fprintf(dhfile,
	    "inline PARSER_GetParserT ISA_PARSE_GetParser(ISA_SUBSET subset)\n"
	    "{\n"
	    "  BE_EXPORTED extern PARSER_GetParserT * ISA_PARSE_tab;\n"
	    "  return ISA_PARSE_tab[subset];\n"
	    "}\n");

    for(int i=0; i<parsers;i++) {
      fprintf(dcfile,"extern PARSER_ReturnT %s_PARSER_GetParser (PARSER_InstanceT *Parser);\n",
	      ISA_SUBSET_Name(i));
    }
    fprintf(dcfile,"\n");
    fprintf(dcfile,
	    "static PARSER_GetParserT static_ISA_PARSE_tab[] = {\n");
    const char *comma = ",";
    for(int i=0; i<parsers;i++) {
      if(i == parsers-1) { comma = ""; }
      fprintf(dcfile,"  &%s_PARSER_GetParser%s\n",
	      ISA_SUBSET_Name(i),comma);
    }
    fprintf(dcfile,"};\n");
    fprintf(dcfile,
	    "PARSER_GetParserT * ISA_PARSE_tab = &static_ISA_PARSE_tab[0];\n");
    fprintf(dcfile,"\n");
  } else {
    if(dhfile == 0 || dcfile == 0) { return; }

    fprintf(dhfile,
            "extern const PARSER_GetParserT * dyn_get_ISA_PARSE_tab ( void );\n");
    fprintf(dhfile,
            "extern const mUINT32 dyn_get_ISA_PARSE_tab_sz ( void );\n");

    fprintf(dcfile,"\n");
    fprintf(dcfile,"#include \"air.h\"\n");
    fprintf(dcfile,"#include \"parser.h\"\n");
    fprintf(dcfile,"\n");

    for(int i=0; i<parsers;i++) {
      fprintf(dcfile,"extern PARSER_ReturnT %s_PARSER_GetParser (PARSER_InstanceT *Parser);\n",
	      ISA_SUBSET_Name(i));
    }

    fprintf(dcfile,
            "static const PARSER_GetParserT ISA_PARSE_dyn_tab[] = {\n");
    const char *comma = ",";
    for(int i=0; i<parsers;i++) {
      if(i == parsers-1) { comma = ""; }
      fprintf(dcfile,"  &%s_PARSER_GetParser%s\n",
	      ISA_SUBSET_Name(i),comma);
    }
    fprintf(dcfile,"};\n");

    fprintf(dcfile,
            "const PARSER_GetParserT * dyn_get_ISA_PARSE_tab ( void )\n"
	    "{\n"
	    "  return ISA_PARSE_dyn_tab;\n"
	    "}\n");

    fprintf(dcfile,
            "const mUINT32 dyn_get_ISA_PARSE_tab_sz ( void )\n"
            "{\n"
	    "  return (const mUINT32) %d;\n"
	    "}\n",parsers);
  }
  
  Emit_Footer (dhfile);
  Emit_C_Footer(dcfile);
}

/* ====================================================================
 *   get_printed_syntax
 *
 * Allocate a new string adapted to isa_print targinfo entry where:
 * - Mandatory and Optional Printed spaces are : space
 * - Optional spaces are: nothing
 * - Tabulations are: '\t'
 * ====================================================================
 */
static char*
get_printed_syntax( const char *syntax) {
  char *printed_syntax = (char*)malloc(strlen(syntax)+1);
  char *syntax_ptr = printed_syntax;
  size_t mspace_len = strlen(mspace);
  size_t ospace_len = strlen(ospace);
  size_t pspace_len = strlen(pspace);
  size_t tab_len = strlen(tab);

  while(*syntax != '\0') {
    if(strncmp(syntax,mspace,mspace_len) == 0) {
      *syntax_ptr++ = ' ';
      syntax += mspace_len;
    }
    else if(strncmp(syntax,ospace,ospace_len) == 0) {
      syntax += ospace_len;
    }
    else if(strncmp(syntax,pspace,pspace_len) == 0) {
      *syntax_ptr++ = ' ';
      syntax += pspace_len;
    }
    else if(strncmp(syntax,tab,tab_len) == 0) {
      *syntax_ptr++ = '\t';
      syntax += tab_len;
    }
    else {
      *syntax_ptr++ = *syntax++;
    }
  }
  *syntax_ptr = '\0';
  return printed_syntax;
}

/* ====================================================================
 *   get_parsed_syntax
 *
 * Allocate a new string adapted to parser description file where:
 * - Mandatory spaces are : tab
 * - Optional and Optional Printed spaces are: space
 * - Tabulations are: nothing
 * ====================================================================
 */
static char*
get_parsed_syntax( const char *syntax) {
  char *parsed_syntax = (char*)malloc(2*strlen(syntax));
  char *syntax_ptr = parsed_syntax;
  size_t mspace_len = strlen(mspace);
  size_t ospace_len = strlen(ospace);
  size_t pspace_len = strlen(pspace);
  size_t tab_len = strlen(tab);
  bool begin_of_syntax = true;

  while(*syntax != '\0') {
    if((begin_of_syntax && 
	!(*syntax == ' ' ||
	  *syntax == '\t')) ||
       !begin_of_syntax) {
      begin_of_syntax = false;
      if(strncmp(syntax,mspace,mspace_len) == 0) {
	*syntax_ptr++ = '\t';
	syntax += mspace_len;
      }
      else if(strncmp(syntax,ospace,ospace_len) == 0) {
	*syntax_ptr++ = ' ';
	syntax += ospace_len;
      }
      else if(strncmp(syntax,pspace,pspace_len) == 0) {
	*syntax_ptr++ = ' ';
	syntax += pspace_len;
      }
      else if(strncmp(syntax,tab,tab_len) == 0) {
	syntax += tab_len;
      }
      else if(*syntax == '[' ||
	      *syntax == ']' ||
	      *syntax == '(' ||
	      *syntax == ')' ||
	      *syntax == ' ' ||
	      *syntax == '\t') {
	/* Escape character */
	*syntax_ptr++ = '\\';
	*syntax_ptr++ = *syntax++;
      }
      else {
	*syntax_ptr++ = *syntax++;
      }
    }
    else {
      syntax++;
    }
  }
  *syntax_ptr = '\0';
  return parsed_syntax;
}

//////////////////////////////////////////////////////////////////////////
// Beginning of code
//////////////////////////////////////////////////////////////////////////

/* ====================================================================
 *   ISA_Parse_Globals
 * ====================================================================
 */
void ISA_Parse_Globals ( const char* globals, ISA_SUBSET subset )
{
  ISA_SUBSET first = (subset == ISA_SUBSET_UNDEFINED) ? ISA_SUBSET_MIN : subset;
  int count = (subset == ISA_SUBSET_UNDEFINED) ? ISA_SUBSET_count: subset+1;
  for(subset = first; subset < count; subset++) {
    FILE *file = pfile[subset];
    CHECK_OPENED_FILE(file);

    fprintf(file,".globdecl: {\n");
    if(gen_static_code) {
      fprintf(file,
	      "#include \"targ_isa_registers.h\"\n"
	      "#include \"targ_isa_lits.h\"\n"
	      "#include \"targ_isa_relocs.h\"\n"
	      "#include \"topcode.h\"\n\n");
    }
    else {
      fprintf(file,
	      "#include \"dyn_isa_registers.h\"\n"
	      "#include \"dyn_isa_lits.h\"\n"
	      "#include \"dyn_isa_relocs.h\"\n"
	      "#include \"dyn_isa_topcode.h\"\n\n");
    }
    
    fprintf(file,"%s\n",globals);
    fprintf(file,"}\n");
  }
}

/* ====================================================================
 *   ISA_Parse_Registers
 * ====================================================================
 */
void ISA_Parse_Registers( const char *_parse_reg_classes, ISA_SUBSET subset )
{
  ISA_SUBSET first = (subset == ISA_SUBSET_UNDEFINED) ? ISA_SUBSET_MIN : subset;
  int count = (subset == ISA_SUBSET_UNDEFINED) ? ISA_SUBSET_count: subset+1;
  for(subset = first; subset < count; subset++) {
    if(parse_reg_classes[subset] != 0) {
      fprintf(stderr, "%s:%d: ### Error: parse registers function already defined for subset %s\n",
	      __FILE__,__LINE__,
	      ISA_SUBSET_Name(subset));
      exit(EXIT_FAILURE);
    }
    parse_reg_classes[subset] = _parse_reg_classes;
  }
}

/* ====================================================================
 *   ISA_Parse_Literals
 * ====================================================================
 */
void ISA_Parse_Literals( const char *_parse_lit_classes, ISA_SUBSET subset )
{
  ISA_SUBSET first = (subset == ISA_SUBSET_UNDEFINED) ? ISA_SUBSET_MIN : subset;
  int count = (subset == ISA_SUBSET_UNDEFINED) ? ISA_SUBSET_count: subset+1;
  for(subset = first; subset < count; subset++) {
    if(parse_lit_classes[subset] != 0) {
      fprintf(stderr, "%s:%d: ### Error: parse literals function already defined for subset %s\n",
	      __FILE__,__LINE__,
	      ISA_SUBSET_Name(subset));
      exit(EXIT_FAILURE);
    }
    parse_lit_classes[subset] = _parse_lit_classes;
  }
}


/* ====================================================================
 *   ISA_Parse_Rules
 * ====================================================================
 */
void ISA_Parse_Rules ( const char* rules, ISA_SUBSET subset )
{
  ISA_SUBSET first = (subset == ISA_SUBSET_UNDEFINED) ? ISA_SUBSET_MIN : subset;
  int count = (subset == ISA_SUBSET_UNDEFINED) ? ISA_SUBSET_count: subset+1;
  for(subset = first; subset < count; subset++) {
    FILE *file = pfile[subset];
    CHECK_OPENED_FILE(file);
    fprintf(file,
	    "\n/* ********************************************* */"
	    "\n/* Begin of rules insertion from ISA_Parse_Rules */"
	    "\n/* ********************************************* */\n\n");
    
    fprintf(file,"%s\n",rules);
    
    fprintf(file,
	    "\n/* ********************************************* */"
	    "\n/* End of rules insertion from ISA_Parse_Rules   */"
	    "\n/* ********************************************* */\n");
  }
}

/* ====================================================================
 *   ISA_Print_Globals
 * ====================================================================
 */
void ISA_Print_Globals ( const char* globals )
{
  CHECK_OPENED_FILE(cfile);

  fprintf(cfile,"%s\n",globals);
}

/* ====================================================================
 *   ISA_Syntax_Rule_Create
 * ====================================================================
 */
ISA_SYNTAX_RULE
ISA_Syntax_Rule_Create(const char *name, bool must_be_parsed ) {
  ISA_SYNTAX_RULE rule = new isa_syntax_rule_type;

  rule->name = name;
  rule->must_be_parsed = must_be_parsed;
  all_syntax_rules.push_back(rule);
  current_syntax_rule = rule;
  return rule;
}

/* ====================================================================
 *   find_next_fd_pos
 *
 * Find next format directive '%s' in syntax string.
 * ====================================================================
 */
static char *
find_next_fd_pos(char *str)
{
  bool find_percent = false;
  /* Skip actual '%' */
  str++;
  while(*str != '\0' && !find_percent) {
    if(*str == '%' && *(str+1) != '%') {
      find_percent = true;
    }
    else {
      str++;
    }
  }
  return str;
}

/* ====================================================================
 *   find_after_fd_pos
 *
 * Find after format directive '%s' in syntax string.
 * ====================================================================
 */
static char *
find_after_fd_pos(char *str, bool skip_spaces)
{
  bool find_s = false;
  /* Skip actual '%' */
  str++;
  while(*str != '\0' && !find_s) {
    if(*str == 's') {
      find_s = true;
    }
    str++;
  }
  if(find_s && skip_spaces) {
    while(*str != '\0' &&
	  (*str == ' ' ||
	   *str == '\t')) {
      str++;
    }
  }
  return str;
}

/* ====================================================================
 *   find_first_fd_pos
 *
 * Find first format directive '%s' in syntax string.
 * ====================================================================
 */
static char *
find_first_fd_pos(char *str)
{
  bool find_percent = false;
  while(*str != '\0' && !find_percent) {
    if(*str == '%' && *(str+1) != '%') {
      find_percent = true;
    }
    else {
      str++;
    }
  }
  return str;
}

/* ====================================================================
 *   ISA_Syntax
 * ====================================================================
 */
void
ISA_Syntax( const char *syntax )
{
  CHECK_CURRENT_SYNTAX_RULE();
  ISA_SYNTAX isa_syntax = new isa_syntax_type;
  isa_syntax->printed_syntax = get_printed_syntax(syntax);
  isa_syntax->parsed_syntax = get_parsed_syntax(syntax);
  isa_syntax->print_condition = 0;
  isa_syntax->parse_action = 0;
  current_syntax = isa_syntax;
  current_syntax_rule->syntaxes.push_back(isa_syntax);
  current_prs_fd_pos = find_first_fd_pos((char*)current_syntax->printed_syntax);
  current_pas_fd_pos = find_first_fd_pos((char*)current_syntax->parsed_syntax);
  return;
}

/* ====================================================================
 *   ISA_Syntax_Print_Condition
 * ====================================================================
 */
void ISA_Syntax_Print_Condition( const char *print_condition )
{
  current_syntax->print_condition = print_condition;
}

/* ====================================================================
 *   ISA_Syntax_Parse_Action
 * ====================================================================
 */
void ISA_Syntax_Parse_Action   ( const char *parse_action )
{
  current_syntax->parse_action = parse_action;
}


/* ====================================================================
 *   Operand
 * ====================================================================
 */
void Operand( int operand_index, const char *print_action, const char *parse_operand, const char *parse_rule )
{
  CHECK_CURRENT_SYNTAX();
  if(operand_index >= MAX_OPNDS) {
    fprintf(stderr, "%s:%d: ### Error: operand index (%d) exceeds %d\n",
	    __FILE__,__LINE__,
	    operand_index, MAX_OPNDS-1);
    exit(EXIT_FAILURE);
  }

  ISA_SYNTAX_COMP comp = new isa_syntax_comp_type;
  comp->type = (COMP_TYPE)(COMP_OPND+operand_index);
  comp->prs_fd_pos = current_prs_fd_pos;
  comp->print_action = print_action;
  comp->parse_operand = parse_operand;
  comp->parse_rule = parse_rule;
  current_prs_fd_pos = find_next_fd_pos(current_prs_fd_pos);
  comp->pas_fd_pos = current_pas_fd_pos;
  current_pas_fd_pos = find_next_fd_pos(current_pas_fd_pos);
  current_syntax->comps.push_back(comp);
  return;
}


/* ====================================================================
 *   Result
 * ====================================================================
 */
void Result( int result_index, const char *print_action, const char *parse_operand, const char *parse_rule )
{
  CHECK_CURRENT_SYNTAX();
  if(result_index >= MAX_RESULTS) {
    fprintf(stderr, "%s:%d: ### Error: result index (%d) exceeds %d\n",
	    __FILE__,__LINE__,
	    result_index, MAX_RESULTS-1);
    exit(EXIT_FAILURE);
  }

  ISA_SYNTAX_COMP comp = new isa_syntax_comp_type;
  comp->type = (COMP_TYPE)(COMP_RESULT+result_index);
  comp->prs_fd_pos = current_prs_fd_pos;
  comp->print_action = print_action;
  comp->parse_operand = parse_operand;
  comp->parse_rule = parse_rule;
  current_prs_fd_pos = find_next_fd_pos(current_prs_fd_pos);
  comp->pas_fd_pos = current_pas_fd_pos;
  current_pas_fd_pos = find_next_fd_pos(current_pas_fd_pos);
  current_syntax->comps.push_back(comp);
  return;
}

/* ====================================================================
 *   Name
 * ====================================================================
 */
void Name()
{
  CHECK_CURRENT_SYNTAX();
  ISA_SYNTAX_COMP comp = new isa_syntax_comp_type;
  comp->type = COMP_NAME;
  comp->prs_fd_pos = current_prs_fd_pos;
  current_prs_fd_pos = find_next_fd_pos(current_prs_fd_pos);
  comp->pas_fd_pos = current_pas_fd_pos;
  current_pas_fd_pos = find_next_fd_pos(current_pas_fd_pos);
  current_syntax->comps.push_back(comp);
  return;
}

/* ====================================================================
 *   ISA_Syntax_Group
 * ====================================================================
 */
void ISA_Syntax_Group(const char *name, ISA_SYNTAX_RULE rule, ...)
{
  va_list ap;
  TOP top;
  TOP stop = gen_static_code ? TOP_UNDEFINED : static_cast<TOP> (-1);

  if(NULL==top2group || NULL==top_specified) {
    fprintf(stderr,"%s:%d: ### Fatal error: ISA_Syntax_Begin must be called before %s\n",
	    __FILE__,__LINE__,
	    __FUNCTION__);
    exit(EXIT_FAILURE);
  }

  ISA_SYNTAX_GROUP  group = new isa_syntax_group;
  group->name = name;
  group->group_id = current_group_id++;
  group->rule = rule;
  va_start(ap,rule);
  while((top = static_cast<TOP>(va_arg(ap,TOP))) != stop) {
    group->tops.push_back(top);
    if(top_specified[top]) {
      fprintf(stderr,"%s:%d: ### Fatal error: TOP '%s' got already a rule %s. Cannot take current rule: %s.\n",
	      __FILE__,__LINE__,
	      TOP_Name(top),top2group[top]->rule->name,rule->name);
      exit(EXIT_FAILURE);
    }
    top_specified[top] = true;
    top2group[top] = group;
  }
  va_end(ap);
  all_syntax_groups.push_back(group);
  return;
}

/* ====================================================================
 *   Emit_Printed_Syntax
 * ====================================================================
 */
static void Emit_Printed_Syntax() {
  list<ISA_SYNTAX_GROUP>::iterator group_iter;
  list<ISA_SYNTAX>::iterator syntax_iter;

  if(gen_static_code) {

    // We assume that macros are common to
    // static and dynamic code generation.
    Emit_Definitions (hfile, "ISA_PRINT_");

    fprintf(hfile,
	    "\n"
	    "typedef struct {\n"
	    "  char    *name;\n"
	    "  mUINT64 value;\n"
	    "} ISA_PRINT_OPND_INFO;\n");
	    

    fprintf(hfile,"\ntypedef int (* ISA_PRINT_INFO_print) ( void * stream, const char * fmt, ...);\n");

    fprintf(hfile,"\ntypedef INT (*ISA_PRINT_INFO)( ISA_PRINT_INFO_print print_func, void *stream, TOP topcode, const char *asmname, ISA_PRINT_OPND_INFO *result, ISA_PRINT_OPND_INFO *opnd );\n");
	    
    fprintf(hfile,"\nBE_EXPORTED extern const ISA_PRINT_INFO *ISA_PRINT_info;\n");
    fprintf(hfile,"\nBE_EXPORTED extern unsigned int ISA_PRINT_info_size;\n\n");

    fprintf(hfile,"\nBE_EXPORTED extern INT ISA_PRINT_Inst( ISA_PRINT_INFO_print print_func, void * stream, TOP topcode, const char *asmname, ISA_PRINT_OPND_INFO *result, ISA_PRINT_OPND_INFO *opnd );\n");
    
    fprintf(efile, "ISA_PRINT_info\n");
    fprintf(efile, "ISA_PRINT_info_size\n");

    fprintf(cfile ,
	    "\nINT ISA_PRINT_Inst( ISA_PRINT_INFO_print print_func, void * stream, TOP topcode, const char *asmname, ISA_PRINT_OPND_INFO *result, ISA_PRINT_OPND_INFO *opnd )\n"
	    "{\n"
	    "  INT index = ISA_PRINT_info_index[(INT)topcode];\n"
	    "  if(index == 0) { return -1; }\n"
	    "  return ISA_PRINT_info[index](print_func,stream,topcode,asmname,result,opnd);\n"
	    "}\n");
    
  } // Emission of static table.

  fprintf(cfile,"\n");
  fprintf(cfile,"#define O_VAL(id) opnd[id].value\n");
  fprintf(cfile,"#define O_NAME(id) opnd[id].name\n");
  fprintf(cfile,"#define R_VAL(id) result[id].value\n");
  fprintf(cfile,"#define R_NAME(id) result[id].name\n");

  for(group_iter = all_syntax_groups.begin(); group_iter != all_syntax_groups.end(); group_iter++) {
    ISA_SYNTAX_GROUP group = *group_iter;
    ISA_SYNTAX_RULE rule = group->rule;
    fprintf (cfile,
	     "\nstatic INT %s("
	     "ISA_PRINT_INFO_print print_func, "
	     "void *stream, "
	     "TOP topcode, "
	     "const char *asmname, "
	     "ISA_PRINT_OPND_INFO *result, "
	     "ISA_PRINT_OPND_INFO *opnd )\n"
	     "{\n"
	     "  INT res = -1;\n",
	     group->name);

/* Start of macro definition */
#define PRINTF_FUNC_MACRO(x) \
    for(syntax_iter = rule->syntaxes.begin();\
	syntax_iter != rule->syntaxes.end();\
	syntax_iter++) {\
      ISA_SYNTAX syntax = *syntax_iter;\
      list<ISA_SYNTAX_COMP>::iterator comp_iter;\
      const char *spaces = "  ";\
      if(syntax->print_condition != 0) {\
	fprintf(cfile,"%sif(%s) {\n",spaces,syntax->print_condition);\
	spaces = "    ";\
      }\
      fprintf (cfile, "%sres = %s \"%s\"", spaces,x,syntax->printed_syntax);\
      for(comp_iter = syntax->comps.begin();\
	  comp_iter != syntax->comps.end();\
	  comp_iter++) {\
	ISA_SYNTAX_COMP comp = *comp_iter;\
	\
	if(comp->type == COMP_NAME) {\
	  fprintf (cfile, ",\n\tasmname");\
	}\
	else if(COMP_OPND <= comp->type && comp->type < (COMP_OPND + MAX_OPNDS)) {\
	  if(comp->print_action != 0) {\
	    fprintf (cfile, ",\n\t%s",comp->print_action);\
	  }\
	  else {\
	    fprintf (cfile, ",\n\topnd[%d].name",comp->type-COMP_OPND);\
	  }\
	}\
	else if(COMP_RESULT <= comp->type && comp->type < (COMP_RESULT + MAX_RESULTS)) {\
	  if(comp->print_action != 0) {\
	    fprintf (cfile, ",\n\t%s",comp->print_action);\
	  }\
	  else {\
	    fprintf (cfile, ",\n\tresult[%d].name",comp->type-COMP_RESULT);\
	  }\
	}\
	else {\
	  fprintf (cfile, ",\n\t\"(bad operand or result)\"");\
	}\
      }\
      fprintf (cfile, ");\n");\
      if(syntax->print_condition != 0) {\
	spaces = "  ";\
	fprintf(cfile,"%s}\n",spaces);\
      }\
    }
/* End of macro definition */      

    fprintf (cfile, "if (NULL==print_func) {\n");
    PRINTF_FUNC_MACRO("printf(")
    fprintf (cfile, "} else {\n");
    PRINTF_FUNC_MACRO("print_func(stream,")
    fprintf (cfile, "}\n");

    fprintf (cfile, "  return res;\n");
    fprintf (cfile, "}\n");
  }

  // Emission of static table.
  if(gen_static_code) {
    fprintf(cfile, "\nstatic const ISA_PRINT_INFO ISA_PRINT_static_info[%d] = {\n",
	    all_syntax_groups.size() + 1);
  }
  // Emission of dynamic table.
  else {
    fprintf(cfile, "\nstatic const ISA_PRINT_INFO ISA_PRINT_dynamic_info[%d] = {\n",
	    all_syntax_groups.size());
  }


  // First empty element is reserved to static code table.
  if(gen_static_code) {
    fprintf (cfile, "\tNULL,\n");
  }

  for(group_iter = all_syntax_groups.begin(); group_iter != all_syntax_groups.end(); group_iter++) {
    ISA_SYNTAX_GROUP group = *group_iter;
    fprintf (cfile, "\t%s,\n",group->name);
  }
  fprintf (cfile, "\n};\n");

  if(gen_static_code)
   { fprintf(hfile,
	     "#define ISA_PRINT_info_static_size (%d)\n",
	     all_syntax_groups.size() + 1);
     fprintf(cfile,
             "\n"
             "const ISA_PRINT_INFO *ISA_PRINT_info      = ISA_PRINT_static_info;\n"
             "mUINT32               ISA_PRINT_info_size = %d;\n\n",
             all_syntax_groups.size());
   }
  else
   { const char * const routine1 = "dyn_get_ISA_PRINT_info_tab";
     const char * const routine2 = "dyn_get_ISA_PRINT_info_tab_sz";

     fprintf(cfile, 
             "\n"
             "const ISA_PRINT_INFO*\n"
             "%s ( void )\n"
             "{ return (const ISA_PRINT_INFO*) ISA_PRINT_dynamic_info;\n"
             "}\n"
             "\n",
             routine1
            );

     fprintf(cfile,
             "const mUINT32\n"
             "%s ( void )\n"
             "{ return (const mUINT32) %d;\n"
             "}",
             routine2,
             all_syntax_groups.size()
            );
                   
     fprintf(hfile,"\n"
                   "extern const ISA_PRINT_INFO* %s ( void );\n"
                   "extern const mUINT32 %s ( void );\n",
                   routine1,
                   routine2);
   }

  // ISA_PRINT_info_index used to be of type unsigned char*.
  // We transform it into unsigned short* to add room for extensions.
  if(gen_static_code)
   {  fprintf(hfile, 
        "\nBE_EXPORTED extern const unsigned short* ISA_PRINT_info_index;\n");
      fprintf(efile, "ISA_PRINT_info_index\n");
   }

  if(gen_static_code)
   fprintf(cfile, "\nstatic const mUINT16 ISA_PRINT_static_info_index[%d] = {\n", TOP_count_limit);
  else
   fprintf(cfile, "\nstatic const mUINT16 ISA_PRINT_dynamic_info_index[%d] = {\n",TOP_count_limit);

  const char *comma = ",";
  for (TOP top = 0; top < TOP_count_limit; ++top ) {
    ISA_SYNTAX_GROUP group = top2group[top];
    
    if(top == TOP_count_limit - 1) { comma = ""; }
    if (group) {
      fprintf(cfile, "  %3d%s  /* %s: %s */\n",
	      group->group_id,
	      comma,
	      TOP_Name(top),
	      group->name);
    } else {
      fprintf(cfile, "  %3d%s  /* %s */\n", 
	      0,
	      comma,
	      TOP_Name((TOP)top));
    }
  }
  fprintf(cfile, "};\n\n");

  if(gen_static_code)
  { fprintf(cfile,"const mUINT16 * ISA_PRINT_info_index = ISA_PRINT_static_info_index;\n\n");
  }
  else
  { const char * const routine_name = "dyn_get_ISA_PRINT_info_index_tab"; 

    fprintf(cfile,"\n"
                  "const mUINT16*\n"
                  "%s ( void )\n"
                  "{ return (const mUINT16*) ISA_PRINT_dynamic_info_index;\n"
                  "}\n\n",
                  routine_name);
    fprintf(hfile,"\n"
                  "extern const mUINT16 *%s (void);\n\n",
                  routine_name);
  }

  if(gen_static_code)
  { fprintf(hfile, "\ninline const ISA_PRINT_INFO ISA_PRINT_Info(TOP topcode)\n"
                 "{\n"
		 "  INT index = ISA_PRINT_info_index[(INT)topcode];\n"
		 "  return index == 0 ? 0 : ISA_PRINT_info[index];\n"
		 "}\n");

#if 0
   fprintf(hfile, "\ninline const char* ISA_PRINT_INFO_Format(const ISA_PRINT_INFO *info)\n"
		 "{\n"
		 "  return info->format;\n"
		 "}\n");

   fprintf(hfile, "\ninline INT ISA_PRINT_INFO_Comp(const ISA_PRINT_INFO *info, INT index)\n"
		 "{\n"
		 "  return info->comp[index];\n"
		 "}\n");
#endif
  }

  if (asmname) {
    // Printing name table.
    if(gen_static_code)
      { fprintf(cfile, 
         "\nstatic const char * const ISA_PRINT_static_asmname[] = {\n");
      }
    else
      { fprintf(cfile,
         "\nstatic const char * const ISA_PRINT_dynamic_asmname[] = {\n");
      }

    for (TOP top = 0; top < TOP_count_limit; ++top) {
      fprintf(cfile, "  \"%s\",\n", asmname(top));
    }

    if(gen_static_code)
      fprintf(cfile, "  \"UNDEFINED\"\n");  // UNDEFINED item reserved to static code gen.
    fprintf(cfile, "};\n");                 // Ending the table.

    if(gen_static_code)
     { fprintf(cfile, "\nBE_EXPORTED const char * const * ISA_PRINT_asmname = ISA_PRINT_static_asmname;\n");
	   fprintf(hfile, "\nBE_EXPORTED extern const char * const * ISA_PRINT_asmname;\n");
       fprintf(efile, "ISA_PRINT_asmname\n");
     }
    else
     { const char* const routine_name = "dyn_get_ISA_PRINT_name_tab";

       fprintf(cfile,"\n"
                     "const char**\n"
                     "%s ( void )\n"
                     "{ return (const char**) ISA_PRINT_dynamic_asmname;\n"
                     "}\n"
                     "\n"
                     "\n",
                     routine_name);
       fprintf(hfile,"extern const char** %s (void );\n",
                     routine_name);
     }

    // Inline routine is reserved for static code.
    if(gen_static_code) {
      fprintf(hfile, "\ninline const char *ISA_PRINT_AsmName(TOP topcode)\n"
                     "{\n"
                     "  BE_EXPORTED extern const char * const * ISA_PRINT_asmname;\n"
                     "  return ISA_PRINT_asmname[(INT)topcode];\n"
                     "}\n");
    }                                      // gen_static_code

  } else {                                 // else (!asmname)
    // Inline routine is reserved for static code.
     if(gen_static_code) {
       fprintf(hfile, "\ninline const char *ISA_PRINT_AsmName(TOP topcode)\n"
                      "{\n"
                      "  return TOP_Name(topcode);\n"
                      "}\n");
      }
  }

#if 0
  //
  // Emit function ISA_PRINT_Operand_Is_Part_Of_Name:
  // (Only for static code).
  if(gen_static_code) {
    fprintf(hfile, "\nBE_EXPORTED extern BOOL ISA_PRINT_Operand_Is_Part_Of_Name(TOP topcode, INT opindex);\n");
    fprintf(efile, "ISA_PRINT_Operand_Is_Part_Of_Name\n");
    fprintf(cfile, 
	    "\nBOOL ISA_PRINT_Operand_Is_Part_Of_Name(TOP topcode, INT opindex)\n"
	    "{\n"
	    "  const ISA_PRINT_INFO *info = ISA_PRINT_Info(topcode);\n"
	    "  const char *place_in_format = ISA_PRINT_INFO_Format(info);\n"
	    "  BOOL in_name_part = 0;\n"
	    "  INT comp;\n"
	    "  INT i = 0;\n"
	    "  for (;;) {\n"
	    "  	comp = ISA_PRINT_INFO_Comp(info,i);\n"
	    "  	if (comp == ISA_PRINT_COMP_end) break;\n"
	    "	place_in_format = strchr(place_in_format, '%%');\n"
	    "	place_in_format += 2; /* assume %%s */\n"
	    "  	if (comp == ISA_PRINT_COMP_name) {\n"
	    "	  if (*place_in_format == '\\0' || *place_in_format == ' ')\n"
	    "		in_name_part = 0;\n"
	    "	  else\n"
	    "		in_name_part = 1;\n"
	    "  	}\n"
	    "  	if (comp >= ISA_PRINT_COMP_opnd && comp < ISA_PRINT_COMP_result) {\n"
	    "  	  if (in_name_part) {\n"
	    "	    INT comp_opindex = comp - ISA_PRINT_COMP_opnd;\n"
	    "	    if (comp_opindex == opindex)\n"
	    "		return 1;\n"
	    "	    if (*place_in_format == '\\0' || *place_in_format == ' ')\n"
	    "		in_name_part = 0;\n"
	    "  	  }\n"
	    "  	}\n"
	    "  	++i;\n"
	    "  }\n"
	    "  return 0;\n"
	    "}\n");
  }
#endif
}

typedef enum {
  OPND_TYPE_UNDEF,
  OPND_TYPE_ON_STACK,
  OPND_TYPE_PARSE_OPERAND,
  OPND_TYPE_RELOC_OPERAND,
  OPND_TYPE_ENUM_OPERAND,
  OPND_TYPE_IMPLICIT
} ISA_SYNTAX_Parsed_Opnd_Type;

typedef struct {
  ISA_SYNTAX_Parsed_Opnd_Type type;
  union {
    INT   index;
    const char *parse_operand;
    char *tmp_var;
    char *reloc_operand;
    char *enum_operand;
  } u1;
} ISA_SYNTAX_Parsed_Opnd;

static char *tmp_var = "tn";
static int tmp_var_idx = 0;

static void
generate_implicits( FILE *file, TOP top, bool result, ISA_SYNTAX_Parsed_Opnd *opnd_types, const ISA_OPERAND_VALTYP *valtype, INT opnd_type_idx)
{
  if(!opnd_types[opnd_type_idx].type == OPND_TYPE_UNDEF) {
    fprintf(stderr,"%s:%d: ### Fatal error: Operand type is already defined\n",
	    __FILE__,__LINE__);
    exit(EXIT_FAILURE);
  }

  if(!ISA_OPERAND_VALTYP_Is_Register(valtype)) {
    fprintf(stderr,"%s:%d: ### Fatal error: TOP: TOP_%s Implicit %s %d is not of type register\n",
	    __FILE__,__LINE__,
	    TOP_Name(top),
	    result?"result":"operand",
	    result?opnd_type_idx:opnd_type_idx-MAX_RESULTS);
    exit(EXIT_FAILURE);
  }

  ISA_REGISTER_CLASS rc = ISA_OPERAND_VALTYP_Register_Class(valtype);
  ISA_REGISTER_SUBCLASS sc = ISA_OPERAND_VALTYP_Register_Subclass(valtype);
  const ISA_REGISTER_CLASS_INFO *rc_info = ISA_REGISTER_CLASS_Info(rc);
  const ISA_REGISTER_SUBCLASS_INFO *sc_info = ISA_REGISTER_SUBCLASS_Info(sc);
  ostringstream tmp_str;

  if(sc != ISA_REGISTER_SUBCLASS_UNDEFINED && 
     ISA_REGISTER_SUBCLASS_INFO_Count(sc_info) != 1) {
    fprintf(stderr,
	    "%s:%d: ### Fatal error: TOP: 'TOP_%s', %s %d cannot be syntaxically implicit: "
	    "ISA_REGISTER_SUBCLASS_%s contains %d registers\n"
	    "(can disable parsing of this top by setting parameter 'must_be_parsed' of 'ISA_Syntax_Rule_Create' routine to false)\n",
	    __FILE__,__LINE__,
	    TOP_Name(top),
	    result?"result":"operand",
	    result?opnd_type_idx:opnd_type_idx-MAX_RESULTS,
	    ISA_REGISTER_SUBCLASS_INFO_Name(sc_info),
	    ISA_REGISTER_SUBCLASS_INFO_Count(sc_info)	    
	    );
    exit(EXIT_FAILURE);
  }

  if(sc == ISA_REGISTER_SUBCLASS_UNDEFINED && 
     ISA_REGISTER_CLASS_INFO_Last_Reg(rc_info) != ISA_REGISTER_CLASS_INFO_First_Reg(rc_info)) {
    fprintf(stderr,
	    "%s:%d: ### Fatal error: TOP: 'TOP_%s', %s %d cannot be syntaxically implicit: "
	    "ISA_REGISTER_CLASS_%s contains %d registers\n"
	    "(can disable parsing of this top by setting parameter 'must_be_parsed' of 'ISA_Syntax_Rule_Create' routine to false)\n",
	    __FILE__,__LINE__,
	    TOP_Name(top),
	    result?"result":"operand",
	    result?opnd_type_idx:opnd_type_idx-MAX_RESULTS,
	    ISA_REGISTER_CLASS_INFO_Name(rc_info),
	    ISA_REGISTER_CLASS_INFO_Last_Reg(rc_info) - ISA_REGISTER_CLASS_INFO_First_Reg(rc_info) + 1
	    );
    exit(EXIT_FAILURE);
  }


  tmp_str << tmp_var << tmp_var_idx++;
  opnd_types[opnd_type_idx].type = OPND_TYPE_IMPLICIT;
  opnd_types[opnd_type_idx].u1.tmp_var = strdup((char*)tmp_str.str().data());
  fprintf(file,"  AIR_TN *%s = AIR_NEW_REG_TN(ISA_REGISTER_CLASS_%s,ISA_REGISTER_SUBCLASS_%s,0);\n",
	  opnd_types[opnd_type_idx].u1.tmp_var,ISA_REGISTER_CLASS_INFO_Name(rc_info),
	  ISA_REGISTER_SUBCLASS_INFO_Name(sc_info));
}



static void
print_parser_syntax( FILE *file, ISA_SYNTAX_Parsed_Opnd *opnd_types, ISA_SYNTAX syntax, TOP top ) {
  list<ISA_SYNTAX_COMP>::iterator comp_iter = syntax->comps.begin();
  list<ISA_SYNTAX_COMP>::iterator comp_iter_next = comp_iter;
  const ISA_OPERAND_INFO * oinfo = ISA_OPERAND_Info(top);
  ISA_SYNTAX_COMP comp = *comp_iter;
  INT opnd_idx = 0;

  fprintf(file,".Sy:");
  *(comp->pas_fd_pos) = '\0';
  fprintf(file,syntax->parsed_syntax);
  comp_iter_next++;
  while(comp_iter != syntax->comps.end()) {
    if(comp_iter_next != syntax->comps.end()) {
      comp = *comp_iter_next;
      *(comp->pas_fd_pos) = '\0';
    }
    comp = *comp_iter;
    *(comp->pas_fd_pos) = '%';
    if(comp->type == COMP_NAME) {
      fprintf(file,comp->pas_fd_pos,asmname(top));
    }
    else if(COMP_OPND <= comp->type && comp->type < (COMP_OPND + MAX_OPNDS)) {
      const ISA_OPERAND_VALTYP *valtype = ISA_OPERAND_INFO_Operand(oinfo,comp->type-COMP_OPND);
      if(comp->parse_rule != 0) {
	ostringstream rule_str;
	rule_str << "<" << comp->parse_rule << ">";
	fprintf (file,comp->pas_fd_pos,rule_str.str().data());
	if(comp->parse_operand == 0) {
	  fprintf(stderr,"%s:%d: ### Fatal error: TOP '%s'. When a parsing rule is specified, must specify also a parse_operand\n",
		__FILE__,__LINE__,
		TOP_Name(top));
	  exit(EXIT_FAILURE);
	}
	opnd_types[comp->type-COMP_OPND+MAX_RESULTS].type = OPND_TYPE_PARSE_OPERAND;
	opnd_types[comp->type-COMP_OPND+MAX_RESULTS].u1.parse_operand = comp->parse_operand;
      }
      else if(comp->parse_operand != 0) {
	fprintf(file,"%s",find_after_fd_pos(comp->pas_fd_pos,syntax->comps.begin() == comp_iter));
	opnd_types[comp->type-COMP_OPND+MAX_RESULTS].type = OPND_TYPE_PARSE_OPERAND;
	opnd_types[comp->type-COMP_OPND+MAX_RESULTS].u1.parse_operand = comp->parse_operand;
      }
      else if(ISA_OPERAND_VALTYP_Is_Relocatable(valtype)) {
	opnd_types[comp->type-COMP_OPND+MAX_RESULTS].type = OPND_TYPE_RELOC_OPERAND;
	opnd_types[comp->type-COMP_OPND+MAX_RESULTS].u1.reloc_operand = GET_Relocatable_Operand_Name(oinfo,comp->type - COMP_OPND);
	fprintf(file,comp->pas_fd_pos,GET_Relocatable_Operand_Rule(oinfo,comp->type - COMP_OPND));
      }
      else if(ISA_OPERAND_VALTYP_Is_Enum(valtype)) {
	opnd_types[comp->type-COMP_OPND+MAX_RESULTS].type = OPND_TYPE_ENUM_OPERAND;
	opnd_types[comp->type-COMP_OPND+MAX_RESULTS].u1.enum_operand = GET_Enum_Operand_Name(oinfo,comp->type - COMP_OPND);
	fprintf(file,comp->pas_fd_pos,GET_Enum_Operand_Rule(oinfo,comp->type - COMP_OPND));
      }
      else {
	fprintf(file,comp->pas_fd_pos,GET_Operand_Identifier(oinfo,comp->type - COMP_OPND));
	opnd_types[comp->type-COMP_OPND+MAX_RESULTS].type = OPND_TYPE_ON_STACK;
	opnd_types[comp->type-COMP_OPND+MAX_RESULTS].u1.index = opnd_idx;
	opnd_idx++;
      }
    }
    else if(COMP_RESULT <= comp->type && comp->type < (COMP_RESULT + MAX_RESULTS)) {
      const ISA_OPERAND_VALTYP *valtype = ISA_OPERAND_INFO_Operand(oinfo,comp->type-COMP_RESULT);
      if(comp->parse_rule != 0) {
	ostringstream rule_str;
	rule_str << "<" << comp->parse_rule << ">";
	fprintf (file,comp->pas_fd_pos,rule_str.str().data());
	if(comp->parse_operand == 0) {
	  fprintf(stderr,"%s:%d: ### Fatal error: TOP '%s'. When a parsing rule is specified, must specify also a parse_operand\n",
		__FILE__,__LINE__,
		TOP_Name(top));
	  exit(EXIT_FAILURE);
	}
	opnd_types[comp->type-COMP_RESULT].type = OPND_TYPE_PARSE_OPERAND;
	opnd_types[comp->type-COMP_RESULT].u1.parse_operand = comp->parse_operand;
      }
      else if(comp->parse_operand != 0) {
	fprintf(file,"%s",find_after_fd_pos(comp->pas_fd_pos,syntax->comps.begin() == comp_iter));
	opnd_types[comp->type-COMP_RESULT].type = OPND_TYPE_PARSE_OPERAND;
	opnd_types[comp->type-COMP_RESULT].u1.parse_operand = comp->parse_operand;
      }
      else {
	fprintf(file,comp->pas_fd_pos,GET_Result_Identifier(oinfo,comp->type - COMP_RESULT));
	opnd_types[comp->type-COMP_RESULT].type = OPND_TYPE_ON_STACK;
	opnd_types[comp->type-COMP_RESULT].u1.index  = opnd_idx;
	opnd_idx++;
      }
    }
    else {
      fprintf (file, ",\n\t\"(bad operand or result)\"");
    }
    comp_iter++;
    comp_iter_next++;
  }
  fprintf(file,"\n");
}
	
static void
print_parser_action( FILE *file, ISA_SYNTAX_Parsed_Opnd *opnd_types, ISA_SYNTAX syntax, TOP top ) {
  list<ISA_SYNTAX_COMP>::iterator comp_iter;
  const ISA_OPERAND_INFO * oinfo = ISA_OPERAND_Info(top);
  int operands = ISA_OPERAND_INFO_Operands(oinfo) + ISA_OPERAND_INFO_Results(oinfo);

  tmp_var_idx = 0;

  fprintf(file,".Act: {\n");

  if(syntax->parse_action != 0) {
    fprintf(file,"%s\n",syntax->parse_action);
  }

  for(int i=0; i<ISA_OPERAND_INFO_Results(oinfo); i++) {
    int operand_idx = i;
    if((operand_idx = ISA_OPERAND_INFO_Same_Res(oinfo,i)) != -1) {
      /* Update operand or result */
      if(opnd_types[i].type != OPND_TYPE_UNDEF && 
	 opnd_types[operand_idx+MAX_RESULTS].type == OPND_TYPE_UNDEF) {
	opnd_types[operand_idx+MAX_RESULTS] = opnd_types[i];
      }
      if(opnd_types[i].type == OPND_TYPE_UNDEF && 
	 opnd_types[operand_idx+MAX_RESULTS].type != OPND_TYPE_UNDEF) {
	opnd_types[i] = opnd_types[operand_idx+MAX_RESULTS];
      }
    }
    if(opnd_types[i].type == OPND_TYPE_UNDEF) {
      const ISA_OPERAND_VALTYP *valtype = ISA_OPERAND_INFO_Result(oinfo,i);
      if(!ISA_OPERAND_VALTYP_Is_Register(valtype)) {
	fprintf(stderr,"%s:%d: ### Fatal error: Result %d of TOP '%s' is not of type register\n",
		__FILE__,__LINE__,
		i,TOP_Name(top));
	exit(EXIT_FAILURE);
      }
      generate_implicits(file,top,true,opnd_types,valtype,i);
    }
  }

  for(int i=0; i<ISA_OPERAND_INFO_Operands(oinfo); i++) {
    if(opnd_types[i+MAX_RESULTS].type == OPND_TYPE_UNDEF) {
      const ISA_OPERAND_VALTYP *valtype = ISA_OPERAND_INFO_Operand(oinfo,i);
      generate_implicits(file,top,false,opnd_types,valtype,MAX_RESULTS+i);
    }
  }

  fprintf(file,"  ADD_INSTR((TOP_%s,%d",TOP_Name(top),operands);
  for(int i=0; i<ISA_OPERAND_INFO_Results(oinfo); i++) {
    int operand_idx = ISA_OPERAND_INFO_Same_Res(oinfo,i);
    if(operand_idx != -1) {
      operand_idx += MAX_RESULTS;
    }
    else {
      operand_idx = i;
    }
    if(opnd_types[operand_idx].type == OPND_TYPE_ON_STACK) {
      fprintf(file,",GETOPND(%d)",opnd_types[operand_idx].u1.index);
    }
    else if(opnd_types[operand_idx].type == OPND_TYPE_PARSE_OPERAND) {
      fprintf(file,",%s",opnd_types[operand_idx].u1.parse_operand);
    }
    else if(opnd_types[operand_idx].type == OPND_TYPE_IMPLICIT) {
      fprintf(file,",%s",opnd_types[operand_idx].u1.tmp_var);
    }
    else {
      /* OPND_TYPE_UNDEF */
      fprintf(stderr,"%s:%d: ### Fatal error: Unexpected undefined operand type for top '%s', result %d\n",
	      __FILE__,__LINE__,
	      TOP_Name(top),i);
      exit(EXIT_FAILURE);
    }
  }
  for(int i=0; i<ISA_OPERAND_INFO_Operands(oinfo); i++) {
    int operand_idx = i + MAX_RESULTS;
    if(opnd_types[operand_idx].type == OPND_TYPE_ON_STACK) {
      fprintf(file,",GETOPND(%d)",opnd_types[operand_idx].u1.index);
    }
    else if(opnd_types[operand_idx].type == OPND_TYPE_PARSE_OPERAND) {
      fprintf(file,",%s",opnd_types[operand_idx].u1.parse_operand);
    }
    else if(opnd_types[operand_idx].type == OPND_TYPE_RELOC_OPERAND) {
      fprintf(file,",%s",opnd_types[operand_idx].u1.reloc_operand);
    }
    else if(opnd_types[operand_idx].type == OPND_TYPE_ENUM_OPERAND) {
      fprintf(file,",%s",opnd_types[operand_idx].u1.enum_operand);
    }
    else if(opnd_types[operand_idx].type == OPND_TYPE_IMPLICIT) {
      fprintf(file,",%s",opnd_types[operand_idx].u1.tmp_var);
    }
    else {
      /* OPND_TYPE_UNDEF */
      fprintf(stderr,"%s:%d: ### Fatal error: Unexpected undefined operand type for top '%s', operand %d\n",
	      __FILE__,__LINE__,
	      TOP_Name(top),i);
      exit(EXIT_FAILURE);
    }
  }
  fprintf(file,"));\n");
  fprintf(file,"}\n");
}


/* ====================================================================
 *   Emit_Reloc_Opnd_Rules
 * ====================================================================
 */
static void Emit_Reloc_Opnd_rules( FILE *file, ISA_SUBSET subset )
{
  fprintf(file,"\n");
  fprintf(file,"\n/* ********************************************* */");
  fprintf(file,"\n/* Begin of reloc operand rules                  */");
  fprintf(file,"\n/* ********************************************* */");
  fprintf(file,"\n\n");


  for(int i=0; i<ISA_OPERAND_types_count;i++) {
    if(ISA_OPERAND_VALTYP_relocs[i] != 0) {
      char *reloc_name = 0;
      const ISA_OPERAND_VALTYP *valtype = &ISA_OPERAND_operand_types[i];
      ISA_RELOC reloc = ISA_OPERAND_VALTYP_First(valtype);
      bool default_reloc_emitted = false;

      fprintf(file,"%s:\n",ISA_OPERAND_VALTYP_relocs[i]->name);
      for(int j=0; j<ISA_OPERAND_VALTYP_Relocs(valtype);j++) {
	char *parsed_reloc = get_parsed_syntax(ISA_RELOC_Syntax(reloc));
	
	if(*parsed_reloc == '\0') {
	  fprintf(file,".Sy:%s\n",GET_Operand_Identifier_From_ValTyp(valtype,i));
	}
	else {
	  char *str_ptr;
	  int paren = 1;
	  for(str_ptr = strchr(parsed_reloc, '('); str_ptr != 0; str_ptr = strchr(str_ptr+1, '(')) {
	    paren++;									   
	  }
	  fprintf(file,".Sy:%s\\( %s ",parsed_reloc,GET_Operand_Identifier_From_ValTyp(valtype,i));
	  for(int x=0;x<paren;x++) {
	    fprintf(file,"\\)");
	  }
	  fprintf(file,"\n");
	}
	fprintf(file,".Act:{\n");
	fprintf(file,"  %s = GETOPND(0);\n",ISA_OPERAND_VALTYP_relocs[i]->name);
	if(ISA_OPERAND_VALTYP_Default_Reloc(valtype) == reloc) {
	  if(*parsed_reloc == '\0') {
	    fprintf(file,"  SET_AIR_TN_DEFAULT_RELOC(%s,ISA_%s);\n",ISA_OPERAND_VALTYP_relocs[i]->name,ISA_RELOC_Name(reloc));
	    default_reloc_emitted = true;
	  }
	  else {
	    fprintf(file,"  SET_AIR_TN_RELOC(%s,ISA_%s);\n",ISA_OPERAND_VALTYP_relocs[i]->name,ISA_RELOC_Name(reloc));
	  }
	}
	else {
	  fprintf(file,"  SET_AIR_TN_RELOC(%s,ISA_%s);\n",ISA_OPERAND_VALTYP_relocs[i]->name,ISA_RELOC_Name(reloc));
	}
	fprintf(file,"}\n\n");
	  
	reloc = ISA_OPERAND_VALTYP_Next(valtype,reloc);
      }
      if(!default_reloc_emitted) {
	reloc = ISA_OPERAND_VALTYP_Default_Reloc(valtype);
	fprintf(file,".Sy:%s\n",GET_Operand_Identifier_From_ValTyp(valtype,i));	
	fprintf(file,".Act:{\n");
	fprintf(file,"  %s = GETOPND(0);\n",ISA_OPERAND_VALTYP_relocs[i]->name);
	if(reloc != ISA_RELOC_UNDEFINED) {
	  fprintf(file,"  SET_AIR_TN_DEFAULT_RELOC(%s,ISA_%s);\n",ISA_OPERAND_VALTYP_relocs[i]->name,ISA_RELOC_Name(reloc));
	}
	fprintf(file,"}\n\n");
      }
    }
  }


  fprintf(file,
	  ".globdecl: {\n"
	  "#include \"targ_isa_registers.h\"\n"
	  "#include \"targ_isa_lits.h\"\n"
	  "#include \"targ_isa_relocs.h\"\n"
	  "#include \"topcode.h\"\n\n");

  fprintf(file,
	    "\n"
	    "/* Rename PARSER_GetParser to mangle it with subset name in static mode. */\n"
	    "#define PARSER_GetParser %s_PARSER_GetParser\n"
	    "\n",
	    ISA_SUBSET_Name(subset));

   fprintf(file,
	    "\n"
	    "/* Rename sccsid what string. */\n"
	    "#define sccsid %s_sccsid\n"
	    "\n",
	    ISA_SUBSET_Name(subset));

  fprintf(file,"/* AIR_TNs used to build relocatable operands.*/\n");
  for(int i=0; i<ISA_OPERAND_types_count;i++) {
    if(ISA_OPERAND_VALTYP_relocs[i] != 0) {
      fprintf(file,"static AIR_TN *%s = 0;\n",ISA_OPERAND_VALTYP_relocs[i]->name);
    }
  }
  fprintf(file,"\n");
  fprintf(file,"}\n");

  fprintf(file,"\n");
  fprintf(file,"\n/* ********************************************* */");
  fprintf(file,"\n/* End of reloc operand rules                    */");
  fprintf(file,"\n/* ********************************************* */\n\n");
}

/* ====================================================================
 *   Emit_Enum_Opnd_Rules
 * ====================================================================
 */
static void Emit_Enum_Opnd_rules( FILE *file, ISA_SUBSET subset )
{
  fprintf(file,"\n");
  fprintf(file,"\n/* ********************************************* */");
  fprintf(file,"\n/* Begin of enum operand rules                  */");
  fprintf(file,"\n/* ********************************************* */");
  fprintf(file,"\n\n");


  for(int i=0; i<ISA_OPERAND_types_count;i++) {
    if(ISA_OPERAND_VALTYP_enums[i] != 0) {
      char *enum_name = 0;
      const ISA_OPERAND_VALTYP *valtype = &ISA_OPERAND_operand_types[i];
      ISA_ENUM_CLASS eclass = ISA_OPERAND_VALTYP_Enum_Class(valtype);

      fprintf(file,"%s:\n",ISA_OPERAND_VALTYP_enums[i]->name);
      for(ISA_ENUM_CLASS_VALUE evalue=ISA_EC_First_Value(eclass);
	  evalue <= ISA_EC_Last_Value(eclass);
	  evalue++) {
	if(ISA_EC_Emit_Name(eclass)) {
	  fprintf(file,".Sy:%s\n",ISA_ECV_Name(evalue));
	}
	else {
	  fprintf(file,".Sy:%d\n",ISA_ECV_Intval(evalue));
	}
	fprintf(file,".Act:{\n");
	fprintf(file,"  %s = AIR_NEW_ENUM_TN(%s,%d);\n",
		ISA_OPERAND_VALTYP_enums[i]->name, ISA_EC_Name(eclass), evalue);
	fprintf(file,"}\n\n");
      }
    }
  }

  fprintf(file,
	  ".globdecl: {\n"
	  "#include \"targ_isa_enums.h\"\n"
	  "#include \"topcode.h\"\n\n");

  fprintf(file,"/* AIR_TNs used to build enum operands.*/\n");
  for(int i=0; i<ISA_OPERAND_types_count;i++) {
    if(ISA_OPERAND_VALTYP_enums[i] != 0) {
      fprintf(file,"static AIR_TN *%s = 0;\n",ISA_OPERAND_VALTYP_enums[i]->name);
    }
  }
  fprintf(file,"\n");
  fprintf(file,"}\n");

  fprintf(file,"\n");
  fprintf(file,"\n/* ********************************************* */");
  fprintf(file,"\n/* End of enum operand rules                    */");
  fprintf(file,"\n/* ********************************************* */\n\n");
}

/* ====================================================================
 *   Emit_Parser_Rules
 * ====================================================================
 */
static void Emit_Parser_Rules( FILE *file, ISA_SUBSET subset ) {
  list<ISA_SYNTAX_GROUP>::iterator group_iter;
  list<ISA_SYNTAX>::iterator syntax_iter;
  list<TOP>::iterator top_iter;
  list<ISA_SYNTAX_GROUP> isa_subset_groups;

  fprintf(file,"\n");
  fprintf(file,"\n/* ********************************************* */");
  fprintf(file,"\n/* Begin of generated rules                      */");
  fprintf(file,"\n/* ********************************************* */");
  fprintf(file,"\n\n");

  
  for(group_iter = all_syntax_groups.begin();
      group_iter != all_syntax_groups.end();
      group_iter++) {
    ISA_SYNTAX_GROUP group = *group_iter;
    ISA_SYNTAX_RULE rule = group->rule;
    bool has_isa_subset_member = false;

    if(rule->must_be_parsed) {
      top_iter = group->tops.begin();
      while(!has_isa_subset_member &&
	    top_iter != group->tops.end()) {
	TOP top = *top_iter;
	if(ISA_SUBSET_Member(subset,top)) {
	  has_isa_subset_member = true;
	}
	top_iter++;
      }
      
      if(has_isa_subset_member) {
	isa_subset_groups.push_back(group);
	fprintf(file,"%s:\n",rule->name);
	
	for(syntax_iter = rule->syntaxes.begin();
	    syntax_iter != rule->syntaxes.end();
	    syntax_iter++) {
	  ISA_SYNTAX syntax = *syntax_iter;
	  for(top_iter = group->tops.begin();
	      top_iter != group->tops.end();
	      top_iter++) {
	    TOP top = *top_iter;
	    if(ISA_SUBSET_Member(subset,top)) {
	      ISA_SYNTAX_Parsed_Opnd opnd_types[MAX_OPNDS+MAX_RESULTS];
	      memset(opnd_types,0,sizeof(ISA_SYNTAX_Parsed_Opnd)*(MAX_OPNDS+MAX_RESULTS));
	      print_parser_syntax(file,opnd_types,syntax,top);
	      print_parser_action(file,opnd_types,syntax,top);
	      fprintf(file,"\n");
	    }
	  }
	}
	fprintf(file,"\n");
      }
    }
  }

  fprintf(file,"\n");
  fprintf(file,"\n/* ********************************************* */");
  fprintf(file,"\n/* End of generated rules                        */");
  fprintf(file,"\n/* ********************************************* */\n\n");

  print_parser_root(file,subset,isa_subset_groups);
}

/////////////////////////////////////
void ISA_Syntax_Begin( void )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  // Whether we generate code for the core (static) or for an extension.
  gen_static_code = Is_Static_Code();

  // Get extension name or NULL for static code generation.
  char *extname = gen_static_code ? NULL : Get_Extension_Name();
  const char * const bname_c = FNAME_TARG_ISA_PRINT;
  const char * const bname_pr = FNAME_TARG_ISA_PARSE;

  hfilename = Gen_Build_Filename(bname_c,extname,gen_util_file_type_hfile);
  hfile     = Gen_Open_File_Handle(hfilename, "w");

  cfilename = Gen_Build_Filename(bname_c,extname,gen_util_file_type_cfile);
  cfile     = Gen_Open_File_Handle(cfilename, "w");


#ifdef DYNAMIC_CODE_GEN
  /* For dynamic extensions, we need to initialize tables
     that describe register files in a similar way to what
     is done by dynamic loader. Indeed, following code require
     that routines
     
     ISA_REGISTER_CLASS_INFO_Name(rcinfo),
     ISA_REGISTER_CLASS_SUBCLASS_INFO_Name(rcinfo),
     
     return correct results.
  */
  ISA_REGISTER_Initialize_Stub();
  ISA_OPERANDS_Initialize_Stub();
  ISA_RELOCS_Initialize_Stub();
  ISA_ENUMS_Initialize_Stub();
  ISA_LITS_Initialize_Stub();
#endif

  pfile = (FILE**)malloc(sizeof(FILE*)*ISA_SUBSET_count);
  memset(pfile,0,sizeof(FILE*)*ISA_SUBSET_count);
  pfilename = (char**)malloc(sizeof(char*)*ISA_SUBSET_count);
  memset(pfilename,0,sizeof(char*)*ISA_SUBSET_count);
  
  parse_reg_classes = (const char**)malloc(sizeof(char*)*ISA_SUBSET_count);
  memset(parse_reg_classes,0,sizeof(char*)*ISA_SUBSET_count);
  parse_lit_classes = (const char**)malloc(sizeof(char*)*ISA_SUBSET_count);
  memset(parse_lit_classes,0,sizeof(char*)*ISA_SUBSET_count);
  
  for(int i=0; i<ISA_SUBSET_count; i++) {
    char prefix[128];
    
    snprintf(prefix,sizeof(prefix),"%s_subset",(char*)ISA_SUBSET_Name(i));
    pfilename[i] = Gen_Build_Filename(bname_pr,prefix,gen_util_file_type_pfile);
    pfile[i]     = Gen_Open_File_Handle(pfilename[i], "w");
  }
  parsers = ISA_SUBSET_count;

    efilename = Gen_Build_Filename(bname_c,extname,gen_util_file_type_efile);
    efile     = Gen_Open_File_Handle(efilename, "w");
    dcfilename = Gen_Build_Filename(bname_pr,extname,gen_util_file_type_cfile);
    dcfile     = Gen_Open_File_Handle(dcfilename, "w");
    dhfilename = Gen_Build_Filename(bname_pr,extname,gen_util_file_type_hfile);
    dhfile     = Gen_Open_File_Handle(dhfilename, "w");
    Emit_C_Header(dcfile);     /* Emit ifdef _cplusplus directive */
    Emit_Header (dhfile, bname_pr, parser_driver_interface,extname);

  /* For dynamic extensions, we want to emit C and not C++     */
  Emit_C_Header(cfile);     /* Emit ifdef _cplusplus directive */
  fprintf(cfile,
          "\n"
	  "#include <stdio.h>\n"
          "#include <string.h>\n"
          "#include <stdarg.h>\n"
          "#include \"topcode.h\"\n");

  fprintf(cfile,"#include \"%s\"\n\n", 
          gen_static_code ? hfilename : "dyn_isa_print.h");

  Emit_Header (hfile, bname_c, isa_print_interface,extname);
//  fprintf(hfile,"#include <stdio.h>\n\n");
  fprintf(hfile,"#include \"targ_isa_registers.h\"\n");
  fprintf(hfile,"#include \"targ_isa_lits.h\"\n");
  fprintf(hfile,"#include \"targ_isa_enums.h\"\n");

  // We assume that macros are common to
  // static and dynamic code generation.
  if(gen_static_code) {
    Emit_Definitions (hfile, "ISA_PRINT_");
  }

  top2group = new ISA_SYNTAX_GROUP [TOP_count_limit + 1];
  for (unsigned int i=0; i<TOP_count_limit+1; i++) top2group[i] = NULL;
  top_specified = new bool   [TOP_count_limit  ];
  for (unsigned int i=0; i<TOP_count_limit; i++) top_specified[i] = false;

  ISA_OPERAND_VALTYP_identifiers = (char**)malloc(sizeof(char*)*ISA_OPERAND_types_count);
  memset(ISA_OPERAND_VALTYP_identifiers,0,sizeof(char*)*ISA_OPERAND_types_count);
  ISA_OPERAND_VALTYP_relocs = (Rule_Opnd_Type**)malloc(sizeof(Rule_Opnd_Type*)*ISA_OPERAND_types_count);
  memset(ISA_OPERAND_VALTYP_relocs,0,sizeof(Rule_Opnd_Type*)*ISA_OPERAND_types_count);
  ISA_OPERAND_VALTYP_enums = (Rule_Opnd_Type**)malloc(sizeof(Rule_Opnd_Type*)*ISA_OPERAND_types_count);
  memset(ISA_OPERAND_VALTYP_enums,0,sizeof(Rule_Opnd_Type*)*ISA_OPERAND_types_count);
}

/////////////////////////////////////
void Set_AsmName_Func(const char *(*asmname_func)(TOP topcode))
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  asmname = asmname_func;
}

/////////////////////////////////////
void Set_Mandatory_Space(const char *str ) {
  mspace = str;
}

/////////////////////////////////////
void Set_Optional_Space( const char *str ) {
  ospace = str;
}

/////////////////////////////////////
void Set_Optional_Printed_Space( const char *str ) {
  pspace = str;
}

/////////////////////////////////////
void Set_Tabulation( const char *str ) {
  tab = str;
}

/////////////////////////////////////
void ISA_Syntax_End( void )
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  Emit_Printed_Syntax();

  for(int i=0; i<parsers;i++) {
    Emit_Parser_Rules(pfile[i],i);
  }

  for(int i=0; i<parsers; i++) {
    Emit_Reloc_Opnd_rules(pfile[i],i);
  }

  for(int i=0; i<parsers; i++) {
    Emit_Enum_Opnd_rules(pfile[i],i);
  }

  for(int i=0; i<parsers;i++) {
    Emit_Valtypes_Identifiers(i,pfile[i]);
  }

  Emit_Parser_Driver();
		       
  Emit_Footer (hfile);
  Emit_C_Footer(cfile);

  /* Closing file handlers */
  Gen_Close_File_Handle(hfile,hfilename);
  Gen_Close_File_Handle(cfile,cfilename);
  for(int i=0; i<parsers;i++) {
    Gen_Close_File_Handle(pfile[i],pfilename[i]);
  }
  Gen_Close_File_Handle(efile,efilename);
  Gen_Close_File_Handle(dcfile,dcfilename);
  Gen_Close_File_Handle(dhfile,dhfilename);

  /* Memory deallocation */
  delete [] top2group;
  delete [] top_specified;

  Gen_Free_Filename(cfilename);
  Gen_Free_Filename(hfilename);
  for(int i=0; i<parsers;i++) {
    Gen_Free_Filename(pfilename[i]);
  }

  Gen_Free_Filename(efilename);
  Gen_Free_Filename(dcfilename);
  Gen_Free_Filename(dhfilename);

  free(pfile);
  pfile = 0;
  free(pfilename);
  pfilename = 0;

  free(parse_reg_classes);
  parse_reg_classes = 0;
  free(parse_lit_classes);
  parse_lit_classes = 0;

  for(int i=0;i<ISA_OPERAND_types_count;i++) {
    if(ISA_OPERAND_VALTYP_identifiers[i] != 0) {
      free(ISA_OPERAND_VALTYP_identifiers[i]);
      ISA_OPERAND_VALTYP_identifiers[i] = 0;
    }
    if(ISA_OPERAND_VALTYP_relocs[i] != 0) {
      free(ISA_OPERAND_VALTYP_relocs[i]->name);
      free(ISA_OPERAND_VALTYP_relocs[i]->rule);
      free(ISA_OPERAND_VALTYP_relocs[i]);
      ISA_OPERAND_VALTYP_relocs[i] = 0;
    }
    if(ISA_OPERAND_VALTYP_enums[i] != 0) {
      free(ISA_OPERAND_VALTYP_enums[i]->name);
      free(ISA_OPERAND_VALTYP_enums[i]->rule);
      free(ISA_OPERAND_VALTYP_enums[i]);
      ISA_OPERAND_VALTYP_enums[i] = 0;
    }
  }
  free(ISA_OPERAND_VALTYP_identifiers);
  free(ISA_OPERAND_VALTYP_relocs);
  free(ISA_OPERAND_VALTYP_enums);

  return;
}
