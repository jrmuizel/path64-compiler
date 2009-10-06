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
 * | 20090813 | - Introduce pattern recognition rule definitions inside        |
 * |          |   the extension dll.                                           |
 * +----------+----------------------------------------------------------------+
 * | 20090903 | - Support extension type size up to 2048 bits                  |
 * +----------+----------------------------------------------------------------+
 * | 20090908 | - Introduce extension options handling and add new flags field |
 * |          |   to wn_record structure.                                      |
 * +----------+----------------------------------------------------------------+
 *
 */
#include "../gccfe/extension_include.h"
#include "dyn_dll_api_access.h"

// ========================================================================
// List of compatible API revisions for high level part of the library
// description
// ========================================================================
#define    NB_SUPPORTED_HL_REV  7
#define    REV_20070131        (20070131)
#define    REV_20070615        (20070615)
#define    REV_20070924        (20070924)
#define    REV_20080715        (20080715)
#define    REV_20090410        (20090410)
#define    REV_20090813        (20090813)
#define    REV_20090903        (20090903)
#define    REV_20090908        (20090908)

static INT supported_HL_rev_tab[NB_SUPPORTED_HL_REV] = {
  REV_20070131,
  REV_20070615,
  REV_20070924,
  REV_20080715,
  REV_20090410,
  REV_20090813,
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
  machine_mode_t  mmode; 
  const char     *name; 
  enum mode_class mclass;
  unsigned short  mbitsize; 
  unsigned char   msize;
  unsigned char   munitsize;
  unsigned char   mwidermode; 
  machine_mode_t  innermode;
  TYPE_ID         mtype;
  unsigned short  alignment;
  int             local_REGISTER_CLASS_id;
  int             local_REGISTER_SUBCLASS_id;
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

struct wn_record_pre_20090908 {
  int wn_opc;               // whirl-node opcode
  int cycles;               // cost in cycles
  int size;                 // cost in size
  char*  asm_stmt_behavior; // asm statement code
  char** asm_stmt_results;  // asm statement "results": out, in/out and tmp
  char** asm_stmt_operands; // asm statement "operands": in
};
typedef struct wn_record_pre_20090908 wn_record_t_pre_20090908;


typedef struct
{
  machine_mode_t   mmode; 
  const char      *name; 
  enum mode_class  mclass;
  unsigned short   mbitsize; 
  unsigned char    msize;
  unsigned char    munitsize;
  unsigned char    mwidermode; 
  machine_mode_t   innermode;
  TYPE_ID          mtype;
  unsigned short   alignment;
  int              local_REGISTER_CLASS_id;
  int              local_REGISTER_SUBCLASS_id;
  unsigned char    mpixelsize;
} extension_machine_types_t_pre_20090813;



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
  
  if ( hooks->magic < REV_20090908 ) { /* any version older than REV_20090908 */
    overriden_extoption_array = NULL; 
    overriden_extoption_count = 0;

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
          overriden_extoption_array = mpx_extoption_array; 
          overriden_extoption_count = mpx_extoption_count;
        }
      }
    }
#endif
    

    if (hooks->magic>REV_20080715) {
      int i;
      int nb_entry = hooks->get_builtins_count();
      extension_builtins_t* builtins;
      builtins = (extension_builtins_t*) hooks->get_builtins();

      for (i=0; i<nb_entry; i++) {
        wn_record_t_pre_20090908* old_record;
        old_record = (wn_record_t_pre_20090908*) builtins[i].wn_table;
        if (old_record!=NULL) {
          int nb_wn = 1;
          while (old_record[nb_wn-1].wn_opc != OPCODE_UNKNOWN) {
            nb_wn++;
          }
          wn_record_t* new_record = new wn_record_t[nb_wn];
          int j;
          for (j=0; j<nb_wn; j++) {
            memcpy(&new_record[j], &old_record[j], sizeof(wn_record_t_pre_20090908));
            new_record[j].flags = EXTOPT_none;
          }
          builtins[i].wn_table = new_record;
        } else {
          builtins[i].wn_table = NULL;
        }
      }
    }
  } else {
    overriden_extoption_array = hooks->get_extoption_array();
    overriden_extoption_count = hooks->get_extoption_count();
  }


  //
  // Conversion of PATTERN RECOGNITION RULES
  //
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

  //
  // Conversion of BUILTIN datatypes
  //
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

  //
  // Conversion of MTYPE datatypes
  //
  if ( hooks->magic < REV_20090903 ) {
    int i;
    int nb_entry = hooks->get_modes_count();
    extension_machine_types_t              *new_tab;
    new_tab = new extension_machine_types_t[nb_entry];

    if ( hooks->magic < REV_20070924 ) {
      extension_machine_types_t_pre_20070615 *old_tab;
      old_tab = (extension_machine_types_t_pre_20070615*)hooks->get_modes();
    
      for (i=0; i<nb_entry; i++)  {
        new_tab[i].mmode      = old_tab[i].mmode;
        new_tab[i].name       = old_tab[i].name;
        new_tab[i].mclass     = old_tab[i].mclass;
        new_tab[i].mbitsize   = old_tab[i].mbitsize;  // uchar to ushort
        new_tab[i].msize      = old_tab[i].msize;     // uchar to ushort
        new_tab[i].munitsize  = old_tab[i].munitsize;
        new_tab[i].mwidermode = old_tab[i].mwidermode;
        new_tab[i].innermode  = old_tab[i].innermode;
        new_tab[i].mtype      = old_tab[i].mtype;
        new_tab[i].alignment  = old_tab[i].alignment;
        new_tab[i].local_REGISTER_CLASS_id    = old_tab[i].local_REGISTER_CLASS_id;
        new_tab[i].local_REGISTER_SUBCLASS_id = old_tab[i].local_REGISTER_SUBCLASS_id;
        new_tab[i].mpixelsize = 0;                    // additional field
      }
    }
    else {
      extension_machine_types_t_pre_20090813 *old_tab;
      old_tab = (extension_machine_types_t_pre_20090813*)hooks->get_modes();
    
      for (i=0; i<nb_entry; i++)  {
        new_tab[i].mmode      = old_tab[i].mmode;
        new_tab[i].name       = old_tab[i].name;
        new_tab[i].mclass     = old_tab[i].mclass;
        new_tab[i].mbitsize   = old_tab[i].mbitsize;  // uchar to ushort
        new_tab[i].msize      = old_tab[i].msize;     // uchar to ushort
        new_tab[i].munitsize  = old_tab[i].munitsize;
        new_tab[i].mwidermode = old_tab[i].mwidermode;
        new_tab[i].innermode  = old_tab[i].innermode;
        new_tab[i].mtype      = old_tab[i].mtype;
        new_tab[i].alignment  = old_tab[i].alignment;
        new_tab[i].local_REGISTER_CLASS_id    = old_tab[i].local_REGISTER_CLASS_id;
        new_tab[i].local_REGISTER_SUBCLASS_id = old_tab[i].local_REGISTER_SUBCLASS_id;
        new_tab[i].mpixelsize = old_tab[i].mpixelsize;
      }
    }
    overriden_machine_types = new_tab;
  }
  else {
    // Extension library uses latest API.
    // No migration required.
    overriden_machine_types = hooks->get_modes();
  }

  // Initialization
  initialize();
}

// Destructor
EXTENSION_HighLevel_Info::~EXTENSION_HighLevel_Info() {
  if (own_hooks) {
    delete hooks;
  }
  if ( hooks->magic > REV_20080715 &&
       hooks->magic < REV_20090908 ) {
    int nb_entry = hooks->get_builtins_count();
    extension_builtins_t* builtins;
    builtins = (extension_builtins_t*) hooks->get_builtins();
    int i;
    for (i=0; i<nb_entry; i++) {
      if (builtins[i].wn_table!=NULL)
        delete [] builtins[i].wn_table;
    }


  }


  if ( hooks->magic < REV_20090903 ) {
    delete [] (extension_machine_types_t*) overriden_machine_types;
  }
  if ( hooks->magic < REV_20080715 ) {
    delete [] (extension_builtins_t*) overriden_builtins;
  }

}

/** 
 * Initialization of internal data structures
 * 
 */
void EXTENSION_HighLevel_Info::initialize(void) {
  if (overriden_extoption_count>0 &&
      overriden_extoption_array!=NULL) {
    int r;
    for (r=0; r<overriden_extoption_count; r++) {
      std::string opt(overriden_extoption_array[r]);
      
      extoption_map[opt]= 1<<r;
    }
  }
}
