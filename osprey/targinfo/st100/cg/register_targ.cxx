/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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


#define INCLUDING_IN_REGISTER // we modify register classes and we're
			      // really part of the register implementation

#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "mempool.h"
#include "config.h"
#include "glob.h"
#include "util.h"
#include "data_layout.h"
#include "tn.h"
#include "targ_sim.h"
#include "register.h"
#include "op.h"
#include "lai_flags.h"
#include "calls.h"
#include "cgtarget.h"
#include "reg_live.h"

#include "targ_isa_registers.h"
#include "targ_abi_properties.h"
#include "targ_isa_operands.h"

/* should be generated together with targ_isa_register.[hc] */
char *ISA_REGISTER_CLASS_symbol[] = {
  "",   /* ISA_REGISTER_CLASS_UNDEFINED */
  "_d",  /* ISA_REGISTER_CLASS_du */
  "_a",  /* ISA_REGISTER_CLASS_au */
  "_b",  /* ISA_REGISTER_CLASS_guard */
  "_c",  /* ISA_REGISTER_CLASS_control */
  "_l"   /* ISA_REGISTER_CLASS_loop */
};

// REGISTER values are biased by REGISTER_MIN, so apply
// it to get REGISTER value given a machine reg number
#define FIRST_INPUT_REG (32+REGISTER_MIN)
#define FIRST_OUTPUT_REG (127+REGISTER_MIN)
#define LAST_STACKED_REG (127+REGISTER_MIN)
#define FIRST_ROTATING_INTEGER_REG (32+REGISTER_MIN)
#define FIRST_ROTATING_FLOAT_REG (32+REGISTER_MIN)
#define FIRST_ROTATING_PREDICATE_REG (16+REGISTER_MIN)
#define LAST_ROTATING_INTEGER_REG (127+REGISTER_MIN)
#define LAST_ROTATING_FLOAT_REG (127+REGISTER_MIN)
#define LAST_ROTATING_PREDICATE_REG (63+REGISTER_MIN)


static INT num_output_parameters;
static INT num_caller;


static char outregname[6] = "out0";

mISA_REGISTER_CLASS Mtype_RegClass_Map[MTYPE_LAST+1];

/* ====================================================================
 *   Init_Mtype_RegClass_Map
 * ====================================================================
 */
void 
Init_Mtype_RegClass_Map ()
{
  INT i;
  mISA_REGISTER_CLASS * const map = Mtype_RegClass_Map;

  for (i = 0; i <= MTYPE_LAST; ++i) 
    map[i] = ISA_REGISTER_CLASS_UNDEFINED;

  map[MTYPE_B] = ISA_REGISTER_CLASS_guard;
  map[MTYPE_I1] = ISA_REGISTER_CLASS_du;
  map[MTYPE_I2] = ISA_REGISTER_CLASS_du;
  map[MTYPE_I4] = ISA_REGISTER_CLASS_du;
  map[MTYPE_I5] = ISA_REGISTER_CLASS_du;
  map[MTYPE_I8] = ISA_REGISTER_CLASS_UNDEFINED;
  map[MTYPE_U1] = ISA_REGISTER_CLASS_du;
  map[MTYPE_U2] = ISA_REGISTER_CLASS_du;
  map[MTYPE_U4] = ISA_REGISTER_CLASS_du;
  map[MTYPE_U5] = ISA_REGISTER_CLASS_du;
  map[MTYPE_U8] = ISA_REGISTER_CLASS_UNDEFINED;
  map[MTYPE_A4] = ISA_REGISTER_CLASS_au;
  map[MTYPE_F4] = ISA_REGISTER_CLASS_UNDEFINED;
  map[MTYPE_F8] = ISA_REGISTER_CLASS_UNDEFINED;
  map[MTYPE_F10] = ISA_REGISTER_CLASS_UNDEFINED;
  map[MTYPE_F16] = ISA_REGISTER_CLASS_UNDEFINED;

  return;
}

/* ====================================================================
 * Define a few things that differ among code generators
 * ====================================================================
 */

/* ====================================================================
 *   Compute_Dedicated_PU_Livein
 * ====================================================================
 */
void
Compute_Dedicated_PU_Livein (
  ST *pu_st,
  REGSET livein
)
{
  livein[REGISTER_CLASS_sp] = 
	REGISTER_SET_Union1 (livein[REGISTER_CLASS_sp], REGISTER_sp);
  livein[REGISTER_CLASS_gp] = 
	REGISTER_SET_Union1 (livein[REGISTER_CLASS_gp], REGISTER_gp);
  livein[REGISTER_CLASS_ra] = 
	REGISTER_SET_Union1 (livein[REGISTER_CLASS_ra], REGISTER_ra);

  return;
}

/* ====================================================================
 *   Compute_Dedicated_PU_Liveout
 * ====================================================================
 */
void
Compute_Dedicated_PU_Liveout (
  ST *pu_st,
  REGSET liveout
)
{
  // add sp to list of liveout registers.
  liveout[REGISTER_CLASS_sp] = 
    REGISTER_SET_Union1 (liveout[REGISTER_CLASS_sp], REGISTER_sp);

  return;
}

/* ====================================================================
 *   Compute_Call_Livein
 * ====================================================================
 */
void
Compute_Call_Livein (
  ST *call_st,
  BB *bb,
  REGSET livein
)
{
  // add sp, gp to the livein set.
  livein[REGISTER_CLASS_gp] = 
		REGISTER_SET_Union1 (livein[REGISTER_CLASS_gp], REGISTER_gp);
  livein[REGISTER_CLASS_sp] = 
		REGISTER_SET_Union1 (livein[REGISTER_CLASS_sp], REGISTER_sp);

  // add ra and callee saves if tail call.
  if (BB_tail_call(bb)) {
    livein[REGISTER_CLASS_ra] = 
		REGISTER_SET_Union1 (livein[REGISTER_CLASS_ra], REGISTER_ra);
  }

  return;
}

/* ====================================================================
 * Shared data structures
 * ====================================================================
 */

/* Track the "allocatable" state of each register.
 */
enum {
  AS_default = 0,	/* the default is what targ_info says */
  AS_allocatable = 1,
  AS_not_allocatable = 2
};

static mUINT8 reg_alloc_status[ISA_REGISTER_CLASS_MAX + 1][REGISTER_MAX + 1];

// list of registers that should not be allocated, both globally and locally.
static vector< pair< ISA_REGISTER_CLASS, REGISTER> > dont_allocate_these_registers;
static vector< pair< ISA_REGISTER_CLASS, REGISTER> > dont_allocate_these_registers_in_pu;

/* ====================================================================
 * ====================================================================
 *
 * Initialization and termination
 *
 * ====================================================================
 * ====================================================================
 */


/* ====================================================================
 *  Initialize_Register_Class
 *
 *  Initialize the register class 'rclass'. A register class may be
 *  intialized multiple times.
 * ====================================================================
 */
void
Initialize_Register_Class(
  ISA_REGISTER_CLASS rclass
)
{
  INT32              i;
  const ISA_REGISTER_CLASS_INFO *icinfo = ISA_REGISTER_CLASS_Info(rclass);
  const char        *rcname         = ISA_REGISTER_CLASS_INFO_Name(icinfo);
  INT		     bit_size       = ISA_REGISTER_CLASS_INFO_Bit_Size(icinfo);
  INT                first_isa_reg  = ISA_REGISTER_CLASS_INFO_First_Reg(icinfo);
  INT                last_isa_reg   = ISA_REGISTER_CLASS_INFO_Last_Reg(icinfo);
  INT                register_count = last_isa_reg - first_isa_reg + 1;

  REGISTER_SET       allocatable    = REGISTER_SET_EMPTY_SET;
  REGISTER_SET       caller         = REGISTER_SET_EMPTY_SET;
  REGISTER_SET       callee         = REGISTER_SET_EMPTY_SET;
  REGISTER_SET       func_argument  = REGISTER_SET_EMPTY_SET;
  REGISTER_SET       func_value     = REGISTER_SET_EMPTY_SET;
  REGISTER_SET       shrink_wrap    = REGISTER_SET_EMPTY_SET;
  REGISTER_SET	     stacked        = REGISTER_SET_EMPTY_SET;
  REGISTER_SET	     rotating       = REGISTER_SET_EMPTY_SET;


  /* Verify we have a valid rclass and that the type used to implement 
   * a register set is large enough.
   */
  FmtAssert(rclass >= ISA_REGISTER_CLASS_MIN && 
	    rclass <= ISA_REGISTER_CLASS_MAX,
	                       ("invalide register class %d", (INT)rclass));
  FmtAssert((sizeof(REGISTER_SET) * 8) >= register_count,
	    ("REGISTER_SET type cannot represent all registers in "
	     "the class %s", rcname));

  REGISTER_CLASS_name(rclass) = rcname;

  /* Now make sets of various register properties:
   */
  for (i = 0; i < register_count; ++i) {
    INT      isa_reg        = i + first_isa_reg;
    REGISTER reg            = i + REGISTER_MIN;
    BOOL     is_allocatable = ABI_PROPERTY_Is_allocatable(rclass, isa_reg);
    INT      alloc_status   = reg_alloc_status[rclass][reg];

    /* CG likes to pretend that a class with only one register can't
     * be allocated, so perpetuate that illusion.
     */
    if (register_count <= 1) is_allocatable = FALSE;

    switch ( alloc_status ) {
      case AS_allocatable:
	is_allocatable = TRUE;
	break;
      case AS_not_allocatable:
	is_allocatable = FALSE;
	break;
      case AS_default:
	break;
      default:
	Is_True(FALSE, ("unhandled allocations status: %d", alloc_status));
    }

    if (is_allocatable) {
      allocatable = REGISTER_SET_Union1(allocatable, reg);

      if (ABI_PROPERTY_Is_global_ptr(rclass, isa_reg)) {
        if (GP_Is_Preserved) {
          /* neither caller nor callee saved (always preserved). */
        } else if (Is_Caller_Save_GP) {
          /* caller-saved. */
          caller = REGISTER_SET_Union1(caller, reg);
        } else {
          /* callee-saved. */
          callee = REGISTER_SET_Union1(callee, reg);
        }
      }
      else {
        if (ABI_PROPERTY_Is_callee(rclass, isa_reg)) {
          callee = REGISTER_SET_Union1(callee, reg);
          shrink_wrap = REGISTER_SET_Union1(shrink_wrap, reg);
        }
        if (ABI_PROPERTY_Is_caller(rclass, isa_reg))
          caller = REGISTER_SET_Union1(caller, reg);
        if (ABI_PROPERTY_Is_func_arg(rclass, isa_reg))
          func_argument = REGISTER_SET_Union1(func_argument, reg);
        if (ABI_PROPERTY_Is_func_val(rclass, isa_reg))
          func_value = REGISTER_SET_Union1(func_value, reg);
        if (ABI_PROPERTY_Is_ret_addr(rclass, isa_reg))
          shrink_wrap = REGISTER_SET_Union1(shrink_wrap, reg);
      }
    }

    REGISTER_bit_size(rclass, reg) = bit_size;
    REGISTER_machine_id(rclass, reg) = isa_reg;
    REGISTER_allocatable(rclass, reg) = is_allocatable;
    REGISTER_name(rclass, reg) = 
                   ISA_REGISTER_CLASS_INFO_Reg_Name(icinfo, isa_reg);

    if ( ABI_PROPERTY_Is_frame_ptr(rclass, isa_reg) ) {
      Set_CLASS_REG_PAIR_reg(CLASS_REG_PAIR_fp, reg);
      Set_CLASS_REG_PAIR_rclass(CLASS_REG_PAIR_fp, rclass);
    }
    else if ( ABI_PROPERTY_Is_global_ptr(rclass, isa_reg) ) {
      Set_CLASS_REG_PAIR_reg(CLASS_REG_PAIR_gp, reg);
      Set_CLASS_REG_PAIR_rclass(CLASS_REG_PAIR_gp, rclass);
    }
    else if ( ABI_PROPERTY_Is_ret_addr(rclass, isa_reg) ) {
      Set_CLASS_REG_PAIR_reg(CLASS_REG_PAIR_ra, reg);
      Set_CLASS_REG_PAIR_rclass(CLASS_REG_PAIR_ra, rclass);
    }
    else if ( ABI_PROPERTY_Is_stack_ptr(rclass, isa_reg) ) {
      Set_CLASS_REG_PAIR_reg(CLASS_REG_PAIR_sp, reg);
      Set_CLASS_REG_PAIR_rclass(CLASS_REG_PAIR_sp, rclass);
    }
    else if ( ABI_PROPERTY_Is_static_link(rclass, isa_reg) ) {
      Set_CLASS_REG_PAIR_reg(CLASS_REG_PAIR_static_link, reg);
      Set_CLASS_REG_PAIR_rclass(CLASS_REG_PAIR_static_link, rclass);
    }
    else if ( ABI_PROPERTY_Is_link(rclass, isa_reg) ) {
      Set_CLASS_REG_PAIR_reg(CLASS_REG_PAIR_link, reg);
      Set_CLASS_REG_PAIR_rclass(CLASS_REG_PAIR_link, rclass);
    }
    else if ( ABI_PROPERTY_Is_loop_count(rclass, isa_reg) ) {
      Set_CLASS_REG_PAIR_reg(CLASS_REG_PAIR_lc, reg);
      Set_CLASS_REG_PAIR_rclass(CLASS_REG_PAIR_lc, rclass);
    }
    else if ( ABI_PROPERTY_Is_true_predicate(rclass, isa_reg) ) {
      Set_CLASS_REG_PAIR_reg(CLASS_REG_PAIR_true, reg);
      Set_CLASS_REG_PAIR_rclass(CLASS_REG_PAIR_true, rclass);
    }
  }

  REGISTER_CLASS_universe(rclass)          =
	REGISTER_SET_Range(REGISTER_MIN, REGISTER_MIN + register_count - 1);
  REGISTER_CLASS_allocatable(rclass)       = allocatable;
  REGISTER_CLASS_callee_saves(rclass)      = callee;
  REGISTER_CLASS_caller_saves(rclass)      = caller;
  REGISTER_CLASS_function_argument(rclass) = func_argument;
  REGISTER_CLASS_function_value(rclass)    = func_value;
  REGISTER_CLASS_shrink_wrap(rclass)       = shrink_wrap;
  REGISTER_CLASS_stacked(rclass)           = stacked;
  REGISTER_CLASS_rotating(rclass)          = rotating;
  REGISTER_CLASS_register_count(rclass)    = register_count;
  REGISTER_CLASS_can_store(rclass)
	= ISA_REGISTER_CLASS_INFO_Can_Store(icinfo);
  REGISTER_CLASS_multiple_save(rclass)
	= ISA_REGISTER_CLASS_INFO_Multiple_Save(icinfo);

  /* There are multiple integer return regs -- v0 is the lowest
   * of the set.
   */
  if (rclass == ISA_REGISTER_CLASS_du) {
    Set_CLASS_REG_PAIR_reg(CLASS_REG_PAIR_v0, REGISTER_SET_Choose(func_value));
    Set_CLASS_REG_PAIR_rclass(CLASS_REG_PAIR_v0, rclass);
  }

  return;
}

/* ====================================================================
 *   REGISTER_Check_Alloc_Status (rclass)
 * ====================================================================
 */
BOOL
REGISTER_Check_Alloc_Status (
  ISA_REGISTER_CLASS rclass
)
{
  REGISTER reg;
  BOOL re_init = FALSE;

  for (reg = REGISTER_MIN;
       reg <= REGISTER_CLASS_last_register(rclass);
       reg++) {
    if (reg_alloc_status[rclass][reg] != AS_default) {
      reg_alloc_status[rclass][reg] = AS_default;
      re_init = TRUE;
    }
  }

  return re_init;
}

/* ====================================================================
 *  REGISTER_set_allocatable
 *
 *  Return TRUE if alloc status changed.
 * ====================================================================
 */
BOOL
REGISTER_set_allocatable(
  ISA_REGISTER_CLASS rclass,
  REGISTER           reg
)
{
  INT prev_status = reg_alloc_status[rclass][reg];
  reg_alloc_status[rclass][reg] = AS_allocatable;
  return (prev_status != AS_allocatable);
}

/* ====================================================================
 *  REGISTER_set_not_allocatable
 *
 *  Return TRUE if alloc status changed.
 * ====================================================================
 */
BOOL
REGISTER_set_not_allocatable(
  ISA_REGISTER_CLASS rclass,
  REGISTER           reg
)
{
  INT prev_status = reg_alloc_status[rclass][reg];
  reg_alloc_status[rclass][reg] = AS_not_allocatable;
  return (prev_status != AS_not_allocatable);
}

/* ====================================================================
 *   Set_Register_Never_Allocatable (regname)
 *
 *   user wants given register to not be allocatable in file.
 * ====================================================================
 */
void
Set_Register_Never_Allocatable (
  char *regname
) 
{
  ISA_REGISTER_CLASS rclass;
  REGISTER reg;

  switch (regname[0]) {
	case 'r':
		rclass = ISA_REGISTER_CLASS_du;
		break;
	case 'f':
		rclass = ISA_REGISTER_CLASS_UNDEFINED;
		break;
	case 'p':
		rclass = ISA_REGISTER_CLASS_au;
		break;
	default:
		FmtAssert(FALSE, ("unexpected reg letter %c", regname[0]));
  }
  reg = REGISTER_MIN + atoi(regname+1);
  FmtAssert(reg <= REGISTER_CLASS_last_register(rclass),
		              ("%s is not a valid register", regname));
  dont_allocate_these_registers.push_back( make_pair( rclass, reg ));
  return;
}

// user wants given register to not be allocatable in file.
void
Set_Register_Never_Allocatable (PREG_NUM preg) 
{
  ISA_REGISTER_CLASS rclass;
  REGISTER reg;
  CGTARG_Preg_Register_And_Class(preg, &rclass, &reg);
  dont_allocate_these_registers.push_back( make_pair( rclass, reg ));
}

  // now check for any registers that user doesn't want allocated
void
Set_Register_Not_Allocatable ()
{
  vector< pair< ISA_REGISTER_CLASS, REGISTER > >::iterator r;
  for (r = dont_allocate_these_registers.begin(); 
	r != dont_allocate_these_registers.end(); 
	++r) {
    REGISTER_set_not_allocatable ((*r).first, (*r).second);
  }
}

