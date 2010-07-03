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


/* =======================================================================
 * =======================================================================
 *
 *  Module: cg_color.cxx
 *
 *  Description:
 *  ============
 *
 *  See client side interface description in cg_color.h
 *
 *
 *  Implementation:
 *  ===============
 *
 *  The implementation for choosing the best register (actually the order
 *  of allocation when there are more than 1 query) accounts for:
 *  - preferred registers (they can be specified in the target dependent part)
 *	that will be tried first,
 *  - preserved registers (callee saves and link register in some cases) that
 *	have a save/restore cost only for the first time use.
 *
 * =======================================================================
 */

#include "defs.h"
#include "tn.h"			/* For RA_TN.  */
#include "cg.h"			/* For PU_Has_Calls().  */
#include "cg_color.h"
#include <stdio.h>
#include "cg_flags.h"

/*
 * These regs_used list track the registers used for the function (approximately)
 * as this has an impact on the coloring choice.
 * In particular a must_preserve[] (see below) register that has already been used should
 * not have hidden cost and can be allocated with the same priority as a 
 * non must-preserve register.
 * These sets are reset at the beginning of the function by CGCOLOR_Initialize_For_PU(),
 * then the client must declare the registers it allocates with the 
 * CGCOLOR_Allocate_N_Registers() function.
 */
static REGISTER_SET regs_used[ISA_REGISTER_CLASS_MAX_LIMIT + 1];

/*
 * This is the list of registers that must be preserved on entry and exit
 * of the function if they are used explicitly at least once in the function.
 * This set contains in particular callee saved registers and the link
 * register for non leaf functions.
 * This set is used for the coloring as allocating a variable to a
 * register in this set means that there is an additional cost to
 * preserve/restore it.
 * Though, this additional cost is paid only once, hence the regs_used[]
 * set is used to check this.
 */
static REGISTER_SET must_preserve[ISA_REGISTER_CLASS_MAX_LIMIT + 1];

/*
 * Registers that must be when possible colored before the others.
 * This set is initialized by the CGTARG_Preferred_GRA_Registers() function.
 * Each time there is the possibility to do so, a register in this set will be
 * preferred v.s. another register not in this set.
 */
static REGISTER_SET preferred_regs[ISA_REGISTER_CLASS_MAX_LIMIT + 1];


/*
 * struct Choose_Transaction
 * 
 * Record the current CGCOLOR transaction when Choose_Best_Register() is used.
 * Then subsequent calls to Choose_Next_Best_register will use this().
 * The current_used field is modified while we iterate on the Choose/Choose_Next
 * function. All other fields are initialized the first time and never modified.
 */
typedef struct {
  ISA_REGISTER_CLASS rc;
  INT nregs;
  REGISTER_SET allowed;
  REGISTER_SET subclass_allowed;
  REGISTER reg_hint;
  REGISTER_SET current_used;    
} Choose_Transaction_T;


/*
 * Current transaction.
 */
Choose_Transaction_T Current_Transaction;


/* ======================================================================
 * Choose_N_Registers()
 * 
 * Pick a register satisfying the constraints.
 * The search is circular in order of growing register identifier.
 * If reg_hint is specified the search will start from reg_hint.
 * ======================================================================
 */
static REGISTER
Choose_N_Registers (INT nregs,
		    REGISTER_SET subclass_allowed,
		    REGISTER_SET allowed,
		    REGISTER reg_hint)
{
  REGISTER r;
  // start at first register if reg_hint is unspecififed
  reg_hint = reg_hint == REGISTER_UNDEFINED ? REGISTER_MIN : reg_hint;

  for (r = reg_hint; r < REGISTER_MAX+1; r++) {
    if (REGISTER_SET_MemberP(subclass_allowed, r) &&
	REGISTER_SET_MembersP (allowed, r, nregs))
      return r;
  }

  for (r = REGISTER_MIN; r < reg_hint; r++) {
    if (REGISTER_SET_MemberP(subclass_allowed, r) &&
	REGISTER_SET_MembersP (allowed, r, nregs))
      return r;
  }
  return REGISTER_UNDEFINED;
}

/* ======================================================================
 * Choose_N_Registers_Intersection()
 * 
 * Pick a register satisfying the constraints <subclass_allowed>,
 * <allowed> constraints and included in the <s> set.
 * ======================================================================
 */
static REGISTER
Choose_N_Registers_Intersection (INT nregs,
				 REGISTER_SET subclass_allowed,
				 REGISTER_SET allowed,
				 REGISTER_SET s,
				 REGISTER reg_hint)
{
  return Choose_N_Registers (nregs,
			     REGISTER_SET_Intersection (subclass_allowed, s),
			     REGISTER_SET_Intersection (allowed, s), reg_hint);
}


/* ======================================================================
 * Choose_Best_N_Registers()
 * 
 * Actual implementation of the choose best algorithm.
 * ======================================================================
 */
static REGISTER
Choose_Best_N_Registers(ISA_REGISTER_CLASS rc, INT nregs,
			REGISTER_SET subclass_allowed,
			REGISTER_SET allowed,
			REGISTER reg_hint)
{
  REGISTER reg = REGISTER_UNDEFINED;

  if ( REGISTER_SET_EmptyP(allowed) ||
       REGISTER_SET_EmptyP(subclass_allowed))
    return REGISTER_UNDEFINED;

  REGISTER_SET used = regs_used[rc];
  REGISTER_SET costly_regs = REGISTER_SET_Difference(must_preserve[rc], used);
  REGISTER_SET non_costly_regs = REGISTER_SET_Difference(REGISTER_CLASS_allocatable(rc), 
							 costly_regs);
  REGISTER_SET never_costly = REGISTER_SET_Difference(REGISTER_CLASS_allocatable(rc), 
						      must_preserve[rc]);

  // Registers are chosen based on the following priorities:
  //
  // 1. Prefer never costly first 
  //    (registers for which there is never additional hidden cost),
  // 2. then all non costly
  //    (registers for which there may be an additional cost for the
  //     first time use only, 
  //     but which have already been used, hence no additional cost)
  // 3. then all.
  //    (all registers available, including those with an additional
  //     hidden cost for the first time use)
  //
  // These three steps will tend to favor reuse of never costly regs
  // which may in turn avoid using a costly reg. In addition we favor
  // non costly registers for which the cost is paid the first time use
  // only.
  //
  // This initial priority ordering is refined by the preferred_regs[]
  // and its associated priority setting:
  // - PREF_REGS_PRIORITY_LOW :
  //   ----> At each step of initial ordering, try preferred_regs[] first.
  //
  // - PREF_REGS_PRIORITY_MEDIUM :
  //   ----> Same as low priority, except that {preferred, non costly}
  //         will be tried before {!preferred, never_costly}
  //
  // - PREF_REGS_PRIORITY_HIGH :
  //   ----> Try to have a preferred registers, even if it is a costly one.
  //         If no preferred reg available, then try a non preferred one.
  //

  // -------------------------------------------------------------
  // Define a macro for a more readable code
  // -------------------------------------------------------------
#define TRY_SET(the_set)                                        \
  reg = Choose_N_Registers_Intersection ( nregs,                \
                                          subclass_allowed,     \
                                          allowed,              \
                                          the_set,              \
                                          reg_hint );           \
  if (reg != REGISTER_UNDEFINED) {                              \
    return reg;                                                 \
  }
  // -------------------------------------------------------------

  TRY_SET ( REGISTER_SET_Intersection(never_costly, preferred_regs[rc]) );

  if (CG_COLOR_pref_regs_priority == PREF_REGS_PRIORITY_LOW) {
    TRY_SET ( never_costly );
    TRY_SET ( REGISTER_SET_Intersection(non_costly_regs, preferred_regs[rc]) );
    TRY_SET ( non_costly_regs );
    TRY_SET ( preferred_regs[rc] );
  }
  else if (CG_COLOR_pref_regs_priority == PREF_REGS_PRIORITY_MEDIUM) {
    TRY_SET ( REGISTER_SET_Intersection(non_costly_regs, preferred_regs[rc]) );
    TRY_SET ( never_costly );
    TRY_SET ( non_costly_regs );
    TRY_SET ( preferred_regs[rc] );
  }
  else { // CG_COLOR_pref_regs_priority == PREF_REGS_PRIORITY_HIGH
    TRY_SET ( REGISTER_SET_Intersection(non_costly_regs, preferred_regs[rc]) );
    TRY_SET ( preferred_regs[rc] );
    TRY_SET ( never_costly );
    TRY_SET ( non_costly_regs );
  }

  TRY_SET ( allowed );
  
  return REGISTER_UNDEFINED;
}

/* ======================================================================
 * Choose_Transaction_Init()
 *
 * Initialize a new transaction of Choose_Best_Register().
 * ======================================================================
 */
static void
Choose_Transaction_Init(Choose_Transaction_T *t, ISA_REGISTER_CLASS rc, INT nregs,
			REGISTER_SET subclass_allowed,
			REGISTER_SET allowed,
			REGISTER reg_hint)
{
  t->rc = rc;
  t->nregs = nregs;
  t->subclass_allowed = subclass_allowed;
  t->allowed = allowed;
  t->reg_hint = reg_hint;
  t->current_used = REGISTER_SET_EMPTY_SET;
}

/* ======================================================================
 * Choose_Transaction_Update()
 *
 * Update the transaction for the next iteration of 
 * CGCOLOR_Choose_Next_Best_register().
 * Actually put the register into the used set of the transaction.
 * ======================================================================
 */
static void
Choose_Transaction_Update(Choose_Transaction_T *t, REGISTER reg)
{
  t->current_used = REGISTER_SET_Union1(t->current_used, reg);
}

/* ======================================================================
 * Choose_Transaction_Best_Register()
 *
 * Get the best register given the current transaction state.
 * ======================================================================
 */
static REGISTER
Choose_Transaction_Best_Register(Choose_Transaction_T *t)
{
  REGISTER_SET subclass_allowed = REGISTER_SET_Difference(t->subclass_allowed, t->current_used);
  return Choose_Best_N_Registers(t->rc, t->nregs, subclass_allowed, t->allowed, t->reg_hint);
}

/* ======================================================================
 * CGCOLOR_Choose_Best_Register()
 * 
 * See cg_color.h for interface description.
 *
 * Implementation uses a new transaction and return the first best 
 * register. Then it updates the transaction for the next query.
 * ======================================================================
 */
REGISTER
CGCOLOR_Choose_Best_Register(ISA_REGISTER_CLASS rc, INT nregs,
			     REGISTER_SET subclass_allowed,
			     REGISTER_SET allowed,
			     REGISTER reg_hint)
{
  REGISTER reg;

  Choose_Transaction_Init(&Current_Transaction, rc, nregs, subclass_allowed, allowed, reg_hint);

  reg = Choose_Transaction_Best_Register(&Current_Transaction);
    
  if (reg != REGISTER_UNDEFINED) {
    Choose_Transaction_Update(&Current_Transaction, reg);
  }
  return reg;
}

/* ======================================================================
 * CGCOLOR_Choose_Next_Best_Register()
 * 
 * See cg_color.h for interface description.
 *
 * Implementation uses the next best register as given by the transaction
 * state. Then it updates the transaction for the next query.
 * ======================================================================
 */
REGISTER
CGCOLOR_Choose_Next_Best_Register(void)
{
  REGISTER reg;
  
  reg = Choose_Transaction_Best_Register(&Current_Transaction);

  if (reg != REGISTER_UNDEFINED) {
    Choose_Transaction_Update(&Current_Transaction, reg);
  }
  return reg;
}

/* ======================================================================
 * CGCOLOR_Allocate_N_Registers()
 * 
 * See interface description.
 * ======================================================================
 */
void
CGCOLOR_Allocate_N_Registers(ISA_REGISTER_CLASS rc, REGISTER reg, INT nregs)
{
  regs_used[rc] = REGISTER_SET_Union (regs_used[rc], REGISTER_SET_Range(reg, reg + nregs - 1));
}


/* ======================================================================
 * CGCOLOR_Can_Allocate_N_Registers()
 * 
 * See interface description.
 * ======================================================================
 */
BOOL
CGCOLOR_Can_Allocate_N_Registers(INT nregs,
				 REGISTER_SET subclass_allowed,
				 REGISTER_SET allowed)
{
  return Choose_N_Registers (nregs, subclass_allowed, allowed, REGISTER_UNDEFINED)
    != REGISTER_UNDEFINED;
}


/* =======================================================================
 *
 *  CGCOLOR_Initialize_For_PU
 *
 *  See interface description.
 *
 * =======================================================================
 */
void
CGCOLOR_Initialize_For_PU(void)
{
  ISA_REGISTER_CLASS rc;

  FOR_ALL_ISA_REGISTER_CLASS( rc ) {
    // Initialize set of preserved registers.
    // This set will not change from now for the PU.
    must_preserve[rc] = REGISTER_CLASS_callee_saves(rc);
    if (RA_TN != NULL && rc == TN_register_class(RA_TN) &&
	!PU_Has_Calls) {
      // If the function is leaf, the first use of RA_TN has some
      // cost, because it must be saved/restored if used.
      must_preserve[rc] = REGISTER_SET_Union1(must_preserve[rc], TN_register(RA_TN));
    }
    
    // Initialize set of registers used in current PU.
    regs_used[rc] = REGISTER_SET_EMPTY_SET;

    // Handle target dependent register preferences.
    if (CG_COLOR_use_pref_regs) {
      preferred_regs[rc] = CGTARG_Preferred_GRA_Registers(rc);
    } else {
      preferred_regs[rc] = REGISTER_SET_EMPTY_SET;
    }

  }
}


/* =======================================================================
 *
 *  CGCOLOR_Finalize_For_PU
 *
 *  See interface description.
 *
 * =======================================================================
 */
void
CGCOLOR_Finalize_For_PU(void)
{

}


