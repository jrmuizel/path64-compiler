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

/* ====================================================================
 * ====================================================================
 *
 *  TN utility routines which include target dependencies.
 *
 *  THIS FILE IS ONLY TO BE INCLUDE BY ../calls.h!!!!
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef calls_targ_INCLUDED
#define calls_targ_INCLUDED

extern TN *CGTARG_Gen_Got_Disp_TN (void);
extern TN *CGTARG_Gen_FP_TN (void);
extern TN *CGTARG_Gen_GP_TN (void);
extern TN *CGTARG_Gen_Pfs_TN (void);

extern TN *CGTARG_Get_SP_Incr (OP *sp_adj);
extern TN *CGTARG_Get_FP_Incr (OP *fp_adj);

/* ====================================================================
 *   Is_Function_Value
 *
 *   Return a boolean that indicates if <tn> is a function value TN.
 * ====================================================================
 */
inline BOOL
Is_Function_Value(TN *tn)
{
  if (TN_is_dedicated(tn)) {
    REGISTER reg = TN_register(tn);
    ISA_REGISTER_CLASS rc = TN_register_class(tn);
    return REGISTER_SET_MemberP(REGISTER_CLASS_function_value(rc), reg);
  }
  return FALSE;
}

/* ====================================================================
 *   Is_Callee_Register
 *
 *   Return a boolean that indicates if <tn> is a callee register.
 * ====================================================================
 */
inline BOOL
Is_Callee_Register(TN *tn)
{
  return (ABI_PROPERTY_Is_callee (TN_register_class(tn),
       REGISTER_machine_id(TN_register_class(tn), TN_register(tn))));
}

#endif /* calls_targ_INCLUDED */
