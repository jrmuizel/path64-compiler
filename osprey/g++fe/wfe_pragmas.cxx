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


#include "W_values.h"
#include <sys/types.h>
#include <elf.h>

#include "gnu_config.h"
#include "gnu/flags.h"
extern "C" {
#include "gnu/system.h"
#include "gnu/tree.h"
#include "gnu/toplev.h"
#include "function.h"
#include "c-pragma.h"
#ifdef TARG_ST
  /* (cbr) 3.3 upgrade fix includes */
#include "gnu/real.h"
#endif
}

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
#include "tree_symtab.h"
#include "wfe_decl.h"
#include "wfe_misc.h"
#include "wfe_dst.h"
#include "wfe_expr.h"
#include "wfe_stmt.h"
#include "wfe_pragmas.h"

#include "wn_pragmas.h"

// #define WFE_DEBUG

//
// Pragma specification derived from wfe_pragmas.def
//
typedef struct wfe_pragma_spec {
  const char *name;		/* Pragma name. */
  int in_code;			/* True if it is a code statement pragma. */
  WN_PRAGMA_ID wn_id;      	/* Wn pragma identifier. */
  int   nargs;                  /* Number of arguments */
  const char *args_spec;	/* Arguments specification. */
  const char *usage;		/* Usage string. */
} WFE_PRAGMA_SPEC;

#define DECL_WFE_PRAGMA(id,name,in_code,wn_id,nargs,args_spec, usage) \
  { name, in_code, wn_id, nargs, args_spec, usage},
static const WFE_PRAGMA_SPEC wfe_pragma_spec[] = {
#include "wfe_pragmas.def"
};
#undef  DECL_WFE_PRAGMA

// Accessors
#define wfe_pragma_name(x) (wfe_pragma_spec[x].name)
#define wfe_pragma_in_code(x) (wfe_pragma_spec[x].in_code)
#define wfe_pragma_wn_id(x) (wfe_pragma_spec[x].wn_id)
#define wfe_pragma_nargs(x) (wfe_pragma_spec[x].nargs)
#define wfe_pragma_args_spec(x) (wfe_pragma_spec[x].args_spec)
#define wfe_pragma_usage(x) (wfe_pragma_spec[x].usage)


//
// Pragma specific srguments processing.
//

// Handle pragma loopdep arguments.
static int 
WFE_PragmaArgs_LOOPDEP (const char *what, int *args)
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
WFE_PragmaArgs_FREQ (const char *what, int *args)
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
WFE_PragmaArgs_LOOPSEQ (const char *what, int *args)
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
    warning ("#pragma %sinline%s (%s) %s",
	     (WN_pragma(pragma) == WN_PRAGMA_KAP_OPTION_NOINLINE) ? "no" : "",
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
static void Default_Inline(const char* name)
{

  WN* old_pragma;
  if (old_pragma = Has_This_Inline_Pragma(name,
					  WN_PRAGMA_KAP_OPTION_INLINE,
					  callsite_pragma_list)) {
    TCON tc = STC_val(WN_st(old_pragma));
    if (!strcmp(Targ_String_Address(tc), "*")) {
      warning ("#pragma defaultinline (%s) overrides previous #pragma inline_next ()", name);
    } else {
      // For inline_next, warn if the user overrides a pragma for an explicit name
      // For inline_function and inline_file, don't warn: that's the intended use
      warning ("#pragma defaultinline (%s) overrides previous #pragma inline_next (%s)", name, name);
    }
    Remove_Pragma_Inline(&callsite_pragma_list, old_pragma);
  }
  if (old_pragma = Has_This_Inline_Pragma(name,
					  WN_PRAGMA_KAP_OPTION_NOINLINE,
					  callsite_pragma_list)) {
    TCON tc = STC_val(WN_st(old_pragma));
    if (!strcmp(Targ_String_Address(tc), "*")) {
      warning ("#pragma defaultinline (%s) overrides previous #pragma noinline_next ()", name);
    } else {
      warning ("#pragma defaultinline (%s) overrides previous #pragma noinline_next (%s)", name, name);
    }
  }

  if (old_pragma = Has_This_Inline_Pragma(name,
					  WN_PRAGMA_KAP_OPTION_INLINE,
					  function_pragma_list)) {
    TCON tc = STC_val(WN_st(old_pragma));
    if (!strcmp(Targ_String_Address(tc), "*")) {
      warning ("#pragma defaultinline (%s) overrides previous #pragma inline_function ()", name);
    }
    Remove_Pragma_Inline(&function_pragma_list, old_pragma);
  }
  if (old_pragma = Has_This_Inline_Pragma(name,
					  WN_PRAGMA_KAP_OPTION_NOINLINE,
					  function_pragma_list)) {
    TCON tc = STC_val(WN_st(old_pragma));
    if (!strcmp(Targ_String_Address(tc), "*")) {
      warning ("#pragma defaultinline (%s) overrides previous #pragma noinline_function ()", name);
    }
    Remove_Pragma_Inline(&function_pragma_list, old_pragma);
  }

  if (old_pragma = Has_This_Inline_Pragma(name,
					  WN_PRAGMA_KAP_OPTION_INLINE,
					  file_pragma_list)) {
    TCON tc = STC_val(WN_st(old_pragma));
    if (!strcmp(Targ_String_Address(tc), "*")) {
      warning ("#pragma defaultinline (%s) overrides previous #pragma noinline_file ()", name);
    }
    Remove_Pragma_Inline(&file_pragma_list, old_pragma);
  }
  if (old_pragma = Has_This_Inline_Pragma(name,
					  WN_PRAGMA_KAP_OPTION_NOINLINE,
					  file_pragma_list)) {
    TCON tc = STC_val(WN_st(old_pragma));
    if (!strcmp(Targ_String_Address(tc), "*")) {
      warning ("#pragma defaultinline (%s) overrides previous #pragma noinline_file ()", name);
    }
    Remove_Pragma_Inline(&file_pragma_list, old_pragma);
  }
}

static void
WFE_Pragma_Error(int p)
{
  warning ("malformed pragma - ignored. Usage is: %s", wfe_pragma_usage(p));
}

void WFE_Expand_Pragma(tree stmt)
{
#ifdef WFE_DEBUG
  /* Prints pragma info. Usefull for debug. */
  const char *sep = " ";
  tree arg;
  fprintf(stdout, "WFE_Expand_Pragma: %s", wfe_pragma_name(Get_Integer_Value(TREE_OPERAND(stmt, 0))));
  for (arg = TREE_OPERAND (stmt, 1); arg; arg = TREE_CHAIN (arg)) {
    if (TREE_CODE(TREE_VALUE(arg)) == INTEGER_CST) {
      fprintf(stdout, "%s%lld", sep,Get_Integer_Value(TREE_VALUE(arg)));
    } else if (TREE_CODE(TREE_VALUE(arg)) == STRING_CST) {
      fprintf(stdout, "%s\"%s\"", sep,TREE_STRING_POINTER(TREE_VALUE(arg)));
    } else if (TREE_CODE(TREE_VALUE(arg)) == IDENTIFIER_NODE) {
      fprintf(stdout, "%s%s", sep,IDENTIFIER_POINTER(TREE_VALUE(arg)));
    } else {
      FmtAssert(0, ("unexpected tree code"));
    }
    sep = ", ";
  }
  fprintf(stdout, "\n");
#endif
  int wfe_pragma = Get_Integer_Value(TREE_OPERAND(stmt, 0));
  const char *pragma_name =  wfe_pragma_name(wfe_pragma);
  tree pragma_args = TREE_OPERAND(stmt, 1);

  if (wfe_pragma_in_code(wfe_pragma)) {
    if (!WFE_Get_Stmt()) {
      WFE_Pragma_Error(wfe_pragma);
      return;
    }
  }
  
  WN *pwn;
  WN_PRAGMA_ID wn_pragma = WN_PRAGMA_UNDEFINED; // [CL] to handle [no]inline pragmas

  int args[2] = {0, 0};
  ST *arg_st = NULL;

  switch (wfe_pragma) {
    const char *what;
    
  case  WFE_PRAGMA_IDENT:
    what = TREE_STRING_POINTER (TREE_VALUE(pragma_args));
    if (!what) 
      WFE_Pragma_Error(wfe_pragma);
    else {
      add_ident(what, 1);
    }
    return;

  case  WFE_PRAGMA_UNROLL:
    args[0] = Get_Integer_Value(TREE_VALUE(pragma_args));
    break;
  
  case  WFE_PRAGMA_LOOPMOD:
    args[0] = Get_Integer_Value(TREE_VALUE(pragma_args));
    if (TREE_CHAIN(pragma_args))
      args[1] = Get_Integer_Value(TREE_VALUE(TREE_CHAIN(pragma_args)));
    else
      args[1] = 0;
    break;

  case  WFE_PRAGMA_PIPELINE:
    args[0] = Get_Integer_Value(TREE_VALUE(pragma_args));
    if (TREE_CHAIN(pragma_args))
      args[1] = Get_Integer_Value(TREE_VALUE(TREE_CHAIN(pragma_args)));
    else
      args[1] = -1;
    break;

  case  WFE_PRAGMA_IVDEP:
    break;

  case  WFE_PRAGMA_LOOPDEP:
    what = IDENTIFIER_POINTER (TREE_VALUE(pragma_args));
    if (WFE_PragmaArgs_LOOPDEP (what, args)) {
      WFE_Pragma_Error(wfe_pragma);
      return;
    }
    break;

  case  WFE_PRAGMA_FREQUENCY_HINT:
    what = IDENTIFIER_POINTER (TREE_VALUE(pragma_args));
    if (WFE_PragmaArgs_FREQ (what, args)) {
      WFE_Pragma_Error(wfe_pragma);
      return;
    }
    break;

  case WFE_PRAGMA_INLINE_NEXT:
  case WFE_PRAGMA_INLINE_FUNCTION:
  case WFE_PRAGMA_INLINE_FILE:
    wn_pragma = WN_PRAGMA_KAP_OPTION_INLINE;
    break;

  case WFE_PRAGMA_NOINLINE_NEXT:
  case WFE_PRAGMA_NOINLINE_FUNCTION:
  case WFE_PRAGMA_NOINLINE_FILE:
    wn_pragma = WN_PRAGMA_KAP_OPTION_NOINLINE;
    break;

  case WFE_PRAGMA_DEFAULT_INLINE:
    {
      // Revert to default policy for each name supplied
      if (pragma_args != NULL) {
	for(tree arg = pragma_args; arg; arg = TREE_CHAIN(arg)) {
	  const char *name = IDENTIFIER_POINTER(TREE_VALUE(arg));
	  Default_Inline(name);
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

  case  WFE_PRAGMA_LOOPTRIP:
    args[0] = Get_Integer_Value(TREE_VALUE(pragma_args));
    break;
    
  case  WFE_PRAGMA_LOOPSEQ:
    what = IDENTIFIER_POINTER (TREE_VALUE(pragma_args));
    if (WFE_PragmaArgs_LOOPSEQ (what, args)) {
      WFE_Pragma_Error(wfe_pragma);
      return;
    }
    break;
  
  case  WFE_PRAGMA_STREAM_ALIGNMENT:
    args[0] = Get_Integer_Value(TREE_VALUE(pragma_args));
    break;
  
  default:
    DevWarn ("unsupported Pragma");
    return;
  }
  
  if (wn_pragma != WN_PRAGMA_UNDEFINED) {
    if (pragma_args != NULL) {
      // The user has provided a list of names, handle them specifically
      INT i;
      WN* old_pragma;
      struct name_list* next;

      for(tree arg = pragma_args; arg; arg = TREE_CHAIN(arg)) {
	const char *name = IDENTIFIER_POINTER(TREE_VALUE(arg));
	TCON tcon = Host_To_Targ_String(MTYPE_STRING, xstrdup(name), strlen(name));
	TY_IDX ty_idx = Be_Type_Tbl(MTYPE_I1);
	ST* st = Gen_String_Sym(&tcon, ty_idx, FALSE);
	pwn = WN_CreatePragma(wn_pragma, ST_st_idx(st), args[0], args[1]);

	switch(wfe_pragma) {
	case WFE_PRAGMA_INLINE_NEXT:
	case WFE_PRAGMA_NOINLINE_NEXT:
	  // If the same pragma was already provided, ignore this one
	  if (old_pragma = Has_This_Inline_Pragma(name,
						  wfe_pragma == WFE_PRAGMA_INLINE_NEXT ?
						  WN_PRAGMA_KAP_OPTION_INLINE : 
						  WN_PRAGMA_KAP_OPTION_NOINLINE,
						  callsite_pragma_list)) {
	    WN_Delete(pwn);
	  } else {
	    Prepend_Inline_Pragma(&callsite_pragma_list, pwn);
	    // If the reverse pragma was already provided for this callsite,
	    // warn the user and remove the previous one
	    if (wfe_pragma == WFE_PRAGMA_INLINE_NEXT) {
	      if (old_pragma = Has_This_Inline_Pragma(name,
						      WN_PRAGMA_KAP_OPTION_NOINLINE,
						      callsite_pragma_list)) {
		Remove_Pragma_Inline(&callsite_pragma_list, old_pragma);
		TCON tc = STC_val(WN_st(old_pragma));
		if (!strcmp(Targ_String_Address(tc), "*")) {
		  warning ("#pragma inline_next (%s) overrides previous #pragma noinline_next ()", name);
		} else {
		  warning ("#pragma inline_next (%s) overrides previous #pragma noinline_next (%s)", name, name);
		}
	      }
	    } else if (wfe_pragma == WFE_PRAGMA_NOINLINE_NEXT) {
	      WN* old_pragma;
	      if (old_pragma = Has_This_Inline_Pragma(name,
						      WN_PRAGMA_KAP_OPTION_INLINE,
						      callsite_pragma_list)) {
		Remove_Pragma_Inline(&callsite_pragma_list, old_pragma);
		TCON tc = STC_val(WN_st(old_pragma));
		if (!strcmp(Targ_String_Address(tc), "*")) {
		  warning ("#pragma noinline_next (%s) overrides previous #pragma inline_next ()", name);
		} else {
		  warning ("#pragma noinline_next (%s) overrides previous #pragma inline_next (%s)", name, name);
		}
	      }
	    }
	  }

	  break;
	case WFE_PRAGMA_INLINE_FUNCTION:
	case WFE_PRAGMA_NOINLINE_FUNCTION:
	  // If the same pragma was already provided, ignore this one
	  if (old_pragma = Has_This_Inline_Pragma(name,
						  wfe_pragma == WFE_PRAGMA_INLINE_FUNCTION ?
						  WN_PRAGMA_KAP_OPTION_INLINE : 
						  WN_PRAGMA_KAP_OPTION_NOINLINE,
						  function_pragma_list)) {
	    WN_Delete(pwn);
	  } else {
	    Prepend_Inline_Pragma(&function_pragma_list, pwn);
	    // If the reverse pragma was already provided for this function,
	    // warn the user and remove the previous one
	    if (wfe_pragma == WFE_PRAGMA_INLINE_FUNCTION) {
	      if (old_pragma = Has_This_Inline_Pragma(name,
						      WN_PRAGMA_KAP_OPTION_NOINLINE,
						      function_pragma_list)) {
		Remove_Pragma_Inline(&function_pragma_list, old_pragma);
		TCON tc = STC_val(WN_st(old_pragma));
		if (!strcmp(Targ_String_Address(tc), "*")) {
		  warning ("#pragma inline_function (%s) overrides previous #pragma noinline_function ()", name);
		  warning ("#pragma inline_function (%s) overrides previous #pragma noinline_function (%s)", name, name);
		} else {
		}
	      }
	    } else if (wfe_pragma == WFE_PRAGMA_NOINLINE_FUNCTION) {
	      WN* old_pragma;
	      if (old_pragma = Has_This_Inline_Pragma(name,
						      WN_PRAGMA_KAP_OPTION_INLINE,
						      function_pragma_list)) {
		Remove_Pragma_Inline(&function_pragma_list, old_pragma);
		TCON tc = STC_val(WN_st(old_pragma));
		if (!strcmp(Targ_String_Address(tc), "*")) {
		  warning ("#pragma noinline_function (%s) overrides previous #pragma inline_function ()", name);
		} else {
		  warning ("#pragma noinline_function (%s) overrides previous #pragma inline_function (%s)", name, name);
		}
	      }
	    }
	  }
	  break;
	case WFE_PRAGMA_INLINE_FILE:
	case WFE_PRAGMA_NOINLINE_FILE:
	  // If the same pragma was already provided, ignore this one
	  if (old_pragma = Has_This_Inline_Pragma(name,
						  wfe_pragma == WFE_PRAGMA_INLINE_FILE ?
						  WN_PRAGMA_KAP_OPTION_INLINE : 
						  WN_PRAGMA_KAP_OPTION_NOINLINE,
						  file_pragma_list)) {
	    WN_Delete(pwn);
	  } else {
	    Prepend_Inline_Pragma(&file_pragma_list, pwn);
	    // If the reverse pragma was already provided for this file,
	    // warn the user and remove the previous one
	    if (wfe_pragma == WFE_PRAGMA_INLINE_FILE) {
	      if (old_pragma = Has_This_Inline_Pragma(name,
						      WN_PRAGMA_KAP_OPTION_NOINLINE,
						      file_pragma_list)) {
		Remove_Pragma_Inline(&file_pragma_list, old_pragma);
		TCON tc = STC_val(WN_st(old_pragma));
		if (!strcmp(Targ_String_Address(tc), "*")) {
		  warning ("#pragma inline_file (%s) overrides previous #pragma noinline_file ()", name);
		} else {
		  warning ("#pragma inline_file (%s) overrides previous #pragma noinline_file (%s)", name, name);
		}
	      }
	    } else if (wfe_pragma == WFE_PRAGMA_NOINLINE_FILE) {
	      WN* old_pragma;
	      if (old_pragma = Has_This_Inline_Pragma(name,
						      WN_PRAGMA_KAP_OPTION_INLINE,
						      file_pragma_list)) {
		Remove_Pragma_Inline(&file_pragma_list, old_pragma);
		TCON tc = STC_val(WN_st(old_pragma));
		if (!strcmp(Targ_String_Address(tc), "*")) {
		  warning ("#pragma noinline_file (%s) overrides previous #pragma inline_file ()", name);
		} else {
		  warning ("#pragma noinline_file (%s) overrides previous #pragma inline_file (%s)", name, name);
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

      switch(wfe_pragma) {
      case WFE_PRAGMA_INLINE_NEXT:
      case WFE_PRAGMA_NOINLINE_NEXT:
	if (callsite_pragma_list) {
	  const char* pragma_name = (wfe_pragma == WFE_PRAGMA_INLINE_NEXT) ? "" : "no";
	  Clear_Callsite_Pragma_List(SILENT);
	  warning ("#pragma %sinline_next () overrides all previous #pragma [no]inline_next ()", pragma_name);
	}
	Prepend_Inline_Pragma(&callsite_pragma_list, pwn);
	break;
      case WFE_PRAGMA_INLINE_FUNCTION:
      case WFE_PRAGMA_NOINLINE_FUNCTION:
	if (function_pragma_list) {
	  const char* pragma_name = (wfe_pragma == WFE_PRAGMA_INLINE_FUNCTION) ? "" : "no";
	  Clear_Function_Pragma_List(SILENT);
	  warning ("#pragma %sinline_function () overrides all previous #pragma [no]inline_function ()", pragma_name);
	}
	Prepend_Inline_Pragma(&function_pragma_list, pwn);
	break;
      case WFE_PRAGMA_INLINE_FILE:
      case WFE_PRAGMA_NOINLINE_FILE:
	if (file_pragma_list) {
	  const char* pragma_name = (wfe_pragma == WFE_PRAGMA_INLINE_FILE) ? "" : "no";
	  Clear_File_Pragma_List(SILENT);
	  warning ("#pragma %sinline_file () overrides all previous #pragma [no]inline_file ()", pragma_name);
	}
	Prepend_Inline_Pragma(&file_pragma_list, pwn);
	break;
      }
    }
  } else {
    FmtAssert(wfe_pragma_in_code(wfe_pragma),
	      ("Unexpected pragma %s inserted in code", pragma_name));
    pwn = WN_CreatePragma(wfe_pragma_wn_id(wfe_pragma),
			  arg_st != NULL ? ST_st_idx(arg_st): (ST_IDX) NULL,
			  args[0], args[1]);
    /* Insert in code. */
    WFE_Stmt_Append (pwn, Get_Srcpos());
  }
  
  return;
}

struct idents * idents_strs;
void add_ident(const char * str, int comment)
{
  struct idents *id;
  id = (struct idents *) xmalloc (sizeof (struct idents));

  if (id) {
    id->next = idents_strs;
    id->string = str;
    id->comment = comment;
    idents_strs = id;
  }
}

#include "config_asm.h"

void
WFE_Idents (void)
{
  struct idents* id = idents_strs;
  while (id)
    {
      int stlen = strlen(id->string);
      int stlen2;
      stlen2 = id->comment ? sizeof(AS_COMMENT) : sizeof(AS_IDENT);
      char *new_str = (char*)malloc (stlen+stlen2+7);

      sprintf (new_str, "\t%s %s%s%s",
               id->comment ? AS_COMMENT : AS_IDENT, 
               id->string[0] == '"' ? "" : "\"",
               id->string,
               id->string[stlen-1] == '"' ? "" : "\"");
      WFE_Assemble_Asm (new_str);
      id = id->next;
      /* clarkes: This storage was allocated using permalloc,
       * so it cannot be freed using free.
       * free (idents_strs);
       */
      idents_strs = id;
    }
}

