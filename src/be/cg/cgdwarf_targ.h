/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */
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


#ifndef cgdwarf_targ_INCLUDED
#define cgdwarf_targ_INCLUDED

extern BOOL Trace_Dwarf;

/* construct the fde for the current procedure. */
#ifdef TARG_X8664
extern Dwarf_P_Fde Build_Fde_For_Proc (Dwarf_P_Debug  dw_dbg,
				       BB            *firstbb,
				       Dwarf_Unsigned      begin_label,
				       Dwarf_Unsigned      end_label,
				       Dwarf_Unsigned      pushbp_label,
				       Dwarf_Unsigned      movespbp_label,
				       Dwarf_Unsigned      adjustsp_label,
				       Dwarf_Unsigned      callee_saved_reg,
				       INT32          end_offset,
				       // The following two arguments
				       // need to go away once
				       // libunwind gives us an
				       // interface that supports
				       // symbolic ranges.
				       INT       low_pc,
				       INT       high_pc);
#elif defined(TARG_MIPS)
extern Dwarf_P_Fde Build_Fde_For_Proc (Dwarf_P_Debug  dw_dbg,
				       BB            *firstbb,
				       Dwarf_Unsigned      begin_label,
				       Dwarf_Unsigned      end_label,
				       Dwarf_Unsigned      adjustsp_label,
				       Dwarf_Unsigned      callee_saved_reg,
				       INT32          new_cfa_offset,
				       INT32          end_offset,
				       // The following two arguments
				       // need to go away once
				       // libunwind gives us an
				       // interface that supports
				       // symbolic ranges.
				       INT       low_pc,
				       INT       high_pc);
#elif TARG_ST
// [CL] changed begin/end_label type and added
// scn_index param to be used by the debug_frame
// machinery along with begin_label
extern Dwarf_P_Fde Build_Fde_For_Proc (Dwarf_P_Debug  dw_dbg,
				       BB            *firstbb,
				       Dwarf_Unsigned begin_label,
				       Dwarf_Unsigned end_label,
				       INT32          end_offset,
				       // The following two arguments
				       // need to go away once
				       // libunwind gives us an
				       // interface that supports
				       // symbolic ranges.
				       INT       low_pc,
				       INT       high_pc,
				       Elf64_Word	scn_index,
				       bool emit_restores=false);
#else
extern Dwarf_P_Fde Build_Fde_For_Proc (Dwarf_P_Debug  dw_dbg,
				       BB            *firstbb,
				       LABEL_IDX      begin_label,
				       LABEL_IDX      end_label,
				       INT32          end_offset,
				       // The following two arguments
				       // need to go away once
				       // libunwind gives us an
				       // interface that supports
				       // symbolic ranges.
				       INT       low_pc,
				       INT       high_pc);
#endif // TARG_X8664

extern void Check_Dwarf_Rel(const Elf32_Rel &);
extern void Check_Dwarf_Rel(const Elf64_AltRel &);
extern void Check_Dwarf_Rela(const Elf64_AltRela &);
extern void Check_Dwarf_Rela(const Elf32_Rela &);
extern BOOL Is_Dwarf_Section_To_Emit(const char *name);
extern BOOL Dwarf_Require_Symbolic_Offsets();

extern void Init_Unwind_Info (BOOL trace);
extern void Finalize_Unwind_Info(void);
#ifdef TARG_ST
// [CL] need to emit labels after bundles too
extern void Emit_Unwind_Directives_For_OP(OP *op, FILE *f, BOOL after_op,
					  BOOL inserted_late);
#else
extern void Emit_Unwind_Directives_For_OP(OP *op, FILE *f);
#endif

#ifdef TARG_ST
void Analyze_OP_For_Unwind_Info (OP *op, UINT when, BB *bb);
// enum of all preserved regs (PR) that can be saved/restored
typedef enum {
    PR_R6,
    PR_R7,
    PR_R8,
    PR_R9,
    PR_R10,
    PR_R11,
    PR_R24,
    PR_R25,
    PR_R26,
    PR_R27,
    PR_R28,
    PR_R29,
    PR_R30,
    PR_R31,
    PR_LK,
    PR_SP,
    PR_GR,
    PR_LC0,
    PR_LS0,
    PR_LE0,
    PR_LR0,
    PR_GP,
    PR_LAST
} PR_TYPE;
#define PR_FIRST PR_R6

PR_TYPE CR_To_PR (CLASS_REG_PAIR crp);
CLASS_REG_PAIR PR_To_CR (PR_TYPE p);

#endif

#endif /* cgdwarf_targ_INCLUDED */
