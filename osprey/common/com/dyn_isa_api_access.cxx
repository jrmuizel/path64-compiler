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
/*
 * This modules defines the common API used to access the ISA description of extensions.
 * It is compatible with the successives library API revisions and insure the migration
 * to the latest one, hiding the implemention details to the other parts of thecompiler.
 *
 * Changes between revisions:
 * +----------+----------------------------------------------------------------+
 * | Revision | Description                                                    |
 * +==========+================================================================+
 * | 20070126 |   Initial state                                                |
 * +----------+----------------------------------------------------------------+
 * | 20070615 | - Updated 'bit_size' field of ISA_REGISTER_CLASS_INFO from     |
 * |          |   mUINT8 to mUINT16                                            |
 * |          |   [commit #12476]                                              |
 * +----------+----------------------------------------------------------------+
 * | 20070924 | - pixel support                                                |
 * +----------+----------------------------------------------------------------+
 * | 20080307 | Support changes done for dev-air2                              |
 * |          | - ISA_PRINT_INFO converted from a structure to a function      |
 * |          |   -> a generic function has been defined for old extension     |
 * |          | - ISA_OPERAND_VALTYP structure has been extended with          |
 * |          |   relocation information                                       |
 * |          |   -> Those fields are currently cleared for older extensions   |
 * |          | - ISA_EXEC_UNIT_PROPERTY changed from 8 to 16 bits             |
 * |          | - ISA_LIT_CLASS_INFO has been extended with 'is_negative' field|
 * |          | - ISA_EXEC_UNIT_SLOTS has been added (new targinfo entry)      |
 * |          | - ISA_BUNDLE_slot_count_tab has been added (new targinfo entry)|
 * +----------+----------------------------------------------------------------+
 * | 20081010 | Support changes done for dev-loader-targinfo-gbu               |
 * |          | - Introduced ISA_PARSE dynamic loading in extensions.          |
 * |          | - Introduced ISA_PACK dynamic loading in extensions.           |
 * |          | - Introduced ISA_DECODE dynamic loading in extensions.         |
 * |          | - Introduced ISA_RELOC dynamic loading in extensions.          |
 * |          | - Introduced ISA_BINUTILS dynamic loading in extensions.       |
 * |          | - ISA_OPERAND_VALTYP resized relocs internal array from 58 to  |
 * |          |   68.                                                          |
 * +----------+----------------------------------------------------------------+
 * | 20090126 | - ISA_LIT_CLASS_INFO has been extended with 'max_right_rotate' |
 * |          |   and 'right_rotate_mask' to managed ARM immediate rotate.     |
 * |          | - ISA_OPERAND_INFO has been extended with 'conflicts' to avoid |
 * |          |   to allocate the same register between specified result and   |
 * |          |   operand.                                                     |
 * +----------+----------------------------------------------------------------+
 * | 20090408 | - ISA_OPERAND_VALTYP resized relocs internal array from 70 to  |
 * |          |   149. Introduction of 2 new relocations for STxP70 v4.        |
 * +----------+----------------------------------------------------------------+
 * | 20090416 | - Introduce callback for ISA_REGISTER_CLASS and                |
 * |          |   ISA_REGISTER_SUBCLASS offset of extensions.                  |
 * |          | - Introduce callback for ISA_LIT_CLASS offset of extensions.   |
 * +----------+----------------------------------------------------------------+
 * | 20090727 | - Introduced new relocations to deal with dynamic loading.     |
 * |          |   Relocs internal array is moved from 149 to 193.              |
 * |          | - Introduced field in relocation to tackle RLLIB relocation    |
 * |          |   specific action.                                             |
 * +----------+----------------------------------------------------------------+
 * | 20090915 | - ISA_OPERAND_INFO does not contain static arrays anymore, but |
 * |          |   ptr to them. This avoid future breaking of backward compat.  |
 * |          |   when update the max number of operands or results.           |
 * |          | - In ISA_OPERAND_INFO, type of elements of <opnd> and <result> |
 * |          |   arrays is changed from mUINT8 to mUINT16                     |
 * |          |   (on xp70, when several extensions are used, more than 255    |
 * |          |    VALTYP might be used)                                       |
 * |          | - In ISA_OPERAND_VALTYP, literal class is now encoded on 16bits|
 * |          |   (used to be 8bits), and the fields have been reorganized     |
 * +----------+----------------------------------------------------------------+
 * | 20100114 | - ISA_EXEC_UNIT_PROPERTY changed from 16 to 32 bits            |
 * +----------+----------------------------------------------------------------+
 * | 20100120 | - Introduce ISA_RELOC_VARIANT_INFO                             |
 * |          | - Introduce new relocations to deal with STxP70 v4 dynamic     |
 * |          |   loading. Reclos internal array is moved from 193 to 307.     |
 * +----------+----------------------------------------------------------------+
 * 
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dyn_isa_api_access.h"
#include "errors.h"

BE_EXPORTED extern EXTENSION_ISA_Info *EXTENSION_Get_ISA_Info_From_TOP(TOP id);

// ========================================================================
// List of compatible API revisions for ISA part of the library description
// ========================================================================
#define    NB_SUPPORTED_ISA_REV 12
#define    REV_20070126        (20070126)
#define    REV_20070615        (20070615)
#define    REV_20070924        (20070924)
#define    REV_20080307        (20080307)
#define    REV_20081010        (20081010)
#define    REV_20090126        (20090126)
#define    REV_20090408        (20090408)
#define    REV_20090416        (20090416)
#define    REV_20090727        (20090727)
#define    REV_20090915        (20090915)
#define    REV_20100114        (20100114)
#define    REV_20100120        (20100120)

static INT supported_ISA_rev_tab[NB_SUPPORTED_ISA_REV] = {
  REV_20070126,
  REV_20070615,
  REV_20070924,
  REV_20080307,
  REV_20081010,
  REV_20090126,
  REV_20090408,
  REV_20090416,
  REV_20090727,
  REV_20090915,
  REV_20100114,
  MAGIC_NUMBER_EXT_ISA_API   /* current one */
};


// Return TRUE if the specified revision is compatible with current compiler,
// FALSE otherwise.
BOOL EXTENSION_Is_Supported_ISA_Revision(INT rev) {
  INT i;
  for (i=0; i<NB_SUPPORTED_ISA_REV; i++) {
    if (rev == supported_ISA_rev_tab[i]) {
      return (TRUE);
    }
  }
  return (FALSE);
}

// #############################################################################
// ##
// ## Versionned structures that might need to be migrated
// ##
// #############################################################################
// -------- Changed at rev 20070615 --------------------------------------------
typedef struct {
  mUINT8 isa_mask;
  mUINT8 min_regnum;
  mUINT8 max_regnum;
  mUINT8 bit_size;
  mBOOL is_ptr;
  mBOOL can_store;
  mBOOL multiple_save;
  const char *name;
  const char *reg_name[ISA_REGISTER_MAX+1];
} ISA_REGISTER_CLASS_INFO_pre_20070615;


// -------- Changed at rev 20070924 --------------------------------------------
typedef struct
{
  TOP (*get_load_TOP) (INT size, AM_Base_Reg_Type base_reg, BOOL offs_is_imm, BOOL offs_is_incr);
  TOP (*get_store_TOP)(INT size, AM_Base_Reg_Type base_reg, BOOL offs_is_imm, BOOL offs_is_incr);
  TOP (*get_move_X2X_TOP)               (INT size);
  TOP (*get_move_R2X_TOP)               (INT size);
  TOP (*get_move_X2R_TOP)               (INT size);
  TOP (*get_insert_and_zeroext_R2X_TOP) (INT size);
  TOP (*get_rotate_and_insert_R2X_TOP)  (INT size);
  TOP (*get_insert_and_zeroext_RR2X_TOP)(INT size);
  TOP (*get_rotate_and_insert_RR2X_TOP) (INT size);
  TOP (*get_extract_X2R_TOP)            (INT size);
  TOP (*get_extract_and_rotate_X2R_TOP) (INT size);
  TOP (*get_clear_TOP)                  (INT size);
  TOP (*get_zeroext_P2X_TOP)            (INT xsize);
  TOP (*get_signext_P2X_TOP)            (INT xsize);
  TOP (*get_zeroext_X_TOP)              (INT size);
  TOP (*get_signext_X_TOP)              (INT size);
  TOP (*get_simulated_compose_TOP) (INT from_size, INT to_size);
  TOP (*get_simulated_extract_TOP) (INT from_size, INT to_size);
  TOP (*get_simulated_widemove_TOP)(INT size); 
} extension_regclass_t_pre_20070924;


// -------- Changed at rev 20080307 --------------------------------------------
typedef struct {
  mUINT8  rclass;
  mUINT8  rsubclass;
  mUINT8  lclass;
  mUINT8  eclass;
  mUINT16 size;
  mUINT8  flags;
} ISA_OPERAND_VALTYP_pre_20080307;

typedef mUINT8 ISA_EXEC_UNIT_PROPERTY_pre_20080307;

typedef struct {
struct { INT64 min; INT64 max; INT32 scaling_value; INT32 scaling_mask; } range[2/*MAX_RANGE_STATIC*/];
  mUINT8 num_ranges;
  mBOOL is_signed;
  const char *name;
} ISA_LIT_CLASS_INFO_pre_20080307;

typedef enum {
  ISA_PRINT_COMP_end    = 0,  /* End of list marker */
  ISA_PRINT_COMP_name   = 1,  /* Instruction name */
  ISA_PRINT_COMP_opnd   = 2,  /* OPND+n => operand n */
  ISA_PRINT_COMP_opnd_MAX = 28,  /* Max operand id */
  ISA_PRINT_COMP_result = 29,  /* RESULT+n => result n */
  ISA_PRINT_COMP_result_MAX = 54,  /* Max result id */
  ISA_PRINT_COMP_MAX    = 54   /* Last component */
} ISA_PRINT_COMP_pre_20080307;

typedef struct {
  const char *format;
  mUINT8 comp[55];
} ISA_PRINT_INFO_pre_20080307;

inline const char* ISA_PRINT_INFO_pre_20080307_Format(const ISA_PRINT_INFO_pre_20080307 *info) {
  return info->format;
}

inline INT ISA_PRINT_INFO_pre_20080307_Comp(const ISA_PRINT_INFO_pre_20080307 *info, INT index) {
  return info->comp[index];
}

/*
 * Generic function used to print an extension instruction.
 * For new extension (rev >= REV_20080307), this function is not used as the
 * targinfo generator should have automatically generated it. But for  older
 * one, we use this generic function.
 * This code is based on the former version of TI_ASM_Print_Inst()
 * [targinfo/access/ti_asm.c]
 */
static INT Print_Extension_Instr_pre_20080307(ISA_PRINT_INFO_print  print_func,
                                              void                 *stream,
                                              TOP                   topcode,
                                              const char           *asmname,
                                              ISA_PRINT_OPND_INFO  *result,
                                              ISA_PRINT_OPND_INFO  *opnd ) {
  INT res = -1;
#ifdef BACK_END
  {
    INT i;
    INT st;
    INT comp;
    const char *arg[(ISA_PRINT_COMP_MAX<13)?13:ISA_PRINT_COMP_MAX];
    char predicate[8];

    EXTENSION_ISA_Info *ext_isa_info = EXTENSION_Get_ISA_Info_From_TOP(topcode);

    // Retrieve pointer to former ISA_PRINT_INFO for current operation
    INT local_topcode = ext_isa_info->get_TOP_local(topcode);
    const ISA_PRINT_INFO_pre_20080307 *pinfo_tab =
      (const ISA_PRINT_INFO_pre_20080307*)(ext_isa_info->get_ISA_PRINT_info_pre_20080307_tab());
    const ISA_PRINT_INFO_pre_20080307 *pinfo = &pinfo_tab[ext_isa_info->get_ISA_PRINT_info_index_tab()[local_topcode]-1];

    i = 0;
    do {
      comp = ISA_PRINT_INFO_pre_20080307_Comp(pinfo, i);
      
      switch (comp) {
      case ISA_PRINT_COMP_name:
	arg[i] = asmname;
	break;
	
      case ISA_PRINT_COMP_end:
	break;
	
      default:
	if ((comp >= ISA_PRINT_COMP_opnd) &&
	    (comp <= ISA_PRINT_COMP_opnd_MAX)) {
	  arg[i] = opnd[comp - ISA_PRINT_COMP_opnd].name;
	}
	else if ((comp >= ISA_PRINT_COMP_result) &&
	       (comp <= ISA_PRINT_COMP_result_MAX)) {
	  arg[i] = result[comp - ISA_PRINT_COMP_result].name;
	}
	else {
	  // Unexpected type
	  return res;
	}
      }
    } while (++i, comp != ISA_PRINT_COMP_end);
    
    if (i > 12) {
      // Too much arguments for operation
      return res;
    }

    // Check that first argument is a guard, a suffix it with the expect symbol
    if (!(arg[0][0] == 'G' && arg[0][1]>='0' && arg[0][1]<='7')) {
      return res;
    }
    sprintf(predicate, "%s ? ", arg[0]);
    arg[0] = predicate;
    
    if (NULL==print_func) {
      printf( "        ");
      res = printf( pinfo->format,
		    arg[0],  arg[1],  arg[2],  arg[3], 
		    arg[4],  arg[5],  arg[6],  arg[7],
		    arg[8],  arg[9],  arg[10], arg[11],
		    arg[12]);
    } else {
      print_func(stream, "        ");
      res = print_func(stream, pinfo->format,
		       arg[0],  arg[1],  arg[2],  arg[3], 
		       arg[4],  arg[5],  arg[6],  arg[7],
		       arg[8],  arg[9],  arg[10], arg[11],
		       arg[12]);
    }
  }
#endif // BACK_END
  return res;
}

// -------- Changed at rev 20081010 --------------------------------------------
#define ISA_RELOC_STATIC_MAX_pre_20081010 58
typedef struct {
  mUINT8  rclass;
  mUINT8  rsubclass;
  mUINT8  lclass;
  mUINT8  eclass;
  mUINT16 size;
  mUINT8  flags;
  mUINT8  default_reloc;
  mUINT8  relocs;
  mINT8   reloc[ISA_RELOC_STATIC_MAX_pre_20081010];
} ISA_OPERAND_VALTYP_pre_20081010;

// -------- Changed at rev 20090126 --------------------------------------------

typedef struct {
  struct { INT64 min; INT64 max; INT32 scaling_value; INT32 scaling_mask; } range[2/*MAX_RANGE_STATIC*/];
  mUINT8 num_ranges;
  mBOOL is_signed;
  mBOOL is_negative;
  const char *name;
} ISA_LIT_CLASS_INFO_pre_20090126;

#define ISA_OPERAND_max_operands_pre_20090126 27
#define ISA_OPERAND_max_results_pre_20090126  26
typedef struct {
  mUINT8  opnds;
  mUINT8  opnd[ISA_OPERAND_max_operands_pre_20090126];
  mUINT16 ouse[ISA_OPERAND_max_operands_pre_20090126];
  mUINT8  results;
  mUINT8  result[ISA_OPERAND_max_results_pre_20090126];
  mUINT16 ruse[ISA_OPERAND_max_results_pre_20090126];
  mINT8   same_res[ISA_OPERAND_max_results_pre_20090126];
} ISA_OPERAND_INFO_pre_20090126;

// -------- Changed at rev 20090408 --------------------------------------------
#define ISA_RELOC_STATIC_MAX_pre_20090408 70
typedef struct {
  mUINT8  rclass;
  mUINT8  rsubclass;
  mUINT8  lclass;
  mUINT8  eclass;
  mUINT16 size;
  mUINT8  flags;
  mUINT8  default_reloc;
  mUINT8  relocs;
  mINT8   reloc[ISA_RELOC_STATIC_MAX_pre_20090408];
} ISA_OPERAND_VALTYP_pre_20090408;

// -------- Changed at rev 20090727 --------------------------------------------
#define ISA_RELOC_STATIC_MAX_pre_20090727 149
typedef struct {
  mUINT8  rclass;
  mUINT8  rsubclass;
  mUINT8  lclass;
  mUINT8  eclass;
  mUINT16 size;
  mUINT8  flags;
  mUINT8  default_reloc;
  mUINT8  relocs;
  mINT8   reloc[ISA_RELOC_STATIC_MAX_pre_20090727];
} ISA_OPERAND_VALTYP_pre_20090727;
typedef struct {
  struct { UINT8 elf_id; UINT8 start_bit; UINT8 stop_bit; } bitfield[MAX_BITFIELDS_STATIC];
  mUINT8 bitfields;
  BOOL main_symbol;
  BOOL second_symbol;
  BOOL addend;
  BOOL pcrel;
  BOOL gprel;
  BOOL gotrel;
  ISA_RELOC_OVERFLOW_TYPE overflow;
  ISA_RELOC_UNDERFLOW_TYPE underflow;
  INT32 right_shift;
  const char *syntax;
  const char *name;
  ISA_VIRTUAL_RELOC virtual_id;
} ISA_RELOC_INFO_pre_20090727;

// -------- Changed at rev 20090915 --------------------------------------------
#define ISA_OPERAND_max_operands_pre_20090915 27
#define ISA_OPERAND_max_results_pre_20090915  26
typedef struct {
  mUINT8  opnds;
  mUINT8  opnd[ISA_OPERAND_max_operands_pre_20090915];
  mUINT16 ouse[ISA_OPERAND_max_operands_pre_20090915];
  mUINT8  results;
  mUINT8  result[ISA_OPERAND_max_results_pre_20090915];
  mUINT16 ruse[ISA_OPERAND_max_results_pre_20090915];
  mINT8   same_res[ISA_OPERAND_max_results_pre_20090915];
  mINT32  conflicts[ISA_OPERAND_max_results_pre_20090915];
} ISA_OPERAND_INFO_pre_20090915;

#define ISA_RELOC_STATIC_MAX_pre_20090915 193
typedef struct {
  mUINT8  rclass;
  mUINT8  rsubclass;
  mUINT8  lclass;
  mUINT8  eclass;
  mUINT16 size;
  mUINT8  flags;
  mUINT8  default_reloc;
  mUINT8  relocs;
  mINT8   reloc[ISA_RELOC_STATIC_MAX_pre_20090915];
} ISA_OPERAND_VALTYP_pre_20090915;

// -------- Changed at rev 20100114 --------------------------------------------
typedef mUINT16 ISA_EXEC_UNIT_PROPERTY_pre_20100114;

// -------- Changed at rev 20100120 --------------------------------------------
#define ISA_RELOC_STATIC_MAX_pre_20100120 193
typedef struct {
  mUINT8  rclass;
  mUINT8  rsubclass;
  mUINT8  lclass;
  mUINT8  eclass;
  mUINT16 size;
  mUINT8  flags;
  mUINT8  default_reloc;
  mUINT8  relocs;
  mINT8   reloc[ISA_RELOC_STATIC_MAX_pre_20100120];
} ISA_OPERAND_VALTYP_pre_20100120;

// #############################################################################
// ##
// ## Class: EXTENSION_Regclass_Info
// ## Descr: Define API used to access some register class specific information
// ##        of the extension
// ##
// #############################################################################
EXTENSION_Regclass_Info::EXTENSION_Regclass_Info() {
  own_rc_info = NULL;
  rc_info     = NULL;
  revision_number = -1;
}

EXTENSION_Regclass_Info::EXTENSION_Regclass_Info(const extension_regclass_t* input_rc_info, INT rev_number) {
  revision_number = rev_number;
  own_rc_info = NULL;
  rc_info     = input_rc_info;
}


EXTENSION_Regclass_Info::EXTENSION_Regclass_Info(const EXTENSION_Regclass_Info &info) {
  own_rc_info = NULL;
  rc_info     = info.rc_info;
  revision_number = info.revision_number;
}

// Destructor
EXTENSION_Regclass_Info::~EXTENSION_Regclass_Info() {
  if (own_rc_info) {
    delete own_rc_info;
  }
}

  // Copy operator
EXTENSION_Regclass_Info&
EXTENSION_Regclass_Info::operator= (const EXTENSION_Regclass_Info &rc_access) {
    rc_info = rc_access.rc_info;
    own_rc_info = NULL;
    revision_number = rc_access.revision_number;
    return (*this);
}


// Memory accesses
TOP
EXTENSION_Regclass_Info::get_load_TOP (INT size, AM_Base_Reg_Type base_reg, BOOL offs_is_imm, BOOL offs_is_incr, INT mpixel_size) const {
  /* revision number before REV_20070924 do support pixel, using the
     older api */
  if (revision_number < REV_20070924) {
    const extension_regclass_t_pre_20070924* rc = 
      (extension_regclass_t_pre_20070924*) rc_info;
    return       (rc->get_load_TOP(size, base_reg, offs_is_imm, offs_is_incr));
  }
  else {
    return       (rc_info->get_load_TOP(size, base_reg, offs_is_imm, offs_is_incr, mpixel_size));
  }
}

TOP
EXTENSION_Regclass_Info::get_store_TOP(INT size, AM_Base_Reg_Type base_reg, BOOL offs_is_imm, BOOL offs_is_incr, INT mpixel_size) const {
  /* revision number before REV_20070924 do support pixel, using the
     older api */
  if (revision_number < REV_20070924) {
    const extension_regclass_t_pre_20070924* rc = 
      (extension_regclass_t_pre_20070924*) rc_info;
    return       (rc->get_store_TOP(size, base_reg, offs_is_imm, offs_is_incr));
  }
  else {
    return       (rc_info->get_store_TOP(size, base_reg, offs_is_imm, offs_is_incr, mpixel_size));
  }
}


// #############################################################################
// ##
// ## Class: EXTENSION_SI_Info
// ## Descr: Define API used to access Scheduling Information (SI) for a single
// ##        a single architecture of the extension
// ##
// #############################################################################
EXTENSION_SI_Info::EXTENSION_SI_Info() {
  own_si_info = NULL;
  si_info     = NULL;
}

EXTENSION_SI_Info::EXTENSION_SI_Info(const extension_si_t* input_si_info) {
  own_si_info = NULL;
  si_info     = input_si_info;
}

EXTENSION_SI_Info::EXTENSION_SI_Info(const EXTENSION_SI_Info &si_access) {
  own_si_info = NULL;
  si_info     = si_access.si_info;
}

// Destructor
EXTENSION_SI_Info::~EXTENSION_SI_Info() {
  if (own_si_info) {
    delete own_si_info;
  }
}


// #############################################################################
// ##
// ## Class: EXTENSION_ISA_Info
// ## Descr: Define API used to access ISA description for given extension
// ##
// #############################################################################
// Constructor
EXTENSION_ISA_Info::EXTENSION_ISA_Info(const ISA_EXT_Interface_t* input_isa_ext) {

  INT i;

  // =====================================================
  // Perform revision migration here
  // =====================================================
  // The input interface might be an older revision of ISA_EXT_Interface_t
  // that must be migrated. However, the magic number will still be accessible
  // at the same location first field of the structure), so it is safe to 
  // first check the magic number then cast the structure to its old type.

  // ISA_EXT_Interface_t has not changed since its revision
  own_isa_ext = NULL;
  isa_ext     = input_isa_ext;
  
  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  //
  // Conversion of ISA_REGISTER_CLASS_INFO structure
  // 
  // Changes are:
  // -----------
  // [1] REV_20070615: Conversion of <bit_size> field from mUINT8 to mUINT16
  //
  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  if (input_isa_ext->magic < REV_20070615) {
    int nb_entry = isa_ext->get_ISA_REGISTER_CLASS_tab_sz();
    ISA_REGISTER_CLASS_INFO_pre_20070615 *old_tab;
    ISA_REGISTER_CLASS_INFO              *new_tab;
    old_tab = (ISA_REGISTER_CLASS_INFO_pre_20070615*)isa_ext->get_ISA_REGISTER_CLASS_tab();
    new_tab = new ISA_REGISTER_CLASS_INFO[nb_entry];
    for (i=0; i<nb_entry; i++) {
      new_tab[i].isa_mask      = old_tab[i].isa_mask;
      new_tab[i].min_regnum    = old_tab[i].min_regnum;
      new_tab[i].max_regnum    = old_tab[i].max_regnum;
      new_tab[i].bit_size      = (mUINT16)old_tab[i].bit_size;  // [1]
      new_tab[i].is_ptr        = old_tab[i].is_ptr;
      new_tab[i].can_store     = old_tab[i].can_store;
      new_tab[i].multiple_save = old_tab[i].multiple_save;
      new_tab[i].name          = old_tab[i].name;
      memcpy(new_tab[i].reg_name, old_tab[i].reg_name, sizeof(const char *)*(ISA_REGISTER_MAX+1));
    }
    overridden_ISA_REGISTER_CLASS_tab = new_tab;
  }
  else {
    overridden_ISA_REGISTER_CLASS_tab = input_isa_ext->get_ISA_REGISTER_CLASS_tab();
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  //
  // Conversion of ISA_OPERAND_VALTYP structure
  //
  // Changes are:
  // -----------
  // [1] REV_20080307: Addition of reloc information fields
  // [2] REV_20081010: Increase of <relocs> size from 58 to 68
  // [3] REV_20090408: Increase of <relocs> size from 68 to 149
  // [4] REV_20090727: Increase of <relocs> size from 149 to 193
  // [5] REV_20090915: Conversion of <lclass> field from 8 to 16 bits
  //                   + Changed field ordering
  // [6] REV_20100120: Increase of <relocs> size from 193 to 307
  //                   + Changed types of default_reloc, relocs and reloc[] fields
  //
  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  if (input_isa_ext->magic < REV_20100120) {
    int nb_entry = isa_ext->get_ISA_OPERAND_operand_types_tab_sz();
    ISA_OPERAND_VALTYP              *new_tab;
    new_tab = new ISA_OPERAND_VALTYP[nb_entry];
    
    if (input_isa_ext->magic < REV_20080307) {
      ISA_OPERAND_VALTYP_pre_20080307 *old_tab;
      old_tab = (ISA_OPERAND_VALTYP_pre_20080307*)isa_ext->get_ISA_OPERAND_operand_types_tab();
      for (i=0; i<nb_entry; i++) {
        int j;
        new_tab[i].rclass        = old_tab[i].rclass;
        new_tab[i].rsubclass     = old_tab[i].rsubclass;
        new_tab[i].lclass        = old_tab[i].lclass;    // [5]
        new_tab[i].eclass        = old_tab[i].eclass;
        new_tab[i].size          = old_tab[i].size;
        new_tab[i].flags         = old_tab[i].flags;
        // Additional fields
        new_tab[i].default_reloc = 0;                    // [1]
        new_tab[i].relocs        = 0;                    // [1]
        for (j=0; j<ISA_RELOC_STATIC_MAX; j++) {         // [1]
          new_tab[i].reloc[j] = ISA_RELOC_UNDEFINED;     // [1]
        }                                                // [1]
      }
    }
    else if (input_isa_ext->magic < REV_20081010) {
      ISA_OPERAND_VALTYP_pre_20081010 *old_tab;
      old_tab = (ISA_OPERAND_VALTYP_pre_20081010*)isa_ext->get_ISA_OPERAND_operand_types_tab();
      for (i=0; i<nb_entry; i++) {
	int j;
	new_tab[i].rclass        = old_tab[i].rclass;
	new_tab[i].rsubclass     = old_tab[i].rsubclass;
	new_tab[i].lclass        = old_tab[i].lclass;    // [5]
	new_tab[i].eclass        = old_tab[i].eclass;
	new_tab[i].size          = old_tab[i].size;
	new_tab[i].flags         = old_tab[i].flags;
	new_tab[i].default_reloc = old_tab[i].default_reloc;
	new_tab[i].relocs        = old_tab[i].relocs;
	for (j=0; j<ISA_RELOC_STATIC_MAX_pre_20081010; j++) { // [2]
	  new_tab[i].reloc[j] = old_tab[i].reloc[j];          // [2]
	}
	for (; j<ISA_RELOC_STATIC_MAX; j++) {
	  new_tab[i].reloc[j] = ISA_RELOC_UNDEFINED;
	}
      }
    }
    else if (input_isa_ext->magic < REV_20090408) {
      int nb_entry = isa_ext->get_ISA_OPERAND_operand_types_tab_sz();
      ISA_OPERAND_VALTYP_pre_20090408 *old_tab;
      old_tab = (ISA_OPERAND_VALTYP_pre_20090408*)isa_ext->get_ISA_OPERAND_operand_types_tab();
      for (i=0; i<nb_entry; i++) {
        int j;
        new_tab[i].rclass        = old_tab[i].rclass;
        new_tab[i].rsubclass     = old_tab[i].rsubclass;
        new_tab[i].lclass        = old_tab[i].lclass;    // [5]
        new_tab[i].eclass        = old_tab[i].eclass;
        new_tab[i].size          = old_tab[i].size;
        new_tab[i].flags         = old_tab[i].flags;
        new_tab[i].default_reloc = old_tab[i].default_reloc;
        new_tab[i].relocs        = old_tab[i].relocs;
        for (j=0; j<ISA_RELOC_STATIC_MAX_pre_20090408; j++) { // [3]
          new_tab[i].reloc[j] = old_tab[i].reloc[j];          // [3]
        }
        for (; j<ISA_RELOC_STATIC_MAX; j++) {
          new_tab[i].reloc[j] = ISA_RELOC_UNDEFINED;
        }
      }
    }
    else if (input_isa_ext->magic < REV_20090727) {
      ISA_OPERAND_VALTYP_pre_20090727 *old_tab;
      old_tab = (ISA_OPERAND_VALTYP_pre_20090727*)isa_ext->get_ISA_OPERAND_operand_types_tab();
      for (i=0; i<nb_entry; i++) {
        int j;
        new_tab[i].rclass        = old_tab[i].rclass;
        new_tab[i].rsubclass     = old_tab[i].rsubclass;
        new_tab[i].lclass        = old_tab[i].lclass;    // [5]
        new_tab[i].eclass        = old_tab[i].eclass;
        new_tab[i].size          = old_tab[i].size;
        new_tab[i].flags         = old_tab[i].flags;
        new_tab[i].default_reloc = old_tab[i].default_reloc;
        new_tab[i].relocs        = old_tab[i].relocs;
        for (j=0; j<ISA_RELOC_STATIC_MAX_pre_20090727; j++) { // [4]
          new_tab[i].reloc[j] = old_tab[i].reloc[j];          // [4]
        }
        for (; j<ISA_RELOC_STATIC_MAX; j++) {
          new_tab[i].reloc[j] = ISA_RELOC_UNDEFINED;
        }
      }
    }
    else if (input_isa_ext->magic < REV_20090915) {
      int nb_entry = isa_ext->get_ISA_OPERAND_operand_types_tab_sz();
      ISA_OPERAND_VALTYP_pre_20090915 *old_tab;
      old_tab = (ISA_OPERAND_VALTYP_pre_20090915*)isa_ext->get_ISA_OPERAND_operand_types_tab();
      for (i=0; i<nb_entry; i++) {
        int j;
        new_tab[i].rclass        = old_tab[i].rclass;
        new_tab[i].rsubclass     = old_tab[i].rsubclass;
        new_tab[i].lclass        = old_tab[i].lclass;    // [5]
        new_tab[i].eclass        = old_tab[i].eclass;
        new_tab[i].size          = old_tab[i].size;
        new_tab[i].flags         = old_tab[i].flags;
        new_tab[i].default_reloc = old_tab[i].default_reloc;
        new_tab[i].relocs        = old_tab[i].relocs;
        for (j=0; j<ISA_RELOC_STATIC_MAX_pre_20090915; j++) {
          new_tab[i].reloc[j] = old_tab[i].reloc[j];
        }
        for (; j<ISA_RELOC_STATIC_MAX; j++) {
          new_tab[i].reloc[j] = ISA_RELOC_UNDEFINED;
        }
      }
    }
    else if (input_isa_ext->magic < REV_20100120) {
      int nb_entry = isa_ext->get_ISA_OPERAND_operand_types_tab_sz();
      ISA_OPERAND_VALTYP_pre_20100120 *old_tab;
      old_tab = (ISA_OPERAND_VALTYP_pre_20100120*)isa_ext->get_ISA_OPERAND_operand_types_tab();
      for (i=0; i<nb_entry; i++) {
        int j;
        new_tab[i].rclass        = old_tab[i].rclass;
        new_tab[i].rsubclass     = old_tab[i].rsubclass;
        new_tab[i].lclass        = old_tab[i].lclass;    // [5]
        new_tab[i].eclass        = old_tab[i].eclass;
        new_tab[i].size          = old_tab[i].size;
        new_tab[i].flags         = old_tab[i].flags;
        new_tab[i].default_reloc = old_tab[i].default_reloc;  // [6]
        new_tab[i].relocs        = old_tab[i].relocs;         // [6]
        for (j=0; j<ISA_RELOC_STATIC_MAX_pre_20100120; j++) { // [6]
          new_tab[i].reloc[j] = old_tab[i].reloc[j];          // [6]
        }
        for (; j<ISA_RELOC_STATIC_MAX; j++) {
          new_tab[i].reloc[j] = ISA_RELOC_UNDEFINED;
        }
      }
    }
    overridden_ISA_OPERAND_operand_types_tab = new_tab;
  }
  else {
    overridden_ISA_OPERAND_operand_types_tab = input_isa_ext->get_ISA_OPERAND_operand_types_tab();
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  //
  // Conversion of ISA_LIT_CLASS_INFO structure
  //
  // Changes are:
  // -----------
  // [1] REV_20080307: Addition of <is_negative> field
  // [2] REV_20090126: Addition of <max_right_rotate> and
  //                               <right_rotate_mask> fields
  //
  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  if (input_isa_ext->magic < REV_20090126) {
    int nb_entry = isa_ext->get_ISA_LIT_CLASS_info_tab_sz();
    ISA_LIT_CLASS_INFO *new_tab;
    new_tab = new ISA_LIT_CLASS_INFO[nb_entry];
    
    if (input_isa_ext->magic < REV_20080307) {
      int j;
      ISA_LIT_CLASS_INFO_pre_20080307 *old_tab;
      old_tab = (ISA_LIT_CLASS_INFO_pre_20080307*)isa_ext->get_ISA_LIT_CLASS_info_tab();
      for (i=0; i<nb_entry; i++) {
	for (j=0; j<=old_tab[i].num_ranges; j++) {
	  new_tab[i].range[j].min           = old_tab[i].range[j].min;
	  new_tab[i].range[j].max           = old_tab[i].range[j].max;
	  new_tab[i].range[j].scaling_value = old_tab[i].range[j].scaling_value;
	  new_tab[i].range[j].scaling_mask  = old_tab[i].range[j].scaling_mask;
	  new_tab[i].range[j].max_right_rotate = 0;  // [2]
	  new_tab[i].range[j].right_rotate_mask = 0; // [2]
	}
	new_tab[i].num_ranges   = old_tab[i].num_ranges;
	new_tab[i].is_signed    = old_tab[i].is_signed;
	new_tab[i].is_negative  = 0;               // [1]
	new_tab[i].name         = old_tab[i].name;
      }
    }
    else if (input_isa_ext->magic < REV_20090126) {
      int j;
      ISA_LIT_CLASS_INFO_pre_20090126 *old_tab;
      old_tab = (ISA_LIT_CLASS_INFO_pre_20090126*)isa_ext->get_ISA_LIT_CLASS_info_tab();
      for (i=0; i<nb_entry; i++) {
	for (j=0; j<=old_tab[i].num_ranges; j++) {
	  new_tab[i].range[j].min           = old_tab[i].range[j].min;
	  new_tab[i].range[j].max           = old_tab[i].range[j].max;
	  new_tab[i].range[j].scaling_value = old_tab[i].range[j].scaling_value;
	  new_tab[i].range[j].scaling_mask  = old_tab[i].range[j].scaling_mask;
	  new_tab[i].range[j].max_right_rotate = 0;  // [2]
	  new_tab[i].range[j].right_rotate_mask = 0; // [2]
	}
	new_tab[i].num_ranges   = old_tab[i].num_ranges;
	new_tab[i].is_signed    = old_tab[i].is_signed;
	new_tab[i].is_negative  = old_tab[i].is_negative;
	new_tab[i].name         = old_tab[i].name;
      }
    }
    overridden_ISA_LIT_CLASS_info_tab = new_tab;
  }
  else {
    overridden_ISA_LIT_CLASS_info_tab = input_isa_ext->get_ISA_LIT_CLASS_info_tab();
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  //
  // Creation of the following bundle related structures:
  //  - ISA_EXEC_UNIT_PROPERTY
  //  - ISA_EXEC_UNIT_SLOTS
  //  - ISA_BUNDLE_slot
  //  - ISA_PRINT_INFO
  //
  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  if (input_isa_ext->magic < REV_20080307) {
    {
      // Convert ISA_EXEC_UNIT_PROPERTY (bundling)
      int nb_entry = isa_ext->get_TOP_count();
      int ext_num_subsets = isa_ext->get_ISA_SUBSET_tab_sz();
      ISA_EXEC_UNIT_PROPERTY_pre_20080307 *old_tab;
      ISA_EXEC_UNIT_PROPERTY              *new_tab;
      ISA_EXEC_UNIT_SLOTS                 *new_slots_tab;
      mUINT8                              *new_bundle_slot_count_tab;
      old_tab = (ISA_EXEC_UNIT_PROPERTY_pre_20080307*)isa_ext->get_ISA_EXEC_unit_prop_tab();
      new_tab = new ISA_EXEC_UNIT_PROPERTY[nb_entry];
      new_slots_tab = new ISA_EXEC_UNIT_SLOTS[nb_entry];
      new_bundle_slot_count_tab = new mUINT8[ext_num_subsets];
      memset(new_bundle_slot_count_tab,1,ext_num_subsets*sizeof(mUINT8));
      for (i=0; i<nb_entry; i++) {
	new_tab[i] = (mUINT32)old_tab[i];
	// For STxP70 v3 number of slots is 1. V4 or other targets are
	// described using new targinfo.
	new_slots_tab[i] = 1;
      }
      overridden_ISA_EXEC_unit_prop_tab = new_tab;
      overridden_ISA_EXEC_unit_slots_tab = new_slots_tab;
      overridden_ISA_BUNDLE_slot_count_tab = new_bundle_slot_count_tab;
    }
    {
      // Convert ISA_PRINT_INFO
      int nb_entry = isa_ext->get_TOP_count();
      ISA_PRINT_INFO  *new_tab;
      new_tab = new ISA_PRINT_INFO[nb_entry];
      for (i=0; i<nb_entry; i++) {
	new_tab[i] = Print_Extension_Instr_pre_20080307;
      }
      overridden_ISA_PRINT_info_tab = new_tab;
    }
  } else if (input_isa_ext->magic < REV_20100114 ) {
      {
      // Convert ISA_EXEC_UNIT_PROPERTY (bundling)
      int nb_entry = isa_ext->get_TOP_count();

      ISA_EXEC_UNIT_PROPERTY_pre_20100114 *old_tab;
      ISA_EXEC_UNIT_PROPERTY              *new_tab;

      old_tab = (ISA_EXEC_UNIT_PROPERTY_pre_20100114*)isa_ext->get_ISA_EXEC_unit_prop_tab();
      new_tab = new ISA_EXEC_UNIT_PROPERTY[nb_entry];

      for (i=0; i<nb_entry; i++) {
	new_tab[i] = (mUINT32)old_tab[i];
      }
      overridden_ISA_EXEC_unit_prop_tab = new_tab;

      overridden_ISA_EXEC_unit_slots_tab = input_isa_ext->get_ISA_EXEC_unit_slots_tab();
      overridden_ISA_BUNDLE_slot_count_tab = input_isa_ext->get_ISA_BUNDLE_slot_count_tab();
      overridden_ISA_PRINT_info_tab = input_isa_ext->get_ISA_PRINT_info_tab();

    }
  }
  else {

    overridden_ISA_EXEC_unit_prop_tab = input_isa_ext->get_ISA_EXEC_unit_prop_tab();
    overridden_ISA_EXEC_unit_slots_tab = input_isa_ext->get_ISA_EXEC_unit_slots_tab();
    overridden_ISA_BUNDLE_slot_count_tab = input_isa_ext->get_ISA_BUNDLE_slot_count_tab();
    overridden_ISA_PRINT_info_tab = input_isa_ext->get_ISA_PRINT_info_tab();
  }

  FmtAssert((sizeof(ISA_EXEC_UNIT_PROPERTY)==4), ("Internal Compiler Error : ISA_EXEC_UNIT_PROPERTY size no equal to 32bits\n"));


  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  //
  // Conversion of ISA_RELOC / ISA_PACK / ISA_PARSE / ISA_DECODE structures
  //
  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  if (input_isa_ext->magic < REV_20081010) {
     static ISA_RELOC_INFO ISA_RELOC_dynamic_info [] = {
       { { {   0,  0,  0, }, }, 0, 0, 0, 0, 0, 0, 0, ISA_RELOC_NO_OVERFLOW, ISA_RELOC_NO_UNDERFLOW, 0, "",  "RELOC_UNDEFINED", 0 }
     };
     static mUINT32 dyn_reloc_subset_tab_0 [] = { 0 /* UNDEFINED       */ };
     static ISA_RELOC_SUBSET_INFO dyn_reloc_subset_tab [1] = { {   1,   0, dyn_reloc_subset_tab_0  } };
     static ISA_RELOC_VARIANT_INFO ISA_RELOC_dynamic_variant_info [] = {
       { ISA_RELOC_UNDEFINED, ISA_RELOC_UNDEFINED }
     };

    /* Setup default functions */
    overridden_ISA_PARSE_tab                   = NULL;
    overridden_ISA_PARSE_tab_sz                = 0;
    overridden_ISA_PACK_OPND_info_tab          = NULL;
    overridden_ISA_PACK_OPND_info_tab_sz       = 0;
    overridden_ISA_PACK_OPND_info_index_tab    = NULL;
    overridden_ISA_PACK_info_tab               = NULL;
    overridden_ISA_PACK_inst_words_tab         = NULL;
    overridden_ISA_PACK_adj_info_tab           = NULL;
    overridden_ISA_PACK_adj_info_tab_sz        = 0;
    overridden_ISA_PACK_adj_info_index_tab     = NULL;
    overridden_ISA_DECODE_decoding_tab         = NULL;
    overridden_ISA_DECODE_decoding_tab_sz      = 0;
    overridden_ISA_RELOC_info_tab              = ISA_RELOC_dynamic_info;
    overridden_ISA_RELOC_info_tab_sz           = 1;
    overridden_ISA_RELOC_SUBSET_info_tab       = dyn_reloc_subset_tab;
    overridden_ISA_RELOC_SUBSET_info_tab_sz    = 1;
    overridden_ISA_RELOC_max_static_virtual_id_core_subset = ISA_SUBSET_MIN;
    overridden_ISA_BINUTILS_info_tab           = NULL;
    overridden_ISA_BINUTILS_info_tab_sz        = 0;
    overridden_ISA_RELOC_variant_info_tab      = ISA_RELOC_dynamic_variant_info;
    overridden_ISA_RELOC_variant_info_tab_sz   = 1;
  } else {
    //
    // -------- Changed after rev 20090727 -----------------------------------------
    // Initializing new ISA_RELOC_RLLIB_MODE field for extension earlier than dynamic 
    // loading introduction
    if (input_isa_ext->magic < REV_20090727) {
      int nb_entry = isa_ext->get_ISA_RELOC_info_tab_sz();
      ISA_RELOC_INFO_pre_20090727 *old_tab;
      ISA_RELOC_INFO              *new_tab;
      
      old_tab = (ISA_RELOC_INFO_pre_20090727*)isa_ext->get_ISA_RELOC_info_tab();
      new_tab = new ISA_RELOC_INFO[nb_entry];
      for (i=0; i<nb_entry; i++) {
        int j;
        
        for (j=0;j<MAX_BITFIELDS_STATIC;j++) {
          new_tab[i].bitfield[j].elf_id    = old_tab[i].bitfield[j].elf_id; 
          new_tab[i].bitfield[j].start_bit = old_tab[i].bitfield[j].start_bit;
          new_tab[i].bitfield[j].stop_bit  = old_tab[i].bitfield[j].stop_bit;
        }
        new_tab[i].bitfields     = old_tab[i].bitfields;
        new_tab[i].main_symbol   = old_tab[i].main_symbol;
        new_tab[i].second_symbol = old_tab[i].second_symbol;
        new_tab[i].addend        = old_tab[i].addend;
        new_tab[i].pcrel         = old_tab[i].pcrel;
        new_tab[i].gprel         = old_tab[i].gprel;
        new_tab[i].gotrel        = old_tab[i].gotrel;
        new_tab[i].overflow      = old_tab[i].overflow;
        new_tab[i].underflow     = old_tab[i].underflow;
        new_tab[i].right_shift   = old_tab[i].right_shift;
        new_tab[i].syntax        = old_tab[i].syntax;
        new_tab[i].name          = old_tab[i].name;
        new_tab[i].virtual_id    = old_tab[i].virtual_id;
        new_tab[i].rllib_mode    = ISA_RELOC_NO_RLLIB;
      }
      overridden_ISA_RELOC_info_tab = new_tab;
    }
    else {
      overridden_ISA_RELOC_info_tab = isa_ext->get_ISA_RELOC_info_tab();
    }

    //
    // -------- Changed after rev 20100120 -----------------------------------------
    // Introduce ISA_RELOC_VARIANT_INFO to deal with multiple relocation for the
    // same instruction when relaxation is activated. 
    if (input_isa_ext->magic < REV_20100120) {
      int nb_entry = isa_ext->get_ISA_RELOC_info_tab_sz();
      ISA_RELOC_VARIANT_INFO * ISA_RELOC_dynamic_variant_info;
      
      ISA_RELOC_dynamic_variant_info = new ISA_RELOC_VARIANT_INFO[nb_entry];
      for (i=0;i<nb_entry;i++) {
         ISA_RELOC_dynamic_variant_info->reloc_next_encoding = ISA_RELOC_UNDEFINED;
         ISA_RELOC_dynamic_variant_info->reloc_prev_encoding = ISA_RELOC_UNDEFINED;
      }
      overridden_ISA_RELOC_variant_info_tab    = ISA_RELOC_dynamic_variant_info;
      overridden_ISA_RELOC_variant_info_tab_sz = nb_entry;
    } else {
      overridden_ISA_RELOC_variant_info_tab    = isa_ext->get_ISA_RELOC_variant_info_tab();
      overridden_ISA_RELOC_variant_info_tab_sz = isa_ext->get_ISA_RELOC_variant_info_tab_sz();
    }
    
    overridden_ISA_PARSE_tab                   = isa_ext->get_ISA_PARSE_tab();
    overridden_ISA_PARSE_tab_sz                = isa_ext->get_ISA_PARSE_tab_sz();
    overridden_ISA_PACK_OPND_info_tab          = isa_ext->get_ISA_PACK_OPND_info_tab();
    overridden_ISA_PACK_OPND_info_tab_sz       = isa_ext->get_ISA_PACK_OPND_info_tab_sz();
    overridden_ISA_PACK_OPND_info_index_tab    = isa_ext->get_ISA_PACK_OPND_info_index_tab();
    overridden_ISA_PACK_info_tab               = isa_ext->get_ISA_PACK_info_tab();
    overridden_ISA_PACK_inst_words_tab         = isa_ext->get_ISA_PACK_inst_words_tab();
    overridden_ISA_PACK_adj_info_tab           = isa_ext->get_ISA_PACK_adj_info_tab();
    overridden_ISA_PACK_adj_info_tab_sz        = isa_ext->get_ISA_PACK_adj_info_tab_sz();
    overridden_ISA_PACK_adj_info_index_tab     = isa_ext->get_ISA_PACK_adj_info_index_tab();
    overridden_ISA_DECODE_decoding_tab         = isa_ext->get_ISA_DECODE_decoding_tab();
    overridden_ISA_DECODE_decoding_tab_sz      = isa_ext->get_ISA_DECODE_decoding_tab_sz();
    overridden_ISA_RELOC_info_tab_sz           = isa_ext->get_ISA_RELOC_info_tab_sz();
    overridden_ISA_RELOC_SUBSET_info_tab       = isa_ext->get_ISA_RELOC_SUBSET_info_tab();
    overridden_ISA_RELOC_SUBSET_info_tab_sz    = isa_ext->get_ISA_RELOC_SUBSET_info_tab_sz();
    overridden_ISA_RELOC_max_static_virtual_id_core_subset = isa_ext->get_ISA_RELOC_max_static_virtual_id_core_subset();
    overridden_ISA_BINUTILS_info_tab           = isa_ext->get_ISA_BINUTILS_info_tab();
    overridden_ISA_BINUTILS_info_tab_sz        = isa_ext->get_ISA_BINUTILS_info_tab_sz();
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  //
  // Conversion of ISA_OPERAND_INFO structure
  //
  // Changes are:
  // -----------
  // [1] REV_20090126: Addition of <conflicts> array
  // [2] REV_20090915: Arrays are no more embedded in this structure.
  //                   They have been replaced by pointers.
  // [3] REV_20090915: Elements of <opnd> and <result> arrays are now mUINT16
  //                   (used to be mUINT8)
  //
  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------
  if (input_isa_ext->magic < REV_20090915) {
    int j;
    int nb_entry = isa_ext->get_ISA_OPERAND_info_tab_sz();
    ISA_OPERAND_INFO              *new_tab;
    new_tab = new ISA_OPERAND_INFO[nb_entry];

    if (input_isa_ext->magic < REV_20090126) {
      ISA_OPERAND_INFO_pre_20090126 *old_tab;
      old_tab = (ISA_OPERAND_INFO_pre_20090126*)isa_ext->get_ISA_OPERAND_info_tab();
      for (i=0; i<nb_entry; i++) {
        new_tab[i].opnds     = old_tab[i].opnds;
        new_tab[i].ouse      = old_tab[i].ouse;     // [2] copy array address
        new_tab[i].results   = old_tab[i].results;
        new_tab[i].ruse      = old_tab[i].ruse;     // [2] copy array address
        new_tab[i].same_res  = old_tab[i].same_res; // [2] copy array address
        // Generate additional array (if required)
        if (old_tab[i].results > 0) {
          new_tab[i].conflicts = new mINT32 [old_tab[i].results];
          new_tab[i].result    = new mUINT16[old_tab[i].results];
          for (j=0; j<old_tab[i].results; j++) {
            new_tab[i].conflicts[j] = 0;  // [1]
            new_tab[i].result[j] = (mUINT16)old_tab[i].result[j]; // [3]
          }
        }
        else {
          new_tab[i].conflicts = NULL;
          new_tab[i].result = NULL;
        }
        if (old_tab[i].opnds > 0) {
          new_tab[i].opnd = new mUINT16[old_tab[i].opnds];
          for (j=0; j<old_tab[i].opnds; j++) {
            new_tab[i].opnd[j] = (mUINT16)old_tab[i].opnd[j]; // [3]
          }
        }
        else {
          new_tab[i].opnd = NULL;
        }
      }
    }
    else {
      ISA_OPERAND_INFO_pre_20090915 *old_tab;
      old_tab = (ISA_OPERAND_INFO_pre_20090915*)isa_ext->get_ISA_OPERAND_info_tab();
      for (i=0; i<nb_entry; i++) {
        new_tab[i].opnds     = old_tab[i].opnds;
        new_tab[i].ouse      = old_tab[i].ouse;      // [2] copy array address
        new_tab[i].results   = old_tab[i].results;
        new_tab[i].ruse      = old_tab[i].ruse;      // [2] copy array address
        new_tab[i].same_res  = old_tab[i].same_res;  // [2] copy array address
        new_tab[i].conflicts = old_tab[i].conflicts; // [2] copy array address
        // Generate additional array (if required)
        if (old_tab[i].results > 0) {
          new_tab[i].result = new mUINT16[old_tab[i].results];
          for (j=0; j<old_tab[i].results; j++) {
            new_tab[i].result[j] = (mUINT16)old_tab[i].result[j]; // [3]
          }
        }
        else {
          new_tab[i].result = NULL;
        }
        if (old_tab[i].opnds > 0) {
          new_tab[i].opnd = new mUINT16[old_tab[i].opnds];
          for (j=0; j<old_tab[i].opnds; j++) {
            new_tab[i].opnd[j] = (mUINT16)old_tab[i].opnd[j]; // [3]
          }
        }
        else {
          new_tab[i].opnd = NULL;
        }
      }
    }
    overridden_ISA_OPERAND_info_tab = new_tab;
  }
  else {
    overridden_ISA_OPERAND_info_tab = input_isa_ext->get_ISA_OPERAND_info_tab();
  }

  //
  // Create REGISTER CLASS Info wrappers (register class to TOP)
  //
  INT nb_rc;
  nb_rc = get_ISA_REGISTER_CLASS_tab_sz();
  const extension_regclass_t *rc_tab = get_REGISTER_CLASS_info_tab();
  regclass_access_tab = new EXTENSION_Regclass_Info[nb_rc];
  for (i=0; i<nb_rc; i++) {
    regclass_access_tab[i] = EXTENSION_Regclass_Info(&rc_tab[i],
                                                     input_isa_ext->magic);
  }

  //
  // Create Scheduling Info wrappers
  //
  INT nb_si;
  nb_si = get_scheduling_info_tab_sz();
  const extension_si_t *si_tab = get_scheduling_info_tab();
  si_access_tab = new EXTENSION_SI_Info[nb_si];
  for (i=0; i<nb_si; i++) {
    si_access_tab[i] = EXTENSION_SI_Info(&si_tab[i]);
  }
}

//Destructor
EXTENSION_ISA_Info::~EXTENSION_ISA_Info() {
  INT initial_rev;
  initial_rev = get_initial_revision();
  
  if (initial_rev < MAGIC_NUMBER_EXT_ISA_API) {
    delete own_isa_ext;
    if (initial_rev < REV_20070615) {
      delete[] (ISA_REGISTER_CLASS_INFO*)overridden_ISA_REGISTER_CLASS_tab;
    }
    if (initial_rev < REV_20080307) {
      delete[] (ISA_EXEC_UNIT_SLOTS*)overridden_ISA_EXEC_unit_slots_tab;
      delete[] (mUINT8*)overridden_ISA_BUNDLE_slot_count_tab;
      delete[] (ISA_PRINT_INFO*)overridden_ISA_PRINT_info_tab;
    }
    if(initial_rev < REV_20090126) {
      delete[] (ISA_LIT_CLASS_INFO*)overridden_ISA_LIT_CLASS_info_tab;
      delete[] (ISA_OPERAND_INFO*)overridden_ISA_OPERAND_info_tab;
    }
    if (initial_rev < REV_20090915) {
      delete[] (ISA_OPERAND_VALTYP*)overridden_ISA_OPERAND_operand_types_tab;
    }
    if (initial_rev < REV_20100114) {
      delete[] (ISA_EXEC_UNIT_PROPERTY*)overridden_ISA_EXEC_unit_prop_tab;      
    }
  }
  delete[] regclass_access_tab;
  delete[] si_access_tab;
}

const void*
EXTENSION_ISA_Info::get_ISA_PRINT_info_pre_20080307_tab() const {
  if (isa_ext->magic < REV_20080307) {
    return (isa_ext->get_ISA_PRINT_info_tab());
  }
  else {
    // Data not available for newer revision
    return (NULL);
  }
}

void
EXTENSION_ISA_Info::set_ISA_RELOC_dynamic_reloc_offset  (mUINT32 offset) const {
  if (isa_ext->magic >= REV_20081010) {
     isa_ext->set_ISA_RELOC_dynamic_reloc_offset(offset);
  }
}

/* Introduced at REV_20090416 */
void 
EXTENSION_ISA_Info::set_ISA_LIT_CLASS_offset (mUINT32 offs) const {
  if (isa_ext->magic >= REV_20090416) {
     isa_ext->set_ISA_LIT_CLASS_offset(offs);
  }
}

void 
EXTENSION_ISA_Info::set_ISA_REGISTER_CLASS_offset (mUINT32 offs) const {
  if (isa_ext->magic >= REV_20090416) {
     isa_ext->set_ISA_REGISTER_CLASS_offset(offs);
  }
}
 
void
EXTENSION_ISA_Info::set_ISA_REGISTER_SUBCLASS_offset (mUINT32 offs) const {
  if (isa_ext->magic >= REV_20090416) {
     isa_ext->set_ISA_REGISTER_SUBCLASS_offset(offs);
  }
}

