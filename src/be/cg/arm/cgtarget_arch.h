/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Copyright (C) STMicroelectronics All Rights Reserved.

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

*/

#ifndef targ_cg_INCLUDED
#define targ_cg_INCLUDED

#include "defs.h"

#define INST_BYTES 4

#define Is_Target_64bit() FALSE


#define HWloopRegisterLR0SFRRank 16
extern BOOL saveHWloopRegisterLR0;


/* ====================================================================
 *   Branch handling:
 * ====================================================================
 */
#define MIN_BRANCH_DISP             (2097151 * ISA_MAX_INST_BYTES)
#define DEFAULT_BRP_BRANCH_LIMIT    (256 * ISA_MAX_INST_BYTES)
#define DEFAULT_LONG_BRANCH_LIMIT   (2000000 * ISA_MAX_INST_BYTES)

// Given a conditional branch with a <branch_taken_probability>
// return TRUE if it would be beneficial to convert it to a brlikely.
inline BOOL 
CGTARG_Use_Brlikely(float branch_taken_probability)
{
  return FALSE;
}

/* ====================================================================
 *   Spilling:
 * ====================================================================
 */

extern UINT32 CGTARG_NUM_SPILL_TYPES;

/* When (Gen_GP_Relative && Is_Caller_Save_GP && !Constant_GP),
   the CGIR contains instructions to restore the GP after a
   function call.
   The LAO scheduler can migrate these instructions, leaving
   places in the code where GP is not valid.  It is not safe to
   rematerialize expressions involving GP in these places.
   It is not possible to determine these places until after scheduling,
   so disable any rematerialization of expressions involving the
   GP when running the LAO scheduler.
   The open64 scheduler also appears to migrate forwards the
   instruction that loads the new GP for an indirect function call,
   so that it is moved before expressions that may be rematerialized
   from the GOT, so always disable rematerialization of expressions
   involving GP.
*/
#define CGTARG_GP_Expressions_Are_Rematerializable_p  \
        ! (Gen_GP_Relative && Is_Caller_Save_GP \
           && !Constant_GP)

#define CGTARG_Address_Constants_Are_Rematerializable_p \
        (! Gen_GP_Relative || CGTARG_GP_Expressions_Are_Rematerializable_p)

extern void
CGTARG_Spill_on_Xfer(TN *tn, BB *bb, OPS *ops);

/* ====================================================================
 *    ASM:
 * ====================================================================
 */

/* call init routine once per asm stmt */
extern void CGTARG_Init_Asm_Constraints(void);

/* given asm constraint and mtype, 
 * pick appropriate dedicated tn and string name */
extern void CGTARG_TN_And_Name_For_Asm_Constraint (char *constraint, 
                                                   TYPE_ID rtype, 
                                                   TYPE_ID desc,
                                                   TN **tn, 
                                                   char **name);

/* --------------------------------------------------------------------
 *   target specific modifiers for printing different versions
 *   of register names when they appear as AM operands.
 * --------------------------------------------------------------------
 */
extern char CGTARG_Asm_Opnd_Modifiers[];
extern INT  CGTARG_Num_Asm_Opnd_Modifiers;

extern const char* CGTARG_Modified_Asm_Opnd_Name(char, TN*, char *);

//TB add subclass parameter to CGTARG_Modify_Asm_String to handle
//registers that have different name depending on thier subclass
extern char* CGTARG_Modify_Asm_String (char* asm_string, 
				       UINT32 position, bool memory, TN* tn, 
				       ISA_REGISTER_SUBCLASS sc);

/* ====================================================================
 *                    Code Generation stuff
 * ====================================================================
 */

inline TN *CGTARG_select_TN (INT32 size) 
{ 
  return Gen_Register_TN (ISA_REGISTER_CLASS_cpsr, size); 
}

inline TN *CGTARG_gen_trip_count_TN (INT32 trip_size) 
{ 
  return Gen_Register_TN(ISA_REGISTER_CLASS_integer, trip_size);
}

inline BOOL
CGTARG_Has_Branch_Predict(void)
{
  return FALSE;
}

inline BOOL
CGTARG_Have_Indexed_Mem_Insts(void)
{
  return FALSE;
}

inline BOOL
CGTARG_Has_min_max_abs(void)
{
  return TRUE;
}
 


/* ====================================================================
 *   Prefetching:
 * ====================================================================
 */
inline INT32
CGTARG_L1_ld_latency (INT32 current_value)
{
  return current_value;
}

inline BOOL
CGTARG_enable_pf_L1_ld (BOOL current_value)
{
  return PROC_has_prefetch ();
}

inline BOOL
CGTARG_enable_pf_L1_st (BOOL current_value)
{
  return FALSE;
}

inline BOOL
CGTARG_enable_pf_L2_ld (BOOL current_value)
{
  return FALSE;
}

inline BOOL
CGTARG_enable_pf_L2_st (BOOL current_value)
{
  return FALSE;
}

inline TN *
CGTARG_EH_Gen_Return_Stackadj_TN ()
{
  FmtAssert (FALSE, ("EH_Return not supported"));
  return NULL;
}

inline BOOL
CGTARG_Is_OP_Intrinsic(OP *op)
{
  return OP_code(op) == TOP_intrncall;
}
/* ====================================================================
 *   Target specific scheduling and dependence graph:
 * ====================================================================
 */

inline INT32 CGTARG_Branch_Taken_Penalty(void)
{
  return CGTARG_branch_taken_penalty_overridden ?
    CGTARG_branch_taken_penalty : 1;
}
/* ====================================================================
 *    Properties:
 * ====================================================================
 */

inline INT
CGTARG_Text_Alignment (void) {
  return DEFAULT_TEXT_ALIGNMENT;
}

#endif /* targ_cg_INCLUDED */
