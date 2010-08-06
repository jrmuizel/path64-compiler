/*
 *  Copyright (C) 2009 PathScale, LLC.  All Rights Reserved.
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
 *  Module: lra.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Description:
 *  ============
 *
 *  Local Register Allocation.
 *
 * =======================================================================
 * =======================================================================
 */

#ifndef lra_INCLUDED
#define lra_INCLUDED

#include "bb.h"
#include "op.h"
#include "register.h"

/* Initialize LRA data structures for PU/region. */
extern void LRA_Init (void);

/* Do Local register allocation for a region. The <lra_for_pu> parameter
 * indicates if the region is the complete procedure.
 */
extern void LRA_Allocate_Registers (BOOL lra_for_pu);


/* Compute the number of local registers that will be required for each 
 * register class for this <bb>. This information is stored internally
 * in LRA and can be accessed later by a call to LRA_Register_Request.
 */
#ifdef TARG_ST
struct LRA_Request_Element {
  ISA_REGISTER_CLASS cl;
  ISA_REGISTER_SUBCLASS sc;
  INT count;
  mINT8 nregs;
  mBOOL demand;  // TRUE if this request is necessary for correct
  // allocation, i.e. this is a demand rather than a
  // request.
};
struct LRA_Request_Info {
  LRA_Request_Element reqs[2 * (ISA_REGISTER_CLASS_MAX_LIMIT + ISA_REGISTER_SUBCLASS_MAX_LIMIT)];
  INT n_reqs;
  mINT8 summary[ISA_REGISTER_CLASS_MAX_LIMIT+1];
};

extern LRA_Request_Info *LRA_Compute_Register_Request (BB *bb, MEM_POOL *pool);
#else
extern mINT8 *LRA_Compute_Register_Request (BB *bb, MEM_POOL *pool);
#endif

/* Compute the fatpoint for a block and optionally, return an array
 * of the combined register requirements at each op in the block.
 */
#ifdef TARG_ST
extern void LRA_Estimate_Fat_Points (BB *bb, LRA_Request_Info *request,
                                     INT *regs_in_use, MEM_POOL* pool);
#else
extern void LRA_Estimate_Fat_Points (BB* bb, mINT8* fatpoint,
				     INT* regs_in_use, MEM_POOL* pool);
#endif

#ifdef TARG_ST
/* Returns the number of registers LRA is requesting from GRA for
 * in the basic block <bb>. If we run the scheduling
 * pass before register allocation for the bb, this returns an 
 * accurate estimate of how many registers LRA needs. Otherwise,
 * it is just a fixed number based on some heuristics.
 */
extern void LRA_Register_Request (BB *bb, LRA_Request_Info *request);
#else
/* Returns the number of registers LRA is requesting from GRA for
 * the class <cl> in the basic block <bb>. If we run the scheduling
 * pass before register allocation for the bb, this returns an 
 * accurate estimate of how many registers LRA needs. Otherwise,
 * it is just a fixed number based on some heuristics.
 */
extern INT LRA_Register_Request (BB *bb,  ISA_REGISTER_CLASS cl);
#endif

/* Allocate registers for any unallocated TNs in <ops>.
 * <bb> is used to determine what temps are available.
 */
extern void Assign_Temp_Regs (OPS *ops, BB *bb);

#ifdef KEY
/* Copy the register requests from one BB to another, in support of local
 * scheduling using scratch BBs.
 */
extern void LRA_Copy_Register_Request (BB *to_bb, BB *from_bb);
#endif

#ifdef TARG_X8664
/* If subclass consists of only one register, then return that register, else
 * return REGISTER_UNDEFINED.
 */
extern REGISTER Single_Register_Subclass (ISA_REGISTER_SUBCLASS subclass);
#endif

#endif /* lra_INCLUDED */
