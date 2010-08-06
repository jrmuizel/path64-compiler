/*
  Copyright (C) 2005, STMicroelectronics, All Rights Reserved.

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

/* ====================================================================
 * ====================================================================
 *
 * Module: mexpand.cxx
 *
 * Description:
 *
 * Converts ops with register-tuple (e.g. register pair) results or
 * operand into "multi-ops" which have a result (resp. operand)
 * TN for each member of the tuple.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "cg.h"
#include "cg_flags.h"
#include "tracing.h"
#include "op_map.h"
#include "cgtarget.h"
#include "cgexp.h"
#include "whirl2ops.h"
#include "op.h"

struct Multi_TN_Info {
  INT count;
  TN **element;
};

static BOOL Trace_MOP;

static MEM_POOL mexpand_pool;

/*
 * For a given tuple ( TN, Nb_Register_In_Sub_TNs ), return a structure
 * containing the list of sub TNs, each one made of Nb_Register_In_Sub_TNs
 * registers.
 * TN is expected to be a multi-register TN, the number of registers
 * being a power of 2.
 * Nb_Register_In_Sub_TNs must also be a power of 2.
 *
 * Example:
 * -------
 * Consider register hierarchy R0 = Rh0:Rh1 = Rq0:Rq1:Rq2:Rq3
 * Multi_TN_MAP_Get(map, R0, 1) will return {Rq0:Rq1:Rq2:Rq3}
 * Multi_TN_MAP_Get(map, R0, 2) will return {Rh0:Rh1}
 */
static Multi_TN_Info *
Multi_TN_MAP_Get (
		  TN_MAP map,
		  const TN *tn,
		  INT subtn_nbregs
		  )
{
  INT i;
  Multi_TN_Info *result = NULL;

  if (TN_is_register (tn) && (TN_nhardregs (tn) > 1) &&
      (subtn_nbregs < TN_nhardregs (tn))) {

    // Compute the number of subdivisions available for the parameter TN
    // as well as the id of the wanted one
    const INT nregs = TN_nhardregs (tn);
    const INT nb_subtns = nregs / subtn_nbregs;
    INT nb_subdiv = 0;
    INT wanted_subdiv = -1;
    for (i = nregs; i > 1 && (!(i&1)); i=i>>1) {
      if (i == nb_subtns) {
	wanted_subdiv = nb_subdiv;
      }
      nb_subdiv++;
    }
    FmtAssert((i==1), ("Number of registers in parameter TN is not a power of 2"));
    FmtAssert((wanted_subdiv!=-1), ("Number of registers in sub TNs is not a power of 2"));

    result = (Multi_TN_Info *)TN_MAP_Get (map, tn);
    if (result == NULL) {
      result = (Multi_TN_Info *)
	// Each subdivision level is associated with a Multi_TN_Info
	MEM_POOL_Alloc (&mexpand_pool, sizeof(Multi_TN_Info)*nb_subdiv);

      for (i = 0; i < nb_subdiv; i++) {
	result[i].count = nregs >> i;
	result[i].element = (TN **)NULL;
      }
      TN_MAP_Set (map, (TN *)tn, result);
    }
    result = &result[wanted_subdiv];
    if (result->element == NULL) {
      // Sub TNS not yet created for current subdivision
      result->element = (TN **)
	MEM_POOL_Alloc (&mexpand_pool, nb_subtns * sizeof(TN *));
      ISA_REGISTER_CLASS rclass = TN_register_class (tn);
      REGISTER reg = TN_register (tn);
      const INT subtn_size = (TN_size (tn) / nregs) * subtn_nbregs;
      for (i = 0; i < nb_subtns; i++) {
	result->element[i] = Gen_Register_TN (rclass, subtn_size);
	if (TN_is_zero_reg (tn)) {
	  Set_TN_register (result->element[i], reg);
	} else {
	  Set_TN_register (result->element[i], reg + (i * subtn_nbregs));
	}
      }
    }
  }
  return result;
}


/* [TTh]
 * Update asm information attached to new_op, which is the expanded version
 * of old_op, that contains Multi-register TNs.
 */
static void Update_For_Multi_ASM_OP_ANNOT(TN_MAP multi_tn_map, OP *new_op, OP *old_op,
					  INT new_res_count, INT new_opnd_count) {
  INT i;
  INT opndnum, resnum;
  INT new_opndnum = 0;
  INT new_resnum = 0;
  INT opnd_old2new[ISA_OPERAND_max_operands]; // Associate operand rank of old_op to operand rank of new_op
  TN *tn;

  ASM_OP_ANNOT* old_annot = (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, old_op);
  // Create a new annotation and attach it to new_op.
  // Note: this is required as call to OP_Copy_Properties() in caller did not
  //       create a new annotation but rather attach the one from old_op to 
  //       new_op.
  ASM_OP_ANNOT* new_annot = Create_Empty_ASM_OP_ANNOT(new_res_count, new_opnd_count);
  Copy_ASM_OP_Non_Parameter_Fields(new_annot, old_annot);
  OP_MAP_Set(OP_Asm_Map, new_op, new_annot);

  for (opndnum = 0; opndnum < OP_opnds(old_op); opndnum++) {
    TN *tn = OP_opnd(old_op, opndnum);
    Multi_TN_Info *info = Multi_TN_MAP_Get (multi_tn_map, tn, 1);
    opnd_old2new[opndnum] = new_opndnum;
    // First copy info for single register TNs or first part of Multi-register TNs
    ASM_OP_opnd_constraint(new_annot)[new_opndnum] = ASM_OP_opnd_constraint(old_annot)[opndnum];
    ASM_OP_opnd_subclass  (new_annot)[new_opndnum] = ASM_OP_opnd_subclass  (old_annot)[opndnum];
    ASM_OP_opnd_position  (new_annot)[new_opndnum] = ASM_OP_opnd_position  (old_annot)[opndnum];
    ASM_OP_opnd_memory    (new_annot)[new_opndnum] = ASM_OP_opnd_memory    (old_annot)[opndnum];
    new_opndnum++;
    if (info) {
      // Then initialized infos for other part of Multi-register TNs
      for (INT i = 1; i < info->count; i++) {
	ASM_OP_opnd_constraint(new_annot)[new_opndnum] = ASM_OP_opnd_constraint(old_annot)[opndnum];
	ASM_OP_opnd_subclass  (new_annot)[new_opndnum] = ASM_OP_opnd_subclass  (old_annot)[opndnum];
	ASM_OP_opnd_position  (new_annot)[new_opndnum] = ASM_OP_position_UNDEF;
	ASM_OP_opnd_memory    (new_annot)[new_opndnum] = ASM_OP_opnd_memory    (old_annot)[opndnum];
	new_opndnum++;
      }
    }
  }

  for (resnum = 0; resnum < OP_results(old_op); resnum++) {
    TN *tn = OP_result(old_op, resnum);
    Multi_TN_Info *info = Multi_TN_MAP_Get (multi_tn_map, tn, 1);
    // First copy info for single register TNs or first part of Multi-register TNs
    ASM_OP_result_constraint(new_annot)[new_resnum] = ASM_OP_result_constraint(old_annot)[resnum];
    ASM_OP_result_subclass  (new_annot)[new_resnum] = ASM_OP_result_subclass  (old_annot)[resnum];
    ASM_OP_result_position  (new_annot)[new_resnum] = ASM_OP_result_position  (old_annot)[resnum];
    ASM_OP_result_clobber   (new_annot)[new_resnum] = ASM_OP_result_clobber   (old_annot)[resnum];
    ASM_OP_result_memory    (new_annot)[new_resnum] = ASM_OP_result_memory    (old_annot)[resnum];
    ASM_OP_result_same_opnd (new_annot)[new_resnum] = ((ASM_OP_result_same_opnd(old_annot)[resnum] == -1)
						       ? -1
						       : opnd_old2new[ASM_OP_result_same_opnd(old_annot)[resnum]]);
    new_resnum++;
    if (info) {
      // Then initialized infos for other part of Multi-register TNs
      for (INT i = 1; i < info->count; i++) {
	ASM_OP_result_constraint(new_annot)[new_resnum] = ASM_OP_result_constraint(old_annot)[resnum];
	ASM_OP_result_subclass  (new_annot)[new_resnum] = ASM_OP_result_subclass  (old_annot)[resnum];
	ASM_OP_result_position  (new_annot)[new_resnum] = ASM_OP_position_UNDEF;
	ASM_OP_result_clobber   (new_annot)[new_resnum] = ASM_OP_result_clobber   (old_annot)[resnum];
	ASM_OP_result_memory    (new_annot)[new_resnum] = ASM_OP_result_memory    (old_annot)[resnum];
	ASM_OP_result_same_opnd (new_annot)[new_resnum] = ((ASM_OP_result_same_opnd(old_annot)[resnum] == -1)
							   ? -1
							   : opnd_old2new[ASM_OP_result_same_opnd(old_annot)[resnum]]+i);
	new_resnum++;
      }
    }
  }
}


static void
Convert_BB_Ops(TN_MAP multi_tn_map, BB *bb)
{
  OP *op, *next;
  
  Trace_MOP = Get_Trace (TP_ALLOC, 0x4000, bb);
  
  for (op = BB_first_op(bb); op != NULL; op = next) {
    BOOL replace = FALSE;
    BOOL rescan_new_ops = FALSE;
    next = OP_next(op);
    OPS new_ops = OPS_EMPTY;
    TOP opr = OP_code (op);
    
    if (OP_extract(op) || OP_compose(op) || OP_widemove(op)) {
      TN *dest[ISA_OPERAND_max_operands];
      TN *source[ISA_OPERAND_max_operands];
      INT i;
      INT ncopies;
      // Handle guarded compose/extract
      INT first_opnd = TOP_is_predicated(opr)?1:0;
      // Set up dest and source arrays.
      if (OP_extract(op)) {
	ncopies = 0;
	INT subtn_nbregs = TN_nhardregs(OP_result (op, 0));
	Multi_TN_Info *info = Multi_TN_MAP_Get (multi_tn_map,
						OP_opnd(op, first_opnd),
						subtn_nbregs);
	for (i = 0; i < OP_results(op); i++) {
	  // Extract to constant result register can be eliminated.
	  // Note that TN_is_const_reg does not work here because
	  // it rejects non-dedicated TNs.
	  if (! TN_is_zero_reg (OP_result (op, i))) {
	    dest[ncopies] = OP_result(op, i);
	    source[ncopies] = info->element[i];
	    ncopies++;
	  }
	}
      } else if (OP_compose(op)) {
	ncopies = 0;
	// Compose to constant result register can be eliminated.
	// Note that TN_is_const_reg does not work here because
	// it rejects non-dedicated TNs.
	if (! TN_is_zero_reg (OP_result (op, 0))) {
	  INT subtn_nbregs = TN_nhardregs(OP_opnd (op, first_opnd));
	  Multi_TN_Info *info = Multi_TN_MAP_Get (multi_tn_map,
						  OP_result(op, 0),
						  subtn_nbregs);
	  for (i = 0; i < OP_opnds(op)-first_opnd; i++) {
	    dest[ncopies] = info->element[i];
	    source[ncopies] =  OP_opnd(op, i + first_opnd);
	    ncopies++;
	  }
	}
      } else {
	// widemove
	if (! TN_is_zero_reg (OP_result (op, 0))) {
	  Multi_TN_Info *dstinfo = Multi_TN_MAP_Get (multi_tn_map, OP_result(op, 0), 1);
	  Multi_TN_Info *srcinfo = Multi_TN_MAP_Get (multi_tn_map, OP_opnd(op, first_opnd), 1);
	  FmtAssert (dstinfo->count == srcinfo->count,
		     ("Destination and source mismatch expanding widemove"));
	  ncopies = 0;
	  for (i = 0; i < dstinfo->count; i++) {
	    dest[ncopies] = dstinfo->element[i];
	    source[ncopies] = srcinfo->element[i];
	    ncopies++;
	  }
	}
      }
      INT to_do = ncopies;
      TN *tempo_reg       = NULL;
      TN *tempo_reg_saved = NULL;
      while (to_do > 0) {
	INT copies_done_this_pass = 0;
	TN *ready_dest[ISA_OPERAND_max_operands];
	TN *ready_source[ISA_OPERAND_max_operands];
	for (i = 0; i < ncopies; i++) {
	  if (dest[i]) {
	    REGISTER rd = TN_register (dest[i]);
	    INT j;
	    for (j = 0; j < ncopies; j++) {
	      if (j != i && source[j] && TN_register(source[j]) == rd) {
		// Another copy needs this destination, so we cannot
		// emit it yet.
		break;
	      }
	    }
	    if (j == ncopies) {
	      // [TTh] Inserted ops must be rescanned if they
	      //       potentially contain Multi-register TNs
	      rescan_new_ops |= (TN_nhardregs(dest[i]) > 1);
	      // Keep description of first move, as its source register
	      // might be used to break a cyclic dependency between
	      // remaining moves.
	      if ((tempo_reg == NULL) && (TN_register(source[i]) != rd)) {
		tempo_reg = source[i];
		tempo_reg_saved = dest[i];
	      }
	      // Move copy from wait state to ready state
	      ready_dest[copies_done_this_pass] = dest[i];
	      ready_source[copies_done_this_pass] = source[i];
	      dest[i] = NULL;
	      source[i] = NULL;
	      copies_done_this_pass++;
	    }
	  }
	}

	// Perform the actual copy generation, with an attempt to merge
	// consecutive copy into bigger composite copies.
	for (i = 0; i < copies_done_this_pass; i++) {

	  if (i < (copies_done_this_pass-3) &&
	      CGTARG_Exist_Single_OP_Copy(TN_register_class(ready_dest[i]), 4) &&
	      CGTARG_Is_Register_Quad(ready_dest[i], ready_dest[i+1], ready_dest[i+2], ready_dest[i+3]) &&
	      CGTARG_Is_Register_Quad(ready_source[i], ready_source[i+1], ready_source[i+2], ready_source[i+3])
	      ) {
	    // Expand a single 4x composite register copy
	    TN *new_source = Build_Dedicated_TN ( TN_register_class(ready_source[i]),
						  TN_register(ready_source[i]),
						  TN_size(ready_source[i])*4 );
	    TN *new_dest = Build_Dedicated_TN ( TN_register_class(ready_dest[i]),
						TN_register(ready_dest[i]),
						TN_size(ready_dest[i])*4 );
	    rescan_new_ops = TRUE;
	    Exp_COPY(new_dest, new_source, &new_ops);
	    i+=3;  // 4 copies processed this pass
	  }
	  else if (i < (copies_done_this_pass-1) &&
		   CGTARG_Exist_Single_OP_Copy(TN_register_class(ready_dest[i]), 2) &&
		   CGTARG_Is_Register_Pair(ready_dest[i], ready_dest[i+1]) &&
		   CGTARG_Is_Register_Pair(ready_source[i], ready_source[i+1])
		   ) {
	    // Expand a single 2x composite register copy
	    TN *new_source = Build_Dedicated_TN ( TN_register_class(ready_source[i]),
						  TN_register(ready_source[i]),
						  TN_size(ready_source[i])*2 );
	    TN *new_dest = Build_Dedicated_TN ( TN_register_class(ready_dest[i]),
						TN_register(ready_dest[i]),
						TN_size(ready_dest[i])*2 );
	    rescan_new_ops = TRUE;
	    Exp_COPY(new_dest, new_source, &new_ops);
	    i++;  // 2 copies processed this pass
	  }
	  else {
	    // Expand a standard copy
	    Exp_COPY(ready_dest[i], ready_source[i], &new_ops);
	  }
	}
	
	if ((copies_done_this_pass == 0) && (tempo_reg != NULL)) {
	  // No copy done this pass, meaning we are probably facing a
	  // cycle dependency between remaining moves. We can use the
	  // source register of a previous copy, if any,  as  a tempo
	  // register to break the cycle.
	  // Note: this code should be seldom used as the regalloc should
	  //       normally avoid generating cyclic dependencies.
	  for (i = 0; i < ncopies; i++) {
	    // ...Select any remaining copy
	    if (dest[i]) {
	      // Inserted ops must be rescanned if they
	      // potentially contain Multi-register TNs
	      rescan_new_ops |= (TN_nhardregs(dest[i]) > 1);
	      // ...Save dest of selected copy to tempo location
	      Exp_COPY (tempo_reg, dest[i], &new_ops);
	      // ...Then expand the selected copy
	      Exp_COPY (dest[i], source[i], &new_ops);
	      copies_done_this_pass++;
	      // ...Update the remaining copy list
	      int j;
	      for (j = 0; j < ncopies; j++) {
		if ((j != i) && (source[j] != NULL) && (TN_register(source[j]) == TN_register(dest[i]))) {
		  source[j] = tempo_reg;
		}
	      }
	      // ...Plan to restore the value of the tempo register
	      //    Required because without liveness analysis we cannot insure
	      //    that the initial value will not be used afterwards
	      source[i] = tempo_reg_saved;
	      dest[i]   = tempo_reg;
	      to_do++;
	      break;
	    }
	  }
	}
	FmtAssert (copies_done_this_pass > 0,
		   ("Blocked during expansion"));
	to_do -= copies_done_this_pass;
      }
      replace = TRUE;
      Reset_BB_scheduled (bb);

      OP *new_op;
      FOR_ALL_OPS_OPs (&new_ops, new_op) {
	// Propagate properties from original op to new op.
	OP_Copy_Properties (new_op, op);
      }
    } else {
      
      TOP multi_opr = CGTARG_TOP_To_Multi (opr);

      // [TTh] For asm statement and kill dummy op, check if it uses multi-register TNs
      if ((opr == TOP_asm) || (opr == TOP_KILL)) {
	INT opndnum;
	INT resnum;
	multi_opr = TOP_UNDEFINED;
	for (opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
	  TN *tn = OP_opnd(op, opndnum);
	  if (TN_is_register (tn) && (TN_nhardregs (tn) > 1)) {
	    multi_opr = opr;
	    break;
	  }
	}
	for (resnum = 0; (multi_opr == TOP_UNDEFINED) && resnum < OP_results(op); resnum++) {
	  TN *tn = OP_result(op, resnum);
	  if (TN_is_register (tn) && (TN_nhardregs (tn) > 1)) {
	    multi_opr = opr;
	    break;
	  }
	}
      }

      if ((multi_opr != TOP_UNDEFINED) &&
	  ((multi_opr != opr) || (multi_opr == TOP_asm) || (multi_opr == TOP_KILL))) {
	TN *new_opnds[ISA_OPERAND_max_operands];
	INT new_opndnum = 0;
	TN *new_results[ISA_OPERAND_max_results];
	INT new_resnum = 0;
	INT opndnum;
	INT resnum;
	
	for (opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
	  TN *tn = OP_opnd(op, opndnum);
	  Multi_TN_Info *info = Multi_TN_MAP_Get (multi_tn_map, tn, 1);
	  if (info) {
	    for (INT i = 0; i < info->count; i++) {
	      new_opnds[new_opndnum++] = info->element[i];
	    }
	  } else {
	    new_opnds[new_opndnum++] = tn;
	  }
	}
	for (resnum = 0; resnum < OP_results(op); resnum++) {
	  TN *tn = OP_result(op, resnum);
	  Multi_TN_Info *info = Multi_TN_MAP_Get (multi_tn_map, tn, 1);
	  if (info) {
	    for (INT i = 0; i < info->count; i++) {
	      new_results[new_resnum++] = info->element[i];
	    }
	  } else {
	    new_results[new_resnum++] = tn;
	  }
	}
	OP *new_op = Mk_VarOP (multi_opr, new_resnum, new_opndnum,
			       new_results, new_opnds);
	// Propagate properties from original op to new op.
	OP_Copy_Properties (new_op, op);
	// Also copy WN for alias info.
	Copy_WN_For_Memory_OP (new_op, op);
	if (multi_opr == TOP_asm) {
	  // Update asm annotation if any
	  Update_For_Multi_ASM_OP_ANNOT(multi_tn_map, new_op, op, new_resnum, new_opndnum);
	}
	OPS_Append_Op (&new_ops, new_op);
	replace = TRUE;
      }
    }
    if (replace) {
#ifdef TARG_ST
      // (cbr) Support for guards on false
      OPS_Copy_Predicate (&new_ops, op);
#endif
      BB_Insert_Ops_Before (bb, op, &new_ops);
      BB_Remove_Op (bb, op);
      if (rescan_new_ops && OPS_first(&new_ops)) {
	next = OPS_first(&new_ops);
      }
    }
  }
}


void Convert_To_Multi_Ops (void)
{
  BB *bb;

  MEM_POOL_Initialize (&mexpand_pool, "MOP_Expand_pool", FALSE);

  TN_MAP multi_tn_map = TN_MAP_Create();

  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    Convert_BB_Ops(multi_tn_map, bb);
  }

  TN_MAP_Delete (multi_tn_map);
  MEM_POOL_Delete (&mexpand_pool);
}
  
void Convert_BB_To_Multi_Ops (BB *bb)
{
  MEM_POOL_Initialize (&mexpand_pool, "MOP_Expand_pool", FALSE);

  TN_MAP multi_tn_map = TN_MAP_Create();

  Convert_BB_Ops(multi_tn_map, bb);

  TN_MAP_Delete (multi_tn_map);
  MEM_POOL_Delete (&mexpand_pool);
}
  
