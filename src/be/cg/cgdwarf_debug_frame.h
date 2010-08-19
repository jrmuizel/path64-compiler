/*

  Copyright (C) 2006 ST Microelectronics.  All Rights Reserved.

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

  Contact information:  , or:

  http://www.st.com

*/


/* ====================================================================
 * ====================================================================
 * Module: cgdwarf_debug_frame.h
 *
 * Description:
 *
 * Interface between cgemit.cxx and em_dwarf.cxx for all the
 * debug_frame related support
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef __CGDWARF_DEBUG_FRAME_H__
#define __CGDWARF_DEBUG_FRAME_H__

//#define DEBUG_UNWIND
#define PROPAGATE_DEBUG
//#define USE_UNREACHABLE

#include "cgdwarf_targ.h"
#include "cgir.h"
#include "tn_map.h"
#include "em_elf.h"
#include "em_dwarf.h" // Needed for DebugRegId defintion

extern BOOL Trace_Unwind;

//#define DEBUG_UNWIND
#define PROPAGATE_DEBUG
//#define USE_UNREACHABLE

// Unwind elements:
enum {
  UE_UNDEFINED,
  UE_CREATE_FRAME,
  UE_DESTROY_FRAME,
  UE_CREATE_FP,         // initialize FP
  UE_DESTROY_FP,        // restore FP
  UE_SAVE_GR,		// save a reg to a GR reg
  UE_SAVE_SP,		// save a reg to memory (sp)
  UE_SAVE_FP,		// save a reg to memory (fp)
  UE_RESTORE_GR,	// restore a reg from a GR reg
  UE_RESTORE_SP,	// restore a reg from memory (sp)
  UE_RESTORE_FP,	// restore a reg from memory (fp)
  UE_EPILOG,		// body epilog
  UE_LABEL,		// body label
  UE_COPY		// body copy 
};

typedef struct unwind_elem {
  mUINT32 when;
  mUINT32 when_bundle_start;    // address of the bundle start to
				// which this UE belongs
  BB *bb;
  mUINT8 kind;
  mUINT8 qp;			// reg number of qualifying predicate
  mUINT16 label;		// body label id
  DebugRegId rc_reg;	// reg whose state is changing
  DebugRegId save_rc_reg;	// reg being saved into
  LABEL_IDX label_idx;          // idx of label generated in asm file
  mINT64 offset;		// stack offset or frame size
  BOOL frame_changed;           // frame size changes in the same bundle
#ifdef PROPAGATE_DEBUG
  BOOL valid;                   // is this ue really really related to
				// a callee-save operation
				// (save/restore)?
  mINT64 top_offset;		// offset from SP at function entry
				// (useful when stack adjustment
				// occurs after saves)
  mUINT8 propagated;            // used internally when propagating
				// infos along the CFG
  BOOL is_copy;                 // is this ue a copy_state?
  BOOL after_sp_adj;            // is this ue after SP adjust?
  mINT64 frame_size;            // frame size at this point in PU
  BOOL handler;                 // belongs to an exception handler
#endif
} UNWIND_ELEM;

#ifdef PROPAGATE_DEBUG
// value for the 'propagated' field
enum {
  UE_PROP_NONE,
  UE_PROP_PUSHED,
  UE_PROP_POPPED
};
#endif

extern BOOL has_asm;

extern BOOL PU_has_FP;  // does the current PU use a frame pointer?
extern BOOL PU_has_restored_FP; // has the current PU already restored FP?

void Print_Unwind_Elem (UNWIND_ELEM ue, char *msg);

void Print_All_Unwind_Elem (char *msg);

BOOL TN_Is_Unwind_Reg (TN *tn);

OP* Find_Def_Of_TN (TN *tn, OP *cur_op, BB* bb);

TN* Get_Copied_Save_TN (TN *tn, OP *cur_op, BB *bb);

void Record_UE(OP* op, UNWIND_ELEM* ue, BB* bb, UINT when);

// call per-PU
void Init_Unwind_Info (BOOL trace);

void Finalize_Unwind_Info(void);

void Emit_Unwind_Directives_For_OP(OP *op, FILE *f, BOOL post_process,
				   BOOL inserted_late);
#endif
