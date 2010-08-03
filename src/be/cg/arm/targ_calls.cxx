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

#include "defs.h"
#include "erglob.h"
#include "erbe.h"
#include "glob.h"
#include "tracing.h"
#include "config_target.h"
#include "config.h"

#include "symtab.h"
#include "strtab.h"
#include "be_symtab.h"
#include "targ_isa_lits.h"
#include "topcode.h"
#include "cgir.h"
#include "tn_set.h"
#include "tn_map.h"

#include "ttype.h"
#include "const.h"
#include "data_layout.h"
#include "targ_sim.h"
#include "whirl2ops.h"
#include "entry_exit_targ.h"
#include "targ_abi_properties.h"

#include "cg_flags.h"
#include "cg.h"
#include "calls.h"
#include "register.h"


/* ====================================================================
 *   CGTARG_Gen_Got_Disp_TN ()
 * ====================================================================
 */
TN *
CGTARG_Gen_Got_Disp_TN (ST *st, OPS *ops)
{
  FmtAssert(FALSE,("Not implemented"));
#if 0
  TN *got_disp_tn = Gen_Symbol_TN(st, 0, TN_RELOC_GPSUB);
  Set_TN_size(got_disp_tn, Pointer_Size);  
  return got_disp_tn;
#else
  return (TN*)NULL;
#endif
}


/* ====================================================================
 *   CGTARG_Gen_GP_Value_TN ()
 * ====================================================================
 */
TN *
CGTARG_Gen_GP_Value_TN( ST *st, OPS *ops )
{
  FmtAssert(FALSE,("Not implemented"));
#if 0
  TN *gp_value_tn = Gen_Symbol_TN(st, 0, TN_RELOC_GPIDENT);
  Set_TN_size(gp_value_tn, Pointer_Size);
  return gp_value_tn;
#else
  return (TN*)NULL;
#endif
}

/* ====================================================================
 *   CGTARG_Gen_FP_TN ()
 * ====================================================================
 */
TN *
CGTARG_Gen_FP_TN (void)
{
  TN *fp_tn = Gen_Register_TN (ISA_REGISTER_CLASS_integer, Pointer_Size);
  return fp_tn;
}

/* ====================================================================
 *   CGTARG_Gen_GP_TN ()
 * ====================================================================
 */
TN *
CGTARG_Gen_GP_TN (void)
{
  TN *gp_tn = Gen_Register_TN (ISA_REGISTER_CLASS_integer, Pointer_Size);
  return gp_tn;
}

/* ====================================================================
 *   CGTARG_Gen_Pfs_TN ()
 * ====================================================================
 */
TN *
CGTARG_Gen_Pfs_TN (void)
{
  FmtAssert(FALSE,("CGTARG_Gen_Pfs_TN: Pfs is not supported"));
  return NULL;
}

/* ====================================================================
 *   CGTARG_Get_SP_Incr ()
 *
 *   For now I assume that the $sp OP is an add or a sub. Just return
 *   the second operand:
 * ====================================================================
 */
TN *
CGTARG_Get_SP_Incr (
  OP *sp_adj
)
{
  FmtAssert(OP_code(sp_adj) == TOP_spadjust,
	             ("CGTARG_Get_SP_Incr: not TOP_spadjust"));
  return OP_opnd(sp_adj, 1);
}

/* ====================================================================
 *   CGTARG_Get_FP_Incr ()
 *
 *   For now I assume that the $fp OP is an add or a sub. Just return
 *   the second operand of sp_adjust
 * ====================================================================
 */
TN *
CGTARG_Get_FP_Incr (
  OP *fp_adj
)
{
  FmtAssert(OP_code(fp_adj) == TOP_spadjust,
	             ("CGTARG_Get_SP_Incr: not TOP_spadjust"));

  return OP_opnd(fp_adj, 1);
}

BOOL
OP_Can_Be_Tail_Call (
  OP *call_op
)
{
  TOP call_top = OP_code(call_op);
  switch (call_top) {
  case TOP_bl_cond:
  case TOP_bl:
  case TOP_blx_npc_cond:
  case TOP_blx_npc:
    break;
  case TOP_blx_i25:
    return FALSE;
    break;
  default:
    FmtAssert(FALSE, ("OP_Can_Be_Tail_Call, op not handled : %s",
		     TOP_Name(call_top)));
    /*NOTREACHED*/
  }

  return TRUE;
}
