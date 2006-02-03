/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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


#ifdef USE_PCH
#include "lno_pch.h"
#endif // USE_PCH
#pragma hdrstop

#include "pf_manual.h"
#include "wn_map.h"
#include "lwn_util.h"
#include "pf_cg.h"
#ifdef TARG_ST
#include "pf_ref.h"
#endif

extern WN_MAP LNO_Info_Map;

MANUAL_PREFETCH_SYMBOLS* mpf_syms = NULL;

#ifdef TARG_ST
// FdF 20050203: Fine tuning of prefetch optimization
// Bit 0x1: Improve association from user prefetch to memory reference
// Bit 0x2: Scheduling of user and auto prefetch
BE_EXPORTED extern INT32 Prefetch_Optimize;
#endif
/***********************************************************************
 *
 * Given a ref and a pref, make a prefetch map from the ref (load/store)
 * to the prefetch.
 *
 ***********************************************************************/
#ifdef TARG_ST
static BOOL Analyze_addr(WN *addr, ST_IDX *addr_st, ST_IDX *addr_idx, int *offset);
static BOOL Same_cache_line(WN *base_wn, WN *ref_wn);
static void Set_Pref_Node(PF_POINTER* pf_info, WN *ref_wn, WN* pf_wn, PF_LOOPNODE *loopnode);

static WN *Map_Memop_To_Prefetch (WN* ref_wn, WN* pf_wn, PF_LOOPNODE *loopnode, WN *base_ref) {
#else
static void Map_Memop_To_Prefetch (WN* ref_wn, WN* pf_wn) {
#endif
  VB_PRINT (printf ("Found a match, setting map\n");
            printf ("Ref : "); dump_tree (ref_wn);  printf ("\n");
            printf ("Pref: "); dump_tree (pf_wn); printf ("\n"));
  PF_POINTER* tmp = (PF_POINTER*) WN_MAP_Get (WN_MAP_PREFETCH, ref_wn);
  if (tmp == NULL) {
#ifdef TARG_ST
    if (base_ref != NULL) {
      tmp = (PF_POINTER*) WN_MAP_Get (WN_MAP_PREFETCH, base_ref);
      Is_True(PF_PTR_wn_pref_1L(tmp) == pf_wn, ("Map_Memop_To_Prefetch: base_ref does not match pf_wn\n"));
      /*
       * We already created a PF_POINTER for pf_wn. Check if ref_wn is
       * prefetched by this pf_wn or not.
       */
      if (Same_cache_line(base_ref, ref_wn)) {
	WN_MAP_Set (WN_MAP_PREFETCH, ref_wn, tmp);
	VB_PRINT (printf("Map_Memop_To_Prefetch (ref %p -> pref %p)\n", ref_wn, pf_wn));
      }
      // This ref_wn will be mapped with another pf_wn
      return base_ref;
    }
#endif
    extern MEM_POOL PF_CG_mpool;
    tmp = CXX_NEW (PF_POINTER, &PF_CG_mpool);
    PF_PTR_wn_pref_1L(tmp) = NULL;
    PF_PTR_lrnum_1L(tmp) = 0;
    PF_PTR_distance_1L(tmp) = 0;
    PF_PTR_wn_pref_2L(tmp) = NULL;
    PF_PTR_lrnum_2L(tmp) = 0;
    PF_PTR_distance_2L(tmp) = 0;
    PF_PTR_flag(tmp) = 0;
    WN_MAP_Set (WN_MAP_PREFETCH, ref_wn, tmp);
#ifdef TARG_ST
    VB_PRINT (printf("Map_Memop_To_Prefetch (ref %p -> pref %p)\n", ref_wn, pf_wn));
    if (Prefetch_Optimize&0x1)
      base_ref = ref_wn;
#endif
    if (WN_pf_stride_1L(pf_wn)) {
      PF_PTR_wn_pref_1L(tmp) = pf_wn;
      PF_PTR_distance_1L(tmp) = WN_offset(pf_wn);
      PF_PTR_set_conf_1L(tmp, WN_pf_confidence(pf_wn));
    }
    if (WN_pf_stride_2L(pf_wn)) {
      PF_PTR_wn_pref_2L(tmp) = pf_wn;
      PF_PTR_distance_2L(tmp) = WN_offset(pf_wn);
      PF_PTR_set_conf_2L(tmp, WN_pf_confidence(pf_wn));
    }
  }
  else {
    /* a preexisting prefetch map on the ref.
     * check cache level.
     */
    if (WN_pf_stride_1L(pf_wn) && (PF_PTR_wn_pref_1L(tmp) == NULL)) {
      PF_PTR_wn_pref_1L(tmp) = pf_wn;
      PF_PTR_distance_1L(tmp) = WN_offset(pf_wn);
      PF_PTR_set_conf_1L(tmp, WN_pf_confidence(pf_wn));
    }
    if (WN_pf_stride_2L(pf_wn) && (PF_PTR_wn_pref_2L(tmp) == NULL)) {
      PF_PTR_wn_pref_2L(tmp) = pf_wn;
      PF_PTR_distance_2L(tmp) = WN_offset(pf_wn);
      PF_PTR_set_conf_2L(tmp, WN_pf_confidence(pf_wn));
    }
    DevWarn ("Preexisting prefetch map: duplicate prefetch?");
#ifdef TARG_ST
    return base_ref;
#endif
  }

#ifdef TARG_ST
  if (loopnode == NULL) {
    VB_PRINT (printf("PREFETCH line %d is embedded in a WHILE loop", Srcpos_To_Line(WN_Get_Linenum(pf_wn))));
  }
  else
    Set_Pref_Node(tmp, ref_wn, pf_wn, loopnode);

  return base_ref;
#endif
}


/***********************************************************************
 *
 * For each prefetch, try to find the corresponding ref, 
 * and match them up.
 *
 ***********************************************************************/

#ifdef TARG_ST
static BOOL
Analyze_addr(WN *addr, ST_IDX *addr_st, ST_IDX *addr_idx, int *offset) {

  OPERATOR opr = WN_operator(addr);

  switch (opr) {
  case OPR_LDA:
  case OPR_LDID:
    *addr_st = WN_st_idx(addr);
    *addr_idx = 0;
    *offset = WN_offset(addr);
    break;

  case OPR_ARRAY:
    {
      if (WN_kid_count(addr) != 3)
	return FALSE;
      *offset = 0;

      WN *base = WN_array_base(addr);
      OPERATOR op = WN_operator(base);

      if (WN_operator(base) == OPR_ADD || WN_operator(base) == OPR_SUB) {
	int negVal = (WN_operator(base) == OPR_ADD ? 1 : -1);
	WN *offset_wn = WN_kid1(base);
	base = WN_kid0(base);
	if (WN_operator(offset_wn) != OPR_INTCONST) {
	  WN *swap = base;
	  base = offset_wn;
	  offset_wn = swap;
	}
	// just let it go to bucket 0
	if (WN_operator(offset_wn) != OPR_INTCONST)
	  return FALSE;
	*offset += WN_const_val(offset_wn) * negVal;

	op = WN_operator(base);
      }

      if ((op != OPR_LDID) && (op != OPR_LDA))
	return FALSE;

      *addr_st = WN_st_idx(base);
      if (op == OPR_LDA)
	*offset += WN_offset(base);

      int elt_size = WN_element_size(addr);
      WN *index = WN_array_index(addr, 0);

      if (WN_operator(index) == OPR_INTCONST) {
	*offset += WN_const_val(index) * elt_size;
	*addr_idx = 0;
      }
      else {
	if (WN_operator(index) == OPR_CVTL)
	  index = WN_kid0(index);
	if (WN_operator(index) == OPR_ADD || WN_operator(index) == OPR_SUB) {
	  int negVal = (WN_operator(index) == OPR_ADD ? 1 : -1);
	  WN *offset_wn = WN_kid1(index);
	  index = WN_kid0(index);
	  if (WN_operator(offset_wn) != OPR_INTCONST) {
	    WN *swap = index;
	    index = offset_wn;
	    offset_wn = swap;
	  }
	  if (WN_operator(offset_wn) != OPR_INTCONST)
	    return FALSE;
	  *offset += WN_const_val(offset_wn) * negVal * elt_size;
	}

	if (WN_operator(index) != OPR_LDID)
	  return FALSE;
	*addr_idx = WN_st_idx(index);
      }

    }
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

static BOOL
Same_cache_line(WN *base_wn, WN *ref_wn) {
  INT distance = 0;
  ST_IDX base_st, ref_st, base_idx, ref_idx;
  int base_offset, ref_offset;

  if ( WN_operator(base_wn) == OPR_ILOAD )
    base_wn = WN_kid0(base_wn);
  else if ( WN_operator(base_wn) == OPR_ISTORE )
    base_wn = WN_kid1(base_wn);

  if ( WN_operator(ref_wn) == OPR_ILOAD )
    ref_wn = WN_kid0(ref_wn);
  else if ( WN_operator(ref_wn) == OPR_ISTORE )
    ref_wn = WN_kid1(ref_wn);

  if (Analyze_addr(base_wn, &base_st, &base_idx, &base_offset) &&
      Analyze_addr(ref_wn, &ref_st, &ref_idx, &ref_offset) &&
      (base_st == ref_st) && (base_idx == ref_idx)) {
    int distance = (ref_offset - base_offset);
    return (distance >= 0 && distance < Cache.LineSize(1));
  }
  return FALSE;
}

static void
Set_Pref_Node(PF_POINTER* pf_info, WN *ref_wn, WN* pf_wn, PF_LOOPNODE *loopnode) {

  // Get the expression of the memory reference, find the UGS this
  // memory reference belongs to.

  if (!(Prefetch_Optimize&0x1))
    return;

  WN *ref_addr;

  if (ref_wn == NULL) {
    ref_addr = WN_kid0(pf_wn);
  } else {
    if ((WN_operator(WN_kid0(pf_wn)) == OPR_LDA) ||
	(WN_operator(WN_kid0(pf_wn)) == OPR_LDID))
      ref_addr = ref_wn;
    else
      ref_addr = ((WN_operator(ref_wn) == OPR_ILOAD) ? WN_kid0(ref_wn) : WN_kid1(ref_wn)); 
  }

  PF_UGS *pf_ugs = loopnode->Get_UGS(ref_addr);

  PF_VOLUME pf_volume;
  PF_LOCLOOP pf_locloop;
  pf_locloop.Update(loopnode->Get_Depth(), pf_volume, 3);
  PF_DESC* pfdesc = pf_ugs ? pf_ugs->GetManualPFVec(level_1, pf_locloop) : NULL;

  UINT32 flag = 0;
  PF_SET_READ(flag);
  PF_SET_MANUAL(flag);
  PF_SET_CONFIDENCE (flag, 3);

  if (!pfdesc || pfdesc->Kind(level_1) == all)
    PF_SET_STRIDE_1L (flag, 1);
  else {
    Is_True (pfdesc->Kind(level_1) == vec,
	     ("Gen_Pref_Node: prefetch when kind is none\n"));
    Is_True (pfdesc->Vec(level_1),
	     ("Gen_Pref_Node: kind is vec, but vector is NULL\n"));
    mINT16* prefetch_vec = pfdesc->Vec(level_1); 
    mINT16 depth = loopnode->Get_Depth();
    if (prefetch_vec[depth]) {
      PF_SET_STRIDE_1L (flag, prefetch_vec[depth]);
    }
    else {
      PF_SET_STRIDE_1L (flag, 1);
    }
  }

  if (pf_ugs) {
    if (pf_ugs->Get_Stride_One_Size() > Cache.LineSize(1))
      PF_SET_STRIDE_1L (flag, 1);
    else {
      PF_SET_STRIDE(flag);
      if (PF_GET_STRIDE_1L(flag) != Cache.LineSize(1)/pf_ugs->Get_Stride_One_Size()) {
	int new_stride = Cache.LineSize(1)/pf_ugs->Get_Stride_One_Size();
	DevWarn("Stride for User prefetch was set to %d, changed to %d.", PF_GET_STRIDE_1L(flag), new_stride);
	PF_SET_STRIDE_1L (flag, new_stride);
      }
    }
  }

  WN_set_flag(pf_wn, flag);

  if (pf_info) {
    PF_PTR_wn_pref_1L(pf_info) = pf_wn;
    PF_PTR_distance_1L(pf_info) = WN_offset(pf_wn);
    PF_PTR_set_conf_1L(pf_info, WN_pf_confidence(pf_wn));
  }
}
#endif

#ifdef TARG_ST
void SINGLE_ARRAY_REF_PREF::Match_Refs_Prefs (PF_LOOPNODE *loopnode) {
#else
void SINGLE_ARRAY_REF_PREF::Match_Refs_Prefs () {
#endif
  INT i, j;
  for (i=0; i<_pref_da.Elements(); i++) {
    WN* pf_wn = _pref_da[i];
    WN* pragma_wn = _pragma_da[i];

    WN* addr_wn = WN_kid0(pf_wn);
    OPERATOR opr = WN_operator(addr_wn);
#ifdef TARG_ST
    WN *base_ref = NULL;
#endif
    switch (opr) {
    case OPR_LDA:
#ifdef TARG_ST
    case OPR_LDID:
#endif
      // simple variable prefetch -- find an ldid/stid
      for (j=0; j<_ref_da.Elements(); j++) {
        WN* ref_wn = _ref_da[j];
        OPERATOR ref_opr = WN_operator(ref_wn);
        if ((ref_opr != OPR_LDID) && (ref_opr != OPR_STID)) continue;
        if ((WN_st(addr_wn) != WN_st(ref_wn)) ||
            (WN_offset(addr_wn) != WN_offset(ref_wn))) continue;
        // matching variables, so go for it
#ifdef TARG_ST
        base_ref = Map_Memop_To_Prefetch (ref_wn, pf_wn, loopnode, base_ref);
	if (!(Prefetch_Optimize&0x1)) break;
#else
        Map_Memop_To_Prefetch (ref_wn, pf_wn);
        break;
#endif
      }
#ifdef TARG_ST
      if (!base_ref && loopnode)
	Set_Pref_Node(NULL, NULL, pf_wn, loopnode);
      if (!base_ref)
#endif
      if (j == _ref_da.Elements()) {
        // didn't find a matching refence, issue a warning.
        DevWarn("Could not match manual prefetch to ref, issuing prefetch anyway");
        // fdump_tree(stderr, pf_wn);
      }
      break;
    case OPR_ARRAY:
      // prefetch of an array reference. 
      // Find an iload/istore with array as the addr expr
#ifdef TARG_ST
      ST_IDX ref_st, prefetch_st, ref_idx, prefetch_idx;
      int ref_offset, prefetch_offset;
      if (!Analyze_addr(addr_wn, &prefetch_st, &prefetch_idx, &prefetch_offset))
	j = _ref_da.Elements();
      else
#endif
      for (j=0; j<_ref_da.Elements(); j++) {
        WN* ref_wn = _ref_da[j];
        OPERATOR ref_opr = WN_operator(ref_wn);
        if ((ref_opr != OPR_ILOAD) && (ref_opr != OPR_ISTORE)) continue;
        WN* ref_addr_wn;
        ref_addr_wn = ((ref_opr == OPR_ILOAD) ?
                       WN_kid0(ref_wn) :
                       WN_kid1(ref_wn)); 
        if (WN_operator(ref_addr_wn) != OPR_ARRAY) continue;
#ifdef TARG_ST
	if (!Analyze_addr(ref_addr_wn, &ref_st, &ref_idx, &ref_offset)) continue;
	else if ((ref_st != prefetch_st) || (ref_idx != prefetch_idx)) continue;
#else
        if (!Tree_Equiv (addr_wn, ref_addr_wn)) continue;
#endif
        // ok, they're the same, go for it.
#ifdef TARG_ST
        base_ref = Map_Memop_To_Prefetch (ref_wn, pf_wn, loopnode, base_ref);
	PF_POINTER* tmp = (PF_POINTER*) WN_MAP_Get (WN_MAP_PREFETCH, ref_wn);
	if (tmp != NULL && PF_PTR_wn_pref_1L(tmp) == pf_wn)
	  PF_PTR_distance_1L(tmp) = prefetch_offset + WN_offset(pf_wn) - ref_offset;
	if (!(Prefetch_Optimize&0x1)) break;
#else
        Map_Memop_To_Prefetch (ref_wn, pf_wn);
        break;
#endif
      }
#ifdef TARG_ST
      if (!base_ref && loopnode)
	Set_Pref_Node(NULL, NULL, pf_wn, loopnode);
      if (!base_ref)
#endif
      if (j == _ref_da.Elements()) {
        // didn't find a matching refence, issue a warning.
        DevWarn("Could not match manual prefetch to ref, issuing prefetch anyway");
        // fdump_tree(stderr, pf_wn);
      }
      break;
    default:
      // some random expression, just do a brute force compare
      for (j=0; j<_ref_da.Elements(); j++) {
        WN* ref_wn = _ref_da[j];
        OPERATOR ref_opr = WN_operator(ref_wn);
        if ((ref_opr != OPR_ILOAD) && (ref_opr != OPR_ISTORE)) continue;
        WN* ref_addr_wn;
        ref_addr_wn = ((ref_opr == OPR_ILOAD) ?
                       WN_kid0(ref_wn) :
                       WN_kid1(ref_wn)); 
        if (!Tree_Equiv (addr_wn, ref_addr_wn)) continue;
        // ok, they're the same, go for it.
#ifdef TARG_ST
        base_ref = Map_Memop_To_Prefetch (ref_wn, pf_wn, loopnode, base_ref);
	if (!(Prefetch_Optimize&0x1)) break;
#else
        Map_Memop_To_Prefetch (ref_wn, pf_wn);
        break;
#endif
      }
#ifdef TARG_ST
      if (!base_ref && loopnode)
	Set_Pref_Node(NULL, NULL, pf_wn, loopnode);
      if (!base_ref)
#endif
      if (j == _ref_da.Elements()) {
        // didn't find a matching refence, issue a warning.
        DevWarn("Could not match manual prefetch to ref, issuing prefetch anyway");
        // fdump_tree(stderr, pf_wn);
      }
      break;
    }
  }
}

/***********************************************************************
 *
 * Given a prefetch whirl node from the front-end,
 * find the appropriate base symbol repository
 * and store it. 
 * Use pragma_wn for the size of this array
 *  (NULL implies no size value was supplied in pragma)
 *
 ***********************************************************************/
void SINGLE_LOOP::Insert_Pref (WN* pragma_wn, WN* pf_wn) {

  VB_PRINT(printf ("Insert_Pref: Manual prefetch in loop 0x%p: \n", this);
           dump_tree (pf_wn));

  /* make the prefetch legal */
  WN_pf_set_manual(pf_wn);
  WN_pf_set_confidence(pf_wn, 3);

  // now let's find the appropriate ST entry
  SYMBOL *sym = NULL;
  WN* addr_wn = WN_kid0(pf_wn);
  OPERATOR opr = WN_operator(addr_wn);
  switch (opr) {
  case OPR_LDA:
#ifdef TARG_ST
    case OPR_LDID:
#endif
    // simple prefetch of a variable
    sym = CXX_NEW (SYMBOL(addr_wn), PF_mpool);
    break;
  case OPR_ARRAY:
  {
    // prefetch of an array reference
    WN* array_base = WN_array_base(addr_wn);
    OPERATOR op = WN_operator(array_base);
    // Base might be a random expression, check for that
    if ((op == OPR_LDID) || (op == OPR_LDA)) {
      sym = CXX_NEW (SYMBOL(array_base), PF_mpool);
    }
    break;
  }
  default:
    // some random expression, pointers, etc. Go to bucket 0
    break;
  }

  if (sym) {
    VB_PRINT(printf ("    Insert_Pref into bucket for sym ");
             sym->Print(stdout); printf ("\n"));
    // since sym is non-NULL, search from bucket 1 onwards
    INT i;
    for (i=1; i<_sarp_list.Elements(); i++) {
      if (_sarp_list[i]->Same_Base_Symbol(sym)) {
        _sarp_list[i]->Insert_Pref (pragma_wn, pf_wn);
        mpf_syms->Enter (sym, (pragma_wn ? WN_pragma_arg2(pragma_wn) : 0));
        break;
      }
    }
    if (i == _sarp_list.Elements()) {
      /* didn't find it, so create a new entry */
      SINGLE_ARRAY_REF_PREF* sarp =
        CXX_NEW (SINGLE_ARRAY_REF_PREF(sym),PF_mpool);
      sarp->Insert_Pref (pragma_wn, pf_wn);
      _sarp_list[_sarp_list.Newidx()] = sarp;
      mpf_syms->Enter (sym, (pragma_wn ? WN_pragma_arg2(pragma_wn) : 0));
    }
    CXX_DELETE (sym, PF_mpool);
  }
  else {
    VB_PRINT(printf ("    Insert_Pref into bucket 0\n"));
    // a bucket 0 candidate. Bucket 0 always exists
    _sarp_list[0]->Insert_Pref (pragma_wn, pf_wn);
  }
}

/***********************************************************************
 *
 * Given a memory access whirl node (one of LDID, STID, ILOAD, ISTORE),
 * find the appropriate base symbol repository and store it.
 *
 ***********************************************************************/
void SINGLE_LOOP::Insert_Ref  (WN* ref_wn) {

  VB_PRINT(printf ("Insert_Ref: Reference in loop 0x%p: \n", this);
           dump_tree (ref_wn));

  // First let's try to find the ST entry for it
  SYMBOL *sym = NULL;
  OPERATOR opr = WN_operator(ref_wn);
  switch (opr) {
  case OPR_LDID:
  case OPR_STID:
    sym = CXX_NEW (SYMBOL(ref_wn), PF_mpool);
    break;
  case OPR_ILOAD:
    if (WN_operator(WN_kid0(ref_wn)) == OPR_ARRAY) {
      WN* array_base = WN_array_base(WN_kid0(ref_wn));
      OPERATOR op = WN_operator(array_base);
      // Base might be a random expression, check for that
#ifdef TARG_ST
      // FdF 20041007: Recognize more complex expressions
      if ((WN_operator(array_base) == OPR_ADD || WN_operator(array_base) == OPR_SUB)) {
	WN *offset_wn = WN_kid1(array_base);
	array_base = WN_kid0(array_base);
	if (WN_operator(offset_wn) != OPR_INTCONST) {
	  WN *swap = array_base;
	  array_base = offset_wn;
	  offset_wn = swap;
	}
	// just let it go to bucket 0
	if (WN_operator(offset_wn) != OPR_INTCONST)
	  break;
	op = WN_operator(array_base);
      }
#endif
      if ((op == OPR_LDID) || (op == OPR_LDA)) {
        sym = CXX_NEW (SYMBOL(array_base), PF_mpool);
      }
    }
    // otherwise just let it go to bucket 0
    break;
  case OPR_ISTORE:
    if (WN_operator(WN_kid1(ref_wn)) == OPR_ARRAY) {
      WN* array_base = WN_array_base(WN_kid1(ref_wn));
      OPERATOR op = WN_operator(array_base);
      // Base might be a random expression, check for that
      if ((op == OPR_LDID) || (op == OPR_LDA)) {
        sym = CXX_NEW (SYMBOL(array_base), PF_mpool);
      }
    }
    // otherwise just let it go to bucket 0
    break;
  default:
    Is_True (FALSE, ("Invalid memory op"));
    return;
  }

  if (sym) {
    VB_PRINT(printf ("    Insert_Ref into bucket for sym ");
             sym->Print(stdout); printf ("\n"));
    // since sym is non-NULL, search from bucket 1 onwards
    INT i;
    for (i=1; i<_sarp_list.Elements(); i++) {
      if (_sarp_list[i]->Same_Base_Symbol(sym)) {
        _sarp_list[i]->Insert_Ref (ref_wn);
        break;
      }
    }
    if (i == _sarp_list.Elements()) {
      /* didn't find it, so create a new entry */
      SINGLE_ARRAY_REF_PREF* sarp =
        CXX_NEW (SINGLE_ARRAY_REF_PREF(sym),PF_mpool);
      sarp->Insert_Ref (ref_wn);
      _sarp_list[_sarp_list.Newidx()] = sarp;
    }
    CXX_DELETE (sym, PF_mpool);
  }
  else {
    VB_PRINT(printf ("    Insert_Ref into bucket 0\n"));
    // a bucket 0 candidate. Bucket 0 always exists
    _sarp_list[0]->Insert_Ref (ref_wn);
  }
}

/***********************************************************************
 *
 * Given a whirl node that is initially the body of the loop,
 * collect all prefetches and loads/stores in the body and store them.
 *
 ***********************************************************************/
void SINGLE_LOOP::Collect_Refs_Prefs (WN* wn) {
  OPERATOR opr = WN_operator(wn);
  switch (opr) {
  case OPR_DO_LOOP:
  {
    SINGLE_LOOP loop (PF_mpool);
#ifdef TARG_ST
    PF_LOOPNODE *loopnode = NULL;
    // Check if this node is not embedded in a WHILE loop
    if (_loopnode != NULL) {
      int i;
      for (i = 0; i < _loopnode->Num_Children(); i++) {
	loopnode = _loopnode->Get_Child(i);
	if (loopnode->Get_Code() == wn)
	  break;
      }
      Is_True(i != _loopnode->Num_Children(), ("Collect_Refs_Prefs: PF_LOOPNODE not found"));
    }
    loop.setLoopnode(loopnode);
#endif
    loop.Process_Loop_Manual (WN_do_body(wn));
    break;
  }
  case OPR_WHILE_DO:
  case OPR_DO_WHILE:
  {
    SINGLE_LOOP loop (PF_mpool);
#ifdef TARG_ST
    loop.setLoopnode(NULL);
#endif
    loop.Process_Loop_Manual (WN_while_body(wn));
    break;
  }
  case OPR_BLOCK:
  {
    WN* kid = WN_first (wn);
    while (kid) {
      Collect_Refs_Prefs (kid);
      // if this kid was a pragma prefetch_ref, then
      // it should be followed by a prefetch, which should
      // already have been processed
      if ((WN_operator(kid) == OPR_PRAGMA) &&
          (WN_pragma(kid) == WN_PRAGMA_PREFETCH_REF)) {
        WN* pf_wn = WN_next(kid);
        FmtAssert(pf_wn && (WN_opcode(pf_wn) == OPC_PREFETCH),
                 ("FE Error: pragma prefetch_ref not followed by prefetch\n"));
        kid = WN_next(kid);
      }
      kid = WN_next(kid);
    }
    break;
  }
  case OPR_PRAGMA:
  {
    switch (WN_pragma(wn)) {
    case WN_PRAGMA_PREFETCH:
    case WN_PRAGMA_PREFETCH_MANUAL:
    case WN_PRAGMA_PREFETCH_REF_DISABLE:
      Is_True (FALSE,
               ("Error: Prefetch(-Manual/Disable) pragma in middle of PU"));
      break;
    case WN_PRAGMA_PREFETCH_REF:
    {
      WN* pf_wn = WN_next(wn);
      FmtAssert (pf_wn && (WN_opcode(pf_wn) == OPC_PREFETCH),
                 ("FE Error: pragma prefetch_ref not followed by prefetch\n"));
      Insert_Pref (wn, pf_wn);
      break;
    }
    default:
      break;
    }
    break;
  }
  case OPR_PREFETCH:
  {
    // This is a manual prefetch that didn't have a size supplied with
    // it, and so didn't have a preceeding pragma
    // Verify that the previous node, if any, was not a prefetch_ref pragma
    if (WN_prev(wn)) {
      OPERATOR opr = WN_operator (WN_prev(wn));
      Is_True ((opr != OPR_PRAGMA) ||
               (WN_pragma(WN_prev(wn)) != WN_PRAGMA_PREFETCH_REF),
               ("FE error: prefetch not processed by prefetch_ref pragma"));
    }
    VB_PRINT(printf ("Manual prefetch in loop 0x%p: \n", this);
             dump_tree (wn));
    Insert_Pref (NULL, wn);
    break;
  }
  case OPR_LDID:
  case OPR_STID:
  case OPR_ILOAD:
  case OPR_ISTORE:
  {
    Insert_Ref (wn);
    // Walk the children, since loads may be subtrees of other loads
    for (INT kidno=0; kidno<WN_kid_count(wn); kidno++)
      Collect_Refs_Prefs (WN_kid(wn, kidno));
    break;
  }
  default:
  {
    for (INT kidno=0; kidno<WN_kid_count(wn); kidno++)
      Collect_Refs_Prefs (WN_kid(wn, kidno));
    break;
  }
  }
}

/***********************************************************************
 * 
 * Always create a new symbol, leave the incoming symbol untouched.
 *
 ***********************************************************************/
void MANUAL_PREFETCH_SYMBOLS::Enter (SYMBOL* sym, INT64 vol) {
  VB_PRINT (printf ("mpf_syms.Enter'ing symbol: ");
            sym->Print(stdout);
            printf (", vol = %lld\n", vol));
  for (INT i=0; i<_sym_da.Elements(); i++) {
    if (*sym == *(_sym_da[i])) {
      // duplicate, just return
      return;
    }
  }
  // didn't find it, so add it
  SYMBOL* lsym = CXX_NEW (SYMBOL(*sym), PF_mpool);
  _sym_da[_sym_da.Newidx()] = lsym;
  _vol_da[_vol_da.Newidx()] = vol;
}
