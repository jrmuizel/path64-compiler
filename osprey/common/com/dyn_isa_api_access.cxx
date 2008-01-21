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
 *
 */
#include <stdlib.h>
#include <string.h>
#include "dyn_isa_api_access.h"

// ========================================================================
// List of compatible API revisions for ISA part of the library description
// ========================================================================
#define    NB_SUPPORTED_ISA_REV 3
#define    REV_20070126        (20070126)
#define    REV_20070615        (20070615)
#define    REV_20070924        (20070924)
static INT supported_ISA_rev_tab[NB_SUPPORTED_ISA_REV] = {
  REV_20070126,
  REV_20070615,
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
} ISA_REGISTER_CLASS_INFO_pre_20070126;


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
} extension_regclass_t_pre_20070615;



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
    const extension_regclass_t_pre_20070615* rc = 
      (extension_regclass_t_pre_20070615*) rc_info;
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
    const extension_regclass_t_pre_20070615* rc = 
      (extension_regclass_t_pre_20070615*) rc_info;
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

  // The input interface might be an older revision of ISA_EXT_Interface_t
  // that must be migrated. However, the magic number will still be accessible
  // at the same location first field of the structure), so it is safe to 
  // to first check the magic number then cast the structure to its old type.
  if (input_isa_ext->magic == MAGIC_NUMBER_EXT_ISA_API) {
    // Extension library uses latest API version
    own_isa_ext = NULL;
    isa_ext     = input_isa_ext;

    overridden_ISA_REGISTER_CLASS_tab = input_isa_ext->get_ISA_REGISTER_CLASS_tab();
  }
  else {
    // =====================================================
    // Perform revision migration here
    // =====================================================
    if (input_isa_ext->magic == REV_20070126) {
      // ISA_EXT_Interface_t has not changed since its revision
      own_isa_ext = NULL;
      isa_ext     = input_isa_ext;

      // Convert ISA_REGISTER_CLASS_INFO
      int nb_entry = isa_ext->get_ISA_REGISTER_CLASS_tab_sz();
      ISA_REGISTER_CLASS_INFO_pre_20070126 *old_tab;
      ISA_REGISTER_CLASS_INFO              *new_tab;
      old_tab = (ISA_REGISTER_CLASS_INFO_pre_20070126*)isa_ext->get_ISA_REGISTER_CLASS_tab();
      new_tab = new ISA_REGISTER_CLASS_INFO[nb_entry];
      for (i=0; i<nb_entry; i++) {
	new_tab[i].isa_mask      = old_tab[i].isa_mask;
	new_tab[i].min_regnum    = old_tab[i].min_regnum;
	new_tab[i].max_regnum    = old_tab[i].max_regnum;
	new_tab[i].bit_size      = (mUINT16)old_tab[i].bit_size;
	new_tab[i].is_ptr        = old_tab[i].is_ptr;
	new_tab[i].can_store     = old_tab[i].can_store;
	new_tab[i].multiple_save = old_tab[i].multiple_save;
	new_tab[i].name          = old_tab[i].name;
	memcpy(new_tab[i].reg_name, old_tab[i].reg_name, sizeof(const char *)*(ISA_REGISTER_MAX+1));
      }
      overridden_ISA_REGISTER_CLASS_tab = new_tab;
    }
  }

  // Create REGISTER CLASS Info wrappers (register class to TOP)
  INT nb_rc;
  nb_rc = get_ISA_REGISTER_CLASS_tab_sz();
  const extension_regclass_t *rc_tab = get_REGISTER_CLASS_info_tab();
  regclass_access_tab = new EXTENSION_Regclass_Info[nb_rc];
  for (i=0; i<nb_rc; i++) {
    regclass_access_tab[i] = EXTENSION_Regclass_Info(&rc_tab[i],
                                                     input_isa_ext->magic);
  }

  // Create Scheduling Info wrappers
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
  }
  delete[] regclass_access_tab;
  delete[] si_access_tab;
}

