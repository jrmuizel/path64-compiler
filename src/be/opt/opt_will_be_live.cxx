/*
 * Copyright 2010 Tsinghua University.  All Rights Reserved.
 */

//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of version 2 of the GNU General Public License as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it would be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Further, this software is distributed without any warranty that it
// is free of the rightful claim of any third person regarding
// infringement  or the like.  Any license provided herein, whether
// implied or otherwise, applies only to this software file.  Patent
// licenses, if any, provided herein do not apply to combinations of
// this program with other software, or any other product whatsoever.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
//
// Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
// Mountain View, CA 94043, or:
//
// http://www.sgi.com
//
// For further information regarding this notice, see:
//
// http://oss.sgi.com/projects/GenInfo/NoticeExplan
//
// ====================================================================
// ====================================================================


#ifdef USE_PCH
#include "opt_pch.h"
#endif // USE_PCH
#pragma hdrstop


#include "opt_etable.h"
#include "opt_dfs.h"
#include "tracing.h"

class WILL_BE_DEAD_SEARCH {
private:
  static BOOL         _tracing;
  static EXP_WORKLST *_worklst;

  EXP_WORKLST *Worklst(void) const       { return _worklst; }

  EXP_PHI *const _cur_node;

public:
  static void Set_tracing(const BOOL tracing) { _tracing = tracing; }
  static void Set_worklst(EXP_WORKLST *const worklst)
    { _worklst = worklst; }

  BOOL        Tracing(void) const { return _tracing; }

  typedef     EXP_PHI            node_type;
  typedef     USE_LIST_ENTRY     adj_list_type;
  typedef     USE_LIST_ITER      adj_list_iter_type;
  typedef     EXP_PHI_OCC_ITER   node_iterator;

  WILL_BE_DEAD_SEARCH(node_type *const cur_node) : _cur_node(cur_node)
    {
      Is_True(Worklst() != NULL,
	      ("WILL_BE_DEAD_SEARCH: must not construct with NULL worklist"));
    }

  void Set_seen(node_type *phi) const
    { phi->Reset_will_be_live(); }

  BOOL Seen(const node_type *const phi) const
    { return !phi->Will_be_live(); }

  /* ARGSUSED */
  void Reach_from_to(const node_type *const def_phi,
		     const INT              opnd_idx,
		     const node_type *const use_phi) const
    { }

  BOOL Start_from(const node_type *const phi) const
    {
      for (INT i = 0; i < phi->Opnd_count(); i++) {
        // there existed a NULL operand and no speculative insertion there
	if (!phi->Opnd(i) && !phi->Speculative_insert(i)) {
	  return TRUE;
	}
      }
      return FALSE;
    }

  /* ARGSUSED */
  BOOL Continue_from_to(const node_type *const def_phi,
			const INT              opnd_idx,
			const node_type *const use_phi) const
    { 
    
      Is_True(use_phi->Opnd(opnd_idx) == def_phi->Result(),
              ("The propagation is not along the def-use chain!")); 
      if(!use_phi->Has_real_occ(opnd_idx) && 
         !use_phi->Speculative_insert(opnd_idx))
        return TRUE;
      else
        return FALSE;
    } 

  /* ARGSUSED */
  void Postorder_processing(node_type *const phi) const
    { }

  node_type *Current_node(void) const { return _cur_node; }

  adj_list_type *Neighbors(const node_type *const def_phi) const
    { return def_phi->Uses(); }

  EXP_OCCURS_CONTAINER &Nodes(void) const
    { return Worklst()->Phi_occurs(); }

  const char *Search_name(void) const
    { return "WILL_BE_DEAD_SEARCH";}
};

BOOL   WILL_BE_DEAD_SEARCH::_tracing = FALSE;
EXP_WORKLST *WILL_BE_DEAD_SEARCH::_worklst = NULL;

// Just for MC-SSAPRE only
// Will_be_live equals to Will_b_avail at classical SSAPRE
// Why we don't directly use Will_b_avail, that's because 
// Will_b_avail equals to Cant_be_avail && Stop() in SSAPRE
// There is no direct flag to tell Will_b_avail
void
EXP_WORKLST::Compute_will_be_live(ETABLE *etable)
{
  Is_Trace(etable->Tracing(),
	   (TFile, "==== On entry to EXP_WORKLST::Compute_fully_avail\n"));
  Is_Trace_cmd(etable->Tracing(), Print(TFile));

  // Compute the local def-use information and calculate availability.
  //
  //Compute_du_info(etable->Etable_local_pool()); // Allocates du-info

  // Determine whether there will be insertion at the PHI_PRED
  {
    // We can use EXP_PHI_OCC_ITER directly
    EXP_ALL_OCCURS_ITER exp_occ_iter(NULL,
                                     NULL,
                                     Phi_occurs().Head(),
                                     NULL,
                                     NULL); /* only real occurrences */
    EXP_OCCURS *occ;
    FOR_ALL_NODE(occ, exp_occ_iter, Init()) {
      EXP_PHI *phi = occ->Exp_phi();
      BB_LIST_ITER pred_bb_iter;
      BB_NODE *pred_bb;
      BB_NODE *phi_bb = phi->Bb();
      // Intialize all PHI to be WILL_BE_LIVE
      phi->Set_will_be_live();
      FOR_ALL_ELEM(pred_bb, pred_bb_iter, Init(phi_bb->Pred())) {
        const INT32 opnd_num = phi_bb->Pred()->Pos(pred_bb);
        // Reset first
        phi->Reset_speculative_insert(opnd_num);
        if(occ->Need_speculative_insertion(opnd_num, etable))
          phi->Set_speculative_insert(opnd_num);
      }
    }
  }

  WILL_BE_DEAD_SEARCH::Set_worklst(this);
  WILL_BE_DEAD_SEARCH::Set_tracing(etable->Tracing());

  WILL_BE_DEAD_SEARCH live_srch(NULL);

  Perform_dfs(live_srch);

  Is_Trace(etable->Tracing(),
	   (TFile, "==== After EXP_WORKLST::Compute_fully_avail\n"));
  Is_Trace_cmd(etable->Tracing(), Print(TFile));
} // EXP_WORKLST::Compute_will_be_live

