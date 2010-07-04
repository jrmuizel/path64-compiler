/*
 *  Copyright (C) 2007, 2008, 2009 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
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


/* =======================================================================
 * =======================================================================
 *
 *  Module: lra.c
 *  $Revision: 1.100 $
 *  $Date: 05/11/28 15:57:58-08:00 $
 *  $Author: tkong@hyalite.keyresearch $
 *  $Source: be/cg/SCCS/s.lra.cxx $
 *
 *  Description:
 *  ============
 *
 *  Local Register Allocation.
 *
 * =======================================================================
 * =======================================================================
 */
/*
 * [TARG_ST] Improvements:
 * - Overlap coalescing
 *   ------------------
 *   Enable coalescing of overlapping live-ranges linked by extracts or moves,
 *   as long as there is no related definition within the overlap.
 *   Overlap coalescing is implemented as a LR tree, where:
 *     - A parent relationship in the tree means that the child is a copy
 *       or an extract of the parent,
 *     - All nodes of the tree can be safely coalesced.
 */

#ifdef USE_PCH
#include "cg_pch.h"
#endif // USE_PCH
#pragma hdrstop

#define __STDC_LIMIT_MACROS
#include <limits.h>
#include <stdint.h>
#if !defined(__FreeBSD__)
#include <alloca.h>
#endif

#include "defs.h"
#include "mempool.h"
#include "config.h"
#include "tracing.h"
#include "timing.h"
#include "cgir.h"
#include "targ_sim.h"
#include "wn.h"
#include "erglob.h"
#include "ercg.h"
#include "data_layout.h"
#include "bb_set.h"
#include "tn_set.h"
#include "gtn_universe.h"
#include "gtn_set.h"
#include "gtn_tn_set.h"
#include "tn_set.h"
#include "cg.h"
#include "cg_flags.h"
#include "cg_internal.h"
#include "cxx_memory.h"
#include "calls.h"
#include "cgexp.h"
#include "register.h"
#include "reg_live.h"
#include "bbregs.h"
#include "gra.h"
#include "whirl2ops.h"
#include "tn_map.h"
#include "cg_spill.h"
#include "cg_vector.h"
#include "lra.h"
#include "glob.h"
#include "cgtarget.h"
#include "targ_proc_properties.h"
#include "hb_sched.h"
#include "ebo.h"
#ifdef TARG_X8664
#include "config_lno.h"  // for LNO_Run_Simd
#endif
#ifdef KEY
static BOOL large_asm_clobber_set[ISA_REGISTER_CLASS_MAX+1];
#endif

#define TN_is_local_reg(r)   (!(TN_is_dedicated(r) | TN_is_global_reg(r)))

/* regs that need to be saved at prolog and restored at epilog: */
#ifdef TARG_ST
static REGISTER_SET Callee_Saved_Regs_Used[ISA_REGISTER_CLASS_MAX_LIMIT+1];
#else
static REGISTER_SET Callee_Saved_Regs_Used[ISA_REGISTER_CLASS_MAX+1];
#endif


/* Data structure to keep a list of available registers in each 
 * register class.
 */
typedef struct {
  BOOL reg[REGISTER_MAX+1];
} AVAIL_REGS;

#ifdef TARG_ST
static AVAIL_REGS avail_regs[ISA_REGISTER_CLASS_MAX_LIMIT+1];
#else
static AVAIL_REGS avail_regs[ISA_REGISTER_CLASS_MAX+1];
#endif

/* Set of available registers in each register class. */
#ifdef TARG_ST
static REGISTER_SET avail_set[ISA_REGISTER_CLASS_MAX_LIMIT+1];
#else
static REGISTER_SET avail_set[ISA_REGISTER_CLASS_MAX+1];
#endif

/* Set of excluded registers in each register class. */
#ifdef TARG_ST
static REGISTER_SET exclude_set[ISA_REGISTER_CLASS_MAX_LIMIT+1];
#else
static REGISTER_SET exclude_set[ISA_REGISTER_CLASS_MAX+1];
#endif

// Keeps track of the last register assigned for each register class.
// This is used to simulate a round-robin allocation of registers.
#ifdef TARG_ST
static REGISTER last_assigned_reg[ISA_REGISTER_CLASS_MAX_LIMIT+1];
#else
static REGISTER last_assigned_reg[ISA_REGISTER_CLASS_MAX+1];
#endif

#ifdef KEY
// Data structure to keep track of the OP number that a register was last
// freed.  Supports the least-recently-used method of register assignment.
typedef struct {
  INT reg[REGISTER_MAX+1];
} LAST_FREED;
static LAST_FREED last_freed[ISA_REGISTER_CLASS_MAX+1];
#endif

/* Array of the OPs in the basic block being processed. */
static VECTOR Insts_Vector;
#define OP_VECTOR_element(v,i)  ((OP *)VECTOR_element(v,i))
#define Set_OP_VECTOR_element(v,i,op)   (VECTOR_element(v,i) = (void *)op)

/* set of registers that are live through the basic block */
#ifdef TARG_ST
static REGISTER_SET livethrough[ISA_REGISTER_CLASS_MAX_LIMIT+1];
#else
static REGISTER_SET livethrough[ISA_REGISTER_CLASS_MAX+1];
#endif
// State variable to indicate if we need to compute the livethrough set.
static BOOL livethrough_computed;

/* If a result of an undeletable OP is not needed, */
/* it may be replaced with one of these TNs.         */
#ifdef TARG_ST
static TN *unused_tn_def[ISA_REGISTER_CLASS_MAX_LIMIT+1];
#else
static TN *unused_tn_def[ISA_REGISTER_CLASS_MAX+1];
#endif


static BOOL do_global_locking = FALSE;

/* Memory pool for allocating things during register allocation of 
 * a single basic block. Memory allocated from this pool is not
 * initialized to zero.
 */
static MEM_POOL lra_pool;

static BOOL Trace_LRA;                  /* -Wb,-tt54:0x01 */
static BOOL Trace_LRA_Detail;           /* -Wb,-tt54:0x02 */
static BOOL Trace_LRA_Spill;            /* -Wb,-tt54:0x04 */
static BOOL Trace_LRA_Entry_Exit;       /* -Wb,-tt54:0x08 */
static BOOL Trace_Move_GRA_Spills;      /* -Wb,-tt54:0x10 */
// 	Trace_Fat_Points		/* -Wb,-tt54:0x20 */

#ifdef TARG_ST
/* [SC] In a 2-D array indexed by class and subclass, it is
 * often convenient to treat the array[cl][ISA_REGISTER_SUBCLASS_UNDEFINED]
 * element as representing the base class.  In this case we need to
 * include the ISA_REGISTER_SUBCLASS_UNDEFINED value in an iteration over
 * subclasses.  The normal iterator, FOR_ALL_ISA_REGISTER_SUBCLASS, does
 * not do this, so here make the desired iterator.
 * It relies omn the fact that ISA_REGISTER_SUBCLASS_UNDEFINED == 0.
 */
#define FOR_ALL_ISA_REGISTER_SUBCLASS_AND_UNDEFINED(sc) \
	for (sc = ISA_REGISTER_SUBCLASS (0); \
	     sc <= ISA_REGISTER_SUBCLASS_MAX; \
	     sc = (ISA_REGISTER_SUBCLASS)(sc + 1))

/* [SC] Data structure to keep track of the live range of each dedicated
 * register.  We record only one live range for each dedicated register:
 * if it has multiple live ranges in a single block then we are
 * pessimistic.  This matches the original open64 method of using a
 * LIVE_RANGE structure to track the live range of dedicated registers,
 * which I have abandoned because it does not work well for multi-register
 * TNs.  (The open64 mapped assigned global TNs to the live range for
 * the dedicated register the TN was assigned to.  For multi-register TNs,
 * this creates a problem: there are multiple dedicated registers, so which
 * one should the TN be mapped to?)
 */
static struct {
  mINT32 first_def;
  mINT32 last_use;
  mINT32 exposed_use;
}
#ifdef TARG_ST
  ded_reg_liveness[ISA_REGISTER_CLASS_MAX_LIMIT+1][REGISTER_MAX+1];
#else
  ded_reg_liveness[ISA_REGISTER_CLASS_MAX+1][REGISTER_MAX+1];
#endif
inline INT32 ded_reg_first_def (ISA_REGISTER_CLASS cl, REGISTER r) {
  return ded_reg_liveness[cl][r].first_def;
}
static void Set_ded_reg_first_def (ISA_REGISTER_CLASS cl, REGISTER r, INT32 v) {
  ded_reg_liveness[cl][r].first_def = v;
}
inline INT32 ded_reg_last_use (ISA_REGISTER_CLASS cl, REGISTER r) {
  return ded_reg_liveness[cl][r].last_use;
}
static void Set_ded_reg_last_use (ISA_REGISTER_CLASS cl, REGISTER r, INT32 v) {
  ded_reg_liveness[cl][r].last_use = v;
}
inline INT32 ded_reg_exposed_use (ISA_REGISTER_CLASS cl, REGISTER r) {
  return ded_reg_liveness[cl][r].exposed_use;
}
static void Set_ded_reg_exposed_use (ISA_REGISTER_CLASS cl, REGISTER r, INT32 v) {
  ded_reg_liveness[cl][r].exposed_use = v;
}
#endif

/* Data structure to keep track of the local live range for each TN. 
 * There is only one live range for a TN even if there are multiple
 * definitions. This is to enforce the assignment of a single register
 * to a TN.
 */
typedef struct live_range {
  TN *tn;               /* the live range tn */
  mINT16 first_def;     /* instruction number for first def in live range. */
  mINT16 last_use;      /* instruction number for last use in live range. */
  mINT16 exposed_use;   /* instruction number for last exposed use (if any) */
  mUINT8 def_cnt;       /* number of defs in the live range. */
  mUINT8 use_cnt;       /* number of uses in the live range. */
  mUINT8 flags;         /* misc. flags (see definition below) */
  mREGISTER prefer_reg; /* if def of live range is a preferencing copy. */
  mINT16 first_spill;    /* first spill of exposed global */
#ifdef TARG_ST
  ISA_REGISTER_SUBCLASS sc; /* Smallest subclass used to reference the live range tn */
  OP *special_def_op;    /* If this live range is defined by an extract or copy 
			    operation, this is the operation */
  mINT32 ovcoal_local_rank;/* Rank of current LR within parent register hierarchy */
  mINT32 ovcoal_init_ver;  /* Value id obtained at first definition point */
  mINT32 ovcoal_last_ver;  /* Value id obtained at last definition point */
  struct live_range *ovcoal_parent;  /* Parent node in overlap coalescing tree */
  struct live_range *ovcoal_child;   /* First child node in overlap coalescing tree */
  struct live_range *ovcoal_sibling; /* next sibling node in overlap coalescing tree */
  struct live_range *next_alias; /* Global live ranges can alias, where each
				    alias is allocated to the same registers.
				    We chain all aliases together in a circular
				    list. */
  TN_LIST *spill_tns;   /* The TNs used when this live range is spilled. */
#endif
  struct live_range *next; /* pointer to next live range */
} LIVE_RANGE;

#define LR_tn(lr)               ((lr)->tn)
#ifdef TARG_ST
inline INT32 LR_first_def (const LIVE_RANGE *lr) {
  return lr->first_def;
}
inline BOOL LR_early_clobber (const LIVE_RANGE *lr); // forward
static void Set_LR_early_clobber (LIVE_RANGE *lr); // forward
static void Set_LR_first_def (LIVE_RANGE *lr, INT32 v,
			      BOOL early_clobber = FALSE) {
  lr->first_def = v;
  if (early_clobber) {
    Set_LR_early_clobber (lr);
    --v;
  }
  TN *tn = LR_tn(lr);
  if (!TN_is_local_reg(tn)
      && TN_register (tn) != REGISTER_UNDEFINED) {
    ISA_REGISTER_CLASS cl = TN_register_class(tn);
    REGISTER reg;
    FOR_ALL_REGISTER_SET_members (TN_registers (tn), reg) {
      if (ded_reg_first_def(cl, reg) > v) {
	Set_ded_reg_first_def(cl, reg, v);
      }
    }
  }
}
inline INT32 LR_effective_first_def (const LIVE_RANGE *lr) {
  return lr->first_def - (LR_early_clobber(lr) ? 1 : 0);
}
inline INT32 LR_last_use (const LIVE_RANGE *lr) {
  return lr->last_use;
}
static void Set_LR_last_use (LIVE_RANGE *lr, INT32 v) {
  lr->last_use = v;
  TN *tn = LR_tn(lr);
  if (!TN_is_local_reg(tn)
      && TN_register (tn) != REGISTER_UNDEFINED) {
    ISA_REGISTER_CLASS cl = TN_register_class(tn);
    REGISTER reg;
    FOR_ALL_REGISTER_SET_members (TN_registers (tn), reg) {
      if (ded_reg_last_use(cl, reg) < v) {
	Set_ded_reg_last_use(cl, reg, v);
      }
    }
  }
}
inline INT32 LR_exposed_use (const LIVE_RANGE *lr) { return lr->exposed_use; }
static void Set_LR_exposed_use (LIVE_RANGE *lr, INT32 v) {
  lr->exposed_use = v;
  TN *tn = LR_tn(lr);
  if (!TN_is_local_reg(tn)
      && TN_register (tn) != REGISTER_UNDEFINED) {
    ISA_REGISTER_CLASS cl = TN_register_class(tn);
    REGISTER reg;
    FOR_ALL_REGISTER_SET_members (TN_registers (tn), reg) {
      if (ded_reg_exposed_use(cl, reg) < v) {
	Set_ded_reg_exposed_use(cl, reg, v);
      }
    }
  }
}
/* Set definer of current lr (either copy or extract) */
static void Set_LR_special_def_op (LIVE_RANGE *lr, OP *op) {
  lr->special_def_op = op;
}
inline OP *LR_special_def_op (const LIVE_RANGE *lr) {
  return (lr->special_def_op);
}
inline OP *LR_extract_op (const LIVE_RANGE *lr) {
  return ((lr->special_def_op && OP_extract(lr->special_def_op))?lr->special_def_op:NULL);
}
inline OP *LR_copy_op (const LIVE_RANGE *lr) {
  return ((lr->special_def_op && OP_copy(lr->special_def_op))?lr->special_def_op:NULL);
}
inline LIVE_RANGE *LR_next_alias (const LIVE_RANGE *lr) { return lr->next_alias; }
static void Set_LR_next_alias (LIVE_RANGE *lr, LIVE_RANGE *alias) {
  lr->next_alias = alias;
}
inline TN_LIST *LR_spill_tns (const LIVE_RANGE *lr) { return lr->spill_tns; }
static void Push_LR_spill_tns (LIVE_RANGE *lr, TN *tn, MEM_POOL *pool) {
  lr->spill_tns =  TN_LIST_Push (tn, lr->spill_tns, pool);
}
inline mINT32 LR_ovcoal_local_rank(LIVE_RANGE *lr) {
  return (lr->ovcoal_local_rank);
}
inline void Set_LR_ovcoal_local_rank(LIVE_RANGE *lr, mINT32 rank) {
  lr->ovcoal_local_rank = rank;
}
inline mINT32 LR_ovcoal_init_ver(LIVE_RANGE *lr) {
  return (lr->ovcoal_init_ver);
}
static void Set_LR_ovcoal_init_ver(LIVE_RANGE *lr, mINT32 v) {
  lr->ovcoal_init_ver = v;
}
inline mINT32 LR_ovcoal_last_ver(LIVE_RANGE *lr) {
  return (lr->ovcoal_last_ver);
}
static void Set_LR_ovcoal_last_ver(LIVE_RANGE *lr, mINT32 v) {
  lr->ovcoal_last_ver = v;
}
inline LIVE_RANGE *LR_ovcoal_parent(LIVE_RANGE *lr) {
  return (lr->ovcoal_parent);
}
static void Set_LR_ovcoal_parent(LIVE_RANGE *lr, LIVE_RANGE *parent_lr) {
  lr->ovcoal_parent = parent_lr;
}
inline LIVE_RANGE *LR_ovcoal_child(LIVE_RANGE *lr) {
  return (lr->ovcoal_child);
}
inline void Set_LR_ovcoal_child(LIVE_RANGE *lr, LIVE_RANGE *child_lr) {
  lr->ovcoal_child = child_lr;
}
inline LIVE_RANGE *LR_ovcoal_sibling(LIVE_RANGE *lr) {
  return (lr->ovcoal_sibling);
}
inline void Set_LR_ovcoal_sibling(LIVE_RANGE *lr, LIVE_RANGE *sibling_lr) {
  lr->ovcoal_sibling = sibling_lr;
}
/* Return the rank of 'lr' within register hierarchy of the root node
 * of its coalescing tree */
static mINT32 LR_ovcoal_rank(LIVE_RANGE *lr) {
  LIVE_RANGE *lr_tmp = lr;
  mINT32 rank = 0;
  while (lr_tmp) {
    rank  += lr_tmp->ovcoal_local_rank;
    lr_tmp = lr_tmp->ovcoal_parent;
  }
  return (rank);
}
/* Add 'sibling_lr' in sibling cyclic list of 'lr' */
static void Add_LR_ovcoal_sibling(LIVE_RANGE *lr, LIVE_RANGE *sibling_lr) {
  LIVE_RANGE *last_lr = sibling_lr;
  while (last_lr->ovcoal_sibling != sibling_lr) {
    last_lr = last_lr->ovcoal_sibling;
  }
  last_lr->ovcoal_sibling = lr->ovcoal_sibling;
  lr->ovcoal_sibling = sibling_lr;
}
/* Attach 'child_lr' to 'lr' coalescing tree */
static void Add_LR_ovcoal_child(LIVE_RANGE *lr, LIVE_RANGE *child_lr, mINT32 local_rank) {
  if (!lr->ovcoal_child) {
    lr->ovcoal_child = child_lr;
  }
  else {
    Add_LR_ovcoal_sibling(lr->ovcoal_child, child_lr);
  }
  Set_LR_ovcoal_parent(child_lr, lr);
  Set_LR_ovcoal_local_rank(child_lr, local_rank);
}
/* Detach 'child_lr' from 'lr' coalescing tree */
static void Remove_LR_ovcoal_child(LIVE_RANGE *lr, LIVE_RANGE *child_lr) {
  if (child_lr->ovcoal_sibling == child_lr) {
    lr->ovcoal_child = NULL;
  }
  else {
    LIVE_RANGE *succ_lr = child_lr->ovcoal_sibling;
    LIVE_RANGE *pred_lr = succ_lr;
    while (pred_lr->ovcoal_sibling != child_lr) {
      pred_lr = pred_lr->ovcoal_sibling;
    }
    pred_lr->ovcoal_sibling = succ_lr;
    child_lr->ovcoal_sibling = child_lr;
    if (lr->ovcoal_child == child_lr) {
      lr->ovcoal_child = pred_lr;
    }
  }
  child_lr->ovcoal_parent     = NULL;
  child_lr->special_def_op    = NULL;
  child_lr->ovcoal_local_rank = 0;
}
/* Fully detach 'lr' from its overlap coalescing tree.
 * Itself, its parent an its children will belongs to separate trees */
static void Detach_LR_ovcoal(LIVE_RANGE *lr) {
  LIVE_RANGE *child_lr, *next_lr;
  if (lr->ovcoal_child) {
    child_lr = lr->ovcoal_child;
    do {
      next_lr = child_lr->ovcoal_sibling;
      child_lr->ovcoal_sibling = child_lr;
      child_lr->ovcoal_parent  = NULL;
      child_lr->special_def_op = NULL;
      child_lr = next_lr;
    } while (child_lr->ovcoal_sibling != child_lr);
    lr->ovcoal_child = NULL;
  }
  if (lr->ovcoal_parent) {
    Remove_LR_ovcoal_child(lr->ovcoal_parent, lr);
  }
}
/* Return TRUE if 'is_ancestor_lr' belongs to ancestor nodes of 'lr'
 * in the coalescing tree, FALSE otherwise */
static BOOL LR_ovcoal_is_ancestor(LIVE_RANGE *lr, LIVE_RANGE *is_ancestor_lr) {
  LIVE_RANGE *lr_tmp = lr->ovcoal_parent;
  while (lr_tmp) {
    if (lr_tmp == is_ancestor_lr) {
      return (TRUE);
    }
    lr_tmp = lr_tmp->ovcoal_parent;
  }
  return (FALSE);
}
/* Return root node in overlap coalescing tree */
static LIVE_RANGE *LR_ovcoal_root(LIVE_RANGE *lr) {
  LIVE_RANGE *lr_tmp = lr;
  while (lr_tmp->ovcoal_parent) {
    lr_tmp = lr_tmp->ovcoal_parent;
  }
  return (lr_tmp);
}
#else
#define LR_first_def(lr)        ((lr)->first_def)
#define LR_last_use(lr)         ((lr)->last_use)
#define LR_exposed_use(lr)      ((lr)->exposed_use)
#endif
#define LR_def_cnt(lr)          ((lr)->def_cnt)
#define LR_use_cnt(lr)          ((lr)->use_cnt)
#define LR_flags(lr)            ((lr)->flags)
#ifdef TARG_ST
// [TTh] When using overlap coalescing, the preference might be
// retrieved from ancestor nodes of the coalescing tree.
// The preference is chosen by the following algorithm, which stop when
// a valid register/preference is found (highest priority first):
// #1 Select register(s) allocated to parent in coalescing tree,
// #2 Select lr->prefer_reg,
// #3 Select parent->prefer_reg.
// #4 Select parent->parent register(s)
// #5 Select parent->parent->prefer_reg
// Repeat #4 and #5 until the root of the coalescing tree is reached
// or a valid register/preference is found.
static REGISTER LRA_TN_register(TN *tn);
static REGISTER
LR_prefer_reg(LIVE_RANGE *lr) {
  mINT32 rank;
  REGISTER parent_reg;
  if (!LRA_overlap_coalescing) {
    return (lr->prefer_reg);
  }
  else {
    if (lr->ovcoal_parent) {
      parent_reg = LRA_TN_register(LR_tn(lr->ovcoal_parent));
      if ((parent_reg != REGISTER_UNDEFINED) && (parent_reg <= REGISTER_MAX)) {
	// Select parent register(s)
	return (parent_reg + lr->ovcoal_local_rank);
      }
    }
    if (lr->prefer_reg != REGISTER_UNDEFINED) {
      // Select lr proper preference
      return (lr->prefer_reg);
    }
    if (lr->ovcoal_parent) {
      INT rank_offset = lr->ovcoal_local_rank;
      LIVE_RANGE *par = lr->ovcoal_parent;
      while (par) {
	parent_reg = LRA_TN_register(LR_tn(par));
	if ((parent_reg != REGISTER_UNDEFINED) && (parent_reg <= REGISTER_MAX)) {
	  // Select ancestor reg
	  return (parent_reg + rank_offset);
	}
	else if (par->prefer_reg != REGISTER_UNDEFINED) {
	  // Select ancestor preference
	  return (par->prefer_reg + rank_offset);
	}
	rank_offset += par->ovcoal_local_rank;
	par = par->ovcoal_parent;
      }
    }
    // No preference found
    return (REGISTER_UNDEFINED);
  }
}
#define Set_LR_prefer_reg(lr, r) ((lr)->prefer_reg=(r))
#else
#define LR_prefer_reg(lr)       ((lr)->prefer_reg)
#endif
#define LR_first_spill(lr)	((lr)->first_spill)
#define LR_next(lr)             ((lr)->next)

#define LR_ASSIGNED      0x1    /* LR assigned register (Compute_Fat_Points) */
#define LR_ADDED         0x2    /* LR added to Live_LRs_Vector(Fix_LRA_Blues) */
#define LR_RELOADABLE    0x4    /* LR is reloadable. */
#define LR_FAT_ALLOCATED 0x8	// LR was allocated real register by fat point
                                // calculation
#define LR_BYTEABLE      0x10	// LR requires a byte-accessible register
#ifdef TARG_ST
#define LR_EARLY_CLOBBER 0x10   // LR is written early at its first def.
#define LR_OVCOAL_PREFER 0x20   // LR has already received a prefer through
                                //    overlap coalescing
#endif
#define LR_assigned(lr)         (LR_flags(lr) & LR_ASSIGNED)
#define Set_LR_assigned(lr)     (LR_flags(lr) |= LR_ASSIGNED)
#define LR_added(lr)            (LR_flags(lr) & LR_ADDED)
#define Set_LR_added(lr)        (LR_flags(lr) |= LR_ADDED)
#define LR_reloadable(lr)       (LR_flags(lr) & LR_RELOADABLE)
#define Set_LR_reloadable(lr)   (LR_flags(lr) |= LR_RELOADABLE)
#define Reset_LR_reloadable(lr) (LR_flags(lr) &= ~LR_RELOADABLE)
#define LR_fat_allocated(lr)    (LR_flags(lr) & LR_FAT_ALLOCATED)
#define Set_LR_fat_allocated(lr)(LR_flags(lr) |= LR_FAT_ALLOCATED)
#define Reset_LR_fat_allocated(lr)(LR_flags(lr) &= ~LR_FAT_ALLOCATED)
#ifdef TARG_ST
#define LR_subclass(lr)         ((lr)->sc)
#define Set_LR_subclass(lr,s)  ((lr)->sc = (s))
#define LR_singleton_ref(lr)    (Single_Register_Subclass(LR_subclass(lr)) != REGISTER_UNDEFINED)
inline BOOL LR_early_clobber (const LIVE_RANGE *lr) {
  return (LR_flags(lr) & LR_EARLY_CLOBBER) != 0;
}
static void Set_LR_early_clobber(LIVE_RANGE *lr) {
  LR_flags(lr) |= LR_EARLY_CLOBBER;
}
static void Reset_LR_early_clobber(LIVE_RANGE *lr) {
  LR_flags(lr) &= ~LR_EARLY_CLOBBER;
}
inline BOOL LR_has_ovcoal_prefer(const LIVE_RANGE *lr) {
  return (LR_flags(lr) & LR_OVCOAL_PREFER) != 0;
}
static void Set_LR_has_ovcoal_prefer(LIVE_RANGE *lr) {
  LR_flags(lr) |= LR_OVCOAL_PREFER;
}
static void Reset_has_ovcoal_prefer(LIVE_RANGE *lr) {
  LR_flags(lr) &= ~LR_OVCOAL_PREFER;
}
#endif
#ifdef TARG_X8664
#define LR_byteable(lr)         (LR_flags(lr) & LR_BYTEABLE)
#define Set_LR_byteable(lr)     (LR_flags(lr) |= LR_BYTEABLE)
#endif

#define LR_Is_Undefined_Local(lr) \
  (TN_is_local_reg(LR_tn(lr)) && LR_def_cnt(lr) == 0)

#ifdef TARG_ST
#define FOR_LR_AND_ALL_ALIASES(lr,alias) \
    LIVE_RANGE *alias = lr; \
    do {
#define FOR_LR_AND_ALL_ALIASES_END(lr,alias) \
      alias = LR_next_alias (alias); \
    } while (alias != lr);

static BOOL
TNs_alias_p (TN *tn1, TN *tn2) {
  return (! TN_is_local_reg (tn1)
	  && ! TN_is_local_reg (tn2)
	  && TN_register_class (tn1) == TN_register_class (tn2)
	  && TN_register (tn1) != REGISTER_UNDEFINED
	  && TN_register (tn1) == TN_register (tn2)
	  && TN_nhardregs (tn1) == TN_nhardregs (tn2));
}
#endif
#ifdef TARG_ST
/* Data structure used to keep track of the overlap coalescing class used by
 * dedicated registers during the backward allocation. It contains the 
 * class id (identified by the root of the correspoding tree), the rank
 * (subpart offset, used for multi-register TNs) and the number of LRs
 * currently using a particular dedicated register.
 */
#define OVCOAL_ROOT_UNDEF NULL
#define OVCOAL_RANK_UNDEF 0
#define OVCOAL_MSG        "OVCOAL:"
#define OVCOAL_BASE_NAME  "OVCOAL_TN"
static struct {
  LIVE_RANGE* root;
  mINT32      rank;
  mINT32      alive_count;
} ded_reg_ovcoal_info[ISA_REGISTER_CLASS_MAX_LIMIT+1][REGISTER_MAX+1];

static inline void Set_ded_reg_ovcoal_root(ISA_REGISTER_CLASS rc, REGISTER r, LIVE_RANGE* c, INT rank) {
  ded_reg_ovcoal_info[rc][r].root = c;
  ded_reg_ovcoal_info[rc][r].rank = rank;
}
static inline void Set_ded_reg_ovcoal_alive_count(ISA_REGISTER_CLASS rc, REGISTER r, INT32 c) {
  ded_reg_ovcoal_info[rc][r].alive_count = c;
}
static inline void Increment_ded_reg_ovcoal_alive_count(ISA_REGISTER_CLASS rc, REGISTER r) {
  ded_reg_ovcoal_info[rc][r].alive_count++;
}
static void Decrement_ded_reg_ovcoal_alive_count(ISA_REGISTER_CLASS rc, REGISTER r) {
  ded_reg_ovcoal_info[rc][r].alive_count--;
  if (ded_reg_ovcoal_info[rc][r].alive_count==0) {
    ded_reg_ovcoal_info[rc][r].root = OVCOAL_ROOT_UNDEF;
    ded_reg_ovcoal_info[rc][r].rank = OVCOAL_RANK_UNDEF;
  }
}
static inline LIVE_RANGE* ded_reg_ovcoal_root(ISA_REGISTER_CLASS rc, REGISTER r) {
  return (ded_reg_ovcoal_info[rc][r].root);
}
static inline INT32 ded_reg_ovcoal_alive_count(ISA_REGISTER_CLASS rc, REGISTER r) {
  return (ded_reg_ovcoal_info[rc][r].alive_count);
}
static inline INT32 ded_reg_ovcoal_rank(ISA_REGISTER_CLASS rc, REGISTER r) {
  return (ded_reg_ovcoal_info[rc][r].rank);
}
#endif

/* List of all the live ranges for a basic block */
static LIVE_RANGE *Live_Range_List;

static hTN_MAP live_range_map;

// The number of times we have tried to allocate registers for the 
// current basic block. We make different spilling decisions depending
// on the trip count.
static INT Trip_Count;
#define MAX_TRIP_COUNT 128

static ST *Magic_Spill_Location;

/* Array containing the LRs live across a fatpoint. */
static VECTOR Live_LRs_Vector;
#define LR_VECTOR_element(v,i)  ((LIVE_RANGE *)VECTOR_element(v,i))

#ifdef TARG_ST
static void
Delete_LR_and_aliases (VECTOR v, LIVE_RANGE *lr)
{
  FOR_LR_AND_ALL_ALIASES (lr, alias) {
    VECTOR_Delete_Element (v, alias);
  } FOR_LR_AND_ALL_ALIASES_END (lr, alias);
}
#endif
/* map from OP -> opnum */
static BB_OP_MAP op_to_opnum;

/* Data structures to keep track of the best spill candidate */

typedef enum {
  SPILL_NONE, 
  SPILL_GLOBAL_REG, 
  SPILL_LIVE_RANGE, 
  SPILL_MOVE_DEF,
  SPILL_MOVE_USE
} SPILL_KIND;

typedef struct spill_candidate {
  SPILL_KIND spill_kind;
  INT spill_num;
  union {
    struct {
      mREGISTER global_spill_reg;
      mISA_REGISTER_CLASS spill_cl;
    } s1;
    LIVE_RANGE *spill_lr;
    struct {
      LIVE_RANGE *move_lr;
      mUINT16 from;
      mUINT16 to;
    } s2;
  } u1;
  float cost;
  INT benefit;
  struct spill_candidate *next;
} SPILL_CANDIDATE;

//
// globals for calculating fat_points in Allocate_Registers
//
typedef INT32 FAT_POINTS_TYPE;
#define FAT_POINTS_MAX INT32_MAX
#define FAT_POINTS_MIN INT32_MIN
static FAT_POINTS_TYPE *fat_points;
static REGISTER last_infinite_freed;
static ISA_REGISTER_CLASS failing_class;
static hTN_MAP fat_point_regs_map;
static BOOL use_fat_point_regs;
static REGISTER reg_infinite;
static INT fatpoint_min;
static TN_SET *bb_live;
static INT local_spills = 0;
static INT global_spills = 0;
static INT trace_tn = 0;
static INT trace_bb = 0;
static INT trace_op = 0;
static ISA_REGISTER_CLASS trace_cl = (ISA_REGISTER_CLASS)2;
#define Calculating_Fat_Points() (failing_class != ISA_REGISTER_CLASS_UNDEFINED)

//
// don't do standard traces after we begin to calculate fat points.  tn's
// will have bogus registers and such.
//
#define Always_Trace(t) (t)
#define Do_LRA_Trace(t) ((t) && !Calculating_Fat_Points())
#define Do_LRA_Fat_Point_Trace(t) ((t) && Calculating_Fat_Points())

#ifdef TARG_ST

// The widest local TN (specified as number fo registers) in each register class.
static INT regclass_widest_local_TN[ISA_REGISTER_CLASS_MAX_LIMIT+1];

// The number of members in each subclass.
static INT subclass_size[ISA_REGISTER_SUBCLASS_MAX_LIMIT+1];
// We have a preferred subclass ordering: a subclass with a lower
// rank appears earlier in the ordering.
static INT subclass_rank[ISA_REGISTER_SUBCLASS_MAX_LIMIT+1];

static REGISTER
Single_Register_Subclass (ISA_REGISTER_SUBCLASS subclass);

// First of two unusable register conditions for extract/compose operations.
// In this case, we want to allocate to a single-register tn that
// is a result of an extract op, or a source of a compose op.
static REGISTER_SET
unusable_regs_for_multi_op_1 (TN *wide_tn, TN *narrow_tn[])
{
  // Need to avoid the situation where the wide_tn regs
  // are a non-identity permutation of the narrow_tn regs.
  // Detect when:
  //   -  wide tn is allocated, all but one narrow tn is allocated,
  //      and the allocated narrow tns are a non-identity permutation
  //      of the registers allocated to the wide tn.
  //      In this case, the unallocated narrow tn must not be
  //      allocated to the remaining register in the permutation.
  // [TTh] Note: narrow_nbregs is used to handle narrow TNs that
  // are multiple-register TN
  if (TN_register (wide_tn) != REGISTER_UNDEFINED
      && TN_register (wide_tn) <= REGISTER_MAX) {
    REGISTER wide_reg = TN_register (wide_tn);
    REGISTER_SET wide_regs = TN_registers (wide_tn);
    BOOL non_identity_permutation = FALSE;
    INT n_regs = TN_nhardregs (wide_tn);
    INT r;
    INT narrow_nbregs = TN_nhardregs (narrow_tn[0]);
    INT narrow_idx = 0;
    // Remove from wide_regs all registers allocated to
    // single-reg tns.
    for (r = 0; r < n_regs; r += narrow_nbregs) {
      REGISTER reg = TN_register (narrow_tn[narrow_idx++]);
      if (reg != REGISTER_UNDEFINED
	  && reg <= REGISTER_MAX) {
	if (reg >= wide_reg
	    && reg < (wide_reg + n_regs)
	    && reg != (wide_reg + r)) {
	  non_identity_permutation = TRUE;
	}
	wide_regs = REGISTER_SET_Difference_Range (wide_regs, reg,
						   reg + narrow_nbregs - 1);
      }
    }
    if (REGISTER_SET_Size (wide_regs) == narrow_nbregs) {
      if (non_identity_permutation) {
	// Only one register left and we have a
	// non-identity permutation.
	// Avoid the remaining register.
	return wide_regs;
      } else {
	// Only one register left, but so far we have an
	// identity permutation.
	// This must mean that using remaining register
	// maintains the identity permutation.
	return REGISTER_SET_EMPTY_SET;
      }
    }
  }
  return REGISTER_SET_EMPTY_SET;
}

// Second of two conditions for extract/compose operations.
// In this case, we want to allocate to a multi-register tn that
// is a source of an extract op, or a result of a compose op.
static REGISTER_SET
unusable_regs_for_multi_op_2 (TN *wide_tn, TN *narrow_tn[])
{
  // Need to avoid the situation where the wide_tn regs
  // are a non-identity permutation of the narrow_tn regs.
  // Detect when:
  //  - wide tn is unallocated
  //  - all narrow_tns are allocated consecutively, but not
  //      in sequence.
  //  In this case we must avoid allocating at least
  //  one of the allocated registers to the wide tn.
  // [TTh] Note: narrow_nbregs is used to handle narrow TNs that
  // are multiple-register TN
  if (TN_register (wide_tn) == REGISTER_UNDEFINED) {
    INT n_regs = TN_nhardregs(wide_tn);
    REGISTER base_reg = REGISTER_UNDEFINED;
    REGISTER low_reg = REGISTER_MAX + 1;
    REGISTER high_reg = 0;
    BOOL in_sequence = TRUE;
    INT narrow_nbregs = TN_nhardregs (narrow_tn[0]);
    INT narrow_idx = 0;
    for (INT r = 0; r < n_regs; r += narrow_nbregs) {
      REGISTER narrow_reg = TN_register (narrow_tn[narrow_idx++]);
      if (narrow_reg != REGISTER_UNDEFINED
	  && narrow_reg <= REGISTER_MAX) {
	if (base_reg == REGISTER_UNDEFINED) {
	  base_reg = narrow_reg - r;
	} else if (narrow_reg != (base_reg + r)) {
	  in_sequence = FALSE;
	}
	if (narrow_reg < low_reg) {
	  low_reg = narrow_reg;
	}
	if ((narrow_reg + narrow_nbregs - 1) > high_reg) {
	  high_reg = (narrow_reg + narrow_nbregs - 1);
	}
      } else {
	// There is an unallocated narrow reg, so no problem.
	return REGISTER_SET_EMPTY_SET;
      }
    }
    // The narrow tns are all allocated.
    if (! in_sequence && (high_reg - low_reg + 1) == n_regs) {
      // The registers allocated are consecutive, but not in sequence.
      // Avoiding one of them is sufficient to prevent a non-identity
      // permutation.
      return REGISTER_SET_Range(low_reg, low_reg);
    }
  }
  return REGISTER_SET_EMPTY_SET;
}
      
// [SC] For an extract op, OP, return the set of registers that
// must be avoided when allocating a result.
// The situation we want to avoid is being unable to expand
// the extract into a sequence of moves.  This happens when
// the result registers are a non-identity permutation of the
// source registers.
static REGISTER_SET
unusable_extract_result_regs (OP *op)
{
  TN *dsts[OP_MAX_FIXED_RESULTS];
  for (INT i = 0; i < OP_results(op); i++) {
    dsts[i] = OP_result(op, i);
  }
  return unusable_regs_for_multi_op_1 (OP_opnd(op, 0), dsts);
}

static REGISTER_SET
unusable_compose_operand_regs (OP *op)
{
  TN *opds[OP_MAX_FIXED_OPNDS];
  for (INT i = 0; i < OP_opnds(op); i++) {
    opds[i] = OP_opnd(op, i);
  }
  return unusable_regs_for_multi_op_1 (OP_result(op, 0), opds);
}

static REGISTER_SET
unusable_extract_operand_regs (OP *op)
{
  TN *dsts[OP_MAX_FIXED_RESULTS];
  for (INT i = 0; i < OP_results(op); i++) {
    dsts[i] = OP_result(op, i);
  }
  return unusable_regs_for_multi_op_2 (OP_opnd(op, 0), dsts);
}

static REGISTER_SET
unusable_compose_result_regs (OP *op)
{
  TN *opds[OP_MAX_FIXED_OPNDS];
  for (INT i = 0; i < OP_opnds(op); i++) {
    opds[i] = OP_opnd(op, i);
  }
  return unusable_regs_for_multi_op_2 (OP_result(op, 0), opds);
}

#endif

#ifdef TARG_X8664
static mBOOL float_reg_could_be_128bit[REGISTER_MAX + 1];
#endif

#ifdef KEY
// Whether to check the dependence graph to determine if a LR is reloadable.
static BOOL Need_Dep_Graph_For_Mark_Reloadable_Live_Ranges;
#endif

//
// allow 32 bit register values during fat point calculation.  tn register
// field too small, and we're using an infinite register set.  these should
// contain the only references to TN_register and TN_Allocate_Register in
// the file.
//
inline REGISTER
LRA_TN_register(TN *tn)
{
  REGISTER reg;
  
  //
  // if we have a valid register set in the tn, use it.  for speed,
  // we only put "infinite register set" registers in the hash table.
  // 
  reg = TN_register(tn);
  if (use_fat_point_regs && reg == REGISTER_UNDEFINED) {
    return((REGISTER)(INTPTR) hTN_MAP_Get(fat_point_regs_map, tn));
  }
  return(reg);
}

inline void
LRA_TN_Allocate_Register(TN *tn, REGISTER reg)
{
  //
  // for speed, put valid registers in the tn, even if
  // we're computing fat points.
  //
  if (use_fat_point_regs && reg > REGISTER_MAX) {
    hTN_MAP_Set(fat_point_regs_map, tn, (void *) reg);
  } else {
    TN_Allocate_Register(tn, reg);
  }
}
#undef TN_register
#undef TN_Allocate_Register
#define TN_register(tn) TN_dont_use_tn_register_error(tn)
#define TN_Allocate_Register(tn) TN_dont_use_tn_Allocate_Register_error(tn)

//
// these two traces are kludgy.  trace_op and trace_tn don't always match up,
// and you'll get some traces that aren't really the one's you want, but its
// okay for now ... you gotta hack the sources to use it, anyway.
//
static void
LRA_Print_Liveness()
{
  INT live_count;
  TN *tn;
  
  for (tn = TN_SET_Choose(bb_live), live_count=0;
       tn != TN_SET_CHOOSE_FAILURE;
       tn = TN_SET_Choose_Next(bb_live, tn), live_count++);

  fprintf(TFile,"<lra> Live at OP:%d (lc=%d): ", trace_op, live_count);
  TN_SET_Print(bb_live, TFile);
  fprintf(TFile,"\n");
}

static void
Print_Live_Across()
{
  INT cnt = 0;
  fprintf(TFile,"<lra> Live across %d: ", trace_op);

  for (INT i = 0; i < VECTOR_count(Live_LRs_Vector); i++) {
    LIVE_RANGE *lr = LR_VECTOR_element (Live_LRs_Vector, i);
    if (LR_first_def(lr) < trace_op && LR_last_use(lr) > trace_op) {
      cnt++;
      fprintf(TFile," TN%d", TN_number(LR_tn(lr)));
    }
  }
  fprintf(TFile,"(lc=%d)\n", cnt);
}

//
// convert an op to a noop. It's ok to remove any TOP_noop's, so
// there's no need to indentify the the ones we specifically create.
//
static inline void
Mark_For_Removal(OP *op)
{
  OP_Change_To_Noop(op);
}

//
// determine if an op has been marked for removal
//
inline BOOL
Is_Marked_For_Removal(OP *op)
{
  return OP_code(op) == TOP_noop;
}

//
// determine if reordering is allowed
//
static BOOL
Check_Allow_Reorder() {
#ifdef TARG_ST
  //
  // Arthur: must also check if scheduling is enabled all together
  //
  if ((CG_opt_level > 1) &&
      IGLS_Enable_All_Scheduling && LOCS_Enable_Scheduling &&
      (Trip_Count <= CG_opt_level) &&
      !(CG_LRA_mask & 0x2000)) {
#else
  if ((CG_opt_level > 1) &&
      (Trip_Count <= CG_opt_level) &&
      !Get_Trace (TP_ALLOC, 0x2000)) {
#endif
    return TRUE;
  }
  return FALSE;
}

//
// determine if we can call the local scheduler again
// on the block.
//
#ifdef TARG_ST
// [CG]: We pass the basic block now, as rescheduling
// decision is taken at BB granularity
static BOOL
Check_Allow_Reschedule(BB *bb)
#else
static BOOL
Check_Allow_Reschedule()
#endif
{
#ifdef TARG_ST
  // [CG]: When LAO is enabled, rescheduling takes place only 
  // if register_requirement > 1.5*(register avail)
  // Rescheduling can be disabled by -GC:LRA_mask=0x200
  // Rescheduling can be forced under -GC:LRA_mask=0x20000
#ifdef LAO_ENABLED
  if (!Check_Allow_Reorder()) return FALSE;
  if (Trip_Count != 1) return FALSE;
  if (CG_LRA_mask & 0x0200) return FALSE;

  if (!CG_LAO_activation) return TRUE;
  
  if (CG_LRA_mask & 0x20000) return TRUE; /* Force even if LAO */

  /* We estimate the registers request compared to the 
     registers availability. If it is above a fixed threshold (1.5)
     we allow rescheduling even after LAO prepass scheduler. */
  BOOL allow = FALSE;
  LRA_Request_Info request;
  LRA_Register_Request (bb, &request);
  ISA_REGISTER_CLASS cl;
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    int n_request = request.summary[cl];
    int n_avail = REGISTER_SET_Size(avail_set[cl]);
    if (n_avail * 1.5  < n_request) {
      DevWarn ("BB:%d rescheduled by LRA after LAO (lra_request:%d, lra_avail:%d)\n", BB_id(bb), n_request, n_avail);
      allow = TRUE; break;
    }
  }
  return allow;
#else
  if (Check_Allow_Reorder() &&
      Trip_Count == 1 &&
      !Get_Trace (TP_ALLOC, 0x0200)) {
    return TRUE;
  }
  return FALSE;
#endif
#else

  if (Check_Allow_Reorder() &&
      Trip_Count == 1 &&
      !Get_Trace (TP_ALLOC, 0x0200)) {
    return TRUE;
  }
  return FALSE;
#endif
}

//
// delete op from Insts_Vector and set its fat_point to -1
//
inline void
Clobber_Op_Info(INT opnum, ISA_REGISTER_CLASS spill_cl)
{
  Set_OP_VECTOR_element(Insts_Vector, opnum, NULL);
#ifdef KEY
  if (fat_points)
#endif
  fat_points[opnum] = -1;
}

//
// returns TRUE if the TN referenced in lr is used by the op specified
// by opnum.
//
static BOOL
Op_Uses_TN(TN *spill_tn, INT opnum)
{
  BOOL is_local_tn = TN_is_local_reg (spill_tn);
  REGISTER spill_reg = REGISTER_UNDEFINED;
#ifdef TARG_ST
  INT spill_nregs;
#endif
  OP *op = OP_VECTOR_element(Insts_Vector, opnum);
  ISA_REGISTER_CLASS cl = TN_register_class(spill_tn);

  if (!is_local_tn) {
    spill_reg = LRA_TN_register(spill_tn);
#ifdef TARG_ST
    spill_nregs = TN_nhardregs (spill_tn);
#endif
  } else {
    spill_reg = REGISTER_UNDEFINED;
#ifdef TARG_ST
    spill_nregs = 1;
#endif
  }
  //
  // op is null if instruction deleted from block during spilling of
  // a live range
  //
#ifdef TARG_ST
  return (op == NULL || (is_local_tn && OP_Refs_TN (op, spill_tn)) ||
	  (!is_local_tn && OP_Refs_Regs (op, cl, spill_reg, spill_nregs) > 0));
#else
  return (op == NULL || (is_local_tn && OP_Refs_TN (op, spill_tn)) ||
	  (!is_local_tn && OP_Refs_Reg (op, cl, spill_reg)));
#endif
}
#ifdef TARG_ST
static BOOL
Op_Uses_spill_tn (TN_LIST *spill_tns, INT opnum)
{
  OP *op = OP_VECTOR_element(Insts_Vector, opnum);

  if (op != NULL) {
    for (TN_LIST *spill_tn_list = spill_tns;
	 spill_tn_list != NULL;
	 spill_tn_list = TN_LIST_rest(spill_tn_list)) {
      TN *spill_tn = TN_LIST_first(spill_tn_list);
      if (Op_Uses_TN(spill_tn, opnum)) {
	return TRUE;
      }
    }
  }
  return FALSE;
}

static void print_lra_request (const LRA_Request_Info *request);
#endif

static void Print_Avail_Set (BB *bb)
{
  REGISTER reg;
  ISA_REGISTER_CLASS cl;

  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    fprintf (TFile, "(BB:%d, cl:%d) \n\tAvail:", BB_id(bb), cl);
    REGISTER_SET_Print (avail_set[cl], TFile);
    fprintf (TFile, "\n\tAvail(Registers):");
    FOR_ALL_REGISTER_SET_members (avail_set[cl], reg) {
      fprintf (TFile, " %s", REGISTER_name (cl, reg));
    }
    if (!CG_localize_tns) {
      fprintf (TFile, "\n\tGRA Grant:");
      REGISTER_SET_Print (GRA_Local_Register_Grant (bb, cl), TFile);
#ifdef TARG_ST
      fprintf (TFile, "\nLRA Request:");
      LRA_Request_Info request;
      LRA_Register_Request (bb, &request);
      print_lra_request (&request);
#else
      fprintf (TFile, "\n\tLRA Request: %d", LRA_Register_Request (bb, cl));
#endif
    }
    fprintf (TFile, "\n");
  }
}

#ifdef TARG_ST
/* Return a version id to uniquely identify a TN value. Several TNs might have
 * the same version if they contain the same value (i.e. one TN is a copy/extract
 * of the other one).
 */
static mINT32
Generate_Overlap_Coalescing_Version() {
  static mINT32 ver;
  return (ver++);
}

/* Considering that current register class can be divided in N composite registers,
 * each one containing exactly (multireg_size) atomic registers, this function
 * returns TRUE only if the specified hardware register (reg) have the expected
 * position (expected_rank) within its composite register.
 */
inline BOOL
REGISTER_Has_Expected_Rank(REGISTER reg, INT multireg_size, INT expected_rank) {
  INT real_reg_id = ((INT)reg - REGISTER_MIN);
  INT rank_mask  = multireg_size-1;
  return ((real_reg_id & rank_mask) == expected_rank);
}

/* Walk through all children of 'root_lr', and detach the ones that belongs to
 * rank range [ref_rank_min, ref_rank_max] and that have been redefined since the
 * last definition of 'used_lr'.
 */
static void
Detach_Conflicting_LR_From_Overlap_Coalescing_Class(LIVE_RANGE *root_lr,
						    LIVE_RANGE *used_lr,
						    INT ref_rank_min,
						    INT ref_rank_max) {
  LIVE_RANGE *cur_lr, *next_lr, *last_lr;

  last_lr = LR_ovcoal_child(root_lr);
  next_lr = LR_ovcoal_sibling(last_lr);

  do {
    cur_lr = next_lr;
    next_lr = LR_ovcoal_sibling(cur_lr);

    // Check if cur_lr belongs to the conflict rank range.
    // Avoid costly computation, when dealing with coalescing tree
    // containing only single register TNs.
    BOOL can_conflict = (ref_rank_min == -1);
    if (!can_conflict) {
      INT cur_rank_min = LR_ovcoal_rank(cur_lr);
      INT cur_rank_max = cur_rank_min + TN_nhardregs(LR_tn(cur_lr)) - 1;
      can_conflict = !((cur_rank_min > ref_rank_max) || (cur_rank_max < ref_rank_min));
    }
    if (can_conflict) {
      if (cur_lr != used_lr &&
	  ((LR_def_cnt(cur_lr) >  1) ||
	   (LR_def_cnt(cur_lr) == 1         &&
	    TN_is_global_reg(LR_tn(cur_lr)) &&
	    LR_exposed_use(cur_lr) > 0))) {
	// Current node is not the read one (used_lr) and has multiple definitions
	if (LR_ovcoal_last_ver(cur_lr) > LR_ovcoal_last_ver(used_lr)) {
	  // Conflicting definition of cur_lr: detach it from coalescing tree
	  if (Do_LRA_Trace(Trace_LRA_Detail)) {
	    fprintf (TFile, OVCOAL_MSG" Detach TN%d from its parent TN%d (redefinition)\n",
		     TN_number(LR_tn(cur_lr)), TN_number(LR_tn(root_lr)));
	  }
	  Remove_LR_ovcoal_child(LR_ovcoal_parent(cur_lr), cur_lr);
	}
      }
      if (LR_ovcoal_parent(cur_lr) && LR_ovcoal_child(cur_lr)) {
	// Perform a recursive walk of the children of cur_lr if the latter
	// one has not been detached.
	// Note: if the detached LR was an ancestor of 'used_lr', we would
	//       have to visit its children as well,  but it cannot happen
	//       because the pruning of ancestors has already been done in
	//       Update_Overlap_Coalescing_Info_For_Use()
	Detach_Conflicting_LR_From_Overlap_Coalescing_Class(cur_lr, used_lr, ref_rank_min, ref_rank_max);
      }
    }
  } while (cur_lr != last_lr);
}

/* This function is called during the forward walk of Setup_Live_Ranges(),
 * when a use of LR 'clr' is found.
 * It checks that none of the node of the coalescing tree has modified the
 * value used by 'clr' (identified by its version). Otherwise, the redefined 
 * nodes are removed from the coalescing tree.
 */
static void
Update_Overlap_Coalescing_Info_For_Use(LIVE_RANGE *clr) {
  LIVE_RANGE *iter_lr   = clr;
  LIVE_RANGE *parent_lr = LR_ovcoal_parent(clr);
  // Walk up the overlap coalescing class tree, and check for each ancestor
  // if it has been redefined since the last definition of the used LR.
  // If it is the case, it is required to split the coalescing tree in
  // 2 parts: one with the ancestor LR, and another one with the use_LR.
  while (parent_lr) {
    if (LR_ovcoal_last_ver(parent_lr) > LR_ovcoal_last_ver(clr)) {
      // No overlap coalescing possible
      if (Do_LRA_Trace(Trace_LRA_Detail)) {
	fprintf (TFile, OVCOAL_MSG" Detach TN%d from its parent  (redefinition)\n",
		 TN_number(LR_tn(iter_lr)));
      }
      Remove_LR_ovcoal_child(parent_lr, iter_lr);
      break;
    }
    else {
      iter_lr   = parent_lr;
      parent_lr = LR_ovcoal_parent(iter_lr);
    }
  }
  // At this point, iter_lr is the root of the coalescing tree.
  // Now iterate over the children of the root, to find child nodes that have
  // been redefined, and must then be removed from current coalescing class.
  if (LR_ovcoal_child(iter_lr)) {
    // Compute range of conflict; -1 is used for single register TN tree,
    // In order to avoid useless computation of range
    INT clr_rank_min=-1, clr_rank_max=-1;
    BOOL check_rank = (TN_nhardregs(LR_tn(iter_lr)) > 1);
    if (check_rank) {
      clr_rank_min = LR_ovcoal_rank(clr);
      clr_rank_max = clr_rank_min + TN_nhardregs(LR_tn(clr)) - 1;
    }
    Detach_Conflicting_LR_From_Overlap_Coalescing_Class(iter_lr, clr, clr_rank_min, clr_rank_max);
  }
}


/* Propagate preferred register through the overlap coalescing tree,
 * by walking up the coalescing tree and setting preference to parent
 * based on child
 */
static void
Propagate_Overlap_Coalescing_Preference_Leaf_To_Root(LIVE_RANGE *lr, REGISTER alloc_reg) {
  LIVE_RANGE *cur_lr, *parent_lr;

  if (alloc_reg == REGISTER_UNDEFINED || alloc_reg > REGISTER_MAX) {
    return; // Might happens during fat point computation 
  }

  cur_lr = lr;
  while ((parent_lr = LR_ovcoal_parent(cur_lr)) != NULL) {

    if (LR_extract_op(cur_lr)) {
      // Update pref register for 'parent_lr'
      if (((INT)alloc_reg - LR_ovcoal_local_rank(cur_lr)) < (INT)REGISTER_MIN) {
	break; // out of range
      }
      alloc_reg -= LR_ovcoal_local_rank(cur_lr);

      // Check if pref register is correctly aligned for 'parent_lr'
      INT rank_mask = TN_nhardregs(LR_tn(parent_lr))-1;
      if ((alloc_reg - REGISTER_MIN) & rank_mask != 0) {
	break; // misalignment
      }
    }
    cur_lr = parent_lr;

    if (LRA_TN_register(LR_tn(cur_lr)) != REGISTER_UNDEFINED) {
      break; // TN already allocated
    }
    else if (cur_lr->prefer_reg != REGISTER_UNDEFINED) {
      if (LR_has_ovcoal_prefer(cur_lr)) {
	// TN has already received a preference through overlap coalescing.
	break;
      }
      // Current node already have a preferencing, but that was propagated through
      // standard preferencing in Setup_Live_Ranges(). We will override it with overlap
      // coalescing preference.
      // Note that standard preferencing remain useful for coalescing with TNs not
      // candidate for overlap coalescing (mainly aliased GTN)
      Set_LR_has_ovcoal_prefer(cur_lr);
    }
    if (Do_LRA_Trace(Trace_LRA_Detail)) {
      fprintf (TFile, OVCOAL_MSG" preference %s to ",
	       REGISTER_name (TN_register_class(LR_tn(cur_lr)), alloc_reg));
      Print_TN(LR_tn(cur_lr),FALSE);
      fprintf(TFile, " (inherited from ");
      Print_TN(LR_tn(lr),FALSE);
      fprintf (TFile, ")\n");
    }
    Set_LR_prefer_reg(cur_lr, alloc_reg);
  }
}

#endif


#ifndef TARG_X8664
static
#endif
REGISTER Single_Register_Subclass (ISA_REGISTER_SUBCLASS subclass)
{
#ifdef TARG_ST
  if (subclass_size[subclass] == 1) {
    REGISTER_SET subclass_regs = REGISTER_SUBCLASS_members(subclass);
    return REGISTER_SET_Choose(subclass_regs);
  }
  return REGISTER_UNDEFINED;
#else
  REGISTER_SET subclass_regs = REGISTER_SUBCLASS_members(subclass);
  if (REGISTER_SET_Size(subclass_regs) == 1) {
    return REGISTER_SET_Choose(subclass_regs);
  }
  return REGISTER_UNDEFINED;
#endif
}
  

/* ======================================================================
 * Init_Avail_Set
 *
 * Initialize avail_set to list of registers that are available to LRA.
 * ======================================================================*/
static void Init_Avail_Set (BB *bb)
{
  REGISTER reg;
  ISA_REGISTER_CLASS cl;
  OP *op;
  INT i;

#ifdef TARG_ST
  // [TTh] Compute set of dedicated registers that are used by GTNs in the BB
  // but that are not live-out. Those registers might be marked as available
  // for LRA, the reverse traversal of the BB will insure that they are not
  // allocated to local TNs if they conflict with the corresponding GTNs.
  REGISTER_SET ded_reg_live_out_set[ISA_REGISTER_CLASS_MAX_LIMIT+1];
  REGISTER_SET ded_reg_non_live_out_set[ISA_REGISTER_CLASS_MAX_LIMIT+1];
  BOOL use_improved_avail_set = FALSE;
  // Refinement done only if liveness has been computed
  if (BB_bbregs(bb) && BB_live_in(bb) && BB_live_out(bb) &&
      !BB_entry(bb) && !BB_exit(bb) && !BB_call(bb)) {
    ISA_REGISTER_CLASS cl;
    TN *tn;
    
    use_improved_avail_set = TRUE;

    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      ded_reg_non_live_out_set[cl] = REGISTER_SET_EMPTY_SET;
      ded_reg_live_out_set[cl] = REGISTER_SET_EMPTY_SET;
    }
    // Build list of dedicated registers used by GTN that are live-out
    for (tn  = GTN_SET_Choose(BB_live_out(bb));
	 tn != GTN_SET_CHOOSE_FAILURE;
	 tn  = GTN_SET_Choose_Next(BB_live_out(bb), tn)) {
      if (LRA_TN_register(tn) != REGISTER_UNDEFINED) {
	ded_reg_live_out_set[TN_register_class(tn)] =
	  REGISTER_SET_Union(ded_reg_live_out_set[TN_register_class(tn)],
			     REGISTER_SET_Range(LRA_TN_register(tn), LRA_TN_register(tn) + TN_nhardregs(tn) - 1));
      }
    }
    // Build list of dedicated registers used by GTNs that are live-in but not live-out.
    // If current GTN contains some live-out registers (might happens when aliased),
    // then all its registers must be considered as live-out, to avoid potential problem
    // when inserting spill code.
    for (tn  = GTN_SET_Choose(BB_live_in(bb));
	 tn != GTN_SET_CHOOSE_FAILURE;
	 tn  = GTN_SET_Choose_Next(BB_live_in(bb), tn)) {
      if (LRA_TN_register(tn) != REGISTER_UNDEFINED) {
	if (!GTN_SET_MemberP(BB_live_out(bb), tn)) {
	  REGISTER_SET cur_gtn_set = REGISTER_SET_Range(LRA_TN_register(tn),
							LRA_TN_register(tn) + TN_nhardregs(tn) - 1);
	  cl = TN_register_class(tn);
	  if (!REGISTER_SET_IntersectsP(cur_gtn_set,
					ded_reg_live_out_set[cl])) {
	    ded_reg_non_live_out_set[cl] =
	      REGISTER_SET_Union(ded_reg_non_live_out_set[cl], cur_gtn_set);
	  }
	  else {
	    ded_reg_live_out_set[cl] =
	      REGISTER_SET_Union(ded_reg_live_out_set[cl], cur_gtn_set);
	  }
	}
      }
    }
    // Need to subtract live-out dedicated registers list from non-live-out registers list,
    // as several GTNs might use the same dedicated registers (they are then aliased).
    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      ded_reg_non_live_out_set[cl] =
	REGISTER_SET_Difference(ded_reg_non_live_out_set[cl],
				ded_reg_live_out_set[cl]);
      // Also remove forbidden registers
      ded_reg_non_live_out_set[cl] =
	REGISTER_SET_Difference(ded_reg_non_live_out_set[cl],
				CGTARG_Forbidden_LRA_Registers(cl));
      
      if (Do_LRA_Trace(Trace_LRA)) {
	if (!REGISTER_SET_EmptyP(ded_reg_non_live_out_set[cl])) {
	  fprintf (TFile, "(BB:%d, cl:%d) Non-live out registers used by GTNs:", BB_id(bb), cl);
	  FOR_ALL_REGISTER_SET_members (ded_reg_non_live_out_set[cl], reg) {
	    fprintf (TFile, " %s", REGISTER_name (cl, reg));
	  }
	  fprintf (TFile, "\n");
	}
      }
    }
  }
#endif // TARG_ST
  
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    if (CG_localize_tns) {
      /* Don't include callee save registers that have not already been
       * used by LRA (since they cost extra).  if this block is an exit,
       * we don't even add the used ones until we see the sp adjustment
       * (to avoid the use of a callee saved register after the restores).
       */
      avail_set[cl] = 
	REGISTER_SET_Difference (REGISTER_CLASS_allocatable(cl),
				 REGISTER_CLASS_callee_saves(cl));
#ifdef TARG_ST
      avail_set[cl] =
      REGISTER_SET_Difference (avail_set[cl],
                               CGTARG_Forbidden_LRA_Registers(cl));
#endif
#ifdef HAS_STACKED_REGISTERS
      REGISTER_SET avail_stacked =
	REGISTER_Get_Stacked_Avail_Set(ABI_PROPERTY_stacked, cl);
      avail_set[cl] = REGISTER_SET_Difference(avail_set[cl],
					      REGISTER_CLASS_stacked(cl));
      avail_set[cl] = REGISTER_SET_Union(avail_set[cl], avail_stacked);
#endif
      if (!BB_exit(bb)) {
	avail_set[cl] = REGISTER_SET_Union(avail_set[cl],
					   Callee_Saved_Regs_Used[cl]);
      }
    }
    else {
      /* Use the registers granted by GRA */
      avail_set[cl] = GRA_Local_Register_Grant (bb, cl);
#ifdef TARG_ST
      /* [TTh] Free dedicated registers used by GTNs in the BB but that are not live out of it */
      if (use_improved_avail_set) {
	avail_set[cl] = REGISTER_SET_Union(avail_set[cl], ded_reg_non_live_out_set[cl]);
      }
#endif // TARG_ST
      exclude_set[cl] = REGISTER_SET_EMPTY_SET;
      if (BB_call(bb)) {
        /* Note: all this code may be unnecessary because gra_grant already
                 adds caller_save registers into the local register grant. */
        OP *call_op = BB_last_op(bb);
        if ((call_op != NULL) &&
            OP_call(call_op) &&
            OP_cond_def(call_op)) {
          /* If the call is conditionally executed, the return value registers
             may already contain values that must be preserved through the block. */
#ifdef TARG_ST
          avail_set[cl] = REGISTER_SET_Union (
                            avail_set[cl],
                            REGISTER_SET_Difference (REGISTER_CLASS_function_value(cl),
                                                     BB_call_clobbered(bb, cl)));
#else
          avail_set[cl] = REGISTER_SET_Union (
                            avail_set[cl],
                            REGISTER_SET_Difference (REGISTER_CLASS_function_value(cl),
                                                     REGISTER_CLASS_caller_saves(cl)));
#endif
        } else {
#ifdef TARG_ST
          avail_set[cl] = REGISTER_SET_Union (
                            avail_set[cl],
			    REGISTER_SET_Intersection (REGISTER_CLASS_caller_saves(cl),
						       BB_call_clobbered(bb, cl)));
#else
          avail_set[cl] = REGISTER_SET_Union (
                            avail_set[cl], REGISTER_CLASS_caller_saves(cl));
#endif
        }
      }
      if (BB_mod_rotating_registers(bb) ||
	  Is_Predicate_REGISTER_CLASS(cl) && BB_mod_pred_rotating_registers(bb)) {
	exclude_set[cl] = REGISTER_Get_Requested_Rotating_Registers(cl);
	avail_set[cl] = REGISTER_SET_Difference (avail_set[cl], exclude_set[cl]);
      }
    }
  }

  /* Remove all dedicated and global TNs that are defined from the list of 
   * registers available at the bottom of the block. This needs to be done 
   * because GRA will include these registers in the grant.
   */
  FOR_ALL_BB_OPs_FWD (bb, op) {
    for (i = 0; i < OP_results(op); ++i) {
      TN *result_tn = OP_result(op, i);
      if (!TN_is_local_reg(result_tn)) {
        reg = LRA_TN_register(result_tn);
        cl = TN_register_class (result_tn);
#ifdef TARG_ST
        avail_set[cl] = REGISTER_SET_Difference_Range
	  (avail_set[cl], reg,
	   reg + TN_nhardregs (result_tn) - 1);
#else
        avail_set[cl] = REGISTER_SET_Difference1 (avail_set[cl], reg);
#endif
      }
    }
  }

#ifdef ICE9A_HW_WORKAROUND
  // Reserve f23 for the FP hardware bug workaround.  Bug 13481.
  if (CG_hw_round > 0 ||
      CG_hw_stall > 0) {
    avail_set[ISA_REGISTER_CLASS_float] =
      REGISTER_SET_Difference1(avail_set[ISA_REGISTER_CLASS_float], 24);
  }
#endif

  if (Do_LRA_Trace(Trace_LRA)) {
    Print_Avail_Set (bb);
  }
}


static void
Init_Insts_Vector (BB *bb, MEM_POOL *pool)
{
  OP *op;

  Insts_Vector = VECTOR_Init (BB_length(bb)+1, pool);
  /* make the zero'th element NULL. */
  VECTOR_Add_Element (Insts_Vector, NULL);
  FOR_ALL_BB_OPs_FWD (bb, op) {
    VECTOR_Add_Element (Insts_Vector, op);
  }

  if (VECTOR_count(Insts_Vector) != (BB_length(bb)+1)) {
    fprintf (TFile, "BAD VECTOR: length=%d\n", VECTOR_count(Insts_Vector));
    Print_BB (bb);
  }
}


/* ======================================================================
 * Return the live range for the <tn>. If one does not exist yet, create
 * an new live range and return it. For non-local TNs, the live range
 * is for the dedicated TN corresponding to the assigned register.
 * ======================================================================*/

static LIVE_RANGE *
LR_For_TN (TN *tn)
{
  LIVE_RANGE *lr;

#ifndef TARG_ST
  // [SC] This normalization does not work for multi-register TNs.
  if (!TN_is_local_reg(tn) && (LRA_TN_register(tn) != REGISTER_UNDEFINED)) {
    tn = Build_Dedicated_TN (TN_register_class(tn), LRA_TN_register(tn), 0);
  }
#endif
  lr = (LIVE_RANGE *) hTN_MAP_Get (live_range_map, tn);
  return lr;
}


/* For global TNs, the LR is for the dedicated TN corresponding to the
 * assigned register. If all references to this register are the same
 * global TN, use it for the LR_tn(lr) field. Otherwise use the dedicated
 * tn for the LR_tn field.
 */
static LIVE_RANGE *
Create_LR_For_TN (TN *tn, BB *bb, BOOL in_lra, MEM_POOL *pool)
{
  LIVE_RANGE *lr;
  BOOL local_lr = TN_is_local_reg(tn);
  TN *orig_tn = tn;
  ISA_REGISTER_CLASS cl = TN_register_class(tn);
  REGISTER reg = LRA_TN_register(tn);

#ifndef TARG_ST
  // [SC] This normalization does not work for multi-register TNs.
  if (!local_lr && (reg != REGISTER_UNDEFINED)) {
    tn = Build_Dedicated_TN (cl, reg, 0);
  }
#endif
  lr = (LIVE_RANGE *) hTN_MAP_Get (live_range_map, tn);
  if (lr == NULL) {
    lr = TYPE_MEM_POOL_ALLOC (LIVE_RANGE, pool);
    memset (lr, 0, sizeof(LIVE_RANGE));
    LR_tn(lr) = orig_tn;
    LR_next(lr) = Live_Range_List;

#ifdef TARG_ST
    Set_LR_next_alias(lr, lr);

    // [TTh] Initially, each TN has its own coalescing tree and
    // its own value version.
    if (LRA_overlap_coalescing) {
      mINT32 v = Generate_Overlap_Coalescing_Version();
      Set_LR_ovcoal_init_ver(lr, v);
      Set_LR_ovcoal_last_ver(lr, v);
      Set_LR_prefer_reg(lr, REGISTER_UNDEFINED);
    }
    Set_LR_ovcoal_parent (lr, NULL);
    Set_LR_ovcoal_child  (lr, NULL);
    Set_LR_ovcoal_sibling(lr, lr);
    Set_LR_ovcoal_local_rank(lr, 0);
#endif

    //
    // default is that globals spilled on entry.  will set otherwise
    // if left in register for some period of time.
    //
    LR_first_spill(lr) = 1;
#ifdef TARG_ST
    Set_LR_subclass(lr, ISA_REGISTER_SUBCLASS_UNDEFINED);
#endif   
    
    // If <cl,reg> is available for allocation at the end of the bb, then it
    // is not liveout of the bb.
    if (!local_lr && (!in_lra || !REGISTER_SET_MemberP (avail_set[cl], reg))) {
      /* make this live range extend just beyond the last OP in the bb */
#ifdef TARG_ST
      Set_LR_last_use (lr, BB_length(bb)+1);
#else
      LR_last_use(lr) = BB_length(bb)+1;
#endif
      /* increment use count to account for the TN being live-out */
      LR_use_cnt(lr)++;
    }
    Live_Range_List = lr;
    hTN_MAP_Set (live_range_map, tn, lr);
  }
  else if (LR_tn(lr) != orig_tn) {
#ifdef TARG_X8664
    // Make LR_tn(lr) be the widest enclosing register so that all parts of the
    // register are spilled.  Bug 7613.
    Is_True(TN_is_dedicated(tn), ("Create_LR_For_TN: not dedicated TN"));
    if (TN_size(tn) >= TN_size(LR_tn(lr)))
      LR_tn(lr) = tn;
    else {
      ISA_REGISTER_CLASS cl = TN_register_class(LR_tn(lr));
      REGISTER reg = LRA_TN_register(LR_tn(lr));
      TN *ded_tn = Build_Dedicated_TN (cl, reg, TN_size(LR_tn(lr)));
      LR_tn(lr) = ded_tn;
    }
#else
    LR_tn(lr) = tn;
#endif
  }
  return lr;
}


static void
Print_BB_For_LRA (BB *bb) 
{
  OP *op;
  INT i;
  fprintf (TFile, "--------------------------------------------\n");
  fprintf (TFile, "LRA for BB:%d  trip_count:%d\n", BB_id(bb), Trip_Count);
  fprintf (TFile, "--------------------------------------------\n");
  i = 0;
  FOR_ALL_BB_OPs_FWD (bb, op) {
    i++;
    fprintf (TFile, "OP:%2d> ", i);
    Print_OP_No_SrcLine (op);
  }
}


static void
Print_Live_Range (LIVE_RANGE *lr)
{
#ifdef TARG_ST
  fprintf (TFile, "  %s_LR>TN%d  %3d%s(%d) to %3d(%d), exposed:%d ",
                TN_is_local_reg(LR_tn(lr)) ? "LOCAL" : "GLOBAL",
                TN_number(LR_tn(lr)), LR_first_def(lr),
                (LR_early_clobber(lr) ? "E" : ""), LR_def_cnt(lr),
	        LR_last_use(lr), LR_use_cnt(lr), LR_exposed_use(lr));
  if (LR_prefer_reg (lr) != REGISTER_UNDEFINED) {
    fprintf (TFile, "prefer ");
    REGISTER_Print (TN_register_class (LR_tn (lr)), LR_prefer_reg (lr), TFile);
  }
  if (LR_subclass (lr) != ISA_REGISTER_SUBCLASS_UNDEFINED) {
    fprintf (TFile, " subclass:%s", REGISTER_SUBCLASS_name (LR_subclass (lr)));
  }
  if (LR_ovcoal_parent(lr)) {
    fprintf(TFile, " "OVCOAL_BASE_NAME"%d (rank:%d)", TN_number(LR_tn(LR_ovcoal_parent(lr))),
	    LR_ovcoal_local_rank(lr));
  }
  fprintf (TFile, "\n");
#else
  fprintf (TFile, "  %s_LR>TN%d  %3d(%d) to %3d(%d), exposed:%d\n",
                TN_is_local_reg(LR_tn(lr)) ? "LOCAL" : "GLOBAL",
                TN_number(LR_tn(lr)), LR_first_def(lr), LR_def_cnt(lr),
                LR_last_use(lr), LR_use_cnt(lr), LR_exposed_use(lr));
#endif
}


static void
Print_Live_Ranges (BB *bb)
{
  LIVE_RANGE *lr;

  fprintf (TFile, "--------------------------------------------\n");
  fprintf (TFile, "LIVE RANGES for BB%d\n", BB_id(bb));
  fprintf (TFile, "--------------------------------------------\n");
  for (lr = Live_Range_List; lr != NULL; lr = LR_next(lr)) {
    Print_Live_Range (lr);
  }
#ifdef TARG_ST
  ISA_REGISTER_CLASS cl;
  FOR_ALL_ISA_REGISTER_CLASS (cl) {
    REGISTER r;
    for (r = REGISTER_MIN; r <= REGISTER_CLASS_last_register(cl); r++) {
      if (ded_reg_first_def (cl, r) != INT32_MAX
	  || ded_reg_last_use (cl, r) != 0
	  || ded_reg_exposed_use (cl, r) != 0) { // [TTh]: Also display dedicated reg used but not live out of current BB
	fprintf (TFile, "  DED_LR>%s  %3d to %3d, exposed:%d \n",
		 REGISTER_name(cl, r), ded_reg_first_def (cl, r),
		 ded_reg_last_use(cl, r), ded_reg_exposed_use(cl, r));
      }
    }
  }
#endif 
}


/* Mark that TN is used in OP. */
static void
Mark_Use (TN *tn, OP *op, INT opnum, BB *bb, BOOL in_lra,
	  INT res_opnd_idx, BOOL is_result, MEM_POOL *pool)
{
  LIVE_RANGE *clr;

  clr = Create_LR_For_TN (tn, bb, in_lra, pool);
#ifdef TARG_ST
      ISA_REGISTER_SUBCLASS subclass = asm_info ?
        ASM_OP_opnd_subclass(asm_info)[opndnum] : OP_opnd_reg_subclass(op, opndnum);
      if (subclass != ISA_REGISTER_SUBCLASS_UNDEFINED
	  && subclass_rank[subclass] < subclass_rank[LR_subclass(clr)]) {
	Set_LR_subclass(clr, subclass);
      }
#endif

  if (!TN_is_local_reg(tn)) {
    if (LR_def_cnt(clr) == 0) {
#ifdef TARG_ST
	  Set_LR_exposed_use (clr, opnum);
#else
      LR_exposed_use(clr) = opnum;
#endif
    }
    /* Add this use to the live range for this TN. */
    LR_use_cnt(clr)++;
#ifdef KEY
    // If the live range is not live-out, then set the last use so that we can
    // determine which OPs the live range spans.  This is needed for making the
    // live range a candidate for spilling.  Bug 7649.
    if (LR_last_use(clr) != BB_length(bb)+1)
      LR_last_use(clr) = opnum;
#endif
  }
  else {
#ifdef TARG_X8664 
    if (!TN_is_preallocated (tn))
#endif /* TARG_X8664 */
      LRA_TN_Allocate_Register (tn, REGISTER_UNDEFINED);
    if (LR_def_cnt(clr) == 0) {
#ifdef TARG_ST
	  if (in_lra && !LRA_no_uninit_strict_check && !OP_prologue(op) && !TN_is_dedicated(tn)) {
	    // FdF 20080618: Set an assert, this case always mean
	    // wrong transformation.
	    Is_True(0, ("TN%d(PREG%d) used before definition in BB:%d",
			TN_number(tn), TN_To_PREG(tn), BB_id(bb)));
	  }
#endif
      if (!OP_dummy(op) && in_lra && (!BB_entry(bb) || (TFile != stdout))) {
	/* This can happen only if we have a use before a definition. */
        /* Note: ignore things in the first block, where we may
           legitimately need to save input registers. */
	DevWarn ("TN%d(PREG%d) used before definition in BB:%d",
		 TN_number(tn), TN_To_PREG(tn), BB_id(bb));	
#ifdef TARG_X8664
#if 0
	// When the inliner is on, it leads to this assert sometimes, when
	// the inlined code does not have a return statement but the
	// return type is non-void. There is no way to distinguish this 
	// case from a legitimate error. 
	if( !CGTARG_Is_Preference_Copy(op) ){
	  FmtAssert ( FALSE, ("TN%d(PREG%d) used before definition in BB:%d",
			      TN_number(tn), TN_To_PREG(tn), BB_id(bb)));
	}
#endif
#endif
      }
#ifdef TARG_ST
          Set_LR_exposed_use(clr, opnum);
#else
      LR_exposed_use(clr) = opnum;
#endif
    }
    /* Add this use to the live range for this TN. */
#ifdef TARG_ST
        Set_LR_last_use (clr, opnum);
#else
    LR_last_use(clr) = opnum;
#endif
    LR_use_cnt(clr)++;
  }
#ifdef TARG_ST
      if (LRA_overlap_coalescing) {
	// [TTh] Check that no LR from the overlap coalescing class 
	// has been redefined since the definition of current LR.
	// otherwise it is require to split the coalescing class.
	Update_Overlap_Coalescing_Info_For_Use(clr);
      }
#endif

#ifdef TARG_X8664
  // If the TN is used as a byte, must allocate to a byte-accessible register.
  if (OP_code(op) == TOP_asm) {
    ASM_OP_ANNOT* asm_info = (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op);
    ISA_REGISTER_SUBCLASS subclass =
      is_result ? ASM_OP_result_subclass(asm_info)[res_opnd_idx] :
		  ASM_OP_opnd_subclass(asm_info)[res_opnd_idx];
    if (subclass == ISA_REGISTER_SUBCLASS_m32_8bit_regs) {
      Set_LR_byteable(clr);
    }
  } else if (Is_Target_32bit() &&
	     ((is_result && OP_result_size(op, res_opnd_idx) == 8) ||
	      (!is_result && OP_opnd_size(op, res_opnd_idx) == 8))) {
    Set_LR_byteable(clr);
  }
#endif
}
#ifdef TARG_ST
static void 
Print_Overlap_Coalescing() {
  fprintf (TFile, "--------------------------------------------\n");
  fprintf (TFile, "Overlap coalescing status for live registers\n");
  fprintf (TFile, "--------------------------------------------\n");
  ISA_REGISTER_CLASS cl;
  FOR_ALL_ISA_REGISTER_CLASS (cl) {
    REGISTER r;
    for (r = REGISTER_MIN; r <= REGISTER_CLASS_last_register(cl); r++) {
      if (ded_reg_ovcoal_alive_count(cl, r) > 0) {
	fprintf (TFile, "  [%s] ", REGISTER_name(cl, r));

	if (ded_reg_ovcoal_root(cl, r) == OVCOAL_ROOT_UNDEF) {
	  fprintf (TFile, "class:UNDEF rank:UNDEF use_count:%d\n",
		   ded_reg_ovcoal_alive_count(cl, r));
	} else {
	  fprintf (TFile, "class:"OVCOAL_BASE_NAME"%d rank:%d use_count:%d\n",
		   TN_number(LR_tn(ded_reg_ovcoal_root(cl, r))),
		   ded_reg_ovcoal_rank(cl, r),
		   ded_reg_ovcoal_alive_count(cl, r));
	}
      }
    }
  }
}
#endif

/* ======================================================================
 * Create the live ranges for all the local TNs in the basic block.
 *
 * Output:   
 *
 *    1. live_range_map: live ranges for all TNs referenced in bb.
 *    2. Live_Range_List: list of all live ranges in bb.
 *    3. Insts_Vector: vector of instructions in the bb.
 *    4. TN_register field for all local TNs set to REGISTER_UNDEFINED.
 * ======================================================================*/
static void
Setup_Live_Ranges (BB *bb, BOOL in_lra, MEM_POOL *pool)
{
  INT opnum;

  /* create a new live_range_map every time through this routine. The 
   * old one is automatically discarded. 
   * TODO: reclaim space for the the old live_range_map.
   */
  live_range_map = hTN_MAP_Create (pool);
  Live_Range_List = NULL;

  Init_Insts_Vector (bb, pool);
#ifdef TARG_ST
 {
   ISA_REGISTER_CLASS cl;
   FOR_ALL_ISA_REGISTER_CLASS (cl) {
     REGISTER r;
     for (r = REGISTER_MIN; r <= REGISTER_MAX; r++) {
       Set_ded_reg_first_def (cl, r, INT32_MAX);
       Set_ded_reg_last_use (cl, r, 0);
       Set_ded_reg_exposed_use (cl, r, 0);
       // [TTh] Initialize overlap coalescing infos
       Set_ded_reg_ovcoal_root(cl, r, OVCOAL_ROOT_UNDEF, OVCOAL_RANK_UNDEF);
       Set_ded_reg_ovcoal_alive_count(cl, r, 0);
     }
   }
 }
#endif

  for (opnum = 1; opnum <= BB_length(bb); opnum++) {
    OP *op = OP_VECTOR_element (Insts_Vector, opnum);
    INT opndnum;
    INT resnum;
#ifdef TARG_ST
    ASM_OP_ANNOT* asm_info = (OP_code(op) == TOP_asm) ?
      (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op) : NULL;
#endif

    /* process all the operand TNs. */
    for (opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
      TN *tn = OP_opnd(op, opndnum);
      if (TN_is_register(tn)) 
	Mark_Use (tn, op, opnum, bb, in_lra, opndnum, FALSE, pool);
    }

    /* process the result TN */
    for (resnum = 0; resnum < OP_results(op); ++resnum) {
#ifdef KEY
      if (OP_cond_def(op)) {	// there is a hidden use
	TN *tn = OP_result(op, resnum);
	if (TN_is_register(tn)) {
	  Mark_Use (tn, op, opnum, bb, in_lra, resnum, TRUE, pool);
	}
      }
#endif
      TN *tn = OP_result(op, resnum);
      LIVE_RANGE *clr = Create_LR_For_TN (tn, bb, in_lra, pool);
      if (OP_cond_def(op) &&
          TN_is_global_reg(tn) &&
          (!TN_is_dedicated(tn) ||
           (CGTARG_Is_Preference_Copy(op) &&
            TN_is_local_reg(OP_opnd(op,CGTARG_Copy_Operand(op))) &&
            TN_spill(OP_opnd(op,CGTARG_Copy_Operand(op))))) &&
          (LRA_TN_register(tn) != REGISTER_UNDEFINED) &&
          !REGISTER_SET_MemberP(avail_set[TN_register_class(tn)], LRA_TN_register(tn))) {
       /* Note: This series of checks was added because
          conditionally defined values may require that a previous value be
          carried into a block. We also need to detect when a spilled value
          was reloaded and is copied into a dedicated register, because the
          code that creates spills and reloads is not smart enough to add
          the proper predicates to the memory ops. 

          The intent of this check is to extend the live range of the register
          to the top of the block.  Unfortunately, this is overly conservative
          and sometimes results in a free register being unavailable for use.
       */
        if (Do_LRA_Trace(Trace_LRA_Detail)) {
          fprintf (TFile, "OP:%d>> don't reuse result reg >> ", opnum); Print_OP_No_SrcLine (op);
        }

#ifdef TARG_ST
	Set_LR_exposed_use (clr, opnum);
#else
        LR_exposed_use(clr) = opnum;
#endif

      }
      if (LR_def_cnt(clr) == 0) {
#ifdef TARG_ST
	if (asm_info
	    && ASM_OP_result_clobber(asm_info)[resnum]) {
	  // [SC] an early-clobber result
	  Set_LR_first_def (clr, opnum, TRUE);
	} else {
	  Set_LR_first_def (clr, opnum);
	}
#else
        LR_first_def(clr) = opnum;
#endif
      }
      LR_def_cnt(clr)++;
#ifdef TARG_ST
      ISA_REGISTER_SUBCLASS subclass = asm_info ?
        ASM_OP_result_subclass(asm_info)[resnum] : OP_result_reg_subclass(op, resnum);
      if (subclass != ISA_REGISTER_SUBCLASS_UNDEFINED
	  && subclass_size[subclass] < subclass_size [LR_subclass(clr)]) {
	Set_LR_subclass (clr, subclass);
      }
      if (LRA_overlap_coalescing) {
	// [TTh] If overlap coalescing is enabled, check if current LR is
	// defined by a special OP (copy, extract) and if so, insert it
	// with the source TN coalescing tree.
	if (OP_cond_def(op) || LR_early_clobber(clr)) {
	  if (Do_LRA_Trace(Trace_LRA_Detail)) {
	    fprintf (TFile, OVCOAL_MSG" Detach TN%d from its tree (conditional def or early clobbered)\n",
		     TN_number(tn));
	  }
	  Detach_LR_ovcoal(clr);
	}
	else if (LR_def_cnt(clr) == 1 &&
		 (TN_is_local_reg(tn) ||
		  LR_exposed_use(clr)==0 ||
		  (LR_ovcoal_parent(clr) == NULL && LR_ovcoal_child(clr) == NULL))) {
	  // The following TN usages are accepted:
	  // (1) first def of a local TN,
	  // (2) first def of a global TN without exposed use,
	  // (3) first real def of a global TN with exposed use but not belonging to a coalescing tree
	  LIVE_RANGE *src_lr = NULL;
	  if (OP_extract(op)) {
	    src_lr = LR_For_TN(OP_opnd(op, OP_is_predicated(op)?1:0));
	  }
	  else if (OP_Is_Preference_Copy(op)) {
	    src_lr = LR_For_TN(OP_opnd(op, OP_Copy_Operand(op)));
	  }
	  if (src_lr && !LR_early_clobber(src_lr) &&
	      src_lr != clr && // case of copy of a reg in itself (typically ABI related)
	      (LR_exposed_use(clr) == 0 ||
	       LR_first_def(LR_ovcoal_root(src_lr)) >= LR_exposed_use(clr)) ) {
	    // This result is defined by an extract or copy,
	    // Update overlap coalescing info.
	    Set_LR_special_def_op (clr, op);
	    if (LR_ovcoal_is_ancestor(src_lr, clr)) {
	      // Cyclic dependency not supported:
	      // A TN cannot be both defined by and the definer of another TN.
	      if (Do_LRA_Trace(Trace_LRA_Detail)) {
		fprintf (TFile, OVCOAL_MSG" Detach TN%d from its tree (possible cyclic definition)\n",
			 TN_number(LR_tn(src_lr)));
	      }
	      Detach_LR_ovcoal(src_lr);
	    }
	    else {
	      // Current LR is an exact (partial) copy of the source,
	      // we must check that it is either not allocated or it is
	      // allocated to a register compatible with the existing
	      // coalescing tree.
	      BOOL compatible = TRUE;
	      INT  local_rank = 0;
	      if (OP_extract(op) && resnum!=0) {
		local_rank = resnum*TN_nhardregs(tn);
	      }
	      if (LRA_TN_register(tn) != REGISTER_UNDEFINED) {
		LIVE_RANGE *iter_lr = src_lr;
		REGISTER iter_reg;
		INT rank = local_rank;
		while (iter_lr) {
		  iter_reg = LRA_TN_register(LR_tn(iter_lr));
		  if (iter_reg != REGISTER_UNDEFINED) {
		    break;
		  }
		  rank   += LR_ovcoal_local_rank(iter_lr);
		  iter_lr = LR_ovcoal_parent(iter_lr);
		}
		compatible = ((iter_reg == REGISTER_UNDEFINED) ||
			      (LRA_TN_register(tn) == iter_reg+rank));
	      }
	      if (compatible) {
		if (Do_LRA_Trace(Trace_LRA_Detail)) {
		  fprintf (TFile, OVCOAL_MSG" Attach TN%d to parent TN%d\n",
			   TN_number(tn), TN_number(LR_tn(src_lr)));
		}
		Add_LR_ovcoal_child(src_lr, clr, local_rank);
		Set_LR_ovcoal_init_ver(clr, LR_ovcoal_last_ver(src_lr));
		if (LR_exposed_use(clr) != 0) {
		  // Update version number if dealing with a GTN with an exposed used
		  Set_LR_ovcoal_last_ver(clr, LR_ovcoal_last_ver(src_lr));
		}
	      }
	    }
	  }
	}
	else {
	  // LR potentially redefined: increment its version or inherits the one
	  // from the source TN in case of preference copy between  2 TNs of the
	  // same coalescing tree (the latter case happens for  instance  when a
	  // GTN is copied into a local TN, that is then updated and finally
	  // copied  back into the initial GTN).
	  LIVE_RANGE *src_lr = NULL;
	  BOOL done = FALSE;
	  if (OP_Is_Preference_Copy(op)) {
	    src_lr = LR_For_TN(OP_opnd(op, OP_Copy_Operand(op)));
	    if (src_lr && !LR_early_clobber(src_lr) &&
		LR_ovcoal_root(src_lr) == LR_ovcoal_root(clr)) {
	      // Inherits version from source TN
	      Set_LR_ovcoal_last_ver(clr, LR_ovcoal_last_ver(src_lr));
	      done = TRUE;
	    }
	  }
	  if (!done) {
	    // Increment LR version
	    Set_LR_ovcoal_last_ver(clr, Generate_Overlap_Coalescing_Version());
	  }
	}
	
	// It is not allowed to have several LRs of the same coal class
	// to be written by the same instruction (except for extract op).
	if (LR_ovcoal_parent(clr) && OP_results(op) > 1 && !OP_extract(op)) {
	  LIVE_RANGE *clr_root = LR_ovcoal_root(clr);
	  INT resnum2;
	  for (resnum2 = 0; resnum2 < OP_results(op); ++resnum2) {
	    if (resnum2 != resnum) {
	      TN *tn2 = OP_result(op, resnum2);
	      if (TN_register_class(tn2) == TN_register_class(tn)) {
		LIVE_RANGE *clr2 = LR_For_TN(tn2);
		// Note: clr2 == NULL means that tn2 is defined by current op
		//       -> it will then not belongs to coalescing tree of clr
		if ((clr2 != NULL) && (LR_ovcoal_root(clr2) == clr_root)) {
		  // Same coalescing class, need to detach one of them
		  if (LR_ovcoal_is_ancestor(clr2, clr)) {
		    Remove_LR_ovcoal_child(LR_ovcoal_parent(clr2), clr2);
		  } else {
		    Remove_LR_ovcoal_child(LR_ovcoal_parent(clr), clr);
		    break;
		  }
		}
	      }
	    }
	  }
	}
      }
      else {
	if (OP_extract(op)) {
	  // [SC] This result is defined by an extract.
	  // Keep a note of the extract source.
	  Set_LR_special_def_op (clr, op);
	}
      }
#endif
      if (TN_is_local_reg(tn)) {
#ifdef TARG_X8664 
	if (!TN_is_preallocated (tn))
#endif /* TARG_X8664 */
        LRA_TN_Allocate_Register (tn, REGISTER_UNDEFINED);
#ifdef TARG_ST
        Set_LR_last_use (clr, opnum);
#else
        LR_last_use(clr) = opnum;
#endif
#ifdef TARG_ST
	  // Default copy/extract preferencing used
        if (OP_Is_Preference_Copy(op)) {
          TN *src_tn = OP_opnd(op, OP_Copy_Operand(op));
	  ISA_REGISTER_CLASS cl = TN_register_class (src_tn);
          if (!TN_is_local_reg(src_tn)) {
	    Set_LR_prefer_reg(clr, LRA_TN_register(src_tn));
	    if (Do_LRA_Trace(Trace_LRA_Detail)) {
	      fprintf (TFile, "OP:%d>> preference %s to ", opnum,
		       REGISTER_name (cl, LR_prefer_reg(clr)));
	      Print_TN(tn,FALSE);
	      fprintf (TFile, "\n");
	    }
	  }
        }
	else if (OP_compose(op)) {
	  // [SC} Preference compose result to an operand
	  ISA_REGISTER_SUBCLASS sc = OP_result_reg_subclass(op, resnum);
	  REGISTER_SET subclass_regs = REGISTER_SUBCLASS_members(sc);
	  for (INT i = 0; i < OP_opnds(op); i++) {
	    TN *src_tn = OP_opnd(op, i);
	    REGISTER src_reg = LRA_TN_register (src_tn);
	    // [TTh] Source TNs might be multiple-registers
	    //       (consider that source TNs all have same size)
	    INT src_offs = i * TN_nhardregs(src_tn);
	    if (!TN_is_local_reg(src_tn)
		&& src_reg != REGISTER_UNDEFINED
		&& src_reg >= (REGISTER_MIN + src_offs)
		&& REGISTER_SET_MemberP (subclass_regs, src_reg - src_offs)) {
	      Set_LR_prefer_reg(clr, (src_reg - src_offs));
	      if (Do_LRA_Trace(Trace_LRA_Detail)) {
		fprintf (TFile, "OP:%d>> preference %s to ", opnum,
			 REGISTER_name (TN_register_class (src_tn),
					LR_prefer_reg (clr)));
		Print_TN(tn,FALSE);
		fprintf (TFile, "\n");
	      }
	      break;
	    }
	  }
	} else if (OP_extract(op)) {
	  // [SC] Preference extract result to the operand
	  TN *src_tn = OP_opnd(op, 0);
	  REGISTER src_reg = LRA_TN_register (src_tn);
	  // [TTh] Extracted TN might be multiple-registers
	  //       (consider that extracted TNs all have same size)
	  INT res_offs = resnum * TN_nhardregs(tn);
	  if (!TN_is_local_reg(src_tn)
	      && src_reg != REGISTER_UNDEFINED
	      && res_offs < TN_nhardregs(src_tn)) {
	    Set_LR_prefer_reg(clr, (src_reg + res_offs));
	    if (Do_LRA_Trace(Trace_LRA_Detail)) {
	      fprintf (TFile, "OP:%d>> preference %s to ", opnum,
		       REGISTER_name (TN_register_class (src_tn),
				      LR_prefer_reg (clr)));
	      Print_TN(tn,FALSE);
	      fprintf (TFile, "\n");
	    }
	  }
	}
#else
        if (CGTARG_Is_Preference_Copy(op)) {
          TN *src_tn = OP_opnd(op, CGTARG_Copy_Operand(op));
          if (!TN_is_local_reg(src_tn))
            LR_prefer_reg(clr) = LRA_TN_register(src_tn);
        }
#endif
      }
      else {
        if (CGTARG_Is_Preference_Copy(op)) {
#ifdef TARG_ST
          TN *src_tn = OP_opnd(op, OP_Copy_Operand(op));
          if (!TN_is_local_reg(src_tn)) {
            Set_LR_prefer_reg(clr, LRA_TN_register(src_tn));
	    if (Do_LRA_Trace(Trace_LRA_Detail)) {
	      fprintf (TFile, "OP:%d>> preference %s to ", opnum,
		       REGISTER_name (TN_register_class(src_tn),
				      LR_prefer_reg(clr)));
	      Print_TN(tn,FALSE);
	      fprintf (TFile, "\n");
	    }
	  }
#else
          TN *src_tn = OP_opnd(op,CGTARG_Copy_Operand(op));
#endif
          if (TN_is_local_reg(src_tn)) {
            LIVE_RANGE *src_lr = LR_For_TN (src_tn);
#ifdef TARG_ST
            Set_LR_prefer_reg(src_lr, LRA_TN_register(tn));
#else
            LR_prefer_reg(src_lr) = LRA_TN_register(tn);
#endif
          }
        }
      }
#ifdef TARG_X8664
      // If the result is a byte, must allocate to a byte-accessible register.
      if (OP_code(op) == TOP_asm) {
	ASM_OP_ANNOT* asm_info = (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op);
	ISA_REGISTER_SUBCLASS subclass =
	  ASM_OP_result_subclass(asm_info)[resnum];
	if (subclass == ISA_REGISTER_SUBCLASS_m32_8bit_regs) {
	  Set_LR_byteable(clr);
	}
      } else if (Is_Target_32bit() &&
		 OP_result_size(op, resnum) == 8) {
	Set_LR_byteable(clr);
      }
#endif
#ifdef TARG_ST
	else if (OP_compose(op)) {
	  // [SC} Preference compose result to an operand
	  ISA_REGISTER_SUBCLASS sc = OP_result_reg_subclass(op, resnum);
	  REGISTER_SET subclass_regs = REGISTER_SUBCLASS_members(sc);
	  for (INT i = 0; i < OP_opnds(op); i++) {
	    TN *src_tn = OP_opnd(op, i);
	    REGISTER src_reg = LRA_TN_register (src_tn);
	    // [TTh] Source TNs might be multiple-registers
	    //       (consider that source TNs all have same size)
	    INT src_offs = i * TN_nhardregs(src_tn);
	    if (!TN_is_local_reg(src_tn)
		&& src_reg != REGISTER_UNDEFINED
		&& src_reg >= (REGISTER_MIN + src_offs)
		&& REGISTER_SET_MemberP (subclass_regs, src_reg - src_offs)) {
	      Set_LR_prefer_reg(clr, (src_reg - src_offs));
	      if (Do_LRA_Trace(Trace_LRA_Detail)) {
		fprintf (TFile, "OP:%d>> preference %s to ", opnum,
			 REGISTER_name (TN_register_class(src_tn),
					LR_prefer_reg(clr)));
		Print_TN(tn,FALSE);
		fprintf (TFile, "\n");
	      }
	      break;
	    }
	  }
	} else if (OP_extract(op)) {
	  // [SC] Preference extract result to the operand
	  TN *src_tn = OP_opnd(op, 0);
	  REGISTER src_reg = LRA_TN_register (src_tn);
	  // [TTh] Extracted TN might be multiple-registers
	  //       (consider that extracted TNs all have same size)
	  INT res_offs = resnum * TN_nhardregs(tn);
	  if (!TN_is_local_reg(src_tn)
	      && src_reg != REGISTER_UNDEFINED
	      && res_offs < TN_nhardregs(src_tn)) {
	    Set_LR_prefer_reg(clr, (src_reg + res_offs));
	    if (Do_LRA_Trace(Trace_LRA_Detail)) {
	      fprintf (TFile, "OP:%d>> preference %s to ", opnum,
		       REGISTER_name (TN_register_class (src_tn),
				      LR_prefer_reg(clr)));
	      Print_TN(tn,FALSE);
	      fprintf (TFile, "\n");
	    }
	  }
	}
#endif
    }
  }
#ifdef TARG_ST
  if (LRA_overlap_coalescing) {
    // Initialize array of widest local TN found in current BB
    ISA_REGISTER_CLASS rc;
    FOR_ALL_ISA_REGISTER_CLASS(rc) {
      regclass_widest_local_TN[rc] = 0;
    }
  }

  // [SC] Now that all the live ranges are created, form their
  // alias lists.
  for (LIVE_RANGE *lr1 = Live_Range_List; lr1 != NULL; lr1 = LR_next(lr1)) {
    TN *tn1 = LR_tn (lr1);
    if (! TN_is_local_reg (tn1)) {
      LIVE_RANGE *lr1_prev = lr1;
      while (LR_next_alias (lr1_prev) != lr1) {
	lr1_prev = LR_next_alias (lr1_prev);
      }
      for (LIVE_RANGE *lr2 = LR_next(lr1); lr2 != NULL; lr2 = LR_next(lr2)) {
	TN *tn2 = LR_tn (lr2);
	if (lr1 != lr2
	    && TNs_alias_p (tn1, tn2)) {
	  // lr1 and lr2 are aliases.  Merge their alias lists.
	  // These are circular lists, so we are
	  // merging two circular lists.
	  LIVE_RANGE *lr2_next = LR_next_alias (lr2);
	  Set_LR_next_alias (lr2, lr1);
	  Set_LR_next_alias (lr1_prev, lr2_next);
	  lr1_prev = lr2;
	  if (Do_LRA_Trace(Trace_LRA_Spill)) {
	    fprintf (TFile, "GTN%d and GTN%d alias\nAlias set:", TN_number(tn1),
		     TN_number(tn2));
	    FOR_LR_AND_ALL_ALIASES(lr1, alias) {
	      fprintf (TFile, " GTN%d", TN_number(LR_tn(alias)));
	    } FOR_LR_AND_ALL_ALIASES_END (lr1, alias);
	    fprintf (TFile, "\n");
	  }
	}	  
      }
    }
    else if (LRA_overlap_coalescing) {
      // [TTh] Benefit of the current loop to find the widest local TN
      // for each register class.
      INT nregs = TN_nhardregs(tn1);
      ISA_REGISTER_CLASS rc = TN_register_class(tn1);
      if (nregs > regclass_widest_local_TN[rc]) {
	regclass_widest_local_TN[rc] = nregs;
      }
    }
  }
#endif

#ifdef TARG_ST
  // [TTh] Fill overlap coalescing info at end of BB
  if (LRA_overlap_coalescing && (BB_length(bb) > 0)) {
    // Define live out dedicated registers as alive
    ISA_REGISTER_CLASS cl;
    FOR_ALL_ISA_REGISTER_CLASS (cl) {
      REGISTER r;
      for (r = REGISTER_MIN; r <= REGISTER_MAX; r++) {
	if (avail_regs[cl].reg[r] == FALSE &&
	    ded_reg_last_use(cl, r) > BB_length(bb)) {
	  Set_ded_reg_ovcoal_alive_count(cl, r, 1);
	}
      }
    }

    BOOL bb_call = BB_call(bb);
    for (LIVE_RANGE *lr1 = Live_Range_List; lr1 != NULL; lr1 = LR_next(lr1)) {
      TN *tn1 = LR_tn (lr1);
      // Cleanup overlap coalescing tree
      if ((!TN_is_local_reg(tn1)) &&
	  (LR_ovcoal_parent(lr1) || LR_ovcoal_child(lr1))) {
	if (LR_next_alias(lr1) != lr1 || LR_early_clobber(lr1)) {
	  // Forbid overlap coalescing with aliased nodes
	  if (Do_LRA_Trace(Trace_LRA_Detail)) {
	    fprintf(TFile, OVCOAL_MSG" Detach GTN%d from its tree (aliased or early clobbered)\n", TN_number(tn1));
	  }
	  Detach_LR_ovcoal(lr1);
	}
	else if ((LR_last_use(lr1) > BB_length(bb)) ||
		 (bb_call && 
		  LRA_TN_register(tn1) != REGISTER_UNDEFINED &&
		  REGISTER_SET_MemberP(REGISTER_CLASS_caller_saves(TN_register_class(tn1)), LRA_TN_register(tn1)))) {
	  // The GTN is either live out or potentially used by the call
	  Update_Overlap_Coalescing_Info_For_Use(lr1);
	}
      }
    }

    // Propagate preference along the coalescing tree
    for (LIVE_RANGE *lr1 = Live_Range_List; lr1 != NULL; lr1 = LR_next(lr1)) {
      if (LRA_TN_register(LR_tn(lr1)) != REGISTER_UNDEFINED) {
	Propagate_Overlap_Coalescing_Preference_Leaf_To_Root(lr1, LRA_TN_register(LR_tn(lr1)));
      }
    }

    // Set overlap coalescing class for live out dedicated register
    for (LIVE_RANGE *lr1 = Live_Range_List; lr1 != NULL; lr1 = LR_next(lr1)) {
      TN *tn1 = LR_tn (lr1);
      if ((!TN_is_local_reg(tn1)) && LR_last_use(lr1) > BB_length(bb)) {
	// [TTh] Dealing with a Global TN, that is live out of the BB
	Set_ded_reg_ovcoal_root(TN_register_class(tn1),
				LRA_TN_register(tn1),
				LR_ovcoal_root(lr1),
				LR_ovcoal_rank(lr1));
      }
    }
  }
#endif

}


static BOOL
Is_OP_Spill_Load (OP *op, ST *spill_loc)
{
  if (!OP_load(op)) return FALSE;

#ifdef KEY
  // 14329: Filter out indexed loads
  if (TOP_Find_Operand_Use(OP_code(op), OU_index) >= 0)
    return FALSE;
#endif

  INT n = TOP_Find_Operand_Use(OP_code(op), OU_offset);
  if (n < 0) {
    return (TN_spill(OP_result(op,0)) == spill_loc);
  }

  TN *ctn = OP_opnd(op, n);
  return (TN_is_constant(ctn) && 
          TN_is_symbol(ctn) &&
          TN_var(ctn) == spill_loc);
}

static BOOL
Is_OP_Spill_Store (OP *op, ST *spill_loc)
{
  if (!OP_store(op)) return FALSE;

#ifdef KEY
  // 14329: Filter out indexed stores
  if (TOP_Find_Operand_Use(OP_code(op), OU_index) >= 0)
    return FALSE;
#endif

  INT n = TOP_Find_Operand_Use(OP_code(op), OU_offset);
  if (n < 0) {
    n = TOP_Find_Operand_Use(OP_code(op), OU_storeval);
    if (n < 0) return FALSE;
    return (TN_spill(OP_opnd(op,n)) == spill_loc);
  }

  TN *ctn = OP_opnd(op, n);
  return (TN_is_constant(ctn) && 
          TN_is_symbol(ctn) &&
          TN_var(ctn) == spill_loc);
}


#ifdef TARG_X8664
// Return TRUE if OP is a load from read-only memory.
static BOOL
Is_Readonly_Mem_Load (OP *op)
{
  int i;

  // Example:	TN894 :- ldss GTN34(%rip) (sym:.rodata+4)
  for (i = 0; i < OP_opnds(op); i++) {
    TN *opnd_tn = OP_opnd(op,i);
    if (TN_is_register(opnd_tn) &&		// (%rip)
	TN_register_class(opnd_tn) == ISA_REGISTER_CLASS_rip)
      continue;
    if (TN_is_symbol(opnd_tn)) {
      ST *var = TN_var(opnd_tn);
      if (!strcmp(ST_name(var), ".rodata"))	// (sym:.rodata+offset)
	continue;
    }
    return FALSE;	// OP is not load from read-only memory
  }
  return TRUE;		// OP is load from read-only memory
}

// A faster version of Mark_Reloadable_Live_Ranges that doesn't check the
// dependence graph (building the dep graph is expensive).  Make a LR
// reloadable only if its define OP is a load from read-only memory.
static void
Quick_Mark_Reloadable_Live_Ranges (ISA_REGISTER_CLASS cl)
{

#ifdef TARG_ST
  // Disable Reloading for spilling with -CG:LRA_mask=0x1000.
  if (CG_LRA_mask & 0x1000) return;
#else
  // Disable Reloading for spilling with -Wb,-ttlra:0x1000.
  if (Get_Trace (TP_ALLOC, 0x1000)) return;
#endif
  for (LIVE_RANGE *lr = Live_Range_List; lr != NULL; lr = LR_next(lr)) {
    // Look for live ranges with a single definition and no exposed uses.
    if (LR_exposed_use(lr) || LR_def_cnt(lr) != 1) continue;

#ifdef TARG_ST
    // Do not attempt to reload live ranges with aliases.
    if (LR_next_alias(lr) != lr) continue;
#endif

    OP *def_op = OP_VECTOR_element (Insts_Vector, LR_first_def(lr));
    // For now, restrict ourselves to considering only non-volatile loads.
    if (!OP_load(def_op) || OP_has_implicit_interactions(def_op))
      continue;

#ifdef TARG_ST
    // [CG] Multiple result load cannot be safely used for reloading
    // a live range. I suppose that the original code did not
    // filter this to handle loads with flag register settings that
    // are safe to duplicate. Though in this case this property
    // should be explicitly tested. For now disable multiple register
    // load duplication.
    if (OP_results(def_op) > 1)
      continue;      
#endif

    TN *result_tn;
    INT i;
    for (i = OP_results(def_op) - 1; i >= 0; --i) {
      if (OP_result(def_op, i) == LR_tn(lr)) {
	result_tn = OP_result(def_op, i);
      }
    }
    if (TN_register_class(result_tn) != cl) continue;

    if (! TN_is_rematerializable(result_tn) && ! TN_is_gra_homeable(result_tn))
      continue;

    // Restrict to read-only memory loads for now.  Relax restriction as
    // needed.
    if (!Is_Readonly_Mem_Load(def_op))
      continue;

    Set_LR_reloadable (lr);
  }
}
#endif


static void
Mark_Reloadable_Live_Ranges (ISA_REGISTER_CLASS cl)
{
  // Disable Reloading for spilling with -Wb,-ttlra:0x1000.
  if (Get_Trace (TP_ALLOC, 0x1000)) return;

#ifdef TARG_X8664
  Need_Dep_Graph_For_Mark_Reloadable_Live_Ranges = FALSE;
#endif

  for (LIVE_RANGE *lr = Live_Range_List; lr != NULL; lr = LR_next(lr)) 
  {
    // Look for live ranges with a single definition and no exposed uses.
    if (LR_exposed_use(lr) || LR_def_cnt(lr) != 1) continue;

    OP *def_op = OP_VECTOR_element (Insts_Vector, LR_first_def(lr));
    // For now, restrict ourselves to considering only non-volatile loads.
    if (!OP_load(def_op) || OP_has_implicit_interactions(def_op))
      continue;

    TN *result_tn;
    INT i;
    for (i = OP_results(def_op) - 1; i >= 0; --i) {
      if (OP_result(def_op, i) == LR_tn(lr)) {
	result_tn = OP_result(def_op, i);
      }
    }
    if (TN_register_class(result_tn) != cl) continue;

#ifdef KEY
    if (! TN_is_rematerializable(result_tn) && ! TN_is_gra_homeable(result_tn))
      continue;
#endif

    BOOL reloadable = TRUE;
    for (i = 0; i < OP_opnds(def_op); i++) {
      TN *opnd_tn = OP_opnd(def_op,i);
      if (TN_is_register(opnd_tn) && TN_register_class(opnd_tn) == cl) {
        LIVE_RANGE *opnd_lr = LR_For_TN (opnd_tn);
        if (LR_last_use(opnd_lr) < LR_last_use(lr)) {
          reloadable = FALSE;
          break;
        }
      }
    }
    if (!reloadable) continue;

    ARC_LIST *arcs;
    for (arcs = OP_succs(def_op); arcs != NULL; arcs = ARC_LIST_rest(arcs)) {
      ARC *arc = ARC_LIST_first(arcs);
      if (ARC_kind(arc) == CG_DEP_REGIN) continue;
      OP *succ_op = ARC_succ(arc);
      INT succ_opnum = BB_OP_MAP32_Get (op_to_opnum, succ_op);
      if (succ_opnum < LR_last_use(lr)) {
        reloadable = FALSE;
        break;
      }
    }

    if (reloadable) {
      Set_LR_reloadable (lr);
#ifdef TARG_X8664
      if (!Is_Readonly_Mem_Load(def_op))
	Need_Dep_Graph_For_Mark_Reloadable_Live_Ranges = TRUE;
#endif
    }
#ifdef TARG_ST
    if (reloadable && Do_LRA_Trace(Trace_LRA_Detail)) {
      fprintf (TFile, "%s_LR>TN%d is reloadable\n",
	       TN_is_local_reg(LR_tn(lr)) ? "LOCAL" : "GLOBAL",
	       TN_number(LR_tn(lr)));
    }
#endif
  }
}


// A live-range that has the LR_reloadable attribute might no longer be
// reloadable.  We have to check if there is a store to the spill location 
// corresponding to any of the operands of the defining op before the last 
// use. If there is such a spill-store, reset the LR_reloadable attribute.
static BOOL Is_LR_Reloadable (LIVE_RANGE *lr)
{
  if (!LR_reloadable(lr)) return FALSE;

  OP *def_op = OP_VECTOR_element (Insts_Vector, LR_first_def(lr));

  for (INT i = 0; i < OP_opnds(def_op); i++) {
    TN *opnd_tn = OP_opnd(def_op,i);
    ST *spill_loc;
    if (TN_is_register(opnd_tn) &&
        !TN_is_rematerializable (opnd_tn) &&
	(spill_loc = TN_spill(opnd_tn)) != NULL) 
    {
      OP *last_use = (LR_last_use(lr) < VECTOR_size(Insts_Vector)) ?
			OP_VECTOR_element (Insts_Vector, LR_last_use(lr)) : 
			NULL;

      for (OP *op = OP_next(def_op); op != last_use; op = OP_next(op)) {
	if (Is_OP_Spill_Store (op, spill_loc)) {
	  Reset_LR_reloadable (lr);
	  return FALSE;
	}
      }
    }
  }
  return TRUE;
}


/* Remove self copies that might have been left behind, and noop
 * ops left as place holders for redundant assignments
 */
static void 
Remove_Redundant_Code (BB *bb)
{
  OP *op = BB_first_op (bb);
  do {
    OP *next_op = OP_next(op);
    if ((CGTARG_Is_Preference_Copy(op) &&
#ifdef TARG_ST
	 // Arthur: we need to consider the COPYA,COPYD, etc. as
	 //         target copies. It seems that IA64 does not
	 //         consider int->fp moves as a copy ... why ?
	 //
	 //         Thus, not only equivalence of TN_register matters
	 //         but also its register class.
	 //
	 LRA_TN_register(OP_result(op,OP_Copy_Result(op))) ==
	 LRA_TN_register(OP_opnd(op, OP_Copy_Operand(op))) &&

	 TN_register_class(OP_result(op,OP_Copy_Result(op))) ==
	 TN_register_class(OP_opnd(op, OP_Copy_Operand(op)))) ||
#else
	 LRA_TN_register(OP_result(op,0)) ==
	 LRA_TN_register(OP_opnd(op,CGTARG_Copy_Operand(op)))) ||
#endif
	Is_Marked_For_Removal(op)) {
#ifdef TARG_ST
      // CL: hope that this won't affect next passes.
      // When allocating a dedicated register to a tn,
      // mark that tn as dedicated too.
      // This is to fix an issue before a call
      // to a function with one arg being an unnamed const
      // and where we miss the dependency because the TN
      // which holds that arg is not marked as dedicated.
    if (OP_Is_Preference_Copy(op) && TN_is_dedicated(OP_result(op,OP_Copy_Result(op)))) {
      Set_TN_is_dedicated(OP_opnd(op, OP_Copy_Operand(op)));
    }
#endif
      BB_Remove_Op (bb, op);
      Reset_BB_scheduled (bb);
    }
    op = next_op;
  } while (op != NULL);
}


/******************** Routines to process avail_regs **********************/

/* ======================================================================
 * Init_Avail_Regs
 *
 * Initialize the avail_regs from the avail_set.
#ifdef KEY
 * Also initialize the last_freed data structure.
#endif
 * ======================================================================*/
static void
Init_Avail_Regs (void)
{
  REGISTER reg;
  ISA_REGISTER_CLASS cl;

  memset(avail_regs, 0, sizeof(avail_regs));
#ifdef KEY
  memset(last_freed, 0, sizeof(last_freed));
#endif

  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    FOR_ALL_REGISTER_SET_members (avail_set[cl], reg) {
      avail_regs[cl].reg[reg] = TRUE;
#ifdef KEY
      last_freed[cl].reg[reg] = INT_MAX;
#endif
    }
  }
}


/* ======================================================================
 * Add_Avail_Reg
 *
 * Add the register (regclass,reg) back to the avail_regs list.
 *
#ifdef TARG_ST
 * [TTh] When overlap coalescing is enabled, several TNs can be allocated
 * at the same time to a given dedicated register (they belong to the same
 * coalescing tree). A use count is then maintained and the dedicated
 * register is effectively freed when there is no more user of it.
#endif
 * ======================================================================*/
#ifdef TARG_ST
  static void
Add_Avail_Reg (ISA_REGISTER_CLASS regclass, REGISTER reg, INT nregs, INT cur_op)
{
  TN *tn = Build_Dedicated_TN (regclass, reg, 0);
  REGISTER r;
  FOR_ALL_NREGS(reg, nregs, r) {
    // are we establishing fat points?
    if (Calculating_Fat_Points()) {
      if (fat_points[cur_op] != FAT_POINTS_MIN) fat_points[cur_op]--;
      //
      // infinite register set used when establishing fat points.
      //
      if (r > REGISTER_MAX) continue;
    }

    /* don't put non-allocatable registers back on the free list. */
    /* I assume that if the first register is non-allocatable, then
       all nregs are non-allocatable, and so return here.
       This helps with the use of register pair ( $0, $1 ) to represent
       zero, where $1 is not actually used. */
    if (!REGISTER_allocatable(regclass, r)) return;

    /* register excluded (by swp) */
    if (REGISTER_SET_MemberP(exclude_set[regclass], r)) continue;

    // For Zero TN, only first register is considered
    if (r != reg && TN_is_zero_reg(tn)) continue;

    if (Do_LRA_Trace(Trace_LRA_Detail)) {
      fprintf (TFile, "Deallocated register %s\n", REGISTER_name(regclass, r));
    }
    Is_True (!avail_regs[regclass].reg[r], (" LRA: Error in Add_Avail_Reg"));
    if (LRA_overlap_coalescing && !TN_is_zero_reg(tn)) {
      // Update overlap coalescing info and effectively free the
      // register if no more alive.
      Decrement_ded_reg_ovcoal_alive_count(regclass, r);
      if (ded_reg_ovcoal_alive_count(regclass, r) <= 0 ) {
	avail_regs[regclass].reg[r] = TRUE;
      }
    }
    else {
      avail_regs[regclass].reg[r] = TRUE;
    }
  }
}
#else
static void
Add_Avail_Reg (ISA_REGISTER_CLASS regclass, REGISTER reg, INT cur_op)
{
  // are we establishing fat points?
  if (Calculating_Fat_Points()) {
    if (fat_points[cur_op] != FAT_POINTS_MIN) fat_points[cur_op]--;
    //
    // infinite register set used when establishing fat points.
    //
    if (reg > REGISTER_MAX) return;
  }

  /* don't put non-allocatable registers back on the free list. */
  if (!REGISTER_allocatable(regclass, reg)) return;

  /* register excluded (by swp) */
  if (REGISTER_SET_MemberP(exclude_set[regclass], reg)) return;

  if (Do_LRA_Trace(Trace_LRA_Detail)) {
    fprintf (TFile, "Deallocated register %s\n", REGISTER_name(regclass, reg));
  }
  Is_True (!avail_regs[regclass].reg[reg], (" LRA: Error in Add_Avail_Reg"));
  avail_regs[regclass].reg[reg] = TRUE;
#ifdef KEY
#ifdef TARG_MIPS
  last_assigned_reg[regclass] = reg;
#else	// This should be removed.  See check-in comment on 3/19/2008.
  if( reg > last_assigned_reg[regclass] ||
      last_assigned_reg[regclass] == REGISTER_MAX ){
    last_assigned_reg[regclass] = reg;
  }
#endif

  // Keep track of when the reg was last assigned.
  last_freed[regclass].reg[reg] = cur_op;
#endif
}
#endif


/* ======================================================================
 * Delete_Avail_Reg
 *
 * Delete the register <regclass, reg> from the avail_regs list. 
 *
#ifdef TARG_ST
 * [TTh] When overlap coalescing is enabled, the same dedicated registers
 * can be allocated several times as long as all allocated TNs belongs to
 * the same overlap coalescing tree.
#endif
 * ======================================================================*/
#ifdef TARG_ST
inline void
Delete_Avail_Reg (ISA_REGISTER_CLASS regclass, REGISTER reg, INT nregs,
		  INT cur_op, LIVE_RANGE* lr)
{
  TN *tn = Build_Dedicated_TN (regclass, reg, 0);
  // are we establishing fat points?
  if (Calculating_Fat_Points()) {
    if (fat_points[cur_op] != FAT_POINTS_MAX) fat_points[cur_op]+=nregs;
    //
    // infinite register set used when establishing fat points.
    //
    if (reg > REGISTER_MAX) return;
  }
  REGISTER r;
  FOR_ALL_NREGS(reg, nregs, r) {
    if (TN_is_zero_reg(tn)) {
      // Specific handling for Zero TN:
      // - No overlap coalescing sanity check performed, as LR with different
      //   overlap coalescing classes might be allocated to it, following
      //   optimistic coalescing done in Is_Non_Allocatable_Reg_Available()
      // - Only first register is considered
      if (r != reg) continue;
    }
    else if (LRA_overlap_coalescing) {
      if (ded_reg_ovcoal_alive_count(regclass, r)>0) {
	// Current register not available: Check that it is allocated to the
	// same overlap coalescing class.
	FmtAssert((ded_reg_ovcoal_root(regclass, r)==LR_ovcoal_root(lr) &&
		   ded_reg_ovcoal_rank(regclass, r)==LR_ovcoal_rank(lr)+(r-reg)),
		  ("Delete_Avail_Reg(): Register '%s' already used by an incompatible overlap coalescing class!\n"
		   "\tAllocated class: "OVCOAL_BASE_NAME"%d (rank:%d) use_count:%d\n"
		   "\tNew       class: "OVCOAL_BASE_NAME"%d (rank:%d) used by TN%d\n",
		   REGISTER_name(regclass, r),
		   ded_reg_ovcoal_root(regclass, r)?TN_number(LR_tn(ded_reg_ovcoal_root(regclass, r))):0,
		   ded_reg_ovcoal_rank(regclass, r),
		   ded_reg_ovcoal_alive_count(regclass, r),
		   LR_ovcoal_root(lr)?TN_number(LR_tn(LR_ovcoal_root(lr))):0,
		   LR_ovcoal_rank(lr)+(r-reg),
		   TN_number(LR_tn(lr))));
	Increment_ded_reg_ovcoal_alive_count(regclass, r);
      }
      else {
	// Current register available: Associate it to current LR
	// overlap coalescing class.
	Set_ded_reg_ovcoal_root(regclass, r, LR_ovcoal_root(lr),
				LR_ovcoal_rank(lr)+(r-reg));
	Set_ded_reg_ovcoal_alive_count(regclass, r, 1);
      }
    }
    if (Do_LRA_Trace(Trace_LRA_Detail)) {
      fprintf (TFile, "Allocated register %s\n", REGISTER_name(regclass, r));
    }
    avail_regs[regclass].reg[r] = FALSE;
  }
}
#else
inline void
Delete_Avail_Reg (ISA_REGISTER_CLASS regclass, REGISTER reg, INT cur_op)
{
  // are we establishing fat points?
  if (Calculating_Fat_Points()) {
    if (fat_points[cur_op] != FAT_POINTS_MAX) fat_points[cur_op]++;
    //
    // infinite register set used when establishing fat points.
    //
    if (reg > REGISTER_MAX) return;
  }
  avail_regs[regclass].reg[reg] = FALSE;
}
#endif

#ifdef TARG_ST
// [TTh] When overlap coalescing is enabled, registers are seen as available if
// they are already used by TN belonging to the same overlap coalescing tree and
// with compatible rank.
static BOOL Regs_Available (ISA_REGISTER_CLASS rclass, REGISTER reg, INT nregs, LIVE_RANGE *lr)
{
  INT rank = LR_ovcoal_rank(lr);
  REGISTER r;
  FOR_ALL_NREGS(reg, nregs, r) {
    if (!avail_regs[rclass].reg[r] &&
	(ded_reg_ovcoal_root(rclass, r) == OVCOAL_ROOT_UNDEF ||
	 ded_reg_ovcoal_root(rclass, r) != LR_ovcoal_root(lr) ||
	 ded_reg_ovcoal_rank(rclass, r) != rank)) {
      return FALSE;
    }
    rank++;
  }
  return TRUE;
}
#endif


/* ======================================================================
 * Is_Reg_Available
 *
 * Return TRUE if <regclass,reg> is available for allocation to <lr>.
 * In addition to checking the avail_regs array for availability, one
 * of the following conditions must be true:
 *   1. the register has not been allocated to a global TN in this block. 
 *   2. the first definition of <lr> is after the exposed use of <reg>.
 *   3. the first definition of <lr> is the same as the exposed use of <lr>
 *      and the definition is not an OP_uniq_res.
 * ======================================================================*/
#ifdef TARG_ST
static BOOL
Is_Reg_Available (ISA_REGISTER_CLASS regclass, 
                  REGISTER_SET usable_regs, 
                  REGISTER reg, 
		  INT nregs,
                  LIVE_RANGE *lr)
{
  //
  // infinite register set used when establishing fat points.
  //
  if (reg > REGISTER_MAX) {
    //
    // we should only get here if we're trying to use the result
    // register for one of the operands.  if so, then it should
    // have just been freed if its available.  otherwise, we don't
    // reuse the register numbers beyond the physical register set
    // (i.e. the "infinite" ones).  as a result, we don't really
    // keep track of their availability.
    //
    if (last_infinite_freed == reg) {
      return TRUE;
    } else {
      return FALSE;
    }
  }

  if (reg != REGISTER_UNDEFINED && 
      Regs_Available(regclass, reg, nregs, lr) &&
      REGISTER_SET_MemberP(usable_regs, reg)) {
    OP *op;
    INT lr_def; 

    //
    // for undefined locals, treat exposed use as first def since we
    // want to free the register immediately after the exposed use.
    //
    if (LR_Is_Undefined_Local(lr)) {
      lr_def = LR_exposed_use(lr);
    } else {
      lr_def = LR_effective_first_def(lr);
    }
    REGISTER r;
    FOR_ALL_NREGS (reg, nregs, r) {
      if (!(lr_def > ded_reg_exposed_use(regclass, r) ||
	    lr_def == ded_reg_exposed_use(regclass, r) &&
	    (op = OP_VECTOR_element (Insts_Vector, LR_first_def(lr))) &&
	    (OP_results(op) > 0 && !OP_uniq_res(op,0)))) {
	return FALSE;
      }
    }
    return TRUE;
  }
  return FALSE;
}
#else
static BOOL
Is_Reg_Available (ISA_REGISTER_CLASS regclass, 
                  REGISTER_SET usable_regs, 
                  REGISTER reg, 
                  LIVE_RANGE *lr)
{
  //
  // infinite register set used when establishing fat points.
  //
  if (reg > REGISTER_MAX) {
    //
    // we should only get here if we're trying to use the result
    // register for one of the operands.  if so, then it should
    // have just been freed if its available.  otherwise, we don't
    // reuse the register numbers beyond the physical register set
    // (i.e. the "infinite" ones).  as a result, we don't really
    // keep track of their availability.
    //
    if (last_infinite_freed == reg) {
      return TRUE;
    } else {
      return FALSE;
    }
  }
  if (reg != REGISTER_UNDEFINED && 
      avail_regs[regclass].reg[reg] &&
      REGISTER_SET_MemberP(usable_regs, reg)) {
    TN *tn = Build_Dedicated_TN (regclass, reg, 0);
    LIVE_RANGE *ded_lr = LR_For_TN (tn);
    OP *op;
    INT lr_def; 

    //
    // for undefined locals, treat exposed use as first def since we
    // want to free the register immediately after the exposed use.
    //
    if (LR_Is_Undefined_Local(lr)) {
      lr_def = LR_exposed_use(lr);
    } else {
      lr_def = LR_first_def(lr);
    }
    if (ded_lr == NULL ||
	lr_def > LR_exposed_use(ded_lr) ||
	(lr_def == LR_exposed_use(ded_lr) && 
         (op = OP_VECTOR_element (Insts_Vector, LR_first_def(lr))) &&
	  !OP_uniq_res(op)))
    {
      return TRUE;
    }
  }
  return FALSE;
}
#endif


/* If the <reg> is non-allocatable, we can use it for the live-range <lr>,
 * as long as it is safe to do so.
 */
#ifdef TARG_ST
static BOOL
Is_Non_Allocatable_Reg_Available (
  ISA_REGISTER_CLASS regclass, 
  REGISTER_SET usable_regs,
  REGISTER reg,
  INT nregs,
  LIVE_RANGE *lr)
{
  if (! REGISTER_SET_MemberP(usable_regs, reg)) {
    return FALSE;
  }

  TN *tn = Build_Dedicated_TN (regclass, reg, 0);

  if (TN_is_zero_reg(tn)) {
    /* For the special case of a single definition of the live range
     * which is a copy from $0, we can assign $0 to the live range.
     */
    if (LR_def_cnt(lr) == 1) {
      OP *op = OP_VECTOR_element (Insts_Vector, LR_first_def(lr));
      if (OP_Is_Preference_Copy(op) &&
	  LRA_TN_register(OP_opnd(op, OP_Copy_Operand(op))) == reg)
        return TRUE;
      else if (OP_compose(op)) {
	// [SC] Allow compose ops where all the opnds are reg.
	//  e.g.   tn :- compose $0, $0
	// can allocate $0 to tn.
	INT opnd;
	for (opnd = 0; opnd < OP_opnds(op); opnd++) {
	  if (LRA_TN_register(OP_opnd(op, opnd)) != reg) {
	    return FALSE;
	  }
	}
	return TRUE;
      }
    }
    return FALSE;
  }

  INT rank = LR_ovcoal_rank(lr);
  REGISTER r;
  FOR_ALL_NREGS (reg, nregs, r) {
    if (REGISTER_allocatable (regclass, r) ||
	LR_effective_first_def(lr) < ded_reg_exposed_use(regclass, r) ||
	LR_last_use(lr) > ded_reg_first_def(regclass, r)
	// [TTh] Disable opportunistic coalescing if non-compatible
	// overlap coalescing class
	|| LR_ovcoal_root(lr) == NULL
	|| LR_ovcoal_root(lr) != ded_reg_ovcoal_root(regclass, r)
	|| rank != ded_reg_ovcoal_rank(regclass, r)) {
      return FALSE;
    }
    rank++;
  }
  /* extend the exposed use of the non-allocatable TN */
  FOR_ALL_NREGS (reg, nregs, r) {
    if (ded_reg_exposed_use(regclass, r) < LR_last_use(lr)) {
      Set_ded_reg_exposed_use(regclass, r, LR_last_use(lr));
    }
  }
  return TRUE;
}
#else
static BOOL
Is_Non_Allocatable_Reg_Available (
  ISA_REGISTER_CLASS regclass, 
  REGISTER_SET usable_regs,
  REGISTER reg,
  LIVE_RANGE *lr)
{
  if (! REGISTER_SET_MemberP(usable_regs, reg)) {
    return FALSE;
  }

  TN *tn = Build_Dedicated_TN (regclass, reg, 0);

  if (TN_is_zero_reg(tn)) {
    /* For the special case of a single definition of the live range
     * which is a copy from $0, we can assign $0 to the live range.
     */
    if (LR_def_cnt(lr) == 1) {
      OP *op = OP_VECTOR_element (Insts_Vector, LR_first_def(lr));
      if (CGTARG_Is_Preference_Copy(op) &&
	  LRA_TN_register(OP_opnd(op, CGTARG_Copy_Operand(op))) == reg)
        return TRUE;
    }
    return FALSE;
  }

  LIVE_RANGE *ded_lr = LR_For_TN (tn);

  if (!REGISTER_allocatable (regclass, reg) &&
      ded_lr != NULL &&
      LR_first_def(lr) >= LR_exposed_use(ded_lr) &&
      LR_last_use(lr) <= LR_first_def(ded_lr))
  {
    /* extend the exposed use of the non-allocatable TN */
    LR_exposed_use(ded_lr) = LR_last_use(lr);
    return TRUE;
  }
  return FALSE;
}
#endif

#ifdef TARG_ST
/*
 * Return TRUE if assigning <reg> to <tn> is optimal, return FALSE otherwise.
 * If the returned value is FALSE, <suboptimal_coalescing> parameter is set
 * depending on wether or not this allocation can impact coalescing.
 *
 * Details:
 * Considering that current register class is divided in N multi registers
 * of size S, and considering only the multi-register MR containing <reg>,
 * return TRUE if <tn> either fully occupy MR register or if at least one part
 * of MR that will not be used by <tn> is already allocated to another TN,
 * that will never need to be coalesced with <reg>.
 * Return FALSE otherwise.
 * The size S of multi-registers corresponds to the widest local TN of this
 * register class in current BB.
 * <suboptimal_coalescing> flag is set to TRUE if choosing this register
 * might lead to suboptimal coalescing for some other TNs.
 *
 * Notes: - This function is context sensitive and must therefore only be
 *          called during the allocation walk (dependency on avail_regs array).
 *        - Rely on the fact that registers are grouped uniformly for
 *          all registers of the register class.
 */
static BOOL
Is_Optimal_Fill_Register_For_TN(TN *tn, REGISTER reg, BOOL *suboptimal_coalescing) {
  INT i;
  INT widest_tn_nregs;
  INT current_tn_nregs;

  *suboptimal_coalescing = FALSE;

  ISA_REGISTER_CLASS rc = TN_register_class(tn);

  if (TN_nhardregs(tn) >= regclass_widest_local_TN[rc]) {
    return TRUE;
  }

  widest_tn_nregs  = regclass_widest_local_TN[rc];
  current_tn_nregs = TN_nhardregs(tn);

  // Register range of current tn if allocated to reg
  INT first_reg_to_alloc = reg;
  INT last_reg_to_alloc  = reg + current_tn_nregs - 1;

  // Register range of the widest multi-register
  INT rank_mask = widest_tn_nregs - 1;
  INT first_reg_to_check = (((INT)reg-1) & ~rank_mask) + 1;
  INT last_reg_to_check  = first_reg_to_check + widest_tn_nregs - 1;

  BOOL fully_avail = TRUE;
  for (i=first_reg_to_check; i<=last_reg_to_check; i++) {
    if ((i < first_reg_to_alloc || i > last_reg_to_alloc) &&
	(avail_regs[rc].reg[i] == FALSE)) {
      fully_avail = FALSE;
      // Register <i> is already allocated to a TN: check if current
      // allocation will block coalescing for this specific TN with
      // its parent in the overlap coalescing tree.
      LIVE_RANGE *lr = ded_reg_ovcoal_root(rc, i);
      if (lr &&
	  ((i < first_reg_to_alloc &&
	    ((i + TN_nhardregs(LR_tn(lr)) - ded_reg_ovcoal_rank(rc, i) - 1) >= first_reg_to_alloc)) ||
	   (i > last_reg_to_alloc &&
	    ((i - ded_reg_ovcoal_rank(rc, i)) <= last_reg_to_alloc)))) {
	*suboptimal_coalescing = TRUE;
	return FALSE;
      }
    }
  }
  
  // Return TRUE if part of the widest register is already allocated, 
  // because choosing <reg> will contribute to fill the widest one.
  return (!fully_avail);
}
#endif

/* ======================================================================
 * Get_Avail_Reg
 *
 * Get a register of the given 'regclass' from the avail_regs list. 
 * We make sure that the register can be used for allocation to <lr>.
 * We also make sure that the register is not the same as <skip_reg>.
 * ======================================================================*/
static REGISTER
Get_Avail_Reg (ISA_REGISTER_CLASS regclass, 
#ifdef TARG_ST
	       INT nregs,
#endif
               REGISTER_SET usable_regs, 
               LIVE_RANGE *lr, 
#ifdef TARG_ST
	       REGISTER_SET skip_regs
#else
               REGISTER skip_reg
#endif
	       )
{
#ifdef TARG_ST
  // [SC]: If LRA_minregs is set, do not allocate registers in
  // round-robin fashion.  This minimizes the set of registers used
  // for LRA.
  REGISTER next_reg = (LRA_minregs ? REGISTER_MIN
                                   : (last_assigned_reg[regclass] + 1));
#else
  REGISTER next_reg = last_assigned_reg[regclass] + 1;
#endif
  REGISTER reg = REGISTER_UNDEFINED;

#ifdef KEY
  // Find the least-recently-used register.
  //
  // For x86-64, handle least-recently-used before legacy-regs because
  // least-recently-used should produce better code.
  if (LRA_prefer_lru_reg) {
    REGISTER lru_reg = REGISTER_UNDEFINED;
    INT lru_opnum;
    for (reg = REGISTER_MIN; reg <= REGISTER_MAX; reg++) {
      if (Is_Reg_Available(regclass, usable_regs, reg, lr) &&
	  reg != skip_reg) {
	if (lru_reg == REGISTER_UNDEFINED ||
	    lru_opnum < last_freed[regclass].reg[reg]) {
	  lru_reg = reg;
	  lru_opnum = last_freed[regclass].reg[reg];
	}
      }
    }
    if (lru_reg != REGISTER_UNDEFINED) {
      return lru_reg;
    }
  } else
#endif
#ifdef TARG_X8664
  if (LRA_prefer_legacy_regs) {
    // Get the next available register starting from the last_assigned_reg.
    // Divide up the registers into a low-cost set and a high-cost set.  Try to
    // get from the low-cost set first.  Use round-robin for both sets.
    const REGISTER max_preferred_reg =
      REGISTER_MIN + ((REGISTER_MAX - REGISTER_MIN + 1) / 2) - 1;
    REGISTER start1, start2, start3, end1, end2, end3;

    if (next_reg <= max_preferred_reg) {
      start1 = next_reg;
      end1 = max_preferred_reg;
      start2 = REGISTER_MIN;
      end2 = next_reg - 1;
      start3 = max_preferred_reg + 1;
      end3 = REGISTER_MAX;
    } else {
      start1 = REGISTER_MIN;
      end1 = max_preferred_reg;
      start2 = next_reg;
      end2 = REGISTER_MAX;
      start3 = max_preferred_reg + 1;
      end3 = next_reg - 1;
    }
    for (reg = start1; reg <= end1; reg++) {
      if (Is_Reg_Available(regclass, usable_regs, reg, lr) && reg != skip_reg)
        return reg;
    }
    for (reg = start2; reg <= end2; reg++) {
      if (Is_Reg_Available(regclass, usable_regs, reg, lr) && reg != skip_reg)
        return reg;
    }
    for (reg = start3; reg <= end3; reg++) {
      if (Is_Reg_Available(regclass, usable_regs, reg, lr) && reg != skip_reg)
        return reg;
    }
  } else
#endif
#ifdef TARG_ST
  // There is a possibility here to have next_reg > REGISTER_MAX
  // in this case set it to REGISTER_MIN
  if (next_reg > REGISTER_MAX) next_reg = REGISTER_MIN;

  // [TTh] When overlap coalescing is enabled, try to find a register which meet
  // the following properties:
  // - OPTIMAL RANK: the register chosen should allow to coalesce current LR
  //   with its parents in the coalescing tree, by checking that its rank is
  //   correct when seeing the register class as group of multi-registers of
  //   size S, S depending on the size of parent LR in coalescing tree.
  // - OPTIMAL FILL: considering that current register class is divided in N
  //   multi registers of size S, prefer a register that will be contiguous
  //   to an already allocated register within its multi-register.
  //   This property will help to keep full multi-registers available.
  INT ovcoal_optimal_rank=0, ovcoal_parent_size=0;
  REGISTER suboptimal_reg   = REGISTER_UNDEFINED;
  INT suboptimal_reg_status = -1;

  if (LRA_overlap_coalescing) {
    ovcoal_optimal_rank = LR_ovcoal_rank(lr);
    ovcoal_parent_size  = TN_nhardregs(LR_tn(LR_ovcoal_root(lr)));
  }
  
  // Get the best available register in the order given by gra_color.
  REGISTER_SET subclass_allowed = usable_regs;
  REGISTER_SET allowed = REGISTER_SET_Difference(REGISTER_CLASS_allocatable(regclass),
						 skip_regs);
  reg = CGCOLOR_Choose_Best_Register(regclass, nregs, subclass_allowed, allowed, next_reg);
  while (reg != REGISTER_UNDEFINED) {
    if (Is_Reg_Available(regclass, usable_regs, reg, nregs, lr)) {
      if (!LRA_overlap_coalescing) {
	break;
      }
      else {
	BOOL subopt_coal = FALSE;
	BOOL is_opt_rank = REGISTER_Has_Expected_Rank(reg, ovcoal_parent_size, ovcoal_optimal_rank);
	BOOL is_opt_fill = Is_Optimal_Fill_Register_For_TN(LR_tn(lr), reg, &subopt_coal);

	if (is_opt_rank && is_opt_fill) {
	  break; // Optimal reg found
	}
	else {
	  // Compute score for current register and keep the best one
	  INT new_status;
	  new_status  = is_opt_rank ? 4 : 0;
	  new_status |= is_opt_fill ? 2 : 0;
	  new_status |= subopt_coal ? 0 : 1;
	  if (new_status > suboptimal_reg_status) {
	    suboptimal_reg = reg;
	    suboptimal_reg_status = new_status;
	  }
	}
      }
    }
    reg = CGCOLOR_Choose_Next_Best_Register();
  }

  if (reg == REGISTER_UNDEFINED) {
    // No optimal register found for coalescing, but maybe a suboptimal
    // one is available.
    reg = suboptimal_reg;
  }
  // If we found it, allocate it
  if (reg != REGISTER_UNDEFINED) goto available;

#else /* TARG_ST */
  {
    // Get the next available register starting from the last_assigned_reg.
    // This gets registers in a round-robin fashion, which is better for the
    // scheduling pass after LRA, since it reduces dependences created due to
    // register assignment. The following 2 loops get us the circular traversal
    // through the registers.
    for (reg = next_reg; reg < REGISTER_MAX+1; reg++) {
#ifdef TARG_ST
    if (Is_Reg_Available(regclass, usable_regs, reg, nregs, lr) &&
	! REGISTER_SET_IntersectsP (REGISTER_SET_Range (reg, reg + nregs - 1),
				    skip_regs)) {
      // Available in reg
      goto available;
    }
#else
      if (Is_Reg_Available(regclass, usable_regs, reg, lr) && reg != skip_reg) {
        return reg;
      }
#endif
    }
    for (reg = REGISTER_MIN; reg < next_reg; reg++) {
#ifdef TARG_ST
    if (Is_Reg_Available(regclass, usable_regs, reg, nregs, lr) &&
	! REGISTER_SET_IntersectsP (REGISTER_SET_Range (reg, reg + nregs - 1),
				    skip_regs)) {
      // Available in reg
      goto available;
    }
#else
      if (Is_Reg_Available(regclass, usable_regs, reg, lr) && reg != skip_reg) {
        return reg;
      }
#endif
    }
  }
#endif /* !TARG_ST */

#ifdef HAS_STACKED_REGISTERS
  //
  // Try a register stack register
 #ifdef TARG_ST
  // Only try a stacked register if nregs == 1, because we have no way
  // to request nregs consecutive registers.
  //
  if (nregs == 1) {
    reg = REGISTER_Request_Stacked_Register(ABI_PROPERTY_stacked, regclass);
    if (reg != REGISTER_UNDEFINED && REGISTER_SET_MemberP(usable_regs, reg)) {
      avail_set[regclass] = REGISTER_SET_Union1(avail_set[regclass], reg);
      if (Do_LRA_Trace(Trace_LRA_Spill)) {
	fprintf (TFile, 
		 "LRA_SPILL>> Got new stack register: %s %d,%d\n", 
		 REGISTER_name(regclass, reg), regclass, reg);
      }
      avail_regs[regclass].reg[reg] = TRUE;
      livethrough_computed = FALSE; /* We have added a new live reg and need to re-compute. */
      // Available in reg
      goto available;
    }
  }
#else
  reg = REGISTER_Request_Stacked_Register(ABI_PROPERTY_stacked, regclass);
  if (reg != REGISTER_UNDEFINED && REGISTER_SET_MemberP(usable_regs, reg)) {
    avail_set[regclass] = REGISTER_SET_Union1(avail_set[regclass], reg);
    if (Do_LRA_Trace(Trace_LRA_Spill)) {
      fprintf (TFile, 
	       "LRA_SPILL>> Got new stack register: %s %d,%d\n", 
                REGISTER_name(regclass, reg), regclass, reg);
      }
    avail_regs[regclass].reg[reg] = TRUE;
    livethrough_computed = FALSE; /* We have added a new live reg and need to re-compute. */
    return reg;
  }
#endif /* !TARG_ST */
#endif  /* HAS_STACKED_REGISERS */

  /* if not running GRA, try to get additional callee save registers */
  if (CG_localize_tns) {
    REGISTER_SET unused_callees = REGISTER_SET_Difference (
                                      REGISTER_CLASS_callee_saves(regclass), 
                                      Callee_Saved_Regs_Used[regclass]);
#ifdef TARG_ST
    REGISTER_SET usable_unused_callees =
      REGISTER_SET_Intersection(unused_callees, usable_regs);
    
    reg = REGISTER_SET_Choose (usable_unused_callees);
    if (reg != REGISTER_UNDEFINED) { 
      REGISTER_SET regs = REGISTER_SET_Range (reg, reg+nregs-1);
      if (REGISTER_SET_ContainsP(unused_callees, regs)) {
        avail_set[regclass] = REGISTER_SET_Union (avail_set[regclass], regs);
        if (Do_LRA_Trace(Trace_LRA_Spill)) {
          fprintf (TFile, 
		   "LRA_SPILL>> Got new callee save register: %d,%d", regclass, reg);
          if (nregs > 1) {
            fprintf (TFile, "-%d", reg + nregs - 1);
	  }
          fprintf (TFile, "\n");
        }
        REGISTER r;
        FOR_ALL_REGISTER_SET_members (regs, r) {
          avail_regs[regclass].reg[r] = TRUE;
        }
        // Available in reg
	goto available;
      }
    }
#else
    unused_callees = REGISTER_SET_Intersection(unused_callees, usable_regs);
    reg = REGISTER_SET_Choose (unused_callees);
    if (reg != REGISTER_UNDEFINED) { 
      Callee_Saved_Regs_Used[regclass] = 
                    REGISTER_SET_Union1(Callee_Saved_Regs_Used[regclass], reg);
      avail_set[regclass] = REGISTER_SET_Union1 (avail_set[regclass], reg);
      if (Do_LRA_Trace(Trace_LRA_Spill)) {
        fprintf (TFile, 
          "LRA_SPILL>> Got new callee save register: %d,%d\n", regclass, reg);
      }
      avail_regs[regclass].reg[reg] = TRUE;
      return reg;
    }
#endif
  }
  if (Calculating_Fat_Points()) {
#ifdef TARG_ST
    reg_infinite += nregs;
    return reg_infinite;
#else
    return ++reg_infinite;
#endif
  } else {
    return REGISTER_UNDEFINED;
  }
  
#ifdef TARG_ST
  // We get here with available register (of nregs size) in reg variable
  // if we found an available register
 available:
  DevAssert(reg != REGISTER_UNDEFINED, ("Unexpected undefined register"));
  
  // Update callee save regs used. This update must be done as the
  // local register allocator may have choosen a callee saved register
  // granted by GRA.
  {
    // Inform cg_color that we actually allocate it
    CGCOLOR_Allocate_N_Registers(regclass, reg, nregs);
      
    for (INT r = reg; r < reg + nregs; r++) {
      if (REGISTER_SET_MemberP(REGISTER_CLASS_callee_saves(regclass), r)) {
	Callee_Saved_Regs_Used[regclass] = 
	  REGISTER_SET_Union1(Callee_Saved_Regs_Used[regclass], r);
      }
    }
  }
  return reg;
#endif
}

#ifdef TARG_ST
static void print_avail_regs ()
{
  REGISTER reg;
  ISA_REGISTER_CLASS cl;
  fprintf (TFile, " avail_regs:");
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    for (reg = REGISTER_MIN; reg <= REGISTER_MAX; reg++) {
      if (avail_regs[cl].reg[reg]) {
	fprintf (TFile, " %s", REGISTER_name (cl, reg));
      }
    }
  }
  fprintf (TFile, "\n");
}
#endif


/* ======================================================================
 * Allocate_Register
 *
 * Allocate a register for the TN If the argument 'uniq_result' is TRUE, 
 * exclude the register 'result_reg' from being allocated.
 * ======================================================================*/
static REGISTER
Allocate_Register (
  TN *tn, 
  BOOL uniq_result, 
  ISA_REGISTER_CLASS result_regclass,
  REGISTER_SET usable_regs,
  REGISTER result_reg,
#ifdef TARG_ST
  INT result_nregs,
#endif
  INT opnum)
{
  ISA_REGISTER_CLASS regclass;
  REGISTER reg;
#ifdef TARG_ST
  INT nregs = TN_nhardregs (tn);
#endif
  LIVE_RANGE *clr = LR_For_TN (tn);

  regclass = TN_register_class(tn);

  /* Check if the result_reg can be used as the register to allocate.
   * If yes, that should be our first preference since it minimizes 
   * the number of registers required for round-robin.
   */
  if (!uniq_result &&
      regclass == result_regclass &&
#ifdef TARG_ST
      Is_Reg_Available (regclass, usable_regs, result_reg, nregs, clr) &&
      // [TTh] With overlap coalescing enabled, uses result_reg only if is correctly
      // aligned to be coalesced with the root of its coalescing tree.
      (!LRA_overlap_coalescing ||
       REGISTER_Has_Expected_Rank(result_reg,
				  TN_nhardregs(LR_tn(LR_ovcoal_root(clr))),
				  LR_ovcoal_rank(clr))))
#else
      Is_Reg_Available (regclass, usable_regs, result_reg, clr))
#endif
  {
    reg = result_reg;
  }
  else {
#ifdef TARG_ST
    REGISTER_SET skip_regs;
#else
    REGISTER skip_reg;
#endif

    if (uniq_result && regclass == result_regclass) {
#ifdef TARG_ST
      if (result_reg == REGISTER_UNDEFINED
	  || (Calculating_Fat_Points() && result_reg > REGISTER_MAX)) {
	skip_regs = REGISTER_SET_EMPTY_SET;
      } else {
	skip_regs = REGISTER_SET_Range (result_reg, result_reg + result_nregs - 1);
      }
#else
      skip_reg = result_reg;
#endif
    }
    else {
#ifdef TARG_ST
      skip_regs = REGISTER_SET_EMPTY_SET;
#else
      skip_reg = REGISTER_UNDEFINED;
#endif
    }

#ifdef TARG_ST
    reg = Get_Avail_Reg (regclass, nregs, usable_regs, clr, skip_regs);
#else
    reg = Get_Avail_Reg (regclass, usable_regs, clr, skip_reg);
#endif
    if (reg == REGISTER_UNDEFINED) return REGISTER_UNDEFINED;
  }
#ifdef TARG_ST
  Delete_Avail_Reg (regclass, reg, nregs, opnum, clr);
#else
  Delete_Avail_Reg (regclass, reg, opnum);
#endif
  if (Do_LRA_Trace(Trace_LRA_Detail)) {
#ifdef TARG_ST
    fprintf (TFile, "Allocated register%s %s", "s"+(nregs == 1),
	     REGISTER_name (regclass, reg));

    if (nregs > 1) {
      fprintf (TFile, "-%s", REGISTER_name (regclass, reg + nregs - 1));
    }
    fprintf (TFile, " for ");
#else
    fprintf (TFile, "Allocated register %s for ", REGISTER_name (regclass, reg));
#endif
    Print_TN(tn,FALSE);fprintf(TFile,"\n");
  }
  if (Always_Trace(Trace_LRA) && trace_tn && trace_cl == regclass) {
    bb_live = TN_SET_Union1D(bb_live, tn, &lra_pool);
  }

  //
  // we only try to reuse real physical registers when calculating
  // fat points. 
  //
  if (reg <= REGISTER_MAX) {
#ifdef TARG_ST
    last_assigned_reg[regclass] = reg + nregs - 1;
#else
#ifndef KEY
    last_assigned_reg[regclass] = reg;
#endif
#endif
  }

  return reg;
}


//
// if we're not going to reschedule, set up data structures to calculate
// fat point data for the instructions above the failure point.
//
static BOOL
Init_Fat_Point_Calculation(ISA_REGISTER_CLASS cl, INT opnum, BB *bb)
{
  //
  // don't bother calculating fat points if we're just going to
  // reschedule
  //
#ifdef TARG_ST
  if (Check_Allow_Reschedule(bb)) {
    return(FALSE);
  }
#else
  if (Check_Allow_Reschedule()) {
    return(FALSE);
  }
#endif
  fat_point_regs_map = hTN_MAP_Create (&lra_pool);
  use_fat_point_regs = TRUE;
  fat_points = TYPE_MEM_POOL_ALLOC_N(FAT_POINTS_TYPE, &lra_pool, BB_length(bb) + 1);
  memset(fat_points, 0, (BB_length(bb)+1)*sizeof(FAT_POINTS_TYPE));
  failing_class = cl;
  reg_infinite = REGISTER_MAX;

  return(TRUE);
}

//
// clear out global data for fat point calculation
//
static void
Clear_Fat_Point_Calculation()
{
  fat_points = NULL;
  failing_class = ISA_REGISTER_CLASS_UNDEFINED;
  reg_infinite = REGISTER_UNDEFINED;
  use_fat_point_regs = FALSE;
}


//
// update availability data structures for used callee saved registers in
// entry and exit blocks.  we must not have any callee saved register used
// above the stack pointer adjustment in the entry block, or below it in
// the exit block (as otherwise, the use will be on the wrong side of the
// saves/restores).
//
static void
Update_Callee_Availability(BB *bb)
{
  ISA_REGISTER_CLASS cl;
  REGISTER reg;
  if (BB_exit(bb)) {
    //
    // can't allow callee saved registers to be used below stack adjustment
    // in exit block (restores are above it).
    //
#if defined(KEY) || defined(TARG_ST)
    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      avail_set[cl] = REGISTER_SET_Union(avail_set[cl],
					 Callee_Saved_Regs_Used[cl]);

      FOR_ALL_REGISTER_SET_members(Callee_Saved_Regs_Used[cl], reg) {
	avail_regs[cl].reg[reg] = TRUE;
      }
    }	  
#else
    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      avail_set[cl] = REGISTER_SET_Union(avail_set[cl],
					 Callee_Saved_Regs_Used[cl]);
    }
    FOR_ALL_REGISTER_SET_members(Callee_Saved_Regs_Used[cl], reg) {
      avail_regs[cl].reg[reg] = TRUE;
    }	  
#endif // KEY
  } else if (BB_entry(bb)) {
    //
    // can't allow callee saved registers above stack adjustment in
    // entry block (saves are below it).
    //
#if defined(KEY) || defined(TARG_ST) // bug 8300
    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      avail_set[cl] = REGISTER_SET_Difference(avail_set[cl],
					      Callee_Saved_Regs_Used[cl]);
      FOR_ALL_REGISTER_SET_members(Callee_Saved_Regs_Used[cl], reg) {
        avail_regs[cl].reg[reg] = FALSE;
      }
    }
#else
    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      avail_set[cl] = REGISTER_SET_Difference(avail_set[cl],
					      Callee_Saved_Regs_Used[cl]);
    }
    FOR_ALL_REGISTER_SET_members(Callee_Saved_Regs_Used[cl], reg) {
      avail_regs[cl].reg[reg] = FALSE;
    }	  
#endif // KEY
  }
}


// Compute the register set that satisfies target-specific constraints
// for the use of <tn> within its live range <lr>. There are two kinds
// of constraints:
// 
// 1) If <tn> is used as an operand or a result from a particular
//    register subclass, then it must be assigned a register that
//    belongs to that cubclass.
// 
// 2) If <tn> is used in an OP that has an operand or a result
//    (different from <tn>) that requires a specific register,
//    then that register must not be assigned to <tn>.
//
#ifdef TARG_ST
// 3) If <tn> is the result of an extract op where the source is
//    an allocated <tn>, and all but one of the source registers
//    are allocated to other results, then <tn> must not be assigned
//    to the final remaining register of the source.
//
// 4) If <tn> is the result of a compose op where the sources are
//    all allocated <tns>, and <tn> cannot be allocated
//    to the final remaining register of the source.
//
// 5) If <tn> is operand/result of an asm statement, none of the clobber
//    list can be assigned.
//
// 6) If <tn> is result of an asm statement where it is early clobber, 
//    none of asm operand can be assigned to it.

#endif
static REGISTER_SET
Usable_Registers (TN* tn, LIVE_RANGE* lr)
{
  ISA_REGISTER_CLASS cl = TN_register_class(tn);
  REGISTER_SET usable_regs = REGISTER_CLASS_universe(cl);

#ifdef TARG_X8664
  if( TN_is_preallocated(tn) ){
    REGISTER reg = LRA_TN_register( tn );
    return REGISTER_SET_Intersection1(usable_regs,reg);
  }
#endif
#ifdef TARG_ST
  if (! TN_is_save_reg(tn)) {
    usable_regs = REGISTER_SET_Difference(usable_regs,
					  CGTARG_Forbidden_LRA_Registers(cl));
  }
#endif

  INT first_op = MAX(LR_first_def(lr), 1);
  INT last_op = MIN(LR_last_use(lr)+1, VECTOR_size(Insts_Vector));

  for (INT opnum = first_op; opnum < last_op; opnum++) {  

    OP* op = OP_VECTOR_element(Insts_Vector, opnum);

    ASM_OP_ANNOT* asm_info = (OP_code(op) == TOP_asm) ?
      (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op) : NULL;

    // cannot use registers clobbered by an ASM statement
    if (asm_info) {
      usable_regs = REGISTER_SET_Difference(usable_regs, 
					    ASM_OP_clobber_set(asm_info)[cl]);
    }

    for (INT resnum = 0; resnum < OP_results(op); resnum++) {
#ifdef KEY
      if (asm_info) {
        if (TN_register_class(OP_result(op, resnum)) != cl)
	  continue;
      } else
#endif
      if (OP_result_reg_class(op, resnum) != cl) {
        continue;
      }

      TN* result = OP_result(op, resnum);

#ifdef TARG_X8664
      if( TN_is_preallocated( result ) ){
	const REGISTER reg = LRA_TN_register( result );
	const LIVE_RANGE* result_lr = LR_For_TN( result );
	if( result_lr == NULL ||
	    LR_last_use(lr) > LR_first_def(result_lr) ){
	  usable_regs = REGISTER_SET_Difference1(usable_regs, reg);
	}
      }
#endif
#ifdef TARG_ST
      // [CG]: If an asm statement and the result is an early clobber
      // we must not allocate it to any pre-assigned operand.
      bool is_result_clobber = asm_info && ASM_OP_result_clobber(asm_info)[resnum];
      if (is_result_clobber) {
	for (INT onum = 0; onum < OP_opnds(op); onum++) {
	  TN *opnd = OP_opnd(op, onum);
	  if (TN_is_register(opnd) && LRA_TN_register(opnd) != REGISTER_UNDEFINED) {
	    usable_regs = REGISTER_SET_Difference(usable_regs, TN_registers(opnd));
	  }
	}
      }
#endif

      ISA_REGISTER_SUBCLASS sc = asm_info ?
        ASM_OP_result_subclass(asm_info)[resnum] :
        OP_result_reg_subclass(op, resnum);

#ifdef TARG_ST
      if (OP_extract(op) || OP_compose(op)) {
	TN* result = OP_result(op, resnum);
	if (result == tn) {
	  // For extract and compose ops, avoid the situation where the
	  // result registers are a non-identity permutation of the
	  // source registers, because that is impossible to expand
	  // into a sequence of copies.
	  if (OP_extract(op)) {
	    usable_regs = REGISTER_SET_Difference(usable_regs,
						  unusable_extract_result_regs(op));
	  } else {
	    usable_regs = REGISTER_SET_Difference(usable_regs,
						  unusable_compose_result_regs(op));
	  }
	}
      }
#endif

      if (sc == ISA_REGISTER_SUBCLASS_UNDEFINED) {
        continue;
      }

      REGISTER_SET subclass_regs = REGISTER_SUBCLASS_members(sc);
      if (result == tn) {
        usable_regs = REGISTER_SET_Intersection(usable_regs, subclass_regs);
      }
#ifdef TARG_ST
      // [SC] Generally, avoid a singleton subclass known to be live somewhere
      // in the live range of TN.  However, allow it for a result in the
      // same instruction as the last use of TN.
      else if (REGISTER_SET_Size(subclass_regs) == 1
	       && opnum < (last_op - 1)) {
#else
      else if (REGISTER_SET_Size(subclass_regs) == 1) {
#endif
        usable_regs = REGISTER_SET_Difference(usable_regs, subclass_regs);
      }
    }
          
    for (INT opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
#ifdef KEY
      if (asm_info) {
        if (!TN_is_register(OP_opnd(op,opndnum)) ||
	    TN_register_class(OP_opnd(op, opndnum)) != cl)
	  continue;
      } else
#endif
      if (OP_opnd_reg_class(op, opndnum) != cl) {
        continue;
      }

      TN* opnd = OP_opnd(op, opndnum);

#ifdef TARG_X8664
      if( TN_is_preallocated( opnd ) ){
	const REGISTER reg = LRA_TN_register( opnd );
	const LIVE_RANGE* opnd_lr = LR_For_TN( opnd );
	if( opnd_lr == NULL ||
	    LR_first_def(lr) < LR_last_use(opnd_lr) ){
	  usable_regs = REGISTER_SET_Difference1(usable_regs, reg);
	}
      }
#endif

      ISA_REGISTER_SUBCLASS sc = asm_info ?
        ASM_OP_opnd_subclass(asm_info)[opndnum] :
        OP_opnd_reg_subclass(op, opndnum);
#ifdef TARG_ST
      if (OP_extract(op) || OP_compose(op)) {
	TN* opnd = OP_opnd(op, opndnum);
	if (opnd == tn) {
	  // For extract and compose ops, avoid the situation where the
	  // result registers are a non-identity permutation of the
	  // source registers, because that is impossible to expand
	  // into a sequence of copies.
	  if (OP_extract(op)) {
	    usable_regs = REGISTER_SET_Difference(usable_regs,
						  unusable_extract_operand_regs(op));
	  } else {
	    usable_regs = REGISTER_SET_Difference(usable_regs,
						  unusable_compose_operand_regs(op));
	  }
	}
      }
#endif
      if (sc == ISA_REGISTER_SUBCLASS_UNDEFINED) {
        continue;
      }
      REGISTER_SET subclass_regs = REGISTER_SUBCLASS_members(sc);
      if (opnd == tn) {
        usable_regs = REGISTER_SET_Intersection(usable_regs, subclass_regs);
      }
#ifdef TARG_ST
      // [SC] Generally, avoid a singleton subclass known to be live somewhere
      // in the live range of TN.  However, allow it for an operand in the
      // same instruction as TN is defined.
      else if (REGISTER_SET_Size(subclass_regs) == 1
	       && opnum > first_op) {
#else
      else if (REGISTER_SET_Size(subclass_regs) == 1) {
#endif
        usable_regs = REGISTER_SET_Difference(usable_regs, subclass_regs);
      }
    }
  }          
  
  FmtAssert(!REGISTER_SET_EmptyP(usable_regs),
            ("Could not find a register for TN%d in live range [%d-%d]",
              TN_number(tn), LR_first_def(lr), LR_last_use(lr)));

  return usable_regs;
}


static BOOL
Assign_Registers_For_OP (OP *op, INT opnum, TN **spill_tn, BB *bb)
{
  INT opndnum;
  INT resnum;
  REGISTER reg, result_reg;
#ifdef TARG_ST
  INT result_nregs;
#endif
  ISA_REGISTER_CLASS result_cl;
  BOOL uniq_result;
  LIVE_RANGE *clr;
  BOOL result_failed = FALSE;
  INT nresults = OP_results(op);
#ifdef TARG_ST
  // [SC] Note on the size of these arrays:
  // free_result_reg et al need to have enough entries to cope with
  // M entries for each result, where M is the number of registers that
  // may be allocated to a TN (i.e. TN_nhardregs()).
  // ISA_OPERAND_max_results must be an upper bound for M,
  // because there must exist an extract instruction with M results.
  // So give the arrays (nresults * ISA_OPERAND_max_results) entries.
  TN **free_result_tn = TYPE_ALLOCA_N(TN *, nresults * ISA_OPERAND_max_results);
  REGISTER *free_result_reg = TYPE_ALLOCA_N(REGISTER, nresults * ISA_OPERAND_max_results);
  ISA_REGISTER_CLASS *free_result_cl = TYPE_ALLOCA_N(ISA_REGISTER_CLASS,
                                          nresults * ISA_OPERAND_max_results);
  INT free_result_entries = 0;
#else
  mBOOL *free_result = TYPE_ALLOCA_N(mBOOL, nresults);
  INT *free_opnum = TYPE_ALLOCA_N(INT, nresults);
  TN **free_result_tn = TYPE_ALLOCA_N(TN *, nresults);
  REGISTER *free_result_reg = TYPE_ALLOCA_N(REGISTER, nresults);
  ISA_REGISTER_CLASS *free_result_cl = TYPE_ALLOCA_N(ISA_REGISTER_CLASS, nresults);
#ifdef KEY
  BOOL *free_result_is_early_clobber = TYPE_ALLOCA_N(BOOL, nresults);
#endif

  for (resnum = 0; resnum < nresults; resnum++) {
    free_result[resnum] = FALSE;
  }
#endif

  //TODO: change trace to give fat point calculation, not this
  if (Do_LRA_Trace(Trace_LRA_Detail)) {
    fprintf (TFile, "OP:%d>> ", opnum);
    Print_OP_No_SrcLine (op);
#ifdef KEY
    fprintf (TFile, "Avail integer registers: ");
    for (reg = REGISTER_MIN; reg <= REGISTER_MAX; reg++) {
      if (avail_regs[ISA_REGISTER_CLASS_integer].reg[reg])
        fprintf (TFile, " %s", REGISTER_name (ISA_REGISTER_CLASS_integer, reg));
    }
    fprintf (TFile, "\n");

    fprintf (TFile, "Avail fp registers: ");
    for (reg = REGISTER_MIN; reg <= REGISTER_MAX; reg++) {
      if (avail_regs[ISA_REGISTER_CLASS_float].reg[reg])
        fprintf (TFile, " %s", REGISTER_name (ISA_REGISTER_CLASS_float, reg));
    }
    fprintf (TFile, "\n");
#endif
#ifdef TARG_ST
    print_avail_regs();
#endif
  }

  // get ASM OP annotation
  ASM_OP_ANNOT* asm_info = (OP_code(op) == TOP_asm) ?
    (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op) : NULL;
  
  //
  // Process the result TN. If this is the first def of the live range,
  // put the register back on the free list. If the OP has the 
  // "uniq_res" attribute, keep track of this result_reg and do not
  // assign it to any of the operands of this OP.
  //
  uniq_result = FALSE;
  result_reg = REGISTER_UNDEFINED;
#ifdef TARG_ST
  result_nregs = 0;
#endif
  result_cl = ISA_REGISTER_CLASS_UNDEFINED;
  for (resnum = 0; resnum < nresults; resnum++) {
    TN *result_tn = OP_result (op,resnum);
#ifdef TARG_ST
    result_nregs = TN_nhardregs(result_tn);
#endif

    clr = LR_For_TN (result_tn);
    result_reg = LRA_TN_register(result_tn);
    result_cl = TN_register_class(result_tn);
#ifndef TARG_ST
    REGISTER_SET must_use = Usable_Registers(result_tn, clr);
#endif

    //
    // no need to look at result if we're calculating fat points, and
    // this result is not of the register class that we'll be spilling.
    //
    if (!Calculating_Fat_Points() ||
	failing_class == result_cl) {

      // Check if a register is assigned to the result tn. The only
      // case it may not have a register is if it is an unused def that
      // was not deleted.
      if (result_reg == REGISTER_UNDEFINED) {
#ifdef TARG_ST
	REGISTER_SET must_use = Usable_Registers(result_tn, clr);
#endif
	REGISTER prefer_reg = LR_prefer_reg (clr);

        if (prefer_reg == REGISTER_UNDEFINED) {
          if (TN_is_local_reg(result_tn) && (unused_tn_def[result_cl] != NULL) && !OP_side_effects(op)
              #ifdef TARG_ST
	      && !OP_Has_Flag_Effect(op)
	      && REGISTER_SET_MemberP (must_use, 
				       LRA_TN_register (unused_tn_def[result_cl]))
	      && TN_nhardregs(result_tn) == TN_nhardregs (unused_tn_def[result_cl])
#endif
#ifdef TARG_ST
	      && (OP_code(op) != TOP_asm)
#endif
              ) {
#ifdef TARG_ST
	    // Should we check that unused_tn_def[result_cl] is in the
	    // appropriate subclass for op?
#endif
            result_tn = unused_tn_def[result_cl];
            Set_OP_result(op,resnum,result_tn);

            if (Do_LRA_Trace(Trace_LRA_Detail)) {
              fprintf(TFile,"unused def BB:%d %s >>> ",
                      BB_id(bb),REGISTER_name (result_cl, LRA_TN_register(result_tn)));
              Print_OP_No_SrcLine (op);
            }

            if (!OP_has_implicit_interactions (op) &&
                !OP_store(op) &&
#ifdef TARG_ST
		!OP_xfer(op) &&
#endif
                !CGTARG_Is_OP_Intrinsic(op)) {
             /* If all the results are unneeded, delete the op. */
              INT k;
              BOOL all_results_unneeded = TRUE;
              for (k = OP_results(op); 0 < k; k--) {
                TN *ck_tn = OP_result (op,k-1);
                ISA_REGISTER_CLASS ck_cl;
                if (ck_tn != NULL) {
                  ck_cl = TN_register_class(ck_tn);
                  if ((unused_tn_def[ck_cl] == NULL) ||
                      (unused_tn_def[ck_cl] != ck_tn)) {
                    all_results_unneeded = FALSE;
                    break;
                  }
                }
              }
              if (all_results_unneeded) {
                if (Do_LRA_Trace(Trace_LRA_Detail)) {
                  fprintf (TFile, "DELETE OP(unneeded results):%d>> ", opnum);
                  Print_OP_No_SrcLine (op);
                }

                BB_Remove_Op (bb, op);
                return TRUE;
              }
            }

            continue;
          }
        }

	if (prefer_reg != REGISTER_UNDEFINED &&
#ifdef TARG_ST
	    (Is_Reg_Available (result_cl, must_use, prefer_reg, result_nregs, clr) ||
	     Is_Non_Allocatable_Reg_Available(result_cl, must_use, prefer_reg, result_nregs, clr))){
#else
	    (Is_Reg_Available (result_cl, must_use, prefer_reg, clr) ||
	     Is_Non_Allocatable_Reg_Available(result_cl, must_use, prefer_reg, clr))){
#endif
	  result_reg = prefer_reg;
#ifdef TARG_ST
	  Delete_Avail_Reg (result_cl, result_reg, result_nregs, opnum, clr);
#else
	  Delete_Avail_Reg (result_cl, result_reg, opnum);
#endif
	  if (Do_LRA_Trace(Trace_LRA_Detail)) {
#ifdef TARG_ST
            if (result_nregs > 1) {
              fprintf (TFile, "Preferred registers %s:%s\n",
		       REGISTER_name (result_cl, prefer_reg),
		       REGISTER_name (result_cl, prefer_reg + result_nregs - 1));
	    } else {
	      fprintf (TFile, "Preferred register %s\n",
		       REGISTER_name (result_cl, prefer_reg));
	    }
#else
	    fprintf (TFile, "Preferred register %s\n",
		     REGISTER_name (result_cl, prefer_reg));
#endif
	  }
	  if (Always_Trace(Trace_LRA) && trace_tn && result_cl == trace_cl) {
	    bb_live = TN_SET_Union1D(bb_live, result_tn, &lra_pool);
	  }
	} else {
	  result_reg =
#ifdef TARG_ST
	    Allocate_Register(result_tn,FALSE, result_cl, must_use, result_reg,
			      result_nregs, opnum);
#else
	    Allocate_Register(result_tn,FALSE, result_cl, must_use, result_reg,opnum);
#endif
	  if (result_reg == REGISTER_UNDEFINED) {
	    *spill_tn = result_tn;
	    if (!Calculating_Fat_Points()) {
	      //
	      // continue going through the loop with the purpose of gathering
	      // fat point info, if appropriate.  if so, redo failing op.
	      //
	      if (Init_Fat_Point_Calculation(TN_register_class(*spill_tn),
					     opnum,bb)){
#ifdef TARG_ST
		result_reg = Allocate_Register(result_tn, FALSE, result_cl,
					       must_use, result_reg, result_nregs,
					       opnum);
#else
		result_reg = Allocate_Register(result_tn, FALSE, result_cl,
					       must_use, result_reg, opnum);
#endif
		result_failed = TRUE;
	      } else {
		return FALSE;
	      }
	    }
	    FmtAssert(result_reg != REGISTER_UNDEFINED,
		      ("LRA: no register found during fat point calculation.\n"
		       ));
	  }
	}
	LRA_TN_Allocate_Register (result_tn, result_reg);
#ifdef TARG_ST
	if (LRA_overlap_coalescing) {
	  Propagate_Overlap_Coalescing_Preference_Leaf_To_Root(clr, result_reg);
	}
#endif
      } else if (result_reg == REGISTER_sp &&
#ifdef KEY	 // Bug 4327.
		 result_cl == ISA_REGISTER_CLASS_integer &&
#endif
		 CG_localize_tns) {
	Update_Callee_Availability(bb);
      } 

#ifdef KEY
      // Always free the result register, even for ASM OPs.  For ASM
      // early_clobber result, free it after all ASM input operands are
      // allocated.  Bug 13348.
      bool ok_to_free_result = TRUE;
#else
      // it's always safe to free a result register for non-ASM OPs
      // for ASM OPs it's only OK if the result is not early_clobber
      bool ok_to_free_result = 
        !asm_info || !ASM_OP_result_clobber(asm_info)[resnum];
#endif

#ifdef KEY
      // Don't free result if result is a read-modifiy-write.
      ok_to_free_result = ok_to_free_result && !OP_cond_def(op);
#endif
        
#ifdef TARG_ST
      /*
       * Remember all the information needed to free registers.
       * They can't be freed now because they may accidently be
       * reassigned for use as another result in the same OP.
       * (This can happen if one of the results is not used.)
       * [TTh] results of cond_def operations must not be freed
       */
      if (ok_to_free_result
	  && opnum == LR_first_def(clr)
	  && !OP_cond_def(op)
	  && (result_reg > REGISTER_MAX
              || REGISTER_allocatable (result_cl, result_reg))) {
	REGISTER r;
	FOR_ALL_NREGS (result_reg, TN_nhardregs(result_tn), r) {
	  // [SC] Registers of dedicated/assigned TNs are considered
	  // to be in use from their first def to their last use, so
	  // do not free up here unless opnum is their first def.
	  // This matches the original open64 implementation, but I
	  // think it could be refined if necessary.
	  if (TN_is_local_reg (result_tn)
	      || ded_reg_first_def(result_cl, r) == opnum) {
	    free_result_tn[free_result_entries] = result_tn;
	    free_result_reg[free_result_entries] = r;
	    free_result_cl[free_result_entries] = result_cl;
	    ++free_result_entries;
	  }
	}
      }
      // Arthur: try out this logic:
      if (OP_uniq_res(op, resnum)) uniq_result = TRUE;
#else

      if (ok_to_free_result && opnum == LR_first_def(clr)) {

/*
 * Remember all the information needed to free registers.
 * They can't be freed now because they may accidently be
 * reassigned for use as another result in the same OP.
 * (This can happen if one of the results is not used.)
 */
        free_opnum[resnum] = opnum;
        free_result_tn[resnum] = result_tn;
        free_result[resnum] = TRUE;
        free_result_reg[resnum] = result_reg;
        free_result_cl [resnum] = result_cl;
#ifdef KEY
	free_result_is_early_clobber[resnum] =
	  (asm_info && ASM_OP_result_clobber(asm_info)[resnum]) ? TRUE : FALSE;
#endif
      }
      if (OP_uniq_res(op)) uniq_result = TRUE;
#endif

#ifdef KEY
      // If ASM writes to a callee-saved register, then add the register to
      // Callee_Saved_Regs_Used.
      if (!result_failed &&
	  asm_info &&
	  REGISTER_SET_MemberP(REGISTER_CLASS_callee_saves(result_cl),
			       result_reg)) {
	Callee_Saved_Regs_Used[result_cl] =
	  REGISTER_SET_Union1(Callee_Saved_Regs_Used[result_cl], result_reg);
      }
#endif
    }
  }

/*
 * NOW, free up the result registers for re-use.
 */
  BOOL assignment_undone = FALSE;
#ifdef TARG_ST
  for (resnum = 0; resnum < free_result_entries; resnum++) {
    assignment_undone = TRUE;
    Add_Avail_Reg (free_result_cl[resnum], free_result_reg[resnum], 1, opnum);
    if (Always_Trace(Trace_LRA) && trace_tn && (free_result_cl[resnum] == trace_cl))  {
      bb_live = TN_SET_Difference1D(bb_live, free_result_tn[resnum]);
      if (TN_number(free_result_tn[resnum]) == trace_tn) {
	LRA_Print_Liveness();
      }
    }
  }
#else
  for (resnum = 0; resnum < nresults; resnum++) {
    if (free_result[resnum]) {
#ifdef KEY
	// Free early_clobber result only after all ASM input operands are
	// allocated.
	if (free_result_is_early_clobber[resnum])
	  continue;
#endif
        BOOL free_reg = TRUE;
#ifdef TARG_X8664
	/* The use of one of the results might have been removed earlier. */
	if( TN_is_preallocated( OP_result(op,resnum) ) &&
	    avail_regs[free_result_cl[resnum]].reg[free_result_reg[resnum]] ){
	  free_reg = FALSE;
	}
#endif
        assignment_undone = TRUE;
	if( free_reg )
	  Add_Avail_Reg (free_result_cl[resnum], free_result_reg[resnum], free_opnum[resnum]);
	if (Always_Trace(Trace_LRA) && trace_tn && (free_result_cl[resnum] == trace_cl))  {
	  bb_live = TN_SET_Difference1D(bb_live, free_result_tn[resnum]);
	  if (TN_number(free_result_tn[resnum]) == trace_tn) {
	    LRA_Print_Liveness();
	  }
	}
    }
  }
#endif
  if (assignment_undone && result_failed) {
   // set fat_points for this guy to one.  these weird things will 
   // get allocated then free'd, so Fix_LRA_Blues will flip because
   // it'll see a spill required for an instruction that needs 
   // no registers (i.e. fat_points is 0 at the failure point).
   // should only happen in rare occasions such as when wopt is
   // run, but optimization is turned off in cg.
    fat_points[opnum] = 1;
  }

#ifdef KEY
  //
  // process ASM clobbers.
  //
  // Add ASM clobbered callee-saved registers to Callee_Saved_Regs_Used.
  if (asm_info) {
    ISA_REGISTER_CLASS cl;
    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      Callee_Saved_Regs_Used[cl] =
	REGISTER_SET_Union(Callee_Saved_Regs_Used[cl],
	  REGISTER_SET_Intersection(REGISTER_CLASS_callee_saves(cl),
				    ASM_OP_clobber_set(asm_info)[cl]));
    }
  }
#endif

  //
  // process all the operand TNs.
  //
  for (opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
    TN *tn = OP_opnd(op, opndnum);

    // If it is an invalid TN or constant TN  or it has already been 
    // assigned a register, continue.
    if (tn == NULL || 
        TN_is_constant(tn)
#ifndef TARG_X8664
        || ( (LRA_TN_register(tn) != REGISTER_UNDEFINED) )
#endif
	)
    {
      continue;
    }

    clr = LR_For_TN (tn);

    REGISTER prefer_reg = LR_prefer_reg (clr);
#ifdef TARG_ST
    INT nregs = TN_nhardregs (tn);
#endif
    ISA_REGISTER_CLASS regclass = TN_register_class(tn);
    REGISTER_SET must_use = Usable_Registers(tn, clr);

#ifdef KEY
    // If ASM uses a callee-saved register, then add the register to
    // Callee_Saved_Regs_Used.  Bug 13100.
    if (asm_info) {
      if (LRA_TN_register(tn) != REGISTER_UNDEFINED) {
	const REGISTER reg = LRA_TN_register(tn);
	const ISA_REGISTER_CLASS rc = TN_register_class(tn);
	if (reg <= REGISTER_MAX &&
	    REGISTER_allocatable(rc, reg) &&
	    REGISTER_SET_MemberP(REGISTER_CLASS_callee_saves(rc), reg)) {
	  Callee_Saved_Regs_Used[rc] =
	    REGISTER_SET_Union1(Callee_Saved_Regs_Used[rc], reg);
	}
      }

      // For 'm' constraint modifier, the operand is a pointer to the memory
      // data.  This pointer is "late use" in the sense that the TN could be
      // used to reference memory after the ASM has written some of its
      // register results.  Late use operands must not be assigned any of the
      // result registers.  ("Late use" is the dual of "early clobber" results
      // which write their register before the ASM has finished reading all its
      // inputs.  Early clobber is identified by the '&' modifier.)  Bug 14957.
      if (ASM_OP_opnd_memory(asm_info)[opndnum]) {
	for (resnum = 0; resnum < nresults; resnum++) {
	  TN *result_tn = OP_result(op, resnum);
	  const ISA_REGISTER_CLASS cl = TN_register_class(result_tn);
	  if (regclass == cl) {
	    const REGISTER reg = LRA_TN_register(result_tn);
	    must_use = REGISTER_SET_Difference1(must_use, reg);
	  }
	}
      }
    }
#endif

#ifdef TARG_X8664
    if (LR_byteable(clr)) {
      const REGISTER_SET regs =
	REGISTER_SUBCLASS_members(ISA_REGISTER_SUBCLASS_m32_8bit_regs);
      must_use = REGISTER_SET_Intersection(must_use, regs);
    }

    /* When the register of a GTN is preallocated for a later div-like op,
       and then put into the avail_regs pool, we need to mark it as used.
    */
    if( LRA_TN_register(tn) != REGISTER_UNDEFINED &&
	!TN_is_preallocated(tn) ){
      const REGISTER reg = LRA_TN_register(tn);
      const ISA_REGISTER_CLASS rc = TN_register_class(tn);
      if( reg <= REGISTER_MAX             &&
	  REGISTER_allocatable( rc, reg ) &&
	  avail_regs[rc].reg[reg] ){
	avail_regs[rc].reg[reg] = FALSE;
      }

      continue;
    }

    if( opndnum == 0       &&
	OP_x86_style( op ) &&
	result_reg <= REGISTER_MAX ){
      prefer_reg = result_reg;
    }
#endif
    
    //
    // no need to look at operands that aren't of the class
    // being spilled.
    //
    if (Calculating_Fat_Points() &&
	failing_class != regclass) {
      continue;
    }
#ifdef TARG_ST
    if ((OP_extract(op) || OP_compose(op) || OP_widemove(op))
	&& LRA_TN_register(OP_result(op, 0)) != REGISTER_UNDEFINED
	&& LRA_TN_register(OP_result(op, 0)) <= REGISTER_MAX) {
      prefer_reg = LRA_TN_register (OP_result(op, 0)) + (opndnum * nregs);
    }
#endif
    if (prefer_reg != REGISTER_UNDEFINED &&
	(!uniq_result || (prefer_reg != result_reg)) &&
#ifdef TARG_ST
        (Is_Reg_Available (regclass, must_use, prefer_reg, nregs, clr) ||
         Is_Non_Allocatable_Reg_Available (regclass, must_use, prefer_reg, nregs, clr)))
#else
        (Is_Reg_Available (regclass, must_use, prefer_reg, clr) ||
         Is_Non_Allocatable_Reg_Available (regclass, must_use, prefer_reg, clr)))
#endif
    {
      LRA_TN_Allocate_Register (tn, prefer_reg);
#ifdef TARG_ST
      if (LRA_overlap_coalescing) {
	Propagate_Overlap_Coalescing_Preference_Leaf_To_Root(clr, prefer_reg);
      }
      Delete_Avail_Reg (TN_register_class(tn), prefer_reg, nregs, opnum, clr);
#else
      Delete_Avail_Reg (TN_register_class(tn), prefer_reg, opnum);
#endif
      if (Do_LRA_Trace(Trace_LRA_Detail)) {
#ifdef TARG_ST
	fprintf (TFile, "Preferred register %s",
		 REGISTER_name (regclass, prefer_reg));
        if (nregs > 1)
          fprintf (TFile, "-%s", REGISTER_name (regclass, prefer_reg + nregs -1));
        fprintf (TFile, "\n");
#else
	fprintf (TFile, "Preferred register %s\n",
		 REGISTER_name (regclass, prefer_reg));
#endif
      }
      if (Always_Trace(Trace_LRA) && trace_tn &&
	  TN_register_class(tn) == trace_cl ) {
	bb_live = TN_SET_Union1D(bb_live, tn, &lra_pool);
      }
      continue;
    }

    // This tn is either the last use of a live range or an exposed use.
    // Pick a register from the free list and assign it to the TN. 
    // NOTE1: We also check if the result of the OP needs to be unique.
    // If yes, make sure we don't pick the same register as the result.
#ifdef TARG_ST
    // [SC] For extract/compose/movp, we have already tried to preference
    // to the result registers.  If that did not work, avoid all result
    // registers to ensure that it is possible to expand to a sequence of movs.
    if (OP_extract(op) || OP_compose(op) || OP_widemove(op)) {
      uniq_result = TRUE;
    }
    // [SC/TTh] For a TN initially defined by an extract op (or, when overlap
    // coalescing is enabled, a TN that contains at least an ancestor in
    // its coalescing tree), we would like to ensure that it gets allocated
    // to a suitable register for the _source_ of the extract op (or the
    // root of the coalescing tree), so that we do not need to expand the
    // extract(s) into move operations.
    // To get this, we here allocate the source of the extract op (or the
    // root of the coalescing tree), then assign the appropriate allocated
    // register to this operand.
    // Immediately deallocate all the other registers, but set preferred
    // register of all the other extract dests to be these deallocated
    // registers (or propagate preferencing through overlap coalescing tree).
    // We rely on the round-robin nature
    // of Get_Avail_Reg and last_assigned_reg to ensure that these registers
    // are not allocated to anything else before we need them for the
    // other extract rests. When overlap coalescing is used, Get_Avail_Reg()
    // is also able to smartly avoid those suboptimal allocations.
    // [TTh] dest_nbregs is useful when extracted TNs are multiple-registers.
    // It is then required to allocate/free group of single register.
    // [TTh] When overlap coalescing is enabled, LR_extract_op is defined only
    // when the current liverange can be coalesced with the source of the extract
    if (LRA_overlap_coalescing) {
      // At this point, we already know that either current LR has no preference
      // or that the preference did not work.
      if (LR_ovcoal_parent(clr)) {
	LIVE_RANGE *root_lr = LR_ovcoal_root(clr);
	TN *root_tn = LR_tn(root_lr);
	if (LRA_TN_register(root_tn) == REGISTER_UNDEFINED) {
	  INT root_LR_last_use = LR_last_use(root_lr);
	  Set_LR_last_use (root_lr, LR_last_use (clr));
	  reg = Allocate_Register (root_tn, uniq_result,
				   regclass,
				   Usable_Registers(root_tn, root_lr),
				   result_reg,
				   result_nregs, opnum);
	  Set_LR_last_use (root_lr, root_LR_last_use);
	  if (reg != REGISTER_UNDEFINED) {
	    if (reg <= REGISTER_MAX) {
	      INT dest_nbregs = TN_nhardregs(LR_tn(clr));
	      INT root_nbregs = TN_nhardregs(root_tn);
	      INT dest_rank   = LR_ovcoal_rank(clr);
	      for (INT d = 0; d < root_nbregs; d++) {
		if (d == dest_rank) {
		  LRA_TN_Allocate_Register (LR_tn(clr), reg + d);
		  Propagate_Overlap_Coalescing_Preference_Leaf_To_Root(clr, reg + d);
		}
		else if (d < dest_rank || d >= dest_rank + dest_nbregs) {
		  Add_Avail_Reg (regclass, reg + d, 1, opnum);
		}
	      }
	      continue;
	    }
	    else {
	      FmtAssert(Calculating_Fat_Points(),
			("LRA: Invalid register returned by Allocate_Register()"));
	      // [TTh] When computing fat points, Allocate_Register()  might
	      // "allocate" a virtual register (reg > REGISTER_MAX) if there
	      // is no real register available.
	      // As we don't care about such registers here, we need to free
	      // them (mainly to decrement the fat point).
	      Add_Avail_Reg (regclass, reg, TN_nhardregs(root_tn), opnum);
	    }
	  }
	}
      }
    }
    else {
      OP *extract_op = LR_extract_op (clr);
      if (LRA_merge_extract
	  && extract_op
	  && OP_extract (extract_op)
	  && LR_def_cnt (clr) == 1) {
	TN *extract_source = OP_opnd(extract_op, 0);
	if (LRA_TN_register(extract_source) == REGISTER_UNDEFINED) {
	  LIVE_RANGE *extract_source_lr = LR_For_TN (extract_source);
	  if (LR_last_use (extract_source_lr) == LR_first_def (clr)) {
	    ISA_REGISTER_CLASS extract_source_cl = TN_register_class (extract_source);
	    Set_LR_last_use (extract_source_lr, LR_last_use (clr));
	    reg = Allocate_Register (extract_source, uniq_result,
				     extract_source_cl,
				     Usable_Registers(extract_source, extract_source_lr),
				     result_reg,
				     result_nregs, opnum);
	    Set_LR_last_use (extract_source_lr, LR_first_def (clr));
	    if (reg != REGISTER_UNDEFINED) {
	      if (reg <= REGISTER_MAX) {
		INT dest_nbregs = TN_nhardregs(OP_result(extract_op, 0));
		for (INT d = 0; d < OP_results(extract_op); d++) {
		  TN *extract_dest = OP_result(extract_op, d);
		  if (extract_dest == tn) {
		    LRA_TN_Allocate_Register (extract_dest, reg + (d * dest_nbregs));
		  } else {
		    LIVE_RANGE *d_lr = LR_For_TN (extract_dest);
		    if (LR_prefer_reg (d_lr) == REGISTER_UNDEFINED) {
		      Set_LR_prefer_reg (d_lr, reg + (d * dest_nbregs));
		    }
		    Add_Avail_Reg (regclass, reg + (d * dest_nbregs), dest_nbregs, opnum);
		  }
		}
		continue;
	      }
	      else {
		FmtAssert(Calculating_Fat_Points(),
			  ("LRA: Invalid register returned by Allocate_Register()"));
		// [TTh] When computing fat points, Allocate_Register()  might
		// "allocate" a virtual register (reg > REGISTER_MAX) if there
		// is no real register available.
		// As we don't care about such registers here, we need to free
		// them (mainly to decrement the fat point).
		Add_Avail_Reg (regclass, reg, TN_nhardregs(extract_source), opnum);
	      }
	    }
	  }
	}
      }
    }
    reg = Allocate_Register (tn, uniq_result, result_cl, must_use, result_reg,
			     result_nregs, opnum);
    if (LRA_overlap_coalescing) {
      Propagate_Overlap_Coalescing_Preference_Leaf_To_Root(clr, reg);
    }
#else
    reg = Allocate_Register (tn, uniq_result, result_cl, must_use, result_reg, opnum);
#endif
#ifdef TARG_X8664
    /* bug#379
       Try to use a register that is used by an inline asm; otherwise, there is
       no register can be allocated for op like div and shift.
    */
    if( reg == REGISTER_UNDEFINED &&
	TN_is_preallocated( tn ) ){
      reg = LRA_TN_register( tn );
      FmtAssert( !REGISTER_allocatable( regclass, reg ),
		 ("no register is available for a pre-allocated tn") );

      TN* ded_tn = Build_Dedicated_TN( regclass, reg, 0 );
      const LIVE_RANGE* ded_lr = LR_For_TN( ded_tn );

      if( ded_lr == NULL ||
	  LR_first_def(clr) < LR_exposed_use(ded_lr ) ||
	  LR_last_use(clr) > LR_first_def(ded_lr) ){
	FmtAssert( false,
		   ("no register is available for a pre-allocated tn") );
      }
    }
#endif

    if (reg == REGISTER_UNDEFINED) {
      *spill_tn = tn;
      if (!Calculating_Fat_Points()) {
	//
	// continue going through the loop with the purpose of gathering
	// fat point info, if appropriate.  if so, redo failing op.
	//
	if (Init_Fat_Point_Calculation(TN_register_class(*spill_tn),opnum,bb)){
	  fat_points[opnum] = 0;
#ifdef TARG_ST
	  reg = Allocate_Register (tn, uniq_result, result_cl, must_use, result_reg,
				   result_nregs, opnum);
#else
	  reg = Allocate_Register (tn, uniq_result, result_cl, must_use, result_reg,
				   opnum);
#endif
	} else {
	  return FALSE;
	}
      }
      FmtAssert(reg != REGISTER_UNDEFINED,
		("LRA: no register found during fat point calculation.\n"));
    }

    LRA_TN_Allocate_Register (tn, reg);

    // If the register we allocated was for an exposed use of a local TN,
    // we deallocate it immediately, since we don't want to block the 
    // register.
    if (opnum <= LR_exposed_use(clr)) {
#ifdef TARG_ST
      Add_Avail_Reg (TN_register_class(tn), reg, nregs, opnum);
#else
      Add_Avail_Reg (TN_register_class(tn), reg, opnum);
#endif
      if (Always_Trace(Trace_LRA) && trace_tn &&
	  TN_register_class(tn) == result_cl)  {
	bb_live = TN_SET_Difference1D(bb_live, tn);
	if (TN_number(tn) == trace_tn) {
	  LRA_Print_Liveness();
	}
      }
    }
  }

#ifdef KEY
  // Free early_clobber ASM results.
  for (resnum = 0; resnum < nresults; resnum++) {
    if (free_result[resnum] &&
	free_result_is_early_clobber[resnum]) {
      Add_Avail_Reg(free_result_cl[resnum], free_result_reg[resnum],
		    free_opnum[resnum]);
      if (Always_Trace(Trace_LRA) &&
	  trace_tn &&
	  (free_result_cl[resnum] == trace_cl)) {
	bb_live = TN_SET_Difference1D(bb_live, free_result_tn[resnum]);
	if (TN_number(free_result_tn[resnum]) == trace_tn) {
	  LRA_Print_Liveness();
	}
      }
    }
  }
#endif

  return TRUE;
}

/* ======================================================================
 * Check_Undefined_Results
 * 
 * See if all results of an op have no use.
 *
 * ======================================================================*/
static BOOL
Check_Undefined_Results(OP* op)
{
  BOOL defined = FALSE;
  for (INT i = OP_results(op) - 1; i >= 0; --i) {
    if (LRA_TN_register(OP_result(op,i)) != REGISTER_UNDEFINED) {
      defined = TRUE;
      break;
    }
  }

#ifdef TARG_X8664
  if( !defined && TOP_is_change_rflags( OP_code(op) ) ){
    for( OP* next = OP_next(op); next != NULL; next = OP_next(next) ){
      if( OP_reads_rflags( next) ){
	defined = TRUE;
	break;
      }
      if( TOP_is_change_rflags( OP_code(next) ) )
	break;
    }
  }
#endif

  return !defined;
}

/* ======================================================================
 * Classes_Match
 * 
 * Check that the register class being currently allocated is the same
 * as the result register class of this instruction.  The problem we
 * currently have is with multiple result instructions.  If we have an
 * instruction that writes registers of multiple classes, then we can't
 * easily delete the instruction.  I don't think at present it is an issue
 * with any architecture that we support, so we'll punt on that for now.
 * Such things would probably have to be handled by EBO anyway since we're
 * allocating one class at a time, and detecting that multiple results of
 * different classes are all not used would be messy to say the least.
 *
 * ======================================================================*/
static BOOL
Classes_Match(OP* op)
{
  for (INT i = OP_results(op) - 1; i >= 0; --i) {
    if (TN_register_class(OP_result(op, i)) != failing_class) {
      return FALSE;
    }
  }
  return TRUE;
}

/* ======================================================================
 * Assign_Registers
 *
 * This pass through the OPs allocates registers for all the 
 * local TNs in the basic block. It returns TRUE if successful.
 * ======================================================================*/
static BOOL
Assign_Registers (BB *bb, TN **spill_tn, BOOL *redundant_code)
{
  if (Do_LRA_Trace(Trace_LRA_Detail)) {
    fprintf (TFile, "--------------------------------------------\n");
    fprintf (TFile, "Register Assignment for BB%d\n", BB_id(bb));
    fprintf (TFile, "--------------------------------------------\n");
  }

  for (INT opnum = BB_length(bb); opnum > 0; opnum--) 
  {
    OP *op = OP_VECTOR_element (Insts_Vector, opnum);

    //
    // if we're calculating fatpoints, start out with the
    // value of the last instruction seen in the backward
    // walk.
    //
    if (Calculating_Fat_Points()) {
      fat_points[opnum] = fat_points[opnum+1];
    }

    //
    // Check if we have a redundant definition. If yes, try to delete it.
    // A definition is known to be undefined because we are doing a backward
    // walk, and if it has not been allocated a register then it must be
    // redundant.  if we're calculating fat points, make sure we only delete
    // instructions of the appropriate class (these are the only one's that
    // are being "assigned" registers).
    //
    if (CG_opt_level > 0 && 
        OP_has_result(op) &&
	Check_Undefined_Results(op) &&
	(!Calculating_Fat_Points() ||
	 Classes_Match(op)) &&
	!OP_has_implicit_interactions (op) &&
        !OP_store(op) &&
#ifdef TARG_ST
	!OP_xfer(op) &&
#endif
        !CGTARG_Is_OP_Intrinsic(op) &&
#ifdef TARG_ST
        !(CG_LRA_mask & 0x400))
#else
        !Get_Trace (TP_ALLOC, 0x400))
#endif
    {
      // Go through all the operands of this <op> and update the 
      // LR_use_cnt and LR_last_use fields for their live ranges.
      for (INT opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
        TN *tn = OP_opnd(op, opndnum);

        // If it is an invalid TN or constant TN  or it has already been 
        // assigned a register, continue.
        if (tn == NULL || 
            TN_is_constant(tn) ||
            LRA_TN_register(tn) != REGISTER_UNDEFINED) 
        {  
#ifdef TARG_X8664
	  if( tn != NULL             &&
	      TN_is_register( tn )   &&
	      !TN_is_dedicated( tn ) &&
	      LRA_TN_register(tn) != REGISTER_UNDEFINED ){
	    const REGISTER reg = LRA_TN_register(tn);
	    const ISA_REGISTER_CLASS rc = TN_register_class(tn);
	    if( reg <= REGISTER_MAX             &&
		REGISTER_allocatable( rc, reg ) &&
		avail_regs[rc].reg[reg] ){
	      avail_regs[rc].reg[reg] = FALSE;
	    }
	  }
#endif
          continue;
        }
        LIVE_RANGE *opnd_lr = LR_For_TN (tn);
        LR_use_cnt(opnd_lr)--;
        if (LR_use_cnt(opnd_lr) != 0) {
          // udpate the LR_last_use field.1
          for (INT pnum = opnum-1; pnum > 0; pnum--) {
            OP *prev_op = OP_VECTOR_element (Insts_Vector, pnum);
            if (OP_Refs_TN (prev_op, tn)) {
#ifdef TARG_ST
              Set_LR_last_use (opnd_lr, pnum);
#else
              LR_last_use(opnd_lr) = pnum;
#endif
              break;
            }
          }
        }
      }
      if (Do_LRA_Trace(Trace_LRA_Detail)) {
        fprintf (TFile, "DELETE OP:%d>> ", opnum);
        Print_OP_No_SrcLine (op);
      }
      // Now delete the unused definition.
      if (!Calculating_Fat_Points()) {
	BB_Remove_Op (bb, op);
      } else {
	//
	// mark the op for removal, and convert it to a noop so that
	// it no longer effects allocation
	//
	Mark_For_Removal(op);
	*redundant_code = TRUE;
      }
      continue;
    }

#ifdef TARG_ST
    if (LRA_overlap_coalescing && !Calculating_Fat_Points() &&
	Do_LRA_Trace(Trace_LRA_Detail)) {
      Print_Overlap_Coalescing();
    }
#endif
    if (!Assign_Registers_For_OP (op, opnum, spill_tn, bb)) {
      return FALSE;
    }

    // Check if the instruction is a self copy.  don't care about this
    // if calculating fat points
    if (!Calculating_Fat_Points() &&
        (Is_Marked_For_Removal(op) ||
         (CGTARG_Is_Preference_Copy(op) &&
#ifdef TARG_ST // CL: ensure source is a register
	  TN_is_register(OP_opnd(op, OP_Copy_Operand(op))) &&
	  LRA_TN_register(OP_result(op,OP_Copy_Result(op))) ==
	    LRA_TN_register(OP_opnd(op, OP_Copy_Operand(op)))))) {
#else
	  LRA_TN_register(OP_result(op,0)) ==
	          LRA_TN_register(OP_opnd(op, CGTARG_Copy_Operand(op)))))) {
#endif
      *redundant_code = TRUE;
    }
  }

#ifdef TARG_ST
  if (BB_asm(bb)) {
    ANNOTATION *ant = ANNOT_Get (BB_annotations(bb), ANNOT_ASMINFO);
    Is_True(ant, ("ASMINFO annotation info not present"));
    ASMINFO *info = ANNOT_asminfo(ant);
    ISA_REGISTER_CLASS rc;

    FOR_ALL_ISA_REGISTER_CLASS(rc) {
      REGISTER_SET clobbers = ASMINFO_kill(info)[rc];
      for (REGISTER reg = REGISTER_SET_Choose(clobbers);
		    reg != REGISTER_UNDEFINED;
		    reg = REGISTER_SET_Choose_Next(clobbers, reg)) {

        if (REGISTER_SET_MemberP(REGISTER_CLASS_callee_saves(rc),reg)) {
          Callee_Saved_Regs_Used[rc] = 
	    REGISTER_SET_Union1(Callee_Saved_Regs_Used[rc],reg);
        }
      }
    }
  }
#endif

  if (Calculating_Fat_Points()) {
    failing_class = ISA_REGISTER_CLASS_UNDEFINED;
    return FALSE;
  }
  return TRUE;
}


/***************************** Spill *****************************************/


/* Compute the set of global registers that are live through the bb. */
static void
Compute_Livethrough_Set (BB *bb)
{
  OP *op;
  ISA_REGISTER_CLASS cl;

  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    livethrough[cl] = REGISTER_SET_Difference (
                                    REGISTER_CLASS_allocatable(cl),
                                    avail_set[cl]);
  }
  FOR_ALL_BB_OPs_FWD (bb, op) {
    INT opndnum;
    INT resnum;
    for (opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
      TN *tn = OP_opnd(op, opndnum);
      if (tn != NULL && 
          TN_is_register(tn) &&
          (TN_is_dedicated(tn) | TN_is_global_reg(tn)))
      {
        ISA_REGISTER_CLASS cl = TN_register_class(tn);
#ifdef TARG_ST
        livethrough[cl] =
	REGISTER_SET_Difference (livethrough[cl], TN_registers(tn));
#else
        REGISTER reg = LRA_TN_register(tn);
        livethrough[cl] = REGISTER_SET_Difference1 (livethrough[cl], reg);
#endif
      }
    }
    for (resnum = 0; resnum < OP_results(op); resnum++) {
      TN *tn = OP_result(op, resnum);
      if (TN_is_dedicated(tn) | TN_is_global_reg(tn)) {
        ISA_REGISTER_CLASS cl = TN_register_class(tn);
#ifdef TARG_ST
        livethrough[cl] =
	  REGISTER_SET_Difference (livethrough[cl], TN_registers (tn));
#else
        REGISTER reg = LRA_TN_register(tn);
        livethrough[cl] = REGISTER_SET_Difference1 (livethrough[cl], reg);
#endif
      }
    }
  }
}


static void
Analyze_Spilling_Global_Register (
  ISA_REGISTER_CLASS cl, 
  SPILL_CANDIDATE *best, 
  INT spill_opnum, 
  INT fatpoint)
{
  /* TODO: Try getting a caller save register from GRA's list. This is better
   * because the save/restore does not have be tracked for the .debug_info
   * information. 
   */
  REGISTER reg = REGISTER_SET_Choose (livethrough[cl]);
  if (reg != REGISTER_UNDEFINED) {
    float store_cost, restore_cost;
    INT benefit = 0;
    TN *spill_tn = Build_Dedicated_TN (cl, reg, 0);
    CGSPILL_Cost_Estimate (spill_tn, NULL, &store_cost, &restore_cost, CGSPILL_LRA);
    best->cost = store_cost + restore_cost;     
    best->spill_kind = SPILL_GLOBAL_REG;
    best->u1.s1.global_spill_reg = reg;
    best->u1.s1.spill_cl = cl;
    for (INT i = 1; i <= spill_opnum; i++) {
      if (fat_points[i] >= fatpoint) benefit++;
    }
    best->benefit = benefit;
    if (Do_LRA_Trace(Trace_LRA_Spill)) {
      fprintf (TFile, "Analyze >> (cost:%g, benefit:%d) spill register %s\n", 
        best->cost, benefit, REGISTER_name (cl, reg));
    }
  }
}

/* ======================================================================
 * Spill_Global_Register
 *
 * Pick one of the registers from GRA's list and spill it at the start
 * of the basic block and restore it at the bottom of the basic block.
 * ======================================================================*/
static void
Spill_Global_Register (BB *bb, SPILL_CANDIDATE *best)
{
  OPS spill_ops;
  TN *new_tn;
  REGISTER reg = best->u1.s1.global_spill_reg;
  ISA_REGISTER_CLASS cl = (ISA_REGISTER_CLASS)best->u1.s1.spill_cl;

  if (Do_LRA_Trace(Trace_LRA_Spill)) {
    fprintf (TFile, "LRA_SPILL>> Spilled Global Register : %s\n", 
                    REGISTER_name (cl, reg));
  }

  /* TODO: Don't use the dedicated tn to store/restore, use a proxy tn 
   * instead.
   */
  TN *spill_tn = Build_Dedicated_TN (cl, reg, 0);
  ST *spill_loc;
  ST *sv_spill_location = NULL;
  BOOL magic_spill_used = FALSE;

#ifdef TARG_X8664
  if( cl == ISA_REGISTER_CLASS_float &&
      float_reg_could_be_128bit[reg] ){
    Set_TN_size( spill_tn, 16 );
  }
#endif

  if (Trip_Count == MAX_TRIP_COUNT &&
      Magic_Spill_Location != NULL &&
#ifdef TARG_ST
      TN_size (spill_tn) <= ST_size (Magic_Spill_Location))
#else
      !TN_is_float(spill_tn)) 
#endif
  {
    sv_spill_location = TN_spill(spill_tn);
    spill_loc = Magic_Spill_Location;
    Set_TN_spill(spill_tn, spill_loc);
    magic_spill_used = TRUE;
    DevWarn ("Used short-offset spill location in BB%d", BB_id(bb));
  }
  else {
    spill_loc = CGSPILL_Get_TN_Spill_Location (spill_tn, CGSPILL_LGRA);
  }

  CGSPILL_Store_To_Memory (spill_tn, spill_loc, OPS_Init(&spill_ops),
			   CGSPILL_LRA, bb);
  CGSPILL_Prepend_Ops (bb, &spill_ops);
  new_tn = Build_TN_Like (spill_tn);
  Set_TN_spill(new_tn, spill_loc);
  CGSPILL_Load_From_Memory (new_tn, spill_loc, OPS_Init(&spill_ops),
			    CGSPILL_LRA, bb);
  CGSPILL_Append_Ops (bb, &spill_ops);
  Set_TN_is_global_reg (new_tn);
  LRA_TN_Allocate_Register (new_tn, reg);

  if (magic_spill_used) {
    Set_TN_spill(spill_tn, sv_spill_location);
    Magic_Spill_Location = NULL;
  }
}

static BOOL
Can_Use_Be_Moved (
  LIVE_RANGE *lr, 
  INT spill_opnum,
  INT *moveto)
{
  ARC_LIST *arcs;
  INT use_opnum = LR_last_use(lr);
  OP *use_op = OP_VECTOR_element (Insts_Vector, use_opnum);
  ISA_REGISTER_CLASS cl = TN_register_class (LR_tn(lr));
  INT tgt_opnum = LR_first_def(lr);

#ifdef TARG_ST
  // Precondition: lr is live range of local (not global, nor dedicated) TN.
  FmtAssert(TN_is_local_reg(LR_tn(lr)),
	    ("LRA: Can_Use_Be_Moved passed non-local TN\n"));
#endif
  // If there is no def in the live range lr>, tgt_opnum is 0.
  if (tgt_opnum == 0 || OP_flag1 (use_op) || OP_xfer(use_op)) return FALSE;

#ifdef TARG_MIPS
  // Don't lengthen the live range of a fcc register in order to prevent
  // spilling.  Spilling fcc is not possible because there is no load/store
  // instruction for fcc.  Bug 13241.
  if (OP_results(use_op) == 1 &&
      TN_register_class(OP_result(use_op, 0)) == ISA_REGISTER_CLASS_fcc)
    return FALSE;
#endif

  for (arcs = OP_preds(use_op); arcs != NULL; arcs = ARC_LIST_rest(arcs)) {
    ARC *arc = ARC_LIST_first(arcs);
    OP *pred_op = ARC_pred(arc);
    // Prevent moving above a just moved def:
    if (OP_flag1(pred_op)) return FALSE;
    INT pred_opnum = BB_OP_MAP32_Get (op_to_opnum, pred_op);
    // Determine the last definition of an input to the move candidate:
    if (pred_opnum > tgt_opnum) tgt_opnum = pred_opnum;
  }

  /* If there is more than one use in the live range, find the 
   * second last use. There is no point moving the last use 
   * above this point.
   */
  if (LR_use_cnt(lr) > 1) {
    for (INT i = use_opnum-1; i > tgt_opnum; i--) {
      OP *op = OP_VECTOR_element (Insts_Vector, i);
      if (OP_Refs_TN (op, LR_tn(lr))) {
        tgt_opnum = i;
        break;
      }
    }
  }

  /* check if the tgt_opnum has already been marked as being moved. */
  while (tgt_opnum < spill_opnum) {
    OP *op = OP_VECTOR_element (Insts_Vector, tgt_opnum);
    if (!OP_flag1(op)) break;
    tgt_opnum++;
  }

  if (tgt_opnum >= (spill_opnum - 1)) return FALSE;

  INT regs_needed = 0;
  INT i;
  for (i = 0; i < OP_results(use_op); i++) {
    TN *tn = OP_result(use_op, i);
    if (!TN_is_const_reg(tn) && (TN_register_class(tn) == cl)) {
      LIVE_RANGE *result_lr = LR_For_TN (tn);
      if (LR_first_def(result_lr) == use_opnum)
#ifdef TARG_ST
	regs_needed -= TN_nhardregs (tn);
#else
        regs_needed--;
#endif
    }
  }
  for (i = 0; i < OP_opnds(use_op); i++) {
    TN *tn = OP_opnd(use_op, i);
    if (!TN_is_const_reg(tn) && TN_is_register(tn) && (TN_register_class(tn) == cl)) {
       LIVE_RANGE *lr = LR_For_TN (tn);
#ifdef TARG_ST
       if (LR_last_use(lr) == use_opnum) regs_needed += TN_nhardregs (tn);
#els
       if (LR_last_use(lr) == use_opnum) regs_needed++;
#endif
    }
  }
  if (regs_needed <= 0) return FALSE;

  *moveto = tgt_opnum;
  return TRUE;
}


/* Check if it is legal and beneficial to move the instruction that is
 * the first definition of the live range <lr>. 
 */
static BOOL
Can_Def_Be_Moved (
  LIVE_RANGE *lr,
  INT spill_opnum,
  INT *moveto)
{
  ARC_LIST *arcs;
  INT def_opnum = LR_first_def(lr);
  OP *def_op = OP_VECTOR_element (Insts_Vector, def_opnum);
  ISA_REGISTER_CLASS cl = TN_register_class (LR_tn(lr));
  INT tgt_opnum = LR_last_use(lr);

#ifdef TARG_ST
  // [SC] What if def_op defines a multi-register global TN?
  // def_op will be the op that defines the TN.
  // Setting tgt_opnum = LR_Last_use(lr) is somewhat arbitrary:
  // it finds an initial maximum on the distance we can move def_op,
  // but this is refined later by iterating round all OP_succs(def_op)
  // which will take into account the uses of all the physical
  // registers assigned by def_op.
  // In principle, we get more benefit by moving a def_op of a
  // multi-register TN, because it reduces register pressure more,
  // but that refinement is not implemented here.
#endif
  if (OP_side_effects(def_op)) return FALSE;
#ifdef TARG_ST
  if (OP_Has_Flag_Effect(def_op)) return FALSE;
#endif
  if (OP_flag1 (def_op)) return FALSE;
  if (LR_def_cnt(lr) != 1) return FALSE;
  if (tgt_opnum <= spill_opnum) return FALSE;

  for (arcs = OP_succs(def_op); arcs != NULL; arcs = ARC_LIST_rest(arcs)) {
    ARC *arc = ARC_LIST_first(arcs);
    OP *succ_op = ARC_succ(arc);
    // Prevent moving below a just moved use:
    if (OP_flag1(succ_op)) return FALSE;
    INT succ_opnum = BB_OP_MAP32_Get (op_to_opnum, succ_op);
    // Determine the first use after this def:
    if (succ_opnum < tgt_opnum) tgt_opnum = succ_opnum;
  }

  INT i;
  for (i = 0; i < OP_opnds(def_op); i++) {
    TN *tn = OP_opnd(def_op, i);

    if (!TN_is_const_reg(tn)) {
      /* if an operand is an exposed use of a result tn, moving the 
       * definition is of no use.
       */
      for (INT j = 0; j < OP_results(def_op); j++) {
        if (OP_result(def_op, j) == tn) return FALSE;
      }

      /* adjust the tgt_opnum to be MIN(tgt_opnum, opnd_last_use) */
      if (TN_is_register(tn) && TN_register_class(tn) == cl) {
         LIVE_RANGE *opnd_lr = LR_For_TN (tn);
         INT opnd_last_use = LR_last_use(opnd_lr);
         if (opnd_last_use < tgt_opnum) tgt_opnum = opnd_last_use;
#ifdef TARG_ST
	 // [SC]: If OPND_LR is the live range of an assigned or
	 // dedicated global TN, then it is possible that there are
	 // other live ranges assigned to the same register.
	 // However, these other TNs cannot interfere with OPND_LR,
	 // so it is not necessary to take them into account here.
	 // But in principle, we could do something like:
	 // if (LRA_TN_register(tn) != REGISTER_UNDEFINED) {
	 //   FOR_ALL_NREGS (LRA_TN_register(tn), TN_nhardregs(tn), r) {
	 //     INT u = ded_reg_last_use(r);
	 //     tgt_opnum = MIN (tgt_opnum, ded_reg_last_use(r));
	 // }
#endif
      }
    }
  }

  /* check if the tgt_opnum has already been marked as being moved. */
  if (tgt_opnum != VECTOR_count(Insts_Vector)) {
    while (tgt_opnum > spill_opnum) {
      OP *op = OP_VECTOR_element (Insts_Vector, tgt_opnum);
      if (!OP_flag1(op)) break;
      tgt_opnum--;
    }
  }

  if (tgt_opnum <= spill_opnum) return FALSE;

  *moveto = tgt_opnum;
  return TRUE;
}


/* Check if it is legal and beneficial to reorder some instructions in
 * the live range. This can either mean moving the definition down OR
 * moving the last use of a live range up. This effectively shortens
 * the live range.
 */
static void 
Analyze_Reordering (
  LIVE_RANGE *lr, 
  SPILL_CANDIDATE *best,
  INT spill_opnum)
{
  INT moveto;

  INT use_opnum = LR_last_use(lr);

  if (LR_use_cnt(lr) != 0 &&
      TN_is_local_reg(LR_tn(lr)) &&
      Can_Use_Be_Moved (lr, spill_opnum, &moveto) &&
      (best->spill_kind == SPILL_NONE || 
       best->benefit < (spill_opnum - moveto)))
  {
    best->spill_kind = SPILL_MOVE_USE;
    best->u1.s2.move_lr = lr;
    best->u1.s2.from = use_opnum;
    best->u1.s2.to = moveto;
    best->cost = 0.0;
    best->benefit = spill_opnum - moveto;
  }

  INT def_opnum = LR_first_def(lr);

  if (LR_def_cnt(lr) != 0 &&
      def_opnum < spill_opnum &&
      Can_Def_Be_Moved (lr, spill_opnum, &moveto) &&
      (best->spill_kind == SPILL_NONE ||
       best->benefit < (spill_opnum - def_opnum)))
  {
    best->spill_kind = SPILL_MOVE_DEF;
    best->u1.s2.move_lr = lr;
    best->u1.s2.from = def_opnum;
    best->u1.s2.to = moveto;
    best->cost = 0.0;
    best->benefit = spill_opnum - def_opnum;
  }

  /* TODO: look at the case where it is not enough to move just one end
   * of the live range. We can try moving the whole live range in that
   * case.
   */
}

static void
Move_Def (BB *bb, SPILL_CANDIDATE *best)
{
  INT from = best->u1.s2.from;
  INT to = best->u1.s2.to;
  OP *from_op = OP_VECTOR_element (Insts_Vector, from);

  if (Do_LRA_Trace(Trace_LRA_Spill)) {
    fprintf (TFile, "LRA_SPILL>> move def %d to before %d\n", from, to);
  }
  BB_Remove_Op (bb, from_op);
  if (to < VECTOR_size(Insts_Vector)) {
    OP *to_op = OP_VECTOR_element (Insts_Vector, to);
    BB_Insert_Op_Before (bb, to_op, from_op);
  }
  else {
    OPS ops = OPS_EMPTY;
    OPS_Append_Op (&ops, from_op);
    CGSPILL_Append_Ops (bb, &ops);
  }
}


static void
Move_Use (BB *bb, SPILL_CANDIDATE *best)
{
  INT from = best->u1.s2.from;
  INT to = best->u1.s2.to;
  OP *from_op = OP_VECTOR_element (Insts_Vector, from);
  OP *to_op = OP_VECTOR_element (Insts_Vector, to);

  if (Do_LRA_Trace(Trace_LRA_Spill)) {
    fprintf (TFile, "LRA_SPILL>> move use %d to after %d\n", from, to);
  }
  BB_Remove_Op (bb, from_op);
  BB_Insert_Op_After (bb, to_op, from_op);
}


// Go through the list of move <transformations> for this basic block
// and apply them all in one pass.
static void 
Apply_Move_Transformations ( BB *bb, SPILL_CANDIDATE *transformations)
{
  SPILL_CANDIDATE *cand;

  for (cand = transformations; cand != NULL; cand = cand->next) {
    switch (cand->spill_kind) {
      case SPILL_MOVE_DEF:
        Move_Def (bb, cand);
        break;
      case SPILL_MOVE_USE:
        Move_Use (bb, cand);
        break;
    }
  }
}


/* Remove the live ranges for the result tn and the operand tns for
 * the OP being moved.
 */
static void
Remove_LRs_For_OP (OP *op)
{
  INT i;
  if (op == NULL) return;
  for (i = 0; i < OP_results(op); i++) {
#ifdef TARG_ST
    Delete_LR_and_aliases (Live_LRs_Vector, LR_For_TN (OP_result(op, i)));
#else
    VECTOR_Delete_Element (Live_LRs_Vector, LR_For_TN (OP_result(op, i)));
#endif
  }
  for (i = 0; i < OP_opnds(op); i++) {
    TN *opnd_tn = OP_opnd(op,i);
    if (TN_is_register(opnd_tn)) {
#ifdef TARG_ST
      Delete_LR_and_aliases (Live_LRs_Vector, LR_For_TN(opnd_tn));
#else
      VECTOR_Delete_Element (Live_LRs_Vector, LR_For_TN(opnd_tn));
#endif
    }
  }
}


static void
Update_Fat_Points (SPILL_CANDIDATE *best, INT spill_opnum)
{
  LIVE_RANGE *lr;
  INT low;
  BOOL global_lr_spill;
#ifdef TARG_ST
  INT nregs;
#endif

  switch (best->spill_kind) {
  case SPILL_LIVE_RANGE:
    lr = best->u1.spill_lr;
    global_lr_spill = !TN_is_local_reg(LR_tn(lr));
#ifdef TARG_ST
    nregs = TN_nhardregs(LR_tn(lr));
#endif
    if (LR_exposed_use(lr)) {
      low = LR_first_spill(lr);
    } else {
      low = LR_first_def(lr) + 1;
    }
#ifdef TARG_ST
    Delete_LR_and_aliases (Live_LRs_Vector, lr);
#else
    VECTOR_Delete_Element (Live_LRs_Vector, lr);
#endif
    break;

  case SPILL_MOVE_DEF:
  case SPILL_MOVE_USE:
    OP *from_op = OP_VECTOR_element (Insts_Vector, best->u1.s2.from);
    Remove_LRs_For_OP (from_op);
    /* mark this flag as having been moved. */
    Set_OP_flag1 (from_op);
    if (best->u1.s2.to != VECTOR_count(Insts_Vector)) {
      OP *to_op = OP_VECTOR_element (Insts_Vector, best->u1.s2.to);
      Set_OP_flag1 (to_op);
    }
    if (best->spill_kind == SPILL_MOVE_DEF) {
      low = best->u1.s2.from;
    }
    else {
      low = best->u1.s2.to + 1;
    }
    lr = best->u1.s2.move_lr;
#ifdef TARG_ST
    nregs = TN_nhardregs(LR_tn(lr));
#endif
    break;
  }

  INT i;
  for (i = low; i <= spill_opnum; i++) {
    //
    // if spilled tn is used at this op, then there is no reduction in its
    // register usage.  if we're at the spill_opnum, and we've spilled a 
    // global, then we'll have a reference to the global register (we make
    // the local a dedicated tn).  allow the decrement in this case.
    //
#ifdef TARG_ST
    // [SC] Live range spilling may have completely removed the original
    // TN, and replaced with new TNs.  We detect these by testing against
    // the list of replacement TNs for the live range.
    if (!(Op_Uses_TN(LR_tn(lr), i) ||
	  Op_Uses_spill_tn(LR_spill_tns(lr), i))
	|| (i == spill_opnum && global_lr_spill)) {
      fat_points[i]-= nregs;
    }
#else
    if (!Op_Uses_TN(LR_tn(lr), i) || (i == spill_opnum && global_lr_spill)) {
      fat_points[i]--;
    }
#endif
  }
}

#ifdef TARG_ST
/* Return TRUE if OP references (reads) the TN we may
   want to spill.  If the TN is global, it may have aliases
   (i.e. other global TNs allocated to the same register).
*/
static BOOL
OP_Refs_Spill_TN (OP *op, TN *spill_tn)
{
  if (TN_is_local_reg (spill_tn)) {
    return OP_Refs_TN (op, spill_tn);
  } else {
    // For a global reg TN, return true only if it is allocated
    // to exactly the same set of registers as the spill_tn.
    // All other TNs that overlap the allocated registers can be
    // treated as independent.
    for (INT num = 0; num < OP_opnds(op); num++) {
      TN *opnd_tn = OP_opnd(op, num);
      if (TN_is_register(opnd_tn) && TNs_alias_p (opnd_tn, spill_tn)) {
	return TRUE;
      }
    }
  }
  return FALSE;
}

/* Return TRUE if OP defines the TN we may
   want to spill.  If the TN is global, it may have aliases
   (i.e. other global TNs allocated to the same register).
*/
static BOOL
OP_Defs_Spill_TN (OP *op, TN *spill_tn)
{
  if (TN_is_local_reg (spill_tn)) {
    return OP_Defs_TN (op, spill_tn);
  } else {
    // For a global reg TN, return true only if it is allocated
    // to exactly the same set of registers as the spill_tn.
    // All other TNs that overlap the allocated registers can be
    // treated as independent.
    for (INT num = 0; num < OP_results(op); num++ ) {
      if (TNs_alias_p (OP_result (op, num), spill_tn)) {
	return  TRUE;
      }
    }
  }
  return FALSE;
}
#endif

//
// determine if the current op is the definition of one of the operands
// of the current fatpoint that we're spilling for.  if it is, we want
// to see if it was unallocatable during the fat point calculations.  
//
static BOOL
At_Unallocated_Op_Definition(INT fat_opnum, INT cur_opnum,
			     ISA_REGISTER_CLASS spill_cl)
{
  INT opndnum;
  OP *op = OP_VECTOR_element (Insts_Vector, fat_opnum);
  for (opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
    TN *tn = OP_opnd(op, opndnum);
    if (tn == NULL || TN_is_constant(tn)) {
      continue;
    }
    LIVE_RANGE *lr = LR_For_TN(tn);
    // lr null if inserted for previous spill.  we're at an unallocated definition
    // if its the last use, another global has not been spilled for it, and the
    // current op is its definition.
    if (TN_register_class(tn) == spill_cl && lr != NULL
	&& !LR_fat_allocated(lr) && LR_last_use(lr) == fat_opnum &&
	LR_first_def(lr) == cur_opnum) {
      return(TRUE);
    }
  }
  return(FALSE);
}

/* This procedure tracks exactly what will happen in Spill_Live_Range to
 * get an accurate estimate of the spill-cost.
 */
static void
Analyze_Spilling_Live_Range (
  LIVE_RANGE *spill_lr,
  INT spill_opnum,
  SPILL_CANDIDATE *best,
  INT fatpoint)
{
  INT spill_fatpoint = fatpoint_min;
  TN *spill_tn = LR_tn(spill_lr);
#ifdef TARG_ST
  INT nregs = TN_nhardregs (spill_tn);
  INT32 exposed_use = 0;
  BOOL live_out = FALSE;
#endif
  BOOL is_local_tn = TN_is_local_reg (spill_tn);
  ISA_REGISTER_CLASS spill_cl = TN_register_class(spill_tn);
  REGISTER spill_reg = REGISTER_UNDEFINED;
  ST *spill_loc;
  BOOL def_available = FALSE;
  BOOL pending_store = FALSE;
  BOOL spill_store_deleted = FALSE;
  BOOL already_spilled = (TN_spill(spill_tn) == NULL) ? FALSE : TRUE;
#ifdef TARG_ST
  BOOL reloadable = Is_LR_Reloadable(spill_lr);
#else
  BOOL reloadable = (already_spilled || Is_LR_Reloadable(spill_lr)) ? TRUE : FALSE;
#endif
  float store_cost, restore_cost;
  float cost = 0.0;
  INT benefit = 0;
  INT cur_benefit = 0;
#ifdef TARG_ST
  INT benefit_since_last_use = 0;
#endif
  INT first_def = LR_first_def(spill_lr);
  INT last_use = LR_last_use(spill_lr);
  INT last_def = 0;
  INT last_use_seen = 0;
  INT def_cnt = 0;
  BOOL lr_spans_spill_opnum = FALSE;
  BOOL upward_exposed_global = FALSE;
  
  CGSPILL_Cost_Estimate (spill_tn, NULL, &store_cost, &restore_cost, CGSPILL_LRA);

  // If the rematerialization cost is too high, reset that flag and
  // spill it like an ordinary TN instead. There is also a problem in 
  // the rematerialization sequence for a 64bit immediate load. It
  // uses and extra register which defeats the purpose of spilling
  // the live range. So, we need to reset rematerialization flag for
  // that reason as well.
  if (restore_cost > 3.0 && TN_is_rematerializable(spill_tn)) {
    Reset_TN_is_rematerializable (spill_tn);
    Set_TN_home(spill_tn, NULL);
    CGSPILL_Cost_Estimate (spill_tn, NULL, &store_cost, &restore_cost, CGSPILL_LRA);
  }

  spill_loc = TN_spill(spill_tn);

  //
  // Yikes!  If this TN was created to spill another TN,
  // try real hard not to spill it again.
  if (spill_loc != NULL
#ifdef KEY
      && ! TN_is_rematerializable(spill_tn)
#endif
	  ) cost += 16.0;
#ifdef TARG_ST
  // [SC] Do not consider spilling a live range referenced in a
  // singleton subclass operand.  This is because we assert (in cgemit.cxx,
  // Verify_Operand()), that all such operands use dedicated TNs.
  // [TTh] Conservatively extended the check to aliases
  FOR_LR_AND_ALL_ALIASES(spill_lr, alias_lr) {
    if (LR_singleton_ref(alias_lr)) {
      return;
    }
  } FOR_LR_AND_ALL_ALIASES_END(spill_lr, alias_lr);
#endif

  //
  // if this is a large stack size integer spill, then we'll require
  // an additional register to calculate the offset (don't have this
  // problem with loads, because we can use the register that we're
  // loading into for the offset).  need to reduce the minimum fat point
  // at which we'll force a pending store into memory.  note that this
  // isn't quite right for locals ... they'll not have had spill temps
  // allocated for them yet.  its not a really big deal, though, because
  // the spills will just happen sooner and will free up more registers
  // than the estimates (freeing less is *bad*).
  //
#ifdef TARG_ST
  // Arthur: get rid of target dependency
  if (REGISTER_CLASS_is_ptr(TN_register_class(spill_tn))
#else
  if (TN_register_class(spill_tn) == ISA_REGISTER_CLASS_integer 
#endif
      && !TN_is_rematerializable(spill_tn) && !TN_is_gra_homeable(spill_tn)
      && Exp_Is_Large_Stack_Sym(spill_loc, 0)) {
    spill_fatpoint--;
  }

  if (reloadable) store_cost = 0.0;

  if (!is_local_tn) {
    spill_reg = LRA_TN_register(spill_tn);
#ifdef TARG_ST
    FOR_LR_AND_ALL_ALIASES (spill_lr, alias_lr) {
      if (LR_exposed_use(alias_lr) > exposed_use) {
	exposed_use = LR_exposed_use(alias_lr);
      }
      if (LR_first_def(alias_lr) < first_def) {
	first_def = LR_first_def(alias_lr);
      }
      if (LR_last_use(alias_lr) > last_use) {
	last_use = LR_last_use(alias_lr);
      }
    } FOR_LR_AND_ALL_ALIASES_END (spill_lr, alias_lr);

    if (exposed_use) {
      upward_exposed_global = TRUE;
      def_available = TRUE;
      pending_store = TRUE;
      first_def = 1;
      last_def = 0;  // def is outside the bb.
      if (last_use == 0) {
	// [TTh] Handling of live-in GTN not redefined in BB:
	// the last exposed use is also the last use.
	last_use = exposed_use;
      }
    }
    if (last_use == VECTOR_size(Insts_Vector)) { live_out = TRUE; last_use--; }
#else
    if (LR_exposed_use(spill_lr)) {
      def_available = TRUE;
      pending_store = TRUE;
      first_def = 1;
      upward_exposed_global = TRUE;
      last_def = 0;  // def is outside the bb.
    }
    if (last_use == VECTOR_size(Insts_Vector)) last_use--;
#endif
  }

  for (INT i = first_def; i <= last_use; i++) 
  {
    OP *op = OP_VECTOR_element(Insts_Vector, i);
    if (op == NULL) continue;

#ifdef TARG_X8664
    // Don't spill the live range if it is a dedicated register that is
    // preallocated to some OP's operand or result.  Spilling such a live range
    // is useless because Preallocate_Single_Register_Subclasses would
    // preallocate the register back to the operand/result.  Bug 5744.
    for (INT opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
      TN* opnd_tn = OP_opnd(op, opndnum);
      if (TN_is_register(opnd_tn) &&
	  TN_is_preallocated(opnd_tn) &&
	  TNs_Are_Equivalent(opnd_tn, spill_tn)) {
	    return;
      }
    }
    for (INT resnum = 0; resnum < OP_results(op); resnum++) {
      TN* res_tn = OP_result(op, resnum);
      if (TN_is_register(res_tn) &&
	  TN_is_preallocated(res_tn) &&
	  TNs_Are_Equivalent(res_tn, spill_tn)) {
	    return;
      }
    }
#endif

#ifdef TARG_MIPS
    // Don't spill the register operand of "jalr" because it might be reloaded
    // into $31, which is not allowed.  SiCortex 4744.
    if (OP_code(op) == TOP_jalr) {
      TN* opnd_tn = OP_opnd(op, 0);
      if (TNs_Are_Equivalent(opnd_tn, spill_tn))
	return;
    }
#endif

    BOOL at_fatpoint = (fat_points[i] > spill_fatpoint && i <= spill_opnum);

    // If the register usage at this point is greater than the fatpoint
    // and if there is a pending store, insert the store now.
    if (at_fatpoint) {
#ifdef TARG_ST
      // [SC] We get benefit if we are at a fatpoint AND the tn is still
      // live.  For tns with multiple defs, we do not know for sure if
      // they are live at this instruction, so accumulate the benefit
      // into benefit_since_last_use.  When we have proof that the tn is
      // live we move it from benefit_since_last_use to cur_benefit.
      benefit_since_last_use += nregs;
#else
      cur_benefit++;
#endif
      if (pending_store) {
        cost += store_cost;
        pending_store = FALSE;
	upward_exposed_global = FALSE;
      }
    }

    // For local TNs, check if the current OP references the TN we want to 
    // spill. For global TNs, check if the current OP references the register
    // assigned to the global TN.
#ifdef TARG_ST
    if (OP_Refs_Spill_TN (op, spill_tn)
	&& ! OP_xfer (op)) {
#else
    if (Op_Uses_TN(spill_tn, i)) {
#endif
#ifdef TARG_ST
      // [SC] tn is still live.
      cur_benefit = benefit_since_last_use;
      benefit_since_last_use = 0;
#endif
      //
      // check if the live range spans the spill_opnum.  get out if
      // if the spill_opnum references the live range.
      //
      if (i == spill_opnum) {
	return;
      } else if (i > spill_opnum && last_def < spill_opnum) {
        lr_spans_spill_opnum = TRUE;
      }

      if (!reloadable && Is_OP_Spill_Store (op, spill_loc) &&
          !already_spilled && (i == last_use)) cost += 1.0;

      // if this is an op that modifies one of its operands, then we
      // will have to insert a copy to ensure that we free up a register
      // when we spill it (otherwise, it'll be tied up between the first
      // definition of the modified tn and this op).  account for that
      // by adding 1 to it (we really should use something from targ_info
      // for this).
      //
#ifdef TARG_ST
      // Arthur: as usual ...
      BOOL same_res = FALSE;
      INT res;
      for (res = 0; res < OP_results(op); res++) {
	if (OP_same_res(op,res) >= 0) {
	  same_res = TRUE;
	  break;
	}
      }

      if (same_res || OP_cond_def(op)) {
#else
      if (OP_same_res(op) || OP_cond_def(op)) {
#endif
	cost += 1.0;
      }

      benefit += cur_benefit;
      cur_benefit = 0;
      last_use_seen = i;
      if (!reloadable && (already_spilled && Is_OP_Spill_Store (op, spill_loc))) {
        // If the use of spill_tn is a store to the spill location, we 
        // don't have to load from memory. Actually, we can get rid
        // of the store as well, since the memory contents are already
        // current.
        spill_store_deleted = TRUE;
        cost -= store_cost;
#ifdef TARG_ST
        if (exposed_use == i) def_available = FALSE;
#else
        if (LR_exposed_use(spill_lr) == i) def_available = FALSE;
#endif
        continue;
      }

      // If there is no definition available, reload from spill location.
      if (!def_available) {
        def_available = TRUE;
        cost += restore_cost;
      }
#ifdef TARG_ST
      if (exposed_use == i) def_available = FALSE;
#else
      if (LR_exposed_use(spill_lr) == i) def_available = FALSE;
#endif
    }

    // Check if the current OP is a definition of the spill tn.
#ifdef TARG_ST
    if (OP_Defs_Spill_TN (op, spill_tn))
#else
    if ((TN_is_local_reg (spill_tn) && 
         OP_Defs_TN (op, spill_tn)) ||
        (!TN_is_local_reg (spill_tn) && 
         OP_Defs_Reg (op, spill_cl, spill_reg)))
#endif
    {
      def_cnt++;
      if (def_cnt > 1 && last_use_seen < spill_opnum && i > spill_opnum) {
	//
	// if this had multiple definitions, and we're not yet to the last
	// one, then we need to add the benefit gained from spilling the
	// live range between the last use of the previous definition and
	// this definition.  
	//
	benefit += cur_benefit;
	lr_spans_spill_opnum = TRUE;
      }
      last_def = i;
      cur_benefit = 0;
#ifdef TARG_ST
      if (OP_cond_def(op)) {
        if (!def_available) {
          // [TTh] For cond_def results, the previous value must be available
          cost += restore_cost;
        }
      }
      else {
        benefit_since_last_use = 0;
      }
      if (reloadable || 
	  (already_spilled && Is_OP_Spill_Load (op, spill_loc)) && (i != first_def)) {
#else
      if (reloadable || (already_spilled && Is_OP_Spill_Load (op, spill_loc))) {
#endif
        // If the definition of the spill_tn is a load from the spill
        // location, we don't have to insert the store to memory. Actually
        // we can get rid of the load as well, since we will be loading 
        // from the spill location before each use of spill_tn.
        cost -= restore_cost;
        continue;
      }

      def_available = TRUE;
      pending_store = TRUE;
      upward_exposed_global = FALSE;
    }
    else if (already_spilled && Is_OP_Spill_Load(op, spill_loc) && pending_store &&
	     spill_store_deleted) {
      //
      // must put spill back if we spilled a global, and are now reloading
      // it back to a local.
      //
      cost += store_cost;
      pending_store = FALSE;
    }
    else if (at_fatpoint || (upward_exposed_global &&
	     At_Unallocated_Op_Definition(spill_opnum, i, spill_cl))) {
      // If the current instruction is a fatpoint, or we're at the definition
      // of an unallocated operand of the fatpoint, make any earlier
      // definition unavailable.  we need to do this for globals at exposed
      // uses since the exposed use will take the register allocated to the
      // global, and this is the register we've made available at the fat
      // point (uses of the global after spill reloads will use whatever
      // register is available at that point).
      def_available = FALSE;

      if (upward_exposed_global) {
	cost += store_cost;
	pending_store = FALSE;
	upward_exposed_global = FALSE;
      }
    }
  }

  // If there was a use that is a store to a spill location it has
  // been deleted. So, if there is still a pending_store at this point,
  // go back and insert the spill store after the last def. Otherwise,
  // the spill location will not get updated.
  if (pending_store && spill_store_deleted) {
    cost += store_cost;
    pending_store = FALSE;
  }

  // Check if spill_tn is live-out from this basic block. If it is,
  // we need to create a definition of spill_tn.
  if (LR_last_use(spill_lr) == VECTOR_size(Insts_Vector)) {
#ifdef TARG_ST
    cur_benefit += benefit_since_last_use;
    benefit_since_last_use = 0;
#endif
    // check if the live range spans the spill_opnum.
    if (last_def < spill_opnum) {
      lr_spans_spill_opnum = TRUE;
    }

    benefit += cur_benefit;
    if (!def_available) {
      cost += restore_cost;
    }
    else {
      // even if there is a def available, we still need to insert a COPY
      // which may not get deleted.
#ifdef TARG_ST
      // [CG] If the last def is a copy the copy will be removed
      OP *last_def_op = OP_VECTOR_element(Insts_Vector, last_def);
      if (last_def_op && OP_Copy_Operand(last_def_op) >= 0) {
	cost += 0.0;
      } else
#endif
      cost += 0.5;
    }
  }

  if (Do_LRA_Trace(Trace_LRA_Spill)) {
    fprintf (TFile, "Analyze >> (cost:%g, benefit:%d, spans spill:%d)", 
        cost, benefit, lr_spans_spill_opnum);
    Print_Live_Range (spill_lr);
  }

  if (lr_spans_spill_opnum &&
      benefit > 0 &&
      (best->spill_kind == SPILL_NONE ||
       (float)(benefit - best->benefit) > 16.0 * (cost - best->cost)
#ifdef TARG_ST
       // [TTh] Additional criterion to select LR to spill:
       // In case of identical benefit/cost balances, prefer spilling the LR
       // with the earliest definition: it is likely to span more spill points
       // and spilling it might reduce register pressure on more locations.
       || (best->spill_kind == SPILL_LIVE_RANGE &&
           KnuthCompareEQ((float)(benefit - best->benefit), 16.0 * (cost - best->cost)) &&
           first_def < LR_first_def(best->u1.spill_lr)
           )
#endif
      ))
  {
    best->spill_kind = SPILL_LIVE_RANGE;
    best->u1.spill_lr = spill_lr;
    best->cost = cost;
    best->benefit = benefit;
  }
}


static void
Add_Spill_Store_After_Def (TN *tn, INT def, BB *bb)
{
  OPS spill_ops = OPS_EMPTY;
  ST *spill_loc = TN_spill(tn);

  CGSPILL_Store_To_Memory (tn, spill_loc, &spill_ops, CGSPILL_LRA, bb);
  // Insert the store right after the previous definition.
  if (def != 0) {
    OP *def_op = OP_VECTOR_element(Insts_Vector, def);
    CGSPILL_Insert_Ops_After (bb, def_op, &spill_ops);
    if (Do_LRA_Trace(Trace_LRA_Spill)) {
      fprintf (TFile, "LRA_SPILL>>    store TN%d after %d\n", TN_number(tn), def);
    }
  }
  else {
    CGSPILL_Prepend_Ops (bb, &spill_ops);
    if (Do_LRA_Trace(Trace_LRA_Spill)) {
      fprintf (TFile, "LRA_SPILL>>    store TN%d at top of bb\n", TN_number(tn));
    }
  }
}


static void
Add_Spill_Load_Before_Use (TN *tn, ST *spill_loc, OP *reload_op, INT use, BB *bb)
{
  OPS spill_ops = OPS_EMPTY;

  if (reload_op) {
    OP *new_op = Dup_OP (reload_op);
    Copy_WN_For_Memory_OP (new_op, reload_op);
    //
    // This assumes that the first result is the destination of the 
    // load.  This should probably be done via some interface, but
    // I'm leaving it for now.
    //
    Set_OP_result (new_op, 0 /*???*/, tn);
    OPS_Append_Op (&spill_ops, new_op);
  }
#ifdef KEY
  else if (TN_is_rematerializable(tn) || TN_is_gra_homeable(tn)) {
    CGSPILL_Load_From_Memory (tn, (ST*)TN_home(tn), &spill_ops, 
			      CGSPILL_GRA, bb);
  }
#endif
  else {
    FmtAssert(spill_loc != NULL, 
	      ("LRA: Add_Spill_Load_Before_Use: spill_loc cannot be NULL\n"));
    Set_TN_spill(tn, spill_loc);
    CGSPILL_Load_From_Memory (tn, spill_loc, &spill_ops, CGSPILL_LRA, bb);
  }

  if (use == VECTOR_size(Insts_Vector)) {
    // append at at of bb.
    CGSPILL_Append_Ops (bb, &spill_ops);
    if (Do_LRA_Trace(Trace_LRA_Spill)) {
      fprintf (TFile, "LRA_SPILL>>    load TN%d at end of bb\n", TN_number(tn));
    }
  }
  else {
    OP *use_op = OP_VECTOR_element(Insts_Vector, use);
    CGSPILL_Insert_Ops_Before (bb, use_op, &spill_ops);
    if (Do_LRA_Trace(Trace_LRA_Spill)) {
      fprintf (TFile, "LRA_SPILL>>    load TN%d before %d\n", TN_number(tn), use);
    }
  }

}


static void
Replace_TN_References (TN *old_tn, TN *new_tn, OP *op)
{
  // replace all references to spill_tn in the op to new_tn.
  for (INT opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
    TN *opnd_tn = OP_opnd(op, opndnum);
    if (!TN_is_register(opnd_tn)) continue;
    ISA_REGISTER_CLASS old_cl = TN_register_class(old_tn);
    REGISTER old_reg = LRA_TN_register(old_tn);
#ifdef TARG_ST
    REGISTER old_nregs = TN_nhardregs(old_tn);
#endif
    if (opnd_tn == old_tn ||
        (!TN_is_local_reg(old_tn) && 
         TN_register_class(opnd_tn) == old_cl &&
#ifdef TARG_ST
	 TN_nhardregs(opnd_tn) == old_nregs &&
#endif
         LRA_TN_register(opnd_tn) == old_reg))
    {
      Set_OP_opnd(op, opndnum, new_tn);
    }
  }

  // Mark the live range of the result TN as not reloadable. Otherwise,
  // if we later decide to spill that live range, we can introduce new
  // references to the TN we are in the process of spilling.
  for (INT i = 0; i < OP_results(op); i++) {
    TN *result_tn = OP_result(op, i);
    LIVE_RANGE *lr = LR_For_TN (result_tn);
    if (lr != NULL)
      Reset_LR_reloadable (lr);
  }
}


/* ======================================================================
 * Spill_Live_Range
 * 
 * Spill a live-range in the <bb>. It inserts stores after each 
 * definition of the live-range and inserts a load before each use. The 
 * following optimizations are performed:
 *
 *  1. If a defintion in the live-range is a load from the spill location, 
 *     don't generate the store and delete this defintion.
 *  2. If the use in the live-range is a store to the spill location, don't 
 *     insert a load before the use. Delete the existing store.
 *  3. If the use is very close to an earlier def/use, don't reload
 *     from the spill location.
 *
#ifdef KEY
 * SPILLPOINT, if non-NULL, is the function to call to determine if the live
 * range should be spilled around an OP.  It determines where to spill in lieu
 * of fat points.
#endif
 *
 * ======================================================================*/
static void
Spill_Live_Range (
  BB *bb, 
  LIVE_RANGE *spill_lr,
  INT fatpoint,
  INT spill_opnum,
  BOOL (*spillpoint)(TN*, OP*) = NULL)
{
#ifdef TARG_ST
  // SPILL_REG:
  //   If spilling a global live range, spill_reg is the register that the
  //   global live range is allocated to.  If live-in, spill_reg contains
  //   the value on entry to the BB.  If live-out, spill_reg must contain
  //   the value on exit from the BB.
  // LIVE_OUT:
  //   True if value is live out of BB, false otherwise.
  // NEW_TN:
  //   when the value is in a register, new_tn is the tn for that register.
  // FIRST_DEF:
  //   start of live range.  1 if value is live into this BB.
  // LAST_USE:
  //   end of live range.  BB_len - 1 if value is live out of this BB.
  //   For a global TN, live on entry to the BB, but not live on exit from
  //   the TN, LAST_USE may be zero, but EXPOSED_USE gives the last use in
  //   the BB.
  // PENDING_STORE:
  //   pending_store is true if the value is not up to date in the spill location.
  //   false otherwise.
  // DEF_AVAILABLE:
  //   def_available is TRUE if the value is available in a register
  //   (the register tn is new_tn), false otherwise.
  // LAST_DEF:
  //   The most recently seen def of the value in the forward scan of the BB.
#endif

  INT spill_fatpoint = fatpoint_min;
  TN *spill_tn = LR_tn(spill_lr);
  BOOL is_local_tn = TN_is_local_reg (spill_tn);
  ISA_REGISTER_CLASS spill_cl = TN_register_class(spill_tn);
  REGISTER spill_reg = REGISTER_UNDEFINED;
#ifdef TARG_ST
  INT spill_nregs = TN_nhardregs(spill_tn);
  BOOL live_out = FALSE;
  INT32 exposed_use = 0;
#endif
  ST *spill_loc = NULL;
  ST *sv_spill_location = NULL;
  BOOL magic_spill_used = FALSE;
  BOOL upward_exposed_global = FALSE;
  TN *new_tn = NULL;
  INT first_def;
  INT last_use;
  BOOL pending_store = FALSE;
  BOOL spill_store_deleted = FALSE;
  BOOL def_available = FALSE;
#ifdef KEY
  const BOOL already_spilled = TN_has_spill(spill_tn);
#else
  BOOL already_spilled = (TN_spill(spill_tn) == NULL) ? FALSE : TRUE;
#endif
#ifdef TARG_ST
  //
  // Arthur: there seems to be a problem here.
  //         I do not understand it, just trying to make it work ...
  //
  BOOL reloadable = LR_reloadable(spill_lr);
#else
  BOOL reloadable = (already_spilled || LR_reloadable(spill_lr)) ? TRUE : FALSE;
#endif
  OP *reloadable_def = NULL;
  INT last_def;
  INT last_opnum;

#ifdef KEY
  last_def = 0;		// Bug 14315, uninitialized variable.
#endif
#ifdef TARG_ST
last_def = -1;
#endif


  if (Do_LRA_Trace(Trace_LRA_Spill)) {
    fprintf (TFile, "LRA_SPILL>> spill lr at OP:%d", spill_opnum);
    Print_Live_Range (spill_lr);
#ifdef TARG_ST
    if (LR_next_alias(spill_lr) != spill_lr) {
      fprintf (TFile, " GTN%d has aliases ", TN_number(LR_tn(spill_lr)));
      FOR_LR_AND_ALL_ALIASES (spill_lr, alias) {
	fprintf (TFile, " GTN%d", TN_number(LR_tn(alias)));
      } FOR_LR_AND_ALL_ALIASES_END(spill_lr, alias);
      fprintf (TFile, "\n");
    }
#endif    
  }

  first_def = LR_first_def(spill_lr);
  last_use = LR_last_use(spill_lr);
#ifndef KEY // this is problematic because Update_Live_LRs_Vector called
  if (reloadable) {
      	    // later can re-introduce those LRs, defeating the purpose of
	    // Remove_LRs_For_OP() (subroutine pset in 301.apsi)
    reloadable_def = OP_VECTOR_element(Insts_Vector, first_def);

    // mark the live ranges of operands of the reloadable_def as being
    // not available for spill. If we try to spill on of the operand
    // TNs later, we will not find the uses added by cloning the 
    // reloadable def.
    Remove_LRs_For_OP (reloadable_def);
  }
#else
  if( already_spilled ){
    spill_loc = (ST*)TN_home(spill_tn);
  }
#endif
  else {
    // Try using the magic symbol as a last ditch effort. It is 
    // guaranteed to be within 16 bits of sp/fp.
    if (Trip_Count == MAX_TRIP_COUNT &&
        Magic_Spill_Location != NULL &&
#ifdef TARG_ST
	TN_size (spill_tn) <= ST_size (Magic_Spill_Location))
#else
        !TN_is_float(spill_tn)) 
#endif
    {
      sv_spill_location = TN_spill(spill_tn);
      Set_TN_spill(spill_tn, Magic_Spill_Location);
      spill_loc = Magic_Spill_Location;
      magic_spill_used = TRUE;
      DevWarn ("Used short-offset spill location in BB%d", BB_id(bb));
    }
    else {
      /* kludge to fix bug 386428 */
      if (GP_TN != NULL && TN_is_rematerializable (spill_tn) && BB_exit (bb)) {
	LIVE_RANGE *gp_lr = LR_For_TN (GP_TN);
	if (gp_lr != NULL && 
	    LR_def_cnt(gp_lr) != 0 &&
	    last_use > LR_first_def(gp_lr))
	{
	  Reset_TN_is_rematerializable (spill_tn);
	  Set_TN_home(spill_tn, NULL);
	}
      }
      spill_loc = CGSPILL_Get_TN_Spill_Location (spill_tn, 
                        (is_local_tn) ? CGSPILL_LRA : CGSPILL_LGRA);
    }
  }

  //
  // if this is a large stack size integer spill, then we'll require
  // an additional register to calculate the offset (don't have this
  // problem with loads, because we can use the register that we're
  // loading into for the offset).  need to reduce the minimum fat point
  // at which we'll force a pending store into memory.
  //
#ifdef TARG_ST
  // Arthur: get rid of target dependency
  if (REGISTER_CLASS_is_ptr(TN_register_class(spill_tn))
#else
  if (TN_register_class(spill_tn) == ISA_REGISTER_CLASS_integer
#endif
      && !TN_is_rematerializable(spill_tn) && !TN_is_gra_homeable(spill_tn)
      && Exp_Is_Large_Stack_Sym(spill_loc, 0)) {
    spill_fatpoint--;
  }

  if (!is_local_tn) {
    spill_reg = LRA_TN_register(spill_tn);
#ifdef TARG_ST
    FOR_LR_AND_ALL_ALIASES (spill_lr, alias_lr) {
      if (LR_exposed_use(alias_lr) > exposed_use) {
	exposed_use = LR_exposed_use(alias_lr);
      }
      if (LR_first_def(alias_lr) < first_def) {
	first_def = LR_first_def(alias_lr);
      }
      if (LR_last_use(alias_lr) > last_use) {
	last_use = LR_last_use(alias_lr);
      }
    } FOR_LR_AND_ALL_ALIASES_END (spill_lr, alias_lr);
    if (exposed_use) {
      upward_exposed_global = TRUE;
      new_tn = spill_tn;
      Push_LR_spill_tns (spill_lr, new_tn, &lra_pool);
      def_available = TRUE;
      pending_store = TRUE;
      last_def = 0;  // def is outside the bb.
      first_def = 1;
      if (last_use == 0) {
	// [TTh] Handling of live-in GTN not redefined in BB:
	// the last exposed use is also the last use.
	last_use = exposed_use;
      }
    }
    if (last_use == VECTOR_size(Insts_Vector)) { live_out = TRUE; last_use--; }
#else
    if (LR_exposed_use(spill_lr)) {
      new_tn = spill_tn;
      def_available = TRUE;
      pending_store = TRUE;
      upward_exposed_global = TRUE;
      last_def = 0;  // def is outside the bb.
      first_def = 1;
    }
#endif
    if (last_use == VECTOR_size(Insts_Vector)) last_use--;
  }

  last_opnum = 0;
  INT i;
  for (i = first_def; i <= last_use; i++) 
  {
    OP *op = OP_VECTOR_element(Insts_Vector, i);
    if (op == NULL) continue;

    BOOL at_fatpoint;

#ifdef TARG_X8664
    // The spill-point function returns TRUE if TN needs to spill around OP.
    if (spillpoint)
      at_fatpoint = (*spillpoint)(spill_tn, op);
    else
#endif
    at_fatpoint = (fat_points[i] > spill_fatpoint && i <= spill_opnum);

    // If the register usage at this point is greater than the fatpoint
    // and if there is a pending store, insert the store now.
    if (pending_store && at_fatpoint) {
      Add_Spill_Store_After_Def (new_tn, last_def, bb);
      if (upward_exposed_global) {
	LR_first_spill(spill_lr) = last_def + 1;
	upward_exposed_global = FALSE;
      }
      pending_store = FALSE;
    }

    // For local TNs, check if the current OP references the TN we want to 
    // spill. For global TNs, check if the current OP references the register
    // assigned to the global TN.
#ifdef TARG_ST
    // [CG]: In addition, if it's a xfer op and a global tn and the xfer op does not
    // redefine the tn, we avoid reloading there. The code at the end of this function
    // will insert the reload operation before the exit of the block in this case.
    // This is an optimization which avoids an additional move operation at the end of the block.
    // It happens on branch or return operation that use a global tn which is also live out.
    // Though we must check that this operation does not also redefine the live out
    // tn, as it is the case for 'increment and branch' instruction such as jrgtudec on STxP70.
    if (OP_Refs_Spill_TN (op, spill_tn)
	&& !(live_out && OP_xfer(op) && !OP_Defs_Spill_TN(op, spill_tn)))
#else
    if ((is_local_tn && 
         OP_Refs_TN (op, spill_tn)) ||
        (!is_local_tn && 
         OP_Refs_Reg (op, spill_cl, spill_reg)))
#endif
    {
#ifdef TARG_ST
      // Arthur: If the use of spill_tn is a store to the spill location, we 
      //         don't have to load from memory. Actually, we can get rid
      //         of the store as well, since the memory contents are already
      //         current. 
      //         Don't delete if it's the last use.
      //         Don't delete if reloadable since there is no first store,
      //         i.e. memory is not up to date.
      //
      if (!reloadable && Is_OP_Spill_Store (op, spill_loc) &&
          already_spilled && (i != last_use)) {
#else
      if (!reloadable && Is_OP_Spill_Store (op, spill_loc) &&
          !already_spilled && (i == last_use)) {
#endif
        // If the use of spill_tn is a store to the spill location, we 
        // don't have to load from memory. Actually, we can get rid
        // of the store as well, since the memory contents are already
        // current.
        BB_Remove_Op (bb, op);
	Clobber_Op_Info(i, spill_cl);
        spill_store_deleted = TRUE;
        if (Do_LRA_Trace(Trace_LRA_Spill)) {
          fprintf (TFile, "LRA_SPILL>>    Removed spill store at %d\n", i);
          Print_OP_No_SrcLine(op);
        }
        if (LR_exposed_use(spill_lr) == i) {
	  def_available = FALSE;
	}
        continue;
      }

      // If there is no definition available, reload from spill location.
      if (!def_available) {
        def_available = TRUE;
        new_tn = Dup_TN_Even_If_Dedicated (spill_tn);
#ifdef TARG_X8664
	Reset_TN_is_preallocated(new_tn);
#endif

#ifdef KEY
	if (TN_has_spill(spill_tn))
#endif
	  Set_TN_spill(new_tn, spill_loc);
        Add_Spill_Load_Before_Use (new_tn, spill_loc, reloadable_def, i, bb);
      }
      else if (Do_LRA_Trace(Trace_LRA_Spill)) {
        fprintf (TFile, "LRA_SPILL>>    skipped load of TN%d before %d\n", 
                          TN_number(new_tn), i);
      }
      Replace_TN_References (spill_tn, new_tn, op);
#ifdef TARG_ST
      if (exposed_use == i) {
#else
      if (LR_exposed_use(spill_lr) == i) {
#endif
	def_available = FALSE;
      }
    }

    // Check if the current OP is a definition of the spill tn.
#ifdef TARG_ST
    if (OP_Defs_Spill_TN (op, spill_tn))
#else
    if ((TN_is_local_reg (spill_tn) && 
         OP_Defs_TN (op, spill_tn)) ||
        (!TN_is_local_reg (spill_tn) && 
         OP_Defs_Reg (op, spill_cl, spill_reg)))
#endif
    {
      if (i == VECTOR_size(Insts_Vector)-1) {
	// if the defining op is the last thing in the block, do nothing
	// with it as we'll give it the register and thus have the correct
	// value in the register upon exit.  i can't think of any situation
	// in which we get here, and we've made a sane spill choice (this
	// would typically come up if we're spilling $31, and the block is
	// terminated by a call.  in such a case, the dedicated tn for $31
	// is likely not used anywhere else in the block (unless this is the
	// prolog), so this spill is probably not going to reduce register
	// pressure.  if we don't handle it this way, though, the reload
	// of the register will be placed *before* the terminating instruction
	// in the block, and we'll get wrong code.  better stupid than
	// erroneous.
	last_def = i;
	continue;
      }
       /* Note: Add the check for !already_spilled to the following "if".
         The call to Is_OP_Spill_Load, detects a spill by using
         the TN_spill macro.  Unfortunately, that information is set
         when a call is made to CGSPILL_Get_TN_Spill_Location, which
         happens early in this routine.  Because of this, the following
         code may delete a load of a value, thinking that it has already
         been saved.  If this is the first definition of the TN in the
         Live Range, there will have been no previous save.  So let's
         skip the following logic unless this TN has already been spilled. */
#ifdef TARG_ST
      //
      // Arthur: there seems to be a problem.
      //         The first def can be marked already spilled if it's a
      //         GRA spill. This would not be a problem, but such GRA
      //         spill may be "dummy" - result of not knowing that a
      //         conditional def is the first def of a TN. GRA thinks
      //         that conditional defs do not kill live ranges, so a
      //         live range may be live-in even when there is no
      //         previous definition. In this case, there is a move
      //         inserted later down here for a conditional def with
      //         the source TN being the one "live-in" which is
      //         undefined. To avoid this, I add an explicit check
      //         for not deleting the first def.
      //
      if (LR_reloadable(spill_lr) || 
	  (already_spilled && Is_OP_Spill_Load (op, spill_loc)) && (i != first_def)) {
#else
      if (TN_is_rematerializable(LR_tn(spill_lr)) ||
	  (TN_is_gra_homeable(LR_tn(spill_lr)) || already_spilled) && 
	   Is_OP_Spill_Load (op, spill_loc)) {
#endif
        // If the definition of the spill_tn is a load from the spill
        // location, we don't have to insert the store to memory. Actually
        // we can get rid of the load as well, since we will be loading 
        // from the spill location before each use of spill_tn.
        BB_Remove_Op (bb, op);
	Clobber_Op_Info(i, spill_cl);
        Set_OP_VECTOR_element(Insts_Vector, i, NULL);
        if (Do_LRA_Trace(Trace_LRA_Spill)) {
	  if (reloadable) {
	    fprintf (TFile, "LRA_SPILL>>    Removed reloadable load at %d\n",
		     i);
	  } else {
	    fprintf (TFile, "LRA_SPILL>>    Removed spill load at %d\n", i);
	  }
          Print_OP_No_SrcLine(op);
        }
        continue;
      }

      // if the OP is a select or an unaligned load, we cannot get a 
      // new tn. This ensures that we get the result tn to be the same
      // as one of the operand tns.  note that if we had a previous def
      // in the block, we will already have created a new tn for that 
      // def.  in that case, that will be the source that must have the
      // copy inserted for it.
      //
      INT resnum = 0;
      TN* prev_tn = new_tn ? new_tn : spill_tn;
      new_tn = Dup_TN_Even_If_Dedicated (spill_tn);
#ifdef TARG_X8664
      Reset_TN_is_preallocated(new_tn);
#endif

      Set_TN_spill(new_tn, spill_loc);
      local_spills++;global_spills++;
#ifdef TARG_ST
      // Arthur: I will be finding out the right resnum at the same
      //         time
      BOOL same_res = FALSE;
      for (resnum = 0; resnum < OP_results(op); resnum++) {
	if (OP_same_res(op, resnum) >= 0) {
	  same_res = TRUE;
	  break;
	}
      }
      
      // if didn't find reset resnum (because it was like this before
      if (same_res == FALSE) {
	resnum = 0;
      }

      if (same_res && TN_Pair_In_OP(op, spill_tn, prev_tn)) {
#else
      if ((OP_same_res(op)
#ifdef TARG_X8664
           || OP_x86_style(op)		// bug 4721
#endif
	  )
          && TN_Pair_In_OP(op, spill_tn, prev_tn)) {
#endif
	//
	// must insert copy to free register here.  otherwise,
	// the register will be tied up from the first definition
	// of spill_tn, to this op (which modifies it).
	//
	BOOL copy_added = FALSE;
	for (INT opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
	  TN* op_tn = OP_opnd(op, opndnum);
	  if (op_tn == prev_tn) {
            if (!copy_added) {
	      OPS copy_ops = OPS_EMPTY;
	      Exp_COPY(new_tn, prev_tn, &copy_ops);
	      OP_srcpos(OPS_last(&copy_ops)) = OP_srcpos(op);
	      if (Do_LRA_Trace(Trace_LRA_Spill)) {
	        fprintf (TFile, "LRA_SPILL>> copy TN%d to TN%d for same_res OP\n", 
	  	         TN_number(prev_tn), TN_number(new_tn));
	      }
	      CGSPILL_Insert_Ops_Before(bb, op, &copy_ops);
	      copy_added = TRUE;
            }
	    Set_OP_opnd(op, opndnum, new_tn);
	  }
	}
	FmtAssert(copy_added, 
                  ("LRA: couln't find same_res operand for copy\n"));

        // For IA-32 target there may be more than one result that is the
        // the same as one of the operands. We need to find out which one 
        // it is and not assume that it's always result 0. 
#ifdef KEY
        resnum = TN_Resnum_In_OP (op, spill_tn, TRUE);	// bug 9489
#else
        resnum = TN_Resnum_In_OP (op, spill_tn);
#endif
      }

      else if (OP_cond_def(op) &&
#ifdef KEY
	       TNs_Are_Equivalent(OP_result(op, 0), spill_tn)
#else
	       OP_result(op,0) == spill_tn
#endif
	       ) {
#ifdef TARG_ST
        if (!def_available) {
          // [TTh] Reload previous version before the cond_def
          def_available = TRUE;
          Add_Spill_Load_Before_Use (new_tn, spill_loc, reloadable_def, i, bb);
        }
        else
#endif
        {
            OPS copy_ops = OPS_EMPTY;
            Exp_COPY(new_tn, prev_tn, &copy_ops);
            OP_srcpos(OPS_last(&copy_ops)) = OP_srcpos(op);
            if (Do_LRA_Trace(Trace_LRA_Spill)) {
                fprintf (TFile, "LRA_SPILL>> copy TN%d to TN%d for cond_def OP\n", 
                         TN_number(prev_tn), TN_number(new_tn));
            }
            CGSPILL_Insert_Ops_Before(bb, op, &copy_ops);
        }
#ifdef KEY
        resnum = 0;
#else
        resnum = TN_Resnum_In_OP (op, spill_tn);
#endif
      }
      
#ifdef TARG_ST
      else if (OP_results(op) > 1) { // && (OP_result(op, 0) != spill_tn)) {

        for (resnum = OP_results(op)-1; resnum >= 0; resnum--) {
          if (spill_tn == OP_result(op,resnum)) break;
        }
        
      }
#else
      else if ((OP_results(op) > 1) && (OP_result(op, 0) != spill_tn)) {
#ifdef KEY
	// TNs_Are_Equivalent gives more accurate result. (bug#3568)
        for( resnum = OP_results(op)-1; resnum >= 0; resnum-- ){
          if( TNs_Are_Equivalent( spill_tn, OP_result(op,resnum) ) )
	    break;
        }
	FmtAssert( resnum >= 0, ("Spill_Live_Range: invalide result") );
#else
#ifdef TARG_ST
        for (resnum = OP_results(op)-1; resnum >= 0; resnum--) {
	  TN *result_tn = OP_result(op, resnum);
          if (spill_tn == result_tn
	      || (TNs_alias_p (spill_tn, result_tn)))
	    break;
#else
        for (resnum = OP_results(op); resnum > 0; resnum--) {
          if (spill_tn == OP_result(op,resnum)) break;
        }
#endif
#endif // KEY
      }
#endif

      Set_OP_result(op, resnum, new_tn);
      def_available = TRUE;
      upward_exposed_global = FALSE;
      pending_store = TRUE;
      last_def = i;
    }
    else if (Is_OP_Spill_Load(op, spill_loc) && pending_store &&
	     spill_store_deleted) {
      //
      // must put spill back if we spilled a global, and are now reloading
      // it back to a local.
      //
      Add_Spill_Store_After_Def (new_tn, last_def, bb);
      pending_store = FALSE;
    }
    else if (at_fatpoint || (upward_exposed_global &&
	     At_Unallocated_Op_Definition(spill_opnum, i, spill_cl))) {
      // If the current instruction is a fatpoint, or we're at the definition
      // of an unallocated operand of the fatpoint, make any earlier
      // definition unavailable.  we need to do this for globals at exposed
      // uses since the exposed use will take the register allocated to the
      // global, and this is the register we've made available at the fat
      // point (uses of the global after spill reloads will use whatever
      // register is available at that point).
      def_available = FALSE;

      if (upward_exposed_global) {
	//
	// spill the global before this instruction
	//
	LR_first_spill(spill_lr) = last_def + 1;
	upward_exposed_global = FALSE;
	Add_Spill_Store_After_Def (new_tn, last_opnum, bb);
	pending_store = FALSE;
      }
    }
    last_opnum = i;
  }

  // If there was a use that is a store to a spill location it has
  // been deleted. So, if there is still a pending_store at this point,
  // go back and insert the spill store after the last def. Otherwise,
  // the spill location will not get updated.
  if (pending_store && spill_store_deleted) {
    Add_Spill_Store_After_Def (new_tn, last_def, bb);
    pending_store = FALSE;
  }

  // Check if spill_tn is live-out from this basic block. If it is,
  // we need to create a definition of spill_tn.  if its last definition
  // is also the last instruction in the block (this is likely $31 ...
  // see comments above), then no need to copy (just let the last
  // instruction write the register).
#ifdef TARG_ST
  if (live_out &&
#else
  if (LR_last_use(spill_lr) == VECTOR_size(Insts_Vector) &&
#endif
      last_def != VECTOR_size(Insts_Vector)-1) {
    if (!def_available) {
#ifdef TARG_ST
      Add_Spill_Load_Before_Use (spill_tn, spill_loc, reloadable_def, 
				 VECTOR_size(Insts_Vector), bb);
#else
      Add_Spill_Load_Before_Use (spill_tn, spill_loc, reloadable_def, 
                                                  LR_last_use(spill_lr), bb);
#endif
    }
    else {
      // Copy the new_tn to the spill_tn.
      OPS spill_ops = OPS_EMPTY;
      Exp_COPY (spill_tn, new_tn, &spill_ops);
      CGSPILL_Append_Ops (bb, &spill_ops);
      if (Do_LRA_Trace(Trace_LRA_Spill)) {
        fprintf (TFile, "LRA_SPILL>>    copy TN%d to TN%d at end of %d\n", 
                    TN_number(new_tn), TN_number(spill_tn), i);
      }
    }
  }

  if (magic_spill_used) {
    // Use the location only once for each basic block.
    Magic_Spill_Location = NULL;
    // For global TNs, restore the TN_spill value.
    if (!is_local_tn) {
      Set_TN_spill(spill_tn, sv_spill_location);
    }
  }

  //
  // for spilled global live ranges, we must ensure that they
  // are given to one of the unallocated operands of the current
  // fat point.  we do this because if we allow any register to
  // use this global above the point that we restore it at the bottom
  // of the block, we are not guaranteed that a register will be available
  // when we try to allocate for the operands of this instruction.  this
  // is mainly because spills are inserted for global live ranges based
  // on the live range of the local that they are spilling for.  thus, if
  // this specific global is unavailable, another global may not have
  // spills placed appropriately, and that global cannot be used.
  //
  if (!is_local_tn && do_global_locking) {
    OP *op = OP_VECTOR_element (Insts_Vector, spill_opnum);
    INT opndnum;
    for (opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
      TN *tn = OP_opnd(op, opndnum);
      if (tn == NULL || TN_is_constant(tn)) {
	continue;
      }
      LIVE_RANGE *lr = LR_For_TN(tn);
      // lr null if inserted for previous spill
      if (TN_register_class(tn) == spill_cl && lr != NULL
	  && !LR_fat_allocated(lr) && LR_last_use(lr) == spill_opnum) {
	Set_TN_is_dedicated(tn);
	Set_TN_register(tn, spill_reg);
	Set_LR_fat_allocated(lr);
      }
    }
  }
}


static void
Add_To_Live_LRs_Vector (LIVE_RANGE *lr)
{
  if (!LR_added(lr)) {
    Set_LR_added(lr);
    VECTOR_Add_Element (Live_LRs_Vector, lr);
  }
}


static void
Update_Live_LRs_Vector (INT opnum, ISA_REGISTER_CLASS cl)
{
  if (opnum == 1) return;

  INT i;
  OP *op = OP_VECTOR_element(Insts_Vector, opnum);
  if (op != NULL) {
    for (i = 0; i < OP_opnds(op); i++) {
      TN *tn = OP_opnd(op,i);
      if (!TN_is_register(tn) || TN_register_class(tn) != cl) continue;
      LIVE_RANGE *lr = LR_For_TN (tn);
      // For new TNs added, lr will be NULL. 
      if (lr != NULL && 
          LR_last_use(lr) == opnum && 
          opnum > (LR_first_def(lr)+1)) 
      {
        Add_To_Live_LRs_Vector (lr);
      }
    }
  }

  /* remove the def for the next OP since we don't want to consider
   * that live range for spilling any longer.
   */
  op = OP_VECTOR_element (Insts_Vector, opnum-1);
  if (op != NULL) {
    for (i = 0; i < OP_results(op); i++) {
      TN *tn = OP_result(op, i);
      LIVE_RANGE *lr = LR_For_TN (tn);
      // For new TNs added, lr will be NULL. 
      if (lr != NULL &&
          LR_first_def(lr) == (opnum-1) && 
          LR_exposed_use(lr) == 0) 
      {
#ifdef TARG_ST
	Delete_LR_and_aliases (Live_LRs_Vector, lr);
#else
        VECTOR_Delete_Element (Live_LRs_Vector, lr);
#endif
      }
    }
  }
}

//
// remove the live range associated with opnum if this is the 
// initial definition of it.  after we've done all of the spilling
// for an op (or passed it over as we don't need to spill), we
// remove its live range as it can't possibly contribute to clearing
// any of the fat points above it.
//
static void
Remove_Current_LR(INT opnum)
{
  OP *op = OP_VECTOR_element (Insts_Vector, opnum-1);
  
  if (op != NULL) {
    for (INT i = 0; i < OP_results(op); i++) {
      TN *tn = OP_result(op, i);
      LIVE_RANGE *lr = LR_For_TN (tn);
      // For new TNs added, lr will be NULL. 
      if (lr != NULL &&
          LR_first_def(lr) == (opnum) && 
          LR_exposed_use(lr) == 0) 
      {
#ifdef TARG_ST
	Delete_LR_and_aliases (Live_LRs_Vector, lr);
#else
        VECTOR_Delete_Element (Live_LRs_Vector, lr);
#endif
      }
    }
  }
}


// Compute the set of live ranges that span the instruction that 
// caused the register allocation failure. First look for local 
// live ranges by traversing through the OPs. Then traverse through
// all registers in this class to add global live ranges that span
// the failure point. We filter out those live ranges which have 
// have a reference in the failing instruction.
static void
Init_Live_LRs_Vector (
  BB *bb, 
  INT failure_point, 
  ISA_REGISTER_CLASS cl, 
  MEM_POOL *pool)
{
  OP *failure_op = OP_VECTOR_element(Insts_Vector, failure_point);
  Live_LRs_Vector = VECTOR_Init (BB_length(bb)+REGISTER_MAX, pool);

  // find candidate global live ranges to spill.
#ifdef TARG_ST
  if (!(CG_LRA_mask & 0x0100)) {
#else
  // don't consider global live ranges with the option -Wb,-ttlra:0x100.
  if (!Get_Trace (TP_ALLOC, 0x0100)) {
#endif
    // Look for live ranges of all allocatable registers in this class.
    REGISTER_SET rs = REGISTER_CLASS_allocatable(cl);
#ifdef TARG_ST
    for (LIVE_RANGE *lr = Live_Range_List; lr != NULL; lr = LR_next(lr)) {
      // If this is a global live range,
      // and all of the allocated registers are live across the
      // failure point, add it to the live LRs vector.
      TN *tn = LR_tn(lr);
      if (TN_is_global_reg(tn) && TN_register_class(tn) == cl) {
	REGISTER reg;
	BOOL all_live = TRUE;
	FOR_ALL_REGISTER_SET_members (TN_registers (tn), reg) {
	  if (! REGISTER_SET_MemberP (rs, reg)
	      || (ded_reg_last_use(cl, reg) <= failure_point
		  && ded_reg_exposed_use(cl, reg) <= failure_point)
	      || (! ded_reg_exposed_use(cl, reg)
		  && ded_reg_first_def(cl, reg) >= failure_point)
	      || OP_Refs_Reg (failure_op, cl, reg)) {
	    all_live = FALSE; break;
	  }
	}
	if (all_live) Add_To_Live_LRs_Vector (lr);
      }
    }
#else
    for (REGISTER reg = REGISTER_SET_Choose(rs);
         reg != REGISTER_UNDEFINED;
         reg = REGISTER_SET_Choose_Next(rs, reg))
    {
      LIVE_RANGE *reg_lr = LR_For_TN (Build_Dedicated_TN(cl, reg, 0));
      if (reg_lr != NULL &&
          LR_last_use(reg_lr) > failure_point &&
          (LR_exposed_use(reg_lr) || LR_first_def(reg_lr) < failure_point) &&
          !OP_Refs_Reg (failure_op, cl, reg))
      {
        Add_To_Live_LRs_Vector (reg_lr);
      }
    }
#endif
  }

  // find candidate local live ranges to spill.
  for (INT opnum = 1; opnum < failure_point; opnum++) {
    LIVE_RANGE *clr;
    OP *op = OP_VECTOR_element(Insts_Vector, opnum);
    for (INT resnum = 0; resnum < OP_results(op); resnum++) {
      TN *result_tn = OP_result(op, resnum);
      if (TN_register_class(result_tn) == cl && 
          TN_is_local_reg(result_tn) &&
          (clr = LR_For_TN (result_tn)) &&
          LR_first_def(clr) == opnum &&
          LR_last_use(clr) > failure_point &&
          !OP_Refs_TN (failure_op, result_tn))
      {
	Add_To_Live_LRs_Vector (clr);
      }
    }
  }
}

/* ======================================================================
 * Fix_LRA_Blues
 *
 * This routine is called when register allocation fails because we ran
 * out of registers. The failure occured when trying to allocate a register
 * for <tn> at instruction number <opnum>.
 *
 * There are several ways to deal with the situation:
 *
 *   1. Spill a global register at the entry of the block and restore it
 *      at the bottom of the block. This effectively increases the number
 *      of registers available for allocation.
 *   2. Reorder instructions to shorten some live ranges. This reduces
 *      the number of overlapping live ranges and increases the chances
 *      for successful register allocation.
 *   3. Spill some live ranges. This has the same effect as 2 in terms of
 *      reducing the number of overlapping live ranges.
 * ======================================================================*/
static void
Fix_LRA_Blues (BB *bb, TN *tn, HB_Schedule *Sched)
{
  // Even when we are not reordering, we need the dep-graph do check 
  // if loads can be replicated instead of spilling.
  BOOL need_dep_graph = (CG_opt_level > 0);

  // The reorder transformations and spill transformations need to 
  // be done in separate passes. This is because spilling can lead
  // to deletion of instructions that might have been used as position
  // markers in the move transformations. So, do only reordering
  // transformations for the first few trips.
  BOOL allow_reordering = Check_Allow_Reorder() || (LRA_do_reorder == TRUE);

#ifdef KEY
  // Build the dependence graph only if it's needed to find reloadable LRs or
  // doing reordering.  Bug 8026.
  need_dep_graph = ((need_dep_graph &&
		     Need_Dep_Graph_For_Mark_Reloadable_Live_Ranges) ||
		    allow_reordering);
#endif

  ISA_REGISTER_CLASS cl = TN_register_class(tn);
  LIVE_RANGE *lr = LR_For_TN(tn);
  INT failure_point;
  INT opnum;

#ifdef KEY
  if (Trip_Count > MAX_TRIP_COUNT &&		// Bug 12183
      large_asm_clobber_set[cl]) {
    ErrMsg(EC_Misc_Asm,
	 "Local register allocation unsuccessful due to large ASM clobber set");
  }
#endif

  FmtAssert (Trip_Count <= MAX_TRIP_COUNT, 
             ("LRA: Unable to spill TN:%d (cl:%d) in BB:%d, GRA grant:%d", 
                TN_number(tn), cl, BB_id(bb), 
                REGISTER_SET_Size(avail_set[cl])));

  if (Do_LRA_Trace(Trace_LRA_Spill)) {
     fprintf (TFile,"LRA_SPILL>> Attempt to spill (cl:%d) in BB:%d, GRA grant:%d\n",
                cl, BB_id(bb), REGISTER_SET_Size(avail_set[cl]));
  }

  // If this is the first attempt to spill for the bb, then try to 
  // reschedule while minimizing register pressure. 
#ifdef TARG_ST
   if (Check_Allow_Reschedule(bb))
#else
  if (Check_Allow_Reschedule()) 
#endif
      {
    if (Do_LRA_Trace(Trace_LRA_Spill)) {
      fprintf (TFile, "LRA_SPILL>> Out of Registers (BB:%d) trip:%d\nLRA_SPILL>>",
	       BB_id(bb), Trip_Count);
      Print_Live_Range (lr);
    }
    if (Do_LRA_Trace(Trace_LRA_Spill)) {
      fprintf (TFile, "LRA_SPILL>> Reschedule to minimize register usage\n");
    }

    mINT8 regs_avail[ISA_REGISTER_CLASS_MAX+1];
    for (INT i = ISA_REGISTER_CLASS_MIN; i <= ISA_REGISTER_CLASS_MAX; i++) {
      regs_avail[i] = REGISTER_SET_Size (avail_set[i]);
    }

    if (!Sched) {
      Sched = CXX_NEW(HB_Schedule(), &MEM_local_pool);
    }
#ifdef TARG_ST
    if (LRA_resched_check) {
      // Force recomputation of register request, which could have changed
      // since scheduling.
      LRA_Request_Info request_info;
      LRA_Estimate_Fat_Points(bb, &request_info, NULL, &MEM_local_pool);
      Sched->Init(bb, 
		  HBS_BEFORE_LRA | HBS_DEPTH_FIRST | HBS_MINIMIZE_REGS, 
		  INT32_MAX, 
		  NULL, 
		  regs_avail,
		  cl,
		  request_info.summary[cl]);
    } else
#endif
    Sched->Init(bb, 
		HBS_BEFORE_LRA | HBS_DEPTH_FIRST | HBS_MINIMIZE_REGS, 
		INT32_MAX, 
		NULL, 
		regs_avail);
    Sched->Schedule_BB(bb, NULL);
    Reset_BB_scheduled (bb);
#ifdef KEY
    // Fix memory leak - call the appropriate destructor to pop and delete
    // _hb_pool after it is used.
    // Note: can not call the destructor in the caller because it is trying to
    // pop lra_pool before.
    if (Sched) {
      CXX_DELETE(Sched, &MEM_local_pool);
      Sched = NULL;
    }
#endif // KEY
    return;
  }

  if (Do_LRA_Trace(Trace_LRA_Spill)) {
    fprintf(TFile, "LRA_SPILL>> reschedule failed.  Must %s in register class %d ",
	    allow_reordering?"reorder":"spill",cl);
    fprintf(TFile, "in block %d ", BB_id(bb));
    fprintf(TFile, "for procedure %s in file %s\n",
	    ST_name(Get_Current_PU_ST()), Src_File_Name);
  }

  // Move the delay slot instruction above if the branch has delay slot.
  if (PROC_has_branch_delay_slot())
    BB_Move_Delay_Slot_Op (bb);

  /* rebuild live ranges */
  Setup_Live_Ranges (bb, TRUE, &lra_pool);

  // 
  // establish failure point from newly created live range.
  //
  lr = LR_For_TN(tn);
  failure_point = LR_last_use (lr);
  if (failure_point == 0) failure_point = LR_exposed_use(lr);
  if (Do_LRA_Trace(Trace_LRA_Spill)) {
    fprintf (TFile,"LRA_SPILL>> Out of Registers (BB:%d) trip:%d\nLRA_SPILL>>",
	     BB_id(bb), Trip_Count);
    Print_Live_Range (lr);
  }

  if (failure_point !=0) {
    OP *op = OP_VECTOR_element (Insts_Vector, failure_point);
    if (!PROC_has_branch_delay_slot() && OP_xfer(op)) {
      if (Do_LRA_Trace(Trace_LRA_Spill)) {
        fprintf(TFile,"LRA_SPILL>> can not reload registers after a branch.\n");
        Print_BB(bb);
      }
      FmtAssert(!OP_xfer(op),
                ("LRA: Spill at end of block OP:%d in BB:%d\n",
                failure_point, BB_id(bb)));
    }
  }

  /* We compute the dependence graph to help determine the legality 
   * of reordering instructions.
   */
  MEM_POOL op_to_opnum_pool;
  if (need_dep_graph) {
    /* build the OP -> opnum map */
    MEM_POOL_Initialize (&op_to_opnum_pool, "LRA_op_to_opnum", FALSE);
    MEM_POOL_Push (&op_to_opnum_pool);
    op_to_opnum = BB_OP_MAP32_Create (bb, &op_to_opnum_pool);
    for (opnum = 1; opnum <= BB_length(bb); opnum++) {
      OP *op = OP_VECTOR_element (Insts_Vector, opnum);
      BB_OP_MAP32_Set (op_to_opnum, op, opnum);
      /* use OP_flag1 to indicate if an OP has been moved. */
      Reset_OP_flag1 (op);
    }
    CG_DEP_Compute_Graph (
      bb,
      INCLUDE_ASSIGNED_REG_DEPS,
      NON_CYCLIC,
      NO_MEMREAD_ARCS,
      INCLUDE_MEMIN_ARCS,
      NO_CONTROL_ARCS,
      NULL);

    // Mark all the live ranges that have a single definition that can
    // be redone before every use instead of spilling. This is useful
    // for FP loads.
    if (!allow_reordering) Mark_Reloadable_Live_Ranges (cl);
  }
#ifdef TARG_X8664
  else {
    // Prefer the lightweight version of Mark_Reloadable_Live_Ranges whenever
    // possible, since it doesn't need the dependence graph (building dep graph
    // is expensive).  Bug 8026.
    Quick_Mark_Reloadable_Live_Ranges (cl);
  }
#endif

  // compute the set of live ranges that span the failure point.
  Init_Live_LRs_Vector (bb, failure_point, cl, &lra_pool);

  if (Always_Trace(Trace_LRA) && trace_tn && trace_bb == BB_id(bb)) {
    Print_Live_Across();
  }

  INT fatpoint = fat_points[failure_point];
  SPILL_CANDIDATE *transformations = NULL;
  INT first_fatpoint = 1;

  if (Get_Trace (TP_ALLOC, 0x20, bb)) {
    fprintf (TFile, "Fat Points (BB:%d)\n", BB_id(bb));
    for (opnum = 1; opnum <= failure_point; opnum++) {
      OP *op = OP_VECTOR_element(Insts_Vector, opnum);
      fprintf(TFile, "OP:%d [fp=%d]>> ", opnum, fat_points[opnum]);
      Print_OP_No_SrcLine(op);
    }
  }

#ifdef KEY
  // The fatpoint can be less than zero at an ASM with more than one result.
  // In that case, fudge the fatpoint to be 0 and continue.  Bug 13697.
  if (fatpoint < 0) {
    OP *op = OP_VECTOR_element(Insts_Vector, failure_point);
    FmtAssert(OP_code(op) == TOP_asm && OP_results(op) > 1,
	      ("Fix_LRA_Blues: negative fatpoint at non-asm OP"));
    fatpoint = 0;
  }
#endif

  //
  // the fatpoint should always be > zero (in fact, its the number of
  // additional registers of the failing class required at the failure
  // point).  all of the operations above the failure point will have a
  // fatpoint value that corresponds to the current deviation from the 
  // requirements at the fat point (0 or less means no spilling needed).
  // if it is not greater than zero, this is usually an indication that
  // the operand requiring the spill is not defined above in the block.
  // lra will allocate and immediately free a register in this case, thus
  // adding no registers and giving a fatpoint of 0.  bump it by one so
  // that we go through the spill loop at least once.
  //
  FmtAssert(fatpoint >= 0,
	    ("LRA: Spill at negative fatpoint at OP:%d in BB:%d\n",
	    failure_point, BB_id(bb)));
  if (fatpoint == 0) {
    DevWarn("Fat point is zero at OP:%d in BB:%d\n", failure_point,
	    BB_id(bb));
    fatpoint = 1;
#ifdef TARG_ST
    fat_points[failure_point] = TN_nhardregs(tn);
#else
    fat_points[failure_point] = 1;
#endif
  }

  /* traverse back from the instruction where register allocation 
   * failed and spill/reorder for all fatpoints above this.
   */
  fatpoint_min = 0;
  for (opnum = failure_point; opnum > 0; ) {
    FAT_POINTS_TYPE cur_usage = fat_points[opnum];
    BOOL reordering_failure = false;

    // 
    // TODO: modify loop for setting first fatpoint to have same
    //       functionality as code below for decrementing opnum.
    //       will make first_fatpoint more accurate, and thus help
    //	     compile speed.
    //

    // recompute first_fatpoint.
    while (fat_points[first_fatpoint] <= fatpoint_min) {
      first_fatpoint++;
      if (opnum < first_fatpoint) break;
    }
    // if we are already past the first fatpoint, we are done.
    if (opnum < first_fatpoint) break;
    
    // Check if there is a register problem at the current instruction.
    if (fat_points[opnum] > fatpoint_min) {

      SPILL_CANDIDATE *best = TYPE_ALLOCA(SPILL_CANDIDATE);
      best->spill_kind = SPILL_NONE;
      best->spill_num = opnum;

      if (Do_LRA_Trace(Trace_LRA_Spill)) {
	fprintf (TFile,
		 "Analyze >> analyze spilling for OP:%d in BB:%d (fp=%d)\n",
		 opnum, BB_id(bb), fat_points[opnum]);
      }
      if (!allow_reordering && !CG_localize_tns) {
	if (!livethrough_computed) {
	  // compute the set of registers that are livethrough the bb.
	  Compute_Livethrough_Set (bb);
	  livethrough_computed = TRUE;
	}
        Analyze_Spilling_Global_Register (cl, best, opnum, fatpoint);
      }

      // pick the best live range to reorder/spill.
      for (INT i = 0; i < VECTOR_count(Live_LRs_Vector); i++) {
        LIVE_RANGE *clr = LR_VECTOR_element (Live_LRs_Vector, i);
        if (allow_reordering) {
          Analyze_Reordering (clr, best, opnum);
        }
        else {
          Analyze_Spilling_Live_Range (clr, opnum, best, fatpoint);
        }
      }
    
      if (best->spill_kind == SPILL_MOVE_DEF || 
          best->spill_kind == SPILL_MOVE_USE)
      {
        // Delay the reordering till all candidates have been identified.
        // However, update the fat_points array and remove the relevant live 
        // ranges from the Live_LRs_Vector so that they are not considered 
        // again.
        Update_Fat_Points (best, opnum);
        best->next = transformations;
        transformations = best;
      }
      else if (best->spill_kind == SPILL_GLOBAL_REG) {
        Spill_Global_Register (bb, best);
        // Increase the maximum girth allowed, since we now have an 
        // additional register available.  Bump cur_usage, too, so
	// that we don't erroneously report no progress.  Remove the
	// global register from the livethrough set.
        fatpoint_min++;
	cur_usage++;
        livethrough[cl] = REGISTER_SET_Difference1 (
                                livethrough[cl], best->u1.s1.global_spill_reg);
      }
      else if (best->spill_kind == SPILL_LIVE_RANGE) {
        Spill_Live_Range (bb, best->u1.spill_lr, fatpoint, opnum);
        Update_Fat_Points (best, opnum);
      } else if (allow_reordering) {
	// since we don't intermix reordering with other types of
	// spilling, we must allow there to be no progress for a
	// given op
	reordering_failure = true;
      }

      // We have already used the magic spill location. So, don't look
      // for any other spill opportunities.
      // TODO: account for exposed use when updating reg usage.
      if (Trip_Count == MAX_TRIP_COUNT) break;
    }


    if (cur_usage <= fat_points[opnum]) {
      //
      // we're not making progress in reducing the register usage at
      // this operation.  something's hosed, so don't continue trying.
      //
      if (!reordering_failure && Do_LRA_Trace(Trace_LRA_Spill)) {
	fprintf (TFile, "LRA: no progress when spilling at OP:%d in BB:%d\n",
		 opnum, BB_id(bb));
      }
      Update_Live_LRs_Vector (opnum, cl);
      opnum--;
    }

    //
    // find next fatpoint above opnum (current last fatpoint).  note that
    // opnum will not move if the fatpoint that its sitting on has not been
    // sufficiently reduced.  
    //
    while (opnum > 0 && fat_points[opnum] <= fatpoint_min) {
      Update_Live_LRs_Vector (opnum, cl);
      opnum--;
    }
  }

  if (allow_reordering) {
    /* Apply all the transformations needed at once */
    Apply_Move_Transformations (bb, transformations);
  }
  if (need_dep_graph) {
    MEM_POOL_Pop (&op_to_opnum_pool);
    MEM_POOL_Delete (&op_to_opnum_pool);
    CG_DEP_Delete_Graph (bb);
  }

  /* if we spill we need to schedule again */
  Reset_BB_scheduled (bb);
  
  Clear_Fat_Point_Calculation();
}


#ifdef TARG_X8664
static void Preallocate_Single_Register_Subclasses (BB* bb);
static void Verify_TARG_X86_Op_For_BB( BB* );
static void Adjust_X86_Style_For_BB( BB*, BOOL*, MEM_POOL* );
static void Adjust_eight_bit_regs (BB*);
static void Presplit_x87_MMX_Live_Ranges (BB*, MEM_POOL*);
#endif
#ifdef KEY
static void Detect_large_asm_clobber (BB*);
#endif /* TARG_X8664 */

/* ======================================================================
 * Alloc_Regs_For_BB
 *
 * Do local register allocation for a basic block.
 * ======================================================================*/
void Alloc_Regs_For_BB (BB *bb, HB_Schedule *Sched)
{
  BOOL lra_done;
  TN *tn;
  BOOL redundant_code = FALSE;

  MEM_POOL_Initialize (&lra_pool, "LRA_pool", FALSE);
  CGSPILL_Reset_Local_Spills ();
  Init_Avail_Set (bb);
  local_spills = 0;
  livethrough_computed = FALSE;
  Trip_Count = 0;
  Magic_Spill_Location = Local_Spill_Sym;

#ifdef KEY
  // Detect ASM clobber sets that are too large which may cause LRA to fail.
  Detect_large_asm_clobber(bb);
#endif

#ifdef TARG_X8664
  // Presplit x87 live ranges to avoid MMX OPs, and vice versa.
  Presplit_x87_MMX_Live_Ranges(bb, &lra_pool);	// Changes Trip_Count.
  Trip_Count = 0;

  Adjust_eight_bit_regs(bb);

  // Adjust_X86_Style_For_BB needs to be run only once because the x86-style
  // property is preserved in the allocation loop.  If the opnd0/result TN
  // needs to be spilled, we make sure spilling would replace both opnd0 and
  // the result with the same TN.
  Adjust_X86_Style_For_BB( bb, &redundant_code, &lra_pool );

  Preallocate_Single_Register_Subclasses(bb);
#endif

  do {
    MEM_POOL_Push (&lra_pool);
    Trip_Count++;

#ifdef TARG_X8664
    ISA_REGISTER_CLASS cl;
    FOR_ALL_ISA_REGISTER_CLASS(cl){
      last_assigned_reg[cl] = REGISTER_UNDEFINED;
    }
#endif

    Init_Avail_Regs ();
    Setup_Live_Ranges (bb, TRUE, &lra_pool);

    if (Always_Trace(Trace_LRA)) {
      if (trace_tn) {
	bb_live = TN_SET_Create_Empty(Last_TN + 1, &lra_pool);
	TN *tn;
	for (tn = GTN_SET_Choose(BB_live_out(bb));
	     tn != GTN_SET_CHOOSE_FAILURE;
	     tn = GTN_SET_Choose_Next(BB_live_out(bb), tn)) {
	  if (LRA_TN_register(tn) != REGISTER_UNDEFINED &&
	      TN_register_class(tn) == trace_cl) {
	    bb_live = TN_SET_Union1D(bb_live, tn, &lra_pool);
	  }
	}
      }
      Print_BB_For_LRA (bb);
      if (Do_LRA_Trace(Trace_LRA_Detail)) {
        Print_Live_Ranges (bb);
      }
    }

    lra_done = Assign_Registers (bb, &tn, &redundant_code);
    if (!lra_done) {
      Fix_LRA_Blues (bb, tn, Sched);
    }
    MEM_POOL_Pop (&lra_pool);
  } while (!lra_done);


#ifdef TARG_X8664
  if( BB_length(bb) > 0 ){
#if Is_True_On
    Verify_TARG_X86_Op_For_BB( bb );
#endif // Is_True_On

  } else {
    redundant_code = FALSE;
  }
#endif // TARG_X8664

  if (redundant_code) Remove_Redundant_Code (bb);

  MEM_POOL_Delete (&lra_pool);
  Set_BB_reg_alloc (bb);

  if (Always_Trace(Trace_LRA)) {
    if (Trip_Count > 1) {

      fprintf(TFile,"%d iterations and %d spills required to allocate regs for BB:%d\n",
              Trip_Count,local_spills,BB_id(bb));
    }
  }

}


/******************* Save/restore of Callee Saved Regs ********************/


// Allocate registers for any unallocated TNs in <ops>.
// <bb> is used to determine what temps are available.
void 
Assign_Temp_Regs (OPS *ops, BB *bb)
{
  REGISTER_SET temps[ISA_REGISTER_CLASS_MAX+1];
  OP *op;
  OP *sp_adj;

  if (BB_entry(bb)) {
        sp_adj = BB_entry_sp_adj_op(bb);
        REG_LIVE_Prolog_Temps (bb, sp_adj, sp_adj, temps);
  }
  else if (BB_exit(bb)) {
        sp_adj = BB_exit_sp_adj_op(bb);
        REG_LIVE_Epilog_Temps (Get_Current_PU_ST(), bb, sp_adj, temps);
  }
  else {
        FmtAssert(FALSE, ("unexpected bb"));
  }
  FOR_ALL_OPS_OPs (ops, op) {
#ifdef TARG_ST
    ASM_OP_ANNOT* asm_info = (OP_code(op) == TOP_asm) ?
          (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op) : NULL;
#endif
    REGISTER_CLASS_OP_Update_Mapping(op);
    for (INT i = 0; i < OP_results(op); i++) {
      TN *result_tn = OP_result(op, i);
      if (LRA_TN_register(result_tn) == REGISTER_UNDEFINED) {
        ISA_REGISTER_CLASS cl = TN_register_class(result_tn);
#ifdef TARG_ST
	REGISTER reg;
        ISA_REGISTER_SUBCLASS sc = asm_info ?
          ASM_OP_result_subclass(asm_info)[i] :
          OP_result_reg_subclass(op, i);
        if (sc != ISA_REGISTER_SUBCLASS_UNDEFINED)
          reg = REGISTER_SET_Choose (
                  REGISTER_SET_Intersection(temps[cl],
                                            REGISTER_SUBCLASS_members(sc)));
        else
          reg = REGISTER_SET_Choose (temps[cl]);
#else
        REGISTER reg = REGISTER_SET_Choose (temps[cl]);
#endif
        FmtAssert (reg != REGISTER_UNDEFINED, ("no temp regs available"));
        LRA_TN_Allocate_Register (result_tn, reg);
#ifdef TARG_ST
        temps[cl] = REGISTER_SET_Difference_Range (temps[cl],
                                                   reg, reg+TN_nhardregs(result_tn)-1);
#else
        temps[cl] = REGISTER_SET_Difference1 (temps[cl], reg);
#endif
      }
    }
  }
}


/* ====================================================================
 * Prolog_save_code - generate save code for callee-saved regs
 * ==================================================================== */
static void
Prolog_save_code(BB *entrybb)
{
  INT callee_num;

  for (callee_num = 0; callee_num < Callee_Saved_Regs_Count; callee_num++) {
    TN *tn = CALLEE_tn(callee_num);
    ISA_REGISTER_CLASS cl = TN_save_rclass(tn);
    REGISTER reg = TN_save_reg(tn);
#ifdef TARG_ST
    REGISTER_SET used = Callee_Saved_Regs_Used[cl];
    if (PU_Has_EH_Return) {
      used = REGISTER_SET_Union (used, REGISTER_CLASS_callee_saves(cl));
      used = REGISTER_SET_Union (used, REGISTER_CLASS_eh_return(cl));
    }
    if (! EETARG_Save_With_Regmask (cl, reg) &&
	REGISTER_SET_IntersectsP(used,
				 REGISTER_SET_Range (reg, reg + TN_nhardregs(tn) - 1))) {
#else
    if (REGISTER_SET_MemberP(Callee_Saved_Regs_Used[cl], reg)) {
#endif
      ST *mem_loc = CGSPILL_Get_TN_Spill_Location (tn, CGSPILL_LCL);
      OPS ops = OPS_EMPTY;
      LRA_TN_Allocate_Register (tn, reg);
      /* Generate the spill ops */
      CGSPILL_Store_To_Memory (tn, mem_loc, &ops, CGSPILL_LRA, entrybb);

      /* allocate registers for any temps used in spill sequence */
      Assign_Temp_Regs (&ops, entrybb);

      /* insert the ops in the op list for the current BB */
      CGSPILL_Prepend_Ops (entrybb, &ops);
  
      if ( Do_LRA_Trace(Trace_LRA_Entry_Exit) )
        fprintf ( TFile,
                  "entry BB:%d saved TN%d saved from reg %d:%d\n", 
                  BB_id(entrybb), TN_number(tn), cl, reg );
    }
  }
}

/* ====================================================================
 * Epilog_restore_code - generate restore code for callee-saved regs
 * ==================================================================== */
static void
Epilog_restore_code(BB *exitbb)
{
  /* check for need to generate restore code for callee-saved regs */
  INT callee_num;

  for (callee_num = 0; callee_num < Callee_Saved_Regs_Count; callee_num++) {
    TN *tn = CALLEE_tn(callee_num);
    ISA_REGISTER_CLASS cl = TN_save_rclass(tn);
    REGISTER reg = TN_save_reg(tn);
#ifdef TARG_ST
    REGISTER_SET used = Callee_Saved_Regs_Used[cl];
    if (PU_Has_EH_Return) {
      used = REGISTER_SET_Union (used, REGISTER_CLASS_callee_saves(cl));
      used = REGISTER_SET_Union (used, REGISTER_CLASS_eh_return(cl));
    }
    if (! EETARG_Save_With_Regmask (cl, reg)
	&& REGISTER_SET_IntersectsP(used,
				 REGISTER_SET_Range (reg, 
						     reg + TN_nhardregs(tn) - 1))) {
#else
    if (REGISTER_SET_MemberP(Callee_Saved_Regs_Used[cl], reg)) {
#endif
      ST *mem_loc = CGSPILL_Get_TN_Spill_Location (tn, CGSPILL_LCL);
      OPS ops = OPS_EMPTY;
      /* generate the reload ops */
      CGSPILL_Load_From_Memory (tn, mem_loc, &ops, CGSPILL_LRA, exitbb);

      /* allocate registers for any temps used in spill sequence */
      Assign_Temp_Regs (&ops, exitbb);

      /* insert the ops in the op list for the current BB */
      CGSPILL_Append_Ops (exitbb, &ops);
  
      if (Do_LRA_Trace(Trace_LRA_Entry_Exit))
        fprintf ( TFile,
                  "exit BB:%d saved TN%d restored to reg %d:%d\n", 
              BB_id(exitbb), TN_number(tn), cl, reg );
    }
  }
}

/* 
 * Insert code at entry and exit to save/restore any callee-saved registers
 * that were used.  This is done only for the Localize_TNs case.
 */
static void 
Spill_Callee_Saved_Regs (void)
{
  ISA_REGISTER_CLASS cl;
  BB_LIST *elist;

  if (Do_LRA_Trace(Trace_LRA_Entry_Exit)) {
    fprintf(TFile, "Callee_Saved_Regs_Used:\n");
    FOR_ALL_ISA_REGISTER_CLASS(cl) {
      fprintf(TFile, "  class %d: ", cl);
      REGISTER_SET_Print(Callee_Saved_Regs_Used[cl], TFile);
      fprintf(TFile, "\n");
    }
  }

  for ( elist = Entry_BB_Head; elist; elist = BB_LIST_rest(elist) ) {
    Prolog_save_code (BB_LIST_first(elist));
  }

  for ( elist = Exit_BB_Head; elist; elist = BB_LIST_rest(elist) ) {
#ifdef TARG_ST
    BB *bb = BB_LIST_first (elist);
    ANNOTATION *annot = ANNOT_Get (BB_annotations(bb), ANNOT_CALLINFO);
    if (annot) {
      CALLINFO *callinfo = ANNOT_callinfo (annot);
      if (callinfo
	  && WN_Call_Never_Return (CALLINFO_call_wn (callinfo))) {
	continue;
      }
    }
#endif
    Epilog_restore_code (BB_LIST_first(elist));
  }

#ifndef TARG_ST
  // Callee_Saved_Regs_Used is used now with or without GRA
  // Thus initialization of Callee_Saved_Regs_Used is now done
  // in LRA_Allocate_Registers
  
  /* Reset the Callee_Saved_Regs_Used set for the next procedure. */
  FOR_ALL_ISA_REGISTER_CLASS(cl)
    Callee_Saved_Regs_Used[cl] = REGISTER_SET_EMPTY_SET;
#endif
}


/* Consistency check to make sure that local TNs are used only in one 
 * basic block. 
 */
static void Consistency_Check (void)
{
  BB *bb;
  TN_MAP Local_TNs;

  Local_TNs = TN_MAP_Create ();
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    OP *op;
    INT i;
    FOR_ALL_BB_OPs_FWD (bb, op) {

      for (i = 0; i < OP_results(op); i++) {
        TN *tn = OP_result(op, i);
        if (TN_is_local_reg(tn)) {
          BB *tn_bb = (BB *)TN_MAP_Get (Local_TNs, tn);
          if (tn_bb != NULL) {
            if (tn_bb != bb &&
                (!BB_reg_alloc(bb) || !BB_reg_alloc(tn_bb)))
#ifdef KEY // save TNs can appear in more than one BB
	      if (! TN_is_save_reg(tn))
#endif
              FmtAssert (FALSE, 
                      ("Local TN%d referenced in BB:%d and BB:%d",
                        TN_number(tn), BB_id(tn_bb), BB_id(bb)));
          }
          else {
            TN_MAP_Set (Local_TNs, tn, bb);
          }
        }
        else {
          FmtAssert (LRA_TN_register(tn) != REGISTER_UNDEFINED,
                ("Global TN%d not assigned a register", TN_number(tn)));
        }
      }

      for (i = 0; i < OP_opnds(op); i++) {
        TN *tn = OP_opnd(op,i);
        if (TN_is_register(tn)) {
          if (TN_is_local_reg(tn)) {
            BB *tn_bb = (BB *)TN_MAP_Get (Local_TNs, tn);
            if (tn_bb != NULL) {
              if (tn_bb != bb &&
                  (!BB_reg_alloc(bb) || !BB_reg_alloc(tn_bb)))
#ifdef KEY // save TNs can appear in more than one BB
		if (! TN_is_save_reg(tn))
#endif
                FmtAssert (FALSE, 
                      ("Local TN%d referenced in BB:%d and BB%d",
                        TN_number(tn), BB_id(tn_bb), BB_id(bb)));
            }
            else {
              TN_MAP_Set (Local_TNs, tn, bb);
            }
          }
          else {
            FmtAssert (LRA_TN_register(tn) != REGISTER_UNDEFINED,
                ("Global TN%d not assigned by GRA", TN_number(tn)));
        }
        }
      }

    }
  }
  TN_MAP_Delete (Local_TNs);
}


// Move the spill loads/stores introduced by GRA right next to their
// corresponding uses and defs.
static void
Move_Spill_Loads_Stores (BB *bb)
{

  OP *op; 
  OP *prev_op = NULL;
  OP *next_op = NULL;

  // Move the delay slot instruction (if any) above the terminating 
  // branch. This makes the spill loads added at end of bb to be in
  // right place. Do it only if the branch has a delay slot.
  if (PROC_has_branch_delay_slot())
    BB_Move_Delay_Slot_Op (bb);

  // Make a forward pass through the basic block. Keep track of the
  // definitions in the bb. If we see a spill store, move it right
  // next to the previous definition. If there is no previous 
  // definition in the bb, move the store to the top of the bb.
  TN_MAP spills_map = TN_MAP_Create ();
  for (op = BB_first_op(bb); op != NULL; op = next_op) {
    next_op = OP_next(op);
    if (OP_has_result(op)) {
      for (INT i = 0; i < OP_results(op); i++) {
	TN *result_tn = OP_result(op, i);
	TN_MAP_Set (spills_map, result_tn, op);
      }
    }
    else if (OP_store(op)) {
      TN *src_tn = OP_opnd(op,TOP_Find_Operand_Use(OP_code(op), OU_storeval));
      ST *spill_loc = TN_spill(src_tn);
      if (Is_OP_Spill_Store (op, spill_loc)) {
        OP *def_op = (OP *)TN_MAP_Get (spills_map, src_tn);
        if (def_op != OP_prev(op)) {
          OPS ops = OPS_EMPTY;
          BB_Remove_Op (bb, op);
          if (Do_LRA_Trace(Trace_Move_GRA_Spills)) {
            fprintf (TFile, ">> MOVE Delete");
            Print_OP_No_SrcLine (op);
          }
          CGSPILL_Store_To_Memory (src_tn, spill_loc, &ops, CGSPILL_LRA, bb);
          if (def_op != NULL) {
            CGSPILL_Insert_Ops_After (bb, def_op, &ops);
            if (Do_LRA_Trace(Trace_Move_GRA_Spills)) {
              fprintf (TFile, ">> MOVE Append");
              Print_OP_No_SrcLine (OPS_first(&ops));
              fprintf (TFile, ">> MOVE After ");
              Print_OP_No_SrcLine (def_op);
            }
          }
          else {
            CGSPILL_Prepend_Ops (bb, &ops);
            if (Do_LRA_Trace(Trace_Move_GRA_Spills)) {
              fprintf (TFile, ">> MOVE_SPILL Prepend");
              Print_OP_No_SrcLine (OPS_first(&ops));
            }
          }
        }
      }
    }
  }
  TN_MAP_Delete (spills_map);

  // Make a backward pass through the basic block. Keep track of all 
  // the uses of register TNs. If we see a spill load, move it right
  // next to the next use. If there are no later uses in the bb, move
  // the spill load to the end of the bb.
  spills_map = TN_MAP_Create ();
  for (op = BB_last_op(bb); op != NULL; op = prev_op) {
    prev_op = OP_prev(op);
    INT i;
    for (i = 0; i < OP_opnds(op); i++) {
      TN *tn = OP_opnd(op,i);
      if (TN_is_register(tn)) {
        TN_MAP_Set (spills_map, tn, op);
      }
    }
    for (i = 0; i < OP_results(op); i++) {
      TN *result_tn = OP_result(op, i);
      ST *spill_loc = TN_spill(result_tn);
      if (Is_OP_Spill_Load (op, spill_loc)) {
        OP *use_op = (OP *)TN_MAP_Get (spills_map, result_tn);
        if (use_op != OP_next(op)) {
          OPS ops = OPS_EMPTY;
          BB_Remove_Op (bb, op);
          CGSPILL_Load_From_Memory (result_tn, spill_loc, &ops, CGSPILL_LRA,
				    bb);
          if (use_op != NULL) {
            CGSPILL_Insert_Ops_Before (bb, use_op, &ops);
          }
          else {
            CGSPILL_Append_Ops (bb, &ops);
          }
        }
      }
      //
      // make this definition the most recent use of the tn.  this is
      // to prevent useless reloads at the top of blocks from moving
      // past the redefinition of the tn.
      //
      TN_MAP_Set (spills_map, result_tn, op);
    }
  }
  TN_MAP_Delete (spills_map);

  if (Do_LRA_Trace(Trace_Move_GRA_Spills)) {
    Print_BB (bb);
  }
}


#ifdef TARG_X8664
// Add copies between TNs and 8-bit registers.
static void
Adjust_one_eight_bit_reg( BB* bb, OP* op, int opnd_idx, BOOL is_opnd )
{
  BOOL is_opnd_and_result = FALSE;
  int result_idx = is_opnd ? -1 : opnd_idx;
  TN* opnd = is_opnd ? OP_opnd( op, opnd_idx ) : OP_result( op, opnd_idx );

  if (!TN_is_register(opnd))
    return;

  if (OP_code(op) == TOP_asm) {
    // Check for ASM '+' constraint which means operand is both input and
    // output.  Bug 14849.
    int i;
    if (is_opnd) {
      for (i = 0; i < OP_results(op); i++) {
	if (OP_result(op, i) == opnd) {
	  // Handle multiple identical results as necessary.
	  FmtAssert(result_idx == -1,
	    ("Adjust_one_eight_bit_reg: ASM has multiple identical results"));
	  is_opnd_and_result = TRUE;
	  result_idx = i;
	}
      }
    } else {
      // If result is for an operand that is both input and output, then it was
      // already handled when we process the operands.
      for (i = 0; i < OP_opnds(op); i++) {
	if (OP_opnd(op, i) == opnd)
	  return;
      }
    }

    ASM_OP_ANNOT* asm_info = (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op);
    ISA_REGISTER_SUBCLASS subclass =
      is_opnd ? ASM_OP_opnd_subclass(asm_info)[opnd_idx] :
		ASM_OP_result_subclass(asm_info)[opnd_idx];
    // If OPND is both input and output, try to get subclass from the output
    // constraint.
    if (subclass == ISA_REGISTER_SUBCLASS_UNDEFINED &&
	is_opnd &&
	is_opnd_and_result) {
      subclass = ASM_OP_result_subclass(asm_info)[result_idx];
    }
    if (subclass == ISA_REGISTER_SUBCLASS_m32_8bit_regs) {
      // Need a m32 byteable register, for both 32 and 64-bit target.
    } else if (Is_Target_64bit() ||
	       TN_size(opnd) != 1) {
      return;
    }

  } else {
    if (Is_Target_64bit())	// All integer registers are byteable under m64.
      return;
    if (is_opnd) {
      if (OP_opnd_size(op, opnd_idx) != 8)
	return;
    } else {
      if (OP_result_size(op, opnd_idx) != 8)
	return;
    }
  }

  const ISA_REGISTER_CLASS cl = TN_register_class( opnd );
  const REGISTER reg = LRA_TN_register( opnd );

  if( reg != REGISTER_UNDEFINED ){
    const REGISTER_SET regs =
      REGISTER_SUBCLASS_members(ISA_REGISTER_SUBCLASS_m32_8bit_regs);

    if( REGISTER_SET_MemberP( regs, reg ) )
      return;

  } else {
    if( TN_size(opnd) == 1 )
      return;
  }

  // Insert a mov.

  // Although RESULT must be assigned a byteble register, it does not mean the
  // size of RESULT is 1-byte.  An ASM can have a 'q' constraint for an operand
  // that is greater than 1-byte.  Bug 14468.
  TN *result = Gen_Register_TN(cl, TN_size(opnd));

  if (is_opnd) {
    OPS ops = OPS_EMPTY;
    Exp_COPY(result, opnd, &ops);
    OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
    BB_Insert_Ops_Before(bb, op, &ops);
    Set_OP_opnd(op, opnd_idx, result);
  }

  if (!is_opnd ||
      is_opnd_and_result) {
    // Do sign/zero extend instead of regular copy.  Needed for "sete" which
    // doesn't clear the upper bits.  Bug 5621.
    OPS ops = OPS_EMPTY;
    Exp_COPY_Ext(TN_size(result) == 2 ? TOP_movzwl : TOP_movzbl,
		 opnd, result, &ops);
    OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
    BB_Insert_Ops_After(bb, op, &ops);
    Set_OP_result(op, result_idx, result);

    // "sete" is no longer cond_def because movzbl extends sete's 1-byte result
    // to full 32 bits, making all 32 bits completely determined.  Bug 8850.
    Set_OP_cond_def_kind(op, OP_ALWAYS_UNC_DEF);
  }
}


static void
Adjust_eight_bit_regs (BB* bb)
{
  OP* op = NULL;

  // Introduce a move for those GTNs requiring 8-bit registers.

  FOR_ALL_BB_OPs(bb, op) {
    for (int i = 0; i < OP_opnds(op); i++) {
      Adjust_one_eight_bit_reg(bb, op, i, TRUE);
    }
    for (int i = 0; i < OP_results(op); i++) {
      Adjust_one_eight_bit_reg(bb, op, i, FALSE);
    }
  }
}


/* Convert any OP_x86_style op whose result and opnd0 have different registers
   into x86-style op, by introducing a mov. */
static void
Adjust_X86_Style_For_BB (BB* bb, BOOL* redundant_code, MEM_POOL* pool)
{
  OP *op = NULL;

  Setup_Live_Ranges( bb, TRUE, pool );

  int opnum = 0;

  FOR_ALL_BB_OPs( bb, op ){
    opnum++;

    if( !OP_x86_style( op ) )
      continue;

    TN* result = OP_result( op, 0 );
    TN* opnd0 = OP_opnd( op, 0 );
    TN* opnd1 = OP_opnd( op, 1 );

    if( TNs_Are_Equivalent( result, opnd0 ) )
      continue;

    if( ( opnd0 != opnd1 ) &&
	TOP_is_commutative( OP_code(op) ) ){
      const LIVE_RANGE* opnd1_lr = LR_For_TN( opnd1 );

      if( TNs_Are_Equivalent( result, opnd1 ) ||
	  LR_last_use(opnd1_lr) == opnum ){
	Set_OP_opnd( op, 0, opnd1 );
	Set_OP_opnd( op, 1, opnd0 );
	opnd0 = opnd1;

	if( TNs_Are_Equivalent( result, opnd0 ) )
	  continue;
      }
    }

    /* Situation 1 where result != opnd0
       before: result = opnd0 OP opnd1
       after: result = opnd0
              result = result OP opnd1

       Situation 2 where result == opnd1, or opnd1 has a register
       before: result = opnd0 OP result
       after:  tmp = result
               result = opnd0
	       result = result OP tmp
    */

    /* The original thought is using the following transform format as
       result_tn` = opnd0
       result_tn = result_tn` OP opnd1
       with the assumption that result_tn will not be defined more than
       once.

       However, after GRA gets involved, this assumption does not hold
       anymore. Thus, the following transform format is used instead
       result_tn = opnd0
       result_tn = result_tn OP opnd1
       given the fact that result_tn could be re-defined whatever.
    */

    if( Do_LRA_Trace(Trace_LRA_Detail) ){
      fprintf( TFile, "Convert op to x86-style: " );
      Print_OP_No_SrcLine( op );
    }

    TN* tmp_opnd0;
    BOOL add_copy_after = FALSE;
    if (TN_register_class(result) == ISA_REGISTER_CLASS_float &&
	TN_register_class(opnd0) == ISA_REGISTER_CLASS_float &&
	TN_size(result) < TN_size(opnd0)) {
      // This is for OPs resulting from operations such as REDUCE_ADD whose
      // result type is smaller than the source type.  F8V16F8REDUCE_ADD
      // translates into the SSE instruction "haddpd":
      //
      //   result = haddpd opnd0,opnd1
      //
      // where result is 8-byte while opnd0 and opnd1 are 16-byte.  Convert to
      // x86-style as follows:
      //
      //   tmp_opnd0 = opnd0			; copy #1
      //   tmp_opnd0 = haddpd tmp_opnd0,opnd1
      //   result = tmp_opnd0			; copy #2
      //
      // tmp_opnd0 is 16-byte.  Copy #2 is inserted to convert between the two
      // different sizes in an orderly manner that fits in the LRA framework;
      // the copy will be deleted once LRA assigns result and tmp_opnd0 to the
      // same register.
      //
      // (This case cannot be handled like a regular instruction with
      // equal-size source and result.  If we first copy opnd0 to result as for
      // a regular instruction, the code becomes:
      //
      //    result = opnd0			; result is 8-byte
      //    result = haddpd result,opne1
      //
      // The first operand of haddpd is now "result", which is 8-byte, which is
      // wrong.  If this operand is later spilled, LRA will use movsd instead
      // of movdq to spill it, causing the upper 8 bytes to be zero'd.
      
      tmp_opnd0 = Build_TN_Like(opnd0);
      Set_OP_result(op, 0, tmp_opnd0);
      add_copy_after = TRUE;
    } else {
      tmp_opnd0 = result;
    }

    OPS ops = OPS_EMPTY;

    for( int opnd = 1; opnd < OP_opnds(op); opnd++ ){
      TN* opnd1 = OP_opnd( op , opnd );

      if( TN_is_register( opnd1 ) &&
	  TNs_Are_Equivalent( opnd1, result ) ){
	TN* tmp_opnd1 = Build_TN_Like( opnd1 );
	Exp_COPY( tmp_opnd1, opnd1, &ops );
	OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
	Set_OP_opnd( op, opnd, tmp_opnd1 );
      }
    }

    Exp_COPY( tmp_opnd0, opnd0, &ops );
    OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
    BB_Insert_Ops_Before( bb, op, &ops );
    *redundant_code = TRUE;

    if (add_copy_after) {
      OPS ops = OPS_EMPTY;
      Exp_COPY(result, tmp_opnd0, &ops);
      OP_srcpos(OPS_last(&ops)) = OP_srcpos(op);
      BB_Insert_Ops_After(bb, op, &ops);
    }

    for( int opnd = 1; opnd < OP_opnds(op); opnd++ ){
      if( OP_opnd( op , opnd ) == OP_opnd( op, 0 ) )
	Set_OP_opnd( op, opnd, tmp_opnd0 );
    }

    Set_OP_opnd( op, 0, tmp_opnd0 );
  }  
}


static void Verify_TARG_X86_Op_For_BB( BB* bb )
{
  OP* op = NULL;

  FOR_ALL_BB_OPs( bb, op ){
    const TOP top = OP_code( op );

    if( top == TOP_asm )
      continue;

    /* Verify the 8-bit-reg requirement. */
    if( Is_Target_32bit() ){
      for( int i = 0; i < OP_opnds( op ); i++ ){
	TN* opnd = OP_opnd( op, i );
	if( TN_is_register( opnd ) &&
	    OP_opnd_size( op, i ) == 8 ){
	  const ISA_REGISTER_CLASS cl = TN_register_class( opnd );
	  const REGISTER reg = LRA_TN_register( opnd );
	  const REGISTER_SET regs =
		 REGISTER_SUBCLASS_members(ISA_REGISTER_SUBCLASS_m32_8bit_regs);
	  FmtAssert(cl == ISA_REGISTER_CLASS_integer,
		    ("Verify_TARG_X86_Op_For_BB: 8-bit value not allocated to integer register"));
	  FmtAssert(REGISTER_SET_MemberP(regs, reg),
		    ("Verify_TARG_X86_Op_For_BB: 8-bit value not allocated to byte-accessible register"));
	}
      }
    }

    /* Make sure the index register is not %rsp. */
    if( top == TOP_leax32 || top == TOP_leax64 ){
      int index = TOP_Find_Operand_Use( top, OU_index );
      REGISTER reg = LRA_TN_register( OP_opnd(op,index) );
      FmtAssert( reg != RSP, ("%rsp can not serve as index register in lea") );
    }

    if( OP_x86_style( op ) ){
      TN* result = OP_result( op, 0 );
      TN* opnd0 = OP_opnd( op, 0 );

      FmtAssert( TN_register_class(result) == TN_register_class(opnd0),
		 ("Result and opnd0 have different register class.") );
      FmtAssert( LRA_TN_register(result) != REGISTER_UNDEFINED,
		 ("Result has no register.") );
      FmtAssert( TNs_Are_Equivalent( result, opnd0 ),
		 ("The first opnd and result use different registers.") );
    }
  }
}


// Return TRUE if TN is a x87/MMX TN that needs to spill around OP because OP
// clobbers TN's register class.
static BOOL
Spillpoint_For_x87_MMX (TN *tn, OP *op)
{
  ISA_REGISTER_CLASS cl = TN_register_class(tn);

  if ((cl == ISA_REGISTER_CLASS_x87 && OP_mmx(op)) ||
      (cl == ISA_REGISTER_CLASS_mmx && OP_x87(op)))
    return TRUE;
  return FALSE;
}


// Presplit x87 live ranges around MMX OPs, and presplit MMX live ranges around
// x87 OPs.  An x87 live range cannot span a MMX OP because MMX OPs clobber all
// x87 registers, causing LRA to die after finding no useable registers for the
// live range.  Vice versa for MMX live ranges.
static void
Presplit_x87_MMX_Live_Ranges(BB *bb, MEM_POOL *pool)
{
  int opnum;
  LIVE_RANGE *lr;

  Setup_Live_Ranges(bb, TRUE, pool);
  for (lr = Live_Range_List; lr != NULL; lr = LR_next(lr)) {
    TN *tn = LR_tn(lr);
    ISA_REGISTER_CLASS cl = TN_register_class(tn);
    if (cl == ISA_REGISTER_CLASS_x87 ||
	cl == ISA_REGISTER_CLASS_mmx) {
      for (opnum = LR_last_use(lr) - 1; opnum > LR_first_def(lr); opnum--) {
	OP *op = OP_VECTOR_element(Insts_Vector, opnum);
        if (op != NULL &&
	    Spillpoint_For_x87_MMX(tn, op)) {
	  // Spill as necessary starting from beginning of BB up to OPNUM.
	  Spill_Live_Range(bb, lr, -1 /* unused */, opnum,
			   Spillpoint_For_x87_MMX);
	  break;
	}
      }
    }
  }
}
#endif	// TARG_X8664

#ifdef KEY
// If the BB has an ASM, see if its clobber set is so large that it may cause
// LRA to run out of registers.
static void
Detect_large_asm_clobber (BB *bb)
{
  ISA_REGISTER_CLASS cl;
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    large_asm_clobber_set[cl] = FALSE;
  }

  if (BB_asm(bb)) {
    ASM_OP_ANNOT* asm_info =
      (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, BB_last_op(bb));

    // cannot use registers clobbered by an ASM statement
    if (asm_info) {
      FOR_ALL_ISA_REGISTER_CLASS(cl) {
	int total = REGISTER_SET_Size(avail_set[cl]);
	int clobbered = REGISTER_SET_Size(ASM_OP_clobber_set(asm_info)[cl]);
	if (clobbered > 3 &&		// arbitrary number
	    total - clobbered < 2) {
	  large_asm_clobber_set[cl] = TRUE;
	}
      }
    }
  }
}
#endif	// KEY

#ifdef TARG_X8664
// Make DED_TN available for other OPs to use, by saving/restoring it to a temp
// TN.  DED_TN must be available immediately after START_OP, and continue to be
// available to just before END_OP.
static void
Make_Dedicated_Available (TN *ded_tn, OP *start_op, OP *end_op,
			  LIVE_RANGE *ded_lr = NULL)
{
  Is_True(ded_tn != NULL && TN_is_register(ded_tn) && TN_is_dedicated(ded_tn),
	  ("Make_Dedicated_Available: not dedicated TN"));
  Is_True(REGISTER_allocatable(TN_register_class(ded_tn),
			       LRA_TN_register(ded_tn)),
	  ("Make_Dedicated_Available: TN not allocatable"));

  Is_True(end_op != NULL, ("Make_Dedicated_Available: unexpected end OP"));

  BB *bb = OP_bb(end_op);

  // Get live range if the caller didn't provide it.
  if (ded_lr == NULL)
    ded_lr = LR_For_TN(ded_tn);

  Is_True(ded_lr != NULL, ("Make_Dedicated_Available: cannot find LR"));

  TN *new_tn = Build_TN_Like(ded_tn);


  // START_OP NULL means live-in.
  if (start_op != NULL) {
    Set_OP_result(start_op, 0, new_tn);
  } else {
    OPS ops = OPS_EMPTY;
    Exp_COPY(new_tn, ded_tn, &ops);
    OP_srcpos(OPS_last(&ops)) = OP_srcpos(BB_first_op(bb));
    BB_Insert_Ops_Before(bb, BB_first_op(bb), &ops);
  }

  // Copy back to the dedicated TN.
  OPS ops = OPS_EMPTY;
  Exp_COPY(ded_tn, new_tn, &ops);
  OP_srcpos(OPS_last(&ops)) = OP_srcpos(end_op);
  BB_Insert_Ops_Before(bb, end_op, &ops);

  // Replace uses of DED_TN with NEW_TN.
  OP *op;
  for (op = (start_op == NULL) ?
	      OP_next(BB_first_op(bb)) :  // BB_first_op is the copy just added
	      OP_next(start_op);
       op != OP_prev(end_op);		  // OP_prev is the restore
       op = OP_next(op)) {
    for (int opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
      TN *opnd_tn = OP_opnd(op, opndnum);
      if (TN_is_register(opnd_tn) &&
	  TNs_Are_Equivalent(opnd_tn, ded_tn)) {
	Set_OP_opnd(op, opndnum, new_tn);
      }
    }

#if Is_True_On
    // There shouldn't be any definition of tn between op and the restore.
    for (int resnum = 0; resnum < OP_results(op); resnum++) {
      TN *result_tn = OP_result(op, resnum);
      FmtAssert(!TNs_Are_Equivalent(result_tn, ded_tn),
		("Make_Dedicated_Available: unexpected definition of TN"));
    }
#endif
  }
}

// Free up live-in or live-out dedicated TNs so that they can be preallocated
// to other TNs.
//
// For example, in the case of parameter and return registers, the WHIRL
// defines the dedicated TN directly.  Replace these definitions with regular
// TNs, and copy to the dedicated TNs at the end of the BB.  (Bug 12744.)
// Change:
//   rax = ...		; rax is return register
//   ... = idiv ...	; operand and results need preallocation
// to:
//   new_tn = ...
//   ... = idiv ...	; idiv can now use rax
//   rax = new_tn
static void
Make_Dedicated_TNs_Available (BB *bb)
{
  // Identify the first and last OP in the BB which must use a particular
  // dedicated register.
  typedef struct {
    INT first_prealloc_opnum[REGISTER_MAX+1];
    INT last_prealloc_opnum[REGISTER_MAX+1];
  } PREALLOC_OPNUMS;

  PREALLOC_OPNUMS prealloc_opnums[ISA_REGISTER_CLASS_MAX+1];

  INT opnum, i;
  INT orig_bb_length = BB_length(bb);

  // Identify OPs requiring preallocation.
  memset(prealloc_opnums, 0, sizeof(prealloc_opnums));
  for (opnum = orig_bb_length; opnum > 0; opnum--) {
    OP *op = OP_VECTOR_element(Insts_Vector, opnum);

    // Don't insert copies after the SP adjust OP at the end of the BB.  LRA
    // calls CGSPILL_Append_Ops to spill around the BB, and CGSPILL_Append_Ops
    // relies on the SP ajust to mark the end of the BB.  Bug 14363.
    if ((BB_exit(bb) &&
	 OP_code(op) == TOP_spadjust) ||
	OP_code(op) == TOP_asm) {	// the ASM at the end of the BB
      continue;
    }

    ASM_OP_ANNOT *asm_info = (OP_code(op) == TOP_asm) ?
      (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op) : NULL;

    // operands
    for (i = 0; i < OP_opnds(op); i++) {
      ISA_REGISTER_SUBCLASS subclass = asm_info ?
        ASM_OP_opnd_subclass(asm_info)[i] : OP_opnd_reg_subclass(op, i);
      REGISTER reg = Single_Register_Subclass(subclass);
      if (reg == REGISTER_UNDEFINED)
        continue;

      TN *tn = OP_opnd(op, i);
      ISA_REGISTER_CLASS cl = TN_register_class(tn);
      prealloc_opnums[cl].first_prealloc_opnum[reg] = opnum;
      if (prealloc_opnums[cl].last_prealloc_opnum[reg] == 0)
	prealloc_opnums[cl].last_prealloc_opnum[reg] = opnum;
    }

    // results
    for (i = 0; i < OP_results(op); i++) {
      ISA_REGISTER_SUBCLASS subclass = asm_info ?
        ASM_OP_result_subclass(asm_info)[i] : OP_result_reg_subclass(op, i);
      REGISTER reg = Single_Register_Subclass(subclass);
      if (reg == REGISTER_UNDEFINED)
        continue;

      TN *tn = OP_result(op, i);
      ISA_REGISTER_CLASS cl = TN_register_class(tn);
      prealloc_opnums[cl].first_prealloc_opnum[reg] = opnum;
      if (prealloc_opnums[cl].last_prealloc_opnum[reg] == 0)
	prealloc_opnums[cl].last_prealloc_opnum[reg] = opnum;
    }
  }

  // Free up dedicated TNs to make them available for preallocation.
  for (opnum = 1; opnum <= orig_bb_length; opnum++) {
    OP *op = OP_VECTOR_element(Insts_Vector, opnum);

    // Free up live-in dedicated TNs.  These have exposed uses.
    for (i = 0; i < OP_opnds(op); i++) {
      TN *tn = OP_opnd(op, i);
      if (TN_is_register(tn) &&
	  TN_is_dedicated(tn) &&
	  REGISTER_allocatable(TN_register_class(tn), LRA_TN_register(tn))) {
	ISA_REGISTER_CLASS cl = TN_register_class(tn);
	REGISTER reg = LRA_TN_register(tn);
	LIVE_RANGE *ded_lr = LR_For_TN(tn);
	if (LR_exposed_use(ded_lr) == opnum &&
	    prealloc_opnums[cl].first_prealloc_opnum[reg] > 0 &&
	    prealloc_opnums[cl].first_prealloc_opnum[reg] < opnum) {
	  OP *last_prealloc_op =
	    OP_VECTOR_element(Insts_Vector,
			      prealloc_opnums[cl].last_prealloc_opnum[reg]);
	  OP *end_op = OP_next(last_prealloc_op);
	  Make_Dedicated_Available(tn, NULL, end_op, ded_lr);
	}
      }
    }

    // Free up live-out dedicated TNs defined in the BB.
    for (i = 0; i < OP_results(op); i++) {
      TN *tn = OP_result(op, i);
      if (TN_is_register(tn) &&
	  TN_is_dedicated(tn) &&
	  REGISTER_allocatable(TN_register_class(tn), LRA_TN_register(tn))) {
	ISA_REGISTER_CLASS cl = TN_register_class(tn);
	REGISTER reg = LRA_TN_register(tn);
	LIVE_RANGE *ded_lr = LR_For_TN(tn);
	// Find definition of parameter and function return registers.  They
	// should have one definition and one non-exposed-use (live-out counts
	// as one use).
	//
	// For ASM BBs, the parameter to the ASM has two non-exposed-use's
	// (appearance in ASM opnd, and live-out).  Bug 14432.
	if (LR_last_use(ded_lr) > orig_bb_length &&	// live-out
	    LR_def_cnt(ded_lr) == 1 &&
	    prealloc_opnums[cl].last_prealloc_opnum[reg] > opnum) {
	  OP *last_prealloc_op =
	    OP_VECTOR_element(Insts_Vector,
			      prealloc_opnums[cl].last_prealloc_opnum[reg]);
	  OP *end_op = OP_next(last_prealloc_op);
	  Make_Dedicated_Available(tn, op, end_op, ded_lr);
	}
      }
    }
  }
}
#endif	// TARG_X8664

static void
Preallocate_Single_Register_Subclasses (BB* bb)
{
  OP* op;

#ifdef KEY
  INT orig_bb_length = BB_length(bb);
  INT opnum;

  Setup_Live_Ranges(bb, TRUE, &lra_pool);

#ifdef TARG_X8664
  Make_Dedicated_TNs_Available(bb);
#endif

  for (opnum = 1; opnum <= orig_bb_length; opnum++)
#else
  FOR_ALL_BB_OPs (bb, op)
#endif
  {

#ifdef KEY
    op = OP_VECTOR_element(Insts_Vector, opnum);
#endif

    ASM_OP_ANNOT* asm_info = (OP_code(op) == TOP_asm) ?
      (ASM_OP_ANNOT*) OP_MAP_Get(OP_Asm_Map, op) : NULL;

    INT i;    
    for (i = 0; i < OP_opnds(op); i++) {
      ISA_REGISTER_SUBCLASS subclass = asm_info ?
        ASM_OP_opnd_subclass(asm_info)[i] : OP_opnd_reg_subclass(op, i);
      REGISTER reg = Single_Register_Subclass(subclass);
      if (reg == REGISTER_UNDEFINED) {
        continue;
      }
      TN* old_tn = OP_opnd(op, i);
#ifdef TARG_X8664
      if( TN_is_preallocated( old_tn ) ){
	continue;
      }
#else
      if (LRA_TN_register(old_tn) != REGISTER_UNDEFINED) {
        Is_True(LRA_TN_register(old_tn) == reg,
                ("LRA: wrong register allocated to TN"));
        continue;
      }
#endif
      TN* new_tn = Build_TN_Like(old_tn);
      LRA_TN_Allocate_Register(new_tn, reg);
#ifdef TARG_X8664
      Set_TN_is_preallocated (new_tn);
      Set_OP_opnd( op, i, new_tn );
#endif /* TARG_X8664 */
      OPS pre_ops = OPS_EMPTY;

      for( int j = i+1; j < OP_opnds(op); j++ ){
	TN* opnd = OP_opnd( op, j );
	if( TN_is_register(opnd) &&
	    LRA_TN_register(opnd) == reg ){
	  /* Copy the opnd before its value is over-written. */
	  TN* tmp_tn = Build_TN_Like( opnd );
	  Exp_COPY( tmp_tn, opnd, &pre_ops );
	  OP_srcpos(OPS_last(&pre_ops)) = OP_srcpos(op);
          Set_OP_opnd( op, j, tmp_tn );
	}
      }

      BOOL has_def = FALSE;
      TN* new_result_tn = NULL;
      for( int j = 0; j < OP_results(op); j++ ){
        if (OP_result(op, j) == old_tn) {
	  new_result_tn = Build_TN_Like( OP_result(op,j) );
          Set_OP_result(op, j, new_result_tn);
          has_def = TRUE;
        }
      }

      Exp_COPY(new_tn, old_tn, &pre_ops);
      OP_srcpos(OPS_last(&pre_ops)) = OP_srcpos(op);

#ifdef TARG_X8664
      /* Bug_151:
	 Maintain the "result==OP_opnd(op,0)" property for x86-style operations
	 after register preallocation.
      */
      if( OP_x86_style( op ) &&
	  new_result_tn != NULL ){
	Exp_COPY( new_result_tn, old_tn, &pre_ops );
	OP_srcpos(OPS_last(&pre_ops)) = OP_srcpos(op);
	Set_OP_opnd( op, 0, new_result_tn );
      }
#endif

      BB_Insert_Ops_Before(bb, op, &pre_ops);
      if (has_def) {
        OPS post_ops = OPS_EMPTY;
        Exp_COPY(old_tn, new_result_tn, &post_ops);
	OP_srcpos(OPS_last(&post_ops)) = OP_srcpos(op);
        BB_Insert_Ops_After(bb, op, &post_ops);
      }
    }

    /* latest_new_op records the last appended op to <op>. */
    OP* latest_new_op = NULL;

    for (i = 0; i < OP_results(op); i++) {
      ISA_REGISTER_SUBCLASS subclass = asm_info ?
        ASM_OP_result_subclass(asm_info)[i] : OP_result_reg_subclass(op, i);
      REGISTER reg = Single_Register_Subclass(subclass);
      if (reg == REGISTER_UNDEFINED) {
        continue;
      }
      TN* old_tn = OP_result(op, i);

#ifdef TARG_X8664
      if( TN_is_sp_reg( old_tn ) ){
	continue;
      }

      if( TN_is_preallocated( old_tn ) ){
	continue;
      }
#endif

#ifndef TARG_X8664
      if (LRA_TN_register(old_tn) != REGISTER_UNDEFINED) {
        Is_True(LRA_TN_register(old_tn) == reg,
                ("LRA: wrong register allocated to TN"));
        continue;
      }
#endif
      TN* new_tn = Build_TN_Like(old_tn);
      LRA_TN_Allocate_Register(new_tn, reg);
#ifdef TARG_X8664
      Set_TN_is_preallocated (new_tn);
#endif /* TARG_X8664 */
      INT j;    
      for (j = 0; j < OP_results(op); j++) {
        if (OP_result(op, j) == old_tn) {
          Set_OP_result(op, j, new_tn);
        }
      }
      BOOL has_use = FALSE;
      TN* new_opnd_tn = NULL;
      for (j = 0; j < OP_opnds(op); j++) {
        if (OP_opnd(op, j) == old_tn) {
	  new_opnd_tn = Build_TN_Like( OP_opnd(op,j) );
          Set_OP_opnd(op, j, new_opnd_tn);
          has_use = TRUE;
        }
      }
      OPS post_ops = OPS_EMPTY;
      Exp_COPY(old_tn, new_tn, &post_ops);
      OP_srcpos(OPS_last(&post_ops)) = OP_srcpos(op);

      if( latest_new_op == NULL ||
	  LRA_TN_register(old_tn) == REGISTER_UNDEFINED ){
	BB_Insert_Ops_After(bb, op, &post_ops);
	latest_new_op = OPS_last( &post_ops );

      } else {
	/* If <old_tn> is a dedicated one, then we must
	   insert the copy at the latest place to avoid splitting
	   the register live range. */
	BB_Insert_Ops_After(bb, latest_new_op, &post_ops);
	latest_new_op = NULL;
      }

      if (has_use) {
        OPS pre_ops = OPS_EMPTY;
        Exp_COPY(new_opnd_tn, old_tn, &pre_ops);
	OP_srcpos(OPS_last(&pre_ops)) = OP_srcpos(op);
        BB_Insert_Ops_Before(bb, op, &pre_ops);
      }
    }
  }
}

/* ======================================================================
 * LRA_Allocate_Registers
 *
 * Main driver for the local register allocation module. 
 * ======================================================================*/
void
LRA_Allocate_Registers (BOOL lra_for_pu)
{
  BB *bb;
  RID *rid;
  ISA_REGISTER_CLASS cl;

  Set_Error_Phase ( "Local Register Allocation" );
  Start_Timer ( T_LRA_CU );

#if Is_True_On 
  Consistency_Check ();
#endif

#ifdef TARG_ST
  /* Reset the the Callee_Saved_Regs_Used for the procedure. */
  FOR_ALL_ISA_REGISTER_CLASS(cl)
    Callee_Saved_Regs_Used[cl] = REGISTER_SET_EMPTY_SET;
#endif
/* Initialize the unused TN definitions. */
  FOR_ALL_ISA_REGISTER_CLASS(cl) unused_tn_def[cl] = NULL;
  if (True_TN) unused_tn_def[TN_register_class(True_TN)] = True_TN;
  if (Zero_TN) unused_tn_def[TN_register_class(Zero_TN)] = Zero_TN;
  if (FZero_TN) unused_tn_def[TN_register_class(FZero_TN)] = FZero_TN;

  FOR_ALL_ISA_REGISTER_CLASS(cl) last_assigned_reg[cl] = REGISTER_UNDEFINED;
  HB_Schedule *Sched = NULL;
  global_spills = 0;

#ifdef TARG_X8664
  /* When LRA spills a global register, LRA does not know the size of that global
     register, and always assumes its size is the default value
     (which is 64-bit for x86-64 and 32-bit for x86). Such spilling will lead to
     only half of the 128-bit value (coming from simd) is spilled.

     My current fix is to scan through all the BBs, and use float_reg_could_be_128bit
     to indicate whether the size of a float register could be 128-bit or not.
     (bug#2801)

     TODO:
     Look for an accurate way to get the global register info.
   */
  memset( (void*)float_reg_could_be_128bit, sizeof(float_reg_could_be_128bit), 0 );

  if( Is_Target_SSE2() &&
      !CG_localize_tns &&
      LNO_Run_Simd ){

    for( BB* bb = REGION_First_BB; bb != NULL; bb = BB_next(bb) ){
      if( BB_reg_alloc(bb) )
	continue;
      for( OP* op = BB_first_op(bb); op != NULL; op = OP_next(op) ){
	if( !TOP_is_vector_op( OP_code(op) ) )
	  continue;

	for( int i = 0; i < OP_results(op); i++ ){
	  TN* result = OP_result( op, i );
	  if( TN_is_global_reg(result ) &&
	      TN_size(result) == 16 ){
	    const REGISTER reg = LRA_TN_register(result);
	    float_reg_could_be_128bit[reg] = true;
	  }
	} // for each result

      } // for each OP
    }  // for each BB
  }
#endif

  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) 
  {
    if (    ( rid = BB_rid(bb) )
         && ( RID_level(rid) >= RL_CGSCHED ) )
      continue;

    /* Someone already did it -- probably SWP */
    if ( BB_reg_alloc(bb) )
      continue;

    Trace_LRA = Get_Trace (TP_ALLOC, 0x01, bb);
    Trace_LRA_Detail = Get_Trace (TP_ALLOC, 0x02, bb);
    Trace_LRA_Spill = Get_Trace (TP_ALLOC, 0x04, bb);
    Trace_LRA_Entry_Exit = Get_Trace (TP_ALLOC, 0x08, bb);
    Trace_Move_GRA_Spills = Get_Trace (TP_ALLOC, 0x10, bb);

#if defined(TARG_IA32)
    Preallocate_Single_Register_Subclasses (bb);
#endif

    Alloc_Regs_For_BB (bb, Sched);
  }

  // If we are not running GRA, we need to generate the instructions to
  // save and restore callee saved registers.
  if (lra_for_pu && CG_localize_tns) {
    Spill_Callee_Saved_Regs ();
  }

  Check_for_Dump (TP_ALLOC, NULL);
  Stop_Timer ( T_LRA_CU );
}


#ifdef TARG_ST
static LRA_Request_Info *Reg_Request_Table;
#define Reg_Request(bb) (Reg_Request_Table[BB_id(bb)])
#else
static mINT8 *Reg_Request_Table;
#define Reg_Request(bb,cl)      \
  (Reg_Request_Table[BB_id(bb)*ISA_REGISTER_CLASS_COUNT + cl - ISA_REGISTER_CLASS_MIN])
#endif

static INT Reg_Table_Size;
#ifdef TARG_ST
struct Reg_SubClass_Info {
  int n_superclasses;
  // The register subclasses which are a superset of this class.
  // For convenience, arrange that a subclass is a superset of itself,
  // and that the subclass ISA_REGISTER_SUBCLASS_UNDEFINED is used for a
  // register of a class with no subclass restrictions.
  // Because we use ISA_REGISTER_SUBCLASS_UNDEFINED, we have to
  // be careful with loops over subclass arrays, because the
  // standard macro, FOR_ALL_ISA_REGISTER_SUBCLASS, does not
  // include the undefined subclass.
  ISA_REGISTER_SUBCLASS superclass[ISA_REGISTER_SUBCLASS_MAX_LIMIT + 1];
  int n_subclasses;
  // The register subclasses which are a proper subset of this class.
  ISA_REGISTER_SUBCLASS subclass[ISA_REGISTER_SUBCLASS_MAX_LIMIT + 1];
  // The difference in size between the Ith subclass and this class.
  INT subclass_diff[ISA_REGISTER_SUBCLASS_MAX_LIMIT + 1];
  // The number of registers in a tn used as an operand with this subclass
  // type.
  int nregs;
} subclass_info[ISA_REGISTER_CLASS_MAX_LIMIT+1][ISA_REGISTER_SUBCLASS_MAX_LIMIT+1];
static BOOL subclass_info_initialized = FALSE;

struct Reg_SubClass_Order {
  int n_subclasses;
  ISA_REGISTER_SUBCLASS ordered_subclasses[ISA_REGISTER_SUBCLASS_MAX_LIMIT+1];
} subclass_order[ISA_REGISTER_CLASS_MAX_LIMIT+1];
static BOOL subclass_order_initialized = FALSE;

// These arrays record register usage as we scan a BB.
static INT max_usage[ISA_REGISTER_CLASS_MAX_LIMIT+1][ISA_REGISTER_SUBCLASS_MAX_LIMIT+1];
static INT current_usage[ISA_REGISTER_CLASS_MAX_LIMIT+1][ISA_REGISTER_SUBCLASS_MAX_LIMIT+1];

static int
compare_subclass_size (const void *p1, const void *p2)
{
  ISA_REGISTER_SUBCLASS sc1 = *(const ISA_REGISTER_SUBCLASS *)p1;
  ISA_REGISTER_SUBCLASS sc2 = *(const ISA_REGISTER_SUBCLASS *)p2;
  INT size1 = subclass_size[sc1];
  INT size2 = subclass_size[sc2];
  if (size1 < size2) {
    return -1;
  } else if (size1 > size2) {
    return +1;
  } else {
    // If they have equal size, then order just by subclass enumeration order.
    return (int)sc1 - (int)sc2;
  }
}

static void
init_subclass_order (void)
{
  ISA_REGISTER_CLASS cl;
  ISA_REGISTER_SUBCLASS sc;

  FOR_ALL_ISA_REGISTER_CLASS (cl) {
    subclass_order[cl].ordered_subclasses[0] = ISA_REGISTER_SUBCLASS_UNDEFINED;
    subclass_order[cl].n_subclasses = 1;
  }
  subclass_size[ISA_REGISTER_SUBCLASS_UNDEFINED] = ISA_REGISTER_MAX + 1;
  FOR_ALL_ISA_REGISTER_SUBCLASS (sc) {
    cl = REGISTER_SUBCLASS_register_class (sc);
    subclass_size[sc] = REGISTER_SET_Size (REGISTER_SUBCLASS_members (sc));
    subclass_order[cl].ordered_subclasses[subclass_order[cl].n_subclasses++] = sc;
  }
  // Sort the subclasses so that they are listed smallest first,
  // then we are guaranteed that subclasses appear before their
  // superclasses, which is the order we require.
  INT rank = 0;
  FOR_ALL_ISA_REGISTER_CLASS (cl) {
    qsort (subclass_order[cl].ordered_subclasses,
	   subclass_order[cl].n_subclasses,
	   sizeof (subclass_order[0].ordered_subclasses[0]),
	   compare_subclass_size);
    for (INT i = 0; i < subclass_order[cl].n_subclasses; i++) {
      subclass_rank[subclass_order[cl].ordered_subclasses[i]] = rank++;
    }
  }
  if (Get_Trace (TP_ALLOC, 0x01)) {
    FOR_ALL_ISA_REGISTER_CLASS (cl) {
      fprintf (TFile, "%s: ordered subclasses:", REGISTER_CLASS_name (cl));
      for (INT i = 0; i < subclass_order[cl].n_subclasses; i++) {
	fprintf (TFile, " %s",
		 REGISTER_SUBCLASS_name (subclass_order[cl].ordered_subclasses[i]));
      }
      fprintf (TFile, "\n");
    }
  }
  subclass_order_initialized = TRUE;
}

static void
init_subclass_info (void)
{
  ISA_REGISTER_CLASS cl;
  ISA_REGISTER_SUBCLASS sc1, sc2;

  FOR_ALL_ISA_REGISTER_CLASS (cl) {
    subclass_info[cl][ISA_REGISTER_SUBCLASS_UNDEFINED].nregs = 1;
    subclass_info[cl][ISA_REGISTER_SUBCLASS_UNDEFINED].superclass[0] =
      ISA_REGISTER_SUBCLASS_UNDEFINED;
    subclass_info[cl][ISA_REGISTER_SUBCLASS_UNDEFINED].n_superclasses = 1;
  }

  FOR_ALL_ISA_REGISTER_CLASS (cl) {
    REGISTER_SET cl_universe = REGISTER_CLASS_universe (cl);

    FOR_ALL_ISA_REGISTER_SUBCLASS_AND_UNDEFINED (sc1) {
      if (sc1 != ISA_REGISTER_SUBCLASS_UNDEFINED
	  && REGISTER_SUBCLASS_register_class (sc1) != cl)
	continue;
      
      REGISTER_SET sc1_members = ((sc1 == ISA_REGISTER_SUBCLASS_UNDEFINED)
				  ? cl_universe
				  : REGISTER_SUBCLASS_members (sc1));
      INT n_superclasses = 0;
      INT n_subclasses = 0;

      subclass_info[cl][sc1].nregs = 1;

      FOR_ALL_ISA_REGISTER_SUBCLASS_AND_UNDEFINED (sc2) {
	if (sc2 != ISA_REGISTER_SUBCLASS_UNDEFINED
	    && REGISTER_SUBCLASS_register_class (sc2) != cl)
	  continue;

	REGISTER_SET sc2_members = ((sc2 == ISA_REGISTER_SUBCLASS_UNDEFINED)
				    ? cl_universe
				    : REGISTER_SUBCLASS_members (sc2));
	if (REGISTER_SET_ContainsP (sc2_members, sc1_members)) {
	  subclass_info[cl][sc1].superclass[n_superclasses++] = sc2;
	} else if (REGISTER_SET_ContainsP (sc1_members, sc2_members)) {
	  // sc2 is a proper subset of sc1.
	  INT diff = REGISTER_SET_Size (sc1_members) - REGISTER_SET_Size (sc2_members);
	  subclass_info[cl][sc1].subclass[n_subclasses] = sc2;
	  subclass_info[cl][sc1].subclass_diff[n_subclasses++] = diff;
	}	
      
	subclass_info[cl][sc1].n_superclasses = n_superclasses;
	subclass_info[cl][sc1].n_subclasses = n_subclasses;
      }
    }
  }
  subclass_info_initialized = TRUE;
}

static void init_live (void)
{
  ISA_REGISTER_CLASS cl;
  ISA_REGISTER_SUBCLASS sc;
  FOR_ALL_ISA_REGISTER_CLASS (cl) {
    FOR_ALL_ISA_REGISTER_SUBCLASS_AND_UNDEFINED (sc) {
      max_usage[cl][sc] = 0;
      current_usage[cl][sc] = 0;
    }
  }
}

static void add_live (ISA_REGISTER_CLASS cl, ISA_REGISTER_SUBCLASS sc, int nregs)
{
  Reg_SubClass_Info *info = &subclass_info[cl][sc];
  for (INT i = 0 ; i < info->n_superclasses; i++) {
    ISA_REGISTER_SUBCLASS super = info->superclass[i];
    current_usage[cl][super] += nregs;
    if (current_usage[cl][super] > max_usage[cl][super]) {
      max_usage[cl][super] = current_usage[cl][super];
    }
  }
  if (info->nregs < nregs) info->nregs = nregs;
}

static void ensure_live(ISA_REGISTER_CLASS cl, ISA_REGISTER_SUBCLASS sc, int nregs)
{
  const Reg_SubClass_Info *info = &subclass_info[cl][sc];
  for (INT i = 0 ; i < info->n_superclasses; i++) {
    ISA_REGISTER_SUBCLASS super = info->superclass[i];
    if (max_usage[cl][super] < nregs) {
      max_usage[cl][super] = nregs;
    }
  }
}

static void remove_live (ISA_REGISTER_CLASS cl, ISA_REGISTER_SUBCLASS sc, int nregs)
{
  const Reg_SubClass_Info *info = &subclass_info[cl][sc];
  for (INT i = 0 ; i < info->n_superclasses; i++) {
    ISA_REGISTER_SUBCLASS super = info->superclass[i];
    current_usage[cl][super] -= nregs;
    // Note: that current usage can go negative: this can happen when
    // we free up the registers associated with a global TN, making them
    // available for local allocation.
  }
  // Note also that freeing up a member of sc can also give us greater
  // precision on the requirement for a subclass of sc.
  // Consider sc2, a subclass of sc that has N fewer members; then
  //    current_usage(sc2) <= (current_usage(sc)+N)
  // This is useful because we have incomplete subclass information for global
  // TNs, so when we free one, we may free a superclass of the true class,
  // giving us an overestimate on the true class.  This hack helps us bound
  // that overestimate.
  for (INT i = 0; i < info->n_subclasses; i++) {
    ISA_REGISTER_SUBCLASS sub = info->subclass[i];
    // Do not apply this refinement for subclasses of different widths.
    if (info->nregs != subclass_info[cl][sub].nregs) continue;
    if (current_usage[cl][sub] > (current_usage[cl][sc] + info->subclass_diff[i])) {
      current_usage[cl][sub] = current_usage[cl][sc] + info->subclass_diff[i];
    }
  }
}
#endif

// Initialize the LRA data structures at the start of each PU/region. 
void
LRA_Init (void)
{
  Reg_Request_Table = NULL;
  Reg_Table_Size = 0;
#ifdef KEY
  // Check the dependence graph to determine if a LR is reloadable.
  Need_Dep_Graph_For_Mark_Reloadable_Live_Ranges = TRUE;
#endif
#ifdef TARG_ST
  if (! subclass_info_initialized) {
    init_subclass_info ();
  }
  if (! subclass_order_initialized) {
    init_subclass_order ();
  }
#endif

}

#ifdef TARG_ST
static void
print_lra_request (const LRA_Request_Info *request)
{
  ISA_REGISTER_CLASS cl;
  ISA_REGISTER_SUBCLASS sc;

  for (INT i = 0; i < request->n_reqs; i++) {
    fprintf (TFile, " [%s %s]:%d",
	     REGISTER_CLASS_name (request->reqs[i].cl),
	     REGISTER_SUBCLASS_name (request->reqs[i].sc),
	     request->reqs[i].count);
    if (request->reqs[i].nregs > 1) {
      fprintf (TFile, "(nregs %d)", request->reqs[i].nregs);
    }
    if (request->reqs[i].demand) {
      fprintf (TFile, " demanded");
    }
  }
  
  fprintf (TFile, "  Summary:");
  FOR_ALL_ISA_REGISTER_CLASS (cl) {
    if (request->summary[cl] > 0) {
      fprintf (TFile, " %s:%d", REGISTER_CLASS_name(cl), request->summary[cl]);
    }
  }
}

static
void
LRA_examine_last_op_needs (BB *bb, INT needs[ISA_REGISTER_CLASS_MAX_LIMIT+1][ISA_REGISTER_SUBCLASS_MAX_LIMIT+1])
{
  // Accumulate in needs the register requirements for the
  // terminating branch op of bb.
  // needs is a 2D array indexed by class and subclass.
  memset (needs, 0, sizeof(INT) * (ISA_REGISTER_SUBCLASS_MAX_LIMIT + 1)
	  * (ISA_REGISTER_CLASS_MAX_LIMIT + 1));
  OP *last_op = BB_last_op (bb);
  INT ptr_rc_needs = 0;

  if (PROC_has_branch_delay_slot() &&
      (last_op != NULL) &&
      (OP_prev(last_op) != NULL)) last_op = OP_prev(last_op);

  if (last_op != NULL && OP_xfer(last_op)) {
    INT i;
    TN *tn;

    for (i = 0; i < OP_opnds(last_op); i++) {

      tn = OP_opnd(last_op, i);
      if ((tn != NULL) &&
          !TN_is_constant(tn) &&
          !TN_is_dedicated(tn) &&
          !(LRA_TN_register(tn) != REGISTER_UNDEFINED)) {
	ISA_REGISTER_CLASS cl = TN_register_class (tn);
	INT nregs = TN_nhardregs (tn);
	LIVE_RANGE *clr = LR_For_TN (tn);
	ISA_REGISTER_SUBCLASS sc = LR_subclass (clr);
	// We have a need for TN_nhardregs of class cl, subclass sc.
	needs[cl][sc] += nregs;
	if (REGISTER_CLASS_is_ptr(cl)) ptr_rc_needs += nregs;
      }
    }
  }

  if (ptr_rc_needs == 0) {
    /* For targets that have restricted offsets from SP, 
     * we need to have at least 1 so that we can insert a spill, if needed. */
    needs[Register_Class_For_Mtype(MTYPE_A4)][ISA_REGISTER_SUBCLASS_UNDEFINED] = 1;
  }
}

static void
build_lra_request (BB *bb, LRA_Request_Info *result)
{
  // Result is a set of ( class, subclass, nregs, count ) requests,
  // where we are requesting COUNT local live ranges, of register class
  // CLASS, occupying NREGS consecutive registers, which must begin
  // at a register in SUBCLASS.
  ISA_REGISTER_CLASS cl;
  ISA_REGISTER_SUBCLASS sc;

  result->n_reqs = 0;
  // First create the last_op_needs as a demand.
  INT last_op_needs[ISA_REGISTER_CLASS_MAX_LIMIT+1][ISA_REGISTER_SUBCLASS_MAX_LIMIT+1];
  LRA_examine_last_op_needs (bb, last_op_needs);
  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    ISA_REGISTER_SUBCLASS sc;
    result->summary[cl] = 0;
    FOR_ALL_ISA_REGISTER_SUBCLASS_AND_UNDEFINED (sc) {
      const Reg_SubClass_Info *info = &subclass_info[cl][sc];
      INT usage = last_op_needs[cl][sc];
      if (usage == 0) continue;
      INT sc_count = usage / info->nregs;
      if (sc_count > 0) {
	result->reqs[result->n_reqs].demand = TRUE;
	result->reqs[result->n_reqs].cl = cl;
	result->reqs[result->n_reqs].sc = sc;
	result->reqs[result->n_reqs].nregs = subclass_info[cl][sc].nregs;
	result->reqs[result->n_reqs++].count = sc_count;
	// Subtract this demand from the request for this {cl,sc}
	// and all super-classes.
	for (INT j = 0; j < info->n_superclasses; j++) {
	  ISA_REGISTER_SUBCLASS super = info->superclass[j];
	  max_usage[cl][super] -= usage;
	}
	result->summary[cl] = (((result->summary[cl] + usage) <= INT8_MAX)
			       ? (result->summary[cl] + usage)
			       : INT8_MAX);
      }
    }
  }
	
  // Second, create the max_usage needs as a request.
  FOR_ALL_ISA_REGISTER_CLASS(cl) {

    // A requirement for a register of a subclass will also satisfy
    // a requirement for a register of any its superclasses.
    // So we want to visit subclasses in an order where we visit
    // a subclass before its superclasses.
    // The required order is precalculated in the array subclass_order.
    for (INT i = 0; i < subclass_order[cl].n_subclasses; i++) {
      ISA_REGISTER_SUBCLASS sc = subclass_order[cl].ordered_subclasses[i];
      const Reg_SubClass_Info *info = &subclass_info[cl][sc];
      INT usage = max_usage[cl][sc];
      INT sc_count = usage / info->nregs;
      if (sc_count > 0) {
	result->reqs[result->n_reqs].demand = FALSE;
	result->reqs[result->n_reqs].cl = cl;
	result->reqs[result->n_reqs].sc = sc;
	result->reqs[result->n_reqs].nregs = subclass_info[cl][sc].nregs;
	result->reqs[result->n_reqs++].count = sc_count;
	// Subtract this usage from all super-classes.
	for (INT j = 0; j < info->n_superclasses; j++) {
	  ISA_REGISTER_SUBCLASS super = info->superclass[j];
	  max_usage[cl][super] -= usage;
	}
	result->summary[cl] = (((result->summary[cl] + usage) <= INT8_MAX)
			       ? (result->summary[cl] + usage)
			       : INT8_MAX);
      }
    }
  }

  if (Get_Trace (TP_ALLOC, 0x20, bb)) {
    fprintf (TFile, "Local reg request (BB:%d)\t", BB_id(bb));
    print_lra_request (result);
    fprintf (TFile, "\n");
  }
}

/* estimate the maximum number of local registers required in each register
 * class for this basic block. If (regs_in_use != NULL) report the total
 * number of registers in use before this instruction is executed.
 */
void
LRA_Estimate_Fat_Points (
  BB *bb, 
  LRA_Request_Info *request,
  INT *regs_in_use,
  MEM_POOL* pool)
{
  OP *op;
  INT i, opnum, opndnum;
  INT tot_use = 0;

  Setup_Live_Ranges (bb, FALSE, pool);

  if (Do_LRA_Trace(Trace_LRA_Detail)) {
    Print_Live_Ranges (bb);
  }

  init_live ();

  for (opnum = BB_length(bb); opnum > 0; opnum--) 
  {
    op = OP_VECTOR_element(Insts_Vector, opnum);

    if (regs_in_use) regs_in_use[opnum] = tot_use;  /* In case there are no new uses and no defs. */

    for (i = 0; i < OP_results(op); i++) {
      TN *result_tn = OP_result(op, i);
      INT nregs = TN_nhardregs (result_tn);
      ISA_REGISTER_CLASS regclass = TN_register_class(result_tn);
      LIVE_RANGE *clr = LR_For_TN (result_tn);
      ISA_REGISTER_SUBCLASS subclass = LR_subclass (clr);
      if (opnum == LR_first_def(clr)) {
        if ((LR_assigned(clr) ||
	     (!regs_in_use && !TN_is_local_reg(result_tn)))) 
        {
	  remove_live (regclass, subclass, nregs);
	  if (regs_in_use &&
              (LR_last_use(clr) != BB_length(bb)+1) &&
	      !LR_exposed_use(clr) &&
              (LR_last_use(clr) != LR_first_def(clr))){
	    tot_use -= nregs;
	    regs_in_use[opnum] = tot_use;
	  }
        }
      }

      /* reserve at least nregs registers for this regclass. */
      ensure_live (regclass, ISA_REGISTER_SUBCLASS_UNDEFINED, nregs);
    }

    /* process all the operand TNs. */
    for (opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
      TN *tn = OP_opnd(op, opndnum);
      if (TN_is_register(tn)) {
        ISA_REGISTER_CLASS regclass = TN_register_class(tn);
	INT nregs = TN_nhardregs (tn);
        LIVE_RANGE *clr = LR_For_TN (tn);
	ISA_REGISTER_SUBCLASS subclass = LR_subclass (clr);
        if (!LR_assigned(clr)) {
          if (TN_is_local_reg(tn) || (!regs_in_use &&
              ((LR_def_cnt(clr) != 0 && opnum <= LR_exposed_use(clr)) ||
	      (OP_copy(op) && !TN_is_local_reg(OP_result(op,OP_Copy_Result(op)))))))
          {
            Set_LR_assigned(clr);
	    add_live (regclass, subclass, nregs);
	    if (regs_in_use &&
                (LR_last_use(clr) != BB_length(bb)+1) &&
	        !LR_exposed_use(clr) &&
                (LR_last_use(clr) != LR_first_def(clr))){
	        tot_use += nregs;
		regs_in_use[opnum] = tot_use;
	    }
          }
        }
        /* reserve at least nregs registers for this regclass. */
	ensure_live (regclass, ISA_REGISTER_SUBCLASS_UNDEFINED, nregs);
      }
    }
    if (Get_Trace (TP_ALLOC, 0x20, bb)) {
      ISA_REGISTER_CLASS cl;
      ISA_REGISTER_SUBCLASS sc;

      fprintf(TFile, "EFP:%2d> ", opnum);
      Print_OP_No_SrcLine (op);
      fprintf(TFile, "EFP:Usage ");
      FOR_ALL_ISA_REGISTER_CLASS (cl) {
	FOR_ALL_ISA_REGISTER_SUBCLASS_AND_UNDEFINED (sc) {
	  if (current_usage[cl][sc] != 0) {
	    fprintf (TFile, " %s ", REGISTER_CLASS_name (cl));
	    if (sc != ISA_REGISTER_SUBCLASS_UNDEFINED) {
	      fprintf (TFile, "%s ", REGISTER_SUBCLASS_name (sc));
	    }
	    fprintf (TFile, "%d", current_usage[cl][sc]);
	  }
	}
      }
      fprintf(TFile, "\n");
    }
  }
  if (Get_Trace (TP_ALLOC, 0x20, bb)) {
    ISA_REGISTER_CLASS cl;
    ISA_REGISTER_SUBCLASS sc;
    fprintf (TFile, "Local reg usage (BB:%d)\t", BB_id(bb));
    FOR_ALL_ISA_REGISTER_CLASS (cl) {
      FOR_ALL_ISA_REGISTER_SUBCLASS_AND_UNDEFINED (sc) {
	if (max_usage[cl][sc] > 0) {
	  fprintf (TFile, "[%s %s]:%d  ",
		   REGISTER_CLASS_name (cl),
		   REGISTER_SUBCLASS_name (sc),
		   max_usage[cl][sc]);
	}
      }
    }
    fprintf (TFile, "\n");
  }
  if (request) {
    build_lra_request (bb, request);
  }
}
#else

/* estimate the maximum number of local registers required in each register
 * class for this basic block. If (regs_in_use != NULL) report the total
 * number of registers in use before this instruction is executed.
 */
void
LRA_Estimate_Fat_Points (
  BB *bb, 
  mINT8 *fatpoint,
  INT *regs_in_use,
  MEM_POOL* pool)
{
  INT inuse[ISA_REGISTER_CLASS_MAX+1];
  OP *op;
  INT i, opnum, opndnum;
  INT tot_use = 0;
#ifdef KEY
  INT noncontinuous_GTNs[ISA_REGISTER_CLASS_MAX+1];
#endif

  Setup_Live_Ranges (bb, FALSE, pool);
  if (Do_LRA_Trace(Trace_LRA_Detail)) {
    Print_Live_Ranges (bb);
  }

  for (i = ISA_REGISTER_CLASS_MIN; i <= ISA_REGISTER_CLASS_MAX; i++) {
    fatpoint[i] = 0;
    inuse[i] = 0;
#ifdef KEY
    noncontinuous_GTNs[i] = 0;
#endif
  }

  for (opnum = BB_length(bb); opnum > 0; opnum--) 
  {
    op = OP_VECTOR_element(Insts_Vector, opnum);
    if (regs_in_use) regs_in_use[opnum] = tot_use;  /* In case there are no new uses and no defs. */

    for (i = 0; i < OP_results(op); i++) {
      TN *result_tn = OP_result(op, i);
      ISA_REGISTER_CLASS regclass = TN_register_class(result_tn);
      LIVE_RANGE *clr = LR_For_TN (result_tn);
      if (opnum == LR_first_def(clr)) {
        if ((LR_assigned(clr) ||
	     (!regs_in_use && !TN_is_local_reg(result_tn)))) 
        {
          inuse[regclass]--;
	  if (regs_in_use &&
              (LR_last_use(clr) != BB_length(bb)+1) &&
	      !LR_exposed_use(clr) &&
              (LR_last_use(clr) != LR_first_def(clr))){
	    regs_in_use[opnum] = --tot_use;
	  }
        }
#ifdef KEY
	// Detect GTN that is live-in and live-out but not continuous in BB.
        if (!TN_is_local_reg(result_tn) &&
	    LR_exposed_use(clr) &&			// live-in
	    LR_last_use(clr) == BB_length(bb)+1 &&	// live-out
	    !OP_Refs_TN(op, result_tn)) {
	  noncontinuous_GTNs[regclass]++;
        }
#endif
      }

      /* reserve at least one register for this regclass. */
      if (fatpoint[regclass] == 0) fatpoint[regclass] = 1;
    }

    /* process all the operand TNs. */
    for (opndnum = 0; opndnum < OP_opnds(op); opndnum++) {
      TN *tn = OP_opnd(op, opndnum);
      if (TN_is_register(tn)) {
        ISA_REGISTER_CLASS regclass = TN_register_class(tn);
        LIVE_RANGE *clr = LR_For_TN (tn);
        if (!LR_assigned(clr)) {
          if (TN_is_local_reg(tn) || (!regs_in_use &&
              ((LR_def_cnt(clr) != 0 && opnum <= LR_exposed_use(clr)) ||
	      (OP_copy(op) && !TN_is_local_reg(OP_result(op,0))))))
          {
            Set_LR_assigned(clr);
	    inuse[regclass]++;
	    if (regs_in_use &&
                (LR_last_use(clr) != BB_length(bb)+1) &&
	        !LR_exposed_use(clr) &&
                (LR_last_use(clr) != LR_first_def(clr))){
		regs_in_use[opnum] = ++tot_use;
	    }
            if (fatpoint[regclass] < inuse[regclass]) {
              if (inuse[regclass] <= 127) {
                fatpoint[regclass] = inuse[regclass];
              }
            }
          }
        }
        /* reserve at least one register for this regclass. */
        if (fatpoint[regclass] == 0) fatpoint[regclass] = 1;
      }
    }
  }

#ifdef KEY
  // Increase fat point estimate by one for each GTN that is live-in and
  // live-out but not continuous in the BB.  The fatpoint calculation assumes
  // each live-range can accept any register assignment, but this is not true
  // for these live-in and live-out segments which must get the same register.
  // To ensure a successful coloring, give LRA one more register.
  if (LRA_inflate_gtn_request &&
      (BB_innermost(bb) ||
       BB_unrollings(bb))) {	// BB is an unrolled body
    for (i = ISA_REGISTER_CLASS_MIN; i <= ISA_REGISTER_CLASS_MAX; i++) {
      int count = noncontinuous_GTNs[i];
      if (count > 4)	// Limit additional request.  Adjust threshold as
	count = 4;	// necessary.
      fatpoint[i] += count;
    }
  }
#endif

  if (Get_Trace (TP_ALLOC, 0x20, bb)) {
    fprintf (TFile, "Fat Points (BB:%d)\t", BB_id(bb));
    for (i = ISA_REGISTER_CLASS_MIN; i <= ISA_REGISTER_CLASS_MAX; i++) {
      fprintf (TFile, "[%d]:%d  ", i, fatpoint[i]);
    }
    fprintf (TFile, "\n");
  }
}


static
mINT8
LRA_examine_last_op_needs (BB *bb, ISA_REGISTER_CLASS cl)
{
  OP *last_op = BB_last_op (bb);
  mINT8 min_regs = 0;

  if (PROC_has_branch_delay_slot() &&
      (last_op != NULL) &&
      (OP_prev(last_op) != NULL)) last_op = OP_prev(last_op);

  if (last_op != NULL && OP_xfer(last_op)) {
    INT i;
    TN *tn;

    for (i = 0; i < OP_opnds(last_op); i++) {
      tn = OP_opnd(last_op, i);
      if ((tn != NULL) &&
          !TN_is_constant(tn) &&
          !TN_is_dedicated(tn) &&
          !(LRA_TN_register(tn) != REGISTER_UNDEFINED) &&
           (TN_register_class(tn) == cl)) min_regs++;
    }

  }

  if ((min_regs == 0) && (cl == ISA_REGISTER_CLASS_integer)) {
   /* We need to have at least 1 so that we can insert a spill, if needed. */
    min_regs = 1;
  }

  return min_regs;
}
#endif
#ifdef TARG_ST
LRA_Request_Info *
LRA_Compute_Register_Request (BB *bb, MEM_POOL *pool)
#else
mINT8 *
LRA_Compute_Register_Request (BB *bb, MEM_POOL *pool)
#endif
{
  // Allocate the Reg_Request_Table if not yet allocated.
  if (Reg_Request_Table == NULL) {
    /* this needs to be initialized to 0, so use a zeroing pool */
#ifdef TARG_ST
    Reg_Request_Table = 
          TYPE_PU_ALLOC_N (LRA_Request_Info, (PU_BB_Count+2));
#else
    Reg_Request_Table = 
          TYPE_PU_ALLOC_N (mINT8, (PU_BB_Count+2)*ISA_REGISTER_CLASS_COUNT);
#endif
    Reg_Table_Size = PU_BB_Count;
  }
#ifdef KEY
  // Scratch BBs can be created to run the BB scheduler multiple times.
  if (BB_id(bb) > Reg_Table_Size) {
    // The above code adds 2 to PU_BB_Count.  Don't know why; so do same here.
    int new_size = (PU_BB_Count * 2 + 2) * ISA_REGISTER_CLASS_COUNT;
    int old_size = (Reg_Table_Size + 2) * ISA_REGISTER_CLASS_COUNT;
    Reg_Request_Table = TYPE_MEM_POOL_REALLOC_N(mINT8,
						MEM_pu_pool_ptr,
						Reg_Request_Table,
						old_size, new_size);
    Reg_Table_Size = PU_BB_Count * 2;
  }
#else
  Is_True (BB_id(bb) <= Reg_Table_Size, ("incorrect BB_id: %d", BB_id(bb)));
#endif
#ifdef TARG_ST
  LRA_Request_Info *fatpoint = &Reg_Request(bb);
#else
  mINT8 *fatpoint = &Reg_Request(bb,0);
#endif
  ISA_REGISTER_CLASS int_regclass = TN_register_class(SP_TN);

  LRA_Estimate_Fat_Points (bb, fatpoint, NULL, pool);

#ifdef TARG_ST
  // The requirements for the trailing conditional branch are now
  // handled by LRA_Estimate_Fat_Points, not here.
#else
  /* Reserve at least 2 integer registers for LRA.  This is the minimum
   * required to allocate the operands of a conditional branch. Any spill
   * code that LRA introduces is above a terminating branch.
   * TODO: check if the basic block has a terminating branch with two
   * operands. For other cases, we only need to reserve 1 integer register.
   * That register might be required as a temp while generating spill code.
   */
  if (fatpoint[int_regclass] < 2) fatpoint[int_regclass] = LRA_examine_last_op_needs (bb, int_regclass);
#endif
  return fatpoint;
}

#ifdef TARG_ST
/* ======================================================================
 * LRA_Register_Request
 *
 * Returns the number of registers LRA is requesting from GRA
 * in the basic block <bb>. If we run the scheduling
 * pass before register allocation for the bb, this returns an 
 * accurate estimate of how many registers LRA needs. Otherwise,
 * it is just a fixed number based on some heuristics.
 * ======================================================================*/
void
LRA_Register_Request (BB *bb,
		      LRA_Request_Info *request)
{
  FmtAssert (!BB_reg_alloc(bb), ("LRA_Register_Request: Invalid bb %d", BB_id(bb)));

  if (BB_id(bb) <= Reg_Table_Size) {
    /* Note: Re-work the estimation of minimum registers needed.
     *
     * Since register re-loads can not be inserted after a branch statement,
     * the minimal number of registers that we need is equivalent to the
     * number needed in the branch instruction.  Examine that instruction
     * and determine the number needed in each register class.
     */
    *request = Reg_Request (bb);
  } else {
    /* If not specified assume no request. */
    ISA_REGISTER_CLASS cl;
    request->n_reqs = 0;
    FOR_ALL_ISA_REGISTER_CLASS (cl) {
      request->reqs[request->n_reqs].demand = FALSE;
      request->reqs[request->n_reqs].cl = cl;
      request->reqs[request->n_reqs].sc = ISA_REGISTER_SUBCLASS_UNDEFINED;
      request->reqs[request->n_reqs].nregs = 1;
      request->reqs[request->n_reqs++].count = 0;
      request->summary[cl] = 0;
    }
  }
}  
#else
/* ======================================================================
 * LRA_Register_Request
 *
 * Returns the number of registers LRA is requesting from GRA for
 * the class <cl> in the basic block <bb>. If we run the scheduling
 * pass before register allocation for the bb, this returns an 
 * accurate estimate of how many registers LRA needs. Otherwise,
 * it is just a fixed number based on some heuristics.
 * ======================================================================*/
INT
LRA_Register_Request (BB *bb,  ISA_REGISTER_CLASS cl)
{
  INT regs_needed;

#ifdef KEY
  if (BB_reg_alloc(bb))
    return 0;
#else
  FmtAssert (!BB_reg_alloc(bb), ("LRA_Register_Request: Invalid bb %d", BB_id(bb)));
#endif

  if (BB_id(bb) <= Reg_Table_Size) {
    /* Note: Re-work the estimation of minimum registers needed.
     *
     * Since register re-loads can not be inserted after a branch statement,
     * the minimal number of registers that we need is equivalent to the
     * number needed in the branch instruction.  Examine that instruction
     * and determine the number needed in each register class.
     */

    mINT8 min_regs = LRA_examine_last_op_needs (bb, cl);
    regs_needed = Reg_Request (bb, cl);
    if (regs_needed < min_regs) {
      regs_needed = min_regs;
      Reg_Request (bb, cl) = min_regs;
    }
#ifdef KEY
    // Inflate register request to reduce constraints on instruction
    // scheduling.
    if ((BB_innermost(bb) ||
	 BB_unrollings(bb)) &&	// BB is an unrolled body
	BB_length(bb) > 20) {
      // LRA_inflate_reg_request is a percentage.
      regs_needed += (int)(regs_needed * LRA_inflate_reg_request * 0.01 + 0.5);
    }
#endif
  }
  else {
    /* TODO: tune this based on register class and length of basic block */ 
    regs_needed = 5;
  }
  return regs_needed;
}
#endif
#ifdef KEY
// Copy the register requests.
void
LRA_Copy_Register_Request (BB *to_bb, BB *from_bb)
{
  ISA_REGISTER_CLASS cl;
  int regs_needed;

  FOR_ALL_ISA_REGISTER_CLASS(cl) {
    regs_needed = Reg_Request(from_bb, cl);
    Reg_Request(to_bb, cl) = regs_needed;
  }
}
#endif
