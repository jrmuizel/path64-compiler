/*

  Copyright (C) 2002, ST Microelectronics, Inc.  All Rights Reserved.

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

  Contact information:  
*/


#include "defs.h"

#include "pqs_defs.h"
#include "pqs_cg.h"
#include "pqs.h"
#include "tracing.h"
#include "pqs_target.h" 

static TN *one_tn;

void
PQSTARG_classify_instruction (PQS_MANAGER *pqsm, OP *op)
{
  // Target-specific code to classify the predicate behaviour
  // of an instruction for the Predicate Query System.
  // The following classifications can be made:
  //  1.  It defines a predicate, in terms of a predicated
  //      comparison between two values.
  //      Add_Predicate_Cmp_Def() is used for this case.
  //  2.  It copies another variable, that may potentially be
  //      a predicate.
  //      Add_Copy() is used for this case.
  //  3.  It initializes a predicate to true or false.
  //      Set_True()/Set_False() are used for this case.
  //
  // For target instructions that define a predicate and
  // its complement, Add_Predicate_Cmp_Def () should be called
  // twice: once for each result, negating the comparison for
  // the complemented result.
  // For target instructions that are (parallel) conditional
  // overwrite, these should be treated as a sequence of
  // read-modify-write  operations.

  FmtAssert(FALSE,("Not implemented"));
#if 0

  TOP topcode = OP_code (op);
  TN *qual;

  if (OP_has_predicate(op)) {
    qual = OP_opnd(op, OP_find_opnd_use(op, OU_predicate));
  } else {
    qual = NULL;
  }

  if (OP_icmp (op)) {
    VARIANT v = TOP_cmp_variant (topcode);

    pqsm->Add_Predicate_Cmp_Def (OP_result (op, 0), qual, v,
				 OP_Opnd1 (op), OP_Opnd2 (op));
  } else if (topcode == TOP_andg
	     || topcode == TOP_org) {
    // I would like to handle TOP_xorg and TOP_andng here, but there are
    // no V_CMP_xxx variants for them.
    VARIANT v = (topcode == TOP_andg)  ? V_CMP_ANDL : V_CMP_ORL;
    
    pqsm->Add_Predicate_Cmp_Def (OP_result (op, 0), qual, v,
				 OP_opnd (op, 1), OP_opnd (op, 2));
  } else if ((topcode == TOP_andng || topcode == TOP_xorg)
	     && OP_opnd (op, 1) == OP_opnd (op, 2)) {
    // Idiom for initializing a guard to false.
    pqsm->Set_False (OP_result (op, 0), qual);
  } else if (topcode == TOP_subequ
	     || topcode == TOP_subeq
	     || topcode == TOP_subgeu
	     || topcode == TOP_subge
	     || topcode ==  TOP_subgtu
	     || topcode ==  TOP_subgt
	     || topcode ==  TOP_subleu
	     || topcode ==  TOP_suble
	     || topcode ==  TOP_subltu
	     || topcode ==  TOP_sublt
	     || topcode ==  TOP_subneu
	     || topcode ==  TOP_subne) {
    VARIANT v = ((topcode == TOP_subequ || topcode == TOP_subeq) ? V_CMP_EQ :
		 (topcode == TOP_subneu || topcode == TOP_subne) ? V_CMP_NE :
		 (topcode == TOP_subgeu) ? V_CMP_GEU :
		 (topcode == TOP_subge)  ? V_CMP_GE  :
		 (topcode == TOP_subgtu) ? V_CMP_GTU :
		 (topcode == TOP_subgt)  ? V_CMP_GT  :
		 (topcode == TOP_subleu) ? V_CMP_LEU :
		 (topcode == TOP_suble)  ? V_CMP_LE  :
		 (topcode == TOP_subltu) ? V_CMP_LTU :
		 (topcode == TOP_sublt)  ? V_CMP_LT  :
		 V_CMP_NONE);
    pqsm->Add_Predicate_Cmp_Def (OP_result (op, 0), qual, v,
				 OP_opnd (op, 1), OP_opnd (op, 2));
  } else if (topcode == TOP_lzcttclr) {
    pqsm->Add_Predicate_Cmp_Def (OP_result (op, 0), qual, V_CMP_NE,
				 OP_opnd (op, 1), Zero_TN);
  } else if (topcode == TOP_jrgtudec || topcode == TOP_jagtudec) {
    pqsm->Add_Predicate_Cmp_Def (OP_result (op, 0), True_TN, V_CMP_GTU,
				 OP_opnd (op, 1), one_tn);
  } else if (OP_copy (op)) {
    pqsm->Add_Copy (OP_Copy_Result_TN (op), qual, OP_Copy_Operand_TN (op));
  } else if (topcode == TOP_bool) {
    pqsm->Add_Copy (OP_result (op, 0), qual, OP_opnd (op, 1));
  }
#endif
}

BOOL
PQSTARG_sets_results_if_qual_true(OP *op)
{
  return TRUE;
}

void
PQSTARG_target_synthesized_info (PQS_MANAGER *pqsm)
{
  // jrgtudec analysis needs this literal, even though it does not
  // appear in the CGIR.
  one_tn = Gen_Literal_TN (1, 4, 0);
  pqsm->Note_Synthesized_TN (one_tn);
}
