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

#ifdef TARG_ST
#include "W_alloca.h"         // using alloca()
#endif


#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "mempool.h"
#include "config.h"
#include "config_TARG.h"
#include "glob.h"
#include "util.h"
#include "data_layout.h"
#include "tn.h"
#include "targ_sim.h"
#include "register.h"
#include "op.h"
#include "cg_flags.h"
#include "calls.h"
#include "cgtarget.h"
#include "reg_live.h"

#include "targ_isa_registers.h"
#include "targ_abi_properties.h"
#include "targ_isa_operands.h"

#include "dyn_isa_api.h"  // reconfigurability
#include "lai_loader_api.h"
#include <ctype.h>                /* for isdigit() */

#include "register_targ.h"	// Implemented interface.


static mISA_REGISTER_CLASS CGTARG_REGISTER_Mtype_RegClass_Map[MTYPE_MAX_LIMIT+1];
//TB associate also a register subclass to an mtype
static mISA_REGISTER_SUBCLASS CGTARG_REGISTER_Mtype_RegSubclass_Map[MTYPE_MAX_LIMIT+1];

/* ====================================================================
 *   CGTARG_Register_Subclass_For_Mtype
 *   CGTARG_Register_Class_For_Mtype
 * ====================================================================
 */
ISA_REGISTER_CLASS
CGTARG_Register_Class_For_Mtype(TYPE_ID mtype)
{
  return   (mtype < FIRST_COMPOSED_MTYPE)
	 ? (ISA_REGISTER_CLASS)CGTARG_REGISTER_Mtype_RegClass_Map[mtype] 
	 : ISA_REGISTER_CLASS_UNDEFINED;
}

ISA_REGISTER_SUBCLASS
CGTARG_Register_Subclass_For_Mtype(TYPE_ID mtype)
{
  return   (mtype < FIRST_COMPOSED_MTYPE)
	 ? (ISA_REGISTER_CLASS)CGTARG_REGISTER_Mtype_RegSubclass_Map[mtype] 
	 : ISA_REGISTER_CLASS_UNDEFINED;
}


/* ====================================================================
 *   CGTARG_REGISTER_Init_Mtype_RegClass_Map
 * ====================================================================
 */
static void 
CGTARG_REGISTER_Init_Mtype_RegClass_Map ()
{
  INT i;
  mISA_REGISTER_CLASS * const map = CGTARG_REGISTER_Mtype_RegClass_Map;

  for (i = 0; i <= MTYPE_LAST; ++i) 
    map[i] = ISA_REGISTER_CLASS_UNDEFINED;

  map[MTYPE_B] = ISA_REGISTER_CLASS_integer;
  map[MTYPE_I1] = ISA_REGISTER_CLASS_integer;
  map[MTYPE_I2] = ISA_REGISTER_CLASS_integer;
  map[MTYPE_I4] = ISA_REGISTER_CLASS_integer;
  map[MTYPE_U1] = ISA_REGISTER_CLASS_integer;
  map[MTYPE_U2] = ISA_REGISTER_CLASS_integer;
  map[MTYPE_U4] = ISA_REGISTER_CLASS_integer;
  map[MTYPE_A4] = ISA_REGISTER_CLASS_integer;
  map[MTYPE_F4] = ISA_REGISTER_CLASS_integer;

  if (Enable_64_Bits_Ops) {
    map[MTYPE_F8] = ISA_REGISTER_CLASS_integer;
    map[MTYPE_I8] = ISA_REGISTER_CLASS_integer;
    map[MTYPE_U8] = ISA_REGISTER_CLASS_integer;
  }

  // Reconfigurability: initialize for extension mtype
  if (NB_PURE_DYNAMIC_MTYPES > 0) {
	for (i = MTYPE_STATIC_LAST + 1; i < FIRST_COMPOSED_MTYPE; i++) {
	  map[i] = EXTENSION_MTYPE_to_REGISTER_CLASS(i);
	}
  }

  return;
}

/* ====================================================================
 *   CGTARG_REGISTER_Init_Mtype_RegSubclass_Map
 * ====================================================================
 */
static void 
CGTARG_REGISTER_Init_Mtype_RegSubclass_Map ()
{
  INT i;
  mISA_REGISTER_SUBCLASS * const map = CGTARG_REGISTER_Mtype_RegSubclass_Map;

  for (i = 0; i <= MTYPE_LAST; ++i) 
    map[i] = ISA_REGISTER_SUBCLASS_UNDEFINED;

  // Reconfigurability: initialize for extension mtype
  if (NB_PURE_DYNAMIC_MTYPES > 0) {
    for (i = MTYPE_STATIC_LAST + 1; i < FIRST_COMPOSED_MTYPE; i++) {
      map[i] = EXTENSION_MTYPE_to_REGISTER_SUBCLASS(i);
    }
  }

  return;
}

/* ====================================================================
 *  CGTARG_Initialize_Register_Class
 *
 *  Initialize the register class 'rclass'. A register class may be
 *  intialized multiple times.
 * ====================================================================
 */
void
CGTARG_Initialize_Register_Class(
  ISA_REGISTER_CLASS rclass
)
{
  /* There are multiple integer return regs -- v0 is the lowest
   * of the set.
   */
  /* [JV] This code should be removed because it managed only reg class
     known by the ABI.
  */
  if (rclass == ISA_REGISTER_CLASS_integer) {
    Set_CLASS_REG_PAIR_reg(CLASS_REG_PAIR_v0, 
			   REGISTER_SET_Choose(REGISTER_CLASS_function_value(rclass)));
    Set_CLASS_REG_PAIR_rclass(CLASS_REG_PAIR_v0, rclass);
  }
  return;
}

/* ====================================================================
 *   CGTARG_REGISTER_Pu_Begin()
 *
 *  Initialize the register package for the current pu.
 * ====================================================================
 */
void
CGTARG_REGISTER_Pu_Begin(void)
{
  // No op.
  return;
}

/* ====================================================================
 *   CGTARG_REGISTER_Begin()
 * ====================================================================
 */
void
CGTARG_REGISTER_Begin(void)
{
  CGTARG_REGISTER_Init_Mtype_RegSubclass_Map ();
  CGTARG_REGISTER_Init_Mtype_RegClass_Map ();
}


#ifdef TARG_ST
/* ====================================================================
 *   Extension_Regname_Register_Class
 *
 *   given register name (asm), get its register class for an extension.
 * ====================================================================
 */
static ISA_REGISTER_CLASS
Extension_Regname_Register_Class (char *regname, int *regnum)
{
  ISA_REGISTER_CLASS cl;
  FOR_ALL_ISA_REGISTER_CLASS( cl ) {
    int reg;
    int first_isa_reg  = ISA_REGISTER_CLASS_info[cl].min_regnum;
    int last_isa_reg   = ISA_REGISTER_CLASS_info[cl].max_regnum;

    for (reg = first_isa_reg; reg < last_isa_reg; reg++) {
      char *temp;
      if (regname[0] == '%') {
	const char *rc_name = ISA_REGISTER_CLASS_info[cl].name;
	temp = (char*)alloca(strlen(rc_name) + strlen(ISA_REGISTER_CLASS_info[cl].reg_name[reg]) + 1);
	sprintf(temp,"%%%s%%%s",rc_name, ISA_REGISTER_CLASS_info[cl].reg_name[reg]);
      } else {
	temp = (char*)ISA_REGISTER_CLASS_info[cl].reg_name[reg];
      }
      if (strcasecmp(temp, regname) == 0) {
	*regnum = reg;
	return cl;
      }
    }
  }
  return ISA_REGISTER_CLASS_UNDEFINED;
}
#endif
				  

/* ====================================================================
 *   CGTARG_Register_Class_Num_From_Name
 *
 *   given register name (asm), get its register class.
 *   returns ISA_REGISTER_CLASS_UNDEFINED if not register match the name.
 * ====================================================================
 */
ISA_REGISTER_CLASS 
CGTARG_Register_Class_Num_From_Name (char *regname, int *regnum) 
{
  ISA_REGISTER_CLASS rclass = ISA_REGISTER_CLASS_UNDEFINED;

  switch (regname[0]) {
  case 'r':
    rclass = ISA_REGISTER_CLASS_integer;
    *regnum = atoi(regname+1);
    break;
  }

  if (rclass == ISA_REGISTER_CLASS_UNDEFINED && Extension_Is_Present)
    return Extension_Regname_Register_Class(regname, regnum);

  return rclass;
}


/* ====================================================================
 *   CGTARG_Forbidden_GRA_Registers 
 * ====================================================================
 */
REGISTER_SET 
CGTARG_Forbidden_GRA_Registers (ISA_REGISTER_CLASS rclass)
{
  REGISTER_SET s = REGISTER_SET_EMPTY_SET;
  return s;
}

REGISTER_SET
CGTARG_Forbidden_LRA_Registers (ISA_REGISTER_CLASS rclass)
{
  return CGTARG_Forbidden_GRA_Registers (rclass);
}

/*
 * The set of forbidden registers for prolog/epilog temporaries
 */
REGISTER_SET
CGTARG_Forbidden_Prolog_Epilog_Registers (ISA_REGISTER_CLASS rclass)
{
  REGISTER_SET s = REGISTER_SET_EMPTY_SET;
  return s;
}

#ifdef TARG_ST
/* ====================================================================
 *   CGTARG_Prefered_GRA_Registers 
 * ====================================================================
 */
REGISTER_SET
CGTARG_Prefered_GRA_Registers(ISA_REGISTER_CLASS rclass)
{
  REGISTER_SET s = REGISTER_SET_EMPTY_SET;
  return s;
}

REGISTER_SET
CGTARG_Prefered_LRA_Registers(ISA_REGISTER_CLASS rclass)
{
  return CGTARG_Prefered_GRA_Registers (rclass);
}

/* ====================================================================
 *   Is_Predicate_REGISTER_CLASS
 *   Returns TRUE if the rclass is the predicate register class.
 * ====================================================================
 */
BOOL Is_Predicate_REGISTER_CLASS(ISA_REGISTER_CLASS rclass) {
  // Should get subclass cpsrcond
  return rclass == ISA_REGISTER_CLASS_cpsr;
}

#endif                   // TARG_ST

