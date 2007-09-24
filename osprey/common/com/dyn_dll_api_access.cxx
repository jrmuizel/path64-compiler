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
 * This modules defines the common API used to access the high level description of extensions.
 * It is compatible with the successives library API revisions and insure the migration
 * to the latest one, hiding the implemention details to the other parts of thecompiler.
 *
 * Changes between revisions:
 * +----------+----------------------------------------------------------------+
 * | Revision | Description                                                    |
 * +==========+================================================================+
 * | 20070131 |   Initial state                                                |
 * +----------+----------------------------------------------------------------+
 * | 20070615 | - No change                                                    |
 * |          |   (The id should not have been updated!!)                      |
 * +----------+----------------------------------------------------------------+
 *
 */
#include "../gccfe/extension_include.h"
#include "dyn_dll_api_access.h"

// ========================================================================
// List of compatible API revisions for ISA part of the library description
// ========================================================================
#define    NB_SUPPORTED_HL_REV  2
#define    REV_20070131        (20070131)
#define    REV_20070615        (20070615)
static INT supported_HL_rev_tab[NB_SUPPORTED_HL_REV] = {
  REV_20070131,
  MAGIC_NUMBER_EXT_API   /* current one */
};


// Return TRUE if the specified revision is compatible with current compiler,
// FALSE otherwise.
BOOL EXTENSION_Is_Supported_HighLevel_Revision(INT hooks_rev) {
  INT i;
  for (i=0; i<NB_SUPPORTED_HL_REV; i++) {
    if (hooks_rev == supported_HL_rev_tab[i]) {
      return TRUE;
    }
  }
  return FALSE;
}

// #############################################################################
// ##
// ## Versionned structures that might need to be migrated
// ##
// #############################################################################

// --- Currently empty ---


// #############################################################################
// ##
// ## Class: EXTENSION_HighLevel_Info
// ## Descr: Define API used to access high level information (types and builtins)
// ##        of the extension
// ##
// #############################################################################
EXTENSION_HighLevel_Info::EXTENSION_HighLevel_Info(const extension_hooks *input_hooks) {
  own_hooks = FALSE;
  hooks = input_hooks;

  // =====================================================
  // Perform revision migration here
  // =====================================================
}

// Destructor
EXTENSION_HighLevel_Info::~EXTENSION_HighLevel_Info() {
  if (own_hooks) {
    delete hooks;
  }
}

