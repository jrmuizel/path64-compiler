/*

  Copyright (C) 2007, STMicroelectronics Inc.  All Rights Reserved.

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


/*================================================================

Initialization and termination routines

See pqs_cg.h for descriptions.

*/

#include "defs.h"
#include "cg.h"
#include "pqs_cg.h"
#include "pqs.h"
#include "pqs_target.h"
#include "mempool.h"
#include "cxx_memory.h"
#include "tracing.h"

// Disable the PQS (for debugging purposes)
BOOL PQS_disabled=FALSE;

// Tracing variable
BOOL PQS_Tracing;

// This is the primary instance of the PQS used by the CG "global" interface
static PQS_MANAGER *pqsm=NULL;

bool lt_tn::operator()(TN *t1, const TN *t2) const {
  // This code seems unnecessarily complicated for what we need here:
  // we just need a canonical ordering of TNs that could be constants
  // or registers, but comparing constant TNs of various types is
  // complicated.
  if (TN_is_register (t1) && TN_is_register (t2)) {
    return TN_number(t1) < TN_number(t2);
  } else if (TN_is_register (t1)) {
    return t1;
  } else if (TN_is_register (t2)) {
    return t2;
  }
  // Neither are registers.
  if (TN_has_value (t1) && TN_has_value (t2)) {
    return TN_value (t1) < TN_value (t2);
  } else if (TN_has_value (t1)) {
    return t1;
  } else if (TN_has_value (t2)) {
    return t2;
  }
  // Neither have integer value.
  if (TN_is_symbol (t1) && TN_is_symbol (t2)) {
    if (ST_st_idx (TN_var (t1)) != ST_st_idx (TN_var (t2))) {
      return ST_st_idx (TN_var (t1)) < ST_st_idx (TN_var (t2));
    } else {
      return TN_offset (t1) < TN_offset (t2);
    }
  } else if (TN_is_symbol (t1)) {
    return t1;
  } else if (TN_is_symbol (t2)) {
    return t2;
  }
  // Neither is a symbol.
  if (TN_is_label (t1) && TN_is_label (t2)) {
    if (TN_label (t1) != TN_label (t2)) {
      return TN_label (t1) < TN_label (t2);
    } else {
      return TN_offset (t1) < TN_offset (t2);
    }
  } else if (TN_is_label (t1)) {
    return t1;
  } else if (TN_is_label (t2)) {
    return t2;
  }
  // Neither is a label.
  FmtAssert (TN_is_enum (t1) && TN_is_enum (t2),
	     ("Unexpected TN in lt_tn"));
  return TN_enum (t1) < TN_enum (t2);
}

void
PQSCG_init (BB *first_bb)
{
   BB *bb;
   OP *op;

   if (pqsm || PQS_disabled) return;

   PQS_Tracing = Get_Trace(TP_PQS, PQS_ENTRY);

   // Initialize the pqs manager
   PQS_Init_Memory();
   MEM_POOL_Push(&PQS_mem_pool);
   hTN_MAP tn_map = hTN_MAP_Create (&PQS_mem_pool);
   pqsm = CXX_NEW(PQS_MANAGER (),&MEM_phase_pool);
   
   PQSTARG_target_synthesized_info (pqsm);

   // We need to inform pqsm of the order of uses and defs of
   // each TN, so that it can decide which are valid for its analysis.
   FOR_ALL_BBLIST_ITEMS(first_bb, bb) {
     FOR_ALL_BB_OPs_FWD(bb, op) {
       for (int opndno = 0; opndno < OP_opnds(op); opndno++) {
	 TN *tn = OP_opnd (op, opndno);
	 pqsm->Note_Use (bb, tn);
       }
       for (int resno = 0; resno < OP_results(op); resno++) {
	 TN *tn = OP_result (op, resno);
	 pqsm->Note_Def (bb, tn);
       }
     }
   }
   // Also compute all modified registers in the code.
   // We will allow unmodified dedicated registers to be valid for the
   // analysis.
   REGISTER_SET modified_registers[ISA_REGISTER_CLASS_MAX_LIMIT+1];
   ISA_REGISTER_CLASS rc;
   FOR_ALL_ISA_REGISTER_CLASS(rc) {
     modified_registers[rc] = REGISTER_SET_EMPTY_SET;
   }
   FOR_ALL_BBLIST_ITEMS(first_bb, bb) {
     BB_Modified_Registers (bb, modified_registers, FALSE);
   }
   pqsm->Note_Modified_Registers (modified_registers);
   // Now we have passed all the required information, calculate
   // the valid TNs.
   pqsm->Calculate_Valid_TNs ();
       
   // Now scan the instructions to determine their predicate
   // behaviour.
   // Create a mapping from TN to PQS_NODE on the fly.
   FOR_ALL_BBLIST_ITEMS(first_bb, bb) {
     FOR_ALL_BB_OPs_FWD(bb, op) {
       PQSTARG_classify_instruction (pqsm, op);
     }
   }
   pqsm->Complete_Graph ();

   if (PQS_Tracing) {
     Trace_IR (TP_PQS, "PQS analysis", NULL);
   }
}

void
PQSCG_term(void)
{
   if (pqsm) {
      CXX_DELETE(pqsm,&MEM_phase_pool);
      MEM_POOL_Pop(&PQS_mem_pool);
      pqsm = NULL;
   }
}

void
PQSCG_reinit (BB *first_bb)
{
  PQSCG_term();
  PQSCG_init(first_bb);
}

BOOL 
PQSCG_pqs_valid(void)
{
  return pqsm != NULL;
}

// Get the mempool used by PQS
MEM_POOL * PQSCG_get_mempool(void)
{
  return &PQS_mem_pool;
}

/*================================================================
  The interface routines which use the "current" PQS_MANAGER set up by PQS_Init()
  ================================================================
*/

#define MAKE_SURE_PQSM_INTIALIZED Is_True(pqsm,("Predicate query system not yet intialized"))

BOOL PQSCG_is_disjoint(TN * tn1, TN * tn2)
{
  return pqsm && pqsm->PQS_is_disjoint (tn1, tn2);
}

BOOL PQSCG_is_disjoint(PQS_TN_SET &tns1, PQS_TN_SET &tns2)
{
  return pqsm && pqsm->PQS_is_disjoint (tns1, tns2);
}

BOOL PQSCG_is_subset_of (TN *tn1, TN *tn2)
{
  if (pqsm) {
    return pqsm->PQS_is_subset_of (tn1, tn2);
  } else {
    return tn1 == tn2 || tn2 == True_TN;
  }
}

BOOL PQSCG_is_subset_of (TN * tn1, PQS_TN_SET &tns2)
{
  if (pqsm) {
    return pqsm->PQS_is_subset_of (tn1, tns2);
  } else {
    return tns2.Is_Subset (True_TN) || tns2.Is_Subset(tn1);
  }
}

BOOL PQSCG_is_subset_of (PQS_TN_SET &tns1, PQS_TN_SET &tns2)
{
  if (pqsm) {
    return pqsm->PQS_is_subset_of (tns1, tns2);
  } else {
    return tns2.Is_Subset (True_TN) || tns2.Is_Subset (tns1);
  }
}

BOOL PQSCG_sets_results_if_qual_true(OP *op)
{
  return PQSTARG_sets_results_if_qual_true (op);
}

