/* 

  Copyright (C) 2007 ST Microelectronics, Inc.  All Rights Reserved. 

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

  Contact information:  ST Microelectronics, Inc., 
  , or: 

  http://www.st.com 

  For further information regarding this notice, see: 

  http: 
*/

#ifdef TARG_ST
#include "W_alloca.h"         // using alloca()
#endif

#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "mempool.h"
#include "config.h"
#include "config_TARG.h"
#include "targ_isa_registers.h"
#include "targ_abi_properties.h"
#include "targ_isa_operands.h"
#include <ctype.h>                /* for isdigit() */

#ifdef TARG_ST
/* ====================================================================
 *   Regname_Register_Class
 *
 *   given register name (asm), get its register class for an
 *   extension.  
 *   TB: This function is not extension register specific and can
 *   apply on any core register also.  *
 *   ====================================================================
 */
static ISA_REGISTER_CLASS
Regname_Register_Class (const char *regname, int *regnum)
{
  ISA_REGISTER_CLASS cl;
  FOR_ALL_ISA_REGISTER_CLASS( cl ) {
    int reg;
    int first_isa_reg  = ISA_REGISTER_CLASS_info[cl].min_regnum;
    int last_isa_reg   = ISA_REGISTER_CLASS_info[cl].max_regnum;

    for (reg = first_isa_reg; reg <= last_isa_reg; reg++) {
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
CGTARG_Register_Class_Num_From_Name (const char *regname, int *regnum) 
{
  ISA_REGISTER_CLASS rclass = ISA_REGISTER_CLASS_UNDEFINED;

  switch (regname[0]) {
  case 'r':
    if (isdigit(regname[1])) {
      rclass = ISA_REGISTER_CLASS_integer;
      *regnum = atoi(regname+1);
    }
    break;
  }

  if (rclass == ISA_REGISTER_CLASS_UNDEFINED) {
    return Regname_Register_Class(regname, regnum);
  }
  else if (*regnum > ISA_REGISTER_CLASS_info[rclass].max_regnum) {
    // Invalid register number
    rclass = ISA_REGISTER_CLASS_UNDEFINED;
  }

  return rclass;
}

//
// Reconfigurability and DWARF stuff
//

/* ====================================================================
 *   CGTARG_DW_DEBUG_Get_Canonical_Index 
 *   Returns the canonical index for an appaired register (or the
 *  "normal" register index for a "normal" register). Also returns the
 *   subclass identifier in the subclass table, and the index of the
 *   appaired register in the considered subclass.
 * ====================================================================
 */
static INT32
CGTARG_DW_DEBUG_Get_Canonical_Index(ISA_REGISTER_CLASS rclass,
                                    INT32  index,
                                    UINT32 bit_size,
                                    ISA_REGISTER_SUBCLASS *subclass,
                                    INT32  *subclass_index)
{
    INT32 nb_basic_reg;
    INT32 nb_paired_reg;
    INT32 regclass_bit_size;
    INT32 can_index;
    INT32 regclass_size;
    INT32 j;
    UINT32 appaired_level;

    const ISA_REGISTER_CLASS_INFO *cinfo;

    FmtAssert(bit_size!=0,("uncorrect register size"));

    cinfo = ISA_REGISTER_CLASS_Info(rclass);

    FmtAssert(index>=ISA_REGISTER_CLASS_INFO_First_Reg(cinfo) && 
              index<=ISA_REGISTER_CLASS_INFO_Last_Reg(cinfo),
              ("uncorrect register index"));

    regclass_bit_size = ISA_REGISTER_CLASS_INFO_Bit_Size(cinfo);

    appaired_level = bit_size / regclass_bit_size;

    // The normal classical case.
    // if appaired_level == 0 and bit_size != regclass_bit_size
    // then bit_size is less than regclass_bit_size and bit_size is not the
    // size of the register class but the size of a type (bug #31965)
    if(regclass_bit_size == bit_size || appaired_level == 0)
     { *subclass       = ISA_REGISTER_SUBCLASS_UNDEFINED;
       *subclass_index = -1; 
        can_index  = index;
        return can_index;
     }
     
    // Looking for appaired registers.
    nb_basic_reg = ISA_REGISTER_CLASS_INFO_Last_Reg(cinfo) -
                   ISA_REGISTER_CLASS_INFO_First_Reg(cinfo)  + 
                   1;

    // Size of appaired register must be a
    // power of two of the basic register size.
    FmtAssert( 0U ==(appaired_level & (appaired_level-1U)) &&
               0U == (bit_size % regclass_bit_size),
               ("uncorrect size for appaired registers"));

    // Calculate canonical index
    can_index = 0;
    j = 1;
    do{
     can_index += nb_basic_reg/j;
     j<<=1;                             // j = j*2;
    } while(j<appaired_level);

   *subclass_index = index / appaired_level;
    can_index     += index / appaired_level; 

    // Looking for the canonical subclass that contains
    // appaired registers
   *subclass      = ISA_REGISTER_SUBCLASS_UNDEFINED;
    nb_paired_reg = nb_basic_reg/appaired_level;

    for(j =ISA_REGISTER_SUBCLASS_MIN;
        j<=ISA_REGISTER_SUBCLASS_MAX && 
        ISA_REGISTER_SUBCLASS_UNDEFINED==*subclass;
        j++) {
      const ISA_REGISTER_SUBCLASS_INFO *sinfo = ISA_REGISTER_SUBCLASS_Info(j);

      // Register class must be ok
      // Subclass size (nb of appaired reg.) must be ok with appairing level
      // Subclass must be canonical.
      if(ISA_REGISTER_SUBCLASS_INFO_Class(sinfo)==rclass        &&
         ISA_REGISTER_SUBCLASS_INFO_Count(sinfo)==nb_paired_reg &&
         ISA_REGISTER_SUBCLASS_INFO_Canonical(sinfo)) {
         *subclass = j;
         }
     }

    FmtAssert(*subclass!=ISA_REGISTER_SUBCLASS_UNDEFINED,
              ("can't find canonical subclass"));

    return index;
}

/* ====================================================================
 *   CGTARG_DW_DEBUG_Get_Extension_Reloc_String 
 *   Returns the relocation string associated to a given register class.
 *   Returns a NULL pointer if:
 *    1 - register class is a core register class,
 *    2 - register class is an extension register class and the
 *        relocation mechanism is not relevant or has not been
 *        specified.
 * ====================================================================
 */
const char*
CGTARG_DW_DEBUG_Get_Extension_Reloc_String(ISA_REGISTER_CLASS rclass)
{
   int ext_id;

   FmtAssert(rclass<ISA_REGISTER_CLASS_MAX+1,
             ("erroneous register class identifier"));

   ext_id = EXTENSION_reg_description_tab[rclass].ext_id;
   if(ext_id==-1)     // Register class belongs to the core.
     return (const char*)(NULL);
 
   return EXTENSION_description_tab[ext_id].reloc_string;
}

/* ====================================================================
 *   CGTARG_DW_DEBUG_Get_Reloc_String 
 *   Returns the name of the extension from which the considered
 *   register class is an architectural ressource.
 *   Returns a NULL pointer if register class belongs to the core.
 * ====================================================================
 */
const char*
CGTARG_DW_DEBUG_Get_Extension_Name(ISA_REGISTER_CLASS rclass)
{
   int ext_id;

   FmtAssert(rclass<ISA_REGISTER_CLASS_MAX+1,
             ("erroneous register class identifier"));

   ext_id = EXTENSION_reg_description_tab[rclass].ext_id;
   if(ext_id==-1)      // Register class belongs to the core.
     return (const char*)(NULL);
 
   return EXTENSION_description_tab[ext_id].extname;
}

/* ====================================================================
 *   CGTARG_DW_DEBUG_Get_Extension_Id 
 *   Return the internal extension identifier the input register
 *   class belongs to. Also return a boolean flag that indicates 
 *   whether extension is activated or not (useful for extensions
 *    built statically with the core).
 * ====================================================================
 */
INT32
CGTARG_DW_DEBUG_Get_Extension_Id(ISA_REGISTER_CLASS rclass, BOOL *activated)
{
   int ext_id;
   FmtAssert(rclass<ISA_REGISTER_CLASS_MAX+1,
             ("erroneous register class identifier"));

   ext_id = EXTENSION_reg_description_tab[rclass].ext_id;
  *activated = ext_id==-1 ? true : EXTENSION_description_tab[ext_id].enabled;

   return ext_id;
}

/* ====================================================================
 *   CGTARG_DW_DEBUG_Get_Reg_Id 
 *   Returns the name of the dwarf debug identifier of a register.
 * ====================================================================
 */
INT32 
CGTARG_DW_DEBUG_Get_Reg_Id(ISA_REGISTER_CLASS rclass,
                           INT32  index,
                           UINT32 bit_size)
{
     
   ISA_REGISTER_SUBCLASS      subclass;
   INT32                     subclass_index;

   // Index is the sum of the canonical index
   // plus the dwarf register class id assigned
   // for the compilation unit
   return CGTARG_DW_DEBUG_Get_Canonical_Index(
                               rclass,
                               index,
                               bit_size,
                               &subclass,
                               &subclass_index) +
          EXTENSION_reg_description_tab[rclass].dw_base_id;
}

/* ====================================================================
 *   CGTARG_DW_DEBUG_Get_Reg_Name 
 *   Returns register name for "normal" and appaired registers.
 * ====================================================================
 */
const char*
CGTARG_DW_DEBUG_Get_Reg_Name(ISA_REGISTER_CLASS rclass,
                             INT32  index,
                             UINT32 bit_size)
{
   ISA_REGISTER_SUBCLASS      subclass;
   INT32                     subclass_index;
   INT32                     can_index;
   const char                *res;

   const ISA_REGISTER_CLASS_INFO    *cinfo;
   const ISA_REGISTER_SUBCLASS_INFO *sinfo;

   // Get canonical index.
   can_index = CGTARG_DW_DEBUG_Get_Canonical_Index(
                                    rclass,
                                    index,
                                    bit_size,
                                    &subclass,
                                    &subclass_index);

   if(can_index==index) { // For a normal register
    cinfo = ISA_REGISTER_CLASS_Info(rclass); 
    res   = ISA_REGISTER_CLASS_INFO_Reg_Name(cinfo,index);
    }
   else {                 // For an appaired register
    sinfo = ISA_REGISTER_SUBCLASS_Info(subclass);
    res   = ISA_REGISTER_SUBCLASS_INFO_Reg_Name(sinfo,subclass_index);
    }

   return res;
}

/* ====================================================================
 *   CGTARG_DW_DEBUG_Begin
 *   Initialization step. Once per file.
 * ====================================================================
 */
void
CGTARG_DW_DEBUG_Begin( void )
{
   return;
}
