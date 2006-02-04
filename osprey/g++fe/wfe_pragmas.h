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

/* WFE == WHIRL Front End 
 * Translate pragmas to whirl from gnu pragma trees.
 * Refer to wfe_pragmas.def for the WFE pragmas definitions.
 */

#ifndef wfe_pragma_INCLUDED
#define wfe_pragma_INCLUDED

#ifdef __cplusplus

/* c files in gnu should not reference direcly WN_PRAGMA names */
#include "wn_pragmas.h"

typedef enum {
  SILENT,
  DISCARD,
  WARN
} CLEAR_INLINE_PRAGMA;

// These functions are not calls from c files in gnu either
WN* Has_Callsite_Pragma_Inline(WN* call_wn);
WN* Has_Callsite_Pragma_NoInline(WN* call_wn);
WN* Has_Function_Pragma_Inline(WN* call_wn);
WN* Has_Function_Pragma_NoInline(WN* call_wn);
WN* Has_File_Pragma_Inline(WN* call_wn);
WN* Has_File_Pragma_NoInline(WN* call_wn);
void Clear_Callsite_Pragma_List(CLEAR_INLINE_PRAGMA warn);
void Clear_Function_Pragma_List(CLEAR_INLINE_PRAGMA warn);
void Clear_File_Pragma_List(CLEAR_INLINE_PRAGMA warn);

#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enumeration  of wfe pragma identifiers. */
#define DECL_WFE_PRAGMA(id,name,in_code,wn_id,nargs,args_spec,usage) id,
typedef enum {
#include "wfe_pragmas.def"
  MAX_WFE_PRAGMA		/* last+1 in enum */
} WFE_PRAGMA_ID;
#undef  DECL_WFE_PRAGMA

extern void WFE_Expand_Pragma (tree);

#ifdef __cplusplus
}
#endif

#endif

