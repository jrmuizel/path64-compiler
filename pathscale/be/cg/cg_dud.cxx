/*
  Copyright (C) 2002, STMicroelectronics, All Rights Reserved.

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
 * Module: cg_dud.cxx
 *
 * Description:
 *
 *
 * ====================================================================
 * ====================================================================
 */

#ifdef TARG_ST

#ifdef _KEEP_RCS_ID
static const char source_file[] = __FILE__;
static const char rcs_id[] = "";
#endif /* _KEEP_RCS_ID */

#include "mempool.h"
#include "defs.h"
#include "bb.h"
#include "bbregs.h" 		/* to get the definition of BBREGS */
#include "gtn_universe.h"
#include "bitset.h"	/* our clients do not need to know about it */
#include "gtn_set.h"
#include "dominate.h"
#include "findloops.h"
#include "cxx_memory.h"
#include "tracing.h"

#include "cg_dud.h"

#define TN_is_DUDreg(tn) (TN_is_register(tn) && !TN_is_dedicated(tn))

void DUD_SITE::Init(DUD_REGION *dud, DUDsite_t site) {
  oper_id = site >> IDX_WIDTH;
  oper = dud->Get_op(oper_id);

  res_opnd = site & IDX_MASK;
  if (res_opnd >= OP_MAX_FIXED_RESULTS) {
    is_result = FALSE;
    res_opnd -= OP_MAX_FIXED_RESULTS;
  }
  else
    is_result = TRUE;
}

/* ************************************************************ *
 * This functions creates Def-Use and Use-Def links for all TNs *
 * in all operations of the region                              *
 *                                                              *
 * The algorithms is decomposed in three parts.                 *
 * -1- Create a topological order of the BBs in the region      *
 * -2- Solve the DefReach data flow equations                   *
 * -3- Use the DefReach sets to create the Def-Use and Use-Def  *
 *     links                                                    *
 * ************************************************************ */

BOOL DUD_REGION::Init(BB_REGION *bb_region, MEM_POOL *region_pool) {

  BB *bb;
  OP *op;

  // Only single entry regions are supported
  if (bb_region->entries.size() != 1)
    return FALSE;

  BB *BB_REGION_entry = bb_region->entries[0];

  // TBD: Create one single name-space for def-use and use-def links,
  // so as to create also def-def links, for predicated definitions
  // for example.

  // TBD: Save SPACE by restricting the defsites to global registers,
  // and one defsite per basic block for a global register. Local TNs
  // and multiple definitions of a global TN in a a basic block will
  // be handled in part -3-

  /* ************************************************************ *
   * -1- Create a topological order of the BBs in the region      *
   * ************************************************************ */

  INT BB_REGION_size; // Number of BBs in the region
  BB_SET *BB_REGION_set; // Set of BBs in the region
  BB **BB_REGION_topo_order; // BBs in the region, ordered topologically
  BB_MAP BB_REGION_topo_map; // Map from topological index to BB

  // First, create a BB_SET from a BB_REGION
  BB_REGION_set = BB_SET_Create (PU_BB_Count + 2, dud_pool());
  BB_REGION_set = BB_REGION_to_BB_SET(BB_REGION_set, *bb_region, dud_pool());
  BB_REGION_size = BB_SET_Size(BB_REGION_set);

  // Then, create a topological order on the region
  BB_REGION_topo_map = BB_Topological_Map(BB_REGION_set, bb_region->entries[0]);
  BB_REGION_topo_order = (BB **)CXX_NEW_ARRAY(BB*, BB_REGION_size, dud_pool() );

  FOR_ALL_BB_SET_members(BB_REGION_set, bb) {
    INT32 i = BB_MAP32_Get(BB_REGION_topo_map, bb);
    Is_True(i >= 0 && i <= BB_REGION_size, ("bad <BB_REGION_topo_map> value"));
    if (i == 0) {
      BB_MAP_Delete(BB_REGION_topo_map);
      return FALSE;
    }
    BB_REGION_topo_order[i-1] = bb;
  }

  Is_True(BB_REGION_topo_order[0] == BB_REGION_entry, ("DUD_REGION: Inconsistency in topological order"));

  /* ************************************************************* *
   * -2- Solve the DefReach data flow equations                    *
   * ************************************************************* */

  /* All operations of the region are considered. They are given a
   * unique DUD_opid index, from 1 to N, in their topological order in
   * the region. The map DUD_opid_map associates an operation to this
   * unique DUD_opid.
   *
   * A DUD_tn is a TN, defined on an operation or live-in on the entry
   * block of the region, that is suitable for Def-Use/Use-Def
   * information.
   *
   * A DEFsite is a point of definition of a DUD_tn. A DEFsite is
   * referenced with an index, from 0 to N. This index is incremented
   * first on the virtual defsites for live-in DUD_tns on the entry
   * block of the region. Then this index is incremented along the
   * traversal of the operations in topological order and along the
   * definitions of an operation. The purpose is to create indexes for
   * a dense representation in BitVectors, on which data flow
   * operations can be performed.
   *
   * A map TN_DEFsites_map is created to associate each DUD_tn with a
   * list of DEFsites. This list is represented with the
   * DEFsite_list_t type.
   *
   * A DUDsite is a pair <op, opnd> which represents a unique use or
   * def of a DUD_tn in an operation in the region.
   *
   * A vector DEF_DUD_site associates each DEFsite with a DUDsite for
   * a definition of a DUD_tn.
   */

  /* typedefs */

  typedef std::list<INT> DEFsite_list_t;
  typedef std::vector<INT> DEF_DUD_site_t;

#define FOR_ALL_DEFSITE_LIST_ITEMS(list,item) \
        for (item = list->begin(); item != list->end(); item++)

  /* declarations */

  INT DUD_op_count;
  INT DEFsite_count;
  TN *DUD_tn;

  // Pointer and iterator to a list of DEFsites
  DEFsite_list_t *DEFsite_list;
  DEFsite_list_t::iterator DEFsite_iter;

  // For each DUD_tn, associate a list of defsites
  TN_MAP TN_DEFsites_map;

  // For each DEFsite, associate its DUDsite
  DEF_DUD_site_t DEF_DUD_site;

  INT topo_idx;
  INT op_idx;
  INT defsite_idx = 0;

  /* Initialization */
  TN_DEFsites_map = TN_MAP_Create();

  DUD_op_count = 1; // 0 is reserved to represent operations outside of the region
  DEFsite_count = 0;

  // Count the number of operations in the region.
  // Count the number of DEFsites for DUD_tns in the region
  // Create the map op -> opid
  // Create the array DEFsite -> DUDsite
  // Create the lists DUD_tn -> (DEFsite-1, DEFsite-2, ..., DEFsite-n)

  // First, consider all the live-registers of the region. These
  // represents virtual def-sites
  FOR_ALL_GTN_SET_members(BB_live_in(BB_REGION_entry), DUD_tn) {
    if (!TN_is_DUDreg(DUD_tn)) continue;
    defsite_idx = DEFsite_count ++;

    DEFsite_list = CXX_NEW(DEFsite_list_t, dud_pool());
    TN_MAP_Set(TN_DEFsites_map, DUD_tn, DEFsite_list);
    DEFsite_list->push_back(defsite_idx);

    DEF_DUD_site.push_back(DUD_SITE::makeDef(0,0)); // Virtual def-site on entry
  }

  // Then, traverse the operations in topological order
  for (topo_idx = 0; topo_idx < BB_REGION_size; topo_idx++) {
    bb = BB_REGION_topo_order[topo_idx];
    FOR_ALL_BB_OPs_FWD( bb, op ) {
      op_idx = DUD_op_count ++;
      Set_opid(op, op_idx);

      for ( INT res = 0; res < OP_results( op ); res++ ) {
	DUD_tn = OP_result(op, res);
	if (!TN_is_DUDreg(DUD_tn)) continue;
	defsite_idx = DEFsite_count ++;

	DEFsite_list = (DEFsite_list_t *)TN_MAP_Get(TN_DEFsites_map, DUD_tn);
	if (DEFsite_list == NULL) {
	  DEFsite_list = CXX_NEW(DEFsite_list_t, dud_pool());
	  TN_MAP_Set(TN_DEFsites_map, DUD_tn, DEFsite_list);
	}
	DEFsite_list->push_back(defsite_idx);

	DEF_DUD_site.push_back(DUD_SITE::makeDef(op_idx, res));
      }
    }
  }

  if (!DUD_check_size(DUD_op_count))
    return FALSE;

  /* ************************************************************* *
   * Initialization before solving the DefReach data flow analysis *
   * ************************************************************* */

  // Allocate and Compute the GEN, KILL, RCHin and RCHout bit-vectors
  // for each BB
  BS **BS_Gen = (BS **)CXX_NEW_ARRAY(BS*, BB_REGION_size, dud_pool() );
  BS **BS_Kill = (BS **)CXX_NEW_ARRAY(BS*, BB_REGION_size, dud_pool() );
  BS **BS_RCHin = (BS **)CXX_NEW_ARRAY(BS*, BB_REGION_size, dud_pool() );
  BS **BS_RCHout = (BS **)CXX_NEW_ARRAY(BS*, BB_REGION_size, dud_pool() );

  // Allocate BS_RCHin and BS_RCHout for all BB in the region
  // Allocate and initialize BB_gen and BB_kill for all BB in the region
  // Initialize DUDinfo_table[op_idx].op

  // Allocate and initialize BS_RCHin_entry to hold the reaching
  // values from the live-in registers at the entry of the region

  defsite_idx = 0;

  BS *BS_RCHin_entry = BS_Create_Empty(DEFsite_count, dud_pool());  
  FOR_ALL_GTN_SET_members(BB_live_in(BB_REGION_entry), DUD_tn) {
    if (!TN_is_DUDreg(DUD_tn)) continue;

    BS_Union1D(BS_RCHin_entry, defsite_idx, NULL);
    defsite_idx ++;
  }

  // Then, proceed with all the blocks in the region
  for (topo_idx = 0; topo_idx < BB_REGION_size; topo_idx++) {
    bb = BB_REGION_topo_order[topo_idx];

    BS *BB_kill = BS_Kill[topo_idx] = BS_Create_Empty(DEFsite_count, dud_pool());
    BS *BB_gen = BS_Gen[topo_idx] = BS_Create_Empty(DEFsite_count, dud_pool());
    BS_RCHin[topo_idx] = BS_Create_Empty(DEFsite_count, dud_pool());
    BS_RCHout[topo_idx] = BS_Create_Empty(DEFsite_count, dud_pool());

    FOR_ALL_BB_OPs_FWD( bb, op ) {
      op_idx = Get_opid(op);

      for ( INT res = 0; res < OP_results( op ); res++ ) {
	DUD_tn = OP_result(op, res);
	if (!TN_is_DUDreg(DUD_tn)) continue;

	Is_True(DEF_DUD_site[defsite_idx] == DUD_SITE::makeDef(op_idx, res),
		("DUD_REGION_Init defsites traversal is not identical to first traversal"));

	// FdF 20060809: Support for predication
	if (!OP_cond_def(op)) {
	  DEFsite_list  = (DEFsite_list_t *)TN_MAP_Get(TN_DEFsites_map, DUD_tn);
	  FOR_ALL_DEFSITE_LIST_ITEMS(DEFsite_list, DEFsite_iter) {
	    BS_Union1D(BB_kill, *DEFsite_iter, NULL);
	    BS_Difference1D(BB_gen, *DEFsite_iter);
	  }
	}
	BS_Union1D(BB_gen, defsite_idx, NULL);
	defsite_idx ++;
      }
    }
  }

  /* ************************************************************** *
   * Solve the DefReach data flow analysis                          *
   * ************************************************************** */
  
  // Now, iterates until a fixed point is reached.
  // Create two temporary sets;

  BS *BS_RCHin_tmp = BS_Create_Empty(DEFsite_count, dud_pool());
  BS *BS_RCHout_tmp = BS_Create_Empty(DEFsite_count, dud_pool());

  BOOL changed;
  do {
    changed = FALSE;
    for (topo_idx = 0; topo_idx < BB_REGION_size; topo_idx++) {
      bb = BB_REGION_topo_order[topo_idx];

      // RCHout = UNION ( Gen, INTER ( RCHin, !Kill ) )
      BS_RCHout_tmp = BS_CopyD(BS_RCHout_tmp, BS_RCHin[topo_idx], NULL);
      BS_RCHout_tmp = BS_DifferenceD(BS_RCHout_tmp, BS_Kill[topo_idx]);
      BS_RCHout_tmp = BS_UnionD(BS_RCHout_tmp, BS_Gen[topo_idx], NULL);

      // RCHin = UNION ( RCHout(all preds) )
      BBLIST *preds;
      BS_ClearD(BS_RCHin_tmp);

      // For the entry BB of the region, initialize its ReachIn with
      // the live-in TNs
      if (topo_idx == 0)
	BS_RCHin_tmp = BS_CopyD(BS_RCHin_tmp, BS_RCHin_entry, NULL);	

      FOR_ALL_BB_PREDS(bb,preds) {
	BB *pred = BBLIST_item(preds);
	if (!BB_SET_MemberP(BB_REGION_set, pred)) continue;
	BS_UnionD(BS_RCHin_tmp, BS_RCHout[BB_MAP32_Get(BB_REGION_topo_map, pred)-1], NULL);
      }

      // Then check if the RCHin or RCHout sets changed
      if (!BS_EqualP(BS_RCHin_tmp, BS_RCHin[topo_idx])) {
	BS_CopyD(BS_RCHin[topo_idx], BS_RCHin_tmp, NULL);
	changed = TRUE;
      }
      if (!BS_EqualP(BS_RCHout_tmp, BS_RCHout[topo_idx])) {
	BS_CopyD(BS_RCHout[topo_idx], BS_RCHout_tmp, NULL);
	changed = TRUE;
      }
    }
  } while (changed);

  /* ************************************************************** *
   * Finally, create the def-use and use-def links                  *
   *                                                                *
   * Since we always traverse the defsites in topological order,    *
   * the def-use and use-def are created in topological order of    *
   * their use and def sites                                        *
   * ************************************************************** */

  DUD_Allocate(DUD_op_count, region_pool);

  BOOL is_result;

  for (topo_idx = 0; topo_idx < BB_REGION_size; topo_idx++) {
    bb = BB_REGION_topo_order[topo_idx];
    BS_CopyD(BS_RCHin_tmp, BS_RCHin[topo_idx], NULL);

    FOR_ALL_BB_OPs_FWD( bb, op ) {
      op_idx = Get_opid(op);
      Set_op(op_idx, op);

      for (INT opnd = 0; opnd < OP_opnds(op); opnd ++) {
	DUD_tn = OP_opnd(op, opnd);
	if (!TN_is_DUDreg(DUD_tn)) continue;

	// From the list of defsites for a TN, look for the ones that
	// are reaching
	DEFsite_list = (DEFsite_list_t *)TN_MAP_Get(TN_DEFsites_map, DUD_tn);
	Is_True(DEFsite_list != NULL, ("DUD_REGION::Init DUD_tn should be defined or live-in in the region"));

	DUD_SITE::DUDsite_t DUDuse_site = DUD_SITE::makeUse(op_idx, opnd);
	FOR_ALL_DEFSITE_LIST_ITEMS(DEFsite_list, DEFsite_iter) {
	  if (BS_MemberP(BS_RCHin_tmp, *DEFsite_iter)) {
	    DUD_SITE::DUDsite_t DUDdef_site = DEF_DUD_site[*DEFsite_iter];
	    // Create the use-def links.
	    if (opnd < OP_MAX_FIXED_OPNDS)
	      TNuse_Push_DUDsite(op_idx, opnd, DUDdef_site);
	    // Create the def-use links
	    if ((DUD_SITE::get_opid(DUDdef_site) > 0) && (DUD_SITE::get_opnd(DUDdef_site, &is_result) < OP_MAX_FIXED_RESULTS))
	      TNdef_Push_DUDsite(DUD_SITE::get_opid(DUDdef_site), DUD_SITE::get_opnd(DUDdef_site, &is_result), DUDuse_site);
	  }
	}
      }

      // Then, update the BS_RCHin set
      for ( INT res = 0; res < OP_results( op ); res++ ) {
	DUD_tn = OP_result(op, res);
	if (!TN_is_DUDreg(DUD_tn)) continue;

	DUD_SITE::DUDsite_t DUDdef_site = DUD_SITE::makeDef(op_idx, res);
	DEFsite_list  = (DEFsite_list_t *)TN_MAP_Get(TN_DEFsites_map, DUD_tn);

	FOR_ALL_DEFSITE_LIST_ITEMS(DEFsite_list, DEFsite_iter) {
	  if (DEF_DUD_site[*DEFsite_iter] == DUDdef_site)
	    // This def is reaching from now on
	    BS_Union1D(BS_RCHin_tmp, *DEFsite_iter, NULL);
	  else
	    // This def kills reaching defs from other sites
	    // FdF 20060809: Support for predication
	    if (!OP_cond_def(op))
	      BS_Difference1D(BS_RCHin_tmp, *DEFsite_iter);
	}
      }
    }
    if (!BS_EqualP(BS_RCHin_tmp, BS_RCHout[topo_idx]))
      Is_True(BS_EqualP(BS_RCHin_tmp, BS_RCHout[topo_idx]), ("DUD_REGION::Init Internal error on BS_RCHin/BS_RCHout"));
  }

  // Finally, DUD_tn that are live-out of the region creates
  // additional def-use links to virtual uses.
  INT exit_idx;
  for (exit_idx = 0; exit_idx < bb_region->exits.size(); exit_idx++) {
    BB *exit = bb_region->exits[exit_idx];

    // Compute the RCHin on the exit blocks
    BBLIST *preds;
    BS_ClearD(BS_RCHin_tmp);
    FOR_ALL_BB_PREDS(exit, preds) {
      BB *pred = BBLIST_item(preds);
      if (!BB_SET_MemberP(BB_REGION_set, pred)) continue;
      BS_UnionD(BS_RCHin_tmp, BS_RCHout[BB_MAP32_Get(BB_REGION_topo_map, pred)-1], NULL);
    }

    DUD_SITE::DUDsite_t DUDuse_site;
    // Each exit-block is associated with a different use-site
    DUDuse_site = DUD_SITE::makeUse(Last_opid()+1+exit_idx, 0);

    // Now, complete the def-use sites for those reaching defs that
    // are live-out of the region.
    FOR_ALL_GTN_SET_members(BB_live_in(exit), DUD_tn) {
      if (!TN_is_DUDreg(DUD_tn)) continue;
      DEFsite_list = (DEFsite_list_t *)TN_MAP_Get(TN_DEFsites_map, DUD_tn);
      if (DEFsite_list == NULL) continue;

      FOR_ALL_DEFSITE_LIST_ITEMS(DEFsite_list, DEFsite_iter) {
	if (BS_MemberP(BS_RCHin_tmp, *DEFsite_iter)) {
	  DUD_SITE::DUDsite_t DUDdef_site = DEF_DUD_site[*DEFsite_iter];
	  if ((DUD_SITE::get_opid(DUDdef_site) > 0) && (DUD_SITE::get_opnd(DUDdef_site, &is_result) < OP_MAX_FIXED_RESULTS))
	    TNdef_Push_DUDsite(DUD_SITE::get_opid(DUDdef_site), DUD_SITE::get_opnd(DUDdef_site, &is_result), DUDuse_site);
	}
      }
    }
  }

  BB_MAP_Delete(BB_REGION_topo_map);
  return TRUE;
}

static inline std::pair<OP*, INT>
make_pair(OP* op, INT opnd)
{
  return std::pair<OP*, INT>(op, opnd);
}

INT DUD_REGION::Get_Use_Def(OP *op, INT opnd, DUD_LIST &UD_list) {
  UD_list.clear();

  INT op_idx = Get_opid(op);
  if (op_idx == 0)
    return -1;

  if ((opnd < 0) || (opnd >= OP_MAX_FIXED_OPNDS))
    return -1;

  UD_list.Init(this, &DUDinfo_table[op_idx].dud_list[OP_MAX_FIXED_RESULTS+opnd], TRUE);
  return UD_list.size();
}

INT DUD_REGION::Get_Def_Use(OP *op, INT res, DUD_LIST &DU_list) {
  DU_list.clear();

  INT op_idx = Get_opid(op);
  if (op_idx == 0)
    return -1;

  if ((res < 0) || (res >= OP_MAX_FIXED_RESULTS))
    return -1;

  DU_list.Init(this, &DUDinfo_table[op_idx].dud_list[res], FALSE);
  return DU_list.size();
}

void DUD_REGION::Trace_DUD() {

  INT op_idx;

  fprintf(TFile, "\n%s %s\n%s", DBar,"                     Trace DUD", DBar);

  for (op_idx = First_opid(); op_idx <= Last_opid(); op_idx ++) {

    fprintf(TFile, "[%3d]", op_idx);
    OP *op = Get_op(op_idx);
    Print_OP_No_SrcLine(op);
    for (INT opnd = 0; opnd < OP_opnds(op); opnd ++) {
      if (!TN_is_DUDreg(OP_opnd(op, opnd))) continue;
      fprintf(TFile, "\t");
      Print_TN(OP_opnd(op, opnd), FALSE);
      DUD_LIST ud_list;
      if (Get_Use_Def(op, opnd, ud_list) > 0) {
	fprintf(TFile, ": ud");
	char *sep = " ";
	for (INT i = 0; i < ud_list.size(); i++) {
	  DUD_SITE ud_site;
	  ud_list.site(i, ud_site);
	  fprintf(TFile, "%s(%d,%d)", sep, ud_site.opid(), ud_site.idx());
	  sep = ", ";
	}
      }
      fprintf(TFile, "\n");
    }

    for (INT res = 0; res < OP_results(op); res ++) {
      if (!TN_is_DUDreg(OP_result(op, res))) continue;
      fprintf(TFile, "\t");
      Print_TN(OP_result(op, res), FALSE);
      DUD_LIST du_list;
      if (Get_Def_Use(op, res, du_list) > 0) {
	fprintf(TFile, ": du");
	char *sep = " ";
	for (INT i = 0; i < du_list.size(); i++) {
	  DUD_SITE du_site;
	  du_list.site(i, du_site);
	  fprintf(TFile, "%s(%d,%d)", sep, du_site.opid(), du_site.idx());
	  sep = ", ";
	}
      }
      fprintf(TFile, "\n");
    }
  }
}

DUD_REGION *Build_DUD_info(BB_REGION *bb_region, MEM_POOL *region_pool) {

  // Only support single entry regions.
  if (bb_region->entries.size() != 1)
    return NULL;

  MEM_POOL dud_pool;
  MEM_POOL_Initialize(&dud_pool, "Def-use/Use-def", TRUE);
  MEM_POOL_Push (&dud_pool);

  DUD_REGION *dudRegion = CXX_NEW(DUD_REGION(&dud_pool), region_pool);

  if (!dudRegion->Init( bb_region, region_pool )) {
    MEM_POOL_Pop( &dud_pool );
    MEM_POOL_Delete( &dud_pool );
    return NULL;
  }

  MEM_POOL_Pop( &dud_pool );
  MEM_POOL_Delete( &dud_pool );

  return dudRegion;

}

/*
static BOOL
check_Incrop(OP *defop) {

  if (defop == NULL) return FALSE;

  if (!OP_iadd(defop) && !OP_isub(defop)) return FALSE;

  // One arg must be a register, the other a constant
  INT op1_idx = TOP_Find_Operand_Use(OP_code(defop),OU_opnd1);
  INT op2_idx = TOP_Find_Operand_Use(OP_code(defop),OU_opnd2);

  INT useopnd;
  if (OP_iadd(defop)) {
    if (TN_is_constant(OP_opnd(defop, op2_idx)))
      useopnd = op1_idx;
    else if (TN_is_constant(OP_opnd(defop, op1_idx)))
      useopnd = op2_idx;
    else return FALSE;
  }
  else {
    useopnd = op1_idx;
    if (!TN_is_constant(OP_opnd(defop, op2_idx))) return FALSE;
  }

  if (!TN_is_register(OP_opnd(defop, useopnd))) return FALSE;

}

void Memop_to_Incrop(DUD_REGION *dud, OP* op) {

  // Look for Base TN
  INT base_idx = OP_find_opnd_use(op, OU_base);
  INT offset_idx = OP_find_opnd_use(op, OU_offset);

  if (base_idx < 0 || offset_idx < 0) return;

  TN *tn_base = OP_opnd(op, base_idx);
  TN *tn_offset = OP_opnd(op, offset_idx);

  // Only support for constant offset.
  if (!TN_is_constant(tn_offset)) return;    

  // Look for use-def link. If unique and a ADD/SUB, this is a candidate

  DUD_LIST ud_link, du_link;

  if (dud->Get_Use_Def(op, base_idx, ud_link) > 2) return;
  if (ud_link.size() == 0) return;

  INT ud_idx = 0;
  if (ud_link.size() == 2) {
    // Check that first def is outside the region
    if (ud_link.op(0) != NULL) return;
    ud_idx = 1;
  }

  OP *defop = ud_link.op(ud_idx);
  INT defidx = ud_link.idx(ud_idx);

  if (!check_Incrop(defop)) return;

  fprintf(TFile, "[%3d]", dud->Get_opid(op));
  Print_OP_No_SrcLine(op);
  if (ud_link.size() == 1) {
    fprintf(TFile, "\t[%3d]", dud->Get_opid(defop));
    Print_OP_No_SrcLine(defop);
  }

  dud->Get_Def_Use(defop, defidx, du_link);

  // No support for predicated code yet
  if (du_link.has_partial_def()) return;

  INT i;
  for (i = 0; i < du_link.size(); i++) {
    OP *useop = du_link.op(i);
    if (check_Incrop(useop)) {
      fprintf(TFile, "\t[%3d]", dud->Get_opid(useop));
      Print_OP_No_SrcLine(useop);
    }
  }

  fprintf(TFile, "\n");

  // TBD: Filter with dominance relation, 

  return;
}

void Perform_AutoMode_Opt() {

  MEM_POOL loop_descr_pool;
  MEM_POOL_Initialize(&loop_descr_pool, "loop_descriptors", TRUE);
  MEM_POOL_Push (&loop_descr_pool);
  BOOL trace_general = Get_Trace(TP_CGLOOP, 1);

  Calculate_Dominators();		//needed for loop recognition

  for (LOOP_DESCR *loop = LOOP_DESCR_Detect_Loops(&loop_descr_pool);
       loop;
       loop = LOOP_DESCR_next(loop)) {

    BB *head = LOOP_DESCR_loophead(loop);

    // Restrict to innermost loop to avoid processing large amount of
    // code ??
    // if (!BB_innermost(head)) continue;
    BB_REGION bbRegion(LOOP_DESCR_bbset(loop), &loop_descr_pool);

    bbRegion.Print();
    DUD_REGION *dudRegion = Build_DUD_info(&bbRegion, &loop_descr_pool);

    if (dudRegion != NULL) {
      dudRegion->Trace_DUD();

      fprintf(TFile, "******************** AutoMod ********************\n");

      for (OP *op = dudRegion->Begin_op();
	   op != dudRegion->End_op();
	   op = dudRegion->Next_op(op)) {
	Memop_to_Incrop(dudRegion, op);
      }

      fprintf(TFile, "*************************************************\n");
    }
  }

  MEM_POOL_Pop (&loop_descr_pool);
  MEM_POOL_Delete(&loop_descr_pool);

  Free_Dominators_Memory ();
}
*/
#endif
