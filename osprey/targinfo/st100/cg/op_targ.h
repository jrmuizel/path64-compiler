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

#ifndef op_targ_INCLUDED
#define op_targ_INCLUDED

#define OP_COPY_OPND 1
#define OP_PREDICATE_OPND 0

#define TOP_is_likely(t)	 (FALSE)
#define TOP_is_branch_predict(t) (FALSE)
#define TOP_is_side_effects(t)   (FALSE)
#define TOP_is_noop(t)           (t == TOP_noop)
/* Memory instructions which are fill/spill type */
#define TOP_is_mem_fill_type(t)  (FALSE)
#define TOP_is_unalign_ld(t)     (FALSE)
#define TOP_is_unalign_store(t)  (FALSE)
#define TOP_is_defs_fcr(t)       (FALSE)
#define TOP_is_defs_fcc(t)       (FALSE)
#define TOP_is_refs_fcr(t)       (FALSE)
#define TOP_is_select(t)         (FALSE)
#define TOP_is_isub(t)           (FALSE)
#define TOP_is_ior(t)            (FALSE)
#define TOP_is_flop(t)           (FALSE)
#define TOP_is_fadd(t)           (FALSE)
#define TOP_is_fsub(t)           (FALSE)
#define TOP_is_fmul(t)           (FALSE)
#define TOP_is_fdiv(t)           (FALSE)
#define TOP_is_imul(t)           (FALSE)
#define TOP_is_idiv(t)           (FALSE)
#define TOP_is_icmp(t)           (FALSE)
/* This is already defined in isa_properties.cxx
#define TOP_is_madd(t)           (FALSE) 
*/

/* Instruction must be first in an instruction group */
#define TOP_is_f_group(t)        (FALSE)
/* Instruction must be last in an instruction group */
#define TOP_is_l_group(t)        (FALSE)
/* Instruction accesses rotating register bank */
#define TOP_is_access_reg_bank(t) (FALSE)
/* Result must not be same as operand */
#define TOP_is_uniq_res(t)       (TRUE)

/* _fixed_results and _fixed_opnds return how many fixed
 * results/operands an instruction has (OP_result/OP_opnds includes
 * any variable operands in the count).
 */
#define TOP_fixed_results(t)	(ISA_OPERAND_INFO_Results(ISA_OPERAND_Info(t)))
#define TOP_fixed_opnds(t)	(ISA_OPERAND_INFO_Operands(ISA_OPERAND_Info(t)))

/* This is defined as a isa_property
#define TOP_is_unsafe(t)        (FALSE)
*/


/* ??? */
#define OP_inst_words(o)        (1)

#define TOP_same_res(t)             (FALSE)
#define TOP_save_predicates(t)      (FALSE)
#define TOP_restore_predicates(t)   (FALSE)

// return simulated nop
//inline OP *_CGTARG_Noop (void) {
//  return Mk_OP (TOP_noop, True_TN);	// return simulated nop
//}

inline TOP CGTARG_Noop_Top (ISA_EXEC_UNIT_PROPERTY unit) {
  return TOP_GP32_NOP;
}

//inline OP *_CGTARG_Noop (ISA_EXEC_UNIT_PROPERTY unit) {
//  return Mk_OP (TOP_GP32_NOP); // not predicated on ST100
//}

#endif /* op_targ_INCLUDED */
