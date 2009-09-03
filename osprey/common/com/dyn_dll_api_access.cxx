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
 * | 20070924 | - pixel support                                                |
 * +----------+----------------------------------------------------------------+
 * | 20080715 | - extension codegen support                                    |
 * +----------+----------------------------------------------------------------+
 * | 20090410 | - Added support of pointer type machine mode (PSI) for         |
 * |          |   builtins parameters (previously, pointer parameters were     |
 * |          |   specified as INT, leading to too minimal type checking in FE)|
 * |          |   Note: no real API change for this revision but useful to     |
 * |          |         check that older compiler not used with recent RTK     |
 * +----------+----------------------------------------------------------------+
 * | 20090813 | - Introduce pattern recognition rule definitions inside
 * |          |   the extension dll.
 * +----------+----------------------------------------------------------------+
*
 */
#include "../gccfe/extension_include.h"
#include "dyn_dll_api_access.h"

// ========================================================================
// List of compatible API revisions for high level part of the library
// description
// ========================================================================
#define    NB_SUPPORTED_HL_REV  6
#define    REV_20070131        (20070131)
#define    REV_20070615        (20070615)
#define    REV_20070924        (20070924)
#define    REV_20080715        (20080715)
#define    REV_20090410        (20090410)
#define    REV_20090813        (20090813)
static INT supported_HL_rev_tab[NB_SUPPORTED_HL_REV] = {
  REV_20070131,
  REV_20070615,
  REV_20070924,
  REV_20080715,
  REV_20090410,
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

typedef struct
{
  machine_mode_t mmode; 
  const char *name; 
  enum mode_class mclass;
  unsigned short mbitsize; 
  unsigned char msize;
  unsigned char munitsize;
  unsigned char mwidermode; 
  machine_mode_t innermode;
  TYPE_ID mtype;
  unsigned short alignment;
  int local_REGISTER_CLASS_id;
  int local_REGISTER_SUBCLASS_id;
} extension_machine_types_t_pre_20070615;

struct extension_builtins_pre_20080715
{
  enum built_in_function 		gcc_builtin_def;
  INTRINSIC	open64_intrincic;

  // Builtins flags follow the WHIRL semantic (opposed to the gcc one
  // for has_no_side_effects and is_pure)
  // has_no_side_effects means is_pure + no access to memory
  char		is_by_val;
  char		is_pure;
  char		has_no_side_effects;
  char		never_returns;
  char		is_actual;
  char		is_cg_intrinsic;
  const char   *c_name;
  const char   *runtime_name;

  /* Prototype information   */
  machine_mode_t             return_type;
  unsigned char	             arg_count;
  const machine_mode_t 	    *arg_type;     /* Number of items in table: arg_count */
  const BUILTARG_INOUT_TYPE *arg_inout;    /* Number of items in table: arg_count */

  // targ_info information
  int type; // standard TOP intrinsic or compose/extract
  union {
    int		 local_TOP_id;        // TOP id if TOP intrinsic
    int		 compose_extract_idx; // subpart access index if compose/extract
  } u1;
};
  
typedef struct extension_builtins_pre_20080715 extension_builtins_t_pre_20080715;

#ifdef TARG_STxP70
// bwd compatibility
#include "../../targinfo/stxp70/be/extension_compatibility.inc"
#endif

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

  if ( hooks->magic < REV_20090813 ) { /* any version older than
                                          REV_20090813 */
    overriden_recrules = NULL;
    overriden_recrules_count = 0;

#ifdef TARG_STxP70

    if ( hooks->magic >= REV_20080715) { /* only MP1x with api
                                            version more recent than
                                            20080715 are concerned */ 
      // backward compatibility
      /* as the extension name is not accessible in the hooks,
       * I check MP1x on the first builtin name
       */
      if (input_hooks->get_builtins_count()>0) {
        const char* name = input_hooks->get_builtins()[0].c_name;
        
        
        if (strstr(name, "MP1x")!=NULL) {
          // backward compatiblity. use static rule table.
          overriden_recrules = mpx_recog_rules; 
          overriden_recrules_count = mpx_recog_rules_count;
        }
      }
    }
#endif
    
  } else {
    overriden_recrules = hooks->get_recrules();
    overriden_recrules_count = hooks->get_recrules_count();
  }


  if ( hooks->magic < REV_20080715 ) {  /* any version older than
                                           REV_20080715 */
    int i;
    int nb_entry = hooks->get_builtins_count();
    extension_builtins_t_pre_20080715* old_builtins;
    extension_builtins_t* new_builtins;
    
    old_builtins = (extension_builtins_t_pre_20080715*)hooks->get_builtins();
    new_builtins = new extension_builtins_t[nb_entry];
    
    for (i=0; i<nb_entry; i++) {
      /* new extension_builtins_t has an extra field wn_table at the end */
      memcpy(&(new_builtins[i]), &(old_builtins[i]),
             sizeof(extension_builtins_t_pre_20080715));

      switch(old_builtins[i].type) {
      case 0: 
        new_builtins[i].type = DYN_INTRN_TOP; break;
      case 1: new_builtins[i].type = DYN_INTRN_PARTIAL_COMPOSE; break;
      case 2: new_builtins[i].type = DYN_INTRN_PARTIAL_EXTRACT; break;
      case 3: new_builtins[i].type = DYN_INTRN_COMPOSE; break;
      case 4: new_builtins[i].type = DYN_INTRN_CONVERT_TO_PIXEL; break;
      case 5: new_builtins[i].type = DYN_INTRN_CONVERT_FROM_PIXEL; break;
      default:
        FmtAssert((false),
                  ("unrecognized builtin type %d", old_builtins[i].type));
      }
      new_builtins[i].wn_table = NULL;
    } 
    overriden_builtins = new_builtins;
  } else { // no migration needed for newer extensions.
    overriden_builtins = hooks->get_builtins();
  }

  if ( hooks->magic < REV_20070924 ) { /* any version older than
                                            REV_20070924 */
      int i;
      int nb_entry = hooks->get_modes_count();
      extension_machine_types_t_pre_20070615 *old_tab;
      extension_machine_types_t              *new_tab;
      old_tab = (extension_machine_types_t_pre_20070615*)hooks->get_modes();
      new_tab = new extension_machine_types_t[nb_entry];

      for (i=0; i<nb_entry; i++)  {
        /* new extension_machine_types_t has an extra field
           'mpixelsize' at the end */
        memcpy(&(new_tab[i]), &(old_tab[i]), sizeof(extension_machine_types_t_pre_20070615));
        new_tab[i].mpixelsize = 0;
      }
      overriden_machine_types = new_tab;
  }
  else {
    // Extension library uses latest API.
    // No migration required.
    overriden_machine_types = hooks->get_modes();
  }
}

// Destructor
EXTENSION_HighLevel_Info::~EXTENSION_HighLevel_Info() {
  if (own_hooks) {
    delete hooks;
  }
  if ( hooks->magic < REV_20070924 ) {
    delete [] (extension_machine_types_t*) overriden_machine_types;
  }
  if ( hooks->magic < REV_20080715 ) {
    delete [] (extension_builtins_t*) overriden_builtins;
  }
}

