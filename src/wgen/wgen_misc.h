/*
 * Copyright (C) 2007, 2008, 2009 PathScale, LLC.  All Rights Reserved.
 */
/*
 * Copyright (C) 2006. QLogic Corporation. All Rights Reserved.
 */

/* 
   Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
   File modified June 20, 2003 by PathScale, Inc. to update Open64 C/C++ 
   front-ends to GNU 3.2.2 release.
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


/* translate gnu decl trees to whirl */

#ifndef wfe_util_INCLUDED
#define wfe_util_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

extern int  WGEN_Keep_Zero_Length_Structs;
#ifdef TARG_X8664
extern int Reg_Parm_Count;
extern BOOL SSE_Reg_Parm;
#endif

extern void WGEN_Init (INT argc, char **arrgv, char **envp);
extern void WGEN_Finish (void);
extern void WGEN_File_Init (INT argc, char **argv);
extern void WGEN_File_Finish (void);
extern void WGEN_Check_Errors (int *error_count, int *warning_count, BOOL *need_inliner);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include "wn.h"
#include "srcpos.h"
#ifdef TARG_ST // [CL]
#include "wgen_dst.h"
#endif


typedef enum {
  wgen_stmk_unknown,
  wgen_stmk_func_entry,
  wgen_stmk_func_pragmas,
  wgen_stmk_func_body,
  wgen_stmk_region_pragmas,
  wgen_stmk_region_body,
  wgen_stmk_region_exits,
  wgen_stmk_call_region_pragmas,	// not used currently
  wgen_stmk_call_region_body,
  wgen_stmk_call_region_exits,	// not used currently
  wgen_stmk_scope,
  wgen_stmk_if_cond,
  wgen_stmk_if_then,
  wgen_stmk_if_else,
  wgen_stmk_while_cond,
  wgen_stmk_while_body,
  wgen_stmk_dowhile_cond,
  wgen_stmk_dowhile_body,
  wgen_stmk_for_cond,
  wgen_stmk_for_body,
  wgen_stmk_switch,
  wgen_stmk_comma,
  wgen_stmk_rcomma,
  wgen_stmk_temp_cleanup,
  wgen_stmk_dummy,	// does not generate code
  wgen_stmk_last
} WGEN_STMT_KIND;
#ifdef TARG_ST
typedef struct wn_stmt {
  WN            *wn;
  WGEN_STMT_KIND  kind;
} WN_STMT;

extern WN_STMT *WGEN_Get_Stmt(void);
#endif

extern void WGEN_Stmt_Push (WN* wn, WGEN_STMT_KIND kind, SRCPOS srcpos);
extern WN*  WGEN_Stmt_Top (void);
#ifdef KEY
extern void WGEN_Stmt_Prepend_Last (WN* wn, SRCPOS srcpos);
extern void WGEN_Stmt_Append (WN* wn, SRCPOS srcpos, BOOL call_stmt = TRUE);
#else
extern void WGEN_Stmt_Append (WN* wn, SRCPOS srcpos);
#endif /* KEY */
extern WN*  WGEN_Stmt_Last (void);
extern WN*  WGEN_Stmt_Pop (WGEN_STMT_KIND kind);
#ifdef TARG_ST
// FdF 20070302
extern void WGEN_Stmt_Move_To_End(WN *first_wn, WN *last_wn);
#endif

extern void WGEN_Guard_Var_Push (void);
extern gs_t WGEN_Guard_Var_Pop (void);
extern gs_t WGEN_Get_Guard_Var (void);
extern bool Check_For_Call_Region (void);
extern bool Did_Not_Terminate_Region;
extern WN * WGEN_Find_Stmt_In_Stack (WGEN_STMT_KIND);
extern void Warning (const char *);

extern BOOL wgen_invoke_inliner;	/* from main.c */
extern char *asm_file_name;		/* from main.c */
extern BOOL TARGET_64BIT;		/* from main.c */
extern int lineno;			/* from main.c */
extern int key_exceptions;		/* from main.c */
extern BOOL opt_regions;		/* from main.c */
extern gs_t program;			/* from main.c */
extern BOOL lang_cplus;			/* from main.c */
#ifdef FE_GNU_4_2_0
#include "wn_util.h"
extern BOOL enable_cxx_openmp;		/* from main.c */
extern void WGEN_add_pragma_to_enclosing_regions (WN_PRAGMA_ID, ST *,
                                                  BOOL = FALSE);
void Add_Pragma_To_MP_Regions (WN_VECTOR *, WN_PRAGMA_ID, ST *, WN_OFFSET,
                               WN_MAP, BOOL, BOOL);
#endif
#if 0
extern char *Spin_File_Name;
extern FILE *Spin_File;
#endif
extern UINT current_file;		/* from wgen_dst.cxx */

// get the srcpos info from the global variable lineno
#ifdef TARG_ST
inline SRCPOS
Get_Srcpos (void)
{
  USRCPOS s;
  USRCPOS_clear(s);
  USRCPOS_filenum(s) = current_file;
  USRCPOS_linenum(s) = lineno; 
  return s.srcpos;
}
#else
inline SRCPOS
Get_Srcpos (void)
{
  SRCPOS s;
  SRCPOS_clear(s);
  SRCPOS_filenum(s) = current_file;
  SRCPOS_linenum(s) = lineno; 
  return s;
}
#endif
#ifdef TARG_ST
// [CL] get the srcpos info from the tree
inline SRCPOS
Get_Srcpos_From_Tree (gs_t node)
{
  // [SC] When manipulating fields of SRCPOS, be sure to use
  // USRCPOS, because SRCPOS field access breaks C aliasing rules.
  USRCPOS s;
  USRCPOS_clear(s);

  // [CL] We should not modify current_file here, but it is a side
  // effect of WGEN_Set_Line_And_File(), which we use to build the
  // dir/file table. Keep a copy to restore it afterwards.
  UINT current_file_cache = current_file;

  FmtAssert (node != NULL, 
	     ("Get_Srcpos_From_Tree called with NULL node"));

  /* Fabricated temporaries have no source coordinates, so take care here. */
  if (gs_operand (node, GS_DECL_SOURCE_FILE)
      &&gs_operand (node, GS_DECL_SOURCE_LINE)) {

    WGEN_Set_Line_And_File (gs_decl_source_line(node), gs_decl_source_file(node));
    USRCPOS_linenum(s) = gs_decl_source_line(node);

    // [CL] do not record artificial filenames (eg compiler-generated functions)
    if (gs_decl_source_file(node)
	&& (strcmp(gs_decl_source_file(node), "<built-in>")== 0
	    || strcmp(gs_decl_source_file(node), "<internal>") == 0) ) {
      current_file = 0;
      USRCPOS_linenum(s) = 0;
    }
  } else {
    current_file = 0;
    USRCPOS_linenum(s) = 0;
  }
  USRCPOS_filenum(s) = current_file;
  current_file = current_file_cache;
  return s.srcpos;
}
#endif

#endif // __cplusplus

#endif
