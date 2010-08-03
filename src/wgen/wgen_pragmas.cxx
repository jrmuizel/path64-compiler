/*
  Copyright (C) 2002, STMicroelectronics, All Rights Reserved.

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


extern "C"{
#include "gspin-wgen-interface.h"
}
#include "W_values.h"
#include <sys/types.h>
#include <elf.h>

#include <map>
#include "libiberty.h"

#include "defs.h"
#include "errors.h"
#include "glob.h"
#include "wn.h"
#include "wn_util.h"
#include "symtab.h"
#include "const.h"
#include "pu_info.h"
#include "ir_bwrite.h"
#include "ir_reader.h"
#include "erfe.h"
#include "wgen_dst.h"
#include "wgen_spin_symbol.h"
#include "wgen_decl.h"
#include "wgen_misc.h"
#include "wgen_expr.h"
#include "wgen_stmt.h"
#include "wgen_pragmas.h"

#include "wn_pragmas.h"

#include "ext_info.h"

//
// Pragma specification derived from wgen_pragmas.def
//
typedef struct wgen_pragma_spec {
  const char *name;		/* Pragma name. */
  int in_code;			/* True if it is a code statement pragma. */
  WN_PRAGMA_ID wn_id;      	/* Wn pragma identifier. */
  int   nargs;                  /* Number of arguments */
  const char *args_spec;	/* Arguments specification. */
  const char *usage;		/* Usage string. */
} WGEN_PRAGMA_SPEC;

#define DECL_WGEN_PRAGMA(id,name,in_code,wn_id,nargs,args_spec, usage) \
  { name, in_code, wn_id, nargs, args_spec, usage},
static const WGEN_PRAGMA_SPEC wgen_pragma_spec[] = {
#include "wgen_pragmas.def"
};
#undef  DECL_WGEN_PRAGMA

// Accessors
#define wgen_pragma_name(x) (wgen_pragma_spec[x].name)
#define wgen_pragma_in_code(x) (wgen_pragma_spec[x].in_code)
#define wgen_pragma_wn_id(x) (wgen_pragma_spec[x].wn_id)
#define wgen_pragma_nargs(x) (wgen_pragma_spec[x].nargs)
#define wgen_pragma_args_spec(x) (wgen_pragma_spec[x].args_spec)
#define wgen_pragma_usage(x) (wgen_pragma_spec[x].usage)


//
// Pragma specific srguments processing.
//

// Handle pragma loopdep arguments.
static int 
WGEN_PragmaArgs_LOOPDEP (const char *what, int *args)
{
  if (!what)
    return 1;

  if (!strcmp (what, "PARALLEL")) {
    args[0] = LOOPDEP_PARALLEL;
  }
  else if (!strcmp (what, "VECTOR")) {
    args[0] = LOOPDEP_VECTOR;
  }
  else if (!strcmp (what, "LIBERAL")) {
    args[0] = LOOPDEP_LIBERAL;
  }
  else {
    return 1;
  }
  
  return 0;
}

// Handle pragma frequency_hint arguments.
static int
WGEN_PragmaArgs_FREQ (const char *what, int *args)
{
  if (!what)
    return 1;

  if (!strcmp (what, "NEVER")) {
    args[0] = FREQUENCY_HINT_NEVER;
  }
  else if (!strcmp (what, "FREQUENT")) {
    args[0] = FREQUENCY_HINT_FREQUENT;
  }
#if 0 /* we don't support this one */
  else if (!strcmp (what, "INIT")) {
    args[0] = FREQUENCY_HINT_INIT;
  }
#endif
  else {
    return 1;
  }
  
  return 0;
}

// Handle pragma loopseq arguments.
static int
WGEN_PragmaArgs_LOOPSEQ (const char *what, int *args)
{
  if (!what)
    return 1;
  if (strcmp (what, "READ") == 0) {
    args[0] = 1;
  } else if (strcmp (what, "WRITE") == 0) {
    args[0] = 2;
  } else {
    return 1;
  }
  return 0;
}

// Handle pragma hwloop arguments.
static int
WGEN_PragmaArgs_HWLOOP (const char *what, int *args)
{
  if (!what)
    return 1;
  if (strcmp (what, "none") == 0) {
    args[0] = 0;
  } else if (strcmp (what, "forcehwloop") == 0) {
    args[0] = 1;
  } else if (strcmp (what, "forcejrgtudec") == 0) {
    args[0] = 2;
  } else {
    return 1;
  }
  return 0;
}

// Prepend pragma inline_pragma to list, and update the list head
static void
Prepend_Inline_Pragma(WN** list, WN* inline_pragma)
{
  WN_next(inline_pragma) = *list;
  if (*list)
    WN_prev(*list) = inline_pragma;
  *list = inline_pragma;
}

// Returns TRUE if call_wn has which_pragma in pragma_list
static WN*
Has_This_Inline_Pragma(const char* func_name, WN_PRAGMA_ID which_pragma, WN* pragma_list)
{

  WN* cur = pragma_list;
  while (cur) {
    TCON tc = STC_val(WN_st(cur));
    if (!strcmp(Targ_String_Address(tc), "*")
	|| !strcmp(Targ_String_Address(tc), func_name)) {
      if (WN_pragma(cur) == which_pragma) {
	return cur;
      }
    }
    cur = WN_next(cur);
  }

  return NULL;
}

static WN* callsite_pragma_list = NULL;  // list of callsite inline pragmas
static WN* function_pragma_list = NULL;  // list of function-scope inline pragmas
static WN* file_pragma_list = NULL;      // list of file-scope inline pragmas

// Externally visible helper functions
WN* Has_Callsite_Pragma_Inline(WN* call_wn)
{
  return Has_This_Inline_Pragma(ST_name(WN_st_idx(call_wn)),
				WN_PRAGMA_KAP_OPTION_INLINE,
				callsite_pragma_list);
}

WN* Has_Callsite_Pragma_NoInline(WN* call_wn)
{
  return Has_This_Inline_Pragma(ST_name(WN_st_idx(call_wn)),
				WN_PRAGMA_KAP_OPTION_NOINLINE,
				callsite_pragma_list);
}

WN* Has_Function_Pragma_Inline(WN* call_wn)
{
  return Has_This_Inline_Pragma(ST_name(WN_st_idx(call_wn)),
				WN_PRAGMA_KAP_OPTION_INLINE,
				function_pragma_list);
}

WN* Has_Function_Pragma_NoInline(WN* call_wn)
{
  return Has_This_Inline_Pragma(ST_name(WN_st_idx(call_wn)),
				WN_PRAGMA_KAP_OPTION_NOINLINE,
				function_pragma_list);
}

WN* Has_File_Pragma_Inline(WN* call_wn)
{
  return Has_This_Inline_Pragma(ST_name(WN_st_idx(call_wn)),
				WN_PRAGMA_KAP_OPTION_INLINE,
				file_pragma_list);
}

WN* Has_File_Pragma_NoInline(WN* call_wn)
{
  return Has_This_Inline_Pragma(ST_name(WN_st_idx(call_wn)),
				WN_PRAGMA_KAP_OPTION_NOINLINE,
				file_pragma_list);
}

//
// The following map and functions are used to associate a list of pragmas
// to a given function name. When translating to WHIRL, each list of pragmas
// will be attached to the pragma field of the corresponding function WN, if
// defined in the source file.
//
struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};
static std::map<const char*, WN*, ltstr> function_to_pragma_map;
typedef std::map<const char*, WN*, ltstr>::iterator pragma_map_iterator;

static void Clear_Pragma_Map (std::map<const char *, WN *, ltstr> &map) {
  pragma_map_iterator it;
  for (it = map.begin (); it != map.end () ; it++) {
    const char *name = it->first;
    WN *blk = it->second;
    free((void *)name);
    WN_DELETE_Tree (blk);
  }
  map.clear ();
}

// Associate a pragma WN to a function name
static void Associate_Pragma_To_Function_Name(WN *wn, const char *fname) {
  WN *blk;
  pragma_map_iterator it;
  it = function_to_pragma_map.find(fname);
  if (it == function_to_pragma_map.end()) {
     blk = WN_CreateBlock();
     function_to_pragma_map[fname] = blk;
  } else {
    blk = (*it).second;
  }
  WN_INSERT_BlockFirst(blk, wn);
}

// Return true if a pragma identified by id is associated to the function <fname>.
// Return false otherwise.
int Has_Function_Pragma(const char *fname, WN_PRAGMA_ID id) {
  pragma_map_iterator it;
  it = function_to_pragma_map.find(fname);
  if (it != function_to_pragma_map.end()) {
    WN *blk = (*it).second;
    for (WN *wn = WN_first(blk); wn; wn = WN_next(wn)) {
      if ((WN_PRAGMA_ID)WN_pragma(wn) == id) {
	return TRUE;
      }
    }
  }
  return 0;
}

// Return the block WN that contains pragma associated with function <fname>.
// Return NULL if not defined.
WN *Get_Function_Pragma_Block(const char *fname) {
  pragma_map_iterator it;
  it = function_to_pragma_map.find(fname);
  if (it != function_to_pragma_map.end()) {
    return (*it).second;
  }
  return NULL;
}

// Remove pragma from pragma_list and updates pragma_list if necessary
static void Remove_Pragma_Inline(WN** pragma_list, WN* pragma)
{
  WN* next = WN_next(pragma);
  WN* prev = WN_prev(pragma);
  if (next) {
    WN_prev(next) = prev;
  }
  if (prev) {
    WN_next(prev) = next;
  } else {
    *pragma_list = next;
  }
}

// Print a warning upon removal of a pragma, according to 'warn' level
static void Warn_Unused_Pragma(WN* pragma, CLEAR_INLINE_PRAGMA warn, char* scope)
{
  if (warn == SILENT) {
    return;
  }

  TCON tc = STC_val(WN_st(pragma));
  char* warn_msg = "";
  switch(warn) {
  case WARN:
    warn_msg = "matched no call";
    break;
  case DISCARD:
    warn_msg = "ignored (incorrect scope)";
    break;
  }

  switch(WN_pragma(pragma)) {
  case WN_PRAGMA_KAP_OPTION_INLINE:
  case WN_PRAGMA_KAP_OPTION_NOINLINE:
    ErrMsg (EC_Unused_Pragma, (WN_pragma(pragma) == WN_PRAGMA_KAP_OPTION_NOINLINE) ? "no" : "",
	    scope, Targ_String_Address(tc), warn_msg);
    break;
  }
}

// Delete pragma list 'list' and emit warnings depending on 'warn' level
static void Clear_Pragma_List(WN** list, CLEAR_INLINE_PRAGMA warn, char* scope)
{
  WN* cur = *list;
  WN* next = NULL;
  while (cur) {
    if (WN_pragma_arg1(cur) == 0) {
      Warn_Unused_Pragma(cur, warn, scope);
    }
    next = WN_next(cur);
    WN_Delete(cur);
    cur = next;
  }

  *list = NULL;
}

// Externally visible helper functions
void Clear_Callsite_Pragma_List(CLEAR_INLINE_PRAGMA warn)
{
  Clear_Pragma_List(&callsite_pragma_list, warn, "_next");
}

void Clear_Function_Pragma_List(CLEAR_INLINE_PRAGMA warn)
{
  Clear_Pragma_List(&function_pragma_list, warn, "_function");
}

void Clear_File_Pragma_List(CLEAR_INLINE_PRAGMA warn)
{
  Clear_Pragma_List(&file_pragma_list, warn, "_file");
}

// Reverts inlining policy to default for function 'name'
static void Default_Inline(const char* name, bool expanded_before)
{

  WN* old_pragma;
  if (old_pragma = Has_This_Inline_Pragma(name,
					  WN_PRAGMA_KAP_OPTION_INLINE,
					  callsite_pragma_list)) {
    TCON tc = STC_val(WN_st(old_pragma));
    // For inline_next, warn if the user overrides a pragma for an explicit name
    // For inline_function and inline_file, don't warn: that's the intended use
    if (!expanded_before) {
      ErrMsg (EC_Pragma_Override,
	      "default_inline",
	      name,
	      "inline_next",
	      !strcmp(Targ_String_Address(tc), "*") ? "" : name);
    }
    Remove_Pragma_Inline(&callsite_pragma_list, old_pragma);
  }
  if (old_pragma = Has_This_Inline_Pragma(name,
					  WN_PRAGMA_KAP_OPTION_NOINLINE,
					  callsite_pragma_list)) {
    TCON tc = STC_val(WN_st(old_pragma));
    if (!expanded_before) {
      ErrMsg (EC_Pragma_Override,
	      "default_inline",
	      name,
	      "noinlinenext",
	      !strcmp(Targ_String_Address(tc), "*") ? "" : name);
    }
  }

  if (old_pragma = Has_This_Inline_Pragma(name,
					  WN_PRAGMA_KAP_OPTION_INLINE,
					  function_pragma_list)) {
    TCON tc = STC_val(WN_st(old_pragma));
    if (!strcmp(Targ_String_Address(tc), "*")) {
      if (!expanded_before) {
	ErrMsg (EC_Pragma_Override,
		"default_inline",
		name,
		"inline_function",
		"");
      }
    }
    Remove_Pragma_Inline(&function_pragma_list, old_pragma);
  }
  if (old_pragma = Has_This_Inline_Pragma(name,
					  WN_PRAGMA_KAP_OPTION_NOINLINE,
					  function_pragma_list)) {
    TCON tc = STC_val(WN_st(old_pragma));
    if (!strcmp(Targ_String_Address(tc), "*")) {
      if (!expanded_before) {
	ErrMsg (EC_Pragma_Override, "default_inline", name, "noinline_function", "");
      }
    }
    Remove_Pragma_Inline(&function_pragma_list, old_pragma);
  }

  if (old_pragma = Has_This_Inline_Pragma(name,
					  WN_PRAGMA_KAP_OPTION_INLINE,
					  file_pragma_list)) {
    TCON tc = STC_val(WN_st(old_pragma));
    if (!strcmp(Targ_String_Address(tc), "*")) {
      if (!expanded_before) {
	ErrMsg (EC_Pragma_Override, "default_inline", name, "inline_file", "");
      }
    }
    Remove_Pragma_Inline(&file_pragma_list, old_pragma);
  }
  if (old_pragma = Has_This_Inline_Pragma(name,
					  WN_PRAGMA_KAP_OPTION_NOINLINE,
					  file_pragma_list)) {
    TCON tc = STC_val(WN_st(old_pragma));
    if (!strcmp(Targ_String_Address(tc), "*")) {
      if (!expanded_before) {
	ErrMsg (EC_Pragma_Override, "default_inline", name, "noinline_file", "");
      }
    }
    Remove_Pragma_Inline(&file_pragma_list, old_pragma);
  }
}

static void
WGEN_Pragma_Error(int p)
{
  ErrMsg (EC_Malformed_Pragma, wgen_pragma_usage(p));
}

void WGEN_Expand_Pragma(gs_t stmt, bool expanded_before)
{
  WGEN_Set_Line_And_File (gs_expr_lineno (stmt), gs_expr_filename (stmt));

  int wgen_pragma = (int) (gs_get_integer_value(gs_tree_operand(stmt, 0)) & 0xffff);
  const char *pragma_name =  wgen_pragma_name(wgen_pragma);
  gs_t pragma_args = gs_tree_operand(stmt, 1);

  if (wgen_pragma_in_code(wgen_pragma)) {
    if (!WGEN_Get_Stmt()) {
      if (!expanded_before) WGEN_Pragma_Error(wgen_pragma);
      return;
    }
  }
  
  WN *pwn;
  WN_PRAGMA_ID wn_pragma = WN_PRAGMA_UNDEFINED; // [CL] to handle [no]inline pragmas

  int args[2] = {0, 0};
  ST *arg_st = NULL;

  switch (wgen_pragma) {
    const char *what;

  case  WGEN_PRAGMA_UNROLL:
    args[0] = (int) gs_get_integer_value(gs_tree_value(pragma_args));
    break;
  
  case  WGEN_PRAGMA_LOOPMOD:
    args[0] = (int) gs_get_integer_value(gs_tree_value(pragma_args));
    if (gs_tree_chain(pragma_args))
      args[1] = (int) gs_get_integer_value(gs_tree_value(gs_tree_chain(pragma_args)));
    else
      args[1] = 0;
    break;

  case  WGEN_PRAGMA_PIPELINE:
    args[0] = (int) gs_get_integer_value(gs_tree_value(pragma_args));
    if (gs_tree_chain(pragma_args))
      args[1] = (int) gs_get_integer_value(gs_tree_value(gs_tree_chain(pragma_args)));
    else
      args[1] = -1;
    break;

  case  WGEN_PRAGMA_PRELOAD:
    args[0] = (int) gs_get_integer_value(gs_tree_value(pragma_args));
    if (gs_tree_chain(pragma_args))
      args[1] = (int) gs_get_integer_value(gs_tree_value(gs_tree_chain(pragma_args)));
    else
      args[1] = -1;
    break;

  case  WGEN_PRAGMA_IVDEP:
    break;

  case  WGEN_PRAGMA_LOOPDEP:
    what = gs_identifier_pointer (gs_tree_value(pragma_args));
    if (WGEN_PragmaArgs_LOOPDEP (what, args)) {
      if (! expanded_before) WGEN_Pragma_Error(wgen_pragma);
      return;
    }
    break;

  case  WGEN_PRAGMA_FREQUENCY_HINT:
    what = gs_identifier_pointer (gs_tree_value(pragma_args));
    if (WGEN_PragmaArgs_FREQ (what, args)) {
      if (!expanded_before) WGEN_Pragma_Error(wgen_pragma);
      return;
    }
    break;

  case WGEN_PRAGMA_INLINE_NEXT:
  case WGEN_PRAGMA_INLINE_FUNCTION:
  case WGEN_PRAGMA_INLINE_FILE:
    wn_pragma = WN_PRAGMA_KAP_OPTION_INLINE;
    break;

  case WGEN_PRAGMA_NOINLINE_NEXT:
  case WGEN_PRAGMA_NOINLINE_FUNCTION:
  case WGEN_PRAGMA_NOINLINE_FILE:
    wn_pragma = WN_PRAGMA_KAP_OPTION_NOINLINE;
    break;

  case WGEN_PRAGMA_DEFAULT_INLINE:
    {
      // Revert to default policy for each name supplied
      if (pragma_args != NULL) {
	for(gs_t arg = pragma_args; arg; arg = gs_tree_chain(arg)) {
	  const char *name = gs_identifier_pointer(gs_tree_value(arg));
	  Default_Inline(name, expanded_before);
	}
      } else {
	// Or remove all pragmas if no name is provided
	Clear_Callsite_Pragma_List(SILENT);
	Clear_Function_Pragma_List(SILENT);
	Clear_File_Pragma_List(SILENT);
      }
      return;
    }
    break;

  case  WGEN_PRAGMA_LOOPTRIP:
    args[0] = (int) gs_get_integer_value(gs_tree_value(pragma_args));
    break;
    
  case  WGEN_PRAGMA_LOOPSEQ:
    what = gs_identifier_pointer (gs_tree_value(pragma_args));
    if (WGEN_PragmaArgs_LOOPSEQ (what, args)) {
      if (!expanded_before) WGEN_Pragma_Error(wgen_pragma);
      return;
    }
    break;
  
  case  WGEN_PRAGMA_HWLOOP:
    what = gs_identifier_pointer (gs_tree_value(pragma_args));
    if (WGEN_PragmaArgs_HWLOOP (what, args)) {
      if (!expanded_before) WGEN_Pragma_Error(wgen_pragma);
      return;
    }
    if (args[0] == 1) {
      if (gs_tree_chain(pragma_args))
	args[1] = (int) gs_get_integer_value(gs_tree_value(gs_tree_chain(pragma_args)));
      else
	args[1] = -1;
    }
    break;
  
  case  WGEN_PRAGMA_LOOPMINITERCOUNT:
  case  WGEN_PRAGMA_LOOPMIN:
  case  WGEN_PRAGMA_LOOPMAXITERCOUNT:
  case  WGEN_PRAGMA_LOOPMAX:
    if (wgen_pragma == WGEN_PRAGMA_LOOPMINITERCOUNT) {
      if (! expanded_before) ErrMsg (EC_Obsolete_Pragma, pragma_name, "loopmin(n)");
    }
    if (wgen_pragma == WGEN_PRAGMA_LOOPMAXITERCOUNT) {
      if (! expanded_before) ErrMsg (EC_Obsolete_Pragma, pragma_name, "loopmax(n)");
    }
    args[0] = (int) gs_get_integer_value(gs_tree_value(pragma_args));
    break;
  
  case  WGEN_PRAGMA_STREAM_ALIGNMENT:
    args[0] = (int) gs_get_integer_value(gs_tree_value(pragma_args));
    break;

  case WGEN_PRAGMA_LOOPPACK:
    args[0] = (int) gs_get_integer_value(gs_tree_value(pragma_args));
    if ((args[0] < 0) || (args[0] > 2)) {
      if (!expanded_before) WGEN_Pragma_Error(wgen_pragma);
    }
    break;

  case WGEN_PRAGMA_FORCE_SPECIFIC_EXTGEN:
  case WGEN_PRAGMA_DISABLE_SPECIFIC_EXTGEN:
    // Those pragmas are similar to following ones, except that they take
    // an extension name as first argument
    {
      const char *extname = gs_identifier_pointer(gs_tree_value(pragma_args));
      if (!EXTENSION_Is_Defined(extname)) {
	if (! expanded_before) {
	  ErrMsg(EC_Pragma_Undefined_Extension, pragma_name, extname);
	}
	return;
      }
      args[0] = EXTENSION_Get_ExtensionId_From_ExtensionName(extname);
      pragma_args = gs_tree_chain(pragma_args);
    }
    // Fall-through
  case WGEN_PRAGMA_FORCE_EXTGEN:
  case WGEN_PRAGMA_DISABLE_EXTGEN:
    // Those pragmas take a list of function identifiers
    // They will not be generated in place, but instead will be duplicated
    // and attached to each of the specified functions.
    for(gs_t arg = pragma_args; arg; arg = gs_tree_chain(arg)) {
      const char *name = gs_identifier_pointer(gs_tree_value(arg));
      pwn = WN_CreatePragma(wgen_pragma_wn_id(wgen_pragma),
			    arg_st != NULL ? ST_st_idx(arg_st): (ST_IDX) NULL,
			    args[0], args[1]);
      Associate_Pragma_To_Function_Name(pwn, xstrdup(name));
    }
    return;
    break;


  case WGEN_PRAGMA_FORCE_EXTENSION_OPTION:
  case WGEN_PRAGMA_DISABLE_EXTENSION_OPTION:
    {
      const char *extname = gs_identifier_pointer(gs_tree_value(pragma_args));
      if (!EXTENSION_Is_Defined(extname)) {
	if (! expanded_before) {
	  ErrMsg(EC_Pragma_Undefined_Extension, pragma_name, extname);
	}

	return;
      }
      args[0] = EXTENSION_Get_ExtensionId_From_ExtensionName(extname);
      pragma_args = gs_tree_chain(pragma_args);
      const char *optionname = gs_identifier_pointer(gs_tree_value(pragma_args));
      
      args[1]=EXTENSION_Get_ExtOption_Flag_By_Name(args[0], optionname);

      pragma_args = gs_tree_chain(pragma_args);
      for(gs_t arg = pragma_args; arg; arg = gs_tree_chain(arg)) {
        const char *name = gs_identifier_pointer(gs_tree_value(arg));
        pwn = WN_CreatePragma(wgen_pragma_wn_id(wgen_pragma),
                              arg_st != NULL ? ST_st_idx(arg_st): (ST_IDX) NULL,
                              args[0], args[1]);
        Associate_Pragma_To_Function_Name(pwn, xstrdup(name));
      }
      return;
      break;
    }

  case  WGEN_PRAGMA_ASM_PARSE:
    args[0] = gs_get_integer_value(gs_tree_value(pragma_args));
    break;
    
  default:
    if (! expanded_before) DevWarn ("unsupported Pragma");
    return;
  }
  
  if (wn_pragma != WN_PRAGMA_UNDEFINED) {
    if (pragma_args != NULL) {
      // The user has provided a list of names, handle them specifically
      INT i;
      WN* old_pragma;
      struct name_list* next;

      for(gs_t arg = pragma_args; arg; arg = gs_tree_chain(arg)) {
	const char *name = gs_identifier_pointer(gs_tree_value(arg));
	TCON tcon = Host_To_Targ_String(MTYPE_STRING, xstrdup(name), strlen(name));
	TY_IDX ty_idx = Be_Type_Tbl(MTYPE_I1);
	ST* st = Gen_String_Sym(&tcon, ty_idx, FALSE);
	pwn = WN_CreatePragma(wn_pragma, ST_st_idx(st), args[0], args[1]);

	switch(wgen_pragma) {
	case WGEN_PRAGMA_INLINE_NEXT:
	case WGEN_PRAGMA_NOINLINE_NEXT:
	  // If the same pragma was already provided, ignore this one
	  if (old_pragma = Has_This_Inline_Pragma(name,
						  wgen_pragma == WGEN_PRAGMA_INLINE_NEXT ?
						  WN_PRAGMA_KAP_OPTION_INLINE : 
						  WN_PRAGMA_KAP_OPTION_NOINLINE,
						  callsite_pragma_list)) {
	    WN_Delete(pwn);
	  } else {
	    Prepend_Inline_Pragma(&callsite_pragma_list, pwn);
	    // If the reverse pragma was already provided for this callsite,
	    // warn the user and remove the previous one
	    if (wgen_pragma == WGEN_PRAGMA_INLINE_NEXT) {
	      if (old_pragma = Has_This_Inline_Pragma(name,
						      WN_PRAGMA_KAP_OPTION_NOINLINE,
						      callsite_pragma_list)) {
		Remove_Pragma_Inline(&callsite_pragma_list, old_pragma);
		TCON tc = STC_val(WN_st(old_pragma));
		if (!expanded_before) {
		  ErrMsg(EC_Pragma_Override, "inline_next", name, "noinline_next",
			 !strcmp(Targ_String_Address(tc), "*") ? "" : name);
		}
	      }
	    } else if (wgen_pragma == WGEN_PRAGMA_NOINLINE_NEXT) {
	      WN* old_pragma;
	      if (old_pragma = Has_This_Inline_Pragma(name,
						      WN_PRAGMA_KAP_OPTION_INLINE,
						      callsite_pragma_list)) {
		Remove_Pragma_Inline(&callsite_pragma_list, old_pragma);
		TCON tc = STC_val(WN_st(old_pragma));
		if (!expanded_before) {
		  ErrMsg(EC_Pragma_Override, "noinline_next", name, "inline_next",
			 !strcmp(Targ_String_Address(tc), "*") ? "" : name);
		}
	      }
	    }
	  }

	  break;
	case WGEN_PRAGMA_INLINE_FUNCTION:
	case WGEN_PRAGMA_NOINLINE_FUNCTION:
	  // If the same pragma was already provided, ignore this one
	  if (old_pragma = Has_This_Inline_Pragma(name,
						  wgen_pragma == WGEN_PRAGMA_INLINE_FUNCTION ?
						  WN_PRAGMA_KAP_OPTION_INLINE : 
						  WN_PRAGMA_KAP_OPTION_NOINLINE,
						  function_pragma_list)) {
	    WN_Delete(pwn);
	  } else {
	    Prepend_Inline_Pragma(&function_pragma_list, pwn);
	    // If the reverse pragma was already provided for this function,
	    // warn the user and remove the previous one
	    if (wgen_pragma == WGEN_PRAGMA_INLINE_FUNCTION) {
	      if (old_pragma = Has_This_Inline_Pragma(name,
						      WN_PRAGMA_KAP_OPTION_NOINLINE,
						      function_pragma_list)) {
		Remove_Pragma_Inline(&function_pragma_list, old_pragma);
		TCON tc = STC_val(WN_st(old_pragma));
		if (!expanded_before) {
		  ErrMsg(EC_Pragma_Override, "inline_function", name, "noinline_function",
			 !strcmp(Targ_String_Address(tc), "*") ? "" : name);
		}
	      }
	    } else if (wgen_pragma == WGEN_PRAGMA_NOINLINE_FUNCTION) {
	      WN* old_pragma;
	      if (old_pragma = Has_This_Inline_Pragma(name,
						      WN_PRAGMA_KAP_OPTION_INLINE,
						      function_pragma_list)) {
		Remove_Pragma_Inline(&function_pragma_list, old_pragma);
		TCON tc = STC_val(WN_st(old_pragma));
		if (!expanded_before) {
		  ErrMsg(EC_Pragma_Override, "noinline_function", name, "inline_function",
			 !strcmp(Targ_String_Address(tc), "*") ? "" : name);
		}
	      }
	    }
	  }
	  break;
	case WGEN_PRAGMA_INLINE_FILE:
	case WGEN_PRAGMA_NOINLINE_FILE:
	  // If the same pragma was already provided, ignore this one
	  if (old_pragma = Has_This_Inline_Pragma(name,
						  wgen_pragma == WGEN_PRAGMA_INLINE_FILE ?
						  WN_PRAGMA_KAP_OPTION_INLINE : 
						  WN_PRAGMA_KAP_OPTION_NOINLINE,
						  file_pragma_list)) {
	    WN_Delete(pwn);
	  } else {
	    Prepend_Inline_Pragma(&file_pragma_list, pwn);
	    // If the reverse pragma was already provided for this file,
	    // warn the user and remove the previous one
	    if (wgen_pragma == WGEN_PRAGMA_INLINE_FILE) {
	      if (old_pragma = Has_This_Inline_Pragma(name,
						      WN_PRAGMA_KAP_OPTION_NOINLINE,
						      file_pragma_list)) {
		Remove_Pragma_Inline(&file_pragma_list, old_pragma);
		TCON tc = STC_val(WN_st(old_pragma));
		if (!expanded_before) {
		  ErrMsg(EC_Pragma_Override, "inline_file", name, "noinline_file",
			 !strcmp(Targ_String_Address(tc), "*") ? "" : name);
		}
	      }
	    } else if (wgen_pragma == WGEN_PRAGMA_NOINLINE_FILE) {
	      WN* old_pragma;
	      if (old_pragma = Has_This_Inline_Pragma(name,
						      WN_PRAGMA_KAP_OPTION_INLINE,
						      file_pragma_list)) {
		Remove_Pragma_Inline(&file_pragma_list, old_pragma);
		TCON tc = STC_val(WN_st(old_pragma));
		if (!expanded_before) {
		  ErrMsg(EC_Pragma_Override, "noinline_file", name, "inline_file",
			 !strcmp(Targ_String_Address(tc), "*") ? "" : name);
		}
	      }
	    }
	  }
	  break;
	}
      }
    } else {
      // The user did not provide a list of function names:
      // create a special pragma that applies to all functions
      // clear possibly existing pragmas of the same level
      TCON tcon = Host_To_Targ_String(MTYPE_STRING, "*", strlen("*"));
      TY_IDX ty_idx = Be_Type_Tbl(MTYPE_I1);
      ST* st = Gen_String_Sym(&tcon, ty_idx, FALSE);
      pwn = WN_CreatePragma(wn_pragma, ST_st_idx(st), args[0], args[1]);

      switch(wgen_pragma) {
      case WGEN_PRAGMA_INLINE_NEXT:
      case WGEN_PRAGMA_NOINLINE_NEXT:
	if (callsite_pragma_list) {
	  Clear_Callsite_Pragma_List(SILENT);
	  if (!expanded_before) {
	    ErrMsg(EC_Pragma_Override_All,
		   (wgen_pragma == WGEN_PRAGMA_INLINE_NEXT) ? "inline_next" : "noinline_next",
		   "[no]inline_next");
	  }
	}
	Prepend_Inline_Pragma(&callsite_pragma_list, pwn);
	break;
      case WGEN_PRAGMA_INLINE_FUNCTION:
      case WGEN_PRAGMA_NOINLINE_FUNCTION:
	if (function_pragma_list) {
	  Clear_Function_Pragma_List(SILENT);
	  if (!expanded_before) {
	    ErrMsg(EC_Pragma_Override_All,
		   (wgen_pragma == WGEN_PRAGMA_INLINE_FUNCTION) ? "inline_function" : "noinline_function",
		   "[no]inline_function");
	  }
	}
	Prepend_Inline_Pragma(&function_pragma_list, pwn);
	break;
      case WGEN_PRAGMA_INLINE_FILE:
      case WGEN_PRAGMA_NOINLINE_FILE:
	if (file_pragma_list) {
	  Clear_File_Pragma_List(SILENT);
	  if (!expanded_before) {
	    ErrMsg(EC_Pragma_Override_All,
		   (wgen_pragma == WGEN_PRAGMA_INLINE_FILE) ? "inline_file" : "noinline_file",
		   "[no]inline_file");
	  }
	}
	Prepend_Inline_Pragma(&file_pragma_list, pwn);
	break;
      }
    }
  } else {
    FmtAssert(wgen_pragma_in_code(wgen_pragma),
	      ("Unexpected pragma %s inserted in code", pragma_name));
    pwn = WN_CreatePragma(wgen_pragma_wn_id(wgen_pragma),
			  arg_st != NULL ? ST_st_idx(arg_st): (ST_IDX) NULL,
			  args[0], args[1]);
    /* Insert in code. */
    WGEN_Stmt_Append (pwn, Get_Srcpos());
  }
  
  return;
}

static gs_t
list_reverse (gs_t head)
{
  gs_t l1 = head;
  gs_t l2 = NULL;
  while (l1 != NULL) {
    // Pop from l1.
    gs_t item = l1;
    l1 = gs_tree_chain (l1);
    // Push on l2.
    gs_set_tree_chain (item, l2);
    l2 = item;
  }
  return l2;
}

void
WGEN_Construct_Pragma_File_Context (gs_t wfe_pragma_context)
{
  // Clear out the current file context.
  Clear_File_Pragma_List(SILENT);
  Clear_Pragma_Map (function_to_pragma_map);
  
  // WFE_PRAGMA_CONTEXT is a linked list of the file-level
  // WFE pragmas that we have seen up until the context we
  // wish to recreate.
  // They are in reverse order, so the most recent pragma is
  // at the head of the list, and the oldest pragma is at
  // the tail.
  // Reverse the list, because we want to apply them in
  // source order.
  gs_t l2 = list_reverse (wfe_pragma_context);
  // Now l2 points to head of reversed list.
  // Apply the list rooted at l2.
  for (gs_t item = l2; item != NULL; item = gs_tree_chain(item)) {
    bool expanded_before = gs_tree_purpose (item) != NULL;
    WGEN_Expand_Pragma (gs_tree_value (item), expanded_before);
    if (!expanded_before) gs_set_tree_purpose (item, gs_build_int_cst (1));
  }
  // Now our context is reconstructed, but we must restore
  // the list.
  l2 = list_reverse (l2);
  FmtAssert (l2 == wfe_pragma_context, ("list reverse failure"));
}

    
