/*
 *  Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright 2006.  QLogic Corporation.  All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/

//  Coloring the interference graph
/////////////////////////////////////
//
//  Description:
//
//      Basically Chaiten/Briggs with hooks for preferencing and splitting.
//
/////////////////////////////////////


//  $Revision: 1.17 $
//  $Date: 05/06/13 17:31:01-07:00 $
//  $Author: tkong@hyalite.keyresearch $
//  $Source: be/cg/gra_mon/SCCS/s.gra_color.cxx $

#ifdef USE_PCH
#include "cg_pch.h"
#endif // USE_PCH
#pragma hdrstop

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source: be/cg/gra_mon/SCCS/s.gra_color.cxx $ $Revision: 1.17 $";
#endif

#include <limits.h>
#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "mempool.h"
#include "register.h"
#include "priority_queue.h"
#include "cg_flags.h"
#include "cg_color.h"
#include "gra_bb.h"
#include "gra.h"
#include "gra_region.h"
#include "gra_trace.h"
#include "gra_lrange.h"
#include "gra_split.h"
#include "gra_grant.h"
#include "gra_spill.h"
#include "gra_interfere.h"
#ifdef TARG_X8664
#include "targ_sim.h"
#include "whirl2ops.h"
#endif

#ifdef KEY
BOOL GRA_reclaim_register = FALSE;
BOOL GRA_reclaim_register_set = FALSE;
#endif

INT32 GRA_local_forced_max = DEFAULT_FORCED_LOCAL_MAX;
    // How many locals to force allocate (out of the number requested by LRA)?

#ifdef TARG_X8664
BOOL GRA_grant_special_regs = FALSE;
BOOL GRA_local_forced_max_set = FALSE;
#endif

BOOL GRA_avoid_glue_references_for_locals = TRUE;
  // Try to grant the forced locals from the set of registers not also used in 
  // glue copies in the same block
#ifdef TARG_ST
static REGISTER_SET non_prefrenced_regs[ISA_REGISTER_CLASS_MAX_LIMIT + 1];
#else
static REGISTER_SET non_prefrenced_regs[ISA_REGISTER_CLASS_MAX + 1];
#endif
  // Registers that noone prefers yet.
#ifdef TARG_ST
// Deprecated callee_saves_used[]. Now use CGCOLOR interface.
#else
static REGISTER_SET callee_saves_used[ISA_REGISTER_CLASS_MAX + 1];
  // Callee saves registers that someone has already used.  This makes it free
  // for someone else to use them.
#endif
  // Callee saves registers that someone has already used.  This makes it free
  // for someone else to use them.
#ifdef TARG_ST
static REGISTER_SET regs_used[ISA_REGISTER_CLASS_MAX_LIMIT + 1]; // statistics only

#else
static REGISTER_SET regs_used[ISA_REGISTER_CLASS_MAX + 1]; // statistics only
#endif

static MEM_POOL prq_pool;

static GRA_REGION *GRA_current_region;  // The current region for which we are 
					// allocating registers.

// Use to iterate over the members of a LRNAGE's preference class
// that have already been allocated a register in their
// _Preference_Priority order.
class LRANGE_PREF_ITER: public GRA_PREF_LRANGE_ITER {
public:
  LRANGE_PREF_ITER(void) {}
  ~LRANGE_PREF_ITER(void) {}

  void Init(LRANGE *lrange)	{ GRA_PREF_LRANGE_ITER::Init(lrange->Pref()); }
};

//  Choosing registers ...
//////////////////////////////////////////////////////////////////////////
//
//  ... trying to pay attention to preferences.  It goes like this:
//
//      1.  If another LRANGE in its preference class already has a register
//          and it's allowed, we'll take this register, or
//
//      2.  Try to pick a register not prefered by its neighbors or the
//          preferences of its neighbors, or
//
//      3.  Try to pick a register prefered by noone yet, or
//
//      4.  Try to pick a caller saves register, or
//
//      5.  Try to pick a callee saves register that we've already used, or
//
//      6.  Take what's left.
//
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////
static void
Initialize(void)
/////////////////////////////////////
//
//  Initialize register sets used by preferencing and callee_saves
//  Description
/////////////////////////////////////
{
  ISA_REGISTER_CLASS rc;
  static BOOL prq_initialized = FALSE;

  if ( ! prq_initialized ) {
    prq_initialized = TRUE;
    MEM_POOL_Initialize(&prq_pool,"GRA LRANGE priority queue",FALSE);
  }

  FOR_ALL_ISA_REGISTER_CLASS( rc ) {
    non_prefrenced_regs[rc] = REGISTER_CLASS_allocatable(rc);
#ifndef TARG_ST
    callee_saves_used[rc] = REGISTER_SET_EMPTY_SET;
#endif

    regs_used[rc] = REGISTER_SET_EMPTY_SET;
  }
}

/////////////////////////////////////
static void
Update_Register_Info( LRANGE* lrange, REGISTER reg
#ifndef TARG_ST
                      , BOOL reclaim = FALSE 
#endif
                      )
/////////////////////////////////////
//
//  Update some information assiciated primarily with the register and used
//  for preferencing.
//
/////////////////////////////////////
#ifdef TARG_ST
{
  ISA_REGISTER_CLASS rc = lrange->Rc();
  INT nregs = lrange->NHardRegs();
  INT i;

  lrange->Allocate_Register(reg, nregs);

  if ( lrange->Has_Preference() ) {
    non_prefrenced_regs[rc] =
        REGISTER_SET_Difference_Range(non_prefrenced_regs[rc],reg,reg+nregs-1);
  }

  if (!lrange->Has_Wired_Register() && !lrange->Tn_Is_Save_Reg()) {
    regs_used[rc] = REGISTER_SET_Union(regs_used[rc],
                                       REGISTER_SET_Range(reg,reg+nregs-1));
    // Don't declare save regs as they are subject to disappear after copy elimination
#ifdef TARG_ST
    // [TTh] For local LR, this is not a real allocation, but a reservation for LRA.
    // Effective allocation *might* happen in LRA
    if (lrange->Type() != LRANGE_TYPE_LOCAL) {
      CGCOLOR_Allocate_N_Registers(rc, reg, nregs);
    }
#else
    CGCOLOR_Allocate_N_Registers(rc, reg, nregs);
#endif
  }
}
#else /* !TARG_ST */
{
  ISA_REGISTER_CLASS rc = lrange->Rc();

  lrange->Allocate_Register(reg, reclaim);

  if ( lrange->Has_Preference() ) {
    non_prefrenced_regs[rc] =
        REGISTER_SET_Difference1(non_prefrenced_regs[rc],reg);
  }

  if (!lrange->Has_Wired_Register() && !lrange->Tn_Is_Save_Reg()) {
    regs_used[rc] = REGISTER_SET_Union1(regs_used[rc], reg);
    if ((REGISTER_SET_MemberP(REGISTER_CLASS_callee_saves(rc),reg)
#ifdef HAS_STACKED_REGISTERS
       || REGISTER_Is_Stacked_Local(rc, reg)
#endif
	)) 
      callee_saves_used[rc] = REGISTER_SET_Union1(callee_saves_used[rc],reg);
  }
}
#endif /* !TARG_ST */

#ifdef TARG_ST
BOOL
Can_Allocate_From (INT nregs,
		   REGISTER_SET subclass_allowed,
		   REGISTER_SET allowed)
/////////////////////////////////////
//
//  Returns whether <nregs> can be allocated given the <subclass_allowed> set for the first
//  register and the <allowed> set for the <nregs> continuous registers.
//
/////////////////////////////////////
{
  return CGCOLOR_Can_Allocate_N_Registers(nregs, subclass_allowed, allowed);
}
#endif
#ifdef TARG_ST
static BOOL
Choose_Best_Register(REGISTER* reg, INT nregs, ISA_REGISTER_CLASS rc,
		     REGISTER_SET subclass_allowed,
                     REGISTER_SET allowed , BOOL remove_global_callees,
                     BOOL prefer_caller_stacked)
/////////////////////////////////////
//
//  Find the best <nregs> registers in <allowed>, a register set of registers in
//  <rc>.  If one is found (it will be if <allowd> is non-empty), return
//  it by reference in <reg> and return TRUE.
//
/////////////////////////////////////
{
  *reg = CGCOLOR_Choose_Best_Register(rc, nregs, subclass_allowed, allowed, REGISTER_UNDEFINED);
  if (*reg == REGISTER_UNDEFINED) return FALSE;
  return TRUE;
}
#else /* !TARG_ST */

/////////////////////////////////////
static BOOL
Choose_Best_Register(REGISTER* reg, ISA_REGISTER_CLASS rc,
                     REGISTER_SET allowed , BOOL remove_global_callees,
                     BOOL prefer_caller_stacked)
/////////////////////////////////////
//
//  Find the best register in <allowed>, a register set of registers in
//  <rc>.  If one is found (it will be if <allowd> is non-empty), return
//  it by reference in <reg> and return TRUE.
//
/////////////////////////////////////
{
  if ( REGISTER_SET_EmptyP(allowed) )
    return FALSE;

#ifdef HAS_STACKED_REGISTERS
  if (! REGISTER_Has_Stacked_Registers(rc)) {
#endif
    *reg = REGISTER_SET_Choose_Intersection(allowed,
					    REGISTER_CLASS_caller_saves(rc));
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;
      
    *reg = REGISTER_SET_Choose_Intersection(allowed,callee_saves_used[rc]);
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;
      
    *reg = REGISTER_SET_Choose(allowed);
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;
    
    return FALSE;
#ifdef HAS_STACKED_REGISTERS
  }
  else {
    // first, try to use any caller-saved regs that have already been allocated
    REGISTER_SET callers =
      REGISTER_SET_Union(REGISTER_CLASS_caller_saves(rc),
                         REGISTER_Get_Stacked_Avail_Set(ABI_PROPERTY_caller,
                                                        rc));
    *reg = REGISTER_SET_Choose_Intersection(allowed, callers);
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;

    // If we'd prefer to use a stacked caller over anything remaining, then
    // try to get a new one by enlarging the stack; it must not be in allowed
    if (prefer_caller_stacked) {
      *reg = REGISTER_Request_Stacked_Register(ABI_PROPERTY_caller, rc);
      if ( *reg != REGISTER_UNDEFINED )
        return TRUE;
    }
      
    // next, try to use any callee-saved regs that have already been allocated
    *reg = REGISTER_SET_Choose_Intersection(allowed,callee_saves_used[rc]);
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;

    if (remove_global_callees) { // not using non-stacked callee-saved regs
      allowed = REGISTER_SET_Difference(allowed,REGISTER_CLASS_callee_saves(rc));
    }
    // choose from previously un-used callee-saved regs
    *reg = REGISTER_SET_Choose(allowed);
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;

    // try to get a new callee-saved one by enlarging the stack
    *reg = REGISTER_Request_Stacked_Register(ABI_PROPERTY_callee, rc);
    if ( *reg != REGISTER_UNDEFINED )
      return TRUE;
  
    return FALSE;
  }
#endif
}
#endif /* !TARG_ST */


/////////////////////////////////////
static BOOL
#ifdef TARG_ST
Choose_Preference( LRANGE* lrange, REGISTER_SET subclass_allowed,
		   REGISTER_SET allowed, GRA_REGION* region )
#else

Choose_Preference( LRANGE* lrange, REGISTER_SET allowed, GRA_REGION* region
#ifdef KEY
		   , BOOL reclaim = FALSE
#endif
		 )
#endif
/////////////////////////////////////
//
//  If one of the registers used by a member of <lrange>'s preference class is
//  in <allowed>, assign it and return TRUE.
//
/////////////////////////////////////
{
  LRANGE_PREF_ITER iter;

  for (iter.Init(lrange); ! iter.Done(); iter.Step()) {
    LRANGE* p_lrange = iter.Current();
#ifdef TARG_ST
    if ( REGISTER_SET_MemberP(subclass_allowed,p_lrange->Reg())
	 && REGISTER_SET_MembersP (allowed, p_lrange->Reg(), lrange->NHardRegs())) {
#else

    if ( REGISTER_SET_MemberP(allowed,p_lrange->Reg()) ) {
#ifdef KEY
      if (reclaim) {
	LRANGE_Split_Reclaimed_BBs(lrange, p_lrange->Reg());
      }
#endif
#endif
#ifdef TARG_ST
       Update_Register_Info(lrange, p_lrange->Reg());
#else
      Update_Register_Info(lrange, p_lrange->Reg(), reclaim);
#endif
      GRA_Trace_Preference_Attempt(lrange, p_lrange, region, TRUE);
      return TRUE;
    }
    GRA_Trace_Preference_Attempt(lrange, p_lrange, region, FALSE);
  }

  return FALSE;
}
#ifdef TARG_ST
static BOOL
Choose_Subclass_Preference (LRANGE *lrange, REGISTER_SET subclass_allowed,
                            REGISTER_SET allowed, GRA_REGION *region )
/////////////////////////////////////
//
//  Try to find a register for <lrange> in the <allowed> set that is also
//  in the preferred (subclass,offset) set specified in the lrange.
//
/////////////////////////////////////
{
  ISA_REGISTER_SUBCLASS sc = lrange->Pref_Subclass ();
  INT sc_offset = lrange->Pref_Subclass_Offset ();
  ISA_REGISTER_CLASS rc = lrange->Rc();
  REGISTER reg;

  if (sc != ISA_REGISTER_SUBCLASS_UNDEFINED) {
    REGISTER_SET sc_regs = (REGISTER_SET_Offset
			    (REGISTER_SUBCLASS_members (sc), sc_offset));
    if (Choose_Best_Register(&reg, lrange->NHardRegs(), rc,
			     REGISTER_SET_Intersection (subclass_allowed, sc_regs),
			     allowed,
			     REGISTER_Has_Stacked_Registers(rc),
			     !(lrange->Spans_A_Call() || lrange->Spans_Infreq_Call()))) {
      Update_Register_Info(lrange,reg);
      GRA_Trace_Preference_Subclass_Attempt (lrange, sc, sc_offset, region, TRUE);
      return TRUE;
    }
    GRA_Trace_Preference_Subclass_Attempt (lrange, sc, sc_offset, region, FALSE);
  }
  return FALSE;
}
#endif

/////////////////////////////////////
static BOOL
#ifdef TARG_ST
Choose_Avoiding_Neighbor_Preferences( LRANGE* lrange, REGISTER_SET subclass_allowed,
				      REGISTER_SET allowed)
#else
Choose_Avoiding_Neighbor_Preferences( LRANGE* lrange, REGISTER_SET allowed
#ifdef KEY
				      , BOOL reclaim = FALSE
#endif
				    )
#endif
/////////////////////////////////////
//
//  Try to find a register for <lrange> in the <allowed> set that is preferred
//  by as few of <lrange>'s neighbors as possible.
//
//  NOTE that this is a little different from the modulo rename preferencing
//  used in SWP.  There may be some fun left here...
//
/////////////////////////////////////
{
  ISA_REGISTER_CLASS rc = lrange->Rc();
  REGISTER reg;
  LRANGE_PREF_ITER p_iter;
  LRANGE_NEIGHBOR_ITER n_iter;

  for (n_iter.Init(lrange, GRA_current_region); ! n_iter.Done(); n_iter.Step()){
    LRANGE* nlr = n_iter.Current();

    if ( nlr->Allocated() )    	    // We avoid it's actual choice, so
      continue;                     //   no need to avoid its preference.

    // Everthing is a neighbor of these, so what's the point?  We'll tend to
    // avoid them in the Choose_Best_Register code...
    if (nlr->Type() == LRANGE_TYPE_COMPLEMENT && TN_is_save_reg(nlr->Tn())
    ) {
      continue;
    }

    for (p_iter.Init(nlr); ! p_iter.Done(); p_iter.Step()) {
      LRANGE* plr = p_iter.Current();
#ifdef TARG_ST
      REGISTER_SET new_subclass_allowed;
#else
      REGISTER_SET new_allowed;
#endif

      if ( plr->Allocated() && plr->Rc() == lrange->Rc() )
#ifdef TARG_ST
        new_subclass_allowed =
	  REGISTER_SET_Difference_Range(subclass_allowed,
					plr->Reg(),
					plr->Reg() + plr->NHardRegs() - 1);
      if ( !REGISTER_SET_EmptyP(new_subclass_allowed) ) {
	subclass_allowed = new_subclass_allowed;
        allowed = REGISTER_SET_Difference_Range (allowed,
						 plr->Reg(),
						 plr->Reg() + plr->NHardRegs() - 1);
      }
#else
        new_allowed = REGISTER_SET_Difference1(allowed, plr->Reg());
      if ( !REGISTER_SET_EmptyP(new_allowed) ) {
        allowed = new_allowed;
      }
#endif

    }
  }
#ifdef TARG_ST
  if (Choose_Best_Register(&reg, lrange->NHardRegs(), rc, subclass_allowed, allowed,
#else
  if (Choose_Best_Register(&reg, rc, allowed,
#endif
			   REGISTER_Has_Stacked_Registers(rc),
			   !(lrange->Spans_A_Call() || lrange->Spans_Infreq_Call()))) {
#if defined( KEY) &&!defined(TARG_ST)
    if (reclaim) {
      LRANGE_Split_Reclaimed_BBs(lrange, reg);
    }
#endif
#ifdef TARG_ST
    Update_Register_Info(lrange, reg);
#else
    Update_Register_Info(lrange, reg, reclaim);
#endif
    return TRUE;
  }

  return FALSE;
}

/////////////////////////////////////
static BOOL
Allocate_Stacked_Register(LRANGE* lrange)
/////////////////////////////////////
//
//  Try to get a new stacked register for the lrange if profitable,
//  and it exists.
//
/////////////////////////////////////
{
#ifdef HAS_STACKED_REGISTERS
  if (!GRA_use_stacked_regs ||
      !REGISTER_Has_Stacked_Registers(lrange->Rc())) {
    return FALSE;
  }
  if (lrange->Spans_A_Setjmp()) 
    return FALSE;

  INT abi_property;
  if (lrange->Spans_A_Call()) {
    abi_property = ABI_PROPERTY_callee;
  } else {
    abi_property = ABI_PROPERTY_caller;
  }
  REGISTER reg = REGISTER_Request_Stacked_Register(abi_property,
						   lrange->Rc());
  if (reg != REGISTER_UNDEFINED) {
    Update_Register_Info(lrange, reg);
    return TRUE;
  }
#endif
  return FALSE;
}

/////////////////////////////////////
static BOOL
#ifdef TARG_ST
Choose_Noones_Preference( LRANGE* lrange, REGISTER_SET subclass_allowed,
			  REGISTER_SET allowed )
#else
Choose_Noones_Preference( LRANGE* lrange, REGISTER_SET allowed
#ifdef KEY
			  , BOOL reclaim = FALSE
#endif
			)
#endif
/////////////////////////////////////
//
//  See if there is a register in 'allowed' that is not yet the preference of
//  any live range.  If there is one, assign it and return TRUE.
//
/////////////////////////////////////
{
  ISA_REGISTER_CLASS rc = lrange->Rc();
  REGISTER     reg;
  REGISTER_SET npr = non_prefrenced_regs[lrange->Rc()];
#ifdef TARG_ST
  subclass_allowed = REGISTER_SET_Intersection (subclass_allowed,npr);
  allowed = REGISTER_SET_Intersection (allowed,npr);
  if ( Choose_Best_Register(&reg,lrange->NHardRegs(),rc,subclass_allowed,allowed,
#else
  allowed = REGISTER_SET_Intersection(allowed,npr);
  if ( Choose_Best_Register(&reg,rc,allowed,
#endif
			    REGISTER_Has_Stacked_Registers(rc),
			    (lrange->Spans_A_Call() || lrange->Spans_Infreq_Call())) ) {
#if defined( KEY) &&!defined(TARG_ST)
    if (reclaim) {
      LRANGE_Split_Reclaimed_BBs(lrange, reg);
    }
#endif
#ifdef TARG_ST
    Update_Register_Info(lrange, reg);
#else
    Update_Register_Info(lrange, reg, reclaim);
#endif
    return TRUE;
  }
  return FALSE;
}

/////////////////////////////////////
static BOOL
#ifdef TARG_ST
Choose_Anything( LRANGE* lrange, REGISTER_SET subclass_allowed, REGISTER_SET allowed )
#else
Choose_Anything( LRANGE* lrange, REGISTER_SET allowed
#ifdef KEY
		 , BOOL reclaim = FALSE
	       )
#endif
#endif
/////////////////////////////////////
//
//  Take any register from the 'allowed' set for 'lrange'.  Return TRUE if
//  successful.
//
/////////////////////////////////////
{
  REGISTER reg;
  ISA_REGISTER_CLASS rc = lrange->Rc();
  BOOL has_stacked = REGISTER_Has_Stacked_Registers(rc);
#ifdef TARG_ST
  if ( Choose_Best_Register(&reg,lrange->NHardRegs(),
			    rc,subclass_allowed, allowed, has_stacked,
#else

  if ( Choose_Best_Register(&reg,rc,allowed, has_stacked,
#endif
			    !(lrange->Spans_A_Call() || lrange->Spans_Infreq_Call())) ) {
#if defined( KEY) && !defined(TARG_ST)
    if (reclaim) {
      LRANGE_Split_Reclaimed_BBs(lrange, reg);
    }
#endif
#ifdef TARG_ST
     Update_Register_Info(lrange,reg);
#else
    Update_Register_Info(lrange, reg, reclaim);
#endif
    return TRUE;
  }

  //
  // If stacked registers are available, try to get a new one.
  //
#if defined( KEY) && !defined(TARG_ST)
  if (!reclaim)
#endif
  if (has_stacked && Allocate_Stacked_Register(lrange)) {
    return TRUE;
  }

  //
  // Now allow "global" callee saved registers to be freely used (they
  // could have been used to satisfy a preference).
  //
#ifdef TARG_ST
  if (Choose_Best_Register(&reg,lrange->NHardRegs(),
			   rc,subclass_allowed, allowed, FALSE, FALSE)) {
#else
  if (Choose_Best_Register(&reg,rc,allowed, FALSE, FALSE)) {
#endif
#if defined( KEY) && !defined(TARG_ST)
    if (reclaim) {
      LRANGE_Split_Reclaimed_BBs(lrange, reg);
    }
#endif
#ifdef TARG_ST
     Update_Register_Info(lrange,reg);
#else
    Update_Register_Info(lrange, reg, reclaim);
#endif
    return TRUE;
  }

  return FALSE;
}

/////////////////////////////////////
static BOOL
Choose_Register( LRANGE* lrange, GRA_REGION* region )
/////////////////////////////////////
//
//  Choose a register for the given 'lrange'.  Return TRUE if successful.
//
/////////////////////////////////////
{
  REGISTER_SET allowed = lrange->Allowed_Registers(GRA_current_region);
  #ifdef TARG_ST
  REGISTER_SET subclass_allowed =
    lrange->SubClass_Allowed_Registers(GRA_current_region);
  GRA_Trace_Allowed_Registers (lrange, subclass_allowed, allowed);
#endif

  if ( lrange->Has_Wired_Register() ) {
#ifdef TARG_ST
    // [SC] This assertion fails when we disallow r63 to LRA.
    // So assume that wired lranges are always allowed their
    // wired register.
    //    DevAssert( REGISTER_SET_MembersP(allowed, lrange->Reg(),
    //                             lrange->NHardRegs()),
    //         ("LRANGE not allowed its wired register"));
#else

#ifdef KEY
    if (! PU_Has_Nonlocal_Goto_Target)
#endif
    DevAssert( REGISTER_SET_MemberP(allowed, lrange->Reg()),
               ("LRANGE not allowed its wired register"));
#endif
    Update_Register_Info(lrange, lrange->Reg());
    return TRUE;
  }
#if defined( KEY) && !defined(TARG_ST)
  GRA_Trace_LRANGE_Choose(lrange, allowed);
#endif
#ifdef HAS_STACKED_REGISTERS
#ifdef TARG_ST
  if (REGISTER_SET_EmptyP(subclass_allowed) ) {
#else
  if (REGISTER_SET_EmptyP(allowed) ) {
#endif
    //
    // Try to get a stacked register.
    //
    return Allocate_Stacked_Register(lrange);
  }      
#endif

#if defined( KEY) && !defined(TARG_ST)
  // SWP may have assigned a register to the TN.  GRA should trust SWP and
  // choose this register no matter what.  (Maybe better to use GRA's
  // Has_Wired_Register framework, which currently covers LOCAL lranges only.)
  if (Enable_SWP &&
      lrange->Type() == LRANGE_TYPE_COMPLEMENT) {
    REGISTER reg = TN_register(lrange->Tn());
    if (reg != REGISTER_UNDEFINED) {
      Update_Register_Info(lrange, reg);
      return TRUE;
    }
  }
#endif
#ifdef TARG_ST
  if ( Choose_Preference(lrange, subclass_allowed, allowed, region) )
    return TRUE;
  else if (GRA_preference_subclass
	   && Choose_Subclass_Preference(lrange, subclass_allowed, allowed, region) )
    return TRUE;
  else if ( Choose_Avoiding_Neighbor_Preferences(lrange, subclass_allowed, allowed) )
    return TRUE;
  else if ( Choose_Noones_Preference(lrange,subclass_allowed, allowed) )
    return TRUE;
  else
    return Choose_Anything(lrange,subclass_allowed, allowed);
#else
  if ( Choose_Preference(lrange, allowed, region) )
    return TRUE;
  else if ( Choose_Avoiding_Neighbor_Preferences(lrange,allowed) )
    return TRUE;
  else if ( Choose_Noones_Preference(lrange,allowed) )
    return TRUE;
  else
    return Choose_Anything(lrange,allowed);
#endif
}
#ifdef TARG_ST
INT
Forced_Locals (ISA_REGISTER_CLASS rc)
{
  INT rc_local_forced_max;
  INT rc_size;
#define RC_SIZE_THRESHOLD 8

  rc_size = REGISTER_SET_Size(REGISTER_CLASS_allocatable(rc));
  
  // Simplified the computation.
  // Note: GRA_local_forced_max is global while it should be a per register class value.
  // Note: it has been observed that for small register classes, this must be set to 0.
  // Thus we adjust it like this:
  // 1. if GRA_local_forced_max <= RC_SIZE_THRESHOLD then set to 0,
  // 2. otherwise, take min of GRA_local_forced_max and FLOOR(rc_size/RC_SIZE_THRESHOLD).
  // 
  if (REGISTER_CLASS_register_count(rc) <= RC_SIZE_THRESHOLD) {
    rc_local_forced_max = 0;
  } else {
    rc_local_forced_max = Min(GRA_local_forced_max, rc_size/RC_SIZE_THRESHOLD);
  }

  return rc_local_forced_max;
}
#endif

#ifdef TARG_X8664
/////////////////////////////////////
static REGISTER_SET
Find_Single_Register_Subclasses(GRA_BB *gbb)
/////////////////////////////////////
//
// Find operands and results that must use a special register.  Return these
// registers.
//
/////////////////////////////////////
{
  // Hard code rax/rcx/rdx into the search for speed.
  int rax = 0, rcx = 0, rdx = 0;
  REGISTER_SET grants = REGISTER_SET_EMPTY_SET;
  OP *op;

  if (GRA_grant_special_regs) {
    // Force GRA to grant the special registers.
    rax = rcx = rdx = 1;
  } else {
    FOR_ALL_BB_OPs (gbb->Bb(), op) {
      ASM_OP_ANNOT *asm_info = (OP_code(op) == TOP_asm) ? 
		      (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op) : NULL;
      INT i;
      for (i = 0; i < OP_opnds(op); i++) {
        ISA_REGISTER_SUBCLASS subclass = asm_info ?
          ASM_OP_opnd_subclass(asm_info)[i] : OP_opnd_reg_subclass(op, i);
        rax |= (subclass == ISA_REGISTER_SUBCLASS_rax);
        rcx |= (subclass == ISA_REGISTER_SUBCLASS_rcx);
        rdx |= (subclass == ISA_REGISTER_SUBCLASS_rdx);
      }
      for (i = 0; i < OP_results(op); i++) {
        ISA_REGISTER_SUBCLASS subclass = asm_info ?
          ASM_OP_result_subclass(asm_info)[i] : OP_result_reg_subclass(op, i);
        rax |= (subclass == ISA_REGISTER_SUBCLASS_rax);
        rcx |= (subclass == ISA_REGISTER_SUBCLASS_rcx);
        rdx |= (subclass == ISA_REGISTER_SUBCLASS_rdx);
      }
      if (rax & rcx & rdx)
        break;
    }
  }

  // Stuff the registers into the register set.
  if (rax)
    grants = REGISTER_SET_Union1(grants, RAX);
  if (rcx)
    grants = REGISTER_SET_Union1(grants, RCX);
  if (rdx)
    grants = REGISTER_SET_Union1(grants, RDX);

  // If the last OP in BB is a ijump or icall, make sure LRA have enough
  // register(s) for it.  LRA cannot spill around the OP because it cannot
  // insert reloads after it.  Don't give parameter registers, and don't give
  // RAX which is set to 0 before the call.  Bug 7366.
  OP *last_op = BB_last_op(gbb->Bb());
  if (last_op) {
    switch (OP_code(last_op)) {
      case TOP_icall:
	// Grant a non-parameter register.
	grants = REGISTER_SET_Union1(grants, Is_Target_32bit() ? RSI : R10);
	break;
      case TOP_ijmpx:
      case TOP_ijmpxxx:
      case TOP_icallx:
      case TOP_icallxxx:
	// One register for either base or index.
	grants = REGISTER_SET_Union1(grants, Is_Target_32bit() ? RCX : R10);
	break;
      case TOP_ijmpxx:
      case TOP_icallxx:
	// One register for base and one for index.
	grants = REGISTER_SET_Union1(grants, Is_Target_32bit() ? RCX : R10);
	grants = REGISTER_SET_Union1(grants, Is_Target_32bit() ? RDX : R11);
	break;
    }
  }

  return grants;
}
#endif

/////////////////////////////////////
static void
Force_Color_Some_Locals( GRA_REGION* region, ISA_REGISTER_CLASS rc )
/////////////////////////////////////
//
//  The first GRA_local_forced_max locals in each BB are not represented by
//  LRANGEs for compile speed reasons.  We'll just choose colors for these
//  right before we start coloring the complement live ranges.
//
/////////////////////////////////////
{
  GRA_REGION_GBB_ITER iter;
  INT rc_local_forced_max;
#ifdef TARG_ST
  rc_local_forced_max = Forced_Locals(rc);
  if (rc_local_forced_max == 0) return;
#else
  //
  // If it's been overridden, use that value.
  //
  // TODO:  This really needs to be rethought ... IA-64 makes this not
  // so simple of a choice, particularly in the general purpose registers
  // with the register stack.
  //
 #ifndef TARG_ST
  if (GRA_LOCAL_FORCED_MAX(rc) == DEFAULT_FORCED_LOCAL_MAX) {
    INT rc_size = (REGISTER_CLASS_last_register(rc) - REGISTER_MIN) + 1;
#ifdef KEY
    rc_local_forced_max = GRA_LOCAL_FORCED_MAX(rc);
#else
    rc_local_forced_max = Min(GRA_LOCAL_FORCED_MAX(rc), rc_size/8);
#endif
  } else {
#ifndef KEY // cannot use this code because with user-assigned registers via
	// asm("reg-name"), there will be less allocatable registers resulting
	// in 0 register granted to LRA (bug 3663)
    INT rc_size = REGISTER_SET_Size(REGISTER_CLASS_allocatable(rc));
    if (rc_size <= 8) {
     /* There are not enough registers in this class to support this option. */
      return;
    }
    if (rc_size < GRA_LOCAL_FORCED_MAX(rc)*2) {
     /* Don't allow a request for more than half of the available registers. */
      rc_local_forced_max = rc_size/2;
    } else {
#endif
      rc_local_forced_max = GRA_LOCAL_FORCED_MAX(rc);
#ifndef KEY
    }
#endif
  }
#endif 
#endif
  for (iter.Init(region); ! iter.Done(); iter.Step()) {
    INT i;
    GRA_BB* gbb = iter.Current();
    INT regs_to_grant = Min(gbb->Register_Girth(rc),rc_local_forced_max);
#ifdef TARG_X8664
    // Give the special-purpose registers referenced in the BB to LRA.  On the
    // x86, only integer registers have special purpose.
    if (rc == ISA_REGISTER_CLASS_integer) {
      REGISTER_SET special_regs = Find_Single_Register_Subclasses(gbb);
      for (REGISTER reg = REGISTER_SET_Choose(special_regs);
	   reg != REGISTER_UNDEFINED;
	   reg = REGISTER_SET_Choose_Next(special_regs, reg)) {
	gbb->Make_Register_Used(ISA_REGISTER_CLASS_integer, reg);
	if (GRA_reclaim_register)
	  gbb->Make_Register_Referenced(ISA_REGISTER_CLASS_integer, reg);
	GRA_GRANT_Local_Register(gbb, ISA_REGISTER_CLASS_integer, reg);
	regs_to_grant--;
      }
    }
#endif
    for ( i = regs_to_grant;
          i > 0;
          --i
    ) {
      REGISTER reg;
      REGISTER_SET allowed = REGISTER_CLASS_allocatable(rc);
#ifdef HAS_STACKED_REGISTERS
      if (REGISTER_Has_Stacked_Registers(rc)) {
	allowed = REGISTER_SET_Difference(allowed, REGISTER_CLASS_stacked(rc));
	allowed = REGISTER_SET_Union(allowed, regs_used[rc]);
      }
#endif
      allowed = REGISTER_SET_Difference(allowed,
                                gbb->Registers_Used(rc));
#ifdef KEY  // this is needed to avoid insufficient regs granted to LRA
      allowed = REGISTER_SET_Difference(allowed, 
	      			REGISTER_CLASS_function_value(rc));
#endif
      BOOL non_glue_found = FALSE;

      REGISTER_SET npr = non_prefrenced_regs[rc];
      REGISTER_SET gru = gbb->Glue_Registers_Used(rc);

      if ( GRA_avoid_glue_references_for_locals ) {
	REGISTER_SET npr_gru = REGISTER_SET_Difference(npr, gru);
#ifdef TARG_ST
	if ( Choose_Best_Register(&reg,1,rc,
				  REGISTER_SET_Intersection(allowed, npr_gru),
				  REGISTER_SET_Intersection(allowed, npr_gru),
				  FALSE, FALSE)
	    || Choose_Best_Register(&reg,1,rc,
				    REGISTER_SET_Difference(allowed, gru),
				    REGISTER_SET_Difference(allowed, gru),
				    FALSE, FALSE)) {
#else

	if ( Choose_Best_Register(&reg,rc,
				  REGISTER_SET_Intersection(allowed, npr_gru),
				  FALSE, FALSE)
	    || Choose_Best_Register(&reg,rc,
				    REGISTER_SET_Difference(allowed, gru),
				    FALSE, FALSE)) {
#endif
	  non_glue_found = TRUE;
	}
      }
      if ( non_glue_found
#ifdef TARG_ST
	  || Choose_Best_Register(&reg,1,rc,
				  REGISTER_SET_Intersection(allowed, npr),
				  REGISTER_SET_Intersection(allowed, npr),
				  FALSE, FALSE)
          || Choose_Best_Register(&reg,1,rc,allowed,allowed,FALSE,FALSE)
#else
	  || Choose_Best_Register(&reg,rc,
				  REGISTER_SET_Intersection(allowed, npr),
				  FALSE, FALSE)
          || Choose_Best_Register(&reg,rc,allowed,FALSE,FALSE)
#endif
      ) {
        gbb->Make_Register_Used(rc,reg);
#if defined( KEY) && !defined(TARG_ST)
	if (GRA_reclaim_register)
	  gbb->Make_Register_Referenced(rc, reg);
#endif
        GRA_GRANT_Local_Register(gbb,rc,reg);
      }
      else {
#ifdef TARG_X8664
	if( Is_Target_64bit() ){
	  DevWarn("Couldn't force allocate %d registers in rc %d for BB:%d",
		  i,rc,BB_id(gbb->Bb()));
	}
#endif
        break;
      }
    }
  }
}

#if defined (KEY) && !defined(TARG_ST)
//  Reclaim registers ...
//////////////////////////////////////////////////////////////////////////
//
//  Choose a register from the reclaimable register set.  A register is
//  reclaimable for a LRANGE X if, throughout X's BBs, the register is
//  allocated to another LRANGE Y but is not referenced by Y (used or defined).
//  Choose_Reclaimable_Register works just like Choose_Register, except that
//  Choose_Reclaimable_Register searches for register candidates over the
//  reclaimable registers instead of the unused registers.
//
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////
static BOOL
Choose_Reclaimable_Register (LRANGE *lrange, GRA_REGION *region)
/////////////////////////////////////
//
// Reclaim a register for the given 'lrange'.  Return TRUE if successful.
//
/////////////////////////////////////
{
  // Don't reclaim a register for local lranges.
  if (lrange->Type() == LRANGE_TYPE_LOCAL)
    return FALSE;

  REGISTER_SET reclaimable = lrange->Reclaimable_Registers(GRA_current_region);

  // Choose_Register should have taken care of LRANGEs with wired registers.
  Is_True(!lrange->Has_Wired_Register(),
	  ("Choose_Reclaimable_Register: LRANGE has wired register"));

  GRA_Trace_LRANGE_Choose_Reclaimable(lrange, reclaimable);

  if (REGISTER_SET_EmptyP(reclaimable)) {
    return FALSE;
  }      

  if (Choose_Preference(lrange, reclaimable, region, TRUE))
    return TRUE;
  else if (Choose_Avoiding_Neighbor_Preferences(lrange, reclaimable, TRUE))
    return TRUE;
  else if (Choose_Noones_Preference(lrange, reclaimable, TRUE))
    return TRUE;
  else
    return Choose_Anything(lrange, reclaimable, TRUE);
}
#endif

//  Simplification
//////////////////////////////////////////////////////////////////////////


TYPE_PRQ(LRANGE,LRPRQ)  // Generate a priority queue type for live ranges.

#ifdef TARG_ST // [CL] Fix floating point difference between SunOS and Linux/Cygwin
extern BOOL Compare_Float_Nearly_Equal(float p1, float p2);
#endif

/////////////////////////////////////
static BOOL
Compare_Priorities( LRANGE* lrange0, LRANGE* lrange1 )
/////////////////////////////////////
//
//  LRNAGE priority comparison function for use with priority queues.
//
/////////////////////////////////////
{
#ifdef TARG_ST
  if (Compare_Float_Nearly_Equal(lrange0->Priority(), lrange1->Priority())) {
    // [SC] When priorities are equal, the priority queue implementation
    // treats element ordering arbitrarily.  To keep some
    // predictability in the coloring of global lranges, order them by
    // TN number.
    // Note that ordering by TN number has the side-effect of spilling
    // the callee-save registers in the same order every time, which may
    // be a useful behaviour.
    if (lrange0->Type() != LRANGE_TYPE_LOCAL
	&& lrange1->Type() != LRANGE_TYPE_LOCAL) {
      return TN_number(lrange0->Tn()) < TN_number(lrange1->Tn());
    }
    else {
      // [TTh] At least one if the LRANGE is local, but we still want to
      // have a deterministic ordering.
      // So (arbitrarily) prioritize:
      // - a global or complement LR over a local one,
      // - if both LR are locals, use container BB ids.
      // (if both local LRs belong to same BB, the order should not impact
      //  the allocation algorithm)
      return (lrange1->Type() != LRANGE_TYPE_LOCAL ||
	      (lrange0->Type() == LRANGE_TYPE_LOCAL &&
	       (BB_id(lrange0->Gbb()->Bb()) < BB_id(lrange1->Gbb()->Bb()))));
    }
  }
#endif

  return lrange0->Priority() < lrange1->Priority();
}

#ifdef TARG_ST
static BOOL
Compare_Benefit_Runeson_Nystrom(LRANGE* lrange0, LRANGE* lrange1)
{
  float priority0 = lrange0->Priority() / lrange0->Colorability_Benefit();
  float priority1 = lrange1->Priority() / lrange1->Colorability_Benefit();
  if (Compare_Float_Nearly_Equal(priority0, priority1)
      && lrange0->Type() != LRANGE_TYPE_LOCAL
      && lrange1->Type() != LRANGE_TYPE_LOCAL) {
    return TN_number(lrange0->Tn()) < TN_number(lrange1->Tn());
  }
  return priority0 < priority1 && !Compare_Float_Nearly_Equal(priority0, priority1);
}

/////////////////////////////////////
static void
Initialize_Priority_Queue( LRPRQ* q, GRA_REGION* region, ISA_REGISTER_CLASS cl,
			   BOOL (*fn)(LRANGE *, LRANGE *) = Compare_Priorities)
/////////////////////////////////////
//
//  Initialize the priority queue <q> so it can hold all the live ranges of
//  the given <cl> in the given <region>.
//  The compare function is <fn>.
//
/////////////////////////////////////
{
  LRPRQ_Initialize(q,fn,
                     NULL,
                     NULL,
                     &prq_pool,
		     region->Lrange_Count(cl),
                     200);
}

void
LRANGE::Print(FILE *file)
{
  char buff[100];
  fprintf(file, "L%d: %s\n", Id(), Format(buff));
  return;
}
#else

/////////////////////////////////////
static void
Initialize_Priority_Queue( LRPRQ* q, GRA_REGION* region, ISA_REGISTER_CLASS cl )
/////////////////////////////////////
//
//  Initialize the priority queue <q> so it can hold all the live ranges of
//  the given <cl> in the given <region>.
//
/////////////////////////////////////
{
  LRPRQ_Initialize(q,Compare_Priorities,
                     NULL,
                     NULL,
                     &prq_pool,
		     region->Lrange_Count(cl),
                     200);
}
#endif

void
LRANGE_CLIST::Print_Clist(void)
{
  LRANGE *lr;
  for (lr = first; lr != last; lr = lr->clist_next) {
    printf("%d ", lr->id);
  }
  if (lr != NULL) {
    printf("%d \n", lr->id);
  }
}

/////////////////////////////////////
static void
Simplify_Initialize( GRA_REGION* region, ISA_REGISTER_CLASS rc,
                                         LRANGE_CLIST*      cl,
                                         LRANGE_CLIST*      pcl,
                                         LRPRQ*             ready,
                                         LRPRQ*             not_ready )
/////////////////////////////////////
//
//  Get ready for simplification of the given <region> and ISA_REGISTER_CLASS,
//  <rc>.  Initialize the two priority queues and enqueue the live ranges to
//  <ready> that have fewer neighbors than there are registers available (to
//  the particular live range).  <cl> is the coloring list and initialized to
//  be empty.  <pcl> is the coloring list for preallocated live ranges.  We'll
//  put any of these we encounter on <pcl> and it can be prepended to <cl>
//  after simplification.  <not_ready> gets any other live ranges in <region>.
//
/////////////////////////////////////
{
  GRA_REGION_RC_LRANGE_ITER iter;
  GRA_REGION_GBB_ITER gbb_iter;
#ifdef TARG_ST
  LRPRQ not_locally_colorable;
#endif
  cl->Initialize();
  pcl->Initialize();
  lrange_mgr.Clear_One_Set();

  Initialize_Priority_Queue(ready,region,rc);
#ifdef TARG_ST
  Initialize_Priority_Queue(&not_locally_colorable,region,rc);
#else
  Initialize_Priority_Queue(not_ready,region,rc);
#endif

  //
  // put unpreferenced wired locals on the preallocated live range coloring
  // list.  these are never part of the conflict graph for compile time
  // reasons.
  //
  for (gbb_iter.Init(region); ! gbb_iter.Done(); gbb_iter.Step()) {
    GRA_BB* gbb = gbb_iter.Current();
    LRANGE_LIST *l;

    for (l = gbb->Unpreferenced_Wired_LRANGEs(rc);
	 l != NULL;
	 l = LRANGE_LIST_rest(l)
	 ) {
      pcl->Push(LRANGE_LIST_first(l));
    }
  }

  for (iter.Init(region,rc); ! iter.Done(); iter.Step()) {
    LRANGE* lr = iter.Current();

    if ( lr->Has_Wired_Register() ) {

      // This is a LRANGE that needs to be allocated to a particular register.
      // These are always local and no two that interfere require the same
      // register.  We'll hold these out during the simplification phase and
      // put them on front of the coloring list.  We may cause more Briggs
      // points this way, but that's life.  We really have no choice about
      // these and anybody who interferes with one won't be able to use its
      // register.

      if ( lr->Has_Preference() ) {
        // those without preferences added above
        pcl->Push(lr);
      }
    }
    else if ( ! lr->Region_Invariant() ) {
      FmtAssert(! lr->Allocated(),("LRANGE already allocated"));
      lr->Neighbors_Left_Initialize();
#ifdef TARG_ST
      lr->Initialize_Local_Colorability (region);
#endif
      lr->Calculate_Priority();
#if !defined( KEY) || defined(TARG_ST) // for key, leave ready queue empty; rank purely based on priority
#ifdef TARG_ST
      if (lr->Locally_Colorable ()) {
#else
      if ( lr->Neighbors_Left() < lr->Candidate_Reg_Count() ) {
#endif
        LRPRQ_Insert(ready,lr);
        lrange_mgr.One_Set_Union1(lr);
      }
      else
#endif
#ifdef TARG_ST
        LRPRQ_Insert(&not_locally_colorable,lr);
#else
        LRPRQ_Insert(not_ready,lr);
#endif
    }
  }

#ifdef TARG_ST
  // [SC] For all nodes that are not locally colorable,
  // Initialize the colorability benefit, and initialize not_ready to be a
  // priority queue ordered by the Runeson/Nystrom benefit.
  Initialize_Priority_Queue (not_ready, region, rc,
			     (GRA_use_runeson_nystrom_spill_metric
			      ? Compare_Benefit_Runeson_Nystrom
			      : Compare_Priorities));
      
  while (LRPRQ_Size (&not_locally_colorable) > 0) {
    LRANGE *lr = LRPRQ_Delete_Top (&not_locally_colorable);
    lr->Initialize_Colorability_Benefit (region);
    LRPRQ_Insert (not_ready, lr);
    GRA_Trace_Local_Colorability (lr);
  }
#endif

}


/////////////////////////////////////
static void
Simplify( GRA_REGION* region, ISA_REGISTER_CLASS rc, LRANGE_CLIST* cl )
/////////////////////////////////////
//
//  This is essentially Chaitin/Briggs simplification.  The live ranges in the
//  given <region> which want a register from the ISA_REGISTER_CLASS <rc> are
//  ordered onto the given coloring list 'cl'.  We are guaranteed that there
//  is no interference between live ranges belonging to different regions;
//  preference is used to express the relationship.
//
/////////////////////////////////////
{
  LRPRQ ready, not_ready;
  LRANGE* lr;
  LRANGE_NEIGHBOR_ITER iter;
  LRANGE_CLIST pcl;

  MEM_POOL_Push(&prq_pool);
  Simplify_Initialize(region,rc,cl,&pcl,&ready,&not_ready);

#if !defined( KEY) || defined(TARG_ST) // for key, leave ready queue empty; rank purely based on priority
  for (;;) {
    if ( LRPRQ_Size(&ready) != 0 )
      lr = LRPRQ_Delete_Top(&ready);
    else if ( LRPRQ_Size(&not_ready) != 0) {
      // Chose one at a Briggs point.
      lr = LRPRQ_Delete_Top(&not_ready);
      GRA_Trace_Color_LRANGE("Briggs point.",lr);
    }
    else
      break;

    FmtAssert(! lr->Allocated(),("LRANGE already allocated"));

    cl->Push(lr);
    GRA_Trace_Color_LRANGE("Listing.",lr);
    lr->Listed_Set();

    for (iter.Init(lr, region); ! iter.Done(); iter.Step()) {
      LRANGE* nlr = iter.Current();

      FmtAssert(! nlr->Allocated(),("LRANGE already allocated"));
#ifdef TARG_ST

      if (! nlr->Listed()
	  && ! nlr->Has_Wired_Register()
	  && ! lrange_mgr.One_Set_MemberP (nlr)) {
	nlr->Neighbors_Left_Decrement ();
	nlr->Locally_Colorable_Remove_Neighbor (lr);
	if (nlr->Locally_Colorable ()) {
	  GRA_Trace_Local_Colorability (nlr);
	  LRPRQ_Remove (&not_ready, nlr);
	  lrange_mgr.One_Set_Union1 (nlr);
	  LRPRQ_Insert (&ready, nlr);
	} else {
	  LRPRQ_Update (&not_ready, nlr);
	  GRA_Trace_Local_Colorability (nlr);
	}
      }
#else

      // We want to preserve the neighbors left field of the simplified
      // LRANGEs.  They will be used during splitting.  See gra_lrange.h.
      if ( ! ( nlr->Listed() || nlr->Has_Wired_Register() ) &&
           nlr->Neighbors_Left_Decrement() == nlr->Candidate_Reg_Count() - 1) {
        LRPRQ_Remove(&not_ready,nlr);

        DevAssert(! lrange_mgr.One_Set_MemberP(nlr),
                  ("Adding a lrange to coloring ready queue twice"));
        lrange_mgr.One_Set_Union1(nlr);

        LRPRQ_Insert(&ready,nlr);

      }
#endif
    }
  }
#else
  while (LRPRQ_Size(&not_ready) != 0) { 
    lr = LRPRQ_Delete_Top(&not_ready);

    FmtAssert(! lr->Allocated(),("LRANGE already allocated"));

    cl->Push(lr);
    GRA_Trace_Color_LRANGE("Listing.",lr);
    lr->Listed_Set();
  }
#endif

  // Any live ranges that must be allocated to particular live ranges are now
  // prepended to the front of the coloring list.  Notice how these have been
  // forcefully held out of simplification.
  pcl.Append(cl);

  FmtAssert(LRPRQ_Size(&ready) == 0,
            ("Ready priority queue not empty after simplification"));
  FmtAssert(LRPRQ_Size(&not_ready) == 0,
            ("Not-ready priority queue not empty after simplification"));

  MEM_POOL_Pop(&prq_pool);
}


//  Coloring drivers
//////////////////////////////////////////////////////////////////////////

static void
Choose_Original_Registers( GRA_REGION* region, ISA_REGISTER_CLASS rc )
/////////////////////////////////////
//
//  We couldn't color the <rc> TNs in <region> using preferencing.  Just back
//  down to the original register use from the initial coloring.
//
/////////////////////////////////////
{
  GRA_REGION_RC_LRANGE_ITER iter;
  GRA_Trace_Color(0,"Using original registers");

  for (iter.Init(region,rc); ! iter.Done(); iter.Step()) {
    LRANGE* lr = iter.Current();

    DevAssert(lr->Type() == LRANGE_TYPE_REGION,
	      ("Choose_Original_Register for non-REGION LRANGE"));
    lr->Allocated_Reset();
    Update_Register_Info(lr, lr->Orig_Reg());
  }
}

/////////////////////////////////////
static BOOL
Color_Region_With_Preferences( GRA_REGION *region, ISA_REGISTER_CLASS rc )
/////////////////////////////////////
//
//  Try to using preferences to color the live ranges in <region> that need
//  registers in <rc>.  Return TRUE to indicate success.
//
/////////////////////////////////////
{
  LRANGE_CLIST cl;          // Coloring list
  LRANGE_CLIST_ITER iter;   // Iterator over above

  GRA_Trace_Color(0,"Color with preferences...");
  Simplify(region,rc,&cl);

  for (iter.Init(&cl); ! iter.Done(); iter.Step()) {
    if ( ! Choose_Register(iter.Current(), region) ) {
      GRA_Trace_Color(0,"Color with preferences failed.");
      return FALSE;
    }
  }

  GRA_Trace_Color(0,"Color with preferences succeeded.");
  return TRUE;
}

/////////////////////////////////////
static void
GRA_Color_Prev_Allocated_Region( GRA_REGION* region )
/////////////////////////////////////
//  No interface description.
/////////////////////////////////////
{
  ISA_REGISTER_CLASS rc;

  GRA_Trace_Color_REGION(region);
  GRA_current_region = region;

  FOR_ALL_ISA_REGISTER_CLASS( rc ) {
    if ( region->Prev_Alloc_By_GRA() )
      Choose_Original_Registers(region,rc);
    else if ( region->Lrange_Count(rc) == 0 )
      continue;
    else {
      // Skip the following to disable coloring of SWP region
#if 0
      if ( ! Color_Region_With_Preferences(region,rc) ) {
      // If we can't get what we want, just take what we already had.
      DevWarn("Couldn't recolor a previously allocated region. "
              "Keeping original.");
#endif
      Choose_Original_Registers(region,rc);
    }
  }
  region->Set_GRA_Colored();
}

/////////////////////////////////////
static BOOL
Must_Split( LRANGE* lrange )
/////////////////////////////////////
//
//  Should we always split instead of spilling?
//
//  Before attempting to color each live range, we check to make sure it's
//  priority is non-negative.  The priority is supposed to approximate the the
//  cost of spilling the live range (loading/restoring it in every block with
//  a reference) vs allocating it (loading/restoring at the points determined
//  by a previous live range split, if there was one.)  However this is a bit
//  crude, becase the spill/restore placement optimization can sometimes
//  improve rather a lot on the cost of allocating (by moving the spills and
//  restores outwared to less frequently traversed blocks).  This was
//  especially obvious with the callee-saves live ranges.  In effect, this
//  function is used to force shrink wrapping, regardless of the priority
//  calculated.
//
/////////////////////////////////////
{
  return    lrange->Type() == LRANGE_TYPE_COMPLEMENT
    && TN_is_save_reg(lrange->Tn()) && 
    #ifdef TARG_ST
    // (cbr) must keep register savings in the prologue in order for the spill
    // in the catch handlers to be removed. see Adjust_Entry.
    !PU_has_exc_scopes (Get_Current_PU()) &&
#endif
    GRA_shrink_wrap;
}

/////////////////////////////////////
static void
GRA_Color_Complement( GRA_REGION* region )
/////////////////////////////////////
//  No interface description.
/////////////////////////////////////
{
  ISA_REGISTER_CLASS  rc;
  LRANGE_CLIST        cl;     // Coloring list
  LRANGE_CLIST_ITER   iter;   // Iterator over above
  char buff[100];

  priority_count = 0.0;

  GRA_Trace_Color(0,"Coloring complement region...");
  GRA_current_region = gra_region_mgr.Complement_Region();

  GRA_GRANT_Initialize();

  FOR_ALL_ISA_REGISTER_CLASS_IN_REVERSE( rc ) {
    BOOL forced_locals = FALSE;

    if ( region->Lrange_Count(rc) == 0 ) {
      Force_Color_Some_Locals(region,rc);
      continue;
    }
    GRA_Init_Trace_Memory();
    Simplify(region,rc,&cl);
    GRA_Trace_Memory("Simplify()");

    GRA_Init_Trace_Memory();
    for (iter.Init(&cl); ! iter.Done(); iter.Step()) {
      LRANGE* split_alloc_lr;
      LRANGE* lr = iter.Current();

      if ( ! (forced_locals || lr->Has_Wired_Register()
#if defined( KEY) && !defined(TARG_ST)
	      // SWP may have allocated the register.
	      || (lr->Type() == LRANGE_TYPE_COMPLEMENT &&
		  TN_register(lr->Tn()) != REGISTER_UNDEFINED)
#endif
	 ) ) {
        forced_locals = TRUE;
        Force_Color_Some_Locals(region,rc);
      }

      GRA_Trace_Color_LRANGE("Coloring",lr);
      GRA_Trace_Complement_LRANGE_Neighbors(lr, region);
#ifdef TARG_ST
      // [SC] Suppress the infreq-call trick for save-reg tns when
      // they might be allocated to caller-save registers.
      // This is because the infreq-call trick forces an immediate
      // split.  That would be okay, but unfortunately, the split
      // lrange does not have any preferencing information, and we rely
      // on the preferencing.  Without the preferencing, the coloring
      // will prefer a caller-save register for the save-reg tn, which
      // forces a copy to and from the save-reg.
      // A better fix would be to maintain the preferencing information
      // when a split is made.  This would also improve the allocation
      // generally.  see also the comment in gra_split.cxx/Fix_Interference()
      // which also bemoans the missing preferencing.  The description
      // at the head of gra_pref.h claims that the preferences are fixed
      // when a split is made, but I can find no code to implement this.
      if (GRA_spill_to_caller_save
	  && lr->Tn_Is_Save_Reg()
	  && REGISTER_SET_MemberP(REGISTER_CLASS_callee_saves(lr->Rc()),
				  TN_save_reg(lr->Tn()))
	  && lr->Spans_Infreq_Call()) {
	lr->Spans_A_Call_Set();
	lr->Spans_Infreq_Call_Reset();
      }
#endif

#ifdef TARG_X8664
      // If lrange is a x87/MMX lrange spanning a mixture of x87/MMX OPs, then
      // split the lrange into the part that contains the mixed OPs
      // (split_alloc_lr) and the part that does not.  The latter is put back
      // into the coloring list to be colored later like a regular lrange.
      if ((rc == ISA_REGISTER_CLASS_x87 && lr->Spans_mmx_OP()) ||
	  (rc == ISA_REGISTER_CLASS_mmx && lr->Spans_x87_OP())) {
	LRANGE_Split_Mixed_x87_MMX(lr, &iter, &split_alloc_lr);
	// Localize the part with mixed x87/MMX OPs.
	GRA_Note_Spill(split_alloc_lr);
	continue;
      }
#endif

      // 
      // can't spill wired registers under any circumstances.  the only
      // way we're going to get here is if the frequency on the block is
      // hosed.  we'll definitely get bad code if we've got these kind
      // of bogus frequencies, but its better than incorrect code.
      //
#ifdef KEY
      if (lr->Priority() < 0.0F && !lr->Has_Wired_Register() && !Must_Split(lr)
	  || lr->No_Appearance()) {
        GRA_Note_Spill(lr);
      } else if (lr->Spans_Infreq_Call() &&
                 (!lr->Tn_Is_Save_Reg() ||
                  !REGISTER_SET_MemberP(REGISTER_CLASS_callee_saves(lr->Rc()),TN_save_reg(lr->Tn()))) &&
		 LRANGE_Split(lr, &iter, &split_alloc_lr)) {
	BOOL did_choose = Choose_Register(split_alloc_lr, region);
	FmtAssert(did_choose,("Failed to choose a register for a split across infreq call of %s",
			      split_alloc_lr->Format(buff)));
	priority_count += split_alloc_lr->Priority();
      } else if (Choose_Register(lr, region)) {
	priority_count += lr->Priority();
      } else if (lr->Tn_Is_Save_Reg()) {  // bug 3552: never split saved-TNs
	GRA_Note_Spill(lr);
      } else if (LRANGE_Split(lr, &iter, &split_alloc_lr) &&
		 (split_alloc_lr->Priority() >= 0.0F ||
		  Must_Split(split_alloc_lr))) {
	BOOL did_choose = Choose_Register(split_alloc_lr, region);
	FmtAssert(did_choose,("Failed to choose a register for a split of %s",
			      split_alloc_lr->Format(buff)));
	priority_count += split_alloc_lr->Priority();
      }
#if 0
      // Allocate a reclaimable register to the lrange.  Skip this step because
      // finding a reclaimable register over the entire lrange is unlikely to
      // be successful due to high register pressure.  The code is fully
      // implemented but requires more testing.
      else if (GRA_reclaim_register &&
	       Choose_Reclaimable_Register(split_alloc_lr, region)) {
	priority_count += lr->Priority();
      }
#endif
      // Split lrange to fit in a reclaimable register.
      else if (GRA_reclaim_register &&
#ifdef TARG_ST
                LRANGE_Split(lr, &iter, &split_alloc_lr) 
#else
	       LRANGE_Split(lr, &iter, &split_alloc_lr, TRUE) 
#endif
               &&
	       (split_alloc_lr->Priority() >= 0.0F ||
		Must_Split(split_alloc_lr))) {
#ifndef TARG_ST
	BOOL did_choose = Choose_Reclaimable_Register(split_alloc_lr, region);
	FmtAssert(did_choose,
		  ("Failed to choose a reclaimable register for a split of %s",
		   split_alloc_lr->Format(buff)));
#endif
	priority_count += split_alloc_lr->Priority();
      } else {
	GRA_Note_Spill(split_alloc_lr);
      }
#else	// KEY
      if (lr->Priority() < 0.0F && !lr->Has_Wired_Register() && !Must_Split(lr)
	  || lr->No_Appearance()) {
        GRA_Note_Spill(lr);
      } else if (lr->Spans_Infreq_Call() &&
                 (!lr->Tn_Is_Save_Reg() ||
                  !REGISTER_SET_MemberP(REGISTER_CLASS_callee_saves(lr->Rc()),TN_save_reg(lr->Tn()))) &&
		 LRANGE_Split(lr, &iter, &split_alloc_lr)) {
	BOOL did_choose = Choose_Register(split_alloc_lr, region);
	FmtAssert(did_choose,("Failed to choose a register for a split across infreq call of %s",
			      split_alloc_lr->Format(buff)));
	priority_count += split_alloc_lr->Priority();
      } else if (!Choose_Register(lr, region)) {
        if (!LRANGE_Split(lr,&iter,&split_alloc_lr) ||
	    (split_alloc_lr->Priority() < 0.0F &&
	     !Must_Split(split_alloc_lr))) {
          GRA_Note_Spill(split_alloc_lr);
        } else {
          BOOL did_choose = Choose_Register(split_alloc_lr, region);
	  FmtAssert(did_choose,("Failed to choose a register for a split of %s",
			        split_alloc_lr->Format(buff)));
	  priority_count += split_alloc_lr->Priority();
        }
      } else {
	priority_count += lr->Priority();
      }
#endif	// KEY
    }

    GRA_Trace_Memory("Complement coloring loop");
    GRA_Trace_Regs_Stats(rc, REGISTER_CLASS_allocatable(rc), regs_used[rc]);

    if (! forced_locals)        // Haven't done this yet if every lrange wired
      Force_Color_Some_Locals(region,rc);
#ifdef TARG_ST
    if (Is_Predicate_REGISTER_CLASS(rc)) {
      REGISTER_SET callee_used = REGISTER_SET_Intersection(REGISTER_CLASS_callee_saves(rc),
							   regs_used[rc]);
	// If the register class is not multiple save, we should not
	// have generated the save of predicates into a single register.
	if (REGISTER_SET_EmptyP(callee_used) && 
	    REGISTER_CLASS_multiple_save(rc))
	  GRA_Remove_Predicates_Save_Restore();  // because they're always generated
    }
#else

    if (Is_Predicate_REGISTER_CLASS(rc) &&
	REGISTER_SET_EmptyP(callee_saves_used[rc]))
      GRA_Remove_Predicates_Save_Restore();  // because they're always generated
#endif
  }
  gra_region_mgr.Complement_Region()->Set_GRA_Colored();
}

/////////////////////////////////////
void
GRA_Color(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  GRA_REGION_PREV_ALLOC_ITER iter;

  Initialize();

  GRA_Init_Trace_Memory();
  for (iter.Init(); ! iter.Done(); iter.Step()) {
    GRA_REGION *region = iter.Current();
    GRA_Color_Prev_Allocated_Region(region);
  }
  GRA_Trace_Memory("Gra_Color_Prev_Allocate_Region()");

  GRA_Color_Complement(gra_region_mgr.Complement_Region());
}
