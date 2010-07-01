/*
  Copyright (C) 2001, STMicroelectronics, All Rights Reserved.

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


//-*-c++-*-
// =======================================================================
// =======================================================================
//
//  Module: cg_dud.h
//
//  Description:
//  ============
//
//  This module builds the def-use and use-def links on a single-entry
//  BB_REGION
//
//  Exported Classes:
//  =================
//
//  DUD_REGION
//    Implements the def-use and use-def on a
//    single-entry/multiple-exit region. The region can contain
//    control-flow and loops.
//
//  DUD_LIST
//    Represents a list of def-use or use-def links
//
// 
//  Exported Functions:
//  ===================
//
//  DUD_REGION *DUD_REGION::DUD_REGION(MEM_POOL *mem_pool)
//    Constructor for the DUD_REGION object.
//
//  DUD_REGION::Init(BB_REGION *bb_region, MEM_POOL *region_pool);
//    Computes the Def-Use/Use-Def links between TNs in a BB_REGION.
//
//  DUD_REGION *Build_DUD_info(BB_REGION *bb_region, MEM_POOL *region_pool);
//    Create a DUD_REGION object, build the def-use/use-def links in
//    the region, and return a DUD_REGION object if successful.
//
//  DUD_REGION::Trace_DUD()
//    Trace the Def-Use/Use-Def links
//
//  INT DUD_REGION::Get_Use_Def(OP *op, INT opnd, DUD_LIST &ud_list);
//    Returns in ud_list the list of use-def links from a given (op,
//    opnd) in the region.
//
//  INT DUD_REGION::Get_Def_Use(OP *op, INT res, DUD_LIST &du_list);
//    Returns in du_list the list of def-use links from a given (op,
//    res) in the region.
//
//  INT DUD_REGION::Get_opid(OP *op)
//    Returns the index in topological order of an operation in the
//    region. Returns 0 if op is not in the region.
//
//  OP *DUD_REGION::Begin_op()
//  OP *DUD_REGION::End_op()
//  OP *DUD_REGION::Next_op()
//    These functions are used to iterate on the operations in the
//    DUD_REGION, in topological order:
//      for (op = dudRegion.Begin_op(); op != dudRegion.End_op(); op = dudRegion.Next_op(op))
//
//  OP *DUD_REGION::Last_op()
//  OP *DUD_REGION::Prev_op()
//    These functions are used to iterate on the operations in the
//    DUD_REGION, in reverse topological order:
//      for (op = dudRegion.Last_op(); op != NULL; op = dudRegion.Prev_op(op))
//
//  INT DUD_LIST::size()
//    Returns the number of elements in a def-use or use-def list
//
//  OP *DUD_LIST::op(INT listIdx)
//    Returns the operation that is referenced by a DU or UD link at
//    index listIdx in the def-use or use-def list. Will return NULL
//    in case of a DU or UD link that refers to an operation outside
//    the region.
//
//  INT DUD_LIST::idx(INT listIdx)
//    Returns the index of the operand or result in the operation that
//    is referenced by a DU or UD link at index listIdx in the def-use
//    or use-def list.
//
// BOOL DUD_LIST::is_def(INT listIdx)
//    Returns whether DUD_LIST::idx(listIdx) returned an operand or a
//    result index in the operation that is referenced by a DU or UD
//    link at index listIdx in the def-use or use-def list.
//
// BOOL DUD_LIST::has_partial_def()
//    Returns TRUE if this is a Def-Use list and the list contains
//    some definitions. These definitions are actually predicated
//    definitions that introduces implicit uses.
//
// =======================================================================
// ======================================================================= */

#ifdef TARG_ST

#if __GNUC__ >=3 || defined(_MSC_VER)
#include <climits>	// For *MAX 
#endif

#include "bb.h"
#include "cg_loop.h"
#include "findloops.h"

#ifndef CG_DUD_INCLUDED
#define CG_DUD_INCLUDED

class DUD_SITE;
class DUD_LIST;
class DUD_REGION;

class DUD_SITE {
  friend class DUD_REGION;
  friend class DUD_LIST;
 private:

  // Statically evaluate the number of bits required to encode
  // an index on operands+results
#if   (OP_MAX_FIXED_RESULTS + OP_MAX_FIXED_OPNDS <= 8)
  static const int IDX_WIDTH = 3;
#elif (OP_MAX_FIXED_RESULTS + OP_MAX_FIXED_OPNDS <= 16)
  static const int IDX_WIDTH = 4;
#elif (OP_MAX_FIXED_RESULTS + OP_MAX_FIXED_OPNDS <= 32)
  static const int IDX_WIDTH = 5;
#elif (OP_MAX_FIXED_RESULTS + OP_MAX_FIXED_OPNDS <= 64)
  static const int IDX_WIDTH = 6;
#elif (OP_MAX_FIXED_RESULTS + OP_MAX_FIXED_OPNDS <= 128)
  static const int IDX_WIDTH = 7;
#else
#error "Need to extend IDX_WIDTH value here"
#endif
  static const int IDX_MASK = ((1<<IDX_WIDTH)-1);

  OP *oper;
  INT oper_id;
  INT res_opnd;
  BOOL is_result;

 protected:
  typedef unsigned int DUDsite_t;

  static INT max_op_count() {
    return (UINT_MAX >> IDX_WIDTH);
  }

  static INT max_opnd_count() {
    return (1<<IDX_WIDTH);
  }

  static DUDsite_t makeDef(UINT op_idx, UINT res_idx) {
    Is_True(res_idx <= IDX_MASK, ("DUDsite_makeDef: Internal Error"));
    return ((op_idx << IDX_WIDTH)|res_idx);
  }

  static DUDsite_t makeUse(UINT op_idx, UINT opnd_idx) {
    Is_True(opnd_idx <= IDX_MASK, ("DUDsite_make: Internal Error"));
    return ((op_idx << IDX_WIDTH)|(OP_MAX_FIXED_RESULTS+opnd_idx));
  }

  static UINT get_opid(DUDsite_t dudlink) { return (dudlink >> IDX_WIDTH); };
  static UINT get_opnd(DUDsite_t dudlink, BOOL *result) {
    INT opnd = dudlink & IDX_MASK;
    *result = TRUE;
    if (opnd >= OP_MAX_FIXED_RESULTS) {
      *result = FALSE;
      opnd -= OP_MAX_FIXED_RESULTS;
    }
    return opnd;
  }

  void Init(DUD_REGION *dud, DUDsite_t site);

  OP *op() { return oper; };
  INT opid() { return oper_id; };
  INT idx() { return res_opnd; };
  BOOL is_def() { return is_result; };
};

class DUD_LIST {
  friend class DUD_REGION;
 private:

  DUD_REGION *dud_region;
  std::vector<DUD_SITE::DUDsite_t> DU_UD_list;
  BOOL is_UD;

 public:
  typedef std::list<DUD_SITE::DUDsite_t> DUDlist_t;

 protected:

  void Init(DUD_REGION *dud, DUDlist_t *dud_list, BOOL list_is_UD) {
    dud_region = dud;
    for (DUDlist_t::iterator iter = dud_list->begin ();
	 iter != dud_list->end ();
	 iter++) {
      DU_UD_list.push_back(*iter);
    }
    is_UD = list_is_UD;
  }

  void clear() { DU_UD_list.clear(); }

  void site(INT idx, DUD_SITE &dud_site) {
    dud_site.Init(dud_region, DU_UD_list[idx]);
  }

 public:

  INT size() { return DU_UD_list.size(); };

  // Interface copied from DUD_SITE
  OP *op(INT list_idx) { DUD_SITE dud_site; site(list_idx, dud_site); return dud_site.op(); };
  INT idx(INT list_idx) { DUD_SITE dud_site; site(list_idx, dud_site); return dud_site.idx(); };
  BOOL is_def(INT list_idx) { DUD_SITE dud_site; site(list_idx, dud_site); return dud_site.is_def(); };

  BOOL has_partial_def() {
    if (!is_UD) {
      INT i;
      for (i = 0; i < DU_UD_list.size(); i++) {
	DUD_SITE ud_site;
	site(i, ud_site);
	if (ud_site.is_def())
	  return TRUE;
      }
    }
    return FALSE;
  }
};

// typedef std::vector< std::pair<OP *, INT> > dud_link_t;

class DUD_REGION {
  friend class DUD_SITE;
   // For each operation, define a struct that olds def-use and use-def
  // links.
  private:

  typedef struct DUDinfo {
    OP *op;
    DUD_LIST::DUDlist_t dud_list[OP_MAX_FIXED_RESULTS+OP_MAX_FIXED_OPNDS];
  } DUDinfo_t;

  MEM_POOL *_loc_mem_pool;

  // Define a map from an OP * to a unique ID in the region.
  OP_MAP DUD_opid_map;

  // Define an array to store all the DUDinfo elements for all
  // operations in a region.
  INT DUDinfo_size;
  DUDinfo_t *DUDinfo_table;

  MEM_POOL *dud_pool() { return _loc_mem_pool; };

  void Set_size(INT size) { DUDinfo_size = size; };

  BOOL DUD_check_size(INT op_count) {
    return (op_count < DUD_SITE::max_op_count());
  }

  void DUD_Allocate(INT op_count, MEM_POOL *pool) {
    Set_size(op_count);
    DUDinfo_table = (DUDinfo_t *) CXX_NEW_ARRAY( DUDinfo_t, DUDinfo_size, pool );
  }

  void Set_op(INT opid, OP *op) { DUDinfo_table[opid].op = op; };

  void Set_opid(OP *op, INT opid) { OP_MAP32_Set(DUD_opid_map, op, opid); };

  INT First_opid() { return 1; };
  INT Last_opid() { return DUDinfo_size-1; };

  void TNuse_Push_DUDsite(INT opid, INT opnd, DUD_SITE::DUDsite_t site) {
    DUDinfo_table[opid].dud_list[opnd+OP_MAX_FIXED_RESULTS].push_back(site);
  }

  void TNdef_Push_DUDsite(INT opid, INT res, DUD_SITE::DUDsite_t site) {
    DUDinfo_table[opid].dud_list[res].push_back(site);
  }

 protected:
  OP *Get_op(INT opid) { return ((opid > 0) && (opid < DUDinfo_size)) ? DUDinfo_table[opid].op : NULL; };

  public:

  DUD_REGION(MEM_POOL *mem_pool)
    : _loc_mem_pool(mem_pool) ,
      DUDinfo_table ( NULL),
      DUDinfo_size ( 0) {
    Is_True(((OP_MAX_FIXED_RESULTS+OP_MAX_FIXED_OPNDS) <= DUD_SITE::max_opnd_count()), ("Internal Error in DUD_REGION"));
    DUD_opid_map = OP_MAP32_Create();
  };

  ~DUD_REGION() {
    OP_MAP_Delete(DUD_opid_map);
  }

  BOOL Init( BB_REGION *bb_region, MEM_POOL *region_pool );

  INT Get_opid(OP *op) { return OP_MAP32_Get(DUD_opid_map, op); };

  INT Get_Use_Def(OP *op, INT opnd, DUD_LIST &UD_list);
  INT Get_Def_Use(OP *op, INT res, DUD_LIST &DU_list);

  OP *Begin_op() { return DUDinfo_table[1].op; };
  OP *End_op() { return NULL; };
  OP *Last_op() { return DUDinfo_table[DUDinfo_size-1].op; };
  OP *Next_op(OP *op) {
    INT opid = Get_opid(op);
    return ((opid) > 0 && (opid < (DUDinfo_size-1))) ? DUDinfo_table[opid+1].op : NULL;
  }
  OP *Prev_op(OP *op) {
    INT opid = Get_opid(op);
    return ((opid) > 1 && (opid < DUDinfo_size)) ? DUDinfo_table[opid-1].op : NULL;
  }

  void Trace_DUD();  
};

DUD_REGION *Build_DUD_info(BB_REGION *bb_region, MEM_POOL *region_pool);
void Perform_AutoMode_Opt();

#endif /* CG_DUD_INCLUDED */

#endif /* TARG_ST */
