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
 *  CGIR operation structure (OP) and utility routines which include target
 *  dependencies.
 *
 *  THIS FILE IS ONLY TO BE INCLUDE BY ../op.h!!!!
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef targ_op_INCLUDED
#define targ_op_INCLUDED

#define OP_COPY_OPND 1
#define OP_PREDICATE_OPND 0

/* Result must not be same as operand */
inline BOOL TOP_is_uniq_res(TOP topcode, INT i) {
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(topcode);
  ISA_OPERAND_USE this_def = ISA_OPERAND_INFO_Def(oinfo, i);
  if (this_def & OU_uniq_res) 
    return TRUE;

  return FALSE;
}

/* Returns index of operand that must be same register as result i */ 
inline INT TOP_is_same_res(TOP topcode, INT i) { 
  const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(topcode); 
  return ISA_OPERAND_INFO_Same_Res(oinfo, i); 
} 

/* _fixed_results and _fixed_opnds return how many fixed
 * results/operands an instruction has (OP_result/OP_opnds includes
 * any variable operands in the count).
 */
#define TOP_fixed_results(t)	(ISA_OPERAND_INFO_Results(ISA_OPERAND_Info(t)))
#define TOP_fixed_opnds(t)	(ISA_OPERAND_INFO_Operands(ISA_OPERAND_Info(t)))

/* ??? */
#define OP_inst_words(o)        (1)

inline TOP CGTARG_Noop_Top (ISA_EXEC_UNIT_PROPERTY unit) {
  return TOP_GP32_NOP;
}

#endif /* op_targ_INCLUDED */
