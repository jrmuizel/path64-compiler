/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Copyright (C) STMicroelectronics All Rights Reserved.

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

*/


/* =======================================================================
 * =======================================================================
 *
 *  Module: cg_color.h
 *
 *  Description:
 *  ============
 *
 *  Reserved prefix: 
 *
 *	CGCOLOR
 *
 *  Purpose:
 *
 *  	This module provides utility routines for use by cg phases that
 *      perform register assignment (or register allocation).
 *	In particular it provides an answer to "what is the register
 *	to choose next?" when performing register assignment.
 *	Clients of this are LRA and GRA currently.
 *
 *  Initialization:
 *
 *	This module must be initialized/finalized for each PU.
 *
 *	CGCOLOR_Initialize_For_PU(void)
 *	Initialization function.
 *	This function must be called after initialization of the register
 *	package for the PU. I.e. after REGISTER_Pu_Begin() (register.cxx).
 *
 *	CGCOLOR_Finalize_For_PU(void)
 *	Finalization function.
 *
 *
 *	REGISTER
 * 	CGCOLOR_Choose_Best_Register(ISA_REGISTER_CLASS rc, INT nregs,
 *				     REGISTER_SET subclass_allowed,
 *				     REGISTER_SET allowed,
 *	       			     REGISTER reg_hint)
 *	Return the best register to use for the current history of
 *	register assignment.
 *	The <rc>, <nregs> are the register requierements: <nregs>
 *	continuous registers of the <rc> register class.
 *	The <subclass_allowed> is a constraint on the first register.
 *	The <allowed> is a constraint on all the <nregs> registers.
 *	The <reg_hint> is a preference for the first register. It will
 *	be satisfied when possible dependening on the constraint.
 *	If <reg_hint> is REGISTER_UNDEFINED, no preferencing is done.
 *
 *	REGISTER
 * 	CGCOLOR_Choose_Next_Best_Register(void)
 *
 *	This function can be called after CGCOLOR_Choose_Best_Register().
 *	It returns the "next" best register. This can be used to iterate
 *	over the ordered list of best register choices. The iteration is:
 *	reg = CGCOLOR_Choose_Best_Register(....);
 *	while (reg != REGISTER_UNDEFINED) {
 *	   // do something with <reg> or exit the loop if it is suitable
 *	   reg = Choose_Next_Best_Register();
 *	}
 *
 *
 *	void
 *	CGCOLOR_Allocate_N_Registers(ISA_REGISTER_CLASS rc, REGISTER reg, INT nregs)
 *	Declare to this module that the client has actually allocated a set of
 *	<nregs> continuous registers of <rc> register class starting at <reg>.
 *	This function is a hint to this module that some registers have already
 *	been allocated for the function.
 *	It is not and never will be used as an actual set of allocated registers
 *	for the function, i.e. it can be imprecise.
 *	In particular a client may declare an allocated register here, while a further
 *	copy propagation pass will actually remove the register usage later on. 
 *	This is not harmful, but it may lead to CGCOLOR_Choose_Best_Register() not
 *	making the best choice.
 *    
 *
 *	BOOL
 *	CGCOLOR_Can_Allocate_N_Registers(INT nregs,
 *					 REGISTER_SET subclass_allowed,
 *	       				 REGISTER_SET allowed)
 *	Returns whether <nregs> continuous registers can be allocated given the
 *	<subclass_allowed> constraint for the first register and the <allowed>
 *	constraint for all <nregs> registers.
 */

#ifndef cg_color_INCLUDED
#define cg_color_INCLUDED

#include "defs.h"
#include "errors.h"
#include "targ_isa_registers.h"
#include "register.h"

extern void
CGCOLOR_Initialize_For_PU(void);

extern void
CGCOLOR_Finalize_For_PU(void);

extern REGISTER
CGCOLOR_Choose_Best_Register(ISA_REGISTER_CLASS rc, INT nregs,
			     REGISTER_SET subclass_allowed,
			     REGISTER_SET allowed,
			     REGISTER reg_hint);

extern REGISTER
CGCOLOR_Choose_Next_Best_Register(void);

extern void
CGCOLOR_Allocate_N_Registers(ISA_REGISTER_CLASS rc, REGISTER reg, INT nregs);

extern BOOL
CGCOLOR_Can_Allocate_N_Registers(INT nregs,
				 REGISTER_SET subclass_allowed,
				 REGISTER_SET allowed);


#endif /* !cg_color_INCLUDED */
