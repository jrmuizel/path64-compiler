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


// target-specific entry_exit code found for now in st100/cg/cg_targ.cxx

// PFS = Previous Function State
extern void EETARG_Save_Pfs (TN *saved_pfs, OPS *ops);

extern void EETARG_Restore_Pfs (TN *saved_pfs, OPS *ops);

// save and restore any extra callee-tns that need special processing
extern void EETARG_Save_Extra_Callee_Tns (OPS *ops);
extern void EETARG_Restore_Extra_Callee_Tns (OPS *ops);

// Do not save specific register classes.
extern BOOL EETARG_Do_Not_Save_Callee_Reg_Class( ISA_REGISTER_CLASS cl );

// target-specific adjustments to entry ops
extern void EETARG_Fixup_Entry_Code (BB *bb);
#ifdef TARG_ST
extern void EETARG_Fixup_Exit_Code (BB *bb);
extern void EETARG_Set_Frame_Len (INT64 frame_len);
#endif

#ifdef TARG_ST
// Should returns a super scratch (not allocatable) or defined register.
// This register will be used to initialize stack pointer in entry block.
extern TN *EETARG_get_temp_for_spadjust( BB *bb);
#endif

//  Replace the call OP with a jump.
extern OP *EETARG_Build_Jump_Instead_Of_Call(OP *call_op);

#ifdef USE_HIGH_LEVEL_PROCEDURE_EXIT
extern OP* EETARG_High_Level_Procedure_Exit();
#else
inline OP* EETARG_High_Level_Procedure_Exit() { return NULL; }
#endif

extern void EETARG_Init_Entry_Exit_Code (WN *, BOOL);

#ifdef TARG_ST
// Target specific stack frame fixup before finalizing the stack frame
// layout.
extern void EETARG_Fixup_Stack_Frame (void);
#endif
